//# AspMatrixCleaner.h: Minor Cycle for Asp deconvolution
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
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id: AspMatrixCleaner.h Genie H. 2020-04-06 <mhsieh@nrao.edu $

#ifndef SYNTHESIS_WAVELETASPCLEANER_H
#define SYNTHESIS_WAVELETASPCLEANER_H

//# Includes
#include <casacore/scimath/Mathematics/FFTServer.h>
#include <synthesis/MeasurementEquations/AspMatrixCleaner.h>
#include <deque>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>
#include <casacore/casa/Utilities/CountedPtr.h>

namespace casa { //# NAMESPACE CASA - BEGIN
	
// Default Gaussian Scale Model
inline double gaussianScaleValue2D(
    int i, int j,
    double cx, double cy,
    float scaleSize)
{
    return (1.0/(2*M_PI*pow(scaleSize,2))) *
           exp(-(pow(i-cx,2) + pow(j-cy,2)) *
           0.5 / pow(scaleSize,2));
}

class WaveletAspCleaner : public AspMatrixCleaner
{
public:
  // Create a cleaner : default constructor
  WaveletAspCleaner();

  // The destructor does nothing special.
  ~WaveletAspCleaner();
  
  // Make an image of the specified scale by Gaussian
  void makeInitScaleImage(casacore::Matrix<casacore::Float>& iscale, const casacore::Float& scaleSize) override;
  void makeScaleImage(casacore::Matrix<casacore::Float>& iscale, const casacore::Float& scaleSize, const casacore::Float& amp, const casacore::IPosition& center) override;
  
  //Normalization Helpers
  float computePeakNormalization(float width) override;
  float computeScaleNormalization(float width1, float width2) override;
  
  //Run algorithm
  void runLBFGS(
	alglib::minlbfgsstate &state,
	alglib::real_1d_array &x,
	alglib::minlbfgsreport &rep,
	const std::vector<casacore::IPosition> &activeSetCenter,
	casacore::FFTServer<casacore::Float,casacore::Complex> &fft) const override;

  
  void setWaveletControl(const casacore::Vector<casacore::Float> waveletScales, const casacore::Vector<casacore::Float> waveletAmps) { itsWaveletScales = waveletScales; itsWaveletAmps=waveletAmps;}

protected:
//private:

  casacore::Vector<casacore::Float> itsWaveletScales;
  casacore::Vector<casacore::Float> itsWaveletAmps;
};

} //# NAMESPACE CASA - END

#endif
