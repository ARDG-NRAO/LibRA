#include <pybind11/pybind11.h>
#include <subms.h>

namespace py = pybind11;

PYBIND11_MODULE(subms2py, m) {
    m.def("SubMS_func", &SubMS_func, 
          py::arg("MSNBuf")="", 
          py::arg("OutMSBuf")="",
          py::arg("WhichColStr")="data", 
          py::arg("deepCopy")=false,
          py::arg("fieldStr")="*", 
          py::arg("timeStr")="",
          py::arg("spwStr")="*", 
          py::arg("baselineStr")="",
          py::arg("scanStr")="", 
          py::arg("arrayStr")="",
          py::arg("uvdistStr")="", 
          py::arg("taqlStr")="",
          py::arg("integ")=-1);
}
