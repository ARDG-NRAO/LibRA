// -*- C++ -*-
//# ThreadCoordinator.h: Definition of the ThreadCoordinator class
//# Copyright (C) 2015
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
#ifndef STDCASA_THREAD_BARRIER_H_
#define STDCASA_THREAD_BARRIER_H_
#include <mutex>
#include <condition_variable>

namespace casa {
    class Barrier {
    private:
        std::mutex mutex_;
        std::condition_variable cvar_;
        std::size_t count_;
    public:
        explicit Barrier(std::size_t count) : count_{count} { }
        void wait( ) {
            std::unique_lock<std::mutex> lock(mutex_);
            if (--count_ == 0) {
                cvar_.notify_all();
            } else {
                cvar_.wait(lock, [this] { return count_ == 0; });
            }
        }
    };
}

#if 0
//
// could switch to this (from Jim) which allows for sequential applications of the barrier...
// after we switch to C++11 threads (from boost threads)...
//
class Barrier {
public:
    explicit Barrier(std::size_t iCount) : 
        mThreshold(iCount), 
        mCount(iCount), 
        mGeneration(0) {
    }

    void Wait() {
        auto lGen = mGeneration;
        std::unique_lock<std::mutex> lLock{mMutex};
        if (!--mCount) {
            mGeneration++;
            mCount = mThreshold;
            mCond.notify_all();
        } else {
            mCond.wait(lLock, [this, lGen] { return lGen != mGeneration; });
        }
    }

private:
    std::mutex mMutex;
    std::condition_variable mCond;
    std::size_t mThreshold;
    std::size_t mCount;
    std::size_t mGeneration;
};
#endif

#endif
