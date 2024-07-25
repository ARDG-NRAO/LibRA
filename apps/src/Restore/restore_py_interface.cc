#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <iostream>
#include <restore.h>

using namespace pybind11;
using namespace std;
/**
 * @file restore2py.cc
 * @brief pybind11-based restore python plugin
 */
/**
 * @brief This module provides a pybind11-based restore python plugin.
 *  
 * @param m The pybind11 module object.
 */
PYBIND11_MODULE(restore2py, m)
{
    // Using the simplest helper function to translate AipsError exceptions (from C++) to
    // PyAipsError (in Python).
    pybind11::register_exception<AipsError>(m, "PyAipsError");

    m.doc() = "pybind11-based restore python plugin"; // optional module docstring
    m.def("Restore", &Restore, "A python interface for the restore component",
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
    "specmode"_a="mfs",
    "pbcor"_a=false,
    "mode"_a="deconvolve"
    );

    m.def("add", &add, "A function which adds two numbers");   
}

