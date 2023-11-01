//# MSUtil.h: Definitions for casacore::MS utilities
//# Copyright (C) 2011
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

#ifndef MSVIS_MSUTIL_H
#define MSVIS_MSUTIL_H
#include <casacore/casa/aips.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <casacore/measures/Measures/MDirection.h>
namespace casa { //# NAMESPACE CASA - BEGIN

  class MSUtil{

  public:
    //Empty Constructor if needed
    MSUtil();
    //This method gives you the channels (and spws) that will cover the 
    //freq range provided in the frame the user stated. 
    //The conversion from the data frame will be done properly for finding this
    //spw, start, nchan will be resized appropriately
    static void getSpwInFreqRange(casacore::Vector<casacore::Int>& spw, casacore::Vector<casacore::Int>& start,
				  casacore::Vector<casacore::Int>& nchan,
				  const casacore::MeasurementSet& ms, 
				  const casacore::Double freqStart,
				  const casacore::Double freqEnd,
				  const casacore::Double freqStep,
				  const casacore::MFrequency::Types freqframe=casacore::MFrequency::LSRK, 
				  const casacore::Int fieldId=0);
    
    //A version of the above for all the fields in the MS
    static void getSpwInFreqRangeAllFields(casacore::Vector<casacore::Int>& spw, casacore::Vector<casacore::Int>& start,
			  casacore::Vector<casacore::Int>& nchan,
			  const casacore::MeasurementSet& ms,
			  const casacore::Double freqStart,
			  const casacore::Double freqEnd,
			  const casacore::Double freqStep,
			  const casacore::MFrequency::Types freqframe=casacore::MFrequency::LSRK);
	//Get the channel range in a spw  and a given frequency range in given frame
     static void getChannelRangeFromFreqRange(casacore::Int& start,
				  casacore::Int& nchan,
				  const casacore::MeasurementSet& ms,
				  const casacore::Int spw,
				  const casacore::Double freqStart,
				  const casacore::Double freqEnd,
				  const casacore::Double freqStep,
			    const casacore::MFrequency::Types freqframe= casacore::MFrequency::LSRK);
     //Get SPW selection in MS that match freqs  in SOURCE frame for a moving source defined by ephemeris table or TRACKFIELD or DE-200 source name
     static void getSpwInSourceFreqRange(
		 casacore::Vector<casacore::Int>& spw,
		 casacore::Vector<casacore::Int>& start,
		 casacore::Vector<casacore::Int>& nchan,
		 const casacore::MeasurementSet& ms, 
		 const casacore::Double freqStart,
		 const casacore::Double freqEnd,
		 const casacore::Double freqStep,
		 const casacore::String& ephemtabOrSourceName,
		 const casacore::Int fieldId);
     
    // The following wil provide the range of frequency convered in the frame requested by the spw, channel selection 
 
     static casacore::Bool getFreqRangeInSpw( casacore::Double& freqStart,
			      casacore::Double& freqEnd, 
			      const casacore::Vector<casacore::Int>& spw, 
			      const casacore::Vector<casacore::Int>& start,
			      const casacore::Vector<casacore::Int>& nchan,
			      const casacore::MeasurementSet& ms, 
			      const casacore::MFrequency::Types freqframe=casacore::MFrequency::LSRK,
				   const casacore::Int fieldId=0, const casacore::Bool edge=true );
     ///This version does not use a fieldid but uses the ones in the ms
     static casacore::Bool getFreqRangeInSpw( casacore::Double& freqStart,
			      casacore::Double& freqEnd, 
			      const casacore::Vector<casacore::Int>& spw, 
			      const casacore::Vector<casacore::Int>& start,
			      const casacore::Vector<casacore::Int>& nchan,
			      const casacore::MeasurementSet& ms, 
			      const casacore::MFrequency::Types freqframe=casacore::MFrequency::LSRK,
			       const casacore::Bool edge=true );
     
     //if useFieldInMS=True fieldids are ignored
     static casacore::Bool getFreqRangeInSpw( casacore::Double& freqStart,
			      casacore::Double& freqEnd, 
			      const casacore::Vector<casacore::Int>& spw, 
			      const casacore::Vector<casacore::Int>& start,
			      const casacore::Vector<casacore::Int>& nchan,
			      const casacore::MeasurementSet& ms, 
			      const casacore::MFrequency::Types freqframe,
					      const casacore::Vector<casacore::Int>& fieldId, const casacore::Bool edge=true, const casacore::Bool useFieldsInMS=false );
     
     //This version gets the range of frequency in SOURCE frame for an ephem source
     //either trackDIr should be a known planet or ephemPath should not be an
     //empty string. Also return a systemic velocity w.r.t TOPO  at the closest time in
     // the ms of the reference epoch provided
     static  casacore::Bool getFreqRangeAndRefFreqShift( casacore::Double& freqStart,
							 casacore::Double& freqEnd,
							 casacore::Quantity& sysvel,
							 const casacore::MEpoch& refEp,
							 const casacore::Vector<casacore::Int>& spw,
							 const casacore::Vector<casacore::Int>& start,
							 const casacore::Vector<casacore::Int>& nchan,
							 const casacore::MeasurementSet& ms, 
						      
						       const casacore::String& ephemPath=casacore::String(""),
						       const casacore::MDirection& trackDir=casacore::MDirection(casacore::MDirection::APP),
						       const casacore::Bool fromEdge=true);
     
    //Return all the selected SPW types selected in the selected casacore::MS if the input ms
    //is a reference MS. Else it will return all the types in the  SPW table
    static casacore::Vector<casacore::String> getSpectralFrames(casacore::Vector<casacore::MFrequency::Types>& types, const casacore::MeasurementSet& ms);

    static void getIndexCombination(const casacore::MSColumns& mscol, casacore::Matrix<casacore::Int>& retval);
  private:
    static void rejectConsecutive(const casacore::Vector<casacore::Double>& t, casacore::Vector<casacore::Double>& retval, casacore::Vector<casacore::Int>& indx);
    static void setupSourceObsVelSystem(const casacore::String& ephemTable, const casacore::MeasurementSet& ms,   const casacore::Int& fieldid, casacore::MDoppler& toSource, casacore::MDoppler& toObs, casacore::MeasFrame& mFrame);
    
  };
} //# NAMESPACE CASA - END
#endif
