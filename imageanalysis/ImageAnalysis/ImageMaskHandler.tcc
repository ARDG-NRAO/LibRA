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

#include <imageanalysis/ImageAnalysis/ImageMaskHandler.h>

#include <imageanalysis/ImageAnalysis/ImageFactory.h>

#include <casa/Exceptions/Error.h>
#include <images/Images/ImageExprParse.h>
#include <images/Images/ImageProxy.h>
#include <images/Regions/RegionHandler.h>

namespace casa {

template <class T> ImageMaskHandler<T>::ImageMaskHandler(SPIIT image)
	: _image(image) {}

template <class T> ImageMaskHandler<T>::~ImageMaskHandler() {}

template <class T> void ImageMaskHandler<T>::set(const casacore::String& name) {
	_image->setDefaultMask(name);
}

template <class T> casacore::String ImageMaskHandler<T>::defaultMask() const {
	return _image->getDefaultMask();
}

template <class T> void ImageMaskHandler<T>::deleteMasks(
	const std::set<casacore::String>& masks
) {
	ThrowIf(masks.empty(), "You have not supplied any mask names");
	for (const auto& mask: masks) {
		_image->removeRegion(mask, casacore::RegionHandler::Masks, false);
	}
}

template <class T> void ImageMaskHandler<T>::rename(
	const casacore::String& oldName, const casacore::String& newName
) {
	_image->renameRegion(newName, oldName, casacore::RegionHandler::Masks, false);
}

template <class T> casacore::Vector<casacore::String> ImageMaskHandler<T>::get() const {
	return _image->regionNames(casacore::RegionHandler::Masks);
}

template <class T> void ImageMaskHandler<T>::copy(
	const casacore::String& currentName, const casacore::String& newName
) {
	ThrowIf(_image->hasRegion(
		newName, casacore::RegionHandler::Any),
		"Mask " + newName + " already exists"
	);
	casacore::Vector<casacore::String> mask2 = stringToVector(currentName, ':');
	ThrowIf(mask2.size() > 2, "Illegal mask specification " + currentName);
	auto external = mask2.size() == 2;
	_image->makeMask(newName, true, false);

	if (external) {
		casacore::ImageProxy proxy(casacore::Vector<casacore::String>(1, mask2[0]), 0);
		ThrowIf(
			! proxy.shape().isEqual(_image->shape()),
			"Images have different shapes"
		);
		auto imagePtrs = ImageFactory::fromFile(mask2[0]);
		if (auto myImage = std::get<0>(imagePtrs)) {
			casacore::ImageUtilities::copyMask(
				*_image, *myImage,
				newName, mask2[1], casacore::AxesSpecifier()
			);
		}
		else if (auto myimage = std::get<1>(imagePtrs)) {
			casacore::ImageUtilities::copyMask(
				*_image, *myimage,
				newName, mask2[1], casacore::AxesSpecifier()
			);
		}
        else {
            ThrowCc("This image pixel data type not supported");
        }
	}
	else {
		casacore::ImageUtilities::copyMask(
			*_image, *_image,
			newName, mask2[0], casacore::AxesSpecifier()
		);
	}
}

template <class T> template<class U> void ImageMaskHandler<T>::copy(
    const casacore::MaskedLattice<U>& mask
) {
    auto shape = _image->shape();
    ThrowIf (
        ! shape.isEqual(mask.shape()),
        "Mask must be the same shape as the image"
    );
    auto cursorShape = _image->niceCursorShape(4096*4096);
    casacore::LatticeStepper stepper(shape, cursorShape, casacore::LatticeStepper::RESIZE);
    if (! _image->hasPixelMask()) {
        if (ImageMask::isAllMaskTrue(mask)) {
            // the current image has no pixel mask and the mask is all true, so
            // there is no point in copying anything.
            return;
        }
        casacore::String maskname = "";
        casacore::LogIO log;
        ImageMaskAttacher::makeMask(*_image, maskname, false, true, log, false);
    }
    casacore::Lattice<casacore::Bool>& pixelMask = _image->pixelMask();
    casacore::LatticeIterator<casacore::Bool> iter(pixelMask, stepper);
    casacore::RO_MaskedLatticeIterator<U> miter(mask, stepper);
    for (iter.reset(); ! iter.atEnd(); ++iter, ++miter) {
        auto mymask = miter.getMask();
        iter.rwCursor() = mymask;
    }
}

template <class T> void ImageMaskHandler<T>::calcmask(
	const casacore::String& mask, casacore::Record& regions,
	const casacore::String& maskName, const casacore::Bool makeDefault
) {
	ThrowIf(mask.empty(), "You must specify an expression");
	ThrowIf (
		! _image->canDefineRegion(),
		"Cannot make requested mask for this image type which is " + _image->imageType()
	);
	casacore::Block<casacore::LatticeExprNode> temps;
	casacore::PtrBlock<const casacore::ImageRegion*> tempRegs;
	_makeRegionBlock(tempRegs, regions);
	casacore::LatticeExprNode node = casacore::ImageExprParse::command(mask, temps, tempRegs);

	// Delete the ImageRegions
	_makeRegionBlock(tempRegs, casacore::Record());

	// Make sure the expression is Boolean
	DataType type = node.dataType();
	ThrowIf(type != TpBool, "The expression type must be Boolean");
	_calcmask(node, maskName, makeDefault);
}

template<class T> void ImageMaskHandler<T>::_calcmask(
    const casacore::LatticeExprNode& node,
    const casacore::String& maskName, const casacore::Bool makeDefault
) {
	// Get the shape of the expression and check it matches that
	// of the output image.  We don't check that the Coordinates
	// match as that would be an un-necessary restriction.
	if (
		! node.isScalar()
		&& ! _image->shape().isEqual(node.shape())
	) {
		ostringstream os;
		os << "The shape of the expression does not conform "
			<< "with the shape of the output image"
			<< "Expression shape = " << node.shape()
			<< "Image shape      = " << _image->shape();
		ThrowCc(os.str());
	}
	// Make mask and get hold of its name.   Currently new mask is forced to
	// be default because of other problems.  Cannot use the usual ImageMaskAttacher<casacore::Float>::makeMask
	// function because I cant attach/make it default until the expression
	// has been evaluated
	// Generate mask name if not given
	casacore::String maskName2 = maskName.empty()
		? _image->makeUniqueRegionName(
			casacore::String("mask"), 0
		) : maskName;

	// Make the mask if it does not exist
	if (! _image->hasRegion(maskName2, casacore::RegionHandler::Masks)) {
		_image->makeMask(maskName2, true, false);
		casacore::LogIO log;
		log << casacore::LogOrigin("ImageMaskHandler", __func__);
		log << casacore::LogIO::NORMAL << "Created mask `" << maskName2 << "'"
			<< casacore::LogIO::POST;
		casacore::ImageRegion iR = _image->getRegion(
			maskName2, casacore::RegionHandler::Masks
		);
		casacore::LCRegion& mask = iR.asMask();
		if (node.isScalar()) {
			casacore::Bool value = node.getBool();
			mask.set(value);
		}
		else {
			mask.copyData(casacore::LatticeExpr<casacore::Bool> (node));
		}
	}
	else {
		// Access pre-existing mask.
		casacore::ImageRegion iR = _image->getRegion(
			maskName2, casacore::RegionHandler::Masks
		);
		casacore::LCRegion& mask2 = iR.asMask();
		if (node.isScalar()) {
			casacore::Bool value = node.getBool();
			mask2.set(value);
		}
		else {
			mask2.copyData(casacore::LatticeExpr<casacore::Bool> (node));
		}
	}
	if (makeDefault) {
		_image->setDefaultMask(maskName2);
	}
}

template<class T> void ImageMaskHandler<T>::_makeRegionBlock(
    casacore::PtrBlock<const casacore::ImageRegion*>& regions,
    const casacore::Record& Regions
) {
    auto n = regions.size();
    for (casacore::uInt j=0; j<n; ++j) {
        delete regions[j];
    }
    regions.resize(0, true, true);
    casacore::uInt nreg = Regions.nfields();
    if (nreg > 0) {
        regions.resize(nreg);
        regions.set(static_cast<casacore::ImageRegion*> (0));
        for (casacore::uInt i=0; i<nreg; ++i) {
            regions[i] = casacore::ImageRegion::fromRecord(Regions.asRecord(i), "");
        }
    }
}

}
