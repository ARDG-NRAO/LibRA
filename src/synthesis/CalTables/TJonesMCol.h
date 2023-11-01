//# TJonesMCol.h: TJones cal_main table column access
//# Copyright (C) 1996,1997,1998,2001,2002,2003
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

#ifndef CALIBRATION_TJONESMCOL_H
#define CALIBRATION_TJONESMCOL_H

#include <synthesis/CalTables/SolvableVJMCol.h>
#include <synthesis/CalTables/TJonesTable.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// ROTJonesPolyMCol: Read-only TJonesPoly cal_main column access
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//  <li> <linkto class="ROCalMainColumns">ROCalMainColumns</linkto> module
// </prerequisite>
//
// <etymology>
// From "read-only","TJonesPoly","cal main" and "columns".
// </etymology>
//
// <synopsis>
// The ROTJonesPolyMCol class allows read-only access to columns in the
// TJonesPoly main calibration table. TJonesPoly matrices are used to 
// store atmospheric gain information in the Measurement Equation formalism 
// in polynomial form over time.
// </synopsis>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate read-only access to TJonesPoly calibration table columns.
// </motivation>
//
// <todo asof="02/02/01">
// (i) Deal with non-standard columns.
// (ii) Migrate TJonesMCol to this file.
// </todo>

class ROTJonesPolyMCol : public ROTJonesMCol
{
 public:
  // Construct from a calibration table
  ROTJonesPolyMCol (const TJonesPolyTable& gjpTable);

  // Default destructor
  virtual ~ROTJonesPolyMCol() {};

  // Read-only column accessors
  const casacore::ScalarColumn<casacore::String>& polyType() const {return polyType_p;};
  const casacore::ScalarColumn<casacore::String>& polyMode() const {return polyMode_p;};
  const casacore::ScalarColumn<casacore::Complex>& scaleFactor() const {return scaleFactor_p;};
  const casacore::ScalarColumn<casacore::Int>& nPolyAmp() const {return nPolyAmp_p;};
  const casacore::ScalarColumn<casacore::Int>& nPolyPhase() const {return nPolyPhase_p;};
  const casacore::ArrayColumn<casacore::Double>& polyCoeffAmp() const {return polyCoeffAmp_p;};
  const casacore::ArrayColumn<casacore::Double>& polyCoeffPhase() const 
    {return polyCoeffPhase_p;};
  const casacore::ScalarColumn<casacore::String>& phaseUnits() const {return phaseUnits_p;};

 protected:
  // Prohibit public use of the null constructor, which
  // does not produce a usable object.
  ROTJonesPolyMCol() {};

 private:
  // Prohibit copy constructor and assignment operator 
  ROTJonesPolyMCol (const ROTJonesPolyMCol&);
  ROTJonesPolyMCol& operator= (const ROTJonesPolyMCol&);

  // Private column accessors
  casacore::ScalarColumn<casacore::String> polyType_p;
  casacore::ScalarColumn<casacore::String> polyMode_p;
  casacore::ScalarColumn<casacore::Complex> scaleFactor_p;
  casacore::ScalarColumn<casacore::Int> nPolyAmp_p;
  casacore::ScalarColumn<casacore::Int> nPolyPhase_p;
  casacore::ArrayColumn<casacore::Double> polyCoeffAmp_p;
  casacore::ArrayColumn<casacore::Double> polyCoeffPhase_p;
  casacore::ScalarColumn<casacore::String> phaseUnits_p;
};

// <summary> 
// TJonesPolyMCol: RW TJonesPoly cal_main column access
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="CalMainColumns">CalMainColumns</linkto> module
// </prerequisite>
//
// <etymology>
// From "TJonesPoly","cal main table" and "columns".
// </etymology>
//
// <synopsis>
// The TJonesPolyMCol class allows read-only access to columns in the
// TJonesPoly main calibration table. TJonesPoly matrices are used to 
// store atmospheric gain information in the Measurement Equation formalism 
// in polynomial form over time.
// </synopsis>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate read-write access to TJonesPoly calibration table columns.
// </motivation>
//
// <todo asof="01/07/01">
// (i) Deal with non-standard columns.
// </todo>

class TJonesPolyMCol : public TJonesMCol
{
 public:
  // Construct from a calibration table
  TJonesPolyMCol (TJonesPolyTable& gjpTable);

  // Default destructor
  virtual ~TJonesPolyMCol() {};

  // Read-write column accessors
  casacore::ScalarColumn<casacore::String>& polyType() {return polyType_p;};
  casacore::ScalarColumn<casacore::String>& polyMode() {return polyMode_p;};
  casacore::ScalarColumn<casacore::Complex>& scaleFactor() {return scaleFactor_p;};
  casacore::ScalarColumn<casacore::Int>& nPolyAmp() {return nPolyAmp_p;};
  casacore::ScalarColumn<casacore::Int>& nPolyPhase() {return nPolyPhase_p;};
  casacore::ArrayColumn<casacore::Double>& polyCoeffAmp() {return polyCoeffAmp_p;};
  casacore::ArrayColumn<casacore::Double>& polyCoeffPhase() {return polyCoeffPhase_p;};
  casacore::ScalarColumn<casacore::String>& phaseUnits() {return phaseUnits_p;};

 protected:
  // Prohibit public use of the null constructor, which
  // does not produce a usable object.
  TJonesPolyMCol() {};

