//# LimbDarkenedDiskShape.h: defines LimbDarkened Disk shape
//
//#  CASA - Common Astronomy Software Applications (http://casa.nrao.edu/)
//# Copyright (C) 2012
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Lesser General Public License as published by
//# the Free Software Foundation; either version 2.1 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Lesser General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//#
//#
//#
//# $Id$

#ifndef COMPONENT_LIMBDARKENED_DISKSHAPE_H
#define COMPONENT_LIMBDARKENED_DISKSHAPE_H

//#! Includes go here
#include <casacore/casa/aips.h>
#include <casacore/casa/BasicSL/Complex.h>
#include <components/ComponentModels/TwoSidedShape.h>
#include <casacore/casa/Arrays/ArrayFwd.h>

namespace casacore{

class MDirection;
class MVAngle;
template <class Qtype> class Quantum;

}

namespace casa { //# NAMESPACE CASA - BEGIN

//# Forward Declarations

// <summary>A limb-darkened disk model for the spatial distribution of emission</summary>

//<use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li><linkto class=TwoSidedShape>TwoSidedShape</linkto>
// </prerequisite>
//
// <etymology>
//#! Except when it is obvious (e.g., "casacore::Array") explain how the class name
//#! expresses the role of this class.  Example: casacore::IPosition is short for
//#! "Integral Position" - a specialized integer vector for specifying
//#! array dimensions and indices.
// </etymology>
//
// <synopsis>
// A LimbDarkenedDiskShape models the spatial distribution of radiation from 
// the sky as a using a limb-darkened elliptical disk with user specified major axis
// width, minor axis width and position angle.
//
// This class like the other component shapes becomes more useful when used
// through the <linkto class=SkyComponent>SkyComponent</linkto> class, which
// incorperates the flux and spectral variation of the emission, or through the
// <linkto class=ComponentList>ComponentList</linkto> class, which handles
// groups of SkyComponent objects.
// 
// The functionality of this class is similar to that of <linkto class=DiskShape>
// DiskShape</linkto> with an additional parameter to describe the exponent in 
// the limb darkened disk model (\f$I=I_{0}(1-(r/R)^{2})^{n/2}\f$). 
//
// </synopsis>
//
// <example>
//#! One or two concise (~10-20 lines) examples, with a modest amount of
//#! text to support code fragments.  Use <srcblock> and </srcblock> to
//#! delimit example code.
// </example>
//
// <motivation>
//#! Insight into a class is often provided by a description of
//#! the circumstances that led to its conception and design.  Describe
//#! them here.
// </motivation>
//
// <todo asof="yyyy/mm/dd">
//#! A casacore::List of bugs, limitations, extensions or planned refinements.
//#! The programmer should fill in a date in the "asof" field, which
//#! will usually be the date at which the class is submitted for review.
//#! If, during the review, new "todo" items come up, then the "asof"
//#! date should be changed to the end of the review period.
//#   <li> add this feature
//#   <li> fix this bug
//#   <li> start discussion of this possible extension
// </todo>

class LimbDarkenedDiskShape: public TwoSidedShape
{
public:

//#! Please arrange class members in the sections laid out below.  A
//#! possible exception (there may be others) -- some operator functions
//#! may perform the same task as "General Member Functions", and so you
//#! may wish to group them together.

//#! Friends

//#! Enumerations

//#! Constructors
  //Default constsructor
  LimbDarkenedDiskShape();

  //<group>
  LimbDarkenedDiskShape(const casacore::MDirection& direction,
            const casacore::Quantum<casacore::Double>& majorAxis,
            const casacore::Quantum<casacore::Double>& minorAxis,
            const casacore::Quantum<casacore::Double>& positionAngle,
            const casacore::Float& n);
  LimbDarkenedDiskShape(const casacore::MDirection& direction, const casacore::Quantum<casacore::Double>& width,
            const casacore::Double axialRatio,
            const casacore::Quantum<casacore::Double>& positionAngle,
            const casacore::Float& n);
  // </group>

  // The copy constructor 
  LimbDarkenedDiskShape(const LimbDarkenedDiskShape& other);

  // The destructor
  virtual ~LimbDarkenedDiskShape();

  //#! Operators
  //The assignment operator
  LimbDarkenedDiskShape& operator=(const LimbDarkenedDiskShape& other);

  //#! General Member Functions
  // get the type of the shape (always returns ComponentType::LimbDakenedDisk)
  virtual ComponentType::Shape type() const;

  // use diskshape ones?
  //<group>
  virtual void setWidthInRad(const casacore::Double majorAxis,
                             const casacore::Double minorAxis,
                             const casacore::Double positionAngle);
  virtual casacore::Double majorAxisInRad() const;
  virtual casacore::Double minorAxisInRad() const;
  virtual casacore::Double positionAngleInRad() const;
  virtual casacore::Float getAttnFactor() const;
  //set n factor in darkening equation, \f$I=I_{0}(1-(\frac{r}{R})^{2})^{\frac{n}{2}}\f$
  virtual void setAttnFactor(const casacore::Float attnFactor);  
  virtual casacore::Vector<casacore::Double> optParameters() const;
  virtual void setOptParameters(const casacore::Vector<casacore::Double>& newOptParms);
  //</group>

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

  //same as above except with many frequencies
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

  virtual casacore::String sizeToString() const;

private:
  casacore::Double calcSample(const casacore::MDirection::MVType& compDirValue,
                    const casacore::MDirection::MVType& dirVal,
                    const casacore::Double majRad, const casacore::Double minRad,
                    const casacore::Double pixValue) const;

  casacore::Double calcVis(casacore::Double u, casacore::Double v, const casacore::Double factor) const;
  static void rotateVis(casacore::Double& u, casacore::Double& v,
                        const casacore::Double cpa, const casacore::Double spa);

  //# The parameters of the limb-darkened disk
  // <group>
  casacore::Double itsMajValue;
  casacore::Double itsMinValue;
  casacore::Double itsPaValue;
  casacore::Double itsHeight;
  casacore::Float  itsAttnFactor;  
  // </group>
};

} //# NAMESPACE CASA - END

#endif
