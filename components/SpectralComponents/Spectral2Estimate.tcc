//# Spectral2Estimate.cc: Member templates for SpectralEstimate
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
//# $Id: Spectral2Estimate.tcc 19935 2007-02-27 05:07:40Z Malte.Marquarding $

//# Includes
#include <components/SpectralComponents/SpectralEstimate.h>

#include <casa/BasicMath/Math.h>
#include <casa/BasicSL/Constants.h>
#include <casa/Utilities/Assert.h>
#include <components/SpectralComponents/CompiledSpectralElement.h>
#include <components/SpectralComponents/GaussianSpectralElement.h>
#include <components/SpectralComponents/PolynomialSpectralElement.h>

namespace casa { //#Begin namespace casa

//# Member templates
template <class MT>
const SpectralList &SpectralEstimate::estimate(const casacore::Vector<MT> &prof,
					       casacore::Vector<MT> *der) {
	  if (prof.nelements() != lprof_p) {
    delete [] deriv_p; deriv_p = 0; lprof_p = 0;
    lprof_p = prof.nelements();
    deriv_p = new casacore::Double[lprof_p];
  };
  // Check if signal in window
  if (!window(prof)) return slist_p;
  // Limit window
  windowEnd_p = casacore::min(windowEnd_p+q_p , casacore::Int(lprof_p));
  windowLow_p = casacore::max(windowLow_p-q_p , 0 );
  // Get the second derivatives
  findc2(prof);
  // Next for debugging
  if (der) {
    for (casacore::uInt i=0; i<lprof_p; i++) (*der)[i] = deriv_p[i];
  };
  // Find the estimates (sorted)
  findga(prof);
  // cout << slist_p << endl;
  return slist_p;
}

template <class MT>
const SpectralList& SpectralEstimate::estimate(const casacore::Vector<MT>& x,
                                               const casacore::Vector<MT>& y)
{
  if (x.nelements() != y.nelements()) {
     throw(casacore::AipsError("Abcissa and ordinate vectors must be the same length"));
  }
  if (x.nelements()==1) {
     throw(casacore::AipsError("Not enough elements in vectors")); 
  }
  // Get pixel-based estimate (into slist_p)
  estimate(y);
  // Convert
  for (casacore::uInt i=0; i<slist_p.nelements(); i++) {
	  if (slist_p[i]->getType() != SpectralElement::GAUSSIAN) {
		  throw casacore::AipsError("Non-gaussian spectral types cannot be estimated");
	  }
	  const GaussianSpectralElement elIn = *dynamic_cast<const GaussianSpectralElement *>(slist_p[i]);
	  GaussianSpectralElement elOut = convertElement (x, elIn);
	  slist_p.set(elOut, i);
  }
  return slist_p;
}


template <class MT>
casacore::uInt SpectralEstimate::window(const casacore::Vector<MT> &prof) {
  windowLow_p =0;
  windowEnd_p = 0;
  if (!useWindow_p || rms_p <= 0.0 || lprof_p == 0) {
    if (regionEnd_p) {
      windowLow_p = casacore::min(casacore::max(0,regionLow_p),casacore::Int(lprof_p));
      windowEnd_p = casacore::min(regionEnd_p, casacore::Int(lprof_p));
    } else windowEnd_p = lprof_p;
    return windowEnd_p-windowLow_p;
  };
  // Total flux in profile and max position
  casacore::Double flux(0.0);
  casacore::Double pmax(prof(0));
  casacore::uInt imax(0);
  for (casacore::Int i=windowLow_p; i<windowEnd_p; i++) {
    if (prof(i)>pmax) {
      pmax = prof(i);
      imax = i;
    };
    flux += prof(i);
  };
  // No data
  if (pmax < cutoff_p) return 0;
  // Window boundaries; new/old base and centre; width
  casacore::Int width(-1);
  casacore::Int nw(0);
  casacore::Double bnew(flux), bold;
  casacore::Double cnew(imax), cold;
  do {
    width++;
    cold = cnew;
    bold = bnew;
    windowLow_p = casacore::max(0, casacore::Int(cold-width+0.5));
    windowEnd_p = casacore::min(casacore::Int(lprof_p), casacore::Int(cold+width+1.5));
    // flux and first moment in window
    casacore::Double s(0);
    casacore::Double c(0);
    for (casacore::Int i=windowLow_p; i<windowEnd_p; i++) {
      s += prof(i);
      c += i*prof(i);
    };
    bnew = flux-s;
    nw = lprof_p-windowEnd_p+windowLow_p;
    if (s != 0.0) {
      cnew = c/s;
      if (cnew < 0 || cnew >= lprof_p) cnew = cold;
    };
  } while (abs(bnew-bold) > rms_p && nw);
  return windowEnd_p-windowLow_p;
}

template <class MT>
void SpectralEstimate::findc2(const casacore::Vector<MT> &prof) {
  for (casacore::Int i=windowLow_p; i<windowEnd_p; i++) {
    // Moments
    casacore::Double m0(0.0); 
    casacore::Double m2(0.0); 
    for (casacore::Int j = -q_p; j <= q_p; j++) {
      casacore::Int k = i+j;
      if (k >= 0 && k<casacore::Int(lprof_p)) {
	// add to moments
	m0 += prof(k);
	m2 += prof(k)*j*j;
      };
    };
    // get the derivative
    deriv_p[i] = a_p*(m2-b_p*m0);
  };
}

template <class MT>
void SpectralEstimate::findga(const casacore::Vector<MT> &prof) {
	casacore::Int i(windowLow_p-1);
	// Window on Gaussian
	casacore::Int iclo(windowLow_p);
	casacore::Int ichi(windowLow_p);
	// Peak counter
	casacore::Int nmax = 0;
	GaussianSpectralElement tspel;
	while (++i < windowEnd_p) {
        if (deriv_p[i] > 0.0) {
			// At edge?
			if (i > windowLow_p && i < windowEnd_p-1) {
				// Peak in 2nd derivative
				if (deriv_p[i-1] < deriv_p[i] && deriv_p[i+1] < deriv_p[i]) nmax++;
				// At start
			} else if (i == windowLow_p && deriv_p[i+1] < deriv_p[i]) nmax++;
			// At end of window
			else if (i == windowEnd_p-1 && deriv_p[i-1] < deriv_p[i]) nmax++;
		};
		switch (nmax) {
		// Search for next peak
		case 1:
			break;
			// Found a Gaussian
		case 2: {
			// Some moments
			casacore::Double m0m(0);
			casacore::Double m0(0);
			casacore::Double m1(0);
			casacore::Double m2(0);
			ichi = i;
			// Do Schwarz' calculation
			casacore::Double b = deriv_p[iclo];
			casacore::Double a = (deriv_p[ichi] - b) / (ichi-iclo);
			for (casacore::Int ic=iclo; ic<=ichi; ic++) {
				m0m += casacore::min(deriv_p[ic], 0.0);
				casacore::Double wi = deriv_p[ic] - a*(ic-iclo) - b;
				m0 += wi;
				m1 += wi*ic;
				m2 += wi*ic*ic;
			};
			// determinant
			casacore::Double det = m2*m0 - m1*m1;
			if (det > 0.0 && fabs(m0m) >  FLT_EPSILON) {
				casacore::Double   xm = m1/m0;
				casacore::Double sg = 1.69*sqrt(det) / fabs(m0);
				// Width above critical?
				if (sg > sigmin_p) {
					casacore::Int is = casacore::Int(1.73*sg+0.5);
					casacore::Int im = casacore::Int(xm+0.5);
					casacore::Double yl(0);
					if ((im-is) >= 0) yl = prof(im-is);
					casacore::Double yh(0);
					if ((im + is) <= casacore::Int(lprof_p-1)) yh = prof(im+is);
					casacore::Double ym = prof(im);
                    // modified by dmehringer 2012apr03 to deal with 0 denominator
                    // 0.0/0.0 produces NaN on Linux but 0 on OSX
                    casacore::Double pg = (ym-0.5*(yh+yl));
					if (pg != 0) {
						casacore::Double denom = (1.0-exp(-0.5*(is*is)/sg/sg));
						if (denom == 0) {
							throw casacore::AipsError("Bailing because division by zero is undefined");
						}
						pg /= denom;
					}
                    // end dmehring mods
					pg = casacore::min(pg, ym);
					// cout << "pg " << pg << " cutoff " << cutoff_p << endl;
					// Above critical level? Add to list
					if (pg > cutoff_p) {
					//	cout << pg << " " << xm << " " << sg << endl;
						tspel.setAmpl(pg);
						tspel.setCenter(xm);
						tspel.setSigma(sg);
                        slist_p.insert(tspel);
					};
				};
			};
			// Next gaussian
			iclo = ichi;
			nmax--;
			break;
		}
		default:
			iclo = i+1;
			break;
		};
	};
}

template <class MT>
GaussianSpectralElement SpectralEstimate::convertElement (const casacore::Vector<MT>& x,
                                                  const GaussianSpectralElement& el) const
{
	GaussianSpectralElement elOut = el;
   const casacore::Int& idxMax = x.nelements()-1;

// Get current (pars are amp, center, width as the SpectralElement
// will always be a Gaussian)

   casacore::Vector<casacore::Double> par, err;
   el.get(par);
   el.getError(err);

// Center

   casacore::Int cenIdx = casacore::Int(par[1]);

// Get the x-increment, local to the center, as best we can from 
// the abcissa vector.  The following algorithm assumes the X
// vector is monotonic

   casacore::Double incX;
   if (cenIdx-1<0) {
      incX = x[1] - x[0];
   } else if (cenIdx+1>idxMax) {
      incX = x[idxMax] - x[idxMax-1];
   } else {
      incX = 0.5 * (x(cenIdx+1) - x(cenIdx-1));  
   }
//
   if (cenIdx<0) {
      par[1] = incX*par[1] + x[0];                 // Extrapolate from x[0]
   } else if (cenIdx>idxMax) {
      par[1] = incX*(par[1]-idxMax) + x[idxMax];   // Extrapolate from x[idxMax]
   } else {
      casacore::Double dIdx = par[1] - cenIdx;
      par[1] = x[cenIdx] + dIdx*incX;              // Interpolate
   }
   err[1] = abs(err[1] * incX);

// Width

   par[2] = abs(par[2] * incX);
   err[2] = abs(err[2] * incX);

   elOut.set(par);
   elOut.setError(err);
   return elOut;
}


} //# End namespace casa
