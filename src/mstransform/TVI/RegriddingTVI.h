//# RegriddingTVI.h: This file contains the interface definition of the MSTransformManager class.
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

#ifndef RegriddingTVI_H_
#define RegriddingTVI_H_

// Base class
#include <mstransform/TVI/FreqAxisTVI.h>

// To handle variant parameters
#include <stdcasa/StdCasa/CasacSupport.h>

// casacore::MS Access
#include <casacore/ms/MeasurementSets.h>

// To get observatory position from observatory name
#include <casacore/measures/Measures/MeasTable.h>

// calcChanFreqs
#include <mstransform/MSTransform/MSTransformManager.h>

// 1d interpolation methods
#include <casacore/scimath/Mathematics/InterpolateArray1D.h>

namespace casa { //# NAMESPACE CASA - BEGIN

namespace vi { //# NAMESPACE VI - BEGIN

//////////////////////////////////////////////////////////////////////////
// Enumerations (see equivalences in InterpolateArray1D.h)
//////////////////////////////////////////////////////////////////////////

enum RegriddingMethod {
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

const casacore::Unit Hz(casacore::String("Hz"));

//////////////////////////////////////////////////////////////////////////
// RegriddingTVI class
//////////////////////////////////////////////////////////////////////////

class RegriddingTVI : public FreqAxisTVI
{

public:

	RegriddingTVI(	ViImplementation2 * inputVii,
					const casacore::Record &configuration);

	// Report the the ViImplementation type
	virtual casacore::String ViiType() const { return casacore::String("Regridding( ")+getVii()->ViiType()+" )"; };

	// Navigation methods
	void origin ();

	// Frequency specification methods
    casacore::Int getReportingFrameOfReference () const; // Used by VisBufferImpl2 to determine default ref. frame
    casacore::Vector<casacore::Double> getFrequencies (	casacore::Double time, casacore::Int frameOfReference,
    								casacore::Int spectralWindowId, casacore::Int msId) const;
    // casacore::Data accessors
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

    void initFrequencyGrid();
    void initFrequencyTransformationEngine() const;

    template<class T> void transformDataCube(	const casacore::Cube<T> &inputVis,
    											casacore::Cube<T> &outputVis) const;

	int nChan_p;
	casacore::String mode_p;
	casacore::String start_p;
	casacore::String width_p;
	casacore::String velocityType_p;
	casacore::String restFrequency_p;
	casacore::String interpolationMethodPar_p;
	casacore::String outputReferenceFramePar_p;
	casac::variant *phaseCenterPar_p;
	casacore::uInt regriddingMethod_p;

	casacore::Bool refFrameTransformation_p;
	casacore::Bool radialVelocityCorrection_p;
	casacore::Bool radialVelocityCorrectionSignificant_p;

	casacore::MEpoch referenceTime_p;
	casacore::MDirection phaseCenter_p;
	casacore::MPosition observatoryPosition_p;
	casacore::MRadialVelocity radialVelocity_p;
	casacore::MFrequency::Types outputReferenceFrame_p;
	casacore::MFrequency::Types inputReferenceFrame_p;

	mutable casacore::MFrequency::Convert freqTransEngine_p;
	mutable casacore::uInt freqTransEngineRowId_p;

	mutable casacore::Double fftShift_p;
    casacore::FFTServer<casacore::Float, casacore::Complex> fFFTServer_p;

	casacore::MSColumns *selectedInputMsCols_p;
	casacore::MSFieldColumns *inputMSFieldCols_p;

	mutable map<casacore::Int,casacore::Float> weightFactorMap_p;
	mutable map<casacore::Int,casacore::Float> sigmaFactorMap_p;
	mutable inputOutputSpwMap inputOutputSpwMap_p;
};


//////////////////////////////////////////////////////////////////////////
// RegriddingTVIFactory class
//////////////////////////////////////////////////////////////////////////

class RegriddingTVIFactory : public ViFactory
{

public:

	RegriddingTVIFactory(casacore::Record &configuration,ViImplementation2 *inputVII);

protected:

	vi::ViImplementation2 * createVi (VisibilityIterator2 *) const;
	vi::ViImplementation2 * createVi () const;

	casacore::Record configuration_p;
	ViImplementation2 *inputVii_p;
};

//////////////////////////////////////////////////////////////////////////
// RegriddingTVILayerFactory class  (for _recursive_ layering)
//////////////////////////////////////////////////////////////////////////

class RegriddingTVILayerFactory : public ViiLayerFactory
{

public:

  RegriddingTVILayerFactory(casacore::Record &configuration);

  virtual ~RegriddingTVILayerFactory() {};

protected:


