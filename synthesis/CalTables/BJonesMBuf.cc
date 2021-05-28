//# BJonesMBuf.cc: Implementation of BJonesMBuf.h
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
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$
//----------------------------------------------------------------------------

#include <synthesis/CalTables/BJonesMBuf.h>
#include <casa/Arrays/ArrayMath.h>
#include <tables/Tables/RefRows.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

BJonesMBuf::BJonesMBuf() : SolvableVisJonesMBuf()
{
// Null constructor
};

//----------------------------------------------------------------------------

BJonesMBuf::BJonesMBuf (const Vector<Int>& calIndices,
			const Block<Vector<Int> >& indexValues) : 
  SolvableVisJonesMBuf (calIndices, indexValues)
{
// Construct from a set of cal buffer indices and specified index values
// Output to private data:
//    SolvableVisJonesMBuf   SolvableVisJonesMBuf    Parent class cal main 
//                                                   table buffer
};

//----------------------------------------------------------------------------

BJonesMBuf::BJonesMBuf (CalIterBase& calIter) 
  : SolvableVisJonesMBuf (calIter)
{
// Construct from a calibration table iterator
// Input:
//    calIter            CalIterBase&         Calibration table iterator
};

//----------------------------------------------------------------------------

BJonesPolyMBuf::BJonesPolyMBuf() : BJonesMBuf()
{
// Null constructor
// Output to private data:
//    BJonesMBuf          BJonesMBuf         BJones cal main buffer
//    polyType_p          Vector<String>     Polynomial type
//    polyMode_p          Vector<String>     Polynomial mode (e.g. A&P)
//    scaleFactor_p       Vector<Complex>    Polynomial scale factor
//    validDomain_p       Array<Double>      Valid domain [x_0, x_1]
//    nPolyAmp_p          Vector<Int>        Polynomial degree (amplitude)
//    nPolyPhase_p        Vector<Int>        Polynomial degree (phase)
//    polyCoeffAmp_p      Array<Double>      Polynomial coeff. (amplitude)
//    polyCoeffPhase_p    Array<Double>      Polynomial coeff. (phase)
//    phaseUnits_p        Vector<String>     Phase units
//    sideBandRef_p       Vector<Complex>    Sideband reference
//    polyTypeOK_p        Bool               Polynomial type cache ok
//    polyModeOK_p        Bool               Polynomial mode cache ok
//    scaleFactorOK_p     Bool               Scale factor cache ok
//    validDomainOK_p     Bool               Valid domain cache ok
//    nPolyAmpOK_p        Bool               Poly. degree (amp) cache ok
//    nPolyPhaseOK_p      Bool               Poly. degree (phase) cache ok
//    polyCoeffAmpOK_p    Bool               Poly. coeff. (amp) cache ok
//    polyCoeffPhaseOK_p  Bool               Poly. coeff. (phase) cache ok
//    phaseUnitsOK_p      Bool               Phase units cache ok
//    sideBandRefOK_p     Bool               Sideband ref. cache ok
//
  // Invalidate cache
  invalidate();
};

//----------------------------------------------------------------------------

BJonesPolyMBuf::BJonesPolyMBuf (const Vector<Int>& calIndices,
				const Block<Vector<Int> >& indexValues) : 
  BJonesMBuf (calIndices, indexValues)
{
// Construct from a set of cal buffer indices and specified index values
// Output to private data:
//    BJonesMBuf       BJonesMBuf         Parent class cal main table buffer
//
  // Set the local non-index columns to default values
  fillAttributes (calIndices);
};

//----------------------------------------------------------------------------

BJonesPolyMBuf::BJonesPolyMBuf (CalIterBase& calIter) : BJonesMBuf (calIter)
{
// Construct from a calibration table iterator
// Input:
//    calIter             CalIterBase&       Calibration table iterator
// Output to private data:
//    BJonesMBuf          BJonesMBuf         BJones cal main buffer
//    polyType_p          Vector<String>     Polynomial type
//    polyMode_p          Vector<String>     Polynomial mode (e.g. A&P)
//    scaleFactor_p       Vector<Complex>    Polynomial scale factor
//    validDomain_p       Array<Double>      Valid domain [x_0, x_1]
//    nPolyAmp_p          Vector<Int>        Polynomial degree (amplitude)
//    nPolyPhase_p        Vector<Int>        Polynomial degree (phase)
//    polyCoeffAmp_p      Array<Double>      Polynomial coeff. (amplitude)
//    polyCoeffPhase_p    Array<Double>      Polynomial coeff. (phase)
//    phaseUnits_p        Vector<String>     Phase units
//    sideBandRef_p       Vector<Complex>    Sideband reference
//    polyTypeOK_p        Bool               Polynomial type cache ok
//    polyModeOK_p        Bool               Polynomial mode cache ok
//    scaleFactorOK_p     Bool               Scale factor cache ok
//    validDomainOK_p     Bool               Valid domain cache ok
//    nPolyAmpOK_p        Bool               Poly. degree (amp) cache ok
//    nPolyPhaseOK_p      Bool               Poly. degree (phase) cache ok
//    polyCoeffAmpOK_p    Bool               Poly. coeff. (amp) cache ok
//    polyCoeffPhaseOK_p  Bool               Poly. coeff. (phase) cache ok
//    phaseUnitsOK_p      Bool               Phase units cache ok
//    sideBandRefOK_p     Bool               Sideband ref. cache ok
//
  // Invalidate cache
  invalidate();
};

