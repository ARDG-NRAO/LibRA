//# UVContSubTVI.h: This file contains the interface definition of the MSTransformManager class.
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

#ifndef UVContSubTVI_H_
#define UVContSubTVI_H_

// Base class
#include <mstransform/TVI/FreqAxisTVI.h>

// Fitting classes
#include <scimath/Fitting/LinearFitSVD.h>
#include <scimath/Functionals/Polynomial.h>
#include <mstransform/TVI/DenoisingLib.h>

// OpenMP
#ifdef _OPENMP
#include <omp.h>
#endif

using namespace casacore;


namespace casa { //# NAMESPACE CASA - BEGIN

namespace vi { //# NAMESPACE VI - BEGIN

//////////////////////////////////////////////////////////////////////////
// UVContSubTVI class
//////////////////////////////////////////////////////////////////////////

class UVContSubTVI : public FreqAxisTVI
{

public:

	UVContSubTVI(	ViImplementation2 * inputVii,
						const Record &configuration);

	~UVContSubTVI();

	// Report the the ViImplementation type
	virtual String ViiType() const { return String("UVContSub( ")+getVii()->ViiType()+" )"; };

    virtual void floatData (Cube<Float> & vis) const;
    virtual void visibilityObserved (Cube<Complex> & vis) const;
    virtual void visibilityCorrected (Cube<Complex> & vis) const;
    virtual void visibilityModel (Cube<Complex> & vis) const;

protected:

    bool parseConfiguration(const Record &configuration);
    void initialize();

    template<class T> void transformDataCube(	const Cube<T> &inputVis,
    											const Cube<Float> &inputWeight,
    											Cube<T> &outputVis) const;

    template<class T> void transformDataCore(	denoising::GslPolynomialModel<Double>* model,
												Vector<Bool> *lineFreeChannelMask,
												const Cube<T> &inputVis,
												const Cube<Bool> &inputFlags,
												const Cube<Float> &inputWeight,
												Cube<T> &outputVis,
												Int parallelCorrAxis=-1) const;
    mutable uInt fitOrder_p;
    mutable Bool want_cont_p;
    mutable String fitspw_p;
    mutable Bool withDenoisingLib_p;
    mutable uInt nThreads_p;
    mutable uInt niter_p;
    mutable map<Int,Vector<Bool> > lineFreeChannelMaskMap_p;

	mutable map<Int, denoising::GslPolynomialModel<Double>* > inputFrequencyMap_p;
};

//////////////////////////////////////////////////////////////////////////
// UVContSubTVIFactory class
//////////////////////////////////////////////////////////////////////////

class UVContSubTVIFactory : public ViFactory
{

public:

	UVContSubTVIFactory(Record &configuration,ViImplementation2 *inputVII);

protected:

	vi::ViImplementation2 * createVi (VisibilityIterator2 *) const;
	vi::ViImplementation2 * createVi () const;

	Record configuration_p;
	ViImplementation2 *inputVii_p;
};

//////////////////////////////////////////////////////////////////////////
// UVContSubTVILayerFactory class  (for _recursive_ layering)
//////////////////////////////////////////////////////////////////////////

class UVContSubTVILayerFactory : public ViiLayerFactory
{

public:

	UVContSubTVILayerFactory(Record &configuration);
	virtual ~UVContSubTVILayerFactory() {};

protected:

	ViImplementation2 * createInstance(ViImplementation2* vii0) const;

