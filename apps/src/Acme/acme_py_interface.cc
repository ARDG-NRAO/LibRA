#include <pybind11/pybind11.h>
#include "acme.h"

namespace py = pybind11;

PYBIND11_MODULE(acme2py, m) {
    m.def("acme_func", &acme_func, py::arg("imageName")="", py::arg("deconvolver")="hogbom", py::arg("normtype")="flatnoise", py::arg("workdir")="", py::arg("mode")="normalize", py::arg("imType")="psf", py::arg("pblimit")=0.2, py::arg("nterms")=1, py::arg("facets")=1, py::arg("psfcutoff")=0.35, py::arg("restoringbeam"), py::arg("partimagenames"), py::arg("resetimages")=False, py::arg("computePB")=False);
}