//----------------------------------------------------------------------------

Int BJonesPolyMBuf::append (CalTable& calTable)
{
// Append the current calibration buffer to a calibration table
// Input:
//    calTable            CalTable&          Calibration table
//
  // Extend the inherited parent class method
  Int nAdded = SolvableVisJonesMBuf::append(calTable);

  // Compute the row numbers already added by the parent class
  uInt endRow = calTable.nRowMain() - 1;
  uInt startRow = endRow - nAdded + 1;

  // Attach a calibration table columns accessor
  BJonesPolyMCol bjpMainCol(dynamic_cast<BJonesPolyTable&>(calTable));

  // Append the current cal buffer main columns
  RefRows refRows(startRow, endRow);
  bjpMainCol.polyType().putColumnCells(refRows, polyType());
  bjpMainCol.polyMode().putColumnCells(refRows, polyMode());
  bjpMainCol.scaleFactor().putColumnCells(refRows, scaleFactor());
  bjpMainCol.validDomain().putColumnCells(refRows, validDomain());
  bjpMainCol.nPolyAmp().putColumnCells(refRows, nPolyAmp());
  bjpMainCol.nPolyPhase().putColumnCells(refRows, nPolyPhase());
  bjpMainCol.polyCoeffAmp().putColumnCells(refRows, polyCoeffAmp());
  bjpMainCol.polyCoeffPhase().putColumnCells(refRows, polyCoeffPhase());
  bjpMainCol.phaseUnits().putColumnCells(refRows, phaseUnits());
  bjpMainCol.sideBandRef().putColumnCells(refRows, sideBandRef());

  return nAdded;
};

//----------------------------------------------------------------------------

Int BJonesPolyMBuf::nRow()
{
// Return the maximum number of rows in the calibration buffer
// Input from private data:
//    polyTypeOK_p        Bool               Polynomial type cache ok
//    polyModeOK_p        Bool               Polynomial mode cache ok
//    scaleFactorOK_p     Bool               Scale factor cache ok
//    validDomainOK_p     Bool               Valid domain cache ok
//    nPolyAmpOK_p        Bool               Poly. degree (amp) cache ok
//    nPolyPhaseOK_p      Bool               Poly. degree (phase) cache ok
//    polyCoeffAmpOK_p    Bool               Poly. coeff. (amp) cache ok
//    polyCoeffPhaseOK_p  Bool               Poly. coeff. (phase) cache ok
//    phaseUnitsOK_p      Bool               Phase units cache ok
//    sideBandRefOK_p     Bool               Sideband ref. cache ok
// Output:
//    nRow                Int                Maximum number of rows
//
  // Extend the inherited parent class method
  Int nRowParent = SolvableVisJonesMBuf::nRow();

  // Process each local column individually
  Vector<Int> colLength(11);
  Int n = 0;
  colLength(n++) = nRowParent;
  colLength(n++) = polyType().nelements();
  colLength(n++) = polyMode().nelements();
  colLength(n++) = scaleFactor().nelements();
  colLength(n++) = validDomain().shape().nelements() > 0 ?
    validDomain().shape().getLast(1)(0) : 0;
  colLength(n++) = nPolyAmp().nelements();
  colLength(n++) = nPolyPhase().nelements();
  colLength(n++) = polyCoeffAmp().shape().nelements() > 0 ?
    polyCoeffAmp().shape().getLast(1)(0) : 0;
  colLength(n++) = polyCoeffPhase().shape().nelements() > 0 ?
    polyCoeffPhase().shape().getLast(1)(0) : 0;
  colLength(n++) = phaseUnits().nelements();
  colLength(n++) = sideBandRef().nelements();

  return max(colLength);
};

//----------------------------------------------------------------------------

