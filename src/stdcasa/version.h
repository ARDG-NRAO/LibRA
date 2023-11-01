//# version.h: Version information for AIPS++
//# Copyright (C) 1996,1997,1999,2000,2001,2002,2004,2016
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
//#
//# $Id$

#ifndef CASA_VERSION_H
#define CASA_VERSION_H

//# Includes
#include <iostream>
#include <vector>

namespace casa { //# NAMESPACE CASA - BEGIN

class VersionInfo {
    public:

    static int major( );            // major version number: e.g. 4
    static int minor( );            // minor version number: e.g. 7
    static int patch( );            // patch number: e.g.         1
    static int feature( );          // feature increment: e.g.    298
    static std::string desc( );     // description: e.g.          " ID cc37ed0c4a"
    static std::string info( );     // combined string: e.g.      "5.0.0-565 ID cc37ed0c4a"
    static std::string str( );      // combined string: e.g.      "5.0.0-565"
    static std::string variant( );  // "Instrument" variant: e.g.          "ALMA"

    // compare the version included in 'vec' against the current version.
    // 'comparitor' is one of: ">", "<", ">=", "<=", "=", "!="
    static bool compare(const  std::string& comparitor,  const std::vector<int>& vec);

    // Summarize the above into an ostream. Note that an
    // <src>ostringstream</src> can be converted to a
    // <linkto class="String">String</linkto> via a constructor.
    // This information is NOT prepended with "AIPS++ version:" or anything
    // like that. You may wish to add this yourself. The date is also not
    // included.
    static void report(std::ostream &os) { os << info( ); }

};

} //# NAMESPACE CASA - END

#endif
