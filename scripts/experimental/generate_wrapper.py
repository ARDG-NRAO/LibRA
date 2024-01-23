import re

def generate_pybind11_wrapper(signatures):
    # Initialize the Pybind11 wrapper code
    wrapper_code = """
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
"""

    for signature in signatures:
        # Extract the function name and the argument list from the signature
        func_name = re.search(r'\b\w+\b(?=\()', signature).group(0)
        arg_list = re.search(r'\((.*?)\)', signature).group(1).split(',')

        # Add the function header to the Pybind11 wrapper code
        wrapper_code += f"""
#include "{func_name}.h"

namespace py = pybind11;

PYBIND11_MODULE(lib{func_name}, m) {{
    m.def("{func_name}", &{func_name}, 
"""

        # Add the arguments to the Pybind11 wrapper code
        for arg in arg_list:
            arg_name = re.search(r'\b\w+\b$', arg).group(0)
            wrapper_code += f'        py::arg("{arg_name}"),\n'

        wrapper_code += "    );\n}"

    # Write the Pybind11 wrapper code to a file
    with open('py_interface.cc', 'w') as f:
        f.write(wrapper_code)

# Test the function with a list of signatures
generate_pybind11_wrapper([
    "void Roadrunner(string& MSNBuf, string& imageName, string& modelImageName, string& dataColumnName, string& sowImageExt, string& cmplxGridName, int& NX, int& nW, float& cellSize, string& stokes, string& refFreqStr, string& phaseCenter, string& weighting, string& rmode,  float& robust, string& ftmName, string& cfCache, string& imagingMode, bool& WBAwp, string& fieldStr, string& spwStr, string& uvDistStr, bool& doPointing, bool& normalize, bool& doPBCorr, bool& conjBeams, float& pbLimit, vector<float>& posigdev, bool& doSPWDataIter)",
    "void Coyote(string &MSNBuf,  &telescopeName, &NX, float &cellSize, &stokes, string &refFreqStr, int &nW, &cfCacheName,& imageNamePrefix, &WBAwp, bool &psTerm, bool aTerm, string &mType,& pa, float& dpa, &fieldStr, string &spwStr, string &phaseCenter, &conjBeams,   &cfBufferSize, int &cfOversampling,::vector<std::string>& cfList,& mode)",
    "float Hummbee(std::string& imageName, std::string& modelImageName,::string& deconvolver,::vector<float>& scales,& largestscale, float& fusedthreshold,& nterms,& gain, float& threshold,& nsigma,& cycleniter, float& cyclefactor,::vector<std::string>& mask, std::string& specmode)",
    ""
])
