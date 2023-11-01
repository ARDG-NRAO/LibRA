//# SolvableVJMCol.h: SolvableVisJones cal_main table column access
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

#ifndef CALIBRATION_SOLVABLEVJMCOL_H
#define CALIBRATION_SOLVABLEVJMCOL_H

#include <synthesis/CalTables/TimeVarVJMCol.h>
#include <synthesis/CalTables/SolvableVJTable.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// ROSolvableVisJonesMCol: Read-only SolvableVisJones cal_main column access
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="ROCalMainColumns">ROCalMainColumns</linkto> module
// </prerequisite>
//
// <etymology>
// From "read-only","solvable visibility Jones","cal main" and "columns".
// </etymology>
//
// <synopsis>
// The ROSolvableVisJonesMCol class allows read-only access to columns in the
// SolvableVisJones main calibration table. Specializations for individual
// solvable Jones matrix types (e.g. GJones) are provided through inheritance.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate read-only access to SVJ calibration table columns.
// </motivation>
//
// <todo asof="01/07/01">
// (i) Deal with non-standard columns.
// </todo>

class ROSolvableVisJonesMCol : public ROTimeVarVisJonesMCol
{
 public:
  // Construct from a calibration table
  ROSolvableVisJonesMCol (const SolvableVisJonesTable& svjTable);

  // Default destructor
  virtual ~ROSolvableVisJonesMCol() {};

  // Read-only column accessors
  const casacore::ScalarColumn<casacore::Bool>& totalSolnOk() const {return totalSolnOk_p;};
  const casacore::ScalarColumn<casacore::Float>& totalFit() const {return totalFit_p;};
  const casacore::ScalarColumn<casacore::Float>& totalFitWgt() const {return totalFitWgt_p;};
  const casacore::ArrayColumn<casacore::Bool>& solnOk() const {return solnOk_p;};
  const casacore::ArrayColumn<casacore::Float>& fit() const {return fit_p;};
  const casacore::ArrayColumn<casacore::Float>& fitWgt() const {return fitWgt_p;};
  const casacore::ArrayColumn<casacore::Bool>& flag() const {return flag_p;};
  const casacore::ArrayColumn<casacore::Float>& snr() const {return snr_p;};

 protected:
  // Prohibit public use of the null constructor, which
  // does not produce a usable object.
  ROSolvableVisJonesMCol() {};

 private:
  // Prohibit copy constructor and assignment operator 
  ROSolvableVisJonesMCol (const ROSolvableVisJonesMCol&);
  ROSolvableVisJonesMCol& operator= (const ROSolvableVisJonesMCol&);

  // Private column accessors
  casacore::ScalarColumn<casacore::Bool> totalSolnOk_p;
  casacore::ScalarColumn<casacore::Float> totalFit_p;
  casacore::ScalarColumn<casacore::Float> totalFitWgt_p;
  casacore::ArrayColumn<casacore::Bool> solnOk_p;
  casacore::ArrayColumn<casacore::Float> fit_p;
  casacore::ArrayColumn<casacore::Float> fitWgt_p;
  casacore::ArrayColumn<casacore::Bool> flag_p;
  casacore::ArrayColumn<casacore::Float> snr_p;
};

// <summary> 
// SolvableVisJonesMCol: RW SolvableVisJones cal_main column access
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="CalMainColumns">CalMainColumns</linkto> module
// </prerequisite>
//
// <etymology>
// From "solvable visibility Jones","cal main table" and "columns".
// </etymology>
//
// <synopsis>
// The SolvableVisJonesMCol class allows read-write access to columns in the
// SolvableVisJones main calibration table. Specializations for individual
// solvable Jones matrix types (e.g. GJones) are provided through inheritance.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate read-write access to SVJ calibration table columns.
// </motivation>
//
// <todo asof="01/07/01">
// (i) Deal with non-standard columns.
// </todo>

class SolvableVisJonesMCol : public TimeVarVisJonesMCol
{
 public:
  // Construct from a calibration table
  SolvableVisJonesMCol (SolvableVisJonesTable& svjTable);

  // Default destructor
  virtual ~SolvableVisJonesMCol() {};

