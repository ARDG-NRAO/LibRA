//# SimpleComponentFTMachine.h: Definition for SimpleComponentFTMachine
//# Copyright (C) 1996,1997,1998,2000,2001
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

#ifndef SYNTHESIS_SIMPLECOMPONENTFTMACHINE_H
#define SYNTHESIS_SIMPLECOMPONENTFTMACHINE_H

#include <msvis/MSVis/VisBuffer.h>
#include <synthesis/TransformMachines/ComponentFTMachine.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MPosition.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Logging/LogIO.h>
#include <casa/Logging/LogSink.h>
#include <casa/Logging/LogMessage.h>


namespace casa { //# NAMESPACE CASA - BEGIN

// <summary>  does the simplest Fourier transform on SkyComponents </summary>

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


class SimpleComponentFTMachine : public ComponentFTMachine {
public:

  // Get actual coherence 
  virtual void get(VisBuffer& vb, const ComponentList& compList, casacore::Int row=-1);
  // Get actual coherence 
  virtual void get(VisBuffer& vb, SkyComponent& skycomponent, casacore::Int row=-1);

protected:
  void applyPhasor(const casacore::Int part, const casacore::Block<casacore::Int>& startrow, 
		   const casacore::Vector<casacore::uInt>& nRowp,  
		   const casacore::Vector<casacore::Double>& dphase,  const casacore::Vector<casacore::Double>& invLambda, 
		   const casacore::Int npol, const casacore::Int nchan, 
		   const casacore::Vector<casacore::Int>& corrType, 
		   const casacore::Cube<casacore::DComplex>& dVis, casacore::Complex*& modData);
  
  void predictVis(casacore::Complex*& modData, const casacore::Vector<casacore::Double>& invLambda, 
		  const casacore::Vector<casacore::Double>& frequency, 
		  const ComponentList& compList, 
		  ComponentType::Polarisation poltype, const casacore::Vector<casacore::Int>& corrType, 
		  const casacore::uInt startrow, const casacore::uInt nrows, 
		  const casacore::uInt nchan, const casacore::uInt npol, 
		  const casacore::Block<casacore::Matrix<casacore::Double> > & uvwcomp, 
		  const casacore::Block<casacore::Vector<casacore::Double> > & dphasecomp);

};

} //# NAMESPACE CASA - END

#endif
