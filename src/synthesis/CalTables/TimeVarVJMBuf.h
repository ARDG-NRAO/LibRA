//# TimeVarVJMBuf.h: TimeVarVisJones calibration main table buffer
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

#ifndef CALIBRATION_TIMEVARVJMBUF_H
#define CALIBRATION_TIMEVARVJMBUF_H

#include <synthesis/CalTables/VisJonesMBuf.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// TimeVarVisJonesMBuf: TimeVarVisJones calibration main table buffer
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="CalMainBuffer">CalMainBuffer</linkto> module
//   <li> <linkto class="VisJonesMBuf">VisJonesMBuf</linkto> module
// </prerequisite>
//
// <etymology>
// From "time-variable visibility Jones matrix", "cal main table" and "buffer"
// </etymology>
//
// <synopsis>
// The TimeVarVisJonesMBuf class holds a buffer, optionally connected to a 
// TimeVarVisJones calibration main table iterator (of base type CalIterBase). 
// Specializations solvable VisJones calibration table formats are provided 
// through inheritance.
// </synopsis>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate TimeVarVisJones calibration main table data buffers.
// </motivation>
//
// <todo asof="01/08/01">
// (i) Deal with non-standard columns.
// </todo>

class TimeVarVisJonesMBuf : public VisJonesMBuf
{
 public:
  // Default constructor. No connection to an underlying
  // calibration table iterator in this case.
  TimeVarVisJonesMBuf();

  // Construct from a set of calibration buffer indices and
  // their specified values. Non-index columns will be set 
  // to default values, and there is no connection to an 
  // underlying calibration table iterator in this case.
  TimeVarVisJonesMBuf (const casacore::Vector<casacore::Int>& calIndices, 
		       const casacore::Block<casacore::Vector<casacore::Int> >& indexValues);

  // Construct from a calibration table iterator. The calibration
  // buffer will remain synchronized with the iterator.
  TimeVarVisJonesMBuf (CalIterBase& calIter);

 protected:

 private:
};

// <summary> 
// PJonesMBuf: PJones calibration main table buffer
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <li> <linkto class="CalMainBuffer">CalMainBuffer</linkto> module
// <li> <linkto class="TimeVarVisJonesMBuf">TimeVarVisJonesMBuf</linkto> module
// </prerequisite>
//
// <etymology>
// From "PJones matrix", "cal main table" and "buffer"
// </etymology>
//
// <synopsis>
// The PJonesMBuf class holds a buffer, optionally connected to a 
// PJones calibration main table iterator (of base type CalIterBase). 
// The PJones matrix contains parallactic angle correction terms.
// </synopsis>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate PJones calibration main table data buffers.
// </motivation>
//
// <todo asof="01/08/01">
// (i) Deal with non-standard columns.
// </todo>

class PJonesMBuf : public TimeVarVisJonesMBuf
{
 public:
  // Default constructor. No connection to an underlying
  // calibration table iterator in this case.
  PJonesMBuf();

  // Construct from a set of calibration buffer indices and
  // their specified values. Non-index columns will be set 
  // to default values, and there is no connection to an 
  // underlying calibration table iterator in this case.
  PJonesMBuf (const casacore::Vector<casacore::Int>& calIndices, 
	      const casacore::Block<casacore::Vector<casacore::Int> >& indexValues);

  // Construct from a calibration table iterator. The calibration
  // buffer will remain synchronized with the iterator.
  PJonesMBuf (CalIterBase& calIter);

 protected:

 private:
};

// <summary> 
// CJonesMBuf: CJones calibration main table buffer
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <li> <linkto class="CalMainBuffer">CalMainBuffer</linkto> module
// <li> <linkto class="TimeVarVisJonesMBuf">TimeVarVisJonesMBuf</linkto> module
// </prerequisite>
//
// <etymology>
// From "CJones matrix", "cal main table" and "buffer"
// </etymology>
//
// <synopsis>
// The CJonesMBuf class holds a buffer, optionally connected to a 
// CJones calibration main table iterator (of base type CalIterBase). 
// The CJones matrix contains polarization configuration information.
// </synopsis>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate CJones calibration main table data buffers.
// </motivation>
//
// <todo asof="01/08/01">
// (i) Deal with non-standard columns.
// </todo>

class CJonesMBuf : public TimeVarVisJonesMBuf
{
 public:
  // Default constructor. No connection to an underlying
  // calibration table iterator in this case.
  CJonesMBuf();

  // Construct from a set of calibration buffer indices and
  // their specified values. Non-index columns will be set 
  // to default values, and there is no connection to an 
  // underlying calibration table iterator in this case.
  CJonesMBuf (const casacore::Vector<casacore::Int>& calIndices, 
	      const casacore::Block<casacore::Vector<casacore::Int> >& indexValues);

  // Construct from a calibration table iterator. The calibration
  // buffer will remain synchronized with the iterator.
  CJonesMBuf (CalIterBase& calIter);

 protected:

 private:
};


} //# NAMESPACE CASA - END

#endif
