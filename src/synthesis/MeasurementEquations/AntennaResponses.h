//# AntennaResponses.h: AntennaResponses provides access to antenna response data
//# Copyright (C) 1995-1999,2000-2004
//# Associated Universities, Inc. Washington DC, USA
//# Copyright by ESO (in the framework of the ALMA collaboration)
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
//# Correspondence concerning CASA should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: CASA Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id: $

#ifndef IMAGES_ANTENNARESPONSES_H
#define IMAGES_ANTENNARESPONSES_H

//# Includes
#include <casacore/casa/aips.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/measures/Measures/MPosition.h>
#include <casacore/measures/Measures/MDirection.h>
#include <casacore/measures/Measures/MEpoch.h>
#include <casacore/measures/Measures/MFrequency.h>
#include <casacore/measures/Measures/MeasFrame.h>
#include <casacore/measures/Measures/MeasRef.h>
#include <casacore/measures/Measures/MeasTable.h>
#include <casacore/images/Images/TempImage.h>
#include <casacore/casa/OS/Time.h>
#include <casacore/casa/Quanta/MVAngle.h>
#include <casacore/casa/System/AipsrcValue.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/ostream.h>

namespace casa { //# NAMESPACE CASA - BEGIN


// <summary>
// AntennaResponses provides access to antenna response data
// </summary>

// <use visibility=local>

// <reviewed reviewer="Ger van Diepen, Max Voronkov, Dirk Petry, Remy Indebetouw" date="Jan 2011" tests="tAntennaResponses" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class=casacore::Measure>Measure</linkto> class 
// </prerequisite>
//
// <etymology>
// 
// </etymology>
//
// <synopsis>
//  The location of the AntennaResponses table for a given observatory is set
//  in the column casacore::String ANTENNA_RESPONSES in the "Observatories" table (accessed
//  via the casacore::MeasTable class).
//
//  The detailed rules of how the location and name of the table is derived
//  from the string found in ANTENNA_RESPONSES is described further down
//  in the comment to the initAntennaResponses() method. 
//
//  E.g., for ALMA, the location of the table would be
//
//          data/alma/AntennaResponses
//
//  and the entry in the Observatories table would be simply "alma/AntennaResponses".
// 
//  "AntennaResponses" is the recommended name of the table but in principle any
//  name can be used. That name will then have to be given in the Observatories table.
//
//  Contents of the AntennaResponses table:
//
//    column 0: casacore::Int BEAM_ID (a unique number in the table for the given observatory name)
//    column 1: casacore::String NAME (name of the observatory as in the observatory table)
//    column 2: casacore::Int BEAM_NUMBER (for observataories which support several simultaneous beams, zero-based)
//    column 3: casacore::Double START_TIME (a casacore::Measure, the time from which onwards this table row is valid)
//    column 4: casacore::String ANTENNA_TYPE (for ALMA: "DV", "DA", "PM", or "CM")
//    column 5: casacore::String RECEIVER_TYPE (for ALMA this will not be filled, at least for the moment)
//    column 6: casacore::Int NUM_SUBBANDS (number of response frequency sub-bands)
//    column 7: casacore::Array casacore::String (size set by NUM_SUBBANDS) BAND_NAME (for ALMA: "1", "2" etc.)
//                            (if there is more than one sub-band per band, the band name repeats)
//    column 8: casacore::Array casacore::Double (a casacore::Quantum, size set by NUM_SUBBANDS) SUBBAND_MIN_FREQ
//    column 9: casacore::Array casacore::Double (a casacore::Quantum, size set by NUM_SUBBANDS) SUBBAND_MAX_FREQ
//    column 10: casacore::MDirection CENTER (the nominal center sky position where this row is valid, default (0.,90.) AZEL)
//    column 11: casacore::MDirection VALID_CENTER_MIN (sky position validity range min values, default (0.,0.) AZEL)
//    column 12: casacore::MDirection VALID_CENTER_MAX (sky position validity range max values, default (360.,90.) AZEL)
//    column 13: casacore::Array casacore::Int (size set by NUM_SUBBANDS) FUNCTION_TYPE
//                      (uses enum FuncTypes defined below:
//                       EFP = complex electric field pattern image,
//                       VP = voltage pattern image,
//                       AIF = complex aperture illumination function image,
//                       NA = the function is not yet available,
//              here other codes can be easily added, e.g.
//              VPMAN = the function is available in casa via the vp manager (details t.b.d.),
//              INTERNAL = the function is generated on the fly internally
//                       using ray-tracing as for the EVLA)
//    column 14: casacore::Array casacore::String (size set by NUM_SUBBANDS) FUNCTION_NAME
//                   (names of the images as paths relative to the directory
//                    where this table is located,
//                    empty string if no image is available for the band, e.g.
//                    "ticra_efield_patterns/melco12m_band6_efp.im")
//    column 15: casacore::Array casacore::uInt (size set by NUM_SUBBANDS) FUNCTION_CHANNEL
//                   (the spectral image channel to use, can be different from 0 in the case
//                    that several antenna responses are stored in one image file)
//    column 16: casacore::Array casacore::Double (a casacore::Quantum, size set by NUM_SUBBANDS) NOMINAL_FREQ
//                    (the nominal frequency of the channel given by FUNCTION_CHANNEL)
//    column 17: casacore::Array casacore::Double (a casacore::Quantum, size set by NUM_SUBBANDS) RESPONSE_ROTATION_OFFSET
//                    (the angle of an additional constant rotation of the response image)
//                     project-dependent implementation) 
//
//  It is assured by the table filling code that columns 10, 11, and 12 use the same casacore::MDirection type.
// </synopsis>
//
// <example>
// 
// </example>
//
// <motivation>
// Information on receiver bands and corresponding primary beams
// (in whatever form) for different antenna types needs to be made
// available to the CASA code in a place where imaging code
// can pick it up.
// </motivation>
//
// <todo asof="2001/01/17">
//   <li>
// </todo>

class AntennaResponses {

public:

