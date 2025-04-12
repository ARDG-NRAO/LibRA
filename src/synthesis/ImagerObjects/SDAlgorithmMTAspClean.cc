//# SDAlgorithmMTAspClean.cc: Implementation of SDAlgorithmMTAspClean classes
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

/*#include <casa/Arrays/ArrayMath.h>
#include <casa/OS/HostInfo.h>
#include <synthesis/ImagerObjects/SDAlgorithmMTAspClean.h>

#include <components/ComponentModels/SkyComponent.h>
#include <components/ComponentModels/ComponentList.h>
#include <images/Images/TempImage.h>
#include <images/Images/SubImage.h>
#include <images/Regions/ImageRegion.h>
#include <casa/OS/File.h>
#include <lattices/LEL/LatticeExpr.h>
#include <lattices/Lattices/TiledLineStepper.h>
#include <lattices/Lattices/LatticeStepper.h>
#include <lattices/Lattices/LatticeIterator.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <coordinates/Coordinates/StokesCoordinate.h>
#include <casa/Exceptions/Error.h>
#include <casa/BasicSL/String.h>
#include <casa/Utilities/Assert.h>
#include <casa/OS/Directory.h>
#include <tables/Tables/TableLock.h>

#include<synthesis/ImagerObjects/SIMinorCycleController.h>

#include <casa/sstream.h>

#include <casa/Logging/LogMessage.h>
#include <casa/Logging/LogIO.h>
#include <casa/Logging/LogSink.h>

#include <casa/System/Choice.h>
#include <msvis/MSVis/StokesVector.h>*/

#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/OS/HostInfo.h>
#include <synthesis/ImagerObjects/SDAlgorithmMTAspClean.h>

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

#include <synthesis/ImagerObjects/SIMinorCycleController.h>

#include <casacore/casa/sstream.h>

#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Logging/LogSink.h>

#include <casacore/casa/System/Choice.h>
#include <msvis/MSVis/StokesVector.h>


