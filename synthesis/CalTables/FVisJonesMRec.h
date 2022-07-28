//# FVisJonesMRec.h: SolvableVisJones cal_main table record access & creation
//# Copyright (C) 1996,1997,1998,2000,2003
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

#ifndef CALIBRATION_FVISJONESMREC_H
#define CALIBRATION_FVISJONESMREC_H

#include <casacore/casa/aips.h>
#include <synthesis/CalTables/TimeVarVJMRec.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// FVisJonesMRec: FVisJones cal_main table record access/creation
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
// From "F VisJones" and "main record".
// </etymology>
//
// <synopsis>
// The FVisJonesMRec class allows the creation of records for the
// main calibration table of FVisJones type, and provides access
// to the individual record fields. 
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate access to FVisJones calibration table records.
// </motivation>
//
// <todo asof="98/01/01">
// (i) Deal with non-standard columns.
// </todo>

class FVisJonesMRec : public TimeVarVisJonesMRec
{
 public:
   // Default null constructor, and destructor
   FVisJonesMRec();
   ~FVisJonesMRec() {};

   // Construct from an existing record
   FVisJonesMRec (const casacore::Record& inpRec);

   // a) define
   void defineRotMeas    (const casacore::Float& rotMeas);
   void defineRotMeasErr (const casacore::Float& rotMeasErr);

   // b) get
   void getRotMeas    (casacore::Float& rotMeas);
   void getRotMeasErr (casacore::Float& rotMeasErr);
 };

// <summary> 
// FVisJonesIonoMRec: FVisJonesIono cal_main table record access/creation
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <li> <linkto class="CalMainRecord">CalMainRecord</linkto> module
// <li> <linkto class="VisJonesMRec">VisJonesMRec</linkto> module
// <li> <linkto class="FVisJonesMRec">FVisJonesMRec</linkto> module
// </prerequisite>
//
// <etymology>
// From "F VisJones - Ionosphere" and "main record".
// </etymology>
//
// <synopsis>
// The FVisJonesIonoMRec class allows the creation of records for the
// main calibration table of FVisJonesIono type, and provides access
// to the individual record fields. 
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate access to FVisJonesIono calibration table records.
// </motivation>
//
// <todo asof="98/01/01">
// (i) Deal with non-standard columns.
// </todo>
class FVisJonesIonoMRec : public FVisJonesMRec
{
 public:
   // Default null constructor, and destructor
   FVisJonesIonoMRec();
   ~FVisJonesIonoMRec() {};

   // Construct from an existing record
   FVisJonesIonoMRec (const casacore::Record& inpRec);

   // Field accessors
   // a) define
   void defineTEC    (const casacore::Float& tec);
   void defineTECErr (const casacore::Float& tecErr);

   // b) get
   void getTEC    (casacore::Float& tec);
   void getTECErr (casacore::Float& tecErr);
 };


} //# NAMESPACE CASA - END

#endif
   
  



