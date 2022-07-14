//# TJonesMBuf.cc: Implementation of TJonesMBuf.h
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

#include <synthesis/CalTables/TJonesMBuf.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/tables/Tables/RefRows.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

TJonesPolyMBuf::TJonesPolyMBuf() : TJonesMBuf()
{
// Null constructor
// Output to private data:
//    TJonesMBuf          TJonesMBuf         TJones cal main buffer
//    polyType_p          Vector<String>     Polynomial type
//    polyMode_p          Vector<String>     Polynomial mode (e.g. A&P)
//    scaleFactor_p       Vector<Complex>    Polynomial scale factor
//    nPolyAmp_p          Vector<Int>        Polynomial degree (amplitude)
//    nPolyPhase_p        Vector<Int>        Polynomial degree (phase)
//    polyCoeffAmp_p      Array<Double>      Polynomial coeff. (amplitude)
//    polyCoeffPhase_p    Array<Double>      Polynomial coeff. (phase)
//    phaseUnits_p        Vector<String>     Phase units
//    polyTypeOK_p        Bool               Polynomial type cache ok
//    polyModeOK_p        Bool               Polynomial mode cache ok
//    scaleFactorOK_p     Bool               Scale factor cache ok
//    nPolyAmpOK_p        Bool               Poly. degree (amp) cache ok
//    nPolyPhaseOK_p      Bool               Poly. degree (phase) cache ok
//    polyCoeffAmpOK_p    Bool               Poly. coeff. (amp) cache ok
//    polyCoeffPhaseOK_p  Bool               Poly. coeff. (phase) cache ok
//    phaseUnitsOK_p      Bool               Phase units cache ok
//
  // Invalidate cache
  invalidate();
};

//----------------------------------------------------------------------------

TJonesPolyMBuf::TJonesPolyMBuf (const Vector<Int>& calIndices,
				const Block<Vector<Int> >& indexValues) : 
  TJonesMBuf (calIndices, indexValues)
{
// Construct from a set of cal buffer indices and specified index values
// Output to private data:
//    TJonesMBuf       TJonesMBuf         Parent class cal main table buffer
//
  // Set the local non-index columns to default values
  fillAttributes (calIndices);
};

//----------------------------------------------------------------------------

TJonesPolyMBuf::TJonesPolyMBuf (CalIterBase& calIter) : TJonesMBuf (calIter)
{
// Construct from a calibration table iterator
// Input:
//    calIter             CalIterBase&       Calibration table iterator
// Output to private data:
//    TJonesMBuf          TJonesMBuf         TJones cal main buffer
//    polyType_p          Vector<String>     Polynomial type
//    polyMode_p          Vector<String>     Polynomial mode (e.g. A&P)
//    scaleFactor_p       Vector<Complex>    Polynomial scale factor
//    nPolyAmp_p          Vector<Int>        Polynomial degree (amplitude)
//    nPolyPhase_p        Vector<Int>        Polynomial degree (phase)
//    polyCoeffAmp_p      Array<Double>      Polynomial coeff. (amplitude)
//    polyCoeffPhase_p    Array<Double>      Polynomial coeff. (phase)
//    phaseUnits_p        Vector<String>     Phase units
//    polyTypeOK_p        Bool               Polynomial type cache ok
//    polyModeOK_p        Bool               Polynomial mode cache ok
//    scaleFactorOK_p     Bool               Scale factor cache ok
//    nPolyAmpOK_p        Bool               Poly. degree (amp) cache ok
//    nPolyPhaseOK_p      Bool               Poly. degree (phase) cache ok
//    polyCoeffAmpOK_p    Bool               Poly. coeff. (amp) cache ok
//    polyCoeffPhaseOK_p  Bool               Poly. coeff. (phase) cache ok
//    phaseUnitsOK_p      Bool               Phase units cache ok
//
  // Invalidate cache
  invalidate();
};

//----------------------------------------------------------------------------