Bool BJonesPolyMBuf::putAntGain (const Int& antennaId, 
				 const String& sFreqGrpName,
				 const String& sPolyType, 
				 const Complex& sScaleFactor,
				 const Vector<Double>& sValidDomain,
				 const Int& sNPolyAmp, const Int& sNPolyPhase, 
				 const Vector<Double>& sPolyCoeffAmp,
				 const Vector<Double>& sPolyCoeffPhase,
				 const String& sPhaseUnits,
				 const Complex& sSideBandRef, 
				 const MFrequency& sRefFreq,
				 const Int& sRefAnt)
{
// Update the parametrized solution for a given antenna id.
// Input:
//    antennaId           const Int&             Antenna id. to use as key;
//                                               all subsequent parameters are
//                                               solution attributes
//    sFreqGrpName        const String&          Freq. group name 
//    sPolyType           const String&          Polynomial type
//    sScaleFactor        const Complex&         Polynomial scale factor
//    sValidDomain        const Vector<Double>&  Valid range [x_0, x_1]
//    sNPolyAmp           const Int&             Poly. degree (amp)
//    sNPolyPhase         const Int&             Poly. degree (phase)
//    sPolyCoeffAmp       const Vector<Double>&  Poly. coeff. (amp)
//    sPolyCoeffPhase     const Vector<Double>&  Poly. coeff. (phase)
//    sPhaseUnits         const String&          Phase units
//    sSideBandRef        const Complex&         Sideband reference factor
//    sRefFreq            const MFrequency&      Reference frequency
//    sRefAnt             const Int &            Reference antenna id.
//
  // Initialization
  Bool retval = false;

  // Find all calibration buffer rows for the antenna 1 id.
  Vector<Int> matchingRows = matchAntenna1(antennaId);
  Int nMatch = matchingRows.nelements();

  if (nMatch > 0) {
    retval = true;
    // Update each matched row
    for (Int i=0; i < nMatch; i++) {
      uInt row = matchingRows(i);
      freqGrpName()(row) = sFreqGrpName;
      polyType()(row) = sPolyType;
      polyMode()(row) = "A&P";
      scaleFactor()(row) = sScaleFactor;
      for (uInt pos=0; pos < 2; pos++) {
	IPosition domainPos(2, pos, row);
	validDomain()(domainPos) = sValidDomain(pos);
      };
      nPolyAmp()(row) = sNPolyAmp;
      nPolyPhase()(row) = sNPolyPhase;

      // Resize the coefficient arrays
      IPosition ampCoeffShape = polyCoeffAmp().shape();
      if (ampCoeffShape(3) != 2*sNPolyAmp) {
	ampCoeffShape(3) = 2*sNPolyAmp;
	polyCoeffAmp().resize(ampCoeffShape);
      };
      IPosition phaseCoeffShape = polyCoeffPhase().shape();
      if (phaseCoeffShape(3) != 2*sNPolyPhase) {
	phaseCoeffShape(3) = 2*sNPolyPhase;
	polyCoeffPhase().resize(phaseCoeffShape);
      };

      // Update all array elements
      for (Int recep=0; recep < ampCoeffShape(0); recep++) {
	for (Int spw=0; spw < ampCoeffShape(1); spw++) {
	  for (Int chan=0; chan < ampCoeffShape(2); chan++) {
	    IPosition ipos4(4, recep, spw, chan, row);
	    // Reference frequency and antenna
	    refFreqMeas()(ipos4) = sRefFreq;
	    refAnt()(ipos4) = sRefAnt;

	    // Amplitude polynomial coefficients
	    for (Int coeff=0; coeff < 2*sNPolyAmp; coeff++) {
	      IPosition ipos5(5, recep, spw, chan, coeff, row);
	      polyCoeffAmp()(ipos5) = sPolyCoeffAmp(coeff);
	    };

	    // Phase polynomial coefficients
	    for (Int coeff=0; coeff < 2*sNPolyPhase; coeff++) {
	      IPosition ipos5(5, recep, spw, chan, coeff, row);
	      polyCoeffPhase()(ipos5) = sPolyCoeffPhase(coeff);
	    };
	  };
	};
      };

      // Phase units
      phaseUnits()(row) = sPhaseUnits;

      // Sideband reference
      sideBandRef()(row) = sSideBandRef;
    };
  };
  return retval;
};

//----------------------------------------------------------------------------

Vector<String>& BJonesPolyMBuf::polyType()
{
// POLY_TYPE data field accessor
// Input from private data:
//    polyType_p          Vector<String>     Polynomial type
//    polyTypeOK_p        Bool               Polynomial type cache ok
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!polyTypeOK_p) {
      calMainCol()->polyType().getColumn (polyType_p);
      polyTypeOK_p = true;
    };
  };
  return polyType_p;
};

