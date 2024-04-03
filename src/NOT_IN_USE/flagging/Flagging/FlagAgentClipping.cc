//# FlagAgentClipping.cc: This file contains the implementation of the FlagAgentClipping class.
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

#include <flagging/Flagging/FlagAgentClipping.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

FlagAgentClipping::FlagAgentClipping(FlagDataHandler *dh, Record config, Bool writePrivateFlagCube, Bool flag):
						FlagAgentBase(dh,config,IN_ROWS,writePrivateFlagCube,flag)
{
	setAgentParameters(config);

	// Request loading polarization map to FlagDataHandler
	flagDataHandler_p->setMapPolarizations(true);
}

FlagAgentClipping::~FlagAgentClipping()
{
	// Compiler automagically calls FlagAgentBase::~FlagAgentBase()
}

void
FlagAgentClipping::setAgentParameters(Record config)
{
	logger_p->origin(LogOrigin(agentName_p,__FUNCTION__,WHERE));

	int exists;

	// First check basic configuration to see what modules are plugged in the check
	exists = config.fieldNumber ("clipzeros");
	if (exists >= 0)
	{
		if( config.type(exists) != TpBool )
		{
			throw( AipsError ( "Parameter 'clipzeros' must be of type 'bool'" ) );
		}

		clipzeros_p = config.asBool("clipzeros");
	}
	else
	{
		clipzeros_p = false;
	}

	*logger_p << logLevel_p << " clipzeros is " << clipzeros_p << LogIO::POST;


	exists = config.fieldNumber ("clipoutside");
	if (exists >= 0)
	{
		if( config.type(exists) != TpBool )
		{
			throw( AipsError ( "Parameter 'clipoutside' must be of type 'bool'" ) );
		}

		clipoutside_p = config.asBool("clipoutside");
	}
	else
	{
		clipoutside_p = true;
	}

	*logger_p << logLevel_p << " clipoutside is " << clipoutside_p << LogIO::POST;

	String datacol = "";
	weightcol_p = false;
	exists = config.fieldNumber ("datacolumn");
	if (exists >= 0)
	{
		datacol = config.asString("datacolumn");
	}

	exists = config.fieldNumber ("clipminmax");
	if (exists >= 0)
	{
		if( config.type(exists) != TpArrayDouble && config.type(exists) != TpArrayFloat && config.type(exists) != TpArrayInt )
		{
			throw( AipsError ( "Parameter 'clipminmax' must be of type 'array double' : [minval,maxval]" ) );
		}

		Array<Double> cliprange = config.asArrayDouble("clipminmax");
		if (cliprange.size() == 2)
		{
			Bool deleteIt = false;
			clipmin_p = cliprange.getStorage(deleteIt)[0];
			clipmax_p = cliprange.getStorage(deleteIt)[1];
			*logger_p << logLevel_p << " clipmin is " << clipmin_p << LogIO::POST;
			*logger_p << logLevel_p << " clipmax is " << clipmax_p << LogIO::POST;

			clipminmax_p = true;

			// for the moment, treat WEIGHT as if it was WEIGHT_SPECTRUM,
			// so the clipminmax given by the user is divided by the
			// number of channels inside preProcessBuffer()
			if (datacol.compare("WEIGHT") == 0)
			{
				weightcol_p = true;
				original_clipmin_p = clipmin_p;
				original_clipmax_p = clipmax_p;
			}
		}
		else
		{
			clipminmax_p = false;
			*logger_p << logLevel_p << " clipminmax range not provided" << LogIO::POST;
		}

	}
	else
	{
		clipminmax_p = false;
		*logger_p << logLevel_p << " clipminmax range not provided" << LogIO::POST;
	}


	// Then, point to the function covering for all the modules selected
	if (clipminmax_p)
	{
		if (clipoutside_p)
		{
			if (clipzeros_p)
			{
				checkVis_p = &FlagAgentClipping::checkVisForClipOutsideAndZeros;
				*logger_p << logLevel_p << " Clipping outside [" <<  clipmin_p << "," <<  clipmax_p << "], plus NaNs and zeros" << LogIO::POST;
			}
			else
			{
				checkVis_p = &FlagAgentClipping::checkVisForClipOutside;
				*logger_p << logLevel_p << " Clipping outside [" <<  clipmin_p << "," <<  clipmax_p << "], plus NaNs" << LogIO::POST;
			}
		}
		else
		{
			if (clipzeros_p)
			{
				checkVis_p = &FlagAgentClipping::checkVisForClipInsideAndZeros;
				*logger_p << logLevel_p << " Clipping inside [" <<  clipmin_p << "," <<  clipmax_p << "], plus NaNs and zeros" << LogIO::POST;
			}
			else
			{
				checkVis_p = &FlagAgentClipping::checkVisForClipInside;
				*logger_p << logLevel_p << " Clipping inside [" <<  clipmin_p << "," <<  clipmax_p << "], plus NaNs" << LogIO::POST;
			}
		}
	}
	else
	{
		if (clipzeros_p)
		{
			checkVis_p = &FlagAgentClipping::checkVisForNaNsAndZeros;
			*logger_p << logLevel_p << " Clipping range not provided, clipping NaNs and zeros" << LogIO::POST;
		}
		else
		{
			checkVis_p = &FlagAgentClipping::checkVisForNaNs;
			*logger_p << logLevel_p << " Clipping range not provided, clipping only NaNs" << LogIO::POST;
		}
	}

	return;
}

