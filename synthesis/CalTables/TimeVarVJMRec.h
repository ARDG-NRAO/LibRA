//# TimeVarVJMRec.h: TimeVarVisJones cal_main table record access and creation
//# Copyright (C) 1996,1997,1998,2003
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

#ifndef CALIBRATION_TIMEVARVJMREC_H
#define CALIBRATION_TIMEVARVJMREC_H

#include <casa/aips.h>
#include <synthesis/CalTables/VisJonesMRec.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// TimeVarVisJonesMRec: TimeVarVisJones cal_main table record access & creation
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="CalMainRecord">CalMainRecord</linkto> module
//   <li> <linkto class="VisJonesMRec">VisJonesMRec</linkto> module
// </prerequisite>
//
// <etymology>
// From "time-variable VisJones" and "main record".
// </etymology>
//
// <synopsis>
// The TimeVarVisJonesMRec class allows the creation of records for the
// main calibration table of TimeVarVisJones type, and provides access
// to the individual record fields. Specializations for CJones and
// PJones calibration table records are provided through inheritance.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate access to TimeVarVisJones calibration table records.
// </motivation>
//
// <todo asof="98/01/01">
// (i) Deal with non-standard columns.
// </todo>

class TimeVarVisJonesMRec : public VisJonesMRec
{
 public:
   // Default null constructor, and destructor
   TimeVarVisJonesMRec();
   ~TimeVarVisJonesMRec() {};

   // Construct from an existing record
   TimeVarVisJonesMRec (const casacore::Record& inpRec);

 };

// <summary> 
// PJonesMRec: PJones cal_main table record access & creation
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <li> <linkto class="CalMainRecord">CalMainRecord</linkto> module
// <li> <linkto class="VisJonesMRec">VisJonesMRec</linkto> module
// <li> <linkto class="TimeVarVisJonesMRec">TimeVarVisJonesMRec</linkto> module
// </prerequisite>
//
// <etymology>
// From "PJones" and "main record".
// </etymology>
//
// <synopsis>
// The PJonesMRec class allows the creation of records for the
// main calibration table of PJones type, and provides access
// to the individual record fields. PJones matrices are used to
// store parallactic angle information in the Measurement
// Equation formalism.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate access to PJones calibration table records.
// </motivation>
//
// <todo asof="98/01/01">
// (i) Deal with non-standard columns.
// </todo>

class PJonesMRec : public TimeVarVisJonesMRec
{
 public:
   // Default null constructor, and destructor
   PJonesMRec();
   ~PJonesMRec() {};

   // Construct from an existing record
   PJonesMRec (const casacore::Record& inpRec);

 };

// <summary> 
// CJonesMRec: CJones cal_main table record access & creation
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <li> <linkto class="CalMainRecord">CalMainRecord</linkto> module
// <li> <linkto class="VisJonesMRec">VisJonesMRec</linkto> module
// <li> <linkto class="TimeVarVisJonesMRec">TimeVarVisJonesMRec</linkto> module
// </prerequisite>
//
// <etymology>
// From "CJones" and "main record".
// </etymology>
//
// <synopsis>
// The CJonesMRec class allows the creation of records for the
// main calibration table of CJones type, and provides access
// to the individual record fields. CJones matrices are used
// to store polarization configuration information in the
// Measurement Equation formalism.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate access to CJones calibration table records.
// </motivation>
//
// <todo asof="98/01/01">
// (i) Deal with non-standard columns.
// </todo>

class CJonesMRec : public TimeVarVisJonesMRec
{
 public:
   // Default null constructor, and destructor
   CJonesMRec();
   ~CJonesMRec() {};

   // Construct from an existing record
   CJonesMRec (const casacore::Record& inpRec);

 };


} //# NAMESPACE CASA - END

#endif
   
  



