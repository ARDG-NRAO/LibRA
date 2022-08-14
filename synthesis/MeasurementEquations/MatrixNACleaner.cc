//# Matrix Non Amnesiac Cleaner Copyright (C) 2015
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU  General Public License as published by
//# the Free Software Foundation; either version 3 of the License, or (at your
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
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id:  $

#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/Cube.h>
#include <casacore/casa/Arrays/MaskedArray.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/MaskArrMath.h>
#include <casacore/casa/Arrays/MatrixMath.h>
#include <casacore/casa/IO/ArrayIO.h>
#include <casacore/casa/BasicMath/Math.h>
#include <casacore/casa/BasicSL/Complex.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/OS/File.h>
#include <casacore/casa/Containers/Record.h>

#include <casacore/lattices/LRegions/LCBox.h>
#include <casacore/casa/Arrays/Slicer.h>
#include <casacore/scimath/Mathematics/FFTServer.h>
#include <casacore/casa/OS/HostInfo.h>
#include <casacore/casa/Arrays/ArrayError.h>
#include <casacore/casa/Arrays/ArrayIter.h>
#include <casacore/casa/Arrays/VectorIter.h>


#include <casacore/casa/Utilities/GenSort.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/Utilities/Fallible.h>

#include <casacore/casa/BasicSL/Constants.h>

#include <casacore/casa/Logging/LogSink.h>
#include <casacore/casa/Logging/LogMessage.h>

#include <synthesis/MeasurementEquations/MatrixNACleaner.h>
#include <synthesis/MeasurementEquations/MatrixCleaner.h>
#include <casacore/coordinates/Coordinates/TabularCoordinate.h>
#ifdef _OPENMP
#include <omp.h>
#endif
using namespace std;
using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

 

  
 
MatrixNACleaner::MatrixNACleaner():
  itsMask( ),
  itsDirty( ),
  itsBitPix( ),
  itsMaximumResidual(1e100),
  itsTotalFlux(0.0),
  itsSupport(3),
  psfShape_p(0),
  itsPositionPeakPsf(0),
  itsRms(0.0),
  typeOfMemory_p(2),
  numSigma_p(5.0)
{
  
 
}

 

MatrixNACleaner::MatrixNACleaner(const Matrix<Float> & psf,
				 const Matrix<Float> &dirty, const Int memType, const Float  numSigma):
  itsBitPix( ), itsSupport(3), itsRms(0.0),typeOfMemory_p(memType), numSigma_p(numSigma)
{
  psfShape_p.resize(0, false);
  psfShape_p=psf.shape();
  // Check that everything is the same dimension and that none of the
  // dimensions is zero length.
  AlwaysAssert(psf.shape().nelements() == dirty.shape().nelements(),
	       AipsError);
  AlwaysAssert(dirty.shape().product() != 0, AipsError);
 
  itsDirty=make_shared<Matrix<Float> >();
  itsDirty->reference(dirty);
  setPsf(psf);
  itsResidual=std::make_shared<Matrix<Float> >();
  itsResidual->assign(dirty);
  itsMask= std::make_shared<Matrix<Float> >(itsDirty->shape(), Float(0.0));
  //itsRms=rms(*itsResidual);
  itsMaximumResidual=1e100;
}


  Bool MatrixNACleaner::validatePsf(const Matrix<Float> & psf)
{
  LogIO os(LogOrigin("MatrixCleaner", "validatePsf()", WHERE));
  
  // Find the peak of the raw Psf
  AlwaysAssert(psf.shape().product() != 0, AipsError);
  Float maxPsf=0;
  itsPositionPeakPsf=IPosition(psf.shape().nelements(), 0);
  Int psfSupport = max(psf.shape().asVector())/2;
  MatrixCleaner::findPSFMaxAbs(psf, maxPsf, itsPositionPeakPsf, psfSupport);
  os << "Peak of PSF = " << maxPsf << " at " << itsPositionPeakPsf
     << LogIO::POST;
  return true;
}
  

void MatrixNACleaner::setPsf(const Matrix<Float>& psf){
  AlwaysAssert(validatePsf(psf), AipsError);
  psfShape_p.resize(0, false);
  psfShape_p=psf.shape();
  itsPsf=make_shared<Matrix<Float> >();
  itsPsf->reference(psf);
  //cout << "shapes " << itsXfr->shape() << " psf " << psf.shape() << endl;
}

MatrixNACleaner::MatrixNACleaner(const MatrixNACleaner & other)
   
{
  operator=(other);
}

