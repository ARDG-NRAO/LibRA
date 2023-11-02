//# Image2DConvolver.cc:  convolution of an image by given Array
//# Copyright (C) 1995,1996,1997,1998,1999,2000,2001,2002
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//   
#include <imageanalysis/ImageAnalysis/ImageConvolverTask.h>

namespace casa {

template <class T> const casacore::String ImageConvolverTask<T>::CLASS_NAME = "ImageConvolverTask";

template <class T> ImageConvolverTask<T>::ImageConvolverTask(
    const SPCIIT image, const casacore::Record *const &region,
    const casacore::String& mask, const casacore::String& outname, const casacore::Bool overwrite
) : ImageTask<T>(image, "", region, "", "", "", mask, outname, overwrite),
    _kernel(), _scale(0) {
    this->_construct(true);
}


template <class T> SPIIT ImageConvolverTask<T>::convolve() {
    auto autoScale = _scale <= 0;
    auto subImage = SubImageFactory<T>::createSubImageRO(
        *this->_getImage(), *this->_getRegion(), this->_getMask(),
        this->_getLog().get(), casacore::AxesSpecifier(), this->_getStretch()
    );
    casacore::TempImage<T> x(subImage->shape(), subImage->coordinates());
    // Make the convolver
    casacore::Bool copyMisc = true;
    auto scaleType = autoScale
        ? ImageConvolver<T>::AUTOSCALE : ImageConvolver<T>::SCALE;
    ImageConvolver<T> aic;
    aic.convolve(
        *this->_getLog(), x, *subImage, _kernel, scaleType,
        _scale, copyMisc
    );
    if (_kernel.ndim() < subImage->ndim()) {
        String msg = "NOTE: kernel with fewer dimensions than the input image applied. Degenerate axes added to the kernel";
        this->addHistory(LogOrigin("ImageConvolverTask", __func__), msg);
    }
    return this->_prepareOutputImage(x);
}

template <class T> void ImageConvolverTask<T>::setKernel(const casacore::Array<T>& kernel) {
    ThrowIf(
        kernel.empty(), "Kernel array cannot be empty"
    );
    _kernel.assign(kernel);
}

}

