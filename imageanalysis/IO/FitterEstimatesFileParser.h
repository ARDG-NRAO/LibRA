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

#ifndef IMAGEANALYSIS_FITTERESTIMATESFILEPARSER_H
#define IMAGEANALYSIS_FITTERESTIMATESFILEPARSER_H

#include <casa/aips.h>
#include <casa/OS/RegularFile.h>
#include <components/ComponentModels/ComponentList.h>
#include <images/Images/ImageInterface.h>
#include <memory>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// Class for parsing a file which holds initial estimates for 2-D components. Used by imfit.
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="tFitterEstimatesFileParser" demos="">
// </reviewed>

// <prerequisite> 
//    <li> <linkto class=RegularFilebufIO>casacore::FilebufIO</linkto> class
// </prerequisite>

// <synopsis> 
// Used for reading files containing initial estimates of models for 2-D fitting.
// The expected format is:
// <ol>
// <li>Lines with a "#" in column 1 are treated as comments and ignored.</li>
// <li> Each non-comment line is treated as a complete set of six comma-delimited
//      parameters for specifying an estimate for a component. The values are
//      Flux (quantity), x pixel position of peak (double), y pixel position of peak (double),
//      FWHM major axis (quantity), FWHM minor axis (quantity), position angle (measured from
//      north to east (quantity).</li>
// <li> Optionally, a component estimate line can have a seventh parameter which is
//      a string specifying which of the parameters for that component should be held
//      fixed during the fit. This string can include any combination of the following
//      identifiers: "f" flux, "x" x position, "y" y position, "a" major axis,
//      "b" minor axis, "p" position angle. So, eg, "apx" means hold the major axis,
//      position angle, and x position constant during the fit. </li>
// </ol>
// If the specified file passed to the constructor does not exist, an exception is thrown.
// Parsing is done during object construction and an exception is thrown if the file
// does not have the expected format.
// </synopsis>
//
// <example>
// <srcblock>
//   FitterEstimatesFilebFileReader reader("myEstimates.txt", myImage);
//   ComponentList cl = reader.getEstimates();
//   casacore::Vector<casacore::String> fixed = reader.getFixed();
// </srcblock>
// </example>

class FitterEstimatesFileParser {
	public:

    FitterEstimatesFileParser() = delete;

		// Constructor
		// <src>filename</src> Name of file containing estimates
		// <src>image</src> Image for which the estimates apply
    template <class T> explicit FitterEstimatesFileParser(
        const casacore::String& filename,
        const casacore::ImageInterface<T>& image
    );

		~FitterEstimatesFileParser();

		// Get the estimates specified in the file as a ComponentList object.
		ComponentList getEstimates() const;

		// Get the fixed parameter masks specified in the file.
		casacore::Vector<casacore::String> getFixed() const;

		// Get the contents of the file
		casacore::String getContents() const;

	private:
		ComponentList _componentList;
		casacore::Vector<casacore::String> _fixedValues;
		std::unique_ptr<casacore::LogIO> _log;
		casacore::Vector<casacore::Double> _peakValues, _xposValues, _yposValues;
		casacore::Vector<casacore::Quantity> _majValues, _minValues, _paValues;
		casacore::String _contents;

		// parse the file
		void _parseFile(const casacore::RegularFile& myFile);
		template <class T> void _createComponentList(
		    const casacore::ImageInterface<T>& image
		);
};

}

#ifndef AIPS_NO_TEMPLATE_SRC
#include <imageanalysis/IO/FitterEstimatesFileParser2.tcc>
#endif

#endif
