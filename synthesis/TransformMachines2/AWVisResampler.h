// -*- C++ -*-
//# AWVisResampler.h: Definition of the AWVisResampler class
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

#ifndef SYNTHESIS_TRANSFORM2_AWVISRESAMPLER_H
#define SYNTHESIS_TRANSFORM2_AWVISRESAMPLER_H

#include <synthesis/TransformMachines2/CFStore.h>
#include <synthesis/TransformMachines2/VBStore.h>
#include <synthesis/TransformMachines2/VisibilityResampler.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>

#include <casa/Logging/LogIO.h>
#include <casa/Logging/LogSink.h>
#include <casa/Logging/LogMessage.h>

namespace casa { //# NAMESPACE CASA - BEGIN
  namespace refim{
  class AWVisResampler: public VisibilityResampler
  {
  public: 
    AWVisResampler(): VisibilityResampler(),
		      //		      cached_phaseGrad_p(),
                      cached_PointingOffset_p()
    {cached_PointingOffset_p.resize(2);cached_PointingOffset_p=-1000.0;runTimeG_p=runTimeDG_p=0.0;};
    //    AWVisResampler(const CFStore& cfs): VisibilityResampler(cfs)      {}
    virtual ~AWVisResampler()                                         {};

    virtual VisibilityResamplerBase* clone()
    {return new AWVisResampler(*this);}
    
    // AWVisResampler(const AWVisResampler& other): VisibilityResampler(other),cfMap_p(), conjCFMap_p()
    // {copy(other);}

    virtual void copyMaps(const AWVisResampler& other)
    {setCFMaps(other.cfMap_p, other.conjCFMap_p);}
    virtual void copy(const VisibilityResamplerBase& other) 
    {
      VisibilityResampler::copy(other);
      // const casacore::Vector<casacore::Int> cfmap=other.getCFMap();
      // const casacore::Vector<casacore::Int> conjcfmap = other.getConjCFMap();

      // setCFMaps(cfmap,conjcfmap);
    }

    virtual void copy(const AWVisResampler& other) 
    {
      VisibilityResampler::copy(other);
      SynthesisUtils::SETVEC(cached_phaseGrad_p, other.cached_phaseGrad_p);
      SynthesisUtils::SETVEC(cached_PointingOffset_p, other.cached_PointingOffset_p);
    }

    AWVisResampler& operator=(const AWVisResampler& other) 
    {
      copy(other);      
      SynthesisUtils::SETVEC(cached_phaseGrad_p, other.cached_phaseGrad_p);
      SynthesisUtils::SETVEC(cached_PointingOffset_p, other.cached_PointingOffset_p);
      return *this;
    }

    virtual void setCFMaps(const casacore::Vector<casacore::Int>& cfMap, const casacore::Vector<casacore::Int>& conjCFMap)
    {SETVEC(cfMap_p,cfMap);SETVEC(conjCFMap_p,conjCFMap);}

    // virtual void setConvFunc(const CFStore& cfs) {convFuncStore_p = cfs;};
    //
    //------------------------------------------------------------------------------
    //
    // Re-sample the griddedData on the VisBuffer (a.k.a gridding).
    //
    // In this class, these just call the private templated version.
    // The first variant grids onto a double precision grid while the
    // second one does it on a single precision grid.
    //
    // Note that the following calls allow using any CFStore object
    // for gridding while de-gridding uses the internal
    // convFuncStore_p object.
    // virtual void DataToGrid(casacore::Array<casacore::DComplex>& griddedData, VBStore& vbs, casacore::Matrix<casacore::Double>& sumwt,
    // 			    const casacore::Bool& dopsf, CFStore& cfs)
    // {DataToGridImpl_p(griddedData, vbs, sumwt,dopsf,cfs);}

    // virtual void DataToGrid(casacore::Array<casacore::Complex>& griddedData, VBStore& vbs, casacore::Matrix<casacore::Double>& sumwt,
    // 			    const casacore::Bool& dopsf, CFStore& cfs)
    // {DataToGridImpl_p(griddedData, vbs, sumwt,dopsf,cfs);}
    //
    // Simulating defaulting CFStore arguemnt in the above calls to convFuncStore_p
    //

