/*
 * ImageReorderer.cc
 *
 *  Created on: May 7, 2010
 *      Author: dmehring
 */

#include <imageanalysis/ImageAnalysis/ImageTransposer.h>

#include <images/Images/ImageUtilities.h>
#include <images/Images/PagedImage.h>
#include <images/Images/TempImage.h>

#include <imageanalysis/ImageAnalysis/SubImageFactory.h>

using namespace casacore;
namespace casa {

const String ImageTransposer::_class = "ImageTransposer";

ImageTransposer::ImageTransposer(
        const SPCIIF image, const String& order, const String& outputImage
)
    : ImageTask<Float>(
        image, "", 0, "", "", "",
        "", outputImage, false
    ),
    _order(Vector<Int>(0)), _reverse(IPosition(0)) {
    LogOrigin origin(_class, String(__FUNCTION__) + "_1");
    *_getLog() << origin;
    //_construct();
    *_getLog() << origin;
    Regex intRegex("^(-?[0-9])+$");
    if (order.matches(intRegex)) {
        _order = _getOrder(order);
    }
    else {
        *_getLog() << "Incorrect order specification " << order
            << ". All characters must be digits." << LogIO::EXCEPTION;
    }
}

ImageTransposer::ImageTransposer(
        const SPCIIF image, const Vector<String> order,
    const String& outputImage
)
:  ImageTask<Float>(
        image, "", 0, "", "", "",
        "", outputImage, false
    ), _order(Vector<Int>()), _reverse(IPosition(0)) {
    LogOrigin origin(_class, String(__FUNCTION__) + "_2");
    *_getLog() << origin;
    //_construct();
    *_getLog() << origin;
    Vector<String> orderCopy = order;
    std::vector<Bool> rev(orderCopy.size());
    uInt nRev = 0;

    for (uInt i=0; i<orderCopy.size(); i++) {
        if (orderCopy[i].startsWith("-")) {
            orderCopy[i] = orderCopy[i].substr(1);
            rev[i] = true;
            nRev++;
        }
        else {
            rev[i] = false;
        }
    }
    _order = _getImage()->coordinates().getWorldAxesOrder(orderCopy, true);
    uInt n = 0;
    if (nRev > 0) {
        _reverse.resize(nRev);
        for (uInt i=0; i<orderCopy.size(); i++) {
            if (rev[i]) {
                _reverse[n] = _order[i];
                n++;
            }
        }
    }
    *_getLog() << "Old to new axis mapping is " << _order << LogIO::NORMAL;
}

ImageTransposer::ImageTransposer(
        const SPCIIF image, uInt order,
    const String& outputImage
)
:  ImageTask<Float>(
        image, "", 0, "", "", "",
        "", outputImage, false
    ), _order(Vector<Int>()), _reverse(IPosition(0)) {
    LogOrigin origin(_class, String(__FUNCTION__) + "_3");
    *_getLog() << origin;
    //_construct();
    *_getLog() << origin;
    _order = _getOrder(order);
}

SPIIF ImageTransposer::transpose() const {
    *_getLog() << LogOrigin(_class, __FUNCTION__);
    // get the image data
    Array<Float> dataCopy = _getImage()->get();
    CoordinateSystem newCsys = _getImage()->coordinates();
    IPosition shape = _getImage()->shape();
    if (_reverse.size() > 0) {
        Vector<Double> refPix = newCsys.referencePixel();
        Vector<Double> inc = newCsys.increment();
        for (
            IPosition::const_iterator iter=_reverse.begin();
            iter!=_reverse.end(); iter++
        ) {
            refPix[*iter] = shape[*iter] - 1 - refPix[*iter];
            inc[*iter] *= -1;
        }
        newCsys.setReferencePixel(refPix);
        newCsys.setIncrement(inc);
    }
    newCsys.transpose(_order, _order);
    IPosition newShape(_order.size());
    for (uInt i=0; i<newShape.size(); ++i) {
        newShape[i] = shape[_order[i]];
    }
    SPIIF output(
        new TempImage<Float>(TiledShape(newShape), newCsys)
    );
    if (_reverse.size() > 0) {
        dataCopy = reverseArray(dataCopy, _reverse);
    }
    output->put(reorderArray(dataCopy, _order));
    if (_getImage()->hasPixelMask()) {
        std::unique_ptr<Lattice<Bool> > maskLattice(
            _getImage()->pixelMask().clone()
        );
        Array<Bool> maskCopy = maskLattice->get();
        if (_reverse.size() > 0) {
            maskCopy = reverseArray(maskCopy, _reverse);
        }
        dynamic_cast<TempImage<Float> *>(output.get())->attachMask(
            ArrayLattice<Bool>(reorderArray(maskCopy, _order))
        );
    }
    ImageUtilities::copyMiscellaneous(*output, *this->_getImage());
    return this->_prepareOutputImage(*output);
}

ImageTransposer::~ImageTransposer() {}

Vector<Int> ImageTransposer::_getOrder(uInt order) const {
    *_getLog() << LogOrigin(_class, String(__func__));
    uInt naxes = _getImage()->ndim();
    uInt raxes = uInt(log10(order)) + 1;
    if (naxes != raxes) {
        istringstream is;
        is >> order;
        if (! String(is.str()).contains("0")) {
            raxes++;
        }
    }
    if (raxes != naxes) {
        *_getLog() << "Image has " << naxes << " axes but " << raxes
                << " were given for reordering. Number of axes to reorder must match the number of image axes"
                << LogIO::EXCEPTION;
    }
    if (raxes > 10) {
        *_getLog() << "Only images with less than 10 axes can currently be reordered. This image has "
                << naxes << " axes" << LogIO::EXCEPTION;
    }
    Vector<Int> myorder(naxes);
    uInt mag = 1;
    for (uInt i=1; i<myorder.size(); i++) {
        mag *= 10;
    }
    uInt scratchOrder = order;
    for (uInt i=0; i<myorder.size(); i++) {
        uInt index = scratchOrder/mag;
        if (index >= naxes) {
            *_getLog() << "Image does not contain zero-based axis number " << index
                    << " but this was incorrectly specified in order parameter. "
                    << order << " All digits in the order parameter must be greater "
                    << "than or equal to zero and less than the number of image axes."
                    << LogIO::EXCEPTION;
        }
        for (uInt j=0; j<i; j++) {
            if ((Int)index == myorder[j]) {
                *_getLog() << "Axis number " << index
                        << " specified multiple times in order parameter "
                        << order << " . It can only be specified once."
                        << LogIO::EXCEPTION;
            }
        }
        myorder[i] = index;
        scratchOrder -= index*mag;
        mag /= 10;
    }
    return myorder;
}

Vector<Int> ImageTransposer::_getOrder(const String& order) {
    String orderCopy = order;
    if (orderCopy.contains('-', 0)) {
        uInt maxn = orderCopy.freq('-') + 1;
        String *parts = new String[maxn];
        split(order, parts, maxn, '-');
        // disregard the first element because that won't have a -
        _reverse.resize(maxn - 1);
        orderCopy = parts[0];
        for (uInt i=1; i<maxn; i++) {
            _reverse[i-1] = String::toInt(parts[i].substr(0, 1));
            orderCopy += parts[i];
        }
        delete [] parts;

    }
    return _getOrder(String::toInt(orderCopy));
}

}
