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

#ifndef CALIBRATION_SOLVABLECALSETMCOL_H
#define CALIBRATION_SOLVABLECALSETMCOL_H

#include <synthesis/CalTables/CalMainColumns2.h>
#include <synthesis/CalTables/ROCalMainColumns2.h>
#include <synthesis/CalTables/CalTable2.h>

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

template <class T>
class ROSolvableCalSetMCol : public  ROCalMainColumns2<T>
{
 public:
  // Construct from a calibration table
  ROSolvableCalSetMCol (const CalTable2& svjTable);

  // Default destructor
  virtual ~ROSolvableCalSetMCol() {};

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
  ROSolvableCalSetMCol() {};

 private:
  // Prohibit copy constructor and assignment operator 
  ROSolvableCalSetMCol (const ROSolvableCalSetMCol&);
  ROSolvableCalSetMCol& operator= (const ROSolvableCalSetMCol&);

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
// SolvableCalSetMCol: RW SolvableCalSet cal_main column access
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
// The SolvableCalSetMCol class allows read-write access to columns in the
// SolvableCalSet main calibration table. Specializations for individual
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

template <class T>
class SolvableCalSetMCol : public CalMainColumns2<T>
{
 public:
  // Construct from a calibration table
  SolvableCalSetMCol (CalTable2& svjTable);

  // Default destructor
  virtual ~SolvableCalSetMCol() {};

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
  SolvableCalSetMCol() {};

 private:
  // Prohibit copy constructor and assignment operator 
  SolvableCalSetMCol (const SolvableCalSetMCol&);
  SolvableCalSetMCol& operator= (const SolvableCalSetMCol&);

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



} //# NAMESPACE CASA - END

#ifndef AIPS_NO_TEMPLATE_SRC
#include <synthesis/CalTables/SolvableCalSetMCol.tcc>
#endif

#endif
   
  



