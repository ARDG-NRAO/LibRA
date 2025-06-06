//# GaussianShape.h:
//# Copyright (C) 1998,1999,2000,2001,2002
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
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id: GaussianShape.h 21130 2011-10-18 07:39:05Z gervandiepen $

#ifndef COMPONENTS_GAUSSIANSHAPE_H
#define COMPONENTS_GAUSSIANSHAPE_H

#include <casacore/casa/aips.h>
#include <casacore/scimath/Functionals/Gaussian2D.h>
#include <casacore/casa/BasicSL/Complex.h>
#include <components/ComponentModels/ComponentType.h>
#include <components/ComponentModels/TwoSidedShape.h>
#include <casacore/casa/Arrays/ArrayFwd.h>

namespace casacore{

class MDirection;
class MVAngle;
template <class Qtype> class Quantum;

}

namespace casa { //# NAMESPACE CASA - BEGIN


// <summary>A Gaussian model for the spatial distribution of emission</summary>

// <use visibility=export> 

// <reviewed reviewer="" date="yyyy/mm/dd" tests="tGaussianShape" demos="dTwoSidedShape">
// </reviewed>

// <prerequisite>
//   <li> <linkto class=TwoSidedShape>TwoSidedShape</linkto>
// </prerequisite>

// <synopsis> 

// A GaussianShape models the spatial distribution of radiation from the sky as
// a two-dimensional Gaussian function with user specified major axis width,
// minor axis width and position angle.

// This class like the other component shapes becomes more useful when used
// through the <linkto class=SkyComponent>SkyComponent</linkto> class, which
// incorperates the flux and spectral variation of the emission, or through the
// <linkto class=ComponentList>ComponentList</linkto> class, which handles
// groups of SkyComponent objects.

// The reference direction is defined in celestial co-ordinates, using a
// <linkto class=casacore::MDirection>MDirection</linkto> object. It indicates where the
// centre of the Gaussian is on the sky. The direction can be specified both in
// the constructor or with the <src>setRefDirection</src> function.

// The width of the Gaussian is defined as the angle subtended by the full
// width at half maximum of the Gaussian. The major axis has the larger width
// and is aligned North-South when the position angle is zero. A positive
// position angle moves the Northern side of the component to the East.  The
// axial ratio is the ratio of the minor to major axis widths. The major axis
// MUST not be smaller than the minor axis otherwise an exception (casacore::AipsError)
// is thrown.

// These parameters of the Gaussian (width, position angle, direction etc.) can
// be specified at construction time, using the <src>*inRad</src> functions or
// through functions in the base classes (TwoSidedShape & ComponentShape). The
// base classes also implement functions for inter-converting this object into
// a record representation. 

// The flux, or integrated intensity, is always normalised to one. This class
// does not model the actual flux or its variation with frequency. It solely
// models the way the emission varies with position on the sky.

// The <src>scale</src> member function is used to sample the component at any
// point on the sky. The scale factor calculated by this function is the
// proportion of the flux that is within a specified pixel size centered on the
// specified direction. Ultimatly this function will integrate the emission
// from the Gaussian over the entire pixel but currently it just assumes the
// flux can be calculated by the height of the Gaussian at the centre of the
// pixel scaled by the pixel area. This is <em>NOT</em> accurate for Gaussians
// whose width is small compared with the pixel size.

// This class contains functions that return the Fourier transform of the
// component at a specified spatial frequency. There are described more fully
// in the description of the <src>visibility</src> functions below.
// </synopsis>

// <example>
// Suppose I had an image of a region of the sky and we wanted to subtract
// a extended source from it. This could be done as follows:
// <ul>
// <li> Construct a SkyComponent with a Gaussian of width that is similar to
//      the extended source.
// <li> Project the component onto an image
// <li> Convolve the image by the point spread function
// <li> subtract the convolved model from the dirty image.
// </ul>
// Shown below is the code to perform the first step in this process, ie
// construct the SkyComponent. This example is also available in the
// <src>dTwoSidedShape.cc</src> file.  Note that it is more accurate to do
// subtraction of components in the (u,v) domain.
// <srcblock>
// { // Construct a Gaussian shape
//   casacore::MDirection blob_dir;
//   { // get the right direction into blob_dir
//     casacore::Quantity blob_ra; casacore::MVAngle::read(blob_ra, "19:39:");
//     casacore::Quantity blob_dec; casacore::MVAngle::read(blob_dec, "-63.43.");
//     blob_dir = casacore::MDirection(blob_ra, blob_dec, casacore::MDirection::J2000);
//   }
//   {
//     const Flux<casacore::Double> flux(6.28, 0.1, 0.15, 0.01);
//     const GaussianShape shape(blob_dir,
//                         casacore::Quantity(30, "arcmin"), 
//                         casacore::Quantity(2000, "mas"), 
//                         casacore::Quantity(M_PI_2, "rad"));
//     const ConstantSpectrum spectrum;
//     SkyComponent component(flux, shape, spectrum);
//     printShape(shape);
//   }
// }
// </srcblock>
// The printShape function is the example shown for the TwoSidedShape class.
// </example>
//
// <todo asof="1999/11/12">
//   <li> Use Measures & Quanta in the interface to the visibility functions.
//   <li> Use a better way of integrating over the pixel area in the sample
//   function. 
// </todo>

// <linkfrom anchor="GaussianShape" classes="ComponentShape TwoSidedShape PointShape DiskShape">
//  <here>GaussianShape</here> - a Gaussian variation in the sky brightness
// </linkfrom>


class GaussianShape: public TwoSidedShape
{
public:
  // The default GaussianShape is at the J2000 North Pole. with a full width at
  // half maximum (FWHM) on both axes of 1 arc-min.
  GaussianShape();

