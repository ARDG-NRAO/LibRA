//# AccorJones.h: Declaration of amplitude normalization VisCal
//# Copyright (C) 1996,1997,2000,2001,2002,2003,2011,2016,2017
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

#ifndef SYNTHESIS_ACCORJONES_H
#define SYNTHESIS_ACCORJONES_H

#include <casacore/casa/aips.h>
#include <synthesis/MeasurementComponents/SolvableVisCal.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// Accor (normalization) VisCal
class AccorJones : public SolvableVisJones {
public:
  AccorJones(VisSet& vs);
  AccorJones(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  AccorJones(const MSMetaInfoForCal& msmc);
  AccorJones(const casacore::Int& nAnt);

  virtual ~AccorJones();

  // Return the type enum
  virtual Type type() { return VisCal::G; };

  // Return type name as string
  virtual casacore::String typeName()     { return "Accor Jones"; };
  virtual casacore::String longTypeName() { return "Accor Jones (normalization)"; };

  // Type of Jones matrix according to nPar()
  virtual Jones::JonesType jonesType() { return Jones::Diagonal; };

  // This type is smoothable
  virtual casacore::Bool smoothable() { return true; };

  // Nominally, we will only use parallel hands for now
  virtual casacore::Bool phandonly() { return true; };

  virtual casacore::Bool normalizable() { return true; };

  virtual casacore::Bool useGenericSolveOne() { return false; }

  // Hazard a guess at parameters (unneeded here)
  virtual void guessPar(VisBuffer&) {};

  // Local implementation of selfSolveOne (generalized signature)
  virtual void selfSolveOne(VisBuffGroupAcc& vbga);
  virtual void selfSolveOne(SDBList& sdbs);

  virtual void keepNCT();

protected:

  // G has two trivial casacore::Complex parameters
  virtual casacore::Int nPar() { return 2; };

  // Jones matrix elements are trivial
  virtual casacore::Bool trivialJonesElem() { return true; };

  // dG/dp are trivial
  virtual casacore::Bool trivialDJ() { return true; };

  // Initialize trivial dJs
  virtual void initTrivDJ();
};

} //# NAMESPACE CASA - END

#endif
