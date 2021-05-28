//# FlagVersions.h: Maintain and manage different flag versions.
//# Copyright (C) 1994,1995,1996,1997,1998,1999,2000,2001,2002,2003
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
//# You should have receied a copy of the GNU Library General Public License
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
//# ----------------------------------------------------------------------------
//# Change Log
//# ----------------------------------------------------------------------------
//# Date         Name             Comments
//# 


#ifndef FLAGVERSION_H
#define FLAGVERSION_H

//# Includes

#include <casa/OS/Timer.h>
#include <casa/OS/File.h>

#include <tables/Tables/Table.h>
#include <tables/Tables/ArrayColumn.h>
#include <tables/Tables/ScalarColumn.h>
#include <tables/Tables/TableColumn.h>

//# FlagVersion Change Log
//# =======================
//# Date              Name      Description
//# April 20 2007    Urvashi R.V.       Created this class.
//# Aug 23 2007      Urvashi R.V.       Added Documentation.
//# Oct 29 2007      Shannon J.         Switched how msg logging was done.

namespace casa { //# NAMESPACE CASA - BEGIN

class SLog;

// <summary>
// Class to generate and manage flag versions for Tables containing flag columns. 
// </summary>

// <reviewed reviewer="" date="" tests="">
// </reviewed>

// <prerequisite>
//   <li> casacore::Table 
// </prerequisite>

// <etymology>
// Manages Flag Versions.
// </etymology>

// <synopsis>
// This class creates and manages flag versions. A flag version as defined in this
// class has the following structure.
//
//    xxxx.ms
//    xxxx.ms.flagversions
//
// When first opened, a flagversions directory is created parallel to the original Table.
// This directory contains a text file : FLAG_VERSION_LIST that holds a list of string pairs
//      version name  :  comment for this version.
//
// The FLAG and FLAG_ROW columns of the main table are copied into new Tables with only two
// columns, whenever a new flag version is created.  Flag versions can be merged using
// logical 'and', logical 'or', and 'replace' semantics. Flag versions can be restored to
// the main table of the MS.
//
// At the end of a "save" or "restore" operation, the latest flags are always also in
// the main table.
// 
// </synopsis>

// <motivation>
//  
// </motivation>

// <thrown>
//    <li>
//    <li>
// </thrown>


// <todo asof="$DATE:$">
//   <li> 
// </todo>

       
class FlagVersion 
{
   public:
      // Constructor
      FlagVersion(casacore::String intab, casacore::String dataflagcolname, casacore::String rowflagcolname);  

      // Destructor
      virtual ~FlagVersion();

      // Operator=
      // Equate by reference.
      FlagVersion& operator=(const FlagVersion&){return *this;}
      
      // Get a list of entries from the version-list file for this table 
      casacore::Vector<casacore::String> getVersionList();

      // Specify the casacore::Table column names to use as flag columns.
      // For example, for a casacore::MS, they are "FLAG" and "FLAG_ROW". 
      casacore::Bool attachFlagColumns(casacore::String version, casacore::ScalarColumn<casacore::Bool> &rowflag, 
                 casacore::ArrayColumn<casacore::Bool> &flags, casacore::Table &subtab);

      // Save current main table flags into a separate version 
      // These keeps a copy in the main table too. 
      // "merge" can be one of 'or','and','replace'
      casacore::Bool saveFlagVersion( casacore::String versionname, casacore::String comment, 
                  casacore::String merge=casacore::String("replace") );

      // Copy flags from a flag version, into the main table 
      casacore::Bool restoreFlagVersion(casacore::String versionname, 
                     casacore::String merge=casacore::String("replace") );

      // Delete a version. This does not touch or update the main table 
      casacore::Bool deleteFlagVersion( casacore::String versionname );

      // Clear all main table flags 
      casacore::Bool clearAllFlags();

   private:
      void FlagVersionError( casacore::String msg );
      
      casacore::Bool readVersionList();
      casacore::Bool saveFlagsInto(casacore::Table &fromFTab, casacore::Table &toFTab, 
                casacore::String merge = casacore::String("replace"));
      casacore::Bool doesVersionExist( casacore::String versionname );

      /* Variables to be maintained for the root casacore::Table */
      casacore::String verlistfile_p;
      casacore::String flagtablename_p;
      casacore::String dataflagcolname_p;
      casacore::String rowflagcolname_p;

      casacore::Table tab_p;
      casacore::String tabname_p;
      casacore::Vector<casacore::String> versionlist_p;
      casacore::Vector<casacore::String> commentlist_p;
      casacore::Bool fcol_p, frcol_p;
      unsigned nrows_p;

      casacore::Table subflagtable_p;

      casacore::File file_p;
      
      SLog* log;
      static casacore::String clname;
};

} //# NAMESPACE CASA - END 

//#ifndef AIPS_NO_TEMPLATE_SRC
//#include <tableplot/TablePlot/FlagVersion.tcc>
//#endif //# AIPS_NO_TEMPLATE_SRC
#endif

