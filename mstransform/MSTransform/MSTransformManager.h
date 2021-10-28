//# MSTransformManager.h: This file contains the interface definition of the MSTransformManager class.
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

#ifndef MSTransformManager_H_
#define MSTransformManager_H_

// To handle configuration records
#include <casacore/casa/Containers/Record.h>

// To handle variant parameters
#include <stdcasa/StdCasa/CasacSupport.h>

// Measurement Set Selection
#include <ms/MSSel/MSSelection.h>

// casacore::Data handling
#include <mstransform/MSTransform/MSTransformDataHandler.h>

// Regridding
#include <mstransform/MSTransform/MSTransformRegridder.h>

// VisibityIterator / VisibilityBuffer framework
#include <msvis/MSVis/VisibilityIterator2.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <msvis/MSVis/ViFrequencySelection.h>

// TVI framework
#include <msvis/MSVis/ViiLayerFactory.h>
#include <msvis/MSVis/IteratingParameters.h>
#include <msvis/MSVis/AveragingVi2Factory.h>
#include <msvis/MSVis/LayeredVi2Factory.h>
#include <mstransform/TVI/UVContSubTVI.h>
#include <mstransform/TVI/PhaseShiftingTVI.h>

// THis is needed just because of vi::AveragingTvi2::weightToSigma
#include <msvis/MSVis/AveragingTvi2.h>

// To apply hanning smooth
#include <scimath/Mathematics/Smooth.h>

// To apply fft shift
#include <scimath/Mathematics/FFTServer.h>

// To apply 1D interpolations
#include <scimath/Mathematics/InterpolateArray1D.h>

// single dish specific
#include <scimath/Mathematics/Convolver.h>

#include <map>

namespace casa { //# NAMESPACE CASA - BEGIN

// Forward declarations
class MSTransformBufferImpl;
class MSTransformIterator;
class MSTransformIteratorFactory;

// casacore::MS Transform Framework utilities
namespace MSTransformations
{
	// Returns 1/sqrt(wt) or -1, depending on whether wt is positive..
	casacore::Double wtToSigma(casacore::Double wt);
	casacore::Double sigmaToWeight(casacore::Double wt);

	enum InterpolationMethod {
	    // nearest neighbour
	    nearestNeighbour,
	    // linear
	    linear,
	    // cubic
	    cubic,
	    // cubic spline
	    spline,
	    // fft shift
	    fftshift
	  };

	enum WeightingSetup {
		spectrum,
		flags,
		cumSum,
		flat,
		flagSpectrum,
		flagCumSum,
		flagsNonZero,
		flagSpectrumNonZero,
		flagCumSumNonZero
	};

	enum SmoothingSetup {
		plainSmooth,
		plainSmoothSpectrum
	};

	enum dataCol {
		visCube,
		visCubeCorrected,
		visCubeModel,
		visCubeFloat,
		weightSpectrum,
		sigmaSpectrum
	  };

	enum weightTransformation {

		transformWeight,
		transformWeightIntoSigma,
		weightIntoSigma
	};
}

// Forward declarations
struct spwInfo;
struct channelContribution;

// casacore::Map definition
typedef map<casacore::MS::PredefinedColumns,casacore::MS::PredefinedColumns> dataColMap;
typedef map< pair< pair<casacore::uInt,casacore::uInt> , casacore::uInt >,std::vector<casacore::uInt> > baselineMap;
typedef map<casacore::uInt,map<casacore::uInt, casacore::uInt > > inputSpwChanMap;
typedef map<casacore::uInt,vector < channelContribution > >  inputOutputChanFactorMap;
typedef map<casacore::uInt,pair < spwInfo, spwInfo > > inputOutputSpwMap;

// Struct definition
struct channelInfo {

	casacore::Int SPW_id;
	casacore::uInt inpChannel;
	casacore::uInt outChannel;
	casacore::Double CHAN_FREQ;
	casacore::Double CHAN_WIDTH;
	casacore::Double EFFECTIVE_BW;
	casacore::Double RESOLUTION;
	std::vector<casacore::Double> contribFrac;
	std::vector<casacore::Int> contribChannel;
	std::vector<casacore::Int> contribSPW_id;

	channelInfo()
	{
		SPW_id = -1;
		inpChannel = 0;
		outChannel = 0;

		CHAN_FREQ = -1;
		CHAN_WIDTH = -1;
		EFFECTIVE_BW = -1;
		RESOLUTION = -1;
	}

	bool operator<(const channelInfo& right_operand) const
	{
		if (CHAN_FREQ<right_operand.CHAN_FREQ)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	casacore::Double upperBound() const
	{
		return CHAN_FREQ + 0.5 * std::abs(CHAN_WIDTH);
	}

	casacore::Double lowerBound() const
	{
		return CHAN_FREQ - 0.5 * std::abs(CHAN_WIDTH);
	}

	casacore::Double overlap(const channelInfo& other) const
	{

		// The other channel completely covers this channel
		if ((lowerBound() <= other.lowerBound()) and (upperBound() >= other.upperBound()))
		{
			return 1.0;
		}
		// The other channel is completely covered by this channel
		else if ((lowerBound() >= other.lowerBound()) and (upperBound() <= other.upperBound()))
		{
			return CHAN_WIDTH/other.CHAN_WIDTH;
		}
		// Lower end of this channel is overlapping with the other channel
		else if (lowerBound() < other.lowerBound() && other.lowerBound() < upperBound() && upperBound() < other.upperBound())
		{
			return (upperBound()-other.lowerBound())/other.CHAN_WIDTH;
		}
		// Upper end of this channel is overlapping with the other channel
		else if (other.lowerBound() < lowerBound() && lowerBound() < other.upperBound() && other.upperBound() < upperBound())
		{
			return (other.upperBound()-lowerBound())/other.CHAN_WIDTH;
		}
		else
		{
			return 0.0;
		}

	}
};

struct channelContribution {

	casacore::Int inpSpw;
	casacore::uInt inpChannel;
	casacore::uInt outChannel;
	casacore::Double weight;
	casacore::Bool flag;

	channelContribution()
	{
		inpSpw = 0;
		inpChannel = 0;
		outChannel = 0;
		weight = 0;
		flag = false;
	}

	channelContribution(casacore::Int inputSpw, casacore::uInt inputChannel, casacore::uInt outputChannel,casacore::Double fraction)
	{
		inpSpw = inputSpw;
		inpChannel = inputChannel;
		outChannel = outputChannel;
		weight = fraction;
		flag = true;
	}
};

struct spwInfo {

	spwInfo()
	{
		initialize(0);
	}

	spwInfo(casacore::uInt nChannels)
	{
		initialize(nChannels);
	}

	spwInfo(casacore::Vector<casacore::Double> &chanFreq,casacore::Vector<casacore::Double> &chanWidth)
	{
		reset(chanFreq,chanWidth);
	}

	void reset(casacore::Vector<casacore::Double> &chanFreq,casacore::Vector<casacore::Double> &chanWidth)
	{
		initialize(chanFreq.size());
		CHAN_FREQ = chanFreq;
		CHAN_WIDTH = chanWidth;
		update();
	}

	void initialize(casacore::uInt nChannels)
	{
		NUM_CHAN = nChannels;
		CHAN_FREQ.resize(nChannels,false);
		CHAN_WIDTH.resize(nChannels,false);
		EFFECTIVE_BW.resize(nChannels,false);
		RESOLUTION.resize(nChannels,false);
		CHAN_FREQ_aux.resize(nChannels,false);
		TOTAL_BANDWIDTH = 0;
		REF_FREQUENCY = 0;
		upperBound = 0;
		lowerBound = 0;
	}

	void update()
	{
		if (CHAN_FREQ(NUM_CHAN - 1) > CHAN_FREQ(0)) {
			upperBound = CHAN_FREQ(NUM_CHAN-1) + 0.5 * std::abs(CHAN_WIDTH(NUM_CHAN-1));
			lowerBound = CHAN_FREQ(0) - 0.5 * std::abs(CHAN_WIDTH(0));
			// ensure width is positive
			for (auto it = CHAN_WIDTH.begin(); it != CHAN_WIDTH.end(); ++it) {
				*it = std::abs(*it);
			}
		}
		else {
			upperBound = CHAN_FREQ(0) + 0.5 * std::abs(CHAN_WIDTH(0));
			lowerBound = CHAN_FREQ(NUM_CHAN-1) - 0.5 * std::abs(CHAN_WIDTH(NUM_CHAN-1));
			// ensure width is negative, may not be the case regridding (without combine)
			// preserves the sign but the width computation is always positive due to use
			// of original combine+regrid cvel code that converts all channels to ascending
			for (auto it = CHAN_WIDTH.begin(); it != CHAN_WIDTH.end(); ++it) {
				if (*it > 0) {
					*it = -*it;
				}
			}
		}

		TOTAL_BANDWIDTH = upperBound - lowerBound;
		REF_FREQUENCY = CHAN_FREQ(0);

		CHAN_FREQ_aux = CHAN_FREQ;
		EFFECTIVE_BW = CHAN_WIDTH;
		RESOLUTION = CHAN_WIDTH;
	}

