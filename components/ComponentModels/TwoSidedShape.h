//# TwoSidedShape.h:
//# Copyright (C) 1998,1999,2000,2002
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
//# $Id: TwoSidedShape.h 21130 2011-10-18 07:39:05Z gervandiepen $

#ifndef COMPONENTS_TWOSIDEDSHAPE_H
#define COMPONENTS_TWOSIDEDSHAPE_H

#include <casacore/casa/aips.h>
#include <components/ComponentModels/ComponentShape.h>
#include <components/ComponentModels/ComponentType.h>
#include <casacore/casa/BasicSL/Complex.h>
#include <casacore/casa/Quanta/Unit.h>
#include <casacore/casa/Quanta/Quantum.h>
#include <casacore/casa/Arrays/ArrayFwd.h>

namespace casacore{

class DirectionCoordinate;
class MDirection;
class MVAngle;
class RecordInterface;
class String;

}

namespace casa { //# NAMESPACE CASA - BEGIN


// <summary>Base class for component shapes with two sides</summary>

// <use visibility=export> 

// <reviewed reviewer="" date="yyyy/mm/dd" tests="tTwoSidedShape" demos="dTwoSidedShape">
// </reviewed>

// <prerequisite>
//   <li> <linkto class=ComponentShape>ComponentShape</linkto>
//   <li> <linkto class=casacore::Quantum>Quantum</linkto>
// </prerequisite>

// <synopsis> 

// This base class adds a common interface for component shapes that have two
// sides. These shapes can be parameterised as having a major-axis width,
// minor-axis width and a position angle (as well as the reference direction).
// Currently there are two shapes which use this parameterisation, the 
// <linkto class=GaussianShape>Gaussian</linkto> shape and the 
// <linkto class=DiskShape>disk</linkto> shape. Shapes which do not have this
// parameterisation, such as the <linkto class=PointShape>point</linkto> shape,
// are derived directly from the 
// <linkto class=ComponentShape>ComponentShape</linkto> class.

// Functions in this class implement a more convenient way of setting the
// parameters associated with two-sided shapes than the very flexible, but
// clumsy, <src>parameters</src> functions. The width parameters can be set
// using the <src>setWidth</src> functions. These functions enforce the rule
// that the major-axis width must not be smaller than the minor-axis
// width. Hence the axial ratio, which is the minor-axis width divided by the
// major-axis width, must always be greater than zero and less than or equal to
// one.

// The functions in this class also free derived classes from the having to
// deal with units. Any angular unit can be used to specify the width of the
// shape. The widths returned by the majoraxis, minoraxis & the positionAngle
// functions are in the same units as they where specified in, unless the
// convertUnit function has been used to specify another unit.

// For maximum speed the <src>*InRad</src> functions are provided. These
// functions bypass all the computation involving units.

// The toRecord and fromRecord functions in this class standardise the record
// representation of classes derived from this one. In addition to the type and
// direction fields all records, discussed in the 
// <linkto class=ComponentShape>ComponentShape</linkto> class, there are three
// more mandatory fields called; majoraxis, minoraxis & positionangle. These
// fields are record representation of casacore::Quantum<casacore::Double> objects and hence
// contain a value and unit. eg., A typical record for any of the shapes
// derived from this class might be:
// <srcblock>
// c := [type = 'gaussian',
//       direction = [type = 'direction',
//                    refer = 'b1950',
//                    m0 = [value = .1, unit = 'rad']
//                    m1 = [value = -.1, unit = 'rad']
//                   ],
//       majoraxis = [value=.5, unit='arcmin'],
//       minoraxis = [value=1, unit='arcsec'],
//       positionangle = [value=10, unit='deg']
//      ]
// </srcblock>
// </synopsis>
//
// <example>

// Because this is an abstract base class, an actual instance of this class
// cannot be constructed. However the interface it defines can be used inside a
// function. This allows functions which have TwoSidedShapes as arguments to
// work for any derived class. Hence the printShape function shown below will
// work for GaussianShapes and DiskShapes but not for PointShapes.

// In this example the printShape function prints out the type of model it is
// working with, the reference direction of that model, the major-axis width,
// minor-axis width and the position angle. This example is also available in
// the <src>dTwoSidedShape.cc</src> file.
// <srcblock>
//  void printShape(const TwoSidedShape& theShape) {
//    cout << "This is a " << ComponentType::name(theShape.type())
//         << " shape " << endl 
//         << "with a reference direction of "
//         << theShape.refDirection().getAngle("deg") << " ("
//         << theShape.refDirection().getRefString() << ")" << endl
//         << "and a major axis of " << theShape.majorAxis() << endl
//         << "      minor axis of " << theShape.minorAxis() << endl
//         << "and   position angle of " << theShape.positionAngle() << endl;
//  }
// </srcblock>
// </example>
//

// <motivation>
// This base class was created to allow the reuse of a number of common
// functions by all derived classes.
// </motivation>

// <todo asof="1999/110/12">
//   <li> Use Measures & Quanta in the interface to the visibility functions.
// </todo>

// <linkfrom anchor="TwoSidedShape" classes="ComponentShape">
//  <here>TwoSidedShape</here> - a base class for shapes with two sides
// </linkfrom>

class TwoSidedShape: public ComponentShape
{
public:

