//# Copyright (C) 1998,1999,2000,2001,2003
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

#include <imageanalysis/ImageAnalysis/ImageMaskAttacher.h>

namespace casa {

template<class T> casacore::Bool ImageMaskAttacher::makeMask(
	casacore::ImageInterface<T>& out, casacore::String& maskName,
	casacore::Bool init, casacore::Bool makeDefault, casacore::LogIO& os, casacore::Bool list
) {
	os << casacore::LogOrigin("ImageMaskAttacher", __func__);
	if (out.canDefineRegion()) {
		// Generate mask name if not given
		if (maskName.empty()) {
			maskName = out.makeUniqueRegionName(casacore::String("mask"), 0);
		}
		// Make the mask if it does not exist
		if (! out.hasRegion(maskName, casacore::RegionHandler::Masks)) {
			out.makeMask(maskName, true, makeDefault, init, true);
			if (list) {
				if (init) {
					os << casacore::LogIO::NORMAL << "Created and initialized mask `"
							<< maskName << "'" << casacore::LogIO::POST;
				} else {
					os << casacore::LogIO::NORMAL << "Created mask `" << maskName << "'"
							<< casacore::LogIO::POST;
				}
			}
		}
		return true;
	}
	else {
		os << casacore::LogIO::WARN
			<< "Cannot make requested mask for this type of image"
				<< casacore::LogIO::POST;
		return false;
	}
}

} //# NAMESPACE CASA - END

