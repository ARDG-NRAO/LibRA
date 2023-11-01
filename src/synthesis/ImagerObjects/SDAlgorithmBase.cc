//# SDAlgorithmBase.cc: Implementation of SDAlgorithmBase classes
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
//# $Id$

#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/OS/HostInfo.h>
#include <synthesis/ImagerObjects/SDAlgorithmBase.h>
#include <components/ComponentModels/SkyComponent.h>
#include <components/ComponentModels/ComponentList.h>
#include <casacore/images/Images/TempImage.h>
#include <casacore/images/Images/SubImage.h>
#include <casacore/images/Regions/ImageRegion.h>
#include <casacore/casa/OS/File.h>
#include <casacore/lattices/LEL/LatticeExpr.h>
#include <casacore/lattices/Lattices/TiledLineStepper.h>
#include <casacore/lattices/Lattices/LatticeStepper.h>
#include <casacore/lattices/Lattices/LatticeIterator.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <casacore/coordinates/Coordinates/StokesCoordinate.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/OS/Directory.h>
#include <casacore/tables/Tables/TableLock.h>

#include<synthesis/ImagerObjects/SIMinorCycleController.h>
#include <imageanalysis/ImageAnalysis/CasaImageBeamSet.h>

#include <sstream>
#include <casacore/casa/OS/EnvVar.h>

#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Logging/LogSink.h>

