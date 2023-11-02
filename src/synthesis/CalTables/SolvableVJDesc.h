//# SolvableVJDesc.h: Define the format of SolvableVisJones cal tables
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
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$

#ifndef CALIBRATION_SOLVABLEVJDESC_H
#define CALIBRATION_SOLVABLEVJDESC_H

#include <synthesis/CalTables/TimeVarVJDesc.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// SolvableVisJonesDesc: Define the format of SolvableVisJones cal tables
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <li> <linkto class="CalTableDesc">CalTableDesc</linkto> module
// <li> <linkto class="VisJonesDesc">VisJonesDesc</linkto> module
// <li> <linkto class="TimeVarVisJonesDesc">TimeVarVisJonesDesc</linkto> module
//
// </prerequisite>
//
// <etymology>
// From "solvable VisJones" and "descriptor".
// </etymology>
//
// <synopsis>
// SolvableVisJonesDesc defines the format of SolvableVisJones calibration 
// tables in terms of table descriptors <linkto class="casacore::TableDesc">TableDesc
// </linkto>, as used in the casacore::Table system. The solvable VisJones calibration
// table formats include fit statistics and weights. Specializations for 
// GJones, DJones and BJones calibration tables are provided through 
// inheritance. At present, this set of classes returns only the default 
// table formats.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// See CalTableDesc.
// </motivation>
//
// <todo asof="98/01/01">
// Generate calibration table descriptors from a VisSet or VisBuffer object
// </todo>

class SolvableVisJonesDesc : public TimeVarVisJonesDesc
{
 public:
   // Default null constructor, and destructor
   SolvableVisJonesDesc();
   virtual ~SolvableVisJonesDesc() {};

   // Construct from the Jones table type
   SolvableVisJonesDesc (const casacore::String& type);

   // Return the cal_main table descriptor
   virtual casacore::TableDesc calMainDesc();

 private:
   // casacore::Table descriptor for fit statistics and weights
   casacore::TableDesc itsFitDesc;

};

// <summary> 
// GJonesDesc: Define the format of GJones cal tables
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//<li><linkto class="CalTableDesc">CalTableDesc</linkto> module
//<li><linkto class="VisJonesDesc">VisJonesDesc</linkto> module
//<li><linkto class="SolvableVisJonesDesc">SolvableVisJonesDesc</linkto> module
// </prerequisite>
//
// <etymology>
// From "GJones" and "descriptor".
// </etymology>
//
// <synopsis>
// GJonesDesc defines the format of GJones calibration tables in terms 
// of table descriptors <linkto class="casacore::TableDesc">casacore::TableDesc</linkto>, 
// as used in the casacore::Table system. GJones matrices define the electronic
// gain corrections in the Measurement Equation formalism. At present, 
// this class returns only the default table formats.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// See CalTableDesc.
// </motivation>
//
// <todo asof="98/01/01">
// Generate calibration table descriptors from a VisSet or VisBuffer object
// </todo>

class GJonesDesc : public SolvableVisJonesDesc
{
 public:
   // Default null constructor, and destructor
   GJonesDesc();
   virtual ~GJonesDesc() {};

};

// <summary> 
// GJonesDelayRateSBDesc: Define the format of GJonesDelayRateSB cal tables
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//<li><linkto class="CalTableDesc">CalTableDesc</linkto> module
//<li><linkto class="VisJonesDesc">VisJonesDesc</linkto> module
//<li><linkto class="SolvableVisJonesDesc">SolvableVisJonesDesc</linkto> module
//<li><linkto class="GJonesDesc">GJonesDesc</linkto> module
// </prerequisite>
//
// <etymology>
// From "GJones" "delay", "rate", "single-band" and "descriptor".
// </etymology>
//
// <synopsis>
// GJonesDelayRateSBDesc defines the format of GJonesDelayRateSB 
// calibration tables in terms of table descriptors
// <linkto class="casacore::TableDesc">casacore::TableDesc</linkto>, 
// as used in the casacore::Table system. This class augments the GJones
// class, with several additional columns for parametrized 
// fringe-fitting solutions. 
// </synopsis>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// See CalTableDesc.
// </motivation>
//
// <todo asof="01/04/30">
// Add multi-band and acceleration columns
// </todo>

class GJonesDelayRateSBDesc : public GJonesDesc
{
 public:
   // Default null constructor, and destructor
   GJonesDelayRateSBDesc();
   virtual ~GJonesDelayRateSBDesc() {};

   // Return the table descriptors for the main calibration table:
   virtual casacore::TableDesc calMainDesc();
};

// <summary> 
// DJonesDesc: Define the format of DJones cal tables
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <li> <linkto class="CalTableDesc">CalTableDesc</linkto> module
// <li> <linkto class="VisJonesDesc">VisJonesDesc</linkto> module
// <li> <linkto class="SolvableVisJonesDesc">SolvableVisJonesDesc</linkto> module
// </prerequisite>
//
// <etymology>
// From "DJones" and "descriptor".
// </etymology>
//
// <synopsis>
// DJonesDesc defines the format of DJones calibration tables in terms 
// of table descriptors <linkto class="casacore::TableDesc">casacore::TableDesc</linkto>, 
// as used in the casacore::Table system. DJones matrices define the instrumental
// polarization corrections in the Measurement Equation formalism. At 
// present, this class returns only the default table formats.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// See CalTableDesc.
// </motivation>
//
// <todo asof="98/01/01">
// Generate calibration table descriptors from a VisSet or VisBuffer object
// </todo>

class DJonesDesc : public SolvableVisJonesDesc
{
 public:
   // Default null constructor, and destructor
   DJonesDesc();
   virtual ~DJonesDesc() {};

};

// <summary> 
// TJonesDesc: Define the format of TJones cal tables
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//<li><linkto class="CalTableDesc">CalTableDesc</linkto> module
//<li><linkto class="VisJonesDesc">VisJonesDesc</linkto> module
//<li><linkto class="SolvableVisJonesDesc">SolvableVisJonesDesc</linkto> module
// </prerequisite>
//
// <etymology>
// From "TJones" and "descriptor".
// </etymology>
//
// <synopsis>
// TJonesDesc defines the format of TJones calibration tables in terms 
// of table descriptors <linkto class="casacore::TableDesc">casacore::TableDesc</linkto>, 
// as used in the casacore::Table system. TJones matrices define the atmospheric
// gain corrections in the Measurement Equation formalism. At present, 
// this class returns only the default table formats.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// See CalTableDesc.
// </motivation>
//
// <todo asof="98/01/01">
// Generate calibration table descriptors from a VisSet or VisBuffer object
// </todo>

class TJonesDesc : public SolvableVisJonesDesc
{
 public:
   // Default null constructor, and destructor
   TJonesDesc();
   virtual ~TJonesDesc() {};

};



} //# NAMESPACE CASA - END

#endif
   
  





