//# SISubterBot.h: This file contains the interface definition SISubIterBot class
//#
//#  CASA - Common Astronomy Software Applications (http://casa.nrao.edu/)
//#  Copyright (C) Associated Universities, Inc. Washington DC, USA 2011, All rights reserved.
//#  Copyright (C) European Southern Observatory, 2011, All rights reserved.
//#
//#  This library is free software; you can redistribute it and/or
//#  modify it under the terms of the GNU Lesser General Public
//#  License as published by the Free software Foundation; either
//#  version 2.1 of the License, or (at your option) any later version.
//#
//#  This library is distributed in the hope that it will be useful,
//#  but WITHOUT ANY WARRANTY, without even the implied warranty of
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//#  Lesser General Public License for more details.
//#
//#  You should have received a copy of the GNU Lesser General Public
//#  License along with this library; if not, write to the Free Software
//#  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
//#  MA 02111-1307  USA
//# $Id: $

#ifndef SYNTHESIS_MINORCYCLECONTROLLER
#define SYNTHESIS_MINORCYCLECONTROLLER

// .casarc interface
#include <casacore/casa/System/AipsrcValue.h>

// System utilities (for profiling macros)
#include <casacore/casa/OS/HostInfo.h>
#include <sys/time.h>


// Include files for the DBus Service
//#include <casadbus/interfaces/SynthImager.adaptor.h>

namespace casacore{

  class Record;
}

namespace casa { //# NAMESPACE CASA - BEGIN
  

  class SIMinorCycleController
 {
  public:
    //   0           1          2            3              4           5       6      7                  8                9               10                11 "No Mask"      12           13         14          15         16         17       18
    // ["iterDone", "peakRes", "modelFlux", "cycleThresh", "mapperId", "chan", "pol", "cycleStartIters", "startIterDone", "startPeakRes", "startModelFlux", "startPeakResNM", "peakResNM", "masksum", "mpiServer", "peakMem", "runtime", "immod", "stopCode"]
    static const casacore::Int nSummaryFields = 19;

    // make noncopyable...
    SIMinorCycleController( const SIMinorCycleController& ) = delete;
    SIMinorCycleController &operator=( const SIMinorCycleController& ) = delete;
    SIMinorCycleController();

    virtual ~SIMinorCycleController();

    /* Method to call to determine if a major cycle is required, true if
         - exceeded number maxcycleniter
         - exceeded cycle threshold
    */
    virtual casacore::Int majorCycleRequired(casacore::Float currentPeakResidual);

    /* This method sets the following controls for the Minor Cycle Controller:
       - niter
       - cycleniter
       - interactiveniter?
       - threshold
       - cyclethreshold
       - interactive theshold?
       - loop gain
       - cycle factor
    */
    void setCycleControls(casacore::Record& );

    /* Get Cycle Execution casacore::Data:
       Returns information about the set of cycles just executed:
       - iterdone
       - maxcycleiterdone
       - updated model flag
       - peak residual
       - minor cycle summary
    */
    casacore::Record getCycleExecutionRecord();


    /* Get Initialization Data
       Returns information at the beginning of a cycle:
       - peak residual
       - maxPSFSidelobe
    */
    casacore::Record getCycleInitializationRecord();

    /* Getter Methods for the control variables */
    casacore::Float getLoopGain(); 

    void incrementMinorCycleCount(casacore::Int itersDonePerStep=1);

    casacore::Int getIterDone();
    casacore::Int getCycleNiter();
    casacore::Float getCycleThreshold();
    casacore::Bool isThresholdReached();

    casacore::Float getPBMask();

    /* This method resets the iteration counter for the cycle */
    void resetCycleIter();

    /* Flag to note that the model has been updated */
    void setUpdatedModelFlag(casacore::Bool updatedmodel);

   void addSummaryMinor(casacore::uInt deconvolverid, casacore::uInt channel, casacore::uInt polarity,
                        casacore::Int cycleStartIter, casacore::Int startIterDone, casacore::Float startmodel, casacore::Float startpeakresidual, casacore::Float startpeakresidualnomask,
                        casacore::Float model, casacore::Float peakresidual, casacore::Float peakresidualnomask, casacore::Float masksum, casacore::Int mpiRank, casacore::Float peakMem, casacore::Float runtime, casacore::Int stopCode);
    
    /* Variables to track status inside each Deconvolver */
    casacore::Float getPeakResidual();
    casacore::Float getIntegratedFlux();
    casacore::Float getMaxPsfSidelobe();

    /* user input for n-sigma */
    casacore::Float getNsigma();
    
    void setPeakResidual(casacore::Float peakResidual);
    void setPeakResidualNoMask(casacore::Float peakResidual);
    void addIntegratedFlux(casacore::Float integratedFlux);
    void setMaxPsfSidelobe(casacore::Float maxPsfSidelobe);
   void setMadRMS(casacore::Float madRMS);
   void setNsigmaThreshold(casacore::Float nsigmaThreshold);
   void setMaskSum(casacore::Float maskSum);
   void setNsigma(casacore::Float nsigma);

   void setPBMask(casacore::Float pbMaskLevel);

   void resetMinResidual();

    // temporary CAS-13683 workaround
    static casacore::Bool useSmallSummaryminor();

 protected:
    /* Control Variables */
    casacore::Int    itsCycleNiter;
    casacore::Float itsCycleThreshold;
    casacore::Float itsNsigmaThreshold;
    casacore::Float itsLoopGain;
    casacore::Bool itsIsThresholdReached;    
 
    casacore::Bool  itsUpdatedModelFlag;

    /* Status Reporting Variables */
    casacore::Int   itsIterDone; // Iterations done during this set of cycles
    casacore::Int   itsCycleIterDone; // Iterations done on this subimage
   casacore::Int   itsIterDiff; // Difference between latest and previous count, to catch zero increments.

    /* Useful only for reporting */
    casacore::Int   itsTotalIterDone; // Total iterations done by deconvolver

    /* This variable keeps track of the maximum number of iterations done
       during a major cycle */
    casacore::Int   itsMaxCycleIterDone;

   casacore::Float itsPeakResidual;
   casacore::Float itsIntegratedFlux;
   casacore::Float itsMaxPsfSidelobe;

   casacore::Float itsMinResidual;
   casacore::Float itsMinResidualNoMask;
   casacore::Float itsPeakResidualNoMask;
   casacore::Float itsNsigma;
   casacore::Float itsMadRMS;
   casacore::Float itsMaskSum;

   casacore::Float itsPBMaskLevel;

    /* Summary Variable */
    casacore::Array<casacore::Double> itsSummaryMinor;
    casacore::Int itsDeconvolverID;    /* An ID per Deconvolver. Used only for the summary */
  };

    
} //# NAMESPACE CASA - END

#endif /* SISUBITERBOT_H_ */
