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

#include <unordered_map>

// Base class
#include <mstransform/TVI/FreqAxisTVI.h>

// Fitting classes
#include <scimath/Fitting/LinearFitSVD.h>
#include <scimath/Functionals/Polynomial.h>
#include <mstransform/TVI/DenoisingLib.h>
#include <mstransform/TVI/UVContSubResult.h>

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
    virtual void result(casacore::Record &res) const;

protected:

    bool parseConfiguration(const Record &configuration);
    void initialize();

    template<class T> void transformDataCube(	const Cube<T> &inputVis,
    											const Cube<Float> &inputWeight,
    											Cube<T> &outputVis) const;

    template<class T> Complex transformDataCore(denoising::GslPolynomialModel<Double>* model,
                                                Vector<Bool> *lineFreeChannelMask,
                                                const Cube<T> &inputVis,
                                                const Cube<Bool> &inputFlags,
                                                const Cube<Float> &inputWeight,
                                                Cube<T> &outputVis,
                                                Int parallelCorrAxis=-1) const;

 private:
    void parseFitSPW(const Record &configuration);
    void parseListFitSPW(const Record &configuration);
    void printFitSPW() const;
    void populatePerFieldLineFreeChannelMask(int fieldID, const std::string& fieldFitspw);

    mutable uInt fitOrder_p;
    mutable Bool want_cont_p;
    // field -> fitspw spec
    std::unordered_map<int, std::string> fitspw_p;
    mutable Bool withDenoisingLib_p;
    mutable uInt nThreads_p;
    mutable uInt niter_p;

    // Maps field -> SPW -> channel_mask (1s will be combined with flags to exclude chans)
    unordered_map<int, unordered_map<Int, Vector<Bool> > > perFieldLineFreeChannelMaskMap_p;
    mutable map<Int, denoising::GslPolynomialModel<Double>* > inputFrequencyMap_p;

    mutable UVContSubResult result_p;

    // cache model when writemodel / MODEL_DATA has to be produced
    mutable Cube<casacore::Complex> precalcModelVis_p;
    bool precalcModel_p = false;
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

	virtual void defaultKernel(Vector<T> &inputVector,
                                   Vector<T> &outputVector) = 0;

	virtual Complex kernelCore(Vector<T> &inputVector,
                                   Vector<Bool> &inputFlags,
                                   Vector<Float> &inputWeights,
                                   Vector<T> &outputVector) = 0;

        Complex getChiSquared() { return chisq_p; }
	void setDebug(Bool debug) { debug_p = debug; }

protected:

	Bool debug_p;
	size_t fitOrder_p;
	denoising::GslPolynomialModel<Double> *model_p;
	Matrix<Double> freqPows_p;
	Vector<Float> frequencies_p;
	Vector<Bool> *lineFreeChannelMask_p;
        Complex chisq_p = Complex(std::numeric_limits<float>::infinity(),
                                  std::numeric_limits<float>::infinity());
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
								Vector<Bool> *lineFreeChannelMask=nullptr);

	void changeFitOrder(size_t order);

	void defaultKernel(Vector<Complex> &inputVector,
                           Vector<Complex> &outputVector);

	void defaultKernel(Vector<Float> &inputVector,
                           Vector<Float> &outputVector);

	Complex kernelCore(Vector<Complex> &inputVector,
                           Vector<Bool> &inputFlags,
                           Vector<Float> &inputWeights,
                           Vector<Complex> &outputVector);

	Complex kernelCore(Vector<Float> &inputVector,
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
							Vector<Bool> *lineFreeChannelMask=nullptr);

	void changeFitOrder(size_t order);

	void defaultKernel(Vector<Complex> &inputVector,
                           Vector<Complex> &outputVector);

	void defaultKernel(Vector<Float> &inputVector,
                           Vector<Float> &outputVector);

	Complex kernelCore(Vector<Complex> &inputVector,
                           Vector<Bool> &inputFlags,
                           Vector<Float> &inputWeights,
                           Vector<Complex> &outputVector);

	Complex kernelCore(Vector<Float> &inputVector,
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
										Vector<Bool> *lineFreeChannelMask=nullptr);

	void changeFitOrder(size_t order);

	void defaultKernel(Vector<T> &inputVector,
                           Vector<T> &outputVector);

	Complex kernelCore(Vector<T> &inputVector,
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
										Vector<Bool> *lineFreeChannelMask=nullptr);

	void changeFitOrder(size_t order);

	void defaultKernel(Vector<T> &inputVector,
                           Vector<T> &outputVector);

	Complex kernelCore(Vector<T> &inputVector,
                           Vector<Bool> &inputFlags,
                           Vector<Float> &inputWeights,
                           Vector<T> &outputVector);

private:

	denoising::IterativelyReweightedLeastSquares fitter_p;
};



} //# NAMESPACE VI - END

} //# NAMESPACE CASA - END

#endif /* UVContSubTVI_H_ */

