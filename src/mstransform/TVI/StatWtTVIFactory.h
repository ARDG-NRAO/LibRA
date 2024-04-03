//#  CASA - Common Astronomy Software Applications (http://casa.nrao.edu/)
//#  Copyright (C) Associated Universities, Inc. Washington DC, USA 2011, All rights reserved.
//#  Copyright (C) European Southern Observatory, 2011, All rights reserved.
//#
//#  This library is free software; you can redistribute it and/or
//#  modify it under the terms of the GNU Lesser General Public
//#  License as published by the Free software Foundation; either
//#  version 2.1 of the License, or (at your option) any later version.
//#
//#  This library is distributed in the hope that it will be useful,
//#  but WITHOUT ANY WARRANTY, without even the implied warranty of
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//#  Lesser General Public License for more details.
//#
//#  You should have received a copy of the GNU Lesser General Public
//#  License along with this library; if not, write to the Free Software
//#  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
//#  MA 02111-1307  USA
//# $Id: $

#ifndef STATWTTVIFACTORY_H_
#define STATWTTVIFACTORY_H_

#include <msvis/MSVis/VisibilityIterator2.h>

#include <casacore/casa/Containers/Record.h>

namespace casa { 

namespace vi { 

class StatWtTVIFactory : public ViFactory {

public:

    StatWtTVIFactory() = delete;

    StatWtTVIFactory(
        casacore::Record &configuration, ViImplementation2 *inputVII
    );

    StatWtTVIFactory(const StatWtTVIFactory&) = delete;

    StatWtTVIFactory operator=(const StatWtTVIFactory&) = delete;

protected:

    casacore::Record _configuration;
    ViImplementation2* _inputVii;

    vi::ViImplementation2 * createVi(VisibilityIterator2 *) const;
    vi::ViImplementation2 * createVi() const;
};

} 

}

#endif 

