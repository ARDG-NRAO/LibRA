//# VWBT.h: This file contains the interface definition of the VWBT class.
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

#ifndef VWBT_H_
#define VWBT_H_

#include <msvis/MSVis/VisibilityIterator.h>
#include <msvis/MSVis/AsynchronousTools.h>
using namespace casa::async;

namespace casa { //# NAMESPACE CASA - BEGIN

class VWBT : public casa::async::Thread {

public:

	VWBT(VisibilityIterator *visibilityIterator,
         casa::async::Mutex * msAccessMutex,
         bool groupRows);

	~VWBT ();

	void start();
	void terminate ();
	bool isWriting() { return writing_p;}

	// We just want a de-referenced copy
	void setFlag(casacore::Cube<casacore::Bool> flagCube);

protected:

	void * run ();

	void initialize();
	bool next();


private:
	
	// State parameters
	volatile casacore::Bool terminationRequested_p;
	casacore::Bool threadTerminated_p;
	casacore::Bool writing_p;

	// Writing members
	VisibilityIterator * visibilityIterator_p;
	casa::async::Mutex * msAccessMutex_p;
	casacore::Cube<casacore::Bool> * flagCube_p;

	// Configuration parameters
	casacore::Bool groupRows_p;
};

} //# NAMESPACE CASA - END

#endif /* VWBT_H_ */

