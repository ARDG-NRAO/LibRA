//# PowerLogPoly.h: Models the spectral variation with a power logarithmic polynomial
//# Copyright (C) 1998-2014
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

#ifndef COMPONENTS_POWERLOGPOLY_H
#define COMPONENTS_POWERLOGPOLY_H

#include <casacore/casa/aips.h>
#include <components/ComponentModels/ComponentType.h>
#include <components/ComponentModels/SpectralModel.h>
#include <casacore/casa/Arrays/ArrayFwd.h>

namespace casacore{

class MFrequency;
class RecordInterface;
class String;

}

namespace casa { //# NAMESPACE CASA - BEGIN


// <summary>Models the spectral variation with a power logarithmic polynomial</summary>

// <use visibility=export>

// <prerequisite>
//   <li> <linkto class="SpectralModel">SpectralModel</linkto>
// </prerequisite>
//
// <synopsis>

// This class models the spectral variation of a component with a power
// logarithmic polynomial.

// This class like the other spectral models becomes more useful when used
// through the <linkto class=SkyComponent>SkyComponent</linkto> class, which
// incorporates the flux and spatial variation of the emission, or through the
// <linkto class=ComponentList>ComponentList</linkto> class, which handles
// groups of SkyComponent objects.

// Inensity that varies as power logarithmic polynomial function of frequency is
// defined by
// <srcblock>
// I_nu = I_nu0 * x^(c0 + c1*ln(x) + c2*(ln(x)^2) + c3*(ln(x))^3 + ... 
//        + cn(ln(x))^n)
// </srcblock>
// Where:
// <dl compact>
// <dt><src>I_nu</src><dd> is the intensity at frequency nu
// <dt><src>I_nu0</src><dd> is the intensity at reference frequency nu0
// <dt><src>x = nu/nu0</src><dd> 
// <dt><src>c_i</src><dd> are user specified coefficients
// </dl>

// This is a more general form of the spectral index equation,
// I_nu = I_nu0 * x^alpha,
// in which alpha = c0 and c1 = c2 = ... = cn = 0

// As with all classes derived from SpectralModel the basic operation of this
// class is to model the flux as a function of frequency. This class does not
// know what the flux is at the reference frequency. Instead the sample
// functions return factors that are used to scale the flux and calculate the
// amount of flux at a specified frequency. 

// Besides the reference frequency this class has n specified coefficents.
// These parameters can be set & queried using the general purpose
// <src>parameters</src> function.

// This class also contains functions (<src>toRecord</src> &
// <src>fromRecord</src>) which perform the conversion between Records and
// PowerLogPoly objects. These functions define how a PowerLogPoly
// object is represented at the UI level. The format of the record that is generated
// and accepted by these functions is:
// <srcblock>
// c := [type = 'Power Logarithmic Polynomial',
//       frequency = [type = 'frequency',
//                    refer = 'lsr',
//                    m0 = [value = 1, unit = 'GHz']
//                   ],
//       coeffs = [0.7, 0.02, -0.2, ...]
//      ]
// </srcblock>
// The frequency field contains a record representation of a frequency measure
// and its format is defined in the Measures module. Its refer field defines
// the reference frame and the m0 field defines the value of
// the reference frequency. The parsing of the type field is case
// insensitive. The coeffs field contain the coeffecients as described previously.
// </synopsis>

//
// <example>
// TODO
// </example>
//
// <motivation>
// Needed for calibration models.
// </motivation>
//
 
class PowerLogPoly: public SpectralModel
{
public:
  // The default PowerLogPoly has a reference frequency of 1 GHz in the LSR
  // frame and c_i's all zero. As such it is no different from the
  // ConstantSpectrum class (except slower).
  PowerLogPoly();

  // Construct a PowerLogPoly with specified reference frequency and
  // coefficients.
  PowerLogPoly(
    const casacore::MFrequency& refFreq,
    casacore::Vector<casacore::Double> coeffs
      = casacore::Vector<casacore::Double>(0)
  );

