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
 * File StringTokenizer.cpp
 */

#include <alma/ASDM/StringTokenizer.h>

#include <iostream>
using namespace std;

namespace asdm {

StringTokenizer::StringTokenizer(const string &s, const string &delim, bool returnDelims) {
	currentPosition = 0;
	newPosition = -1;
	delimsChanged = false;
	maxPosition = s.length();
	str = new char [maxPosition + 1];
	int i = 0;
	for (i = 0; i < maxPosition; i++)
		str[i] = s.at(i);
	str[i] = '\0';
	ostr = new string (str);
	numDelimiters = delim.length();
	strDelimiter = delim;
	//delimiter = new char [numDelimiters];
	delimiter = strDelimiter.data();
	//for (i = 0; i < numDelimiters; i++)
	//	delimiter[i] = delim.at(i);
	retDelims = returnDelims;
    setMaxDelimChar();
}

StringTokenizer::~StringTokenizer() {
	delete [] str;
	delete ostr;
	//delete [] delimiter;
}

void StringTokenizer::setMaxDelimChar() {
    if (numDelimiters == 0) {
        throw InvalidArgumentException("Delimiters cannot be null.");
    }
	char m = 0;
	for (int i = 0; i < numDelimiters; i++) {
	    char c = delimiter[i];
	    if (m < c)
			m = c;
	}
	maxDelimChar = m;
}

int StringTokenizer::indexOfDelimiters(char c) {
        // int nd = 0;  // set but never used
	for (int i = 0; i < numDelimiters; ++i) {
	        //  nd = delimiter[i];  // set but never used
		if (c == delimiter[i]) {
			return i;
		}
	}
	return -1;
}

int StringTokenizer::skipDelimiters(int startPos) {
    if (numDelimiters == 0) {
        throw InvalidArgumentException("Delimiters cannot be null.");
    }
    int position = startPos;
	while (!retDelims && position < maxPosition) {
        char c = str[position];
        if ((c > maxDelimChar) || (indexOfDelimiters(c) < 0))
            break;
        position++;
	}
    return position;
}

int StringTokenizer::scanToken(int startPos) {
    int position = startPos;
    while (position < maxPosition) {
        char c = str[position];
        if ((c <= maxDelimChar) && (indexOfDelimiters(c) >= 0))
            break;
        position++;
	}
	if (retDelims && (startPos == position)) {
        char c = str[position];
		if ((c <= maxDelimChar) && (indexOfDelimiters(c) >= 0))
            position++;
    }
    return position;
}

bool StringTokenizer::hasMoreTokens() {
	// Temporary store this position and use it in the following
	// nextToken() method only if the delimiters have'nt been changed in
	// that nextToken() invocation.
	newPosition = skipDelimiters(currentPosition);
	return (newPosition < maxPosition);
}

string StringTokenizer::nextToken() {
	// If next position already computed in hasMoreElements() and
	// delimiters have changed between the computation and this invocation,
	// then use the computed value.

	currentPosition = (newPosition >= 0 && !delimsChanged) ?  
	    newPosition : skipDelimiters(currentPosition);

	// Reset these anyway
	delimsChanged = false;
	newPosition = -1;

	if (currentPosition >= maxPosition)
	    throw OutOfBoundsException("No more tokens.");
	int start = currentPosition;
	currentPosition = scanToken(currentPosition);
	return ostr->substr(start, (currentPosition - start));
}

string StringTokenizer::nextToken(const string delim) {
	numDelimiters = delim.length();
	strDelimiter = delim;
	delimiter = strDelimiter.data();
	//delimiter = new char [numDelimiters];
	//for (int i = 0; i < numDelimiters; ++i)
	//	delimiter[i] = delim.at(i);

	// delimiter string specified, so set the appropriate flag.
	delimsChanged = true;

    setMaxDelimChar();
	return nextToken();
}

int StringTokenizer::countTokens() {
	int count = 0;
	int currpos = currentPosition;
	while (currpos < maxPosition) {
        currpos = skipDelimiters(currpos);
	    if (currpos >= maxPosition)
			break;
        currpos = scanToken(currpos);
	    count++;
	}
	return count;
}

} // End namespace.
