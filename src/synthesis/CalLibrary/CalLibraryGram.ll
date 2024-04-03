/* -*- C -*-
    CalLibraryGram.ll: Lexical analyzer for cal library files
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

/* yy_unput is not used, so let flex not generate it, otherwise picky
   compilers will issue warnings. */
%option nounput

%{
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) result = calLibraryGramInput(buf, max_size)
#undef YY_DECL
#define YY_DECL int CalLibraryGramlex (YYSTYPE* lvalp)
#include <string.h>
%}

QSTRING		\"[^\"\n]*\"
ASTRING		\'[^\'\n]*\'
STRINGS		({QSTRING}|{ASTRING})+
%x ORDERED_LIST

%%
[ \t]             ;
^\n               { ++(calLibLineNum()); }
^[ \t]*#.*\n      { ++(calLibLineNum()); } // comment
True|T|true       { lvalp->bval = true; return BOOLEAN; }
False|F|false     { lvalp->bval = false; return BOOLEAN; }
[0-9]+\.[0-9]+    { lvalp->fval = atof(CalLibraryGramtext); return FLOAT; }
[0-9]+            { lvalp->ival = atoi(CalLibraryGramtext); return INT; }
"="               { return EQ; }
[,\n]             { ++(calLibLineNum()); return ENDL; }

[a-zA-Z0-9]+   {
	// copy in case CalLibraryGramtext changes underneath us:
	lvalp->sval = strdup(CalLibraryGramtext);
	return STRING; }
{STRINGS} {
	// copy in case CalLibraryGramtext changes underneath us:
	std::string str = strdup(CalLibraryGramtext);
    char * cstr = new char[yyleng];
	strcpy(cstr, str.substr(1, yyleng - 2).c_str());
	lvalp->sval = cstr;
	return STRING; }

"["                   { BEGIN(ORDERED_LIST); }   // begin ordered list
<ORDERED_LIST>[ ,]    ;                          // ignore spaces and commas
<ORDERED_LIST>[0-9]+  { lvalp->idx_int = atoi(CalLibraryGramtext);
                        return ARRAY_INT; }
<ORDERED_LIST>"]"     { BEGIN(INITIAL);          // end of list, back to normal
                        return END_LIST; }

.                     ;
