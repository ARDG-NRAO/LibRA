#include <pybind11/pybind11.h>
#include <string>
#include <iostream>
#include <Hummbee/hummbee_func.h>

using namespace pybind11;
using namespace std;

PYBIND11_MODULE(hummbee2py, m)
{
    // Using the simplest helper function to translate AipsError exceptions (from C++) to
    // PyAipsError (in Python).
    pybind11::register_exception<AipsError>(m, "PyAipsError");

    m.doc() = "pybind11-based hummbee python plugin"; // optional module docstring
    m.def("Hummbee", &Hummbee, "A task-level interface for the model update component",
    "msnbuf"_a, "imagename"_a,
    "modelimagename"_a="",
    "imsize"_a,", nw"_a=1,
    "cellsize"_a,
    "stokes"_a="I",
    "reffreqstr"_a="3GHz",
    "phasecenter"_a,
    "weighting"_a="natural",
    "robust"_a=0.0,
    "cfcache"_a="",
    "fieldstr"_a="*",
    "spwstr"_a="*",
    "dopbcorr"_a=true,
    "conjbeams"_a=true,
    "pblimit"_a=0.2,
    "deconvolver"_a="hogbom",
    "scales"_a,
    "largestscale"_a=0,
    "fusedthreshold"_a=0,
    "nterms"_a=1,
    "gain"_a=0.1,
    "threshold"_a=0.0,
    "nsigma"_a=0.0,
    "cycleniter"_a=1000,
    "cyclefactor"_a=1.5,
    "mask"_a=""
    );
}
