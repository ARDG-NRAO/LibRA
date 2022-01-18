//# StokesImageUtil.h: Definitions for casacore::Stokes Image utilities
//# Copyright (C) 1996,1997,1998,1999
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

#ifndef SYNTHESIS_STOKESIMAGEUTIL_H
#define SYNTHESIS_STOKESIMAGEUTIL_H

#include <casa/aips.h>
#include <casa/BasicSL/Complex.h>
#include <images/Images/ImageInterface.h>
#include <casa/Quanta/Quantum.h>
#include <ms/MeasurementSets/MeasurementSet.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// </summary>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// </prerequisite>
//
// <etymology>
// </etymology>
//
// <synopsis> 
// </synopsis> 
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// </motivation>
//
// <todo asof="">
// </todo>


class StokesImageUtil {
  
public:
 
  enum PolRep {
    CIRCULAR=0,
    LINEAR,
    UNKNOWN=-1
  };
  // Make a Gaussian PSF
  //<group>
  static void MakeGaussianPSF(casacore::ImageInterface<casacore::Float>& psf, casacore::Vector<casacore::Float>& beam,
			      casacore::Bool norm=false);
  static void MakeGaussianPSF(casacore::ImageInterface<casacore::Float>& psf,
			      casacore::Quantity& bmaj, casacore::Quantity& bmin, casacore::Quantity& bpa,
			      casacore::Bool norm=false);
  //</group>
  
  // Fit a Gaussian PSF
  //<group>
      static casacore::Bool FitGaussianPSF(casacore::ImageInterface<casacore::Float>& psf, casacore::Vector<casacore::Float>& beam, casacore::Float psfcutoff = 0.35);
  static casacore::Bool FitGaussianPSF(casacore::ImageInterface<casacore::Float>& psf, casacore::GaussianBeam& beam, casacore::Float psfcutoff = 0.35);
  static casacore::Bool FitGaussianPSF(casacore::ImageInterface<casacore::Float>& psf, casacore::ImageBeamSet& beam, casacore::Float psfcutoff = 0.35);
  static void FindNpoints(casacore::Int& npoints , casacore::IPosition& blc,  casacore::IPosition& trc, casacore::Int nrow, casacore::Float amin,  casacore::Int px, casacore::Int py, casacore::Vector<casacore::Double>& deltas, casacore::Matrix<casacore::Double>& x , casacore::Vector<casacore::Double>& y, casacore::Vector<casacore::Double>& sigma, casacore::Matrix<casacore::Float>& lpsf);
    static  void ResamplePSF(casacore::Matrix<casacore::Float>& psf, casacore::Int& oversampling, casacore::Matrix<casacore::Float>& resampledPsf,casacore::String& InterpMethod);
  //</group>
  // Locat peak of PSF return pos, peak and first plane that satisfies
  // peak >0.9
  static void locatePeakPSF(casacore::ImageInterface<casacore::Float>& psf, casacore::Int& xpos, casacore::Int& ypos, 
			    casacore::Float& amp, casacore::Matrix<casacore::Float>& psfplane);
  ////make the psf of each plane have peak 1.0 (i.e peak in the inner 1/8 of each plane)
  /// you are on your own if the peak outside that zone :(
  /// It returns the maximum of the peak value found
  static casacore::Float normalizePSF(casacore::ImageInterface<casacore::Float>& psf);

  // Convolve a casacore::Stokes Image in place
  //<group>
  static void Convolve(casacore::ImageInterface<casacore::Float>& image,
		       casacore::ImageInterface<casacore::Float>& psf);
  static void Convolve(casacore::ImageInterface<casacore::Float>& image, casacore::Float bmaj,
		       casacore::Float bmin, casacore::Float bpa, casacore::Bool normalizeVolume=false);
  static void Convolve(casacore::ImageInterface<casacore::Float>& image,
		       casacore::GaussianBeam& beam,
		       casacore::Bool normalizeVolume=false);
  static void Convolve(casacore::ImageInterface<casacore::Float>& image,
		       casacore::ImageBeamSet& beams,
		       casacore::Bool normalizeVolume=false);
  //</group>
  
  
  // Zero selected planes of a casacore::Stokes image
  static void Zero(casacore::ImageInterface<casacore::Float>& image, casacore::Vector<casacore::Bool>& mask);

  // Mask mask iif(image > threshhold), where threshhold is in image's units.
  static void MaskFrom(casacore::ImageInterface<casacore::Float>& mask,
		       casacore::ImageInterface<casacore::Float>& image,
		       const casacore::Double threshhold);

  // This version uses threshold.get("Jy").getValue().
  static void MaskFrom(casacore::ImageInterface<casacore::Float>& mask,
		       casacore::ImageInterface<casacore::Float>& image,
		       const casacore::Quantity& threshold);
  

  // Zero pixels where casacore::Stokes I < some value
  static void MaskOnStokesI(casacore::ImageInterface<casacore::Float>& image, const casacore::Quantity& threshold);

