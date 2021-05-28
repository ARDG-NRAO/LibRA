//# SpectralImageUtil.cc: Spectral Image Utilities
//# Copyright (C) 2013
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
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$
#include <casa/aips.h>
#include <images/Images/SubImage.h>

#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <coordinates/Coordinates/StokesCoordinate.h>
#include <coordinates/Coordinates/Projection.h>
#include <casa/Logging/LogSink.h>
#include <casa/Logging/LogMessage.h>

#include <imageanalysis/Utilities/SpectralImageUtil.h>

#include <casa/iostream.h>
using namespace casacore;
namespace casa {
  

  Double SpectralImageUtil::worldFreq(const CoordinateSystem& cs, Double spectralpix){
    ///freq part
    Int spectralIndex=cs.findCoordinate(Coordinate::SPECTRAL);
    SpectralCoordinate
      spectralCoord=
      cs.spectralCoordinate(spectralIndex);
    Vector<String> units(1); units = "Hz";
    spectralCoord.setWorldAxisUnits(units);	
    Vector<Double> spectralWorld(1);
    Vector<Double> spectralPixel(1);
    spectralPixel(0) = spectralpix;
    spectralCoord.toWorld(spectralWorld, spectralPixel);  
    Double freq  = spectralWorld(0);
    return freq;
  }

} //#End casa namespace
