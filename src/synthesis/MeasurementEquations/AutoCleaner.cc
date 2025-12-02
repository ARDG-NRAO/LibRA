#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/Cube.h>
#include <casacore/casa/Arrays/ArrayMath.h>
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
#include <casacore/casa/Arrays/ArrayUtil.h>
#include <casacore/tables/TaQL/MArray.h>
#include <casacore/casa/BasicMath/Functors.h>

#include <casacore/casa/Utilities/GenSort.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/Utilities/Fallible.h>

#include <casacore/casa/BasicSL/Constants.h>

#include <casacore/casa/Logging/LogSink.h>
#include <casacore/casa/Logging/LogMessage.h>

#include <synthesis/MeasurementEquations/AutoCleaner.h>
#include <casacore/coordinates/Coordinates/TabularCoordinate.h>
#include <casacore/casa/Utilities/CountedPtr.h>
#ifdef _OPENMP
#include <omp.h>
#endif
using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

Bool AutoCleaner::validatePsf(const Matrix<Float> & psf)
{
	LogIO os(LogOrigin("AutoCleaner", "validatePsf()", WHERE));

	//Find the peak of the raw psf
	AlwaysAssert(psf.shape().product() != 0, AipsError);
	Float maxPsf=0;
	itsPositionPeakPsf=IPosition(psf.shape().nelements(), 0);
	Int psfSupport = findBeamPatch(0.0,psf.shape()(0), psf.shape()(1), 4.0, 20.0);
	findPSFMaxAbs(psf, maxPsf, itsPositionPeakPsf, psfSupport);
	os << "Peak of PSF = " << maxPsf << " at " << itsPositionPeakPsf << LogIO::POST;
	return true;
}

AutoCleaner::AutoCleaner():
  itsMask( ),
  itsMaskThreshold(0.9),
  itsDirty( ),
  itsMaximumResidual(0.0),
  itsStrengthOptimum(0.0),
  itsTotalFlux(0.0),
  itsChoose(true),
  itsDoSpeedup(false),
  itsIgnoreCenterBox(false),
  psfShape_p(0),
  noClean_p(false)
{
  itsMemoryMB=Double(HostInfo::memoryTotal()/1024)/16.0;
  itsStartingIter = 0;
  itsFracThreshold=Quantity(0.0, "%");
}

AutoCleaner::AutoCleaner(const Matrix<Float> & psf,
				  const Matrix<Float> &dirty):
  itsMask( ),
  itsMaximumResidual(0.0),
  itsStrengthOptimum(0.),
  itsTotalFlux(0.0),
  itsChoose(true),
  itsDoSpeedup(false),
  itsIgnoreCenterBox(false),
  itsdimensionsareeven(true),
  noClean_p(false)
{
  AlwaysAssert(validatePsf(psf), AipsError);
  psfShape_p.resize(0, false);
  psfShape_p=psf.shape();
  // Check that everything is the same dimension and that none of the
  // dimensions is zero length.
  AlwaysAssert(psf.shape().nelements() == dirty.shape().nelements(),
	       AipsError);
  AlwaysAssert(dirty.shape().product() != 0, AipsError);
  // looks OK so make the convolver
  
  // We need to guess the memory use. For the moment, we'll assume
  // that about 4 scales will be used, giving about 32 TempLattices
  // in all. Also we'll try not to take more that half of the memory

  // Ah, but when we are doing a mosaic, its actually worse than this!
  // So, we pass it in
  itsMemoryMB=Double(HostInfo::memoryTotal()/1024)/16.0;

  Matrix<Float> itsDirty;
  itsDirty.resize(psf.shape());
  itsDirty.assign(dirty);
  setPsf(psf);
  itsStartingIter = 0;
  itsFracThreshold=Quantity(0.0, "%");
}

void AutoCleaner::setPsf(const Matrix<Float>& psf){
  LogIO os(LogOrigin("AutoCleaner", "setPsf()", WHERE));
  AlwaysAssert(validatePsf(psf), AipsError);
  psfShape_p.resize(0, false);
  psfShape_p = psf.shape();
  itsPsf.resize(psf.shape());
  itsPsf.assign(psf);

  cmap.resize(psf.shape());
  mod.resize(psf.shape());
  delta.resize(psf.shape());

  shifted.resize(psf.shape());
  clean_map.resize(psf.shape());

  BB.resize(psf.shape());
  tildeII.resize(psf.shape());
  BI.resize(psf.shape());
  basis_function.resize(psf.shape());
  tildeMB.resize(psf.shape());
  mspsf.resize(psf.shape());
  tildeMI.resize(psf.shape());
  tildeMBB.resize(psf.shape());
  MtildeMBB.resize(psf.shape());
  tildeMBI.resize(psf.shape());
  MtildeMB.resize(psf.shape());
  window_basis.resize(psf.shape());

  cmap=0.0;
  mod=0.0;
  delta=0.0;

  shifted=0.0;
  clean_map=0.0;

  BB=0.0;
  tildeII=0.0;
  BI=0.0;
  basis_function=0.0;
  tildeMB=0.0;
  mspsf=0.0;
  tildeMI=0.0;
  tildeMBB=0.0;
  MtildeMBB=0.0;
  tildeMBI=0.0;
  MtildeMB=0.0;
  window_basis=0.0;

  normcmap=0.0;
  normcmapsq=0.0;
  sumcmap=0.0;
  blcautomask=itsDirty.shape();
  trcautomask=itsDirty.shape();
  itsAutoThreshold=0.0;
  itsAutoMaxiter=0;
  itsAutoGain=0.0;
  itsAutoHogbom=false;
  itsAutoTrigger=1.0;
  triggerhogbom=0.0;
  itsAutoPower=1.0;
  itsmaxbeam=1.0;
}

AutoCleaner::AutoCleaner(const AutoCleaner & other)
   
{
  operator=(other);
}

AutoCleaner & AutoCleaner::operator=(const AutoCleaner & other) {
  if (this != &other) {
    itsCleanType = other.itsCleanType;
    itsMask = other.itsMask;
    itsDirty = other.itsDirty;
    itsScales = other.itsScales;
    itsStartingIter = other.itsStartingIter;
    itsMaximumResidual = other.itsMaximumResidual;
    itsIgnoreCenterBox = other.itsIgnoreCenterBox;
    itsStrengthOptimum = other.itsStrengthOptimum;
    itsTotalFlux=other.itsTotalFlux;
    itsMaskThreshold = other.itsMaskThreshold;
    psfShape_p.resize(0, false);
    psfShape_p=other.psfShape_p;
    noClean_p=other.noClean_p;
  }
  return *this;
}

AutoCleaner::~AutoCleaner()
{
  if(!itsMask.null()) itsMask=0;
}

void AutoCleaner::update(const Matrix<Float> &dirty)
{
  //itsDirty->assign(dirty);
  itsDirty.assign(dirty);

  LogIO os(LogOrigin("AutoCleaner", "update()", WHERE));

// here we need to reupdate the other matrices
 
}

// add a mask image
void AutoCleaner::setMask(Matrix<Float> & mask, const Float& maskThreshold) 
{
  itsMaskThreshold = maskThreshold;
  IPosition maskShape = mask.shape();

  //cerr << "Mask Shape " << mask.shape() << endl;
  // This is not needed after the first steps
  itsMask = new Matrix<Float>(mask.shape());
  itsMask->assign(mask);
  noClean_p=(max(*itsMask) < itsMaskThreshold) ? true : false;

}

Bool AutoCleaner::setcontrol(CleanEnums::CleanType cleanType,
				   const Int niter,
				   const Float gain,
				   const Quantity& threshold)
{
  return setcontrol(cleanType, niter, gain, threshold, Quantity(0.0, "%"));
}

// Set up the control parameters
Bool AutoCleaner::setcontrol(CleanEnums::CleanType cleanType,
				   const Int niter,
				   const Float gain,
				   const Quantity& aThreshold,
				   const Quantity& fThreshold)
{
  itsCleanType=cleanType;
  itsMaxNiter=niter;
  itsGain=gain;
  itsThreshold=aThreshold;
  itsFracThreshold=fThreshold;
  return true;
}