  // a virtual destructor is needed so that the actual destructor in the
  // derived class will be used.
  virtual ~TwoSidedShape();

  // return the actual component type.
  virtual ComponentType::Shape type() const = 0;

  // set/get the width and orientation of the Shape. The width of the major
  // must be larger than the width of the minor axes. The position angle is
  // measured North through East ie a position angle of zero degrees means
  // that the major axis is North-South and a position angle of 10 degrees
  // moves the Northern edge to the East. The axial ratio is the ratio of the
  // minor to major axes widths. Hence it is always between zero and one.
  // <group>
  void setWidth(const casacore::Quantum<casacore::Double>& majorAxis,
		const casacore::Quantum<casacore::Double>& minorAxis, 
		const casacore::Quantum<casacore::Double>& positionAngle);
  void setWidth(const casacore::Quantum<casacore::Double>& majorAxis,
		const casacore::Double axialRatio, 
		const casacore::Quantum<casacore::Double>& positionAngle);
  casacore::Quantum<casacore::Double> majorAxis() const;
  casacore::Quantum<casacore::Double> minorAxis() const;
  casacore::Quantum<casacore::Double> positionAngle() const;
  casacore::Double axialRatio() const;
  // </group>

  // set/get the errors on the shape parameters. 
  // <group>
  void setErrors(const casacore::Quantum<casacore::Double>& majorAxisError,
		 const casacore::Quantum<casacore::Double>& minorAxisError, 
		 const casacore::Quantum<casacore::Double>& positionAngleError);
  const casacore::Quantum<casacore::Double>& majorAxisError() const;
  const casacore::Quantum<casacore::Double>& minorAxisError() const;
  const casacore::Quantum<casacore::Double>& positionAngleError() const;
  casacore::Double axialRatioError() const;
  // </group>

  // set/get the width and orientation of the Shape. These are the same as the
  // above functions except that all widths are in radians.
  // <group>
  virtual void setWidthInRad(const casacore::Double majorAxis,
			     const casacore::Double minorAxis, 
			     const casacore::Double positionAngle) = 0;
  virtual casacore::Double majorAxisInRad() const = 0;
  virtual casacore::Double minorAxisInRad() const = 0;
  virtual casacore::Double positionAngleInRad() const = 0;
  // </group>

  // Calculate the proportion of the flux that is in a pixel of specified size
  // centered in the specified direction. The returned value will always be
  // between zero and one (inclusive).
  virtual casacore::Double sample(const casacore::MDirection& direction, 
			const casacore::MVAngle& pixelLatSize,
			const casacore::MVAngle& pixelLongSize) const = 0;

