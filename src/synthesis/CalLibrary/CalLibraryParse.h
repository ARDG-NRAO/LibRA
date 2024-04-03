//# CalLibraryParse.h: Class to hold results from cal library parser
//# Copyright (C) 1994,1995,1997,1998,1999,2000,2001,2003
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

#ifndef CALLIB_CALLIBRARYPARSE_H
#define CALLIB_CALLIBRARYPARSE_H

#include <casacore/casa/aips.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Containers/Record.h>

namespace casacore{

// The class MSFieldParse only contains information about a table
}

namespace casa {

// <motivation>
// It is necessary to be able to give a cal library filename for 
// instructions for on-the-fly calibration.
// This can be given in a CLI or set in a GUI file browser.
// </motivation>

// <summary>
// Class to hold values from cal library file parser
// </summary>

// <use visibility=local>

// <etymology>
// CalLibraryParse is the class used to parse a cal library.
// </etymology>

// <synopsis>
// CalLibraryParse is used by the parser of cal library files.
// The parser is written in Bison and Flex in files CalLibraryGram.yy and .ll.
// The statements in there use the routines in this file to complete
// a casacore::Record with default and specified calibration parameters.
//
// used in the table command. Global variables (like a list and a vector)
// are used in MSFieldParse.cc to hold further information.
//
// Global functions are used to operate on the information.
// The main function is the global function callibSetParams.
// It parses the given calibration library file and completes
// the given Record.  This is, in fact, the only function to 
// be used by a user.
// </synopsis>

class CalLibraryParse
{
    public:
        CalLibraryParse();
        ~CalLibraryParse();

        static CalLibraryParse* thisCalLibParser;
        const casacore::Record* record();

        void reset();
        void addStringParam(casacore::String key, casacore::String val);
        void addBoolParam(casacore::String key, casacore::Bool val);
        void addMapParam(casacore::String mapname, casacore::Int val);
        void addMapParam(casacore::Int val);
        void addMap();
        void addCaltable();

    private:
        void resetCaltable();
        void resetParamRecord();
        void addDefaultMap(casacore::String key);
        void issueKeywordWarning(casacore::String key);

        casacore::Record* callibRec_;     // holds (entire) cal library parsing results
        casacore::Record paramRec_;
        casacore::String caltableName_;
        casacore::String mapName_;
        casacore::Vector<casacore::Int> mapList_;
        casacore::Bool calwt_;
};

}

#endif
