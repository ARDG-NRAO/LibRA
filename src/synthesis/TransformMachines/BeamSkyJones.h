//# BeamSkyJones.h: Definitions of interface for BeamSkyJones 
//# Copyright (C) 1996,1997,1998,1999,2000,2002,2003
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

#ifndef SYNTHESIS_BEAMSKYJONES_H
#define SYNTHESIS_BEAMSKYJONES_H

#include <casacore/casa/aips.h>
#include <casacore/casa/Containers/Block.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/measures/Measures/MDirection.h>
#include <casacore/measures/Measures.h>
#include <casacore/measures/Measures/Stokes.h>
#include <synthesis/TransformMachines/SkyJones.h>
#include <synthesis/TransformMachines/PBMath.h>
#include <msvis/MSVis/VisBufferUtil.h>

namespace casacore{

class ImageRegion;
}

namespace casa { //# NAMESPACE CASA - BEGIN

//#forward
class SkyModel;
      
//# Need forward declaration for Solve in the Jones Matrices
class SkyEquation;

// <summary> beam-like sky-plane effects for the SkyEquation </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <li> <linkto class="SkyEquation">SkyEquation</linkto> class
// <li> <linkto class="SkyJones">SkyJones</linkto> class
// <li> <linkto class="PBMathInterface">PBMathInterface</linkto> class
// </prerequisite>
//
// <etymology>
// BeamSkyJones, derived from SkyJones, describes an interface to
// beam-based SkyJones objects.  Like SkyJones, it too is an Abstract
// Base Class, but implements the beam-related methods.
// </etymology>
//
// <synopsis> 
//
// <motivation>
// The properties of sky-plane based calibration effects must be described
// for the <linkto class="SkyEquation">SkyEquation</linkto>; this class
// encapsulates the antenna beam-based aspects which are present in at least
// a few other specific SkyJones (VPSkyJones and DBeamSkyJones).
// </motivation>
//
// <todo asof="98/11/01">
// <li> Solvable part needs implementation: we need to derive an
// image of gradients of the elements of the Jones matrix. See VisJones
// for how to do this.
// <li> The casacore::MS, version II, will have a beam subtable, which will have PBMaths
// for each antenna.  Until this becomes available, we need to do some
// fudging; for example, we 
// </todo>

class BeamSkyJones : virtual public SkyJones {

public:

  // Eventually, the casacore::MS will have all the beam information in its Beam Subtable.
  // Till then, we either guess the PB to use or explicitly define it upon construction.
  // Construct from a Measurement Set, figure out the most appropriate PBMath object
  // from casacore::MS information
  BeamSkyJones(const casacore::Quantity &parallacticAngleIncrement = casacore::Quantity(720.0, "deg"), // def= 1 PA interval
	       BeamSquint::SquintType doSquint = BeamSquint::NONE,  // def= no beam squint offsets
	       const casacore::Quantity &skyPositionThreshold = casacore::Quantity(180,"deg"));  // def= assume there is no change of
	                                                     // this operator due to position offset


  // Virtual destructor (this is a virtual base class)
  virtual ~BeamSkyJones() = 0;

  // Print out information concerning the state of this object
  virtual void showState(casacore::LogIO& os);

  // Apply Jones matrix to an image (and adjoint)
  // No "applyInverse" is available from the SkyJones classes,
  // you can get them directly from PBMath or you can get
  // the equivalent effect by dividing by grad grad Chi^2
  // in ImageSkyModel.
  // <group>
  casacore::ImageInterface<casacore::Complex>& apply(const casacore::ImageInterface<casacore::Complex>& in,
				 casacore::ImageInterface<casacore::Complex>& out,
				 const VisBuffer& vb, casacore::Int row,
				 casacore::Bool forward=true);
  casacore::ImageInterface<casacore::Float>& apply(const casacore::ImageInterface<casacore::Float>& in,
				     casacore::ImageInterface<casacore::Float>& out,
				     const VisBuffer& vb, casacore::Int row);

  casacore::ImageInterface<casacore::Float>& applySquare(const casacore::ImageInterface<casacore::Float>& in,
				     casacore::ImageInterface<casacore::Float>& out,
				     const VisBuffer& vb, casacore::Int row);
  // </group>

  // Apply Jones matrix to a sky component (and adjoint)
  //if fullspectral==True then the output skycomponent will have a TabularSpectrum
  //Spectral Model which will be at all the channel frequencies available in the vb;
  //when False the out SkyComponent is modified using the first channel frequency only.
  //Thus dealing with the frequency dependence of the SkyJones.
  // <group>
  SkyComponent& apply(SkyComponent& in,
		      SkyComponent& out,
		      const VisBuffer& vb, casacore::Int row,
		      casacore::Bool forward = true, casacore::Bool fullspectral=false);
  SkyComponent& applySquare(SkyComponent& in,
			    SkyComponent& out,
			    const VisBuffer& vb, casacore::Int row, casacore::Bool fullspectral=false);
  // </group>

