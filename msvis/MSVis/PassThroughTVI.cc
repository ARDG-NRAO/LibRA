/* -*- mode: c++ -*- */
//# PassThroughTVI
//# Copyright (C) 2017
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

#include <msvis/MSVis/PassThroughTVI.h>
#include <msvis/MSVis/VisBuffer2.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

namespace vi { //# NAMESPACE VI - BEGIN

PassThroughTVI::PassThroughTVI(ViImplementation2 * inputVii)
 :  TransformingVi2 (inputVii)
{
    setVisBuffer(createAttachedVisBuffer (VbRekeyable));
}

void PassThroughTVI::origin()
{
    // Drive underlying ViImplementation2
    getVii()->origin();

    // Synchronize own VisBuffer
    configureNewSubchunk();

    return;
}

void PassThroughTVI::next()
{
    // Drive underlying ViImplementation2
    getVii()->next();

    // Synchronize own VisBuffer
    configureNewSubchunk();

    return;
}

PassThroughTVIFactory::PassThroughTVIFactory(ViImplementation2 *inputVII)
 : inputVii_p(inputVII)
{
}

PassThroughTVIFactory::~PassThroughTVIFactory ()
{
}

ViImplementation2 * PassThroughTVIFactory::createVi () const {

    ViImplementation2* vii = new PassThroughTVI(inputVii_p);
    return vii;
}

PassThroughTVILayerFactory::PassThroughTVILayerFactory()
{
}

PassThroughTVILayerFactory::~PassThroughTVILayerFactory()
{
}

ViImplementation2 * 
PassThroughTVILayerFactory::createInstance(ViImplementation2* vii0) const
{
    // Make the PassThroughTVI, using supplied ViImplementation2, and return it
    ViImplementation2 *vii = new PassThroughTVI(vii0);
    return vii;
}

} //# NAMESPACE VI - END

} //# NAMESPACE CASA - END
