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

#include <mstransform/TVI/StatWtTVILayerFactory.h>

using namespace casacore;

namespace casa {

namespace vi {

StatWtTVILayerFactory::StatWtTVILayerFactory(Record& configuration)
    : ViiLayerFactory(), _configuration(configuration) {}

ViImplementation2* StatWtTVILayerFactory::createInstance(ViImplementation2* vii0) const {
    _statWtTVI = new StatWtTVI(vii0, _configuration);
    ViImplementation2 *vii = _statWtTVI;
    return vii; 
}

}

}

