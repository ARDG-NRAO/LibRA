// -*- C++ -*-
//# DataIteratior.h: Definition of the DataIterator class
//# Copyright (C) 2021
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
//# Correspondence concerning this should be addressed as follows:
//#        Postal address: National Radio Astronomy Observatory
//#                        1003 Lopezville Road,
//#                        Socorro, NM - 87801, USA
//#
//# $Id$

/**
 * @file DataIterations.h
 * @brief Contains classes for iterating over visibility data.
 */

#ifndef ROADRUNNER_DATAITERATOR_H
#define ROADRUNNER_DATAITERATOR_H

#include <mutex>
#include <future>
#include <thread>
#include <condition_variable>
#include <functional>
#include <casacore/casa/Logging/LogFilter.h>
#include <synthesis/TransformMachines2/MakeCFArray.h>
#include <synthesis/TransformMachines2/ThreadCoordinator.h>
#include <synthesis/TransformMachines2/FTMachine.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <casacore/casa/System/ProgressMeter.h>
#include <msvis/MSVis/VisBuffer2.h>
//#include <hpg/hpg.hpp>

using namespace std;
using namespace casa;
using namespace casa::refim;
using namespace casacore;
// Possible example for making a generic functor in C++11.  C++14
// allows generic lambdas (where the complier produces similar code in
// the background).  We may benefit from using similar code for
// modifier lamdba functions as arguments.
//
// struct CompareFirst {
//     template <class Fst, class Snd>
//     bool operator()(const pair<Fst,Snd>& l, const pair<Fst,Snd>& r) const {
//         return l.first < r.first;
//     }
// };
// CompareFirst(1,2) and CompareFirst(1.2,3.1) will both work?


/**
 * @brief A class for iterating over visibility data.
 *
 * This class provides functions for iterating over visibility data.
 */
class DataIterator
{
public:
  /**
   * @brief Constructs a DataIterator object with the specified parameters.
   *
   * This constructor constructs a DataIterator object with the specified parameters.
   *
   * @param isroot Whether this is the root DataIterator object.
   * @param dataCol The type of data column to use.
   */
  DataIterator(const bool isroot,casa::refim::FTMachine::Type dataCol)
    :isRoot_p(isroot), dataCol_l(dataCol) {};
  /**
   * @brief Destroys the DataIterator object.
   *
   * This destructor destroys the DataIterator object.
   */
  ~DataIterator() {};
  //
  //-------------------------------------------------------------------------------------------------
  //
  /**
   * @brief Iterates over a VisibilityIterator2 object.
   *
   * This function iterates over a VisibilityIterator2 object, applying the specified FTMachine to each VisBuffer2 object and writing the results to the specified data column.
   *
   * @param vi2 The VisibilityIterator2 object to iterate over.
   * @param vb The VisBuffer2 object to use for the iteration.
   * @param ftm The FTMachine object to apply to each VisBuffer2 object.
   * @param doPSF Whether to calculate the PSF.
   * @param dataCol The type of data column to use.
   * @param nVB The number of VisBuffer2 objects processed.
   * @param dataConsumer The lambda function that consumes the VisBuffer2 produced in the iterations
   * @param cfSentNotifier A function to call when a VisBuffer2 object has been processed.
   * @return A std::vector<double> of length >=3 elements.
   *         First element of the vector has the total data volume in bytes.
   *         Second elements has time for the data I/O in seconds.
   *         Third elements has the total number of MS rows consumed.
   */
  std::vector<double>
  iterVB(vi::VisibilityIterator2 *vi2,
	 vi::VisBuffer2 *vb,
	 int& nVB,
	 std::function<std::vector<double> (vi::VisBuffer2* vb,vi::VisibilityIterator2 *vi2_l)> dataConsumer,
	 std::function<void(const int&)> cfSentNotifier
	 )
  {
    int vol=0,nRows=0;
    double dataIO_time=0.0;

    for (vi2->origin(); vi2->more(); vi2->next())
      {
	auto ret=dataConsumer(vb,vi2);
	vol += ret[0]; // Vis volume in bytes
	dataIO_time += ret[1];
	nRows+=vb->nRows();

	cfSentNotifier(nVB);
	
	nVB++;
      }

    std::vector<double> ret={(double)vol,(double)dataIO_time,(double)nRows};
    return ret;
  };

