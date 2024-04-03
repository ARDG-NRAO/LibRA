//# VLACDA.cc:
//# Copyright (C) 1999,2000,2001,2002
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

#include <nrao/VLA/VLACDA.h>
#include <nrao/VLA/VLAContinuumRecord.h>
#include <nrao/VLA/VLASpectralLineRecord.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/Utilities/Assert.h>

VLACDA::VLACDA()
  :itsRecord(),
   itsOffset(0),
   itsBaselineSize(0),
   itsNant(0),
   itsNchan(0),
   itsACorr(0),
   itsXCorr(0)
{
  //  cout << "VLACDA::VLACDA()" << endl;
}

VLACDA::VLACDA(ByteSource& record, uInt offset, uInt baselineSize,
	       uInt nant, uInt nchan)
  :itsRecord(record),
   itsOffset(offset),
   itsBaselineSize(baselineSize),
   itsNant(nant),
   itsNchan(nchan),
   itsACorr(itsNant, (VLABaselineRecord*)0),
   itsXCorr(itsNant*(itsNant-1)/2, (VLABaselineRecord*)0)
{
  //  cout << "VLACDA::VLACDA(...)" << endl;
  DebugAssert(record.isNull() || record.isReadable(), AipsError);
  DebugAssert(record.isNull() || record.isSeekable(), AipsError);
}

VLACDA::VLACDA(const VLACDA& other)
  :itsRecord(),
   itsOffset(0),
   itsBaselineSize(0),
   itsNant(0),
   itsNchan(0),
   itsACorr(0),
   itsXCorr(0)
{
  //  cout << "VLACDA::VLACDA(const VLACDA& other)" << endl;
  DebugAssert(other.itsRecord.isNull() || 
	      other.itsRecord.isReadable(), AipsError);
  DebugAssert(other.itsRecord.isNull() ||
	      other.itsRecord.isSeekable(), AipsError);
  attach(other.itsRecord, other.itsOffset, other.itsBaselineSize,
 	 other.itsNant, other.itsNchan);
}

VLACDA::~VLACDA() {
  //  cout << "~VLACDA::VLACDA()" << endl;
  deleteACorr(0);
  deleteXCorr(0);
}

VLACDA& VLACDA::operator=(const VLACDA& other) {
  //  cout << "VLACDA::operator=(const VLACDA& other)" << endl;
  if (this != &other) {
    DebugAssert(other.itsRecord.isNull() ||
		other.itsRecord.isReadable(), AipsError);
    DebugAssert(other.itsRecord.isNull() ||
		other.itsRecord.isSeekable(), AipsError);
    attach(other.itsRecord, other.itsOffset, other.itsBaselineSize,
	   other.itsNant, other.itsNchan);
  }
  return *this;
}

void VLACDA::deleteACorr(uInt start) {
  for (uInt a = start; a < itsNant; a++) {
    VLABaselineRecord*& ptr = itsACorr[a];
    if (ptr != 0) {
      delete ptr;
      ptr = 0;
    }
  }
}

void VLACDA::deleteXCorr(uInt start) {
  const uInt nCorr = itsNant*(itsNant-1)/2;
  for (uInt a = start; a < nCorr; a++) {
    VLABaselineRecord*& ptr = itsXCorr[a];
    if (ptr != 0) {
      delete ptr;
      ptr = 0;
    }
  }
}

