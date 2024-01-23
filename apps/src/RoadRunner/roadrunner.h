//# Roadrunner_func.h: Definition of the Roadrunner function
//# Copyright (C) 2021
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
//# Correspondence concerning this should be addressed as follows:
//#        Postal address: National Radio Astronomy Observatory
//#                        1003 Lopezville Road,
//#                        Socorro, NM - 87801, USA
//#
//# $Id$

#ifndef ROADRUNNER_ROADRUNNER_FUNC_H
#define ROADRUNNER_ROADRUNNER_FUNC_H
#include <casacore/casa/namespace.h>
#include <casacore/casa/OS/Directory.h>
//#include <casacore/casa/Utilities/Regex.h>
#include <casacore/casa/Logging/LogFilter.h>
#include <casacore/casa/System/ProgressMeter.h>

#include <casacore/measures/Measures/Stokes.h>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/PagedImage.h>
//#include <casacore/tables/TaQL/ExprNode.h>

#include <casacore/ms/MSSel/MSSelection.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <msvis/MSVis/ViFrequencySelection.h>

#include <synthesis/TransformMachines2/FTMachine.h>
#include <synthesis/TransformMachines2/AWVisResampler.h>
//#include <synthesis/TransformMachines2/PointingOffsets.h>
#include <synthesis/TransformMachines2/CFStore2.h>
#include <synthesis/TransformMachines2/MakeCFArray.h>
#include <RoadRunner/ThreadCoordinator.h>
#ifdef ROADRUNNER_USE_HPG
#include <synthesis/TransformMachines2/AWVisResamplerHPG.h>
#include <synthesis/TransformMachines2/AWProjectWBFTHPG.h>
#include <hpg/hpg.hpp>
#endif


using namespace casa;
using namespace casa::refim;
using namespace casacore;

//vi::VisBuffer2 *vb_cfsrvr;
//vi::VisibilityIterator2 *vi2_cfsrvr;

//
//-------------------------------------------------------------------------
//
/**
 * @fn std::string remove_extension(const std::string& path)
 * @brief Removes the extension from a file path.
 * @param path The input file path.
 * @return The file path without the extension.
 */
std::string remove_extension(const std::string& path);
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
//-------------------------------------------------------------------------
// Class to manages the HPG initialize/finalize scope.
// hpg::finalize() in the destructor.  This also reports, via the
// destructor, the time elapsed between contruction and destruction.
//
/**
 * @class LibHPG
 * @brief Manages the HPG initialize/finalize scope.
 *
 * This class is responsible for initializing and finalizing the HPG library. 
 * It also reports the time elapsed between construction and destruction.
 */
class LibHPG
{
public:
  /**
   * @brief Constructor for the LibHPG class.
   * @param usingHPG A boolean indicating whether to use HPG. Default is true.
   */
  LibHPG(const bool usingHPG=true): init_hpg(usingHPG)
  {
    initialize();
  }
  /**
   * @brief Initializes the HPG library.
   * @return A boolean indicating whether the initialization was successful.
   */
  bool initialize()
  {
    bool ret = true;
    startTime = std::chrono::steady_clock::now();
#ifdef ROADRUNNER_USE_HPG
    if (init_hpg) ret = hpg::initialize();
#endif
    if (!ret)
      throw(AipsError("LibHPG::initialize() failed"));
    return ret;
  };
  /**
   * @brief Finalizes the HPG library.
   */
  void finalize()
  {
#ifdef ROADRUNNER_USE_HPG
    if (init_hpg)
      {
	cerr << endl << "CALLING hpg::finalize()" << endl;
	hpg::finalize();
      }	
#endif
  };
  /**
   * @brief Destructor for the LibHPG class.
   */
  ~LibHPG()
  {
    finalize();

    auto endTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> runtime = endTime - startTime;
    std::cerr << "roadrunner runtime: " << runtime.count()
	      << " sec" << std::endl;
  };

