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
 * File TagFormatException.h
 */

#ifndef TagFormatException_CLASS
#define TagFormatException_CLASS

#include <string>

namespace asdm {
	
/**
 * The TagFormatException occurs when a Tag is built from a String which does not represent
 * correctly the content of a Tag.
 *  
 * @author caillat
 */
  class TagFormatException {
  public:		
    
    /**
     * An empty constructor
     */
    TagFormatException();

    /**
     * 	Create an TagFormatException .
     * 	@param msg  A message indicating the cause of the exception.
     */	
    TagFormatException(std::string msg);

    /**
     * The DTOR.
     */
    
    virtual ~TagFormatException();

    /**
     * Return the name of the exception followed by its cause.
     */
    std::string getMessage() const;
    
  protected:
    std::string message;
  };
} // end namespace asdm.
#endif
