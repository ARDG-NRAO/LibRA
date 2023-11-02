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

#ifndef IMAGEANALYSIS_PEAKINTENSITYFLUXDENSITYCONVERTER_TCC
#define IMAGEANALYSIS_PEAKINTENSITYFLUXDENSITYCONVERTER_TCC

#include <imageanalysis/ImageAnalysis/PeakIntensityFluxDensityConverter.h>

#include <components/ComponentModels/SkyCompRep.h>
#include <casacore/images/Images/ImageUtilities.h>

namespace casa {

template<class T>
PeakIntensityFluxDensityConverter<T>::PeakIntensityFluxDensityConverter(
		const SPCIIT  image
) : ImageTask<T>(
		image, "", 0, "", "", "", "", "", ""
	), _size(Angular2DGaussian::NULL_BEAM), _shape(ComponentType::GAUSSIAN),
	_beam(casacore::GaussianBeam::NULL_BEAM) {
	this->_construct(false);
}

template<class T>
PeakIntensityFluxDensityConverter<T>::~PeakIntensityFluxDensityConverter() {}

template<class T>
void PeakIntensityFluxDensityConverter<T>::setBeam(
    casacore::Int channel, casacore::Int polarization
) {
	_beam = this->_getImage()->imageInfo().restoringBeam(channel, polarization);
}

template<class T>
casacore::Quantity PeakIntensityFluxDensityConverter<T>::fluxDensityToPeakIntensity(
	casacore::Bool& hadToMakeFakeBeam, const casacore::Quantity& fluxDensity
) const {

	hadToMakeFakeBeam = false;
	const auto& csys = this->_getImage()->coordinates();
	const auto& brightnessUnit = this->_getImage()->units();
	GaussianBeam beam = _beam;
	if (brightnessUnit.getName().contains("/beam") && beam.isNull()) {
		beam = ImageUtilities::makeFakeBeam(
			*this->_getLog(), csys,
			this->_getVerbosity() >= ImageTask<T>::WHISPER
		);
		hadToMakeFakeBeam = true;
	}
	return SkyCompRep::integralToPeakFlux(
		csys.directionCoordinate(), _shape, fluxDensity, brightnessUnit,
		_size.getMajor(), _size.getMinor(), beam
	);
}

template<class T>
casacore::Quantity PeakIntensityFluxDensityConverter<T>::peakIntensityToFluxDensity(
	casacore::Bool& hadToMakeFakeBeam, const casacore::Quantity& peakIntensity
) const {
	hadToMakeFakeBeam = false;
	const auto& brightnessUnit = this->_getImage()->units();
	const auto& csys = this->_getImage()->coordinates();
	auto beam = _beam;
	if (brightnessUnit.getName().contains("/beam") && beam.isNull()) {
		beam = ImageUtilities::makeFakeBeam(
			*this->_getLog(), csys,
			this->_getVerbosity() >= ImageTask<T>::WHISPER
		);
		hadToMakeFakeBeam = true;
	}
	return SkyCompRep::peakToIntegralFlux(
		csys.directionCoordinate(),
		_shape, peakIntensity,
		_size.getMajor(), _size.getMinor(), beam
	);
}

}

#endif
