//# IDL.cc: Implementation of IDL class
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

#include <casa/Utilities/Assert.h>
#include <casa/Exceptions/Error.h>
#include <casa/Logging/LogIO.h>
#include <synthesis/IDL/IDL.h>
#include <casa/sstream.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

void idl_free_cb(unsigned char* loc) {
  LogIO os(LogOrigin("free_cb", "free_cb)", WHERE));
  os << LogIO::WARN << "IDL freed memory starting at " << (void*)loc << LogIO::POST;
}

void idl_output(int flags, char* buf, int n) {
  LogIO os(LogOrigin("IDLrunCommand(s)", "IDL::runCommmand(s)", WHERE));
#ifdef HAVE_IDL_LIB
  String message(buf, n);
  if(flags==IDL_TOUT_F_STDERR) {
    os << LogIO::WARN << message << LogIO::POST;
  }
  else {
    os << LogIO::NORMAL << message << LogIO::POST;
  }
#else
  (void)flags;(void)buf;(void)n; // remove compiler warning
#endif
}

IDL::IDL(Int options)
{
  LogIO os(LogOrigin("IDL", "IDL::IDL()", WHERE));

#ifdef HAVE_IDL_LIB
  int argc;
  argc=0;
  char *argv;
  argv=0;
  if(IDL_Init(options, &argc, &argv)) {
    os << "Started IDL successfully" << LogIO::POST;
  }
  else {
    os << LogIO::SEVERE << "Failed to start IDL" << LogIO::EXCEPTION;
  }
  IDL_ToutPush(idl_output);
#else
  (void) options;
  os << LogIO::SEVERE << "IDL processing is not available in this CASA installation." << LogIO::EXCEPTION;
#endif
}

//---------------------------------------------------------------------- 
IDL& IDL::operator=(const IDL& other)
{
  if(this!=&other) {
  };
  return *this;
};

//----------------------------------------------------------------------
IDL::IDL(const IDL& other)
{
  operator=(other);
}

Bool IDL::sendArray(const Array<Float>& a, String aname) {

  LogIO os(LogOrigin("sendArray", "", WHERE));

#ifdef HAVE_IDL_LIB
  Bool del;
  const float *data=a.getStorage(del);
  IDL_LONG dim[IDL_MAX_ARRAY_DIM];
  uInt len;
  len=0;
  for (uInt i=0;i<a.shape().nelements();i++) {
    if(a.shape()(i)>1) {
      dim[i]=a.shape()(i);
      len++;
    }
    else {
      break;
    }
  }
  IDL_VARIABLE* v;
  v=IDL_ImportNamedArray(getIDLName(aname), len, dim, IDL_TYP_FLOAT,
			 (UCHAR*) data, idl_free_cb, 0);
  if(!v) {
    os << LogIO::SEVERE << "Failed to send " << aname
       << "to IDL for processing" << LogIO::EXCEPTION;
  }
  else {
    os << "Defined IDL float array " <<  aname << " " << a.shape()
       << LogIO::POST;
  }
  return true;
#else
  (void) aname; (void)a;
  return false;
#endif
}

char* IDL::getIDLName(String aname) {

  LogIO os(LogOrigin("getIDLname", "getIDLname", WHERE));

  char* name;
  name = new char[aname.length()+1];
  aname.copy(name, aname.length());
  name[aname.length()]='\0';
  return name;
}

Array<Float> IDL::getArray(String aname) {

  LogIO os(LogOrigin("getArray", "getArray", WHERE));

#ifdef HAVE_IDL_LIB
  IDL_VPTR v=IDL_FindNamedVariable(getIDLName(aname), 0);
  // Make a copy 
  if(v) {
    IPosition shape(Int(v->value.arr->n_dim), 0);
    for (Int i=0;i<Int(v->value.arr->n_dim);i++) {
      shape(i)=Int(v->value.arr->dim[i]);
    }
    Array<Float> a(shape, (Float*)(v->value.arr->data));
    os << "Copied IDL float array " <<  aname << " " << LogIO::POST;
    return a;
  }
  else {
    os << LogIO::SEVERE << "IDL float array " << aname << " does not exist" << LogIO::EXCEPTION;
  }
#else
  (void) aname;
    os << LogIO::SEVERE << "IDL not enabled" << LogIO::EXCEPTION;
#endif
  Array<Float> a;
  return a;
}

#ifdef HAVE_IDL_LIB
IDL_VPTR IDL::getPointer(String aname) {

  LogIO os(LogOrigin("getPointer", "getPointer", WHERE));

  IDL_VPTR v;
  v=0;
  v=IDL_FindNamedVariable(getIDLName(aname), 0);
  if(!v) {
    os << LogIO::SEVERE << "Failed to find " << aname
       << " in IDL" << LogIO::EXCEPTION;
  }
  return v;
}
#endif

Int IDL::getInt(String aname) {

  LogIO os(LogOrigin("getInt", "getInt", WHERE));

#ifdef HAVE_IDL_LIB
  return Int(getPointer(aname)->value.i);
#else
  (void) aname;
  return 0;
#endif
}

Float IDL::getFloat(String aname) {

  LogIO os(LogOrigin("getFloat", "getFloat", WHERE));

#ifdef HAVE_IDL_LIB
  return Int(getPointer(aname)->value.f);
#else
  (void) aname;
  return 0;
#endif
}

IDL::~IDL() {
#ifdef HAVE_IDL_LIB
  IDL_ToutPop();
  IDL_Cleanup(1);
#endif
}

Bool IDL::runCommands(const Vector<String>& commands, Bool log) {

  LogIO os(LogOrigin("runCommand", "runCommand(const Vector<String>& commands)", WHERE));

#ifdef HAVE_IDL_LIB
  try {
    Bool result = true;
    for (uInt i=0; i < commands.nelements(); i++) {
      if(log) os << "IDL : " << commands(i) << LogIO::POST;
      char comm[commands(i).length()+1];
      commands(i).copy(comm, commands(i).length());
      comm[commands(i).length()]='\0';
      IDL_ExecuteStr(comm);
    }
    return result;
  } catch (AipsError x) {
    os << LogIO::SEVERE << "Exception: " << x.getMesg() << LogIO::POST;
    return false;
  } 
#else
  (void)commands;(void)log;    
#endif
  return false;
}

Bool IDL::runCommand(const String& command, Bool log) {

  LogIO os(LogOrigin("runCommand", "runCommand(const String command)", WHERE));
#ifdef HAVE_IDL_LIB
  try {
    if(log) os << "IDL : " << command << LogIO::POST;
    char comm[command.length()+1];
    command.copy(comm, command.length());
    comm[command.length()]='\0';
    IDL_ExecuteStr(comm);
    return true;
  } catch (AipsError x) {
    os << LogIO::SEVERE << "Exception: " << x.getMesg() << LogIO::POST;
    return false;
  } 
#else
  (void)command;(void)log;
#endif
  return false;
}

Bool IDL::setPath(const Vector<String>& paths) {
  String IDLCommand="!path=";
  for (uInt i=0;i < paths.nelements(); i++) {
    IDLCommand += "expand_path('+" + paths(i) + "')+':'+";
  }
  IDLCommand += "!path";
  return runCommand(IDLCommand, true);
}
  

} //# NAMESPACE CASA - END

