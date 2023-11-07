//# FlagAgenExtension.cc: This file contains the implementation of the FlagAgenExtension.cc class.
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

#include <flagging/Flagging/FlagAgentExtension.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

FlagAgentExtension::FlagAgentExtension(FlagDataHandler *dh, Record config, Bool writePrivateFlagCube):
		FlagAgentBase(dh,config,ANTENNA_PAIRS_FLAGS,writePrivateFlagCube)
{
	setAgentParameters(config);

	// Request loading polarization map to FlagDataHandler
	flagDataHandler_p->setMapPolarizations(true);
}

FlagAgentExtension::~FlagAgentExtension()
{
	// Compiler automagically calls FlagAgentBase::~FlagAgentBase()
}

void FlagAgentExtension::setAgentParameters(Record config)
{
	logger_p->origin(LogOrigin(agentName_p,__FUNCTION__,WHERE));
	int exists;

	exists = config.fieldNumber ("extendpols");
	if (exists >= 0)
	{
	        if( config.type(exists) != TpBool )
	        {
			 throw( AipsError ( "Parameter 'extendpols' must be of type 'bool'" ) );
	        }
		
		extendpols_p = config.asBool("extendpols");
	}
	else
	{
		extendpols_p = true;
	}

	*logger_p << logLevel_p << " extendpols is " << extendpols_p << LogIO::POST;


	exists = config.fieldNumber ("growtime");
	if (exists >= 0)
	{
	        if( config.type(exists) != TpDouble && config.type(exists) != TpFloat  && config.type(exists) != TpInt)
	        {
			 throw( AipsError ( "Parameter 'growtime' must be of type 'double'" ) );
	        }
		
		growtime_p = config.asDouble("growtime");

		if( growtime_p < 0.0 || growtime_p > 100.0 )
	        {
		         throw( AipsError ( "Unsupported value for growtime:" + String::toString(growtime_p) + ". Allowed range is 0.0 through 100.0" ) );
	        }
	}
	else
	{
		growtime_p = 50.0;
	}

	*logger_p << logLevel_p << " growtime is " << growtime_p << LogIO::POST;


	exists = config.fieldNumber ("growfreq");
	if (exists >= 0)
	{
	        if( config.type(exists) != TpDouble && config.type(exists) != TpFloat  && config.type(exists) != TpInt )
	        {
			 throw( AipsError ( "Parameter 'growfreq' must be of type 'double'" ) );
	        }
		
		growfreq_p = config.asDouble("growfreq");

		if( growfreq_p < 0.0 || growfreq_p > 100.0 )
	        {
		         throw( AipsError ( "Unsupported value for growfreq:" + String::toString(growfreq_p) + ". Allowed range is 0.0 through 100.0" ) );
	        }
	}
	else
	{
		growfreq_p = 50.0;
	}

	*logger_p << logLevel_p << " growfreq is " << growfreq_p << LogIO::POST;


	exists = config.fieldNumber ("growaround");
	if (exists >= 0)
	{
	        if( config.type(exists) != TpBool )
	        {
			 throw( AipsError ( "Parameter 'growaround' must be of type 'bool'" ) );
	        }
		
		growaround_p = config.asBool("growaround");
	}
	else
	{
		growaround_p = false;
	}

	*logger_p << logLevel_p << " growaround is " << growaround_p << LogIO::POST;


	exists = config.fieldNumber ("flagneartime");
	if (exists >= 0)
	{
	        if( config.type(exists) != TpBool )
	        {
			 throw( AipsError ( "Parameter 'flagneartime' must be of type 'bool'" ) );
	        }
		
		flagneartime_p = config.asBool("flagneartime");
	}
	else
	{
		flagneartime_p = false;
	}

	*logger_p << logLevel_p << " flagneartime is " << flagneartime_p << LogIO::POST;


	exists = config.fieldNumber ("flagnearfreq");
	if (exists >= 0)
	{
	        if( config.type(exists) != TpBool )
	        {
			 throw( AipsError ( "Parameter 'flagnearfreq' must be of type 'bool'" ) );
	        }
		
		flagnearfreq_p = config.asBool("flagnearfreq");
	}
	else
	{
		flagnearfreq_p = false;
	}

	*logger_p << logLevel_p << " flagnearfreq is " << flagnearfreq_p << LogIO::POST;


	return;
}

