//# VLALogicalRecord.h: This class interprets a VLA logical record.
//# Copyright (C) 1999,2000
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

#ifndef NRAO_VLALOGICALRECORD_H
#define NRAO_VLALOGICALRECORD_H

#include <casa/Containers/Block.h>
#include <casa/Utilities/CountedPtr.h>
#include <casa/aips.h>
#include <nrao/VLA/VLAADA.h>
#include <nrao/VLA/VLAArchiveInput.h>
#include <nrao/VLA/VLACDA.h>
#include <nrao/VLA/VLAEnum.h>
#include <nrao/VLA/VLARCA.h>
#include <nrao/VLA/VLASDA.h>

#include <casa/namespace.h>

// <summary>This class interprets a VLA logical record.</summary>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <ol>
//   <li> The IO Module
// </ol>
// </prerequisite>
//
// <etymology>
// This class is designed to reconstitute VLA archive data records.
// </etymology>
//
// <synopsis> 

// This class is designed to read VLA archive data.  The data may be read from
// a disk, tape drive or any other data source supported by the IO module.  A
// call to the operator++() function assembles the next reconstructed VLA
// archive data record from the input. A reference to this data can be
// obtained using the logicalRecord function.
//
// Refer to the "VLA Archive casacore::Data casacore::Format", VLA Computer Memorandum 186
// by G.C. Hunt, K.P. Sowinski, and T.J. Bottomly; June 1993.
// (This is also available as AIPS++ note 159)
//
// The VLA archive records are always a multiple of 2048 bytes.  The
// record sizes were designed for use with magnetic tapes, so there is
// a maximum physical record size of 13*2048=26624 bytes.
//
// The low level class (blockio), that actually does the I/O, allows
// for a record (hereinafter chunk) size and for a input block size of
// a multiple of the chunk size.  The low level read operation tests
// for the number of bytes actually read from the device.
//
// The helper classes VlaDiskInput, VlaTapeInput, and VlaStdInput are
// designed to deal with the low level input from the devices in an
// analogous fashion to the ones used for casacore::FITS input.
//
// Since a read may be issued for an arbitrary number of bytes from a
// disk, the chunk multiple is arbitrary and may be used to tune the
// speed of operation.  There is an obvious trade-off between the
// block size created in the blockio class and the number of read
// operations.
//
// The story is quite different for tape input.  A read request for at
// least the maximum physical record size must be made to avoid loss of
// data.  Since a single tape record will be read with a single read
// operation, there is no point is having it any larger.  The chunk
// multiple must be exactly 13 so that the block size is 26624.
//
// The reconstitution algorithm is as follows:
//
// 1. Read a 2048 chunk from the input.
//
// The first two 16-bit integers should contain the values 1 and n,
// where n is the number of "physical records" in the current "logical  
// record."  (If the first value is not 1, then the chunk is rejected
// and a new one read until the first 16-bit value is 1.)  These two
// values are not part of the reconstituted "logical record."
//
// 2. The next 32-bit integer contains the length of the "logical
// record" in 16-bit words.  The buffer is resized so that it can
// contain the whole reconstituted "logical record."
//
// 3. The remainder of the chunk is copied to the buffer.
//
// 4. Successive chunks are read from the input.
//
// The chunks are copied into the buffer until the "logical record"
// is complete.  For "logical records" longer than 26620 byte, this is
// not the whole story.  Each "physical record" contains maximum of 13
// chunks.  When the first "physical record" of 13 chunks has been read,
// the next chunk will be the first of the next "physical record."  The
// first two 16-bit integers will now be 2 and n, to indicate that this
// is the second "physical record" of the sequence.  These 4 bytes are
// decoded and the rest of this chunk is copied to the buffer.  And so
// on...
//
// An end-of-file condition on the input will cause record processing
// to be declared complete.  
// </synopsis> 
//
// <example>
// To open and read a VLA archive data file
// <code>
//    VLAArchiveInput *in;
//    casacore::Block <casacore::Char> *buff;
//    casacore::String fileName = " ";
//    casacore::String fileType = "tape";
//
//    if (fileType == casacore::String("tape")) {
//        in = new VLAArchiveInput(fileName.chars(), VLAArchiveInput::Tape);
//    } else {
//        in = new VLAArchiveInput(fileName.chars(), VLAArchiveInput::Disk);
//    }
//
//    casacore::uInt record = 0;
//    for (buff=&(in->next()); in->atEnd()==false; buff=&(in->next()), record++) {
//        cout << "casacore::Record" << record << endl;
//        // process record pointed to by buff
//    }
// </code>
//
// </example>
//
// <motivation>
// </motivation>
//
// <todo asof="">
// <ol>
//   <li> Bulletproofing - check for realistic buffer size (<1e6)
//   <li> Bulletproofing - check newn and newm on each read
//   <li> What happens after a single end-of-file on a tape drive?
//   <li> Add record skipping
//   <li> Should it work with stdin?  This is in place but not debugged.
// </ol>
// </todo>

