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

#include <imageanalysis/ImageAnalysis/ImageTask.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <casa/IO/FilebufIO.h>
#include <casa/OS/Directory.h>
#include <casa/OS/RegularFile.h>
#include <casa/OS/SymLink.h>
#include <images/Images/FITSImage.h>
#include <images/Images/ImageUtilities.h>
#include <images/Images/MIRIADImage.h>
#include <images/Images/PagedImage.h>
#include <images/Images/TempImage.h>
#include <tables/Tables/PlainTable.h>

#include <imageanalysis/ImageAnalysis/ImageHistory.h>
#include <imageanalysis/ImageAnalysis/ImageInputProcessor.h>
#include <imageanalysis/ImageAnalysis/ImageMask.h>
#include <imageanalysis/ImageAnalysis/SubImageFactory.h>
#include <imageanalysis/IO/LogFile.h>
#include <stdcasa/variant.h>

namespace casa {

template <class T> ImageTask<T>::ImageTask(
    const SPCIIT image,
    const casacore::String& region, const casacore::Record *const &regionPtr,
    const casacore::String& box, const casacore::String& chanInp,
    const casacore::String& stokes, const casacore::String& maskInp,
    const casacore::String& outname, casacore::Bool overwrite
) : _image(image), _regionPtr(regionPtr),_region(region), _box(box),
    _chan(chanInp), _stokesString(stokes), _mask(maskInp),
    _outname(outname), _overwrite(overwrite), _stretch(false),
    _logfile() {}

template <class T> ImageTask<T>::ImageTask(
    const SPCIIT image, const casacore::Record *const &regionPtr,
    const casacore::String& mask, const casacore::String& outname,
    casacore::Bool overwrite
) : _image(image), _regionPtr(regionPtr),
    _region(), _box(), _chan(), _stokesString(), _mask(mask),
    _outname(outname), _overwrite(overwrite) {}

template <class T> ImageTask<T>::~ImageTask() {}

template <class T> std::vector<OutputDestinationChecker::OutputStruct> ImageTask<T>::_getOutputStruct() {
    std::vector<OutputDestinationChecker::OutputStruct> outputs;
    _outname.trim();
    if (! _outname.empty()) {
        OutputDestinationChecker::OutputStruct outputImage;
        outputImage.label = "output image";
        outputImage.outputFile = &_outname;
        outputImage.required = true;
        outputImage.replaceable = _overwrite;
        outputs.push_back(outputImage);
    }
    return outputs;
}

template <class T> void ImageTask<T>::_construct(casacore::Bool verbose) {
    ThrowIf(
        ! _supportsMultipleBeams() && _image->imageInfo().hasMultipleBeams(),
        "This application does not support images with multiple "
        "beams. Please convolve your image with a single beam "
        "and run this application using that image"
    );
    casacore::String diagnostics;
    std::vector<OutputDestinationChecker::OutputStruct> outputs = _getOutputStruct();
    std::vector<OutputDestinationChecker::OutputStruct> *outputPtr = outputs.size() > 0
        ? &outputs
        : 0;
    std::vector<casacore::Coordinate::Type> necCoords = _getNecessaryCoordinates();
    std::vector<casacore::Coordinate::Type> *coordsPtr = necCoords.size() > 0
        ? &necCoords
        : 0;
    ThrowIf(
        _mustHaveSquareDirectionPixels()
        && _image->coordinates().hasDirectionCoordinate()
        && ! _image->coordinates().directionCoordinate().hasSquarePixels(),
        "This application requires that the input image must have square "
        "direction pixels, but the input image does not. Please regrid it "
        "so it does and rerun on the regridded image"
    );
    ImageInputProcessor inputProcessor;
    inputProcessor.process(
        _regionRecord, diagnostics, outputPtr,
        _stokesString, _image, _regionPtr,
        _region, _box, _chan,
        _getStokesControl(), _supportsMultipleRegions(),
        coordsPtr, verbose
    );
}

template <class T> void ImageTask<T>::setRegion(const casacore::Record& region) {
    ThrowIf(
        ! _supportsMultipleRegions() && region.isDefined("regions"),
        "This application does not support multiple region selection"
    );
    _regionRecord = region;
    _box = "";
    _chan = "";
    _stokesString = "";
    _region = "";
}

template <class T> void ImageTask<T>::_removeExistingFileIfNecessary(
    const casacore::String& filename, casacore::Bool overwrite, casacore::Bool warnOnly
) const {
    casacore::File out(filename);
    if (out.exists()) {
        if (overwrite) {
            File f(filename);
            ThrowIf(
                PlainTable::tableCache()(f.path().absoluteName()),
                filename + " is currently present in the table cache "
                + "and so is being used by another process. Please close "
                + "it in the other process first before attempting to "
                + "overwrite it"
            );
            if (out.isDirectory()) {
                casacore::Directory dir(filename);
                dir.removeRecursive();
            }
            else if (out.isRegular()) {
                casacore::RegularFile reg(filename);
                reg.remove();
            }
            else if (out.isSymLink()) {
                casacore::SymLink link(filename);
                link.remove();
            }
        }
        else {
            casacore::String msg = "File " + filename + " exists but overwrite is false "
                "so it cannot be overwritten";
            if (warnOnly) {
                *_log << casacore::LogIO::WARN << msg << casacore::LogIO::POST;
            }
            else {
                ThrowCc(msg);
            }
        }
    }
}

template <class T> void ImageTask<T>::_removeExistingOutfileIfNecessary() const {
    _removeExistingFileIfNecessary(_outname, _overwrite);
}

template <class T> casacore::String ImageTask<T>::_summaryHeader() const {
    casacore::String region = _box.empty() ? _region : "";
    ostringstream os;
    os << "Input parameters ---" << endl;
    os << "       --- imagename:           " << _image->name() << endl;
    os << "       --- region:              " << region << endl;
    os << "       --- box:                 " << _box << endl;
    os << "       --- channels:            " << _chan << endl;
    os << "       --- stokes:              " << _stokesString << endl;
    os << "       --- mask:                " << _mask << endl;
    return os.str();
}

template <class T> void ImageTask<T>::setLogfile(const casacore::String& lf) {
    if (lf.empty()) {
        return;
    }
    ThrowIf(
        ! _hasLogfileSupport(),
        "Logic Error: This task does not support writing of a log file"
    );
    try {
        _logfile.reset(new LogFile(lf));
        _logfile->setAppend(_logfileAppend);
    }
    catch (const casacore::AipsError& x) {}
}

template <class T> const std::shared_ptr<LogFile> ImageTask<T>::_getLogFile() const {
    ThrowIf(
        ! _hasLogfileSupport(),
        "Logic Error: This task does not support writing of a log file"
    );
    return _logfile;
}

template <class T> casacore::Bool ImageTask<T>::_openLogfile() {
    if (_logfile.get() == 0) {
        return false;
    }
    ThrowIf(
        ! _hasLogfileSupport(),
        "Logic Error: This task does not support writing of a log file"
    );
    return _logfile->open();
}

template <class T> void ImageTask<T>::_closeLogfile() const {
    if (_logfile) {
        _logfile->close();
    }
}

template<class T> casacore::Bool ImageTask<T>::_writeLogfile(
    const casacore::String& output, const casacore::Bool open, const casacore::Bool close
) {
    ThrowIf(
        ! _hasLogfileSupport(),
        "Logic Error: This task does not support writing of a log file"
    );
    if (! _logfile) {
        return false;
    }
    return _logfile->write(output, open, close);
}

template <class T> void ImageTask<T>::setLogfileAppend(casacore::Bool a) {
    ThrowIf(
        ! _hasLogfileSupport(),
        "Logic Error: This task does not support writing of a log file"
    );
    _logfileAppend = a;
    if (_logfile) {
        _logfile->setAppend(a);
    }
}

template <class T> void ImageTask<T>::addHistory(
    const vector<std::pair<casacore::String, casacore::String> >& msgs
) const {
    _newHistory.insert(
        _newHistory.end(), msgs.begin(), msgs.end()
    );
}

template <class T> void ImageTask<T>::addHistory(
    const casacore::LogOrigin& origin, const casacore::String& msg
) const {
    std::pair<casacore::String, casacore::String> x;
    x.first = origin.fullName();
    x.second = msg;
    _newHistory.push_back(x);
}

template <class T> void ImageTask<T>::addHistory(
    const casacore::LogOrigin& origin, const vector<casacore::String>& msgs
) const {
    std::pair<casacore::String, casacore::String> x;
    x.first = origin.fullName();
    for( casacore::String m: msgs ) {
        x.second = m;
        _newHistory.push_back(x);
    }
}

template <class T> void ImageTask<T>::addHistory(
    const casacore::LogOrigin& origin, const casacore::String& taskname,
    const vector<casacore::String>& paramNames, const vector<casac::variant>& paramValues
) const {
    auto appHistory = ImageHistory<T>::getApplicationHistory(
        origin, taskname, paramNames, paramValues, _image->name()
    );
    _newHistory.insert(_newHistory.end(), appHistory.begin(), appHistory.end());
}

template <class T> void ImageTask<T>::_copyMask(
    casacore::Lattice<casacore::Bool>& mask, const casacore::ImageInterface<T>& image
) {
    auto cursorShape = image.niceCursorShape(4096*4096);
    casacore::LatticeStepper stepper(image.shape(), cursorShape, casacore::LatticeStepper::RESIZE);
    casacore::RO_MaskedLatticeIterator<T> iter(image, stepper);
    casacore::LatticeIterator<casacore::Bool> miter(mask, stepper);
    std::unique_ptr<casacore::RO_LatticeIterator<casacore::Bool>> pmiter;
    if (image.hasPixelMask()) {
        pmiter.reset(new casacore::RO_LatticeIterator<casacore::Bool>(image.pixelMask(), stepper));
    }
    for (iter.reset(); ! iter.atEnd(); ++iter, ++miter) {
        auto mymask = iter.getMask();
        if (pmiter) {
            mymask = mymask && pmiter->cursor();
            pmiter->operator++();
        }
        miter.rwCursor() = mymask;
    }
}

template <class T> void ImageTask<T>::_copyData(
    Lattice<T>& data, const Lattice<T>& image
) {
    auto cursorShape = image.niceCursorShape(4096*4096);
    casacore::LatticeStepper stepper(image.shape(), cursorShape, casacore::LatticeStepper::RESIZE);
    casacore::RO_LatticeIterator<T> iter(image, stepper);
    casacore::LatticeIterator<T> diter(data, stepper);
    for (iter.reset(); ! iter.atEnd(); ++iter, ++diter) {
        diter.rwCursor() = iter.cursor();
    }
}

template <class T> casacore::Bool ImageTask<T>::_isPVImage() const {
    const CoordinateSystem& csys = _image->coordinates();
    if (csys.hasLinearCoordinate() && csys.hasSpectralAxis()) {
        auto pixelAxes = csys.linearAxesNumbers();
        auto nPixelAxes = pixelAxes.size();
        auto names = csys.worldAxisNames();
        for (uInt j=0; j<nPixelAxes; ++j) {
            if (pixelAxes[j] >= 0 && names[pixelAxes[j]] == "Offset") {
                return true;
            }
        }
    }
    return false;
}

template <class T> SPIIT ImageTask<T>::_prepareOutputImage(
    const casacore::ImageInterface<T>& image, const casacore::Array<T> *const values,
    const casacore::ArrayLattice<casacore::Bool> *const mask,
    const casacore::IPosition *const outShape, const casacore::CoordinateSystem *const coordsys,
    const casacore::String *const outname, casacore::Bool overwrite, casacore::Bool dropDegen
) const {
    auto oShape = outShape == 0 ? image.shape() : *outShape;
    casacore::CoordinateSystem csys = coordsys ? *coordsys : image.coordinates();
    std::shared_ptr<casacore::TempImage<T>> tmpImage(
        new casacore::TempImage<T>(casacore::TiledShape(oShape), csys)
    );
    if (mask && (! ImageMask::isAllMaskTrue(*mask))) {
        tmpImage->attachMask(*mask);
    }
    // because subimages can have two types of masks, a region mask and
    // a pixel mask, but most other types of images really just have a
    // pixel mask. its very confusing
    else if (image.hasPixelMask() || image.isMasked()) {
        // A paged array is stored on disk and is preferred over an
        // ArrayLattice which will exhaust memory for large images.
        std::unique_ptr<casacore::Lattice<casacore::Bool>> mymask;
        const static auto mmasksize = 4096*4096;
        if (image.size() > mmasksize) {
            mymask.reset(new casacore::PagedArray<casacore::Bool>(image.shape()));
        }
        else {
            mymask.reset(new casacore::ArrayLattice<casacore::Bool>(image.shape()));
        }
        _copyMask(*mymask, image);
        if (! ImageMask::isAllMaskTrue(image)) {
            tmpImage->attachMask(*mymask);
        }
    }
    auto myOutname = outname ? *outname : _outname;
    if (! outname) {
        overwrite = _overwrite;
    }
    SPIIT outImage = tmpImage;
    if (values) {
        outImage->put(*values);
    }
    else {
        // FIXME this is a superfluous copy if  dropgen || ! myOutname.empty()
        _copyData(*outImage, image);
    }
    if (dropDegen || ! myOutname.empty()) {
        if (! myOutname.empty()) {
            _removeExistingFileIfNecessary(myOutname, overwrite);
        }
        casacore::String emptyMask = "";
        casacore::Record empty;
        outImage = SubImageFactory<T>::createImage(
            *tmpImage, myOutname, empty, emptyMask,
            dropDegen, false, true, false
        );
    }
    casacore::ImageUtilities::copyMiscellaneous(*outImage, image);
    _doHistory(outImage);
    // CAS-9267 force metadata to be written to disk, in case of PagedImage
    outImage->flush();
    return outImage;
}

template <class T> SPIIT ImageTask<T>::_prepareOutputImage(
    const casacore::ImageInterface<T>& image, casacore::Bool dropDeg
) const {
    if (! _outname.empty()) {
        _removeExistingFileIfNecessary(_outname, _overwrite);
    }
    static const casacore::Record empty;
    static const casacore::String emptyString;
    auto outImage = SubImageFactory<T>::createImage(
        image, _outname, empty, emptyString, dropDeg,
        _overwrite, true, false, false
    );
    _doHistory(outImage);
    return outImage;
}

template <class T> SPIIT ImageTask<T>::_prepareOutputImage(
    const casacore::ImageInterface<T>& image, const casacore::String& outname,
    casacore::Bool overwrite, casacore::Bool warnOnly
) const {
    if (! outname.empty()) {
        _removeExistingFileIfNecessary(outname, overwrite, warnOnly);
    }
    static const casacore::Record empty;
    static const casacore::String emptyString;
    auto outImage = SubImageFactory<T>::createImage(
        image, outname, empty, emptyString, false,
        overwrite, true, false, false
    );
    _doHistory(outImage);
    return outImage;
}


template <class T> SPIIT ImageTask<T>::_prepareOutputImage(
    const casacore::ImageInterface<T>& image, const casacore::Lattice<T>& data
) const {
    if (! _outname.empty()) {
        _removeExistingFileIfNecessary(_outname, _overwrite);
    }
    static const casacore::Record empty;
    static const casacore::String emptyString;
    auto outImage = SubImageFactory<T>::createImage(
        image, _outname, empty, emptyString, false,
        _overwrite, true, false, false, &data
    );
    _doHistory(outImage);
    return outImage;
}

template <class T>
template <class U> void ImageTask<T>::_doHistory(std::shared_ptr<casacore::ImageInterface<U>>& image) const {
    if (! _suppressHistory) {
        ImageHistory<U> history(image);
        if (history.get(false).empty()) {
            history.append(_image);
        }
        history.addHistory(_newHistory);
        /*
        for (const auto& line: _newHistory) {
            history.addHistory(line.first, line.second);
        }
        */
    }
}

template <class T> void ImageTask<T>::_reportOldNewImageShapes(const IPosition& outShape) const {
    LogOrigin lor(getClass(), __func__);
    ostringstream os;
    os << "Original " << _getImage()->name() << " size => "
        << _getImage()->shape();
    addHistory(lor, os.str());
    *_getLog() << LogIO::NORMAL << os.str() << LogIO::POST;
    os.str("");
    os << "New " << _getOutname() << " size => " << outShape;
    addHistory(lor, os.str());
    *_getLog() << LogIO::NORMAL << os.str() << LogIO::POST;
}

template <class T> void ImageTask<T>::_reportOldNewImageShapes(const ImageInterface<T>& out) const {
    _reportOldNewImageShapes(out.shape());
}

}

