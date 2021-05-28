// -*- C++ -*-
//# CFStore.h: Definition of the CFStore class
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
#ifndef SYNTHESIS_TRANSFORM2_CFSTORE_H
#define SYNTHESIS_TRANSFORM2_CFSTORE_H
#include <synthesis/TransformMachines2/CFDefs.h>
#include <synthesis/TransformMachines/SynthesisError.h>
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <casa/Logging/LogIO.h>
#include <casa/Logging/LogSink.h>
#include <casa/Logging/LogOrigin.h>
#include <casa/Utilities/CountedPtr.h>
#include <images/Images/ImageInterface.h>
#include <msvis/MSVis/VisBuffer2.h>
namespace casa { //# NAMESPACE CASA - BEGIN
  using namespace vi;
  namespace refim{
  using namespace CFDefs;
  class CFStore
  {
  public:
    CFStore():data(), rdata(), coordSys(), sampling(), 
	      xSupport(), ySupport(), 
	      maxXSupport(-1), maxYSupport(-1),
	      pa(), mosPointingPos(0) {};

    CFStore(CFType *dataPtr, casacore::CoordinateSystem& cs, casacore::Vector<casacore::Float>& samp,
	    casacore::Vector<casacore::Int>& xsup, casacore::Vector<casacore::Int>& ysup, casacore::Int maxXSup, casacore::Int maxYSup,
	    casacore::Quantity PA, casacore::Int mosPointing):
      data(),rdata(), coordSys(cs), sampling(samp),
      xSupport(xsup), ySupport(ysup), maxXSupport(maxXSup),
      maxYSupport(maxYSup), pa(PA), mosPointingPos(mosPointing)
    {data = new CFType(*dataPtr);};

    ~CFStore() {};

    CFStore& operator=(const CFStore& other);
    void show(const char *Mesg=NULL,std::ostream &os=std::cerr);
    casacore::Bool null() {return data.null();};
    void set(const CFStore& other)
    {
      coordSys = other.coordSys; sampling.assign(other.sampling); 
      xSupport.assign(other.xSupport); ySupport.assign(other.ySupport);
      maxXSupport=other.maxXSupport;  maxYSupport=other.maxYSupport; pa=other.pa;
      mosPointingPos = other.mosPointingPos;
    }
    void set(CFType *dataPtr, casacore::CoordinateSystem& cs, casacore::Vector<casacore::Float>& samp,
	     casacore::Vector<casacore::Int>& xsup, casacore::Vector<casacore::Int>& ysup, casacore::Int maxXSup, casacore::Int maxYSup,
	     casacore::Quantity PA, const casacore::Int mosPointing=0)
    {
      data=dataPtr; coordSys=cs; sampling.assign(samp); 
      xSupport.assign(xsup); ySupport.assign(ysup);
      maxXSupport=maxXSup;maxYSupport=maxYSup;
      pa=PA;
      mosPointingPos = mosPointing;
    }

    void resize(casacore::Int nw, casacore::Bool retainValues=false);
    void resize(casacore::IPosition imShape, casacore::Bool retainValues=false);


    casacore::CountedPtr<CFType> data;
    casacore::CountedPtr<CFTypeReal> rdata;
    casacore::CoordinateSystem coordSys;
    casacore::Vector<casacore::Float> sampling;
    casacore::Vector<casacore::Int> xSupport,ySupport;
    casacore::Int maxXSupport, maxYSupport;
    casacore::Quantity pa;
    casacore::Int mosPointingPos;
  };
} //# NAMESPACE CASA - END
}
#endif
