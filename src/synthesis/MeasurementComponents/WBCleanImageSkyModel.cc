//# WBCleanImageSkyModel.cc: Implementation of WBCleanImageSkyModel class
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
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id: WBCleanImageSkyModel.cc 13615 2010-12-20 14:04:00 UrvashiRV$
//# v2.6 : Added psf-patch support to reduce memory footprint.

#include <casacore/casa/Arrays/ArrayMath.h>
#include <synthesis/MeasurementComponents/WBCleanImageSkyModel.h>
#include <synthesis/MeasurementEquations/CubeSkyEquation.h>
#include <casacore/casa/OS/File.h>
#include <synthesis/MeasurementEquations/SkyEquation.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <synthesis/MeasurementEquations/LatticeModel.h>
#include <synthesis/MeasurementEquations/LatConvEquation.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/Assert.h>

#include <casacore/images/Images/PagedImage.h>
#include <casacore/images/Images/SubImage.h>
#include <casacore/images/Regions/ImageRegion.h>
#include <casacore/images/Regions/RegionManager.h>

#include <casacore/images/Regions/WCBox.h>

#include <casacore/measures/Measures/Quality.h>
#include <casacore/coordinates/Coordinates/QualityCoordinate.h>
#include <casacore/images/Images/ImageUtilities.h>

#include <casacore/scimath/Mathematics/MatrixMathLA.h>

#include <msvis/MSVis/VisSet.h>
#include <msvis/MSVis/VisSetUtil.h>

#include <casacore/ms/MeasurementSets/MSColumns.h>

#include <sstream>

#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogSink.h>

#include <casacore/casa/OS/HostInfo.h>


using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN
#define TMR(a) "[User: " << a.user() << "] [System: " << a.system() << "] [Real: " << a.real() << "]"
	
#define MIN(a,b) ((a)<=(b) ? (a) : (b))
#define MAX(a,b) ((a)>=(b) ? (a) : (b))

/*************************************
 *          Constructor
 *************************************/
WBCleanImageSkyModel::WBCleanImageSkyModel()
{
  initVars();
  nscales_p=4;
  ntaylor_p=2;
  refFrequency_p=1.42e+09;
  scaleSizes_p.resize(0);
};
  WBCleanImageSkyModel::WBCleanImageSkyModel(const Int ntaylor,const Int nscales,const Double reffreq)
{
  initVars();
  nscales_p=nscales;
  ntaylor_p=ntaylor;
  refFrequency_p=reffreq;
  scaleSizes_p.resize(0);
};
  WBCleanImageSkyModel::WBCleanImageSkyModel(const Int ntaylor,const Vector<Float>& userScaleSizes,const Double reffreq)
{
  initVars();
  if(userScaleSizes.nelements()==0)  os << "No scales set !" << LogIO::WARN;
  if(userScaleSizes[0]!=0.0)
  {
    nscales_p=userScaleSizes.nelements()+1;
    scaleSizes_p.resize(nscales_p);
    for(Int i=1;i<nscales_p;i++) scaleSizes_p[i] = userScaleSizes[i-1];
    scaleSizes_p[0]=0.0;
  }
  else
  {
    nscales_p=userScaleSizes.nelements();
    scaleSizes_p.resize(nscales_p);
    for(Int i=0;i<nscales_p;i++) scaleSizes_p[i] = userScaleSizes[i];
  }
  ntaylor_p=ntaylor;
  refFrequency_p=reffreq;
};

void WBCleanImageSkyModel::initVars()
{
  adbg=0; 
  ddbg=1; // output per iteration
  tdbg=0;
  
  modified_p=true;
  memoryMB_p = Double(HostInfo::memoryTotal(true)/1024)/(2.0);
  donePSF_p=false;
  doneMTMCinit_p=false;

  numbermajorcycles_p=0;
  nfields_p=1;
  lc_p.resize(0);

  os = LogIO(LogOrigin("WBCleanImageSkyModel","solve"));

  setAlgorithm("MSMFS");
  
}

/*************************************
 *          Destructor
 *************************************/
WBCleanImageSkyModel::~WBCleanImageSkyModel()
{
  lc_p.resize(0);
  //cout << "WBCleanImageSkyModel destructor " << endl;

  /*
  if(nmodels_p > numberOfModels())
    {
      resizeWorkArrays(numberOfModels());
      nmodels_p = numberOfModels();
    }
  */

};

/*************************************
 *          Solver
 *************************************/
