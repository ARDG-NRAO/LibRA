// -*- C++ -*-
//# ResamplerWorklet.cc: Implementation of the ResamplerWorklet class
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
#include <synthesis/TransformMachines/Utils.h>
#include <synthesis/TransformMachines/VisibilityResampler.h>
#include <synthesis/MeasurementComponents/ResamplerWorklet.h>
#include <synthesis/MeasurementComponents/MThWorkIDEnum.h>
#include <synthesis/Utilities/ThreadCoordinator.h>
//#include <synthesis/Utilities/ThreadTimers.h>
#include <fstream>
#include <sys/syscall.h>

using namespace casacore;
namespace casa{
  ResamplerWorklet& ResamplerWorklet::operator=(const ResamplerWorklet& other)
  {
    myResampler_p = other.myResampler_p;
    myVBStore_p = other.myVBStore_p;
    myGriddedDataDouble_p = other.myGriddedDataDouble_p;
    myGriddedDataSingle_p = other.myGriddedDataSingle_p;
    mySumWt_p = other.mySumWt_p;
    myThreadClerk_p = other.myThreadClerk_p;
    myID_p = other.myID_p;
    myPID_p = other.myPID_p;
    myTID_p = other.myTID_p;
    mySkyFTGrid_p = other.mySkyFTGrid_p;
    return *this;
  }
  
  void ResamplerWorklet::initThread(Int& id, 
				    CountedPtr<ThreadCoordinator<Int> >& threadClerk,
				    VisibilityResamplerBase* resampler)
  {
    //      LogIO log_p(LogOrigin("ResamplerWorklet","initThread"));
      setID(id);
      myThreadClerk_p = threadClerk;
      myResampler_p = resampler;
  }
  void ResamplerWorklet::initToVis(VBStore* vbs, const Array<Complex>* skyFTGrid) 
  {
    //    LogIO log_p(LogOrigin("ResamplerWorklet","initToVis"));
    myVBStore_p = vbs;
    mySkyFTGrid_p = skyFTGrid;
  }
  void ResamplerWorklet::initToSky(VBStore* vbs,Array<DComplex>* griddedData, Matrix<Double>* sumwt) 
  {
    //    LogIO log_p(LogOrigin("ResamplerWorklet","init (double precision)"));
    myVBStore_p = vbs;
    myGriddedDataDouble_p = griddedData;
    mySumWt_p = sumwt;
  }
  void ResamplerWorklet::initToSky(VBStore* vbs,Array<Complex>* griddedData, Matrix<Double>* sumwt) 
  {
    //    LogIO log_p(LogOrigin("ResamplerWorklet","init (single precision)"));
    myVBStore_p = vbs;
    myGriddedDataSingle_p = griddedData;
    mySumWt_p = sumwt;
  }
  void ResamplerWorklet::init(Int& id, 
			      CountedPtr<ThreadCoordinator<Int> >& threadClerk,
			      VisibilityResamplerBase* resampler, VBStore* vbs,
			      Array<DComplex>* griddedData, Matrix<Double>* sumwt,
			      Array<Complex>* skyFTGrid)
    {
      //      LogIO log_p(LogOrigin("ResamplerWorklet","init (double precision)"));

      setID(id);
      myThreadClerk_p = threadClerk;
      myResampler_p = resampler;
      myVBStore_p = vbs;
      myGriddedDataDouble_p = griddedData;
      mySumWt_p = sumwt;
      mySkyFTGrid_p = skyFTGrid;
      //      log_p << "Gridder worklet # " << id << " ready." << LogIO::POST;
    };
  void ResamplerWorklet::init(Int& id, 
			      CountedPtr<ThreadCoordinator<Int> >& threadClerk,
			      VisibilityResamplerBase* resampler, VBStore* vbs,
			      Array<Complex>* griddedData, Matrix<Double>* sumwt,
			      Array<Complex>* skyFTGrid)
    {
      //      LogIO log_p(LogOrigin("ResamplerWorklet","init (single precision)"));

      setID(id);
      myThreadClerk_p = threadClerk;
      myResampler_p = resampler;
      myVBStore_p = vbs;
      myGriddedDataSingle_p = griddedData;
      mySumWt_p = sumwt;
      mySkyFTGrid_p = skyFTGrid;
      //      log_p << "Gridder worklet # " << id << " ready." << LogIO::POST;
    };

