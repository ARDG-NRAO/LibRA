//# PBMath.h: Envelope class for PBMath1D and PBMath2D types
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

#ifndef SYNTHESIS_PBMATH_H
#define SYNTHESIS_PBMATH_H

#include <casacore/casa/aips.h>
#include <synthesis/TransformMachines/PBMathInterface.h>
#include <synthesis/TransformMachines/PBMath1DGauss.h>
#include <synthesis/TransformMachines/PBMath1DNumeric.h>
#include <synthesis/TransformMachines/PBMath1DAiry.h>
#include <synthesis/TransformMachines/PBMath1DPoly.h>
#include <synthesis/TransformMachines/PBMath1DIPoly.h>
#include <synthesis/TransformMachines/PBMath1DCosPoly.h>
#include <synthesis/TransformMachines/PBMath2DImage.h>
#include <synthesis/TransformMachines/BeamSquint.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/measures/Measures.h>
#include <casacore/casa/Utilities/CountedPtr.h>

namespace casacore{

class Table;
class ImageRegion;
class RecordInterface;
class CoordinateSystem;
}

namespace casa { //# NAMESPACE CASA - BEGIN

//#forward
// <summary> Primary beam envelope class, derived from PBMathInterface </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <li> <linkto class="SkyJones">SkyJones</linkto> class
// <li> <linkto class="BeamSkyJones">BeamSkyJones</linkto> class
// <li> <linkto class="PBMathInterface">PBMathInterface</linkto> class
// </prerequisite>
//
// <etymology>
// PBMath types do the mathematical operations of the Primary Beamss or
// Voltage patterns.  This envelope class's name was designed for simplicity.
// </etymology>
//
// <synopsis> 
// PBMath is an envelope class for the various types of 1D and 2D primary
// beam/voltage pattern objects.  They all have a common interface defined
// by PBMathInterface.  The PBMath object has a counted pointer to the
// actual PB type; hence, the PBMath object is lightweight and can
// be passed around by value in other classes.
//
// Construct a PBMath via PBMath's enumerated CommonPB type.
// <example>
// <srcblock>
// PBMath wsrtPB(PBMath::WSRT);
// PBMath vla_LPB(PBMath::VLA_L);  // has L band squint built in
// </srcblock>
// </example>
//
// A PBMath can also be constructed by the complete argument list for
// the type of PB it holds.  Since different PB types can have the same
// argument list (PBMath1DPoly and PBMath1DIPoly, for example), the exact
// type also needs to be specified via the PBMathInterface::PBClass.
//
// The main thing you want to do with a primary beam or voltage pattern is
// to apply it to an image.  The PBMath object forwards its low level
// "apply" calls to the letter class's "apply" calls.
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
// </srcblock>
// </example>
// 
// </synopsis> 
//
//
// <motivation>
// This Envelope class allows the user to create a PBMath object which
// can display all the polymorphic traits of PBMath1D classes or
// PBMath2D classes.
// </motivation>
//
// <todo asof="98/010/21">
// </todo>

 
class PBMath {
public:

  // This enumeration provides a simple way to instantiate the
  // common primary beam models.  They are each represented by one of
  // the more fundamental PBMath1D types
  //
  // New CommonPB Types must go to the end
  enum CommonPB {UNKNOWN, DEFAULT,
		 ATCA_L1, ATCA_L2, ATCA_L3, ATCA_S, ATCA_C, ATCA_X, 
		 GBT, 
		 GMRT,
		 HATCREEK,
		 NRAO12M,
		 NRAO140FT,
		 OVRO, 
		 VLA, VLA_INVERSE, VLA_NVSS, VLA_2NULL, 
		 VLA_4, VLA_P, VLA_L, VLA_C, VLA_X, VLA_U, VLA_K, VLA_Q, 
		 WSRT, WSRT_LOW, ALMA, ALMASD, ACA, IRAMPDB, IRAM30M, SMA, 
		 ATA, ATCA_C_RI, ATCA, ATCA_16, ATCA_K, ATCA_Q, ATCA_W,  EVLA, NGVLA, NONE
  };

  enum {
    NumberOfCommonPB=44  };

  // Default constructor, required for casacore::Block<PBMath>
  PBMath();
  
  // Make a PB of common type
  explicit PBMath(PBMath::CommonPB myPBType, casacore::Bool useSymmetricBeam=false);

  //Make a PB from a telescopename and frequency

  PBMath(casacore::String& telescopeName, casacore::Bool useSymmetricBeam=false, 
	 casacore::Quantity freq=casacore::Quantity(0.0, "Hz"));

