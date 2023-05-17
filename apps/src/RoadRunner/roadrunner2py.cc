#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <iostream>
#include <RoadRunner/Roadrunner_func.h>

using namespace pybind11;
using namespace std;

void test(std::string& name)
{
	cerr << name << endl;
}
PYBIND11_MODULE(roadrunner2py, m) {
    m.doc() = "pybind11 roadrunner2py plugin"; // optional module docstring

    
//    m.def("test",&test, "A test function", "test"_a);
    m.def("Roadrunner_py", &Roadrunner_py, "A function which takes a string and an int and returns their sum",
          "msnbuf"_a, "imagename"_a, "modelimagename"_a="", "datacolumnname"_a="data", "sowimageext"_a="",
          "cmplxgridname"_a="", "imsize"_a, "nw"_a=1, "cellsize"_a, "stokes"_a="I", "reffreqstr"_a="3GHz",
          "phasecenter"_a, "weighting"_a="natural", "rmode"_a="", "robust"_a=0.0, "ftmname"_a="awprojecthpg", "cfcache"_a="",
          "imagingmode"_a="predict", "wbawp"_a=true, "fieldstr"_a="*", "spwstr"_a="*", "uvdiststr"_a="", "dopointing"_a=true,
          "normalize"_a=true, "dopbcorr"_a=true, "conjbeams"_a=true, "pblimit"_a=0.2, "posigdev"_a={300.0,300.0}, "dospwdataiter"_a=true);
}
