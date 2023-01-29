// -*- C++ -*-
//# AWVisResamplerHPG.h: Definition of the AWVisResamplerHPG class
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

#ifndef SYNTHESIS_TRANSFORM2_AWVISRESAMPLERHPG_H
#define SYNTHESIS_TRANSFORM2_AWVISRESAMPLERHPG_H

#include <synthesis/TransformMachines2/CFStore.h>
#include <synthesis/TransformMachines2/VBStore.h>
#include <synthesis/TransformMachines2/VisibilityResampler.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/casa/Arrays/Vector.h>

#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Logging/LogSink.h>
#include <casacore/casa/Logging/LogMessage.h>
#include <synthesis/TransformMachines2/AWVisResampler.h>
#include <synthesis/TransformMachines2/MyCFArray.h>
#include <hpg/hpg.hpp>
#include <hpg/hpg_indexing.hpp>
#include <tuple>
#define HPGNPOL 2
#include <chrono>

namespace casa { //# NAMESPACE CASA - BEGIN
  namespace refim{

  class AWVisResamplerHPG: public AWVisResampler
  {

  public:
    AWVisResamplerHPG(bool hpgInitAndFin=false): AWVisResampler(), hpgGridder_p(NULL), vis(), grid_cubes(), cf_cubes(), weights(), frequencies(),
						 cf_phase_screens(), hpgPhases(), visUVW(),
						 nVBS_p(0),  maxVBList_p(1), cachedVBSpw_p(-1),
						 hpgVBList_p(), HPGModelImageName_p(),hpgSoW_p(),HPGDevice_p(hpg::Device::Cuda),isHPGCustodian_p(hpgInitAndFin),
						 cfsi_p({1,false},{1,false},{1,true},{1,true}, 1), cfArray_p(),dcf_ptr_p(),rwdcf_ptr_p(),
						 mkHPGVB_startTime(), mkHPGVB_duration(), sizeofVisData_p(0)

    {
      hpgVBList_p.reserve(maxVBList_p);

      String hpgDevice="cuda";
      std::tie(hpgDevice, HPGDevice_p) = getHPGDevice();
      LogIO log_l(LogOrigin("AWVRHPG", "AWVRHPG()"));
      log_l << "Using HPG device " << hpgDevice << LogIO::POST;

      //      cached_PointingOffset_p.resize(2);cached_PointingOffset_p=-1000.0;runTimeG_p=runTimeDG_p=0.0;
      hpg::VisData<HPGNPOL> vd;
      sizeofVisData_p=0;
      for(auto x:vd.m_visibilities) sizeofVisData_p += sizeof(x);
      for(auto x:vd.m_weights) sizeofVisData_p += sizeof(x);
      for(auto x:vd.m_uvw) sizeofVisData_p += sizeof(x);
      for(auto x:vd.m_cf_phase_gradient) sizeofVisData_p += sizeof(x);
      for(auto x:vd.m_cf_index) sizeofVisData_p += sizeof(x);
	    
      sizeofVisData_p += sizeof(vd.m_frequency);
      sizeofVisData_p += sizeof(vd.m_phase);

    };

    virtual ~AWVisResamplerHPG()
    {
      //LogIO log_l(LogOrigin("AWVisResamplerHPG[R&D]","~AWVisResamplerHPG"));
      //log_l << "Gridding time: " << griddingTime << " sec" << LogIO::POST;
      cerr << "Cumulative time in makeHPGV: " << mkHPGVB_duration.count() << " sec" << endl;
    };

    std::tuple<String, hpg::Device> getHPGDevice();

    virtual String name(){return String("HPGResampler");};
    virtual Bool needCFPhaseScreen() {return false;};
    // virtual VisibilityResamplerBase* clone()
    // {
    //   return new AWVisResamplerHPG(*this);
    // }

    virtual void copyMaps(const AWVisResamplerHPG& other)
    {
      setCFMaps(other.cfMap_p, other.conjCFMap_p);
    }