 private:
  // Prohibit copy constructor and assignment operator 
  TJonesPolyMCol (const TJonesPolyMCol&);
  TJonesPolyMCol& operator= (const TJonesPolyMCol&);

  // Private column accessors
  casacore::ScalarColumn<casacore::String> polyType_p;
  casacore::ScalarColumn<casacore::String> polyMode_p;
  casacore::ScalarColumn<casacore::Complex> scaleFactor_p;
  casacore::ScalarColumn<casacore::Int> nPolyAmp_p;
  casacore::ScalarColumn<casacore::Int> nPolyPhase_p;
  casacore::ArrayColumn<casacore::Double> polyCoeffAmp_p;
  casacore::ArrayColumn<casacore::Double> polyCoeffPhase_p;
  casacore::ScalarColumn<casacore::String> phaseUnits_p;
};

// <summary> 
// ROTJonesSplineMCol: Read-only TJonesSpline cal_main column access
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//  <li> <linkto class="ROCalMainColumns">ROCalMainColumns</linkto> module
// </prerequisite>
//
// <etymology>
// From "read-only","TJonesSpline","cal main" and "columns".
// </etymology>
//
// <synopsis>
// The ROTJonesSplineMCol class allows read-only access to columns in the
// TJonesSpline main calibration table. TJonesSpline matrices are used to 
// store atmospheric gain information in the Measurement Equation formalism 
// in spline polynomial form over time.
// </synopsis>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate read-only access to TJonesSpline calibration table columns.
// </motivation>
//
// <todo asof="02/02/01">
// (i) Deal with non-standard columns.
// (ii) Migrate TJonesMCol to this file.
// </todo>

class ROTJonesSplineMCol : public ROTJonesPolyMCol
{
 public:
  // Construct from a calibration table
  ROTJonesSplineMCol (const TJonesSplineTable& gjsTable);

  // Default destructor
  virtual ~ROTJonesSplineMCol() {};

  // Read-only column accessors
  const casacore::ScalarColumn<casacore::Int>& nKnotsAmp() const {return nKnotsAmp_p;};
  const casacore::ScalarColumn<casacore::Int>& nKnotsPhase() const {return nKnotsPhase_p;};
  const casacore::ArrayColumn<casacore::Double>& splineKnotsAmp() const 
    {return splineKnotsAmp_p;};
  const casacore::ArrayColumn<casacore::Double>& splineKnotsPhase() const
    {return splineKnotsPhase_p;};

 protected:
  // Prohibit public use of the null constructor, which
  // does not produce a usable object.
  ROTJonesSplineMCol() {};

 private:
  // Prohibit copy constructor and assignment operator 
  ROTJonesSplineMCol (const ROTJonesSplineMCol&);
  ROTJonesSplineMCol& operator= (const ROTJonesSplineMCol&);

  // Private column accessors
  casacore::ScalarColumn<casacore::Int> nKnotsAmp_p;
  casacore::ScalarColumn<casacore::Int> nKnotsPhase_p;
  casacore::ArrayColumn<casacore::Double> splineKnotsAmp_p;
  casacore::ArrayColumn<casacore::Double> splineKnotsPhase_p;
};

// <summary> 
// TJonesSplineMCol: RW TJonesSpline cal_main column access
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="CalMainColumns">CalMainColumns</linkto> module
// </prerequisite>
//
// <etymology>
// From "TJonesSpline","cal main table" and "columns".
// </etymology>
//
// <synopsis>
// The TJonesSplineMCol class allows read-only access to columns in the
// TJonesSpline main calibration table. TJonesSpline matrices are used to 
// store atmospheric gain information in the Measurement Equation formalism 
// in spline polynomial form over time.
// </synopsis>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate read-write access to TJonesSpline calibration table columns.
// </motivation>
//
// <todo asof="01/07/01">
// (i) Deal with non-standard columns.
// </todo>

class TJonesSplineMCol : public TJonesPolyMCol
{
 public:
  // Construct from a calibration table
  TJonesSplineMCol (TJonesSplineTable& gjsTable);

  // Default destructor
  virtual ~TJonesSplineMCol() {};

  // Read-write column accessors
  casacore::ScalarColumn<casacore::Int>& nKnotsAmp() {return nKnotsAmp_p;};
  casacore::ScalarColumn<casacore::Int>& nKnotsPhase() {return nKnotsPhase_p;};
  casacore::ArrayColumn<casacore::Double>& splineKnotsAmp() {return splineKnotsAmp_p;};
  casacore::ArrayColumn<casacore::Double>& splineKnotsPhase() {return splineKnotsPhase_p;};

 protected:
  // Prohibit public use of the null constructor, which
  // does not produce a usable object.
  TJonesSplineMCol() {};

 private:
  // Prohibit copy constructor and assignment operator 
  TJonesSplineMCol (const TJonesSplineMCol&);
  TJonesSplineMCol& operator= (const TJonesSplineMCol&);

  // Private column accessors
  casacore::ScalarColumn<casacore::Int> nKnotsAmp_p;
  casacore::ScalarColumn<casacore::Int> nKnotsPhase_p;
  casacore::ArrayColumn<casacore::Double> splineKnotsAmp_p;
  casacore::ArrayColumn<casacore::Double> splineKnotsPhase_p;
};


} //# NAMESPACE CASA - END

#endif
   
  



