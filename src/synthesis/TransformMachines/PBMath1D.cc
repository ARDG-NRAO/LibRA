//# PBMath1D.cc: Implementation for PBMath1D
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
 
#include <casacore/casa/aips.h>
#include <casacore/casa/BasicSL/Complex.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/Vector.h>
#include <synthesis/TransformMachines/PBMath1D.h>

#include <casacore/images/Regions/ImageRegion.h>
#include <casacore/images/Images/ImageInterface.h>

#include <components/ComponentModels/SkyComponent.h>
#include <components/ComponentModels/Flux.h>
#include <components/ComponentModels/ComponentShape.h>

#include <casacore/lattices/Lattices/LatticeIterator.h>
#include <casacore/lattices/Lattices/LatticeStepper.h>
#include <casacore/lattices/LRegions/LCSlicer.h>
#include <casacore/casa/Arrays/IPosition.h>

#include <casacore/measures/Measures.h>
#include <casacore/measures/Measures/MeasConvert.h>

#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>
#include <casacore/coordinates/Coordinates/StokesCoordinate.h>
#include <casacore/coordinates/Coordinates/Projection.h>
#include <casacore/coordinates/Coordinates/CoordinateUtil.h>

#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/Exceptions/Error.h>



using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

PBMath1D::PBMath1D()
  : composite_p(2048)
{
};


PBMath1D::PBMath1D(Quantity maximumRadius,
		   Quantity refFreq,
		   Bool isThisVP,
		   BeamSquint squint,
		   Bool useSymmetricBeam) :
  PBMathInterface(isThisVP, squint, useSymmetricBeam),
  wideFit_p(false),maximumRadius_p(maximumRadius),
  refFreq_p(refFreq),
  composite_p(2048)
{
  fScale_p = refFreq_p.getValue("GHz");  // scale is ratio of refFreq_p to 1GHz
  refFreq_p = Quantity( 1.0, "GHz");  // internal Ref Freq is now 1GHz
  // convert instantiation parameters to GHz*arcmin reference
  maximumRadius_p = maximumRadius_p * fScale_p;
  scale_p = 1.0/(C::arcmin * C::giga);
};

PBMath1D::~PBMath1D()
{
};




ImageRegion* 
PBMath1D::extent (const ImageInterface<Complex>& in, const MDirection& pointDir,
		  const Int row, const Float fPad,  const Int iChan,  
		  const SkyJones::SizeType sizeType)
{
  if (row) {} // Not used yet

  CoordinateSystem  coords=in.coordinates();  

  Vector<Float> blc(4);
  Vector<Float> trc(4);
  blc.set(0.0);
  trc.set(0.0);
  {
    Int stokesIndex, k1, k2;
    CoordinateUtil::findStokesAxis(stokesIndex, k1, k2, coords);
    blc(stokesIndex) = 0.0;
    trc(stokesIndex) = in.shape()(stokesIndex)-1;
    Int spectralIndex=CoordinateUtil::findSpectralAxis(coords); 
    blc(spectralIndex) = 0.0;
    trc(spectralIndex) = in.shape()(spectralIndex)-1;
  }
  extentguts(coords,  pointDir, fPad, iChan, blc, trc);
  refineSize(blc, trc, in.shape(), sizeType);
  LCSlicer lcs( blc, trc );
  return  ( new ImageRegion(lcs) );
};
ImageRegion* 
PBMath1D::extent (const ImageInterface<Float>& in, const MDirection& pointDir,
                  const Int row, const Float fPad, const Int iChan, 
		  const SkyJones::SizeType sizeType)
{
  if (row) {} // unused
  CoordinateSystem  coords=in.coordinates();
  Vector<Float> blc(4);
  Vector<Float> trc(4);
  blc.set(0.0);
  trc.set(0.0);
  {
    Int stokesIndex, k1, k2;
    CoordinateUtil::findStokesAxis(stokesIndex, k1, k2, coords);
    blc(stokesIndex) = 0.0;
    trc(stokesIndex) = in.shape()(stokesIndex)-1;
    Int spectralIndex=CoordinateUtil::findSpectralAxis(coords);
    blc(spectralIndex) = 0.0;
    trc(spectralIndex) = in.shape()(spectralIndex)-1;
  }
  extentguts(coords,  pointDir, fPad, iChan, blc, trc);
  refineSize(blc, trc, in.shape(), sizeType);
  LCSlicer lcs( blc, trc );
  return  ( new ImageRegion(lcs) );
};



