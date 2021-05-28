//# FluxCalcLogFreqPolynomial.tcc 
//# Copyright (C) 2010
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify
//# it under the terms of the GNU General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or
//# (at your option) any later version.
//#
//# This library is distributed in the hope that it will be useful,
//# but WITHOUT ANY WARRANTY; without even the implied warranty of
//# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//# GNU General Public License for more details.
//# 
//# You should have received a copy of the GNU General Public License
//# along with this library; if not, write to the Free Software
//# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
#include <components/ComponentModels/FluxCalcLogFreqPolynomial.h>
#include <casa/Arrays/Vector.h>
#include <scimath/Mathematics/RigidVector.h>

namespace casa { //# NAMESPACE CASA - BEGIN

template <casacore::Int lford, casacore::Int errord>
void FluxCalcLogFreqPolynomial::fill_coeffs(const casacore::RigidVector<casacore::Float, lford>& lfrv,
                                            const casacore::RigidVector<casacore::Float, errord>& errrv)
{
  coeffs_p(0) = lfrv.vector();
  coeffs_p(1) = errrv.vector();
}

template <casacore::Int lford>
void FluxCalcLogFreqPolynomial::fill_coeffs(const casacore::RigidVector<casacore::Float, lford>& lfrv)
{
  coeffs_p(0) = lfrv.vector();
}

template <casacore::Int lford1, casacore::Int lford2>
void FluxCalcLogFreqBrokenPolynomial::fill_lohi_coeffs(const casacore::RigidVector<casacore::Float, lford1> & lorv,
                                                       const casacore::MFrequency& break_freq,
                                                       const casacore::RigidVector<casacore::Float, lford2>& hirv)
{
  low_coeffs_p = lorv.vector();
  break_freq_p = break_freq;
  high_coeffs_p = hirv.vector();
  fill_coeffs(low_coeffs_p);
  in_low_state_p = true;
}

} //# NAMESPACE CASA - END