  //Make a PB by dish diameter
  PBMath(casacore::Double dishDiam, casacore::Bool useSymmetricBeam, casacore::Quantity freq);

  // Make a PB from a RecordInterface
  explicit PBMath(const casacore::RecordInterface& myrec);

  // Make a Gaussian Primary beam
  PBMath(PBMathInterface::PBClass theclass, casacore::Quantity halfWidth, 
	 casacore::Quantity maxRad, casacore::Quantity refFreq, 
	 casacore::Bool isThisVP=false,
	 BeamSquint squint=BeamSquint(casacore::MDirection(casacore::Quantity(0.0, "deg"),
						 casacore::Quantity(0.0, "deg"),
						 casacore::MDirection::Ref(casacore::MDirection::AZEL)), 
				      casacore::Quantity(1.0, "GHz")),
	 casacore::Bool useSymmetricBeam=false);
  

  // Make a casacore::Polynomial, Inverse casacore::Polynomial, or Numeric Primary beam
  // (These three have identical calling signatures, they can only be distinguished
  // by the PBClass enumeration)
  PBMath(PBMathInterface::PBClass theclass, const casacore::Vector<casacore::Double>& coeff, 
	 casacore::Quantity maxRad, casacore::Quantity refFreq, 
	 casacore::Bool isThisVP=false,
	 BeamSquint squint=BeamSquint(casacore::MDirection(casacore::Quantity(0.0, "deg"),
						 casacore::Quantity(0.0, "deg"),
						 casacore::MDirection::Ref(casacore::MDirection::AZEL)),
				      casacore::Quantity(1.0, "GHz")),
	 casacore::Bool useSymmetricBeam=false);

  

  // Make a Numeric Primary beam (FLOAT!)
  PBMath(PBMathInterface::PBClass theclass, const casacore::Vector<casacore::Float>& coeff, 
	 casacore::Quantity maxRad, casacore::Quantity refFreq, 
	 casacore::Bool isThisVP=false,
	 BeamSquint squint=BeamSquint(casacore::MDirection(casacore::Quantity(0.0, "deg"),
						 casacore::Quantity(0.0, "deg"),
						 casacore::MDirection::Ref(casacore::MDirection::AZEL)),
				      casacore::Quantity(1.0, "GHz")),
	 casacore::Bool useSymmetricBeam=false);

  
  // Make a CosPolynomial beam type
  PBMath(PBMathInterface::PBClass theclass, const casacore::Vector<casacore::Double>& coeff, 
	 const casacore::Vector<casacore::Double>& scale, 
	 casacore::Quantity maxRad, casacore::Quantity refFreq,
	 casacore::Bool isThisVP=false,
	 BeamSquint squint=BeamSquint(casacore::MDirection(casacore::Quantity(0.0, "deg"),
						 casacore::Quantity(0.0, "deg"),
						 casacore::MDirection::Ref(casacore::MDirection::AZEL)),
				      casacore::Quantity(1.0, "GHz")),
	 casacore::Bool useSymmetricBeam=false);
  
  // Make an Airy beam type
  PBMath(PBMathInterface::PBClass theclass, casacore::Quantity dishDiam, casacore::Quantity blockageDiam,
	 casacore::Quantity maxRad, casacore::Quantity refFreq,
	 BeamSquint squint=BeamSquint(casacore::MDirection(casacore::Quantity(0.0, "deg"),
						 casacore::Quantity(0.0, "deg"),
						 casacore::MDirection::Ref(casacore::MDirection::AZEL)),
				      casacore::Quantity(1.0, "GHz")),
	 casacore::Bool useSymmetricBeam=false);
  
  // Make an Image-based beam
  PBMath(PBMathInterface::PBClass theclass,
	 casacore::ImageInterface<casacore::Float>& reJones);
  PBMath(PBMathInterface::PBClass theclass,
	 casacore::ImageInterface<casacore::Float>& reJones,
	 casacore::ImageInterface<casacore::Float>& imJones);
  
  // Instantiate from a Table
  //  PBMath(const casacore::Table& theTable, casacore::Int row);
  
  
  // copy ctor: copies the reference counted pointer
  PBMath(const PBMath &other);

  // destructor
  virtual ~PBMath();
  
  //operator=   returns a reference to the PBMath; reference counted pointer
  PBMath& operator=(const PBMath& other);
  
  // comparison operator: just checks that the objects are same
  // (could have two different objects with identical data, these are treated
  // as !=)
  casacore::Bool operator==(const PBMath& other) const;
  
