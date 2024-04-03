//# CTColumns.h: provides easy access to (new) CalTable columns
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
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$

#ifndef CALIBRATION_NEWCALTABLECOLUMNS_H
#define CALIBRATION_NEWCALTABLECOLUMNS_H

#include <casacore/casa/aips.h>
#include <casacore/measures/Measures/MDirection.h>
#include <casacore/measures/Measures/MEpoch.h>
#include <synthesis/CalTables/NewCalTable.h>
#include <synthesis/CalTables/CTMainColumns.h>
#include <casacore/ms/MeasurementSets/MSAntennaColumns.h>
#include <casacore/ms/MeasurementSets/MSFieldColumns.h>
#include <casacore/ms/MeasurementSets/MSHistoryColumns.h>
#include <casacore/ms/MeasurementSets/MSSpWindowColumns.h>

namespace casa { //# NAMESPACE CASA - BEGIN

class NewCalTable;
// <summary>
// A class to provide easy read-only access to new CalTable columns
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> NewCalTable
//   <li> ArrayColumn
//   <li> ScalarColumn
// </prerequisite>
//
// <etymology>
// ROCTColumns stands for Read-Only NewCalTable casacore::Table columns.
// </etymology>
//
// <synopsis>
// This class provides read-only access to the columns in the NewCalTable, similar
// to the casacore::MSColumns for MeasurementSet.
// It does the declaration of all the Scalar and ArrayColumns with the
// correct types, so the application programmer doesn't have to
// worry about getting those right. There is an access function
// for every predefined column. Access to non-predefined columns will still
// have to be done with explicit declarations.
// </synopsis>
//
// <example>
// <srcblock>
// // use as follows
// NewCalTable newcalt("mynewcalt");
// RONCTColumns nctc(newcalt);
// // show time from row 5
// cout << nctc.time()(5);
// // show name of antenna on row 3 in antenna table
// cout << nctc.antenna().name();
// </srcblock>
// </example>
//
// <motivation>
// See <linkto class=CTColumns> CTColumns</linkto> for the motivation.
// </motivation>
//
// <todo asof="">
// </todo>

class ROCTColumns: public ROCTMainColumns
{
public:
  // Create a columns object that accesses the data in the specified MS
  ROCTColumns(const NewCalTable& caltable);

  // The destructor does nothing special
  ~ROCTColumns();

  // Access to required subtables
  // <group>
  const ROCTObservationColumns& observation() const {return observation_p;}
  const ROCTAntennaColumns& antenna() const {return antenna_p;}
  const ROCTFieldColumns& field() const {return field_p;}
  const ROCTHistoryColumns& history() const {return history_p;}
  const ROCTSpWindowColumns& spectralWindow() const {
    return spectralWindow_p;}
  // </group>

  // Access to optional subtables
  // <group>
  // </group>

protected:
  friend class NewCalTable; 
private:
  // Access to subtables
  ROCTObservationColumns observation_p;
  ROCTAntennaColumns antenna_p;
  ROCTFieldColumns field_p;
  ROCTHistoryColumns history_p;
  ROCTSpWindowColumns spectralWindow_p;

};

// <summary>
// A class to provide easy read-write access to NewCalTable columns
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> NewCalTable
//   <li> ArrayColumn
//   <li> ScalarColumn
// </prerequisite>
//
// <etymology>
// CTColumns stands for  NewCalTable columns.
// </etymology>
//
// <synopsis>
// This class provides access to all the subtables and direct access to all the
// columns in the NewCalTable.  It does the declaration of all the Scalar
// and ArrayColumns with the correct types, so the application programmer
// doesn't have to worry about getting those right. There is an access function
// for every predefined column. Access to non-predefined columns will still
// have to be done with explicit declarations.
// </synopsis>
//
// <example>
// <srcblock>
// // use as follows
// NewCalTable newcalt("mynewcalt",casacore::Table::Update);
// CTColumns nctc(newcalt);
// // show time from row 5
// cout << nctc.time()(5);
// // change name of antenna on row 3 in antenna table
// nctc.antenna().name().put(3,"NewAnt-3");
// </srcblock>
// </example>
//
// <motivation>
// Having to type long lists of Scalar and casacore::Array column declarations gets
// very tedious. This class attempts to relieve some of that tedium, while
// at the same time concentrating all the declarations in one place,
// making Type errors in the column declaration (only caught at run-time) less
// probable. Type errors in the use of the columns is caught at compile
// time.
// </motivation>
//
// <todo asof="">
// </todo>

class CTColumns: public CTMainColumns
{   
public:
  // Create a columns object that accesses the data in the specified caltable 
  CTColumns(NewCalTable& caltable);

  // The destructor does nothing special
  ~CTColumns();

  // Read-write access to required subtables
  // <group>
  CTObservationColumns& observation() {return observation_p;}
  CTAntennaColumns& antenna() {return antenna_p;}
  CTFieldColumns& field() {return field_p;}
  CTHistoryColumns& history() {return history_p;}
  CTSpWindowColumns& spectralWindow() {return spectralWindow_p;}

  // </group>

  // Read-write access to optional subtables
  // <group>
  // </group>

  // Read-only access to required subtables
  // <group>
  const ROCTObservationColumns& observation() const {return observation_p;}
  const ROCTAntennaColumns& antenna() const {return antenna_p;}
  const ROCTFieldColumns& field() const {return field_p;}
  const ROCTHistoryColumns& history() const {return history_p;}
  const ROCTSpWindowColumns& spectralWindow() const {
    return spectralWindow_p;}

//protected:
//  friend class NewCalTable; 

private:
  // Access to subtables
  CTObservationColumns observation_p;
  CTAntennaColumns antenna_p;
  CTFieldColumns field_p;
  CTHistoryColumns history_p;
  CTSpWindowColumns spectralWindow_p;
};


} //# NAMESPACE CASA - END

#endif

