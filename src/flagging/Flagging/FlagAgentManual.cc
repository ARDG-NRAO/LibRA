//# FlagAgentManual.cc: This file contains the implementation of the FlagAgentManual class.
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

#include <flagging/Flagging/FlagAgentManual.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

FlagAgentManual::FlagAgentManual(FlagDataHandler *dh, Record config, Bool writePrivateFlagCube, Bool flag):
		FlagAgentBase(dh,config,ROWS,writePrivateFlagCube,flag)
{
	initialize(config);
}

FlagAgentManual::~FlagAgentManual()
{
	// Compiler automagically calls FlagAgentBase::~FlagAgentBase()
}

void
FlagAgentManual::initialize(Record config)
{

	int exists;
	bool autocorr = false;

	exists = config.fieldNumber ("autocorr");
	if (exists >= 0)
		autocorr = config.asBool("autocorr");

	// For the auto-correlation flagging
	if (flagDataHandler_p->tableTye_p == FlagDataHandler::MEASUREMENT_SET
			and autocorr){
		flagDataHandler_p->preLoadColumn(VisBufferComponent2::ProcessorId);
		flagDataHandler_p->loadProcessorTable_p = true;
	}

}

bool
FlagAgentManual::computeRowFlags(const vi::VisBuffer2 &/*visBuffer*/, FlagMapper &/*flags*/, uInt /*row*/)
{
	return true;

}


} //# NAMESPACE CASA - END