Bool WBCleanImageSkyModel::solve(SkyEquation& se) 
{
	os << "MSMFS algorithm (v2.6) with " << ntaylor_p << " Taylor coefficients and Reference Frequency of " << refFrequency_p  << " Hz" << LogIO::POST;
	Int stopflag=0;
	Int nchan=0,npol=0;

	/* Gather shape information */
	nmodels_p = numberOfModels();

	if(nmodels_p % ntaylor_p != 0)
	{
	  os << "Incorrect number of input models " << LogIO::EXCEPTION;
	  os << "NModels = N_fields x N_taylor" << LogIO::EXCEPTION;
	  AlwaysAssert((nmodels_p % ntaylor_p == 0), AipsError);
	}

	/* Check supplied bandwidth-ratio and print warnings if needed */
        checkParameters();

	/* Calc the number of fields */
	nfields_p = nmodels_p/ntaylor_p;
        ///// NOTE : Code is written with loops on 'fields' to support a future implementation
        /////             Disable it for now, since it has not been tested.
        //AlwaysAssert(nfields_p==1, AipsError);

	//cout << "Number of fields : " << nfields_p << endl;

        /* Current restriction : one pol and one chan-plane */
	for(Int model=0;model<nmodels_p;model++)
	{
	  nx = image(model).shape()(0);
	  ny = image(model).shape()(1);
	  npol=image(model).shape()(2);
	  nchan=image(model).shape()(3);
	  if(nchan > 1) os << "Cannot process more than one output channel" << LogIO::EXCEPTION;
	  if(npol > 1) os << "Cannot process more than one output polarization" << LogIO::EXCEPTION;
	  AlwaysAssert((nchan==1), AipsError);  
	  AlwaysAssert((npol==1), AipsError);  
	}

	//------------------------------- For 'active'  ---------------------------------------------------------------///
	/* Calculate the initial residual image for all models. */
	if( se.isNewFTM() == false )
	  {
	    if(!donePSF_p)
	      {
		os << "Calculating initial residual images..." << LogIO::POST;
		solveResiduals(se,(numberIterations()<1)?true:False);
	      }
	    else
	      {
		/*
		if(numbermajorcycles_p>0) 
		  {
		    os << "RE-Calculating residual images because previous residuals have been modified in-place during restoration to be 'coefficient residuals'." << LogIO::POST;
		    solveResiduals(se,(numberIterations()<1)?true:False);
		  }
		*/
	      }
	  }
	//------------------------------- For 'active'  ---------------------------------------------------------------///
	
	/* Create the Point Spread Functions */
	if(!donePSF_p)
	{
  	    /* Resize the work arrays to calculate extra PSFs */
	    Int original_nmodels = nmodels_p;
	    nmodels_p = original_nmodels + nfields_p * (ntaylor_p - 1);
	    resizeWorkArrays(nmodels_p);

	    try
	    {
	      /* Make the 2N-1 PSFs */
	      os << "Calculating spectral PSFs..." << LogIO::POST;
              makeSpectralPSFs(se, numberIterations()<0?true:False);
	    }
	    catch(AipsError &x)
	    {
	      /* Resize the work arrays to normal size - the destructors use 'nmodels_p' on other lists */
	      nmodels_p = original_nmodels;
	      resizeWorkArrays(nmodels_p);
	      os << "Could not make PSFs. Please check image co-ordinate system : " << x.getMesg() << LogIO::EXCEPTION;
	    }

	    if(adbg) cout << "Shape of lc_p : " << lc_p.nelements() << endl;
	    /* Initialize MTMC, allocate memory, and send in all 2N-1 psfs */
	    if(lc_p.nelements()==0 && numberIterations()>=0)
	      {
		lc_p.resize(nfields_p);
                Bool state=true;
		/* Initialize the MultiTermMatrixCleaners */
		for(Int thismodel=0;thismodel<nfields_p;thismodel++)
		  {
		    lc_p[thismodel].setscales(scaleSizes_p);
		    lc_p[thismodel].setntaylorterms(ntaylor_p);
		    nx = image(thismodel).shape()(0);
		    ny = image(thismodel).shape()(1);
		    state &= lc_p[thismodel].initialise(nx,ny); // allocates memory once....
		  }
                if( !state ) // initialise will return false if there is any internal inconsistency with settings so far.
		  {
		    lc_p.resize(0);
		    nmodels_p = original_nmodels;
		    resizeWorkArrays(nmodels_p);
		    os << LogIO::SEVERE << "Could not initialize MS-MFS minor cycle" << LogIO::EXCEPTION;
		    return false; // redundant
		  }
		
		/* Send all 2N-1 PSFs into the MultiTermLatticeCleaner */
		for(Int thismodel=0;thismodel<nfields_p;thismodel++)
		  {
		    for (Int order=0;order<2*ntaylor_p-1;order++)
		      {
			// This should be doing a reference only. Make sure of this.
			Matrix<Float> tempMat;
			Array<Float> tempArr;
			(PSF(getModelIndex(thismodel,order))).get(tempArr,true);
			tempMat.reference(tempArr);
			
			lc_p[thismodel].setpsf( order , tempMat ); 
		      }
		  }
		doneMTMCinit_p = true;
	      }
	    
	    /* Resize the work arrays to normal size - for residual comps, etc. */
	    nmodels_p = original_nmodels;
	    resizeWorkArrays(nmodels_p);
            ///// TODO : Make sure this releases memory as it is supposed to.	   

	    donePSF_p=true;
	}


	//------------------------------ For new FTMs --------------------------------------------------------------//
	///// Consider doing lc_p.setmodel and getmodel instead of dividing/multiplying the avgPB in NewMTFT::initializeToVis
	if( se.isNewFTM() == true )
	  {
	    /* Calculate the initial residual image for all models. */
	    if( numbermajorcycles_p==0 )
	      {
		os << "Calculating initial residual images..." << LogIO::POST;
		solveResiduals(se,(numberIterations()<1)?true:False);
	      }

	  }
	//------------------------------- For new FTMs -------------------------------------------------------------//

	/* Return if niter=0 */
	/* Check if this is an interactive-clean run, or if niter=0 */
	if(adbg) cout << "NumberIterations - before any cycles: " << numberIterations() << endl;
	if(numberIterations() < 1)
	{
		return true;
	}

	/* Check that lc_p's have been initialized by now.. */
	if(doneMTMCinit_p == false)
	  {
	    os << LogIO::SEVERE << "MultiTermMatrixCleaners are un-initialized, perhaps because of a previous im.clean(niter=-1) call. Please close imager, re-open it, and run with niter>=0" << LogIO::POST;
	  }

	/* Set up the Mask image */
	for(Int thismodel=0;thismodel<nfields_p;thismodel++)
	{
	  if(hasMask(getModelIndex(thismodel,0))) 
	  {
	    if(adbg) os << "Sending in the mask for lc_p : " << thismodel << LogIO::POST;

            Matrix<Float> tempMat;
            Array<Float> tempArr;
            (mask(getModelIndex(thismodel,0))).get(tempArr,true);
            tempMat.reference(tempArr);

	    lc_p[thismodel].setmask( tempMat );
	  }
	}

	/******************* START MAJOR CYCLE LOOP *****************/
        /* Logic for number of iterations in the minor-cycle vs major cycle vs total,
           how they relate to convergence thresholds, and how they change with 
           interactive vs non-interactive use (user specified total niter, vs niters per 
           major cycle...) is a bit of a mess. Needs cleanup. Right now, hard-coded for
           nfields=1 */
        previous_maxresidual_p=1e+10;
	Int index=0;
        Float fractionOfPsf=0.1;
        Vector<Int> iterationcount(nfields_p); iterationcount=0;
        Vector<Int> moreiterations(nfields_p); moreiterations=0;
        Vector<Int> thiscycleniter(nfields_p); thiscycleniter=0;
	for(Int itercountmaj=0;itercountmaj<1000;itercountmaj++)
	  {
	    numbermajorcycles_p ++;
	    os << "**** Major Cycle " << numbermajorcycles_p << LogIO::POST;
	    thiscycleniter=0;
	    /* Compute stopping threshold for this major cycle */
	    stopflag = static_cast<casacore::Int>(computeFluxLimit(fractionOfPsf));
	    /* If the peak residual is already less than the user-threshold, stop */
	    if(stopflag==1) break;
	    /* If we detect divergence across major cycles, stop */
	    if(stopflag==-1) break;
	    
	    for(Int thismodel=0;thismodel<nfields_p;thismodel++) // For now, nfields_p=1 always.
	      {

		if( !isSolveable(getModelIndex(thismodel,0)) )
		  {
		    // This field is not to be cleaned in this set of minor-cycle iterations
		    continue;
		  }

		/* Number of iterations left to do */ 
		moreiterations[thismodel] = numberIterations() - iterationcount[thismodel];
		
		/* If all iterations are done for this run, stop - for all fields*/
		if(moreiterations[thismodel] <=0 ) {stopflag=-1;break;}
		
		/* Send in the current model and residual */
		for (Int order=0;order<ntaylor_p;order++)
		  {
		    index = getModelIndex(thismodel,order);
		    
		    Matrix<Float> tempMat;
		    Array<Float> tempArr;
		    
		    (residual(index)).get(tempArr,true);
		    tempMat.reference(tempArr);
		    lc_p[thismodel].setresidual(order,tempMat); 
		    
		    (image(index)).get(tempArr,true);
		    tempMat.reference(tempArr);
		    lc_p[thismodel].setmodel(order,tempMat); 
		  }
		
		/* Deconvolve */
		/* Return value is the number of minor-cycle iterations done */
		os << "Starting Minor Cycle iterations for field : " << thismodel << LogIO::POST;
		thiscycleniter[thismodel] = lc_p[thismodel].mtclean(moreiterations[thismodel], fractionOfPsf, gain(), threshold());
		
		/* A signal for a singular Hessian : stop */
		if(thiscycleniter[thismodel]==-2) { stopflag=-2; break;}
		
		/* A signal for convergence with no iterations  */
		/* One field may have converged, while others go on... so 'continue' */
		//             if(thiscycleniter[thismodel]==0) { stopflag=1; break; }
		if(thiscycleniter[thismodel]==0) { stopflag=1; continue; }
		
		///* A signal for divergence. Save current model and stop (force a major cycle). */
		//if(thiscycleniter==-1) { stopflag=1; }
		
		/* Increment the minor-cycle iteration counter */
		iterationcount[thismodel] += thiscycleniter[thismodel];
		
		/* Get out the updated model */
		for (Int order=0;order<ntaylor_p;order++)
		  {
		    index = getModelIndex(thismodel,order);
		    Matrix<Float> tempMod;
		    lc_p[thismodel].getmodel(order,tempMod);
		    image(index).put(tempMod);
		  }           
		
	      }// end of model loop
	    
            if(adbg) cout << "end of major cycle : " << itercountmaj << " -> iterationcount : " << iterationcount << "  thiscycleniter : " << thiscycleniter << "  stopflag : " << stopflag << endl;
	    
	    /* Exit without further ado if MTMC cannot invert matrices */
	    if(stopflag == -2)
	      {
		os << "Cannot invert Multi-Term Hessian matrix. Please check the reference-frequency and ensure that the number of frequency-channels in the selected data >= nterms" << LogIO::WARN << LogIO::POST;
		break;
	      }
	    
	    /* If reached 1000 major cycles - assume something is wrong */
	    if(itercountmaj==999) os << " Reached the allowed maximum of 1000 major cycles " << LogIO::POST;
	    
	    /* Do the prediction and residual computation for all models. */
	    /* Exit if we're already at the user-specified flux threshold, or reached numberIterations() */
	    if( abs(stopflag)==1 || max(iterationcount) >= numberIterations() || itercountmaj==999) 
	      {
		os << "Calculating final residual images..." << LogIO::POST;
		/* Call 'solveResiduals' with modelToMS = true to write the model to the MS */
		solveResiduals(se,true);
		break;
	      }
	    else 
	      {
		os << "Calculating new residual images..." << LogIO::POST;
		solveResiduals(se);
	      }

	    /*	    
	    if( se.isNewFTM() == true )
	      {
		// The model will get PB-corrected in-place before prediction.
		// This needs to be reset to what the preceeding minor cycle got, 
		//    for incremental additions in the next cycle.
		saveCurrentModels();
	      }
	    */

	  } 
	/******************* END MAJOR CYCLE LOOP *****************/
	
	/* Compute and write alpha,beta results to disk */
	///writeResultsToDisk();
	//	calculateCoeffResiduals();
	
	/* stopflag=1 -> stopped because the user-threshold was reached */
        /* stopflag=-1 -> stopped because number of iterations was reached */
        /* stopflag=-2 -> stopped because of singular Hessian */
        /* stopflag=0 -> reached maximum number of major-cycle iterations !! */
	if(stopflag>0) return(true);
	else return(false);
} // END OF SOLVE


