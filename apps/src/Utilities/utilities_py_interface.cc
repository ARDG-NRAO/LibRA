// # Copyright (C) 2021, 2026
// # Associated Universities, Inc. Washington DC, USA.
// #
// # This library is free software; you can redistribute it and/or modify it
// # under the terms of the GNU Library General Public License as published by
// # the Free Software Foundation; either version 2 of the License, or (at your
// # option) any later version.
// #
// # This library is distributed in the hope that it will be useful, but WITHOUT
// # ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// # FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
// # License for more details.is
// #
// # You should have received a copy of the GNU Library General Public License
// # along with this library; if not, write to the Free Software Foundation,
// # Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
// #
// # Correspondence concerning this should be addressed as follows:
// #        Postal address: National Radio Astronomy Observatory
// #                        1003 Lopezville Road,
// #                        Socorro, NM - 87801, USA
// #
// # $Id$
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

#include <mdspan/mdspan.hpp>
#include <casacore/casa/namespace.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogSink.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <synthesis/ImagerObjects/SIImageStore.h>
#include <casacore/images/Images/PagedImage.h>

#include <casacore/scimath/Fitting/NonLinearFitLM.h>
#include <casacore/scimath/Functionals/Gaussian2D.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <librautils/utils.h>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>


using namespace std;
namespace py = pybind11;


enum ImageType {
    PSF,
    RESIDUAL,
    MODEL,
    MASK,
    PB,
    IMAGE
};


// Helper function to flatten a 4D array
vector<float> flatten4DArray(const casacore::Array<casacore::Float>& arr) 
{
    const auto shape = arr.shape();
    size_t nx = static_cast<size_t>(shape(0));
    size_t ny = static_cast<size_t>(shape(1));
    size_t np = static_cast<size_t>(shape(2));
    size_t nc = static_cast<size_t>(shape(3));

    vector<float> flattened;
    flattened.reserve(nx * ny * np * nc);

    for (size_t k = 0; k < nc; ++k) {
        for (size_t l = 0; l < np; ++l) {
            for (size_t j = 0; j < ny; ++j) {
                for (size_t i = 0; i < nx; ++i) {
                    flattened.push_back(arr(IPosition(4, j, i, l, k)));
                }
            }
        }
    }

    return flattened;
}


py::array_t<float> getchunk(const string& imageName, ImageType type) 
{
    shared_ptr<casa::SIImageStore> itsImages;
    itsImages.reset(new casa::SIImageStore(imageName, true, true));

    casacore::Array<casacore::Float> itsPsf, itsResidual, itsModel, itsMask, itsMatPB, itsMatImage;
    itsImages->residual()->get(itsResidual, false);
    itsImages->model()->get(itsModel, false);
    itsImages->psf()->get(itsPsf, false);
    itsImages->mask()->get(itsMask, false);
    itsImages->pb()->get( itsMatPB, false);
    itsImages->image()->get( itsMatImage, false);

    auto shape = itsPsf.shape();
    size_t nx = static_cast<size_t>(shape(0));
    size_t ny = static_cast<size_t>(shape(1));
    size_t np = static_cast<size_t>(shape(2));
    size_t nc = static_cast<size_t>(shape(3));

    // Flatten the appropriate array based on the type
    vector<float> flattened;
    switch (type) {
        case ImageType::PSF:
            flattened = flatten4DArray(itsPsf);
            break;
        case ImageType::RESIDUAL:
            flattened = flatten4DArray(itsResidual);
            break;
        case ImageType::MODEL:
            flattened = flatten4DArray(itsModel);
            break;
        case ImageType::MASK:
            flattened = flatten4DArray(itsMask);
            break;
        case ImageType::PB:
            flattened = flatten4DArray(itsMatPB);
            break;
        case ImageType::IMAGE:
            flattened = flatten4DArray(itsMatImage);
            break;
        default:
            throw std::invalid_argument("Invalid image type");
    }

    // Create a 4-dimension NumPy array
    py::array_t<float> result({ny, nx, np, nc}, flattened.data());
    return result;
}

// Open any casa image directly by path (works for multi-term .tt0/.tt1
// images, which SIImageStore-based getchunk cannot open). Returns the pixel
// array with casacore's native axis order (x, y, pol, chan), matching
// casatools' ia.getchunk().

template <class T>
py::array_t<T> getchunkFromPath(const string& imagePath)
{
    casacore::Array<T> arr = librautils::getChunkFromPath<T>(imagePath);

    const auto shape = arr.shape();
    casacore::Bool deleteIt;
    const T* data = arr.getStorage(deleteIt);

    std::vector<py::ssize_t> dims, strides;
    py::ssize_t stride = sizeof(T);
    for (size_t i = 0; i < shape.nelements(); ++i) {
        dims.push_back(static_cast<py::ssize_t>(shape(i)));
        strides.push_back(stride);
        stride *= shape(i);
    }
    py::array_t<T> result(dims, strides, data);  // copies
    arr.freeStorage(data, deleteIt);
    return result;
}

