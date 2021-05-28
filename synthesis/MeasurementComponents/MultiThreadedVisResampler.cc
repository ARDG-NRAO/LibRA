// -*- C++ -*-
//# MultiThreadedVisResample.cc: Implementation of the MultiThreadedVisibilityResampler class
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
#include <synthesis/Utilities/ThreadCoordinator.h>
//#include <msvis/MSVis/UtilJ.h>
#include <synthesis/TransformMachines/Utils.h>
#include <synthesis/TransformMachines/VisibilityResampler.h>
#include <synthesis/MeasurementComponents/MultiThreadedVisResampler.h>
#include <synthesis/MeasurementComponents/ResamplerWorklet.h>
#include <synthesis/TransformMachines/AWVisResampler.h>
#include <synthesis/MeasurementComponents/MThWorkIDEnum.h>
#include <fstream>

using namespace casacore;
namespace casa{
  template 
  void MultiThreadedVisibilityResampler::DataToGridImpl_p(Array<Complex>& griddedData,  
							  VBStore& vbs, 
							  Matrix<Double>& sumwt,
							  const Bool& dopsf,
							  Bool useConjFreqCF);
  template 
  void MultiThreadedVisibilityResampler::DataToGridImpl_p(Array<DComplex>& griddedData,  
							  VBStore& vbs, 
							  Matrix<Double>& sumwt,
							  const Bool& dopsf,
							  Bool useConjFreqCF);
  //
  //---------------------------------------------------------------------------------------
  //
  MultiThreadedVisibilityResampler::MultiThreadedVisibilityResampler(const Bool& doublePrecision,
								     CountedPtr<VisibilityResamplerBase>& visResampler, 
								     const Int& n):
    resamplers_p(), doubleGriddedData_p(), singleGriddedData_p(), sumwt_p(), gridderWorklets_p(), 
    vbsVec_p(), threadClerk_p(),threadStarted_p(false), visResamplerCtor_p(visResampler), 
    whoLoadedVB_p(MThWorkID::NOONE), currentVBS_p(0)
    {
      if (n < 0) nelements_p = SynthesisUtils::getenv(FTMachineNumThreadsEnvVar, n);
      if (nelements_p < 0) nelements_p = 1;
      init(doublePrecision);
      // t4G_p=Timers::getTime();
      // t4DG_p=Timers::getTime();
    }
  //
  //---------------------------------------------------------------------------------------
  //
  MultiThreadedVisibilityResampler::MultiThreadedVisibilityResampler(const Bool& doublePrecision,
								     const Int& n):
    resamplers_p(), doubleGriddedData_p(), singleGriddedData_p(), sumwt_p(), gridderWorklets_p(), 
    vbsVec_p(), threadClerk_p(),threadStarted_p(false), visResamplerCtor_p(), 
    whoLoadedVB_p(MThWorkID::NOONE),currentVBS_p(0)
    {
      if (n < 0) nelements_p = SynthesisUtils::getenv(FTMachineNumThreadsEnvVar, n);
      if (nelements_p < 0) nelements_p = 1;
      init(doublePrecision);
      // t4G_p=Timers::getTime();
      // t4DG_p=Timers::getTime();
    }
  //
  //---------------------------------------------------------------------------------------
  //
  void MultiThreadedVisibilityResampler::copy(const MultiThreadedVisibilityResampler& other)
  {
    resamplers_p.assign(other.resamplers_p);
    gridderWorklets_p.assign(other.gridderWorklets_p);
    vbsVec_p.assign(other.vbsVec_p);

    doubleGriddedData_p.assign(other.doubleGriddedData_p);
    singleGriddedData_p.assign(other.singleGriddedData_p);
    sumwt_p.assign(other.sumwt_p);
    // doubleGriddedData_p.reference(other.doubleGriddedData_p);
    // singleGriddedData_p.reference(other.singleGriddedData_p);
    // sumwt_p.reference(other.sumwt_p);

    nelements_p       = other.nelements_p;
    doublePrecision_p = other.doublePrecision_p;
    threadClerk_p     = other.threadClerk_p;
    threadStarted_p   = other.threadStarted_p;
    whoLoadedVB_p     = other.whoLoadedVB_p;
    currentVBS_p      = other.currentVBS_p;
    // t4G_p=other.t4G_p;
    // t4DG_p=other.t4DG_p;
  }
  //
  //---------------------------------------------------------------------------------------
  //
  MultiThreadedVisibilityResampler& 
  MultiThreadedVisibilityResampler::operator=(const MultiThreadedVisibilityResampler& other)
  { copy(other); return *this;}
  //
  //---------------------------------------------------------------------------------------
  //
  void MultiThreadedVisibilityResampler::init(const Bool& doublePrecision)
  {
    LogIO log_p(LogOrigin("MultiThreadedVisibilityResampler","init"));
    doublePrecision_p=doublePrecision;
    allocateBuffers();
  }
  //
  //---------------------------------------------------------------------------------------
  //
  void MultiThreadedVisibilityResampler::cleanup()
  {
    //    if ((nelements() > 1)  && (threadClerk_p->nThreads() > 0))
    if (threadClerk_p->nThreads() > 0)
      {
	//      threadClerk_p->getToWork(NULL); // Signal the threads to quit
	threadClerk_p->giveWorkToWorkers(NULL); // Signal the threads to quit
	threadClerk_p->setNThreads(0);
	//	if (!threadClerk_p.null()) {delete &(*threadClerk_p);}
	vbsVec_p.resize(0,0);
	resamplers_p.resize(0);
	//    for(Int i=0; i<gridderWorklets_p.nelements(); i++) delete &(*gridderWorklets_p[i]);
	gridderWorklets_p.resize(0);
	sumwt_p.resize(0);
	doubleGriddedData_p.resize(0);
	singleGriddedData_p.resize(0);
	nelements_p=0;
	whoLoadedVB_p = MThWorkID::NOONE;
	currentVBS_p=0;
	//    delete mutexForResamplers_p;
      }
  }
  //
  //---------------------------------------------------------------------------------------
  //
  void MultiThreadedVisibilityResampler::releaseBuffers()
  {
    // for (Int i=0;i<nelements();i++)
    //   {
    // 	//	if (!resamplers_p[i].null()) delete &(*resamplers_p[i]);
    // 	//	if (!gridderWorklets_p[i].null()) delete &(*gridderWorklets_p[i]);
    // 	if (doublePrecision_p)
    // 	  if (!doubleGriddedData_p[i].null()) delete &(*doubleGriddedData_p[i]);
    // 	  else if (!singleGriddedData_p[i].null()) delete &(*singleGriddedData_p[i]);
    // 	if (!sumwt_p[i].null()) delete &(*sumwt_p[i]);
    //   }
    // resamplers_p.resize(0);
    // gridderWorklets_p.resize(0);
    doubleGriddedData_p.resize(0);
    singleGriddedData_p.resize(0);
    sumwt_p.resize(0);
  }
  //
  //---------------------------------------------------------------------------------------
  //
  void MultiThreadedVisibilityResampler::makeInfrastructureContainers()
  {
    //
    // Fill the various containers (allocate buffers)
    //----------------------------------------------------------------
    if (threadClerk_p.null()) threadClerk_p = new ThreadCoordinator<Int>(nelements());
    for (Int i=0;i<nelements();i++)
      {
	resamplers_p[i] = visResamplerCtor_p->clone();
	gridderWorklets_p[i] = new ResamplerWorklet();
      }
  }
  //
  //---------------------------------------------------------------------------------------
  //
  Double MultiThreadedVisibilityResampler::allocateDataBuffers()
  {
    Int totalMem=0;
    for (Int i=0;i<nelements();i++)
      {
	//
	// *GriddedData and sumwt are the target complex grids
	// *(for gridding) and accumulating sum-of-weights (also
	// *during gridding).  These may or may not be shared.
	//
	if (doublePrecision_p) doubleGriddedData_p[i] = new Array<DComplex>;
	else                   singleGriddedData_p[i] = new Array<Complex>;
	sumwt_p[i] = new Matrix<Double>;
	if (!threadStarted_p)
	  {
	    if (doublePrecision_p) totalMem += (*doubleGriddedData_p[i]).size()*sizeof(DComplex);
	    else totalMem += (*singleGriddedData_p[i]).size()*sizeof(Complex);
	    totalMem += (*sumwt_p[i]).size()*sizeof(Double);
	    // (*gridderWorklets_p[i]).initThread(i, threadClerk_p, 
	    // 				       &(*resamplers_p[i]));
	    // (*gridderWorklets_p[i]).startThread();
	  }
      }
    return totalMem;
  }
  //
  //---------------------------------------------------------------------------------------
  //
  void MultiThreadedVisibilityResampler::startThreads()
  {
    if (!threadStarted_p)
      for (Int i=0;i<nelements();i++)
  	{
	  (*gridderWorklets_p[i]).initThread(i, threadClerk_p, 
					     &(*resamplers_p[i]));
	  (*gridderWorklets_p[i]).startThread();
	}
    threadStarted_p=true;
  }
  //
  //---------------------------------------------------------------------------------------
  //
  void MultiThreadedVisibilityResampler::allocateBuffers(Bool /*newDataBuffers*/)
  {
    LogIO log_p(LogOrigin("MultiThreadedVisibilityResampler","allocateBuffers"));
    Double totalMem=0;
    //    if (nelements() > 1)
      {
	if (visResamplerCtor_p.null())
	  log_p << "Internal Error: VisResampler Ctor not initialized" << LogIO::EXCEPTION;

	log_p << "Allocating buffers per thread.  No. of threads = " << nelements() << endl;
	//	mutexForResamplers_p = new async::Mutex;

	//
	// Resize the containers
	//----------------------------------------------------------------
	if (doublePrecision_p)  doubleGriddedData_p.resize(nelements());
	else                    singleGriddedData_p.resize(nelements());

	resamplers_p.resize(nelements());
	sumwt_p.resize(nelements());
	gridderWorklets_p.resize(nelements());
	vbsVec_p.resize(nelements(),1);
	//----------------------------------------------------------------

	makeInfrastructureContainers();
	totalMem=allocateDataBuffers();
	startThreads();
	//----------------------------------------------------------------
      }
    // else
    //   {
    // 	resamplers_p.resize(1);
    // 	//	resamplers_p[0] = new VisibilityResampler();
    // 	//	resamplers_p[0] = visResamplerCtor_p;
    // 	resamplers_p[0] = visResamplerCtor_p->clone();
    // 	vbsVec_p.resize(1,1);
    // 	cerr << "#@%@%@%#$@#$"<< endl;
    //     	gridderWorklets_p.resize(1);
    // 	    gridderWorklets_p[0] = new ResamplerWorklet();
    //         singleGriddedData_p.resize(1);
    // 	    sumwt_p.resize(1);
    // 	    if (doublePrecision_p) doubleGriddedData_p[0] = new Array<DComplex>;
    // 	    else	           singleGriddedData_p[0] = new Array<Complex>;
    // 	    sumwt_p[0] = new Matrix<Double>;
    //   }
    if (totalMem > 0)
      log_p << "Total memory used in buffers for multi-threading: " << totalMem/(1024*1024) << " MB" << LogIO::POST;
  }
  //
  //---------------------------------------------------------------------------------------
  //
  void MultiThreadedVisibilityResampler::setParams(const Vector<Double>& uvwScale, 
						   const Vector<Double>& offset,
						   const Vector<Double>& dphase)
  {for (Int i=0;i < nelements(); i++) resamplers_p[i]->setParams(uvwScale, offset, dphase);}
  //
  //---------------------------------------------------------------------------------------
  //
  void MultiThreadedVisibilityResampler::setMaps(const Vector<Int>& chanMap, 
						 const Vector<Int>& polMap)
  {for (Int i=0;i < nelements(); i++) resamplers_p[i]->setMaps(chanMap, polMap);};
  //
  //---------------------------------------------------------------------------------------
  //
  void MultiThreadedVisibilityResampler::setCFMaps(const Vector<Int>& cfMap, 
						   const Vector<Int>& conjCFMap)
  {for (Int i=0;i < nelements(); i++) resamplers_p[i]->setCFMaps(cfMap, conjCFMap);};
  //
  //---------------------------------------------------------------------------------------
  //
  void MultiThreadedVisibilityResampler::setConvFunc(const CFStore& cfs)
  {for (Int i=0;i < nelements(); i++) resamplers_p[i]->setConvFunc(cfs);};
  //
  //---------------------------------------------------------------------------------------
  //
  void MultiThreadedVisibilityResampler::setFreqMaps(const Matrix<Double>& spwChanFreqs, const Matrix<Double>& spwChanConjFreqs)
  {for (Int i=0;i < nelements(); i++) resamplers_p[i]->setFreqMaps(spwChanFreqs,spwChanConjFreqs);}
  //
  //---------------------------------------------------------------------------------------
  //
  void MultiThreadedVisibilityResampler::scatter(Matrix<VBStore>& vbStores,const VBStore& vbs)
  {
    Int nRows=vbs.nRow_p, nr,br=0;
    nr=(Int)(nRows/nelements())+1;

    for(Int i=0; i < nelements(); i++)
      {
	vbStores(i,currentVBS_p).reference(vbs);
	vbStores(i,currentVBS_p).beginRow_p = min(br,nRows); 
	vbStores(i,currentVBS_p).endRow_p   = min(br+nr,nRows);
	br = vbStores(i,currentVBS_p).endRow_p;
      }
  }
  //
  //---------------------------------------------------------------------------------------
  //
  void MultiThreadedVisibilityResampler::GatherGrids(Array<DComplex>& griddedData,
						     Matrix<Double>& sumwt)
  {
    LogIO log_p(LogOrigin("MultiThreadedVisibilityResampler(Double)","GatherGrids"));
    //    if (nelements() > 1)
      {
	//	log_p << "Deleting thread clerk" << LogIO::POST;
	//	delete threadClerk_p; threadClerk_p=NULL;
	log_p << "Gathering grids..." << LogIO::POST;
	// cerr << "Gridded data shape = " << griddedData.shape() << " " << sumwt.shape() << endl;
	// for (Int i=0;i<nelements();i++)
	//   cerr << "Gridded buffer shape = " 
	//        << (*doubleGriddedData_p[i]).shape() << " " 
	//        << (*sumwt_p[i]).shape() << endl;
	for(Int i=0;i<nelements(); i++)
	  {
	    griddedData += *(doubleGriddedData_p[i]);
	    sumwt += *(sumwt_p[i]);
	  }
      }
    log_p << "Gridder timing: " 
     	  << "Setup = " << tSetupG.formatAverage().c_str() << " " 
     	  << "SendData = " << tSendDataG.formatAverage().c_str() << " " 
     	  << "WaitForWork = " << tWaitForWorkG.formatAverage().c_str() 
     	  << "Outside = " << tOutsideG.formatAverage().c_str() 
     	  << LogIO::POST;
    //  log_p << "DGridder timing: "
    //  	  << "Setup = " << tSetupDG.formatAverage().c_str() << " " 
    //  	  << "SendData = " << tSendDataDG.formatAverage().c_str() << " " 
    //  	  << "WaitForWork = " << tWaitForWorkDG.formatAverage().c_str() 
    //  	  << "Outside = " << tOutsideDG.formatAverage().c_str() 
    //  	  << LogIO::POST;
  }
  //
  //---------------------------------------------------------------------------------------
  //
  void MultiThreadedVisibilityResampler::GatherGrids(Array<Complex>& griddedData,
						     Matrix<Double>& sumwt)
  {
    LogIO log_p(LogOrigin("MultiThreadedVisibilityResampler(Single)","GatherGrids"));
    //    if (nelements() > 1)
      {
	log_p << "Gathering grids..." << LogIO::POST;
	for(Int i=0;i<nelements(); i++)
	  {
	    griddedData += *(singleGriddedData_p[i]);
	    sumwt += *(sumwt_p[i]);
	  }
      }
    log_p << "Gridder timing: " 
     	  << "Setup = " << tSetupG.formatAverage().c_str() << " " 
     	  << "SendData = " << tSendDataG.formatAverage().c_str() << " " 
     	  << "WaitForWork = " << tWaitForWorkG.formatAverage().c_str() 
     	  << "Outside = " << tOutsideG.formatAverage().c_str() 
     	  << LogIO::POST;
  }
  //
  //---------------------------------------------------------------------------------------
  //
  void MultiThreadedVisibilityResampler::initializePutBuffers(const Array<DComplex>& griddedData,
							      const Matrix<Double>& sumwt)
  {
    LogIO log_p(LogOrigin("MultiThreadedVisibilityResampler","initializeBuffers"));
    //    if ((nelements() > 1))
      {
    	//	if (threadClerk_p) {delete threadClerk_p; threadClerk_p=NULL;}
    	Double totalMem=0;
    	// if (!threadClerk_p) threadClerk_p = new ThreadCoordinator<Int>(nelements());
    	for(Int i=0; i<nelements(); i++)
    	  {
    	    // Resize and copy The following commented code attempts
	    // to defeat the problem of false sharing, should it be
	    // the bottlenec.
	    // {
	    //   Vector<Array<DComplex>* > tmp(5);
	    //   for (Int j=0;j<5;j++) 
	    // 	{
	    // 	  (tmp[j]) = new Array<DComplex>;
	    // 	  (tmp[j])->assign(griddedData);
	    // 	}
	    //   (*doubleGriddedData_p[i]).assign(griddedData);
	    //   for (Int j=0;j<5;j++) delete tmp[j];
	    // }

	    // The following code relies on system memalloc
	    (*doubleGriddedData_p[i]).assign(griddedData);
    	    (*sumwt_p[i]).assign(sumwt);
	    if (!threadStarted_p)
	      {
		totalMem += (*doubleGriddedData_p[i]).size()*sizeof(DComplex);
		totalMem += (*sumwt_p[i]).size()*sizeof(Double);
	      }
    	  }
    	if (!threadStarted_p) 
	  log_p << "Total memory used in buffers:" << totalMem/(1024*1024) << " MB" << LogIO::POST;
	// for (Int i=0;i<nelements(); i++)
	//   log_p << "Activating worklet " 
	// 	<< "# "     << (*gridderWorklets_p[i]).getID()  << ". " 
	// 	<< "PID = " << (*gridderWorklets_p[i]).getPID() << " "
	// 	<< "TID = " << (*gridderWorklets_p[i]).getTID() 
	// 	<< LogIO::POST;
	//	threadStarted_p = true;
      }
  }
  //
  //---------------------------------------------------------------------------------------
  //
  void MultiThreadedVisibilityResampler::initializePutBuffers(const Array<Complex>& griddedData,
							      const Matrix<Double>& sumwt)
  {
    LogIO log_p(LogOrigin("MultiThreadedVisibilityResampler", "initializePutBuffers(Single)"));
    //    if (nelements() > 1)
      {
	for(Int i=0; i<nelements(); i++)
	  {
	    (*singleGriddedData_p[i]).assign(griddedData);
	    (*sumwt_p[i]).assign(sumwt);
	  }
      }
  }
  //
  //------------------------------------------------------------------------------
  //
  // Re-sample the griddedData on the VisBuffer (a.k.a gridding).
  //
  // Make the following four methods via templated implementation.
  template <class T>
  void MultiThreadedVisibilityResampler::DataToGridImpl_p(Array<T>& /*griddedData*/,  
							  VBStore& vbs, 
							  Matrix<Double>& /*sumwt*/,
							  const Bool& dopsf,
							  Bool /*useConjFreqCF*/)
  {
    //    LogIO log_p(LogOrigin("MultiThreadedVisibilityResampler", "DataToGridImpl_p"));
    if (whoLoadedVB_p == MThWorkID::DATATOGRID)  {/*scatter(vbsVec_p,vbs); */whoLoadedVB_p = MThWorkID::DATATOGRID;}
    scatter(vbsVec_p,vbs); 

    // if (nelements() == 1)
    //   resamplers_p[0]->DataToGrid(griddedData, vbsVec_p(0,currentVBS_p), sumwt, dopsf);
    //   //      resamplers_p[0]->DataToGrid(*singleGriddedData_p[0], vbsVec_p(0,currentVBS_p),*sumwt_p[0] , dopsf);
    // else
      {
	//	Int workRequestDataToGrid=1;
	Int workRequestDataToGrid=MThWorkID::DATATOGRID;
	casa::utilj::ThreadTimes t1=casa::utilj::ThreadTimes::getTime();
	for(Int i=0; i < nelements(); i++) 
	  {
	    vbsVec_p(i,currentVBS_p).dopsf_p = dopsf;
	    if (doublePrecision_p) 
	      (*gridderWorklets_p[i]).initToSky(&vbsVec_p(i,currentVBS_p), 
						&(*doubleGriddedData_p[i]),
						&(*sumwt_p[i]));
	    else                    
	      {
		// cerr << &(*gridderWorklets_p[i]) << " "
		//      << &(*singleGriddedData_p[i]) << " " 
		//      << &(*sumwt_p[i]) << " " << i << endl;
		// if(gridderWorklets_p[i].null()) cerr << "worklet null" << endl;
		// if (singleGriddedData_p[i].null()) cerr << "data null" << endl;
		// if (sumwt_p[i].null()) cerr << "wt null" << endl;
		(*gridderWorklets_p[i]).initToSky(&vbsVec_p(i,currentVBS_p), 
						  &(*singleGriddedData_p[i]),
						  &(*sumwt_p[i]));
	      }
	  }

	casa::utilj::ThreadTimes t2=casa::utilj::ThreadTimes::getTime();
	//	threadClerk_p->getToWork(&workRequestDataToGrid);
	threadClerk_p->giveWorkToWorkers(&workRequestDataToGrid);
	casa::utilj::ThreadTimes t3=casa::utilj::ThreadTimes::getTime();
	threadClerk_p->waitForWorkersToFinishTask();
	casa::utilj::ThreadTimes t4=casa::utilj::ThreadTimes::getTime();
	
	 tSetupG       += t2-t1;
	 tSendDataG    += t3-t2;
	 tWaitForWorkG += t4-t3;
	 tOutsideG     += t1-t4G_p;
      }
      t4G_p = casa::utilj::ThreadTimes::getTime();
  }
  //
  //------------------------------------------------------------------------------
  //
  // Re-sample VisBuffer to a regular grid (griddedData) (a.k.a. de-gridding)
  // Still single threaded...
  void MultiThreadedVisibilityResampler::GridToData(VBStore& vbs, const Array<Complex>& griddedData) 
  {
    //    LogIO log_p(LogOrigin("MultiThreadedVisibilityResampler", "GridToData"));
    if (whoLoadedVB_p == MThWorkID::GRIDTODATA) {/*scatter(vbsVec_p,vbs);*/ whoLoadedVB_p = MThWorkID::GRIDTODATA;}
    scatter(vbsVec_p,vbs); 

    // if (nelements() == 1)
    //   resamplers_p[0]->GridToData(vbsVec_p(0,currentVBS_p),griddedData);
    // else
      {
	//    	Int workRequestDataToGrid=0;
    	Int workRequestDataToGrid=MThWorkID::GRIDTODATA;
	//	Timers t1=Timers::getTime();
	for(Int i=0; i < nelements(); i++) 
	  {
	    (*gridderWorklets_p[i]).initToVis(&vbsVec_p(i,currentVBS_p),&griddedData);
	  }
	//	Timers t2=Timers::getTime();
	//	threadClerk_p->getToWork(&workRequestDataToGrid);
	threadClerk_p->giveWorkToWorkers(&workRequestDataToGrid);
	//	Timers t3=Timers::getTime();
	threadClerk_p->waitForWorkersToFinishTask();
	//	Timers t4=Timers::getTime();
	// tSetupDG += t2-t1;
	// tSendDataDG += t3-t2;
	// tWaitForWorkDG += t4-t3;
	// tOutsideDG += t1-t4DG_p;
      }
      //      t4DG_p = Timers::getTime();
  }
  //
  //------------------------------------------------------------------------------
  //
  void MultiThreadedVisibilityResampler::ComputeResiduals(VBStore& vbs)
  {
    if (whoLoadedVB_p == MThWorkID::RESIDUALCALC) {/*scatter(vbsVec_p,vbs);*/ whoLoadedVB_p = MThWorkID::RESIDUALCALC;}
    scatter(vbsVec_p,vbs); 

    // if (nelements() == 1)
    //   resamplers_p[0]->ComputeResiduals(vbsVec_p(0,currentVBS_p));
    // else
      {
	//	Int workRequested=2;
	Int workRequested=MThWorkID::RESIDUALCALC;
	for (Int i=0; i<nelements(); i++)
	  (*gridderWorklets_p[i]).initToVis(&vbsVec_p(i,currentVBS_p),NULL);
	threadClerk_p->giveWorkToWorkers(&workRequested);
	threadClerk_p->waitForWorkersToFinishTask();
      }
  }

};