    //***TEMP REMOVAL OF casacore::DComplex gridder*****

    virtual void DataToGrid(casacore::Array<casacore::DComplex>& griddedData, VBStore& vbs, casacore::Matrix<casacore::Double>& sumwt,
    			    const casacore::Bool& dopsf,casacore::Bool useConjFreqCF=false)
    {DataToGridImpl_p(griddedData, vbs, sumwt,dopsf,useConjFreqCF);}

    virtual void DataToGrid(casacore::Array<casacore::Complex>& griddedData, VBStore& vbs, casacore::Matrix<casacore::Double>& sumwt,
			    const casacore::Bool& dopsf,casacore::Bool useConjFreqCF=false)
    {DataToGridImpl_p(griddedData, vbs, sumwt,dopsf,useConjFreqCF);}

    //
    //------------------------------------------------------------------------------
    //
    // Re-sample VisBuffer to a regular grid (griddedData) (a.k.a. de-gridding)
    //
    virtual void GridToData(VBStore& vbs,const casacore::Array<casacore::Complex>& griddedData); 
    //    virtual void GridToData(VBStore& vbs, casacore::Array<casacore::Complex>& griddedData); 
  protected:
    virtual casacore::Complex getConvFuncVal(const casacore::Cube<casacore::Double>& convFunc, const casacore::Matrix<casacore::Double>& uvw, 
				   const casacore::Int& irow, const casacore::Vector<casacore::Int>& pixel)
    {
      (void)uvw; (void)irow;return convFunc(pixel[0],pixel[1],pixel[2]);
    }
    casacore::Complex getCFArea(casacore::Complex* __restrict__& convFuncV, casacore::Double& wVal,
		      casacore::Vector<casacore::Int>& scaledSupport, casacore::Vector<casacore::Float>& scaledSampling,
		      casacore::Vector<casacore::Double>& off,
		      casacore::Vector<casacore::Int>& convOrigin, casacore::Vector<casacore::Int>& cfShape,
		      casacore::Double& sinDPA, casacore::Double& cosDPA);

  template <class T>
  casacore::Complex accumulateOnGrid(casacore::Array<T>& grid, casacore::Complex* __restrict__& convFuncV, 
			   casacore::Complex& nvalue,
			   casacore::Double& wVal, casacore::Vector<casacore::Int>& scaledSupport, 
			   casacore::Vector<casacore::Float>& scaledSampling, casacore::Vector<casacore::Double>& off,
			   casacore::Vector<casacore::Int>& convOrigin, casacore::Vector<casacore::Int>& /*cfShape*/,
			   casacore::Vector<casacore::Int>& loc, casacore::Vector<casacore::Int>& igrdpos, 
			   casacore::Double& /*sinDPA*/, casacore::Double& /*cosDPA*/,
			   casacore::Bool& finitePointingOffset, casacore::Bool dopsf);
  template <class T>
  void XInnerLoop(const casacore::Int *scaleSupport, const casacore::Float* scaledSampling,
		  const casacore::Double* off,
		  const casacore::Int* loc, casacore::Complex& cfArea,  
		  const casacore::Int * __restrict__ iGrdPosPtr,
		  casacore::Complex *__restrict__& convFuncV,
		  const casacore::Int* convOrigin,
		  casacore::Complex& nvalue,
		  casacore::Double& wVal,
		  casacore::Bool& /*finitePointingOffset*/,
		  casacore::Bool& /*doPSFOnly*/,
		  T* __restrict__ gridStore,
		  casacore::Int* iloc,
		  casacore::Complex& norm,
		  casacore::Int* igrdpos);

