//# SpectralIndex.h: Models the spectral variation with a spectral index
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
//# $Id: SpectralIndex.h 21229 2012-04-02 12:00:20Z gervandiepen $

#ifndef COMPONENTS_SPECTRALINDEX_H
#define COMPONENTS_SPECTRALINDEX_H

#include <casa/aips.h>
#include <components/ComponentModels/ComponentType.h>
#include <components/ComponentModels/SpectralModel.h>
#include <casacore/casa/Arrays/ArrayFwd.h>

namespace casacore{

class MFrequency;
class RecordInterface;
class String;

}

namespace casa { //# NAMESPACE CASA - BEGIN


// <summary>Models the spectral variation with a spectral index</summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="tSpectralIndex" demos="dSpectralModel">
// </reviewed>

// <prerequisite>
//   <li> <linkto class="SpectralModel">SpectralModel</linkto>
// </prerequisite>
//
// <synopsis>

// This class models the spectral variation of a component with a spectral
// index.

// This class like the other spectral models becomes more useful when used
// through the <linkto class=SkyComponent>SkyComponent</linkto> class, which
// incorporates the flux and spatial variation of the emission, or through the
// <linkto class=ComponentList>ComponentList</linkto> class, which handles
// groups of SkyComponent objects.

// A spectral index is the exponent in a power law model for the variation flux
// with frequency. It is mathematically is defined as:
// <srcblock>
//  (nu / nu_0)^alpha
// </srcblock>
// Where:
// <dl compact>
// <dt><src>nu_0</src><dd> is the reference frequency
// <dt><src>alpha</src><dd> is the spectral index
// <dt><src>nu</src><dd> is the user specified frequency
// </dl>

// As with all classes derived from SpectralModel the basic operation of this
// class is to model the flux as a function of frequency. This class does not
// know what the flux is at the reference frequency. Instead the sample
// functions return factors that are used to scale the flux and calculate the
// amount of flux at a specified frequency. 

// Besides the reference frequency this class has one parameter; the spectral
// index. This parameter can be set & queried using the general purpose
// <src>parameters</src> functions or the class specific <src>index</src>
// functions.

//<Dec-2013> Have added a full stokes spectral variation ...
// This is done via setting setStokesIndex. 
// If setIndex is used then only the first element of  4 parameters is set
// The 4 elements are 
// 0) alpha for stokes I ..such that 
// 1) alpha for linear pol fraction i.e make sure that Q and U is such that  sqrt(Q_0^2+U_0^2)/I_0 * (nu/nu_0)^alpha(1)) is obeyed.
// 2) Rot measure (RM=alpha(2)) value to rotate linear pol by angle RM*(lambda^2- lambda_0^2)
// 3) alpha for circular pol fraction i.e to make V such that V/I=V_0/I_0 *(nu/nu_0)^alpha(3) 
//</Dec-2013> 
// This class also contains functions (<src>toRecord</src> &
// <src>fromRecord</src>) which perform the conversion between Records and
// SpectralIndex objects. These functions define how a SpectralIndex
// object is represented in glish. The format of the record that is generated
// and accepted by these functions is:
// <srcblock>
// c := [type = 'spectral index',
//       frequency = [type = 'frequency',
//                    refer = 'lsr',
//                    m0 = [value = 1, unit = 'GHz']
//                   ],
//       index = 0.7
//      ]
// </srcblock>
// The frequency field contains a record representation of a frequency measure
// and its format is defined in the Measures module. Its refer field defines
// the reference frame for the direction and the m0 field defines the value of
// the reference frequency. The parsing of the type field is case
// insensitive. The index field contains the spectral index.
// </synopsis>

//
// <example>
// These examples are coded in the tSpectralModel.h file.
// <h4>Example 1:</h4>
// In this example a SpectralIndex object is created and used to calculate the
// flux at a number of frequencies.
// <srcblock>
//<Dec-2013> The  example below was NEVER implemented... 
// Could well have written the following and call it documentation:
// Sous un arbre, vos laitues naissent-elles ?
// Si vos laitues naissent, vos navets aussi naissent !
// Leaving it as is for archeological purposes
// </Dec-2013>
//  SpectralIndex siModel;
//  siModel.setRefFrequency(casacore::MFrequency(casacore::Quantity(1.0, "GHz")));
//  siModel.setIndex(1.0, casacore::Stokes::I);  
//  siModel.setIndex(0.5, casacore::Stokes::Q);  
//  siModel.setIndex(0.5, casacore::Stokes::U);  
//  siModel.setIndex(-1.0, casacore::Stokes::V);
//  const Flux<casacore::Double> LBandFlux(1.0, 1.0, 1.0, 1.0);
//  const casacore::MVFrequency step(casacore::Quantity(100.0, "MHz"));
//  casacore::MVFrequency sampleFreq = siModel.refFrequency().getValue();
//  Flux<casacore::Double> sampleFlux;
//  cout << "Frequency\t I-Flux\t Q-Flux\t U-Flux\t V-Flux\n";
//  for (casacore::uInt i = 0; i < 11; i++) {
//    sampleFlux = LBandFlux.copy();
//    sampleFlux.convertPol(ComponentType::LINEAR);
//    sampleFlux.convertUnit(casacore::Unit("WU"));
//    siModel.sample(sampleFlux,
//   	             casacore::MFrequency(sampleFreq, siModel.refFrequency().getRef()));
//    cout << setprecision(3) << sampleFreq.get("GHz")
//         << "\t\t " << sampleFlux.value(0u).re
//         << "\t " << sampleFlux.value(1u).re
//         << "\t " << sampleFlux.value(2u).re
//         << "\t " << sampleFlux.value(3u).re
//         << " " << sampleFlux.unit().getName() << endl;
//    sampleFreq += step;
//  }
//<Dec-2013>
// Now for an example
/////////////////////////////////////////////
// const casacore::MFrequency f1(casacore::Quantity(1.0, "GHz"), casacore::MFrequency::LSRK);
// const casacore::MFrequency f2(casacore::Quantity(2.0, "GHz"), casacore::MFrequency::LSRK);
// SpectralIndex siModel;
//  siModel.setIndex(1.0);
// cout << "scale value at 1 GHz for setIndex 1.0 " << siModel.sample(f1) << endl;
// casacore::Vector<casacore::Double> indices(4);
// indices(0)=1.0; indices(1)=0.2; indices(2)=0.0005; indices(3)=0.1;     
// siModel.setStokesIndex(indices);
// casacore::Vector<casacore::Double> iquv(4);
// iquv(0)=10.0; iquv(1)=0.2; iquv(2)=0.4; iquv(3)=0.1;
// cerr << "iquv in " << iquv << "  indices " << indices << endl;
// siModel.sampleStokes(f1, iquv);
// cerr << "scale value of I at 1.0 GHz " << siModel.sample(f1) << " iquv out " << iquv << endl;
// siModel.sampleStokes(f2, iquv);
// cerr << "scale value of I at 2.0 GHz " << siModel.sample(f2) << " iquv out " << iquv << endl;
//</Dec-2013>
// </srcblock>
// </example>
//
// <motivation> A Spectral Index frequency variation is the  most widely used
// model in radio astronomy. In particular the NFRA package 'newstar' uses it
// extensively.
// </motivation>
//
// <todo asof="1999/11/23">
//   <li> Nothing I hope
// </todo>

// <linkfrom anchor="SpectralIndex" classes="SpectralModel ConstantSpectrum">
//  <here>SpectralIndex</here> - Uses a spectral index to model the spectrum
// </linkfrom>
 
class SpectralIndex: public SpectralModel
{
public:
  // The default SpectralIndex has a reference frequency of 1 GHz in the LSR
  // frame and a spectral index of zero. As such it is no different from the
  // ConstantSpectrum class (except slower).
  SpectralIndex();

