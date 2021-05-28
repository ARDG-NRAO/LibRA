//# VisBufferUtil.h: Definitions for VisBuffer utilities
//# Copyright (C) 1996-2014
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

#ifndef MSVIS_VISBUFFERUTIL_H
#define MSVIS_VISBUFFERUTIL_H

#include <casa/aips.h>
#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MPosition.h>
#include <measures/Measures/MFrequency.h>
#include <ms/MeasurementSets/MeasurementSet.h>
#include <scimath/Mathematics/InterpolateArray1D.h>
#include <unordered_map>
namespace casa { //# NAMESPACE CASA - BEGIN
//forward 
class VisBuffer;
// <summary> 
// </summary>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// </prerequisite>
//
// <etymology>
// </etymology>
//
// <synopsis> 
// </synopsis> 
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// </motivation>
//
// <todo asof="">
// </todo>
class VisBufferUtil{

public:

  //Empty Constructor
  VisBufferUtil();

  // Construct from a VisBuffer (sets a casacore::MeasFrame)
  VisBufferUtil(const VisBuffer& vb);

  // Same as above but with VisBuffer2;
  // It has to be attached to a VisIter2 as the frame is determined that way
  // otherwise use one of the constructors below or the empty constructor above
  VisBufferUtil(const vi::VisBuffer2& vb);
  // Construct with the vi2 to get access to the ms to define frame

  VisBufferUtil(const vi::VisibilityIterator2& iter);

  VisBufferUtil(const casacore::MeasFrame& mframe);

  // Make PSF VisBuffer
  void makePSFVisBuffer(VisBuffer& vb);
  

  //Regrid the data on a new frequency grid (defined by outFreqGrid) , on the frequency 
  //frame (defined by freqFrame). It will pass back the interpolated data and flags 
 
  casacore::Bool interpolateFrequency(casacore::Cube<casacore::Complex>& data, 
			    casacore::Cube<casacore::Bool>& flags, 
			    const VisBuffer& vb,
			    const casacore::Vector<casacore::Float>& outFreqGrid, const casacore::MS::PredefinedColumns whichCol=casacore::MS::DATA,
			    const casacore::MFrequency::Types freqFrame=casacore::MFrequency::LSRK,
			    const casacore::InterpolateArray1D< casacore::Float, casacore::Complex>::InterpolationMethod interp = 
			    (casacore::InterpolateArray1D<casacore::Float,casacore::Complex>::nearestNeighbour));

  // Converts the frequency in this visbuffer to the frame requested
  void convertFrequency(casacore::Vector<casacore::Double>& outFreq, 
			const VisBuffer& vb, 
			const casacore::MFrequency::Types freqFrame);
  //This one is just to test VisBuffer2 internal conversions
  void convertFrequency(casacore::Vector<casacore::Double>& outFreq, 
			const vi::VisBuffer2& vb, 
			const casacore::MFrequency::Types freqFrame);
  //This is a helper function as vi2 does not have a get Freq range for the data selected
  static void getFreqRange(casacore::Double& freqMin, casacore::Double& freqMax,  vi::VisibilityIterator2& vi, casacore::MFrequency::Types freqFrame);

  //Get the freqRange from in Frame to range it represents in outFrame for 
  //all the fields and time selected in the vi
  //Returns False if failed to do any conversion.
  static casacore::Bool getFreqRangeFromRange(casacore::Double& outfreqMin, casacore::Double& outfreqMax,  const casacore::MFrequency::Types inFreqFrame, const casacore::Double infreqMin, const casacore::Double infreqMax, vi::VisibilityIterator2& vi, casacore::MFrequency::Types outFreqFrame);

  // Converts the frequency in this VisBuffer to velocity in the frame/def requested
  void toVelocity(casacore::Vector<casacore::Double>& outVel, 
		  const VisBuffer& vb, 
		  const casacore::MFrequency::Types freqFrame,
		  const casacore::MVFrequency restFreq,
		  const casacore::MDoppler::Types veldef);

  // Converts the frequencies on given row of VisBuffer2 to velocity in the frame/def requested
  void toVelocity(casacore::Vector<casacore::Double>& outVel,
  		  const vi::VisBuffer2& vb,
  		  const casacore::MFrequency::Types freqFrame,
  		  const casacore::MVFrequency restFreq,
  		  const casacore::MDoppler::Types veldef,
  		  const casacore::Int row=0);

