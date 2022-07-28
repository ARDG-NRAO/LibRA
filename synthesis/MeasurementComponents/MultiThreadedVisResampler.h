// -*- C++ -*-
//# MultiThreadedVisResampler.h: Definition of the MultiThreadedVisibilityResampler class
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

#ifndef SYNTHESIS_MULTITHREADEDVISIBILITYRESAMPLER_H
#define SYNTHESIS_MULTITHREADEDVISIBILITYRESAMPLER_H

#include <synthesis/TransformMachines/VisibilityResamplerBase.h>
#include <synthesis/MeasurementComponents/ResamplerWorklet.h>
#include <synthesis/TransformMachines/CFStore.h>
#include <synthesis/TransformMachines/VBStore.h>
#include <synthesis/MeasurementComponents/MThWorkIDEnum.h>
#include <msvis/MSVis/UtilJ.h>
#include <msvis/MSVis/VisBuffer.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/casa/Arrays/Vector.h>

#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Logging/LogSink.h>
#include <casacore/casa/Logging/LogMessage.h>
#define DEFAULTNOOFCORES -1
#define FTMachineNumThreadsEnvVar "ftmachine_num_threads"
namespace casa { //# NAMESPACE CASA - BEGIN
  class MultiThreadedVisibilityResampler: public VisibilityResamplerBase
  {
  public: 
    //    enum VBLoader {NOONE=-1, DATATOGRID=0, GRIDTODATA, RESIDUALCALC};
     MultiThreadedVisibilityResampler():
      resamplers_p(), doubleGriddedData_p(), singleGriddedData_p(), 
      sumwt_p(), gridderWorklets_p(), vbsVec_p(), visResamplerCtor_p(), 
      whoLoadedVB_p(MThWorkID::NOONE), currentVBS_p(0)
    {
      nelements_p = SynthesisUtils::getenv(FTMachineNumThreadsEnvVar, 1);
      if (nelements_p < 0) nelements_p = 1;
      //      nelements_p=DEFAULTNOOFCORES;
    };

    MultiThreadedVisibilityResampler(const casacore::Bool& doublePrecision, 
				     const casacore::Int& n=DEFAULTNOOFCORES);

    MultiThreadedVisibilityResampler(const casacore::Bool& doublePrecision, 
				     casacore::CountedPtr<VisibilityResamplerBase>& visResampler, 
				     const casacore::Int& n=DEFAULTNOOFCORES);
    virtual ~MultiThreadedVisibilityResampler()
    {
      cleanup();
      nelements_p=0;
      currentVBS_p=0;
    }

    MultiThreadedVisibilityResampler(const MultiThreadedVisibilityResampler& other) : VisibilityResamplerBase() {copy(other);};

    // This version will make a clone with shared data buffers (the
    // complex grids and the sum-of-weights arrays).
    MultiThreadedVisibilityResampler* clone()
    {
      //Allocate a new instance, and copy the internals.
      MultiThreadedVisibilityResampler *clonedCopy;
      clonedCopy = new MultiThreadedVisibilityResampler(*this);
      // Now reset the data buffers with independent buffers (arrays of size 0)
      clonedCopy->allocateDataBuffers();
      return clonedCopy;
    };

    MultiThreadedVisibilityResampler& operator=(const MultiThreadedVisibilityResampler& other);

    void cleanup();
    void releaseBuffers();

    void copy(const MultiThreadedVisibilityResampler& other);

    virtual casacore::Int nelements() {return nelements_p;};
    virtual void setParams(const casacore::Vector<casacore::Double>& uvwScale, const casacore::Vector<casacore::Double>& offset,
			   const casacore::Vector<casacore::Double>& dphase);

    virtual void setMaps(const casacore::Vector<casacore::Int>& chanMap, const casacore::Vector<casacore::Int>& polMap);
    virtual void setCFMaps(const casacore::Vector<casacore::Int>& cfMap, const casacore::Vector<casacore::Int>& conjCFMap);
    virtual void setFreqMaps(const casacore::Matrix<casacore::Double>& spwChanFreqs, const casacore::Matrix<casacore::Double>& spwChanConjFreqs);

