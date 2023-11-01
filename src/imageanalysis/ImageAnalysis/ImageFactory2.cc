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

#include <casacore/casa/OS/EnvVar.h>
#include <casacore/casa/System/AppState.h>
#include <casacore/images/Images/ImageFITSConverter.h>
#include <casacore/images/Images/ImageUtilities.h>
#include <casacore/images/Images/ImageOpener.h>

#include <imageanalysis/ImageAnalysis/PixelValueManipulator.h>
#include <imageanalysis/ImageAnalysis/SubImageFactory.h>

using namespace std;

using namespace casacore;
namespace casa {


std::shared_ptr<ComponentListImage> ImageFactory::createComponentListImage(
    const String& outfile, const Record& cl, const Vector<Int>& shape,
    const Record& csys, Bool overwrite, Bool log, Bool cache
) {
    _checkOutfile(outfile, overwrite);
    ComponentList mycl;
    String err;
    ThrowIf(! mycl.fromRecord(err, cl), err);
    CoordinateSystem mycsys;
    std::unique_ptr<CoordinateSystem> csysPtr;
    if (csys.empty()) {
        mycsys = CoordinateUtil::makeCoordinateSystem(shape, False);
        _centerRefPix(mycsys, shape);
    }
    else {
        csysPtr.reset(
            _makeCoordinateSystem(csys, shape)
        );
        mycsys = *csysPtr;
    }

    std::shared_ptr<ComponentListImage> image(
        outfile.empty()
        ? new ComponentListImage(mycl, mycsys, IPosition(shape), cache)
        : new ComponentListImage(mycl, mycsys, IPosition(shape), outfile, cache)
    );
    ostringstream os;
    os << "Created ComponentListImage " << outfile
       << " of shape " << shape << ".";
    ImageHistory<Float> hist(image);
    LogOrigin lor("ImageFactory", __func__);
    hist.addHistory(lor, os.str());
    if (log) {
        casacore::LogIO mylog;
        mylog << casacore::LogIO::NORMAL << os.str() << casacore::LogIO::POST;
    }
    return image;
}

SPIIF ImageFactory::floatImageFromShape(
		const String& outfile, const Vector<Int>& shape,
		const Record& csys, Bool linear,
		Bool overwrite, Bool verbose,
		const vector<std::pair<LogOrigin, String> > *const &msgs
) {
	return fromShape<Float>(
			outfile, shape, csys, linear,
			overwrite, verbose, msgs
	);
}

SPIIC ImageFactory::complexImageFromShape(
		const String& outfile, const Vector<Int>& shape,
		const Record& csys, Bool linear,
		Bool overwrite, Bool verbose,
		const vector<std::pair<LogOrigin, String> > *const &msgs
) {
	return fromShape<Complex>(
			outfile, shape, csys, linear,
			overwrite, verbose, msgs
	);
}

SPIID ImageFactory::doubleImageFromShape(
    const String& outfile, const Vector<casacore::Int>& shape,
    const Record& csys, Bool linear,
    Bool overwrite, Bool verbose,
    const std::vector<std::pair<LogOrigin, String> > *const &msgs
) {
    return fromShape<Double>(
        outfile, shape, csys, linear,
        overwrite, verbose, msgs
    );
}

SPIIDC ImageFactory::complexDoubleImageFromShape(
    const String& outfile, const Vector<Int>& shape,
    const Record& csys, casacore::Bool linear,
    Bool overwrite, casacore::Bool verbose,
    const std::vector<std::pair<LogOrigin, String> > *const &msgs
) {
    return fromShape<DComplex>(
        outfile, shape, csys, linear, overwrite, verbose, msgs
    );
}
/*
SPIIF ImageFactory::fromASCII(
    const String& outfile, const String& infile,
    const IPosition& shape, const String& sep, const Record& csys,
    const Bool linear, const Bool overwrite
) {
    Path filePath(infile);
    auto fileName = filePath.expandedName();
    ifstream inFile(fileName.c_str());
    ThrowIf(!inFile, "Cannot open " + infile);
    auto n = shape.product();
    auto nx = shape[0];
    Vector<Float> a(n, 0.0);
    int idx = 0;
    string line;
    unique_ptr<string[]> managed(new string[2 * nx]);
    auto line2 = managed.get();
    uInt iline = 0;
    uInt nl = 1;
    while (nl > 0) {
        getline(inFile, line, '\n');
        nl = split(line, line2, 2 * nx, sep);
        if (nl > 0) {
            ThrowIf(
                nl != nx,
                "Length of line " + String::toString(iline)
                + " is " + String::toString(nl) + " but should be "
                + String::toString(nx)
            );
            for (uInt i = 0; i < nx; i++) {
                a[idx + i] = atof(line2[i].c_str());
            }
            idx += nx;
            iline += 1;
        }
    }
    Vector<Float> vec(n);
    for (uInt i = 0; i < n; ++i) {
        vec[i] = a[i];
    }
    Array<Float> pixels(vec.reform(IPosition(shape)));
    return imageFromArray(outfile, pixels, csys, linear, overwrite);
}
*/
ITUPLE ImageFactory::fromImage(
    const String& outfile, const String& infile,
    const Record& region, const String& mask, Bool dropdeg,
    Bool overwrite
) {
    auto imagePtrs = fromFile(infile, False);
    auto imageF = std::get<0>(imagePtrs);
    auto imageC = std::get<1>(imagePtrs);
    auto imageD = std::get<2>(imagePtrs);
    auto imageDC = std::get<3>(imagePtrs);
    if (imageF) {
        imageF = SubImageFactory<Float>::createImage(
            *imageF, outfile, region,
            mask, dropdeg, overwrite, false, false
        );
        ThrowIf(! imageF, "Failed to create image");
    }
    else if (imageC) {
        imageC = SubImageFactory<Complex>::createImage(
            *imageC, outfile, region,
            mask, dropdeg, overwrite, false, false
        );
        ThrowIf(! imageC, "Failed to create image");
    }
    else if (imageD) {
        imageD = SubImageFactory<Double>::createImage(
            *imageD, outfile, region,
            mask, dropdeg, overwrite, false, false
        );
        ThrowIf(! imageD, "Failed to create image");
    }
    else {
        imageDC = SubImageFactory<DComplex>::createImage(
            *imageDC, outfile, region,
            mask, dropdeg, overwrite, false, false
        );
        ThrowIf(! imageDC, "Failed to create image");
    }
    LogIO mylog;
    mylog << LogOrigin("ImageFactory", __func__);
    ITUPLE ret(imageF, imageC, imageD, imageDC);
    mylog << LogIO::NORMAL << _imageCreationMessage(outfile, ret)
        << LogIO::POST;
    return ret;
}

pair<SPIIF, SPIIC> ImageFactory::fromRecord(
    const RecordInterface& rec, const String& name
) {
    auto mytype = rec.type(rec.fieldNumber("imagearray"));
    pair<SPIIF, SPIIC> imagePair;
    if (isReal(mytype)) {
        imagePair.first = _fromRecord<Float>(rec, name);
    }
    else {
        imagePair.second = _fromRecord<Complex>(rec, name);
    }
    return imagePair;
}

void ImageFactory::_centerRefPix(
	CoordinateSystem& csys, const IPosition& shape
) {
	Int after = -1;
	Int iS = csys.findCoordinate(Coordinate::STOKES, after);
	Int sP = -1;
	if (iS >= 0) {
		Vector<Int> pixelAxes = csys.pixelAxes(iS);
		sP = pixelAxes(0);
	}
	Vector<Double> refPix = csys.referencePixel();
	for (Int i = 0; i < Int(refPix.nelements()); i++) {
		if (i != sP)
			refPix(i) = Double(shape(i) / 2);
	}
	csys.setReferencePixel(refPix);
}

CoordinateSystem* ImageFactory::_makeCoordinateSystem(
    const Record& coordinates, const IPosition& shape
) {
    std::unique_ptr<CoordinateSystem> csys;
    if (coordinates.nfields() == 1) {
        // must be a record as an element
        Record tmp(coordinates.asRecord(RecordFieldId(0)));
        csys.reset(CoordinateSystem::restore(tmp, ""));
    } 
    else {
        csys.reset(CoordinateSystem::restore(coordinates, ""));
    }
    // Fix up any body longitude ranges...
    String errMsg;
    if (csys->hasDirectionCoordinate()) {
        auto axes = csys->directionAxesNumbers();
        if (min(axes) >= 0) {
            ThrowIf(
                ! CoordinateUtil::cylindricalFix(*csys, errMsg, shape),
                errMsg
            );
        }
        else {
            LogIO log(LogOrigin("ImageFactory", __func__));
            log << LogIO::WARN << "Direction coordinate has at least one "
                << "axis that has been removed, skipping cylindrical fix "
                << "which is normally only important for imported image formats "
                << "such as FITS" << LogIO::POST;
        }
    }
    return csys.release();
}

ITUPLE ImageFactory::fromFile(const String& infile, Bool cache) {
    _checkInfile(infile);
    ComponentListImage::registerOpenFunction();
    unique_ptr<LatticeBase> latt(ImageOpener::openImage(infile));
    ThrowIf (! latt, "Unable to open image " + infile);
    auto imagePtrs = _fromLatticeBase(latt);
    auto imageF = std::get<0>(imagePtrs);
    if (
        imageF
        && imageF->imageType().contains(ComponentListImage::IMAGE_TYPE)
    ) {
        std::dynamic_pointer_cast<ComponentListImage>(imageF)->setCache(cache);
    }
    return imagePtrs;
}

SPIIF ImageFactory::fromFile(
    const casacore::String& filename, casacore::Float, casacore::Bool cache
) {
    auto t = fromFile(filename, cache);
    return std::get<0>(t);
}

SPIIC ImageFactory::fromFile(
    const casacore::String& filename, casacore::Complex, casacore::Bool cache
) {
    auto t = fromFile(filename, cache);
    return std::get<1>(t);
}

SPIID ImageFactory::fromFile(
    const casacore::String& filename, casacore::Double, casacore::Bool cache
) {
    auto t = fromFile(filename, cache);
    return std::get<2>(t);
}

SPIIDC ImageFactory::fromFile(
    const casacore::String& filename, casacore::DComplex, casacore::Bool cache
) {
    auto t = fromFile(filename, cache);
    return std::get<3>(t);
}
 
ITUPLE ImageFactory::_fromLatticeBase(
    unique_ptr<LatticeBase>& latt
) {
    DataType dataType = latt->dataType();
    tuple<SPIIF, SPIIC, SPIID, SPIIDC> ret(nullptr, nullptr, nullptr, nullptr);
    if (dataType == TpFloat) {
        auto f = SPIIF(
            dynamic_cast<ImageInterface<Float> *>(latt.release())
        );
        ThrowIf(! f, "Could not cast LatticeBase to ImageInterface<Float>");
        std::get<0>(ret) = f;
    }
    else if (dataType == TpComplex) {
        auto c = SPIIC(
            dynamic_cast<ImageInterface<Complex> *>(latt.release())
        );
        ThrowIf(! c, "Could not cast LatticeBase to ImageInterface<Complex>");
        std::get<1>(ret) = c;
    }
    else if (dataType == TpDouble) {
        auto d = SPIID(
            dynamic_cast<ImageInterface<Double> *>(latt.release())
        );
        ThrowIf(! d, "Could not cast LatticeBase to ImageInterface<Double>");
        std::get<2>(ret) = d;
    }
    else if (dataType == TpDComplex) {
        auto dc = SPIIDC(
            dynamic_cast<ImageInterface<DComplex> *>(latt.release())
        );
        ThrowIf(! dc, "Could not cast LatticeBase to ImageInterface<DComplex>");
        std::get<3>(ret) = dc;
    }
    else {
        ostringstream os;
        os << dataType;
        throw AipsError("unsupported image data type " + os.str());
    }
    return ret;
}

void ImageFactory::_checkInfile(const String& infile) {
    ThrowIf(
        infile.empty(), "File name is empty"
    );
    File thefile(infile);
    ThrowIf(
        ! thefile.exists(),
        "File " + infile + " does not exist."
    );
}

SPIIF ImageFactory::fromFITS(
    const String& outfile, const String& fitsfile,
    const Int whichrep, const Int whichhdu,
    const Bool zeroBlanks, const Bool overwrite
) {
    _checkOutfile(outfile, overwrite);
    ThrowIf(
        whichrep < 0,
        "The Coordinate Representation index must be non-negative"
    );
    ImageInterface<Float> *x = nullptr;
    String error;
    Bool rval = ImageFITSConverter::FITSToImage(
        x, error, outfile, fitsfile, whichrep, whichhdu,
        HostInfo::memoryFree() / 1024, overwrite, zeroBlanks
    );
    SPIIF pOut(x);
    ThrowIf(! rval || ! pOut, error);
    return pOut;
}

void ImageFactory::rename(
	SPIIF& image, const String& name, const Bool overwrite
) {
	image = std::get<0>(_rename(image, name, overwrite));
}

void ImageFactory::rename(
	SPIIC& image, const String& name, const Bool overwrite
) {
	image = std::get<1>(_rename(image, name, overwrite));
}
/*
void ImageFactory::toASCII(
	SPCIIF image, const String& outfile, Record& region,
	const String& mask, const String& sep,
	const String& format, Double maskvalue,
	Bool overwrite, Bool extendMask
) {
	String outFileStr(outfile);
	// Check output file name

	if (outFileStr.empty()) {
		Bool strippath(true);
		outFileStr = image->name(strippath);
		outFileStr = outFileStr + ".ascii";
	}
	_checkOutfile(outFileStr, overwrite);

	Path filePath(outFileStr);
	String fileName = filePath.expandedName();

	ofstream outFile(fileName.c_str());
	ThrowIf(! outFile, "Cannot open file " + outfile);

	PixelValueManipulator<Float> pvm(
		image, &region, mask
	);
	pvm.setVerbosity(ImageTask<Float>::QUIET);
	pvm.setStretch(extendMask);
	auto ret = pvm.get();

	auto pixels = ret.asArrayFloat("values");
	auto pixmask = ret.asArrayBool("mask");
	auto shape = pixels.shape();
	auto vshp = pixmask.shape();
	uInt nx = shape(0);
	uInt n = pixels.size();
	uInt nlines = 0;
	if (nx > 0) {
		nlines = n / nx;
	}
	IPosition vShape(1, n);
	Vector<Float> vpixels(pixels.reform(vShape));
	if (pixmask.size() > 0) {
		Vector<Bool> vmask(pixmask.reform(vShape));
		for (uInt i = 0; i<n; ++i) {
			if (! vmask[i]) {
				vpixels[i] = (float) maskvalue;
			}
		}
	}
	int idx = 0;
	uInt nline = 0;
	char nextentry[128];
	while (nline < nlines) {
		string line;
		for (uInt i = 0; i < nx - 1; ++i) {
			sprintf(
				nextentry, (format + "%s").c_str(), vpixels[idx + i],
				sep.c_str()
			);
			line += nextentry;
		}
		sprintf(nextentry, format.c_str(), vpixels[idx + nx - 1]);
		line += nextentry;
		outFile << line.c_str() << endl;
		idx += nx;
		nline += 1;
	}
}
*/
void ImageFactory::toFITS(
	SPCIIF image, const String& outfile, Bool velocity, Bool optical,
	Int bitpix, Double minpix, Double maxpix,
	const Record& region, const String& mask,
	Bool overwrite, Bool dropdeg, Bool deglast,
	Bool dropStokes, Bool stokeslast,
	Bool wavelength, Bool airWavelength,
	const String& origin, Bool stretch, Bool history
) {
	LogIO log;
	log << LogOrigin("ImageFactory", __func__);
	_checkOutfile(outfile, overwrite);
	// The SubImage that goes to the FITSCOnverter no longer will know
	// the name of the parent mask, so spit it out here
	if (image->isMasked()) {
		log << LogIO::NORMAL << "Applying mask of name '"
			<< image->getDefaultMask() << "'" << LogIO::POST;
	}
	IPosition keepAxes;
	if (! dropdeg) {
		if (dropStokes) {
			const auto& cSys = image->coordinates();
			if (
				cSys.hasPolarizationCoordinate()
				&& cSys.nCoordinates() > 1
			) {
				// Stokes axis exists and its not the only one
				auto cNames = cSys.worldAxisNames();
				keepAxes = IPosition(cNames.size() - 1);
				uInt j = 0;
				for (uInt i = 0; i < cNames.size(); ++i) {
					if (cNames(i) != "Stokes") { // not Stokes?
						keepAxes(j) = i; // keep it
						j++;
					}
				}
			}
		}
	}
	AxesSpecifier axesSpecifier;
	if (dropdeg) {
		axesSpecifier = AxesSpecifier(false);
	}
	else if (! keepAxes.empty()) {
		axesSpecifier = AxesSpecifier(keepAxes);
	}
	auto subImage = SubImageFactory<Float>::createSubImageRO(
		*image, region, mask, &log, axesSpecifier, stretch
	);
	// FIXME remove when the casacore interface has been updated to const
	SPIIF myclone(subImage->cloneII());
	String error;
	ThrowIf (
		! ImageFITSConverter::ImageToFITS(
			error, *myclone, outfile,
			HostInfo::memoryFree() / 1024,
			velocity, optical, bitpix, minpix,
			maxpix, overwrite, deglast,
			false, //  verbose default
			stokeslast,	wavelength,
			airWavelength, // for airWavelength=true
			origin, history
		), error
	);
}

SPIIF ImageFactory::testImage(
    const String& outfile, const Bool overwrite,
    const String& imagetype
) {
    // setup image name relative to the data root...
    String testname;
    if (imagetype.contains("cube")) {
        testname = "demo/Images/test_imageFloat.fits";
    }
    else if (imagetype.contains("2d")) {
        testname = "demo/Images/imagetestimage.fits";
    }
    else {
        ThrowCc("imageType must be either \"cube\" or \"2d\"");
    }

    String fitsfile;

    const casacore::AppState &state = casacore::AppStateSource::fetch( );
    if ( state.initialized( ) )
        fitsfile = state.resolve(testname);

    else {
        String var = EnvironmentVariable::get("CASAPATH");
        if (var.empty()) {
            var = EnvironmentVariable::get("AIPSPATH");
        }
        ThrowIf( var.empty(),
                 "Neither CASAPATH nor AIPSPATH is set, so cannot locate data directory" );
        String fields[4];
        Int num = split(var, fields, 4, String(" "));
        ThrowIf (num <= 0, "Bad CASAPATH/AIPSPATH value: " + var);

        fitsfile = fields[0] + "/data/" + testname;
    }

    return fromFITS(
        outfile, fitsfile, 0, 0, false, overwrite
    );
}

void ImageFactory::_checkOutfile(const String& outfile, Bool overwrite) {
    if (! overwrite && ! outfile.empty()) {
        NewFile validfile;
        String errmsg;
        ThrowIf(
            ! validfile.valueOK(outfile, errmsg), errmsg
        );
    }
}

String ImageFactory::_imageCreationMessage(
    const String& outfile, const IPosition& shape,
    DataType dataType
) {
    auto blank = outfile.empty();
    ostringstream os;
    os << "Created "
        << (blank ? "Temp" : "Paged") << " image "
        << (blank ? "" : "'" + outfile + "'")
        << " of shape " << shape << " with "
        << dataType << " valued pixels.";
    return os.str();
}

String ImageFactory::_imageCreationMessage(
    const String& outfile, const ITUPLE& imagePtrs
) {
    if (auto x = std::get<0>(imagePtrs)) {
        return _imageCreationMessage(
            outfile, x->shape(), TpFloat
        );
    }
    else if (auto x = std::get<1>(imagePtrs)) {
        return _imageCreationMessage(
            outfile, x->shape(), TpComplex
        );
    }
    else if (auto x = std::get<2>(imagePtrs)) {
        return _imageCreationMessage(
            outfile, x->shape(), TpDouble
        );
    }
    else if (auto x = std::get<3>(imagePtrs)) {
        return _imageCreationMessage(
            outfile, x->shape(), TpDComplex
        );
    }
    else {
        ThrowCc("Logic Error");
    }
}


}

