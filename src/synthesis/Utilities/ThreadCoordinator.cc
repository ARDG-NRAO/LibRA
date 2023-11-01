// -*- C++ -*-
//# ThreadCoordinator.cc: Implementation of the ThreadCoordinator class
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

#include <synthesis/Utilities/ThreadCoordinator.h>
#include <stdcasa/thread/AsynchronousTools.h>
#include <cassert>
#include <stdcasa/thread/Barrier.h>


#define Log(level, ...) \
    {Logger::get()->log (__VA_ARGS__);};

using namespace casacore;
using namespace casa::async;

using namespace casacore;
namespace casa {

ThreadCoordinatorBase::ThreadCoordinatorBase (Int nThreads, bool logStates)
  : nThreads_p (nThreads),
    barrier_p (new Barrier(nThreads)),
    logStates_p (logStates),
    mutex_p (new async::Mutex()),
    nThreadsAtBarrier_p (0),
    nThreadsDispatched_p (0),
    readyForWork_p (false),
    stateChanged_p (new Condition ()),
    workCompleted_p (false),
    workToBeDone_p (false)
{}

ThreadCoordinatorBase::~ThreadCoordinatorBase ()
{
    delete mutex_p;
    delete stateChanged_p;
}

void
ThreadCoordinatorBase::dispatchWork ()
{
  MutexLocker ml (* mutex_p);

  installWorkInfo (); // have subclass install work info for distribution

  workToBeDone_p = true;
  workCompleted_p = false;
  stateChanged_p->notify_all ();
}

void
ThreadCoordinatorBase::getToWork ()
{

  waitForWorkersToReport ();

  // Direct the workers to do another round of work

  dispatchWork ();

  // Wait for them to complete the current round of work

  waitForWorkersToFinishTask ();
}

void
ThreadCoordinatorBase::logState (const String & tag) const
{
    if (logStates_p){
        Log (1, "%s:\tready=%s\tworkToBeDone=%s\tworkCompleted=%s\n",
             tag.c_str(),
             readyForWork_p ? "T" : "F",
             workToBeDone_p ? "T" : "F",
             workCompleted_p ? "T" : "F");
    }
}

Bool
ThreadCoordinatorBase::waitForWork (const Thread * thisThread)
{

  // Decrement the working thread counter.  If it's now zero then
  // clear the workToBeDone flag and signal a stateChange.

  //  logState ("waitForWork (at barrier)");

  // Wait until all of the worker threads are done. This prevents an
  // eager beaver thread from trying to do two passes through the work
  // cycle while another has not been awoken.

  {
      MutexLocker ml (* mutex_p);
      ++ nThreadsAtBarrier_p;
      if (nThreadsAtBarrier_p == nThreads_p){
          workCompleted_p = true;
          nThreadsAtBarrier_p = 0;
          stateChanged_p->notify_all ();
      }
  }

  barrier_p->wait ();

  UniqueLock uniqueLock (* mutex_p);

  if (! readyForWork_p){
      readyForWork_p = true;
      stateChanged_p->notify_all ();
  }

  //  logState ("waitForWork (past barrier)");

  // Wait for the next bit of work

  while (! workToBeDone_p && ! thisThread->isTerminationRequested()){
    stateChanged_p->wait (uniqueLock);
  }

  ++ nThreadsDispatched_p;
  if (nThreadsDispatched_p == nThreads_p){
      nThreadsDispatched_p = 0;
      workToBeDone_p = false;
  }

  readyForWork_p = false;

  //  logState ("waitForWork (end wait)");

  return ! thisThread->isTerminationRequested();
}

void
ThreadCoordinatorBase::waitForWorkersToFinishTask ()
{
  UniqueLock uniqueLock (* mutex_p);

  // Wait for them to complete the current round of work

  //  logState ("Waiting for workers to finish task...");

  while (! workCompleted_p){
    stateChanged_p->wait (uniqueLock);
  }

  //  logState ("... workers have finished task.");

}

void
ThreadCoordinatorBase::waitForWorkersToReport ()
{
  UniqueLock uniqueLock (* mutex_p);

  // Wait for all of the worker threads to complete
  // the previous work.

  //  logState ("Waiting for workers to report ...");

  while (! readyForWork_p || workToBeDone_p){
    stateChanged_p->wait (uniqueLock);
  }
  //  logState ("... workers have reported");
}

using namespace casacore;
} // end namespace casa
