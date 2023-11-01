//# TimeVarVJMCol.h: TimeVarVisJones cal_main table column access
//# Copyright (C) 1996,1997,1998,2001,2003
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

#ifndef CALIBRATION_TIMEVARVJMCOL_H
#define CALIBRATION_TIMEVARVJMCOL_H

#include <synthesis/CalTables/VisJonesMCol.h>
#include <synthesis/CalTables/TimeVarVJTable.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// ROTimeVarVisJonesMCol: Read-only TimeVarVisJones cal_main column access
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="ROCalMainColumns">ROCalMainColumns</linkto> module
//   <li> <linkto class="ROVisJonesMCol">ROVisJonesMCol</linkto> module
// </prerequisite>
//
// <etymology>
// From "read-only","time-variable visibility Jones","cal main" and "columns".
// </etymology>
//
// <synopsis>
// The ROTimeVarVisJonesMCol class allows read-only access to columns in the
// TimeVarVisJones main calibration table. Specializations for solvable 
// VisJones calibration table column access are provided through inheritance.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate read-only access to TVVJ calibration table columns.
// </motivation>
//
// <todo asof="01/07/01">
// (i) Deal with non-standard columns.
// </todo>

class ROTimeVarVisJonesMCol : public ROVisJonesMCol
{
 public:
  // Construct from a calibration table
  ROTimeVarVisJonesMCol (const TimeVarVisJonesTable& tvvjTable);

  // Default destructor
  virtual ~ROTimeVarVisJonesMCol() {};

 protected:
  // Prohibit public use of the null constructor, which
  // does not produce a usable object.
  ROTimeVarVisJonesMCol() {};

 private:
  // Prohibit copy constructor and assignment operator 
  ROTimeVarVisJonesMCol (const ROTimeVarVisJonesMCol&);
  ROTimeVarVisJonesMCol& operator= (const ROTimeVarVisJonesMCol&);
};

// <summary> 
// TimeVarVisJonesMCol: RW TimeVarVisJones cal_main column access
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="CalMainColumns">CalMainColumns</linkto> module
//   <li> <linkto class="VisJonesMCol">VisJonesMCol</linkto> module
// </prerequisite>
//
// <etymology>
// From "time-variable visibility Jones","cal main table" and "columns".
// </etymology>
//
// <synopsis>
// The TimeVarVisJonesMCol class allows read-write access to columns in the
// TimeVarVisJones main calibration table. Specializations for solvable 
// VisJones calibration table types are provided through inheritance.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate read-write access to TVVJ calibration table columns.
// </motivation>
//
// <todo asof="01/07/01">
// (i) Deal with non-standard columns.
// </todo>

class TimeVarVisJonesMCol : public VisJonesMCol
{
 public:
  // Construct from a calibration table
  TimeVarVisJonesMCol (TimeVarVisJonesTable& tvvjTable);

  // Default destructor
  virtual ~TimeVarVisJonesMCol() {};

 protected:
  // Prohibit public use of the null constructor, which
  // does not produce a usable object.
  TimeVarVisJonesMCol() {};

 private:
  // Prohibit copy constructor and assignment operator 
  TimeVarVisJonesMCol (const TimeVarVisJonesMCol&);
  TimeVarVisJonesMCol& operator= (const TimeVarVisJonesMCol&);
};

// <summary> 
// ROPJonesMCol: Read-only PJones cal_main column access
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//  <li> <linkto class="ROCalMainColumns">ROCalMainColumns</linkto> module
//  <li> <linkto class="ROVisJonesMCol">ROVisJonesMCol</linkto> module
// </prerequisite>
//
// <etymology>
// From "read-only","PJones","cal main" and "columns".
// </etymology>
//
// <synopsis>
// The ROPJonesMCol class allows read-only access to columns in the
// PJones main calibration table. PJones matrices are used to store
// parallactic angle information in the Measurement Equation formalism.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate read-only access to PJones calibration table columns.
// </motivation>
//
// <todo asof="01/07/01">
// (i) Deal with non-standard columns.
// </todo>

class ROPJonesMCol : public ROTimeVarVisJonesMCol
{
 public:
  // Construct from a calibration table
  ROPJonesMCol (const PJonesTable& pjTable);

