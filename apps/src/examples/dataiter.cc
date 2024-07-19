//# Copyright (C) 2024
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or (at your option)
//# any later version.
//#
//# This program is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//# more details.
//#
//# You should have received a copy of the GNU General Public License along
//# with this program; if not, write to the Free Software Foundation, Inc.,
//# 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$

//
// Following are from the parafeed project (the UI library)
//
#include <cl.h> // C++ized version
#include <clinteract.h>
#include <clgetBaseCode.h>
//
//================================================================================
//
//
//-------------------------------------------------------------------------
//
#define RestartUI(Label)  {if(clIsInteractive()) {goto Label;}}

#include <DataIterations.h>
#include <DataBase.h>

// The interface function that to setup the parameters of the
// application.  Can be replaced with whatever may be the appropriate,
// or preferred way of getting these parameters.
void UI(Bool restart, int argc, char **argv, bool interactive, 
	string& msName, string& antStr,string& fieldStr,
	string& spwStr, string& uvDistStr)
{
  clSetPrompt(interactive);

  if (!restart)
    {
      BeginCL(argc,argv);
      //clCmdLineFirst(0);//In an interactive session, this is supposed
      //to keep the order of the keywords presented
      //the same as in the code.  But it does not!
      //The library always behaves like
      //clCmdLineFirst(1).
      clInteractive(0);
    }

 REENTER:
  try
    {
      // Get the parameters via an interactive session, or in a
      // non-iterative execution via commandline options.
      // Ref: https://github.com/sanbee/parafeed
      {
	//SMap watchPoints; VString exposedKeys;
	int i;
	i=1;clgetSValp("msname", msName,i);
	// A commas in the value of the following fields is interpreted
	// in the application code (MSSelection module of CASACore).  So
	// use a call from parafeed to get the value without it being
	// interpreted for commas by the parafeed library.
	i=1;clgetFullValp("baseline", antStr);
	i=1;clgetFullValp("field", fieldStr);
	i=1;clgetFullValp("spw", spwStr);
	i=1;clgetFullValp("uvrange", uvDistStr);
      }
      EndCL();
    }
  catch (clError& err)
    {
      // Report the error.  The clError object implements the
      // operator<<(), to keep control of how and what is reported as
      // error.  By default, the output stream is cerr
      //
      // Re-implemetion of just this (not implemented yet) can be
      // used, e.g., to handle runtime errors in a distributed
      // execution.
      err << err << endl;
      //clRetry();
      //RestartUI(REENTER);
      exit(1);
    }
}

int main(int argc, char** argv)
{
  bool interactive=true, restartUI=false,
    restart=false;

  string msName="", antStr="", fieldStr="",
    spwStr="", uvDistStr="";

  UI(restart, argc, argv, interactive, 
     msName, antStr,fieldStr, spwStr,uvDistStr); 
  //
  //-------------------------------------------------------------------------
  //
  DataBase db;

  //
  // Setup the MSSelection object.  This setup is reflected in the
  // db.theSelectedMS object.
  //
  db.msSelection.setSpwExpr(spwStr);
  db.msSelection.setAntennaExpr(antStr);
  db.msSelection.setFieldExpr(fieldStr);
  db.msSelection.setUvDistExpr(uvDistStr);

  //
  // Open the MS and apply the MSelection object.  After this call,
  // the raw (un-selected) and the selected MSes are available as
  // db.theMS and db.theSelectedMS.  These are lightweight objects and
  // NOT copies of the MS (as is a popular superstition in the RA
  // religion).
  //
  bool spwSort=false;
  db.openDB(msName,spwSort);

  //
  // Examples of getting meta-information of the selection.  The
  // msSelection object is a lightweight object that holds the
  // meta-information necessary to construct a reference to the
  // selected MS.  Hence, both the selection object and the selected
  // database object are lightweight representation of the persistent
  // DB.  See the full MSSelection interface at
  // http://casacore.github.io/casacore/classcasacore_1_1MSSelection.html
  //
  auto spwid=db.msSelection.getSpwList().tovector();
  auto fieldid=db.msSelection.getFieldList().tovector();
  cerr << "No. of rows selected: " << db.selectedMS.nrow() << endl;
  if (spwid.size() > 0)  cerr << "Selected SPW: " << spwid << endl;
  if (fieldid.size() > 0) cerr << "Selected FIELD IDs: " << fieldid << endl;

  // Data column to use for the data fidelity term in a solver.
  casa::refim::FTMachine::Type dataCol_l=casa::refim::FTMachine::CORRECTED;
  bool isRoot=true; // A left-over from days when code was being MPI'ed in place (crazy days!)

  DataIterator di(isRoot,dataCol_l);
}
