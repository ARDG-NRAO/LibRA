// -*- C++ -*-
//# AppRC.cc: Implementation of the AppRC class
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
#include <synthesis/Utilities/AppRC.h>
#include <casacore/casa/OS/RegularFile.h>
#include <casacore/casa/OS/HostInfo.h>
#include <synthesis/TransformMachines/Utils.h>
using namespace casacore;
namespace casa{
  //
  //--------------------------------------------------------------
  //
  AppRC::~AppRC() 
  {
    // if (deleteFile_p)
    //   {
    // 	RegularFile ff(fileName_p);
    // 	cerr << id_p << " " << fileName_p << endl;
    // 	if (ff.exists()) 
    // 	  {
    // 	    if (id_p != "") 
    // 	      cerr << id_p << " removing " << fileName_p << endl;
    // 	    ff.remove();
    // 	  }
    // 	else
    // 	  {
    // 	    if (id_p != "") 
    // 	      cerr << id_p << " did not find " << fileName_p << endl;
    // 	  }
    //   }

  };
  //
  //--------------------------------------------------------------
  //
  void AppRC::init(const string& filename, const Bool addPID,
		   const Bool deleteFile) 
  {
    addPID_p=addPID; deleteFile_p=deleteFile;
    if (rc_p == NULL)
      {
	ostringstream tt; 
	tt << filename;
	if (addPID_p) 
	  {
	    setPID();
	    tt << "_" << myPID_p;
	  }
	fileName_p=tt.str();
	//cerr << fileName_p <<endl; 
	if(fileName_p.size() >0){
	  Casarc::setDefaultPath(fileName_p);
	  rc_p = &Casarc::instance(fileName_p);
	}
	else
	  rc_p = &Casarc::instance();
	//cerr << rc_p->path() << endl;
      }
  };
  //
  //--------------------------------------------------------------
  //
  string AppRC::get(const string& name) 
  {return rc_p->get(name);}
  //
  //--------------------------------------------------------------
  //
  string AppRC::get(const string& name, Int& val) 
  {
    string strVal=rc_p->get(name); 
    if (strVal != "")
      val=atoi(strVal.c_str());
    return strVal;
  }
  //
  //--------------------------------------------------------------
  //
  string AppRC::get(const string& name, Float& val) 
  {
    string strVal=rc_p->get(name); 
    if (strVal != "")
      val=(Float)atof(strVal.c_str());
    return strVal;
  }
  //
  //--------------------------------------------------------------
  //
  string AppRC::get(const string& name, Double& val) 
  {
    string strVal=rc_p->get(name); 
    if (strVal != "")
      val=(Double)atof(strVal.c_str());
    return strVal;
  }
  //
  //--------------------------------------------------------------
  //
  void AppRC::put(const string& name, const string& val) 
  {rc_p->put(name,val);}
  //
  //--------------------------------------------------------------
  //
  void AppRC::put(const string& name, const Int& val) 
  {
    ostringstream tt; tt << val;
    put(name,tt.str());
  }
  //
  //--------------------------------------------------------------
  //
  void AppRC::put(const string& name, const Float& val) 
  {
    ostringstream tt; tt << val;
    put(name,tt.str());
  }
  //
  //--------------------------------------------------------------
  //
  void AppRC::put(const string& name, const Double& val) 
  {
    ostringstream tt; tt << val;
    put(name,tt.str());
 
 }
 
  Double AppRC::getMemoryAvailable(string envVarName)
  {
    AppRC myRC;
    string def_rc_val=myRC.get("system.resources.memory");
    Double memval=-1.0;
    if(def_rc_val.size() >0){
      //This is in MB usually
      stringstream(def_rc_val) >> memval;     
      memval*=1e6;
    }
    if(memval < 0.0){
      //This is in KB
      memval=HostInfo::memoryTotal(true);
      memval*=1e3;
    }
    Double valfromenv=-1.0;
    //Assuming it is in MB
    valfromenv=SynthesisUtils::getenv(envVarName.c_str(), valfromenv);
    valfromenv *=1e6;
    if(valfromenv <0.0)
      return memval;
    return valfromenv;
  }

};
