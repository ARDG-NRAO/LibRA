//# SDAlgorithmClarkClean2.cc: Implementation of SDAlgorithmClarkClean2 classes
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

#include <synthesis/ImagerObjects/SDAlgorithmClarkClean2.h>
#include <synthesis/MeasurementEquations/ClarkCleanLatModel.h>
#include <synthesis/MeasurementEquations/LatConvEquation.h>


using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

  SDAlgorithmClarkClean2::SDAlgorithmClarkClean2(String clarktype):
    SDAlgorithmBase()
 {
   itsAlgorithmName=String(clarktype);
   itsMatDeltaModel.resize();
   itsMatModel.resize();
 }

  SDAlgorithmClarkClean2::~SDAlgorithmClarkClean2()
 {
   
 }
  Long SDAlgorithmClarkClean2::estimateRAM(const vector<int>& imsize){
    Long mem=0;
    if(itsImages)
    //Clark deconvolvers will have psf + residual + model + mask (1 plane at a time)
      mem=sizeof(Float)*(itsImages->getShape()(0))*(itsImages->getShape()(1))*6/1024;
    else if(imsize.size() >1)
       mem=sizeof(Float)*(imsize[0])*(imsize[1])*6/1024;
    else
      return 0;
      //throw(AipsError("Deconvolver cannot estimate the memory usage at this point"));
    return mem;
  }

  //  void SDAlgorithmClarkClean2::initializeDeconvolver( Float &peakresidual, Float &modelflux )
  void SDAlgorithmClarkClean2::initializeDeconvolver()
  {
    LogIO os( LogOrigin("SDAlgorithmClarkClean2","initializeDeconvolver",WHERE) );

    itsImages->residual()->get( itsMatResidual, true );
    
    //itsImages->model()->get( itsMatModel, true ); not necessary here as it is done in Onestep all over again
    itsImages->psf()->get( itsMatPsf, true );
    itsImages->mask()->get( itsMatMask, true );

    /*
    cout << "Clark : initDecon : " << itsImages->residual()->shape() << " : " << itsMatResidual.shape() 
    	 << itsImages->model()->shape() << " : " << itsMatModel.shape() 
  	 << itsImages->psf()->shape() << " : " << itsMatPsf.shape() 
  	 << endl;
    */
    /*
    IPosition shp = itsMatResidual.shape();
    IPosition startp( shp.nelements(),0);
    IPosition stopp( shp.nelements(),0);
    stopp[0]=shp[0]-1; stopp[1]=shp[1]-1;
    Matrix<Float> oneplane;
    oneplane.doNotDegenerate( itsMatResidual(startp,stopp), IPosition(  ) );
    findMaxAbs( oneplane, itsPeakResidual, itsMaxPos );
    */

    /*
    findMaxAbs( itsMatResidual, itsPeakResidual, itsMaxPos );
    itsModelFlux = sum( itsMatModel );

    peakresidual = itsPeakResidual;
    modelflux = itsModelFlux;
    */

    // Initialize the Delta Image model. Resize if needed.
    // if ( itsMatDeltaModel.shape().nelements() != itsMatModel.shape().nelements() )
    //  { itsMatDeltaModel.resize ( itsMatModel.shape() ); }


    //cout << "Image Shapes : " << itsMatResidual.shape() << endl;

    ////////////////////////////////////////////////////////////////////////////////////
    // Get psf patch size
    // ...........fill this in....... after fitted beams are precomputed earlier..... and stored in ImageStore.

      // 4 pixels:  pretty arbitrary, but only look for sidelobes
      // outside the inner (2n+1) * (2n+1) square
      Int ncent=4;
      
      {//locate peak size
	CoordinateSystem cs= itsImages->psf()->coordinates();
	Vector<String> unitas=cs.worldAxisUnits();
	unitas(0)="arcsec";
	unitas(1)="arcsec";
	cs.setWorldAxisUnits(unitas);
	//Get the minimum increment in arcsec
	Double incr=abs(min(cs.increment()(0), cs.increment()(1)));
	if(incr > 0.0){
	  GaussianBeam beamModel=itsImages->getBeamSet()(0,0);
	  //	  GaussianBeam beamModel=beam(model)(0,0);
	  ncent=max(ncent, Int(ceil(beamModel.getMajor("arcsec")/incr)));
	  ncent=max(ncent, Int(ceil(beamModel.getMinor("arcsec")/incr)));
	}
      }
      
      psfpatch_p=3*ncent+1;

      os << "Choosing a PSF patch size of " << psfpatch_p << " pixels."<< LogIO::POST;

  }

  void SDAlgorithmClarkClean2::takeOneStep( Float loopgain, 
					    Int cycleNiter, 
					    Float cycleThreshold, 
					    Float &peakresidual, 
					    Float &modelflux, 
					    Int &iterdone)
  {

    //// Store current model in this matrix.
    LatticeLocker lockmod (*(itsImages->model()), FileLocker::Write);
    LatticeLocker lockresid (*(itsImages->residual()), FileLocker::Write);
    itsImages->model()->get( itsMatDeltaModel, true );
    //    cerr << "Orig deltaMod " << max(itsMatDeltaModel) << endl;
    itsMatModel.resize();
    itsMatModel.assign( itsMatDeltaModel ); // This should make an explicit copy
    //cerr << "Orig Mod " << max(itsMatModel) << endl;
    //// Set model to zero
    itsImages->model()->set( 0.0 );
    
    // cerr << "max after set " << max(itsImages->model()->get(true)) << endl;

    //// Prepare a single plane mask. For multi-stokes, it still needs a single mask.
    IPosition shp = itsImages->mask()->shape();
    IPosition startp( shp.nelements(),0);
    IPosition stopp( shp.nelements(),1);
    stopp[0]=shp[0]; stopp[1]=shp[1];
    Slicer oneslice(startp, stopp);
    //cout << "Making mask slice of : " << oneslice.start() << " : " << oneslice.end() << endl;
    SubImage<Float> oneplane( *(itsImages->mask()), oneslice )  ;

    //// Set up the cleaner
    ClarkCleanLatModel cleaner(*(itsImages->model()));
    cleaner.setMask( oneplane );
    cleaner.setNumberIterations(cycleNiter);
    cleaner.setMaxNumberMajorCycles(10);
    cleaner.setMaxNumberMinorIterations(cycleNiter);
    cleaner.setGain(loopgain);
    cleaner.setThreshold(cycleThreshold);
    cleaner.setPsfPatchSize(IPosition(2,psfpatch_p));
    cleaner.setHistLength(1024);
    //    cleaner.setMaxExtPsf(..)
    cleaner.setSpeedup(-1.0);
    cleaner.setMaxNumPix(32*1024);

    //// Prepare a single plane PSF
    IPosition shp2 = itsImages->psf()->shape();
    IPosition startp2( shp2.nelements(),0);
    IPosition stopp2( shp2.nelements(),1);
    stopp2[0]=shp2[0]; stopp2[1]=shp2[1];
    Slicer oneslice2(startp2, stopp2);
    //    cout << "Making psf slice of : " << oneslice2.start() << " : " << oneslice2.end() << endl;
    SubImage<Float> oneplane2( *(itsImages->psf()), oneslice2 )  ;

    //// Make the convolution equation with a single plane PSF and possibly multiplane residual
    LatConvEquation eqn( oneplane2 , *(itsImages->residual()) );
   
    //Bool result = 
    cleaner.solve( eqn );
    //    cout << result << endl;

    iterdone = cleaner.getNumberIterations();

    // Retrieve residual before major cycle
    itsImages->residual()->copyData( cleaner.getResidual() );
    itsImages->residual()->get( itsMatResidual, true );

    // Add delta model to old model
    //Bool ret2 = 
    itsImages->model()->get( itsMatDeltaModel, true );
    //cerr << "deltaModel " << max(itsMatDeltaModel) << endl;
    itsMatModel += itsMatDeltaModel;
    //cerr << "totModel " << max(itsMatModel) << endl;
    //--------------------------------- DECIDE WHICH PEAK RESIDUAL TO USE HERE.....

    //////  Find Peak Residual across the whole image
    /*
    itsImages->residual()->get( itsMatResidual, true );
    itsImages->mask()->get( itsMatMask, true );
    findMaxAbsMask( itsMatResidual, itsMatMask, itsPeakResidual, itsMaxPos );
    */

    ////// Find Peak Residual from the Clark Clean method (within current active pixels only).
    itsPeakResidual = cleaner.getMaxResidual();

    peakresidual = itsPeakResidual;

    // Find Total Model flux
    modelflux = sum( itsMatModel ); // Performance hog ?
    //cerr << "tot flux " << modelflux << endl;
    (itsImages->model())->put( itsMatModel );
  }	    

  void SDAlgorithmClarkClean2::finalizeDeconvolver()
  {
    (itsImages->residual())->put( itsMatResidual );
    (itsImages->model())->put( itsMatModel );
  }


  void SDAlgorithmClarkClean2::queryDesiredShape(Int &nchanchunks, 
						Int &npolchunks, 
						IPosition imshape) // , nImageFacets.
  {  
    AlwaysAssert( imshape.nelements()==4, AipsError );
    nchanchunks=imshape(3);  // Each channel should appear separately.

    itsAlgorithmName.downcase();

    if( itsAlgorithmName.matches("clarkstokes") )
      {
	npolchunks=imshape(2); // Each pol should appear separately.
      }
    else // "Clark"
      {
	npolchunks=1; // Send in all pols together.
      }

  }


} //# NAMESPACE CASA - END

