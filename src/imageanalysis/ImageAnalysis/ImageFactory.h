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
//# $Id: tSubImage.cc 20567 2009-04-09 23:12:39Z gervandiepen $

#ifndef IMAGEANALYSIS_IMAGEFACTORY_H
#define IMAGEANALYSIS_IMAGEFACTORY_H

#include <imageanalysis/ImageTypedefs.h>

#include <imageanalysis/Images/ComponentListImage.h>

#include <casacore/casa/Arrays/Array.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Containers/Record.h>
#include <casacore/casa/Logging/LogOrigin.h>
#include <casacore/casa/namespace.h>
#include <casacore/lattices/Lattices/LatticeBase.h>
#include <casacore/casa/Arrays/ArrayFwd.h>
#include <utility>
#include <vector>

namespace casacore{

class CoordinateSystem;
class IPosition;
class Record;
template <class T> class TempImage;

}

namespace casa {

class ImageFactory {
	// <summary>
	// Static methods for creating images 
	// </summary>

	// <reviewed reviewer="" date="" tests="" demos="">
	// </reviewed>

	// <prerequisite>
	// </prerequisite>

	// <etymology>
	// </etymology>

	// <synopsis>
	// </synopsis>

public:

	enum ComplexToFloatFunction {
		REAL,
		IMAG
	};

	ImageFactory() = delete;

    ~ImageFactory() {};

    // Create a casacore::TempImage if outfile is empty, otherwise a PagedImage.
    // If log is true, log useful messages, quiet if false. Created image
    // has all pixel values set to zero and is unmasked.
    template <class T> static SPIIT createImage(
        const casacore::String& outfile,
        const casacore::CoordinateSystem& cSys, const casacore::IPosition& shape,
        casacore::Bool log, casacore::Bool overwrite,
        const std::vector<std::pair<casacore::LogOrigin, casacore::String> > *const &msgs
    );

    // create a ComponentListImage
    static std::shared_ptr<ComponentListImage> createComponentListImage(
        const casacore::String& outfile, const casacore::Record& cl,
        const casacore::Vector<casacore::Int>& shape,
        const casacore::Record& csys, casacore::Bool overwrite,
        casacore::Bool log, casacore::Bool cache
    );

    static casacore::String className() {
        static const casacore::String s = "ImageFactory"; return s;
    }

    // create an image with the specified shape and specified coordinate system.
    // If outfile is blank, the returned object is a casacore::TempImage, casacore::PagedImage otherwise.
    // If csys is empty,
    // a default coordiante system is attached to the image, and if linear
    // is true, it has linear coordinates in place of the direction coordinate.

    static SPIIF floatImageFromShape(
    	const casacore::String& outfile,
    	const casacore::Vector<casacore::Int>& shape,
    	const casacore::Record& csys, casacore::Bool linear=true,
    	casacore::Bool overwrite=false, casacore::Bool verbose=true,
        const std::vector<
            std::pair<casacore::LogOrigin, casacore::String>
        > *const &msgs=nullptr
    );

    static SPIIC complexImageFromShape(
    	const casacore::String& outfile,
    	const casacore::Vector<casacore::Int>& shape,
    	const casacore::Record& csys, casacore::Bool linear=true,
    	casacore::Bool overwrite=false, casacore::Bool verbose=true,
        const std::vector<
            std::pair<casacore::LogOrigin, casacore::String>
        > *const &msgs=nullptr
    );

    static SPIID doubleImageFromShape(
        const casacore::String& outfile,
        const casacore::Vector<casacore::Int>& shape,
        const casacore::Record& csys, casacore::Bool linear=true,
        casacore::Bool overwrite=false, casacore::Bool verbose=true,
        const std::vector<
            std::pair<casacore::LogOrigin, casacore::String>
        > *const &msgs=nullptr
    );

    static SPIIDC complexDoubleImageFromShape(
        const casacore::String& outfile, const casacore::Vector<casacore::Int>& shape,
        const casacore::Record& csys, casacore::Bool linear=true,
        casacore::Bool overwrite=false, casacore::Bool verbose=true,
        const std::vector<
            std::pair<casacore::LogOrigin, casacore::String>
        > *const &msgs=nullptr
    );

    // only the pointer of the correct data type will be valid, the other
    // will be null.
    static ITUPLE fromImage(
        const casacore::String& outfile, const casacore::String& infile,
        const casacore::Record& region, const casacore::String& mask,
        casacore::Bool dropdeg=false,
        casacore::Bool overwrite=false
    );

    template <class T> static SPIIT imageFromArray(
    	const casacore::String& outfile, const casacore::Array<T>& pixels,
    	const casacore::Record& csys, casacore::Bool linear=false,
    	casacore::Bool overwrite=false, casacore::Bool verbose=true,
    	const std::vector<
    	    std::pair<casacore::LogOrigin, casacore::String>
        > *const &msgs=nullptr
    );
/*
    static SPIIF fromASCII(
        const casacore::String& outfile, const casacore::String& infile,
        const casacore::IPosition& shape, const casacore::String& sep,
        const casacore::Record& csys, const casacore::Bool linear,
        const casacore::Bool overwrite
    );
*/
    template <class T> static SPIIT fromShape(
        const casacore::String& outfile,
        const casacore::Vector<casacore::Int>& shape,
        const casacore::Record& csys, casacore::Bool linear=false,
        casacore::Bool overwrite=false, casacore::Bool verbose=false,
        const std::vector<
            std::pair<casacore::LogOrigin, casacore::String>
        > *const &msgs=nullptr
    );

