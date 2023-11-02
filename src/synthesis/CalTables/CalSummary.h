//# CalSummary.h: Helper class for listing a CalTable
//# Copyright (C) 2018
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
//#
#ifndef CALIBRATION_CALSUMMARY_H
#define CALIBRATION_CALSUMMARY_H

#include <casacore/casa/aips.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Arrays.h>
#include <casacore/measures/Measures/MPosition.h>

namespace casa { //# NAMESPACE CASA - BEGIN

class CalTable;

// <summary>Provides and lists information about the header of a
// calibration table. Based on MSSummary and CTSummary</summary>
// <use visibility=export>
// 
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>
// 
// <prerequisite>
//   <li> <linkto class=CalTable>CalTable</linkto>
//   <li> <linkto class=MSSummary>MSSummary</linkto>
//   <li> <linkto module=Coordinates>Coordinates</linkto> 
// </prerequisite>
//
// <etymology>
// This class lists the ancilliary or header information from a
// CalTable in a Summary format.
// </etymology>
//
// <synopsis>
// CalTables consist of pixels and descriptive information stored in what
// is loosely termed the header. This is information describing the
// coordinate system, the image units etc.  This class enables you to
// retrieve the descriptive header information and/or list it.
// </synopsis>
//
// <example>
// <srcBlock>
//     CalTable inputCT(fileName);
//     CalSummary header(inputCT);
//     LogOrigin or("myClass", "myFunction(...)", WHERE);
//     LogIO os(or);
//     header.list(os);
// </srcBlock>
// A <src>CalTable</src> object is constructed and then logged 
// to the supplied <src>LogIO</src> object.
// </example>
//
// <motivation>
// The viewing of the cal table header is a basic capability that is
// commonly required.
// </motivation>
//
 

class CalSummary
{
public:
// Constructor
// <group>
   CalSummary (CalTable& ct);
   CalSummary (CalTable* ct);
// </group>

// Destructor
  ~CalSummary();

// Retrieve number of rows
   casacore::Int nrow() const;
 
// Retrieve caltable name
   const casacore::String name() const;

// Set a new CT. 
   casacore::Bool setCT (CalTable& ct);

// List all header information.
   void list (casacore::LogIO& os, casacore::Bool verbose=false) const;

// List a title for the Summary.
   void listTitle (casacore::LogIO& os) const;

// List convenient groupings of tables:
// List where CT obtained (Observation table)
   void listWhere (casacore::LogIO& os, casacore::Bool verbose=false) const;
// List what was observed (Field and Main tables)
   void listWhat (casacore::LogIO& os, casacore::Bool verbose=false) const;
// List how data were obtained (SpectralWindow and Antenna tables)
   void listHow (casacore::LogIO& os, casacore::Bool verbose=false) const;

// List main table
   void listMain (casacore::LogIO& os, casacore::Bool verbose=false) const;

// List subtables
// <group>
   void listObservation (casacore::LogIO& os, casacore::Bool verbose=false) const;
   void listField (casacore::LogIO& os, casacore::Bool verbose=false) const;
   void listAntenna (casacore::LogIO& os, casacore::Bool verbose=false) const;
   void listSpectralWindow (casacore::LogIO& os, casacore::Bool verbose=false) const;
   void listHistory (casacore::LogIO& os) const;
// </group>

// List table size summary
   void listTables (casacore::LogIO& os, casacore::Bool verbose=false) const;

private:
// Pointer to CalTable 
   CalTable* pCT;

// for fieldnames and antenna names
   casacore::String msname_p;
   bool haveMS_p;

// Formatting strings
   const casacore::String dashlin1, dashlin2;

// Set msname_p if MS exists. 
   void setMSname();

// Needed for antenna offsets
   bool getObservatoryPosition(casacore::MPosition& obspos,
      casacore::String& name) const;

// Clear formatting flags
   void clearFormatFlags (casacore::LogIO& os) const;

// For keeping track of the number of vis per field
   mutable casacore::Vector<casacore::Int> nVisPerField_;

};


} //# NAMESPACE CASA - END

#endif
