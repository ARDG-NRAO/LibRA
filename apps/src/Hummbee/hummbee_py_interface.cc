#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <iostream>
#include <hummbee.h>

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
    "waveletscales"_a,
    "waveletamps"_a,
    "wavelettrigger"_a=false,
    "mfasp"_a=false,
    "autothreshold"_a=0,
    "automaxiter"_a=0,
    "autogain"_a=0,
    "autohogbom"_a=false,
    "autotrigger"_a=0,
    "autopower"_a=1,
    "autoxmask"_a=0,
    "autoymask"_a=0,
    "lbfgsepsf"_a=0.001,
    "lbfgsepsx"_a=0.001,
    "lbfgsepsg"_a=0.001,
    "lbfgsmaxit"_a=5,
    "nterms"_a=1,
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
   
}

