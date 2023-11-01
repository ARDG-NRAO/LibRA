//# GroupWorker.cc: Partial implementation for base classes of objects that
//# process VisBuffGroups as fed to them by GroupProcessor.
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

#include <msvis/MSVis/GroupWorker.h>
#include <msvis/MSVis/VisBufferComponents.h>
#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/VBRemapper.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>
//#include <casa/Exceptions/Error.h>
//#include <casa/Logging/LogIO.h>

using namespace casacore;
namespace casa {

const asyncio::PrefetchColumns *GroupWorkerBase::prefetchColumns() const
{
  return &prefetchColumns_p;
}

GroupWorker::GroupWorker(const ROVisibilityIterator& invi) :
  invi_p(invi)
{
  // Necessary (or at least a good idea) before the invi calls while
  // initializing outvi_p.
  invi_p.originChunks();
  invi_p.origin();

  outvi_p = VisibilityIterator(const_cast<MeasurementSet&>(invi.ms()),
                               invi.getSortColumns(), false,
                               invi.getInterval());

  // Consistency.
  outvi_p.originChunks();
  outvi_p.origin();
}

// GroupWorker& GroupWorker::operator=(const GroupWorker &other)
// {
//   // trivial so far.
//   vi_p = other.vi_p;
//   return *this;
// }

GroupWriteToNewMS::GroupWriteToNewMS(MeasurementSet& outms, MSColumns *msc,
                                     const VBRemapper& remapper) :
  outms_p(outms),
  msc_p(msc),
  remapper_p(remapper),
  rowsdone_p(0)
{
}

uInt GroupWriteToNewMS::write(MeasurementSet& outms, MSColumns *msc, VisBuffer& vb,
                              uInt rowsdone, const VBRemapper& remapper, const Bool doFC,
                              const Bool doFloat, const Bool doSpWeight)
{
  uInt rowsnow = vb.nRow();
  RefRows rowstoadd(rowsdone, rowsdone + rowsnow - 1);

  outms.addRow(rowsnow);
  remapper.remap(vb, rowsdone == 0);

  msc->antenna1().putColumnCells(rowstoadd, vb.antenna1());
  msc->antenna2().putColumnCells(rowstoadd, vb.antenna2());
  Vector<Int> arrID(rowsnow);
  arrID.set(vb.arrayId());
  msc->arrayId().putColumnCells(rowstoadd, arrID);
  msc->data().putColumnCells(rowstoadd, vb.visCube());
  if(doFloat)
    msc->floatData().putColumnCells(rowstoadd, vb.floatDataCube());

  Vector<Int> ddID(rowsnow);
  ddID.set(vb.dataDescriptionId());
  msc->dataDescId().putColumnCells(rowstoadd, ddID);

  msc->exposure().putColumnCells(rowstoadd, vb.exposure());
  msc->feed1().putColumnCells(rowstoadd, vb.feed1());
  msc->feed2().putColumnCells(rowstoadd, vb.feed2());

  Vector<Int> fieldID(rowsnow);
  fieldID.set(vb.fieldId());
  msc->fieldId().putColumnCells(rowstoadd, fieldID);

  msc->flagRow().putColumnCells(rowstoadd, vb.flagRow()); 
  msc->flag().putColumnCells(rowstoadd, vb.flagCube());

  if(doFC)
    msc->flagCategory().putColumnCells(rowstoadd, vb.flagCategory());

  msc->interval().putColumnCells(rowstoadd, vb.timeInterval());
  msc->observationId().putColumnCells(rowstoadd, vb.observationId());
  msc->processorId().putColumnCells(rowstoadd, vb.processorId());
  msc->scanNumber().putColumnCells(rowstoadd, vb.scan());   // Don't remap!

  if(doSpWeight)
    msc->weightSpectrum().putColumnCells(rowstoadd, vb.weightSpectrum());
  msc->weight().putColumnCells(rowstoadd, vb.weightMat());
  msc->sigma().putColumnCells(rowstoadd, vb.sigmaMat());

  msc->stateId().putColumnCells(rowstoadd, vb.stateId());
  msc->time().putColumnCells(rowstoadd, vb.time());
  msc->timeCentroid().putColumnCells(rowstoadd, vb.timeCentroid());
  msc->uvw().putColumnCells(rowstoadd, vb.uvwMat());
      
  rowsdone += rowsnow;

  return rowsdone;
}

using namespace casacore;
} // end namespace casa
