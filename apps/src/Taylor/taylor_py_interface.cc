// # Copyright (C) 2021,2026
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

// Python bindings for the Taylor app.
//
// The other libra modules bind a single free function because those apps ARE a
// single free function. Taylor is a class whose configuration (reffreq, nterms,
// pblimit, imtype, overwrite) is shared across several operations, so it is
// bound as a class: construct once with the configuration, then call the
// operations with image names.
//
// Every method returns None. Failure arrives as an exception carrying the
// message LogIO::EXCEPTION produced.

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <taylor.h>

#include <casacore/casa/Exceptions/Error.h>

namespace py = pybind11;
using taylor::ImageType;
using taylor::Taylor;

PYBIND11_MODULE(taylor2py, m)
{
    m.doc() = "Taylor-term conversions for the mtmfs-via-cube pipeline.";

    // AipsError derives from std::exception, so pybind11 would already map it
    // to RuntimeError, but via what() rather than getMesg(). Translating it
    // explicitly keeps the casacore message intact and readable.
    static py::exception<casacore::AipsError> aipsError(m, "AipsError");
    py::register_exception_translator([](std::exception_ptr p) {
        try
        {
            if (p)
            {
                std::rethrow_exception(p);
            }
        }
        catch (const casacore::AipsError &e)
        {
            aipsError(e.getMesg().c_str());
        }
    });

    py::enum_<ImageType>(m, "ImageType")
        .value("PSF", taylor::PSF)
        .value("RESIDUAL", taylor::RESIDUAL)
        .value("PB", taylor::PB)
        .value("SUMWT", taylor::SUMWT)
        .value("MODEL", taylor::MODEL)
        .value("INVALID", taylor::INVALID)
        .export_values();

    py::class_<Taylor>(m, "Taylor")
        .def(py::init<const std::string &, int, float, ImageType, bool>(),
             py::arg("reffreq") = "",
             py::arg("nterms") = 0,
             py::arg("pblimit") = 0.0f,
             py::arg("imtype") = taylor::INVALID,
             py::arg("overwrite") = false,
             "Configuration shared by every operation on this object.")

        .def("cube_to_taylor_sum", &Taylor::cubeToTaylorSum,
             py::arg("cubenames"), py::arg("mtnames"), py::arg("pbnames"),
             py::arg("sumwtnames"),
             "Way In. mtnames[0] is a PREFIX: terms are written as "
             "<prefix>.tt0 ... <prefix>.ttN. pbnames must hold one per-SPW PB "
             "per cube image, in the same order, with the avgPB appended last.")

        .def("taylor_to_cube", &Taylor::taylorToCube,
             py::arg("mtnames"), py::arg("cubenames"), py::arg("pbnames"),
             "Way Out. A non-empty pbnames divides the result by pbnames[0], "
             "removing the avgPB baked in on the Way In so the model leaves as "
             "true sky.")

        .def("compute_alpha", &Taylor::computeAlpha,
             py::arg("mtnames"), py::arg("alphaimages"),
             "Spectral index from the Taylor terms. mtnames[0] is a prefix. "
             "With nterms > 2 a second output name is required for beta.")

        .def("compute_avg_pb", &Taylor::computeAvgPB,
             py::arg("pbnames"), py::arg("avgpbname"),
             py::arg("minfreqpbname") = "", py::arg("avgpbmode") = "mean",
             "Average the per-SPW PBs. avgpbmode is 'mean' or 'median'.")

        .def("apply_pb", &Taylor::applyPB,
             py::arg("pbname"), py::arg("cubenames"), py::arg("mode"),
             "Multiply or divide each cube by pbname, in place. mode is "
             "'multiply' or 'divide'.")

        .def("remove_freq_dep_pb", &Taylor::removeFreqDepPB,
             py::arg("avgpbname"), py::arg("cubenames"), py::arg("pbnames"),
             "Way-In flattening: replace each per-SPW beam with the avgPB, "
             "in place, leaving the sky spectrum untouched.")

        .def("apply_freq_dep_pb", &Taylor::applyFreqDepPB,
             py::arg("avgpbname"), py::arg("cubenames"), py::arg("pbnames"),
             "The inverse of remove_freq_dep_pb.")

        .def_property_readonly("nterms", &Taylor::nterms)
        .def_property_readonly("pblimit", &Taylor::pblimit)
        .def_property_readonly("imtype", &Taylor::imtype);
}
