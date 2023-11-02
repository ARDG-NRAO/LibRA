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

#include <imageanalysis/ImageAnalysis/ImageCropper.h>

#include <imageanalysis/ImageAnalysis/SubImageFactory.h>

namespace casa {

template <class T>
const casacore::String ImageCropper<T>::_class = "ImageCropper";

template <class T>
ImageCropper<T>::ImageCropper(
	const std::shared_ptr<const casacore::ImageInterface<T> > image,
	const casacore::Record *const &regionRec, const casacore::String& box,
	const casacore::String& chanInp, const casacore::String& stokes,
	const casacore::String& maskInp, const casacore::String& outname,
	const casacore::Bool overwrite
) : ImageTask<T>(
		image, "", regionRec, box, chanInp, stokes,
		maskInp, outname, overwrite
	), _axes(std::set<casacore::uInt>()) {
	this->_construct();
}


template <class T>
ImageCropper<T>::~ImageCropper() {}

template <class T>
void ImageCropper<T>::setAxes(const std::set<casacore::uInt>& axes) {
	casacore::uInt ndim = this->_getImage()->ndim();

	if (axes.size() == 0) {
		for (casacore::uInt i=0; i< ndim; i++) {
			_axes.insert(i);
		}
	}
	else {
		std::set<casacore::uInt>::const_iterator end = axes.end();
		for (
			std::set<casacore::uInt>::const_iterator i=axes.begin();
			i!=end; i++
		) {
			if (*i >= ndim) {
				casacore::LogOrigin(_class, __FUNCTION__);
				*this->_getLog() << "Illegal axis number " << *i
					<< ". Image has only " << ndim << " dimensions"
					<< casacore::LogIO::EXCEPTION;
			}
		}
		_axes = axes;
	}
}

template <class T>
std::shared_ptr<casacore::ImageInterface<T> > ImageCropper<T>::crop(
	const casacore::Bool wantReturn
) const {
	*this->_getLog() << casacore::LogOrigin(_class, __FUNCTION__, WHERE);

	std::shared_ptr<const casacore::SubImage<T> > subImage = SubImageFactory<T>::createSubImageRO(
		*this->_getImage(), *this->_getRegion(), this->_getMask(),
		this->_getLog().get(), casacore::AxesSpecifier(), this->_getStretch(), true
	);
	*this->_getLog() << casacore::LogOrigin(_class, __FUNCTION__, WHERE);
	casacore::Array<casacore::Bool> mask = subImage->getMask();
	if (! anyTrue(mask)) {
		*this->_getLog() << "(Sub)image is completely masked." << casacore::LogIO::EXCEPTION;
	}
	std::set<casacore::uInt>::const_iterator end = _axes.end();
	casacore::IPosition shape = mask.shape();
	casacore::IPosition blc = casacore::IPosition(shape.size(), 0);
	casacore::IPosition trc = shape - 1;

	for (
		std::set<casacore::uInt>::const_iterator i=_axes.begin();
		i!=end; i++
	) {
		casacore::Bool minFound = false;
		casacore::Bool maxFound = false;
		casacore::IPosition testblc = casacore::IPosition(shape.size(), 0);
		casacore::IPosition testtrc = shape - 1;
		casacore::uInt end1 = (casacore::uInt)(shape[*i]/2 + 1);
        casacore::IPosition step = testtrc - testblc + 1;
        step[*i] = 1;
		for (casacore::uInt j=0; j<end1; j++) {
			testblc[*i] = j;
			testtrc[*i] = j;
			casacore::Slicer slicer(testblc, testtrc, casacore::Slicer::endIsLast);
            if (! minFound && anyTrue(mask(slicer))) {
				blc[*i] = j;
				minFound = true;
			}
			testblc[*i] = shape[*i] - j - 1;
			testtrc[*i] = testblc[*i];
            slicer = casacore::Slicer(testblc, testtrc, casacore::Slicer::endIsLast);
			if (! maxFound && anyTrue(mask(slicer))) {
				trc[*i] = testblc[*i];
				maxFound = true;
			}
			if (maxFound && minFound) {
				break;
			}
		}
		AlwaysAssert(minFound && maxFound, casacore::AipsError);
	}
	casacore::LCBox lcbox(blc, trc, shape);
	std::shared_ptr<const casacore::SubImage<T> > cropped = SubImageFactory<T>::createSubImageRO(
		*subImage, lcbox.toRecord(""), "",
		this->_getLog().get(), casacore::AxesSpecifier(), false, true
	);
    this->_reportOldNewImageShapes(*cropped);
	std::shared_ptr<casacore::ImageInterface<T> > outImage( this->_prepareOutputImage(*cropped));
	if (! wantReturn) {
		outImage.reset();
	}
	return outImage;
}

template <class T>
String ImageCropper<T>::getClass() const {
	return _class;
}

}