void TJonesPolyMBuf::invalidate()
{
// Invalidate the current cache
// Output to private data:
//    polyTypeOK_p        Bool               Polynomial type cache ok
//    polyModeOK_p        Bool               Polynomial mode cache ok
//    scaleFactorOK_p     Bool               Scale factor cache ok
//    nPolyAmpOK_p        Bool               Poly. degree (amp) cache ok
//    nPolyPhaseOK_p      Bool               Poly. degree (phase) cache ok
//    polyCoeffAmpOK_p    Bool               Poly. coeff. (amp) cache ok
//    polyCoeffPhaseOK_p  Bool               Poly. coeff. (phase) cache ok
//    phaseUnitsOK_p      Bool               Phase units cache ok
//
  // Invalidate parent class cache
  TJonesMBuf::invalidate();

  // Set all cache flags to false
  polyTypeOK_p = false;
  polyModeOK_p = false;
  scaleFactorOK_p = false;
  nPolyAmpOK_p = false;
  nPolyPhaseOK_p = false;
  polyCoeffAmpOK_p = false;
  polyCoeffPhaseOK_p = false;
  phaseUnitsOK_p = false;
};

//----------------------------------------------------------------------------

Int TJonesPolyMBuf::append (CalTable& calTable)
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
  TJonesPolyMCol gjpMainCol(dynamic_cast<TJonesPolyTable&>(calTable));

  // Append the current cal buffer main columns
  RefRows refRows(startRow, endRow);
  gjpMainCol.polyType().putColumnCells(refRows, polyType());
  gjpMainCol.polyMode().putColumnCells(refRows, polyMode());
  gjpMainCol.scaleFactor().putColumnCells(refRows, scaleFactor());
  gjpMainCol.nPolyAmp().putColumnCells(refRows, nPolyAmp());
  gjpMainCol.nPolyPhase().putColumnCells(refRows, nPolyPhase());
  gjpMainCol.polyCoeffAmp().putColumnCells(refRows, polyCoeffAmp());
  gjpMainCol.polyCoeffPhase().putColumnCells(refRows, polyCoeffPhase());
  gjpMainCol.phaseUnits().putColumnCells(refRows, phaseUnits());

  return nAdded;
};

//----------------------------------------------------------------------------

Int TJonesPolyMBuf::nRow()
{
// Return the maximum number of rows in the calibration buffer
// Input from private data:
//    polyType_p          Vector<String>     Polynomial type
//    polyMode_p          Vector<String>     Polynomial mode (e.g. A&P)
//    scaleFactor_p       Vector<Complex>    Polynomial scale factor
//    nPolyAmp_p          Vector<Int>        Polynomial degree (amplitude)
//    nPolyPhase_p        Vector<Int>        Polynomial degree (phase)
//    polyCoeffAmp_p      Array<Double>      Polynomial coeff. (amplitude)
//    polyCoeffPhase_p    Array<Double>      Polynomial coeff. (phase)
//    phaseUnits_p        Vector<String>     Phase units
//    polyTypeOK_p        Bool               Polynomial type cache ok
//    polyModeOK_p        Bool               Polynomial mode cache ok
//    scaleFactorOK_p     Bool               Scale factor cache ok
//    nPolyAmpOK_p        Bool               Poly. degree (amp) cache ok
//    nPolyPhaseOK_p      Bool               Poly. degree (phase) cache ok
//    polyCoeffAmpOK_p    Bool               Poly. coeff. (amp) cache ok
//    polyCoeffPhaseOK_p  Bool               Poly. coeff. (phase) cache ok
//    phaseUnitsOK_p      Bool               Phase units cache ok
// Output:
//    nRow                Int                Maximum number of rows
//
  // Extend the inherited parent class method
  Int nRowParent = SolvableVisJonesMBuf::nRow();

  // Process each local column individually
  Vector<Int> colLength(9);
  Int n = 0;
  colLength(n++) = nRowParent;
  colLength(n++) = polyType().nelements();
  colLength(n++) = polyMode().nelements();
  colLength(n++) = scaleFactor().nelements();
  colLength(n++) = nPolyAmp().nelements();
  colLength(n++) = nPolyPhase().nelements();
  colLength(n++) = polyCoeffAmp().shape().nelements() > 0 ?
    polyCoeffAmp().shape().getLast(1)(0) : 0;
  colLength(n++) = polyCoeffPhase().shape().nelements() > 0 ?
    polyCoeffPhase().shape().getLast(1)(0) : 0;
  colLength(n++) = phaseUnits().nelements();

  return max(colLength);
};

//----------------------------------------------------------------------------