// Set up speedup parameters
void AutoCleaner::speedup(const Float nDouble)
{
  itsDoSpeedup=true;
  itsNDouble = nDouble;
};

// Do the clean as set up
Int AutoCleaner::clean(Matrix<Float>& model,
                         Bool /*showProgress*/)
{
  LogIO os(LogOrigin("AutoCleaner", "clean()", WHERE));

  AlwaysAssert(model.shape()==itsDirty.shape(), AipsError);
  itsdimensionsareeven = (model.shape()(0) == 2*(model.shape()(0)/2));
  AlwaysAssert((itsdimensionsareeven==false) || itsAutoHogbom, AipsError);

  Float tmpMaximumResidual=0.0;

  // Define a subregion for the inner quarter
  IPosition blcDirty(model.shape().nelements(), 0);
  IPosition trcDirty(model.shape()-1);

  // Handle Mask
  if(!itsMask.null()){
    os << "Cleaning using given mask" << LogIO::POST;
    if (itsMaskThreshold<0) {
        os << LogIO::NORMAL
           << "Mask thresholding is not used, values are interpreted as weights"
           <<LogIO::POST;
    } else {
      // a mask that does not allow for clean was sent
      if(noClean_p)
	return 0;
        os << LogIO::NORMAL
           << "Cleaning pixels with mask values above " << itsMaskThreshold
           << LogIO::POST;
    }

    Int nx=model.shape()(0);
    Int ny=model.shape()(1);
        
    AlwaysAssert(itsMask->shape()(0)==nx, AipsError);
    AlwaysAssert(itsMask->shape()(1)==ny, AipsError);    
    Int xbeg=nx-1;
    Int ybeg=ny-1;
    Int xend=0;
    Int yend=0;
    for (Int iy=0;iy<ny;iy++) {
      for (Int ix=0;ix<nx;ix++) {
	if((*itsMask)(ix,iy)>0.000001) {
	  xbeg=min(xbeg,ix);
	  ybeg=min(ybeg,iy);
	  xend=max(xend,ix);
	  yend=max(yend,iy);
	}
      }
    }
    
    if (!itsIgnoreCenterBox) {
      if((xend - xbeg)>nx/2) {
	xbeg=nx/4-1; //if larger than quarter take inner of mask
	os << LogIO::WARN << "Mask span over more than half the x-axis: Considering inner half of the x-axis"  << LogIO::POST;
      } 
      if((yend - ybeg)>ny/2) { 
	ybeg=ny/4-1;
	os << LogIO::WARN << "Mask span over more than half the y-axis: Considering inner half of the y-axis" << LogIO::POST;
      }  
      xend=min(xend,xbeg+nx/2-1);
      yend=min(yend,ybeg+ny/2-1);
    }

    //blcDirty(0)=xbeg> 0 ? xbeg-1 : 0;
    //blcDirty(1)=ybeg > 0 ? ybeg-1 : 0;
    blcDirty(0)=xbeg;
    blcDirty(1)=ybeg;
    trcDirty(0)=xend;
    trcDirty(1)=yend;
  }
  else {
    if (itsIgnoreCenterBox) {
      os << LogIO::NORMAL << "Cleaning entire image" << LogIO::POST;
      os << LogIO::NORMAL1 << "as per MF/WF" << LogIO::POST; // ???
    }
    else {
      os << "Cleaning inner quarter of the image" << LogIO::POST;
      for (Int i=0;i<Int(model.shape().nelements());i++) {
	blcDirty(i)=model.shape()(i)/4;
	trcDirty(i)=blcDirty(i)+model.shape()(i)/2-1;
	if(trcDirty(i)<0) trcDirty(i)=1;
      }
    }
  }
  LCBox centerBox(blcDirty, trcDirty, model.shape());

  // Start the iteration
  //Float maximum;
  IPosition posMaximum(model.shape().nelements(), 0);
  IPosition posMaximumlast(model.shape().nelements(), 0);
  IPosition posMaximumHogbom(model.shape().nelements(),0);
  Float MaximumHogbom;
  Float StrengthOptimumlast;
  Float totalFlux=0.0;
  Int converged=0;
  Int stopPointModeCounter = 0;
  //Float itsStrengthOptimum=0.0;
  os << "Starting iteration"<< LogIO::POST;
  
  //
  Int nx=model.shape()(0);
  Int ny=model.shape()(1);
  IPosition gip; 
  gip = IPosition(2,nx,ny);  
  casacore::Matrix<casacore::Float> vecWork_p;
  vecWork_p.resize(gip);
  casacore::Matrix<casacore::Float> vecWork_pms;
  vecWork_pms.resize(gip);
  Float tempGain;
  Bool cleanhogbom=false;
  //

  itsIteration = itsStartingIter;
  for (Int ii=itsStartingIter; ii < itsMaxNiter; ii++) {

    itsIteration++;
    // Find the peak residual
    itsStrengthOptimum = 0.0;

    // Find absolute maximum for the dirty image
    Matrix<Float> work = vecWork_p(blcDirty,trcDirty);   
    work = 0.0;
    work = work + itsDirty(blcDirty,trcDirty);

    if (!itsMask.null()) {
	    findMaxAbsMask(vecWork_p, *itsMask, MaximumHogbom, posMaximumHogbom);
    } else {
	    findMaxAbs(vecWork_p, MaximumHogbom, posMaximumHogbom);
	  }

    // Find absolute maximum for the ms dirty image
    Matrix<Float> workms = vecWork_pms(blcDirty,trcDirty);  
    workms = 0.0;
    workms = workms + tildeMI(blcDirty,trcDirty);

    if (!itsMask.null()) {
	    findMaxAbsMask(vecWork_pms, *itsMask, itsStrengthOptimum, posMaximum);
    } else {
	    findMaxAbs(vecWork_pms, itsStrengthOptimum, posMaximum);
	  }

    if(abs(MaximumHogbom) > abs(itsStrengthOptimum)/normcmap || itsAutoHogbom){
	itsStrengthOptimum = MaximumHogbom;
	posMaximum = posMaximumHogbom;
        tempGain=itsGain/max(itsPsf);
        cleanhogbom=true;
        totalFlux += (itsStrengthOptimum*tempGain);
        os << "Using Hogbom-CLEAN step"<< LogIO::POST;
    }
    else{
        tempGain=itsGain/max(tildeMB);
        cleanhogbom=false;
	if (abs(itsStrengthOptimum)*max(tildeMB)/max(MtildeMB) > abs(itsDirty(posMaximum))){
		itsStrengthOptimum*=max(tildeMB)/max(MtildeMB);
                }
        else{
                itsStrengthOptimum=itsDirty(posMaximum);
		}
        //totalFlux += (itsStrengthOptimum*tempGain*sumcmap);
        os << "Using MS-CLEAN step"<< LogIO::POST;
    }

    //trigger hogbom if we are stuck in a minimum
    if(cleanhogbom==false){
	if(posMaximum == posMaximumlast){
		if(abs(abs(itsStrengthOptimum)-abs(StrengthOptimumlast))<0.1*abs(itsStrengthOptimum) && abs(itsStrengthOptimum+StrengthOptimumlast)<0.1*abs(itsStrengthOptimum)){
			os << "We trigger Hogbom step instead, since the MS-CLEAN step got stuck" << LogIO::POST;
			itsStrengthOptimum = MaximumHogbom;
			posMaximum = posMaximumHogbom;
			tempGain=itsGain/max(itsPsf);
			cleanhogbom=true;
			totalFlux += (itsStrengthOptimum*tempGain);
			os << "Using Hogbom-CLEAN step"<< LogIO::POST;
		}
	}
    }

    if(cleanhogbom==false){
	totalFlux += (itsStrengthOptimum*tempGain*sumcmap);
    }

    // Now add to the total flux
    itsTotalFlux=totalFlux;

    //
    Float scaleFactor;
    scaleFactor=tempGain*itsStrengthOptimum;

    // Continuing: subtract the peak that we found from all dirty images
    // Define a subregion so that that the peak is centered
    IPosition support(model.shape());
    
    IPosition inc(model.shape().nelements(), 1);
    IPosition blc(posMaximum-support/2);
    IPosition trc(posMaximum+support/2); 
    LCBox::verify(blc, trc, inc, model.shape());
   
    IPosition blcPsf(blc+itsPositionPeakPsf-posMaximum);
    IPosition trcPsf(trc+itsPositionPeakPsf-posMaximum);
    LCBox::verify(blcPsf, trcPsf, inc, model.shape());
    makeBoxesSameSize(blc,trc,blcPsf,trcPsf);
    if (itsdimensionsareeven){
	    //blc(0) = posMaximum(0)-support(0)/2;
	    //blc(1) = posMaximum(1)-support(1)/2;
	    //trc(0) = posMaximum(0)-support(0)/2-1;
	    //trc(1) = posMaximum(1)-support(1)/2-1;
	    blc(posMaximum-support/2);
	    trc(posMaximum-support/2-1);
	    LCBox::verify(blc, trc, inc, model.shape());
	   
	    //blcPsf(0) = blc(0)+itsPositionPeakPsf(0)-posMaximum(0);
	    //blcPsf(1) = blc(1)+itsPositionPeakPsf(1)-posMaximum(1);
	    //trcPsf(0) = trc(0)+itsPositionPeakPsf(0)-posMaximum(0);
	    //trcPsf(1) = trc(1)+itsPositionPeakPsf(1)-posMaximum(1);
	    blcPsf(blc+itsPositionPeakPsf-posMaximum);
	    trcPsf(trc+itsPositionPeakPsf-posMaximum);
	    LCBox::verify(blcPsf, trcPsf, inc, model.shape());
	    makeBoxesSameSize(blc,trc,blcPsf,trcPsf);
    }

    // The inverse shift indices
    IPosition blcconj(support/2-posMaximum);
    IPosition trcconj(support/2+support-posMaximum-1); 
    LCBox::verify(blcconj, trcconj, inc, model.shape());

    IPosition blcPsfconj(blcconj+itsPositionPeakPsf+posMaximum-support+1);
    IPosition trcPsfconj(trcconj+itsPositionPeakPsf+posMaximum-support+1);
    LCBox::verify(blcPsfconj, trcPsfconj, inc, model.shape());
    makeBoxesSameSize(blcconj,trcconj,blcPsfconj,trcPsfconj);
    
    if(itsAutoHogbom){
	subtractBeam(itsDirty, itsPsf, blc, trc, blcPsf, trcPsf, scaleFactor, false, false);  
	subtractBeam(model, itsScales, blc, trc, blcPsf, trcPsf, scaleFactor, false, true); 
    }
    else{
	    if(cleanhogbom){
		    triggerhogbom += itsGain;
		    Matrix<Float> BIold(BI.shape());
		    BIold.assign_conforming(BI);
			Matrix<Float> tildeII_copy1(tildeII.shape()), tildeII_copy2(tildeII.shape());
			tildeII_copy1 = 0;
			tildeII_copy2 = 0;
		    #pragma omp parallel default(shared) num_threads(7)
		    {
			 #pragma omp master
			 {

			    #pragma omp task
			    tildeII += scaleFactor*scaleFactor*BB;

			    //Matrix<Float> tildeIISubc=tildeII(blcconj, trcconj);
			    ////Matrix<Float> tildeIIscaleSubc=tildeMBI(blcPsfconj,trcPsfconj);
			    //tildeIISubc -= scaleFactor*tildeMBI(blcPsfconj,trcPsfconj);//tildeIIscaleSubc; 
			    #pragma omp task
			    subtractBeam(tildeII_copy1, BIold, blcconj, trcconj, blcPsfconj, trcPsfconj, scaleFactor, false, false);   

			    //Matrix<Float> tildeIISub=tildeII(blc, trc);
			    ////Matrix<Float> tildeIIscaleSubr = reverseArray(reverseArray(tildeMBI, 0), 1)(blcPsf,trcPsf);
			    //tildeIISub -= scaleFactor*reverseArray(reverseArray(tildeMBI, 0), 1)(blcPsf,trcPsf);//tildeIIscaleSubr;
			    #pragma omp task    
			    subtractBeam(tildeII_copy2, BIold, blc, trc, blcPsf, trcPsf, scaleFactor, true, false);

				#pragma omp taskwait
				#pragma omp task
				tildeII += tildeII_copy1+tildeII_copy2;   

			    //Matrix<Float> tildeMBISub=tildeMBI(blc, trc);
			    ////Matrix<Float> tildeMBIscaleSub=MtildeMBB(blcPsf,trcPsf);
			    //tildeMBISub -= scaleFactor*MtildeMBB(blcPsf,trcPsf);//tildeMBIscaleSub;
			    //#pragma omp taskwait
			    #pragma omp task
			    subtractBeam(tildeMBI, tildeMBB, blc, trc, blcPsf, trcPsf, scaleFactor, false, false); 

			    //Matrix<Float> dirtySub=itsDirty(blc,trc);
			    ////Matrix<Float> psfSub=tildeMB(blcPsf, trcPsf);
			    //dirtySub -= scaleFactor*tildeMB(blcPsf, trcPsf);//psfSub;
			    #pragma omp task
			    subtractBeam(itsDirty, itsPsf, blc, trc, blcPsf, trcPsf, scaleFactor, false, false);   

			    //Matrix<Float> tildeMISub=tildeMI(blc, trc);
			    ////Matrix<Float> tildeMIscaleSub=MtildeMB(blcPsf,trcPsf);
			    //tildeMISub -= scaleFactor*MtildeMB(blcPsf,trcPsf);//tildeMIscaleSub;
			    #pragma omp task
			    subtractBeam(tildeMI, tildeMB, blc, trc, blcPsf, trcPsf, scaleFactor, false, false);

			    //Matrix<Float> BISub=BI(blc, trc);
			    ////Matrix<Float> BIscaleSub=tildeMBB(blcPsf,trcPsf);
			    //BISub -= scaleFactor*tildeMBB(blcPsf,trcPsf);//BIscaleSub;
			    #pragma omp task
			    subtractBeam(BI, BB, blc, trc, blcPsf, trcPsf, scaleFactor, false, false);   

			    //// Now do the addition of this scale to the model image....
			    ////Matrix<Float> clean_mapSub=clean_map(blc, trc);
			    ////Matrix<Float> clean_mapscaleSub=mspsf(blcPsf,trcPsf);
			    ////clean_mapSub += scaleFactor*clean_mapscaleSub;

			    //Matrix<Float> modelSub=model(blc, trc);
			    ////Matrix<Float> scaleSub=itsScales(blcPsf,trcPsf); //itsScale needs to be defined and is delta function, but there should be faster option here
			    //modelSub += scaleFactor*itsScales(blcPsf,trcPsf);//scaleSub;
			    #pragma omp task
			    subtractBeam(model, itsScales, blc, trc, blcPsf, trcPsf, scaleFactor/itsmaxbeam, false, true);   
			 }
		    }
	    }
	    else{    
        triggerhogbom = 0.0;
		Matrix<Float> tildeMBIold(tildeMBI.shape());
		tildeMBIold.assign_conforming(tildeMBI);
		Matrix<Float> tildeII_copy1(tildeII.shape()), tildeII_copy2(tildeII.shape());
		tildeII_copy1 = 0;
		tildeII_copy2 = 0;
		#pragma omp parallel default(shared) num_threads(7)
		    {
			 #pragma omp master
			 {

			    #pragma omp task
			    tildeII += scaleFactor*scaleFactor*MtildeMBB;

			    //Matrix<Float> tildeIISubc=tildeII(blcconj, trcconj);
			    ////Matrix<Float> tildeIIscaleSubc=tildeMBI(blcPsfconj,trcPsfconj);
			    //tildeIISubc -= scaleFactor*tildeMBI(blcPsfconj,trcPsfconj);//tildeIIscaleSubc; 
			    #pragma omp task
			    subtractBeam(tildeII_copy1, tildeMBIold, blcconj, trcconj, blcPsfconj, trcPsfconj, scaleFactor, false, false);   

			    //Matrix<Float> tildeIISub=tildeII(blc, trc);
			    ////Matrix<Float> tildeIIscaleSubr = reverseArray(reverseArray(tildeMBI, 0), 1)(blcPsf,trcPsf);
			    //tildeIISub -= scaleFactor*reverseArray(reverseArray(tildeMBI, 0), 1)(blcPsf,trcPsf);//tildeIIscaleSubr;
			    #pragma omp task    
			    subtractBeam(tildeII_copy2, tildeMBIold, blc, trc, blcPsf, trcPsf, scaleFactor, true, false);   

				#pragma omp taskwait
				#pragma omp task
				tildeII += tildeII_copy1+tildeII_copy2;

			    //Matrix<Float> tildeMBISub=tildeMBI(blc, trc);
			    ////Matrix<Float> tildeMBIscaleSub=MtildeMBB(blcPsf,trcPsf);
			    //tildeMBISub -= scaleFactor*MtildeMBB(blcPsf,trcPsf);//tildeMBIscaleSub;
			    #pragma omp task
			    subtractBeam(tildeMBI, MtildeMBB, blc, trc, blcPsf, trcPsf, scaleFactor, false, false); 

			    //Matrix<Float> dirtySub=itsDirty(blc,trc);
			    ////Matrix<Float> psfSub=tildeMB(blcPsf, trcPsf);
			    //dirtySub -= scaleFactor*tildeMB(blcPsf, trcPsf);//psfSub;
			    #pragma omp task
			    subtractBeam(itsDirty, tildeMB, blc, trc, blcPsf, trcPsf, scaleFactor, false, false);   

			    //Matrix<Float> tildeMISub=tildeMI(blc, trc);
			    ////Matrix<Float> tildeMIscaleSub=MtildeMB(blcPsf,trcPsf);
			    //tildeMISub -= scaleFactor*MtildeMB(blcPsf,trcPsf);//tildeMIscaleSub;
			    #pragma omp task
			    subtractBeam(tildeMI, MtildeMB, blc, trc, blcPsf, trcPsf, scaleFactor, false, false);

			    //Matrix<Float> BISub=BI(blc, trc);
			    ////Matrix<Float> BIscaleSub=tildeMBB(blcPsf,trcPsf);
			    //BISub -= scaleFactor*tildeMBB(blcPsf,trcPsf);//BIscaleSub;
			    #pragma omp task
			    subtractBeam(BI, tildeMBB, blc, trc, blcPsf, trcPsf, scaleFactor, false, false);   

			    //// Now do the addition of this scale to the model image....
			    ////Matrix<Float> clean_mapSub=clean_map(blc, trc);
			    ////Matrix<Float> clean_mapscaleSub=mspsf(blcPsf,trcPsf);
			    ////clean_mapSub += scaleFactor*clean_mapscaleSub;

			    //Matrix<Float> modelSub=model(blc, trc);
			    ////Matrix<Float> scaleSub=itsScales(blcPsf,trcPsf); //itsScale needs to be defined and is delta function, but there should be faster option here
			    //modelSub += scaleFactor*itsScales(blcPsf,trcPsf);//scaleSub;
			    #pragma omp task
			    subtractBeam(model, cmap, blc, trc, blcPsf, trcPsf, scaleFactor/itsmaxbeam, false, true);   
			 }
		    }
	    }

	    updateBasisFunction();

	    blcDirty = blc;
	    trcDirty = trc;
	 
	    //update the last stored position for multiscale variant
	    if(cleanhogbom==false){
		    posMaximumlast = posMaximum;
		    StrengthOptimumlast = itsStrengthOptimum;
	    }
    }

    os << itsDirty(posMaximum) << "   " << MaximumHogbom-scaleFactor*max(itsPsf) << LogIO::POST;
    os << itsIteration << "   " << itsStrengthOptimum << "   " << totalFlux << LogIO::POST;
    if(triggerhogbom>itsAutoTrigger){
        os << "we switch to Hogbom CLEAN permanently" << LogIO::POST;
	itsAutoHogbom=true;
    }
  }
    return 1;
  // End of iteration
}

  Bool AutoCleaner::findPSFMaxAbs(const Matrix<Float>& lattice,
				    Float& maxAbs,
				    IPosition& posMaxAbs,
				    const Int& supportSize)
  {
    LogIO os(LogOrigin("AutoCleaner", "findPSFMaxAbs()", WHERE));
    posMaxAbs = IPosition(lattice.shape().nelements(), 0);
    maxAbs=0.0;

    Float maxVal=0.0;
    IPosition psf2DShape(lattice.shape());
    Int blc0 = (psf2DShape(0) > supportSize) ? psf2DShape(0)/2 - supportSize/2 : 0;
    Int   blc1 = (psf2DShape(1) > supportSize) ? psf2DShape(1)/2 - supportSize/2 : 0;
    Int trc0 = (psf2DShape(0) > supportSize) ? psf2DShape(0)/2 + supportSize/2 : psf2DShape(0)-1;

    Int trc1 = (psf2DShape(1) > supportSize) ? (psf2DShape(1)/2 + supportSize/2) : (psf2DShape(1)-1) ;


    //       cerr  << "####### " << blc0 << " " << blc1 << " " << trc0 << " " << trc1 << endl;
    // cerr << "Max of lattice " << max(lattice) << " min " << min(lattice) << endl; 
    for (Int j=blc1; j < trc1; ++j)
      for (Int i=blc0 ; i < trc0; ++i)
        if ((maxAbs = abs(lattice(i,j))) > maxVal)
          {
            maxVal = maxAbs;
            posMaxAbs(0)=i; posMaxAbs(1)=j;
          }
    maxAbs=maxVal;
    //cerr << "######## " << posMaxAbs << " " << maxVal << endl;
    return true;
  }

