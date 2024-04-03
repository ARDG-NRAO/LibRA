//# CalInterpolator.h: a class to interpolate calibration information
//# Copyright (C) 1996,1997,1998,2000,2001,2002,2003
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

#ifndef CALIBRATION_CALINTERPOLATOR_H
#define CALIBRATION_CALINTERPOLATOR_H

#include <casacore/casa/aips.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <msvis/MSVis/VisBuffer.h>
#include <synthesis/CalTables/CalInterpolation.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// CalInterpolator: a class to interpolate calibration information
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="CalTable">CalTable</linkto> module
//   <li> <linkto class="CalInterpolation">CalInterpolation</linkto> module
//   <li> <linkto class="VisBuffer">VisBuffer</linkto> module
// </prerequisite>
//
// <etymology>
// From "calibration" and "interpolation".
// </etymology>
//
// <synopsis>
// This base class defines the interface for calibration interpolation.
// Specializations for calibration table and calibration model interpolation
// are provided through inheritance. Calibration table solutions may be
// discretely sampled values for parametrized solutions and interpolation
// of both is supported through inherited sub-types. Calibration models
// may be analytic (e.g. parallactic angle PJones) or empirical based on
// global or other data not stored in calibration tables (e.g. empirical
// ionosphere models).
// </synopsis>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Define the base class interface for different calibration interpolation
// types.
// </motivation>
//
// <todo asof="02/07/01">
// (i) None known.
// </todo>

class CalInterpolator
{
 public:
  // Set the visibility buffer on which the calibration interpolator acts
  virtual void setVisBuffer (VisBuffer& vb) {vb_p = &vb; return;};

  // Compute the interpolated calibration correction for a given 
  // visibility buffer row.
  virtual casacore::Bool getVal (const casacore::Int& /*row*/, casacore::Matrix<casacore::Complex>& /*jonesMatrix*/){return false;};

 protected:
  // Public construction is prohibited.
  // Copy constructor and assignment operator are implicit.
  // Construct from a set of calibration interpolation parameters.
  CalInterpolator (const CalInterpolation& calInterp);
 
  // Desctructor
  virtual ~CalInterpolator() {};

  // Reference to the calibration interpolation parameters
  const CalInterpolation& calInterp_p;

  // Pointer to the current visibility buffer
  VisBuffer* vb_p;
};


} //# NAMESPACE CASA - END

#endif
   
  



