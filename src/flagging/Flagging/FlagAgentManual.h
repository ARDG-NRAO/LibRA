//# FlagAgentManual.h: This file contains the interface definition of the FlagAgentManual class.
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

#ifndef FlagAgentManual_H_
#define FlagAgentManual_H_

#include <flagging/Flagging/FlagAgentBase.h>

namespace casa { //# NAMESPACE CASA - BEGIN

class FlagAgentManual : public FlagAgentBase {

public:

	FlagAgentManual(FlagDataHandler *dh, casacore::Record config, casacore::Bool writePrivateFlagCube = false, casacore::Bool flag = true);
	~FlagAgentManual();

protected:

	void initialize(casacore::Record config);

	// Flag entire row
	bool computeRowFlags(const vi::VisBuffer2 &visBuffer, FlagMapper &flags, casacore::uInt row);

private:

};


} //# NAMESPACE CASA - END

#endif /* FlagAgentManual_H_ */