  // Construct a Gaussian shape centred in the specified direction, specifying
  // the widths & position angle.
  // <group>
  GaussianShape(const casacore::MDirection& direction,
		const casacore::Quantum<casacore::Double>& majorAxis,
		const casacore::Quantum<casacore::Double>& minorAxis,
		const casacore::Quantum<casacore::Double>& positionAngle);
  GaussianShape(const casacore::MDirection& direction, const casacore::Quantum<casacore::Double>& width,
		const casacore::Double axialRatio,
		const casacore::Quantum<casacore::Double>& positionAngle);
  // </group>

  // The copy constructor uses copy semantics.
  GaussianShape(const GaussianShape& other);

  // The destructor does nothing special.
  virtual ~GaussianShape();

  // The assignment operator uses copy semantics.
  GaussianShape& operator=(const GaussianShape& other);

  // get the type of the shape. This function always returns
  // ComponentType::GAUSSIAN.
  virtual ComponentType::Shape type() const;

  // set or return the width and orientation of the Gaussian. All numerical
  // values are in radians. There are also functions in the base class for
  // doing this with other angular units.
  // <group>
  virtual void setWidthInRad(const casacore::Double majorAxis,
			     const casacore::Double minorAxis, 
			     const casacore::Double positionAngle);
  virtual casacore::Double majorAxisInRad() const;
  virtual casacore::Double minorAxisInRad() const;
  virtual casacore::Double positionAngleInRad() const;
  virtual casacore::Double axialRatio() const;
  // </group>

  // Calculate the proportion of the flux that is in a pixel of specified size
  // centered in the specified direction. The returned value will always be
  // between zero and one (inclusive).
  virtual casacore::Double sample(const casacore::MDirection& direction, 
			const casacore::MVAngle& pixelLatSize,
			const casacore::MVAngle& pixelLongSize) const;

  // Same as the previous function except that many directions can be sampled
  // at once. The reference frame and pixel size must be the same for all the
  // specified directions.
  virtual void sample(casacore::Vector<casacore::Double>& scale, 
		      const casacore::Vector<casacore::MDirection::MVType>& directions, 
		      const casacore::MDirection::Ref& refFrame,
		      const casacore::MVAngle& pixelLatSize,
		      const casacore::MVAngle& pixelLongSize) const;

  // Return the Fourier transform of the component at the specified point in
  // the spatial frequency domain. The point is specified by a 3 element vector
  // (u,v,w) that has units of meters and the frequency of the observation, in
  // Hertz. These two quantities can be used to derive the required spatial
  // frequency <src>(s = uvw*freq/c)</src>. The w component is not used in
  // these functions.

  // The reference position for the transform is the direction of the
  // component. As this component is symmetric about this point the transform
  // is always a real value.
  virtual casacore::DComplex visibility(const casacore::Vector<casacore::Double>& uvw,
			      const casacore::Double& frequency) const;

  // Same as the previous function except that many (u,v,w) points can be
  // sampled at once. The uvw casacore::Matrix must have a first dimension of three, and
  // a second dimension that is the same as the length of the scale
  // Vector. Otherwise and exception is thrown (when compiled in debug mode).
  virtual void visibility(casacore::Vector<casacore::DComplex>& scale, const casacore::Matrix<casacore::Double>& uvw,
			  const casacore::Double& frequency) const;

  // as above but with many frequencies
  virtual void visibility(casacore::Matrix<casacore::DComplex>& scale, const casacore::Matrix<casacore::Double>& uvw,
			  const casacore::Vector<casacore::Double>& frequency) const;

  // Return a pointer to a copy of this object upcast to a ComponentShape
  // object. The class that uses this function is responsible for deleting the
  // pointer. This is used to implement a virtual copy constructor.
  virtual ComponentShape* clone() const;

  // casacore::Function which checks the internal data of this class for correct
  // dimensionality and consistent values. Returns true if everything is fine
  // otherwise returns false.
  virtual casacore::Bool ok() const;

  // return a pointer to this object.
  virtual const ComponentShape* getPtr() const; 

  // TODO This probably should be made a pure virtual method in TwoSidedShape
  // Return the effective area of the Gaussian (pi/(4*ln(2))*maj*min.
  // Units of the returned casacore::Quantity are steradians.
  virtual casacore::Quantity getArea() const;

  virtual casacore::String sizeToString() const;

private:
  //# Updates the parameters of the itsFT object
  void updateFT();
  //# A generic Gaussian function
  casacore::Gaussian2D<casacore::Double> itsShape;
  //# The FT of a Gaussian is also a Gaussian. Its parameters are stored here
  casacore::Gaussian2D<casacore::Double> itsFT;
};

} //# NAMESPACE CASA - END

#endif
