//# PBMath2DImage.h: Definitions of 2-D Image PBMath objects
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

#ifndef SYNTHESIS_PBMATH2DIMAGE_H
#define SYNTHESIS_PBMATH2DIMAGE_H

#include <casa/aips.h>
#include <synthesis/TransformMachines/PBMath2D.h>
#include <images/Images/ImageInterface.h>
#include <measures/Measures.h>

namespace casacore{

template<class T> class TempImage;
}

namespace casa { //# NAMESPACE CASA - BEGIN

//#forward

typedef casacore::SquareMatrix<casacore::Complex,2> mjJones2;
typedef casacore::SquareMatrix<casacore::Complex,4> mjJones4;
typedef casacore::SquareMatrix<casacore::Float,2> mjJones2F;

// <summary> 2-D Image Primary Beam Model </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <li> <linkto class="PBMathInterface">PBMathInterface</linkto> class
// <li> <linkto class="PBMath2D">PBMath2D</linkto> class
// </prerequisite>
//
// <etymology>
//  PBMath2DImage: derived from  PBMath2D, implements PB and VP from sampled 2D VP
// </etymology>
//
// <synopsis> 
// See PBMath2D for a general synopsis of the 2D PB types.
// 
// The user supplies a vector which is a numerical representation
// of a voltage [attern (hey, if you have a PB, just take the square
// root, and look out for sidelobes which could be negative).
// The first element in the vector needs to be 1.0, the center of the
// voltage pattern.  The last element of the vector is the value of the
// VP at the maximumRadius.  The maximumRadius and the reference frequency at
// which the tabulated VP is intended are also required for construction.
// The PBMath2DImage constructor proceeds by performing SINC interpolation
// on the input vector to generate the highly oversampled lookup vector.
// 
// </synopsis> 
//
//
// <example>
// <srcblock>

//    numPB.applyPB( im1, im2, pointingDir);
// </srcblock>
// </example>
//
// <motivation>
// All of the 2-D PB types have everything in common except for the
// details of their parameterization.  This lightweight class
// deals with those differences: construction, filling the PBArray
// from construction parameters, and flushing to disk.
// The Image type is very handy: someone can take a sample
// illumination pattern, FT, and take a slice of the resulting voltage
// pattern and construct a  VP from that slice.
// </motivation>
//
// <todo asof="98/10/21">
// <li> constructor from a casacore::MS beam subtable
// <li> flush to casacore::MS beam subtable
// </todo>

 
class PBMath2DImage : public PBMath2D {
public:

  PBMath2DImage();

  // Instantiation from arguments; only an image is needed
  PBMath2DImage(casacore::ImageInterface<casacore::Float>& reJonesImage);
  PBMath2DImage(casacore::ImageInterface<casacore::Float>& reJonesImage,
		casacore::ImageInterface<casacore::Float>& imJonesImage);

  PBMath2DImage(const casacore::ImageInterface<casacore::Complex>& jonesImage);

  // Copy constructor
  // PBMath2DGImage(const PBMath2DImage& other);

  // Assignment operator, by reference
  PBMath2DImage& operator=(const PBMath2DImage& other);

  //destructor
 ~PBMath2DImage();  

  // Get the type of PB this is
  PBMathInterface::PBClass whichPBClass() { return PBMathInterface::IMAGE; }  
  
  // Summarize the construction data for this primary beam
  void summary(casacore::Int nValues=0);

protected:

  virtual casacore::ImageInterface<casacore::Complex>& apply(const casacore::ImageInterface<casacore::Complex>& in,
				 casacore::ImageInterface<casacore::Complex>& out,
				 const casacore::MDirection& sp,
				 const casacore::Quantity parAngle,	      
				 const BeamSquint::SquintType doSquint,
				 casacore::Bool inverse,
				 casacore::Bool conjugate,
				 casacore::Int ipower,  // ie, 1=VP, 2=PB, 4=PB^2
				 casacore::Float cutoff,
				 casacore::Bool forward); 