  void ResamplerWorklet::setPID()
  {
    LogIO log_p(LogOrigin("ResamplerWorklet","setPID"));
    myTID_p = gettid_p (); myPID_p = getpid ();
    log_p << "Worklet # " << myID_p << " Activated. " 
    	  << "PID = " << myPID_p << " TID = " << myTID_p
    	  << LogIO::POST;
  }
  void* ResamplerWorklet::run()
  {
    LogIO log_p(LogOrigin("ResamplerWorklet","run"));

    setPID();
    //    DT tCycleG, tWaitG, tWorkG,tCycleDG, tWaitDG, tWorkDG, tWorkR, tWaitR, tCycleR;

    //    T t1,t2,t3;
    Int *doDataToGrid;
    while(true)
      {
	//	Timers t1 = Timers::getTime();
	//	Bool *doDataToGrid;
	doDataToGrid = myThreadClerk_p->waitForWork(this);
	if (doDataToGrid == NULL) break;

	// Timers t2G=Timers::getTime();
	// Timers t2DG=Timers::getTime();
	// Timers t2R=Timers::getTime();
	//	if (*doDataToGrid==1)   // Gridding work
	if (*doDataToGrid==MThWorkID::DATATOGRID)   // Gridding work
	  {
	    //	    cerr << myID_p << " " << myVBStore_p->beginRow_p << " " << myVBStore_p->endRow_p << endl;
	    //	    t2G=Timers::getTime();
	    if (myGriddedDataDouble_p != NULL)
	      myResampler_p->DataToGrid(*myGriddedDataDouble_p, *myVBStore_p, 
					*mySumWt_p, myVBStore_p->dopsf_p);
	    else
	      myResampler_p->DataToGrid(*myGriddedDataSingle_p, *myVBStore_p, 
					*mySumWt_p, myVBStore_p->dopsf_p);
	  }
	//	else if (*doDataToGrid == 0)                      // De-gridding work
	else if (*doDataToGrid == MThWorkID::GRIDTODATA)                      // De-gridding work
	  {
	    //	    t2DG=Timers::getTime();
	    myResampler_p->GridToData(*myVBStore_p, *mySkyFTGrid_p);
	  }
	//	else if (*doDataToGrid == 2)
	else if (*doDataToGrid == MThWorkID::RESIDUALCALC)
	  {
	    //	    t2R=Timers::getTime();
	    myResampler_p->ComputeResiduals(*myVBStore_p);
	  }
	  
	//	Timers t3=Timers::getTime();

	// if (*doDataToGrid==1)
	//   {
	//     tWaitG += t2G - t1;
	//     tWorkG += t3 - t2G;
	//     tCycleG += t3 - t1;
	//   }
	// else if (*doDataToGrid == 2)
	//   {
	//     tWaitDG += t2DG - t1;
	//     tWorkDG += t3 - t2DG;
	//     tCycleDG += t3 - t1;
	//   }
	// else
	//   {
	//     tWaitR += t2R - t1;
	//     tWorkR +=  t3 - t2R;
	//     tCycleR += t3 -t1;
	//   }
      }
    log_p << "Alveeda from Workelet # " << myID_p << LogIO::POST;
    //    doDataToGrid = myThreadClerk_p->waitForWork(this);
    //    terminate();

    // log_p << "GWait="  << tWaitG.formatAverage().c_str() << " "
    // 	  << "GWork="  << tWorkG.formatAverage().c_str() << " "
    // 	  << "GTotal=" << tCycleG.formatAverage().c_str()
    // 	  << LogIO::POST;
    // log_p << "DGWait="  << tWaitDG.formatAverage().c_str() << " "
    // 	  << "DGWork="  << tWorkDG.formatAverage().c_str() << " "
    // 	  << "DGTotal=" << tCycleDG.formatAverage().c_str()
    // 	  << LogIO::POST;
    // log_p << "RWait="  << tWaitR.formatAverage().c_str() << " "
    // 	  << "RWork="  << tWorkR.formatAverage().c_str() << " "
    // 	  << "RTotal=" << tCycleR.formatAverage().c_str()
    // 	  << LogIO::POST;
    return NULL;
  }
};