MatrixNACleaner & MatrixNACleaner::operator=(const MatrixNACleaner & other) {
  if (this != &other) {
    
   
    itsMask = other.itsMask;
    itsDirty = other.itsDirty;
    itsResidual=other.itsResidual;
    itsBitPix=other.itsBitPix;
    itsTotalFlux=other.itsTotalFlux;
    psfShape_p.resize(0, false);
    psfShape_p=other.psfShape_p;
    itsSupport=other.itsSupport;
    itsRms=other.itsRms;
    typeOfMemory_p=other.typeOfMemory_p;
    
  }
  return *this;
}

MatrixNACleaner::~MatrixNACleaner()
{
  
  
}




// add a mask image
  void MatrixNACleaner::setMask(Matrix<Float> & mask) 
{
 

  //cerr << "Mask Shape " << mask.shape() << endl;
  // This is not needed after the first steps
  itsMask = std::make_shared<Matrix<Float> >(mask.shape());
  itsMask->reference(mask);
 

}
 
void MatrixNACleaner::setPixFlag(const Matrix<Bool> & bitpix) 
{
 

  //cerr << "Mask Shape " << mask.shape() << endl;
  // This is not needed after the first steps
  itsBitPix = std::make_shared<Matrix<Bool> >(bitpix.shape());
  itsBitPix->reference(bitpix);
 

}


// Set up the control parameters
void MatrixNACleaner::setcontrol(
				   const Int niter,
				   const Float gain,
				   const Quantity& aThreshold,
				   const Int masksupp, const Int memType, const Float numSigma)
{
  
  itsMaxNiter=niter;
  itsGain=gain;
  itsThreshold=aThreshold;
  itsSupport=masksupp;
  typeOfMemory_p=memType;
  numSigma_p=numSigma;
  
}




