//# CSCleanImageSkyModel.h: Definition for CSCleanImageSkyModel
//# Copyright (C) 1996,1997,1998,1999,2000,2003
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

#ifndef SYNTHESIS_CSCLEANIMAGESKYMODEL_H
#define SYNTHESIS_CSCLEANIMAGESKYMODEL_H

#include <synthesis/MeasurementComponents/CleanImageSkyModel.h>
#include <synthesis/MeasurementEquations/ClarkCleanProgress.h>
namespace casacore{

template<class T> class Lattice;
}

namespace casa { //# NAMESPACE CASA - BEGIN

//forward
class SkyEquation;

// <summary> 
// CS Image Sky Model: Image Sky Model implementing the CS Clean algorithm
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class=ImageSkyModel>ImageSkyModel</linkto> module
// </prerequisite>
//
// <etymology>
// CSCleanImageSkyModel implements the Cotton-Schwab Clean algorithm.
// It is derived from <linkto class=SkyModel>SkyModel</linkto>.
// </etymology>
//
// <synopsis> 
// The CS Clean is an FFT-based clean algorithm. Cleaning is
// split into major and minor cycles. In a minor cycle, the
// brightest pixels are cleaned using only the strongest sidelobes
// (and main lobe) of the PSF. In the major cycle, a fully correct
// subtraction of the PSF is done for all points accumulated in the
// minor cycle by degridding and degridding.
//
// The CS Clean is implemented using the <linkto class=ClarkCleanModel</linkto>
// class.
//
// Masking is optionally performed using a mask image: only points
// where the mask is non-zero are cleaned. If no mask is specified
// all points in the inner quarter of the image are cleaned.
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
// <ul> Allow specification of more control parameters
// </todo>

class CSCleanImageSkyModel : public CleanImageSkyModel {
public:

  virtual casacore::Int add(casacore::ImageInterface<casacore::Float>& image, const casacore::Int maxNumXfr=100);

  virtual casacore::Bool addResidual(casacore::Int image, casacore::ImageInterface<casacore::Float>& residual);

  virtual casacore::Bool addMask(casacore::Int image, casacore::ImageInterface<casacore::Float>& mask);

  // Solve for this SkyModel
  virtual casacore::Bool solve (SkyEquation& me);

protected:
  // Return the maximum absolute value per field
  casacore::Float maxField(casacore::Block<casacore::Vector<casacore::Float> >& imagemax, casacore::Block<casacore::Vector<casacore::Float> >& imagemin);
  // Return maximum abs outer sidelobe, more than nCenter pixels from the center
  casacore::Vector<casacore::Float> outerMinMax(casacore::Lattice<casacore::Float> & lat, const casacore::uInt nCenter );
  // Pointer to the progress display object
  ClarkCleanProgress *progress_p;

};


} //# NAMESPACE CASA - END

#endif


