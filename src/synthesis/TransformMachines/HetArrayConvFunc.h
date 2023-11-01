//# HetArrayConvFunc.h: Definition for HetArrayConvFunc
//# Copyright (C) 2008
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
#ifndef SYNTHESIS_HETARRAYCONVFUNC_H
#define SYNTHESIS_HETARRAYCONVFUNC_H

#include <synthesis/TransformMachines/SimplePBConvFunc.h>
#include <casacore/casa/Arrays/ArrayFwd.h>

namespace casacore{

  template<class T> class ImageInterface;

}

namespace casa{

  // <summary>  A class to support FTMachines get their convolution casacore::Function </summary>
  
  // <use visibility=export>
  // <prerequisite>
  //   <li> <linkto class=VisBuffer>VisBuffer</linkto> module
  // </prerequisite>
  // <etymology>
  // "HetArray" for Heterogeneous casacore::Array => different dish sizes
  // "ConvFunc" for Convolution Functions 
  //  appropriate convfunctions for each pair of antenna generated and cached
  // </etymology>
  //
  // <synopsis> 
  // FTMachines like WProjection and MosaicFT need convolution functions to 
  // deal with directional dependent issues...
  // this class and related ones provide and cache  such functions for re-use 
  //</synopsis>
  //Forward declarations
  class VisBuffer;
  class MosaicFT;
  class HetArrayConvFunc : public SimplePBConvFunc

  {
  public:
    HetArrayConvFunc();
    HetArrayConvFunc(const PBMathInterface::PBClass 
		     typeToUse, const casacore::String vpTable="");
    //Constructor from record
    //if for prediction only no need to recover fluxscale
    HetArrayConvFunc(const casacore::RecordInterface& rec, casacore::Bool calcFluxscale);
    virtual ~HetArrayConvFunc();

    //Returns the convfunctions in the Arrays...the rowMap maps the vb.row 
    //to the  plane of the convfunc appropriate...chanMap and polMap similarly 

    virtual void findConvFunction(const casacore::ImageInterface<casacore::Complex>& iimage, 
				    const VisBuffer& vb,
				    const casacore::Int& convSampling,
				  const casacore::Vector<casacore::Double>& visFreq,
				    casacore::Array<casacore::Complex>& convFunc,
				    casacore::Array<casacore::Complex>& weightConvFunc,
				    casacore::Vector<casacore::Int>& convsize,
				    casacore::Vector<casacore::Int>& convSupport,
				  casacore::Vector<casacore::Int>& polMap, casacore::Vector<casacore::Int>& chanMap, casacore::Vector<casacore::Int>& rowMap,
				 const casacore::Bool conjugateFreqFuncs=false);

    virtual casacore::ImageInterface<casacore::Float>&  getFluxScaleImage();
    // slice flux scale images 
    virtual void sliceFluxScale(const casacore::Int npol);
    //Serialization
   virtual casacore::Bool toRecord(casacore::RecordInterface& rec);
   virtual casacore::Bool fromRecord(casacore::String& err, const casacore::RecordInterface& rec, casacore::Bool calcFluxscale=false);
   virtual void reset();
   virtual casacore::String name() {return casacore::String("HetArrayConvFunc");}
    //----------------------------------------------

    private:
   void applyGradientToYLine(const casacore::Int iy, casacore::Complex*& convFunctions, 
			     casacore::Complex*& convWeights, const casacore::Double pixXdir, const casacore::Double pixYdir, 
			     casacore::Int convSize, const casacore::Int ndishpair, const casacore::Int nchan, const casacore::Int nPol);
      casacore::Int factorial(casacore::Int n);
      // the return value are -1 or false for not in cache yet but pointing direction 
      //seems to be inside image
      // 1 if value is cached..we have stopped caching..so it should not return this value
      // 2 pointing is off image ...thus valid but not useful
      casacore::Int checkPBOfField(const VisBuffer& vb, casacore::Vector<casacore::Int>& rowMap);
      void findAntennaSizes(const VisBuffer& vb);
      void supportAndNormalize(casacore::Int plane, casacore::Int convSampling);
      void supportAndNormalizeLatt(casacore::Int plane, casacore::Int convSampling, casacore::TempLattice<casacore::Complex>& convFuncLat,
				   casacore::TempLattice<casacore::Complex>& weightConvFuncLat);
      void init(const PBMathInterface::PBClass typeToUse);
      void makerowmap(const VisBuffer& vb, casacore::Vector<casacore::Int>& rowMap);
      casacore::Float interpLanczos( const casacore::Double& x , const casacore::Double& y, const casacore::Double& nx, const casacore::Double& ny,   const casacore::Float* data, const casacore::Float a=3);
      casacore::Float sinc(const casacore::Float x) ;
      casacore::Array<casacore::Complex> resample(const casacore::Array<casacore::Complex>& inarray, const casacore::Double factor);
      PBMathInterface::PBClass pbClass_p;
      //std::map <casacore::String, casacore::Int> convFunctionMap_p;
      casacore::Vector<casacore::Int64> convFunctionMap_p;
      casacore::Int64 nDefined_p;
      std::map <casacore::String, casacore::Int> antDiam2IndexMap_p;
      casacore::Vector<casacore::Int> antIndexToDiamIndex_p;
      casacore::Block<casacore::CountedPtr<PBMathInterface> > antMath_p;
      casacore::Int msId_p;
      casacore::Int actualConvIndex_p;
      casacore::Array<casacore::Complex> convFunc_p;
      casacore::Array<casacore::Complex> weightConvFunc_p;
      casacore::Array<casacore::Complex> convSave_p;
      casacore::Array<casacore::Complex> weightSave_p;
      casacore::Int convSize_p; 
      casacore::String vpTable_p;
      casacore::Vector<casacore::Int> convSupport_p;
      casacore::Block <casacore::CountedPtr<casacore::Array<casacore::Complex> > > convFunctions_p;
      casacore::Block <casacore::CountedPtr<casacore::Array<casacore::Complex> > > convWeights_p;
      casacore::Block<casacore::CountedPtr<casacore::Vector<casacore::Int> > > convSizes_p;
      casacore::Block <casacore::CountedPtr<casacore::Vector<casacore::Int> > > convSupportBlock_p;

    };

} // end namespace casa
#endif
