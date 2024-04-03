//# SolvableVJMBuf.h: SolvableVisJones calibration main table buffer
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

#ifndef CALIBRATION_SOLVABLEVJMBUF_H
#define CALIBRATION_SOLVABLEVJMBUF_H

#include <synthesis/CalTables/TimeVarVJMBuf.h>
#include <synthesis/CalTables/SolvableVJMCol.h>
#include <synthesis/CalTables/SolvableVJTable.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// SolvableVisJonesMBuf: SolvableVisJones calibration main table buffer
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <li> <linkto class="CalMainBuffer">CalMainBuffer</linkto> module
// <li> <linkto class="TimeVarVisJonesMBuf">TimeVarVisJonesMBuf</linkto> module
// </prerequisite>
//
// <etymology>
// From "solvable visibility Jones matrix", "cal main table" and "buffer"
// </etymology>
//
// <synopsis>
// The SolvableVisJonesMBuf class holds a buffer, optionally connected to a 
// SolvableVisJones calibration main table iterator (of base type 
// CalIterBase). Specializations for different solvable Jones matrix
// types are provided through inheritance.
// </synopsis>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate SolvableVisJones calibration main table data buffers.
// </motivation>
//
// <todo asof="01/08/01">
// (i) Deal with non-standard columns.
// </todo>

class SolvableVisJonesMBuf : public TimeVarVisJonesMBuf
{
 public:
  // Default constructor. No connection to an underlying
  // calibration table iterator in this case.
  SolvableVisJonesMBuf();

  // Construct from a set of calibration buffer indices and
  // their specified values. Non-index columns will be set 
  // to default values, and there is no connection to an 
  // underlying calibration table iterator in this case.
  SolvableVisJonesMBuf (const casacore::Vector<casacore::Int>& calIndices, 
			const casacore::Block<casacore::Vector<casacore::Int> >& indexValues);

  // Construct from a calibration table iterator. The calibration
  // buffer will remain synchronized with the iterator.
  SolvableVisJonesMBuf (CalIterBase& calIter);

  // Write the current buffer at the end of a specified cal table;
  // returns the number of rows appended
  virtual casacore::Int append (CalTable& calTable);

  // Maximum number of rows in the calibration buffer
  virtual casacore::Int nRow();

  // casacore::Data field accessors
  virtual casacore::Vector<casacore::Bool>& totalSolnOk();
  virtual casacore::Vector<casacore::Float>& totalFit();
  virtual casacore::Vector<casacore::Float>& totalFitWgt();
  virtual casacore::Array<casacore::Bool>& solnOk();
  virtual casacore::Array<casacore::Float>& fit();
  virtual casacore::Array<casacore::Float>& fitWgt();
  virtual casacore::Array<casacore::Bool>& flag();
  virtual casacore::Array<casacore::Float>& snr();

 protected:
  // Factory method to create a columns accessor object of the appropriate type
  virtual SolvableVisJonesMCol* newCalMainCol (CalTable& calTable) {
    return new 
      SolvableVisJonesMCol(dynamic_cast<SolvableVisJonesTable&>(calTable));};

  // Calibration table columns accessor
  virtual SolvableVisJonesMCol* calMainCol() 
    {return dynamic_cast<SolvableVisJonesMCol*>(CalMainBuffer::calMainCol());};

  // Invalidate the current cache.
  virtual void invalidate();

  // <group>
  // Fill the cal buffer attribute columns in an empty cal buffer,
  // after the cal indices have been set using fillIndices(). The
  // cal indices, specified as enums from class MSCalEnums, are excluded
  // as non-attribute columns
  //
  // Use a visibility buffer to define the attribute values (NYI)
  virtual void fillAttributes(const casacore::Vector<casacore::Int>& /*calIndices*/,
			      const VisBuffer& /*vb*/) {};
  //
  // Set default attribute values
  virtual void fillAttributes(const casacore::Vector<casacore::Int>& calIndices);
  // </group>

 private:
  // Buffer fields
  casacore::Vector<casacore::Bool> totalSolnOk_p;
  casacore::Vector<casacore::Float> totalFit_p;
  casacore::Vector<casacore::Float> totalFitWgt_p;
  casacore::Array<casacore::Bool> solnOk_p;
  casacore::Array<casacore::Float> fit_p;
  casacore::Array<casacore::Float> fitWgt_p;
  casacore::Array<casacore::Bool> flag_p;
  casacore::Array<casacore::Float> snr_p;

  // Buffer field status flags
  casacore::Bool totalSolnOkOK_p;
  casacore::Bool totalFitOK_p;
  casacore::Bool totalFitWgtOK_p;
  casacore::Bool solnOkOK_p;
  casacore::Bool fitOK_p;
  casacore::Bool fitWgtOK_p;
  casacore::Bool flagOK_p;
  casacore::Bool snrOK_p;
};