  // comparison operator: just checks that the objects are different
  // (could have two different objects with identical data, these are treated
  // as !=)
  casacore::Bool operator!=(const PBMath& other) const;
  
  // copy()      returns a copy of the PBMath
  // PBMath copy() const;
  

  //  Apply Jones matrix to an image (and adjoint)
  // <group>

  casacore::ImageInterface<casacore::Complex>& applyVP(const casacore::ImageInterface<casacore::Complex>& in,
				   casacore::ImageInterface<casacore::Complex>& out,
				   const casacore::MDirection& sp,
				   const casacore::Quantity parAngle = casacore::Quantity(0.0,"deg"),
				   const BeamSquint::SquintType doSquint = BeamSquint::NONE,
				   casacore::Bool inverse = false,
				   casacore::Bool conjugate = false,
				   casacore::Float cutoff = 0.01,
				   casacore::Bool forward=true);

  casacore::ImageInterface<casacore::Complex>& applyPB(const casacore::ImageInterface<casacore::Complex>& in,
				   casacore::ImageInterface<casacore::Complex>& out,
				   const casacore::MDirection& sp,
				   const casacore::Quantity parAngle = casacore::Quantity(0.0,"deg"),
				   const BeamSquint::SquintType doSquint = BeamSquint::NONE,
				   casacore::Bool inverse = false,
				   casacore::Float cutoff = 0.01,
				   casacore::Bool forward = true);

  casacore::ImageInterface<casacore::Float>& applyPB(const casacore::ImageInterface<casacore::Float>& in,
				   casacore::ImageInterface<casacore::Float>& out,
				   const casacore::MDirection& sp,
				   const casacore::Quantity parAngle = casacore::Quantity(0.0,"deg"),
				   const BeamSquint::SquintType doSquint = BeamSquint::NONE,
				   casacore::Float cutoff = 0.01);


  casacore::ImageInterface<casacore::Float>& applyPB2(const casacore::ImageInterface<casacore::Float>& in,
				  casacore::ImageInterface<casacore::Float>& out,
				  const casacore::MDirection& sp,
				  const casacore::Quantity parAngle = casacore::Quantity(0.0,"deg"),
				  const BeamSquint::SquintType doSquint = BeamSquint::NONE,
				  casacore::Float cutoff=0.01);
  // </group>
  
  // Apply Jones matrix to a sky component (and adjoint)
  // <group>
  SkyComponent& applyVP(SkyComponent& in,
			SkyComponent& out,
			const casacore::MDirection& sp,
			const casacore::Quantity frequency,
			const casacore::Quantity parAngle = casacore::Quantity(0.0,"deg"),
			const BeamSquint::SquintType doSquint = BeamSquint::NONE,
			casacore::Bool inverse = false,
			casacore::Bool conjugate = false,
			casacore::Float cutoff = 0.01,
			casacore::Bool forward = true);

  SkyComponent& applyPB(SkyComponent& in,
			SkyComponent& out,
			const casacore::MDirection& sp,
			const casacore::Quantity frequency,
			const casacore::Quantity parAngle = casacore::Quantity(0.0,"deg"),
			const BeamSquint::SquintType doSquint = BeamSquint::NONE,
			casacore::Bool inverse = false,
			casacore::Float cutoff = 0.01,
			casacore::Bool forward = true);

  SkyComponent& applyPB2(SkyComponent& in,
			 SkyComponent& out,
			 const casacore::MDirection& sp,
			 const casacore::Quantity frequency,
			 const casacore::Quantity parAngle = casacore::Quantity(0.0,"deg"),
			 const BeamSquint::SquintType doSquint = BeamSquint::NONE);
  // </group>





  
  // Fit an azimuthally symmetric beam to the squinted or otherwise 2-D beam
  // for casacore::Stokes I?
  void setUseSymmetric(casacore::Bool useSym=true) { pb_pointer_p->setUseSymmetric(useSym); }

  // Get value of useSymmetric
  casacore::Bool getUseSymmetric() { return pb_pointer_p->getUseSymmetric(); }
  
  // Get the type of PB this is
  PBMathInterface::PBClass whichPBClass() { return pb_pointer_p->whichPBClass(); }  

  // Summarize the Voltage Pattern;
  void summary(casacore::Int nValues=0);

  // Is the state of the PBMath OK?
  casacore::Bool ok() const;