Bool TJonesPolyMBuf::fillMatchingRows (const Vector<Int>& matchingRows,
				       const String& sFreqGrpName,
				       const String& sPolyType, 
				       const String& sPolyMode,
				       const Complex& sScaleFactor,
				       const Int& sNPolyAmp, 
				       const Int& sNPolyPhase, 
				       const Vector<Double>& sPolyCoeffAmp,
				       const Vector<Double>& sPolyCoeffPhase,
				       const String& sPhaseUnits,
				       const MFrequency& sRefFreq,
				       const Int& sRefAnt)
{
// Update the parametrized solution in each of a set of buffer rows
// Input:
//    matchingRows        const Vec<Int>&        Buffer rows to update
//    sFreqGrpName        const String&          Freq. group name 
//    sPolyType           const String&          Polynomial type
//    sPolyMode           const String&          Polynomial mode (e.g. A&P)
//    sScaleFactor        const Complex&         Polynomial scale factor
//    sNPolyAmp           const Int&             Poly. degree (amp)
//    sNPolyPhase         const Int&             Poly. degree (phase)
//    sPolyCoeffAmp       const Vector<Double>&  Poly. coeff. (amp)
//    sPolyCoeffPhase     const Vector<Double>&  Poly. coeff. (phase)
//    sPhaseUnits         const String&          Phase units
//    sRefFreq            const MFrequency&      Reference frequency
//    sRefAnt             const Int &            Reference antenna id.
//
  // Initialization
  Bool retval = false;
  Int nMatch = matchingRows.nelements();

  if (nMatch > 0) {
    retval = true;
    // Update each matched row
    for (Int i=0; i < nMatch; i++) {
      uInt row = matchingRows(i);
      freqGrpName()(row) = sFreqGrpName;
      polyType()(row) = sPolyType;

      // Check if the polynomial mode needs to be updated
      String currMode = polyMode()(row);
      if ((currMode.contains("AMP") && sPolyMode.contains("PHAS")) ||
	  (currMode.contains("PHAS") && sPolyMode.contains("AMP"))) {
	polyMode()(row) = "A&P";
      } else {
	polyMode()(row) = sPolyMode;
      };

      Complex currFactor = scaleFactor()(row);
      scaleFactor()(row) = 
	abs(currFactor) > 0 ? currFactor*sScaleFactor : sScaleFactor;
      nPolyAmp()(row) = sNPolyAmp;
      nPolyPhase()(row) = sNPolyPhase;

      // Resize the coefficient arrays
      IPosition ampCoeffShape = polyCoeffAmp().shape();
      if (ampCoeffShape(3) != sNPolyAmp) {
	ampCoeffShape(3) = sNPolyAmp;
	polyCoeffAmp().resize(ampCoeffShape);
      };
      IPosition phaseCoeffShape = polyCoeffPhase().shape();
      if (phaseCoeffShape(3) != sNPolyPhase) {
	IPosition phaseCoeffShape = polyCoeffPhase().shape();
	phaseCoeffShape(3) = sNPolyPhase;
	polyCoeffPhase().resize(phaseCoeffShape);
	polyCoeffPhase() = 0;
      };

      // Update all array elements
      for (Int recep=0; recep < ampCoeffShape(0); recep++) {
	for (Int spw=0; spw < ampCoeffShape(1); spw++) {
	  for (Int chan=0; chan < ampCoeffShape(2); chan++) {
	    IPosition ipos4(4, recep, spw, chan, row);
	    // Compute a phase scale factor if the reference
	    // frequency has changed 
	    Double phaseScaleFactor = 0;
	    Double refFreqHz = refFreqMeas()(ipos4).get("Hz").getValue();
	    Double sRefFreqHz = sRefFreq.get("Hz").getValue();
	    if (std::abs(refFreqHz) > 0 && std::abs(sRefFreqHz) > 0) {
	      phaseScaleFactor = sRefFreqHz / refFreqHz;
	    };

	    // Reference frequency and antenna
	    refFreqMeas()(ipos4) = sRefFreq;
	    refAnt()(ipos4) = sRefAnt;

	    // Amplitude polynomial coefficients
	    for (Int coeff=0; coeff < sNPolyAmp; coeff++) {
	      IPosition ipos5(5, recep, spw, chan, coeff, row);
	      polyCoeffAmp()(ipos5) = sPolyCoeffAmp(coeff);
	    };

	    // Phase polynomial coefficients
	    for (Int coeff=0; coeff < sNPolyPhase; coeff++) {
	      IPosition ipos5(5, recep, spw, chan, coeff, row);
	      // Multiply exisiting coefficients by the phase scale factor
	      polyCoeffPhase()(ipos5) *= phaseScaleFactor;
	      polyCoeffPhase()(ipos5) += sPolyCoeffPhase(coeff);
	    };
	  };
	};
      };

      // Phase units
      phaseUnits()(row) = sPhaseUnits;
    };
  };
  return retval;
};