void VLACDA::attach(ByteSource& newRecord, uInt newOffset,
		    uInt newBaselineSize, uInt newNant, uInt newChan) {
  //  cout << "VLACDA::attach(...)" << endl;
  DebugAssert(newRecord.isNull() || newRecord.isReadable(), AipsError);
  DebugAssert(newRecord.isNull() || newRecord.isSeekable(), AipsError);
  itsRecord = newRecord;
  itsOffset = newOffset;
  itsBaselineSize = newBaselineSize;
  // pointers are only deleted if newNant < itsNant;
  deleteACorr(newNant);
  const uInt newNcorr = (newNant*(newNant-1))/2;
  deleteXCorr(newNcorr);
  itsACorr.resize(newNant, false, true);
  itsXCorr.resize(newNcorr, false, true);
  for (uInt a = itsNant; a < newNant; a++) { // only done if newNant > itsNant
    itsACorr[a] = 0;
  }
  for (uInt a = itsNant*(itsNant-1)/2; a < newNcorr; a++) {
    itsXCorr[a] = 0;
  }
  itsNant = newNant;

  const uInt xCorrOffset = itsOffset + itsBaselineSize*itsNant;
  if (newChan == 1 && itsNchan == 1) {
    for (uInt a = 0; a < itsNant; a++) {
      DebugAssert(itsACorr[a] == 0 ||
		  itsACorr[a]->type() == VLABaselineRecord::CONTINUUM, 
		  AipsError);
      VLAContinuumRecord*& ptr = (VLAContinuumRecord*&) itsACorr[a];
      if (ptr != 0) {
	ptr->attach(itsRecord, itsOffset+itsBaselineSize*a);
      }
    }
    for (uInt a = 0; a < newNcorr; a++) {
      DebugAssert(itsXCorr[a] == 0 ||
		  itsXCorr[a]->type() == VLABaselineRecord::CONTINUUM, 
		  AipsError);
      VLAContinuumRecord*& ptr = (VLAContinuumRecord*&) itsXCorr[a];
      if (ptr != 0) {
	ptr->attach(newRecord, xCorrOffset+itsBaselineSize*a);
      }
    }
  } else if (newChan > 1 && itsNchan > 1) {
    for (uInt a = 0; a < itsNant; a++) {
      DebugAssert(itsACorr[a] == 0 || 
		  itsACorr[a]->type() == VLABaselineRecord::SPECTRALLINE, 
		  AipsError);
      VLASpectralLineRecord*& ptr = (VLASpectralLineRecord*&) itsACorr[a];
      if (ptr != 0) {
	ptr->attach(itsRecord, itsOffset+itsBaselineSize*a, newChan);
      }
    }
    for (uInt a = 0; a < newNcorr; a++) {
      DebugAssert(itsXCorr[a] == 0 ||
		  itsXCorr[a]->type() == VLABaselineRecord::SPECTRALLINE, 
		  AipsError);
      VLASpectralLineRecord*& ptr = (VLASpectralLineRecord*&) itsXCorr[a];
      if (ptr != 0) {
	ptr->attach(newRecord, xCorrOffset+itsBaselineSize*a, newChan);
      }
    }
  } else { // delete the Baseline records. They get recreated when needed.
    deleteACorr(0);
    deleteXCorr(0);
  }
  itsNchan = newChan;
}

Bool VLACDA::isValid() const {
  return (itsOffset != 0);
}

const VLABaselineRecord& VLACDA::autoCorr(uInt which) const {
  DebugAssert(which < itsACorr.nelements(), AipsError);
  if (itsACorr[which] == 0) {
    if (itsNchan == 1) {
      itsACorr[which] = 
	new VLAContinuumRecord(itsRecord, itsOffset+itsBaselineSize*which);
    } else {
      itsACorr[which] = 
	new VLASpectralLineRecord(itsRecord, itsOffset+itsBaselineSize*which,
				  itsNchan);
    }
  }
  return *itsACorr[which];
}

const VLABaselineRecord& VLACDA::crossCorr(uInt which) const {
  DebugAssert(which < itsXCorr.nelements(), AipsError);
  if (itsXCorr[which] == 0) {
    const uInt xCorrOffset = itsBaselineSize*itsNant + itsOffset;
    if (itsNchan == 1) {
      itsXCorr[which] = 
	new VLAContinuumRecord(itsRecord, xCorrOffset+itsBaselineSize*which);
    } else {
      itsXCorr[which] = 
	new VLASpectralLineRecord(itsRecord, xCorrOffset+itsBaselineSize*which,
				  itsNchan);
    }
  }
  return *itsXCorr[which];
}
// Local Variables: 
// compile-command: "gmake VLACDA"
// End: 
