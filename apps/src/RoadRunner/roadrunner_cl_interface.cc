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

#include <roadrunner.h>
//
//-------------------------------------------------------------------------
//
#define RestartUI(Label)  {if(clIsInteractive()) {goto Label;}}

//#define RestartUI(Label)  {if(clIsInteractive()) {clRetry();goto Label;}}
//
void UI(Bool restart, int argc, char **argv, bool interactive, 
	string& MSNBuf,
	string& imageName, string& modelImageName,string& dataColumnName,
	string& sowImageExt,
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
  clSetPrompt(interactive);

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
  // REENTER:
  //  try
    {
      SMap watchPoints; VString exposedKeys;
      int i;
      MSNBuf="";
      i=1;clgetSValp("vis", MSNBuf,i);
      i=1;clgetSValp("imagename", imageName,i);
      i=1;clgetSValp("modelimagename", modelImageName,i);

      i=1;clgetSValp("datacolumn", dataColumnName,i);
      clSetOptions("datacolumn",{"data","model","corrected"});

      i=1;clgetSValp("sowimageext", sowImageExt,i);
      i=1;clgetSValp("complexgrid", cmplxGridName,i);

      i=1;clgetValp("imsize", ImSize,i);
      i=1;clgetValp("cell", cellSize,i);
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
      clSetOptions("rmode",{"abs","norm", "none"});

      i=1;clgetValp("robust", robust,i);

      i=1;clgetValp("wprojplanes", nW,i);
#ifdef ROADRUNNER_USE_HPG
      i=1;clgetSValp("gridder", FTMName,i); clSetOptions("gridder",{"awphpg","awproject"});
#else // !ROADRUNNER_USE_HPG
      i=1;clgetSValp("gridder", FTMName,i); clSetOptions("gridder",{"awproject"});
#endif // ROADRUNNER_USE_HPG
      i=1;clgetSValp("cfcache", CFCache,i);

      // Expose the modelimagename parameter only for mode=residual or
      // mode=predict
      InitMap(watchPoints,exposedKeys);
      exposedKeys.push_back("modelimagename");
      watchPoints["residual"]=exposedKeys;
      watchPoints["predict"]=exposedKeys;
      
      // Expose the datacolumn parameter only for mode=residual
      // exposedKeys.push_back("datacolumn");
      // watchPoints["residual"]=exposedKeys;
      std::vector<std::string> imagingModeOpts = {"weight","psf","snrpsf","residual","predict"};
      i=1;clgetSValp("mode", imagingMode,i,watchPoints); clSetOptions("mode",imagingModeOpts);

      i=1;clgetValp("wbawp", WBAwp,i);

      // A commas in the value of the following fields is interpreted
      // in the application code (MSSelection module of CASACore).  So
      // use a call from parafeed to get the value without it being
      // interpreted for commas by the parafeed library.
      i=1;clgetFullValp("field", fieldStr);
      i=1;clgetFullValp("spw", spwStr);
      i=1;clgetFullValp("uvrange", uvDistStr);

      i=1;clgetValp("pbcor", doPBCorr,i);
      i=1;clgetValp("conjbeams", conjBeams,i);
      i=1;clgetValp("pblimit", pbLimit,i);
      InitMap(watchPoints,exposedKeys);
      exposedKeys.push_back("pointingoffsetsigdev");
      watchPoints["1"]=exposedKeys;


      i=1;clgetValp("usepointing", doPointing,i,watchPoints);
      i=2;i=clgetValp("pointingoffsetsigdev", posigdev,i);

      i=1;cldbggetBValp("normalize",normalize,i);
      i=1;cldbggetBValp("spwdataiter",doSPWDataIter,i);
     EndCL();

     // do some input parameter checking now.
     string mesgs;
     auto si=std::find(imagingModeOpts.begin(), imagingModeOpts.end(),imagingMode);
     if ((si != imagingModeOpts.end()) && (imagingMode != "predict"))
       if (imageName == "")
	 mesgs += "Imaging mode="+imagingMode+" needs imagename to be set.\n";
     
     if (CFCache == "")
       mesgs += "The cfcache parameter needs to be set.\n";

     if (phaseCenter == "")
       mesgs += "The phasecenter parameter needs to be set.\n";

     if (refFreqStr == "")
       mesgs += "The reffreq parameter needs to be set.\n";

     if (ImSize <= 0)
       mesgs += "The imsize parameter needs to be set to a positive finite value.\n";

     if (cellSize <= 0)
       mesgs += "The cellsize parameter needs to be set to a positive finite value.\n";

     if (mesgs != "")
       clThrowUp(mesgs,"###Fatal", CL_FATAL);

    }
  // catch (clError& x)
  //   {
  //     if (x.Severity() == CL_FATAL) {throw;} // Re-throw instead of exit - allows proper exception handling
  //   }
}
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

#ifndef ROADRUNNER_LIBRARY_BUILD
int main(int argc, char **argv)
{
  //
  // -------------------------------------- Just the UI -------------------------------------------------------------------
  //
  // The Factory Settings.
  string MSNBuf,ftmName=defaultFtmName,
    cfCache, fieldStr="", spwStr="*", uvDistStr="", dataColumnName="",
    imageName, modelImageName,cmplxGridName="",phaseCenter, stokes="I",
    refFreqStr="3.0e9", weighting="natural", sowImageExt,
    imagingMode="residual",
    rmode="norm"; // set to "norm" to match rWeightor.h

  float cellSize=0;
  float robust=0.0;
  int NX=0, nW=1;
  bool WBAwp=true;
  bool restartUI=false;
  bool doPointing=false;
  bool normalize=false;
  bool doPBCorr= true;
  bool conjBeams= true;
  float pbLimit=1e-3;
  bool doSPWDataIter=false;
  vector<float> posigdev = {300.0,300.0};
  bool interactive = true;

  try
    {
      UI(restartUI, argc, argv, interactive,
	 MSNBuf,imageName, modelImageName, dataColumnName,
	 sowImageExt, cmplxGridName, NX, nW, cellSize,
	 stokes, refFreqStr, phaseCenter, weighting, rmode, robust,
	 ftmName,cfCache, imagingMode, WBAwp,fieldStr,spwStr,uvDistStr,
	 doPointing,normalize,doPBCorr, conjBeams, pbLimit, posigdev,
	 doSPWDataIter);

      set_terminate(NULL);
      RRReturnType rrr;
      rrr=Roadrunner(MSNBuf,imageName, modelImageName,dataColumnName,
		     sowImageExt, cmplxGridName, NX, nW, cellSize,
		     stokes, refFreqStr, phaseCenter, weighting, rmode, robust,
		     ftmName,cfCache, imagingMode, WBAwp,fieldStr,spwStr,uvDistStr,
		     doPointing,normalize,doPBCorr, conjBeams, pbLimit, posigdev,
		     doSPWDataIter);
    }
  catch(clError& er)
    {
      cerr << er.what() << endl;
    }
  catch(AipsError& er)
    {
      cerr << er.what() << endl;
    }
  catch(std::exception& e)
    {
      cerr << e.what() << endl;
    }

  // An example code below for extracting the info from RRRetrunType (a std::unordered_map).
  //  cerr << rrr[MAKEVB_TIME] << " " << rrr[NVIS] << " " << rrr[DATA_VOLUME] << " " << rrr[IMAGING_RATE] << " " << rrr[NVIS]/rrr[CUMULATIVE_GRIDDING_ENGINE_TIME] << " vis/sec" << endl;
  return 0;
}
#endif // ROADRUNNER_LIBRARY_BUILD
