#ifndef IMAGEANALYSIS_IMAGEEXPRCALCULATOR_H
#define IMAGEANALYSIS_IMAGEEXPRCALCULATOR_H

#include <casa/namespace.h>

namespace casacore{

class String;
}

namespace casa {


template <class T> class ImageExprCalculator {
	// <summary>
	// Top level interface for computing the results of an image expression
	// </summary>

	// <reviewed reviewer="" date="" tests="" demos="">
	// </reviewed>

	// <prerequisite>
	// </prerequisite>

	// <etymology>
	// </etymology>

	// <synopsis>
	// </synopsis>

public:

    ImageExprCalculator() = delete;

    // expression is the lattice expression to compute
	ImageExprCalculator(
		const casacore::String& expression, const casacore::String& outname, casacore::Bool overwrite
	);

	~ImageExprCalculator() {}

	// the image from which to copy metadata, including the coordinate system,
	// imageInfo() and miscInfo()
	void setCopyMetaDataFromImage(const casacore::String& name) { _copyMetaDataFromImage = name; }

	casacore::String getClass() const { const static casacore::String s = "ImageExprCalculator"; return s; }

	// Compute the expression and return the image.
	SPIIT compute() const;


	// modify image in place
    static void compute2(SPIIT image, const casacore::String& expr, casacore::Bool verbose);

private:
    casacore::String _expr, _copyMetaDataFromImage, _outname;
    casacore::Bool _overwrite;
    mutable casacore::LogIO _log;

    SPIIT _imagecalc(
    	const casacore::LatticeExprNode& node, const casacore::IPosition& shape,
    	const casacore::CoordinateSystem& csys, const casacore::LELImageCoord* const imCoord
    ) const;

    static void _makeRegionBlock(
    	casacore::PtrBlock<const casacore::ImageRegion*>& regions,
    	const casacore::Record& Regions
    );

    void _checkImages() const;

    static void _calc(SPIIT image, const casacore::LatticeExprNode& node);
};
}

#ifndef AIPS_NO_TEMPLATE_SRC
#include <imageanalysis/ImageAnalysis/ImageExprCalculator.tcc>
#endif

#endif
