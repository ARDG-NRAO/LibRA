//# SDAlgorithmClarkClean2.h: Definition for SDAlgorithmClarkClean2
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

#ifndef SYNTHESIS_SDALGORITHMCLARKCLEAN2_H
#define SYNTHESIS_SDALGORITHMCLARKCLEAN2_H

#include <casacore/ms/MeasurementSets/MeasurementSet.h>
//#include <synthesis/MeasurementComponents/SkyModel.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/images/Images/TempImage.h>
#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogSink.h>
#include <casacore/casa/System/PGPlotter.h>

#include<synthesis/ImagerObjects/SDAlgorithmBase.h>

namespace casa { //# NAMESPACE CASA - BEGIN

  /* Forware Declaration */
  class SIMinorCycleController;


  class SDAlgorithmClarkClean2 : public SDAlgorithmBase 
  {
  public:
    
    // Empty constructor
    SDAlgorithmClarkClean2(casacore::String clarktype);
    virtual  ~SDAlgorithmClarkClean2();
     ///returns the estimate of memory used in kilobytes (kB);
    virtual casacore::Long estimateRAM(const std::vector<int>& imstor);

  protected:
    
    // Local functions to be overloaded by various algorithm deconvolvers.
    virtual void takeOneStep( casacore::Float loopgain, casacore::Int cycleNiter, casacore::Float cycleThreshold, casacore::Float &peakresidual, casacore::Float &modelflux, casacore::Int &iterdone );
    //    virtual void initializeDeconvolver( casacore::Float &peakresidual, casacore::Float &modelflux );
    virtual void initializeDeconvolver();
    virtual void finalizeDeconvolver();
    virtual void queryDesiredShape(casacore::Int &nchanchunks, casacore::Int& npolchunks, casacore::IPosition imshape);

    casacore::Array<casacore::Float> itsMatResidual, itsMatModel, itsMatPsf, itsMatMask;
    casacore::Array<casacore::Float> itsMatDeltaModel;

    casacore::Int psfpatch_p;

  };

} //# NAMESPACE CASA - END

#endif
