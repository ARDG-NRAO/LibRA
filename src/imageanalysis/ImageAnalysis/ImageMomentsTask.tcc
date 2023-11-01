#include <imageanalysis/ImageAnalysis/ImageMomentsTask.h>

#include <imageanalysis/ImageAnalysis/ImageMoments.h>

#include <algorithm>

namespace casa {

template<class T> ImageMomentsTask<T>::ImageMomentsTask(
	const SPCIIT image,	const Record *const region,
	const String& maskInp, const String& outname, Bool overwrite
) : ImageTask<T>(
	image, "", region, "", "", "", maskInp, outname, overwrite
), _moments(), _axis(-1), _methods(), _kernels(), _smoothAxes(),
    _kernelWidths(), _range(), _isIncludeRange(false),
    _removeAxis(true), _snr(-1), _stddev(0), _velocityType("RADIO"),
    _momentName(), _imageMomentsProgressMonitor(nullptr) {
    this->_construct();
}

template<class T> SPIIT ImageMomentsTask<T>::makeMoments() const {
    *this->_getLog() << LogOrigin(getClass(), __func__);
    // Note that the user may give the strings (method & kernels)
    // as either vectors of strings or one string with separators.
    // Hence the code below that deals with it.
    String tmpImageName;
    Record r;
    SPIIT pIm;
    try {
        SPCIIT x;
        if (this->_getImage()->imageType() == FITSImage::className()) {
            Path tmpImage = File::newUniqueName (".", "moments.scratch.image");
            tmpImageName = tmpImage.baseName();
            *this->_getLog() << LogIO::NORMAL
                << "Calculating moments of FITS images can be notoriously slow, "
                << "so converting to a CASA temporary paged image named "
                << tmpImageName  << " first which will be written to the current "
                << "directory" << LogIO::POST;
            x = SubImageFactory<T>::createSubImageRO(
                *this->_getImage(), *this->_getRegion(), this->_getMask(),
                this->_getLog().get(), AxesSpecifier(),
                this->_getStretch()
            );
            x = SubImageFactory<T>::createImage(
                *x, tmpImageName, r, "", false,
                false, true, false
            );
        }
        else {
            x = SubImageFactory<T>::createSubImageRO(
                *this->_getImage(), *this->_getRegion(), this->_getMask(),
                this->_getLog().get(), AxesSpecifier(),
                this->_getStretch()
            );
        }
        // Create ImageMoments object
        ImageMoments<T> momentMaker(*x, *this->_getLog(), this->_getOverwrite(), true);
        if (_imageMomentsProgressMonitor) {
            momentMaker.setProgressMonitor( _imageMomentsProgressMonitor );
        }
        // Set which moments to output, NOTE we add one from what the UI
        // gives us.
        ThrowIf(
            ! momentMaker.setMoments(_moments + 1),
            momentMaker.errorMessage()
        );
        // Set moment axis
        if (_axis >= 0) {
            momentMaker.setMomentAxis(_axis);
        }
        if (x->imageInfo().hasMultipleBeams()) {
            const CoordinateSystem& csys = x->coordinates();
            if (csys.hasPolarizationCoordinate() && _axis == csys.polarizationAxisNumber()) {
                LogOrigin logOrigin(getClass(), __func__);
                String msg = "This image has multiple beams and you are determining moments ";
                msg += "along the polarization axis. Interpret your results carefully";
                *this->_getLog() << logOrigin << LogIO::WARN << msg << LogIO::POST;
                this->addHistory(logOrigin, msg);
            }
        }
        // Set moment methods
        if (! _methods.empty() && ! _methods[0].empty()) {
            String tmp;
            for (const auto& m : _methods) {
                tmp += m + " ";
            }
            auto intmethods = momentMaker.toMethodTypes(tmp);
            ThrowIf(
                ! momentMaker.setWinFitMethod(intmethods),
                momentMaker.errorMessage()
            );
        }
        // Set smoothing
        if (
            _kernels.size() >= 1 && _kernels[0] != "" && _smoothAxes.size() >= 1
            && _kernelWidths.size() >= 1
        ) {
            auto intkernels = VectorKernel::toKernelTypes(Vector<String>(_kernels));
            ThrowIf(
                ! momentMaker.setSmoothMethod(
                    Vector<Int>(_smoothAxes), intkernels,
                    Vector<Quantity>(_kernelWidths)
                ), momentMaker.errorMessage()
            );
        }
        // Set pixel include/exclude range
        if (! _range.empty()) {
            auto includepix = _isIncludeRange ? Vector<T>(_range) : Vector<T>();
            auto excludepix = _isIncludeRange ? Vector<T>() : Vector<T>(_range);
            momentMaker.setInExCludeRange(includepix, excludepix);
        }
        // Set SNR cutoff
        momentMaker.setSnr(_snr, _stddev);
        // Set velocity type
        if (! _velocityType.empty()) {
            MDoppler::Types velType;
            if (! MDoppler::getType(velType, _velocityType)) {
                LogOrigin logOrigin(getClass(), __func__);
                ostringstream oss;
                oss << "Illegal velocity type " << _velocityType << ". Using RADIO";
                auto msg = oss.str();
                *this->_getLog() << logOrigin << LogIO::WARN << msg << LogIO::POST;
                this->addHistory(logOrigin, msg);
                velType = MDoppler::RADIO;
            }
            momentMaker.setVelocityType(velType);
        }
        // Set output names
        auto smoothout = this->_getOutname();
        ThrowIf(
            ! smoothout.empty() && ! momentMaker.setSmoothOutName(smoothout),
            momentMaker.errorMessage()
        );
        // If no file name given for one moment image, make TempImage.
        // Else PagedImage results
        Bool doTemp = _momentName.empty() && _moments.size() == 1;
        // Create moments
        auto images = momentMaker.createMoments(doTemp, _momentName, _removeAxis);
        for (auto& image: images) {
            // copy history from input to all created images
            SPIIT x = dynamic_pointer_cast<ImageInterface<T>>(image);
            this->_doHistory(x);
        }
        // Return handle of first image
        pIm = dynamic_pointer_cast<ImageInterface<T>>(images[0]);
    }
    catch (const AipsError& x) {
        _deleteTempImage(tmpImageName);
        RETHROW(x);
    }
    _deleteTempImage(tmpImageName);
    return pIm;
}

template<class T> void ImageMomentsTask<T>::_deleteTempImage(
    const String& tmpImage
) const {
    if (! tmpImage.empty()) {
        Directory dir(tmpImage);
        if (dir.exists()) {
            dir.removeRecursive(false);
        }
    }
}

template<class T> void ImageMomentsTask<T>::setAxis(Int axis) {
    ThrowIf(
        axis >= (Int)this->_getImage()->ndim(),
        "Axis " + String::toString(axis) + " doesn't exist."
        "Image has only "
        + String::toString(this->_getImage()->ndim()) + " dimensions"
    );
    _axis = axis;
}

template<class T> void ImageMomentsTask<T>::setIncludeExcludeRange(
    const vector<T>& range, Bool isInclude
) {
    ThrowIf(
       _range.size() > 2, "range cannot have more than 2 values"
    );
    _range = range;
    _isIncludeRange = isInclude;
}

template<class T> void ImageMomentsTask<T>::setMoments(
    const Vector<Int>& moments
) {
    uInt nMom = moments.nelements();
    ThrowIf(nMom == 0, "No moments requested");
    ThrowIf(
        nMom > MomentsBase<T>::NMOMENTS,
        "Too many moments specified"
    );
    _moments = moments;
}

template<class T> void ImageMomentsTask<T>::setSmoothAxes(
    const std::vector<uInt>& axes
) {
    auto mymax = *max_element(axes.begin(), axes.end());
    ThrowIf(
        mymax >= this->_getImage()->ndim(),
        "Axis " + String::toString(mymax) + " doesn't exist."
        "Image has only " + String::toString(this->_getImage()->ndim())
        + " dimensions"
    );
    _smoothAxes = axes;
}

}
