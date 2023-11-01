//# FreqAxisTVI.h: This file contains the implementation of the FreqAxisTVI class.
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

#include <mstransform/TVI/FreqAxisTVI.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

namespace vi { //# NAMESPACE VI - BEGIN

//////////////////////////////////////////////////////////////////////////
// FreqAxisTVI class
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
FreqAxisTVI::FreqAxisTVI(	ViImplementation2 * inputVii) :
							TransformingVi2 (inputVii)
{
	initialize();

	// Initialize attached VisBuffer
	setVisBuffer(createAttachedVisBuffer (VbRekeyable));

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
FreqAxisTVI::~FreqAxisTVI()
{
	// The parent class destructor (~TransformingVi2) deletes the inner
	// ViImplementation2 object. However if it might have been already
	// deleted at the top level context
	// 2/8/2016 (jagonzal): As per request from George M. (via CAS-8220)
	// I allow TransformingVi2 destructor to delete its inner input VI;
	// This relies on the application layer that produces the inner VI not
	// deleting it which can be guaranteed when using the Factory pattern.
	// inputVii_p = NULL;

	return;
}


// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void FreqAxisTVI::initialize()
{

    if (inputVii_p == nullptr)
        return;

    //Create a map with the input SPWs and their channels.
    //Note that this access directly the information provided by the previous
    //layer and it is no longer based on the original MS selection, which
    //for this particular layer might not make any sense (see CAS-9679).
    formChanMap();
    return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void FreqAxisTVI::formChanMap()
{
    // This triggers realization of the channel selection
    inputVii_p->originChunks();

    // Refresh map
    spwInpChanIdxMap_p.clear();

    for (Int ispw = 0; ispw < inputVii_p->nSpectralWindows(); ++ispw)
    {
        // TBD trap unselected spws with a continue

        Vector<Int> chansV;
        chansV.reference(inputVii_p->getChannels(0.0, -1, ispw, 0));

        Int nChan = chansV.nelements();
        if (nChan > 0)
        {
            spwInpChanIdxMap_p[ispw].clear(); // creates ispw's map
            for (Int ich = 0; ich < nChan; ++ich)
            {
                spwInpChanIdxMap_p[ispw].push_back(chansV[ich]); // accum into map
            }
        }
    } // ispw

    return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void FreqAxisTVI::origin()
{
    // Drive underlying ViImplementation2
    getVii()->origin();

    configureShapes();

    // Synchronize own VisBuffer
    configureNewSubchunk();

    return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void FreqAxisTVI::next()
{
    // Drive underlying ViImplementation2
    getVii()->next();

    configureShapes();

    // Synchronize own VisBuffer
    configureNewSubchunk();

    return;
}

void FreqAxisTVI::configureShapes()
{
    Vector<Int> spws;
    spectralWindows(spws);
    Vector<Int> channels = getChannels (0, 0, spws (0) , msId());
    nChannPerShape_ = casacore::Vector<casacore::Int> (1, channels.nelements());
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Bool FreqAxisTVI::existsColumn (VisBufferComponent2 id) const
{

	Bool ret;
	switch (id)
	{
		case VisBufferComponent2::WeightSpectrum:
		{
			ret = true;
			break;
		}
		case VisBufferComponent2::SigmaSpectrum:
		{
			ret = true;
			break;
		}
		default:
		{
			ret = getVii()->existsColumn(id);
			break;
		}
	}

	return ret;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Vector<Int> FreqAxisTVI::getChannels (Double,Int,Int spectralWindowId,Int) const
{
	Vector<Int> ret(spwOutChanNumMap_p[spectralWindowId]);

	for (uInt chanIdx = 0; chanIdx<spwOutChanNumMap_p[spectralWindowId];chanIdx++)
	{
		ret(chanIdx) = chanIdx;
	}

	return ret;
}

const casacore::Vector<casacore::Int>&
FreqAxisTVI::nChannelsPerShape () const
{
    return nChannPerShape_;
}


// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void FreqAxisTVI::writeFlagRow (const Vector<Bool> & flag)
{
	getVii()->writeFlagRow(flag);
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void FreqAxisTVI::flagRow (Vector<Bool> & flagRow) const
{
	// Get flagCube from own VisBuffer
	const Cube<Bool> &flagCube = getVisBuffer()->flagCube();

	// Calculate output flagRow
	accumulateFlagCube(flagCube,flagRow);
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void FreqAxisTVI::weight (Matrix<Float> & weight) const
{
	if (weightSpectrumExists()) // Defined by each derived class or inner TVI
	{
		// Get flags and weightSpectrum from own VisBuffer
		const Cube<Bool> &flags = getVisBuffer()->flagCube();
		const Cube<Float> &weightSpectrum = getVisBuffer()->weightSpectrum();

		// Calculate output weight
		accumulateWeightCube(weightSpectrum,flags,weight);
	}
	else
	{
		getVii()->weight (weight);
	}

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void FreqAxisTVI::sigma (Matrix<Float> & sigma) const
{
	if (sigmaSpectrumExists())
	{
		// Get flags and sigmaSpectrum from own VisBuffer
		const Cube<Bool> &flags = getVisBuffer()->flagCube();
		const Cube<Float> &sigmaSpectrum = getVisBuffer()->sigmaSpectrum();

		// Calculate output sigma
		accumulateWeightCube(sigmaSpectrum,flags,sigma);
	}
	else
	{
		getVii()->sigma (sigma);
	}

	return;
}

} //# NAMESPACE VI - END

} //# NAMESPACE CASA - END