Bool AutoCleaner::findMaxAbs(const Matrix<Float>& lattice,
					  Float& maxAbs,
					  IPosition& posMaxAbs)
{

  posMaxAbs = IPosition(lattice.shape().nelements(), 0);
  maxAbs=0.0;

  Float minVal;
  IPosition posmin(lattice.shape().nelements(), 0);
  minMax(minVal, maxAbs, posmin, posMaxAbs, lattice);
  //cout << "min " << minVal << "  " << maxAbs << "   " << max(lattice) << endl;
  if(abs(minVal) > abs(maxAbs)){
    maxAbs=minVal;
    posMaxAbs=posmin;
  }
  return true;
}

Bool AutoCleaner::findMaxAbsMask(const Matrix<Float>& lattice,
					      const Matrix<Float>& mask,
					      Float& maxAbs,
					      IPosition& posMaxAbs)
{

  posMaxAbs = IPosition(lattice.shape().nelements(), 0);
  maxAbs=0.0;
  Float minVal;
  IPosition posmin(lattice.shape().nelements(), 0);
  minMaxMasked(minVal, maxAbs, posmin, posMaxAbs, lattice, mask);
  if(abs(minVal) > abs(maxAbs)){
    maxAbs=minVal;
    posMaxAbs=posmin;
  }
 
  return true;
}

