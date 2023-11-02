#ifndef __casac_record_h__
#define __casac_record_h__

#include <map>
#include <string>
#include <stdcasa/variant.h>

namespace casac {

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
record initialize_record( const std::string & );
}	// casac namespace

#endif
