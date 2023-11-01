//# FlagAgentQuack.cc: This file contains the implementation of the FlagAgentQuack class.
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

#include <flagging/Flagging/FlagAgentQuack.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

FlagAgentQuack::FlagAgentQuack(FlagDataHandler *dh, Record config, Bool writePrivateFlagCube, Bool flag):
		FlagAgentBase(dh,config,ROWS,writePrivateFlagCube,flag)
{
	setAgentParameters(config);

	// Request loading antenna pointing map to FlagDataHandler
	flagDataHandler_p->setScanStartStopMap(true);
	if (quackincrement_p) flagDataHandler_p->setScanStartStopFlaggedMap(true);
}

FlagAgentQuack::~FlagAgentQuack()
{
	// Compiler automagically calls FlagAgentBase::~FlagAgentBase()
}

void
FlagAgentQuack::setAgentParameters(Record config)
{
	logger_p->origin(LogOrigin(agentName_p,__FUNCTION__,WHERE));
	int exists;

	exists = config.fieldNumber ("quackinterval");
	if (exists >= 0)
	{
	        if( config.type(exists) != TpDouble && config.type(exists) != TpFloat && config.type(exists) != TpInt )
	        {
			 throw( AipsError ( "Parameter 'quackinterval' must be of type 'double'" ) );
	        }
		
		quackinterval_p = config.asDouble("quackinterval");

		if (quackinterval_p <= 0)
		{
			throw( AipsError ( "Parameter 'quackinterval' must be greater than zero." ) );
		}
	}
	else
	{
		quackinterval_p = 1.0;
	}

	*logger_p << logLevel_p << " quackinterval is " << quackinterval_p << LogIO::POST;

	exists = config.fieldNumber ("quackmode");
	String quackmode;
	if (exists >= 0)
	{
	        if( config.type(exists) != TpString )
	        {
			 throw( AipsError ( "Parameter 'quackmode' must be of type 'string'" ) );
	        }
		
		quackmode = config.asString("quackmode");
		if (quackmode == "beg")
		{
			quackmode_p = BEGINNING_OF_SCAN;
		}
		else if (quackmode == "endb")
		{
			quackmode_p = END_OF_SCAN;
		}
		else if (quackmode == "end")
		{
			quackmode_p = ALL_BUT_END_OF_SCAN;
		}
		else if (quackmode == "tail")
		{
			quackmode_p = ALL_BUT_BEGINNING_OF_SCAN;
		}
		else
		{
			 throw( AipsError ( "Unsupported quack mode: " + quackmode + ". Supported modes: beg,endb,end,tail (following AIPS convention)" ) );
		}
	}
	else
	{
		quackmode_p = BEGINNING_OF_SCAN;
		quackmode = "beg";
	}

	*logger_p << logLevel_p << " quackmode is " << quackmode << LogIO::POST;

	exists = config.fieldNumber ("quackincrement");
	if (exists >= 0)
	{
	        if( config.type(exists) != TpBool )
	        {
			 throw( AipsError ( "Parameter 'quackincrement' must be of type 'bool'" ) );
	        }
		
		quackincrement_p = config.asBool("quackincrement");
	}
	else
	{
		quackincrement_p = false;
	}
	*logger_p << logLevel_p << " quackincrement is " << quackincrement_p << LogIO::POST;


	return;
}

bool
FlagAgentQuack::computeRowFlags(const vi::VisBuffer2 &visBuffer, FlagMapper &/*flags*/, uInt row)
{
	logger_p->origin(LogOrigin(agentName_p,__FUNCTION__,WHERE));

	// Get scan for each particular row to cover for the "combine scans" case
	Int scan = visBuffer.scan()[row];

	// First of all check if this scan is in the scan start/stop map
	if ( (*flagDataHandler_p->getMapScanStartStop()).find(scan) == (*flagDataHandler_p->getMapScanStartStop()).end())
	{
/*		*logger_p << LogIO::WARN << " start/stop time for scan "
				<< scan << " not found" << LogIO::POST;*/
		return false;
	}

	// If the scan is available in the map, then we proceed with the algorithm
	Double scan_start = (*flagDataHandler_p->getMapScanStartStop())[scan].at(0);
	Double scan_stop = (*flagDataHandler_p->getMapScanStartStop())[scan].at(1);
	Double row_time = visBuffer.time()[row];
	bool flagRow = false;

	switch (quackmode_p)
	{
		case BEGINNING_OF_SCAN:
		{
			if (row_time <= (scan_start + quackinterval_p)) flagRow = true;
			break;
		}
		case END_OF_SCAN:
		{
			if (row_time >= (scan_stop - quackinterval_p)) flagRow = true;
			break;
		}
		case ALL_BUT_BEGINNING_OF_SCAN:
		{
			if (row_time > (scan_start + quackinterval_p)) flagRow = true;
			break;
		}
		case ALL_BUT_END_OF_SCAN:
		{
			if (row_time < (scan_stop - quackinterval_p)) flagRow = true;
			break;
		}
	}

	return flagRow;
}

} //# NAMESPACE CASA - END


