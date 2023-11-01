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
 * File EntityId.cpp
 */
//#define _POSIX_C_SOURCE

#ifndef WITHOUT_BOOST
#include <boost/regex.hpp>
#else
#include <regex>
#endif

#include <alma/ASDM/EntityId.h>
#include <alma/ASDM/OutOfBoundsException.h>
#include <alma/ASDM/InvalidArgumentException.h>
using namespace std;

namespace asdm {

  static bool EntityIdVerbose = getenv("ASDM_DEBUG") != NULL;

  EntityId EntityId::getEntityId(StringTokenizer &t) {
    try {
      string value = t.nextToken();
      return EntityId (value);
    } catch (const OutOfBoundsException &err) {
      throw InvalidArgumentException("Unexpected end-of-string!");
    }
  }
  
  /**
   * Returns a null string if the string x contains a valid
   * entity-id.  Otherwise, the string contains the error message.
   */
  string EntityId::validate(string x) {

    if (EntityIdVerbose) 
      cout << "EntityId::validate. Validating '" << x << "'." << endl;   

    // Here we use the boost machinery for the regular expressions, assuming its thread safety.
    // std::regex (optionally used here) is as thread-safe in this context

    // We will test two possible syntaxes for the EntityId, since EVLA did not want to follow
    // the rules established by ALMA.
    //
#ifndef WITHOUT_BOOST
    boost::regex ALMAbasicUIDRegex("^[uU][iI][dD]://[0-9a-zA-Z]+(/[xX][0-9a-fA-F]+){2}(#\\w{1,}){0,}$");
    boost::regex EVLAbasicUIDRegex("^[uU][iI][dD]:///?evla/[0-9a-zA-Z]+/.+$");

    const char * theUID_p = x.c_str();
    boost::cmatch whatALMA, whatEVLA;
    string result = "";

    if (!(boost::regex_match(theUID_p, whatALMA, ALMAbasicUIDRegex) ||
	  boost::regex_match(theUID_p, whatEVLA, EVLAbasicUIDRegex))
	)
      result  = "Invalid format for EntityId: " + x;
#else
    std::regex ALMAbasicUIDRegex("^[uU][iI][dD]://[0-9a-zA-Z]+(/[xX][0-9a-fA-F]+){2}(#\\w{1,}){0,}$");
    std::regex EVLAbasicUIDRegex("^[uU][iI][dD]:///?evla/[0-9a-zA-Z]+/.+$");

    const char * theUID_p = x.c_str();
    std::cmatch whatALMA, whatEVLA;
    string result = "";

    if (!(std::regex_match(theUID_p, whatALMA, ALMAbasicUIDRegex) ||
	  std::regex_match(theUID_p, whatEVLA, EVLAbasicUIDRegex))
	)
      result  = "Invalid format for EntityId: " + x;
#endif
   
    if (EntityIdVerbose) 
      cout << "EntityId::validate. Validation message is '" << result << "'. (An empty message is a good sign.)" << endl;
    
    return result;

    /*
     * This is the previous version of the code which was using
     * the regex machinery contained in libc. I was not sure
     * of its thread safetiness...

     // Check the entityId for the correct format.
     //	the old one	 char * rexp = "^uid://X[a-fA-F0-9]\\+/X[a-fA-F0-9]\\+\\(/X[a-fA-F0-9]\\+\\)\\?$";
     char * rexp = "^[uU][iI][dD]://[0-9a-zA-Z]+(/[xX][0-9a-fA-F]+){2}(#\\w{1,}){0,}$";
	  
     regex_t preg;
     regcomp(&preg, rexp, REG_NOSUB);
     if ( regexec(&preg, x.c_str(), 0, 0, 0) )
     result =  msg;
	  
     regfree(&preg);
     return string("");
    */		
  }

  EntityId::EntityId(const string &id) {
    string msg = validate(id);
    if (msg.length() != 0)
      throw InvalidArgumentException(msg);
    this->id = id;		
  }

#ifndef WITHOUT_ACS
  EntityId::EntityId(asdmIDLTypes::IDLEntityId &x) {
    string tmp(x.value);
    string msg = validate(tmp);
    if (msg.length() != 0)
      throw InvalidArgumentException(msg);
    this->id = tmp;		
  }
#endif
	
  void EntityId::toBin(EndianOSStream& eoss) const {
    eoss.writeString(id);
  }
	
  EntityId EntityId::fromBin(EndianIStream& eis) {
    return EntityId(eis.readString());		
  }
} // End namespace asdm
