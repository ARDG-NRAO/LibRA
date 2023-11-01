//# SPWCombinationTVI.h: This file contains the implementation of the SPWCombinationTVI class.
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

#include <iomanip>
#include <mstransform/TVI/SPWCombinationTVI.h>
#include <casacore/ms/MeasurementSets/MSSpWindowColumns.h>

namespace casa { //# NAMESPACE CASA - BEGIN

namespace vi { //# NAMESPACE VI - BEGIN

//////////////////////////////////////////////////////////////////////////
// SPWCombinationTVI class
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
SPWCombinationTVI::SPWCombinationTVI(ViImplementation2 * inputVii) :
  FreqAxisTVI (inputVii)
{
std::cout << std::setprecision(10) << std::endl;
    initialize();

    return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void SPWCombinationTVI::initialize()
{
    freqWidthChan_p = checkEqualWidth();

    spwInpChanOutFreqMap_p.clear();
    
    // Where do the output SPWs indexes start. This assumes that
    // this TVI is not doing any reindexing in the output it produces.
    outSpwStartIdx_p = inputVii_p->nSpectralWindows();

    // TODO: If the input has not been "reindexed" there can be 
    // spwIds, polIds, ddIds which are not referenced in the main table
    // The proper way to do it is to filter all the input SPWs to those
    // ones for which there is real data.
    auto nPolIds = inputVii_p->nPolarizationIds();
    //There are as many output SPWs as polarizations.
std::cout << " npol " << nPolIds << std::endl;
    for(ssize_t polId = 0; polId < nPolIds; polId++)
    {
        int outSpwId = polId + outSpwStartIdx_p;
        std::vector<double> thisOutSpwFreqs;
        std::vector<int> thisOutSpwChann;
        spwInpChanOutFreqMap_p[outSpwId].clear();
        // TODO: Filter SPWs that do not appear together with this polarization
        // (they are not together in the same DDId)
        for(auto inpSpw : spwInpChanIdxMap_p)
        {
            for(auto channel : inpSpw.second)
            {
                double channFreq = getChannelNominalFreq(inpSpw.first, channel);
                spwInpChanOutFreqMap_p[outSpwId][inpSpw.first].push_back(channFreq);
                thisOutSpwFreqs.push_back(channFreq);
            }
        }
        spwOutFirstFreq_p[outSpwId] = *std::min_element(thisOutSpwFreqs.begin(), thisOutSpwFreqs.end());
        for(auto inpSpw : spwInpChanIdxMap_p)
        {
            for(auto freq : spwInpChanOutFreqMap_p[outSpwId][inpSpw.first])
            {
                //TODO: Check all the frequencies fall more or less in the same bins. With a bin tolerance (0.01% of the bin?)
                //TODO: Check that no frequencies overlap.
                spwInpChanOutMap_p[outSpwId][inpSpw.first].push_back(std::floor((freq - spwOutFirstFreq_p[outSpwId]) / freqWidthChan_p + 0.1));
                thisOutSpwChann.push_back(std::floor((freq - spwOutFirstFreq_p[outSpwId]) / freqWidthChan_p + 0.1));
            }
        }
        spwOutChanNumMap_p[outSpwId] = *std::max_element(thisOutSpwChann.begin(), thisOutSpwChann.end()) + 1;
    }
            
    
    return;
}

double SPWCombinationTVI::getChannelNominalFreq(size_t inpSpw, size_t channel) const
{
    //TODO: refactor with next one
    auto& inputSPWSubtablecols = inputVii_p->spectralWindowSubtablecols();

    auto& channelNominalFreqs = inputSPWSubtablecols.chanFreq();
//std::cout << " channelNominalFreqs " << channelNominalFreqs.getColumn() << std::endl;

    return channelNominalFreqs(inpSpw)(casacore::IPosition(1, channel));
}

double SPWCombinationTVI::checkEqualWidth() const
{
    double channelWidth = -1;
    auto& inputSPWSubtablecols = inputVii_p->spectralWindowSubtablecols();

    auto& channelWidths = inputSPWSubtablecols.chanWidth();

    for(auto& inpSpw : spwInpChanIdxMap_p)
    {
        if (channelWidth == -1)
            channelWidth = channelWidths(inpSpw.first)(casacore::IPosition(1, 0));
        auto spwChannelWidths = channelWidths(inpSpw.first); //This is a copy
        if(!std::all_of(spwChannelWidths.begin(), spwChannelWidths.end(), [&](double width){return width == channelWidth;}))
            throw casacore::AipsError("SPWs to combine have different widths");
    }
    return channelWidth;
}

casacore::Vector<double> SPWCombinationTVI::getFrequencies(double time, int frameOfReference,
                                                           int spectralWindowId, int msId) const
{
    if(spectralWindowId < outSpwStartIdx_p ||
        spectralWindowId > int(outSpwStartIdx_p + spwInpChanOutFreqMap_p.size()))
        throw casacore::AipsError("SPWId out of valid range");

    auto inputSpwForThisOutputSpw = spwInpChanOutMap_p.at(spectralWindowId);
    std::vector<double> freqs(spwOutChanNumMap_p.at(spectralWindowId));
    for(auto inputSpw : inputSpwForThisOutputSpw)
    {
        auto innerFreqs = getVii()->getFrequencies(time, frameOfReference, inputSpw.first, msId);
        auto outChanThisInputSpw = spwInpChanOutMap_p.at(spectralWindowId).at(inputSpw.first)[0];
        std::copy(innerFreqs.begin(), innerFreqs.end(), freqs.begin() + outChanThisInputSpw);
    }
    return casacore::Vector<double>(freqs);
}

void SPWCombinationTVI::origin()
{
    // Drive underlying ViImplementation2
    getVii()->origin();

    // Set structure parameters for this subchunk iteration
    setUpCurrentSubchunkStructure();

    // Synchronize own VisBuffer
    configureNewSubchunk();

    return;
}

void SPWCombinationTVI::next()
{
    // Drive underlying ViImplementation2
    getVii()->next();

    // Set structure parameters for this subchunk iteration
    setUpCurrentSubchunkStructure();

    // Synchronize own VisBuffer
    configureNewSubchunk();

    return;
}

void SPWCombinationTVI::setUpCurrentSubchunkStructure()
{
    auto& innerNRowsPerShape = getVii()->nRowsPerShape();
    getVii()->polarizationIds(currentSubchunkInnerPolIds_p);
    std::set<casacore::Int> uniquePolIDs;
    std::copy(currentSubchunkInnerPolIds_p.begin(),currentSubchunkInnerPolIds_p.end(),
              std::inserter(uniquePolIDs, uniquePolIDs.end()));

    ssize_t thisSubchunkNPolIds = uniquePolIDs.size();
    // This VisBuffer contains one single timestamp with "all" DDIds.
    // After SPW combination then number of rows is equal to the number of distinct DDIds,
    // i.e., the number of polarizations, which is also the number of distinct shapes
    nRowsPerShape_p.resize(thisSubchunkNPolIds);
    nChannelsPerShape_p.resize(thisSubchunkNPolIds);


    currentSubchunkSpwIds_p.resize(thisSubchunkNPolIds);
    std::iota(currentSubchunkSpwIds_p.begin(), currentSubchunkSpwIds_p.end(), outSpwStartIdx_p);

    getVii()->spectralWindows(currentSubchunkInnerSpwIds_p);

    // Set up the channels for shape
    size_t iShape=0;
    for(auto outSpw : spwOutChanNumMap_p)
    {
        nChannelsPerShape_p[iShape] = outSpw.second;
        iShape++;
    }

    nRowsPerShape_p = thisSubchunkNPolIds;

    //TODO: Check all the NRows are the same.
    //TODO: For several polarizations (nShapes)
}

casacore::rownr_t SPWCombinationTVI::nShapes() const
{
    return nRowsPerShape_p.size();
}

const casacore::Vector<casacore::rownr_t>& SPWCombinationTVI::nRowsPerShape() const
{
    return nRowsPerShape_p;
}

const casacore::Vector<casacore::Int>& SPWCombinationTVI::nChannelsPerShape() const
{
    return nChannelsPerShape_p;
}

void SPWCombinationTVI::spectralWindows(casacore::Vector<casacore::Int>& spws) const
{
    spws = currentSubchunkSpwIds_p;
}

void SPWCombinationTVI::antenna1(casacore::Vector<casacore::Int> & ant1) const
{
    // Resize vector
    ant1.resize(nShapes());

    // It is assumed the values are constant in this VisBuffer
    ant1 = getVii()->getVisBuffer()->antenna1()(0);
}

void SPWCombinationTVI::antenna2(casacore::Vector<casacore::Int> & ant2) const
{
    // Resize vector
    ant2.resize(nShapes());

    // It is assumed the values are constant in this VisBuffer
    ant2 = getVii()->getVisBuffer()->antenna2()(0);
}

void SPWCombinationTVI::arrayIds (casacore::Vector<casacore::Int>&arrayIds) const
{
    // Resize vector
    arrayIds.resize(nShapes());

    // It is assumed the values are constant in this VisBuffer
    arrayIds = getVii()->getVisBuffer()->arrayId()(0);
}

void SPWCombinationTVI::exposure(casacore::Vector<double> & expo) const
{
    // Resize vector
    expo.resize(nShapes());

    // It is assumed the values are constant in this VisBuffer
    expo = getVii()->getVisBuffer()->exposure()(0);
}

void SPWCombinationTVI::feed1(casacore::Vector<casacore::Int> & fd1) const
{
    // Resize vector
    fd1.resize(nShapes());

    // It is assumed the values are constant in this VisBuffer
    fd1 = getVii()->getVisBuffer()->feed1()(0);
}

void SPWCombinationTVI::feed2(casacore::Vector<casacore::Int> & fd2) const
{
    // Resize vector
    fd2.resize(nShapes());

    // It is assumed the values are constant in this VisBuffer
    fd2 = getVii()->getVisBuffer()->feed2()(0);
}

void SPWCombinationTVI::fieldIds (casacore::Vector<casacore::Int>& fieldId) const
{
    // Resize vector
    fieldId.resize(nShapes());

    // It is assumed the values are constant in this VisBuffer
    fieldId = getVii()->getVisBuffer()->fieldId()(0);
}

void SPWCombinationTVI::observationId (casacore::Vector<casacore::Int> & obsids) const
{
    // Resize vector
    obsids.resize(nShapes());

    // It is assumed the values are constant in this VisBuffer
    obsids = getVii()->getVisBuffer()->observationId()(0);
}

void SPWCombinationTVI::processorId (casacore::Vector<casacore::Int> & procids) const
{
    // Resize vector
    procids.resize(nShapes());

    // It is assumed the values are constant in this VisBuffer
    procids = getVii()->getVisBuffer()->processorId()(0);
}

void SPWCombinationTVI::stateId (casacore::Vector<casacore::Int> & stateids) const
{
    // Resize vector
    stateids.resize(nShapes());

    // It is assumed the values are constant in this VisBuffer
    stateids = getVii()->getVisBuffer()->stateId()(0);
}

void SPWCombinationTVI::time(casacore::Vector<double> & t) const
{
    // Resize vector
    t.resize(nShapes());

    // It is assumed the values are constant in this VisBuffer
    t = getVii()->getVisBuffer()->time()(0);
}

void SPWCombinationTVI::timeCentroid(casacore::Vector<double> & t) const
{
    // Resize vector
    t.resize(nShapes());

    // It is assumed the values are constant in this VisBuffer
    t = getVii()->getVisBuffer()->timeCentroid()(0);
}

void SPWCombinationTVI::timeInterval(casacore::Vector<double> & t) const
{
    // Resize vector
    t.resize(nShapes());

    // It is assumed the values are constant in this VisBuffer
    t = getVii()->getVisBuffer()->timeInterval()(0);
}

void SPWCombinationTVI::flag(casacore::Cube<casacore::Bool>& flag) const
{
    auto& flagCubes = getVisBuffer()->flagCubes();
    flag = flagCubes[0];
}

void SPWCombinationTVI::flag(casacore::Vector<casacore::Cube<casacore::Bool>>& flagCubes) const
{
    // Get input VisBuffer and flag cubes
    VisBuffer2 *vb = getVii()->getVisBuffer();
    auto& innerFlagCubes = vb->flagCubes();
    transformCubesVector(innerFlagCubes, flagCubes);
}

void SPWCombinationTVI::floatData(casacore::Cube<casacore::Float> & vis) const
{
    auto& visCubes = getVisBuffer()->visCubesFloat();
    vis = visCubes[0];
}

void SPWCombinationTVI::floatData(casacore::Vector<casacore::Cube<casacore::Float>> & vis) const
{
    // Get input VisBuffer and visibility float cubes
    VisBuffer2 *vb = getVii()->getVisBuffer();
    auto& innerVisCubes = vb->visCubesFloat();
    transformCubesVector(innerVisCubes, vis);
}

void SPWCombinationTVI::visibilityObserved(casacore::Cube<casacore::Complex> & vis) const
{
    auto& visCubes = getVisBuffer()->visCubes();
    vis = visCubes[0];
}

void SPWCombinationTVI::visibilityObserved(casacore::Vector<casacore::Cube<casacore::Complex>> & vis) const
{
    // Get input VisBuffer and visibility observed cubes
    VisBuffer2 *vb = getVii()->getVisBuffer();
    auto& innerVisCubes = vb->visCubes();
    transformCubesVector(innerVisCubes, vis);
}

void SPWCombinationTVI::visibilityCorrected(casacore::Cube<casacore::Complex> & vis) const
{
    auto& visCubes = getVisBuffer()->visCubesCorrected();
    vis = visCubes[0];
}

void SPWCombinationTVI::visibilityCorrected(casacore::Vector<casacore::Cube<casacore::Complex>> & vis) const
{
    // Get input VisBuffer and visibility corrected cubes
    VisBuffer2 *vb = getVii()->getVisBuffer();
    auto& innerVisCubes = vb->visCubesCorrected();
    transformCubesVector(innerVisCubes, vis);
}

void SPWCombinationTVI::visibilityModel(casacore::Cube<casacore::Complex> & vis) const
{
    auto& visCubes = getVisBuffer()->visCubesModel();
    vis = visCubes[0];
}

void SPWCombinationTVI::visibilityModel(casacore::Vector<casacore::Cube<casacore::Complex>> & vis) const
{
    // Get input VisBuffer and visibility model cubes
    VisBuffer2 *vb = getVii()->getVisBuffer();
    auto& innerVisCubes = vb->visCubesModel();
    transformCubesVector(innerVisCubes, vis);
}

void SPWCombinationTVI::weight(casacore::Matrix<casacore::Float> &weight) const
{
    auto& weightCubes = getVisBuffer()->weights();
    weight = weightCubes[0];
}

void SPWCombinationTVI::weight(casacore::Vector<casacore::Matrix<casacore::Float>> &weight) const
{
    // Get input VisBuffer and weight cubes
    VisBuffer2 *vb = getVii()->getVisBuffer();
    auto& innerWeight = vb->weights();
    transformMatricesVector(innerWeight, weight);
}

void SPWCombinationTVI::weightSpectrum(casacore::Cube<casacore::Float> &weightSp) const
{
    auto& weightSpCubes = getVisBuffer()->weightSpectra();
    weightSp = weightSpCubes[0];
}

void SPWCombinationTVI::weightSpectrum(casacore::Vector<casacore::Cube<casacore::Float>> &weightSp) const
{
    // Get input VisBuffer and weight Sp cubes
    VisBuffer2 *vb = getVii()->getVisBuffer();
    auto& innerWeightSp = vb->weightSpectra();
    transformCubesVector(innerWeightSp, weightSp);
}

void SPWCombinationTVI::sigma(casacore::Matrix<casacore::Float> &sigma) const
{
    auto& sigmaCubes = getVisBuffer()->sigmas();
    sigma = sigmaCubes[0];
}

void SPWCombinationTVI::sigma(casacore::Vector<casacore::Matrix<casacore::Float>> &sigma) const
{
    // Get input VisBuffer and sigma cubes
    VisBuffer2 *vb = getVii()->getVisBuffer();
    auto& innerSigmaMat = vb->sigmas();
    transformMatricesVector(innerSigmaMat, sigma);
}

void SPWCombinationTVI::sigmaSpectrum(casacore::Cube<casacore::Float> &sigmaSp) const
{
    auto& sigmaSpCubes = getVisBuffer()->sigmaSpectra();
    sigmaSp = sigmaSpCubes[0];
}

void SPWCombinationTVI::sigmaSpectrum(casacore::Vector<casacore::Cube<casacore::Float>> &sigmaSp) const
{
    // Get input VisBuffer and sigma Sp cubes
    VisBuffer2 *vb = getVii()->getVisBuffer();
    auto& innerSigmaSp = vb->sigmaSpectra();
    transformCubesVector(innerSigmaSp, sigmaSp);
}

SPWCombinationTVIFactory::SPWCombinationTVIFactory (ViImplementation2 *inputVii)
 : inputVii_p(inputVii)
{
}

SPWCombinationTVIFactory::~SPWCombinationTVIFactory()
{
}

ViImplementation2 * SPWCombinationTVIFactory::createVi() const
{
    ViImplementation2* vii = new SPWCombinationTVI(inputVii_p);
    return vii;
}

SPWCombinationTVILayerFactory::SPWCombinationTVILayerFactory()
{
}

SPWCombinationTVILayerFactory::~SPWCombinationTVILayerFactory()
{
}

ViImplementation2*
SPWCombinationTVILayerFactory::createInstance(ViImplementation2* vii0) const
{
    // Make the SPWCombinationTVI, using supplied ViImplementation2, and return it
    ViImplementation2 *vii = new SPWCombinationTVI(vii0);
    return vii;
}

} //# NAMESPACE VI - END

} //# NAMESPACE CASA - END
