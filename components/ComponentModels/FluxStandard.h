//# FluxStandard.h: Compute flux densities for standard reference sources
//# Copyright (C) 1996,1997,1999,2001
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
//# $Id: FluxStandard.h 21292 2012-11-28 14:58:19Z gervandiepen $

#ifndef COMPONENTS_FLUXSTANDARD_H
#define COMPONENTS_FLUXSTANDARD_H

#include <casa/aips.h>
#include <components/ComponentModels/Flux.h>
#include <measures/Measures/MDirection.h>

namespace casacore{

class String;        //#include <casa/BasicSL/String.h>
class MEpoch;        //#include <measures/Measures/MEpoch.h>
class MFrequency;    //#include <measures/Measures/MFrequency.h>
}

namespace casa { //# NAMESPACE CASA - BEGIN

// Forward declarations
class SpectralModel; //#include <components/ComponentModels/SpectralModel.h>

// <summary> 
// FluxStandard: Compute flux densities for standard reference sources
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <li><linkto class="Flux">Flux</linkto> module
// </prerequisite>
//
// <etymology>
// From "flux density" and "standard".
// </etymology>
//
// <synopsis>
// The FluxStandard class provides a means to compute total flux
// densities for specified sources on a standard
// flux density scale, such as that established by Baars or
// Perley and Taylor.
// </synopsis>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate information on standard flux density computation in one class.
// </motivation>
//
// <todo asof="99/06/01">
// <li> closer integration into component models.
// </todo>

class FluxStandard
{
 public:
  // Flux scale types.
  // Standards which do not include resolution info must come before
  // HAS_RESOLUTION_INFO, and those with it must come after.
  enum FluxScale {
    // Perley (1990); plus Reynolds (1934-638; 7/94); Baars (3C138)
    PERLEY_90 = 0,

    // Perley and Taylor (1995.2); plus Reynolds (1934-638; 7/94)
    PERLEY_TAYLOR_95,

    // Perley and Taylor (1999.2); plus Reynolds (1934-638; 7/94)
    PERLEY_TAYLOR_99,

    // Baars scale
    // Baars J. W. M., Genzel R., Pauliny-Toth I. I. K., et al., 1977,
    // A&A, 61, 99
    // http://cdsads.u-strasbg.fr/abs/1977A%26A....61...99B
    BAARS,

    // Perley-Butler 2010 Scale (using VLA [not EVLA!] data)
    PERLEY_BUTLER_2010,

    // Perley-Butler 2013 (include time variable sources)
    PERLEY_BUTLER_2013,

    // Scaife & Heald 2012MNRAS.423L..30S
    // broadband low-frequency flux scale for frequencies <~500 MHz
    SCAIFE_HEALD_2012,

    // Stevens & Reynolds 2016, A combination of the Reynolds low
    // frequency flux scale with the Stevens high frequency flux scale
    // for 1934-638 described in Partridge et al (2016), ApJ 821,1
    STEVENS_REYNOLDS_2016,
    //
    // Perley-Butler 2017 (inculde time variable sources from Perley-Butler 2013)
    PERLEY_BUTLER_2017,

    HAS_RESOLUTION_INFO,

    // Estimate the flux density for a Solar System object using a JPL Horizons
    // ephemeris/data page and model provided by Bryan Butler.
    SS_JPL_BUTLER = HAS_RESOLUTION_INFO,

    // The number of standards in this enumerator.
    NUMBER_STANDARDS
  };

  // Default constructor, and destructor
  FluxStandard(const FluxStandard::FluxScale scale =
               FluxStandard::PERLEY_TAYLOR_99);
  ~FluxStandard();

  // Compute the flux density for a specified source at a specified frequency
  casacore::Bool compute (const casacore::String& sourceName, const casacore::MDirection& sourceDir, const casacore::MFrequency& mfreq, const casacore::MEpoch& mtime,
		Flux<casacore::Double>& value, Flux<casacore::Double>& error);

