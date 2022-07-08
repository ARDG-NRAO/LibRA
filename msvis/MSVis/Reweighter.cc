//# Reweighter.cc 
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

#include <msvis/MSVis/Reweighter.h>
#include <casacore/ms/MSSel/MSSelection.h>
//#include <ms/MSSel/MSTimeGram.h>
//#include <tables/TaQL/ExprNode.h>
#include <casacore/tables/Tables/RefRows.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/Cube.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/ArrayLogical.h>
#include <casacore/casa/Arrays/ArrayUtil.h>
#include <casacore/casa/Arrays/IPosition.h>
#include <casacore/casa/Arrays/Slice.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/OS/File.h>
#include <casacore/casa/OS/HostInfo.h>
#include <casacore/casa/OS/Memory.h>              // Can be commented out along with
//                                         // Memory:: calls.

//#ifdef COPYTIMER
#include <casacore/casa/OS/Timer.h>
//#endif

#include <casacore/casa/Containers/Record.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/Utilities/GenSort.h>
#include <casacore/casa/System/AppInfo.h>
#include <casacore/casa/System/ProgressMeter.h>
#include <casacore/casa/Quanta/QuantumHolder.h>
#include <msvis/MSVis/GroupProcessor.h>
//#include <msvis/MSVis/VisSet.h>
#include <msvis/MSVis/StatWT.h>
#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/VisBufferComponents.h>
#include <msvis/MSVis/VisChunkAverager.h>
#include <msvis/MSVis/VisIterator.h>
//#include <msvis/MSVis/VisibilityIterator.h>
#include <casacore/tables/DataMan/IncrementalStMan.h>
#include <casacore/tables/Tables/ScalarColumn.h>
#include <casacore/tables/Tables/ScaColDesc.h>
#include <casacore/tables/DataMan/StandardStMan.h>
#include <casacore/tables/Tables/Table.h>
#include <casacore/tables/Tables/PlainTable.h>
#include <casacore/tables/Tables/TableDesc.h>
#include <casacore/tables/Tables/TableInfo.h>
#include <casacore/tables/Tables/TableLock.h>
#include <casacore/tables/Tables/TableRecord.h>
#include <casacore/tables/Tables/TableCopy.h>
#include <casacore/tables/Tables/TableRow.h>
#include <casacore/tables/DataMan/TiledColumnStMan.h>
#include <casacore/tables/DataMan/TiledShapeStMan.h>
#include <casacore/tables/DataMan/TiledDataStMan.h>
#include <casacore/tables/DataMan/TiledStManAccessor.h>
#include <casacore/ms/MeasurementSets/MSTileLayout.h>
#include <casa/sstream.h>
#include <casa/iomanip.h>
#include <functional>
#include <map>
#include <set>
#include <casacore/scimath/Mathematics/Smooth.h>
#include <casacore/casa/Quanta/MVTime.h>

