//# SDAlgorithmMSMFS.cc: Implementation of SDAlgorithmMSMFS classes
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
#include <synthesis/ImagerObjects/SDAlgorithmMSMFS.h>
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

#include <sstream>

#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Logging/LogSink.h>

#include <casacore/casa/System/Choice.h>
#include <msvis/MSVis/StokesVector.h>


using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN


  SDAlgorithmMSMFS::SDAlgorithmMSMFS( uInt nTaylorTerms, Vector<Float> scalesizes, Float smallscalebias):
    SDAlgorithmBase(),
    //    itsImages(),
    itsMatPsfs(), itsMatResiduals(), itsMatModels(),
    itsNTerms(nTaylorTerms),
    itsScaleSizes(scalesizes),
    itsSmallScaleBias(smallscalebias),
    itsMTCleaner(),
    itsMTCsetup(false)
  {
    itsAlgorithmName=String("mtmfs");
    if( itsScaleSizes.nelements()==0 ){ itsScaleSizes.resize(1); itsScaleSizes[0]=0.0; }
  }

  SDAlgorithmMSMFS::~SDAlgorithmMSMFS()
  {

  }


  //  void SDAlgorithmMSMFS::initializeDeconvolver( Float &peakresidual, Float &modelflux )
  void SDAlgorithmMSMFS::initializeDeconvolver()
  {
    LogIO os( LogOrigin("SDAlgorithmMSMFS","initializeDeconvolver",WHERE) );

    AlwaysAssert( (bool) itsImages, AipsError );
    AlwaysAssert( itsNTerms == itsImages->getNTaylorTerms() , AipsError );

    itsMatPsfs.resize( 2*itsNTerms-1 );
    itsMatResiduals.resize( itsNTerms );
    itsMatModels.resize( itsNTerms );

    ////  Why is this needed ?  I hope this is by reference.
    for(uInt tix=0; tix<2*itsNTerms-1; tix++)
    {
    	if(tix<itsNTerms)
    	{
    	  (itsImages->residual(tix))->get( itsMatResiduals[tix], true );
    	  (itsImages->model(tix))->get( itsMatModels[tix], true );
    	}
    	(itsImages->psf(tix))->get( itsMatPsfs[tix], true );
    }

    itsImages->mask()->get( itsMatMask, true );

    //// Initialize the MultiTermMatrixCleaner.

    ///  ----------- do once ----------
    if( itsMTCsetup == false)
    {
    	//cout << "Setting up the MT Cleaner once" << endl;
    	//Vector<Float> scalesizes(1); scalesizes[0]=0.0;
    	itsMTCleaner.setscales( itsScaleSizes );

    	if(itsSmallScaleBias > 1)
    	{
    	  os << LogIO::WARN << "Acceptable smallscalebias values are [-1,1].Changing smallscalebias from " << itsSmallScaleBias <<" to 1." << LogIO::POST;
    	  itsSmallScaleBias = 1;
    	}

    	if(itsSmallScaleBias < -1)
    	{
    	  os << LogIO::WARN << "Acceptable smallscalebias values are [-1,1].Changing smallscalebias from " << itsSmallScaleBias <<" to -1." << LogIO::POST;
    	  itsSmallScaleBias = -1;
    	}


    	itsMTCleaner.setSmallScaleBias(itsSmallScaleBias);
    	itsMTCleaner.setntaylorterms( itsNTerms );
    	itsMTCleaner.initialise( itsImages->getShape()[0], itsImages->getShape()[1] );

    	for(uInt tix=0; tix<2*itsNTerms-1; tix++)
    	{
        Matrix<Float> tempMat;
        tempMat.reference( itsMatPsfs[tix] );
        itsMTCleaner.setpsf( tix, tempMat );
        ///	itsMTCleaner.setpsf( tix, itsMatPsfs[tix] );
    	}
    	itsMTCsetup=true;
    }
    /// -----------------------------------------

    /*
    /// Find initial max vals..
    findMaxAbsMask( itsMatResiduals[0], itsMatMask, itsPeakResidual, itsMaxPos );
    itsModelFlux = sum( itsMatModels[0] );

    peakresidual = itsPeakResidual;
    modelflux = itsModelFlux;
    */

    // Parts to be repeated at each minor cycle start....

    Matrix<Float> tempmask(itsMatMask);
    itsMTCleaner.setmask( tempmask );

    for(uInt tix=0; tix<itsNTerms; tix++)
    {
    	Matrix<Float> tempMat;
    	tempMat.reference( itsMatResiduals[tix] );
    	itsMTCleaner.setresidual( tix, tempMat );
    	//	itsMTCleaner.setresidual( tix, itsMatResiduals[tix] );

    	Matrix<Float> tempMat2;
    	tempMat2.reference( itsMatModels[tix] );
    	itsMTCleaner.setmodel( tix, tempMat2 );
    	//	itsMTCleaner.setmodel( tix, itsMatModels[tix] );
    }

    // Print some useful info about the frequency setup for this MTMFS run
    itsImages->calcFractionalBandwidth();
  }

  Long SDAlgorithmMSMFS::estimateRAM(const vector<int>& imsize){
    ///taken from imager_resource_predictor.py

    Long mem=0;
    IPosition shp;
    if(itsImages){
      shp=itsImages->getShape();
    }
    else if(imsize.size() >1){
      shp=IPosition(imsize);
    }
    else
      return 0;
      //throw(AipsError("Deconvolver cannot estimate the memory usage at this point"));

      // psf patches in the Hessian
    Long nscales=itsScaleSizes.nelements();
    Long n4d = (nscales * (nscales+1) / 2.0) * (itsNTerms * (itsNTerms+1) / 2);

      Long nsupport = Long(Float(100.0/Float(shp(0)))* Float(100.0/Float(shp(1))* Float(n4d + nscales)));

      Long nfull = 2 + 2 + 3 * nscales + 3 * itsNTerms + (2 * itsNTerms - 1) + 2 * itsNTerms * nscales;
      Long nfftserver = 1 + 2*2 ; /// 1 float and 2 complex

      Long mystery = 1 + 1;  /// TODO

      Long ntotal = nsupport + nfull + nfftserver + mystery;
      mem=sizeof(Float)*(shp(0))*(shp(1))*ntotal/1024;

    return mem;
  }

  void SDAlgorithmMSMFS::takeOneStep( Float loopgain, Int cycleNiter, Float cycleThreshold, Float &peakresidual, Float &modelflux, Int &iterdone)
  {

    iterdone = itsMTCleaner.mtclean( cycleNiter, 0.0, loopgain, cycleThreshold );

    if( iterdone==-2 ) throw(AipsError("MT-Cleaner error : Non-invertible Hessian. Please check if the multi-frequency data selection is appropriate for a polynomial fit of the desired order."));

    for(uInt tix=0; tix<itsNTerms; tix++)
    {
    	Matrix<Float> tempMat;
    	tempMat.reference( itsMatModels[tix] );

    	itsMTCleaner.getmodel( tix, tempMat ); //itsMatModels[tix] );
    }

    /////////////////
    //findMaxAbs( itsMatResiduals[0], itsPeakResidual, itsMaxPos );
    //peakresidual = itsPeakResidual;

    peakresidual = itsMTCleaner.getpeakresidual();

    modelflux = sum( itsMatModels[0] ); // Performance hog ?

    // Retrieve residual to be saved to the .residual file in finalizeDeconvolver
    for(uInt tix=0; tix<itsNTerms; tix++)
    {
      casacore::Matrix<Float> tmp;
      itsMTCleaner.getresidual(tix, tmp); // possible room for optimization here -> get residual without extra tmp copy? maybe change getResidual to accept an array?
      itsMatResiduals[tix] = tmp;
    }
  }	    

  void SDAlgorithmMSMFS::finalizeDeconvolver()
  {
    //    itsResidual.put( itsMatResidual );
    //    itsModel.put( itsMatModel );

    // Why is this needed ?  If the matrices are by reference, then why do we need this ?
    for(uInt tix=0; tix<itsNTerms; tix++)
    {
    	(itsImages->residual(tix))->put( itsMatResiduals[tix] );
    	(itsImages->model(tix))->put( itsMatModels[tix] );
    }
  }

  void SDAlgorithmMSMFS::restore(std::shared_ptr<SIImageStore> imagestore )
  {

    LogIO os( LogOrigin("SDAlgorithmMSMFS","restore",WHERE) );

    if( ! imagestore->hasResidualImage() ) return;

    // Compute principal solution ( if it hasn't already been done to this ImageStore......  )
    //////  Put some image misc info in here, to say if it has been done or not.

    // Loop over polarization planes here, as MTC knows only about Matrices.
    Int nSubChans, nSubPols;
    queryDesiredShape(nSubChans, nSubPols, imagestore->getShape());
    for( Int chanid=0; chanid<nSubChans;chanid++) // redundant since only 1 chan
    {

   	for( Int polid=0; polid<nSubPols;polid++) // one pol plane at a time
      {
	    itsImages = imagestore->getSubImageStore( 0, 1, chanid, nSubChans, polid, nSubPols );

	    //cout << "Units for chan " << chanid << " and pol " << polid << " are " << itsImages->image()->units().getName() << endl;

    ///  ----------- do once if trying to 'only restore' without model ----------
    if( itsMTCsetup == false)
      {

	itsMatPsfs.resize( 2*itsNTerms-1 );
	for(uInt tix=0; tix<2*itsNTerms-1; tix++)
	  {
	    (itsImages->psf(tix))->get( itsMatPsfs[tix], True );
	  }

	//cout << "Setting up the MT Cleaner once" << endl;
	//Vector<Float> scalesizes(1); scalesizes[0]=0.0;
	itsMTCleaner.setscales( itsScaleSizes );
	itsMTCleaner.setntaylorterms( itsNTerms );
	itsMTCleaner.initialise( itsImages->getShape()[0], itsImages->getShape()[1] );

	for(uInt tix=0; tix<2*itsNTerms-1; tix++)
	  {
	    Matrix<Float> tempMat;
	    tempMat.reference( itsMatPsfs[tix] );
	    itsMTCleaner.setpsf( tix, tempMat );
	    ///	itsMTCleaner.setpsf( tix, itsMatPsfs[tix] );
	  }
	itsMTCsetup=true;
      }
    /// -----------------------------------------


    Vector<TempImage<Float> > tempResOrig(itsNTerms);

    // Set residual images into mtcleaner
    for(uInt tix=0; tix<itsNTerms; tix++)
      {
	Array<Float> tempArr;
	(itsImages->residual(tix))->get( tempArr, True );
	Matrix<Float> tempMat;
	tempMat.reference( tempArr );
	itsMTCleaner.setresidual( tix, tempMat );

	// Also save them temporarily (copies)
	tempResOrig[tix] = TempImage<Float>(itsImages->getShape(), itsImages->residual(tix)->coordinates());
	tempResOrig[tix].copyData( LatticeExpr<Float>(* ( itsImages->residual(tix) ) ) );
      }

    // Modify the original in place
    itsMTCleaner.computeprincipalsolution();

    for(uInt tix=0; tix<itsNTerms; tix++)
      {
	Matrix<Float> tempRes;
	itsMTCleaner.getresidual(tix,tempRes);
	(itsImages->residual(tix))->put( tempRes );
      }

    // Calculate restored image and alpha using modified residuals
    SDAlgorithmBase::restore( itsImages );

    // Put back original unmodified residuals.o
    for(uInt tix=0; tix<itsNTerms; tix++)
      {
	(itsImages->residual(tix))->copyData( LatticeExpr<Float>( tempResOrig(tix) ) );
      }

      } // for polid loop
    }// for chanid loop

    // This log message is important. This call of imagestore->image(...) is the first call if there is
    // a multi-channel or multi-pol image. This is what will set the units correctly. Ref. CAS-13153
    os << LogIO::POST << "Restored images : ";
    for(uInt tix=0; tix<itsNTerms; tix++)
      {
	os << LogIO::POST << imagestore->image(tix)->name() << "  (model=" << imagestore->model(tix)->name() << ") " ;
      }
    os << LogIO::POST << endl;

    //cout << "Units for  " << imagestore->image()->name() << "  aaaaaare " << imagestore->image()->units().getName() << endl;
  }// ::restore


  /*
  void SDAlgorithmMSMFS::restorePlane()
  {

    LogIO os( LogOrigin("SDAlgorithmMSMFS","restorePlane",WHERE) );

    try
      {
	// Fit a Gaussian to the PSF.
	GaussianBeam beam = itsImages->getPSFGaussian();

	os << "Restore with beam : "
	   << beam.getMajor(Unit("arcmin")) << " arcmin, "
	   << beam.getMinor(Unit("arcmin"))<< " arcmin, "
	   << beam.getPA(Unit("deg")) << " deg" << LogIO::POST;

	// Compute principal solution ( if it hasn't already been done to this ImageStore......  )
	itsMTCleaner.computeprincipalsolution();
	for(uInt tix=0; tix<itsNTerms; tix++)
	  {
	    Matrix<Float> tempRes;
	    itsMTCleaner.getresidual(tix,tempRes);
	    (itsImages->residual(tix))->put( tempRes );
	  }

	// Restore all terms
	ImageInfo ii = itsImages->image(0)->imageInfo();
	ii.setRestoringBeam( beam );

	for(uInt tix=0; tix<itsNTerms; tix++)
	  {
	    (itsImages->image(tix))->set(0.0);
	    (itsImages->image(tix))->copyData( LatticeExpr<Float>(*(itsImages->model(tix))) );
	    StokesImageUtil::Convolve( *(itsImages->image(tix)) , beam);
	    (itsImages->image(tix))->copyData( LatticeExpr<Float>
					       ( *(itsImages->model(tix)) + *(itsImages->residual(tix)) )   );
	    itsImages->image()->setImageInfo(ii);
	  }

	// Calculate alpha and beta
	LatticeExprNode leMaxRes = max( *( itsImages->residual(0) ) );
	Float maxres = leMaxRes.getFloat();
	Float specthreshold = maxres/5.0;  //////////// do something better here.....

      os << "Calculating spectral parameters for  Intensity > peakresidual/5 = " << specthreshold << " Jy/beam" << LogIO::POST;
      LatticeExpr<Float> mask1(iif(((*(itsImages->image(0))))>(specthreshold),1.0,0.0));
      LatticeExpr<Float> mask0(iif(((*(itsImages->image(0))))>(specthreshold),0.0,1.0));

      /////// Calculate alpha
      LatticeExpr<Float> alphacalc( (((*(itsImages->image(1))))*mask1)/(((*(itsImages->image(0))))+(mask0)) );
      itsImages->alpha()->copyData(alphacalc);

      // Set the restoring beam for alpha
      itsImages->alpha()->setImageInfo(ii);
      //      itsImages->alpha()->table().unmarkForDelete();

      // Make a mask for the alpha image
      LatticeExpr<Bool> lemask(iif(((*(itsImages->image(0))) > specthreshold) , true, false));

      createMask(lemask, *(itsImages->alpha()));

      }
    catch(AipsError &x)
      {
	throw( AipsError("Restoration Error : " + x.getMesg() ) );
      }

  }


Bool SDAlgorithmMSMFS::createMask(LatticeExpr<Bool> &lemask, ImageInterface<Float> &outimage)
{
      ImageRegion outreg = outimage.makeMask("mask0",false,true);
      LCRegion& outmask=outreg.asMask();
      outmask.copyData(lemask);
      outimage.defineRegion("mask0",outreg, RegionHandler::Masks, true);
      outimage.setDefaultMask("mask0");
      return true;
}

*/

} //# NAMESPACE CASA - END