//----------------------------------------------------------------------------

Vector<String>& TJonesPolyMBuf::polyType()
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

Vector<String>& TJonesPolyMBuf::polyMode()
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

Vector<Complex>& TJonesPolyMBuf::scaleFactor()
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

Vector<Int>& TJonesPolyMBuf::nPolyAmp()
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

Vector<Int>& TJonesPolyMBuf::nPolyPhase()
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

Array<Double>& TJonesPolyMBuf::polyCoeffAmp()
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

Array<Double>& TJonesPolyMBuf::polyCoeffPhase()
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

Vector<String>& TJonesPolyMBuf::phaseUnits()
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

  void TJonesPolyMBuf::fillAttributes (const Vector<Int>& /*calIndices*/)
{
// Resize all TJonesPoly attribute columns and set to their default values
// Input:
//    calIndices         const Vector<Int>&   Vector of cal indices, specified
//                                            as enums from class MSCalEnums,
//                                            to exclude
// Output to private data:
//    polyType_p          Vector<String>     Polynomial type
//    polyMode_p          Vector<String>     Polynomial mode (e.g. A&P)
//    scaleFactor_p       Vector<Complex>    Polynomial scale factor
//    nPolyAmp_p          Vector<Int>        Polynomial degree (amplitude)
//    nPolyPhase_p        Vector<Int>        Polynomial degree (phase)
//    polyCoeffAmp_p      Array<Double>      Polynomial coeff. (amplitude)
//    polyCoeffPhase_p    Array<Double>      Polynomial coeff. (phase)
//    phaseUnits_p        Vector<String>     Phase units
//
  // Process each TJonesPoly cal buffer column separately
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

  return;
};

//----------------------------------------------------------------------------

TJonesSplineMBuf::TJonesSplineMBuf() : TJonesPolyMBuf()
{
// Null constructor
// Output to private data:
//    TJonesPolyMBuf        TJonesPolyMBuf    TJonesPoly cal main buffer
//    nKnotsAmp_p           Vector<Int>       No. of amplitude spline knots
//    nKnotsPhase_p         Vector<Int>       No. of phase spline knots
//    splineKnotsAmp_p      Array<Double>     Amp. spline knot positions
//    splineKnotsPhase_p    Array<Double>     Phase spline knot positions
//    nKnotsAmpOK_p         Bool              No. amp. spline knots cache ok
//    nKnotsPhaseOK_p       Bool              No. phase spline knots cache ok
//    splineKnotsAmpOK_p    Bool              Amp. spline knot posn. cache ok
//    splineKnotsPhaseOK_p  Bool              Phase spline knot posn. cache ok
//
  // Invalidate cache
  invalidate();
};

//----------------------------------------------------------------------------

TJonesSplineMBuf::TJonesSplineMBuf (const Vector<Int>& calIndices,
				    const Block<Vector<Int> >& indexValues) : 
  TJonesPolyMBuf (calIndices, indexValues)
{
// Construct from a set of cal buffer indices and specified index values
// Output to private data:
//    TJonesPolyMBuf      TJonesPolyMBuf     Parent class cal main table buffer
//
  // Set the local non-index columns to default values
  fillAttributes (calIndices);
};

//----------------------------------------------------------------------------