  //
  //-------------------------------------------------------------------------------------------------
  //
  /**
   * @brief Iterates over a VisibilityIterator2 object with chunking.
   *
   * This function iterates over a VisibilityIterator2 object with chunking, applying the specified FTMachine to each VisBuffer2 object and writing the results to the specified data column.
   *
   * @param vi2 The VisibilityIterator2 object to iterate over.
   * @param vb2 The VisBuffer2 object to use for the iteration.
   * @param ftm The FTMachine object to apply to each VisBuffer2 object.
   * @param doPSF Whether to calculate the PSF.
   * @param dataConsumer The lambda function that consumes the VisBuffer2 produced in the iterations
   * @param waitForCFReady A function to call when a chunk has been processed.
   * @param cfSentNotifier A function to call when a VisBuffer2 object has been processed.
   * @return A std::vector<double> of length >=5.
   *         First element contains the cumulative number of VisBuffer2 processed.
   *         Second element contains the cumulative data processed in bytes. 
   *         Third element contains the cumulative time spend in gridding/degridding in seconds.
   *         Fourth element contains the cumulative time in data I/O in seconds. 
   *         Fifth element contains the cumulative number of MS rows processed.
   */
  std::vector<double>
  dataIter(vi::VisibilityIterator2 *vi2,
	   vi::VisBuffer2 *vb2,
	   std::function<std::vector<double> (vi::VisBuffer2* vb,vi::VisibilityIterator2 *vi2_l)> dataConsumer,
	   std::function<void(int&, int& )> waitForCFReady=[](int&, int&){},//NoOp
	   std::function<void(const int&)> cfSentNotifier= [](const int&){} //NoOp
	   )
    
