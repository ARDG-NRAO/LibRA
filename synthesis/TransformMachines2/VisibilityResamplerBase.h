// -*- C++ -*-
//# VisibilityResamplerBase.h: Definition of the VisibilityResamplerBase class
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

#ifndef SYNTHESIS_TRANSFORM2_VISIBILITYRESAMPLERBASE_H
#define SYNTHESIS_TRANSFORM2_VISIBILITYRESAMPLERBASE_H

#include <synthesis/TransformMachines2/CFStore.h>
#include <synthesis/TransformMachines2/CFStore2.h>
#include <synthesis/TransformMachines2/ConvolutionFunction.h>
#include <synthesis/TransformMachines2/Utils.h>
#include <synthesis/TransformMachines2/VBStore.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <msvis/MSVis/AsynchronousTools.h>

#include <casa/Logging/LogIO.h>
#include <casa/Logging/LogSink.h>
#include <casa/Logging/LogMessage.h>
#include <casa/OS/Timer.h>

namespace casa { //# NAMESPACE CASA - BEGIN
  using namespace vi;
  namespace refim{
  class VisibilityResamplerBase
  {
  public: 
    VisibilityResamplerBase(): 
      runTimeG_p(0.0), runTimeDG_p(0.0),runTimeG1_p(0.0), runTimeG2_p(0.0), runTimeG3_p(0.0), runTimeG4_p(0.0), runTimeG5_p(0.0), runTimeG6_p(0.0), runTimeG7_p(0.0),
      timer_p(),
      uvwScale_p(), offset_p(), chanMap_p(), polMap_p(), spwChanFreq_p(), spwChanConjFreq_p (), convFuncStore_p(), inc_p(),
      cfMap_p(), conjCFMap_p(), paTolerance_p(360.0), cached_phaseGrad_p(),vb2CFBMap_p()

    {};
    // VisibilityResamplerBase(const CFStore& cfs): 
    //   uvwScale_p(), offset_p(), chanMap_p(), polMap_p(), convFuncStore_p(), inc_p(),
    //   cfMap_p(), conjCFMap_p()
    // {setConvFunc(cfs);};

    VisibilityResamplerBase(const VisibilityResamplerBase& other):
      uvwScale_p(), offset_p(), chanMap_p(), polMap_p(), spwChanFreq_p(), spwChanConjFreq_p (), convFuncStore_p(), inc_p(),
      cfMap_p(), conjCFMap_p(), paTolerance_p(360.0),vb2CFBMap_p()
    {copy(other);}

    virtual ~VisibilityResamplerBase() {};

    VisibilityResamplerBase& operator=(const VisibilityResamplerBase& other);

    virtual VisibilityResamplerBase* clone() = 0;

    virtual void copy(const VisibilityResamplerBase& other);
    virtual void setParams(const casacore::Vector<casacore::Double>& uvwScale, 
			   const casacore::Vector<casacore::Double>& offset,
			   const casacore::Vector<casacore::Double>& dphase) = 0;

    virtual void setMaps(const casacore::Vector<casacore::Int>& chanMap, const casacore::Vector<casacore::Int>& polMap) = 0;
    virtual void setCFMaps(const casacore::Vector<casacore::Int>& cfMap, const casacore::Vector<casacore::Int>& conjCFMap)=0;
    virtual void setFreqMaps(const casacore::Matrix<casacore::Double>& spwChanFreqs, const casacore::Matrix<casacore::Double>& spwnChanConjFreqs) = 0;

    virtual void setConvFunc(const CFStore& cfs) = 0;
    virtual void setPATolerance(const double& dPA) = 0;
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
			    casacore::Bool useConjFreqCF=false) = 0;

    virtual void DataToGrid(casacore::Array<casacore::Complex>& griddedData, VBStore& vbs, 
    			    casacore::Matrix<casacore::Double>& sumwt, const casacore::Bool& dopsf,
			    casacore::Bool useConjFreqCF=false) = 0;
    //
    //------------------------------------------------------------------------------
    //
    // Re-sample VisBuffer to a regular grid (griddedData) (a.k.a. de-gridding)
    //
    virtual void GridToData(VBStore& vbs,const casacore::Array<casacore::Complex>& griddedData) = 0; 
    //    virtual void GridToData(VBStore& vbs, casacore::Array<casacore::Complex>& griddedData); 

    virtual void ComputeResiduals(VBStore& vbs) = 0;
    
