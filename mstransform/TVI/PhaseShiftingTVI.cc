//# PhaseShiftingTVI.h: This file contains the implementation of the PhaseShiftingTVI class.
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

#include <mstransform/TVI/PhaseShiftingTVI.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

namespace vi { //# NAMESPACE VI - BEGIN

//////////////////////////////////////////////////////////////////////////
// PhaseShiftingTVI class
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
PhaseShiftingTVI::PhaseShiftingTVI(	ViImplementation2 * inputVii,
								const Record &configuration):
								FreqAxisTVI (inputVii)
{
	// Parse and check configuration parameters
	// Note: if a constructor finishes by throwing an exception, the memory
	// associated with the object itself is cleaned up — there is no memory leak.
	if (not parseConfiguration(configuration))
	{
		throw AipsError("Error parsing PhaseShiftingTVI configuration");
	}

	initialize();

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Bool PhaseShiftingTVI::parseConfiguration(const Record &configuration)
{
	int exists = -1;
	Bool ret = true;

	exists = -1;
	exists = configuration.fieldNumber ("XpcOffset");
	if (exists >= 0)
	{
		configuration.get (exists, dx_p);
	}

	exists = -1;
	exists = configuration.fieldNumber ("YpcOffset");
	if (exists >= 0)
	{
		configuration.get (exists, dy_p);
	}

	if (abs(dx_p) > 0 or abs(dy_p) > 0)
	{
		logger_p 	<< LogIO::NORMAL << LogOrigin("PhaseShiftingTVI", __FUNCTION__)
					<< "Phase shift is dx="<< dx_p << " dy=" << dy_p << LogIO::POST;
	}

	return ret;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void PhaseShiftingTVI::initialize()
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
void PhaseShiftingTVI::visibilityObserved (Cube<Complex> & vis) const
{
	// Get input VisBuffer
	VisBuffer2 *vb = getVii()->getVisBuffer();
	Matrix<Double> uvw = vb->uvw();
	Vector<Double> frequencies = vb->getFrequencies(0);

	// Reshape output data before passing it to the DataCubeHolder
	vis.resize(getVisBuffer()->getShape(),false);

	// Gather input data
	DataCubeMap inputData;
	DataCubeHolder<Complex> inputVisCubeHolder(vb->visCube());
	inputData.add(MS::DATA,inputVisCubeHolder);

	// Gather output data
	DataCubeMap outputData;
	DataCubeHolder<Complex> outputVisCubeHolder(vis);
	outputData.add(MS::DATA,outputVisCubeHolder);

	// Configure Transformation Engine
	PhaseShiftingTransformEngine<Complex> transformer(dx_p,dy_p,&uvw,&frequencies,&inputData,&outputData);

	// Transform data
	transformFreqAxis2(vb->getShape(),transformer);

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void PhaseShiftingTVI::visibilityCorrected (Cube<Complex> & vis) const
{
	// Get input VisBuffer
	VisBuffer2 *vb = getVii()->getVisBuffer();
	Matrix<Double> uvw = vb->uvw();
	Vector<Double> frequencies = vb->getFrequencies(0);

	// Reshape output data before passing it to the DataCubeHolder
	vis.resize(getVisBuffer()->getShape(),false);

	// Gather input data
	DataCubeMap inputData;
	DataCubeHolder<Complex> inputVisCubeHolder(vb->visCubeCorrected());
	inputData.add(MS::DATA,inputVisCubeHolder);

	// Gather output data
	DataCubeMap outputData;
	DataCubeHolder<Complex> outputVisCubeHolder(vis);
	outputData.add(MS::DATA,outputVisCubeHolder);

	// Configure Transformation Engine
	PhaseShiftingTransformEngine<Complex> transformer(dx_p,dy_p,&uvw,&frequencies,&inputData,&outputData);

	// Transform data
	transformFreqAxis2(vb->getShape(),transformer);

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void PhaseShiftingTVI::visibilityModel (Cube<Complex> & vis) const
{
	// Get input VisBuffer
	VisBuffer2 *vb = getVii()->getVisBuffer();
	Matrix<Double> uvw = vb->uvw();
	Vector<Double> frequencies = vb->getFrequencies(0);

	// Reshape output data before passing it to the DataCubeHolder
	vis.resize(getVisBuffer()->getShape(),false);

	// Gather input data
	DataCubeMap inputData;
	DataCubeHolder<Complex> inputVisCubeHolder(vb->visCubeModel());
	inputData.add(MS::DATA,inputVisCubeHolder);

	// Gather output data
	DataCubeMap outputData;
	DataCubeHolder<Complex> outputVisCubeHolder(vis);
	outputData.add(MS::DATA,outputVisCubeHolder);

	// Configure Transformation Engine
	PhaseShiftingTransformEngine<Complex> transformer(dx_p,dy_p,&uvw,&frequencies,&inputData,&outputData);

	// Transform data
	transformFreqAxis2(vb->getShape(),transformer);

	return;
}

//////////////////////////////////////////////////////////////////////////
// PhaseShiftingTVIFactory class
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
PhaseShiftingTVIFactory::PhaseShiftingTVIFactory (	Record &configuration,
													ViImplementation2 *inputVii)
{
	inputVii_p = inputVii;
	configuration_p = configuration;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
vi::ViImplementation2 * PhaseShiftingTVIFactory::createVi(VisibilityIterator2 *) const
{
	return new PhaseShiftingTVI(inputVii_p,configuration_p);
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
vi::ViImplementation2 * PhaseShiftingTVIFactory::createVi() const
{
	return new PhaseShiftingTVI(inputVii_p,configuration_p);
}

//////////////////////////////////////////////////////////////////////////
// PhaseShiftingTVILayerFactory class
//////////////////////////////////////////////////////////////////////////

PhaseShiftingTVILayerFactory::PhaseShiftingTVILayerFactory(Record &configuration) :
  ViiLayerFactory(),
  configuration_p(configuration)
{}

ViImplementation2* 
PhaseShiftingTVILayerFactory::createInstance(ViImplementation2* vii0) const 
{
  // Make the PhaseShiftingTVi2, using supplied ViImplementation2, and return it
  ViImplementation2 *vii = new PhaseShiftingTVI(vii0,configuration_p);
  return vii;
}

//////////////////////////////////////////////////////////////////////////
// PhaseShiftingTransformEngine class
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> PhaseShiftingTransformEngine<T>::PhaseShiftingTransformEngine(
															Double dx, Double dy,
															Matrix<Double> *uvw,
															Vector<Double> *frequencies,
															DataCubeMap *inputData,
															DataCubeMap *outputData):
															FreqAxisTransformEngine2<T>(inputData,outputData)
{
	uvw_p = uvw;
	frequencies_p = frequencies;

	// Offsets in radians (input is arcsec)
	dx_p = dx*(C::pi / 180.0 / 3600.0);
	dy_p = dy*(C::pi / 180.0 / 3600.0);
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> void PhaseShiftingTransformEngine<T>::transform(	)
{
	transformCore(inputData_p,outputData_p);
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> void PhaseShiftingTransformEngine<T>::transformCore(	DataCubeMap *inputData,
																		DataCubeMap *outputData)
{
	// Get input/output data
	Vector<T> &inputVector = inputData->getVector<T>(MS::DATA);
	Vector<T> &outputVector = outputData->getVector<T>(MS::DATA);

	// Extra path as fraction of U and V in m
	Double phase = dx_p*(*uvw_p)(0,rowIndex_p) + dy_p*(*uvw_p)(1,rowIndex_p);

	// In radian/Hz
	phase *= -2.0 * C::pi / C::c;

	// Main loop
	Double phase_i;
	Complex factor;
	for (uInt chan_i=0;chan_i<inputVector.size();chan_i++)
	{
		phase_i = phase * (*frequencies_p)(chan_i);
		factor = Complex(cos(phase_i), sin(phase_i));
		outputVector(chan_i) = factor*inputVector(chan_i);
	}
}


} //# NAMESPACE VI - END

} //# NAMESPACE CASA - END