void AutoCleaner::initializeCorrProducts()
{
  //BB = correlate(itsPsf, itsPsf);
  //tildeII = correlate(itsDirty, itsDirty);
  //BI = correlate(itsPsf, itsDirty);

  LogIO os(LogOrigin("AutoCleaner", "initializeCorrProducts()", WHERE));
  //AlwaysAssert(!itsDirty.null(), AipsError);

  Float max_beam = max(itsPsf);
  itsPsf /= max_beam;

  FFTServer<Float,Complex> fft(itsDirty.shape());
  Matrix<Complex> FTPsf;
  Matrix<Complex> FTDirty;
  Matrix<Complex> cWork;

  fft.fft0(FTPsf, itsPsf);
  cWork=((FTPsf)*(conj(FTPsf)));
  fft.fft0(BB, cWork, false);
  fft.flip(BB, false, false);

  max_beam = max(BB);
  BB /= max_beam;
  itsPsf /= sqrt(max_beam);
  itsmaxbeam = sqrt(max_beam);

  fft.fft0(FTDirty, itsDirty);
  cWork=((FTDirty)*(conj(FTDirty)));
  fft.fft0(tildeII, cWork, false);
  fft.flip(tildeII, false, false);

  cWork=((FTDirty)*(FTPsf));
  fft.fft0(BI, cWork, false);
  fft.flip(BI, false, false);

  BI /= sqrt(max_beam);

  os << "Correlation Products initialized" << LogIO::POST;
}

