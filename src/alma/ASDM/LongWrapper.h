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
 * File Long.h
 */
# ifndef Long_CLASS
# define Long_CLASS
#include <alma/ASDM/NumberFormatException.h>

#include <stdint.h>
#include <string>

namespace asdm {
/**
 * A collection of static methods to perform conversions
 * between strings and int64_t values.
 */
class Long {

public:
	/**
	 * Parse a string supposed to represent a int64_t value and returns this value.
	 * @param s the string to parse
	 * @return a int64_t.
	 * @throws NumberFormatException.
	 */
	static int64_t parseLong(const std::string &s);

	/**
	 * Encode a int64_t value into its string representation.
	 * @param l the int64_t value to be encoded.
	 * @return the string representing the int64_t value passed as parameter.
	 */
	static std::string toString(int64_t l);


     /**
	 * The minimum value for a int64_t.
	 */
    static const int64_t MIN_VALUE;
    
   /**
	 * The maximum value for a int64_t.
	 */
    static const int64_t MAX_VALUE;

};

} // End namespace asdm

#endif /* Long_CLASS */