class VLALogicalRecord
{
public: 
  // Create an object that is not good for anything except that it can
  // be assigned to (using the assigment operator).
  VLALogicalRecord();
    
  // Create an object that reads its data from the specified VLAArchiveInput
  // object. The supplied pointer should be obtained from new and is "taken
  // over". Hence the user should not delete it.
  VLALogicalRecord(VLAArchiveInput* input);
    
  // The copy constructor uses reference semantics
  VLALogicalRecord(const VLALogicalRecord& other);

  // The destructor just cleans up.
  ~VLALogicalRecord();

  // The assignment operator uses reference semantics
  VLALogicalRecord& operator=(const VLALogicalRecord& other);

  // This returns the reconstructed VLA archive record from the input
  // stream. This casacore::ByteSource has the data stored in memory, and hence is
  // seekable. casacore::Data read from this casacore::ByteSource will have the ModComp numeric
  // conversions applied.
  casacore::ByteSource& logicalRecord();

  // Reads the next logical record from the ByteSource. Returns false if there
  // was a problem assembling the next record.
  casacore::Bool read();

  // Returns true if this class is attached to a VLAArchiveInput object.
  casacore::Bool isValid() const;

  // returns a reference to a class which can be used to interpret the data in
  // the casacore::Record Control Area of the current logical record.
  const VLARCA& RCA() const;

  // returns a reference to a class which can be used to interpret the data in
  // the Subarray casacore::Data Area of the current logical record.
  const VLASDA& SDA() const;

  // returns a reference to a class which can be used to interpret the data in
  // the Correlator casacore::Data Area of the current logical record. A logical record
  // can have have up to four correlator data areas and the argument is used to
  // specify which one you are interested in. And exception is thrown if the
  // input argument is greater than three.
  const VLACDA& CDA(casacore::uInt which) const;

  // returns a reference to a class which can be used to interpret the data in
  // the Antenna casacore::Data Area (ADA) of the current logical record. A logical
  // record has one ADA for each antenna that contributed to the data in this
  // record and the argument is used to specify which one you are interested
  // in. And exception is thrown if the input argument is not less than the
  // number of antennas in this subarray.
  const VLAADA& ADA(casacore::uInt which) const;

  // Return the polarisations of the data in the specified CDA. This
  // information is obtained from knowing what the correlator modes represent
  // and checking the IF transfer switch for the specified antennas.
  casacore::Vector<casacore::Stokes::StokesTypes> polarisations(VLAEnum::CDA cda, 
					    casacore::uInt ant1=0, casacore::uInt ant2=0) const;

private: 
  //# A pointer to a class which can read VLA Archives from a variety of
  //# sources
  casacore::CountedPtr<VLAArchiveInput> itsRecordPtr;

  //# The class which can interpret the casacore::Record Control Area portion of the
  //# logical record.
  VLARCA itsRCA;

  //# The class which can interpret the Subarray casacore::Data Area portion of the
  //# logical record.
  VLASDA itsSDA;

  //# The class which can interpret the casacore::Array casacore::Data Area portions of the
  //# logical record.
  casacore::Block<VLAADA> itsADA;

  //# The class which can interpret the Correlator casacore::Data Area portions of the
  //# logical record.
  casacore::Block<VLACDA> itsCDA;

};
#endif
