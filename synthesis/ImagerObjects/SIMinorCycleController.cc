

//# SIISubterBot.cc: This file contains the implementation of the SISubIterBot class.
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

#include <synthesis/ImagerObjects/SIMinorCycleController.h>

/* Records Interface */
#include <casacore/casa/Containers/Record.h>
#include <casacore/casa/BasicMath/Math.h>
#include <casacore/casa/Utilities/GenSort.h>
#include <math.h> // For FLT_MAX

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN
  
  
  SIMinorCycleController::SIMinorCycleController(): 
                                itsCycleNiter(0),
                                itsCycleThreshold(0.0),
                                itsNsigmaThreshold(0.0),
                                itsLoopGain(0.1),
                                itsIsThresholdReached(false),
                                itsUpdatedModelFlag(false),
                                itsIterDone(0),
                                itsCycleIterDone(0),
                                itsTotalIterDone(0),
                                itsMaxCycleIterDone(0),
                                itsPeakResidual(0),
                                itsIntegratedFlux(0),
                                itsMaxPsfSidelobe(0),
                                itsMinResidual(0),itsMinResidualNoMask(0),
                                itsPeakResidualNoMask(0), itsNsigma(0),
                                itsMadRMS(0), itsMaskSum(0),
                                itsSummaryMinor(IPosition(2,SIMinorCycleController::nSummaryFields,0)),
				itsDeconvolverID(0) 
  {}



  SIMinorCycleController::~SIMinorCycleController(){}


  Int SIMinorCycleController::majorCycleRequired(Float currentPeakResidual)
  {
    LogIO os( LogOrigin("SIMinorCycleController",__FUNCTION__,WHERE) );
    
    Int stopCode=0;

    // Reached iteration limit
    if (itsCycleIterDone >= itsCycleNiter ) {stopCode=1;}
    // Reached cyclethreshold
    //if( fabs(currentPeakResidual) <= itsCycleThreshold ) { stopCode=2; }
    // Reached cyclethreshold or n-sigma threshold
    //debug (TT)
    //os << LogIO::DEBUG1<< "itsNsigma="<<itsNsigma<<" itsIterDiff="<<itsIterDiff<<LogIO::POST;
    os << LogIO::DEBUG1<< "itsNsigmaThreshoild="<<itsNsigmaThreshold<<" itsCycleThreshold="<<itsCycleThreshold<<" currentPeakRes="<<currentPeakResidual<<LogIO::POST;
    if (itsCycleThreshold >= itsNsigmaThreshold) {
      //if( fabs(currentPeakResidual) <= itsCycleThreshold ) { stopCode=2; }
      if( fabs(currentPeakResidual) <= itsCycleThreshold ) { 
        //itsNsigmaThreshold = 0.0; // since stopped by gobal threshold, reset itsNsigmaThreshold 
        stopCode=2; 
      }
    }
    else {
      if( fabs(currentPeakResidual) <= itsNsigmaThreshold && !(itsIterDiff<=0)) { if (itsNsigma!=0.0) stopCode=6; }
    }
    // Zero iterations done
    if( itsIterDiff==0 ) {stopCode=3;}
    // Diverged : CAS-8767, CAS-8584
    //cout << " itsIterDiff : " << itsIterDiff << "  itsPeak : " << itsPeakResidual << " currentPeak : " << currentPeakResidual << " itsMin : " << itsMinResidual << " stopcode so far : " << stopCode ;
    if( itsIterDiff>0 &&
	( fabs(itsMinResidual) > 0.0 ) && 
	( fabs(currentPeakResidual) - fabs(itsMinResidual) )/ fabs(itsMinResidual) >0.1  ) 
      {stopCode=4;}

   //  cout << " -> " << stopCode << endl;

    /*    // Going nowhere
    if( itsIterDiff > 1500 && 
	( fabs(itsPeakResidual) > 0.0 ) && 
	( fabs(currentPeakResidual) - fabs(itsPeakResidual) )/ fabs(itsPeakResidual) < itsLoopGain  ) 
      {stopCode=5;}
    */

    return stopCode;


  }


  Float SIMinorCycleController::getLoopGain()
  {
    return itsLoopGain;
  }
  
  
  void SIMinorCycleController::setUpdatedModelFlag(Bool updatedmodel)
  {
    itsUpdatedModelFlag = updatedmodel;
  }

  void SIMinorCycleController::incrementMinorCycleCount(Int itersDonePerStep)
  {
    /*
    if( itersDonePerStep <= 0 )
      {
	LogIO os( LogOrigin("SIMinorCycleController",__FUNCTION__,WHERE) );
	os << LogIO::WARN << "Zero iterations done after " << itsCycleIterDone << LogIO::POST;
      }
    */

    itsIterDiff = itersDonePerStep;
    itsIterDone += itersDonePerStep;
    itsTotalIterDone += itersDonePerStep;
    itsCycleIterDone += itersDonePerStep;
  }

  Float SIMinorCycleController::getPeakResidual()
  {
    return itsPeakResidual;
  }

  // This is the max residual across all channels/stokes (subimages).
  // This is returned in the end-minor-cycle record.
  /// TODO : Make arrays/lists for peakresidual per 'subimage'. Max over subims gets returned.
  void SIMinorCycleController::setPeakResidual(Float peakResidual)
  {
    itsPeakResidual = peakResidual;
    //    cout << "Setting peak res (SIMinorCycleController) : " << itsPeakResidual << endl;

    if( itsMinResidual > itsPeakResidual )
      itsMinResidual = itsPeakResidual;

  }

  void SIMinorCycleController::setPeakResidualNoMask(Float peakResidual)
  {
    itsPeakResidualNoMask = peakResidual;
    //    cout << "Setting peak res (SIMinorCycleController) : " << itsPeakResidual << endl;

    if( itsMinResidualNoMask > itsPeakResidualNoMask )
      itsMinResidualNoMask = itsPeakResidualNoMask;

  }

  void SIMinorCycleController::setMadRMS(Float madRMS)
  {
    itsMadRMS = madRMS;
  }

  Float SIMinorCycleController::getNsigma()
  {
    return itsNsigma;
  }

  void SIMinorCycleController::setNsigma(Float nSigma)
  {
    itsNsigma = nSigma;
  }

  void SIMinorCycleController::setNsigmaThreshold(Float nsigmaThreshold)
  {
    
    LogIO os( LogOrigin("SIMinorCycleController",__FUNCTION__,WHERE) );
    itsNsigmaThreshold = nsigmaThreshold;
  }

  void SIMinorCycleController::setPBMask(Float pbMaskLevel)
  {
    itsPBMaskLevel = pbMaskLevel;
  }

  void SIMinorCycleController::setMaskSum(Float maskSum)
  {
    itsMaskSum = maskSum;
  }

  void SIMinorCycleController::resetMinResidual()
  {
    itsMinResidual = itsPeakResidual;
    itsIterDiff=-1;
  }

  Float SIMinorCycleController::getIntegratedFlux()
  {
    return itsIntegratedFlux;
  }

  void SIMinorCycleController::addIntegratedFlux(Float integratedFlux)
  {
    itsIntegratedFlux += integratedFlux;
  }

  Float SIMinorCycleController::getMaxPsfSidelobe()
  {
    return itsMaxPsfSidelobe;
  }

  void SIMinorCycleController::setMaxPsfSidelobe(Float maxPsfSidelobe)
  {
    itsMaxPsfSidelobe = maxPsfSidelobe;
  }

  Int SIMinorCycleController::getIterDone()
  {
    return itsIterDone;
  }

  Int SIMinorCycleController::getCycleNiter()
  {
    return itsCycleNiter;
  }

  Float SIMinorCycleController::getCycleThreshold()
  {
    return itsCycleThreshold;
  }
 
  Bool SIMinorCycleController::isThresholdReached()
  {
    return itsIsThresholdReached;
  }

  Record SIMinorCycleController::getCycleExecutionRecord() {
    LogIO os( LogOrigin("SISkyModel",__FUNCTION__,WHERE) );
    Record returnRecord;

    returnRecord.define( RecordFieldId("iterdone"),  itsIterDone);
    returnRecord.define(RecordFieldId("peakresidual"), itsPeakResidual);
    returnRecord.define(RecordFieldId("updatedmodelflag"), 
                        itsUpdatedModelFlag);
    returnRecord.define(RecordFieldId("summaryminor"), itsSummaryMinor);
    returnRecord.define(RecordFieldId("updatedmodelflag"),
                        itsUpdatedModelFlag);
    returnRecord.define(RecordFieldId("maxcycleiterdone"),
                        itsMaxCycleIterDone);
    returnRecord.define( RecordFieldId("peakresidualnomask"), itsPeakResidualNoMask);

    return returnRecord;
  }

  Float SIMinorCycleController::getPBMask() {
    return itsPBMaskLevel;
  }

  Record SIMinorCycleController::getCycleInitializationRecord() {
    LogIO os( LogOrigin("SIMinorCycleController",__FUNCTION__,WHERE) );

    Record returnRecord;

    /* Control Variables */
    returnRecord.define(RecordFieldId("peakresidual"), itsPeakResidual);
    returnRecord.define(RecordFieldId("maxpsfsidelobe"), itsMaxPsfSidelobe);
    returnRecord.define( RecordFieldId("peakresidualnomask"), itsPeakResidualNoMask);
    returnRecord.define( RecordFieldId("madrms"), itsMadRMS);
    returnRecord.define( RecordFieldId("masksum"), itsMaskSum);
    returnRecord.define( RecordFieldId("nsigmathreshold"), itsNsigmaThreshold);
    returnRecord.define( RecordFieldId("nsigma"), itsNsigma);

    /* Reset Counters and summary for the current set of minorcycle iterations */
    itsIterDone = 0;
    itsIterDiff = -1;
    itsSummaryMinor.resize( IPosition( 2, SIMinorCycleController::nSummaryFields, 0) , true );

    return returnRecord;
  }

  void SIMinorCycleController::setCycleControls(Record &recordIn) {
    LogIO os( LogOrigin("SIMinorCycleController",__FUNCTION__,WHERE) );
    if (recordIn.isDefined("cycleniter"))
      {recordIn.get(RecordFieldId("cycleniter"), itsCycleNiter);}
    else
      {throw(AipsError("cycleniter not defined in input minor-cycle controller") );}

    if (recordIn.isDefined("cyclethreshold")) 
      {recordIn.get(RecordFieldId("cyclethreshold"),itsCycleThreshold);}
    else
      {throw(AipsError("cyclethreshold not defined in input minor-cycle controller") );}
     
    if (recordIn.isDefined("thresholdreached"))
      {recordIn.get(RecordFieldId("thresholdreached"), itsIsThresholdReached);}
    else
      { throw(AipsError("thresholdreached not defined in input minor-cycle controller") );}

    if (recordIn.isDefined("loopgain")) 
      {recordIn.get(RecordFieldId("loopgain"), itsLoopGain);}
    else
      {throw(AipsError("loopgain not defined in input minor-cycle controller") );}

    if (recordIn.isDefined("nsigma"))
      {recordIn.get(RecordFieldId("nsigma"), itsNsigma);}
    else 
      { throw(AipsError(" nsigma is not defined in input minor-cycle controller ") );}

    /* Reset the counters for the new cycle */
    itsMaxCycleIterDone = 0;
    itsCycleIterDone = 0;
    itsUpdatedModelFlag = false;
  }

  void SIMinorCycleController::resetCycleIter(){
    itsMaxCycleIterDone = max(itsCycleIterDone, itsMaxCycleIterDone);
    itsCycleIterDone = 0;
  }

  void SIMinorCycleController::addSummaryMinor(uInt deconvolverid, uInt chan, uInt pol,
                                               Int cycleStartIter, Int startIterDone, Float startmodelflux, Float startpeakresidual, Float startpeakresidualnomask,
                                               Float modelflux, Float peakresidual, Float peakresidualnomask, Float masksum, Int stopCode)
  {
    LogIO os( LogOrigin("SIMinorCycleController", __FUNCTION__ ,WHERE) );

    IPosition shp = itsSummaryMinor.shape();
    if( shp.nelements() != 2 && shp[0] != SIMinorCycleController::nSummaryFields ) 
      throw(AipsError("Internal error in shape of minor-cycle summary record"));
     itsSummaryMinor.resize( IPosition( 2, SIMinorCycleController::nSummaryFields, shp[1]+1 ) , true );
     // iterations done
     itsSummaryMinor( IPosition(2, 0, shp[1] ) ) = itsIterDone;
     // peak residual
     itsSummaryMinor( IPosition(2, 1, shp[1] ) ) = (Double) peakresidual;
     // model flux
     itsSummaryMinor( IPosition(2, 2, shp[1] ) ) = (Double) modelflux;
     // cycle threshold
     itsSummaryMinor( IPosition(2, 3, shp[1] ) ) = itsCycleThreshold;
     // mapper id
     itsSummaryMinor( IPosition(2, 4, shp[1] ) ) = deconvolverid;
     // channel id
     itsSummaryMinor( IPosition(2, 5, shp[1] ) ) = chan;
     // polarity id
     itsSummaryMinor( IPosition(2, 6, shp[1] ) ) = pol;
     // cycle start iterations done (ie earliest iterDone for the entire minor cycle)
     itsSummaryMinor( IPosition(2, 7, shp[1] ) ) = cycleStartIter;
     // starting iterations done
     itsSummaryMinor( IPosition(2, 8, shp[1] ) ) = startIterDone;
     // starting peak residual
     itsSummaryMinor( IPosition(2, 9, shp[1] ) ) = (Double) startpeakresidual;
     // starting model flux
     itsSummaryMinor( IPosition(2, 10, shp[1] ) ) = (Double) startmodelflux;
     // starting peak residual, not limited to the user's mask
     itsSummaryMinor( IPosition(2, 11, shp[1] ) ) = (Double) startpeakresidualnomask;
     // peak residual, not limited to the user's mask
     itsSummaryMinor( IPosition(2, 12, shp[1] ) ) = (Double) peakresidualnomask;
     // number of pixels in the mask
     itsSummaryMinor( IPosition(2, 13, shp[1] ) ) = (Double) masksum;
     // stopcode
     itsSummaryMinor( IPosition(2, 14, shp[1] ) ) = stopCode;
  }// end of addSummaryMinor
  
  
} //# NAMESPACE CASA - END

