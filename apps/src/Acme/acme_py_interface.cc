#include <pybind11/pybind11.h>
#include <acme.h>

using namespace Acme;

namespace py = pybind11;

PYBIND11_MODULE(acme2py, m) {
    m.def("acme", &Acme::acme, py::arg("imageName")="", py::arg("stats")="checkamp");
}