    virtual void setConvFunc(const CFStore& cfs);
    virtual void setPATolerance(const double& dPA) {paTolerance_p = dPA;}
    void init(const casacore::Bool& doublePrecision);
    //
    //------------------------------------------------------------------------------
    //
    // Re-sample the griddedData on the VisBuffer (a.k.a gridding).
    //
    // In this class, these just call the private templated version.
    // The first variant grids onto a double precision grid while the
    // second one does it on a single precision grid.
    //
    virtual void DataToGrid(casacore::Array<casacore::DComplex>& griddedData,  
			    VBStore& vbs, 
			    casacore::Matrix<casacore::Double>& sumwt,
			    const casacore::Bool& dopsf,casacore::Bool /*useConjFreqCF*/=false)
    {DataToGridImpl_p(griddedData, vbs, sumwt,dopsf);}

    virtual void DataToGrid(casacore::Array<casacore::Complex>& griddedData, 
			    VBStore& vbs, 
    			    casacore::Matrix<casacore::Double>& sumwt,
			    const casacore::Bool& dopsf,casacore::Bool /*useConjFreqCF*/=false)
    {
      DataToGridImpl_p(griddedData, vbs, sumwt,dopsf);
    }

    //
    // This is the gather operation on griddedData for nelements > 1
    //
    void GatherGrids(casacore::Array<casacore::DComplex>& griddedData, casacore::Matrix<casacore::Double>& sumwt);
    void GatherGrids(casacore::Array<casacore::Complex>& griddedData, casacore::Matrix<casacore::Double>& sumwt);
    void initializePutBuffers(const casacore::Array<casacore::DComplex>& griddedData,
			      const casacore::Matrix<casacore::Double>& sumwt);
    void initializePutBuffers(const casacore::Array<casacore::Complex>& griddedData,
			      const casacore::Matrix<casacore::Double>& sumwt);
    void initializeDataBuffers(VBStore& /*vbs*/) {};

    //
    //------------------------------------------------------------------------------
    //
    // Re-sample VisBuffer to a regular grid (griddedData) (a.k.a. de-gridding)
    //
    virtual void GridToData(VBStore& vbs, const casacore::Array<casacore::Complex>& griddedData); 
    //    virtual void GridToData(VBStore& vbs, casacore::Array<casacore::Complex>& griddedData); 

    virtual void ComputeResiduals(VBStore& vbs);
    //
    //------------------------------------------------------------------------------
    //----------------------------Private parts-------------------------------------
    //------------------------------------------------------------------------------
    //
  private:
    template <class T>
    void DataToGridImpl_p(casacore::Array<T>& griddedData, VBStore& vb,  
			  casacore::Matrix<casacore::Double>& sumwt,const casacore::Bool& dopsf,casacore::Bool useConjFreqCF=false);
    // template <class T>
    // void GatherGrids_p(const casacore::Array<T>& griddedData, 
    // 		       const casacore::Matrix<casacore::Double>& sumwt);

    // template <class T>
    // void initializeBuffers(const casacore::Array<T>& griddedData,
    // 			   const casacore::Matrix<casacore::Double>& sumwt);

    void allocateBuffers(casacore::Bool newDataBuffers=true);
    void makeInfrastructureContainers();
    casacore::Double allocateDataBuffers();
    void startThreads();
    void scatter(casacore::Matrix<VBStore>& vbsStores,const VBStore& vbs);

    casacore::Int nelements_p;
    casacore::Bool doublePrecision_p;
    casacore::Vector<casacore::CountedPtr<VisibilityResamplerBase> > resamplers_p;
    casacore::Vector<casacore::CountedPtr<casacore::Array<casacore::DComplex> > > doubleGriddedData_p;
    casacore::Vector<casacore::CountedPtr<casacore::Array<casacore::Complex> > > singleGriddedData_p;
    casacore::Vector<casacore::CountedPtr<casacore::Matrix<casacore::Double> > > sumwt_p;
    casacore::Vector<casacore::CountedPtr<ResamplerWorklet> > gridderWorklets_p;
    //    casacore::Vector<VBStore> vbsVec_p;
    casacore::Matrix<VBStore> vbsVec_p;

    casacore::CountedPtr<ThreadCoordinator<casacore::Int> > threadClerk_p;
    casacore::Bool threadStarted_p;
    casa::utilj::DeltaThreadTimes tSetupG, tSendDataG, tWaitForWorkG, tOutsideG;
    casa::utilj::DeltaThreadTimes tSetupDG, tSendDataDG, tWaitForWorkDG, tOutsideDG;
    casa::utilj::ThreadTimes t4G_p,t4DG_p;
    //    async::Mutex *mutexForResamplers_p;
    casacore::CountedPtr<VisibilityResamplerBase> visResamplerCtor_p;
    casacore::Int whoLoadedVB_p;
    casacore::Int currentVBS_p;
 };
}; //# NAMESPACE CASA - END

#endif // 