using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

  SDAlgorithmMTAspClean::SDAlgorithmMTAspClean(uInt nTaylorTerms, Float fusedThreshold, bool isSingle, Int largestScale, Int stoppointmode):
    SDAlgorithmBase(),
    itsMatPsfs(), itsMatResiduals(), itsMatModels(),
    itsNTerms(nTaylorTerms),
    itsStopPointMode(stoppointmode),
    itsCleaner(),
    itsFusedThreshold(fusedThreshold),
    itsMCsetup(true),
    itsUserLargestScale(largestScale),
    itsPrevPsfWidth(0),
    itsIsSingle(isSingle)
  {
    itsAlgorithmName = String("asp");
  }

  SDAlgorithmMTAspClean::~SDAlgorithmMTAspClean()
  {

  }

  void SDAlgorithmMTAspClean::initializeDeconvolver()
  {
    LogIO os(LogOrigin("SDAlgorithmMTAspClean", "initializeDeconvolver", WHERE));
    AlwaysAssert((bool)itsImages, AipsError);
    //cout << "itsNTerms " << itsNTerms << " itsImages->getNTaylorTerms() " << itsImages->getNTaylorTerms() << " type " << itsImages->getType() << endl;
    AlwaysAssert(itsNTerms == itsImages->getNTaylorTerms() , AipsError);

    itsMatPsfs.resize(2*itsNTerms - 1);
    itsMatResiduals.resize(itsNTerms);
    itsMatModels.resize(itsNTerms);

    for(uInt tix = 0; tix < 2*itsNTerms-1; tix++)
    {
      if(tix < itsNTerms)
      {
        (itsImages->residual(tix))->get( itsMatResiduals[tix], true );
        (itsImages->model(tix))->get( itsMatModels[tix], true );
      }

      (itsImages->psf(tix))->get( itsMatPsfs[tix], true );
    }
    itsImages->mask()->get( itsMatMask, true );

    // Initialize the AspMatrixCleaner.
    // If it's single channel, this only needs to be computed once.
    // Otherwise, it needs to be called repeatedly at each minor cycle start to
    // get psf for each channel
    if(itsMCsetup)
    {
      itsCleaner.setNTaylorTerms(itsNTerms);
      //if (itsNTerms > 1) // wide-band
      itsCleaner.allocateMemory(itsImages->getShape()[0], itsImages->getShape()[1]);

      for(uInt tix=0; tix < 2*itsNTerms-1; tix++)
      {
        Matrix<Float> tempM;
        tempM.reference(itsMatPsfs[tix]);
        //if (itsNTerms > 1) // wide-band
        itsCleaner.setMTPsf(tix, tempM);
      }

      // Initial scales are unchanged and only need to be
      // computed when psf width is updated
      // use the 1st psf to define the initial scales(?)
      const Float width = itsCleaner.getPsfGaussianWidth(*(itsImages->psf(0)));
      //if user does not provide the largest scale, we calculate it internally.
      itsCleaner.setUserLargestScale(itsUserLargestScale);
      // we do not use the shortest baseline approach below b/c of reasons in CAS-940 dated around Jan 2022
      //itsCleaner.getLargestScaleSize(*(itsImages->psf(0)));

      if (itsPrevPsfWidth != width)
      {
        itsPrevPsfWidth = width;
        itsCleaner.setInitScaleXfrs(width);
      }

      itsCleaner.stopPointMode( itsStopPointMode );
      itsCleaner.ignoreCenterBox( true ); // Clean full image

      // If it's single channel, we do not do the expensive set up repeatedly
      if (itsIsSingle)
        itsMCsetup = false;

      itsCleaner.setFusedThreshold(itsFusedThreshold);
    }

    // Parts to be repeated at each minor cycle start....
    itsCleaner.setaspcontrol(0, 0, 0, Quantity(0.0, "%"));/// Needs to come before the rest
    itsCleaner.setInitScaleMasks(itsMatMask);

    for(uInt tix=0; tix<itsNTerms; tix++)
    {
      Matrix<Float> tempMat;
      tempMat.reference(itsMatResiduals[tix]);
      //if (itsNTerms > 1)
      //{ // wide-band
      itsCleaner.setResidual(tix, tempMat);
      Matrix<Float> tempMat2;
      tempMat2.reference(itsMatModels[tix]);
      itsCleaner.setModel(tix, tempMat2);
      //}
    }

    // InitScaleXfrs and InitScaleMasks should already be set
    itsScaleSizes.clear();
    itsScaleSizes = itsCleaner.getActiveSetAspen();
    itsScaleSizes.push_back(0.0); // put 0 scale
    itsCleaner.defineAspScales(itsScaleSizes);
  }


  void SDAlgorithmMTAspClean::takeOneStep( Float loopgain,
					  Int cycleNiter,
					  Float cycleThreshold,
					  Float &peakresidual,
					  Float &modelflux,
					  Int &iterdone)
  {
    LogIO os( LogOrigin("SDAlgorithmMTAspClean","takeOneStep", WHERE) );

    Quantity thresh(cycleThreshold, "Jy");
    itsCleaner.setaspcontrol(cycleNiter, loopgain, thresh, Quantity(0.0, "%"));

    // retval
    //  1 = converged
    //  0 = not converged but behaving normally
    // -1 = Non-invertible Hessian error for wide-band
    // -2 = not converged and either large scale hit negative or diverging
    itsCleaner.startingIteration( 0 );
    //if (itsNTerms > 1) // wide-band
    //{
      Int retval = itsCleaner.mtaspclean();
      iterdone = itsCleaner.numberIterations();

      if( retval==-1 ) throw(AipsError("MTAspClean error : Non-invertible Hessian. Please check if the multi-frequency data selection is appropriate for a polynomial fit of the desired order."));
      if( retval==-2 ) {os << LogIO::WARN << "MTAspClean minor cycle stopped at large scale negative or diverging" << LogIO::POST;}

      for(uInt tix=0; tix<itsNTerms; tix++)
      {
        Matrix<Float> tempMat;
        tempMat.reference(itsMatModels[tix]);

        itsCleaner.getModel(tix, tempMat);
      }
    //}
    /*else // narrow-band
    {
      Matrix<Float> tempModel;
      tempModel.reference(itsMatModels[0]);
      Int retval = itsCleaner.aspclean( tempModel );
      iterdone = itsCleaner.numberIterations();

      if( retval==-1 ) {os << LogIO::WARN << "AspClean minor cycle stopped on cleaning consecutive smallest scale" << LogIO::POST; }
      if( retval==-2 ) {os << LogIO::WARN << "AspClean minor cycle stopped at large scale negative or diverging" << LogIO::POST;}
      if( retval==-3 ) {os << LogIO::WARN << "AspClean minor cycle stopped because it is diverging" << LogIO::POST; }
    }*/

    peakresidual = itsCleaner.getterPeakResidual();
    //cout << "SDAlg: peakres " << peakresidual << endl;
    modelflux = sum( itsMatModels[0] );
  }

  void SDAlgorithmMTAspClean::restore(std::shared_ptr<SIImageStore> imagestore)
  {

    LogIO os( LogOrigin("SDAlgorithmMTAspClean","restore",WHERE) );

    if (!imagestore->hasResidualImage()) return;

    if (itsNTerms == 1) // narrow-band only needs default restore
    {
      SDAlgorithmBase::restore(imagestore);
      return;
    }

    // Compute principal solution
    // Loop over polarization planes here, as MT knows only about Matrices.
    Int nSubChans, nSubPols;
    queryDesiredShape(nSubChans, nSubPols, imagestore->getShape());
    //cout << "nSubChans " << nSubChans << " nSubPols " << nSubPols << " itsMCsetup " << itsMCsetup << endl;
    for (Int chanid=0; chanid<nSubChans; chanid++) // redundant since only 1 chan
    {
      for (Int polid=0; polid<nSubPols; polid++) // one pol plane at a time
      {
        itsImages = imagestore->getSubImageStore( 0, 1, chanid, nSubChans, polid, nSubPols);
        //cout << "Units for chan " << chanid << " and pol " << polid << " are " << itsImages->image()->units().getName() << endl;

        //  ----------- do once if trying to 'only restore' without model ----------
        //if (itsMCsetup)
        //{
          itsCleaner.setNTaylorTerms(itsNTerms);
          itsCleaner.allocateMemory(itsImages->getShape()[0], itsImages->getShape()[1]);

          itsMatPsfs.resize( 2*itsNTerms-1 );
          for(uInt tix=0; tix < 2*itsNTerms-1; tix++)
            (itsImages->psf(tix))->get( itsMatPsfs[tix], True );

          for(uInt tix=0; tix < 2*itsNTerms-1; tix++)
          {
            Matrix<Float> tempMat;
            tempMat.reference(itsMatPsfs[tix]);
            itsCleaner.setMTPsf(tix, tempMat);
          }

          //cout << "Setting up the Asp Cleaner once" << endl;
          const Float width = itsCleaner.getPsfGaussianWidth(*(itsImages->psf(0)));
          //if user does not provide the largest scale, we calculate it internally.
          itsCleaner.setUserLargestScale(itsUserLargestScale);
          itsCleaner.getLargestScaleSize(*(itsImages->psf(0)));
          
          if (itsPrevPsfWidth != width)
          {
            itsPrevPsfWidth = width;
            itsCleaner.setInitScaleXfrs(width);
          }

          itsCleaner.stopPointMode( itsStopPointMode );
          itsCleaner.ignoreCenterBox( true ); // Clean full image
          itsCleaner.setFusedThreshold(itsFusedThreshold);

        //  itsMCsetup = false;
        //}

        /// -----------------------------------------
        Vector<TempImage<Float> > tempResOrig(itsNTerms);

        // Set residual images into mtcleaner
        for(uInt tix=0; tix < itsNTerms; tix++)
        {
          Array<Float> tempArr;
          (itsImages->residual(tix))->get( tempArr, True );
          Matrix<Float> tempMat;
          tempMat.reference( tempArr );
          itsCleaner.setResidual( tix, tempMat );

          // Also save them temporarily (copies)
          tempResOrig[tix] = TempImage<Float>(itsImages->getShape(), itsImages->residual(tix)->coordinates());
          tempResOrig[tix].copyData( LatticeExpr<Float>(* ( itsImages->residual(tix) ) ) );

        }

        // Modify the original in place
        itsCleaner.computePrincipalSolution();

        for(uInt tix=0; tix < itsNTerms; tix++)
        {
          Matrix<Float> tempRes;
          itsCleaner.getResidual(tix, tempRes);
          (itsImages->residual(tix))->put(tempRes);
        }

        // Calculate restored image and alpha using modified residuals
        SDAlgorithmBase::restore( itsImages );

        // Put back original unmodified residuals
        for (uInt tix=0; tix<itsNTerms; tix++)
          (itsImages->residual(tix))->copyData( LatticeExpr<Float>( tempResOrig(tix) ) );

      } // for polid loop
    }// for chanid loop

    // This log message is important. This call of imagestore->image(...) is the first call if there is
    // a multi-channel or multi-pol image. This is what will set the units correctly. Ref. CAS-13153
    os << LogIO::POST << "Restored images : ";
    for(uInt tix=0; tix<itsNTerms; tix++)
      os << LogIO::POST << imagestore->image(tix)->name() << "  (model=" << imagestore->model(tix)->name() << ") " ;
    os << LogIO::POST << endl;
    //cout << "Units for  " << imagestore->image()->name() << "  aaaaaare " << imagestore->image()->units().getName() << endl;
  }


  void SDAlgorithmMTAspClean::finalizeDeconvolver()
  { 
    for(uInt tix=0; tix<itsNTerms; tix++)
    {
      // debug
      /*float maxvalue;
      IPosition peakpos;
      findMaxAbs(itsMatModels[tix], maxvalue, peakpos);
      cout << "finalized: model[t" << tix << "]: maxval " << maxvalue << " peakpos " << peakpos << endl;*/

      (itsImages->residual(tix))->put( itsMatResiduals[tix] );
      (itsImages->model(tix))->put( itsMatModels[tix] );
    }
  }

} //# NAMESPACE CASA - END
