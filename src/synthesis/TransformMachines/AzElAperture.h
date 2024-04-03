// -*- C++ -*-
//# AzElAperture.h: Definition of the AzElAperture class
//# Copyright (C) 1997,1998,1999,2000,2001,2002,2003
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
//
#ifndef SYNTHESIS_AZELAPERTURE_H
#define SYNTHESIS_AZELAPERTURE_H

#include <casacore/images/Images/ImageInterface.h>
#include <synthesis/TransformMachines/ATerm.h>
#include <synthesis/TransformMachines/Utils.h>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/casa/Arrays/ArrayFwd.h>
//
//---------------------------------------------------------------------
//---------------------------------------------------------------------
// TEMPS The following #defines should REALLLLAY GO!
//---------------------------------------------------------------------
//---------------------------------------------------------------------
//
// #define CONVSIZE (1024*2)
// #define CONVWTSIZEFACTOR sqrt(2.0)
// #define OVERSAMPLING 10
// #define THRESHOLD 1E-3


namespace casacore{

  template<class T> class ImageInterface;

}

namespace casa { //# NAMESPACE CASA - BEGIN
  class VisBuffer;
  class AzElAperture : public ATerm
  {
  public:
    AzElAperture(): ATerm(), cachedCFC_p(), cachedCFCPA_p(400.0){};
    ~AzElAperture() {};
    virtual void rotate(const VisBuffer& vb, CFCell& cfc, const casacore::Double& rotAngleIncrement=5.0);
    virtual void rotate2(const VisBuffer& vb, CFCell& baseCFC, CFCell& cfc, const casacore::Double& rotAngleIncrement=5.0);
  private:
    casacore::Array<TT> cachedCFC_p;
    casacore::Float cachedCFCPA_p;
  };
};
#endif