Int PBMath1D::support(const CoordinateSystem& cs){
Int directionIndex=cs.findCoordinate(Coordinate::DIRECTION);
 AlwaysAssert(directionIndex>=0, AipsError);
 DirectionCoordinate
   directionCoord=cs.directionCoordinate(directionIndex);
 
 Vector<String> dirunit=directionCoord.worldAxisUnits();

 Double freq;
 {
   Int spectralIndex=cs.findCoordinate(Coordinate::SPECTRAL);
   AlwaysAssert(spectralIndex>=0, AipsError);
   SpectralCoordinate
     spectralCoord=cs.spectralCoordinate(spectralIndex);

   
   Vector<String> units(1);
   units = "Hz";
   spectralCoord.setWorldAxisUnits(units);

   Vector<Double> spectralWorld(1);
   Vector<Double> spectralPixel(1);
   spectralPixel(0) = 0;
   spectralCoord.toWorld(spectralWorld, spectralPixel);  
   freq  = spectralWorld(0);
  }



  // maximumRadius_p: maximum radius at 1 GHz frequency
  //Double delta = maximumRadius_p.getValue("rad") *  1.0e+9 / freq;


  //Number of pix at freq
  Double numpix=maximumRadius_p.getValue(dirunit(0))/fabs(directionCoord.increment()(0))*2.0*1.0e9/freq ;
  
 
  return Int(floor(numpix));


}
void  PBMath1D::refineSize(Vector<Float>& blc, Vector<Float>& trc, const IPosition& shape, 
			    SkyJones::SizeType sizeType)
{
  // Round Down and Up for BLC and TRC, make them integers
  Vector<Bool> blcTrouble(blc.nelements(), false);
  Vector<Bool> trcTrouble(blc.nelements(), false);
  Vector<Float> d1(2);
  Vector<Float> d2(2);

  for (Int i=0; i<2; i++) {

    blc(i) = (Int)(blc(i));
    trc(i) = (Int)(trc(i)+0.99);  // OK, its ALMOST rounding up    

   if (blc(i) < 0) {
      blc(i) = 0;
      blcTrouble(i) = true;
    }
   if (trc(i) > shape(i)-1) {
      trc(i) = shape(i)-1;
      trcTrouble(i) = true;
    }

    d1(i) = trc(i) - blc(i) + 1;

    if (sizeType == SkyJones::POWEROF2) {
      d2(i) = (Int)( pow( 2.0, (Double)(Int)(log((Double)d1(i))/log(2.0) + 1.0) )+0.01);
    } else if (sizeType == SkyJones::COMPOSITE) {
      d2(i) = composite_p.nextLarger( (Int)d1(i) );      
    } else {
      d2(i) = d1(i);
    }

    // Deal with cases:

    if (d2(i) >= shape(i)) {
      // requested size doesn't even fit into image:
      // ----- revert to image size
      blc(i) = 0; trc(i) = shape(i)-1;

    } else if (blcTrouble(i)) {
      // requseted size fits, but buts up against the "bottom";
      // ----- make full adjustment to the "top"
      blc(i) = 0; trc(i) = d2(i)-1;

    } else if (trcTrouble(i)) {
      // requseted size fits, but buts up against the "top";
      // ----- make full adjustment to the "bottom"
      trc(i) = shape(i)-1;  blc(i) = shape(i) - d2(i);

    } else {
      // requested subimage does not exceed starting image
      // ----- do appropriate thing, based on even or odd
      Float diff = d2(i) - d1(i);
      Bool even = (Bool)( (Int)diff == 2 * (Int)(diff/2) );
      if (even) {
	blc(i) = blc(i) - diff/2;
	trc(i) = trc(i) + diff/2;
      } else {
	blc(i) = blc(i) - diff/2 + 0.5;
	trc(i) = trc(i) + diff/2 + 0.5;
      }
    }
  }
};




void
PBMath1D::extentguts (const CoordinateSystem& coords, const MDirection& pointDir,
                  const Float fPad, const Int iChan, Vector<Float>& blc, Vector<Float>& trc)

{
  Int directionIndex=coords.findCoordinate(Coordinate::DIRECTION);
  AlwaysAssert(directionIndex>=0, AipsError);
  DirectionCoordinate
    directionCoord=coords.directionCoordinate(directionIndex);
  Vector<String> units(2); units = "deg";                       
  directionCoord.setWorldAxisUnits(units);

  // convert to the EPOCH of these coords
  MDirection::Types t2 = (MDirection::Types) (pointDir.getRef().getType());
  MDirection pointDirE;
  if (t2 != directionCoord.directionType()) {
    MDirection::Convert converter;
    ObsInfo oi=coords.obsInfo();
    converter.setOut(MDirection::Ref(directionCoord.directionType(), 
				     MeasFrame(oi.obsDate(), oi.telescopePosition())));
    pointDirE = converter(pointDir);
  } else {
    pointDirE = pointDir;
  }

  Double freq;
  {
    Int spectralIndex=coords.findCoordinate(Coordinate::SPECTRAL);
    AlwaysAssert(spectralIndex>=0, AipsError);
    SpectralCoordinate
      spectralCoord=coords.spectralCoordinate(spectralIndex);
    
    units.resize(1);
    units = "Hz";
    spectralCoord.setWorldAxisUnits(units);

    Vector<Double> spectralWorld(1);
    Vector<Double> spectralPixel(1);
    spectralPixel(0) = iChan;
    spectralCoord.toWorld(spectralWorld, spectralPixel);  
    freq  = spectralWorld(0);
  }

  Vector<Double> edgeWorld(2);
  Vector<Double> edge1Pixel(2);
  Vector<Double> edge2Pixel(2);


  // maximumRadius_p: maximum radius at 1 GHz frequency
  Double delta = maximumRadius_p.getValue("rad") *  1.0e+9 / freq;
  {
    MDirection edgeDir( pointDirE );
    edgeDir.shift( delta, 0.0, true);
    edgeWorld(0) = edgeDir.getAngle().getValue("deg")(0);
    edgeWorld(1) = edgeDir.getAngle().getValue("deg")(1);
    directionCoord.toPixel(edge1Pixel, edgeWorld);
  }
  {
    MDirection edgeDir( pointDirE );
    edgeDir.shift( -delta, 0.0, true);
    edgeWorld(0) = edgeDir.getAngle().getValue("deg")(0);
    edgeWorld(1) = edgeDir.getAngle().getValue("deg")(1);
    directionCoord.toPixel(edge2Pixel, edgeWorld);
  }
  blc(0) = min( edge1Pixel(0), edge2Pixel(0) );
  trc(0) = max( edge1Pixel(0), edge2Pixel(0) );
  if (fPad > 0.1) {
    Float pad = (trc(0) - blc(0)) * (fPad - 1.0)/2;
    blc(0) = blc(0) - pad;
    trc(0) = trc(0) + pad;
  }
  {
    MDirection edgeDir( pointDirE );
    edgeDir.shift( 0.0, delta, true);
    edgeWorld(0) = edgeDir.getAngle().getValue("deg")(0);
    edgeWorld(1) = edgeDir.getAngle().getValue("deg")(1);
    directionCoord.toPixel(edge1Pixel, edgeWorld);
  }
  {
    MDirection edgeDir( pointDirE );
    edgeDir.shift( 0.0, -delta, true);
    edgeWorld(0) = edgeDir.getAngle().getValue("deg")(0);
    edgeWorld(1) = edgeDir.getAngle().getValue("deg")(1);
    directionCoord.toPixel(edge2Pixel, edgeWorld);
  }
  blc(1) = min( edge1Pixel(1), edge2Pixel(1) );
  trc(1) = max( edge1Pixel(1), edge2Pixel(1) );
  if (fPad > 0.1) {
    Float pad = (trc(1) - blc(1)) * (fPad - 1.0)/2;
    blc(1) = blc(1) - pad;
    trc(1) = trc(1) + pad;
  }
};




