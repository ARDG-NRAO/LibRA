// -*- C++ -*-
//# VisibilityResampler.h: Definition of the VisibilityResampler class
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

#ifndef SYNTHESIS_TRANSFORM2_VISIBILITYRESAMPLER_H
#define SYNTHESIS_TRANSFORM2_VISIBILITYRESAMPLER_H

#include <synthesis/TransformMachines2/CFStore.h>
#include <synthesis/TransformMachines2/Utils.h>
#include <synthesis/TransformMachines2/VBStore.h>
#include <synthesis/TransformMachines2/VisibilityResamplerBase.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
//#include <msvis/MSVis/AsynchronousTools.h>

#include <casa/Logging/LogIO.h>
#include <casa/Logging/LogSink.h>
#include <casa/Logging/LogMessage.h>

namespace casa { //# NAMESPACE CASA - BEGIN
  namespace refim{
  class VisibilityResampler: public VisibilityResamplerBase
  {
  public: 
    VisibilityResampler(): VisibilityResamplerBase() {};
    //    VisibilityResampler(const CFStore& cfs): VisibilityResamplerBase(cfs) {};
    VisibilityResampler(const VisibilityResampler& other):VisibilityResamplerBase()
    {copy(other);}

    //    {setConvFunc(cfs);};
    virtual ~VisibilityResampler() {};

    //    VisibilityResampler& operator=(const VisibilityResampler& other);

    void copy(const VisibilityResamplerBase& other)
    {VisibilityResamplerBase::copy(other);}

    virtual VisibilityResamplerBase* clone() 
    {return new VisibilityResampler(*this);}

    virtual void getParams(casacore::Vector<casacore::Double>& uvwScale, casacore::Vector<casacore::Double>& offset, casacore::Vector<casacore::Double>& dphase)
    {uvwScale.assign(uvwScale_p); offset.assign(offset_p); dphase.assign(dphase_p);};
    virtual void setParams(const casacore::Vector<casacore::Double>& uvwScale, const casacore::Vector<casacore::Double>& offset,
			   const casacore::Vector<casacore::Double>& dphase)
    {
      // SynthesisUtils::SETVEC(uvwScale_p, uvwScale); 
      // SynthesisUtils::SETVEC(offset_p, offset);
      // SynthesisUtils::SETVEC(dphase_p, dphase);
      uvwScale_p.reference(uvwScale);
      offset_p.reference(offset);
      dphase_p.reference(dphase);
    };

    virtual void setMaps(const casacore::Vector<casacore::Int>& chanMap, const casacore::Vector<casacore::Int>& polMap)
    {
      // SynthesisUtils::SETVEC(chanMap_p,chanMap);
      // SynthesisUtils::SETVEC(polMap_p,polMap);
      chanMap_p.reference(chanMap);
      polMap_p.reference(polMap);
    }

    virtual void setFreqMaps(const casacore::Matrix<casacore::Double>& spwChanFreqs, const casacore::Matrix<casacore::Double>& spwChanConjFreqs)
    {
      spwChanFreq_p.assign(spwChanFreqs);
      spwChanConjFreq_p.assign(spwChanConjFreqs);
    }

    virtual void setConvFunc(const CFStore& cfs) 
    {
      convFuncStore_p = cfs;
    };
    virtual void setCFMaps(const casacore::Vector<casacore::Int>& cfMap, const casacore::Vector<casacore::Int>& conjCFMap) {(void)cfMap;(void)conjCFMap;};
    virtual void setPATolerance(const double& dPA) {paTolerance_p = dPA;};
    //
    //------------------------------------------------------------------------------
    //
    // Re-sample the griddedData on the VisBuffer (a.k.a gridding).
    //
    // In this class, these just call the private templated version.
    // The first variant grids onto a double precision grid while the
    // second one does it on a single precision grid.
    //
    virtual void DataToGrid(casacore::Array<casacore::DComplex>& griddedData, VBStore& vbs, 
			    casacore::Matrix<casacore::Double>& sumwt, const casacore::Bool& dopsf,
			    casacore::Bool /*useConjFreqCF*/=false)
    {DataToGridImpl_p(griddedData, vbs, dopsf, sumwt);}

    virtual void DataToGrid(casacore::Array<casacore::Complex>& griddedData, VBStore& vbs, 
    			    casacore::Matrix<casacore::Double>& sumwt, const casacore::Bool& dopsf,
			    casacore::Bool /*useConjFreqCF*/=false)
    {DataToGridImpl_p(griddedData, vbs, dopsf, sumwt);}

    //
    //------------------------------------------------------------------------------
    //
    // Re-sample VisBuffer to a regular grid (griddedData) (a.k.a. de-gridding)
    //
    virtual void GridToData(VBStore& vbs,const casacore::Array<casacore::Complex>& griddedData); 
    //    virtual void GridToData(VBStore& vbs, casacore::Array<casacore::Complex>& griddedData); 

    virtual void ComputeResiduals(VBStore& vbs);
    virtual void setMutex(async::Mutex *mu) {myMutex_p = mu;};