  // Default destructor
  virtual ~ROPJonesMCol() {};

 protected:
  // Prohibit public use of the null constructor, which
  // does not produce a usable object.
  ROPJonesMCol() {};

 private:
  // Prohibit copy constructor and assignment operator 
  ROPJonesMCol (const ROPJonesMCol&);
  ROPJonesMCol& operator= (const ROPJonesMCol&);
};

// <summary> 
// PJonesMCol: RW PJones cal_main column access
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="CalMainColumns">CalMainColumns</linkto> module
//   <li> <linkto class="VisJonesMCol">VisJonesMCol</linkto> module
// </prerequisite>
//
// <etymology>
// From "PJones","cal main table" and "columns".
// </etymology>
//
// <synopsis>
// The PJonesMCol class allows read-write access to columns in the
// PJones main calibration table. PJones matrices are used to store
// parallactic angle information in the Measurement Equation formalism.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate read-write access to PJones calibration table columns.
// </motivation>
//
// <todo asof="01/07/01">
// (i) Deal with non-standard columns.
// </todo>

class PJonesMCol : public TimeVarVisJonesMCol
{
 public:
  // Construct from a calibration table
  PJonesMCol (PJonesTable& pjTable);

  // Default destructor
  virtual ~PJonesMCol() {};

 protected:
  // Prohibit public use of the null constructor, which
  // does not produce a usable object.
  PJonesMCol() {};

 private:
  // Prohibit copy constructor and assignment operator 
  PJonesMCol (const PJonesMCol&);
  PJonesMCol& operator= (const PJonesMCol&);
};

// <summary> 
// ROCJonesMCol: Read-only CJones cal_main column access
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//  <li> <linkto class="ROCalMainColumns">ROCalMainColumns</linkto> module
//  <li> <linkto class="ROVisJonesMCol">ROVisJonesMCol</linkto> module
// </prerequisite>
//
// <etymology>
// From "read-only","CJones","cal main" and "columns".
// </etymology>
//
// <synopsis>
// The ROCJonesMCol class allows read-only access to columns in the
// CJones main calibration table. CJones matrices are used to store
// polarization configuration information in the Measurement Equation 
// formalism.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate read-only access to CJones calibration table columns.
// </motivation>
//
// <todo asof="01/07/01">
// (i) Deal with non-standard columns.
// </todo>

class ROCJonesMCol : public ROTimeVarVisJonesMCol
{
 public:
  // Construct from a calibration table
  ROCJonesMCol (const CJonesTable& cjTable);

  // Default destructor
  virtual ~ROCJonesMCol() {};

 protected:
  // Prohibit public use of the null constructor, which
  // does not produce a usable object.
  ROCJonesMCol() {};

 private:
  // Prohibit copy constructor and assignment operator 
  ROCJonesMCol (const ROCJonesMCol&);
  ROCJonesMCol& operator= (const ROCJonesMCol&);
};

// <summary> 
// CJonesMCol: RW CJones cal_main column access
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="CalMainColumns">CalMainColumns</linkto> module
//   <li> <linkto class="VisJonesMCol">VisJonesMCol</linkto> module
// </prerequisite>
//
// <etymology>
// From "CJones","cal main table" and "columns".
// </etymology>
//
// <synopsis>
// The CJonesMCol class allows read-write access to columns in the
// CJones main calibration table. CJones matrices are used to store
// polarization configuration information in the Measurement 
// Equation formalism.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate read-write access to CJones calibration table columns.
// </motivation>
//
// <todo asof="01/07/01">
// (i) Deal with non-standard columns.
// </todo>

class CJonesMCol : public TimeVarVisJonesMCol
{
 public:
  // Construct from a calibration table
  CJonesMCol (CJonesTable& cjTable);

  // Default destructor
  virtual ~CJonesMCol() {};

 protected:
  // Prohibit public use of the null constructor, which
  // does not produce a usable object.
  CJonesMCol() {};

 private:
  // Prohibit copy constructor and assignment operator 
  CJonesMCol (const CJonesMCol&);
  CJonesMCol& operator= (const CJonesMCol&);
};


} //# NAMESPACE CASA - END

#endif
   
  



