#include <imageanalysis/ImageAnalysis/ImageRebinner.h>

namespace casa {

template<class T> ImageRebinner<T>::ImageRebinner(
	const SPCIIT image,
	const casacore::Record *const region,
	const casacore::String& maskInp,
	const casacore::String& outname, casacore::Bool overwrite
) : ImageTask<T>(
		image, "", region, "", "", "",
		maskInp, outname, overwrite
	),
	_factors(), _crop(false) {
        this->_construct();
    }

template<class T> void ImageRebinner<T>::setFactors(const casacore::Vector<casacore::Int>& f) {
	ThrowIf(
		f.empty(),
		"Rebinning factor vector cannot be empty"
	);
	ThrowIf(
		anyTrue(f <= 0),
		"All factors must be positive"
	);
	ThrowIf(
		allTrue(f == 1),
		"All rebinning factors are 1, which "
		"means rebinning cannot occur"
	);
	casacore::uInt ndim = this->_getImage()->ndim();
	ThrowIf(
		f.size() > ndim,
		"Factor vector length must be less than or "
		"equal to the number of input image axes"
	);
	casacore::Vector<casacore::Int> fcopy = f.copy();
	casacore::uInt mysize = f.size();
	if (mysize < ndim) {
		fcopy.resize(ndim, true);
		for (casacore::uInt i=mysize; i<ndim; i++) {
			fcopy[i] = 1;
		}
		*this->_getLog() << casacore::LogOrigin(getClass(), __func__)
			<< casacore::LogIO::NORMAL << "Not all rebinning factors were specified, "
			<< "filling in those not specified with factors of "
			<< "1, so will rebin according to factor=" << fcopy
			<< casacore::LogIO::POST;
	}
	const casacore::CoordinateSystem& csys = this->_getImage()->coordinates();
	ThrowIf(
		csys.hasPolarizationCoordinate() && fcopy[csys.polarizationAxisNumber()] > 1,
		"A polarization axis cannot be rebinned"
	);
	_factors = fcopy;
}

template<class T> SPIIT ImageRebinner<T>::rebin() const {
	ThrowIf(_factors.empty(), "Logic Error: factors have not been set");
    SPCIIT image = this->_getImage();
    casacore::uInt ndim = image->ndim();
    ThrowIf(
        ndim != _factors.size(),
        "You have provided " + casacore::String::toString(_factors.size())
        + " factors. You must provide exactly " + casacore::String::toString(ndim)
        + (
            this->_getDropDegen()
                ? ". If you wish to drop degenerate axes, specify binning factors of 1 for them"
                : ""
        )
    );
    casacore::IPosition myFactors;
    if (this->_getDropDegen()) {
        casacore::IPosition shape = image->shape();
        casacore::IPosition degenAxes;
        for (casacore::uInt i=0; i<ndim; i++) {
            if (shape[i] == 1) {
                degenAxes.append(casacore::IPosition(1, i));
            }
        }
        myFactors = _factors.removeAxes(degenAxes);
    }
    else { 
        myFactors = _factors;
    }
    SPIIT subImage(
		SubImageFactory<T>::createImage(
			*this->_getImage(), "", *this->_getRegion(),
			this->_getMask(), this->_getDropDegen(),
			false, false, this->_getStretch()
		)
	);
	if (_crop) {
        ndim = subImage->ndim();
		casacore::IPosition shape = subImage->shape();
		casacore::IPosition trc = shape - 1;
		casacore::Vector<casacore::Int> mods(ndim);
		for (casacore::uInt i=0; i<ndim; i++) {
			mods[i] = shape[i] % myFactors[i];
			if (mods[i] > 0) {
				trc[i] -= mods[i];
			}
		}
		if (anyTrue(mods > 0)) {
			casacore::LCBox box(casacore::IPosition(ndim, 0), trc, shape);
			subImage = 	SubImageFactory<T>::createImage(
				*subImage, "", box.toRecord(""),
				"", false, false, false, false
			);
		}
	}
	casacore::RebinImage<T> binIm(*subImage, myFactors);
	SPIIT outIm = this->_prepareOutputImage(binIm, this->_getDropDegen());
	/*
	// remove any axes that have been binned into a remaining degenerate axis,
	// CAS-5836
	if (this->_getDropDegen()) {
		casacore::IPosition outShape = outIm->shape();
		casacore::uInt outDim = outIm->ndim();
		for (casacore::uInt i=0; i<outDim; ++i) {
			if (outShape[i] == 1) {
				casacore::Record empty;
				outIm = SubImageFactory<T>::createImage(
					*outIm, this->_getOutname(), casacore::Record(),
					"", true, true, false, false
				);
				break;
			}
		}
	}
	*/
	return outIm;
}

}