  {
    unsigned long vol=0,nRows=0;
    int nVB=0;
    int spwNdx=0;
    double griddingEngine_time=0,totalDataIO_time=0.0;

    ProgressMeter pm(1.0, vi2->ms().nrow(),
		     "dataIter", "","","",true);

    for (vi2->originChunks();vi2->moreChunks(); vi2->nextChunk())
      {
  	vi2->origin(); // So that the global vb is valid

	waitForCFReady(nVB,spwNdx);

  	std::chrono::time_point<std::chrono::steady_clock> griddingEngine_start
  	  = std::chrono::steady_clock::now();

	//
	// In the following control-flow waitForCFReady() and
	// cfSentNotifier() encapsulate coordination between the
	// main-thread (where the gridder runs) and the cfserver-thread
	// (which manages the CF paging).  For single-threaded operations
	// (non HPG) these functions are a NoOp.
	//
	// The loop over data below blocks till waitForCFReady() returns.
	// After CFReady signal is received from the CFServer thread, in
	// waitForCFSent the current CFSI (cfsi_g) and the device CFArray
	// (dcfa_sptr_g) are set for the vis resampler if a new CF set is
	// available (due to change in VB SPW ID).  The CFArray is then
	// sent to the device from the waitForCFReady() lambda
	// function. iterVB() then calls cfSentNotifier() function to send
	// the CFSent signal after successfully triggering the gridding
	// with the current VB and CFs on the device.  CFServer in the
	// main thread blocks till it gets the CFSent signal, after which
	// it starts filling the DeviceCFArray with the next CF set in
	// parallel with gridding which is always in the main thread, and
	// issues a CFReady signal when the DeviceCFArray is ready for
	// use.  Gridding is triggered via the call to iterVB() of this
	// class instance, but the call is always made from the main
	// thread.
	//

  	auto ret = iterVB(vi2, vb2,
			  nVB,
			  dataConsumer,
			  cfSentNotifier);

  	std::chrono::duration<double> tt = std::chrono::steady_clock::now() - griddingEngine_start;
  	griddingEngine_time += tt.count();

	vol+=ret[0];
	totalDataIO_time+=ret[1];
	nRows+=ret[2];
	if (isRoot_p)
  	  pm.update(Double(vol));
      }

    std::vector<double> ret={(double)nVB, (double)vol,(double)griddingEngine_time,(double)totalDataIO_time,(double)nRows};
    return ret;
  };


private:
  bool isRoot_p;
  /**
   * @brief The type of data column to use.
   */
  casa::refim::FTMachine::Type dataCol_l;
};
#endif



  // std::tuple<int, double>
  // iterVB(vi::VisibilityIterator2 *vi2,
  // 	     vi::VisBuffer2 *vb,
  // 	     const casacore::CountedPtr<casa::refim::FTMachine>& ftm,
  // 	     const bool doPSF,
  // 	     const casa::refim::FTMachine::Type dataCol,
  // 	     int& nVB,
  // 	     const std::string& imagingMode,
  // 	     std::function<void(const int&)> cfSentNotifier
  // 	     )
  // {
  //   int vol=0;
  //   double dataIO_time=0.0;
    
  //   std::chrono::time_point<std::chrono::steady_clock> dataIO_start;

  //   auto DataConsumer = [&](vi::VisBuffer2 *vb_l,
  // 			    const std::string& imagingMode_l,
  // 			    const std::chrono::time_point<std::chrono::steady_clock>& dataIO_start_l)
  //   {
  // 	if (imagingMode_l=="predict")
  // 	  {
  // 	    // Predict the data into the VB (presumably the name get()
  // 	    // means "get the data from the complex grid into the VB")
  // 	    ftm->get(*vb_l,0);
	    
  // 	    // Write the VB to the specific data column.  Predicted data
  // 	    // in the in-memory model is always in the VB::visCubeModel.
  // 	    // So always make that persistent in the specified column of
  // 	    // the VI.
  // 	    dataIO_start = std::chrono::steady_clock::now();

  // 	    if (dataCol==casa::refim::FTMachine::MODEL)          vi2->writeVisModel(vb_l->visCubeModel());
  // 	    else if (dataCol==casa::refim::FTMachine::CORRECTED) vi2->writeVisCorrected(vb_l->visCubeModel());//vb->visCubeCorrected());
  // 	    else                                                 vi2->writeVisObserved(vb_l->visCubeModel());//vb->visCube());

  // 	    std::chrono::duration<double> tt = std::chrono::steady_clock::now() - dataIO_start_l;
  // 	    return tt.count();
  // 	  }
  // 	else
  // 	  {
  // 	    // Read the data from a specific data column into the
  // 	    // in-memory buffer
  // 	    dataIO_start = std::chrono::steady_clock::now();

  // 	    if (dataCol==casa::refim::FTMachine::CORRECTED)   vb->setVisCube(vb->visCubeCorrected());
  // 	    else if (dataCol==casa::refim::FTMachine::MODEL)  vb->setVisCube(vb->visCubeModel());
  // 	    else                                              vb->setVisCube(vb->visCube());

  // 	    std::chrono::duration<double> tt = std::chrono::steady_clock::now() - dataIO_start_l;

  // 	    // Grid the data from the VB (presumably the name put()
  // 	    // means "put the data from the VB into the complex grid")
  // 	    ftm->put(*vb,-1,doPSF);
	    
  // 	    return tt.count();
  // 	  }
      
  //   };
    
  //   for (vi2->origin(); vi2->more(); vi2->next())
  //     {

  // 	dataIO_time += DataConsumer(vb,imagingMode,dataIO_start);
  // 	vol+=vb->nRows();

  // 	cfSentNotifier(nVB);
	
  // 	nVB++;
  //     }
  //   return std::make_tuple(vol,dataIO_time);
  // };
  
  // /**
  //  * @brief Iterates over a VisibilityIterator2 object with chunking.
  //  *
  //  * This function iterates over a VisibilityIterator2 object with chunking, applying the specified FTMachine to each VisBuffer2 object and writing the results to the specified data column.
  //  *
  //  * @param vi2 The VisibilityIterator2 object to iterate over.
  //  * @param vb2 The VisBuffer2 object to use for the iteration.
  //  * @param ftm The FTMachine object to apply to each VisBuffer2 object.
  //  * @param doPSF Whether to calculate the PSF.
  //  * @param imagingMode The imaging mode to use.
  //  * @param waitForCFReady A function to call when a chunk has been processed.
  //  * @param cfSentNotifier A function to call when a VisBuffer2 object has been processed.
  //  * @return A tuple containing the total number of rows processed, the number of VisBuffer2 objects processed, and the time spent in the gridding engine.
  //  */
  // std::tuple<int, int, double,double>
  // dataIter(vi::VisibilityIterator2 *vi2,
  // 	   vi::VisBuffer2 *vb2,
  // 	   const casacore::CountedPtr<casa::refim::FTMachine>& ftm,
  // 	   const bool& doPSF,
  // 	   const std::string& imagingMode,
  // 	   std::function<void(int&, int& )> waitForCFReady=[](int&, int&){},//NoOp
  // 	   std::function<void(const int&)> cfSentNotifier= [](const int&){} //NoOp
  // 	   )
    
  // {
  //   int vol=0;
  //   int nVB=0;
  //   int spwNdx=0;
  //   double griddingEngine_time=0,totalDataIO_time=0.0;

  //   ProgressMeter pm(1.0, vi2->ms().nrow(),
  // 		     "dataIter", "","","",true);

  //   // In the following control-flow waitForCFReady() and
  //   // cfSentNotifier() encapsulate coordination between the
  //   // main-thread (where the gridder runs) and the cfserver-thread
  //   // (which manages the CF paging).  For single-threaded operations
  //   // (non HPG) these functions are a NoOp.
  //   //
  //   // The loop over data below blocks till waitForCFReady() returns.
  //   // After CFReady signal is received from the CFServer thread, in
  //   // waitForCFSent the current CFSI (cfsi_g) and the device CFArray
  //   // (dcfa_sptr_g) are set for the vis resampler if a new CF set is
  //   // available (due to change in VB SPW ID).  The CFArray is then
  //   // sent to the device from the waitForCFReady() lambda
  //   // function. iterVB() then calls cfSentNotifier() function to send
  //   // the CFSent signal after successfully triggering the gridding
  //   // with the current VB and CFs on the device.  CFServer in the
  //   // main thread blocks till it gets the CFSent signal, after which
  //   // it starts filling the DeviceCFArray with the next CF set in
  //   // parallel with gridding which is always in the main thread, and
  //   // issues a CFReady signal when the DeviceCFArray is ready for
  //   // use.  Gridding is triggered via the call to iterVB() of this
  //   // class instance, but the call is always made from the main
  //   // thread.
  //   //
  //   for (vi2->originChunks();vi2->moreChunks(); vi2->nextChunk())
  //     {
  // 	vi2->origin(); // So that the global vb is valid

  // 	waitForCFReady(nVB,spwNdx);

  // 	std::chrono::time_point<std::chrono::steady_clock> griddingEngine_start
  // 	  = std::chrono::steady_clock::now();

  // 	auto ret = iterVB(vi2, vb2,
  // 			  ftm,
  // 			  doPSF,
  // 			  dataCol_l,
  // 			  nVB,
  // 			  imagingMode,
  // 			  cfSentNotifier);

  // 	std::chrono::duration<double> tt = std::chrono::steady_clock::now() - griddingEngine_start;
  // 	griddingEngine_time += tt.count();

  // 	vol+=std::get<0>(ret);
  // 	totalDataIO_time+=std::get<1>(ret);
  // 	if (isRoot_p)
  // 	  pm.update(Double(vol));
  //     }

  //   return std::make_tuple(nVB, vol,griddingEngine_time,totalDataIO_time);
  // };