  // Understand if things have changed since last PB application
  // <group>

  // Reset to pristine state
  virtual void reset();

  // Has this operator changed since the last Application?
  // (or more properly, since the last update() ) 
  virtual casacore::Bool changed(const VisBuffer& vb, casacore::Int row);

  // Does the operator change in this visbuffer or since the last call?
  // May not be useful -- check it out:  m.a.h. Dec 30 1999
  virtual casacore::Bool change(const VisBuffer& vb);

  // Does this operator changed in this VisBuffer,
  // starting with row1?
  // If yes, we return in row2, the last row that has the
  // same SkyJones as row1.
  // NOTE: need to first call changed(const VisBuffer& vb, casacore::Int row) and
  // shield the user from the case where the fieldID has changed
  // (which only changes in blocks)
  virtual casacore::Bool changedBuffer(const VisBuffer& vb, casacore::Int row1, casacore::Int& row2);

  // Update the FieldID, Telescope, pointingDirection, Parallactic angle info
  void update(const VisBuffer& vb, casacore::Int row);

  // if (changed) {update()}
  virtual void assure (const VisBuffer& vb, casacore::Int row);
  // </group>

  // Return the type of this Jones matrix (actual type of derived class).
  virtual Type type() {return SkyJones::E;};

  // Apply gradient
  virtual casacore::ImageInterface<casacore::Complex>& 
  applyGradient(casacore::ImageInterface<casacore::Complex>& result, const VisBuffer& vb,
		casacore::Int row);

  virtual SkyComponent&
  applyGradient(SkyComponent& result, const VisBuffer& vb,
		casacore::Int row);

  // Is this solveable?
  virtual casacore::Bool isSolveable() {return false;};

  // Initialize for gradient search
  virtual void initializeGradients();

  // Finalize for gradient search
  virtual void finalizeGradients();
 
  // Add to Gradient Chisq
  virtual void addGradients(const VisBuffer& vb, casacore::Int row, const casacore::Float sumwt, 
			    const casacore::Float chisq, const casacore::Matrix<casacore::Complex>& c, 
			    const casacore::Matrix<casacore::Float>& f);
 
  // Solve
  virtual casacore::Bool solve (SkyEquation& se);
  
  // Manage the PBMath objects
  // <group>
  // set the PB based on telescope name, antennaID and feedID
  // If antennaID or feedID is -1, the PBMath object is set for
  // all antennae or feeds, respectively. These are the default
  // values to retain the previous interface.
  //
  // Note. It would be nice to change the interface and make antennaID
  // and feedID the second and the third parameter, respectively.
  void setPBMath(const casacore::String &telescope, PBMath &myPBmath,
                 const casacore::Int &antennaID = -1, const casacore::Int &feedID = -1);
  
  // get the PBMath object; returns false if that one doesn't exist,
  // true if it does exist and is OK
  // whichAnt is an index into an array of PBMaths, which is different
  // for all telescope/antenna/feed combinations
  // Not sure why we need such a low-level method declared as public,
  // retained to preserve old interface
  casacore::Bool getPBMath(casacore::uInt whichAnt, PBMath &myPBMath) const;
  
  // get the PBMath object; returns false if that one doesn't exist,
  // true if it does exist and is OK
  // antennaID and feedID default to -1 to preserve the old interface
  // TODO: change the interface and make antennaID and feedID the
  // second and third parameter respectively to have a better looking code
  casacore::Bool getPBMath(const casacore::String &telescope, PBMath &myPBMath,
                 const casacore::Int &antennaID = -1, const casacore::Int &feedID = -1) const;

  casacore::Quantity getPAIncrement() {return casacore::Quantity(parallacticAngleIncrement_p,"rad");}

  casacore::Quantity getSkyPositionThreshold() {return casacore::Quantity(skyPositionThreshold_p,"rad");}
  
  // Return true if all antennas share a common VP
  casacore::Bool isHomogeneous() const;
  //</group>
  
  // Get the casacore::ImageRegion of the primary beam on an Image for a given pointing
  // Note: casacore::ImageRegion is not necesarily constrained to lie within the
  // image region (for example, if the pointing center is near the edge of the
  // image).  fPad: extra padding over the primary beam supporrt, 
  // fractional (ie, 1.2 for 20% padding), in all directions.
  // (note: we do not properly treat squint yet, this will cover it for now)
  // iChan: frequency channel to take: lowest frequency channel is safe for all
  //
  // Potential problem: this casacore::ImageRegion includes all casacore::Stokes and Frequency Channels
  // present in the input image.
  casacore::ImageRegion*  extent (const casacore::ImageInterface<casacore::Complex>& im, 
			const VisBuffer& vb,  
			const casacore::Int irow=-1,			
			const casacore::Float fPad=1.2,  
			const casacore::Int iChan=0, 
			const SkyJones::SizeType sizeType=SkyJones::COMPOSITE);

