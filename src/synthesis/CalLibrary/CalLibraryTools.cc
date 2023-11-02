//# CalLibraryTools.cc: access to cal library file parser
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

#include <synthesis/CalLibrary/CalLibraryTools.h>
#include <fstream>

//# stdlib.h is needed for bison 1.28 and needs to be included here
//# (before the flex/bison files).
#include <casacore/casa/stdlib.h>
//# Define register as empty string to avoid warnings in C++11 compilers
//# because keyword register is not supported anymore.
#define register
#include "CalLibraryGram.ycc"                  // bison output
#include "CalLibraryGram.lcc"                  // flex output

// Define the yywrap function for flex.
int CalLibraryGramwrap()
{
  return 1;
}


using namespace casacore;
namespace casa {

static Int callib_linenum = 0;
static const Char* strpCalLibraryGram = 0;

Record callibSetParams(const String& calLibrary) {
    Record callibRec;
    CalLibraryParse* calParser = new CalLibraryParse();
    try {
        callibRec = calLibraryGramParseCommand(calParser, calLibrary);
    } catch(AipsError &x) {
        delete calParser;
        throw;
    }
    delete calParser;
    return callibRec; 
}

Record calLibraryGramParseCommand(CalLibraryParse* parser, const String& calLibrary) {
    ifstream myfile;
    try {
        if (calLibrary.contains("=")) {
            //calLibrary is a String
            CalLibraryGramrestart(CalLibraryGramin);
            yy_start = 1;
            String inputstr = calLibrary + "\n"; // caltable terminator
            strpCalLibraryGram = inputstr.chars();
            callib_linenum = 1;
            CalLibraryParse::thisCalLibParser = parser;
            CalLibraryGramparse();
            return *(parser->record());
        } else {
            // calLibrary is a File
            CalLibraryGramrestart(CalLibraryGramin);
            yy_start = 1;
            callib_linenum = 1;
            CalLibraryParse::thisCalLibParser = parser;

            myfile.open(calLibrary.chars());
            if (myfile.is_open()) {
                string line;
                string inputstr="";
                while (getline(myfile, line)) {
                    inputstr += line + "\n"; // caltable terminator
                }
                strpCalLibraryGram = inputstr.c_str();
                CalLibraryGramparse(); 
                myfile.close();
                return *(parser->record());
            } else {
                throw (AipsError("Cal Library parser failed to open file"));
            }
        }
    } catch(AipsError &x) {
        myfile.close();
        throw;
    }
}

//# Get the next input characters for flex.
int calLibraryGramInput (char* buf, int max_size)
{
    int nr=0;
    //clearBuf(buf, max_size);

    while (*strpCalLibraryGram != 0) {
      if (nr >= max_size) {
        break;                         // get max. max_size char.
      }
      buf[nr++] = *strpCalLibraryGram++;
    }
    return nr;
}

void clearBuf(char* buf, int size) {
    for (int i=0; i<size; i++)
        buf[i] = 0;
}

void CalLibraryGramerror(const char* s) {
    stringstream ss;
    ss << "Cal Library file: Parse error on line " << callib_linenum <<  ": " << s;
    throw (AipsError(ss.str()));
}

Int& calLibLineNum() {
    return callib_linenum;
}

}