void AutoCleaner::reinitializeCorrProducts()
{
  //BB = correlate(itsPsf, itsPsf);
  //tildeII = correlate(itsDirty, itsDirty);
  //BI = correlate(itsPsf, itsDirty);

  LogIO os(LogOrigin("AutoCleaner", "reinitializeCorrProducts()", WHERE));
  //AlwaysAssert(!itsDirty.null(), AipsError);

  Float max_beam = max(itsPsf);
  itsPsf /= max_beam;

  FFTServer<Float,Complex> fft(itsDirty.shape());
  Matrix<Complex> FTPsf;
  Matrix<Complex> FTDirty;
  Matrix<Complex> FTcmap;
  Matrix<Complex> FTBI;
  Matrix<Complex> cWork;

  fft.fft0(FTPsf, itsPsf);
  cWork=((FTPsf)*(conj(FTPsf)));
  fft.fft0(BB, cWork, false);
  fft.flip(BB, false, false);

  max_beam = max(BB);
  BB /= max_beam;
  itsPsf /= sqrt(max_beam);
  itsmaxbeam = sqrt(max_beam);

  fft.fft0(FTDirty, itsDirty);
  cWork=((FTDirty)*(conj(FTDirty)));
  fft.fft0(tildeII, cWork, false);
  fft.flip(tildeII, false, false);

  cWork=((FTDirty)*(FTPsf));
  fft.fft0(BI, cWork, false);
  fft.flip(BI, false, false);

  BI /= sqrt(max_beam);

  fft.fft0(FTcmap, cmap);
  cWork=((FTDirty)*(conj(FTcmap)));
  fft.fft0(tildeMI, cWork, false);
  fft.flip(tildeMI, false, false);

  fft.fft0(FTBI, BI);
  cWork=((FTBI)*(conj(FTcmap)));
  fft.fft0(tildeMBI, cWork, false);
  fft.flip(tildeMBI, false, false);

  //cWork=((FTDirty)*(FTPsf)*(conj(FTcmap)));
  //fft.fft0(tildeMBI, cWork, false);
  //fft.flip(tildeMBI, false, false);

  //tildeMBI /= sqrt(max_beam);

  casacore::MArray<casacore::Float> Mcmap(cmap);
  normcmap = norm(Mcmap.flatten());
  normcmapsq = normcmap*normcmap;
  sumcmap = sum(cmap);

  //Find mask for the autocorrelation by searching for first sidelobe around central peak
  Int ix=0;
  Int iy=0;
  Int ixy=0;
  Bool searchxx=true;
  Bool searchyy=true;
  Bool searchxy=true;
  IPosition support(itsDirty.shape());

  IPosition posxx(support/2);
  IPosition posyy(support/2);
  IPosition posxy(support/2);
  if(itsAutoXMask == 0 && itsAutoYMask == 0){
	  for(Int is=0; is < support(0); is++){
		posxx(0)=support(0)/2+is;
		posyy(1)=support(1)/2+is;
		posxy(0)=support(0)/2+is;
		posxy(1)=support(1)/2+is;
		if(tildeII(posxx)<0 && searchxx){
			ix=is;
			searchxx=false;
		}
		if(tildeII(posyy)<0 && searchyy){
			iy=is;
			searchyy=false;
		}
		if(tildeII(posxy)<0 && searchxy){
			ixy=is;
			searchxy=false;
		}
		if(searchxx==false && searchyy==false && searchxy==false){
			break;
		}
	  }

	  //blcautomask(support);
	  //trcautomask(support);
	  if(ix<ixy){
		blcautomask(0)=support(0)/2-2*ixy;
		trcautomask(0)=support(0)/2+2*ixy;
		}
	  else{
		blcautomask(0)=support(0)/2-2*ix;
		trcautomask(0)=support(0)/2+2*ix;
		}
	  if(iy<ixy){
		blcautomask(1)=support(1)/2-2*ixy;
		trcautomask(1)=support(1)/2+2*ixy;
		}
	  else{
		blcautomask(1)=support(1)/2-2*iy;
		trcautomask(1)=support(1)/2+2*iy;
		}
  }
  else{
	blcautomask(0)=support(0)/2-itsAutoXMask;
	blcautomask(1)=support(1)/2-itsAutoYMask;
	trcautomask(0)=support(0)/2+itsAutoXMask;
	trcautomask(1)=support(1)/2+itsAutoYMask;
  }
  os << "Correlation Products reinitialized" << LogIO::POST;
}

void AutoCleaner::recomputeCorrProducts()
{
  LogIO os(LogOrigin("AutoCleaner", "recomputeCorrProducts()", WHERE));

  FFTServer<Float,Complex> fft(itsDirty.shape());
  Matrix<Complex> FTPsf;
  Matrix<Complex> FTDirty;
  Matrix<Complex> FTcmap;
  Matrix<Complex> FTBI;
  Matrix<Complex> FTBB;
  Matrix<Complex> FTMB;
  Matrix<Complex> FTMBB;
  Matrix<Complex> cWork;

  fft.fft0(FTPsf, itsPsf);
  fft.fft0(FTcmap, cmap);
  cWork=((FTPsf)*(conj(FTcmap)));
  fft.fft0(tildeMB, cWork, false);
  fft.flip(tildeMB, false, false);

  fft.fft0(FTBB, BB);
  cWork=((FTBB)*(conj(FTcmap)));
  fft.fft0(tildeMBB, cWork, false);
  fft.flip(tildeMBB, false, false);

  fft.fft0(FTDirty, itsDirty);
  cWork=((FTDirty)*(conj(FTcmap)));
  fft.fft0(tildeMI, cWork, false);
  fft.flip(tildeMI, false, false);

  fft.fft0(FTBI, BI);
  cWork=((FTBI)*(conj(FTcmap)));
  fft.fft0(tildeMBI, cWork, false);
  fft.flip(tildeMBI, false, false);

  fft.fft0(FTMB, tildeMB);
  cWork=((FTMB)*(FTcmap));
  fft.fft0(MtildeMB, cWork, false);
  fft.flip(MtildeMB, false, false);

  fft.fft0(FTMBB, tildeMBB);
  cWork=((FTMBB)*(FTcmap));
  fft.fft0(MtildeMBB, cWork, false);
  fft.flip(MtildeMBB, false, false);

  os << "Correlation Products recomputed" << LogIO::POST;
}

