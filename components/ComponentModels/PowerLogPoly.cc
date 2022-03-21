//# SpectralIndex.cc:
//# Copyright (C) 1998,1999,2000,2003
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
//# $Id: SpectralIndex.cc 21292 2012-11-28 14:58:19Z gervandiepen $

#include <components/ComponentModels/PowerLogPoly.h>
#include <casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/ArrayLogical.h>
#include <casa/Containers/RecordInterface.h>
#include <casa/Exceptions/Error.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Logging/LogIO.h>
#include <casa/Logging/LogOrigin.h>
#include <casa/BasicMath/Math.h>
#include <measures/Measures/MFrequency.h>
#include <measures/Measures/MCFrequency.h>
#include <measures/Measures/MeasConvert.h>
#include <casa/Quanta/MVFrequency.h>
#include <casa/Quanta/Quantum.h>
#include <casa/Utilities/Assert.h>
#include <casa/Utilities/DataType.h>
#include <casa/BasicSL/String.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

PowerLogPoly::PowerLogPoly() : SpectralModel() {}

PowerLogPoly::PowerLogPoly(
  const MFrequency& refFreq, const Vector<Double> coeffs
) : SpectralModel(refFreq), _coeffs(coeffs.copy()),
_errors(Vector<Double>(coeffs.size(), 0)) {}

PowerLogPoly::PowerLogPoly(const PowerLogPoly& other) 
  : SpectralModel(other), _coeffs(other._coeffs.copy()),
  _errors(other._errors.copy()) {}

PowerLogPoly::~PowerLogPoly() {}

PowerLogPoly& PowerLogPoly::operator=(const PowerLogPoly& other) {
  if (this != &other) {
    SpectralModel::operator=(other);
    _coeffs.resize(other._coeffs.size());
    _coeffs = other._coeffs.copy();
    _errors.resize(other._errors.size());
    _errors = other._errors.copy();
  }
  return *this;
}

ComponentType::SpectralShape PowerLogPoly::type() const {
  return ComponentType::PLP;
}

Double PowerLogPoly::_getNu0(const MFrequency::Ref& refFrame) const {
  const MFrequency& refFreq(refFrequency());
  const auto nu0 = refFrame == refFreq.getRef()
    ? refFreq.getValue().getValue()
    : MFrequency::Convert(refFreq, refFrame)().getValue().getValue();
  ThrowIf(nu0 <= 0.0, "the reference frequency is zero or negative");
  return nu0;
}

Double PowerLogPoly::_getIntensityRatio(Double x) const {
  const auto logx = log(x);
  Double exponent = 0;
  for(uInt i=0; i<_coeffs.size(); ++i) {
    if (i == 0) {
      exponent = _coeffs[i];
    }
    else if (i == 1) {
      exponent += _coeffs[i] * logx;
    }
    else if (i == 2) {
      exponent += _coeffs[i] * logx * logx;
    }
    else {
      exponent += _coeffs[i] * pow(logx, i);
    }
  }
  return pow(x, exponent);
}

Double PowerLogPoly::sample(const MFrequency& centerFreq) const {
    const auto x = centerFreq.getValue()/_getNu0(centerFreq.getRef());
    return _getIntensityRatio(x);
}

void PowerLogPoly::sampleStokes(
    const MFrequency& centerFreq, Vector<Double>& iquv
) const {
    ThrowIf(iquv.size() != 4, "Four stokes parameters in iquv are expected");
    iquv[0] *= sample(centerFreq);
    // TODO add full stokes support.
}

void PowerLogPoly::sample(
    Vector<Double>& scale,  const Vector<MFrequency::MVType>& frequencies, 
    const MFrequency::Ref& refFrame
) const {
    const auto nSamples = frequencies.size();
    ThrowIf(
        scale.size() != nSamples, 
        "A Vector of length " + String::toString(nSamples) + " is required"
    );
    const auto nu0 = _getNu0(refFrame);
    for (uInt i=0; i<nSamples; ++i) {
        scale[i] = _getIntensityRatio(frequencies[i].getValue()/nu0);
    }
}

