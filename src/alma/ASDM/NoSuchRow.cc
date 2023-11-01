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
 * File NoSuchRow.cpp
 */
#include <alma/ASDM/NoSuchRow.h>

using std::string;

namespace asdm {
  const string NoSuchRow::ITSNAME = "No such row exception: ";

  NoSuchRow::NoSuchRow() : message ("") { ; }
  
  NoSuchRow::NoSuchRow(int rowNumber, string tableName) :
    message("No such row as number " + Integer::toString(rowNumber) + 
	    " in table " + tableName) {
  }

  NoSuchRow::NoSuchRow(string key, string tableName) :
    message("No such row with key " + key + " in table " + tableName) {
  }

  NoSuchRow::NoSuchRow(int N, string toTableName, string fromTableName) :
    message("No such link as number " + Integer::toString(N) + " to table " + 
	    toTableName + " in this row of table " + fromTableName) {
  }

  NoSuchRow::NoSuchRow(string key, string toTableName, string fromTableName) :
    message("No such link with key " + key + " to table " + 
	    toTableName + " in this row of table " + fromTableName) {
  }

  NoSuchRow::NoSuchRow(string toTableName, string fromTableName, bool /* option */ ) :
    message("The optional link to table " + toTableName + " in this row of table " + 
	    fromTableName + " does not exist! ")  {
  }
  
  NoSuchRow::~NoSuchRow() { ; }

  string NoSuchRow::getMessage() const {
    return ITSNAME + message;
  }  
} // end namsespace asdm