void AutoCleaner::approximateBasisFunction()
{
  LogIO os(LogOrigin("AutoCleaner", "approximateBasisFunction()", WHERE));
  os << "Start to clean autocorrelation function" << LogIO::POST;
  Float gain=itsAutoGain;
  Float power=itsAutoPower;      
  Int niter=itsAutoMaxiter;
  Int ii=0;
  Matrix<Float> dmap(itsPsf.shape());
  dmap = 0.0;
  dmap = dmap+tildeII;

  //Find mask for the autocorrelation by searching for first sidelobe around central peak
  Int ix=0;
  Int iy=0;
  Int ixy=0;
  Bool searchxx=true;
  Bool searchyy=true;
  Bool searchxy=true;
  IPosition support(dmap.shape());

  IPosition posxx(support/2);
  IPosition posyy(support/2);
  IPosition posxy(support/2);

  if(itsAutoXMask == 0 && itsAutoYMask == 0){
	  for(Int is=0; is < support(0); is++){
		posxx(0)=support(0)/2+is;
		posyy(1)=support(1)/2+is;
		posxy(0)=support(0)/2+is;
		posxy(1)=support(1)/2+is;
		if(tildeII(posxx)<0 && searchxx){
			ix=is;
			searchxx=false;
		}
		if(tildeII(posyy)<0 && searchyy){
			iy=is;
			searchyy=false;
		}
		if(tildeII(posxy)<0 && searchxy){
			ixy=is;
			searchxy=false;
		}
		if(searchxx==false && searchyy==false && searchxy==false){
			break;
		}
	  }

	  //blcautomask(support);
	  //trcautomask(support);
	  if(ix<ixy){
		blcautomask(0)=support(0)/2-2*ixy;
		trcautomask(0)=support(0)/2+2*ixy;
		}
	  else{
		blcautomask(0)=support(0)/2-2*ix;
		trcautomask(0)=support(0)/2+2*ix;
		}
	  if(iy<ixy){
		blcautomask(1)=support(1)/2-2*ixy;
		trcautomask(1)=support(1)/2+2*ixy;
		}
	  else{
		blcautomask(1)=support(1)/2-2*iy;
		trcautomask(1)=support(1)/2+2*iy;
		}
	}
  else{
	blcautomask(0)=support(0)/2-itsAutoXMask;
	blcautomask(1)=support(1)/2-itsAutoYMask;
	trcautomask(0)=support(0)/2+itsAutoXMask;
	trcautomask(1)=support(1)/2+itsAutoYMask;
  }

  Float strength;
  IPosition index(dmap.shape()); 
  Float scaleFactor;
  Float scaleFactorpow;

  Vector<Float> strengths(niter);
  Vector<Int> xindex(niter);
  Vector<Int> yindex(niter);

  strengths=0.0;
  xindex=0;
  yindex=0;

  Float threshmaxtildeII = itsAutoThreshold*max(abs(tildeII(blcautomask,trcautomask)));

  //for (Int ii=0; ii < niter; ii++) {
  while(max(abs(dmap(blcautomask,trcautomask))) > threshmaxtildeII && ii<niter){
	//findMaxAbs(dmap, strength, index);

	Float minval;
	Float maxval;
	IPosition minpos;
	IPosition maxpos;
	minMax(minval, maxval, minpos, maxpos, dmap(blcautomask,trcautomask));
	if (abs(maxval) > abs(minval) || window_basis(minpos)!=1.0){
		index = maxpos+blcautomask;
		strength = maxval;
	}
	else{
		index = minpos+blcautomask;
		strength = minval;
	}

	scaleFactor=gain*strength;
        scaleFactorpow=sign(scaleFactor)*pow(abs(scaleFactor),power);

	xindex(ii) = index(0);
        yindex(ii) = index(1);
        strengths(ii) = strength;

	// Continuing: subtract the peak that we found from all dirty images
	// Define a subregion so that that the peak is centered

	IPosition inc(dmap.shape().nelements(), 1);
	IPosition blc(index-support/2);
	IPosition trc(index+support/2); 
	LCBox::verify(blc, trc, inc, dmap.shape());
   
	IPosition blcBB(blc+itsPositionPeakPsf-index);
	IPosition trcBB(trc+itsPositionPeakPsf-index);
	LCBox::verify(blcBB, trcBB, inc, dmap.shape());
	makeBoxesSameSize(blc,trc,blcBB,trcBB);

	// The inverse shift indices
	IPosition blcconj(support/2-index);
	IPosition trcconj(support/2+support-index-1); 
	LCBox::verify(blcconj, trcconj, inc, dmap.shape());
   
	IPosition blcBBconj(blcconj+itsPositionPeakPsf+index-support+1);
	IPosition trcBBconj(trcconj+itsPositionPeakPsf+index-support+1);
	LCBox::verify(blcBBconj, trcBBconj, inc, dmap.shape());
	makeBoxesSameSize(blcconj,trcconj,blcBBconj,trcBBconj);

        normcmapsq -= cmap(index)*cmap(index);
        normcmapsq -= cmap(support-index)*cmap(support-index);
        sumcmap += 2*scaleFactor;

        #pragma omp parallel default(shared) num_threads(14)
        {
             #pragma omp master
             {
		// Substract the beam
                #pragma omp task
		subtractBeam(dmap, BB, blc, trc, blcBB, trcBB, scaleFactor, false, false);   

		// Now do the addition of this scale to the model image....
                #pragma omp task
		subtractBeam(cmap, itsScales, blc, trc, blcBB, trcBB, scaleFactorpow, false, true);   

                #pragma omp task
		subtractBeam(mod, BB, blc, trc, blcBB, trcBB, scaleFactor, false, true);   

		// Build the channel products
                #pragma omp task
		subtractBeam(tildeMB, itsPsf, blcconj, trcconj, blcBBconj, trcBBconj, scaleFactorpow, false, true);   

                #pragma omp task
		subtractBeam(tildeMI, itsDirty, blcconj, trcconj, blcBBconj, trcBBconj, scaleFactorpow, false, true);   

                #pragma omp task
		subtractBeam(tildeMBB, BB, blcconj, trcconj, blcBBconj, trcBBconj, scaleFactorpow, false, true);   

                #pragma omp task
		subtractBeam(tildeMBI, BI, blcconj, trcconj, blcBBconj, trcBBconj, scaleFactorpow, false, true);   

		// Redo the same for the conjugate position
                #pragma omp taskwait
                #pragma omp task
		subtractBeam(dmap, BB, blcconj, trcconj, blcBBconj, trcBBconj, scaleFactor, false, false);   

		// Add scale to image
                #pragma omp task
		subtractBeam(cmap, itsScales, blcconj, trcconj, blcBBconj, trcBBconj, scaleFactorpow, false, true);   

                #pragma omp task
		subtractBeam(mod, BB, blcconj, trcconj, blcBBconj, trcBBconj, scaleFactor, false, true);   

                #pragma omp task
		subtractBeam(tildeMB, itsPsf, blc, trc, blcBB, trcBB, scaleFactorpow, false, true);   

                #pragma omp task
		subtractBeam(tildeMI, itsDirty, blc, trc, blcBB, trcBB, scaleFactorpow, false, true);   

                #pragma omp task
		subtractBeam(tildeMBB, BB, blc, trc, blcBB, trcBB, scaleFactorpow, false, true);   

                #pragma omp task
		subtractBeam(tildeMBI, BI, blc, trc, blcBB, trcBB, scaleFactorpow, false, true);   
              }

        }
	window_basis(index) = 1.0;
	window_basis(support-index) = 1.0;
        normcmapsq += cmap(index)*cmap(index);
        normcmapsq += cmap(support-index)*cmap(support-index);
	ii += 1;
  }

  normcmap = sqrtop(normcmapsq);
  niter = ii;

  // continue with the build of channel products that need precomputed quantities
  for (Int i=0; i < niter; i++) {
        // Read stored positions and strength
	index(0)=xindex(i);
	index(1)=yindex(i);
	strength=strengths(i);
	scaleFactor=gain*strength;
	scaleFactorpow=sign(scaleFactor)*pow(abs(scaleFactor),power);

	// Define a subregion so that that the peak is centered
	IPosition support(dmap.shape());

	IPosition inc(dmap.shape().nelements(), 1);
	IPosition blc(index-support/2);
	IPosition trc(index+support/2); 
	LCBox::verify(blc, trc, inc, dmap.shape());
   
	IPosition blcBB(blc+itsPositionPeakPsf-index);
	IPosition trcBB(trc+itsPositionPeakPsf-index);
	LCBox::verify(blcBB, trcBB, inc, dmap.shape());
	makeBoxesSameSize(blc,trc,blcBB,trcBB);

	// The inverse shift indices
	IPosition blcconj(support/2-index);
	IPosition trcconj(support/2+support-index-1); 
	LCBox::verify(blcconj, trcconj, inc, dmap.shape());
   
	IPosition blcBBconj(blcconj+itsPositionPeakPsf+index-support+1);
	IPosition trcBBconj(trcconj+itsPositionPeakPsf+index-support+1);
	LCBox::verify(blcBBconj, trcBBconj, inc, dmap.shape());
	makeBoxesSameSize(blcconj,trcconj,blcBBconj,trcBBconj);

        #pragma omp parallel default(shared) num_threads(4)
        {
            #pragma omp master
            {
		// these two blocks need to be executed after the complete iterations have been done on the previous quantities
		//Matrix<Float> MtildeMBBSub=MtildeMBB(blc, trc);
		////Matrix<Float> MtildeMBBscaleSub=tildeMBB(blcBB,trcBB);
		//MtildeMBBSub += scaleFactor*tildeMBB(blcBB,trcBB);//MtildeMBBscaleSub;
                #pragma omp task
		subtractBeam(MtildeMBB, tildeMBB, blc, trc, blcBB, trcBB, scaleFactorpow, false, true);   

		//Matrix<Float> MtildeMBSub=MtildeMB(blc, trc);
		////Matrix<Float> MtildeMBscaleSub=tildeMB(blcBB,trcBB);
		//MtildeMBSub += scaleFactor*tildeMB(blcBB,trcBB);//MtildeMBscaleSub;
                #pragma omp task
		subtractBeam(MtildeMB, tildeMB, blc, trc, blcBB, trcBB, scaleFactorpow, false, true);   

		// do the conjugate
		//Matrix<Float> MtildeMBBSubc=MtildeMBB(blcconj, trcconj);
		////Matrix<Float> MtildeMBBscaleSubc=tildeMBB(blcBBconj,trcBBconj);
		//MtildeMBBSubc += scaleFactor*tildeMBB(blcBBconj,trcBBconj);//MtildeMBBscaleSubc;
				#pragma omp taskwait
                #pragma omp task
		subtractBeam(MtildeMBB, tildeMBB, blcconj, trcconj, blcBBconj, trcBBconj, scaleFactorpow, false, true);   

		//Matrix<Float> MtildeMBSubc=MtildeMB(blcconj, trcconj);
		////Matrix<Float> MtildeMBscaleSubc=tildeMB(blcBBconj,trcBBconj);
		//MtildeMBSubc += scaleFactor*tildeMB(blcBBconj,trcBBconj);//MtildeMBscaleSubc;
                #pragma omp task
		subtractBeam(MtildeMB, tildeMB, blcconj, trcconj, blcBBconj, trcBBconj, scaleFactorpow, false, true);
           }   
       }
  }
  os << "we used " << niter << " initial iterations" << LogIO::POST;
  os << " to achieve at a threshold " << itsAutoThreshold << LogIO::POST;
}

