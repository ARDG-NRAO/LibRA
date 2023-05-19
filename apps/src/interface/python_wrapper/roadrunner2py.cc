#include <pybind11/pybind11.h>
#include <string>
#include <iostream>
#include <RoadRunner/Roadrunner_func.h>

using namespace pybind11;
using namespace std;

PYBIND11_MODULE(roadrunner2py, m)
{


  // Using the simplest helper function to translate AipsError exceptions (from C++) to
  // PyAipsError (in Python).
  pybind11::register_exception<AipsError>(m, "PyAipsError");

  m.doc() = "pybind11-based roadrunner python plugin"; // optional module docstring
  std::vector<float> posigdev_def(2); posigdev_def[0]=posigdev_def[1]=300.0;
  m.def("Roadrunner", &Roadrunner, "A task-level interface for the imaging derivative component",
	"msnbuf"_a, "imagename"_a,
	"modelimagename"_a="",
	"datacolumnname"_a="data",
	"sowimageext"_a="",
	"cmplxgridname"_a="",
	"imsize"_a,
	"nw"_a=1,
	"cellsize"_a,
	"stokes"_a="I",
	"reffreqstr"_a="3GHz",
	"phasecenter"_a,
	"weighting"_a="natural",
	"rmode"_a="",
	"robust"_a=0.0,
	"ftmname"_a="awphpg",
	"cfcache"_a="",
	"imagingmode"_a="residual",
	"wbawp"_a=true,
	"fieldstr"_a="*",
	"spwstr"_a="*",
	"uvdiststr"_a="",
	"dopointing"_a=true,
	"normalize"_a=true,
	"dopbcorr"_a=true,
	"conjbeams"_a=true,
	"pblimit"_a=0.2,
	"posigdev"_a=posigdev_def,
	"dospwdataiter"_a=true);
}