TJonesSplineMBuf::TJonesSplineMBuf (CalIterBase& calIter) : 
  TJonesPolyMBuf (calIter)
{
// Construct from a calibration table iterator
// Input:
//    calIter             CalIterBase&       Calibration table iterator
// Output to private data:
//    TJonesPolyMBuf        TJonesPolyMBuf    TJonesPoly cal main buffer
//    nKnotsAmp_p           Vector<Int>       No. of amplitude spline knots
//    nKnotsPhase_p         Vector<Int>       No. of phase spline knots
//    splineKnotsAmp_p      Array<Double>     Amp. spline knot positions
//    splineKnotsPhase_p    Array<Double>     Phase spline knot positions
//    nKnotsAmpOK_p         Bool              No. amp. spline knots cache ok
//    nKnotsPhaseOK_p       Bool              No. phase spline knots cache ok
//    splineKnotsAmpOK_p    Bool              Amp. spline knot posn. cache ok
//    splineKnotsPhaseOK_p  Bool              Phase spline knot posn. cache ok
//
  // Invalidate cache
  invalidate();
};

//----------------------------------------------------------------------------

void TJonesSplineMBuf::invalidate()
{
// Invalidate the current cache
// Output to private data:
//    nKnotsAmpOK_p         Bool              No. amp. spline knots cache ok
//    nKnotsPhaseOK_p       Bool              No. phase spline knots cache ok
//    splineKnotsAmpOK_p    Bool              Amp. spline knot posn. cache ok
//    splineKnotsPhaseOK_p  Bool              Phase spline knot posn. cache ok
//
  // Invalidate parent class cache
  TJonesPolyMBuf::invalidate();

  // Set all cache flags to false
  nKnotsAmpOK_p = false;
  nKnotsPhaseOK_p = false;
  splineKnotsAmpOK_p = false;
  splineKnotsPhaseOK_p = false;
};

//----------------------------------------------------------------------------

Int TJonesSplineMBuf::append (CalTable& calTable)
{
// Append the current calibration buffer to a calibration table
// Input:
//    calTable            CalTable&          Calibration table
//
  // Extend the inherited parent class method
  Int nAdded = TJonesPolyMBuf::append(calTable);

  // Compute the row numbers already added by the parent class
  uInt endRow = calTable.nRowMain() - 1;
  uInt startRow = endRow - nAdded + 1;

  // Attach a calibration table columns accessor
  TJonesSplineMCol gjsMainCol(dynamic_cast<TJonesSplineTable&>(calTable));

  // Append the current cal buffer main columns
  RefRows refRows(startRow, endRow);
  gjsMainCol.nKnotsAmp().putColumnCells(refRows, nKnotsAmp());
  gjsMainCol.nKnotsPhase().putColumnCells(refRows, nKnotsPhase());
  gjsMainCol.splineKnotsAmp().putColumnCells(refRows, splineKnotsAmp());
  gjsMainCol.splineKnotsPhase().putColumnCells(refRows, splineKnotsPhase());

  return nAdded;
};

//----------------------------------------------------------------------------

Int TJonesSplineMBuf::nRow()
{
// Return the maximum number of rows in the calibration buffer
// Input from private data:
//    nKnotsAmp_p           Vector<Int>       No. of amplitude spline knots
//    nKnotsPhase_p         Vector<Int>       No. of phase spline knots
//    splineKnotsAmp_p      Array<Double>     Amp. spline knot positions
//    splineKnotsPhase_p    Array<Double>     Phase spline knot positions
//    nKnotsAmpOK_p         Bool              No. amp. spline knots cache ok
//    nKnotsPhaseOK_p       Bool              No. phase spline knots cache ok
//    splineKnotsAmpOK_p    Bool              Amp. spline knot posn. cache ok
//    splineKnotsPhaseOK_p  Bool              Phase spline knot posn. cache ok
// Output:
//    nRow                  Int               Maximum number of rows
//
  // Extend the inherited parent class method
  Int nRowParent = TJonesPolyMBuf::nRow();

  // Process each local column individually
  Vector<Int> colLength(5);
  Int n = 0;
  colLength(n++) = nRowParent;
  colLength(n++) = nKnotsAmp().nelements();
  colLength(n++) = nKnotsPhase().nelements();
  colLength(n++) = splineKnotsAmp().shape().nelements() > 0 ?
    splineKnotsAmp().shape().getLast(1)(0) : 0;
  colLength(n++) = splineKnotsPhase().shape().nelements() > 0 ?
    splineKnotsPhase().shape().getLast(1)(0) : 0;

  return max(colLength);
};

//----------------------------------------------------------------------------

