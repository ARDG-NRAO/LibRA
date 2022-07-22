//# Applicator.cc: Implementation of Applicator.h
//# Copyright (C) 1999,2000,2002
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

#include <casa/Utilities/Assert.h>

#include <synthesis/Parallel/Applicator.h>
#include <synthesis/Parallel/MPITransport.h>
#include <synthesis/Parallel/SerialTransport.h>
#include <synthesis/Parallel/Algorithm.h>
#include <synthesis/MeasurementComponents/ClarkCleanAlgorithm.h>
#include <synthesis/MeasurementComponents/ReadMSAlgorithm.h>
#include <synthesis/MeasurementComponents/MakeApproxPSFAlgorithm.h>
#include <synthesis/MeasurementComponents/PredictAlgorithm.h>
#include <synthesis/MeasurementComponents/ResidualAlgorithm.h>
#include <synthesis/ImagerObjects/CubeMajorCycleAlgorithm.h>
#include <synthesis/ImagerObjects/CubeMakeImageAlgorithm.h>
#include <synthesis/ImagerObjects/CubeMinorCycleAlgorithm.h>
#include <synthesis/Parallel/MPIError.h>

using namespace casacore;
using namespace std;
namespace casa { //# NAMESPACE CASA - BEGIN

Applicator::Applicator() : comm(0), algorithmIds( ),
  knownAlgorithms( ), LastID(101), usedAllThreads(false),
                           serial(true), nProcs(0), procStatus(0), initialized_p(false)
{
// Default constructor; requires later init().
}

Applicator::~Applicator()
{
// Default destructor
//  
  if (comm) {
    // If controller, then stop all worker processes
    if (isController() && !(comm->isFinalized())) {
      comm->setTag(STOP);
      for (Int i=0; i<nProcs; i++) {
	if (i != comm->controllerRank()) {
	  comm->connect(i);
	  put(STOP);
	}
      }
    }
    delete comm;
  }

  for (auto &algo : knownAlgorithms) {
      delete algo.second;
  }
}

void Applicator::initThreads(Int argc, Char *argv[]){

   // A no-op if not using MPI
#ifdef HAVE_MPI
  //if (debug_p) {
  if(initialized_p) return;
  //  cerr << "In initThreads. argc: " << argc << ", argv: " << argv << '\n';
      //}
  // Initialize the MPI transport layer
  try {
     comm = new MPITransport(argc, argv);

     // Initialize the process status list
     setupProcStatus();

     // If controller then exit, else loop, waiting for an assigned task
     if (isWorker()) {
       loop();
     }

  } catch (MPIError x) {
    cerr << x.getMesg() << " doing serial "<< endl;
    initThreads();
  } 

#else
  (void)argc;
  (void)argv;
  cerr << " doing serial "<< endl;
  initThreads();
#endif
}

