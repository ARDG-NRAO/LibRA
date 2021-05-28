//# CalLibraryTools.h: access to cal library file parser
//# Copyright (C) 2015
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
//
#ifndef CALLIB_CALLIBRARYTOOLS_H
#define CALLIB_CALLIBRARYTOOLS_H

#include <casacore/casa/aips.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Containers/Record.h>
#include <synthesis/CalLibrary/CalLibraryParse.h>

namespace casa {

// <summary>
// Interface to flex/bison scanner/parser for a CalLibrary file.
// The calibration instructions are returned in a Record.
// </summary>

// <use visibility=local>

// <prerequisite>
//# Classes you should understand before using this one.
//  <li> CalLibraryGram.l and .y  (flex and bison grammar)
// </prerequisite>

// <synopsis> 
// Global functions are needed to define the input of the flex scanner
// and to start the bison parser.
// The input is taken from a cal library file or string (basically the
// contents of the file).
// </synopsis>

// The casacore::Record for the specified file is filled in by the parser.
casacore::Record callibSetParams(const casacore::String& calLibrary);

// Declare the bison parser (is implemented by bison command).
casacore::Record calLibraryGramParseCommand(CalLibraryParse* parser, const casacore::String& calLibrary);

// The yyerror function for the parser.
// It throws an exception with the current token.
void CalLibraryGramerror(const char* s);

// Declare the input routine for flex/bison.
int calLibraryGramInput (char* buf, int max_size);
void clearBuf(char* buf, int size);

// Current line number in file, for parse errors
casacore::Int& calLibLineNum();

// Give the Record
const casacore::Record* calLibraryRecord();
}

#endif
