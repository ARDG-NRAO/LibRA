// -*- C++ -*-
//# AWVisResamplerHPG.cc: Implementation of the AWVisResamplerHPG class
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

#include <synthesis/TransformMachines/SynthesisError.h>
#include <synthesis/TransformMachines2/AWVisResamplerHPG.h>
#include <synthesis/TransformMachines2/Utils.h>
#include <synthesis/TransformMachines/SynthesisMath.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/casa/OS/Timer.h>
#include <fstream>
#include <iostream>
#include <typeinfo>
#include <iomanip>
#include <map>
#include <synthesis/TransformMachines2/FortranizedLoops.h>
//#include <synthesis/TransformMachines2/hpg.hpp>
#include <hpg/hpg.hpp>
#include <hpg/hpg_indexing.hpp>
#include <type_traits>
#include <synthesis/TransformMachines2/HPGVisBuffer.inc>
#include <thread>

#ifdef _OPENMP
#include <omp.h>
#endif

//#include <casa/BasicMath/Functors.h>
using namespace casacore;
using namespace hpg;
namespace casa{
  using namespace refim;
  
  //
  // Global functions
  // --------------------------------------------------------------------------------------
  template <unsigned N>
  void makeMuellerIndexes(const PolMapType& mNdx,
			  const PolMapType& mValues,
			  const unsigned& nGridPol,
			  std::vector<std::array<int, N>>& muellerIndexes)
  {
    PolMapType tt;
    
    // for(unsigned i=0;i<mNdx.size();i++)
    //   {
    // 	cerr << mNdx[i] << endl;
    //   }
    // cerr << endl;
    
    for(unsigned i=0,k=0;i<mNdx.size();i++)
      {
	if (mNdx[i].size()>0)
	  {
	    tt.resize(k+1,true);
	    tt[k].resize(mNdx[i].size());
	    tt[k]=mNdx[i];
	    k++;
	  }
      }
    
    muellerIndexes.resize(nGridPol);
    for(unsigned i=0;i<muellerIndexes.size();i++)
      for(unsigned j=0;j<muellerIndexes.size();j++)
	muellerIndexes[i][j]=-1;
    
    for(unsigned i=0;i<mValues.size();i++)
      {
	//	cerr << mValues[i] << " " << tt[i] << endl;
	for(unsigned j=0;j<mValues[i].size();j++)
	  {
	    muellerIndexes[j][mValues[i][j]%N]=tt[i][j];
	    //muellerIndexes[j][mValues[i][j]%N]=1;
	  }
      }
    
    // for(unsigned ir=0;ir<muellerIndexes.size();ir++)
    //   {
    // 	for(unsigned ic=0;ic<muellerIndexes[ir].size();ic++)
    // 	    cerr << muellerIndexes[ir][ic] << " ";
    // 	cerr << endl;
    //   }
  }
  //
  //-------------------------------------------------------------------------
  //  
  // This is a global method, used in AWVRHPG::DataToGrid_impl().
  template <unsigned N>
  hpg::Gridder* initGridder2(const hpg::Device HPGDevice_l,
			     const uInt& NProcs,
			     const hpg::CFArray* cfArray_ptr,
			     const std::array<unsigned, 4>& grid_size,
			     const std::array<double, 2>& grid_scale,
			     const PolMapType& mNdx,
			     const PolMapType& mVals,
			     const PolMapType& conjMNdx,
			     const PolMapType& conjMVals,
			     const int& nAntenna,
			     const int& nChannel,
			     const unsigned& nVBsPerBucket
			     // std::vector<std::array<int, N> > mueller_indexes,
			     // std::vector<std::array<int, N> > conjugate_mueller_indexes
			     )
  {
    //
    // FYI:  gridSize{(uInt)nx,(uInt)ny,(uInt)nGridPol,(uInt)nGridChan};
    //
    LogIO log_l(LogOrigin("AWVRHPG", "initGridder2[R&D]"));
    
    log_l << "grid_size: "
	  << grid_size[0] << " " << grid_size[1] << " "
	  << grid_size[2] << " " << grid_size[3] << " " << LogIO::POST;
    log_l << "Per VB:  nAnt: " << nAntenna << ", nChannel: " << nChannel << LogIO::POST;
    
    if (!hpg::is_initialized()) hpg::initialize();
    
    
    {
      std::vector<std::array<int, N> > mueller_indexes,conjugate_mueller_indexes;
      
      makeMuellerIndexes<N>(mNdx, mVals, grid_size[2]/*nGridPol*/, mueller_indexes);
      makeMuellerIndexes<N>(conjMNdx, conjMVals, grid_size[2]/*nGridPol*/, conjugate_mueller_indexes);
      
      
      //size_t max_visibilities_batch_size = 351*2*1000;

      unsigned nRows=(nAntenna*(nAntenna-1)/2)*2*nChannel;
      size_t max_visibilities_batch_size = nRows*nVBsPerBucket;

      cerr << "Mueller indexes: initgridder2: " << endl;
      cerr << "M: " << endl;
      for(unsigned ir=0;ir<mueller_indexes.size();ir++)
	{
	  for(unsigned ic=0;ic<mueller_indexes[ir].size();ic++)
	    {	
	      //		  mueller_indexes[ir][ic] = 1;
	      cerr << mueller_indexes[ir][ic] << " ";
	    }
	  cerr << endl;
	}
      cerr << "M*: " << endl;
      for(unsigned ir=0;ir<conjugate_mueller_indexes.size();ir++)
	{
	  for(unsigned ic=0;ic<conjugate_mueller_indexes[ir].size();ic++)
	    {
	      //		  conjugate_mueller_indexes[ir][ic] = 0;
	      cerr << conjugate_mueller_indexes[ir][ic] << " ";
	    }
	  cerr << endl;
	}
      
      // 	  mueller_indexes.resize(N);  conjugate_mueller_indexes.resize(N);
      
      //	  cerr << "Mueller index shape: " << mueller_indexes.size() << "X" << HPGNPOL << endl;
      //	  if (hpgDevice=="serial") Device=hpg::Device::Serial;
      

      // !!!!!!!!!! CHANGE OF TYPE FROM COMES-IN TO WHAT THE HPG CODE IN rcgrid BRANCH WANTS for Gridder::create<N> !!!!!!!!!!!!!!
      std::array<int, 4> grid_size_int;
      grid_size_int[0]=grid_size[0];
      grid_size_int[1]=grid_size[1];
      grid_size_int[2]=grid_size[2];
      grid_size_int[3]=grid_size[3];
      
	
      hpg::rval_t<Gridder> g = Gridder::create<N>(HPGDevice_l, NProcs, max_visibilities_batch_size,
						  cfArray_ptr, grid_size_int, grid_scale, mueller_indexes,
						  conjugate_mueller_indexes);
      if (!hpg::is_value(g))
	{
	  log_l << "Error in hpg::create<N>(): Current setting of NPROCS (="<< NProcs << ") env. variable will require "
		<< NProcs+1 << "x the value in the following message." << endl
		<< hpg::get_error(g).message()
		<< LogIO::EXCEPTION;
	  throw(std::runtime_error(string("Error in hpg::create<N>()")));
	}

      //hpgGridder_p = new hpg::Gridder(hpg::get_value(std::move(g)));
      
      return new hpg::Gridder(hpg::get_value(std::move(g)));
    }
  };
  //
  // END -- Global functions
  // --------------------------------------------------------------------------------------
  
