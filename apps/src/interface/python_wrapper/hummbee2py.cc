#include <pybind11/pybind11.h>
#include <string>
#include <iostream>
#include <Hummbee/hummbee_func.h>

using namespace pybind11;
using namespace std;
/**
 * @file hummbee2py.cc
 * @brief pybind11-based hummbee python plugin
 */
/**
 * @brief This module provides a pybind11-based hummbee python plugin.
 * 
 * The module provides a task-level interface for the model update component.
 * It registers an exception handler for AipsError exceptions (from C++) to PyAipsError (in Python).
 * 
 * @param m The pybind11 module object.
 */
PYBIND11_MODULE(hummbee2py, m)
{
    // Using the simplest helper function to translate AipsError exceptions (from C++) to
    // PyAipsError (in Python).
    pybind11::register_exception<AipsError>(m, "PyAipsError");

    m.doc() = "pybind11-based hummbee python plugin"; // optional module docstring
    m.def("Hummbee", &Hummbee, "A task-level interface for the model update component",
    "imagename"_a,
    "modelimagename"_a="",
    "deconvolver"_a="hogbom",
    "scales"_a,
    "largestscale"_a=-1,
    "fusedthreshold"_a=0,
    "nterms"_a=2,
    "gain"_a=0.1,
    "threshold"_a=0.0,
    "nsigma"_a=0.0,
    "cycleniter"_a=-1,
    "cyclefactor"_a=1.0,
    "mask"_a="",
    "specmode"_a="mfs"
    );
}

