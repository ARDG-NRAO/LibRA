// -*- C++ -*-
//# AppRC.h: Definition of the AppRC class
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
#ifndef SYNTHESIS_APPRC_H
#define SYNTHESIS_APPRC_H

#include <casa/aips.h>
#include <casa/System/Casarc.h>
#include <casa/sstream.h>
#include <iostream>
#include <unistd.h>

using namespace std;

namespace casa { //# NAMESPACE CASA - BEGIN
  class AppRC
  {
  public:
    AppRC()
      :rc_p(NULL), addPID_p(false), deleteFile_p(false),
       id_p()
    {init("", addPID_p, deleteFile_p);};
    
    AppRC(const string& filename, 
	  const casacore::Bool addPID=false,
	  const casacore::Bool deleteFile=false)
      :rc_p(NULL), deleteFile_p(deleteFile), id_p()
    {init(filename,addPID,deleteFile_p);};
    
    ~AppRC();
 
    ///The environment variable is assumed  in MB if that is used
    //returns Bytes available (reason why it is a double)
    static casacore::Double getMemoryAvailable(string envVarName="CASA_MAX_MEMORY");
    void setID(const string& id) {id_p=id;};
    void init(const string& filename, const casacore::Bool addPID=false, 
	      const casacore::Bool deleteFile=true);

    string get(const string& name);
    string get(const string& name, casacore::Int& val);
    string get(const string& name, casacore::Float& val);
    string get(const string& name, casacore::Double& val);
    void put(const string& name, const string& val);
    void put(const string& name, const casacore::Int& val);
    void put(const string& name, const casacore::Float& val);
    void put(const string& name, const casacore::Double& val);
    
  private:
    casacore::Casarc *rc_p;
    casacore::CasarcCleanup rcCleanup;
    pid_t myPID_p /*, myTID_p*/;
    casacore::Bool addPID_p,deleteFile_p;
    string fileName_p, id_p;
    void setPID() {/*myTID_p = gettid_p ();*/ myPID_p = getpid ();}
  };
  
}; //# NAMESPACE CASA - END

#endif // 