  // Construct a SpectralIndex with specified reference frequency and
  // exponent.
  SpectralIndex(const casacore::MFrequency& refFreq, casacore::Double exponent = 0.0);

  // The copy constructor uses copy semantics
  SpectralIndex(const SpectralIndex& other);

  // The destructor does nothing special.
  virtual ~SpectralIndex();

  // The assignment operator uses copy semantics.
  SpectralIndex& operator=(const SpectralIndex& other);

  // return the actual spectral type ie., ComponentType::SPECTRAL_INDEX
  virtual ComponentType::SpectralShape type() const;

  // set/get the spectral index.
  // <group>
  const casacore::Double& index() const;
  void setIndex(const casacore::Double& newIndex);
  const casacore::Vector<casacore::Double>& stokesIndex() const;
  void setStokesIndex(const casacore::Vector<casacore::Double>& newIndex);
  // </group>

  // Return the scaling factor that indicates what proportion of the flux is at
  // the specified frequency. ie. if the centreFrequency argument is the
  // reference frequency then this function will always return one. At other
  // frequencies it will return a non-negative number.
  virtual casacore::Double sample(const casacore::MFrequency& centerFrequency) const;

  virtual void sampleStokes(const casacore::MFrequency& centerFrequency, casacore::Vector<casacore::Double>& iquv) const;
  // Same as the previous function except that many frequencies can be sampled
  // at once. The reference frame must be the same for all the specified
  // frequencies. Uses a customised implementation for improved speed.
  virtual void sample(casacore::Vector<casacore::Double>& scale, 
                      const casacore::Vector<casacore::MFrequency::MVType>& frequencies, 
                      const casacore::MFrequency::Ref& refFrame) const;

