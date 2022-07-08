//# PlotOperation.h: Classes for managing large, threaded plot operations.
//# Copyright (C) 2009
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
//# $Id: $
#ifndef PLOTOPERATION_H_
#define PLOTOPERATION_H_

#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/CountedPtr.h>

#include <vector>

namespace casa {

//# Forward Declarations
class PlotOperationWatcher;


// Abstract class, for a synchronization mutex specific to the threading
// library used by the plotting implementations.
class PlotMutex {
public:
    // Constructor.
    PlotMutex() { }
    
    // Destructor.
    virtual ~PlotMutex() { }
    
    
    // ABSTRACT METHODS //
    
    // Locks the mutex.
    virtual void lock() = 0;
    
    // Unlocks the mutex.
    virtual void unlock() = 0;
    
    // Tries to lock the mutex, and returns immediately with either true for
    // success (mutex is now locked), or false (mutex is unavailable).
    virtual bool tryLock() = 0;
};
typedef casacore::CountedPtr<PlotMutex> PlotMutexPtr;


// Simple object to synchronize operation progress information across threads.
class PlotOperation {    
public:
    // Constructor which takes the operation name and a synchronization mutex.
    PlotOperation(const casacore::String& name, PlotMutexPtr mutex);
    
    // Destructor.
    ~PlotOperation();
    
    
    // Accessors (synchronized).
    // <group>
    casacore::String name() const;    
    bool inProgress() const;    
    bool isFinished() const;    
    unsigned int currentProgress() const;    
    casacore::String currentStatus() const;
    bool cancelRequested() const;
    // </group>
    
    // Mutators (synchronized).  Any mutator will notify registered watchers of
    // a change.
    // <group>
    void setInProgress(bool inProgress);
    void setIsFinished(bool isFinished);
    void setCurrentProgress(unsigned int currentProgress);
    void setCurrentStatus(const casacore::String& currentStatus);
    void setCancelRequested(bool cancel);
    // </group>
    
    // Sets the operation's mutex to the given.
    void setMutex(PlotMutexPtr mutex);
    
    // Adds the given watcher for this object.
    void addWatcher(PlotOperationWatcher* watcher);
    
    // Removes the given watcher for this object.
    void removeWatcher(PlotOperationWatcher* watcher);
    
    // Resets the in progress, is finished, current progress, and current
    // status members.  Will notify registered watchers of a change.
    void reset();
    
    // Updates in progress, is finished, current progress, and current status
    // members to reflect a "finished" state.  Will notify registered watchers
    // of a change.
    void finish();
    
private:    
    // Name.
    casacore::String m_name;
    
    // Flags.
    bool m_inProgress, m_isFinished;
    
    // Current progress (0 - 100)%.
    unsigned int m_currentProgress;
    
    // Current status message.
    casacore::String m_currentStatus;
    
    // Cancel requested flag.
    bool m_cancelRequested;
    
    // Synchronization mutex.
    PlotMutexPtr m_mutex;
    
    // Watchers.
    std::vector<PlotOperationWatcher*> m_watchers;
    
    
    // Notifies any registered watchers that the operation has changed.
    void notifyWatchers() const;
};
typedef casacore::CountedPtr<PlotOperation> PlotOperationPtr;


// Abstract interface for any object that wants to watch a PlotOperation object
// for changes.
class PlotOperationWatcher {
public:
    // Constructor.
    PlotOperationWatcher() { }
    
    // Destructor.
    virtual ~PlotOperationWatcher() { }
    
    
    // This method is called to notify the watcher that the given PlotOperation
    // object has changed.
    virtual void operationChanged(const PlotOperation& operation) = 0;
};

}

#endif /* PLOTOPERATION_H_ */