    // Forward looking genealogical baggage -- required for the
    // MultiThreadedVisibilityResampler
    virtual void init(const casacore::Bool& doublePrecision) = 0;
    virtual void GatherGrids(casacore::Array<casacore::DComplex>& griddedData, casacore::Matrix<casacore::Double>& sumwt) = 0;
    virtual void GatherGrids(casacore::Array<casacore::Complex>& griddedData, casacore::Matrix<casacore::Double>& sumwt) = 0;
    virtual void initializePutBuffers(const casacore::Array<casacore::DComplex>& griddedData,
				      const casacore::Matrix<casacore::Double>& sumwt) = 0;
    virtual void initializePutBuffers(const casacore::Array<casacore::Complex>& griddedData,
				      const casacore::Matrix<casacore::Double>& sumwt) = 0;
    virtual void initializeDataBuffers(VBStore& vbs)=0;
    //
    // Aliases for more readable code at the FTMachine layer.
    //
    inline void finalizeToSky(casacore::Array<casacore::DComplex>& griddedData, casacore::Matrix<casacore::Double>& sumwt) 
    {GatherGrids(griddedData, sumwt);};
    inline void finalizeToSky(casacore::Array<casacore::Complex>& griddedData, casacore::Matrix<casacore::Double>& sumwt) 
    {GatherGrids(griddedData, sumwt);};
    inline void initializeToSky(const casacore::Array<casacore::DComplex>& griddedData,const casacore::Matrix<casacore::Double>& sumwt) 
    {initializePutBuffers(griddedData, sumwt);};
    inline void initializeToSky(const casacore::Array<casacore::Complex>& griddedData,const casacore::Matrix<casacore::Double>& sumwt)
    {initializePutBuffers(griddedData, sumwt);};
    const casacore::Vector<casacore::Int> getCFMap() {return cfMap_p;};
    const casacore::Vector<casacore::Int> getConjCFMap() {return conjCFMap_p;};

    
    virtual void releaseBuffers() = 0;
//    VBRow2CFMapType& getVBRow2CFMap() {return vbRow2CFMap_p;};
    VB2CFBMap& getVBRow2CFBMap() {return *vb2CFBMap_p;};
    // virtual casacore::Int makeVBRow2CFBMap(CFStore2& cfs,
    // 				ConvolutionFunction& cf,
    // 				const VisBuffer2& vb, const casacore::Quantity& dPA,
    // 				const casacore::Vector<casacore::Int>& dataChan2ImChanMap,
    // 				const casacore::Vector<casacore::Int>& dataPol2ImPolMap,
    // 				const casacore::Vector<casacore::Double>& pointingOffset);

    void setFieldPhaseGrad(const casacore::Matrix<casacore::Complex>& phaseGrad) {cached_phaseGrad_p.reference(phaseGrad);};
    void setVB2CFMap(const casacore::CountedPtr<refim::VB2CFBMap>& thisMap);

    casacore::Double runTimeG_p, runTimeDG_p, runTimeG1_p, runTimeG2_p, runTimeG3_p, runTimeG4_p, runTimeG5_p, runTimeG6_p, runTimeG7_p;
    casacore::Timer timer_p;
    //
    //------------------------------------------------------------------------------
    //----------------------------Private parts-------------------------------------
    //------------------------------------------------------------------------------
    //
    //  private:
  protected:
    casacore::Vector<casacore::Double> uvwScale_p, offset_p, dphase_p;
    casacore::Vector<casacore::Int> chanMap_p, polMap_p;
    casacore::Matrix<casacore::Double> spwChanFreq_p, spwChanConjFreq_p;
    CFStore convFuncStore_p;
    //    casacore::Int inc0_p, inc1_p, inc2_p, inc3_p;
    casacore::Vector<casacore::Int> inc_p;
    casacore::Int* __restrict__ incPtr_p;
    casacore::Vector<casacore::Int> cfMap_p, conjCFMap_p;
//    VBRow2CFMapType vbRow2CFMap_p;
    double paTolerance_p;
    casacore::Matrix<casacore::Complex> cached_phaseGrad_p;
    casacore::CountedPtr<refim::VB2CFBMap> vb2CFBMap_p;

    void sgrid(casacore::Int& ndim, 
	       casacore::Double* __restrict__  pos, 
	       casacore::Int* __restrict__  loc, 
	       casacore::Int* __restrict__  off, 
    	       casacore::Complex& phasor, const casacore::Int& irow, 
	       const casacore::Double* __restrict__  uvw, 
    	       const casacore::Double& dphase, const casacore::Double& freq, 
    	       const casacore::Double* __restrict__  scale, 
	       const casacore::Double* __restrict__  offset,
    	       const casacore::Float* __restrict__  sampling);

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


    //===============================================================================
    // CASACORE-LEVEL MATERIAL
    //===============================================================================
    // Internal methods to address a 4D array.  These should ulimately
    // moved to a Array4D class in CASACore
    //