//----------------------------------------------------------------------------

Vector<String>& BJonesPolyMBuf::polyMode()
{
// POLY_MODE data field accessor
// Input from private data:
//    polyMode_p          Vector<String>     Polynomial mode (e.g. A&P)
//    polyModeOK_p        Bool               Polynomial mode cache ok
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!polyModeOK_p) {
      calMainCol()->polyMode().getColumn (polyMode_p);
      polyModeOK_p = true;
    };
  };
  return polyMode_p;
};

//----------------------------------------------------------------------------

Vector<Complex>& BJonesPolyMBuf::scaleFactor()
{
// SCALE_FACTOR data field accessor
// Input from private data:
//    scaleFactor_p       Vector<Complex>    Polynomial scale factor
//    scaleFactorOK_p     Bool               Polynomial scale factor cache ok
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!scaleFactorOK_p) {
      calMainCol()->scaleFactor().getColumn (scaleFactor_p);
      scaleFactorOK_p = true;
    };
  };
  return scaleFactor_p;
};

//----------------------------------------------------------------------------

Array<Double>& BJonesPolyMBuf::validDomain()
{
// VALID_DOMAIN data field accessor
// Input from private data:
//    validDomain_p      Array<Double>      Valid domain [x_0, x_1]
//    validDomainOK_p    Bool               Valid domain cache ok
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!validDomainOK_p) {
      calMainCol()->validDomain().getColumn (validDomain_p);
      validDomainOK_p = true;
    };
  };
  return validDomain_p;
};

//----------------------------------------------------------------------------

Vector<Int>& BJonesPolyMBuf::nPolyAmp()
{
// N_POLY_AMP data field accessor
// Input from private data:
//    nPolyAmp_p          Vector<Int>        Polynomial degree (amplitude)
//    nPolyAmpOK_p        Bool               Poly. degree (amp) cache ok
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!nPolyAmpOK_p) {
      calMainCol()->nPolyAmp().getColumn (nPolyAmp_p);
      nPolyAmpOK_p = true;
    };
  };
  return nPolyAmp_p;
};

//----------------------------------------------------------------------------

Vector<Int>& BJonesPolyMBuf::nPolyPhase()
{
// N_POLY_PHASE data field accessor
// Input from private data:
//    nPolyPhase_p          Vector<Int>        Polynomial degree (phase)
//    nPolyPhaseOK_p        Bool               Poly. degree (phase) cache ok
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!nPolyPhaseOK_p) {
      calMainCol()->nPolyPhase().getColumn (nPolyPhase_p);
      nPolyPhaseOK_p = true;
    };
  };
  return nPolyPhase_p;
};

//----------------------------------------------------------------------------

Array<Double>& BJonesPolyMBuf::polyCoeffAmp()
{
// POLY_COEFF_AMP data field accessor
// Input from private data:
//    polyCoeffAmp_p      Array<Double>      Polynomial coeff. (amplitude)
//    polyCoeffAmpOK_p    Bool               Poly. coeff. (amp) cache ok
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!polyCoeffAmpOK_p) {
      calMainCol()->polyCoeffAmp().getColumn (polyCoeffAmp_p);
      polyCoeffAmpOK_p = true;
    };
  };
  return polyCoeffAmp_p;
};

//----------------------------------------------------------------------------

Array<Double>& BJonesPolyMBuf::polyCoeffPhase()
{
// POLY_COEFF_PHASE data field accessor
// Input from private data:
//    polyCoeffPhase_p      Array<Double>      Polynomial coeff. (phase)
//    polyCoeffPhaseOK_p    Bool               Poly. coeff. (phase) cache ok
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!polyCoeffPhaseOK_p) {
      calMainCol()->polyCoeffPhase().getColumn (polyCoeffPhase_p);
      polyCoeffPhaseOK_p = true;
    };
  };
  return polyCoeffPhase_p;
};

//----------------------------------------------------------------------------

Vector<String>& BJonesPolyMBuf::phaseUnits()
{
// PHASE_UNITS data field accessor
// Input from private data:
//    phaseUnits_p        Vector<String>     Phase units
//    phaseUnitsOK_p      Bool               Phase units cache ok
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!phaseUnitsOK_p) {
      calMainCol()->phaseUnits().getColumn (phaseUnits_p);
      phaseUnitsOK_p = true;
    };
  };
  return phaseUnits_p;
};

//----------------------------------------------------------------------------