	void resize(casacore::uInt nChannels)
	{
		NUM_CHAN = nChannels;
		CHAN_FREQ.resize(nChannels,true);
		CHAN_WIDTH.resize(nChannels,true);
		EFFECTIVE_BW.resize(nChannels,true);
		RESOLUTION.resize(nChannels,true);
		CHAN_FREQ_aux.resize(nChannels,true);
		update();
	}

	casacore::uInt NUM_CHAN;
	casacore::Vector<casacore::Double> CHAN_FREQ;
	casacore::Vector<casacore::Double> CHAN_WIDTH;
	casacore::Vector<casacore::Double> EFFECTIVE_BW;
	casacore::Vector<casacore::Double> RESOLUTION;
	casacore::Vector<casacore::Double> CHAN_FREQ_aux;
	casacore::Double TOTAL_BANDWIDTH;
	casacore::Double REF_FREQUENCY;
	casacore::Double upperBound;
	casacore::Double lowerBound;
};

//  MSTransformManager definition
class MSTransformManager
{

	friend class MSTransformBufferImpl;
	friend class MSTransformIterator;
	friend class MSTransformIteratorFactory;

public:

	MSTransformManager();
	MSTransformManager(casacore::Record configuration);

	virtual ~MSTransformManager();

	void initialize();
	void configure(casacore::Record &configuration);

	void open();
	void setup();
	void close();

	void setupBufferTransformations(vi::VisBuffer2 *vb);
	void fillOutputMs(vi::VisBuffer2 *vb);

	// For buffer handling classes (MSTransformIterator)

	// Needed by MSTransformIteratorFactory
	vi::VisibilityIterator2 * getVisIter() {return visibilityIterator_p;}

	// Needed by MSTransformIterator
	casacore::MeasurementSet * getOutputMs () {return outputMs_p;};
	casacore::String getOutputMsName () {return outMsName_p;};

	// Needed by MSTransformBuffer
	vi::VisBuffer2 * getVisBuffer() {return visibilityIterator_p->getVisBuffer();}
	casacore::IPosition getShape();
	casacore::IPosition getTransformedShape(vi::VisBuffer2 *inputVisBuffer);

	// Need by tMSTransformIterator
	dataColMap getDataColMap() { return dataColMap_p;}



protected:

	void parseMsSpecParams(casacore::Record &configuration);
	void parseDataSelParams(casacore::Record &configuration);
	void parseFreqTransParams(casacore::Record &configuration);
	void parseChanAvgParams(casacore::Record &configuration);
	void parseRefFrameTransParams(casacore::Record &configuration);
	void parseFreqSpecParams(casacore::Record &configuration);
	void parsePhaseShiftParams(casacore::Record &configuration);
	void parseTimeAvgParams(casacore::Record &configuration);
	void parseCalParams(casacore::Record &configuration);
	void parseUVContSubParams(casacore::Record &configuration);
	void parsePhaseShiftSubParams(casacore::Record &configuration);
	void setSpwAvg(casacore::Record &configuration);
	void parsePolAvgParams(casacore::Record &configuration);
	void parsePointingsInterpolationParams(casacore::Record &configuration);
	void parseAtmCorrectionParams(casacore::Record &configuration);

	// From input MS
	void initDataSelectionParams();
	void getInputNumberOfChannels();

	// To re-grid SPW subtable
	void initRefFrameTransParams();
	casacore::MFrequency::Types determineInputRefFrame();
	casacore::MDirection determinePhaseCenter();
	void regridSpwSubTable();
	void regridAndCombineSpwSubtable();
	/// Regrids one SPW
	void regridSpwAux(casacore::Int spwId, casacore::MFrequency::Types spwInputRefFrame,
			  casacore::Vector<casacore::Double> &inputCHAN_FREQ,
			  casacore::Vector<casacore::Double> &inputCHAN_WIDTH,
			  casacore::Vector<casacore::Double> &originalCHAN_FREQ,
			  casacore::Vector<casacore::Double> &originalCHAN_WIDTH,
			  casacore::Vector<casacore::Double> &regriddedCHAN_FREQ,
			  casacore::Vector<casacore::Double> &regriddedCHAN_WIDTH,
			  string msg);

	void reindexColumn(casacore::ScalarColumn<casacore::Int> &inputCol, casacore::Int value);
	void reindexSourceSubTable();
	void reindexDDISubTable();
	void reindexFeedSubTable();
	void reindexSysCalSubTable();
	void reindexFreqOffsetSubTable();
	void reindexGenericTimeDependentSubTable(const casacore::String& subtabname);

	void separateSpwSubtable();
	void separateFeedSubtable();
	void separateSourceSubtable();
	void separateSyscalSubtable();
	void separateFreqOffsetSubtable();
	void separateCalDeviceSubtable();
	void separateSysPowerSubtable();

	// To average polarization components
	casacore::Int getAveragedPolarizationId();
	void reindexPolarizationIdInDataDesc(casacore::Int newPolarizationId);

	// Setters for Weight-based transformation
	void propagateWeights(casacore::Bool on);
	void setBufferMode(casacore::Bool on);
	void setChannelAverageKernel(casacore::uInt mode);
	void setSmoothingKernel(casacore::uInt mode);
	void setSmoothingFourierKernel(casacore::uInt mode);

	// Drop channels with non-uniform width when doing channel average
	void dropNonUniformWidthChannels();

	// From output MS
	void getOutputNumberOfChannels();

	void checkAndPreaverageChannelsIfNeeded(Int spwId,
						Vector<Double> &inputCHAN_FREQ,
						Vector<Double> &inputCHAN_WIDTH,
						const Vector<Double> &originalCHAN_FREQ,
						const Vector<Double> &originalCHAN_WIDTH,
						const Vector<Double> &regriddedCHAN_FREQ,
						const Vector<Double> &regriddedCHAN_WIDTH);
	void doPreAveragingBeforeRegridding(uInt widthFactor, Int spwId,
					    const Vector<Double> &originalCHAN_FREQ,
					    const Vector<Double> &originalCHAN_WIDTH,
					    Vector<Double> &inputCHAN_FREQ,
					    Vector<Double> &inputCHAN_WIDTH);

	// For channel averaging and selection
	void calculateIntermediateFrequencies(casacore::Int spwId,
					      const casacore::Vector<casacore::Double> &inputChanFreq,
					      const casacore::Vector<casacore::Double> &inputChanWidth,
					      casacore::Vector<casacore::Double> &intermediateChanFreq,
					      casacore::Vector<casacore::Double> &intermediateChanWidth);

	void initGridForRegridTClean(const Vector<Double> &originalCHAN_FREQ,
				     const Vector<Double> &regriddedCHAN_FREQ,
				     const Vector<Double> &regriddedCHAN_WIDTH,
				     Double widthFactor);


	void calculateWeightAndSigmaFactors();
	void calculateNewWeightAndSigmaFactors();

	// Column check
	void checkFillFlagCategory();
	void checkFillWeightSpectrum();
	void checkDataColumnsAvailable();
	void checkDataColumnsToFill();
	void colCheckInfo(const casacore::String& inputColName, const casacore::String& outputColName);
	void checkSPWChannelsKnownLimitation();
	void checkCorrelatorPreaveraging();

	// Iterator set-up
	virtual void setIterationApproach();
	void generateIterator();

	void initFrequencyTransGrid(vi::VisBuffer2 *vb);
	void fillIdCols(vi::VisBuffer2 *vb,casacore::RefRows &rowRef);
	void fillDataCols(vi::VisBuffer2 *vb,casacore::RefRows &rowRef);

	void fillWeightCols(vi::VisBuffer2 *vb,casacore::RefRows &rowRef);
	void transformAndWriteSpectrum(	vi::VisBuffer2 *vb,
									casacore::RefRows &rowRef,
									const casacore::Cube<casacore::Float> &inputSpectrum,
									casacore::ArrayColumn<casacore::Float> &outputCubeCol,
									casacore::ArrayColumn<casacore::Float> &outputMatrixCol,
									MSTransformations::weightTransformation weightTransformation,
									casacore::Bool flushSpectrumCube);

