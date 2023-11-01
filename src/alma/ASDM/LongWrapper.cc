/*
 * ALMA - Atacama Large Millimeter Array
 * (c) European Southern Observatory, 2002
 * (c) Associated Universities Inc., 2002
 * Copyright by ESO (in the framework of the ALMA collaboration),
 * Copyright by AUI (in the framework of the ALMA collaboration),
 * All rights reserved.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307  USA
 *
 * File Long.cpp
 */

#include <alma/ASDM/LongWrapper.h>
#include <alma/ASDM/NumberFormatException.h>

#include <sstream>

#include <iostream>
using namespace std;

namespace asdm {

    const int64_t Long::MIN_VALUE = 0x8000000000000000LL;
    const int64_t Long::MAX_VALUE = 0x7fffffffffffffffLL;

	int64_t Long::parseLong(const string &s) {
//		istringstream *in = new istringstream(s.c_str());
	    istringstream in;
	    in.str(s.c_str());
		int64_t x;
//		(*in) >> x;
	    in >> x;
//		if (in->rdstate() == istream::failbit)
		if (in.rdstate() == istream::failbit)
			throw NumberFormatException (s);
//		delete in;
		return x;
	}

	string Long::toString(int64_t x) {
		ostringstream out ;
		out.width(35);
		out.precision(25);
		out << x;
		string s = out.str();
		int i = 0;
		while (i < 35 && s.at(i) == ' ')
			++i;
		return s.substr(i,s.length() - i);
	}

} // End namespace.
