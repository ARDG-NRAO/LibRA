 //# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#
//# $Id$

#ifndef CONTACT_HUMMBEE_FUNC_H
#define CONTACT_HUMMBEE_FUNC_H

#include <casacore/casa/namespace.h>
#include <casacore/casa/Logging/LogIO.h>

#include <casacore/casa/aips.h>
#include <casacore/casa/OS/Timer.h>
#include <casacore/casa/Containers/Record.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/casa/Arrays/IPosition.h>
#include <casacore/casa/Quanta/Quantum.h>
#include <casacore/measures/Measures/MDirection.h>

#include <synthesis/ImagerObjects/SDAlgorithmBase.h>
#include <synthesis/ImagerObjects/SDAlgorithmHogbomClean.h>
#include <synthesis/ImagerObjects/SDAlgorithmClarkClean.h>
#include <synthesis/ImagerObjects/SDAlgorithmClarkClean2.h>
#include <synthesis/ImagerObjects/SDAlgorithmMSMFS.h>
#include <synthesis/ImagerObjects/SDAlgorithmMSClean.h>
#include <synthesis/ImagerObjects/SDAlgorithmMEM.h>
#include <synthesis/ImagerObjects/SDAlgorithmAAspClean.h>

#include <synthesis/ImagerObjects/SDMaskHandler.h>
#include <synthesis/ImagerObjects/SIMinorCycleController.h>

#include <synthesis/ImagerObjects/SynthesisUtilMethods.h>

#include <synthesis/ImagerObjects/SynthesisDeconvolver.h>
#include <synthesis/ImagerObjects/grpcInteractiveClean.h>

using namespace casa;
using namespace casacore;

/**
 * @brief Runs the hummbee deconvolution algorithm.
 *
 * This function runs the hummbee deconvolution algorithm on the specified image and model image names, using the specified deconvolver and deconvolution parameters.
 *
 * @param imageName The name of the input image.
 * @param modelImageName The name of the model image.
 * @param deconvolver The name of the deconvolver algorithm to use.
 * @param scales A vector of scales to use for multi-scale deconvolution.
 * @param largestscale The largest scale to use for multi-scale deconvolution.
 * @param fusedthreshold The threshold to use for multi-scale deconvolution.
 * @param nterms The number of Taylor terms to use for multi-term deconvolution.
 * @param gain The gain to use for the deconvolution algorithm.
 * @param threshold The threshold to use for the deconvolution algorithm.
 * @param nsigma The number of sigma to use for the deconvolution algorithm.
 * @param cycleniter The number of iterations to use for the deconvolution algorithm.
 * @param cyclefactor The cycle factor to use for the deconvolution algorithm.
 * @param mask A vector of masks to use for the deconvolution algorithm.
 */
float Hummbee(std::string& imageName, std::string& modelImageName,
        std::string& deconvolver,
        std::vector<float>& scales,
        float& largestscale, float& fusedthreshold,
        int& nterms,
        float& gain, float& threshold,
        float& nsigma,
        int& cycleniter, float& cyclefactor,
        std::vector<std::string>& mask, std::string& specmode);


#endif
