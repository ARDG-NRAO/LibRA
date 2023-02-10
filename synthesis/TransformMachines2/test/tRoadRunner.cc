//# roadrunner.cc: Driver for the AWProject class of FTMachines
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
#define ROADRUNNER_USE_HPG 1
//#undef ROADRUNNER_USE_MPI 
//
// Following are from the parafeed project (the UI library)
//
#include <cl.h> // C++ized version
#include <clinteract.h>

#ifdef ROADRUNNER_USE_MPI
# include <mpi.h>
#endif
//
//================================================================================
//

#include <synthesis/TransformMachines2/test/rWeightor.h>
#include <synthesis/TransformMachines2/test/DataIterations.h>
#include <synthesis/TransformMachines2/test/DataBase.h>
#include <synthesis/TransformMachines2/test/MakeComponents.h>
#include <synthesis/TransformMachines2/test/Roadrunner_func.h>
//
//-------------------------------------------------------------------------
//
#define RestartUI(Label)  {if(clIsInteractive()) {goto Label;}}

//#define RestartUI(Label)  {if(clIsInteractive()) {clRetry();goto Label;}}
//
void UI(Bool restart, int argc, char **argv, string& MSNBuf,
	string& imageName, string& modelImageName,string& sowImageExt,
	string& cmplxGridName, int& ImSize, int& nW,
	float& cellSize, string& stokes, string& refFreqStr,
	string& phaseCenter, string& weighting,
	string& rmode, float& robust,
	string& FTMName, string& CFCache, string& imagingMode,
	Bool& WBAwp,string& fieldStr, string& spwStr,
	string& uvDistStr,
	Bool& doPointing, Bool& normalize,
	Bool& doPBCorr,
	Bool& conjBeams,
	Float& pbLimit,
	vector<float> &posigdev,
	Bool& doSPWDataIter)
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
      i=1;clgetSValp("sowimageext", sowImageExt,i);  
      i=1;clgetSValp("complexgrid", cmplxGridName,i);  

      i=1;clgetIValp("imsize", ImSize,i);  
      i=1;clgetFValp("cellsize", cellSize,i);  
      i=1;clgetSValp("stokes", stokes,i);  clSetOptions("stokes",{"I","IV"});
      i=1;clgetSValp("reffreq", refFreqStr,i);  
      i=1;clgetSValp("phasecenter", phaseCenter,i);  

      InitMap(watchPoints,exposedKeys);
      exposedKeys.push_back("rmode");
      exposedKeys.push_back("robust");
      watchPoints["briggs"]=exposedKeys;

      //Add watchpoints for exposing rmode and robust parameters when weight=briggs
      i=1;clgetSValp("weighting", weighting, i ,watchPoints);
      clSetOptions("weighting",{"natural","uniform","briggs"});

      i=1;clgetSValp("rmode", rmode,i);  
      i=1;clgetFValp("robust", robust,i);  
      

      i=1;clgetIValp("wplanes", nW,i);  
      i=1;clgetSValp("ftm", FTMName,i); clSetOptions("ftm",{"awphpg","awproject"});
      i=1;clgetSValp("cfcache", CFCache,i);

      InitMap(watchPoints,exposedKeys);
      exposedKeys.push_back("modelimagename");
      watchPoints["residual"]=exposedKeys;
      watchPoints["predict"]=exposedKeys;
      i=1;clgetSValp("mode", imagingMode,i, watchPoints); clSetOptions("mode",{"weight","psf","snrpsf","residual","predict"});

      i=1;clgetBValp("wbawp", WBAwp,i); 
      i=1;clgetSValp("field", fieldStr,i);
      i=1;clgetSValp("spw", spwStr,i);
      i=1;clgetSValp("uvdist", uvDistStr,i);
      i=1;clgetBValp("pbcor", doPBCorr,i);
      i=1;clgetBValp("conjbeams", conjBeams,i);
      i=1;clgetFValp("pblimit", pbLimit,i);
      InitMap(watchPoints,exposedKeys);
      exposedKeys.push_back("pointingoffsetsigdev");
      watchPoints["1"]=exposedKeys;


      i=1;clgetBValp("dopointing", doPointing,i,watchPoints);
      i=2;clgetNFValp("pointingoffsetsigdev", posigdev,i);

      i=1;dbgclgetBValp("normalize",normalize,i);
      i=1;dbgclgetBValp("spwdataiter",doSPWDataIter,i);
     EndCL();

     // do some input parameter checking now.
     string mesgs;
     if (phaseCenter == "")
       mesgs += "The phasecenter parameter needs to be set. ";

     if (refFreqStr == "")
       mesgs += "The reffreq parameter needs to be set. ";

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
#ifdef ROADRUNNER_USE_HPG
static const string defaultFtmName = "awphpg";
#else // !ROADRUNNER_USE_HPG
static const string defaultFtmName = "awproject";
#endif // ROADRUNNER_USE_HPG

int main(int argc, char **argv)
{
  //
  // -------------------------------------- Just the UI -------------------------------------------------------------------
  //
  // The Factory Settings.
  string MSNBuf,ftmName=defaultFtmName,
    cfCache, fieldStr="", spwStr="*", uvDistStr="",
    imageName, modelImageName,cmplxGridName="",phaseCenter, stokes="I",
    refFreqStr="3.0e9", weighting="natural", sowImageExt,
    imagingMode="residual",rmode="none";

  float cellSize;//refFreq=3e09, freqBW=3e9;
  float robust=0.0;
  int NX=0, nW=1;//cfBufferSize=512, cfOversampling=20, nW=1;
  bool WBAwp=true;
  bool restartUI=false;
  bool doPointing=false;
  bool normalize=false;
  bool doPBCorr= true;
  bool conjBeams= true;
  float pbLimit=1e-3;
  bool doSPWDataIter=true;
  vector<float> posigdev = {300.0,300.0};

  UI(restartUI, argc, argv, MSNBuf,imageName, modelImageName,
     sowImageExt, cmplxGridName, NX, nW, cellSize,
     stokes, refFreqStr, phaseCenter, weighting, rmode, robust,
     ftmName,cfCache, imagingMode, WBAwp,fieldStr,spwStr,uvDistStr,
     doPointing,normalize,doPBCorr, conjBeams, pbLimit, posigdev,
     doSPWDataIter);

  set_terminate(NULL);

  try
    {
      Roadrunner(restartUI, argc, argv, MSNBuf,imageName, modelImageName,
		 sowImageExt, cmplxGridName, NX, nW, cellSize,
		 stokes, refFreqStr, phaseCenter, weighting, rmode, robust,
		 ftmName,cfCache, imagingMode, WBAwp,fieldStr,spwStr,uvDistStr,
		 doPointing,normalize,doPBCorr, conjBeams, pbLimit, posigdev,
		 doSPWDataIter);
    }
  catch(AipsError& er)
    {
      cerr << er.what() << endl;
    }
  return 0;
}
