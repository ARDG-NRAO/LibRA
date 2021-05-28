//# FJones.h:  Declaration of Ionospheric corrections
//# Copyright (C) 1996,1997,2000,2001,2002,2003,2011,2014
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

#ifndef SYNTHESIS_FJONES_H
#define SYNTHESIS_FJONES_H

#include <casa/aips.h>
#include <casa/Containers/Record.h>
#include <casa/BasicSL/Complex.h>
#include <synthesis/MeasurementComponents/VisCal.h>
#include <synthesis/MeasurementComponents/SolvableVisCal.h>
//#include <synthesis/MeasurementComponents/CalCorruptor.h>

#include <measures/Measures/EarthMagneticMachine.h>

namespace casa { //# NAMESPACE CASA - BEGIN

class FJones : public SolvableVisJones {
public:

  // Constructor
  FJones(VisSet& vs);
  FJones(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  FJones(const MSMetaInfoForCal& msmc);

  virtual ~FJones();

  // Return the type enum
  virtual Type type() { return VisCal::F; };

  // Return type name as string
  virtual casacore::String typeName()     { return "F Jones"; };
  virtual casacore::String longTypeName() { return "F Jones (Ionosphere)"; };

  // Report current Jones type (circ=diag, lin=general)
  Jones::JonesType jonesType() { return pjonestype_; };

  // Par is freq-INdep, but matrix is freq-dep
  virtual casacore::Bool freqDepPar() { return false; };
  virtual casacore::Bool freqDepMat() { return true; };

  // We have casacore::Float parameters
  virtual VisCalEnum::VCParType parType() { return VisCalEnum::REAL; };

  // Local setapply to enforce calwt=F and spwmap=[0] for FJones
  virtual void setApply(const casacore::Record& apply);
  using SolvableVisJones::setApply;

  // Report apply-related info
  casacore::String applyinfo();

  // Local specializations for TEC generation
  virtual void setSpecify(const casacore::Record& specify);
  virtual void specify(const casacore::Record& specify);

protected:

  // FJones has one casacore::Float par per ant
  virtual casacore::Int nPar() { return 1; };

  // Jones matrix elements are NOT trivial
  virtual casacore::Bool trivialJonesElem() { return false; };

  // Access to z.a. data
  casacore::Vector<casacore::Double>& za() { return za_; };

  // Detect pol basis and some geometry
  virtual void syncMeta(const VisBuffer& vb);
  virtual void syncMeta2(const vi::VisBuffer2& vb);

  // Calculate parameters (in this case, the z.a.)
  virtual void calcPar();

  // Calculate the FJones matrix for all ants
  virtual void calcAllJones();

private:

  // TEC specify/retrieval mode
  casacore::String tectype_;

  // Field calculation components
  casacore::MeasFrame mframe_;
  casacore::EarthMagneticMachine *emm_;

  // Fiducial ionosphere height
  casacore::Quantity ionhgt_;

  // TEC image filename
  casacore::String tecimname_;

  // zenith angle per antenna
  casacore::Vector<casacore::Double> za_;

  // The magnetic field
  casacore::Vector<casacore::Double> BlosG_;

  // Units
  casacore::Double radper_;

  // Lin (general) or Circ (diag)
  Jones::JonesType pjonestype_;

  // Measures geometry
  casacore::MDirection phasedir_p;
  casacore::ROScalarMeasColumn<casacore::MPosition> antpos_p;

};

} //# NAMESPACE CASA - END

#endif

