//# TJonesMBuf.h: TJones calibration main table buffer
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

#ifndef CALIBRATION_TJONESMBUF_H
#define CALIBRATION_TJONESMBUF_H

#include <casacore/measures/Measures/MFrequency.h>
#include <synthesis/CalTables/SolvableVJMBuf.h>
#include <synthesis/CalTables/TJonesMCol.h>
#include <synthesis/CalTables/TJonesTable.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// TJonesPolyMBuf: TJonesPoly calibration main table buffer
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//<li><linkto class="CalMainBuffer">CalMainBuffer</linkto> module
//<li><linkto class="SolvableVisJonesMBuf">SolvableVisJonesMBuf</linkto> module
//<li><linkto class="TJonesMBuf">TJonesMBuf</linkto> module
// </prerequisite>
//
// <etymology>
// From "TJonesPoly matrix", "cal main table" and "buffer"
// </etymology>
//
// <synopsis>
// The TJonesPolyMBuf class holds a buffer, optionally connected to a 
// TJonesPoly calibration main table iterator (of base type CalIterBase). 
// The TJonesPoly matrix contains atmospheric gains expressed as
// polynomials over time.
// </synopsis>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate TJonesPoly calibration main table data buffers.
// </motivation>
//
// <todo asof="01/08/01">
// (i) Deal with non-standard columns.
// </todo>

class TJonesPolyMBuf : public TJonesMBuf
{
 public:
  // Default constructor. No connection to an underlying
  // calibration table iterator in this case.
  TJonesPolyMBuf();

  // Construct from a set of calibration buffer indices and
  // their specified values. Non-index columns will be set 
  // to default values, and there is no connection to an 
  // underlying calibration table iterator in this case.
  TJonesPolyMBuf (const casacore::Vector<casacore::Int>& calIndices, 
		  const casacore::Block<casacore::Vector<casacore::Int> >& indexValues);

  // Construct from a calibration table iterator. The calibration
  // buffer will remain synchronized with the iterator.
  TJonesPolyMBuf (CalIterBase& calIter);

  // Invalidate the current calibration buffer. This signals 
  // that a re-read is required as the iterator has advanced
  virtual void invalidate();

  // Write the current buffer at the end of a specified cal table
  virtual casacore::Int append (CalTable& calTable);

  // Maximum number of rows in the calibration buffer
  virtual casacore::Int nRow();

  // Update the parametrized solution in each of a set of buffer rows
  virtual casacore::Bool fillMatchingRows (const casacore::Vector<casacore::Int>& matchingRows,
				 const casacore::String& sFreqGrpName,
				 const casacore::String& sPolyType, 
				 const casacore::String& sPolyMode,
				 const casacore::Complex& sScaleFactor,
				 const casacore::Int& sNPolyAmp, const casacore::Int& sNPolyPhase, 
				 const casacore::Vector<casacore::Double>& sPolyCoeffAmp,
				 const casacore::Vector<casacore::Double>& sPolyCoeffPhase,
				 const casacore::String& sPhaseUnits, 
				 const casacore::MFrequency& sRefFreq, 
				 const casacore::Int& sRefAnt);

  // casacore::Data field accessors
  virtual casacore::Vector<casacore::String>& polyType();
  virtual casacore::Vector<casacore::String>& polyMode();
  virtual casacore::Vector<casacore::Complex>& scaleFactor();
  virtual casacore::Vector<casacore::Int>& nPolyAmp();
  virtual casacore::Vector<casacore::Int>& nPolyPhase();
  virtual casacore::Array<casacore::Double>& polyCoeffAmp();
  virtual casacore::Array<casacore::Double>& polyCoeffPhase();
  virtual casacore::Vector<casacore::String>& phaseUnits();

 protected:
  // Factory method to create a columns accessor object of the 
  // appropriate type
  virtual TJonesPolyMCol* newCalMainCol (CalTable& calTable) {
    return new TJonesPolyMCol(dynamic_cast<TJonesPolyTable&>(calTable));};
  
  // Access to the columns accessor object
  virtual TJonesPolyMCol* calMainCol() 
    {return dynamic_cast<TJonesPolyMCol*>(CalMainBuffer::calMainCol());};

  // <group>
  // Fill the TJonesPoly cal buffer attribute columns in an empty 
  // cal buffer, to extend the inherited method fillAttributes. The
  // cal indices, specified as enums from class MSCalEnums, are excluded
  // as non-attribute columns.
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
  casacore::Vector<casacore::String> polyType_p;
  casacore::Vector<casacore::String> polyMode_p;
  casacore::Vector<casacore::Complex> scaleFactor_p;
  casacore::Vector<casacore::Int> nPolyAmp_p;
  casacore::Vector<casacore::Int> nPolyPhase_p;
  casacore::Array<casacore::Double> polyCoeffAmp_p;
  casacore::Array<casacore::Double> polyCoeffPhase_p;
  casacore::Vector<casacore::String> phaseUnits_p;

