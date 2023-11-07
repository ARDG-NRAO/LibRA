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
#include <casacore/lattices/Lattices/LatticeStepper.h>
#include <flagging/Flagging/RFFloatLattice.h>
#include <flagging/Flagging/RFCommon.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

RFFloatLatticeIterator::RFFloatLatticeIterator ()
  : n_chan(0), n_ifr(0), /* n_time(0),*/ n_bit(0), n_corr(0)
{
  iter_pos = 0;
  curs = Matrix<Float>();
  lattice = NULL;
}

 RFFloatLatticeIterator::RFFloatLatticeIterator(std::vector<std::vector<bool> > *lat,
								   unsigned nchan, unsigned nifr, 
								   unsigned /* ntime */, unsigned nbit,
								   unsigned ncorr)
   : n_chan(nchan), n_ifr(nifr), /* n_time(ntime),*/ n_bit(nbit), n_corr(ncorr)

{
  iter_pos = 0;
  lattice = lat;
  update_curs();
}

RFFloatLatticeIterator::~RFFloatLatticeIterator()
{
  flush_curs();
}

/* Update cursor from buffer, convert from bitset to Matrix<uInt> */

/*
    The format is like this:

    [ ... | agent1 | agent0 | corrN | ... | corr1 | corr0 ]

    i.e. the length is number of agents + number of correlations.

    Except (don't ask why) if nCorr = 1, the format is

    [ ... | agent1 | agent0 | notUsed | corr0 ]

*/

void RFFloatLatticeIterator::update_curs()
{
  if (lattice != NULL && iter_pos < lattice->size()) {
    curs.resize(IPosition(2, n_chan, n_ifr));

    //curs = (*lattice)[iter_pos];

    std::vector<bool> &l = (*lattice)[iter_pos];

    std::vector<bool> val(n_bit);
    if (n_corr == 1) {
      val.resize(n_bit+1);
    }

    for (unsigned ifr = 0; ifr < n_ifr; ifr++) {
      for (unsigned chan = 0; chan < n_chan; chan++) {
	if (n_corr <= 1) {
	  /* write corr0 */
	  unsigned indx = 0 + n_bit*(chan + n_chan*ifr);
	  val[0] = l[indx];

	  /* write agents starting from b[2] */
	  for (unsigned b = 1; b < n_bit; b++) {
	    indx = b + n_bit*(chan + n_chan*ifr);
	    val[b+1] = l[indx];
	  }
	}
	else {
	  unsigned indx;
	  for (unsigned b = 0; b < n_bit; b++) {
	    indx = b + n_bit*(chan + n_chan*ifr);
	    
	    val[b] = l[indx]; 
	  }
        }

        curs(chan, ifr) = bitvec_to_ulong(val);
      }
    }
  }
}

/* Write cursor back to buffer, convert from Matrix<uInt> to bitset.
   See above for an explanation of the format.
*/
void RFFloatLatticeIterator::flush_curs()
{
  if (lattice != NULL && iter_pos < lattice->size()) {

    //(*lattice)[iter_pos] = curs;
    std::vector<bool> &l = (*lattice)[iter_pos];
    
    std::vector<bool> val(n_bit+1);

    for (unsigned ifr = 0; ifr < n_ifr; ifr++) {
      for (unsigned chan = 0; chan < n_chan; chan++) {

	unsigned c = (unsigned) curs(chan, ifr);
	for (unsigned i = 0; i < n_bit+1; i++) {
	  val[i] = (c & 1);
	  c = c >> 1;
	}

	if (n_corr <= 1) {
	  unsigned indx = 0 + n_bit*(chan + n_chan*ifr);
	  l[indx] = val[0];

	  for (unsigned b = 1; b < n_bit; b++) {
	    indx = b + n_bit*(chan + n_chan*ifr);
	    l[indx] = val[b+1];
	  }
	}
	else {
	  unsigned indx;
	  for (unsigned b = 0; b < n_bit; b++) {
	    indx = b + n_bit*(chan + n_chan*ifr);
	    
	    l[indx] = val[b];
	  }
	}
      }
    }
  }
}

Matrix<Float> * RFFloatLatticeIterator::reset()
{
  if (iter_pos != 0) flush_curs();
  iter_pos = 0;
  update_curs();
  return cursor();
}

Matrix<Float> * RFFloatLatticeIterator::advance(uInt t1)
{
  if (iter_pos != t1) flush_curs();
  iter_pos = t1;
  update_curs();
  return cursor();
}

