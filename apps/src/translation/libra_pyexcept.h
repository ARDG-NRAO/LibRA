// # Copyright (C) 2026
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
// # License for more details.
// #
// # You should have received a copy of the GNU Library General Public License
// # along with this library; if not, write to the Free Software Foundation,
// # Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
// #
// # Correspondence concerning this should be addressed as follows:
// #        Postal address: National Radio Astronomy Observatory
// #                        1003 Lopezville Road,
// #                        Socorro, NM - 87801, USA
//
/**
 * @file libra_pyexcept.h
 * @brief register_libra_exceptions(m) re-exports the shared LibRA exception
 * types from libraexcept2py into a binding module.
 */
#ifndef LIBRA_TRANSLATION_LIBRA_PYEXCEPT_H
#define LIBRA_TRANSLATION_LIBRA_PYEXCEPT_H

#include <pybind11/pybind11.h>

inline void register_libra_exceptions(pybind11::module_ &m)
{
  pybind11::module_ ex = pybind11::module_::import("libraexcept2py");

  for (const char *name : {"PyAipsError",
                           "AipsError",
                           "SynthesisError",
                           "SynthesisFTMachineError",
                           "PSFZero",
                           "CFNotCached",
                           "CFSupportZero",
                           "CFCIsEmpty"})
    {
      m.attr(name) = ex.attr(name);
    }
}

#endif // LIBRA_TRANSLATION_LIBRA_PYEXCEPT_H
