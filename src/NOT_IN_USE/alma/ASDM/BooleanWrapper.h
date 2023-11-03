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
 * File Boolean.h
 */
# ifndef Boolean_CLASS
# define Boolean_CLASS
#include <alma/ASDM/NumberFormatException.h>

#include <string>

namespace asdm {
/**
 * A collection of static classes to perform conversions
 * between strings and bool values.
 * 
 */
class Boolean {

public:
	/**
	 * Static method which returns a c++ bool value from a string.
	 * returns true if the string is equal to "TRUE" or "true" and
	 * false otherwise.
	 * @param s a string.
	 * @return a bool value.
	 */
	static bool parseBoolean(const std::string &s);

	/**
	 * Static method returning a string equals to "true" (resp "false")
	 * if the parameter is equal to true (resp. false).
	 *  @param b a boolean. 
	 * 	@return a string.
	 */ 
	static std::string toString(bool b);

};

} // End namespace asdm

#endif /* Boolean_CLASS */