void WBCleanImageSkyModel::saveCurrentModels()
{
  
  for(Int thismodel=0;thismodel<nfields_p;thismodel++)
    {
      /* Get out the updated model */
      for (Int order=0;order<ntaylor_p;order++)
	{
	  Int index = getModelIndex(thismodel,order);
	  Matrix<Float> tempMod;
	  lc_p[thismodel].getmodel(order,tempMod);
	  image(index).put(tempMod);
	}           
    }// end of model loop
  
}// end of saveCurrentModels


/* Calculate stopping threshold for the current set of minor-cycle iterations */
/* Note : The stopping threshold is computed from the residual image.
   However, MTMC checks on the peak of (residual * psf * scale_0).
   These values can differ slightly. 
   TODO : Perhaps move this function back into MTMC. 
   (It was brought out to conform to the other devonvolvers..)
*/
Float WBCleanImageSkyModel::computeFluxLimit(Float &fractionOfPsf)
{
  LogIO os(LogOrigin("WBCleanImageSkyModel", "computeFluxLimit", WHERE));
  Float maxres=0.0,maxval=0.0,minval=0.0;
  Vector<Float> fmaxres(nfields_p); fmaxres=0.0;
  
  /* Measure the peak residual across all fields */
  for(Int field=0;field<nfields_p;field++)
    {
      Int index = getModelIndex(field,0);
      Array<Float> tempArr;
      (residual(index)).get(tempArr,true);
      IPosition maxpos(tempArr.shape()),minpos(tempArr.shape());
      minMax(minval,maxval, minpos, maxpos,tempArr);
      fmaxres[field]=maxval;
    }
  
  /* Pick the peak residual across all fields */
  maxres = max(fmaxres);
  if(adbg) cout << "Peak Residual across fields (over all pixels) : " << maxres << endl;
  
  /* If we've already converged, return */
  if(maxres < threshold()) 
    {
      os << "Peak residual : " << maxres << " is lower than the user-specified stopping threshold : " << threshold() << LogIO::POST;
      return 1;
    }
  
  /* If we detect divergence across major cycles, warn or STOP */
  if(fabs( ((maxres - previous_maxresidual_p)/previous_maxresidual_p ) > 10.0 ))
    {
      os << "Peak residual : " << maxres << " has increased by more than a factor of 10 across major cycles. Could be diverging. Stopping" << LogIO::POST;
      return -1;
    }
  if(fabs( ((maxres - previous_maxresidual_p)/previous_maxresidual_p ) > 2.0 ))
    {
      os << "Peak residual : " << maxres << " has increased across major cycles. Could be diverging, but continuing..." << LogIO::POST;
    }
  previous_maxresidual_p = maxres;
  
  /* Find PSF sidelobe level */
  Float maxpsfside=0.0;
  for(uInt field=0;static_cast<Int>(field)<nfields_p;field++)
    {
      Int index = getModelIndex(field,0);
      /* abs(min of the PSF) will be treated as the max sidelobe level */
      Array<Float> tempArr;
      (PSF(index)).get(tempArr,true);
      IPosition maxpos(tempArr.shape()),minpos(tempArr.shape());
      minMax(minval,maxval, minpos, maxpos,tempArr);
      maxpsfside = max(maxpsfside, abs(minval));
    }
  
  fractionOfPsf = min(cycleMaxPsfFraction_p, cycleFactor_p * maxpsfside);
  if(fractionOfPsf>0.8) fractionOfPsf=0.8;
  // cyclethreshold = max(threshold(), fractionOfPsf * maxres);
  
  // if(adbg) 
  {
    os << "Peak Residual (all pixels) : " << maxres  << "  User Threshold : " << threshold() << "  Max PSF Sidelobe : " << abs(minval) <<  " User maxPsfFraction : " << cycleMaxPsfFraction_p  << "  User cyclefactor : " << cycleFactor_p << "  fractionOfPsf = min(maxPsfFraction, PSFsidelobe x cyclefactor) : " << fractionOfPsf << LogIO::POST;
    //    os << "Stopping threshold for this major cycle min(user threshold , fractionOfPsf x Max Residual) : " <<  cyclethreshold  << endl;
  }
  
  return 0;
}

