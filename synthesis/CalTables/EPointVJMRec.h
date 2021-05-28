//# EPointVJMRec.h: EPointVisJones cal_main table record access & creation
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

#ifndef CALIBRATION_EPOINTVJMREC_H
#define CALIBRATION_EPOINTVJMREC_H

#include <casa/aips.h>
#include <synthesis/CalTables/SolvableVJMRec.h>

namespace casa {

// <summary> 
// EPointVisJonesMRec: EPointVisJones cal_main table record access/creation
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
// From "solvable VisJones" and "main record".
// </etymology>
//
// <synopsis>
// The EPointVisJonesMRec class allows the creation of records for the
// main calibration table of EPointVisJones type, and provides access
// to the individual record fields. Specializations for GJones, DJones,
// TJones and BJones calibration table records are provided through 
// inheritance.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate access to EPointVisJones calibration table records.
// </motivation>
//
// <todo asof="98/01/01">
// (i) Deal with non-standard columns.
// </todo>

class EPointVisJonesMRec : public SolvableVisJonesMRec
{
 public:
  // Default null constructor, and destructor
  EPointVisJonesMRec();
  ~EPointVisJonesMRec() {};

  // Construct from an existing record
  EPointVisJonesMRec (const casacore::Record& inpRec);

  // Field accessors
  // Note: uses protected functions defined in CalMainRecord,
  // as fit parameters are used elsewhere in the inheritance
  // tree for baseline-based and image plane calibration tables.
  // a) define
  void definePointingOffset(const casacore::Array<casacore::Float>& pointingOffset);
  //  {dPointingOffset(pointingOffset);}

  // b) get
  void getPointingOffset (casacore::Array<casacore::Float>& pointingOffset);
  //  {gPointingOffset (pointingOffset);};
};

}
#endif