  enum FuncTypes{
    NA, // not available
    AIF, // complex aperture illumination function
    EFP, // complex electric field pattern
    VP, // real voltage pattern
    VPMAN, // the function is available in casa via the vp manager (details t.b.d.)
    INTERNAL, // the function is generated on the fly internally
    N_FuncTypes,
    ANY, // used in requests
    INVALID
  };
  // this can of course be extended for additional types if necessary

  // Constructor, does not call init()
  AntennaResponses(){}; 

  // Constructor, calls init()
  AntennaResponses(const casacore::String& path); 

  // Takes the path (as taken from the new ANTENNA_RESPONSES column of the Observatories table)
  // and uses it as the name and location of the AntennaResponses table, and then initializes 
  // the member vectors;
  // can be called repeatedly with different paths;
  // each call will overwrite the vectors.
  // If the path is an empty string, the member vectors will be reset to empty, i.e.
  // this is interpreted to mean that there is no AntennaResponses table on disk.
  // Returns true unless for some reason the initialisation fails (other than path=="").
  casacore::Bool init(const casacore::String& path="");

  // As init but does not overwrite the table (member vectors) in memory.
  // Instead it appends to the vectors.
  // Returns false if the path was already read before.
  casacore::Bool append(const casacore::String& path);

  // returns true if paths_p has at least one member
  casacore::Bool isInit(); 

  // returns true if path is a member element of paths_p, i.e. the contents of path was read
  casacore::Bool isInit(const casacore::String& path); 

  // find the row containing the information pertinent to the given parameters
  // (this is also the index in the member vectors)
  // and the index (subband) to the frequency channel of the response
  // return false if no matching row could be found
  casacore::Bool getRowAndIndex(casacore::uInt& row, casacore::uInt& subBand,
		      const casacore::String& obsName,
		      const casacore::MEpoch& obsTime,
		      const casacore::MFrequency& freq, // if requFType==INTERNAL, a frequency value of 0 (zero) will return the first row and band found
		      const FuncTypes& requFType = ANY, // the requested function type
		      const casacore::String& antennaType = "",
		      const casacore::MDirection& center = casacore::MDirection(casacore::Quantity( 0., "deg"), // the center to be matched with the CENTER column,
							    casacore::Quantity(90., "deg"), // default is the Zenith
							    casacore::MDirection::AZEL), // the center to be matched with the CENTER column
		      const casacore::String& receiverType = "",
		      const casacore::Int& beamNumber = 0);

  // overloaded method: as previous method but using beamId
  // (instead of obs. time, ant. type,  rec. type, center, and beam number)
  casacore::Bool getRowAndIndex(casacore::uInt& row, casacore::uInt& subBand,
		      const casacore::String& obsName,
		      const casacore::Int& beamId,
		      const casacore::MFrequency& freq);

  // getRowAndIndex is then used by the following methods

