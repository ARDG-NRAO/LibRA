//# SpectralCollapser.h: Header file for class SpectralCollapser
//# Copyright (C) 1998,1999,2000,2001,2003
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
//# $Id: tSubImage.cc 20567 2009-04-09 23:12:39Z gervandiepen $

#ifndef IMAGEANALYSIS_SPECTRALCOLLAPSER_H
#define IMAGEANALYSIS_SPECTRALCOLLAPSER_H

#include <imageanalysis/ImageAnalysis/ImageTask.h>

#include <casa/namespace.h>

namespace casacore{

template <class T> class SubImage;
}

namespace casa {


class SpectralCollapser {
	// <summary>
	// Class to collapse an image along the spectral axis.
	// </summary>

	// <reviewed reviewer="" date="" tests="" demos="">
	// </reviewed>

	// <prerequisite>
	//   <li> <linkto class=ImageCollapser>ImageCollapser</linkto>
	// </prerequisite>

	// <etymology>
	// Collapses an image along the spectral axis
	// </etymology>

	// <synopsis>
	// Helper class to collapse an image along the spectral axis. The spectral
	// range to combine is provided interactively from the profiler. The class
	// transforms the range to the channels to be combined. The actual image\
	// combination is then done with the class "ImageCollapser".
	// </synopsis>

public:
	// The different collapse types
	enum CollapseType {
		PMEAN,
		PMEDIAN,
		PSUM,
		CUNKNOWN
		//PVRMSE,
	};

	enum CollapseError {
		PNOERROR,
		PERMSE,
		PPROPAG,
		EUNKNOWN
	};

	// Constructor
	SpectralCollapser(const SPCIIF image);

	// Constructor
	SpectralCollapser(const SPCIIF image, const casacore::String storePath);

	// Destructor
	virtual ~SpectralCollapser();

	// Parameters:
	// <src>specVals</src>  - the vector of spectral values
	// <src>startVal</src>  - the spectral value to start the collapse
	// <src>endVal</src>    - the spectral value to end the collapse
	// <src>unit</src>      - the unit of the spectral values
	// <src>collType</src>  - the collapse type (e.g. "mean" or "median")
	// <src>collError</src> - information on what the error
	// <src>outname</src>   - name of the collapsed image (output)
	// <src>msg</src>       - message string (output)
	virtual casacore::Bool collapse(const casacore::Vector<casacore::Float> &specVals, const casacore::Float startVal, const casacore::Float endVal,
			const casacore::String &unit, const SpectralCollapser::CollapseType &collType, const SpectralCollapser::CollapseError &collError, casacore::String &outname, casacore::String &msg);

	casacore::String summaryHeader() const;

   void collapseTypeToVector(const SpectralCollapser::CollapseType &collType, casacore::Vector<casacore::Int> &momentVec);

   // Convert from string to collapse type
	static void stringToCollapseType(const casacore::String &text,  SpectralCollapser::CollapseType &collType);

	// Convert from string to error type
	static void stringToCollapseError(const casacore::String &text, SpectralCollapser::CollapseError &collError);

	// Convert from collapse type to string
   static void collapseTypeToString(const SpectralCollapser::CollapseType &collType, casacore::String &strCollType);

   // Convert from error type to string
   static void collapseErrorToString(const SpectralCollapser::CollapseError &collError, casacore::String &strCollError);

private:
   SPCIIF _image;
   casacore::LogIO *_log;

   casacore::String _storePath;
   casacore::String _all; // = CasacRegionManager::ALL;
   casacore::IPosition _specAxis;
   casacore::Bool _hasQualAxis;

	// Disallow default constructor
   SpectralCollapser();

   void _setUp();
   casacore::Bool _cleanTmpData(const casacore::String &tmpFileName) const;
   casacore::Bool _cleanTmpData(const casacore::String &tmpData, const casacore::String &tmpError) const;
   casacore::Bool _getQualitySubImg(const casacore::ImageInterface<casacore::Float>* image, const casacore::Bool &data, casacore::SubImage<casacore::Float> &qualitySub);
   casacore::Bool _getQualitySubImgs(SPCIIF image, std::shared_ptr<casacore::SubImage<casacore::Float> > &subData, std::shared_ptr<casacore::SubImage<casacore::Float> > &subError) const;
   casacore::Bool _getOutputName(const casacore::String &wcsInp, casacore::String &outImg, casacore::String &outImgData, casacore::String &outImgError) const;
   casacore::Bool _collapse(const SPCIIF image, const casacore::String &aggString,
   		const casacore::String& chanInp, const casacore::String& outname) const;
   casacore::Bool _moments(const casacore::ImageInterface<casacore::Float> *image, const casacore::Vector<casacore::Int> &momentVec,
   		const casacore::Int & startIndex, const casacore::Int &endIndex, const casacore::String& outname);
   casacore::Bool _mergeDataError(const casacore::String &outImg, const casacore::String &dataImg, const casacore::String &errorImg, const casacore::Float &normError=1.0) const;
	void _addMiscInfo(const casacore::String &outName, const casacore::String &wcsInput, const casacore::String &chanInput,
			const SpectralCollapser::CollapseType &collType, const SpectralCollapser::CollapseError &collError) const;
	void _collTypeToImCollString(const SpectralCollapser::CollapseType &collType, casacore::String &colType) const;
	void _collErrorToImCollString(const SpectralCollapser::CollapseError &collError, casacore::String &colError) const ;

};
}

#endif
