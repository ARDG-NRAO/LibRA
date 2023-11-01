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
 * File StringTokenizer.h
 */
# ifndef StringTokenizer_CLASS
# define StringTokenizer_CLASS

#include <alma/ASDM/InvalidArgumentException.h>
#include <alma/ASDM/OutOfBoundsException.h>

#include <string>

namespace asdm {

/**
 * The StringTokenizer class is a translation into C++
 * of the Java class of the same name in Java's util 
 * pacakge.  See the Java class documentation.
 */
class StringTokenizer {

public:

    StringTokenizer(const std::string &s, 
    	const std::string &delim = " \t\n\r\f", 
    	bool returnDelims = false);
	virtual ~StringTokenizer();

    bool hasMoreTokens(); 
    std::string nextToken();
    std::string nextToken(const std::string delim);
    int countTokens();

private:

    int currentPosition;
    int newPosition;
    int maxPosition;
    char *str;
	std::string *ostr;
	std::string strDelimiter;
    const char *delimiter;
	int numDelimiters;
    bool retDelims;
    bool delimsChanged;

    char maxDelimChar;

    void setMaxDelimChar();
    int skipDelimiters(int startPos);
    int scanToken(int startPos);
	int indexOfDelimiters(char c);

};

} // End namespace asdm

#endif /* StringTokenizer_CLASS */
