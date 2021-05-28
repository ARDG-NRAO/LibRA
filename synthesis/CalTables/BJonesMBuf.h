//# BJonesMBuf.h: BJones calibration main table buffer
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

#ifndef CALIBRATION_BJONESMBUF_H
#define CALIBRATION_BJONESMBUF_H

#include <measures/Measures/MFrequency.h>
#include <synthesis/CalTables/SolvableVJMBuf.h>
#include <synthesis/CalTables/BJonesMCol.h>
#include <synthesis/CalTables/BJonesTable.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// BJonesMBuf: BJones calibration main table buffer
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//<li><linkto class="CalMainBuffer">CalMainBuffer</linkto> module
//<li><linkto class="SolvableVisJonesMBuf">SolvableVisJonesMBuf</linkto> module
// </prerequisite>
//
// <etymology>
// From "BJones matrix", "cal main table" and "buffer"
// </etymology>
//
// <synopsis>
// The BJonesMBuf class holds a buffer, optionally connected to a 
// BJones calibration main table iterator (of base type CalIterBase). 
// The BJones matrix contains bandpass correction terms.
// </synopsis>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate BJones calibration main table data buffers.
// </motivation>
//
// <todo asof="01/08/01">
// (i) Deal with non-standard columns.
// </todo>

class BJonesMBuf : public SolvableVisJonesMBuf
{
 public:
  // Default constructor. No connection to an underlying
  // calibration table iterator in this case.
  BJonesMBuf();

  // Construct from a set of calibration buffer indices and
  // their specified values. Non-index columns will be set 
  // to default values, and there is no connection to an 
  // underlying calibration table iterator in this case.
  BJonesMBuf (const casacore::Vector<casacore::Int>& calIndices, 
	      const casacore::Block<casacore::Vector<casacore::Int> >& indexValues);

  // Construct from a calibration table iterator. The calibration
  // buffer will remain synchronized with the iterator.
  BJonesMBuf (CalIterBase& calIter);

 protected:

 private:
};

// <summary> 
// BJonesPolyMBuf: BJonesPoly calibration main table buffer
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//<li><linkto class="CalMainBuffer">CalMainBuffer</linkto> module
//<li><linkto class="SolvableVisJonesMBuf">SolvableVisJonesMBuf</linkto> module
// </prerequisite>
//
// <etymology>
// From "BJonesPoly matrix", "cal main table" and "buffer"
// </etymology>
//
// <synopsis>
// The BJonesPolyMBuf class holds a buffer, optionally connected to a 
// BJonesPoly calibration main table iterator (of base type CalIterBase). 
// The BJonesPoly matrix contains bandpass correction terms.
// </synopsis>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate BJonesPoly calibration main table data buffers.
// </motivation>
//
// <todo asof="01/08/01">
// (i) Deal with non-standard columns.
// </todo>

class BJonesPolyMBuf : public BJonesMBuf
{
 public:
  // Default constructor. No connection to an underlying
  // calibration table iterator in this case.
  BJonesPolyMBuf();

  // Construct from a set of calibration buffer indices and
  // their specified values. Non-index columns will be set 
  // to default values, and there is no connection to an 
  // underlying calibration table iterator in this case.
  BJonesPolyMBuf (const casacore::Vector<casacore::Int>& calIndices, 
		  const casacore::Block<casacore::Vector<casacore::Int> >& indexValues);

  // Construct from a calibration table iterator. The calibration
  // buffer will remain synchronized with the iterator.
  BJonesPolyMBuf (CalIterBase& calIter);

  // Write the current buffer at the end of a specified cal table
  virtual casacore::Int append (CalTable& calTable);

  // Maximum number of rows in the calibration buffer
  virtual casacore::Int nRow();

  // Update the parametrized solution for a given antenna id.
  virtual casacore::Bool putAntGain (const casacore::Int& antennaId, const casacore::String& sFreqGrpName,
			   const casacore::String& sPolyType, 
			   const casacore::Complex& sScaleFactor, 
			   const casacore::Vector<casacore::Double>& sValidDomain,
			   const casacore::Int& sNPolyAmp, const casacore::Int& sNPolyPhase, 
			   const casacore::Vector<casacore::Double>& sPolyCoeffAmp,
			   const casacore::Vector<casacore::Double>& sPolyCoeffPhase,
			   const casacore::String& sPhaseUnits, 
			   const casacore::Complex& sSideBandRef, 
			   const casacore::MFrequency& sRefFreq, const casacore::Int& sRefAnt);

  // casacore::Data field accessors
  casacore::Vector<casacore::String>& polyType();
  casacore::Vector<casacore::String>& polyMode();
  casacore::Vector<casacore::Complex>& scaleFactor();
  casacore::Array<casacore::Double>& validDomain();
  casacore::Vector<casacore::Int>& nPolyAmp();
  casacore::Vector<casacore::Int>& nPolyPhase();
  casacore::Array<casacore::Double>& polyCoeffAmp();
  casacore::Array<casacore::Double>& polyCoeffPhase();
  casacore::Vector<casacore::String>& phaseUnits();
  casacore::Vector<casacore::Complex>& sideBandRef();

 protected:
  // Factory method to create a columns accessor object of the 
  // appropriate type
  virtual BJonesPolyMCol* newCalMainCol (CalTable& calTable) {
    return new BJonesPolyMCol(dynamic_cast<BJonesPolyTable&>(calTable));};
  
  // Access to the columns accessor object
  virtual BJonesPolyMCol* calMainCol() 
    {return dynamic_cast<BJonesPolyMCol*>(CalMainBuffer::calMainCol());};

  // Invalidate the current cache.
  virtual void invalidate();

  // <group>
  // Fill the BJonesPoly cal buffer attribute columns in an empty 
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
  casacore::Array<casacore::Double> validDomain_p;
  casacore::Vector<casacore::Int> nPolyAmp_p;
  casacore::Vector<casacore::Int> nPolyPhase_p;
  casacore::Array<casacore::Double> polyCoeffAmp_p;
  casacore::Array<casacore::Double> polyCoeffPhase_p;
  casacore::Vector<casacore::String> phaseUnits_p;
  casacore::Vector<casacore::Complex> sideBandRef_p;

  // Buffer field status flags
  casacore::Bool polyTypeOK_p;
  casacore::Bool polyModeOK_p;
  casacore::Bool scaleFactorOK_p;
  casacore::Bool validDomainOK_p;
  casacore::Bool nPolyAmpOK_p;
  casacore::Bool nPolyPhaseOK_p;
  casacore::Bool polyCoeffAmpOK_p;
  casacore::Bool polyCoeffPhaseOK_p;
  casacore::Bool phaseUnitsOK_p;
  casacore::Bool sideBandRefOK_p;
};


} //# NAMESPACE CASA - END

#endif
   
  



