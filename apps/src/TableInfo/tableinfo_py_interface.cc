#include <pybind11/pybind11.h>
#include <tableinfo.h>

namespace py = pybind11;

PYBIND11_MODULE(tableinfo2py, m) {
    m.def("TableInfo_func", &TableInfo_func, py::arg("MSNBuf")="", py::arg("OutBuf")="", py::arg("verbose")=false);
}
