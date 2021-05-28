//# SkyCompRep.h: A model component of the sky brightness
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002
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
//# $Id: SkyCompRep.h 21292 2012-11-28 14:58:19Z gervandiepen $

#ifndef COMPONENTS_SKYCOMPREP_H
#define COMPONENTS_SKYCOMPREP_H

#include <casa/aips.h>
#include <components/ComponentModels/ComponentType.h>
#include <components/ComponentModels/Flux.h>
#include <components/ComponentModels/SkyCompBase.h>
#include <casa/Utilities/CountedPtr.h>
#include <casa/BasicSL/String.h>
#include <measures/Measures/Stokes.h>
#include <casacore/casa/Arrays/ArrayFwd.h>

namespace casacore{

class CoordinateSystem;
class DirectionCoordinate;
class LogIO;
class MDirection;
class MFrequency;
class MVAngle;
class MVDirection;
class MVFrequency;
class RecordInterface;
class Unit;
class GaussianBeam;
template <class Ms> class MeasRef;
template <class T> class Quantum;
}

namespace casa { //# NAMESPACE CASA - BEGIN

class ComponentShape;
class SpectralModel;
class TwoSidedShape;



// <summary>A model component of the sky brightness</summary>

// <use visibility=export>
// <reviewed reviewer="" date="yyyy/mm/dd" tests="tSkyCompRep" demos="">
// </reviewed>

// <prerequisite> 
// <li> <linkto class=Flux>Flux</linkto>
// <li> <linkto class=ComponentShape>ComponentShape</linkto>
// <li> <linkto class=SpectralModel>SpectralModel</linkto>
// </prerequisite>
//
// <synopsis> 

// This class is concrete implementation of a class that represents a component
// of a model of the sky brightness. 

// The base class (<linkto class="SkyCompBase">SkyCompBase</linkto>) contains a
// description of components and all the member functions used to manipulate
// them and hence will not be discussed here. But the base class does not
// include any constructors or a description of the copy semantics. This will
// be discussed below.

// A SkyCompRep is an "envelope" class in the sense that it can contain one of
// a variety of different component shapes and spectral models. It is necessary
// to specify the which shape and spectral model you want at construction
// time. This can be done either with enumerators or by constructing the
// classes derived from ComponentShape & SpectralModel and supplying them as
// construction arguments.

// This class uses copy semantics for both the copy constructor and the
// assignment operator. 

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

// <linkfrom anchor="SkyCompRep" classes="SkyComponent SkyCompBase">
//  <here>SkyCompRep</here> - Models the sky brightness (copy semantics)
// </linkfrom>
 
class SkyCompRep: public SkyCompBase
{
public:
  // The default SkyCompRep is a point source with a constant spectrum. See
  // the default constructors in the PointShape, ConstantSpectrum and Flux
  // classes for the default values for the flux, shape and spectrum.
  SkyCompRep();
  
  // Construct a SkyCompRep of the specified shape. The resultant component
  // has a constant spectrum and a shape given by the default constructor of
  // the specified ComponentShape class.
  // <thrown>
  // <li> casacore::AipsError - if the shape is UNKNOWN_SHAPE or NUMBER_SHAPES
  // </thrown>
  SkyCompRep(const ComponentType::Shape& shape);
  
  // Construct a SkyCompRep with the user specified model for the shape and
  // spectrum. The resultant component has a shape given by the default
  // constructor of the specified ComponentShape class and a spectrum given by
  // the default constructor of the specified SpectralModel class
  // <thrown>
  // <li> casacore::AipsError - if the shape is UNKNOWN_SHAPE or NUMBER_SHAPES
  // <li> casacore::AipsError - if the spectrum is UNKNOWN_SPECTRAL_SHAPE or
  //                  NUMBER_SPECTRAL_SHAPES
  // </thrown>
  SkyCompRep(const ComponentType::Shape& shape,
 	     const ComponentType::SpectralShape& spectrum);

