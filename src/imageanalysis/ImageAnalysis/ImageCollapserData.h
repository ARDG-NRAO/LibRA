#ifndef IMAGES_IMAGECOLLAPSERDATA_H
#define IMAGES_IMAGECOLLAPSERDATA_H

#include <casacore/casa/typeinfo.h>

#include <map>
#include <memory>
#include <set>

namespace casacore{
    class String;
}

namespace casa {

class ImageCollapserData {
	// <summary>
	// Non-templated data related bits for ImageCollapser.
	// </summary>

	// <reviewed reviewer="" date="" tests="" demos="">
	// </reviewed>

	// <prerequisite>
	// </prerequisite>

	// <etymology>
	// casacore::Data for ImageCollapser.
	// </etymology>

	// <synopsis>
	// </synopsis>

	// <example>
	// </example>

    //<todo>
    // None of this is thread safe
    //</todo>
public:

	enum AggregateType {
		MAX,
		MEAN,
		MEDIAN,
		MIN,
		NPTS,
		RMS,
		STDDEV,
		SUM,
		VARIANCE,
		// set all pixels in output image to 0
		ZERO,
		FLUX,
		// SQRTSUM and MEANSQRT are bizarre statistics, but
		// ImageAnalysis::getFreqProfile() supported this for the GUI profiler,
		// so sadly, we must support them here. I actually suspect the intent was for
		// the sums in these to be the sum of the squares, at least that's what the comments
		// in ImageAnalysis::getFreqProfile() seemed to imply, but they were implemented
		// as straight sums, so I followed the implementation because there is no design
		// or requirements doc to draw from. What a surprise.
		// the square root of the sum, sqrt(sum)
		SQRTSUM,
		// the square root of the sum divided by the number of pixels, sqrt(sum)/npix
		SQRTSUM_NPIX,
		// the square root of the sum divided by the number of pixels per beam, sqrt(sum)/(beam_area/pixel_area)
		// Yes that's how it was implemented in ImageAnalysis::getFreqProfile().
		SQRTSUM_NPIX_BEAM,
		// median absolute devation from mean
		MADM,
        // madm * 1/Phi^-1 
        XMADM,
		UNKNOWN
	};

	ImageCollapserData() = delete;

	ImageCollapserData(const ImageCollapserData&) = delete;

	ImageCollapserData operator=(const ImageCollapserData&) const = delete;

	// destructor
	~ImageCollapserData() {}

	static AggregateType aggregateType(const casacore::String& aggString);

	static std::shared_ptr<const std::set<AggregateType>> aggTypesSupportedDegenAxes();

	static std::shared_ptr<const std::map<casacore::uInt, casacore::String>> funcNameMap();

	static std::shared_ptr<const std::map<casacore::uInt, casacore::String>> minMatchMap();

private:

	static std::shared_ptr<std::map<casacore::uInt, casacore::String>> _funcNameMap, _minMatchMap;

	static std::shared_ptr<std::set<AggregateType>> _degenAxesSupported;

};
}

#endif
