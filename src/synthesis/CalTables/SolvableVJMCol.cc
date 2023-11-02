//# SolvableVJMCol.cc: Implementation of SolvableVJMCol.h
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
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$
//----------------------------------------------------------------------------

#include <synthesis/CalTables/SolvableVJMCol.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

ROSolvableVisJonesMCol::ROSolvableVisJonesMCol (const SolvableVisJonesTable& 
						svjTable) :
  ROTimeVarVisJonesMCol (svjTable)
{
// Construct from a calibration table
// Input:
//    svjTable              const SolvableVisJonesTable&  SVJ calibration table
// Output to private data:
//    ROTimeVarVisJonesMCol ROTimeVarVisJonesMCol         Read-only TVVJ 
//                                                        cal main col
  // Attach all column accessors for additional SolvableVisJones columns
  // (all of which are required columns)
  attach (svjTable, totalSolnOk_p, MSC::TOTAL_SOLUTION_OK);
  attach (svjTable, totalFit_p, MSC::TOTAL_FIT);
  attach (svjTable, totalFitWgt_p, MSC::TOTAL_FIT_WEIGHT);
  attach (svjTable, solnOk_p, MSC::SOLUTION_OK);
  attach (svjTable, fit_p, MSC::FIT);
  attach (svjTable, fitWgt_p, MSC::FIT_WEIGHT);
  attach (svjTable, flag_p, MSC::FLAG);
  attach (svjTable, snr_p, MSC::SNR);
};

//----------------------------------------------------------------------------

SolvableVisJonesMCol::SolvableVisJonesMCol (SolvableVisJonesTable& svjTable) :
  TimeVarVisJonesMCol (svjTable)
{
// Construct from a calibration table
// Input:
//    svjTable             SolvableVisJonesTable&      SVJ calibration table
// Output to private data:
//    TimeVarVisJonesMCol  TimeVarVisJonesMCol         Read-write TVVJ 
//                                                     cal main col
  // Attach all column accessors for additional SolvableVisJones columns
  // (all of which are required columns)
  attach (svjTable, totalSolnOk_p, MSC::TOTAL_SOLUTION_OK);
  attach (svjTable, totalFit_p, MSC::TOTAL_FIT);
  attach (svjTable, totalFitWgt_p, MSC::TOTAL_FIT_WEIGHT);
  attach (svjTable, solnOk_p, MSC::SOLUTION_OK);
  attach (svjTable, fit_p, MSC::FIT);
  attach (svjTable, fitWgt_p, MSC::FIT_WEIGHT);
  attach (svjTable, flag_p, MSC::FLAG);
  attach (svjTable, snr_p, MSC::SNR);
};

//----------------------------------------------------------------------------

ROGJonesMCol::ROGJonesMCol (const GJonesTable& gjTable) :
  ROSolvableVisJonesMCol (gjTable)
{
// Construct from a calibration table
// Input:
//    gjTable                 const GJonesTable&          GJones cal table
// Output to private data:
//    ROSolvableVisJonesMCol  ROSolvableVisJonesMCol      Read-only SVJ cal 
//                                                        main col
};

//----------------------------------------------------------------------------

GJonesMCol::GJonesMCol (GJonesTable& gjTable) : SolvableVisJonesMCol (gjTable)
{
// Construct from a calibration table
// Input:
//    gjTable              GJonesTable&               GJones calibration table
// Output to private data:
//    SolvableVisJonesMCol SolvableVisJonesMCol       Read-write SVJ cal 
//                                                    main col
};

//----------------------------------------------------------------------------

RODJonesMCol::RODJonesMCol (const DJonesTable& djTable) :
  ROSolvableVisJonesMCol (djTable)
{
// Construct from a calibration table
// Input:
//    djTable                 const DJonesTable&       DJones calibration table
// Output to private data:
//    ROSolvableVisJonesMCol  ROSolvableVisJonesMCol   Read-only SVJ cal 
//                                                     main col
};

//----------------------------------------------------------------------------

DJonesMCol::DJonesMCol (DJonesTable& djTable) : SolvableVisJonesMCol (djTable)
{
// Construct from a calibration table
// Input:
//    djTable              DJonesTable&               DJones calibration table
// Output to private data:
//    SolvableVisJonesMCol SolvableVisJonesMCol       Read-write SVJ cal 
//                                                    main col
};

//----------------------------------------------------------------------------

ROTJonesMCol::ROTJonesMCol (const TJonesTable& djTable) :
  ROSolvableVisJonesMCol (djTable)
{
// Construct from a calibration table
// Input:
//    djTable                 const TJonesTable&       TJones calibration table
// Output to private data:
//    ROSolvableVisJonesMCol  ROSolvableVisJonesMCol   Read-only SVJ cal 
//                                                     main col
};

//----------------------------------------------------------------------------

TJonesMCol::TJonesMCol (TJonesTable& djTable) : SolvableVisJonesMCol (djTable)
{
// Construct from a calibration table
// Input:
//    djTable              TJonesTable&               TJones calibration table
// Output to private data:
//    SolvableVisJonesMCol SolvableVisJonesMCol       Read-write SVJ cal 
//                                                    main col
};

//----------------------------------------------------------------------------


} //# NAMESPACE CASA - END

