//# SDAlgorithmAAspClean.cc: Implementation of SDAlgorithmAAspClean classes
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

#include <casa/Arrays/ArrayMath.h>
#include <casa/OS/HostInfo.h>
#include <synthesis/ImagerObjects/SDAlgorithmAAspClean.h>

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
#include <msvis/MSVis/StokesVector.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

  SDAlgorithmAAspClean::SDAlgorithmAAspClean(Float fusedThreshold, bool isSingle, Int largestScale, Int stoppointmode):
    SDAlgorithmBase(),
    itsMatPsf(), itsMatResidual(), itsMatModel(),
    itsCleaner(),
    itsStopPointMode(stoppointmode),
    itsMCsetup(true),
    itsFusedThreshold(fusedThreshold),
    itsPrevPsfWidth(0),
    itsIsSingle(isSingle),
    itsUserLargestScale(largestScale)
  {
    itsAlgorithmName = String("asp");
  }

  SDAlgorithmAAspClean::~SDAlgorithmAAspClean()
  {

  }

  void SDAlgorithmAAspClean::initializeDeconvolver()
  {
    LogIO os(LogOrigin("SDAlgorithmAAspClean", "initializeDeconvolver", WHERE));
    AlwaysAssert((bool)itsImages, AipsError);

    itsImages->residual()->get( itsMatResidual, true );
    itsImages->model()->get( itsMatModel, true );
    itsImages->psf()->get( itsMatPsf, true );
    itsImages->mask()->get( itsMatMask, true );

    // Initialize the AspMatrixCleaner.
    // If it's single channel, this only needs to be computed once.
    // Otherwise, it needs to be called repeatedly at each minor cycle start to
    // get psf for each channel
    if (itsMCsetup)
    {
      Matrix<Float> tempMat(itsMatPsf);
      itsCleaner.setPsf(tempMat);
      // Initial scales are unchanged and only need to be
      // computed when psf width is updated
      const Float width = itsCleaner.getPsfGaussianWidth(*(itsImages->psf()));
      //if user does not provide the largest scale, we calculate it internally.
      itsCleaner.setUserLargestScale(itsUserLargestScale);
      // we do not use the shortest baseline approach below b/c of reasons in CAS-940 dated around Jan 2022
      // itsCleaner.getLargestScaleSize(*(itsImages->psf()));

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
      // Not used. Kept for unit test
      //Matrix<Float> tempMat1(itsMatResidual);
      //itsCleaner.setOrigDirty( tempMat1 );

      if (itsFusedThreshold < 0)
      {
        os << LogIO::WARN << "Acceptable fusedthreshld values are >= 0. Changing fusedthreshold from " << itsFusedThreshold << " to 0." << LogIO::POST;
        itsFusedThreshold = 0.0;
      }

      itsCleaner.setFusedThreshold(itsFusedThreshold);
    }

    // Parts to be repeated at each minor cycle start....
    itsCleaner.setInitScaleMasks(itsMatMask);
    itsCleaner.setaspcontrol(0, 0, 0, Quantity(0.0, "%"));/// Needs to come before the rest

    Matrix<Float> tempMat1;
    tempMat1.reference(itsMatResidual);
    itsCleaner.setDirty( tempMat1 );
    // InitScaleXfrs and InitScaleMasks should already be set
    itsScaleSizes.clear();
    itsScaleSizes = itsCleaner.getActiveSetAspen();
    itsScaleSizes.push_back(0.0); // put 0 scale
    itsCleaner.defineAspScales(itsScaleSizes);
  }


  void SDAlgorithmAAspClean::takeOneStep( Float loopgain,
					  Int cycleNiter,
					  Float cycleThreshold,
					  Float &peakresidual,
					  Float &modelflux,
					  Int &iterdone)
  {
    LogIO os( LogOrigin("SDAlgorithmAAspClean","takeOneStep", WHERE) );

    Quantity thresh(cycleThreshold, "Jy");
    itsCleaner.setaspcontrol(cycleNiter, loopgain, thresh, Quantity(0.0, "%"));
    Matrix<Float> tempModel;
    tempModel.reference( itsMatModel );
    //save the previous model
    Matrix<Float> prevModel;
    prevModel = itsMatModel;

    //cout << "AAspALMS,  matrix shape : " << tempModel.shape() << " array shape : " << itsMatModel.shape() << endl;

    // retval
    //  1 = converged
    //  0 = not converged but behaving normally
    // -1 = not converged and stopped on cleaning consecutive smallest scale
    // -2 = not converged and either large scale hit negative or diverging
    // -3 = clean is diverging rather than converging
    itsCleaner.startingIteration( 0 );
    Int retval = itsCleaner.aspclean( tempModel );
    iterdone = itsCleaner.numberIterations();

    if( retval==-1 ) {os << LogIO::WARN << "AspClean minor cycle stopped on cleaning consecutive smallest scale" << LogIO::POST; }
    if( retval==-2 ) {os << LogIO::WARN << "AspClean minor cycle stopped at large scale negative or diverging" << LogIO::POST;}
    if( retval==-3 ) {os << LogIO::WARN << "AspClean minor cycle stopped because it is diverging" << LogIO::POST; }

    // update residual - this is critical
    itsMatResidual = itsCleaner.getterResidual();

    peakresidual = itsCleaner.getterPeakResidual();
    //cout << "SDAlg: peakres " << peakresidual << endl;
    modelflux = sum( itsMatModel );
  }

  void SDAlgorithmAAspClean::finalizeDeconvolver()
  {
    (itsImages->residual())->put( itsMatResidual );
    (itsImages->model())->put( itsMatModel );
  }

} //# NAMESPACE CASA - END
