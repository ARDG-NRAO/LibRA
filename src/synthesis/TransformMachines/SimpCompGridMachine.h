//# SimpleComponentGridMachine.h: Definition for SimpleComponentGridMachine
//# Copyright (C) 1996,1997,1998,1999,2000,2001
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$

#ifndef SYNTHESIS_SIMPCOMPGRIDMACHINE_H
#define SYNTHESIS_SIMPCOMPGRIDMACHINE_H

#include <msvis/MSVis/VisBuffer.h>
#include <synthesis/TransformMachines/ComponentFTMachine.h>
#include <casacore/measures/Measures/MDirection.h>
#include <casacore/measures/Measures/MPosition.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Logging/LogSink.h>
#include <casacore/casa/Logging/LogMessage.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> does the simplest Fourier transform on SkyComponents </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class=ComponentFTMachine>ComponentFTMachine</linkto> class
//   <li> <linkto class=SkyEquation>SkyEquation</linkto> class
//   <li> <linkto class=VisBuffer>VisBuffer</linkto> module
// </prerequisite>
//
// <etymology>
// ComponentFTMachine is a Machine for Fourier Transforms of
// SkyComponents
// </etymology>
//
// <synopsis> 
// Does a simple transform of a sky component. The phase term
// is fully accurate but no smearing is included.
// </synopsis> 
//
// <example>
// </example>
//
// <motivation>
// Allow transformation of sky components.
//
// </motivation>
//
// <todo asof="97/10/01">
// </todo>

// Forward declarations
class ComponentList;

class SimpleComponentGridMachine : public ComponentFTMachine {
public:

  // Get actual coherence 
  virtual void get(VisBuffer& vb, SkyComponent& component, casacore::Int row=-1);
  // Get actual coherence 
  virtual void get(VisBuffer& vb, const ComponentList& skycomponentlist, casacore::Int row=-1);



protected:

};

} //# NAMESPACE CASA - END

#endif
