//# contact.cc: Driver for the SDAlgorithm class from ImagerObjects
//# Copyright (C) 2021
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
#define CONTACT_USE_HPG 1
//#undef CONTACT_USE_MPI 
//
// Following are from the parafeed project (the UI library)
//
#include <cl.h> // C++ized version
#include <clinteract.h>
//#include <casacore/casa/namespace.h>

#ifdef CONTACT_USE_MPI
# include <mpi.h>
#endif
//
//================================================================================
//
//
//-------------------------------------------------------------------------
//
#define RestartUI(Label)  {if(clIsInteractive()) {goto Label;}}
//using namespace casacore;
//using namespace std;

#include <synthesis/ImagerObjects/test/hummbee_func.h>

//
void UI(bool restart, int argc, char **argv, string& MSNBuf,
	string& imageName, string& modelImageName,
	int& ImSize, int& nW,
	float& cellSize, string& stokes, string& refFreqStr,
	string& phaseCenter, string& weighting,
	float& robust,
	string& CFCache,
	string& fieldStr, string& spwStr,
	Bool& doPBCorr,
	Bool& conjBeams,
	Float& pbLimit,
  string& deconvolver,
  vector<float>& scales,
  float& largestscale, float& fusedthreshold,
  int& nterms,
  float& gain, float& threshold,
  float& nsigma,
  int& cycleniter, float& cyclefactor,
  vector<string>& mask
// how about min/maxpsffraction, smallscalbias?
  )
{
  if (!restart)
    {
	BeginCL(argc,argv);
	clInteractive(0);
      //clCmdLineFirst(0);//In an interactive session, this is supposed
			//to keep the order of the keywords presented
			//the same as in the code.  But it does not!
			//The library always behaves like
			//clCmdLineFirst(1).
      }
  // else
  //   clRetry();
 REENTER:
      try
    {
      SMap watchPoints; VString exposedKeys;
      int i;
      MSNBuf="";
      i=1;clgetSValp("ms", MSNBuf,i);  
      i=1;clgetSValp("imagename", imageName,i);  
      i=1;clgetSValp("modelimagename", modelImageName,i);  

      i=1;clgetIValp("imsize", ImSize,i);  
      i=1;clgetFValp("cellsize", cellSize,i);  
      i=1;clgetSValp("stokes", stokes,i);  clSetOptions("stokes",{"I","IV"});
      i=1;clgetSValp("reffreq", refFreqStr,i);  
      i=1;clgetSValp("phasecenter", phaseCenter,i);  

      InitMap(watchPoints,exposedKeys);
      exposedKeys.push_back("robust");
      watchPoints["briggs"]=exposedKeys;

      //Add watchpoints for exposing robust parameter when weight=briggs
      i=1;clgetSValp("weighting", weighting, i ,watchPoints);
      clSetOptions("weighting",{"natural","uniform","briggs"});

      i=1;clgetFValp("robust", robust,i);  
      

      i=1;clgetIValp("wplanes", nW,i);  
      i=1;clgetSValp("cfcache", CFCache,i);
    
      i=1;clgetSValp("field", fieldStr,i);
      i=1;clgetSValp("spw", spwStr,i);
      i=1;clgetBValp("pbcor", doPBCorr,i);
      i=1;clgetBValp("conjbeams", conjBeams,i);
      i=1;clgetFValp("pblimit", pbLimit,i);

      InitMap(watchPoints,exposedKeys);
      exposedKeys.push_back("scales");
      watchPoints["multiscale"]=exposedKeys;
      
      //InitMap(watchPoints,exposedKeys);
      exposedKeys.push_back("largestscale");
      exposedKeys.push_back("fusedthreshold");
      watchPoints["asp"]=exposedKeys;

      i=1;clgetSValp("deconvolver", deconvolver, i ,watchPoints);
      clSetOptions("deconvolver",{"hogbom","mtmfs","clark", "multiscale","asp"}); //genie todo: add full list

      int N;
      N=0; N=clgetNFValp("scales", scales, N);

      i=1;clgetFValp("largestscale", largestscale,i);
      i=1;clgetFValp("fusedthreshold", fusedthreshold,i);

      i=1;clgetIValp("nterms", nterms,i);
      i=1;clgetFValp("gain", gain,i);
      i=1;clgetFValp("nsigma", nsigma,i);
      i=1;clgetFValp("threshold", threshold,i);
      i=1;clgetIValp("cycleniter", cycleniter,i);
      i=1;clgetFValp("cyclefactor", cyclefactor,i);
      //i=1;clgetSValp("mask", mask,i);
      N=0; N=clgetNSValp("mask", mask, N);
    
     EndCL();

     // do some input parameter checking now.
     string mesgs;

     if (ImSize <= 0)
       mesgs += "The imsize parameter needs to be set to a positive finite value. ";

     if (cellSize <= 0)
       mesgs += "The cellsize parameter needs to be set to a positive finite value. ";

     if (mesgs != "")
       clThrowUp(mesgs,"###Fatal", CL_FATAL);
    }
  catch (clError& x)
    {
      x << x << endl;
      if (x.Severity() == CL_FATAL) exit(1);
      //clRetry();
      RestartUI(REENTER);
    }
}
//
//-------------------------------------------------------------------------
//
// Moving the embedded MPI interfaces out of the (visual) way.  If
// necessary in the future, MPI code should be implemented in a
// separate class and this code appropriate re-factored to be used as
// an API for MPI class.  Ideally, MPI code should not be part of the
// roadrunner (application layer) code.
//#include <synthesis/TransformMachines2/test/RR_MPI.h>
//

