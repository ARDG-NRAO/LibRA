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
#include <casa/Containers/Record.h>

#include<synthesis/ImagerObjects/SIMinorCycleController.h>

#include <casa/sstream.h>

#include <casa/Logging/LogMessage.h>
#include <casa/Logging/LogIO.h>
#include <casa/Logging/LogSink.h>

#include <casa/System/Choice.h>
#include <msvis/MSVis/StokesVector.h>


using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

  SDAlgorithmAAspClean::SDAlgorithmAAspClean():
    SDAlgorithmBase(), hogbom_p()
  {
    itsAlgorithmName=String("aasp");
    cerr << "#### " << "AAsp constructed" << endl;
  }

  SDAlgorithmAAspClean::~SDAlgorithmAAspClean()
  {
    
  }

  //  void SDAlgorithmAAspClean::initializeDeconvolver( Float &peakresidual, Float &modelflux )
  void SDAlgorithmAAspClean::initializeDeconvolver()
  {
    LogIO os( LogOrigin("SDAlgorithmAAspClean","initializeDeconvolver",WHERE) );

    itsImages->residual()->get( itsMatResidual, true );
    itsImages->model()->get( itsMatModel, true );
    itsImages->psf()->get( itsMatPsf, true );
    itsImages->mask()->get( itsMatMask, true );

    /*
    /////////////////
    findMaxAbsMask( itsMatResidual, itsMatMask, itsPeakResidual, itsMaxPos );
    peakresidual = itsPeakResidual;
    modelflux = sum( itsMatModel ); // Performance hog ?
    */
  }


  void SDAlgorithmAAspClean::takeOneStep( Float loopgain, 
					  Int cycleNiter, 
					  Float cycleThreshold, 
					  Float &peakresidual, 
					  Float &modelflux, 
					  Int &iterdone)
  {
    LogIO os( LogOrigin("SDAlgorithmAAspClean","takeOneStep") );

    os << "AAsp algorithms construction work in progress...." << LogIO::WARN << LogIO::POST;
    
    // Bool delete_iti, delete_its, delete_itp, delete_itm;
    // const Float *lpsf_data, *lmask_data;
    // Float *limage_data, *limageStep_data;

    // limage_data = itsMatModel.getStorage( delete_iti );
    // limageStep_data = itsMatResidual.getStorage( delete_its );
    // lpsf_data = itsMatPsf.getStorage( delete_itp );
    // lmask_data = itsMatMask.getStorage( delete_itm );

    Float loopGain_l=loopgain, cycleThreshold_l=cycleThreshold;
    Int cycleNiter_l = cycleNiter;
    {
      Record lcRec;
      SIMinorCycleController loopcontrols;

      lcRec.define("loopgain", loopGain_l);
      lcRec.define("cycleniter", cycleNiter_l);
      lcRec.define("cyclethreshold", cycleThreshold_l);

      loopcontrols.setCycleControls(lcRec);
      hogbom_p.deconvolve(loopcontrols, itsImages,1);
    }

    
    iterdone=cycleNiter;
    itsImages->residual()->get( itsMatResidual, true );
    itsImages->model()->get( itsMatModel, true );
    
    // itsMatModel.putStorage( limage_data, delete_iti );
    // itsMatResidual.putStorage( limageStep_data, delete_its );
    // itsMatPsf.freeStorage( lpsf_data, delete_itp );
    // itsMatMask.freeStorage( lmask_data, delete_itm );
    
    
    /////////////////
    findMaxAbsMask( itsMatResidual, itsMatMask, itsPeakResidual, itsMaxPos );
    peakresidual = itsPeakResidual;

    modelflux = sum( itsMatModel ); // Performance hog ?
  }	    

  void SDAlgorithmAAspClean::finalizeDeconvolver()
  {
    (itsImages->residual())->put( itsMatResidual );
    (itsImages->model())->put( itsMatModel );
  }


} //# NAMESPACE CASA - END