void PBMath1D::symmetrizeSquintedBeam()
{
  // eventually we need to create the 2D squinted RR and LL
  // beams and average them.  For now, we just return the
  // unsquinted beams

  if (vp_p.nelements() == 0) {
    fillPBArray();
  }
  esvp_p = vp_p; 
};

ImageInterface<Complex>&  
PBMath1D::apply(const ImageInterface<Complex>& in,
		ImageInterface<Complex>& out,
		const MDirection& pointDir,
		const Quantity parAngle,       
		const BeamSquint::SquintType doSquint,
		Bool inverse,
		Bool conjugate,
		Int iPower,  
		Float cutoff,
		Bool forward)
{
  LogIO os(LogOrigin("PBMath1D", "apply"));
  // Check that in and out are comparable:
  if (in.shape() != out.shape()) {
    throw(AipsError("PBMath1D::apply(ImageInterface...) - in and out images have different shapes"));    
  } 
  CoordinateSystem coords=in.coordinates();
  if (!coords.near(out.coordinates()) ) {
    throw(AipsError("PBMath1D::apply(ImageInterface...) - in and out images have different coordinates"));    
  }

  Int directionIndex=coords.findCoordinate(Coordinate::DIRECTION);
  AlwaysAssert(directionIndex>=0, AipsError);
  DirectionCoordinate
    directionCoord=coords.directionCoordinate(directionIndex);
  Vector<String> units(2); units = "deg";                       
    directionCoord.setWorldAxisUnits(units);

  // convert to the EPOCH of these coords
  MDirection::Types t2 = (MDirection::Types) (pointDir.getRef().getType());
  MDirection pointDirE;


  if (t2 != directionCoord.directionType()) {
    MDirection::Convert converter;
    ObsInfo oi=coords.obsInfo();
    converter.setOut(MDirection::Ref(directionCoord.directionType(), 
				     MeasFrame(oi.obsDate(), oi.telescopePosition())));
    pointDirE = converter(pointDir);
  } else {
    pointDirE = pointDir;
  }

  Int stokesIndex=coords.findCoordinate(Coordinate::STOKES);
  AlwaysAssert(stokesIndex>=0, AipsError);
  StokesCoordinate
    stokesCoord=coords.stokesCoordinate(stokesIndex);
 
  Int spectralIndex=coords.findCoordinate(Coordinate::SPECTRAL);
  AlwaysAssert(spectralIndex>=0, AipsError);
  SpectralCoordinate
    spectralCoord=coords.spectralCoordinate(spectralIndex);
 
  units.resize(1);
  units = "Hz";
  spectralCoord.setWorldAxisUnits(units);
 
  Int nchan=in.shape()(3);
    
  Vector<Double> pointingCenterWorld(2);
  Vector<Double> pointingCenterPixel(2);
  Vector<Double> directionPixel(2);

  pointingCenterWorld(0) = pointDirE.getAngle().getValue("deg")(0);
  pointingCenterWorld(1) = pointDirE.getAngle().getValue("deg")(1);
  directionCoord.toPixel(pointingCenterPixel, pointingCenterWorld);
  MDirection newpointDirE;
  Vector<Double> nonSquintedPointingPixel = pointingCenterPixel.copy();

  os << "pointingCenterWorld " << pointingCenterWorld << LogIO::DEBUGGING;
  os << "pointingCenterPixel " << pointingCenterPixel << LogIO::DEBUGGING;
  
  // Fill in a cache of the frequencies & squints
  Vector<Double> spectralWorld(1);
  Vector<Double> spectralPixel(1);
  Matrix<Double> xSquintPixCache(2, nchan);
  Matrix<Double> ySquintPixCache(2, nchan);
  Vector<Double> spectralCache(nchan);

  {
    for(Int chan=0;chan<nchan;chan++) {
      spectralPixel(0)=chan;
      if(!spectralCoord.toWorld(spectralWorld, spectralPixel)) {
        os  << "Cannot find frequency for this plane" << LogIO::EXCEPTION;
      }
      spectralCache(chan)=spectralWorld(0);

      
      if (doSquint == BeamSquint::RR || doSquint == BeamSquint::GOFIGURE) {
	squint_p.getPointingDirection (pointDirE,
				       parAngle, 
				       Quantity(spectralWorld(0),"Hz"),
				       BeamSquint::RR, newpointDirE);
	pointingCenterWorld(0) = newpointDirE.getAngle().getValue("deg")(0);
	pointingCenterWorld(1) = newpointDirE.getAngle().getValue("deg")(1);
	directionCoord.toPixel(pointingCenterPixel, pointingCenterWorld);
	xSquintPixCache(0, chan) = pointingCenterPixel(0);
	ySquintPixCache(0, chan) = pointingCenterPixel(1);
      } else {
	xSquintPixCache(0, chan) =  nonSquintedPointingPixel(0);
	ySquintPixCache(0, chan) =  nonSquintedPointingPixel(1);
      }
      if (doSquint == BeamSquint::LL || doSquint == BeamSquint::GOFIGURE) {
	squint_p.getPointingDirection (pointDirE,
				       parAngle, 
				       Quantity(spectralWorld(0),"Hz"),
				       BeamSquint::LL, newpointDirE);
	pointingCenterWorld(0) = newpointDirE.getAngle().getValue("deg")(0);
	pointingCenterWorld(1) = newpointDirE.getAngle().getValue("deg")(1);
	directionCoord.toPixel(pointingCenterPixel, pointingCenterWorld);
	xSquintPixCache(1, chan) = pointingCenterPixel(0);
	ySquintPixCache(1, chan) = pointingCenterPixel(1);
      } else {
	xSquintPixCache(1, chan) =  nonSquintedPointingPixel(0);
	ySquintPixCache(1, chan) =  nonSquintedPointingPixel(1);
      }
    }
  }

  /*
  cout << "pointingCenterPixel x,y = " << nonSquintedPointingPixel << endl;
  cout << "squinted pointingCenterPixel x,y RR = " << xSquintPixCache(0, 0) << ", " 
       << ySquintPixCache(0, 0) << endl;
  cout << "squinted pointingCenterPixel x,y LL = " << xSquintPixCache(1, 0) << ", " 
       << ySquintPixCache(1, 0) << endl;
  */

  // Iterate through in minimum IO/Memory chunks
  //IPosition ncs = in.niceCursorShape();
  IPosition ncs=in.shape();
  ncs(2) = 1; ncs(3) = 1;
  RO_LatticeIterator<Complex> li(in, LatticeStepper(in.shape(), ncs, IPosition(4,0,1,2,3) )  );
  LatticeIterator<Complex> oli(out, LatticeStepper(in.shape(), ncs, IPosition(4,0,1,2,3)) );

  // taper no longer appears to be used
  //Complex taper;
  //Float r2=0.0;
  //Float r=0.0;

  Vector<Double> increment = directionCoord.increment();
  Int rrplane = -1;
  Int llplane = -1;
  stokesCoord.toPixel( rrplane, Stokes::RR );
  stokesCoord.toPixel( llplane, Stokes::LL );

  /*
  cout << "stokes types in image = " << stokesCoord.stokes() << endl;
  cout << "rr plane = " << rrplane << "   ll plane = " << llplane << endl;

  */
  Double xPixel;  Double yPixel;

  Int laststokes = -1;
  Int lastChan = -1;
  Int ichan;
  Int istokes;
  Int ix0, iy0;
  //Int indx;
  for(li.reset(),oli.reset();!li.atEnd();li++,oli++) {

    IPosition itsShape(li.matrixCursor().shape());
    IPosition loc(li.position());

    ichan = loc(3);
    istokes = loc(2);
    iy0 = loc(1);
    ix0 = loc(0);

    // determine the pointing: RR, LL, or Center? We make a slight mistake
    // here since we ignore the difference between the RR beam and the
    // RL beam, say. The latter is slightly smaller because of the
    // squint. Hence this code should be deprecated in favor of the
    // correct 2D version (when mosaicing in polarization)
    if ((doSquint == BeamSquint::RR) ||
	((doSquint == BeamSquint::GOFIGURE) && (istokes == rrplane)) ) {
      xPixel = xSquintPixCache(0, ichan);
      yPixel = ySquintPixCache(0, ichan);
    } else if ((doSquint == BeamSquint::LL) ||
	       ((doSquint == BeamSquint::GOFIGURE) && (istokes == llplane)) ) {
      xPixel = xSquintPixCache(1, ichan);
      yPixel = ySquintPixCache(1, ichan);
    } else {
      xPixel = nonSquintedPointingPixel(0);
      yPixel = nonSquintedPointingPixel(1);
    }

    if (istokes != laststokes) {
      //cerr << "Stokes = " << istokes << " pix = " << xPixel << ", " << yPixel << endl;
      laststokes = istokes;
    }

    Double factor = 60.0 * spectralCache(ichan)/1.0e+9 ;  // arcminutes * GHz
    Double rmax2 = square( maximumRadius_p.getValue("'") / factor );
    if (wideFit_p) {
      // fill vp with interpolated values for current frequency
      if (ichan!=lastChan) {
	nearestVPArray(spectralCache(ichan));
	lastChan=ichan;
      }
    }
		       
    Vector<Float> rx2(itsShape(0));
    Vector<Float> ry2(itsShape(1));
    for(Int ix=0;ix<itsShape(0);ix++) {
      rx2(ix) =  square( increment(0)*((Double)(ix+ix0) - xPixel) );
    }
    for(Int iy=0;iy<itsShape(1);iy++) {
      ry2(iy) =  square( increment(1)*((Double)(iy+iy0) - yPixel) );
    }
   
    const Matrix<Complex>& inmat = li.matrixCursor();
    Matrix<Complex>& outmat=oli.rwMatrixCursor();

    Bool incopy, outcopy, del;
    const Complex * inpoint = inmat.getStorage(incopy);
    Complex *outpoint =outmat.getStorage(outcopy);
    Float * rx2point = rx2.getStorage(del);
    Float * ry2point= ry2.getStorage(del);
    Complex* vppoint=vp_p.getStorage(del);
    Int nx=itsShape(0);
    Int ny=itsShape(1);
    Double inverseIncrementRadius=inverseIncrementRadius_p;
#pragma omp parallel default(none) firstprivate(inpoint, outpoint, rx2point, ry2point, vppoint, iPower, conjugate, inverse, forward, nx, ny, rmax2, factor, inverseIncrementRadius, cutoff)
    {
#pragma omp for   
    for(Int iy=0;iy<ny;iy++) {
      Float ry2val=ry2point[iy];
      applyXLine(inpoint, outpoint , rx2point , vppoint , ry2val, iPower, conjugate, inverse, forward, nx, iy, rmax2, 
		 factor, inverseIncrementRadius, cutoff);
      /*for(Int ix=0;ix<itsShape(0);ix++) {

	r2 =  rx2(ix) +  ry2(iy);
	
	if (r2 > rmax2) {
	  oli.rwMatrixCursor()(ix, iy) = 0.0;
	} else {
	  r = sqrt(r2) * factor;
	  indx = Int(r*inverseIncrementRadius_p);
	  if (norm(vp_p(indx)) > 0.0) {
	    if(iPower==2) {
	      taper = vp_p(indx) * conj(vp_p(indx));
	    }
	    else {
	      taper = vp_p(indx);
	    }
	  } else {
	    taper = 0.0;
	  }
	  if (conjugate) {
	    taper =  conj(taper);
	  }
	  // Differentiate between forward (Sky->UV) and
	  // inverse (UV->Sky) - these need different
	  // applications of the PB
	  if(!forward) {
	    taper =  conj(taper);
	  }
	  if (inverse) {
	    if (abs(taper) < cutoff ) {
	      oli.rwMatrixCursor()(ix, iy) = 0.0;
	    } else {
	      oli.rwMatrixCursor()(ix, iy) = li.matrixCursor()(ix, iy) / taper ;
	    }
	  } else {  // not inverse!
	    oli.rwMatrixCursor()(ix, iy) = li.matrixCursor()(ix, iy) * taper;
	  }
	}
      }
      */
    }
    } //end pragma
    outmat.putStorage(outpoint, outcopy);
    inmat.freeStorage(inpoint, incopy);
  }

  return out;

};

  void PBMath1D::applyXLine(const Complex*& in, Complex*& out, Float*& rx2, Complex*& vp, const Float ry2, const Int ipower, const Bool conjugate, const Bool inverse, const Bool forward, const Int nx, const Int iy, const Double rmax2, const Double factor, const Double inverseIncrementRadius, const Float cutoff)
{ 
  Float r;
  Int indx;
  Complex taper;
  for(Int ix=0;ix<nx;ix++) {
    
    Float r2 =  rx2[ix] +  ry2;
	
    if (r2 > rmax2){
      out[ix+iy*nx] = 0.0;
    } 
    else {
      r = sqrt(r2) * factor;
      indx = Int(r*inverseIncrementRadius);
      if (norm(vp[indx]) > 0.0) {
	if(ipower==2) {
	  taper = vp[indx] * conj(vp[indx]);
	}
	else {
	  taper = vp[indx];
	}
      } else {
	taper = 0.0;
      }
      if (conjugate) {
	taper =  conj(taper);
      }
      // Differentiate between forward (Sky->UV) and
      // inverse (UV->Sky) - these need different
      // applications of the PB
      if(!forward) {
	taper =  conj(taper);
      }
      if (inverse) {
	if (abs(taper) < cutoff ) {
	  out[ix+iy*nx] = 0.0;
	} else {
	  out[ix+iy*nx]  = (in[ix+iy*nx]) / taper ;
	}
      } else {  // not inverse!
	out[ix+iy*nx]  = (in[ix+iy*nx]) * taper ;
      }
    }
  }
};