bool
FlagAgentExtension::computeAntennaPairFlags(const vi::VisBuffer2 &/*visBuffer*/,
				FlagMapper &flags,Int /*antenna1*/,Int /*antenna2*/,vector<uInt> &/*rows*/)
{
	// Set logger origin
	logger_p->origin(LogOrigin(agentName_p,__FUNCTION__,WHERE));

	// Get flag cube size
	IPosition flagCubeShape = flags.shape();
	Int nPols,nChannels,nTimesteps;
	flags.shape(nPols, nChannels, nTimesteps);

	// STEP1: Grow flags around, across time and across frequency
	bool flag;
	uInt extendAround;
	if (flagneartime_p or flagnearfreq_p or growaround_p)
	{
		// Create the growing flag cube setting all flags initially to false
		Cube<Bool> *growFlags = new Cube<Bool>(nPols, nChannels+2, nTimesteps+2,false);

		// Iterate over flag cube and grow flags
		for (Int timestep_i=0;timestep_i<nTimesteps;timestep_i++)
		{
			for (Int chan_j=0;chan_j<nChannels;chan_j++)
			{
				for (Int pol_k=0;pol_k<nPols;pol_k++)
				{
					flag = flags.getModifiedFlags(pol_k,chan_j,timestep_i);

					// If the point is flagged we may extend in the time,frequency or polarization direction
					if (flag)
					{
						if (flagneartime_p)
						{
							// Extend flags to the next timestep
							growFlags->operator()(pol_k,chan_j+1,timestep_i + 2) = true;

							// Extend flags to the previous timestep
							growFlags->operator()(pol_k,chan_j+1,timestep_i) = true;
						}

						if (flagnearfreq_p)
						{
							// Extend flags to the next frequency
							growFlags->operator()(pol_k,chan_j+2,timestep_i+1) = true;

							// Extend flags to the previous frequency
							growFlags->operator()(pol_k,chan_j,timestep_i+1) = true;
						}
					}
					// Only if the point is not flagged, and grow around is activated, we check the nearest neighbours
					else if (growaround_p)
					{
						extendAround = 0;
						for (Int timestep_inner=timestep_i-1;timestep_inner<=timestep_i+1;timestep_inner++)
						{
							// Calculate number of flagged neighbours
							for (Int chan_inner=chan_j-1;chan_inner<=chan_j+1;chan_inner++)
							{
								if (	(timestep_inner>=0) and
										(timestep_inner<nTimesteps) and
										(chan_inner>=0) and
										(chan_inner<nChannels))
								{
									extendAround += flags.getModifiedFlags(pol_k,chan_inner,timestep_inner);
								}
							} // End grow around channel iteration
						} // End grow around time iteration

						// Extend flags if we have more than 4 flagged  neighbours
						if (extendAround>4) growFlags->operator()(pol_k,chan_j+1,timestep_i+1) = true;

					} // End grow around

				} // For polarization
			} // For channel
		} // For timestep

		// Apply grow flags to the common modified flag cube
		for (uInt timestep_i=0;timestep_i<static_cast<uInt>(nTimesteps);timestep_i++)
		{
			for (uInt chan_j=0;chan_j<static_cast<uInt>(nChannels);chan_j++)
			{
				for (uInt pol_k=0;pol_k<static_cast<uInt>(nPols);pol_k++)
				{
					if (growFlags->operator()(pol_k,chan_j+1,timestep_i+1))
					{
						flags.setModifiedFlags(pol_k,chan_j,timestep_i);
						visBufferFlags_p += 1;
					}
				}
			}
		}

		// Delete growing flag cube
		delete growFlags;

	} // End grow flags step


	// STEP2: Extend flags across time and frequency
	if ( (growtime_p > 0) or (growfreq_p > 0) )
	{
		// Per correlation accumulate over time and frequency
		Matrix<Double> acummTime(nPols,nTimesteps,0);
		Matrix<Double> acummFreq(nPols,nChannels,0);
		for (Int timestep_i=0;timestep_i<nTimesteps;timestep_i++)
		{
			for (Int chan_j=0;chan_j<nChannels;chan_j++)
			{
				for (Int pol_k=0;pol_k<nPols;pol_k++)
				{
					flag = flags.getModifiedFlags(pol_k,chan_j,timestep_i);
					acummTime(pol_k,timestep_i) += flag;
					acummFreq(pol_k,chan_j) += flag;
				}
			}
		}

		// Extend across frequency
		if (growfreq_p > 0)
		{
			for (Int timestep_i=0;timestep_i<nTimesteps;timestep_i++)
			{
				for (Int pol_k=0;pol_k<nPols;pol_k++)
				{
					if ((100.0*acummTime(pol_k,timestep_i)/nChannels) >= growfreq_p)
					{
						// Flag all the channels for this polarization and timestep
						for (Int chan_j=0;chan_j<nChannels;chan_j++)
						{
							flags.setModifiedFlags(pol_k,chan_j,timestep_i);
							visBufferFlags_p += 1;
						}
					}
				}
			}
		}

		// Extend across time
		if (growtime_p > 0)
		{
			for (Int chan_j=0;chan_j<nChannels;chan_j++)
			{
				for (Int pol_k=0;pol_k<nPols;pol_k++)
				{
					if ((100.0*acummFreq(pol_k,chan_j)/nTimesteps) >= growtime_p)
					{
						// Flag all the timesteps for this polarization and channel
						for (Int timestep_i=0;timestep_i<nTimesteps;timestep_i++)
						{
							flags.setModifiedFlags(pol_k,chan_j,timestep_i);
							visBufferFlags_p += 1;
						}
					}
				}
			}
		}
	}

	// STEP3: Extend across polarizations
	if (extendpols_p)
	{
		for (Int timestep_i=0;timestep_i<nTimesteps;timestep_i++)
		{
			for (Int chan_j=0;chan_j<nChannels;chan_j++)
			{
				for (Int pol_k=0;pol_k<nPols;pol_k++)
				{
					if (flags.getModifiedFlags(pol_k,chan_j,timestep_i))
					{
						for (Int ineer_pol_k=0;ineer_pol_k<nPols;ineer_pol_k++)
						{
							flags.setModifiedFlags(ineer_pol_k,chan_j,timestep_i);
						}
						visBufferFlags_p += nPols;
						break;
					}
				}
			}
		}
	}

	return false;
}

} //# NAMESPACE CASA - END