// supporting code for profileMinorCycle()
#include <thread>
#include <chrono>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN


  SDAlgorithmBase::SDAlgorithmBase():
    itsAlgorithmName("Test")
    //    itsDecSlices (),
    //    itsResidual(), itsPsf(), itsModel()
 {
 }

  SDAlgorithmBase::~SDAlgorithmBase()
 {
   
 }


  void SDAlgorithmBase::deconvolve( SIMinorCycleController &loopcontrols, 
				    std::shared_ptr<SIImageStore> &imagestore,
				    Int deconvolverid,
                                    Bool isautomasking, Bool fastnoise, Record robuststats)
  {
    LogIO os( LogOrigin("SDAlgorithmBase","deconvolve",WHERE) );

    // Make a list of Slicers.
    //partitionImages( imagestore );

    Int nSubChans, nSubPols;
    queryDesiredShape(nSubChans, nSubPols, imagestore->getShape());

    //    cout << "Check imstore from deconvolver : " << endl;
    imagestore->validate();

    //itsImages = imagestore;

    //    loadMask();

    //os << "-------------------------------------------------------------------------------------------------------------" << LogIO::POST;


    os << "[" << imagestore->getName() << "]";
    os << " Run " << itsAlgorithmName << " minor-cycle " ;
    if( nSubChans>1 || nSubPols>1 ) os << "on ";
    if( nSubChans >1 ) os << nSubChans << " chans " ;
    if( nSubPols >1 ) os << nSubPols << " pols ";
    os << "| CycleThreshold=" << loopcontrols.getCycleThreshold()
       << ", CycleNiter=" << loopcontrols.getCycleNiter() 
       << ", Gain=" << loopcontrols.getLoopGain()
       << LogIO::POST;

    Float itsPBMask = loopcontrols.getPBMask();
    Int cycleStartIteration = loopcontrols.getIterDone();

    Float maxResidualAcrossPlanes=0.0; Int maxResChan=0,maxResPol=0;
    Float totalFluxAcrossPlanes=0.0;

    for( Int chanid=0; chanid<nSubChans;chanid++)
      {
	for( Int polid=0; polid<nSubPols; polid++)
	  {
	    //	    itsImages = imagestore->getSubImageStoreOld( chanid, onechan, polid, onepol );
	    itsImages = imagestore->getSubImageStore( 0, 1, chanid, nSubChans, polid, nSubPols );

	    Int startiteration = loopcontrols.getIterDone(); // TODO : CAS-8767 key off subimage index
	    Float peakresidual=0.0, peakresidualnomask=0.0;
	    Float modelflux=0.0;
	    Int iterdone=0;

	    ///itsMaskHandler.resetMask( itsImages ); //, (loopcontrols.getCycleThreshold()/peakresidual) );
	    Int stopCode=0;

	    Float startpeakresidual = 0.0, startpeakresidualnomask = 0.0;
	    Float startmodelflux = 0.0;
            Array<Double> robustrms;

	    Float masksum = itsImages->getMaskSum();
	    Bool validMask = ( masksum > 0 );

	    peakresidualnomask = itsImages->getPeakResidual();
	    if( validMask ) peakresidual = itsImages->getPeakResidualWithinMask();
	    else peakresidual = peakresidualnomask;
	    modelflux = itsImages->getModelFlux();

	    startpeakresidual = peakresidual;
	    startpeakresidualnomask = peakresidualnomask;
	    startmodelflux = modelflux;

            //Float nsigma = 150.0; // will set by user, fixed for 3sigma for now.
            Float nsigma = loopcontrols.getNsigma();
            //os<<"robustrms nelements="<<robustrms.nelements()<<LogIO::POST;
            Float nsigmathresh; 
            if (robustrms.nelements()==0) {
              nsigmathresh = 0.0; 
            } else{
              nsigmathresh = nsigma * (Float)robustrms(IPosition(1,0)); 
            }

            ContextBoundBool stopProfiling(false); // will be set to true when loop exits
            uLong peakMem = 0;
            std::thread profileMemThread(&SDAlgorithmBase::profileMinorCycle, this, std::ref(stopProfiling), std::ref(peakMem));
            std::chrono::steady_clock::time_point profileStartTime = std::chrono::steady_clock::now();
              
            Float thresholdtouse;
            if (nsigma>0.0) {
              // returns as an Array but itsImages is already single plane so 
              // the return rms contains only a single element
              Array<Double> medians;
              //os<<"robuststats rec size="<<robuststats.nfields()<<LogIO::POST;
              Bool statsexists = false;
              IPosition statsindex;
              if (robuststats.nfields()) {
                // use existing stats
                if (robuststats.isDefined("robustrms")) {
                  robuststats.get(RecordFieldId("robustrms"), robustrms);
                  robuststats.get(RecordFieldId("median"), medians);
                  statsexists=True;
		  
                }
                else if(robuststats.isDefined("medabsdevmed")) {
                  Array<Double> mads;
                  robuststats.get(RecordFieldId("medabsdevmed"), mads);
                  robuststats.get(RecordFieldId("median"), medians);
                  robustrms = mads * 1.4826; // convert to rms
                  statsexists=True;
		  
                }
                //statsindex=IPosition(1,chanid); // this only support for npol =1, need to fix this
		if((robustrms.shape().nelements() ==2) && (robustrms.shape()[0] > polid) &&  (robustrms.shape()[1] > chanid) )
		  statsindex=IPosition(2,polid,chanid);
		else if((robustrms.shape().nelements() ==1) && (robustrms.shape()[0] > chanid))
		  statsindex=IPosition(1,chanid);
		else ///no idea what got passed in the record
		  statsexists=False;
		
              }
              if (statsexists) {
                os<<LogIO::DEBUG1<<"Using the existing robust image statatistics!"<<LogIO::POST;
              } 
              else {
                robustrms = itsImages->calcRobustRMS(medians, itsPBMask, fastnoise);
                statsindex=IPosition(1,0);
              }
              if (isautomasking) { // new threshold defination 
                //nsigmathresh = (Float)medians(IPosition(1,0)) + nsigma * (Float)robustrms(IPosition(1,0));
                nsigmathresh = (Float)medians(statsindex) + nsigma * (Float)robustrms(statsindex);
              }
              else {
                nsigmathresh = nsigma * (Float)robustrms(statsindex);
              }
              thresholdtouse = max( nsigmathresh, loopcontrols.getCycleThreshold());
            }
            else {
              thresholdtouse = loopcontrols.getCycleThreshold();
            }
            //os << LogIO::DEBUG1<<"loopcontrols.getCycleThreshold()="<<loopcontrols.getCycleThreshold()<<LogIO::POST;
            os << LogIO::NORMAL3<<"current CycleThreshold="<<loopcontrols.getCycleThreshold()<<" nsigma threshold="<<nsigmathresh<<LogIO::POST;
            String thresholddesc = (thresholdtouse == loopcontrols.getCycleThreshold() ? "cyclethreshold" : "n-sigma");
            os << LogIO::NORMAL3<< "thresholdtouse="<< thresholdtouse << "("<<thresholddesc<<")"<< LogIO::POST;

            if (thresholddesc=="n-sigma") {
              //os << LogIO::DEBUG1<< "Set nsigma thresh="<<nsigmathresh<<LogIO::POST;
              loopcontrols.setNsigmaThreshold(nsigmathresh);
            }
	    loopcontrols.setPeakResidual( peakresidual );
	    loopcontrols.resetMinResidual(); // Set it to current initial peakresidual.
            
	    stopCode = checkStop( loopcontrols,  peakresidual );

	    if( validMask && stopCode==0 )
	      {
		// Init the deconvolver
                 //where we write in model and residual may be
                {

                  LatticeLocker lockresid (*(itsImages->residual()), FileLocker::Read);
                  LatticeLocker lockmodel (*(itsImages->model()), FileLocker::Read);
                  LatticeLocker lockmask (*(itsImages->mask()), FileLocker::Read);
                  LatticeLocker lockpsf (*(itsImages->psf()), FileLocker::Read);
		  
                  initializeDeconvolver();
                }
                
		while ( stopCode==0 )
		  {

                    if (nsigma>0.0) {
                      os << "Using " << thresholddesc << " for threshold criterion: (cyclethreshold="<<loopcontrols.getCycleThreshold()<< ", nsigma threshold="<<nsigmathresh<<" )" << LogIO::POST;
                      loopcontrols.setNsigmaThreshold(nsigmathresh);
                    }
		    Int thisniter = loopcontrols.getCycleNiter() <5000 ? loopcontrols.getCycleNiter() : 2000;

		    loopcontrols.setPeakResidual( peakresidual );

                     //where we write in model and residual may be
                    {
                      //no need to lock here as only arrays are used

                      //LatticeLocker lock1 (*(itsImages->residual()), FileLocker::Write);
                      //LatticeLocker lock2 (*(itsImages->model()), FileLocker::Write);
		    takeOneStep( loopcontrols.getLoopGain(), 
				 //				 loopcontrols.getCycleNiter(),
				 thisniter,
				 //loopcontrols.getCycleThreshold(),
				 thresholdtouse,
				 peakresidual, 
				 modelflux,
				 iterdone);
                    }

		    os << LogIO::NORMAL1  << "SDAlgoBase: After one step, dec : " << deconvolverid << "    residual=" << peakresidual << " model=" << modelflux << " iters=" << iterdone << LogIO::POST; 

		    SynthesisUtilMethods::getResource("In Deconvolver : one step" );
		    
		    loopcontrols.incrementMinorCycleCount( iterdone ); // CAS-8767 : add subimageindex and merge with addSummaryMinor call later.
		    
		    stopCode = checkStop( loopcontrols,  peakresidual );

		    /// Catch the situation where takeOneStep returns without satisfying any
		    ///  convergence criterion. For now, takeOneStep is the entire minor cycle.
		    /// Later, when you can interrupt minor cycles, takeOneStep will become more
		    /// fine grained, and then stopCode=0 will be valid.  For now though, check on
                    /// it and handle it (for CAS-7898).
		    if(stopCode==0 && iterdone != thisniter)
		      {
			os << LogIO::NORMAL1 << "Exited " << itsAlgorithmName << " minor cycle without satisfying stopping criteria " << LogIO::POST;
			stopCode=5;
		      }
		    
		  }// end of minor cycle iterations for this subimage.

                //where we write in model and residual may be
                {

                  LatticeLocker lock1 (*(itsImages->residual()), FileLocker::Write);
                  LatticeLocker lock2 (*(itsImages->model()), FileLocker::Write);
                  finalizeDeconvolver();
                }

                // get the new peakres without a mask for the summary minor
                peakresidualnomask = itsImages->getPeakResidual();

	      }// if validmask
	    
	    // same as checking on itscycleniter.....
	    loopcontrols.setUpdatedModelFlag( loopcontrols.getIterDone()-startiteration );
	    
	    os << "[" << imagestore->getName();
	    if(nSubChans>1) os << ":C" << chanid ;
	    if(nSubPols>1) os << ":P" << polid ;
	    Int iterend = loopcontrols.getIterDone();
	    os << "]"
	      //	       <<" iters=" << ( (iterend>startiteration) ? startiteration+1 : startiteration )<< "->" << iterend
	       <<" iters=" << startiteration << "->" << iterend << " [" << iterend-startiteration << "]"
	       << ", model=" << startmodelflux << "->" << modelflux
	       << ", peakres=" << startpeakresidual << "->" << peakresidual ;

	    switch (stopCode)
	      {
	      case 0:
		os << ", Skipped this plane. Zero mask.";
		break;
	      case 1: 
		os << ", Reached cycleniter.";
		break;
	      case 2:
		os << ", Reached cyclethreshold.";
		break;
	      case 3:
		os << ", Zero iterations performed.";
		break;
	      case 4:
		os << ", Possible divergence. Peak residual increased by 10% from minimum.";
		break;
	      case 5:
		os << ", Exited " << itsAlgorithmName << " minor cycle without reaching any stopping criterion.";
		break;
              case 6:
                os << ", Reached n-sigma threshold.";
	      default:
		break;
	      }

	       os << LogIO::POST;
	       
	    Int rank(0);
	    String rankStr = EnvironmentVariable::get("OMPI_COMM_WORLD_RANK");
	    if (!rankStr.empty()) {
	      rank = stoi(rankStr);
	    }
	    stopProfiling.stop();
	    profileMemThread.join();
	    std::chrono::steady_clock::time_point profileFinishTime = std::chrono::steady_clock::now();
	    Float runtime = (  (Float) std::chrono::duration_cast<std::chrono::milliseconds>(profileFinishTime - profileStartTime).count()  )/1000.0;
	    Float fpeakMem = (Float) peakMem / 1000000.0; // to MB
	    int chunkId = chanid; // temporary CAS-13683 workaround
	    if (SIMinorCycleController::useSmallSummaryminor()) { // temporary CAS-13683 workaround
	        chunkId = chanid + nSubChans*polid;
	    }
	    loopcontrols.addSummaryMinor( deconvolverid, chunkId, polid, cycleStartIteration,
	                                  startiteration, startmodelflux, startpeakresidual, startpeakresidualnomask,
	                                  modelflux, peakresidual, peakresidualnomask, masksum, rank, fpeakMem, runtime, stopCode);

	    loopcontrols.resetCycleIter(); 

	    if( peakresidual > maxResidualAcrossPlanes && stopCode!=0 )
	      {maxResidualAcrossPlanes=peakresidual; maxResChan=chanid; maxResPol=polid;}

	    totalFluxAcrossPlanes += modelflux;
	    
	  }// end of polid loop
	
      }// end of chanid loop
    
    loopcontrols.setPeakResidual( maxResidualAcrossPlanes );

    /// Print total flux over all planes (and max res over all planes). IFF there are more than one plane !!
    if( nSubChans>1 || nSubPols>1 )
      {
	os << "[" << imagestore->getName() << "] ";
	os << "Total model flux (over all planes) : " << totalFluxAcrossPlanes; //<< LogIO::POST;
	os << "     Peak Residual (over all planes) : " << maxResidualAcrossPlanes << " in C"<<maxResChan << ":P"<<maxResPol << LogIO::POST;
      }

  }// end of deconvolve

  void SDAlgorithmBase::profileMinorCycle(ContextBoundBool &stop, uLong &peakMem)
  {
    LogIO os( LogOrigin("SDAlgorithmBase",__FUNCTION__,WHERE) );
    while (true)
    {
      for (int i = 0; i < 10; i++) // check for stop 10 times a second
      {
      	bool doStop = stop.getVal();
        if (i == 1 || doStop) { // check for peak memory usage 1 times a second
          uLong currentMem = SynthesisUtilMethods::getAllocatedMemoryInBytes();
          if (currentMem > peakMem) { peakMem = currentMem; }
        }
        if (doStop) {
          return;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
    }
  }
  
  Int SDAlgorithmBase::checkStop( SIMinorCycleController &loopcontrols, 
				   Float currentresidual )
  {
    return loopcontrols.majorCycleRequired(currentresidual);
  }

  Long SDAlgorithmBase::estimateRAM(const vector<int>& imsize){
    Long mem=0;
    if(itsImages)
    //Simple deconvolvers will have psf + residual + model + mask (1 plane at a time)
      mem=sizeof(Float)*(itsImages->getShape()(0))*(itsImages->getShape()(1))*4/1024;
    else if(imsize.size() >1)
      mem=sizeof(Float)*(imsize[0])*(imsize[1])*4/1024;
    else
      return 0;
      //throw(AipsError("Deconvolver cannot estimate the memory usage at this point"));
    return mem;
  }

  void SDAlgorithmBase::setRestoringBeam( GaussianBeam restbeam, String usebeam )
  {
    LogIO os( LogOrigin("SDAlgorithmBase","setRestoringBeam",WHERE) );
    itsRestoringBeam = restbeam;
    itsUseBeam = usebeam;
  }

  /*
  void SDAlgorithmBase::setMaskOptions( String maskstring )
  {
    itsMaskString = maskstring;
  }
  */
  /*
  void SDAlgorithmBase::loadMask()
  { 
    if (! itsIsMaskLoaded ) {
	if( itsMaskString.length()==0 )   {
	  itsMaskHandler.resetMask( itsImages );
	}
	else {
	  itsMaskHandler.fillMask( itsImages->mask() , itsMaskString );
	}
	itsIsMaskLoaded=true;
      }
  }
  */

   void SDAlgorithmBase::restore(std::shared_ptr<SIImageStore> imagestore )
  {

    LogIO os( LogOrigin("SDAlgorithmBase","restore",WHERE) );

    os << "[" << imagestore->getName() << "] : Restoring model image." << LogIO::POST;

    if( imagestore->hasResidualImage() )  imagestore->restore( itsRestoringBeam, itsUseBeam );
    else{os << "Cannot restore with a residual image" << LogIO::POST;}
      

   
  }

 
  void SDAlgorithmBase::pbcor(std::shared_ptr<SIImageStore> imagestore )
  {

    LogIO os( LogOrigin("SDAlgorithmBase","pbcor",WHERE) );

    os << "[" << imagestore->getName() << "] : Applying PB correction" << LogIO::POST;

    imagestore->pbcor();

  }
  
  /*  
  void SDAlgorithmBase::restorePlane()
  {

    LogIO os( LogOrigin("SDAlgorithmBase","restorePlane",WHERE) );
    //     << ". Optionally, PB-correct too." << LogIO::POST;

    try
      {
	// Fit a Gaussian to the PSF.
	GaussianBeam beam = itsImages->getPSFGaussian();

	os << "Restore with beam : " << beam.getMajor(Unit("arcmin")) << " arcmin, " << beam.getMinor(Unit("arcmin"))<< " arcmin, " << beam.getPA(Unit("deg")) << " deg)" << LogIO::POST; 
	
	// Initialize restored image
	itsImage.set(0.0);
	// Copy model into it
	itsImage.copyData( LatticeExpr<Float>(itsModel )  );
	// Smooth model by beam
	StokesImageUtil::Convolve( itsImage, beam);
	// Add residual image
	itsImage.copyData( LatticeExpr<Float>( itsImage + itsResidual ) );
      }
    catch(AipsError &x)
      {
	throw( AipsError("Restoration Error " + x.getMesg() ) );
      }

  }
  */

  // Use this decide how to partition
  // the image for separate calls to 'deconvolve'.
  // Give codes to signal one or more of the following.
    ///    - channel planes separate
    ///    - stokes planes separate
    ///    - partitioned-image clean (facets ?)
  // Later, can add more complex partitioning schemes.... 
  // but there will be one place to do it, per deconvolver.
  void SDAlgorithmBase::queryDesiredShape(Int &nchanchunks, Int &npolchunks, IPosition imshape) // , nImageFacets.
  {  
    AlwaysAssert( imshape.nelements()==4, AipsError );
    nchanchunks=imshape(3);  // Each channel should appear separately.
    npolchunks=imshape(2); // Each pol should appear separately.
  }

  /*
  /// Make a list of Slices, to send sequentially to the deconvolver.
  /// Loop over this list of reference subimages in the 'deconvolve' call.
  /// This will support...
  ///    - channel cube clean
  ///    - stokes cube clean
  ///    - partitioned-image clean (facets ?)
  ///    - 3D deconvolver
  void SDAlgorithmBase::partitionImages( std::shared_ptr<SIImageStore> &imagestore )
  {
    LogIO os( LogOrigin("SDAlgorithmBase","partitionImages",WHERE) );

    IPosition imshape = imagestore->getShape();


    // TODO : Check which axes is which first !!!
    ///// chanAxis_p=CoordinateUtil::findSpectralAxis(dirty_p->coordinates());
    //// Vector<Stokes::StokesTypes> whichPols;
    //// polAxis_p=CoordinateUtil::findStokesAxis(whichPols, dirty_p->coordinates());
    uInt nx = imshape[0];
    uInt ny = imshape[1];
    uInt npol = imshape[2];
    uInt nchan = imshape[3];

    /// (1) /// Set up the Deconvolver Slicers.

    // Ask the deconvolver what shape it wants.
    Bool onechan=false, onepol=false;
    queryDesiredShape(onechan, onepol);

    uInt nSubImages = ( (onechan)?imshape[3]:1 ) * ( (onepol)?imshape[2]:1 ) ;
    uInt polstep = (onepol)?1:npol;
    uInt chanstep = (onechan)?1:nchan;

    itsDecSlices.resize( nSubImages );

    uInt subindex=0;
    for(uInt pol=0; pol<npol; pol+=polstep)
      {
	for(uInt chan=0; chan<nchan; chan+=chanstep)
	  {
	    AlwaysAssert( subindex < nSubImages , AipsError );
	    IPosition substart(4,0,0,pol,chan);
	    IPosition substop(4,nx-1,ny-1, pol+polstep-1, chan+chanstep-1);
	    itsDecSlices[subindex] = Slicer(substart, substop, Slicer::endIsLast);
	    subindex++;
	  }
      }

   }// end of partitionImages
  */

  /*
  void SDAlgorithmBase::initializeSubImages( std::shared_ptr<SIImageStore> &imagestore, uInt subim)
  {
    itsResidual = SubImage<Float>( *(imagestore->residual()), itsDecSlices[subim], true );
    itsPsf = SubImage<Float>( *(imagestore->psf()), itsDecSlices[subim], true );
    itsModel = SubImage<Float>( *(imagestore->model()), itsDecSlices[subim], true );

    itsImages = imagestore;

  }
  */

  /////////// Helper Functions for all deconvolvers to use if they need it.

  Bool SDAlgorithmBase::findMaxAbs(const Array<Float>& lattice,
					  Float& maxAbs,
					  IPosition& posMaxAbs)
  {
    //    cout << "findmax : lat shape : " << lattice.shape() << " posmax : " << posMaxAbs << endl;
    posMaxAbs = IPosition(lattice.shape().nelements(), 0);
    maxAbs=0.0;
    Float minVal;
    IPosition posmin(lattice.shape().nelements(), 0);
    minMax(minVal, maxAbs, posmin, posMaxAbs, lattice);
    //cout << "min " << minVal << "  " << maxAbs << "   " << max(lattice) << endl;
    if(abs(minVal) > abs(maxAbs)){
      maxAbs=abs(minVal);
      posMaxAbs=posmin;
    }
    return true;
  }

  Bool SDAlgorithmBase::findMaxAbsMask(const Array<Float>& lattice,
				       const Array<Float>& mask,
					  Float& maxAbs,
					  IPosition& posMaxAbs)
  {

    //cout << "maxabsmask shapes : " << lattice.shape() << " " << mask.shape() << endl;

    posMaxAbs = IPosition(lattice.shape().nelements(), 0);
    maxAbs=0.0;
    Float minVal;
    IPosition posmin(lattice.shape().nelements(), 0);
    minMaxMasked(minVal, maxAbs, posmin, posMaxAbs, lattice,mask);
    //cout << "min " << minVal << "  " << maxAbs << "   " << max(lattice) << endl;
    if(abs(minVal) > abs(maxAbs)){
      maxAbs=abs(minVal);
      posMaxAbs=posmin;
    }
    return true;
  }

  /*
  Bool SDAlgorithmBase::findMinMaxMask(const Array<Float>& lattice,
				       const Array<Float>& mask,
				       Float& minVal, Float& maxVal)
  //				       IPosition& minPos, IPosition& maxPos)
  {
    //posMaxAbs = IPosition(lattice.shape().nelements(), 0);
    //maxAbs=0.0;
    IPosition posmin(lattice.shape().nelements(), 0);
    IPosition posmax(lattice.shape().nelements(), 0);
    minMaxMasked(minVal, maxVal, posmin, posmax, lattice,mask);

    return true;
  }
  */
  /*

  GaussianBeam SDAlgorithmBase::getPSFGaussian()
  {

    GaussianBeam beam;
    try
      {
	if( itsPsf.ndim() > 0 )
	  {
	    StokesImageUtil::FitGaussianPSF( itsPsf, beam );
	  }
      }
    catch(AipsError &x)
      {
	LogIO os( LogOrigin("SDAlgorithmBase","getPSFGaussian",WHERE) );
	os << "Error in fitting a Gaussian to the PSF : " << x.getMesg() << LogIO::POST;
	throw( AipsError("Error in fitting a Gaussian to the PSF" + x.getMesg()) );
      }

    return beam;
  }

*/ 

  ContextBoundBool::ContextBoundBool(bool startValue)
  {
    itsVal = startValue;
    itsInitialValue = startValue;
  }

  ContextBoundBool::~ContextBoundBool()
  {
    // This destructor is the entire reason this class exists.
    // We need to guarantee that stop will be called when the encapsulating
    // context around this object is closed (such as with a return or throw statement).
    stop();
  }

  bool ContextBoundBool::getVal()
  { // mutex context guard
    std::lock_guard<std::mutex> lock(itsMutex);
    return itsVal;
  }

  void ContextBoundBool::stop()
  { // mutex context guard
    std::lock_guard<std::mutex> lock(itsMutex);
    itsVal = !itsInitialValue;
  }
 
} //# NAMESPACE CASA - END

