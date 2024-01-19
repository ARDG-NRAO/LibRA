#include <pybind11/pybind11.h>
#include "acme.h"

namespace py = pybind11;

PYBIND11_MODULE(acme2py, m) {
    m.def("acme_func", &acme_func, py::arg("MSNBuf")="", py::arg("OutBuf")="", py::arg("verbose")=false);
}