ImageInterface<Float>& 
PBMath1D::apply(const ImageInterface<Float>& in,
		ImageInterface<Float>& out,
		const MDirection& pointDir,
		const Quantity parAngle,
		const BeamSquint::SquintType doSquint,
		Float /*cutoff*/, const Int ipower)
{
  LogIO os(LogOrigin("PBMath1D", "apply"));
 
  //  cout << "PBMath1D::apply: image shape: " << in.shape() << endl;
  // Check that in and out are comparable:
  if (in.shape() != out.shape()) {
    throw(AipsError("PBMath1D::apply(ImageInterface...) - in and out images have different shapes"));
    
  } 
  CoordinateSystem coords=in.coordinates();
  if (!coords.near(out.coordinates())) {
    throw(AipsError("PBMath1D::apply(ImageInterface...) - in and out images have different coordinates"));
  }

  Int directionIndex=coords.findCoordinate(Coordinate::DIRECTION);
  AlwaysAssert(directionIndex>=0, AipsError);
  DirectionCoordinate
    directionCoord=coords.directionCoordinate(directionIndex);
  Vector<String> units(2); units = "deg";                       
    directionCoord.setWorldAxisUnits(units);

  // convert to the EPOCH of these coords
  MDirection::Types t2 = (MDirection::Types) (pointDir.getRef().getType());
  MDirection pointDirE(pointDir);

  if (t2 != directionCoord.directionType()) {
    MDirection::Convert converter;
    ObsInfo oi=coords.obsInfo();
    converter.setOut(MDirection::Ref(directionCoord.directionType(), 
				     MeasFrame(oi.obsDate(), oi.telescopePosition())));
    pointDirE = converter(pointDir);
  } else {
    pointDirE = pointDir;
  }
  Int stokesIndex=coords.findCoordinate(Coordinate::STOKES);
  AlwaysAssert(stokesIndex>=0, AipsError);
  StokesCoordinate
    stokesCoord=coords.stokesCoordinate(stokesIndex);
 
  Int spectralIndex=coords.findCoordinate(Coordinate::SPECTRAL);
  AlwaysAssert(spectralIndex>=0, AipsError);
  SpectralCoordinate
    spectralCoord=coords.spectralCoordinate(spectralIndex);
 
  units.resize(1);
  units = "Hz";
  spectralCoord.setWorldAxisUnits(units);
 
  Int nchan=in.shape()(3);
    
  Vector<Double> pointingCenterWorld(2);
  Vector<Double> pointingCenterPixel(2);
  Vector<Double> directionPixel(2);

  pointingCenterWorld(0) = pointDirE.getAngle().getValue("deg")(0);
  pointingCenterWorld(1) = pointDirE.getAngle().getValue("deg")(1);
  directionCoord.toPixel(pointingCenterPixel, pointingCenterWorld);
  MDirection newpointDirE;
  Vector<Double> nonSquintedPointingPixel = pointingCenterPixel.copy();

  os << "pointingCenterWorld " << pointingCenterWorld << LogIO::DEBUGGING;
  os << "pointingCenterPixel " << pointingCenterPixel << LogIO::DEBUGGING;

  // Fill in a cache of the frequencies & squints
  Vector<Double> spectralWorld(1);
  Vector<Double> spectralPixel(1);
  Matrix<Double> xSquintPixCache(2, nchan);  // kludge: prevent errors when nchan = 1
  Matrix<Double> ySquintPixCache(2, nchan);
  Vector<Double> spectralCache(nchan);

  {
    for(Int chan=0;chan<nchan;chan++) {
      spectralPixel(0)=chan;
      if(!spectralCoord.toWorld(spectralWorld, spectralPixel)) {
        os  << "Cannot find frequency for this plane" << LogIO::EXCEPTION;
      }
      spectralCache(chan)=spectralWorld(0);

      
      if (doSquint == BeamSquint::RR || doSquint == BeamSquint::GOFIGURE) {
	squint_p.getPointingDirection (pointDirE,
				       parAngle, 
				       Quantity(spectralWorld(0),"Hz"),
				       BeamSquint::RR, newpointDirE);
	pointingCenterWorld(0) = newpointDirE.getAngle().getValue("deg")(0);
	pointingCenterWorld(1) = newpointDirE.getAngle().getValue("deg")(1);
	directionCoord.toPixel(pointingCenterPixel, pointingCenterWorld);
	xSquintPixCache(0, chan) = pointingCenterPixel(0);
	ySquintPixCache(0, chan) = pointingCenterPixel(1);
      } else {
	xSquintPixCache(0, chan) =  nonSquintedPointingPixel(0);
	ySquintPixCache(0, chan) =  nonSquintedPointingPixel(1);
      }
      if (doSquint == BeamSquint::LL || doSquint == BeamSquint::GOFIGURE) {
	squint_p.getPointingDirection (pointDirE,
				       parAngle, 
				       Quantity(spectralWorld(0),"Hz"),
				       BeamSquint::LL, newpointDirE);
	pointingCenterWorld(0) = newpointDirE.getAngle().getValue("deg")(0);
	pointingCenterWorld(1) = newpointDirE.getAngle().getValue("deg")(1);
	directionCoord.toPixel(pointingCenterPixel, pointingCenterWorld);
	xSquintPixCache(1, chan) = pointingCenterPixel(0);
	ySquintPixCache(1, chan) = pointingCenterPixel(1);
      } else {
	xSquintPixCache(1, chan) =  nonSquintedPointingPixel(0);
	ySquintPixCache(1, chan) =  nonSquintedPointingPixel(1);
      }
    }
  }


  // Iterate through in minimum IO/Memory chunks
  IPosition ncs = in.niceCursorShape();
  // IPosition ncs=in.shape();
  ncs(2) = 1; ncs(3) = 1;
  ncs(2) = 1; ncs(3) = 1;
  RO_LatticeIterator<Float> li(in, LatticeStepper(in.shape(), ncs, IPosition(4,0,1,2,3) )  );
  LatticeIterator<Float> oli(out, LatticeStepper(in.shape(), ncs, IPosition(4,0,1,2,3)) );

  //Vector<Float> taper(vp_p.nelements());
  Float taper=0.0;
  Float r2=0.0;
  Float r=0.0;

  Vector<Double> increment = directionCoord.increment();
  Int rrplane = -1;
  Int llplane = -1;
  stokesCoord.toPixel( rrplane, Stokes::RR );
  stokesCoord.toPixel( llplane, Stokes::LL );

  Double xPixel;  Double yPixel;

  Int laststokes = -1;
  Int lastChan   = -1;
  Int ichan;
  Int istokes;
  Int ix0, iy0;
  Int indx;

  for(li.reset(),oli.reset();!li.atEnd();li++,oli++) {

    IPosition itsShape(li.matrixCursor().shape());
    IPosition loc(li.position());

    ichan = loc(3);
    istokes = loc(2);
    iy0 = loc(1);
    ix0 = loc(0);

    // determine the pointing: RR, LL, or Center?
    if ((doSquint == BeamSquint::RR) ||
	((doSquint == BeamSquint::GOFIGURE) && (istokes == rrplane)) ) {
      xPixel = xSquintPixCache(0, ichan);
      yPixel = ySquintPixCache(0, ichan);
    } else if ((doSquint == BeamSquint::LL) ||
	       ((doSquint == BeamSquint::GOFIGURE) && (istokes == llplane ))) {
      xPixel = xSquintPixCache(1, ichan);
      yPixel = ySquintPixCache(1, ichan);
    } else {
      xPixel = nonSquintedPointingPixel(0);
      yPixel = nonSquintedPointingPixel(1);
    }
  
    if (istokes != laststokes) {
      //      cout << "Stokes = " << istokes << " pix = " << xPixel << ", " << yPixel << endl;
      laststokes = istokes;
    }

    Double factor = 60.0 * spectralCache(ichan)/1.0e+9 ;  // arcminutes * GHz
    Double rmax2 = square( maximumRadius_p.getValue("'") / factor );
    if (wideFit_p) {
      // fill vp with interpolated values for current frequency
      if (ichan!=lastChan) {
	nearestVPArray(spectralCache(ichan));
	lastChan = ichan;
      }
    }
    
    //taper.set(0.0);
    /*for (uInt inda=0; inda < vp_p.nelements(); ++inda){
      if (norm(vp_p(inda)) > 0.0) {
	taper[inda] = real(vp_p(inda)) * real(vp_p(inda)) + imag(vp_p(inda))*imag(vp_p(inda));
	if(ipower==4)
	  taper[inda] *= taper[inda];
      } 

    } 
    */

    Vector<Float> rx2(itsShape(0));
    Vector<Float> ry2(itsShape(1));
    for(Int ix=0;ix<itsShape(0);ix++) {
      rx2(ix) =  square( increment(0)*((Double)(ix+ix0) - xPixel) );
    }
    for(Int iy=0;iy<itsShape(1);iy++) {
      ry2(iy) =  square( increment(1)*((Double)(iy+iy0) - yPixel) );
    }

    for(Int iy=0;iy<itsShape(1);iy++) {
      for(Int ix=0;ix<itsShape(0);ix++) {

	r2 =  rx2(ix) +  ry2(iy);
	if (r2 > rmax2) {
	  oli.rwMatrixCursor()(ix, iy) = 0.0;
	} else {
	  r = sqrt(r2) * factor;
	  indx = Int(r*inverseIncrementRadius_p);
	  if (norm(vp_p(indx)) > 0.0) {
	    taper = real(vp_p(indx)) * real(vp_p(indx))+ imag(vp_p(indx)) * imag(vp_p(indx));
	    if(ipower==4)
	      taper *= taper;
	  }
	  //else {
	  //  taper = 0.0;
	  //}
	  oli.rwMatrixCursor()(ix, iy) = li.matrixCursor()(ix,iy) * taper;
	}
      }
    }
  }
  return out;

};

