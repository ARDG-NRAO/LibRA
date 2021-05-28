//# CCList.h:
//# Copyright (C) 1998,1999
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
//# $Id$

#ifndef SYNTHESIS_CCLIST_H
#define SYNTHESIS_CCLIST_H


#include <casa/aips.h>
#include <casa/Containers/Block.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Exceptions/Error.h>
#include <casa/Utilities/Assert.h>
#include <casa/Utilities/GenSort.h>
namespace casa { //# NAMESPACE CASA - BEGIN

// <summary>
// </summary>

// <use visibility=local>   or   <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> SomeClass
//   <li> SomeOtherClass
//   <li> some concept
// </prerequisite>
//
// <etymology>
// </etymology>
//
// <synopsis>
// </synopsis>
//
// <example>
// </example>
//
// <motivation>
// </motivation>
//
// <templating arg=T>
//    <li>
//    <li>
// </templating>
//
// <thrown>
//    <li>
//    <li>
// </thrown>
//
// <todo asof="yyyy/mm/dd">
//   <li> add this feature
//   <li> fix this bug
//   <li> start discussion of this possible extension
// </todo>

class CCList
{
public:

  // vanilla constructor (npol=1, nComp=0)
  CCList();

  // more controlling constructor:
  // How many casacore::Stokes per flux casacore::Block?
  // How many spatial dimensions per position?
  // How many total components to allocate for?
  CCList(const casacore::uInt nPol, const casacore::uInt nDim=2, const casacore::uInt nComp=0);

  // copy constructor
  CCList(const CCList & other);

  // destructor
  ~CCList();

  // operator=
  CCList & operator=(const CCList & other);

  // get pointer to the casacore::Block<casacore::Int> which holds the CC's positions in pixel 
  // coordinates.  casacore::Block[nDim()*i] holds ith x, casacore::Block[nDim()*i+1] holds ith y
  // <group>
  casacore::Int * positionPtr();
  const casacore::Int * positionPtr() const;
  // </group>

  // get pointer to the whichCC'th component's position
  casacore::Int * pixelPosition(const casacore::uInt whichCC);

  // Get a pointer into the Position casacore::Block for the next free
  // clean component.
  // <group>
  casacore::Int * freePositionPtr();
  const casacore::Int * freePositionPtr() const;
  // </group>

  // get pointer to the casacore::Block<casacore::Float> which holds the CC's Flux.
  // The flux itself is a casacore::Block, as it may be 1, 2, or 4 Stokes.
  // casacore::Block[nPol()*i + iPol] holds the ith component's iPol Stokes.
  // <group>
  casacore::Float * fluxPtr();
  const casacore::Float * fluxPtr() const;
  // </group>

  // get pointer to the whichCC'th component's flux
  casacore::Float * pixelFlux(const casacore::uInt whichCC);

  // Get a pointer into the Flux casacore::Block for the next free
  // clean component.
  // <group>
  casacore::Float * freeFluxPtr();
  const casacore::Float * freeFluxPtr() const;
  // </group>

  // How many polarizations?
  casacore::uInt nPol() const;

  // How many dimentions?
  casacore::uInt nDim() const;

  // How many components have been filled into the list?
  // <group>
  casacore::uInt nComp() const;
  casacore::uInt & nComp();
  // </group>

  // How many components have been allocated for this list?
  casacore::uInt maxComp() const;

  // We have free space for how many more components?
  casacore::uInt freeComp() const;

  // Make the list bigger or smaller
  void resize(const casacore::uInt nComp);

  // add a Component; if not enough space, resize the storage blocks
  // to 2*maxComp()+1
  void addComp(const casacore::Block<casacore::Float> & flux, const casacore::Block<casacore::Int> & position);

  // casacore::Sort components to be organized by tile number.
  // If the position and Flux blocks have free space at the end, it gets cut off.
  void tiledSort(const casacore::IPosition & tileShape);

  // ok() is called to check for an internally consistent state
  // by most everything, often behind the scenes;  sometimes, 
  // the internal state is in flux
  // after a partial operation, and ok() fails; to turn off ok() checking,
  // set  itsSuspendOKCheck = true and remember to reset it to false
  // when the state-changing operation is complete.
  casacore::Bool ok() const;
private:

  // turn OFF error checking via "ok()"
  void suspendOKCheck();

  // turn ON error checking vua "ok()"
  void reactivateOKCheck();

  casacore::uInt itsPol;
  casacore::uInt itsDim;
  casacore::uInt itsComp; 
  casacore::Block<casacore::Float> itsFlux;
  casacore::Block<casacore::Int> itsPos;
  casacore::Bool itsSuspendOKCheck;
};


inline casacore::uInt CCList::nPol() const {
  DebugAssert(ok(), casacore::AipsError);
  return itsPol;
};

inline casacore::uInt CCList::nDim() const {
  DebugAssert(ok(), casacore::AipsError);
  return itsDim;
};

inline casacore::uInt CCList::nComp() const {
  DebugAssert(ok(), casacore::AipsError);
  return itsComp;
};

inline casacore::uInt & CCList::nComp() {
  DebugAssert(ok(), casacore::AipsError);
  return itsComp;
};



} //# NAMESPACE CASA - END

#endif


