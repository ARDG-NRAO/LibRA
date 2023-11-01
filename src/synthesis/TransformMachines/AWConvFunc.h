// -*- C++ -*-
//# AWConvFunc.h: Definition of the AWConvFunc class
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
#ifndef SYNTHESIS_AWCONVFUNC_H
#define SYNTHESIS_AWCONVFUNC_H

#include <synthesis/TransformMachines/ConvolutionFunction.h>
#include <synthesis/TransformMachines/PolOuterProduct.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <synthesis/TransformMachines/CFStore.h>
#include <synthesis/TransformMachines/CFStore2.h>
#include <synthesis/TransformMachines/CFBuffer.h>
#include <synthesis/TransformMachines/PSTerm.h>
#include <synthesis/TransformMachines/WTerm.h>
#include <synthesis/TransformMachines/ATerm.h>
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
  class AWConvFunc : public ConvolutionFunction
  {
  public:
    AWConvFunc(const casacore::CountedPtr<ATerm> ATerm,
	       const casacore::CountedPtr<PSTerm> psTerm,
	       const casacore::CountedPtr<WTerm> wTerm,
	       const casacore::Bool wbAWP=false,
	       const casacore::Bool conjPB=casacore::True);

    ~AWConvFunc() {};
    AWConvFunc& operator=(const AWConvFunc& other);
    virtual void makeConvFunction(const casacore::ImageInterface<casacore::Complex>& image,
				  const VisBuffer& vb,
				  const casacore::Int wConvSize,
				  const casacore::CountedPtr<PolOuterProduct>& pop,
				  const casacore::Float pa,
				  const casacore::Float dpa,
				  const casacore::Vector<casacore::Double>& uvScale,
				  const casacore::Vector<casacore::Double>& uvOffset,
				  const casacore::Matrix<casacore::Double>& vbFreqSelection,
				  CFStore2& cfs,
				  CFStore2& cfwts,
				  casacore::Bool fillCF=true);
    virtual void fillConvFuncBuffer(CFBuffer& cfb, CFBuffer& cfWtb,
				    const casacore::Int& nx, const casacore::Int& ny,
				    const casacore::Vector<casacore::Double>& freqValues,
				    const casacore::Vector<casacore::Double>& wValues,
				    const casacore::Double& wScale,
				    const casacore::Double& vbPA, const casacore::Double& freqHi,
				    const PolMapType& muellerElements,
				    const PolMapType& muellerElementsIndex,
				    const VisBuffer& vb, const casacore::Float& psScale,
				    PSTerm& psTerm, WTerm& wTerm, ATerm& aTerm, 
				    casacore::Bool isDryRun=false);
    static void makeConvFunction2(const casacore::String& uvGridDiskimage,
				  const casacore::Vector<casacore::Double>& uvScale,
				  const casacore::Vector<casacore::Double>& uvOffset,
				  const casacore::Matrix<casacore::Double>& vbFreqSelection,
				  CFStore2& cfs,
				  CFStore2& cfwts,
				  const casacore::Bool psTermOn,
				  const casacore::Bool aTermOn,
				  const casacore::Bool conjBeams);
    static void fillConvFuncBuffer2(CFBuffer& cfb, CFBuffer& cfWtb,
				    const casacore::Int& nx, const casacore::Int& ny,
				    const casacore::ImageInterface<casacore::Complex>& skyImage,
				    //const CoordinateSystem& skyCoords,
				    const CFCStruct& miscInfo,
				    PSTerm& psTerm, WTerm& wTerm, ATerm& aTerm,
				    casacore::Bool conjPB);

    virtual casacore::Bool makeAverageResponse(const VisBuffer& vb, 
				     const casacore::ImageInterface<casacore::Complex>& image,
				     casacore::ImageInterface<casacore::Float>& theavgPB,
				     casacore::Bool reset=true);
    virtual casacore::Bool makeAverageResponse(const VisBuffer& vb, 
				     const casacore::ImageInterface<casacore::Complex>& image,
				     casacore::ImageInterface<casacore::Complex>& theavgPB,
				     casacore::Bool reset=true);

    virtual int getVisParams(const VisBuffer& vb,const casacore::CoordinateSystem& skyCoord=casacore::CoordinateSystem())
    {return aTerm_p->getVisParams(vb,skyCoord);};

    virtual void setPolMap(const casacore::Vector<casacore::Int>& polMap) {aTerm_p->setPolMap(polMap);};
    //    virtual void setFeedStokes(const casacore::Vector<casacore::Int>& feedStokes) {aTerm_p->setFeedStokes(feedStokes);};

    virtual casacore::Bool findSupport(casacore::Array<casacore::Complex>& func, casacore::Float& threshold,casacore::Int& origin, casacore::Int& R);

    virtual casacore::Vector<casacore::Double> findPointingOffset(const casacore::ImageInterface<casacore::Complex>& /*image*/,
								  const VisBuffer& /*vb*/)
    {casacore::Vector<casacore::Double> tt(2); tt=0;return tt;};

    virtual void prepareConvFunction(const VisBuffer& vb, VBRow2CFBMapType& cfs);
    casacore::Int mapAntIDToAntType(const casacore::Int& ant) {return aTerm_p->mapAntIDToAntType(ant);};

    virtual casacore::Vector<casacore::Double> makeFreqValList(casacore::Double& freqScale,const VisBuffer& vb, 
							       const casacore::ImageInterface<casacore::Complex>& uvGrid,
							       casacore::Vector<String>& bandNames);
    virtual casacore::Vector<casacore::Double> makeWValList(const casacore::Double &dW, const casacore::Int &nW);

    virtual void setMiscInfo(const casacore::RecordInterface& params);

    virtual casacore::Matrix<casacore::Double> getFreqRangePerSpw(const VisBuffer& vb);



    //
    // Global methods (services)
    //
    static void makeConjPolAxis(casacore::CoordinateSystem& cs, casacore::Int conjStokes_in=-1);
    static casacore::Complex cfArea(casacore::Matrix<casacore::Complex>& cf, const casacore::Int& xSupport,
				    const casacore::Int& ySupport, const casacore::Float& sampling);
    static casacore::Bool awFindSupport(casacore::Array<casacore::Complex>& func, casacore::Float& threshold,
					casacore::Int& origin, casacore::Int& radius);
    static casacore::Bool setUpCFSupport(casacore::Array<casacore::Complex>& func, casacore::Int& xSupport,
					 casacore::Int& ySupport,const casacore::Float& sampling, const casacore::Complex& peak);
    static casacore::Bool resizeCF(casacore::Array<casacore::Complex>& func,  casacore::Int& xSupport,
				   casacore::Int& ySupport, const casacore::Int& supportBuffer, const casacore::Float& sampling,
				   const casacore::Complex& peak);
    static int getOversampling(PSTerm& psTerm, WTerm& wTerm, ATerm& aTerm);
    virtual casacore::CountedPtr<CFTerms> getTerm(const casacore::String& name)
    {if (name=="ATerm") return aTerm_p; else return NULL;}

    
    casacore::CountedPtr<ATerm> aTerm_p;
    casacore::CountedPtr<PSTerm> psTerm_p;
    casacore::CountedPtr<WTerm> wTerm_p;

  protected:
    void normalizeAvgPB(casacore::ImageInterface<casacore::Complex>& inImage,
			casacore::ImageInterface<casacore::Float>& outImage);
    casacore::Bool makeAverageResponse_org(const VisBuffer& vb, 
					   const casacore::ImageInterface<casacore::Complex>& image,
					   casacore::ImageInterface<casacore::Float>& theavgPB,
					   casacore::Bool reset=true);
    void makePBSq(casacore::ImageInterface<casacore::Complex>& inImage);


    casacore::Vector<casacore::Double> thePix_p, pixFieldGrad_p;
    casacore::Double imRefFreq_p;
    casacore::Bool wbAWP_p, conjPB_p;
    casacore::CountedPtr<CFBuffer> baseCFB_p;
  };
  //
  //-------------------------------------------------------------------------------------------
  //
};
#endif
