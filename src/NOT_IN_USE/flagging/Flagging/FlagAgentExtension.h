//# FlagAgentExtension.h: This file contains the interface definition of the FlagAgentExtension class.
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

#ifndef FlagAgentExtension_H_
#define FlagAgentExtension_H_

#include <flagging/Flagging/FlagAgentBase.h>

namespace casa { //# NAMESPACE CASA - BEGIN

class FlagAgentExtension : public FlagAgentBase {

public:

	FlagAgentExtension(FlagDataHandler *dh, casacore::Record config, casacore::Bool writePrivateFlagCube = false);
	~FlagAgentExtension();

protected:

	// Compute flags for a given (time,freq) map
	bool computeAntennaPairFlags(const vi::VisBuffer2 &visBuffer, FlagMapper &flags,casacore::Int antenna1,casacore::Int antenna2,std::vector<casacore::uInt> &rows);


	// Parse configuration parameters
	void setAgentParameters(casacore::Record config);

private:

	// casacore::Input parameters
	casacore::Bool extendpols_p;
	casacore::Bool growaround_p;
	casacore::Bool flagneartime_p;
	casacore::Bool flagnearfreq_p;
	casacore::Double growtime_p;
	casacore::Double growfreq_p;
};


} //# NAMESPACE CASA - END

#endif /* FlagAgentExtension_H_ */

