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
 * File Byte.cpp
 */

#include <alma/ASDM/ByteWrapper.h>
#include <alma/ASDM/NumberFormatException.h>

#include <sstream>

#include <iostream>
using namespace std;

namespace asdm {

    const char Byte::MIN_VALUE = -128;
    const char Byte::MAX_VALUE = 127;

	char Byte::parseByte(const string &s) {
		istringstream in; 
		in.str(s);
		short x;
		in >> x;
		if (in.rdstate() == istream::failbit || x < MIN_VALUE || x > MAX_VALUE)
			throw NumberFormatException (s);
		return (char)x;
	}

	string Byte::toString(char x) {
		ostringstream out;
		out.width(10);
		out.precision(6);
		out << x;
		string s = out.str();
		int i = 0;
		while (i < 10 && s.at(i) == ' ')
			++i;
		return s.substr(i,s.length() - i);
	}

} // End namespace.
