//# PBMath2D.h: Definitions of interface for 2-D PBMath objects
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

#ifndef SYNTHESIS_PBMATH2D_H
#define SYNTHESIS_PBMATH2D_H

#include <casa/aips.h>
#include <synthesis/TransformMachines/PBMathInterface.h>

namespace casacore{

class Table;
class ImageRegion;
class CoordinateSystem;
}

namespace casa { //# NAMESPACE CASA - BEGIN

//#forward
class SkyComponent;

// <summary> base class for 1D PBMath objects </summary>


// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <li> <linkto class="SkyJones">SkyJones</linkto> class
// <li> <linkto class="BeamSkyJones">BeamSkyJones</linkto> class
// <li> <linkto class="PBMathInterface">PBMathInterface</linkto> class
// </prerequisite>
//
// <etymology>
// PBMath types do the mathematical operations of the PB's or VP's.
// This is the base class for the 1D (ie, rotationally symmetric) PB's.
// </etymology>
//
// <synopsis> 
// PBMath2D,  the virtual base class for 1D PBMath objects, is
// derived from PBMathInterface.  Its cousin, PBMath2D, can deal with
// inherently 2D voltage patterns or primary beams.  PBMath2D can deal
// with beam squint, (ie, the offset of the LL and RR beams on opposite
// sides of the pointing center) which rotates on the sky with parallactic angle.
// 
// The 1D PB philosophy is to specify the Voltage pattern or Primary Beam
// via a small number of
// parameters via one of the derived types (PBMath2DGauss, for example).  The
// derived type knows how to instantiate itself from a row in a beam subTable,
// and how to convert itself into a lookup vector.  The lookup vector is
// fine enough that no interpolation need be done when finding the nearest
// PB or VP value for a particular pixel (currently, there are 1e+4 elements
// in the lookup vector, so on average, an error on order of 1e-4 is made
// when applying the primary beam).
//
// There are two ways of creating the derived PB types:
// 1) explicitly create one of the babies.  You have control
// over the details such as PB size and total extent, the reference
// frequency at which this size is true (the size scales inversely
// with wavelength), the squint orientation, and whether a mean
// symmetrized beam will be calculated from the squinted beam.
// (Nice defaults can reduce the arguments in most cases.)
// <example>
// <srcblock>
//  PBMath2DGauss myPB  (casacore::Quantity(1.0, "'"), casacore::Quantity(3.0, "'"), casacore::Quantity(1.0, "GHz"),
//                       false,   // these are PB parameters, not VP
//                       BeamSquint(casacore::MDirection(casacore::Quantity(2.0, "\""),
//                                                      casacore::Quantity(0.0, "\""),
//                                                      casacore::MDirection::Ref(casacore::MDirection::AZEL)),
//                                  casacore::Quantity(2.0, "GHz")),
//                       false);
//  PBMath2DGauss myPB2  (casacore::Quantity(1.0, "'"), casacore::Quantity(3.0, "'"), casacore::Quantity(1.0, "GHz"));
// 
// </srcblock>
// </example>
// 2) via the envelope class PBMath's enumerated CommonPB type.  This is much simpler,
// and will deal with a majority of the cases required:
// <example>
// <srcblock>
// PBMath wsrtPB(PBMath::WSRT);
// PBMath vla_LPB(PBMath::VLA_L);  // has L band squint built in
// </srcblock>
// </example>
//
// The main thing you want to do with a primary beam or voltage pattern is
// to apply it to an image.  The top level "apply" methods are defined in
// PBMathInterface.  They are applyPB, applyPB2, applyVP.  These top level
// apply's then call a lower level private polymorphic apply, which are defined
// in PBMath2D and in PBMath2D.  These two different apply's deal with the
// different details of 1D and 2D primary beam application.
// <example>
// <srcblock>
//
//    casacore::PagedImage<casacore::Float> in;
//    casacore::PagedImage<casacore::Complex> out;
//    casacore::MDirection pointingDir(casacore::Quantity(135.0, "deg"), casacore::Quantity(60.0, "deg"), 
//                           casacore::MDirection::Ref(casacore::MDirection::J2000));
//    casacore::Quantity parallacticAngle(26.5, "deg");
//    PBMath wsrtPB(PBMath::WSRT_LOW);
//    wsrtPB.applyPB(in, out, pointingDir);   // multiply by primary beam
//    wsrtPB.applyPB(in, out, pointingDir, parallacticAngle, BeamSquint::GOFIGURE, 
//                   true, 0.02); // divide by primary beam
//    wsrtPB.applyVP(in, out, pointingDir);   // multiply by voltage pattern
//
// </srcblock>
// </example>
// </synopsis> 
//
// <motivation>
// All of the 2-D PB types have everything in common except for the
// details of their parameterization.
// </motivation>
//
// lower level helping apply methods: reduce code by this bundling
// <thrown>
// <li> casacore::AipsError - in apply(Image...), if in and out images are 
//      inconsistent in shape or coordinates
// <li> casacore::AipsError - in  apply(SkyComponent...), if doSqiont==RR or LL
// </thrown>
// <todo asof="98/010/21">
// <li> SymmetrizeBeam doesn't do anything yet.  (It should calculate
//      the mean symmetric beam about the pointing center when
//      squint is present, slightly larger than the symmetric beam 
//      about the squint position.
// </todo>

 
class PBMath2D : public PBMathInterface {
public:

  // required so PBMath can see the protected "apply" method
  // Other derivatives of PBMathInterface, such as PBMath2D, will
  // also require friend class PBMath;
  friend class PBMath;  

  PBMath2D();

  virtual ~PBMath2D();

  // Summarize the Voltage Pattern;
  // For PBMath2D, list nValues worth of the VP array
  virtual void summary(casacore::Int nValues=0);

  // Is state of PBMath OK?
  virtual casacore::Bool ok();

  // Get the casacore::ImageRegion of the primary beam on an Image for a given pointing
  // Note: casacore::ImageRegion is not necesarily constrained to lie within the
  // image region (for example, if the pointing center is near the edge of the
  // image).  fPad: extra fractional padding, beyond Primary Beam support
  // (note: we do not properly treat squint yet, this will cover it for now)
  // iChan: frequency channel to take: lowest frequency channel is safe for all
  //
  // Potential problem: this casacore::ImageRegion includes all casacore::Stokes and Frequency Channels
  // present in the input image.
  casacore::ImageRegion* extent (const casacore::ImageInterface<casacore::Complex>& in, 
		       const casacore::MDirection& pointing,
		       const casacore::Int irow,	
		       const casacore::Float fPad,  
		       const casacore::Int iChan,  
		       const SkyJones::SizeType sizeType);
  casacore::ImageRegion* extent (const casacore::ImageInterface<casacore::Float>& in, 
		       const casacore::MDirection& pointing,
		       const casacore::Int irow,	
		       const casacore::Float fPad,  
		       const casacore::Int iChan,  
		       const SkyJones::SizeType sizeType);

  virtual casacore::Int support(const casacore::CoordinateSystem& cs);

protected:

private:    

};


} //# NAMESPACE CASA - END

#endif
