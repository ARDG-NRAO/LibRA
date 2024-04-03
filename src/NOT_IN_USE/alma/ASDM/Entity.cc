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
 * File Entity.cpp
 */

#include <alma/ASDM/Entity.h>
#include <alma/ASDM/OutOfBoundsException.h>
#include <alma/ASDM/InvalidArgumentException.h>
#include <alma/ASDM/InvalidDataException.h>

using namespace std;

namespace asdm {

  // Entity constructors
  Entity::Entity(const string &s) {
    setFromXML(s);
  }

  // Entity destructor
  Entity::~Entity() { }

  bool Entity::isNull() const {
    return entityId.isNull();
  }

  string Entity::toString() const {
    return toXML();
  }

  bool Entity::equals(const Entity &x) const {
    return *this == x;
  }

  // Getters and Setters

  EntityId Entity::getEntityId() const {
    return entityId;
  }

  string Entity::getEntityIdEncrypted() const {
    return entityIdEncrypted;
  }

  string Entity::getEntityTypeName() const {
    return entityTypeName;
  }

  string Entity::getEntityVersion() const {
    return entityVersion;
  }

  string Entity::getInstanceVersion() const {
    return instanceVersion;
  }

  void Entity::setEntityId(EntityId e) {
    entityId = e;
  }

  void Entity::setEntityIdEncrypted(string s) {
    entityIdEncrypted = s;
  }

  void Entity::setEntityTypeName(string s) {
    entityTypeName = s;
  }

  void Entity::setEntityVersion(string s) {
    entityVersion = s;
  }

  void Entity::setInstanceVersion(string s) {
    instanceVersion = s;
  }

  // Friend functions

  ostream & operator << ( ostream &o, const Entity &x ) {
    o << x.toXML();
    return o;
  }

  Entity Entity::getEntity(StringTokenizer &t) {
    try {
      string s = t.nextToken("<>");
      if (s == " ")
	s = t.nextToken();
      Entity e;
      e.setFromXML(s);
      return e;
    } catch (const OutOfBoundsException &err) {
      throw InvalidArgumentException("Unexpected end-of-string!");
    }
  }

  Entity::Entity() : entityId() {
    entityIdEncrypted = "";
    entityTypeName = "";
    entityVersion = "";
    instanceVersion = "";
  }

#ifndef WITHOUT_ACS
  Entity::Entity(asdmIDLTypes::IDLEntity &x) : entityId(string(x.entityId)) {
    entityIdEncrypted = string(x.entityIdEncrypted);
    entityTypeName = string(x.entityTypeName);
    entityVersion = string(x.entityVersion);
    instanceVersion = string(x.instanceVersion);
  }
#endif

  Entity::Entity(string id, string sEncrypted, string sTypeName,
		 string sVersion, string sInstanceVersion) : entityId(id){
    entityIdEncrypted = sEncrypted;
    entityTypeName = sTypeName;
    entityVersion = sVersion;
    instanceVersion = sInstanceVersion;
  }

  bool Entity::operator == (const Entity& e) const {
    return 	entityId.getId() == e.entityId.getId() &&
      entityIdEncrypted == e.entityIdEncrypted &&
      entityTypeName == e.entityTypeName &&
      entityVersion == e.entityVersion &&
      instanceVersion == e.instanceVersion;
  }

  bool Entity::operator != (const Entity& e) const {
    return 	entityId.getId() != e.entityId.getId() ||
      entityIdEncrypted != e.entityIdEncrypted ||
      entityTypeName != e.entityTypeName ||
      entityVersion != e.entityVersion ||
      instanceVersion != e.instanceVersion;
  }

  string Entity::getXMLValue(string xml, string parm) const {
    string::size_type n = xml.find(parm+"=",0);
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

  string Entity::validXML() const {
    // Check for any null values.
    string msg = "Null values detected in Entity " + entityId.getId();
    if (entityId.isNull() ||
	entityIdEncrypted.length() == 0 ||
	entityTypeName.length() == 0 ||
	entityVersion.length() == 0 ||
	instanceVersion.length() == 0)
      return msg;
    // Check the entityId for the correct format.
    return EntityId::validate(entityId.toString());
  }

  /**
   * Return the values of this Entity as an XML-formated string.
   * As an example, for the Main table in the ASDM, the toXML 
   * method would give:
   * <ul>
   * <li>	&lt;Entity 
   * <li>		entityId="uid://X0000000000000079/X00000000" 
   * <li>		entityIdEncrypted="none" 
   * <li>		entityTypeName="Main" 
   * <li>		schemaVersion="1" 
   * <li>		documentVersion="1"/&gt;
   * </ul>
   * 
   * @return The values of this Entity as an XML-formated string.
   * @throws IllegalStateException if the values of this Entity do not conform to the proper XML format.
   */
  string Entity::toXML() const {
    string msg = validXML();
    if (msg.length() != 0) 
      throw InvalidDataException(msg);
    string s = "<Entity entityId=\"" + entityId.toString() +
      "\" entityIdEncrypted=\"" + entityIdEncrypted +
      "\" entityTypeName=\"" + entityTypeName + 
      "\" schemaVersion=\"" + entityVersion +
      "\" documentVersion=\"" + instanceVersion + "\"/>";
    return s;
  }

#ifndef WITHOUT_ACS
  asdmIDLTypes::IDLEntity Entity::toIDLEntity() const {
    asdmIDLTypes::IDLEntity e;
    e.entityId = CORBA::string_dup(entityId.getId().c_str());
    e.entityIdEncrypted = CORBA::string_dup(entityIdEncrypted.c_str());
    e.entityTypeName = CORBA::string_dup(entityTypeName.c_str());
    e.entityVersion = CORBA::string_dup(entityVersion.c_str());
    e.instanceVersion = CORBA::string_dup(instanceVersion.c_str());
    return e;
  }
#endif

  void Entity::setFromXML(string xml) {
    entityId.setId(getXMLValue(xml,"entityId"));
    entityIdEncrypted = getXMLValue(xml,"entityIdEncrypted");
    entityTypeName = getXMLValue(xml,"entityTypeName");
    entityVersion = getXMLValue(xml,"schemaVersion");
    instanceVersion = getXMLValue(xml,"documentVersion");
    if (entityIdEncrypted.length() == 0 ||
	entityTypeName.length() == 0 ||
	entityVersion.length() == 0 ||
	instanceVersion.length() == 0)
      throw InvalidArgumentException("Null values detected in Entity " + entityId.toString());
  }
		
  void Entity::toBin(EndianOSStream& eoss) const {
    entityId.toBin(eoss);
    eoss.writeString(entityIdEncrypted);
    eoss.writeString(entityTypeName);
    eoss.writeString(entityVersion);
    eoss.writeString(instanceVersion);
  }
	
  Entity Entity::fromBin(EndianIStream& eis) {
    Entity entity;
    entity.setEntityId(EntityId(eis.readString()));
    entity.setEntityIdEncrypted(eis.readString());
    entity.setEntityTypeName(eis.readString());
    entity.setEntityVersion(eis.readString());
    entity.setInstanceVersion(eis.readString());
    return entity;
  }
} // End namespace asdm