  // Compute the flux densities and their uncertainties for a specified source
  // at a set of specified frequencies.
  casacore::Bool compute(const casacore::String& sourceName, 
               const casacore::MDirection& sourceDir,
               const casacore::Vector<casacore::MFrequency>& mfreqs,
               const casacore::MEpoch& mtime,
	       casacore::Vector<Flux<casacore::Double> >& values,
               casacore::Vector<Flux<casacore::Double> >& errors,
               const casacore::Bool verbose=true);

  // Compute the flux densities and their uncertainties for a specified source
  // for a set of sets of specified frequencies, i.e. mfreqs[spw] is a set of
  // frequencies for channels in spectral window spw, and values and errors are
  // arranged the same way.
  casacore::Bool compute(const casacore::String& sourceName,
               const casacore::MDirection& sourceDir,
               const casacore::Vector<casacore::Vector<casacore::MFrequency> >& mfreqs,
               const casacore::MEpoch& mtime,
               casacore::Vector<casacore::Vector<Flux<casacore::Double> > >& values,
               casacore::Vector<casacore::Vector<Flux<casacore::Double> > >& errors);

  // Like compute, but it also saves a set of ComponentLists for the source to
  // disk and puts the paths (sourceName_mfreq_mtime.cl) in clnames, making it
  // suitable for resolved sources.
  // mtime is ignored for nonvariable objects.
  // Solar System objects are typically resolved and variable!
  // The ComponentList names are formed from prefix, sourceName, the
  // frequencies, and times.
  casacore::Bool computeCL(const casacore::String& sourceName, const casacore::Vector<casacore::Vector<casacore::MFrequency> >& mfreqs,
                 const casacore::MEpoch& mtime, const casacore::MDirection& position,
                 casacore::Vector<casacore::Vector<Flux<casacore::Double> > >& values,
                 casacore::Vector<casacore::Vector<Flux<casacore::Double> > >& errors,
                 casacore::Vector<casacore::String>& clnames, const casacore::String& prefix="");

  // set interpolation method for a time-variable source
  void setInterpMethod(const casacore::String& interpmethod);
  

  // Take a component cmp and save it to a ComponentList on disk, returning the
  // pathname.  ("" if unsuccessful, sourceName_mfreqGHzDateTime.cl otherwise)
  //
  // This is also used outside of FluxStandard, but it is declared here instead
  // of in ComponentList because it is somewhat specialized, mainly in setting
  // up the pathname.  The ComponentList name is formed from prefix, sourceName,
  // mfreq, and mtime.
  //
  static casacore::String makeComponentList(const casacore::String& sourceName, const casacore::MFrequency& mfreq,
                                  const casacore::MEpoch& mtime, const Flux<casacore::Double>& fluxval,
                                  const ComponentShape& cmp,
                                  const SpectralModel& spectrum,
				  const casacore::String& prefix="");

  // Variation of the above that will fill a TabularSpectrum with mfreqs and
  // values if appropriate.
  static casacore::String makeComponentList(const casacore::String& sourceName,
                                  const casacore::Vector<casacore::MFrequency>& mfreqs,
                                  const casacore::MEpoch& mtime,
                                  const casacore::Vector<Flux<casacore::Double> >& values,
                                  const ComponentShape& cmp,
                                  const casacore::String& prefix="");

  // Decode a string representation of the standard or catalog name
  static casacore::Bool matchStandard(const casacore::String& name, 
			    FluxStandard::FluxScale& stdEnum,
			    casacore::String& stdName);

  // Return a standard string description for each scale or catalog
  static casacore::String standardName(const FluxStandard::FluxScale& stdEnum);

 private:
  // Flux scale in use
  FluxStandard::FluxScale itsFluxScale;

  casacore::Bool has_direction_p;

  casacore::MDirection direction_p;

  casacore::String interpmethod_p;
};

} //# NAMESPACE CASA - END

#endif
