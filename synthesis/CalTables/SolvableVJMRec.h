//# SolvableVJMRec.h: SolvableVisJones cal_main table record access & creation
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

#ifndef CALIBRATION_SOLVABLEVJMREC_H
#define CALIBRATION_SOLVABLEVJMREC_H

#include <casa/aips.h>
#include <synthesis/CalTables/TimeVarVJMRec.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// SolvableVisJonesMRec: SolvableVisJones cal_main table record access/creation
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
// The SolvableVisJonesMRec class allows the creation of records for the
// main calibration table of SolvableVisJones type, and provides access
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
// Encapsulate access to SolvableVisJones calibration table records.
// </motivation>
//
// <todo asof="98/01/01">
// (i) Deal with non-standard columns.
// </todo>

class SolvableVisJonesMRec : public TimeVarVisJonesMRec
{
 public:
   // Default null constructor, and destructor
   SolvableVisJonesMRec();
   ~SolvableVisJonesMRec() {};

   // Construct from an existing record
   SolvableVisJonesMRec (const casacore::Record& inpRec);

   // Field accessors
   // Note: uses protected functions defined in CalMainRecord,
   // as fit parameters are used elsewhere in the inheritance
   // tree for baseline-based and image plane calibration tables.
   // a) define
   void defineTotalSolnOk (const casacore::Bool& totalSolnOk) 
     {dTotalSolnOk (totalSolnOk);};
   void defineTotalFit (const casacore::Float& totalFit) {dTotalFit (totalFit);};
   void defineTotalFitWgt (const casacore::Float& totalFitWgt) 
     {dTotalFitWgt (totalFitWgt);};
   void defineSolnOk (const casacore::Array <casacore::Bool>& solnOk) {dSolnOk (solnOk);};
   void defineFit (const casacore::Array <casacore::Float>& fit) {dFit (fit);};
   void defineFitWgt (const casacore::Array <casacore::Float>& fitWgt) {dFitWgt (fitWgt);};

   // b) get
   void getTotalSolnOk (casacore::Bool& totalSolnOk) {gTotalSolnOk (totalSolnOk);};
   void getTotalFit (casacore::Float& totalFit) {gTotalFit (totalFit);};
   void getTotalFitWgt (casacore::Float& totalFitWgt) {gTotalFitWgt (totalFitWgt);};
   void getSolnOk (casacore::Array <casacore::Bool>& solnOk) {gSolnOk (solnOk);};
   void getFit (casacore::Array <casacore::Float>& fit) {gFit (fit);};
   void getFitWgt (casacore::Array <casacore::Float>& fitWgt) {gFitWgt (fitWgt);};
 };

// <summary> 
// GJonesMRec: GJones cal_main table record access & creation
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//<li><linkto class="CalMainRecord">CalMainRecord</linkto> module
//<li><linkto class="VisJonesMRec">VisJonesMRec</linkto> module
//<li><linkto class="SolvableVisJonesMRec">SolvableVisJonesMRec</linkto> module
// </prerequisite>
//
// <etymology>
// From "GJones" and "main record".
// </etymology>
//
// <synopsis>
// The GJonesMRec class allows the creation of records for the
// main calibration table of GJones type, and provides access
// to the individual record fields. GJones matrices are used to
// store electronic information in the Measurement Equation formalism.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate access to GJones calibration table records.
// </motivation>
//
// <todo asof="98/01/01">
// (i) Deal with non-standard columns.
// </todo>

class GJonesMRec : public SolvableVisJonesMRec
{
 public:
   // Default null constructor, and destructor
   GJonesMRec();
   ~GJonesMRec() {};

   // Construct from an existing record
   GJonesMRec (const casacore::Record& inpRec);

};

// <summary> 
// DJonesMRec: DJones cal_main table record access & creation
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//<li><linkto class="CalMainRecord">CalMainRecord</linkto> module
//<li><linkto class="VisJonesMRec">VisJonesMRec</linkto> module
//<li><linkto class="SolvableVisJonesMRec">SolvableVisJonesMRec</linkto> module
// </prerequisite>
//
// <etymology>
// From "DJones" and "main record".
// </etymology>
//
// <synopsis>
// The DJonesMRec class allows the creation of records for the
// main calibration table of DJones type, and provides access
// to the individual record fields. DJones matrices are used
// to store instrumental polarization information in the
// Measurement Equation formalism.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate access to DJones calibration table records.
// </motivation>
//
// <todo asof="98/01/01">
// (i) Deal with non-standard columns.
// </todo>

class DJonesMRec : public SolvableVisJonesMRec
{
 public:
   // Default null constructor, and destructor
   DJonesMRec();
   ~DJonesMRec() {};

   // Construct from an existing record
   DJonesMRec (const casacore::Record& inpRec);

};

// <summary> 
// TJonesMRec: TJones cal_main table record access & creation
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//<li><linkto class="CalMainRecord">CalMainRecord</linkto> module
//<li><linkto class="VisJonesMRec">VisJonesMRec</linkto> module
//<li><linkto class="SolvableVisJonesMRec">SolvableVisJonesMRec</linkto> module
// </prerequisite>
//
// <etymology>
// From "TJones" and "main record".
// </etymology>
//
// <synopsis>
// The TJonesMRec class allows the creation of records for the
// main calibration table of TJones type, and provides access
// to the individual record fields. TJones matrices are used
// to atmospheric corrections in the Measurement Equation formalism.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate access to TJones calibration table records.
// </motivation>
//
// <todo asof="98/01/01">
// (i) Deal with non-standard columns.
// </todo>

class TJonesMRec : public SolvableVisJonesMRec
{
 public:
   // Default null constructor, and destructor
   TJonesMRec();
   ~TJonesMRec() {};

   // Construct from an existing record
   TJonesMRec (const casacore::Record& inpRec);

};


} //# NAMESPACE CASA - END

#endif
   
  



