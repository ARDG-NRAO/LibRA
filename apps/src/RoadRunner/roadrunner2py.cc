#include <pybind11/pybind11.h>
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
          "msnbuf"_a, "imagename"_a, "modelimagename"_a, "datacolumnname"_a, "sowimageext"_a,
          "cmplxgridname"_a, "imsize"_a, "nw"_a, "cellsize"_a, "stokes"_a, "reffreqstr"_a,
          "phasecenter"_a, "weighting"_a, "rmode"_a, "robust"_a, "ftmname"_a, "cfcache"_a,
          "imagingmode"_a, "wbawp"_a, "fieldstr"_a, "spwstr"_a, "uvdiststr"_a, "dopointing"_a,
          "normalize"_a, "dopbcorr"_a, "conjbeams"_a, "pblimit"_a, "posigdev"_a, "dospwdataiter"_a);
}