// Do the clean as set up
Int MatrixNACleaner::clean(Matrix<Float>& model)
{
  AlwaysAssert(model.shape()==itsDirty->shape(), AipsError);

  LogIO os(LogOrigin("MatrixCleaner", "clean()", WHERE));


  if(typeOfMemory_p==0)
    f_p=std::bind(&MatrixNACleaner::amnesiac, this, std::placeholders::_1);
  else if(typeOfMemory_p==1)
    f_p=std::bind(&MatrixNACleaner::weak, this, std::placeholders::_1);
  else if(typeOfMemory_p==3)
    f_p=std::bind(&MatrixNACleaner::strong, this, std::placeholders::_1);
  else
    f_p=std::bind(&MatrixNACleaner::medium, this, std::placeholders::_1);
  Int converged=0;
  itsTotalFlux=0.0;
   if(!itsBitPix){
     itsRms=rms(*itsResidual);
   }else{
     itsRms=rms(MaskedArray<Float>(*itsResidual, *itsBitPix));
   }

  // Define a subregion for the inner quarter
  IPosition blcDirty(model.shape().nelements(), 0);
  IPosition trcDirty(model.shape()-1);

  if(!itsMask){
    itsMask=make_shared<Matrix<Float> >(model.shape(), Float(0.0));
  } 

    
   
 
 
  os << "Starting iteration"<< LogIO::POST;
  Float maxima=0.0;
  IPosition posMaximum;
  itsIteration = 0;
  for (Int ii=0; ii < itsMaxNiter; ii++) {
    itsIteration++;
 
	
	
	
    if(!findMaxAbsMask(*itsResidual, *itsMask,
		       maxima, posMaximum, itsSupport))
      break;

	

   
   

    // Now add to the total flux
    itsTotalFlux += (maxima*itsGain);
    


    if(ii==0 ) {
      itsMaximumResidual=abs(maxima);
      os << "Initial maximum residual is " << itsMaximumResidual;
     
      os << LogIO::POST;
    }

    // Various ways of stopping:
    //    1. stop if below threshold
    if(abs(maxima)<threshold() ) {
      os << "Reached stopping threshold " << threshold() << " at iteration "<<
            ii << LogIO::POST;
      os << "Optimum flux is " << abs(maxima) << LogIO::POST;
      converged = 1;
      break;
    }
    
    /*
    if(progress) {
      progress->info(false, itsIteration, itsMaxNiter, maxima,
		     posMaximum, itsStrengthOptimum,
		     optimumScale, positionOptimum,
		     totalFlux, totalFluxScale,
		     itsJustStarting );
      itsJustStarting = false;
      } else*/ {
      if (itsIteration ==   1) {
          os << "iteration    MaximumResidual   CleanedFlux" << LogIO::POST;
      }
      if ((itsIteration % (itsMaxNiter/10 > 0 ? itsMaxNiter/10 : 1)) == 0) {
	//Good time to redo rms if necessary
	if(abs(maxima) >  3*itsRms){
	  if(!itsBitPix){
	    itsRms=rms(*itsResidual);
	  }else{
	    itsRms=rms(MaskedArray<Float>(*itsResidual, *itsBitPix));
	  }
	}
	os << itsIteration <<"      "<< maxima<<"      "
	   << itsTotalFlux << " rms " << itsRms << LogIO::POST;
      }
    }

   

    // Continuing: subtract the peak that we found from all dirty images
    // Define a subregion so that that the peak is centered
    IPosition support(model.shape());
    

    IPosition inc(model.shape().nelements(), 1);
    //cout << "support " << support.asVector()  << endl;
    //support(0)=1024;
    //support(1)=1024;
    //support(0)=min(Int(support(0)), Int(trcDirty(0)-blcDirty(0)));
    //support(1)=min(Int(support(1)), Int(trcDirty(1)-blcDirty(1)));
    // support(0)=min(Int(support(0)), (trcDirty(0)-blcDirty(0)+
    //				Int(2*abs(positionOptimum(0)-blcDirty(0)/2.0-trcDirty(0)/2.0))));
    //support(1)=min(Int(support(1)), (trcDirty(1)-blcDirty(1)+
    //				Int(2*abs(positionOptimum(1)-blcDirty(1)/2.0-trcDirty(1)/2.0))));

    IPosition blc(posMaximum-support/2);
    IPosition trc(posMaximum+support/2-1);
    LCBox::verify(blc, trc, inc, model.shape());
    
    //cout << "blc " << blc.asVector() << " trc " << trc.asVector() << endl;

    IPosition blcPsf(blc+itsPositionPeakPsf-posMaximum);
    IPosition trcPsf(trc+itsPositionPeakPsf-posMaximum);
    LCBox::verify(blcPsf, trcPsf, inc, model.shape());
    makeBoxesSameSize(blc,trc,blcPsf,trcPsf);
    // cout << "blcPsf " << blcPsf.asVector() << " trcPsf " << trcPsf.asVector() << endl;
    //cout << "blc " << blc.asVector() << " trc " << trc.asVector() << endl;
    //    LCBox subRegion(blc, trc, model.shape());
    //  LCBox subRegionPsf(blcPsf, trcPsf, model.shape());
    
   
    Matrix<Float> resSub=(*itsResidual)(blc,trc);
    
 
    // Now do the addition to the model image....
    model(posMaximum) += itsGain*maxima;

   			
  
      
	
    Matrix<Float> psfSub=(*itsPsf)(blcPsf, trcPsf);
    resSub -= itsGain*maxima*psfSub;
	    
      
    
  }
  // End of iteration

 
    os << LogIO::NORMAL
       << "  " << "  Total Flux  " << itsTotalFlux
       << LogIO::POST;
  
  // Finish off the plot, etc.
  /*
  if(progress) {
    progress->info(true, itsIteration, itsMaxNiter, maxima, posMaximum,
		   itsStrengthOptimum,
		   optimumScale, positionOptimum,
		   totalFlux, totalFluxScale);
  }
  */

  if(!converged) {
    os << "Failed to reach stopping threshold" << LogIO::POST;
  }

  return converged;
}






