//# Applicator.h: interface to parallelization infrastructure
//# Copyright (C) 1999,2000
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
//#
//# $Id$

#ifndef SYNTHESIS_APPLICATOR_H
#define SYNTHESIS_APPLICATOR_H

//# Includes

#include <map>
#include <casa/aips.h>
#include <casa/Arrays/Vector.h>
#include <lattices/Lattices/Lattice.h>
#include <casa/Containers/Record.h>
#include <synthesis/Parallel/PTransport.h>

namespace casa { //# NAMESPACE CASA - BEGIN

//# Forward Declarations
class Algorithm;

// <summary>
// Class which provides an interface to the parallelization infrastructure
// </summary>

// <use visibility=local>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> Algorithm
//   <li> PTransport
// </prerequisite>
//
// <etymology>
// Applies or controls the execution of parallelized algorithms.
// </etymology>
//
// <synopsis>
// The Applicator class provides the interface to parallel communication.
// It holds the parallel transport layer, and controls the execution of
// parallelized algorithms.
// </synopsis>
//
// <example>
// </example>
//
// <motivation>
// To provide a simple programming interface to parallelization, and
// to encapsulate the transport layer and parallel process control.
// </motivation>
//
//# <thrown>
//#    <li>
//#    <li>
//# </thrown>
//
//# <todo asof="1999/12/21">
//#   <li> Document
//# </todo>


class Applicator {
 public:
  // Enum to define the process status table
  enum Status {FREE, ASSIGNED};

  // Recognized signals
  enum Signals {STOP = 0, DONE = -1};

  // Default constructor, and destructor
  Applicator();
  ~Applicator();

  // Initialization (includes parallel transport initialization)
  void init(casacore::Int argc, casacore::Char *argv[]);
  void initThreads(casacore::Int argc, casacore::Char *argv[]);
  void initThreads();
  // release workers from loop
  void destroyThreads();
  // tells you if initThreads(argc,  argv) version has been called already
  // MPI does not like more than 1 call to MPI_Init or MPI_InitThread
  bool initialized();
  // define an Algorithm if we need too; takes ownership of the alg pointer
  void defineAlgorithm(Algorithm *);

  // Status functions to indicate whether this Applicator is
  // executing as a controller or worker process
  casacore::Bool isController();
  casacore::Bool isWorker();

  // true if executing serially
  casacore::Bool isSerial() {return serial;};

  // Return the number of processes
  casacore::Int numProcs() {return nProcs;};

  // Assign the next free worker process to a specified Algorithm
  casacore::Bool nextAvailProcess(Algorithm &a, casacore::Int &rank);

  // Return the rank of the next process to complete the specified Algorithm
  casacore::Int nextProcessDone(Algorithm &a, casacore::Bool &allDone);

  // Signal that a worker process is done
  void done();

  // Execute an algorithm directly
  void apply(Algorithm &a);

  // Put and get methods to be executed on the parallel transport layer
  casacore::Int put(const casacore::Array<casacore::Float> &an) {return comm->put(an);};
  casacore::Int put(const casacore::Array<casacore::Double> &an) {return comm->put(an);};
  casacore::Int put(const casacore::Array<casacore::Int> &an) {return comm->put(an);};
  casacore::Int put(const casacore::Array<casacore::Complex> &an) {return comm->put(an);};
  casacore::Int put(const casacore::Array<casacore::DComplex> &an) {return comm->put(an);};
  casacore::Int put(const casacore::Float &n) {return comm->put(n);};
  casacore::Int put(const casacore::Complex &n) {return comm->put(n);};
  casacore::Int put(const casacore::DComplex &n) {return comm->put(n);};
  casacore::Int put(const casacore::Double &n) {return comm->put(n);};
  casacore::Int put(const casacore::Int &n) {return comm->put(n);};
  casacore::Int put(const casacore::Bool &b) {return comm->put(b);};
  casacore::Int put(const casacore::String &s) {return comm->put(s);};
  casacore::Int put(const casacore::Record &r) {return comm->put(r);};

  casacore::Int get(casacore::Array<casacore::Float> &an) {return comm->get(an);};
  casacore::Int get(casacore::Array<casacore::Double> &an) {return comm->get(an);};
  casacore::Int get(casacore::Array<casacore::Complex> &an) {return comm->get(an);};
  casacore::Int get(casacore::Array<casacore::DComplex> &an) {return comm->get(an);};
  casacore::Int get(casacore::Array<casacore::Int> &an) {return comm->get(an);};
  casacore::Int get(casacore::Float &n) {return comm->get(n);};
  casacore::Int get(casacore::Double &n) {return comm->get(n);};
  casacore::Int get(casacore::Complex &n) {return comm->get(n);};
  casacore::Int get(casacore::DComplex &n) {return comm->get(n);};
  casacore::Int get(casacore::Int &n) {return comm->get(n);};
  casacore::Int get(casacore::Bool &b) {return comm->get(b);};
  casacore::Int get(casacore::String &s) {return comm->get(s);};
  casacore::Int get(casacore::Record &r) {return comm->get(r);};

 private:
  // Pointer to the parallel transport
  PTransport *comm;

  // casacore::Map of known algorithm names and id.'s
  std::map<casacore::String, casacore::Int> algorithmIds;
  std::map<casacore::Int, Algorithm*> knownAlgorithms;

  // ID for the last Algorithm defined.
  casacore::Int LastID;

  // true if no more processes are free
  casacore::Bool usedAllThreads;

  // true if executing in serial
  casacore::Bool serial;

  // Number of processes
  casacore::Int nProcs;

  // Process status list
  casacore::Vector<casacore::Int> procStatus;

  // Executed by worker process waiting for an assigned task
  void loop();

  // Fill algorithm map
  void defineAlgorithms();

  // Utility functions for the current list of processes, and their status
  void setupProcStatus();
  casacore::Int findFreeProc(casacore::Bool &lastOne);

  bool debug_p = false;
  bool initialized_p;
  casacore::Int donesig_p; //have to keep in context in serial case
};


} //# NAMESPACE CASA - END

#endif




