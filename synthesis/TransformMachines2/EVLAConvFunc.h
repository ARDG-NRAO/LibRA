// -*- C++ -*-
//# EVLAConvFunc.h: Definition of the EVLAConvFunc class
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
#ifndef SYNTHESIS_TRANSFORM2_EVLACONVFUNC_H
#define SYNTHESIS_TRANSFORM2_EVLACONVFUNC_H

#include <casacore/images/Images/ImageInterface.h>
#include <synthesis/TransformMachines2/Utils.h>
#include <synthesis/TransformMachines/BeamCalc.h>
#include <synthesis/TransformMachines2/CFStore.h>
#include <synthesis/TransformMachines2/VLACalcIlluminationConvFunc.h>
//#include <synthesis/MeasurementComponents/IlluminationConvFunc.h>
//#include <synthesis/MeasurementComponents/PixelatedConvFunc.h>
#include <synthesis/TransformMachines2/ConvolutionFunction.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>
#include <casacore/coordinates/Coordinates/StokesCoordinate.h>
#include <casacore/lattices/LatticeMath/LatticeFFT.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Logging/LogSink.h>
#include <casacore/casa/Logging/LogOrigin.h>
#include <casacore/casa/Arrays/ArrayFwd.h>

namespace casacore{

  template<class T> class ImageInterface;

}

namespace casa { //# NAMESPACE CASA - BEGIN
  //class VisBuffer2;
  namespace refim{
  class EVLAConvFunc : public ConvolutionFunction
  {
  public:
    //    EVLAConvFunc(const casacore::CountedPtr<IlluminationConvFunc> ATerm):
    //      ConvolutionFunction(),bandID_p(-1), polMap_p(), feedStokes_p(), ATerm_p(ATerm)
    EVLAConvFunc():     
      ConvolutionFunction(),bandID_p(-1), polMap_p(), feedStokes_p()
    {};
    ~EVLAConvFunc() {};
    EVLAConvFunc& operator=(const EVLAConvFunc& other);
    casacore::Int getVLABandID(casacore::Double& freq,casacore::String&telescopeName);
    casacore::Bool findSupport(casacore::Array<casacore::Complex>& func, casacore::Float& threshold,casacore::Int& origin, casacore::Int& R);
    void makeConvFunction(const casacore::ImageInterface<casacore::Complex>& image,
			  const VisBuffer2& vb,
			  const casacore::Int wConvSize,
			  const casacore::Float pa,
			  const casacore::Float dpa,
			  CFStore& cfs,
			  CFStore& cfwts, casacore::Bool fillCF=true);
    int getVisParams(const VisBuffer2& vb);
    casacore::Int makePBPolnCoords(const VisBuffer2 &vb,
			 const casacore::Vector<casacore::Int>& polMap,
			 const casacore::Int& convSize,
			 const casacore::Int& convSampling,
			 const casacore::CoordinateSystem& skyCoord,
			 const casacore::Int& skyNx, const casacore::Int& skyNy,
			 casacore::CoordinateSystem& feedCoord,
			 casacore::Vector<casacore::Int>& cfStokes);
    //
    // Overloading these functions from ConvolutionFunction class
    //
    void setPolMap(const casacore::Vector<casacore::Int>& polMap);
    void setFeedStokes(const casacore::Vector<casacore::Int>& feedStokes);
  private:
    casacore::Int bandID_p;
    casacore::Float Diameter_p, Nant_p, HPBW, sigma;
    
    casacore::LogIO& logIO() {return logIO_p;}
    casacore::LogIO logIO_p;
    casacore::Vector<casacore::Int> polMap_p;
    casacore::Vector<casacore::Int> feedStokes_p;
    casacore::CountedPtr<IlluminationConvFunc> ATerm_p;
  };
};
};
#endif
