//# ComponentShape.h: Base class for component shapes
//# Copyright (C) 1998,1999,2000,2001
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
//# $Id: ComponentShape.h 21130 2011-10-18 07:39:05Z gervandiepen $

#ifndef COMPONENTS_COMPONENTSHAPE_H
#define COMPONENTS_COMPONENTSHAPE_H

#include <casacore/casa/aips.h>
#include <casacore/casa/BasicSL/Complexfwd.h>
#include <casacore/measures/Measures/MDirection.h>
#include <casacore/casa/Quanta/Quantum.h>
#include <casacore/casa/Utilities/RecordTransformable.h>
#include <components/ComponentModels/ComponentType.h>
#include <casacore/casa/Arrays/ArrayFwd.h>

namespace casacore{

class DirectionCoordinate;
class MVAngle;
class RecordInterface;
class String;
template <class T> class MeasRef;
}

namespace casa { //# NAMESPACE CASA - BEGIN


// <summary>Base class for component shapes</summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="tComponentShape" demos="dPointShape">
// </reviewed>

// <prerequisite>
//   <li> <linkto class=casacore::MDirection>MDirection</linkto>
// </prerequisite>
//
// <synopsis>

// This abstract base class defines the interface for classes which
// define the shape of a component. The most fundamental derived class is the
// <linkto class=PointShape>point</linkto> shape class but the 
// <linkto class=GaussianShape>Gaussian</linkto> shape,
// <linkto class=DiskShape>disk</linkto> shape and 
// <linkto class=LimbDarkenedDiskShape>limbdarkeneddisk classes are also
// available. These classes model the spatial distribution of emission from the
// sky. 

// Classes derived from the <linkto class=SpectralModel>SpectralModel</linkto>
// class are used to model the spectral characteristics and the 
// <linkto class=Flux>Flux</linkto> class is used to model the flux. The
// <linkto class=SkyComponent>SkyComponent</linkto> class incorporates these
// three characteristics (flux, shape & spectrum) and the 
// <linkto class=ComponentList>ComponentList</linkto> class handles groups of
// SkyComponent objects.

// This base class parameterises shapes with two quantities.
// <dl>
// <dt><em> A reference direction.</em>
// <dd> This is specified using an <linkto class=casacore::MDirection>MDirection</linkto>
//      object and indicates the direction on a defined reference point
//      within the shape. Usually this reference point is the centre of the
//      shape. 
// <dt> <em>A casacore::Vector of parameters.</em>
// <dd> This contains other parameters that the are defined differently for
//      different shapes. The length of the vector may vary for different
//      component shapes. 
// </dl>
// 
// The basic operation of classes using this interface is to model the flux as
// a function of direction on the sky. Classes derived from this one do not
// know what the flux of the component. Instead the sample and visibility
// functions return factors that are used to scale the flux and calculate the
// amount of flux at a specified point on the sky or on the (u,v) plane.

// Any allowed direction reference frame can be used. However the reference
// frame must be adequately specified in order to allow conversions to other
// reference frames. For example if the reference frame code for a component is
// casacore::MDirection::AZEL then the reference frame must also contain the time and
// position, on the earth, that the specified azimuth and elevation to refer
// to. This way the sample functions can convert the direction to a value in
// the J2000 reference frame (if you specify the sample direction in the J2000
// frame).

// </synopsis>

// <example> 

// Because this is an abstract base class, an actual instance of this class
// cannot be constructed. However the interface it defines can be used inside a
// function. This is always recommended as it allows functions which have
// ComponentShapes as arguments to work for any derived class.

// In this example the printShape function prints out the type of model it is
// working with and the reference direction of that model. This example is also
// available in the <src>dPointShape.cc</src> file.

// <srcblock>
// void printShape(const ComponentShape& theShape) {
//   cout << "This is a " << ComponentType::name(theShape.type())
//        << " shape " << endl 
//        << "with a reference direction of "
//        << theShape.refDirection() << endl;
// }
// </srcblock>
// </example>

// <motivation>
// The Shape base class was split from the SkyCompRep base class so that mixing
// components with different spatial and spectral shapes did not result in a
// combinatorial explosion in the number of classes required.
// </motivation>
//
// <todo asof="1999/11/11">
//   <li> Use Measures & Quanta in the interface to the visibility functions.
// </todo>

class ComponentShape: public casacore::RecordTransformable
{
public:
  // a virtual destructor is needed so that the actual destructor in the
  // derived class will be used.
  virtual ~ComponentShape();

  // return the actual shape. The ident function returns it as a String.
  // <group>
  virtual ComponentType::Shape type() const = 0;
  virtual const casacore::String& ident() const;
  // </group>

  // set/get the reference direction
  // <group>
  void setRefDirection(const casacore::MDirection& newRefDir);
  const casacore::MDirection& refDirection() const;
  // </group>

  // set/get the error in the reference direction. Values must be positive
  // angular quantities otherwise an casacore::AipsError exception is thrown. The errors
  // are usually interpreted as the 1-sigma bounds in latitude/longitude and
  // implicitly assume a Gaussian distribution.
  // <group>
  void setRefDirectionError(const casacore::Quantum<casacore::Double>& newRefDirErrLat, 
			    const casacore::Quantum<casacore::Double>& newRefDirErrLong);
  const casacore::Quantum<casacore::Double>& refDirectionErrorLat() const;
  const casacore::Quantum<casacore::Double>& refDirectionErrorLong() const;
  // </group>

  // copy direction info from that object to this object
  void copyDirectionInfo(const ComponentShape& that);

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