/***********************************************************************/
Bool WBCleanImageSkyModel::calculateAlphaBeta(const Vector<String> &restoredNames, 
                                                                           const Vector<String> &residualNames)
{
  LogIO os(LogOrigin("WBCleanImageSkyModel", "calculateAlphaBeta", WHERE));
  //UNUSED: Int index=0;
  Bool writeerror=true;
  

  for(Int field=0;field<nfields_p;field++)
    {
      Int baseindex = getModelIndex(field,0);
      String alphaname,betaname, alphaerrorname;
      if(  ( (restoredNames[baseindex]).substr( (restoredNames[baseindex]).length()-3 , 3 ) ).matches("tt0") )
	{
	  alphaname = (restoredNames[baseindex]).substr(0,(restoredNames[baseindex]).length()-3) + "alpha";
	  betaname = (restoredNames[baseindex]).substr(0,(restoredNames[baseindex]).length()-3) + "beta";
	  if(writeerror) alphaerrorname = alphaname+".error";
	}
      else
	{
	  alphaname = (restoredNames[baseindex]) +  String(".alpha");
	  betaname = (restoredNames[baseindex]) +  String(".beta");
	}
      
      /* Create empty alpha image, and alpha error image */
      PagedImage<Float> imalpha(image(baseindex).shape(),image(baseindex).coordinates(),alphaname); 
      imalpha.set(0.0);

      /* Open restored images */
      PagedImage<Float> imtaylor0(restoredNames[getModelIndex(field,0)]);
      PagedImage<Float> imtaylor1(restoredNames[getModelIndex(field,1)]);

      /* Open first residual image */
      PagedImage<Float> residual0(residualNames[getModelIndex(field,0)]);

      /* Create a mask - make this adapt to the signal-to-noise */
      LatticeExprNode leMaxRes=max(residual0);
      Float maxres = leMaxRes.getFloat();
      // Threshold is either 10% of the peak residual (psf sidelobe level) or 
      // user threshold, if deconvolution has gone that deep.
      Float specthreshold = MAX( threshold()*5 , maxres/5.0 );
      os << "Calculating spectral parameters for  Intensity > MAX(threshold*5,peakresidual/5) = " << specthreshold << " Jy/beam" << LogIO::POST;
      LatticeExpr<Float> mask1(iif((imtaylor0)>(specthreshold),1.0,0.0));
      LatticeExpr<Float> mask0(iif((imtaylor0)>(specthreshold),0.0,1.0));

      /////// Calculate alpha
      LatticeExpr<Float> alphacalc( ((imtaylor1)*mask1)/((imtaylor0)+(mask0)) );
      imalpha.copyData(alphacalc);

      // Set the restoring beam for alpha
      ImageInfo ii = imalpha.imageInfo();
      ii.setRestoringBeam( (imtaylor0.imageInfo()).restoringBeam() );
      imalpha.setImageInfo(ii);
      //imalpha.setUnits(Unit("Spectral Index"));
      imalpha.table().unmarkForDelete();

      // Make a mask for the alpha image
      LatticeExpr<Bool> lemask(iif((imtaylor0 > specthreshold) , true, false));

      createMask(lemask, imalpha);
      os << "Written Spectral Index Image : " << alphaname << LogIO::POST;


      ////// Calculate error on alpha
      if(writeerror)
	{
	  PagedImage<Float> imalphaerror(image(baseindex).shape(),image(baseindex).coordinates(),alphaerrorname); 
	  imalphaerror.set(0.0);
	  PagedImage<Float> residual1(residualNames[getModelIndex(field,1)]);

	  LatticeExpr<Float> alphacalcerror( abs(alphacalc) * sqrt( ( (residual0*mask1)/(imtaylor0+mask0) )*( (residual0*mask1)/(imtaylor0+mask0) ) + ( (residual1*mask1)/(imtaylor1+mask0) )*( (residual1*mask1)/(imtaylor1+mask0) )  ) );
	  imalphaerror.copyData(alphacalcerror);
	  imalphaerror.setImageInfo(ii);
          createMask(lemask, imalphaerror);
	  imalphaerror.table().unmarkForDelete();      
	  os << "Written Spectral Index Error Image : " << alphaerrorname << LogIO::POST;

	  //          mergeDataError( imalpha, imalphaerror, alphaerrorname+".new" );

	}

      ////// Calculate beta
      if(ntaylor_p>2)
	{
	  PagedImage<Float> imbeta(image(baseindex).shape(),image(baseindex).coordinates(),betaname); 
	  imbeta.set(0.0);
	  PagedImage<Float> imtaylor2(restoredNames[getModelIndex(field,2)]);
	  
	  LatticeExpr<Float> betacalc( ((imtaylor2)*mask1)/((imtaylor0)+(mask0))-0.5*(imalpha)*(imalpha-1.0) );
	  imbeta.copyData(betacalc);
	  imbeta.setImageInfo(ii);
	  //imbeta.setUnits(Unit("Spectral Curvature"));
          createMask(lemask, imbeta);
	  imbeta.table().unmarkForDelete();

	  os << "Written Spectral Curvature Image : " << betaname << LogIO::POST;
	}
      
    }// field loop

  return 0;

}
/***********************************************************************/

Bool WBCleanImageSkyModel::createMask(LatticeExpr<Bool> &lemask, ImageInterface<Float> &outimage)
{
      ImageRegion outreg = outimage.makeMask("mask0",false,true);
      LCRegion& outmask=outreg.asMask();
      outmask.copyData(lemask);
      outimage.defineRegion("mask0",outreg, RegionHandler::Masks, true);
      outimage.setDefaultMask("mask0");
      return true;
}


