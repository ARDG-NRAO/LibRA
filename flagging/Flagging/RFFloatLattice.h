//# RFFloatLattice.h: this defines RFFloatLattice
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
#ifndef FLAGGING_RFFLOATLATTICE_H
#define FLAGGING_RFFLOATLATTICE_H
    
#include <casa/Arrays/Matrix.h> 
#include <lattices/Lattices/TempLattice.h> 
#include <lattices/Lattices/LatticeIterator.h> 
#include <vector>

namespace casa { //# NAMESPACE CASA - BEGIN


// <summary>
// RFFloatLatticeIterator: iterator over a cubic buffer
// </summary>

// <use visibility=local>

// <reviewed reviewer="" date="" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> std::vector, Matrix
// </prerequisite>
//
// <synopsis>
// See RFFloatLattice, below
// </synopsis>
//
// <todo asof="2001/04/16">
//   <li> add this feature
//   <li> fix this bug
//   <li> start discussion of this possible extension
// </todo>

class RFFloatLattice;

class RFFloatLatticeIterator
{
  private:
    // vector<bool>: is a space-efficient specialization of std::vector for the type bool
    std::vector<std::vector<bool> > *lattice;

    casacore::Matrix<casacore::Float> curs;

    unsigned int iter_pos;

    unsigned n_chan, n_ifr, /* n_time,*/ n_bit, n_corr;

    void update_curs();
  
  public:
    // default constructor creates empty iterator
    RFFloatLatticeIterator();
    
    // creates and attaches to lattice
    RFFloatLatticeIterator(std::vector<std::vector<bool> > *lat, 
			  unsigned nchan, unsigned nifr, 
			  unsigned ntime, unsigned nbit, unsigned ncorr);
    
    // destructor
    ~RFFloatLatticeIterator();

    // resets the lattice iterator to beginning, returns cursor
    casacore::Matrix<casacore::Float> * reset();
    
    // advances internal iterator to specified slot along the Z axis, returns cursor
    casacore::Matrix<casacore::Float> * advance( casacore::uInt iz );
    
    // returns position of internal iterator
    casacore::uInt position ()                 
      { return iter_pos; }
    
    //  returns internal cursor
    casacore::Matrix<casacore::Float> * cursor();
    
    // returns element at i,j of cursor
    casacore::Float & operator () ( casacore::uInt i,casacore::uInt j );

    void flush_curs();
};


// <summary>
// RFFloatLatice: a cubic lattice
// </summary>

// <use visibility=local>

// <reviewed reviewer="" date="" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> TempLattice
// </prerequisite>
//
// <synopsis>
// RFFloatLattice is a [NX,NY,NZ] vector of Matrices which 
// is iterated over the Z axis. 
// While a vector of Matrices may not be localized in memory, it has the
// advantage that the total amount of memory allocated can exceed
// the available RAM, which is probably not possible if allocated as a
// single giant block.
// Each element of the matrices is a few bits, therefore (in order to
// save memory), the full matrix is represented as a bitsequence, which
// is converted to casacore::Matrix<casacore::Float> on the fly.
//
// The buffer is no longer implemented using a casacore::TempLattice because the
// template parameter to casacore::TempLattice is restricted to certain types, and
// cannot be dynamic_bitset<>. Besides, casacore::TempLattice is currently(?)
// *not* well implemented: it creates casacore::TempLattice disk files although most
// of the RAM is free.
//
// If more memory than avilable RAM is requested, swapping will occur.
// The underlying OS probably knows better when to swap!
//
// </synopsis>
//
// <motivation>
// Many flagging agents make use of cubic lattices (typically, to maintain
// [NCHAN,NIFR,NTIME] cubes of something) in an identical way. This class
// provides a clean and convenient interface to the basic functions.
// </motivation>
//
// <templating arg=T>
//    <li> same as Matrix
// </templating>
//
// <todo asof="2001/04/16">
//   <li> add this feature
//   <li> fix this bug
//   <li> start discussion of this possible extension
// </todo>

class RFFloatLattice
{
protected:
  casacore::IPosition                              lat_shape;
  std::vector<std::vector<bool> >        lat;
  RFFloatLatticeIterator                 iter;
  unsigned n_chan, n_ifr, n_time, n_bit, n_corr;

public:
// default constructor creates empty cube
  RFFloatLattice();
// creates NX x NY x NZ cube
  RFFloatLattice( casacore::uInt nx,casacore::uInt ny,casacore::uInt nz, casacore::uInt ncorr, casacore::uInt nAgent, casacore::Int maxmem=-1 );
// creates NX x NY x NZ cube and fills with initial value
  RFFloatLattice( casacore::uInt nx,casacore::uInt ny,casacore::uInt nz, casacore::uInt ncorr, casacore::uInt nAgent, const casacore::Float &init_val,casacore::Int maxmem=-1 );
// destructor
  ~RFFloatLattice();

// creates NX x NY x NZ cube
// tile_mb is the tile size, in MB (when using paging)
  void init ( casacore::uInt nx,casacore::uInt ny,casacore::uInt nz, casacore::uInt ncorr, casacore::uInt nAgent, casacore::Int maxmem=-1,casacore::Int tile_mb=2 );
// creates NX x NY x NZ cube and fills with initial value
// tile_mb is the tile size, in MB (when using paging)
  void init ( casacore::uInt nx,casacore::uInt ny,casacore::uInt nz, casacore::uInt ncorr, casacore::uInt nAgent, const casacore::Float &init_val,casacore::Int maxmem=-1,casacore::Int tile_mb=2 );
// destroys cube
  void cleanup ();
// returns size of cube
  static casacore::uInt estimateMemoryUse ( casacore::uInt nx,casacore::uInt ny,casacore::uInt nz )
        { return nx*ny*nz*sizeof(casacore::Float)/(1024*1024) + 1; }

// resets the lattice iterator to beginning. 
  casacore::Matrix<casacore::Float> * reset( casacore::Bool will_read=true,
                     casacore::Bool will_write=true );  
  
// advances internal iterator to specified slot along the Z axis, returns cursor
  casacore::Matrix<casacore::Float> * advance( casacore::Int iz )   { return iter.advance(iz); };
  
// returns position of internal iterator
  casacore::Int position ()                 { return iter.position(); }
  
// returns shape
  const casacore::IPosition & shape ()      { return lat_shape; }

//  returns internal cursor
  casacore::Matrix<casacore::Float> * cursor()              { return iter.cursor(); }
  
// returns element at i,j of cursor
  casacore::Float & operator () ( casacore::uInt i,casacore::uInt j )  { return (*iter.cursor())(i,j); }
  
// provides access to lattice itself  
//  std::vector<std::vector<bool> > & lattice()    { return lat; }

// provides access to iterator  
  RFFloatLatticeIterator & iterator()    { return iter; }

// creates a new iterator for this lattice
  RFFloatLatticeIterator newIter();
};



} //# NAMESPACE CASA - END

#endif
