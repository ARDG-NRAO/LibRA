/**
 * @file
 * This file contains the implementation of the main function and UI function for the TableInfo application.
 * The TableInfo application is a command-line tool that provides information about a MeasurementSet table.
 * It takes input table name and output file name as command-line arguments.
 * It uses the TableInfo_func function to extract information from the input table and write it to the output file.
 */
// # Copyright (C) 2021
// # Associated Universities, Inc. Washington DC, USA.
// #
// # This library is free software; you can redistribute it and/or modify it
// # under the terms of the GNU Library General Public License as published by
// # the Free Software Foundation; either version 2 of the License, or (at your
// # option) any later version.
// #
// # This library is distributed in the hope that it will be useful, but WITHOUT
// # ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// # FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
// # License for more details.is
// #
// # You should have received a copy of the GNU Library General Public License
// # along with this library; if not, write to the Free Software Foundation,
// # Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
// #
// # Correspondence concerning this should be addressed as follows:
// #        Postal address: National Radio Astronomy Observatory
// #                        1003 Lopezville Road,
// #                        Socorro, NM - 87801, USA
// #
// # $Id$


#include <cl.h>
#include <clinteract.h>

#include <tableinfo.h>


//
//-------------------------------------------------------------------------
//
#define RestartUI(Label)  {if(clIsInteractive()) {goto Label;}}
//#define RestartUI(Label)  {if(clIsInteractive()) {clRetry();goto Label;}}
//


void UI(Bool restart, int argc, char **argv, bool interactive,
	string& MSNBuf, string& OutBuf,
	bool& verbose)
{
  clSetPrompt(interactive);
  
  if (!restart)
    {
      BeginCL(argc,argv);
      clInteractive(0);
    }
  //else
  //clRetry();
  REENTER:
  try
    {
      int i;
       
      // can't init the follwoing; otherwise, the values passed through the non-interactive UI would be reset.
      //MSNBuf=OutBuf="";
      i=1;clgetSValp("table", MSNBuf,i);  
      i=1;clgetSValp("outfile",OutBuf,i);  
      i=1;clgetBValp("verbose",verbose,i);  
      EndCL();

      // do some input parameter checking now.
      string mesgs;

      if (MSNBuf=="")
       mesgs += "Input table name needs to be set.\n";

      if (mesgs != "")
       clThrowUp(mesgs,"###Fatal", CL_FATAL);
    }
    catch (clError& x)
    {
      x << x << endl;
      if (x.Severity() == CL_FATAL) {throw;} // Re-throw instead of exit - allows proper exception handling
    }
  
}
//
//-------------------------------------------------------------------------
//
int main(int argc, char **argv)
{
  //
  //---------------------------------------------------
  //
  string MSNBuf,OutBuf;
  Bool verbose=0, restartUI=False;;

 RENTER:// UI re-entry point.
  try
    {
      MSNBuf=OutBuf="";
      bool interactive = true;
      UI(restartUI,argc, argv, interactive, MSNBuf,OutBuf,verbose);
      restartUI = False;
      //
      //---------------------------------------------------
      //
      TableInfo_func(MSNBuf, OutBuf, verbose);
    }
  catch (clError& x)
    {
      x << x.what() << endl;
      restartUI=True;
    }
  //
  // Catch any exception thrown by AIPS++ libs.  Do your cleanup here
  // before returning to the UI (if you choose to).  Without this, all
  // exceptions (AIPS++ or otherwise) are caught in the default
  // exception handler clDefaultErrorHandler (installed by the CLLIB).
  //
  catch (AipsError& x)
    {
      cerr << "###AipsError: " << x.getMesg() << endl;
      restartUI=True;
      //      exit(0);
    }
  if (restartUI) RestartUI(RENTER);
}
