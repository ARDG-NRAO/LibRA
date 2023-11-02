//# NNLSImageSkyModel.h: Definition for NNLSImageSkyModel
//# Copyright (C) 1996,1997,1998,2000
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

#ifndef SYNTHESIS_NNLSIMAGESKYMODEL_H
#define SYNTHESIS_NNLSIMAGESKYMODEL_H

#include <synthesis/MeasurementComponents/CleanImageSkyModel.h>
namespace casa { //# NAMESPACE CASA - BEGIN

//forward
class SkyEquation;

// <summary> Briggs' Non-Negative Least Squares deconvolution algorithm </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class=ImageSkyModel>ImageSkyModel</linkto> module
// </prerequisite>
//
// <etymology>
// NNLSImageSkyModel implements the NNLS deconvolution algorithm.
// It is derived from <linkto class=SkyModel>ImageSkyModel</linkto>.
// </etymology>
//
// <synopsis> 
// NNLS is implemented using the 
// <linkto class=casacore::NNLSMatrixSolver>NNLSMatrixSolver</linkto> 
// class. Since the brightness must be non-negative, only the casacore::Stokes I
// is solved for. Any other polarizations are not changed.
//
// Two masks are needed: the fluxmask determining the
// free pixels in the output image, and the datamask determining
// the constraint pixels in the dirty image. 
//
// In the solution, a matrix is required having elements connecting
// all free pixels with all constraint pixels. This is constructed
// from the point spread function. The total size of this matrix
// dominates the memory usage. 
// </synopsis> 
//
// <example>
// See the example for <linkto class=SkyModel>SkyModel</linkto>.
// </example>
//
// <motivation>
// </motivation>
//
// <todo asof="97/10/01">
// </todo>

class NNLSImageSkyModel : public CleanImageSkyModel {
public:

  // Solve for this SkyModel
  virtual casacore::Bool solve (SkyEquation& me);

private:

  casacore::Bool maskedZeroI();

};


} //# NAMESPACE CASA - END

#endif


