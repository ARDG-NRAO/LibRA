#include <pybind11/pybind11.h>
#include "dale.h"

using namespace Dale;

namespace py = pybind11;

PYBIND11_MODULE(dale2py, m) {
    m.def("dale", &Dale::dale, py::arg("imageName")="", py::arg("deconvolver")="hogbom", py::arg("normtype")="flatnoise", py::arg("imType")="psf", py::arg("pblimit")=0.2, py::arg("nterms")=1, py::arg("facets")=1, py::arg("psfcutoff")=0.35, py::arg("restoringbeam"), py::arg("computePB")=False, py::arg("normalize_weight")=True);
}
