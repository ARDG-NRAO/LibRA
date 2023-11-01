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
 * File Entity.h
 */

#ifndef Entity_CLASS
#define Entity_CLASS

#include <iostream>
#include <string>

#ifndef WITHOUT_ACS
#include <asdmIDLTypesC.h>
#endif

#include <alma/ASDM/EntityId.h>

#include <alma/ASDM/StringTokenizer.h>
#include <alma/ASDM/InvalidArgumentException.h>
#include <alma/ASDM/InvalidDataException.h>

#include <alma/ASDM/EndianStream.h>

namespace asdm {

class Entity;
std::ostream & operator << ( std::ostream &, const Entity & );

/**
 * The Entity class is an identification of a persistant
 * entity in the ALMA archive.  It easily maps onto an EntityT
 * object in ACS system entities.
 * 
 * @throw InvalidArgumentException
 * @version 1.00 Jan. 7, 2005
 * @author Allen Farris
 */
class Entity {
    friend std::ostream & operator << ( std::ostream &, const Entity & );
    friend std::istream & operator >> ( std::istream &, Entity & );

public:
	static Entity getEntity(StringTokenizer &t);
	Entity();
	Entity(const std::string &s);
#ifndef WITHOUT_ACS
	Entity(asdmIDLTypes::IDLEntity &);
#endif
	Entity(std::string entityId, std::string entityIdEncrypted, std::string entityTypeName,
			std::string entityVersion, std::string instanceVersion);
	virtual ~Entity();

	bool operator == (const Entity &) const;
	bool equals(const Entity &) const;
	bool operator != (const Entity &) const;

	bool isNull() const;

	std::string toString() const;
	std::string toXML() const;
#ifndef WITHOUT_ACS
    asdmIDLTypes::IDLEntity toIDLEntity() const;
#endif
	void setFromXML(std::string xml);
	
	/**
	 * Write the binary representation of this to a EndianOSStream.
	 */		
	void toBin(EndianOSStream& eoss) const ;
	
	/**
	 * Read the binary representation of an Enity from a EndianIStream
	 * and use the read value to set an  Entity.
	 * @param eis the EndianStream to be read
	 * @return an Entity
	 */
	static Entity fromBin(EndianIStream& eis);

	
	EntityId getEntityId() const;
	std::string getEntityIdEncrypted() const;
	std::string getEntityTypeName() const;
	std::string getEntityVersion() const;
	std::string getInstanceVersion() const;
	void setEntityId(EntityId e);
	void setEntityIdEncrypted(std::string s);
	void setEntityTypeName(std::string s);
	void setEntityVersion(std::string s);
	void setInstanceVersion(std::string s);

private:
	EntityId entityId;
	std::string entityIdEncrypted;
	std::string entityTypeName;
	std::string entityVersion;
	std::string instanceVersion;

	std::string getXMLValue(std::string xml, std::string parm) const;
	std::string validXML() const;

};



} // End namespace asdm

#endif /* Entity_CLASS */
