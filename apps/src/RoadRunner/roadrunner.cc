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
#include <clgetBaseCode.h>

#ifdef ROADRUNNER_USE_MPI
# include <mpi.h>
#endif
//
//================================================================================
//

#include <RoadRunner/rWeightor.h>
#include <RoadRunner/DataIterations.h>
#include <RoadRunner/DataBase.h>
#include <RoadRunner/MakeComponents.h>
#include <RoadRunner/Roadrunner_func.h>
//
//-------------------------------------------------------------------------
//
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
  bool doSPWDataIter=false;
  vector<float> posigdev = {300.0,300.0};

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
