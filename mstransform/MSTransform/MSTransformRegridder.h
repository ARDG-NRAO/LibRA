//# MSTransformRegridder.h: This file contains the interface definition of the MSTransformRegridder class.
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

#ifndef MSTransformRegridder_H_
#define MSTransformRegridder_H_

// Logging
#include <casa/Logging/LogIO.h>

// casacore::Coordinate systems
#include <measures/Measures/MeasTable.h>

// casacore::MS handling
#include <ms/MeasurementSets.h>

// To use ostringstream
#include <iomanip>

// Enable record conversions
#include <casa/Quanta/QuantumHolder.h>


namespace casa { //# NAMESPACE CASA - BEGIN

//  MSTransformRegridder definition
class MSTransformRegridder
{

public:

	MSTransformRegridder();
	~MSTransformRegridder();

	// Make one spectral window from all SPWs given by the SPW Ids vector
	static casacore::Bool combineSpws(	casacore::LogIO& os,
								casacore::String msName,
								const casacore::Vector<casacore::Int>& spwids, // casacore::Vector<casacore::Int>(1,-1) means: use all SPWs
								casacore::Vector<casacore::Double>& newCHAN_FREQ, // Will return the grid of the resulting SPW
								casacore::Vector<casacore::Double>& newCHAN_WIDTH,
								std::vector<std::vector<casacore::Int> >& averageWhichChan,
								std::vector<std::vector<casacore::Int> >& averageWhichSPW,
								std::vector<std::vector<casacore::Double> >& averageChanFrac,
								casacore::Bool verbose = false);

	// Make one spectral window from all SPWs given by the SPW Ids vector
	static casacore::Bool combineSpwsCore(	casacore::LogIO& os,
									casacore::MeasurementSet& ms_p,
									const casacore::Vector<casacore::Int>& spwids, // casacore::Vector<casacore::Int>(1,-1) means: use all SPWs
									casacore::Vector<casacore::Double>& newCHAN_FREQ, // Will return the grid of the resulting SPW
									casacore::Vector<casacore::Double>& newCHAN_WIDTH,
									std::vector<std::vector<casacore::Int> >& averageWhichChan,
									std::vector<std::vector<casacore::Int> >& averageWhichSPW,
									std::vector<std::vector<casacore::Double> >& averageChanFrac,
									casacore::Bool verbose = false);

	// A wrapper for regridChanBounds() which takes the user interface type re-gridding parameters
	// The ready-made grid is returned in newCHAN_FREQ and newCHAN_WIDTH
	static casacore::Bool calcChanFreqs(	casacore::LogIO& os,
								// Output
								casacore::Vector<casacore::Double>& newCHAN_FREQ,
								casacore::Vector<casacore::Double>& newCHAN_WIDTH,
								casacore::Double& weightScale,
								// casacore::Input (original grid)
								const casacore::Vector<casacore::Double>& oldCHAN_FREQ,
								const casacore::Vector<casacore::Double>& oldCHAN_WIDTH,
								// Re-gridding parameters
								const casacore::MDirection  phaseCenter,
								const casacore::MFrequency::Types theOldRefFrame,
								const casacore::MEpoch theObsTime,
								const casacore::MPosition mObsPos,
								const casacore::String& mode,
								const int nchan,
								const casacore::String& start,
								const casacore::String& width,
								const casacore::String& restfreq,
								const casacore::String& outframe,
								const casacore::String& veltype,
								const casacore::Bool verbose=false,
								// Additional radial velocity shift to apply, used e.g. when outframe=="SOURCE"
								const casacore::MRadialVelocity mRV=casacore::MRadialVelocity() );

	  // Helper function for handling the re-gridding parameter user input
	  static casacore::Bool convertGridPars(casacore::LogIO& os,
				      	  	  	  const casacore::String& mode,
				      	  	  	  const int nchan,
				      	  	  	  const casacore::String& start,
				      	  	  	  const casacore::String& width,
				      	  	  	  const casacore::String& interp,
				      	  	  	  const casacore::String& restfreq,
				      	  	  	  const casacore::String& outframe,
				      	  	  	  const casacore::String& veltype,
				      	  	  	  casacore::String& t_mode,
				      	  	  	  casacore::String& t_outframe,
				      	  	  	  casacore::String& t_regridQuantity,
				      	  	  	  casacore::Double& t_restfreq,
				      	  	  	  casacore::String& t_regridInterpMeth,
				      	  	  	  casacore::Double& t_cstart,
				      	  	  	  casacore::Double& t_bandwidth,
				      	  	  	  casacore::Double& t_cwidth,
				      	  	  	  casacore::Bool& t_centerIsStart,
				      	  	  	  casacore::Bool& t_startIsEnd,
				      	  	  	  casacore::Int& t_nchan,
				      	  	  	  casacore::Int& t_width,
				      	  	  	  casacore::Int& t_start);

	  // Calculate the final new channel boundaries from the re-regridding parameters and
	  // the old channel boundaries (already transformed to the desired reference frame).
	  // Returns false if input parameters were invalid and no useful boundaries could be created
	  static casacore::Bool regridChanBounds(casacore::Vector<casacore::Double>& newChanLoBound,
				       	   	   	   casacore::Vector<casacore::Double>& newChanHiBound,
				       	   	   	   const casacore::Double regridCenter,
				       	   	   	   const casacore::Double regridBandwidth,
				       	   	   	   const casacore::Double regridChanWidth,
				       	   	   	   const casacore::Double regridVeloRestfrq,
				       	   	   	   const casacore::String regridQuant,
				       	   	   	   const casacore::Vector<casacore::Double>& transNewXin,
				       	   	   	   const casacore::Vector<casacore::Double>& transCHAN_WIDTH,
				       	   	   	   casacore::String& message, // message to the user, especially in case of error
				       	   	   	   const casacore::Bool centerIsStart=false, // if true, the parameter regridCenter specifies the start
				       	   	   	   const casacore::Bool startIsEnd=false, // if true, and centerIsStart is true, regridCenter specifies the upper end in frequency
				       	   	   	   const casacore::Int nchan=0, // if != 0 : used instead of regridBandwidth, -1 means use all channels
				       	   	   	   const casacore::Int width=0, // if >0 and regridQuant=="freq": used instead of regridChanWidth
				       	   	   	   const casacore::Int start=-1); // if >=0 and regridQuant=="freq": used instead of regridCenter

	  // The following inline convenience methods for regridSpw bypass the whole CASA measure system
	  // because when they are used, they can assume that the frame stays the same and the units are OK
	  static casacore::lDouble vrad(const casacore::lDouble freq, const casacore::lDouble rest){ return (casacore::C::c * (1. - freq/rest)); };
	  static casacore::lDouble vopt(const casacore::lDouble freq, const casacore::lDouble rest){ return (casacore::C::c *(rest/freq - 1.)); };
	  static casacore::lDouble lambda(const casacore::lDouble freq){ return (casacore::C::c/freq); };
	  static casacore::lDouble freq_from_vrad(const casacore::lDouble vrad, const casacore::lDouble rest){ return (rest * (1. - vrad/casacore::C::c)); };
	  static casacore::lDouble freq_from_vopt(const casacore::lDouble vopt, const casacore::lDouble rest){ return (rest / (1. + vopt/casacore::C::c)); };
	  static casacore::lDouble freq_from_lambda(const casacore::lDouble lambda){ return (casacore::C::c/lambda); };
};

} //# NAMESPACE CASA - END

#endif /* MSTransformRegridder_H_ */
