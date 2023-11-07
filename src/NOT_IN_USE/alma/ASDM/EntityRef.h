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
 * File EntityRef.h
 */

#ifndef EntityRef_CLASS
#define EntityRef_CLASS

#include <iostream>
#include <string>
#include <vector>

#include <alma/ASDM/EntityId.h>
#include <alma/ASDM/PartId.h>

#ifndef WITHOUT_ACS
#include <asdmIDLTypesC.h>
#endif

#include <alma/ASDM/StringTokenizer.h>
#include <alma/ASDM/InvalidArgumentException.h>
#include <alma/ASDM/InvalidDataException.h>

namespace asdm {

class EntityRef;
std::ostream & operator << ( std::ostream &, const EntityRef & );

/**
 * The EntityRef class is an identification of a persistant
 * entity in the ALMA archive.  It easily maps onto an EntityRefT
 * object in ACS system entities.
 * 
 * @version 1.00 Jan. 7, 2005
 * @author Allen Farris
 */
class EntityRef {
    friend std::ostream & operator << ( std::ostream &, const EntityRef & );

public:
	static EntityRef getEntityRef(StringTokenizer &t);

	EntityRef();
	EntityRef(const std::string &s);
#ifndef WITHOUT_ACS
	EntityRef(asdmIDLTypes::IDLEntityRef &);
#endif
	EntityRef(std::string entityId, std::string partId, std::string entityTypeName,
			std::string instanceVersion);
	virtual ~EntityRef();

	bool operator == (const EntityRef &) const;
	bool equals(const EntityRef &) const;
	bool operator != (const EntityRef &) const;

	bool isNull() const;

	std::string toString() const;
	std::string toXML() const;
#ifndef WITHOUT_ACS
    asdmIDLTypes::IDLEntityRef toIDLEntityRef() const;
#endif
	void setFromXML(std::string xml) ;
	
   /**
	 * Write the binary representation of this to a EndianOSStream.
	 */
	void toBin(EndianOSStream& eoss) const ;

   /**
    * Write the binary representation of a vector of EntityRef to an EndianOSStream.
    */
	static void toBin(const std::vector<EntityRef>& entityRef,  EndianOSStream& eoss);
	 
   /**
	 * Read the binary representation of an EntityRef from a EndianIStream
	 * and use the read value to set an  EntityRef.
	 * @param eis the EndianStream to be read
	 * @return an EntityRef
	 */
	static EntityRef fromBin(EndianIStream& eis);
	/**
	 * Read the binary representation of  a vector of  EntityRef from an EndianIStream
	 * and use the read value to set a vector of  EntityRef.
	 * @param dis the EndianIStream to be read
	 * @return a vector of EntityRef
	 */
	static std::vector<EntityRef> from1DBin(EndianIStream & eis);

	EntityId getEntityId() const;
	PartId getPartId() const;
	std::string getEntityTypeName() const;
	std::string getInstanceVersion() const;
	void setEntityId(EntityId e);
	void setPartId(PartId s);
	void setEntityTypeName(std::string s);
	void setInstanceVersion(std::string s);

private:
	EntityId entityId;
	PartId partId;
	std::string entityTypeName;
	std::string instanceVersion;

	std::string getXMLValue(std::string xml, std::string parm) const;
	std::string validXML() const;

};

// EntityRef constructors
inline EntityRef::EntityRef(const std::string &s) {
	setFromXML(s);
}

// EntityRef destructor
inline EntityRef::~EntityRef() { }

inline bool EntityRef::isNull() const {
	return entityId.isNull();
}

inline std::string EntityRef::toString() const {
	return toXML();
}

inline bool EntityRef::equals(const EntityRef &x) const {
	return *this == x;
}

// Getters and Setters

inline EntityId EntityRef::getEntityId() const {
	return entityId;
}

inline PartId EntityRef::getPartId() const {
	return partId;
}

inline std::string EntityRef::getEntityTypeName() const {
	return entityTypeName;
}

inline std::string EntityRef::getInstanceVersion() const {
	return instanceVersion;
}

inline void EntityRef::setEntityId(EntityId e) {
	entityId = e;
}

inline void EntityRef::setPartId(PartId p) {
	partId = p;
}

inline void EntityRef::setEntityTypeName(std::string s) {
	entityTypeName = s;
}

inline void EntityRef::setInstanceVersion(std::string s) {
	instanceVersion = s;
}

// Friend functions

inline std::ostream & operator << ( std::ostream &o, const EntityRef &x ) {
	o << x.toXML();
	return o;
}

} // End namespace asdm

#endif /* EntityRef_CLASS */
