//# StokesImageUtil.cc: Stokes Image Utilities
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$

#include <casa/aips.h>

#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/MatrixMath.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/Cube.h>
#include <casa/BasicMath/Math.h>
#include <casa/BasicSL/Complex.h>
#include <scimath/Mathematics/FFTServer.h>
#include <casa/BasicSL/Constants.h>
#include <casa/Utilities/Assert.h>
#include <lattices/Lattices/Lattice.h>
#include <lattices/LEL/LatticeExpr.h>
#include <lattices/Lattices/LatticeIterator.h>
#include <lattices/Lattices/LatticeStepper.h>
#include <lattices/LatticeMath/LatticeConvolver.h>
#include <scimath/Fitting/NonLinearFitLM.h>
#include <scimath/Functionals/Gaussian2D.h>
#include <scimath/Mathematics/Interpolate2D.h>
#include <casacore/casa/IO/ArrayIO.h>

#include <ms/MeasurementSets/MSColumns.h>
#include <ms/MeasurementSets/MSDopplerUtil.h>

#include <msvis/MSVis/StokesVector.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <images/Images/PagedImage.h>
#include <images/Images/SubImage.h>
#include <images/Images/ImageBeamSet.h>
#include <casa/OS/File.h>

#include <casa/Quanta/UnitMap.h>
#include <casa/Quanta/UnitVal.h>
#include <measures/Measures/Stokes.h>
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <coordinates/Coordinates/StokesCoordinate.h>
#include <coordinates/Coordinates/Projection.h>
#include <casa/Logging/LogSink.h>
#include <casa/Logging/LogMessage.h>

#include <synthesis/TransformMachines2/Utils.h>

#include <casa/iostream.h>
#include <ctime>