  // Read-write column accessors
  casacore::ScalarColumn<casacore::Bool>& totalSolnOk() {return totalSolnOk_p;};
  casacore::ScalarColumn<casacore::Float>& totalFit() {return totalFit_p;};
  casacore::ScalarColumn<casacore::Float>& totalFitWgt() {return totalFitWgt_p;};
  casacore::ArrayColumn<casacore::Bool>& solnOk() {return solnOk_p;};
  casacore::ArrayColumn<casacore::Float>& fit() {return fit_p;};
  casacore::ArrayColumn<casacore::Float>& fitWgt() {return fitWgt_p;};
  casacore::ArrayColumn<casacore::Bool>& flag() {return flag_p;};
  casacore::ArrayColumn<casacore::Float>& snr() {return snr_p;};

 protected:
  // Prohibit public use of the null constructor, which
  // does not produce a usable object.
  SolvableVisJonesMCol() {};

 private:
  // Prohibit copy constructor and assignment operator 
  SolvableVisJonesMCol (const SolvableVisJonesMCol&);
  SolvableVisJonesMCol& operator= (const SolvableVisJonesMCol&);

  // Private column accessors
  casacore::ScalarColumn<casacore::Bool> totalSolnOk_p;
  casacore::ScalarColumn<casacore::Float> totalFit_p;
  casacore::ScalarColumn<casacore::Float> totalFitWgt_p;
  casacore::ArrayColumn<casacore::Bool> solnOk_p;
  casacore::ArrayColumn<casacore::Float> fit_p;
  casacore::ArrayColumn<casacore::Float> fitWgt_p;
  casacore::ArrayColumn<casacore::Bool> flag_p;
  casacore::ArrayColumn<casacore::Float> snr_p;
};

// <summary> 
// ROGJonesMCol: Read-only GJones cal_main column access
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//  <li> <linkto class="ROCalMainColumns">ROCalMainColumns</linkto> module
// </prerequisite>
//
// <etymology>
// From "read-only","GJones","cal main" and "columns".
// </etymology>
//
// <synopsis>
// The ROGJonesMCol class allows read-only access to columns in the
// GJones main calibration table. GJones matrices are used to store
// electronic gain information in the Measurement Equation formalism.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate read-only access to GJones calibration table columns.
// </motivation>
//
// <todo asof="01/07/01">
// (i) Deal with non-standard columns.
// </todo>

class ROGJonesMCol : public ROSolvableVisJonesMCol
{
 public:
  // Construct from a calibration table
  ROGJonesMCol (const GJonesTable& gjTable);

  // Default destructor
  virtual ~ROGJonesMCol() {};

 protected:
  // Prohibit public use of the null constructor, which
  // does not produce a usable object.
  ROGJonesMCol() {};

 private:
  // Prohibit copy constructor and assignment operator 
  ROGJonesMCol (const ROGJonesMCol&);
  ROGJonesMCol& operator= (const ROGJonesMCol&);
};

// <summary> 
// GJonesMCol: RW GJones cal_main column access
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="CalMainColumns">CalMainColumns</linkto> module
// </prerequisite>
//
// <etymology>
// From "GJones","cal main table" and "columns".
// </etymology>
//
// <synopsis>
// The GJonesMCol class allows read-write access to columns in the
// GJones main calibration table. GJones matrices are used to store
// electronic gain information in the Measurement Equation formalism.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate read-write access to GJones calibration table columns.
// </motivation>
//
// <todo asof="01/07/01">
// (i) Deal with non-standard columns.
// </todo>

class GJonesMCol : public SolvableVisJonesMCol
{
 public:
  // Construct from a calibration table
  GJonesMCol (GJonesTable& gjTable);

  // Default destructor
  virtual ~GJonesMCol() {};

 protected:
  // Prohibit public use of the null constructor, which
  // does not produce a usable object.
  GJonesMCol() {};

 private:
  // Prohibit copy constructor and assignment operator 
  GJonesMCol (const GJonesMCol&);
  GJonesMCol& operator= (const GJonesMCol&);
};

// <summary> 
// RODJonesMCol: Read-only DJones cal_main column access
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//  <li> <linkto class="ROCalMainColumns">ROCalMainColumns</linkto> module
// </prerequisite>
//
// <etymology>
// From "read-only","DJones","cal main" and "columns".
// </etymology>
//
// <synopsis>
// The RODJonesMCol class allows read-only access to columns in the
// DJones main calibration table. DJones matrices are used to store
// instrumental polarization calibration information in the 
// Measurement Equation formalism.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate read-only access to DJones calibration table columns.
// </motivation>
//
// <todo asof="01/07/01">
// (i) Deal with non-standard columns.
// </todo>

