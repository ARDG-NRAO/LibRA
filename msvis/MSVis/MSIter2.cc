//# MSIter2.cc: Implementation of MSIter2.h
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
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

#include <msvis/MSVis/MSIter2.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/ArrayLogical.h>
#include <casacore/casa/Arrays/Slice.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/tables/Tables/TableIter.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/Utilities/BinarySearch.h>
#include <casacore/casa/Utilities/GenSort.h>
#include <casacore/casa/Arrays/Slicer.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <casacore/ms/MSSel/MSSpwIndex.h>
#include <casacore/measures/Measures.h>
#include <casacore/measures/Measures/MeasTable.h>
#include <casacore/measures/Measures/MPosition.h>
#include <casacore/measures/Measures/MEpoch.h>
#include <casa/Quanta/MVTime.h>
#include <casacore/measures/Measures/Stokes.h>
#include <casacore/tables/Tables/TableRecord.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/iostream.h>

using namespace casacore;

namespace casa { //# NAMESPACE CASA - BEGIN
namespace vi {  //# NAMESPACE VI - BEGIN
 
MSSmartInterval::MSSmartInterval(Double interval,
				 Vector<Double>& timebounds) :
  MSInterval(interval),
  interval2_(interval),
  offset2_(0.0),
  timeBounds_(timebounds),
  nBounds_(timebounds.nelements()),
  zeroInterval_(abs(interval)<(2.0*DBL_MIN)),   // NB: DBL_MIN is smallest positive
  found_(false),
  bidx_(0)
{

  // If timebounds unspecified, set one element to zero
  //   In this case, we are gridding time globally
  if (timeBounds_.nelements()==0) {
    timeBounds_.resize(1);
    timeBounds_(0)=0.0;
  }
  // Initialize offset2_ to the first timeBounds_
  offset2_=timeBounds_(0);
}

void MSSmartInterval::setOffset(Double offset)
{
  // Used only for absolute reset of the offset w.r.t.
  //  the bounts list
  if (offset==0.0) {
    bidx_=0;
    offset2_=timeBounds_(0);
  }
}

int MSSmartInterval::comp(const void * obj1, const void * obj2) const
{
  double v1 = *(const Double*)obj1;
  double v2 = *(const Double*)obj2;

  //  cout.precision(6);
  //  cout << " " << Float(v1-3600.0*floor(v1/3600.0)) << " ";

  // Shortcut if values are equal.
  //   In synthesis MSs, this captures most row comparisons, luckily!
  if (v1 == v2) return 0;

  // Shortcut if interval is trivially small, or supplied times
  //   differ by more than the interval
  //   TBD: move zeroInterval_ context to own function 
  //        and point to it (handle v1==v2 also)?
  if (zeroInterval_ || abs(v1-v2)>interval2_)
    return v1 < v2 ? -1 : 1;

  // Reaching here, v1 and v2 differ by less than the inverval_, 
  //  so work harder to discern if they are in the same interval,
  //  with attention to where the specified timeBounds_ are.

  // Find the reference time for the 2nd specified value
  //  This costs, at worst, nBounds_*log(nBounds_) _each_ execution,
  //   which could be problematic....
  if (nBounds_>1) {
    if (v2<timeBounds_[bidx_]) {
      // search from beginning, 
      bidx_=binarySearch(found_,timeBounds_,v2,nBounds_,0);
      if (!found_) --bidx_;  // handle exact match
    }
    else if (((bidx_+1)<nBounds_) &&      // a forward bound available
	     (v2>timeBounds_[bidx_+1])) { // value is beyond it
      // search from current position
      bidx_=binarySearch(found_,timeBounds_,v2,nBounds_-bidx_,bidx_);
      if (!found_) --bidx_;  // handle exact match
    }
    // else current bidx_ is correct

    // Offset boundary identified
    offset2_=timeBounds_[bidx_];
    
    // If v1 is prior to the detected boundary...
    if (v1 < offset2_) return -1;
    
    // If v1 later than the next higher boundary...
    if ( ((bidx_+1)<nBounds_) &&      // a forward bound available   
	 (v1 > timeBounds_[bidx_+1]) ) return 1;
  }
  else {
    // this should match initialization...
    bidx_=0;
    offset2_=timeBounds_[0];
  }

  // Reaching here, both values are in the same timeBounds_ interval;
  // As last resort, determine if v1 and v2 are in different gridded
  //  intervals within the current timeBounds_ interval

  // The times are binned in bins with a width of interval_.
  t1_ = floor((v1 - offset2_) / interval2_);
  t2_ = floor((v2 - offset2_) / interval2_);

  return (t1_==t2_ ? 0 : (t1_<t2_ ? -1 : 1));
}


MSIter2::MSIter2():MSIter() {}

MSIter2::MSIter2(const MeasurementSet& ms,
		 const Block<Int>& sortColumns,
		 Double timeInterval,
		 Bool addDefaultSortColumns,
		 Bool storeSorted)
  : MSIter(ms,sortColumns,timeInterval,addDefaultSortColumns,storeSorted)
{}


MSIter2::MSIter2(const Block<MeasurementSet>& mss,
		 const Block<Int>& sortColumns,
		 Double timeInterval,
		 Bool addDefaultSortColumns,
		 Bool storeSorted)
  : MSIter()
{
  bms_p=mss;
  curMS_p=0;
  lastMS_p=-1;
  storeSorted_p=storeSorted;
  interval_p=timeInterval;
  prevFirstTimeStamp_p = -1.0;
  this->construct2(sortColumns,addDefaultSortColumns);

}

void MSIter2::origin()
{
  // Reset time comparer, if present
  //  (Ensures repeatability!)
  if (timeComp_p) timeComp_p->setOffset(0.0);

  // Call conventional
  MSIter::origin();
}



void MSIter2::construct2(const Block<Int>& sortColumns, 
			 Bool addDefaultSortColumns)
{
  This = (MSIter2*)this; 
  nMS_p=bms_p.nelements();
  if (nMS_p==0) throw(AipsError("MSIter::construct -  No input MeasurementSets"));
  for (size_t i=0; i<nMS_p; i++) {
    if (bms_p[i].nrow()==0) {
      throw(AipsError("MSIter::construct - Input MeasurementSet.has zero selected rows"));
    }
  }
  tabIter_p.resize(nMS_p);
  tabIterAtStart_p.resize(nMS_p);
  // 'sort out' the sort orders
  // We normally require the table to be sorted on ARRAY_ID and FIELD_ID,
  // DATA_DESC_ID and TIME for the correct operation of the
  // VisibilityIterator (it needs to know when any of these changes to
  // be able to give the correct coordinates with the data)
  // If these columns are not explicitly sorted on, they will be added
  // BEFORE any others, unless addDefaultSortColumns=False

  Block<Int> cols; 
  // try to reuse the existing sorted table if we didn't specify
  // any sortColumns
  if (sortColumns.nelements()==0 && 
      bms_p[0].keywordSet().isDefined("SORT_COLUMNS")) {
    // note that we use the order of the first MS for all MS's
    Vector<String> colNames = bms_p[0].keywordSet().asArrayString("SORT_COLUMNS");
    uInt n=colNames.nelements();
    cols.resize(n);
    for (uInt i=0; i<n; i++) cols[i]=MS::columnType(colNames(i));
  } else {
    cols=sortColumns;
  }

  timeInSort_p=False, arrayInSort_p=False, ddInSort_p=False, fieldInSort_p=False;
  bool scanSeen = false;
  Int nCol=0;
  for (uInt i=0; i<cols.nelements(); i++) {
    if (cols[i]>0 && 
	cols[i]<MS::NUMBER_PREDEFINED_COLUMNS) {
      if (cols[i]==MS::ARRAY_ID && !arrayInSort_p) { arrayInSort_p=True; nCol++; }
      if (cols[i]==MS::FIELD_ID && !fieldInSort_p) { fieldInSort_p=True; nCol++; }
      if (cols[i]==MS::DATA_DESC_ID && !ddInSort_p) { ddInSort_p=True; nCol++; }
      if (cols[i]==MS::TIME && !timeInSort_p) { timeInSort_p=True; nCol++; }
      if (cols[i]==MS::SCAN_NUMBER && !scanSeen) { scanSeen=True; }
    } else {
      throw(AipsError("MSIter() - invalid sort column"));
    }
  }
  Block<String> columns;
  
  Int iCol=0;
  if (addDefaultSortColumns) {
    columns.resize(cols.nelements()+4-nCol);
    if (!arrayInSort_p) {
      // add array if it's not there
      columns[iCol++]=MS::columnName(MS::ARRAY_ID);
    }
    if (!fieldInSort_p) {
      // add field if it's not there
      columns[iCol++]=MS::columnName(MS::FIELD_ID);
    }
    if (!ddInSort_p) {
      // add dd if it's not there
      columns[iCol++]=MS::columnName(MS::DATA_DESC_ID);
    }
    if (!timeInSort_p) {
      // add time if it's not there
      columns[iCol++]=MS::columnName(MS::TIME);
      timeInSort_p = True;
    }
  } else {
    columns.resize(cols.nelements());
  }
  for (uInt i=0; i<cols.nelements(); i++) {
    columns[iCol++]=MS::columnName(MS::PredefinedColumns(cols[i]));
  }

  if (interval_p==0.0) {
    interval_p=DBL_MAX; // semi infinite
  } else {
    // assume that we want to sort on time if interval is set
    if (!timeInSort_p) {
      columns.resize(columns.nelements()+1);
      columns[iCol++]=MS::columnName(MS::TIME);
    }
  }
  
  // now find the time column and set the compare function
  Block<CountedPtr<BaseCompare> > objComp(columns.nelements());
  Block<Int> sortOrd(columns.nelements());
  timeComp_p = 0;
  Bool fieldBounded(false);
  Bool scanBounded=scanSeen;
  for (uInt i=0; i<columns.nelements(); i++) {
    
    // Meaningful if this occurs before TIME
    if (columns[i]==MS::columnName(MS::FIELD_ID)) fieldBounded=true;

    if (columns[i]==MS::columnName(MS::TIME)) {

      Vector<Double> timebounds(0);
      //this->discernEnforcedTimeBounds(timebounds,scanBounded);
      //this->discernEnforcedTimeBounds(timebounds,scanBounded,fieldBounded);
      this->discernEnforcedTimeBounds(timebounds,scanBounded,fieldBounded,interval_p);

      timeComp_p = new MSSmartInterval(interval_p,timebounds);
      //timeComp_p = new MSInterval(interval_p);
      objComp[i] = timeComp_p;
    }
    sortOrd[i]=Sort::Ascending;
  }
  Block<Int> orders(columns.nelements(),TableIterator::Ascending);
  
  // Store the sorted table for future access if possible, 
  // reuse it if already there
  for (size_t i=0; i<nMS_p; i++) {
    Bool useIn=False, store=False, useSorted=False;
    Table sorted;
    // check if we already have a sorted table consistent with the requested
    // sort order
    if (!bms_p[i].keywordSet().isDefined("SORT_COLUMNS") ||
	!bms_p[i].keywordSet().isDefined("SORTED_TABLE") ||
	bms_p[i].keywordSet().asArrayString("SORT_COLUMNS").nelements()!=
	columns.nelements() ||
	!allEQ(bms_p[i].keywordSet().asArrayString("SORT_COLUMNS"),
	       Vector<String>(columns.begin(),columns.end()))) {
      // if not, sort and store it (if possible)
      store=(bms_p[i].isWritable() && (bms_p[i].tableType() != Table::Memory));
    } else {
      sorted = bms_p[i].keywordSet().asTable("SORTED_TABLE");
      // if sorted table is smaller it can't be useful, remake it
      if (sorted.nrow() < bms_p[i].nrow()) store = bms_p[i].isWritable();
      else { 
	// if input is a sorted subset of the stored sorted table
	// we can use the input in the iterator
	if (isSubSet(bms_p[i].rowNumbers(),sorted.rowNumbers())) {
	  useIn=True;
	} else {
	  // check if #rows in input table is the same as the base table
	  // i.e., this is the entire table, if so, use sorted version instead
	  String anttab = bms_p[i].antenna().tableName(); // see comments below
	  Table base (anttab.erase(anttab.length()-8));
	  if (base.nrow()==bms_p[i].nrow()) {
	    useSorted = True;
	  } else {
	    store=bms_p[i].isWritable();
	  }
	}
      }
    }

    if (!useIn && !useSorted) {
      // we have to resort the input
      if (aips_debug) cout << "MSIter::construct - resorting table"<<endl;
      sorted = bms_p[i].sort(columns, objComp, sortOrd, Sort::QuickSort);
    }
    
    // Only store if globally requested _and_ locally decided
    if (storeSorted_p && store) {
	// We need to get the name of the base table to add a persistent
	// subtable (the ms used here might be a reference table)
	// There is no table function to get this, so we use the name of
	// the antenna subtable to get at it.
	String anttab = bms_p[i].antenna().tableName();
	sorted.rename(anttab.erase(anttab.length()-7)+"SORTED_TABLE",Table::New); 
	sorted.flush();
	bms_p[i].rwKeywordSet().defineTable("SORTED_TABLE",sorted);
	bms_p[i].rwKeywordSet().define("SORT_COLUMNS", Vector<String>(columns.begin( ),columns.end( )));
    }

    // create the iterator for each MS
    // at this stage either the input is sorted already or we are using
    // the sorted table, so the iterator can avoid sorting.
    if (useIn) {
      tabIter_p[i] = new TableIterator(bms_p[i],columns,objComp,orders,
				       TableIterator::NoSort);
    } else {
      tabIter_p[i] = new TableIterator(sorted,columns,objComp,orders,
				       TableIterator::NoSort);
    } 
    tabIterAtStart_p[i]=True;
  }
  setMSInfo();

}

MSIter2::MSIter2(const MSIter2& other): MSIter(other)
{
  operator=(other);
}

MSIter2::~MSIter2() 
{}

MSIter2& 
MSIter2::operator=(const MSIter2& other) 
{
  if (this==&other) return *this;
  MSIter::operator=(other);
  return *this;
}

void MSIter2::setState()
{
  setMSInfo();
  if(newMS_p)
    checkFeed_p=True;
  curTable_p=tabIter_p[curMS_p]->table();
  colArray_p.attach(curTable_p,MS::columnName(MS::ARRAY_ID));
  // msc_p is already defined here (it is set in setMSInfo)
  if(newMS_p)
    msc_p->antenna().mount().getColumn(antennaMounts_p,True);

  if(!ddInSort_p)
  {
    // If Data Description is not in the sorting columns, then the DD, SPW, pol
    // can change between elements of the same iteration, so the safest
    // is to signal that it changes.
    newDataDescId_p = true;
    newSpectralWindowId_p = true;
    newPolarizationId_p = true;
    freqCacheOK_p= false;

    // This indicates that the current DD, SPW, Pol Ids are not computed.
    // Note that the last* variables are not set, since the new* variables
    // are unconditionally set to true.
    // These cur* vars wiil be computed lazily if it is needed, together
    // with some more vars set in cacheExtraDDInfo().
    curDataDescIdFirst_p = -1;
    curSpectralWindowIdFirst_p = -1;
    curPolarizationIdFirst_p = -1;
  }
  else
  {
    // First we cache the current DD, SPW, Pol since we know it changed
    cacheCurrentDDInfo();

    // In this case we know that the last* variables were computed and
    // we can know whether there was a changed in these keywords by
    // comparing the two.
    newDataDescId_p=(lastDataDescId_p!=curDataDescIdFirst_p);
    newSpectralWindowId_p=(lastSpectralWindowId_p!=curSpectralWindowIdFirst_p);
    newPolarizationId_p=(lastPolarizationId_p!=curPolarizationIdFirst_p);

    lastDataDescId_p=curDataDescIdFirst_p;
    lastSpectralWindowId_p = curSpectralWindowIdFirst_p;
    lastPolarizationId_p = curPolarizationIdFirst_p;

    // Some extra information depends on the new* keywords, so compute
    // it now that new* have been set.
    cacheExtraDDInfo();
  }

  setArrayInfo();
  feedInfoCached_p = false;
  curFieldIdFirst_p=-1;
  //If field is not in the sorting columns, then the field id
  //can change between elements of the same iteration, so the safest
  //is to signal that it changes.
  if(!fieldInSort_p)
    newFieldId_p = true;
  else
  {
    setFieldInfo();
    newFieldId_p=(lastFieldId_p!=curFieldIdFirst_p);
    lastFieldId_p = curFieldIdFirst_p;
  }


  // If time binning, update the MSInterval's offset to account for glitches.
  // For example, if averaging to 5s and the input is
  //   TIME  STATE_ID  INTERVAL
  //    0      0         1
  //    1      0         1
  //    2      1         1
  //    3      1         1
  //    4      1         1
  //    5      1         1
  //    6      1         1
  //    7      0         1
  //    8      0         1
  //    9      0         1
  //   10      0         1
  //   11      0         1
  //  we want the output to be
  //   TIME  STATE_ID  INTERVAL
  //    0.5    0         2
  //    4      1         5
  //    9      0         5
  //  not what we'd get without the glitch fix:
  //   TIME  STATE_ID  INTERVAL
  //    0.5    0         2
  //    3      1         3
  //    5.5    1         2
  //    8      0         3
  //   10.5    0         2
  //
  // Resetting the offset with each advance() might be too often, i.e. we might
  // need different spws to share the same offset.  But in testing resetting
  // with each advance produces results more consistent with expectations than
  // either not resetting at all or resetting only
  // if(colTime_p(0) - 0.02 > timeComp_p->getOffset()).
  //
  // Don't need to do this with the MSSmartInterval?
  // if(timeComp_p && keyChange()=="SCAN_NUMBER"){
  //      timeComp_p->setOffset(0.0);
  //  }
}


void MSIter2::discernEnforcedTimeBounds(Vector<Double>& timebounds,
					Bool scanBounded) {

  ScalarColumn<Double> timecol;
  std::map<Int,Double> scanmap;
  Int nscan(0);
  for (size_t iMS=0; iMS<nMS_p; iMS++) {
    TableIterator ti(bms_p[iMS],String("SCAN_NUMBER"));

    if (scanBounded) {
      while (!ti.pastEnd()) {
	timecol.attach(ti.table(),MS::columnName(MS::TIME));
	scanmap[nscan++]=timecol(0)-0.001;
	ti.next();
      }
    }
    else {
      // first time in the scan
      timecol.attach(ti.table(),MS::columnName(MS::TIME));
      scanmap[nscan++]=timecol(0)-0.001;
    }
  }
  
  timebounds.resize(nscan);
  for (Int iscan=0;iscan<nscan;++iscan) 
    timebounds[iscan]=scanmap[iscan];
  
  //cout << "timebounds = " << timebounds-86400.0*floor(timebounds(0)/86400.0) << endl;
}


void MSIter2::discernEnforcedTimeBounds(Vector<Double>& timebounds,
					Bool scanBounded,
					Bool fieldBounded) {

  Int nsort(1+(fieldBounded?1:0));
  Block<String> cols(nsort);
  cols[0]="SCAN_NUMBER";
  if (fieldBounded) cols[1]="FIELD_ID";
  ScalarColumn<Double> timecol,expcol;
  ScalarColumn<Int> fieldcol,scancol;
  std::map<Int,Double> timemap;
  Int nscan(0);
  for (size_t iMS=0; iMS<nMS_p; iMS++) {
    TableIterator ti(bms_p[iMS],cols);
    Int thisScan(-1),thisField(-1),lastScan(-1),lastField(-1);
    while (!ti.pastEnd()) {
      timecol.attach(ti.table(),MS::columnName(MS::TIME));
      expcol.attach(ti.table(),MS::columnName(MS::EXPOSURE));
      scancol.attach(ti.table(),MS::columnName(MS::SCAN_NUMBER));
      fieldcol.attach(ti.table(),MS::columnName(MS::FIELD_ID));
      thisScan=scancol(0);
      thisField=fieldcol(0);
      
      if (nscan==0                                    ||  // first iteration
	  (scanBounded && (thisScan!=lastScan))       ||  // per-scan and scan change
	  (fieldBounded && ( (thisField!=lastField) ||    // per-field and field change
			     ((thisScan-lastScan)>1) ) )  //               or scan discont
	  ) {

	/*
	cout << nscan << " " 
	     << thisScan << " "
	     << thisField << " "
	     << MVTime((timecol(0)-expcol(0)/2.0)/C::day).string(MVTime::YMD,7)
	     << endl;
	*/

	timemap[nscan++]=timecol(0)-0.001;

      }
      lastScan=thisScan;
      lastField=thisField;
      ti.next();
    }
  }

  timebounds.resize(nscan);
  for (Int iscan=0;iscan<nscan;++iscan) 
    timebounds[iscan]=timemap[iscan];
  
}


void MSIter2::discernEnforcedTimeBounds(Vector<Double>& timebounds,
					Bool scanBounded,
					Bool fieldBounded,
					Double dt) {

  Int nsort(1+(fieldBounded?1:0));
  Block<String> cols(nsort);
  cols[0]="SCAN_NUMBER";
  if (fieldBounded) cols[1]="FIELD_ID";
  ScalarColumn<Double> timecol,expcol;
  ScalarColumn<Int> fieldcol,scancol;
  std::map<Int,Double> timemap;
  Int nscan(0);
  cout.precision(12);
  for (size_t iMS=0; iMS<nMS_p; iMS++) {
    TableIterator ti(bms_p[iMS],cols);
    Int thisScan(-1),thisField(-1),lastScan(-1),lastField(-1);
    while (!ti.pastEnd()) {
      timecol.attach(ti.table(),MS::columnName(MS::TIME));
      expcol.attach(ti.table(),MS::columnName(MS::EXPOSURE));
      scancol.attach(ti.table(),MS::columnName(MS::SCAN_NUMBER));
      fieldcol.attach(ti.table(),MS::columnName(MS::FIELD_ID));
      thisScan=scancol(0);
      thisField=fieldcol(0);
      
      if (nscan==0                                    ||  // first iteration
	  (scanBounded && (thisScan!=lastScan))       ||  // per-scan and scan change
	  (fieldBounded && (thisField!=lastField))    ||  // per-field and field change
	  ( (thisScan-lastScan)>1  &&                     // scan discontinuity _and_
	    (timecol(0)-timemap[nscan-1])>dt )            //   previous interval expired
	  ) {

	/*
	cout << nscan << " " 
	     << thisScan << " "
	     << thisField << " "
	     << MVTime((timecol(0)-expcol(0)/2.0)/C::day).string(MVTime::YMD,7)
	     << endl;
	*/

	timemap[nscan++]=(timecol(0)-0.01);

      }
      lastScan=thisScan;
      lastField=thisField;
      ti.next();
    }
  }

  timebounds.resize(nscan);
  for (Int iscan=0;iscan<nscan;++iscan) 
    timebounds[iscan]=timemap[iscan];
  
}


} //# NAMESPACE VI - END
} //# NAMESPACE CASA - END

