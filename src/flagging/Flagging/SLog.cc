//# SLog.cc: 
//# Copyright (C) 2007
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
//#
//# -------------------------------------------------------------------------


#include <flagging/Flagging/SLog.h>
    
#define LOGLEVEL 3
//debuglevel{DEBUGGING, DEBUG2, DEBUG1, NORMAL5, ..., WARN, SEVERE}
//  loglevel{LOG0,      LOG1,   LOG2,   LOG3,    ..., LOG9, LOG10}
//set LOGLEVEL to 3 to filter out all debug message
  
using namespace casacore;
namespace casa {

SLog* SLog::instance = 0;
//uInt SLog::refCount = 0;

SLog* SLog::slog() {
   if (!instance) {
      instance = new SLog();
   }
   //refCount++;
   //cout << "slog::refCount=" << refCount << endl;   
   return instance;
}

SLog::~SLog() {
   out("destroy logger");

   //generally speaking, there is no need to destruct
   //SLog because it has at most 1 real object, others
   //are just pointers. The real log object will be 
   //removed when the whole process is done.
   //cout << "----------------- destroy slog---------" << endl; 
   //delete instance;
   //instance = 0;
   //this destruct never get called anyway
  
}
    
SLog::SLog() {
   out("create logger");
}

void SLog::FnEnter(String fnname, String clname) {
   out("---->> Enter", fnname, clname, LogMessage::DEBUG1);
}
    
void SLog::FnExit(String fnname, String clname) {
   out("<<---- Exit", fnname, clname, LogMessage::DEBUG1);
}

void SLog::FnPass(String fnname, String clname) {
   out("-- Pass --", fnname, clname, LogMessage::DEBUG1);
}

void SLog::out(const String &msg, const String& /*fnname*/,
               const String& /*clname*/,
	       LogMessage::Priority msglevel,
	       Bool onconsole) {
   if (msglevel >= LOGLEVEL) {
      priority(msglevel);
      output() << (const char *)msg.c_str();
      post();
      if (onconsole) {
         cout << LogMessage::toString(msglevel)
             // << " from " << clname << "::" << fnname
             // << " ==>> " 
              << " " << msg << endl;
      }	
   }
}

};

