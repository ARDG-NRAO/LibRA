//# SkyComponent.h: this defines SkyComponent.h
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2003
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
//# $Id: SkyComponent.h 21292 2012-11-28 14:58:19Z gervandiepen $

#ifndef COMPONENTS_SKYCOMPONENT_H
#define COMPONENTS_SKYCOMPONENT_H

#include <casa/aips.h>
#include <casa/Utilities/CountedPtr.h>
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <components/ComponentModels/ComponentType.h>
#include <components/ComponentModels/SkyCompBase.h>
#include <measures/Measures/Stokes.h>
#include <casacore/casa/Arrays/ArrayFwd.h>

namespace casacore{

class MDirection;
class MFrequency;
class MVAngle;
class MVDirection;
class MVFrequency;
class RecordInterface;
class String;
class Unit;
class GaussianBeam;
template<class Ms> class MeasRef;
template<class T> class Quantum;
}

namespace casa { //# NAMESPACE CASA - BEGIN

class ComponentShape;
class SkyCompRep;
class SpectralModel;
template<class T> class Flux;

// <summary>A component of a model of the sky </summary>

// <use visibility=export>
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite> 
// <li> <linkto class=Flux>Flux</linkto>
// <li> <linkto class=ComponentShape>ComponentShape</linkto>
// <li> <linkto class=SpectralModel>SpectralModel</linkto>
// </prerequisite>

// <synopsis> 

// This class is concrete implementation of a class that represents a component
// of a model of the sky brightness. 

// The base class (<linkto class="SkyCompBase">SkyCompBase</linkto>) contains a
// description of components and all the member functions used to manipulate
// them and hence will not be discussed here. But the base class does not
// include any constructors or a description of the copy semantics. This will
// be discussed below.

// A SkyComponent is an "envelope" class in the sense that it can contain one
// of a variety of different component shapes and spectral models. It is
// necessary to specify the which shape and spectral model you want at
// construction time. This can be done either with enumerators or by
// constructing the classes derived from ComponentShape & SpectralModel and
// supplying them as construction arguments.

// This class uses reference semantics for both the copy constructor and the
// assignment operator. Becuase of this the only way to make a true copy of a
// SkyComponent is to use the <src>copy</src> member function.

// </synopsis>

// <example>
// These examples are coded in the tSkyCompRep.h file.
// <h4>Example 1:</h4>
// In this example a SkyCompRep object is created and used to calculate the
// ...
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Model fitting is an important part of astronomical data
// reduction/interpretation. This class defines a model component. Many
// components can be strung together (using the ComponentList class) to
// construct a model. It is expected that this class will eventually allow you
// to solve for parameters of the model.
// </motivation>

// <thrown>
// <li> casacore::AipsError - If an internal inconsistancy is detected, when compiled in 
// debug mode only.
// </thrown>
//
// <todo asof="1998/05/22">
//   <li> Add time variability
//   <li> Add the ability to solve for component parameters.
// </todo>

// <linkfrom anchor="SkyComponent" classes="SkyCompRep SkyCompBase">
//  <here>SkyComponent</here> - Models the sky brightness (reference semantics)
// </linkfrom>
 
class SkyComponent: public SkyCompBase
{
public:
  // The default SkyComponent is a point source with a constant spectrum. See
  // the default constructors in the PointShape, ConstantSpectrum and Flux
  // classes for the default values for the flux, shape and spectrum.
  SkyComponent();

  // Construct a SkyCompRep of the specified shape. The resultant component
  // has a constant spectrum and a shape given by the default constructor of
  // the specified ComponentShape class.
  SkyComponent(const ComponentType::Shape & shape);
  
  // Construct a SkyCompRep with the user specified model for the shape and
  // spectrum. The resultant component has a shape given by the default
  // constructor of the specified ComponentShape class and a spectrum given by
  // the default constructor of the specified SpectralModel class
  SkyComponent(const ComponentType::Shape & shape, 
	       const ComponentType::SpectralShape & spectralModel);
  
  // Construct a SkyComponent with a fully specified model for the shape, 
  // spectrum and flux.
  SkyComponent(const Flux<casacore::Double> & flux,
	       const ComponentShape & shape, 
	       const SpectralModel & spectrum);

  // The copy Constructor uses reference semantics
  SkyComponent(const SkyComponent & other);

  // the destructor does nothing obvious (its all done by the casacore::CountedPtr)
  virtual ~SkyComponent();

  // The assignment operator uses reference semantics
  SkyComponent & operator=(const SkyComponent & other);

  // See the corresponding functions in the
  // <linkto class="SkyCompBase">SkyCompBase</linkto>
  // class for a description of these functions.
  // <group>
  virtual Flux<casacore::Double> & flux();
  virtual const Flux<casacore::Double> & flux() const;
  // </group>

  // See the corresponding functions in the
  // <linkto class="SkyCompBase">SkyCompBase</linkto>
  // class for a description of these functions.
  // <group>
  virtual const ComponentShape & shape() const;
  virtual ComponentShape & shape();
  virtual void setShape(const ComponentShape & newShape);
  // </group>
  
  // See the corresponding functions in the
  // <linkto class="SkyCompBase">SkyCompBase</linkto>
  // class for a description of these functions.
  // <group>
  virtual const SpectralModel & spectrum() const;
  virtual SpectralModel & spectrum();
  virtual void setSpectrum(const SpectralModel & newSpectrum);
  // </group>
  
