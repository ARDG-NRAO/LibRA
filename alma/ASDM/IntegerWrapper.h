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
 * File Integer.h
 */
# ifndef Integer_CLASS
# define Integer_CLASS
#include <alma/ASDM/NumberFormatException.h>

#include <string>

namespace asdm {

class Integer {
/**
 * A collection of static methods to perform conversions
 * between strings and integer values.
 */
public:
	/**
	 * Parse a string supposed to represent an integer value and returns this value.
	 * @param s the string to parse
	 * @return an int.
	 * @throws NumberFormatException.
	 */
	static int parseInt(const std::string &s);

	/**
	 * Encode an integer value into its string representation.
	 * @param i the integer value to be encoded.
	 * @return the string representing the integer value passed as parameter.
	 */
	static std::string toString(int i);

     /**
	 * The minimum value for an integer.
	 */
    static const int MIN_VALUE;
    
 	/**
	 * The maximum value for an integer.
	 */   
    static const int MAX_VALUE;

};

} // End namespace asdm

#endif /* Integer_CLASS */