  // Construct a SkyCompRep with a fully specified model for the shape, 
  // spectrum and flux.
  SkyCompRep(const Flux<casacore::Double>& flux,
 	     const ComponentShape& shape, 
 	     const SpectralModel& spectrum);
  
  // The copy constructor uses copy semantics
  SkyCompRep(const SkyCompRep& other);
  
  // The destructor does not appear to do much
  virtual ~SkyCompRep();

  // The assignment operator uses copy semantics.
  SkyCompRep& operator=(const SkyCompRep& other);

  // See the corresponding functions in the
  // <linkto class="SkyCompBase">SkyCompBase</linkto>
  // class for a description of these functions.
  // <group>
  virtual const Flux<casacore::Double>& flux() const;
  virtual Flux<casacore::Double>& flux();
  // </group>

  // See the corresponding functions in the
  // <linkto class="SkyCompBase">SkyCompBase</linkto>
  // class for a description of these functions.
  // <group>
  virtual const ComponentShape& shape() const;
  virtual ComponentShape& shape();
  virtual void setShape(const ComponentShape& newShape);
  // </group>
  
  // See the corresponding functions in the
  // <linkto class="SkyCompBase">SkyCompBase</linkto>
  // class for a description of these functions.
  // <group>
  virtual const SpectralModel& spectrum() const;
  virtual SpectralModel& spectrum();
  virtual void setSpectrum(const SpectralModel& newSpectrum);
  // </group>
  
  // See the corresponding functions in the
  // <linkto class="SkyCompBase">SkyCompBase</linkto>
  // class for a description of these functions.
  // <group>
  virtual casacore::String& label();
  virtual const casacore::String& label() const;
  // </group>

  // See the corresponding functions in the
  // <linkto class="SkyCompBase">SkyCompBase</linkto>
  // class for a description of these functions.
  // <group>
  virtual casacore::Vector<casacore::Double>& optionalParameters();
  virtual const casacore::Vector<casacore::Double>& optionalParameters() const;
  // </group>

  // See the corresponding function in the
  // <linkto class="SkyCompBase">SkyCompBase</linkto>
  // class for a description of this function.
  virtual casacore::Bool isPhysical() const;
  
  // See the corresponding function in the
  // <linkto class="SkyCompBase">SkyCompBase</linkto>
  // class for a description of this function.
  virtual Flux<casacore::Double> sample(const casacore::MDirection& direction, 
			      const casacore::MVAngle& pixelLatSize,
			      const casacore::MVAngle& pixelLongSize,
			      const casacore::MFrequency& centerFrequency) const;

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
  virtual Flux<casacore::Double> visibility(const casacore::Vector<casacore::Double>& uvw,
				  const casacore::Double& frequency) const;

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
  virtual casacore::Bool fromRecord(casacore::String& errorMessage, 
			  const casacore::RecordInterface& record);
  virtual casacore::Bool toRecord(casacore::String& errorMessage, 
			casacore::RecordInterface& record) const;
  // </group>

  // Convert the SkyComponent to a vector of Doubles 
  // for the specified casacore::Stokes type (others are lost).
  // The first three elements of the returned vector are : flux for given 
  // casacore::Stokes (in the units you specify), longitude location (absolute pixels), and
  // latitude location (absolute pixels).  For DISK and GAUSSIAN shapes,
  // the next three elements are major axis (absolute pixels)
  // minor axis (absolute pixels), and position angle (N->E; radians).
  // You must specify the brightness units to which the integral flux stored
  // in the SkyComponent should be converted.  So as to be able to handle
  // /beam units, the restoring beam must also be suppluied.  It can be obtained 
  // from the casacore::ImageInfo class.  It should be of length 3 or 0 (no beam).  
  //  A constant spectrum is used so any spectral index information in
  // the component is lost.
  casacore::Vector<casacore::Double> toPixel (
		  const casacore::Unit& brightnessUnitOut,
          const casacore::GaussianBeam& restoringBeam,
          const casacore::CoordinateSystem& cSys,
          casacore::Stokes::StokesTypes stokes
  ) const;

