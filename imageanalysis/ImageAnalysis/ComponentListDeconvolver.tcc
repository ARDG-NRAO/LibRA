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
//# $Id: $

#include <imageanalysis/ImageAnalysis/ImageCropper.h>

#include <components/ComponentModels/ComponentList.h>
#include <components/ComponentModels/SkyComponentFactory.h>

namespace casa {

template <class T>
ComponentListDeconvolver<T>::ComponentListDeconvolver(SPCIIT image)
	: _image(image) {
	ThrowIf(
		! _image->imageInfo().hasBeam(),
		"This image does not have a restoring beam"
	);
	ThrowIf(
		! _image->coordinates().hasDirectionCoordinate(),
		"This image does not contain a direction coordinate"
	);
}

template <class T>
ComponentListDeconvolver<T>::~ComponentListDeconvolver() {}

template <class T>
ComponentList ComponentListDeconvolver<T>::deconvolve(
	const ComponentList& compList, casacore::Int channel, casacore::Int polarization
) const {
	auto n = compList.nelements();
	vector<SkyComponent> list(n);
	for (casacore::uInt i = 0; i < n; ++i) {
		list[i] = compList.component(i);
	}

	auto beam = _image->imageInfo().restoringBeam(channel, polarization);

	// Loop over components and deconvolve
	ComponentList outCL;
	casacore::LogIO log;
	for (casacore::uInt i = 0; i < n; ++i) {
		outCL.add(SkyComponentFactory::deconvolveSkyComponent(log, list[i], beam));
	}
	return outCL;
}

}