  // Buffer field status flags
  casacore::Bool polyTypeOK_p;
  casacore::Bool polyModeOK_p;
  casacore::Bool scaleFactorOK_p;
  casacore::Bool nPolyAmpOK_p;
  casacore::Bool nPolyPhaseOK_p;
  casacore::Bool polyCoeffAmpOK_p;
  casacore::Bool polyCoeffPhaseOK_p;
  casacore::Bool phaseUnitsOK_p;
};

// <summary> 
// TJonesSplineMBuf: TJonesSpline calibration main table buffer
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//<li><linkto class="CalMainBuffer">CalMainBuffer</linkto> module
//<li><linkto class="SolvableVisJonesMBuf">SolvableVisJonesMBuf</linkto> module
//<li><linkto class="TJonesMBuf">TJonesMBuf</linkto> module
//<li><linkto class="TJonesPolyMBuf">TJonesPolyMBuf</linkto> module
// </prerequisite>
//
// <etymology>
// From "TJonesSpline matrix", "cal main table" and "buffer"
// </etymology>
//
// <synopsis>
// The TJonesSplineMBuf class holds a buffer, optionally connected to a 
// TJonesSpline calibration main table iterator (of base type CalIterBase). 
// The TJonesSpline matrix contains atmospheric gain terms expressed as
// as spline polynomials over time.
// </synopsis>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate TJonesSpline calibration main table data buffers.
// </motivation>
//
// <todo asof="01/08/01">
// (i) Deal with non-standard columns.
// </todo>

class TJonesSplineMBuf : public TJonesPolyMBuf
{
 public:
  // Default constructor. No connection to an underlying
  // calibration table iterator in this case.
  TJonesSplineMBuf();

  // Construct from a set of calibration buffer indices and
  // their specified values. Non-index columns will be set 
  // to default values, and there is no connection to an 
  // underlying calibration table iterator in this case.
  TJonesSplineMBuf (const casacore::Vector<casacore::Int>& calIndices, 
		    const casacore::Block<casacore::Vector<casacore::Int> >& indexValues);

  // Construct from a calibration table iterator. The calibration
  // buffer will remain synchronized with the iterator.
  TJonesSplineMBuf (CalIterBase& calIter);

  // Invalidate the current calibration buffer. This signals 
  // that a re-read is required as the iterator has advanced
  virtual void invalidate();

  // Write the current buffer at the end of a specified cal table
  virtual casacore::Int append (CalTable& calTable);

  // Maximum number of rows in the calibration buffer
  virtual casacore::Int nRow();

  // Update the parametrized solution in each of a set of buffer rows
  virtual casacore::Bool fillMatchingRows (const casacore::Vector<casacore::Int>& matchingRows,
				 const casacore::String& sFreqGrpName,
				 const casacore::String& sPolyType, 
				 const casacore::String& sPolyMode,
				 const casacore::Complex& sScaleFactor,
				 const casacore::Int& sNPolyAmp, const casacore::Int& sNPolyPhase, 
				 const casacore::Vector<casacore::Double>& sPolyCoeffAmp,
				 const casacore::Vector<casacore::Double>& sPolyCoeffPhase,
				 const casacore::String& sPhaseUnits, 
				 const casacore::Int& sNKnotsAmp, 
				 const casacore::Int& sNKnotsPhase,
				 const casacore::Vector<casacore::Double>& sSplineKnotsAmp,
				 const casacore::Vector<casacore::Double>& sSplineKnotsPhase,
				 const casacore::MFrequency& sRefFreq, 
				 const casacore::Int& sRefAnt);

  // casacore::Data field accessors
  virtual casacore::Vector<casacore::Int>& nKnotsAmp();
  virtual casacore::Vector<casacore::Int>& nKnotsPhase();
  virtual casacore::Array<casacore::Double>& splineKnotsAmp();
  virtual casacore::Array<casacore::Double>& splineKnotsPhase();

 protected:
  // Factory method to create a columns accessor object of the 
  // appropriate type
  virtual TJonesSplineMCol* newCalMainCol (CalTable& calTable) {
    return new TJonesSplineMCol(dynamic_cast<TJonesSplineTable&>(calTable));};
  
  // Access to the columns accessor object
  virtual TJonesSplineMCol* calMainCol() 
    {return dynamic_cast<TJonesSplineMCol*>(CalMainBuffer::calMainCol());};

  // <group>
  // Fill the TJonesSpline cal buffer attribute columns in an empty 
  // cal buffer, to extend the inherited method fillAttributes. The
  // cal indices, specified as enums from class MSCalEnums, are excluded
  // as non-attribute columns.
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
  casacore::Vector<casacore::Int> nKnotsAmp_p;
  casacore::Vector<casacore::Int> nKnotsPhase_p;
  casacore::Array<casacore::Double> splineKnotsAmp_p;
  casacore::Array<casacore::Double> splineKnotsPhase_p;

  // Buffer field status flags
  casacore::Bool nKnotsAmpOK_p;
  casacore::Bool nKnotsPhaseOK_p;
  casacore::Bool splineKnotsAmpOK_p;
  casacore::Bool splineKnotsPhaseOK_p;
};


} //# NAMESPACE CASA - END

#endif
   
  



