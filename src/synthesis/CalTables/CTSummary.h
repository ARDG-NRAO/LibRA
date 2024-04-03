//# CTSummary.h: Helper class for listing a NewCalHeader
//# Copyright (C) 2017
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
#ifndef CALIBRATION_CTSUMMARY_H
#define CALIBRATION_CTSUMMARY_H

#include <casacore/casa/aips.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Arrays.h>
#include <casacore/measures/Measures/MPosition.h>

namespace casa { //# NAMESPACE CASA - BEGIN

class NewCalTable;

// <summary>Provides and lists information about the header of a
// calibration table. Based on MSSummary</summary>
// <use visibility=export>
// 
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>
// 
// <prerequisite>
//   <li> <linkto class=NewCalTable>NewCalTable</linkto>
//   <li> <linkto class=MSSummary>MSSummary</linkto>
//   <li> <linkto module=Coordinates>Coordinates</linkto> 
// </prerequisite>
//
// <etymology>
// This class lists the ancilliary or header information from a
// NewCalTable in a Summary format.
// </etymology>
//
// <synopsis>
// NCTs consist of pixels and descriptive information stored in what
// is loosely termed the header. This is information describing the
// coordinate system, the image units etc.  This class enables you to
// retrieve the descriptive header information and/or list it.
// </synopsis>
//
// <example>
// <srcBlock>
//     NewCalTable inputCT(fileName);
//     CTSummary header(inputCT);
//     LogOrigin or("myClass", "myFunction(...)", WHERE);
//     LogIO os(or);
//     header.list(os);
// </srcBlock>
// A <src>NewCalTable</src> object is constructed and then logged 
// to the supplied <src>LogIO</src> object.
// </example>
//
// <motivation>
// The viewing of the cal table header is a basic capability that is
// commonly required.
// </motivation>
//
 

class CTSummary
{
public:
// Constructor
// <group>
   CTSummary (const NewCalTable& ct);
   CTSummary (const NewCalTable* ct);
// </group>

// Destructor
  ~CTSummary();

// Retrieve number of rows
  casacore::Int nrow() const;
 
// Retrieve caltable name
  casacore::String name() const;

// Set a new NCT. 
  casacore::Bool setNCT (const NewCalTable& ct);

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
// Pointer to NewCalTable 
   const NewCalTable* pNCT;

// Formatting strings
   const casacore::String dashlin1, dashlin2;

// Needed for antenna offsets
   bool getObservatoryPosition(casacore::MPosition& obspos,
      casacore::String& name) const;

// Clear formatting flags
   void clearFormatFlags (casacore::LogIO& os) const;

// For keeping track of the number of vis per field
   mutable casacore::Vector<casacore::Int> nVisPerField_;

   // Name of the CT used in the constructor
   casacore::String ctname_p;

};


} //# NAMESPACE CASA - END

#endif
