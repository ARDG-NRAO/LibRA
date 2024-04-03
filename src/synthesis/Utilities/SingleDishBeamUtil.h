//# --------------------------------------------------------------------
//# SingleDishBeamUtil.h: this defines utility functions to caluculate
//#  single dish beam of image
//# --------------------------------------------------------------------
//# Copyright (C) 2015
//# National Astronomical Observatory of Japan
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
//# $Id$
#ifndef SINGLEDISH_BEAM_UTIL_H
#define SINGLEDISH_BEAM_UTIL_H

#include <casacore/casa/aips.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Quanta/Quantum.h>
#include <casacore/casa/Quanta/Unit.h>
#include <casacore/casa/Utilities/CountedPtr.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>

namespace casa {

class SingleDishBeamUtil
{
 public:
  SingleDishBeamUtil(const casacore::MeasurementSet &ms,
		     const casacore::String &referenceFrame,
		     const casacore::String &movingSource,
		     const casacore::String &pointingColumn,
		     const casacore::String &antenna);
  /* 
   * Return pointing sampling of map
   */
  casacore::Bool getPointingSamplingRaster(
		     casacore::Quantum<casacore::Vector<casacore::Double>> &sampling,
                     casacore::Quantity &positionAngle);
 private:
  // get pointing direction of rows in MS.
  // out put is a column major Matrix of PointingList in radian
  // (column=0: longitude, column=1: latitude)
  // Longitude is reorganized to in -pi~+pi range, if map area corsses 2pi. 
  casacore::Bool getMapPointings(
		       casacore::Matrix<casacore::Double> &pointingList);
  casacore::CountedPtr<casacore::MeasurementSet> ms_;
  const casacore::String referenceFrame_;
  const casacore::String movingSource_;
  const casacore::String pointingColumn_;
  const casacore::String antSel_;
  casacore::Unit directionUnit_;

};

} //# NAMESPACE CASA - END

#endif
