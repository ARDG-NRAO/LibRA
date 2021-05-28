#ifndef IMAGEANALYSIS_PIXELVALUEMANIPULATOR_H
#define IMAGEANALYSIS_PIXELVALUEMANIPULATOR_H

#include <imageanalysis/ImageAnalysis/ImageTask.h>

#include <imageanalysis/ImageAnalysis/ImageCollapserData.h>
#include <imageanalysis/ImageAnalysis/PixelValueManipulatorData.h>

namespace casa {

template <class T> class PixelValueManipulator : public ImageTask<T> {
    // <summary>
    // Top level interface for getting and setting image pixel values.
    // </summary>

    // <reviewed reviewer="" date="" tests="" demos="">
    // </reviewed>

    // <prerequisite>
    // </prerequisite>

    // <etymology>
    // Manipulates pixel values.
    // </etymology>

    // <synopsis>
    // Top level interface for getting and setting image pixel values.
    // </synopsis>

    // <example>
    // </example>

public:

    enum Operator {
        ADDITION,
        SUBTRACTION,
        MULTIPLICATION,
        DIVISION,
        NONE
    };

    PixelValueManipulator() = delete;

    // regionRec = 0 => no region selected, full image used
    // mask = "" => No additional mask specification, although image default
    // mask will be used if one exists.
    PixelValueManipulator(
        const SPCIIT image, const casacore::Record *const regionRec,
        const casacore::String& mask,
        casacore::Bool verboseDuringConstruction=casacore::True
    );

    ~PixelValueManipulator() {}

    static void addNoise(
        SPIIT image, const casacore::String& type,
        const casacore::Record& region,
        const casacore::Vector<casacore::Double>& pars,
        casacore::Bool zero,
        const std::pair<casacore::Int, casacore::Int> *const &seeds
    );

    // <src>dirFrame</src> and <src>freqFrame</src> are the codes for the
    // frames for which it is desired that the returned measures should be
    // specified. In both cases, one can specify "native" for the native
    // coordinate frame, "cl" for the conversion layer frame, or any valid frame
    // string from casacore::MDirection::showType() or MFrequency::showType().
    static casacore::Record* coordMeasures(
        casacore::Quantum<T>& intensity, casacore::Record& direction,
        casacore::Record& frequency, casacore::Record& velocity,
        SPCIIT image, const casacore::Vector<casacore::Double>& pixel,
        const casacore::String& dirFrame, const casacore::String& freqFrame
    );

    // set axes to average over. If invert is true, select all axes other than
    // the specified axes to average over.
    void setAxes(const casacore::IPosition& axes, casacore::Bool invert=false);

    // Get pixel values, pixel mask values, etc.
    // The return casacore::Record has the following fields:
    // 'values' => casacore::Array<T> of pixel values
    // 'mask'   => casacore::Array<casacore::Bool> of pixel mask values
    casacore::Record get() const;

    // get a slice through the image. The values are interpolated at regular
    // intervals to provide samples at npts number of points. x and y are
    // in pixel coordinates

    static casacore::Record* getSlice(
        SPCIIT image, const casacore::Vector<casacore::Double>& x,
        const casacore::Vector<casacore::Double>& y,
        const casacore::Vector<casacore::Int>& axes,
        const casacore::Vector<casacore::Int>& coord,
        casacore::Int npts=0, const casacore::String& method="linear"
    );

    static void put(
        SPIIT image, const casacore::Array<T>& pixelsArray,
        const casacore::Vector<casacore::Int>& blc,
        const casacore::Vector<casacore::Int>& inc, casacore::Bool list,
        casacore::Bool locking, casacore::Bool replicate
    );

    static casacore::Bool putRegion(
        SPIIT image, const casacore::Array<T>& pixels,
        const casacore::Array<casacore::Bool>& mask,
        casacore::Record& region, casacore::Bool list,
        casacore::Bool usemask, casacore::Bool replicateArray
    );

    // get the aggregated values along the specified pixel axis using the region
    // and mask at construction and any other mask the image may have. Supported
    // values of <src>function</src> are (case-insensitive, minimum match) those
    // supported by ImageCollapser, ie "flux", "max", "mean", "median", "min",
    // "rms", "sqrtsum", "sqrtsum_npix", sqrtsum_npix_beam", "stddev", "sum",
    // "variance",  and "zero". Aggregation of values occurs along all axes
    // orthogonal to the one specified. One may specify the unit in which
    // coordinate values are calculated using the <src>unit</src> parameter. If
    // unit starts with "pix", then pixel coordinates are calculated, world
    // coordinates otherwise. If pixel coordinates, the values are relative to
    // the zeroth pixel on the corresponding axis of the input image.  If
    // specified and it doesn't start with "pix", the unit must be conformant
    // with the unit of <src>axis</src> in the coordinate system of the image,
    // or it must be a unit that this axis can be readily converted to (eg km/s
    // if the axis is a frequency axis with base unit of Hz). If the selected
    // axis is the spectral axis and if the unit is chosen to be something other
    // than the native spectral coordinate unit (such as velocity or wavelength
    // for a native frequency unit), <src>specType</src> indicates the system to
    // use when converting the frequency. Values of RELATVISTIC, RADIO_VELOCITY,
    // and OPTICAL_VELOCITY are only permitted if <src>unit</src> represents a
    // velocity unit. Values of WAVELENGTH and AIR_WAVELENGTH are only permitted
    // if <src>unit</src> represents a length unit. For a velocity unit, DEFAULT
    // is equivalent to RELATIVISTIC. For a length unit, DEFAULT is equivalent
    // to WAVELENGTH.
    // If the selected axis is the spectral axis and <src>unit</src> is a
    // velocity unit, <src>restFreq</src> represents the rest frequency with
    // respect to which the velocity scale should be calculated. If null, the
    // rest frequency associated with the spectral coordinate is used. If the
    // selected axis is the spectral axis, and <src>unit</src> is a frequency
    // unit, <src>frame</src> represents the frame of reference with respect to
    // which the frequency scale should be calculated. If empty, the reference
    // frame associated with the spectral coordinate is used. The return Record
    // has the following keys: "values" is a casacore::Vector<T> containing the
    // aggregate pixel values, "mask" is the associated mask values
    // (Vector<Bool>), "coords" is a casacore::Vector<casacore::Double> of
    // coordinate values, and "xUnit" is a casacore::String containing the
    // coordinate unit, and "yUnit" is a string containing the ordinate unit.
    casacore::Record getProfile(
        casacore::uInt axis, const casacore::String& function,
        const casacore::String& unit,
        PixelValueManipulatorData::SpectralType specType
            =PixelValueManipulatorData::DEFAULT,
        const casacore::Quantity *const restFreq=nullptr,
        const casacore::String& frame=""
    );

    casacore::Record getProfile(
        casacore::uInt axis, ImageCollapserData::AggregateType function,
        const casacore::String& unit,
        PixelValueManipulatorData::SpectralType specType
            =PixelValueManipulatorData::DEFAULT,
        const casacore::Quantity *const restFreq=nullptr,
        const casacore::String& frame=""
    );

    casacore::String getClass() const { return _className; }

    // region refers to the region in the image to be inserted, not the region
    // that was chosen at object construction
    static void insert(
        casacore::ImageInterface<T>& target,
        const casacore::ImageInterface<T>& image,
        const casacore::Record& region,
        const casacore::Vector<casacore::Double>& locatePixel,
        casacore::Bool verbose
    );

    // Make a block of regions from a Record
    // public so ImageAnalysis can use it, once those methods have been
    // excised, make private
    static void makeRegionBlock(
        casacore::PtrBlock<const casacore::ImageRegion*>& regions,
        const casacore::Record& Regions
    );

    casacore::Record pixelValue(
        const casacore::Vector<casacore::Int>& pixel
    ) const;

    void pixelValue(
        casacore::Bool& offImage, casacore::Quantum<T>& value,
        casacore::Bool& mask, casacore::Vector<casacore::Int>& pos
    ) const;

    // set specified pixels or mask equal to provided scalar value
    static casacore::Bool set(
        SPIIF image, const casacore::String& pixels,
        const casacore::Int pixelmask, casacore::Record& region,
        const casacore::Bool list = false
    );

    // set region name for logging purposes. Only used if the logfile is set.
    void setRegionName(const casacore::String& rname) { _regionName = rname; }

protected:
    CasacRegionManager::StokesControl _getStokesControl() const {
        return CasacRegionManager::USE_ALL_STOKES;
    }

    std::vector<casacore::Coordinate::Type> _getNecessaryCoordinates() const {
        return std::vector<casacore::Coordinate::Type>();
    }

    casacore::Bool _hasLogfileSupport() const { return true; }

    casacore::Bool _supportsMultipleRegions() const {return true;}

private:
    casacore::IPosition _axes, _lastChunkShape;
    casacore::String _regionName;
    const static casacore::String _className;

    void _checkUnit(
        const casacore::String& unit, const casacore::CoordinateSystem& csys,
        PixelValueManipulatorData::SpectralType specType
    ) const;

    SPIIT _doComposite(
        casacore::uInt axis, const casacore::String& function, Operator op
    ) const;

    SPIIT _doSingle(
        casacore::uInt axis, const casacore::String& function
    ) const;

    casacore::Record _doWorld(
        SPIIT collapsed, const casacore::String& unit,
        PixelValueManipulatorData::SpectralType specType,
        const casacore::Quantity *const restFreq, const casacore::String& frame,
        casacore::uInt axis
    ) const;

    void _doNoncomformantUnit(
        casacore::Vector<casacore::Double>& coords,
        const casacore::CoordinateSystem& csys,
        const casacore::String& unit,
        PixelValueManipulatorData::SpectralType specType,
        const casacore::Quantity *const restFreq,
        const casacore::String& axisUnit
    ) const;

    casacore::Vector<casacore::uInt> _npts(casacore::uInt axis) const;
};

}

#ifndef AIPS_NO_TEMPLATE_SRC
#include <imageanalysis/ImageAnalysis/PixelValueManipulator.tcc>
#endif //# AIPS_NO_TEMPLATE_SRC

#endif