// jagonzal: With the new WEIGHT/SIGMA convention it is not longer
// necessary divide the clipping range between the number of channels
/*
void
FlagAgentClipping::preProcessBuffer(const vi::VisBuffer2 &visBuffer)
{

	logger_p->origin(LogOrigin(agentName_p,__FUNCTION__,WHERE));

	// Only done if datacolumn is WEIGHT
	if (weightcol_p)
	{
		Int nChannels;
		nChannels = visBuffer.nChannels();
		clipmin_p = original_clipmin_p/nChannels;
		clipmax_p = original_clipmax_p/nChannels;
	}
	*logger_p << LogIO::DEBUG1 << " clipmin is " << clipmin_p << LogIO::POST;
	*logger_p << LogIO::DEBUG1 << " clipmax is " << clipmax_p << LogIO::POST;

}
*/

bool
FlagAgentClipping::computeInRowFlags(const vi::VisBuffer2 &/*visBuffer*/, VisMapper &visibilities,
		FlagMapper &flags, uInt row)
{
	// Get flag cube size
	Float visExpression;
	Int nPols,nChannels,nTimesteps;
	visibilities.shape(nPols, nChannels, nTimesteps);

	for (uInt chan_i=0;chan_i<(uInt) nChannels;chan_i++)
	{
		for (uInt pol_i=0;pol_i<(uInt) nPols;pol_i++)
		{
			visExpression = visibilities(pol_i,chan_i,row);
			if ((*this.*checkVis_p)(visExpression))
			{
				flags.applyFlag(pol_i,chan_i,row);
				visBufferFlags_p += 1;
			}
		}
	}

	return false;
}

bool
FlagAgentClipping::checkVisForClipOutside(Float visExpression)
{
	if ((visExpression >  clipmax_p) or (visExpression <  clipmin_p))
	{
		return true;
	}
	else
	{
		return isNaN(visExpression);
	}
}

bool
FlagAgentClipping::checkVisForClipInside(Float visExpression)
{
	if ((visExpression <=  clipmax_p) and (visExpression >=  clipmin_p))
	{
		return true;
	}
	else
	{
		return isNaN(visExpression);
	}
}

bool
FlagAgentClipping::checkVisForClipOutsideAndZeros(Float visExpression)
{
	if ((visExpression >  clipmax_p) or (visExpression <  clipmin_p))
	{
		return true;
	}
	else
	{
		return isNaNOrZero(visExpression);
	}
}

bool
FlagAgentClipping::checkVisForClipInsideAndZeros(Float visExpression)
{
	if ((visExpression <=  clipmax_p) and (visExpression >=  clipmin_p))
	{
		return true;
	}
	else
	{
		return isNaNOrZero(visExpression);
	}
}

bool
FlagAgentClipping::checkVisForNaNs(Float visExpression)
{
	return isNaN(visExpression);
}

bool
FlagAgentClipping::checkVisForNaNsAndZeros(Float visExpression)
{
	return isNaNOrZero(visExpression);
}



} //# NAMESPACE CASA - END


