//# tSubImage.cc: Test program for class SubImage
//# Copyright (C) 1998,1999,2000,2001,2003
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or (at your option)
//# any later version.
//#
//# This program is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//# more details.
//#
//# You should have received a copy of the GNU General Public License along
//# with this program; if not, write to the Free Software Foundation, Inc.,
//# 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id: $

#include <imageanalysis/ImageAnalysis/ImagePadder.h>

#include <imageanalysis/ImageAnalysis/SubImageFactory.h>
#include <images/Images/TempImage.h>
#include <images/Images/ImageUtilities.h>

using namespace casacore;
namespace casa {

const String ImagePadder::_class = "ImagePadder";

ImagePadder::ImagePadder(
        const SPCIIF image,
    const Record *const regionRec,
    const String& box,
    const String& chanInp, const String& stokes,
    const String& maskInp, const String& outname,
    const Bool overwrite
) : ImageTask<Float>(
        image, "", regionRec, box, chanInp, stokes,
        maskInp, outname, overwrite
    ), _nPixels(0), _value(0), _good(False) {
    _construct();
}

ImagePadder::~ImagePadder() {}

void ImagePadder::setPaddingPixels(
    const uInt nPixels, const Float value,
    const Bool good
) {
    _nPixels = nPixels;
    _value = value;
    _good = good;
}

SPIIF ImagePadder::pad(const Bool wantReturn) const {
    *_getLog() << LogOrigin(_class, __FUNCTION__, WHERE);
    std::shared_ptr<const SubImage<Float> > subImage = SubImageFactory<Float>::createSubImageRO(
        *this->_getImage(), *_getRegion(), _getMask(),
        _getLog().get(), AxesSpecifier(), _getStretch()
    );
    Vector<Int> dirAxes = subImage->coordinates().directionAxesNumbers();
    IPosition outShape = subImage->shape();
    outShape[dirAxes[0]] += 2*_nPixels;
    outShape[dirAxes[1]] += 2*_nPixels;
    CoordinateSystem newCoords = subImage->coordinates();
    Vector<Double> refpix = newCoords.referencePixel();
    refpix[dirAxes[0]] += _nPixels;
    refpix[dirAxes[1]] += _nPixels;
    newCoords.setReferencePixel(refpix);
    ArrayLattice<Bool> mask(outShape);
    mask.set(_good);
    IPosition blc(subImage->ndim(), 0);
    blc[dirAxes[0]] = _nPixels;
    blc[dirAxes[1]] = _nPixels;
    if (subImage->isMasked() || ! _good) {
        IPosition end = subImage->shape();
        Slicer slice(blc, end, Slicer::endIsLength);
        SubLattice<Bool> subLatt(mask, slice, True);
        ImageTask<Float>::_copyMask(subLatt, *subImage);
    }
    Array<Float> valArray(outShape, _value);
    ArrayLattice<Float> values(valArray);
    values.putSlice(subImage->get(), blc);
    const Array<Float>& vals = values.get();
    this->_reportOldNewImageShapes(outShape);
    SPIIF outImage = _prepareOutputImage(
        *subImage, &vals, &mask, &outShape, &newCoords
    );
    if (! wantReturn) {
        outImage.reset();
    }
    return outImage;
}

String ImagePadder::getClass() const {
    return _class;
}

}