void PowerLogPoly::sampleStokes(
    Matrix<Double>& iquv, const Vector<MFrequency::MVType>& frequencies, 
	const MFrequency::Ref& refFrame
) const {
    ThrowIf(
        iquv.shape() != IPosition(2, frequencies.size(), 4),
        "Incorrect Matrix shape"
    );
    const auto nSamples = frequencies.size();
    const auto nu0 = _getNu0(refFrame);
    for (uInt i=0; i<nSamples; ++i) {
        iquv(i, 0) *= _getIntensityRatio(frequencies[i].getValue()/nu0);
    }
    // TODO full polarization implementation to come
}
  
SpectralModel* PowerLogPoly::clone() const {
  SpectralModel* tmpPtr = new PowerLogPoly(*this);
  AlwaysAssert(tmpPtr, AipsError);
  return tmpPtr;
}

uInt PowerLogPoly::nParameters() const {
  return _coeffs.size();
}

void PowerLogPoly::setParameters(const Vector<Double>& newSpectralParms) {
    _coeffs.resize(0);
    _coeffs = newSpectralParms.copy();
    const auto s = _coeffs.size();
    if (_errors.size() != s) {
        _errors.resize(s, True);
    }
}

Vector<Double> PowerLogPoly::parameters() const {
  return _coeffs.copy();
}

void PowerLogPoly::setErrors(const Vector<Double>& newSpectralErrs) {
    ThrowIf(anyLT(newSpectralErrs, 0.0), "The errors must be non-negative.");
    _errors.resize(newSpectralErrs.size());
    _errors = newSpectralErrs.copy();
    const auto s = _errors.size();
    if (_coeffs.size() != s) {
        _coeffs.resize(s, True);
    }
}

Vector<Double> PowerLogPoly::errors() const {
  return _errors.copy();
}

Bool PowerLogPoly::fromRecord(
  String& errorMessage,  const RecordInterface& record) {
  if (! SpectralModel::fromRecord(errorMessage, record)) {
    return false;
  }
  if (!record.isDefined(String("coeffs"))) {
    errorMessage += "The record must have an 'coeffs' field";
    return false;
  }
  {
     const RecordFieldId coeffs("coeffs");
     Vector<Double> coeffVal;
     switch (record.dataType(coeffs)) {
     case TpArrayDouble:
     case TpArrayFloat:
     case TpArrayInt:
       coeffVal = record.asArrayDouble(coeffs);
       break;
     default:
       errorMessage += "The 'coeff' field must be an array of numbers";
       return false;
     }
     setParameters(coeffVal);
  }
  // TODO full stokes implementation to come
  {
      Vector<Double> errorVals(nParameters(), 0.0);
      if (record.isDefined("error")) {
        const RecordFieldId error("error");
        switch (record.dataType(error)) {
        case TpArrayDouble:
        case TpArrayFloat:
        case TpArrayInt: {
          const auto x = record.asArrayDouble(error);
          if (x.size() != nParameters()) {
            errorMessage = "Coefficient and error array lengths are not equal";
            return false;
          }
          else {
            setErrors(x);
          }
        }
        break;
        default:
          errorMessage += "The 'error' field must be an array of numbers";
          return false;
        }
     }
  }
  return true;
}

Bool PowerLogPoly::toRecord(
  String& errorMessage, RecordInterface& record) const {
  if (! SpectralModel::toRecord(errorMessage, record)) {
    return false;
  }
  record.define("coeffs", parameters());
  // TODO full stokes support still to come
  record.define("error", errors());
  return true;
}

Bool PowerLogPoly::convertUnit(
  String&, const RecordInterface& 
) {
  // parameters are dimensionless, no conversion can be done
  return true;
}

Bool PowerLogPoly::ok() const {
  if (! SpectralModel::ok()) {
    return false;
  }
  if (refFrequency().getValue().getValue() <= 0.0) {
    LogIO logErr(LogOrigin("PowerLogPoly", "ok()"));
    logErr << LogIO::SEVERE << "The reference frequency is zero or negative!" 
           << LogIO::POST;
    return false;
  }
  if (anyGT(abs(_coeffs), 100.0)) {
    LogIO logErr(LogOrigin("PowerLogPoly", "ok()"));
    logErr << LogIO::SEVERE << "At least one coefficient is greater than 100!" 
           << LogIO::POST;
    return false;
  }
  return true;
}

} //# NAMESPACE CASA - END