  // Access methods for the response images: takes an observatory name, 
  // an observation time, the antenna type (e.g. "DV"), the receiverType,
  // a frequency, a function type (enum FuncTypes, can be ANY, i.e. take the first available),
  // a direction, and a beam number and finds the appropriate image in the AntennaResponses table
  // which contains the image of the requested type and returns
  // the path, the channel to use, the frequency of that channel, and the FuncType of the image.
  // The image will contain a spectral axis even if degenerate.
  // The direction axis pair of the stored images is set as follows:
  // The axes are parallel to the ones given by the coordinate system type of the CENTER column
  // of the AntennaReponses table. The center of the image is that given by the CENTER column.
  // Furthermore, the images contain a casacore::Stokes axis (even if degenerate) to express the
  // beams for the different polarizations or polarization products.
  // Returns false if no appropriate image could be found.
  casacore::Bool getImageName(casacore::String& functionImageName, // the path to the image
		    casacore::uInt& funcChannel, // the channel to use in the image  
		    casacore::MFrequency& nomFreq, // nominal frequency of the image (in the given channel)
		    FuncTypes& fType, // the function type of the image
		    casacore::MVAngle& rotAngOffset, // the response rotation angle offset
		    const casacore::String& obsName, // (the observatory name, e.g. "ALMA" or "ACA")
		    const casacore::MEpoch& obsTime,
		    const casacore::MFrequency& freq,
		    const FuncTypes& requFType = ANY, // the requested function type
		    const casacore::String& antennaType = "",
		    const casacore::MDirection& center = casacore::MDirection(casacore::Quantity( 0., "deg"), // the center to be matched with the CENTER column,
							  casacore::Quantity(90., "deg"), // default is the Zenith
							  casacore::MDirection::AZEL), 
		    const casacore::String& receiverType = "",
		    const casacore::Int& beamNumber=0);   

  // Overloaded method: returning the validity range
  casacore::Bool getImageName(casacore::String& functionImageName, // the path to the image
		    casacore::uInt& funcChannel, // the channel to use in the image  
		    casacore::MFrequency& nomFreq, // nominal frequency of the image (in the given channel)
		    casacore::MFrequency& loFreq, // lower end of the frequency validity range
		    casacore::MFrequency& hiFreq, // upper end of the frequency validity range
		    FuncTypes& fType, // the function type of the image
		    casacore::MVAngle& rotAngOffset, // the response rotation angle offset
		    const casacore::String& obsName, // (the observatory name, e.g. "ALMA" or "ACA")
		    const casacore::MEpoch& obsTime,
		    const casacore::MFrequency& freq,
		    const FuncTypes& requFType = ANY, // the requested function type
		    const casacore::String& antennaType = "",
		    const casacore::MDirection& center = casacore::MDirection(casacore::Quantity( 0., "deg"), // the center to be matched with the CENTER column,
							  casacore::Quantity(90., "deg"), // default is the Zenith
							  casacore::MDirection::AZEL), 
		    const casacore::String& receiverType = "",
		    const casacore::Int& beamNumber=0);
		
  // Overloaded method: as previous method but using beamId
  // (instead of obs. time, ant. type,  rec. type, center, and functype)
  casacore::Bool getImageName(casacore::String& functionImageName,
		    casacore::uInt& funcChannel, // the channel to use in the image  
		    casacore::MFrequency& nomFreq, // nominal frequency of the image (at the given channel) 
		    FuncTypes& fType, // the function type of the image
		    casacore::MVAngle& rotAngOffset, // the response rotation angle offset
		    const casacore::String& obsName, // (the observatory name, e.g. "ALMA" or "ACA")
		    const casacore::Int& beamId,
		    const casacore::MFrequency& freq);


  // Get a vector containing all unique antenna type strings for the given constraints
  casacore::Bool getAntennaTypes(casacore::Vector<casacore::String>& antTypes,
		       const casacore::String& obsName, // (the observatory name, e.g. "ALMA" or "ACA")
		       const casacore::MEpoch& obsTime,
		       const casacore::MFrequency& freq,
		       const FuncTypes& requFType = ANY, // the requested function type
		       const casacore::MDirection& center = casacore::MDirection(casacore::Quantity( 0., "deg"), // the center to be matched with the CENTER column,
							     casacore::Quantity(90., "deg"), // default is the Zenith
							     casacore::MDirection::AZEL), 
		       const casacore::String& receiverType = "",
		       const casacore::Int& beamNumber=0);