  //Same as above except for lots of frequencies too...scale rows is uvw points, columns
  // is frequency values
  virtual void visibility(casacore::Matrix<casacore::DComplex>& scale, const casacore::Matrix<casacore::Double>& uvw,
			  const casacore::Vector<casacore::Double>& frequency) const = 0;

  // determine whether the shape is symmetric or not. If it is then all the
  // scale factors returned by the visibility functions will be real numbers.
  virtual casacore::Bool isSymmetric() const = 0;

  // Return a pointer to a copy of the derived object upcast to a
  // ComponentShape object. The class that uses this function is responsible
  // for deleting the pointer. This is used to implement a virtual copy
  // constructor.
  virtual ComponentShape* clone() const = 0;

  // return the number of parameters in this shape and set/get them. The error
  // functions provide a way to set/get the error (nominally 1-sigma in an
  // implicit Gaussian distribution) in the corresponding parameter.
  // <group>
  virtual casacore::uInt nParameters() const = 0;
  virtual void setParameters(const casacore::Vector<casacore::Double>& newParms) = 0;
  virtual casacore::Vector<casacore::Double> parameters() const = 0;
  virtual void setErrors(const casacore::Vector<casacore::Double>& newErrs) = 0;
  virtual casacore::Vector<casacore::Double> errors() const = 0;
  virtual casacore::Vector<casacore::Double> optParameters() const = 0;
  virtual void setOptParameters(const casacore::Vector<casacore::Double>& newOptParms) = 0;
  // </group>

  // These functions convert between a record and a ComponentShape. This way
  // derived classes can interpret fields in the record in a class specific
  // way. They return false if the record is malformed and append an error
  // message to the supplied string giving the reason.  These functions define
  // how the shape is represented in glish. All records should have 'type' &
  // 'direction' fields which contain respectively; a string indicating which
  // shape is actually used, and a record representation of a direction
  // measure.  The interpretation of all other fields depends on the specific
  // component shape used.
  // <group>
  virtual casacore::Bool fromRecord(casacore::String& errorMessage, 
			  const casacore::RecordInterface& record) = 0;
  virtual casacore::Bool toRecord(casacore::String& errorMessage,
			casacore::RecordInterface& record) const = 0;
  // </group>

  // Convert the parameters of the shape to the specified units. The Record
  // must contain the same fields that the to/from casacore::Record functions have (with
  // the exception of the direction & type fields). These fields will contain
  // strings (and not record representations of Quantums) that specify the new
  // units for these parameters. The new units must have the same dimensions as
  // the existing ones. If there is any problem parsing the record then an
  // error message is appended to the supplied string and the function returns
  // false.
  virtual casacore::Bool convertUnit(casacore::String& errorMessage,
			   const casacore::RecordInterface& record) = 0;
  
  // Return the shape that the supplied record represents. The
  // shape is determined by parsing a 'type' field in the supplied
  // record. Returns ComponentType::UNKNOWN_SHAPE if the type field
  // (which contains a string) could not be translated into a known
  // shape. It then appends an appropriate error message to the errorMessage
  // String.
  static ComponentType::Shape getType(casacore::String& errorMessage,
				      const casacore::RecordInterface& record);

  // Convert component shape to absolute pixels.  The returned
  // vector is the longitude and latitude location in absolute pixels.
  virtual casacore::Vector<casacore::Double> toPixel (const casacore::DirectionCoordinate& dirCoord) const;

  // Fill the shape direction from the vector (longitude and latitude
  // in absolute pixels).  The return value is always true.
  virtual casacore::Bool fromPixel (const casacore::Vector<casacore::Double>& parameters,
                          const casacore::DirectionCoordinate& dirCoord);

  // casacore::Function which checks the internal data of this class for correct
  // dimensionality and consistant values. Returns true if everything is fine
  // otherwise returns false.
  virtual casacore::Bool ok() const;

  // Return a pointer to the object. All subclasses must implement. 
  virtual const ComponentShape* getPtr() const = 0;

  // Return a nicely formatted string describing the component's size.
  // All subclasses must implement.
  virtual casacore::String sizeToString() const = 0;

protected:
  // The constructors and assignment operator are protected as only derived
  // classes should use them.
  // <group>
  //# The default ComponentShape direction is at the J2000 North Pole.
  ComponentShape();

  //# Construct a ComponentShape at the specified direction.
  ComponentShape(const casacore::MDirection& direction);

  //# The copy constructor uses copy semantics.
  ComponentShape(const ComponentShape& other);

  //# The assignment operator uses copy semantics.
  ComponentShape& operator=(const ComponentShape& other);
  // </group>

  //# Try and decide if the two reference directions are different, as the
  //# casacore::MeasRef<T>::operator== function is too restrictive.
  static casacore::Bool differentRefs(const casacore::MeasRef<casacore::MDirection>& ref1,
			    const casacore::MeasRef<casacore::MDirection>& ref2);

  // returns true if the quantum is not a non-negative angular quantity
  static casacore::Bool badError(const casacore::Quantum<casacore::Double>& quantum);

  // Turns the specified field in the specified record into an casacore::Quantum 
  // with angular units
  static casacore::Bool fromAngQRecord(casacore::Quantum<casacore::Double>& returnValue, 
			     casacore::String& errorMessage,
			     const casacore::String& fieldString, 
			     const casacore::RecordInterface& record);

private:
  //# The reference direction of the component
  casacore::MDirection itsDir;
  //# The errors in the reference direction of the component in radians
  casacore::Quantum<casacore::Double> itsDirErrLat;
  casacore::Quantum<casacore::Double> itsDirErrLong;
};

} //# NAMESPACE CASA - END

#endif
