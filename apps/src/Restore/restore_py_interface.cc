#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <iostream>
#include <restore.h>

using namespace std;
namespace py = pybind11;

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
 
    m.def("Restore", &Restore<float>, "A python interface for the restore API", 
        py::arg("model"), 
        py::arg("residual"), 
        py::arg("image"),
        py::arg("size_x"), py::arg("size_y"), 
        py::arg("refi"), py::arg("refj"),
        py::arg("inci"), py::arg("incj"), 
        py::arg("majaxis"), py::arg("minaxis"),
        py::arg("pa"), 
        py::arg("pbcor") = false, 
        py::arg("pb") = nullptr,
        py::arg("image_pbcor") = nullptr);

  
}