using namespace casacore;
namespace casa {

typedef ROVisibilityIterator ROVisIter;
typedef VisibilityIterator VisIter;

Reweighter::Reweighter(const String& theMS, const Bool dorms, const uInt minsamp) :
  ms_p(MeasurementSet(theMS, Table::Update)),
  mssel_p(ms_p),
  dorms_p(dorms),
  minsamp_p(minsamp),
  msc_p(NULL),
  antennaSel_p(false),
  timeBin_p(-1.0),
  scanString_p(""),
  intentString_p(""),
  obsString_p(""),
  timeRange_p(""),
  arrayExpr_p(""),
  combine_p(""),
  fitspw_p("*"),
  outspw_p("*")
{
}
  
Reweighter::~Reweighter()
{
  delete msc_p;
  msc_p = NULL;
}

Bool Reweighter::selectSpw(std::set<Int>& spwset, Vector<Int>& chanStartv,
                           Vector<Int>& chanEndv, Vector<Int>& chanStepv,
                           const String& spwstr)
{
  LogIO os(LogOrigin("Reweighter", "selectSpw()"));

  MSSelection mssel;
  String myspwstr(spwstr == "" ? "*" : spwstr);

  mssel.setSpwExpr(myspwstr);

  // Each row should have spw, start, stop, step
  // A single width is a default, but multiple widths should be used
  // literally.
  Matrix<Int> chansel = mssel.getChanList(&ms_p, 1);

  if(chansel.nrow() > 0) {         // Use myspwstr if it selected anything...
    Vector<Int> spwv(chansel.column(0));

    uInt nspwsel = spwv.size();
    chanStartv.resize(nspwsel);
    chanEndv.resize(nspwsel);
    chanStepv.resize(nspwsel);

    chanStartv = chansel.column(1);
    chanEndv   = chansel.column(2);
    chanStepv  = chansel.column(3);

    uInt nspw = chanEndv.nelements();

    for(uInt k = 0; k < nspw; ++k){
      spwset.insert(spwv[k]);

      if(chanStepv[k] == 0)	// CAS-2224, triggered by spw='0:2'
        chanStepv[k] = 1;	// (as opposed to '0:2~2').
	
      //nchan[k] = 1 + (chanEndv[k] - chanStartv[k]) / chanStepv[k];
      //if(nchan[k] < 1)
      //  nchan[k] = 1;
    }
  }
  else{                            // select everything and rely on widths.
    MSSpWindowColumns mySpwTab(ms_p.spectralWindow());
    uInt nspw = mySpwTab.nrow();
    Vector<Int> nchan(nspw);

    nchan = mySpwTab.numChan().getColumn();
            
    chanStartv.resize(nspw);
    chanStepv.resize(nspw);
    for(uInt k = 0; k < nspw; ++k){
      spwset.insert(k);      
      chanStartv[k] = 0;
      chanEndv[k]   = nchan[k] - 1;
      chanStepv[k]  = 1;
    }
  }
    
  mssel.getChanSlices(chanSlices_p, &ms_p, 1);
  return true;
}

Bool Reweighter::selectCorrelations(const String& corrstr)
{
  LogIO os(LogOrigin("Reweighter", "selectCorrelations()"));
  MSSelection mssel;
  const Bool areSelecting = corrstr != "" && corrstr != "*";

  if(areSelecting)
    mssel.setPolnExpr(corrstr);
  mssel.getCorrSlices(corrSlices_p, &ms_p);
  //return getCorrTypes(polIDs_p, corrTypes_p, mssel, ms_p, areSelecting);
  return areSelecting;
}

Bool Reweighter::getCorrTypes(Vector<Int>& polIDs,
                              Vector<Vector<Int> >& corrTypes,
                              const MSColumns& msc)
{
  Bool cando = true;

  polIDs = msc.dataDescription().polarizationId().getColumn();
  corrTypes = msc.polarization().corrType().getColumn();
  return cando;
}

Bool Reweighter::setmsselect(const String& fitspw, const String& outspw,
                             const String& field,  const String& baseline,
                             const String& scan,   const String& subarray,
                             const String& correlation, const String& intent,
                             const String& obs)
{
  LogIO os(LogOrigin("Reweighter", "setmsselect()"));
  Bool  ok;
    
  // All of the requested selection functions will be tried, even if an
  // earlier one has indicated its failure.  This allows all of the selection
  // strings to be tested, yielding more complete feedback for the user
  // (fewer retries).  This is a matter of taste, though.  If the selections
  // turn out to be slow, this function should return on the first false.

  // Do the output spw selection first because the channel vectors are dummies;
  // for output we only care about the spws.
  if(!selectSpw(outspwset_p, fitStart_p, fitEnd_p, fitStep_p, outspw)){
    os << LogIO::SEVERE
       << "No spectral windows selected for reweighting."
       << LogIO::POST;
    ok = false;
  }
  chanSlices_p.resize(0);
  // Check for : in outspw, and warn about it, at the Python level.

  if(!selectSpw(fitspwset_p, fitStart_p, fitEnd_p, fitStep_p, fitspw)){
    os << LogIO::SEVERE
       << "No channels selected for calculating the scatter."
       << LogIO::POST;
    ok = false;
  }
  Record selrec = ms_p.msseltoindex(outspw, field);
  ok = selectSource(selrec.asArrayInt("field"));

  if(baseline != ""){
    Vector<Int> antid(0);
    Vector<String> antstr(1,baseline);
    selectAntenna(antid, antstr);
  }
  scanString_p    = scan;
  intentString_p  = intent;
  obsString_p     = obs;

  if(subarray != "")
    selectArray(subarray);

  if(!selectCorrelations(correlation)){
    //os << LogIO::SEVERE << "No correlations selected." << LogIO::POST;
    //For now, false here means no selection== use all available correlations
    //so no nead to raise an error.
    //ok = false;
  }

  return ok;
}

Bool Reweighter::selectSource(const Vector<Int>& fieldid)
{
  LogIO os(LogOrigin("Reweighter", "selectSource()"));
  Bool cando = true;

  if(fieldid.nelements() < 1){
    fieldId_p = Vector<Int>(1, -1);
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
    fieldId_p = fieldid;
  }

  if(fieldId_p.nelements() == 1 && fieldId_p[0] < 0){
    fieldId_p.resize(ms_p.field().nrow());
    indgen(fieldId_p);
  }
  return cando;
}

MS::PredefinedColumns Reweighter::dataColStrToEnum(const String& col)
{
  LogIO os(LogOrigin("Reweighter", "dataColStrToEnum()"));
  String tmpName(col);

  tmpName.upcase();

  MS::PredefinedColumns result = MS::DATA;
	    
  if(tmpName == "OBSERVED" || tmpName == "DATA" || tmpName == MS::columnName(MS::DATA))
    result = MS::DATA;
  else if(tmpName == "FLOAT" || tmpName == "FLOAT_DATA" || tmpName == MS::columnName(MS::FLOAT_DATA))
    result = MS::FLOAT_DATA;
  else if(tmpName == "LAG" || tmpName == "LAG_DATA" || tmpName == MS::columnName(MS::LAG_DATA))
    result = MS::LAG_DATA;
  else if(tmpName == "MODEL" || tmpName == "MODEL_DATA" || tmpName == MS::columnName(MS::MODEL_DATA))
    result = MS::MODEL_DATA;
  else if(tmpName == "CORRECTED" || tmpName == "CORRECTED_DATA" || 
          tmpName == MS::columnName(MS::CORRECTED_DATA))
    result = MS::CORRECTED_DATA;
  else
    os << LogIO::WARN
       << "Unrecognized data column " << tmpName << "...using DATA."
       << LogIO::POST;
  return result;
}

void Reweighter::selectTime(Double timeBin, String timerng)
{  
  timeBin_p   = timeBin;
  timeRange_p = timerng;
}  
  
Bool Reweighter::reweight(String& colname, const String& combine)
{
    //Bool retval = true;
  LogIO os(LogOrigin("Reweighter", "reweight()"));

  try{
    if(!makeSelection()){
      os << LogIO::SEVERE 
         << "Failed on selection: the combination of spw, field, antenna, correlation, "
         << "and timerange may be invalid." 
         << LogIO::POST;
      ms_p=MeasurementSet();
      return false;
    }
    msc_p = new MSColumns(mssel_p);
    // Note again the parseColumnNames() a few lines back that stops setupMS()
    // from being called if the MS doesn't have the requested columns.
      
    combine_p = combine;

    //Detaching the selected part
    ms_p = MeasurementSet();

    Block<Int> sort;
    if(!setSortOrder(sort, "obs,scan,state", false)){
      os << LogIO::WARN
         << "Something in the combine string is not supported for reweighting."
         << LogIO::POST;
    }

    // Aaargh...everywhere else VisIter is used a timeInterval of 0 is treated as
    // DBL_MAX, meaning that TIME can be in sort but effectively be ignored for
    // major chunking.  Why couldn't they just have said DBL_MAX in the first
    // place?
    ROVisibilityIterator viIn(mssel_p, sort, false, DBL_MIN);

    // Make sure it is initialized before any copies are made.
    viIn.originChunks();


    // NB: The following correlation selection---which doesn't work
    //  in multi-corrshape datasets---has been rendered meaningless
    //  within StatWT.  In general, it is probably not desirable to
    //  make the weight disposition correlation-dependent, in any
    //  case.  As such, correlation selection has been disabled
    //  in the user interface, too.   (gmoellen, 2015Aug12)

    // Make a list of indices of selected correlations
    vector<uInt> selcorrs;
    uInt nSelCorrs=corrSlices_p[0].nelements();
    if (nSelCorrs>0) {
     for (uInt i = 0; i < nSelCorrs; i++) {
         Slice slice=corrSlices_p[0][i];
         selcorrs.push_back(slice.start());
     }
    }
    else { // no selection == all 
      for (Int i = 0; i < viIn.nCorr(); i++) selcorrs.push_back(static_cast<uInt>(i));
    } 

    StatWT statwt(viIn, dataColStrToEnum(colname), fitspw_p, outspw_p, dorms_p,
         minsamp_p,selcorrs);
    GroupProcessor gp(viIn, &statwt);

    gp.go();

    // There should be now be statistically determined sigmas and weights for
    // each selected row.  If smoothing is wanted, i.e. convolving by a
    // gaussian in time (taking the ends into account), do it now with a pass
    // over just WEIGHT, SIGMA, and the flags.
    //
    // In theory two passes is a little less accurate than gathering all the
    // data for a time window and then calculating the variance, but I show
    // here that the loss of accuracy is very small at worst, and in practice
    // two passes is much better.
    //
    // The variance of \sigma^2 is \sigma^4 (\frac{2}{n - 1} + \frac{k}{n}),
    // where the kurtosis k is 0 for a normal distribution.  It will be dropped
    // from here on since it does not affect the argument.
    //
    // So for a sample of mn visibilities (gather up all the data for a time
    // window before calculating the variances), the variance of \sigma^2 is
    // 2 \sigma^4 / (mn - 1).
    //
    // If the variance of the time window is instead calculated from the
    // variances of m groups of size n, the variance of the variance is
    // 2 \sigma^4 / [m (n - 1)]
    //
    // Thus there is a difference, but a small one if n >> 1 (i.e. a reasonable
    // number of channels).  More importantly, the m groups of n method
    //  * is robust against the n groups having different means.
    //    The only likely way that all the groups would have exactly the same
    //    mean is if the mean is 0, in which case rms should be used instead of
    //    stddev and the relevant sample size is simply mn no matter how
    //    they're grouped.  (I think; untested)
    //  * m groups of n is a lot more flexible for programming and needs less
    //    memory.  It does mean some extra I/O for another pass, but it's only
    //    over WEIGHT, SIGMA, FLAG_ROW, and unfortunately FLAG.
    
    return true;
  }
  catch(AipsError x){
    ms_p = MeasurementSet();
    throw(x);
  }
  catch(...){
    ms_p = MeasurementSet();
    throw(AipsError("Unknown exception caught"));
  }
}

void Reweighter::makeUnionSpw()
{
  std::set<Int> unionset = fitspwset_p;
  std::set<Int>::iterator it;
  uInt i = 0;

  for(it = outspwset_p.begin(); it != outspwset_p.end(); ++it)
    unionset.insert(*it);
  unionspw_p.resize(unionset.size());

  for(it = unionset.begin(); it != unionset.end(); ++it)
    unionspw_p[i++] = *it;
}

Bool Reweighter::makeSelection()
{    
  LogIO os(LogOrigin("Reweighter", "makeSelection()"));
    
  //VisSet/MSIter will check if the SORTED exists
  //and resort if necessary
  {
    // Matrix<Int> noselection;
    // VisSet vs(ms_p, noselection);
    Block<Int> sort;
    ROVisibilityIterator(ms_p, sort);
  }
   
  const MeasurementSet *elms;
  elms = &ms_p;
  MeasurementSet sorted;
  if(ms_p.keywordSet().isDefined("SORTED_TABLE")){
    sorted = ms_p.keywordSet().asTable("SORTED_TABLE");
    //If ms is not writable and sort is a subselection...use original ms
    if( ms_p.nrow() == sorted.nrow())
      elms = &sorted;
  }
 
  MSSelection thisSelection;
  if(fieldId_p.nelements() > 0)
    thisSelection.setFieldExpr(MSSelection::indexExprStr(fieldId_p));
  if(unionspw_p.nelements() > 0)
    thisSelection.setSpwExpr(MSSelection::indexExprStr(unionspw_p));
  if(antennaSel_p){
    if(antennaId_p.nelements() > 0){
      thisSelection.setAntennaExpr(MSSelection::indexExprStr( antennaId_p ));
    }
    if(antennaSelStr_p[0] != "")
      thisSelection.setAntennaExpr(MSSelection::nameExprStr( antennaSelStr_p));
  }
  if(timeRange_p != "")
    thisSelection.setTimeExpr(timeRange_p);
    
  thisSelection.setScanExpr(scanString_p);
  thisSelection.setStateExpr(intentString_p);
  thisSelection.setObservationExpr(obsString_p);
  if(arrayExpr_p != "")
    thisSelection.setArrayExpr(arrayExpr_p);
  if(corrString_p != "")
    thisSelection.setPolnExpr(corrString_p);
    
  TableExprNode exprNode=thisSelection.toTableExprNode(elms);
  selTimeRanges_p = thisSelection.getTimeList();

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

    Bool trivial = true;
    for(uInt k = 0; k < nAnts; ++k)
      trivial &= (selAnts[k] == static_cast<Int>(k));   // trivial = selAnts == indgen(nAnts)
                                       // It is possible to exclude baselines
    antennaSel_p = !trivial;           // without excluding any antennas.
  }                                    // This still gets tripped up by VLA:OUT.
   
  if(mssel_p.nrow() < ms_p.nrow()){
    os << LogIO::NORMAL
       << mssel_p.nrow() << " out of " << ms_p.nrow() << " rows are going to be" 
       << " considered due to the selection criteria." 
       << LogIO::POST;
  }
  return true;
}

Bool Reweighter::shouldWatch(Bool& conflict, const String& col,
                             const String& uncombinable,
                             const Bool verbose) const
{
  Bool wantWatch = !combine_p.contains(col);

  if(!wantWatch && uncombinable.contains(col)){
    conflict = true;
    wantWatch = false;

    if(verbose){
      LogIO os(LogOrigin("Reweighter", "shouldWatch()"));

      os << LogIO::WARN
         << "Combining by " << col
         << " was requested, but it is not allowed by this operation and will be ignored."
         << LogIO::POST;
    }
  }
  return wantWatch;
}

Bool Reweighter::setSortOrder(Block<Int>& sort, const String& uncombinable,
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

const ArrayColumn<Complex>& Reweighter::right_column(const MSColumns *msclala,
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

} //#End casa namespace
