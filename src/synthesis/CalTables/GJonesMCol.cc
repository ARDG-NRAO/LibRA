//# GJonesMCol.cc: Implementation of GJonesMCol.h
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

#include <synthesis/CalTables/GJonesMCol.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

ROGJonesPolyMCol::ROGJonesPolyMCol (const GJonesPolyTable& gjpTable) :
  ROGJonesMCol (gjpTable)
{
// Construct from a calibration table
// Input:
//    gjpTable          const GJonesPolyTable&      GJonesPoly cal. table
// Output to private data:
//    ROGJonesMCol            ROGJonesMCol          Read-only GJones cal 
//                                                  main columns
//    polyType_p              ROScalarCol<String>   Polynomial type 
//    polyMode_p              ROScalarCol<String>   Polynomial mode (e.g. A&P)
//    scaleFactor_p           ROScalarCol<Complex>  Polynomial scale factor
//    nPolyAmp_p              ROScalarCol<Int>      Polynomial degree (amp)
//    nPolyPhase_p            ROScalarCol<Int>      Polynomial degree (phase)
//    polyCoeffAmp_p          ROArrayCol<Double>    Polynomial coeff. (amp)
//    polyCoeffPhase_p        ROArrayCol<Double>    Polynomial coeff. (phase)
//    phaseUnits_p            ROScalarCol<String>   Phase units.
//
  // Attach all column accessors for additional GJonesPoly columns
  // (some are optional, depending on polynomial mode)
  attach (gjpTable, polyType_p, MSC::POLY_TYPE);
  attach (gjpTable, polyMode_p, MSC::POLY_MODE);
  attach (gjpTable, scaleFactor_p, MSC::SCALE_FACTOR);
  attach (gjpTable, nPolyAmp_p, MSC::N_POLY_AMP, true);
  attach (gjpTable, nPolyPhase_p, MSC::N_POLY_PHASE, true);
  attach (gjpTable, polyCoeffAmp_p, MSC::POLY_COEFF_AMP, true);
  attach (gjpTable, polyCoeffPhase_p, MSC::POLY_COEFF_PHASE, true);
  attach (gjpTable, phaseUnits_p, MSC::PHASE_UNITS, true);
};

//----------------------------------------------------------------------------

GJonesPolyMCol::GJonesPolyMCol (GJonesPolyTable& gjpTable) :
  GJonesMCol (gjpTable)
{
// Construct from a calibration table
// Input:
//    gjpTable                GJonesPolyTable&    GJonesPoly cal. table
// Output to private data:
//    GJonesMCol              GJonesMCol          Read-write GJones cal 
//                                                main columns
//    polyType_p              ScalarCol<String>   Polynomial type 
//    polyMode_p              ScalarCol<String>   Polynomial mode (e.g. A&P)
//    scaleFactor_p           ScalarCol<Complex>  Polynomial scale factor
//    nPolyAmp_p              ScalarCol<Int>      Polynomial degree (amp)
//    nPolyPhase_p            ScalarCol<Int>      Polynomial degree (phase)
//    polyCoeffAmp_p          ArrayCol<Double>    Polynomial coeff. (amp)
//    polyCoeffPhase_p        ArrayCol<Double>    Polynomial coeff. (phase)
//    phaseUnits_p            ScalarCol<String>   Phase units.
//
  // Attach all column accessors for additional GJonesPoly columns
  // (some are optional, depending on polynomial mode)
  attach (gjpTable, polyType_p, MSC::POLY_TYPE);
  attach (gjpTable, polyMode_p, MSC::POLY_MODE);
  attach (gjpTable, scaleFactor_p, MSC::SCALE_FACTOR);
  attach (gjpTable, nPolyAmp_p, MSC::N_POLY_AMP, true);
  attach (gjpTable, nPolyPhase_p, MSC::N_POLY_PHASE, true);
  attach (gjpTable, polyCoeffAmp_p, MSC::POLY_COEFF_AMP, true);
  attach (gjpTable, polyCoeffPhase_p, MSC::POLY_COEFF_PHASE, true);
  attach (gjpTable, phaseUnits_p, MSC::PHASE_UNITS, true);
};

//----------------------------------------------------------------------------

ROGJonesSplineMCol::ROGJonesSplineMCol (const GJonesSplineTable& gjsTable) :
  ROGJonesPolyMCol (gjsTable)
{
// Construct from a calibration table
// Input:
//    gjsTable          const GJonesSplineTable&    GJonesSpline cal. table
// Output to private data:
//    ROGJonesPolyMCol        ROGJonesMCol          Read-only GJonesPoly cal 
//                                                  main columns
//    nKnotsAmp_p             ROScalarCol<Int>      No. spline knots in amp.
//    nKnotsPhase_p           ROScalarCol<Int>      No. spline knots in phase
//    splineKnotsAmp_p        ROScalarCol<Double>   Spline knot positions (amp)
//    splineKnotsPhase_p      ROScalarCol<Double>   Spline knot posn. (phase)
//
  // Attach all column accessors for additional GJonesSpline columns
  // (all are optional, dependent on polynomial mode)
  attach (gjsTable, nKnotsAmp_p, MSC::N_KNOTS_AMP, true);
  attach (gjsTable, nKnotsPhase_p, MSC::N_KNOTS_PHASE, true);
  attach (gjsTable, splineKnotsAmp_p, MSC::SPLINE_KNOTS_AMP, true);
  attach (gjsTable, splineKnotsPhase_p, MSC::SPLINE_KNOTS_PHASE, true);
};

//----------------------------------------------------------------------------

GJonesSplineMCol::GJonesSplineMCol (GJonesSplineTable& gjsTable) :
  GJonesPolyMCol (gjsTable)
{
// Construct from a calibration table
// Input:
//    gjsTable                GJonesSplineTable&  GJonesSpline cal. table
// Output to private data:
//    GJonesMCol              GJonesMCol          Read-write GJonesPoly cal 
//                                                main columns
//    nKnotsAmp_p             ScalarCol<Int>      No. spline knots in amp.
//    nKnotsPhase_p           ScalarCol<Int>      No. spline knots in phase
//    splineKnotsAmp_p        ScalarCol<Double>   Spline knot positions (amp)
//    splineKnotsPhase_p      ScalarCol<Double>   Spline knot posn. (phase)
//
  // Attach all column accessors for additional GJonesSpline columns
  // (all are optional, dependent on polynomial mode)
  attach (gjsTable, nKnotsAmp_p, MSC::N_KNOTS_AMP, true);
  attach (gjsTable, nKnotsPhase_p, MSC::N_KNOTS_PHASE, true);
  attach (gjsTable, splineKnotsAmp_p, MSC::SPLINE_KNOTS_AMP, true);
  attach (gjsTable, splineKnotsPhase_p, MSC::SPLINE_KNOTS_PHASE, true);
};

//----------------------------------------------------------------------------


} //# NAMESPACE CASA - END