  virtual void sampleStokes(casacore::Vector<casacore::Vector<casacore::Double> >& scale,
			    const casacore::Vector<casacore::MFrequency::MVType>& frequencies, 
			    const casacore::MFrequency::Ref& refFrame) const;

  // Return a pointer to a copy of this object upcast to a SpectralModel
  // object. The class that uses this function is responsible for deleting the
  // pointer. This is used to implement a virtual copy constructor.
  virtual SpectralModel* clone() const;

  // return the number of parameters. There is one parameter  or 4 for this spectral
  // model, namely the spectral index for I or I,Q,U,V. So you supply a unit length vector 
  // or one 4 element long when
  // using these functions. Otherwise an exception (casacore::AipsError) may be thrown.
  // <group>
  virtual casacore::uInt nParameters() const;
  virtual void setParameters(const casacore::Vector<casacore::Double>& newSpectralParms);
  virtual casacore::Vector<casacore::Double> parameters() const;
  virtual void setErrors(const casacore::Vector<casacore::Double>& newSpectralErrs);
  virtual casacore::Vector<casacore::Double> errors() const;
  // </group>

  // These functions convert between a casacore::Record and a SpectralIndex. These
  // functions define how a SpectralIndex object is represented in glish and
  // this is detailed in the synopsis above. These functions return false if
  // the record is malformed and append an error message to the supplied string
  // giving the reason.
  // <group>
  virtual casacore::Bool fromRecord(casacore::String& errorMessage, const casacore::RecordInterface& record);
  virtual casacore::Bool toRecord(casacore::String& errorMessage, casacore::RecordInterface& record) const;
  // </group>

  // Convert the parameters of the spectral index object to the specified
  // units. Only one field of the supplied record is used, namely 'index'. This
  // field is optional as the spectral index is a unitless quantity. If the
  // index field is specified it must have the empty string as its value.  This
  // function always returns true unless the index field is specified and does
  // not contain an empty string.
  virtual casacore::Bool convertUnit(casacore::String& errorMessage,
			   const casacore::RecordInterface& record);

  // casacore::Function which checks the internal data of this class for consistant
  // values. Returns true if everything is fine otherwise returns false.
  virtual casacore::Bool ok() const;

private:
  casacore::Double itsIndex;
  casacore::Vector<casacore::Double> itsStokesIndex;
  casacore::Double itsError;
};

} //# NAMESPACE CASA - END

#endif
