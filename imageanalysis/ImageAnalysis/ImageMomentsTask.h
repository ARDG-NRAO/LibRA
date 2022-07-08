#ifndef IMAGEANALYSIS_IMAGEMOMENTSTASK_H
#define IMAGEANALYSIS_IMAGEMOMENTSTASK_H

#include <imageanalysis/ImageAnalysis/ImageTask.h>

#include <casacore/casa/namespace.h>

namespace casa {

class ImageMomentsProgressMonitor;

template <class T> class ImageMomentsTask : public ImageTask<T> {
	// <summary>
	// Top level interface for creating image moments
	// </summary>

	// <reviewed reviewer="" date="" tests="" demos="">
	// </reviewed>

	// <prerequisite>
	// </prerequisite>

	// <etymology>
	// Rebins an image to larger pixels.
	// </etymology>

	// <synopsis>
	// Top level interface for rebinning images.
	// </synopsis>

public:

    ImageMomentsTask() = delete;

    // overwrite only applies to the smooth image. The moment images should
    // be removed if they exist.
	ImageMomentsTask(
		const SPCIIT image, const casacore::Record *const region,
		const casacore::String& mask, const casacore::String& smoothImageName,
		casacore::Bool overwrite
	);

	~ImageMomentsTask() {}

	casacore::String getClass() const { const static casacore::String s = "ImageMomentsTask"; return s; }

	// the first requested moments image is returned.
	SPIIT makeMoments() const;

	// set moments axis number
	void setAxis(casacore::Int axis);

	// set the include or exclude range. An empty vector means no range is set,
	// a one element vector means -range[0] to range[0] is set. If the vector
	// contains more than two elements, an exception is thrown. If isInclude is
	// true, then the range is the range to be included, excluded if false.
	void setIncludeExcludeRange(const std::vector<T>& range, casacore::Bool isInclude);

	// Set smoothing kernels
	void setKernels(const std::vector<casacore::String>& kernels) { _kernels = kernels; }

	// set smoothing kernel widths
	void setKernelWidths(const std::vector<casacore::Quantity>& kernelWidths) {
	    _kernelWidths = kernelWidths;
	}

	// set moment methods
	void setMethods(const std::vector<casacore::String>& methods) { _methods = methods; }

	// set moments to compute
	void setMoments(const casacore::Vector<casacore::Int>& moments);

	void setMomentsProgressMonitor(ImageMomentsProgressMonitor* progressMonitor ) {
	    _imageMomentsProgressMonitor = progressMonitor;
	}

	// set the base name for the moment images.
	void setMomentImageName(const casacore::String& name) { _momentName = name; }

	// Should the moment axis be removed. These rules will be followed:
	// If the moment axis is associated with a coordinate with one axis only,
	// the axis and its coordinate are physically removed from the output image.  Otherwise,
	// if <src>remove=true</src> then the output axis is logically removed from the
	// the output CoordinateSystem.  If <src>remove=false</src> then the axis
	// is retained with shape=1 and with its original coordinate information (which
	// is probably meaningless).
	void setRemoveAxis(casacore::Bool remove) { _removeAxis = remove; }

	// set smoothing axes
	void setSmoothAxes(const std::vector<casacore::uInt>& axes);

	// set desired signal-to-noise. Less than or equal to zero means
	// use the default value.
	void setSNR(casacore::Double snr) { _snr = snr; }

	// set desired standard deviation. Less than or equal to zero means
	// set to 0.
	void setStdDev(casacore::Double stddev) { _stddev = stddev; }

	// set the velocity type.
	void setVelocityType(const casacore::String& type) { _velocityType = type; }

protected:

	CasacRegionManager::StokesControl _getStokesControl() const {
		return CasacRegionManager::USE_ALL_STOKES;
	}

	std::vector<casacore::Coordinate::Type> _getNecessaryCoordinates() const {
		return std::vector<casacore::Coordinate::Type>();
	}

    casacore::Bool _supportsMultipleBeams() const {return true;}

    casacore::Bool _supportsMultipleRegions() const {return true;}

private:
    casacore::Vector<casacore::Int> _moments;
    casacore::Int _axis;
    std::vector<casacore::String> _methods, _kernels;
    std::vector<casacore::uInt> _smoothAxes;
    std::vector<casacore::Quantity> _kernelWidths;
    std::vector<T> _range;
    casacore::Bool _isIncludeRange, _removeAxis;
    casacore::Double _snr, _stddev;
    casacore::String _velocityType, _momentName;
    ImageMomentsProgressMonitor* _imageMomentsProgressMonitor;

    void _deleteTempImage(const casacore::String& tmpImage) const;

};
}

#ifndef AIPS_NO_TEMPLATE_SRC
#include <imageanalysis/ImageAnalysis/ImageMomentsTask.tcc>
#endif

#endif
