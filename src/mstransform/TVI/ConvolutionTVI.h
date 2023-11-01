//# ConvolutionTVI.h: This file contains the interface definition of the MSTransformManager class.
//#
//#  CASA - Common Astronomy Software Applications (http://casa.nrao.edu/)
//#  Copyright (C) Associated Universities, Inc. Washington DC, USA 2011, All rights reserved.
//#  Copyright (C) European Southern Observatory, 2011, All rights reserved.
//#
//#  This library is free software; you can redistribute it and/or
//#  modify it under the terms of the GNU Lesser General Public
//#  License as published by the Free software Foundation; either
//#  version 2.1 of the License, or (at your option) any later version.
//#
//#  This library is distributed in the hope that it will be useful,
//#  but WITHOUT ANY WARRANTY, without even the implied warranty of
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//#  Lesser General Public License for more details.
//#
//#  You should have received a copy of the GNU Lesser General Public
//#  License along with this library; if not, write to the Free Software
//#  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
//#  MA 02111-1307  USA
//# $Id: $

#ifndef ConvolutionTVI_H_
#define ConvolutionTVI_H_

// Base class
#include <mstransform/TVI/FreqAxisTVI.h>


namespace casa { //# NAMESPACE CASA - BEGIN

namespace vi { //# NAMESPACE VI - BEGIN

//////////////////////////////////////////////////////////////////////////
// ConvolutionTVI class
//////////////////////////////////////////////////////////////////////////

class ConvolutionTVI : public FreqAxisTVI
{

public:

	ConvolutionTVI(	ViImplementation2 * inputVii,
					const casacore::Record &configuration = casacore::Record());

    void flag(casacore::Cube<casacore::Bool>& flagCube) const;
    void floatData (casacore::Cube<casacore::Float> & vis) const;
    void visibilityObserved (casacore::Cube<casacore::Complex> & vis) const;
    void visibilityCorrected (casacore::Cube<casacore::Complex> & vis) const;
    void visibilityModel (casacore::Cube<casacore::Complex> & vis) const;
    void weightSpectrum(casacore::Cube<casacore::Float> &weightSp) const;
    void sigmaSpectrum (casacore::Cube<casacore::Float> &sigmaSp) const;

    bool weightSpectrumExists () const {return true;}
    bool sigmaSpectrumExists () const {return true;}

protected:

    casacore::Bool parseConfiguration(const casacore::Record &configuration);
    void initialize();

    mutable casacore::Vector<casacore::Float> convCoeff_p;
};

//////////////////////////////////////////////////////////////////////////
// ConvolutionTVIFactory class
//////////////////////////////////////////////////////////////////////////

class ConvolutionTVIFactory : public ViFactory
{

public:

	ConvolutionTVIFactory(casacore::Record &configuration,ViImplementation2 *inputVII);

protected:

	vi::ViImplementation2 * createVi (VisibilityIterator2 *) const;
	vi::ViImplementation2 * createVi () const;

	casacore::Record configuration_p;
	ViImplementation2 *inputVii_p;;
};

//////////////////////////////////////////////////////////////////////////
// ConvolutionTransformEngine class
//////////////////////////////////////////////////////////////////////////


template<class T> class ConvolutionKernel; // Forward declaration

template<class T> class ConvolutionTransformEngine : public FreqAxisTransformEngine<T>
{

public:

	ConvolutionTransformEngine(ConvolutionKernel<T> *kernel, casacore::uInt width);

	void transform(casacore::Vector<T> &inputVector,casacore::Vector<T> &outputVector);

protected:

	casacore::uInt width_p;
	// This member has to be a pointer, otherwise there
	// are compile time problems due to the fact that
	// it is a pure virtual class.
	ConvolutionKernel<T> *convolutionKernel_p;
};

//////////////////////////////////////////////////////////////////////////
// ConvolutionKernel class
//////////////////////////////////////////////////////////////////////////

template<class T> class ConvolutionKernel
{

public:

	ConvolutionKernel(casacore::Vector<casacore::Float> *convCoeff);

	virtual void kernel(	casacore::Vector<T> &inputVector,
							casacore::Vector<T> &outputVector,
							casacore::uInt startInputPos,
							casacore::uInt outputPos) = 0;

protected:

	casacore::uInt width_p;
	casacore::Vector<casacore::Float> *convCoeff_p;

};

//////////////////////////////////////////////////////////////////////////
// ConvolutionDataKernel class
//////////////////////////////////////////////////////////////////////////

template<class T> class ConvolutionDataKernel : public ConvolutionKernel<T>
{
	using ConvolutionKernel<T>::width_p;
	using ConvolutionKernel<T>::convCoeff_p;

public:

	ConvolutionDataKernel(casacore::Vector<casacore::Float> *convCoeff);

	void kernel(	casacore::Vector<T> &inputVector,
					casacore::Vector<T> &outputVector,
					casacore::uInt startInputPos,
					casacore::uInt outputPos);
};

//////////////////////////////////////////////////////////////////////////
// ConvolutionLogicalORKernel class
//////////////////////////////////////////////////////////////////////////

template<class T> class ConvolutionLogicalORKernel : public ConvolutionKernel<T>
{

	using ConvolutionKernel<T>::width_p;
	using ConvolutionKernel<T>::convCoeff_p;

public:

	ConvolutionLogicalORKernel(casacore::Vector<casacore::Float> *convCoeff);

	void kernel(	casacore::Vector<T> &inputVector,
					casacore::Vector<T> &outputVector,
					casacore::uInt startInputPos,
					casacore::uInt outputPos);
};

//////////////////////////////////////////////////////////////////////////
// ConvolutionWeightPropagationKernel class
//////////////////////////////////////////////////////////////////////////

template<class T> class ConvolutionWeightPropagationKernel : public ConvolutionKernel<T>
{

	using ConvolutionKernel<T>::width_p;
	using ConvolutionKernel<T>::convCoeff_p;

public:

	ConvolutionWeightPropagationKernel(casacore::Vector<casacore::Float> *convCoeff);

	void kernel(	casacore::Vector<T> &inputVector,
					casacore::Vector<T> &outputVector,
					casacore::uInt startInputPos,
					casacore::uInt outputPos);
};


} //# NAMESPACE VI - END

} //# NAMESPACE CASA - END

#endif /* ConvolutionTVI_H_ */

