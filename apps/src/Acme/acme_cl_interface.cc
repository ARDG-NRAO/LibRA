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

#include <acme.h>

void acme_func(std::string& imageName, std::string& deconvolver,
		string& normtype, string& workdir, string& imType,
		float& pblimit, int& nterms, int& facets,
		float& psfcutoff,
		vector<float>& restoringbeam,
		Bool& computePB);

//
//-------------------------------------------------------------------------
//
#define RestartUI(Label)  {if(clIsInteractive()) {goto Label;}}
//#define RestartUI(Label)  {if(clIsInteractive()) {clRetry();goto Label;}}
//


void UI(Bool restart, int argc, char **argv,
	std::string& imageName, std::string& deconvolver,
        string& normtype, string& workdir, string& imType,
        float& pblimit, int& nterms, int& facets,
        float& psfcutoff,
        vector<float>& restoringbeam,
	bool& computePB)
{
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
      //i=1;clgetSValp("deconvolver", deconvolver,i);  
      i=1;clgetSValp("normtype", normtype,i);
      i=1;clgetSValp("workdir", workdir,i);
      i=1;clgetSValp("imtype", imType, i); clSetOptions("imtype",{"psf","residual"});
      i=1;clgetFValp("pblimit", pblimit,i);
      //      i=1;clgetIValp("nterms", nterms,i);
      //      i=1;clgetIValp("facets", facets,i);
      i=1;clgetFValp("psfcutoff", psfcutoff,i);
      i=1;clgetBValp("computepb", computePB, i);

      int N;
      //      N=0; N=clgetNFValp("restoringbeam", restoringbeam, N);
      
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
  string imageName, deconvolver="hogbom", normtype="flatnoise", workdir, imType="psf";
  float pblimit=0.2, psfcutoff=0.35;
  int nterms=1, facets=1;
  vector<float> restoringbeam;
  Bool computePB=False;
  Bool restartUI=False;;

 RENTER:// UI re-entry point.
  try
    {
      imageName="";
      UI(restartUI,argc, argv, imageName, deconvolver, normtype, workdir, imType, pblimit, nterms, facets, psfcutoff, restoringbeam, computePB);
      restartUI = False;
      //
      //---------------------------------------------------
      //
      acme_func(imageName, deconvolver, normtype, workdir, imType, pblimit, nterms, facets, psfcutoff, restoringbeam, computePB);
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
