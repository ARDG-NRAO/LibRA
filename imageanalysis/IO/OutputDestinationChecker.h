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

#ifndef IMAGES_OUTPUTDESTINATIONCHECKER_H
#define IMAGES_OUTPUTDESTINATIONCHECKER_H

#include <casacore/casa/aips.h>
#include <casacore/casa/namespace.h>

#include <casacore/casa/BasicSL/String.h>

#include <casacore/casa/Logging/LogIO.h>

#include <vector>

namespace casa {

class OutputDestinationChecker {
	// <summary>
	// </summary>

	// <reviewed reviewer="" date="" tests="" demos="">
	// </reviewed>

	// <prerequisite>
	// </prerequisite>

	// <etymology>
	// </etymology>

	// <synopsis>
	// </synopsis>

public:

	// struct for checking output file writability
	struct OutputStruct {
		// label used for messages, eg "residual image", "estmates file"
		casacore::String label;
		// pointer to the output name
		casacore::String *outputFile;
		// is this file required to be written, or can the task continue if it cannot be?
		casacore::Bool required;
		// If a file by the same name already exists, will the task allow it to be overwritten?
		casacore::Bool replaceable;
	};

    OutputDestinationChecker();

	//Destructor
	~OutputDestinationChecker();

    static void checkOutputs(std::vector<OutputStruct> *const output, casacore::LogIO& log);

    static void checkOutput(OutputStruct& output, casacore::LogIO& log);

    // Get the number of channels that have been selected. The process() method must
    // be called prior to calling this method or an exception is thrown.
    casacore::uInt nSelectedChannels() const;

};

}

#endif /* IMAGES_IMAGEINPUTPROCESSOR_H */
