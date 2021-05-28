#ifndef IMAGEANALYSIS_IMAGEMASKEDPIXELREPLACER_H
#define IMAGEANALYSIS_IMAGEMASKEDPIXELREPLACER_H

#include <imageanalysis/ImageAnalysis/ImageTask.h>
#include <casa/namespace.h>

namespace casa {

template <class T>  class ImageMaskedPixelReplacer : public ImageTask<T> {
    // <summary>
    // Top level interface for replacing masked pixel values of an image
    // </summary>

    // <reviewed reviewer="" date="" tests="" demos="">
    // </reviewed>

    // <prerequisite>
    // </prerequisite>

    // <etymology>
    // Replace masked pixel values of an image
    // </etymology>

    // <synopsis>
    // Top level interface for replacing masked pixel values of an image
    // </synopsis>

public:

    ImageMaskedPixelReplacer() = delete;

    // The total pixel mask is calculated by a logical OR of the existing default
    // image pixel mask when the mask specified by <src>mask</src>. Pixel replacement
    // is done in place; ie on the image passed to the constructor. No copy of that image
    // is made.
    ImageMaskedPixelReplacer(
        const SPIIT image,
        const casacore::Record *const &region = nullptr,
        const casacore::String& maskInp = ""
    );

    ~ImageMaskedPixelReplacer() {}

    // Perform the substitution. expr is the LEL expression to use for the new pixel values.
    // If updateMask is true, the false mask values will be changed to true.
    void replace(const casacore::String& expr, casacore::Bool updateMask, casacore::Bool verbose);

    casacore::String getClass() const;

protected:
    inline  CasacRegionManager::StokesControl _getStokesControl() const {
        return CasacRegionManager::USE_ALL_STOKES;
    }

    inline std::vector<casacore::Coordinate::Type> _getNecessaryCoordinates() const {
        return std::vector<casacore::Coordinate::Type>(0);
     }

private:
    static const casacore::String _class;

    // This class holds a writable image object to write pixel value
    // changes to.
    const SPIIT _image;

    static void _makeRegionBlock(
        casacore::PtrBlock<const casacore::ImageRegion*>& imageRegions,
        const casacore::Record& regions
    );
};
}

#ifndef AIPS_NO_TEMPLATE_SRC
#include <imageanalysis/ImageAnalysis/ImageMaskedPixelReplacer.tcc>
#endif

#endif