    // Genealogical baggage -- required for the
    // MultiThreadedVisibilityResampler -- that everyone else has to
    // carray around.
    //
    // These are no-ops for unithreaded samplers.
    //
    virtual void init(const casacore::Bool& doublePrecision) {(void)doublePrecision;};
    virtual void GatherGrids(casacore::Array<casacore::DComplex>& griddedData, casacore::Matrix<casacore::Double>& sumwt) {(void)griddedData;(void)sumwt;};
    virtual void GatherGrids(casacore::Array<casacore::Complex>& griddedData, casacore::Matrix<casacore::Double>& sumwt) {(void)griddedData;(void)sumwt;};
    virtual void initializePutBuffers(const casacore::Array<casacore::DComplex>& griddedData,
				      const casacore::Matrix<casacore::Double>& sumwt) {(void)griddedData;(void)sumwt;};
    virtual void initializePutBuffers(const casacore::Array<casacore::Complex>& griddedData,
				      const casacore::Matrix<casacore::Double>& sumwt) {(void)griddedData;(void)sumwt;};
    virtual void initializeDataBuffers(VBStore& vbs) {(void)vbs;};


    virtual void releaseBuffers() {};
    //
    //------------------------------------------------------------------------------
    //----------------------------Private parts-------------------------------------
    //------------------------------------------------------------------------------
    //
  protected:
    async::Mutex *myMutex_p;
    // casacore::Vector<casacore::Double> uvwScale_p, offset_p, dphase_p;
    // casacore::Vector<casacore::Int> chanMap_p, polMap_p;
    // CFStore convFuncStore_p;
    //    casacore::Int inc0_p, inc1_p, inc2_p, inc3_p;
    //
    // Re-sample the griddedData on the VisBuffer (a.k.a de-gridding).
    //
    template <class T>
    void DataToGridImpl_p(casacore::Array<T>& griddedData, VBStore& vb,  
			  const casacore::Bool& dopsf, casacore::Matrix<casacore::Double>& sumwt,casacore::Bool useConjFreqCF=false);

    // void sgrid(casacore::Vector<casacore::Double>& pos, casacore::Vector<casacore::Int>& loc, casacore::Vector<casacore::Int>& off, 
    // 	       casacore::Complex& phasor, const casacore::Int& irow, const casacore::Matrix<casacore::Double>& uvw, 
    // 	       const casacore::Double& dphase, const casacore::Double& freq, 
    // 	       const casacore::Vector<casacore::Double>& scale, const casacore::Vector<casacore::Double>& offset,
    // 	       const casacore::Vector<casacore::Float>& sampling);

    /*
    void sgrid(casacore::Int& ndim, casacore::Double* pos, casacore::Int* loc, casacore::Int* off, 
    	       casacore::Complex& phasor, const casacore::Int& irow, const casacore::Double* uvw, 
    	       const casacore::Double& dphase, const casacore::Double& freq, 
    	       const casacore::Double* scale, const casacore::Double* offset,
    	       const casacore::Float* sampling);

    inline casacore::Bool onGrid (const casacore::Int& nx, const casacore::Int& ny, 
			const casacore::Vector<casacore::Int>& __restrict__ loc, 
			const casacore::Vector<casacore::Int>& __restrict__ support) __restrict__ 
    {
      return (((loc(0)-support[0]) >= 0 ) && ((loc(0)+support[0]) < nx) &&
	      ((loc(1)-support[1]) >= 0 ) && ((loc(1)+support[1]) < ny));
    };
    inline casacore::Bool onGrid (const casacore::Int& nx, const casacore::Int& ny, 
			const casacore::Int& loc0, const casacore::Int& loc1, 
			const casacore::Int& support) __restrict__ 
    {
      return (((loc0-support) >= 0 ) && ((loc0+support) < nx) &&
	      ((loc1-support) >= 0 ) && ((loc1+support) < ny));
    };

    // casacore::Array assignment operator in CASACore requires lhs.nelements()
    // == 0 or lhs.nelements()=rhs.nelements()
    // template <class T>
    // inline void SETVEC(casacore::Vector<T>& lhs, const casacore::Vector<T>& rhs)
    // {lhs.resize(rhs.shape()); lhs = rhs;};


    //
    // Internal methods to address a 4D array.  These should ulimately
    // moved to a Array4D class in CASACore
    //

    // This is called less frequently.  Currently once per VisBuffer
    inline void cacheAxisIncrements(casacore::Int& n0, casacore::Int& n1, casacore::Int& n2, casacore::Int& n3)
    {inc0_p=1, inc1_p=inc0_p*n0, inc2_p=inc1_p*n1, inc3_p=inc2_p*n2;(void)n3;}


    // The following two methods are called in the innermost loop.
    inline casacore::Complex getFrom4DArray(const casacore::Complex* __restrict__ store,
				  const casacore::Int* __restrict__ iPos) __restrict__ 
    {return store[iPos[0] + iPos[1]*inc1_p + iPos[2]*inc2_p +iPos[3]*inc3_p];};

    template <class T>
    void addTo4DArray(T* __restrict__ store,
		      const casacore::Int* __restrict__ iPos, 
		      casacore::Complex& nvalue, casacore::Double& wt) __restrict__ 
    {store[iPos[0] + iPos[1]*inc1_p + iPos[2]*inc2_p +iPos[3]*inc3_p] += (nvalue*wt);}
    */
  };
}; //# NAMESPACE CASA - END
};
#endif // 
