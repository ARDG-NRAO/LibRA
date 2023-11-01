//# MSMetaInfoForCal.h: MS Meta info services in Calibraiton context
//# Copyright (C) 2016
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

#ifndef SYNTHESIS_MSMETAINFOFORCAL_H
#define SYNTHESIS_MSMETAINFOFORCAL_H

#include <casacore/casa/aips.h>
#include <iostream>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <msvis/MSVis/SimpleSimVi2.h>

namespace casacore {

class MSMetaData;

}

namespace casa { //# NAMESPACE CASA - BEGIN

class MSMetaInfoForCal {
  
public:

  // Construct from the filename of an MS
  //  (If the MS doesn't exist, services will be trivialized)
  MSMetaInfoForCal(casacore::String msname);

  // Construct from a supplied MS object
  MSMetaInfoForCal(const casacore::MeasurementSet& ms);

  // Construct from nAnt,nSpw
  MSMetaInfoForCal(casacore::uInt nAnt,casacore::uInt nSpw,casacore::uInt nFld);

  // Construct from SimpleSimVi2Paremeters
  //   (useful for testing w/ actual (spoofed) data iteration
  MSMetaInfoForCal(const vi::SimpleSimVi2Parameters& sspar);

  // Dtor
  ~MSMetaInfoForCal();

  // The MS name
  casacore::String msname() const { return msname_; };

  // Signal if a real MS was specified in ctor
  casacore::Bool msOk() const { return msOk_; };

  // Return access to MSMetaData object (if avail)
  //  Would like this to be const, but many methods in MSMetaData aren't....
  casacore::MSMetaData& msmd() const;

  // Return accss to SimpleSimVi2Parameters object (if avail)
  const vi::SimpleSimVi2Parameters& ssp() const;

  // The number of antennas
  casacore::uInt nAnt() const { return nAnt_;};

  // Antenna name, by index
  casacore::String antennaName(casacore::uInt iant) const;

  // All antenna names, as vector
  void antennaNames(casacore::Vector<casacore::String>& antnames) const;

  // The number of spws
  casacore::uInt nSpw() const { return nSpw_;};

  // Spw name, by index
  casacore::String spwName(casacore::uInt ispw) const;

  // The number of fields
  casacore::uInt nFld() const { return nFld_;};

  // Field name, by index
  casacore::String fieldName(casacore::uInt ifld) const;

  // All field names, as vector
  void fieldNames(casacore::Vector<casacore::String>& fldnames) const;

  // Field id at time
  casacore::Int fieldIdAtTime(casacore::Double time) const;

  // Scan number at time
  casacore::Int scanNumberAtTime(casacore::Double time) const;

  // Center freq for specified spw
  casacore::Double centerFreq(casacore::uInt ispw) const;
  

private:

  // The supplied MS name
  casacore::String msname_;

  // Is the MS available (else spoof)
  bool msOk_;

  // Remember basic shapes
  casacore::uInt nAnt_, nSpw_, nFld_;

  // Spw absolute center frequencies
  //  (this gets filled OTF, if necessary)
  mutable casacore::Vector<casacore::Double> centerFreqs_;

  // MS pointer
  casacore::MeasurementSet *ms_;

  // MSMetaData pointer
  casacore::MSMetaData *msmd_;

  // SimpleSimVi2Parameters pointer
  vi::SimpleSimVi2Parameters* ssp_;


};


} //# NAMESPACE CASA - END

#endif