	template <class T> void setTileShape(casacore::RefRows &rowRef,casacore::ArrayColumn<T> &outputDataCol);

	const casacore::Cube<casacore::Float>& getApplicableSpectrum(vi::VisBuffer2 *vb, casacore::MS::PredefinedColumns datacol);
	casacore::ArrayColumn<casacore::Float>& getOutputWeightColumn(vi::VisBuffer2 *vb, casacore::MS::PredefinedColumns datacol);
	const casacore::Cube<casacore::Float>& getWeightSpectrumFromSigmaSpectrum(vi::VisBuffer2 *vb);
	const casacore::Cube<casacore::Float>& getWeightSpectrumFlat(vi::VisBuffer2 *vb);

	// Methods to transform and write vectors

	template <class T> void transformAndWriteNotReindexableVector(	const casacore::Vector<T> &inputVector,
																	casacore::Vector<T> &outputVector,
																	casacore::Bool constant,
																	casacore::ScalarColumn<T> &outputCol,
																	casacore::RefRows &rowReference);

	template <class T> void transformAndWriteReindexableVector(	const casacore::Vector<T> &inputVector,
																casacore::Vector<T> &outputVector,
																casacore::Bool constant,
																map<casacore::uInt,casacore::uInt> &inputOutputIndexMap,
																casacore::ScalarColumn<T> &outputCol,
																casacore::RefRows &rowReference);

	casacore::Bool transformDDIVector(const casacore::Vector<casacore::Int> &inputVector,casacore::Vector<casacore::Int> &outputVector);

	void mapAndAverageVector(	const casacore::Vector<casacore::Double> &inputVector,
								casacore::Vector<casacore::Double> &outputVector);

	void mapAndAverageVector(	const casacore::Vector<casacore::Bool> &inputVector,
								casacore::Vector<casacore::Bool> &outputVector);

	// Templates methods to transform vectors that must be available for MSTransformBuffer

	template <class T> casacore::Bool transformNotReindexableVector(	const casacore::Vector<T> &inputVector,
															casacore::Vector<T> &outputVector,
															casacore::Bool constant)
	{
		casacore::Bool transformed = true;

		if ((combinespws_p) or (nspws_p >1))
		{
			if (constant)
			{
				outputVector = inputVector(0);
			}
			else
			{
				mapVector(inputVector,outputVector);
			}
		}
		else
		{
			transformed = false;
		}

		return transformed;
	};

	template <class T> casacore::Bool transformReindexableVector(	const casacore::Vector<T> &inputVector,
														casacore::Vector<T> &outputVector,
														casacore::Bool constant,
														map<casacore::uInt,casacore::uInt> &inputOutputIndexMap)
	{
		casacore::Bool transformed = true;

		if (inputOutputIndexMap.size() == 0)
		{
			transformed = transformNotReindexableVector(inputVector,outputVector,constant);
		}
		else
		{
			if (constant)
			{
				outputVector = inputOutputIndexMap[inputVector(0)];
			}
			else if (combinespws_p)
			{
				mapAndReindexVector(inputVector,outputVector,inputOutputIndexMap);
			}
			else
			{
				reindexVector(inputVector,outputVector,inputOutputIndexMap);
			}
		}

		return transformed;
	};

	template <class T> void mapAndReindexVector(	const casacore::Vector<T> &inputVector,
													casacore::Vector<T> &outputVector,
													map<casacore::uInt,casacore::uInt> &inputOutputIndexMap)
	{
		if (nspws_p <2)
		{
			for (casacore::uInt index=0; index<rowIndex_p.size();index++)
			{
				outputVector(index) = inputOutputIndexMap[inputVector(rowIndex_p[index])];
			}
		}
		else
		{
			casacore::uInt absoluteIndex = 0;
			for (casacore::uInt index=0; index<rowIndex_p.size();index++)
			{
				for (casacore::uInt spwIndex=0;spwIndex < nspws_p; spwIndex++)
				{
					outputVector(absoluteIndex) = inputOutputIndexMap[inputVector(rowIndex_p[index])];
					absoluteIndex += 1;
				}
			}
		}

		return;
	}


	template <class T> void reindexVector(	const casacore::Vector<T> &inputVector,
											casacore::Vector<T> &outputVector,
											map<casacore::uInt,casacore::uInt> &inputOutputIndexMap)
	{
		if (nspws_p <2)
		{
			for (casacore::uInt index=0; index<inputVector.shape()[0];index++)
			{
				outputVector(index) = inputOutputIndexMap[inputVector(index)];
			}
		}
		else
		{
			casacore::uInt absoluteIndex = 0;
			for (casacore::uInt index=0; index<inputVector.shape()[0];index++)
			{
				for (casacore::uInt spwIndex=0;spwIndex < nspws_p; spwIndex++)
				{
					outputVector(absoluteIndex) = inputOutputIndexMap[inputVector(index)];
					absoluteIndex += 1;
				}
			}
		}

		return;
	};

	template <class T> void mapVector(	const casacore::Vector<T> &inputVector,
										casacore::Vector<T> &outputVector)
	{
		if (nspws_p < 2)
		{
			for (casacore::uInt index=0; index<rowIndex_p.size();index++)
			{
				outputVector(index) = inputVector(rowIndex_p[index]);
			}
		}
		else
		{
			casacore::uInt absoluteIndex = 0;
			for (casacore::uInt index=0; index<rowIndex_p.size();index++)
			{
				for (casacore::uInt spwIndex=0;spwIndex < nspws_p; spwIndex++)
				{
					outputVector(absoluteIndex) = inputVector(rowIndex_p[index]);
					absoluteIndex += 1;
				}
			}
		}


		return;
	}

	// ------------------------------------------------------------------------------------
	// Fill the data from an input matrix with shape [nCol,nBaselinesxnSPWsxnScans/nStates]
	// into an output matrix with shape [nCol,nBaselinesxnScans/nStates]
	// ------------------------------------------------------------------------------------
	template <class T> void mapMatrix(	const casacore::Matrix<T> &inputMatrix,casacore::Matrix<T> &outputMatrix)
	{
		// Get number of columns
		casacore::uInt nCols = outputMatrix.shape()(0);

		for (casacore::uInt index=0; index<rowIndex_p.size();index++)
		{
			for (casacore::uInt col = 0; col < nCols; col++)
			{
				outputMatrix(col,index) = inputMatrix(col,rowIndex_p[index]);
			}
		}

		return;
	}


	template <class T> void mapAndAverageMatrix(	const casacore::Matrix<T> &inputMatrix,
													casacore::Matrix<T> &outputMatrix,
													casacore::Bool convolveFlags=false,
													vi::VisBuffer2 *vb=NULL);
	template <class T> void mapAndScaleMatrix(	const casacore::Matrix<T> &inputMatrix,
												casacore::Matrix<T> &outputMatrix,
												map<casacore::uInt,T> scaleMap,
												casacore::Vector<casacore::Int> spws);
	template <class T> void writeMatrix(	const casacore::Matrix<T> &inputMatrix,
											casacore::ArrayColumn<T> &outputCol,
											casacore::RefRows &rowRef,
											casacore::uInt nBlocks);

	// Methods to transform and write cubes

	template <class T> void writeCube(	const casacore::Cube<T> &inputCube,
										casacore::ArrayColumn<T> &outputCol,
										casacore::RefRows &rowRef);

	void transformCubeOfData(	vi::VisBuffer2 *vb,
								casacore::RefRows &rowRef,
								const casacore::Cube<casacore::Complex> &inputDataCube,
								casacore::ArrayColumn<casacore::Complex> &outputDataCol,
								casacore::ArrayColumn<casacore::Bool> *outputFlagCol,
								const casacore::Cube<casacore::Float> &inputWeightCube);
	void transformCubeOfData(	vi::VisBuffer2 *vb,
								casacore::RefRows &rowRef,
								const casacore::Cube<casacore::Float> &inputDataCube,
								casacore::ArrayColumn<casacore::Float> &outputDataCol,
								casacore::ArrayColumn<casacore::Bool> *outputFlagCol,
								const casacore::Cube<casacore::Float> &inputWeightCube);
	void (casa::MSTransformManager::*transformCubeOfDataComplex_p)(	vi::VisBuffer2 *vb,
																		casacore::RefRows &rowRef,
																		const casacore::Cube<casacore::Complex> &inputDataCube,
																		casacore::ArrayColumn<casacore::Complex> &outputDataCol,
																		casacore::ArrayColumn<casacore::Bool> *outputFlagCol,
																		const casacore::Cube<casacore::Float> &inputWeightCube);
	void (casa::MSTransformManager::*transformCubeOfDataFloat_p)(	vi::VisBuffer2 *vb,
																		casacore::RefRows &rowRef,
																		const casacore::Cube<casacore::Float> &inputDataCube,
																		casacore::ArrayColumn<casacore::Float> &outputDataCol,
																		casacore::ArrayColumn<casacore::Bool> *outputFlagCol,
																		const casacore::Cube<casacore::Float> &inputWeightCube);

