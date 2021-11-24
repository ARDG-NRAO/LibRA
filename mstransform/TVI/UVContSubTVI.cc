//# UVContSubTVI.h: This file contains the implementation of the UVContSubTVI class.
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

#include <mstransform/TVI/UVContSubTVI.h>

using namespace casacore;

namespace casa { //# NAMESPACE CASA - BEGIN

namespace vi { //# NAMESPACE VI - BEGIN

//////////////////////////////////////////////////////////////////////////
// UVContSubTVI class
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
UVContSubTVI::UVContSubTVI(	ViImplementation2 * inputVii,
								const Record &configuration):
								FreqAxisTVI (inputVii)
{
	fitOrder_p = 0;
	want_cont_p = False;
	withDenoisingLib_p = True;
	nThreads_p = 1;
	niter_p = 1;

	inputFrequencyMap_p.clear();

	// Parse and check configuration parameters
	// Note: if a constructor finishes by throwing an exception, the memory
	// associated with the object itself is cleaned up — there is no memory leak.
	if (not parseConfiguration(configuration))
	{
		throw AipsError("Error parsing UVContSubTVI configuration");
	}

	initialize();

	return;
}

UVContSubTVI::~UVContSubTVI()
{
	for (auto iter = inputFrequencyMap_p.begin();iter != inputFrequencyMap_p.end(); iter++)
	{
		delete iter->second;
	}
	inputFrequencyMap_p.clear();
}


/*
 * Check if this is a valid field index
 *
 * @param str string from a (multi)field spec string
 *
 * @return whether this is a valid field index
 */
bool isFieldIndex(const std::string &str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

/*
 * For a field string (example: '3', '0,1,4' get the integer indices
 * specified in the string.
 * Tokenizes the string by ',' separator and trims spaces.
 *
 * @param a string with field indices as used in uvcontsub/fitspw parameter
 *
 * @return vector of field indices
 */
std::vector<int> stringToFieldIdxs(const std::string &str)
{
    stringstream stream(str);
    vector<int> result;

    while(stream.good()) {
        string token;
        getline(stream, token, ',' );
        token.erase(std::remove(token.begin(), token.end(), ' '), token.end());
        if (!isFieldIndex(token)) {
            throw AipsError("Invalid field index: " + token);

        }
        auto idx = std::stoi(token);
        result.push_back(idx);
    }
    return result;
}

void UVContSubTVI::parseFitSPW(const Record &configuration)
{
    int exists = -1;

    exists = -1;
    exists = configuration.fieldNumber ("fitspw");
    if (exists < 0) {
        return;
    }

    try {
        String fitspw;
        configuration.get(exists, fitspw);
        fitspw_p.insert({-1, fitspw});
    } catch(AipsError &exc) {
        const auto rawFieldFitspw = configuration.asArrayString("fitspw");
        if (0 == rawFieldFitspw.size()) {
            throw AipsError("The list of fitspw specifications is empty!");
        }

        auto nelem = rawFieldFitspw.size();
        if (0 != (nelem % 2)) {
            throw AipsError("fitspw must be a string or a list of pairs of strings: "
                            "field, field_fitspw. But the list or array passed has " +
                            std::to_string(nelem) + " items.");
        }
        const Matrix<String> fieldFitspw =
            rawFieldFitspw.reform(IPosition(2, rawFieldFitspw.size()/2, 2));
        logger_p << LogIO::NORMAL << LogOrigin("UVContSubTVI", __FUNCTION__)
                 << "  Number of per-field fitspw specifications received: "
                 << nelem/2 << LogIO::POST;

        // Get valid FIELD IDs. MSv2 uses the FIELD table row index as FIELD ID.
        const auto fieldsTable = getVii()->ms().field();
        const auto maxField = fieldsTable.nrow() - 1;

        const auto shape = fieldFitspw.shape();
        // iterate through fields
        for (int row=0; row<shape[0]; ++row) {
            //for (int col=0; col<shape[0]; ++col) {
            const auto &fieldsStr = fieldFitspw(row, 0);
            const auto &fitSpw = fieldFitspw(row, 1);

            auto fieldIdxs = stringToFieldIdxs(fieldsStr);
            for (const auto fid: fieldIdxs) {
                const auto inserted = fitspw_p.insert(std::make_pair(fid, fitSpw));
                // check for duplicates in the fields given in the list
                if (false == inserted.second) {
                    throw AipsError("Duplicated field in list of per-field fitspw : " +
                                    std::to_string(fid));
                }
                // check that the field is in the MS
                if (fid < 0 || static_cast<unsigned int>(fid) > maxField) {
                    throw AipsError("Wrong field ID given: " +
                                    std::to_string(fid) +
                                    ". This MeasurementSet has field IDs between"
                                    " 0 and " + std::to_string(maxField));
                }
            }
        }
    }
}

std::string fieldNameFromId(int idx)
{
    std::string name;
    if (idx >= 0) {
        name = std::to_string(idx);
    } else {
        name = "all fields";
    }
    return name;
}

void UVContSubTVI::printFitSPW() const
{
    if (fitspw_p.size() > 0) {
        logger_p << LogIO::NORMAL << LogOrigin("UVContSubTVI", __FUNCTION__)
                 << "Line-free channel selection is: " << LogIO::POST;

        for (auto const &elem: fitspw_p) {
            logger_p << LogIO::NORMAL << LogOrigin("UVContSubTVI", __FUNCTION__)
                     << "   field: " << fieldNameFromId(elem.first)
                     << ": " << elem.second << LogIO::POST;
        }
    } else {
        logger_p << LogIO::NORMAL << LogOrigin("UVContSubTVI", __FUNCTION__)
                 << "Line-free channel selection: not given" << LogIO::POST;
    }
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Bool UVContSubTVI::parseConfiguration(const Record &configuration)
{
	int exists = -1;
	Bool ret = True;

	exists = -1;
	exists = configuration.fieldNumber ("fitorder");
	if (exists >= 0)
	{
		configuration.get (exists, fitOrder_p);

		if (fitOrder_p > 0)
		{
			logger_p 	<< LogIO::NORMAL << LogOrigin("UVContSubTVI", __FUNCTION__)
						<< "Fit order is " << fitOrder_p << LogIO::POST;
		}
	}

	exists = -1;
	exists = configuration.fieldNumber ("want_cont");
	if (exists >= 0)
	{
		configuration.get (exists, want_cont_p);

		if (want_cont_p)
		{
			logger_p 	<< LogIO::NORMAL << LogOrigin("UVContSubTVI", __FUNCTION__)
						<< "Producing continuum estimate instead of continuum subtracted data "
						<< LogIO::POST;
		}
	}

        parseFitSPW(configuration);

	exists = -1;
	exists = configuration.fieldNumber ("denoising_lib");
	if (exists >= 0)
	{
		configuration.get (exists, withDenoisingLib_p);

		if (withDenoisingLib_p)
		{
			logger_p 	<< LogIO::NORMAL << LogOrigin("UVContSubTVI", __FUNCTION__)
						<< "Using GSL based multiparameter regression with linear least-squares fitting" << LogIO::POST;
		}
	}

	exists = -1;
	exists = configuration.fieldNumber ("nthreads");
	if (exists >= 0)
	{
		configuration.get (exists, nThreads_p);

		if (nThreads_p > 1)
		{
#ifdef _OPENMP
			if (omp_get_max_threads() < (int)nThreads_p)
			{
				logger_p << LogIO::WARN << LogOrigin("UVContSubTVI", __FUNCTION__)
						<< "Requested " <<  nThreads_p << " OMP threads but maximum possible is " << omp_get_max_threads()<< endl
						<< "Setting number of OMP threads to " << omp_get_max_threads() << endl
						<< "Check OMP_NUM_THREADS environmental variable and number of cores in your system"
						<< LogIO::POST;
				nThreads_p = omp_get_max_threads();
			}
			else
			{
				logger_p 	<< LogIO::NORMAL << LogOrigin("UVContSubTVI", __FUNCTION__)
							<< "Numer of OMP threads set to " << nThreads_p << LogIO::POST;
			}
#else
			logger_p << LogIO::WARN << LogOrigin("UVContSubTVI", __FUNCTION__)
					<< "Requested " <<  nThreads_p << " threads but OMP is not available in your system"
					<< LogIO::POST;
			nThreads_p = 1;
#endif
		}
	}

	exists = -1;
	exists = configuration.fieldNumber ("niter");
	if (exists >= 0)
	{
		configuration.get (exists, niter_p);

		if (niter_p > 1)
		{
			logger_p 	<< LogIO::NORMAL << LogOrigin("UVContSubTVI", __FUNCTION__)
						<< "Number of iterations for re-weighted linear fit: " << niter_p << LogIO::POST;
		}
	}


	return ret;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void UVContSubTVI::populatePerFieldLineFreeChannelMask(int fieldID,
                                                       const std::string& fieldFitspw)
{
    // Some selection string
    MSSelection mssel;
    mssel.setSpwExpr(fieldFitspw);
    const auto spwchan = mssel.getChanList(&(inputVii_p->ms()));

    // Create line-free channel map based on MSSelection channel ranges
    const auto nSelections = spwchan.shape()[0];
    unordered_map<Int,vector<Int> > lineFreeChannelMap;
    for (uInt selection_i=0; selection_i<nSelections; ++selection_i)
    {
        auto spw = spwchan(selection_i,0);
        if (lineFreeChannelMap.find(spw) == lineFreeChannelMap.end())
        {
            lineFreeChannelMap[spw].clear(); // Accessing the vector creates it
        }

        const auto channelStart = spwchan(selection_i,1);
        const auto channelStop = spwchan(selection_i,2);
        const auto channelStep = spwchan(selection_i,3);
        for (auto inpChan=channelStart; inpChan<=channelStop; inpChan += channelStep)
        {
            lineFreeChannelMap[spw].push_back(inpChan);
        }
    }

    // Create line-free channel mask, spw->channel_mask
    unordered_map<int, Vector<Bool> > lineFreeChannelMaskMap;
    for (auto const spwInp: spwInpChanIdxMap_p)
    {
        const auto spw = spwInp.first;
        if (lineFreeChannelMaskMap.find(spw) == lineFreeChannelMaskMap.end())
        {
            lineFreeChannelMaskMap[spw] = Vector<Bool>(spwInp.second.size(),True);
            for (uInt selChanIdx=0; selChanIdx<lineFreeChannelMap[spw].size();
                 ++selChanIdx)
            {
                const auto selChan = lineFreeChannelMap[spw][selChanIdx];
                lineFreeChannelMaskMap[spw](selChan) = False;
            }
        }
    }

    // Poppulate per field map: field -> spw -> channel_mask
    perFieldLineFreeChannelMaskMap_p.emplace(fieldID, lineFreeChannelMaskMap);
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void UVContSubTVI::initialize()
{
    // Populate nchan input-output maps
    for (auto spwInp: spwInpChanIdxMap_p)
    {
        auto spw = spwInp.first;
        spwOutChanNumMap_p[spw] = spwInp.second.size();
    }

    // Process line-free channel selection
    for (const auto item: fitspw_p) {
        unordered_map<int, Vector<Bool> > lineFreeChannelMaskMap;
        // Parse line-free channel selection using MSSelection syntax
        const auto fieldID = item.first;
        const auto fieldFitspw = item.second;

        logger_p << LogIO::NORMAL << LogOrigin("UVContSubTVI", __FUNCTION__)
                 << "Parsing fitspw string for field: " << fieldNameFromId(fieldID)
                 << ", fitspw: '" << fieldFitspw << "'" << LogIO::POST;

        if ("NONE" == fieldFitspw) {
            // Not inserting any entries in perFieldLineFreeChannelMaskMap_p[fieldID]
            // implies no transformation for that field (inputVis -> outputVis)
            continue;
        }

        if (fieldFitspw.empty()) {
            // -1 is the "all-fields-included" field pseudo-index
            // empty selection -> all SPW, channels, leave all SPW masks unset
            if (-1 == fieldID) {
                perFieldLineFreeChannelMaskMap_p.emplace(fieldID, lineFreeChannelMaskMap);
            }
            continue;
        }

        populatePerFieldLineFreeChannelMask(fieldID, fieldFitspw);
    }
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void UVContSubTVI::floatData (Cube<Float> & vis) const
{
	// Get input VisBuffer
	VisBuffer2 *vb = getVii()->getVisBuffer();

	// Transform data
	transformDataCube(vb->visCubeFloat(),vb->weightSpectrum(),vis);

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void UVContSubTVI::visibilityObserved (Cube<Complex> & vis) const
{
	// Get input VisBuffer
	VisBuffer2 *vb = getVii()->getVisBuffer();

	// Get weightSpectrum from sigmaSpectrum
	Cube<Float> weightSpFromSigmaSp;
	weightSpFromSigmaSp.resize(vb->sigmaSpectrum().shape(),False);
	weightSpFromSigmaSp = vb->sigmaSpectrum(); // = Operator makes a copy
	arrayTransformInPlace (weightSpFromSigmaSp,sigmaToWeight);

	// Transform data
	transformDataCube(vb->visCube(),weightSpFromSigmaSp,vis);

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void UVContSubTVI::visibilityCorrected (Cube<Complex> & vis) const
{
	// Get input VisBuffer
	VisBuffer2 *vb = getVii()->getVisBuffer();


	// Transform data
	transformDataCube(vb->visCubeCorrected(),vb->weightSpectrum(),vis);

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void UVContSubTVI::visibilityModel (Cube<Complex> & vis) const
{
	// Get input VisBuffer
	VisBuffer2 *vb = getVii()->getVisBuffer();

	// Transform data
	transformDataCube(vb->visCubeModel(),vb->weightSpectrum(),vis);

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> void UVContSubTVI::transformDataCube(	const Cube<T> &inputVis,
														const Cube<Float> &inputWeight,
														Cube<T> &outputVis) const
{
    // Get input VisBuffer
    VisBuffer2 *vb = getVii()->getVisBuffer();

    // Get polynomial model for this SPW (depends on number of channels and gridding)
    Int spwId = vb->spectralWindows()(0);
    if (inputFrequencyMap_p.find(spwId) == inputFrequencyMap_p.end())
    {
        const Vector<Double> &inputFrequencies = vb->getFrequencies(0);
        // STL should trigger move semantics
        inputFrequencyMap_p[spwId] = new denoising::GslPolynomialModel<Double>(inputFrequencies,fitOrder_p);
    }


    auto mapIt = perFieldLineFreeChannelMaskMap_p.find(-1);
    if (mapIt == perFieldLineFreeChannelMaskMap_p.end()) {
        auto fieldID = vb->fieldId()[0];
        mapIt = perFieldLineFreeChannelMaskMap_p.find(fieldID);
        if (mapIt == perFieldLineFreeChannelMaskMap_p.end()) {
            // This was a "NONE", no subtraction for this field
            outputVis = inputVis;
            return;
        }
    }

    // Get input line-free channel mask
    Vector<Bool> *lineFreeChannelMask = NULL;
    auto spwMap = mapIt->second;
    auto lookup = spwMap.find(spwId);
    if (lookup != spwMap.end())
    {
            lineFreeChannelMask = &(lookup->second);
    }

    // Reshape output data before passing it to the DataCubeHolder
    outputVis.resize(getVisBuffer()->getShape(),False);

    // Get input flag Cube
    const Cube<Bool> &flagCube = vb->flagCube();

	// Transform data
	if (nThreads_p > 1)
	{
#ifdef _OPENMP

		uInt nCorrs = vb->getShape()(0);
		if (nCorrs < nThreads_p)
		{
			omp_set_num_threads(nCorrs);
		}
		else
		{
			omp_set_num_threads(nThreads_p);
		}

		#pragma omp parallel for
		for (uInt corrIdx=0; corrIdx < nCorrs; corrIdx++)
		{
			transformDataCore(inputFrequencyMap_p[spwId],lineFreeChannelMask,
					inputVis,flagCube,inputWeight,outputVis,corrIdx);
		}

		omp_set_num_threads(nThreads_p);
#endif
	}
	else
	{
		transformDataCore(inputFrequencyMap_p[spwId],lineFreeChannelMask,
				inputVis,flagCube,inputWeight,outputVis);
	}

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> void UVContSubTVI::transformDataCore(	denoising::GslPolynomialModel<Double>* model,
														Vector<Bool> *lineFreeChannelMask,
														const Cube<T> &inputVis,
														const Cube<Bool> &inputFlags,
														const Cube<Float> &inputWeight,
														Cube<T> &outputVis,
														Int parallelCorrAxis) const
{
	// Gather input data
	DataCubeMap inputData;
	DataCubeHolder<T> inputVisCubeHolder(inputVis);
	DataCubeHolder<Bool> inputFlagCubeHolder(inputFlags);
	DataCubeHolder<Float> inputWeightsCubeHolder(inputWeight);
	inputData.add(MS::DATA,inputVisCubeHolder);
	inputData.add(MS::FLAG,inputFlagCubeHolder);
	inputData.add(MS::WEIGHT_SPECTRUM,inputWeightsCubeHolder);

	// Gather output data
	DataCubeMap outputData;
	DataCubeHolder<T> outputVisCubeHolder(outputVis);
	outputData.add(MS::DATA,outputVisCubeHolder);

	if (want_cont_p)
	{
		if (withDenoisingLib_p)
		{
			 UVContEstimationDenoisingKernel<T> kernel(model,niter_p,lineFreeChannelMask);
			 UVContSubTransformEngine<T> transformer(&kernel,&inputData,&outputData);
			 transformFreqAxis2(inputVis.shape(),transformer,parallelCorrAxis);
		}
		else
		{
			UVContEstimationKernel<T> kernel(model,lineFreeChannelMask);
			UVContSubTransformEngine<T> transformer(&kernel,&inputData,&outputData);
			transformFreqAxis2(inputVis.shape(),transformer,parallelCorrAxis);
		}
	}
	else
	{
		if (withDenoisingLib_p)
		{
			 UVContSubtractionDenoisingKernel<T> kernel(model,niter_p,lineFreeChannelMask);
			 UVContSubTransformEngine<T> transformer(&kernel,&inputData,&outputData);
			 transformFreqAxis2(inputVis.shape(),transformer,parallelCorrAxis);
		}
		else
		{
			UVContSubtractionKernel<T> kernel(model,lineFreeChannelMask);
			UVContSubTransformEngine<T> transformer(&kernel,&inputData,&outputData);
			transformFreqAxis2(inputVis.shape(),transformer,parallelCorrAxis);
		}
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
// UVContSubTVIFactory class
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
UVContSubTVIFactory::UVContSubTVIFactory (	Record &configuration,
											ViImplementation2 *inputVii)
{
	inputVii_p = inputVii;
	configuration_p = configuration;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
vi::ViImplementation2 * UVContSubTVIFactory::createVi(VisibilityIterator2 *) const
{
	return new UVContSubTVI(inputVii_p,configuration_p);
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
vi::ViImplementation2 * UVContSubTVIFactory::createVi() const
{
	return new UVContSubTVI(inputVii_p,configuration_p);
}

//////////////////////////////////////////////////////////////////////////
// UVContSubTVILayerFactory class
//////////////////////////////////////////////////////////////////////////

UVContSubTVILayerFactory::UVContSubTVILayerFactory(Record &configuration) :
	ViiLayerFactory()
{
	configuration_p = configuration;
}

ViImplementation2*
UVContSubTVILayerFactory::createInstance(ViImplementation2* vii0) const
{
	// Make the UVContSubTVI, using supplied ViImplementation2, and return it
	ViImplementation2 *vii = new UVContSubTVI(vii0, configuration_p);
	return vii;
}

//////////////////////////////////////////////////////////////////////////
// UVContSubTransformEngine class
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> UVContSubTransformEngine<T>::UVContSubTransformEngine(	UVContSubKernel<T> *kernel,
																			DataCubeMap *inputData,
																			DataCubeMap *outputData):
															FreqAxisTransformEngine2<T>(inputData,outputData)
{
	uvContSubKernel_p = kernel;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> void UVContSubTransformEngine<T>::transform(	)
{
	uvContSubKernel_p->setDebug(debug_p);
	uvContSubKernel_p->kernel(inputData_p,outputData_p);
}

//////////////////////////////////////////////////////////////////////////
// UVContSubKernel class
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> UVContSubKernel<T>::UVContSubKernel(	denoising::GslPolynomialModel<Double> *model,
														Vector<Bool> *lineFreeChannelMask)
{
	model_p = model;
	fitOrder_p = model_p->ncomponents()-1;
	freqPows_p.reference(model_p->getModelMatrix());
	frequencies_p.reference(model_p->getLinearComponentFloat());

	lineFreeChannelMask_p = lineFreeChannelMask != NULL? lineFreeChannelMask : NULL;
	debug_p = False;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> void UVContSubKernel<T>::kernel(	DataCubeMap *inputData,
													DataCubeMap *outputData)
{
	// Get input/output data
	Vector<T> &outputVector = outputData->getVector<T>(MS::DATA);
	Vector<T> &inputVector = inputData->getVector<T>(MS::DATA);

	// Apply line-free channel mask
	Vector<Bool> &inputFlags = inputData->getVector<Bool>(MS::FLAG);
	if (lineFreeChannelMask_p != NULL) inputFlags |= *lineFreeChannelMask_p;

	// Calculate number of valid data points and adapt fit
	size_t validPoints = nfalse(inputFlags);
	if (validPoints > 0)
	{
		Bool restoreDefaultPoly = False;
		uInt tmpFitOrder = fitOrder_p;

		// Reduce fit order to match number of valid points
		if (validPoints <= fitOrder_p)
		{
			changeFitOrder(validPoints-1);
			restoreDefaultPoly = True;
		}

		// Get weights
		Vector<Float> &inputWeight = inputData->getVector<Float>(MS::WEIGHT_SPECTRUM);

		// Convert flags to mask
		Vector<Bool> mask = !inputFlags;

		// Calculate and subtract continuum
		kernelCore(inputVector,mask,inputWeight,outputVector);

		// Go back to default fit order to match number of valid points
		if (restoreDefaultPoly)
		{
			changeFitOrder(tmpFitOrder);
		}
	}
	else
	{
		defaultKernel(inputVector,outputVector);
	}

	return;
}


//////////////////////////////////////////////////////////////////////////
// UVContSubtractionKernel class
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> UVContSubtractionKernel<T>::UVContSubtractionKernel(	denoising::GslPolynomialModel<Double>* model,
																		Vector<Bool> *lineFreeChannelMask):
																		UVContSubKernel<T>(model,lineFreeChannelMask)
{
	changeFitOrder(fitOrder_p);
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> void UVContSubtractionKernel<T>::changeFitOrder(size_t order)
{
	fitOrder_p = order;
	Polynomial<AutoDiff<Float> > poly(order);
	fitter_p.setFunction(poly); // poly It is cloned
	fitter_p.asWeight(True);

	return;
}


// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> void UVContSubtractionKernel<T>::defaultKernel(	Vector<Complex> &inputVector,
																	Vector<Complex> &outputVector)
{
	outputVector = inputVector;
	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> void UVContSubtractionKernel<T>::defaultKernel(	Vector<Float> &inputVector,
																	Vector<Float> &outputVector)
{
	outputVector = inputVector;
	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> void UVContSubtractionKernel<T>::kernelCore(	Vector<Complex> &inputVector,
																Vector<Bool> &inputFlags,
																Vector<Float> &inputWeights,
																Vector<Complex> &outputVector)
{
	// Fit for imaginary and real components separately
	Vector<Float> realCoeff;
	Vector<Float> imagCoeff;
	realCoeff = fitter_p.fit(frequencies_p, real(inputVector), inputWeights, &inputFlags);
	imagCoeff = fitter_p.fit(frequencies_p, imag(inputVector), inputWeights, &inputFlags);

	// Fill output data
	outputVector = inputVector;
	outputVector -= Complex(realCoeff(0),imagCoeff(0));
	for (uInt order_idx = 1; order_idx <= fitOrder_p; order_idx++)
	{
		Complex coeff(realCoeff(order_idx),imagCoeff(order_idx));
		for (uInt chan_idx=0; chan_idx < outputVector.size(); chan_idx++)
		{
			outputVector(chan_idx) -= (freqPows_p(order_idx,chan_idx))*coeff;
		}
	}

	if (debug_p)
	{
		LogIO logger;
		logger << "fit order = " << fitOrder_p << LogIO::POST;
		logger << "realCoeff =" << realCoeff << LogIO::POST;
		logger << "imagCoeff =" << imagCoeff << LogIO::POST;
		logger << "inputFlags =" << inputFlags << LogIO::POST;
		logger << "inputWeights =" << inputWeights << LogIO::POST;
		logger << "inputVector =" << inputVector << LogIO::POST;
		logger << "outputVector =" << outputVector << LogIO::POST;
	}

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> void UVContSubtractionKernel<T>::kernelCore(	Vector<Float> &inputVector,
																Vector<Bool> &inputFlags,
																Vector<Float> &inputWeights,
																Vector<Float> &outputVector)
{
	// Fit model
	Vector<Float> coeff;
	coeff = fitter_p.fit(frequencies_p, inputVector, inputWeights, &inputFlags);

	// Fill output data
	outputVector = inputVector;
	outputVector -= coeff(0);
	for (uInt order_idx = 1; order_idx <= fitOrder_p; order_idx++)
	{
		for (uInt chan_idx=0; chan_idx < outputVector.size(); chan_idx++)
		{
			outputVector(chan_idx) -= (freqPows_p(order_idx,chan_idx))*coeff(order_idx);
		}
	}

	if (true or debug_p)
	{
		LogIO logger;
		logger << "fit order = " << fitOrder_p << LogIO::POST;
		logger << "coeff =" << coeff << LogIO::POST;
		logger << "inputFlags =" << inputFlags << LogIO::POST;
		logger << "inputWeights =" << inputWeights << LogIO::POST;
		logger << "inputVector =" << inputVector << LogIO::POST;
		logger << "outputVector =" << outputVector << LogIO::POST;
	}

	return;
}


//////////////////////////////////////////////////////////////////////////
// UVContEstimationKernel class
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> UVContEstimationKernel<T>::UVContEstimationKernel(	denoising::GslPolynomialModel<Double>* model,
																		Vector<Bool> *lineFreeChannelMask):
																		UVContSubKernel<T>(model,lineFreeChannelMask)
{
	changeFitOrder(fitOrder_p);
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> void UVContEstimationKernel<T>::changeFitOrder(size_t order)
{
	fitOrder_p = order;
	Polynomial<AutoDiff<Float> > poly(order);
	fitter_p.setFunction(poly); // poly It is cloned
	fitter_p.asWeight(True);

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> void UVContEstimationKernel<T>::defaultKernel(Vector<Complex> &,
																Vector<Complex> &outputVector)
{
	outputVector = 0;
	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> void UVContEstimationKernel<T>::defaultKernel(Vector<Float> &,
																Vector<Float> &outputVector)
{
	outputVector = 0;
	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> void UVContEstimationKernel<T>::kernelCore(	Vector<Complex> &inputVector,
																Vector<Bool> &inputFlags,
																Vector<Float> &inputWeights,
																Vector<Complex> &outputVector)
{
	// Fit for imaginary and real components separately
	Vector<Float> realCoeff;
	Vector<Float> imagCoeff;
	realCoeff = fitter_p.fit(frequencies_p, real(inputVector), inputWeights, &inputFlags);
	imagCoeff = fitter_p.fit(frequencies_p, imag(inputVector), inputWeights, &inputFlags);

	// Fill output data
	outputVector = Complex(realCoeff(0),imagCoeff(0));
	for (uInt order_idx = 1; order_idx <= fitOrder_p; order_idx++)
	{
		Complex coeff(realCoeff(order_idx),imagCoeff(order_idx));
		for (uInt chan_idx=0; chan_idx < outputVector.size(); chan_idx++)
		{
			outputVector(chan_idx) += (freqPows_p(order_idx,chan_idx))*coeff;
		}
	}

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> void UVContEstimationKernel<T>::kernelCore(	Vector<Float> &inputVector,
																Vector<Bool> &inputFlags,
																Vector<Float> &inputWeights,
																Vector<Float> &outputVector)
{
	// Fit model
	Vector<Float> coeff;
	coeff = fitter_p.fit(frequencies_p, inputVector, inputWeights, &inputFlags);

	// Fill output data
	outputVector = coeff(0);
	for (uInt order_idx = 1; order_idx <= fitOrder_p; order_idx++)
	{
		for (uInt chan_idx=0; chan_idx < outputVector.size(); chan_idx++)
		{
			outputVector(chan_idx) += (freqPows_p(order_idx,chan_idx))*coeff(order_idx);
		}
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
// UVContSubtractionDenoisingKernel class
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> UVContSubtractionDenoisingKernel<T>::UVContSubtractionDenoisingKernel(denoising::GslPolynomialModel<Double>* model,
																						size_t nIter,
																						Vector<Bool> *lineFreeChannelMask):
																						UVContSubKernel<T>(model,lineFreeChannelMask)
{
	fitter_p.resetModel(*model);
	fitter_p.setNIter(nIter);
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> void UVContSubtractionDenoisingKernel<T>::changeFitOrder(size_t order)
{
	fitOrder_p = order;
	fitter_p.resetNComponents(order+1);
	return;
}


// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> void UVContSubtractionDenoisingKernel<T>::defaultKernel(	Vector<T> &inputVector,
																			Vector<T> &outputVector)
{
	outputVector = inputVector;
	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> void UVContSubtractionDenoisingKernel<T>::kernelCore(	Vector<T> &inputVector,
																		Vector<Bool> &inputFlags,
																		Vector<Float> &inputWeights,
																		Vector<T> &outputVector)
{	fitter_p.setWeightsAndFlags(inputWeights,inputFlags);
	fitter_p.calcFitCoeff(inputVector);

	Vector<T> model(outputVector.size());
	fitter_p.calcFitModel(model);

	outputVector = inputVector;
	outputVector -= model;

	/*
	fitter_p.setWeightsAndFlags(inputWeights,inputFlags);
	Vector<T> coeff = fitter_p.calcFitCoeff(inputVector);

	// Fill output data
	outputVector = inputVector;
	outputVector -= coeff(0);
	for (uInt order_idx = 1; order_idx <= fitOrder_p; order_idx++)
	{
		for (uInt chan_idx=0; chan_idx < outputVector.size(); chan_idx++)
		{
			outputVector(chan_idx) -= (freqPows_p(order_idx,chan_idx))*coeff(order_idx);
		}
	}
	*/

	/*
	if (debug_p)
	{
		LogIO logger;
		logger << "freqPows_p =" << freqPows_p << LogIO::POST;
		logger << "fit order = " << fitOrder_p << LogIO::POST;
		logger << "coeff =" << coeff << LogIO::POST;
		logger << "inputFlags =" << inputFlags << LogIO::POST;
		logger << "inputWeights =" << inputWeights << LogIO::POST;
		logger << "inputVector =" << inputVector << LogIO::POST;
		logger << "outputVector =" << outputVector << LogIO::POST;
	}
	*/

	return;
}

//////////////////////////////////////////////////////////////////////////
// UVContEstimationDenoisingKernel class
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> UVContEstimationDenoisingKernel<T>::UVContEstimationDenoisingKernel(	denoising::GslPolynomialModel<Double>* model,
																						size_t nIter,
																						Vector<Bool> *lineFreeChannelMask):
																						UVContSubKernel<T>(model,lineFreeChannelMask)
{
	fitter_p.resetModel(*model);
	fitter_p.setNIter(nIter);
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> void UVContEstimationDenoisingKernel<T>::changeFitOrder(size_t order)
{
	fitOrder_p = order;
	fitter_p.resetNComponents(order+1);
	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> void UVContEstimationDenoisingKernel<T>::defaultKernel(	Vector<T> &,
																			Vector<T> &outputVector)
{
	outputVector = 0;
	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> void UVContEstimationDenoisingKernel<T>::kernelCore(	Vector<T> &inputVector,
																		Vector<Bool> &inputFlags,
																		Vector<Float> &inputWeights,
																		Vector<T> &outputVector)
{
	fitter_p.setWeightsAndFlags(inputWeights,inputFlags);
	fitter_p.calcFitCoeff(inputVector);
	fitter_p.calcFitModel(outputVector);

	/*
	fitter_p.setWeightsAndFlags(inputWeights,inputFlags);
	Vector<T> coeff = fitter_p.calcFitCoeff(inputVector);

	// Fill output data
	outputVector = coeff(0);
	for (uInt order_idx = 1; order_idx <= fitOrder_p; order_idx++)
	{
		for (uInt chan_idx=0; chan_idx < outputVector.size(); chan_idx++)
		{
			outputVector(chan_idx) += (freqPows_p(order_idx,chan_idx))*coeff(order_idx);
		}
	}
	*/

	return;
}

} //# NAMESPACE VI - END

} //# NAMESPACE CASA - END


