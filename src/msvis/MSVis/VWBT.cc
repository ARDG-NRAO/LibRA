//# VWBT.h: This file contains the implementation of the VWBT class.
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

#include <msvis/MSVis/VWBT.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

VWBT::VWBT(VisibilityIterator *visibilityIterator,
		   casa::async::Mutex * msAccessMutex,
           bool groupRows)
{
   visibilityIterator_p = visibilityIterator;
   msAccessMutex_p = msAccessMutex;
   groupRows_p = groupRows;

   terminationRequested_p = false;
   threadTerminated_p = false;
   writing_p = false;

   flagCube_p = NULL;

   initialize();
}

VWBT::~VWBT ()
{
	if (flagCube_p) delete flagCube_p;
}

void
VWBT::initialize()
{
	// Acquire lock
	msAccessMutex_p->acquirelock();

	// Initialize chunks
	visibilityIterator_p->originChunks();

	// Initialize first chunk
	if (groupRows_p)
	{
		Int nRowChunk = visibilityIterator_p->nRowChunk();
		visibilityIterator_p->setRowBlocking(nRowChunk);
	}
	visibilityIterator_p->origin();

	// Release lock
	msAccessMutex_p->unlock();
}

void *
VWBT::run ()
{
	while (!terminationRequested_p)
	{
		if (writing_p)
		{
			// Acquire lock
			msAccessMutex_p->acquirelock();

			// Write flag cube
			visibilityIterator_p->setFlag(*flagCube_p);
			writing_p = false;

			// Perform next iteration
			bool moreBuffers = next();

			// Release lock
			msAccessMutex_p->unlock();

			// Exit if there is no more data
			if (!moreBuffers) break;
		}
		else
		{
			sched_yield();
		}
	}

	threadTerminated_p = true;

	return NULL;
}

void
VWBT::start()
{
	casa::async::Thread::startThread();

	return;
}

void
VWBT::terminate ()
{
	terminationRequested_p = true;
	while (!threadTerminated_p)
	{
		sched_yield();
	}
	casa::async::Thread::terminate();

	return;
}

bool
VWBT::next()
{
	// Check if we have more vis buffers in the current chunk
	(*visibilityIterator_p)++;
	if (visibilityIterator_p->more())
	{
		return true;
	}
	// If there are not more buffers we go to the next chunk
	else
	{
		visibilityIterator_p->nextChunk();

		// If the new chunk is valid initialize it
		if (visibilityIterator_p->moreChunks())
		{
			// Check if all the rows have to be loaded in a single vis buffer
			if (groupRows_p)
			{
				Int nRowChunk = visibilityIterator_p->nRowChunk();
				visibilityIterator_p->setRowBlocking(nRowChunk);
			}
			// Initialize sub-chunks (vis buffers)
			visibilityIterator_p->origin();
			return true;
		}
		else
		{
			return false;
		}
	}
}

void
VWBT::setFlag(Cube<Bool> flagCube)
{
	// Wait until prev. flag cube is written
	while (writing_p)
	{
		sched_yield();
	}

	// Delete prev. flag cube
	if (flagCube_p) delete flagCube_p;

	// Prepare next flag cube to be written
	flagCube_p = new Cube<Bool>(flagCube);
	writing_p = true;

	return;
}


} //# NAMESPACE CASA - END


