// # Copyright (C) 2021
// # Associated Universities, Inc. Washington DC, USA.
// #
// # This library is free software; you can redistribute it and/or modify it
// # under the terms of the GNU Library General Public License as published by
// # the Free Software Foundation; either version 2 of the License, or (at your
// # option) any later version.
// #
// # This library is distributed in the hope that it will be useful, but WITHOUT
// # ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// # FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
// # License for more details.is
// #
// # You should have received a copy of the GNU Library General Public License
// # along with this library; if not, write to the Free Software Foundation,
// # Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
// #
// # Correspondence concerning this should be addressed as follows:
// #        Postal address: National Radio Astronomy Observatory
// #                        1003 Lopezville Road,
// #                        Socorro, NM - 87801, USA
// #
// # $Id$
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

