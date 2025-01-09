#include <pybind11/pybind11.h>
#include "dale.h"

using namespace Dale;

namespace py = pybind11;

PYBIND11_MODULE(dale2py, m) {
    m.def("dale",
	  &Dale::dale,
	  py::arg("imageName")="",
	  py::arg("wtimageName")="",
	  py::arg("sowimageName")="",
	  py::arg("normtype")="flatnoise",
	  py::arg("imType")="psf",
	  py::arg("pblimit")=0.2,
	  py::arg("psfcutoff")=0.35,
	  py::arg("computePB")=False,
	  py::arg("normalize_weight")=True);
}