Vector<Complex>& BJonesPolyMBuf::sideBandRef()
{
// SIDEBAND_REF data field accessor
// Input from private data:
//    sideBandRef_p       Vector<Complex>    Sideband reference
//    sideBandRefOK_p     Bool               Sideband ref. cache ok
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!sideBandRefOK_p) {
      calMainCol()->sideBandRef().getColumn (sideBandRef_p);
      sideBandRefOK_p = true;
    };
  };
  return sideBandRef_p;
};

//----------------------------------------------------------------------------

void BJonesPolyMBuf::invalidate()
{
// Invalidate the current cache
// Output to private data:
//    polyTypeOK_p        Bool               Polynomial type cache ok
//    polyModeOK_p        Bool               Polynomial mode cache ok
//    scaleFactorOK_p     Bool               Scale factor cache ok
//    validDomainOK_p     Bool               Valid domain cache ok
//    nPolyAmpOK_p        Bool               Poly. degree (amp) cache ok
//    nPolyPhaseOK_p      Bool               Poly. degree (phase) cache ok
//    polyCoeffAmpOK_p    Bool               Poly. coeff. (amp) cache ok
//    polyCoeffPhaseOK_p  Bool               Poly. coeff. (phase) cache ok
//    phaseUnitsOK_p      Bool               Phase units cache ok
//    sideBandRefOK_p     Bool               Sideband ref. cache ok
//
  // Invalidate parent class cache
  BJonesMBuf::invalidate();

  // Set all cache flags to false
  polyTypeOK_p = false;
  polyModeOK_p = false;
  scaleFactorOK_p = false;
  validDomainOK_p = false;
  nPolyAmpOK_p = false;
  nPolyPhaseOK_p = false;
  polyCoeffAmpOK_p = false;
  polyCoeffPhaseOK_p = false;
  phaseUnitsOK_p = false;
  sideBandRefOK_p = false;
};

//----------------------------------------------------------------------------

  void BJonesPolyMBuf::fillAttributes (const Vector<Int>& /*calIndices*/)
{
// Resize all BJonesPoly attribute columns and set to their default values
// Input:
//    calIndices         const Vector<Int>&   Vector of cal indices, specified
//                                            as enums from class MSCalEnums,
//                                            to exclude
// Output to private data:
//    polyType_p          Vector<String>     Polynomial type
//    polyMode_p          Vector<String>     Polynomial mode (e.g. A&P)
//    scaleFactor_p       Vector<Complex>    Polynomial scale factor
//    validDomain_p       Array<Double>      Valid range [x_0, x_1]
//    nPolyAmp_p          Vector<Int>        Polynomial degree (amplitude)
//    nPolyPhase_p        Vector<Int>        Polynomial degree (phase)
//    polyCoeffAmp_p      Array<Double>      Polynomial coeff. (amplitude)
//    polyCoeffPhase_p    Array<Double>      Polynomial coeff. (phase)
//    phaseUnits_p        Vector<String>     Phase units
//    sideBandRef_p       Vector<Complex>    Sideband reference
//
  // Process each BJonesPoly cal buffer column separately
  //
  // Use the maximum number of rows currently defined in the cal buffer
  uInt nrow = nRow();

  // POLY_TYPE
  polyType().resize(nrow);
  polyType() = "";

  // POLY_MODE
  polyMode().resize(nrow);
  polyMode() = "";

  // SCALE_FACTOR
  scaleFactor().resize(nrow);
  scaleFactor() = Complex(1,0);

  // VALID_DOMAIN
  IPosition domainSize(2, 2, nrow);
  validDomain().resize(domainSize);
  validDomain() = 0;

  // N_POLY_AMP
  nPolyAmp().resize(nrow);
  nPolyAmp() = 0;

  // NPOLY_PHASE
  nPolyPhase().resize(nrow);
  nPolyPhase() = 0;

  // Array-based columns POLY_COEFF_AMP and POLY_COEFF_PHASE (set 
  // to default unit length in each dimension)
  uInt numSpw = 1;
  uInt numChan = 1;
  uInt numReceptors = 1;
  uInt numCoeff = 1;
  IPosition coeffSize(5, numReceptors, numSpw, numChan, numCoeff, nrow);

  polyCoeffAmp().resize(coeffSize);
  polyCoeffAmp() = 0;
  polyCoeffPhase().resize(coeffSize);
  polyCoeffPhase() = 0;

  // PHASE_UNITS
  phaseUnits().resize(nrow);
  phaseUnits() = "";

  // SIDEBAND_REF
  sideBandRef().resize(nrow);
  sideBandRef() = Complex(1.0, 0.0);

  return;
};

//----------------------------------------------------------------------------



} //# NAMESPACE CASA - END