  template <class T>
  void accumulateFromGrid(T& nvalue, casacore::Complex& norm, const T* __restrict__& grid, 
			  casacore::Vector<casacore::Int>& iGrdPos,
			  casacore::Complex* __restrict__& convFuncV, 
			  casacore::Double& wVal, casacore::Vector<casacore::Int>& scaledSupport, 
			  casacore::Vector<casacore::Float>& scaledSampling, casacore::Vector<casacore::Double>& off,
			  casacore::Vector<casacore::Int>& convOrigin, casacore::Vector<casacore::Int>& cfShape,
			  casacore::Vector<casacore::Int>& loc, 
			  casacore::Complex& phasor, 
			  casacore::Double& sinDPA, casacore::Double& cosDPA,
			  casacore::Bool& finitePointingOffset, 
			  casacore::Matrix<casacore::Complex>& cached_phaseGrad_p);

    //
    //------------------------------------------------------------------------------
    //----------------------------Private parts-------------------------------------
    //------------------------------------------------------------------------------
    //
  private:
    // casacore::Vector<casacore::Double> uvwScale_p, offset_p, dphase_p;
    // casacore::Vector<casacore::Int> chanMap_p, polMap_p;
    // CFStore convFuncStore_p;
    // //    casacore::Int inc0_p, inc1_p, inc2_p, inc3_p;
    // casacore::Vector<casacore::Int> inc_p;
    //    casacore::Vector<casacore::Int> cfMap_p, conjCFMap_p;
    casacore::Vector<casacore::Int> gridInc_p, cfInc_p;
    //    casacore::Matrix<casacore::Complex> cached_phaseGrad_p;
    casacore::Vector<casacore::Double> cached_PointingOffset_p;
    //
    // Re-sample the griddedData on the VisBuffer (a.k.a de-gridding).
    //
    template <class T>
    void DataToGridImpl_p(casacore::Array<T>& griddedData, VBStore& vb,  
			  casacore::Matrix<casacore::Double>& sumwt,const casacore::Bool& dopsf,
			  casacore::Bool /*useConjFreqCF*/);

    void sgrid(casacore::Vector<casacore::Double>& pos, casacore::Vector<casacore::Int>& loc, casacore::Vector<casacore::Double>& off, 
    	       casacore::Complex& phasor, const casacore::Int& irow, const casacore::Matrix<casacore::Double>& uvw, 
    	       const casacore::Double& dphase, const casacore::Double& freq, 
    	       const casacore::Vector<casacore::Double>& scale, const casacore::Vector<casacore::Double>& offset,
    	       const casacore::Vector<casacore::Float>& sampling);

    inline casacore::Bool onGrid (const casacore::Int& nx, const casacore::Int& ny, const casacore::Int& nw, 
    			const casacore::Vector<casacore::Int>& loc, 
    			const casacore::Vector<casacore::Int>& support)
    {
      return (((loc(0)-support[0]) >= 0 ) && ((loc(0)+support[0]) < nx) &&
    	      ((loc(1)-support[1]) >= 0 ) && ((loc(1)+support[1]) < ny) &&
    	      (loc(2) >= 0) && (loc(2) <= nw));
    };

    // casacore::Array assignment operator in CASACore requires lhs.nelements()
    // == 0 or lhs.nelements()=rhs.nelements()
    template <class T>
    inline void SETVEC(casacore::Vector<T>& lhs, const casacore::Vector<T>& rhs)
    {lhs.resize(rhs.shape()); lhs = rhs;};


    //
    // Internal methods to address a 4D array.  These should ulimately
    // moved to a Array4D class in CASACore
    //

    // This is called less frequently.  Currently once per VisBuffer
    // inline void cacheAxisIncrements(const casacore::Vector<casacore::Int>& n, casacore::Vector<casacore::Int>& inc)
    // {inc.resize(4);inc[0]=1, inc[1]=inc[0]*n[0], inc[2]=inc[1]*n[1], inc[3]=inc[2]*n[2];(void)n[3];}