	Record configuration_p;

};

//////////////////////////////////////////////////////////////////////////
// UVContSubTransformEngine class
//////////////////////////////////////////////////////////////////////////

template<class T> class UVContSubKernel; // Forward declaration

template<class T> class UVContSubTransformEngine : public FreqAxisTransformEngine2<T>
{
	using FreqAxisTransformEngine2<T>::inputData_p;
	using FreqAxisTransformEngine2<T>::outputData_p;
	using FreqAxisTransformEngine2<T>::debug_p;

public:

	UVContSubTransformEngine(	UVContSubKernel<T> *kernel,
								DataCubeMap *inputData,
								DataCubeMap *outputData	);

	void transform();

	void transformCore(DataCubeMap *inputData,DataCubeMap *outputData);

protected:

	// This member has to be a pointer, otherwise there
	// are compile time problems due to the fact that
	// it is a pure virtual class.
	UVContSubKernel<T> *uvContSubKernel_p;
};

//////////////////////////////////////////////////////////////////////////
// UVContSubKernel class
//////////////////////////////////////////////////////////////////////////

template<class T> class UVContSubKernel
{

public:

	UVContSubKernel(	denoising::GslPolynomialModel<Double>* model,
						Vector<Bool> *lineFreeChannelMask);

	virtual void kernel(DataCubeMap *inputData,
						DataCubeMap *outputData);

	virtual void changeFitOrder(size_t order) = 0;

	virtual void defaultKernel(	Vector<T> &inputVector,
								Vector<T> &outputVector) = 0;

	virtual void kernelCore(Vector<T> &inputVector,
							Vector<Bool> &inputFlags,
							Vector<Float> &inputWeights,
							Vector<T> &outputVector) = 0;

	void setDebug(Bool debug) { debug_p = debug;}

protected:

	Bool debug_p;
	size_t fitOrder_p;
	denoising::GslPolynomialModel<Double> *model_p;
	Matrix<Double> freqPows_p;
	Vector<Float> frequencies_p;
	Vector<Bool> *lineFreeChannelMask_p;
};

//////////////////////////////////////////////////////////////////////////
// UVContSubtractionKernel class
//////////////////////////////////////////////////////////////////////////

template<class T> class UVContSubtractionKernel : public UVContSubKernel<T>
{
	using UVContSubKernel<T>::fitOrder_p;
	using UVContSubKernel<T>::model_p;
	using UVContSubKernel<T>::freqPows_p;
	using UVContSubKernel<T>::frequencies_p;
	using UVContSubKernel<T>::lineFreeChannelMask_p;
	using UVContSubKernel<T>::debug_p;


public:

	UVContSubtractionKernel(	denoising::GslPolynomialModel<Double>* model,
								Vector<Bool> *lineFreeChannelMask=NULL);

	void changeFitOrder(size_t order);

	void defaultKernel(	Vector<Complex> &inputVector,
						Vector<Complex> &outputVector);

	void defaultKernel(	Vector<Float> &inputVector,
						Vector<Float> &outputVector);

	void kernelCore(	Vector<Complex> &inputVector,
						Vector<Bool> &inputFlags,
						Vector<Float> &inputWeights,
						Vector<Complex> &outputVector);

	void kernelCore(	Vector<Float> &inputVector,
						Vector<Bool> &inputFlags,
						Vector<Float> &inputWeights,
						Vector<Float> &outputVector);

private:

	LinearFitSVD<Float> fitter_p;
};

//////////////////////////////////////////////////////////////////////////
// UVContEstimationKernel class
//////////////////////////////////////////////////////////////////////////

template<class T> class UVContEstimationKernel : public UVContSubKernel<T>
{

	using UVContSubKernel<T>::fitOrder_p;
	using UVContSubKernel<T>::model_p;
	using UVContSubKernel<T>::freqPows_p;
	using UVContSubKernel<T>::frequencies_p;
	using UVContSubKernel<T>::lineFreeChannelMask_p;
	using UVContSubKernel<T>::debug_p;

public:

	UVContEstimationKernel(	denoising::GslPolynomialModel<Double>* model,
							Vector<Bool> *lineFreeChannelMask=NULL);

	void changeFitOrder(size_t order);

	void defaultKernel(	Vector<Complex> &inputVector,
						Vector<Complex> &outputVector);

	void defaultKernel(	Vector<Float> &inputVector,
						Vector<Float> &outputVector);

	void kernelCore(	Vector<Complex> &inputVector,
						Vector<Bool> &inputFlags,
						Vector<Float> &inputWeights,
						Vector<Complex> &outputVector);

	void kernelCore(	Vector<Float> &inputVector,
						Vector<Bool> &inputFlags,
						Vector<Float> &inputWeights,
						Vector<Float> &outputVector);

private:

	LinearFitSVD<Float> fitter_p;
};

//////////////////////////////////////////////////////////////////////////
// UVContSubtractionDenoisingKernel class
//////////////////////////////////////////////////////////////////////////

template<class T> class UVContSubtractionDenoisingKernel : public UVContSubKernel<T>
{
	using UVContSubKernel<T>::fitOrder_p;
	using UVContSubKernel<T>::model_p;
	using UVContSubKernel<T>::freqPows_p;
	using UVContSubKernel<T>::frequencies_p;
	using UVContSubKernel<T>::lineFreeChannelMask_p;
	using UVContSubKernel<T>::debug_p;


public:

	UVContSubtractionDenoisingKernel(	denoising::GslPolynomialModel<Double>* model,
										size_t nIter,
										Vector<Bool> *lineFreeChannelMask=NULL);

	void changeFitOrder(size_t order);

	void defaultKernel(	Vector<T> &inputVector,
						Vector<T> &outputVector);

	void kernelCore(	Vector<T> &inputVector,
						Vector<Bool> &inputFlags,
						Vector<Float> &inputWeights,
						Vector<T> &outputVector);

private:

	denoising::IterativelyReweightedLeastSquares fitter_p;
};

//////////////////////////////////////////////////////////////////////////
// UVContEstimationDenoisingKernel class
//////////////////////////////////////////////////////////////////////////

template<class T> class UVContEstimationDenoisingKernel : public UVContSubKernel<T>
{

	using UVContSubKernel<T>::fitOrder_p;
	using UVContSubKernel<T>::model_p;
	using UVContSubKernel<T>::freqPows_p;
	using UVContSubKernel<T>::frequencies_p;
	using UVContSubKernel<T>::lineFreeChannelMask_p;
	using UVContSubKernel<T>::debug_p;

public:

	UVContEstimationDenoisingKernel(	denoising::GslPolynomialModel<Double>* model,
										size_t nIter,
										Vector<Bool> *lineFreeChannelMask=NULL);

	void changeFitOrder(size_t order);

	void defaultKernel(	Vector<T> &inputVector,
						Vector<T> &outputVector);

	void kernelCore(	Vector<T> &inputVector,
						Vector<Bool> &inputFlags,
						Vector<Float> &inputWeights,
						Vector<T> &outputVector);

private:

	denoising::IterativelyReweightedLeastSquares fitter_p;
};



} //# NAMESPACE VI - END

} //# NAMESPACE CASA - END

#endif /* UVContSubTVI_H_ */