  std::chrono::time_point<std::chrono::steady_clock> startTime;
  bool init_hpg;
};
//
//-------------------------------------------------------------------------
//
//
//-------------------------------------------------------------------------
// The engine that loads the required CFs from the cache and copies
// them to the hpg::CFArray.
/**
 * @fn std::tuple<bool, std::shared_ptr<hpg::RWDeviceCFArray>> prepCFEngine(casa::refim::MakeCFArray& mkCF, bool WBAwp, int nW, int ispw, double spwRefFreq, int nDataPol, casacore::ImageInterface<casacore::Float>& skyImage, casacore::Vector<Int>& polMap, casacore::CountedPtr<casa::refim::CFStore2>& cfs2_l)
 * @brief Prepares the CF Engine.
 * @param mkCF A reference to the MakeCFArray object.
 * @param WBAwp A boolean indicating whether to use wideband AWP.
 * @param nW The number of w-projection planes.
 * @param ispw The spectral window index.
 * @param spwRefFreq The reference frequency of the spectral window.
 * @param nDataPol The number of data polarizations.
 * @param skyImage A reference to the sky image.
 * @param polMap A reference to the polarization map.
 * @param cfs2_l A reference to the CFStore2 object.
 * @return A tuple containing a boolean indicating whether a new CF was created and a shared pointer to the RWDeviceCFArray.
 */
std::tuple<bool, std::shared_ptr<hpg::RWDeviceCFArray>>
prepCFEngine(casa::refim::MakeCFArray& mkCF,
	     bool WBAwp, int nW,
	     int ispw, double spwRefFreq,
	     int nDataPol,
	     casacore::ImageInterface<casacore::Float>& skyImage,
	     casacore::Vector<Int>& polMap,
	     casacore::CountedPtr<casa::refim::CFStore2>& cfs2_l
	     );


//
//-------------------------------------------------------------------------
// Server function that triggers the prepCFEngine() fro each SPW in
// the spwidList.  This is run in parallel with gridding in a separate
// thread than the gridder-thread (the main thread).  Coordination
// between the thread is managed by the ThreadCoordinator object.
//
/**
 * @fn void CFServer(ThreadCoordinator& thcoord, casa::refim::MakeCFArray& mkCF, bool& WBAwp, int& nW, casacore::ImageInterface<casacore::Float>& skyImage, casacore::Vector<Int>& polMap, casacore::CountedPtr<casa::refim::CFStore2>& cfs2_l, casacore::Vector<int>& spwidList, casacore::Vector<double>& spwRefFreqList, int& nDataPol)
 * @brief Server function that triggers the prepCFEngine() for each SPW in the spwidList.
 * @param thcoord A reference to the ThreadCoordinator object.
 * @param mkCF A reference to the MakeCFArray object.
 * @param WBAwp A boolean indicating whether to use wideband AWP.
 * @param nW The number of w-projection planes.
 * @param skyImage A reference to the sky image.
 * @param polMap A reference to the polarization map.
 * @param cfs2_l A reference to the CFStore2 object.
 * @param spwidList A reference to the list of spectral window indices.
 * @param spwRefFreqList A reference to the list of reference frequencies for the spectral windows.
 * @param nDataPol The number of data polarizations.
 */
void CFServer(ThreadCoordinator& thcoord,
	      casa::refim::MakeCFArray& mkCF,
	      bool& WBAwp, int& nW,
	      casacore::ImageInterface<casacore::Float>& skyImage,
	      casacore::Vector<Int>& polMap,
	      casacore::CountedPtr<casa::refim::CFStore2>& cfs2_l,
	      casacore::Vector<int>& spwidList,
	      casacore::Vector<double>& spwRefFreqList,
	      int& nDataPol);



//--------------------------------------------------------------------------------------------
// Place-holder functions to build the appropriate Mueller index
// matrix.  For production this should be generalized and perhaps
// internally build using the user stokes setup and visibility vector
// from the MS.
/**
 * @fn std::vector<std::vector<int>> readMNdxText(const std::string& fileName)
 * @brief Reads a text file and returns a 2D vector of integers.
 * @param fileName The name of the file to read.
 * @return A 2D vector of integers read from the file.
 */
