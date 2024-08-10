// Copyright 2021 Associated Universities, Inc. Washington DC, USA.
// SPDX-License-Identifier: Apache-2.0
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef UTILITIES_FUNC_H
#define UTILITIES_FUNC_H

#include <iostream>
#include <vector>

#include <mdspan.hpp>
#include <casacore/casa/namespace.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogSink.h>
#include <casacore/casa/Arrays/Matrix.h>
#include<synthesis/ImagerObjects/SIImageStore.h>

#include <casacore/scimath/Fitting/NonLinearFitLM.h>
#include <casacore/scimath/Functionals/Gaussian2D.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

enum ImageType {
    PSF,
    RESIDUAL,
    MODEL,
    MASK
};


//vector<vector<float>> getchunk(const std::string& imageName, ImageType type); 
py::array_t<float> getchunk(const std::string& imageName, ImageType type); 

#endif