  // Make a box mask
  static void BoxMask(casacore::ImageInterface<casacore::Float>& mask, const casacore::IPosition& blc,
		      const casacore::IPosition& trc, const casacore::Float value);

  // Constrain a casacore::Stokes Image
  static void Constrain(casacore::ImageInterface<casacore::Float>& image);
  
  // Convert from casacore::Stokes Image to "correlation" image.
  static void From(casacore::ImageInterface<casacore::Complex>& out,
		   const casacore::ImageInterface<casacore::Float>& in);
  
  // Convert to casacore::Stokes Image from "correlation" image.
  static void To(casacore::ImageInterface<casacore::Float>& out, casacore::ImageInterface<casacore::Complex>& in);

  // Direct copy from casacore::Float to casacore::Complex with 
  static void directCFromR(casacore::ImageInterface<casacore::Complex>& out,
		   const casacore::ImageInterface<casacore::Float>& in);
  
  // Direct copy To Float...
  static void directCToR(casacore::ImageInterface<casacore::Float>& out, casacore::ImageInterface<casacore::Complex>& in);

  
  // Convert to casacore::Stokes PSF
  static void ToStokesPSF(casacore::ImageInterface<casacore::Float>& out, casacore::ImageInterface<casacore::Complex>& in);

  static void ToStokesSumWt(casacore::Matrix<casacore::Float> sumwtStokes, casacore::Matrix<casacore::Float> sumwtCorr, const casacore::CoordinateSystem& outcoord, const casacore::CoordinateSystem& incoord);
  
  static void ToStokesSumWt(casacore::Matrix<casacore::Float> sumwtStokes, casacore::Matrix<casacore::Float> sumwtCorr);
  
  // Find the mapping from pixel on the casacore::Stokes Axis to I,Q,U,V
  static casacore::Int StokesPolMap(casacore::Vector<casacore::Int>& map, const casacore::CoordinateSystem& coord);
  
  // Find the mapping from pixel on the casacore::Stokes Axis to either XX,XY,YX,YY
  // of LL, LR, RL, RR. Return type as well: Linear:1, Circular 0
  static casacore::Int CStokesPolMap(casacore::Vector<casacore::Int>& map, StokesImageUtil::PolRep& polframe,
			   const casacore::CoordinateSystem& coord);
  
  // Find all mappings from coordinate to axis number
  static casacore::Bool StokesMap(casacore::Vector<casacore::Int>& map, const casacore::CoordinateSystem& coord);
  
  //Create a casacore::CoordinateSystem from a casacore::MeasurementSet and other info
  static casacore::CoordinateSystem StokesCoordFromMS(const casacore::IPosition& shape,
					    casacore::Vector<casacore::Double>& deltas,
					    casacore::MeasurementSet& ms);
  
  // Create a casacore::CoordinateSystem from a casacore::MeasurementSet and other info
  static casacore::CoordinateSystem StokesCoordFromMS(const casacore::IPosition& shape,
					    casacore::Vector<casacore::Double>& deltas,
					    casacore::MeasurementSet& ms,
					    casacore::Vector<casacore::Int>& whichStokes,
					    casacore::Bool doCStokes=false,
					    casacore::Int fieldID=0, casacore::Int SPWID=0,
					    casacore::Int feedID=0);
  
  // Create a casacore::CoordinateSystem 
  static casacore::CoordinateSystem CStokesCoord(//const casacore::IPosition& shape,
				       const casacore::CoordinateSystem& coord, 
				       casacore::Vector<casacore::Int>& whichStokes,
				       StokesImageUtil::PolRep  polRep=StokesImageUtil::CIRCULAR);
  /*
  static CoordinateSystem
  CStokesCoordFromImage(const casacore::ImageInterface<casacore::Complex>& image,
			casacore::Vector<casacore::Int>& whichStokes,
			SkyModel::PolRep polRep);
  */
  // Change the stokes representation (but not the data!)
  static void changeCStokesRep(casacore::ImageInterface<casacore::Complex>& image,
			       StokesImageUtil::PolRep polRep);

  static void changeLabelsStokesToCorrStokes(casacore::StokesCoordinate &stokesCoord, 
					                             StokesImageUtil::PolRep polRep,
                                                                     casacore::Vector<casacore::Int>&whichStokes);

  // check to see if Image coordinates have the standard order:
  // Direction, casacore::Stokes, Spectral.  Returns false if this is not
  // the order, or if any are missing
  // <group>
  static casacore::Bool standardImageCoordinates(const casacore::CoordinateSystem& coords);
  static casacore::Bool standardImageCoordinates(const casacore::ImageInterface<casacore::Complex>& image);
  static casacore::Bool standardImageCoordinates(const casacore::ImageInterface<casacore::Float>& image);
  // </group>

};

} //# NAMESPACE CASA - END

#endif


