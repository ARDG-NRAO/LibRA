//# SDAlgorithmMSClean.h: Definition for SDAlgorithmMSClean
//# Copyright (C) 1996,1997,1998,1999,2000,2002
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
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$

#ifndef SYNTHESIS_SDALGORITHMAutoCLEAN_H
#define SYNTHESIS_SDALGORITHMAutoCLEAN_H

#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/images/Images/TempImage.h>
#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogSink.h>
#include <casacore/casa/System/PGPlotter.h>

#include<synthesis/ImagerObjects/SDAlgorithmBase.h>
#include <synthesis/MeasurementEquations/AutoCleaner.h>

namespace casa { //# NAMESPACE CASA - BEGIN

  /* Forware Declaration */
  class SIMinorCycleController;


  class SDAlgorithmAutoClean : public SDAlgorithmBase 
  {
  public:
    
    // Empty constructor
    SDAlgorithmAutoClean(casacore::Float autoThreshold, casacore::Int autoMaxiter, casacore::Float autoGain, casacore::Bool autoHogbom, casacore::Float autoTrigger, casacore::Float autoPower, casacore::Int autoXMask, casacore::Int autoYMask);
    virtual  ~SDAlgorithmAutoClean();
    
    //returns the estimate of memory used in kilobytes (kB);
    virtual casacore::Long estimateRAM( const std::vector<int>& imsize);
  protected:
    
    // Local functions to be overloaded by various algorithm deconvolvers.
    void takeOneStep( casacore::Float loopgain, casacore::Int cycleNiter, casacore::Float cycleThreshold, 
		      casacore::Float &peakresidual, casacore::Float &modelflux, casacore::Int &iterdone );
    void initializeDeconvolver();
    void finalizeDeconvolver();

    casacore::Array<casacore::Float> itsMatPsf, itsMatResidual, itsMatModel;
    casacore::Array<casacore::Float> itsMatMask;  // Make an array if we eventually use multi-term masks...
    casacore::Array<casacore::Float> itsMatTemp;

    casacore::Float itsautoThreshold;
    casacore::Int itsautoMaxiter;
    casacore::Float itsautoGain;
    casacore::Bool itsautoHogbom;
    casacore::Float itsautoTrigger;
    casacore::Float itsautoPower;
    casacore::Int itsautoXMask;
    casacore::Int itsautoYMask;

    AutoCleaner itsCleaner;

  private:
    //casacore::Bool itsMCsetup; 

  };

} //# NAMESPACE CASA - END

#endif
