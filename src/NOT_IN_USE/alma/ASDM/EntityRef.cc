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
 * File EntityRef.cpp
 */

#include <alma/ASDM/EntityRef.h>
#include <alma/ASDM/OutOfBoundsException.h>
#include <alma/ASDM/InvalidArgumentException.h>
#include <alma/ASDM/InvalidDataException.h>

using namespace std;

namespace asdm {

	EntityRef EntityRef::getEntityRef(StringTokenizer &t) {
		try {
			string s = t.nextToken("<>");
			if (s == " ")
				s = t.nextToken();
			EntityRef e;
			e.setFromXML(s);
			return e;
		} catch (const OutOfBoundsException &err) {
			throw InvalidArgumentException("Unexpected end-of-string!");
		}
	}

	EntityRef::EntityRef() : entityId(), partId() {
		entityTypeName = "";
		instanceVersion = "";
	}

#ifndef WITHOUT_ACS
	EntityRef::EntityRef(asdmIDLTypes::IDLEntityRef &x) : entityId(string(x.entityId)),
			partId(string(x.partId)) {
		entityTypeName = string(x.entityTypeName);
		instanceVersion = string(x.instanceVersion);
	}
#endif

	EntityRef::EntityRef(string id, string pId, string sTypeName,
			string sInstanceVersion) : entityId(id), partId(pId) {
		entityTypeName = sTypeName;
		instanceVersion = sInstanceVersion;
	}

	bool EntityRef::operator == (const EntityRef& e) const {
		return 	entityId.getId() == e.entityId.getId() &&
				partId.toString() == e.partId.toString() &&
				entityTypeName == e.entityTypeName &&
				instanceVersion == e.instanceVersion;
	}

	bool EntityRef::operator != (const EntityRef& e) const {
		return 	entityId.getId() != e.entityId.getId() ||
				partId.toString() != e.partId.toString()||
				entityTypeName != e.entityTypeName ||
				instanceVersion != e.instanceVersion;
	}

	string EntityRef::getXMLValue(string xml, string parm) const {
		string::size_type n = xml.find(parm,0);
		if (n == string::npos)
			return "";
		string::size_type beg = xml.find("\"",n + parm.length());
		if (beg == string::npos)
			return "";
		beg++;
		string::size_type end = xml.find("\"",beg);
		if (end == string::npos)
			return "";
		return xml.substr(beg,(end - beg));
	}

	string EntityRef::validXML() const {
		// Check for any null values. PartId may be null.
		string msg = "Null values detected in EntityRef " + entityId.getId();
		if (entityId.isNull() ||
			entityTypeName.length() == 0 ||
			instanceVersion.length() == 0)
			return msg;
		// Check the entityId for the correct format.
		msg = EntityId::validate(entityId.toString());
		if (msg.length() != 0)
			return msg;
		// Check the entityId for the correct format.
		if (partId.toString().length() != 0)
			return PartId::validate(partId.toString());
		return "";
	}

	/**
	 * Return the values of this EntityRef as an XML-formated string.
	 * As an example, for the Main table in the ASDM, the toXML 
	 * method would give:
	 * <ul>
	 * <li>	&lt;EntityRef 
	 * <li>		entityId="uid://X0000000000000079/X00000000" 
	 * <li>		partId="X00000002" 
	 * <li>		entityTypeName="Main" 
	 * <li>		documentVersion="1"/&gt;
	 * </ul>
	 * 
	 * @return The values of this EntityRef as an XML-formated string.
	 */
	string EntityRef::toXML() const {
		string msg = validXML();
		if (msg.length() != 0) 
			throw InvalidDataException(msg);
		string s = "<EntityRef entityId=\"" + entityId.toString();
		if (partId.toString().length() != 0)
			s += "\" partId=\"" + partId.toString();
		s += "\" entityTypeName=\"" + entityTypeName + 
			 "\" documentVersion=\"" + instanceVersion + "\"/>";
		return s;
	}

#ifndef WITHOUT_ACS
	asdmIDLTypes::IDLEntityRef EntityRef::toIDLEntityRef() const {
		asdmIDLTypes::IDLEntityRef e;
		e.entityId = CORBA::string_dup(entityId.getId().c_str());
		e.partId = CORBA::string_dup(partId.toString().c_str());
		e.entityTypeName = CORBA::string_dup(entityTypeName.c_str());
		e.instanceVersion = CORBA::string_dup(instanceVersion.c_str());
		return e;
	}
#endif

	void EntityRef::setFromXML(string xml)  {
		entityId.setId(getXMLValue(xml,"entityId"));
		partId.setId(getXMLValue(xml,"partId"));
		entityTypeName = getXMLValue(xml,"entityTypeName");
		instanceVersion = getXMLValue(xml,"documentVersion");
		string msg = validXML();
		if (msg.length() != 0) {
		  throw InvalidArgumentException(msg);
		}
	}
	
	void EntityRef::toBin(EndianOSStream& eoss) const {
		entityId.toBin(eoss);
		partId.toBin(eoss);
		eoss.writeString(entityTypeName);
		eoss.writeString(instanceVersion);
	}	

	void EntityRef::toBin(const vector<EntityRef>& entityRef,  EndianOSStream& eoss) {
		eoss.writeInt((int) entityRef.size());
		for (unsigned int i = 0; i < entityRef.size(); i++)
			entityRef.at(i).toBin(eoss);
	}

	EntityRef EntityRef::fromBin(EndianIStream& eis) {
		return EntityRef(eis.readString(), eis.readString(), eis.readString(), eis.readString());
	}

	vector<EntityRef> EntityRef::from1DBin(EndianIStream & eis) {
		int dim1 = eis.readInt();
		vector<EntityRef> result;
		for (int i = 0; i < dim1; i++)
			result.push_back(EntityRef(eis.readString(), eis.readString(), eis.readString(), eis.readString()));
		return result;
	}

} // End namespace asdm
