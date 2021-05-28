#ifndef IMAGES_IMAGEEDECIMATOR_H
#define IMAGES_IMAGEEDECIMATOR_H

#include <imageanalysis/ImageAnalysis/ImageTask.h>

#include <imageanalysis/ImageAnalysis/ImageDecimatorData.h>

#include <casa/namespace.h>

namespace casa {

template <class T> class ImageDecimator : public ImageTask<T> {
    // <summary>
    // Top level interface for removing image planes.
    // </summary>

    // <reviewed reviewer="" date="" tests="" demos="">
    // </reviewed>

    // <prerequisite>
    // </prerequisite>

    // <etymology>
    // Decimates planes of image
    // </etymology>

    // <synopsis>
    // Top level interface for removing image planes.
    // </synopsis>

public:

    ImageDecimator(
        const SPCIIT image,
        const casacore::Record *const region,
        const casacore::String& maskInp,
        const casacore::String& outname, casacore::Bool overwrite
    );

    // destructor
    ~ImageDecimator() {}

    SPIIT decimate() const;

    casacore::String getClass() const { const static casacore::String s = "ImageDecimator"; return s; }

    // every nth plane will be kept
    void setFactor(casacore::uInt n);

    // Set the pixel axis number along which the decimation will occur
    void setAxis(casacore::uInt n);

    // set the decimation function
    void setFunction(ImageDecimatorData::Function f);

protected:
    inline  CasacRegionManager::StokesControl _getStokesControl() const {
        return CasacRegionManager::USE_ALL_STOKES;
    }

    inline std::vector<casacore::Coordinate::Type> _getNecessaryCoordinates() const {
        return std::vector<casacore::Coordinate::Type>();
    }

    inline casacore::Bool _supportsMultipleBeams() const {return false;}

    inline casacore::Bool _supportsMultipleRegions() const {return true;}

private:
    casacore::uInt _axis, _factor;
    ImageDecimatorData::Function _function;

    // disallow default constructor
    ImageDecimator();
};
}

#ifndef AIPS_NO_TEMPLATE_SRC
#include <imageanalysis/ImageAnalysis/ImageDecimator.tcc>
#endif

#endif
