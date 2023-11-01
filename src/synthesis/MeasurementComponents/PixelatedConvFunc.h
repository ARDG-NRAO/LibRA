//# PixelatedConvFunc.h: Definition for PixelatedConvFunc
//# Copyright (C) 1996,1997,1998,1999,2000,2002
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
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$

#ifndef SYNTHESIS_PIXELATEDCONVFUNC_H
#define SYNTHESIS_PIXELATEDCONVFUNC_H

#include <casacore/casa/Arrays/Vector.h>
//#include <synthesis/MeasurementComponents/Exp.h>
#include <synthesis/MeasurementComponents/CExp.h>
#include <synthesis/TransformMachines/ConvolutionFunction.h>

namespace casa{

  template <class T> class PixelatedConvFunc: public ConvolutionFunction
  {
  public:
    PixelatedConvFunc():ConvolutionFunction() {};
    PixelatedConvFunc(casacore::IPosition& shape):ConvolutionFunction(shape.nelements()) 
    {setSize(shape);}
    ~PixelatedConvFunc() {};
    
    virtual void setSize(casacore::IPosition& size)
    {
      nDim=size.nelements(); 
      shape=size;
      cache.resize(shape);
    };

    virtual T getValue(casacore::Vector<T>& , casacore::Vector<T>& ) { return (T)(1.0);};
    virtual int getVisParams(const VisBuffer&,const casacore::CoordinateSystem& ) {return 0;};
    virtual void makeConvFunction(const casacore::ImageInterface<casacore::Complex>&,
				  const VisBuffer&,
				  const casacore::Int,
				  const casacore::Float,
				  const casacore::Float,
				  CFStore&,
				  CFStore&,casacore::Bool ) {};
    virtual void setPolMap(const casacore::Vector<casacore::Int>& polMap) {(void)polMap;};
    virtual void setFeedStokes(const casacore::Vector<casacore::Int>& feedStokes){(void)feedStokes;};
    PixelatedConvFunc& operator=(const PixelatedConvFunc& )
    {
      cerr << "******* PixelatedConvFunc& operator=(PixelatedConvFunc&) called!" << endl;
      return *this;
    }
    PixelatedConvFunc& operator=(const ConvolutionFunction& )
    {
      cerr << "******* PixelatedConvFunc& operator=(ConvolutionFunction&) called!" << endl;
      return *this;
    }
    virtual casacore::Bool makeAverageResponse(const VisBuffer&, 
				     const casacore::ImageInterface<casacore::Complex>&,
				     casacore::ImageInterface<casacore::Complex>&,
				     casacore::Bool)
    {throw(casacore::AipsError("PixelatedConvFunc::makeAverageRes(Complex) called"));};
    virtual void prepareConvFunction(const VisBuffer&, CFStore&) {};

  private:
    casacore::Int nDim;
    casacore::IPosition shape;
    casacore::Array<T> cache;
  };

};

#endif
