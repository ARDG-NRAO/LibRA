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

#ifndef IMAGEANALYSIS_COMPONENTLISTDECONVOLVER_H
#define IMAGEANALYSIS_COMPONENTLISTDECONVOLVER_H

#include <imageanalysis/ImageTypedefs.h>

#include <casa/namespace.h>

namespace casa {

class ComponentList;

template <class T>  class ComponentListDeconvolver {
	// <summary>
	// Top level interface for deconvolving a component list
	// </summary>

	// <reviewed reviewer="" date="" tests="" demos="">
	// </reviewed>

	// <prerequisite>
	// </prerequisite>

	// <etymology>
	// </etymology>

	// <synopsis>
	// </synopsis>

	// <example>
	// <srcblock>
	// </srcblock>
	// </example>

public:

	ComponentListDeconvolver() = delete;

	ComponentListDeconvolver(SPCIIT image);

	// destructor
	~ComponentListDeconvolver();

	ComponentList deconvolve(
		const ComponentList& compList, casacore::Int channel, casacore::Int polarization
	) const;

private:
	SPCIIT _image;

};
}

#ifndef AIPS_NO_TEMPLATE_SRC
#include <imageanalysis/ImageAnalysis/ComponentListDeconvolver.tcc>
#endif

#endif
