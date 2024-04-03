//# ACoh.h: Definition for ACoh
//# Copyright (C) 1996,1997,1999,2000
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

#ifndef SYNTHESIS_ACOH_H
#define SYNTHESIS_ACOH_H

#include <casacore/casa/aips.h>
#include <casacore/casa/BasicSL/Complex.h>
#include <synthesis/MeasurementEquations/Iterate.h>
#include <msvis/MSVis/VisSet.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// ACoh: Model additive errors for the VisEquation.
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto module="MeasurementComponents">MeasurementComponents</linkto> module
//   <li> <linkto class="VisSet">VisSet</linkto> class
//   <li> <linkto class="VisEquation">VisEquation</linkto> class
// </prerequisite>
//
// <etymology>
// ACoh describes an interface for additive errors to be used in
// the <linkto class="VisEquation">VisEquation</linkto>. It is an Abstract Base Class: most methods
// must be defined in derived classes.
// </etymology>
//
// <synopsis> 
//
// See <linkto class="VisEquation">VisEquation</linkto> for definition of the
// Visibility Measurement Equation.
//
// The following examples illustrate how an ACoh can be
// used:
// <ul>
// <li> Simulation: random additive noise of a certain scaling
// can be added: <linkto class="SimACoh">SimACoh</linkto>
// <li> Correlator offsets
// </ul>
// </synopsis> 
//
// <example>
// <srcblock>
//      // Read the VisSet from disk
//      VisSet vs("3c84.MS");
//
//      VisEquation ve(vs);
//
//      // Make an ACoh
//      CorrelatorOffsetACoh coac(vs);
//
//      ve.setACoh(coac);
//
//      // Correct the visset
//      ve.correct();
// </srcblock>
// </example>
//
// <motivation>
// The properties of an additive component must be described
// for the <linkto class="VisEquation">VisEquation</linkto>.
// </motivation>
//
// <todo asof="97/10/01">
// </todo>

class VisEquation;

class ACoh  {
public:
    ACoh() {};
    virtual ~ACoh() {};
    
    virtual casacore::Bool solve(VisEquation& ve) = 0;

    // Apply additive error (Inverse is minus)
    // <group>
    virtual VisBuffer& apply(VisBuffer& vb) = 0;
    virtual VisBuffer& applyInv(VisBuffer& vb) = 0;
    // </group>
private:
};


} //# NAMESPACE CASA - END

#endif
