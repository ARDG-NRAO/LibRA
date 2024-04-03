//# CalIter.h: Sequential calibration table iterator
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

#ifndef CALIBRATION_CALITER_H
#define CALIBRATION_CALITER_H

#include <synthesis/CalTables/CalIterBase.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// CalIter: Sequential calibration table iterator
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="CalTable">CalTable</linkto> module
//   <li> <linkto class="CalIterBase">CalIterBase</linkto> module
// </prerequisite>
//
// <etymology>
// From "calibration table" and "iterator"
// </etymology>
//
// <synopsis>
// The CalIter class is a sequential calibration table iterator,
// inherited from the abstract base class CalIterBase. 
// </synopsis>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate sequential calibration table iteration.
// </motivation>
//
// <todo asof="01/08/01">
// (i) Deal with non-standard columns.
// </todo>

class CalIter : public CalIterBase
{
 public:
  // Construct from a calibration table without specifying iteration
  // indices and intervals. This returns the input calibration table
  // in its entirety for each iteration, in the original sort order.
  CalIter (CalTable& calTable);

  // Construct from a calibration table, set of iteration indices,
  // iteration order and iteration sort option. This returns the 
  // input calibration table in its entirety for each iteration, 
  // sorted on the specified iteration indices.
  CalIter (CalTable& calTable, const casacore::Vector<casacore::Int>& iterIndices,
	   casacore::Sort::Order order = casacore::Sort::Ascending, 
	   casacore::Sort::Option = casacore::Sort::HeapSort);

  // Default destructor
  virtual ~CalIter();

  // Iterator control
  void origin() {};
  casacore::Bool more() {return true;};

  // Current iteration sub-table
  virtual CalTable& subTable();

 protected:
  // Prohibit public use of the null constructor, which
  // does not produce a usable object.
  CalIter() {};

 private:
  // Pointer to calibration table
  CalTable* calTable_p;
};


} //# NAMESPACE CASA - END

#endif
   
  



