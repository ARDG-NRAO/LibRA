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
 * File PartId.cpp
 */

#include <alma/ASDM/PartId.h>
#include <alma/ASDM/OutOfBoundsException.h>
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {

	/**
	 * Returns a null string if the string x contains a valid
	 * entity-id.  Otherwise, the string contains the error message.
	 */
	string PartId::validate(string x) {
		string msg = "Invalid format for PartId: " + x;
		// Check the partId for the correct format.
		if (x.length() == 0)
			return "";
		if (x.length() != 9 || x.at(0) != 'X')
			return msg;
		for (int i = 1; i < 9; ++i) {
			if (!((x.at(i) >= '0' && x.at(i) <= '9') ||
					(x.at(i) >= 'a' && x.at(i) <= 'f')))
				return msg;
		}
		return "";				
	}

	PartId::PartId(const string &id) {
		string msg = validate(id);
		if (msg.length() != 0)
			throw InvalidArgumentException(msg);
		this->id = id;		
	}

	void PartId::setId(const string &id) {
		string msg = validate(id);
		if (msg.length() != 0)
			throw InvalidArgumentException(msg);
		this->id = id;		
	}
	
	void PartId::toBin(EndianOSStream& eoss) const {
		eoss.writeString(id);	
	}
	
	PartId PartId::fromBin(EndianIStream& eis) {
		return PartId(eis.readString());	
	}

} // End namespace asdm
