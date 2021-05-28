//# CalDescRecord.h: Cal_desc table record access and creation
//# Copyright (C) 1996,1997,1998
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
//# $Id: 

#ifndef CALIBRATION_CALDESCRECORD_H
#define CALIBRATION_CALDESCRECORD_H

#include <casa/aips.h>
#include <casa/Containers/Record.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// CalDescRecord: Cal_desc table record access and creation
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="casacore::Record">casacore::Record</linkto> module
//   <li> <linkto class="CalMainRecord">CalMainRecord</linkto> module
// </prerequisite>
//
// <etymology>
// From "calibration description sub-table" and "record".
// </etymology>
//
// <synopsis>
// The CalDescRecord classes allow access to, and creation of, records
// in the cal_desc calibration sub-table. Specializations for SkyJones
// calibration table record types is provided through inheritance.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate access to cal_desc calibration table records.
// </motivation>
//
// <todo asof="98/01/01">
// </todo>

class CalDescRecord
{
 public:
   // Default null constructor, and destructor
   CalDescRecord();
   ~CalDescRecord() {};

   // Construct from an existing record
   CalDescRecord (const casacore::Record& inpRec);

   // Return as record
   const casacore::Record& record();

   // Field accessors
   // a) define
   void defineNumSpw (const casacore::Int& numSpw);
   void defineNumChan (const casacore::Array <casacore::Int>& numChan);
   void defineNumReceptors (const casacore::Int& numReceptors);
   void defineNJones (const casacore::Int& nJones);
   void defineSpwId (const casacore::Array <casacore::Int>& spwId);
   void defineChanFreq (const casacore::Array <casacore::Double>& chanFreq);
   void defineChanWidth (const casacore::Array <casacore::Double>& chanWidth);
   void defineChanRange (const casacore::Array <casacore::Int>& chanRange);
   void definePolznType (const casacore::Array <casacore::String>& polznType);
   void defineJonesType (const casacore::String& jonesType);
   void defineMSName (const casacore::String& msName);
     
   // b) get
   void getNumSpw (casacore::Int& numSpw);
   void getNumChan (casacore::Array <casacore::Int>& numChan);
   void getNumReceptors (casacore::Int& numReceptors);
   void getNJones (casacore::Int& nJones);
   void getSpwId (casacore::Array <casacore::Int>& spwId);
   void getChanFreq (casacore::Array <casacore::Double>& chanFreq);
   void getChanWidth (casacore::Array <casacore::Double>& chanWidth);
   void getChanRange (casacore::Array <casacore::Int>& chanRange);
   void getPolznType (casacore::Array <casacore::String>& polznType);
   void getJonesType (casacore::String& jonesType);
   void getMSName (casacore::String& msName);

 protected:
   // Add to itsRecord
   void addRec (const casacore::Record& newRec);

 private:
   casacore::Record itsRecord;

};


} //# NAMESPACE CASA - END

#endif
   
  