    // This is called less frequently.  Currently once per VisBuffer
    inline void cacheAxisIncrements(const casacore::Int& n0, const casacore::Int& n1, const casacore::Int& n2, const casacore::Int& n3)
    {
      //      inc0_p=1, inc1_p=inc0_p*n0, inc2_p=inc1_p*n1, inc3_p=inc2_p*n2;(void)n3;
      inc_p.resize(4);
      inc_p[0]=1; inc_p[1]=inc_p[0]*n0; inc_p[2]=inc_p[1]*n1; inc_p[3]=inc_p[2]*n2;(void)n3;
      casacore::Bool D;
      incPtr_p = inc_p.getStorage(D);
    }
    inline void cacheAxisIncrements(const casacore::Vector<casacore::Int>& n)
    {cacheAxisIncrements(n[0],n[1],n[2],n[3]);}

    inline void cacheAxisIncrements(const casacore::Vector<casacore::Int>& n, casacore::Vector<casacore::Int>& inc)
    {inc.resize(4);inc[0]=1; inc[1]=inc[0]*n[0]; inc[2]=inc[1]*n[1]; inc[3]=inc[2]*n[2];(void)n[3];}

    inline void cacheAxisIncrements(const casacore::Int n[4], casacore::Int inc[4])
    {inc[0]=1; inc[1]=inc[0]*n[0]; inc[2]=inc[1]*n[1]; inc[3]=inc[2]*n[2];(void)n[3];}

    // Version that use internally cached inc_p
    //    template <class T>
    inline void addTo4DArray(casacore::DComplex* __restrict__& store, casacore::Int* __restrict__& iPos, 
			     casacore::Complex& nvalue, casacore::Double& wt) __restrict__ 
    {addTo4DArray(store, iPos, incPtr_p, nvalue, wt);}

    inline void addTo4DArray(casacore::Complex* __restrict__& store, casacore::Int* __restrict__& iPos, 
			     casacore::Complex& nvalue, casacore::Double& wt) __restrict__ 
    {addTo4DArray(store, iPos, incPtr_p, nvalue, wt);}

    
    // Version where inc_p is supplied from outside
    inline void addTo4DArray(casacore::DComplex* __restrict__& store, casacore::Int* __restrict__& iPos, 
			     casacore::Int* __restrict__ inc, casacore::Complex& nvalue, casacore::Double& wt) __restrict__ 
    {store[iPos[0] + iPos[1]*inc[1] + iPos[2]*inc[2] +iPos[3]*inc[3]] += (nvalue*casacore::Complex(wt));}

    inline void addTo4DArray(casacore::Complex* __restrict__& store, casacore::Int* __restrict__& iPos, 
			     casacore::Int* __restrict__ inc, casacore::Complex& nvalue, casacore::Double& wt) __restrict__ 
    {store[iPos[0] + iPos[1]*inc[1] + iPos[2]*inc[2] +iPos[3]*inc[3]] += (nvalue*casacore::Complex(wt));}


    inline casacore::Complex getFrom4DArray(const casacore::Complex* __restrict__& store, 
				  const casacore::Int* __restrict__& iPos, 
				  const casacore::Vector<casacore::Int>& inc) 
    //  __restrict__ 
    {return store[iPos[0] + iPos[1]*inc[1] + iPos[2]*inc[2] +iPos[3]*inc[3]];};

    inline casacore::Complex getFrom4DArray(const casacore::Complex* __restrict__& store, 
				  const casacore::Vector<casacore::Int> iPos, const casacore::Vector<casacore::Int>& inc) 
    //  __restrict__ 
    {return store[iPos[0] + iPos[1]*inc[1] + iPos[2]*inc[2] +iPos[3]*inc[3]];};

    inline casacore::DComplex getFrom4DArray(const casacore::DComplex* __restrict__& store, 
		                   const casacore::Int* __restrict__& iPos, 
				   const casacore::Vector<casacore::Int>& inc) 
    //  __restrict__ 
    {return store[iPos[0] + iPos[1]*inc[1] + iPos[2]*inc[2] +iPos[3]*inc[3]];};

    inline casacore::DComplex getFrom4DArray(const casacore::DComplex* __restrict__& store, 
				  const casacore::Vector<casacore::Int> iPos, const casacore::Vector<casacore::Int>& inc) 
    //  __restrict__ 
    {return store[iPos[0] + iPos[1]*inc[1] + iPos[2]*inc[2] +iPos[3]*inc[3]];};


    // The following two methods are called in the innermost loop.
    inline casacore::Complex getFrom4DArray(const casacore::Complex* __restrict__& store, const casacore::Int* __restrict__& iPos) 
    //  __restrict__ 
    {return getFrom4DArray(store, iPos, inc_p);}

    inline casacore::DComplex getFrom4DArray(const casacore::DComplex* __restrict__& store, const casacore::Int* __restrict__& iPos) 
    //  __restrict__ 
    {return getFrom4DArray(store, iPos, inc_p);}

  };
}; //# NAMESPACE CASA - END
};
#endif // 