void AutoCleaner::updateBasisFunction()
{
	LogIO os(LogOrigin("AutoCleaner", "updateBasisFunction()", WHERE));
	os << "Update the Autocorrelation model" << LogIO::POST;

        Float normalize;
        normalize = max(abs(tildeII))/max(abs(mod));
	Float normalizepow;
	normalizepow=pow(normalize,itsAutoPower);

	cmap *= normalizepow; 
  	mod *= normalize;
	tildeMB *= normalizepow;
	tildeMI *= normalizepow;
	tildeMBB *= normalizepow;
	MtildeMBB *= normalizepow*normalizepow;
	tildeMBI *= normalizepow;
	MtildeMB *= normalizepow*normalizepow;

	normcmapsq *= normalizepow*normalizepow;
	normcmap *= normalizepow;
	sumcmap *= normalizepow;

	Matrix<Float> dmap(itsPsf.shape());
	dmap = 0.0;
	dmap = dmap+tildeII-mod;
	Float gain=itsAutoGain;

	Float strength;
	IPosition index(dmap.shape()); 
	Float scaleFactor;
	Float scaleFactorpow;

        Float threshmaxtildeII = itsAutoThreshold*max(abs(tildeII(blcautomask,trcautomask)));
	Int niter=itsAutoMaxiter;
	Int ii=0;

	Vector<Float> strengths(niter);
	Vector<Int> xindex(niter);
	Vector<Int> yindex(niter);

	strengths=0.0;
	xindex=0;
	yindex=0;

        while(max(abs(dmap(blcautomask,trcautomask))) > threshmaxtildeII && ii<niter){

		//findMaxAbs(dmap, strength, index);

		Float minval;
		Float maxval;
		IPosition minpos;
		IPosition maxpos;
		minMax(minval, maxval, minpos, maxpos, dmap(blcautomask,trcautomask));
		if (abs(maxval) > abs(minval) || window_basis(minpos)!=1.0){
			index = maxpos+blcautomask;
			strength = maxval;
		}
		else{
			index = minpos+blcautomask;
			strength = minval;
		}

		scaleFactor=gain*strength;
		scaleFactorpow=sign(scaleFactor)*pow(abs(scaleFactor),itsAutoPower);

		// Define a subregion so that that the peak is centered
		IPosition support(dmap.shape());

		IPosition inc(dmap.shape().nelements(), 1);
		IPosition blc(index-support/2);
		IPosition trc(index+support/2); 
		LCBox::verify(blc, trc, inc, dmap.shape());
	   
		IPosition blcBB(blc+itsPositionPeakPsf-index);
		IPosition trcBB(trc+itsPositionPeakPsf-index);
		LCBox::verify(blcBB, trcBB, inc, dmap.shape());
		makeBoxesSameSize(blc,trc,blcBB,trcBB);

		// The inverse shift indices
		IPosition blcconj(support/2-index);
		IPosition trcconj(support/2+support-index-1); 
		LCBox::verify(blcconj, trcconj, inc, dmap.shape());
	   
		IPosition blcBBconj(blcconj+itsPositionPeakPsf+index-support+1);
		IPosition trcBBconj(trcconj+itsPositionPeakPsf+index-support+1);
		LCBox::verify(blcBBconj, trcBBconj, inc, dmap.shape());
		makeBoxesSameSize(blcconj,trcconj,blcBBconj,trcBBconj);

		normcmapsq -= cmap(index)*cmap(index);
		normcmapsq -= cmap(support-index)*cmap(support-index);

		sumcmap += 2*scaleFactor;

		xindex(ii) = index(0);
		yindex(ii) = index(1);
		strengths(ii) = strength;

		#pragma omp parallel default(shared) num_threads(3)
		{
		     #pragma omp master
		     {
			#pragma omp task
			subtractBeam(dmap, BB, blc, trc, blcBB, trcBB, scaleFactor, false, false);
			#pragma omp task
			subtractBeam(cmap, itsScales, blc, trc, blcBB, trcBB, scaleFactorpow, false, true);  
			#pragma omp task
			subtractBeam(mod, BB, blc, trc, blcBB, trcBB, scaleFactor, false, true);    
			
			#pragma omp taskwait
			#pragma omp task
			subtractBeam(dmap, BB, blcconj, trcconj, blcBBconj, trcBBconj, scaleFactor, false, false); 
			#pragma omp task
			subtractBeam(cmap, itsScales, blcconj, trcconj, blcBBconj, trcBBconj, scaleFactorpow, false, true);   
			#pragma omp task
			subtractBeam(mod, BB, blcconj, trcconj, blcBBconj, trcBBconj, scaleFactor, false, true);   
		     }
		}

		window_basis(index) = 1.0;
		window_basis(support-index) = 1.0;

		normcmapsq += cmap(index)*cmap(index);
		normcmapsq += cmap(support-index)*cmap(support-index);

		ii+=1;
	}

	normcmap = sqrtop(normcmapsq);
	niter = ii;

	//for just a few iterations, the fastest version is to track down the change in the correlation products by an analytic description
	// for more iterations, it makes more sense to just redo the convolution operation
	if(niter<10){
		for (Int i=0; i < niter; i++) {
			// Read stored positions and strength
			index(0)=xindex(i);
			index(1)=yindex(i);
			strength=strengths(i);
			scaleFactor=gain*strength;
			scaleFactorpow=sign(scaleFactor)*pow(abs(scaleFactor),itsAutoPower);

			// Define a subregion so that that the peak is centered
			IPosition support(dmap.shape());

			IPosition inc(dmap.shape().nelements(), 1);
			IPosition blc(index-support/2);
			IPosition trc(index+support/2); 
			LCBox::verify(blc, trc, inc, dmap.shape());
		   
			IPosition blcBB(blc+itsPositionPeakPsf-index);
			IPosition trcBB(trc+itsPositionPeakPsf-index);
			LCBox::verify(blcBB, trcBB, inc, dmap.shape());
			makeBoxesSameSize(blc,trc,blcBB,trcBB);

			// The inverse shift indices
			IPosition blcconj(support/2-index);
			IPosition trcconj(support/2+support-index-1); 
			LCBox::verify(blcconj, trcconj, inc, dmap.shape());
		   
			IPosition blcBBconj(blcconj+itsPositionPeakPsf+index-support+1);
			IPosition trcBBconj(trcconj+itsPositionPeakPsf+index-support+1);
			LCBox::verify(blcBBconj, trcBBconj, inc, dmap.shape());
			makeBoxesSameSize(blcconj,trcconj,blcBBconj,trcBBconj);

			// store old products for parallel computing
			Matrix<Float> tildeMBold(tildeMB.shape());
			tildeMBold.assign_conforming(tildeMB);
			Matrix<Float> tildeMBBold(tildeMBB.shape());
			tildeMBBold.assign_conforming(tildeMBB);

			#pragma omp parallel default(shared) num_threads(6)
			{
			     #pragma omp master
			     {

				//#pragma omp task
				//subtractBeam(dmap, BB, blc, trc, blcBB, trcBB, scaleFactor, false, false);	

				// Add scale to image
				//#pragma omp task
				//subtractBeam(cmap, itsScales, blc, trc, blcBB, trcBB, scaleFactor, false, true);   

				//#pragma omp task
				//subtractBeam(mod, BB, blc, trc, blcBB, trcBB, scaleFactor, false, true);   

				//update channel products, first the beams
				#pragma omp task
				MtildeMB += scaleFactorpow*scaleFactorpow*itsPsf;
				#pragma omp task
				MtildeMBB += scaleFactorpow*scaleFactorpow*BB;

				//wait for task to finish before writing to MtildeMB again
				#pragma omp taskwait
				#pragma omp task  
				subtractBeam(MtildeMB, tildeMBold, blc, trc, blcBB, trcBB, scaleFactorpow, false, true);      

				#pragma omp task  
				subtractBeam(MtildeMBB, tildeMBBold, blc, trc, blcBB, trcBB, scaleFactorpow, false, true);   
	   
				#pragma omp task
				subtractBeam(tildeMB, itsPsf, blcconj, trcconj, blcBBconj, trcBBconj, scaleFactorpow, false, true);   

				#pragma omp task
				subtractBeam(tildeMBB, BB, blcconj, trcconj, blcBBconj, trcBBconj, scaleFactorpow, false, true);   

				//Now the maps
				#pragma omp task
				subtractBeam(tildeMI, itsDirty, blcconj, trcconj, blcBBconj, trcBBconj, scaleFactorpow, false, true);   

				#pragma omp task
				subtractBeam(tildeMBI, BI, blcconj, trcconj, blcBBconj, trcBBconj, scaleFactorpow, false, true);   

				// Add the conjugate scale to image as well
				//#pragma omp task
				//subtractBeam(cmap, itsScales, blcconj, trcconj, blcBBconj, trcBBconj, scaleFactor, false, true);   

				//#pragma omp task
				//subtractBeam(mod, BB, blcconj, trcconj, blcBBconj, trcBBconj, scaleFactor, false, true);   

				//previous tasks have to finish first to avoid overwriting of MtildeMB and MtildeMBB
				#pragma omp taskwait 
				#pragma omp task  
				subtractBeam(MtildeMB, tildeMBold, blcconj, trcconj, blcBBconj, trcBBconj, scaleFactorpow, true, true);

				#pragma omp task 
				subtractBeam(MtildeMBB, tildeMBBold, blcconj, trcconj, blcBBconj, trcBBconj, scaleFactorpow, true, true);

				// update channel products with the conjugate as well
				#pragma omp taskwait
				#pragma omp task
				{tildeMBold.assign_conforming(tildeMB);		
				tildeMBBold.assign_conforming(tildeMBB);}
			
				#pragma omp task
				MtildeMB += scaleFactorpow*scaleFactorpow*itsPsf;
				#pragma omp task
				MtildeMBB += scaleFactorpow*scaleFactorpow*BB;

				//These two conjugates cannot be added above with the reverse, since we need the update of tildeMB and tildeMBB first
				#pragma omp taskwait
				#pragma omp task  
				subtractBeam(MtildeMB, tildeMBold, blcconj, trcconj, blcBBconj, trcBBconj, scaleFactorpow, false, true);   
	  
				#pragma omp task
				subtractBeam(MtildeMBB, tildeMBBold, blcconj, trcconj, blcBBconj, trcBBconj, scaleFactorpow, false, true);   

				#pragma omp task
				subtractBeam(tildeMB, itsPsf, blc, trc, blcBB, trcBB, scaleFactorpow, false, true);   

				#pragma omp task
				subtractBeam(tildeMBB, BB, blc, trc, blcBB, trcBB, scaleFactorpow, false, true);   

				//Now the maps
				#pragma omp task
				subtractBeam(tildeMI, itsDirty, blc, trc, blcBB, trcBB, scaleFactorpow, false, true);   

				#pragma omp task
				subtractBeam(tildeMBI, BI, blc, trc, blcBB, trcBB, scaleFactorpow, false, true);

				//#pragma omp task
				//subtractBeam(dmap, BB, blcconj, trcconj, blcBBconj, trcBBconj, scaleFactor, false, false); 

				//wait for task to finish to prevent overwriting
				#pragma omp taskwait
				#pragma omp task
				subtractBeam(MtildeMB, tildeMBold, blc, trc, blcBB, trcBB, scaleFactorpow, true, true); 
	 
				#pragma omp task
				subtractBeam(MtildeMBB, tildeMBBold, blc, trc, blcBB, trcBB, scaleFactorpow, true, true);
			     }	
			}
		}
	}
	else{
		recomputeCorrProducts();
	}
	basis_function = cmap;
	os << "Using " << niter << " iterations for that" << LogIO::POST;
}

