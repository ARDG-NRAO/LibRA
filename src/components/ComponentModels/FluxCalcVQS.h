//# FluxCalcVQS.h: Base class for flux standard calculations taking into account for 
//# time variability 
//# Copyright (C) 2013
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
#ifndef COMPONENTS_FLUXCALCVQS_H
#define COMPONENTS_FLUXCALCVQS_H

#include <components/ComponentModels/FluxStandard.h>
#include <components/ComponentModels/FluxStdSrcs.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/OS/Path.h>
#include <casacore/measures/Measures/MDirection.h>
#include <casacore/tables/Tables/Table.h>

//# Handy for passing anonymous arrays to functions.
#include <casacore/scimath/Mathematics/RigidVector.h>

#include <casacore/scimath/Functionals/Interpolate1D.h>
#include <map>


namespace casacore{

class MFrequency;
//class Vector;
}

namespace casa { //# NAMESPACE CASA - BEGIN

//class Flux;

// <summary> 
// FluxCalcVQS: Base class for flux standard calculations taking account for  
// time variability of the sources. 
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <li><linkto class="FluxStandard">FluxStandard</linkto> module
// </prerequisite>
//
// <etymology>
// From "flux density", "calculator", "variable", and "quasistatic".
// </etymology>
//
// <synopsis>
// The FluxCalcVQS class provides an interface and a small amount of machinery
// for computing total flux densities of calibrators which may be variable in time.
// For the source with significant enough time variability, inteploation in time
// with choice of nearest neighbour, linear, cubic, or cubic spline, is performed. 
// See FluxStdsQS for actual definitions of the standards. The polynomial coefficents
// for the standard include time-variable sources are assumed to be stored in an external
// table.
// </synopsis>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Provide a base interface for calculating standard flux
// densities, and include any common functions.
// </motivation>

class FluxCalcVQS: public FluxStdSrcs
{
public:

  typedef FluxCalcVQS FCVQS;
  typedef casacore::RigidVector<casacore::String, 4> RVS4;
  typedef casacore::RigidVector<casacore::String, 5> RVS5;

  // Source identifiers.
  /****
  enum Source {
    THREEC286 = 0,      // 3C286
    THREEC48,
    THREEC147,
    THREEC138,
    NINETEEN34M638,   // 1934-638
    THREEC295,
    THREEC196,
    THREEC123,
    // The number of standards in this enumerator.
    NUMBER_SOURCES,
    UNKNOWN_SOURCE = NUMBER_SOURCES
  };
  ***/
  virtual ~FluxCalcVQS();

  virtual casacore::Bool operator()(Flux<casacore::Double>& value, Flux<casacore::Double>& error,
                          const casacore::MFrequency& mfreq, const casacore::Bool updatecoeffs) = 0;
  casacore::Bool operator()(casacore::Vector<Flux<casacore::Double> >& values,
                  casacore::Vector<Flux<casacore::Double> >& errors,
                  const casacore::Vector<casacore::MFrequency>& mfreqs);
  
  //for time variable case with interpolation method 
  casacore::Bool operator()(casacore::Vector<Flux<casacore::Double> >& values,
                  casacore::Vector<Flux<casacore::Double> >& errors,
                  const casacore::Vector<casacore::MFrequency>& mfreqs, 
                  const casacore::MEpoch& mtime,
                  const casacore::String& interpmethod);

  // If a FS::Source enum matches srcName, returns the enum.
  // Otherwise, FCQS::UNKNOWN_SOURCE.
  //FCQS::Source srcNameToEnum(const casacore::String& srcName) const;

  // Sets srcEnum_p = srcNameToEnum(sourceName), and returns
  // srcEnum_p != FCQS::UNKNOWN_SOURCE
  virtual casacore::Bool setSource(const casacore::String& sourceName, const casacore::MDirection& sourceDir);

  FCVQS::Source getSrcEnum();

  //casacore::MDirection getDirection() {return directions_p[srcEnum_p];}
  casacore::MDirection getDirection() {return FluxStdSrcs::getDirection(srcEnum_p);}

  // Read the coefficient data table
  void readQSCoeffsTable(const casacore::Path& fileName);
  // Interpolate for time variable source
  void interpolate(const casacore::String& interpmethod);
  // Set the coefficients from one epoch where i is row number in the original data table  
  void setSourceCoeffsfromVec(casacore::uInt& i);
  // Get currently set coefficients
  casacore::RigidVector<casacore::Vector<casacore::Float>,2 >  getCurrentCoeffs() {return tvcoeffs_p;}
  //casacore::RigidVector<casacore::Double,2 >  getValidFreqRange() {return validfreqrange_p;}
  casacore::Vector<casacore::MFrequency>  getValidFreqRange() {return validfreqrange_p;}

  //keep track if it is non-time var source for Perley-Butler2013
  void isTimeVar(casacore::Bool istimevar); 

protected:
  FluxCalcVQS();   // Initializes names_p.

private:
  FCVQS::Source srcEnum_p;       // The source identifier.

  // A map from an FS::Source enum to a list of recognized names for it.
  //std::map<FCQS::Source, casacore::Vector<casacore::String> > names_p;

  // A map from an FS::Source enum to its J2000 direction.
  //std::map<FCQS::Source, casacore::MDirection> directions_p;

  // get interpolate method enum
  casacore::Interpolate1D<casacore::Double,casacore::Float>::Method getInterpMethod_p(const casacore::String& interpmethod);

  //convert epochs in year.frac to mjds
  void convertYearFracToMjd(const casacore::Vector<casacore::Double>& yearfrac, casacore::Vector<casacore::Double>& mjds);

  casacore::Vector<casacore::Double> epochvec_p;
  casacore::Matrix<casacore::Float> coeffsmat_p;
  casacore::Matrix<casacore::Float> coefferrsmat_p;
  casacore::Vector<casacore::Float> fluxes_p;
  //casacore::Vector<casacore::Float> tvcoeffs_p;
  casacore::RigidVector<casacore::Vector<casacore::Float>,2> tvcoeffs_p;
  casacore::Vector<casacore::MFrequency> validfreqrange_p;
  casacore::Table Table_p;
  casacore::Bool istimevar_p;
  //virtual casacore::Bool setCoeffs() = 0;
  //
};

} //# NAMESPACE CASA - END

#endif /* COMPONENTS_FLUXCALCVQS_H */