  casacore::ImageRegion*  extent (const casacore::ImageInterface<casacore::Float>& im, 
			const VisBuffer& vb,  const casacore::Int irow=-1,
			const casacore::Float fPad=1.2,  const casacore::Int iChan=0, 
			const SkyJones::SizeType sizeType=SkyJones::COMPOSITE);

  // summarize the PBMaths contained here.
  // n = -1 => terse table
  // n =  0 => table plus constructor values
  // n =  m => plot m samples of the PB profile
  virtual void summary(casacore::Int n=0);

  //return the telescope it is on at this state
  casacore::String telescope();

  //Get an idea of the support of the PB in number of pixels
  virtual casacore::Int support(const VisBuffer& vb, const casacore::CoordinateSystem& cs);

private:  




  casacore::Int lastFieldId_p;

  casacore::Int lastArrayId_p;

  casacore::Int lastMSId_p;

  casacore::Double lastTime_p;
  BeamSquint::SquintType doSquint_p;

  casacore::Double  parallacticAngleIncrement_p; // a parallactic angle threshold
                        // beyond which the operator is considered to be
			// changed (in radians)
  casacore::Double  skyPositionThreshold_p;     // a sky position threshold beyond
                        // which the operator is considered to be changed
			// (in radians)
  casacore::Block<casacore::Double> lastParallacticAngles_p; // a cache of parallactic angles
                        // used when the operator was applied last time.
			// One value in radians for each beam model in PBMaths.
			// A zero-length block means that the operator
			// has never been applied
  casacore::Block<casacore::MDirection> lastDirections_p; // a chache of directions
                        // used when the operator was applied last time.
			// One element for each beam model in PBMaths.
			// A zero-length block means that the operator
			// has never been applied

  // One or more PBMaths (a common one for the
  // entire array, or one for each antenna)
  // This requires some sorting out for heterogeneous arrays!
  casacore::Block<PBMath> myPBMaths_p;  
  // Names of telescopes (parralel with PBMaths
  casacore::Block<casacore::String> myTelescopes_p;

  // Antenna IDs (parallel with PBMaths)
  casacore::Block<casacore::Int> myAntennaIDs_p;
  // Feed IDs (parallel with PBMaths)
  casacore::Block<casacore::Int> myFeedIDs_p;

  // cache of the indices to the PBMaths container for antenna/feed 1 and 2  
  mutable const VisBuffer *lastUpdateVisBuffer_p; // to ensure that the cache
                                          // is filled for the correct
					  // VisBuffer. The value is used
					  // for comparison only
  mutable casacore::Int lastUpdateRow_p;  // to ensure that the cache is filled for
                                // correct row in the VisBuffer
  mutable casacore::Int lastUpdateIndex1_p; // index of the first antenna/feed
  mutable casacore::Int lastUpdateIndex2_p; // index of the second antenna/feed
  //

  mutable casacore::Bool hasBeenApplied;  // true if the operator has been applied at least once

  // update the indices cache. This method could be made protected in the
  // future (need access functions for lastUpdateIndex?_p to benefit from it)
  // Cache will be valid for a given VisBuffer and row
  void updatePBMathIndices(const VisBuffer &vb, casacore::Int row) const;

protected:
  // return true if two directions are close enough to consider the
  // operator unchanged, false otherwise
  casacore::Bool directionsCloseEnough(const casacore::MDirection &dir1,
                             const casacore::MDirection &dir2) const;
  			     
  // return index of compareTelescope, compareAntenna and compareFeed in
  // myTelescopes_p, myAntennaIDs and myFeedIDs; -1 if not found
  // if compareAntenna or compareTelescope is -1, this means that the
  // PBMath class is the same for all antennae/feeds. An exception will
  // be raised, if separate PBMath objects have been assigned by setPBMath
  // for different feeds/antennae but -1 is used for query.
  //
  // It would be good to rename this function to indexBeams as this name
  // is more appropriate. 
  //
  casacore::Int indexTelescope(const casacore::String & compareTelescope,
                     const casacore::Int &compareAntenna=-1,
		     const casacore::Int &compareFeed=-1) const;
  casacore::MDirection convertDir(const VisBuffer& vb, const casacore::MDirection& inDir, const casacore::MDirection::Types outType);
  casacore::String telescope_p;
  casacore::CountedPtr<VisBufferUtil> vbutil_p;

};
 

} //# NAMESPACE CASA - END

#endif

