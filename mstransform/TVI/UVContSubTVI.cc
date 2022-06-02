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
#include <casacore/ms/MeasurementSets/MSFieldColumns.h>

// OpenMP
#ifdef _OPENMP
#include <omp.h>
#endif

using namespace casacore;

namespace casa { //# NAMESPACE CASA - BEGIN

namespace vi { //# NAMESPACE VI - BEGIN

FitSpec:: FitSpec(Vector<bool> mask, unsigned int order){
    lineFreeChannelMask = mask;
    fitOrder = order;
};

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
	withDenoisingLib_p = true;
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
 * @param a string with field indices as used in uvcontsub/fitspec parameter
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

void UVContSubTVI::parseFitSpec(const Record &configuration)
{
    const auto exists = configuration.fieldNumber ("fitspec");
    if (exists < 0) {
        return;
    }

    try {
        // fitspec is a simple string (spw:chan MSSelection syntax)
        String fitspec;
        configuration.get(exists, fitspec);

        std::vector <InFitSpec> specs = { InFitSpec(fitspec, fitOrder_p) };
        // -1 (all fields): global fit spw:chan string and global fitOrder
        fitspec_p.insert({-1, specs});
    } catch(AipsError &exc) {
        // alternatively, fitspec must be a record with field/spw specs
        parseListFitSpec(configuration);
    }
}

// TODO: this is just transition list->dict/record. Very limited, supports only single-SPW
// strings
unsigned int spwIDFromSpwStr(const std::string &spwStr) {
    if (0 == spwStr.length()) {
        throw AipsError("Unexpected empty SPW IDs string");
    } else if (spwStr.find(',') != std::string::npos) {
        // this would need to split by ',' and get the list of spws -> return list of int
        throw AipsError("Unexpected SPW separator , found in per-field per-SPW spw ID");
    } else {
        auto pos = spwStr.find(':');
        return stoi(spwStr.substr(0, pos));
    }
}

/**
 * Takes the input list of fieldID: fitSpec, parses it and populates a
 * map in fitspec_p.
 *
 * @param configuration dictionary/record passed from the task/tool interface, with
 *        uvcontsub task parameters
 *
 */
void UVContSubTVI::parseListFitSpec(const Record &configuration)
{
    // TODO: finish rename fitspw->fitspec. Should be split
    const auto rawFieldFitspec = configuration.asArrayString("fitspec");
    if (0 == rawFieldFitspec.size()) {
        throw AipsError("The list of fit specifications is empty!");
    }

    const size_t mult = 3;
    auto nelem = rawFieldFitspec.size();
    if (0 != (nelem % mult)) {
        throw AipsError("fitspec must be a string or a list of triplets: "
                        "field, field_fitspw_chan, polynomial_order. But the list or array "
                        "passed has " + std::to_string(nelem) + " items.");
    }
    const Matrix<String> fieldFitspec =
        rawFieldFitspec.reform(IPosition(2, rawFieldFitspec.size()/mult, mult));
    logger_p << LogIO::NORMAL << LogOrigin("UVContSubTVI", __FUNCTION__)
             << "Number of per-field fit specifications read: "
             << nelem/mult << LogIO::POST;

    // Get valid FIELD IDs. MSv2 uses the FIELD table row index as FIELD ID.
    const auto &fieldsTable = getVii()->fieldSubtablecols();
    const auto maxField = fieldsTable.nrow() - 1;

    const auto shape = fieldFitspec.shape();
    // iterate through fields
    for (int row=0; row<shape[0]; ++row) {
        //for (int col=0; col<shape[0]; ++col) {
        const auto &fieldsStr = fieldFitspec(row, 0);
        const auto &spwStr = fieldFitspec(row, 1);
        const auto &orderStr = fieldFitspec(row, 2);
        int polOrder = fitOrder_p;
        try {
            // TODO: better check (no exception when converting and must be >0)
            polOrder = std::stoi(orderStr);
        } catch(std::exception &exc) {
            throw AipsError("Invalid fit order value (" + std::string(exc.what()) + "): "
                            + std::string(orderStr));
        }
        if (polOrder < 0) {
            throw AipsError("Fit order cannot be negative. Value given: " + orderStr);
        }

        auto fieldIdxs = stringToFieldIdxs(fieldsStr);
        for (const auto fid: fieldIdxs) {
            // TODO: transition list->dict/record - doesn't check for all possible errors
            // in inputs (deferred)
            const auto fieldFound = fitspec_p.find(fid);
            auto spec = std::make_pair(spwStr, polOrder);
            if (fieldFound == fitspec_p.end()) {
                std::vector<InFitSpec> firstSpw = { spec };
                fitspec_p[fid] = firstSpw;
            } else {
                fitspec_p[fid].emplace_back(spec);
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

void UVContSubTVI::printInputFitSpec() const
{
    if (fitspec_p.size() > 0) {
        logger_p << LogIO::NORMAL << LogOrigin("UVContSubTVI", __FUNCTION__)
                 << "Fit order and line-free channel specification is: " << LogIO::POST;

        for (const auto &elem: fitspec_p) {
            const auto &specs = elem.second;
            for (const auto &oneSpw : specs) {
                logger_p << LogIO::NORMAL << LogOrigin("UVContSubTVI", __FUNCTION__)
                         << "   field: " << fieldNameFromId(elem.first)
                         << ": " << oneSpw.second << ", " << oneSpw.first << LogIO::POST;
            }
        }
    } else {
        logger_p << LogIO::NORMAL << LogOrigin("UVContSubTVI", __FUNCTION__)
                 << "Line-free channel selection: not given" << LogIO::POST;
    }
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
bool UVContSubTVI::parseConfiguration(const Record &configuration)
{
	bool ret = true;

	auto exists = configuration.fieldNumber ("fitorder");
	if (exists >= 0)
	{
		configuration.get (exists, fitOrder_p);

		if (fitOrder_p > 0)
		{
			logger_p 	<< LogIO::NORMAL << LogOrigin("UVContSubTVI", __FUNCTION__)
						<< "Global/default fit order is " << fitOrder_p << LogIO::POST;
		}
	}

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

        parseFitSpec(configuration);
        // TODO: revisit the prints after fitspw/fitspec record changes
        printInputFitSpec();

	exists = configuration.fieldNumber ("writemodel");
	if (exists >= 0)
	{
            configuration.get(exists, precalcModel_p);
            if (precalcModel_p)
            {
                logger_p 	<< LogIO::NORMAL << LogOrigin("UVContSubTVI", __FUNCTION__)
                                << "Producing continuum estimate in the MODEL_DATA column"
                                << LogIO::POST;
            }
	}

	exists = configuration.fieldNumber ("denoising_lib");
	if (exists >= 0)
	{
		configuration.get (exists, withDenoisingLib_p);

		if (withDenoisingLib_p)
		{
			logger_p << LogIO::NORMAL << LogOrigin("UVContSubTVI", __FUNCTION__)
                                 << "Using GSL based multiparameter regression with linear "
                                 << "least-squares fitting" << LogIO::POST;
		}
	}

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
                                     << "Requested " <<  nThreads_p
                                     << " OMP threads but maximum possible is " << omp_get_max_threads()<< endl
                                     << "Setting number of OMP threads to " << omp_get_max_threads() << endl
                                     << "Check OMP_NUM_THREADS environmental variable and number of cores in your system"
                                     << LogIO::POST;
                            nThreads_p = omp_get_max_threads();
			}
			else
			{
                            logger_p << LogIO::NORMAL << LogOrigin("UVContSubTVI", __FUNCTION__)
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
void UVContSubTVI::populatePerFieldSpec(int fieldID,
                                        const std::vector<InFitSpec> &fitSpecs)
{
    // TODO: transition list->record. Join individual spw:chan string, make map of
    // spw->fitorder
    std::unordered_map<int, int> perSpwFitOrder;
    std::string fullSpwStr = "";
    for (const auto &spec : fitSpecs) {
        if (fullSpwStr == "NONE") {
            break;
        } else if (fullSpwStr.length() == 0) {
            fullSpwStr = spec.first;
        } else {
            // MSSelection syntax spw:chan, SPWs separated by commas
            fullSpwStr += "," + spec.first;
        }
        auto spw = spwIDFromSpwStr(spec.first);
        perSpwFitOrder[spw] = spec.second;
    }

    // Some selection string
    MSSelection mssel;
    mssel.setSpwExpr(fullSpwStr);
    // This access the MS directly: far from ideal (CAS-11638) but no easy solution
    const auto spwchan = mssel.getChanList(&(inputVii_p->ms()));

    // TODO: move this out into a function
    // Create line-free channel map based on MSSelection channel ranges
    const auto nSelections = spwchan.shape()[0];
    unordered_map<Int,vector<Int> > lineFreeChannelSelMap;
    for (uInt selection_i=0; selection_i<nSelections; ++selection_i)
    {
        auto spw = spwchan(selection_i,0);
        if (lineFreeChannelSelMap.find(spw) == lineFreeChannelSelMap.end())
        {
            lineFreeChannelSelMap[spw].clear(); // Accessing the vector creates it
        }

        const auto channelStart = spwchan(selection_i, 1);
        const auto channelStop = spwchan(selection_i, 2);
        const auto channelStep = spwchan(selection_i, 3);
        for (auto inpChan=channelStart; inpChan<=channelStop; inpChan += channelStep)
        {
            lineFreeChannelSelMap[spw].push_back(inpChan);
        }
    }

    // Create line-free channel mask, spw->channel_mask
    unordered_map<int, FitSpec> lineFreeChannelMaskMap;   // rename: fitSpecMap
    for (auto const spwInp: spwInpChanIdxMap_p)
    {
        const auto spw = spwInp.first;
        if (lineFreeChannelMaskMap.find(spw) == lineFreeChannelMaskMap.end())
        {
            if (lineFreeChannelSelMap.size() > 0 && 0 == lineFreeChannelSelMap[spw].size()) {
                // Some SPWs have been selected, but this SPW doesn't have any selection
                //   => a 0-elements mask says there is nothing to mask or fit here, or
                // otherwise we'd make the effort to prepare and call the fit routine for an
                // "all masked/True" channel mask which will not even start minimizing
                lineFreeChannelMaskMap[spw].lineFreeChannelMask = Vector<bool>();
            } else {
                auto &mask = lineFreeChannelMaskMap[spw].lineFreeChannelMask;
                mask = Vector<bool>(spwInp.second.size(), true);
                for (uInt selChanIdx=0; selChanIdx<lineFreeChannelSelMap[spw].size();
                     ++selChanIdx)
                {
                    const auto selChan = lineFreeChannelSelMap[spw][selChanIdx];
                    mask(selChan) = False;
                }
            }

            unsigned int fitOrder = fitOrder_p;
            const auto find = perSpwFitOrder.find(spw);
            if (find != perSpwFitOrder.end()) {
                fitOrder = find->second;
            }
            lineFreeChannelMaskMap[spw].fitOrder = fitOrder;
        }
    }

    // Poppulate per field map: field -> spw -> channel_mask
    // emplace struct (linefreechannelmaskmap, + fitorder)
    perFieldSpecMap_p.emplace(fieldID, lineFreeChannelMaskMap);
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

    // TODO: split to func fitspec_p -> populatePerFieldSpec
    // Process line-free channel specifications
    for (const auto item: fitspec_p) {
        unordered_map<int, FitSpec> fieldSpecMap;
        // Parse line-free channel selection using MSSelection syntax
        const auto fieldID = item.first;
        const auto &specs = item.second;

        // TODO: message 'parsing fit spw string' needs revisiting
        bool noneFound = false;
        for (const auto spwSpec : specs) {
            const auto spwStr = spwSpec.first;
            const auto order = spwSpec.second;
            const auto fieldName = fieldNameFromId(fieldID);
            logger_p << LogIO::NORMAL << LogOrigin("UVContSubTVI", __FUNCTION__)
                     << "Parsing fit spw:chan string and order for field: " << fieldName
                     << ", spw/chan: '" << spwStr << "', order: " << order << LogIO::POST;
            if (spwStr == "NONE") {
                noneFound = true;
                if (specs.size() != 1) {
                throw AipsError("For field '" + fieldName + "' A \"NONE\" fit specification "
                                "has been given but additional fit specs have been given");
                }
            }
        }

        if (noneFound) {
            // Not inserting any entries in perFieldSpecMap_p[fieldID]
            // implies no transformation for that field (inputVis -> outputVis)
            continue;
        }

        if (1 == specs.size() && specs[0].first.empty()) {
            // -1 is the "all-fields-included" field pseudo-index
            // empty spw string -> all SPW, channels, leave all SPW masks unset
            if (-1 == fieldID) {
                perFieldSpecMap_p.emplace(fieldID, fieldSpecMap);
            }
            continue;
        }

        cerr << "specs.size: " << specs.size() << ", empty: " << specs.empty() << std::endl;
        if (! specs.empty()) {
            cerr << "specs[0] " << specs[0].first << ", " << specs[0].second << std::endl;
        }
        populatePerFieldSpec(fieldID, specs);
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

    // save it for visibilityModel
    if (precalcModel_p) {
        // Or otherwise the next assignment would fail a conform check
        if (precalcModelVis_p.shape() != vis.shape()) {
            precalcModelVis_p.resize(vis.shape());
        }
        precalcModelVis_p = vb->visCube() - vis;
    }
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

    // save it for visibilityModel   ====> TODO: put into function
    if (precalcModel_p) {
        // Or otherwise the next assignment would fail a conform check
        if (precalcModelVis_p.shape() != vis.shape()) {
            precalcModelVis_p.resize(vis.shape());
        }
        precalcModelVis_p = vb->visCubeCorrected() - vis;
    }
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void UVContSubTVI::visibilityModel (Cube<Complex> & vis) const
{
	// Get input VisBuffer
	VisBuffer2 *vb = getVii()->getVisBuffer();

        // from visiblityObserved we have calculated the polynomial subtraction
        if (precalcModel_p && precalcModelVis_p.shape() == vb->visCubeModel().shape()) {
            vis = precalcModelVis_p;
            return;
        }

	// Transform data
	transformDataCube(vb->visCubeModel(),vb->weightSpectrum(),vis);

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void UVContSubTVI::result(Record &res) const
{
    auto acc = result_p.getAccumulatedResult();
    res.defineRecord("uvcontsub", acc);
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

    auto fieldID = vb->fieldId()[0];
    // First check the "all fields" (no per-individual field list in fitspec)
    auto fieldMapIt = perFieldSpecMap_p.find(-1);
    if (fieldMapIt == perFieldSpecMap_p.end()) {
        // otherwise, check for individual fields (coming from a fitspec list)
        fieldMapIt = perFieldSpecMap_p.find(fieldID);
        if (fieldMapIt == perFieldSpecMap_p.end()) {
            // This was a "NONE" = no-subtraction for this field ==> no-op
            outputVis = inputVis;
            return;
        }
    }

    Int spwId = vb->spectralWindows()(0);

    // Get input line-free channel mask and fitorder
    Vector<bool> *lineFreeChannelMask = nullptr;
    auto fitOrder = fitOrder_p;
    auto spwMap = fieldMapIt->second;
    auto maskLookup = spwMap.find(spwId);
    if (maskLookup != spwMap.end())
    {
        if (maskLookup->second.lineFreeChannelMask.nelements() == 0) {
            // This was a non-selected SPW in a non-empty SPW selection string ==> no-op
            outputVis = inputVis;
            return;
        } else {
            lineFreeChannelMask = &(maskLookup->second.lineFreeChannelMask);
            fitOrder = maskLookup->second.fitOrder;
        }
    }

    // Get polynomial model for this SPW (depends on number of channels and gridding)
    // TODO: this if to keep reusing the polynomialmodels is no longer valid!!!
    const auto freqIter = inputFrequencyMap_p.find(spwId);
    const Vector<Double> &inputFrequencies = vb->getFrequencies(0);
    // STL should trigger move semantics
    // TODO: unique_ptr. Perhaps per field-spw pair map - But can be big
    //  n_fields X n_spw X n_chans
    if (freqIter != inputFrequencyMap_p.end()) {
        delete inputFrequencyMap_p.find(spwId)->second;
    }
    inputFrequencyMap_p[spwId] =
        new denoising::GslPolynomialModel<double>(inputFrequencies, fitOrder);

    // Reshape output data before passing it to the DataCubeHolder
    outputVis.resize(getVisBuffer()->getShape(),False);

    // Get input flag Cube
    const Cube<bool> &flagCube = vb->flagCube();

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
            auto scanID = vb->scan()[0];
            uInt nCorrs = vb->getShape()(0);
            for (uInt corrIdx=0; corrIdx < nCorrs; corrIdx++)
            {
                Complex chisq =
                    transformDataCore(inputFrequencyMap_p[spwId], lineFreeChannelMask,
                                      inputVis, flagCube, inputWeight, outputVis, corrIdx);
                result_p.addOneFit(fieldID, scanID, spwId, (int)corrIdx, chisq);
            }
        }

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> Complex UVContSubTVI::transformDataCore(denoising::GslPolynomialModel<Double>* model,
                                                          Vector<bool> *lineFreeChannelMask,
                                                          const Cube<T> &inputVis,
                                                          const Cube<bool> &inputFlags,
                                                          const Cube<Float> &inputWeight,
                                                          Cube<T> &outputVis,
                                                          Int parallelCorrAxis) const
{
    // Gather input data
    DataCubeMap inputData;
    DataCubeHolder<T> inputVisCubeHolder(inputVis);
    DataCubeHolder<bool> inputFlagCubeHolder(inputFlags);
    DataCubeHolder<Float> inputWeightsCubeHolder(inputWeight);
    inputData.add(MS::DATA,inputVisCubeHolder);
    inputData.add(MS::FLAG,inputFlagCubeHolder);
    inputData.add(MS::WEIGHT_SPECTRUM,inputWeightsCubeHolder);

    // Gather output data
    DataCubeMap outputData;
    DataCubeHolder<T> outputVisCubeHolder(outputVis);
    outputData.add(MS::DATA,outputVisCubeHolder);

    Complex chisq;
    if (want_cont_p)
    {
        if (withDenoisingLib_p)
        {
            UVContEstimationDenoisingKernel<T> kernel(model,niter_p,lineFreeChannelMask);
            UVContSubTransformEngine<T> transformer(&kernel,&inputData,&outputData);
            transformFreqAxis2(inputVis.shape(),transformer,parallelCorrAxis);
            chisq = kernel.getChiSquared();
        }
        else
        {
            UVContEstimationKernel<T> kernel(model,lineFreeChannelMask);
            UVContSubTransformEngine<T> transformer(&kernel,&inputData,&outputData);
            transformFreqAxis2(inputVis.shape(),transformer,parallelCorrAxis);
            chisq = kernel.getChiSquared();
        }
    }
    else
    {
        if (withDenoisingLib_p)
        {
            UVContSubtractionDenoisingKernel<T> kernel(model,niter_p,lineFreeChannelMask);
            UVContSubTransformEngine<T> transformer(&kernel,&inputData,&outputData);
            transformFreqAxis2(inputVis.shape(),transformer,parallelCorrAxis);
            chisq = kernel.getChiSquared();
        }
        else
        {
            UVContSubtractionKernel<T> kernel(model,lineFreeChannelMask);
            UVContSubTransformEngine<T> transformer(&kernel,&inputData,&outputData);
            transformFreqAxis2(inputVis.shape(),transformer,parallelCorrAxis);
            chisq = kernel.getChiSquared();
        }
    }

    return chisq;
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
														Vector<bool> *lineFreeChannelMask)
{
	model_p = model;
	fitOrder_p = model_p->ncomponents()-1;
	freqPows_p.reference(model_p->getModelMatrix());
	frequencies_p.reference(model_p->getLinearComponentFloat());

	lineFreeChannelMask_p = lineFreeChannelMask ? lineFreeChannelMask : nullptr;
	debug_p = False;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> void UVContSubKernel<T>::kernel(DataCubeMap *inputData,
                                                  DataCubeMap *outputData)
{
	// Get input/output data
	Vector<T> &outputVector = outputData->getVector<T>(MS::DATA);
	Vector<T> &inputVector = inputData->getVector<T>(MS::DATA);

	// Apply line-free channel mask
	Vector<bool> &inputFlags = inputData->getVector<bool>(MS::FLAG);
	if (lineFreeChannelMask_p != nullptr)
            inputFlags |= *lineFreeChannelMask_p;

	// Calculate number of valid data points and adapt fit
	size_t validPoints = nfalse(inputFlags);
	if (validPoints > 0)
	{
		bool restoreDefaultPoly = False;
		uInt tmpFitOrder = fitOrder_p;

		// Reduce fit order to match number of valid points
		if (validPoints <= fitOrder_p)
		{
			changeFitOrder(validPoints-1);
			restoreDefaultPoly = true;
		}

		// Get weights
		Vector<Float> &inputWeight = inputData->getVector<Float>(MS::WEIGHT_SPECTRUM);

		// Convert flags to mask
		Vector<bool> mask = !inputFlags;

		// Calculate and subtract continuum
		chisq_p = kernelCore(inputVector,mask,inputWeight,outputVector);

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
}


//////////////////////////////////////////////////////////////////////////
// UVContSubtractionKernel class
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> UVContSubtractionKernel<T>::UVContSubtractionKernel(	denoising::GslPolynomialModel<Double>* model,
																		Vector<bool> *lineFreeChannelMask):
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
	fitter_p.asWeight(true);

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
template<class T> Complex UVContSubtractionKernel<T>::kernelCore(	Vector<Complex> &inputVector,
																Vector<bool> &inputFlags,
																Vector<Float> &inputWeights,
																Vector<Complex> &outputVector)
{
	// Fit for imaginary and real components separately
	Vector<Float> realCoeff;
	Vector<Float> imagCoeff;
	realCoeff = fitter_p.fit(frequencies_p, real(inputVector), inputWeights, &inputFlags);
        double realChisq = fitter_p.chiSquare();
	imagCoeff = fitter_p.fit(frequencies_p, imag(inputVector), inputWeights, &inputFlags);
        double imagChisq = fitter_p.chiSquare();

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

	return Complex(realChisq, imagChisq);
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> Complex UVContSubtractionKernel<T>::kernelCore(	Vector<Float> &inputVector,
																Vector<bool> &inputFlags,
																Vector<Float> &inputWeights,
																Vector<Float> &outputVector)
{
	// Fit model
	Vector<Float> coeff;
	coeff = fitter_p.fit(frequencies_p, inputVector, inputWeights, &inputFlags);
        const double chisq = fitter_p.chiSquare();

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

	if (debug_p)
	{
		LogIO logger;
		logger << "fit order = " << fitOrder_p << LogIO::POST;
		logger << "coeff =" << coeff << LogIO::POST;
		logger << "inputFlags =" << inputFlags << LogIO::POST;
		logger << "inputWeights =" << inputWeights << LogIO::POST;
		logger << "inputVector =" << inputVector << LogIO::POST;
		logger << "outputVector =" << outputVector << LogIO::POST;
	}

	return Complex(chisq, chisq);
}


//////////////////////////////////////////////////////////////////////////
// UVContEstimationKernel class
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> UVContEstimationKernel<T>::UVContEstimationKernel(	denoising::GslPolynomialModel<Double>* model,
																		Vector<bool> *lineFreeChannelMask):
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
	fitter_p.asWeight(true);

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
template<class T> Complex UVContEstimationKernel<T>::kernelCore(Vector<Complex> &inputVector,
                                                                Vector<bool> &inputFlags,
                                                                Vector<Float> &inputWeights,
                                                                Vector<Complex> &outputVector
                                                                )
{
	// Fit for imaginary and real components separately
	Vector<Float> realCoeff;
	Vector<Float> imagCoeff;
	realCoeff = fitter_p.fit(frequencies_p, real(inputVector), inputWeights, &inputFlags);
        double realChisq = fitter_p.chiSquare();

	imagCoeff = fitter_p.fit(frequencies_p, imag(inputVector), inputWeights, &inputFlags);
        double imagChisq = fitter_p.chiSquare();

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

	return Complex(realChisq, imagChisq);
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> Complex UVContEstimationKernel<T>::kernelCore(	Vector<Float> &inputVector,
																Vector<bool> &inputFlags,
																Vector<Float> &inputWeights,
																Vector<Float> &outputVector)
{
	// Fit model
	Vector<Float> coeff;
	coeff = fitter_p.fit(frequencies_p, inputVector, inputWeights, &inputFlags);
	double chisq = fitter_p.chiSquare();

	// Fill output data
	outputVector = coeff(0);
	for (uInt order_idx = 1; order_idx <= fitOrder_p; order_idx++)
	{
		for (uInt chan_idx=0; chan_idx < outputVector.size(); chan_idx++)
		{
			outputVector(chan_idx) += (freqPows_p(order_idx,chan_idx))*coeff(order_idx);
		}
	}

	return Complex(chisq, chisq);
}

//////////////////////////////////////////////////////////////////////////
// UVContSubtractionDenoisingKernel class
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> UVContSubtractionDenoisingKernel<T>::UVContSubtractionDenoisingKernel(denoising::GslPolynomialModel<Double>* model,
																						size_t nIter,
																						Vector<bool> *lineFreeChannelMask):
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
template<class T> Complex UVContSubtractionDenoisingKernel<T>::kernelCore(	Vector<T> &inputVector,
																		Vector<bool> &inputFlags,
																		Vector<Float> &inputWeights,
																		Vector<T> &outputVector)
{	fitter_p.setWeightsAndFlags(inputWeights,inputFlags);
    vector<T> coeff;
    Complex chisq;
    tie(coeff, chisq) = fitter_p.calcFitCoeff(inputVector);

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

	return chisq;
}

//////////////////////////////////////////////////////////////////////////
// UVContEstimationDenoisingKernel class
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> UVContEstimationDenoisingKernel<T>::UVContEstimationDenoisingKernel(	denoising::GslPolynomialModel<Double>* model,
																						size_t nIter,
																						Vector<bool> *lineFreeChannelMask):
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
template<class T> Complex UVContEstimationDenoisingKernel<T>::kernelCore(	Vector<T> &inputVector,
																		Vector<bool> &inputFlags,
																		Vector<Float> &inputWeights,
																		Vector<T> &outputVector)
{
	fitter_p.setWeightsAndFlags(inputWeights,inputFlags);
    Vector<T> coeff;
    Complex chisq;
    tie(coeff, chisq) = fitter_p.calcFitCoeff(inputVector);
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

	return chisq;
}

} //# NAMESPACE VI - END

} //# NAMESPACE CASA - END


