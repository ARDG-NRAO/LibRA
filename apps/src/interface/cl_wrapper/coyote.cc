//# Coyote.cc: Driver for the AWConvFunc class of FTMachines
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
//
// Following are from the parafeed project (the UI library)
//
#include <cl.h> // C++ized version
#include <clinteract.h>
#include <clgetBaseCode.h>

// The following are for the convolution function production
#include <Coyote/Coyote_func.h>

#include <synthesis/TransformMachines2/AWConvFunc.h>


#define RestartUI(Label)  {if(clIsInteractive()) {goto Label;}}

//#define RestartUI(Label)  {if(clIsInteractive()) {clRetry();goto Label;}}
//
void UI(bool restart, int argc, char **argv, string& MSNBuf,
	//string& imageName,
	string& telescopeName, int& ImSize, 
	float& cellSize, string& stokes, string& refFreqStr,
	int& nW, string& CFCache, string& imageNamePrefix,
	bool& WBAwp, bool& aTerm, bool& psTerm, string& mType,
	float& pa, float& dpa,
	string& fieldStr, string& spwStr, string& phaseCenter,
	bool& conjBeams,
	int& cfBufferSize,
	int& cfOversampling,
	std::vector<std::string>& cfList,
	//	std::vector<std::string>& wtCFList,
	string& mode)
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
      i=1;clgetSValp("vis", MSNBuf,i);  
      //i=1;clgetSValp("imagename", imageName,i);  
      i=1;clgetSValp("telescope", telescopeName,i);  
      
      i=1;clgetIValp("imsize", ImSize,i);  
      i=1;clgetFValp("cell", cellSize,i);  
      i=1;clgetSValp("stokes", stokes,i);  clSetOptions("stokes",{"I","IV"});
      i=1;clgetSValp("reffreq", refFreqStr,i);  
      i=1;clgetSValp("phasecenter", phaseCenter,i);
      
      i=1;clgetIValp("wplanes", nW,i);  
      i=1;clgetSValp("cfcache", CFCache,i);
      i=1;clgetSValp("nameprefix", imageNamePrefix,i);
      
      i=1;clgetBValp("wbawp", WBAwp,i);
      i=1;clgetBValp("aterm", aTerm,i); 
      i=1;clgetBValp("psterm", psTerm,i); 
      i=1;clgetBValp("conjbeams", conjBeams,i);
      i=1;clgetSValp("muellertype", mType,i);
      i=1;dbgclgetFValp("pa", pa, i);
      i=1;clgetFValp("dpa", dpa, i);
      
      i=1;clgetFullValp("field", fieldStr);
      i=1;clgetFullValp("spw", spwStr);
      //      i=1;clgetFValp("pblimit", pbLimit,i);
      i=1;clgetIValp("buffersize", cfBufferSize,i);
      i=1;clgetIValp("oversampling", cfOversampling,i);

      InitMap(watchPoints,exposedKeys);exposedKeys.resize(0);
      exposedKeys.push_back("cflist");
      //      exposedKeys.push_back("wtcflist");
      watchPoints["fillcf"]=exposedKeys;

      i=1;clgetSValp("mode", mode,i,watchPoints);clSetOptions("mode",{"dryrun","fillcf"});
      clgetNSValp("cflist", cfList,i);
      //      clgetNSValp("wtcflist", wtCFList,i);
      
      EndCL();
      
      // do some input parameter checking now.
      string mesgs;
      if (CFCache == "")
	mesgs += "The cfcache parameter needs to be set.\n ";
      
      if (refFreqStr == "")
	mesgs += "The reffreq parameter needs to be set.\n ";
      
      if (ImSize <= 0)
	mesgs += "The imsize parameter needs to be set to a positive finite value.\n ";
      
      if (cellSize <= 0)
	mesgs += "The cell parameter needs to be set to a positive finite value.\n ";
      
      if (cfBufferSize <= 0)
	mesgs += "The buffersize parameter needs to be set to a positive finite value.\n ";
      
      if (cfOversampling <= 0)
	mesgs += "The oversampling parameter needs to be set to a positive value.\n ";
      
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
int main(int argc, char **argv)
{
  //
  // -------------------------------------- Just the UI -------------------------------------------------------------------
  //
  // The Factory Settings.
  string MSNBuf="", cfCache="", fieldStr="", spwStr="*",
    imageName,cmplxGridName="",phaseCenter, stokes="I",
    refFreqStr, telescopeName="EVLA", mType="diagonal",
    imageNamePrefix="",mode="dryrun";
  std::vector<std::string> cfList;
  //  std::vector<std::string> wtCFList;
  
  float cellSize=0;//refFreq=3e09, freqBW=3e9;
  int NX=0, nW=1, cfBufferSize=0, cfOversampling=20;
  bool WBAwp=true;
  bool restartUI=false;
  bool doPointing=false;
  bool normalize=false;
  bool conjBeams= true;
  bool psTerm = false;
  bool aTerm = true;
  float pa=-200.0, // Get PA from the MS
    dpa=360.0; // Don't rotate CFs for PA
  
  UI(restartUI, argc, argv, MSNBuf,
     //imageName,
     telescopeName,
     NX, cellSize, stokes, refFreqStr, nW,
     cfCache, imageNamePrefix, WBAwp,
     psTerm, aTerm, mType, pa, dpa,
     fieldStr, spwStr, phaseCenter, conjBeams,
     cfBufferSize, cfOversampling,
     cfList,
     //     wtCFList,
     mode);
  
  set_terminate(NULL);
  
  try
    {
      Coyote(restartUI, argc, argv, MSNBuf,//imageName, 
	     telescopeName, NX, cellSize,
	     stokes, refFreqStr, nW,
	     cfCache, imageNamePrefix,
	     WBAwp,
	     psTerm, aTerm, mType, pa, dpa,
	     fieldStr,spwStr, phaseCenter,
	     conjBeams,
	     cfBufferSize, cfOversampling,
	     cfList,
	     //wtCFList,
	     mode);
      
    }
  catch(AipsError& er)
    {
      cerr << er.what() << endl;
    }
  return 0;
}
