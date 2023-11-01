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

#ifndef PASSTHROUGHTVI_H_
#define PASSTHROUGHTVI_H_

#include <msvis/MSVis/TransformingVi2.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <msvis/MSVis/ViiLayerFactory.h>

namespace casa { //# NAMESPACE CASA - BEGIN

namespace vi { //# NAMESPACE VI - BEGIN

/* 
 * This class is the simplest TVI possible that passes all the requests to the
 * underlying VI (delegating to TransformingVi2). It can be used for
 * testing purposes.
 */
class PassThroughTVI : public TransformingVi2
{
public:
  
    //Constructor
    PassThroughTVI(ViImplementation2 * inputVii);

    void origin() override;

    void next() override;

};

/*
 * Factory that allows the creation of PassThroughTVI classes.
 * This factory doesn't have any parameter to configure
 */
class PassThroughTVIFactory : public ViFactory 
{

public:

    PassThroughTVIFactory(ViImplementation2 *inputVII);

    ~PassThroughTVIFactory ();

protected:

    virtual ViImplementation2 * createVi () const;

private:

    ViImplementation2 *inputVii_p;;
};

/*
 * Factory that allows the creation of PassThroughTVI classes
 * which act upon an underlying VI2
 * This factory doesn't have any parameter to configure
 */
class PassThroughTVILayerFactory : public ViiLayerFactory
{

public:

    PassThroughTVILayerFactory();

    virtual ~PassThroughTVILayerFactory();

protected:

    virtual ViImplementation2 * createInstance(ViImplementation2* vii0) const;
};


} // end namespace vi

} // end namespace casa

#endif /* PASSTHROUGHTVI_H_ */
