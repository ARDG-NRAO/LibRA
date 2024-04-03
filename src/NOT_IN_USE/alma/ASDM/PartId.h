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
 * File PartId.h
 */

#ifndef PartId_CLASS
#define PartId_CLASS

#include <string>

#include <alma/ASDM/InvalidArgumentException.h>

#include <alma/ASDM/EndianStream.h>

namespace asdm {

/**
 * description
 * 
 * @version 1.00 Jan. 7, 2005
 * @author Allen Farris
 */
class PartId {

public:
	static std::string validate(std::string x);

	PartId();
	PartId(const PartId &);
	PartId(const std::string &id);
	virtual ~PartId();

	bool equals(const PartId &) const;

	std::string toString() const;
	
	void setId(const std::string &s);

	/**
	 * Write the binary representation of this to a EndianOSStream.
	 */		
	void toBin(EndianOSStream& eoss) const;
	
	/**
	 * Read the binary representation of a PartId  from a EndianIStream
	 * and use the read value to set an  PartId.
	 * @param eis the EndianStream to be read
	 * @return a PartId
	 * @throw InvalidArgumentException
	 */
	static PartId fromBin(EndianIStream& eis);
private:
	std::string id;

};

// PartId constructors
inline PartId::PartId() { }

inline PartId::PartId(const PartId &x) : id(x.id) { }

// PartId destructor
inline PartId::~PartId() { }

inline bool PartId::equals(const PartId &x) const {
	return id == x.id;
}

inline std::string PartId::toString() const {
	return id;
}

} // End namespace asdm

#endif /* PartId_CLASS */
