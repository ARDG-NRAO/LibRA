//# Flux.h:
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
//#
//# $Id: Flux.h 21130 2011-10-18 07:39:05Z gervandiepen $

#ifndef COMPONENTS_FLUX_H
#define COMPONENTS_FLUX_H

#include <casa/aips.h>
#include <casa/Arrays/Vector.h>
#include <scimath/Mathematics/NumericTraits.h>
#include <measures/Measures/Stokes.h>
#include <casa/Quanta/Unit.h>
#include <casa/Utilities/CountedPtr.h>
#include <components/ComponentModels/ComponentType.h>

namespace casacore{

class RecordInterface;
template <class Qtype> class Quantum;
}

namespace casa { //# NAMESPACE CASA - BEGIN


// <summary>A class that represents the Flux (copy semantics)</summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class="casacore::Vector">casacore::Vector</linkto>
//   <li> <linkto class="casacore::Unit">casacore::Unit</linkto>
// </prerequisite>
//
// <etymology>
// This class actually contains the flux values as distinct from the Flux class
// which is a pointer to this class. Hence this class name is a shortening of
// "Flux Representation". The Flux class is probably of more general user
// utility as its reference semantics can cut down on the number of temporary
// objects that get constructed.
// </etymology>
//
// <synopsis>
// This class encapsulates three quantities that are needed to more completely
// represent the polarised flux of a component. These quantities are:
// <dl>
// <dt> Flux values.
// <dd> These are four numbers which are the numerical value of the flux in all
//      four polarisations. The values can be represented in either single or
//      double precision depending on the template type.
// <dt> Flux units.
// <dd> These are the units for the flux values. The units must have dimensions
//      of W/m^2/Hz and are represented using the
//      <linkto class="casacore::Unit">casacore::Unit</linkto> class. The most common unit is "Jy".
// <dt> Polarisation representation.
// <dd> This describes how the polarised flux is represented. It can be one of
//      the following:
//      <dl compact><dt>casacore::Stokes<dd>
//      The flux is representing using the casacore::Stokes I, Q, U, V components
//      respectively.
//      <dt>Linear<dd>
//      The flux is representing using the XX,XY,YX,YY correlation products
//      resp. X and Y is the signal from linear feeds at zero parallactic
//      angle.
//      <dt>Circular<dd>
//      The flux is representing using the RR,RL,LR,LL correlation products
//      resp. R and L is the signal from right and left handed circular feeds.
//      </dl>
// </dl>
// This class is templated but only two templated types are valid. These are:
// <ul> 
// <li> T = Float
// <li> T = Double
// </ul>
// The template type defines the precision of the Flux Values.
//
// This class uses functions in the Flux class which convert between the
// different polarisation representations. They assume (until a decision has
// been made on this) that the total intensity is the average of the linear or
// circular correlation products. ie., <src>I = (RR+LL)/2 = (XX+YY)/2</src>.
//
// In order to represent the Flux using the circular or linear representations
// the Flux values need to be complex (eg.,<src>XY = I + iV, YX = I - iV</src>)
// This class does not require, or check a number of constraints such as
// <src>XY = conj(YX)</src> and hence it is possible to define a flux using a
// linear or circular representation that cannot be completely represented
// using the casacore::Stokes representation. Because this class stores the flux values
// as complex numbers there is no loss of accuracy when converting between
// different polarisation representations. But it discards the imaginary
// component of the flux when externally representing the flux using with a
// casacore::Stokes representation (eg., when calling the <src>value(casacore::Vector<T>&)</src>
// function).
//
// Because this class using casacore::Complex numbers with a precision that depends on
// the template type many of the function arguments are of type
// <src>casacore::NumericTraits<T></src>. This simply a type that maps to casacore::Complex if T is
// casacore::Float and casacore::DComplex if T is a Double. Because of problems with the the gnu
// compiler functions which use this type as an argument MUST be
// inline. Hopefully this problem will go away sometime.
// </synopsis>
//
// <example>
// The following example creates a FluxRep object using a casacore::Stokes representation
// and converts it to "WU" (Westerbork Units). After printing out the converted
// I flux it converts the Flux to a linear representation and prints out a
// casacore::Vector with the [XX,XY,YX,YY] values (still in "WU")
// <srcblock>
// FluxRep<casacore::Double> flux(1.0, 0.0, 0.0, 0.1); // I = 1.0, V = 0.1
// flux.convertUnit("WU");
// cout << "The I flux (in WU is)" << flux.value(0) << endl;
// flux.convertPol(ComponentType::LINEAR);
// cout << "The XX,XY,YX,YY flux (in WU is)" << flux.value() << endl;
// </srcblock>
// </example>
//
// <motivation>
// This class was needed to contain the flux in the ComponentModels module and
// centralizes a lot of code that would otherwise be duplicated in disparate
// places. 
// </motivation>
//
// <thrown>
//    <li> casacore::AipsError, When the Vectors are not of length 4 or when indices are
//          greater than 4
// </thrown>
//
// <todo asof="1998/03/11">
//   <li> get this class reviewed.
// </todo>

template<class T> class FluxRep
{
public:

  // The default constructor makes an object with <src>I = 1, Q=U=V=0</src>,
  // a casacore::Stokes representation, and units of "Jy".
  FluxRep();

  // This constructor makes an object where I is specified and
  // <src>Q=U=V=0</src>. It assumes a casacore::Stokes representation, and units of "Jy".
  FluxRep(T i);

  // This constructor makes an object where I,Q,U,V are all specified. It
  // assumes a casacore::Stokes representation, and units of "Jy".
  FluxRep(T i, T q, T u, T v);

  // This constructor makes an object where the flux values and polarisation
  // representation are specified. It assumes the units are "Jy".
  FluxRep(typename casacore::NumericTraits<T>::ConjugateType xx,
	  typename casacore::NumericTraits<T>::ConjugateType xy,
	  typename casacore::NumericTraits<T>::ConjugateType yx,
	  typename casacore::NumericTraits<T>::ConjugateType yy,
	  ComponentType::Polarisation pol);
  
  // This constructor makes an object where I,Q,U,V are all specified by a
  // casacore::Vector that must have four elements. It assumes a casacore::Stokes representation,
  // and units of "Jy".
  FluxRep(const casacore::Vector<T>& flux);

  // This constructor makes an object where the flux values are all specified
  // by a casacore::Vector that must have four elements. The polarisation representation
  // must also be specified. It assumes the units are "Jy".
  FluxRep(const casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& flux,
	  ComponentType::Polarisation pol);
  
  // This constructor makes an object where the flux values are all specified
  // by a casacore::Quantum<casacore::Vector> that must have four elements.  The casacore::Quantum must have
  // units that are dimensionally equivalent to the "Jy" and these are the
  // units of the FluxRep object. A casacore::Stokes polarisation representation is
  // assumed.
  FluxRep(const casacore::Quantum<casacore::Vector<T> >& flux);

  // This constructor makes an object where the flux values are all specified
  // by a casacore::Quantum<casacore::Vector> that must have four elements. The casacore::Quantum must have
  // units that are dimensionally equivalent to the "Jy" and these are the
  // units of the FluxRep object. The polarisation representation must also be
  // specified.
  FluxRep(const
	  casacore::Quantum<casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType> >& flux,
	  ComponentType::Polarisation pol);

  // The copy constructor uses copy semantics.
  FluxRep(const FluxRep<T>& other);

  // The destructor is trivial
  ~FluxRep();

  // The assignment operator uses copy semantics.
  FluxRep<T>& operator=(const FluxRep<T>& other);

  // These two functions return the current units
  // <group>
  const casacore::Unit& unit() const;
  void unit(casacore::Unit& unit) const;
  // </group>

  // This function sets the current unit. It does NOT convert the flux values
  // to correspond to the new unit. The new unit must be dimensionally
  // equivalent to the "Jy".
  void setUnit(const casacore::Unit& unit);

  // This function sets the current units to the supplied value and
  // additionally converts the internal flux values to the correspond to the
  // new unit.
  void convertUnit(const casacore::Unit& unit);

  // These two functions return the current polarisation representation.
  // <group>
  ComponentType::Polarisation pol() const;
  void pol(ComponentType::Polarisation& pol) const;
  // </group>

  // This function sets the current polarisation representation. It does NOT
  // convert the flux values.
  void setPol(ComponentType::Polarisation pol);

  // This function sets the current polarisation representation to the supplied
  // value and additionally converts the internal flux values to the correspond
  // to the new polarisation representation.
  void convertPol(ComponentType::Polarisation pol);

  // This function returns the flux values. The polarisation representation and
  // units are in whatever is current.
  const casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& value() const;

  // This function returns the specified component of the flux values.
  // The polarisation representation and units are in whatever is current.
  const typename casacore::NumericTraits<T>::ConjugateType& value(casacore::uInt p) const;

  // This function returns the flux values after converting it to the Stokes
  // representation. The units of the returned casacore::Vector are the current units.
  void value(casacore::Vector<T>& value);

  // This function returns the flux values. The polarisation representation and
  // units are in whatever is current.
  void value(casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& value) const;

  // This function returns the flux values after converting it to the Stokes
  // representation. The units of the returned casacore::Quantum are the current units of
  // the FluxRep object. The length of the casacore::Vector in the casacore::Quantum will be
  // resized to 4 elements if it is not already that length.
  void value(casacore::Quantum<casacore::Vector<T> >& value);

  // This function returns the flux values. The units of the returned Quantum
  // are the current units of the FluxRep object. Similarly the polarisation
  // representation of the returned casacore::Quantum is the current polarisation
  // representation. The length of the casacore::Vector in the casacore::Quantum will be resized to
  // 4 elements if it is not already that length.
  void value(casacore::Quantum
	     <casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType> >& value) const;

  // Return the flux value in a casacore::Quantum for the specified Stokes.  Can convert
  // to Jy if requested.  
  casacore::Quantum<T> value (casacore::Stokes::StokesTypes stokes, casacore::Bool toJy=true);

  // This function sets the Flux values assuming the supplied value represents
  // the casacore::Stokes I flux in the current units. The other casacore::Stokes parameters are
  // set to zero.
  void setValue(T value);
 
  // This function sets the Flux values assuming the supplied values represent
  // the flux in the casacore::Stokes representation and is in the current units. The
  // casacore::Vector must have four elements.
  void setValue(const casacore::Vector<T>& value); 

  // This function sets the Flux values assuming the supplied values represent
  // the flux in the current representation and units. The casacore::Vector must have
  // four elements.
  void setValue(const casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& value);

  // This function sets the flux values and units assuming the supplied values
  // represent the flux in the casacore::Stokes representation. The units of the Quantum
  // must be dimensionally equivalent to the "Jy" and the casacore::Vector must have four
  // elements.
  void setValue(const casacore::Quantum<casacore::Vector<T> >& value);

  // This function sets the flux values, units and polarisation assuming the
  // supplied values represent the flux in the specified representation. The
  // units of the casacore::Quantum must be dimensionally equivalent to the "Jy" and the
  // casacore::Vector must have four elements.
  void setValue(const
		casacore::Quantum<casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType> >& value,
		ComponentType::Polarisation pol);

  // Set flux for given casacore::Stokes from Quantum. 
  void setValue (const casacore::Quantum<T>& value, casacore::Stokes::StokesTypes stokes);

  // Scale the Flux value by the specified amount. These functions multiply the
  // flux values irrespective of the current polarisation representation. If
  // only one scale factor is supplied then only the first component of the
  // flux is scaled.
  // <group>
  void scaleValue(const T& factor);
  void scaleValue(const T& factor0, const T& factor1,
		  const T& factor2, const T& factor3);
  void scaleValue(const typename casacore::NumericTraits<T>::ConjugateType& factor);
  void scaleValue(const typename casacore::NumericTraits<T>::ConjugateType& factor0,
		  const typename casacore::NumericTraits<T>::ConjugateType& factor1,
		  const typename casacore::NumericTraits<T>::ConjugateType& factor2,
		  const typename casacore::NumericTraits<T>::ConjugateType& factor3);
  // </group>

  // Set/get the errors in the flux
  // <group>
  void setErrors(const typename casacore::NumericTraits<T>::ConjugateType& error0,
		 const typename casacore::NumericTraits<T>::ConjugateType& error1,
		 const typename casacore::NumericTraits<T>::ConjugateType& error2,
		 const typename casacore::NumericTraits<T>::ConjugateType& error3);

  void setErrors(const casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& errors);

  const casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& errors() const;
  // </group>

  // This functions convert between a casacore::RecordInterface and a FluxRep object and
  // define how the FluxRep is represented in glish.  They return false if the
  // casacore::RecordInterface is malformed and append an error message to the supplied
  // string giving the reason.
  // <group>
  casacore::Bool fromRecord(casacore::String& errorMessage, const casacore::RecordInterface& record);
  casacore::Bool toRecord(casacore::String& errorMessage, casacore::RecordInterface& record) const;
  // </group>

  // casacore::Function which checks the internal data of this class for correct
  // dimensionality and consistent values. Returns true if everything is fine
  // otherwise returns false.
  casacore::Bool ok() const;

private:
  casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType> itsVal;
  ComponentType::Polarisation itsPol;
  casacore::Unit itsUnit;
  casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType> itsErr;
  static const std::vector<casacore::Unit> _allowedUnits;

    // returns the conversion unit to which <src>unit</src> conforms.
    // If <src>unit</src> does not conform to a supported unit, an
    // exception is thrown.
    static casacore::Unit _getConversionUnit(const casacore::Unit& unit);

};

// <summary>A class that represents the Flux (reference semantics)</summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class="casacore::Vector">casacore::Vector</linkto>
//   <li> <linkto class="casacore::Unit">casacore::Unit</linkto>
//   <li> <linkto class="FluxRep">FluxRep</linkto>
// </prerequisite>
//
// <etymology>
// The Flux class is used to represent the flux of a component on the sky
// </etymology>
//
// <synopsis>

// This class is nearly identical to the <linkto
// class="FluxRep">FluxRep</linkto> class and the reader is referred to the
// documentation of this class for a general explanation of this class.  Most
// of the functions in this class just call the functions in the FluxRep class.

// There are two important differences between the Flux class and the FluxRep
// class. 
// <ul>
// <li> This class uses reference semantics rather than copy semantics. This
// aids in cutting down on the number of temporary objects that need to be
// constructed. An example of this is illustrated in the examples section
// below.
// <li> This class contains functions for converting between different
// polarisation representations. 
// </ul>
// The functions for converting between different polarisation representations
// require that the supplied and returned vector have all four polarisations.
// In the casacore::Stokes representation the order of the elements is I,Q,U,V, in the
// linear representation it is XX,XY,YX,YY, and in the circular representation
// it is RR,RL,LR,LL.
//
// These functions will correctly convert between Linear/Circular
// representations and the casacore::Stokes representation even if the linear or circular
// polarisation cannot represent a physically realisable polarisation (eg if
// <src>XY != conj(YX)</src>). In these cases the stokes representation will
// have an imaginary component and be complex. When converting the complex
// casacore::Stokes representation to a real one the imaginary components are simply
// discarded.
// </synopsis>
//
// <example>
// The function in this example calculates the total flux of all the
// components in a list. It accumulates the flux in a casacore::Vector after ensuring
// that the flux is in the appropriate units and Polarisation. It then returns
// the sum as a Flux object. Because this class uses reference semantics the
// returned object is passed by reference and hence this is a relatively cheap
// operation.
// <srcblock>
// Flux<casacore::Double> totalFlux(ComponentList& list) {
//   casacore::Vector<casacore::DComplex> sum(4, casacore::DComplex(0.0, 0.0));
//   for (casacore::uInt i = 0; i < list.nelements(); i++) {
//     list.component(i).flux().convertPol(ComponentType::STOKES);
//     list.component(i).flux().convertUnit("Jy");
//     sum += list.component(i).flux().value()
//   }
//   return Flux<casacore::Double>(value, ComponentType::STOKES);
// }
// </srcblock>
// </example>
//
// <motivation>
// This class was needed to contain the flux in the ComponentModels class. It
// centralizes a lot of code that would otherwise be duplicated. The reference
// semantics further simplify the interface that the component classes use.
// </motivation>
//
// <thrown>
//    <li> casacore::AipsError, When the Vectors are not of length 4 or when indices are
//          greater than 4
// </thrown>
//
// <todo asof="1998/03/11">
//   <li> get this class reviewed.
// </todo>

template<class T> class Flux
{
public:
  // The default constructor makes an object with <src>I = 1, Q=U=V=0</src>,
  // a casacore::Stokes representation, and units of "Jy".
  Flux();

  // The default constructor makes an object with <src>I = 1, Q=U=V=0</src>,
  // a casacore::Stokes representation, and units of "Jy".
   Flux(T i);

  // This constructor makes an object where I,Q,U,V are all specified. It
  // assumes a casacore::Stokes representation, and units of "Jy".
  Flux(T i, T q, T u, T v);

  // This constructor makes an object where the flux values and polarisation
  // representation are specified. It assumes the units are "Jy".
  Flux(typename casacore::NumericTraits<T>::ConjugateType xx,
       typename casacore::NumericTraits<T>::ConjugateType xy,
       typename casacore::NumericTraits<T>::ConjugateType yx,
       typename casacore::NumericTraits<T>::ConjugateType yy, 
       ComponentType::Polarisation pol);

  // This constructor makes an object where I,Q,U,V are all specified by a
  // casacore::Vector that must have four elements. It assumes a casacore::Stokes representation,
  // and units of "Jy".
  Flux(const casacore::Vector<T>& flux);

  // This constructor makes an object where the flux values are all specified
  // by a casacore::Vector that must have four elements. The polarisation representation
  // must also be specified. It assumes the units are "Jy".
  Flux(const casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& flux,
       ComponentType::Polarisation pol);
  
  // This constructor makes an object where the flux values are all specified
  // by a casacore::Quantum<casacore::Vector> that must have four elements.  The casacore::Quantum must have
  // units that are dimensionally equivalent to the "Jy" and these are the
  // units of the FluxRep object. A casacore::Stokes polarisation representation is
  // assumed.
  Flux(const casacore::Quantum<casacore::Vector<T> >& flux);

  // This constructor makes an object where the flux values are all specified
  // by a casacore::Quantum<casacore::Vector> that must have four elements. The casacore::Quantum must have
  // units that are dimensionally equivalent to the "Jy" and these are the
  // units of the FluxRep object. The polarisation representation must also be
  // specified.
  Flux(const casacore::Quantum<casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType> >& flux,
       ComponentType::Polarisation pol);

  // The copy constructor uses reference semantics.
  Flux(const Flux<T>& other);

  // The destructor is trivial
  ~Flux();

  // The assignment operator uses reference semantics.
  Flux<T>& operator=(const Flux<T>& other);

  // Return a distinct copy of this flux. As both the assignment operator
  // and the copy constructor use reference semantics this is the only way to
  // get a real copy.
  Flux<T> copy() const;

  // These two functions return the current units.
  // <group>
  const casacore::Unit& unit() const;
  void unit(casacore::Unit& unit) const;
  // </group>

  // This function sets the current unit. It does NOT convert the flux values
  // to correspond to the new unit. The new unit must be dimensionally
  // equivalent to the "Jy".
  void setUnit(const casacore::Unit& unit);

  // This function sets the current units to the supplied value and
  // additionally converts the internal flux values to the correspond to the
  // new unit.
  void convertUnit(const casacore::Unit& unit);

  // These two functions return the current polarisation representation.
  // <group>
  ComponentType::Polarisation pol() const;
  void pol(ComponentType::Polarisation& pol) const;
  // </group>

  // This function sets the current polarisation representation. It does NOT
  // convert the flux values.
  void setPol(ComponentType::Polarisation pol);

  // This function sets the current polarisation representation to the supplied
  // value and additionally converts the internal flux values to the correspond
  // to the new polarisation representation.
  void convertPol(ComponentType::Polarisation pol);

  // This function returns the flux values. The polarisation representation and
  // units are in whatever is current.
  const casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& value() const;

  // This function returns the specified component of the flux values.
  // The polarisation representation and units are in whatever is current.
  const typename casacore::NumericTraits<T>::ConjugateType& value(casacore::uInt p) const;

  // This function returns the flux values after converting it to the Stokes
  // representation. The units of the returned casacore::Vector are the current units.
  void value(casacore::Vector<T>& value);

  // This function returns the flux values. The polarisation representation and
  // units are in whatever is current.
  void value(casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& value) const;

  // This function returns the flux values after converting it to the Stokes
  // representation. The units of the returned casacore::Quantum are the current units of
  // the FluxRep object. The length of the casacore::Vector in the casacore::Quantum will be
  // resized to 4 elements if it is not already that length.
  void value(casacore::Quantum<casacore::Vector<T> >& value);

  // This function returns the flux values. The units of the returned Quantum
  // are the current units of the FluxRep object. Similarly the polarisation
  // representation of the returned casacore::Quantum is the current polarisation
  // representation. The length of the casacore::Vector in the casacore::Quantum will be resized to
  // 4 elements if it is not already that length.
  void value(casacore::Quantum
	     <casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType> >& value) const;

  // Return the flux value in a casacore::Quantum for the specified Stokes. Can convert
  // to Jy if requested.  
   casacore::Quantum<T> value (casacore::Stokes::StokesTypes stokes, casacore::Bool toJy=true);

  // This function sets the Flux values assuming the supplied value represents
  // the casacore::Stokes I flux in the current units. The other casacore::Stokes parameters are
  // set to zero.
  void setValue(T value);

  // This function sets the Flux values assuming the supplied values represent
  // the flux in the casacore::Stokes representation and is in the current units. The
  // casacore::Vector must have four elements.
  void setValue(const casacore::Vector<T>& value); 

  // This function sets the Flux values assuming the supplied values represent
  // the flux in the current representation and units. The casacore::Vector must have
  // four elements.
  void setValue(const casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& value);

  // This function sets the flux values and units assuming the supplied values
  // represent the flux in the casacore::Stokes representation. The units of the Quantum
  // must be dimensionally equivalent to the "Jy" and the casacore::Vector must have four
  // elements.
  void setValue(const casacore::Quantum<casacore::Vector<T> >& value);

  // This function sets the flux values, units and polarisation assuming the
  // supplied values represent the flux in the specified representation. The
  // units of the casacore::Quantum must be dimensionally equivalent to the "Jy" and the
  // casacore::Vector must have four elements.
  void setValue(const
		casacore::Quantum<casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType> >& value,
		ComponentType::Polarisation pol);

// Set flux for given casacore::Stokes from Quantum. 
  void setValue (const casacore::Quantum<T>& value, casacore::Stokes::StokesTypes stokes);

  // Scale the Flux value by the specified amount. These functions multiply the
  // flux values irrespective of the current polarisation representation. If
  // only one scale factor is supplied then only the first component of the
  // flux is scaled.
  // <group>
  void scaleValue(const T& factor);
  void scaleValue(const T& factor0, const T& factor1,
		  const T& factor2, const T& factor3);
  void scaleValue(const typename casacore::NumericTraits<T>::ConjugateType& factor);
  void scaleValue(const typename casacore::NumericTraits<T>::ConjugateType& factor0,
		  const typename casacore::NumericTraits<T>::ConjugateType& factor1,
		  const typename casacore::NumericTraits<T>::ConjugateType& factor2,
		  const typename casacore::NumericTraits<T>::ConjugateType& factor3);
  // </group>

  // Set/get the errors in the flux
  // <group>
  void setErrors(const typename casacore::NumericTraits<T>::ConjugateType& error0,
		 const typename casacore::NumericTraits<T>::ConjugateType& error1,
		 const typename casacore::NumericTraits<T>::ConjugateType& error2,
		 const typename casacore::NumericTraits<T>::ConjugateType& error3);

  void setErrors(const casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& errors);

  const casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& errors() const;
  // </group>

  // This functions convert between a casacore::RecordInterface and a Flux object and
  // define how the Flux is represented in glish.  They return false if the
  // casacore::RecordInterface is malformed and append an error message to the supplied
  // string giving the reason.
  // <group>
  casacore::Bool fromRecord(casacore::String& errorMessage, const casacore::RecordInterface& record);
  casacore::Bool toRecord(casacore::String& errorMessage, casacore::RecordInterface& record) const;
  // </group>

  // casacore::Function which checks the internal data of this class for correct
  // dimensionality and consistent values. Returns true if everything is fine
  // otherwise returns false.
  casacore::Bool ok() const;

  // This function converts between a casacore::Vector in casacore::Stokes representation and one
  // in Circular representation.
  static void 
    stokesToCircular(casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& out, 
		     const casacore::Vector<T>& in);

  // This function converts between a casacore::Vector in casacore::Stokes representation and one
  // in Circular representation. The imaginary components of the casacore::Stokes vector
  // are NOT ignored.
  static void 
    stokesToCircular(casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& out, 
		     const
		     casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& in);

  // This function converts between a casacore::Vector in Circular representation and one
  // in casacore::Stokes representation. The imaginary components of the casacore::Stokes vector
  // are discarded.
  static 
  void circularToStokes(casacore::Vector<T>& out,
			const
			casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& in);

  // This function converts between a casacore::Vector in Circular representation and one
  // in casacore::Stokes representation. The imaginary components of the casacore::Stokes vector
  // are NOT ignored.
  static 
  void circularToStokes(casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& out,
			const
			casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& in);

  // This function converts between a casacore::Vector in casacore::Stokes representation and one
  // in Linear representation.
  static void
    stokesToLinear(casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& out, 
		   const casacore::Vector<T>& in);

  // This function converts between a casacore::Vector in casacore::Stokes representation and one
  // in Linear representation. The imaginary components of the casacore::Stokes vector
  // are NOT ignored.
  static 
  void stokesToLinear(casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& out, 
		      const
		      casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& in);

  // This function converts between a casacore::Vector in Linear representation and one
  // in casacore::Stokes representation. The imaginary components of the casacore::Stokes vector
  // are discarded.
  static void
    linearToStokes(casacore::Vector<T>& out, 
		   const
		   casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& in);

  // This function converts between a casacore::Vector in Linear representation and one
  // in casacore::Stokes representation. The imaginary components of the casacore::Stokes vector
  // are NOT ignored.
  static void
    linearToStokes(casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& out, 
		   const casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& in);

  // This function converts between a casacore::Vector in Linear representation and one
  // in Circular representation.
  static void
    linearToCircular(casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& out, 
		     const
		     casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& in);

  // This function converts between a casacore::Vector in Circular representation and one
  // in Linear representation.
  static void
    circularToLinear(casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& out, 
		     const
		     casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& in);

private:
  casacore::CountedPtr<FluxRep<T> > itsFluxPtr;
};


} //# NAMESPACE CASA - END

#ifndef CASACORE_NO_AUTO_TEMPLATES
#include <components/ComponentModels/Flux.tcc>
#endif //# CASACORE_NO_AUTO_TEMPLATES
#endif
