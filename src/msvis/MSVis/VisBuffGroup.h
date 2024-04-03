//# VisBuffGroup.h: class to store a group of VisBuffers
//# Copyright (C) 2011
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

#ifndef MSVIS_VISBUFFGROUP_H
#define MSVIS_VISBUFFGROUP_H

#include <casacore/casa/aips.h>
#include <msvis/MSVis/VisBuffer.h>
#include <casacore/casa/Arrays/Vector.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary>
// A class to store a group of VisBuffers.
// </summary>
//
// <use visibility=export>
//
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> VisBuffer
// </prerequisite>
//
// <etymology>
//   It is a group of VisBuffers
// </etymology>
//
// <synopsis>
// This class stores a group of VisBuffers.  Unlike VisBuffGroupAcc or
// VisBuffAccumulator, the VisBuffers are copied and stored in a nearly
// pristine state so that their individual contents can be accessed later
// (i.e. without any averaging).  The only difference between a normal (or
// "fresh") VisBuffer and one retrieved from a VisBuffGroup is that
// VisBuffGroup detaches its VisBuffers from any underlying VisIter.
// </synopsis>
//
// <example>
// </example>
//
// <motivation>
// Some calculations or applications need more than one VisBuffer.  For
// example, one might want to estimate and subtract the continuum from the
// visibilities of an casacore::MS that has a broad line completely spanning spw 1, but
// spws 0 and 2 line-free, so the spws should be combined (combine='spw') for
// the continuum estimation.
//
// It is much more efficient if the group of necessary data can be read only
// once, worked on, and then written.  The CalTable approach is more flexible
// in that a CalTable can be applied to an casacore::MS with a different number or
// arrangement of rows from the input casacore::MS, but per chunk it requires two more
// reads (the CalTable and the _output_ casacore::MS) and an extra write (the CalTable).
// </motivation>
//
// <todo asof="2011/11/07">
//   <li> Allow retrieval by timestamp.
// </todo>

class VisBuffGroup
{
public:
  VisBuffGroup();

  // Null destructor
  ~VisBuffGroup();

  // Add a VisBuffer.  Returns true on success and false on failure.
  casacore::Bool store(const VisBuffer& vb);

  // casacore::Record the end of a chunk.
  // Doing so marks that if "playing back" a VisIter vi matching the order that
  // the VBs were stored in, vi.nextChunk() will be needed at this point
  // instead of ++vi.
  void endChunk();

  // (See endChunk())  Returns whether or not vi.nextChunk() should be used
  // when advancing past buffer number buf.
  casacore::Bool chunkEnd(const casacore::Int buf) const {return endChunk_p[buf];}

  // Replace the VisBuffer in slot buf with vb.
  // Returns true on success and false on failure.
  // casacore::Bool replace(const VisBuffer& vb, const casacore::uInt buf);

  // How many VisBuffers are contained herein?
  casacore::uInt nBuf() const {return nBuf_p;}

  // casacore::uInt nDDID() const {return nDDID_p;}
  // casacore::uInt nFld()  const {return nFld_p;}

  // Return a reference to the indexed VisBuffer.
  // Throws an exception if buf > nBuf().
  VisBuffer& operator()(const casacore::Int buf);

  // // Returns the buffer index corresponding to data description ID ddid and
  // // field ID fld.
  // //
  // // Returns -1 if there is no such buffer.
  // //
  // // Returns -(# of matching buffers) if there is > 1 match (use bufInds).
  // casacore::Int bufInd(const casacore::Int ddid, const casacore::Int fld);

  // // Returns the buffer indices corresponding to data description ID ddid and
  // // field ID fld.
  // casacore::Vector<casacore::Int> bufInds(const casacore::Int ddid, const casacore::Int fld)

  // The flagging approach to channel selection.
  // Sets chanmaskedflags to true wherever the channels in chanmask or flags in
  // vb.flagCube() are true, resizing if necessary.
  // Returns true/false on success/error (i.e. chanmask having a different # of
  // channels from vb.flagCube()).
  static casacore::Bool applyChanMask(casacore::Cube<casacore::Bool>& chanmaskedflags,
                            const casacore::Vector<casacore::Bool> *chanmask, const VisBuffer& vb);

private:
  // Prohibit public copying and assignment.
  VisBuffGroup(const VisBuffGroup&);
  VisBuffGroup& operator=(const VisBuffGroup&);

  // // Numbers of data description ids and fields
  // casacore::uInt nDDID_p, nFld_p, nBuf_p;

  // Number of buffers.
  casacore::uInt nBuf_p;

  // The list of buffers.
  casacore::PtrBlock<VisBuffer*> VB_p;
  
  casacore::Vector<casacore::Bool> endChunk_p;

  // // casacore::Map spw,fld to the buffer id
  // casacore::Matrix<casacore::Int> spwfldids_p;
};


} //# NAMESPACE CASA - END

#endif