  // See the corresponding functions in the
  // <linkto class="SkyCompBase">SkyCompBase</linkto>
  // class for a description of these functions.
  // <group>
  virtual casacore::String & label();
  virtual const casacore::String & label() const;
  // </group>

  // See the corresponding functions in the
  // <linkto class="SkyCompBase">SkyCompBase</linkto>
  // class for a description of these functions.
  // <group>
  virtual casacore::Vector<casacore::Double> & optionalParameters();
  virtual const casacore::Vector<casacore::Double> & optionalParameters() const;
  // </group>

  // See the corresponding function in the
  // <linkto class="SkyCompBase">SkyCompBase</linkto>
  // class for a description of this function.
  virtual casacore::Bool isPhysical() const;
  
  // See the corresponding functions in the
  // <linkto class="SkyCompBase">SkyCompBase</linkto>
  // class for a description of this function.
  virtual Flux<casacore::Double> sample(const casacore::MDirection & direction, 
			      const casacore::MVAngle & pixelLatSize, 
			      const casacore::MVAngle & pixelLongSize, 
			      const casacore::MFrequency & centerFrequency) const;

  // See the corresponding function in the
  // <linkto class="SkyCompBase">SkyCompBase</linkto>
  // class for a description of this function.
  virtual void sample(casacore::Cube<casacore::Double>& samples,
		      const casacore::Unit& reqUnit,
		      const casacore::Vector<casacore::MVDirection>& directions, 
		      const casacore::MeasRef<casacore::MDirection>& dirRef, 
		      const casacore::MVAngle& pixelLatSize, 
		      const casacore::MVAngle& pixelLongSize, 
		      const casacore::Vector<casacore::MVFrequency>& frequencies,
		      const casacore::MeasRef<casacore::MFrequency>& freqRef) const;

  // See the corresponding function in the
  // <linkto class="SkyCompBase">SkyCompBase</linkto>
  // class for a description of this function.
  virtual Flux<casacore::Double> visibility(const casacore::Vector<casacore::Double> & uvw,
				  const casacore::Double & frequency) const;

  // See the corresponding function in the
  // <linkto class="SkyCompBase">SkyCompBase</linkto>
  // class for a description of this function.
  virtual void visibility(casacore::Cube<casacore::DComplex>& visibilities,
			  const casacore::Matrix<casacore::Double>& uvws,
			  const casacore::Vector<casacore::Double>& frequencies) const;

  // See the corresponding functions in the
  // <linkto class="SkyCompBase">SkyCompBase</linkto>
  // class for a description of these functions.
  // <group>
  virtual casacore::Bool fromRecord(casacore::String & errorMessage, 
			  const casacore::RecordInterface & record);
  virtual casacore::Bool toRecord(casacore::String & errorMessage, 
			casacore::RecordInterface & record) const;
  // </group>

  // Return a distinct copy of this component. As both the assignment operator
  // and the copy constructor use reference semantics this is the only way to
  // get a real copy.
  SkyComponent copy() const;

  // See the corresponding functions in the
  // <linkto class="SkyCompBase">SkyCompBase</linkto>
  // class for a description of this function.
  casacore::Bool ok() const;

  // Convert from and to pixel vectors. See the SkyCompRep class for details.
  // <group>
   casacore::Vector<casacore::Double> toPixel (const casacore::Unit& brightnessUnitIn,
                           const casacore::GaussianBeam& restoringBeam,
                           const casacore::CoordinateSystem& cSys,
                           casacore::Stokes::StokesTypes stokes) const;
  void fromPixel (casacore::Double& fluxRatio, const casacore::Vector<casacore::Double>& parameters,
                  const casacore::Unit& brightnessUnitIn,
                  const casacore::GaussianBeam& restoringBeam,
                  const casacore::CoordinateSystem& cSys,
                  ComponentType::Shape componentShape,
                  casacore::Stokes::StokesTypes stokes);

  // </group>

  // Get a nicely formatted string summarizing the component. If longErrOnGreatCircle is
  // true, the longitude error is interpreted as lying
  // along the great circle that is tangent to the center position, rather than along the
  // circle of constant longitude. In this case, the longitude error presented as a time has
  // been multiplied by 1/cos(latitude), but the associated angle reported as an angular measure
  // will not have this factor applied to it, nor will the error reported in pixels.
  virtual casacore::String summarize(
		 const casacore::DirectionCoordinate *const &dc = 0, casacore::Bool longErrOnGreatCircle=false
  ) const;

  // If the <src>dc</src> parameter is specified, also return the corresponding world
  // coordinates, and the pixel coordinates will be returned in the <src>pixelCoords</src>
  // parameter (otherwise it is set to nullptr). If longErrOnGreatCircle is true, the
  // longitude error is interpreted as lying along the great circle that is tangent to the center
  // position, rather than along the circle of constant latitude. In this case, the longitude error
  // presented as a time has been multiplied by 1/cos(latitude), but the associated angle reported
  // as an angular measure will not have this factor applied to it, nor will the error reported in
  // pixels.
  casacore::String positionToString(
          std::shared_ptr<casacore::Vector<casacore::Double>>& pixelCoords,
		  const casacore::DirectionCoordinate *const &dc=nullptr, casacore::Bool longErrOnGreatCircle=false
  ) const;

private:
  casacore::CountedPtr<SkyCompRep> itsCompPtr;

  static std::pair<casacore::String, casacore::String> _axisLabels(
		const casacore::DirectionCoordinate *const &dc
  );

};

} //# NAMESPACE CASA - END

#endif