// Behavior:  doSquint == RR or LL don't make sense here
//
//
 
SkyComponent& 
PBMath1D::apply(SkyComponent& in,
		SkyComponent& out,
		const MDirection& pointDir,
		const Quantity frequency,
		const Quantity parAngle,          
		const BeamSquint::SquintType doSquint,
		Bool inverse,
		Bool conjugate,
		Int iPower,
		Float cutoff,
		Bool /*forward*/)
{
  // if ( doSquint == NONE ) we can deal with any polarisation representation
  // if ( doSquint == GOFIGURE) an exception is thrown if polarisation is not CIRCULAR
  // if ( doSquint == RR || doSquint == LL ) an exception is thrown,
  //           as it is not valid to apply the RR or LL squint to ALL polarisations

  // Also: we can do nothing with spectral index models
  

  // convert to the EPOCH of these coords
  MDirection::Types t1 = (MDirection::Types) (in.shape().refDirection().getRef().getType());
  MDirection::Types t2 = (MDirection::Types) (pointDir.getRef().getType());

  MDirection pointDirE;
  if ( t1 != t2) {
    MDirection::Convert converter;
    converter.setOut( t1 );
    pointDirE = converter(pointDir);
  } else {
    pointDirE = pointDir;
  }

  if (doSquint == BeamSquint::RR || doSquint == BeamSquint::LL) {
    throw(AipsError("PBMath1D::apply(SkyComponent...) - cannot force a SkyComponent to have Squint RR or LL"));
  }
  if (doSquint == BeamSquint::GOFIGURE) {
    if (in.flux().pol() != ComponentType::CIRCULAR) {
      in.flux().convertPol(ComponentType::CIRCULAR);
    }
  }

  Vector<DComplex> compFluxIn = in.flux().value();
  Vector<DComplex> compFlux = out.flux().value();
  compFlux = compFluxIn.copy();

  // Find the direction of the component
  MDirection compDir = in.shape().refDirection();

  // Now taper all polarizations appropriately
  
  // Sort out any frequency interpolation
  //Int ifit=0;
  //Float lfit=0;
  //Int nFreq=wFreqs_p.nelements();
  if (wideFit_p) {
    Double freq = frequency.getValue("Hz");
    nearestVPArray(freq);
    /* for (ifit=0; ifit<nFreq; ifit++) {
      if (freq<=wFreqs_p(ifit)) break;
    }
    if (ifit>0 && ifit<nFreq) {
      lfit=(freq-wFreqs_p(ifit-1)) / (wFreqs_p(ifit)-wFreqs_p(ifit-1));
    }
    */
  }
  
  MDirection newpointDirE;
  for (Int pol=0;pol<4;pol++) {
    Stokes::StokesTypes stokes=Stokes::type(pol+5);

    if (stokes == Stokes::RR &&  doSquint == BeamSquint::GOFIGURE) {
      squint_p.getPointingDirection (pointDirE, parAngle, frequency, BeamSquint::RR,
				     newpointDirE );
    } else if (stokes == Stokes::LL && doSquint == BeamSquint::GOFIGURE) {
      squint_p.getPointingDirection (pointDirE, parAngle, frequency, BeamSquint::LL,
				     newpointDirE );
    } else {
      newpointDirE = pointDirE;
    }

    MVDirection mvd1( compDir.getAngle() );
    MVDirection mvd2( newpointDirE.getAngle() );
    Quantity sep =  mvd1.separation(mvd2, "'"); 
    double r = sep.getValue("'") * frequency.getValue("Hz") / 1.0e+9;  // arcminutes * GHz 
    Complex taper;
    Int ir = Int(r*inverseIncrementRadius_p);
    //vp_p is interpolated wvp_p from above 
    Complex vpVal = ir >= Int(vp_p.nelements()) ? Complex(0) : vp_p(ir);
    /*if (wideFit_p) {
      if (ifit==0) {
	vpVal = wbvp_p(ir,0);
      } else if (ifit==nFreq) {
	vpVal = wbvp_p(ir,nFreq-1);
      } else {
	vpVal = wbvp_p(ir,ifit-1)*(1-lfit) + wbvp_p(ir,ifit)*lfit;
      }
    }
     */
    
    if (r > maximumRadius_p.getValue("'")) {
      compFlux(pol) = 0.0;
    } else {
      if (norm(vpVal) > 0.0) {
	if(iPower>1){
	  taper=vpVal*conj(vpVal);
	  if(iPower==4)
	    taper*=taper;
	}  
	else{
	  taper = vpVal;
	  //taper = pow( vp_p(Int(r*inverseIncrementRadius_p)), (Float)iPower);
	}
      } else {
	taper = 0.0;
      }
      if (conjugate) {
	taper =  conj(taper);
      }
      if (inverse) {
	if (abs(taper) < cutoff ) {
	  compFlux(pol) = 0.0;
	} else {
	  compFlux(pol) /= taper ;
	}
      } else {  // not inverse!
	compFlux(pol) *= taper;
      }
    }    
  }

  // Set the output component fluxes 
  out = in.copy();
  out.flux().setValue(compFlux);

  return out;

};

