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

#include <imageanalysis/ImageAnalysis/ImageConcatenator.h>

#include <casacore/casa/BasicSL/STLIO.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/images/Images/ImageConcat.h>
#include <imageanalysis/ImageAnalysis/ImageFactory.h>

namespace casa {

template <class T>
const casacore::String ImageConcatenator<T>::_class = "ImageConcatenator";

template <class T>
ImageConcatenator<T>::ImageConcatenator(
    std::vector<casacore::String>& imageNames, const casacore::String& outname,
	casacore::Bool overwrite
) : _imageNames(imageNames), _outname(outname), _overwrite(overwrite) {
    ThrowIf(
        _imageNames.size() < 2,
        "You must give at least two extant images to concatenate"
    );
    if (! _outname.empty()) {
        casacore::File p(_outname);
        ThrowIf(
            p.exists() && ! _overwrite,
            _outname + " exists and overwrite is false"
        );
    }
    LogIO log;
    log << casacore::LogOrigin(_class, __func__, WHERE);
    log << casacore::LogIO::NORMAL << "Number of images to concatenate = "
        << (_imageNames.size()) << casacore::LogIO::POST;
}

template <class T>
ImageConcatenator<T>::~ImageConcatenator() {}

template <class T>
void ImageConcatenator<T>::setAxis(int axis) {
    uInt count = 0;
    Int oAxis = 0;
    for (const auto name: _imageNames) {
        auto image = ImageFactory::fromFile(name, T(0), false);
        if (axis >= 0) {
            ThrowIf(
                axis >= (casacore::Int)image->ndim(),
                "Specified zero-based value of axis exceeds "
                "number of dimensions in " + image->name()
            );
            _axis = axis;
        }
        else {
            const auto& csys = image->coordinates();
            ThrowIf(
                ! csys.hasSpectralAxis(),
                "Image " + name + " has no spectral axis"
            );
            _axis = csys.spectralAxisNumber(false);
            if (count == 0) {
                oAxis = _axis;
            }
            else {
                ThrowIf(
                    _axis != oAxis,
                    "Spectral axis numbers between images are not the same"
                );
            }
        }
        ++count;
    }
}

template <class T>
SPIIT ImageConcatenator<T>::concatenate() {
    ThrowIf(
        _outname.empty() && _mode != PAGED,
        "An empty outname can be used only if mode == PAGED"
    );
    if (_mode != PAGED) {
        casacore::Path t(_outname);
        for (const auto& name: _imageNames) {
            casacore::Path p(name);
            ThrowIf(
                p.absoluteName() == t.absoluteName(),
                "Cannot have one of the input images also be "
                "the output image is mode is not PAGED"
            );
        }
    }
    auto myImage = ImageFactory::fromFile(_imageNames[0], T(0), false);
    auto ndim = myImage->ndim();
    const auto& csys = myImage->coordinates();
    casacore::Int whichCoordinate, axisInCoordinate;
    csys.findPixelAxis(whichCoordinate, axisInCoordinate, _axis);
    const auto ctype = csys.coordinate(whichCoordinate).type();
    const auto pix = csys.referencePixel();
   	auto isIncreasing = false;
    casacore::Vector<casacore::Double> minVals;
    casacore::uInt n = 0;
    if (! _relax || _reorder) {
		n = _imageNames.size();
		minVals.resize(n);
        isIncreasing = _minAxisValues(
            minVals[0], csys, myImage->shape()
        );
    }
    auto dataType = myImage->dataType();
    for (uInt i = 1; i < n; ++i) { 
        auto myIm = ImageFactory::fromFile(_imageNames[i], T(0), false);
        ThrowIf(
            myIm->ndim() != ndim,
            "Images do not have the same number of dimensions"
        );
        auto oDType = myIm->dataType();
   		ThrowIf(
			oDType != dataType,
			"Concatenation of images of different data types is not supported"
		);
        if (! _relax || _reorder) {
            auto shape = myIm->shape();
            const auto& oCsys = myIm->coordinates();
            ThrowIf(
                _minAxisValues(minVals[i], oCsys, shape) != isIncreasing,
				"Coordinate axes in different images with opposing increment "
				"signs is not permitted if relax=false or reorder=true"
			);
        }
		if (! _relax) {
            const auto& oCsys = myIm->coordinates();
			oCsys.findPixelAxis(
				whichCoordinate, axisInCoordinate, _axis
			);
			ThrowIf(
				oCsys.coordinate(whichCoordinate).type() != ctype,
				"Cannot concatenate different coordinates in different images "
				"if relax=false"
			);
		}
    }
	if (_reorder) {
		casacore::Sort sorter;
		sorter.sortKey(
			minVals.data(), TpDouble, 0,
			isIncreasing ? casacore::Sort::Ascending
			    : casacore::Sort::Descending
		);
		casacore::Vector<casacore::uInt> indices;
		sorter.sort(indices, n);
        auto tmp = _imageNames;
		auto iter = tmp.begin();
		auto end = tmp.end();
		auto index = indices.begin();
		while (iter != end) {
			*iter++ = _imageNames[*index++];
		}
		_imageNames = tmp;
        LogIO log;
		log << LogOrigin("ImageConcatenator", __func__)
            << casacore::LogIO::NORMAL
			<< "Images will be concatenated in the order "
			<< _imageNames << " and the coordinate system of "
			<< _imageNames[0] << " will be used as the reference"
			<< casacore::LogIO::POST;
	}
    std::shared_ptr<casacore::ImageConcat<T>> pConcat(
	    new casacore::ImageConcat<T>(_axis, _tempClose)
    );
	ThrowIf(
		! pConcat.get(), "Failed to create ImageConcat object"
	);
    if (_mode == COPYVIRTUAL || _mode == MOVEVIRTUAL) {
        casacore::File p(_outname);
        casacore::Directory eldir(p);
        eldir.create(_overwrite);
        auto rootdir = eldir.path().absoluteName();
        casacore::uInt k = 0;
        auto copyNames = _imageNames;
        // FIXME probably needs to be imageList since imageList can get screwed around above
        for (auto imname: _imageNames) {
            casacore::Directory elim(imname);
            copyNames[k] = rootdir + "/" + elim.path().baseName();
            if (_mode == MOVEVIRTUAL) {
                elim.move(copyNames[k]);
            }
            else if (_mode == COPYVIRTUAL) {
                elim.copy(copyNames[k]);
            }
            else {
                ThrowCc("Logic Error");
            }
            ++k;
        }
        _imageNames = copyNames;
    }
	if (_axis < 0) {
        setAxis(-1);
	}
    auto first = true;
	for(const auto& name: _imageNames) {
		_addImage(pConcat, name, first);
        first = false;
	}
    if (_mode == PAGED) {
        // return this->_prepareOutputImage(*pConcat);
        static const casacore::Record empty;
        static const casacore::String emptyString;
        return SubImageFactory<T>::createImage(
            *pConcat, _outname, empty, emptyString, false,
            _overwrite, true, false, false
        );  
    }
    pConcat->save(_outname);
    return ImageFactory::fromFile(_outname, T(0), false);
}

template <class T> void ImageConcatenator<T>::setMode(const casacore::String& mymode) {
    auto m = mymode;
    m.downcase();
    if (m.startsWith("m")) {
        _mode = MOVEVIRTUAL;
    }
    else if (m.startsWith("c")) {
        _mode = COPYVIRTUAL;
    }
    else if (m.startsWith("n")) {
        _mode = NOMOVEVIRTUAL;
    }
    else if (m.startsWith("p")) {
        _mode = PAGED;
    }
    else {
        ThrowCc("Unsupported mode " + mymode);
    }
}

template <class T> void ImageConcatenator<T>::_addImage(
	std::shared_ptr<casacore::ImageConcat<T>> pConcat, const casacore::String& name,
    casacore::Bool first
) const {
    if (first) {
        pConcat->setImage(*ImageFactory::fromFile(name, T(0), false), _relax);
		return;
	}
	auto doneOpen = false;
	try {
		SPIIT im2 = casacore::ImageUtilities::openImage<T>(name);
        doneOpen = true;
		pConcat->setImage(*im2, _relax);
	}
	catch (const casacore::AipsError& x) {
		ThrowIf(doneOpen, x.getMesg());
		ThrowCc(
			"Failed to open file " + name
			+ "This may mean you have too many files open simultaneously. "
			"Try using tempclose=T in the imageconcat constructor. "
			"Exception message " + x.getMesg()
		);
	}
}

template <class T> casacore::Bool ImageConcatenator<T>::_minAxisValues(
    casacore::Double& mymin, const casacore::CoordinateSystem& csys,
    const casacore::IPosition& shape
) const {
    auto pix = csys.referencePixel();
    auto isIncreasing = csys.increment()[_axis] > 0;
    pix[_axis] = isIncreasing ? 0 : shape[_axis] - 1;
    mymin = csys.toWorld(pix)[_axis];
    if (isIncreasing) {
        return true;
    }
    else if (shape[_axis] == 1) {
        // _axis is degenerate
        return isIncreasing;
    }
    else {
        // axis is decreasing, so the max pixel coordinate
        // has the minimum world value
        return false;
    }
}

template <class T>
String ImageConcatenator<T>::getClass() const {
	return _class;
}

}