  virtual casacore::ImageInterface<casacore::Float>& apply(const casacore::ImageInterface<casacore::Float>& in,
			       casacore::ImageInterface<casacore::Float>& out,
			       const casacore::MDirection& sp,
			       const casacore::Quantity parAngle,	      
			       const BeamSquint::SquintType doSquint,
			       casacore::Float cutoff, casacore::Int ipower);

  virtual SkyComponent& apply(SkyComponent& in,
		      SkyComponent& out,
		      const casacore::MDirection& sp,
		      const casacore::Quantity frequency,	      
		      const casacore::Quantity parAngle,	      
		      const BeamSquint::SquintType doSquint,
		      casacore::Bool inverse,
		      casacore::Bool conjugate,
		      casacore::Int ipower,  // ie, 1=VP, 2=PB, 4=PB^2
		      casacore::Float cutoff,
		      casacore::Bool forward); 

  virtual casacore::Int support(const casacore::CoordinateSystem& cs);



private:    

  // Check for congruency
  void checkJonesCongruent(casacore::ImageInterface<casacore::Float>& reJones,
			   casacore::ImageInterface<casacore::Float>& imJones);
  void checkImageCongruent(casacore::ImageInterface<casacore::Float>& image);


  // Update the Jones Matrix
  void updateJones(const casacore::CoordinateSystem& coords,
		   const casacore::IPosition& shape,
		   const casacore::MDirection& pc,
		   const casacore::Quantity& paAngle);

  // casacore::Complex to Complex
  void applyJones(const casacore::Array<casacore::Float>* reJones,
		  const casacore::Array<casacore::Float>* imJones,
		  const casacore::Array<casacore::Complex>& in,
		  casacore::Array<casacore::Complex>& out,
		  casacore::Vector<casacore::Int>& polmap,
		  casacore::Bool inverse,
		  casacore::Bool conjugate,
		  casacore::Int ipower,  // ie, 1=VP, 2=PB
		  casacore::Float cutoff,
		  casacore::Bool circular=true,
		  casacore::Bool forward=true);

 void applyJonesFast(const casacore::Float*& reJones,
			  const casacore::Float*& imJones,
			  const casacore::Array<casacore::Complex>& in,
			  casacore::Array<casacore::Complex>& out,
			  casacore::Vector<casacore::Int>& polmap,
			  casacore::Bool /*inverse*/,
			  casacore::Bool /*conjugate*/,
			  casacore::Int ipower,  // ie, 1=VP, 2=PB
			  casacore::Float /*cutoff*/,
			  casacore::Bool circular,
		     casacore::Bool forward);
 void applyJonesFastX(const casacore::Float*& reJones,
			  const casacore::Float*& imJones,
			  const casacore::Complex*& in,
			  casacore::Complex*& outstor,
			  const casacore::Int*& polmap,
			  casacore::Bool /*inverse*/,
			  casacore::Bool /*conjugate*/,
			  casacore::Int ipower,  // ie, 1=VP, 2=PB
			  casacore::Float /*cutoff*/,
		      casacore::Bool circular,
		      casacore::Bool forward,
		      const casacore::Int ix, const casacore::Int nx, const casacore::Int ny, const casacore::Int npol);


  // casacore::Float to Float
  void applyJones(const casacore::Array<casacore::Float>* reJones,
		  const casacore::Array<casacore::Float>* imJones,
		  const casacore::Array<casacore::Float>& in,
		  casacore::Array<casacore::Float>& out,
		  casacore::Vector<casacore::Int>& polmap,
		  casacore::Float cutoff,
		  casacore::Bool circular=true);


  casacore::TempImage<casacore::Float>* reJonesImage_p;
  casacore::TempImage<casacore::Float>* reRegridJonesImage_p;
  casacore::TempImage<casacore::Float>* imJonesImage_p;
  casacore::TempImage<casacore::Float>* imRegridJonesImage_p;

  casacore::Vector<casacore::Double>* incrementsReJones_p;
  casacore::Vector<casacore::Double>* incrementsImJones_p;

  casacore::Vector<casacore::Double>* referencePixelReJones_p;
  casacore::Vector<casacore::Double>* referencePixelImJones_p;

  casacore::Float pa_p;
};


} //# NAMESPACE CASA - END

#endif
