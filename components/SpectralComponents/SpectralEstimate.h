//# SpectralEstimate.h: Get an initial estimate for spectral lines
//# Copyright (C) 2001,2002,2003,2004
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
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id: SpectralEstimate.h 20229 2008-01-29 15:19:06Z gervandiepen $

#ifndef COMPONENTS_SPECTRALESTIMATE_H
#define COMPONENTS_SPECTRALESTIMATE_H

#include <casa/aips.h>
#include <components/SpectralComponents/SpectralElement.h>
#include <components/SpectralComponents/SpectralList.h>
#include <casacore/casa/Arrays/ArrayFwd.h>

namespace casa { //# NAMESPACE CASA - BEGIN

//# Forward Declarations
class GaussianSpectralElement;

// <summary>
// Get an initial estimate for spectral lines
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="tSpectralFit" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class=SpectralFit>SpectralFit</linkto> class
// </prerequisite>
//
// <etymology>
// From spectral line and estimate
// </etymology>
//
// <synopsis>
// The SpectralEstimate class obtains an initial guess for spectral
// components. The current implementation uses the entire 
// profile as signal region, or can set a window to be searched around
// the highest peak automatically. A window can also be set manually.
// The second derivative of
// the profile in the signal region is calculated by fitting
// a second degree polynomal. The smoothing parameter Q
// determines the number of points used for this (=2*Q+1).
// The gaussians can then be estimated as described by
// Schwarz, 1968, Bull.Astr.Inst.Netherlands, Volume 19, 405.
//
// The elements guessed  can be used in the
// <linkto class=SpectralFit>SpectralFit</linkto> class.
//
// The default type found is a Gaussian, defined as:
// <srcblock>
//	AMPL.exp[ -(x-CENTER)<sup>2</sup>/2 SIGMA<sup>2</sup>]
// </srcblock>
//
// The parameter estimates are returned in units of zero-based
// pixel indices.
// </synopsis>
//
// <example>
// </example>
//
// <motivation>
// To have an automatic method to find spectral lines
// </motivation>
//
// <todo asof="2001/02/14">
//   <li> find a way to get to absorption lines as well
//   <li> add more estimation options
// </todo>

class SpectralEstimate {
 public:
  //# Constants
  // Default maximum number of components to be found
  static const casacore::uInt MAXPAR = 200;
  //# Enumerations
  //# Friends

  //# Constructors
  // Default constructor creates a default estimator (default max number
  // of components to be found is 200) with the given maximum number
  // of components that will be found. A value of zero will indicate
  // an unlimited number.
  explicit SpectralEstimate(const casacore::uInt maxpar=MAXPAR);
  // Create an estimator with the given maximum number of possible
  // elements. A value of zero will indicate an unlimited number.
  // Construct with a given rms in profiles, a cutoff for amplitudes
  // found, and a minimum width. Cutoff and minsigma default to 0.0, maximum
  // size of list produced to 200.
  explicit SpectralEstimate(const casacore::Double rms,
			    const casacore::Double cutoff=0.0, const casacore::Double minsigma=0.0,
			    const casacore::uInt maxpar=MAXPAR);
  // Copy constructor (deep copy)
  SpectralEstimate(const SpectralEstimate &other);

  //#Destructor
  // Destructor
  ~SpectralEstimate();

  //# Operators
  // Assignment (copy semantics)
  SpectralEstimate &operator=(const SpectralEstimate &other);

  //# Member functions
  // Generate the estimates for a profile and return the 
  // list found.  The first function returns component parameters 
  // in units of pixel indices. The second function calls the first
  // and then converts to the specified abcissa space (the supplied 
  // vector must be monotonic); if the pixel-based center is out of range 
  // of the supplied abcissa vector the conversion is done via extrapolation.
  // The der pointer is meant for debugging, and can return 
  // the derivative profile.  The second function throws an AipsError
  // if the vectors are not the same length.
  // <group>
  template <class MT>
    const SpectralList& estimate(const casacore::Vector<MT>& ordinate,
				 casacore::Vector<MT> *der = 0);
  template <class MT>
    const SpectralList& estimate(const casacore::Vector<MT>& abcissa,
                                 const casacore::Vector<MT>& ordinate);
  // </group>

  // Return the list found.
  const SpectralList &list() const {return slist_p; };

  // Set estimation parameters
  // <group>
  // Set the profile's estimated rms (forced to abs(rms))
  void setRMS(const casacore::Double rms=0.0);
  // Set the amplitude cutoff for valid estimate (forced to max(0,cutoff))
  void setCutoff(const casacore::Double cutoff=0.0);
  // Set the minimum width allowed (forced to max(0,minsigma))
  void setMinSigma(const casacore::Double minsigma=0.0);
  // Set the number of points consider at each side of test point (i.e. a
  // width of 2q+1 is taken). Default internally is 2; max(1,q) taken.
  void setQ(const casacore::uInt q=2);
  // Set a region [lo,hi] over which to estimate. Lo and hi are given as
  // zero-based vector indices.
  void setRegion(const casacore::Int lo, const casacore::Int hi);
  // Do you want to look in an automatically determined window with signal?
  // Default is false, meaning the full (possibly regioned) profile.
  void setWindowing(const casacore::Bool win=false);
  // Set the maximum number of estimates to find (forced to >=1; 200 default)
  void setMaxN(const casacore::uInt maxpar=MAXPAR);
  // </group>

 private:
  //#Data
  // Use window search
  casacore::Bool useWindow_p;
  // rms estimate in profile
  casacore::Double rms_p;
  // Source cutoff amplitude
  casacore::Double cutoff_p;
  // Window low and end value
  // <group>
  casacore::Int windowLow_p;
  casacore::Int windowEnd_p;
  // </group>
  // Region low and high value
  // <group>
  casacore::Int regionLow_p;
  casacore::Int regionEnd_p;
  // </group>
  // Smoothing parameter. I.e. 2q+1 points are taken
  casacore::Int q_p;
  // Internal cashing of calculated values based on q
  // <group>
  casacore::Double a_p;
  casacore::Double b_p;
  // </group>
  // The minimum Gaussian width
  casacore::Double sigmin_p;
  // The second derivatives
  casacore::Double *deriv_p;
  // The list of components
  SpectralList slist_p;
  // The length of the current profile being estimated
  casacore::uInt lprof_p;

  //# Member functions
  // Get the window or the total spectrum
  template <class MT>
    casacore::uInt window(const casacore::Vector<MT> &prof);
  // Get the second derivatives
  template <class MT>
    void findc2(const casacore::Vector<MT> &prof);
  // Find the Gaussians
  template <class MT>
    void findga(const casacore::Vector<MT> &prof);
  // Convert the parameters of the components in the list from 
  // pixel-based indices to the given abcissa-vector space.
  template <class MT> GaussianSpectralElement convertElement (const casacore::Vector<MT>& abcissa,
                                                      const GaussianSpectralElement& el) const;
};


} //# NAMESPACE CASA - END

#ifndef CASACORE_NO_AUTO_TEMPLATES
#include <components/SpectralComponents/Spectral2Estimate.tcc>
#endif //# CASACORE_NO_AUTO_TEMPLATES
#endif
