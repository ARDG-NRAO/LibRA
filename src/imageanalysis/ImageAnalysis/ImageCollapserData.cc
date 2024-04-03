#include <imageanalysis/ImageAnalysis/ImageCollapserData.h>

#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Exceptions/Error.h>

using namespace casacore;
namespace casa {

std::shared_ptr<std::map<uInt, String>> ImageCollapserData::_funcNameMap = nullptr;
std::shared_ptr<std::map<uInt, String>> ImageCollapserData::_minMatchMap = nullptr;
std::shared_ptr<std::set<ImageCollapserData::AggregateType>>
ImageCollapserData::_degenAxesSupported = nullptr;

std::shared_ptr<const std::map<uInt, String>> ImageCollapserData::funcNameMap() {
	if (! _funcNameMap) {
		std::map<uInt, String> ref;
		ref[(uInt)FLUX] = "flux";
		ref[(uInt)MADM] = "madm";
		ref[(uInt)MAX] = "max";
		ref[(uInt)MEAN] = "mean";
		ref[(uInt)MEDIAN] = "median";
		ref[(uInt)MIN] = "min";
		ref[(uInt)NPTS] = "npts";
		ref[(uInt)RMS] = "rms";
		ref[(uInt)SQRTSUM] = "sqrtsum";
		ref[(uInt)SQRTSUM_NPIX_BEAM] = "sqrtsum_npix_beam";
		ref[(uInt)SQRTSUM_NPIX] = "sqrtsum_npix";
		ref[(uInt)STDDEV] = "stddev";
		ref[(uInt)SUM] = "sum";
		ref[(uInt)VARIANCE] = "variance";
		ref[(uInt)XMADM] = "xmadm";
		ref[(uInt)ZERO] = "zero";
		_funcNameMap.reset(new std::map<uInt, String>(ref));
	}
	return _funcNameMap;
}

std::shared_ptr<const std::map<uInt, String>> ImageCollapserData::minMatchMap() {
	if (! _minMatchMap) {
		std::map<uInt, String> ref;
		ref[(uInt)FLUX] = "f";
		ref[(uInt)MADM] = "mad";
		ref[(uInt)MAX] = "max";
		ref[(uInt)MEAN] = "mea";
		ref[(uInt)MEDIAN] = "med";
		ref[(uInt)MIN] = "mi";
		ref[(uInt)NPTS] = "n";
		ref[(uInt)RMS] = "r";
		ref[(uInt)SQRTSUM] = "sqrtsum";
		ref[(uInt)SQRTSUM_NPIX_BEAM] = "sqrtsum_npix_beam";
		ref[(uInt)SQRTSUM_NPIX] = "sqrtsum_npix";
		ref[(uInt)STDDEV] = "st";
		ref[(uInt)SUM] = "su";
		ref[(uInt)VARIANCE] = "v";
		ref[(uInt)XMADM] = "x";
		ref[(uInt)ZERO] = "z";
		_minMatchMap.reset(new std:: map<uInt, String>(ref));
	}
	return _minMatchMap;
}

ImageCollapserData::AggregateType ImageCollapserData::aggregateType(
	const String& aggString
) {
	ThrowIf (
		aggString.empty(),
		"Aggregate function name is not specified and it must be."
	);
	String agg = aggString;
	agg.downcase();
	auto funcNamePtr = funcNameMap();
	auto minMatch = minMatchMap();
	for (const auto& p: *minMatch) {
	    auto key = p.first;
	    auto minMatch = p.second;
	    auto funcName = (*funcNamePtr).at(key);
	    if (
	        agg.startsWith(minMatch)
	        && funcName.startsWith(agg)
	    ) {
	        return (AggregateType)key;
	    }
	}
	ThrowCc("Unknown aggregate function specified by " + aggString);
}

std::shared_ptr<const std::set<ImageCollapserData::AggregateType>>
ImageCollapserData::aggTypesSupportedDegenAxes() {
    if (! _degenAxesSupported) {
        std::set<AggregateType> ref;
        ref.insert(MADM);
        ref.insert(MAX);
        ref.insert(MEAN);
        ref.insert(MEDIAN);
        ref.insert(MIN);
        ref.insert(NPTS);
        ref.insert(RMS);
        ref.insert(STDDEV);
        ref.insert(SUM);
        ref.insert(VARIANCE);
        ref.insert(XMADM);
        _degenAxesSupported.reset(new std::set<AggregateType>(ref));
    }
    return _degenAxesSupported;
}


}
