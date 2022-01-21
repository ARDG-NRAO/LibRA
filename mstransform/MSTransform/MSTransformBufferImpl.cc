//# MSTransformBufferImpl.h: This file contains the implementation of the MSTransformBufferImpl.h class.
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

#include <mstransform/MSTransform/MSTransformBufferImpl.h>

using namespace casacore;
namespace casa
{

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
MSTransformBufferImpl::MSTransformBufferImpl(MSTransformManager *manager)
{
	manager_p = manager;
	rowIdOffset_p = 0;

	// First store SPW related info (CAS-7460)
	MSSpectralWindow spwTable = manager_p->outputMs_p->spectralWindow();
	MSSpWindowColumns spwCols(spwTable);
	spwFrequencies_p.reference(spwCols.chanFreq());
	spwRefRame_p.reference(spwCols.chanFreqMeas());
	inputOutputSPWIndexMap_p = manager_p->inputOutputSPWIndexMap_p;

	// Store observatory position
    MSObservation observationTable = manager_p->selectedInputMs_p->observation();
    MSObservationColumns observationCols(observationTable);
    String observatoryName = observationCols.telescopeName()(0);
    MeasTable::Observatory(observatoryPosition_p,observatoryName);

	if (not manager_p->reindex_p)
	{
		manager_p->inputOutputObservationIndexMap_p.clear();
		manager_p->inputOutputArrayIndexMap_p.clear();
		manager_p->inputOutputScanIndexMap_p.clear();
		manager_p->inputOutputScanIntentIndexMap_p.clear();
		manager_p->inputOutputFieldIndexMap_p.clear();
		manager_p->inputOutputSPWIndexMap_p.clear();
		manager_p->inputOutputDDIndexMap_p.clear();
		manager_p->inputOutputAntennaIndexMap_p.clear();
		manager_p->outputInputSPWIndexMap_p.clear();
	}

	dx_p = 0;
	dy_p = 0;

	// NONE datacol handling
	if (manager_p->datacolumn_p.contains("NONE"))
	{
		noneDataCol_p = true;
	}
	else
	{
		noneDataCol_p = false;
	}

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void MSTransformBufferImpl::resetState()
{
	observationIdOk_p = false;
	arrayIdOk_p = false;
	scanOk_p = false;
	stateIdOk_p = false;
	fieldIdOk_p = false;
	dataDescIdOk_p = false;
	spectralWindowsOk_p = false;
	processorIdOk_p = false;
	antenna1Ok_p = false;
	antenna2Ok_p = false;
	feed1Ok_p = false;
	feed2Ok_p = false;
	flagRowOk_p = false;
	timeOk_p = false;
	timeCentroidOk_p = false;
	timeIntervalOk_p = false;
	exposureOk_p = false;
	uvwOk_p = false;
	weightOk_p = false;
	sigmaOk_p = false;
	flagCubeOk_p = false;
	visCubeOk_p = false;
	visCubeCorrectedOk_p = false;
	visCubeModelOk_p = false;
	visCubeFloatOk_p = false;
	weightSpectrumOk_p = false;
	sigmaSpectrumOk_p = false;
	feedPaOk_p = false;
	parangOk_p = false;
	azelOk_p = false;
	frequenciesOk_p = false;
	channelNumbersOk_p = false;
	channelNumbersSelectedOk_p = false;
	rowIdsOk_p = false;
	shapeOk_p = false;
	nRowsOk_p = false;
	nChannelsOk_p = false;
	nCorrelationsOk_p = false;
	nAntennasOk_p = false;
	freqRefFrameTypeOk_p = false;

	observationIdTransformed_p = false;
	arrayIdTransformed_p = false;
	scanTransformed_p = false;
	stateIdTransformed_p = false;
	fieldIdTransformed_p = false;
	dataDescIdTransformed_p = false;
	spectralWindowsTransformed_p = false;
	processorIdTransformed_p = false;
	antenna1Transformed_p = false;
	antenna2Transformed_p = false;
	feed1Transformed_p = false;
	feed2Transformed_p = false;
	flagRowTransformed_p = false;
	uvwTransformed_p = false;
	weightTransformed_p = false;
	sigmaTransformed_p = false;
	timeTransformed_p = false;
	timeCentroidTransformed_p = false;
	timeIntervalTransformed_p = false;
	exposureTransformed_p = false;
	feedPaTransformed_p = false;
	parangTransformed_p = false;
	azelTransformed_p = false;
	frequenciesTransformed_p = false;
	channelNumbersTransformed_p = false;
	rowIdsTransformed_p = false;

	manager_p->weightSpectrumFlatFilled_p = false;
	manager_p->weightSpectrumFromSigmaFilled_p = false;

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void MSTransformBufferImpl::generateWeights() const
{
	// Make sure shape is defined
	getShape();

	// Make sure flags are updated
	// (are needed to extract weight/sigma from the spectrum columns by using mean/median)
	flagCube();

	// Reshape arrays
	sigmaSpectrum_p.resize(shape_p,false);
	weightSpectrum_p.resize(shape_p,false);
	weight_p.resize(nCorrelations_p,nRows_p,false);
	sigma_p.resize(nCorrelations_p,nRows_p,false);

	// Assign array pointers in the manager to the arrays resident in this buffer
	manager_p->weight_p = &weight_p;
	manager_p->sigma_p = &sigma_p;
	manager_p->flagCube_p = &flagCube_p;
	manager_p->sigmaSpectrum_p = &sigmaSpectrum_p;
	manager_p->weightSpectrum_p = &weightSpectrum_p;

	// Transform weight columns
	RefRows dummyRefRows(0,0);
	manager_p->fillWeightCols(manager_p->getVisBuffer(),dummyRefRows);

	// Set state
	weightOk_p = true;
	sigmaOk_p = true;
	weightSpectrumOk_p = true;
	sigmaSpectrumOk_p = true;
	sigmaTransformed_p = true;
	weightTransformed_p = true;

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Vector<Int> & MSTransformBufferImpl::dataDescriptionIds () const
{
	if (not dataDescIdOk_p)
	{
		getShape();
		dataDescriptionIds_p.resize(nRows_p,false);

		dataDescIdTransformed_p = manager_p->transformDDIVector(manager_p->getVisBuffer()->dataDescriptionIds(),
																dataDescriptionIds_p);

		if (not dataDescIdTransformed_p)
		{
			dataDescriptionIds_p = manager_p->getVisBuffer()->dataDescriptionIds();
		}

		dataDescIdOk_p = true;
	}

	return dataDescriptionIds_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Vector<Int> & MSTransformBufferImpl::spectralWindows () const
{
	if (not spectralWindowsOk_p)
	{
		dataDescriptionIds();

		getShape();
		spectralWindows_p.resize(nRows_p,false);

		if (not dataDescIdTransformed_p)
		{
			spectralWindows_p = manager_p->getVisBuffer()->spectralWindows();
		}
		else
		{
			for (rownr_t rowIdx = 0; rowIdx<nRows_p;rowIdx++)
			{
				spectralWindows_p(rowIdx) = dataDescriptionIds_p(rowIdx);
			}

			spectralWindowsTransformed_p = true;
		}

		spectralWindowsOk_p = true;
	}

	return spectralWindows_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Vector<Int> & MSTransformBufferImpl::observationId () const
{
	if (not observationIdOk_p)
	{
		getShape();
		observationId_p.resize(nRows_p,false);

		observationIdTransformed_p = manager_p->transformReindexableVector(	manager_p->getVisBuffer()->observationId(),
																			observationId_p,
																			true,
																			manager_p->inputOutputObservationIndexMap_p);
		observationIdOk_p = true;
	}

	if (not observationIdTransformed_p)
	{
		return manager_p->getVisBuffer()->observationId();
	}

	return observationId_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Vector<Int> & MSTransformBufferImpl::arrayId () const
{
	if (not arrayIdOk_p)
	{
		getShape();
		arrayId_p.resize(nRows_p,false);

		arrayIdTransformed_p = manager_p->transformReindexableVector(	manager_p->getVisBuffer()->arrayId(),
																		arrayId_p,
																		true,
																		manager_p->inputOutputArrayIndexMap_p);
		arrayIdOk_p = true;
	}

	if (not arrayIdTransformed_p)
	{
		return manager_p->getVisBuffer()->arrayId();
	}

	return arrayId_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Vector<Int> & MSTransformBufferImpl::fieldId () const
{
	if (not fieldIdOk_p)
	{
		getShape();
		fieldId_p.resize(nRows_p,false);

		fieldIdTransformed_p = manager_p->transformReindexableVector(	manager_p->getVisBuffer()->fieldId(),
																		fieldId_p,
																		!manager_p->timespan_p.contains("field"),
																		manager_p->inputOutputFieldIndexMap_p);
		fieldIdOk_p = true;
	}

	if (not fieldIdTransformed_p)
	{
		return manager_p->getVisBuffer()->fieldId();
	}

	return fieldId_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Vector<Int> & MSTransformBufferImpl::stateId () const
{
	if (not stateIdOk_p)
	{
		getShape();
		stateId_p.resize(nRows_p,false);

		stateIdTransformed_p = manager_p->transformReindexableVector(	manager_p->getVisBuffer()->stateId(),
																		stateId_p,
																		!manager_p->timespan_p.contains("state"),
																		manager_p->inputOutputScanIntentIndexMap_p);
		stateIdOk_p = true;
	}

	if (not stateIdTransformed_p)
	{
		return manager_p->getVisBuffer()->stateId();
	}


	return stateId_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Vector<Int> & MSTransformBufferImpl::antenna1 () const
{
	if (not antenna1Ok_p)
	{
		getShape();
		antenna1_p.resize(nRows_p,false);

		antenna1Transformed_p = manager_p->transformReindexableVector(	manager_p->getVisBuffer()->antenna1(),
																		antenna1_p,
																		false,
																		manager_p->inputOutputAntennaIndexMap_p);
		antenna1Ok_p = true;
	}

	if (not antenna1Transformed_p)
	{
		return manager_p->getVisBuffer()->antenna1();
	}

	return antenna1_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Vector<Int> & MSTransformBufferImpl::antenna2 () const
{
	if (not antenna2Ok_p)
	{
		getShape();
		antenna2_p.resize(nRows_p,false);

		antenna2Transformed_p = manager_p->transformReindexableVector(	manager_p->getVisBuffer()->antenna2(),
																		antenna2_p,
																		false,
																		manager_p->inputOutputAntennaIndexMap_p);
		antenna2Ok_p = true;
	}

	if (not antenna2Transformed_p)
	{
		return manager_p->getVisBuffer()->antenna2();
	}

	return antenna2_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Vector<Int> & MSTransformBufferImpl::scan () const
{
	if (not scanOk_p)
	{
		getShape();
		scan_p.resize(nRows_p,false);

		scanTransformed_p = manager_p->transformNotReindexableVector(	manager_p->getVisBuffer()->scan(),
																		scan_p,
																		!manager_p->timespan_p.contains("scan"));
		scanOk_p = true;
	}

	if (not scanTransformed_p)
	{
		return manager_p->getVisBuffer()->scan();
	}

	return scan_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Vector<Int> & MSTransformBufferImpl::processorId () const
{
	if (not processorIdOk_p)
	{
		getShape();
		processorId_p.resize(nRows_p,false);

		processorIdTransformed_p = manager_p->transformNotReindexableVector(	manager_p->getVisBuffer()->processorId(),
																				processorId_p,
																				false);
		processorIdOk_p = true;
	}

	if (not processorIdTransformed_p)
	{
		return manager_p->getVisBuffer()->processorId();
	}

	return processorId_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Vector<Int> & MSTransformBufferImpl::feed1 () const
{
	if (not feed1Ok_p)
	{
		getShape();
		feed1_p.resize(nRows_p,false);

		feed1Transformed_p = manager_p->transformNotReindexableVector(	manager_p->getVisBuffer()->feed1(),
																		feed1_p,
																		false);
		feed1Ok_p = true;
	}

	if (not feed1Transformed_p)
	{
		return manager_p->getVisBuffer()->feed1();
	}

	return feed1_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Vector<Int> & MSTransformBufferImpl::feed2 () const
{
	if (not feed2Ok_p)
	{
		getShape();
		feed2_p.resize(nRows_p,false);

		feed2Transformed_p = manager_p->transformNotReindexableVector(	manager_p->getVisBuffer()->feed2(),
																		feed2_p,
																		false);
		feed2Ok_p = true;
	}

	if (not feed2Transformed_p)
	{
		return manager_p->getVisBuffer()->feed2();
	}

	return feed2_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Vector<Double> & MSTransformBufferImpl::time () const
{
	if (not timeOk_p)
	{
		getShape();
		time_p.resize(nRows_p,false);

		timeTransformed_p = manager_p->transformNotReindexableVector(	manager_p->getVisBuffer()->time(),
																		time_p,
																		false);
		timeOk_p = true;
	}

	if (not timeTransformed_p)
	{
		return manager_p->getVisBuffer()->time();
	}

	return time_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Vector<Double> & MSTransformBufferImpl::timeCentroid () const
{
	if (not timeCentroidOk_p)
	{
		getShape();
		timeCentroid_p.resize(nRows_p,false);

		timeCentroidTransformed_p = manager_p->transformNotReindexableVector(	manager_p->getVisBuffer()->timeCentroid(),
																				timeCentroid_p,
																				false);
		timeCentroidOk_p = true;
	}

	if (not timeCentroidTransformed_p)
	{
		return manager_p->getVisBuffer()->timeCentroid();
	}

	return timeCentroid_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Vector<Double> & MSTransformBufferImpl::timeInterval () const
{
	if (not timeIntervalOk_p)
	{
		getShape();
		timeInterval_p.resize(nRows_p,false);

		timeIntervalTransformed_p = manager_p->transformNotReindexableVector(	manager_p->getVisBuffer()->timeInterval(),
																				timeInterval_p,
																				false);
		timeIntervalOk_p = true;
	}

	if (not timeIntervalTransformed_p)
	{
		return manager_p->getVisBuffer()->timeInterval();
	}

	return timeInterval_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Vector<Double> & MSTransformBufferImpl::exposure () const
{
	if (not exposureOk_p)
	{
		getShape();
		exposure_p.resize(nRows_p,false);

		if (manager_p->combinespws_p)
		{
			manager_p->mapAndAverageVector(	manager_p->getVisBuffer()->exposure(),exposure_p);
			exposureTransformed_p = true;
		}
		else
		{
			exposureTransformed_p = false;
		}

		exposureOk_p = true;
	}

	if (not exposureTransformed_p)
	{
		return manager_p->getVisBuffer()->exposure();
	}

	return exposure_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Vector<Bool> & MSTransformBufferImpl::flagRow () const
{
	if (not flagRowOk_p)
	{
		getShape();
		flagRow_p.resize(nRows_p,false);

		if (manager_p->combinespws_p)
		{
			manager_p->mapAndAverageVector(	manager_p->getVisBuffer()->flagRow(),flagRow_p);
			flagRowTransformed_p = true;
		}
		else
		{
			flagRowTransformed_p = false;
		}

		flagRowOk_p = true;
	}

	if (not flagRowTransformed_p)
	{
		return manager_p->getVisBuffer()->flagRow();
	}

	return flagRow_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Matrix<Double> & MSTransformBufferImpl::uvw () const
{
	if (not uvwOk_p)
	{
		getShape();
		uvw_p.resize(3,nRows_p,false);

		if (manager_p->combinespws_p)
		{
			manager_p->mapMatrix(manager_p->getVisBuffer()->uvw(),uvw_p);
			uvwTransformed_p = true;
		}
		else
		{
			uvwTransformed_p = false;
		}

		uvwOk_p = true;
	}

	if (not uvwTransformed_p)
	{
		return manager_p->getVisBuffer()->uvw();
	}

	return uvw_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Matrix<Float> & MSTransformBufferImpl::weight () const
{
	if (not weightOk_p)
	{
		if (manager_p->cubeTransformation_p)
		{
			generateWeights();
			weightTransformed_p = true;
		}
		else
		{
			getShape();

			if (manager_p->combinespws_p)
			{
				weight_p.resize(nCorrelations_p,nRows_p,false);

				if (manager_p->newWeightFactorMap_p.size() > 0)
				{
					manager_p->mapAndScaleMatrix(	manager_p->getVisBuffer()->weight(),
													weight_p,
													manager_p->newWeightFactorMap_p,
													manager_p->getVisBuffer()->spectralWindows());
				}
				else
				{
					manager_p->mapMatrix(manager_p->getVisBuffer()->weight(),weight_p);
				}
				weightTransformed_p = true;
			}
			else if (manager_p->newWeightFactorMap_p.size() > 0)
			{
				weight_p.resize(nCorrelations_p,nRows_p,false);
				weight_p = manager_p->getVisBuffer()->weight();

				// Apply scale factor
				if ( 	(manager_p->newWeightFactorMap_p.find(manager_p->getVisBuffer()->spectralWindows()(0))  != manager_p->newWeightFactorMap_p.end()) and
						(manager_p->newWeightFactorMap_p[manager_p->getVisBuffer()->spectralWindows()(0)] != 1) )
				{
					weight_p *= manager_p->newWeightFactorMap_p[manager_p->getVisBuffer()->spectralWindows()(0)];
				}
				weightTransformed_p = true;
			}
			else
			{
				weightTransformed_p = false;
			}
		}

		weightOk_p = true;
	}

	if (not weightTransformed_p)
	{
		return manager_p->getVisBuffer()->weight();
	}

	return weight_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Matrix<Float> & MSTransformBufferImpl::sigma () const
{
	if (not sigmaOk_p)
	{
		if (manager_p->cubeTransformation_p)
		{
			generateWeights();
			sigmaTransformed_p = true;
		}
		else
		{
			getShape();

			if (manager_p->combinespws_p)
			{
				sigma_p.resize(nCorrelations_p,nRows_p,false);

				if (manager_p->correctedToData_p)
				{
					// Sigma must be redefined to 1/weight when corrected data becomes data
					sigma_p = weight(); // Copy operator implements an actual copy
					arrayTransformInPlace(sigma_p, vi::AveragingTvi2::weightToSigma);
				}
				else if (manager_p->newSigmaFactorMap_p.size() > 0)
				{
					manager_p->mapAndScaleMatrix(	manager_p->getVisBuffer()->sigma(),
													sigma_p,
													manager_p->newSigmaFactorMap_p,
													manager_p->getVisBuffer()->spectralWindows());
				}
				else
				{
					manager_p->mapMatrix(manager_p->getVisBuffer()->sigma(),sigma_p);
				}
				sigmaTransformed_p = true;
			}
			else if (manager_p->correctedToData_p)
			{
				// Sigma must be redefined to 1/weight when corrected data becomes data
				sigma_p.resize(nCorrelations_p,nRows_p,false);
				sigma_p = weight(); // Copy operator implements an actual copy
				arrayTransformInPlace(sigma_p, vi::AveragingTvi2::weightToSigma);
				sigmaTransformed_p = true;
			}
			else if (manager_p->newSigmaFactorMap_p.size() > 0)
			{
				sigma_p.resize(nCorrelations_p,nRows_p,false);
				sigma_p = manager_p->getVisBuffer()->sigma();

				// Apply scale factor
				if ( 	(manager_p->newSigmaFactorMap_p.find(manager_p->getVisBuffer()->spectralWindows()(0))  != manager_p->newSigmaFactorMap_p.end()) and
						(manager_p->newSigmaFactorMap_p[manager_p->getVisBuffer()->spectralWindows()(0)] != 1) )
				{
					sigma_p *= manager_p->newSigmaFactorMap_p[manager_p->getVisBuffer()->spectralWindows()(0)];
				}
				sigmaTransformed_p = true;
			}
			else
			{
				sigmaTransformed_p = false;
			}
		}

		sigmaOk_p = true;
	}

	if (not sigmaTransformed_p)
	{
		return manager_p->getVisBuffer()->sigma();
	}

	return sigma_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Cube<Bool> & MSTransformBufferImpl::flagCube () const
{
	if (not manager_p->cubeTransformation_p)
	{
		return manager_p->getVisBuffer()->flagCube();
	}
	else if (not flagCubeOk_p)
	{
		if (noneDataCol_p)
		{
			// Setup input data map
			DataCubeMap inputDataCubeMap;
			Cube<Bool> inputFlagCube = manager_p->getVisBuffer()->flagCube();
			DataCubeHolder<Bool> inputFlagDataHolder(inputFlagCube);
			inputDataCubeMap.add(MS::FLAG,static_cast<DataCubeHolderBase*>(&inputFlagDataHolder));

			// Setup output data map
			DataCubeMap outputDataCubeMap;
			flagCube_p.resize(getShape(),false);
			DataCubeHolder<Bool> outputFlagDataHolder(flagCube_p);
			outputDataCubeMap.add(MS::FLAG,static_cast<DataCubeHolderBase*>(&outputFlagDataHolder));

			// Setup transformation
			Int inputSpw = manager_p->getVisBuffer()->spectralWindows()(0);
			uInt chanbin = manager_p->freqbinMap_p[inputSpw];
			inputDataCubeMap.setWindowShape(IPosition(1,chanbin));

			// Transform data
			transformDataCube(	manager_p->getVisBuffer(),
								inputDataCubeMap,
								outputDataCubeMap,
								&MSTransformBufferImpl::channelAverage);
		}
		else if (manager_p->dataColumnAvailable_p)
		{
			visCube();
		}
		else if (manager_p->correctedDataColumnAvailable_p)
		{
			visCubeCorrected();
		}
		else if (manager_p->modelDataColumnAvailable_p)
		{
			visCubeModel();
		}
		else if (manager_p->floatDataColumnAvailable_p)
		{
			visCubeFloat();
		}

		flagCubeOk_p = true;
	}

	return flagCube_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Cube<Complex> & MSTransformBufferImpl::visCube () const
{

	if (not manager_p->dataColumnAvailable_p)
	{
		manager_p->logger_p << LogIO::EXCEPTION << LogOrigin("MSTransformBufferImpl", __FUNCTION__)
				<< "visCube requested but DATA column not present in input MS" << LogIO::POST;
	}

	if (applyPhaseShifting_p and not visCubeOk_p)
	{
		manager_p->getVisBuffer()->dirtyComponentsAdd(VisBufferComponent2::VisibilityCubeObserved);
		manager_p->getVisBuffer()->visCube();
		manager_p->getVisBuffer()->phaseCenterShift(dx_p,dy_p);
	}

	if (not manager_p->cubeTransformation_p)
	{
		visCubeOk_p = true;
		return manager_p->getVisBuffer()->visCube();
	}
	else if (not visCubeOk_p)
	{
		visCube_p.resize(getShape(),false);
		manager_p->visCube_p = &visCube_p;

		flagCube_p.resize(getShape(),false);
		manager_p->flagCube_p = &flagCube_p;

		RefRows dummyRefRows(0,0);
		ArrayColumn<Complex> dummyDataCol;

		const Cube<Float> &applicableSpectrum = manager_p->getApplicableSpectrum(manager_p->getVisBuffer(),MS::DATA);

		manager_p->dataBuffer_p = MSTransformations::visCube;
		manager_p->transformCubeOfData(	manager_p->getVisBuffer(),
										dummyRefRows,
										manager_p->getVisBuffer()->visCube(),
										dummyDataCol,
										NULL,
										applicableSpectrum);
		flagCubeOk_p = true;
		visCubeOk_p = true;
	}

	return visCube_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Cube<Complex> & MSTransformBufferImpl::visCubeCorrected () const
{
	if (not manager_p->correctedDataColumnAvailable_p)
	{
		manager_p->logger_p << LogIO::EXCEPTION << LogOrigin("MSTransformBufferImpl", __FUNCTION__)
				<< "visCubeCorrected requested but CORRECTED_DATA column not present in input MS" << LogIO::POST;
	}

	if (applyPhaseShifting_p and not visCubeCorrectedOk_p)
	{
		manager_p->getVisBuffer()->dirtyComponentsAdd(VisBufferComponent2::VisibilityCubeCorrected);
		manager_p->getVisBuffer()->visCubeCorrected();
		manager_p->getVisBuffer()->phaseCenterShift(dx_p,dy_p);
	}

	if (not manager_p->cubeTransformation_p)
	{
		visCubeCorrectedOk_p = true;
		return manager_p->getVisBuffer()->visCubeCorrected();
	}
	else if (not visCubeCorrectedOk_p)
	{
		visCubeCorrected_p.resize(getShape(),false);
		manager_p->visCubeCorrected_p = &visCubeCorrected_p;

		flagCube_p.resize(getShape(),false);
		manager_p->flagCube_p = &flagCube_p;

		RefRows dummyRefRows(0,0);
		ArrayColumn<Complex> dummyDataCol;

		const Cube<Float> &applicableSpectrum = manager_p->getApplicableSpectrum(manager_p->getVisBuffer(),MS::CORRECTED_DATA);

		manager_p->dataBuffer_p = MSTransformations::visCubeCorrected;
		manager_p->transformCubeOfData(	manager_p->getVisBuffer(),
											dummyRefRows,
											manager_p->getVisBuffer()->visCubeCorrected(),
											dummyDataCol,
											NULL,
											applicableSpectrum);
		flagCubeOk_p = true;
		visCubeCorrectedOk_p = true;
	}

	return visCubeCorrected_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Cube<Complex> & MSTransformBufferImpl::visCubeModel () const
{

	if (not manager_p->modelDataColumnAvailable_p)
	{
		manager_p->logger_p << LogIO::EXCEPTION << LogOrigin("MSTransformBufferImpl", __FUNCTION__)
				<< "visCubeModel requested but MODEL_DATA column not present in input MS" << LogIO::POST;
	}

	if (applyPhaseShifting_p and not visCubeModelOk_p)
	{
		manager_p->getVisBuffer()->dirtyComponentsAdd(VisBufferComponent2::VisibilityCubeModel);
		manager_p->getVisBuffer()->visCubeModel();
		manager_p->getVisBuffer()->phaseCenterShift(dx_p,dy_p);
	}

	if (not manager_p->cubeTransformation_p)
	{
		visCubeModelOk_p = true;
		return manager_p->getVisBuffer()->visCubeModel();
	}
	else if (not visCubeModelOk_p)
	{
		visCubeModel_p.resize(getShape(),false);
		manager_p->visCubeModel_p = &visCubeModel_p;

		flagCube_p.resize(getShape(),false);
		manager_p->flagCube_p = &flagCube_p;

		RefRows dummyRefRows(0,0);
		ArrayColumn<Complex> dummyDataCol;

		const Cube<Float> &applicableSpectrum = manager_p->getApplicableSpectrum(manager_p->getVisBuffer(),MS::MODEL_DATA);

		manager_p->dataBuffer_p = MSTransformations::visCubeModel;
		manager_p->transformCubeOfData(	manager_p->getVisBuffer(),
										dummyRefRows,
										manager_p->getVisBuffer()->visCubeModel(),
										dummyDataCol,
										NULL,
										applicableSpectrum);
		flagCubeOk_p = true;
		visCubeModelOk_p= true;
	}

	return visCubeModel_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Cube<Float> & MSTransformBufferImpl::visCubeFloat () const
{
	if (not manager_p->floatDataColumnAvailable_p)
	{
		manager_p->logger_p << LogIO::EXCEPTION << LogOrigin("MSTransformBufferImpl", __FUNCTION__)
				<< "visCubeFloat requested but FLOAT_DATA column not present in input MS" << LogIO::POST;
	}

	if (not manager_p->cubeTransformation_p)
	{
		return manager_p->getVisBuffer()->visCubeFloat();
	}
	else if (not visCubeFloatOk_p)
	{
		visCubeFloat_p.resize(getShape(),false);
		manager_p->visCubeFloat_p = &visCubeFloat_p;

		flagCube_p.resize(getShape(),false);
		manager_p->flagCube_p = &flagCube_p;

		RefRows dummyRefRows(0,0);
		ArrayColumn<Float> dummyDataCol;

		const Cube<Float> &applicableSpectrum = manager_p->getApplicableSpectrum(manager_p->getVisBuffer(),MS::FLOAT_DATA);

		manager_p->dataBuffer_p = MSTransformations::visCubeFloat;
		manager_p->transformCubeOfData(	manager_p->getVisBuffer(),
										dummyRefRows,
										manager_p->getVisBuffer()->visCubeFloat(),
										dummyDataCol,
										NULL,
										applicableSpectrum);

		flagCubeOk_p = true;
		visCubeFloatOk_p = true;
	}

	return visCubeFloat_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Cube<Float> & MSTransformBufferImpl::weightSpectrum () const
{
	if (not manager_p->cubeTransformation_p)
	{
		return manager_p->getVisBuffer()->weightSpectrum();
	}
	else if (not weightSpectrumOk_p)
	{
		generateWeights();
		weightSpectrumOk_p = true;
	}

	return weightSpectrum_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Cube<Float> & MSTransformBufferImpl::sigmaSpectrum () const
{
	if (not manager_p->cubeTransformation_p)
	{
		return manager_p->getVisBuffer()->sigmaSpectrum();
	}
	if (not sigmaSpectrumOk_p)
	{
		generateWeights();
		sigmaSpectrumOk_p = true;
	}

	return sigmaSpectrum_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Array<Bool> & MSTransformBufferImpl::flagCategory () const
{
	if (not manager_p->inputFlagCategoryAvailable_p)
	{
		manager_p->logger_p << LogIO::EXCEPTION << LogOrigin("MSTransformBufferImpl", __FUNCTION__)
				<< "FlagCategory requested but FLAG_CATEGORY column not present in input MS" << LogIO::POST;
	}

	if (not manager_p->spectrumReshape_p)
	{
		return manager_p->getVisBuffer()->flagCategory();
	}
	else if (not flagCategoryOk_p)
	{
		uInt nCategories = manager_p->getVisBuffer()->flagCategory().shape()(2); // [nC,nF,nCategories,nR]
		IPosition flagCategoryShape(4,nCorrelations_p,nChannels_p,nCategories,nRows_p);
		flagCategory_p.resize(flagCategoryShape,false);
		flagCategoryOk_p = true;
	}

	return flagCategory_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Vector<Float> & MSTransformBufferImpl::feedPa (Double time) const
{
	return manager_p->getVisBuffer()->feedPa(time);
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Float MSTransformBufferImpl::parang0 (Double time) const
{
	return manager_p->getVisBuffer()->parang0(time);
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Vector<Float> & MSTransformBufferImpl::parang(Double time) const
{
	return manager_p->getVisBuffer()->parang(time);
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
MDirection MSTransformBufferImpl::azel0 (Double time) const
{
	return manager_p->getVisBuffer()->azel0(time);
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Vector<MDirection> & MSTransformBufferImpl::azel(Double time) const
{
	return manager_p->getVisBuffer()->azel(time);
}
// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Vector<MDirection> & MSTransformBufferImpl::direction1() const
{
	return manager_p->getVisBuffer()->direction1();
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Double MSTransformBufferImpl::hourang(Double time) const
{
	return manager_p->getVisBuffer()->hourang(time);
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Vector<Int> MSTransformBufferImpl::getCorrelationTypes () const
{
	return manager_p->getVisBuffer()->getCorrelationTypes();
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Vector<Int> & MSTransformBufferImpl::correlationTypes () const
{
	return manager_p->getVisBuffer()->correlationTypes();
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Vector<Stokes::StokesTypes> MSTransformBufferImpl::getCorrelationTypesDefined () const
{
	return manager_p->getVisBuffer()->getCorrelationTypesDefined();
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Vector<Stokes::StokesTypes> MSTransformBufferImpl::getCorrelationTypesSelected () const
{
	return manager_p->getVisBuffer()->getCorrelationTypesSelected();
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Double MSTransformBufferImpl::getFrequency (Int rowInBuffer, Int frequencyIndex, Int frame) const
{
	Bool newFrequencies = 	manager_p->combinespws_p ||
							manager_p->refFrameTransformation_p ||
							manager_p->channelAverage_p ||
							(manager_p->nspws_p > 1);

	if (not newFrequencies)
	{
		return manager_p->getVisBuffer()->getFrequency (rowInBuffer,frequencyIndex,frame);
	}

	getFrequencies(rowInBuffer,frame);


	return frequencies_p(frequencyIndex);
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Vector<Double> & MSTransformBufferImpl::getFrequencies (Int rowInBuffer,Int frame) const
{
	// CAS-7699: Check if frequencies have to be transformed to a new ref. frame
	Bool freqRefFrameTransRequested  = false;
	MFrequency::Types requestedFreqRefFrame = static_cast<MFrequency::Types> (frame);
	if (frame != FrameNotSpecified and requestedFreqRefFrame != freqRefFrameType())
	{
		frequenciesOk_p = false;
		freqRefFrameTransRequested = true;
		freqRefFrameType_p = requestedFreqRefFrame;
	}

	if (not frequenciesOk_p)
	{
		Bool newFrequencies = 	manager_p->combinespws_p ||
								manager_p->refFrameTransformation_p ||
								manager_p->channelAverage_p ||
								(manager_p->nspws_p > 1);

		if (not newFrequencies)
		{
			frequenciesTransformed_p = false;
		}
		else
		{
			getShape();
			spectralWindows();
			frequencies_p.resize(nChannels_p,false);

			if ( (manager_p->reindex_p) or inputOutputSPWIndexMap_p.size() == 0)
			{
				frequencies_p = spwFrequencies_p(spectralWindows_p(rowInBuffer));
			}
			else
			{
				uInt inputSPWIndex = spectralWindows_p(rowInBuffer);
				uInt outputSPWIndex = inputOutputSPWIndexMap_p.find(inputSPWIndex)->second;
				frequencies_p = spwFrequencies_p(outputSPWIndex);
			}

			if (freqRefFrameTransRequested)
			{

				MFrequency::Convert freqRefTranEngine = generateFreqRefTranEngine(time()(rowInBuffer),frame,false);
				for (uInt chan_i=0;chan_i<frequencies_p.size();chan_i++)
				{
					frequencies_p(chan_i) = freqRefTranEngine (Quantity (frequencies_p(chan_i), "Hz")).get ("Hz").getValue();
				}
			}

			frequenciesTransformed_p = true;
		}

		frequenciesOk_p = true;
	}

	if (not frequenciesTransformed_p)
	{
		return manager_p->getVisBuffer()->getFrequencies(rowInBuffer,frame);
	}

	return frequencies_p;

}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const MDirection & MSTransformBufferImpl::phaseCenter () const
{
	return manager_p->getVisBuffer()->phaseCenter();
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const MFrequency::Types & MSTransformBufferImpl::freqRefFrameType () const
{
	if ( not freqRefFrameTypeOk_p)
	{
		spectralWindows();

		uInt inputSPWIndex;
		if ( (manager_p->reindex_p) or inputOutputSPWIndexMap_p.size() == 0)
		{
			inputSPWIndex = spectralWindows_p(0);
		}
		else
		{
			inputSPWIndex = inputOutputSPWIndexMap_p.find(spectralWindows_p(0))->second;
		}

		MFrequency refFrame = Vector<MFrequency>(spwRefRame_p(inputSPWIndex))(0);
		freqRefFrameType_p = MFrequency::castType (refFrame.type());
	}

	return freqRefFrameType_p;
}


// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
MFrequency::Convert MSTransformBufferImpl::generateFreqRefTranEngine (	Double time,
																	Int outputRefFrame,
																	Bool toObservedFrame) const
{
    // Construct input (observed) ref. frame object
    MDirection direction = phaseCenter();
    MEpoch epoch (MVEpoch (Quantity (time, "s")));
    MPosition position = observatoryPosition_p;
    MFrequency::Types refFrameType = freqRefFrameType();
    MeasFrame inputRefFrame (epoch, position, direction);
    MFrequency::Ref inputFreqRefFrame (refFrameType, inputRefFrame);

    // Construct output ref. frame
    MFrequency::Types outputFreqRefFrame = static_cast<MFrequency::Types> (outputRefFrame);

    // Generate freq. trans engine
    MFrequency::Convert result;
    if (toObservedFrame)
    {
        result = MFrequency::Convert (outputFreqRefFrame, inputFreqRefFrame);
    }
    else
    {
        result = MFrequency::Convert (inputFreqRefFrame, outputFreqRefFrame);
    }

    return result;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Int MSTransformBufferImpl::getChannelNumber (Int rowInBuffer, Int frequencyIndex) const
{
	Bool newFrequencies = 	manager_p->combinespws_p ||
							manager_p->refFrameTransformation_p ||
							manager_p->channelAverage_p ||
							(manager_p->nspws_p > 1);

	if (not newFrequencies)
	{
		return manager_p->getVisBuffer()->getChannelNumber (rowInBuffer,frequencyIndex);
	}

	return frequencyIndex;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Vector<Int> & MSTransformBufferImpl::getChannelNumbers (Int rowInBuffer) const
{
	if (not channelNumbersOk_p)
	{
		Bool newFrequencies = 	manager_p->combinespws_p ||
								manager_p->refFrameTransformation_p ||
								manager_p->channelAverage_p ||
								(manager_p->nspws_p > 1);

		if (not newFrequencies)
		{
			channelNumbersTransformed_p = false;
		}
		else
		{
			getShape();
			channelNumbers_p.resize(nChannels_p,false);

			for (uInt chanIdx = 0; chanIdx<nChannels_p;chanIdx++)
			{
				channelNumbers_p(chanIdx) = chanIdx;
			}

			channelNumbersTransformed_p = true;
		}

		channelNumbersOk_p = true;
	}

	if (not channelNumbersTransformed_p)
	{
		return manager_p->getVisBuffer()->getChannelNumbers(rowInBuffer);
	}

	return channelNumbers_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Vector<Int> MSTransformBufferImpl::getChannelNumbersSelected (Int outputChannelIndex) const
{
	if (not channelNumbersSelectedOk_p)
	{
		// Make sure output channel numbers are filled in and map is cleared
		outputInputChannelMap_p.clear();
		Vector<Int> channelNumbers = getChannelNumbers(0); // Sometimes is FW directly from the inner VB

		// Get current SPW
		Int inputSpw = manager_p->getVisBuffer()->spectralWindows()[0];

		// If there is no chan. selection for this SPW create a dummy selection
		if (manager_p->inputOutputChanIndexMap_p.find(inputSpw) == manager_p->inputOutputChanIndexMap_p.end())
		{
			manager_p->inputOutputChanIndexMap_p[inputSpw] = vector<Int>(manager_p->getVisBuffer()->nChannels(),0);
			for (Int idx=0;idx<manager_p->getVisBuffer()->nChannels();idx++)
			{
				manager_p->inputOutputChanIndexMap_p[inputSpw].at(idx) = idx;
			}
		}

		// If there is no channel average just map input-output channel
		Int selectedChannel;
		if (not manager_p->channelAverage_p)
		{
			for (uInt outChanIdx=0;outChanIdx<channelNumbers.size();outChanIdx++)
			{
				selectedChannel = manager_p->inputOutputChanIndexMap_p[inputSpw].at(outChanIdx);
				// CAS-8018: If there is not channel average (also trigger by chanbin=1)
				//           Then getChannelNumbers are not an index ranging from 0 to nOutChans
				//           but the actual channel selection from the inner VI/VB
				outputInputChannelMap_p[outChanIdx] = Vector<Int>(1,selectedChannel);
			}
		}
		// If there channel average map all selected channels falling in each bin
		else
		{
			for (uInt outChanIdx=0;outChanIdx<channelNumbers.size();outChanIdx++)
			{
				// CAS-8018: If there is channel average then getChannelNumbers
				//           is just an index ranging from 0 to nOutChans so there
				//           is no need to re-map it.
				outputInputChannelMap_p[outChanIdx] = manager_p->dataHandler_p->getSelectedChannelsMap()[inputSpw][outChanIdx];
			}
		}

		channelNumbersSelectedOk_p = true;
	}

	return outputInputChannelMap_p[outputChannelIndex];
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Vector<rownr_t> & MSTransformBufferImpl::rowIds () const
{
	if (not rowIdsOk_p)
	{
		Bool newFrequencies = 	manager_p->combinespws_p ||
								manager_p->refFrameTransformation_p ||
								manager_p->channelAverage_p ||
								(manager_p->nspws_p > 1);

		if (not newFrequencies)
		{
			rowIdsTransformed_p = false;
		}
		else
		{
			getShape();
			rowIds_p.resize(nRows_p,false);

			for (uInt rowIdx = 0; rowIdx<nRows_p;rowIdx++)
			{
				rowIds_p(rowIdx) = rowIdOffset_p + rowIdx;
			}

			rowIdsTransformed_p = true;
		}

		rowIdsOk_p = true;
	}

	if (not rowIdsTransformed_p)
	{
		return manager_p->getVisBuffer()->rowIds();
	}

	return rowIds_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
IPosition MSTransformBufferImpl::getShape () const
{
	if (not shapeOk_p)
	{
		shape_p = manager_p->getShape();
		nRows_p = shape_p(2);
		nChannels_p = shape_p(1);
		nCorrelations_p = shape_p(0);
		nAntennas_p = manager_p->getVisBuffer()->nAntennas();
		shapeOk_p = true;
	}

	return shape_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
rownr_t MSTransformBufferImpl::nRows () const
{
	if (not nRowsOk_p)
	{
		getShape();
		nRowsOk_p = true;
	}

	return nRows_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Int MSTransformBufferImpl::nChannels () const
{
	if (not nChannelsOk_p)
	{
		getShape();
		nChannelsOk_p = true;
	}

	return nChannels_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Int MSTransformBufferImpl::nCorrelations () const
{
	if (not nCorrelationsOk_p)
	{
		getShape();
		nCorrelationsOk_p  = true;
	}

	return nCorrelations_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Int MSTransformBufferImpl::nAntennas () const
{
	if (not nAntennasOk_p)
	{
		getShape();
		nAntennasOk_p = true;
	}

	return nAntennas_p;
}


// -----------------------------------------------------------------------
// Rotate visibility phase for given vector (dim = nrow of vb) of phases (meters)
// phase*(-2*pi*f/c) gives phase for the channel of the given baseline in radian
// sign convention will _correct_ data
// -----------------------------------------------------------------------
void MSTransformBufferImpl::phaseCenterShift(const Vector<Double>& phase)
{
	// Trigger phase center shift in the inner buffer as it always has to happen before any averaging
	manager_p->getVisBuffer()->phaseCenterShift(phase);

	// Clear vis cube state so that they are re-filled and re-averaged with the shifted visibilities
	visCubeOk_p = false;
	visCubeCorrectedOk_p = false;
	visCubeModelOk_p = false;

	return;
}

// -----------------------------------------------------------------------
// Rotate visibility phase for phase center offsets
// -----------------------------------------------------------------------
void MSTransformBufferImpl::phaseCenterShift(Double dx, Double dy)
{
	// Trigger phase center shift in the inner buffer as it always has to happen before any averaging
	manager_p->getVisBuffer()->phaseCenterShift(dx,dy);

	// Clear vis cube state so that they are re-filled and re-averaged with the shifted visibilities
	visCubeOk_p = false;
	visCubeCorrectedOk_p = false;
	visCubeModelOk_p = false;

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void MSTransformBufferImpl::transformDataCube(	vi::VisBuffer2 *vb,
												DataCubeMap &inputDataCubeMap,
												DataCubeMap &outputDataCubeMap,
												TransformFunction funcPointer) const
{
	uInt nRows = vb->nRows();
	uInt nCorrs = vb->nCorrelations();

	for (uInt rowIndex=0; rowIndex < nRows; rowIndex++)
	{
		// Switch input data to current row
		inputDataCubeMap.setMatrixIndex(rowIndex);
		outputDataCubeMap.setMatrixIndex(rowIndex);

		for (uInt corrIndex=0; corrIndex < nCorrs; corrIndex++)
		{
			// Switch input data to current row
			inputDataCubeMap.setVectorIndex(corrIndex);
			outputDataCubeMap.setVectorIndex(corrIndex);

			// Process data
			(*this.*funcPointer)(vb,inputDataCubeMap,outputDataCubeMap);
		}
	}

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void MSTransformBufferImpl::decimationWindow(	vi::VisBuffer2 *vb,
												DataCubeMap &inputDataCubeMap,
												DataCubeMap &outputDataCubeMap,
												TransformKernel1D kernelPointer) const
{
	uInt width = inputDataCubeMap.getWindowShape()(0);
	uInt inputSize = inputDataCubeMap.getVectorShape()(0);
	uInt outputSize = outputDataCubeMap.getVectorShape()(0);

	uInt outputIndex = 0;
	uInt inputStartIndex = 0;
	uInt tail = inputSize % width;
	uInt limit = inputSize - tail;

	while (inputStartIndex < limit)
	{
		(*this.*kernelPointer)(	vb,
								inputDataCubeMap,
								outputDataCubeMap,
								inputStartIndex,
								outputIndex,
								width);
		outputIndex += 1;
		inputStartIndex += width;
	}

	if (tail > 0  and outputIndex < outputSize )
	{
		(*this.*kernelPointer)(	vb,
								inputDataCubeMap,
								outputDataCubeMap,
								inputStartIndex,
								outputIndex,
								tail);
	}

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void MSTransformBufferImpl::channelAverage(	vi::VisBuffer2 *vb,
											DataCubeMap &inputDataCubeMap,
											DataCubeMap &outputDataCubeMap) const
{
	decimationWindow(vb,inputDataCubeMap,outputDataCubeMap,&MSTransformBufferImpl::flagAverageKernel);
	return;
}


// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void MSTransformBufferImpl::flagAverageKernel(	vi::VisBuffer2 *,
												DataCubeMap &inputDataCubeMap,
												DataCubeMap &outputDataCubeMap,
												uInt &inputPos,
												uInt &outputPos,
												uInt &kernelSize) const
{
	Vector<Bool> inputFlags =  inputDataCubeMap.getVector<Bool>(MS::FLAG);
	Vector<Bool> outputFlags =  outputDataCubeMap.getVector<Bool>(MS::FLAG);

	Bool outputFlag = true;
	for (uInt deltaPos=0;deltaPos<kernelSize;deltaPos++)
	{
		if (not inputFlags(inputPos+deltaPos))
		{
			outputFlag = false;
			break;
		}
	}

	outputFlags(outputPos) = outputFlag;

	return;
}


} //# NAMESPACE CASA - END


