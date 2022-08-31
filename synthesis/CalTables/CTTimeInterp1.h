//# CTTimeInterp1.h: Definition for Single-element Cal Interpolation time
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
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

#ifndef CALTABLES_CTTIMEINTERP1_H
#define CALTABLES_CTTIMEINTERP1_H

#include <casacore/casa/aips.h>

#include <synthesis/CalTables/NewCalTable.h>
#include <synthesis/CalTables/RIorAParray.h>
#include <casacore/casa/Arrays/Cube.h>
#include <casacore/scimath/Functionals/Interpolate1D.h>
#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogSink.h>

namespace casa { //# NAMESPACE CASA - BEGIN


class CTTimeInterp1
{
public:

  // Null ctor 
  //  CTTimeInterp1();  // TBD: make private?

  // From NewCalTable
  CTTimeInterp1(NewCalTable& ct,
		const casacore::String& timetype,
		casacore::Array<casacore::Float>& result,
		casacore::Array<casacore::Bool>& rflag);

  // Destructor
  virtual ~CTTimeInterp1();

  // Some state info
  casacore::Double timeRef() const { return timeRef_; };
  casacore::String timeType() const { return timeType_; };
  casacore::Vector<casacore::Double> domain() const { return domain_; };

  // Interpolate, given timestamp; returns T if new result
  virtual casacore::Bool interpolate(casacore::Double time);

  // Interpolate, given timestamp and fiducial freq; returns T if new result
  virtual casacore::Bool interpolate(casacore::Double newtime, casacore::Double freq);

  virtual void state(casacore::Bool verbose=false);

  // static factory method to make CTTimeInterp1
  static CTTimeInterp1* factory(NewCalTable& ct,
				const casacore::String& timetype,
				casacore::Array<casacore::Float>& result,
				casacore::Array<casacore::Bool>& rflag) {
    return new casa::CTTimeInterp1(ct,timetype,result,rflag); }

  // Set interpolation type
  void setInterpType(casacore::String strtype);

protected:

  // Find time registration
  casacore::Bool findTimeRegistration(casacore::Int& idx,casacore::Bool& exact,casacore::Float newtime);

  // Do phase delay math
  void applyPhaseDelay(casacore::Double freq);


  // The CalTable 
  //  Should be a single antenna, spw
  NewCalTable ct_;

  // Access to columns
  ROCTMainColumns *mcols_p;

  // Interpolation modes
  casacore::String timeType_;

  // Current time, idx
  casacore::Double currTime_;
  casacore::Int currIdx_;
  casacore::Bool lastWasExact_;

  // casacore::Time list
  casacore::Double timeRef_;
  casacore::Vector<casacore::Float> timelist_;
  casacore::Vector<casacore::Double> domain_;

  // Flags list
  casacore::Cube<casacore::Bool> flaglist_;

  // Pointer to the time interpolator
  casacore::Interpolate1D<casacore::Float,casacore::Array<casacore::Float> > *tInterpolator_p;

  // Phase-delay correction info
  casacore::Double cfreq_;
  casacore::Cube<casacore::Float> cycles_;   // [npar,nchan,ntime]
  
  // Arrays in which to deposit results
  //  (ordinarily, these (CASA-casacore::Array) reference external Arrays)
  casacore::Array<casacore::Float> result_;
  casacore::Array<casacore::Bool> rflag_;

};

// Pointer to static factory methods for CTTimeInterp1
typedef CTTimeInterp1* (*CTTIFactoryPtr)(NewCalTable&,
					 const casacore::String&,
					 casacore::Array<casacore::Float>&,
					 casacore::Array<casacore::Bool>&);

} //# NAMESPACE CASA - END

#endif
