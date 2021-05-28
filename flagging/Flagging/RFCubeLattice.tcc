//# RFCubeLattice.cc: this defines RFCubeLattice
//# Copyright (C) 2000,2001
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
//# $Id$
#include <lattices/Lattices/LatticeStepper.h>
#include <flagging/Flagging/RFCubeLattice.h>
#include <flagging/Flagging/RFCommon.h>

namespace casa { //# NAMESPACE CASA - BEGIN

template<class T> RFCubeLatticeIterator<T>::RFCubeLatticeIterator ()
  : n_chan(0), n_ifr(0), n_time(0), n_bit(0), n_corr(0)
{
  iter_pos = 0;
  //curs = casacore::Matrix<T>();
  lattice = NULL;
}

 template<class T> RFCubeLatticeIterator<T>::RFCubeLatticeIterator(std::vector<std::vector<bool> > *lat,
								   unsigned nchan, unsigned nifr, 
								   unsigned ntime, unsigned nbit,
								   unsigned ncorr)
   : n_chan(nchan), n_ifr(nifr), n_time(ntime), n_bit(nbit), n_corr(ncorr)

{
  iter_pos = 0;
  lattice = lat;
}

template<class T> RFCubeLatticeIterator<T>::~RFCubeLatticeIterator()
{
}

/*
    The format is like this:

    [ ... | agent1 | agent0 | corrN | ... | corr1 | corr0 ]

    i.e. the length is number of agents + number of correlations.

    Except (don't ask why) if nCorr = 1, the format is

    [ ... | agent1 | agent0 | notUsed | corr0 ]

*/


template<class T> void RFCubeLatticeIterator<T>::reset()
{
  iter_pos = 0;
  return;
}

template<class T> void RFCubeLatticeIterator<T>::advance(casacore::uInt t1)
{
  iter_pos = t1;
  return;
}

template<class T> T
RFCubeLatticeIterator<T>::operator()(casacore::uInt chan, casacore::uInt ifr) const
{ 
    T val = 0;
    
    std::vector<bool> &l = (*lattice)[iter_pos];

    if (n_bit == 2) {
        unsigned indx = n_bit*(chan + n_chan*ifr);
        val = l[indx] + 4*l[indx+1];
    }
    else if (n_corr <= 1) {
      /* write corr0 */
      unsigned indx = 0 + n_bit*(chan + n_chan*ifr);
      if (l[indx]) {
        val |= 1;
      }
      
      /* write agents starting from b[2] */
      for (unsigned b = 1; b < n_bit; b++) {
        indx++;
        if (l[indx]) {
          val |= 1 << (b+1);
        }
      }
    }
    else {
      unsigned indx = n_bit*(chan + n_chan*ifr);
      for (unsigned b = 0; b < n_bit; b++) {
        if (l[indx++]) {
          val |= 1 << b;
        }
      }
    }
    
    return val;
}

template<class T> void 
RFCubeLatticeIterator<T>::set( casacore::uInt chan, casacore::uInt ifr, const T &val )
{
    std::vector<bool> &l = (*lattice)[iter_pos];

    if (n_bit == 2) {
      unsigned indx = n_bit*(chan + n_chan*ifr);
      l[indx] = val & 1;
      l[indx+1] = val & 4;
    }
    else if (n_corr <= 1) {
      unsigned indx = 0 + n_bit*(chan + n_chan*ifr);
      l[indx] = val & 1;
      
      for (unsigned b = 1; b < n_bit; b++) {
	indx++;
	l[indx] = val & (1<<(b+1));
      }
    }
    else {
      unsigned indx = n_bit*(chan + n_chan*ifr);
      for (unsigned b = 0; b < n_bit; b++) {
	l[indx++] = val & (1<<b);
      }
    }
    
    return;
}


template<class T> void
RFCubeLatticeIterator<T>::set( casacore::uInt ichan, 
                               casacore::uInt ifr, 
                               casacore::uInt icorr, 
                               bool val)
{
  std::vector<bool> &l = (*lattice)[iter_pos];

  unsigned indx = icorr + n_bit*(ichan + n_chan*ifr);
    
  l[indx] = val;
  
  return;
}



template<class T> RFCubeLattice<T>::RFCubeLattice ()
{
}

template<class T> RFCubeLattice<T>::RFCubeLattice ( casacore::uInt nchan,
                                                    casacore::uInt nifr,
                                                    casacore::uInt ntime,
                                                    casacore::uInt ncorr,
                                                    casacore::uInt nAgent)
{
  init(nchan, nifr, ntime, ncorr, nAgent);
}
template<class T> RFCubeLattice<T>::RFCubeLattice ( casacore::uInt nchan,
                                                    casacore::uInt nifr,
                                                    casacore::uInt ntime,
                                                    casacore::uInt ncorr,
                                                    casacore::uInt nAgent,
                                                    const T &init_val)
{
  init(nchan, nifr, ntime, ncorr, nAgent, init_val);
}

template<class T> RFCubeLattice<T>::~RFCubeLattice ()
{
  cleanup();
}

template<class T> void
RFCubeLattice<T>::init(casacore::uInt nchan,
                       casacore::uInt nifr,
                       casacore::uInt ntime,
		       casacore::uInt ncorr,
		       casacore::uInt nAgent)
{
  n_bit = ncorr + nAgent;

  if (n_bit > 32) {
    std::ostringstream ss;
    ss << 
      "Sorry, too many polarizations (" << ncorr <<
      ") and agents (" << nAgent << "). Max supported number is 32 in total.";
    std::cerr << ss.str();
    throw casacore::AipsError(ss.str());
  }

  lat_shape = casacore::IPosition(3, nchan, nifr, ntime);

  lat = std::vector<std::vector<bool> >(ntime);
  for (unsigned i = 0; i < ntime; i++) {
    lat[i] = std::vector<bool>(nchan * nifr * (ncorr+nAgent));
  }

  iter = RFCubeLatticeIterator<T>(&lat, nchan, nifr, ntime, ncorr+nAgent, ncorr);
}

template<class T> RFCubeLatticeIterator<T> RFCubeLattice<T>::newIter()
{
  return RFCubeLatticeIterator<T>(&lat, n_chan, n_ifr, n_time, n_bit, n_corr);
}

template<class T> void RFCubeLattice<T>::init(casacore::uInt nchan,
                                              casacore::uInt nifr,
                                              casacore::uInt ntime,
					      casacore::uInt ncorr,
					      casacore::uInt nAgent,
                                              const T &init_val)
{
  n_chan = nchan;
  n_ifr = nifr;
  n_time = ntime;
  n_bit = ncorr + nAgent;
  n_corr = ncorr;
  init(nchan, nifr, ntime, ncorr, nAgent);

  casacore::uInt nbits = ncorr + nAgent;

  /* Write init_val to every matrix element.
     See above for description of format */
  std::vector<bool> val = bitvec_from_ulong( (unsigned) init_val, nbits+1 );
  for (unsigned i = 0; i < ntime; i++) {
    if (n_bit == 2) {
        unsigned indx = 0;
        bool v0 = val[0];
        bool v2 = val[2];
        while (indx < 2*n_chan*n_ifr) {
          lat[i][indx++] = v0;
          lat[i][indx++] = v2;
        } 
    } 
    else {
      for (unsigned ifr = 0; ifr < nifr; ifr++) {
        for (unsigned chan = 0; chan < nchan; chan++) {
         if (n_corr <= 1) {
	  unsigned indx = 0 + n_bit*(chan + n_chan*ifr);
	  lat[i][indx] = val[0];

	  for (unsigned b = 1; b < n_bit; b++) {
	    indx++;
	    lat[i][indx] = val[b+1];
	  }
	}
	else {
	  unsigned indx = n_bit*(chan + n_chan*ifr);
	  for (unsigned b = 0; b < n_bit; b++) {
	    lat[i][indx++] = val[b];
	  }
	}
      }
    }
  }
}
}

template<class T> void
RFCubeLattice<T>::set_column( casacore::uInt ifr, const T &val )
{
  for (unsigned chan = 0; chan < n_chan; chan++) {
    set(chan, ifr, val);
  }
  return;
}

template<class T> void RFCubeLattice<T>::cleanup ()
{
  iter = RFCubeLatticeIterator<T>();
  lat.resize(0);
  lat_shape.resize(0);
}

template<class T> void RFCubeLattice<T>::reset ()
{
  iter.reset();
}


} //# NAMESPACE CASA - END

