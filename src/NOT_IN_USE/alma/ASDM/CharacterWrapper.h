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
 * File Character.h
 */
# ifndef Character_CLASS
# define Character_CLASS
#include <alma/ASDM/NumberFormatException.h>

#include <string>

namespace asdm {
/**
 * A collection of static classes to perform conversions
 * between strings and char values.
 * 
 */
class Character {

public:

	/**
	 * Returns the first character of s or 0 if s has a null length.
	 * @param s a string
	 * @return an unsigned char
	 */
	static unsigned char parseCharacter(const std::string &s);

	/**
	 * Returns a string constructed from a the char passed as parameter.
	 * @param c a char
	 * @return a string
	 */
	static std::string toString(unsigned char c);

};

} // End namespace asdm

#endif /* Character_CLASS */
