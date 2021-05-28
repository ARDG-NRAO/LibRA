//# MSTransformIteratorFactory.h: This file contains the implementation of the MSTransformIteratorFactory class.
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
#include <ctime>
#include <mstransform/MSTransform/MSTransformIteratorFactory.h>
#include <casa/Utilities/CountedPtr.h>
using namespace casacore;
namespace casa
{

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
MSTransformIteratorFactory::MSTransformIteratorFactory(Record &configuration)
{
	tmpMSFileName_p = String("");
	setConfiguration(configuration);
	eligibleSubTables_p = MrsEligibility::defaultEligible();

	initializeManager();

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
MSTransformIteratorFactory::MSTransformIteratorFactory(Record &configuration, MrsEligibility &eligibleSubTables)
{
	setConfiguration(configuration);
	eligibleSubTables_p = eligibleSubTables;

	initializeManager();

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void MSTransformIteratorFactory::initializeManager()
{
	// Initialize manager to unpack parameters
	manager_p = std::shared_ptr<MSTransformManager>(new MSTransformManager(configuration_p));

	// Open manager to create selected inputMS
	manager_p->open();

	// Need to call setup in order to determine the output channel structure
	manager_p->setup();

	// Enable memory resident SubTables in transformed MS
	manager_p->getOutputMs()->setMemoryResidentSubtables(eligibleSubTables_p);

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
MSTransformIteratorFactory::~MSTransformIteratorFactory()
{
	if (manager_p)
	{
		// ~MSTransformManager => ~VisibilityIterator2 => ~VisibilityIteratorImpl2
		// ~MSTransformManager => MSTransformManager.close()
		manager_p.reset();

		Table::relinquishAutoLocks(true);
	}

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
std::vector<IPosition> MSTransformIteratorFactory::getVisBufferStructure()
{
	// Initialize logger
	LogIO logger;

	// Get MS filename as we may need to remove the tmp file from the Factory class
	tmpMSFileName_p = manager_p->getOutputMsName();

	// Generate tmp iterator
	vi::VisibilityIterator2 *visIter = new vi::VisibilityIterator2(	*(manager_p->selectedInputMs_p),
																	vi::SortColumns(manager_p->sortColumns_p, false),
																	false,NULL,manager_p->timeBin_p);

	// Set channel selection
	if (manager_p->channelSelector_p != NULL)
	{
		visIter->setFrequencySelection(*(manager_p->channelSelector_p));
	}

	// Prepare buffer-related objects
	std::vector<IPosition> bufferShapeVector;
	vi::VisBuffer2 *visBuffer = visIter->getVisBuffer();

	// Start timer
	logger 	<< LogIO::NORMAL << LogOrigin("MSTransformIteratorFactory", __FUNCTION__)
			<< "Start pre-calculation of buffer structure " << LogIO::POST;
	time_t startTime = std::time(NULL);

	// Count number of chunks
	visIter->originChunks();
	while (visIter->moreChunks())
	{
		visIter->origin();

		// In case of time average all buffers in the chunk are averaged together, so we only (should) have 1 buffer
		if (manager_p->timeAverage_p)
		{
			if (visIter->more())
			{
				// Call buffer transformations to properly define nRows
				manager_p->setupBufferTransformations(visBuffer);

				// Get transformed shape
				IPosition firstBufferShape = manager_p->getTransformedShape(visBuffer);

				// Find max number of baselines in this chunk
				rownr_t maxnRows = firstBufferShape(2);
				while (visIter->more())
				{
					if (visBuffer->nRows() > maxnRows)
					    maxnRows = visBuffer->nRows();
					visIter->next();
				}

				// Override number of rows in first buffer shape
				firstBufferShape(2) = maxnRows;

				// Add shape to vector
				bufferShapeVector.push_back(firstBufferShape);
			}
		}
		// In the general case we have to go through the buffers
		else
		{
			while (visIter->more())
			{
				// Call buffer transformations to properly define nRows
				manager_p->setupBufferTransformations(visBuffer);

				// Get shape
				bufferShapeVector.push_back(manager_p->getTransformedShape(visBuffer));

				// Next buffer
				visIter->next();
			}
		}

		visIter->nextChunk();
	}

	// Stop timer
	time_t endTime = std::time(NULL);
	logger 	<< LogIO::NORMAL << LogOrigin("MSTransformIteratorFactory", __FUNCTION__)
			<< "End pre-calculation of buffer structure, time: " << endTime-startTime << LogIO::POST;

	// Delete tmp iter
	delete visIter;


	return bufferShapeVector;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void MSTransformIteratorFactory::setConfiguration(Record &configuration)
{
	configuration_p = configuration;
	configuration_p.define("buffermode",true);
	configuration_p.define("outputms",File::newUniqueName("").absoluteName());
	return;
}


// -----------------------------------------------------------------------
// InpVi is the VisibilityIterator in which the implementation provided is to be plugged
// -----------------------------------------------------------------------

vi::ViImplementation2 * MSTransformIteratorFactory::createVi() const
{
	// Create output VisibilityIterator
	MSTransformIterator *outputVI = new MSTransformIterator(manager_p->getVisIter()->getImpl(),manager_p);

	return outputVI;
}

} //# NAMESPACE CASA - END


