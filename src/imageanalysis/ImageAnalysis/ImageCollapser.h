#ifndef IMAGES_IMAGECOLLAPSER_H
#define IMAGES_IMAGECOLLAPSER_H

#include <imageanalysis/ImageAnalysis/ImageTask.h>

#include <imageanalysis/ImageAnalysis/ImageCollapserData.h>

#include <casacore/casa/namespace.h>

namespace casacore{

template <class T> class TempImage;
template <class T> class SubImage;
}

namespace casa {


template <class T> class ImageCollapser : public ImageTask<T> {
    // <summary>
    // Top level interface which allows collapsing of images along a single axis. An aggregate method
    // (average, sum, etc) is applied to the collapsed pixels.
    // </summary>

    // <reviewed reviewer="" date="" tests="" demos="">
    // </reviewed>

    // <prerequisite>
    // </prerequisite>

    // <etymology>
    // Collapses image.
    // </etymology>

    // <synopsis>
    // High level interface for collapsing an image along a single axis.
    // </synopsis>

    // <example>
    // <srcblock>
    // ImageCollapser collapser(...);
    // collapser.collapse();
    // </srcblock>
    // </example>

public:

    // if <src>outname</src> is empty, no image will be written
    // if <src>overwrite</src> is True, if image already exists it will be removed
    // if <src>overwrite</src> is False, if image already exists exception will be thrown
    //
    // <group>

    ImageCollapser(
        const casacore::String& aggString, SPCIIT image,
        const casacore::Record *const regionRec,
        const casacore::String& maskInp, const casacore::IPosition& axes,
        casacore::Bool invertAxesSelection,
        const casacore::String& outname, casacore::Bool overwrite
    );

    ImageCollapser(
        const SPCIIT image,
		const casacore::IPosition& axes, casacore::Bool invertAxesSelection,
        const ImageCollapserData::AggregateType aggregateType,
		const casacore::String& outname, casacore::Bool overwrite
    );
    // </group>

    ~ImageCollapser() {}

    // perform the collapse and return the resulting image.
    SPIIT collapse() const;

	casacore::String getClass() const { const static casacore::String name = "ImageCollapser"; return name; }

protected:
    inline  CasacRegionManager::StokesControl _getStokesControl() const {
        return CasacRegionManager::USE_ALL_STOKES;
    }

	inline std::vector<casacore::Coordinate::Type> _getNecessaryCoordinates() const {
		return std::vector<casacore::Coordinate::Type>();
    }

	casacore::Bool _supportsMultipleRegions() const {return true;}

private:
	casacore::Bool _invertAxesSelection;
	casacore::IPosition _axes;
    ImageCollapserData::AggregateType _aggType;

    // disallow default constructor
    ImageCollapser();

    void _doFluxUnits(
        casacore::TempImage<T>& tmpIm,
        const std::shared_ptr<const casacore::SubImage<T>> subImage
    ) const;

    void _invert();

    void _finishConstruction();

    void _checkFlux(SPCIIT subImage) const;

    void _doDegenerateAxesCase(
        casacore::TempImage<T>& tmpIm, SPCIIT subImage
    ) const;

    void _doLowPerf(
        TempImage<T>& tmpIm, SPCIIT subImage, T npixPerBeam
    ) const;

    // necessary to improve performance
    void _doHighPerf(SPCIIT image, casacore::TempImage<T>& outImage) const;

    // returns true if miscellaneous info was copied
    Bool _doMultipleBeams(
        TempImage<T>& tmpIm, SPCIIT subImage, Bool hasDir,
        const CoordinateSystem& outCoords
    ) const;

    void _doOtherStats(TempImage<T>& tmpIm, SPCIIT subImage) const;

    LatticeStatsBase::StatisticsTypes _getStatsType() const;

    static void _zeroNegatives(casacore::Array<T>& arr);

};
}

#ifndef AIPS_NO_TEMPLATE_SRC
#include <imageanalysis/ImageAnalysis/ImageCollapser.tcc>
#endif //# AIPS_NO_TEMPLATE_SRC

#endif
