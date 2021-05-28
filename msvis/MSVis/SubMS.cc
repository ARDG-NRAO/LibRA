//# SubMS.cc 
//# Copyright (C) 1996-2007
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify
//# it under the terms of the GNU General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or
//# (at your option) any later version.
//#
//# This library is distributed in the hope that it will be useful,
//# but WITHOUT ANY WARRANTY; without even the implied warranty of
//# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//# GNU General Public License for more details.
//# 
//# You should have received a copy of the GNU General Public License
//# along with this library; if not, write to the Free Software
//# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id: $

// To make Timer reports in the inner loop of the simple copy,
// uncomment the following line:
//#define COPYTIMER

#include <msvis/MSVis/SubMS.h>
#include <asdmstman/AsdmStMan.h>
#include <ms/MSSel/MSSelection.h>
//#include <ms/MSSel/MSTimeGram.h>
//#include <tables/TaQL/ExprNode.h>
#include <tables/Tables/RefRows.h>
#include <ms/MeasurementSets/MSColumns.h>
#include <coordinates/Coordinates/CoordinateUtil.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Cube.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/ArrayOpsDiffShapes.h>
#include <casa/Arrays/ArrayLogical.h>
#include <casa/Arrays/ArrayUtil.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Slice.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Logging/LogIO.h>
#include <casa/OS/File.h>
#include <casa/OS/Directory.h>
#include <casa/OS/HostInfo.h>
#include <casa/OS/Memory.h>              // Can be commented out along with
//                                         // Memory:: calls.

//#ifdef COPYTIMER
#include <casa/OS/Timer.h>
//#endif

#include <casa/Containers/Record.h>
#include <casa/BasicSL/String.h>
#include <casa/Utilities/Assert.h>
#include <casa/Utilities/GenSort.h>
#include <casa/System/AppInfo.h>
#include <casa/System/ProgressMeter.h>
#include <casa/Quanta/QuantumHolder.h>
#include <msvis/MSVis/GroupProcessor.h>
//#include <msvis/MSVis/VisSet.h>
#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/VisBufferComponents.h>
#include <msvis/MSVis/VBGContinuumSubtractor.h>
#include <msvis/MSVis/VBRemapper.h>
#include <msvis/MSVis/VisChunkAverager.h>
#include <msvis/MSVis/VisIterator.h>
//#include <msvis/MSVis/VisibilityIterator.h>
#include <tables/DataMan/IncrementalStMan.h>
#include <tables/Tables/ScalarColumn.h>
#include <tables/Tables/ScaColDesc.h>
#include <tables/Tables/SetupNewTab.h>
#include <tables/DataMan/StandardStMan.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/PlainTable.h>
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/TableInfo.h>
#include <tables/Tables/TableLock.h>
#include <tables/Tables/TableRecord.h>
#include <tables/Tables/TableCopy.h>
#include <tables/Tables/TableRow.h>
#include <tables/DataMan/TiledColumnStMan.h>
#include <tables/DataMan/TiledShapeStMan.h>
#include <tables/DataMan/TiledDataStMan.h>
#include <tables/DataMan/TiledStManAccessor.h>
#include <ms/MeasurementSets/MSTileLayout.h>
#include <scimath/Mathematics/InterpolateArray1D.h>
#include <scimath/Mathematics/FFTServer.h>
#include <casa/sstream.h>
#include <casa/iomanip.h>
#include <functional>
#include <map>
#include <set>
#include <measures/Measures/MeasTable.h>
#include <scimath/Mathematics/Smooth.h>
#include <casa/Quanta/MVTime.h>


using namespace casacore;


namespace casa {

typedef ROVisibilityIterator ROVisIter;
typedef VisibilityIterator VisIter;

namespace subms {


// Weight <-> Sigma routines
//   A value in zero in either will be interpretted as 
//    practically zero weight (~inf noise), w/out generating
//    inf or NaN
Double wtToSigma(Double wt)
{
  // sig = 1/sqrt(wt)
  return wt > 0.0 ? 1.0 / sqrt(wt) : FLT_MAX;
}
Double sigToWeight(Double sig)
{
  // wt = 1/square(sig)
  return sig > 0.0 ? 1.0 / square(sig) : FLT_EPSILON;
}

} // end namespace subms

//#warning "Debugging kluge here"
//inline Complex operator*(const Complex& val, Double f) { return val*Float(f); }
//

  SubMS::SubMS(String& theMS, Table::TableOption option) :
    ms_p(MeasurementSet(theMS, option)),
    mssel_p(ms_p),
    msc_p(NULL),
    mscIn_p(NULL),
    keepShape_p(true),
    //    sameShape_p(true),
    antennaSel_p(false),
    timeBin_p(-1.0),
    scanString_p(""),
    intentString_p(""),
    obsString_p(""),
    uvrangeString_p(""),
    taqlString_p(""),
    timeRange_p(""),
    arrayExpr_p(""),
    combine_p(""),
    fitorder_p(-1),
    fitspw_p("*"),
    fitoutspw_p("*"),
    fillMainTable_p(True),
    tvi_debug(False),
    want_cont_p(False)
  {
  }
  
  SubMS::SubMS(MeasurementSet& ms) :
    ms_p(ms),
    mssel_p(ms_p),
    msc_p(NULL),
    mscIn_p(NULL),
    keepShape_p(true),
    //sameShape_p(true),
    antennaSel_p(false),
    timeBin_p(-1.0),
    scanString_p(""),
    intentString_p(""),
    obsString_p(""),
    uvrangeString_p(""),
    taqlString_p(""),
    timeRange_p(""),
    arrayExpr_p(""),
    combine_p(""),
    fitorder_p(-1),
    fitspw_p("*"),
    fitoutspw_p("*"),
    fillMainTable_p(true),
    tvi_debug(False),
    want_cont_p(False)
  {
  }
  
  SubMS::~SubMS()
  {
    delete msc_p;
    msc_p = nullptr;
    
    delete mscIn_p;
    mscIn_p = nullptr;

    msOut_p=MeasurementSet();

    // parseColumnNames unavoidably has a static String and Vector<MS::PredefinedColumns>.
    // Collapse them down to free most of that memory.
    parseColumnNames("None");

  }

std::set<Int> SubMS::findBadSpws(MeasurementSet& ms, Vector<Int> spwv)
{
  ScalarColumn<Int> spws_in_dd(ms.dataDescription(), 
				 MSDataDescription::columnName(MSDataDescription::SPECTRAL_WINDOW_ID));
  std::set<Int> uniqSpwsInDD;
  uInt nspwsInDD = spws_in_dd.nrow();
  for(uInt ddrow = 0; ddrow < nspwsInDD; ++ddrow)
    uniqSpwsInDD.insert(spws_in_dd(ddrow));
  std::set<Int>::iterator ddend = uniqSpwsInDD.end();
  std::set<Int> badSelSpwSlots;
  for(uInt k = 0; k < spwv.nelements(); ++k){
    if(uniqSpwsInDD.find(spwv[k]) == ddend){
      badSelSpwSlots.insert(k);
    }
  }
  return badSelSpwSlots;
}
  
  // This is the version used by split.
  Bool SubMS::selectSpw(const String& spwstr, const Vector<Int>& steps)
  {
    LogIO os(LogOrigin("SubMS", "selectSpw()"));

    MSSelection mssel;
    String myspwstr(spwstr == "" ? "*" : spwstr);

    mssel.setSpwExpr(myspwstr);

    widths_p = steps.copy();
    if(widths_p.nelements() < 1){
      widths_p.resize(1);
      widths_p[0] = 1;
    }
    else{
      for(uInt k = 0; k < widths_p.nelements(); ++k){
        if(widths_p[k] == 0){
          os << LogIO::WARN
             << "0 cannot be used for channel width...using 1 instead."
             << LogIO::POST;
          widths_p[k] = 1;
        }
      }
    }

    // Each row should have spw, start, stop, step
    // A single width is a default, but multiple widths should be used
    // literally.
    Matrix<Int> chansel = mssel.getChanList(&ms_p, 1);

    if(chansel.nrow() > 0) {         // Use myspwstr if it selected anything...
      spw_p       = chansel.column(0);
      chanStart_p = chansel.column(1);
      chanEnd_p   = chansel.column(2);
      chanStep_p  = chansel.column(3);

      uInt nspw = chanEnd_p.nelements();
      nchan_p.resize(nspw);

      // A single width is a default, but multiple widths should be used
      // literally.
      if(widths_p.nelements() > 1 && widths_p.nelements() != spw_p.nelements()){
        os << LogIO::SEVERE
           << "Mismatch between the # of widths specified by width and the # of spws."
           << LogIO::POST;
        return false;
      }

      // Copy the default width to all spws.
      if(widths_p.nelements() < nspw){
        widths_p.resize(nspw, true);
        for(uInt k = 1; k < nspw; ++k)
          widths_p[k] = widths_p[0];
      }

      for(uInt k = 0; k < nspw; ++k){
	if(chanStep_p[k] == 0)	// CAS-2224, triggered by spw='0:2'
	  chanStep_p[k] = 1;	// (as opposed to '0:2~2').
	
        // if((nchan_p[k] - chanStart_p[k] + 1) % (chanStep_p[k] * widths_p[k]) != 0)
        //   os << LogIO::WARN
        //      << "The number of selected channels, " << nchan_p[k]
        //      << ", for spw " << spw_p[k] << " is not a multiple of the increment, "
        //      << chanStep_p[k] * widths_p[k] << ".\n"
        //      << "The reported width and frequency of the final channel may be"
        //      << "\noff by a fraction of a channel width.\n"
        //      << "(This is being worked on.)"
        //      << LogIO::POST;

        nchan_p[k] = 1 + (chanEnd_p[k] -
                          chanStart_p[k]) / (chanStep_p[k] * widths_p[k]);
        if(nchan_p[k] < 1)
          nchan_p[k] = 1;
      }
    }
    else{                            // select everything and rely on widths.
      MSSpWindowColumns mySpwTab(ms_p.spectralWindow());
      uInt nspw = mySpwTab.nrow();

      nchan_p = mySpwTab.numChan().getColumn();
      
      spw_p.resize(nspw);
      indgen(spw_p);
      
      chanStart_p.resize(nspw);
      chanStep_p.resize(nspw);
      for(uInt k = 0; k < nspw; ++k){
        chanStart_p[k] = 0;
        chanEnd_p[k]   = nchan_p[k] - 1;
        chanStep_p[k]  = 1;
      }

      if(widths_p.nelements() != spw_p.nelements()){
        if(widths_p.nelements() == 1){
          widths_p.resize(spw_p.nelements(), true);
          for(uInt k = 1; k < spw_p.nelements(); ++k)
            widths_p[k] = widths_p[0];
	}
        else{
          os << LogIO::SEVERE
             << "Mismatch between the # of widths specified by width and the # of spws."
             << LogIO::POST;
          return false;
        }
      }

      for(uInt k = 0; k < nspw; ++k){
        // if((nchan_p[k] + 1) % (chanStep_p[k] * widths_p[k]) != 0)
        //   os << LogIO::WARN
        //      << "The number of selected channels, " << nchan_p[k]
        //      << ", for spw " << spw_p[k] << " is not a multiple of the increment, "
        //      << chanStep_p[k] * widths_p[k] << ".\n"
        //      << "The reported width and frequency of the final channel may be"
        //      << "\noff by a fraction of a channel width.\n"
        //      << "(This is being worked on.)"
        //      << LogIO::POST;

        nchan_p[k] = 1 + (nchan_p[k] - 1) / widths_p[k];
      }
    }
    
    // Check for and filter out selected spws that aren't included in
    // DATA_DESCRIPTION.  (See CAS-1673 for an example.)
    std::set<Int> badSelSpwSlots(SubMS::findBadSpws(ms_p, spw_p));
    uInt nbadSelSpwSlots = badSelSpwSlots.size();
    if(nbadSelSpwSlots > 0){
      os << LogIO::WARN << "Selected input spw(s)\n";
      for(std::set<Int>::iterator bbit = badSelSpwSlots.begin();
          bbit != badSelSpwSlots.end(); ++bbit)
        os << spw_p[*bbit] << " ";
      os << "\nwere not found in DATA_DESCRIPTION and are being excluded."
         << LogIO::POST;

      uInt nSelSpw = spw_p.nelements();
      uInt ngoodSelSpwSlots = nSelSpw - nbadSelSpwSlots;
      Vector<Int> spwc(ngoodSelSpwSlots);
      Vector<Int> chanStartc(ngoodSelSpwSlots);
      Vector<Int> chanEndc(ngoodSelSpwSlots);
      Vector<Int> nchanc(ngoodSelSpwSlots);
      Vector<Int> chanStepc(ngoodSelSpwSlots);
      std::set<Int>::iterator bsend = badSelSpwSlots.end();
      
      uInt j = 0;
      for(uInt k = 0; k < nSelSpw; ++k){
        if(badSelSpwSlots.find(k) == bsend){
          spwc[j]       = spw_p[k];
          chanStartc[j] = chanStart_p[k];
          chanEndc[j]   = chanEnd_p[k];
          nchanc[j]     = nchan_p[k];
          chanStepc[j]  = chanStep_p[k];
          ++j;
        }
      }
      spw_p.resize(ngoodSelSpwSlots);
      spw_p = spwc;
      chanStart_p.resize(ngoodSelSpwSlots);
      chanStart_p = chanStartc;
      chanEnd_p.resize(ngoodSelSpwSlots);
      chanEnd_p = chanEndc;
      nchan_p.resize(ngoodSelSpwSlots);
      nchan_p = nchanc;
      chanStep_p.resize(ngoodSelSpwSlots);
      chanStep_p = chanStepc;
    }
    
    mssel.getChanSlices(chanSlices_p, &ms_p, 1);
    return true;
  }
  
  // This older version is used elsewhere.
  void SubMS::selectSpw(Vector<Int> spw, Vector<Int> nchan, Vector<Int> start, 
			Vector<Int> step)
  {
    spw_p.resize();
    spw_p = spw;
    
    //check for default
    if(spw_p.nelements() == 1 && spw_p[0] < 0){
      spw_p.resize(ms_p.spectralWindow().nrow());
      indgen(spw_p);

      //no may be we have to redo the chan selection
      
      if (nchan.nelements() != spw_p.nelements()){
	nchan.resize(spw_p.nelements(), true);
	for(uInt k = 1; k < spw_p.nelements(); ++k){
	  nchan[k] = nchan[0];
	}
      }
      if (start.nelements() != spw_p.nelements()){
	start.resize(spw_p.nelements(), true);
	for(uInt k = 1; k < spw_p.nelements(); ++k){
	  start[k] = start[0];
	}
      }
      if (step.nelements() != spw_p.nelements()){
	step.resize(spw_p.nelements(), true);
	for(uInt k = 1; k < spw_p.nelements(); ++k){
	  step[k] = step[0];
	}
      }
    }
        
    nchan_p.resize();
    nchan_p = nchan;
    chanStart_p.resize();
    chanStart_p = start;
    chanEnd_p.resize(spw_p.nelements());
    for(uInt k =0; k < spw_p.nelements(); ++k)
      chanEnd_p[k] = nchan[k] - 1;
    chanStep_p.resize();
    chanStep_p = step;
    // check for defaults
    if(nchan_p[0]<=0 || (nchan_p.nelements() != spw_p.nelements())){
      nchan_p.resize(spw_p.nelements());
      MSSpWindowColumns mySpwTab(ms_p.spectralWindow());
      for (uInt k =0; k < spw_p.nelements(); ++k){
	if(nchan[0]<=0)
	  nchan_p[k]=mySpwTab.numChan()(spw_p[k]);
	else
	  nchan_p[k]=nchan[0];
        chanEnd_p[k] = nchan_p[k] - 1;
      }
      chanStart_p.resize(spw_p.nelements());
      chanStep_p.resize(spw_p.nelements());
      if(chanStart_p.nelements() == start.nelements()){
	chanStart_p=start;
      }
      else{
	chanStart_p.set(start[0]);
      }
      if(chanStep_p.nelements() == step.nelements()){
	chanStep_p=step;
      }
      else{
	chanStep_p.set(step[0]);
      } 
    }    
  }

  // selectSpw must be called first because this uses spwRelabel_p!
  Bool SubMS::selectCorrelations(const String& corrstr)
  {
    LogIO os(LogOrigin("SubMS", "selectCorrelations()"));
    MSSelection mssel;
    const Bool areSelecting = corrstr != "" && corrstr != "*";

    if(areSelecting)
      mssel.setPolnExpr(corrstr);
    corrString_p = corrstr;
    mssel.getCorrSlices(corrSlices_p, &ms_p);
    return getCorrMaps(mssel, ms_p, inPolOutCorrToInCorrMap_p, areSelecting);
  }

Bool SubMS::getCorrMaps(MSSelection& mssel, const MeasurementSet& ms,
			Vector<Vector<Int> >& outToIn, const Bool areSelecting)
{
  Bool cando = true;

  uInt npol = ms.polarization().nrow();  // The total number of polids
    
  // Nominally empty selection for all polids
  outToIn.resize(npol);
  outToIn.set(Vector<Int>());
    
  if(areSelecting){
    // Get the corr indices as an ordered map
    std::map<Int, Vector<Vector<Int> > > corrmap(mssel.getCorrMap(&ms));

    // Iterate over the ordered map to fill the vector maps
    for( auto mi = corrmap.begin( ); mi != corrmap.end( ); ++mi ) {
      Int pol = mi->first;
      outToIn[pol] = mi->second[0];
    }
  }
  else{	// Make outToIn an identity map.
    ScalarColumn<Int> numCorr(ms.polarization(), 
				MSPolarization::columnName(MSPolarization::NUM_CORR));
    
    for(uInt polid = 0; polid < npol; ++polid){
      uInt ncorr = numCorr(polid);
      
      outToIn[polid].resize(ncorr);
      for(uInt cid = 0; cid < ncorr; ++cid)
	outToIn[polid][cid] = cid;
    }
  }
  return cando;
}

  // This is the one used by split.
  Bool SubMS::setmsselect(const String& spw, const String& field,
                          const String& baseline, const String& scan,
                          const String& uvrange, const String& taql,
                          const Vector<Int>& step, const String& subarray,
                          const String& correlation, const String& intent,
                          const String& obs)
  {
    LogIO os(LogOrigin("SubMS", "setmsselect()"));
    Bool  ok;
    
    String myspwstr(spw == "" ? "*" : spw);
    Record selrec = ms_p.msseltoindex(myspwstr, field);

    ok = selectSource(selrec.asArrayInt("field"));

    // All of the requested selection functions will be tried, even if an
    // earlier one has indicated its failure.  This allows all of the selection
    // strings to be tested, yielding more complete feedback for the user
    // (fewer retries).  This is a matter of taste, though.  If the selections
    // turn out to be slow, this function should return on the first false.

    if(!selectSpw(myspwstr, step)){
      os << LogIO::SEVERE << "No channels selected." << LogIO::POST;
      ok = false;
    }
    
    if(baseline != ""){
      Vector<Int> antid(0);
      Vector<String> antstr(1,baseline);
      selectAntenna(antid, antstr);
    }
    scanString_p    = scan;
    intentString_p  = intent;
    obsString_p     = obs;
    uvrangeString_p = uvrange;
    taqlString_p    = taql;

    if(subarray != "")
      selectArray(subarray);

    if(!selectCorrelations(correlation)){
      os << LogIO::SEVERE << "No correlations selected." << LogIO::POST;
      ok = false;
    }

    return ok;
  }

  // This is the older version, used elsewhere.
  void SubMS::setmsselect(const String& spw, const String& field, 
                          const String& baseline, const String& scan,
                          const String& obs, const String& uvrange, 
                          const String& taql, const Vector<Int>& nchan, 
                          const Vector<Int>& start, const Vector<Int>& step,
                          const String& subarray, const String& intent)
  {
    Vector<Int> inchan(1, -1);
    Vector<Int> istart(1, 0);
    Vector<Int> istep(1, 1);
    Record      selrec = ms_p.msseltoindex(spw, field);
    Vector<Int> spwids = selrec.asArrayInt("spw");

    selectSource(selrec.asArrayInt("field"));
    if(spwids.nelements() < 1)
      spwids=Vector<Int>(1, -1);

    //use nchan if defined else use caret-column syntax of  msselection 
    if((nchan.nelements()>0) && nchan[0] > 0){
      inchan.resize(); inchan=nchan;
      if((step.nelements() >0 ) && (step.nelements() != nchan.nelements()) && (nchan.nelements() >1)){
	istep[0]=step[0];
      }
      if(step.nelements() != nchan.nelements()){
	istep.resize(nchan.nelements(), true);
	istep.set(istep[0]);
      }
      else{
	istep.resize(); istep=step;
      }
      if((start.nelements() >0 ) && (start.nelements() != nchan.nelements()) && (nchan.nelements() >1)){
	istart[0]=start[0];
      }
      if(start.nelements() != nchan.nelements()){
	istart.resize(nchan.nelements(), true);
	istart.set(istart[0]);
      }
      else{
	istart.resize(); istart=start;
      }
    }
    else{
      Matrix<Int> chansel=selrec.asArrayInt("channel");
      if(chansel.nelements() != 0){
        inchan.resize(chansel.nrow());
        istep.resize(chansel.nrow());
        istart.resize(chansel.nrow());
        // if the vector step is used ..for averaging ..let's use it
        Bool stepused=false;
        if( (step.nelements() >= 1) && (max(step) > 1))
          stepused=true;
        for (uInt k =0 ; k < chansel.nrow(); ++k){
          if(stepused){
            if(step.nelements() == 1)
              istep[k] = step[0];
            else if(step.nelements() == istep.nelements())
              istep[k] = step[k];
            else //confused at this stage
              istep[k] = 1;
          }
          else{
            istep[k] = chansel.row(k)(3);
            if(istep[k] < 1)
              istep[k] = 1;
          }
          istart[k] = chansel.row(k)(1);
          inchan[k] = (chansel.row(k)(2) - istart[k] + 1) / istep[k];
          if(inchan[k] < 1)
            inchan[k] = 1;
        }
      } 
    }
    selectSpw(spwids, inchan, istart, istep);
    
    if(baseline != ""){
      Vector<Int> antid(0);
      Vector<String> antstr(1,baseline);
      selectAntenna(antid, antstr);
    }
    scanString_p    = scan;
    obsString_p    = obs;
    uvrangeString_p = uvrange;
    taqlString_p    = taql;
    intentString_p  = intent;

    if(subarray != "")
      selectArray(subarray);
  }  

  
  Bool SubMS::selectSource(const Vector<Int>& fieldid)
  {
    LogIO os(LogOrigin("SubMS", "selectSource()"));
    Bool cando = true;

    if(fieldid.nelements() < 1){
      fieldid_p = Vector<Int>(1, -1);
    }
    else if(fieldid.nelements() > ms_p.field().nrow()){
      os << LogIO::SEVERE
         << "More fields were requested than are in the input MS.\n"
         << LogIO::POST;
      cando = false;
    }
    else if(max(fieldid) >= static_cast<Int>(ms_p.field().nrow())){
      // Arriving here is very unlikely since if fieldid came from MSSelection
      // bad fields were presumably already quietly dropped.
      os << LogIO::SEVERE
         << "At least 1 field was requested that is not in the input MS.\n"
         << LogIO::POST;      
      cando = false;
    }
    else{
      fieldid_p = fieldid;
    }

    if(fieldid_p.nelements() == 1 && fieldid_p[0] < 0){
      fieldid_p.resize(ms_p.field().nrow());
      indgen(fieldid_p);
    }
    return cando;
  }
  
Bool SubMS::pickAntennas(Vector<Int>& selected_antennaids,
			 Vector<String>& selected_antenna_strs,
			 const Vector<Int>& antennaids,
			 const Vector<String>& antennaSel)
{
  Bool didSelect = true;
  if((antennaids.nelements() == 1) && (antennaids[0] == -1)){
    if(antennaSel[0] == "")
      didSelect = false;
    else
      selected_antennaids.resize();
  }
  else
    selected_antennaids = antennaids;
  selected_antenna_strs = antennaSel;
  return didSelect;
}
  
  void SubMS::selectTime(Double timeBin, String timerng)
  {  
    timeBin_p   = timeBin;
    timeRange_p = timerng;
  }  
  
  Bool SubMS::makeSubMS(String& msname, String& colname,
                        const Vector<Int>& tileShape, const String& combine)
  {
    LogIO os(LogOrigin("SubMS", "makeSubMS()"));

    /*
    try{
    */

      if((spw_p.nelements()>0) && (max(spw_p) >= Int(ms_p.spectralWindow().nrow()))){
        os << LogIO::SEVERE 
           << "SpectralWindow selection contains elements that do not exist in "
           << "this MS"
           << LogIO::POST;
        ms_p=MeasurementSet();
        return false;   
      }
      
      // Watch out!  This throws an AipsError if ms_p doesn't have the
      // requested columns.
      const Vector<MS::PredefinedColumns> colNamesTok = parseColumnNames(colname, ms_p);

      if(!makeSelection()){
        //os << LogIO::WARN 
        //   << "Failed on selection: the combination of spw, field, antenna, correlation, "
        //   << "and timerange may be invalid." 
        //   << LogIO::POST;
        ms_p=MeasurementSet();
        return false;
      }
      mscIn_p=new MSColumns(mssel_p);
      // Note again the parseColumnNames() a few lines back that stops setupMS()
      // from being called if the MS doesn't have the requested columns.
      MeasurementSet* outpointer=0;

      if(tileShape.nelements() == 3){
        outpointer = setupMS(msname, nchan_p[0], ncorr_p[0],  
                             colNamesTok, tileShape);
      }

      // the following calls MSTileLayout...  disabled for now because it
      // forces tiles to be the full spw bandwidth in width (gmoellen, 2010/11/07)
  
      else if((tileShape.nelements()==1) && (tileShape[0]==0 || tileShape[0]==1)){
        outpointer = setupMS(msname, nchan_p[0], ncorr_p[0],
                             mscIn_p->observation().telescopeName()(0),
                             colNamesTok, tileShape[0]);
      }
      /* 
      else{
	// Derive tile shape based on input dataset's tiles, borrowed
	//  from VisSet's scr col tile shape derivation
	//  (this may need some tweaking for averaging cases)
        TableDesc td = mssel_p.actualTableDesc();

        // If a non-DATA column, i.e. CORRECTED_DATA, is being written to DATA,
        // datacolname must be set to DATA because the tile management in
        // setupMS() will look for "TiledDATA", not "TiledCorrectedData".
        String datacolname = MS::columnName(MS::DATA);
        // But if DATA is not present in the input MS, using it would cause a
        // segfault.
        if(!td.isColumn(datacolname))
          // This is could be any other kind of *DATA column, including
          // FLOAT_DATA or LAG_DATA, but it is guaranteed to be something.
          datacolname = MS::columnName(colNamesTok[0]);

        const ColumnDesc& cdesc = td[datacolname];

        String dataManType = cdesc.dataManagerType();
        String dataManGroup = cdesc.dataManagerGroup();

        Bool tiled = (dataManType.contains("Tiled"));

        if (tiled) {
            ROTiledStManAccessor tsm(mssel_p, dataManGroup);
            uInt nHyper = tsm.nhypercubes();

            // Test clause
            if(1){
              os << LogIO::DEBUG1
                 << datacolname << "'s max cache size: "
                 << tsm.maximumCacheSize() << " bytes.\n"
                 << "\tnhypercubes: " << nHyper << ".\n"
                 << "\ttshp of row 0: " << tsm.tileShape(0)
                 << "\n\thypercube shape of row 0: " << tsm.hypercubeShape(0)
                 << LogIO::POST;
            }
    

            // Find smallest tile shape
            Int highestProduct=-INT_MAX;
            Int highestId=0;
            for (uInt id=0; id < nHyper; id++) {
              IPosition tshp(tsm.getTileShape(id));
              Int product = tshp.product();

              os << LogIO::DEBUG2
                 << "\thypercube " << id << ":\n"
		 << "\t\ttshp: " << tshp << "\n"
		 << "\t\thypercube shape: " << tsm.getHypercubeShape(id)
		 << ".\n\t\tcache size: " << tsm.getCacheSize(id)
		 << " buckets.\n\t\tBucket size: " << tsm.getBucketSize(id)
		 << " bytes."
		 << LogIO::POST;

              if (product > 0 && (product > highestProduct)) {
                highestProduct = product;
                highestId = id;
              }
            }
	    Vector<Int> dataTileShape = tsm.getTileShape(highestId).asVector();

	    outpointer = setupMS(msname, nchan_p[0], ncorr_p[0],  
				 colNamesTok, dataTileShape);

        }
      */
      else{
	  //Sweep all other cases of bad tileshape to a default one.
	  //  (this probably never happens)
	  outpointer = setupMS(msname, nchan_p[0], ncorr_p[0],
			       mscIn_p->observation().telescopeName()(0),  
			       colNamesTok, 0);
	
      }
      
      combine_p = combine;

      msOut_p= *outpointer;
      
      if(!fillAllTables(colNamesTok)){
        delete outpointer;
        os << LogIO::WARN << msname << " left unfinished." << LogIO::POST;
        ms_p=MeasurementSet();
        return false;
      }
      
      //  msOut_p.relinquishAutoLocks (true);
      //  msOut_p.unlock();
      //Detaching the selected part
      ms_p=MeasurementSet();
      
      delete outpointer;
      return true;

    /*
    }
    catch(AipsError x){
      ms_p=MeasurementSet();
      throw(x);
    }
    catch(...){
      ms_p=MeasurementSet();
      throw(AipsError("Unknown exception caught"));
    }
    */

  }
  
  MeasurementSet* SubMS::makeScratchSubMS(const String& colname,
                                          const Bool forceInMemory)
  {
    return makeScratchSubMS(parseColumnNames(colname, ms_p), forceInMemory);
  }
  
  MeasurementSet* SubMS::makeScratchSubMS(const Vector<MS::PredefinedColumns>& whichDataCols,
                                          const Bool forceInMemory)
  {
    LogIO os(LogOrigin("SubMS", "makeScratchSubMS()"));
    
    if(max(fieldid_p) >= Int(ms_p.field().nrow())){
      os << LogIO::SEVERE 
         << "Field selection contains elements that do not exist in "
         << "this MS"
         << LogIO::POST;
      ms_p=MeasurementSet();
      return 0;  
    }
    if(max(spw_p) >= Int(ms_p.spectralWindow().nrow())){
      os << LogIO::SEVERE 
         << "SpectralWindow selection contains elements that do not exist in "
         << "this MS"
         << LogIO::POST;
      ms_p=MeasurementSet();
      return 0;
    }
    
    if(!makeSelection()){
      os << LogIO::SEVERE 
         << "Failed on selection: combination of spw and/or field and/or time "
         << "chosen may be invalid."
         << LogIO::POST;
      ms_p=MeasurementSet();
      return 0;
    }
    mscIn_p=new MSColumns(mssel_p);
    Double sizeInMB= 1.5 * n_bytes() / (1024.0 * 1024.0);
    String msname=AppInfo::workFileName(uInt(sizeInMB), "TempSubMS");
    
    MeasurementSet* outpointer=setupMS(msname, nchan_p[0], ncorr_p[0],  
                                       mscIn_p->observation().telescopeName()(0),
                                       whichDataCols);
    
    outpointer->markForDelete();
    //Hmmmmmm....memory...... 
    if(sizeInMB <  (Double)(HostInfo::memoryTotal(true))/(2048.0) 
       || forceInMemory){
      MeasurementSet* a = outpointer;
      outpointer= new MeasurementSet(a->copyToMemoryTable("TmpMemoryMS"));
      outpointer->initRefs();
      delete a;
    }
    
    msOut_p = *outpointer;
    
    if(!fillAllTables(whichDataCols)){
      delete outpointer;
      outpointer = 0;
      ms_p = MeasurementSet();
      return 0;
    }
    //Detaching the selected part
    ms_p=MeasurementSet();
    return outpointer;
  }

  
Bool SubMS::fillAllTables(const Vector<MS::PredefinedColumns>& datacols)
{

  LogIO os(LogOrigin("SubMS", "fillAllTables()"));
  Bool success = true;

  // Copy the subtables before doing anything with the main table.  Otherwise
  // MSColumns won't work.

  // fill or update
  Timer timer;

  timer.mark();
  success &= copyPointing();
  os << LogIO::DEBUG1
     << "copyPointing took " << timer.real() << "s."
     << LogIO::POST;

  // Optional columns should be set up before msc_p.
  addOptionalColumns(mssel_p.spectralWindow(), msOut_p.spectralWindow(), true);

  // Force the Measures frames for all the time type columns to have the same
  // reference as the TIME column of the main table.
  // Disable the empty table check (with false) because some of the subtables
  // (like POINTING) might already have been written.
  // However, empty tables are still empty after setting up the reference codes
  // here.
  msc_p = new MSColumns(msOut_p);
  msc_p->setEpochRef(MEpoch::castType(mscIn_p->timeMeas().getMeasRef().getType()),
                     false);

  // UVW is the only other Measures column in the main table.
  msc_p->uvwMeas().setDescRefCode(Muvw::castType(mscIn_p->uvwMeas().getMeasRef().getType()));

  if(!mscIn_p->flagCategory().isNull() && mscIn_p->flagCategory().isDefined(0))
    msc_p->setFlagCategories(mscIn_p->flagCategories());

  timer.mark();
  if(!fillDDTables())
    return false;
  os << LogIO::DEBUG1
     << "fillDDTables took " << timer.real() << "s."
     << LogIO::POST;

  // SourceIDs need to be remapped around here.  It could not be done in
  // selectSource() because mssel_p was not setup yet.
  timer.mark();
  relabelSources();
  os << LogIO::DEBUG1
     << "relabelSources took " << timer.real() << "s."
     << LogIO::POST;

  success &= fillFieldTable();
  success &= copySource();

  success &= copyAntenna();
  if(!copyFeed())         // Feed table writing has to be after antenna 
    return false;

  success &= copyFlag_Cmd();
  success &= copyHistory();
  success &= copyObservation();
  success &= copyProcessor();
  success &= copyState();

  timer.mark();
  success &= copySyscal();
  os << LogIO::DEBUG1
     << "copySyscal took " << timer.real() << "s."
     << LogIO::POST;

  timer.mark();
  success &= copyWeather();
  os << LogIO::DEBUG1
     << "copyWeather took " << timer.real() << "s."
     << LogIO::POST;
  
  timer.mark();
  success &= filterOptSubtable("CALDEVICE");
  os << LogIO::DEBUG1
     << "CALDEVICE took " << timer.real() << "s."
     << LogIO::POST;

  timer.mark();
  success &= filterOptSubtable("SYSPOWER");
  os << LogIO::DEBUG1
     << "SYSPOWER took " << timer.real() << "s."
     << LogIO::POST;

  // Run this after running the other copy*()s.  Maybe there should be an
  // option to *not* run it.
  success &= copyGenericSubtables();

  //sameShape_p = areDataShapesConstant();

  // jagonzal: Allow main table to be left empty, so that it can be filled by another layer.
  if (fillMainTable_p)
  {
	  if(fitorder_p < 0 && timeBin_p <= 0.0)
	    success &= writeAllMainRows(datacols);
	  else
	    success &= writeSomeMainRows(datacols);
	  return success;
  }
  else
  {
	  return success;
  }
}
  
  
  Bool SubMS::makeSelection(){
    
    LogIO os(LogOrigin("SubMS", "makeSelection()"));
    
    //VisSet/MSIter will check if the SORTED exists
    //and resort if necessary
    {
      // Matrix<Int> noselection;
      // VisSet vs(ms_p, noselection);
      Block<Int> sort;
      ROVisibilityIterator(ms_p, sort);
    }
   
    const MeasurementSet *elms;
    elms=&ms_p;
    MeasurementSet sorted;
    if (ms_p.keywordSet().isDefined("SORTED_TABLE")) {
      sorted=ms_p.keywordSet().asTable("SORTED_TABLE");
      //If ms is not writable and sort is a subselection...use original ms
      if( ms_p.nrow() == sorted.nrow())
        elms=&sorted;
    }
 
    MSSelection thisSelection;
    if(fieldid_p.nelements() > 0)
      thisSelection.setFieldExpr(MSSelection::indexExprStr(fieldid_p));
    if(spw_p.nelements() > 0)
      thisSelection.setSpwExpr(MSSelection::indexExprStr(spw_p));
    if(antennaSel_p){
      if(antennaId_p.nelements() > 0){
        thisSelection.setAntennaExpr(MSSelection::indexExprStr( antennaId_p ));
      }
      if(antennaSelStr_p[0] != "")
        thisSelection.setAntennaExpr(MSSelection::nameExprStr( antennaSelStr_p));
    }
    if(timeRange_p != "")
      thisSelection.setTimeExpr(timeRange_p);
    
    thisSelection.setUvDistExpr(uvrangeString_p);
    thisSelection.setScanExpr(scanString_p);
    thisSelection.setStateExpr(intentString_p);
    thisSelection.setObservationExpr(obsString_p);
    if(arrayExpr_p != "")
      thisSelection.setArrayExpr(arrayExpr_p);
    if(corrString_p != "")
      thisSelection.setPolnExpr(corrString_p);
    thisSelection.setTaQLExpr(taqlString_p);
    
    TableExprNode exprNode=thisSelection.toTableExprNode(elms);
    selTimeRanges_p = thisSelection.getTimeList();
    selObsId_p = thisSelection.getObservationList();

    {      
      const MSDataDescription ddtable = elms->dataDescription();
      ScalarColumn<Int> polId(ddtable, 
                                MSDataDescription::columnName(MSDataDescription::POLARIZATION_ID));
      const MSPolarization poltable = elms->polarization();
      ArrayColumn<Int> pols(poltable, 
                              MSPolarization::columnName(MSPolarization::CORR_TYPE));
      
      ScalarColumn<Int> spwId(ddtable, 
                                MSDataDescription::columnName(MSDataDescription::SPECTRAL_WINDOW_ID));

      uInt nddids = polId.nrow();
      uInt nSpws = spw_p.nelements();

      // # of distinct channel ranges or pol setups (!#pols) per spw.
      Vector<uInt> nuses_per_spw;

      Int highestSpw = max(spw_p);
      if(highestSpw < 0)
        highestSpw = 0;
      spw2ddid_p.resize(highestSpw + 1);
      nuses_per_spw.resize(highestSpw + 1);
      spw2ddid_p.set(0);                 // This is a row #, so must be >= 0.
      nuses_per_spw.set(0);
      Bool ddidprob = false;
      for(uInt j = 0; j < nddids; ++j){
        Int spw = spwId(j);
        for(uInt k = 0; k < nSpws; ++k){
          if(spw == spw_p[k]){
            ++nuses_per_spw[spw];
            if(nuses_per_spw[spw_p[k]] == 2){
              ddidprob = true;
              os << LogIO::SEVERE
                 << "Input spw " << spw_p[k] << " was selected for > 1 "
                 << "channel ranges or polarization setups." << LogIO::POST;
            }
            spw2ddid_p[spw] = j;
          }
        }
      }

      if(ddidprob){
        os << LogIO::SEVERE
           << "split does not yet support more than 1 channel range (';' in spw)"
           << "\nor polarization setup per spectral window."
           << LogIO::POST;
        return false;
      }

      Vector<Int> ddids;
      ddids.resize(nSpws);

      inNumCorr_p.resize(nSpws);
      ncorr_p.resize(nSpws);
      for(uInt k = 0; k < nSpws; ++k){
	Int ddid = spw2ddid_p[spw_p[k]];
	
        inNumCorr_p[k] = pols(polId(ddid)).nelements();
	ncorr_p[k] = inPolOutCorrToInCorrMap_p[polId(ddid)].nelements();
	if(ncorr_p[k] == 0){
          os << LogIO::SEVERE
             << "None of the selected correlations are in spectral window "
	     << spw_p[k]
             << LogIO::POST;
          return false;
	}
      }
    }
    
    // Now remake the selected ms
    if(!(exprNode.isNull())){
      mssel_p = MeasurementSet((*elms)(exprNode));
    }
    else{
      // Null take all the ms ...setdata() blank means that
      mssel_p = MeasurementSet((*elms));
    }
    //mssel_p.rename(ms_p.tableName()+"/SELECTED_TABLE", Table::Scratch);
    if(mssel_p.nrow() == 0)
      return false;

    // Setup antNewIndex_p now that mssel_p is ready.
    if(antennaSel_p){
      // Watch out! getAntenna*List() and getBaselineList() return negative
      // numbers for negated antennas!
      //Vector<Int> selAnt1s(thisSelection.getAntenna1List());
      //Vector<Int> selAnt2s(thisSelection.getAntenna2List());
      ScalarColumn<Int> ant1c(mssel_p, MS::columnName(MS::ANTENNA1));
      ScalarColumn<Int> ant2c(mssel_p, MS::columnName(MS::ANTENNA2));
      Vector<Int> selAnts(ant1c.getColumn());
      uInt nAnts = selAnts.nelements();

      selAnts.resize(2 * nAnts, true);
      selAnts(Slice(nAnts, nAnts)) = ant2c.getColumn();
      nAnts = GenSort<Int>::sort(selAnts, Sort::Ascending, Sort::NoDuplicates);
      selAnts.resize(nAnts, true);
      Int maxAnt = max(selAnts);

      if(maxAnt < 0){
        os << LogIO::SEVERE
           << "The maximum selected antenna number, " << maxAnt
           << ", seems to be < 0."
           << LogIO::POST;
        return false;
      }
      antNewIndex_p.resize(maxAnt + 1);
      antNewIndex_p.set(-1); //So if you see -1 in the main, feed, or pointing
			     //tables, fix it
      Bool trivial = true;
      for(uInt k = 0; k < nAnts; ++k){
        trivial &= (selAnts[k] == static_cast<Int>(k));   // trivial = selAnts == indgen(nAnts)
        antNewIndex_p[selAnts[k]] = k;
      }                                  // It is possible to exclude baselines
      antennaSel_p = !trivial;           // without excluding any antennas.
    }                                    // This still gets tripped up by VLA:OUT.
    else{        // Make a default antNewIndex_p.
      antNewIndex_p.resize(mssel_p.antenna().nrow());
      indgen(antNewIndex_p);
    }
      
    if(mssel_p.nrow() < ms_p.nrow()){
      os << LogIO::NORMAL
         << mssel_p.nrow() << " out of " << ms_p.nrow() << " rows are going to be" 
         << " considered due to the selection criteria." 
         << LogIO::POST;
    }
    return true;
  }

  MeasurementSet* SubMS::setupMS(const String& MSFileName, const Int nchan,
                                 const Int nCorr, const String& telescop,
                                 const Vector<MS::PredefinedColumns>& colNames,
                                 const Int obstype,
                                 const Bool compress,
				 const asdmStManUseAlternatives asdmStManUse)
  {
    //Choose an appropriate tileshape
    IPosition dataShape(2, nCorr, nchan);
    IPosition tileShape = MSTileLayout::tileShape(dataShape, obstype, telescop);
    return setupMS(MSFileName, nchan, nCorr, colNames, tileShape.asVector(),
		   compress, asdmStManUse);
    //return setupMS(MSFileName, nchan, nCorr, colNames, tileShape.asVector());
  }
  MeasurementSet* SubMS::setupMS(const String& MSFileName, const Int nchan,
                                 const Int nCorr, 
                                 const Vector<MS::PredefinedColumns>& colNamesTok,
                                 const Vector<Int>& tshape, const Bool compress,
				 const asdmStManUseAlternatives asdmStManUse)
  {
    if(tshape.nelements() != 3)
      throw(AipsError("TileShape has to have 3 elements ") );

    // This is more to shush a compiler warning than to warn users.
    LogIO os(LogOrigin("SubMS", "setupMS()"));
    if(tshape[0] != nCorr)
      os << LogIO::DEBUG1
	 << "Warning: using " << tshape[0] << " from the tileshape instead of "
	 << nCorr << " for the number of correlations."
	 << LogIO::POST;
    if(tshape[1] != nchan)
      os << LogIO::DEBUG1
	 << "Warning: using " << tshape[1] << " from the tileshape instead of "
	 << nchan << " for the number of channels."
	 << LogIO::POST;

    // Choose an appropriate tileshape
    //IPosition dataShape(2,nCorr,nchan);
    //IPosition tileShape = MSTileLayout::tileShape(dataShape,obsType, telescop);
    //////////////////
    
    IPosition tileShape(tshape);

    // Make the MS table
    TableDesc td = MS::requiredTableDesc();
    Vector<String> tiledDataNames;

    // Even though we know the data is going to be the same shape throughout I'll
    // still create a column that has a variable shape as this will permit MS's
    // with other shapes to be appended.
    uInt ncols = colNamesTok.nelements();
    const Bool mustWriteOnlyToData = mustConvertToData(ncols, colNamesTok);
    Bool hasFloatData = False;

    if (mustWriteOnlyToData)
      {
        MS::addColumnToDesc(td, MS::DATA, 2);
	if(asdmStManUse==DONT){
	  if (compress) MS::addColumnCompression(td,MS::DATA,true);
	  String hcolName=String("Tiled")+String("DATA");
	  td.defineHypercolumn(hcolName, 3,
			       stringToVector("DATA"));
	  tiledDataNames.resize(1);
	  tiledDataNames[0] = hcolName;
	}
      }
    else{
      tiledDataNames.resize(ncols);
      for(uInt i = 0; i < ncols; ++i){
	// only add data-like columns here
        // Unfortunately MS::PredefinedColumns aren't ordered so that I can just check if
        // colNamesTok[i] is in the "data range".
        if(colNamesTok[i] == MS::DATA ||
           colNamesTok[i] == MS::MODEL_DATA ||
           colNamesTok[i] == MS::CORRECTED_DATA ||
           colNamesTok[i] == MS::FLOAT_DATA ||
           colNamesTok[i] == MS::LAG_DATA) {
	  MS::addColumnToDesc(td, colNamesTok[i], 2);
	  // compress them unless they are DATA or FLOAT_DATA and the AsdmStMan is in use
	  if(asdmStManUse==DONT ||
	     (colNamesTok[i] != MS::DATA && colNamesTok[i] != MS::FLOAT_DATA)){
	    if (compress) MS::addColumnCompression(td,colNamesTok[i],true);
	  }
	  if (colNamesTok[i] == MS::FLOAT_DATA) {
	    // make a note that FLOAT_DATA is being used, for later.
	    hasFloatData = True;
	  }
        }
        else {
          throw(AipsError(MS::columnName(colNamesTok[i]) +
                          " is not a recognized data column "));
        }
	//  Add tiled versions for these columns except for DATA and FLOAT_DATA when AsdmStMan is in use.
	if(asdmStManUse==DONT ||
	   (colNamesTok[i] != MS::DATA && colNamesTok[i] != MS::FLOAT_DATA)){
	  String hcolName = String("Tiled") + MS::columnName(colNamesTok[i]);
	  td.defineHypercolumn(hcolName, 3,
			       stringToVector(MS::columnName(colNamesTok[i])));
	  tiledDataNames[i] = hcolName;
	}
      }
    }

    //other cols for compression
    if (compress && asdmStManUse!=USE_FOR_DATA_WEIGHT_SIGMA_FLAG) {
      MS::addColumnCompression(td, MS::WEIGHT, true);
      MS::addColumnCompression(td, MS::SIGMA, true);
    }
    
    // add this optional column because random group fits has a
    // weight per visibility
    MS::addColumnToDesc(td, MS::WEIGHT_SPECTRUM, 2);

    // if(asdmStManUse==DONT){
    //     td.defineHypercolumn("TiledDATA",3,
    //                           stringToVector(MS::columnName(MS::DATA)));
    // }
    td.defineHypercolumn("TiledFlagCategory",4,
                         stringToVector(MS::columnName(MS::FLAG_CATEGORY)));
    td.defineHypercolumn("TiledWgtSpectrum",3,
                         stringToVector(MS::columnName(MS::WEIGHT_SPECTRUM)));
    td.defineHypercolumn("TiledUVW",2,
                         stringToVector(MS::columnName(MS::UVW)));
    if(asdmStManUse!=USE_FOR_DATA_WEIGHT_SIGMA_FLAG){
      td.defineHypercolumn("TiledFlag",3,
			   stringToVector(MS::columnName(MS::FLAG)));
      td.defineHypercolumn("TiledWgt",2,
			   stringToVector(MS::columnName(MS::WEIGHT)));
      td.defineHypercolumn("TiledSigma", 2,
			   stringToVector(MS::columnName(MS::SIGMA)));
    }

    SetupNewTable newtab(MSFileName, td, Table::New);
    
    uInt cache_val=32768;
    // Set the default Storage Manager to be the Incr one
    IncrementalStMan incrStMan ("ISMData",cache_val);
    newtab.bindAll(incrStMan, true);
    //Override the binding for specific columns
    
    IncrementalStMan incrStMan0("Array_ID",cache_val);
    newtab.bindColumn(MS::columnName(MS::ARRAY_ID), incrStMan0);
    IncrementalStMan incrStMan1("EXPOSURE",cache_val);
    newtab.bindColumn(MS::columnName(MS::EXPOSURE), incrStMan1);
    IncrementalStMan incrStMan2("FEED1",cache_val);
    newtab.bindColumn(MS::columnName(MS::FEED1), incrStMan2);
    IncrementalStMan incrStMan3("FEED2",cache_val);
    newtab.bindColumn(MS::columnName(MS::FEED2), incrStMan3);
    IncrementalStMan incrStMan4("FIELD_ID",cache_val);
    newtab.bindColumn(MS::columnName(MS::FIELD_ID), incrStMan4);

    // jagonzal (CAS-6746): Don't use IncrementalStMan with RW cols ///////////////////////////////////////////////////
    //IncrementalStMan incrStMan5("FLAG_ROW",cache_val/4);
    //newtab.bindColumn(MS::columnName(MS::FLAG_ROW), incrStMan5);
    StandardStMan aipsStManFlagRow("FLAG_ROW", cache_val/4);
    newtab.bindColumn(MS::columnName(MS::FLAG_ROW), aipsStManFlagRow);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    IncrementalStMan incrStMan6("INTERVAL",cache_val);
    newtab.bindColumn(MS::columnName(MS::INTERVAL), incrStMan6);
    IncrementalStMan incrStMan7("OBSERVATION_ID",cache_val);
    newtab.bindColumn(MS::columnName(MS::OBSERVATION_ID), incrStMan7);
    IncrementalStMan incrStMan8("PROCESSOR_ID",cache_val);
    newtab.bindColumn(MS::columnName(MS::PROCESSOR_ID), incrStMan8);
    IncrementalStMan incrStMan9("SCAN_NUMBER",cache_val);
    newtab.bindColumn(MS::columnName(MS::SCAN_NUMBER), incrStMan9);
    IncrementalStMan incrStMan10("STATE_ID",cache_val);
    newtab.bindColumn(MS::columnName(MS::STATE_ID), incrStMan10);
    IncrementalStMan incrStMan11("TIME",cache_val);
    newtab.bindColumn(MS::columnName(MS::TIME), incrStMan11);
    IncrementalStMan incrStMan12("TIME_CENTROID",cache_val);
    newtab.bindColumn(MS::columnName(MS::TIME_CENTROID), incrStMan12);
    
    // Bind ANTENNA1, ANTENNA2 and DATA_DESC_ID to the standardStMan 
    // as they may change sufficiently frequently to make the
    // incremental storage manager inefficient for these columns.
    
      
    StandardStMan aipsStMan0("ANTENNA1", cache_val);
    newtab.bindColumn(MS::columnName(MS::ANTENNA1), aipsStMan0);
    StandardStMan aipsStMan1("ANTENNA2", cache_val);
    newtab.bindColumn(MS::columnName(MS::ANTENNA2), aipsStMan1);
    StandardStMan aipsStMan2("DATA_DESC_ID", cache_val);
    newtab.bindColumn(MS::columnName(MS::DATA_DESC_ID), aipsStMan2);
    
     
    //    itsLog << LogOrigin("MSFitsInput", "setupMeasurementSet");
    //itsLog << LogIO::NORMAL << "Using tile shape "<<tileShape <<" for "<<
    //  array_p<<" with obstype="<< obsType<<LogIO::POST;
    
    //    TiledShapeStMan tiledStMan1("TiledData",tileShape);
    TiledShapeStMan tiledStMan1f("TiledFlag",tileShape);
    TiledShapeStMan tiledStMan1fc("TiledFlagCategory",
                                  IPosition(4,tileShape(0),tileShape(1),1,
                                            tileShape(2)));
    TiledShapeStMan tiledStMan2("TiledWgtSpectrum",tileShape);
    TiledColumnStMan tiledStMan3("TiledUVW",IPosition(2, 3, (tileShape(0) * tileShape(1) * tileShape(2)) / 3));
    TiledShapeStMan tiledStMan4("TiledWgt", 
                                IPosition(2,tileShape(0), tileShape(1) * tileShape(2)));
    TiledShapeStMan tiledStMan5("TiledSigma", 
                                IPosition(2,tileShape(0), tileShape(1) * tileShape(2)));
    
    // Bind the DATA or FLOAT_DATA, FLAG & WEIGHT_SPECTRUM columns to the tiled stman or asdmStMan

    AsdmStMan sm;

    if (mustWriteOnlyToData){
      if(asdmStManUse==DONT){
	TiledShapeStMan tiledStMan1Data("TiledDATA",tileShape);
      
	newtab.bindColumn(MS::columnName(MS::DATA), tiledStMan1Data);
      }
      else{
	if (hasFloatData) {
	  newtab.bindColumn(MS::columnName(MS::FLOAT_DATA), sm);
	} else{
	  newtab.bindColumn(MS::columnName(MS::DATA), sm);
	}
      }
    }
    else{
      for(uInt i = 0; i < ncols; ++i){
        TiledShapeStMan tiledStMan1Data(tiledDataNames[i], tileShape);
        
        newtab.bindColumn(MS::columnName(colNamesTok[i]), tiledStMan1Data);
      }
      if(asdmStManUse!=DONT){
	if (hasFloatData) {
	  newtab.bindColumn(MS::columnName(MS::FLOAT_DATA), sm);
	} else {
	  newtab.bindColumn(MS::columnName(MS::DATA), sm);
	}
      }
    }
    newtab.bindColumn(MS::columnName(MS::FLAG_CATEGORY),tiledStMan1fc);
    newtab.bindColumn(MS::columnName(MS::WEIGHT_SPECTRUM),tiledStMan2);
    
    newtab.bindColumn(MS::columnName(MS::UVW),tiledStMan3);
    if(asdmStManUse==USE_FOR_DATA_WEIGHT_SIGMA_FLAG){
      newtab.bindColumn(MS::columnName(MS::FLAG),sm);
      newtab.bindColumn(MS::columnName(MS::WEIGHT),sm);
      newtab.bindColumn(MS::columnName(MS::SIGMA),sm);      
    }
    else{
      newtab.bindColumn(MS::columnName(MS::FLAG),tiledStMan1f);
      newtab.bindColumn(MS::columnName(MS::WEIGHT),tiledStMan4);
      newtab.bindColumn(MS::columnName(MS::SIGMA),tiledStMan5);
    }
      

    // avoid lock overheads by locking the table permanently
    TableLock lock(TableLock::AutoLocking);
    MeasurementSet *ms = new MeasurementSet (newtab,lock);
    
    // Set up the subtables for the UVFITS MS
    // we make new tables with 0 rows
    Table::TableOption option=Table::New;
    createSubtables(*ms, option); 

    { // Set the TableInfo
      TableInfo& info(ms->tableInfo());
      info.setType(TableInfo::type(TableInfo::MEASUREMENTSET));
      info.setSubType(String("UVFITS"));
      info.readmeAddLine
        ("This is a measurement set Table holding astronomical observations");
    }
    return ms;
  }
  
  
  Bool SubMS::fillDDTables()
  {  
    LogIO os(LogOrigin("SubMS", "fillDDTables()"));
    
    MSSpWindowColumns inSpWCols(mssel_p.spectralWindow());
    MSSpWindowColumns& msSpW(msc_p->spectralWindow());
    // Detect which optional columns of SPECTRAL_WINDOW are present.
    // inSpWCols and msSpW should agree because addOptionalColumns() was done
    // for SPECTRAL_WINDOW in fillAllTables() before making msc_p or calling
    // fillDDTables.
    Bool haveSpwAN  = inSpWCols.assocNature().hasContent();
    Bool haveSpwASI = inSpWCols.assocSpwId().hasContent();
    Bool haveSpwBN  = inSpWCols.bbcNo().hasContent();
    Bool haveSpwBS  = inSpWCols.bbcSideband().hasContent();
    Bool haveSpwDI  = inSpWCols.dopplerId().hasContent();

    MSDataDescColumns& msDD(msc_p->dataDescription());
    MSPolarizationColumns& msPol(msc_p->polarization());
    
    //DD table
    const MSDataDescription ddtable = mssel_p.dataDescription();
    ScalarColumn<Int> polId(ddtable, 
			      MSDataDescription::columnName(MSDataDescription::POLARIZATION_ID));
    
    //Fill in matching spw to datadesc in old ms 
    {
      MSDataDescColumns msOldDD(ddtable);
      oldDDSpwMatch_p=msOldDD.spectralWindowId().getColumn();
    }

    //POLARIZATION table    
    const MSPolarization poltable= mssel_p.polarization();
    ScalarColumn<Int> numCorr (poltable, 
				 MSPolarization::columnName(MSPolarization::NUM_CORR));
    ArrayColumn<Int> corrType(poltable, 
				MSPolarization::columnName(MSPolarization::CORR_TYPE));
    ArrayColumn<Int> corrProd(poltable, MSPolarization::columnName(MSPolarization::CORR_PRODUCT));
    ScalarColumn<Bool> polFlagRow(poltable, MSPolarization::columnName(MSPolarization::FLAG_ROW));
    
    spwRelabel_p.resize(mscIn_p->spectralWindow().nrow());
    spwRelabel_p.set(-1);
    
    inNumChan_p.resize(spw_p.nelements()); 
    
    polID_p = polId.getColumn();
    Bool dum;
    Sort sort( polID_p.getStorage(dum),sizeof(Int) );
    sort.sortKey((uInt)0,TpInt);
    Vector<uInt> index,uniq;
    sort.sort(index,polID_p.nelements());
    uInt nPol = sort.unique(uniq,index);
    Vector<Int> selectedPolId(nPol); 	// Map from output polID to input polID.
    for(uInt k = 0; k < nPol; ++k)
      selectedPolId[k] = polID_p[index[uniq[k]]];
    
    // Make map from input to output spws.
    Sort sortSpws(spw_p.getStorage(dum), sizeof(Int));
    sortSpws.sortKey((uInt)0, TpInt);
    Vector<uInt> spwsortindex, spwuniqinds;
    sortSpws.sort(spwsortindex, spw_p.nelements());
    uInt nuniqSpws = sortSpws.unique(spwuniqinds, spwsortindex);
    spw_uniq_p.resize(nuniqSpws);
    for(uInt k = 0; k < nuniqSpws; ++k){
      spw_uniq_p[k] = spw_p[spwuniqinds[k]];
      spwRelabel_p[spw_uniq_p[k]] = k;
    }
    if(nuniqSpws < spw_p.nelements()){
      os << LogIO::WARN
         << "Multiple channel ranges within an spw may not work.  SOME DATA MAY BE OMITTED!"
         << "\nConsider splitting them individually and optionally combining the output MSes with concat."
         << "\nEven then, expect problems if exporting to uvfits."
         << LogIO::POST;
    }

    Vector<Int> newPolId(nuniqSpws);
    for(uInt k = 0; k < nuniqSpws; ++k){
      Bool found = false;
      
      for(uInt j = 0; j < nPol; ++j){ 
	if(selectedPolId[j] == polID_p[spw2ddid_p[spw_uniq_p[k]]]){
	  newPolId[k] = j;
	  found = true;
	  break;
	}
      }
      if(!found){
	os << LogIO::SEVERE
	   << "No polarization ID found for output polarization setup " << k
	   << LogIO::POST;
	return false;
      }
    }
    corrSlice_p.resize(nPol);
    for(uInt outpid = 0; outpid < nPol; ++outpid){
      uInt inpid = selectedPolId[outpid];
      uInt ncorr = inPolOutCorrToInCorrMap_p[inpid].nelements();
      const Vector<Int> inCT(corrType(inpid));
      
      // ncorr will be 0 if none of the selected spws have this pid. 
      if(ncorr > 0 && ncorr < inCT.nelements()){
	keepShape_p = false;

	// Check whether the requested correlations can be accessed by slicing.
	// That means there must be a constant stride.  The most likely (only?)
	// way to violate that is to ask for 3 out of 4 correlations.
	if(ncorr > 2){
	  os << LogIO::SEVERE
	     << "Sorry, the requested correlation selection is not unsupported.\n"
	     << "Try selecting fewer or all of the correlations."
	     << LogIO::POST;
	  return false;
	}

	corrSlice_p[outpid] = Slice(inPolOutCorrToInCorrMap_p[inpid][0],
			       ncorr,
			       ncorr > 1 ? inPolOutCorrToInCorrMap_p[inpid][1] -
			                   inPolOutCorrToInCorrMap_p[inpid][0] :
			       1);
      }
      else
	corrSlice_p[outpid] = Slice(0, ncorr);
      
      msOut_p.polarization().addRow();
      msPol.numCorr().put(outpid, ncorr);
      msPol.flagRow().put(outpid, polFlagRow(inpid));

      Vector<Int> outCT;
      const Matrix<Int> inCP(corrProd(inpid));
      Matrix<Int> outCP;
      outCT.resize(ncorr);
      outCP.resize(2, ncorr);
      for(uInt k = 0; k < ncorr; ++k){
	Int inCorrInd = inPolOutCorrToInCorrMap_p[inpid][k];
	
	outCT[k] = inCT[inCorrInd];
	for(uInt feedind = 0; feedind < 2; ++feedind)
	  outCP(feedind, k) = inCP(feedind, inCorrInd);
      }
      msPol.corrType().put(outpid, outCT);
      msPol.corrProduct().put(outpid, outCP);
    }

    for(uInt k = 0; k < spw_p.nelements(); ++k)
      inNumChan_p[k] = inSpWCols.numChan()(spw_p[k]);
    
    Vector<Vector<Int> > spwinds_of_uniq_spws(nuniqSpws);

    totnchan_p.resize(nuniqSpws);
    for(uInt k = 0; k < nuniqSpws; ++k){
      Int maxchan = 0;
      uInt j = 0;

      msOut_p.spectralWindow().addRow();
      msOut_p.dataDescription().addRow();

      totnchan_p[k] = 0;
      spwinds_of_uniq_spws[k].resize();
      for(uInt spwind = 0; spwind < spw_p.nelements(); ++spwind){
        if(spw_p[spwind] == spw_uniq_p[k]){
          Int highchan = nchan_p[spwind] * chanStep_p[spwind]
            + chanStart_p[spwind];

          if(highchan > maxchan)
            maxchan = highchan;

          totnchan_p[k] += nchan_p[spwind];

          // The true is necessary to avoid scrambling previously assigned
          // values.
          spwinds_of_uniq_spws[k].resize(j + 1, true);

          // Warning!  spwinds_of_uniq_spws[k][j] will compile without warning,
          // but dump core at runtime.
          (spwinds_of_uniq_spws[k])[j] = spwind;
          ++j;
        }
      }
      if(maxchan > inSpWCols.numChan()(spw_uniq_p[k])){
        os << LogIO::SEVERE
           << " Channel settings wrong; exceeding number of channels in spw "
           << spw_uniq_p[k] << LogIO::POST;
        return false;
      }
    }

    // min_k is an index for getting an spw index via spw_uniq_p[min_k].
    // k is an index for getting an spw index via spw_p[k].
    for(uInt min_k = 0; min_k < nuniqSpws; ++min_k){
      uInt k = spwinds_of_uniq_spws[min_k][0];

      if(spwinds_of_uniq_spws[min_k].nelements() > 1 ||
         nchan_p[k] != inSpWCols.numChan()(spw_p[k])){
        Vector<Double> effBWIn = inSpWCols.effectiveBW()(spw_uniq_p[min_k]);
	Int nOutChan = totnchan_p[min_k];
        Vector<Double> chanFreqOut(nOutChan);
        Vector<Double> chanFreqIn = inSpWCols.chanFreq()(spw_uniq_p[min_k]);
        Vector<Double> chanWidthOut(nOutChan);
        Vector<Double> chanWidthIn = inSpWCols.chanWidth()(spw_uniq_p[min_k]);
        Vector<Double> spwResolOut(nOutChan);
        Vector<Double> spwResolIn = inSpWCols.resolution()(spw_uniq_p[min_k]);
        Vector<Double> effBWOut(nOutChan);
        Int outChan = 0;

        keepShape_p = false;

        // The sign of CHAN_WIDTH defaults to +.  Its determination assumes
        // that chanFreqIn is monotonic, but not that the sign of the
        // chanWidthIn is correct.
        Bool neginc = chanFreqIn[chanFreqIn.nelements() - 1] < chanFreqIn[0];

        effBWOut.set(0.0);
        Double totalBW = 0.0;
        for(uInt rangeNum = 0;
            rangeNum < spwinds_of_uniq_spws[min_k].nelements(); ++rangeNum){
          k = spwinds_of_uniq_spws[min_k][rangeNum];

          Int span = chanStep_p[k] * widths_p[k];

          for(Int j = 0; j < nchan_p[k]; ++j){
            Int inpChan = chanStart_p[k] + j * span;

            if(span > 1){
              Int lastChan = inpChan + span - 1;

              if(lastChan > chanEnd_p[k]){
                // The averaging width is not a factor of the number of
                // selected input channels, so the last output bin receives
                // fewer input channels than the other bins.
                lastChan = chanEnd_p[k];

                Int nchan = lastChan - inpChan + 1;
                os << LogIO::NORMAL
                   << "The last output channel of spw " << spw_p[k]
                   << " has only " << nchan << " input channel";
                if(nchan > 1)
                  os << "s.";
                //else
                //  os << ".\nRemember that MS selection ranges (unlike Python), *include* the last number.";
                os << LogIO::POST;
                //os << LogIO::WARN
                //   << "You will not be able to export an MS where the width varies by channel to UVFITS!"
                //   << LogIO::POST;
              }

              chanFreqOut[outChan] = (chanFreqIn[inpChan] +
                                      chanFreqIn[lastChan]) / 2;

              Double sep = chanFreqIn[lastChan] - chanFreqIn[inpChan];

              if(neginc)
                sep = -sep;

              // The internal abs is necessary because the sign of chanWidthIn
              // may be wrong.
              chanWidthOut[outChan] = sep + 0.5 * abs(chanWidthIn[inpChan] +
                                                      chanWidthIn[lastChan]);
              if(neginc)
                chanWidthOut[outChan] = -chanWidthOut[outChan];
            
              spwResolOut[outChan] = 0.5 * (spwResolIn[inpChan] +
                                            spwResolIn[lastChan])
                                     + sep;

              for(Int avgChan = inpChan; avgChan <= lastChan;
                  avgChan += chanStep_p[k])
                effBWOut[outChan] += effBWIn[avgChan];
            }
            else{
              chanFreqOut[outChan] = chanFreqIn[inpChan];
              spwResolOut[outChan] = spwResolIn[inpChan];
              chanWidthOut[outChan] = chanWidthIn[inpChan];
              effBWOut[outChan]    = effBWIn[inpChan];
            }
            totalBW += effBWOut[outChan];
            ++outChan;
          }
        }
        --outChan;

        msSpW.chanFreq().put(min_k, chanFreqOut);
	msSpW.refFrequency().put(min_k, min(chanFreqOut[0],chanFreqOut[chanFreqOut.size()-1]));
        msSpW.resolution().put(min_k, spwResolOut);
        msSpW.numChan().put(min_k, nOutChan);
        msSpW.chanWidth().put(min_k, chanWidthOut);
        msSpW.effectiveBW().put(min_k, spwResolOut);
        msSpW.totalBandwidth().put(min_k, totalBW);
      }
      else{
	msSpW.chanFreq().put(min_k, inSpWCols.chanFreq()(spw_p[k]));
	msSpW.refFrequency().put(min_k, inSpWCols.refFrequency()(spw_p[k]));
	msSpW.resolution().put(min_k, inSpWCols.resolution()(spw_p[k]));
	msSpW.numChan().put(min_k, inSpWCols.numChan()(spw_p[k]));    
	msSpW.chanWidth().put(min_k, inSpWCols.chanWidth()(spw_p[k]));
	msSpW.effectiveBW().put(min_k, inSpWCols.effectiveBW()(spw_p[k]));
	msSpW.totalBandwidth().put(min_k, inSpWCols.totalBandwidth()(spw_p[k]));
      }
      
      msSpW.flagRow().put(min_k, inSpWCols.flagRow()(spw_p[k]));
      msSpW.freqGroup().put(min_k, inSpWCols.freqGroup()(spw_p[k]));
      msSpW.freqGroupName().put(min_k, inSpWCols.freqGroupName()(spw_p[k]));
      msSpW.ifConvChain().put(min_k, inSpWCols.ifConvChain()(spw_p[k]));
      msSpW.measFreqRef().put(min_k, inSpWCols.measFreqRef()(spw_p[k]));
      msSpW.name().put(min_k, inSpWCols.name()(spw_p[k]));
      msSpW.netSideband().put(min_k, inSpWCols.netSideband()(spw_p[k]));
      if(haveSpwAN)
        msSpW.assocNature().put(min_k, inSpWCols.assocNature()(spw_p[k]));
      if(haveSpwASI)
        msSpW.assocSpwId().put(min_k, inSpWCols.assocSpwId()(spw_p[k]));
      if(haveSpwBN)
        msSpW.bbcNo().put(min_k, inSpWCols.bbcNo()(spw_p[k]));
      if(haveSpwBS)
        msSpW.bbcSideband().put(min_k, inSpWCols.bbcSideband()(spw_p[k]));
      if(haveSpwDI)
        msSpW.dopplerId().put(min_k, inSpWCols.dopplerId()(spw_p[k]));

      msDD.flagRow().put(min_k, false);
      msDD.polarizationId().put(min_k, newPolId[min_k]);
      msDD.spectralWindowId().put(min_k, min_k);
    }
    return true;
  }
  
  Bool SubMS::fillFieldTable() 
  {  
    LogIO os(LogOrigin("SubMS", "fillFieldTable()"));
    
    //MSField fieldtable= mssel_p.field();
    // optionalCols[0] = "EPHEMERIS_ID";
    uInt nAddedCols = addOptionalColumns(mssel_p.field(), msOut_p.field(),
                                         true);

    MSFieldColumns msField(msOut_p.field());

    const MSFieldColumns& fieldIn = mscIn_p->field(); 
    ScalarColumn<String> code(fieldIn.code());
    ArrayColumn<Double>  delayDir(fieldIn.delayDir());
    ScalarColumn<Bool>   flagRow(fieldIn.flagRow());
    ScalarColumn<String> name(fieldIn.name());
    ScalarColumn<Int>    numPoly(fieldIn.numPoly());
    ArrayColumn<Double>  phaseDir(fieldIn.phaseDir());
    ArrayColumn<Double>  refDir(fieldIn.referenceDir());
    ScalarColumn<Int>    sourceId(fieldIn.sourceId());
    ScalarColumn<Double> time(fieldIn.time());
    
    String refstr;
    String nameVarRefColDelayDir, nameVarRefColPhaseDir, nameVarRefColRefDir;

    // Need to correctly define the direction measures.
    // DelayDir
    if(delayDir.keywordSet().asRecord("MEASINFO").isDefined("Ref")){
      delayDir.keywordSet().asRecord("MEASINFO").get("Ref", refstr);
      //  MDirection::getType(dir1, refstr);
      msField.delayDir().rwKeywordSet().asrwRecord("MEASINFO").define("Ref",refstr);
    }
    if(delayDir.keywordSet().asRecord("MEASINFO").isDefined("VarRefCol")){ // it's a variable ref. column
      delayDir.keywordSet().asRecord("MEASINFO").get("VarRefCol", refstr);
      msField.delayDir().rwKeywordSet().asrwRecord("MEASINFO").define("VarRefCol",refstr);
      nameVarRefColDelayDir = refstr;
      Vector<String> refTypeV;
      delayDir.keywordSet().asRecord("MEASINFO").get("TabRefTypes", refTypeV);
      msField.delayDir().rwKeywordSet().asrwRecord("MEASINFO").define("TabRefTypes",refTypeV);      
      Vector<uInt> refCodeV;
      delayDir.keywordSet().asRecord("MEASINFO").get("TabRefCodes", refCodeV);
      msField.delayDir().rwKeywordSet().asrwRecord("MEASINFO").define("TabRefCodes",refCodeV);      
      Int refid=msField.delayDir().keywordSet().asRecord("MEASINFO").fieldNumber("Ref");
      if(refid>=0){ // erase the redundant Ref keyword
	msField.delayDir().rwKeywordSet().asrwRecord("MEASINFO").removeField(RecordFieldId(refid));
      }
    }
    // PhaseDir
    if(phaseDir.keywordSet().asRecord("MEASINFO").isDefined("Ref")){
      phaseDir.keywordSet().asRecord("MEASINFO").get("Ref", refstr);
      msField.phaseDir().rwKeywordSet().asrwRecord("MEASINFO").define("Ref", refstr);
    }
    if(phaseDir.keywordSet().asRecord("MEASINFO").isDefined("VarRefCol")){ // it's a variable ref. column
      phaseDir.keywordSet().asRecord("MEASINFO").get("VarRefCol", refstr);
      msField.phaseDir().rwKeywordSet().asrwRecord("MEASINFO").define("VarRefCol",refstr);
      nameVarRefColPhaseDir = refstr;
      Vector<String> refTypeV;
      phaseDir.keywordSet().asRecord("MEASINFO").get("TabRefTypes", refTypeV);
      msField.phaseDir().rwKeywordSet().asrwRecord("MEASINFO").define("TabRefTypes",refTypeV);      
      Vector<uInt> refCodeV;
      phaseDir.keywordSet().asRecord("MEASINFO").get("TabRefCodes", refCodeV);
      msField.phaseDir().rwKeywordSet().asrwRecord("MEASINFO").define("TabRefCodes",refCodeV);      
      Int refid=msField.phaseDir().keywordSet().asRecord("MEASINFO").fieldNumber("Ref");
      if(refid>=0){
	msField.phaseDir().rwKeywordSet().asrwRecord("MEASINFO").removeField(RecordFieldId(refid));
      }
    }
    // ReferenceDir
    if(refDir.keywordSet().asRecord("MEASINFO").isDefined("Ref")){
      refDir.keywordSet().asRecord("MEASINFO").get("Ref", refstr);
      msField.referenceDir().rwKeywordSet().asrwRecord("MEASINFO").define("Ref",refstr);
    }
    if(refDir.keywordSet().asRecord("MEASINFO").isDefined("VarRefCol")){ // it's a variable ref. column
      refDir.keywordSet().asRecord("MEASINFO").get("VarRefCol", refstr);
      msField.referenceDir().rwKeywordSet().asrwRecord("MEASINFO").define("VarRefCol",refstr);
      nameVarRefColRefDir = refstr;
      Vector<String> refTypeV;
      refDir.keywordSet().asRecord("MEASINFO").get("TabRefTypes", refTypeV);
      msField.referenceDir().rwKeywordSet().asrwRecord("MEASINFO").define("TabRefTypes",refTypeV);      
      Vector<uInt> refCodeV;
      refDir.keywordSet().asRecord("MEASINFO").get("TabRefCodes", refCodeV);
      msField.referenceDir().rwKeywordSet().asrwRecord("MEASINFO").define("TabRefCodes",refCodeV);
      Int refid=msField.referenceDir().keywordSet().asRecord("MEASINFO").fieldNumber("Ref");
      if(refid>=0){
	msField.referenceDir().rwKeywordSet().asrwRecord("MEASINFO").removeField(RecordFieldId(refid));
      }
    }

    // ...and the time measure...
    time.keywordSet().asRecord("MEASINFO").get("Ref", refstr);
    msField.time().rwKeywordSet().asrwRecord("MEASINFO").define("Ref", refstr);

    // fieldRelabel_p size: nrow of a input MS, -1 for unselected field ids 
    fieldRelabel_p.resize(mscIn_p->field().nrow());
    fieldRelabel_p.set(-1);

    os << LogIO::DEBUG1
       << fieldid_p.nelements() << " fields selected out of "
       << mscIn_p->field().nrow()
       << LogIO::POST;

    try{
      msOut_p.field().addRow(fieldid_p.nelements());
      for(uInt k = 0; k < fieldid_p.nelements(); ++k){
        fieldRelabel_p[fieldid_p[k]] = k;
      
        msField.code().put(k, code(fieldid_p[k]));
        msField.delayDir().put(k, delayDir(fieldid_p[k]));
        msField.flagRow().put(k, flagRow(fieldid_p[k]));
        msField.name().put(k, name(fieldid_p[k]));
        msField.numPoly().put(k, numPoly(fieldid_p[k]));
        msField.phaseDir().put(k, phaseDir(fieldid_p[k]));
        msField.referenceDir().put(k, refDir(fieldid_p[k]));
        msField.time().put(k, time(fieldid_p[k]));

        Int inSrcID = sourceId(fieldid_p[k]);
        if(inSrcID < 0)
          msField.sourceId().put(k, -1);
        else
          msField.sourceId().put(k, sourceRelabel_p[inSrcID]);
      }

      if(nAddedCols > 0){

        ScalarColumn<Int> eID(fieldIn.ephemerisId());
	if(eID.hasContent()){
	  String destPathName = Path(msOut_p.field().tableName()).absoluteName();
	  for(uInt k = 0; k < fieldid_p.nelements(); ++k){

	    Int theEphId = eID(fieldid_p[k]);

	    if(theEphId>-1){ // there is an ephemeris attached to this field
	      Path ephPath = Path(fieldIn.ephemPath(fieldid_p[k]));
	      if(ephPath.length()>0){ // copy the ephemeris table over to the output FIELD table
		Directory origEphemDir(ephPath);
		origEphemDir.copy(destPathName+"/"+ephPath.baseName());
		os << LogIO::NORMAL << "Transferring ephemeris " << ephPath.baseName() 
		   << " for output field " << name(fieldid_p[k]) << LogIO::POST;
	      }
	    } 
	    msField.ephemerisId().put(k, theEphId);
	  }
	}

	if(!nameVarRefColDelayDir.empty()){ // need to copy the reference column
	  ScalarColumn<Int>  dM(mssel_p.field(), nameVarRefColDelayDir);
	  ScalarColumn<Int> cdMDirRef(msOut_p.field(), nameVarRefColDelayDir);
	  for(uInt k = 0; k < fieldid_p.nelements(); ++k){
	    cdMDirRef.put(k, dM(fieldid_p[k]));
	  }
	}
	if(!nameVarRefColPhaseDir.empty()){ // need to copy the reference column
	  ScalarColumn<Int>  dM(mssel_p.field(), nameVarRefColPhaseDir);
	  ScalarColumn<Int> cdMDirRef(msOut_p.field(), nameVarRefColPhaseDir);
	  for(uInt k = 0; k < fieldid_p.nelements(); ++k){
	    cdMDirRef.put(k, dM(fieldid_p[k]));
	  }
	}
	if(!nameVarRefColRefDir.empty()){ // need to copy the reference column
	  ScalarColumn<Int>  dM(mssel_p.field(), nameVarRefColRefDir);
	  ScalarColumn<Int> cdMDirRef(msOut_p.field(), nameVarRefColRefDir);
	  for(uInt k = 0; k < fieldid_p.nelements(); ++k){
	    cdMDirRef.put(k, dM(fieldid_p[k]));
	  }
	}
      }

    }
    catch(AipsError x){
      os << LogIO::EXCEPTION
         << "Error " << x.getMesg() << " setting up the output FIELD table."
         << LogIO::POST;
    }
    catch(...){
      throw(AipsError("Unknown exception caught and released in fillFieldTable()"));
    }
    
    return true;    
  }

  // Sets up sourceRelabel_p for mapping input SourceIDs (if any) to output
  // ones.  Must be called after fieldid_p is set and before calling
  // fillFieldTable() or copySource().
  void SubMS::relabelSources()
  {
    LogIO os(LogOrigin("SubMS", "relabelSources()"));

    //Source is an optional table, so it may not exist
    if(Table::isReadable(mssel_p.sourceTableName())){
      // Note that mscIn_p->field().sourceId() has ALL of the sourceIDs in
      // the input MS, not just the selected ones.
      const Vector<Int>& inSrcIDs = mscIn_p->field().sourceId().getColumn();

      Int highestInpSrc = max(inSrcIDs);
    
      if(highestInpSrc < 0)                   // Ensure space for -1.
        highestInpSrc = 0;
      sourceRelabel_p.resize(highestInpSrc + 1);
      sourceRelabel_p.set(-1);   	          // Default to "any".

      // Enable sourceIDs that are actually referred to by selected fields, and
      // remap them using j.
      uInt j = 0;
      for(uInt k = 0; k < fieldid_p.nelements(); ++k){
        Int fldInSrcID = inSrcIDs[fieldid_p[k]];
        
        if(fldInSrcID > -1){
          if(sourceRelabel_p[fldInSrcID] == -1){ // Multiple fields can use the same
            sourceRelabel_p[fldInSrcID] = j;     // source in a mosaic.
            ++j;
          }
        }
      }
    }
    else{
      os << LogIO::NORMAL
         << "The input MS does not have the optional SOURCE table.\n"
         << "-1 will be used as a generic source ID."
         << LogIO::POST;
      sourceRelabel_p.resize(1);
      sourceRelabel_p.set(-1);   	          // Default to "any".      
    }
  }

  // This method is currently not called in SubMS.  It should really be called
  // in setupMS, but that has been made into a static method and it cannot be
  // called there.  The ms argument is unused, but it is there to preserve the
  // signature, and is commented to prevent a compiler warning.
  //
  void SubMS::verifyColumns(const MeasurementSet&, // ms,
                            const Vector<MS::PredefinedColumns>& colNames)
  {
    LogIO os(LogOrigin("SubMS", "verifyColumns()"));
    for(uInt i=0;i<colNames.nelements();i++)
      if (!ms_p.tableDesc().isColumn(MS::columnName(colNames[i])))
	{
	  ostringstream ostr;
	  ostr << "Desired column (" << MS::columnName(colNames[i])
               << ") not found in the input MS (" << ms_p.tableName() << ").";
	  throw(AipsError(ostr.str()));
	}
  }

  Int SubMS::regridSpw(String& regridMessage,
		       const String& outframe,
		       const String& regridQuant,
		       const Double regridVeloRestfrq,
		       const String& regridInterpMeth,
		       const Double regridCenter, 
		       const Double regridBandwidth, 
		       const Double regridChanWidth,
		       const Bool doHanningSmooth,
		       const Int phaseCenterFieldId,
		       MDirection phaseCenter,
		       const Bool centerIsStart,
		       const Bool startIsEnd,
		       const Int nchan,
		       const Int width,
		       const Int start
		  ){
    
    LogIO os(LogOrigin("SubMS", "regridSpw()"));

    Int rval = -1; // return values: -1 = MS not modified, 1 = MS modified and OK, 
                   // 0 = MS modified but not OK 

    // get the original table description of the MS 
    TableDesc origMSTD(ms_p.actualTableDesc());
    if(!origMSTD.isColumn("CORRECTED_DATA") &&  !origMSTD.isColumn("DATA") 
       && !origMSTD.isColumn("FLOAT_DATA") && !origMSTD.isColumn("MODEL_DATA")){ 
      os << LogIO::WARN << "MS has no DATA columns. Nothing to regrid." << LogIO::POST;
    }	  

    // Set up a little database to keep track of which pairs (FieldId, SPWId) have already
    // been dealt with and what parameters were used

    vector<Int> oldSpwId;
    vector<Int> oldFieldId;
    vector<Int> newDataDescId;
    vector<Bool> regrid;
    vector<Bool> transform;
    vector<MDirection> theFieldDirV;
    vector<MPosition> mObsPosV;
    vector<MFrequency::Types> fromFrameTypeV; // original ref frame of the SPW
    vector<MFrequency::Ref> outFrameV; // new ref frame
    vector<MRadialVelocity> outRadVelV; // radial velocity correction applied to the new ref frame
    vector<Double> weightScaleV; // the scaling factor for the WEIGHTs
    vector< Vector<Double> > xold; // the frequencies of the original SPW in the old ref frame
    vector< Vector<Double> > xout; // the frequencies of the new SPW in the new ref frame
    vector< Vector<Double> > xin;  // the frequencies of the old SPW in the new ref frame
    vector< Int > method; // interpolation method cast to Int
    

    Bool msModified = false;

    String oframe = outframe;
    oframe.upcase();
    Bool doRadVelCorr = (oframe=="SOURCE");

    // Loop 1: Verify the input parameters, no modification of the MS
    if(!setRegridParameters(oldSpwId,
			    oldFieldId,
			    newDataDescId,
			    regrid,
			    transform,
			    theFieldDirV,
			    mObsPosV,
			    fromFrameTypeV,
			    outFrameV,
			    outRadVelV,
			    weightScaleV,
			    xold,
			    xout, 
			    xin, 
			    method,
			    msModified,
			    outframe,
			    regridQuant,
			    regridVeloRestfrq,
			    regridInterpMeth,
			    regridCenter,
			    regridBandwidth, 
			    regridChanWidth,
			    phaseCenterFieldId,
			    phaseCenter,
			    false, // <-----
			    os,
			    regridMessage,
			    centerIsStart,
			    startIsEnd,
			    nchan,
			    width,
			    start
			    )){ // an error occured
      return -1;
    }

    if(oframe=="SOURCE" || oframe=="GEO"){
      os << LogIO::NORMAL
	 << "Note: with outframe==GEO or outframe==SOURCE, the resulting spectral windows with be labeled as having reference frame REST.\n"
	 << "      All integrations will be Doppler-tracked to correspond to the GEO frame at the beginning of the observation\n"
	 << "      (with radial velocity corrections in the case of outframe==SOURCE).\n"
	 << "      No subsequent regridding to other reference frames will be possible."
	 << LogIO::POST;
    }

    vector<Double> sigmaScaleV(weightScaleV.size());
    for(uInt i=0; i<weightScaleV.size(); i++){
      if(weightScaleV[i]<=0.){
	os << LogIO::WARN << "Internal error: Encountered non-positive weight scaling factor " << weightScaleV[i] <<endl
	   << "Aborting." << LogIO::POST;
	return -1;
      }
      else if(doHanningSmooth){
	weightScaleV[i] *= 1.32;
      }
      sigmaScaleV[i] = 1./sqrt(weightScaleV[i]);
    }

    // Loop 2: Write modified DD, SPW, and SOURCE tables

    if(!setRegridParameters(oldSpwId,
			    oldFieldId,
			    newDataDescId,
			    regrid,
			    transform,
			    theFieldDirV,
			    mObsPosV,
			    fromFrameTypeV,
			    outFrameV,
			    outRadVelV,
			    weightScaleV,
			    xold,
			    xout, 
			    xin, 
			    method,
			    msModified,
			    outframe,
			    regridQuant,
			    regridVeloRestfrq,
			    regridInterpMeth,
			    regridCenter,
			    regridBandwidth, 
			    regridChanWidth,
			    phaseCenterFieldId,
			    phaseCenter,
			    true, // <-----
			    os,
			    regridMessage,
			    centerIsStart,
			    startIsEnd,
			    nchan,
			    width,
			    start
			    )){ // an error occured
      if(msModified){
	return 0;
      }
      else{
	return -1;
      }
    }
    
    if(!msModified){ // nothing to be done in terms of regridding
      if(doHanningSmooth){ // but we still need to Hanning smooth
	os << LogIO::NORMAL << "Hanning smoothing not applied in regridding step since no regridding was necessary." <<  LogIO::POST;
      }
      return -1;
    }
    
    // now we need to modify the main table ...

    Bool needRegridding = false;
    for(uInt i=0; i<regrid.size(); i++){
      if(regrid[i]){
	needRegridding = true;
      }
    }

    if(needRegridding){

      os << LogIO::NORMAL << "Main table data array columns will be rewritten." <<  LogIO::POST;	    

      // create the "partner" columns, i.e. rename the old array columns to old...
      // and create new empty columns with the original names to hold the regridded values

      MSMainColumns mCols(ms_p);
      Int nCorr = mCols.data().shape(0)(0); // the first dimension of DATA
      IPosition dataShape(2, nCorr, xout[0].size());
      Int obstype = 0; // default
      MSObservationColumns obsCols(ms_p.observation());
      String telescop = obsCols.telescopeName()(mCols.observationId()(0));
      IPosition tileShape = MSTileLayout::tileShape(dataShape, obstype, telescop);

      createPartnerColumn(origMSTD, "CORRECTED_DATA", "oldCORRECTED_DATA", 3, tileShape);
      createPartnerColumn(origMSTD, "DATA", "oldDATA", 3, tileShape);
      createPartnerColumn(origMSTD, "FLOAT_DATA", "oldFLOAT_DATA", 3, tileShape);
      createPartnerColumn(origMSTD, "LAG_DATA", "oldLAG_DATA", 3, tileShape);
      createPartnerColumn(origMSTD, "MODEL_DATA", "oldMODEL_DATA", 3, tileShape);
      createPartnerColumn(origMSTD, "SIGMA_SPECTRUM", "oldSIGMA_SPECTRUM", 3, tileShape);
      createPartnerColumn(origMSTD, "WEIGHT_SPECTRUM", "oldWEIGHT_SPECTRUM", 3, tileShape);
      createPartnerColumn(origMSTD, "FLAG", "oldFLAG", 3, tileShape);

      createPartnerColumn(origMSTD, "FLAG_CATEGORY", "oldFLAG_CATEGORY", 4,
			  IPosition(4,tileShape(0),tileShape(1),1, tileShape(2)));
    }

    MSMainColumns mainCols(ms_p);

    // columns which depend on the number of frequency channels and may need to be regridded:
    // DATA, FLOAT_DATA, CORRECTED_DATA, MODEL_DATA, LAG_DATA, SIGMA_SPECTRUM,
    // WEIGHT_SPECTRUM, FLAG, and FLAG_CATEGORY    
    ArrayColumn<Complex> CORRECTED_DATACol =  mainCols.correctedData();
    ArrayColumn<Complex>* oldCORRECTED_DATAColP  = 0;
    ArrayColumn<Complex>  DATACol =  mainCols.data();
    ArrayColumn<Complex>* oldDATAColP = 0;
    ArrayColumn<Float> FLOAT_DATACol =  mainCols.floatData();
    ArrayColumn<Float>* oldFLOAT_DATAColP = 0;
    ArrayColumn<Complex> LAG_DATACol =  mainCols.lagData();
    ArrayColumn<Complex>* oldLAG_DATAColP = 0;
    ArrayColumn<Complex> MODEL_DATACol =  mainCols.modelData();
    ArrayColumn<Complex>* oldMODEL_DATAColP = 0;
    ArrayColumn<Float> SIGMA_SPECTRUMCol =  mainCols.sigmaSpectrum();
    ArrayColumn<Float>* oldSIGMA_SPECTRUMColP = 0;
    ArrayColumn<Float> WEIGHT_SPECTRUMCol =  mainCols.weightSpectrum();
    ArrayColumn<Float>* oldWEIGHT_SPECTRUMColP = 0;
    ArrayColumn<Bool> FLAGCol =  mainCols.flag();
    ArrayColumn<Bool>* oldFLAGColP = 0;
    ArrayColumn<Bool> FLAG_CATEGORYCol =  mainCols.flagCategory();
    ArrayColumn<Bool>* oldFLAG_CATEGORYColP = 0;

    // WEIGHT and SIGMA will possibly need to be modified (these are also arrays but only for corr. product)
    ArrayColumn<Float> SIGMACol = mainCols.sigma();
    ArrayColumn<Float> WEIGHTCol = mainCols.weight();

    if(needRegridding){

      if(doHanningSmooth){
	os << LogIO::NORMAL << "The following columns will be Hanning-smoothed before regridding: " <<  LogIO::POST;
	if(!DATACol.isNull()){
	  os << LogIO::NORMAL << " DATA ";
	}
	if(!CORRECTED_DATACol.isNull()){
	  os << LogIO::NORMAL << " CORRECTED_DATA " <<  LogIO::POST;
	}
	if(!LAG_DATACol.isNull()){
	  os << LogIO::NORMAL << " LAG_DATA ";
	}
	if(!FLOAT_DATACol.isNull()){
	  os << LogIO::NORMAL << " FLOAT_DATA ";
	}
	os << LogIO::POST;
      }

      // (create column objects for all "partners" of the array columns to be modified)
      if(!CORRECTED_DATACol.isNull()){
	oldCORRECTED_DATAColP = new ArrayColumn<Complex>(ms_p, "oldCORRECTED_DATA");
      }
      if(!DATACol.isNull()){
	oldDATAColP = new ArrayColumn<Complex>(ms_p, "oldDATA");
      }
      if(!FLOAT_DATACol.isNull()){
	oldFLOAT_DATAColP = new ArrayColumn<Float>(ms_p, "oldFLOAT_DATA");
      }
      if(!LAG_DATACol.isNull()){
	oldLAG_DATAColP = new ArrayColumn<Complex>(ms_p, "oldLAG_DATA");
      }
      if(!MODEL_DATACol.isNull()){
	oldMODEL_DATAColP = new ArrayColumn<Complex>(ms_p, "oldMODEL_DATA");
      }
      if(!SIGMA_SPECTRUMCol.isNull()){
	oldSIGMA_SPECTRUMColP = new ArrayColumn<Float>(ms_p, "oldSIGMA_SPECTRUM");
      }
      if(!WEIGHT_SPECTRUMCol.isNull()){
	oldWEIGHT_SPECTRUMColP = new ArrayColumn<Float>(ms_p, "oldWEIGHT_SPECTRUM");
      }
      if(!FLAGCol.isNull()){
	oldFLAGColP = new ArrayColumn<Bool>(ms_p, "oldFLAG"); 
      }
      if(!FLAG_CATEGORYCol.isNull()){
	oldFLAG_CATEGORYColP = new ArrayColumn<Bool>(ms_p, "oldFLAG_CATEGORY");
      }
    } // end if needRegridding

    // administrational columns needed from the main table
    ScalarColumn<Int> fieldIdCol = mainCols.fieldId();
    ScalarColumn<Int> DDIdCol = mainCols.dataDescId();
    ScalarMeasColumn<MEpoch> mainTimeMeasCol = mainCols.timeMeas();
   
    // columns needed from subtables
    MSDataDescription ddtable=ms_p.dataDescription();
    MSDataDescColumns DDCols(ddtable);
    ScalarColumn<Int> SPWIdCol = DDCols.spectralWindowId(); 
    MSFieldColumns fldCols(ms_p.field());

    // Loop 3: Apply to MAIN table rows
    
    //    cout << "Modifying main table ..." << endl;

    uInt nMainTabRows = ms_p.nrow();

    // create time-sorted index for main table access
    Vector<uInt> sortedI(nMainTabRows);

    Vector<Double> mainTimesV = mainCols.time().getColumn();
    GenSortIndirect<Double>::sort(sortedI,mainTimesV);

    // prepare progress meter
    Float progress = 0.2;
    Float progressStep = 0.2;
    if(ms_p.nrow()>100000){
      progress = 0.1;
      progressStep = 0.1;
    }

    // prepare some regridding prerequisites
    FFTServer<Float, Complex> fFFTServer; // for fftshift, if needed
 
    // start loop over main table
    for(uInt mainTabRowI=0; mainTabRowI<nMainTabRows; mainTabRowI++){
      
      uInt mainTabRow = sortedI(mainTabRowI); // i.e. mainTabRow is sorted in time

      //if(mainTabRow!=mainTabRowI){
      //  cout << "processing row " << mainTabRow << ", index " << mainTabRowI << endl;
      //}

      // For each MAIN table row, the FIELD_ID cell and the DATA_DESC_ID cell are read 
      Int theFieldId = fieldIdCol(mainTabRow);
      Int theDataDescId = DDIdCol(mainTabRow);
      // and the SPW_ID extracted from the corresponding row in the DATA_DESCRIPTION table.
      Int theSPWId = SPWIdCol(theDataDescId);

      MEpoch theObsTime = mainTimeMeasCol(mainTabRow);
      
      //  The pair (theFieldId, theSPWId) is looked up in the "done table". 
      Int iDone = -1;
      for (uInt i=0; i<oldSpwId.size(); i++){
	if(oldSpwId[i]==theSPWId && (oldFieldId[i]==theFieldId || phaseCenterFieldId>=-1)){
	  // if common phase center is given, treat all fields the same
	  iDone = i;
	  break;
	}
      }
      if(iDone<0){ // should not occur
	os << LogIO::SEVERE << "Internal error: Did not find regrid parameters for field ==" 
	   << theFieldId << " spw ==" <<  theSPWId << LogIO::POST;
	return 0;
      }
      
      
      if (DDIdCol(mainTabRow)!=newDataDescId[iDone]){
	// If the data description actually changed, then DATA_DESC_ID 
	//	of this main table row is set to the new value given in the "done" table
	DDIdCol.put(mainTabRow, newDataDescId[iDone]);
	
      }
      
      //Furthermore, if regrid[iDone] is true, the visibilities and all 
      // channel-number-dependent arrays need to be regridded.
      if(regrid[iDone]){

	Bool doExtrapolate = false;

	// regrid the complex columns
	Array<Complex> yout;
	Array<Bool> youtFlags;
	Bool youtFlagsWritten(false);
	Array<Complex> yin;
	Array<Bool> yinFlags((*oldFLAGColP)(mainTabRow));
	Array<Bool> yinFlagsUnsmoothed;
	Array<Complex> yinIntermediate;
	Array<Bool> yinFlagsIntermediate;
	if(doHanningSmooth){
	  yinFlagsUnsmoothed.assign(yinFlags);
	}

	Vector<Double> xindd(xold[iDone].size());
	Double theShift = 0.;

	if(transform[iDone]){

	  // create frequency machine for this time stamp
	  MDirection fldDir = fldCols.phaseDirMeas(theFieldId, mainTimesV(mainTabRow));


	  MFrequency::Ref fromFrame = MFrequency::Ref(fromFrameTypeV[iDone], MeasFrame(fldDir, mObsPosV[iDone], theObsTime));
	  Unit unit(String("Hz"));
	  MFrequency::Convert freqTrans2(unit, fromFrame, outFrameV[iDone]);

	  MDoppler radVelCorr; // no correction
	  Bool radVelSignificant = false;

	  // prepare correction for radial velocity if requested and possible
	  if(doRadVelCorr && fldCols.needInterTime(theFieldId)){
	    MRadialVelocity mRV = fldCols.radVelMeas(theFieldId, mainTimesV(mainTabRow));
	    MRadialVelocity::Ref mRVR = mRV.getRef();
	    if(mRVR.getType() == MRadialVelocity::GEO){
	      Quantity mrv = mRV.get("m/s");
	      Quantity offsetMrv = outRadVelV[iDone].get("m/s"); // the radvel by which the out SPW def was shifted 
	      radVelCorr = MDoppler(mrv-(2.*offsetMrv)); 
	      if(fabs(mrv.getValue())>1E-6){
		radVelSignificant = true;
	      }
	    }
	    else{
	      os << LogIO::SEVERE << "Cannot perform radial velocity correction with ephemerides of type "  
		 << MRadialVelocity::showType(mRVR.getType()) << ".\nType needs to be GEO." << LogIO::POST;
	      return 0;
	    }
	  } 
	
	  // prepare fftshift if necessary
	  if(method[iDone]==(Int)useFFTShift || method[iDone]==(Int)useLinIntThenFFTShift){
	    uInt centerChan = xold[iDone].size()/2;
	    Vector<Double> newFreq(1);
	    newFreq[0] = freqTrans2(xold[iDone][centerChan]).get(unit).getValue();
	    if(radVelSignificant){
	      newFreq = radVelCorr.shiftFrequency(newFreq);
	    }

	    theShift = newFreq[0] - xin[iDone][centerChan];

	    //cout << "the shift " << theShift << endl;
	    for(uInt i=0; i<xin[iDone].size(); i++){ // cannot use assign due to different data type
	      xindd[i] = xin[iDone][i];
	    }
	  }
	  else{
	    // transform from this timestamp to the one of the output SPW
	    for(uInt i=0; i<xindd.size(); i++){
	      xindd[i] = freqTrans2(xold[iDone][i]).get(unit).getValue();
	    }
	    if(radVelSignificant){
	      xindd = radVelCorr.shiftFrequency(xindd);
	    }
	    // 	  if(mainTabRow==0){ // debug output
	    // 	    Int i = 25;
	    // 	    cout << "i " << i << " xin " << setprecision(9) << xin[iDone][i] << " xindd " << setprecision(9) << xindd[i] 
	    // 		 << " xout " << setprecision(9) << xout[iDone][i] << endl;
	    // 	    cout << "i " << i << " vradxin " << setprecision(9) << vrad(xin[iDone][i], regridVeloRestfrq) 
	    // 		 << " vradxindd " << setprecision(9) << vrad(xindd[i], regridVeloRestfrq)  
	    // 		 << " xout " << setprecision(9) << vrad(xout[iDone][i], regridVeloRestfrq) << endl;
	    // 	  }
	  }
	}
	else{ // no additional transformation of input grid
	  for(uInt i=0; i<xin[iDone].size(); i++){ // cannot use assign due to different data type
	    xindd[i] = xin[iDone][i];
	  }
	}


	Double relShift = 0.;
	InterpolateArray1D<Double,Complex>::InterpolationMethod  methodC = InterpolateArray1D<Double,Complex>::linear; // the default
	InterpolateArray1D<Double,Float>::InterpolationMethod  methodF = InterpolateArray1D<Double,Float>::linear;

	if(fabs(theShift)>0. && 
	   (method[iDone]==(Int)useFFTShift || method[iDone]==(Int)useLinIntThenFFTShift)
	   ){
	  Int endChan = xout[iDone].size()-1;
	  if(endChan<=0){
	    os << LogIO::SEVERE << "Internal error: Cannot regrid a single-channel SPW using FFTshift" << LogIO::POST;
	    return 0;
	  }
	  Double chanWidth = xout[iDone][1] - xout[iDone][0];
	  relShift = -theShift/(xout[iDone][endChan] - xout[iDone][0] + chanWidth);
	  //cout << "the relshift " << relShift << endl;
	}
	else{
	  methodC = (InterpolateArray1D<Double,Complex>::InterpolationMethod) method[iDone];
	  methodF = (InterpolateArray1D<Double,Float>::InterpolationMethod) method[iDone];
	}

	if(!CORRECTED_DATACol.isNull()){
	  yin.assign((*oldCORRECTED_DATAColP)(mainTabRow));

	  // hanning smooth if requested
	  if(doHanningSmooth){

	    // copy yin to yinUnsmoothed 
	    Array<Complex> yinUnsmoothed;
	    yinUnsmoothed.assign(yin);

	    Smooth<Complex>::hanning(yin, // the output
				     yinFlags, // the output flags
				     yinUnsmoothed, // the input
				     yinFlagsUnsmoothed, // the input flags
				     false);  // for flagging: good is not true
	  }
	  
	  if(method[iDone]==(Int)useLinIntThenFFTShift){
	    // first interpolate to equidistant grid at initial timestamp
	    InterpolateArray1D<Double,Complex>::interpolate(yinIntermediate, // the new visibilities
							    yinFlagsIntermediate, // the new flags
							    xout[iDone], // the new channel centers (for the output SPW timestamp)
							    xindd, // the old channel centers 
							    yin, // the old visibilities 
							    yinFlags,// the old flags
							    InterpolateArray1D<Double,Complex>::linear, // the interpol method
							    false, // for flagging: good is not true
							    doExtrapolate // do not extrapolate
							    );	    
	    // shift from this timestamp to the output SPW timestamp
	    fFFTServer.fftshift(yout, youtFlags, yinIntermediate, yinFlagsIntermediate, 
				1, // axis 1 of the array is the polarisation axis 
				relShift, 
				false, // for flagging: good is not true 
				false);

	  }
	  else if(method[iDone]==(Int)useFFTShift){
	    // shift from this timestamp to the output SPW timestamp
	    fFFTServer.fftshift(yout, youtFlags, yin, yinFlags, 
				1, // axis 1 of the array is the polarisation axis 
				relShift, 
				false, // for flagging: good is not true 
				false);
	  }
	  else{
	    InterpolateArray1D<Double,Complex>::interpolate(yout, // the new visibilities
							    youtFlags, // the new flags
							    xout[iDone], // the new channel centers
							    xindd, // the old channel centers
							    yin, // the old visibilities 
							    yinFlags,// the old flags
							    methodC, // the interpol method
							    false, // for flagging: good is not true
							    doExtrapolate // do not extrapolate
							    );
	  }

	  CORRECTED_DATACol.put(mainTabRow, yout);
	  if(!youtFlagsWritten){ 
	    FLAGCol.put(mainTabRow, youtFlags);
	    youtFlagsWritten = true;
	  }
	}
	if(!DATACol.isNull()){
	  yin.assign((*oldDATAColP)(mainTabRow));
	  if(doHanningSmooth){
	    Array<Complex> yinUnsmoothed;
	    yinUnsmoothed.assign(yin);

	    Smooth<Complex>::hanning(yin, yinFlags, yinUnsmoothed, yinFlagsUnsmoothed, false);  
	  }

	  if(method[iDone]==(Int)useLinIntThenFFTShift){
	    InterpolateArray1D<Double,Complex>::interpolate(yinIntermediate, yinFlagsIntermediate, xout[iDone], 
							    xindd, yin, yinFlags,
							    InterpolateArray1D<Double,Complex>::linear, 
							    false, doExtrapolate);	    
	    fFFTServer.fftshift(yout, youtFlags, yinIntermediate, yinFlagsIntermediate, 
				1, relShift, false, false);

	  }
	  else if(method[iDone]==(Int)useFFTShift){
	    fFFTServer.fftshift(yout, youtFlags, yin, yinFlags, 
				1, relShift, false, false);
	  }
	  else{
	    InterpolateArray1D<Double,Complex>::interpolate(yout, youtFlags, xout[iDone], 
							    xindd, yin, yinFlags,
							    methodC, false, doExtrapolate);
	  }

	  DATACol.put(mainTabRow, yout);
	  if(!youtFlagsWritten){ 
	    FLAGCol.put(mainTabRow, youtFlags);
	    youtFlagsWritten = true;
	  }	  
	}
	if(!LAG_DATACol.isNull()){
	  yin.assign((*oldLAG_DATAColP)(mainTabRow));
	  if(doHanningSmooth){ 
	    Array<Complex> yinUnsmoothed;
	    yinUnsmoothed.assign(yin);

	    Smooth<Complex>::hanning(yin, yinFlags, yinUnsmoothed, yinFlagsUnsmoothed, false);  
	  }
	  if(method[iDone]==(Int)useLinIntThenFFTShift){
	    InterpolateArray1D<Double,Complex>::interpolate(yinIntermediate, yinFlagsIntermediate, xout[iDone], 
							    xindd, yin, yinFlags,
							    InterpolateArray1D<Double,Complex>::linear, 
							    false, doExtrapolate);	    
	    fFFTServer.fftshift(yout, youtFlags, yinIntermediate, yinFlagsIntermediate, 
				1, relShift, false, false);

	  }
	  else if(method[iDone]==(Int)useFFTShift){
	    fFFTServer.fftshift(yout, youtFlags, yin, yinFlags, 
				1, relShift, false, false);
	  }
	  else{
	    InterpolateArray1D<Double,Complex>::interpolate(yout, youtFlags, xout[iDone], 
							    xindd, yin, yinFlags,
							    methodC, false, doExtrapolate);
	  }

	  LAG_DATACol.put(mainTabRow, yout);
	}
	if(!MODEL_DATACol.isNull()){
	  yin.assign((*oldMODEL_DATAColP)(mainTabRow));

	  if(method[iDone]==(Int)useLinIntThenFFTShift){
	    InterpolateArray1D<Double,Complex>::interpolate(yinIntermediate, yinFlagsIntermediate, xout[iDone], 
							    xindd, yin, yinFlags,
							    InterpolateArray1D<Double,Complex>::linear, 
							    false, doExtrapolate);	    
	    fFFTServer.fftshift(yout, youtFlags, yinIntermediate, yinFlagsIntermediate, 
				1, relShift, false, false);

	  }
	  else if(method[iDone]==(Int)useFFTShift){
	    fFFTServer.fftshift(yout, youtFlags, yin, yinFlags, 
				1, relShift, false, false);
	  }
	  else{
	    InterpolateArray1D<Double,Complex>::interpolate(yout, youtFlags, xout[iDone], 
							    xindd, yin, yinFlags,
							    methodC, false, doExtrapolate);
	  }

	  MODEL_DATACol.put(mainTabRow, yout);
	  if(!youtFlagsWritten){ 
	    FLAGCol.put(mainTabRow, youtFlags);
	    youtFlagsWritten = true;
	  }
	}
	
	// regrid the Float columns
	Array<Float> yinf;
	Array<Float> youtf;
	Array<Float> fYinIntermediate;
	if(!FLOAT_DATACol.isNull()){
	  yinf.assign((*oldFLOAT_DATAColP)(mainTabRow));
	  if(doHanningSmooth){
	    Array<Float> yinfUnsmoothed;
	    yinfUnsmoothed.assign(yinf);

	    Smooth<Float>::hanning(yinf, yinFlags, yinfUnsmoothed, yinFlagsUnsmoothed, false);  
	  }

	  if(method[iDone]==(Int)useLinIntThenFFTShift){
	    InterpolateArray1D<Double,Float>::interpolate(fYinIntermediate, yinFlagsIntermediate, xout[iDone], 
							  xindd, yinf, yinFlags,
							  InterpolateArray1D<Double,Float>::linear, 
							  false, doExtrapolate);	    
	    fFFTServer.fftshift(youtf, youtFlags, fYinIntermediate, yinFlagsIntermediate, 
				1, relShift, false);

	  }
	  else if(method[iDone]==(Int)useFFTShift){
	    fFFTServer.fftshift(youtf, youtFlags, yinf, yinFlags, 
				1, relShift, false);
	  }
	  else{
	    InterpolateArray1D<Double, Float>::interpolate(youtf, youtFlags, xout[iDone], xindd, 
							   yinf, yinFlags, methodF, false, doExtrapolate);
	  }

	  FLOAT_DATACol.put(mainTabRow, youtf);
	  if(!youtFlagsWritten){ 
	    FLAGCol.put(mainTabRow, youtFlags);
	    youtFlagsWritten = true;
	  }
	}

	if(!SIGMA_SPECTRUMCol.isNull()){
	  yinf.assign((*oldSIGMA_SPECTRUMColP)(mainTabRow));
	  InterpolateArray1D<Double, Float>::interpolate(youtf, youtFlags, xout[iDone], xindd, 
							 yinf, yinFlags, methodF, false, doExtrapolate);
	  SIGMA_SPECTRUMCol.put(mainTabRow, youtf * sigmaScaleV[iDone]); //  an approximation
	}
	if(!WEIGHT_SPECTRUMCol.isNull() && oldWEIGHT_SPECTRUMColP->isDefined(mainTabRow)){ // required column, but can be empty
	  yinf.assign((*oldWEIGHT_SPECTRUMColP)(mainTabRow));
	  InterpolateArray1D<Double, Float>::interpolate(youtf, youtFlags, xout[iDone],
                                                         xindd, yinf, yinFlags,
                                                         methodF, false, doExtrapolate);
	  WEIGHT_SPECTRUMCol.put(mainTabRow, youtf * weightScaleV[iDone]); // an approximation
	}
	

	SIGMACol.put(mainTabRow, SIGMACol(mainTabRow)*sigmaScaleV[iDone]);
	WEIGHTCol.put(mainTabRow, WEIGHTCol(mainTabRow)*weightScaleV[iDone]);


	// deal with FLAG_CATEGORY
	// note: FLAG_CATEGORY is a required column, but it can be undefined (empty)
	
	if(FLAG_CATEGORYCol.isDefined(mainTabRow)){
	  Array<Bool> flagCat((*oldFLAG_CATEGORYColP)(mainTabRow));  
	  IPosition flagCatShape = (*oldFLAG_CATEGORYColP).shape(mainTabRow);
	  Int nCorrelations = flagCatShape(0); // get the dimension of the first axis
	  Int nChannels = flagCatShape(1); // get the dimension of the second axis
	  Int nCat = flagCatShape(2); // the dimension of the third axis ==
                                      // number of categories
	  Int nOutChannels = xout[iDone].size();
	  
	  Vector<Float> dummyYin(nChannels);
	  Vector<Float> dummyYout(nOutChannels);
	  Array<Bool> flagCatOut(IPosition(3, nCorrelations, nOutChannels, nCat)); 
	  
	  for(Int i=0; i<nCat; i++){
	    IPosition start(0,0,i), length (nCorrelations,nChannels,i), stride (1,1,0);
	    Slicer slicer (start, length, stride, Slicer::endIsLast);
	    yinFlags.assign(flagCat(slicer));
	    InterpolateArray1D<Double, Float>::interpolate(dummyYout, youtFlags,
                                                           xout[iDone], xindd, 
							   dummyYin, yinFlags,
                                                           methodF, false, false);
	    // write the slice to the array flagCatOut
	    for(Int j=0; j<nCorrelations; j++){
	      for(Int k=0; k<nOutChannels; k++){
		flagCatOut(IPosition(3, j, k, i)) = youtFlags(IPosition(2,j,k));
	      }
	    }
	  }
	  
	  FLAG_CATEGORYCol.put(mainTabRow, flagCatOut);
	  
	}
	
	msModified = true;
	
      } // end if regridding necessary

      if(mainTabRow>nMainTabRows*progress){
	cout << "regridSpw progress: " << progress*100 << "% processed ... " << endl;
	progress += progressStep;
      }
      
    } // end loop over main table rows
    cout << "regridSpw progress: 100% processed." << endl;

    if(msModified){
      if(needRegridding){

	// remove the "partner" columns
	if(!CORRECTED_DATACol.isNull()){
	  ms_p.removeColumn("oldCORRECTED_DATA");
	}
	if(!DATACol.isNull()){
	  ms_p.removeColumn("oldDATA");
	}
	if(!FLOAT_DATACol.isNull()){
	  ms_p.removeColumn("oldFLOAT_DATA");
	}
	if(!LAG_DATACol.isNull()){
	  ms_p.removeColumn("oldLAG_DATA");
	}
	if(!MODEL_DATACol.isNull()){
	  ms_p.removeColumn("oldMODEL_DATA");
	}
	if(!SIGMA_SPECTRUMCol.isNull()){
	  ms_p.removeColumn("oldSIGMA_SPECTRUM");
	}
	if(!WEIGHT_SPECTRUMCol.isNull()){
	  ms_p.removeColumn("oldWEIGHT_SPECTRUM");
	}
	if(!FLAGCol.isNull()){
	  ms_p.removeColumn("oldFLAG");
	} 
	if(!FLAG_CATEGORYCol.isNull()){
	  ms_p.removeColumn("oldFLAG_CATEGORY");
	}
      }
      
      rval = 1; // successful modification
    }
    return rval;

  }


  Bool SubMS::createPartnerColumn(TableDesc& modMSTD,
				  const String& oldName, 
				  const String& newName,
				  const Int& hypercolumnDim,
				  const IPosition& tileShape
				  ){
    Bool rval = false;
    if(modMSTD.isColumn(oldName)){
      // get the old column desc
      ColumnDesc myColDesc(modMSTD.columnDesc(oldName));
      // move the column away
      ms_p.renameColumn(newName, oldName);
      // rename the hypercolumn
      String hcName(myColDesc.dataManagerGroup());
      String oldHcName = hcName;
      String newHcName = hcName + "B";
      if(!oldHcName.empty() && ms_p.actualTableDesc().isHypercolumn(oldHcName)){
	ms_p.renameHypercolumn(newHcName, oldHcName);
      }
      // rename the datamanager
      DataManager* myDM = ms_p.findDataManager(oldHcName);
      ((TiledStMan*) myDM)->setDataManagerName(newHcName);
      // create new hypercolumn and a new column with new data manager
      TiledShapeStMan*  tiledStMan = new TiledShapeStMan(oldHcName, tileShape);
      ms_p.addColumn(myColDesc, *tiledStMan);
      modMSTD.defineHypercolumn(oldHcName, hypercolumnDim, stringToVector(oldName));

      rval = true;
    }
    return rval;
  }


  Bool SubMS::regridChanBounds(Vector<Double>& newChanLoBound, 
			       Vector<Double>& newChanHiBound,
			       const Double regridCenterC,
			       const Double regridBandwidth, 
			       const Double regridChanWidthC, 
			       const Double regridVeloRestfrq, 
			       const String regridQuant,
			       const Vector<Double>& transNewXinC, 
			       const Vector<Double>& transCHAN_WIDTHC,
			       String& message,
			       const Bool centerIsStartC,
			       const Bool startIsEndC,
			       const Int nchanC,
			       const Int width,
			       const Int startC
			       ){
    ostringstream oss;

//     cout << "regridCenterC " <<  regridCenterC << " regridBandwidth " << regridBandwidth 
//  	 << " regridChanWidthC " << regridChanWidthC << endl;
//     cout << " nchanC " << nchanC << " width " << width << " startC " << startC << endl;
//     cout << " regridQuant " << regridQuant << " centerIsStartC " <<  centerIsStartC << " startIsEndC " << startIsEndC << endl;
    

    Vector<Double> transNewXin(transNewXinC);
    Vector<Double> transCHAN_WIDTH(transCHAN_WIDTHC);
    Bool centerIsStart = centerIsStartC;
    Bool startIsEnd = startIsEndC;
    Double regridChanWidth = regridChanWidthC;
    Double regridCenter = regridCenterC;
    Int nchan = nchanC;
    Int start = startC;

    Int oldNUM_CHAN = transNewXin.size();


    // detect spectral windows defined with descending frequency
    Bool isDescending=false;
    for(uInt i=1; i<transNewXin.size(); i++){
      if(transNewXin(i)<transNewXin(i-1)){
	isDescending = true;
      }
      else if(isDescending){ // i.e. descending was detected but now we encounter ascending
	oss << "Channel frequencies are neither in ascending nor in descending order. Cannot process.";
	message = oss.str();
	return false;  
      }	
    }

    if(isDescending){ // need to reverse the order for processing and later reverse the result
      //cout << "SPW has descending order ..." << endl;
      uInt n = transNewXin.size();
      Vector<Double> tempF, tempW;
      tempF.assign(transNewXin);
      tempW.assign(transCHAN_WIDTH);
      for(uInt i=0; i<n; i++){
	transNewXin(i) = tempF(n-1-i);
	transCHAN_WIDTH(i) = tempW(n-1-i);
	//cout << "i f w " << i << " " << transNewXin(i) << " " << transCHAN_WIDTH(i) << endl;
      }
      // also need to adjust the start values 
      if(startC>=0){
	start = n-1-startC;
	if(centerIsStartC){
	  startIsEnd = !startIsEnd;
	}
      }
      //}
    }

    // verify regridCenter, regridBandwidth, and regridChanWidth 
    // Note: these are in the units corresponding to regridQuant!
    
    if(regridQuant=="chan"){ ////////////////////////
      // channel numbers ...
      Int regridCenterChan = -1;
      Int regridBandwidthChan = -1;
      Int regridChanWidthChan = -1;

      if(regridCenter<-1E30){ // not set
	// find channel center closest to center of bandwidth
	lDouble BWCenterF = (transNewXin[0]+transNewXin[oldNUM_CHAN-1])/2.;
	for(Int i=0; i<oldNUM_CHAN; i++){
	  if(transNewXin[i] >= BWCenterF){
	    regridCenterChan = i;
	    break;
	  }
	}
	centerIsStart = false;
      }
      else if(0. <= regridCenter && regridCenter < Double(oldNUM_CHAN)){ // valid input
	regridCenterChan = (Int) floor(regridCenter);  
      }
      else { // invalid
	if(centerIsStart){
	  oss << "SPW start ";
	}
	else{
	  oss << "SPW center ";
	}
	oss << regridCenter << " outside valid range which is "
	    << 0 << " - " << oldNUM_CHAN-1 <<".";
	message = oss.str();
	return false;  
      }  
      
      if(regridBandwidth<=0.|| nchan>0){ // not set or nchan set
	if(nchan>0){
	  regridBandwidthChan = nchan;
	}
	else{
	  regridBandwidthChan = oldNUM_CHAN;
	}
      }
      else{
	regridBandwidthChan = (Int) floor(regridBandwidth);
      }

      if(centerIsStart){
	if(startIsEnd){
	  regridCenterChan = regridCenterChan - regridBandwidthChan/2;
	}
	else{
	  regridCenterChan = regridCenterChan + regridBandwidthChan/2;
	}
	centerIsStart = false;
      }

      if(regridCenterChan-regridBandwidthChan/2 < 0) { // center too close to lower edge
	regridBandwidthChan = 2 * regridCenterChan + 1;
	oss << " *** Requested output SPW width too large." << endl;
      }
      if( oldNUM_CHAN < regridCenterChan+regridBandwidthChan/2){  // center too close to upper edge
	regridBandwidthChan = 2*(oldNUM_CHAN - regridCenterChan);
	oss << " *** Requested output SPW width too large." << endl;
      } 
      
      if(regridChanWidth < 1.){
	regridChanWidthChan = 1;
      }
      else if(regridChanWidth > Double(regridBandwidthChan)){
	regridChanWidthChan = regridBandwidthChan; // i.e. SPW = a single channel
	oss << " *** Requested output channel width too large. Adjusted to maximum possible value." << endl;
      }
      else { // valid input
	regridChanWidthChan = (Int) floor(regridChanWidth);
	if(nchan>0){
	  regridBandwidthChan = nchan * regridChanWidthChan;
	}
      }
      
      if(regridBandwidthChan != floor(regridBandwidth)){
	oss << " *** Output SPW width set to " << regridBandwidthChan << " original channels" << endl;
	oss << "     in an attempt to keep center of output SPW close to center of requested SPW." << endl;
      } 
      
      // calculate newChanLoBound and newChanHiBound from regridCenterChan, regridBandwidthChan, and regridChanWidthChan
      Int bwLowerEndChan = regridCenterChan - regridBandwidthChan/2;
      Int bwUpperEndChan = bwLowerEndChan + regridBandwidthChan - 1;
      Int numNewChanDown = 0;
      Int numNewChanUp = 0;
      
      if(regridChanWidthChan == regridBandwidthChan){ // only one new channel
	newChanLoBound.resize(1);
	newChanHiBound.resize(1);
	newChanLoBound[0] = transNewXin[bwLowerEndChan]-transCHAN_WIDTH[bwLowerEndChan]/2.;
	newChanHiBound[0] = transNewXin[bwUpperEndChan]+transCHAN_WIDTH[bwUpperEndChan]/2.;
	numNewChanUp = 1;
      }
      else { // have more than one new channel
	// Need to accomodate the possibility that the original channels are
	// not contiguous!

        // the numbers of the Channels from which the lower bounds will be taken for the new channels 
	vector<Int> loNCBup;
	// starting from the central channel going up
	vector<Int> hiNCBup; // the numbers of the Channels from which the high
                             // bounds will be taken for the new channels
	// starting from the central channel going up
	vector<Int> loNCBdown; // the numbers of the Channels from which the
                               // lower bounds will be taken for the new
                               // channels
	// starting from the central channel going down
	vector<Int> hiNCBdown; // the numbers of the Channels from which the
                               // high bounds will be taken for the new
                               // channels
	// starting from the central channel going down
	//    Want to keep the center of the center channel at the center of
	//    the new center channel if the bandwidth is an odd multiple of the
        //    new channel width 
	//    otherwise the center channel is the lower edge of the new center channel
	Int startChan;
	lDouble tnumChan = regridBandwidthChan/regridChanWidthChan;
	if((Int)tnumChan % 2 != 0 ){
          // odd multiple 
	  startChan = regridCenterChan-regridChanWidthChan/2;
	}
	else{
	  startChan = regridCenterChan;
	}
	for(Int i=startChan; i<=bwUpperEndChan; i+=regridChanWidthChan){ // upper half
	  loNCBup.push_back(i);
	  if(i+regridChanWidthChan-1<=bwUpperEndChan){
            // can go one more normal step up
	    hiNCBup.push_back(i+regridChanWidthChan-1);
	  }
	  else{
            // create narrower channels at the edges if necessary
	    oss << " *** Last channel at upper edge of new SPW made only " << bwUpperEndChan-i+1 
		<< " original channels wide to fit given total bandwidth." << endl;
	    hiNCBup.push_back(bwUpperEndChan);
	  }
	}

        // lower half
	for(Int i=startChan - 1; i>=bwLowerEndChan; i-=regridChanWidthChan){ 
	  hiNCBdown.push_back(i);
	  if(i-regridChanWidthChan+1>=bwLowerEndChan){
            // can go one more normal step down
	    loNCBdown.push_back(i-regridChanWidthChan+1);
	  }
	  else{
            // create narrower channels at the edges if necessary
	    oss << " *** First channel at lower edge of new SPW made only " << i-bwLowerEndChan+1 
		<< " original channels wide to fit given total bandwidth." << endl;
	    loNCBdown.push_back(bwLowerEndChan);
	  }
	}

        // the number of channels below the central one
	numNewChanDown = loNCBdown.size();

        // the number of channels above and including the central one
	numNewChanUp = loNCBup.size();

	newChanLoBound.resize(numNewChanDown+numNewChanUp);
	newChanHiBound.resize(numNewChanDown+numNewChanUp);
	for(Int i=0; i<numNewChanDown; i++){
	  Int k = numNewChanDown-i-1; // need to assign in reverse
	  newChanLoBound[i] = transNewXin[loNCBdown[k]] -
            transCHAN_WIDTH[loNCBdown[k]]/2.; 
	  newChanHiBound[i] = transNewXin[hiNCBdown[k]] +
            transCHAN_WIDTH[hiNCBdown[k]]/2.;
	}
	for(Int i=0; i<numNewChanUp; i++){
	  newChanLoBound[i+numNewChanDown] = transNewXin[loNCBup[i]] -
            transCHAN_WIDTH[loNCBup[i]]/2.;
	  newChanHiBound[i+numNewChanDown] = transNewXin[hiNCBup[i]] +
            transCHAN_WIDTH[hiNCBup[i]]/2.;
	}
      } // end if 

      oss << " New channels defined based on original channels" << endl
	  << " Central channel contains original channel " <<  regridCenterChan << endl 
	  << " Channel width = " << regridChanWidthChan
          << " original channels" << endl
	  << " Total width of SPW = " <<  regridBandwidthChan << " original channels == " 
	  << numNewChanDown + numNewChanUp << " new channels" << endl;

      uInt nc = newChanLoBound.size();
      oss << " Total width of SPW (in output frame) = " << newChanHiBound[nc-1] - newChanLoBound[0] 
	  << " Hz" << endl;
      oss << " Lower edge = " << newChanLoBound[0] << " Hz,"
	  << " upper edge = " << newChanHiBound[nc-1] << " Hz" << endl;

      if(isDescending){ 
	Vector<Double> tempL, tempU;
	tempL.assign(newChanLoBound);
	tempU.assign(newChanHiBound);
	for(uInt i=0; i<nc; i++){
	  newChanLoBound(i) = tempL(nc-1-i);
	  newChanHiBound(i) = tempU(nc-1-i);
	}
      }

      message = oss.str();

      return true;
    }
    else { // we operate on real numbers /////////////////
      // first transform them to frequencies
      lDouble regridCenterF = -1.; // initialize as "not set"
      lDouble regridBandwidthF = -1.;
      lDouble regridChanWidthF = -1.;

      if(regridQuant=="vrad"){ ///////////////
	// radio velocity ...
	// need restfrq
	if(regridVeloRestfrq<-1E30){ // means "not set"
	  oss << "Parameter \"restfreq\" needs to be set if regrid_quantity==vrad. Cannot proceed with regridSpw ..."; 
	  message = oss.str();
	  return false;
	}
	else if(regridVeloRestfrq < 0. || regridVeloRestfrq > 1E30){
	  oss << "Parameter \"restfreq\" value " << regridVeloRestfrq << " is invalid.";
	  message = oss.str();
	  return false;
	}	  
	lDouble regridCenterVel; 
	if(regridCenter>-C::c){
          // (we deal with invalid values later)
	  if(centerIsStart){
	    Double tcWidth;
	    if(regridChanWidth > 0.){
	      tcWidth = regridChanWidth;
	    }
	    else{
	      tcWidth = vrad(transNewXin[0]-transCHAN_WIDTH[0]/2.,regridVeloRestfrq)
		- vrad(transNewXin[0]+transCHAN_WIDTH[0]/2.,regridVeloRestfrq);
	    }
	    if(startIsEnd){ // start is the center of the last channel (in freq)
	      regridCenter -= tcWidth/2.;
	    }
	    else{ // start is the center of the first channel (in freq)
	      regridCenter += tcWidth/2.;
	    }
	  }

	  regridCenterF = freq_from_vrad(regridCenter,regridVeloRestfrq);

	  regridCenterVel = regridCenter;
	}
	else{ // center was not specified
	  regridCenterF = (transNewXin[0]+transNewXin[oldNUM_CHAN-1])/2.;
	  regridCenterVel = vrad(regridCenterF,regridVeloRestfrq);
	  centerIsStart = false;
	}
	if(nchan>0){
	  if(regridChanWidth > 0.){
	    lDouble chanUpperEdgeF = freq_from_vrad(regridCenterVel - regridChanWidth/2.,
						   regridVeloRestfrq);
	    regridChanWidthF = 2.* (chanUpperEdgeF - regridCenterF); 
	  }
	  else{ // take channel width from first channel
	    regridChanWidthF = transCHAN_WIDTH[0];
	  }
	  regridBandwidthF = nchan*regridChanWidthF;
	  // can convert start to center
	  if(centerIsStart){
	    if(startIsEnd){
	      regridCenterF = regridCenterF - regridBandwidthF/2.;
	    }
	    else{
	      regridCenterF = regridCenterF + regridBandwidthF/2.;
	    }
	    centerIsStart = false;
	  }
	}
	else if(regridBandwidth > 0.){
	  // can convert start to center
	  if(centerIsStart){
	    if(startIsEnd){
	      regridCenterVel = regridCenter + regridBandwidth/2.;
	    }
	    else{
	      regridCenterVel = regridCenter - regridBandwidth/2.;
	    }
	    regridCenterF = freq_from_vrad(regridCenterVel,regridVeloRestfrq);
	    centerIsStart = false;
	  }
	  lDouble bwUpperEndF = freq_from_vrad(regridCenterVel - regridBandwidth/2.,
                                              regridVeloRestfrq);
	  regridBandwidthF = 2.* (bwUpperEndF - regridCenterF); 
	}
	if(regridChanWidth > 0. && regridChanWidthF<0.){
	  lDouble chanUpperEdgeF = freq_from_vrad(regridCenterVel - regridChanWidth/2.,
						  regridVeloRestfrq);
	  regridChanWidthF = 2.* (chanUpperEdgeF - freq_from_vrad(regridCenterVel, regridVeloRestfrq));
	}
      }
      else if(regridQuant=="vopt"){ ///////////
	// optical velocity ...
	// need restfrq
	if(regridVeloRestfrq < -1E30){ // means "not set"
	  oss << "Parameter \"restfreq\" needs to be set if regrid_quantity==vopt. Cannot proceed with regridSpw ...";
	  message = oss.str();
	  return false;
	}
	else if(regridVeloRestfrq <= 0. || regridVeloRestfrq > 1E30){
	  oss << "Parameter \"restfreq\" value " << regridVeloRestfrq
              << " is invalid."; 
	  message = oss.str();
	  return false;
	}
	lDouble regridCenterVel; 
	if(regridCenter > -C::c){
	  if(centerIsStart){
	    Double tcWidth;
	    if(regridChanWidth > 0.){
	      tcWidth = regridChanWidth;
	    }
	    else{
	      tcWidth = vopt(transNewXin[0]-transCHAN_WIDTH[0]/2.,regridVeloRestfrq)
		- vopt(transNewXin[0]+transCHAN_WIDTH[0]/2.,regridVeloRestfrq);
	    }
	    if(startIsEnd){ // start is the center of the last channel (in freq)
	      regridCenter -= tcWidth/2.;
	    }
	    else{ // start is the center of the first channel (in freq)
	      regridCenter += tcWidth/2.;
	    }
	  }
          // (we deal with invalid values later)
	  regridCenterF = freq_from_vopt(regridCenter,regridVeloRestfrq);
	  regridCenterVel = regridCenter;
	}
	else{ // center was not specified
	  regridCenterF = (transNewXin[0]-transCHAN_WIDTH[0]+transNewXin[oldNUM_CHAN-1]+transCHAN_WIDTH[oldNUM_CHAN-1])/2.;
	  regridCenterVel = vopt(regridCenterF,regridVeloRestfrq);
	  centerIsStart = false;
	}
	if(nchan>0){
	  lDouble cw;
	  lDouble divbytwo = 0.5;
	  if(centerIsStart){
	    divbytwo = 1.;
	  }
	  if(regridChanWidth > 0.){
	    cw = regridChanWidth;
	  }
	  else{ // determine channel width from first channel
	    lDouble upEdge = vopt(transNewXin[0]-transCHAN_WIDTH[0],regridVeloRestfrq);
	    lDouble loEdge = vopt(transNewXin[0]+transCHAN_WIDTH[0],regridVeloRestfrq);
	    cw = abs(upEdge-loEdge); 
	  }
	  lDouble bwUpperEndF = 0.;
	  if(centerIsStart && !startIsEnd){ // start is end in velocity
	    bwUpperEndF = freq_from_vopt(regridCenterVel - (lDouble)nchan*cw*divbytwo,
					 regridVeloRestfrq);
	  }
	  else{
	    bwUpperEndF = freq_from_vopt(regridCenterVel + (lDouble)nchan*cw*divbytwo,
					 regridVeloRestfrq);
	  }
	  regridBandwidthF = abs(bwUpperEndF-regridCenterF)/divbytwo; 
	  // can convert start to center
	  if(centerIsStart){
	    if(startIsEnd){
	      regridCenterVel = regridCenterVel + (lDouble)nchan*cw/2.;
	    }
	    else{
	      regridCenterVel = regridCenterVel - (lDouble)nchan*cw/2.;
	    }
	    regridCenterF = freq_from_vopt(regridCenterVel,regridVeloRestfrq);
	    centerIsStart = false;
	  }
	  nchan=0; // indicate that nchan should not be used in the following
	}
	else if(regridBandwidth > 0.){
	  // can convert start to center
	  if(centerIsStart){
	    if(startIsEnd){
	      regridCenterVel = regridCenter + regridBandwidth/2.;
	    }
	    else{
	      regridCenterVel = regridCenter - regridBandwidth/2.;
	    }
	    regridCenterF = freq_from_vopt(regridCenterVel,regridVeloRestfrq);
	    centerIsStart = false;
	  }
	  lDouble bwUpperEndF =  freq_from_vopt(regridCenterVel - regridBandwidth/2.,
                                               regridVeloRestfrq);
	  regridBandwidthF = 2.* (bwUpperEndF- regridCenterF); 
	}
	if(regridChanWidth > 0. && regridChanWidthF<0.){
	  lDouble chanUpperEdgeF = freq_from_vopt(regridCenterVel - regridChanWidth/2.,
                                                 regridVeloRestfrq);
	  regridChanWidthF = 2.* (chanUpperEdgeF - freq_from_vopt(regridCenterVel,regridVeloRestfrq)); 
	}
      } 
      else if(regridQuant=="freq"){ ////////////////////////
	if(width>0){ // width parameter overrides regridChanWidth
	  regridChanWidth = width*transCHAN_WIDTH[0];
	}
	if(start>=0){
	  Int firstChan = start;
	  if(start >= (Int)transNewXin.size()){
	    oss << " *** Parameter start exceeds total number of channels which is "
		<< transNewXin.size() << ". Set to 0." << endl;
	    firstChan = 0;
	    startIsEnd = false;
	  }
	  if(startIsEnd){
	    regridCenter = transNewXin[firstChan]+transCHAN_WIDTH[firstChan]/2.;
	  }
	  else{
	    regridCenter = transNewXin[firstChan]-transCHAN_WIDTH[firstChan]/2.;
	  }
	  centerIsStart = true;
	}
	else{
	  if(centerIsStart){ // start is the center of the first channel
	    Double tcWidth;
	    if(regridChanWidth > 0.){ 
	      tcWidth = regridChanWidth;
	    }
	    else{
	      tcWidth = transCHAN_WIDTH[0];
	    }
	    if(startIsEnd){
	      regridCenter += tcWidth/2.;
	    }
	    else{
	      regridCenter -= tcWidth/2.;
	    }
	  }
	}
	regridCenterF = regridCenter;
	regridBandwidthF = regridBandwidth;
	regridChanWidthF = regridChanWidth;
      }
      else if(regridQuant=="wave"){ ///////////////////////
	// wavelength ...
	lDouble regridCenterWav; 
	if(regridCenter > 0.){
	  if(centerIsStart){
	    Double tcWidth;
	    if(regridChanWidth > 0.){
	      tcWidth = regridChanWidth;
	    }
	    else{
	      tcWidth = lambda(transNewXin[0]-transCHAN_WIDTH[0]/2.)
		- lambda(transNewXin[0]+transCHAN_WIDTH[0]/2.);
	    }
	    if(startIsEnd){ // start is the center of the last channel (in freq)
	      regridCenter -= tcWidth/2.;
	    }
	    else{ // start is the center of the first channel (in freq)
	      regridCenter += tcWidth/2.;
	    }
	  }
	  regridCenterF = freq_from_lambda(regridCenter); 
	  regridCenterWav = regridCenter;
	}
	else{ // center was not specified
	  regridCenterF = (transNewXin[0] + transNewXin[oldNUM_CHAN-1])/2.;
	  regridCenterWav = lambda(regridCenterF);
	  centerIsStart = false;
	}
	if(nchan>0){
	  lDouble cw;
	  lDouble divbytwo = 0.5;
	  if(centerIsStart){
	    divbytwo = 1.;
	  }
	  if(regridChanWidth > 0.){
	    cw = regridChanWidth;
	  }
	  else{ // determine channel width from first channel
	    lDouble upEdge = lambda(transNewXin[0]-transCHAN_WIDTH[0]);
	    lDouble loEdge = lambda(transNewXin[0]+transCHAN_WIDTH[0]);
	    cw = abs(upEdge-loEdge); 
	  }
	  lDouble bwUpperEndF = 0.;
	  if(centerIsStart && !startIsEnd){
	    bwUpperEndF = freq_from_lambda(regridCenterWav - (lDouble)nchan*cw*divbytwo);	    
	  }
	  else{
	    bwUpperEndF = freq_from_lambda(regridCenterWav + (lDouble)nchan*cw*divbytwo);
	  }
	  regridBandwidthF = (bwUpperEndF-regridCenterF)/divbytwo; 
	  // can convert start to center
	  if(centerIsStart){
	    if(startIsEnd){
	      regridCenterWav = regridCenterWav + (lDouble)nchan*cw/2.;
	    }
	    else{
	      regridCenterWav = regridCenterWav - (lDouble)nchan*cw/2.;
	    }
	    regridCenterF = freq_from_lambda(regridCenterWav);
	    centerIsStart = false;
	  }
	  nchan=0; // indicate that nchan should not be used in the following
	}
	else if(regridBandwidth > 0. && regridBandwidth/2. < regridCenterWav){
	  // can convert start to center
	  if(centerIsStart){
	    if(startIsEnd){
	      regridCenterWav = regridCenter + regridBandwidth/2.;
	    }
	    else{
	      regridCenterWav = regridCenter - regridBandwidth/2.;
	    }
	    regridCenterF = freq_from_lambda(regridCenterWav);
	    centerIsStart = false;
	  }
	  lDouble bwUpperEndF =  lambda(regridCenterWav - regridBandwidth/2.);
	  regridBandwidthF = 2.* (bwUpperEndF - regridCenterF); 
	}
	if(regridChanWidth>0. && regridChanWidth/2.< regridCenterWav){
	  lDouble chanUpperEdgeF =  lambda(regridCenterWav - regridChanWidth/2.);
	  regridChanWidthF = 2.* (chanUpperEdgeF - regridCenterF); 
	}
      }
      else{
	oss << "Invalid value " << regridQuant << " for parameter \"mode\".";
	message = oss.str();
	return false;
      }
      // (transformation of regrid parameters to frequencies completed)
      
      // then determine the actually possible parameters
      lDouble theRegridCenterF;
      lDouble theRegridBWF;
      lDouble theCentralChanWidthF;
      
      // for vrad and vopt also need to keep this adjusted value
      lDouble theChanWidthX = -1.;

      if(regridCenterF < 0.){ //  means "not set"
	// keep regrid center as it is in the data
	theRegridCenterF = (transNewXin[0] - transCHAN_WIDTH[0]/2. 
			    + transNewXin[oldNUM_CHAN-1]+transCHAN_WIDTH[oldNUM_CHAN-1]/2.)/2.;
	centerIsStart = false;
      }
      else { // regridCenterF was set
	// keep center in limits
	theRegridCenterF = regridCenterF;
	if( (theRegridCenterF - (transNewXin[oldNUM_CHAN-1]+transCHAN_WIDTH[oldNUM_CHAN-1]/2.)) > 1. ){ // 1 Hz tolerance
	  oss << "*** Requested center of SPW " << theRegridCenterF << " Hz is too large by "
	      << theRegridCenterF - transNewXin[oldNUM_CHAN-1]+transCHAN_WIDTH[oldNUM_CHAN-1]/2. << " Hz\n";
	  theRegridCenterF = transNewXin[oldNUM_CHAN-1]+transCHAN_WIDTH[oldNUM_CHAN-1]/2.;
	  oss << "*** Reset to maximum possible value " <<  theRegridCenterF  << " Hz";
	}
	else if( theRegridCenterF < (transNewXin[0]-transCHAN_WIDTH[0]/2.)  ){ 
	  Double diff = (transNewXin[0]-transCHAN_WIDTH[0]/2.) - theRegridCenterF;
	  // cope with numerical accuracy problems
	  if(diff>1.){
	    oss << "*** Requested center of SPW " << theRegridCenterF << " Hz is smaller than minimum possible value";
	    oss << " by " << diff << " Hz";
	  }  
	  theRegridCenterF = transNewXin[0]-transCHAN_WIDTH[0]/2.;
	  if(diff>1.){
	    oss << "\n*** Reset to minimum possible value " <<  theRegridCenterF  << " Hz";
	  }
	}
      }
      if(regridBandwidthF<=0.|| nchan!=0){ // "not set" or use nchan instead
	// keep bandwidth as is
	theRegridBWF = transNewXin[oldNUM_CHAN-1] - transNewXin[0] 
	  + transCHAN_WIDTH[0]/2. + transCHAN_WIDTH[oldNUM_CHAN-1]/2.;
	if(nchan!=0){ // use nchan parameter if available
	  if(nchan<0){
	    if(regridQuant=="freq" || regridQuant=="vrad"){ // i.e. equidistant in freq
	      // define via width of first channel to avoid numerical problems
	      if(regridChanWidthF <= 0.){ // channel width not set
		theRegridBWF = transCHAN_WIDTH[0]*floor((theRegridBWF+transCHAN_WIDTH[0]*0.01)/transCHAN_WIDTH[0]);
	      }
	      else{
		theRegridBWF = regridChanWidthF*floor((theRegridBWF+regridChanWidthF*0.01)/regridChanWidthF);
	      }
	    }
	  }
	  else if(regridChanWidthF <= 0.){ // channel width not set
	    theRegridBWF = transCHAN_WIDTH[0]*nchan;
	  }
	  else{ 
	    theRegridBWF = regridChanWidthF*nchan;
	  }

	  if(regridCenterF <= 0.|| regridCenter <-C::c ){ // center was not set by user but calculated
	    // need to update
	    theRegridCenterF = transNewXin[0] - transCHAN_WIDTH[0]/2. + theRegridBWF/2.;
	    centerIsStart = false;
	  }
	  else if(nchan<0){ // center but not nchan was set by user
	    // verify that the bandwidth is correct
	    if(centerIsStart){
	      if(startIsEnd){
		theRegridBWF = theRegridCenterF - transNewXin[0] + transCHAN_WIDTH[0]/2.;
	      }
	      else{ // start is start
		theRegridBWF = transNewXin[oldNUM_CHAN-1] + transCHAN_WIDTH[oldNUM_CHAN-1]/2. - theRegridCenterF;
	      }
	      if(regridQuant=="freq" || regridQuant=="vrad"){ // i.e. equidistant in freq
		// define via width of first channel to avoid numerical problems
		if(regridChanWidthF <= 0.){ // channel width not set
		  theRegridBWF = transCHAN_WIDTH[0]*floor((theRegridBWF+transCHAN_WIDTH[0]*0.01)/transCHAN_WIDTH[0]);
		}
		else{
		  theRegridBWF = regridChanWidthF*floor((theRegridBWF+regridChanWidthF*0.01)/regridChanWidthF);
		}
	      }
	    }
	    else{ // center is center
	      theRegridBWF = 2. * min((Double)(theRegridCenterF - transNewXin[0] - transCHAN_WIDTH[0]), 
				      (Double)(transNewXin[oldNUM_CHAN-1] + transCHAN_WIDTH[oldNUM_CHAN-1] 
					       - theRegridCenterF));
	    }
	  }
	}
	// now can convert start to center
	if(centerIsStart){
	  if(startIsEnd){
	    theRegridCenterF = theRegridCenterF - theRegridBWF/2.;
	  }
	  else{
	    theRegridCenterF = theRegridCenterF + theRegridBWF/2.;
	  }
	  centerIsStart = false;
	}
      }
      else { // regridBandwidthF was set
	// determine actually possible bandwidth:
	// width will be truncated to the maximum width possible symmetrically
	// around the value given by "regrid_center"
	theRegridBWF = regridBandwidthF;
	// now can convert start to center
	if(centerIsStart){
	  if(startIsEnd){
	    theRegridCenterF = theRegridCenterF - theRegridBWF/2.;
	  }
	  else{
	    theRegridCenterF = theRegridCenterF + theRegridBWF/2.;
	  }
	  centerIsStart = false;
	}
	{
	  Double rangeTol = 1.; // Hz
	  if((regridQuant=="vopt" || regridQuant=="wave")){ // i.e. if the center is the center w.r.t. wavelength
	    rangeTol = transCHAN_WIDTH[0];
	  }
	  if((theRegridCenterF + theRegridBWF / 2.) -  (transNewXin[oldNUM_CHAN-1] + transCHAN_WIDTH[oldNUM_CHAN-1]/2.) > rangeTol ){
	    oss << " *** Input spectral window exceeds upper end of original window. Adjusting to max. possible value." << endl;	  
	    theRegridBWF = min((Double)fabs(transNewXin[oldNUM_CHAN-1] + transCHAN_WIDTH[oldNUM_CHAN-1]/2. - theRegridCenterF),
			       (Double)fabs(theRegridCenterF - transNewXin[0] + transCHAN_WIDTH[0]/2.)) * 2.;
	    if(theRegridBWF<transCHAN_WIDTH[0]){
	      theRegridCenterF = (transNewXin[0]+transCHAN_WIDTH[oldNUM_CHAN-1]+transCHAN_WIDTH[oldNUM_CHAN-1]/2.-transCHAN_WIDTH[0]/2.)/2.;
	      theRegridBWF = transCHAN_WIDTH[oldNUM_CHAN-1]-transNewXin[0]
		+transCHAN_WIDTH[oldNUM_CHAN-1]/2. + transCHAN_WIDTH[0]/2.;
	    }
	  }
	  if((theRegridCenterF - theRegridBWF/2.) - (transNewXin[0] - transCHAN_WIDTH[0]/2.) < -rangeTol ){
	    oss << " *** Input spectral window exceeds lower end of original window. Adjusting to max. possible value." << endl;
	    theRegridBWF = min((Double)fabs(transNewXin[oldNUM_CHAN-1] + transCHAN_WIDTH[oldNUM_CHAN-1]/2. - theRegridCenterF),
			       (Double)fabs(theRegridCenterF - transNewXin[0] + transCHAN_WIDTH[0]/2.)) * 2.;
	    if(theRegridBWF<transCHAN_WIDTH[0]){
	      theRegridCenterF = (transNewXin[0]+transCHAN_WIDTH[oldNUM_CHAN-1]+transCHAN_WIDTH[oldNUM_CHAN-1]/2.-transCHAN_WIDTH[0]/2.)/2.;
	      theRegridBWF = transCHAN_WIDTH[oldNUM_CHAN-1]-transNewXin[0]
		+transCHAN_WIDTH[oldNUM_CHAN-1]/2. + transCHAN_WIDTH[0]/2.;
	    }
	  }
	}
      }
      if(regridChanWidthF <= 0.){ // "not set"
	if(nchan!=0 || centerIsStartC){ // use first channel
	  theCentralChanWidthF = transCHAN_WIDTH[0];
	}
	else{
	  // keep channel width similar to the old one 
	  theCentralChanWidthF = transCHAN_WIDTH[oldNUM_CHAN/2]; // use channel width from
	                                                         // near central channel
	}
      }
      else { // regridChanWidthF was set
	// keep in limits
	theCentralChanWidthF = regridChanWidthF;
	if(theCentralChanWidthF>theRegridBWF){ // too large => make a single channel
	  theCentralChanWidthF = theRegridBWF;
	  oss << " *** Requested new channel width exceeds defined SPW width." << endl
	      << "     Creating a single channel with the defined SPW width." << endl;
	}
	else if(theCentralChanWidthF<transCHAN_WIDTH[0]){ // check if too small
	  // determine smallest channel width
	  lDouble smallestChanWidth = 1E30;
	  Int ii = 0;
	  for(Int i=0; i<oldNUM_CHAN; i++){
	    if(transCHAN_WIDTH[i] < smallestChanWidth){ 
	      smallestChanWidth = transCHAN_WIDTH[i];
	      ii = i;
	    }
	  }
	  if(theCentralChanWidthF < smallestChanWidth - 1.){ // 1 Hz tolerance to cope with numerical accuracy problems
	    oss << " *** Requested new channel width is smaller than smallest original channel width" << endl;
	    oss << "     which is " << smallestChanWidth << " Hz" << endl;
	    if(regridQuant == "vrad"){
	      oss << "     or " << (vrad(transNewXin[ii],regridVeloRestfrq) 
				    - vrad(transNewXin[ii]+transCHAN_WIDTH[ii]/2.,regridVeloRestfrq)) * 2. << " m/s";
	    }
	    if(regridQuant == "vopt"){
	      oss << "     or " << (vopt(transNewXin[ii],regridVeloRestfrq) 
				    - vopt(transNewXin[ii]+transCHAN_WIDTH[ii]/2.,regridVeloRestfrq)) * 2. << " m/s";
	    }
	    message = oss.str();
	    return false;  
	  }
	  else { // input channel width was OK, memorize
	    theChanWidthX = regridChanWidth;
	  }
	}   	    
      }

      oss << " Channels equidistant in " << regridQuant << endl
	  << " Central frequency (in output frame) = " << theRegridCenterF
          << " Hz";
      if(regridQuant == "vrad"){
	oss << " == " << vrad(theRegridCenterF, regridVeloRestfrq) << " m/s radio velocity";
      }
      else if(regridQuant == "vopt"){
	oss << " == " << vopt(theRegridCenterF, regridVeloRestfrq) << " m/s optical velocity";
      }      
      else if(regridQuant == "wave"){
	oss << " == " << lambda(theRegridCenterF) << " m wavelength";
      }
      oss << endl;

      if(isDescending){
	oss << " Channel central frequency is decreasing with increasing channel number." << endl;
      }

      oss << " Width of central channel (in output frame) = "
          << theCentralChanWidthF << " Hz";
      if(regridQuant == "vrad"){
	oss << " == " << vrad(theRegridCenterF - theCentralChanWidthF, regridVeloRestfrq) 
	  -  vrad(theRegridCenterF, regridVeloRestfrq) << " m/s radio velocity";
      }
      else if(regridQuant == "vopt"){
	oss << " == " << vopt(theRegridCenterF - theCentralChanWidthF, regridVeloRestfrq) 
	  - vopt(theRegridCenterF, regridVeloRestfrq) << " m/s optical velocity";
      }      
      else if(regridQuant == "wave"){
	oss << " == " << lambda(theRegridCenterF - theCentralChanWidthF) - lambda(theRegridCenterF) << " m wavelength";
      }
      oss << endl;
       
      // now calculate newChanLoBound, and newChanHiBound from
      // theRegridCenterF, theRegridBWF, theCentralChanWidthF
      vector<lDouble> loFBup; // the lower bounds for the new channels 
                             // starting from the central channel going up
      vector<lDouble> hiFBup; // the lower bounds for the new channels 
	                     // starting from the central channel going up
      vector<lDouble> loFBdown; // the lower bounds for the new channels
                               // starting from the central channel going down
      vector<lDouble> hiFBdown; // the lower bounds for the new channels
                               // starting from the central channel going down
      
      lDouble edgeTolerance = theCentralChanWidthF*0.01; // needed to avoid numerical accuracy problems

      if(regridQuant=="vrad"){
	// regridding in radio velocity ...
	
	// create freq boundaries equidistant and contiguous in radio velocity
	lDouble upperEndF = theRegridCenterF + theRegridBWF/2.;
	lDouble lowerEndF = theRegridCenterF - theRegridBWF/2.;
	lDouble upperEndV = vrad(upperEndF,regridVeloRestfrq);
	lDouble lowerEndV = vrad(lowerEndF,regridVeloRestfrq);
	lDouble velLo;
	lDouble velHi;


	//    Want to keep the center of the center channel at the center of
	//    the new center channel if the bandwidth is an odd multiple of the
        //    new channel width,
	//    otherwise the center channel is the lower edge of the new center channel
	lDouble tnumChan = floor((theRegridBWF+edgeTolerance)/theCentralChanWidthF);
	if((Int)tnumChan % 2 != 0 ){
          // odd multiple 
	  loFBup.push_back(theRegridCenterF-theCentralChanWidthF/2.);
	  hiFBup.push_back(theRegridCenterF+theCentralChanWidthF/2.);
	  loFBdown.push_back(theRegridCenterF-theCentralChanWidthF/2.);
	  hiFBdown.push_back(theRegridCenterF+theCentralChanWidthF/2.);
	}
	else{
	  loFBup.push_back(theRegridCenterF);
	  hiFBup.push_back(theRegridCenterF+theCentralChanWidthF);
	  loFBdown.push_back(theRegridCenterF);
	  hiFBdown.push_back(theRegridCenterF+theCentralChanWidthF);
	}

	if(theChanWidthX<0){ // cannot use original channel width in velocity units
	  // need to calculate back from central channel width in Hz
	  theChanWidthX = vrad(loFBup[0],
                               regridVeloRestfrq) - vrad(hiFBup[0],
                                                         regridVeloRestfrq);
	}
	// calc velocity corresponding to the upper end (in freq) of the last
	// added channel which is the lower end of the next channel
	velLo = vrad(hiFBup[0],regridVeloRestfrq);
	// calc velocity corresponding to the upper end (in freq) of the next channel
	velHi = velLo - theChanWidthX; // vrad goes down as freq goes up!
	while(upperEndV - theChanWidthX/10. < velHi){ // (preventing accuracy problems)
	  // calc frequency of the upper end (in freq) of the next channel
	  lDouble freqHi = freq_from_vrad(velHi,regridVeloRestfrq);
	  if(freqHi<=upperEndF+edgeTolerance){ // end of bandwidth not yet reached
	    loFBup.push_back(hiFBup.back());
	    hiFBup.push_back(freqHi);
	  }
	  else if(freqHi<upperEndF+edgeTolerance){ 
	    loFBup.push_back(hiFBup.back());
	    hiFBup.push_back(upperEndF);
	    break;
	  }
	  else{
	    break;
	  }
	  // calc velocity corresponding to the upper end (in freq) of the added channel
	  velLo = vrad(hiFBup.back(),regridVeloRestfrq);
	  // calc velocity corresponding to the upper end (in freq) of the next channel
	  velHi = velLo - theChanWidthX; // vrad goes down as freq goes up
	}

	// calc velocity corresponding to the lower end (in freq) of the last
	// added channel which is the upper end of the next channel
	velHi = vrad(loFBdown[0],regridVeloRestfrq);
	// calc velocity corresponding to the lower end (in freq) of the next channel
	velLo = velHi + theChanWidthX; // vrad goes up as freq goes down!
	while(velLo < lowerEndV + theChanWidthX/10.){ // (preventing accuracy problems)  
	  // calc frequency of the lower end (in freq) of the next channel
	  lDouble freqLo = freq_from_vrad(velLo,regridVeloRestfrq);
	  if(freqLo>=lowerEndF-edgeTolerance){ // end of bandwidth not yet reached
	    hiFBdown.push_back(loFBdown.back());
	    loFBdown.push_back(freqLo);
	  }
	  else if(freqLo>lowerEndF-edgeTolerance){ 
	    hiFBdown.push_back(loFBdown.back());
	    loFBdown.push_back(lowerEndF);
	    break;
	  }
	  else {
	    break;
	  }
	  // calc velocity corresponding to the upper end of the next channel
	  velHi = vrad(loFBdown.back(),regridVeloRestfrq);
	  // calc velocity corresponding to the lower end (in freq) of the next channel
	  velLo = velHi + theChanWidthX; // vrad goes up as freq goes down
	}	  
      }
      else if(regridQuant=="vopt"){
	// regridding in optical velocity ...
	
	// create freq boundaries equidistant and contiguous in optical velocity
	lDouble upperEndF = theRegridCenterF + theRegridBWF/2.;
	lDouble lowerEndF = theRegridCenterF - theRegridBWF/2.;
	lDouble upperEndV = vopt(upperEndF,regridVeloRestfrq);
	lDouble lowerEndV = vopt(lowerEndF,regridVeloRestfrq);
	lDouble velLo;
	lDouble velHi;

	//    Want to keep the center of the center channel at the center of
	//    the new center channel if the bandwidth is an odd multiple of the
	//    new channel width,
	//    otherwise the center channel is the lower edge of the new center
	//    channel

	// enlarged edge tolerance since channels non-equidistant in freq
	lDouble tnumChan = floor((theRegridBWF+edgeTolerance)/theCentralChanWidthF); 
	if((Int)tnumChan % 2 != 0 ){
          // odd multiple 
	  loFBup.push_back(theRegridCenterF-theCentralChanWidthF/2.);
	  hiFBup.push_back(theRegridCenterF+theCentralChanWidthF/2.);
	  loFBdown.push_back(theRegridCenterF-theCentralChanWidthF/2.);
	  hiFBdown.push_back(theRegridCenterF+theCentralChanWidthF/2.);
	}
	else{
	  loFBup.push_back(theRegridCenterF);
	  hiFBup.push_back(theRegridCenterF+theCentralChanWidthF);
	  loFBdown.push_back(theRegridCenterF);
	  hiFBdown.push_back(theRegridCenterF+theCentralChanWidthF);
	}

	if(theChanWidthX<0){ // cannot use original channel width in velocity units
	  // need to calculate back from central channel width in Hz
	  theChanWidthX = vopt(loFBup[0],
                               regridVeloRestfrq) - vopt(hiFBup[0],regridVeloRestfrq);
	}
	// calc velocity corresponding to the upper end (in freq) of the last
	// added channel which is the lower end of the next channel
	velLo = vopt(hiFBup[0],regridVeloRestfrq);
	// calc velocity corresponding to the upper end (in freq) of the next channel
	velHi = velLo - theChanWidthX; // vopt goes down as freq goes up!
	while(upperEndV - velHi < theChanWidthX/10.){ // (preventing accuracy problems)
	  // calc frequency of the upper end (in freq) of the next channel
	  lDouble freqHi = freq_from_vopt(velHi,regridVeloRestfrq);
	  if(freqHi<=upperEndF+edgeTolerance){ // end of bandwidth not yet reached
	    loFBup.push_back(hiFBup.back());
	    hiFBup.push_back(freqHi);
	  }
	  else if(freqHi<upperEndF+edgeTolerance){ 
	    loFBup.push_back(hiFBup.back());
	    hiFBup.push_back(upperEndF);
	    break;
	  }
	  else{
	    break;
	  }
	  // calc velocity corresponding to the upper end (in freq) of the added channel
	  velLo = vopt(hiFBup.back(),regridVeloRestfrq);
	  // calc velocity corresponding to the upper end (in freq) of the next channel
	  velHi = velLo - theChanWidthX; // vopt goes down as freq goes up
	}

	// calc velocity corresponding to the lower end (in freq) of the last
	// added channel which is the upper end of the next channel
	velHi = vopt(loFBdown[0],regridVeloRestfrq);
	// calc velocity corresponding to the lower end (in freq) of the next channel
	velLo = velHi + theChanWidthX; // vopt goes up as freq goes down!
	while(velLo - lowerEndV < theChanWidthX/10.){ // (preventing accuracy problems)  
	  // calc frequency of the lower end (in freq) of the next channel
	  lDouble freqLo = freq_from_vopt(velLo,regridVeloRestfrq);
	  if(freqLo>=lowerEndF-edgeTolerance){ // end of bandwidth not yet reached
	    hiFBdown.push_back(loFBdown.back());
	    loFBdown.push_back(freqLo);
	  }
	  else if(freqLo>lowerEndF-edgeTolerance){ 
	    hiFBdown.push_back(loFBdown.back());
	    loFBdown.push_back(lowerEndF);
	    break;
	  }
	  else {
	    break;
	  }
	  // calc velocity corresponding to the upper end of the next channel
	  velHi = vopt(loFBdown.back(),regridVeloRestfrq);
	  // calc velocity corresponding to the lower end (in freq) of the next channel
	  velLo = velHi + theChanWidthX; // vopt goes up as freq goes down
	}	  
      }
      else if(regridQuant=="freq"){
	// regridding in frequency  ...
	
	// create freq boundaries equidistant and contiguous in frequency
	lDouble upperEndF = theRegridCenterF + theRegridBWF/2.;
	lDouble lowerEndF = theRegridCenterF - theRegridBWF/2.;

	//    Want to keep the center of the center channel at the center of
	//    the new center channel if the bandwidth is an odd multiple of the
        //    new channel width, 
	//    otherwise the center channel is the lower edge of the new center channel
	lDouble tnumChan = floor((theRegridBWF+edgeTolerance)/theCentralChanWidthF);

	//cout << "theRegridBWF " << theRegridBWF << " upperEndF " << upperEndF << " lowerEndF " << lowerEndF << " tnumChan " << tnumChan << endl;

	if((Int) tnumChan % 2 != 0){
          // odd multiple 
	  loFBup.push_back(theRegridCenterF-theCentralChanWidthF/2.);
	  hiFBup.push_back(theRegridCenterF+theCentralChanWidthF/2.);
	  loFBdown.push_back(theRegridCenterF-theCentralChanWidthF/2.);
	  hiFBdown.push_back(theRegridCenterF+theCentralChanWidthF/2.);
	}
	else{
	  loFBup.push_back(theRegridCenterF);
	  hiFBup.push_back(theRegridCenterF+theCentralChanWidthF);
	  loFBdown.push_back(theRegridCenterF);
	  hiFBdown.push_back(theRegridCenterF+theCentralChanWidthF);
	}

	while(hiFBup.back()< upperEndF+edgeTolerance){
	  // calc frequency of the upper end of the next channel
	  lDouble freqHi = hiFBup.back() + theCentralChanWidthF;
	  if(freqHi<=upperEndF+edgeTolerance){ // end of bandwidth not yet reached
	    loFBup.push_back(hiFBup.back());
	    hiFBup.push_back(freqHi);
	  }
	  else{
	    break;
	  }
	}

	while(loFBdown.back() > lowerEndF-edgeTolerance){
	  // calc frequency of the lower end of the next channel
	  lDouble freqLo = loFBdown.back() - theCentralChanWidthF;
	  if(freqLo>=lowerEndF-edgeTolerance){ // end of bandwidth not yet reached
	    hiFBdown.push_back(loFBdown.back());
	    loFBdown.push_back(freqLo);
	  }
	  else {
	    break;
	  }
	}	  
      }
      else if(regridQuant=="wave"){
	// regridding in wavelength  ...
	
	// create freq boundaries equidistant and contiguous in wavelength
	lDouble upperEndF = theRegridCenterF + theRegridBWF/2.;
	lDouble lowerEndF = theRegridCenterF - theRegridBWF/2.;
	lDouble upperEndL = lambda(upperEndF);
	lDouble lowerEndL = lambda(lowerEndF);
	lDouble lambdaLo;
	lDouble lambdaHi;

	//    Want to keep the center of the center channel at the center of
	//    the new center channel if the bandwidth is an odd multiple of the
	//    new channel width, 
	//    otherwise the center channel is the lower edge of the new center
	//    channel
	lDouble tnumChan = floor((theRegridBWF+edgeTolerance)/theCentralChanWidthF);
	if((Int)tnumChan % 2 != 0){
          // odd multiple 
	  loFBup.push_back(theRegridCenterF-theCentralChanWidthF/2.);
	  hiFBup.push_back(theRegridCenterF+theCentralChanWidthF/2.);
	  loFBdown.push_back(theRegridCenterF-theCentralChanWidthF/2.);
	  hiFBdown.push_back(theRegridCenterF+theCentralChanWidthF/2.);
	}
	else{
	  loFBup.push_back(theRegridCenterF);
	  hiFBup.push_back(theRegridCenterF+theCentralChanWidthF);
	  loFBdown.push_back(theRegridCenterF);
	  hiFBdown.push_back(theRegridCenterF+theCentralChanWidthF);
	}

	if(theChanWidthX<0){ // cannot use original channel width in wavelength units
	  // need to calculate back from central channel width in Hz
	  theChanWidthX = lambda(loFBup[0]) - lambda(hiFBup[0]);
	}
	// calc wavelength corresponding to the upper end (in freq) of the last
	// added channel which is the lower end of the next channel
	lambdaLo = lambda(hiFBup[0]);
	// calc wavelength corresponding to the upper end (in freq) of the next channel
	lambdaHi = lambdaLo - theChanWidthX; // lambda goes down as freq goes up!
	while(upperEndL - lambdaHi < theChanWidthX/10.){ // (preventing accuracy problems)
	  // calc frequency of the upper end (in freq) of the next channel
	  lDouble freqHi = freq_from_lambda(lambdaHi);
	  if(freqHi<=upperEndF+edgeTolerance){ // end of bandwidth not yet reached
	    loFBup.push_back(hiFBup.back());
	    hiFBup.push_back(freqHi);
	  }
	  else if(freqHi<upperEndF+edgeTolerance){ 
	    loFBup.push_back(hiFBup.back());
	    hiFBup.push_back(upperEndF);
	    break;
	  }
	  else{
	    break;
	  }
	  // calc wavelength corresponding to the upper end (in freq) of the
	  // added channel
	  lambdaLo = lambda(hiFBup.back());
	  // calc wavelength corresponding to the upper end (in freq) of the next channel
	  lambdaHi = lambdaLo - theChanWidthX; // lambda goes down as freq goes up
	}

	// calc wavelength corresponding to the lower end (in freq) of the last
	// added channel which is the upper end of the next channel
	lambdaHi = lambda(loFBdown[0]);
	// calc wavelength corresponding to the lower end (in freq) of the next channel
	lambdaLo = lambdaHi + theChanWidthX; // lambda goes up as freq goes down!
	while(lambdaLo - lowerEndL < theChanWidthX/10.){  // (preventing accuracy problems) 
	  // calc frequency of the lower end (in freq) of the next channel
	  lDouble freqLo = freq_from_lambda(lambdaLo);
	  if(freqLo>=lowerEndF-edgeTolerance){ // end of bandwidth not yet reached
	    hiFBdown.push_back(loFBdown.back());
	    loFBdown.push_back(freqLo);
	  }
	  else if(freqLo>lowerEndF-edgeTolerance){ 
	    hiFBdown.push_back(loFBdown.back());
	    loFBdown.push_back(lowerEndF);
	    break;
	  }
	  else {
	    break;
	  }
	  // calc wavelength corresponding to the upper end of the next channel
	  lambdaHi = lambda(loFBdown.back());
	  // calc wavelength corresponding to the lower end (in freq) of the next channel
	  lambdaLo = lambdaHi + theChanWidthX; // wavelength goes up as freq goes down
	}	  

      }
      else{ // should not get here
	oss << "Invalid value " << regridQuant << " for parameter \"mode\".";
	message = oss.str();
	return false;
      }

      Int numNewChanDown = loFBdown.size();
      Int numNewChanUp = loFBup.size();

      // central channel contained in both vectors
      newChanLoBound.resize(numNewChanDown+numNewChanUp - 1);

      newChanHiBound.resize(numNewChanDown+numNewChanUp - 1);
      for(Int i=0; i<numNewChanDown; i++){ 
	Int k = numNewChanDown-i-1; // need to assign in reverse
	newChanLoBound[i] = loFBdown[k];
	newChanHiBound[i] = hiFBdown[k];
      }
      for(Int i=1; i<numNewChanUp; i++){ // start at 1 to omit the central channel here
	newChanLoBound[i+numNewChanDown-1] = loFBup[i];
	newChanHiBound[i+numNewChanDown-1] = hiFBup[i];
      }
      
      uInt nc = newChanLoBound.size();
      oss << " Number of channels = " << nc << endl;
      oss << " Total width of SPW (in output frame) = " << newChanHiBound[nc-1] - newChanLoBound[0] 
	  << " Hz" << endl;
      oss << " Lower edge = " << newChanLoBound[0] << " Hz,"
	  << " upper edge = " << newChanHiBound[nc-1] << " Hz" << endl;

      if(isDescending){ // original SPW was in reverse order; need to restore that
	Vector<Double> tempL, tempU;
	tempL.assign(newChanLoBound);
	tempU.assign(newChanHiBound);
	for(uInt i=0; i<nc; i++){
	  newChanLoBound(i) = tempL(nc-1-i);
	  newChanHiBound(i) = tempU(nc-1-i);
	}
      }

      message = oss.str();

      return true;
      
    } // end if (regridQuant== ...

  }

  Bool SubMS::convertGridPars(LogIO& os,
			      const String& mode, 
			      const int nchan, 
			      const String& start, 
			      const String& width,
			      const String& interp, 
			      const String& restfreq, 
			      const String& outframe,
			      const String& veltype,
			      String& t_mode,
			      String& t_outframe,
			      String& t_regridQuantity,
			      Double& t_restfreq,
			      String& t_regridInterpMeth,
			      Double& t_cstart, 
			      Double& t_bandwidth,
			      Double& t_cwidth,
			      Bool& t_centerIsStart, 
			      Bool& t_startIsEnd,			      
			      Int& t_nchan,
			      Int& t_width,
			      Int& t_start
			      ){
    Bool rstat(false);
    
    try {
      
      os << LogOrigin("SubMS", "convertGridPars");
      
      casacore::QuantumHolder qh;
      String error;

      t_mode = mode;
      t_restfreq = 0.; 
      if(!restfreq.empty() && !(restfreq=="[]")){
	if(qh.fromString(error, restfreq)){
	  t_restfreq = qh.asQuantity().getValue("Hz");
	}
	else{
	  os << LogIO::SEVERE  << "restfreq: " << error << LogIO::POST; 	  
	  return false;
	}
      }
      
      // Determine grid
      t_cstart = -9e99; // default value indicating that the original start of the SPW should be used
      t_bandwidth = -1.; // default value indicating that the original width of the SPW should be used
      t_cwidth = -1.; // default value indicating that the original channel width of the SPW should be used
      t_nchan = -1; 
      t_width = 0;
      t_start = -1;
      t_startIsEnd = false; // false means that start specifies the lower end in frequency (default)
      // true means that start specifies the upper end in frequency

      if(!start.empty() && !(start=="[]")){ // start was set
	if(t_mode == "channel"){
	  t_start = atoi(start.c_str());
	}
	if(t_mode == "channel_b"){
	  t_cstart = Double(atoi(start.c_str()));
	}
	else if(t_mode == "frequency"){
	  if(qh.fromString(error, start)){
	    t_cstart = qh.asQuantity().getValue("Hz");
	  }
	  else{
	    os << LogIO::SEVERE  << "start: " << error << LogIO::POST; 	  
	    return false;
	  }	
	}
	else if(t_mode == "velocity"){
	  if(qh.fromString(error, start)){
	    t_cstart = qh.asQuantity().getValue("m/s");
	  }
	  else{
	    os << LogIO::SEVERE << "start: " << error << LogIO::POST; 	  
	    return false;
	  }	
	}
      }
      if(!width.empty() && !(width=="[]")){ // channel width was set
	if(t_mode == "channel"){
	  Int w = atoi(width.c_str());
	  t_width = abs(w);
	  if(w<0){
	    t_startIsEnd = true;
	  }
	}
	else if(t_mode == "channel_b"){
	  Double w = atoi(width.c_str());
	  t_cwidth = abs(w);
	  if(w<0){
	    t_startIsEnd = true;
	  }	
	}
	else if(t_mode == "frequency"){
	  if(qh.fromString(error, width)){
	    Double w = qh.asQuantity().getValue("Hz");
	    t_cwidth = abs(w);
	    if(w<0){
	      t_startIsEnd = true;
	    }	
	  }
	  else{
	    os << LogIO::SEVERE << "width: " << error << LogIO::POST; 	  
	    return false;
	  }	
	}
	else if(t_mode == "velocity"){
	  if(qh.fromString(error, width)){
	    Double w = qh.asQuantity().getValue("m/s");
	    t_cwidth = abs(w);
	    if(w>=0){
	      t_startIsEnd = true; 
	    }		
	  }
	  else{
	    os << LogIO::SEVERE << "width: " << error << LogIO::POST; 	  
	    return false;	    
	  }
	}
      }
      else{ // width was not set
	// for the velocity mode the default t_startIsEnd is true if the sign of width is not known
	if(t_mode == "velocity"){
	  t_startIsEnd = true;
	}
      }

      if(nchan > 0){ // number of output channels was set
	if(t_mode == "channel_b"){
	  if(t_cwidth>0){
	    t_bandwidth = Double(nchan*t_cwidth);
	  }
	  else{
	    t_bandwidth = Double(nchan);	  
	  }
	}
	else{
	  t_nchan = nchan;
	}
      }
      
      if(t_mode == "channel"){
	t_regridQuantity = "freq";
      }
      else if(t_mode == "channel_b"){
	t_regridQuantity = "chan";
      }
      else if(t_mode == "frequency"){
	t_regridQuantity = "freq";
      }
      else if(t_mode == "velocity"){
	if(t_restfreq == 0.){
	  os << LogIO::SEVERE << "Need to set restfreq in velocity mode." << LogIO::POST; 
	  return false;
	}	
	t_regridQuantity = "vrad";
	if(veltype == "optical"){
	  t_regridQuantity = "vopt";
	}
	else if(veltype != "radio"){
	  os << LogIO::WARN << "Invalid velocity type "<< veltype 
	     << ", setting type to \"radio\"" << LogIO::POST; 
	}
      }   
      else{
	os << LogIO::WARN << "Invalid mode " << t_mode << LogIO::POST;
	return false;
      }
      
      t_outframe=outframe;
      t_regridInterpMeth=interp;
      t_centerIsStart = true;
            
      // end prepare regridding parameters
      
      rstat = true;

    } catch (AipsError x) {
      os << LogIO::SEVERE << "Exception Reported: " << x.getMesg() << LogIO::POST;
      rstat = false;
    }
    return rstat;
  }


  Bool SubMS::calcChanFreqs(LogIO& os,
			    Vector<Double>& newCHAN_FREQ, 
			    Vector<Double>& newCHAN_WIDTH,
			    const Vector<Double>& oldCHAN_FREQ, 
			    const Vector<Double>& oldCHAN_WIDTH,
			    const MDirection  phaseCenter,
			    const MFrequency::Types theOldRefFrame,
			    const MEpoch theObsTime,
			    const MPosition mObsPos,
			    const String& mode, 
			    const int nchan, 
			    const String& start, 
			    const String& width,
			    const String& restfreq, 
			    const String& outframe,
			    const String& veltype,
			    const Bool verbose,
			    const MRadialVelocity mRV // only used for outframe=="SOURCE"
			    ){

    Double weightScale;
    return calcChanFreqs(os,
			 newCHAN_FREQ, 
			 newCHAN_WIDTH,
			 weightScale,
			 oldCHAN_FREQ, 
			 oldCHAN_WIDTH,
			 phaseCenter,
			 theOldRefFrame,
			 theObsTime,
			 mObsPos,
			 mode, 
			 nchan, 
			 start, 
			 width,
			 restfreq, 
			 outframe,
			 veltype,
			 verbose,
			 mRV // only used for outframe=="SOURCE"
			 );
  }


  Bool SubMS::calcChanFreqs(LogIO& os,
			    Vector<Double>& newCHAN_FREQ, 
			    Vector<Double>& newCHAN_WIDTH,
			    Double& weightScale,
			    const Vector<Double>& oldCHAN_FREQ, 
			    const Vector<Double>& oldCHAN_WIDTH,
			    const MDirection  phaseCenter,
			    const MFrequency::Types theOldRefFrame,
			    const MEpoch theObsTime,
			    const MPosition mObsPos,
			    const String& mode, 
			    const int nchan, 
			    const String& start, 
			    const String& width,
			    const String& restfreq, 
			    const String& outframe,
			    const String& veltype,
			    const Bool verbose,
			    const MRadialVelocity mRV // only used for outframe=="SOURCE"
			    ){

    Vector<Double> newChanLoBound; 
    Vector<Double> newChanHiBound;
    String t_phasec;

    String t_mode;
    String t_outframe;
    String t_regridQuantity;
    Double t_restfreq;
    String t_regridInterpMeth;
    Double t_cstart;
    Double t_bandwidth;
    Double t_cwidth;
    Bool t_centerIsStart;
    Bool t_startIsEnd;
    Int t_nchan;
    Int t_width;
    Int t_start;

    if(!convertGridPars(os,
			mode, 
			nchan, 
			start, 
			width,
			"linear", // a dummy value in this context
			restfreq, 
			outframe,
			veltype,
			////// output ////
			t_mode,
			t_outframe,
			t_regridQuantity,
			t_restfreq,
			t_regridInterpMeth,
			t_cstart, 
			t_bandwidth,
			t_cwidth,
			t_centerIsStart, 
			t_startIsEnd,			      
			t_nchan,
			t_width,
			t_start
			)
       ){
      // an error occured
      return false;
    }

    // reference frame transformation
    Bool needTransform = true;
    Bool doRadVelCorr = false;
    MFrequency::Types theFrame;
    String oframe = outframe;
    oframe.upcase();
    if(outframe==""){ // no ref frame given 
      // keep the reference frame as is
      theFrame = theOldRefFrame;
      needTransform = false;
    }
    else if(oframe=="SOURCE"){ // GEO trafo + radial velocity correction
      theFrame = MFrequency::GEO;
      doRadVelCorr = true;
    }
    else if(!MFrequency::getType(theFrame, outframe)){
      os << LogIO::SEVERE
	 << "Parameter \"outframe\" value " << outframe << " is invalid." 
	 << LogIO::POST;
      return false;
    }
    else if (theFrame == theOldRefFrame){
      needTransform = false;
    }

    uInt oldNUM_CHAN = oldCHAN_FREQ.size();
    if(oldNUM_CHAN == 0){
      newCHAN_FREQ.resize(0);
      newCHAN_WIDTH.resize(0);
      return true;
    }

    if(oldNUM_CHAN != oldCHAN_WIDTH.size()){
      os << LogIO::SEVERE
	 << "Internal error: inconsistent dimensions of input channel freq and width arrays." 
	 << LogIO::POST;
      return false;
    }      

    Vector<Double> absOldCHAN_WIDTH;
    absOldCHAN_WIDTH.assign(oldCHAN_WIDTH);
    {
      Bool negativeWidths = false;
      for(uInt i=0; i<oldCHAN_WIDTH.size(); i++){
	if(oldCHAN_WIDTH(i) < 0.){
	  negativeWidths = true;
	  absOldCHAN_WIDTH(i) = -oldCHAN_WIDTH(i);
	}
      }
      if(negativeWidths && verbose){
	os << LogIO::NORMAL
	   << " *** Encountered negative channel widths in input spectral window."
	   << LogIO::POST;
      }
    }

    Vector<Double> transNewXin;
    Vector<Double> transCHAN_WIDTH(oldNUM_CHAN);

    if(needTransform){
      transNewXin.resize(oldNUM_CHAN);
      // set up conversion
      Unit unit(String("Hz"));
      MFrequency::Ref fromFrame = MFrequency::Ref(theOldRefFrame, MeasFrame(phaseCenter, mObsPos, theObsTime));
      MFrequency::Ref toFrame = MFrequency::Ref(theFrame, MeasFrame(phaseCenter, mObsPos, theObsTime));
      MFrequency::Convert freqTrans(unit, fromFrame, toFrame);
      
      MDoppler radVelCorr; // no correction
      Bool radVelSignificant = false; // is the radial velocity large enough to warrant a shift?
      // prepare correction for radial velocity if requested and possible
      if(doRadVelCorr){
	Quantity mrv = mRV.get("m/s"); // (const)
	radVelCorr = MDoppler(-mrv); // NOTE: opposite sign to achieve correction!
	Double mRVVal = mrv.getValue();
	if(fabs(mRVVal)>1E-6){
	  radVelSignificant = true;
	  if(verbose){
	    os << LogIO::NORMAL
	       << "Note: The given additional radial velocity of " << mRVVal << " m/s will be taken into account."
	       << LogIO::POST;
	  }
	}
	else if(verbose){
	  os << LogIO::NORMAL
	     << "Note: The given additional radial velocity is less than 1E-6 m/s and will not be taken into account."
	     << LogIO::POST;
	}
      } 

      for(uInt i=0; i<oldNUM_CHAN; i++){
	transNewXin[i] = freqTrans(oldCHAN_FREQ[i]).get(unit).getValue();
	transCHAN_WIDTH[i] = freqTrans(oldCHAN_FREQ[i] +
				       absOldCHAN_WIDTH[i]/2.).get(unit).getValue()
	  - freqTrans(oldCHAN_FREQ[i] -
		      absOldCHAN_WIDTH[i]/2.).get(unit).getValue(); // eliminate possible offsets
      }

      if(radVelSignificant){ // correct in addition for radial velocity
	transNewXin  = radVelCorr.shiftFrequency(transNewXin);
	transCHAN_WIDTH = radVelCorr.shiftFrequency(transCHAN_WIDTH); //shiftFrequency is a scaling, so chan widths scale as well
      }

    }
    else {
      // just copy
      transNewXin.assign(oldCHAN_FREQ);
      transCHAN_WIDTH.assign(absOldCHAN_WIDTH);
    }

    // calculate new grid

    String message;

    if(!regridChanBounds(newChanLoBound, 
			 newChanHiBound,
			 t_cstart,  
			 t_bandwidth, 
			 t_cwidth, 
			 t_restfreq,
			 t_regridQuantity,
			 transNewXin, 
			 transCHAN_WIDTH,
			 message,
			 t_centerIsStart,
			 t_startIsEnd,
			 t_nchan,
			 t_width,
			 t_start
			 )
       ){ // there was an error
      os << LogIO::WARN << message << LogIO::POST;
      return false;
    }
    
    if(verbose){
      os << LogIO::NORMAL << message << LogIO::POST;
    }

    // we have a useful set of channel boundaries
    uInt newNUM_CHAN = newChanLoBound.size();
    
    // complete the calculation of the new channel centers and widths
    // from newNUM_CHAN, newChanLoBound, and newChanHiBound 
    newCHAN_FREQ.resize(newNUM_CHAN);
    newCHAN_WIDTH.resize(newNUM_CHAN);
    for(uInt i=0; i<newNUM_CHAN; i++){
      newCHAN_FREQ[i] = (newChanLoBound[i]+newChanHiBound[i])/2.;
      newCHAN_WIDTH[i] = newChanHiBound[i]-newChanLoBound[i];
      //cout << "new lo hi freq width " << newChanLoBound[i] << " " << newChanHiBound[i] << " " << newCHAN_FREQ[i] << " " << newCHAN_WIDTH[i] << endl;
    }

    weightScale = newCHAN_WIDTH[0]/transCHAN_WIDTH[0];
    
    return true;

  }
  

  Bool SubMS::setRegridParameters(vector<Int>& oldSpwId,
				  vector<Int>& oldFieldId,
				  vector<Int>& newDataDescId,
				  vector<Bool>& regrid,
				  vector<Bool>& transform,
				  vector<MDirection>& theFieldDirV,
				  vector<MPosition>& mObsPosV,
				  vector<MFrequency::Types>& fromFrameTypeV,
				  vector<MFrequency::Ref>& outFrameV,
				  vector<MRadialVelocity>& outRadVelV,
				  vector< Double >& weightScaleV, 
				  vector< Vector<Double> >& xold, 
				  vector< Vector<Double> >& xout, 
				  vector< Vector<Double> >& xin, 
				  vector<Int>& method,
				  Bool& msModified,
				  const String& outframe,
				  const String& regridQuant,
				  const Double regridVeloRestfrq,
				  const String& regridInterpMeth,
				  const Double regridCenter, 
				  const Double regridBandwidth, 
				  const Double regridChanWidth,
				  const Int regridPhaseCenterFieldId,
				  const MDirection regridPhaseCenter,
				  const Bool writeTables,
				  LogIO& os,
				  String& regridMessage,
				  const Bool centerIsStart,
				  const Bool startIsEnd,
				  const Int nchan,
				  const Int width,
				  const Int start
				  )
  {
    Bool rval = true;

    // reset the "done" table.
    newDataDescId.resize(0);
    oldSpwId.resize(0);
    oldFieldId.resize(0);
    xold.resize(0);
    xin.resize(0);
    xout.resize(0);
    theFieldDirV.resize(0);
    mObsPosV.resize(0);
    fromFrameTypeV.resize(0);
    outFrameV.resize(0);
    outRadVelV.resize(0);
    weightScaleV.resize(0);
    MFrequency::Ref outFrame;
    MRadialVelocity outRadVel(Quantity(0, "m/s"), MRadialVelocity::GEO);
    method.resize(0);
    regrid.resize(0);	
    transform.resize(0);	
    
    // Determine the highest data_desc_id from the DATA_DESCRIPTION table
    MSDataDescription ddtable = ms_p.dataDescription();
    Int origNumDataDescs = ddtable.nrow();
    Int nextDataDescId = origNumDataDescs - 1;
    Int numNewDataDesc = 0;

    // Determine the highest spw_id in the SPW table
    MSSpectralWindow spwtable = ms_p.spectralWindow();
    Int origNumSPWs = spwtable.nrow();
    Int nextSPWId = origNumSPWs - 1;
    Int numNewSPWIds = 0;

    // Determine the highest row number in the SOURCE table
    Int origNumSourceRows = 0;
    Int nextSourceRow = -1;
    Int numNewSourceRows = 0;
    vector<Int> newSourceIds;
    vector<Int> newSourceSPWIds;
    MSSource* p_sourcetable = NULL;
    MSSourceColumns* p_sourceCol = NULL;
    if(Table::isReadable(ms_p.sourceTableName())){
      p_sourcetable = &(ms_p.source());
      p_sourceCol = new MSSourceColumns(*p_sourcetable);
      origNumSourceRows = p_sourcetable->nrow();
      nextSourceRow = origNumSourceRows - 1;
    }
    else if(!writeTables) { // there is no source table
      os << LogIO::NORMAL << "Note: MS does not contain a SOURCE table ..." << LogIO::POST;
      nextSourceRow = -1;
    }

    MSMainColumns mainCols(ms_p);
    ScalarColumn<Int> fieldIdCol = mainCols.fieldId();
    ScalarColumn<Int> DDIdCol = mainCols.dataDescId();
    ScalarMeasColumn<MEpoch> mainTimeMeasCol = mainCols.timeMeas();
    // other administrational tables

    MSDataDescColumns DDCols(ddtable);
    ScalarColumn<Int> SPWIdCol = DDCols.spectralWindowId(); 

    MSSpWindowColumns SPWCols(spwtable);
    ScalarColumn<Int> numChanCol = SPWCols.numChan(); 
    ArrayColumn<Double> chanFreqCol = SPWCols.chanFreq(); 
    ArrayMeasColumn<MFrequency> chanFreqMeasCol = SPWCols.chanFreqMeas();
    ScalarColumn<Int> measFreqRefCol = SPWCols.measFreqRef();
    ArrayColumn<Double> chanWidthCol = SPWCols.chanWidth(); 
    ArrayColumn<Double> effectiveBWCol = SPWCols.effectiveBW();   
    ScalarColumn<Double> refFrequencyCol = SPWCols.refFrequency(); 
    ScalarMeasColumn<MFrequency> refFrequencyMeasCol = SPWCols.refFrequencyMeas(); 
    ArrayColumn<Double> resolutionCol = SPWCols.resolution(); 
    ScalarColumn<Double> totalBandwidthCol = SPWCols.totalBandwidth();

    MSField fieldtable = ms_p.field();
    MSFieldColumns FIELDCols(fieldtable);
    //ArrayMeasColumn<MDirection> referenceDirMeasCol = FIELDCols.referenceDirMeasCol(); 
    //ScalarMeasColumn<MEpoch>& timeMeasCol = FIELDCols.timeMeas();
    ScalarColumn<Int> FIELDsourceIdCol = FIELDCols.sourceId(); 

    // calculate mean antenna position for TOPO transformation
    MSAntenna anttable = ms_p.antenna();
    MSAntennaColumns ANTCols(anttable);
    ROScalarMeasColumn<MPosition> ANTPositionMeasCol = ANTCols.positionMeas(); 
    ScalarColumn<Bool> ANTflagRowCol = ANTCols.flagRow();
    Int nAnt = 0;
    Vector<Double> pos(3); pos=0;
    for (uInt i=0; i<anttable.nrow(); i++) {
      if(!ANTflagRowCol(i)){
	pos+=ANTPositionMeasCol(i).getValue().get();
	nAnt++;
      }
    }
    if(nAnt>0){
      pos /= Double(nAnt);
    }
    else {
      os << LogIO::WARN << "No unflagged antennas in this MS. Cannot proceed with regridSpw ..." 
	 << LogIO::POST;
      return rval; 
    }

    MPosition mObsPos = ANTPositionMeasCol(0); // transfer reference frame
    mObsPos.set(MVPosition(pos)); // set coordinates

    // but use a tabulated version if available (as in clean)
    {
      MPosition Xpos;
      String Xobservatory;
      MSObservationColumns XObsCols(ms_p.observation());
      if (ms_p.observation().nrow() > 0) {
	Xobservatory = XObsCols.telescopeName()(mainCols.observationId()(0));
      }
      if (Xobservatory.length() == 0 || 
	  !MeasTable::Observatory(Xpos,Xobservatory)) {
	// unknown observatory, use the above calculated centroid position
	if(!writeTables){
	  os << LogIO::WARN << "Unknown observatory: \"" << Xobservatory 
	     << "\". Determining observatory position from antenna 0." << LogIO::POST;
	}
	Xpos=MPosition::Convert(ANTPositionMeasCol(0), MPosition::ITRF)();
      }
      else{
	if(!writeTables){
	  os << LogIO::NORMAL << "Using tabulated observatory position for " << Xobservatory << ":"
	     << LogIO::POST;
	  Xpos=MPosition::Convert(Xpos, MPosition::ITRF)();
	}
      }
      mObsPos = Xpos;
      if(!writeTables){
	ostringstream oss;
	oss <<  "   " << mObsPos << " (ITRF)";
	os << LogIO::NORMAL << oss.str() << LogIO::POST;
      }
    }
    
    // create time-sorted index for main table access
    uInt nMainTabRows = ms_p.nrow();
    Vector<uInt> sortedI(nMainTabRows);
    {
      Vector<Double> mainTimesV = mainCols.time().getColumn();
      GenSortIndirect<Double>::sort(sortedI,mainTimesV);
    }
    
    for(uInt mainTabRowI=0; mainTabRowI<nMainTabRows; mainTabRowI++){
    
      uInt mainTabRow = sortedI(mainTabRowI); // i.e. mainTabRow is sorted in Time

      // For each MAIN table row, the FIELD_ID cell and the DATA_DESC_ID cell are read 
      Int theFieldId = fieldIdCol(mainTabRow);
      Int theDataDescId = DDIdCol(mainTabRow);
      // and the SPW_ID extracted from the corresponding row in the
      // DATA_DESCRIPTION table.
      Int theSPWId = -2;
      if (theDataDescId < origNumDataDescs){
	theSPWId = SPWIdCol(theDataDescId);
      }
      else {
	os << LogIO::SEVERE
           << "Incoherent MS: Found at main table row " << mainTabRow
	   << " reference to non-existing DATA_DESCRIPTION table entry "
           << theDataDescId
	   << LogIO::POST;
	rval = false;
	return rval;
      }

      // variables saying what has to be done for this row
      Bool needTransform = false;
      Bool doRegrid = false;
      Int equivalentSpwFieldPair = -1;

      String message;

      //  The pair (theFieldId, theSPWId) is looked up in the "done table". 
      Int iDone = -1;
      for (uInt i=0; i<oldSpwId.size(); i++){
	if(oldSpwId[i]==theSPWId && (oldFieldId[i]==theFieldId || regridPhaseCenterFieldId>=-1)){ 
	  // if common phase center is given, treat all fields the same
	  iDone = i;
	  break;
	}
      }

      if(iDone<0){ // this (theFieldId, theSPWId) pair was not yet encountered 

	// Determine information for new row in "done" table
	//   The information necessary for the transformation is extracted:  
	//   1) center frequency of each channel (taken from the CHAN_FREQ cell
	//      corresponding to theSPWId in the SPW table)
	Vector<Double> newXin;
	newXin.assign(chanFreqCol(theSPWId));
	//      -> store in  xin (further below)
	//   2) reference frame for these frequencies (taken from the
	//      MEAS_FREQ_REF cell corresponding to theSPWId in the SPW table)
	MFrequency::Types theOldRefFrame = MFrequency::castType(measFreqRefCol(theSPWId));
	//      -> store in oldRefFrame[numNewDataDesc] (further below)
	//   3) in case either the original or the destination reference frame
	//      is TOPO or GEO, we need the observation time
	//      (taken from the time of the first integration for this (theFieldId, theSPWId) pair)
	//      -> store in obsTime[numNewDataDesc] (further below)
       	MEpoch theObsTime = mainTimeMeasCol(mainTabRow);
	  ////      (taken uniformly for the whole MS from the first row of the MS
	  ////      which is also the earliest row because it is time-sorted)
       	  //MEpoch theObsTime = mainTimeMeasCol(0);

	// Determine if a reference frame transformation is necessary
	// Bool 	getType (MFrequency::Types &tp, const String &in)
	needTransform = true;
	Bool doRadVelCorr = false;
	MFrequency::Types theFrame;
	String oframe = outframe;
	oframe.upcase();
	if(outframe==""){ // no ref frame given 
	  // keep the reference frame as is
	  theFrame = theOldRefFrame;
	  needTransform = false;
	}
	else if(oframe=="SOURCE"){ // GEO trafo + radial velocity correction
	  theFrame = MFrequency::GEO;
	  doRadVelCorr = true;
	}
	else if(!MFrequency::getType(theFrame, outframe)){
	  os << LogIO::SEVERE
             << "Parameter \"outframe\" value " << outframe << " is invalid." 
	     << LogIO::POST;
	  return false;
	}
	else if (theFrame == theOldRefFrame){
	  needTransform = false;
	}

	Double weightScale = 1.;

	//   4) direction of the field, i.e. the phase center
	MDirection theFieldDir;
	uInt theFieldIdToUse = theFieldId;
	if(regridPhaseCenterFieldId<-1){ // take it from the PHASE_DIR cell
	                                 // corresponding to theFieldId in the FIELD table)
	  theFieldDir = FIELDCols.phaseDirMeas(theFieldId, mainCols.time()(mainTabRow));

	  if(FIELDCols.needInterTime(theFieldId)){
	    String ephPath = FIELDCols.ephemPath(theFieldId);
	    os << LogIO::NORMAL << "Note: Field with id " << theFieldId; 
	    if(ephPath.size()>0){
	      os << ", uses ephemeris " << ephPath << LogIO::POST;
	    }
	    else{
	      os << ", has a time-dependent position described by a polynomial." << LogIO::POST;
	    }
	  }

	}
	else if(regridPhaseCenterFieldId==-1){ // use the given direction
	  theFieldDir = regridPhaseCenter;
	  doRadVelCorr = false;
	}
	else if((uInt)regridPhaseCenterFieldId < fieldtable.nrow()){ // use this valid field ID
	  theFieldDir = FIELDCols.phaseDirMeas(regridPhaseCenterFieldId, mainCols.time()(mainTabRow));
	  theFieldIdToUse = regridPhaseCenterFieldId;
	  if(FIELDCols.needInterTime(regridPhaseCenterFieldId)){
	    String ephPath = FIELDCols.ephemPath(regridPhaseCenterFieldId);
	    os << LogIO::NORMAL << "Note: Field to be used as phase center, id " << regridPhaseCenterFieldId; 
	    if(ephPath.size()>0){
	      os << ", uses ephemeris " << ephPath << LogIO::POST;
	    }
	    else{
	      os << ", has a time-dependent position described by a polynomial." << LogIO::POST;
	    }
	  }

	}
	else{
	  os << LogIO::SEVERE << "Field to be used as phase center, id " 
	     << regridPhaseCenterFieldId 
	     << ", does not exist." << LogIO::POST;
	  return false;
	}

	//	cout << "theFieldId = " << theFieldId << ", theObsTime = " << theObsTime
	//	     << ", theFieldDir = " << theFieldDir.getAngle() << endl;

	//      -> store in fieldDir[numNewDataDesc] (further below)
	//   5) in case either the original or the destination reference frame
	//      (but not both) are TOPO, we need the observatory position
	//      (from the mean antenna position calculated above) 
	//      -> store in obsPos[numNewDataDesc] (further below)

	// Perform the pure frequency transformation (no regridding yet)
	Vector<Double> transNewXin;
	// also take care of the other parameters of the spectral window
	Int oldNUM_CHAN = numChanCol(theSPWId); 
	Vector<Double> oldCHAN_WIDTH = chanWidthCol(theSPWId);
	{
	  Bool negativeWidths = false;
	  for(uInt i=0; i<oldCHAN_WIDTH.nelements(); i++){
	    if(oldCHAN_WIDTH(i) < 0.){
	      negativeWidths = true;
	      oldCHAN_WIDTH(i) = -oldCHAN_WIDTH(i);
	    }
	  }
	  if(negativeWidths){
	    os << LogIO::NORMAL
	       << " *** Encountered negative channel widths in SPECTRAL_WINDOW table."
	       << LogIO::POST;
	  }
	}
	MFrequency oldREF_FREQUENCY = refFrequencyMeasCol(theSPWId);
	Double oldTOTAL_BANDWIDTH = totalBandwidthCol(theSPWId);
	Vector<Double> oldEFFECTIVE_BW = effectiveBWCol(theSPWId);   
	Vector<Double> oldRESOLUTION = resolutionCol(theSPWId);

	// storage for values with pure freq trafo applied
	Vector<Double> transCHAN_WIDTH(oldNUM_CHAN);
	MFrequency transREF_FREQUENCY;
	Double transTOTAL_BANDWIDTH;
	Vector<Double> transRESOLUTION(oldNUM_CHAN);;

	if(needTransform){

	  transNewXin.resize(oldNUM_CHAN);
	  // set up conversion
	  Unit unit(String("Hz"));
	  MFrequency::Ref fromFrame = MFrequency::Ref(theOldRefFrame, MeasFrame(theFieldDir, mObsPos, theObsTime));
	  MFrequency::Ref toFrame = MFrequency::Ref(theFrame, MeasFrame(theFieldDir, mObsPos, theObsTime));
	  MFrequency::Convert freqTrans(unit, fromFrame, toFrame);
	  
	  MDoppler radVelCorr; // no correction
	  Bool radVelSignificant = false; // is the radial velocity large enough to warrant a shift?
	  // prepare correction for radial velocity if requested and possible
	  if(doRadVelCorr && FIELDCols.needInterTime(theFieldIdToUse)){
	    MRadialVelocity mRV = FIELDCols.radVelMeas(theFieldIdToUse, mainCols.time()(mainTabRow));
	    Quantity mrv = mRV.get("m/s");
	    radVelCorr = MDoppler(-mrv); // NOTE: opposite sign to achieve correction
	    if(fabs(mrv.getValue())>1E-6){
	      radVelSignificant = true;
	      outRadVel = mRV;
	      os << LogIO::NORMAL
		 << "Note: The geocentric radial velocity information (ca. " << mrv.getValue()
		 << "m/s) from the ephemeris for field " << theFieldIdToUse << " will be taken into account."
		 << LogIO::POST;
	    }
	    else{
	      outRadVel = MRadialVelocity(Quantity(0, "m/s"), MRadialVelocity::GEO);
	      os << LogIO::NORMAL
		 << "Note: The geocentric radial velocity from the ephemeris for field " << theFieldIdToUse 
		 << " is less than 1E-6 m/s and will not be taken into account."
		 << LogIO::POST;

	    }
	  } 

	  // also create the reference for storage in the "Done" table
	  outFrame = MFrequency::Ref(theFrame, MeasFrame(theFieldDir, mObsPos, theObsTime));
	  

	  for(Int i=0; i<oldNUM_CHAN; i++){
	    transNewXin[i] = freqTrans(newXin[i]).get(unit).getValue();
	    transCHAN_WIDTH[i] = freqTrans(newXin[i] +
					   oldCHAN_WIDTH[i]/2.).get(unit).getValue()
	      - freqTrans(newXin[i] -
                          oldCHAN_WIDTH[i]/2.).get(unit).getValue(); // eliminate possible offsets
	    transRESOLUTION[i] = freqTrans(newXin[i] +
                                           oldRESOLUTION[i]/2.).get(unit).getValue() 
	      - freqTrans(newXin[i]
                          - oldRESOLUTION[i] / 2.0).get(unit).getValue(); // eliminate possible offsets
	  }
	  transREF_FREQUENCY = freqTrans(oldREF_FREQUENCY);
	  transTOTAL_BANDWIDTH = fabs(transNewXin[oldNUM_CHAN-1] - transNewXin[0]) +
            fabs(transCHAN_WIDTH[oldNUM_CHAN-1])/2.  + fabs(transCHAN_WIDTH[0])/2.;

	  if(radVelSignificant){ // correct in addition for radial velocity
	    transNewXin  = radVelCorr.shiftFrequency(transNewXin);
	    transCHAN_WIDTH = radVelCorr.shiftFrequency(transCHAN_WIDTH); //shiftFrequency is a scaling, so chan widths scale as well
	    transRESOLUTION = radVelCorr.shiftFrequency(transRESOLUTION);
	    Vector<Double> tmpV(1);
	    tmpV(0) = transREF_FREQUENCY.get("Hz").getValue();
	    transREF_FREQUENCY.set( MVFrequency( radVelCorr.shiftFrequency(tmpV)(0) ) );
	    tmpV(0) = transTOTAL_BANDWIDTH;
	    transTOTAL_BANDWIDTH = radVelCorr.shiftFrequency(tmpV)(0);
	  }

	}
	else {
	  // just copy
	  transNewXin.assign(newXin);
	  transCHAN_WIDTH.assign(oldCHAN_WIDTH);
	  transRESOLUTION.assign(oldRESOLUTION);
	  transREF_FREQUENCY = oldREF_FREQUENCY;
	  transTOTAL_BANDWIDTH = oldTOTAL_BANDWIDTH;
	}

	// (reference frame transformation completed)


	// storage for values with complete freq trafo + regridding applied
	// (set to default values for the case of no regridding)
	Vector<Double> newXout;
	newXout.assign(transNewXin);
	Int newNUM_CHAN = oldNUM_CHAN;
	Vector<Double> newCHAN_WIDTH;
	newCHAN_WIDTH.assign(transCHAN_WIDTH);
	MFrequency newREF_FREQUENCY = transREF_FREQUENCY;
	Vector<Double> newRESOLUTION;
	newRESOLUTION.assign(transRESOLUTION);
	Double newTOTAL_BANDWIDTH = transTOTAL_BANDWIDTH;
	Vector<Double> newEFFECTIVE_BW;
	newEFFECTIVE_BW.assign(oldEFFECTIVE_BW);
	Int theMethod;

	// check if theSPWId was already handled
	Int iDone2 = -1;
	for (uInt i=0; i<oldSpwId.size(); i++){
	  if(oldSpwId[i]==theSPWId){
	    iDone2 = i;
	    break;
	  }
	}
	equivalentSpwFieldPair = iDone2;

	if(equivalentSpwFieldPair>=0 && !needTransform){ 
          // a transformation was not needed, 
          // i.e. the operation on this SPW is independent of the FIELD
	  // and (since equivalentSpwFieldPair>=0) this SPW was already processed
	  // so we can reuse a previous SpwFieldPair

	  // get the parameters from the "done table"
	  theSPWId = oldSpwId[equivalentSpwFieldPair];
	  // don't get the field id!
	  theDataDescId = newDataDescId[equivalentSpwFieldPair] + origNumDataDescs; 
	  transNewXin.assign(xin[equivalentSpwFieldPair]);
	  newXout.assign(xout[equivalentSpwFieldPair]);
	  theMethod = method[equivalentSpwFieldPair];
	  doRegrid = regrid[equivalentSpwFieldPair];

	}
	else {
	  // Determine if regridding is necessary and set the parameters (at
	  // the same time, determine if the transformation is non-linear. If
	  // so set trafoNonlin (further below).)
	  
	  String methodName;
	  
	  if(regridQuant=="" ||
	     (regridCenter<-1E30 &&  regridBandwidth <= 0. && regridChanWidth <= 1. 
	      && regridQuant=="chan")
	     ){
	    // No regridding will take place.
	    // Set the interpol methods to some dummy value
	    theMethod = (Int) InterpolateArray1D<Double,Complex>::linear;
	    methodName = "linear";
	    message = " output frame = " + MFrequency::showType(theFrame) + " (pure transformation of the channel definition)";
	    // cout <<  regridQuant << " " << regridCenter << " " << regridBandwidth << " " << regridChanWidth << endl;
	  }
	  else { // a regrid quantity was chosen
	    // determine interpolation method (this is common for all possible
	    // values of regridQuant)
	    String meth=regridInterpMeth;
	    meth.downcase();
	    if(meth.contains("nearest")){
	      theMethod = (Int) InterpolateArray1D<Double,Complex>::nearestNeighbour;
	      methodName = "nearestNeighbour";
	    }
	    else if(meth.contains("splin")){
	      theMethod = (Int) InterpolateArray1D<Double,Complex>::spline;
	      methodName = "spline";
	    }	    
	    else if(meth.contains("cub")){
	      theMethod = (Int) InterpolateArray1D<Double,Complex>::cubic;
	      methodName = "cubic spline";
	    }
	    else if(meth.contains("fft")){
	      // check if input grid is equidistant in frequency
	      Bool isEquidistant = true;
	      Double sep = fabs(transCHAN_WIDTH(0));
	      for(uInt i=1; i<transCHAN_WIDTH.size(); i++){
		if((fabs(transCHAN_WIDTH(i)-transCHAN_WIDTH(i-1))>0.1)
		   || fabs(fabs(transNewXin(i)-transNewXin(i-1))-sep)>0.1 ){
		  isEquidistant = false;
		  break;
		}
		sep = fabs(transNewXin(i)-transNewXin(i-1));
	      }

	      if(isEquidistant){
		theMethod = (Int) useFFTShift;
		methodName = "fftshift";
	      }
	      else{
		theMethod = (Int) useLinIntThenFFTShift;
		methodName = "fftshift (preceeded by a linear transform to make grid equidistant)";
	      }
	      // for this method to work, the output grid needs to be equidistant in frequency
	      if(!(regridQuant=="freq" || regridQuant=="vrad" 
		   || (regridQuant=="chan" && isEquidistant) // (chan equidistance depends on input grid) 
		   )){
		os << LogIO::SEVERE
		   << "Parameter \"interpolation\" value \"" << meth << "\" requires an output grid equidistant in frequency."
		   << endl << "Cannot proceed." 
		   << LogIO::POST;
		return false;
	      }
	    }
	    else {
	      if(!meth.contains("linear") && meth!=""){
		os << LogIO::WARN
                   << "Parameter \"interpolation\" value \"" << meth << "\" is invalid." 
		   << LogIO::POST;
		return false;
	      }
	      theMethod = (Int) InterpolateArray1D<Double,Complex>::linear;
	      methodName = "linear";
	    }
	    
	    Vector<Double> newChanLoBound; 
	    Vector<Double> newChanHiBound;
	    
	    if(!regridChanBounds(newChanLoBound, 
				 newChanHiBound,
				 regridCenter,  
				 regridBandwidth, 
				 regridChanWidth, 
				 regridVeloRestfrq,
				 regridQuant,
				 transNewXin, 
				 transCHAN_WIDTH,
				 message,
				 centerIsStart,
				 startIsEnd,
				 nchan,
				 width,
				 start
				 )
	       ){ // there was an error
	      os << LogIO::SEVERE << message << LogIO::POST;
	      throw(AipsError("Regridding failed."));
	      //return false;
	    }
	    
	    // we have a useful set of channel boundaries
	    newNUM_CHAN = newChanLoBound.size();
	    
	    if(theMethod==(Int) useFFTShift
	       && newNUM_CHAN != oldNUM_CHAN){ // need to precede by lin. interpol. after all
	      theMethod = (Int) useLinIntThenFFTShift;
	    }

	    message = "input frame = " + MFrequency::showType(theOldRefFrame) 
	      + ", output frame = " + MFrequency::showType(theFrame)
              + "\n" + message + " Interpolation Method = " + methodName;
	    
	    // complete the calculation of the new spectral window parameters
	    // from newNUM_CHAN, newChanLoBound, and newChanHiBound 
	    newXout.resize(newNUM_CHAN);
	    newCHAN_WIDTH.resize(newNUM_CHAN);
	    newRESOLUTION.resize(newNUM_CHAN);
 	    newEFFECTIVE_BW.resize(newNUM_CHAN);
	    for(Int i=0; i<newNUM_CHAN; i++){
	      newXout[i] = (newChanLoBound[i]+newChanHiBound[i])/2.;
	      newCHAN_WIDTH[i] = newChanHiBound[i]-newChanLoBound[i];
	      newRESOLUTION[i] = newCHAN_WIDTH[i]; // to be revisited
	      newEFFECTIVE_BW[i] = newCHAN_WIDTH[i]; // to be revisited
	    }
	    // set the reference frequency to the central frequency of the first channel,
	    // keeping the already changed frame
	    MVFrequency mvf(newXout[0]);
	    newREF_FREQUENCY.set(mvf);
	    
	    // trivial definition of the bandwidth (taking into account possibility of descending freqs)
	    newTOTAL_BANDWIDTH = max(newChanHiBound[newNUM_CHAN-1], newChanHiBound[0])
	      -min(newChanLoBound[0],newChanLoBound[newNUM_CHAN-1]);

// 	    // effective bandwidth needs to be interpolated in quadrature
// 	    Vector<Double> newEffBWSquared(newNUM_CHAN);
// 	    Vector<Double> oldEffBWSquared(oldEFFECTIVE_BW);
// 	    for(Int i=0; i<oldNUM_CHAN; i++){
// 	      oldEffBWSquared[i] *= oldEffBWSquared[i];
// 	    }
// 	    InterpolateArray1D<Double, Double>::interpolate(newEffBWSquared, newXout,
//                                                             transNewXin,
//                                                             oldEffBWSquared,
//                                                             InterpolateArray1D<Double,Double>::linear);
// 	    for(uInt i=0; i<newNUM_CHAN; i++){
// 	      newEFFECTIVE_BW[i] = sqrt(newEffBWSquared[i]);
// 	    }

	    if(!allEQ(newXout, transNewXin)){ // grids are different
	      doRegrid = true;
	    }
	    
	  } // end if (regridQuant=="" ... 
	  
	  if(writeTables && (needTransform || doRegrid)){
            // new SPW amd DD table rows may need to be created
	    
	    // Create new row in the SPW table (with ID nextSPWId) by copying
	    // all information from row theSPWId
	    if(!spwtable.canAddRow()){
	      os << LogIO::WARN
                 << "Unable to add new row to SPECTRAL_WINDOW table. Cannot proceed with regridSpw ..." 
		 << LogIO::POST;
	      return false; 
	    }
	    
	    numNewSPWIds++;
	    nextSPWId++;
	    
	    // prepare parameter string for later entry into MS history and report to logger
	    {    
	      ostringstream param;
	      param << "Regridded spectral window "
		    << nextSPWId - origNumSPWs << " will be created for field " 
		    << theFieldId << " with parameters " << endl
                    << message << endl;
	      regridMessage += param.str(); // append
	      os << LogIO::NORMAL << param.str() << LogIO::POST;
	    }
	    
	    spwtable.addRow();
	    TableRow SPWRow(spwtable);
	    TableRecord spwRecord = SPWRow.get(theSPWId);
	    // TODO        Warn if the original channels are not contiguous or overlap!
	    SPWRow.putMatchingFields(nextSPWId, spwRecord);
	    
	    // and replacing the following columns with updated information:
	    // Store xout as new value of CHAN_FREQ.
	    chanFreqCol.put(nextSPWId, newXout);
	    numChanCol.put(nextSPWId, newNUM_CHAN);
	    chanWidthCol.put(nextSPWId,  newCHAN_WIDTH);
	    refFrequencyCol.put(nextSPWId, newREF_FREQUENCY.getValue());
	    if(theFrame==MFrequency::GEO){ // i.e. outframe was GEO or SOURCE
	      measFreqRefCol.put(nextSPWId, (Int)MFrequency::REST);
	    }
	    else{
	      measFreqRefCol.put(nextSPWId, (Int)theFrame);
	    }

	    totalBandwidthCol.put(nextSPWId, newTOTAL_BANDWIDTH);
	    effectiveBWCol.put(nextSPWId, newEFFECTIVE_BW);
	    resolutionCol.put(nextSPWId, newRESOLUTION);
	    
	    msModified = true;
	    //   Create a new row in the DATA_DESCRIPTION table and enter
	    //   nextSPWId in the SPW_ID column, copy the polarization id and
	    //   the flag_row content from the old DATA_DESCRIPTION row.
	    if(!ddtable.canAddRow()){
	      os << LogIO::WARN
                 << "Unable to add new row to DATA_DESCRIPTION table.  Cannot proceed with regridSpw ..." 
		 << LogIO::POST;
	      return false; 
	    }
	    numNewDataDesc++;
	    nextDataDescId++;
	    ddtable.addRow();
	    TableRow DDRow(ddtable);
	    TableRecord DDRecord = DDRow.get(theDataDescId);
	    DDRow.putMatchingFields(nextDataDescId, DDRecord);

            // anticipate the deletion of the original SPW table rows
	    SPWIdCol.put(nextDataDescId, nextSPWId - origNumSPWs); 
	    
	    // writing the value of nextDataDescId into the DATA_DESC_ID cell
	    // of the present MAIN table row.  will be done in the main regridSpw
	    // method
	    theDataDescId = nextDataDescId;
	  
	  } // end if(writeTables && (needTransform || doRegrid)

	} // end if there is a reusable SPW row

	if(writeTables && (needTransform || doRegrid)){
          // a new SOURCE table row has to be created

          // Add a row to the SOURCE table by copying the contents from the row
          // identified by the SOURCE_ID cell in the row theFieldId from the
          // FIELD table. Set the value of the cell SPECTRAL_WINDOW_ID in this
          // new row to the value nextSPWId.
	  if(nextSourceRow>=0){ // there is a source table
	    if(!p_sourcetable->canAddRow()){
	      os << LogIO::WARN
                 << "Unable to add new row to SOURCE table. Cannot proceed with regridSpw ..." 
		 << LogIO::POST;
	      return false; 
	    }
	    numNewSourceRows++;
	    nextSourceRow++;
	    // find the row in the SOURCE table which has
	    // SOURCE_ID==theSOURCEId and SPW_ID==theSPWId
	    Int theSOURCEId = FIELDsourceIdCol(theFieldId);
	    ScalarColumn<Int> SOURCEsourceIdCol = p_sourceCol->sourceId();
	    ScalarColumn<Int> SOURCESPWIdCol = p_sourceCol->spectralWindowId();
	    Int foundRow = -1;
	    for(int i=0; i<nextSourceRow; i++){
	      if(SOURCEsourceIdCol(i) == theSOURCEId && (SOURCESPWIdCol(i)==theSPWId || SOURCESPWIdCol(i)==-1)){
		foundRow = i;
		break;
	      }
	    }
	    if(foundRow<0){ 
	      os << LogIO::SEVERE << "Incoherent MS: Did not find SOURCE table entry with SOURCE_ID == " 
		 << theSOURCEId << " and  SPECTRAL_WINDOW_ID == " << theSPWId << endl
		 <<" even though the FIELD and the DATA_DESCRIPTION table entries for main table row " 
		 << mainTabRow << " refer to it." 
		 << LogIO::POST;
	      return false;
	    }
	    else { // found matching row
	      p_sourcetable->addRow();
	      TableRow SOURCERow(*p_sourcetable);
	      TableRecord SOURCERecord = SOURCERow.get(foundRow);
	      SOURCERow.putMatchingFields(nextSourceRow, SOURCERecord);

              // anticipate the deletion of the original SPW rows
	      SOURCESPWIdCol.put(nextSourceRow, nextSPWId - origNumSPWs);
	      newSourceIds.push_back(theSOURCEId);
	      newSourceSPWIds.push_back(nextSPWId - origNumSPWs);
	    }
	      
	  } // end if there is a source table
	}

	weightScale = newCHAN_WIDTH[0]/transCHAN_WIDTH[0]; // = deltaNuNew/deltaNuOld (both in outframe)

	//Put a new row into the "done" table.
	// (do all the push_backs in one place)
	oldSpwId.push_back(theSPWId);
	oldFieldId.push_back(theFieldId);

        // anticipate the deletion of the original DD rows
	newDataDescId.push_back(theDataDescId - origNumDataDescs);

	weightScaleV.push_back(weightScale);
	xold.push_back(newXin);
	xin.push_back(transNewXin);
	xout.push_back(newXout);
	method.push_back(theMethod);
	regrid.push_back(doRegrid);
	transform.push_back(needTransform);
	theFieldDirV.push_back(theFieldDir);
	mObsPosV.push_back(mObsPos);
	fromFrameTypeV.push_back(theOldRefFrame);
	outFrameV.push_back(outFrame);
	outRadVelV.push_back(outRadVel);

      } // end if(!alreadyDone)
      // reference frame transformation and regridding of channel definition completed
      ////////////////////

    } // end loop over main table

    if(writeTables && msModified){
      // delete the original rows in DD, SPW, and SOURCE if necessary
      if(numNewDataDesc>0){
	for(Int i=0; i<origNumDataDescs; i++){
	  ddtable.removeRow(0);
	}
      }
      if(numNewSPWIds>0){
	for(Int i=0; i<origNumSPWs; i++){
	  spwtable.removeRow(0);
	}
      }
      if(numNewSourceRows>0){
	ScalarColumn<Int> sourceIdCol = p_sourceCol->sourceId();
	ScalarColumn<Int> spwIdCol = p_sourceCol->spectralWindowId();
	// delete duplicate rows among the original Source rows
	vector<Bool> tBRemRows(origNumSourceRows, false);
	for(Int i=0; i<origNumSourceRows-1; i++){
	  if(tBRemRows[i]){
	    continue;
	  }
	  Int tSId = sourceIdCol(i);
	  Int tSpwId = spwIdCol(i);
	  for(Int j=i+1; j<origNumSourceRows; j++){
	    if(sourceIdCol(j)==tSId && spwIdCol(j)==tSpwId){
	      tBRemRows[j]=true;
	    }
	  }
	}
	for(Int i=origNumSourceRows-1; i>0; i--){
	  if(tBRemRows[i]){
	    p_sourcetable->removeRow(i);
	    origNumSourceRows--; 
	  }
	}

	// delete those original Source rows with Source/SPW ID pairs also present among the new ones
	uInt sIndex=0;
	for(Int i=0; i<origNumSourceRows; i++){
	  Bool sFound=false;
	  for(uInt j=0; j<newSourceIds.size(); j++){
	    if(sourceIdCol(sIndex)==newSourceIds[j] && spwIdCol(sIndex)==newSourceSPWIds[j]){
	      sFound=true;
	      break;
	    }
	  }
	  if(sFound){
	    p_sourcetable->removeRow(sIndex);
	  }
	  else{
	    sIndex++;
	  }
	}
      }

      // prepare parameter string for later entry into MS history
      ostringstream param;
      param << "Added " << numNewDataDesc
            << " new rows to the DATA_DESCRIPTION table and deleted "
            << origNumDataDescs << " old ones." << endl
            << "Added " << numNewSPWIds
            << " rows to the SPECTRAL_WINDOW table and deleted " << origNumSPWs 
            << " old ones." << endl
            << "Added " << numNewSourceRows
            << " rows to the SOURCE table and deleted " << origNumSourceRows
            << " old ones.";
      regridMessage += param.str() + "\n"; // append

      os << LogIO::NORMAL << param.str() <<  LogIO::POST;
    }

    delete p_sourceCol;

    return rval;
  }


  Bool SubMS::combineSpws(const Vector<Int>& spwids,
			  const Bool noModify,
			  Vector<Double>& newCHAN_FREQ,
			  Vector<Double>& newCHAN_WIDTH,
			  Bool verbose){


    using casacore::operator*;
    
    LogIO os(LogOrigin("SubMS", "combineSpws()"));
      
    // Analyse spwids

    if(spwids.nelements()==0){
      os << LogIO::WARN << "No SPWs selected for combination ..." <<  LogIO::POST;
      return true;
    }

    String tempNewName = ms_p.tableName()+".spwCombined"; // temporary name for the MS to store the result

    { // begin scope for MS related objects

      // find all existing spws, 
      MSSpectralWindow spwtable = ms_p.spectralWindow();
      Int origNumSPWs = spwtable.nrow();
      Int newSPWId = origNumSPWs;

      vector<Int> spwsToCombine;
      Vector<Bool> includeIt(origNumSPWs, false);

      // jagonzal: This covers for the case when we want to combine all the input SPWs
      if(spwids(0) == -1)
      {
    	  for(Int i=0; i<origNumSPWs; i++)
    	  {
    		  spwsToCombine.push_back(i);
    		  includeIt(i) = true;
    	  }
      }
      // jagonzal: Nominal case when we want to combine a sub-set of the input SPWs
      else
      {
    	  for(uInt i=0; i<spwids.nelements(); i++)
    	  {
    		  if(spwids(i)<origNumSPWs && spwids(i)>=0)
    		  {
    			  spwsToCombine.push_back(spwids(i));
    			  includeIt(spwids(i)) = true;
    		  }
    		  else
    		  {
    			  os << LogIO::SEVERE << "Invalid SPW ID selected for combination " << spwids(i)
    					  << "valid range is 0 - " << origNumSPWs-1 << ")" << LogIO::POST;
    			  return false;
    		  }
    	  }
      }
      // jagonzal: Marginal case when there is no actual SPW combination
      if(spwsToCombine.size()<=1)
      {
    	  if(verbose)
    	  {
    		  os << LogIO::NORMAL << "Less than two SPWs selected. No combination necessary." << LogIO::POST;
    	  }
    	  return true;
      }
      
      // sort the spwids
      std::sort(spwsToCombine.begin(), spwsToCombine.end());

      uInt nSpwsToCombine = spwsToCombine.size();

      // prepare access to the SPW table
      MSSpWindowColumns SPWColrs(spwtable);
      ScalarColumn<Int> numChanColr = SPWColrs.numChan(); 
      ArrayColumn<Double> chanFreqColr = SPWColrs.chanFreq(); 
      ArrayColumn<Double> chanWidthColr = SPWColrs.chanWidth(); 
      //    ArrayMeasColumn<MFrequency> chanFreqMeasColr = SPWColrs.chanFreqMeas();
      ScalarColumn<Int> measFreqRefColr = SPWColrs.measFreqRef();
      ArrayColumn<Double> effectiveBWColr = SPWColrs.effectiveBW();   
      ScalarColumn<Double> refFrequencyColr = SPWColrs.refFrequency(); 
      //    ScalarMeasColumn<MFrequency> refFrequencyMeasColr = SPWColrs.refFrequencyMeas(); 
      ArrayColumn<Double> resolutionColr = SPWColrs.resolution(); 
      ScalarColumn<Double> totalBandwidthColr = SPWColrs.totalBandwidth();

      // create a list of the spw ids sorted by first (lowest) channel frequency
      vector<Int> spwsSorted(nSpwsToCombine);
      Vector<Bool> isDescending(origNumSPWs, false);
      Bool negChanWidthWarned = false;
      {
	Double* firstFreq = new Double[nSpwsToCombine];
	uInt count = 0;
	for(uInt i=0; (Int)i<origNumSPWs; i++){
	  if(includeIt(i)){
	    Vector<Double> CHAN_FREQ(chanFreqColr(i));
	    // if frequencies are ascending, take the first channel, otherwise the last
	    uInt nCh = CHAN_FREQ.nelements();
	    if(CHAN_FREQ(0)<=CHAN_FREQ(nCh-1)){
	      firstFreq[count] = CHAN_FREQ(0);
	    }
	    else{
	      firstFreq[count] = CHAN_FREQ(nCh-1);
	      isDescending(i) = true;
	    }	   
	    count++;
	  }
	}
	Sort sort;
	sort.sortKey (firstFreq, TpDouble); // define sort key
	Vector<uInt> inx(nSpwsToCombine);
	sort.sort(inx, nSpwsToCombine);
	for (uInt i=0; i<nSpwsToCombine; i++) {
	  spwsSorted[i] = spwsToCombine[inx(i)];
	}
	delete[] firstFreq;
      }

      Int id0 = spwsSorted[0];

      uInt newNUM_CHAN = numChanColr(id0);
      newCHAN_FREQ.assign(chanFreqColr(id0));
      newCHAN_WIDTH.assign(chanWidthColr(id0));
      Bool newIsDescending = isDescending(id0);
      {
	Bool negativeWidths = false;
	for(uInt i=0; i<newNUM_CHAN; i++){
	  if(newCHAN_WIDTH(i) < 0.){
	    negativeWidths = true;
	    newCHAN_WIDTH(i) = -newCHAN_WIDTH(i);
	  }
	}
	if(negativeWidths && verbose){
	  os << LogIO::NORMAL
	     << " *** Encountered negative channel widths in SPECTRAL_WINDOW table."
	     << LogIO::POST;
	  negChanWidthWarned = true;
	}
      }

      if(newIsDescending){ // need to reverse the order for processing 
	Vector<Double> tempF, tempW;
	tempF.assign(newCHAN_FREQ);
	tempW.assign(newCHAN_WIDTH);
	for(uInt i=0; i<newNUM_CHAN; i++){
	  newCHAN_FREQ(i) = tempF(newNUM_CHAN-1-i);
	  newCHAN_WIDTH(i) = tempW(newNUM_CHAN-1-i);
	}
      }

      Vector<Double> newEFFECTIVE_BW(effectiveBWColr(id0));
      Double newREF_FREQUENCY(refFrequencyColr(id0));
      //MFrequency newREF_FREQUENCY = refFrequencyMeasColr(id0);
      Int newMEAS_FREQ_REF = measFreqRefColr(id0);
      Vector<Double> newRESOLUTION(resolutionColr(id0));
      Double newTOTAL_BANDWIDTH = totalBandwidthColr(id0);

      vector<Int> averageN; // for each new channel store the number of old channels to average over
      vector<vector<Int> > averageWhichSPW; // for each new channel store the
      // (old) SPWs to average over  
      vector<vector<Int> > averageWhichChan; // for each new channel store the
      // channel numbers to av. over
      vector<vector<Double> > averageChanFrac; // for each new channel store the
      // channel fraction for each old channel

      // initialise the averaging vectors
      for(uInt i=0; i<newNUM_CHAN; i++){
	averageN.push_back(1);
	vector<Int> tv; // just a temporary auxiliary vector
	tv.push_back(id0);
	averageWhichSPW.push_back(tv);
	if(newIsDescending){
	  tv[0] = newNUM_CHAN-1-i;
	}
	else{
	  tv[0] = i;
	}
	averageWhichChan.push_back(tv);
	vector<Double> tvd; // another one
	tvd.push_back(1.); 
	averageChanFrac.push_back(tvd);
      }

      if(verbose){
	os << LogIO::NORMAL << "Input SPWs sorted by first (lowest) channel frequency:" << LogIO::POST;

	ostringstream oss; // needed for iomanip functions
	oss << "   SPW " << std::setw(3) << id0 << ": " << std::setw(5) << newNUM_CHAN 
	    << " channels, first channel = " << std::setprecision(9) << std::setw(14) << std::scientific << newCHAN_FREQ(0) << " Hz";
	if(newNUM_CHAN>1){
	  oss << ", last channel = " << std::setprecision(9) << std::setw(14) << std::scientific << newCHAN_FREQ(newNUM_CHAN-1) << " Hz";
	}
	os << LogIO::NORMAL << oss.str() << LogIO::POST;
      }

      // loop over remaining given spws
      for(uInt i=1; i<nSpwsToCombine; i++){
	Int idi = spwsSorted[i];
      
	uInt newNUM_CHANi = numChanColr(idi);
	Vector<Double> newCHAN_FREQi(chanFreqColr(idi));
	Vector<Double> newCHAN_WIDTHi(chanWidthColr(idi));
	Bool newIsDescendingI = isDescending(idi);
	{
	  Bool negativeWidths = false;
	  for(uInt ii=0; ii<newNUM_CHANi; ii++){
	    if(newCHAN_WIDTHi(ii) < 0.){
	      negativeWidths = true;
	      newCHAN_WIDTHi(ii) = -newCHAN_WIDTHi(ii);
	    }
	  }
	  if(negativeWidths && !negChanWidthWarned && verbose){
	    os << LogIO::NORMAL
	       << " *** Encountered negative channel widths in SPECTRAL_WINDOW table."
	       << LogIO::POST;
	    negChanWidthWarned = true;
	  }
	}
	if(newIsDescendingI){ // need to reverse the order for processing 
	  Vector<Double> tempF, tempW;
	  tempF.assign(newCHAN_FREQi);
	  tempW.assign(newCHAN_WIDTHi);
	  for(uInt ii=0; ii<newNUM_CHANi; ii++){
	    newCHAN_FREQi(ii) = tempF(newNUM_CHANi-1-ii);
	    newCHAN_WIDTHi(ii) = tempW(newNUM_CHANi-1-ii);
	  }
	}

	Vector<Double> newEFFECTIVE_BWi(effectiveBWColr(idi));
	//Double newREF_FREQUENCYi(refFrequencyColr(idi));
	//MFrequency newREF_FREQUENCYi = refFrequencyMeasColr(idi);
	Int newMEAS_FREQ_REFi = measFreqRefColr(idi);
	Vector<Double> newRESOLUTIONi(resolutionColr(idi));
	//Double newTOTAL_BANDWIDTHi = totalBandwidthColr(idi);

	if(verbose){
	  ostringstream oss; // needed for iomanip functions
	  oss << "   SPW " << std::setw(3) << idi << ": " << std::setw(5) << newNUM_CHANi 
	      << " channels, first channel = " << std::setprecision(9) << std::setw(14) << std::scientific << newCHAN_FREQi(0) << " Hz";
	  if(newNUM_CHANi>1){
	    oss << ", last channel = " << std::setprecision(9) << std::setw(14) << std::scientific << newCHAN_FREQi(newNUM_CHANi-1) << " Hz";
	  }
	  os << LogIO::NORMAL << oss.str() << LogIO::POST;
	}

	vector<Double> mergedChanFreq;
	vector<Double> mergedChanWidth;
	vector<Double> mergedEffBW;
	vector<Double> mergedRes;
	vector<Int> mergedAverageN;
	vector<vector<Int> > mergedAverageWhichSPW;
	vector<vector<Int> > mergedAverageWhichChan;
	vector<vector<Double> > mergedAverageChanFrac;

	// check for compatibility
	if(newMEAS_FREQ_REFi != newMEAS_FREQ_REF){
	  os << LogIO::WARN
	     << "SPW " << idi << " cannot be combined with SPW " << id0 << ". Non-matching ref. frame."
	     << LogIO::POST;
	  return false; 
	}

	// append or prepend spw to new spw
	// overlap at all?
	if(newCHAN_FREQ(newNUM_CHAN-1) + newCHAN_WIDTH(newNUM_CHAN-1)/2. 
	   < newCHAN_FREQi(0) - newCHAN_WIDTHi(0)/2.) {
	  // no overlap, and need to append
	  for(uInt j=0; j<newNUM_CHAN; j++){
	    mergedChanFreq.push_back(newCHAN_FREQ(j));
	    mergedChanWidth.push_back(newCHAN_WIDTH(j));
	    mergedEffBW.push_back(newEFFECTIVE_BW(j));
	    mergedRes.push_back(newRESOLUTION(j));
	    mergedAverageN.push_back(averageN[j]);
	    mergedAverageWhichSPW.push_back(averageWhichSPW[j]);
	    mergedAverageWhichChan.push_back(averageWhichChan[j]);
	    mergedAverageChanFrac.push_back(averageChanFrac[j]);
	  }
	  vector<Int> tv;
	  tv.push_back(idi); // origin is spw idi
	  vector<Int> tv2;
	  tv2.push_back(0);
	  vector<Double> tvd;
	  tvd.push_back(1.); // fraction is 1.
	  for(uInt j=0; j<newNUM_CHANi; j++){
	    mergedChanFreq.push_back(newCHAN_FREQi(j));
	    mergedChanWidth.push_back(newCHAN_WIDTHi(j));
	    mergedEffBW.push_back(newEFFECTIVE_BWi(j));
	    mergedRes.push_back(newRESOLUTIONi(j));
	    mergedAverageN.push_back(1); // so far only one channel
	    mergedAverageWhichSPW.push_back(tv);
	    if(newIsDescendingI){
	      tv2[0] = newNUM_CHANi-1-j;
	    }
	    else{
	      tv2[0] = j;
	    }
	    mergedAverageWhichChan.push_back(tv2); // channel number is j
	    mergedAverageChanFrac.push_back(tvd);
	  }
	}
	else if( newCHAN_FREQ(0) - newCHAN_WIDTH(0)/2. 
		 > newCHAN_FREQi(newNUM_CHANi-1) + newCHAN_WIDTHi(newNUM_CHANi-1)/2.){ 
	  // no overlap, need to prepend
	  vector<Int> tv;
	  tv.push_back(idi); // origin is spw idi
	  vector<Int> tv2;
	  tv2.push_back(0);
	  vector<Double> tvd;
	  tvd.push_back(1.); // fraction is 1.
	  for(uInt j=0; j<newNUM_CHANi; j++){
	    mergedChanFreq.push_back(newCHAN_FREQi(j));
	    mergedChanWidth.push_back(newCHAN_WIDTHi(j));
	    mergedEffBW.push_back(newEFFECTIVE_BWi(j));
	    mergedRes.push_back(newRESOLUTIONi(j));
	    mergedAverageN.push_back(1); // so far only one channel
	    mergedAverageWhichSPW.push_back(tv);
	    if(newIsDescendingI){
	      tv2[0] = newNUM_CHANi-1-j;
	    }
	    else{
	      tv2[0] = j;
	    }
	    mergedAverageWhichChan.push_back(tv2); // channel number is j
	    mergedAverageChanFrac.push_back(tvd);
	  }
	  for(uInt j=0; j<newNUM_CHAN; j++){
	    mergedChanFreq.push_back(newCHAN_FREQ(j));
	    mergedChanWidth.push_back(newCHAN_WIDTH(j));
	    mergedEffBW.push_back(newEFFECTIVE_BW(j));
	    mergedRes.push_back(newRESOLUTION(j));
	    mergedAverageN.push_back(averageN[j]);
	    mergedAverageWhichSPW.push_back(averageWhichSPW[j]);
	    mergedAverageWhichChan.push_back(averageWhichChan[j]);
	    mergedAverageChanFrac.push_back(averageChanFrac[j]);
	  }
	}
	else{ // there is overlap
	  Int id0StartChan = 0;
	  if(newCHAN_FREQi(0) - newCHAN_WIDTHi(0)/2. < 
	     newCHAN_FREQ(newNUM_CHAN-1) - newCHAN_WIDTH(newNUM_CHAN-1)/2.){
	    // spw idi starts before spw id0

	    // some utilities for the averaging info
	    vector<Int> tv; // temporary vector
	    tv.push_back(idi); // origin is spw idi
	    vector<Int> tv2;
	    tv2.push_back(0);
	    vector<Double> tvd;
	    tvd.push_back(1.); // fraction is 1.

	    // find the first overlapping channel and prepend non-overlapping channels
	    Double ubound0 = newCHAN_FREQ(0) + newCHAN_WIDTH(0)/2.;
	    Double lbound0 = newCHAN_FREQ(0) - newCHAN_WIDTH(0)/2.;
	    Double uboundk = 0.;
	    Double lboundk = 0.;	      
	    uInt k;
	    for(k=0; k<newNUM_CHANi; k++){
	      uboundk = newCHAN_FREQi(k) + newCHAN_WIDTHi(k)/2.;
	      lboundk = newCHAN_FREQi(k) - newCHAN_WIDTHi(k)/2.;	      
	      if(lbound0 < uboundk){
		break;
	      }
	      mergedChanFreq.push_back(newCHAN_FREQi(k));
	      mergedChanWidth.push_back(newCHAN_WIDTHi(k));
	      mergedEffBW.push_back(newEFFECTIVE_BWi(k));
	      mergedRes.push_back(newRESOLUTIONi(k));
	      mergedAverageN.push_back(1); // so far only one channel
	      mergedAverageWhichSPW.push_back(tv);
	      if(newIsDescendingI){
		tv2[0] = newNUM_CHANi-1-k;
	      }
	      else{
		tv2[0] = k;
	      }
	      mergedAverageWhichChan.push_back(tv2); // channel number is k
	      mergedAverageChanFrac.push_back(tvd);	    
	    }
	    // k's the one
	    if(lbound0 < uboundk && lboundk < lbound0){ // actual overlap, need to merge channel k with channel 0
	      Double newWidth = ubound0 - lboundk;
	      Double newCenter = lboundk + newWidth/2.;
	      mergedChanFreq.push_back(newCenter);
	      mergedChanWidth.push_back(newWidth);
	      mergedEffBW.push_back(newWidth); 
	      mergedRes.push_back(newWidth); 
	      mergedAverageN.push_back(averageN[0]+1); // one more channel contributes
	      // channel k is from spw idi
	      if(newIsDescendingI){
		tv2[0] = newNUM_CHANi-1-k;
	      }
	      else{
		tv2[0] = k;
	      }
	      for(int j=0; j<averageN[0]; j++){
		tv.push_back(averageWhichSPW[0][j]); // additional contributors
		tv2.push_back(averageWhichChan[0][j]); // channel 0 from spw id0
		tvd.push_back(averageChanFrac[0][j]);
	      }
	      mergedAverageWhichSPW.push_back(tv);
	      mergedAverageWhichChan.push_back(tv2); 
	      mergedAverageChanFrac.push_back(tvd);
	      id0StartChan = 1;
	    }
	  }
	  // now move along SPW id0 and merge until end of id0 is reached, then just copy
	  for(uInt j=id0StartChan; j<newNUM_CHAN; j++){
	    mergedChanFreq.push_back(newCHAN_FREQ(j));
	    mergedChanWidth.push_back(newCHAN_WIDTH(j));
	    mergedEffBW.push_back(newEFFECTIVE_BW(j));
	    mergedRes.push_back(newRESOLUTION(j));
	    for(uInt k=0; k<newNUM_CHANi; k++){
	      Double overlap_frac = 0.;
	      // does channel j in spw id0 overlap with channel k in spw idi?
	      Double uboundj = newCHAN_FREQ(j) + newCHAN_WIDTH(j)/2.;
	      Double uboundk = newCHAN_FREQi(k) + newCHAN_WIDTHi(k)/2.;
	      Double lboundj = newCHAN_FREQ(j) - newCHAN_WIDTH(j)/2.;
	      Double lboundk = newCHAN_FREQi(k) - newCHAN_WIDTHi(k)/2.;
	      // determine fraction 
	      if(lboundj <= lboundk && uboundk <= uboundj){ // chan k is completely covered by chan j
		overlap_frac = 1.;
		//cout << "j " << j << " k " << k << " case 1" << endl;
		//cout << "overlap " << overlap_frac << endl;
	      }
	      else if(lboundk <= lboundj && uboundj <= uboundk){ // chan j is completely covered by chan k 
		overlap_frac = newCHAN_WIDTH(j)/newCHAN_WIDTHi(k);
		//cout << "j " << j << " k " << k << " case 2" << endl;
		//cout << "overlap " << overlap_frac << endl;
	      }
	      else if(lboundj < lboundk && lboundk < uboundj && uboundj < uboundk){ // lower end of k is overlapping with j
		overlap_frac = (uboundj - lboundk)/newCHAN_WIDTHi(k);
		//cout << "j " << j << " k " << k << " case 3" << endl;
		//cout << "overlap " << overlap_frac << endl;
	      }
	      else if(lboundk < lboundj && lboundj < uboundk && lboundj < uboundk){ // upper end of k is overlapping with j 
		overlap_frac = (uboundk - lboundj)/newCHAN_WIDTHi(k);
		//cout << "j " << j << " k " << k << " case 4" << endl;
		//cout << "overlap " << overlap_frac << endl;
	      }
	      if(overlap_frac > 0.){ // update averaging info
		averageN[j] += 1;
		averageWhichSPW[j].push_back(idi);
		if(newIsDescendingI){
		  averageWhichChan[j].push_back(newNUM_CHANi-1-k);
		}
		else{
		  averageWhichChan[j].push_back(k);
		}
		averageChanFrac[j].push_back(overlap_frac);
	      }
	    } // end loop over spw idi
	    // append this channel with updated averaging info
	    mergedAverageN.push_back(averageN[j]);
	    mergedAverageWhichSPW.push_back(averageWhichSPW[j]);
	    mergedAverageWhichChan.push_back(averageWhichChan[j]);
	    mergedAverageChanFrac.push_back(averageChanFrac[j]);  
	  } // end loop over spw id0
	  if(newCHAN_FREQ(newNUM_CHAN-1) + newCHAN_WIDTH(newNUM_CHAN-1)/2.
	     < newCHAN_FREQi(newNUM_CHANi-1) + newCHAN_WIDTHi(newNUM_CHANi-1)/2.){// spw idi still continues!
	    // find the last overlapping channel
	    Int j = newNUM_CHAN-1;
	    Double uboundj = newCHAN_FREQ(j) + newCHAN_WIDTH(j)/2.;
	    Double lboundj = newCHAN_FREQ(j) - newCHAN_WIDTH(j)/2.;
	    Double uboundk = 0;
	    Double lboundk = 0;	      
	    Int k;
	    for(k=newNUM_CHANi-1; k>=0; k--){
	      uboundk = newCHAN_FREQi(k) + newCHAN_WIDTHi(k)/2.;
	      lboundk = newCHAN_FREQi(k) - newCHAN_WIDTHi(k)/2.;	      
	      if(lboundk <= uboundj){
		break;
	      }
	    }
	    // k's the one 
 	    if(lboundk < uboundj && uboundj < uboundk ){ // actual overlap
	      Double overlap_frac = (uboundj - lboundk)/newCHAN_WIDTHi(k);
	      if(overlap_frac>0.01){ // merge channel k completely with channel j 
		Double newWidth = uboundk - lboundj;
		Double newCenter = (lboundj+uboundk)/2.;
		mergedChanFreq[j] =  newCenter;
		mergedChanWidth[j] = newWidth;
		mergedEffBW[j] = newWidth; 
		mergedRes[j] = newWidth; 
		mergedAverageChanFrac[j][mergedAverageN[j]-1] = 1.; 
	      }
	      else{ // create separate, (slightly) more narrow channel
		Double newWidth = uboundk - uboundj;
		Double newCenter = (uboundj+uboundk)/2.;
		vector<Int> tv;
		tv.push_back(idi); // origin is spw idi
		vector<Int> tv2;
		tv2.push_back(0);
		vector<Double> tvd;
		tvd.push_back(1.); // fraction is 1.
		mergedChanFreq.push_back(newCenter);
		mergedChanWidth.push_back(newWidth);
		mergedEffBW.push_back(newWidth);
		mergedRes.push_back(newWidth);
		mergedAverageN.push_back(1); // so far only one channel
		mergedAverageWhichSPW.push_back(tv);
		if(newIsDescendingI){
		  tv2[0] = newNUM_CHANi-1-k;
		}
		else{
		  tv2[0] = k;
		}
		mergedAverageWhichChan.push_back(tv2); // channel number is k
		mergedAverageChanFrac.push_back(tvd);
	      }
	      k++; // start appending remaining channels after k
 	    }
	    // append the remaining channels
	    vector<Int> tv;
	    tv.push_back(idi); // origin is spw idi
	    vector<Int> tv2;
	    tv2.push_back(0);
	    vector<Double> tvd;
	    tvd.push_back(1.); // fraction is 1.
	    for(uInt m=k; m<newNUM_CHANi; m++){
	      mergedChanFreq.push_back(newCHAN_FREQi(m));
	      mergedChanWidth.push_back(newCHAN_WIDTHi(m));
	      mergedEffBW.push_back(newEFFECTIVE_BWi(m));
	      mergedRes.push_back(newRESOLUTIONi(m));
	      mergedAverageN.push_back(1); // so far only one channel
	      mergedAverageWhichSPW.push_back(tv);
		if(newIsDescendingI){
		  tv2[0] = newNUM_CHANi-1-m;
		}
		else{
		  tv2[0] = m;
		}
	      mergedAverageWhichChan.push_back(tv2); // channel number is m
	      mergedAverageChanFrac.push_back(tvd);
	    }
	  } // end if spw idi still continues
	} // end if there is overlap    


	newNUM_CHAN = mergedChanFreq.size();
	newCHAN_FREQ.assign(Vector<Double>(mergedChanFreq));
	newCHAN_WIDTH.assign(Vector<Double>(mergedChanWidth));
	newEFFECTIVE_BW.assign(Vector<Double>(mergedEffBW));
	newREF_FREQUENCY = newCHAN_FREQ(0); 
	newTOTAL_BANDWIDTH = fabs(newCHAN_FREQ(newNUM_CHAN-1) - newCHAN_FREQ(0))
	  + fabs(newCHAN_WIDTH(newNUM_CHAN-1)/2.) + fabs(newCHAN_WIDTH(0)/2.);
	newRESOLUTION.assign(Vector<Double>(mergedRes));
	averageN = mergedAverageN;
	averageWhichSPW = mergedAverageWhichSPW;
	averageWhichChan = mergedAverageWhichChan;
	averageChanFrac = mergedAverageChanFrac;

      } // end loop over SPWs

      // print channel fractions for debugging
//       vector< vector<bool> > wasprinted;
//       for(uInt i=0; i<newNUM_CHAN; i++){
// 	vector<bool> tboolv;
// 	cout << "i freq width " << i << " " << newCHAN_FREQ(i) << " " << newCHAN_WIDTH(i) << endl;
// 	for(Int j=0; j<averageN[i]; j++){
// 	  cout << " i, j " << i << ", " << j << " averageWhichChan[i][j] " << averageWhichChan[i][j]
// 	       << " averageWhichSPW[i][j] " << averageWhichSPW[i][j] << endl;
// 	  cout << " averageChanFrac[i][j] " << averageChanFrac[i][j] << endl;
// 	  tboolv.push_back(false);
// 	}
// 	wasprinted.push_back(tboolv);
//       }
      
      
      if(noModify){ // newCHAN_FREQ and newCHAN_WIDTH have been determined now
	return true;
      }

      // now need write access
      MSSpWindowColumns SPWCols(spwtable);
      ScalarColumn<Int> numChanCol = SPWCols.numChan(); 
      ArrayColumn<Double> chanFreqCol = SPWCols.chanFreq(); 
      ArrayColumn<Double> chanWidthCol = SPWCols.chanWidth(); 
      //    ArrayMeasColumn<MFrequency> chanFreqMeasCol = SPWCols.chanFreqMeas();
      ScalarColumn<Int> measFreqRefCol = SPWCols.measFreqRef();
      ArrayColumn<Double> effectiveBWCol = SPWCols.effectiveBW();   
      ScalarColumn<Double> refFrequencyCol = SPWCols.refFrequency(); 
      //    ScalarMeasColumn<MFrequency> refFrequencyMeasCol = SPWCols.refFrequencyMeas(); 
      ArrayColumn<Double> resolutionCol = SPWCols.resolution(); 
      ScalarColumn<Double> totalBandwidthCol = SPWCols.totalBandwidth();

      if(verbose){
	os << LogIO::NORMAL << "Combined SPW will have " << newNUM_CHAN 
	   << " channels. May change in later regridding." << LogIO::POST;
      }

      // Create new row in the SPW table (with ID nextSPWId) by copying
      // all information from row theSPWId
      if(!spwtable.canAddRow()){
	os << LogIO::WARN
	   << "Unable to add new row to SPECTRAL_WINDOW table. Cannot proceed with spwCombine ..." 
	   << LogIO::POST;
	return false; 
      }

      TableRow SPWRow(spwtable);
      TableRecord spwRecord = SPWRow.get(id0);

      // write new spw to spw table (ID =  newSpwId)
      spwtable.addRow();
      SPWRow.putMatchingFields(newSPWId, spwRecord);

      chanFreqCol.put(newSPWId, newCHAN_FREQ);
      numChanCol.put(newSPWId, newNUM_CHAN);
      chanWidthCol.put(newSPWId,  newCHAN_WIDTH);
      refFrequencyCol.put(newSPWId, newREF_FREQUENCY);
      totalBandwidthCol.put(newSPWId, newTOTAL_BANDWIDTH);
      effectiveBWCol.put(newSPWId, newEFFECTIVE_BW);
      resolutionCol.put(newSPWId, newRESOLUTION);

      // delete unwanted spws and memorize the new ID of the new merged one.
      // (remember the IDs were sorted above)
      for(int i=nSpwsToCombine-1; i>=0; i--){ // remove highest row numbers first
	spwtable.removeRow(spwsToCombine[i]);
      }
      newSPWId -= nSpwsToCombine; 

      // other tables to correct: MAIN, FREQ_OFFSET, SYSCAL, FEED, DATA_DESCRIPTION, SOURCE

      // 1) SOURCE (an optional table)
      uInt numSourceRows = 0;
      MSSource* p_sourcetable = NULL;
      MSSourceColumns* p_sourceCol = NULL;
      if(Table::isReadable(ms_p.sourceTableName())){
	p_sourcetable = &(ms_p.source());
	p_sourceCol = new MSSourceColumns(*p_sourcetable);
	numSourceRows = p_sourcetable->nrow();
	ScalarColumn<Int> SOURCESPWIdCol = p_sourceCol->spectralWindowId();
	// loop over source table rows
	for(uInt i=0; i<numSourceRows; i++){
	  for(uInt j=0; j<nSpwsToCombine; j++){
	    // if spw id affected, replace by newSpwId
	    if(SOURCESPWIdCol(i) == spwsToCombine[j]){ // source row i is affected
	      SOURCESPWIdCol.put(i, newSPWId);
	    }
	  } // end for j
	}// end for i
      }
      else if(verbose){ // there is no source table
	os << LogIO::NORMAL << "Note: MS contains no SOURCE table ..." << LogIO::POST;
      }

      // 2) DATA_DESCRIPTION
      MSDataDescription ddtable = ms_p.dataDescription();
      uInt numDataDescs = ddtable.nrow();
      MSDataDescColumns DDCols(ddtable);
      ScalarColumn<Int> SPWIdCol = DDCols.spectralWindowId();
      ScalarColumn<Int> PolIdCol = DDCols.polarizationId();
      vector<uInt> affDDIds;  
      vector<Bool> DDRowsToDelete(numDataDescs, false);
      std::map<Int, Int> tempDDIndex; // store relation between old and new DD Ids
      std::map<Int, Int> DDtoSPWIndex; // store relation between old DD Ids and old SPW Ids 
      //  (only for affected SPW IDs)
      // loop over DD table rows
      for(uInt i=0; i<numDataDescs; i++){
	// if spw id affected, replace by newSpwId
	for(uInt j=0; j<nSpwsToCombine; j++){
	  // if spw id affected, replace by newSpwId
	  if(SPWIdCol(i) == spwsToCombine[j]){ // DD row i is affected
	    // correct the SPW Id in the DD table
	    SPWIdCol.put(i, newSPWId);
	    // memorize affected DD IDs in affDDIds
	    affDDIds.push_back(i);
	    // store relation between old DD Id and old SPW ID for later use in the modification of the MAIN table
	    DDtoSPWIndex.insert(std::pair<Int, Int>(i, spwsToCombine[j])); // note: this relation can be many-to-one  
	  }     
	}
      }
      // Find redundant DD IDs
      // loop over DD table rows
      for(uInt i=0; i<numDataDescs; i++){
	Bool affected = false;
	for(uInt j=0; j<affDDIds.size(); j++){
	  if(i == affDDIds[j] && !DDRowsToDelete[i]){
	    affected = true;
	    break;
	  }
	}
	if(!affected){
	  continue;
	}
	else { // i is an affected row
	  Int PolIDi = PolIdCol(i);
	  Int SpwIDi = SPWIdCol(i);
	  // loop over following DD table rows
	  for(uInt j=i+1; j<numDataDescs; j++){
	    // if row i and row j redundant?
	    if(PolIDi == PolIdCol(j) && SpwIDi == SPWIdCol(j)){
	      // mark for deletion
	      DDRowsToDelete[j] = true;
	      // fill map for DDrenumbering
	      tempDDIndex.insert(std::pair<Int, Int>(j, i));
	    }
	  }    
	} // end if affected 
      }
      // delete redundant DD rows
      Int removed = 0;
      for(uInt i=0; i<numDataDescs; i++){
	if(DDRowsToDelete[i]){
	  ddtable.removeRow(i-removed);
	  removed++;
	}
	else{ // this row is not deleted but changes its number by <removed> due to removal of others
        tempDDIndex.insert(std::pair<Int, Int>(i, i-removed));
	}
      }

      // 3) FEED  
      MSFeed feedtable = ms_p.feed();
      uInt numFeedRows = feedtable.nrow();
      MSFeedColumns feedCols(feedtable);
      ScalarColumn<Int> feedSPWIdCol = feedCols.spectralWindowId();
 
      // loop over FEED table rows
      for(uInt i=0; i<numFeedRows; i++){
	// if spw id affected, replace by newSpwId
	for(uint j=0; j<nSpwsToCombine; j++){
	  // if spw id affected, replace by newSpwId
	  if(feedSPWIdCol(i) == spwsToCombine[j]){ // feed row i is affected
	    feedSPWIdCol.put(i, newSPWId);
	  }     
	}
      }

      // TODO: (possibly, not clear if necessary) remove redundant FEED rows and propagate

      // 4) SYSCAL

      // note: syscal is optional

      if(!ms_p.sysCal().isNull()){
	MSSysCal sysCaltable = ms_p.sysCal();
	uInt numSysCalRows = sysCaltable.nrow();
	MSSysCalColumns sysCalCols(sysCaltable);
	ScalarColumn<Int> sysCalSPWIdCol = sysCalCols.spectralWindowId();
      
	// loop over SYSCAL table rows
	for(uInt i=0; i<numSysCalRows; i++){
	  // if spw id affected, replace by newSpwId
	  for(uInt j=0; j<nSpwsToCombine; j++){
	    // if spw id affected, replace by newSpwId
	    if(sysCalSPWIdCol(i) == spwsToCombine[j]){ // SysCal row i is affected
	      sysCalSPWIdCol.put(i, newSPWId);
	    }     
	  }
	}
      }

      // 5) FREQ_OFFSET

      // note: freq_offset is optional

      if(!ms_p.freqOffset().isNull()){
	MSFreqOffset freqOffsettable = ms_p.freqOffset();
	uInt numFreqOffsetRows = freqOffsettable.nrow();
	MSFreqOffsetColumns freqOffsetCols(freqOffsettable);
	ScalarColumn<Int> freqOffsetSPWIdCol = freqOffsetCols.spectralWindowId();
      
	// loop over FREQ_OFFSET table rows
	for(uInt i=0; i<numFreqOffsetRows; i++){
	  // if spw id affected, replace by newSpwId
	  for(uInt j=0; j<nSpwsToCombine; j++){
	    // if spw id affected, replace by newSpwId
	    if(freqOffsetSPWIdCol(i) == spwsToCombine[j]){ // FreqOffset row i is affected
	      freqOffsetSPWIdCol.put(i, newSPWId);
	    }     
	  }
	}
      }

      // 6) MAIN

      Table newMain(TableCopy::makeEmptyTable( tempNewName,
					       Record(),
					       (Table) ms_p,
					       Table::New,
					       Table::AipsrcEndian,
					       true, // replaceTSM 
					       true // noRows
					       )
		    );
      
      TableCopy::copySubTables(newMain, ms_p, false);
      
      MSMainColumns mainCols((MeasurementSet&)newMain);
      MSMainColumns oldMainCols(ms_p);
      
      uInt nMainTabRows = ms_p.nrow();
      
      // columns which depend on the number of frequency channels and may need to be combined:
      // DATA, FLOAT_DATA, CORRECTED_DATA, MODEL_DATA, LAG_DATA, SIGMA_SPECTRUM,
      // WEIGHT_SPECTRUM, FLAG, and FLAG_CATEGORY    
      ArrayColumn<Complex> CORRECTED_DATACol =  mainCols.correctedData();
      ArrayColumn<Complex> oldCORRECTED_DATACol = oldMainCols.correctedData();
      ArrayColumn<Complex>  DATACol =  mainCols.data();
      ArrayColumn<Complex>  oldDATACol = oldMainCols.data();
      ArrayColumn<Float> FLOAT_DATACol =  mainCols.floatData();
      ArrayColumn<Float> oldFLOAT_DATACol = oldMainCols.floatData();
      ArrayColumn<Complex> LAG_DATACol =  mainCols.lagData();
      ArrayColumn<Complex> oldLAG_DATACol = oldMainCols.lagData();
      ArrayColumn<Complex> MODEL_DATACol =  mainCols.modelData();
      ArrayColumn<Complex> oldMODEL_DATACol = oldMainCols.modelData();
      ArrayColumn<Float> SIGMA_SPECTRUMCol =  mainCols.sigmaSpectrum();
      ArrayColumn<Float> oldSIGMA_SPECTRUMCol = oldMainCols.sigmaSpectrum();
      ArrayColumn<Float> WEIGHT_SPECTRUMCol =  mainCols.weightSpectrum();
      ArrayColumn<Float> oldWEIGHT_SPECTRUMCol = oldMainCols.weightSpectrum();
      ArrayColumn<Bool> FLAGCol =  mainCols.flag();
      ArrayColumn<Bool> oldFLAGCol = oldMainCols.flag();
      ArrayColumn<Bool> FLAG_CATEGORYCol =  mainCols.flagCategory();
      ArrayColumn<Bool> oldFLAG_CATEGORYCol = oldMainCols.flagCategory();
      
      // columns which may be different for otherwise matching main table rows
      //  and need to be combined
      ScalarColumn<Bool> flagRowCol = oldMainCols.flagRow();

      // administrational columns needed from the main table
      ArrayColumn<Float> SIGMACol =  oldMainCols.sigma();
      ScalarColumn<Int> fieldCol = oldMainCols.fieldId();
      ScalarColumn<Int> DDIdCol = oldMainCols.dataDescId();
      ScalarColumn<Int> antenna1Col = oldMainCols.antenna1();
      ScalarColumn<Int> antenna2Col = oldMainCols.antenna2();
      ScalarColumn<Double> timeCol = oldMainCols.time(); 
      ScalarColumn<Double> intervalCol = oldMainCols.interval();
      ScalarColumn<Double> exposureCol = oldMainCols.exposure();
      ScalarMeasColumn<MEpoch> mainTimeMeasCol = oldMainCols.timeMeas();

      // arrays for composing the combined columns 
      // model them on the first affected row of the main table
      
      Matrix<Complex> newCorrectedData; 
      Matrix<Complex> newData;
      Matrix<Float> newFloatData;
      Matrix<Complex> newLagData;
      Matrix<Complex> newModelData;
      Matrix<Float> newSigmaSpectrum;
      Matrix<Float> newWeightSpectrum;
      Matrix<Bool> newFlag;
      Array<Bool> newFlagCategory; // has three dimensions
      Bool newFlagRow; 

      // create time-sorted index for main table access
      Vector<uInt> sortedI(nMainTabRows);
      {
	Vector<Double> mainTimesV = oldMainCols.time().getColumn();
	GenSortIndirect<Double>::sort(sortedI,mainTimesV);
      }

      // find the first row affected by the spw combination
      Int firstAffRow = 0;
      for(uInt mRow=0; mRow<nMainTabRows; mRow++){
	uInt sortedMRow = sortedI(mRow);
	if(DDtoSPWIndex.find(DDIdCol(sortedMRow)) != DDtoSPWIndex.end( )){
	  firstAffRow = sortedMRow;
	  break;
	}
      }
      
      // get the number of correlations from the
      // dimension of the first axis of the sigma column
      uInt nCorrelations = SIGMACol(firstAffRow).shape()(0); 
      
      IPosition newShape = IPosition(2, nCorrelations, newNUM_CHAN);
      
      Bool CORRECTED_DATAColIsOK = !CORRECTED_DATACol.isNull();
      Bool DATAColIsOK = !DATACol.isNull();
      Bool FLOAT_DATAColIsOK = !FLOAT_DATACol.isNull();
      Bool LAG_DATAColIsOK = !LAG_DATACol.isNull();
      Bool MODEL_DATAColIsOK = !MODEL_DATACol.isNull();
      Bool SIGMA_SPECTRUMColIsOK = !SIGMA_SPECTRUMCol.isNull();
      Bool WEIGHT_SPECTRUMColIsOK = !WEIGHT_SPECTRUMCol.isNull(); // rechecked further below
      Bool FLAGColIsOK = !FLAGCol.isNull();
      Bool FLAG_CATEGORYColIsOK = false; // to be set to the correct value further below
      
      // initialize arrays to store combined column data
      if(CORRECTED_DATAColIsOK){
	newCorrectedData.resize(newShape);
      }
      if(DATAColIsOK){
	newData.resize(newShape);
      }
      if(FLOAT_DATAColIsOK){
	newFloatData.resize(newShape);
      }
      if(LAG_DATAColIsOK){
	newLagData.resize(newShape); 
      }
      if(MODEL_DATAColIsOK){
	newModelData.resize(newShape);
      }
      if(SIGMA_SPECTRUMColIsOK){
	newSigmaSpectrum.resize(newShape);
      }
      if(WEIGHT_SPECTRUMColIsOK){
	if(oldWEIGHT_SPECTRUMCol.isDefined(firstAffRow)){ // required column but may be empty
	  newWeightSpectrum.resize(newShape);
	}
	else{
	  WEIGHT_SPECTRUMColIsOK = false;
	}
      }
      if(FLAGColIsOK){ // required but one never knows (there may be bugs elsewhere)
	newFlag.resize(newShape);
      }
      IPosition flagCatShape;
      uInt nCat  = 0;
      if(!FLAG_CATEGORYCol.isNull()){ 
	if(oldFLAG_CATEGORYCol.isDefined(firstAffRow)){ // required column but may be empty
	  FLAG_CATEGORYColIsOK = true;
	  flagCatShape = oldFLAG_CATEGORYCol.shape(firstAffRow);
	  nCat = flagCatShape(2); // the dimension of the third axis ==
	  // number of categories
	  newFlagCategory.resize(IPosition(3, nCorrelations, newNUM_CHAN, nCat));
	} 
      }
      
      /////////////////////////////////////////
 
      // Loop over main table rows
      uInt mainTabRowI = 0;
      uInt mainTabRow = sortedI(mainTabRowI);
      uInt newMainTabRow = 0;
      uInt nIncompleteCoverage = 0; // number of rows with incomplete SPW coverage

      Int prevNewMainTabRow = -1; // needed to reduce redundant warning output
      vector<Int> prevFailedAvChans;
      vector<Int> prevFailedAvCorrs;

      // prepare progress meter
      Float progress = 0.2;
      Float progressStep = 0.2;
      if(nMainTabRows>100000){
	progress = 0.1;
	progressStep = 0.1;
      }

      while(mainTabRowI<nMainTabRows &&
	    (mainTabRow = sortedI(mainTabRowI)) < nMainTabRows
	    ){
	
	// should row be combined with others, i.e. has SPW changed?
	// no -> just renumber DD ID (because of shrunk DD ID table)
	
	// yes-> find rows from the spws tobe combined with same timestamp, antennas and field
	//       merge these rows
	//       write merged row over first one, correcting DD ID at the same time
	//       set TIME to 0 in other merged rows
	//       reduce nMainTabRows accordingly
	
	// continue
	//
	// when finished, delete all rows with TIME = 0
	
	Double theTime = timeCol(mainTabRow);
	
	// row was already combined with a previous row?
	if(theTime == 0){
	  //	  cout << "skipping row with zero time " << mainTabRow << endl;
	  mainTabRowI++;
	  continue;
	}
	
	Int theDataDescId = DDIdCol(mainTabRow);
	
	// row affected by the spw combination? (uses the old DD numbering)
	if(DDtoSPWIndex.find(theDataDescId) != DDtoSPWIndex.end( )){
	  // find matching affected rows with same time stamp, antennas and field
	  Int theAntenna1 = antenna1Col(mainTabRow);
	  Int theAntenna2 = antenna2Col(mainTabRow);
	  Int theField = fieldCol(mainTabRow);
	  Double theInterval = intervalCol(mainTabRow);
	  Double toleratedTimeDiff = theInterval/10.;
	  Double theExposure = exposureCol(mainTabRow);
	  vector<Int> matchingRows;
	  matchingRows.push_back(mainTabRow);
	  vector<Int> matchingRowSPWIds;
	  matchingRowSPWIds.push_back(DDtoSPWIndex.at(theDataDescId));
	  std::map<Int, Int> SPWtoRowIndex;
	  SPWtoRowIndex.insert(std::pair<Int, Int>(matchingRowSPWIds[0], mainTabRow));

	  //	  cout << "theRow = " << mainTabRow << ", time = " << theTime << " DDID " << theDataDescId << endl;
	  
	  uInt nextRowI = mainTabRowI+1;
	  uInt nextRow;
	  //	  cout << "nextRow = " << nextRow << ", time diff  = " << timeCol(nextRow) - theTime << " DDID " << DDIdCol(nextRow) << endl;
	  while(nextRowI<nMainTabRows && 
		(nextRow = sortedI(nextRowI))<nMainTabRows && // assignment!
		(timeCol(nextRow) - theTime)< toleratedTimeDiff &&
		matchingRows.size() < nSpwsToCombine // there should be one matching row per SPW
		){

        if( DDtoSPWIndex.find(DDIdCol(nextRow)) == DDtoSPWIndex.end( ) ||
	       antenna1Col(nextRow) != theAntenna1 ||
	       antenna2Col(nextRow) != theAntenna2 ||
	       fieldCol(nextRow) != theField ){ // not a matching row
	      nextRowI++;
	      continue;
	    }
	    // check that the intervals are the same
	    if(fabs(intervalCol(nextRow)-theInterval) > 1E-5){
	      os << LogIO::WARN << "Error: for time " <<  MVTime(theTime/C::day).string(MVTime::DMY,7) << ", baseline (" << theAntenna1 << ", "
		 << theAntenna2 << "), field "<< theField << ", DataDescID " << DDIdCol(mainTabRow)
		 << " found matching row with DataDescID " << DDIdCol(nextRow) << endl
		 << " but the two rows have different intervals: " << theInterval
		 << " vs. " << intervalCol(nextRow)
		 << LogIO::POST;
	      if(!flagRowCol(nextRow)){
		os << LogIO::SEVERE << "You need to flag row " << nextRow << " to proceed." << LogIO::POST;
		return false;
	      }
	      else{
		os << LogIO::WARN << "Fortunately, row " << nextRow << " is flagged so this problem will be ignored." << LogIO::POST;
	      }		
	    }
	    // check that the exposures are the same
	    if(fabs(exposureCol(nextRow)-theExposure) > 1E-5){
	      os << LogIO::WARN << "Error: for time " <<  MVTime(theTime/C::day).string(MVTime::DMY,7) << ", baseline (" << theAntenna1 << ", "
		 << theAntenna2 << "), field "<< theField << ", DataDescID " << DDIdCol(mainTabRow)
		 << " found matching row with DataDescID " << DDIdCol(nextRow) << endl
		 << " but the two rows have different exposures: " << theExposure
		 << " vs. " << exposureCol(nextRow)
		 << LogIO::POST;
	      if(!flagRowCol(nextRow)){
		os << LogIO::SEVERE << "You need to flag row " << nextRow << " to proceed." << LogIO::POST;
		return false;
	      }
	      else{
		os << LogIO::WARN << "Fortunately row " << nextRow << " is flagged so this problem will be ignored." << LogIO::POST;
	      }		
	    }
	    // found a matching row
	    Int theSPWId = DDtoSPWIndex.at(DDIdCol(nextRow));
        if(SPWtoRowIndex.find(theSPWId) != SPWtoRowIndex.end( )){ // there should be a one-to-one relation: SPW <-> matching row
	      os << LogIO::WARN << "Error: for time " << MVTime(theTime/C::day).string(MVTime::DMY,7) << ", baseline (" << theAntenna1 << ","
		 << theAntenna2 << "), field "<< theField << " found more than one row for SPW "
		 << theSPWId << LogIO::POST;
	      if(!flagRowCol(nextRow)){
		os << LogIO::SEVERE << "You need to flag row " << nextRow << " to proceed." << LogIO::POST;
		return false;
	      }
	      else{
		os << LogIO::WARN << "Fortunately, row " << nextRow << " is flagged so this problem will be ignored." << LogIO::POST;
	      }		
	    }
	    else{ // this SPW not yet covered, memorize SPWId, row number, and relation
	      matchingRowSPWIds.push_back(theSPWId);
	      matchingRows.push_back(nextRow);
	      SPWtoRowIndex.insert(std::pair<Int, Int>(theSPWId, nextRow));
	      // cout << "matching nextRow = " << nextRow << ", time = " << timeCol(nextRow) << " DDID " << DDIdCol(nextRow) << endl;
	    }
	    nextRowI++;
	  } // end while nextRowI ...
	  
	  // now we have a set of matching rows
	  uInt nMatchingRows = matchingRows.size();
	  
	  if(nMatchingRows < nSpwsToCombine){
	    if(nIncompleteCoverage==0){
	      os << LogIO::WARN << "Incomplete coverage of combined SPW starting at timestamp " 
		 <<  MVTime(timeCol(mainTabRow)/C::day).string(MVTime::DMY,7)
		 << ", baseline ( " << theAntenna1 << ", " << theAntenna2 << " )" << endl
		 << "In this and further affected rows, the data arrays will be padded with zeros and corresponding channels flagged." <<  LogIO::POST;
	    }
	    nIncompleteCoverage++;
	  }

	  // reset arrays and prepare input data matrices
	  
	  if(CORRECTED_DATAColIsOK){
	    newCorrectedData.set(0);
	  }
	  if(DATAColIsOK){
	    newData.set(0);
	  }
	  if(FLOAT_DATAColIsOK){
	    newFloatData.set(0);
	  }
	  if(LAG_DATAColIsOK){
	    newLagData.set(0);
	  }
	  if(MODEL_DATAColIsOK){
	    newModelData.set(0);
	  }
	  if(SIGMA_SPECTRUMColIsOK){
	    newSigmaSpectrum.set(0);
	  }
	  if(WEIGHT_SPECTRUMColIsOK){
	    newWeightSpectrum.set(0);
	  }
	  if(FLAGColIsOK){
	    newFlag.set(0);
	  }
	  if(FLAG_CATEGORYColIsOK){
	    newFlagCategory.set(0);
	  }

	  vector<Matrix<Complex> > newCorrectedDataI(nSpwsToCombine); 
	  vector<Matrix<Complex> > newDataI(nSpwsToCombine);
	  vector<Matrix<Float> > newFloatDataI(nSpwsToCombine);
	  vector<Matrix<Complex> > newLagDataI(nSpwsToCombine);
	  vector<Matrix<Complex> > newModelDataI(nSpwsToCombine);
	  vector<Matrix<Float> > newSigmaSpectrumI(nSpwsToCombine);
	  vector<Matrix<Float> > newWeightSpectrumI(nSpwsToCombine);
	  vector<Matrix<Bool> > newFlagI(nSpwsToCombine);
	  vector<Array<Bool> > newFlagCategoryI(nSpwsToCombine); // has three dimensions
	  vector<Bool> newFlagRowI(nSpwsToCombine);

	  for(uInt i=0; i<nSpwsToCombine; i++){
	    Int theRowSPWId = spwsToCombine[i];
        if(SPWtoRowIndex.find(theRowSPWId) != SPWtoRowIndex.end( )){ // there actually is a matching row for this SPW
	      Int theRow = SPWtoRowIndex[theRowSPWId];
	      if(CORRECTED_DATAColIsOK){
		newCorrectedDataI[theRowSPWId].reference(oldCORRECTED_DATACol(theRow));
	      }
	      if(DATAColIsOK){
		newDataI[theRowSPWId].reference(oldDATACol(theRow));
	      }
	      if(FLOAT_DATAColIsOK){
		newFloatDataI[theRowSPWId].reference(oldFLOAT_DATACol(theRow));
	      }
	      if(LAG_DATAColIsOK){
		newLagDataI[theRowSPWId].reference(oldLAG_DATACol(theRow));
	      }
	      if(MODEL_DATAColIsOK){
		newModelDataI[theRowSPWId].reference(oldMODEL_DATACol(theRow));
	      }
	      if(SIGMA_SPECTRUMColIsOK){
		newSigmaSpectrumI[theRowSPWId].reference(oldSIGMA_SPECTRUMCol(theRow));
	      }
	      if(WEIGHT_SPECTRUMColIsOK){
		newWeightSpectrumI[theRowSPWId].reference(oldWEIGHT_SPECTRUMCol(theRow));
	      }
	      if(FLAGColIsOK){
		newFlagI[theRowSPWId].reference(oldFLAGCol(theRow));
	      }
	      if(FLAG_CATEGORYColIsOK){
		newFlagCategoryI[theRowSPWId].reference(oldFLAG_CATEGORYCol(theRow));
	      }
	      newFlagRowI[theRowSPWId] = flagRowCol(theRow);
	    } // end if
	  } // end for i
	  
	  // merge data columns from all rows found using the averaging info from above
	  // averageN[], averageWhichSPW[], averageWhichChan[], averageChanFrac[]

	  Bool failedAv = false;	  
	  vector<Int> failedAvChans;
	  vector<Int> failedAvCorrs;

	  // loop over new channels
	  for(uInt i=0; i<newNUM_CHAN; i++){
	    // initialise special treatment for Bool columns
	    if(FLAGColIsOK){
	      for(uInt k=0; k<nCorrelations; k++){ 
		newFlag(k,i) =  true; // overwritten with false below if there is a SPW where this channel is not flagged for this correlator
	      }
	    }
	    if(FLAG_CATEGORYColIsOK){
	      for(uInt k=0; k<nCorrelations; k++){ 
		for(uInt m=0; m<nCat; m++){ 
		  newFlagCategory(IPosition(3,k,i,m)) = false;
		}
	      }
	    }

	    Bool haveCoverage = false;
	    Vector<Double> modNorm(nCorrelations, 0.); // normalization for the averaging of the contributions from the SPWs
	    vector<vector<Double> > modAverageChanFrac(averageN[i], vector<Double>(nCorrelations, 0.));
	    for(uInt k=0; k<nCorrelations; k++){
	      Vector<Int> spwCount(nSpwsToCombine, 0);
	      for(Int j=0; j<averageN[i]; j++){
        if(SPWtoRowIndex.find(averageWhichSPW[i][j]) != SPWtoRowIndex.end( )){
		  if(!newFlagI[ averageWhichSPW[i][j] ]( k, averageWhichChan[i][j] )){
		    haveCoverage = true;
		    if(FLAGColIsOK){
		      newFlag(k,i) = false; // there is valid data for this channel and correlation => don't flag in output
		    }
		    modAverageChanFrac[j][k] = averageChanFrac[i][j];
		    modNorm(k) += averageChanFrac[i][j];
		    if(modAverageChanFrac[j][k]!=1.){
		      ++spwCount(averageWhichSPW[i][j]); // count the contributions with non-unity overlap fraction for each spw
		    }
		  }
		}
	      }
	      if(modNorm(k)>0.){ // there are contributions
		for(Int j=0; j<averageN[i]; j++){
		  // Second iteration: eliminate contributions from spws with odd numbers of contribs with non-unity overlap fraction
		  // which could influence the averaging asymmetrically
          if(SPWtoRowIndex.find(averageWhichSPW[i][j]) != SPWtoRowIndex.end( )
		     && spwCount(averageWhichSPW[i][j])<averageN[i] // there are also channels with full overlap 
		     && spwCount(averageWhichSPW[i][j])%2!=0     // the number of channels with non-unity overlap is odd
		     && modAverageChanFrac[j][k]!=1.){           // this contributor j has non-unity overlap
		    //cout << "spw count " << averageWhichSPW[i][j] << " " << spwCount(averageWhichSPW[i][j]) << endl;
		    //cout << "not using i j k spw frac " << i << " " << j << " " << k << " " 
                    //     << averageWhichSPW[i][j] << " " << modAverageChanFrac[j][k] << endl;
		    modNorm(k) -= modAverageChanFrac[j][k];
		    modAverageChanFrac[j][k] = 0.;
		    if(modNorm(k)<=0.){ // should only occur in rare cases
		      if(FLAGColIsOK && !newFlag(k,i)){
			failedAv = true;
			newFlag(k,i) = true;
			if(prevNewMainTabRow<0){
			  prevNewMainTabRow = newMainTabRow;
			}
			failedAvChans.push_back(i);
			failedAvCorrs.push_back(k);
		      }
		      modNorm(k) += averageChanFrac[i][j];
		      modAverageChanFrac[j][k] = averageChanFrac[i][j];
		      break;
		    }
		  }		
		}
	      }
	    } // end loop over correlations

	    if(haveCoverage){ // there is unflagged data for this channel
	      // loop over SPWs
	      for(Int j=0; j<averageN[i]; j++){
		// new channel value i 
		//   = SUM{j=0 to averageN[i]}( channelValue(SPW = averageWhichSPW[i][j], CHANNEL = averageWhichChan[i][j]) * averageChanFrac[i][j])
        if(SPWtoRowIndex.find(averageWhichSPW[i][j]) != SPWtoRowIndex.end( )){

		  Double weight = 0.;

		  // loop over first dimension (number of correlations)
		  for(uInt k=0; k<nCorrelations; k++){
		    if(!newFlagI[ averageWhichSPW[i][j] ]( k, averageWhichChan[i][j] )
		       && modNorm(k)>0.){ // this channel is not flagged for the given SPW and correlator and there are contributions

                      // renormalize for the case of missing SPW coverage
		      weight = modAverageChanFrac[j][k] / modNorm(k);

		      if(CORRECTED_DATAColIsOK){
			newCorrectedData(k,i) += newCorrectedDataI[ averageWhichSPW[i][j] ]( k, averageWhichChan[i][j] ) * weight;			
		      }
		      if(DATAColIsOK){
			newData(k,i) += newDataI[ averageWhichSPW[i][j] ]( k, averageWhichChan[i][j] ) * weight;
// 			if (!wasprinted[i][j]){
// 			  cout << "row " << SPWtoRowIndex(averageWhichSPW[i][j]) << "averageWhichSPW[i][j] " 
// 			       << averageWhichSPW[i][j] << "  averageWhichChan[i][j] " << averageWhichChan[i][j]
// 			       << " i, j, k " << i << ", " << j << ", " << k << " modAverageChanFrac[j][k] " << modAverageChanFrac[j][k] 
// 			       << " modNorm(k) " << modNorm(k) << " newDataI[ averageWhichSPW[i][j] ]( k, averageWhichChan[i][j] ) "
// 			       << newDataI[ averageWhichSPW[i][j] ]( k, averageWhichChan[i][j] ) 
// 			       << " newData(k,i) " << newData(k,i) 
// 			       << " weight " << weight << endl;
// 			  wasprinted[i][j] = true;
// 			} 
		      }
		      if(FLOAT_DATAColIsOK){
			newFloatData(k,i) += newFloatDataI[ averageWhichSPW[i][j] ]( k, averageWhichChan[i][j] ) * weight;
		      }
		      if(LAG_DATAColIsOK){
			newLagData(k,i) += newLagDataI[ averageWhichSPW[i][j] ]( k, averageWhichChan[i][j] ) * weight;
		      }
		      if(MODEL_DATAColIsOK){
			newModelData(k,i) += newModelDataI[ averageWhichSPW[i][j] ]( k, averageWhichChan[i][j] ) * weight;
		      }
		      if(SIGMA_SPECTRUMColIsOK){
			newSigmaSpectrum(k,i) += newSigmaSpectrumI[ averageWhichSPW[i][j] ]( k, averageWhichChan[i][j] ) * weight;
		      }
		      if(WEIGHT_SPECTRUMColIsOK){
			newWeightSpectrum(k,i) += newWeightSpectrumI[ averageWhichSPW[i][j] ]( k, averageWhichChan[i][j] ) * weight;
		      }

		    } // end if flagged
		  } // end for k = 0
		
		  // special treatment for flag cat
		  if(FLAG_CATEGORYColIsOK){
		    for(uInt k=0; k<nCorrelations; k++){ // logical OR of all input spws
		      for(uInt m=0; m<nCat; m++){ 
			newFlagCategory(IPosition(3,k,i,m)) = 
			  newFlagCategory(IPosition(3,k,i,m)) || newFlagCategoryI[ averageWhichSPW[i][j] ](IPosition(3,k,averageWhichChan[i][j],m));
		      }
		    }
		  }

		} // end if there is a row for this SPW	      
	      } // end for j=0, loop over SPWs
	    } // if there is coverage for this channel 
	  } // end for i=0, loop over new channels

	  // give warnings about channels with averaging problems
	  if(prevNewMainTabRow>=0){ // may need to issue warnings
	    if(!failedAv 
	       || mainTabRowI==nMainTabRows-1
	       || (prevFailedAvChans.size()>0 && !((failedAvChans==prevFailedAvChans) && (failedAvCorrs==prevFailedAvCorrs)))
	       ){
	      uInt lastRow = newMainTabRow-1;
	      if(mainTabRowI==nMainTabRows-1){
		lastRow = newMainTabRow;
	      }
	      os << LogIO::WARN << "Averaging failed for the following channel/correllation pairs from output row " 
		 << prevNewMainTabRow << " up to " << lastRow << ". Corresponding visibilities will be flagged: " << endl;
	      for(uInt iii=0; iii<prevFailedAvChans.size(); iii++){
		os << "(" << prevFailedAvChans[iii] << ", " << prevFailedAvCorrs[iii] << ") ";
	      }
	      os << LogIO::POST;
	      prevNewMainTabRow = -1;
	    }
	    prevFailedAvChans = failedAvChans;
	    prevFailedAvCorrs = failedAvCorrs;
	  }     
	    
	  // calculate FLAG_ROW as logical OR of all input rows
	  newFlagRow = newFlagRowI[0];
	  for(uInt i=1; i<nMatchingRows; i++){
	    newFlagRow = newFlagRow || newFlagRowI[i];
	  }

	  // write data into the new main table
	  newMain.addRow(1,false);

	  //	  cout << "writing new row " << newMainTabRow << endl;
	  
	  if(CORRECTED_DATAColIsOK){
	    CORRECTED_DATACol.put(newMainTabRow, newCorrectedData);
	  }
	  if(DATAColIsOK){
// 	    cout << "old " << oldDATACol(mainTabRow).shape() << endl;
// 	    cout << "new " << newData.shape() << endl;
	    DATACol.put(newMainTabRow, newData);
	  }
	  if(FLOAT_DATAColIsOK){
	    FLOAT_DATACol.put(newMainTabRow, newFloatData);
	  }
	  if(LAG_DATAColIsOK){
	    LAG_DATACol.put(newMainTabRow, newLagData);
	  }
	  if(MODEL_DATAColIsOK){
	    MODEL_DATACol.put(newMainTabRow, newModelData);
	  }
	  if(SIGMA_SPECTRUMColIsOK){
	    SIGMA_SPECTRUMCol.put(newMainTabRow, newSigmaSpectrum);
	  }
	  if(WEIGHT_SPECTRUMColIsOK){
	    WEIGHT_SPECTRUMCol.put(newMainTabRow, newWeightSpectrum);
	  }
	  if(FLAGColIsOK){
	    FLAGCol.put(newMainTabRow, newFlag);
	  }
	  if(FLAG_CATEGORYColIsOK){
	    FLAG_CATEGORYCol.put(newMainTabRow, newFlagCategory);
	  }

	  mainCols.flagRow().put(newMainTabRow, newFlagRow);
	  
      if(tempDDIndex.find(theDataDescId) != tempDDIndex.end( )){
	    // do DD ID renumbering (due to shrunk DD table and spw combination )
	    mainCols.dataDescId().put(newMainTabRow, tempDDIndex.at(theDataDescId)); 
	  }
	  else{
	    mainCols.dataDescId().put(newMainTabRow,  DDIdCol(mainTabRow)); 
	  }	  
	  // copy the rest of the row contents from mainTabRow
	  mainCols.sigma().put(newMainTabRow, SIGMACol(mainTabRow));
	  mainCols.weight().put(newMainTabRow, oldMainCols.weight()(mainTabRow));

	  mainCols.fieldId().put(newMainTabRow, fieldCol(mainTabRow));
	  mainCols.antenna1().put(newMainTabRow, antenna1Col(mainTabRow));
	  mainCols.antenna2().put(newMainTabRow, antenna2Col(mainTabRow));
	  mainCols.time().put(newMainTabRow, timeCol(mainTabRow));
	  mainCols.exposure().put(newMainTabRow, exposureCol(mainTabRow));
	  mainCols.interval().put(newMainTabRow, intervalCol(mainTabRow));

	  mainCols.uvw().put(newMainTabRow, oldMainCols.uvw()(mainTabRow));
	  mainCols.arrayId().put(newMainTabRow, oldMainCols.arrayId()(mainTabRow));
	  mainCols.feed1().put(newMainTabRow, oldMainCols.feed1()(mainTabRow));
	  mainCols.feed2().put(newMainTabRow, oldMainCols.feed2()(mainTabRow));
	  mainCols.observationId().put(newMainTabRow, oldMainCols.observationId()(mainTabRow));
	  mainCols.processorId().put(newMainTabRow, oldMainCols.processorId()(mainTabRow));
	  mainCols.scanNumber().put(newMainTabRow, oldMainCols.scanNumber()(mainTabRow));
	  mainCols.stateId().put(newMainTabRow, oldMainCols.stateId()(mainTabRow));
	  mainCols.timeCentroid().put(newMainTabRow, oldMainCols.timeCentroid()(mainTabRow));

	  //	  cout << "Wrote new row " << newMainTabRow << endl;
	  newMainTabRow++;	  
	  // mark other found rows to be ignored, i.e. set their time to zero
	  for(uInt i=1; i<nMatchingRows; i++){ // don't mark the first
	    //	    cout << "setting time to zero in row " << matchingRows[i] << endl;
	    timeCol.put(matchingRows[i], 0);
	  }
	  
	} // end if row is affected
	
	mainTabRowI++;
	if(mainTabRowI>nMainTabRows*progress){
	  cout << "combineSpws progress: " << progress*100 << "% processed ... " << endl;
	  progress += progressStep;
	}
      
      } // end loop over main table rows
      cout << "combineSpws progress: 100% processed." << endl;
      ////////////////////////////////////////////

      if(verbose){
	os << LogIO::NORMAL << "Processed " << mainTabRowI << " original rows, wrote "
	   << newMainTabRow << " new ones." << LogIO::POST;

	if(nIncompleteCoverage>0){
	  os << LogIO::WARN << "Incomplete coverage of combined SPW in " << nIncompleteCoverage
	     << " of " <<  newMainTabRow << " output rows." <<  LogIO::POST;
	}
      }

    } // end scope for MS related objects
 
    String oldName(ms_p.tableName());

    // detach old MS
    ms_p = MeasurementSet();
    mssel_p = MeasurementSet();

    // rename the result MS overwriting the old MS
    {
      Table tab(tempNewName, Table::Update);
      tab.rename(oldName, Table::New);
    }

    // attach new MS
    ms_p = MeasurementSet(oldName, Table::Update);
    mssel_p = ms_p;

    if(verbose){
      os << LogIO::NORMAL << "Spectral window combination complete." << LogIO::POST;
    }

    return true;

  }

  const Vector<MS::PredefinedColumns>& SubMS::parseColumnNames(String col)
  {
    // Memoize both for efficiency and so that the info message at the bottom
    // isn't unnecessarily repeated.
    static String my_colNameStr = "";
    static Vector<MS::PredefinedColumns> my_colNameVect;

    col.upcase();
    if(col == my_colNameStr && col != ""){
      return my_colNameVect;
    }    
    else if(col == "None"){
      my_colNameStr = "";
      my_colNameVect.resize(0);
      return my_colNameVect;
    }
 
    LogIO os(LogOrigin("SubMS", "parseColumnNames()"));
    
    uInt nNames;
    
    if(col.contains("ALL")){
      nNames = 3;
      my_colNameVect.resize(nNames);
      my_colNameVect[0] = MS::DATA;
      my_colNameVect[1] = MS::MODEL_DATA;
      my_colNameVect[2] = MS::CORRECTED_DATA;
    }
    else{
      nNames = dataColStrToEnums(col, my_colNameVect);
    }

    // Whether or not the MS has the columns is checked by verifyColumns().
    // Unfortunately it cannot be done here because this is a static method.

    if(col != "NONE"){  // "NONE" is used by ~SubMS().
      os << LogIO::NORMAL
         << "Using ";     // Don't say "Splitting"; this is used elsewhere.
      for(uInt i = 0; i < nNames; ++i)
        os << MS::columnName(my_colNameVect[i]) << " ";
      os << " column" << (my_colNameVect.nelements() > 1 ? "s." : ".")
         << LogIO::POST;
    }
    
    my_colNameStr = col;
    return my_colNameVect;
  }

// This version uses the MeasurementSet to check what columns are present,
// i.e. it makes col=="all" smarter, and it is not necessary to call
// verifyColumns() after calling this.  Unlike the other version, it knows
// about FLOAT_DATA and LAG_DATA.  It throws an exception if a
// _specifically_ requested column is absent.
const Vector<MS::PredefinedColumns>& SubMS::parseColumnNames(String col,
                                                  const MeasurementSet& msref)
{
  // Memorize both for efficiency and so that the info message at the bottom
  // isn't unnecessarily repeated.
  static String my_colNameStr = "";
  static Vector<MS::PredefinedColumns> my_colNameVect;

  Vector<MS::PredefinedColumns> wanted;       // Data columns to pick up if present.
  
  col.upcase();
// This version of parseColumnNames does not reuse results of previous calls
// but always checks the given columns because it cannot be certain that msref
// refers to the same MS with every call.

  if(col == "None"){
    my_colNameStr = "";
    my_colNameVect.resize(0);
    return my_colNameVect;
  }
 
  LogIO os(LogOrigin("SubMS", "parseColumnNames()"));
    
  // Are we choosy?
  const Bool doAny = col.contains("ALL") || col.contains("ANY");
  
  uInt nPoss;
  if(doAny){
    nPoss = 5;
    wanted.resize(nPoss);
    wanted[0] = MS::DATA;
    wanted[1] = MS::MODEL_DATA;
    wanted[2] = MS::CORRECTED_DATA;
    wanted[3] = MS::FLOAT_DATA;
    wanted[4] = MS::LAG_DATA;
  }
  else{ // split name string into individual names
    nPoss = dataColStrToEnums(col, wanted);
  } 

  uInt nFound = 0;
  my_colNameVect.resize(0);
  for(uInt i = 0; i < nPoss; ++i){
    if(msref.tableDesc().isColumn(MS::columnName(wanted[i]))){
      ++nFound;
      my_colNameVect.resize(nFound, true);
      my_colNameVect[nFound - 1] = wanted[i];
    }
    else if(!doAny){
      ostringstream ostr;
      ostr << "Desired column (" << MS::columnName(wanted[i])
           << ") not found in the input MS (" << msref.tableName() << ").";
      throw(AipsError(ostr.str()));
    }
  }
  if(nFound == 0)
    throw(AipsError("Did not find and select any data columns."));
  
  os << LogIO::NORMAL
     << "Using ";     // Don't say "Splitting"; this is used elsewhere.
  for(uInt i = 0; i < nFound; ++i)
    os << MS::columnName(my_colNameVect[i]) << " ";
  os << "column" << (nFound > 1 ? "s." : ".") << LogIO::POST;
    
  my_colNameStr = col;
  return my_colNameVect;
}

uInt SubMS::dataColStrToEnums(const String& col, Vector<MS::PredefinedColumns>& colvec)
{
  LogIO os(LogOrigin("SubMS", "dataColStrToEnums()"));
  String tmpNames(col);
  Vector<String> tokens;
  tmpNames.upcase();
    
  // split name string into individual names
  char * pch;
  Int i = 0;
  pch = strtok((char*)tmpNames.c_str(), " ,");
  while (pch != NULL){
    tokens.resize(i + 1, true);
    tokens[i] = String(pch);
    ++i;
    pch = strtok(NULL, " ,");
  }

  uInt nNames = tokens.nelements();

  uInt nFound = 0;
  for(uInt i = 0; i < nNames; ++i){
    colvec.resize(nFound + 1, true);
    colvec[nFound] = MS::UNDEFINED_COLUMN;
	    
    if (tokens[i] == "OBSERVED" || 
        tokens[i] == "DATA" || 
        tokens[i] == MS::columnName(MS::DATA)){
      colvec[nFound++] = MS::DATA;
    }
    else if(tokens[i] == "FLOAT" || 
            tokens[i] == "FLOAT_DATA" || 
            tokens[i] == MS::columnName(MS::FLOAT_DATA)){
      colvec[nFound++] = MS::FLOAT_DATA;
    } 
    else if(tokens[i] == "LAG" || 
            tokens[i] == "LAG_DATA" || 
            tokens[i] == MS::columnName(MS::LAG_DATA)){
      colvec[nFound++] = MS::LAG_DATA;
    } 
    else if(tokens[i] == "MODEL" || 
            tokens[i] == "MODEL_DATA" || 
            tokens[i] == MS::columnName(MS::MODEL_DATA)){
      colvec[nFound++] = MS::MODEL_DATA;
    } 
    else if(tokens[i] == "CORRECTED" || 
            tokens[i] == "CORRECTED_DATA" || 
            tokens[i] == MS::columnName(MS::CORRECTED_DATA)){
      colvec[nFound++] = MS::CORRECTED_DATA;
    }
    else if(tmpNames != "NONE"){  // "NONE" is used by ~SubMS().
      os << LogIO::SEVERE;
      if(nFound == 0){
        colvec[0] = MS::DATA;
        os << "Unrecognized data column " << tokens[i] << "...trying DATA.";
      }
      else
        os << "Skipping unrecognized data column " << tokens[i];
      os << LogIO::POST;
    }
  }
  return nFound;
}


Bool SubMS::fillAccessoryMainCols(){
  LogIO os(LogOrigin("SubMS", "fillAccessoryMainCols()"));
  uInt nrows = mssel_p.nrow();

  msOut_p.addRow(nrows, true);
  
  //#ifdef COPYTIMER
  Timer timer;
  timer.mark();
  //#endif
  if(!antennaSel_p){
    msc_p->antenna1().putColumn(mscIn_p->antenna1().getColumn());
    msc_p->antenna2().putColumn(mscIn_p->antenna2().getColumn());
    os << LogIO::DEBUG1
       << "Straight copying ANTENNA* took " << timer.real() << "s."
       << LogIO::POST;
  }
  else{
    Vector<Int> ant1(mscIn_p->antenna1().getColumn());
    Vector<Int> ant2(mscIn_p->antenna2().getColumn());
    
    for(uInt k = 0; k < nrows; ++k){
      ant1[k] = antNewIndex_p[ant1[k]];
      ant2[k] = antNewIndex_p[ant2[k]];
    }
    msc_p->antenna1().putColumn(ant1);
    msc_p->antenna2().putColumn(ant2);
    os << LogIO::DEBUG1
       << "Selectively copying ANTENNA* took " << timer.real() << "s."
       << LogIO::POST;
  }
  
  timer.mark();
  msc_p->feed1().putColumn(mscIn_p->feed1().getColumn());
  msc_p->feed2().putColumn(mscIn_p->feed2().getColumn());
  os << LogIO::DEBUG1
     << "Copying FEED* took " << timer.real() << "s."
     << LogIO::POST;
  
  timer.mark();
  msc_p->exposure().putColumn(mscIn_p->exposure().getColumn());
  os << LogIO::DEBUG1
     << "Copying EXPOSURE took " << timer.real() << "s."
     << LogIO::POST;

  timer.mark();
  msc_p->flagRow().putColumn(mscIn_p->flagRow().getColumn());
  os << LogIO::DEBUG1
     << "Copying flagRow took " << timer.real() << "s."
     << LogIO::POST;

  timer.mark();
  msc_p->interval().putColumn(mscIn_p->interval().getColumn());
  os << LogIO::DEBUG1
     << "Copying INTERVAL took " << timer.real() << "s."
     << LogIO::POST;

  timer.mark();
  msc_p->scanNumber().putColumn(mscIn_p->scanNumber().getColumn());
  os << LogIO::DEBUG1
     << "Copying scanNumber took " << timer.real() << "s."
     << LogIO::POST;

  timer.mark();
  msc_p->time().putColumn(mscIn_p->time().getColumn());
  os << LogIO::DEBUG1
     << "Copying TIME took " << timer.real() << "s."
     << LogIO::POST;

  timer.mark();
  msc_p->timeCentroid().putColumn(mscIn_p->timeCentroid().getColumn());
  os << LogIO::DEBUG1
     << "Copying timeCentroid took " << timer.real() << "s."
     << LogIO::POST;
  
  // ScalarMeasColumn doesn't have a putColumn() for some reason.
  //msc_p->uvwMeas().putColumn(mscIn_p->uvwMeas());
  timer.mark();
  //msc_p->uvw().putColumn(mscIn_p->uvw());      // 98s for 4.7e6 rows

  // 3.06s for 4.7e6 rows
  //RefRows refrows(0,  nrows - 1);
  //msc_p->uvw().putColumnCells(refrows, mscIn_p->uvw().getColumn());

  msc_p->uvw().putColumn(mscIn_p->uvw().getColumn());   // 2.74s for 4.7e6 rows
  os << LogIO::DEBUG1
     << "Copying uvw took " << timer.real() << "s."
     << LogIO::POST;
  
  msc_p->arrayId().putColumn(mscIn_p->arrayId().getColumn());
  msc_p->processorId().putColumn(mscIn_p->processorId().getColumn());

  timer.mark();
  relabelIDs();
  os << LogIO::DEBUG1
     << "relabelIDs took " << timer.real() << "s."
     << LogIO::POST;

  return true;
}

  Bool SubMS::writeAllMainRows(const Vector<MS::PredefinedColumns>& colNames)
  {  
    LogIO os(LogOrigin("SubMS", "writeAllMainRows()"));
    Bool success = true;
    Timer timer;

    fillAccessoryMainCols();

    //Deal with data, flags, sigma, and weights.
    timer.mark();
    if(keepShape_p){
      Vector<MS::PredefinedColumns> complexCols;
      const Bool doFloat = sepFloat(colNames, complexCols);
      const uInt nDataCols = complexCols.nelements();
      const Bool writeToDataCol = mustConvertToData(nDataCols, complexCols);

      copyDataFlagsWtSp(complexCols, writeToDataCol);
      if(doFloat)
        msc_p->floatData().putColumn(mscIn_p->floatData());
    }
    else{
      doChannelMods(colNames);
    }
    os << LogIO::DEBUG1
       << "Total data read/write time = " << timer.real()
       << LogIO::POST;
    
    return success;
  }
  
Bool SubMS::existsFlagCategory() const
{
  Bool hasFC = false;
  if(!mscIn_p->flagCategory().isNull() &&
     mscIn_p->flagCategory().isDefined(0)){
    IPosition fcshape(mscIn_p->flagCategory().shape(0));
    IPosition fshape(mscIn_p->flag().shape(0));

    // I don't know or care how many flag categories there are.
    hasFC = fcshape(0) == fshape(0) && fcshape(1) == fshape(1);
  }
  return hasFC;
}

  Bool SubMS::getDataColumn(ArrayColumn<Complex>& data,
                            const MS::PredefinedColumns colName)
  {
    if(colName == MS::DATA)
      data.reference(mscIn_p->data());
    else if(colName == MS::MODEL_DATA)
      data.reference(mscIn_p->modelData());
    else if(colName == MS::LAG_DATA)
      data.reference(mscIn_p->lagData());
    else                                // The honored-by-time-if-nothing-else
      data.reference(mscIn_p->correctedData()); // default.
    return true;
  }

  Bool SubMS::getDataColumn(ArrayColumn<Float>& data,
                            const MS::PredefinedColumns colName)
  {
    LogIO os(LogOrigin("SubMS", "getDataColumn()"));

    if(colName != MS::FLOAT_DATA)
      os << LogIO::WARN
	 << "Using FLOAT_DATA (because it has type Float) instead of the requested "
	 << colName
	 << LogIO::POST;
    
    data.reference(mscIn_p->floatData());
    return true;
  }

  Bool SubMS::putDataColumn(MSColumns& msc, ArrayColumn<Complex>& data, 
                            const MS::PredefinedColumns colName,
                            const Bool writeToDataCol)
  {
    if(writeToDataCol || colName == MS::DATA) 
      msc.data().putColumn(data);
    else if (colName ==  MS::MODEL_DATA)
      msc.modelData().putColumn(data);
    else if (colName == MS::CORRECTED_DATA)
      msc.correctedData().putColumn(data);
    //else if(colName == MS::FLOAT_DATA)              // promotion from Float
    //  msc.floatData().putColumn(data);              // to Complex is pvt?
    else if(colName == MS::LAG_DATA)
      msc.lagData().putColumn(data);
    else
      return false;
    return true;
  }

void SubMS::setFitOrder(Int fitorder, Bool advise)
{
  fitorder_p = fitorder;

  if(advise){
    LogIO os(LogOrigin("SubMS", "setFitOrder()"));

    if(fitorder < 0)
      os << LogIO::NORMAL
         << "Keeping the continuum.";
    else if(fitorder > 1)
      os << LogIO::WARN
         << "Fit orders > 1 tend to drastically add noise to line channels.";
    os << LogIO::POST;
  }
}

Bool SubMS::shouldWatch(Bool& conflict, const String& col,
                        const String& uncombinable,
                        const Bool verbose) const
{
  Bool wantWatch = !combine_p.contains(col);

  if(!wantWatch && uncombinable.contains(col)){
    conflict = true;
    wantWatch = false;

    if(verbose){
      LogIO os(LogOrigin("SubMS", "shouldWatch()"));

      os << LogIO::WARN
         << "Combining by " << col
         << " was requested, but it is not allowed by this operation and will be ignored."
         << LogIO::POST;
    }
  }
  return wantWatch;
}

Bool SubMS::setSortOrder(Block<Int>& sort, const String& uncombinable,
                         const Bool verbose) const
{
  Bool conflict = false;
  uInt n_cols_to_watch = 7;     // 3 + #(watchables), whether or not they are watched.

  // Already separated by the chunking.
  //const Bool watch_array(!combine_p.contains("arr")); // Pirate talk for "array".

  Bool watch_obs   = shouldWatch(conflict, "obs",   uncombinable, verbose);
  Bool watch_scan  = shouldWatch(conflict, "scan",  uncombinable, verbose);
  Bool watch_spw   = shouldWatch(conflict, "spw",   uncombinable, verbose);
  Bool watch_state = shouldWatch(conflict, "state", uncombinable, verbose);

  // if(watch_obs)
  //   ++n_cols_to_watch;
  // if(watch_scan)
  //   ++n_cols_to_watch;
  // if(watch_spw)
  //   ++n_cols_to_watch;
  // if(watch_state)
  //   ++n_cols_to_watch;

  uInt colnum = 1;

  sort.resize(n_cols_to_watch);
  sort[0] = MS::ARRAY_ID;
  if(watch_scan){
    sort[colnum] = MS::SCAN_NUMBER;
    ++colnum;
  }
  if(watch_state){
    sort[colnum] = MS::STATE_ID;
    ++colnum;
  }
  sort[colnum] = MS::FIELD_ID;
  ++colnum;
  if(watch_spw){
    sort[colnum] = MS::DATA_DESC_ID;
    ++colnum;
  }
  sort[colnum] = MS::TIME;
  ++colnum;  
  if(watch_obs){
    sort[colnum] = MS::OBSERVATION_ID;
    ++colnum;
  }

  // Now all the axes that should be combined should be added, so that they end
  // up in the same chunk.
  if(!watch_scan){
    sort[colnum] = MS::SCAN_NUMBER;
    ++colnum;
  }
  if(!watch_state){
    sort[colnum] = MS::STATE_ID;
    ++colnum;
  }
  if(!watch_spw){
    sort[colnum] = MS::DATA_DESC_ID;
    ++colnum;
  }
  if(!watch_obs){
    sort[colnum] = MS::OBSERVATION_ID;
    //++colnum;
  }

  return !conflict;
}

Bool SubMS::subtractContinuum(const Vector<MS::PredefinedColumns>& colNames,
                              const VBRemapper& vbmaps)
{
  LogIO os(LogOrigin("SubMS", "subtractContinuum()"));
  Bool retval = true;

  if(colNames.nelements() != 1){
    os << LogIO::SEVERE
       << "The continuum cannot be subtracted from > 1 *DATA column at a time."
       << LogIO::POST;
    return false;
  }

  Block<Int> sort;
  if(!setSortOrder(sort, "obs,scan,state", false)){
    os << LogIO::WARN
       << "This version of continuum subtraction intentionally does not support\n"
       << "time smearing.  The only recommended (and used) values for combine in\n"
       << "this case are '' or 'spw'."
       << LogIO::POST;
  }

  // Aaargh...everywhere else VisIter is used a timeInterval of 0 is treated as
  // DBL_MAX, meaning that TIME can be in sort but effectively be ignored for
  // major chunking.  Why couldn't they just have said DBL_MAX in the first
  // place?
  ROVisibilityIterator viIn(mssel_p, sort, false, DBL_MIN);

  // Make sure it is initialized before any copies are made.
  viIn.originChunks();

  VBGContinuumSubtractor vbgcs(msOut_p, msc_p, vbmaps, viIn, fitorder_p,
                               colNames[0], fitspw_p, fitoutspw_p);
  vbgcs.setTVIDebug(tvi_debug);
  vbgcs.setWantCont(want_cont_p);
  GroupProcessor gp(viIn, &vbgcs);
  gp.setTVIDebug(tvi_debug);

  retval = gp.go();

  // TODO: Support uvcontsub(3)'s spw parameter by
  //  * filtering the output by fitoutspw_p
  //  * remapping DDID as necessary, taking (union)spw into account
  //  * filtering and rewriting the DATA_DESC_ID and SPECTRAL_WINDOW subtables.

  return retval;
}

void SubMS::fill_vbmaps(std::map<VisBufferComponents::EnumType, std::map<Int, Int> >& vbmaps)
{
  // In general, _IDs which are row numbers in a subtable must be
  // remapped, and those which are not probably shouldn't be.
  if(antennaSel_p){
    std::map<Int, Int> antIndexer;

    fillAntIndexer(antIndexer, mscIn_p);
    vbmaps[VisBufferComponents::Ant1] = antIndexer;
    vbmaps[VisBufferComponents::Ant2] = antIndexer;
  }

  if(!allEQ(spwRelabel_p, spw_p)){
    std::map<Int, Int> ddidMapper;

    for(uInt i = 0; i < oldDDSpwMatch_p.nelements(); ++i)
      ddidMapper[i] = spwRelabel_p[oldDDSpwMatch_p[i]];

    vbmaps[VisBufferComponents::DataDescriptionId] = ddidMapper;
  }

  if(fieldid_p.nelements() < mscIn_p->field().nrow()){
    std::map<Int, Int> fldMapper;
    
    make_map2(fldMapper, fieldRelabel_p);
    vbmaps[VisBufferComponents::FieldId] = fldMapper;
  }

  if(selObsId_p.nelements() > 0 && selObsId_p.nelements() < mscIn_p->observation().nrow()){
    std::map<Int, Int> obsMapper;

    make_map(obsMapper, selObsId_p);
    vbmaps[VisBufferComponents::ObservationId] = obsMapper;
  }

  //std::map<Int, Int> procMapper;
  //make_map(procMapper, mscIn_p->processorId().getColumn());

  if(stateRemapper_p.size() < 1)
    make_map(stateRemapper_p, mscIn_p->stateId().getColumn());
  if(stateRemapper_p.size() < mscIn_p->state().nrow())
    vbmaps[VisBufferComponents::StateId] = stateRemapper_p;
}

Bool SubMS::copyDataFlagsWtSp(const Vector<MS::PredefinedColumns>& colNames,
                              const Bool writeToDataCol)
  {
    Block<Int> columns;
    // include scan and state iteration, for more optimal iteration
    columns.resize(6);
    columns[0]=MS::ARRAY_ID;
    columns[1]=MS::SCAN_NUMBER;
    columns[2]=MS::STATE_ID;
    columns[3]=MS::FIELD_ID;
    columns[4]=MS::DATA_DESC_ID;
    columns[5]=MS::TIME;

#ifdef COPYTIMER
    Timer timer;
    timer.mark();

    Vector<Int> inscan, outscan;
#endif

    ROVisIter viIn(mssel_p,columns,0.0);
    VisIter viOut(msOut_p,columns,0.0);
    viIn.setRowBlocking(1000);
    viOut.setRowBlocking(1000);
    Int iChunk(0), iChunklet(0);
    Cube<Complex> data;
    Cube<Bool> flag;
    Array<Bool> flagcat;

    Matrix<Float> wtmat;
    viIn.originChunks();                                // Makes me feel better.
    const Bool doWtSp(viIn.existsWeightSpectrum());
    Cube<Float> wtsp;

    uInt ninrows = mssel_p.nrow();
    ProgressMeter meter(0.0, ninrows * 1.0, "split", "rows copied", "", "",
                        true, 1);
    uInt inrowsdone = 0;  // only for the meter.

    uInt nDataCols = colNames.nelements();

    // Is CORRECTED_DATA being moved to DATA?
    Bool fromCorrToData = writeToDataCol && nDataCols == 1;
    if(fromCorrToData)
      fromCorrToData = colNames[0] == MS::CORRECTED_DATA;

    Bool doFC = existsFlagCategory();

    for (iChunk=0,viOut.originChunks(),viIn.originChunks();
	 viOut.moreChunks(),viIn.moreChunks();
	 viOut.nextChunk(),viIn.nextChunk(),++iChunk) {
      inrowsdone += viIn.nRowChunk();

      // The following can help evaluable in/out index alignment
      /*
      cout << "****iChunk=" << iChunk 
	   << " scn: " << viIn.scan(inscan)(0) << "/" << viOut.scan(outscan)(0) << "   "
	   << "fld: " << viIn.fieldId() << "/"  << viOut.fieldId() << "   "
	   << "ddi: " << viIn.dataDescriptionId() << "/" << viOut.dataDescriptionId() << "   "
	   << "spw: " << viIn.spectralWindow() << "/"  << viOut.spectralWindow() << "   "
	   << endl;
      */
      for (iChunklet=0,viIn.origin(),viOut.origin();
	   viIn.more(),viOut.more();
	   viIn++,viOut++,++iChunklet) { //  

	//	cout << "nRows = " << viIn.nRow() << "/" << viOut.nRow() << endl;

#ifdef COPYTIMER
	timer.mark();
#endif
        viIn.flag(flag);
        viOut.setFlag(flag);
        if(doFC){
          viIn.flagCategory(flagcat);
          viOut.setFlagCategory(flagcat);
        }
        if(fromCorrToData) {
	  viIn.weightMat(wtmat);
	  viOut.setWeightMat(wtmat);
          arrayTransformInPlace(wtmat, subms::wtToSigma);   // for corrected weights.
	  viOut.setSigmaMat(wtmat);
	}
        else {
          viIn.sigmaMat(wtmat);           // Yes, I'm reusing wtmat.
	  viOut.setSigmaMat(wtmat);
          arrayTransformInPlace(wtmat, subms::sigToWeight);   // for corrected weights.
	  viOut.setWeightMat(wtmat);
	}	  

        if(doWtSp){
          viIn.weightSpectrum(wtsp);
          viOut.setWeightSpectrum(wtsp);
        }

        for(uInt colnum = 0; colnum < nDataCols; ++colnum){
          if(writeToDataCol || colNames[colnum] == MS::DATA) {
            // write DATA, MODEL_DATA, or CORRECTED_DATA to DATA
            switch (colNames[colnum]) {
            case MS::DATA:
              viIn.visibility(data,VisibilityIterator::Observed);
              break;
            case MS::MODEL_DATA:
              viIn.visibility(data,VisibilityIterator::Model);
              break;
            case MS::CORRECTED_DATA:
              viIn.visibility(data,VisibilityIterator::Corrected);
              break;
            default:
              throw(AipsError("Unrecognized input column!"));
              break;
            }
            viOut.setVis(data,VisibilityIterator::Observed);
          }
          else if (colNames[colnum] ==  MS::MODEL_DATA) {
            // write MODEL_DATA to MODEL_DATA
            viIn.visibility(data,VisibilityIterator::Model);
            viOut.setVis(data,VisibilityIterator::Model);
          }
          else if (colNames[colnum] == MS::CORRECTED_DATA) {
            // write CORRECTED_DATA to CORRECTED_DATA
            viIn.visibility(data,VisibilityIterator::Corrected);
            viOut.setVis(data,VisibilityIterator::Corrected);
          }
          //else if(colNames[colnum] == MS::FLOAT_DATA)              // TBD
          //	else if(colNames[colnum] == MS::LAG_DATA)      // TBD
          else
            return false;
        }

#ifdef COPYTIMER	
	Double t=timer.real();
	cout << "Chunk: " << iChunk << " " << iChunklet 
	     << " scn: " << viIn.scan(inscan)(0) << "/" << viOut.scan(outscan)(0) 
             << "   "
	     << " spw: " << viIn.spectralWindow() << "/"  << viOut.spectralWindow() 
             << " : "
	     << data.nelements() << " cells = " 
	     << data.nelements()*8.e-6 << " MB in " 
	     << t << " sec, for " << data.nelements()*8.e-6/t << " MB/s" 
	     << endl;
#endif
	
      }
      meter.update(inrowsdone);
    }
    return true;
  }

  Bool SubMS::putDataColumn(MSColumns& msc, ArrayColumn<Float>& data, 
                            const MS::PredefinedColumns colName,
                            const Bool writeToDataCol)
  {
    LogIO os(LogOrigin("SubMS", "putDataColumn()"));

    if(writeToDataCol)
      os << LogIO::NORMAL
	 << "Writing to FLOAT_DATA instead of DATA."
	 << LogIO::POST;

    if(colName == MS::FLOAT_DATA){
      msc.floatData().putColumn(data);
    }
    else{
      os << LogIO::SEVERE
	 << "Float data cannot be written to "
	 << MS::columnName(colName)
	 << LogIO::POST;
      return false;
    }
    return true;
  }

  // Sets outcol to row numbers in the corresponding subtable of its ms that
  // correspond to the values of incol.
  //
  // Can only be used when incol and outcol have the same # of rows!
  //
void SubMS::remapColumn(ScalarColumn<Int>& outcol,
                        const ScalarColumn<Int>& incol)
{
  uInt nrows = incol.nrow();
    
  if(nrows != outcol.nrow()){
    ostringstream ostr;

    ostr << "SubMS::remapColumn(): the # of input rows, " << nrows
         << ", != the # of output rows, " << outcol.nrow();
    throw(AipsError(ostr.str()));
  }
    
  std::map<Int, Int> mapper;
  Vector<Int> inv(incol.getColumn());

  make_map(mapper, inv);
  if(mapper.size() == 1){
    outcol.fillColumn(0);        // Just a little optimization.
  }
  else{
    Vector<Int> outv(nrows);

    if(mapper.size() > 0){
      for(uInt k = 0; k < nrows; ++k)
        outv[k] = mapper[inv[k]];
    }
    else
      incol.getColumn(outv);
    outcol.putColumn(outv);
  }
}

void SubMS::remapColumn(ScalarColumn<Int>& outcol,
                        const ScalarColumn<Int>& incol,
                        const Vector<Int>& selvals)
{
  uInt nrows = incol.nrow();
    
  if(nrows != outcol.nrow()){
    ostringstream ostr;

    ostr << "SubMS::remapColumn(): the # of input rows, " << nrows
         << ", != the # of output rows, " << outcol.nrow();
    throw(AipsError(ostr.str()));
  }
    
  std::map<Int, Int> mapper;
  Vector<Int> inv(incol.getColumn());
  Vector<Int> outv(nrows);

  make_map(mapper, selvals);
  if(mapper.size() == 1){
    outcol.fillColumn(0);        // Just a little optimization.
  }
  else{
    Vector<Int> outv(nrows);

    if(mapper.size() > 0){
      for(uInt k = 0; k < nrows; ++k)
        outv[k] = mapper[inv[k]];
    }
    else
      incol.getColumn(outv);
    outcol.putColumn(outv);
  }
}
  
// Realigns some _ID vectors so that the output looks like a whole dataset
// instead of part of one.  (i.e. we don't want references to unselected spws,
// etc.)
void SubMS::relabelIDs()
{
  const ScalarColumn<Int> inDDID(mscIn_p->dataDescId());
  const ScalarColumn<Int> fieldId(mscIn_p->fieldId());
  
  for(Int k = inDDID.nrow(); k--;){
    msc_p->dataDescId().put(k, spwRelabel_p[oldDDSpwMatch_p[inDDID(k)]]);
    msc_p->fieldId().put(k, fieldRelabel_p[fieldId(k)]);
  }

  //remapColumn(msc_p->arrayId(), mscIn_p->arrayId());
  remapColumn(msc_p->stateId(), mscIn_p->stateId());
  //remapColumn(msc_p->processorId(), mscIn_p->processorId());
  remapColumn(msc_p->observationId(), mscIn_p->observationId(), selObsId_p);
}

Bool SubMS::writeSomeMainRows(const Vector<MS::PredefinedColumns>& colNames)
{    
  LogIO os(LogOrigin("SubMS", "writeSomeMainRows()"));
  Bool retval = true;
    
  os << LogIO::DEBUG1 // helpdesk ticket in from Oleg Smirnov (ODU-232630)
     << "Before fillAntIndexer(): "
     << Memory::allocatedMemoryInBytes() / (1024.0 * 1024.0) << " MB"
     << LogIO::POST;

  // A set of maps from input ID to output ID, keyed by VisBufferComponent.
  std::map<VisBufferComponents::EnumType, std::map<Int, Int> > vbmaps;
  fill_vbmaps(vbmaps);
  VBRemapper remapper(vbmaps);

  //things to be taken care of in doTimeAver() or subtractContinuum...
  // flagRow		ScanNumber	uvw		weight		
  // sigma		ant1		ant2		time
  // timeCentroid	feed1 		feed2		exposure
  // stateId		processorId	observationId	arrayId
  if(fitorder_p >= 0)
    retval = subtractContinuum(colNames, remapper); // writeToDataCol = complexCols == 1
  else
    retval = (corrString_p != "") ? doTimeAverVisIterator(colNames, remapper)
                                  : doTimeAver(colNames, remapper);
  return retval;
}

uInt SubMS::addOptionalColumns(const Table& inTab, Table& outTab,
                               const Bool beLazy)
{
  uInt nAdded = 0;    
  const TableDesc& inTD = inTab.actualTableDesc();
    
  // Only rely on the # of columns if you are sure that inTab and outTab
  // can't have the same # of columns without having _different_ columns,
  // i.e. use beLazy if outTab.actualTableDesc() is in its default state.
  uInt nInCol = inTD.ncolumn();
  if(!beLazy || nInCol > outTab.actualTableDesc().ncolumn()){
    LogIO os(LogOrigin("SubMS", "addOptionalColumns()"));

    Vector<String> oldColNames = inTD.columnNames();
      
    for(uInt k = 0; k < nInCol; ++k){
      if(!outTab.actualTableDesc().isColumn(oldColNames[k])){
        //TableDesc tabDesc;
        try{
          //M::addColumnToDesc(tabDesc, M::columnType(oldColNames[k]));
          //if(tabDesc.ncolumn())                 // The tabDesc[0] is too 
          //  outTab.addColumn(tabDesc[0]);       // dangerous otherwise - it 
          //else                                  // can dump core without
          //  throw(AipsError("Unknown column")); // throwing an exception.
          outTab.addColumn(inTD.columnDesc(k), false);
          ++nAdded;
        }
        catch(...){   // NOT AipsError x
          os << LogIO::WARN 
             << "Could not add column " << oldColNames[k] << " to "
             << outTab.tableName()
             << LogIO::POST;
        }
      }
    }
  }
  return nAdded;
}
  
Bool SubMS::copyCols(Table& out, const Table& in, const Bool flush)
{
  LogIO os(LogOrigin("SubMS", "copyCols()"));
  const TableDesc& inTD = in.actualTableDesc();
  Vector<String> inColNames = inTD.columnNames();
  uInt nInCol = inTD.ncolumn();
  Bool retval = true;

  if(in.nrow() > out.nrow())
    out.addRow(in.nrow() - out.nrow());

  for(uInt k = 0; k < nInCol; ++k){
    // Add the input column desc to out if necessary.
    if(!out.actualTableDesc().isColumn(inColNames[k])){
      try{
        out.addColumn(inTD.columnDesc(k), false);
      }
      catch(...){   // NOT AipsError x
        os << LogIO::WARN 
           << "Could not add column " << inColNames[k] << " to "
           << out.tableName()
           << LogIO::POST;
        retval = false;
      }
    }

    // I can't see a way to explicitly* avoid constructing these in each
    // iteration.  (attach() would implicitly construct them.)
    // * without relying on compiler optimization.
    TableColumn incol(in, inColNames[k]);
    TableColumn outcol(out, inColNames[k]);

    outcol.putColumn(incol);
  }
  if(flush)
    out.flush();
  return retval;
}

  Bool SubMS::copyAntenna(){
    const MSAntenna& oldAnt = mssel_p.antenna();
    MSAntenna& newAnt = msOut_p.antenna();
    const MSAntennaColumns incols(oldAnt);
    MSAntennaColumns         outcols(newAnt);
    Bool 		     retval = false;
    
    outcols.setOffsetRef(MPosition::castType(incols.offsetMeas().getMeasRef().getType()));
    outcols.setPositionRef(MPosition::castType(incols.positionMeas().getMeasRef().getType()));

    if(!antennaSel_p){
      TableCopy::copyRows(newAnt, oldAnt);
      retval = true;
    }
    else{
      uInt nAnt = antNewIndex_p.nelements();
      if(nAnt > oldAnt.nrow())                  // Don't use min() here,
        nAnt = oldAnt.nrow();                   // it's too overloaded.

      for(uInt k = 0; k < nAnt; ++k){
        if(antNewIndex_p[k] > -1)
          TableCopy::copyRows(newAnt, oldAnt, antNewIndex_p[k], k, 1, false);
      }
      retval = true;
    }
    return retval;    
  }


  Bool SubMS::copyFeed()
  {
    const MSFeed& oldFeed = mssel_p.feed();

    // if(oldFeed.nrow() < 1)     Useless, because it ignores spw selection
    
    MSFeed& newFeed = msOut_p.feed();
    const MSFeedColumns incols(oldFeed);
    MSFeedColumns         outcols(newFeed);
    
    outcols.setDirectionRef(MDirection::castType(incols.beamOffsetMeas().getMeasRef().getType()));
    outcols.setEpochRef(MEpoch::castType(incols.timeMeas().getMeasRef().getType()));
    outcols.setPositionRef(MPosition::castType(incols.positionMeas().getMeasRef().getType()));

    if(!antennaSel_p && allEQ(spwRelabel_p, spw_p)){
      TableCopy::copyRows(newFeed, oldFeed);
    }
    else{
      const Vector<Int>& antIds = incols.antennaId().getColumn();
      const Vector<Int>& spwIds = incols.spectralWindowId().getColumn();

      // Copy selected rows.
      uInt totNFeeds = antIds.nelements();
      uInt totalSelFeeds = 0;
      Int maxSelAntp1 = antNewIndex_p.nelements();
      for (uInt k = 0; k < totNFeeds; ++k){
        // antenna must be selected, and spwId must be -1 (any) or selected.
	if(antIds[k] < maxSelAntp1 && antNewIndex_p[antIds[k]] > -1 &&
           (spwIds[k] < 0 || spwRelabel_p[spwIds[k]] > -1)){
          //                  outtab   intab    outrow       inrow nrows
	  TableCopy::copyRows(newFeed, oldFeed, totalSelFeeds, k, 1, false);
          ++totalSelFeeds;
	}
      }

      // Remap antenna and spw #s.
      ScalarColumn<Int>& antCol = outcols.antennaId();
      ScalarColumn<Int>& spwCol = outcols.spectralWindowId();

      for(uInt k = 0; k < totalSelFeeds; ++k){
	antCol.put(k, antNewIndex_p[antCol(k)]);
        if(spwCol(k) > -1)
          spwCol.put(k, spwRelabel_p[spwCol(k)]);
      }
    }

    if(newFeed.nrow() < 1){
      LogIO os(LogOrigin("SubMS", "copyFeed()"));
      os << LogIO::SEVERE << "No feeds were selected." << LogIO::POST;
      return false;
    }
    return true;
  }
  
  Bool SubMS::copyFlag_Cmd(){
    // Like POINTING, FLAG_CMD is supposed to exist but is allowed not to.
    if(Table::isReadable(mssel_p.flagCmdTableName())){
      // An attempt to select from FLAG_CMD by timerange.  Fails because the
      // TEN refers to the main table, not FLAG_CMD.
      // TableExprNode condition;

      // if(timeRange_p != "" &&
      // 	 msTimeGramParseCommand(&ms_p, timeRange_p, condition) == 0){
      // 	const TableExprNode *timeNode = 0x0;
	  
      // 	timeNode = msTimeGramParseNode();
      // 	if(timeNode && !timeNode->isNull())
      // 	  condition = *timeNode;
      // }

      const MSFlagCmd& oldFlag_Cmd = mssel_p.flagCmd();

      if(oldFlag_Cmd.nrow() > 0){
	// Could be declared as Table&
        MSFlagCmd& newFlag_Cmd = msOut_p.flagCmd();

        LogIO os(LogOrigin("SubMS", "copyFlag_Cmd()"));

        // Add optional columns if present in oldFlag_Cmd.
        uInt nAddedCols = addOptionalColumns(oldFlag_Cmd, newFlag_Cmd, true);
        os << LogIO::DEBUG1 << "FLAG_CMD has " << nAddedCols
           << " optional columns." << LogIO::POST;
	
        const MSFlagCmdColumns oldFCs(oldFlag_Cmd);
        MSFlagCmdColumns newFCs(newFlag_Cmd);
        newFCs.setEpochRef(MEpoch::castType(oldFCs.timeMeas().getMeasRef().getType()));
	
        //if(!antennaSel_p && timeRange_p == ""){
          TableCopy::copyRows(newFlag_Cmd, oldFlag_Cmd);
        // }
        // else{
        //   const ScalarColumn<Double>& time = oldFCs.time();

	//   uInt nTRanges = selTimeRanges_p.ncolumn();

	//   uInt outRow = 0;
        //   for (uInt inRow = 0; inRow < antIds.nrow(); ++inRow){
        //     Int newAntInd = antIds(inRow);
	//     if(antennaSel_p)
	//       newAntInd = antNewIndex_p[newAntInd];
	//     Double t = time(inRow);
	    
        //     if(newAntInd > -1){
	//       Bool matchT = false;
	//       for(uInt tr = 0; tr < nTRanges; ++tr){
	// 	if(t >= selTimeRanges_p(0, tr) && t <= selTimeRanges_p(1, tr)){
	// 	  matchT = true;
	// 	  break;
	// 	}
	//       }
	      
	//       if(matchT){
	// 	TableCopy::copyRows(newFlag_Cmd, oldFlag_Cmd, outRow, inRow, 1, false);
	// 	outants.put(outRow, newAntInd);
	// 	++outRow;
	//       }
        //     }
        //   }
      }
    }
    return true;
  }
  
  Bool SubMS::copyHistory(){
    const MSHistory& oldHistory = mssel_p.history();

    // Could be declared as Table&
    MSHistory& newHistory = msOut_p.history();

    LogIO os(LogOrigin("SubMS", "copyHistory()"));

    // Add optional columns if present in oldHistory.
    uInt nAddedCols = addOptionalColumns(oldHistory, newHistory, true);
    os << LogIO::DEBUG1 << "HISTORY has " << nAddedCols
       << " optional columns." << LogIO::POST;
	
    const MSHistoryColumns oldHCs(oldHistory);
    MSHistoryColumns newHCs(newHistory);
    newHCs.setEpochRef(MEpoch::castType(oldHCs.timeMeas().getMeasRef().getType()));
	
    TableCopy::copyRows(newHistory, oldHistory);
    return true;
  }
  
  Bool SubMS::copySource(){
    //Source is an optional table, so it may not exist
    if(Table::isReadable(mssel_p.sourceTableName())){
      LogIO os(LogOrigin("SubMS", "copySource()"));

      const MSSource& oldSource = mssel_p.source();
      MSSource& newSource = msOut_p.source();
      
      // Add optional columns if present in oldSource.
      uInt nAddedCols = addOptionalColumns(oldSource, newSource, true);
      os << LogIO::DEBUG1 << "SOURCE has " << nAddedCols
         << " optional columns." << LogIO::POST;
      
      const MSSourceColumns incols(oldSource);
      MSSourceColumns         outcols(newSource);

      // Copy the Measures frame info.  This has to be done before filling the
      // rows.
      outcols.setEpochRef(MEpoch::castType(incols.timeMeas().getMeasRef().getType()));
      outcols.setDirectionRef(MDirection::castType(incols.directionMeas().getMeasRef().getType()));
      outcols.setPositionRef(MPosition::castType(incols.positionMeas().getMeasRef().getType()));
      outcols.setFrequencyRef(MFrequency::castType(incols.restFrequencyMeas().getMeasRef().getType()));
      outcols.setRadialVelocityRef(MRadialVelocity::castType(incols.sysvelMeas().getMeasRef().getType()));

      const ScalarColumn<Int>& inSId   = incols.sourceId();
      ScalarColumn<Int>& 	 outSId  = outcols.sourceId();
      const ScalarColumn<Int>& inSPW   = incols.spectralWindowId();
      ScalarColumn<Int>& 	 outSPW  = outcols.spectralWindowId();

      // 2009-06-09: It is hard to say whether to remap pulsarID when the
      // PULSAR table is not described in the MS v2.0 def'n.
//       const ScalarColumn<Int>& inPId   = incols.pulsarId();
//       ScalarColumn<Int>& 	 outPId  = outcols.pulsarId();

      uInt outrn = 0; 		   	   		// row number in output.
      uInt nInputRows = inSId.nrow();
      Int maxSId = sourceRelabel_p.nelements();  // inSidVal is Int.
      Int maxSPWId = spwRelabel_p.nelements();
      for(uInt inrn = 0; inrn < nInputRows; ++inrn){
	Int inSidVal = inSId(inrn);
	Int inSPWVal = inSPW(inrn);  // -1 means the source is valid for any SPW.
	
        if(inSidVal >= maxSId)
	  os << LogIO::WARN
             << "Invalid SOURCE ID in SOURCE table row " << inrn << LogIO::POST;
	if(inSPWVal >= maxSPWId)
	  os << LogIO::WARN
             << "Invalid SPW ID in SOURCE table row " << inrn << LogIO::POST;

	if((inSidVal > -1) && (inSidVal < maxSId) &&
           (sourceRelabel_p[inSidVal] > -1) && 
           ((inSPWVal == -1) || (inSPWVal < maxSPWId && spwRelabel_p[inSPWVal] > -1))){
	  // Copy inrn to outrn.
	  TableCopy::copyRows(newSource, oldSource, outrn, inrn, 1);
	  outSId.put(outrn, sourceRelabel_p[inSidVal]);
	  outSPW.put(outrn, inSPWVal > -1 ? spwRelabel_p[inSPWVal] : -1);
	  
	  ++outrn;
	}
      }

      return true;
    }
  
    return false;
  }
  
Bool SubMS::copyGenericSubtables(){
  LogIO os(LogOrigin("SubMS", "copyGenericSubtables()"));

  // Already handled subtables will be removed from this, so a modifiable copy
  // is needed.
  TableRecord inkws(mssel_p.keywordSet());

  // Some of the standard subtables need special handling,
  // e.g. DATA_DESCRIPTION, SPECTRAL_WINDOW, and ANTENNA, so they are already
  // defined in msOut_p.  Several more (e.g. FLAG_CMD) were also already
  // created by MS::createDefaultSubtables().  Don't try to write over them - a
  // locking error will result.
  const TableRecord& outkws = msOut_p.keywordSet();
  for(uInt i = 0; i < outkws.nfields(); ++i){
    // os << LogIO::DEBUG1
    //    << "outkws.name(" << i << "): " << outkws.name(i)
    //    << LogIO::POST;
    if(outkws.type(i) == TpTable && inkws.isDefined(outkws.name(i)))
      inkws.removeField(outkws.name(i));
  }

  // Includes a flush. 
  //msOut_p.unlock();

  // msOut_p.rwKeywordSet() (pass a reference, not a copy) will put a lock on
  // msOut_p.
  TableCopy::copySubTables(msOut_p.rwKeywordSet(), inkws, msOut_p.tableName(),
			   msOut_p.tableType(), mssel_p);
  // TableCopy::copySubTables(Table, Table, Bool) includes this other code,
  // which seems to be copying subtables at one level deeper, but not
  // recursively? 
  const TableDesc& inDesc = mssel_p.tableDesc();
  const TableDesc& outDesc = msOut_p.tableDesc();
  for(uInt i = 0; i < outDesc.ncolumn(); ++i){
    // Only writable cols can have keywords (and thus subtables) defined.
    if(msOut_p.isColumnWritable(i)){
      const String& name = outDesc[i].name();

      if(inDesc.isColumn(name)){
	TableColumn outCol(msOut_p, name);
	TableColumn inCol(mssel_p, name);
	
	TableCopy::copySubTables(outCol.rwKeywordSet(), inCol.keywordSet(),
				 msOut_p.tableName(), msOut_p.tableType(),
				 mssel_p);

	// Copy the keywords if column is FLOAT_DATA
	if (name == "FLOAT_DATA")
		copyMainTableKeywords(outCol.rwKeywordSet(), inCol.keywordSet());

      }
    }
  }

  return true;
}

  Bool SubMS::copyObservation()
  {
    const MSObservation& oldObs = mssel_p.observation();
    MSObservation& newObs = msOut_p.observation();
    const MSObservationColumns oldObsCols(oldObs);
    MSObservationColumns newObsCols(newObs);
    newObsCols.setEpochRef(MEpoch::castType(oldObsCols.releaseDateMeas().getMeasRef().getType()));

    uInt nObs = selObsId_p.nelements();
    if(nObs > 0){
      for(uInt outrn = 0; outrn < nObs; ++outrn)
        TableCopy::copyRows(newObs, oldObs, outrn, selObsId_p[outrn], 1);
    }
    else  // '' -> '*'
      TableCopy::copyRows(newObs, oldObs);
      
    //W TableCopy::deepCopy(newObs, oldObs, false);
    
    return true;
  }
  
  Bool SubMS::copyProcessor()
  {  
    const MSProcessor& oldProc = mssel_p.processor();
    MSProcessor& newProc = msOut_p.processor();

    TableCopy::copyRows(newProc, oldProc);
    //W TableCopy::deepCopy(newProc, oldProc, false);
    
    return true;
  }

Bool SubMS::copyState()
{  
  // STATE is allowed to not exist, even though it is not optional in the MS
  // def'n.  For one thing, split dropped it for quite a while.
  if(Table::isReadable(mssel_p.stateTableName())){
    LogIO os(LogOrigin("SubMS", "copyState()"));
    const MSState& oldState = mssel_p.state();

    if(oldState.nrow() > 0){
      MSState& newState = msOut_p.state();
      const MSStateColumns oldStateCols(oldState);
      MSStateColumns newStateCols(newState);

      // Initialize stateRemapper_p if necessary.
      if(stateRemapper_p.size() < 1)
	make_map(stateRemapper_p, mscIn_p->stateId().getColumn());

      uInt nStates = stateRemapper_p.size();
      
      // stateRemapper_p goes from input to output, as is wanted in most
      // places.  Here we need a map going the other way, so make one.
      Vector<Int> outStateToInState(nStates);
      std::map<Int, Int>::iterator mit;
      
      for(mit = stateRemapper_p.begin(); mit != stateRemapper_p.end(); ++mit)
	outStateToInState[(*mit).second] = (*mit).first;
      
      for(uInt outrn = 0; outrn < nStates; ++outrn)
	TableCopy::copyRows(newState, oldState, outrn,
			    outStateToInState[outrn], 1);
    }
  }
  return true;
}

void SubMS::createSubtables(MeasurementSet& ms, Table::TableOption option)
{
  SetupNewTable antennaSetup(ms.antennaTableName(),
                             MSAntenna::requiredTableDesc(),option);
  ms.rwKeywordSet().defineTable(MS::keywordName(MS::ANTENNA),
                             Table(antennaSetup));
  SetupNewTable dataDescSetup(ms.dataDescriptionTableName(),
                              MSDataDescription::requiredTableDesc(),option);
  ms.rwKeywordSet().defineTable(MS::keywordName(MS::DATA_DESCRIPTION), 
                             Table(dataDescSetup));
  SetupNewTable feedSetup(ms.feedTableName(),
                          MSFeed::requiredTableDesc(),option);
  ms.rwKeywordSet().defineTable(MS::keywordName(MS::FEED), Table(feedSetup));
  SetupNewTable flagCmdSetup(ms.flagCmdTableName(),
                             MSFlagCmd::requiredTableDesc(),option);
  ms.rwKeywordSet().defineTable(MS::keywordName(MS::FLAG_CMD), 
                             Table(flagCmdSetup));
  SetupNewTable fieldSetup(ms.fieldTableName(),
                           MSField::requiredTableDesc(),option);
  ms.rwKeywordSet().defineTable(MS::keywordName(MS::FIELD), Table(fieldSetup));
  SetupNewTable historySetup(ms.historyTableName(),
                             MSHistory::requiredTableDesc(),option);
  ms.rwKeywordSet().defineTable(MS::keywordName(MS::HISTORY), 
                             Table(historySetup));
  SetupNewTable observationSetup(ms.observationTableName(),
                                 MSObservation::requiredTableDesc(),option);
  ms.rwKeywordSet().defineTable(MS::keywordName(MS::OBSERVATION), 
                                Table(observationSetup));
  SetupNewTable polarizationSetup(ms.polarizationTableName(),
                                  MSPolarization::requiredTableDesc(),option);
  ms.rwKeywordSet().defineTable(MS::keywordName(MS::POLARIZATION),
                                Table(polarizationSetup));
  SetupNewTable processorSetup(ms.processorTableName(),
			       MSProcessor::requiredTableDesc(),option);
  ms.rwKeywordSet().defineTable(MS::keywordName(MS::PROCESSOR),
                                Table(processorSetup));
  SetupNewTable spectralWindowSetup(ms.spectralWindowTableName(),
                                    MSSpectralWindow::requiredTableDesc(),option);
  ms.rwKeywordSet().defineTable(MS::keywordName(MS::SPECTRAL_WINDOW),  
                                Table(spectralWindowSetup));
  SetupNewTable stateSetup(ms.stateTableName(),
                           MSState::requiredTableDesc(),option);
  ms.rwKeywordSet().defineTable(MS::keywordName(MS::STATE),  
                                Table(stateSetup));

  // add the optional Source sub table to allow for specification of the rest
  // frequency
  SetupNewTable sourceSetup(ms.sourceTableName(), MSSource::requiredTableDesc(),
                            option);
  ms.rwKeywordSet().defineTable(MS::keywordName(MS::SOURCE),
                                Table(sourceSetup, 0));

  // update the references to the subtable keywords
  ms.initRefs();
}
  
  Bool SubMS::copyPointing(){
    LogIO os(LogOrigin("SubMS", "copyPointing()"));

    //Pointing is allowed to not exist
    if(Table::isReadable(mssel_p.pointingTableName())){
      // An attempt to select from POINTING by timerange.  Fails because the
      // TEN refers to the main table, not POINTING.
      // TableExprNode condition;

      // if(timeRange_p != "" &&
      // 	 msTimeGramParseCommand(&ms_p, timeRange_p, condition) == 0){
      // 	const TableExprNode *timeNode = 0x0;
	  
      // 	timeNode = msTimeGramParseNode();
      // 	if(timeNode && !timeNode->isNull())
      // 	  condition = *timeNode;
      // }

      //Wconst Table oldPoint(mssel_p.pointingTableName(), Table::Old);
      const MSPointing& oldPoint = mssel_p.pointing();

      if(!antennaSel_p && timeRange_p == ""){
        copySubtable(MS::keywordName(MS::POINTING), oldPoint);
      }
      else{
        setupNewPointing();

        if(oldPoint.nrow() > 0){
          // Could be declared as Table&
          MSPointing& newPoint = msOut_p.pointing();
          // Add optional columns if present in oldPoint.
          uInt nAddedCols = addOptionalColumns(oldPoint, newPoint, true);
          os << LogIO::DEBUG1 << "POINTING has " << nAddedCols
             << " optional columns." << LogIO::POST;
	
          // W = Works, DW = Doesn't Work
          //DW  	msOut_p.pointing() = mssel_p.pointing();	
          //DW  	//TableCopy::copyInfo(newPoint, oldPoint);
          //W  	TableColumn newTC(newPoint, "DIRECTION");
          //W  	const ScalarColumn<MDirection> oldTC(oldPoint, "DIRECTION");
          //W  	const TableColumn oldTC(oldPoint, "DIRECTION");
          //W  	newTC.rwKeywordSet() = oldTC.keywordSet();

          const MSPointingColumns oldPCs(oldPoint);
          MSPointingColumns newPCs(newPoint);
          newPCs.setEpochRef(MEpoch::castType(oldPCs.timeMeas().getMeasRef().getType()));
          newPCs.setDirectionRef(MDirection::castType(oldPCs.directionMeasCol().getMeasRef().getType()));
          newPCs.setEncoderDirectionRef(MDirection::castType(oldPCs.encoderMeas().getMeasRef().getType()));

          const ScalarColumn<Int>& antIds  = oldPCs.antennaId();
          const ScalarColumn<Double>& time = oldPCs.time();
          ScalarColumn<Int>& 	     outants = newPCs.antennaId();

	  uInt nTRanges = selTimeRanges_p.ncolumn();

	  uInt outRow = 0;
          Int maxSelAntp1 = antNewIndex_p.nelements();  // Int for comparison
                                                        // with newAntInd.
          for (uInt inRow = 0; inRow < antIds.nrow(); ++inRow){
            Int newAntInd = antIds(inRow);
	    if(antennaSel_p)
	      newAntInd = newAntInd < maxSelAntp1 ? antNewIndex_p[newAntInd] : -1;
	    Double t = time(inRow);
	    
            if(newAntInd > -1){
	      Bool matchT = false;
	      for(uInt tr = 0; tr < nTRanges; ++tr){
		if(t >= selTimeRanges_p(0, tr) && t <= selTimeRanges_p(1, tr)){
		  matchT = true;
		  break;
		}
	      }
	      
	      if(matchT){
		TableCopy::copyRows(newPoint, oldPoint, outRow, inRow, 1, false);
		outants.put(outRow, newAntInd);
		++outRow;
	      }
            }
          }
        }
      }
    }
    else
      setupNewPointing();       // Make an empty stub for MSColumns.

    return true;
  }

void SubMS::setupNewPointing()
{
  SetupNewTable pointingSetup(msOut_p.pointingTableName(),
                              MSPointing::requiredTableDesc(), Table::New);
  // POINTING can be large, set some sensible defaults for storageMgrs
  IncrementalStMan ismPointing ("ISMPointing");
  StandardStMan ssmPointing("SSMPointing", 32768);
  pointingSetup.bindAll(ismPointing, true);
  pointingSetup.bindColumn(MSPointing::columnName(MSPointing::DIRECTION),
                           ssmPointing);
  pointingSetup.bindColumn(MSPointing::columnName(MSPointing::TARGET),
                           ssmPointing);
  pointingSetup.bindColumn(MSPointing::columnName(MSPointing::TIME),
                           ssmPointing);
  msOut_p.rwKeywordSet().defineTable(MS::keywordName(MS::POINTING),
                                     Table(pointingSetup));
  msOut_p.initRefs();
}
  
  Bool SubMS::copyWeather(){
    // Weather is allowed to not exist.
    if(Table::isReadable(mssel_p.weatherTableName())){
      const MSWeather& oldWeath = mssel_p.weather();

      if(oldWeath.nrow() > 0){
        // Add a WEATHER subtable to msOut_p with 0 rows for now.
        Table::TableOption option = Table::New;
        TableDesc weatherTD = MSWeather::requiredTableDesc();
        SetupNewTable weatherSetup(msOut_p.weatherTableName(), weatherTD,
                                   option);
        msOut_p.rwKeywordSet().defineTable(MS::keywordName(MS::WEATHER),
                                           Table(weatherSetup,0));
        // update the references to the subtable keywords
        msOut_p.initRefs();

	MSWeather& newWeath = msOut_p.weather();  // Could be declared as
                                                  // Table&

	LogIO os(LogOrigin("SubMS", "copyWeather()"));

	uInt nAddedCols = addOptionalColumns(oldWeath, newWeath, true);
	os << LogIO::DEBUG1 << "WEATHER has " << nAddedCols
	   << " optional columns." << LogIO::POST;
	
  	const MSWeatherColumns oldWCs(oldWeath);
	MSWeatherColumns newWCs(newWeath);
	newWCs.setEpochRef(MEpoch::castType(oldWCs.timeMeas().getMeasRef().getType()));
	
	if(!antennaSel_p){
	  TableCopy::copyRows(newWeath, oldWeath);
	}
	else{
	  const Vector<Int>& antIds(oldWCs.antennaId().getColumn());
	  ScalarColumn<Int>& outants = newWCs.antennaId();

	  uInt selRow = 0;
          Int maxSelAntp1 = antNewIndex_p.nelements();

	  for(uInt k = 0; k < antIds.nelements(); ++k){	    
            if(antIds[k] > -1){         // Weather station is on antenna?
              if(antIds[k] < maxSelAntp1){
                Int newAntInd = antNewIndex_p[antIds[k]]; // remap ant num
              
                if(newAntInd > -1){                       // Ant selected?
                  TableCopy::copyRows(newWeath, oldWeath, selRow, k, 1);
                  outants.put(selRow, newAntInd);
                  ++selRow;
                }
              }
            }
            else{
              TableCopy::copyRows(newWeath, oldWeath, selRow, k, 1);
              outants.put(selRow, -1);  // means valid for all antennas.
              ++selRow;
            }
	  }
	}
      }
    }
    return true;
  }

Bool SubMS::copySyscal()
{
  // SYSCAL is allowed to not exist.
  if(Table::isReadable(mssel_p.sysCalTableName())){
    const MSSysCal& oldSysc = mssel_p.sysCal();

    if(oldSysc.nrow() > 0){
      // Add a SYSCAL subtable to msOut_p with 0 rows for now.
      Table::TableOption option = Table::New;
      TableDesc sysCalTD = MSSysCal::requiredTableDesc();
      SetupNewTable sysCalSetup(msOut_p.sysCalTableName(), sysCalTD,
                                option);
      msOut_p.rwKeywordSet().defineTable(MS::keywordName(MS::SYSCAL),
                                         Table(sysCalSetup, 0));
      // update the references to the subtable keywords
      msOut_p.initRefs();

      // Could be declared as Table&.
      MSSysCal& newSysc = msOut_p.sysCal();

      LogIO os(LogOrigin("SubMS", "copySysCal()"));

      uInt nAddedCols = addOptionalColumns(oldSysc, newSysc, true);
      os << LogIO::DEBUG1 << "SYSCAL has " << nAddedCols
         << " optional columns." << LogIO::POST;
	
      const MSSysCalColumns incols(oldSysc);
      MSSysCalColumns outcols(newSysc);
      outcols.setEpochRef(MEpoch::castType(incols.timeMeas().getMeasRef().getType()));
	
      if(!antennaSel_p && allEQ(spwRelabel_p, spw_p)){
        TableCopy::copyRows(newSysc, oldSysc);
      }
      else{
        const Vector<Int>& antIds = incols.antennaId().getColumn();
        const Vector<Int>& spwIds = incols.spectralWindowId().getColumn();

        // Copy selected rows.
        uInt totNSyscals = antIds.nelements();
        uInt totalSelSyscals = 0;
        Int maxSelAntp1 = antNewIndex_p.nelements(); // Int for comparison with antIds.
        for(uInt k = 0; k < totNSyscals; ++k){
          // antenna must be selected, and spwId must be -1 (any) or selected.
          if(antIds[k] < maxSelAntp1 && antNewIndex_p[antIds[k]] > -1 &&
             (spwIds[k] < 0 || spwRelabel_p[spwIds[k]] > -1)){
            //                  outtab   intab    outrow       inrow nrows
            TableCopy::copyRows(newSysc, oldSysc, totalSelSyscals, k, 1, false);
            ++totalSelSyscals;
          }
        }

        // Remap antenna and spw #s.
        ScalarColumn<Int>& antCol = outcols.antennaId();
        ScalarColumn<Int>& spwCol = outcols.spectralWindowId();

        for(uInt k = 0; k < totalSelSyscals; ++k){
          antCol.put(k, antNewIndex_p[antCol(k)]);
          if(spwCol(k) > -1)
            spwCol.put(k, spwRelabel_p[spwCol(k)]);
        }
      }
    }
  }
  return true;
}

void SubMS::copySubtable(const String& tabName, const Table& inTab,
                         const Bool doFilter)
{
  String outName(msOut_p.tableName() + '/' + tabName);

  if(PlainTable::tableCache()(outName))
    PlainTable::tableCache().remove(outName);
  inTab.deepCopy(outName, Table::New, false, Table::AipsrcEndian,
                 doFilter);
  Table outTab(outName, Table::Update);
  msOut_p.rwKeywordSet().defineTable(tabName, outTab);
  msOut_p.initRefs();
}

Bool SubMS::filterOptSubtable(const String& subtabname)
{
  LogIO os(LogOrigin("SubMS", "filterOptSubtable()"));

  // subtabname is allowed to not exist.  Use ms_p instead of mssel_p, because
  // the latter has a random name which does NOT necessarily lead to
  // subtabname.  (And if selection took care of subtables, we probably
  // wouldn't have to do it here.)
  if(Table::isReadable(ms_p.tableName() + '/' + subtabname)){
    const Table intab(ms_p.tableName() + '/' + subtabname);

    if(intab.nrow() > 0){
      // Add feed if selecting by it is ever added.
      Bool doFilter = antennaSel_p || !allEQ(spwRelabel_p, spw_p);

      copySubtable(subtabname, intab, doFilter);

      if(doFilter){
        // At this point msOut_p has subtab with 0 rows.
        Table outtab(msOut_p.tableName() + '/' + subtabname, Table::Update);

        ScalarColumn<Int> inAntIdCol(intab, "ANTENNA_ID");         // + FEED_ID if it
        ScalarColumn<Int> inSpwIdCol(intab, "SPECTRAL_WINDOW_ID"); // ever changed.
        const Vector<Int>& antIds = inAntIdCol.getColumn();
        const Vector<Int>& spwIds = inSpwIdCol.getColumn();

        // Copy selected rows.
        uInt totNOuttabs = antIds.nelements();
        uInt totalSelOuttabs = 0;
        Int maxSelAntp1 = antNewIndex_p.nelements(); // Int for comparison with
                                                     // antIds.
        Bool haveRemappingProblem = false;
        for(uInt inrow = 0; inrow < totNOuttabs; ++inrow){
          // antenna must be selected, and spwId must be -1 (any) or selected.
          // Extra care must be taken because for a while MSes had CALDEVICE
          // and SYSPOWER, but the ANTENNA_ID and SPECTRAL_WINDOW_ID of those
          // subtables were not being remapped by split.
          if(antIds[inrow] < maxSelAntp1 && antNewIndex_p[antIds[inrow]] > -1){
            
            if(spwIds[inrow] < 0 || (spwIds[inrow] <
                                     static_cast<Int>(spwRelabel_p.nelements()) &&
                                     spwRelabel_p[spwIds[inrow]] > -1)){
              TableCopy::copyRows(outtab, intab, totalSelOuttabs, inrow, 1, false);
              ++totalSelOuttabs;
            }
            // Ideally we'd like to catch antenna errors too.  They are
            // avoided, but because of the way antNewIndex_p is made, 
            // antIds[inrow] >= maxSelAntp1
            // is not necessarily an error.  It's not even possible to catch
            // all the spw errors, so we settle for catching the ones we can
            // and reliably avoiding segfaults.
            else if(spwIds[inrow] >= static_cast<Int>(spwRelabel_p.nelements()))
              haveRemappingProblem = true;
          }
        }

        if(haveRemappingProblem)
          os << LogIO::WARN
             << "At least one row of " << intab.tableName()
             << " has an antenna or spw mismatch.\n"
             << "(Presumably from an older split, sorry.)\n"
             << "If " << subtabname
             << " is important, it should be fixed with tb or browsetable,\n"
             << "or by redoing the split that made " << ms_p.tableName()
             << " (check its history)."
             << LogIO::POST;

        // Remap antenna and spw #s.
        ScalarColumn<Int> outAntCol(outtab, "ANTENNA_ID");
        ScalarColumn<Int> outSpwCol(outtab, "SPECTRAL_WINDOW_ID");

        for(uInt k = 0; k < totalSelOuttabs; ++k){
          outAntCol.put(k, antNewIndex_p[outAntCol(k)]);
          if(outSpwCol(k) > -1)
            outSpwCol.put(k, spwRelabel_p[outSpwCol(k)]);
        }
      }
    }
  }
  return true;
}

// writeDiffSpwShape() was the VisIter-using channel averager, which sounds
// great, but:
// 0. If VisIter's sort order is different from the one in the MS, then if you
//    write _any_ columns using VisIter-fetched data, then you must write _all_ of
//    them.  This is crucially important for channel averaging in split: normally
//    only *DATA, FLAG*, WEIGHT, and SIGMA would have to be updated, but if using
//    VisIter, the ANTENNA*, FEED*, STATE_ID, etc. have to be updated too because
//    they could be rearranged.
//
//    There does not appear to be an easy way of getting the MS's starting sort
//    order, i.e. AFAIK it is not written into a header.
//
// 1. If FLOAT_DATA is present, VisIter will ignore DATA.  (G. Moellenbrock,
//    =casa-staff, 1/15/2010)

Bool SubMS::doChannelMods(const Vector<MS::PredefinedColumns>& datacols)
{
  LogIO os(LogOrigin("SubMS", "doChannelMods()"));

  Vector<MS::PredefinedColumns> cmplxColLabels;
  const Bool doFloat = sepFloat(datacols, cmplxColLabels);
  const uInt nCmplx = cmplxColLabels.nelements();
  if(doFloat && nCmplx > 0)           // 2010-12-14
    os << LogIO::WARN
       << "Using VisIter to average both FLOAT_DATA and another DATA column is extremely experimental."
       << LogIO::POST;

  //ArrayColumn<Complex> outCmplxCols[nCmplx];
  //outDataColMap(outCmplxCols, nCmplx, cmplxColLabels);
  const Bool writeToData = mustConvertToData(nCmplx, cmplxColLabels);

  // Is CORRECTED_DATA being moved to DATA?
  Bool fromCorrToData = writeToData && nCmplx == 1;
  if(fromCorrToData)
    fromCorrToData = cmplxColLabels[0] == MS::CORRECTED_DATA;
  
  Vector<Int> spwindex(max(spw_p) + 1);
  spwindex.set(-1);
  for(uInt k = 0; k < spw_p.nelements(); ++k)
    spwindex[spw_p[k]] = k;

  Block<Int> columns;
  // include scan and state iteration, for more optimal iteration
  columns.resize(6);
  columns[0]=MS::ARRAY_ID;
  columns[1]=MS::SCAN_NUMBER;
  columns[2]=MS::STATE_ID;
  columns[3]=MS::FIELD_ID;
  columns[4]=MS::DATA_DESC_ID;
  columns[5]=MS::TIME;

#ifdef COPYTIMER
  Timer timer;
  timer.mark();

  Vector<Int> inscan, outscan;
#endif  

  ROVisIterator viIn(mssel_p, columns, 0.0);
  //ROVisibilityIterator viIn(mssel_p, columns, 0.0);
  VisIter viOut(msOut_p,columns,0.0);

  viIn.setRowBlocking(1000);
  viOut.setRowBlocking(1000);
  //viIn.slurp();
  //cerr << "Finished slurping." << endl;

  // Translate chanSlices_p into the form vb.channelAve() wants.
  Vector<Matrix<Int> > chanAveBounds;
  viIn.slicesToMatrices(chanAveBounds, chanSlices_p, widths_p);

  // // If we don't want to skip every (width - 1) out of width channels,
  // // the increments in chanSlices_p must be set to 1.
  // if(averageChannel_p){
  //   for(uInt spwind = 0; spwind < chanSlices_p.nelements(); ++spwind){
  //     Vector<Slice>& spwsls = chanSlices_p[spwind];

  //     for(uInt slnum = 0; slnum < spwsls.nelements(); ++slnum){
  //       Slice& sl = spwsls[slnum];

  //       spwsls[slnum] = Slice(sl.start(), sl.length());
  //     }
  //   }
  // }

  // Apply selection
  // for(uInt spwind = 0; spwind < spw_p.nelements(); ++spwind)
  //   viOut.selectChannel(1, chanStart_p[spwind], nchan_p[spwind],
  //                       chanStep_p[spwind], spw_p[spwind]);
  viIn.selectChannel(chanSlices_p);     // ROVisIterator
  viIn.selectCorrelation(corrSlices_p);
  
  viIn.originChunks();                                // Makes me feel better.

  const Bool doSpWeight = viIn.existsWeightSpectrum();

  Bool doFC = existsFlagCategory();
  uInt rowsdone = 0;
  ProgressMeter meter(0.0, mssel_p.nrow() * 1.0, "split", "rows averaged", "", "",
		      true, 1);
  Cube<Complex> vis;
  Cube<Float> floatvis;
  VisBuffer vb(viIn);
  Matrix<Float> wtmat;

  for(viOut.originChunks(), viIn.originChunks();
      viOut.moreChunks(), viIn.moreChunks();
      viOut.nextChunk(), viIn.nextChunk()){
    for(viIn.origin(),viOut.origin(); viIn.more(),viOut.more();
        ++viIn, ++viOut){
      uInt rowsnow = vb.nRow();

      if(rowsnow > 0){
        //RefRows rr(rowsdone, rowsdone + rowsnow - 1);

        // Preload the things that need to be channel averaged.
        for(uInt colind = 0; colind < nCmplx; ++colind){
          if(cmplxColLabels[colind] == MS::DATA)
            vb.visCube();
          else if(cmplxColLabels[colind] == MS::MODEL_DATA)
            vb.modelVisCube();
          else if(cmplxColLabels[colind] == MS::CORRECTED_DATA)
            vb.correctedVisCube();
        }
        if(doFloat)
          vb.floatDataCube();

        // The flags and weights are already loaded by this point, UNLESS the
        // row flag was true for all the rows.  Make sure they're loaded, or
        // they could end up with the wrong shape.
        vb.flagCube();
        // if(viIn.existsWeightSpectrum())
        //   vb.weightSpectrum();
        // vb.weightMat();
        if(doFC)
          vb.flagCategory();
      
        vb.channelAve(chanAveBounds[viIn.spectralWindow()],false);

        if(nCmplx > 0){
          if(vb.flagCube().shape() !=
             vb.dataCube(cmplxColLabels[0]).shape())
            throw(AipsError("Shape error after channel averaging!"));
        }
        else if(doFloat){
          if(vb.flagCube().shape() != vb.floatDataCube().shape())
            throw(AipsError("Shape error after channel averaging!"));
        }
        // else  we aren't doing anything, it seems.

        // Write the output.
        for(uInt colind = 0; colind < nCmplx; ++colind){
          if(cmplxColLabels[colind] == MS::DATA)
            viOut.setVis(vb.visCube(), VisibilityIterator::Observed);
          else if(cmplxColLabels[colind] == MS::MODEL_DATA)
            viOut.setVis(vb.modelVisCube(),
                         writeToData ? VisibilityIterator::Observed :
                                       VisibilityIterator::Model);
          else if(cmplxColLabels[colind] == MS::CORRECTED_DATA)
            viOut.setVis(vb.correctedVisCube(),
                         writeToData ? VisibilityIterator::Observed :
                                       VisibilityIterator::Corrected);
        }
        //if(doFloat)
        //  viOut.setFloatData(vb.floatDataCube());    TBD!
        viOut.setFlag(vb.flagCube());
        if(doFC)
          viOut.setFlagCategory(vb.flagCategory());


        if(doSpWeight)
          viOut.setWeightSpectrum(vb.weightSpectrum());


        if(fromCorrToData) {
	  wtmat.reference(vb.weightMat());
	  viOut.setWeightMat(wtmat);
          arrayTransformInPlace(wtmat, subms::wtToSigma);   // for corrected weights.
	  viOut.setSigmaMat(wtmat);
	}
        else {
          wtmat.reference(vb.sigmaMat());           // Yes, I'm reusing wtmat.
	  viOut.setSigmaMat(wtmat);
          arrayTransformInPlace(wtmat, subms::sigToWeight);   // for corrected weights.
	  viOut.setWeightMat(wtmat);
	}

        rowsdone += rowsnow;
      }
    }
    meter.update(rowsdone);
  }   // End of for(viIn.originChunks(); viIn.moreChunks(); viIn.nextChunk())
  os << LogIO::NORMAL << "Data binned." << LogIO::POST;

  //const ColumnDescSet& cds = mssel_p.tableDesc().columnDescSet();
  //const ColumnDesc& cdesc = cds[MS::columnName(MS::DATA)];
  //ROTiledStManAccessor tacc(mssel_p, cdesc.dataManagerGroup());
  //tacc.showCacheStatistics(cerr);  // A 99.x% hit rate is good.  0% is bad.

  os << LogIO::DEBUG1 // helpdesk ticket in from Oleg Smirnov (ODU-232630)
     << "Post binning memory: " << Memory::allocatedMemoryInBytes() / (1024.0 * 1024.0) << " MB"
     << LogIO::POST;

  return true;
}

// // Sets mapper to the distinct values of mscol, in increasing order.
// // A static method that is used by SubMS, but doesn't necessarily have to go
// // with it.  It may belong in something more MSColumnsish.
// void SubMS::make_map(const Vector<Int>& mscol, Vector<Int>& mapper)
// {
//   std::set<Int> valSet;
  
//   for(Int i = mscol.nelements(); i--;)  // Strange, but slightly more
//     valSet.insert(mscol[i]);            // efficient than going forward.
//   mapper.resize(valSet.size());

//   uInt remaval = 0;
//   for(std::set<Int>::const_iterator vs_iter = valSet.begin();
//       vs_iter != valSet.end(); ++vs_iter){
//     mapper[remaval] = *vs_iter;
//     ++remaval;
//   }
// }

void SubMS::make_map(std::map<Int, Int>& mapper, const Vector<Int>& inv)
{
  std::set<Int> valSet;
  
  for(Int i = inv.nelements(); i--;)  // Strange, but slightly more
    valSet.insert(inv[i]);         // efficient than going forward.

  uInt remaval = 0;
  for(std::set<Int>::const_iterator vs_iter = valSet.begin();
      vs_iter != valSet.end(); ++vs_iter){
    mapper[*vs_iter] = remaval;
    ++remaval;
  }
}

void SubMS::make_map2(std::map<Int, Int>& mapper, const Vector<Int>& inv)
{
  LogIO os(LogOrigin("SubMS", "make_map2()"));
  // This method assumes the int vector contains mapping scheme already
  // if inv[i] is not selected one => -1 so should be skipped
  // ith vector element is mapped value of index i
  // Created for remapping of field Ids with fieldRelable_p (TT 2012.07.27)
  for(Vector<Int>::const_iterator vs_iter = inv.begin();
     vs_iter != inv.end(); ++vs_iter){
    if (*vs_iter!=-1) {
      Int dist=std::distance(inv.begin(),vs_iter);
      mapper.insert(std::make_pair(dist, *vs_iter));
      os << LogIO::DEBUG1
         << " *vs_iter="<< *vs_iter
         << " index="<< dist 
         << LogIO::POST;
    }
  }
}
  //

void SubMS::remap(Vector<Int>& col, const Vector<Int>& mapper)
{
  if(mapper.nelements() > 0)
    for(Int row = col.nelements(); row--;)
      col[row] = mapper[col[row]];
}

void SubMS::remap(Vector<Int>& col, const std::map<Int, Int>& mapper)
{
  if(mapper.size() > 0)
    for(Int row = col.nelements(); row--;)
      col[row] = mapper.find(col[row])->second;
}

// Returns rv s.t. mapper[rv] == ov, assuming that mapper[i + 1] >= mapper[i]
// for 0 <= i < mapper.nelements() - 1.
// i can be supplied as the first guess.
uInt SubMS::remapped(const Int ov, const Vector<Int>& mapper, uInt i=0)
{
  uInt nvals = mapper.nelements();
  uInt nvalsm1 = nvals - 1;

  if(i >= nvals)	// Second guess the guess.
    i = nvals >> 1;
  
  // Do a widening search for the brackets, since i is probably close to the
  // answer.
  if(mapper[i] == ov)
    return i;
  
  uInt inc = 1;
  uInt lb = i;
  uInt ub;
  if(ov > mapper[lb]){	// Hunt up.
    if(lb == nvalsm1)
      return lb;
    ub = lb + 1;
    while(ov >= mapper[ub]){
      lb = ub;
      inc += inc;
      ub = lb + inc;
      if(ub > nvalsm1){
	ub = nvals;
	break;
      }
    }
  }
  else{			// Hunt down.
    ub = lb;
    --lb;
    while(ov < mapper[lb]){
      ub = lb;
      inc += inc;
      if(inc >= ub){
	lb = 0;
	break;
      }
      else
	lb = ub - inc;
    }
  }
        
  i = lb + ((ub - lb) >> 1);			// (lb + ub) / 2 might overflow. 
  while(mapper[i] != ov && ub - lb > 1){
    if(mapper[i] < ov)
      lb = i;
    else
      ub = i;
    i = lb + ((ub - lb) >> 1);
  }
  return i;  
}

uInt SubMS::fillAntIndexer(std::map<Int, Int>& antIndexer, const MSColumns *msc)
{
  const Vector<Int>& ant1 = msc->antenna1().getColumn();
  const Vector<Int>& ant2 = msc->antenna2().getColumn();

  std::set<Int> ants;
  for(Int i = ant1.nelements(); i--;){   // Strange, but slightly more
    ants.insert(ant1[i]);	         // efficient than going forward.
    ants.insert(ant2[i]);
  }
  uInt nant = ants.size();
  
  Vector<Int> selAnt(nant);
  Int remaval = 0;
  for(std::set<Int>::const_iterator ant_iter = ants.begin();
      ant_iter != ants.end(); ++ant_iter){
    selAnt[remaval] = *ant_iter;
    ++remaval;
  }
    
  for(uInt j = 0; j < nant; ++j)
    antIndexer[selAnt[j]] = static_cast<Int>(j);
  return nant;
}

const ArrayColumn<Complex>& SubMS::right_column(const MSColumns *msclala,
                                                const MS::PredefinedColumns col)
{
  if(col == MS::DATA)
    return msclala->data();
  else if(col == MS::MODEL_DATA)
    return msclala->modelData();
  //  else if(col == MS::FLOAT_DATA) // Not complex.
  //  return msclala->floatData();
  else if(col == MS::LAG_DATA)
    return msclala->lagData();
  else                                // The honored-by-time-if-nothing-else
    return msclala->correctedData();  // default.
}

ArrayColumn<Complex>& SubMS::right_column(MSColumns *msclala,
					  const MS::PredefinedColumns col,
					  const Bool writeToDataCol)
{
  if(writeToDataCol || col == MS::DATA)
    return msclala->data();
  else if(col == MS::MODEL_DATA)
    return msclala->modelData();
  //  else if(col == MS::FLOAT_DATA) // Not complex.
  //  return msclala->floatData();
  else if(col == MS::LAG_DATA)
    return msclala->lagData();
  else                                // The honored-by-time-if-nothing-else
    return msclala->correctedData();  // default.
}

Bool SubMS::sepFloat(const Vector<MS::PredefinedColumns>& anyDataCols,
                     Vector<MS::PredefinedColumns>& complexDataCols)
{
  // Determine whether FLOAT_DATA is in anyDataCols[], and fill
  // complexDataCols[] with the Complex members of anyDataCols[].
  Bool doFloat = false;
  uInt ntok = anyDataCols.nelements();

  complexDataCols.resize(ntok);
  uInt j = 0;
  for(uInt i = 0; i < ntok; ++i){
    if(anyDataCols[i] != MS::FLOAT_DATA){
      complexDataCols[j] = anyDataCols[i];
      ++j;
    }
    else
      doFloat = true;
  }
  if(doFloat)
    complexDataCols.resize(j, true);
  return doFloat;
}

Bool SubMS::doTimeAver(const Vector<MS::PredefinedColumns>& dataColNames,
                       const VBRemapper& remapper)
{
  LogIO os(LogOrigin("SubMS", "doTimeAver()"));

  //No channel averaging with time averaging ... it's better this way, but
  //maybe that should be revisited with VisibilityIterator.
  if(chanStep_p[0] > 1){
    throw(AipsError("Simultaneous time and channel averaging is not handled."));
    return false;
  }

  os << LogIO::DEBUG1 // helpdesk ticket from Oleg Smirnov (ODU-232630)
     << "Before msOut_p.addRow(): "
     << Memory::allocatedMemoryInBytes() / (1024.0 * 1024.0) << " MB"
     << LogIO::POST;

  Vector<MS::PredefinedColumns> cmplxColLabels;
  const Bool doFloat = sepFloat(dataColNames, cmplxColLabels);
  const uInt nCmplx = cmplxColLabels.nelements();
  if(doFloat && cmplxColLabels.nelements() > 0)           // 2010-12-14
    os << LogIO::WARN
       << "Using VisibilityIterator to average both FLOAT_DATA and another DATA column is extremely experimental."
       << LogIO::POST;

  // Is CORRECTED_DATA being moved to DATA?
  Bool fromCorrToData = nCmplx == 1 && cmplxColLabels[0] == MS::CORRECTED_DATA;

  ArrayColumn<Complex> *outCmplxCols = new ArrayColumn<Complex>[nCmplx];
  getDataColMap(outCmplxCols, nCmplx, cmplxColLabels);

  // We may need to watch for chunks (timebins) that should be split because of
  // changes in scan, etc. (CAS-2401).  The old split way would have
  // temporarily shortened timeBin, but vi.setInterval() does not work without
  // calling vi.originChunks(), so that approach does not work with
  // VisibilityIterator.  Instead, get VisibilityIterator's sort (which also
  // controls how the chunks are split) to do the work.
  Block<Int> sort;
  if(!setSortOrder(sort, "spw", false))
    os << LogIO::WARN
       << "The request to combine spws while time averaging is being ignored."
       << LogIO::POST;

  // MSIter tends to produce output INTERVALs that are longer than the
  // requested interval length, by ~0.5 input integrations for a random
  // timeBin_p.  Giving it timeBin_p - 0.5 * interval[0] removes the bias and
  // brings it almost in line with binTimes() (which uses -0.5 *
  // interval[bin_start]).
  //
  // late April 2011: MSIter removed the bias, which threw off the correction.
  // October 2011: The bias seems to be back, possibly because of a change in
  //               when MSInterval's "offset" is reset.
  //               But, then, practically, timebins can be cut short but never
  //               lengthened by changes in scan, state, or obs ID, so it seems
  //               better to leave the bias in!
  //
  //Double tbin = mscIn_p->interval()(0);
  //tbin = timeBin_p > tbin ? timeBin_p - 0.5 * tbin : timeBin_p;
  ROVisibilityIterator vi(mssel_p, sort, timeBin_p);
  //vi.slurp();
  //cerr << "Finished slurping." << endl;

  // Apply selection
  for(uInt spwind = 0; spwind < spw_p.nelements(); ++spwind)
    vi.selectChannel(1, chanStart_p[spwind], nchan_p[spwind],
                     chanStep_p[spwind], spw_p[spwind]);
  //vi.selectChannel(chanSlices_p);     // ROVisIterator
  //vi.selectCorrelation(corrSlices_p);

  // Translate chanSlices_p into the form vb.channelAve() wants.
  Vector<Matrix<Int> > chanAveBounds;
  vi.slicesToMatrices(chanAveBounds, chanSlices_p, widths_p);

  Matrix<Float> wtmat;
  const Bool doSpWeight = vi.existsWeightSpectrum();

  //os << LogIO::NORMAL2 << "outNrow = " << msOut_p.nrow() << LogIO::POST;

  // All of this ddid/spw confusion really needs cleaning up.
  // a map from input to output
  // DATA_DESC_ID.  (ddidmap[input_ddid] = output_ddid.  Setting ddidmap to -1
  // for unselected ddids will help make it obvious if unexpected ddids sneak
  // through.)  Vector<Int> ddidmap(oldDDSpwMatch_p.nelements());
  // ddidmap.set(-1);
  // for(uInt i = 0; i < ddidmap.nelements(); ++i){
  //   Int oldspwid = oldDDSpwMatch_p[i];

  //   if(oldspwid > -1 && oldspwid < spwRelabel_p.nelements())
  //     ddidmap[i] = spwRelabel_p[oldspwid];
  // }

  uInt rowsdone = 0;    // Output rows, used for the RefRows.

  uInt ninrows = mssel_p.nrow();
  ProgressMeter meter(0.0, ninrows * 1.0, "split", "rows averaged", "", "",
		      true, 1);
  uInt inrowsdone = 0;  // only for the meter.

  VisChunkAverager vca(dataColNames, doSpWeight, chanAveBounds);

  Bool doFC = existsFlagCategory();

  // Iterate through the chunks.  A timebin will have multiple chunks if it has
  // > 1 arrays, fields, or ddids.
  for(vi.originChunks(); vi.moreChunks(); vi.nextChunk()){
    vca.reset();        // Should be done at the start of each chunk.

    inrowsdone += vi.nRowChunk();

    // Fill and time average vi's current chunk.
    VisBuffer& avb(vca.average(vi));
    uInt rowsnow = avb.nRow();

    if(rowsnow > 0){
      RefRows rowstoadd(rowsdone, rowsdone + rowsnow - 1);

      // msOut_p.addRow(rowsnow, true);
      msOut_p.addRow(rowsnow);            // Try it without initialization.

      // avb.freqAveCubes();  // Watch out, weight must currently be handled separately.

      remapper.remap(avb);

      // Fill in the nonaveraging values from slotv0.
      msc_p->antenna1().putColumnCells(rowstoadd, avb.antenna1());
      msc_p->antenna2().putColumnCells(rowstoadd, avb.antenna2());

      Vector<Int> arrID(rowsnow);
      arrID.set(avb.arrayId());                              // Don't remap!
      msc_p->arrayId().putColumnCells(rowstoadd, arrID);

      // outCmplxCols determines whether the input column is output to DATA or not.
      for(uInt datacol = 0; datacol < nCmplx; ++datacol){
        if(dataColNames[datacol] == MS::DATA)
          outCmplxCols[datacol].putColumnCells(rowstoadd, avb.visCube());
        else if(dataColNames[datacol] == MS::MODEL_DATA)
          outCmplxCols[datacol].putColumnCells(rowstoadd, avb.modelVisCube());
        else if(dataColNames[datacol] == MS::CORRECTED_DATA)
          outCmplxCols[datacol].putColumnCells(rowstoadd, avb.correctedVisCube());
      }
      if(doFloat)
        msc_p->floatData().putColumnCells(rowstoadd, avb.floatDataCube());

      Vector<Int> ddID(rowsnow);
      ddID.set(avb.dataDescriptionId());
      msc_p->dataDescId().putColumnCells(rowstoadd, ddID);

      msc_p->exposure().putColumnCells(rowstoadd, avb.exposure());
      msc_p->feed1().putColumnCells(rowstoadd, avb.feed1());
      msc_p->feed2().putColumnCells(rowstoadd, avb.feed2());

      Vector<Int> fieldID(rowsnow);
      fieldID.set(avb.fieldId());
      msc_p->fieldId().putColumnCells(rowstoadd, fieldID);

      msc_p->flagRow().putColumnCells(rowstoadd, avb.flagRow()); 
      msc_p->flag().putColumnCells(rowstoadd, avb.flagCube());

      if(doFC)
        msc_p->flagCategory().putColumnCells(rowstoadd, avb.flagCategory());

      msc_p->interval().putColumnCells(rowstoadd, avb.timeInterval());
      msc_p->observationId().putColumnCells(rowstoadd, avb.observationId());
      msc_p->processorId().putColumnCells(rowstoadd, avb.processorId());
      msc_p->scanNumber().putColumnCells(rowstoadd, avb.scan());   // Don't remap!

      if(doSpWeight)
        msc_p->weightSpectrum().putColumnCells(rowstoadd, avb.weightSpectrum());

      if(fromCorrToData) {                          
	wtmat.reference(avb.weightMat());
	msc_p->weight().putColumnCells(rowstoadd, wtmat);
        arrayTransformInPlace(wtmat, subms::wtToSigma);   // sig=1/sqrt(wt)
	msc_p->sigma().putColumnCells(rowstoadd, wtmat);
      }
      else {
        wtmat.reference(avb.sigmaMat());           // Yes, I'm reusing wtmat.
	msc_p->sigma().putColumnCells(rowstoadd, wtmat);
        arrayTransformInPlace(wtmat, subms::sigToWeight);   // wt=1/sig^2
	msc_p->weight().putColumnCells(rowstoadd, wtmat);
      }


      msc_p->stateId().putColumnCells(rowstoadd, avb.stateId());
      msc_p->time().putColumnCells(rowstoadd, avb.time());
      msc_p->timeCentroid().putColumnCells(rowstoadd, avb.timeCentroid());
      msc_p->uvw().putColumnCells(rowstoadd, avb.uvwMat());
      
      rowsdone += rowsnow;
    }
    meter.update(inrowsdone);
  }   // End of for(vi.originChunks(); vi.moreChunks(); vi.nextChunk())
  delete [] outCmplxCols;
  os << LogIO::NORMAL << "Data binned." << LogIO::POST;

  //const ColumnDescSet& cds = mssel_p.tableDesc().columnDescSet();
  //const ColumnDesc& cdesc = cds[MS::columnName(MS::DATA)];
  //ROTiledStManAccessor tacc(mssel_p, cdesc.dataManagerGroup());
  //tacc.showCacheStatistics(cerr);  // A 99.x% hit rate is good.  0% is bad.

  os << LogIO::DEBUG1 // helpdesk ticket in from Oleg Smirnov (ODU-232630)
     << "Post binning memory: " << Memory::allocatedMemoryInBytes() / (1024.0 * 1024.0) << " MB"
     << LogIO::POST;

  if(rowsdone < 1){
    os << LogIO::WARN
       << "No rows were written.  Is all the selected input flagged?"
       << LogIO::POST;
    return false;
  }
  return true;
}

// This should become the default soon (with a name change).
Bool SubMS::doTimeAverVisIterator(const Vector<MS::PredefinedColumns>& dataColNames,
                                  const VBRemapper& remapper)
{
  LogIO os(LogOrigin("SubMS", "doTimeAverVisIterator()"));

  //No channel averaging with time averaging ... it's better this way, but
  //maybe that should be revisited with VisibilityIterator.
  if(chanStep_p[0] > 1){
    throw(AipsError("Simultaneous time and channel averaging is not handled."));
    return false;
  }

  os << LogIO::DEBUG1 // helpdesk ticket from Oleg Smirnov (ODU-232630)
     << "Before msOut_p.addRow(): "
     << Memory::allocatedMemoryInBytes() / (1024.0 * 1024.0) << " MB"
     << LogIO::POST;

  Vector<MS::PredefinedColumns> cmplxColLabels;
  const Bool doFloat = sepFloat(dataColNames, cmplxColLabels);
  const uInt nCmplx = cmplxColLabels.nelements();
  if(doFloat && nCmplx > 0)                             // 2010-12-14
    os << LogIO::WARN
       << "Using VisIterator to average both FLOAT_DATA and another DATA column is extremely experimental."
       << LogIO::POST;

  // Is CORRECTED_DATA being moved to DATA?
  Bool fromCorrToData =  nCmplx == 1 && cmplxColLabels[0] == MS::CORRECTED_DATA;

  ArrayColumn<Complex> *outCmplxCols = new ArrayColumn<Complex>[nCmplx];
  getDataColMap(outCmplxCols, nCmplx, cmplxColLabels);

  // We may need to watch for chunks (timebins) that should be split because of
  // changes in scan, etc. (CAS-2401).  The old split way would have
  // temporarily shortened timeBin, but vi.setInterval() does not work without
  // calling vi.originChunks(), so that approach does not work with VisIterator.
  // Instead, get VisIterator's sort (which also controls how the chunks are split)
  // to do the work.
  Block<Int> sort;
  if(!setSortOrder(sort, "spw", false))
    os << LogIO::WARN
       << "The request to combine spws while time averaging is being ignored."
       << LogIO::POST;

  // MSIter tends to produce output INTERVALs that are longer than the
  // requested interval length, by ~0.5 input integrations for a random
  // timeBin_p.  Giving it timeBin_p - 0.5 * interval[0] removes the bias and
  // brings it almost in line with binTimes() (which uses -0.5 *
  // interval[bin_start]).
  //
  // late April 2011: MSIter removed the bias, which threw off the correction.
  // October 2011: The bias seems to be back, possibly because of a change in
  //               when MSInterval's "offset" is reset.
  //               But, then, practically, timebins can be cut short but never
  //               lengthened by changes in scan, state, or obs ID, so it seems
  //               better to leave the bias in!
  //
  //Double tbin = mscIn_p->interval()(0);
  //tbin = timeBin_p > tbin ? timeBin_p - 0.5 * tbin : timeBin_p;
  ROVisIterator vi(mssel_p, sort, timeBin_p);
  
  // Apply selection
  vi.selectChannel(chanSlices_p);     // ROVisIterator
  vi.selectCorrelation(corrSlices_p);

  // Translate chanSlices_p into the form vb.channelAve() wants.
  Vector<Matrix<Int> > chanAveBounds;
  vi.slicesToMatrices(chanAveBounds, chanSlices_p, widths_p);

  Matrix<Float> wtmat;
  const Bool doSpWeight = vi.existsWeightSpectrum();

  //os << LogIO::NORMAL2 << "outNrow = " << msOut_p.nrow() << LogIO::POST;

  // All of this ddid/spw confusion really needs cleaning up.
  // a map from input to output
  // DATA_DESC_ID.  (ddidmap[input_ddid] = output_ddid.  Setting ddidmap to -1
  // for unselected ddids will help make it obvious if unexpected ddids sneak
  // through.)  Vector<Int> ddidmap(oldDDSpwMatch_p.nelements());
  // ddidmap.set(-1);
  // for(uInt i = 0; i < ddidmap.nelements(); ++i){
  //   Int oldspwid = oldDDSpwMatch_p[i];

  //   if(oldspwid > -1 && oldspwid < spwRelabel_p.nelements())
  //     ddidmap[i] = spwRelabel_p[oldspwid];
  // }

  uInt rowsdone = 0;    // Output rows, used for the RefRows.

  uInt ninrows = mssel_p.nrow();
  ProgressMeter meter(0.0, ninrows * 1.0, "split", "rows averaged", "", "",
		      true, 1);
  uInt inrowsdone = 0;  // only for the meter.

  VisChunkAverager vca(dataColNames, doSpWeight, chanAveBounds);

  Bool doFC = existsFlagCategory();

  // Iterate through the chunks.  A timebin will have multiple chunks if it has
  // > 1 arrays, fields, or ddids.
  for(vi.originChunks(); vi.moreChunks(); vi.nextChunk()){
    vca.reset();        // Should be done at the start of each chunk.

    inrowsdone += vi.nRowChunk();

    // Fill and time average vi's current chunk.
    VisBuffer& avb(vca.average(vi));
    uInt rowsnow = avb.nRow();

    if(rowsnow > 0){
      RefRows rowstoadd(rowsdone, rowsdone + rowsnow - 1);

      // msOut_p.addRow(rowsnow, true);
      msOut_p.addRow(rowsnow);            // Try it without initialization.
        
      // avb.freqAveCubes();  // Watch out, weight must currently be handled
      // separately.

      remapper.remap(avb);

      // Fill in the nonaveraging values from slotv0.
      msc_p->antenna1().putColumnCells(rowstoadd, avb.antenna1());
      msc_p->antenna2().putColumnCells(rowstoadd, avb.antenna2());

      Vector<Int> arrID(rowsnow);
      arrID.set(avb.arrayId());                              // Don't remap!
      msc_p->arrayId().putColumnCells(rowstoadd, arrID);

      // outCmplxCols determines whether the input column is output to DATA or not.
      for(uInt datacol = 0; datacol < nCmplx; ++datacol){
        if(dataColNames[datacol] == MS::DATA)
          outCmplxCols[datacol].putColumnCells(rowstoadd, avb.visCube());
        else if(dataColNames[datacol] == MS::MODEL_DATA)
          outCmplxCols[datacol].putColumnCells(rowstoadd, avb.modelVisCube());
        else if(dataColNames[datacol] == MS::CORRECTED_DATA)
          outCmplxCols[datacol].putColumnCells(rowstoadd, avb.correctedVisCube());
      }
      if(doFloat)
        msc_p->floatData().putColumnCells(rowstoadd, avb.floatDataCube());

      Vector<Int> ddID(rowsnow);
      ddID.set(avb.dataDescriptionId());
      msc_p->dataDescId().putColumnCells(rowstoadd, ddID);

      msc_p->exposure().putColumnCells(rowstoadd, avb.exposure());
      msc_p->feed1().putColumnCells(rowstoadd, avb.feed1());
      msc_p->feed2().putColumnCells(rowstoadd, avb.feed2());

      Vector<Int> fieldID(rowsnow);
      fieldID.set(avb.fieldId());
      msc_p->fieldId().putColumnCells(rowstoadd, fieldID);

      msc_p->flagRow().putColumnCells(rowstoadd, avb.flagRow()); 
      msc_p->flag().putColumnCells(rowstoadd, avb.flagCube());

      if(doFC)
        msc_p->flagCategory().putColumnCells(rowstoadd, avb.flagCategory());

      msc_p->interval().putColumnCells(rowstoadd, avb.timeInterval());
      msc_p->observationId().putColumnCells(rowstoadd, avb.observationId());
      msc_p->processorId().putColumnCells(rowstoadd, avb.processorId());
      msc_p->scanNumber().putColumnCells(rowstoadd, avb.scan());	// Don't remap!

      if(doSpWeight)
        msc_p->weightSpectrum().putColumnCells(rowstoadd, avb.weightSpectrum());

      if(fromCorrToData) {
	wtmat.reference(avb.weightMat());
	msc_p->weight().putColumnCells(rowstoadd, wtmat);
        arrayTransformInPlace(wtmat, subms::wtToSigma);  // sig=1/sqrt(wt)
	msc_p->sigma().putColumnCells(rowstoadd, wtmat);
      }
      else {
        wtmat.reference(avb.sigmaMat());           // Yes, I'm reusing wtmat.     
	msc_p->sigma().putColumnCells(rowstoadd, wtmat);
        arrayTransformInPlace(wtmat, subms::sigToWeight);  // wt = 1/sig^2
	msc_p->weight().putColumnCells(rowstoadd, wtmat);
      }

      msc_p->stateId().putColumnCells(rowstoadd, avb.stateId());
      msc_p->time().putColumnCells(rowstoadd, avb.time());
      msc_p->timeCentroid().putColumnCells(rowstoadd, avb.timeCentroid());
      msc_p->uvw().putColumnCells(rowstoadd, avb.uvwMat());
      
      rowsdone += rowsnow;
    }
    meter.update(inrowsdone);
  }   // End of for(vi.originChunks(); vi.moreChunks(); vi.nextChunk())
  delete [] outCmplxCols;
  os << LogIO::NORMAL << "Data binned." << LogIO::POST;

  os << LogIO::DEBUG1 // helpdesk ticket in from Oleg Smirnov (ODU-232630)
     << "Post binning memory: " << Memory::allocatedMemoryInBytes() / (1024.0 * 1024.0) << " MB"
     << LogIO::POST;

  if(rowsdone < 1){
    os << LogIO::WARN
       << "No rows were written.  Is all the selected input flagged?"
       << LogIO::POST;
    return false;
  }
  return true;
}

void SubMS::getDataColMap(MSColumns* msc, ArrayColumn<Complex>* mapper,
			  uInt ntok,
                          const Vector<MS::PredefinedColumns>& colEnums)
{
  // Set up a map from dataColumn indices to ArrayColumns in the output.
  // mapper has to be a pointer (gasp!), not a Vector, because
  // Vector<ArrayColumn<Complex> > mapper(ntok) would implicitly call
  // .resize(), which uses =, which is banned for ArrayColumn.

  if(mustConvertToData(ntok, colEnums)){
    mapper[0].reference(msc->data());
  }
  else{
    for(uInt i = 0; i < ntok; ++i){
      if(colEnums[i] == MS::CORRECTED_DATA)
        mapper[i].reference(msc->correctedData());
      else if(colEnums[i] == MS::MODEL_DATA)
        mapper[i].reference(msc->modelData());
      else if(colEnums[i] == MS::LAG_DATA)
        mapper[i].reference(msc->lagData());
      else                                  // The output default !=
        mapper[i].reference(msc->data()); // the input default.
    }
  }
}

inline Bool SubMS::areDataShapesConstant()
{
  return allSame(inNumChan_p) && allSame(nchan_p) && allSame(inNumCorr_p) && allSame(ncorr_p);
}

  Bool isAllColumns(const Vector<MS::PredefinedColumns>& colNames)
  {
    Bool dCol=false, mCol=false, cCol=false;
    for(uInt i=0;i<colNames.nelements();i++)
      {
	if (colNames[i] == MS::DATA)                dCol=true;
	else if (colNames[i] == MS::MODEL_DATA)     mCol=true;
	else if (colNames[i] == MS::CORRECTED_DATA) cCol=true;
        // else turn off all?
      }
    return (dCol && mCol && cCol);
  }

  // -----------------------------------------------------------------------
  // Work-around to copy the keywords of the FLOAT_DATA column to the output MS
  // -----------------------------------------------------------------------
  void SubMS::copyMainTableKeywords (TableRecord& outKeys,
  		const TableRecord& inKeys)
  {
  	for (uInt i=0; i<inKeys.nfields(); i++) {
  		if (inKeys.type(i) == TpString) {
  			// Add keywords for MAIN table columns such as FLOAT_DATA
  			String ikey = inKeys.name(i);
  			if (!outKeys.isDefined (ikey)) {
  				String keyval;
  				inKeys.get(ikey, keyval);
  				outKeys.define(ikey,keyval);
  			}

  		}

  	}
  }

} //#End casa namespace