  template
  void AWVisResamplerHPG::DataToGridImpl_p(Array<DComplex>& grid, VBStore& vbs, 
					   Matrix<Double>& sumwt,const Bool& dopsf,
					   Bool useConjFreqCF); // __restrict__;
  template
  void AWVisResamplerHPG::DataToGridImpl_p(Array<Complex>& grid, VBStore& vbs, 
					   Matrix<Double>& sumwt,const Bool& dopsf,
					   Bool useConjFreqCF); // __restrict__;
  
  //
  //-------------------------------------------------------------------------
  //  
  std::tuple<String, hpg::Device> AWVisResamplerHPG::getHPGDevice()
  {
    String hpgDevice="cuda";
    hpg::Device Device=hpg::Device::Cuda;

    hpgDevice=refim::SynthesisUtils::getenv("HPGDEVICE",hpgDevice);
    if      (hpgDevice=="cuda")    Device=hpg::Device::Cuda;
    else if (hpgDevice=="serial")  Device=hpg::Device::Serial;
    else
      throw(
	    SynthesisFTMachineError("HPGDEVICE env. var. setting incorrect: " +
				    hpgDevice)
	    );

    return std::make_tuple(hpgDevice,Device);
  }
  //
  // AWProjectFT::finalizetoSky() calls AWVRHPG::finzlieToSky().
  // Here, we apply the FFT on the device to the grid.  GatherGrids()
  // then copies the image from the device to the CPU memory.
  //
  //
  //-------------------------------------------------------------------------
  //  
  void AWVisResamplerHPG::finalizeToSky(casacore::Array<casacore::DComplex>& griddedData,
					casacore::Matrix<casacore::Double>& sumwt)
  {
    LogIO log_l(LogOrigin("AWVisResamplerHPG[R&D]","finalizeToSky(DCompelx)"));
    log_l << "Finalizing gridding..." << LogIO::POST;
    if (hpgVBBucket_p.counter() != 0)
      {
	log_l << "Sending the last of the " << hpgVBBucket_p.counter() << " data points" << LogIO::POST;
	griddingTime += sendData(hpgVBBucket_p, nVisGridded_p, nDataBytes_p,
				 sizeofVisData_p, (HPGModelImageName_p != ""));
	hpgGridder_p->fence();
      }
    
    hpgGridder_p->shift_grid(ShiftDirection::FORWARD);
    hpgGridder_p->apply_grid_fft();
    hpgGridder_p->shift_grid(ShiftDirection::BACKWARD);
    
    GatherGrids(griddedData,sumwt);
    
    if (isHPGCustodian_p) hpg::finalize();
    
    log_l << "...done finalizing gridding" << LogIO::POST;
  } 
  //
  //-------------------------------------------------------------------------
  // Save the gridded data to the named image on the disk
  //
  void AWVisResamplerHPG::saveGriddedData(const std::string& name,
					  const casacore::CoordinateSystem& csys)
  {
    LogIO log_l(LogOrigin("AWVisResamplerHPG[R&D]","saveGriddedData"));
    log_l << "Saving complex grid to the disk" << LogIO::POST;
    //casacore::Array<casacore::DComplex> griddedData;    
    //
    // Save a SP versin of the grid since casaviewer (and perhaps
    // CASA's imagetool) can't handle DComplex images.  However for
    // numerical precision, FT of the complex grid must be done in DP.
    //
    casacore::Array<casacore::Complex> griddedData;    
    getGriddedData(griddedData);

    refim::storeArrayAsImage(name, csys, griddedData);
  }
  //
  //-------------------------------------------------------------------------
  // Retrieve the gridded data from the device.  The grid from the
  // device is in DP and is converted OTF to the target type.
  //
  template <class T>
  void AWVisResamplerHPG::getGriddedData(casacore::Array<T>& griddedData)
  {
    auto gg=hpgGridder_p->grid_values();
    IPosition shp = griddedData.shape(),ndx(4);
    for (size_t i = 0; i < 4; ++i)
      ndx[i]=gg->extent(i);
    griddedData.resize(ndx);
    shp = griddedData.shape();
    Bool dummy;
    T *stor = griddedData.getStorage(dummy);
    DComplex val;
    unsigned long k=0;

    // To TEST: All the following nested loops could be replaced with
    // gg->copy_to(hpg::Device::OpenMP, stor);
    for(ndx[3]=0;  ndx[3]<shp[3];  ndx[3]++)
      for(ndx[2]=0;  ndx[2]<shp[2];  ndx[2]++)
	for(ndx[1]=0;  ndx[1]<shp[1];  ndx[1]++)
	  for(ndx[0]=0;  ndx[0]<shp[0];  ndx[0]++)
	    {
	      val = (*gg)(ndx[0],ndx[1],ndx[2],ndx[3]);
	      stor[k++] = (T)val;
	    }
  }
  //
  //-------------------------------------------------------------------------
  // Gather the grid and sum-of-weights from the Device (external to
  // the memory address space), and shut the device.
  //
  void AWVisResamplerHPG::GatherGrids(casacore::Array<casacore::DComplex>& griddedData,
				      casacore::Matrix<casacore::Double>& sumwt) 
  {
    LogIO log_l(LogOrigin("AWVisResamplerHPG[R&D]","GatherGrids(DCompelx)"));
    {
      std::unique_ptr<hpg::GridWeightArray> wts=hpgGridder_p->grid_weights();
      // log_l << "Got the grid from the device " << griddedData.shape() << LogIO::POST;
      // cerr << std::setprecision(20) << "Weights shape "
      // 	   << wts->extent(0) << " x " << wts->extent(1) << endl;
      
      hpgSoW_p.resize(wts->extent(0));
      for (unsigned i=0;i<hpgSoW_p.size();i++)
	hpgSoW_p[i].resize(wts->extent(1));
      
      for (unsigned i=0;i<hpgSoW_p.size();i++)
      	{
      	  for (unsigned j=0;j<hpgSoW_p[i].size();j++)
      	    {
      	      hpgSoW_p[i][j]=(*wts)(i,j);
	      //      	      cerr << (*wts)(i,j) << " ";
      	    }
	  //      	  cerr << endl;
      	}
      // cerr << "Shapes: hpgSoW_p, sumwt: " << sumwt << endl;

      // hpgSoW_p is of type sumofweight_fp (vector<vector<double>>).  sumWeight is a Matrix.
      // sow[i][*] is a Mueller row. i is the index for the polarization product.
      // wt below is a sum of the weights of all the Mueller elements in a row.
      if (hpgSoW_p.size() > 0)
	{
	  for (int i=0; i<sumwt.shape()(0); i++)
	    {
	      double wt=0.0;
	      for (unsigned j=0; j<hpgSoW_p[i].size(); j++)
		wt += hpgSoW_p[i][j];
	      for (unsigned j=0; j<sumwt.shape()(1); j++)
		{
		  sumwt(i,j)=wt;
		}
	    }
	}
      
      // auto gg=hpgGridder_p->grid_values();
      // // for (size_t i = 0; i < 4; ++i)
      // //   cerr << gg->extent(i) << " ";
      // IPosition shp = griddedData.shape(),ndx(4);
      // Bool dummy;
      // casacore::DComplex *stor = griddedData.getStorage(dummy);
      // DComplex val, max=casacore::DComplex(0.0);
      
      // // To TEST: All the following nested loops could be replaced with
      // // gg->copy_to(hpg::Device::OpenMP, stor);
      // unsigned long k=0;
      // for(ndx[3]=0;  ndx[3]<shp[3];  ndx[3]++)
      // 	for(ndx[2]=0;  ndx[2]<shp[2];  ndx[2]++)
      // 	  for(ndx[1]=0;  ndx[1]<shp[1];  ndx[1]++)
      // 	    for(ndx[0]=0;  ndx[0]<shp[0];  ndx[0]++)
      // 	      {
      // 		val = (*gg)(ndx[0],ndx[1],ndx[2],ndx[3]);
      // 		stor[k++] = val;
      // 	      }

      getGriddedData(griddedData);
      log_l << "Copied grid to CPU buffer "<< LogIO::POST;
    }
  };
  //
  //-------------------------------------------------------------------------
  //  
  bool AWVisResamplerHPG::sendModelImage(const std::string& HPGModelImageName,
					 const std::array<unsigned, 4>& gridSize)
  {
    LogIO log_l(LogOrigin("AWVisResamplerHPG[R&D]","sendModelImage"));
    
    if (HPGModelImageName == "")
      {
	//log_l << "Name for model image is blank!  No model image sent to the device" << LogIO::WARN << LogIO::POST;
	return false;
      }
    
    // Open the CASA image and transfer it to hpg::GridValueArray in
    // a code-block so that the CASA image storage is released at the end
    // of the block.
    casacore::CoordinateSystem csys;
    
    log_l << "Loading model image " << "\"" << HPGModelImageName << "\"" <<  LogIO::POST;
    casacore::TempImage<casacore::DComplex> modelImageGrid; // This will be used for in-place FFT on the GPU for prediction
    csys=SynthesisUtils::makeModelGridFromImage(HPGModelImageName, modelImageGrid);
    
    // Stop if the model image sname is not the same as the grid shape
    {
      for (auto i : {0,1,2,3})
	if (modelImageGrid.shape()[i] != gridSize[i])
	  {
	    log_l << "Model image shape: " << modelImageGrid.shape() << ". "
		  << "Grid shape: [" << gridSize[0] << "," << gridSize[1] << ","
		  << gridSize[2] << "," << gridSize[3] << "]"
		  << LogIO::WARN;
	    log_l << "Model image is of incorrect shape "
		  << LogIO::EXCEPTION << LogIO::POST;
	  }
    }
    Bool dummy;
    assert(hpg::GridValueArray::rank == modelImageGrid.shape().nelements());
    // std::array<unsigned,hpg::GridValueArray::rank> extents={(unsigned)modelImageGrid.shape()[0],
    // 							    (unsigned)modelImageGrid.shape()[1],
    // 							    (unsigned)modelImageGrid.shape()[2],
    // 							    (unsigned)modelImageGrid.shape()[3]};
    std::array<int,hpg::GridValueArray::rank> extents={(int)modelImageGrid.shape()[0],
						       (int)modelImageGrid.shape()[1],
						       (int)modelImageGrid.shape()[2],
						       (int)modelImageGrid.shape()[3]};
    casacore::Array<casacore::DComplex> tarr=modelImageGrid.get();
    casacore::DComplex *tstor = tarr.getStorage(dummy);
    
    std::unique_ptr<hpg::GridValueArray> HPGModelImage;
    
    HPGModelImage = hpg::GridValueArray::copy_from("whatever",
						   HPGDevice_p, // target_device
						   //hpg::Device::Cuda, //target_device
						   hpg::Device::OpenMP,//Device host_device,
						   (hpg::GridValueArray::value_type*)tstor,
						   extents);
    
    
    unsigned shape[hpg::GridValueArray::rank];
    
    log_l << "Sending model image of shape ";
    for (unsigned i=0;i<hpg::GridValueArray::rank;i++)
      {
	shape[i]=HPGModelImage->extent(i);
	log_l << shape[i] << " ";
      }
    log_l << LogIO::POST;
    
    hpg::opt_t<hpg::Error>
      err = hpgGridder_p->set_model(hpg::Device::OpenMP,std::move(*HPGModelImage));
    //    bool success=(err == nullptr);
    if (err)
      log_l << "Failed hpg::Gridder::set_model(). Error type: "
	    << static_cast<int>(err->type()) << LogIO::SEVERE;
    else
      log_l << "Finished sending image to the device" << LogIO::POST;
    
    
    grid_scale_fp norm = 1.0;//((grid_value_fp)(shape[0]*shape[1]));
    log_l << "Applying (in-place) FFT on the model image.  Norm = " << norm << LogIO::POST;
    
    // Below is the following sequence of operations on the GPU: shift -> FFT -> shift.
    // The second shift is equivalent of applying a phase shift corresponding to N/2 pixels. I think...
    
    hpgGridder_p->shift_model(ShiftDirection::FORWARD);          // <============= shift_model()
    
    err = hpgGridder_p->apply_model_fft(norm,FFTSign::NEGATIVE); // <============= apply_model_fft()
    if (err) log_l << "Gridder::apply_model_fft() failed.  Error type: "
		   << static_cast<int>(err->type()) << LogIO::SEVERE;
    hpgGridder_p->shift_model(ShiftDirection::BACKWARD);         // <============= shift_model()
    
    log_l << "...finished FFT." << LogIO::POST;
    
    return bool(!err);
  }
  //
  //-----------------------------------------------------------------------------------
  // Send CFs to the device
  //
  void sendCF(hpg::Gridder* hpgGridder,
	    std::shared_ptr<hpg::RWDeviceCFArray>& rwdcf_ptr)
  {
    LogIO log_l(LogOrigin("AWVisResamplerHPG[R&D]","sendCF"));

    log_l << "Sending CFs..." << LogIO::POST;
    //    cerr << "############# " << rwdcf_ptr << endl;
    //    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    auto err=hpgGridder->set_convolution_function(hpg::Device::OpenMP, std::move(*rwdcf_ptr));
    log_l << "...done" << LogIO::POST;
    if (err)
      {
	log_l << "HPGError in calling set_convolution_function(): " << LogIO::EXCEPTION;
      }
    //	    dcf_ptr_p.reset();
    rwdcf_ptr.reset();
  }
  //
  //-----------------------------------------------------------------------------------
  // Create an instance of the HPG.  If a new HPG was created, if
  // requested, load the model image as well.
  //
  bool AWVisResamplerHPG::createHPG(const int& nx, const int& ny, const int& nGridPol, const int& nGridChan,
				    const int& nVBAntenna, const int& nVBChannels,
				    const PolMapType& mVals,  const PolMapType& mNdx,
				    const PolMapType& conjMVals, const PolMapType& conjMNdx)
  {
    // The grid is always a 4D array: NX x NY x NPol x NChan
    const std::array<unsigned, 4> gridSize{(uInt)nx,(uInt)ny,(uInt)nGridPol,(uInt)nGridChan};
    const std::array<double, 2> gridScale{uvwScale_p(0), uvwScale_p(1)};
    // std::vector<std::array<int, HPGNPOL> > mueller_indexes,conjugate_mueller_indexes;
    // makeMuellerIndexes<HPGNPOL>(mNdx, mVals, (uInt)nGridPol, mueller_indexes);
    // makeMuellerIndexes<HPGNPOL>(conjMNdx, conjMVals, (uInt)nGridPol, conjugate_mueller_indexes);
    LogIO log_l(LogOrigin("AWVisResamplerHPG[R&D]","createHPG"));

    int nProcs=2;

    nProcs=refim::SynthesisUtils::getenv("NPROCS",nProcs);
    log_l << "NPROCS: " << nProcs << endl;

    //    hpgGridder_p = initGridder2<HPGNPOL>(HPGDevice_p,nProcs,&cfArray_p, gridSize, gridScale,
    hpgGridder_p = initGridder2<HPGNPOL>(HPGDevice_p,nProcs,rwdcf_ptr_p.get(), gridSize, gridScale,
					 mNdx,mVals,conjMNdx,conjMVals,
					 nVBAntenna, nVBChannels,nVBsPerBucket_p);

    
    unsigned nRows=(nVBAntenna*(nVBAntenna-1)/2);
    log_l << "Resizing HPGVB Bucket: " << HPGNPOL << " " << nVBChannels << " " << nRows
	  << " x " << nVBsPerBucket_p << endl;

    hpgVBBucket_p.resize(HPGNPOL, nVBChannels, nRows*nVBsPerBucket_p);

    cerr << "Gridder initialized..." << endl;
    bool do_degrid;
    do_degrid = sendModelImage(HPGModelImageName_p,gridSize);
    if (do_degrid)
      {
	log_l << "Running degrid_grid GPU kernel" << LogIO::POST;
      }
    return do_degrid;
  }
  //
  //-----------------------------------------------------------------------------------
  // Template implementation for DataToGrid
  //
  std::tuple<bool, CountedPtr<CFBuffer>>
  AWVisResamplerHPG::initializeHPG(VBStore&vbs, const IPosition& gridShape)
  {
    //
    // Load the VB in the vbsList_p.  This list is emptied, once it is
    // full, in the code at the end of this block.
    //
    int nx = gridShape[0];
    int ny = gridShape[1];
    int nGridPol = gridShape[2];
    int nGridChan = gridShape[3];
    //    int nDataPol  = vbs.flagCube_p.shape()[0];

    int vbSpw = (vbs.vb_p)->spectralWindows()(0);
    int nVBAntenna = vbs.vb_p->nAntennas();
    int nVBChannels = vbs.vb_p->nChannels();

    Vector<Double> wVals, fVals; PolMapType mVals, mNdx, conjMVals, conjMNdx;
    Double fIncr, wIncr;
    CountedPtr<CFBuffer> cfb = (*vb2CFBMap_p)[0];
    //
    // This loads the all-importnat conjMNDx and mNdx maps
    //
    cfb->getCoordList(fVals,wVals,mNdx, mVals, conjMNdx, conjMVals, fIncr, wIncr);
    //    runTimeG1_p += timer_p.real();

    // uint nVisPol=0;// nRows=rend-rbeg+1, nVisChan=endChan - startChan+1;
    // for(int ipol=0; ipol< nDataPol; ipol++)
    //   {
    // 	int targetIMPol=polMap_p(ipol);
    // 	if ((targetIMPol>=0) && (targetIMPol<nGridPol))
    // 	  nVisPol++;
    //   }
    //
    // Page-in the new CFs and load them on the device upon SPW change.
    //
    //hpg::CFSimpleIndexer cfsi({1,false},{1,false},{1,true},{1,true}, 1);

    // Reload CFs if the SPW changed and the setup is for
    // AW-Projection (wbAWP=T & nW > 1), or if this is the first pass
    // (hpgGridder_p==NULL)
    bool reloadCFs = SynthesisUtils::needNewCF(cachedVBSpw_p, vbSpw, vbs.nWPlanes_p, vbs.wbAWP_p,
					       hpgGridder_p==NULL);
    bool do_degrid=(HPGModelImageName_p != "");

    // If this is the first pass, create the HPG object.
    if (hpgGridder_p==NULL) do_degrid = createHPG(nx,ny,nGridPol, nGridChan, nVBAntenna, nVBChannels,
						  mVals, mNdx, conjMVals, conjMNdx);

    if (reloadCFs)
      sendCF(hpgGridder_p, rwdcf_ptr_p);

    return std::make_tuple(do_degrid,cfb);
  }
  //
  //-----------------------------------------------------------------------------------
  // Method to send the data to HPG and trigger degridding/gridding
  // computations.
  //
  double AWVisResamplerHPG::sendData(HPGVisBufferBucket<HPGNPOL>& VBBucket,
				     double& nVisGridded, long unsigned int& nDataBytes,
				     //const uint& hpgVBNRows,
				     const uint& sizeofVisData,
				     const bool& do_degrid)
  {
    //if (VBBucket.isFull() && (hpgVBNRows>0))
    //if (VBBucket.isFull() && (VBBucket.counter()>0))

    //
    // If the VBBucket is full, shrink() it remove any unfilled space
    // in the bucket, send the rest of the buffer to the gridder, and
    // reset the bucket (reset()).  std::move() empties the storage of
    // its argument.  The emptying of the buffer may (?)  happen with
    // the std::move(VBBucket.hpgVB_p) anyway.
    //
    if (VBBucket.counter()>0)
      {
	// Shrink the internal storage of the bucket to be length
	// it is filled (so that unfilled content does not reach
	// the gridder).
	VBBucket.shrink();


	timer_p.mark();
	unsigned nHPGVBRows = VBBucket.counter();

	//	cerr << nHPGVBRows << " " << VBBucket.totalUnits() << " " << VBBucket.size() << endl;
	nVisGridded += nHPGVBRows*HPGNPOL;
	nDataBytes += sizeofVisData*nHPGVBRows;

	hpg::opt_t<hpg::Error> err;

	if (do_degrid)
	  err = hpgGridder_p->degrid_grid_visibilities(
						       hpg::Device::OpenMP,
						       //std::move(hpgVBList_p[i])
						       std::move(VBBucket.hpgVB_p)
						       );
	else
	  err = hpgGridder_p->grid_visibilities(
						hpg::Device::OpenMP,
						//std::move(hpgVBList_p[i])
						std::move(VBBucket.hpgVB_p)
						);
	assert(VBBucket.size()==0);
	VBBucket.reset();
	    
	if (err)
	  {
	    LogIO log_l(LogOrigin("AWVisResamplerHPG[R&D]","sendData"));
	    log_l << "Failed hpg::" << ((do_degrid)?"degrid_grid_visibilities()":"grid_visibilities()")
		  << " Error type: " << static_cast<int>(err->type()) << LogIO::SEVERE;
	  }
      }
    return timer_p.real();
  }
  //
  //-----------------------------------------------------------------------------------
  // Template implementation for DataToGrid.  Each call of this
  // transfers the VB in the VBS to the VBBucket.  Once full, the
  // bucket is sent to the HPG via sendData() which empties the
  // bucket, triggers the degridding/gridding computations and blocks
  // till HPG call for it returns.
  //
  template <class T>
  void AWVisResamplerHPG::DataToGridImpl_p(Array<T>& grid,  VBStore& vbs, 
					   Matrix<Double>& sumwt,const Bool& /*dopsf*/,
					   Bool /*useConjFreqCF*/)
  {
    int rend = vbs.endRow_p;
    if(rend==0) return;
    int rbeg = vbs.beginRow_p;
    
    auto ret = initializeHPG(vbs, grid.shape());
    bool do_degrid = std::get<0>(ret);
    CountedPtr<CFBuffer> cfb = std::get<1>(ret);
    casacore::Vector<casacore::Vector<casacore::Double> > pointingOffsets = cfb->getPointingOffset();

    int nGridPol = grid.shape()[2];
    int nGridChan = grid.shape()[3];
    
    int nDataPol  = vbs.flagCube_p.shape()[0];
    int nDataChan = vbs.flagCube_p.shape()[1];
    int startChan = 0;
    int endChan = nDataChan;
    uInt nVisPol=0;// nRows=rend-rbeg+1, nVisChan=endChan - startChan+1;
    for(Int ipol=0; ipol< nDataPol; ipol++)
      {
    	int targetIMPol=polMap_p(ipol);
    	if ((targetIMPol>=0) && (targetIMPol<nGridPol)) 
    	  nVisPol++;
      }
    
    //cerr << "Pointing offsets: " << pointingOffsets << " " << rbeg << " " << rend << " " << startChan << " " << endChan << " " << max(vbs.vb_p->visCube()) << endl;
    std::chrono::time_point<std::chrono::steady_clock> 
      mkHPGVB_startTime=std::chrono::steady_clock::now();
    

    Vector<Double> wVals, fVals; PolMapType mVals, mNdx, conjMVals, conjMNdx;
    Double fIncr, wIncr;
    // This loads the all-importnat conjMNDx and mNdx maps
    //
    cfb->getCoordList(fVals,wVals,mNdx, mVals, conjMNdx, conjMVals, fIncr, wIncr);

    //    std::vector<hpg::VisData<HPGNPOL> > hpgVB;
    // = makeHPGVisBuffer<HPGNPOL>(sumwt,
    //    unsigned int hpgVBNRows =
      makeHPGVisBuffer<HPGNPOL>(//hpgVB_p,
				hpgVBBucket_p,
				sumwt,
				vbs,
				vb2CFBMap_p,
				nGridPol, nGridChan, nVisPol,
				rbeg, rend, startChan, endChan,
				chanMap_p, polMap_p,
				conjMNdx, conjMVals,
				dphase_p,pointingOffsets,
				cfsi_p);
				
    //    cerr << "MkHPGVB: " << rbeg << " " << rend << " " << hpgVBNRows << " " << hpgVBBucket_p.filledUnits() << endl;
    // Resize so that there are no unused VisData in the hpgVB at the end.
    if (!hpgVBBucket_p.isFull())
      {
	//	cerr << "VisData bucket not yet full. HPGList, hpgRows: " << hpgVB_p.size() << " " << hpgVBNRows << endl;
      }
    else
      {
	std::chrono::duration<double> thisVB_duration = std::chrono::steady_clock::now() - mkHPGVB_startTime;
    
	mkHPGVB_duration += thisVB_duration;

	//
	// Send the data to the gridder and trigger gridding
	// calculations.  This call blocks till HPG library is ready
	// to receive more data.
	//
	//    if (hpgVBList_p.size() >= maxVBList_p)
	griddingTime += sendData(hpgVBBucket_p, nVisGridded_p, nDataBytes_p,
				 sizeofVisData_p, do_degrid);
      }    
    return;
  }
  //
  //-----------------------------------------------------------------------------------
  // Implementation for GridToData.
  //
  void
  AWVisResamplerHPG::GridToData(VBStore& vbs, const Array<Complex>& grid)
  {
    Cube<Complex> visCubeModel;
    
    int rend = vbs.endRow_p;
    if(rend==0) return;
    int rbeg = vbs.beginRow_p;
    
    auto ret = initializeHPG(vbs, grid.shape());
    bool do_degrid = std::get<0>(ret);
    CountedPtr<CFBuffer> cfb = std::get<1>(ret);
    casacore::Vector<casacore::Vector<casacore::Double> > pointingOffsets = cfb->getPointingOffset();

    int nGridPol = grid.shape()[2];
    int nGridChan = grid.shape()[3];
    
    int nDataPol  = vbs.flagCube_p.shape()[0];
    int nDataChan = vbs.flagCube_p.shape()[1];
    int startChan = 0;
    int endChan = nDataChan;
    uInt nVisPol=0;// nRows=rend-rbeg+1, nVisChan=endChan - startChan+1;
    for(Int ipol=0; ipol< nDataPol; ipol++)
      {
    	int targetIMPol=polMap_p(ipol);
    	if ((targetIMPol>=0) && (targetIMPol<nGridPol)) 
    	  nVisPol++;
      }
    
    //cerr << "Pointing offsets: " << pointingOffsets << " " << rbeg << " " << rend << " " << startChan << " " << endChan << " " << max(vbs.vb_p->visCube()) << endl;
    std::chrono::time_point<std::chrono::steady_clock> 
      mkHPGVB_startTime=std::chrono::steady_clock::now();
    

    Vector<Double> wVals, fVals; PolMapType mVals, mNdx, conjMVals, conjMNdx;
    Double fIncr, wIncr;
    // This loads the all-importnat conjMNDx and mNdx maps
    //
    cfb->getCoordList(fVals,wVals,mNdx, mVals, conjMNdx, conjMVals, fIncr, wIncr);
    Matrix<Double> sumwt(IPosition(2,nGridPol,nGridChan));// This is not used for degridding
    bool useFlaggedData=true;

    //    std::vector<hpg::VisData<HPGNPOL> > hpgVB;
    unsigned int hpgVBNRows = makeHPGVisBuffer<HPGNPOL>(hpgVB_p,
							sumwt,
							vbs,
							vb2CFBMap_p,
							nGridPol, nGridChan, nVisPol,
							rbeg, rend, startChan, endChan,
							chanMap_p, polMap_p,
							conjMNdx, conjMVals,
							dphase_p,pointingOffsets,
							cfsi_p,useFlaggedData);

    // Resize so that there are no unused VisData in the hpgVB at the end.
    hpgVB_p.resize(hpgVBNRows);    
    bool visDataBucketFull=(hpgVB_p.size()==hpgVBNRows);

    // Add to the list only if the hpgVB holds any data. This guard
    // is required since in the loop below we also count the number
    // visibilities gridded (the nVisGridded_p).
    if (hpgVB_p.size() > 0) hpgVBList_p.push_back(hpgVB_p);
    
    std::chrono::duration<double> thisVB_duration = std::chrono::steady_clock::now() - mkHPGVB_startTime;
    
    mkHPGVB_duration += thisVB_duration;
    //     cerr << "Done making hpg::VisData<N> (" << thisVB_duration.count() << ")" << endl;
    //
    // If the vbsList_p is full, send the VBs loaded in vbsList_p for gridding, and empty vbsList_p.
    // std::move() empties the storage of its argument.
    //

    if (!visDataBucketFull)
      {
	cerr << "VisData bucket not yet full. " << hpgVB_p.size() << " " << hpgVBNRows << endl;
      }
    if (hpgVBList_p.size() >= maxVBList_p)
      {
	timer_p.mark();
	for(unsigned i=0;i<hpgVBList_p.size();i++)
	  {
	    nVisGridded_p += hpgVBList_p[i].size()*HPGNPOL;
	    if (do_degrid)
	      {
	    	// hpg::degrid_get_predicted_visibilities() signature for reference
	    	// rval_t<std::tuple<GridderState, future<VisDataVector>>>
	    	// degrid_get_predicted_visibilities(
	    	// 				Device host_device,
	    	// 				std::vector<VisData<N>>&& visibilities)

		//cerr << "HPG VB: " << hpgVBList_p[i].size() << endl;
	    	auto maybe_predictedVis = hpgGridder_p->degrid_get_predicted_visibilities(
											  hpg::Device::OpenMP,
											  std::move(hpgVBList_p[i])
											  );
		if (hpg::is_error(maybe_predictedVis))
		  {
		    LogIO log_l(LogOrigin("AWVisResamplerHPG[R&D]","GridToData"));
		    log_l << "Failed hpg::degrid_get_predicted_visibilities()"
			  << " Error type: " << static_cast<int>(hpg::get_error(maybe_predictedVis).type()) << LogIO::SEVERE;
		  }
		hpgGridder_p->fence();
		//
		// Pack the visibilities and UVW from the predictedVis
		// data strcuture back into vbs->vb_p (the
		// casa::VisBuffer2)
		std::vector<hpg::VisData<HPGNPOL>> hpgModelVis=*std::get<1>(maybe_predictedVis).get();

		// unsigned nVisChan = endChan - startChan + 1;
		// unsigned nVisRow  = rend - rbeg + 1;
		// Transfer vis from hpgModelVis to vbs.vb_p
		makeCASAVisBuffer<HPGNPOL>(//nVisRow, nVisChan, nVisPol,
					   polMap_p, vbs, hpgModelVis);
	      }
	    griddingTime += timer_p.real();
	  }
	hpgVBList_p.resize(0);
      }
    return;
  }
  //
  //-----------------------------------------------------------------------------------
  //
  std::shared_ptr<std::complex<double>> AWVisResamplerHPG::getGridPtr(size_t& size) const
  {
    if (hpgGridder_p)
      {
        hpgGridder_p->fence();
        size = hpgGridder_p->grid_values_span();
        return hpgGridder_p->grid_values_ptr();
      }
    else
      {
        size = 0;
        return std::shared_ptr<std::complex<double>>();
      }
  }
  //
  //-------------------------------------------------------------------------
  //  
  std::shared_ptr<double> AWVisResamplerHPG::getSumWeightsPtr(size_t& size) const
  {
    if (hpgGridder_p)
      {
        hpgGridder_p->fence();
        size = hpgGridder_p->grid_weights_span();
        return hpgGridder_p->grid_weights_ptr();
      }
    else
      {
        size = 0;
        return std::shared_ptr<double>();
      }
  }
};// end namespace casa
