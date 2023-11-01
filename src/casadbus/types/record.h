//# record.h: dictionary of variants
//#
//# Copyright (C) 2011
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$

#ifndef __casadbus_record_h__
#define __casadbus_record_h__

#include <map>
#include <string>
#include <casadbus/types/variant.h>

namespace casa {
    namespace dbus {

	typedef std::map<std::string,variant> rec_map;

	// todo:	o create python to/from record functions
	//		o implement compare()
	//		o implement record_to_string()
	//		o create a thorough test program
	//		o verify that std::map<>'s copy ctor does a complete copy
	//		o add copy-on-write (w/ reference counting)
	//
	class record : public  rec_map {
	    public:
		typedef rec_map::iterator iterator;
		typedef rec_map::const_iterator const_iterator;
		typedef rec_map::value_type value_type;

		record();
		record *clone() const { return new record(*this); }
		int compare(const record*) const;

		record( const record &r ) : rec_map(r) { }
		record &operator=(const record &r) { rec_map::operator=(r); return *this; }
		std::pair<rec_map::iterator,bool> insert(const std::string &s,const variant &v);
	};

    }	// dbus namespace
}	// casa namespace

#endif
