//# CalVisBuffer.h: A VisBuffer with extra stuff for calibration
//# Copyright (C) 2008
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
//# $Id: VisBuffer.h,v 19.14 2006/02/28 04:48:58 mvoronko Exp $

#ifndef MSVIS_CALVISBUFFER_H
#define MSVIS_CALVISBUFFER_H

#include <casa/aips.h>
#include <msvis/MSVis/VisBuffer.h>

namespace casa { //# NAMESPACE CASA - BEGIN

//#forward

//<summary>CalVisBuffer extends VisBuffer to support storage and recall of associated residual and differentiated residual data. </summary>
//
// <use visibility=export>
//
// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="VisBuffer">VisBuffer</linkto>
// </prerequisite>
//
// <etymology>
// CalVisBuffer is a VisBuffer for calibration solving
// </etymology>
//
//<synopsis>
// This class extends <linkto class="VisBuffer">VisBuffer to support 
// storage and recall of calibration solving-related information
// such as residuals (difference of trial-corrupted model data and
// observed data) and corresponding differentiated residuals.
// 
// (say something about synchronization with VisIter, etc.)
// 
//</synopsis>

//<todo>
// <li> write todo list
//</todo>
class CalVisBuffer : public VisBuffer
{
public:
  // Create empty VisBuffer you can assign to or attach.
  CalVisBuffer();
  // Construct VisBuffer for a particular VisibilityIterator
  // The buffer will remain synchronized with the iterator.
  CalVisBuffer(ROVisibilityIterator & iter);

  // Copy construct, looses synchronization with iterator: only use buffer for
  // current iteration (or reattach).
  CalVisBuffer(const CalVisBuffer& cvb);

  // Destructor (detaches from VisIter)
  ~CalVisBuffer(); 

  // Assignment, looses synchronization with iterator: only use buffer for 
  // current iteration (or reattach)
  CalVisBuffer& operator=(const VisBuffer& cvb);

  // Assignment, optionally without copying the data across; with copy=true
  // this is identical to normal assignment operator
  CalVisBuffer& assign(const VisBuffer& vb, casacore::Bool copy=true);

  // Update (simple) coord info 
  // (this OVERRIDES VisBuffer::updateCoordInfo(), which does more)
  void updateCoordInfo(const VisBuffer * vb = NULL, const casacore::Bool dirDependent=true);

  // Apply amp-only or phase-only to data
  void enforceAPonData(const casacore::String& apmode);

  // Set the focus channel
  //  (forms references to focus-channel flag/data/model)
  void setFocusChan(const casacore::Int focusChan=-1);

  // Size/init/finalize the residuals workspaces
  void sizeResiduals(const casacore::Int& nPar,const casacore::Int& nDiff);
  void initResidWithModel();
  void finalizeResiduals();

  // Delete the workspaces
  void cleanUp();

  // <group>
  // Access functions
  //

  // Access to focus-channel slices of the flags, data, and model
  //  casacore::Cube<casacore::Bool>& infocusFlagCube() { return infocusFlagCube_p; }
  //  const casacore::Cube<casacore::Bool>& infocusFlagCube() const {return this->infocusFlagCube();}

  casacore::Matrix<casacore::Bool>& infocusFlag() { return infocusFlag_p; }
  const casacore::Matrix<casacore::Bool>& infocusFlag() const {return infocusFlag_p;}

  casacore::Cube<casacore::Complex>& infocusVisCube() { return infocusVisCube_p; }
  const casacore::Cube<casacore::Complex>& infocusVisCube() const {return infocusVisCube_p;}

  casacore::Cube<casacore::Complex>& infocusModelVisCube() { return infocusModelVisCube_p; }
  const casacore::Cube<casacore::Complex>& infocusModelVisCube() const {return infocusModelVisCube_p;}

  // Workspace for the residual visibilities
  casacore::Cube<casacore::Complex>& residuals() { return residuals_p; }
  const casacore::Cube<casacore::Complex>& residuals() const {return residuals_p;}

  // Workspace for flags of the residuals
  casacore::Matrix<casacore::Bool>& residFlag() { return residFlag_p; }
  const casacore::Matrix<casacore::Bool>& residFlag() const {return residFlag_p;}

  // Workspace for the differentiated residuals
  casacore::Array<casacore::Complex>& diffResiduals() { return diffResiduals_p; }
  const casacore::Array<casacore::Complex>& diffResiduals() const {return diffResiduals_p;}

  //</group>

private:

  // The current in-focus channel
  casacore::Int focusChan_p;

  // actual storage for the data
  //  casacore::Cube<casacore::Bool> infocusFlagCube_p;
  casacore::Matrix<casacore::Bool> infocusFlag_p;
  casacore::Cube<casacore::Complex> infocusVisCube_p;
  casacore::Cube<casacore::Complex> infocusModelVisCube_p;

  casacore::Cube<casacore::Complex> residuals_p;
  casacore::Matrix<casacore::Bool> residFlag_p;
  casacore::Array<casacore::Complex> diffResiduals_p;
};


} //# NAMESPACE CASA - END

#endif