    // The following method is also called from the inner loop, but
    // does not use CASA casacore::Vector (which are not thread safe, I (SB) am
    // told).
    inline casacore::Complex getFrom4DArray(const casacore::Complex *__restrict__& store,
    				  const casacore::Int* iPos, const casacore::Int* inc)
    {
      return *(store+(iPos[0] + iPos[1]*inc[1] + iPos[2]*inc[2] +iPos[3]*inc[3]));
      //      return store[iPos[0] + iPos[1]*inc[1] + iPos[2]*inc[2] +iPos[3]*inc[3]];
    };

    // The following two methods are called in the innermost loop.
    inline casacore::Complex getFrom4DArray(const casacore::Complex *__restrict__& store,
    				  const casacore::Vector<casacore::Int>& iPos, const casacore::Vector<casacore::Int>& inc)
    {
      return *(store+(iPos[0] + iPos[1]*inc[1] + iPos[2]*inc[2] +iPos[3]*inc[3]));
      //      return store[iPos[0] + iPos[1]*inc[1] + iPos[2]*inc[2] +iPos[3]*inc[3]];
    };
    inline casacore::DComplex getFrom4DArray(const casacore::DComplex *__restrict__& store,
    				  const casacore::Vector<casacore::Int>& iPos, const casacore::Vector<casacore::Int>& inc)
    {
      return *(store+(iPos[0] + iPos[1]*inc[1] + iPos[2]*inc[2] +iPos[3]*inc[3]));
      //      return store[iPos[0] + iPos[1]*inc[1] + iPos[2]*inc[2] +iPos[3]*inc[3]];
    };

    template <class T>
    void addTo4DArray(T *__restrict__& store,
    		      const casacore::Int *__restrict__& iPos, const casacore::Vector<casacore::Int>& inc, 
		      casacore::Complex& nvalue, casacore::Complex& wt) __restrict__
    {
      // T *tmp=store+(iPos[0] + iPos[1]*inc[1] + iPos[2]*inc[2] +iPos[3]*inc[3]);
      // *tmp += nvalue*wt;
      store[iPos[0] + iPos[1]*inc[1] + iPos[2]*inc[2] +iPos[3]*inc[3]] += (nvalue*wt);
    }

    template <class T>
    void addTo4DArray_ptr(T *__restrict__& store,
			  const casacore::Int *__restrict__& iPos,
			  const casacore::Int *__restrict__& inc, 
			  casacore::Complex& nvalue, casacore::Complex& wt) __restrict__
    {
      // T *tmp=store+(iPos[0] + iPos[1]*inc[1] + iPos[2]*inc[2] +iPos[3]*inc[3]);
      // *tmp += nvalue*wt;
      store[iPos[0] + iPos[1]*inc[1] + iPos[2]*inc[2] +iPos[3]*inc[3]] += (nvalue*wt);
    }

    //
    // This rotates the convolution function by rotating the
    // co-ordinate system.  For the accuracies already required for
    // EVLA and ALMA, this is not useful.  Leaving it hear for now....
    //
    casacore::Bool reindex(const casacore::Vector<casacore::Int>& in, casacore::Vector<casacore::Int>& out,
			   const casacore::Double& sinDPA, const casacore::Double& cosDPA,
			   const casacore::Vector<casacore::Int>& Origin, const casacore::Vector<casacore::Int>& size);

    casacore::Complex* getConvFunc_p(const double& vbPA,
				     casacore::Vector<casacore::Int>& cfShape,
				     casacore::Vector<int>& support,
				     int& muellerElement,
				     CountedPtr<CFBuffer>& cfb,
				     casacore::Double& wVal, casacore::Int& fndx, 
				     casacore::Int& wndx,
				     PolMapType& mNdx, PolMapType& conjMNdx,
				     casacore::Int& ipol, casacore::uInt& mRow);
    void cachePhaseGrad_p(const casacore::Vector<casacore::Double>& pointingOffset,
			  const casacore::Vector<casacore::Int>&cfShape,
			  const casacore::Vector<casacore::Int>& convOrigin,
			  const casacore::Double& cfRefFreq,
			  const casacore::Double& imRefFreq,
			  const casacore::Int& spwID=0, const casacore::Int& fieldId=0);
  };
}; //# NAMESPACE CASA - END
};
#endif // 
