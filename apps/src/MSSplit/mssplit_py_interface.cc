#include <pybind11/pybind11.h>
#include <mssplit.h>

namespace py = pybind11;

PYBIND11_MODULE(mssplit2py, m) {
    m.def("MSSplit_func", &MSSplit_func, 
          py::arg("MSNBuf")="", 
          py::arg("OutMSBuf")="",
          py::arg("deepCopy")=false, 
          py::arg("fieldStr")="",
          py::arg("timeStr")="", 
          py::arg("spwStr")="",
          py::arg("baselineStr")="", 
          py::arg("uvdistStr")="",
          py::arg("taqlStr")="", 
          py::arg("scanStr")="",
          py::arg("arrayStr")="", 
          py::arg("polnStr")="",
          py::arg("stateObsModeStr")="",
          py::arg("observationStr")="");
}
