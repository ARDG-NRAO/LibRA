//# SDAlgorithmTest.cc: Implementation of SDAlgorithmTest classes
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
#include <synthesis/ImagerObjects/SDAlgorithmTest.h>
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

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN


  SDAlgorithmTest::SDAlgorithmTest():
    SDAlgorithmBase(),
    itsMatResidual(), itsMatModel(), itsMatPsf(),
    itsMaxPos( IPosition() ),
    itsPeakResidual(0.0),
    itsModelFlux(0.0)
 {
   itsAlgorithmName=String("Test");
 }

  SDAlgorithmTest::~SDAlgorithmTest()
 {
   
 }

  void SDAlgorithmTest::initializeDeconvolver( Float &peakresidual, Float &modelflux )
  {

    itsResidual.get( itsMatResidual, true );
    itsModel.get( itsMatModel, true );
    itsPsf.get( itsMatPsf, true );

    //    cout << "Residual image : " << itsMatResidual << endl;
    //  cout << "PSF image : " << itsMatPsf << endl;

    findMaxAbs( itsMatResidual, itsPeakResidual, itsMaxPos );
    itsModelFlux = sum( itsMatModel );

    peakresidual = itsPeakResidual;
    modelflux = itsModelFlux;

  }


  /* NOT USING cycleNiter and cycleThreshold */
  void SDAlgorithmTest::takeOneStep( Float loopgain, Int /*cycleNiter*/, Float /*cycleThreshold*/, Float &peakresidual, Float &modelflux, Int &iterdone )
  {
    /*
    findNextComponent();
    updateModel( loopgain );
    updateResidual( loopgain );
    */

    

    itsMatModel( itsMaxPos ) += itsPeakResidual * loopgain;
    itsModelFlux += itsPeakResidual*loopgain;

    // Remove a scaled and shifted version of the PSF.
    // Calculate blc/trc
    calculatePatchBoundaries();
    Matrix<Float> residualSub = itsMatResidual( itsBlc, itsTrc );
    Matrix<Float> psfSub = itsMatPsf( itsBlcPsf, itsTrcPsf );

    residualSub -= itsPeakResidual * loopgain * psfSub;
      //    itsMatResidual( itsMaxPos ) -= itsPeakResidual*loopgain;

    findMaxAbs( itsMatResidual, itsPeakResidual, itsMaxPos );

    peakresidual = itsPeakResidual;
    modelflux = itsModelFlux;
    iterdone = 1;
  }	    

  void SDAlgorithmTest::finalizeDeconvolver()
  {
    itsResidual.put( itsMatResidual );
    itsModel.put( itsMatModel );
  }


  /*
  void SDAlgorithmTest::findNextComponent()
  {
    //Hogbom Clean. find the peak residual and its location.
    ///findMaxAbs( itsResidual, itsPeakResidual, itsMaxPos );

    // This is a no-op. The peak residual is alread known !

  }

  void SDAlgorithmTest::updateModel( Float loopgain )
  {
    // Add the delta function component to the model image
    //    itsModel.putAt( itsModel.getAt(itsMaxPos) + (itsPeakResidual*loopgain) , itsMaxPos);

    itsMatModel( itsMaxPos ) += itsPeakResidual * loopgain;

    itsModelFlux += itsPeakResidual*loopgain;
  }

  // Subtract the shifted PSF from that location
  void SDAlgorithmTest::updateResidual( Floag loopgain )
  {
    //    itsResidual.putAt( itsResidual.getAt(itsMaxPos) - (itsPeakResidual*loopgain)  , itsMaxPos );
    itsMatResidual( itsMaxPos ) -= itsPeakResidual*loopgain;
    findMaxAbs( itsMatResidual, itsPeakResidual, itsMaxPos );
  }
  */

  /*
  void SDAlgorithmTest::restorePlane( )
  {

    LogIO os( LogOrigin("SDAlgorithmTest","restore",WHERE) );
    
    os << "Smooth model and add residuals "
       << ". Optionally, PB-correct too." << LogIO::POST;


  }
  */

  // Use this decide how to partition
  // the image for separate calls to 'deconvolve'.
  void SDAlgorithmTest::queryDesiredShape(Bool &onechan, Bool &onepol) // , nImageFacets.
  {  
    onechan = true;
    onepol = true;
  }


  /*
Bool SDAlgorithmTest::findMaxAbs(const Matrix<Float>& lattice,
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
  */



  /*
Bool SDAlgorithmTest::findMaxAbsMask(const Matrix<Float>& lattice,
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
  */


  void SDAlgorithmTest::calculatePatchBoundaries()
  {
    IPosition support(itsMatModel.shape());
    IPosition inc(itsMatModel.shape().nelements(), 1);

    itsBlc = IPosition(itsMaxPos-support/2);
    // itsTrc = IPosition(itsMaxPos+support/2-1);
    itsTrc = IPosition(itsMaxPos+support/2);
    LCBox::verify(itsBlc, itsTrc, inc, itsMatModel.shape());
    
    IPosition posPeakPsf( support/2 );

    itsBlcPsf = IPosition(itsBlc+posPeakPsf-itsMaxPos);
    itsTrcPsf = IPosition(itsTrc+posPeakPsf-itsMaxPos);
    LCBox::verify(itsBlcPsf, itsTrcPsf, inc, support);

    makeBoxesSameSize(itsBlc,itsTrc,itsBlcPsf,itsTrcPsf);

    /*    
    cout << "support : " << support;
    cout << "   peak : " << itsMaxPos << "   psf peak : " << posPeakPsf;
    cout << "   blc " << itsBlc.asVector() << " trc " << itsTrc.asVector();
    cout << "   blcPSF " << itsBlcPsf.asVector() << " trcPSF " << itsTrcPsf.asVector() << endl;
    */

  }

void SDAlgorithmTest::makeBoxesSameSize(IPosition& blc1, IPosition& trc1, 
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