void AutoCleaner::subtractBeam(Matrix<Float> &map, Matrix<Float> &beam, IPosition blc, IPosition trc, IPosition blcbeam, IPosition trcbeam, Float factor, Bool reverse, Bool add)
{
  Matrix<Float> mapSub=map(blc,trc);
  if(reverse){
     if(add){
        mapSub += factor*reverseArray(reverseArray(beam, 0), 1)(blcbeam,trcbeam);
     }
     else{
        mapSub -= factor*reverseArray(reverseArray(beam, 0), 1)(blcbeam,trcbeam);
     }
  }
  else{
     if(add){
        mapSub += factor*beam(blcbeam,trcbeam);
     }
     else{
        mapSub -= factor*beam(blcbeam,trcbeam);
     }
  }
}

Bool AutoCleaner::setscales()
{
  Int nx=itsPsf.shape()(0);
  Int ny=itsPsf.shape()(1);

  Double refi=nx/2;
  Double refj=ny/2;

  //itsScales=new Matrix<Float>(itsDirty.shape());
  Matrix<Float> itsScales_c(nx,ny);
  //itsScales(itsPsf.shape());
  itsScales_c = 0.0;

  itsScales_c(Int(refi), Int(refj)) = 1.0;

  itsScales.resize(itsPsf.shape());
  itsScales.assign(itsScales_c);

  return true;
}

void AutoCleaner::setDirty(const Matrix<Float>& dirty){

  //itsDirty=new Matrix<Float>(dirty.shape());
  //Matrix<Float> itsDirty;
  itsDirty.resize(dirty.shape());
  itsDirty.assign(dirty);
}

//void AutoCleaner::setPsf(const Matrix<Float>& psf){
//  //Matrix<Float> itsPsf;
//  itsPsf(psf.shape());
//  itsPsf.assign(psf);
//}


void AutoCleaner::unsetMask()
{
  if(!itsMask.null())
    itsMask=0;
  noClean_p=false;
}

Float AutoCleaner::threshold() const
{
  if (! itsDoSpeedup) {
    return max(itsFracThreshold.get("%").getValue() * itsMaximumResidual /100.0,
	       itsThreshold.get("Jy").getValue());
  } else {
    const Float factor = exp( (Float)( itsIteration - itsStartingIter )/ itsNDouble )
      / 2.7182818;
    return factor * max(itsFracThreshold.get("%").getValue() * itsMaximumResidual /100.0,
		       itsThreshold.get("Jy").getValue());
  }
}

void AutoCleaner::makeBoxesSameSize(IPosition& blc1, IPosition& trc1, 
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

Int AutoCleaner::findBeamPatch(const Float maxScaleSize, const Int& nx, const Int& ny,
				 const Float psfBeam, const Float nBeams)
{
  Int psupport = (Int) ( sqrt( psfBeam*psfBeam + maxScaleSize*maxScaleSize ) * nBeams  );

  // At least this big...
  if(psupport < psfBeam*nBeams ) psupport = static_cast<Int>(psfBeam*nBeams);

  // Not too big...
  if(psupport > nx || psupport > ny)   psupport = std::min(nx,ny);

  // Make it even.
  if (psupport%2 != 0) psupport -= 1;

  return psupport;
}



} //# NAMESPACE CASA - END
