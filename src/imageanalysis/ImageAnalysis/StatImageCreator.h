#ifndef IMAGEANALYSIS_STATIMAGECREATOR_H
#define IMAGEANALYSIS_STATIMAGECREATOR_H

#include <imageanalysis/ImageAnalysis/ImageStatsBase.h>

#include <casacore/scimath/Mathematics/Interpolate2D.h>

#include <casacore/casa/namespace.h>

namespace casa {

class StatImageCreator : public ImageStatsBase<Float> {
	// <summary>
	// Create a "statistic" image from an image.
	// </summary>

	// <reviewed reviewer="" date="" tests="" demos="">
	// </reviewed>

	// <prerequisite>
	// </prerequisite>

	// <etymology>
	// </etymology>

	// <synopsis>
    // See CAS-9195.
	// </synopsis>

public:

	StatImageCreator() = delete;

	// <src>region</src> is the usual region used in image analysis tasks.
	// It is the region in the image over which to perform the computations.
	// The subregion over which to carry out a single statistic computation is
	// set separately after the object has been constructed.
	StatImageCreator(
		SPCIIF image, const Record *const region, const String& mask,
		const String& outname, Bool overwrite
	);

	// destructor
	~StatImageCreator() {}

	SPIIF compute();

	String getClass() const { const static String s = "StatImageCreator"; return s; }

	// set the anchor pixel value. Note that this applies to the image used at construction,
	// not the resulting subimage if a region was also specified.
	void setAnchorPosition(Int x, Int y);

    // rounds reference pixel to nearest ints, if necessary
    void useReferencePixelAsAnchor();

	// set spacing between grid pixels.
	void setGridSpacing(uInt x, uInt y);

	// set interpolation algortihm
	void setInterpAlgorithm(Interpolate2D::Method alg);

	// set radius for circle subregion over which to carry out individual statistics computations.
	void setRadius(const Quantity& radius);

	// set a rectangular subregion over which to carry out individual statistics computations.
	void setRectangle(const Quantity& xLength, const Quantity& yLength);

	// void setStatType(casacore::LatticeStatsBase::StatisticsTypes s);
    void setStatType(casacore::StatisticsData::STATS s);

	void setStatType(const String& s);

protected:

	inline  CasacRegionManager::StokesControl _getStokesControl() const {
		return CasacRegionManager::USE_ALL_STOKES;
	}

	inline std::vector<Coordinate::Type> _getNecessaryCoordinates() const {
		return std::vector<Coordinate::Type>(1, Coordinate::DIRECTION);
	}

	Bool _mustHaveSquareDirectionPixels() const { return false; }

    Bool _supportsMultipleBeams() const {return true;}

private:

    Quantity _xlen = Quantity(1, "pix");
    Quantity _ylen = Quantity(1, "pix");

    std::pair<uInt, uInt> _grid = std::make_pair(1,1);
    // _anchor pixel world coordinates
    Vector<Double> _anchor;
    IPosition _dirAxes = IPosition(2);
    casacore::Interpolate2D _interpolater = Interpolate2D(Interpolate2D::LINEAR);
    String _interpName = "LINEAR";
    casacore::String _statName = "standard deviation";
    casacore::StatisticsData::STATS _statType
        = casacore::StatisticsData::STDDEV;
    casacore::Bool _doMask = casacore::False;
    casacore::Bool _doProbit = casacore::False;

    void _doInterpolation(
        SPIIF output, TempImage<Float>& store, SPCIIF subImage,
        uInt nxpts, uInt nypts, Int xstart, Int ystart
    ) const;

    void _computeStat(
        TempImage<Float>& writeTo, SPCIIF subImage, uInt nxpts,
        uInt nypts, Int xstart, Int ystart
    );

    void _doStatsLoop(
        casacore::TempImage<Float>& writeTo,
        casacore::RO_MaskedLatticeIterator<Float>& lattIter,
        casacore::uInt nxpts, casacore::uInt nypts, casacore::Int xstart,
        casacore::Int ystart, casacore::uInt xBlcOff, casacore::uInt yBlcOff,
        casacore::uInt xChunkSize, casacore::uInt yChunkSize,
        const casacore::IPosition& imshape,
        const casacore::IPosition& chunkShape,
        std::shared_ptr<casacore::Array<casacore::Bool>> regionMask,
        std::shared_ptr<
            casacore::StatisticsAlgorithm<
                casacore::Double,
                casacore::Array<casacore::Float>::const_iterator,
                casacore::Array<casacore::Bool>::const_iterator,
                casacore::Array<casacore::Float>::const_iterator
            >
        >& alg, const casacore::Array<casacore::Bool>& regMaskCopy,
        const casacore::IPosition& loopAxes, casacore::uInt nPts
    );

    // start is the pixel offset in the result matrix relative the
    // storage matrix. If one or both values are 0, then pixel 0 of the
    // result matrix corresponds to pixel 0 in the storage matrix. If one or both
    // of the start values are positive, then pixel 0 in the result matrix
    // corresponds to pixel (start/pointsPerCell - 1) in the storage matrix (which
    // is always negative and always greater than -1).
    void _interpolate(
        casacore::Matrix<casacore::Float>& result,
        casacore::Matrix<casacore::Bool>& resultMask,
        const casacore::Matrix<casacore::Float>& storage,
        const casacore::Matrix<casacore::Bool>& storeMask,
        const std::pair<casacore::uInt, casacore::uInt>& start
    ) const;

    // the Blc offsets are the pixel offsets from the grid point
    void _nominalChunkInfo(
        std::shared_ptr<Array<Bool>>& chunkMask,
        uInt& xBlcOff, uInt& yBlcOff, uInt& xChunkSize, uInt& yChunkSize,
        SPCIIF subimage
    ) const;

    std::shared_ptr<StatisticsAlgorithm<
        Double, Array<Float>::const_iterator, Array<Bool>::const_iterator>
    > _getStatsAlgorithm(String& algName) const;

};
}

#endif
