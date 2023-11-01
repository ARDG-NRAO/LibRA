//# ConvolutionTVI.h: This file contains the implementation of the ConvolutionTVI class.
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

#include <mstransform/TVI/ConvolutionTVI.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

namespace vi { //# NAMESPACE VI - BEGIN

//////////////////////////////////////////////////////////////////////////
// ConvolutionTVI class
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
ConvolutionTVI::ConvolutionTVI(	ViImplementation2 * inputVii,
								const Record &configuration):
								FreqAxisTVI (inputVii)
{
	// Parse and check configuration parameters
	// Note: if a constructor finishes by throwing an exception, the memory
	// associated with the object itself is cleaned up — there is no memory leak.
	if (not parseConfiguration(configuration))
	{
		throw AipsError("Error parsing ConvolutionTVI configuration");
	}

	initialize();

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Bool ConvolutionTVI::parseConfiguration(const Record &configuration)
{
	int exists = -1;
	Bool ret = true;

	// Parse kernel parameter (optional)
	exists = -1;
	exists = configuration.fieldNumber ("kernel");
	if (exists >= 0)
	{
		if( configuration.type(exists) == casacore::TpArrayFloat )
		{
			convCoeff_p.resize(0,false);
			convCoeff_p = configuration.asArrayFloat( exists );
			logger_p << LogIO::NORMAL << LogOrigin("ConvolutionTVI", __FUNCTION__)
					<< "Kernel is " << convCoeff_p << LogIO::POST;
		}
		else
		{
			ret = false;
			logger_p << LogIO::SEVERE << LogOrigin("ConvolutionTVI", __FUNCTION__)
					<< "Wrong format of kernel parameter (only float/double/int arrays are supported) "
					<< LogIO::POST;
		}
	}

	return ret;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void ConvolutionTVI::initialize()
{
	// Populate nchan input-output maps
	Int spw;
	uInt spw_idx = 0;
	map<Int,vector<Int> >::iterator iter;
	for(iter=spwInpChanIdxMap_p.begin();iter!=spwInpChanIdxMap_p.end();iter++)
	{
		spw = iter->first;
		spwOutChanNumMap_p[spw] = spwInpChanIdxMap_p[spw].size();

		spw_idx++;
	}

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void ConvolutionTVI::flag(Cube<Bool>& flagCube) const
{
	// Get input VisBuffer and SPW
	VisBuffer2 *vb = getVii()->getVisBuffer();

	// Configure Transformation Engine
	ConvolutionLogicalORKernel<Bool> kernel(&convCoeff_p);
	ConvolutionTransformEngine<Bool> transformer(&kernel,convCoeff_p.size());

	// Transform data
	transformFreqAxis(vb->flagCube(),flagCube,transformer);

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void ConvolutionTVI::floatData (Cube<Float> & vis) const
{
	// Get input VisBuffer and SPW
	VisBuffer2 *vb = getVii()->getVisBuffer();

	// Configure Transformation Engine
	ConvolutionDataKernel<Float> kernel(&convCoeff_p);
	ConvolutionTransformEngine<Float> transformer(&kernel,convCoeff_p.size());

	// Transform data
	transformFreqAxis(vb->visCubeFloat(),vis,transformer);

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void ConvolutionTVI::visibilityObserved (Cube<Complex> & vis) const
{
	// Get input VisBuffer and SPW
	VisBuffer2 *vb = getVii()->getVisBuffer();

	// Configure Transformation Engine
	ConvolutionDataKernel<Complex> kernel(&convCoeff_p);
	ConvolutionTransformEngine<Complex> transformer(&kernel,convCoeff_p.size());

	// Transform data
	transformFreqAxis(vb->visCube(),vis,transformer);

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void ConvolutionTVI::visibilityCorrected (Cube<Complex> & vis) const
{
	// Get input VisBuffer and SPW
	VisBuffer2 *vb = getVii()->getVisBuffer();

	// Configure Transformation Engine
	ConvolutionDataKernel<Complex> kernel(&convCoeff_p);
	ConvolutionTransformEngine<Complex> transformer(&kernel,convCoeff_p.size());

	// Transform data
	transformFreqAxis(vb->visCubeCorrected(),vis,transformer);

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void ConvolutionTVI::visibilityModel (Cube<Complex> & vis) const
{
	// Get input VisBuffer and SPW
	VisBuffer2 *vb = getVii()->getVisBuffer();

	// Configure Transformation Engine
	ConvolutionDataKernel<Complex> kernel(&convCoeff_p);
	ConvolutionTransformEngine<Complex> transformer(&kernel,convCoeff_p.size());

	// Transform data
	transformFreqAxis(vb->visCubeModel(),vis,transformer);

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void ConvolutionTVI::weightSpectrum(Cube<Float> &weightSp) const
{
	// Get input VisBuffer and SPW
	VisBuffer2 *vb = getVii()->getVisBuffer();

	// Configure Transformation Engine
	ConvolutionWeightPropagationKernel<Float> kernel(&convCoeff_p);
	ConvolutionTransformEngine<Float> transformer(&kernel,convCoeff_p.size());

	// Transform data
	transformFreqAxis(vb->weightSpectrum(),weightSp,transformer);

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void ConvolutionTVI::sigmaSpectrum(Cube<Float> &sigmaSp) const
{
	// Get input VisBuffer and SPW
	VisBuffer2 *vb = getVii()->getVisBuffer();

	// Configure Transformation Engine
	ConvolutionWeightPropagationKernel<Float> kernel(&convCoeff_p);
	ConvolutionTransformEngine<Float> transformer(&kernel,convCoeff_p.size());

	// Get weightSpectrum from sigmaSpectrum
	Cube<Float> weightSpFromSigmaSp;
	weightSpFromSigmaSp.resize(vb->sigmaSpectrum().shape(),false);
	weightSpFromSigmaSp = vb->sigmaSpectrum(); // = Operator makes a copy
	arrayTransformInPlace (weightSpFromSigmaSp,sigmaToWeight);

	// Transform data
	transformFreqAxis(weightSpFromSigmaSp,sigmaSp,transformer);

	// Transform back from weight format to sigma format
	arrayTransformInPlace (sigmaSp,weightToSigma);

	return;
}

//////////////////////////////////////////////////////////////////////////
// ConvolutionTVIFactory class
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
ConvolutionTVIFactory::ConvolutionTVIFactory (Record &configuration,
													ViImplementation2 *inputVii)
{
	inputVii_p = inputVii;
	configuration_p = configuration;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
vi::ViImplementation2 * ConvolutionTVIFactory::createVi(VisibilityIterator2 *) const
{
	return new ConvolutionTVI(inputVii_p,configuration_p);
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
vi::ViImplementation2 * ConvolutionTVIFactory::createVi() const
{
	return new ConvolutionTVI(inputVii_p,configuration_p);
}

//////////////////////////////////////////////////////////////////////////
// ConvolutionTransformEngine class
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> ConvolutionTransformEngine<T>::ConvolutionTransformEngine
												(ConvolutionKernel<T> *kernel,
												uInt width)
{
	width_p = width;
	convolutionKernel_p = kernel;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> void ConvolutionTransformEngine<T>::transform(Vector<T> &inputVector,
																Vector<T> &outputVector)
{
	uInt startChanIndex = 0;
	uInt outChanStart = width_p / 2;
	uInt outChanIndex = outChanStart;
	uInt outChanStop = inputVector.size() - width_p / 2;
	while (startChanIndex < outChanStop)
	{
		convolutionKernel_p->kernel(inputVector,outputVector,startChanIndex,outChanIndex);
		startChanIndex += 1;
		outChanIndex += 1;
	}

	// Process low end
	for (uInt chanIndex = 0; chanIndex<outChanStart; chanIndex++)
	{
		convolutionKernel_p->kernel(inputVector,outputVector,chanIndex,chanIndex);
		chanIndex += 1;
	}

	// Process high end
	for (uInt chanIndex = outChanStop; chanIndex<inputVector.size(); chanIndex++)
	{
		convolutionKernel_p->kernel(inputVector,outputVector,chanIndex,chanIndex);
		chanIndex += 1;
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
// ConvolutionKernel class
//////////////////////////////////////////////////////////////////////////
template<class T> ConvolutionKernel<T>::ConvolutionKernel(Vector<Float> *convCoeff)
{
	convCoeff_p = convCoeff;
	width_p = convCoeff->size();
}

//////////////////////////////////////////////////////////////////////////
// ConvolutionDataKernel class
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> ConvolutionDataKernel<T>::ConvolutionDataKernel(Vector<Float> *convCoeff):
		ConvolutionKernel<T>(convCoeff)
{

}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> void ConvolutionDataKernel<T>::kernel(	Vector<T> &inputVector,
															Vector<T> &outputVector,
															uInt startInputPos,
															uInt outputPos)
{
	// Do not process edges
	if (startInputPos == outputPos)
	{
		outputVector(outputPos) = inputVector(startInputPos);
		return;
	}

	// Initialization
	outputVector(outputPos) = (*convCoeff_p)(0)*inputVector(startInputPos);

	// Main loop
	for (uInt chanIndex = 1; chanIndex<width_p; chanIndex++)
	{
		outputVector(outputPos) += (*convCoeff_p)(chanIndex)*inputVector(startInputPos+chanIndex);
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
// ConvolutionLogicalORKernel class
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> ConvolutionLogicalORKernel<T>::ConvolutionLogicalORKernel(Vector<Float> *convCoeff):
		ConvolutionKernel<T>(convCoeff)
{


}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> void ConvolutionLogicalORKernel<T>::kernel(	Vector<T> &inputVector,
																Vector<T> &outputVector,
																uInt startInputPos,
																uInt outputPos)
{
	// Flag edges
	if (startInputPos == outputPos)
	{
		outputVector(outputPos) = true;
		return;
	}

	Bool outputFlag = false;
	// Output sample is flagged if any of the contributors are flagged
	for (uInt chanIndex = 0; chanIndex<width_p; chanIndex++)
	{
		if (inputVector(startInputPos+chanIndex))
		{
			outputFlag = true;
			break;
		}
	}

	outputVector(outputPos) = outputFlag;

	return;
}

//////////////////////////////////////////////////////////////////////////
// ConvolutionWeightPropagationKernel class
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> ConvolutionWeightPropagationKernel<T>::ConvolutionWeightPropagationKernel(Vector<Float> *convCoeff):
		ConvolutionKernel<T>(convCoeff)
{

}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> void ConvolutionWeightPropagationKernel<T>::kernel(	Vector<T> &inputVector,
																		Vector<T> &outputVector,
																		uInt startInputPos,
																		uInt outputPos)
{
	// Do not process edges
	if (startInputPos == outputPos)
	{
		outputVector(outputPos) = inputVector(startInputPos);
		return;
	}

	// Initialization (mind for zeros as there is a division operation)
	outputVector(outputPos) = 0;
	if (inputVector(startInputPos) > FLT_MIN)
	{
		outputVector(outputPos) = (*convCoeff_p)(0)*(*convCoeff_p)(0)/inputVector(startInputPos);
	}

	// Main accumulation loop
	for (uInt chanIndex = 1; chanIndex<width_p; chanIndex++)
	{
		// Mind for zeros as there is a division operation
		if (inputVector(startInputPos+chanIndex) > FLT_MIN)
		{
			outputVector(outputPos) += (*convCoeff_p)(chanIndex)*(*convCoeff_p)(chanIndex)/inputVector(startInputPos+chanIndex);
		}
	}

	// Final propagated weight is the inverse of the accumulation
	if (outputVector(outputPos) > FLT_MIN)
	{
		outputVector(outputPos) = 1/outputVector(outputPos);
	}

	return;
}

} //# NAMESPACE VI - END

} //# NAMESPACE CASA - END


