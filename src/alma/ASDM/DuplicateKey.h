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
 * File DuplicateKey.h
 */

#ifndef DuplicateKey_CLASS
#define DuplicateKey_CLASS

#include <string>

namespace asdm {

/**
 * Generate an exception when a new row cannot be inserted 
 * because it contains a duplicate key. 
 */
class DuplicateKey {

public:
  /**
   * An empty CTOR.
   */
  DuplicateKey();

  /**
   * Create an exception if an error occurs in converting a table
   * to its external representation or in restoring a table from
   * one of its external representations.
   * @param m The conversion error.
   * @param t The table being converted.
   */
  DuplicateKey(const std::string& m, const std::string& t);
  

  /**
   * The DTOR.
   */
  ~DuplicateKey();

  /**
   * @return a text describing the exception.
   */
  std::string getMessage() const;
  
 protected:  
  std::string message;

 private:
  static const std::string ITSNAME;
  
};
 


} // End namespace asdm

#endif /* DuplicateKey_CLASS */