  // Get the BLC, TRC of the primary beam on an Image for a given pointing
  // Note: BLC and TRC are not necesarily constrained to lie within the
  // image region (for example, if the pointing center is near the edge of the
  // image).  fPad: extra fractional padding above PB support
  // (note: we do not properly treat squint yet, this will cover it for now)
  // iChan: frequency channel to take: lowest frequency channel is safe for all
  casacore::ImageRegion* extent (const casacore::ImageInterface<casacore::Complex>& im, 
		       const casacore::MDirection& pointing,
		       const casacore::Int row,
		       const casacore::Float fPad,  
		       const casacore::Int iChan,  
		       const SkyJones::SizeType);
  casacore::ImageRegion* extent (const casacore::ImageInterface<casacore::Float>& im, 
		       const casacore::MDirection& pointing,
		       const casacore::Int row,
		       const casacore::Float fPad,  
		       const casacore::Int iChan,  
		       const SkyJones::SizeType);

  virtual casacore::Int support (const casacore::CoordinateSystem& cs);

  // given the Telescope name and the frequency, guess the most approrpiate
  // CommonPB primary beam type and the band
  static void whichCommonPBtoUse (casacore::String &telescope, casacore::Quantity &freq, 
				  casacore::String &band, PBMath::CommonPB &whichPB, 
				  casacore::String &pbName);

  // converts the enumrated type into a string
  static void nameCommonPB(const PBMath::CommonPB iPB, casacore::String & str);
  
  // converts the PB casacore::String into an enumrated type
  static void enumerateCommonPB(const casacore::String & str, PBMath::CommonPB &ipb);


  // return a PBMathInteface for a commonPB ; caller needs to delete pointer
  static PBMathInterface* pbMathInterfaceForCommonPB(const PBMath::CommonPB ipb, bool useSymmetricBeam);

  // return a PBMathInteface from a Vpmanager record ; caller needs to delete pointer
  static PBMathInterface* pbMathInterfaceFromRecord(const casacore::RecordInterface& rec);
  // gives the name of the PB Class that has been used
  void namePBClass(casacore::String & name) {  pb_pointer_p->namePBClass(name); }  

  
  // Gradient stuff: this needs more thought, as it is not
  // really connected with the stuff in VPSkyJones and all.
  // This will return the partial derivatives of chi^2 w.r.t
  // the beam model parameters, represented here as a vector.
  // Not implemented yet.  
  // void parameterGradient(casacore::Vector<casacore::Float> grad);
  
  // Flush the construction parameters to disk
  // This is a public method because this class doesn't know what Table
  // it is to be stored in

  //virtual casacore::Bool flushToTable(casacore::Table& beamSubTable, casacore::Int iRow);

  
protected:

  // polymorphic lower level helping apply methods
  // <group>
  casacore::ImageInterface<casacore::Complex>& apply(const casacore::ImageInterface<casacore::Complex>& in,
				 casacore::ImageInterface<casacore::Complex>& out,
				 const casacore::MDirection& sp,
				 const casacore::Quantity parAngle,	      
				 const BeamSquint::SquintType doSquint,
				 casacore::Bool inverse,
				 casacore::Bool conjugate,
				 casacore::Int ipower,  // ie, 1=VP, 2=PB
				 casacore::Float cutoff=0.01,
				 casacore::Bool forward=true); 

  SkyComponent& apply(SkyComponent& in,
		      SkyComponent& out,
		      const casacore::MDirection& sp,
		      const casacore::Quantity frequency,	      
		      const casacore::Quantity parAngle,	      
		      const BeamSquint::SquintType doSquint,
		      casacore::Bool inverse,
		      casacore::Bool conjugate,
		      casacore::Int ipower,  // ie, 1=VP, 2=PB, 4=PB^2
		      casacore::Float cutoff=0.01,
		      casacore::Bool forward=true); 
  // </group>  


private:    
  
  casacore::CountedPtr<PBMathInterface> pb_pointer_p;
  

  // These should probably move to casacore::RecordInterface when we are happy with their performance
  // <group>

  static casacore::Bool getQuantity(const casacore::RecordInterface& rec, const casacore::String& item, 
		   casacore::Quantity& returnedQuantity) ;
  static casacore::Bool getMDirection(const casacore::RecordInterface& rec, const casacore::String& item, 
		     casacore::MDirection& returnedMDirection);
  //casacore::Function to initialize the state of the tool
  void initByTelescope(PBMath::CommonPB myPBType, casacore::Bool useSymmetricBeam=false, 
		       casacore::Double frequency=0.0);

  //casacore::Function to initialize the by dish diameter 
  void initByDiameter(casacore::Double diam, casacore::Bool useSymmetricBeam, casacore::Double freq); 

  // </group>

};

  

} //# NAMESPACE CASA - END

#endif