  // Take a vector Doubles and fill the SkyComponent from the values.
  // The first three elements of the given vector are : flux for given 
  // casacore::Stokes (in the units you specify), longitude location (absolute pixels), and
  // latitude location (absolute pixels).  For DISK and GAUSSIAN shapes,
  // the next three elements are major axis (absolute pixels)
  // minor axis (absolute pixels), and position angle (N->E; radians).
  // You must specify the brightness units in which the flux is stored
  // in the vector.  It will be converted to an integral reprentation
  // internally for the SkyComponent.  So as to be able to handle
  // /beam units, the restoring beam must also be supplied.  It can be obtained 
  // from the casacore::ImageInfo class.  It should be of length 3 or 0 (no beam).  
  // Multiplying by fluxRatio converts the brightness units to Jy/whatever (e.g. mJy/beam
  // to Jy/beam). You must specify the type of shape to convert to.
  // The SkyComponent is given a  constant spectrum.
  void fromPixel (casacore::Double& fluxRatio, const casacore::Vector<casacore::Double>& parameters,
                  const casacore::Unit& brightnessUnitIn,
                  const casacore::GaussianBeam& restoringBeam,
                  const casacore::CoordinateSystem& cSys,
                  ComponentType::Shape componentShape,
                  casacore::Stokes::StokesTypes stokes);

  // See the corresponding function in the
  // <linkto class="SkyCompBase">SkyCompBase</linkto>
  // class for a description of this function.
  virtual casacore::Bool ok() const;


// Find the factor that converts whatever per whatevers (e.g. mJy per beam)
// to Jy per whatevers (e.g. Jy per beam)
   static casacore::Double convertToJy (const casacore::Unit& brightnessUnit);

   // Convert a peak flux density to integral flux density
	static casacore::Quantity peakToIntegralFlux (
		const casacore::DirectionCoordinate& dirCoord,
		const ComponentType::Shape componentShape,
		const casacore::Quantum<casacore::Double>& peakFlux,
		const casacore::Quantum<casacore::Double>& majorAxis,
		const casacore::Quantum<casacore::Double>& minorAxis,
		const casacore::GaussianBeam& restoringBeam
	);

	// Convert an integral flux density to peak flux density.  The brightness unit
	// of the output quantum (e.g. mJy/beam) is specified by <src>brightnessUnit</src>
	// Throws an exception if the units of <src>integralFlux</src> do not conform to Jy.
	static casacore::Quantity integralToPeakFlux (
		const casacore::DirectionCoordinate& dirCoord,
		const ComponentType::Shape componentShape,
		const casacore::Quantity& integralFlux,
		const casacore::Unit& brightnessUnit,
		const casacore::Quantity& majorAxis,
		const casacore::Quantity& minorAxis,
		const casacore::GaussianBeam& restoringBeam
	);

private:
  casacore::CountedPtr<ComponentShape> itsShapePtr;
  casacore::CountedPtr<SpectralModel> itsSpectrumPtr;
  Flux<casacore::Double> itsFlux;
  casacore::String itsLabel;
  casacore::Vector<casacore::Double> itsOptParms;


  // Make definitions to handle "/beam" and "/pixel" units.   The restoring beam
  // is provided in a vector of quanta (major, minor, position angle).  Should
  // be length 0 or 3. It can be obtained from class ImageInfo
  static casacore::Unit defineBrightnessUnits (
		  casacore::LogIO& os, const casacore::Unit& brightnessUnitIn,
		  const casacore::DirectionCoordinate& dirCoord,
		  const casacore::GaussianBeam& restoringBeam,
		  const casacore::Bool integralIsJy
  );

  // Remove the user defined "/beam" and "/pixel" definitions
     static void undefineBrightnessUnits();

};

} //# NAMESPACE CASA - END

#endif
