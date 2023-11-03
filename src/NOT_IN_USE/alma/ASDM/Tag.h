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
 * File Tag.h
 */

#ifndef Tag_CLASS
#define Tag_CLASS

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#ifndef WITHOUT_ACS
#include <asdmIDLTypesC.h>
#endif

#ifndef WITHOUT_BOOST
#include <boost/regex.hpp>
#else
#include <regex>
#endif

#include <alma/ASDM/StringTokenizer.h>
#include <alma/ASDM/InvalidArgumentException.h>
#include <alma/ASDM/TagFormatException.h>
#include <alma/ASDM/TagType.h>
#include <alma/ASDM/IntegerWrapper.h>

#include <alma/ASDM/EndianStream.h>

namespace asdm {

class Tag;
/**
 * The Tag class is an implementation of a unique index identifying 
 * a row of an ASDM table.
 * 
 * Basically a Tag is defined by  :
 * 
 * <ul>
 * <li>a <i>value</i> which is an integer</li>
 * <li>a <i>type</i> which is a pointer to a TagType.</li>
 *  </ul>
 * 
 * @version 1.00 Jan. 7, 2005
 * @author Allen Farris
 * @version 2.00 Aug. 3, 2006
 * @author Michel Caillat
 */
class Tag {
 
public:	
	/**
	 *  Create a default Tag that is null
	 */
	Tag();
	
	/**
	 * Create a Tag from an integer value.
	 * @param i the integer defining the Tag value.
	 * 
	 * @note The created Tag has a TagType::NoType.
	 */
	Tag(unsigned int i);
	
	/**
	 * Create a Tag from an integer value and a TagType
	 * @param i an integer value to define the Tag value
	 * @param t a TagType to define the type of the Tag
	 * 
	 * @note called with t == null this method creates a null Tag.
	 */
	Tag(unsigned int i, const TagType* t) ;
		 
	/**
	 *  Create a Tag from a Tag (copy constructor).
	 */
	Tag(const Tag &t);
	
	
	//Tag(const string &id);
#ifndef WITHOUT_ACS
	/**
	 *  Create a Tag from an IDLTag object.
	 * 
	 * @param t The IDLTag object.
	 * @throws TagFormatException
	 */
	Tag(asdmIDLTypes::IDLTag &t);
#endif

	/**
	 * Overloading of << to output the value of a Tag on an ostream.
	 * @param os a reference to the ostream to be written on.
	 * @param x a const reference to a Tag.
	 */
	friend std::ostream & operator << (std::ostream& os, const Tag & x);

	/**
	 * Overloading of >> to read a Tag from an istream.
	 */
	friend std::istream & operator >> (std::istream & is, Tag & x);
	
	/**
	 * Create a Tag whose initial value is the specified string.
	 * 
	 * @throws TagFormatException
	 */
	static Tag parseTag (std::string s);
	
	/**
	 * Return the Tag as a String.
	 * The resulting string consists in the string representation of the type followed by an underscore
	 *  followed by the String representation of the value. Examples : "Antenna_12", "SpectralWindow_0".
	 * @return The Tag as a String.
	 */
    std::string toString() const;
	 
   /**
	 * Write the binary representation of this to a EndianOSStream. 
	 */
	void  toBin(EndianOSStream& eoss); 
	
   /**
	 * Write the binary representation of a 1D array of Tag to an EndianOSStream.
	 * @param tag  the vector of Tag to be written
	 * @param eoss the EndianOSStream to be written to
	 */
	static void toBin(const std::vector<Tag>& tag, EndianOSStream& eoss) ;	 
	
   /**
	 * Read the binary representation of a Tag  from a EndianIStream.
	 * and use the read value to set a Tag.
	 * @param eis the the EndianIStream to be read
	 * @return a Tag
	 * @throws TagFormatException 
	 */
	static Tag fromBin(EndianIStream& eis);
	
   /**
	 * Read the binary representation of a of  a vector of  Tag from an EndianIStream
	 * and use the read value to set a vector of  Tag.
 	 * @param eiis the EndianIStream to be read
	 * @return a vector of Tag
	 * @throws TagFormatException 
	 */
	static std::vector<Tag> from1DBin(EndianIStream & eis);
	
	/**
	 * The Tag destructor.
	 */
	virtual ~Tag();
	
   /**
	 * Returns the type of this Tag. 
	 * 
	 * @return the type of this Tag as a TagType.
	 * 
	 *  @note A null returned value indicates that the Tag is null (i.e. this.isNull() == true)/
	 */
	TagType* getTagType(); 
	
   /**
	 * Returns the value of this Tag.
	 * The returned value has no meaning if the Tag is null.
	 * @return the value of this Tag as an int.
	 * 
	 * @note The returned integer value has a meaning if and only if the Taf is not null.
	 */
	unsigned int getTagValue() const;

#ifndef WITHOUT_ACS
	/**
	 * Returns a IDLTag object built from this Tag.
	 * 
	 * @return an IDLTag.
	 */
    asdmIDLTypes::IDLTag toIDLTag() const;
#endif

	/**
	 * Return this Tag value as a string.
	 * @return This Tag.
	 * @deprecated
	 */
    std::string getTag() const;

	/**
	 * Assignment operator.
	 */
	Tag& operator = (const Tag &);

	/**
	 * @return true if and only if this->value==t.value and this-> type==t.type 
	 */
	bool equals(const Tag & t) const;
	
	/**
	 * 	 @return true if and only if this->value==t.value and this-> type==t.type
	 * 
	 *  \note
	 * Behaves exactly like the equals method.
	 */ 
	bool operator == (const Tag& t) const;
	
	/**
	 * @return true if this->value != t.value or this->type != t.value
	 */
	bool operator != (const Tag& t) const;
	
	/**
	 * @return true if this->value < t.value
	 * 
	 * \note Note that the types of the Tags being compared is not taken into account.
	 */
	bool operator < (const Tag& t) const;

	/**
	 * @return true if this->value > t.value
	 * 
	 * \note Note that the types of the Tags being compared is not taken into account.
	 */
	bool operator > (const Tag& t) const;


	/**
	 * Return true if and only if this Tag is null, i.e. if its TagType field is null.
	 * @return true if and only if this Tag is null.
	 */
	bool isNull() const;

protected:
    std::string tag;
	TagType* type;

 private:
#ifndef WITHOUT_BOOST
	static boost::regex tagSyntax;
#else
	static std::regex tagSyntax;
#endif
};

// End namespace asdm
}
#endif /* Tag_CLASS */
