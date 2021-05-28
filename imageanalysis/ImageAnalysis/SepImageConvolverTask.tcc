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
#include <imageanalysis/ImageAnalysis/SepImageConvolverTask.h>

namespace casa {

template <class T> const casacore::String SepImageConvolverTask<T>::CLASS_NAME = "SepImageConvolverTask";

template <class T> SepImageConvolverTask<T>::SepImageConvolverTask(
    const SPCIIT image, const casacore::Record *const &region,
    const casacore::String& mask, const casacore::String& outname, const casacore::Bool overwrite
) : ImageTask<T>(image, "", region, "", "", "", mask, outname, overwrite),
    _kernels(), _scale(0) {
    this->_construct(true);
}

template <class T> SPIIT SepImageConvolverTask<T>::convolve() {
    ThrowIf(
        _axes.empty(),
        "You have not specified any axes to convolve"
    );
    ThrowIf(
        _axes.size() != _kernels.size()
        || _axes.size() != _kernelWidths.size(),
        "You must give the same number of axes, kernels and widths"
    );
    *this->_getLog() << casacore::LogOrigin(getClass(), __func__);
    auto autoScale = _scale < 0;
    auto myscale = autoScale ? 1 : _scale;
    auto subImage = SubImageFactory<T>::createSubImageRO(
        *this->_getImage(), *this->_getRegion(), this->_getMask(),
        this->_getLog().get(), casacore::AxesSpecifier(), this->_getStretch()
    );
    // logging requested in CAS-8486
    *this->_getLog() << LogIO::NORMAL << "Original image shape: "
        << this->_getImage()->shape() << LogIO::POST;  
    *this->_getLog() << LogIO::NORMAL << "Kernel types: "
        << _kernels << LogIO::POST;  
    *this->_getLog() << LogIO::NORMAL << "Kernel widths: "
        << _kernelWidths << LogIO::POST;  
    SepImageConvolver<T> sic(*subImage, *this->_getLog(), true);
    Bool useImageShapeExactly = false;
    const auto naxes = _axes.size();
    for (uInt i=0; i<naxes; ++i) {
        VectorKernel::KernelTypes type = VectorKernel::toKernelType(_kernels[i]);
        sic.setKernel(
            uInt(_axes[i]), type, _kernelWidths[i], autoScale,
            useImageShapeExactly, myscale
        );
        *this->_getLog() << casacore::LogIO::NORMAL << "Axis " << _axes[i]
            << " : kernel shape = "
            << sic.getKernelShape(uInt(_axes[i])) << casacore::LogIO::POST;
    }
    auto output = this->_prepareOutputImage(*subImage);
    sic.convolve(*output);
    return output;
}

}

