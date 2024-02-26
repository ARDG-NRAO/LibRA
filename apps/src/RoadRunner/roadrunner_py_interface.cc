#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <iostream>
#include <roadrunner.h>

using namespace pybind11;
using namespace std;

/**
 * @file roadrunner_py_interface.cc
 * @brief pybind11-based roadrunner python plugin
 */
/**
 * @brief This module provides a pybind11-based roadrunner python plugin.
 * 
 * The module defines a function "Roadrunner" which is a task-level interface for the imaging derivative component.
 * It also registers an exception handler for AipsError exceptions from C++ to PyAipsError in Python.
 * 
 * @param m A pybind11 module object.
 */
PYBIND11_MODULE(roadrunner2py, m)
{
  // Using the simplest helper function to translate AipsError exceptions (from C++) to
  // PyAipsError (in Python).
  pybind11::register_exception<AipsError>(m, "PyAipsError");

  m.doc() = "pybind11-based roadrunner python plugin"; // optional module docstring
  std::vector<float> posigdev_def(2); posigdev_def[0]=posigdev_def[1]=300.0;
  m.def("Roadrunner", &Roadrunner, "A task-level interface for the imaging derivative component",
	"vis"_a="",	"imagename"_a="",	"modelimagename"_a="",
	"datacolumn"_a="data",
	"sowimageext"_a="",	"complexgrid"_a="",
	"imsize"_a,	"wprojplanes"_a=1,	"cell"_a,
	"stokes"_a="I",	"reffreq"_a="3GHz",	"phasecenter"_a,
	"weighting"_a="natural", "rmode"_a="",	"robust"_a=0.0,
	"gridder"_a="awphpg",	"cfcache"_a="",	"mode"_a="residual",
	"wbawp"_a=true,
	"field"_a="*",	"spw"_a="*",	"uvrange"_a="",
	"usepointing"_a=false, "normalize"_a=false,	"pbcor"_a=true,
	"conjbeams"_a=true,	"pblimit"_a=0.2, "pointingoffsetsigdev"_a=posigdev_def,
	"spwdataiter"_a=true);
}