	template <class T> void copyCubeOfData(	vi::VisBuffer2 *vb,
											casacore::RefRows &rowRef,
											const casacore::Cube<T> &inputDataCube,
											casacore::ArrayColumn<T> &outputDataCol,
											casacore::ArrayColumn<casacore::Bool> *outputFlagCol,
											const casacore::Cube<casacore::Float> &inputWeightCube);

	template <class T> void combineCubeOfData(	vi::VisBuffer2 *vb,
												casacore::RefRows &rowRef,
												const casacore::Cube<T> &inputDataCube,
												casacore::ArrayColumn<T> &outputDataCol,
												casacore::ArrayColumn<casacore::Bool> *outputFlagCol,
												const casacore::Cube<casacore::Float> &inputWeightCube);

	// Methods to transform data in cubes

	void addWeightSpectrumContribution(	casacore::Double &weight,
										casacore::uInt &pol,
										casacore::uInt &inputChannel,
										casacore::uInt &row,
										const casacore::Cube<casacore::Float> &inputWeightsCube);
	void dontAddWeightSpectrumContribution(	casacore::Double &weight,
											casacore::uInt &pol,
											casacore::uInt &inputChannel,
											casacore::uInt &row,
											const casacore::Cube<casacore::Float> &inputWeightsCube);
	void (casa::MSTransformManager::*addWeightSpectrumContribution_p)(	casacore::Double &weight,
																			casacore::uInt &pol,
																			casacore::uInt &inputChannel,
																			casacore::uInt &row,
																			const casacore::Cube<casacore::Float> &inputWeightsCube);


	void fillWeightsPlane(	casacore::uInt pol,
							casacore::uInt inputChannel,
							casacore::uInt outputChannel,
							casacore::uInt inputRow,
							const casacore::Cube<casacore::Float> &inputWeightsCube,
							casacore::Matrix<casacore::Float> &inputWeightsPlane,
							casacore::Double weight);
	void dontfillWeightsPlane(	casacore::uInt ,
								casacore::uInt ,
								casacore::uInt ,
								casacore::uInt ,
								const casacore::Cube<casacore::Float> &,
								casacore::Matrix<casacore::Float> &,
								casacore::Double ) {return;}
	void (casa::MSTransformManager::*fillWeightsPlane_p)(	casacore::uInt pol,
																casacore::uInt inputChannel,
																casacore::uInt outputChannel,
																casacore::uInt inputRow,
																const casacore::Cube<casacore::Float> &inputWeightsCube,
																casacore::Matrix<casacore::Float> &inputWeightsPlane,
																casacore::Double weight);

	void normalizeWeightsPlane(	casacore::uInt pol,
								casacore::uInt outputChannel,
								casacore::Matrix<casacore::Float> &inputPlaneWeights,
								casacore::Matrix<casacore::Double> &normalizingFactorPlane);
	void dontNormalizeWeightsPlane(	casacore::uInt ,
									casacore::uInt ,
									casacore::Matrix<casacore::Float> &,
									casacore::Matrix<casacore::Double> &) {return;}
	void (casa::MSTransformManager::*normalizeWeightsPlane_p)(	casacore::uInt pol,
																	casacore::uInt outputChannel,
																	casacore::Matrix<casacore::Float> &inputPlaneWeights,
																	casacore::Matrix<casacore::Double> &normalizingFactorPlane);

	template <class T> void averageCubeOfData(	vi::VisBuffer2 *vb,
												casacore::RefRows &rowRef,
												const casacore::Cube<T> &inputDataCube,
												casacore::ArrayColumn<T> &outputDataCol,
												casacore::ArrayColumn<casacore::Bool> *outputFlagCol,
												const casacore::Cube<casacore::Float> &inputWeightCube);
	template <class T> void smoothCubeOfData(	vi::VisBuffer2 *vb,
												casacore::RefRows &rowRef,
												const casacore::Cube<T> &inputDataCube,
												casacore::ArrayColumn<T> &outputDataCol,
												casacore::ArrayColumn<casacore::Bool> *outputFlagCol,
												const casacore::Cube<casacore::Float> &inputWeightCube);
	template <class T> void regridCubeOfData(	vi::VisBuffer2 *vb,
												casacore::RefRows &rowRef,
												const casacore::Cube<T> &inputDataCube,
												casacore::ArrayColumn<T> &outputDataCol,
												casacore::ArrayColumn<casacore::Bool> *outputFlagCol,
												const casacore::Cube<casacore::Float> &inputWeightCube);
	template <class T> void separateCubeOfData(	vi::VisBuffer2 *vb,
												casacore::RefRows &rowRef,
												const casacore::Cube<T> &inputDataCube,
												casacore::ArrayColumn<T> &outputDataCol,
												casacore::ArrayColumn<casacore::Bool> *outputFlagCol,
												const casacore::Cube<casacore::Float> &inputWeightCube);

	template <class T> void transformAndWriteCubeOfData(	casacore::Int inputSpw,
															casacore::RefRows &rowRef,
															const casacore::Cube<T> &inputDataCube,
															const casacore::Cube<casacore::Bool> &inputFlagsCube,
															const casacore::Cube<casacore::Float> &inputWeightsCube,
															casacore::IPosition &outputPlaneShape,
															casacore::ArrayColumn<T> &outputDataCol,
															casacore::ArrayColumn<casacore::Bool> *outputFlagCol);


	void setWeightsPlaneByReference(	casacore::uInt inputRow,
										const casacore::Cube<casacore::Float> &inputWeightsCube,
										casacore::Matrix<casacore::Float> &inputWeightsPlane);
	void dontsetWeightsPlaneByReference(	casacore::uInt ,
											const casacore::Cube<casacore::Float> &,
											casacore::Matrix<casacore::Float> &) {return;}
	void (casa::MSTransformManager::*setWeightsPlaneByReference_p)(	casacore::uInt inputRow,
																		const casacore::Cube<casacore::Float> &inputWeightsCube,
																		casacore::Matrix<casacore::Float> &inputWeightsPlane);

	template <class T> void transformAndWritePlaneOfData(	casacore::Int inputSpw,
															casacore::uInt row,
															casacore::Matrix<T> &inputDataPlane,
															casacore::Matrix<casacore::Bool> &inputFlagsPlane,
															casacore::Matrix<casacore::Float> &inputWeightsPlane,
															casacore::Matrix<T> &outputDataPlane,
															casacore::Matrix<casacore::Bool> &outputFlagsPlane,
															casacore::ArrayColumn<T> &outputDataCol,
															casacore::ArrayColumn<casacore::Bool> *outputFlagCol);
	void setWeightStripeByReference(	casacore::uInt corrIndex,
										casacore::Matrix<casacore::Float> &inputWeightsPlane,
										casacore::Vector<casacore::Float> &inputWeightsStripe);
	void dontSetWeightStripeByReference(	casacore::uInt ,
											casacore::Matrix<casacore::Float> &,
											casacore::Vector<casacore::Float> &) {return;}
	void (casa::MSTransformManager::*setWeightStripeByReference_p)(	casacore::uInt corrIndex,
																		casacore::Matrix<casacore::Float> &inputWeightsPlane,
																		casacore::Vector<casacore::Float> &inputWeightsStripe);

	void setOutputbuffer(casacore::Cube<casacore::Complex> *& dataBufferPointer,casacore::Cube<casacore::Bool> *& flagBufferPointer);
	void setOutputbuffer(casacore::Cube<casacore::Float> *& dataBufferPointer,casacore::Cube<casacore::Bool> *& flagBufferPointer);

	template <class T> void bufferOutputPlanes(	casacore::uInt row,
												casacore::Matrix<T> &outputDataPlane,
												casacore::Matrix<casacore::Bool> &outputFlagsPlane,
												casacore::ArrayColumn<T> &outputDataCol,
												casacore::ArrayColumn<casacore::Bool> &outputFlagCol);
	template <class T> void bufferOutputPlanesInSlices(	casacore::uInt row,
														casacore::Matrix<T> &outputDataPlane,
														casacore::Matrix<casacore::Bool> &outputFlagsPlane,
														casacore::ArrayColumn<T> &outputDataCol,
														casacore::ArrayColumn<casacore::Bool> &outputFlagCol);

