//# BJonesMCol.cc: Implementation of BJonesMCol.h
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

#include <synthesis/CalTables/BJonesMCol.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

ROBJonesMCol::ROBJonesMCol (const BJonesTable& bjTable) :
  ROSolvableVisJonesMCol (bjTable)
{
// Construct from a calibration table
// Input:
//    bjTable                 const BJonesTable&      BJones calibration table
// Output to private data:
//    ROSolvableVisJonesMCol  ROSolvableVisJonesMCol  Read-only SVJ cal 
//                                                    main col
};

//----------------------------------------------------------------------------

BJonesMCol::BJonesMCol (BJonesTable& bjTable) :
  SolvableVisJonesMCol (bjTable)
{
// Construct from a calibration table
// Input:
//    bjTable                BJonesTable&           BJones calibration table
// Output to private data:
//    SolvableVisJonesMCol   SolvableVisJonesMCol   Read-write SVJ cal main col
};

//----------------------------------------------------------------------------

ROBJonesPolyMCol::ROBJonesPolyMCol (const BJonesPolyTable& bjpTable) :
  ROBJonesMCol (bjpTable)
{
// Construct from a calibration table
// Input:
//    bjpTable          const BJonesPolyTable&    BJonesPoly calibration table
// Output to private data:
//    ROBJonesMCol      ROBJonesMCol              Read-only BJones cal main col
//    polyType_p        ROScalarCol<String>       Polynomial type
//    polyMode_p        ROScalarCol<String>       Polynomial mode (e.g. A&P)
//    scaleFactor_p     ROScalarCol<Complex>      Polynomial scale factor
//    validDomain_p     ROArrayCol<Double>        Valid domain [x_0, x_1]
//    nPolyAmp_p        ROScalarCol<Int>          Polynomial degree (amplitude)
//    nPolyPhase_p      ROScalarCol<Int>          Polynomial degree (phase)
//    polyCoeffAmp_p    ROArrayCol<Double>        Polynomial coeff. (amplitude)
//    polyCoeffPhase_p  ROArrayCol<Double>        Polynomial coeff. (phase)
//    phaseUnits_p      ROScalarCol<String>       Phase units 
//    sideBandRef_p     ROScalarCol<Complex>      Sideband reference
//
  // Attach all column accessors for additional BJonesPoly columns
  // (some are optional, depending on polynomial mode)
  attach (bjpTable, polyType_p, MSC::POLY_TYPE);
  attach (bjpTable, polyMode_p, MSC::POLY_MODE);
  attach (bjpTable, scaleFactor_p, MSC::SCALE_FACTOR);
  attach (bjpTable, validDomain_p, MSC::VALID_DOMAIN, true);
  attach (bjpTable, nPolyAmp_p, MSC::N_POLY_AMP, true);
  attach (bjpTable, nPolyPhase_p, MSC::N_POLY_PHASE, true);
  attach (bjpTable, polyCoeffAmp_p, MSC::POLY_COEFF_AMP, true);
  attach (bjpTable, polyCoeffPhase_p, MSC::POLY_COEFF_PHASE, true);
  attach (bjpTable, phaseUnits_p, MSC::PHASE_UNITS, true);
  attach (bjpTable, sideBandRef_p, MSC::SIDEBAND_REF);
};

//----------------------------------------------------------------------------

BJonesPolyMCol::BJonesPolyMCol (BJonesPolyTable& bjpTable) :
  BJonesMCol (bjpTable)
{
// Construct from a calibration table
// Input:
//    bjpTable           BJonesPolyTable&       BJonesPoly calibration table
// Output to private data:
//    BJonesMCol         BJonesMCol             Read-write BJones cal main col
//    polyType_p         ScalarCol<String>      Polynomial type
//    polyMode_p         ScalarCol<String>      Polynomial mode (e.g. A&P)
//    scaleFactor_p      ScalarCol<Complex>     Polynomical scale factor
//    validDomain_p      ArrayCol<Double>       Valid domain [x_0, x_1]
//    nPolyAmp_p         ScalarCol<Int>         Polynomial degree (amplitude)
//    nPolyPhase_p       ScalarCol<Int>         Polynomial degree (phase)
//    polyCoeffAmp_p     ArrayCol<Double>       Polynomial coeff. (amplitude)
//    polyCoeffPhase_p   ArrayCol<Double>       Polynomial coeff. (phase)
//    phaseUnits_p       ScalarCol<String>      Phase units
//    sideBandRef_p      ScalarCol<Complex>     Sideband reference
//
  // Attach all column accessors for additional BJonesPoly columns
  // (some are optional, depending on polynomial mode)
  attach (bjpTable, polyType_p, MSC::POLY_TYPE);
  attach (bjpTable, polyMode_p, MSC::POLY_MODE);
  attach (bjpTable, scaleFactor_p, MSC::SCALE_FACTOR);
  attach (bjpTable, validDomain_p, MSC::VALID_DOMAIN);
  attach (bjpTable, nPolyAmp_p, MSC::N_POLY_AMP, true);
  attach (bjpTable, nPolyPhase_p, MSC::N_POLY_PHASE, true);
  attach (bjpTable, polyCoeffAmp_p, MSC::POLY_COEFF_AMP, true);
  attach (bjpTable, polyCoeffPhase_p, MSC::POLY_COEFF_PHASE, true);
  attach (bjpTable, phaseUnits_p, MSC::PHASE_UNITS, true);
  attach (bjpTable, sideBandRef_p, MSC::SIDEBAND_REF);
};

//----------------------------------------------------------------------------


} //# NAMESPACE CASA - END

