//# SkyJones.h: Definitions of interface for SkyJones 
//# Copyright (C) 1996,1997,1998,2000,2003
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

#ifndef SYNTHESIS_TRANSFORM2_SKYJONES_H
#define SYNTHESIS_TRANSFORM2_SKYJONES_H

#include <casacore/casa/aips.h>
#include <casacore/casa/BasicSL/Complex.h>
#include <casacore/images/Images/ImageInterface.h>
#include <components/ComponentModels/ComponentList.h>
#include <components/ComponentModels/SkyComponent.h>
#include <casacore/casa/Utilities/CompositeNumber.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <synthesis/TransformMachines/SkyJones.h>

namespace casacore{

class ImageRegion;
class CoordinateSystem;
}

namespace casa{
//class SkyEquation;
namespace refim{ //#namespace for refactored imaging
//#forward


//# Need forward declaration for Solve in the Jones Matrices

// <summary> 
// Sky Jones: Model sky-plane instrumental effects for the SkyEquation
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <li> <linkto class="SkyEquation">SkyEquation</linkto> class
// </prerequisite>
//
// <etymology>
// SkyJones describes an interface for Components to be used in
// the <linkto class="SkyEquation">SkyEquation</linkto>.
// It is an Abstract Base Class: most methods
// must be defined in derived classes.
// </etymology>
//
// <synopsis> 
//
// A SkyJones implements the instrumental effect of some sky-plane based
// calibration effect. It conceptually applies an image of Jones matrices
// to an image. For example, it takes an image of the sky brightness and
// applies the complex primary beam for a given interferometer. Only the
// interface is defined here in the Abstract Base Class. Actual concrete
// classes must be derived from SkyJones.  Some (most) SkyJones are
// solvable: the SkyEquation can be used by the SkyJones to return
// gradients with respect to itself (via the image interface). Thus for a
// SkyJones to solve for itself, it calls the SkyEquation methods to get
// gradients of chi-squared with respect to the image pixel values. The
// SkyJones then uses these gradients as appropriate to update itself.
//
//
// The following examples illustrate how a SkyJones can  be
// used:
// <ul>
// <li> Mosaicing: one SkyJones for the primary beam. For efficiency
// the image must be cached and the VisSet iteration order set to
// minimize the number of recalculations.
// </ul>
// </synopsis> 
//
// <example>
// <srcblock>
//
//  // Low level code example:
//
//    casacore::MeasurementSet ms("imagertest/3C273XC1.ms", casacore::Table::Update);
//    VPSkyJones  unsquintedVPSJ(ms);
//    VPSkyJones  squintedVPSJ(ms, true, casacore::Quantity(5.0, "deg"), BeamSquint::GOFIGURE);
//    unsquintedVPSJ.reset();
//    squintedVPSJ.reset();
//
//    casacore::PagedImage<casacore::Complex> imageIn;
//    casacore::PagedImage<casacore::Complex> imageOut;  // in reality, this ouwld need more construction with coords and shapes
//
//    VisSet vs(ms,bi,chanSelection,interval);
//
//    casacore::Int row = 0;
//    VisIter &vi = vs.iter();
//    VisBuffer vb(vi);
//    for (vi.originChunks();vi.moreChunks();vi.nextChunk()) {
//      cout << "This is integration: " << nChunks++ << endl;
//      for (vi.origin();vi.more();vi++) {
//        if (myVPSJ.change(vb)) { 
//          cout << "State has changed: " << endl;
//          squintedVPSJ.showState(os);
//          squintedVPSJ.apply(imageIn, imageOut, vb, row);
//        }
//      }
//    }
//
//
//      // High Level Code Example:
//
//
//      // Create an ImageSkyJones from an image on disk
//      ImageSkyModel ism(casacore::PagedImage<casacore::Float>("3C273XC1.modelImage"));
//
//      // Make an FTMachine: here we use a simple Grid and FT.
//      GridFT ft;
//
//      SkyEquation se(ism, vs, ft);
//
//      // Make a Primary Beam Sky Model
//      VPSkyJones pbsj(ms);
//
//      // Add it to the SkyEquation
//      se.setSkyJones(pbsj);
//
//      // Predict the visibility set
//      se.predict();
//
//      // Read some other data
//      VisSet othervs("3c84.MS.Otherdata");
//
//      // Make a Clean Image and write it out
//      HogbomCleanImageSkyJones csm(ism);
//      if (csm.solve(othervs)) {
//        casacore::PagedImage<casacore::Float> cleanImage=csm.image(0);
//        cleanImage.setName("3c84.cleanImage");
//      }
//
// </srcblock>
// </example>
//
// <motivation>
// The properties of sky-plane based calibration effects must be described
// for the <linkto class="SkyEquation">SkyEquation</linkto>
// </motivation>
//
// <todo asof="97/10/01">
// <li> There are no Concrete Classes derived from SkyJones!
// <li> Solvable part needs implementation: we need to derive an
// image of gradients of the elements of the Jones matrix. See VisJones
// for how to do this.
// </todo>

class SkyJones {
public:
 
  // Allowed types of VisJones matrices
  //enum Type{E,T,F,D};

  //enum SizeType{COMPOSITE, POWEROF2, ANY};

  SkyJones();
  // Destructor.
  virtual ~SkyJones();

  // Apply Jones matrix to an image (and adjoint)
  // <group>
  virtual casacore::ImageInterface<casacore::Complex>& apply(const casacore::ImageInterface<casacore::Complex>& in,
					 casacore::ImageInterface<casacore::Complex>& out,
					 const vi::VisBuffer2& vb, casacore::Int row,
					 casacore::Bool forward=true) = 0;