//
//-------------------------------------------------------------------------
// Experimantal code not yet working
//
// #include <synthesis/TransformMachines2/test/ImagingEngine.h>
//   ImagingEngine imgEngine();


//
//-------------------------------------------------------------------------
//
//
//-------------------------------------------------------------------------
//


int main(int argc, char **argv)
{
  //
  // -------------------------------------- Just the UI -------------------------------------------------------------------
  //
  // The Factory Settings.
  string MSNBuf,
    cfCache, fieldStr="", spwStr="*",
    imageName, modelImageName,phaseCenter, stokes="I",
    refFreqStr="3.0e9", weighting="natural", deconvolver="hogbom"; //, mask="";

  float cellSize;//refFreq=3e09, freqBW=3e9;
  float robust=0.0;
  int NX=0, nW=1;//cfBufferSize=512, cfOversampling=20, nW=1;

  bool restartUI=false;

  bool doPBCorr= true;
  bool conjBeams= true;
  float pbLimit=1e-3;
  vector<float> scales;
  float largestscale = -1;
  float fusedthreshold = 0;
  int nterms=2;
  float gain=0.1; 
  float threshold=0.0;
  float nsigma=0.0;
  int cycleniter=-1;
  float cyclefactor=1.0;
  vector<string> mask; 

  UI(restartUI, argc, argv, MSNBuf,imageName, modelImageName, 
    NX, nW, cellSize,
     stokes, refFreqStr, phaseCenter, weighting, robust,
     cfCache, fieldStr,spwStr
     ,doPBCorr, conjBeams, pbLimit, 
    deconvolver,
    scales,
    largestscale, fusedthreshold,
    nterms,
    gain, threshold,
    nsigma,
    cycleniter, cyclefactor,
    mask);

  set_terminate(NULL);

  try
    {
      Hummbee(restartUI, argc, argv, MSNBuf,imageName, modelImageName,
                 NX, nW, cellSize,
                 stokes, refFreqStr, phaseCenter, weighting, robust,
                 cfCache, fieldStr,spwStr,
                 doPBCorr, conjBeams, pbLimit,
                 deconvolver,
                 scales,
                 largestscale, fusedthreshold,
                 nterms,
                 gain, threshold,
                 nsigma,
                 cycleniter, cyclefactor,
                 mask
                 ); // genie - only need imagename (for .psf and .residual, cycleniter, deconvolver)
    }
  catch(AipsError& er)
    {
      cerr << er.what() << endl;
    }

  return 0;
}