    virtual void copy(const VisibilityResamplerBase& other)
    {
      VisibilityResampler::copy(other);
    }

    virtual void copy(const AWVisResamplerHPG& other)
    {
      VisibilityResampler::copy(other);
      SynthesisUtils::SETVEC(cached_phaseGrad_p, other.cached_phaseGrad_p);
      SynthesisUtils::SETVEC(cached_PointingOffset_p, other.cached_PointingOffset_p);
    }

    AWVisResamplerHPG& operator=(const AWVisResamplerHPG& other)
    {
      copy(other);
      SynthesisUtils::SETVEC(cached_phaseGrad_p, other.cached_phaseGrad_p);
      SynthesisUtils::SETVEC(cached_PointingOffset_p, other.cached_PointingOffset_p);
      hpgGridder_p = other.hpgGridder_p;
      cfsi_p = other.cfsi_p;
      cfArray_p = other.cfArray_p;
      //      dcf_ptr_p = other.dcf_ptr_p;
      return *this;
    }

    virtual void setCFMaps(const casacore::Vector<casacore::Int>& cfMap, const casacore::Vector<casacore::Int>& conjCFMap)
    {SETVEC(cfMap_p,cfMap);SETVEC(conjCFMap_p,conjCFMap);}


    virtual void DataToGrid(casacore::Array<casacore::DComplex>& griddedData, VBStore& vbs, casacore::Matrix<casacore::Double>& sumwt,
    			    const casacore::Bool& dopsf,casacore::Bool useConjFreqCF=false)
    {DataToGridImpl_p(griddedData, vbs, sumwt,dopsf,useConjFreqCF);}

    virtual void DataToGrid(casacore::Array<casacore::Complex>& griddedData, VBStore& vbs, casacore::Matrix<casacore::Double>& sumwt,
			    const casacore::Bool& dopsf,casacore::Bool useConjFreqCF=false)
    {DataToGridImpl_p(griddedData, vbs, sumwt,dopsf,useConjFreqCF);}

    //    virtual void setModelImage(const std::unique_ptr<hpg::GridValueArray> HPGModelImage) {HPGModelImage_p = HPGModelImage;}
    virtual void setModelImage(const std::string& HPGModelImageName) {HPGModelImageName_p = HPGModelImageName;}

    //    virtual void sendModelImage(const std::unique_ptr<hpg::GridValueArray>& HPGModelImage)
    virtual bool sendModelImage(const std::string& HPGModelImageName,
				const std::array<unsigned, 4>& gridSize);

    virtual void saveGriddedData(const std::string& name,const casacore::CoordinateSystem& csys);
    //
    //---------------------------------------------------------------------------------------------------------------------
    // These specialized methods reterive the products from the external Device and do the necessary finalization for it.
    //
    virtual void finalizeToSky(casacore::Array<casacore::DComplex>& griddedData, casacore::Matrix<casacore::Double>& sumwt);
    virtual void GatherGrids(casacore::Array<casacore::DComplex>& griddedData, casacore::Matrix<casacore::Double>& sumwt);

    virtual void finalizeToSky(casacore::Array<casacore::Complex>& griddedData, casacore::Matrix<casacore::Double>& sumwt)
    {
      LogIO log_l(LogOrigin("AWVisResamplerHPG[R&D]","finalizeToSky(Complex)"));
      log_l << "Single precision AWVisResamplerHPG::finalizeToSky() is not yet supported for HPG" << LogIO::POST << LogIO::EXCEPTION;
      (void)griddedData;(void)sumwt;
    };
    //---------------------------------------------------------------------------------------------------------------------

    std::tuple<hpg::opt_t<hpg::Error>, hpg::CFSimpleIndexer>
    loadCF(VBStore& vbs, Int& nGridPol, Int& nDataPol,
	   Vector<Int>& wNdxList, Vector<Int>& spwNdxList,
	   bool send_to_device=false);