  virtual casacore::ImageInterface<casacore::Float>& apply(const casacore::ImageInterface<casacore::Float>& in,
					 casacore::ImageInterface<casacore::Float>& out,
					 const vi::VisBuffer2& vb, casacore::Int row) = 0;

  virtual casacore::ImageInterface<casacore::Float>& applySquare(const casacore::ImageInterface<casacore::Float>& in,
					     casacore::ImageInterface<casacore::Float>& out,
					     const vi::VisBuffer2& vb, casacore::Int row) = 0;
  // </group>

  // Apply Jones matrix to a sky component (and adjoint)
  // <group>
  virtual SkyComponent& apply(SkyComponent& in,
			      SkyComponent& out,
			      const vi::VisBuffer2& vb, casacore::Int row, casacore::Bool forward=true) = 0;
  virtual SkyComponent& applySquare(SkyComponent& in,
				    SkyComponent& out,
				    const vi::VisBuffer2& vb, casacore::Int row) = 0;
  // </group>

  // Has this operator changed since the last application?
  virtual casacore::Bool changed(const vi::VisBuffer2& vb, casacore::Int row) = 0;

  // Does this operator changed in this VisBuffer,
  // starting with row1?
  // If yes, we return in row2, the last row that has the
  // same SkyJones as row1.
  virtual casacore::Bool changedBuffer(const vi::VisBuffer2& vb, casacore::Int row1, casacore::Int& row2) = 0;

  // Reset
  virtual void reset() = 0;

  // Assure
  virtual void assure(const vi::VisBuffer2& vb, casacore::Int row) = 0;

  // Does the operator change in this visbuffer or since the last
  // call?       
  // I'm not sure this is useful -- come back to it
  // m.a.h.  Dec 30 1999
  virtual casacore::Bool change(const vi::VisBuffer2& vb) = 0;

  // Return the type of this Jones matrix (actual type of derived class).
  virtual ::casa::SkyJones::Type type() = 0;

  // Apply gradient
  virtual casacore::ImageInterface<casacore::Complex>& 
  applyGradient(casacore::ImageInterface<casacore::Complex>& result, const vi::VisBuffer2& vb, casacore::Int row)
  = 0;
  virtual SkyComponent&
  applyGradient(SkyComponent& result, const vi::VisBuffer2& vb, casacore::Int row)
  = 0;

  // Is this solveable?
  virtual casacore::Bool isSolveable()=0;

  // Initialize for gradient search
  virtual void initializeGradients()=0;

  // Finalize for gradient search
  virtual void finalizeGradients()=0;
 
  // Add to Gradient Chisq
  virtual void addGradients(const vi::VisBuffer2& vb, casacore::Int row, const casacore::Float sumwt,
			    const casacore::Float chisq, const casacore::Matrix<casacore::Complex>& c, 
			    const casacore::Matrix<casacore::Float>& f)=0;
 
  // Solve
  //virtual casacore::Bool solve (SkyEquation& se) = 0;

  // Get the casacore::ImageRegion of the primary beam on an Image for a given pointing
  // Note: casacore::ImageRegion is not necesarily constrained to lie within the
  // image region (for example, if the pointing center is near the edge of the
  // image).  fPad: extra fractional padding beyond the primary beam support
  // (note: we do not properly treat squint yet, this will cover it for now)
  // iChan: frequency channel to take: lowest frequency channel is safe for all
  // SizeType: COMPOSITE = next larger composite number,
  //           POWEROF2  = next larger power of 2,
  //           ANY       = just take what we get!
  //
  // Potential problem: this casacore::ImageRegion includes all casacore::Stokes and Frequency Channels
  // present in the input image.
  //COMMENTING out for now as this depend on PBMathInterface and which depends
    //back on SkyJones::sizeType

  virtual casacore::ImageRegion*  extent (const casacore::ImageInterface<casacore::Complex>& im, 
				const vi::VisBuffer2& vb,
				const casacore::Int irow=-1,                        
				const casacore::Float fPad=1.2,  
				const casacore::Int iChan=0, 
				const casa::SkyJones::SizeType sizeType=casa::SkyJones::COMPOSITE)=0;
  virtual casacore::ImageRegion*  extent (const casacore::ImageInterface<casacore::Float>& im, 
				const vi::VisBuffer2& vb,
				const casacore::Int irow=-1,
				const casacore::Float fPad=1.2,  
				const casacore::Int iChan=0, 
				const casa::SkyJones::SizeType sizeType=casa::SkyJones::COMPOSITE)=0;

  virtual casacore::String telescope()=0;
  
  virtual casacore::Int support(const vi::VisBuffer2& vb, const casacore::CoordinateSystem& cs)=0;

  virtual void setThreshold(const casacore::Float t){threshold_p=t;};
  virtual casacore::Float threshold(){return  threshold_p;};


protected:

  // Could be over-ridden if necessary
  virtual casacore::String typeName() {
    switch(type()) {
    case ::casa::SkyJones::E:    // voltage pattern (ie, on-axis terms)
      return "E Jones";
    case ::casa::SkyJones::T:    // Tropospheric effects
      return "T Jones";
    case ::casa::SkyJones::F:    // Faraday
      return "F Jones";
    case ::casa::SkyJones::D:    // D Beam (ie, polarization leakage beam; off axis terms)
      return "D Jones";
    }
    return "Not known";
  };

  casacore::Float threshold_p;
 

private:    
};
 
}//# end of namespace refim

} // end namespace casa

#endif