class RODJonesMCol : public ROSolvableVisJonesMCol
{
 public:
  // Construct from a calibration table
  RODJonesMCol (const DJonesTable& djTable);

  // Default destructor
  virtual ~RODJonesMCol() {};

 protected:
  // Prohibit public use of the null constructor, which
  // does not produce a usable object.
  RODJonesMCol() {};

 private:
  // Prohibit copy constructor and assignment operator 
  RODJonesMCol (const RODJonesMCol&);
  RODJonesMCol& operator= (const RODJonesMCol&);
};

// <summary> 
// DJonesMCol: RW DJones cal_main column access
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="CalMainColumns">CalMainColumns</linkto> module
// </prerequisite>
//
// <etymology>
// From "DJones","cal main table" and "columns".
// </etymology>
//
// <synopsis>
// The DJonesMCol class allows read-write access to columns in the
// DJones main calibration table. DJones matrices are used to store
// instrumental polarization calibration information in the Measurement 
// Equation formalism.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate read-write access to DJones calibration table columns.
// </motivation>
//
// <todo asof="01/07/01">
// (i) Deal with non-standard columns.
// </todo>

class DJonesMCol : public SolvableVisJonesMCol
{
 public:
  // Construct from a calibration table
  DJonesMCol (DJonesTable& djTable);

  // Default destructor
  virtual ~DJonesMCol() {};

 protected:
  // Prohibit public use of the null constructor, which
  // does not produce a usable object.
  DJonesMCol() {};

 private:
  // Prohibit copy constructor and assignment operator 
  DJonesMCol (const DJonesMCol&);
  DJonesMCol& operator= (const DJonesMCol&);
};

// <summary> 
// ROTJonesMCol: Read-only TJones cal_main column access
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//  <li> <linkto class="ROCalMainColumns">ROCalMainColumns</linkto> module
// </prerequisite>
//
// <etymology>
// From "read-only","TJones","cal main" and "columns".
// </etymology>
//
// <synopsis>
// The ROTJonesMCol class allows read-only access to columns in the
// TJones main calibration table. TJones matrices are used to store
// atmospheric calibration information in the Measurement Equation 
// formalism.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate read-only access to TJones calibration table columns.
// </motivation>
//
// <todo asof="01/07/01">
// (i) Deal with non-standard columns.
// </todo>

class ROTJonesMCol : public ROSolvableVisJonesMCol
{
 public:
  // Construct from a calibration table
  ROTJonesMCol (const TJonesTable& djTable);

  // Default destructor
  virtual ~ROTJonesMCol() {};

 protected:
  // Prohibit public use of the null constructor, which
  // does not produce a usable object.
  ROTJonesMCol() {};

 private:
  // Prohibit copy constructor and assignment operator 
  ROTJonesMCol (const ROTJonesMCol&);
  ROTJonesMCol& operator= (const ROTJonesMCol&);
};

// <summary> 
// TJonesMCol: RW TJones cal_main column access
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="CalMainColumns">CalMainColumns</linkto> module
// </prerequisite>
//
// <etymology>
// From "TJones","cal main table" and "columns".
// </etymology>
//
// <synopsis>
// The TJonesMCol class allows read-write access to columns in the
// TJones main calibration table. TJones matrices are used to store
// atmospheric calibration information in the Measurement Equation 
// formalism.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate read-write access to TJones calibration table columns.
// </motivation>
//
// <todo asof="01/07/01">
// (i) Deal with non-standard columns.
// </todo>

class TJonesMCol : public SolvableVisJonesMCol
{
 public:
  // Construct from a calibration table
  TJonesMCol (TJonesTable& djTable);

  // Default destructor
  virtual ~TJonesMCol() {};

 protected:
  // Prohibit public use of the null constructor, which
  // does not produce a usable object.
  TJonesMCol() {};

 private:
  // Prohibit copy constructor and assignment operator 
  TJonesMCol (const TJonesMCol&);
  TJonesMCol& operator= (const TJonesMCol&);
};


} //# NAMESPACE CASA - END

#endif
   
  