Bool MatrixNACleaner::findMaxAbsMask(const Matrix<Float>& lattice,
				   Matrix<Float>& mask,
					      Float& peakval,
				   IPosition& posPeak, const Int support)
{

  /*
    Here is the secret of non masking algorithm
    find peak of mask*resid -- m1*p1   p1 (value of resid at that peak)
    find peak of resid -- p2
    if p2 < m1*p1
       return p1 
       modify box (using user given support) at pos of p1 in mask to be f(p1) or m@p1 which ever is higher
   else 
        return p2
         modify box at pos of p2 in mask to be f(p2) or m@p2  which ever is higher
    
   stop modifying mask when peak reaches 5 (or user settable) sigma
   
   f(p) is a memory function...f(p)=1 implies no memory or normal clean
   default right now we are using f(p)=p
    a weak memory can be f(p)=1+ k*p  ( k << 1)  or p ^0.1
    a strong memory can be f(p)=p^2
   */

  //cerr << "SHAPES " << lattice.shape() << "  " << mask.shape() << endl;
  Matrix<Float> wgtresid=lattice*mask;
  
  IPosition posMaxWgt = IPosition(lattice.shape().nelements(), 0);
  Float maxValWgt=0.0;
  Float minValWgt;
  IPosition posMinWgt(lattice.shape().nelements(), 0);
  if(!itsBitPix)
    minMax(minValWgt, maxValWgt, posMinWgt, posMaxWgt, wgtresid);
  else
     minMax(minValWgt, maxValWgt, posMinWgt, posMaxWgt, MaskedArray<Float>(wgtresid, *itsBitPix));
  if(abs(minValWgt) > abs(maxValWgt)){
    posMaxWgt=posMinWgt;
    maxValWgt=minValWgt;
  }
  IPosition posMaxRes = IPosition(lattice.shape().nelements(), 0);
  Float maxValRes=0.0;
  Float minValRes;
  IPosition posMinRes(lattice.shape().nelements(), 0);
  if(!itsBitPix)
    minMax(minValRes, maxValRes, posMinRes, posMaxRes, lattice);
  else
    minMax(minValRes, maxValRes, posMinRes, posMaxRes,  MaskedArray<Float>(lattice, *itsBitPix));
  if(abs(minValRes) > abs(maxValRes)){
    posMaxRes=posMinRes;
    maxValRes=minValRes;
  }
  if(abs(maxValWgt) > abs(maxValRes)){
    posMaxRes=posMaxWgt;
    maxValRes=lattice(posMaxRes);
  }
  posPeak=posMaxRes;
  peakval=maxValRes;
  Int nx=lattice.shape()(0);
  Int ny=lattice.shape()(1);
  Float apeakval=abs(peakval);
  if(apeakval > numSigma_p*itsRms){
    for (Int y=-support; y < support+1; ++y){
      if(((posPeak[1]+y) >=0) && ((posPeak[1]+y) < ny)){
	for(Int x=-support; x < support+1; ++x){
	  if(((posPeak[0]+x) >=0) && ((posPeak[0]+x) < nx))
	    if(mask(x+posPeak[0], y+posPeak[1]) < f_p(apeakval))
	      mask(x+posPeak[0], y+posPeak[1])=f_p(apeakval);
	}
      }
    }
  }
  itsMaximumResidual=peakval;
  //if(peakval > 2*itsRms)
  //  return true;

  return true;
}
  Float  MatrixNACleaner::amnesiac(const Float& ){
    return 1.0;
  }

  Float MatrixNACleaner::weak(const Float& v){
    return 1.0+0.1*v;
  }

  Float MatrixNACleaner::medium(const Float& v){
    return v;

  }
  Float MatrixNACleaner::strong(const Float& v){
    return v*v;
  }


void MatrixNACleaner::setDirty(const Matrix<Float>& dirty){

  itsDirty=make_shared<Matrix<Float> >(dirty.shape());
  itsDirty->reference(dirty);
   itsResidual=make_shared<Matrix<Float> >(dirty.shape());
  itsResidual->assign(dirty);
  itsRms=rms(*itsResidual);
  
  
  

} 

//Define the scales without doing anything else
// user will call make makePsfScales and makeDirtyScales like an adult in the know




void MatrixNACleaner::unsetMask()
{
 
  if(!itsMask)
    itsMask=nullptr;
  
}








Float MatrixNACleaner::threshold() const
{
  return Float(itsThreshold.getValue("Jy"));
}



void MatrixNACleaner::makeBoxesSameSize(IPosition& blc1, IPosition& trc1, 
                  IPosition &blc2, IPosition& trc2)
{
  const IPosition shape1 = trc1 - blc1;
  const IPosition shape2 = trc2 - blc2;

  AlwaysAssert(shape1.nelements() == shape2.nelements(), AipsError);
  
  if (shape1 == shape2) {
      return;
  }
  for (uInt i=0;i<shape1.nelements();++i) {
       Int minLength = shape1[i];
       if (shape2[i]<minLength) {
           minLength = shape2[i];
       }
       AlwaysAssert(minLength>=0, AipsError);
       //if (minLength % 2 != 0) {
           // if the number of pixels is odd, ensure that the centre stays 
           // the same by making this number even
           //--minLength; // this code is a mistake and should be removed
       //}
       const Int increment1 = shape1[i] - minLength;
       const Int increment2 = shape2[i] - minLength;
       blc1[i] += increment1/2;
       trc1[i] -= increment1/2 + (increment1 % 2 != 0 ? 1 : 0);
       blc2[i] += increment2/2;
       trc2[i] -= increment2/2 + (increment2 % 2 != 0 ? 1 : 0);
  }
}



} //# NAMESPACE CASA - END