/***********************************************************************/
Bool WBCleanImageSkyModel::calculateCoeffResiduals()
{
  for(Int field=0;field<nfields_p;field++)
    {
      Int baseindex = getModelIndex(field,0);

      
      // Send in the final residuals
      for (Int order=0;order<ntaylor_p;order++)
	{
	  Int index = getModelIndex(field,order);
	  Matrix<Float> tempMat;
	  Array<Float> tempArr;
	  (residual(index)).get(tempArr,true);
	  tempMat.reference(tempArr);
	  lc_p[baseindex].setresidual(order,tempMat); 
	}      
      
      // Compute principal solution in-place.
      lc_p[baseindex].computeprincipalsolution();

      // Get the new residuals
      for (Int order=0;order<ntaylor_p;order++)
	{
	  Int index = getModelIndex(field,order);
	  Matrix<Float> tempMod;
	  lc_p[baseindex].getresidual(order,tempMod);
	  residual(index).put(tempMod);
	}

      
      /*

      //Apply Inverse Hessian to the residuals 
      IPosition gip(4,image(baseindex).shape()[0],image(baseindex).shape()[1],1,1);
      Matrix<Double> invhessian;
      lc_p[field].getinvhessian(invhessian);
      //cout << "Inverse Hessian : " << invhessian << endl;
      
      Int tindex;
      LatticeExprNode len_p;
      PtrBlock<TempLattice<Float>* > coeffresiduals(ntaylor_p); //,smoothresiduals(ntaylor_p);
      for(Int taylor1=0;taylor1<ntaylor_p;taylor1++)
	{
	  coeffresiduals[taylor1] = new TempLattice<Float>(gip,memoryMB_p);
	}
      
      //Apply the inverse Hessian to the residuals 
      for(Int taylor1=0;taylor1<ntaylor_p;taylor1++)
	{
	  len_p = LatticeExprNode(0.0);
	  for(Int taylor2=0;taylor2<ntaylor_p;taylor2++)
	    {
	      tindex = getModelIndex(field,taylor2);
	      len_p = len_p + LatticeExprNode((Float)(invhessian)(taylor1,taylor2)*(residual(tindex)));
	    }
	  (*coeffresiduals[taylor1]).copyData(LatticeExpr<Float>(len_p));
	}

      //Fill in the residual images with these coefficient residuals 
      for(Int taylor=0;taylor<ntaylor_p;taylor++)
	{
          tindex = getModelIndex(field,taylor);
	  (residual(tindex)).copyData(LatticeExpr<Float>(*coeffresiduals[taylor]));
	}

      for(uInt i=0;i<coeffresiduals.nelements();i++) 
	{
	  if(coeffresiduals[i]) delete coeffresiduals[i];
	}
      */


    }//end of field loop
  os << "Converting final residuals to 'coefficient residuals', for restoration" << LogIO::POST;
  return true;
}//end of calculateCoeffResiduals

/***********************************************************************/
///// Write alpha and beta to disk. Calculate from smoothed model + residuals.
Int WBCleanImageSkyModel::writeResultsToDisk()
{
  if(ntaylor_p<=1) return 0;
  
  os << "Output Taylor-coefficient images : " << imageNames << LogIO::POST;
  //  if(ntaylor_p==2) os << "Calculating Spectral Index" << LogIO::POST;
  //  if(ntaylor_p>2) os << "Calculating Spectral Index and Curvature" << LogIO::POST;
  
  
  GaussianBeam beam;
  Int index=0;
  
  for(Int field=0;field<nfields_p;field++)
    {
      PtrBlock<TempLattice<Float>* > smoothed;
      if(ntaylor_p>2) smoothed.resize(3);
      else if(ntaylor_p==2) smoothed.resize(2);
      
      Int baseindex = getModelIndex(field,0);
      String alphaname,betaname;
      if(  ( (imageNames[baseindex]).substr( (imageNames[baseindex]).length()-3 , 3 ) ).matches("tt0") )
	{
	  alphaname = (imageNames[baseindex]).substr(0,(imageNames[baseindex]).length()-3) + "alpha";
	  betaname = (imageNames[baseindex]).substr(0,(imageNames[baseindex]).length()-3) + "beta";
	}
      else
	{
	  alphaname = (imageNames[baseindex]) +  String(".alpha");
	  betaname = (imageNames[baseindex]) +  String(".beta");
	}
      
      StokesImageUtil::FitGaussianPSF(PSF(baseindex), beam);
      
      /* Create empty alpha image */
      PagedImage<Float> imalpha(image(baseindex).shape(),image(baseindex).coordinates(),alphaname); 
      imalpha.set(0.0);
      
      /* Apply Inverse Hessian to the residuals */
      IPosition gip(4,image(baseindex).shape()[0],image(baseindex).shape()[1],1,1);
      Matrix<Double> invhessian;
      lc_p[field].getinvhessian(invhessian);
      //cout << "Inverse Hessian : " << invhessian << endl;
      
      Int tindex;
      LatticeExprNode len_p;
      PtrBlock<TempLattice<Float>* > coeffresiduals(ntaylor_p); //,smoothresiduals(ntaylor_p);
      for(Int taylor1=0;taylor1<ntaylor_p;taylor1++)
	{
	  coeffresiduals[taylor1] = new TempLattice<Float>(gip,memoryMB_p);
	}
      
      /* Apply the inverse Hessian to the residuals */
      for(Int taylor1=0;taylor1<ntaylor_p;taylor1++)
	{
	  len_p = LatticeExprNode(0.0);
	  for(Int taylor2=0;taylor2<ntaylor_p;taylor2++)
	    {
	      tindex = getModelIndex(field,taylor2);
	      len_p = len_p + LatticeExprNode((Float)(invhessian)(taylor1,taylor2)*(residual(tindex)));
	    }
	  (*coeffresiduals[taylor1]).copyData(LatticeExpr<Float>(len_p));
	}

      /* Fill in the residual images with these coefficient residuals */
      for(Int taylor=0;taylor<ntaylor_p;taylor++)
	{
          tindex = getModelIndex(field,taylor);
	  (residual(taylor)).copyData(LatticeExpr<Float>(*coeffresiduals[taylor]));
	}


      /* Smooth the model images and add the above coefficient residuals */
      for(uInt i=0;i<smoothed.nelements();i++)
	{
	  smoothed[i] = new TempLattice<Float>(gip,memoryMB_p);
	  
	  index = getModelIndex(field,i);
	  LatticeExpr<Float> cop(image(index));
	  imalpha.copyData(cop);
	  StokesImageUtil::Convolve(imalpha, beam);
	  //cout << "Clean Beam from WBC : " << bmaj  << " , " << bmin << " , " << bpa << endl;
          //cout << "SkyModel internally-recorded beam for index " << index << " : " << beam(index) << endl;
	  //LatticeExpr<Float> le(imalpha); 
	  LatticeExpr<Float> le(imalpha+( *coeffresiduals[i] )); 
	  (*smoothed[i]).copyData(le);
	}
      
      
      /* Create a mask - make this adapt to the signal-to-noise */
      LatticeExprNode leMaxRes=max(residual( getModelIndex(field,0) ));
      Float maxres = leMaxRes.getFloat();
      // Threshold is either 10% of the peak residual (psf sidelobe level) or 
      // user threshold, if deconvolution has gone that deep.
      Float specthreshold = MAX( threshold()*5 , maxres/5.0 );
      os << "Calculating spectral parameters for  Intensity > MAX(threshold*5,peakresidual/5) = " << specthreshold << " Jy/beam" << LogIO::POST;
      LatticeExpr<Float> mask1(iif((*smoothed[0])>(specthreshold),1.0,0.0));
      LatticeExpr<Float> mask0(iif((*smoothed[0])>(specthreshold),0.0,1.0));
      
      /* Calculate alpha and beta */
      LatticeExpr<Float> alphacalc( ((*smoothed[1])*mask1)/((*smoothed[0])+(mask0)) );
      imalpha.copyData(alphacalc);
      
      ImageInfo ii = imalpha.imageInfo();
      ii.setRestoringBeam(beam);
      
      imalpha.setImageInfo(ii);
      //imalpha.setUnits(Unit("Spectral Index"));
      imalpha.table().unmarkForDelete();
      os << "Written Spectral Index Image : " << alphaname << LogIO::POST;
      
      if(ntaylor_p>2)
	{
	  PagedImage<Float> imbeta(image(baseindex).shape(),image(baseindex).coordinates(),betaname); 
	  imbeta.set(0.0);
	  
	  LatticeExpr<Float> betacalc( ((*smoothed[2])*mask1)/((*smoothed[0])+(mask0))-0.5*(imalpha)*(imalpha-1.0) );
	  imbeta.copyData(betacalc);
	  
	  imbeta.setImageInfo(ii);
	  //imbeta.setUnits(Unit("Spectral Curvature"));
	  imbeta.table().unmarkForDelete();
	  os << "Written Spectral Curvature Image : " << betaname << LogIO::POST;
	}
      
      /* Print out debugging info for center pixel */
      /*
	IPosition cgip(4,512,512,0,0);
	IPosition cgip2(4,490,542,0,0);
	for(Int i=0;i<ntaylor_p;i++)
	{
	cout << "Extended : " << endl;
	cout << "Original residual : " << i << " : " << residual( getModelIndex(model,i) ).getAt(cgip) << endl;
	cout << "Smoothed residual : " << i << " : " << (*smoothresiduals[i]).getAt(cgip) << endl;
	cout << "Coeff residual : " << i << " : " << (*coeffresiduals[i]).getAt(cgip) << endl;
	cout << "Point : " << endl;
	cout << "Original residual : " << i << " : " << residual( getModelIndex(model,i) ).getAt(cgip2) << endl;
	cout << "Smoothed residual : " << i << " : " << (*smoothresiduals[i]).getAt(cgip2) << endl;
	cout << "Coeff residual : " << i << " : " << (*coeffresiduals[i]).getAt(cgip2) << endl;
	}
      */
      
      
      /* Clean up temp arrays */
      for(uInt i=0;i<smoothed.nelements();i++) if(smoothed[i]) delete smoothed[i];
      for(uInt i=0;i<coeffresiduals.nelements();i++) 
	{
	  if(coeffresiduals[i]) delete coeffresiduals[i];
	}
      
    }// field loop
  return 0;
}