  // Put the given row into the present antenna reponses table (in memory).
  // If the row exists at the position given by casacore::uInt row, it is overwritten.
  // If it doesn't exist, the table is resized by one in memory and the new
  // row is added at the last position. The variable "row" then contains the
  // actual row that was filled.
  // Returns false, if the table was not initialised or the given data was
  // not consistent.
  // Consistency checks: 
  //   - all vectors have same dimension which is then used to set numSubbands
  //   - beamId is unique for the given observatory
  //   - center, validCenterMin, and validCenterMax have the same casacore::MDirection type
  casacore::Bool putRow(casacore::uInt& row,
	      const casacore::String& obsName,
	      const casacore::Int& beamId,
	      const casacore::Vector<casacore::String>& bandName,
	      const casacore::Vector<casacore::MVFrequency>& subbandMinFreq,
	      const casacore::Vector<casacore::MVFrequency>& subbandMaxFreq,
	      const casacore::Vector<FuncTypes>& funcType,
	      const casacore::Vector<casacore::String>& funcName,
	      const casacore::Vector<casacore::uInt>& funcChannel, 
	      const casacore::Vector<casacore::MVFrequency>& nomFreq,
	      const casacore::Vector<casacore::MVAngle>& rotAngOffset, 
	      const casacore::String& antennaType = "",
	      const casacore::MEpoch& startTime = casacore::MEpoch(casacore::MVEpoch(casacore::Quantity(40588., "d")), casacore::MEpoch::UTC), // beginning of 1970
	      const casacore::MDirection& center = casacore::MDirection(casacore::Quantity( 0., "deg"), // the center to be matched with the CENTER column,
						    casacore::Quantity(90., "deg"), // default is the Zenith
						    casacore::MDirection::AZEL), 
	      const casacore::MDirection& validCenterMin = casacore::MDirection(casacore::Quantity( 0., "deg"), 
							    casacore::Quantity( 0., "deg"), 
							    casacore::MDirection::AZEL),  
	      const casacore::MDirection& validCenterMax = casacore::MDirection(casacore::Quantity( 360., "deg"),
							    casacore::Quantity( 90., "deg"),
							    casacore::MDirection::AZEL),  
	      const casacore::String& receiverType = "",
	      const casacore::Int& beamNumber = 0);

  // Create an new AntennaReponses table on disk at the given path
  // and fill it with the table contents presently in memory.
  // Throw exceptions if there are problems writing the table. 
  void create(const casacore::String& path);

  // Convert from casacore::Int to FuncType
  FuncTypes FuncType(casacore::Int i);

  // Convert from casacore::String to FuncType
  static FuncTypes FuncType(const casacore::String& sftyp);

  // get the name of the band corresponding to the frequency (in the rest frame of the observatory)
  casacore::Bool getBandName(casacore::String& bandName,
		   const casacore::String& obsName, 
		   const casacore::MVFrequency& freq);

private:

  // after initialization, this contains the name of the path where the 
  // AntennaResponses table was read from;
  // if append was used to read additional tables in the memory table,
  // then this vector has several elements representing the different tables
  casacore::Vector<casacore::String> paths_p; 

  // here a complete copy of the AntennaResponses table is stored
  casacore::uInt numRows_p;
  casacore::Vector<casacore::String> ObsName_p;
  casacore::Vector<casacore::MEpoch> StartTime_p;
  casacore::Vector<casacore::String> AntennaType_p;
  casacore::Vector<casacore::String> ReceiverType_p;
  casacore::Vector<casacore::Int> BeamId_p;
  casacore::Vector<casacore::Int> BeamNumber_p;
  casacore::Vector<casacore::MDirection> ValidCenter_p;
  casacore::Vector<casacore::MDirection> ValidCenterMin_p;
  casacore::Vector<casacore::MDirection> ValidCenterMax_p;
  casacore::Vector<casacore::uInt> NumSubbands_p;
  casacore::Vector<casacore::Vector<casacore::String> > BandName_p;
  casacore::Vector<casacore::Vector<casacore::MVFrequency> > SubbandMinFreq_p;
  casacore::Vector<casacore::Vector<casacore::MVFrequency> > SubbandMaxFreq_p;
  casacore::Vector<casacore::Vector<FuncTypes> > FuncType_p;
  casacore::Vector<casacore::Vector<casacore::String> > FuncName_p;
  casacore::Vector<casacore::Vector<casacore::uInt> > FuncChannel_p;
  casacore::Vector<casacore::Vector<casacore::MVFrequency> > NomFreq_p;
  casacore::Vector<casacore::Vector<casacore::MVAngle> > RotAngOffset_p;

  // not part of the table but same number of elements:
  // memory of the path from which the row was read (index to paths_p)
  casacore::Vector<casacore::uInt> pathIndex_p; 

};


} //# NAMESPACE CASA - END

#endif