// <summary> 
// GJonesMBuf: GJones calibration main table buffer
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <li> <linkto class="CalMainBuffer">CalMainBuffer</linkto> module
// <li> <linkto class="SolvableVisJonesMBuf">SolvableVisJonesMBuf</linkto> module
// </prerequisite>
//
// <etymology>
// From "GJones matrix", "cal main table" and "buffer"
// </etymology>
//
// <synopsis>
// The GJonesMBuf class holds a buffer, optionally connected to a 
// GJones calibration main table iterator (of base type CalIterBase). 
// The GJones matrix contains electronic gain correction terms.
// </synopsis>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate GJones calibration main table data buffers.
// </motivation>
//
// <todo asof="01/08/01">
// (i) Deal with non-standard columns.
// </todo>

class GJonesMBuf : public SolvableVisJonesMBuf
{
 public:
  // Default constructor. No connection to an underlying
  // calibration table iterator in this case.
  GJonesMBuf();

  // Construct from a set of calibration buffer indices and
  // their specified values. Non-index columns will be set 
  // to default values, and there is no connection to an 
  // underlying calibration table iterator in this case.
  GJonesMBuf (const casacore::Vector<casacore::Int>& calIndices, 
	      const casacore::Block<casacore::Vector<casacore::Int> >& indexValues);

  // Construct from a calibration table iterator. The calibration
  // buffer will remain synchronized with the iterator.
  GJonesMBuf (CalIterBase& calIter);

  // Update the calibration solution in each of a set of buffer rows
  virtual casacore::Bool fillMatchingRows (const casacore::Vector<casacore::Int>& matchingRows,
				 const casacore::String& sFreqGrpName,
				 const casacore::Complex& sGain,
				 const casacore::MFrequency& sRefFreq,
				 const casacore::Int& sRefAnt);
 protected:

 private:
};

// <summary> 
// DJonesMBuf: DJones calibration main table buffer
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <li> <linkto class="CalMainBuffer">CalMainBuffer</linkto> module
// <li> <linkto class="SolvableVisJonesMBuf">SolvableVisJonesMBuf</linkto> module
// </prerequisite>
//
// <etymology>
// From "DJones matrix", "cal main table" and "buffer"
// </etymology>
//
// <synopsis>
// The DJonesMBuf class holds a buffer, optionally connected to a 
// DJones calibration main table iterator (of base type CalIterBase). 
// The DJones matrix contains instrumental polarization corrections.
// </synopsis>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate DJones calibration main table data buffers.
// </motivation>
//
// <todo asof="01/08/01">
// (i) Deal with non-standard columns.
// </todo>

class DJonesMBuf : public SolvableVisJonesMBuf
{
 public:
  // Default constructor. No connection to an underlying
  // calibration table iterator in this case.
  DJonesMBuf();

  // Construct from a set of calibration buffer indices and
  // their specified values. Non-index columns will be set 
  // to default values, and there is no connection to an 
  // underlying calibration table iterator in this case.
  DJonesMBuf (const casacore::Vector<casacore::Int>& calIndices, 
	      const casacore::Block<casacore::Vector<casacore::Int> >& indexValues);

  // Construct from a calibration table iterator. The calibration
  // buffer will remain synchronized with the iterator.
  DJonesMBuf (CalIterBase& calIter);

 protected:

 private:
};

// <summary> 
// TJonesMBuf: TJones calibration main table buffer
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <li> <linkto class="CalMainBuffer">CalMainBuffer</linkto> module
// <li> <linkto class="SolvableVisJonesMBuf">SolvableVisJonesMBuf</linkto> module
// </prerequisite>
//
// <etymology>
// From "TJones matrix", "cal main table" and "buffer"
// </etymology>
//
// <synopsis>
// The TJonesMBuf class holds a buffer, optionally connected to a 
// TJones calibration main table iterator (of base type CalIterBase). 
// The TJones matrix contains atmospheric corrections.
// </synopsis>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate TJones calibration main table data buffers.
// </motivation>
//
// <todo asof="01/08/01">
// (i) Deal with non-standard columns.
// </todo>

class TJonesMBuf : public SolvableVisJonesMBuf
{
 public:
  // Default constructor. No connection to an underlying
  // calibration table iterator in this case.
  TJonesMBuf();

  // Construct from a set of calibration buffer indices and
  // their specified values. Non-index columns will be set 
  // to default values, and there is no connection to an 
  // underlying calibration table iterator in this case.
  TJonesMBuf (const casacore::Vector<casacore::Int>& calIndices, 
	      const casacore::Block<casacore::Vector<casacore::Int> >& indexValues);

  // Construct from a calibration table iterator. The calibration
  // buffer will remain synchronized with the iterator.
  TJonesMBuf (CalIterBase& calIter);

 protected:

 private:
};


} //# NAMESPACE CASA - END

#endif
   
  



