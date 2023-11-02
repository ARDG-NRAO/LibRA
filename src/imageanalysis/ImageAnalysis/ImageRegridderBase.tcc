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

#include <imageanalysis/ImageAnalysis/ImageRegridderBase.h>

namespace casa {

template <class T> ImageRegridderBase<T>::ImageRegridderBase(
	const SPCIIT image, const casacore::Record *const regionRec,
	const casacore::String& maskInp, const casacore::String& outname,
	casacore::Bool overwrite, const casacore::CoordinateSystem& csys,
	const casacore::IPosition& axes, const casacore::IPosition& shape
) : ImageTask<T>(
		image, "", regionRec, "", "", "", maskInp, outname, overwrite
	), _csysTo(csys), _axes(axes), _shape(shape),
	_specAsVelocity(false), _doRefChange(false),
	_replicate(false), _forceRegrid(false), _decimate(10),
	_method(casacore::Interpolate2D::LINEAR), _outputStokes(),
	_nReplicatedChans(0) {
    ThrowIf(
        this->_isPVImage(),
        "PV images are not supported. Please first regrid the image from which "
        "the PV image was generated, and then create the PV image from that "
        "regridded image"
    );
	this->_construct();
	_finishConstruction();
}

template <class T> ImageRegridderBase<T>::~ImageRegridderBase() {}

template <class T>
casacore::Bool ImageRegridderBase<T>::_regriddingDirectionAxes() const {
	auto dirAxesNumbers = _csysTo.directionAxesNumbers();
	if (! dirAxesNumbers.empty()) {
		auto v = dirAxesNumbers.tovector();
		for (casacore::Int i=0; i<(casacore::Int)_axes.size(); ++i) {
			if (std::find(v.begin(), v.end(), _axes[i]) != v.end()) {
				return true;
			}
		}
	}
	return false;
}

template <class T> void ImageRegridderBase<T>::setDecimate(casacore::Int d) {
	if (d > 1 && _regriddingDirectionAxes()) {
		auto dirAxesNumbers = _csysTo.directionAxesNumbers();
		auto v = dirAxesNumbers.tovector();
		for (casacore::Int i=0; i<(casacore::Int)_axes.size(); i++) {
			casacore::Int axis = _axes[i];
			ThrowIf(
				(casacore::Int)_shape[axis] < 3*d
				&& std::find(v.begin(), v.end(), axis) != v.end(),
				"The output image has only "
				+ casacore::String::toString(_shape[axis])
				+ " pixels along axis " + casacore::String::toString(axis)
				+ ", so the maximum value of decimate should "
				"be " + casacore::String::toString(_shape[axis]/3)
			);
		}
	}
	_decimate = d;
}

template <class T> void ImageRegridderBase<T>::_finishConstruction() {
	casacore::Bool shapeSpecified = ! _shape.empty() && _shape[0] >= 0;
	if (! shapeSpecified) {
		casacore::IPosition imShape = this->_getImage()->shape();
		_shape.resize(imShape.size());
		_shape = imShape;
		if (this->_getDropDegen()) {
			casacore::IPosition tmp = _shape.nonDegenerate();
			_shape.resize(tmp.size());
			_shape = tmp;
		}
	}
	_kludgedShape = _shape;
	const auto& csysFrom = this->_getImage()->coordinates();
	// enforce stokes rules CAS-4960
	if (
	    csysFrom.hasPolarizationCoordinate()
	    && _csysTo.hasPolarizationCoordinate()
	) {
		auto templateStokes = _csysTo.stokesCoordinate().stokes();
		auto inputStokes = csysFrom.stokesCoordinate().stokes();
		auto inputPolAxisNumber = csysFrom.polarizationAxisNumber();
		if (
			(
				_axes.empty()
				|| inputPolAxisNumber < (casacore::Int)_axes.size()
			) && templateStokes.size() > 1
		) {
			if (
				(
					_axes.empty() && inputStokes.size() > 1
				)
				|| _axes[inputPolAxisNumber] > 0
			) {
				auto stokesFrom = csysFrom.stokesCoordinate();
				auto stokesTo = _csysTo.stokesCoordinate();
				casacore::Stokes::StokesTypes valFrom, valTo;
				for (casacore::uInt i=0; i<inputStokes.size(); i++) {
					stokesFrom.toWorld(valFrom, i);
					for (casacore::uInt j=0; j<templateStokes.size(); j++) {
						stokesTo.toWorld(valTo, j);
						if (valFrom == valTo) {
							_outputStokes.push_back(
							    casacore::Stokes::name(valFrom)
							);
							break;
						}
					}
				}
				ThrowIf(
					_outputStokes.empty(),
					"Input image and template coordinate "
					"system have no common stokes."
				);
				ThrowIf(
					shapeSpecified && (
					    (casacore::Int)_outputStokes.size()
					    != _shape[inputPolAxisNumber]
					),
					"Specified output stokes axis length ("
					+ casacore::String::toString(_shape[inputPolAxisNumber])
					+ ") does not match the number of common stokes ("
					+ casacore::String::toString(_outputStokes.size())
					+ ") in the input image and template coordinate system."
				);
				// This is a kludge to fool the underlying casacore::ImageRegrid
				// constructor that the shape is acceptable to it. We copy just
				// the stokes we from the output of ImageRegrid.
				ImageMetaData<T> md(this->_getImage());
				_kludgedShape[csysFrom.polarizationAxisNumber(false)]
				    = md.nStokes();
			}
		}
	}
	casacore::Int spectralAxisNumber = csysFrom.spectralAxisNumber(false);
	if (
		csysFrom.hasSpectralAxis() && _csysTo.hasSpectralAxis()
		&& this->_getImage()->shape()[spectralAxisNumber] == 1
		&& ! _axes.empty()
	) {
		casacore::uInt count = 0;
		for( casacore::Int axis: _axes ) {
			if (axis == spectralAxisNumber) {
				*this->_getLog() << casacore::LogIO::NORMAL << "You've "
				    << "specified explicitly that the spectral axis should be "
					<< "regridded. However, the input image has a "
					<< "degenerate spectral axis and so it cannot be "
					<< "regridded. Instead, the resulting single output "
					<< "channel will be replicated " << _shape[axis]
					<< " times in the output image." << casacore::LogIO::POST;
				casacore::IPosition newAxes(_axes.size() - 1, 0);
				casacore::IPosition toRemove(1, count);
				newAxes = _axes.removeAxes(toRemove);
				_axes.resize(newAxes.size());
				_axes = newAxes;
				_nReplicatedChans = _shape[axis];
				_kludgedShape[axis] = 1;

				break;
			}
			count++;
		}
	}
}

template <class T> template <class U>
void ImageRegridderBase<T>::setConfiguration(
    const ImageRegridderBase<U>& that
) {
	_method = that._method;
	_decimate = that._getDecimate();
	_replicate = that._getReplicate();
	_doRefChange = that._getDoRefChange();
	_forceRegrid = that._getForceRegrid();
	this->setStretch(that._getStretch());
	_specAsVelocity = that._specAsVelocity;
	this->setDropDegen(that._getDropDegen());
}

}