  // Same as the previous function except that many directions can be sampled
  // at once. The reference frame and pixel size must be the same for all the
  // specified directions. A default implementation of this function is
  // available that uses the single pixel sample function described above.
  // However customised versions of this function will be more efficient as
  // intermediate values only need to be computed once.
  virtual void sample(casacore::Vector<casacore::Double>& scale, 
		      const casacore::Vector<casacore::MDirection::MVType>& directions, 
		      const casacore::MDirection::Ref& refFrame,
		      const casacore::MVAngle& pixelLatSize,
		      const casacore::MVAngle& pixelLongSize) const = 0;

  // Return the Fourier transform of the component at the specified point in
  // the spatial frequency domain. The point is specified by a 3-element vector
  // (u,v,w) that has units of meters and the frequency of the observation, in
  // Hertz. These two quantities can be used to derive the required spatial
  // frequency <src>(s = uvw*freq/c)</src>. The w component is not used in
  // these functions.  The scale factor returned by this function can be used
  // to scale the flux at the origin of the Fourier plane in order to determine
  // the visibility at the specified point.

  // The "origin" of the transform is the reference direction of the
  // component. This means for symmetric components, where the reference
  // direction is at the centre, that the Fourier transform will always be
  // real.
  virtual casacore::DComplex visibility(const casacore::Vector<casacore::Double>& uvw,
			      const casacore::Double& frequency) const = 0;

  // Same as the previous function except that many (u,v,w) points can be
  // sampled at once. The observation frequency is the same for all the
  // specified points. The uvw casacore::Matrix must have first dimension of three and
  // the second dimension must match the length of the scale vector. A default
  // implementation of this function is available that uses the single point
  // visibility function described above.  However customised versions of this
  // function may be more efficient as intermediate values only need to be
  // computed once.
  virtual void visibility(casacore::Vector<casacore::DComplex>& scale, const casacore::Matrix<casacore::Double>& uvw,
			  const casacore::Double& frequency) const = 0;

  // same as above but with many frequencies
 virtual void visibility(casacore::Matrix<casacore::DComplex>& scale, const casacore::Matrix<casacore::Double>& uvw,
			  const casacore::Vector<casacore::Double>& frequency) const = 0; 

  // determine whether the shape is symmetric or not. Always returns true.
  virtual casacore::Bool isSymmetric() const;

  // Return a pointer to a copy of this object upcast to a ComponentShape
  // object. The class that uses this function is responsible for deleting the
  // pointer. This is used to implement a virtual copy constructor.
  virtual ComponentShape* clone() const = 0;

  // set/get the shape parameters associated with this shape. There are always
  // three these being in order: the major-axis, the minor-axis and the
  // position angle. All these angular quantities are specified and returned in
  // radians. The casacore::Vector supplied to the <src>setParameters</src> and
  // <src>setErrors</src> functions must have three elements and the Vector
  // returned by the <src>parameters</src> and <src>errors</src> functions will
  // have three elements. The errors are nominally 1-sigma in an implicit
  // Gaussian distribution.
  // <group>
  virtual casacore::uInt nParameters() const;
  virtual void setParameters(const casacore::Vector<casacore::Double>& newParms);
  virtual casacore::Vector<casacore::Double> parameters() const;
  virtual void setErrors(const casacore::Vector<casacore::Double>& newParms);
  virtual casacore::Vector<casacore::Double> errors() const;
  virtual casacore::Vector<casacore::Double> optParameters() const;
  virtual void setOptParameters(const casacore::Vector<casacore::Double>& newOptParms);
  // </group>