Bool TJonesSplineMBuf::fillMatchingRows (const Vector<Int>& matchingRows,
					 const String& sFreqGrpName,
					 const String& sPolyType, 
					 const String& sPolyMode,
					 const Complex& sScaleFactor,
					 const Int& sNPolyAmp, 
					 const Int& sNPolyPhase, 
					 const Vector<Double>& sPolyCoeffAmp,
					 const Vector<Double>& sPolyCoeffPhase,
					 const String& sPhaseUnits,
					 const Int& sNKnotsAmp,
					 const Int& sNKnotsPhase,
					 const Vector<Double>& sSplineKnotsAmp,
					 const Vector<Double>& 
					 sSplineKnotsPhase,
					 const MFrequency& sRefFreq,
					 const Int& sRefAnt)
{
// Update the parametrized solution in each of a set of buffer rows
// Input:
//    matchingRows        const Vector<Int>&     Buffer rows to update
//    sFreqGrpName        const String&          Freq. group name 
//    sPolyType           const String&          Polynomial type
//    sPolyMode           const String&          Polynomial mode (e.g. A&P)
//    sScaleFactor        const Complex&         Polynomial scale factor
//    sNPolyAmp           const Int&             Poly. degree (amp)
//    sNPolyPhase         const Int&             Poly. degree (phase)
//    sPolyCoeffAmp       const Vector<Double>&  Poly. coeff. (amp)
//    sPolyCoeffPhase     const Vector<Double>&  Poly. coeff. (phase)
//    sPhaseUnits         const String&          Phase units
//    sNKnotsAmp          const Int&             No. of amp. spline knots
//    sNKnotsPhase        const Int&             No. of phase spline knots
//    sSplineKnotsAmp     const Vector<Double>&  Amp. spline knot positions
//    sSplineKnotsPhase   const Vector<Double>&  Phase spline knot positions
//    sRefFreq            const MFrequency&      Reference frequency
//    sRefAnt             const Int &            Reference antenna id.
//
  // Initialization
  Bool retval = false;

  // Extend parent method
  TJonesPolyMBuf::fillMatchingRows (matchingRows, sFreqGrpName, sPolyType, 
				    sPolyMode, sScaleFactor, sNPolyAmp, 
				    sNPolyPhase, sPolyCoeffAmp, 
				    sPolyCoeffPhase, sPhaseUnits, 
				    sRefFreq, sRefAnt);

  // Add local spline parameters.
  //
  Int nMatch = matchingRows.nelements();

  if (nMatch > 0) {
    retval = true;
    // Update each matched row
    for (Int i=0; i < nMatch; i++) {
      uInt row = matchingRows(i);

      // No of knots in amplitude and phase splines
      nKnotsAmp()(row) = sNKnotsAmp;
      nKnotsPhase()(row) = sNKnotsPhase;
    
      // Resize the spline knot arrays in ampltidue and phase
      IPosition splineKnotsAmpShape = splineKnotsAmp().shape();
      if (splineKnotsAmpShape(3) != sNKnotsAmp) {
	splineKnotsAmpShape(3) = sNKnotsAmp;
	splineKnotsAmp().resize(splineKnotsAmpShape);
      };
      IPosition splineKnotsPhaseShape = splineKnotsPhase().shape();
      if (splineKnotsPhaseShape(3) != sNKnotsPhase) {
	splineKnotsPhaseShape(3) = sNKnotsPhase;
	splineKnotsPhase().resize(splineKnotsPhaseShape);
      };

      // Update all array elements
      for (Int recep=0; recep < splineKnotsAmpShape(0); recep++) {
	for (Int spw=0; spw < splineKnotsAmpShape(1); spw++) {
	  for (Int chan=0; chan < splineKnotsAmpShape(2); chan++) {

	    // Amplitude spline knot positions
	    for (Int coeff=0; coeff < sNKnotsAmp; coeff++) {
	      IPosition ipos5(5, recep, spw, chan, coeff, row);
	      splineKnotsAmp()(ipos5) = sSplineKnotsAmp(coeff);
	    };

	    // Phase spline knot positions
	    for (Int coeff=0; coeff < sNKnotsPhase; coeff++) {
	      IPosition ipos5(5, recep, spw, chan, coeff, row);
	      splineKnotsPhase()(ipos5) = sSplineKnotsPhase(coeff);
	    };
	  };
	};
      };
    };
  };
  return retval;
};

