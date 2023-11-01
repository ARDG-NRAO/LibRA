/* -*- C++ -*-
    CalLibraryGram.yy: Parser for cal library files
    Copyright (C) 2015
    Associated Universities, Inc. Washington DC, USA.

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; if not, write to the Free Software Foundation,
    Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.

    Correspondence concerning AIPS++ should be addressed as follows:
           Internet email: aips2-request@nrao.edu.
           Postal address: AIPS++ Project Office
                           National Radio Astronomy Observatory
                           520 Edgemont Road
                           Charlottesville, VA 22903-2475 USA
*/

%{
#include <errno.h>
using namespace casacore;
using namespace casa;
%}

%pure-parser                /* make parser re-entrant */

%union {
	Int ival;
	Float fval;
	Char *sval;
    Bool bval;
    Int idx_int;
    const Record* rec;
}

%token ENDL
%token EQ

%token <ival> INT
%token <fval> FLOAT
%token <sval> STRING
%token <bval> BOOLEAN
%token <idx_int> ARRAY_INT;
%token END_LIST;

%type <rec> callibrary

%{
int CalLibraryGramlex (YYSTYPE*);
%}

%%

callibrary: calibration_lines
    {
        $$ = CalLibraryParse().record();
    }
    ;
calibration_lines:
    calibration_lines calibration_line
  | calibration_line
  ;
calibration_line:
    calibration_line calibration_param
  | calibration_param
  ;
calibration_param:
    STRING EQ STRING {
        CalLibraryParse::thisCalLibParser->addStringParam($1, $3);
        }
  | STRING EQ BOOLEAN {
        CalLibraryParse::thisCalLibParser->addBoolParam($1, $3);
        }
  | STRING EQ ARRAY_INT  {
        CalLibraryParse::thisCalLibParser->addMapParam($1, $3);
        }
  | ARRAY_INT  {
        CalLibraryParse::thisCalLibParser->addMapParam($1);
        }
  | END_LIST {
        CalLibraryParse::thisCalLibParser->addMap();
        }
  | ENDL {   // end of this caltable
        CalLibraryParse::thisCalLibParser->addCaltable();
        }
  ; 

%%


