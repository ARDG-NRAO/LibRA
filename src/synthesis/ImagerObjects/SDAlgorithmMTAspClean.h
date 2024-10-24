//# SDAlgorithmMTAspClean.h: Definition for SDAlgorithmMTAspClean
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

#ifndef SYNTHESIS_SDALGORITHMMTASPCLEAN_H
#define SYNTHESIS_SDALGORITHMMTASPCLEAN_H

#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/images/Images/TempImage.h>
#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogSink.h>
#include <casacore/casa/System/PGPlotter.h>

/*#include <ms/MeasurementSets/MeasurementSet.h>
#include <synthesis/MeasurementComponents/SkyModel.h>
#include <casa/Arrays/Matrix.h>
#include <images/Images/ImageInterface.h>
#include <images/Images/PagedImage.h>
#include <images/Images/TempImage.h>
#include <casa/Logging/LogMessage.h>
#include <casa/Logging/LogSink.h>
#include <casa/System/PGPlotter.h>*/

#include <synthesis/ImagerObjects/SDAlgorithmBase.h>
#include <synthesis/MeasurementEquations/MTAspMatrixCleaner.h>

namespace casa { //# NAMESPACE CASA - BEGIN

  /* Forware Declaration */
  class SIMinorCycleController;


  class SDAlgorithmMTAspClean : public SDAlgorithmBase
  {
  public:

    // Empty constructor
    SDAlgorithmMTAspClean(casacore::uInt nTaylorTerms, casacore::Float fusedThreshold = 0.0, bool isSingle = true, casacore::Int largestScale = -1, casacore::Int stoppointmode = -1);
    virtual  ~SDAlgorithmMTAspClean();

    void restore(std::shared_ptr<SIImageStore> imagestore);

  protected:

    // Local functions to be overloaded by various algorithm deconvolvers.
    void takeOneStep( casacore::Float loopgain, casacore::Int cycleNiter, casacore::Float cycleThreshold, casacore::Float &peakresidual, casacore::Float &modelflux, casacore::Int &iterdone );
    void initializeDeconvolver();
    void finalizeDeconvolver();
    casacore::uInt getNTaylorTerms(){ return itsNTerms; };

    casacore::Vector< casacore::Array<casacore::Float> >itsMatPsfs, itsMatResiduals, itsMatModels;
    casacore::Array<casacore::Float> itsMatMask;  // Make an array if we eventually use multi-term masks...

    MTAspMatrixCleaner itsCleaner;
    std::vector<casacore::Float> itsScaleSizes;
    casacore::Int itsStopPointMode;
    casacore::Float itsFusedThreshold;
    casacore::uInt itsNTerms;
    casacore::Int itsUserLargestScale;

    private:
    casacore::Bool itsMCsetup; // if we should do setup or not
    casacore::Float itsPrevPsfWidth;
    bool itsIsSingle;

  };

} //# NAMESPACE CASA - END

#endif