// The four getchunkFromPath<T>() instantiations differ only in their return
// type, so neither C++ overloading nor pybind11's argument-based overload
// resolution can pick between them.  Dispatch instead on the pixel type
// recorded in the image on disk, and hand Python back a NumPy array whose
// dtype matches it.

py::object getchunkFromPathAuto(const string& imagePath)
{
    switch (casacore::imagePixelType(imagePath)) {
        case casacore::TpFloat:
            return getchunkFromPath<float>(imagePath);
        case casacore::TpDouble:
            return getchunkFromPath<double>(imagePath);
        case casacore::TpComplex:
            return getchunkFromPath<casacore::Complex>(imagePath);
        case casacore::TpDComplex:
            return getchunkFromPath<casacore::DComplex>(imagePath);
        default:
            throw std::invalid_argument("Unsupported pixel type in image "
                                        + imagePath);
    }
}

// Write-back counterpart to getchunkFromPath: overwrite the pixels of an
// existing casa image on disk with the contents of a NumPy array in place.
// The array must already carry the on-disk pixel type and getchunkFromPath's
// (x, y, pol, chan) axis order -- a mismatch is a caller bug and is reported
// as such rather than silently reshaped or cast. The actual write, and the
// writability/shape checks, are in librautils::putChunkFromPath; 

template <class T>
void putChunkFromPath(const string& imagePath,
    py::array_t<T, py::array::f_style | py::array::forcecast> array)
{
    // f_style | forcecast makes pybind11 hand us an array already laid out
    // the way casacore's constructor below expects. Pybind11 will do the copying if the
    // caller's array wasn't already that shape in memory, instead of it.
    // can potentially lead to overflow for very large images.
    py::buffer_info buf = array.request();

    casacore::IPosition shape(buf.ndim);
    for (int i = 0; i < buf.ndim; ++i)
        shape(i) = buf.shape[i];

    casacore::Array<T> arr(shape, static_cast<T*>(buf.ptr), casacore::SHARE);
    librautils::putChunkFromPath<T>(imagePath, arr);
}

py::object putChunkFromPathAuto(const string& imagePath, py::array array)
{
    switch (casacore::imagePixelType(imagePath)) {
        case casacore::TpFloat:
            if (!array.dtype().is(py::dtype::of<float>()))
                throw std::invalid_argument(
                    "putchunkfrompath: image " + imagePath
                    + " is float32, array is not");
            putChunkFromPath<float>(imagePath, array);
            break;
        case casacore::TpDouble:
            if (!array.dtype().is(py::dtype::of<double>()))
                throw std::invalid_argument(
                    "putchunkfrompath: image " + imagePath
                    + " is float64, array is not");
            putChunkFromPath<double>(imagePath, array);
            break;
        case casacore::TpComplex:
            if (!array.dtype().is(py::dtype::of<std::complex<float>>()))
                throw std::invalid_argument(
                    "putchunkfrompath: image " + imagePath
                    + " is complex64, array is not");
            putChunkFromPath<casacore::Complex>(imagePath, array);
            break;
        case casacore::TpDComplex:
            if (!array.dtype().is(py::dtype::of<std::complex<double>>()))
                throw std::invalid_argument(
                    "putchunkfrompath: image " + imagePath
                    + " is complex128, array is not");
            putChunkFromPath<casacore::DComplex>(imagePath, array);
            break;
        default:
            throw std::invalid_argument("Unsupported pixel type in image "
                                        + imagePath);
    }
    return py::none();
}

// Binding code
PYBIND11_MODULE(utilities2py, m) {
  py::enum_<ImageType>(m, "ImageType")
    .value("PSF", PSF)
    .value("RESIDUAL", RESIDUAL)
    .value("MODEL", MODEL)
    .value("MASK", MASK)
    .value("PB", PB)
    .value("IMAGE", IMAGE)
    .export_values();

  m.def("getchunk", &getchunk, "Retrieve a chunk of casa image",
        py::arg("imageName"), py::arg("type"));

  m.def("getchunkfrompath", &getchunkFromPathAuto,
        "Retrieve the pixel array of any casa image by path (axis order "
        "x, y, pol, chan).  The dtype of the returned array follows the "
        "pixel type of the image on disk (float32, float64, complex64 or "
        "complex128).",
        py::arg("imagePath"));

  m.def("putchunkfrompath", &putChunkFromPathAuto,
        "Overwrite the pixels of an existing casa image on disk with a "
        "NumPy array in place (axis order x, y, pol, chan).  The array's "
        "dtype and shape must already match the image; use "
        "getchunkfrompath's output as the template.",
        py::arg("imagePath"), py::arg("array"));
}
