//# IlluminationConvFunc.h: Definition for IlluminationConvFunc
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

#ifndef SYNTHESIS_ILLUMINATIONCONVFUNC_H
#define SYNTHESIS_ILLUMINATIONCONVFUNC_H

#include <casacore/casa/Exceptions.h>
#include <synthesis/MeasurementComponents/TabulatedConvFunc.h>
#include <synthesis/MeasurementComponents/ExpCache.h>
#include <synthesis/MeasurementComponents/CExp.new3.h>
#include <synthesis/TransformMachines/PolOuterProduct.h>

namespace casa{

  class IlluminationConvFunc: public TabulatedConvFunc
  {
  public:
    IlluminationConvFunc(): TabulatedConvFunc() 
    {expTableReady=cexpTableReady=sigmaReady=false;pa_p=1000.0;};
    IlluminationConvFunc(casacore::Int n): TabulatedConvFunc(n)
    {expTableReady=cexpTableReady=sigmaReady=false;pa_p=1000.0;};
    ~IlluminationConvFunc() {};

    IlluminationConvFunc& operator=(const IlluminationConvFunc& /*other*/)
    {
      return *this;
    }
    IlluminationConvFunc& operator=(const ConvolutionFunction& /*other*/)
    {
      return *this;
    }
    void initExpTable(casacore::Int n, CF_TYPE step)  {ExpTable.build(n,step);expTableReady=true;};
    void initCExpTable(casacore::Int n)               {CExpTable.build(n);cexpTableReady=true;};
    void setSigma(casacore::Double s)                 {sigma = s;sigmaReady=true;}

    casacore::Bool isReady() {return (expTableReady & cexpTableReady & sigmaReady);}

    CF_TYPE area(casacore::Vector<casacore::Int>& convSupport, casacore::Vector<casacore::Double>& uvScale);
    casacore::Vector<casacore::Int> supportSize(casacore::Vector<casacore::Double>& uvScale);

    CF_TYPE getValue(casacore::Vector<CF_TYPE>& coord, casacore::Vector<CF_TYPE>& offset) 
    {
      (void)coord;(void)offset;
      throw(casacore::AipsError("This version of IlluminationConvFunc::getValue() is not useful."));
      return 0;
    };
    virtual CF_TYPE getValue(casacore::Double *coord,
			     casacore::Double *raoff1,  casacore::Double *raoff2, 
			     casacore::Double *decoff1, casacore::Double *decoff2,
			     casacore::Double *area,
			     casacore::Int *doGrad,
			     casacore::Complex& weight,
			     casacore::Complex& dweight1,
			     casacore::Complex& dweight2,
			     casacore::Double& currentCFPA
			     //		  ,casacore::Double lsigma
			     );
    int getVisParams(const casa::VisBuffer& vb, const casacore::CoordinateSystem& skyCoord=casacore::CoordinateSystem())
    {(void)vb;(void)skyCoord;return 0;};
    void makeConvFunction(const casacore::ImageInterface<std::complex<float> >&,
			  const casa::VisBuffer&, casacore::Int,
			  const casacore::CountedPtr<PolOuterProduct>& ,//pop,
			  casacore::Float, casacore::Float,
			  const casacore::Vector<casacore::Double>& /*uvScale*/, const casacore::Vector<casacore::Double>& /*uvOffset*/,
			  const casacore::Matrix<casacore::Double>& /*freqSelection*/,
			  casa::CFStore2&, casa::CFStore2&, casacore::Bool /*fillCF=true*/ ) {};
    void setPolMap(const casacore::Vector<int>&polMap) {(void)polMap;};
    void setFeedStokes(const casacore::Vector<int>&feedStokes) {(void)feedStokes;};
    void setParams(const casacore::Vector<int>& polMap, const casacore::Vector<int>& feedStokes)
    {(void)polMap;(void)feedStokes;};

    casacore::Bool findSupport(casacore::Array<casacore::Complex>&, casacore::Float& ,casacore::Int&, casacore::Int&) 
    {throw(casacore::AipsError("IlluminationConvFunc::findSupport() not implemented"));};
    virtual casacore::Vector<casacore::Double> findPointingOffset(const casacore::ImageInterface<casacore::Complex>& /*image*/,
					      const VisBuffer& /*vb*/)
    {throw(casacore::AipsError("IlluminationConvFunc::findPointingOffset() not implemented"));};
    virtual void prepareConvFunction(const VisBuffer& /*vb*/, VBRow2CFBMapType& /*theMap*/)
    {throw(casacore::AipsError("IlluminationConvFunc::prepareConvFunction() not implemented"));};

    virtual casacore::Bool makeAverageResponse(const VisBuffer&,
				     const casacore::ImageInterface<casacore::Complex>&,
				     casacore::ImageInterface<casacore::Float>&,
				     casacore::Bool)
      {throw(casacore::AipsError("IlluminationConvFunc::makeAverageRes() called"));};

    virtual casacore::Bool makeAverageResponse(const VisBuffer&,
				     const casacore::ImageInterface<casacore::Complex>&,
				     casacore::ImageInterface<casacore::Complex>&,
				     casacore::Bool)
      {throw(casacore::AipsError("IlluminationConvFunc::makeAverageRes() called"));};

		  
  private:
    ExpCache<CF_TYPE> ExpTable;
    CExp3<CF_TYPE> CExpTable;
    casacore::Double sigma, pa_p;
    casacore::Bool expTableReady,cexpTableReady,sigmaReady;
  };

};
#endif
