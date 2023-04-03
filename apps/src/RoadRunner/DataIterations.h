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
#include <hpg/hpg.hpp>

using namespace std;

class DataIterator
{
public:
  DataIterator(const bool isroot):isRoot_p(isroot) {};
  ~DataIterator() {};
  int iterVB(vi::VisibilityIterator2 *vi2,
	     vi::VisBuffer2 *vb,
	     const casacore::CountedPtr<casa::refim::FTMachine>& ftm,
	     const bool doPSF,
	     const casa::refim::FTMachine::Type dataCol,
	     int& nVB,
	     const std::string& imagingMode,
	     std::function<void(const int&)> cfSentNotifier
	     )
  {
    int vol=0;

    for (vi2->origin(); vi2->more(); vi2->next())
      {
	//cerr << "interVB: vol=" << vol << " " << nVB << endl;
	if (dataCol==casa::refim::FTMachine::CORRECTED)
	  vb->setVisCube(vb->visCubeCorrected());
	else
	  vb->setVisCube(vb->visCube());

	if (imagingMode=="predict")
	  {
	    ftm->get(*vb,0);
	    vi2->writeVisModel(vb->visCubeModel());
	  }
	else
	  ftm->put(*vb,-1,doPSF);

	vol+=vb->nRows();

	cfSentNotifier(nVB);

	nVB++;
      }

    return vol;
  };
  //
  //------------------------------------------------------------------------------------------
  // By default this method does the normal, single-threaded
  // iterations. The waitForCFReady function is exectured in the chunk
  // iterations.  cfSentNotifier is exectued in the VB iterations.
  //
  std::tuple<int, int, double>
  dataIter(vi::VisibilityIterator2 *vi2,
	   vi::VisBuffer2 *vb2,
	   const casacore::CountedPtr<casa::refim::FTMachine>& ftm,
	   const bool& doPSF,
	   const std::string& imagingMode,
	   std::function<void(int&, int& )> waitForCFReady=[](int&, int&){},//NoOp
	   std::function<void(const int&)> cfSentNotifier= [](const int&){} //NoOp
	   )

  {
    int vol=0;
    int nVB=0;
    int spwNdx=0;
    double griddingEngine_time=0;

    casa::refim::FTMachine::Type dataCol_l=casa::refim::FTMachine::CORRECTED;
    if(vi2->ms().tableDesc().isColumn("CORRECTED_DATA") ) dataCol_l = casa::refim::FTMachine::CORRECTED;
    else
      {
	LogIO log_l(LogOrigin("DataIterator","dataIter",WHERE));
	log_l << "CORRECTED_DATA column not found.  Using the DATA column instead." << LogIO::WARN << LogIO::POST;
	dataCol_l = casa::refim::FTMachine::OBSERVED;
      }
    
    ProgressMeter pm(1.0, vi2->ms().nrow(),
		     "dataIter", "","","",true);

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
    for (vi2->originChunks();vi2->moreChunks(); vi2->nextChunk())
      {
  	vi2->origin(); // So that the global vb is valid

	waitForCFReady(nVB,spwNdx);
	
  	std::chrono::time_point<std::chrono::steady_clock> griddingEngine_start
  	  = std::chrono::steady_clock::now();

  	vol+=iterVB(vi2, vb2,
		    ftm,
		    doPSF,
		    dataCol_l,
		    nVB,
		    imagingMode,
		    cfSentNotifier);

  	std::chrono::duration<double> tt = std::chrono::steady_clock::now() - griddingEngine_start;
  	griddingEngine_time += tt.count();

	if (isRoot_p)
  	  pm.update(Double(vol));
      }

    return std::make_tuple(nVB, vol,griddingEngine_time);
  };
private:
  bool isRoot_p;
};
#endif
