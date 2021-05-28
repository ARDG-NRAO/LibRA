//# Flux.cc:
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
//# $Id: Flux.tcc 21292 2012-11-28 14:58:19Z gervandiepen $

#include <components/ComponentModels/Flux.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/ArrayLogical.h>
#include <casa/Containers/RecordInterface.h>
#include <casa/Exceptions/Error.h>
#include <casa/Logging/LogIO.h>
#include <casa/Logging/LogOrigin.h>
#include <casa/BasicSL/Complex.h>
#include <casa/BasicSL/Constants.h>
#include <measures/Measures/Stokes.h>
#include <casa/Quanta/Quantum.h>
#include <casa/Quanta/QuantumHolder.h>
#include <casa/Utilities/Assert.h>
#include <casa/Utilities/DataType.h>
#include <casa/BasicSL/String.h>
#include <casa/iostream.h>

namespace casa { //# NAMESPACE CASA - BEGIN

template<class T> const std::vector<casacore::Unit> FluxRep<T>::_allowedUnits {"Jy", "Jy.km/s", "K.rad2"};

template<class T> FluxRep<T>::
FluxRep()
  :itsVal(4, typename casacore::NumericTraits<T>::ConjugateType(0,0)),
   itsPol(ComponentType::STOKES),
   itsUnit("Jy"),
   itsErr(4, typename casacore::NumericTraits<T>::ConjugateType(0,0))
{
  itsVal(0) = 1;
}

template<class T> FluxRep<T>::
FluxRep(T i) 
  :itsVal(4, typename casacore::NumericTraits<T>::ConjugateType(0,0)),
   itsPol(ComponentType::STOKES),
   itsUnit("Jy"),
   itsErr(4, typename casacore::NumericTraits<T>::ConjugateType(0,0))
{
  itsVal(0) = i;
}

template<class T> FluxRep<T>::
FluxRep(T i, T q, T u, T v)
  :itsVal(4, typename casacore::NumericTraits<T>::ConjugateType(0,0)),
   itsPol(ComponentType::STOKES),
   itsUnit("Jy"),
   itsErr(4, typename casacore::NumericTraits<T>::ConjugateType(0,0))
{
  itsVal(0) = i;
  itsVal(1) = q;
  itsVal(2) = u;
  itsVal(3) = v;
}

template<class T> FluxRep<T>::
FluxRep(typename casacore::NumericTraits<T>::ConjugateType xx,
	typename casacore::NumericTraits<T>::ConjugateType xy,
	typename casacore::NumericTraits<T>::ConjugateType yx,
	typename casacore::NumericTraits<T>::ConjugateType yy,
	ComponentType::Polarisation pol)
  :itsVal(4),
   itsPol(pol),
   itsUnit("Jy"),
   itsErr(4, typename casacore::NumericTraits<T>::ConjugateType(0,0))
{
  itsVal(0) = xx;
  itsVal(1) = xy;
  itsVal(2) = yx;
  itsVal(3) = yy;
}

template<class T> FluxRep<T>::
FluxRep(const casacore::Vector<T>& flux)
  :itsVal(4, typename casacore::NumericTraits<T>::ConjugateType(0,0)),
   itsPol(ComponentType::STOKES),
   itsUnit("Jy"),
   itsErr(4, typename casacore::NumericTraits<T>::ConjugateType(0,0))
{
  for (casacore::uInt i = 0 ; i < 4; i++) {
    itsVal(i) = flux(i);
  }
}

template<class T> FluxRep<T>::
FluxRep(const casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& flux,
	ComponentType::Polarisation pol)
  :itsVal(flux.copy()),
   itsPol(pol),
   itsUnit("Jy"),
   itsErr(4, typename casacore::NumericTraits<T>::ConjugateType(0,0))
{
}

template<class T> FluxRep<T>::
FluxRep(const casacore::Quantum<casacore::Vector<T> >& flux)
  :itsVal(4, typename casacore::NumericTraits<T>::ConjugateType(0,0)),
   itsPol(ComponentType::STOKES),
   itsUnit(flux.getFullUnit()),
   itsErr(4, typename casacore::NumericTraits<T>::ConjugateType(0,0))
{
  const casacore::Vector<T>& fluxVal(flux.getValue());
  convertArray(itsVal, fluxVal);
}

template<class T> FluxRep<T>::
FluxRep(const casacore::Quantum<casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType> >& flux,
	ComponentType::Polarisation pol)
  :itsVal(flux.getValue().copy()),
   itsPol(pol),
   itsUnit(flux.getFullUnit()),
   itsErr(4, typename casacore::NumericTraits<T>::ConjugateType(0,0))
{
}

template<class T> FluxRep<T>::
FluxRep(const FluxRep<T>& other) 
  :itsVal(other.itsVal.copy()),
   itsPol(other.itsPol),
   itsUnit(other.itsUnit),
   itsErr(other.itsErr)
{
}

template<class T> FluxRep<T>::
~FluxRep() {
}

template<class T> FluxRep<T>& FluxRep<T>::
operator=(const FluxRep<T>& other) {
  if (this != &other) {
    itsVal = other.itsVal;
    itsPol = other.itsPol;
    itsUnit = other.itsUnit;
    itsErr = other.itsErr;
  }
  return *this;
}

template<class T> const casacore::Unit& FluxRep<T>::
unit() const {
  return itsUnit;
}

template<class T> void FluxRep<T>::
unit(casacore::Unit& unit) const {
  unit = itsUnit;
}

template<class T> void FluxRep<T>::
setUnit(const casacore::Unit& unit) {
  itsUnit = unit;
}

template<class T> void FluxRep<T>::
convertUnit(const casacore::Unit& unit) {
  if (unit.getName() != itsUnit.getName()) {
    T factor = itsUnit.getValue().getFac()/unit.getValue().getFac();
    for (casacore::uInt i = 0; i < 4; i++) {
      itsVal(i) *= factor;
      itsErr(i) *= factor;
    }
    itsUnit = unit;
  }
}

template<class T> ComponentType::Polarisation FluxRep<T>::
pol() const {
  return itsPol;
}

template<class T> void FluxRep<T>::
pol(ComponentType::Polarisation& pol) const {
  pol = itsPol;
}

template<class T> void FluxRep<T>::
setPol(ComponentType::Polarisation pol) {
  itsPol = pol;
}

template<class T> void FluxRep<T>::
convertPol(ComponentType::Polarisation pol) {
  if (itsPol != pol) {
    switch (pol){
    case ComponentType::STOKES:
      if (itsPol == ComponentType::LINEAR) {
 	Flux<T>::linearToStokes(itsVal, itsVal);
      } else {
 	Flux<T>::circularToStokes(itsVal, itsVal);
      }
      break;
    case ComponentType::LINEAR:
      if (itsPol == ComponentType::STOKES) {
 	Flux<T>::stokesToLinear(itsVal, itsVal);
      } else {
 	Flux<T>::circularToLinear(itsVal, itsVal);
      }
      break;
    case ComponentType::CIRCULAR:
      if (itsPol == ComponentType::STOKES) {
 	Flux<T>::stokesToCircular(itsVal, itsVal);
      } else {
 	Flux<T>::linearToCircular(itsVal, itsVal);
      }
      break;
    default:
      throw(casacore::AipsError("FluxRep<T>::convertPol(...) - bad polarisation type"));
    };

    setPol(pol);                     // New pol rep label

    if (!allNearAbs(itsErr, typename casacore::NumericTraits<T>::ConjugateType(0,0), 
		    casacore::C::dbl_epsilon)) {
      casacore::LogIO logErr(casacore::LogOrigin("FluxRep", "convertPol()"));
      logErr << casacore::LogIO::WARN 
	     << "The change in flux representation means the numerical values"
	     << " have changed" << std::endl
	     << "But the errors in the flux are not being changed "
	     << "and are probably now incorrect"
	     << casacore::LogIO::POST;
    }
  }
}

template<class T>
const casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& FluxRep<T>::
value() const {
  return itsVal;
}

template<class T>
const typename casacore::NumericTraits<T>::ConjugateType& FluxRep<T>::
value(casacore::uInt p) const {
  return itsVal(p);
}

template<class T> void FluxRep<T>::
value(casacore::Vector<T>& value) {
  const casacore::uInt len = value.nelements();
  if (len == 0) value.resize(4);
  convertPol(ComponentType::STOKES);
  for (casacore::uInt s = 0 ; s < 4; s++) {
    value(s) = itsVal(s).real();
  }
}

template<class T> void FluxRep<T>::
value(casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& value) const {
  value = itsVal;
}

template<class T> void FluxRep<T>::
value(casacore::Quantum<casacore::Vector<T> >& value) {
  const casacore::Unit& curUnit = value.getFullUnit();
  if (curUnit != itsUnit) {
    value.setUnit(itsUnit);
  }
  convertPol(ComponentType::STOKES);
  casacore::Vector<T>& newValue = value.getValue();
  if (newValue.nelements() != 4) newValue.resize(4);
  for (casacore::uInt s = 0 ; s < 4; s++) {
    newValue(s) = itsVal(s).real();
  }
}

template<class T> casacore::Quantum<T>  FluxRep<T>::
value (casacore::Stokes::StokesTypes stokes, casacore::Bool toJy) 
{
   casacore::LogIO os(casacore::LogOrigin("FluxRep", "value(Stokes::StokesTypes)"));

// Get the vector of values that we are holding
  
   casacore::Vector<T> values;
   value(values);
  
// Make a quantum for the result
      
   casacore::Quantum<T> value;
   value.setUnit(unit());

// Fish it out
   
   ComponentType::Polarisation cPol = pol();
   if ( (stokes==casacore::Stokes::I || stokes==casacore::Stokes::Q ||
         stokes==casacore::Stokes::U || stokes==casacore::Stokes::V)  &&
         cPol==ComponentType::STOKES) {
//
      casacore::String error("Failed to extract Flux from SkyComponent because not enough Stokes values");
      if (stokes==casacore::Stokes::I) {
         if (values.nelements()<1) os << error << casacore::LogIO::EXCEPTION;
         value.setValue(values(0));
      } else if (stokes==casacore::Stokes::Q) {
         if (values.nelements()<2) os << error << casacore::LogIO::EXCEPTION;
         value.setValue(values(1));
      } else if (stokes==casacore::Stokes::U) {
         if (values.nelements()<3) os << error << casacore::LogIO::EXCEPTION;
         value.setValue(values(2));
      } else if (stokes==casacore::Stokes::V) {
         if (values.nelements()<4) os << error << casacore::LogIO::EXCEPTION;
         value.setValue(values(3));
      }
   } else {
      os << "It is not possible currently to extract the flux value" << std::endl;
      os << "for Stokes type " << casacore::Stokes::name(stokes)  << " from the SkyComponent" << casacore::LogIO::EXCEPTION;
   }
//
   if (toJy) value.convert(casacore::Unit("Jy"));
   return value;
}

template<class T> void FluxRep<T>::
value(casacore::Quantum<casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType> >&
      value) const {
  const casacore::Unit& curUnit = value.getFullUnit();
  if (curUnit != itsUnit) {
    value.setUnit(itsUnit);
  }
  casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& newValue = value.getValue();
  if (newValue.nelements() != 4) newValue.resize(4);
  for (casacore::uInt s = 0 ; s < 4; s++) {
    newValue(s) = itsVal(s);
  }
}

template<class T> void FluxRep<T>::
setValue(T value) {
  for (casacore::uInt i = 0; i < 4; i++) {
    itsVal(i) = 0.0;
  }
  itsVal(0) = value;
  itsPol = ComponentType::STOKES;
}

template<class T> void FluxRep<T>::
setValue(const casacore::Vector<T>& value) {
  for (casacore::uInt i = 0; i < 4; i++) {
    itsVal(i) = value(i);
  }
  itsPol = ComponentType::STOKES;
}

template<class T> void FluxRep<T>::
setValue(const casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& value) {
  itsVal = value;
}

template<class T> void FluxRep<T>::
setValue(const casacore::Quantum<casacore::Vector<T> >& value) {
  const casacore::Vector<T>& val = value.getValue();
  for (casacore::uInt s = 0; s < 4; s++) {
    itsVal(s) = val(s);
  }
  itsUnit = value.getFullUnit();
  itsPol = ComponentType::STOKES;
}

template<class T> void FluxRep<T>::
setValue(const
	 casacore::Quantum<casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType> >& value,
	 ComponentType::Polarisation pol) {
  itsVal = value.getValue();
  itsUnit = value.getFullUnit();
  itsPol = pol;
}

template<class T> casacore::Unit FluxRep<T>::_getConversionUnit(const casacore::Unit& unit) {
    auto iter = _allowedUnits.begin();
    auto end = _allowedUnits.end();
    for (; iter!=end; ++iter) {
        if (unit.getValue() == iter->getValue()) {
		    return *iter;
		}
    }
    std::ostringstream oss;
    using casacore::AipsError;
	ThrowCc(
        "The flux unit "
		+ unit.getName()
        + " has dimensions that are not supported"
    );
}

template<class T> void FluxRep<T>::setValue(
		const casacore::Quantum<T>& value, casacore::Stokes::StokesTypes stokes
) {
	casacore::LogIO os(casacore::LogOrigin("FluxRep", "setValue(const Quantum<Double>&, Stokes::StokesTypes)"));
	casacore::Vector<T> tmp(4, 0.0);
	auto conversionUnit = _getConversionUnit(value.getUnit()) ;
	if (stokes==casacore::Stokes::I || stokes==casacore::Stokes::Q || stokes==casacore::Stokes::U || stokes==casacore::Stokes::V) {
		if (stokes==casacore::Stokes::I) {
			tmp(0) = value.getValue(casacore::Unit(conversionUnit));
		} else if (stokes==casacore::Stokes::Q) {
			tmp(1) = value.getValue(casacore::Unit(conversionUnit));
		} else if (stokes==casacore::Stokes::U) {
			tmp(2) = value.getValue(casacore::Unit(conversionUnit));
		} else if (stokes==casacore::Stokes::V) {
			tmp(3) = value.getValue(casacore::Unit(conversionUnit));
		}
	}
	else {
		os << casacore::LogIO::WARN << "Can only properly handle I,Q,U,V presently." << std::endl;
		os << "The brightness is assumed to be Stokes I"  << casacore::LogIO::POST;
		tmp(0) = value.getValue(casacore::Unit("Jy"));
	}
	setValue(tmp);
}

template<class T> void FluxRep<T>::
scaleValue(const T& factor) {
  itsVal(0) *= factor;
}

template<class T> void FluxRep<T>::
scaleValue(const T& factor0, const T& factor1, 
	   const T& factor2, const T& factor3) {
  itsVal(0) *= factor0;
  itsVal(1) *= factor1;
  itsVal(2) *= factor2;
  itsVal(3) *= factor3;
}

template<class T> void FluxRep<T>::
scaleValue(const typename  casacore::NumericTraits<T>::ConjugateType& factor) {
  itsVal(0) *= factor;
}

template<class T> void FluxRep<T>::
scaleValue(const typename casacore::NumericTraits<T>::ConjugateType& factor0,
	   const typename casacore::NumericTraits<T>::ConjugateType& factor1,
	   const typename casacore::NumericTraits<T>::ConjugateType& factor2,
	   const typename casacore::NumericTraits<T>::ConjugateType& factor3) {
  itsVal(0) *= factor0;
  itsVal(1) *= factor1;
  itsVal(2) *= factor2;
  itsVal(3) *= factor3;
}

template<class T> void FluxRep<T>::
setErrors(const typename casacore::NumericTraits<T>::ConjugateType& error0,
	  const typename casacore::NumericTraits<T>::ConjugateType& error1,
	  const typename casacore::NumericTraits<T>::ConjugateType& error2,
	  const typename casacore::NumericTraits<T>::ConjugateType& error3) {
  itsErr(0) = error0;
  itsErr(1) = error1;
  itsErr(2) = error2;
  itsErr(3) = error3;
}

template<class T> void FluxRep<T>::setErrors(
	const casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& errors
) {
	itsErr = errors;
}


template<class T> const
casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& FluxRep<T>::errors() const {
  return itsErr;
}

template<class T> casacore::Bool FluxRep<T>::
fromRecord(casacore::String& errorMessage, const casacore::RecordInterface& record) {
  ComponentType::Polarisation newPol = ComponentType::UNKNOWN_POLARISATION;
  {
    const casacore::String polarisationString("polarisation");
    if (!record.isDefined(polarisationString)) {
      casacore::LogIO logErr(casacore::LogOrigin("FluxRep", "fromRecord()"));
      logErr << casacore::LogIO::WARN 
	     << "The flux does not have a 'polarisation' field." << std::endl
	     << "Using the default of Stokes"
	     << casacore::LogIO::POST;
      setPol(ComponentType::STOKES);
    } else {
      const casacore::RecordFieldId polarisation(polarisationString);
      // Maybe the polarisation field should contain ["I", "Q", "U", "V"]. This
      // is harder to parse but more flexible for the future.
      if (record.dataType(polarisation) != casacore::TpString) {
	errorMessage += "The 'polarisation' field must be a record\n";
	return false;
      }      
      if (record.shape(polarisation) != casacore::IPosition(1,1)) {
	errorMessage += "The 'polarisation' field must have only 1 element\n";
	return false;
      } 
      const casacore::String polVal = record.asString(polarisation);
      newPol = ComponentType::polarisation(polVal);
      if (newPol == ComponentType::UNKNOWN_POLARISATION) {
	errorMessage += casacore::String("The polarisation type is not known.\n") +
	  casacore::String("Allowed values are 'Stokes', 'Linear' & 'Circular'\n");
	return false;
      }
    }
  }
  setPol(newPol);
  {
    casacore::QuantumHolder qh;
    if (!qh.fromRecord(errorMessage, record)) {
      errorMessage += "Could not parse the flux record\n";
      return false;
    }
    if (qh.isScalar() && pol() == ComponentType::STOKES) {
      if (qh.isReal()) {
	const casacore::Quantum<casacore::Double>& qVal = qh.asQuantumDouble();
	setValue(qVal.getValue());
	setUnit(qVal.getFullUnit());
      } else {
	const casacore::Quantum<casacore::DComplex>& qVal = qh.asQuantumDComplex();
	const casacore::DComplex& val = qVal.getValue();
	if (! casacore::nearAbs(val.imag(), 0.0, casacore::NumericTraits<T>::minimum)) {
	  errorMessage += "I value cannot be complex\n";
	  return false;
	}
	setValue(T(val.real()));
	setUnit(qVal.getFullUnit());
      }
      casacore::LogIO logErr(casacore::LogOrigin("FluxRep", "fromRecord()"));
      logErr << casacore::LogIO::WARN
	     << "Only the I flux specified. Assuming Q, U, & V are zero\n" 
	     << casacore::LogIO::POST;
    } else {
      if (qh.nelements() != 4u) {
	errorMessage += casacore::String("Must specify all 4 flux values\n")
	  + casacore::String("if the polarisation representation is not Stokes\n");
	return false;
      }
      if (qh.isQuantumVectorDouble()) {
	const casacore::Quantum<casacore::Vector<casacore::Double> > qVal = qh.asQuantumVectorDouble();
	setUnit(qVal.getFullUnit());
	casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType> val(4);
	convertArray(val, qVal.getValue());
	setValue(val);
      } else if (qh.isQuantumVectorDComplex()) {
	const casacore::Quantum<casacore::Vector<casacore::DComplex> >& qVal = qh.asQuantumVectorDComplex();
	setUnit(qVal.getFullUnit());
	casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType> val(4);
	convertArray(val, qVal.getValue());
	setValue(val);
      } else if (qh.isQuantumVectorComplex()) {
	const casacore::Quantum<casacore::Vector<casacore::Complex> >& qVal = qh.asQuantumVectorComplex();
	setUnit(qVal.getFullUnit());
	casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType> val(4);
	convertArray(val, qVal.getValue());
	setValue(val);
      } else if (qh.isQuantumVectorFloat()) {
	const casacore::Quantum<casacore::Vector<casacore::Float> >& qVal = qh.asQuantumVectorFloat();
	setUnit(qVal.getFullUnit());
	casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType> val(4);
	convertArray(val, qVal.getValue());
	setValue(val);
      } else if (qh.isQuantumVectorInt()) {
	const casacore::Quantum<casacore::Vector<casacore::Int> >& qVal = qh.asQuantumVectorInt();
	setUnit(qVal.getFullUnit());
	casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType> val(4);
	convertArray(val, qVal.getValue());
	setValue(val);
      } else {
	errorMessage += "value field must be a real or complex vector\n";
	return false;
      }
    }
  }
  {
    const casacore::String errorString("error");
    if (record.isDefined(errorString)) {
      const casacore::RecordFieldId error(errorString);
      casacore::Vector<casacore::DComplex> err(4, casacore::DComplex(0.0, 0.0));
      if  (record.shape(error) != casacore::IPosition(1,4)) {
 	errorMessage += 
 	  "The 'error' field must be a vector with 4 elements\n";
 	return false;
      }
      if (record.dataType(error) == casacore::TpArrayDouble &&
	  newPol == ComponentType::STOKES) {
	convertArray(err, record.asArrayDouble(error));
      } else if (record.dataType(error) == casacore::TpArrayDComplex) {
	err = record.asArrayDComplex(error);
      } else {
 	errorMessage += 
 	  "The 'error' field must be a complex vector with 4 elements\n";
 	return false;
      }
      setErrors(err(0), err(1), err(2), err(3));
    }
  }
  if (unit() != casacore::Unit("Jy") && unit() != casacore::Unit("K.rad.rad")) {
    errorMessage += "The dimensions of the units must be same as the Jy\n";
    return false;
  }
  if (!ok()) {
    errorMessage += "Inconsistencies in the FluxRep object\n";
    return false;
  }
  return true;
}

template<class T> casacore::Bool FluxRep<T>::
toRecord(casacore::String& errorMessage, casacore::RecordInterface& record) const {
  if (!ok()) {
    errorMessage += "Inconsistancies in the FluxRep object\n";
    return false;
  }
  casacore::QuantumHolder qh;
  if (pol() == ComponentType::STOKES) {
    FluxRep<T> fluxCopy = *this;
    casacore::Quantum<casacore::Vector<T> > qVal;
    fluxCopy.value(qVal);
    qh = casacore::QuantumHolder(qVal);
  } else {
    casacore::Quantum<casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType> > qVal;
    value(qVal);
    qh = casacore::QuantumHolder(qVal);
  }
  if (!qh.toRecord(errorMessage, record)) {
    errorMessage += "Problem generating the flux record\n";
    return false;
  }
  record.define(casacore::RecordFieldId("polarisation"), ComponentType::name(pol()));
  if (pol() != ComponentType::STOKES) {
    record.define(casacore::RecordFieldId("error"), errors());
  } else {
    casacore::Vector<T> realErr(4);
    real(realErr, errors());
    record.define(casacore::RecordFieldId("error"), realErr);
  }
  return true;
}


template<class T> casacore::Bool FluxRep<T>::ok() const {
	// The casacore::LogIO class is only constructed if an Error is detected for
	// performance reasons. Both function static and file static variables
	// where considered and rejected for this purpose.
	if (itsVal.nelements() != 4) {
		casacore::LogIO logErr(casacore::LogOrigin("FluxRep", "ok()"));
		logErr << casacore::LogIO::SEVERE << "The flux does not have 4 elements"
				<< " (corresponding to four polarisations)"
				<< casacore::LogIO::POST;
		return false;
	}
    _getConversionUnit(itsUnit);
    return true;
}

template<class T> Flux<T>::
Flux()
  :itsFluxPtr(new FluxRep<T>)
{
}

template<class T> Flux<T>::
Flux(T i)
  :itsFluxPtr(new FluxRep<T>(i))
{
}

template<class T> Flux<T>::
Flux(T i, T q, T u, T v)
 :itsFluxPtr(new FluxRep<T>(i, q, u, v))
{
}

template<class T> Flux<T>::
Flux(typename casacore::NumericTraits<T>::ConjugateType xx,
     typename casacore::NumericTraits<T>::ConjugateType xy,
     typename casacore::NumericTraits<T>::ConjugateType yx,
     typename casacore::NumericTraits<T>::ConjugateType yy, 
     ComponentType::Polarisation pol)  
  :itsFluxPtr(new FluxRep<T>(xx, xy, yx, yy, pol))
{
}

template<class T> Flux<T>::
Flux(const casacore::Vector<T>& flux)
  :itsFluxPtr(new FluxRep<T>(flux))
{
}

template<class T> Flux<T>::
Flux(const casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& flux,
     ComponentType::Polarisation pol)
  :itsFluxPtr(new FluxRep<T>(flux, pol))
{
}
  
template<class T> Flux<T>::
Flux(const casacore::Quantum<casacore::Vector<T> >& flux)
  :itsFluxPtr(new FluxRep<T>(flux))
{
}

template<class T> Flux<T>::
Flux(const Flux<T>& other) 
  :itsFluxPtr(other.itsFluxPtr)
{ 
}

template<class T> Flux<T>::
~Flux() {
}

template<class T> Flux<T>& Flux<T>::
operator=(const Flux<T>& other) {
  if (this != &other) {
    itsFluxPtr = other.itsFluxPtr;
  }
  return *this;
}

template<class T> Flux<T> Flux<T>::
copy() const {
  Flux<T> newFlux(value(), pol());
  newFlux.setUnit(unit());
  return newFlux;
}

template<class T> const casacore::Unit& Flux<T>::
unit() const {
  return itsFluxPtr->unit();
}

template<class T> void Flux<T>::
unit(casacore::Unit& unit) const {
  itsFluxPtr->unit(unit);
}

template<class T> void Flux<T>::
setUnit(const casacore::Unit& unit) {
  itsFluxPtr->setUnit(unit);
}

template<class T> void Flux<T>::
convertUnit(const casacore::Unit& unit) {
  itsFluxPtr->convertUnit(unit);
}

template<class T> ComponentType::Polarisation Flux<T>::
pol() const {
  return itsFluxPtr->pol();
}

template<class T> void Flux<T>::
pol(ComponentType::Polarisation& pol) const {
  itsFluxPtr->pol(pol);
}

template<class T> void Flux<T>::
setPol(ComponentType::Polarisation pol) {
  itsFluxPtr->setPol(pol);
}

template<class T> void Flux<T>::
convertPol(ComponentType::Polarisation pol) {
  itsFluxPtr->convertPol(pol);
}

template<class T> const
casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& Flux<T>::value() const {
  return itsFluxPtr->value();
}

template<class T> const typename casacore::NumericTraits<T>::ConjugateType& Flux<T>::
value(casacore::uInt p) const {
  return itsFluxPtr->value(p);
}

template<class T> void Flux<T>::
value(casacore::Vector<T>& value) {
  itsFluxPtr->value(value);
}

template<class T> void Flux<T>::
value(casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& value) const {
  itsFluxPtr->value(value);
}

template<class T> void Flux<T>::
value(casacore::Quantum<casacore::Vector<T> >& value) {
  itsFluxPtr->value(value);
}

template<class T> void Flux<T>::
value(casacore::Quantum<casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType> >&
      value) const {
  itsFluxPtr->value(value);
}

template<class T> casacore::Quantum<T>  Flux<T>::
value (casacore::Stokes::StokesTypes stokes, casacore::Bool toJy) 
{
  return itsFluxPtr->value(stokes, toJy);
}



template<class T> void Flux<T>::
setValue(T value) {
  itsFluxPtr->setValue(value);
}

template<class T> void Flux<T>::
setValue(const casacore::Vector<T>& value) {
  itsFluxPtr->setValue(value);
}

template<class T> void Flux<T>::
setValue(const casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& value) {
  itsFluxPtr->setValue(value);
}

template<class T> void Flux<T>::
setValue(const casacore::Quantum<casacore::Vector<T> >& value) {
  itsFluxPtr->setValue(value);
}

template<class T> void Flux<T>::
setValue(const
	 casacore::Quantum<casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType> >& value,
	 ComponentType::Polarisation pol) {
  itsFluxPtr->setValue(value, pol);
}

template<class T> void Flux<T>::
setValue(const casacore::Quantum<T>& value, casacore::Stokes::StokesTypes stokes)
{
  itsFluxPtr->setValue(value, stokes);
}

template<class T> void Flux<T>::
scaleValue(const T& factor) {
  itsFluxPtr->scaleValue(factor);
}

template<class T> void Flux<T>::
scaleValue(const T& factor0, const T& factor1, 
	   const T& factor2, const T& factor3) {
  itsFluxPtr->scaleValue(factor0, factor1, factor2, factor3);
}

template<class T> void Flux<T>::
scaleValue(const typename casacore::NumericTraits<T>::ConjugateType& factor) {
  itsFluxPtr->scaleValue(factor);
}

template<class T> void Flux<T>::
scaleValue(const typename casacore::NumericTraits<T>::ConjugateType& factor0,
	   const typename casacore::NumericTraits<T>::ConjugateType& factor1,
	   const typename casacore::NumericTraits<T>::ConjugateType& factor2,
	   const typename casacore::NumericTraits<T>::ConjugateType& factor3) {
  itsFluxPtr->scaleValue(factor0, factor1, factor2, factor3);
}

template<class T> void Flux<T>::
setErrors(const typename casacore::NumericTraits<T>::ConjugateType& error0,
	  const typename casacore::NumericTraits<T>::ConjugateType& error1,
	  const typename casacore::NumericTraits<T>::ConjugateType& error2,
	  const typename casacore::NumericTraits<T>::ConjugateType& error3) {
  itsFluxPtr->setErrors(error0, error1, error2, error3);
}

template<class T> void Flux<T>::setErrors(
	const casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& errors
) {
	itsFluxPtr->setErrors(errors);
}


template<class T> const
casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& Flux<T>::errors() const {
  return itsFluxPtr->errors();
}

template<class T> casacore::Bool Flux<T>::
fromRecord(casacore::String& errorMessage, const casacore::RecordInterface& record) {
  return itsFluxPtr->fromRecord(errorMessage, record);
}

template<class T> casacore::Bool Flux<T>::
toRecord(casacore::String& errorMessage, casacore::RecordInterface& record) const {
  return itsFluxPtr->toRecord(errorMessage, record);
}

template<class T> casacore::Bool Flux<T>::
ok() const {
  if (itsFluxPtr.null() == true) {
    casacore::LogIO logErr(casacore::LogOrigin("Flux", "ok()"));
    logErr << casacore::LogIO::SEVERE << "Internal pointer is not pointing to anything"
           << casacore::LogIO::POST;
    return false;
  }
  if (itsFluxPtr->ok() == false) {
    casacore::LogIO logErr(casacore::LogOrigin("Flux", "ok()"));
    logErr << casacore::LogIO::SEVERE << "Flux representation is not ok"
           << casacore::LogIO::POST;
    return false;
  }
  return true;
}

template<class T> void Flux<T>::
stokesToCircular(casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& out, 
		 const casacore::Vector<T>& in) {
  const T i = in(0);
  const T q = in(1);
  const T u = in(2);
  const T v = in(3);
  out(0) = typename casacore::NumericTraits<T>::ConjugateType(i + v);
  out(1) = typename casacore::NumericTraits<T>::ConjugateType(q, u);
  out(2) = typename casacore::NumericTraits<T>::ConjugateType(q, -u);
  out(3) = typename casacore::NumericTraits<T>::ConjugateType(i - v);
}

template<class T> void Flux<T>::
stokesToCircular(casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& out, 
		 const casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& in) {
  const typename casacore::NumericTraits<T>::ConjugateType i = in(0);
  const typename casacore::NumericTraits<T>::ConjugateType q = in(1);
  const typename casacore::NumericTraits<T>::ConjugateType& u = in(2);
  const typename casacore::NumericTraits<T>::ConjugateType ju(-u.imag(), u.real());
  const typename casacore::NumericTraits<T>::ConjugateType v = in(3);
  out(0) = i + v;
  out(1) = q + ju;
  out(2) = q - ju;
  out(3) = i - v;
}

template<class T> void Flux<T>::
circularToStokes(casacore::Vector<T>& out,
		 const casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& in) {
  const T rr = in(0).real();
  const typename casacore::NumericTraits<T>::ConjugateType rl = in(1);
  const typename casacore::NumericTraits<T>::ConjugateType lr = in(2);
  const T ll = in(3).real();
  const T two(2);
  out(0) = (rr + ll)/two;
  out(1) = (rl.real() + lr.real())/two;
  out(2) = (rl.imag() - lr.imag())/two;
  out(3) = (rr - ll)/two;
}

template<class T> void Flux<T>::
circularToStokes(casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& out,
		 const casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& in) {
  const typename casacore::NumericTraits<T>::ConjugateType rr = in(0);
  const typename casacore::NumericTraits<T>::ConjugateType rl = in(1);
  const typename casacore::NumericTraits<T>::ConjugateType lr = in(2);
  const typename casacore::NumericTraits<T>::ConjugateType ll = in(3);
  const T two(2);
  out(0) = (rr + ll)/two;
  out(1) = (rl + lr)/two;
  out(2) = typename casacore::NumericTraits<T>::ConjugateType((rl.imag()-lr.imag())/two,
					   (lr.real()-rl.real())/two);
  out(3) = (rr - ll)/two;
}

template<class T> void Flux<T>::
stokesToLinear(casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& out, 
	       const casacore::Vector<T>& in) {
  const T i = in(0);
  const T q = in(1);
  const T u = in(2);
  const T v = in(3);
  out(0) = typename casacore::NumericTraits<T>::ConjugateType(i + q);
  out(1) = typename casacore::NumericTraits<T>::ConjugateType(u, v);
  out(2) = typename casacore::NumericTraits<T>::ConjugateType(u, -v);
  out(3) = typename casacore::NumericTraits<T>::ConjugateType(i - q);
}

template<class T> void Flux<T>::
stokesToLinear(casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& out, 
	       const casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& in) {
  const typename casacore::NumericTraits<T>::ConjugateType i = in(0);
  const typename casacore::NumericTraits<T>::ConjugateType q = in(1);
  const typename casacore::NumericTraits<T>::ConjugateType u = in(2);
  const typename casacore::NumericTraits<T>::ConjugateType& v = in(3);
  const typename casacore::NumericTraits<T>::ConjugateType jv(-v.imag(), v.real());
  out(0) = i + q;
  out(1) = u + jv;
  out(2) = u - jv;
  out(3) = i - q;
}

template<class T> void Flux<T>::
linearToStokes(casacore::Vector<T>& out, 
	       const casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& in) {
  const T xx = in(0).real();
  const typename casacore::NumericTraits<T>::ConjugateType xy = in(1);
  const typename casacore::NumericTraits<T>::ConjugateType yx = in(2);
  const T yy = in(3).real();
  const T two(2);
  out(0) = (xx + yy)/two;
  out(1) = (xx - yy)/two;
  out(2) = (xy.real() + xy.real())/two;
  out(3) = (xy.imag() - yx.imag())/two;
}

template<class T> void Flux<T>::
linearToStokes(casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& out, 
	       const casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& in) {
  const typename casacore::NumericTraits<T>::ConjugateType xx = in(0);
  const typename casacore::NumericTraits<T>::ConjugateType xy = in(1);
  const typename casacore::NumericTraits<T>::ConjugateType yx = in(2);
  const typename casacore::NumericTraits<T>::ConjugateType yy = in(3);
  const T two(2);
  out(0) = (xx + yy)/two;
  out(1) = (xx - yy)/two;
  out(2) = (xy + yx)/two;
  out(3) = typename casacore::NumericTraits<T>::ConjugateType((xy.imag()-yx.imag())/two,
					   (yx.real()-xy.real())/two);
}

template<class T> void Flux<T>::
linearToCircular(casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& out, 
		 const casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& in) {
  const typename casacore::NumericTraits<T>::ConjugateType xx = in(0);
  const typename casacore::NumericTraits<T>::ConjugateType& xy = in(1);
  const typename casacore::NumericTraits<T>::ConjugateType jxy(-xy.imag(), xy.real());
  const typename casacore::NumericTraits<T>::ConjugateType& yx = in(2);
  const typename casacore::NumericTraits<T>::ConjugateType jyx(-yx.imag(), yx.real());
  const typename casacore::NumericTraits<T>::ConjugateType yy = in(3);
  const T two(2);
  out(0) = (xx - jxy + jyx + yy)/two;
  out(1) = (xx + jxy + jyx - yy)/two;
  out(2) = (xx - jxy - jyx - yy)/two;
  out(3) = (xx + jxy - jyx + yy)/two;
}

template<class T> void Flux<T>::
circularToLinear(casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& out, 
		 const casacore::Vector<typename casacore::NumericTraits<T>::ConjugateType>& in) {
  const typename casacore::NumericTraits<T>::ConjugateType rr = in(0);
  const typename casacore::NumericTraits<T>::ConjugateType rl = in(1);
  const typename casacore::NumericTraits<T>::ConjugateType lr = in(2);
  const typename casacore::NumericTraits<T>::ConjugateType ll = in(3);
  const T two(2);
  out(0) = (rr + rl + lr + ll)/two;
  out(1) = typename casacore::NumericTraits<T>::ConjugateType(
		     (-rr.imag() + rl.imag() - lr.imag() + ll.imag())/two,
                     ( rr.real() - rl.real() + lr.real() - ll.real())/two);
  out(2) = typename casacore::NumericTraits<T>::ConjugateType(
		     ( rr.imag() + rl.imag() - lr.imag() - ll.imag())/two,
                     (-rr.real() - rl.real() + lr.real() + ll.real())/two);
  out(3) = (rr - rl - lr + ll)/two;
}
// Local Variables: 
// compile-command: "gmake Flux"
// End: 

} //# NAMESPACE CASA - END

