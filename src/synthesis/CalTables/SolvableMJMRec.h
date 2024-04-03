//# SolvableMJMRec.h: SolvableMJones cal_main table record access & creation
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

#ifndef CALIBRATION_SOLVABLEMJMREC_H
#define CALIBRATION_SOLVABLEMJMREC_H

#include <synthesis/CalTables/TimeVarMJMRec.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// SolvableMJonesMRec: SolvableMJones cal_main table record access/creation
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <li> <linkto class="CalMainRecord">CalMainRecord</linkto> module
// <li> <linkto class="MJonesMRec">MJonesMRec</linkto> module
// <li> <linkto class="TimeVarMJonesMRec">TimeVarMJonesMRec</linkto> module
// </prerequisite>
//
// <etymology>
// From "solvable MJones" and "main record".
// </etymology>
//
// <synopsis>
// The SolvableMJonesMRec class allows the creation of records for the
// main calibration table of SolvableMJones type, and provides access
// to the individual record fields. A specialization for MIfr
// calibration table records is provided through inheritance.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate access to SolvableMJones calibration table records.
// </motivation>
//
// <todo asof="98/01/01">
// (i) Deal with non-standard columns.
// </todo>

class SolvableMJonesMRec : public TimeVarMJonesMRec
{
 public:
   // Default null constructor, and destructor
   SolvableMJonesMRec();
   ~SolvableMJonesMRec() {};

   // Construct from an existing record
   SolvableMJonesMRec (const casacore::Record& inpRec);

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
// MIfrMRec: MIfr cal_main table record access & creation
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//<li><linkto class="CalMainRecord">CalMainRecord</linkto> module
//<li><linkto class="MJonesMRec">MJonesMRec</linkto> module
//<li><linkto class="SolvableMJonesMRec">SolvableMJonesMRec</linkto> module
// </prerequisite>
//
// <etymology>
// From "MIfr" and "main record".
// </etymology>
//
// <synopsis>
// The MIfrMRec class allows the creation of records for the
// main calibration table of MIfr type, and provides access
// to the individual record fields. MIfr matrices are used to
// store diagonal, multiplicative interferometer-based
// corrections in the Measurement Equation formalism.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate access to MIfr calibration table records.
// </motivation>
//
// <todo asof="98/01/01">
// (i) Deal with non-standard columns.
// </todo>

class MIfrMRec : public SolvableMJonesMRec
{
 public:
   // Default null constructor, and destructor
   MIfrMRec();
   ~MIfrMRec() {};

   // Construct from an existing record
   MIfrMRec (const casacore::Record& inpRec);

};


} //# NAMESPACE CASA - END

#endif



