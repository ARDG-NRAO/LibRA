#include <imageanalysis/ImageAnalysis/TwoPointCorrelator.h>

#include <imageanalysis/ImageAnalysis/ImageTwoPtCorr.h>

namespace casa {

template<class T> TwoPointCorrelator<T>::TwoPointCorrelator(
	const SPCIIT image, const casacore::Record *const region,
	const casacore::String& maskInp,
	const casacore::String& outname, casacore::Bool overwrite
) : ImageTask<T>(
		image, region, maskInp, outname, overwrite
) {
	this->_construct();
}

template<class T> void TwoPointCorrelator<T>::setAxes(const casacore::IPosition& axes) {
	auto ndim = this->_getImage()->ndim();
	ThrowIf(
		! axes.empty() && *max_element(axes.begin(), axes.end()) >= (casacore::Int)ndim,
		"All axes must be less than the number of "
		"axes in the image which is " + casacore::String::toString(ndim)
	);
	_axes.resize(0);
	_axes = axes;
}

template<class T> SPIIT TwoPointCorrelator<T>::correlate() {
	*this->_getLog() << casacore::LogOrigin(getClass(), __func__);
	auto subImage = SubImageFactory<T>::createSubImageRO(
		*this->_getImage(), *this->_getRegion(), this->_getMask(),
		this->_getLog().get(), casacore::AxesSpecifier(), this->_getStretch()
	);

	// Deal with axes and shape
	const auto& cSysIn = subImage->coordinates();
	auto axes = ImageTwoPtCorr<T>::setUpAxes(_axes, cSysIn);
	auto shapeOut = ImageTwoPtCorr<T>::setUpShape(
		subImage->shape(), axes
	);

	// Create the output image and mask
	casacore::TempImage<T> tmp(shapeOut, cSysIn);
	casacore::String maskName("");
	ImageMaskAttacher::makeMask(tmp, maskName, true, true, *this->_getLog(), true);

	// Do the work.  The Miscellaneous items and units are dealt with
	// by function ImageTwoPtCorr::autoCorrelation
	ImageTwoPtCorr<T> twoPt;
	casacore::Bool showProgress = true;
	twoPt.autoCorrelation(tmp, *subImage, axes, _method, showProgress);
	return this->_prepareOutputImage(tmp);
}

}