void PBMath1D::summary(Int nValues)
{
  String  name;
  namePBClass(name);
  LogIO os(LogOrigin("PBMath1D", "summary"));
  os << "Using " << name << " PB Class " <<  LogIO::POST;
  PBMathInterface::summary(nValues);

  if (nValues > 0) {
    os << "Primary Beam Sampled Data: " <<  LogIO::POST;
    os << "  r[']      pb[@ 1 GHz] " <<  LogIO::POST;
    Vector<Float> rr;
    Vector<Float> pb;
    viewPB(rr, pb, nValues);
    for (Int ii=0;ii<nValues;ii++) { 
      os << rr(ii) << " " << pb(ii) <<  LogIO::POST; 
    }
  }
  os << "Max Radius at " <<   refFreq_p.getValue("GHz") << " GHz: " 
     << maximumRadius_p.getValue("'") << " arcmin " <<  LogIO::POST; 

};


Bool PBMath1D::ok()
{
  if (vp_p.nelements() == 0) {
    return false;
  } else if (maximumRadius_p.getValue() <= 0.0) {
    return false;
  } else if (refFreq_p.getValue() <= 0.0) {
    return false;
  } else if (inverseIncrementRadius_p <= 0.0) {
    return false;
  } else {
    return true;
  }
};


void PBMath1D::viewPB(Vector<Float>& r, Vector<Float>& pb, Int n_pb, const Double freq)
{
  r.resize(n_pb);
  pb.resize(n_pb);
  if(wideFit_p)
    nearestVPArray(freq);
  Int nSamples= vp_p.nelements();
  for (Int i=0; i< n_pb; i++) {
    pb(i) = norm( vp_p( (Int) ((nSamples-1)* (((Float)i)/(n_pb-1) ) ) ) );
    r(i) =  maximumRadius_p.getValue("'") *  (((Float)i)/(n_pb-1));
  }

};
void PBMath1D::nearestVPArray(Double freq, bool printINFO){
    LogIO os(LogOrigin("PBMATH1D", "nearestVPArray"));
    Int ifit=0;
  
  Int nFreq=wFreqs_p.nelements();
  for (ifit=0; ifit<nFreq; ifit++) {
    if (freq <=wFreqs_p(ifit)) break;
  }
  if(printINFO)
        os << LogIO::NORMAL1 << "Using beam model of frequency  " << ((ifit==nFreq) ? wFreqs_p(nFreq-1) : wFreqs_p(ifit)) << " MHz " << LogIO::POST;
  if (ifit==0) {
    vp_p = wbvp_p.column(0);
  } else if (ifit==nFreq) {
    vp_p = wbvp_p.column(nFreq-1);
  } else {
    Float l = (freq - wFreqs_p(ifit-1))/
      (wFreqs_p(ifit)-wFreqs_p(ifit-1));
    vp_p = wbvp_p.column(ifit-1)*(1-l) + wbvp_p.column(ifit)*l;
  }


}

} //# NAMESPACE CASA - END

