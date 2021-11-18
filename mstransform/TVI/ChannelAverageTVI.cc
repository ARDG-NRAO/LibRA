//# ChannelAverageTVI.h: This file contains the implementation of the ChannelAverageTVI class.
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

#include <mstransform/TVI/ChannelAverageTVI.h>
#include <casa/Arrays/VectorIter.h>

#ifdef _OPENMP
 #include <omp.h>
#endif


using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

namespace vi { //# NAMESPACE VI - BEGIN

//////////////////////////////////////////////////////////////////////////
// ChannelAverageTVI class
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
ChannelAverageTVI::ChannelAverageTVI(	ViImplementation2 * inputVii,
										const Record &configuration):
										FreqAxisTVI (inputVii)
{
    // Parse and check configuration parameters
    // Note: if a constructor finishes by throwing an exception, the memory
    // associated with the object itself is cleaned up — there is no memory leak.
    if (not parseConfiguration(configuration))
        throw AipsError("Error parsing ChannelAverageTVI configuration");

    if (inputVii == nullptr)
        throw AipsError("Input Vi is empty");

    initialize();

    return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Bool ChannelAverageTVI::parseConfiguration(const Record &configuration)
{
	int exists = -1;
	Bool ret = true;

	// Parse chanbin parameter (mandatory)
	exists = -1;
	exists = configuration.fieldNumber ("chanbin");
	if (exists >= 0)
	{
		if ( configuration.type(exists) == casacore::TpInt )
		{
			Int freqbin;
			configuration.get (exists, freqbin);
			chanbin_p = Vector<Int>(spwInpChanIdxMap_p.size(),freqbin);
		}
		else if ( configuration.type(exists) == casacore::TpArrayInt)
		{
			configuration.get (exists, chanbin_p);
		}
		else
		{
			ret = false;
			logger_p << LogIO::SEVERE << LogOrigin("ChannelAverageTVI", __FUNCTION__)
					<< "Wrong format for chanbin parameter (only Int and arrayInt are supported) "
					<< LogIO::POST;
		}

		logger_p << LogIO::NORMAL << LogOrigin("ChannelAverageTVI", __FUNCTION__)
				<< "Channel bin is " << chanbin_p << LogIO::POST;
		if (anyEQ(chanbin_p,0)) {
		  logger_p << LogIO::NORMAL << LogOrigin("ChannelAverageTVI", __FUNCTION__)
			   << "  NB: Channel bin '0' means no channel averaging for the corresponding spw." << LogIO::POST;
		}
	}
	else
	{
		ret = false;
		logger_p << LogIO::SEVERE << LogOrigin("ChannelAverageTVI", __FUNCTION__)
				<< "chanbin parameter not found in configuration "
				<< LogIO::POST;
	}

	exists = configuration.fieldNumber ("flagdataFlagPropagation");
	flagdataFlagPropagation_p = exists >= 0;

	// Check consistency between chanbin vector and selected SPW/Chan map
	if (chanbin_p.size() !=  spwInpChanIdxMap_p.size())
	{
		ret = false;
		logger_p << LogIO::SEVERE << LogOrigin("ChannelAverageTVI", __FUNCTION__)
				<< "Number of elements in chanbin vector does not match number of selected SPWs"
				<< LogIO::POST;
	}

	return ret;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void ChannelAverageTVI::initialize()
{
	// Populate nchan input-output maps
	Int spw;
	uInt spw_idx = 0;
	map<Int,vector<Int> >::iterator iter;

	for(iter=spwInpChanIdxMap_p.begin();iter!=spwInpChanIdxMap_p.end();iter++)
	{
		spw = iter->first;

		// No averaging when user specifies 0
		if (chanbin_p(spw_idx)==0) {
		  logger_p << LogIO::DEBUG1 << LogOrigin("ChannelAverageTVI", __FUNCTION__)
			   << "Specified chanbin for spw " << spw
			   << " of 0 means no averaging."
			   << LogIO::POST;
		  
		  spwChanbinMap_p[spw] = 1;
		}
		// Make sure that chanbin is greater than 1
		else if ((uInt)chanbin_p(spw_idx) <= 1)
		{
			logger_p << LogIO::DEBUG1 << LogOrigin("ChannelAverageTVI", __FUNCTION__)
					<< "Specified chanbin for spw " << spw
					<< " less than 1 falls back to the default number of"
					<< " existing/selected channels: " << iter->second.size()
					<< LogIO::POST;

			spwChanbinMap_p[spw] = iter->second.size();
		}
		// Make sure that chanbin does not exceed number of selected channels
		else if ((uInt)chanbin_p(spw_idx) > iter->second.size())
		{
			logger_p << LogIO::DEBUG1 << LogOrigin("ChannelAverageTVI", __FUNCTION__)
					<< "Number of selected channels " << iter->second.size()
					<< " for SPW " << spw
				        << " is smaller than specified chanbin " << (uInt)chanbin_p(spw_idx) << endl
					<< "Setting chanbin to " << iter->second.size()
					<< " for SPW " << spw
					<< LogIO::POST;
			spwChanbinMap_p[spw] = iter->second.size();
		}
		else
		{
			spwChanbinMap_p[spw] = chanbin_p(spw_idx);
		}

		// Calculate number of output channels per spw
		spwOutChanNumMap_p[spw] = spwInpChanIdxMap_p[spw].size() / spwChanbinMap_p[spw];
		if (spwInpChanIdxMap_p[spw].size() % spwChanbinMap_p[spw] > 0) spwOutChanNumMap_p[spw] += 1;

		spw_idx++;
	}

	return;
}

#define DOJUSTO false
// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void ChannelAverageTVI::flag(Cube<Bool>& flagCube) const
{

        // Pass-thru for single-channel case
        if (getVii()->visibilityShape()[1]==1) {
	  getVii()->flag(flagCube);
	  return;
	}
    
	// Get input VisBuffer and SPW
	VisBuffer2 *vb = getVii()->getVisBuffer();
	Int inputSPW = vb->spectralWindows()(0);

	// Pass-thru for chanbin=1 case:
	if (spwChanbinMap_p[inputSPW]==1) {
	  getVii()->flag(flagCube);
	  return;
	}

#ifdef _OPENMP
	// Pre-load relevant input info and start clock
	vb->flagCube();
	Double time0=omp_get_wtime();
#endif


	// Reshape output data before passing it to the DataCubeHolder
	flagCube.resize(getVisBuffer()->getShape(),false);

	// Gather input data
	DataCubeMap inputData;
	DataCubeHolder<Bool> inputFlagCubeHolder(vb->flagCube());
	inputData.add(MS::FLAG,inputFlagCubeHolder);

	// Gather output data
	DataCubeMap outputData;
	DataCubeHolder<Bool> outputFlagCubeHolder(flagCube);
	outputData.add(MS::FLAG,outputFlagCubeHolder);

	// Configure Transformation Engine
	LogicalANDKernel<Bool> kernel;
	uInt width = spwChanbinMap_p[inputSPW];
	ChannelAverageTransformEngine<Bool> transformer(&kernel,&inputData,&outputData,width);

	// Transform data
	if (DOJUSTO) {
	  transformFreqAxis2(vb->getShape(),transformer);
	} else {
	  transformer.transformAll();
	}

#ifdef _OPENMP
	// Accumulate elapsed time
	Tfl_+=omp_get_wtime()-time0;
#endif
	  
	  return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void ChannelAverageTVI::floatData (Cube<Float> & vis) const
{

        // Pass-thru for single-channel case
        if (getVii()->visibilityShape()[1]==1) {
	  getVii()->floatData(vis);
	  return;
	}

	// Get input VisBuffer and SPW
	VisBuffer2 *vb = getVii()->getVisBuffer();
	Int inputSPW = vb->spectralWindows()(0);

	// Pass-thru for chanbin=1 case:
	if (spwChanbinMap_p[inputSPW]==1) {
	  getVii()->floatData(vis);
	  return;
	}

	// Reshape output data before passing it to the DataCubeHolder
	vis.resize(getVisBuffer()->getShape(),false);

	// Gather input data
	DataCubeMap inputData;
	DataCubeHolder<Float> inputVisCubeHolder(vb->visCubeFloat());
	DataCubeHolder<Bool> inputFlagCubeHolder(vb->flagCube());
	DataCubeHolder<Float> weightCubeHolder(vb->weightSpectrum());
	inputData.add(MS::DATA,inputVisCubeHolder);
	inputData.add(MS::FLAG,inputFlagCubeHolder);
	inputData.add(MS::WEIGHT_SPECTRUM,weightCubeHolder);

	// Gather output data
	DataCubeMap outputData;
	DataCubeHolder<Float> outputVisCubeHolder(vis);
	outputData.add(MS::DATA,outputVisCubeHolder);

	// Configure Transformation Engine
	uInt width = spwChanbinMap_p[inputSPW];
	WeightedChannelAverageKernel<Float> kernel;
	ChannelAverageTransformEngine<Float> transformer(&kernel,&inputData,&outputData,width);

	// Transform data
	if (DOJUSTO) {
	  transformFreqAxis2(vb->getShape(),transformer);
	} else {
	  transformer.transformAll();
	}

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void ChannelAverageTVI::visibilityObserved (Cube<Complex> & vis) const
{

        // Pass-thru for single-channel case
        if (getVii()->visibilityShape()[1]==1) {
	  getVii()->visibilityObserved(vis);
	  return;
	}

	// Get input VisBuffer and SPW
	VisBuffer2 *vb = getVii()->getVisBuffer();
	Int inputSPW = vb->spectralWindows()(0);

	// Reshape output data before passing it to the DataCubeHolder
	vis.resize(getVisBuffer()->getShape(),false);

	// Get weightSpectrum from sigmaSpectrum
	Cube<Float> weightSpFromSigmaSp;
	weightSpFromSigmaSp.resize(vb->sigmaSpectrum().shape(),false);
	weightSpFromSigmaSp = vb->sigmaSpectrum(); // = Operator makes a copy
	arrayTransformInPlace (weightSpFromSigmaSp,sigmaToWeight);

	// Gather input data
	DataCubeMap inputData;
	DataCubeHolder<Complex> inputVisCubeHolder(vb->visCube());
	DataCubeHolder<Bool> inputFlagCubeHolder(vb->flagCube());
	DataCubeHolder<Float> weightCubeHolder(weightSpFromSigmaSp);
	inputData.add(MS::DATA,inputVisCubeHolder);
	inputData.add(MS::FLAG,inputFlagCubeHolder);
	inputData.add(MS::WEIGHT_SPECTRUM,weightCubeHolder);

	// Gather output data
	DataCubeMap outputData;
	DataCubeHolder<Complex> outputVisCubeHolder(vis);
	outputData.add(MS::DATA,outputVisCubeHolder);

	// Configure Transformation Engine
	uInt width = spwChanbinMap_p[inputSPW];
	WeightedChannelAverageKernel<Complex> kernel;
	ChannelAverageTransformEngine<Complex> transformer(&kernel,&inputData,&outputData,width);

	// Transform data
	if (DOJUSTO) {
	  transformFreqAxis2(vb->getShape(),transformer);
	}
	else {
	  transformer.transformAll();
	}

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void ChannelAverageTVI::visibilityCorrected (Cube<Complex> & vis) const
{

        // Pass-thru for single-channel case
        if (getVii()->visibilityShape()[1]==1) {
	  getVii()->visibilityCorrected(vis);
	  return;
	}

	// Get input VisBuffer and SPW
	VisBuffer2 *vb = getVii()->getVisBuffer();
	Int inputSPW = vb->spectralWindows()(0);

	// Pass-thru for chanbin=1 case:
	if (spwChanbinMap_p[inputSPW]==1) {
	  getVii()->visibilityCorrected(vis);
	  return;
	}


#ifdef _OPENMP
	// Pre-load relevant input info and start clock
	vb->visCubeCorrected();
	vb->flagCube();
	vb->weightSpectrum();
	Double time0=omp_get_wtime();
#endif


	// Reshape output data before passing it to the DataCubeHolder
	vis.resize(getVisBuffer()->getShape(),false);

	// Gather input data
	DataCubeMap inputData;
	DataCubeHolder<Complex> inputVisCubeHolder(vb->visCubeCorrected());
	DataCubeHolder<Bool> inputFlagCubeHolder(vb->flagCube());
	DataCubeHolder<Float> weightCubeHolder(vb->weightSpectrum());
	inputData.add(MS::DATA,inputVisCubeHolder);
	inputData.add(MS::FLAG,inputFlagCubeHolder);
	inputData.add(MS::WEIGHT_SPECTRUM,weightCubeHolder);

	// Gather output data
	DataCubeMap outputData;
	DataCubeHolder<Complex> outputVisCubeHolder(vis);
	outputData.add(MS::DATA,outputVisCubeHolder);

	// Configure Transformation Engine
	uInt width = spwChanbinMap_p[inputSPW];
	WeightedChannelAverageKernel<Complex> kernel;
	ChannelAverageTransformEngine<Complex> transformer(&kernel,&inputData,&outputData,width);

	// Transform data
	if (DOJUSTO) {
	  transformFreqAxis2(vb->getShape(),transformer);
	} else {
	  transformer.transformAll();

	  /*
	  // Demo version upon which upgrades to DataCubeHolder/Map (in UtilsTVI.h)
	  //  and ChannelAverageTransformEngine::transformAll() are based
	  // As written here, it averages _all_ channels (no 
	  //  partial binning is supported). 
	  // NB: This is a bit faster than transformAll
	  //     (due mainly to fewer function calls? E.g. kernel.kernel()? )
	  vis.set(0.0f);  // initialize the output cube
	  Cube<Complex> ivis(vb->visCubeCorrected());
	  Cube<Float> iwtsp(vb->weightSpectrum());
	  Cube<Bool> ifl(vb->flagCube());
	  VectorIterator<Complex> vi(ivis,1);
	  VectorIterator<Float> wi(iwtsp,1);
	  VectorIterator<Bool> fi(ifl,1);
	  VectorIterator<Complex> vo(vis,1);
	  
	  Vector<Complex>& viv = vi.vector();
	  Vector<Float>& wiv = wi.vector();
	  Vector<Bool>& fiv = fi.vector();
	  Vector<Complex>& vov = vo.vector();
	  
	  Int nchan=viv.nelements();
	  
	  while (!vi.pastEnd()) {
	    
	    Float swt(0.0f);
	    for (Int ich=0;ich<nchan;++ich) {
	      if (!fiv(ich)) {
		vov(0)+=(viv(ich)*wiv(ich));
		swt+=wiv(ich);
	      }
	    }
	    if (swt>0.0f)
	      vov(0)/=swt;
	    else
	      vov(0)=0.0;
	    
	    vi.next();
	    wi.next();
	    fi.next();
	    vo.next();
	  }
	  */
	}

#ifdef _OPENMP
	// Accumulate elapsed time
	Tcd_+=omp_get_wtime()-time0;
#endif

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void ChannelAverageTVI::visibilityModel (Cube<Complex> & vis) const
{

        // Pass-thru for single-channel case
        if (getVii()->visibilityShape()[1]==1) {
	  getVii()->visibilityModel(vis);
	  return;
	}

	// Get input VisBuffer and SPW
	VisBuffer2 *vb = getVii()->getVisBuffer();
	Int inputSPW = vb->spectralWindows()(0);

	// Pass-thru for chanbin=1 case:
	if (spwChanbinMap_p[inputSPW]==1) {
	  getVii()->visibilityModel(vis);
	  return;
	}

#ifdef _OPENMP
	// Pre-load relevant input info and start clock
	vb->visCubeModel();
	vb->flagCube();
	vb->weightSpectrum();
	Double time0=omp_get_wtime();
#endif


	// Reshape output data before passing it to the DataCubeHolder
	vis.resize(getVisBuffer()->getShape(),false);

	// Gather input data
	DataCubeMap inputData;
	DataCubeHolder<Complex> inputVisCubeHolder(vb->visCubeModel());
	DataCubeHolder<Bool> inputFlagCubeHolder(vb->flagCube());
	inputData.add(MS::DATA,inputVisCubeHolder);
	inputData.add(MS::FLAG,inputFlagCubeHolder);

	// Gather output data
	DataCubeMap outputData;
	DataCubeHolder<Complex> outputVisCubeHolder(vis);
	outputData.add(MS::DATA,outputVisCubeHolder);

	// Configure Transformation Engine
	uInt width = spwChanbinMap_p[inputSPW];
	FlaggedChannelAverageKernel<Complex> kernel;
	ChannelAverageTransformEngine<Complex> transformer(&kernel,&inputData,&outputData,width);

	// Transform data
	if (DOJUSTO) {
	  transformFreqAxis2(vb->getShape(),transformer);
	} else {
	  transformer.transformAll();
	}

#ifdef _OPENMP
	// Accumulate elapsed time
	Tmd_+=omp_get_wtime()-time0;
#endif

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void ChannelAverageTVI::weightSpectrum(Cube<Float> &weightSp) const
{

        // Pass-thru for single-channel case
        if (getVii()->visibilityShape()[1]==1) {
	  getVii()->weightSpectrum(weightSp);
	  return;
	}

	// Get input VisBuffer and SPW
	VisBuffer2 *vb = getVii()->getVisBuffer();
	Int inputSPW = vb->spectralWindows()(0);

	// Pass-thru for chanbin=1 case:
	if (spwChanbinMap_p[inputSPW]==1) {
	  getVii()->weightSpectrum(weightSp);;
	  return;
	}

#ifdef _OPENMP
	// Pre-load relevant input info and start clock
	vb->weightSpectrum();
	vb->flagCube();
	Double time0=omp_get_wtime();
#endif


	// Reshape output data before passing it to the DataCubeHolder
	weightSp.resize(getVisBuffer()->getShape(),false);

	// Gather input data
	DataCubeMap inputData;
	DataCubeHolder<Float> inputWeightCubeHolder(vb->weightSpectrum());
	DataCubeHolder<Bool> inputFlagCubeHolder(vb->flagCube());
	inputData.add(MS::DATA,inputWeightCubeHolder);
	inputData.add(MS::FLAG,inputFlagCubeHolder);

	// Gather output data
	DataCubeMap outputData;
	DataCubeHolder<Float> outputWeightCubeHolder(weightSp);
	outputData.add(MS::DATA,outputWeightCubeHolder);

	// Configure Transformation Engine
	uInt width = spwChanbinMap_p[inputSPW];
	ChannelAccumulationKernel<Float> kernel;
	ChannelAverageTransformEngine<Float> transformer(&kernel,&inputData,&outputData,width);

	// Transform data
	if (DOJUSTO) {
	  transformFreqAxis2(vb->getShape(),transformer);
	} else {
	  transformer.transformAll();
	}

#ifdef _OPENMP
	// Accumulate elapsed time
	Tws_+=omp_get_wtime()-time0;
#endif

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void ChannelAverageTVI::sigmaSpectrum(Cube<Float> &sigmaSp) const
{
        // Pass-thru for single-channel case
        if (getVii()->visibilityShape()[1]==1) {
	  getVii()->sigmaSpectrum(sigmaSp);
	  return;
	}

	// Get input VisBuffer and SPW
	VisBuffer2 *vb = getVii()->getVisBuffer();
	Int inputSPW = vb->spectralWindows()(0);

	// Pass-thru for chanbin=1 case:
	if (spwChanbinMap_p[inputSPW]==1) {
	  getVii()->sigmaSpectrum(sigmaSp);;
	  return;
	}

#ifdef _OPENMP
	// Pre-load relevant input info and start clock
	vb->sigmaSpectrum();
	vb->flagCube();
	Double time0=omp_get_wtime();
#endif

	// Reshape output data before passing it to the DataCubeHolder
	sigmaSp.resize(getVisBuffer()->getShape(),false);

	// Get weightSpectrum from sigmaSpectrum
	Cube<Float> weightSpFromSigmaSp;
	weightSpFromSigmaSp.resize(vb->sigmaSpectrum().shape(),false);
	weightSpFromSigmaSp = vb->sigmaSpectrum(); // = Operator makes a copy
	arrayTransformInPlace (weightSpFromSigmaSp,sigmaToWeight);

	// Gather input data
	DataCubeMap inputData;
	DataCubeHolder<Float> inputWeightCubeHolder(weightSpFromSigmaSp);
	DataCubeHolder<Bool> inputFlagCubeHolder(vb->flagCube());
	inputData.add(MS::DATA,inputWeightCubeHolder);
	inputData.add(MS::FLAG,inputFlagCubeHolder);

	// Gather output data
	DataCubeMap outputData;
	DataCubeHolder<Float> outputWeightCubeHolder(sigmaSp);
	outputData.add(MS::DATA,outputWeightCubeHolder);

	// Configure Transformation Engine
	uInt width = spwChanbinMap_p[inputSPW];
	ChannelAccumulationKernel<Float> kernel;
	ChannelAverageTransformEngine<Float> transformer(&kernel,&inputData,&outputData,width);

	// Transform data
	if (DOJUSTO) {
	  transformFreqAxis2(vb->getShape(),transformer);
	} else {
	  transformer.transformAll();
	}

	// Transform back from weight format to sigma format
	arrayTransformInPlace (sigmaSp,weightToSigma);

#ifdef _OPENMP
	// Accumulate elapsed time
	Tss_+=omp_get_wtime()-time0;
#endif

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Vector<Double> ChannelAverageTVI::getFrequencies ( Double time,
						   Int frameOfReference,
						   Int spectralWindowId,
						   Int msId) const
{

        // Pass-thru for chanbin=1 case
        if (spwChanbinMap_p[spectralWindowId]==1) {
	  return getVii()->getFrequencies(time,frameOfReference,spectralWindowId,msId);
	}

	// Get frequencies from input VI, which we will process
	Vector<Double> inputFrequencies = getVii()->getFrequencies(time,frameOfReference,
								   spectralWindowId,msId);
	     
	// Pass-thru for single-channel case
	//  NB: This does NOT depend on current iteration having same spw as specified spectralWindowId
	//  NB: Nor does it rely on sensing the apparent shape of the visibility data object (so that need not be pre-filled)
	if (inputFrequencies.nelements()==1)
	  return inputFrequencies;

	// Produce output (transformed) frequencies
	Vector<Double> outputFrecuencies(spwOutChanNumMap_p[spectralWindowId],0);

	// Gather input data
	DataCubeMap inputData;
	DataCubeHolder<Double> inputFrecuenciesHolder(inputFrequencies);
	inputData.add(MS::DATA,inputFrecuenciesHolder);

	// Gather output data
	DataCubeMap outputData;
	DataCubeHolder<Double> outputFrecuenciesHolder(outputFrecuencies);
	outputData.add(MS::DATA,outputFrecuenciesHolder);

	// Configure Transformation Engine
	PlainChannelAverageKernel<Double> kernel;
	uInt width = spwChanbinMap_p[spectralWindowId];
	ChannelAverageTransformEngine<Double> transformer(&kernel,&inputData,&outputData,width);

	// Transform data
	transformer.transform();

	return outputFrecuencies;
}

Vector<Double> ChannelAverageTVI::getChanWidths ( Double time,
						  Int frameOfReference,
						  Int spectralWindowId,
						  Int msId) const
{

        // Pass-thru for chanbin=1 case
        if (spwChanbinMap_p[spectralWindowId]==1) {
	  return getVii()->getChanWidths(time,frameOfReference,spectralWindowId,msId);
	}

	// Get widths from input VI
	Vector<Double> inputWidths = getVii()->getChanWidths(time,frameOfReference,
							     spectralWindowId,msId);
	// Pass-thru for single-channel case
	//  NB: This does NOT depend on current iteration having same spw as specified spectralWindowId
	//  NB: Nor does it rely on sensing the apparent shape of the visibility data object (so that need not be pre-filled)
	if (inputWidths.nelements()==1)
	  return inputWidths;

	// Produce output (summed) widths
	Vector<Double> outputWidths(spwOutChanNumMap_p[spectralWindowId],0);

	// Gather input data
	DataCubeMap inputData;
	DataCubeHolder<Double> inputWidthsHolder(inputWidths);
	inputData.add(MS::DATA,inputWidthsHolder);
	// We need flag vector (all false==unflagged) for ChannelAccumulationKernal (which just adds within bins)
	Vector<Bool> nonflagged(inputWidths.nelements(),false);
	DataCubeHolder<Bool> inputFlagHolder(nonflagged);
	inputData.add(MS::FLAG,inputFlagHolder);

	// Gather output data
	DataCubeMap outputData;
	DataCubeHolder<Double> outputWidthsHolder(outputWidths);
	outputData.add(MS::DATA,outputWidthsHolder);

	// Configure Transformation Engine
	//PlainChannelAverageKernel<Double> kernel;
	ChannelAccumulationKernel<Double> kernel;
	uInt width = spwChanbinMap_p[spectralWindowId];
	ChannelAverageTransformEngine<Double> transformer(&kernel,&inputData,&outputData,width);

	// Transform data
	transformer.transform();

	return outputWidths;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void ChannelAverageTVI::writeFlag (const Cube<Bool> & flag)
{
	// Create a flag cube with the input VI shape
	Cube<Bool> propagatedFlagCube;
	propagatedFlagCube = getVii()->getVisBuffer()->flagCube();

	// Propagate flags from the input cube to the propagated flag cube
	propagateChanAvgFlags(flag,propagatedFlagCube);

	// Pass propagated flag cube downstream for further propagation and/or writting
	getVii()->writeFlag(propagatedFlagCube);

	return;
}

/**
 * Strategy to support different ways of propagating flags from the 'transformed' cube to
 * the original ('propagated') cube. Iterates through rows, channels, correlations.
 *
 * This is meant to be used from propagateChanAvgFlags with at least two alternative
 * functors. One to propagate flags as required by flagdata (preserve pre-existing flags
 * in the original data cube), and a second one to propagate flags as required by plotms.
 * CAS-12737, CAS-9985, CAS-12205.
 *
 * @param transformedFlagCube Cube of flags after averaging
 * @param propagatedFlagClube Original cube of flags
 * @param inputOutputChan input->output channel mapping
 * @param propagate functor to implement the (back)propagation of flags (flagdata/plotms).
 */
template <typename Functor>
void cubePropagateFlags(const Cube<Bool> &transformedFlagCube,
                        Cube<Bool> &propagatedFlagCube,
                        const Vector<uInt> &inputOutputChan, Functor propagate)
{
    // Get propagated (input) shape
    const auto inputShape = propagatedFlagCube.shape();
    const uInt nCorr = inputShape(0);
    const uInt nChan = inputShape(1);
    const uInt nRows = inputShape(2);

    // Get transformed (output) shape
    const auto transformedShape = transformedFlagCube.shape();
    const auto nTransChan = transformedShape(1);

    uInt outChan;
    for (size_t row_i =0;row_i<nRows;row_i++)
    {
        for (size_t chan_i =0;chan_i<nChan;chan_i++)
        {
            outChan = inputOutputChan(chan_i);
            if (outChan < nTransChan)  // outChan >= nChan  may happen when channels are dropped
            {
                for (size_t corr_i =0;corr_i<nCorr;corr_i++)
                    propagate(row_i, chan_i, corr_i, outChan);
            }
        }
    }
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void ChannelAverageTVI::propagateChanAvgFlags (const Cube<Bool> &transformedFlagCube,
                                               Cube<Bool> &propagatedFlagCube)
{
    // Get current SPW and chanbin
    const VisBuffer2 *inputVB = getVii()->getVisBuffer();
    const Int inputSPW = inputVB->spectralWindows()(0);
    const uInt width = spwChanbinMap_p[inputSPW];

    // Map input-output channel
    const uInt nChan = propagatedFlagCube.shape()(1);
    uInt binCounts = 0;
    uInt transformedIndex = 0;
    Vector<uInt> inputOutputChan(nChan);
    for (size_t chan_i =0;chan_i<nChan;chan_i++)
    {
        binCounts += 1;

        if (binCounts > width)
        {
            binCounts = 1;
            transformedIndex += 1;
        }

        inputOutputChan(chan_i) = transformedIndex;
    }

    // Propagate chan-avg flags
    // Keeping two separate alternatives for 'flagdataFlagPropagation_p' (CAS-12737,
    // CAS-9985) until this issue is better settled.
    if (flagdataFlagPropagation_p)
    {
        cubePropagateFlags(transformedFlagCube, propagatedFlagCube, inputOutputChan,
                           [&transformedFlagCube, &propagatedFlagCube]
                           (size_t row_i, size_t chan_i, size_t corr_i, uInt outChan) {
                               if (transformedFlagCube(corr_i, outChan, row_i))
                                   propagatedFlagCube(corr_i, chan_i, row_i) = true;
                           });
    }
    else
    {
        cubePropagateFlags(transformedFlagCube, propagatedFlagCube, inputOutputChan,
                           [&transformedFlagCube, &propagatedFlagCube]
                           (size_t row_i, size_t chan_i, size_t corr_i, uInt outChan) {
                               propagatedFlagCube(corr_i, chan_i, row_i) =
                                   transformedFlagCube(corr_i, outChan, row_i);
                           });
    }
}

//////////////////////////////////////////////////////////////////////////
// ChannelAverageTVIFactory class
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
ChannelAverageTVIFactory::ChannelAverageTVIFactory (Record &configuration,
													ViImplementation2 *inputVii)
{
	inputVii_p = inputVii;
	configuration_p = configuration;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
vi::ViImplementation2 * ChannelAverageTVIFactory::createVi() const
{
	return new ChannelAverageTVI(inputVii_p,configuration_p);
}

//////////////////////////////////////////////////////////////////////////
// ChannelAverageTVILayerFactory class
//////////////////////////////////////////////////////////////////////////

ChannelAverageTVILayerFactory::ChannelAverageTVILayerFactory(Record &configuration) :
  ViiLayerFactory(),
  configuration_p(configuration)
{}

ViImplementation2* 
ChannelAverageTVILayerFactory::createInstance(ViImplementation2* vii0) const 
{
  // Make the ChannelAverageTVi2, using supplied ViImplementation2, and return it
  ViImplementation2 *vii = new ChannelAverageTVI(vii0,configuration_p);
  return vii;
}


//////////////////////////////////////////////////////////////////////////
// ChannelAverageTransformEngine class
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> ChannelAverageTransformEngine<T>::ChannelAverageTransformEngine(	ChannelAverageKernel<T> *kernel,
																					DataCubeMap *inputData,
																					DataCubeMap *outputData,
																					uInt width):
																					FreqAxisTransformEngine2<T>(inputData,
																												outputData)
{
	width_p = width;
	chanAvgKernel_p = kernel;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> void ChannelAverageTransformEngine<T>::transformAll()
{
  // NB: Does NOT implement "parallelCorrAxis" option 
  //  (see, e.g., FreqAxisTVI::transformFreqAxis2(...))

  // Set up the VectorIterators inside the DataCubeMap/Holders
  inputData_p->setupVecIter();
  outputData_p->setupVecIter();

  // Iterate implicitly over row and correlation
  while (!inputData_p->pastEnd()) {
    this->transform();   // processes the current channel axis
    inputData_p->next();
    outputData_p->next();
  }
}


// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> void ChannelAverageTransformEngine<T>::transform()
{
	uInt startChan = 0;
	uInt outChanIndex = 0;
	uInt inputSize = inputData_p->getVectorShape()(0);
	uInt outputSize = outputData_p->getVectorShape()(0);
	uInt tail = inputSize % width_p;
	uInt limit = inputSize - tail;
	while (startChan < limit)
	{
		chanAvgKernel_p->kernel(inputData_p,outputData_p,
								startChan,outChanIndex,width_p);
		startChan += width_p;
		outChanIndex += 1;
	}

	if (tail and (outChanIndex <= outputSize - 1) )
	{
		chanAvgKernel_p->kernel(inputData_p,outputData_p,
								startChan,outChanIndex,tail);
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
// PlainChannelAverageKernel class
//   (numerical averaging, ignoring flags)
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> void PlainChannelAverageKernel<T>::kernel(	DataCubeMap *inputData,
																DataCubeMap *outputData,
																uInt startInputPos,
																uInt outputPos,
																uInt width)
{
	Vector<T> &inputVector = inputData->getVector<T>(MS::DATA);
	Vector<T> &outputVector = outputData->getVector<T>(MS::DATA);

	uInt pos = startInputPos + 1;
	uInt counts = 1;
	T avg = inputVector(startInputPos);
	while (counts < width)
	{
		avg += inputVector(pos);
		counts += 1;
		pos += 1;
	}

	if (counts > 0)
	{
		avg /= counts;
	}

	outputVector(outputPos) = avg;

	return;
}

//////////////////////////////////////////////////////////////////////////
// FlaggedChannelAverageKernel class
//   (numerical averaging, respecting flags)
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> void FlaggedChannelAverageKernel<T>::kernel(DataCubeMap *inputData,
																DataCubeMap *outputData,
																uInt startInputPos,
																uInt outputPos,
																uInt width)
{
	T avg = 0;
	T normalization = 0;
	uInt inputPos = 0;
	Vector<T> &inputVector = inputData->getVector<T>(MS::DATA);
	Vector<Bool> &inputFlagVector = inputData->getVector<Bool>(MS::FLAG);
	Vector<T> &outputVector = outputData->getVector<T>(MS::DATA);
	Bool accumulatorFlag = inputFlagVector(startInputPos);

	for (uInt sample_i=0;sample_i<width;sample_i++)
	{
		// Get input index
		inputPos = startInputPos + sample_i;

		// true/true or false/false
		if (accumulatorFlag == inputFlagVector(inputPos))
		{
		        normalization += 1.0f;
		        avg += inputVector(inputPos);
		}
		// true/false: Reset accumulation when accumulator switches from flagged to unflag
		else if ( (accumulatorFlag == true) and (inputFlagVector(inputPos) == false) )
		{
			accumulatorFlag = false;
			normalization = 1.0f;
			avg = inputVector(inputPos);
		}

	}


	// Apply normalization factor
	if (normalization > 0)
	{
		avg /= normalization;
		outputVector(outputPos) = avg;
	}
	// If all weights are zero set accumulatorFlag to true
	else
	{
		accumulatorFlag = true;
		outputVector(outputPos) = 0; // If all weights are zero then the avg is 0 too
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
// WeightedChannelAverageKernel class
//   (weighted averaging, respecting flags)
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> void WeightedChannelAverageKernel<T>::kernel(	DataCubeMap *inputData,
																DataCubeMap *outputData,
																uInt startInputPos,
																uInt outputPos,
																uInt width)
{
	T avg = 0;
	T normalization = 0;
	uInt inputPos = 0;
	Vector<T> &inputVector = inputData->getVector<T>(MS::DATA);
	Vector<Bool> &inputFlagVector = inputData->getVector<Bool>(MS::FLAG);
	Vector<Float> &inputWeightVector = inputData->getVector<Float>(MS::WEIGHT_SPECTRUM);
	Vector<T> &outputVector = outputData->getVector<T>(MS::DATA);
	Bool accumulatorFlag = inputFlagVector(startInputPos);

	for (uInt sample_i=0;sample_i<width;sample_i++)
	{
		// Get input index
		inputPos = startInputPos + sample_i;

		Float& wt=inputWeightVector(inputPos);

		// true/true or false/false
		if (accumulatorFlag == inputFlagVector(inputPos))
		{
		        normalization += wt;
		        avg += inputVector(inputPos)*wt;
		}
		// true/false: Reset accumulation when accumulator switches from flagged to unflag
		else if ( (accumulatorFlag == true) and (inputFlagVector(inputPos) == false) )
		{
			accumulatorFlag = false;
			normalization = inputWeightVector(inputPos);
			avg = inputVector(inputPos)*inputWeightVector(inputPos);
		}
	}


	// Apply normalization factor
	if (normalization > 0)
	{
		avg /= normalization;
		outputVector(outputPos) = avg;
	}
	// If all weights are zero set accumulatorFlag to true
	else
	{
		accumulatorFlag = true;
		outputVector(outputPos) = 0; // If all weights are zero then the avg is 0 too
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
// LogicalANDKernel class
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> void LogicalANDKernel<T>::kernel(	DataCubeMap *inputData,
													DataCubeMap *outputData,
													uInt startInputPos,
													uInt outputPos,
													uInt width)
{
	Vector<Bool> &inputVector = inputData->getVector<Bool>(MS::FLAG);
	Vector<Bool> &outputVector = outputData->getVector<Bool>(MS::FLAG);

	Bool outputFlag = true;
	for (uInt sample_i=0;sample_i<width;sample_i++)
	{
		if (not inputVector(startInputPos+sample_i))
		{
			outputFlag = false;
			break;
		}
                }
	outputVector(outputPos) = outputFlag;

	return;
}

//////////////////////////////////////////////////////////////////////////
// ChannelAccumulationKernel class
//   (numerical accumulation, respecting flags)
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
template<class T> void ChannelAccumulationKernel<T>::kernel(	DataCubeMap *inputData,
																DataCubeMap *outputData,
																uInt startInputPos,
																uInt outputPos,
																uInt width)
{
	T acc = 0;
	uInt inputPos = 0;
	Vector<T> &inputVector = inputData->getVector<T>(MS::DATA);
	Vector<Bool> &inputFlagVector = inputData->getVector<Bool>(MS::FLAG);
	Vector<T> &outputVector = outputData->getVector<T>(MS::DATA);
	Bool accumulatorFlag = inputFlagVector(startInputPos);

	for (uInt sample_i=0;sample_i<width;sample_i++)
	{
		// Get input index
		inputPos = startInputPos + sample_i;

		// true/true or false/false
		if (accumulatorFlag == inputFlagVector(inputPos))
		{
			acc += inputVector(inputPos);
		}
		// true/false: Reset accumulation when accumulator switches from flagged to unflag
		else if ( (accumulatorFlag == true) and (inputFlagVector(inputPos) == false) )
		{
			accumulatorFlag = false;
			acc = inputVector(inputPos);
		}
	}


	outputVector(outputPos) = acc;

	return;
}

} //# NAMESPACE VI - END

} //# NAMESPACE CASA - END


