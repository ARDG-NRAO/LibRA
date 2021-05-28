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

#include <imageanalysis/ImageAnalysis/ImageFactory.h>

#include <casa/OS/Directory.h>
#include <casa/OS/RegularFile.h>
#include <casa/OS/SymLink.h>
#include <coordinates/Coordinates/CoordinateUtil.h>
#include <images/Images/PagedImage.h>
#include <images/Images/TempImage.h>
#include <tables/LogTables/NewFile.h>

#include <imageanalysis/ImageAnalysis/ImageHistory.h>
#include <imageanalysis/ImageAnalysis/SubImageFactory.h>

namespace casa {

template <class T> void ImageFactory::remove(SPIIT& image, casacore::Bool verbose) {
	ThrowIf(
		! image, "Image cannot be null"
	);
	ThrowIf(
		! image->isPersistent(),
		"This image tool is not associated with a "
		"persistent disk file. It cannot be deleted"
	);
	auto fileName = image->name(false);
	ThrowIf(
		fileName.empty(),
		"Filename is empty or does not exist."
    );
	casacore::File f(fileName);
	ThrowIf(
		! f.exists(),
		fileName + " does not exist."
	);

	// Destroy object before deleting image. This is why a reference
	// needs to be passed in.
	image.reset();

	// Now try and blow it away.  If it's open, tabledelete won't delete it.
	casacore::String message;
	casacore::LogIO log;
	if (casacore::Table::canDeleteTable(message, fileName, true)) {
		try {
			casacore::Table::deleteTable(fileName, true);
			log << (verbose ? casacore::LogIO::NORMAL : casacore::LogIO::DEBUG1)
            	<< "deleted table " << fileName << casacore::LogIO::POST;
		}
		catch (const casacore::AipsError& x) {
			ThrowCc(
				"Failed to delete file " + fileName
				+ " because " + x.getMesg()
			);
		};
	}
	else {
		ThrowCc(
			"Cannot delete file " + fileName
            + " because " + message
		);
	}
}

template<class T>
std::shared_ptr<TempImage<std::complex<T>>> ImageFactory::makeComplexImage(
    SPCIIT realPart, SPCIIT imagPart
) {
    auto shape = realPart->shape();
    ThrowIf(
        shape != imagPart->shape(),
        "Real and imaginary parts have different shapes"
    );
    std::shared_ptr<TempImage<std::complex<T>>> newImage(
        new TempImage<std::complex<T>>(shape, realPart->coordinates())
    );
    LatticeExpr<std::complex<T>> expr(
        casacore::formComplex(*realPart, *imagPart)
    );
    if (ImageMask::isAllMaskTrue(expr)) {
        newImage->copyData(expr);
    }
    else {
        newImage->attachMask(casacore::ArrayLattice<Bool>(shape));
        LogIO os;
        casacore::LatticeUtilities::copyDataAndMask(os, *newImage, expr, False);
    }
    ImageUtilities::copyMiscellaneous(*newImage, *realPart);
    return newImage;
}

template <class T>
std::shared_ptr<casacore::ImageInterface<std::complex<T>>> ImageFactory::makeComplex(
    SPCIIT realPart, SPCIIT imagPart, const String& outfile,
    const Record& region, Bool overwrite
) {
    _checkOutfile(outfile, overwrite);
    const IPosition realShape = realPart->shape();
    const IPosition imagShape = imagPart->shape();
    ThrowIf(! realShape.isEqual(imagShape), "Image shapes are not identical");
    const auto& cSysReal = realPart->coordinates();
    const auto& cSysImag = imagPart->coordinates();
    ThrowIf(
        !cSysReal.near(cSysImag), "Image Coordinate systems are not conformant"
    );
    String mask;
    auto subRealImage = SubImageFactory<T>::createSubImageRO(
        *realPart, region, mask, nullptr
    );
    auto subImagImage = SubImageFactory<T>::createSubImageRO(
        *imagPart, region, mask, nullptr
    );
    auto complexImage = makeComplexImage(
        std::dynamic_pointer_cast<const casacore::ImageInterface<T>>(subRealImage),
        std::dynamic_pointer_cast<const casacore::ImageInterface<T>>(subImagImage)
    );
    return SubImageFactory<std::complex<T>>::createImage(
        *complexImage, outfile, Record(), "", AxesSpecifier(),
        overwrite, false, false
    );
}

template <class T> SPIIT ImageFactory::createImage(
    const casacore::String& outfile,
    const casacore::CoordinateSystem& cSys, const casacore::IPosition& shape,
    casacore::Bool log, casacore::Bool overwrite,
    const vector<std::pair<casacore::LogOrigin, casacore::String> > *const &msgs
) {
    _checkOutfile(outfile, overwrite);
    ThrowIf(
        shape.nelements() != cSys.nPixelAxes(),
        "Supplied CoordinateSystem and image shape are inconsistent"
    );
    SPIIT image;
    if (outfile.empty()) {
        image.reset(new casacore::TempImage<T>(shape, cSys));
        ThrowIf(! image, "Failed to create TempImage");
    }
    else {
        image.reset(new casacore::PagedImage<T>(shape, cSys, outfile));
        ThrowIf(
            ! image,
            "Failed to create PagedImage"
        );
    }
    T *x = 0;
    auto creationMsg = _imageCreationMessage(outfile, shape, whatType(x));
    ImageHistory<T> hist(image);
    if (msgs) {
        hist.addHistory(*msgs);
    }
    LogOrigin lor("ImageFactory", __func__);
    hist.addHistory(lor, creationMsg);
    image->set(0.0);
    if (log) {
        LogIO mylog;
        mylog << LogOrigin("ImageFactory", __func__)
            << LogIO::NORMAL << creationMsg << LogIO::POST;
    }
    return image;
}

template<class T>
std::shared_ptr<casacore::TempImage<T>> ImageFactory::floatFromComplex(
    std::shared_ptr<const casacore::ImageInterface<std::complex<T>>> complexImage,
    ComplexToFloatFunction function
) {
    std::shared_ptr<TempImage<T>> newImage(
        new TempImage<T>(
            TiledShape(complexImage->shape()),
            complexImage->coordinates()
        )
    );
    {
        // FIXME use lattice copies
        auto mymask = complexImage->getMask();
        if (complexImage->hasPixelMask()) {
            mymask = mymask && complexImage->pixelMask().get();
        }
        if (! allTrue(mymask)) {
            newImage->attachMask(ArrayLattice<Bool>(mymask));
        }
    }
    ImageUtilities::copyMiscellaneous(*newImage, *complexImage);
    switch (function) {
    case REAL:
        // FIXME use lattice copies
        newImage->put(real(complexImage->get()));
        break;
    case IMAG:
        // FIXME use lattice copies
        newImage->put(imag(complexImage->get()));
        break;
    default:
        ThrowCc("Logic Error: Unhandled function");
    }
    return newImage;
}

template <class T> SPIIT ImageFactory::fromShape(
	const casacore::String& outfile,
	const casacore::Vector<casacore::Int>& shapeV,
	const casacore::Record& coordinates, casacore::Bool linear,
	casacore::Bool overwrite, casacore::Bool verbose,
    const vector<std::pair<casacore::LogOrigin, casacore::String> > *const &msgs
) {
	ThrowIf(
		shapeV.nelements() == 0, "The shape must have more than zero elements"
	);
	ThrowIf(anyTrue(shapeV <= 0), "All elements of shape must be positive");
    casacore::CoordinateSystem mycsys;
	std::unique_ptr<casacore::CoordinateSystem> csysPtr;
	if (coordinates.empty()) {
		mycsys = casacore::CoordinateUtil::makeCoordinateSystem(shapeV, linear);
		_centerRefPix(mycsys, shapeV);
	}
	else {
		csysPtr.reset(_makeCoordinateSystem(coordinates, shapeV));
        mycsys = *csysPtr;
	}
	return createImage<T>(outfile, mycsys, shapeV, verbose, overwrite, msgs);
}

template <class T> SPIIT ImageFactory::imageFromArray(
    const casacore::String& outfile, const casacore::Array<T>& pixels,
    const casacore::Record& csys, casacore::Bool linear,
    casacore::Bool overwrite, casacore::Bool verbose,
    const vector<std::pair<casacore::LogOrigin, casacore::String> > *const &msgs
) {
	SPIIT myim = fromShape<T>(
		outfile, pixels.shape().asVector(),
		csys, linear, overwrite, verbose, msgs
	);
	myim->put(pixels);
	return myim;
}

template <class T> SPIIT ImageFactory::_fromRecord(
    const casacore::RecordInterface& rec, const casacore::String& name
) {
    SPIIT image;
    casacore::String err;
    image.reset(new casacore::TempImage<T>());
    ThrowIf(
        ! image->fromRecord(err, rec),
        "Error converting image from record: " + err
    );
    if (! name.empty()) {
        image = SubImageFactory<T>::createImage(
            *image, name, casacore::Record(), "", false,
            true, false, false
        );
    }
    return image;
}

template <class T> ITUPLE ImageFactory::_rename(
	SPIIT& image, const casacore::String& name,
	const casacore::Bool overwrite
) {
	casacore::LogIO mylog;
	mylog << casacore::LogOrigin(className(), __func__);
	ThrowIf (! image, "Image pointer cannot be null");
	ThrowIf(
		! image->isPersistent(),
		"This image tool is not associated with a "
		"persistent disk file. It cannot be renamed"
	);
	ThrowIf(
		name.empty(), "new file name must be specified"
	);
	casacore::String oldName = image->name(false);
	ThrowIf(
		oldName.empty(),
		"Current file name is empty, cannot rename"
	);
	ThrowIf(
		oldName == name,
		"Specified output name is the same as the current image name"
	);
	// Let's see if it exists.  If it doesn't, then the user has deleted it
	casacore::File file(oldName);
	if (file.isSymLink()) {
		file = casacore::File(casacore::SymLink(file).followSymLink());
	}
	ThrowIf(
		! file.exists(), "The image to be renamed no longer exists"
	);
	_checkOutfile(name, overwrite);
	// close image before renaming
	image.reset();
	// Now try and move it
	casacore::Bool follow(true);
	if (file.isRegular(follow)) {
		casacore::RegularFile(file).move(name, overwrite);
	}
	else if (file.isDirectory(follow)) {
		casacore::Directory(file).move(name, overwrite);
	}
	else if (file.isSymLink()) {
		casacore::SymLink(file).copy(name, overwrite);
	}
	else {
		ThrowCc("Failed to rename file " + oldName + " to " + name);
	}
	mylog << casacore::LogIO::NORMAL << "Successfully renamed file " << oldName
	    << " to " << name << casacore::LogIO::POST;
	return fromFile(name);

}


}