std::vector<std::vector<int>>
readMNdxText(const string& fileName);

//
//--------------------------------------------------------------------------------------------
// Function to build the vis-to-imageplane map (and it's conjugate).
//
// If the named file exist, it is expect to have the the map.
// Validation of the contents of this file is not yet complete.  If
// the file reading fails, a map is built based on the number grid
// planes and length of the vis vector. A warning message is given
// with the generated map and that the named file could not be read.
//
// The maps constructed here are translated in HPGVisBuffer to be
// consistent with the minimum-set visibility data that is transported
// to the GPU.
/**
 * @fn std::tuple<PolMapType, PolMapType> makeMNdx(const std::string& fileName, const Vector<int>& polMap, const int& nGridPlanes)
 * @brief Builds the vis-to-imageplane map (and its conjugate).
 * @param fileName The name of the file to read.
 * @param polMap The polarization map.
 * @param nGridPlanes The number of grid planes.
 * @return A tuple containing the vis-to-imageplane map and its conjugate.
 */
std::tuple<PolMapType, PolMapType>
makeMNdx(const string& fileName,
	 const Vector<int>& polMap,
	 const int& nGridPlanes);


/**
 * @fn void Roadrunner(string& MSNBuf, string& imageName, string& modelImageName, string& dataColumnName, string& sowImageExt, string& cmplxGridName, int& NX, int& nW, float& cellSize, string& stokes, string& refFreqStr, string& phaseCenter, string& weighting, string& rmode,  float& robust, string& ftmName, string& cfCache, string& imagingMode, bool& WBAwp, string& fieldStr, string& spwStr, string& uvDistStr, bool& doPointing, bool& normalize, bool& doPBCorr, bool& conjBeams, float& pbLimit, vector<float>& posigdev, bool& doSPWDataIter)
 * @brief Main function for the Roadrunner application.
 * @param MSNBuf The measurement set name buffer.
 * @param imageName The name of the image.
 * @param modelImageName The name of the model image.
 * @param dataColumnName The name of the data column.
 * @param sowImageExt The extension of the sow image.
 * @param cmplxGridName The name of the complex grid.
 * @param NX The number of X-direction grid points.
 * @param nW The number of w-projection planes.
 * @param cellSize The size of the cell.
 * @param stokes The Stokes parameters.
 * @param refFreqStr The reference frequency string.
 * @param phaseCenter The phase center.
 * @param weighting The weighting scheme.
 * @param rmode The robustness mode.
 * @param robust The robustness parameter.
 * @param ftmName The name of the FT machine.
 * @param cfCache The CF cache.
 * @param imagingMode The imaging mode.
 * @param WBAwp A boolean indicating whether to use wideband AWP.
 * @param fieldStr The field string.
 * @param spwStr The spectral window string.
 * @param uvDistStr The UV distance string.
 * @param doPointing A boolean indicating whether to do pointing.
 * @param normalize A boolean indicating whether to normalize.
 * @param doPBCorr A boolean indicating whether to do PB correction.
 * @param conjBeams A boolean indicating whether to conjugate the beams.
 * @param pbLimit The limit for the primary beam.
 * @param posigdev The position sigma deviation.
 * @param doSPWDataIter A boolean indicating whether to do SPW data iteration.
 */
void Roadrunner(//bool& restartUI, int& argc, char** argv,
		string& MSNBuf, string& imageName, string& modelImageName,
		string& dataColumnName,
		string& sowImageExt, string& cmplxGridName,
		int& NX, int& nW, float& cellSize,
		string& stokes, string& refFreqStr, string& phaseCenter,
		string& weighting, string& rmode,  float& robust,
		string& ftmName, string& cfCache, string& imagingMode,
		bool& WBAwp,
		string& fieldStr, string& spwStr, string& uvDistStr,
		bool& doPointing, bool& normalize, bool& doPBCorr,
		bool& conjBeams, float& pbLimit, vector<float>& posigdev,
		bool& doSPWDataIter);



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
  Bool& doSPWDataIter);


#endif

