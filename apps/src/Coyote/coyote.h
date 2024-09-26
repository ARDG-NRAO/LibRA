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

#ifndef COYOTE_H
#define COYOTE_H

#include <casacore/casa/namespace.h>
//#include <casacore/casa/OS/Directory.h>
#include <casacore/casa/Logging/LogFilter.h>
//#include <casacore/casa/System/ProgressMeter.h>

#include <casacore/measures/Measures/Stokes.h>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/PagedImage.h>
//#include <casacore/tables/TaQL/ExprNode.h>

#include <casacore/ms/MSSel/MSSelection.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <msvis/MSVis/ViFrequencySelection.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>

//#include <synthesis/TransformMachines2/MakeCFArray.h>
//#include <synthesis/TransformMachines2/ThreadCoordinator.h>
#include <synthesis/TransformMachines2/CFStore2.h>
#include <synthesis/TransformMachines2/AWConvFunc.h>
#include <synthesis/TransformMachines2/CFCache.h>
#include <synthesis/TransformMachines2/PolOuterProduct.h>
#include <synthesis/TransformMachines2/ImageInformation.h>
#include <synthesis/TransformMachines2/Utils.h>
#include <casacore/casa/Utilities/Regex.h>

// #include <hpg/hpg.hpp>
#include <unistd.h>

using namespace casa;
using namespace casa::refim;
using namespace casacore;
using namespace std;

/// @brief This function makes an empty image for the sky model.
/// @param vi2 is the visibility iterator.
/// @param selectedMS is the MeasurementSet object.
/// @param msSelection is the MSSelection object.
/// @param imageName is the name of the image.
/// @param imSize is the size of the image.
/// @param cellSize is the cell size of the image.
/// @param phaseCenter is the phase center of the image.
/// @param stokes is the Stokes parameter of the image.
/// @param refFreq is the reference frequency of the image.
/// @return 
PagedImage<Complex> makeEmptySkyImage4CF(VisibilityIterator2& vi2,
					 const MeasurementSet& selectedMS,
					 MSSelection& msSelection,
					 const String& imageName,
					 const Vector<Int>& imSize, const float& cellSize,
					 const String& phaseCenter, const String& stokes,
					 const String& refFreq);



/// @brief Sets the Polarization Outer Product (POP) object.
/// @param vb2 is the VisBuffer2 object.
/// @param visPolMap is the vector of Stokes::StokesTypes.
/// @param polMap is the vector of polarization indices.
/// @param stokes is the Stokes parameter.
/// @param mType is the Muller parameter.
/// @return 
CountedPtr<refim::PolOuterProduct> setPOP(vi::VisBuffer2 &vb2,
					  Vector<casacore::Stokes::StokesTypes> visPolMap,
					  Vector<int> polMap,
					  std::string &stokes, std::string &mType);

/// @brief This function returns the fileList of CFs to fill.
/// @param cfCacheName is the name of the CF cache.
/// @param regexList is the list of regexes.
/// @return 
std::vector<std::string> fileList(const std::string& cfCacheName,
				  const std::vector<std::string>& regexList);


/// @brief Is a Function to generate a list of CFs which can be filled usinga  different mode
/// @param MSNBuf is the name of the MeasurementSet.
/// @param telescopeName is the name of the telescope.
/// @param NX is the size of the image.
/// @param cellSize is the cell size of the image.
/// @param stokes is the Stokes parameter.
/// @param refFreqStr is the reference frequency.
/// @param nW is the number of W-terms.
/// @param cfCacheName is the name of the CF cache.
/// @param WBAwp is the flag for WBAwp.
/// @param psTerm is the flag for psTerm.
/// @param aTerm is the flag for aTerm.
/// @param mType is	the Muller parameter.
/// @param pa is the parallactic angle.
/// @param dpa is the delta parallactic angle.
/// @param fieldStr is the field selection string.
/// @param spwStr is the spectral window selection string.
/// @param phaseCenter is the phase center.
/// @param conjBeams is the flag for conjBeams.
/// @param cfBufferSize is the CF buffer size.
/// @param cfOversampling is the CF oversampling.
/// @param cfList is the list of CFs.
/// @param mode is the mode which can be either dryrun or fillcf.
void Coyote(//bool &restartUI, int &argc, char **argv,
	    string &MSNBuf, 
	    string &telescopeName,
	    int &NX, float &cellSize,
	    string &stokes, string &refFreqStr, int &nW,
	    string &cfCacheName,
	    bool &WBAwp, bool& aTerm, bool &psTerm, string &mType,
	    float& pa, float& dpa,
	    string &fieldStr, string &spwStr, string &phaseCenter,
	    bool &conjBeams,  
	    int &cfBufferSize, int &cfOversampling,
	    std::vector<std::string>& cfList,
	    string& mode);

// UI Funtions 
 
/**
 * @brief This function handles the user interface for the Coyote application.
 *
 * @param restart A boolean parameter indicating whether to restart the UI.
 * @param argc The number of command line arguments.
 * @param argv The command line arguments.
 * @param interactive A boolean parameter indicating whether to run in the interactive mode
 * @param MSNBuf A string parameter.
 * @param telescopeName The name of the telescope.
 * @param ImSize The image size.
 * @param cellSize The cell size.
 * @param stokes The Stokes parameters.
 * @param refFreqStr The reference frequency string.
 * @param nW The number of W-planes.
 * @param CFCache The convolution function cache.
 * @param WBAwp A boolean parameter indicating whether to use WBAwp.
 * @param aTerm A boolean parameter indicating whether to use aTerm.
 * @param psTerm A boolean parameter indicating whether to use psTerm.
 * @param mType The measurement type.
 * @param pa The position angle.
 * @param dpa The change in position angle.
 * @param fieldStr The field string.
 * @param spwStr The spectral window string.
 * @param phaseCenter The phase center.
 * @param conjBeams A boolean parameter indicating whether to use conjugate beams.
 * @param cfBufferSize The convolution function buffer size.
 * @param cfOversampling The convolution function oversampling factor.
 * @param cfList The list of convolution functions.
 * @param mode The mode of operation.
 */
void UI(bool restart, int argc, char **argv, bool interactive, 
	string& MSNBuf,
        //string& imageName,
        string& telescopeName, int& ImSize,
        float& cellSize, string& stokes, string& refFreqStr,
        int& nW, string& CFCache, 
        bool& WBAwp, bool& aTerm, bool& psTerm, string& mType,
	float& pa, float& dpa,
        string& fieldStr, string& spwStr, string& phaseCenter,
        bool& conjBeams,
        int& cfBufferSize,
        int& cfOversampling,
        std::vector<std::string>& cfList,
        //      std::vector<std::string>& wtCFList,
	string& mode);


#endif
