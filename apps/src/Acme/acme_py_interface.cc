#include <pybind11/pybind11.h>
#include "acme.h"

namespace py = pybind11;

PYBIND11_MODULE(acme2py, m) {
    m.def("acme_func", &acme_func, py::arg("imageName")="", py::arg("deconvolver")="hogbom", py::arg("normtype")="flatnoise", py::arg("workdir")="", py::arg("imType")="psf", py::arg("pblimit")=0.05, py::arg("nterms")=1, py::arg("facets")=1, py::arg("psfcutoff")=0.35, py::arg("restoringbeam"), py::arg("computePB")=False);
}