	void writeOutputPlanes(	casacore::uInt row,
							casacore::Matrix<casacore::Complex> &outputDataPlane,
							casacore::Matrix<casacore::Bool> &outputFlagsPlane,
							casacore::ArrayColumn<casacore::Complex> &outputDataCol,
							casacore::ArrayColumn<casacore::Bool> &outputFlagCol);
	void writeOutputPlanes(	casacore::uInt row,
							casacore::Matrix<casacore::Float> &outputDataPlane,
							casacore::Matrix<casacore::Bool> &outputFlagsPlane,
							casacore::ArrayColumn<casacore::Float> &outputDataCol,
							casacore::ArrayColumn<casacore::Bool> &outputFlagCol);
	void (casa::MSTransformManager::*writeOutputPlanesComplex_p)(	casacore::uInt row,
																		casacore::Matrix<casacore::Complex> &outputDataPlane,
																		casacore::Matrix<casacore::Bool> &outputFlagsPlane,
																		casacore::ArrayColumn<casacore::Complex> &outputDataCol,
																		casacore::ArrayColumn<casacore::Bool> &outputFlagCol);
	void (casa::MSTransformManager::*writeOutputPlanesFloat_p)(	casacore::uInt row,
																	casacore::Matrix<casacore::Float> &outputDataPlane,
																	casacore::Matrix<casacore::Bool> &outputFlagsPlane,
																	casacore::ArrayColumn<casacore::Float> &outputDataCol,
																	casacore::ArrayColumn<casacore::Bool> &outputFlagCol);

	template <class T> void writeOutputPlanesInBlock(	casacore::uInt row,
														casacore::Matrix<T> &outputDataPlane,
														casacore::Matrix<casacore::Bool> &outputFlagsPlane,
														casacore::ArrayColumn<T> &outputDataCol,
														casacore::ArrayColumn<casacore::Bool> &outputFlagCol);
	void (casa::MSTransformManager::*writeOutputFlagsPlane_p)(	casacore::Matrix<casacore::Bool> &outputPlane,
																	casacore::ArrayColumn<casacore::Bool> &outputCol,
																	casacore::IPosition &outputPlaneShape,
																	casacore::uInt &outputRow);
	void writeOutputFlagsPlane(	casacore::Matrix<casacore::Bool> &outputPlane,
								casacore::ArrayColumn<casacore::Bool> &outputCol,
								casacore::IPosition &outputPlaneShape,
								casacore::uInt &outputRow);
	void dontWriteOutputFlagsPlane(	casacore::Matrix<casacore::Bool> &,
									casacore::ArrayColumn<casacore::Bool> &,
									casacore::IPosition &,
									casacore::uInt &) {return;}

	template <class T> void writeOutputPlanesInSlices(	casacore::uInt row,
														casacore::Matrix<T> &outputDataPlane,
														casacore::Matrix<casacore::Bool> &outputFlagsPlane,
														casacore::ArrayColumn<T> &outputDataCol,
														casacore::ArrayColumn<casacore::Bool> &outputFlagCol);
	template <class T> void writeOutputPlaneSlices(	casacore::Matrix<T> &outputPlane,
													casacore::ArrayColumn<T> &outputDataCol,
													casacore::Slice &sliceX,
													casacore::Slice &sliceY,
													casacore::IPosition &outputPlaneShape,
													casacore::uInt &outputRow);
	template <class T> void writeOutputPlaneReshapedSlices(	casacore::Matrix<T> &outputPlane,
															casacore::ArrayColumn<T> &outputDataCol,
															casacore::Slice &sliceX,
															casacore::Slice &sliceY,
															casacore::IPosition &outputPlaneShape,
															casacore::uInt &outputRow);
	void (casa::MSTransformManager::*writeOutputFlagsPlaneSlices_p)(	casacore::Matrix<casacore::Bool> &outputPlane,
																			casacore::ArrayColumn<casacore::Bool> &outputCol,
																			casacore::Slice &sliceX,
																			casacore::Slice &sliceY,
																			casacore::IPosition &outputPlaneShape,
																			casacore::uInt &outputRow);
	void writeOutputFlagsPlaneSlices(	casacore::Matrix<casacore::Bool> &outputPlane,
										casacore::ArrayColumn<casacore::Bool> &outputCol,
										casacore::Slice &sliceX,
										casacore::Slice &sliceY,
										casacore::IPosition &outputPlaneShape,
										casacore::uInt &outputRow);
	void dontWriteOutputFlagsPlaneSlices(	casacore::Matrix<casacore::Bool> &,
											casacore::ArrayColumn<casacore::Bool> &,
											casacore::Slice &,
											casacore::Slice &,
											casacore::IPosition &,
											casacore::uInt &) {return;}
	void (casa::MSTransformManager::*writeOutputFlagsPlaneReshapedSlices_p)(	casacore::Matrix<casacore::Bool> &outputPlane,
																					casacore::ArrayColumn<casacore::Bool> &outputCol,
																					casacore::Slice &sliceX,
																					casacore::Slice &sliceY,
																					casacore::IPosition &outputPlaneShape,
																					casacore::uInt &outputRow);
	void writeOutputFlagsPlaneReshapedSlices(	casacore::Matrix<casacore::Bool> &outputPlane,
												casacore::ArrayColumn<casacore::Bool> &outputCol,
												casacore::Slice &sliceX,
												casacore::Slice &sliceY,
												casacore::IPosition &outputPlaneShape,
												casacore::uInt &outputRow);
	void dontWriteOutputPlaneReshapedSlices(	casacore::Matrix<casacore::Bool> &,
												casacore::ArrayColumn<casacore::Bool> &,
												casacore::Slice &,
												casacore::Slice &,
												casacore::IPosition &,
												casacore::uInt &) {return;}

	void transformStripeOfData(casacore::Int inputSpw,
				   const casacore::Vector<casacore::Complex> &inputDataStripe,
				   const casacore::Vector<casacore::Bool> &inputFlagsStripe,
				   const casacore::Vector<casacore::Float> &inputWeightsStripe,
				   casacore::Vector<casacore::Complex> &outputDataStripe,
				   casacore::Vector<casacore::Bool> &outputFlagsStripe);

	void transformStripeOfData(casacore::Int inputSpw,
				   const casacore::Vector<casacore::Float> &inputDataStripe,
				   const casacore::Vector<casacore::Bool> &inputFlagsStripe,
				   const casacore::Vector<casacore::Float> &inputWeightsStripe,
				   casacore::Vector<casacore::Float> &outputDataStripe,
				   casacore::Vector<casacore::Bool> &outputFlagsStripe);

	void (casa::MSTransformManager::*transformStripeOfDataComplex_p)(casacore::Int inputSpw,
									 const casacore::Vector<casacore::Complex> &inputDataStripe,
									 const casacore::Vector<casacore::Bool> &inputFlagsStripe,
									 const casacore::Vector<casacore::Float> &inputWeightsStripe,
									 casacore::Vector<casacore::Complex> &outputDataStripe,
									 casacore::Vector<casacore::Bool> &outputFlagsStripe);

	void (casa::MSTransformManager::*transformStripeOfDataFloat_p)(casacore::Int inputSpw,
								       const casacore::Vector<casacore::Float> &inputDataStripe,
								       const casacore::Vector<casacore::Bool> &inputFlagsStripe,
								       const casacore::Vector<casacore::Float> &inputWeightsStripe,
								       casacore::Vector<casacore::Float> &outputDataStripe,
								       casacore::Vector<casacore::Bool> &outputFlagsStripe);

	template <class T> void average(casacore::Int inputSpw,
					const casacore::Vector<T> &inputDataStripe,
					const casacore::Vector<casacore::Bool> &inputFlagsStripe,
					const casacore::Vector<casacore::Float> &inputWeightsStripe,
					casacore::Vector<T> &outputDataStripe,
					casacore::Vector<casacore::Bool> &outputFlagsStripe);

	template <class T> void simpleAverage(casacore::uInt width,
					      const casacore::Vector<T> &inputData,
					      casacore::Vector<T> &outputData);