  // This functions convert between a casacore::Record and a shape derived from this
  // class. These functions define how the object is represented in glish and
  // this is detailed in the synopsis above.  They return false if the record
  // is malformed and append an error message to the supplied string giving the
  // reason.
  // <group>
  virtual casacore::Bool fromRecord(casacore::String& errorMessage,
			  const casacore::RecordInterface& record);
  virtual casacore::Bool toRecord(casacore::String& errorMessage,
			casacore::RecordInterface& record) const;
  // </group>

  // Convert the parameters of the component to the specified units. The
  // supplied record must have three fields, namely 'majoraxis', 'minoraxis' &
  // 'positionangle'. These fields must contains strings that are angular units
  // and this function will convert the corresponding parameters to the
  // specified units. This will have no effect on the shape of this class but
  // will affect the format of the record returned by the toRecord function,
  // and the units used in the the Quanta returned by the majoraxis, minoraxis
  // & positionangle functions.
  virtual casacore::Bool convertUnit(casacore::String& errorMessage,
			   const casacore::RecordInterface& record);

  // casacore::Function which checks the internal data of this class for correct
  // dimensionality and consistent values. Returns true if everything is fine
  // otherwise returns false.
  virtual casacore::Bool ok() const;

  // Convert component shape to absolute pixels (longitude, latitude, major axis, 
  // minor axis, position angle [positive +x -> +y ; rad])
  virtual casacore::Vector<casacore::Double> toPixel (const casacore::DirectionCoordinate& dirCoord) const;

  // Fill the shape from the vector (longitude, latitude, major axis, 
  // minor axis, position angle [positive +x -> +y ; rad]).  The return
  // is true if the input major axis (in pixels) became the minor 
  // axis (in world coordinates), else false.
  virtual casacore::Bool fromPixel (const casacore::Vector<casacore::Double>& parameters,
                          const casacore::DirectionCoordinate& dirCoord);

  // Get the string containing the various size quantities of a component.
  virtual casacore::String sizeToString() const = 0;

  // casacore::Format the string containing the various size quantities of a component.
  static casacore::String sizeToString(
    	casacore::Quantity major, casacore::Quantity minor, casacore::Quantity posangle,
    	casacore::Bool includeUncertainties = true, casacore::Quantity majorErr = 0,
    	casacore::Quantity minorErr = 0, casacore::Quantity posanErr = 0);

protected:
  // The constructors and assignment operator are protected as only derived
  // classes should use them.
  // <group>
  //# The default TwoSidedShape is at the J2000 North Pole.
  TwoSidedShape();

  //# Construct a TwoSidedShape at the specified direction, and return all all
  //# widths using the specified units.
  TwoSidedShape(const casacore::MDirection& direction, const casacore::Unit& majorAxisUnit, const
		casacore::Unit& minorAxisUnit, const casacore::Unit& paUnit);

  //# The copy constructor uses copy semantics.
  TwoSidedShape(const TwoSidedShape& other);

  //# The assignment operator uses copy semantics.
  TwoSidedShape& operator=(const TwoSidedShape& other);
  // </group>

private:
  casacore::Unit itsMajUnit;
  casacore::Unit itsMinUnit;
  casacore::Unit itsPaUnit;
  casacore::Quantum<casacore::Double> itsMajErr;
  casacore::Quantum<casacore::Double> itsMinErr;
  casacore::Quantum<casacore::Double> itsPaErr;
//
  casacore::Vector<casacore::Double> widthToCartesian (const casacore::Quantum<casacore::Double>& width,
                                   const casacore::Quantum<casacore::Double>& pa, 
                                   const casacore::MDirection& dirRef,
                                   const casacore::DirectionCoordinate& dirCoord,
                                   const casacore::Vector<casacore::Double>& pixelCen) const;
//
  casacore::MDirection directionFromCartesian (casacore::Double width, casacore::Double pa,
                                     const casacore::DirectionCoordinate& dirCoord,
                                     const casacore::Vector<casacore::Double>& pixelCen) const;

};

} //# NAMESPACE CASA - END

#endif
