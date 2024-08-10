#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

#include <mdspan.hpp>
#include <casacore/casa/namespace.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogSink.h>
#include <casacore/casa/Arrays/Matrix.h>
#include<synthesis/ImagerObjects/SIImageStore.h>

#include <casacore/scimath/Fitting/NonLinearFitLM.h>
#include <casacore/scimath/Functionals/Gaussian2D.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>


using namespace std;
namespace py = pybind11;


enum ImageType {
    PSF,
    RESIDUAL,
    MODEL,
    MASK
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

    casacore::Array<casacore::Float> itsPsf, itsResidual, itsModel, itsMask;
    itsImages->residual()->get(itsResidual, false);
    itsImages->model()->get(itsModel, false);
    itsImages->psf()->get(itsPsf, false);
    itsImages->mask()->get(itsMask, false);

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
        default:
            throw std::invalid_argument("Invalid image type");
    }

    // Create a 4-dimension NumPy array
    py::array_t<float> result({ny, nx, np, nc}, flattened.data());
    return result;
}


// Binding code
PYBIND11_MODULE(utilities2py, m) {
    py::enum_<ImageType>(m, "ImageType")
        .value("PSF", PSF)
        .value("RESIDUAL", RESIDUAL)
        .value("MODEL", MODEL)
        .value("MASK", MASK)
        .export_values();

    m.def("getchunk", &getchunk, "Retrieve a chunk of casa image",
          py::arg("imageName"), py::arg("type"));
}