Matrix<Float> * RFFloatLatticeIterator::cursor()
{
  return &curs;
  //return &((*lattice)[iter_pos]);
}

Float & RFFloatLatticeIterator::operator()(uInt i,uInt j)
{ 
  return curs(i, j);
  //return (*lattice)[iter_pos](i, j);
}




RFFloatLattice::RFFloatLattice ()
{
}

RFFloatLattice::RFFloatLattice ( uInt nchan,
                                 uInt nifr,
                                 uInt ntime,
                                 uInt ncorr,
                                 uInt nAgent,
                                 Int maxmem )
{
  init(nchan, nifr, ntime, ncorr, nAgent, maxmem);
}
RFFloatLattice::RFFloatLattice ( uInt nchan,
                                 uInt nifr,
                                 uInt ntime,
                                 uInt ncorr,
                                 uInt nAgent,
                                 const Float &init_val,
                                 Int maxmem )
{
    init(nchan, nifr, ntime, ncorr, nAgent, init_val, maxmem);
}
RFFloatLattice::~RFFloatLattice ()
{
  cleanup();
}

void 
RFFloatLattice::init(uInt nchan,
                     uInt nifr,
                     uInt ntime,
                     uInt ncorr,
                     uInt nAgent,
                     Int /*maxmem*/,
                     Int /*tile_mb*/)
{
  n_bit = ncorr + nAgent;
  
  if (n_bit > 32) {
      stringstream ss;
      ss << 
          "Sorry, too many polarizations (" << ncorr <<
          ") and agents (" << nAgent << "). Max supported number is 32 in total.";
      cerr << ss.str();
      abort();
      throw AipsError(ss.str());
  }
  
  lat_shape = IPosition(3,nchan,nifr,ntime);
  
  lat = std::vector<std::vector<bool> >(ntime);
  for (unsigned i = 0; i < ntime; i++) {
      //lat[i] = Matrix<Float>(IPosition(2, nchan, nifr));
      lat[i] = std::vector<bool>(nchan * nifr * (ncorr+nAgent));
  }
  
  iter = RFFloatLatticeIterator(&lat, nchan, nifr, ntime, ncorr+nAgent, ncorr);
}

RFFloatLatticeIterator RFFloatLattice::newIter()
{
  return RFFloatLatticeIterator(&lat, n_chan, n_ifr, n_time, n_bit, n_corr);
}

void RFFloatLattice::init(uInt nchan,
                                            uInt nifr,
                                            uInt ntime,
                                            uInt ncorr,
                                            uInt nAgent,
                                            const Float &init_val,
                                            Int maxmem,
                                            Int tile_mb)
{
  n_chan = nchan;
  n_ifr = nifr;
  n_time = ntime;
  n_bit = ncorr + nAgent;
  n_corr = ncorr;
  init(nchan, nifr, ntime, ncorr, nAgent, maxmem, tile_mb);
  //  lat.set(init_val);

  uInt nbits = ncorr + nAgent;

  /* Write init_val to every matrix element.
     See above for description of format */
  std::vector<bool> val = bitvec_from_ulong(reinterpret_cast<const unsigned &>(init_val),nbits+1);
  for (unsigned i = 0; i < ntime; i++) {
    for (unsigned chan = 0; chan < nchan; chan++) {
      for (unsigned ifr = 0; ifr < nifr; ifr++) {
	if (n_corr <= 1) {
	  unsigned indx = 0 + n_bit*(chan + n_chan*ifr);
	  lat[i][indx] = val[0];

	  for (unsigned b = 1; b < n_bit; b++) {
	    indx = b + n_bit*(chan + n_chan*ifr);
	    lat[i][indx] = val[b+1];
	  }
	}
	else {
	  unsigned indx;
	  for (unsigned b = 0; b < n_bit; b++) {
	    indx = b + n_bit*(chan + n_chan*ifr);
	    
	    lat[i][indx] = val[b];
	  }
	}
      }
    }
  }
}

void RFFloatLattice::cleanup ()
{
  iter.flush_curs();
  iter.cursor()->resize(IPosition(2, 0, 0));
  iter = RFFloatLatticeIterator();
  lat.resize(0);
  lat_shape.resize(0);
}

Matrix<Float> * RFFloatLattice::reset ( Bool /*r*/,Bool /*w*/ )
{
  return iter.reset();
}


} //# NAMESPACE CASA - END