   // Serial transport all around.
void Applicator::initThreads(){
     // Initialize a serial transport layer
  comm = new SerialTransport();
     // Initialize the process status list
  setupProcStatus();
}
void Applicator::destroyThreads(){
  if(initialized_p){
    if (comm) {
    // If controller, then stop all worker processes
      if (isController() && !isSerial() && !(comm->isFinalized())) {
              //comm->setTag(STOP);
	for (Int i=0; i<nProcs; i++) {
	  if (i != comm->controllerRank()) {
	    comm->connect(i);
	    comm->setTag(STOP);
	    put(STOP);

	  }
	}
      }
      //delete comm; ///leaking this for now as if initialized from python..it brings down the whole house
      //comm=nullptr;
    }

  }

}
void Applicator::init(Int argc, Char *argv[])
{
// Initialize the process and parallel transport layer
//
  //cerr <<"Applicatorinit " << initialized_p << endl;
  if(comm){
    //if worker  was released from loop...want it back now
    if(comm && isWorker() && !isSerial())
      loop();
    return;
  }
  // Fill the map of known algorithms
  //cerr << "APPINIT defining algorithms " << endl;
  defineAlgorithms();

#ifdef HAVE_MPI
  if (debug_p) {
     cerr << "In init threads, HAVE_MPI...\n";
  }
  initThreads(argc, argv);
#else
  if (debug_p) {
      cerr << "In init threads, not HAVE_MPI...\n";
  }
  (void)argc;
  (void)argv;
  initThreads();
#endif
  initialized_p=true;
  return;
}

Bool Applicator::isController()
{
// Return T if the current process is the controller
//
  Bool result;
  if (comm) {
    result = comm->isController();
  } else {
    throw(AipsError("Parallel transport layer not initialized"));
  }
  return result;
}

Bool Applicator::isWorker()
{
// Return T if the current process is a worker process
//
  Bool result;
  if (comm) {
    result = comm->isWorker();
  } else {
    throw(AipsError("Parallel transport layer not initialized"));
  }
  return result;
}

void Applicator::loop()
{
// Loop, if a worker process, waiting for an assigned task
//
  Bool die(false);
  Int what;
  // Wait for a message from the controller with any Algorithm tag
  while(!die){
    comm->connectToController();
    comm->setAnyTag();
    //cerr << "in loop get" << endl;
    comm->get(what);
    if (debug_p) {
        cerr << "worker, got what (algID/stop): " << what << endl;
    }
    switch(what){
    case STOP :
      die = true;
      break;
    default :
      // In this case, an Algorithm tag is expected.
      // First check that it is known.
      if (knownAlgorithms.find(what) != knownAlgorithms.end( )) {
	// Identified algorithm tag; set for subsequent communication
	comm->setTag(what);
	// Execute (apply) the algorithm
	knownAlgorithms.at(what)->apply();
      } else {
	throw(AipsError("Unidentified parallel algorithm code"));
      }
      break;
    }
  }
  //cerr <<"getting out of loop " <<endl;
  return;
}

Bool Applicator::nextAvailProcess(Algorithm &a, Int &rank)
{
// Assign the next available process for the specified Algorithm
//  
  // Must be the controller to request a worker process
  Bool assigned=False;
  if (isWorker()) {
    throw(AipsError("Must be the controller to assign a worker process"));
  } else {
    if (!usedAllThreads) {
      // Connect to the next available process in the list
      Bool lastOne;
      rank = findFreeProc(lastOne);
      AlwaysAssert(rank >= 0, AipsError);
      if (lastOne) usedAllThreads = true;
      Int tag = algorithmIds.find(a.name()) == algorithmIds.end( ) ? 0 : algorithmIds.at(a.name());
      
      // Send wake-up message (containing the Algorithm tag) to
      // the assigned worker process to activate it (see loop()).
      comm->connect(rank);
      comm->setTag(tag);
      //cerr << "nextAvailproc settag " << tag << " rank " << rank << " name " << a.name() << endl;
      put(tag);
      /*
      if (not isWorker() and numProcs() <= 1){
      // the first int, algID, is consumed in the loop for the workers when running
      // in multiprocess mode and there are at least 2 processes. When not multiprocess or a
      // single process, we need to consume it:
      // TODO - it could be consumed up here, right after the put()
      int algID;
      comm->get(algID);
    if (debug_p) {
      cerr << "nextAvailproc controller, got algID: " << algID << " assigned " << assigned << " donesig " << donesig_p<<  endl;
     }
      }
      */
      assigned = true;
      procStatus(rank) = ASSIGNED;
    } else {
      assigned = false;
    }
  }
  //cerr << "nextAvailproc controller assigned " << assigned << endl;
  
  if ((!isWorker()) && (numProcs() <= 1) && assigned){
      // the first int, algID, is consumed in the loop for the workers when running
      // in multiprocess mode and there are at least 2 processes. When not multiprocess or a
      // single process, we need to consume it:
      // TODO - it could be consumed up here, right after the put()
      Int algID;
      //comm->get(algID);
      get(algID);
    if (debug_p) {
      cerr << "nextAvailproc controller, got algID: " << algID << " assigned " << assigned << " donesig " << donesig_p<<  endl;
     }
  }
  
  return assigned;
}

bool Applicator::initialized(){
#ifdef HAVE_MPI
  return initialized_p;  
#endif  
  
  return false;
}
Int Applicator::nextProcessDone(Algorithm &a, Bool &allDone)
{
// Return the rank of the next process to complete the specified algorithm
//
  Int rank = -1;
  allDone = true;
  //cerr << "nextprocess done procstatus " << procStatus << endl;
  for (uInt i=0; i<procStatus.nelements(); i++) {
    if (procStatus(i) == ASSIGNED) {
      if (isSerial()) {
	// In the serial case, the controller can be assigned
	allDone = false;
      } else {
	// In the parallel case, the controller is not assigned
	if (i != static_cast<uInt>(comm->controllerRank())) {
	  allDone = false;
	}
      }
    }
  }
  if (!allDone) {
    // Wait for a process to finish with the correct algorithm tag
    comm->connectAnySource();
    Int tag = algorithmIds.find(a.name()) == algorithmIds.end( ) ? 0 : algorithmIds.at(a.name());
    //cerr <<"procdone name" << a.name() << " id " << tag << endl;
    comm->setTag(tag);
    Int doneSignal;
    rank = get(doneSignal);
    //cerr <<" procdone rank " << rank << " donesig " << doneSignal << endl;
    // Consistency check; should return a DONE signal to contoller
    // on completion.
    if (doneSignal != DONE) {
      throw(AipsError("Worker process terminated unexpectedly"));
    } else {
      // Set source in parallel transport layer
      comm->connect(rank);
      // Mark process as free
      procStatus(rank) = FREE;
      //cerr << "NEXTProcDone connect rank" << rank << " procstat " << procStatus << endl; 
      usedAllThreads = false;
    }
  }
  return rank;
}

void Applicator::done()
{
// Signal that a worker process is done
//
  donesig_p=DONE;
  Int donesig=DONE;
  if(isSerial())
    put(donesig_p);
  else
    put(donesig);
  return;
}

void Applicator::apply(Algorithm &a)
{
// Execute an algorithm directly
//
  // Null operation unless serial, in which case the 
  // controller needs to execute the algorithm directly.
  // In the parallel case, the algorithm applies are
  // performed in workers processes' applicator.init().
  donesig_p=10000;
  if (isSerial() && isController()) {
    a.apply();
  }
  return;
}

void Applicator::defineAlgorithm(Algorithm *a)
{
  //no need to add if it is already defined
  //  if(algorithmIds.count(a->name()) <1){
  //knownAlgorithms.insert( std::pair<casacore::Int,Algorithm*>(LastID, a) );
  // algorithmIds.insert( std::pair<casacore::String, casacore::Int>(a->name(), LastID) );
  Int theid=LastID;
  if(algorithmIds.count(a->name()) >0){
    theid=algorithmIds[a->name()];
  }
  else{
    theid=LastID;
    algorithmIds[a->name()]=LastID;
    ++LastID;
  }
  knownAlgorithms[theid]=a;
   // }
   return;
}

void Applicator::defineAlgorithms()
{
// Fill the algorithm map
//
  // Clark CLEAN parallel deconvolution
  Algorithm *a1 = new ClarkCleanAlgorithm;
  knownAlgorithms.insert( std::pair<casacore::Int, Algorithm*>(LastID, a1) );
  algorithmIds.insert( std::pair<casacore::String, casacore::Int>(a1->name(), LastID) );
  LastID++;
  Algorithm *a2 = new ReadMSAlgorithm;
  knownAlgorithms.insert( std::pair<casacore::Int, Algorithm*>(LastID, a2) );
  algorithmIds.insert( std::pair<casacore::String, casacore::Int>(a2->name(), LastID) );
  LastID++;
  Algorithm *a3 = new MakeApproxPSFAlgorithm;
  knownAlgorithms.insert( std::pair<casacore::Int, Algorithm*>(LastID, a3) );
  algorithmIds.insert( std::pair<casacore::String, casacore::Int>(a3->name(), LastID) );
  LastID++;
  Algorithm *a4 = new PredictAlgorithm;
  knownAlgorithms.insert( std::pair<casacore::Int, Algorithm*>(LastID, a4) );
  algorithmIds.insert( std::pair<casacore::String, casacore::Int>(a4->name(), LastID) );
  LastID++;
  Algorithm *a5 = new ResidualAlgorithm;
  knownAlgorithms.insert( std::pair<casacore::Int, Algorithm*>(LastID, a5) );
  algorithmIds.insert( std::pair<casacore::String, casacore::Int>(a5->name(), LastID) );
  LastID++;
  Algorithm *a6 = new CubeMajorCycleAlgorithm;
  knownAlgorithms.insert( std::pair<casacore::Int, Algorithm*>(LastID, a6) );
  algorithmIds.insert( std::pair<casacore::String, casacore::Int>(a6->name(), LastID) );
  LastID++;
  Algorithm *a7 = new CubeMakeImageAlgorithm;
  knownAlgorithms.insert( std::pair<casacore::Int, Algorithm*>(LastID, a7) );
  algorithmIds.insert( std::pair<casacore::String, casacore::Int>(a7->name(), LastID) );
  LastID++;
  Algorithm *a8 = new CubeMinorCycleAlgorithm;
  knownAlgorithms.insert( std::pair<casacore::Int, Algorithm*>(LastID, a8) );
  algorithmIds.insert( std::pair<casacore::String, casacore::Int>(a8->name(), LastID) );
  LastID++;
  return;
}

void Applicator::setupProcStatus()
{
// Set up the process status list
//
  nProcs = comm->numThreads();
  if (nProcs <= 1) {
    serial = true;
  } else {
    serial = false;
  }
  // Resize the process list, and mark as unassigned (except for controller)
  usedAllThreads = false;
  procStatus.resize(max(nProcs,1));
  procStatus = FREE;
  // In the parallel case, the controller is never assigned
  if (!isSerial())
      procStatus(comm->controllerRank()) = ASSIGNED;
}

Int Applicator::findFreeProc(Bool &lastOne)
{
// Search the process status list for the next free process
// 
  Int freeProc = -1;
  Int nfree = 0;
 
  for (uInt i=0; i<procStatus.nelements(); i++) {
    if (procStatus(i) == FREE) {
      nfree++;
      if (freeProc < 0) freeProc = i;
    }
  }
  lastOne = (nfree==1);
  //cerr <<"FreeProc procstat "<< procStatus << " nfree " << nfree << endl; 
  return freeProc;
}

// The applicator is ominpresent.
// Moved here for shared libraries.
Applicator applicator;


} //# NAMESPACE CASA - END

