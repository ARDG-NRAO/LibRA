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
#include <dale.h>

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
	  //py::arg("psfcutoff")=0.35,
	  py::arg("computePB")=False);
    //py::arg("normalize_weight")=True);
}
