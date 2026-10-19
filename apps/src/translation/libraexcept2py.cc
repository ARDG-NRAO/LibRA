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
 * @file libraexcept2py.cc
 * @brief Python exception types for LibRA, shared by every binding module.
 *
 * Mirrors the C++ hierarchy: AipsError > SynthesisError >
 * SynthesisFTMachineError > PSFZero, with CFNotCached, CFSupportZero and
 * CFCIsEmpty under SynthesisError.  Binding modules re-export these through
 * libra_pyexcept.h.
 */
#include <pybind11/pybind11.h>

#include <casacore/casa/Exceptions/Error.h>
#include <synthesis/TransformMachines/SynthesisError.h>

#include <exception>

namespace py = pybind11;

PYBIND11_MODULE(libraexcept2py, m)
{
  m.doc() = "LibRA exception types, shared by every LibRA Python module";

  // Static so the translator below can use them.
  static py::exception<casacore::AipsError>
    exAips(m, "PyAipsError");
  static py::exception<casa::SynthesisError>
    exSynth(m, "SynthesisError", exAips.ptr());
  static py::exception<casa::SynthesisFTMachineError>
    exFTM(m, "SynthesisFTMachineError", exSynth.ptr());
  static py::exception<casa::PSFZero>
    exPSFZero(m, "PSFZero", exFTM.ptr());
  static py::exception<casa::CFNotCached>
    exCFNotCached(m, "CFNotCached", exSynth.ptr());
  static py::exception<casa::CFSupportZero>
    exCFSupportZero(m, "CFSupportZero", exSynth.ptr());
  static py::exception<casa::CFCIsEmpty>
    exCFCIsEmpty(m, "CFCIsEmpty", exSynth.ptr());

  // AipsError is an alias for PyAipsError.
  m.attr("AipsError") = m.attr("PyAipsError");

  // Most-derived first: the first matching catch wins.
  py::register_exception_translator(
    [](std::exception_ptr p)
    {
      try
        {
          if (p) std::rethrow_exception(p);
        }
      catch (const casa::PSFZero &e)          { exPSFZero(e.what()); }
      catch (const casa::SynthesisFTMachineError &e) { exFTM(e.what()); }
      catch (const casa::CFNotCached &e)      { exCFNotCached(e.what()); }
      catch (const casa::CFSupportZero &e)    { exCFSupportZero(e.what()); }
      catch (const casa::CFCIsEmpty &e)       { exCFCIsEmpty(e.what()); }
      catch (const casa::SynthesisError &e)   { exSynth(e.what()); }
      catch (const casacore::AipsError &e)    { exAips(e.what()); }
    });
}