  // The copy constructor uses copy semantics
  PowerLogPoly(const PowerLogPoly& other);

  // The destructor does nothing special.
  virtual ~PowerLogPoly();

  // The assignment operator uses copy semantics.
  PowerLogPoly& operator=(const PowerLogPoly& other);

  // return the actual spectral type ie., ComponentType::PLP
  virtual ComponentType::SpectralShape type() const;

  // Return the scaling factor that indicates what proportion of the flux is at
  // the specified frequency. ie. if the centerFrequency argument is the
  // reference frequency then this function will always return one. At other
  // frequencies it will return a non-negative number.
  virtual casacore::Double sample(const casacore::MFrequency& centerFrequency) const;

  // The returned vector contains four elements, one each for I, Q, U, V.
  // The I value is computed by calling sample(). The Q, U, and V values
  // are identical to the input values of the Vector.
  virtual void sampleStokes(
    const casacore::MFrequency& centerFrequency,
    casacore::Vector<casacore::Double>& iquv
  ) const;

    // Same as the previous function except that many frequencies can be sampled
    // at once. The reference frame must be the same for all the specified
    // frequencies. The scale Vector must be of length frequencies.size()
    virtual void sample(
        casacore::Vector<casacore::Double>& scale, 
        const casacore::Vector<casacore::MFrequency::MVType>& frequencies, 
        const casacore::MFrequency::Ref& refFrame
    ) const;

    // The scale Matrix must be of shape (frequencies.size(), 4), and all the
    // Only the zeroth elements of the rows will be modified.
    virtual void sampleStokes(
        casacore::Matrix<casacore::Double>& scale,
		const casacore::Vector<casacore::MFrequency::MVType>& frequencies, 
		const casacore::MFrequency::Ref& refFrame
    ) const;

  // Return a pointer to a copy of this object upcast to a SpectralModel
  // object. The caller is responsible for deleting the
  // pointer. This is used to implement a virtual copy constructor.
  virtual SpectralModel* clone() const;

  // return the number of parameters. 
  // <group>
  virtual casacore::uInt nParameters() const;
  // note that setParameters will adjust the size of _errors Vector for consistency
  // if necessary and will use copyValues=True
  virtual void setParameters(const casacore::Vector<casacore::Double>& newSpectralParms);
  virtual casacore::Vector<casacore::Double> parameters() const;
  // note that setErros will adjust the size of _coeffs Vector for consistency 
  // if necessary and will use copyValues=True
  virtual void setErrors(const casacore::Vector<casacore::Double>& newSpectralErrs);
  virtual casacore::Vector<casacore::Double> errors() const;
  // </group>

  // These functions convert between a casacore::Record and a PowerLogPoly. These
  // functions define how a SpectralIndex object is represented in glish and
  // this is detailed in the synopsis above. These functions return false if
  // the record is malformed and append an error message to the supplied string
  // giving the reason.
  // <group>
  virtual casacore::Bool fromRecord(
    casacore::String& errorMessage, const casacore::RecordInterface& record
  );
  virtual casacore::Bool toRecord(
    casacore::String& errorMessage, casacore::RecordInterface& record
  ) const;
  // </group>

  virtual casacore::Bool convertUnit(
    casacore::String& errorMessage, const casacore::RecordInterface& record
  );

  // casacore::Function which checks the internal data of this class for consistant
  // values. Returns true if everything is fine otherwise returns false.
  virtual casacore::Bool ok() const;

private:
  casacore::Vector<casacore::Double> _coeffs = {0};
  casacore::Vector<casacore::Double> _errors = {0};

  casacore::Double _getNu0(const casacore::MFrequency::Ref& refFrame) const;

  casacore::Double _getIntensityRatio(casacore::Double x) const;
};

} //# NAMESPACE CASA - END

#endif