/***********************************************************************/
/*************************************
 *          Make Residuals and compute the current peak  
 *************************************/
Bool WBCleanImageSkyModel::solveResiduals(SkyEquation& se, Bool modelToMS) 
{
  blankOverlappingModels();
  makeNewtonRaphsonStep(se,false,modelToMS);
  restoreOverlappingModels();
  
  return true;
}
/***********************************************************************/

/*************************************
 *          Make Residuals 
 *************************************/
// Currently - all normalized by ggS from taylor0.
// SAME WITH MAKE_SPECTRAL_PSFS

Bool WBCleanImageSkyModel::makeNewtonRaphsonStep(SkyEquation& se, Bool incremental, Bool modelToMS) 
{
  LogIO os(LogOrigin("WBCleanImageSkyModel", "makeNewtonRaphsonStep"));
  se.gradientsChiSquared(incremental, modelToMS);
  
  Int index=0,baseindex=0;
  LatticeExpr<Float> le;
  for(Int thismodel=0;thismodel<nfields_p;thismodel++) 
    {
      baseindex = getModelIndex(thismodel,0);
      for(Int taylor=0;taylor<ntaylor_p;taylor++)
	{
	  /* Normalize by the Taylor 0 weight image */
	  index = getModelIndex(thismodel,taylor);
	  //cout << "Normalizing image " << index << " with " << baseindex << endl;
	  //cout << "Shapes : " << ggS(index).shape() << gS(baseindex).shape() << endl;
	  
	  // UUU
	  //	  LatticeExpr<Float> le(iif(ggS(baseindex)>(0.0), -gS(index)/ggS(baseindex), 0.0));

	  ///cout << "WBC : makeNRs : isnormalized : " << isImageNormalized() << endl;

	  if (isImageNormalized()) le = LatticeExpr<Float>(gS(index));
	  else                   le = LatticeExpr<Float>(iif(ggS(baseindex)>(0.0), 
							     -gS(index)/ggS(baseindex), 0.0));
	  residual(index).copyData(le);

	  //LatticeExprNode LEN = max( residual(index) );
	  //LatticeExprNode len2 = max( ggS(baseindex) );
	  //cout << "Max Residual : " << LEN.getFloat() << "  Max ggS : " << len2.getFloat() << endl;


	  //storeAsImg(String("Weight.")+String::toString(thismodel)+String(".")+String::toString(taylor),ggS(index));
	  //storeAsImg(String("TstResidual.")+String::toString(thismodel)+String(".")+String::toString(taylor),residual(index));
	}
    }
  modified_p=false;
  return true;
}

/*************************************
 *          Make Approx PSFs. 
 *************************************/
// The normalization ignores that done in makeSimplePSFs in the Sky Eqn
// and recalculates it from gS and ggS.
Int WBCleanImageSkyModel::makeSpectralPSFs(SkyEquation& se, Bool writeToDisk) 
{
  LogIO os(LogOrigin("WBCleanImageSkyModel", "makeSpectralPSFs"));
  if(!donePSF_p)
    {
      //make sure the psf images are made
      for (Int thismodel=0;thismodel<nmodels_p;thismodel++) 
	{
	  PSF(thismodel);
	}
    }
  
  // All 2N-1 psfs will get made here....
  se.makeApproxPSF(psf_p);  
  
  // Normalize
  Float normfactor=1.0;
  Int index=0,baseindex=0;
  LatticeExpr<Float> le;
  for (Int thismodel=0;thismodel<nfields_p;thismodel++) 
    {
      normfactor=1.0;
      baseindex = getModelIndex(thismodel,0);
      for(Int taylor=0;taylor<2*ntaylor_p-1;taylor++)
	{
	  /* Normalize by the Taylor 0 weight image */
	  index = getModelIndex(thismodel,taylor);
	  //cout << "Normalizing PSF " << index << " with " << baseindex << endl;
	  //cout << "Shapes : " << ggS(index).shape() << gS(baseindex).shape() << endl;
	  //le = LatticeExpr<Float>(iif(ggS(baseindex)>(0.0), gS(index)/ggS(baseindex), 0.0));
	  if (isImageNormalized()) le = LatticeExpr<Float>(gS(index));
	  else                   le = LatticeExpr<Float>(iif(ggS(baseindex)>(0.0), 
							     gS(index)/ggS(baseindex), 0.0));
	  PSF(index).copyData(le);
	  if(taylor==0)
	    { 
	      LatticeExprNode maxPSF=max(PSF(index));
	      normfactor = maxPSF.getFloat();
	      if(adbg) os << "Normalize PSFs for field " << thismodel << " by " << normfactor << LogIO::POST;
	    }
	  LatticeExpr<Float> lenorm(PSF(index)/normfactor);
	  PSF(index).copyData(lenorm);
	  LatticeExprNode maxPSF2=max(PSF(index));
	  Float maxpsf=maxPSF2.getFloat();
	  if(adbg) os << "Psf for Model " << thismodel << " and Taylor " << taylor << " has peak " << maxpsf << LogIO::POST;

	  if(writeToDisk)
	    {
	      //need unique name as multiple processes can be running in the 
	      //same workingdir
	      String tmpName=image_p[thismodel]->name();
	      tmpName.del(String(".model.tt0"), 0);
	      storeAsImg(tmpName+String(".TempPsf.")+String::toString(taylor),PSF(index));
	    }
	}
      
      //     index = getModelIndex(thismodel,0);
      //beam(thismodel)=0.0;
      if(!StokesImageUtil::FitGaussianPSF(PSF(baseindex),beam(thismodel))) 
	os << "Beam fit failed: using default" << LogIO::POST;
    }
  return 0;
}



