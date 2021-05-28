//# Copyright (C) 1996,1997,1999,2002
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
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id:  $

#ifndef IMAGEANALYSIS_PROFILEFITTERESTIMATESFILEPARSER_H
#define IMAGEANALYSIS_PROFILEFITTERESTIMATESFILEPARSER_H

//# Includes
#include <casa/aips.h>
#include <casa/OS/RegularFile.h>
#include <components/SpectralComponents/SpectralList.h>
#include <images/Images/ImageInterface.h>
#include <memory>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// Class for parsing a file which holds initial estimates for 1-D components. Used by ImageProfileFitter.
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="tProfileFitterEstimatesFileParser" demos="">
// </reviewed>

// <prerequisite> 
//    <li> <linkto class=RegularFilebufIO>casacore::FilebufIO</linkto> class
// </prerequisite>

// <synopsis> 
// Used for reading files containing initial estimates of models for 1-D fitting.
// The expected format is:
// <ol>
// <li>Lines with a "#" in column 1 are treated as comments and ignored.</li>
// <li> Each non-comment line is treated as a complete set of three comma-delimited
//      parameters for specifying an estimate for a gaussian component. The values are
//      peak in image units (double), pixel position of center (double),
//		and FWHM in pixels (double)</li>
// <li> Optionally, a gaussian component estimate line can have a fourth parameter which is
//      a string specifying which of the parameters for that component should be held
//      fixed during the fit. This string can include any combination of the following
//      identifiers: "p" peak, "c" center position, "f" FWHM. So, eg, "cp" means hold the center
//      and peak constant during the fit. </li>
// </ol>
// If the specified file passed to the constructor does not exist, an exception is thrown.
// Parsing is done during object construction and an exception is thrown if the file
// does not have the expected format.
// </synopsis>
//
// <example>
// <srcblock>
//   ProfileFitterEstimatesFilebFileReader reader("myEstimates.txt", myImage);
//   SpectralList sl = reader.getEstimates();
//   vector<casacore::String> fixed = reader.getFixed();
// </srcblock>
// </example>

class ProfileFitterEstimatesFileParser {
	public:

		// Constructor
		// <src>filename</src> Name of file containing estimates
		// <src>image</src> Image for which the estimates apply
		explicit ProfileFitterEstimatesFileParser(
			const casacore::String& filename
		);

		~ProfileFitterEstimatesFileParser();

		// Get the estimates specified in the file as a ComponentList object.
		SpectralList getEstimates() const;

		// Get the fixed parameter masks specified in the file.
		std::vector<casacore::String> getFixed() const;

		// Get the contents of the file
		casacore::String getContents() const;

	private:
		const static casacore::String _class;
		SpectralList _spectralList;
		std::vector<casacore::String> _fixedValues;
		casacore::LogIO _log;
		std::vector<casacore::Double> _peakValues, _centerValues, _fwhmValues;
		casacore::String _contents;

		// parse the file
		void _parseFile(const casacore::RegularFile& myFile);
		void _createSpectralList();
};

} //# NAMESPACE CASA - END

#endif
