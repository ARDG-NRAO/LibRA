//# CCList.cc:  implementation file for CCList
//# Copyright (C) 1996,1997,1998,1999
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

#include <synthesis/MeasurementEquations/CCList.h>
#include <casa/Arrays/Vector.h>


using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

CCList::CCList()
  :itsPol(1),
   itsDim(2),
   itsComp(0),
   itsFlux(),
   itsPos(),
   itsSuspendOKCheck(false)
{
  DebugAssert(ok(), AipsError);
};

CCList::CCList(const uInt nPol, const uInt nDim, const uInt nComp)
  :itsPol(nPol),
   itsDim(nDim),
   itsComp(0),
   itsFlux(nPol*nComp),
   itsPos(nDim*nComp),
   itsSuspendOKCheck(false)
{
  DebugAssert(ok(), AipsError);
};

CCList::CCList(const CCList & other)
  :itsPol(other.itsPol),
   itsDim(other.itsDim),
   itsComp(other.itsComp),
   itsFlux(other.itsFlux),
   itsPos(other.itsPos),
   itsSuspendOKCheck(false)
{
  DebugAssert(ok(), AipsError);
};

CCList::~CCList() {
  DebugAssert(ok(), AipsError);
};

CCList & CCList::operator=(const CCList & other){
  if (this != &other) {
    itsPol = other.itsPol;
    itsDim = other.itsDim;
    itsComp = other.itsComp;
    itsFlux = other.itsFlux;
    itsPos = other.itsPos;
    itsSuspendOKCheck = other.itsSuspendOKCheck;
  }
  DebugAssert(ok(), AipsError);
  return *this;
};

Int * CCList::positionPtr() {
  DebugAssert(ok(), AipsError)
  return itsPos.storage();
};

const Int * CCList::positionPtr() const {
  DebugAssert(ok(), AipsError);
  return itsPos.storage();
};

Float * CCList::fluxPtr() {
  DebugAssert(ok(), AipsError);
  return itsFlux.storage();
};

const Float * CCList::fluxPtr() const {
  DebugAssert(ok(), AipsError);
  return itsFlux.storage();
};

Int * CCList::freePositionPtr() {
  DebugAssert(ok(), AipsError);
  return itsPos.storage() + nComp()*nDim();
};

const Int * CCList::freePositionPtr() const {
  DebugAssert(ok(), AipsError);
  return itsPos.storage() + nComp()*nDim();
};

Float * CCList::freeFluxPtr() {
  DebugAssert(ok(), AipsError);
  return itsFlux.storage() + nComp()*nPol();
};

const Float * CCList::freeFluxPtr() const {
  DebugAssert(ok(), AipsError);
  return itsFlux.storage() + nComp()*nPol();
};

uInt CCList::maxComp() const {
  DebugAssert(ok(), AipsError)
  return itsPos.nelements()/nDim();
};

uInt CCList::freeComp() const {
  DebugAssert(ok(), AipsError);
  return maxComp() - nComp();
};

void CCList::resize(const uInt newSize) {
  suspendOKCheck();
  Block<Int>  pr(itsPos);
  Block<Float> fr(itsFlux);
  itsPos.resize(nDim()*newSize);
  itsFlux.resize(nPol() * newSize);
  for (uInt i=0; i< nComp()*nDim(); i++) {
    itsPos[i] = pr[i];
  }
  for (uInt i=0; i< nComp()*nPol(); i++) {
    itsFlux[i] = fr[i];
  }
  reactivateOKCheck();

  DebugAssert(ok(), AipsError);
};

void CCList::addComp(const Block<Float> & flux, const Block<Int> & position) {

  if (nComp() >= maxComp() ) {
    resize(2*nComp()+1);
  }  
  Int * pos_p = freePositionPtr();
  Float * flux_p = freeFluxPtr();
  uInt n = flux.nelements();
  for (uInt i=0;i<n;i++) {
    *(flux_p+i) = flux[i];
  }
  *(pos_p) = position[0];
  *(pos_p+1) = position[1];
  nComp()++;
};


Bool CCList::ok() const {
  if (itsSuspendOKCheck) {
    return true;
  }
  // Be arbitary: disallow more than 4-D Clean Components
  if ((itsDim < 1) || (itsDim > 4)) {  
    return false;
  }
  if ((itsPol != 1) && (itsPol != 2) && (itsPol != 4)) {
    return false;
  }
  if (itsComp*itsDim > itsPos.nelements()) {
    return false;
  }
  if (itsComp*itsPol > itsFlux.nelements()) {
    return false;
  }
  if (itsPos.nelements()/itsDim != itsFlux.nelements()/itsPol) {
    return false;
  }
  return true;
};

Int * CCList::pixelPosition(const uInt whichPixel) {
  DebugAssert(whichPixel < nComp(), AipsError);
  DebugAssert(ok(), AipsError);
  return itsPos.storage() + whichPixel*nDim();
};

Float * CCList::pixelFlux(const uInt whichPixel) {
  DebugAssert(whichPixel < nComp(), AipsError);
  DebugAssert(ok(), AipsError);
  return itsFlux.storage() + whichPixel*nPol();
};

void  CCList::tiledSort(const IPosition & tileShape) {
  if (nComp() == 0) {
    return;
  }
  Vector<uInt> index(nComp());
  Block<Int> tileNumber(nComp());
  const Int tx = tileShape(0);
  const Int ty = tileShape(1);
  //  const Int tileArea = tx * tileShape(1);
  Int * posPtr;
  Int x, y;
  Int it, jt;
  for (uInt c = 0; c < nComp(); c++) {
    posPtr = pixelPosition(c);
    x = *posPtr;
    posPtr++;
    y = *posPtr;
    //    tileNumber[c] = (x + tx * y) % tileArea;   old algorithm
    it = (Int)(x/tx);
    jt = (Int)(y/ty);
    tileNumber[c] = jt*10000 + it;
  }

  AlwaysAssert(genSort(index, tileNumber) == nComp(), AipsError);

  Int * sortedPos = new Int[nDim()*nComp()];
  Float * sortedFlux = new Float[nPol()*nComp()];
  uInt whichElem, newElem;
  for (uInt c = 0; c < nComp(); c++) {
    whichElem = nDim()*index(c);
    newElem = nDim()*c;
    sortedPos[newElem] = itsPos[whichElem];
    whichElem++; newElem++;
    sortedPos[newElem] = itsPos[whichElem];
    whichElem = nPol()*index(c);
    newElem = nPol()*c;
    for (uInt k = 0; k < nPol(); k++) {
      sortedFlux[newElem] = itsFlux[whichElem];
      whichElem++; newElem++;
    }
  }

  suspendOKCheck();
  itsPos.replaceStorage((nDim()*nComp()), sortedPos);
  itsFlux.replaceStorage((nPol()*nComp()), sortedFlux);
  reactivateOKCheck();
  ok();
};

void CCList::suspendOKCheck() {
  itsSuspendOKCheck = true;
};

void CCList::reactivateOKCheck() {
  itsSuspendOKCheck = false;
};


} //# NAMESPACE CASA - END

