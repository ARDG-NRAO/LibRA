// -*- C++ -*-
//# TheadCoordinator.h: Definition of the ThreadCoordinator class
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

#ifndef ROADRUNNER_THREADCOORDINATOR_H
#define ROADRUNNER_THREADCOORDINATOR_H

#include <mutex>
#include <future>
#include <thread>
#include <condition_variable>
#include <functional>
using namespace std;
// using namespace std::this_thread; // sleep_for, sleep_until
// using namespace std::chrono;      // nanoseconds, system_clock, seconds

class ThreadCoordinator
{
public:
  ThreadCoordinator():
    eod(false), RS_ready(false), CF_ready(false), CF_Sent(false),newCF(false),
    dcfa_sptr(), RS_mut(), CF_mut(), CFSent_mut(), RS_cv(), CF_cv(), CFSent_cv()
  {}
  ~ThreadCoordinator() {};

  bool waitForCFReady_or_EoD()
  {
    if (eod) return eod;
    std::unique_lock<std::mutex> lok(CF_mut);
    if (!CF_ready) cerr << "...........Main thread waiting for CFReady..........." << CF_ready << " " << eod << endl;
    CF_cv.wait(lok, [this]{return CF_ready||eod;});
    //CF_ready = false;
    return eod;
  }
  bool waitForCFSent()
  {
    std::unique_lock<std::mutex> lok(CFSent_mut);
    CFSent_cv.wait(lok, [this]{return CF_Sent;});
    CF_Sent=false;
    return CF_Sent;
  }
  bool waitForGridderReady()
  {
    std::unique_lock<std::mutex> lok(RS_mut);
    RS_cv.wait(lok, [this]{return RS_ready;});
    RS_ready=false;
    return RS_ready;
  }

  void setCFSent(bool val)
  {
    std::lock_guard<std::mutex> k(CFSent_mut);
    CF_Sent=val;
  }
  void setCFReady(bool val)
  {
    std::lock_guard<std::mutex> k(CF_mut);
    CF_ready=val;
  }
  void setRSReady(bool val)
  {
    std::lock_guard<std::mutex> k(RS_mut);
    RS_ready=val;
  }
  void setEoD(bool val)
  {
    std::lock_guard<std::mutex> k(CF_mut);
    eod=val;
  }

  bool isEoD()
  {
    std::lock_guard<std::mutex> k(CF_mut);
    return eod;
  }
  void Notify_CFSent()
  {
    CFSent_cv.notify_one();
  }
  void Notify_RSReady()
  {
      RS_cv.notify_one();
  }
  void Notify_CFReady()
  {
    CF_cv.notify_one();
  }
  void setDCFA(std::shared_ptr<hpg::DeviceCFArray>& val)
  {
    dcfa_sptr = val;
  }

  bool eod;
  bool RS_ready;
  bool CF_ready;
  bool CF_Sent;
  bool newCF;
  std::shared_ptr<hpg::DeviceCFArray> dcfa_sptr;

private:  
  std::mutex RS_mut;
  std::mutex CF_mut;
  std::mutex CFSent_mut;
  std::condition_variable RS_cv;
  std::condition_variable CF_cv;
  std::condition_variable CFSent_cv;

};
#endif
