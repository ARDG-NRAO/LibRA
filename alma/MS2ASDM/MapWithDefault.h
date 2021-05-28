//# Copyright (C) 2019
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

#ifndef MS2ASDM_MAPWITHHDEFAULT_H
#define MS2ASDM_MAPWITHHDEFAULT_H

#include <map>

#include <casa/aips.h>

namespace casa { //# NAMESPACE CASA - BEGIN

    // <summary>
    // MapWithDefault adds a default value to the std::map class. This default value
    // is used by the [] operator when the key is not already known.
    // <summary>
    
    // <visibility=export>

    // <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
    // </reviewed>

    // <prerequisite>
    //   <li> std::map
    // </prerequisite>
    // <etymology>
    // </etymology>
    //
    // <synopsis>
    // </synopsis>

    template<class K, class V> class MapWithDefault : public std::map<K,V>
    {
    public:
        // creates a std::map with the associated specified defaultt value
        MapWithDefault (const V& defaultValue) : std::map<K,V>(), defaultVal(defaultValue) {;}
        
        // creates a map with default from another one; use copy semantics.
        MapWithDefault (const MapWithDefault<K,V>& other) : std::map<K,V>(other), defaultVal(other.defaultVal) {;}

        // Removes a map with default.
        ~MapWithDefault () {;}

        // Assigns this map with default to another one; copy semantics
        MapWithDefault<K,V>& operator= (const MapWithDefault<K,V>& other);

        // this is the specialization which uses the default value as necessary.
        // If the map from the key to a value is not defined a mapping will
        // be defined from the key to the default value.
        V &operator[](const K &key);

    private:
        V defaultVal;
    };

    template<class K, class V> inline MapWithDefault<K,V> &MapWithDefault<K,V>::operator=(const MapWithDefault<K,V> &other) \
    {
        if (this != other) {
            std::map<K,V>::operator=(*other);
            this->defaultVal = other->defaultVal;
        }
        return this;
    }

    template<class K, class V> inline V &MapWithDefault<K,V>::operator[](const K &key)
    {
        if (this->count(key) == 0) {
            std::map<K,V>::operator[](key) = this->defaultVal;
        }
        return this->at(key);
    }
    
} // # NAMESPACE CASA - END

#endif