	void averageKernel(const casacore::Vector<casacore::Complex> &inputData,
			   const casacore::Vector<casacore::Bool> &inputFlags,
			   const casacore::Vector<casacore::Float> &inputWeights,
			   casacore::Vector<casacore::Complex> &outputData,
			   casacore::Vector<casacore::Bool> &outputFlags,
			   casacore::uInt startInputPos,
			   casacore::uInt outputPos,
			   casacore::uInt width);

	void averageKernel(const casacore::Vector<casacore::Float> &inputData,
			   const casacore::Vector<casacore::Bool> &inputFlags,
			   const casacore::Vector<casacore::Float> &inputWeights,
			   casacore::Vector<casacore::Float> &outputData,
			   casacore::Vector<casacore::Bool> &outputFlags,
			   casacore::uInt startInputPos,
			   casacore::uInt outputPos,
			   casacore::uInt width);

	void (casa::MSTransformManager::*averageKernelComplex_p)(const casacore::Vector<casacore::Complex> &inputData,
								 const casacore::Vector<casacore::Bool> &inputFlags,
								 const casacore::Vector<casacore::Float> &inputWeights,
								 casacore::Vector<casacore::Complex> &outputData,
								 casacore::Vector<casacore::Bool> &outputFlags,
								 casacore::uInt startInputPos,
																	casacore::uInt outputPos,
																	casacore::uInt width);
	void (casa::MSTransformManager::*averageKernelFloat_p)(const casacore::Vector<casacore::Float> &inputData,
							       const casacore::Vector<casacore::Bool> &inputFlags,
							       const casacore::Vector<casacore::Float> &inputWeights,
							       casacore::Vector<casacore::Float> &outputData,
							       casacore::Vector<casacore::Bool> &outputFlags,
							       casacore::uInt startInputPos,
							       casacore::uInt outputPos,
																	casacore::uInt width);
	template <class T> void simpleAverageKernel(const casacore::Vector<T> &inputData,
						    const casacore::Vector<casacore::Bool> &,
						    const casacore::Vector<casacore::Float> &,
						    casacore::Vector<T> &outputData,
						    casacore::Vector<casacore::Bool> &,
						    casacore::uInt startInputPos,
						    casacore::uInt outputPos,
						    casacore::uInt width);

	template <class T> void flagAverageKernel(const casacore::Vector<T> &inputData,
						  const casacore::Vector<casacore::Bool> &inputFlags,
						  const casacore::Vector<casacore::Float> &,
						  casacore::Vector<T> &outputData,
						  casacore::Vector<casacore::Bool> &outputFlags,
						  casacore::uInt startInputPos,
						  casacore::uInt outputPos,
						  casacore::uInt width);

	template <class T> void weightAverageKernel(const casacore::Vector<T> &inputData,
						    const casacore::Vector<casacore::Bool> &,
						    const casacore::Vector<casacore::Float> &inputWeights,
						    casacore::Vector<T> &outputData,
						    casacore::Vector<casacore::Bool> &outputFlags,
						    casacore::uInt startInputPos,
						    casacore::uInt outputPos,
						    casacore::uInt width);

	template <class T> void cumSumKernel(const casacore::Vector<T> &inputData,
					     const casacore::Vector<casacore::Bool> &,
					     const casacore::Vector<casacore::Float> &,
					     casacore::Vector<T> &outputData,
					     casacore::Vector<casacore::Bool> &,
					     casacore::uInt startInputPos,
					     casacore::uInt outputPos,
					     casacore::uInt width);

	template <class T> void flagWeightAverageKernel(const casacore::Vector<T> &inputData,
							const casacore::Vector<casacore::Bool> &inputFlags,
							const casacore::Vector<casacore::Float> &inputWeights,
							casacore::Vector<T> &outputData,
							casacore::Vector<casacore::Bool> &outputFlags,
							casacore::uInt startInputPos,
							casacore::uInt outputPos,
							casacore::uInt width);

	template <class T> void flagCumSumKernel(const casacore::Vector<T> &inputData,
						 const casacore::Vector<casacore::Bool> &inputFlags,
						 const casacore::Vector<casacore::Float> &,
						 casacore::Vector<T> &outputData,
						 casacore::Vector<casacore::Bool> &,
						 casacore::uInt startInputPos,
						 casacore::uInt outputPos,
						 casacore::uInt width);

	template <class T> void flagNonZeroAverageKernel(const casacore::Vector<T> &inputData,
							 const casacore::Vector<casacore::Bool> &inputFlags,
							 const casacore::Vector<casacore::Float> &,
							 casacore::Vector<T> &outputData,
							 casacore::Vector<casacore::Bool> &,
							 casacore::uInt startInputPos,
							 casacore::uInt outputPos,
							 casacore::uInt width);

	template <class T> void flagWeightNonZeroAverageKernel(const casacore::Vector<T> &inputData,
							       const casacore::Vector<casacore::Bool> &inputFlags,
							       const casacore::Vector<casacore::Float> &,
							       casacore::Vector<T> &outputData,
							       casacore::Vector<casacore::Bool> &,
							       casacore::uInt startInputPos,
							       casacore::uInt outputPos,
							       casacore::uInt width);

	template <class T> void flagCumSumNonZeroKernel(const casacore::Vector<T> &inputData,
							const casacore::Vector<casacore::Bool> &inputFlags,
							const casacore::Vector<casacore::Float> &,
							casacore::Vector<T> &outputData,
							casacore::Vector<casacore::Bool> &,
							casacore::uInt startInputPos,
							casacore::uInt outputPos,
							casacore::uInt width);


	template <class T> void smooth(casacore::Int inputSpw,
				       const casacore::Vector<T> &inputDataStripe,
				       const casacore::Vector<casacore::Bool> &inputFlagsStripe,
				       const casacore::Vector<casacore::Float> &inputWeightsStripe,
				       casacore::Vector<T> &outputDataStripe,
				       casacore::Vector<casacore::Bool> &outputFlagsStripe);

	void smoothKernel(const casacore::Vector<casacore::Complex> &inputData,
			  const casacore::Vector<casacore::Bool> &inputFlags,
			  const casacore::Vector<casacore::Float> &inputWeights,
			  casacore::Vector<casacore::Complex> &outputData,
			  casacore::Vector<casacore::Bool> &outputFlags,
			  casacore::uInt outputPos);

	void smoothKernel(const casacore::Vector<casacore::Float> &inputData,
			  const casacore::Vector<casacore::Bool> &inputFlags,
			  const casacore::Vector<casacore::Float> &inputWeights,
			  casacore::Vector<casacore::Float> &outputData,
			  casacore::Vector<casacore::Bool> &outputFlags,
			  casacore::uInt outputPos);

	void (casa::MSTransformManager::*smoothKernelComplex_p)(const casacore::Vector<casacore::Complex> &inputData,
								const casacore::Vector<casacore::Bool> &inputFlags,
								const casacore::Vector<casacore::Float> &inputWeights,
								casacore::Vector<casacore::Complex> &outputData,
								casacore::Vector<casacore::Bool> &outputFlags,
								casacore::uInt outputPos);

	void (casa::MSTransformManager::*smoothKernelFloat_p)(const casacore::Vector<casacore::Float> &inputData,
							      const casacore::Vector<casacore::Bool> &inputFlags,
							      const casacore::Vector<casacore::Float> &inputWeights,
							      casacore::Vector<casacore::Float> &outputData,
							      casacore::Vector<casacore::Bool> &outputFlags,
							      casacore::uInt outputPos);

	template <class T> void plainSmooth(const casacore::Vector<T> &inputData,
					    const casacore::Vector<casacore::Bool> &inputFlags,
					    const casacore::Vector<casacore::Float> &inputWeights,
					    casacore::Vector<T> &outputData,
					    casacore::Vector<casacore::Bool> &outputFlags,
					    casacore::uInt outputPos);

	template <class T> void plainSmoothSpectrum(const casacore::Vector<T> &inputData,
						    const casacore::Vector<casacore::Bool> &inputFlags,
						    const casacore::Vector<casacore::Float> &inputWeights,
						    casacore::Vector<T> &outputData,
						    casacore::Vector<casacore::Bool> &outputFlags,
						    casacore::uInt outputPos);


	template <class T> void regrid(casacore::Int ,
				       const casacore::Vector<T> &inputDataStripe,
				       const casacore::Vector<casacore::Bool> &inputFlagsStripe,
				       const casacore::Vector<casacore::Float> &,
				       casacore::Vector<T> &outputDataStripe,
				       casacore::Vector<casacore::Bool> &outputFlagsStripe);