    virtual hpg::CFSimpleIndexer setCFSI(const hpg::CFSimpleIndexer cfsi) {cfsi_p = cfsi; return cfsi_p;}
    virtual bool set_cf(casa::refim::MyCFArray& cfArray)// Can't figure out how to create hpg::opt_t<hpg::Error> to return!
    {
      cfArray_p = cfArray;

      // auto err=hpgGridder_p->set_convolution_function(hpg::Device::OpenMP, std::move(cfArray));
      // if (err)
      // 	{
      // 	  throw(AipsError("Error in AWVRHPG::set_cf()"));
      // 	}

      return true;
    }  
    virtual bool set_cf(std::shared_ptr<hpg::DeviceCFArray>& dcfArray)// Can't figure out how to create hpg::opt_t<hpg::Error> to return!
    {
      dcf_ptr_p = dcfArray;
      return (dcf_ptr_p!=NULL);
    }
    virtual bool set_cf(std::shared_ptr<hpg::RWDeviceCFArray>& rwdcfArray)// Can't figure out how to create hpg::opt_t<hpg::Error> to return!
    {
      rwdcf_ptr_p = rwdcfArray;
      return (rwdcf_ptr_p!=NULL);
    }
    virtual std::shared_ptr<std::complex<double>> getGridPtr(size_t& size) const override;
    virtual std::shared_ptr<double> getSumWeightsPtr(size_t& size) const override;
    bool createHPG(const int& nx, const int& ny, const int& nGridPol, const int& nGridChan,
		   const int& nVBAntenna, const int& nVBChannels,
		   const PolMapType& mVals,  const PolMapType& mNdx,
		   const PolMapType& conjMVals, const PolMapType& conjMNdx);
    //
    //------------------------------------------------------------------------------
    //
  protected:
    hpg::Gridder* hpgGridder_p;
    template <class T>
    void DataToGridImpl_p(casacore::Array<T>& griddedData, VBStore& vb,
			  casacore::Matrix<casacore::Double>& sumwt,const casacore::Bool& /*dopsf*/,
			  casacore::Bool /*useConjFreqCF*/);
    //
    // Retrieve the gridded data from the device.  The grid from the
    // device is in DP and is converted OTF to the target type.
    //
    template <class T>
    void getGriddedData(casacore::Array<T>& griddedData);
    void GridToData(VBStore& vbs,const casacore::Array<casacore::Complex>& griddedData);
    //    casacore::Cube<casacore::Complex>& GridToData_p(VBStore& vbs,const casacore::Array<casacore::Complex>& griddedData); 

    std::tuple<bool, CountedPtr<CFBuffer>>
    initializeHPG(VBStore& vbs, const IPosition& gridShape);

    std::vector<std::complex<hpg::visibility_fp>> vis;
    std::vector<unsigned> grid_cubes, cf_cubes;
    std::vector<hpg::vis_weight_fp> weights;
    std::vector<hpg::vis_frequency_fp> frequencies;
    std::vector<hpg::cf_phase_gradient_t> cf_phase_screens;
    std::vector<hpg::vis_phase_fp> hpgPhases;
    std::vector<hpg::vis_uvw_t> visUVW;
    uInt nVBS_p, maxVBList_p;
    Int cachedVBSpw_p;
    std::vector < std::vector<hpg::VisData<HPGNPOL> > > hpgVBList_p;
    std::string HPGModelImageName_p;
    sumofweight_fp hpgSoW_p;
    hpg::Device HPGDevice_p;
    bool isHPGCustodian_p;
    hpg::CFSimpleIndexer cfsi_p;
    casa::refim::MyCFArray cfArray_p;
    std::shared_ptr<hpg::DeviceCFArray> dcf_ptr_p;
    std::shared_ptr<hpg::RWDeviceCFArray> rwdcf_ptr_p;
    std::chrono::time_point<std::chrono::steady_clock> mkHPGVB_startTime;
    std::chrono::duration<double> mkHPGVB_duration;

    unsigned int sizeofVisData_p;
  };
  }; //# NAMESPACE CASA - END
};
#endif //
