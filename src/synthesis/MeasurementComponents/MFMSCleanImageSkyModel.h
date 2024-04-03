//# MFMSCleanImageSkyModel.h: Definition for MFMSCleanImageSkyModel
//# Copyright (C) 1996,1997,1998,1999,2000
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

#ifndef SYNTHESIS_MFMSCLEANIMAGESKYMODEL_H
#define SYNTHESIS_MFMSCLEANIMAGESKYMODEL_H

#include <synthesis/MeasurementComponents/MFCleanImageSkyModel.h>
#include <casacore/lattices/LatticeMath/LatticeCleanProgress.h>

namespace casa { //# NAMESPACE CASA - BEGIN

//forward
class SkyEquation;

// <summary> Image Sky Model implementing the MultiScale, MultiField Clean algorithm </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class=ImageSkyModel>ImageSkyModel</linkto> class
//   <li> <linkto class=MFCleanImageSkyModel>MFCleanImageSkyModel</linkto> class
//   <li> <linkto class=casacore::LatticeCleaner>LatticeCleaner</linkto> class
// </prerequisite>
//
// <etymology>
// MFMSCleanImageSkyModel implements the MultiScale, MultiField Clean algorithm.
// It is derived from <linkto class=SkyModel>MFCleanImageSkyModel</linkto>.
// </etymology>
//
// <synopsis> 
// The MF Clean is an FFT-based clean algorithm. Cleaning is
// split into major and minor cycles. In a minor cycle, the
// image is cleaned using an approximate PSF and the MultiScale
// clean algorithm. In the major cycle, a fully correct
// subtraction the of cleanned emission from the visibilities
// on a pointing-by-pointing bassis is performed.
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

class MFMSCleanImageSkyModel : public MFCleanImageSkyModel {
public:

  // Create a MFMSCleanImageSkyModel with 4 scales,
  // stoplargenegatives == 2,  stoppointmode==-1
  MFMSCleanImageSkyModel();

  // Create a MFMSCleanImageSkyModel with nScales, we figure out what they are
  // Also: stoplargenegatives controls if we stop the cycle when the largest 
  // component goes negative;
  // stoppointmode controls how many of the smallest component must be
  // hit consecutively before we stop that cycle (-1 ==> don't stop)
  MFMSCleanImageSkyModel(const casacore::Int nscales,
			 const casacore::Int stoplargenegatives=2,
			 const casacore::Int stoppointmode=-1,
                         const casacore::Float smallScaleBias=0.6);

  // Create a MFMSCleanImageSkyModel, you provide the scale sizes, IN PIXELS
  // for example:  casacore::Vector<casacore::Float> scales(4); scales(0) = 0.0;  scales(1) = 3.0;  
  // scales(2) = 10.0;  scales(3) = 30.0; 
  MFMSCleanImageSkyModel(const casacore::Vector<casacore::Float>& useScaleSize,
			 const casacore::Int stoplargenegatives=2,
			 const casacore::Int stoppointmode=-1,
                         const casacore::Float smallScaleBias=0.6);

  // destructor
  ~MFMSCleanImageSkyModel();

  // Solve for this SkyModel
  virtual casacore::Bool solve (SkyEquation& me);

private:
  // Chattily get the scales into userScaleSizes_p, doing some calculation if necessary.
  void getScales();

  // set the scales
  //  void setScales(casacore::LatticeCleaner<casacore::Float>& cleaner);

  enum Scale_Method{NSCALES, USERVECTOR};
  Scale_Method method_p;

  casacore::uInt nscales_p;
  casacore::Vector<casacore::Float> userScaleSizes_p;

  casacore::LatticeCleanProgress *progress_p;

  // parameter to stop the cycle (for the first N cycles) if you get a 
  // negative on the largest scale;
  // To disable, use < 0
  casacore::Int stopLargeNegatives_p;

  // parameter which stops the cycle if you hit the smallest scale this many
  // consecutive times; if < 0, don't stop for this
  casacore::Int stopPointMode_p;

  casacore::Float smallScaleBias_p;

};


} //# NAMESPACE CASA - END

#endif