	void regridCore(casacore::Int inputSpw,
			const casacore::Vector<casacore::Complex> &inputDataStripe,
			const casacore::Vector<casacore::Bool> &inputFlagsStripe,
			const casacore::Vector<casacore::Float> &inputWeightsStripe,
			casacore::Vector<casacore::Complex> &outputDataStripe,
			casacore::Vector<casacore::Bool> &outputFlagsStripe);
	void regridCore(casacore::Int inputSpw,
			const casacore::Vector<casacore::Float> &inputDataStripe,
			const casacore::Vector<casacore::Bool> &inputFlagsStripe,
			const casacore::Vector<casacore::Float> &inputWeightsStripe,
			casacore::Vector<casacore::Float> &outputDataStripe,
			casacore::Vector<casacore::Bool> &outputFlagsStripe);

	void (casa::MSTransformManager::*regridCoreComplex_p)(casacore::Int inputSpw,
							      const casacore::Vector<casacore::Complex> &inputDataStripe,
							      const casacore::Vector<casacore::Bool> &inputFlagsStripe,
							      const casacore::Vector<casacore::Float> &inputWeightsStripe,
							      casacore::Vector<casacore::Complex> &outputDataStripe,
							      casacore::Vector<casacore::Bool> &outputFlagsStripe);

	void (casa::MSTransformManager::*regridCoreFloat_p)(casacore::Int inputSpw,
							    const casacore::Vector<casacore::Float> &inputDataStripe,
							    const casacore::Vector<casacore::Bool> &inputFlagsStripe,
							    const casacore::Vector<casacore::Float> &inputWeightsStripe,
							    casacore::Vector<casacore::Float> &outputDataStripe,
							    casacore::Vector<casacore::Bool> &outputFlagsStripe);

	void fftshift(casacore::Int inputSpw,
		      const casacore::Vector<casacore::Complex> &inputDataStripe,
		      const casacore::Vector<casacore::Bool> &inputFlagsStripe,
		      const casacore::Vector<casacore::Float> &inputWeightsStripe,
		      casacore::Vector<casacore::Complex> &outputDataStripe,
		      casacore::Vector<casacore::Bool> &outputFlagsStripe);
	void fftshift(casacore::Int inputSpw,
		      const casacore::Vector<casacore::Float> &inputDataStripe,
		      const casacore::Vector<casacore::Bool> &inputFlagsStripe,
		      const casacore::Vector<casacore::Float> &inputWeightsStripe,
		      casacore::Vector<casacore::Float> &outputDataStripe,
		      casacore::Vector<casacore::Bool> &outputFlagsStripe);

	template <class T> void interpol1D(casacore::Int inputSpw,
					   const casacore::Vector<T> &inputDataStripe,
					   const casacore::Vector<casacore::Bool> &inputFlagsStripe,
					   const casacore::Vector<casacore::Float> &,
					   casacore::Vector<T> &outputDataStripe,
					   casacore::Vector<casacore::Bool> &outputFlagsStripe);

	template <class T> void interpol1Dfftshift(casacore::Int inputSpw,
						   const casacore::Vector<T> &inputDataStripe,
						   const casacore::Vector<casacore::Bool> &inputFlagsStripe,
						   const casacore::Vector<casacore::Float> &inputWeightsStripe,
						   casacore::Vector<T> &outputDataStripe,
						   casacore::Vector<casacore::Bool> &outputFlagsStripe);

	template <class T> void interpolateByChannelMap(Int spw,
							const Vector<T> &inputDataStripe,
							const Vector<Bool> &inputFlagsStripe,
							Vector<T> &outputDataStripe,
							Vector<Bool> &outputFlagsStripe);

	template <class T> void averageRegrid(casacore::Int inputSpw,
					      const casacore::Vector<T> &inputDataStripe,
					      const casacore::Vector<casacore::Bool> &inputFlagsStripe,
					      const casacore::Vector<casacore::Float> &inputWeightsStripe,
					      casacore::Vector<T> &outputDataStripe,
					      casacore::Vector<casacore::Bool> &outputFlagsStripe);

	template <class T> void smoothRegrid(casacore::Int inputSpw,
					     const casacore::Vector<T> &inputDataStripe,
					     const casacore::Vector<casacore::Bool> &inputFlagsStripe,
					     const casacore::Vector<casacore::Float> &inputWeightsStripe,
					     casacore::Vector<T> &outputDataStripe,
					     casacore::Vector<casacore::Bool> &outputFlagsStripe);

	template <class T> void averageSmooth(casacore::Int inputSpw,
					      const casacore::Vector<T> &inputDataStripe,
					      const casacore::Vector<casacore::Bool> &inputFlagsStripe,
					      const casacore::Vector<casacore::Float> &inputWeightsStripe,
					      casacore::Vector<T> &outputDataStripe,
					      casacore::Vector<casacore::Bool> &outputFlagsStripe);

	template <class T> void averageSmoothRegrid(casacore::Int inputSpw,
						    const casacore::Vector<T> &inputDataStripe,
						    const casacore::Vector<casacore::Bool> &inputFlagsStripe,
						    const casacore::Vector<casacore::Float> &inputWeightsStripe,
						    casacore::Vector<T> &outputDataStripe,
						    casacore::Vector<casacore::Bool> &outputFlagsStripe);

	// The following methods are single dish specific so far
	void smoothFourierFloat(casacore::Int , const casacore::Vector<casacore::Float> &inputDataStripe,
				const casacore::Vector<casacore::Bool> &inputFlagsStripe,
				const casacore::Vector<casacore::Float> &inputWeightStripe,
				casacore::Vector<casacore::Float> &outputDataStripe,
				casacore::Vector<casacore::Bool> &outputFlagsStripe);

	void smoothFourierComplex(casacore::Int , const casacore::Vector<casacore::Complex> &inputDataStripe,
				  const casacore::Vector<casacore::Bool> &inputFlagsStripe,
				  const casacore::Vector<casacore::Float> &inputWeightStripe,
				  casacore::Vector<casacore::Complex> &outputDataStripe,
				  casacore::Vector<casacore::Bool> &outputFlagsStripe);

	casacore::Convolver<casacore::Float> *getConvolver(casacore::Int const numChan);

	// casacore::MS specification parameters
	casacore::String inpMsName_p;
	casacore::String outMsName_p;
	casacore::String datacolumn_p;
	casacore::Bool makeVirtualModelColReal_p;
	casacore::Bool makeVirtualCorrectedColReal_p;
	casacore::Vector<casacore::Int> tileShape_p;

	// casacore::Data selection parameters
	casacore::String arraySelection_p;
	casacore::String fieldSelection_p;
	casacore::String scanSelection_p;
	casacore::String timeSelection_p;
	casacore::String spwSelection_p;
	casacore::String baselineSelection_p;
	casacore::String uvwSelection_p;
	casacore::String polarizationSelection_p;
	casacore::String scanIntentSelection_p;
	casacore::String observationSelection_p;
	casacore::String taqlSelection_p;
	casacore::String feedSelection_p;

	// casacore::Input-Output index maps
	map<casacore::uInt,casacore::uInt> inputOutputObservationIndexMap_p;
	map<casacore::uInt,casacore::uInt> inputOutputArrayIndexMap_p;
	map<casacore::uInt,casacore::uInt> inputOutputScanIndexMap_p;
	map<casacore::uInt,casacore::uInt> inputOutputScanIntentIndexMap_p;
	map<casacore::uInt,casacore::uInt> inputOutputFieldIndexMap_p;
	map<casacore::uInt,casacore::uInt> inputOutputSPWIndexMap_p;
	map<casacore::uInt,casacore::uInt> inputOutputDDIndexMap_p;
	map<casacore::uInt,casacore::uInt> inputOutputAntennaIndexMap_p;
	map<casacore::uInt,casacore::uInt> outputInputSPWIndexMap_p;
	map<casacore::Int,std::vector<casacore::Int> > inputOutputChanIndexMap_p;