  /// same as above but can be a detached Visbuffer ...the iterator is used explicitly
  /// to get some info like direction from field table and frame from spectral window table
  void toVelocity(casacore::Vector<casacore::Double>& outVel,
    		  const vi::VisBuffer2& vb,
    		  const vi::VisibilityIterator2& iter,
    		  const casacore::MFrequency::Types freqFrame,
    		  const casacore::MVFrequency restFreq,
    		  const casacore::MDoppler::Types veldef,
    		  const casacore::Int row=0);


  ////get the velocity values for the frequencies passed in
  /// for  the epoch and direction.
  /// If rest frequency is -ve then the middle channel is used as the rest value
  void toVelocity(casacore::Vector<casacore::Double>& outVel,
  		  const casacore::MFrequency::Types outfreqFrame,
  		  const casacore::Vector<casacore::Double>& inFreq,
  		  const casacore::MFrequency::Types inFreqFrame,
  		  const casacore::MEpoch& ep,
  		  const casacore::MDirection& dir,
  		  const casacore::MVFrequency restFreq,
  		  const casacore::MDoppler::Types veldef);

  //Rotate the uvw in the vb along with giving the phase needed to convert the visibilities to a new phasecenter
  // will return a false if it is a NoOP...don't need then to waste time
  //applying the phasor etc 
   casacore::Bool rotateUVW(const vi::VisBuffer2&vb, const casacore::MDirection& desiredDir,
		  casacore::Matrix<casacore::Double>& uvw, casacore::Vector<casacore::Double>& dphase);

   /// get the pointing direction for a given integration and antenna id
   /// will cache it for large pointing table specially so that it can be reused pronto
   casacore::MDirection getPointingDir(const VisBuffer& vb, const casacore::Int antid, const casacore::Int row);
   casacore::MDirection getPointingDir(const vi::VisBuffer2& vb, const casacore::Int antid, const casacore::Int row, const casacore::MDirection::Types dirframe=casacore::MDirection::AZELGEO, const casacore::Bool usePointing=true);
   //get the phaseCenter for a given time   ..cached so that it does not need to do small read every time of access
   // time -ve means   use the first time in the vb
    casacore::MDirection getPhaseCenter(const vi::VisBuffer2& vb, const casacore::Double time=-1.0);
    ///Get the ephemeris direction of a source fieldid of vb at time given 
    //or the first time in the visBuffer 
    casacore::MDirection getEphemDir(const vi::VisBuffer2& vb, const casacore::Double time=-1.0);
	
 private:
  void swapyz(casacore::Cube<casacore::Bool>& out, const casacore::Cube<casacore::Bool>& in);
  void swapyz(casacore::Cube<casacore::Complex>& out, const casacore::Cube<casacore::Complex>& in);
  void rejectConsecutive(const casacore::Vector<casacore::Double>& t, casacore::Vector<casacore::Double>& retval);
  void rejectConsecutive(const casacore::Vector<casacore::Double>& t, casacore::Vector<casacore::Double>& retval, casacore::Vector<casacore::Int>& indx);
  //A quicker pointing index finder
  //void pointingIndex(const casacore::MSPointing& pcols, const casacore::Int antenna, const casacore::Int ntimes,  casacore::Double*& ptime, casacore::Vector<casacore::Int>& indices,casacore::Vector<casacore::MDirection>& direction );
  void pointingIndex( casacore::Double*& timecol, casacore::Int*& antcol, casacore::Double*& intervalcol, const casacore::rownr_t nrow, const casacore::Int antenna, const casacore::Int ntimes,  casacore::Double*& ptime, casacore::Vector<ssize_t>& indices );
  //get ephemeris based direction corrected for parallax for the position in the frame
  casacore::MDirection getEphemBasedPhaseDir(const vi::VisBuffer2& vb, const casacore::String& ephemPath, const casacore::MDirection&refDir,  const casacore::Double t);
  // A casacore::MeasFrame for conversions
  casacore::MeasFrame mframe_;
  casacore::Int oldMSId_p;
  casacore::Int oldPCMSId_p;
  casacore::Vector<std::map<std::pair<double,int>, int> > timeAntIndex_p;
  casacore::Vector<casacore::Vector<casacore::MDirection> > cachedPointingDir_p;
  casacore::Vector<std::map<casacore::Double, casacore::MDirection> > cachedPhaseCenter_p;
};

} //# NAMESPACE CASA - END

#endif