/***********************************************************************/

/*************************************
 *          Store a Templattice as an image
 *************************************/

  Int WBCleanImageSkyModel::storeAsImg(String fileName, ImageInterface<Float>& theImg)
  {
  PagedImage<Float> tmp(theImg.shape(), theImg.coordinates(), fileName);
  LatticeExpr<Float> le(theImg);
  tmp.copyData(le);
  return 0;
  }
/*  
  Int WBCleanImageSkyModel::storeTLAsImg(String fileName, TempLattice<Float> &TL, ImageInterface<Float>& theImg)
  {
  PagedImage<Float> tmp(TL.shape(), theImg.coordinates(), fileName);
  LatticeExpr<Float> le(TL);
  tmp.copyData(le);
  return 0;
  }
  
  Int WBCleanImageSkyModel::storeTLAsImg(String fileName, TempLattice<Complex> &TL, ImageInterface<Float>& theImg)
  {
  PagedImage<Complex> tmp(TL.shape(), theImg.coordinates(), fileName);
  LatticeExpr<Complex> le(TL);
  tmp.copyData(le);
  return 0;
  }
*/

/**************************
  Resize Work Arrays to calculate extra PSFs.
  (Resizing back to a shorter list must free memory correctly).
*************************/
Bool WBCleanImageSkyModel::resizeWorkArrays(Int length)
{
  Int originallength = gS_p.nelements();
  
  if(length < originallength) // Clean up extra arrays
    {
      for(Int i = length; i < originallength; ++i)
	{
	  if(psf_p[i]){delete psf_p[i]; psf_p[i]=0;}
	  if(image_p[i]){delete image_p[i]; image_p[i]=0;}
	  if(cimage_p[i]){delete cimage_p[i]; cimage_p[i]=0;}
	  if(gS_p[i]){delete gS_p[i]; gS_p[i]=0;}
	  if(ggS_p[i]){delete ggS_p[i]; ggS_p[i]=0;}
	  if(work_p[i]){delete work_p[i]; work_p[i]=0;}
	  if(fluxScale_p[i]){delete fluxScale_p[i]; fluxScale_p[i]=0;}
	}

    }
  
  psf_p.resize(length,true);
  image_p.resize(length,true);
  cimage_p.resize(length,true);
  gS_p.resize(length,true);
  ggS_p.resize(length,true);
  work_p.resize(length,true);
  fluxScale_p.resize(length,true);
  
  if(length > originallength) // Add extra arrays  // TODO : This part can go - I think !!!
    {
      for(Int i = originallength; i < length; ++i)
	{
	  psf_p[i]=0;gS_p[i]=0;ggS_p[i]=0;cimage_p[i]=0;work_p[i]=0;fluxScale_p[i]=0;
	  
	  Int ind = getFieldIndex(i);
	  TempImage<Float>* imptr = 
	    new TempImage<Float> (IPosition(image_p[ind]->ndim(),
					    image_p[ind]->shape()(0),
					    image_p[ind]->shape()(1),
					    image_p[ind]->shape()(2),
					    image_p[ind]->shape()(3)),
				  image_p[ind]->coordinates(), memoryMB_p);
	  AlwaysAssert(imptr, AipsError);
	  image_p[i] = imptr;
	}
    }

  ///cout << "Memory held by ImageSkyModel : " << 6*length << " float + " << 1*length << " complex images " << endl;

  return true;
}

/************************************************************************************
Check some input parameters and print warnings for the user  
	 fbw = (fmax-fmin)/fref.  
	   if(fbw < 0.1) and nterms>2 
	       => lever-arm may be insufficient for more than alpha.
	       => polynomial fit will work but alpha interpretation may not be ok.
	   if(ref < fmin or ref > fmax) 
	       => polynomial fit will work, but alpha interpretation will not be right.
	   if(nchan==1) or fbw = 0, then ask to use only nterms=1, or choose more than one chan 

***********************************************************************************/
Bool WBCleanImageSkyModel::checkParameters()
{
  /* Check ntaylor_p, nrefFrequency_p with min and max freq from the image-coords */
  
  for(uInt i=0; i<image(0).coordinates().nCoordinates(); i++)
    {
      if( image(0).coordinates().type(i) == Coordinate::SPECTRAL )
	{
	  SpectralCoordinate speccoord(image(0).coordinates().spectralCoordinate(i));
	  Double startfreq=0.0,startpixel=-0.5;
	  Double endfreq=0.0,endpixel=+0.5;
	  speccoord.toWorld(startfreq,startpixel);
	  speccoord.toWorld(endfreq,endpixel);
	  Float fbw = (endfreq - startfreq)/refFrequency_p;
	  //os << "Freq range of the mfs channel : " << startfreq << " -> " << endfreq << endl;
	  //cout << "Fractional bandwidth : " << fbw << endl;
	  
	  os << "Fractional Bandwidth : " << fbw*100 << " %." << LogIO::POST;
	  
	  /*
	    if(fbw < 0.1 && ntaylor_p == 2 )
	    os << "Fractional Bandwidth is " << fbw*100 << " %. Please check that the flux variation across the chosen frequency range (" << startfreq << " Hz to " << endfreq << " Hz) is at least twice the single-channel noise-level. If not, please use nterms=1." << LogIO::WARN << LogIO::POST; 
	    
	    if(fbw < 0.1 && ntaylor_p > 2)
	    os << "Fractional Bandwidth is " << fbw*100 << " %. Please check that (a) the flux variation across the chosen frequency range (" << startfreq << " Hz to " << endfreq << " Hz) is at least twice the single-channel noise-level, and (b) a " << ntaylor_p << "-term Taylor-polynomial fit across this frequency range is appropriate. " << LogIO::WARN << LogIO::POST; 
	  */
	  if(refFrequency_p < startfreq || refFrequency_p > endfreq)
	    os << "A Reference frequency of " << refFrequency_p << "Hz is outside the frequency range of the selected data (" << startfreq << " Hz to " << endfreq << " Hz). A power-law interpretation of the resulting Taylor-coefficients may not be accurate." << LogIO::POST;
	  
	}
    }
  
  
  return true;
}

