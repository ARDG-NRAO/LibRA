//# ViiLayerFactory.h: Recursive factory definition for layered ViImplementation2s
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003,2016
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

#ifndef MSVIS_VIILAYERFACTORY2_H
#define MSVIS_VIILAYERFACTORY2_H

#include <casacore/casa/aips.h>
#include <casacore/casa/Arrays/Vector.h>

namespace casa { //# NAMESPACE CASA - BEGIN

namespace vi {

class ViImplementation2;



//////////////////////////////////////////////////////////////////////
//
// Class ViiLayerFactory
//

class ViiLayerFactory {

public:

  virtual ~ViiLayerFactory () {}
  
  virtual ViImplementation2 * createViImpl2 (casacore::Vector<ViiLayerFactory*> viifactories) const;

protected:

    // Make a ViImplementation with another as input
    //   Must be specialized for each type
    virtual ViImplementation2 * createInstance (ViImplementation2*) const = 0;

};

} // end namespace vi

} //# NAMESPACE CASA - END

#endif
