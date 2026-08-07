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
/**
 * @file coyote2py.cc
 * @brief This file contains the Pybind11 wrapper for the Coyote function.
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <coyote.h>

namespace py = pybind11;
using namespace py::literals;  // to bring in the "_a" literal

/**
 * @brief This function is a Pybind11 wrapper for the Coyote function.
 * 
 * @param MSNBuf A string parameter.
 * @param telescopeName A string parameter.
 * @param NX An integer parameter.
 * @param cellSize A double parameter.
 * @param stokes A string parameter representing input stokes.
 * @param refFreqStr A string parameter.
 * @param nW An integer parameter.
 * @param cfCacheName A string parameter.
 * @param WBAwp A boolean parameter.
 * @param psTerm A boolean parameter.
 * @param aTerm A boolean parameter.
 * @param mType A string parameter.
 * @param pa A double parameter.
 * @param dpa A double parameter.
 * @param fieldStr A string parameter.
 * @param spwStr A string parameter.
 * @param phaseCenter A string parameter.
 * @param conjBeams A boolean parameter.
 * @param cfBufferSize An integer parameter.
 * @param cfOversampling An integer parameter.
 * @param cfList A vector of strings parameter.
 * @param mode A string parameter.
 */
PYBIND11_MODULE(coyote2py, m) {
    m.doc() = "pybind11-based roadrunner python plugin"; // optional module docstring
    std::vector<string> cfList(1); cfList[0] = "";;  
    m.def("Coyote", &Coyote, "Coyote app pybind11 wrapper"
        "MSNBuf"_a,  // std::string
        "telescopeName"_a,  // std::string
        "NX"_a,  // int
        "cellSize"_a,  // double
        "stokes"_a,  // std::string
        "refFreqStr"_a,  // std::string
        "nW"_a,  // int
        "cfCacheName"_a,  // std::string
        "WBAwp"_a,  // bool
        "psTerm"_a,  // bool
        "aTerm"_a,  // bool
        "mType"_a,  // std::string
        "pa"_a,  // double
        "dpa"_a,  // double
        "fieldStr"_a,  // std::string
        "spwStr"_a,  // std::string
        "phaseCenter"_a,  // std::string
        "conjBeams"_a,  // bool
        "cfBufferSize"_a,  // int
        "cfOversampling"_a,  // int
        "cfList"_a=cfList ,  // std::vector<std::string>
        "mode"_a=""  // std::string
        );
}
