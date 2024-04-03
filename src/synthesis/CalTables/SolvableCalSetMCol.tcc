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

#include <synthesis/CalTables/SolvableCalSetMCol.h>

namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

template<class T>
ROSolvableCalSetMCol<T>::ROSolvableCalSetMCol (const CalTable2& 
					    svjTable):
  ROCalMainColumns2<T>(svjTable)
{
// Construct from a calibration table
// casacore::Input:
//    svjTable              const SolvableCalSetTable&  SVJ calibration table
// Output to private data:
//    ROTimeVarVisJonesMCol ROTimeVarVisJonesMCol         Read-only TVVJ 
//                                                        cal main col
  // Attach all column accessors for additional SolvableCalSet columns
  // (all of which are required columns)
  ROCalMainColumns2<T>::attach (svjTable, totalSolnOk_p, MSC::TOTAL_SOLUTION_OK);
  ROCalMainColumns2<T>::attach (svjTable, totalFit_p, MSC::TOTAL_FIT);
  ROCalMainColumns2<T>::attach (svjTable, totalFitWgt_p, MSC::TOTAL_FIT_WEIGHT);
  ROCalMainColumns2<T>::attach (svjTable, solnOk_p, MSC::SOLUTION_OK);
  ROCalMainColumns2<T>::attach (svjTable, fit_p, MSC::FIT);
  ROCalMainColumns2<T>::attach (svjTable, fitWgt_p, MSC::FIT_WEIGHT);
  ROCalMainColumns2<T>::attach (svjTable, flag_p, MSC::FLAG);
  ROCalMainColumns2<T>::attach (svjTable, snr_p, MSC::SNR);
};

//----------------------------------------------------------------------------

template<class T>
SolvableCalSetMCol<T>::SolvableCalSetMCol (CalTable2& svjTable) :
  CalMainColumns2<T> (svjTable)
{
// Construct from a calibration table
// casacore::Input:
//    svjTable             SolvableCalSetTable&      SVJ calibration table
// Output to private data:
//    TimeVarVisJonesMCol  TimeVarVisJonesMCol         Read-write TVVJ 
//                                                     cal main col
  // Attach all column accessors for additional SolvableCalSet columns
  // (all of which are required columns)
  CalMainColumns2<T>::attach (svjTable, totalSolnOk_p, MSC::TOTAL_SOLUTION_OK);
  CalMainColumns2<T>::attach (svjTable, totalFit_p, MSC::TOTAL_FIT);
  CalMainColumns2<T>::attach (svjTable, totalFitWgt_p, MSC::TOTAL_FIT_WEIGHT);
  CalMainColumns2<T>::attach (svjTable, solnOk_p, MSC::SOLUTION_OK);
  CalMainColumns2<T>::attach (svjTable, fit_p, MSC::FIT);
  CalMainColumns2<T>::attach (svjTable, fitWgt_p, MSC::FIT_WEIGHT);
  CalMainColumns2<T>::attach (svjTable, flag_p, MSC::FLAG);
  CalMainColumns2<T>::attach (svjTable, snr_p, MSC::SNR);
};

//----------------------------------------------------------------------------


} //# NAMESPACE CASA - END

