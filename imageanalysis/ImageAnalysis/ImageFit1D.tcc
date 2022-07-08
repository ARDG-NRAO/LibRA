//# ImageFit1D.cc: Class to fit Spectral components to vectors in an image
//# Copyright (C) 2004
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
//#   $Id: ImageFit1D.tcc 20652 2009-07-06 05:04:32Z Malte.Marquarding $

#include <imageanalysis/ImageAnalysis/ImageFit1D.h>

#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/coordinates/Coordinates/CoordinateUtil.h>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/SubImage.h>
#include <casacore/images/Regions/ImageRegion.h>
#include <casacore/lattices/Lattices/LatticeUtilities.h>
#include <casacore/lattices/LatticeMath/LatticeMathUtil.h>
#include <components/SpectralComponents/SpectralEstimate.h>
#include <components/SpectralComponents/SpectralElement.h>
#include <casacore/casa/Utilities/Assert.h>

#include <memory>

namespace casa {

/*
template <class T> 
ImageFit1D<T>::ImageFit1D()
 : _image(0),
   _weights(0),
   _axis(0),
   _converged(false), _success(false), _isValid(true), _x(0)
{
   checkType();
}
*/

template <class T> 
ImageFit1D<T>::ImageFit1D(
	std::shared_ptr<const casacore::ImageInterface<T> > image, casacore::uInt pixelAxis
) : _image(image), _weights(), _axis(pixelAxis),
	_converged(false), _success(false), _isValid(true)
{
   //checkType();
   //setImage(image, pixelAxis);
	_construct();
}

template <class T> 
ImageFit1D<T>::ImageFit1D(
	std::shared_ptr<const casacore::ImageInterface<T> > image,
	std::shared_ptr<const casacore::ImageInterface<T> > weights,
	casacore::uInt pixelAxis
) : _image(image), _weights(weights),
   _axis(pixelAxis), _converged(false), _success(false), _isValid(true)
{
   //checkType();
   //setImage(image, pixelAxis);
   // setWeightsImage (weights);
	_construct();
}


template <class T> 
ImageFit1D<T>::ImageFit1D(const ImageFit1D<T>& other)
 : _image(other._image), _weights(other._weights),
   _axis(other._axis), _converged(false), _success(false), _isValid(true)
{
   //checkType();
   copy(other);
}

template <class T> 
ImageFit1D<T>& ImageFit1D<T>::operator=(const ImageFit1D<T>& other)
{
  if (this != &other) {
     copy(other);
  }
  return *this;
}

template <class T> ImageFit1D<T>::~ImageFit1D() {}

template <class T> 
void ImageFit1D<T>::_construct() {
	checkType();
	_resetFitter();
   AlwaysAssert(_axis < _image->ndim(), casacore::AipsError);

   if (_weights) {
	   AlwaysAssert (_image->shape().isEqual(_weights->shape()), casacore::AipsError);
   }
   else {
	   _unityWeights.resize(_image->shape()[_axis], false);
	   _unityWeights = 1.0;
   }
   _weightSlice.resize(_image->shape()[_axis], false);
   _sliceShape = casacore::IPosition(_image->ndim(), 1);
   _sliceShape[_axis] = _image->shape()[_axis];
}

/*
template <class T> 
void ImageFit1D<T>::setImage (const casacore::ImageInterface<T>& image,
                              const casacore::ImageInterface<T>& weights,
                              casacore::uInt pixelAxis)
{
	_resetFitter();
   setImage(image, pixelAxis);
   setWeightsImage(weights);
}
*/

template <class T>  void ImageFit1D<T>::setData (
	const casacore::IPosition& pos,
	/*const ImageFit1D<T>::AbcissaType abcissaType,
	const casacore::Bool doAbs, const casacore::Double* const &abscissaDivisor,
    casacore::Array<casacore::Double> (*xfunc)(const casacore::Array<casacore::Double>&), */
    casacore::Array<FitterType> (*yfunc)(const casacore::Array<FitterType>&)
) {
	_resetFitter();
	/*
	const casacore::uInt nDim = _image->ndim();
	casacore::IPosition start(nDim);
	start(_axis) = 0;
	for (casacore::uInt i=0; i<nDim; i++) {
		if (i!=_axis) {
			start(i) = pos(i);
		}
	}
	*/
	casacore::IPosition start = pos;
	start[_axis] = 0;
	// Get ordinate data

	casacore::Vector<T> y;
	y = _image->getSlice(start, _sliceShape, true);

	// Mask

	casacore::Vector<casacore::Bool> mask;
	mask = _image->getMaskSlice(start, _sliceShape, true);

	// Weights

	if (_weights.get()) {
		convertArray(_weightSlice, _weights->getSlice(start, _sliceShape, true));
	}
	else {
		_weightSlice = _unityWeights;
	}
	// Set data in fitter; we need to use a casacore::Double fitter at present
	casacore::Vector<FitterType> y2(y.shape());
	convertArray(y2, y);
	if (yfunc) {
		y2 = (*yfunc)(y2);
		// in some cases, the supplied function will return NAN values, eg
		// log(y) will return NAN for nonpositive y values. Just mask those.
		mask = mask && ! isNaN(y2);
	}
	ThrowIf(
		!_fitter.setData (_x, y2, mask, _weightSlice),
		_fitter.errorMessage()
	);
}

template <class T>  void ImageFit1D<T>::setData (
	const casacore::IPosition& pos,
	const ImageFit1D<T>::AbcissaType abcissaType,
	const casacore::Bool doAbs, const casacore::Double* const &abscissaDivisor,
    casacore::Array<casacore::Double> (*xfunc)(const casacore::Array<casacore::Double>&),
    casacore::Array<FitterType> (*yfunc)(const casacore::Array<FitterType>&)
) {
	_resetFitter();
	/*
	const casacore::uInt nDim = _image->ndim();
	casacore::IPosition start(nDim);
	start(_axis) = 0;
	for (casacore::uInt i=0; i<nDim; i++) {
		if (i!=_axis) {
			start(i) = pos(i);
		}
	}
	*/

	casacore::IPosition start = pos;
	start[_axis] = 0;

	// Get ordinate data

	casacore::Vector<T> y;
	y = _image->getSlice(start, _sliceShape, true);

	// Mask

	casacore::Vector<casacore::Bool> mask;
	mask = _image->getMaskSlice(start, _sliceShape, true);

	// Weights

	if (_weights.get()) {
		convertArray(_weightSlice, _weights->getSlice(start, _sliceShape, true));
	}
	else {
		_weightSlice = _unityWeights;
	}

	// Generate Abscissa

	casacore::Vector<casacore::Double> x = _x.copy();
	if (x.size() == 0) {
		x = makeAbscissa(abcissaType, doAbs, abscissaDivisor);
		if (xfunc) {
			x = (*xfunc)(x);
		}
	}
	// Set data in fitter; we need to use a casacore::Double fitter at present

	casacore::Vector<FitterType> y2(y.shape());
	convertArray(y2, y);
	if (yfunc) {
		y2 = (*yfunc)(y2);
		// in some cases, the supplied function will return NAN values, eg
		// log(y) will return NAN for nonpositive y values. Just mask those.
		mask = mask && ! isNaN(y2);
	}
	ThrowIf(
		!_fitter.setData (_x, y2, mask, _weightSlice),
		_fitter.errorMessage()
	);
}

/*
template <class T> casacore::Bool ImageFit1D<T>::setData (
		const casacore::ImageRegion& region,
		const ImageFit1D<T>::AbcissaType abcissaType,
		const casacore::Bool doAbs)
		{
	_resetFitter();
	// Make SubImage

	const casacore::SubImage<T> subImage(*_image, region, false);

	// Average over non-profile axes

	const casacore::uInt nDim = subImage.ndim();
	casacore::IPosition axes = casacore::IPosition::otherAxes(nDim, casacore::IPosition(1,_axis));
	casacore::Bool dropDeg = true;
	casacore::Vector<T> y;
	casacore::Vector<casacore::Bool> mask;
	casacore::LatticeMathUtil::collapse (y, mask, axes, subImage, dropDeg);

	// Weights

	casacore::Vector<T> weights(y.nelements());
	weights = 1.0;
	if (_weights.get()) {
		casacore::LatticeMathUtil::collapse (weights, axes, *_weights, dropDeg);
	}

	// Generate Abcissa

	casacore::Vector<casacore::Double> x = _x;
	if (x.size() == 0) {
		x = makeAbscissa(abcissaType, doAbs, 0);
	}

	// Set data in fitter; we need to use a casacore::Double fitter at present

	casacore::Vector<FitterType> y2(y.shape());
	convertArray(y2, y);
	casacore::Vector<casacore::Double> w2(weights.shape());
	convertArray(w2, weights);
	if (!_fitter.setData (x, y2, mask, w2)) {
		_error = _fitter.errorMessage();
		return false;
	}
	return true;
}
*/

template <class T> 
void ImageFit1D<T>::setGaussianElements (casacore::uInt nGauss) {
	if (nGauss > 0) {
		check();
		ThrowIf(
			!_fitter.setGaussianElements (nGauss),
			_fitter.errorMessage()
		);
	}
}

template <class T> 
bool ImageFit1D<T>::fit () {
   // check();
   _converged = _fitter.fit();
   _success = true;
   return _converged;
}

template <class T> 
bool ImageFit1D<T>::succeeded() const {
	return _success;
}

template <class T>
bool ImageFit1D<T>::converged() const {
	return _converged;
}

template <class T>
bool ImageFit1D<T>::setAbcissaState (
	casacore::String& errMsg, ImageFit1D<T>::AbcissaType& type,
    casacore::CoordinateSystem& cSys, const casacore::String& xUnit,
    const casacore::String& doppler, casacore::uInt pixelAxis
) {
	if (xUnit == "native") {
		type = ImageFit1D<T>::IM_NATIVE;
		return true;
	}
   if (xUnit.contains(casacore::String("pix"))) {
      type = ImageFit1D<T>::PIXEL;
      return true;
   }
   casacore::Unit unitKMS(casacore::String("km/s"));

   auto isSpectral = cSys.spectralAxisNumber(false) == (casacore::Int)pixelAxis;

// Defer unit making until now as 'pix' not a valid unit

   casacore::Bool ok(false);
   casacore::Unit unit(xUnit);
   if (unit==unitKMS && isSpectral) {
      ok = casacore::CoordinateUtil::setSpectralState (errMsg, cSys, xUnit, doppler);
      type = ImageFit1D<T>::VELOCITY;
   } else {
      casacore::Vector<casacore::String> units = cSys.worldAxisUnits().copy();
      units(pixelAxis) = xUnit;
      ok = cSys.setWorldAxisUnits(units);
      if (!ok) errMsg = cSys.errorMessage();
      type = ImageFit1D<T>::IM_NATIVE;
   }
//
   return ok;
}

template <class T> 
casacore::Vector<casacore::Double> ImageFit1D<T>::makeAbscissa (
	ImageFit1D<T>::AbcissaType type,
	casacore::Bool doAbs, const casacore::Double* const &abscissaDivisor
) {
   const casacore::uInt n = _image->shape()(_axis);
   casacore::Vector<casacore::Double> x(n);

   const casacore::CoordinateSystem& csys = _image->coordinates();
   casacore::Double refPix = csys.referencePixel()(_axis);
   if (type==PIXEL) {
      indgen(x);
      if (!doAbs) {
    	  x -= refPix;
      }
      return x;
   }

// Find the pixel axis

   casacore::Int coord, axisInCoord;
   csys.findPixelAxis (coord, axisInCoord, _axis);
   if (type==VELOCITY) {
      AlwaysAssert(csys.type(coord)==casacore::Coordinate::SPECTRAL, casacore::AipsError);
      const casacore::SpectralCoordinate& sCoord = csys.spectralCoordinate(coord);
      casacore::Double world;
      for (casacore::uInt i=0; i<n; i++) {
         if (!sCoord.pixelToVelocity (world, casacore::Double(i))) {
            throw casacore::AipsError(sCoord.errorMessage());
         } else {
            if (doAbs) {
               x[i] = world;
            } else {
               casacore::Double worldRefVal;
               sCoord.pixelToVelocity (worldRefVal, refPix);
               x -= worldRefVal;
            }
         }
      }
   }
   else if (type==IM_NATIVE) {
      const casacore::Coordinate& gCoord = csys.coordinate(coord);
      casacore::Vector<casacore::Double> pixel(gCoord.referencePixel().copy());
      casacore::Vector<casacore::Double> world;

      for (casacore::uInt i=0; i<n; i++) {
         pixel(axisInCoord) = i;
         if (!gCoord.toWorld(world, pixel)) {
            throw casacore::AipsError(gCoord.errorMessage());
         }
//
         if (!doAbs) {
        	 gCoord.makeWorldRelative(world);
         }
         x[i] = world(axisInCoord);
      }
      if (abscissaDivisor) {
    	  x /= *abscissaDivisor;
      }
   } else {
      throw casacore::AipsError("Unrecognized abscissa type");
   }
   return x;

}


template <class T> 
void ImageFit1D<T>::check() const
{
   if (!_image.get()) {
      throw(casacore::AipsError("Image has not been set"));
   }
}

/*
template <class T> 
void ImageFit1D<T>::setWeightsImage (const casacore::ImageInterface<T>& image)
{
   AlwaysAssert (_image->shape().isEqual(image.shape()), casacore::AipsError);
   _weights.reset(image.cloneII());
}
*/

template <class T> 
void ImageFit1D<T>::copy(const ImageFit1D<T>& other)
{
	_image.reset(
		other._image.get()
			? other._image->cloneII()
			: 0
	);
	_weights.reset(
		other._weights.get()
			? other._weights->cloneII()
			: 0
	);

// These things are copies

   _axis = other._axis;
//
   _fitter = other._fitter;

   _converged = other._converged;
   _success = other._success;
   _isValid = other._isValid;
   _sliceShape = other._sliceShape;
   _unityWeights = other._unityWeights.copy();
}

template <class T> 
void ImageFit1D<T>::checkType() const
//
// At this point, ProfileFitter and SpectralFitter
// take the *Same* template type for X and Y
// To avoid precision problems we do it all in Double
// at the moment.  Later X<T> and Y<T> can be separated
 //
{
   FitterType* p=0;
   casacore::DataType tp = whatType(p);
   AlwaysAssert(tp==casacore::TpDouble, casacore::AipsError);
}


template <class T> 
casacore::Vector<T> ImageFit1D<T>::getEstimate (casacore::Int which) const
{
   casacore::Vector<FitterType> e = _fitter.getEstimate(which);
   casacore::Vector<T> t(e.shape());
   convertArray (t, e);
   return t;
}


template <class T> 
casacore::Vector<T> ImageFit1D<T>::getFit (casacore::Int which) const
{
   casacore::Vector<FitterType> f = _fitter.getFit(which);
   casacore::Vector<T> t(f.shape());
   convertArray (t, f);
   return t;
}

template <class T> 
casacore::Vector<T> ImageFit1D<T>::getResidual(casacore::Int which, casacore::Bool fit) const
{
   casacore::Vector<FitterType> r = _fitter.getResidual(which, fit);
   casacore::Vector<T> t(r.shape());
   convertArray (t, r);
   return t;
}

template <class T>  void ImageFit1D<T>::invalidate() {
	_isValid = false;
}

template <class T>
bool ImageFit1D<T>::isValid() const {
	return _isValid;
}

template <class T>
void ImageFit1D<T>::_resetFitter() {
	_fitter = ProfileFit1D<FitterType>();
	_fitter.setElements(_fitter.getList(false));
	_isValid = true;
	_converged = false;
	_success = false;
}


} //#End casa namespace