	// Frequency transformation parameters
  casacore::uInt nspws_p = 1;
	casacore::Int ddiStart_p = 0;
	casacore::Bool combinespws_p = false;
	casacore::Bool enableChanPreAverage_p = false;
	casacore::Bool channelAverage_p = false;
	casacore::Bool hanningSmooth_p = false;
	casacore::Bool refFrameTransformation_p = false;
	casacore::Vector<casacore::Int> freqbin_p = casacore::Vector<Int>(1,-1);
	// For when the interpolation needs to be done the tclean way
	// (output width > 2 input width). CAS-9853, CAS-9852
	casacore::Bool regridTClean_p = false;
	Vector<Double> regridTCleanCHAN_FREQ_p;
	Vector<Int> regridTCleanChanMap_p;
	casacore::String useweights_p = "flags";
	casacore::uInt weightmode_p = MSTransformations::flags;
	// Options are: nearest, linear, cubic, spline, fftshift
	casacore::String interpolationMethodPar_p = casacore::String("linear");
	casac::variant *phaseCenterPar_p = new casac::variant("");
	casacore::String restFrequency_p = casacore::String("");
	// Options are: LSRK, LSRD, BARY, GALACTO, LGROUP, CMB, GEO, or TOPO
	casacore::String outputReferenceFramePar_p = String("");
	casacore::Bool radialVelocityCorrection_p = false;
	casacore::Bool regridding_p = false ;
	casacore::uInt smoothBin_p = 3;
	casacore::uInt smoothmode_p = MSTransformations::plainSmooth;
	casacore::Vector<casacore::Float> smoothCoeff_p;

	// Frequency specification parameters
	casacore::String mode_p;
	casacore::String start_p;
	casacore::String width_p;
	int nChan_p;
	casacore::String velocityType_p;

	// Phase shifting parameters
	casacore::Bool phaseShifting_p;
	casacore::Double dx_p, dy_p;

	// CAS-12706 To run phase shift via a TVI which has
	// support for shifting across large offset/angles
	casacore::Bool tviphaseshift_p;
	casacore::Record tviphaseshiftConfig_p;

    // For scalar averaging, use "timebin" for iter interval but don't average
	casacore::Bool scalarAverage_p;

	// casacore::Time transformation parameters
	casacore::Bool timeAverage_p;
	casacore::Double timeBin_p;
	casacore::String timespan_p;
	vi::AveragingOptions timeAvgOptions_p;
	casacore::Double maxuvwdistance_p;
	// casacore::uInt minbaselines_p;

	// Calibration parameters
	casacore::Bool calibrate_p;
	casacore::String callib_p;
	casacore::Record callibRec_p;

	// UVContSub parameters
	casacore::Bool uvcontsub_p;
	casacore::Record uvcontsubRec_p;
        // To produce a MODEL column in output MS, not coming from the input MS MODEL
        // This is used by uvcontsub which from input DATA (or CORRECTED) can produde both
        // DATA and MODEL
        casacore::Bool produceModel_p = false;

	// Spw averaging parameters
	casacore::Bool spwAverage_p;

	// Polarization transformation parameters
	casacore::Bool polAverage_p;
	casacore::Record polAverageConfig_p;

	// Pointings interpolation transformation parameters
	casacore::Bool pointingsInterpolation_p;
	casacore::Record pointingsInterpolationConfig_p;

	// Weight Spectrum parameters
	casacore::Bool usewtspectrum_p;

	// Buffer handling parameters
	casacore::Bool bufferMode_p;
	casacore::Bool userBufferMode_p;
	casacore::Bool reindex_p;
	casacore::Bool factory_p;
	casacore::Bool interactive_p;

	// casacore::MS-related members
	MSTransformDataHandler *dataHandler_p;
	casacore::MeasurementSet *inputMs_p;
	casacore::MeasurementSet *selectedInputMs_p;
	casacore::MeasurementSet *outputMs_p;
	casacore::MSColumns *selectedInputMsCols_p;
	casacore::MSColumns *outputMsCols_p;
	std::shared_ptr<casacore::MSFieldColumns> inputMSFieldCols_p;

	// VI/VB related members
	casacore::Block<casacore::Int> sortColumns_p;
	vi::VisibilityIterator2 *visibilityIterator_p;
	vi::FrequencySelectionUsingChannels *channelSelector_p;

	// Output casacore::MS structure related members
	casacore::Bool inputFlagCategoryAvailable_p;
	casacore::Bool correctedToData_p;
	casacore::Bool bothDataColumnsAreOutput_p;
	casacore::Bool doingData_p;
	casacore::Bool doingCorrected_p;
	casacore::Bool doingModel_p;
	dataColMap dataColMap_p;
	casacore::MSMainEnums::PredefinedColumns mainColumn_p;
	casacore::uInt nRowsToAdd_p;

	// Frequency transformation members
	casacore::uInt chansPerOutputSpw_p;
	casacore::uInt tailOfChansforLastSpw_p;
	casacore::uInt interpolationMethod_p;
	baselineMap baselineMap_p;
	std::vector<casacore::uInt> rowIndex_p;
	inputSpwChanMap spwChannelMap_p;
	inputOutputSpwMap inputOutputSpwMap_p;
	inputOutputChanFactorMap inputOutputChanFactorMap_p;
	map<casacore::uInt,casacore::uInt> freqbinMap_p;
	map<casacore::uInt,casacore::uInt> numOfInpChanMap_p;
	map<casacore::uInt,casacore::uInt> numOfSelChanMap_p;
	map<casacore::uInt,casacore::uInt> numOfOutChanMap_p;
	map<casacore::uInt,casacore::uInt> numOfCombInputChanMap_p;
	map<casacore::uInt,casacore::uInt> numOfCombInterChanMap_p;
	map<casacore::uInt,casacore::Float> weightFactorMap_p;
	map<casacore::uInt,casacore::Float> sigmaFactorMap_p;
    map<casacore::uInt,casacore::Float> newWeightFactorMap_p;
	map<casacore::uInt,casacore::Float> newSigmaFactorMap_p;

	// Reference Frame Transformation members
	casacore::MFrequency::Types inputReferenceFrame_p;
	casacore::MFrequency::Types outputReferenceFrame_p;
	casacore::MPosition observatoryPosition_p;
	casacore::MEpoch referenceTime_p;
	casacore::MDirection phaseCenter_p;
	casacore::MRadialVelocity radialVelocity_p;
	casacore::MFrequency::Convert freqTransEngine_p;
	casacore::MFrequency::Convert refTimeFreqTransEngine_p;
    casacore::FFTServer<casacore::Float, casacore::Complex> fFFTServer_p;
    casacore::Bool fftShiftEnabled_p;
	casacore::Double fftShift_p;
	casacore::ROScalarMeasColumn<casacore::MEpoch> timeMeas_p;

	// Weight Spectrum members
	casacore::Bool spectrumTransformation_p;
	casacore::Bool propagateWeights_p;
	casacore::Bool inputWeightSpectrumAvailable_p;
	casacore::Bool createWeightSpectrumCols_p;
	// whether to create and fill the WEIGHT/SIMA_SPECTRUM columns in the output MS
	casacore::Bool flushWeightSpectrum_p;
	casacore::Bool weightSpectrumFlatFilled_p;
	casacore::Bool weightSpectrumFromSigmaFilled_p;
	casacore::Bool combinationOfSPWsWithDifferentExposure_p;
	casacore::Cube<casacore::Float> weightSpectrumCube_p;
	casacore::Cube<casacore::Float> weightSpectrumCubeFlat_p;
	casacore::Cube<casacore::Float> weightSpectrumCubeDummy_p;

	// Buffer handling members
	casacore::uInt dataBuffer_p;
	casacore::uInt relativeRow_p;
	casacore::Bool spectrumReshape_p;
	casacore::Bool cubeTransformation_p;
	casacore::Bool dataColumnAvailable_p;
	casacore::Bool correctedDataColumnAvailable_p;
	casacore::Bool modelDataColumnAvailable_p;
	casacore::Bool floatDataColumnAvailable_p;
	casacore::Bool lagDataColumnAvailable_p;
	casacore::Cube<casacore::Bool> *flagCube_p;
	casacore::Cube<casacore::Complex> *visCube_p;
	casacore::Cube<casacore::Complex> *visCubeCorrected_p;
	casacore::Cube<casacore::Complex> *visCubeModel_p;
	casacore::Cube<casacore::Float> *visCubeFloat_p;
	casacore::Cube<casacore::Float> *weightSpectrum_p;
	casacore::Cube<casacore::Float> *sigmaSpectrum_p;
	casacore::Matrix<casacore::Float> *weight_p;
	casacore::Matrix<casacore::Float> *sigma_p;
	casacore::ArrayColumn<casacore::Float> dummyWeightCol_p;

	// single dish specific
	casacore::Bool smoothFourier_p;
	map<casacore::Int, casacore::Convolver<casacore::Float> > convolverPool_;
	casacore::Bool doAtmCor_p;
	casacore::Record atmCorConfig_p;

	// Logging
	casacore::LogIO logger_p;

private:
	casacore::Bool shouldCreateOutputWtSpectrum(casacore::Bool usewtspectrum);
	void createOutputMSStructure();

};

} //# NAMESPACE CASA - END

#endif /* MSTransformManager_H_ */
