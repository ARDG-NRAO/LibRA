// -*- C++ -*-
//# WOnlyConvFunc.h: Definition of the WOnlyConvFunc class
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
#ifndef SYNTHESIS_WONLYCONVFUNC_H
#define SYNTHESIS_WONLYCONVFUNC_H

#include <synthesis/TransformMachines/ConvolutionFunction.h>
#include <synthesis/TransformMachines/CFStore.h>
#include <synthesis/TransformMachines/WTerm.h>
#include <synthesis/TransformMachines/PSTerm.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/TempImage.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Logging/LogSink.h>
#include <casacore/casa/Logging/LogOrigin.h>
#include <casacore/casa/Arrays/ArrayFwd.h>

namespace casacore{

  template<class T> class ImageInterface;

}

namespace casa { //# NAMESPACE CASA - BEGIN
  class VisBuffer;
  //
  //-------------------------------------------------------------------------------------------
  //
  class WOnlyConvFunc : public ConvolutionFunction
  {
  public:
    WOnlyConvFunc(const casacore::CountedPtr<WTerm>& wTerm,
		  const casacore::CountedPtr<PSTerm>& psTerm):
      ConvolutionFunction(),wTerm_p(wTerm), psTerm_p(psTerm)
    {};
    ~WOnlyConvFunc() {};
    WOnlyConvFunc& operator=(const WOnlyConvFunc& other);


    void makeConvFunction(const casacore::ImageInterface<casacore::Complex>& image,
			  const VisBuffer& vb,
			  const casacore::Int wConvSize,
			  const casacore::Float pa,
			  CFStore& cfs,
			  CFStore& cfwts);

    virtual int getVisParams(const VisBuffer& vb) {return wTerm_p->getVisParams(vb);};
    virtual void setPolMap(const casacore::Vector<casacore::Int>& polMap) {wTerm_p->setPolMap(polMap);};

    virtual casacore::Bool findSupport(casacore::Array<casacore::Complex>& func, casacore::Float& threshold,casacore::Int& origin, casacore::Int& R);

    //
    // Pedgree baggage (NoOps).  
    //
    casacore::Bool makeAverageResponse(const VisBuffer& /*vb*/, 
			     const casacore::ImageInterface<casacore::Complex>& /*image*/,
			     casacore::ImageInterface<casacore::Float>& /*theavgPB*/,
			     casacore::Bool /*reset*/=true) {return false;};
    casacore::Bool makeAverageResponse(const VisBuffer& /*vb*/, 
			     const casacore::ImageInterface<casacore::Complex>& /*image*/,
			     casacore::ImageInterface<casacore::Complex>& /*theavgPB*/,
			     casacore::Bool /*reset*/=true) {return false;};
  protected:
    void setSupport(casacore::Array<casacore::Complex>& convFunc, CFStore& cfs);

  private:
    casacore::CountedPtr<WTerm> wTerm_p;
    casacore::CountedPtr<PSTerm> psTerm_p;
  };
  //
  //-------------------------------------------------------------------------------------------
  //
};
#endif