using namespace casacore;
namespace casa {



// <summary> 
// </summary>

// <reviewed reviewer="" date="" tests="tMEGI" demos="">

// <prerequisite>
// </prerequisite>
//
// <etymology>
// </etymology>
//
// <synopsis> 
// </synopsis> 
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// </motivation>
//
// <todo asof="">
// </todo>

// In-place convolve. image is 4 dimensional. RA and Dec first.
void StokesImageUtil::Convolve(ImageInterface<Float>& image,
			       ImageInterface<Float>& psf) {


  Double availablemem=Double(HostInfo::memoryFree())*1024.0;
  //Do an in memory if x-y complex can fit in remaining memory
  if(availablemem < Double(image.shape()(0)*image.shape()(1))*6.0*8.0 ){
    LatticeConvolver<Float> lattCon(psf);
    lattCon.linear(image);
  }
  else{
    
    Vector<Int> map;
    AlwaysAssert(StokesMap(map, image.coordinates()), AipsError);
    
    Vector<Int> psfmap;
    AlwaysAssert(StokesMap(psfmap, psf.coordinates()), AipsError);
    
    Int nx = image.shape()(map(0));
    Int ny = image.shape()(map(1));
    
    // Make FFT machine
    FFTServer<Float,Complex> fft(IPosition(2, nx, ny));
    
    // Get the image into a Matrix for Fourier transformation
    Array<Complex> xfr;
    Matrix<Complex> cft;
    
    LatticeStepper ls(image.shape(), IPosition(4, nx, ny, 1, 1),
		      IPosition(4, map(0), map(1), map(2), map(3)));
    LatticeIterator<Float> li(image, ls);
    
    // Get the PSF into a Matrix for Fourier transformation
    LatticeStepper psfls(psf.shape(), IPosition(4, nx, ny, 1, 1),
			 IPosition(4, map(0), map(1), map(2), map(3)));
    RO_LatticeIterator<Float> psfli(psf, psfls);
    psfli.reset();
    //fft.fft(xfr, psfli.matrixCursor());
    fft.fft0(xfr, psfli.matrixCursor());
    
    // Loop over all planes
    for (li.reset();!li.atEnd();li++) {
      //fft.fft(cft, li.matrixCursor());
      fft.fft0(cft,li.matrixCursor());
      cft *= xfr;
      // fft.fft(li.rwMatrixCursor(), cft);
      fft.fft0(li.rwMatrixCursor(),cft,false);
      fft.flip(li.rwMatrixCursor(), false, false);
    }
  } 
};

void StokesImageUtil::Convolve(ImageInterface<Float>& image, 
				 ImageBeamSet& beams, Bool normalizeVolume){

  Int nbeams=beams.shape()(0);
  
  Int freqAx=CoordinateUtil::findSpectralAxis(image.coordinates());
  Int nchan=image.shape()(freqAx);
  if((nchan != nbeams) || (nchan==1)){
    GaussianBeam elbeam=beams(0, 0);
    Convolve(image, elbeam, normalizeVolume);     
  }
  else{
    IPosition blc=image.shape();
    blc=0;
    IPosition trc=image.shape()-1;
    for (Int k=0; k < nchan; ++k){
      blc[freqAx]=k;
      trc[freqAx]=k;
      Slicer slc(blc, trc, Slicer::endIsLast);
      SubImage<Float> subim(image, slc, true);
      GaussianBeam elbeam=beams(k,0);
      Convolve(subim, elbeam, normalizeVolume);

    }
  }
}

void StokesImageUtil::Convolve(ImageInterface<Float>& image,
			       GaussianBeam& beam,
			       Bool normalizeVolume)
{
  Convolve(image, beam.getMajor().get("arcsec").getValue(),
	   beam.getMinor().get("arcsec").getValue(),
	   beam.getPA().get("deg").getValue(),
	   normalizeVolume);
}

void StokesImageUtil::Convolve(ImageInterface<Float>& image, Float bmaj,
			       Float bmin, Float bpa,
			       Bool normalizeVolume) {
  
  Vector<Float> beam(3);
  beam(0)=bmaj*C::arcsec;
  beam(1)=bmin*C::arcsec;
  beam(2)=(bpa+90.0)*C::degree;
  PagedImage<Float>
    cleanpsf(IPosition(4, image.shape()(0), image.shape()(1), 1, 1),
	     image.coordinates(),
	     File::newUniqueName("./","imagesolver::cleanpsf").originalName());
  cleanpsf.table().markForDelete();
  MakeGaussianPSF(cleanpsf, beam, normalizeVolume);
  Convolve(image, cleanpsf);
}

void StokesImageUtil::MakeGaussianPSF(ImageInterface<Float>& psf,
				      Quantity& bmaj, Quantity& bmin,
				      Quantity& bpa,
				      Bool normalizeVolume) 
{
  Vector<Float> beam(3);
  beam(0)=bmaj.get("arcsec").getValue();
  beam(1)=bmin.get("arcsec").getValue();
  beam(2)=bpa.get("deg").getValue();
  MakeGaussianPSF(psf, beam, normalizeVolume);
}

// Make an image into a Gaussian PSF
void StokesImageUtil::MakeGaussianPSF(ImageInterface<Float>& psf, Vector<Float>& beam,
				      Bool normalizeVolume) {
  
  Int nx=psf.shape()(0);
  Int ny=psf.shape()(1);
  Matrix<Float> ipsf(nx, ny);
  
  Double cospa=cos(beam(2));
  Double sinpa=sin(beam(2));
  Vector<Double> rp=psf.coordinates().referencePixel();
  Vector<Double> d=psf.coordinates().increment();
  Double refi=rp(0);
  Double refj=rp(1);
  AlwaysAssert(beam(0)>0.0,AipsError);
  AlwaysAssert(beam(1)>0.0,AipsError);
  Double sbmaj, sbmin;
  
  // ---old
  // Assumes that the cell sizes are the same in both directions!
  //sbmaj=4.0*log(2.0)*square(d(0)/beam(0));
  //sbmin=4.0*log(2.0)*square(d(0)/beam(1));
  // ---old

  // Evaluate the Gaussian beam using real-world coordinates, to handle rectangular pixels (cas-7171)
  sbmaj=4.0*log(2.0)*square(1.0/beam(0));
  sbmin=4.0*log(2.0)*square(1.0/beam(1));

  Vector<Double>fd(fabs(d));

  Float volume=0.0;
  for (Int j=0;j<ny;j++) {
    for (Int i=0;i<nx;i++) {
      //   ---old
      //      Double x =   cospa * (Double(i)-refi) + sinpa * (Double(j)-refj);
      //      Double y = - sinpa * (Double(i)-refi) + cospa * (Double(j)-refj);
      //   ---old
      Double x =   cospa * (Double(i)-refi)*fd(0) + sinpa * (Double(j)-refj)*fd(1);
      Double y = - sinpa * (Double(i)-refi)*fd(0) + cospa * (Double(j)-refj)*fd(1);
      Double radius = sbmaj*square(x) + sbmin*square(y);
      if (radius<20.) {
	ipsf(i,j) = exp(-radius);
        volume+=ipsf(i,j);
      }
      else {
	ipsf(i,j)=0.;
      }
    }
  }
  if(normalizeVolume) ipsf/=volume;
  psf.putSlice(ipsf, IPosition(psf.ndim(),0), IPosition(psf.ndim(),1));
  
}

Float  StokesImageUtil::normalizePSF(casacore::ImageInterface<casacore::Float>& psf){
  AlwaysAssert(psf.ndim()==4,AipsError);
  
  Float retval=-1e10;
  Vector<Int> map;
  AlwaysAssert(StokesMap(map, psf.coordinates()), AipsError);
  Int nx = psf.shape()(map(0));
  Int ny = psf.shape()(map(1));
  if(nx < 32 || ny < 32)
    throw(AipsError("Will not normalize images less than 32 pixels"));
  Int npol = psf.shape()(map(2));
  Int nchan = psf.shape()(map(3));
  IPosition blc(4);
  IPosition trc(4);
  IPosition blc8(4,0,0,0,0);
  IPosition trc8(4,0,0,0,0);
  blc(map(0))=0; blc(map(1))=0;
  trc(map(0))=nx-1; trc(map(1))=ny-1;
  blc8(map(0))=nx/2-nx/16; blc8(map(1))=ny/2-ny/16;
  trc8(map(0))=nx/2+nx/16; trc8(map(1))=ny/2+ny/16;
  Slicer inner8(blc8, trc8,  Slicer::endIsLast);
  for (Int k=0; k < nchan ; ++k){
    blc(map(3))=k;
    trc(map(3))=k;
    for (Int j=0; j < npol; ++j){
      blc(map(2))=j;
      trc(map(2))=j;
      Slicer sl(blc, trc, Slicer::endIsLast);
      SubImage<Float> psfSub(psf, sl, True);
      Float maxCenter=max(psfSub.getSlice(inner8));
      if(maxCenter > retval)
	retval=maxCenter;
      if(maxCenter !=0){
	psfSub.copyData( (LatticeExpr<Float>)(psfSub/maxCenter));
      }
      else{
	psfSub.set(0.0);
      }
    }
  }
  return retval;
}


// Zero specified elements of a Stokes image
void StokesImageUtil::Zero(ImageInterface<Float>& image, Vector<Bool>& mask) {
  
  AlwaysAssert(image.ndim()==4,AipsError);
  AlwaysAssert(mask.shape()(0)==4,AipsError);
  
  Vector<Int> map;
  AlwaysAssert(StokesMap(map, image.coordinates()), AipsError);
  
  Int nx = image.shape()(map(0));
  Int ny = image.shape()(map(1));
  Int npol = image.shape()(map(2));
  Int nchan = image.shape()(map(3));
  
  LatticeStepper ls(image.shape(), IPosition(4, nx, 1, 1, 1),
		    IPosition(4, map(0), map(1), map(2), map(3)));
  LatticeIterator<Float> li(image, ls);
  
  // Loop over all planes
  li.reset();
  for (Int chan=0;chan<nchan;chan++) {
    for(Int pol=0;pol<npol;pol++) {
      for(Int iy=0;iy<ny;iy++,li++) {
	if(mask(pol)) {
	  li.woVectorCursor()=0.0;
	}
      }
    }
  }
  
}

void StokesImageUtil::MaskFrom(ImageInterface<Float>& mask,
			       ImageInterface<Float>& image,
			       const Quantity& threshold) 
{
  MaskFrom(mask, image, threshold.get("Jy").getValue());
}

void StokesImageUtil::MaskFrom(ImageInterface<Float>& mask,
			       ImageInterface<Float>& image,
			       const Double thres) 
{
  AlwaysAssert(image.ndim()==4,AipsError);
  AlwaysAssert(mask.shape()==image.shape(),AipsError);
  
  Vector<Int> map;
  AlwaysAssert(StokesMap(map, image.coordinates()), AipsError);
  
  Int nx = image.shape()(map(0));
  Int npol = image.shape()(map(2));
  
  LatticeStepper ls(image.shape(), IPosition(4, nx, 1, npol, 1),
		    IPosition(4, map(0), map(1), map(2), map(3)));
  RO_LatticeIterator<Float> li(image, ls);
  LatticeIterator<Float> mli(mask, ls);
  
  // Loop over all planes
  for (li.reset(),mli.reset();!li.atEnd()&&!mli.atEnd();li++,mli++) {
    if(npol>1) {
      mli.rwMatrixCursor()=0.0;
      for (Int ix=0;ix<nx;ix++) {
	if(li.matrixCursor()(ix,0)>thres) {
	  for (Int pol=0;pol<npol;pol++) mli.rwMatrixCursor()(ix,pol)=1.0;
	}
      }
    }
    else {
      mli.rwVectorCursor()=0.0;
      for (Int ix=0;ix<nx;ix++) {
	if(li.vectorCursor()(ix)>thres) {
	  mli.rwVectorCursor()(ix)=1.0;
	}
      }
    }
  }
}

void StokesImageUtil::MaskOnStokesI(ImageInterface<Float>& image,
				    const Quantity& threshold) 
{
  
  Double thres=threshold.get("Jy").getValue();
  
  AlwaysAssert(image.ndim()==4,AipsError);
  
  Vector<Int> map;
  AlwaysAssert(StokesMap(map, image.coordinates()), AipsError);
  
  Int nx = image.shape()(map(0));
  Int npol = image.shape()(map(2));
  
  LatticeStepper ls(image.shape(), IPosition(4, nx, 1, npol, 1),
		    IPosition(4, map(0), map(1), map(2), map(3)));
  LatticeIterator<Float> li(image, ls);
  
  // Loop over all planes
  for (li.reset();!li.atEnd();li++) {
    for (Int ix=0;ix<nx;ix++) {
      if(npol>1) {
	if(li.matrixCursor()(ix,0)<thres) {
	  for (Int pol=0;pol<npol;pol++) li.rwMatrixCursor()(ix,pol)=0.0;
	}
      }
      else {
	if(li.vectorCursor()(ix)<thres) {
	  li.rwVectorCursor()(ix)=0.0;
	}
      }
    }
  }
  
}

// Constrain (a) Stokes I to be > 0 and (b) I > abs(V) and
// (c) fractional pol<1.0
void StokesImageUtil::Constrain(ImageInterface<Float>& image) {
  
  AlwaysAssert(image.ndim()==4,AipsError);
  
  Vector<Int> map;
  AlwaysAssert(StokesMap(map, image.coordinates()), AipsError);
  
  Int nx = image.shape()(map(0));
  Int npol = image.shape()(map(2));
  
  LatticeStepper ls(image.shape(), IPosition(4, nx, 1, npol, 1),
		    IPosition(4, map(0), map(2), map(1), map(3)));
  LatticeIterator<Float> li(image, ls);
  
  // Loop over all planes
  for (li.reset();!li.atEnd();li++) {
    
    // Make a copy of the cursor
    Matrix<Float>& limage=li.rwMatrixCursor();
    
    for (Int ix=0;ix<nx;ix++) {
      // I >=0
      Bool zero=false;
      if(limage(ix,0)<0.0) {
	zero=true;
      }
      // I >= abs(V)
      else if (npol==2) {
	if(limage(ix,0)<abs(limage(ix,1))) zero=true;
      }
      // fractional pol < 1.0
      else if (npol==4) {
	StokesVector sv(limage(ix,0),limage(ix,1),
			limage(ix,2),limage(ix,3));
	if(sv.minEigenValue()<0.0) zero=true;
      }
      if(zero) for (Int pol=0;pol<npol;pol++) limage(ix,pol)=0.0;
    }
  }
}

//See CAS-13022 for FitGaussianPSF algorithm details
Bool StokesImageUtil::FitGaussianPSF(ImageInterface<Float>& psf, ImageBeamSet& elbeam, Float psfcutoff){
  
  Bool retval=true;
  Int freqAx=CoordinateUtil::findSpectralAxis(psf.coordinates());
  Vector<Stokes::StokesTypes> whichPols;
  Int polAx=CoordinateUtil::findStokesAxis(whichPols, psf.coordinates());
  Int nchan=psf.shape()(freqAx);
  IPosition blc=psf.shape();
  blc=0;
  IPosition trc=psf.shape()-1;
  trc[polAx]=0;
  elbeam=ImageBeamSet(nchan, 1);
  IPosition ipos(2,0,0);
  Matrix<GaussianBeam> tempBeam(nchan,1);
  Vector<Bool> fitted(nchan, false);

  for (Int k=0; k < nchan;++k){ 
    blc[freqAx]=k;
    trc[freqAx]=k;
    Slicer slc(blc, trc, Slicer::endIsLast);
    SubImage<Float> subpsf(psf, slc, false);
    try{
      fitted(k)=FitGaussianPSF(subpsf, tempBeam(k,0), psfcutoff);
    }
    catch (AipsError x_error){
      Int ik=k;
      fitted(k)=false;
      while ((ik > 0) && !fitted(k)){
	if(fitted(ik-1)){
	  fitted(k)=true;
	  tempBeam(k,0)=tempBeam(ik-1, 0);
	}
	--ik;
      }
    } 
    ipos(0)=k;
  }
  Float maxMaj=0.0;
  Float minMaj=C::flt_max;
  Int posMax=0;
  DirectionCoordinate directionCoord=psf.coordinates().directionCoordinate(psf.coordinates().findCoordinate(Coordinate::DIRECTION));
  String dirunit=directionCoord.worldAxisUnits()(0);
  for(Int k=0; k < nchan; ++k){
	  if(fitted(k) && (maxMaj < tempBeam(k,0).getMajor(dirunit))){
		  maxMaj=tempBeam(k,0).getMajor(dirunit);
		  posMax=k;
	  }
	  if(fitted(k) && (minMaj > tempBeam(k,0).getMajor(dirunit))){
	  		  minMaj=tempBeam(k,0).getMajor(dirunit);
	  		  posMax=k;
	  }
  }
  //If the beams are within 0.5 pixel the same resolution then
  //who cares to have a beam per plane
  if(abs(minMaj-maxMaj) < 0.5*abs(directionCoord.increment()(0))){
	  GaussianBeam theBeam=tempBeam(posMax,0);
	  tempBeam.resize(1,1);
	  tempBeam(0,0)=theBeam;
	  fitted.resize(1);
	  fitted=true;
  }
  if(!anyTrue(fitted)){
      retval=false;
      throw(AipsError("No valid fits were found to PSF"));
  }
  if(!allTrue(fitted)){
     for (Int k=0; k < nchan;++k){ 
       int ik=k;
       while((ik < (nchan-1)) && !fitted(k)){
	 if(fitted(ik+1)){
	   fitted(k)=true;
	   tempBeam(k,0)=tempBeam(ik+1, 0);
	 }
	 ++ik;
       }
     }
  }
  elbeam=ImageBeamSet(tempBeam.shape());
  elbeam.setBeams(tempBeam);
  
  return retval;
}

Bool StokesImageUtil::FitGaussianPSF(ImageInterface<Float>& psf, 
				     GaussianBeam& beam, Float psfcutoff)
{
	Vector<Float> vbeam(3, 0.0);
  Bool status=true;
  if(!FitGaussianPSF(psf, vbeam, psfcutoff)) status=false;
  beam = GaussianBeam(
		  Quantity(abs(vbeam[0]),"arcsec"),
		  Quantity(abs(vbeam[1]),"arcsec"),
		  Quantity(vbeam[2],"deg")
	);
  return status;

}


void StokesImageUtil::FindNpoints(Int& npoints, IPosition& blc,  IPosition& trc, Int nrow, Float amin, Int px, Int py, Vector<Double>& deltas, Matrix<Double>& x , Vector<Double>& y, Vector<Double>& sigma,  Matrix<Float>& lpsf){
    
    Int maxnpoints=(2*nrow+1)*(2*nrow+1);
    Matrix<Double> ix(maxnpoints,2);
    Vector<Double> iy(maxnpoints), isigma(maxnpoints);
    ix=0.0; iy=0.0; isigma=0.0;
    npoints = 0;
    
    //IPosition blc(2), trc(2);
//blc = 0; trc = 0;
    
    blc(0) = lpsf.shape()(0)-1;
    blc(1) = lpsf.shape()(1)-1;
    trc(0) = 0;
    trc(1) = 0;
    
    IPosition psf_shape = lpsf.shape();

    //we sample the central part of a, 2*nrow+1 by 2*nrow+1
    
    Int iflip = 1;
    Int jflip = 1;
    // loop through rows. Include both above and below in case
    // we are fitting an image feature
    
    for(Int jlo = 0;jlo<2;jlo++) {
      jflip*=-1;
      // loop from 0 to nrow then from 1 to nrow
      for(Int j = jlo;j<=nrow;j++) {
        // the current row under consideration
        Int jrow = py + j*jflip;
        // loop down row doing alternate halves. work our way
        // out from the center until we cross threshold
        // don't include any sidelobes!
        for(Int ilo=0;ilo<2;ilo++) {
      iflip*=-1;
      // start at center row this may or may not be in the lobe,
      // if it's narrow and the pa is near 45 degrees
      
      
      if((jrow > (psf_shape(1)-1)) || (jrow < 0 ) ) break;
      //cout << "11.** " << maxnpoints << ",*,"<< npoints << ",*," << px << ",*," << jrow << endl;
      Bool inlobe = lpsf(px,jrow)>amin;
    
      //cout << "12.** " << maxnpoints << ",*,"<< npoints << ",*," << px << ",*," << jrow << endl;
      for(Int i = ilo;i<=nrow;i++) {
        if(npoints < maxnpoints){
          Int irow = px + i*iflip;
          // did we step out of the lobe?
         
          if((irow > (psf_shape(0)-1)) || (irow < 0 ) ) break;
          //cout << "irow , jrow " << irow  << ",*," << jrow  << ",*," << lpsf.shape() << ",*," << px << ",*," << py << endl;
          if (inlobe&&(lpsf(irow,jrow)<amin)) break;
          if (lpsf(irow,jrow)>amin) {
            //cout << "$%$irow , jrow " << lpsf.shape() << ",*,"<< irow << "," << jrow << ",*, "<< irow - px << ",*," << jrow - py << ",*, " << lpsf(irow,jrow) << ",*, " << inlobe << ",*, " << npoints <<  endl;
            inlobe = true;
            // the sign on the ra can cause problems.  we just fit
            // for what the beam "looks" like here, and worry about
            // it later.
            ix(npoints,0) = (irow-px)*abs(deltas(0));
            ix(npoints,1) = (jrow-py)*abs(deltas(1));
            iy(npoints) = lpsf(irow,jrow);
              //cout << "1.**" << endl;
            
              if(blc(0) > irow) blc(0) = irow;
              if(blc(1) > jrow) blc(1) = jrow;
              
              if(trc(0) < irow) trc(0) = irow;
              if(trc(1) < jrow) trc(1) = jrow;
              //cout << "2.**" << endl;
  
            isigma(npoints) = 1.0;
            ++npoints;
             // cout << "3.** " << maxnpoints << ",*,"<< npoints << endl;
            if(npoints > (maxnpoints-1)) {
          inlobe=false;
              //cout << "Over max points .** " << maxnpoints << ",*,"<< npoints << endl;
          //break;
                goto endSearch;
            }
             // cout << "5.** " << maxnpoints << ",*,"<< npoints << endl;
          }
        }
      }
        }
      }
    }
    
    endSearch:
    
    //cout << "2. blc, trc " << blc << ",*," << trc << endl;
    //Vector<Double> y(npoints), sigma(npoints);
    //Matrix<Double> x(npoints,2);
    //cout << "hallo " << endl;
    
    y.resize(npoints);
    x.resize(npoints,2);
    sigma.resize(npoints);
    
    //cout << "2. findNPoints " << npoints << " " << amin << endl;
    
    for (Int ip=0;ip<npoints;ip++) {
      x(ip,0)=ix(ip,0); x(ip,1)=ix(ip,1);
      y(ip)=iy(ip);
      sigma(ip)=isigma(ip);
      if(!(isigma(ip)>0.0)) break;
    }
    
    //Ensure that it is square
    if(blc(0) > blc(1)){
        blc(0) = blc(1);
    }else{
        blc(1) = blc(0);
    }
    
    if(trc(0) > trc(1)){
        trc(1) = trc(0);
    }else{
        trc(0) = trc(1);
    }
    
    
}

void StokesImageUtil::ResamplePSF(Matrix<Float>& psf, Int& oversampling, Matrix<Float>& resampledPsf, String& InterpMethod)
{
    Int nx = psf.shape()(0);
    Int ny = psf.shape()(1);
    Int nxRe = nx*oversampling - oversampling + 1;
    Int nyRe = ny*oversampling - oversampling + 1;
    
    Vector<Double> pos(2);
    resampledPsf.resize(nxRe,nyRe);
    
    Interpolate2D::Method method;
    if(InterpMethod == "NEAREST") method = Interpolate2D::NEAREST;
    if(InterpMethod == "LINEAR") method = Interpolate2D::LINEAR;
    if(InterpMethod == "CUBIC") method = Interpolate2D::CUBIC;
    if(InterpMethod == "LANCZOS") method = Interpolate2D::LANCZOS;
    
    Interpolate2D resampleInterp(method);
//Interpolate2D resampleInterp(Interpolate2D::CUBIC);
    
    Bool ok;
    Float result;
    
    for (Int i=0; i < nxRe ; ++i){
        for (Int j=0; j < nyRe ; ++j){
            pos(0) = (Float) i/(Float) oversampling;
            pos(1) = (Float) j/(Float) oversampling;
            
            ok = resampleInterp.interp(result, pos, psf);
//cout << "pos is" << pos << " result " << result << " psf " << psf(i,j)<< endl;
            resampledPsf(i,j) = result;
        }
    }
    
    
}


Bool StokesImageUtil::FitGaussianPSF(ImageInterface<Float>& psf, Vector<Float>& beam, Float psfcutoff) {
  
  LogIO os(LogOrigin("StokesImageUtil", "FitGaussianPSF()",WHERE));
  os << LogIO::DEBUG1 << "Psfcutoff is  " << psfcutoff << LogIO::POST;
    
  Float npix = 20;
  Int expand_pixel = 5;
  Int target_npoints = 3001;
  String InterpMethod = "CUBIC";

  //##########################################################
  
  Vector<Double> deltas;
  deltas=psf.coordinates().increment();
  
  Vector<Int> map;
  AlwaysAssert(StokesMap(map, psf.coordinates()), AipsError);
  
  Int nx = psf.shape()(map(0));
  Int ny = psf.shape()(map(1));
  //// Int nchan = psf.shape()(map(3)); // apparently not used and no side effects
  Int px=0;
  Int py=0;
  Float bamp=0;
  Matrix<Float> lpsf;
  locatePeakPSF(psf, px, py, bamp, lpsf);

  //check if peak is ZERO
  if( bamp < 1e-07 ) {
    throw(AipsError("Psf peak is zero"));
  }

  //check if peak is outside inner quarter
  if(px < nx/4.0 || px > 3.0*nx/4.0 || py < ny/4.0 || py > 3.0*ny/4.0) {    
    throw(AipsError("Peak of psf is outside the inner quarter of defined image"));
  }

 
  if((bamp > 1.1) || (bamp < 0.9)) // No warning if 10% error in PSF peak
    os << LogIO::WARN << "Peak of PSF is " << bamp << LogIO::POST;


  static const Float fdiam = 2.5*abs(deltas(0))/C::arcsec;
try{  


  if(bamp==0.0) {
	  beam[0] = fdiam;
	  beam[1] = fdiam;
	  beam[2] = 0.0;
    os << LogIO::WARN << "Could not find peak " << LogIO::POST;
    return false;
  }
    

  
  lpsf/=bamp; //Normalize

  // The selection code attempts to avoid including any sidelobes, even
  // if they exceed the threshold, by starting from the center column and
  // working out, exiting the loop when it crosses the threshold.  It
  // assumes that the first time it finds a "good" point starting from
  // the center and working out that it's in the main lobe.  Narrow,
  // sharply ringing beams inclined at 45 degrees will confuse it, but 
  // that's even more pathological than most VLBI beams.
  
  Float amin=psfcutoff;
  Int nrow=npix;
  
  //we sample the central part of a, 2*nrow+1 by 2*nrow+1
  Bool converg=false;
  Vector<Double> solution;
  Int kounter=0;
    
 while(amin > 0.009 && !converg && (kounter < 50)){
  kounter+=1;
  Int npoints=0;
  Vector<Double> y, sigma;
  Matrix<Double> x;
  IPosition blc(2), trc(2);
     
  FindNpoints(npoints, blc, trc, nrow, amin, px, py, deltas, x , y, sigma, lpsf);
  os << LogIO::DEBUG1 << "First FindNpoints is " << npoints << LogIO::POST;
     
  blc = blc-expand_pixel;
  trc = trc+expand_pixel;
     
  if(blc(0) < 0) blc(0)=0;
  if(blc(1) < 0) blc(1)=0;
  if(trc(0) >= lpsf.shape()(0)) trc(0)=lpsf.shape()(0)-1;
  if(trc(1) >= lpsf.shape()(1)) trc(1)=lpsf.shape()(1)-1;
     
  Matrix<Float> lpsfWindowed = lpsf(blc,trc);
  os << LogIO::DEBUG1 << "The windowed Psf shape is " << lpsfWindowed.shape() << LogIO::POST;
  Matrix<Float> resampledPsf;
     
  Int oversampling = (Int) sqrt(target_npoints/(lpsfWindowed.shape()(0)*lpsfWindowed.shape()(1)));
   if(oversampling == 0){
         oversampling = 1;
   }
   os << LogIO::DEBUG1 << "The oversampling is " << oversampling << LogIO::POST;
     
  ResamplePSF(lpsfWindowed, oversampling, resampledPsf,InterpMethod);
  os << LogIO::DEBUG1 << "The resampled windowed Psf shape is " << lpsfWindowed.shape() << LogIO::POST;
     
  Float minVal, maxVal;
  IPosition minPos(2);
  IPosition maxPos(2);
  minMax(minVal, maxVal, minPos, maxPos, resampledPsf);
  resampledPsf = resampledPsf/maxVal;
    
  Vector<Double> resampledDeltas = deltas/ (Double) oversampling;
     
  Int minLen;
  if((trc(0) - blc(0)) > (trc(1) - blc(1)) ){
    minLen = trc(1) - blc(1) + 1;
  }else{
    minLen = trc(0) - blc(0) + 1;
  }
     
  Int nrowRe = (Int) (oversampling*minLen - 1)/2;
  FindNpoints(npoints, blc, trc, nrowRe, amin,  maxPos(0), maxPos(1), resampledDeltas, x , y, sigma, resampledPsf);
  os << LogIO::DEBUG1 << "Second FindNpoints is " << npoints << LogIO::POST;

  Gaussian2D<AutoDiff<Double> > gauss2d;
  gauss2d[0] = 1.0; //Height of Gaussian
  gauss2d[1] = 0.0; //The center of the Gaussian in the x direction
  gauss2d[2] = 0.0; //The center of the Gaussian in the y direction.
  //gauss2d[3] = 2.5*abs(resampledDeltas(0)); //The width (FWHM) of the Gaussian on one axis.
  gauss2d[3] = 2.5*abs(deltas(0));
  gauss2d[4] = 0.5; //A modified axial ratio.
  
  // Fix height and center
  gauss2d.mask(0) = false;
  gauss2d.mask(1) = false;
  gauss2d.mask(2) = false;
  
  // CAS-13515: Fitting sometimes fails with "NonLinearFitLM: error in loop solution". This occurs in casacore/scimath/Fitting/NonLinearFitLM.tcc LSQFit::invertRect() due to a matrix that can not be inverted.
  // This problem is solved by retrying fit with a different position angle.
  Bool loopSolutionFound=false;
  Int retryCounter = 0;
  Double posAng = 1.0; //Initial position angle.
  Int nRetries = 10;
  while(!loopSolutionFound && retryCounter < nRetries){
      //Create casacore fitter and fit Gaussian to subset of PSF interpolated values.
      gauss2d[5] = posAng; //The position angle.
      NonLinearFitLM<Double> fitter;
      // Set maximum number of iterations to 1000
      fitter.setMaxIter(1000);
      
      // Set converge criteria.  Default is 0.001
      fitter.setCriteria(0.0001);
      
      // Set the function and initial values
      fitter.setFunction(gauss2d);
      
      try{
          // The current parameter values are used as the initial guess.
          solution = fitter.fit(x, y, sigma);
          loopSolutionFound = true;
      }catch(AipsError x_error){
          loopSolutionFound = false;
          retryCounter++;
          posAng = 1.0 + retryCounter*M_PI/nRetries;
          os << LogIO::NORMAL3 << "Fit failed, another atempt will be made with position angle  " << posAng  << " rad." << LogIO::POST;
      }
      converg=fitter.converged();
  }
     
  if(!loopSolutionFound)
  {
    throw(AipsError(String("Error in StokesImageUtil::FitGaussianPSF: error in loop solution.")));
  }

  if (!converg) {
    beam(0)=2.5*abs(deltas(0))/C::arcsec;
    beam(1)=2.5*abs(deltas(0))/C::arcsec;
    beam(2)=0.0;

    //fit did not coverge...reduce the minimum i.e expand the field a bit
    amin/=1.5;
    
    os << LogIO::WARN << "The fit did not coverge; another atempt will be made with psfcutoff " << amin << LogIO::POST;
  }
  
 }
 if (converg) {
   if (abs(solution(4))>1.0) {
     beam(0)=abs(solution(3)*solution(4))/C::arcsec;
     beam(1)=abs(solution(3))/C::arcsec;
     beam(2)=solution(5)/C::degree-90.0;
   } else {
     beam(0)=abs(solution(3))/C::arcsec;
     beam(1)=abs(solution(3)*solution(4))/C::arcsec;
     beam(2)=solution(5)/C::degree;
   }
   
   beam(2)=fmod(beam(2), Float(360.0));
   //while (abs(beam(2)/180.0)> 1) {
   //  if (beam(2) > 180.0) beam(2)-=360.0;
   //  else beam(2)+=360.0;
   //}
   //CAS-8627: adjust pa to the range -90d, +90d
   while (abs(beam(2)/90.0)> 1) {
     if (beam(2) > 270.0) beam(2)-=360.0;
     else if (beam(2) > 90.0) beam(2) -=180.0;
     else if (beam(2) < -270.0) beam(2) +=360.0;
     else beam(2)+=180.0;
   }
     
   return true;
 }
 else os << LogIO::WARN << "The fit did not coverge; check your PSF" <<
	LogIO::POST; 
 return false;

 } catch (AipsError x_error) {
	 beam[0] = fdiam;
	 beam[1] = fdiam;
	 beam[2] = 0.0;
    os << LogIO::SEVERE << "Caught Exception: "<< x_error.getMesg() <<
      LogIO::POST;
    return false;
   } 

}

void StokesImageUtil::locatePeakPSF(ImageInterface<Float>& in, Int& xpos, Int& ypos, Float& amp, Matrix<Float>& lpsf){
  Vector<Int> map;
  AlwaysAssert(StokesMap(map, in.coordinates()), AipsError);
  
  Int nx = in.shape()(map(0));
  Int ny = in.shape()(map(1));
  Int nchan = in.shape()(map(3));
  xpos=0;
  ypos=0;
  Float psfmax;

  amp=0;


  IPosition oneplane(in.ndim(), nx, ny, 1, 1);
  LatticeStepper psfls(in.shape(), oneplane,
		       IPosition(4,map(0),map(1),map(3),map(2)));
  RO_LatticeIterator<Float> psfli(in,psfls);
  while (nchan >= 1 && amp < 0.9){
    lpsf=psfli.matrixCursor();
    
    IPosition psfposmax(lpsf.ndim());
    IPosition psfposmin(lpsf.ndim());
    Float psfmin;
    
    minMax(psfmin, psfmax, psfposmin, psfposmax, lpsf); 
    
    xpos=psfposmax(0);
    ypos=psfposmax(1);
    


    amp=lpsf(xpos,ypos);

    ++psfli;
    --nchan;
  }

}

void StokesImageUtil::directCFromR(ImageInterface<Complex>& out, const ImageInterface<Float>& in) {
  AlwaysAssert(in.shape()(0)==out.shape()(0), AipsError);
  AlwaysAssert(in.shape()(1)==out.shape()(1), AipsError);
  AlwaysAssert(in.shape()(3)==out.shape()(3), AipsError);
  
  Vector<Int> inmap;
  AlwaysAssert(StokesMap(inmap, in.coordinates()), AipsError);
  Vector<Int> outmap;
  AlwaysAssert(StokesMap(outmap, out.coordinates()), AipsError);
  
  Int innpol = in.shape()(inmap(2));
  Int outnpol = out.shape()(outmap(2));
  if(innpol != outnpol){
    throw(AipsError("Cannot convert directly between images of different polarization shape"));
  }
  Vector<Int> inMap(innpol), outMap(outnpol); 
  StokesImageUtil::PolRep outPolFrame;
  Int nStokesOut=CStokesPolMap(outMap, outPolFrame, out.coordinates());
  StokesImageUtil::PolRep inPolFrame;
  Int nStokesIn=CStokesPolMap(inMap, inPolFrame, in.coordinates());
  AlwaysAssert(nStokesOut, AipsError);
  AlwaysAssert(nStokesIn, AipsError);

  if(inPolFrame != outPolFrame){
    throw(AipsError("Cannot convert directly between polarization types"));
  }
  IPosition inblc(4,0,0,0,0);
  IPosition intrc(4,0,0,0,0);
  intrc(inmap(0))=in.shape()(inmap(0))-1;
  intrc(inmap(1))=in.shape()(inmap(1))-1;
  intrc(inmap(2))=0;
  intrc(inmap(3))= in.shape()(inmap(3))-1;
  IPosition outblc(4,0,0,0,0);
  IPosition outtrc(4,0,0,0,0);
  outtrc(outmap(0))=in.shape()(outmap(0))-1;
  outtrc(outmap(1))=in.shape()(outmap(1))-1;
  outtrc(outmap(2))=0;
  outtrc(outmap(3))= in.shape()(outmap(3))-1;
  
  for (Int k=0; k < innpol ; ++k){
    inblc(inmap(2))=k;
    intrc(inmap(2))=k;
    Int outindex=-1;
    for ( Int j=0; j < innpol; ++j){
      if(inMap[k]==outMap[j])
	outindex=j;
    }
    if(outindex < 0){
      throw(AipsError("cannot match polarization in direct copy"));
    }
    outblc(outmap(2))=outindex;
    outtrc(outmap(2))=outindex;
    Slicer slin(inblc, intrc, Slicer::endIsLast);
    Slicer slout(outblc, outtrc, Slicer::endIsLast);
    SubImage<Complex> sliceout(out, slout, true);
    SubImage<Float> slicein(in, slin);
    sliceout.copyData(LatticeExpr<Complex>(toComplex(slicein)));
  }




}

void StokesImageUtil::directCToR(ImageInterface<Float>& out, ImageInterface<Complex>& in) {
  AlwaysAssert(in.shape()(0)==out.shape()(0), AipsError);
  AlwaysAssert(in.shape()(1)==out.shape()(1), AipsError);
  AlwaysAssert(in.shape()(3)==out.shape()(3), AipsError);
  
  Vector<Int> inmap;
  AlwaysAssert(StokesMap(inmap, in.coordinates()), AipsError);
  Vector<Int> outmap;
  AlwaysAssert(StokesMap(outmap, out.coordinates()), AipsError);
  
  Int innpol = in.shape()(inmap(2));
  Int outnpol = out.shape()(outmap(2));
  if(innpol != outnpol){
    throw(AipsError("Cannot convert directly between images of different polarization shape"));
  }
  Vector<Int> inMap(innpol), outMap(outnpol); 
  StokesImageUtil::PolRep outPolFrame;
  Int nStokesOut=CStokesPolMap(outMap, outPolFrame, out.coordinates());
  StokesImageUtil::PolRep inPolFrame;
  Int nStokesIn=CStokesPolMap(inMap, inPolFrame, in.coordinates());


  AlwaysAssert(nStokesOut, AipsError);
  AlwaysAssert(nStokesIn, AipsError);
  if(inPolFrame != outPolFrame){
    throw(AipsError("Cannot convert directly between polarization types"));
  }
  IPosition inblc(4,0,0,0,0);
  IPosition intrc(4,0,0,0,0);
  intrc(inmap(0))=in.shape()(inmap(0))-1;
  intrc(inmap(1))=in.shape()(inmap(1))-1;
  intrc(inmap(2))=0;
  intrc(inmap(3))= in.shape()(inmap(3))-1;
  IPosition outblc(4,0,0,0,0);
  IPosition outtrc(4,0,0,0,0);
  outtrc(outmap(0))=in.shape()(outmap(0))-1;
  outtrc(outmap(1))=in.shape()(outmap(1))-1;
  outtrc(outmap(2))=0;
  outtrc(outmap(3))= in.shape()(outmap(3))-1;

  for (Int k=0; k < innpol ; ++k){
    inblc(inmap(2))=k;
    intrc(inmap(2))=k;
    Int outindex=-1;
    for ( Int j=0; j < innpol; ++j){
      if(inMap[k]==outMap[j])
	outindex=j;
    }
    if(outindex < 0){
      throw(AipsError("cannot match polarization in direct copy"));
    }
    outblc(outmap(2))=outindex;
    outtrc(outmap(2))=outindex;
    Slicer slin(inblc, intrc, Slicer::endIsLast);
    Slicer slout(outblc, outtrc, Slicer::endIsLast);
    SubImage<Float> sliceout(out, slout, true);
    SubImage<Complex> slicein(in, slin);
    sliceout.copyData(LatticeExpr<Float>(real(slicein)));
  }
  
  
}

void StokesImageUtil::To(ImageInterface<Float>& out, ImageInterface<Complex>& in) {
  
  AlwaysAssert(in.shape()(0)==out.shape()(0), AipsError);
  AlwaysAssert(in.shape()(1)==out.shape()(1), AipsError);
  AlwaysAssert(in.shape()(3)==out.shape()(3), AipsError);
  
  Vector<Int> map;
  AlwaysAssert(StokesMap(map, in.coordinates()), AipsError);
  Vector<Int> outmap;
  AlwaysAssert(StokesMap(outmap, out.coordinates()), AipsError);

  Int nx = in.shape()(map(0));
  Int innpol = in.shape()(map(2));
  Int outnpol = out.shape()(outmap(2));
  
  // Loop over all planes
  LatticeStepper inls(in.shape(), IPosition(4, nx, 1, innpol, 1),
		      IPosition(4, map(0), map(2), map(1), map(3)));
  LatticeStepper outls(out.shape(), IPosition(4, nx, 1, outnpol, 1),
		       IPosition(4, map(0), map(2), map(1), map(3)));
  RO_LatticeIterator<Complex> inli(in,  inls);
  LatticeIterator<Float>  outli(out, outls);
  
  Vector<Int> inMap(innpol), outMap(outnpol);
  StokesImageUtil::PolRep polFrame=StokesImageUtil::CIRCULAR;
  Int nStokesIn=CStokesPolMap(inMap, polFrame, in.coordinates());
  Int nStokesOut=StokesPolMap(outMap, out.coordinates());
  
  if(nStokesOut <=0){
    directCToR(out, in);
    return;
  }
  AlwaysAssert(nStokesOut, AipsError);
  // Try taking real part only (uses LatticeExpr)
  if(nStokesIn==0) {
    nStokesIn=StokesPolMap(inMap, in.coordinates());
    if(nStokesIn==nStokesOut) {
      outli=LatticeIterator<Float>  (out, inls); 
      //replaced copyData with iteration as it seems to load the whole array in memory
       for (inli.reset(), outli.reset(); !inli.atEnd() && !outli.atEnd();
	 inli++,outli++) {
	 outli.woCursor()=real(inli.cursor());

       }
      

       // out.copyData(LatticeExpr<Float>(real(in)));
      return;
    }
    throw(AipsError("Illegal conversion in ToStokesImage"));
  }
  
  Vector<Complex> cs(4);
  cs=Complex(0.0);
  CStokesVector csv(0.0, 0.0, 0.0, 0.0);
  StokesVector sv(0.0, 0.0, 0.0, 0.0);
  Int pol;
  if(nStokesIn==1) {
    for (inli.reset(), outli.reset(); !inli.atEnd() && !outli.atEnd();
	 inli++,outli++) {
      for (Int ix=0;ix<nx;ix++) {
        cs(inMap(0))=inli.vectorCursor()(ix);
	csv=CStokesVector(cs(0), cs(1), cs(2), cs(3));
	if(polFrame==StokesImageUtil::LINEAR) {
	  applySlinInv(sv, csv);
	}
	else {
	  applyScircInv(sv, csv);
	}
        if(nStokesOut==1) {
	  outli.rwVectorCursor()(ix)=sv(outMap(0));
	}
	else {
	  for (pol=0;pol<outnpol;pol++)
	    outli.rwMatrixCursor()(ix,pol)=sv(outMap(pol));
	}
      }
    }
  }
  else {
    for (inli.reset(), outli.reset(); !inli.atEnd() && !outli.atEnd(); inli++,outli++) {
      for (Int ix=0;ix<nx;ix++) {
	cs=Complex(0.0);
        for (pol=0;pol<innpol;pol++) cs(inMap(pol))=inli.matrixCursor()(ix,pol);
	csv=CStokesVector(cs(0), cs(1), cs(2), cs(3));
	if(polFrame==StokesImageUtil::LINEAR) {
	  applySlinInv(sv, csv);
	}
	else {
	  applyScircInv(sv, csv);
	}
        if(nStokesOut==1) {
	  outli.rwVectorCursor()(ix)=sv(outMap(0));
	}
	else {
	  for (pol=0;pol<outnpol;pol++) {
	    outli.rwMatrixCursor()(ix,pol)=sv(outMap(pol));
	  }
	}
      }
    }
  }
};

void StokesImageUtil::ToStokesPSF(ImageInterface<Float>& out, ImageInterface<Complex>& in) {
  
  AlwaysAssert(in.shape()(0)==out.shape()(0), AipsError);
  AlwaysAssert(in.shape()(1)==out.shape()(1), AipsError);
  AlwaysAssert(in.shape()(3)==out.shape()(3), AipsError);
  
  Vector<Int> map;
  AlwaysAssert(StokesMap(map, in.coordinates()), AipsError);
  Vector<Int> outmap;
  AlwaysAssert(StokesMap(outmap, out.coordinates()), AipsError);
  
  Int nx = in.shape()(map(0));
  Int innpol = in.shape()(map(2));
  Int outnpol = out.shape()(outmap(2));
  
  // Loop over all planes
  LatticeStepper inls(in.shape(), IPosition(4, nx, 1, innpol, 1),
		      IPosition(4, map(0), map(2), map(1), map(3)));
  LatticeStepper outls(out.shape(), IPosition(4, nx, 1, outnpol, 1),
		       IPosition(4, map(0), map(2), map(1), map(3)));
  RO_LatticeIterator<Complex> inli(in,  inls);
  LatticeIterator<Float>  outli(out, outls);
  
  Vector<Int> inMap(innpol), outMap(outnpol);
  StokesImageUtil::PolRep polFrame=StokesImageUtil::CIRCULAR;
  Int nStokesIn=CStokesPolMap(inMap, polFrame, in.coordinates());
  Int nStokesOut=StokesPolMap(outMap, out.coordinates());
  if(nStokesOut <=0){
    directCToR(out, in);
    return;
  }
  AlwaysAssert(nStokesOut, AipsError);
  // Try taking real part only (uses LatticeExpr)
  if(nStokesIn==0) {
    nStokesIn=StokesPolMap(inMap, in.coordinates());
    if(nStokesIn==nStokesOut) {
      //replaced copyData with iteration as it seems to load the whole array in memory
      outli=LatticeIterator<Float>  (out, inls);
      for (inli.reset(), outli.reset(); !inli.atEnd() && !outli.atEnd();
	 inli++,outli++) {
	 outli.woCursor()=real(inli.cursor());

       }
      
      //out.copyData(LatticeExpr<Float>(real(in)));
      return;
    }
    throw(AipsError("Illegal conversion in ToStokesImage"));
  }

  /* STOKESDBG */  //cout << "ToStokesPSF - nstokesIn :  " <<  nStokesIn << " inmap : " << inMap << "  nstokesOut : " << nStokesOut << " outmap : " << outMap << endl;
  /* STOKESDBG */  //cout << "IN (data) stokesCoord : " << ( (in.coordinates()).stokesCoordinate( (in.coordinates()).findCoordinate(Coordinate::STOKES)  ) ).stokes() << "     OUT (image) stokesCoord : " << ( (out.coordinates()).stokesCoordinate( (out.coordinates()).findCoordinate(Coordinate::STOKES)  ) ).stokes() << endl;

  Vector<Int> inST =  ( (in.coordinates()).stokesCoordinate( (in.coordinates()).findCoordinate(Coordinate::STOKES)  ) ).stokes();
  Vector<Int> outST = ( (out.coordinates()).stokesCoordinate( (out.coordinates()).findCoordinate(Coordinate::STOKES)  ) ).stokes();

  Vector<Complex> cs(4);
  cs=Complex(0.0);
  CStokesVector csv(0.0, 0.0, 0.0, 0.0);
  StokesVector sv(0.0, 0.0, 0.0, 0.0);
  Int pol;
  if(nStokesIn==1) {
    for (inli.reset(), outli.reset(); !inli.atEnd() && !outli.atEnd();
	 inli++,outli++) {
      for (Int ix=0;ix<nx;ix++) {
        cs(inMap(0))=inli.vectorCursor()(ix);
	csv=CStokesVector(cs(0), cs(1), cs(2), cs(3));
	if(polFrame==StokesImageUtil::LINEAR) {
	  applySlinInv(sv, csv);
	}
	else {
	  applyScircInv(sv, csv);
	}
        // nstokesout will be 1, 2 or 4 only.
        if(nStokesOut==1) {
	  //outli.rwVectorCursor()(ix)=sv(0); //sv(outMap(0));
          // If outstokescoord = I or Q, use (I:0) : XX+YY for the PSF
          // If outstokescoord = U or V, use (U:2) : XY+YX  for the PSF
          if( polFrame==StokesImageUtil::LINEAR )
	    { 
	      if(Stokes::type(outST[0])==Stokes::I || Stokes::type(outST[0])==Stokes::Q ) 
	          outli.rwVectorCursor()(ix)=sv(0);
              if( Stokes::type(outST[0])==Stokes::U || Stokes::type(outST[0])==Stokes::V ) 
	          outli.rwVectorCursor()(ix)=sv(2);
	    }
          if( polFrame==StokesImageUtil::CIRCULAR )
	    { 
	      if(Stokes::type(outST[0])==Stokes::I || Stokes::type(outST[0])==Stokes::V ) 
	          outli.rwVectorCursor()(ix)=sv(0);
              if( Stokes::type(outST[0])==Stokes::Q || Stokes::type(outST[0])==Stokes::U ) 
	          outli.rwVectorCursor()(ix)=sv(1);
	    }
	}
        else if(nStokesOut==2) {
          // If outstokescoord = IQ, use (I:0) : XX+YY for the PSF in both planes
          // If outstokescoord = UV, use (U:2) : XY+YX  for the PSF in both planes
         if( polFrame==StokesImageUtil::LINEAR )
	    { 
	      if(Stokes::type(outST[0])==Stokes::I || Stokes::type(outST[0])==Stokes::Q || 
                 Stokes::type(outST[1])==Stokes::I || Stokes::type(outST[1])==Stokes::Q ) 
		{ outli.rwMatrixCursor()(ix,0)=sv(0);   outli.rwMatrixCursor()(ix,1)=sv(0); }
              if( Stokes::type(outST[0])==Stokes::U || Stokes::type(outST[0])==Stokes::V ||
                  Stokes::type(outST[1])==Stokes::U || Stokes::type(outST[1])==Stokes::V ) 
		{ outli.rwMatrixCursor()(ix,0)=sv(2); outli.rwMatrixCursor()(ix,1)=sv(2); }
	    }
          if( polFrame==StokesImageUtil::CIRCULAR )
	    { 
	      if(Stokes::type(outST[0])==Stokes::I || Stokes::type(outST[0])==Stokes::V ||
                 Stokes::type(outST[1])==Stokes::I || Stokes::type(outST[1])==Stokes::V ) 
		{ outli.rwMatrixCursor()(ix,0)=sv(0); outli.rwMatrixCursor()(ix,1)=sv(0); }
              if( Stokes::type(outST[0])==Stokes::Q || Stokes::type(outST[0])==Stokes::U ||
                  Stokes::type(outST[1])==Stokes::Q || Stokes::type(outST[1])==Stokes::U ) 
		{ outli.rwMatrixCursor()(ix,0)=sv(1); outli.rwMatrixCursor()(ix,1)=sv(1); }
	    }
	}
	else { // nstokesout = 4  : do a one-to-one mapping.
	  for (pol=0;pol<outnpol;pol++)
       	    outli.rwMatrixCursor()(ix,pol)=sv(outMap(pol));
	}
      }
    }
  }
  else {
    for (inli.reset(), outli.reset(); !inli.atEnd() && !outli.atEnd(); inli++,outli++) {
      for (Int ix=0;ix<nx;ix++) {
	cs=Complex(0.0);
        for (pol=0;pol<innpol;pol++) cs(inMap(pol))=inli.matrixCursor()(ix,pol);
	csv=CStokesVector(cs(0), cs(1), cs(2), cs(3));
	if(polFrame==StokesImageUtil::LINEAR) {
	  applySlinInv(sv, csv);
	}
	else {
	  applyScircInv(sv, csv);
	}
        if(nStokesOut==1) {
	  //outli.rwVectorCursor()(ix)=sv(0); //sv(outMap(0));
          // If outstokescoord = I or Q, use (I:0)
          // If outstokescoord = U or V, use (U:2)
          if( polFrame==StokesImageUtil::LINEAR )
	    { 
	      if(Stokes::type(outST[0])==Stokes::I || Stokes::type(outST[0])==Stokes::Q ) 
	          outli.rwVectorCursor()(ix)=sv(0);
              if( Stokes::type(outST[0])==Stokes::U || Stokes::type(outST[0])==Stokes::V ) 
	          outli.rwVectorCursor()(ix)=sv(2);
	    }
          if( polFrame==StokesImageUtil::CIRCULAR )
	    { 
	      if(Stokes::type(outST[0])==Stokes::I || Stokes::type(outST[0])==Stokes::V ) 
	          outli.rwVectorCursor()(ix)=sv(0);
              if( Stokes::type(outST[0])==Stokes::Q || Stokes::type(outST[0])==Stokes::U ) 
	          outli.rwVectorCursor()(ix)=sv(1);
	    }
	}
        else if(nStokesOut==2) {
          // If outstokescoord = IQ, use (I:0) : XX+YY for the PSF in both planes
          // If outstokescoord = UV, use (U:2) : XY+YX  for the PSF in both planes
         if( polFrame==StokesImageUtil::LINEAR )
	    { 
	      if(Stokes::type(outST[0])==Stokes::I || Stokes::type(outST[0])==Stokes::Q || 
                 Stokes::type(outST[1])==Stokes::I || Stokes::type(outST[1])==Stokes::Q ) 
		{ outli.rwMatrixCursor()(ix,0)=sv(0);   outli.rwMatrixCursor()(ix,1)=sv(0); }
              if( Stokes::type(outST[0])==Stokes::U || Stokes::type(outST[0])==Stokes::V ||
                  Stokes::type(outST[1])==Stokes::U || Stokes::type(outST[1])==Stokes::V ) 
		{ outli.rwMatrixCursor()(ix,0)=sv(2); outli.rwMatrixCursor()(ix,1)=sv(2); }
	    }
          if( polFrame==StokesImageUtil::CIRCULAR )
	    { 
	      if(Stokes::type(outST[0])==Stokes::I || Stokes::type(outST[0])==Stokes::V ||
                 Stokes::type(outST[1])==Stokes::I || Stokes::type(outST[1])==Stokes::V ) 
		{ outli.rwMatrixCursor()(ix,0)=sv(0); outli.rwMatrixCursor()(ix,1)=sv(0); }
              if( Stokes::type(outST[0])==Stokes::Q || Stokes::type(outST[0])==Stokes::U ||
                  Stokes::type(outST[1])==Stokes::Q || Stokes::type(outST[1])==Stokes::U ) 
		{ outli.rwMatrixCursor()(ix,0)=sv(1); outli.rwMatrixCursor()(ix,1)=sv(1); }
	    }
	}
	else { // nstokesout = 4
	  for (pol=0;pol<outnpol;pol++) {
	    outli.rwMatrixCursor()(ix,pol)=sv(outMap(0));
	  }
	}
      }
    }
  }
};

void StokesImageUtil::ToStokesSumWt(Matrix<Float> sumwtStokes, Matrix<Float> sumwtCorr, const CoordinateSystem& outcoord, const CoordinateSystem& incoord)
{

  AlwaysAssert( sumwtStokes.shape()[1] == sumwtCorr.shape()(1) , AipsError );
  Vector<Int> inmap;
  AlwaysAssert(StokesMap(inmap, incoord), AipsError);
  Vector<Int> outmap;
  AlwaysAssert(StokesMap(outmap, outcoord), AipsError);

  
  Int innpol = sumwtCorr.shape()[0];
  Int outnpol = sumwtStokes.shape()[0];
  
  
  Vector<Int> inMap(innpol), outMap(outnpol);
  StokesImageUtil::PolRep polFrame=StokesImageUtil::CIRCULAR;
  Int nStokesIn=CStokesPolMap(inMap, polFrame, incoord);
  Int nStokesOut=StokesPolMap(outMap, outcoord);
  if(nStokesOut <=0  || nStokesIn==0)  {
    if(innpol != outnpol)
      throw(AipsError("Cannot copy sumwt as input and output have different pol axis length"));
    convertArray(sumwtStokes, sumwtCorr);
    return;
  }
  if( nStokesOut==1 && nStokesIn==2){
    
    for(uInt chan=0;chan<sumwtStokes.shape()[1];chan++)
      {
        sumwtStokes(0,chan) = min(sumwtCorr(0,chan),sumwtCorr(1, chan));
      }     
  }
 if( nStokesOut==2 && nStokesIn==2){
    
    for(uInt chan=0;chan<sumwtStokes.shape()[1];chan++)
      {
        sumwtStokes(0,chan) = min(sumwtCorr(0,chan),sumwtCorr(1, chan));
        sumwtStokes(1,chan) = min(sumwtCorr(1,chan),sumwtCorr(1, chan));
      }     
  } 
 if( nStokesOut==4 && nStokesIn==4){
    
   for(uInt chan=0;chan<sumwtStokes.shape()[1];chan++)
     {
       if(polFrame==StokesImageUtil::LINEAR){
         sumwtStokes(0,chan) = min(sumwtCorr(0,chan),sumwtCorr(3, chan));
         sumwtStokes(1,chan) = min(sumwtCorr(0,chan),sumwtCorr(3, chan));
         sumwtStokes(2,chan) = min(sumwtCorr(1,chan),sumwtCorr(2, chan));
         sumwtStokes(3,chan) = min(sumwtCorr(1,chan),sumwtCorr(2, chan));
       }
       else{
         sumwtStokes(0,chan) = min(sumwtCorr(0,chan),sumwtCorr(3, chan));
         sumwtStokes(1,chan) = min(sumwtCorr(1,chan),sumwtCorr(2, chan));
         sumwtStokes(2,chan) = min(sumwtCorr(1,chan),sumwtCorr(2, chan));
         sumwtStokes(3,chan) = min(sumwtCorr(0,chan),sumwtCorr(3, chan));
       }
     } 
 }
  

}

void StokesImageUtil::ToStokesSumWt(Matrix<Float> sumwtStokes, Matrix<Float> sumwtCorr)
{
  AlwaysAssert( sumwtStokes.shape()[1] == sumwtCorr.shape()(1) , AipsError ); //same nchan
  AlwaysAssert( sumwtCorr.shape()[0] > 0, AipsError ); // at least one correlation gridded.

  /// Pick the value from the first correlation plane, and fill it into all stokes planes.
  /// This is valid when the same weights are used across correlations.
  for(uInt pol=0;pol<sumwtStokes.shape()[0];pol++)
    {
      for(uInt chan=0;chan<sumwtStokes.shape()[1];chan++)
	{
	  sumwtStokes(pol,chan) = sumwtCorr(0,chan);
	}
    }

}


#if 0
void StokesImageUtil::ToStokesPSF(ImageInterface<Float>& out, ImageInterface<Complex>& in) {
  
  AlwaysAssert(in.shape()(0)==out.shape()(0), AipsError);
  AlwaysAssert(in.shape()(1)==out.shape()(1), AipsError);
  AlwaysAssert(out.shape()(2)==out.shape()(3), AipsError);
  AlwaysAssert(in.shape()(3)==out.shape()(4), AipsError);
  
  Vector<Int> map;
  AlwaysAssert(StokesMap(map, in.coordinates()), AipsError);
  Vector<Int> outmap;
  AlwaysAssert(StokesMap(outmap, out.coordinates()), AipsError);
  
  Int nx = in.shape()(map(0));
  Int innpol = in.shape()(map(2));
  Int outnpol = out.shape()(outmap(2));
  
  // Loop over all planes. For the input we get a line of complex pixels
  // for the output we put a line of float matrices
  LatticeStepper inls(in.shape(), IPosition(4, nx, 1, innpol, 1),
		      IPosition(4, map(0), map(2), map(1), map(3)));
  LatticeStepper outls(out.shape(), IPosition(5, nx, 1, outnpol, outnpol, 1),
		       IPosition(5, map(0), map(3), map(1), map(2), 4));
  RO_LatticeIterator<Complex> inli(in,  inls);
  LatticeIterator<Float>  outli(out, outls);
  
  Vector<Int> inMap(innpol), outMap(outnpol);
  StokesImageUtil::PolRep polFrame;
  Int nStokesIn=CStokesPolMap(inMap, polFrame, in.coordinates());
  AlwaysAssert(nStokesIn, AipsError);
  Int nStokesOut=StokesPolMap(outMap, out.coordinates());
  AlwaysAssert(nStokesOut, AipsError);
  
  Matrix<Complex> s(4,4), sAdjoint(4,4);
  s=0;
  if(polFrame==StokesImageUtil::LINEAR) {
    s(0,0)=Complex(0.5);
    s(0,1)=Complex(0.5);
    s(1,2)=Complex(0.5);
    s(1,3)=Complex(0.0,0.5);
    s(2,2)=Complex(0.5);
    s(2,3)=Complex(0.0,-0.5);
    s(3,0)=Complex(0.5);
    s(3,1)=Complex(-0.5);
  }
  else {
    s(0,0)=Complex(0.5);
    s(0,3)=Complex(0.5);
    s(1,1)=Complex(0.5);
    s(1,2)=Complex(0.0,0.5);
    s(2,1)=Complex(0.5);
    s(2,2)=Complex(0.0,-0.5);
    s(3,0)=Complex(0.5);
    s(3,3)=Complex(-0.5);
  }
  sAdjoint=adjoint(s);
  
  Matrix<Complex> lambda(4,4);
  lambda=Complex(0.0);
  
  Matrix<Float> stokesPSF(4,4);
  Int i, j;
  for (inli.reset(), outli.reset(); !inli.atEnd() && !outli.atEnd(); inli++,outli++) {
    for (Int ix=0;ix<nx;ix++) {
      // Get the channel
      // Fill in the weight vector
      if(innpol==1) {
	lambda(inMap(0),inMap(0))=inli.vectorCursor()(ix);
      }
      else {
	for (i=0;i<innpol;i++) lambda(inMap(i),inMap(i))=
				 inli.matrixCursor()(ix,i);
      }
      stokesPSF=real(product(s,product(lambda, sAdjoint)));
      if(nStokesOut==1) {
	outli.rwVectorCursor()(ix)=stokesPSF(outMap(0),outMap(0));
      }
      else {
	for (j=0;j<outnpol;j++) {
	  for (i=0;i<outnpol;i++) {
	    outli.rwCubeCursor()(ix,i,j)=stokesPSF(outMap(i),outMap(j));
	  }
	}
      }
    }
  }
};
#endif

void StokesImageUtil::From(ImageInterface<Complex>& out,
			   const ImageInterface<Float>& in) {
  
  AlwaysAssert(in.shape()(0)==out.shape()(0), AipsError);
  AlwaysAssert(in.shape()(1)==out.shape()(1), AipsError);
  AlwaysAssert(in.shape()(3)==out.shape()(3), AipsError);
  
  
  Vector<Int> map;
  AlwaysAssert(StokesMap(map, in.coordinates()), AipsError);
  Vector<Int> outmap;
  AlwaysAssert(StokesMap(outmap, out.coordinates()), AipsError);
  
  Int nx = in.shape()(map(0));
  Int innpol = in.shape()(map(2));
  Int outnpol = out.shape()(outmap(2));
  
  Vector<Int> inMap(innpol), outMap(outnpol);
  StokesImageUtil::PolRep polFrame=StokesImageUtil::LINEAR;
  Int nStokesIn=StokesPolMap(inMap, in.coordinates());
  if(nStokesIn <=0){
    directCFromR(out, in);
    return;
  }
  AlwaysAssert(nStokesIn, AipsError);
  Int nStokesOut=CStokesPolMap(outMap, polFrame, out.coordinates());
  if(nStokesOut==0) {
    nStokesOut=StokesPolMap(outMap, out.coordinates());
    if(nStokesIn==nStokesOut) {
      out.copyData(LatticeExpr<Complex>(toComplex(in)));
      return;
    }
    throw(AipsError("Illegal conversion in FromStokesImage"));
  }
  
  // Loop over all planes
  LatticeStepper inls(in.shape(), IPosition(4, nx, 1, innpol, 1),
		      IPosition(4, map(0), map(2), map(1), map(3)));
  LatticeStepper outls(out.shape(), IPosition(4, nx, 1, outnpol, 1),
		       IPosition(4, map(0), map(2), map(1), map(3)));
  RO_LatticeIterator<Float> inli(in,  inls);
  LatticeIterator<Complex>  outli(out, outls);
  
  Vector<Float> s(4);
  CStokesVector csv(0.0, 0.0, 0.0, 0.0);
  StokesVector sv(0.0, 0.0, 0.0, 0.0);
  Int pol;
  if(nStokesIn==1) {
    for (inli.reset(), outli.reset(); !inli.atEnd() && !outli.atEnd(); inli++,outli++) {
      for (Int ix=0;ix<nx;ix++) {
	s=0.0;
	s(inMap(0))=inli.vectorCursor()(ix);
	sv=StokesVector(s(0), s(1), s(2), s(3));
	if(polFrame==StokesImageUtil::LINEAR) {
	  applySlin(csv, sv);
	}
	else {
	  applyScirc(csv, sv);
	}
        if(nStokesOut==1) {
	  outli.rwVectorCursor()(ix)=csv(outMap(0));
	}
	else {
	  for (pol=0;pol<outnpol;pol++) {
	    outli.rwMatrixCursor()(ix,pol)=csv(outMap(pol));
	  }
	}
      }
    }
  }
  else {
    for (inli.reset(), outli.reset(); !inli.atEnd() && !outli.atEnd(); inli++,outli++) {
      for (Int ix=0;ix<nx;ix++) {
	s=0.0;
	for(pol=0;pol<innpol;pol++) s(inMap(pol))=inli.matrixCursor()(ix,pol);
	sv=StokesVector(s(0), s(1), s(2), s(3));
	if(polFrame==StokesImageUtil::LINEAR) {
	  applySlin(csv, sv);
	}
	else {
	  applyScirc(csv, sv);
	}
        if(nStokesOut==1) {
	  outli.rwVectorCursor()(ix)=csv(outMap(0));
	}
	else {
	  for (pol=0;pol<outnpol;pol++) {
	    outli.rwMatrixCursor()(ix,pol)=csv(outMap(pol));
	  }
	}
      }
    }
  }
};

// map(axis) is the polarization in the sequence I,Q,U,V
// This is the sequence used by StokesVector
Int StokesImageUtil::StokesPolMap(Vector<Int>& map, const CoordinateSystem& coord) {
  
  map=-1;
  Int stokesIndex;
  stokesIndex= coord.findCoordinate(Coordinate::STOKES);
  StokesCoordinate stokesCoord=coord.stokesCoordinate(stokesIndex);
  Int pol, p;
  Bool Found= stokesCoord.toPixel(p, Stokes::I)||
    stokesCoord.toPixel(p, Stokes::Q)||
    stokesCoord.toPixel(p, Stokes::U)||
    stokesCoord.toPixel(p, Stokes::V);
  if(Found) {
    pol=0;
    Int found=0;
    if(stokesCoord.toPixel(p, Stokes::I)) {map(p)=pol;found++;} pol++;
    if(stokesCoord.toPixel(p, Stokes::Q)) {map(p)=pol;found++;} pol++;
    if(stokesCoord.toPixel(p, Stokes::U)) {map(p)=pol;found++;} pol++;
    if(stokesCoord.toPixel(p, Stokes::V)) {map(p)=pol;found++;} pol++;
    return found;
  }
  return 0;
}

// map(axis) is the polarization in the sequence XX,XY,YX,YY or RR,RL,LR,LL.
// This is the sequence used by CStokesVector
Int StokesImageUtil::CStokesPolMap(Vector<Int>& map, StokesImageUtil::PolRep& polFrame,
				   const CoordinateSystem& coord) {
  
  map=-1;
  Int stokesIndex;
  stokesIndex=coord.findCoordinate(Coordinate::STOKES);
  StokesCoordinate stokesCoord=coord.stokesCoordinate(stokesIndex);
  Int pol, p;
  Int found=0;
  Bool Linear= stokesCoord.toPixel(p, Stokes::XX)||
    stokesCoord.toPixel(p, Stokes::XY)||
    stokesCoord.toPixel(p, Stokes::YX)||
    stokesCoord.toPixel(p, Stokes::YY);
  if(Linear) {
    pol=0;
    if(stokesCoord.toPixel(p, Stokes::XX)) {map(p)=pol;found++;} pol++;
    if(stokesCoord.toPixel(p, Stokes::XY)) {map(p)=pol;found++;} pol++;
    if(stokesCoord.toPixel(p, Stokes::YX)) {map(p)=pol;found++;} pol++;
    if(stokesCoord.toPixel(p, Stokes::YY)) {map(p)=pol;found++;} pol++;
    polFrame=StokesImageUtil::LINEAR;
    return found;
  }
  Bool Circular= stokesCoord.toPixel(p, Stokes::LL)||
    stokesCoord.toPixel(p, Stokes::LR)||
    stokesCoord.toPixel(p, Stokes::RL)||
    stokesCoord.toPixel(p, Stokes::RR);


  if(Circular) {
    pol=0;
    if(stokesCoord.toPixel(p, Stokes::RR)) {map(p)=pol;found++;} pol++;
    if(stokesCoord.toPixel(p, Stokes::RL)) {map(p)=pol;found++;} pol++;
    if(stokesCoord.toPixel(p, Stokes::LR)) {map(p)=pol;found++;} pol++;
    if(stokesCoord.toPixel(p, Stokes::LL)) {map(p)=pol;found++;} pol++;
    polFrame=StokesImageUtil::CIRCULAR;
    return found;
  }
  return 0;
}

CoordinateSystem StokesImageUtil::StokesCoordFromMS(const IPosition& shape, Vector<Double>& deltas,
						    MeasurementSet& ms) {
  Vector<Int> whichStokes(0);
  return StokesCoordFromMS(shape, deltas, ms, whichStokes);
}

CoordinateSystem StokesImageUtil::StokesCoordFromMS(const IPosition& shape, Vector<Double>& deltas,
						    MeasurementSet& ms, Vector<Int>& whichStokes,
						    Bool doCStokes, Int fieldID, Int SPWID, Int feedID) {
  
  MSColumns msc(ms);
  
  Int nx=shape(0);
  Int ny=shape(1);
  Int npol=shape(2);
  Int nchan=shape(3);
  
  Vector<Double> refCoord=msc.field().phaseDir()(fieldID); 
  Vector<Double> refPixel(2); 
  refPixel(0)=Double(nx/2);
  refPixel(1)=Double(ny/2);
  
  Matrix<Double> xform(2,2);
  xform=0.0;xform.diagonal()=1.0;
  DirectionCoordinate myRaDec(MDirection::J2000,
			      Projection::SIN,
			      refCoord(0), refCoord(1),
			      deltas(0), deltas(1),
			      xform,
			      refPixel(0), refPixel(1));
  
  // Frequency
  Vector<Double> chanFreq=msc.spectralWindow().chanFreq()(SPWID); 
  if(nchan==0) nchan=chanFreq.shape()(SPWID);
  Double refChan=0.0;
  Vector<Double> freqResolution=msc.spectralWindow().resolution()(SPWID); 

  // Retrieve the first rest frequency used with this SPW_ID (for now)
  MSDopplerUtil msdoppler(ms);
  Vector<Double> restFreqArray;
  Double restFreq;
  if (msdoppler.dopplerInfo(restFreqArray,SPWID,fieldID)) {
    restFreq=restFreqArray(0);
  } else {
    restFreq=1.0;
  };
  
  SpectralCoordinate mySpectral(MFrequency::TOPO,  chanFreq(0),
				freqResolution(0), refChan,
				restFreq);
  
  // Polarization: If the specified whichStokes are ok, we use them
  // otherwise we guess
  if(Int(whichStokes.nelements())!=npol) {
    Vector<String> polType=msc.feed().polarizationType()(feedID);
    whichStokes.resize(npol);
    // Polarization
    if(doCStokes) {
      if (polType(0)=="X" || polType(0)=="Y") {
	switch(npol) {
	case 1:
	  whichStokes.resize(1);
	  whichStokes(0)=Stokes::I;
	  break;
	case 2:
	  whichStokes.resize(2);
	  whichStokes(0)=Stokes::XX;
	  whichStokes(1)=Stokes::YY;
	  break;
	default:
	  whichStokes.resize(4);
	  whichStokes(0)=Stokes::XX;
	  whichStokes(1)=Stokes::XY;
	  whichStokes(2)=Stokes::YX;
	  whichStokes(3)=Stokes::YY;
	}
      }
      else {
	switch(npol) {
	case 1:
	  whichStokes.resize(1);
	  whichStokes(0)=Stokes::I;
	  break;
	case 2:
	  whichStokes.resize(2);
	  whichStokes(0)=Stokes::LL;
	  whichStokes(1)=Stokes::RR;
	  break;
	default:
	  whichStokes.resize(4);
	  whichStokes(0)=Stokes::LL;
	  whichStokes(1)=Stokes::LR;
	  whichStokes(2)=Stokes::RL;
	  whichStokes(3)=Stokes::RR;
	};
      }
    }
    else {
      switch(npol) {
      case 1:
	whichStokes.resize(1);
	whichStokes(0)=Stokes::I;
	break;
      case 2:
	whichStokes.resize(2);
	whichStokes(0)=Stokes::I;
	if (polType(0)=="X" || polType(0)=="Y") {
	  whichStokes(1)=Stokes::Q;
	}
	else {
	  whichStokes(1)=Stokes::V;
	}
	break;
      case 3:
	whichStokes.resize(3);
	whichStokes(0)=Stokes::I;
	whichStokes(1)=Stokes::Q;
	whichStokes(2)=Stokes::U;
	break;
      case 4:
	whichStokes.resize(4);
	whichStokes(0)=Stokes::I;
	whichStokes(1)=Stokes::Q;
	whichStokes(2)=Stokes::U;
	whichStokes(3)=Stokes::V;
	break;
      };
    }
  }
  StokesCoordinate myStokes(whichStokes);
  
  // Now set up coordinates for image. If the shape is
  // 5 dimensional then we add another StokesCoordinate
  CoordinateSystem coordInfo; 
  coordInfo.addCoordinate(myRaDec);
  coordInfo.addCoordinate(myStokes);
  if(shape.nelements()==5) coordInfo.addCoordinate(myStokes);
  coordInfo.addCoordinate(mySpectral);
  return coordInfo;
  
}

/*  This function sets up the Stokes labelling for the polarization planes of 
    'cImage', the complex image that stores gridded correlations.
      
     For example, if 'V' is asked for, we need to grid RR and LL (npol = 2 pol planes in cImage).

     Called from the first call of ImageStokesImageUtil::cImage().

     Input :  shape = shape of cImage. 
                polRep : Circular or Linear - present in the data. 
     Input/Output : coords = Input polarization labelling is that of the target image ('V')
                                         Output polarization labelling is for the correlations ('RR','LL').
                           whichStokes = empty on input (first time). 
                                                  Fill in the data correlation labels ('RR', 'LL')

*/
CoordinateSystem StokesImageUtil::CStokesCoord(//const IPosition& shape,
					       const CoordinateSystem& coord,
					       Vector<Int>& whichStokes,
					       StokesImageUtil::PolRep polRep) {
  /*  
  Int nx=shape(0);
  Int ny=shape(1);
  Int npol=shape(2);
  Int nchan=shape(3);
  AlwaysAssert(nx>0, AipsError);
  AlwaysAssert(ny>0, AipsError);
  AlwaysAssert(npol>0, AipsError);
  AlwaysAssert(nchan>0, AipsError);
  */

  Int directionIndex=coord.findCoordinate(Coordinate::DIRECTION);
  DirectionCoordinate directionCoord=coord.directionCoordinate(directionIndex);
  
  Int spectralIndex=coord.findCoordinate(Coordinate::SPECTRAL);
  SpectralCoordinate spectralCoord=coord.spectralCoordinate(spectralIndex);
 
  Int stokesIndex=coord.findCoordinate(Coordinate::STOKES);
  StokesCoordinate
    stokesCoord=coord.stokesCoordinate(stokesIndex);

  /* STOKESDBG */ //cout << "Util : CStokesCoord - input - stokesCoord : " << stokesCoord.stokes()  << endl;
 
  // Polarization: If the specified whichStokes are ok, we use them
  //  if(Int(whichStokes.nelements())!=npol) {
  //  whichStokes.resize(npol);
  //  whichStokes=0;
    changeLabelsStokesToCorrStokes(stokesCoord, polRep, whichStokes);
    //}
  AlwaysAssert(whichStokes.nelements(), AipsError);
  StokesCoordinate stokesCoordOut(whichStokes);

  /* STOKESDBG */ //cout << "Util : CStokesCoord - output - stokesCoord : " << stokesCoordOut.stokes() << endl;
 
  // Now set up coordinates
  CoordinateSystem coordInfo; 
  coordInfo.addCoordinate(directionCoord);
  coordInfo.addCoordinate(stokesCoordOut);
  coordInfo.addCoordinate(spectralCoord);
  return coordInfo;

}

/*
     Logic : Read the desired image stokes from "stokesCoord" : 'V'
                Read the desired "npol" from the length of stokesCoord : '2'
                For npol = 1,2,3,4, if any image-pol choice requires an explicit mapping
                                            to correlations, resize 'whichStokes' accordingly, and fill in.
                             For example, 'V' is mapped to 'RR,LL' for Circular, and 'XY,YX' for Linear.
                Finally, check that all correlations in 'whichStokes' are present in the data.

      Subsequent calls to this function -- which already have the input stokesCoord in the 
      correlation basis, will not do anything.

      Rules :
           npol = 1 : I, XX,YY,XY,YX,RR,LL.RL,LR  --- choose only one correlation 
                          Q, U, V -- map to 2 correlations as required for Circular vs Linear
                          Note : "I" is a special-case for which ftmachines and gridders know how to use only one plane.
           npol = 2 : RRLL,RLLR,XXYY,XYYX;  IV,QU for Circular;  IQ,UV for Linear -- map to 2 correlations
                           IV, QU for Linear; IQ,UV for Circular -- map to 4 correlations
           npol = 3,4 : Choose all 4 correlations.

      'whichStokes' contains the output
*/
void StokesImageUtil::changeLabelsStokesToCorrStokes(StokesCoordinate &stokesCoord, 
						     StokesImageUtil::PolRep polRep, //Int npol,
                                                 Vector<Int>&whichStokes)
{
    Int inputNPol = (stokesCoord.stokes()).nelements();
    Vector<Int> svec = stokesCoord.stokes();
    AlwaysAssert( (inputNPol==1 || inputNPol==2 || inputNPol==3 || inputNPol), AipsError );

    switch( inputNPol )
    {
    case 1:
           whichStokes.resize(1);
           // by default, set to what the input is. This will take care of RR,LL,RL,LR,XX,YY,XY,YX and I.
           whichStokes[0]=svec[0];
           // for Q,U,V, two correlations needs to be gridded
           if(polRep==StokesImageUtil::CIRCULAR && Stokes::type(svec[0])==Stokes::Q)
	          {whichStokes.resize(2); whichStokes[0]=Stokes::RL; whichStokes[1]=Stokes::LR; }
           else if(polRep==StokesImageUtil::CIRCULAR && Stokes::type(svec[0])==Stokes::U)
	          {whichStokes.resize(2); whichStokes[0]=Stokes::RL; whichStokes[1]=Stokes::LR; }
           else if(polRep==StokesImageUtil::CIRCULAR && Stokes::type(svec[0])==Stokes::V)
	          {whichStokes.resize(2); whichStokes[0]=Stokes::RR; whichStokes[1]=Stokes::LL; }
           else if(polRep==StokesImageUtil::LINEAR && Stokes::type(svec[0])==Stokes::Q)
	          {whichStokes.resize(2); whichStokes[0]=Stokes::XX; whichStokes[1]=Stokes::YY; }
           else if(polRep==StokesImageUtil::LINEAR && Stokes::type(svec[0])==Stokes::U)
	          {whichStokes.resize(2); whichStokes[0]=Stokes::XY; whichStokes[1]=Stokes::YX; }
           else if(polRep==StokesImageUtil::LINEAR && Stokes::type(svec[0])==Stokes::V)
	          {whichStokes.resize(2); whichStokes[0]=Stokes::XY; whichStokes[1]=Stokes::YX; }
      break;
    case 2:
           whichStokes.resize(2);
           // by default, set to what the input is. This will take care of RRLL, RLLR, XXYY, XYYX
           whichStokes(0)=svec[0];
           whichStokes(1)=svec[1];           
           // only 2 correlations need to be gridded
           if(polRep==StokesImageUtil::CIRCULAR && Stokes::type(svec[0])==Stokes::I && Stokes::type(svec[1])==Stokes::V) 
   	        {whichStokes(0)=Stokes::RR; whichStokes(1)=Stokes::LL;}
           else if(polRep==StokesImageUtil::CIRCULAR && Stokes::type(svec[0])==Stokes::Q && Stokes::type(svec[1])==Stokes::U) 
   	        {whichStokes(0)=Stokes::RL; whichStokes(1)=Stokes::LR;}
           else if(polRep==StokesImageUtil::LINEAR && Stokes::type(svec[0])==Stokes::I && Stokes::type(svec[1])==Stokes::Q) 
   	        {whichStokes(0)=Stokes::XX; whichStokes(1)=Stokes::YY;}
           else if(polRep==StokesImageUtil::LINEAR && Stokes::type(svec[0])==Stokes::U && Stokes::type(svec[1])==Stokes::V) 
   	        {whichStokes(0)=Stokes::XY; whichStokes(1)=Stokes::YX;}
	   // all 4 correlations to be gridded. Only difference with above is CIRC to LIN
           else if( (polRep==StokesImageUtil::LINEAR && Stokes::type(svec[0])==Stokes::I && Stokes::type(svec[1])==Stokes::V) ||
		      (polRep==StokesImageUtil::LINEAR && Stokes::type(svec[0])==Stokes::Q && Stokes::type(svec[1])==Stokes::U) )
	              {
                          whichStokes.resize(4); 
                          whichStokes(0)=Stokes::XX; whichStokes(1)=Stokes::XY; 
                          whichStokes(2)=Stokes::YX; whichStokes(3)=Stokes::YY;
                      }
	   else if( (polRep==StokesImageUtil::CIRCULAR && Stokes::type(svec[0])==Stokes::I && Stokes::type(svec[1])==Stokes::Q) ||
                      (polRep==StokesImageUtil::CIRCULAR && Stokes::type(svec[0])==Stokes::U && Stokes::type(svec[1])==Stokes::V) )
	              {
                           whichStokes.resize(4); 
                           whichStokes(0)=Stokes::RR; whichStokes(1)=Stokes::RL; 
                           whichStokes(2)=Stokes::LR; whichStokes(3)=Stokes::LL;
                      }
       break;
    case 3: /* If npol=3, all 4 correlations need to be gridded */
    case 4:  /* Select all 4 correlations when npol=4 */
 	whichStokes.resize(4);
        if( polRep==StokesImageUtil::LINEAR)
	{
	   whichStokes(0)=Stokes::XX;
 	   whichStokes(1)=Stokes::XY;
	   whichStokes(2)=Stokes::YX;
	   whichStokes(3)=Stokes::YY;
	}
        else
	{
	   whichStokes(0)=Stokes::RR;
 	   whichStokes(1)=Stokes::RL;
	   whichStokes(2)=Stokes::LR;
	   whichStokes(3)=Stokes::LL;
	}
        break;
    }

    // Verify that all entries in whichStokes are present in the data 
    // This is to catch things like : dataset contains only RR,LL, but the user has asked for IQUV.
    

}// end of changeStokesToCorrStokes


/*  This function is not called from anywhere !!!!  It should go. */
#if 0
CoordinateSystem
StokesImageUtil::CStokesCoordFromImage(const ImageInterface<Complex>& image,
				       Vector<Int>& whichStokes,
				       StokesImageUtil::PolRep polRep) {

  IPosition shape=image.shape();

  Int npol=shape(2);
  
  CoordinateSystem coord=image.coordinates();

  Int directionIndex=coord.findCoordinate(Coordinate::DIRECTION);
  DirectionCoordinate directionCoord=coord.directionCoordinate(directionIndex);

  Int spectralIndex=coord.findCoordinate(Coordinate::SPECTRAL);
  SpectralCoordinate spectralCoord=coord.spectralCoordinate(spectralIndex);

  // Polarization: If the specified whichStokes are ok, we use them
  // otherwise we guess
  if(Int(whichStokes.nelements())!=npol) {
    whichStokes.resize(npol);
    // Polarization
    if (polRep==StokesImageUtil::LINEAR) {
      switch(npol) {
      case 1:
	whichStokes.resize(1);
	whichStokes(1)=Stokes::I;
	break;
      case 2:
	whichStokes.resize(2);
	whichStokes(1)=Stokes::XX;
	whichStokes(0)=Stokes::YY;
	break;
      default:
	whichStokes.resize(4);
	whichStokes(0)=Stokes::XX;
	whichStokes(1)=Stokes::XY;
	whichStokes(2)=Stokes::YX;
	whichStokes(3)=Stokes::YY;
      }
    }
    else {
      switch(npol) {
      case 1:
	whichStokes.resize(1);
	whichStokes(0)=Stokes::I;
	break;
      case 2:
	whichStokes.resize(2);
	whichStokes(0)=Stokes::LL;
	whichStokes(1)=Stokes::RR;
	break;
      default:
	whichStokes.resize(4);
	whichStokes(0)=Stokes::LL;
	whichStokes(1)=Stokes::LR;
	whichStokes(2)=Stokes::RL;
	whichStokes(3)=Stokes::RR;
      };
    }
  }
  StokesCoordinate stokesCoord(whichStokes);
  
  // Now set up coordinates for image.
  CoordinateSystem coordInfo; 
  coordInfo.addCoordinate(directionCoord);
  coordInfo.addCoordinate(stokesCoord);
  coordInfo.addCoordinate(spectralCoord);
  return coordInfo;

}
#endif

// Return a map to RA, DEC, pol, chan. 0 is RA, 1 is Dec, 2 is
// polarization, and 3 is Channel
Bool StokesImageUtil::StokesMap(Vector<Int>& map, const CoordinateSystem& coord) {
  
  map.resize(4);
  map=-1;

  Int dirIndex=coord.findCoordinate(Coordinate::DIRECTION);
  if(dirIndex>-1) {
    Vector<Int> dirAxes=coord.pixelAxes(dirIndex);
    if(dirAxes.nelements()>2) {
      return false;
    }
    map(0)=dirAxes(0);
    map(1)=dirAxes(1);
  }
  else {
    return false;
  }

  Int stokesIndex=coord.findCoordinate(Coordinate::STOKES);
  if(stokesIndex>-1) {
    Vector<Int> stokesAxes=coord.pixelAxes(stokesIndex);
    if(stokesAxes.nelements()>1) {
      return false;
    }
    map(2)=stokesAxes(0);
  }
  else {
    return false;
  }

  Int spectralIndex=coord.findCoordinate(Coordinate::SPECTRAL);
  if(spectralIndex>-1) {
    Vector<Int> spectralAxes=coord.pixelAxes(spectralIndex);
    if(spectralAxes.nelements()>1) {
      return false;
    }
    map(3)=spectralAxes(0);
  }
  else {
    return false;
  }

  return true;
}

void StokesImageUtil::BoxMask(ImageInterface<Float>& mask,
			      const IPosition& blc,
			      const IPosition& trc, const Float value) 
{
  LatticeStepper ls(mask.shape(), IPosition(4, mask.shape()(0), 1, 1, 1),
		    IPosition(4, 0, 1, 2, 3));
  ls.subSection(blc, trc);
  LatticeIterator<Float> mli(mask, ls);
  
  // Loop over all planes
  for (mli.reset();!mli.atEnd();mli++) {
    mli.rwCursor()=value;
  }
  
}

// Change the representation used. The contents of the image
// are not changed!
void StokesImageUtil::changeCStokesRep(ImageInterface<Complex>& image,
				       StokesImageUtil::PolRep polRep) {

  CoordinateSystem coords=image.coordinates();

  Int stokesIndex=coords.findCoordinate(Coordinate::STOKES);
  StokesCoordinate
    stokesCoord=coords.stokesCoordinate(stokesIndex);

  ///// Int npol=stokesCoord.stokes().nelements(); // apparently not used and no side effects
 
  /* STOKESDBG */ //cout << "Util::changeCStokesRep - input - stokescoord : " << stokesCoord.stokes() << "   npol : " << npol << endl;

  Vector<Int> whichStokes(0);
  //whichStokes=0;
  changeLabelsStokesToCorrStokes(stokesCoord, polRep,  whichStokes);

  StokesCoordinate newStokesCoord(whichStokes);
  coords.replaceCoordinate(newStokesCoord, stokesIndex);
  image.setCoordinateInfo(coords);
  /* STOKESDBG */ //cout << "Util::changeCStokesRep - output - stokescoord : " << newStokesCoord.stokes() << endl;
}

Bool StokesImageUtil::
standardImageCoordinates(const ImageInterface<Complex>& image) {
  return (standardImageCoordinates(image.coordinates()));
};

Bool StokesImageUtil::
standardImageCoordinates(const ImageInterface<Float>& image) {
  return (standardImageCoordinates(image.coordinates()));
};

Bool StokesImageUtil::
standardImageCoordinates(const CoordinateSystem& coords) {
  Bool isStandard = true;
  {
    Int ind=coords.findCoordinate(Coordinate::DIRECTION);
    if (ind != 0) isStandard = false;
    ind=coords.findCoordinate(Coordinate::STOKES);
    if (ind != 1) isStandard = false;
    ind=coords.findCoordinate(Coordinate::SPECTRAL);
    if (ind != 2) isStandard = false;
  }
  return isStandard;
};

} //#End casa namespace


