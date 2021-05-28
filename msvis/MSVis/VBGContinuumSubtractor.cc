//# VBGContinuumSubtractor.cc: Subtract the continuum from VisBuffGroups and
//# write them to a different MS.
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

#include <msvis/MSVis/VBGContinuumSubtractor.h>
#include <msvis/MSVis/VBContinuumSubtractor.h>
#include <msvis/MSVis/VBRemapper.h>
#include <msvis/MSVis/SubMS.h>
#include <msvis/MSVis/VisBufferComponents.h>
#include <msvis/MSVis/VisBuffGroup.h>
#include <msvis/MSVis/VisBuffGroupAcc.h>
#include <casa/Exceptions/Error.h>
#include <casa/Logging/LogIO.h>
#include <ms/MSSel/MSSelection.h>

using namespace casacore;
namespace casa {

VBGContinuumSubtractor::VBGContinuumSubtractor(MeasurementSet& outms,
                                               MSColumns *msc,
                                               const VBRemapper& remapper,
                                               const ROVisibilityIterator& invi,
                                               const uInt fitorder,
                                               const MS::PredefinedColumns datacol,
                                               const String& fitspw,
                                               const String& outspw) :
  GroupWriteToNewMS(outms, msc, remapper),
  fitorder_p(fitorder),
  datacol_p(datacol),
  outspw_p(outspw),
  rowsdone_p(0),
  tvi_debug(False),
  want_cont_p(False)
{
  doWS_p = invi.existsWeightSpectrum();
  doFC_p = invi.existsFlagCategory();

  // Almost everything except the derived columns.
  prefetchColumns_p = asyncio::PrefetchColumns::prefetchColumns(
                                  VisBufferComponents::Ant1,
                                  VisBufferComponents::Ant2,
                                  VisBufferComponents::ArrayId,
                                  VisBufferComponents::DataDescriptionId,
                                  VisBufferComponents::Exposure,
                                  VisBufferComponents::Feed1,
                                  VisBufferComponents::Feed2,
                                  VisBufferComponents::FieldId,
                                  VisBufferComponents::FlagCube,
                                  VisBufferComponents::Flag,
                                  VisBufferComponents::FlagRow,
                                  VisBufferComponents::Freq,
                                  VisBufferComponents::ObservationId,

                                  // The cube always gets used, even if its
                                  // contents aren't.
                                  VisBufferComponents::ObservedCube,

                                  VisBufferComponents::NChannel,
                                  VisBufferComponents::NCorr,
                                  VisBufferComponents::NRow,
                                  VisBufferComponents::ProcessorId,
                                  VisBufferComponents::Scan,
                                  VisBufferComponents::SpW,
                                  VisBufferComponents::SigmaMat,
                                  VisBufferComponents::StateId,
                                  VisBufferComponents::Time,
                                  VisBufferComponents::TimeCentroid,
                                  VisBufferComponents::TimeInterval,
                                  VisBufferComponents::WeightMat,
                                  VisBufferComponents::UvwMat,
                                  -1);
  if(datacol == MS::MODEL_DATA)
    prefetchColumns_p.insert(VisBufferComponents::ModelCube);
  else if(datacol == MS::CORRECTED_DATA)
    prefetchColumns_p.insert(VisBufferComponents::CorrectedCube);
  
  if(doWS_p)
    prefetchColumns_p.insert(VisBufferComponents::WeightSpectrum);
  if(doFC_p)
    prefetchColumns_p.insert(VisBufferComponents::FlagCategory);

  VisBuffGroupAcc::fillChanMask(fitmask_p, fitspw, invi.ms());

  MSSelection mssel;
  mssel.setSpwExpr(outspw);
  Matrix<Int> chansel = mssel.getChanList(&invi.ms(), 1);
  Vector<Int> spws(chansel.column(0));
  uInt nselspws = spws.nelements();

  for(uInt i = 0; i < nselspws; ++i)
    outspws_p.insert(spws[i]);
}

VBGContinuumSubtractor::~VBGContinuumSubtractor()
{
  VisBuffGroupAcc::clearChanMask(fitmask_p);
}

// VBGContinuumSubtractor& VBGContinuumSubtractor::operator=(const VBGContinuumSubtractor &other)
// {
//   // trivial so far.
//   vi_p = other.vi_p;
//   return *this;
// }

Bool VBGContinuumSubtractor::process(VisBuffGroup& vbg)
{
  Bool worked = true;
  uInt nvbs = vbg.nBuf();
  Int maxAnt = 0;
  Int maxSpw = 0;   // VisBuffGroupAcc is 1 of those things that uses SpW when
                    // it should use DDID.
  Int maxFld = 0;

  // Dagnabbit, VisBuffGroupAcc accumulates DATA and MODEL_DATA (even if it
  // isn't there, apparently), but not CORRECTED_DATA or FLOAT_DATA.
  // Compensate by moving the wanted column into DATA if necessary, before
  // accumulating.
  Bool otherToData = datacol_p != MS::DATA;

  for(uInt bufnum = 0; bufnum < nvbs; ++bufnum){
    if(otherToData)
      vbg(bufnum).visCube() = vbg(bufnum).dataCube(datacol_p);
    
    if(vbg(bufnum).numberAnt() > maxAnt)        // Record maxAnt and maxFld
      maxAnt = vbg(bufnum).numberAnt();         // even for buffers that won't
    if(vbg(bufnum).fieldId() > maxFld)          // be used in the fit.
      maxFld = vbg(bufnum).fieldId();

    Int spw = vbg(bufnum).spectralWindow();
    if(fitmask_p.count(spw) > 0){               // This requires fitspw to
                                                // follow the '' = nothing,
                                                // '*' = everything convention.
      if(spw > maxSpw)
        maxSpw = vbg(bufnum).spectralWindow();
    }
  }

  // Find the continuum
  VisBuffGroupAcc vbga(maxAnt + 1,
                       maxSpw + 1,    // VBContinuumSubtractor doesn't care
                                      // whether all the vbs have distinct spws or not.
                       maxFld + 1,    // There should only be 1 selected
                       0.0,           // field, but its number is arbitrary.
                       false);        // VBGA is very Calibrater-centric when
                                      // it comes to MODEL_DATA.

  vbga.setTVIDebug(tvi_debug);

  for(uInt bufnum = 0; bufnum < nvbs; ++bufnum)
    if(fitmask_p.count(vbg(bufnum).spectralWindow()) > 0)
      vbga.accumulate(vbg(bufnum));

  // Select the fit channels after loading the VBs into vbga, so the VBs in vbg
  // are unaffected.
  vbga.applyChanMask(fitmask_p);

  vbga.finalizeAverage();               // Is this necessary when each vb is
                                        // being stored seperately?

  VBContinuumSubtractor vbcs;

  // It might be better later to cache the known lo and hi freqs, and use
  // vbcs.init().  See AMueller::selfSolveOne().
  vbcs.initFromVBGA(vbga);

  vbcs.setTVIDebug(tvi_debug);

  // datacol_p is in DATA now.
  vbcs.fit(vbga, fitorder_p, MS::DATA, coeffs_p, coeffsOK_p, false, true, false);

  //uInt oldrowsdone = rowsdone_p;
  for(uInt bufnum = 0; bufnum < nvbs; ++bufnum){
    uInt spw = vbg(bufnum).spectralWindow();

    if(outspws_p.find(spw) != outspws_p.end()){
      // datacol_p is in DATA now.  Is this repetitious?  Yes it is.
      if(!vbcs.apply(vbg(bufnum), MS::DATA, coeffs_p, coeffsOK_p, !want_cont_p,
                     appliedSpWs_p.count(spw) < 1)){
        worked = false;
        break;
      }
      appliedSpWs_p.insert(spw);

      // Use SIGMA like a storage place for corrected weights.
      if(otherToData){
        vbg(bufnum).sigmaMat() = vbg(bufnum).weightMat();
        arrayTransformInPlace(vbg(bufnum).sigmaMat(), subms::wtToSigma);
      }

      rowsdone_p = GroupWriteToNewMS::write(outms_p, msc_p, vbg(bufnum),
                                            rowsdone_p, remapper_p,
                                            doFC_p,
                                            false,      // for now
                                            doWS_p);
      //cerr << "Wrote out row IDs " << oldrowsdone << " - " << rowsdone_p - 1 << ",";
    }
    //else
    //  cerr << "No output for";
    //cerr << " spw " << spw << endl;
    //oldrowsdone = rowsdone_p;
  }
  
  return worked;
}

using namespace casacore;
} // end namespace casa