  virtual ViImplementation2 * createInstance(ViImplementation2* vii0) const;

  const casacore::Record configuration_p;

};

//////////////////////////////////////////////////////////////////////////
// RegriddingTransformEngine class
//////////////////////////////////////////////////////////////////////////

template<class T> class RegriddingKernel; // Forward declaration

template<class T> class RegriddingTransformEngine : public FreqAxisTransformEngine2<T>
{

	using FreqAxisTransformEngine2<T>::inputData_p;
	using FreqAxisTransformEngine2<T>::outputData_p;

public:

	RegriddingTransformEngine	(	RegriddingKernel<T> *kernel,
									DataCubeMap *inputData,
									DataCubeMap *outputData);

	void transform();

protected:

	// This member has to be a pointer, otherwise there
	// are compile time problems due to the fact that
	// it is a pure virtual class.
	RegriddingKernel<T> *regriddingKernel_p;
};

//////////////////////////////////////////////////////////////////////////
// RegriddingKernel class
//////////////////////////////////////////////////////////////////////////

template<class T> class RegriddingKernel
{

public:

	RegriddingKernel();
	virtual void kernel(DataCubeMap *inputData,DataCubeMap *outputData) = 0;
	virtual ~RegriddingKernel() {}

protected:

	casacore::Vector<casacore::Bool> & getInputFlagVector(DataCubeMap *inputData);
	casacore::Vector<casacore::Bool> & getOutputFlagVector(DataCubeMap *outputData);
	casacore::Vector<T> & getInputDataVector(DataCubeMap *inputData);
	casacore::Vector<T> & getOutputDataVector(DataCubeMap *outputData);

	casacore::Vector<casacore::Bool> inputDummyFlagVector_p;
	casacore::Vector<casacore::Bool> outputDummyFlagVector_p;
	casacore::Vector<T> inputDummyDataVector_p;
	casacore::Vector<T> outputDummyDataVector_p;

	casacore::Bool inputDummyFlagVectorInitialized_p;
	casacore::Bool outputDummyFlagVectorInitialized_p;
	casacore::Bool inputDummyDataVectorInitialized_p;
	casacore::Bool outputDummyDataVectorInitialized_p;

};

//////////////////////////////////////////////////////////////////////////
// DataInterpolationKernel class
//////////////////////////////////////////////////////////////////////////

template<class T> class DataInterpolationKernel : public RegriddingKernel<T>
{
	using RegriddingKernel<T>::getInputFlagVector;
	using RegriddingKernel<T>::getOutputFlagVector;
	using RegriddingKernel<T>::getInputDataVector;
	using RegriddingKernel<T>::getOutputDataVector;

public:

	DataInterpolationKernel(	casacore::uInt interpolationMethod,
								casacore::Vector<casacore::Double> *inputFreq,
								casacore::Vector<casacore::Double> *outputFreq);

	~DataInterpolationKernel() {}

	void kernel(DataCubeMap *inputData,DataCubeMap *outputData);

protected:

	casacore::uInt interpolationMethod_p;
	casacore::Vector<casacore::Double> *inputFreq_p;
	casacore::Vector<casacore::Double> *outputFreq_p;
};

//////////////////////////////////////////////////////////////////////////
// FlagFFTKernel class
//////////////////////////////////////////////////////////////////////////

template<class T> class DataFFTKernel : public RegriddingKernel<T>
{
	using RegriddingKernel<T>::getInputFlagVector;
	using RegriddingKernel<T>::getOutputFlagVector;
	using RegriddingKernel<T>::getInputDataVector;
	using RegriddingKernel<T>::getOutputDataVector;

public:

	DataFFTKernel(casacore::Double fftShift);

	~DataFFTKernel() {}

	void kernel(DataCubeMap *inputData,DataCubeMap *outputData);

protected:

	void fftshift(	casacore::Vector<casacore::Complex> &inputDataVector,
					casacore::Vector<casacore::Bool> &inputFlagVector,
					casacore::Vector<casacore::Complex> &outputDataVector,
					casacore::Vector<casacore::Bool> &outputFlagVector);

	void fftshift(	casacore::Vector<casacore::Float> &inputDataVector,
					casacore::Vector<casacore::Bool> &inputFlagVector,
					casacore::Vector<casacore::Float> &outputDataVector,
					casacore::Vector<casacore::Bool> &outputFlagVector);


	casacore::Double fftShift_p;
	casacore::FFTServer<casacore::Float, casacore::Complex> fFFTServer_p;
};

} //# NAMESPACE VI - END

} //# NAMESPACE CASA - END

#endif /* RegriddingTVI_H_ */