    // Create a float-valued image from a complex-valued image. All metadata is
    // copied and pixel values are initialized according to <src>func</src>.
    template<class T>
    static std::shared_ptr<casacore::TempImage<T>> floatFromComplex(
        std::shared_ptr<
            const casacore::ImageInterface<std::complex<T>>
        > complexImage, ComplexToFloatFunction func
    );

    // Create a complex-valued image from two real valued images. All metadata
    // is copied from the real image and pixel values are initialized to
    // realPart + i*complexPart
    template<class T>
    static std::shared_ptr<casacore::TempImage<std::complex<T>>> makeComplexImage(
    	SPCIIT realPart, SPCIIT imagPart
    );

    // Create a complex-valued image from two real-valued images. All metadata
    // is copied from the real image and pixel values are initialized to
    // realPart + i*complexPart
    template<class T>
    static std::shared_ptr<casacore::ImageInterface<std::complex<T>>> makeComplex(
    	SPCIIT realPart, SPCIIT imagPart, const casacore::String& outfile,
		const casacore::Record& region, casacore::Bool overwrite = false
    );

    // exactly one of the pointers will not be null, indicating the
    // pixel data type. Cache is only used if the image is a CompoenentListImage
    static ITUPLE fromFile(
        const casacore::String& filename, casacore::Bool cache=casacore::True
    );

    // for specific types
    static SPIIF fromFile(
        const casacore::String& filename, casacore::Float, casacore::Bool cache=casacore::True
    );

    static SPIID fromFile(
        const casacore::String& filename, casacore::Double, casacore::Bool cache=casacore::True
    );

    static SPIIC fromFile(
        const casacore::String& filename, casacore::Complex, casacore::Bool cache=casacore::True
    );

    static SPIIDC fromFile(
        const casacore::String& filename, casacore::DComplex, casacore::Bool cache=casacore::True
    ); 

    static SPIIF fromFITS(
        const casacore::String& outfile, const casacore::String& fitsfile,
        const casacore::Int whichrep, const casacore::Int whichhdu,
        const casacore::Bool zeroBlanks, const casacore::Bool overwrite
    );

    static std::pair<SPIIF, SPIIC> fromRecord(
        const casacore::RecordInterface& rec, const casacore::String& imagename=""
    );

    template <class T> static void remove(SPIIT& image, casacore::Bool verbose);

    // if successful, image will be reset to point to new image upon return
    static void rename(
    	SPIIF& image, const casacore::String& name, const casacore::Bool overwrite
    );

    static void rename(
    	SPIIC& image, const casacore::String& name, const casacore::Bool overwrite
    );

    // open a canonical image
    static SPIIF testImage(
        const casacore::String& outfile, const casacore::Bool overwrite,
        const casacore::String& imagetype="2d"
    );
/*
    static void toASCII(
    	SPCIIF image,
    	const casacore::String& outfile, casacore::Record& region, const casacore::String& mask,
        const casacore::String& sep=" ", const casacore::String& format="%e",
        casacore::Double maskvalue=-999, casacore::Bool overwrite=false,
        casacore::Bool extendMask=false
    );
*/
    static void toFITS(
    	SPCIIF image, const casacore::String& outfile, casacore::Bool velocity,
		casacore::Bool optical, casacore::Int bitpix, casacore::Double minpix, casacore::Double maxpix,
		const casacore::Record& region, const casacore::String& mask,
		casacore::Bool overwrite=false, casacore::Bool dropdeg=false, casacore::Bool deglast=false,
		casacore::Bool dropstokes=false, casacore::Bool stokeslast=false,
		casacore::Bool wavelength=false, casacore::Bool airWavelength=false,
		const casacore::String& origin="", casacore::Bool stretch=false, casacore::Bool history=true
    );

private:

	template <class T> static SPIIT _fromRecord(
	    const casacore::RecordInterface& rec, const casacore::String& name
	);

	static void _centerRefPix(
		casacore::CoordinateSystem& csys, const casacore::IPosition& shape
	);

	static void _checkInfile(const casacore::String& infile);

    // Convert a casacore::Record to a CoordinateSystem
    static casacore::CoordinateSystem* _makeCoordinateSystem(
        const casacore::Record& cSys,
        const casacore::IPosition& shape
    );

    static void _checkOutfile(const casacore::String& outfile, casacore::Bool overwrite);

    static ITUPLE _fromLatticeBase(
        std::unique_ptr<casacore::LatticeBase>& latt
    );

    static casacore::String _imageCreationMessage(
        const casacore::String& outfile, const ITUPLE& imagePtrs
    );

    static casacore::String _imageCreationMessage(
        const casacore::String& outfile, const casacore::IPosition& shape,
        casacore::DataType dataType
    );

    // if successful, image will point to the newly named image
    // upone return
    template <class T> static ITUPLE _rename(
    	SPIIT& image, const casacore::String& name,
    	const casacore::Bool overwrite
    );

};
}

#ifndef AIPS_NO_TEMPLATE_SRC
#include <imageanalysis/ImageAnalysis/ImageFactory.tcc>
#endif

#endif
