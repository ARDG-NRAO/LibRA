//# PixonProcessor.cc: Implementation of PixonProcessor class
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

#include <synthesis/DataSampling/PixonProcessor.h>
#include <synthesis/IDL/IDL.h>
#include <casa/OS/EnvVar.h>
#include <casa/Utilities/Assert.h>
#include <casa/Exceptions/Error.h>
#include <casa/sstream.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

PixonProcessor::PixonProcessor() 

{
  LogIO os(LogOrigin("PixonProcessor", "PixonProcessor::PixonProcessor()", WHERE));

  // Set up path: we need to look at the environment variables to
  // see where the IDL code is stored. Add a trailing / just in
  // case the user left it off.
  if(EnvironmentVariable::isDefined("IDL_PIXON_HOME")) {
    IDLPixonHome=EnvironmentVariable::get("IDL_PIXON_HOME") + "/";
  }
  else {
    os << LogIO::SEVERE << "Environment variable IDL_PIXON_HOME must point to the location of the IDL Pixon code" << LogIO::EXCEPTION;
  }
  if(EnvironmentVariable::isDefined("IDL_PUBLIC_HOME")) {
    IDLPublicHome=EnvironmentVariable::get("IDL_PUBLIC_HOME") + "/";
  }
  else {
    os << LogIO::SEVERE << "Environment variable IDL_PUBLIC_HOME must point to the location of the IDL public code" << LogIO::EXCEPTION;
  }
  
  Vector<String> paths(2);
  paths(0)=IDLPixonHome;
  paths(1)=IDLPublicHome;
  idl.setPath(paths);
  idl.runCommand("@app_startup");
}

//---------------------------------------------------------------------- 
PixonProcessor& PixonProcessor::operator=(const PixonProcessor& other)
{
  if(this!=&other) {
  };
  return *this;
};

//----------------------------------------------------------------------
PixonProcessor::PixonProcessor(const PixonProcessor& other)
{
  operator=(other);
}

//----------------------------------------------------------------------
PixonProcessor::~PixonProcessor() {
}

Bool PixonProcessor::calculate(const DataSampling& ds, Array<Float>& result) {
  LogIO os(LogOrigin("calculate", "calculate()", WHERE));

  try {

    // Send the Data
    idl.sendArray(ds.getDX(),    String("ADX"));
    idl.sendArray(ds.getPRF(),   String("APRF"));
    idl.sendArray(ds.getSigma(), String("ASigma"));
    idl.sendArray(ds.getData(),  String("AData"));

    // Now execute commands
    os << "Executing pixon estimation under IDL" << LogIO::POST;

    Bool ok=idl.runCommand(ds.getIDLScript());

    result=idl.getArray(String("AModel"));

    return ok;

  } catch (AipsError x) {
    os << LogIO::SEVERE << "Exception: " << x.getMesg() << LogIO::POST;
  } 
  return false;
}

Bool PixonProcessor::save(const DataSampling& ds, const String& saveName) {

  LogIO os(LogOrigin("save", "PixonProcessor::save()", WHERE));

  try {
    // Send the Data
    idl.sendArray(ds.getDX(),  String("ADX"));
    idl.sendArray(ds.getPRF(),  String("APRF"));
    idl.sendArray(ds.getData(), String("AData"));
    idl.sendArray(ds.getSigma(),  String("ASigma"));
    // Now execute commands
    String IDLcommand;
    os << "Saving data for subsequent processing under IDL" << LogIO::POST;
    IDLcommand="save,/all,/compress,filename='"+saveName+"',/verbose";
    Bool result=idl.runCommand(IDLcommand);

    return result;
  } catch (AipsError x) {
    os << LogIO::SEVERE << "Exception: " << x.getMesg() << LogIO::POST;
  } 
  return false;
}

Bool PixonProcessor::standardTest() {

  LogIO os(LogOrigin("standardtest", "standardtest()", WHERE));

  try {
    os << "Executing standard pixon test under IDL" << LogIO::POST;
    Vector<String> IDLcommands(1);
    IDLcommands(0)="test,/full,/display,/singleframe";
    return idl.runCommands(IDLcommands);
  } catch (AipsError x) {
    os << LogIO::SEVERE << "Exception: " << x.getMesg() << LogIO::POST;
  } 
  return false;
}

} //# NAMESPACE CASA - END

