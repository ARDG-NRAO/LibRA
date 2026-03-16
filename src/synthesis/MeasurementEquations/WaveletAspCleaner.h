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

class WaveletAspCleaner : public AspMatrixCleaner
{
public:
  // Create a cleaner : default constructor
  WaveletAspCleaner();

  // The destructor does nothing special.
  ~WaveletAspCleaner();

  // Make an image of the specified scale by Gaussian
  std::vector<casacore::Float> getActiveSetAspen() override;
  
  //Normalization Helpers
  float aspScaleModel(const casacore::Int& i, const casacore::Int& j, const casacore::Float& scaleSize, const casacore::Int& refi, const casacore::Int& refj) override;
  float aspPeakNormModel(const casacore::Float& width) override;
  float aspNormalizationModel(const casacore::Float& width1, const casacore::Float& width2) override;
  
  void setWaveletControl(const casacore::Vector<casacore::Float> waveletScales, const casacore::Vector<casacore::Float> waveletAmps) { itsWaveletScales = waveletScales; itsWaveletAmps=waveletAmps;}

protected:
//private:

  casacore::Vector<casacore::Float> itsWaveletScales;
  casacore::Vector<casacore::Float> itsWaveletAmps;
};

} //# NAMESPACE CASA - END

#endif