// Copied from MFCleanImageSkyModel
void WBCleanImageSkyModel::blankOverlappingModels(){
  if(nfields_p == 1)  return;
  
  for(Int taylor=0;taylor<ntaylor_p;taylor++)
    {
      for (Int field=0;field<nfields_p-1; ++field) 
	{
	  Int model=getModelIndex(field,taylor);
	  CoordinateSystem cs0=image(model).coordinates();
	  IPosition iblc0(image(model).shape().nelements(),0);
	  
	  IPosition itrc0(image(model).shape());
	  itrc0=itrc0-Int(1);
	  LCBox lbox0(iblc0, itrc0, image(model).shape());
	  
	  ImageRegion imagreg0(WCBox(lbox0, cs0));
	  for (Int nextfield=field+1; nextfield < nfields_p; ++nextfield)
	    {
	      Int nextmodel = getModelIndex(nextfield, taylor);
	      CoordinateSystem cs=image(nextmodel).coordinates();
	      IPosition iblc(image(nextmodel).shape().nelements(),0);
	      
	      IPosition itrc(image(nextmodel).shape());
	      itrc=itrc-Int(1);
	      
	      LCBox lbox(iblc, itrc, image(nextmodel).shape());
	      
	      ImageRegion imagreg(WCBox(lbox, cs));
	      try{
		LatticeRegion latReg=imagreg.toLatticeRegion(image(model).coordinates(), image(model).shape());
		ArrayLattice<Bool> pixmask(latReg.get());
		SubImage<Float> partToMask(image(model), imagreg, true);
		LatticeExpr<Float> myexpr(iif(pixmask, 0.0, partToMask) );
		partToMask.copyData(myexpr);
		
	      }
	      catch(...){
		//no overlap you think ?
		//cout << "Did i fail " << endl;
		continue;
	      }
	      
	      
	    }
	  
	  
	  
	}// for field
    }// for taylor
}

// Copied from MFCleanImageSkyModel
void WBCleanImageSkyModel::restoreOverlappingModels(){
  LogIO os(LogOrigin("WBImageSkyModel","restoreOverlappingModels"));
  if(nfields_p == 1)  return;
  
  for(Int taylor=0;taylor<ntaylor_p;taylor++)
    {
      
      for (Int field=0;field<nfields_p-1; ++field) 
	{
	  Int model = getModelIndex(field,taylor);
	  CoordinateSystem cs0=image(model).coordinates();
	  IPosition iblc0(image(model).shape().nelements(),0);
	  IPosition itrc0(image(model).shape());
	  itrc0=itrc0-Int(1);
	  LCBox lbox0(iblc0, itrc0, image(model).shape());
	  ImageRegion imagreg0(WCBox(lbox0, cs0));
	  
	  for (Int nextfield=field+1; nextfield < nfields_p; ++nextfield)
	    {
	      Int nextmodel = getModelIndex(nextfield,taylor);
	      CoordinateSystem cs=image(nextmodel).coordinates();
	      IPosition iblc(image(nextmodel).shape().nelements(),0);
	      
	      IPosition itrc(image(nextmodel).shape());
	      itrc=itrc-Int(1);
	      
	      LCBox lbox(iblc, itrc, image(nextmodel).shape());
	      
	      ImageRegion imagreg(WCBox(lbox, cs));
	      try{
		LatticeRegion latReg0=imagreg0.toLatticeRegion(image(nextmodel).coordinates(), image(nextmodel).shape());
		LatticeRegion latReg=imagreg.toLatticeRegion(image(model).coordinates(), image(model).shape());
		ArrayLattice<Bool> pixmask(latReg.get());

		SubImage<Float> partToMerge(image(nextmodel), imagreg0, true);
		SubImage<Float> partToUnmask(image(model), imagreg, true);
		
		LatticeExpr<Float> myexpr0(iif(pixmask,partToMerge,partToUnmask));
		partToUnmask.copyData(myexpr0);
		
	      }
	      catch(AipsError x){
		/*
		  os << LogIO::WARN
		  << "no overlap or failure of copying the clean components"
		  << x.getMesg()
		  << LogIO::POST;
		*/
		continue;
	      }
	    }// for field - inner
	}// for field - outer
    }// for taylor
}

Bool WBCleanImageSkyModel::mergeDataError(ImageInterface<Float> &data, ImageInterface<Float> &error, const String &outImg)
///Bool WBCleanImageSkyModel::mergeDataError(const String &dataImg, const String &errorImg, const String &outImg)
{
  LogIO os(LogOrigin("WBImageSkyModel",__FUNCTION__));

       // open the data and the error image
  //       ImageInterface<Float>  *data  = new PagedImage<Float>(dataImg, TableLock::AutoNoReadLocking);
  //       ImageInterface<Float>  *error = new PagedImage<Float>(errorImg, TableLock::AutoNoReadLocking);

       // create the tiled shape for the output image
       IPosition newShape=IPosition(data.shape());
       newShape.append(IPosition(1, 2));
       TiledShape tiledShape(newShape);

       // create the coordinate system for the output image
       CoordinateSystem newCSys = data.coordinates();
       Vector<Int> quality(2);
       quality(0) = Quality::DATA;
       quality(1) = Quality::ERROR;
       QualityCoordinate qualAxis(quality);
       newCSys.addCoordinate(qualAxis);

       Array<Float> outData=Array<Float>(newShape, 0.0f);
       Array<Bool>  outMask;

       // get all the data values
       Array<Float> inData;
       Slicer inSlicer(IPosition((data.shape()).size(), 0), IPosition(data.shape()));
       data.doGetSlice(inData, inSlicer);

       // define in the output array
       // the slicers for data and error
       Int qualCooPos, qualIndex;
       qualCooPos = newCSys.findCoordinate(Coordinate::QUALITY);
       (newCSys.qualityCoordinate(qualCooPos)).toPixel(qualIndex, Quality::DATA);
       IPosition outStart(newShape.size(), 0);
       outStart(newShape.size()-1)=qualIndex;
       IPosition outLength(newShape);
       outLength(newShape.size()-1)=1;
       Slicer outDataSlice(outStart, outLength);
       (newCSys.qualityCoordinate(qualCooPos)).toPixel(qualIndex, Quality::ERROR);
       outStart(newShape.size()-1)=qualIndex;
       Slicer outErrorSlice(outStart, outLength);

       // add the data values to the output array
       outData(outDataSlice) = inData.addDegenerate(1);

       // get all the error values
       error.doGetSlice(inData, inSlicer);


       // add the error values to the output array
       outData(outErrorSlice) = inData.addDegenerate(1);

       // check whether a mask is necessary
       if (data.hasPixelMask() || error.hasPixelMask()){
               Array<Bool> inMask;

               outMask=Array<Bool>(newShape, true);

               // make the mask for the data values
               if (data.hasPixelMask()){
                       inMask  = (data.pixelMask()).get();
               }
               else{
                       inMask = Array<Bool>(data.shape(), true);
               }

               // add the data mask to the output
               outMask(outDataSlice)  = inMask.addDegenerate(1);

               // make the mask for the error values
               if (error.hasPixelMask()){
                       inMask  = (error.pixelMask()).get();
               }
               else{
                       inMask = Array<Bool>(error.shape(), true);
               }

               // add the data mask to the output
               outMask(outErrorSlice) = inMask.addDegenerate(1);
       }


  // write out the combined image
       ImageUtilities::writeImage(tiledShape, newCSys, outImg, outData, os, outMask);

       //       delete data;
       //   delete error;

       return true;
}

} //# NAMESPACE CASA - END