//----------------------------------------------------------------------------

Vector<Int>& TJonesSplineMBuf::nKnotsAmp()
{
// N_KNOTS_AMP data field accessor
// Input from private data:
//    nKnotsAmp_p          Vector<Int>       No. of amp. spline knots
//    nKnotsAmpOK_p        Bool              No. amp. spline knots cache ok
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!nKnotsAmpOK_p) {
      calMainCol()->nKnotsAmp().getColumn (nKnotsAmp_p);
      nKnotsAmpOK_p = true;
    };
  };
  return nKnotsAmp_p;
};

//----------------------------------------------------------------------------

Vector<Int>& TJonesSplineMBuf::nKnotsPhase()
{
// N_KNOTS_PHASE data field accessor
// Input from private data:
//    nKnotsPhase_p          Vector<Int>     No. phase spline knots
//    nKnotsPhaseOK_p        Bool            No. phase spline knots cache ok
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!nKnotsPhaseOK_p) {
      calMainCol()->nKnotsPhase().getColumn (nKnotsPhase_p);
      nKnotsPhaseOK_p = true;
    };
  };
  return nKnotsPhase_p;
};

//----------------------------------------------------------------------------

Array<Double>& TJonesSplineMBuf::splineKnotsAmp()
{
// SPLINE_KNOTS_AMP data field accessor
// Input from private data:
//    splineKnotsAmp_p      Array<Double>    Amp. spline knot positions
//    splineKnotsAmpOK_p    Bool             Amp. spline knot posn. cache ok
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!splineKnotsAmpOK_p) {
      calMainCol()->splineKnotsAmp().getColumn (splineKnotsAmp_p);
      splineKnotsAmpOK_p = true;
    };
  };
  return splineKnotsAmp_p;
};

//----------------------------------------------------------------------------

Array<Double>& TJonesSplineMBuf::splineKnotsPhase()
{
// SPLINE_KNOTS_PHASE data field accessor
// Input from private data:
//    splineKnotsPhase_p      Array<Double>   Phase spline knot positions
//    splineKnotsPhaseOK_p    Bool            Phase spline knot posn. cache ok
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!splineKnotsPhaseOK_p) {
      calMainCol()->splineKnotsPhase().getColumn (splineKnotsPhase_p);
      splineKnotsPhaseOK_p = true;
    };
  };
  return splineKnotsPhase_p;
};

//----------------------------------------------------------------------------

  void TJonesSplineMBuf::fillAttributes (const Vector<Int>& /*calIndices*/)
{
// Resize all TJonesSpline attribute columns and set to their default values
// Input:
//    calIndices         const Vector<Int>&   Vector of cal indices, specified
//                                            as enums from class MSCalEnums,
//                                            to exclude
// Output to private data:
//    nKnotsAmp_p           Vector<Int>       No. of amplitude spline knots
//    nKnotsPhase_p         Vector<Int>       No. of phase spline knots
//    splineKnotsAmp_p      Array<Double>     Amp. spline knot positions
//    splineKnotsPhase_p    Array<Double>     Phase spline knot positions
//
  // Process each TJonesSpline cal buffer column separately
  //
  // Use the maximum number of rows currently defined in the cal buffer
  uInt nrow = nRow();

  // N_KNOTS_AMP
  nKnotsAmp().resize(nrow);
  nKnotsAmp() = 0;

  // N_KNOTS_PHASE
  nKnotsPhase().resize(nrow);
  nKnotsPhase() = 0;

  // Array-based columns SPLINE_KNOTS_AMP and SPLINE_KNOTS_PHASE (set 
  // to default unit length in each dimension)
  uInt numSpw = 1;
  uInt numChan = 1;
  uInt numReceptors = 1;
  uInt numCoeff = 1;
  IPosition knotSize(5, numReceptors, numSpw, numChan, numCoeff, nrow);

  splineKnotsAmp().resize(knotSize);
  splineKnotsAmp() = 0;
  splineKnotsPhase().resize(knotSize);
  splineKnotsPhase() = 0;

  return;
};

//----------------------------------------------------------------------------


} //# NAMESPACE CASA - END

