/**
 * @file This file contains the command-line interface implementation
 * chip application.  The purpose of this application is to
 * accumulate (gather, in CS-speak) a list of images and save the
 * result in an output image.
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

#include <acme.h>

//
//-------------------------------------------------------------------------
//
#define RestartUI(Label)  {if(clIsInteractive()) {goto Label;}}
//#define RestartUI(Label)  {if(clIsInteractive()) {clRetry();goto Label;}}
//


void UI(Bool restart, int argc, char **argv, bool interactive,
	std::string& imageName, string& stats)
{
  clSetPrompt(interactive);

  if (!restart)
    {
      BeginCL(argc,argv);
      clInteractive(0);
    }
  else
    clRetry();

  try
  {
    int i;

    i=1;clgetSValp("imagename", imageName,i);  
    i=1;clgetSValp("stats", stats,i);
    clSetOptions("stats",{"checkamp","all"});
    EndCL();
  }
  catch (clError x)
    {
      x << x << endl;
      clRetry();
    }

  if (imageName=="")
    throw(AipsError("Input image name not set."));
    
}
//
//-------------------------------------------------------------------------
//
int main(int argc, char **argv)
{
  //
  //---------------------------------------------------
  //

  bool restartUI = false;
  bool interactive = true;
  std::string imageName;
  string stats="checkamp";

 RENTER:// UI re-entry point.
  try
    {
      UI(restartUI,argc, argv, interactive, 
      	 imageName, stats);
      //      restartUI = False;
      //
      //---------------------------------------------------
      //
      Acme::acme(imageName, stats);
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
  //  if (restartUI) RestartUI(RENTER);
}
