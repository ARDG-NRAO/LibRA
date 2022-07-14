///# Flagger.cc: this defines Flagger
//# Copyright (C) 2000,2001,2002
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
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/ArrayLogical.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <casacore/ms/MeasurementSets/MSSpWindowColumns.h>
#include <casacore/casa/BasicSL/Complex.h>
#include <casacore/measures/Measures/Stokes.h>
#include <casacore/casa/Utilities/Regex.h>
#include <casacore/casa/OS/HostInfo.h>
#include <flagging/Flagging/Flagger.h>
#include <flagging/Flagging/RFAFlagExaminer.h>
#include <flagging/Flagging/RFAMedianClip.h>
#include <flagging/Flagging/RFASpectralRej.h>
#include <flagging/Flagging/RFASelector.h>
#include <flagging/Flagging/RFAUVBinner.h>
#include <flagging/Flagging/RFATimeFreqCrop.h>
#include <msvis/MSVis/VisibilityIterator.h>
#include <msvis/MSVis/VisBuffer.h>
#include <casacore/casa/System/ProgressMeter.h>
#include <stdio.h>
#include <cmath>
#include <stdarg.h>

#include <casacore/tables/Tables/Table.h>
#include <casacore/tables/TaQL/TableParse.h>
#include <casacore/tables/Tables/TableRecord.h>
#include <casacore/tables/Tables/TableDesc.h>
#include <casacore/tables/Tables/TableLock.h>
#include <casacore/tables/Tables/SetupNewTab.h>

#include <casacore/tables/TaQL/ExprNode.h>
#include <msvis/MSVis/VisSet.h>
#include <msvis/MSVis/VisSetUtil.h>

#include <casacore/measures/Measures/Stokes.h>
#include <casacore/casa/Quanta/UnitMap.h>
#include <casacore/casa/Quanta/UnitVal.h>
#include <casacore/casa/Quanta/MVAngle.h>
#include <casacore/measures/Measures/MDirection.h>
#include <casacore/measures/Measures/MPosition.h>
#include <casacore/casa/Quanta/MVEpoch.h>
#include <casacore/measures/Measures/MEpoch.h>
#include <casacore/measures/Measures/MeasTable.h>

#include <flagging/Flagging/RFANewMedianClip.h>

#include <algorithm>

#include <sstream>

using namespace casacore;
namespace casa {

const bool Flagger::dbg = false;

LogIO Flagger::os( LogOrigin("Flagger") );
static char str[1024];
// uInt debug_ifr=9999,debug_itime=9999;


// -----------------------------------------------------------------------
// Default Constructor
// -----------------------------------------------------------------------
Flagger::Flagger ():mssel_p(0), vs_p(0), scan_looping(false)
{
	msselection_p = new MSSelection();
	spw_selection = false;

	agents_p = NULL;
	agentCount_p=0;
	opts_p = NULL;

	logSink_p = LogSink(LogMessage::NORMAL, false);

	nant = 0;
	nifr = 0;
	nfeed = 0;
	nfeedcorr = 0;

	setdata_p = false;
	selectdata_p = false;
	// setupAgentDefaults();

	quack_agent_exists = false;
}

// -----------------------------------------------------------------------
// Constructor
// constructs and attaches to MS
// -----------------------------------------------------------------------
Flagger::Flagger ( MeasurementSet &mset ) : mssel_p(0), vs_p(0), scan_looping(false)
{
	msselection_p = new MSSelection();
	spw_selection = false;

	agents_p = NULL;
	agentCount_p=0;
	opts_p = NULL;

	logSink_p = LogSink(LogMessage::NORMAL, false);

	nant = 0;
	setdata_p = false;
	selectdata_p = false;
	attach(mset);

	quack_agent_exists = false;
}

Flagger::~Flagger ()
{
	/*
	  jmlarsen: Eh? The following code is probably a bug
	  (it closes the MS when the length() is 0)
	  What about calling detach()?
	 */

	if ( !ms.tableName().length()) {
		os << "Flagger closing out " << ms.tableName() << LogIO::POST;
		ms.flush();
		ms.relinquishAutoLocks(true);
		ms.unlock();

		//originalms = NULL;
		originalms_p = NULL;
	}

	if (vs_p)  delete vs_p;
	vs_p = 0;

	if (dbg) cout << "Flagger destructor :: about to clean mssel_p" << endl;

	if (mssel_p) delete mssel_p;
	mssel_p = 0;

	if (dbg) cout << "Flagger destructor :: cleaned mssel_p" << endl;

	if (msselection_p) delete msselection_p;
	msselection_p = 0;

	if (agents_p) delete agents_p;
	agents_p = NULL;

	if (opts_p) delete opts_p;
	opts_p = NULL;
}

// -----------------------------------------------------------------------
// queryOptions
// Returns record of available options and their default values
// -----------------------------------------------------------------------
const RecordInterface & Flagger::defaultOptions ()
{
	static Record rec;
	// create record description on first entry
	if ( !rec.nfields() )
	{
		rec.defineRecord(RF_GLOBAL, Record());
		rec.define(RF_TRIAL, false);
		rec.define(RF_RESET, false);

		rec.setComment(RF_GLOBAL, "Record of global parameters applied to all methods");
		rec.setComment(RF_TRIAL, "T for trial run (no flags written out)");
		rec.setComment(RF_RESET, "T to reset existing flags before running");
	}
	return rec;
}

// -----------------------------------------------------------------------
// Flagger::attach
// attaches to MS
// -----------------------------------------------------------------------

bool Flagger::attach( MeasurementSet &mset, Bool setAgentDefaults )
{
	nant = 0;
	setdata_p = false;
	selectdata_p = false;
	if (vs_p)
		delete vs_p;
	vs_p = 0;
	if (mssel_p)
		delete mssel_p;
	mssel_p = 0;
	if (setAgentDefaults)
		setupAgentDefaults();
	ms = mset;

	originalms = mset;
	originalms_p = &mset;

	//  	Matrix<Int> noselection;
	// Use default sort order - and no scratch cols....
	//        vs_p = new VisSet(*originalms_p,noselection,0.0);
	//	vs_p->resetVisIter(sort, 0.0);

	Matrix<Int> noselection;
	Block<Int> bsort(3);
	bsort[0] = MS::FIELD_ID;
	bsort[1] = MS::DATA_DESC_ID;
	bsort[2] = MS::TIME;

	// extract various interesting info from the MS
	// obtain number of distinct time slots
	MSColumns msc(ms);
	Vector<Double> time( msc.time().getColumn() );

	uInt nrows = time.nelements();
	unsigned ntime;

	if (nrows == 0) ntime = 0;
	else {
		Bool dum;
		Double *tp = time.getStorage(dum);

		std::sort(tp, tp + nrows);

		ntime = 1;
		for (unsigned i = 0; i < nrows-1; i++) {
			if (tp[i] < tp[i+1]) ntime += 1;
		}
	}

	// obtain central frequencies of spws.
	const MSSpectralWindow spwin( ms.spectralWindow() );
	ScalarColumn<Double> sfreqs(spwin, "REF_FREQUENCY");
	spwfreqs.resize();
	spwfreqs = sfreqs.getColumn();
	spwfreqs *= 1e+6;

	// obtain number of antennas and interferometers
	const MSAntenna msant( ms.antenna() );
	nant = msant.nrow();
	nifr = nant*(nant+1)/2; // cheap & dirty
	ScalarColumn<String> names(msant,"NAME");
	antnames.resize();
	antnames = names.getColumn();
	antnames.apply(stringUpper);
	//  cerr<<"Antenna names: "<<antnames<<endl;
	// map ifrs to antennas
	ifr2ant1.resize(nifr);
	ifr2ant1.set(-1);
	ifr2ant2.resize(nifr);
	ifr2ant2.set(-1);
	for( uInt i1=0; i1<nant; i1++ ) {
		for( uInt i2=0; i2<=i1; i2++ ) {
			uInt ifr = ifrNumber(i1,i2);
			ifr2ant1(ifr) = i1;
			ifr2ant2(ifr) = i2;
		}
	}

	// get feed info
	const MSFeed msfeed( ms.feed() );
	nfeed = msfeed.nrow();
	nfeedcorr = nfeed*(nfeed+1)/2;

	sprintf(str,"attached MS %s: %d rows, %d times, %d baselines\n",ms.tableName().chars(),nrows,ntime,nifr);
	//os << "--------------------------------------------------" << LogIO::POST;
	os<<str<<LogIO::POST;

	return true;
}

// -----------------------------------------------------------------------
// Flagger::detach
// detaches from MS
// -----------------------------------------------------------------------
void Flagger::detach()
{
	if ( ms.tableName().length() == 0) {
		os << "no measurement set was attached" << LogIO::POST;
	}
	else {
		os << "detaching from MS " << ms.tableName() << LogIO::POST;
		nant = 0;
		setdata_p = false;
		selectdata_p = false;
		ms.flush();
		ms.relinquishAutoLocks(true);
		ms.unlock();
		ms = MeasurementSet();
	}
}

/************************************ DATA SELECTION **************************************/
/* return: true iff succesful
 */
Bool Flagger::selectdata(Bool useoriginalms,
		String field, String spw, String array,
		String feed, String scan,
		String baseline, String uvrange, String time,
		String correlation, String intent, String observation)
{
	if (dbg) cout << "selectdata: "
			<< "useoriginalms=" << useoriginalms
			<< " field=" << field << " spw=" << spw
			<< " array=" << array << " feed=" << feed
			<< " scan=" << scan << " baseline=" << baseline
			<< " uvrange=" << uvrange << " time=" << time
			<< " correlation=" << correlation << " scan intent="<< intent
			<< " observation=" << observation << endl;

	LogIO os(LogOrigin("Flagger", "selectdata()", WHERE));
	if (ms.isNull()) {
		os << LogIO::SEVERE << "NO MeasurementSet attached"
				<< LogIO::POST;
		return false;
	}

	/* If a diff sort-order is required, put it in here, and
       create the MSs with the new ordering */

	MeasurementSet *tms = NULL;
	if ( useoriginalms ) tms = &originalms;
	else
	{
		if (!mssel_p)
		{
			cout << "Flagger::selectdata -> mssel_p is NULL !!!" << endl;
			return false;
		}
		tms = mssel_p;
	}

	if (dbg) cout << "Setting selection strings" << endl;


	/* Row-Selection */

	if (spw.length() == 0 && uvrange.length() > 0) {
		spw = String("*");
	}

	const String dummyExpr = String("");
	if (msselection_p) {
		delete msselection_p;
		msselection_p = NULL;
		spw_selection = false;

	}
	msselection_p = new MSSelection(*tms,
			MSSelection::PARSE_NOW,
			(const String)time,
			(const String)baseline,
			(const String)field,
			(const String)spw,
			(const String)uvrange,
			dummyExpr, // taqlExpr
			dummyExpr, // corrExpr
			(const String)scan,
			(const String)array,
			(const String)intent,
			(const String)observation);
	spw_selection = ((spw != "" && spw != "*") || uvrange.length() > 0);
	// multiple spw agents are also needed for uvrange selections...

	selectdata_p = false;
	/* Print out selection info - before selecting ! */
	if (dbg)
	{
		cout.precision(16);
		cout << "Antenna 1 : " << msselection_p->getAntenna1List() << endl;
		cout << "Antenna 2 : " << msselection_p->getAntenna2List() << endl;
		Matrix<Int> baselinelist(msselection_p->getBaselineList());
		IPosition shp = baselinelist.shape();
		if (shp.product() < 20)
		{
			IPosition transposed = shp;
			transposed[0]=shp[1]; transposed[1]=shp[0];
			Matrix<Int> blist(transposed);
			for (Int i=0;i<shp[0];i++)
				for (Int j=0;j<shp[1];j++)
					blist(j,i) = baselinelist(i,j);
			cout << "Baselines : " << blist << endl;
		}
		cout << "Fields : " << msselection_p->getFieldList() << endl;
		cout << "Spw : " << msselection_p->getSpwList() << endl;
		cout << "SpwChans : " << msselection_p->getChanList() << endl;
		Matrix<Double> tlist(msselection_p->getTimeList());
		cout << "Time : " << tlist << endl;
		cout << "Time : " << endl;
		for(Int i=0;i<(tlist.shape())[1];i++)
			cout << "[" << MVTime(tlist(0,i)/C::day).string(MVTime::DMY,7) << " ~ " << MVTime(tlist(1,i)/C::day).string(MVTime::DMY,7) << "]" << endl;
		cout << "UVrange : " << msselection_p->getUVList() << endl;
		cout << "UV Units : " << msselection_p->getUVUnitsList() << endl;
		cout << "Scans : " << msselection_p->getScanList() << endl;
		cout << "Arrays : " << msselection_p->getSubArrayList() << endl;
		// cout << "Feeds : " << msselection_p->getFeedList() << endl;
		cout << "Scan intent : " << msselection_p->getStateObsModeList() << endl;
		cout << "Observation : " << msselection_p->getObservationList() << endl;

	}

	/* Correlations */
	correlations_p.resize(0);
	string tcorr[50];
	Regex delim("(,| )+");
	Int ncorr = split(correlation, tcorr, 50, delim);
	correlations_p.resize(ncorr);
	for(Int i=0;i<ncorr;i++) correlations_p[i] = upcase(String(tcorr[i]));

	if (dbg) cout << "Correlations : " << correlations_p << endl;

	selectdata_p = true;
	return true;
}

Bool Flagger::setdata(
		String field, String spw, String array,
		String feed, String scan,
		String baseline,  String uvrange,  String time,
		String correlation, String intent, String observation)
{
	if (dbg) cout << "setdata: "
			<< " field=" << field << " spw=" << spw
			<< " array=" << array << " feed=" << feed
			<< " scan=" << scan << " baseline=" << baseline
			<< " uvrange=" << uvrange << " time=" << time
			<< " correlation=" << correlation << " scan intent=" << intent
			<< " observation=" << observation << endl;

	LogIO os(LogOrigin("Flagger", "setdata()", WHERE));
	setdata_p = false;


	/* check the MS */
	if (ms.isNull()) {
		os << LogIO::SEVERE << "NO MeasurementSet attached"
				<< LogIO::POST;
		return false;
	}

	/* Parse selection parameters */
	if (!spw.length()) spw = String("*");

	if (!selectdata(true,field,spw,array,feed,scan, baseline,uvrange,time,correlation,intent,observation))
	{
		os << LogIO::SEVERE << "Selection failed !!"
				<< LogIO::POST;
		return false;
	}

	/* Create selected reference MS */
	MeasurementSet mssel_p2(*originalms_p);

	msselection_p->getSelectedMS(mssel_p2, String(""));

	if (dbg) cout << "Original ms has nrows : " << originalms.nrow() << LogIO::POST;
	if (dbg) cout << "Selected ms has " << mssel_p2.nrow() << " rows." << LogIO::POST;

	if ( mssel_p2.nrow() ) {
		if (mssel_p) {
			delete mssel_p;
			mssel_p=NULL;
		}
		if (mssel_p2.nrow() == originalms_p->nrow()) {
			mssel_p = new MeasurementSet(*originalms_p);
		}
		else {
			mssel_p = new MeasurementSet(mssel_p2);
		}
		if (dbg)cout << "assigned new MS to mssel_p" << endl;
		ScalarColumn<String> fname( mssel_p->field(),"NAME" );
		if (dbg)cout << "fields : " << fname.getColumn() << endl;

		//mssel_p->rename("selectedms",Table::New);
		//mssel_p->flush();
	}
	else {
		os << LogIO::WARN << "Selected MS has zero rows" << LogIO::POST;
		//mssel_p = &originalms;
		if (mssel_p) {
			delete mssel_p; 
			mssel_p=NULL;
		}
		return false;
	}

	/* Print out selection info - before selecting ! */
	if (mssel_p->nrow()!=ms.nrow()) {
		os << "By selection " << originalms.nrow() << " rows are reduced to "
				<< mssel_p->nrow() << LogIO::POST;
	}
	else {
		os << "Selection did not drop any rows" << LogIO::NORMAL3;
	}
	/* Create a vis iter */
	Matrix<Int> noselection;
	Block<int> sort2(4);
	//sort2[0] = MS::SCAN_NUMBER;
	// Do scan priority only if quacking
	sort2[0]= MS::ARRAY_ID;
	sort2[1]= MS::FIELD_ID;
	sort2[2]= MS::DATA_DESC_ID;
	sort2[3] = MS::TIME;
	/* Set the chunk time interval to be the full time-range.
          - FlagExaminer counters depend on this.
       However, autoflag agents like tfcrop will take a performance hit
       since it will store flags for the entire chunk in memory
	 */
	Double timeInterval = 7.0e9; //a few thousand years
	//Double timeInterval = 6000; // seconds : 100 minutes.

	if (vs_p) {
		delete vs_p; vs_p = NULL;
	}
	if (!vs_p) {
		if (!mssel_p)
			throw AipsError("No measurement set selection available");
		//vs_p = new VisSet(*mssel_p,sort,noselection,0.0);

		Bool addScratch = false;
		vs_p = new VisSet(*mssel_p, sort2, noselection,
				addScratch, timeInterval);
		// Use default sort order - and no scratch cols....
		//vs_p = new VisSet(*mssel_p,noselection,0.0);
	}
	vs_p->resetVisIter(sort2, timeInterval);

	// Optimize for iterating randomly through one MS
	vs_p->iter().slurp();

	/* Channel selection */ // Always select all chans
	selectDataChannel();
	ms = *mssel_p;


	setdata_p = true;
	return true;
}


Bool Flagger::selectDataChannel(){

	if (!vs_p || !msselection_p) return false;
	/* Set channel selection in visiter */
	/* Set channel selection per spectral window - from msselection_p->getChanList(); */
	// this is needed when "setdata" is used to select data for autoflag algorithms
	// This should not be done for manual flagging, because the channel indices for the
	//  selected subset start from zero - and throw everything into confusion.
	Vector<Int> spwlist = msselection_p->getSpwList();

	if ( spwlist.nelements() && spw_selection ) {
		Matrix<Int> spwchan = msselection_p->getChanList();
		IPosition cshp = spwchan.shape();
		if ( (Int)spwlist.nelements() > (spwchan.shape())[0] )
			cout << "WARN : Using only the first channel range per spw" << endl;
		for( uInt i=0;i<spwlist.nelements();i++ )
		{
			Int j=0;
			for ( j=0;j<(spwchan.shape())[0];j++ )
				if ( spwchan(j,0) == spwlist[i] ) break;
			vs_p->iter().selectChannel(1, Int(spwchan(j,1)),
					Int(spwchan(j,2)-spwchan(j,1)+1),
					Int(spwchan(j,3)), spwlist[i]);
		}
	}
	else{
		return false;
	}


	return true;
}



/************************** Set Manual Flags ***************************/

Bool Flagger::fillSelections(Record &rec)
{

	//TableExprNode ten = msselection_p->toTableExprNode(originalms_p);

	/* Fill in record for selected values. */
	/* Field ID */
	Vector<Int> fieldlist = msselection_p->getFieldList();
	if (fieldlist.nelements())
	{
		RecordDesc flagDesc;
		flagDesc.addField(RF_FIELD, TpArrayInt);
		Record flagRec(flagDesc);
		flagRec.define(RF_FIELD, fieldlist);
		rec.mergeField(flagRec, RF_FIELD, RecordInterface::OverwriteDuplicates);
	}
	/* BASELINE */
	Matrix<Int> baselinelist = msselection_p->getBaselineList();

	/* Here, it may be necessary to convert negative indices to positive ones
       (see CAS-2021).

       For now, fail cleanly if getBaselineList returned negative indices.
	 */

	if (baselinelist.nelements())
	{
		IPosition shp = baselinelist.shape();
		if (dbg)cout << "Original shape of baselinelist : " << shp << endl;
		IPosition transposed = shp;
		transposed[0] = shp[1]; transposed[1] = shp[0];
		Matrix<Int> blist(transposed);

		for(Int i=0; i < shp[0]; i++)
			for(Int j=0; j < shp[1]; j++) {

				if (baselinelist(i, j) < 0) {
					throw AipsError("Sorry, negated antenna selection (such as '!2') is not supported by flagger (CAS-2021)");
				}
				blist(j, i) = baselinelist(i, j);
			}

		RecordDesc flagDesc;
		flagDesc.addField(RF_BASELINE, TpArrayInt);
		Record flagRec(flagDesc);
		flagRec.define(RF_BASELINE, blist);
		rec.mergeField(flagRec, RF_BASELINE, RecordInterface::OverwriteDuplicates);
	}
	/* FEED */
	/*
      Matrix<Int> feedlist = msselection_p->getFeedList();
      if (feedlist.nelements())
      {
      IPosition shp = feedlist.shape();
      if (dbg)cout << "Original shape of feedlist : " << shp << endl;
      IPosition transposed = shp;
      transposed[0]=shp[1]; transposed[1]=shp[0];
      Matrix<Int> blist(transposed);
      for(Int i=0;i<shp[0];i++)
      for(Int j=0;j<shp[1];j++)
      blist(j,i) = feedlist(i,j);
      // need to add 1 because RFASelector expects 1-based indices.

      RecordDesc flagDesc;       
      flagDesc.addField(RF_FEED, TpArrayInt);
      Record flagRec(flagDesc);  
      flagRec.define(RF_FEED, blist);
      rec.mergeField(flagRec, RF_FEED, RecordInterface::OverwriteDuplicates);
      }
	 */
	/* TIME */
	Matrix<Double> timelist = msselection_p->getTimeList();
	if (timelist.nelements())
	{
		/* Times need to be in MJD */
		for( Int i=0;i<(timelist.shape())[0];i++ )
			for( Int j=0;j<(timelist.shape())[1];j++ )
				timelist(i,j) /= (Double)(24*3600);
		//for( Int i=0;i<(timelist.shape())[0];i++ )
		//  for( Int j=0;j<(timelist.shape())[1];j++ )
		//    cout << "timelist(" << i << ", " << j << ")="
		//           << timelist(i,j) << endl;
		RecordDesc flagDesc;
		flagDesc.addField(RF_TIMERANGE, TpArrayDouble);
		Record flagRec(flagDesc);
		flagRec.define(RF_TIMERANGE, timelist);
		rec.mergeField(flagRec, RF_TIMERANGE, RecordInterface::OverwriteDuplicates);
	}
	/* RF_CORR */
	if (correlations_p.nelements())
	{
		RecordDesc flagDesc;
		flagDesc.addField(RF_CORR, TpArrayString);
		Record flagRec(flagDesc);
		flagRec.define(RF_CORR, correlations_p);
		rec.mergeField(flagRec, RF_CORR, RecordInterface::OverwriteDuplicates);
	}
	/* Array ID */
	Vector<Int> arraylist = msselection_p->getSubArrayList();
	if (arraylist.nelements())
	{
		RecordDesc flagDesc;
		flagDesc.addField(RF_ARRAY, TpArrayInt);
		Record flagRec(flagDesc);
		flagRec.define(RF_ARRAY, arraylist);
		rec.mergeField(flagRec, RF_ARRAY, RecordInterface::OverwriteDuplicates);
	}
	/* Scan ID */
	Vector<Int> scanlist = msselection_p->getScanList();
	if (scanlist.nelements())
	{
		RecordDesc flagDesc;
		flagDesc.addField(RF_SCAN, TpArrayInt);
		Record flagRec(flagDesc);
		flagRec.define(RF_SCAN, scanlist);
		rec.mergeField(flagRec, RF_SCAN, RecordInterface::OverwriteDuplicates);
	}
	/* State ID  (obsMode) */
	Vector<Int> stateobsmodelist = msselection_p->getStateObsModeList();
	if (stateobsmodelist.nelements())
	{
		RecordDesc flagDesc;
		flagDesc.addField(RF_INTENT, TpArrayInt);
		Record flagRec(flagDesc);
		flagRec.define(RF_INTENT, stateobsmodelist);
		rec.mergeField(flagRec, RF_INTENT, RecordInterface::OverwriteDuplicates);
	}
	/* Observation ID */
	Vector<Int> observationlist = msselection_p->getObservationList();
	if (observationlist.nelements())
	{
		RecordDesc flagDesc;
		flagDesc.addField(RF_OBSERVATION, TpArrayInt);
		Record flagRec(flagDesc);
		flagRec.define(RF_OBSERVATION, observationlist);
		rec.mergeField(flagRec, RF_OBSERVATION, RecordInterface::OverwriteDuplicates);
	}

	return true;
}



/*
      Sets up agents for mode = 'manualflag' and mode = 'summary'
 */

Bool Flagger::setmanualflags(Bool autocorr,
		Bool unflag,
		String clipexpr,
		Vector<Double> cliprange,
		String clipcolumn,
		Bool outside,
		Bool channel_average,
		Double quackinterval,
		String quackmode,
		Bool quackincrement,
		String opmode,
		Double diameter,
		Double lowerlimit,
		Double upperlimit)
{
	if (dbg)
		cout << "setmanualflags: "
		<< "autocorr=" << autocorr
		<< " unflag=" << unflag
		<< " clipexpr=" << clipexpr
		<< " cliprange=" << cliprange
		<< " clipcolumn=" << clipcolumn
		<< " outside=" << outside
		<< " quackinterval=" << quackinterval
		<< " quackmode=" << quackmode
		<< " quackincrement=" << quackincrement
		<< " opmode=" << opmode
		<< " diameter=" << diameter
		<< endl;

	LogIO os(LogOrigin("Flagger", "setmanualflags()", WHERE));
	if (ms.isNull()) {
		os << LogIO::SEVERE << "NO MeasurementSet attached"
				<< LogIO::POST;
		return false;
	}
	if (!selectdata_p) {
		os << LogIO::SEVERE << "Please run selectdata with/without arguments before setmanualflags"
				<< LogIO::POST;
		return false;
	}

	/* Fill in an agent record */
	/* This assumes that selectdata has already been called */

	/* Loop over SPW and chan ranges. */

	Vector<Int> spwlist = msselection_p->getSpwList();

	Matrix<Int> spwchan = msselection_p->getChanList();
	Matrix<Double> uvrangelist = msselection_p->getUVList();
	Vector<Bool> uvrangeunits = msselection_p->getUVUnitsList();
	IPosition cshp = spwchan.shape();

	/* If no selection depends on spw, ( i.e. no spw, chan, uvrange )
       then no need to make separate records for each spw. */
	bool separatespw = false;
	Int nrec;
	if (spwlist.nelements() && spw_selection) {
		separatespw = true;
		nrec = spwlist.nelements();
	}
	else {
		separatespw = false; nrec = 1;
	}

	if (dbg) {
		cout << "separatespw = " << separatespw << endl;
		cout << spwlist << endl;
	}

	scan_looping = false;

	for ( Int i=0; i < nrec; i++ ) {
		Record selrec;
		if (upcase(opmode).matches("FLAG") ||
				upcase(opmode).matches("SHADOW") ||
				upcase(opmode).matches("ELEVATION")) {
			selrec.define("id", String("select"));

			if (upcase(opmode).matches("SHADOW")) {
				selrec.define(RF_DIAMETER, Double(diameter));
			}
			else if (upcase(opmode).matches("ELEVATION")) {
				selrec.define(RF_LOWERLIMIT, Double(lowerlimit));
				selrec.define(RF_UPPERLIMIT, Double(upperlimit));
			}
		}
		else if (upcase(opmode).matches("SUMMARY")) {
			selrec.define("id", String("flagexaminer"));
		}
		else {
			throw AipsError("Unknown mode " + upcase(opmode));
		}

		/* Fill selections for all but spw, chan, corr */
		fillSelections(selrec);

		if (separatespw) {
			/* SPW ID */
			{
				RecordDesc flagDesc;
				flagDesc.addField(RF_SPWID, TpArrayInt);
				Record flagRec(flagDesc);
				Vector<Int> t_spw(1); t_spw[0] = spwlist[i];
				flagRec.define(RF_SPWID, t_spw);
				selrec.mergeField(flagRec, RF_SPWID, RecordInterface::OverwriteDuplicates);
			}

			/* reform chan ranges */
			Int ccount=0;
			for( Int j=0;j<cshp[0];j++ ) if ( spwlist[i] == spwchan(j,0) ) ccount++;
			Matrix<Int> chanlist(2,ccount); chanlist.set(0);

			ccount=0;
			for( Int j=0;j<cshp[0];j++ )
			{
				if ( spwlist[i] == spwchan(j,0) )
				{
					chanlist(0,ccount) = spwchan(j,1);
					chanlist(1,ccount) = spwchan(j,2);
					if ( spwchan(j,3) > 1 )
						os << LogIO::WARN << ".... ignoring chan 'step' for manual flags" << LogIO::POST;
					ccount++;
				}
			}
			/* RF_CHANS */
			{
				RecordDesc flagDesc;
				flagDesc.addField(RF_CHANS, TpArrayInt);
				Record flagRec(flagDesc);
				flagRec.define(RF_CHANS, chanlist);
				selrec.mergeField(flagRec, RF_CHANS, RecordInterface::OverwriteDuplicates);
			}

			/* UV-RANGE */
			if (uvrangelist.nelements())
			{
				Matrix<Double> templist(uvrangelist.shape());
				/* Convert to Metres... */
				/* or complain if units are not metres... */
				// current spw : spwlist[i];

				if ( (templist.shape())[1] != (Int)uvrangeunits.nelements() )
					cout << "UVRANGE units are wrong length ! " << endl;
				for( Int j=0;j<(templist.shape())[1];j++ )
				{
					Double unit=1.0;
					if ( ! uvrangeunits[j] ) unit = C::c/(spwfreqs[spwlist[i]]/1e+6);
					for( Int k=0;k<(templist.shape())[0];k++ )
						templist(k,j) = uvrangelist(k,j) * unit ;
				}

				RecordDesc flagDesc;
				flagDesc.addField(RF_UVRANGE, TpArrayDouble);
				Record flagRec(flagDesc);
				flagRec.define(RF_UVRANGE, templist);
				selrec.mergeField(flagRec, RF_UVRANGE, RecordInterface::OverwriteDuplicates);
				if (dbg) cout << "uv list (m) : " << templist << endl;
			}
		}

		// Operation related parameters.
		if (upcase(opmode).matches("SHADOW") ) {
			RecordDesc flagDesc;
			flagDesc.addField(RF_SHADOW, TpBool);
			Record flagRec(flagDesc);
			flagRec.define(RF_SHADOW, true);
			selrec.mergeField(flagRec, RF_SHADOW, RecordInterface::OverwriteDuplicates);
		}

		else if (upcase(opmode).matches("ELEVATION") ) {
			RecordDesc flagDesc;
			flagDesc.addField(RF_ELEVATION, TpBool);
			Record flagRec(flagDesc);
			flagRec.define(RF_ELEVATION, true);
			selrec.mergeField(flagRec, RF_ELEVATION, RecordInterface::OverwriteDuplicates);
		}

		/* Flag Autocorrelations too? */
		if (autocorr) {
			RecordDesc flagDesc;
			flagDesc.addField(RF_AUTOCORR, TpBool);
			Record flagRec(flagDesc);
			flagRec.define(RF_AUTOCORR, autocorr);
			selrec.mergeField(flagRec, RF_AUTOCORR, RecordInterface::OverwriteDuplicates);
		}

		/* Unflag! */
		if (unflag) {
			RecordDesc flagDesc;
			flagDesc.addField(RF_UNFLAG, TpBool);
			Record flagRec(flagDesc);
			flagRec.define(RF_UNFLAG, unflag);
			selrec.mergeField(flagRec, RF_UNFLAG, RecordInterface::OverwriteDuplicates);
		}

		/* Reset flags before applying new ones */
		// ( I think... )
		/*
	  {
	  RecordDesc flagDesc;       
	  flagDesc.addField(RF_RESET, TpBool);
	  Record flagRec(flagDesc);  
	  flagRec.define(RF_RESET, true);
	  selrec.mergeField(flagRec, RF_RESET, RecordInterface::OverwriteDuplicates);
	  }
		 */

		/* Clip/FlagRange */
		/* Jira Casa 212 : Check if "clipexpr" has multiple
	  comma-separated expressions
	  and loop here, creating multiple clipRecs. 
	  The RFASelector will handle it. */
		if (clipexpr.length() && cliprange.nelements() == 2 &&
				(cliprange[0] < cliprange[1] ||
						(cliprange[0] <= cliprange[1] && !outside) // i.e. exact matching
				)
		)
		{
			RecordDesc flagDesc;
			if ( outside )
				flagDesc.addField(RF_CLIP, TpRecord);
			else
				flagDesc.addField(RF_FLAGRANGE, TpRecord);

			Record flagRec(flagDesc);

			RecordDesc clipDesc;
			clipDesc.addField(RF_EXPR, TpString);
			clipDesc.addField(RF_MIN, TpDouble);
			clipDesc.addField(RF_MAX, TpDouble);
			clipDesc.addField(RF_CHANAVG, TpBool);
			Record clipRec(clipDesc);
			clipRec.define(RF_EXPR, clipexpr);
			clipRec.define(RF_MIN, cliprange[0]);
			clipRec.define(RF_MAX, cliprange[1]);
			clipRec.define(RF_CHANAVG, channel_average);

			if ( outside )
			{
				flagRec.defineRecord(RF_CLIP, clipRec);
				selrec.mergeField(flagRec, RF_CLIP, RecordInterface::OverwriteDuplicates);
			}
			else
			{
				flagRec.defineRecord(RF_FLAGRANGE, clipRec);
				selrec.mergeField(flagRec, RF_FLAGRANGE, RecordInterface::OverwriteDuplicates);
			}

			/* clip column */
			if (!clipcolumn.length()) clipcolumn=String("DATA");
			RecordDesc flagDesc2;
			flagDesc2.addField(RF_COLUMN, TpString);
			Record flagRec2(flagDesc2);
			flagRec2.define(RF_COLUMN, clipcolumn);
			selrec.mergeField(flagRec2, RF_COLUMN, RecordInterface::OverwriteDuplicates);

		}

		/* Quack! */
		if (quackinterval > 0.0) {
			//Reset the Visiter to have SCAN before time
			scan_looping = true;
			Block<int> sort2(5);
			sort2[0] = MS::ARRAY_ID;
			sort2[1] = MS::FIELD_ID;
			sort2[2] = MS::DATA_DESC_ID;
			sort2[3] = MS::SCAN_NUMBER;
			sort2[4] = MS::TIME;
			Double timeInterval = 7.0e9; //a few thousand years

			vs_p->resetVisIter(sort2, timeInterval);
			vs_p->iter().slurp();
			//lets make sure the data channel selection is done
			selectDataChannel();

			RecordDesc flagDesc;
			flagDesc.addField(RF_QUACK, TpArrayDouble);
			flagDesc.addField(RF_QUACKMODE, TpString);
			flagDesc.addField(RF_QUACKINC, TpBool);

			Vector<Double> quackparams(2);
			quackparams[0] = quackinterval;
			quackparams[1] = 0.0;

			Record flagRec(flagDesc);
			flagRec.define(RF_QUACK, quackparams);
			flagRec.define(RF_QUACKMODE, quackmode);
			flagRec.define(RF_QUACKINC, quackincrement);
			selrec.mergeField(flagRec, RF_QUACK    , RecordInterface::OverwriteDuplicates);
			selrec.mergeField(flagRec, RF_QUACKMODE, RecordInterface::OverwriteDuplicates);
			selrec.mergeField(flagRec, RF_QUACKINC , RecordInterface::OverwriteDuplicates);

			quack_agent_exists = true;
		}
		/* end if opmode = ... */

		/* Add this agent to the list */
		addAgent(selrec);
	}

	return true;
}

Bool Flagger::setautoflagparams(String algorithm,Record &parameters)
{
	LogIO os(LogOrigin("Flagger", "setautoflagparams()", WHERE));
	if (ms.isNull()) {
		os << LogIO::SEVERE << "NO MeasurementSet attached"
				<< LogIO::POST;
		return false;
	}
	if (!selectdata_p) {
		os << LogIO::SEVERE << "Please run setdata with/without arguments before setautoflagparams"
				<< LogIO::POST;
		return false;
	}

	/* Create an agent record */
	Record selrec;
	selrec = getautoflagparams(algorithm);
	selrec.merge(parameters,Record::OverwriteDuplicates);

	/* special case for "sprej". need to parse a param.*/
	if (algorithm.matches("sprej") && selrec.isDefined("fitspwchan")) {
		/* Get the "fitspwchan" string" */
		/* Pass this through the msselection parser and getChanList */
		/* Construct a list of regions records from this list */
		String fitspwchan;
		selrec.get(RecordFieldId("fitspwchan"),fitspwchan);

		if (fitspwchan.length())
		{
			/* Parse it */
			const String dummy("");
			MSSelection tmpmss(*mssel_p,MSSelection::PARSE_NOW,
					dummy,dummy, dummy, fitspwchan, dummy,
					dummy, dummy, dummy, dummy);
			Matrix<Int> spwchanlist = tmpmss.getChanList();
			Vector<Int> spwlist = tmpmss.getSpwList();

			/* Create region record template */
			RecordDesc regdesc;
			for(uInt i=0;i<spwlist.nelements();i++)
				regdesc.addField(String(i),TpRecord);
			Record regions(regdesc);

			/* reform chan ranges */
			Int ccount=0;
			IPosition cshp = spwchanlist.shape();
			for(uInt i=0;i<spwlist.nelements();i++)
			{
				ccount=0;
				for( Int j=0;j<cshp[0];j++ )
					if ( spwlist[i] == spwchanlist(j,0) ) ccount++;
				Matrix<Int> chanlist(2,ccount); chanlist.set(0);

				ccount=0;
				for( Int j=0;j<cshp[0];j++ )
				{
					if ( spwlist[i] == spwchanlist(j,0) )
					{
						chanlist(0,ccount) = spwchanlist(j,1);
						chanlist(1,ccount) = spwchanlist(j,2);
						if ( spwchanlist(j,3) > 1 )
							os << LogIO::WARN << ".... ignoring chan 'step' for 'sprej' fitting" << LogIO::POST;
						ccount++;
					}
				}

				RecordDesc spwDesc;
				spwDesc.addField(RF_SPWID, TpInt);
				spwDesc.addField(RF_CHANS, TpArrayInt);
				Record spwRec(spwDesc);
				spwRec.define(RF_SPWID, spwlist[i]);
				spwRec.define(RF_CHANS, chanlist);

				/* create a single region record */
				/* add this to the list of region records */
				regions.defineRecord(String(i),spwRec);
			}

			/* Attach the list of region records */
			selrec.defineRecord(RF_REGION, regions);
		}
	}

	/* Add this agent to the list */
	addAgent(selrec);

	return true;
}


Record Flagger::getautoflagparams(String algorithm)
{
	LogIO os(LogOrigin("Flagger", "getautoflagparams()", WHERE));

	// Use "RFATimeMedian::getDefaults()" !!!!!!!

	Record defrecord;
	if ( agent_defaults.isDefined(algorithm) )
	{
		RecordFieldId rid(algorithm);
		defrecord = agent_defaults.asRecord(rid);
		defrecord.define("id",algorithm);

		if (defrecord.isDefined("expr")) defrecord.define("expr","ABS I");
	}
	return defrecord;

}

/* Clean up all selections */
// TODO Later add in the ability to clear specified
// agent types.
//
// subRecord = agents_p->getField(RecordFieldId(x));
// if ( subRecord.isDefined('id') && 'id' is "select" )
//         agents_p->removeField(RecordFieldId(x));
//
Bool Flagger::clearflagselections(Int recordindex)
{
	LogIO os(LogOrigin("Flagger", "clearflagselections()", WHERE));

	if ( agents_p && agents_p->nfields() )
	{
		if ( recordindex >= 0 )
		{
			if (dbg) cout << "Deleting only agent : " << recordindex << endl;
			agents_p->removeField(RecordFieldId(recordindex));
		}
		else
		{
			if (dbg) cout << "Deleting all agents" << endl;
			delete agents_p;
			agents_p =0;
			agentCount_p = 0;
		}
	}

	//      printflagselections();

	return true;
}

Bool Flagger::printflagselections()
{
	LogIO os(LogOrigin("Flagger", "printflagselections()", WHERE));
	if ( agents_p )
	{
		os << "Current list of agents : " << agents_p << LogIO::POST;
		ostringstream out;
		agents_p->print(out);
		os << out.str() << LogIO::POST;
	}
	else os << " No current agents " << LogIO::POST;

	return true;
}

Bool Flagger::addAgent(RecordInterface &newAgent)
{
	if (!agents_p)
	{
		agentCount_p = 0;
		agents_p = new Record;
		if (dbg) cout << "creating new agent" << endl;
	}

	ostringstream fieldName;
	fieldName << agentCount_p++;
	agents_p->defineRecord(fieldName.str(), newAgent);

	//      printflagselections();

	return true;
}


// computes IFR index, given two antennas
uInt Flagger::ifrNumber ( Int ant1,Int ant2 ) const
{
	if ( ant1<ant2 )
		return ifrNumber(ant2,ant1);
	return ant1*(ant1+1)/2 + ant2;
}
//TODO
// Here, fill in correct indices for the baseline ordering in the MS
// All agents will see this and will be fine.

// computes vector of IFR indeces, given two antennas
Vector<Int> Flagger::ifrNumbers ( Vector<Int> ant1,Vector<Int> ant2 ) const
{
	unsigned n = ant1.nelements();
	Vector<Int> r(n);

	for (unsigned i = 0; i < n; i++) {
		Int a1 = ant1(i);
		Int a2 = ant2(i);
		if (a1 > a2) {
			r(i) = a1*(a1+1)/2 + a2;
		}
		else {
			r(i) = a2*(a2+1)/2 + a1;
		}
	}
	return r;
}

void Flagger::ifrToAnt ( uInt &ant1,uInt &ant2,uInt ifr ) const
{
	ant1 = ifr2ant1(ifr);
	ant2 = ifr2ant2(ifr);
}

// -----------------------------------------------------------------------
// Flagger::setupAgentDefaults
// Sets up record of available agents and their default parameters
// -----------------------------------------------------------------------
const RecordInterface & Flagger::setupAgentDefaults ()
{
	agent_defaults = Record();
	agent_defaults.defineRecord("timemed", RFATimeMedian::getDefaults());
	agent_defaults.defineRecord("newtimemed", RFANewMedianClip::getDefaults());
	agent_defaults.defineRecord("freqmed", RFAFreqMedian::getDefaults());
	agent_defaults.defineRecord("sprej", RFASpectralRej::getDefaults());
	agent_defaults.defineRecord("select", RFASelector::getDefaults());
	agent_defaults.defineRecord("flagexaminer", RFAFlagExaminer::getDefaults());
	agent_defaults.defineRecord("uvbin", RFAUVBinner::getDefaults());
	agent_defaults.defineRecord("tfcrop", RFATimeFreqCrop::getDefaults());
	return agent_defaults;
}

// -----------------------------------------------------------------------
// Flagger::createAgent
// Creates flagging agent based on name
// -----------------------------------------------------------------------
std::shared_ptr<RFABase> Flagger::createAgent (const String &id,
		RFChunkStats &chunk,
		const RecordInterface &parms,
		bool &only_selector)
{
	if (id != "select" && id != "flagexaminer") {
		only_selector = false;
	}
	// cerr << "Agent id: " << id << endl;
	if ( id == "timemed" )
		return std::shared_ptr<RFABase>(new RFATimeMedian(chunk, parms));
	else if ( id == "newtimemed" )
		return std::shared_ptr<RFABase>(new RFANewMedianClip(chunk, parms));
	else if ( id == "freqmed" )
		return std::shared_ptr<RFABase>(new RFAFreqMedian(chunk, parms));
	else if ( id == "sprej" )
		return std::shared_ptr<RFABase>(new RFASpectralRej(chunk, parms));
	else if ( id == "select" )
		return std::shared_ptr<RFABase>(new RFASelector(chunk, parms));
	else if ( id == "flagexaminer" )
		return std::shared_ptr<RFABase>(new RFAFlagExaminer(chunk, parms));
	else if ( id == "uvbin" )
		return std::shared_ptr<RFABase>(new RFAUVBinner(chunk, parms));
	else if ( id == "tfcrop" )
		return std::shared_ptr<RFABase>(new RFATimeFreqCrop(chunk, parms));
	else
		return std::shared_ptr<RFABase>();
}


void Flagger::summary( const RecordInterface &agents )
{
	//os << "Autoflag summary will report results here" << LogIO::POST;
	for(uInt i=0;i<agents.nfields(); i++){

		if (agents.dataType(i) != TpRecord){
			os << "Unrecognized field: " << agents.name(i) << LogIO::EXCEPTION;
		}
		String agent_id(downcase(agents.name(i)));
		// cerr << i << " " << agent_id << endl;
		printAgentRecord(agent_id, i, agents.asRecord(i));
	}
}

/*
      Well, for boolean values this function gives you 1 + 1 = 2,
      where the AIPS++ built-in, never-should-have-been-written, sum()
      surprisingly gives you 1 + 1 = 1.
 */
int Flagger::my_aipspp_sum(const Array<Bool> &a)
{
	return a.contiguousStorage() ?
			std::accumulate(a.cbegin(), a.cend(), 0) :
			std::accumulate(a.begin(),  a.end(),  0);
}

void Flagger::printAgentRecord(String &agent_id, uInt agentCount,
		const RecordInterface &agent_rec){
	// but if an id field is set in the sub-record, use that instead
	if ( agent_rec.isDefined("id") && agent_rec.dataType("id") == TpString ){
		agent_id = agent_rec.asString("id");
	}
	for(uInt i=0; i<agent_rec.nfields(); i++){
		os << agent_id << "[" << agentCount+1 << "] : ";
		String myName(agent_rec.name(i));
		os << myName << ": ";
		switch(agent_rec.type(i)){
		case TpRecord :
			printAgentRecord(myName, i, agent_rec.asRecord(i));
			break;
		case TpArrayBool :
			os << agent_rec.asArrayBool(i);
			break;
		case TpArrayUChar :
			os << agent_rec.asArrayuChar(i);
			break;
		case TpArrayShort:
			os << agent_rec.asArrayShort(i);
			break;
		case TpArrayInt:
			os << agent_rec.asArrayInt(i);
			break;
		case TpArrayUInt:
			os << agent_rec.asArrayuInt(i);
			break;
		case TpArrayFloat:
			os << agent_rec.asArrayFloat(i);
			break;
		case TpArrayDouble:
			os << agent_rec.asArrayDouble(i);
			break;
		case TpArrayComplex:
			os << agent_rec.asArrayComplex(i);
			break;
		case TpArrayDComplex:
			os << agent_rec.asArrayDComplex(i);
			break;
		case TpArrayString:
			os << agent_rec.asArrayString(i);
			break;
		case TpBool:
			os << agent_rec.asBool(i);
			break;
		case TpUChar:
			os << agent_rec.asuChar(i);
			break;
		case TpShort:
			os << agent_rec.asShort(i);
			break;
		case TpInt:
			os << agent_rec.asInt(i);
			break;
		case TpUInt:
			os << agent_rec.asuInt(i);
			break;
		case TpFloat:
			os << agent_rec.asFloat(i);
			break;
		case TpDouble:
			os << agent_rec.asDouble(i);
			break;
		case TpComplex:
			os << agent_rec.asComplex(i);
			break;
		case TpDComplex:
			os << agent_rec.asDComplex(i);
			break;
		case TpString:
			os << agent_rec.asString(i);
			break;
		default :
			break;
		}
		os << endl << LogIO::POST;
	}
	//
}

// -----------------------------------------------------------------------
// Flagger::run
// Performs the actual flagging
// -----------------------------------------------------------------------
Record Flagger::run (Bool trial, Bool reset)
{
	if (!agents_p)
	{
		agentCount_p = 0;
		agents_p = new Record;
		if (dbg) cout << "creating new EMPTY agent and returning" << endl;
		return Record();
	}
	Record agents = *agents_p;

	if (dbg) cout << agents << endl;

	if (!opts_p)
	{
		opts_p = new Record();
	}
	*opts_p = defaultOptions();
	opts_p->define(RF_RESET,reset);
	opts_p->define(RF_TRIAL,trial);
	Record opt = *opts_p;

	if (!setdata_p) {
		os << LogIO::SEVERE << "Please run setdata with/without arguments before any setmethod"
				<< LogIO::POST;
		return Record();
	}


	Record result;

	//printflagselections();

	if ( !nant )
		os<<"No Measurement Set has been attached\n"<<LogIO::EXCEPTION;

	RFABase::setIndexingBase(0);
	// set debug level
	Int debug_level = 0;
	if ( opt.isDefined("debug") )
		debug_level = opt.asInt("debug");

	// reset existing flags?
	Bool reset_flags = isFieldSet(opt, RF_RESET);

	/* Don't use the progmeter if less than this
       number of timestamps (for performance reasons;
       just creating a ProgressMeter is relatively
       expensive). */
	const unsigned progmeter_limit = 1000;

	try { // all exceptions to be caught below

		bool didSomething=0;
		// create iterator, visbuffer & chunk manager
		// Block<Int> sortCol(1);
		// sortCol[0] = MeasurementSet::SCAN_NUMBER;
		//sortCol[0] = MeasurementSet::TIME;
		// Setdata already made a data selection

		VisibilityIterator &vi(vs_p->iter());
		vi.slurp();
		VisBuffer vb(vi);

		RFChunkStats chunk(vi, vb,
				*this);

		// setup global options for flagging agents
		Record globopt(Record::Variable);
		if ( opt.isDefined(RF_GLOBAL) )
			globopt = opt.asRecord(RF_GLOBAL);

		// generate new array of agents by iterating through agents record
		Record agcounts; // record of agent instance counts

		acc.resize(agents.nfields());

		for (uInt i=0; i<agents.nfields(); i++) {
			acc[i] = std::shared_ptr<RFABase>();
		}

		uInt nacc = 0;
		bool only_selector = true;  // only RFASelector agents?
		for (uInt i=0; i<agents.nfields(); i++) {
			if (  agents.dataType(i) != TpRecord )
				os << "Unrecognized field '" << agents.name(i) << "' in agents\n" << LogIO::EXCEPTION;

			const RecordInterface & agent_rec( agents.asRecord(i) );

			// normally, the field name itself is the agent ID

			String agent_id( downcase(agents.name(i)) );

			// but if an id field is set in the sub-record, use that instead
			if ( agent_rec.isDefined("id") && agent_rec.dataType("id") == TpString )
			{
				agent_id = agent_rec.asString("id");
			}
			// check that this is agent really exists
			if ( !agent_defaults.isDefined(agent_id) )
			{
				//cerr << agent_defaults;
				os << "Unknown flagging method '" <<
						agents.name(i) << "'\n" << LogIO::EXCEPTION;
			}

			// create parameter record by taking agent defaults, and merging in global
			// and specified options
			const RecordInterface & defparms(agent_defaults.asRecord(agent_id));
			Record parms(defparms);
			parms.merge(globopt,Record::OverwriteDuplicates);
			parms.merge(agent_rec,Record::OverwriteDuplicates);

			// add the global reset argumnent
			parms.define(RF_RESET,reset_flags);

			// see if this is a different instance of an already activated agent
			if (agcounts.isDefined(agent_id)) {
				// increment the instance counter
				Int count = agcounts.asInt(agent_id)+1;
				agcounts.define(agent_id,count);

				// modify the agent name to include an instance count
				char s[1024];
				sprintf(s,"%s#%d",defparms.asString(RF_NAME).chars(),count);
				parms.define(RF_NAME,s);
			}
			else
				agcounts.define(agent_id,1);
			// create agent based on name
			std::shared_ptr<RFABase> agent =
					createAgent(agent_id,
							chunk,
							parms,
							only_selector);
			if ( agent.get() == NULL )
				os<<"Unrecognized method name '"<<agents.name(i)<<"'\n"<<LogIO::EXCEPTION;
			agent->init();
			agent->setNAgent(agents.nfields());
			String inp,st;
			//    agent->logSink()<<agent->getDesc()<<endl<<LogIO::POST;
			acc[nacc++] = agent;
		}

		for (unsigned i = 0; i < nacc; i++) {
			acc[i]->setOnlySelector(only_selector);
		}
		acc.resize(nacc);

		// begin iterating over chunks
		uInt nchunk=0;

		bool new_field_spw = true;   /* Is the current chunk the first chunk for this (field,spw)? */

		Int64 inRowFlags=0, outRowFlags=0, totalRows=0, inDataFlags=0, outDataFlags=0, totalData=0;

		for (vi.originChunks();
				vi.moreChunks();
		) { //vi.nextChunk(), nchunk++) {
			//Start of loop over chunks
			didSomething = false;
			for (uInt i = 0; i < acc.size(); i++) {
				acc[i]->initialize();
			}

			chunk.newChunk(quack_agent_exists);

			// limit frequency of progmeter updates
			Int pm_update_freq = chunk.num(TIME)/200;

			// How much memory do we have?
			Int availmem = opt.isDefined("maxmem") ?
					opt.asInt("maxmem") : HostInfo::memoryTotal()/1024;
			if ( debug_level>0 )
				dprintf(os,"%d MB memory available\n",availmem);

			// call newChunk() for all accumulators; determine which ones are active
			Vector<Int> iter_mode(acc.size(),RFA::DATA);
			Vector<Bool> active(acc.size());

			for (uInt i = 0; i < acc.size(); i++)
			{
				Int maxmem;
				maxmem = availmem;
				if ( ! (active(i) = acc[i]->newChunk(maxmem))  ) // refused this chunk?
				{
					iter_mode(i) = RFA::STOP;  // skip over it
				}
				else
				{ // active, so reserve its memory
					if ( debug_level>0 )
						dprintf(os,"%s reserving %d MB of memory, %d left in pool\n",
								acc[i]->name().chars(),availmem-maxmem,maxmem);
					availmem = maxmem>0 ? maxmem : 0;
				}
			}
			if (dbg) cout << "Active for this chunk: " << my_aipspp_sum(active) << endl;

			// initially active agents
			Vector<Bool> active_init = active;
			// start executing passes
			char subtitle[1024];
			sprintf(subtitle,"Flagging %s chunk %d: ",ms.tableName().chars(),nchunk+1);
			String title(subtitle);
			//cout << "--------title=" << title << endl;

			if ( !sum(active) )
			{
				//os<<LogIO::WARN<<"Unable to process this chunk with any active method.\n"<<LogIO::POST;

				goto end_of_loop;
				/* Oh no, an evil goto statement...
                   This used to be a 
                      continue;
                   which (in this context) is just as evil. But goto is used because some
                   looping code (the visIter update) needs to be always executed at the
                   end of this loop.

                   The good solution would be to
                   -  make the remainder of this loop
                      work also in the special case with sum(active) == 0
                   -  simplify this overly long loop
				 */
			}

			for( uInt npass=0; anyNE(iter_mode,(Int)RFA::STOP); npass++ ) // repeat passes while someone is active
			{
				uInt itime=0;
				chunk.newPass(npass);
				// count up who wants a data pass and who wants a dry pass
				Int ndata = sum(iter_mode==(Int)RFA::DATA);
				Int ndry  = sum(iter_mode==(Int)RFA::DRY);
				Int nactive = ndata+ndry;
				if ( !nactive ) // no-one? break out then
					break;
				//	      didSomething++;
				// Decide when to schedule a full data iteration, and when do dry runs only.
				// There's probably room for optimizations here, but let's keep it simple
				// for now: since data iterations are more expensive, hold them off as long
				// as someone is requesting a dry run.
				Bool data_pass = !ndry;
				// Doing a full data iteration
				if ( data_pass )
				{

					sprintf(subtitle,"pass %d (data)",npass+1);

					std::unique_ptr<ProgressMeter> progmeter;
					if (chunk.num(TIME) > progmeter_limit) {
						progmeter = std::unique_ptr<ProgressMeter>(new ProgressMeter(1.0,static_cast<Double>(chunk.num(TIME)+0.001),title+subtitle,"","","",true,pm_update_freq));
					}

					// start pass for all active agents
					//cout << "-----------subtitle=" << subtitle << endl;
					for( uInt ival = 0; ival<acc.size(); ival++ )
						if ( active(ival) ) {
							if ( iter_mode(ival) == RFA::DATA )
								acc[ival]->startData(new_field_spw);
							else if ( iter_mode(ival) == RFA::DRY )
								acc[ival]->startDry(new_field_spw);
						}
					// iterate over visbuffers
					for( vi.origin(); vi.more() && nactive; vi++,itime++ ) {

						if (progmeter.get() != NULL) {
							progmeter->update(itime);
						}
						chunk.newTime();
						Bool anyActive = false;
						anyActive=false;
						for( uInt i = 0; i<acc.size(); i++ )
						{
							//if ((acc[i]->getID() != "FlagExaminer") &&
							if (active_init(i)) {
								anyActive=true;
							}
						}

						for(uInt i=0;i<acc.size();i++) {
							if (anyActive) acc[i]->initializeIter(itime);
						}

						inRowFlags += my_aipspp_sum(vb.flagRow());
						totalRows += vb.flagRow().nelements();
						totalData += vb.flagCube().shape().product();

						inDataFlags += my_aipspp_sum(vb.flagCube());

						// now, call individual VisBuffer iterators
						for( uInt ival = 0; ival<acc.size(); ival++ )
							if ( active(ival) ) {
								// call iterTime/iterDry as appropriate
								RFA::IterMode res = RFA::STOP;
								if ( iter_mode(ival) == RFA::DATA ) {
									res = acc[ival]->iterTime(itime);
								}
								else if ( iter_mode(ival) == RFA::DRY )
									res = acc[ival]->iterDry(itime);
								// change requested? Deactivate agent
								if ( ! ( res == RFA::CONT || res == iter_mode(ival) ) )
								{
									active(ival) = false;
									nactive--;
									iter_mode(ival)==RFA::DATA ? ndata-- : ndry--;
									iter_mode(ival) = res;
									if ( nactive <= 0 )
										break;
								}
							}

						// also iterate over rows for data passes
						for( Int ir=0; ir<vb.nRow() && ndata; ir++ ) {
							for( uInt ival = 0; ival<acc.size(); ival++ )
								if ( iter_mode(ival) == RFA::DATA )
								{
									RFA::IterMode res = acc[ival]->iterRow(ir);
									if ( ! ( res == RFA::CONT || res == RFA::DATA ) )
									{
										ndata--; nactive--;
										iter_mode(ival) = res;
										active(ival) = false;
										if ( ndata <= 0 )
											break;
									}
								}
						}

						for( uInt ival = 0; ival<acc.size(); ival++ ) {
							if ( active(ival) ) {
								acc[ival]->endRows(itime);
							}
						}
					} /* for vi... */

					// end pass for all agents
					for( uInt ival = 0; ival<acc.size(); ival++ )
					{
						if ( active(ival) ) {
							if ( iter_mode(ival) == RFA::DATA )
								iter_mode(ival) = acc[ival]->endData();
							else if ( iter_mode(ival) == RFA::DRY )
								iter_mode(ival) = acc[ival]->endDry();
						}
					}
				}
				else  // dry pass only
				{
					sprintf(subtitle,"pass %d (dry)",npass+1);
					//cout << "-----------subtitle=" << subtitle << endl;

					std::unique_ptr<ProgressMeter> progmeter;
					if (chunk.num(TIME) > progmeter_limit) {
						progmeter = std::unique_ptr<ProgressMeter>(new ProgressMeter (1.0,static_cast<Double>(chunk.num(TIME)+0.001),title+subtitle,"","","",true,pm_update_freq));
					}
					// start pass for all active agents
					for( uInt ival = 0; ival<acc.size(); ival++ )
						if ( iter_mode(ival) == RFA::DRY )
							acc[ival]->startDry(new_field_spw);
					for( uInt itime=0; itime<chunk.num(TIME) && ndry; itime++ )
					{
						if (progmeter.get() != NULL) {
							progmeter->update(itime);
						}
						// now, call individual VisBuffer iterators
						for( uInt ival = 0; ival<acc.size(); ival++ )
							if ( iter_mode(ival) == RFA::DRY )
							{
								// call iterTime/iterDry as appropriate
								RFA::IterMode res = acc[ival]->iterDry(itime);
								// change requested? Deactivate agent
								if ( ! ( res == RFA::CONT || res == RFA::DRY ) )
								{
									iter_mode(ival) = res;
									active(ival) = false;
									if ( --ndry <= 0 )
										break;
								}
							}
					}
					// end pass for all agents
					for( uInt ival = 0; ival<acc.size(); ival++ )
						if ( iter_mode(ival) == RFA::DRY )
							iter_mode(ival) = acc[ival]->endDry();
				} // end of dry pass
			} // end loop over passes

			//cout << opt << endl;
			//cout << "any active = " << active_init << endl;

			if ( !isFieldSet(opt, RF_TRIAL) && anyNE(active_init, false) )
			{
				sprintf(subtitle,"pass (flag)");
				//cout << "-----------subtitle=" << subtitle << endl;

				std::unique_ptr<ProgressMeter> progmeter;
				if (chunk.num(TIME) > progmeter_limit) {
					progmeter = std::unique_ptr<ProgressMeter>(new ProgressMeter(1.0,static_cast<Double>(chunk.num(TIME)+0.001),title+"storing flags","","","",true,pm_update_freq));
				}
				for (uInt i = 0; i<acc.size(); i++)
					if (active_init(i))
						acc[i]->startFlag(new_field_spw);
				uInt itime=0;
				for( vi.origin(); vi.more(); vi++,itime++ ) {
					if (progmeter.get() != NULL) {
						progmeter->update(itime);
					}

					chunk.newTime();
					//		  inRowFlags += sum(chunk.nrfIfr());

					Bool anyActive = false;
					anyActive=false;
					for( uInt i = 0; i<acc.size(); i++ ) {
						//		      cout << i << " " << acc[i]->getID() << " " << active_init(i) << endl;
						if ((acc[i]->getID() != "FlagExaminer") &&
								active_init(i))
							anyActive=true;
					}

					//cout << "anyActive" << anyActive << endl;

					didSomething = (anyActive==true);
					for( uInt i = 0; i<acc.size(); i++ ) {
						if ( active_init(i) ) {
							//if (acc[i]->getID() != "FlagExaminer" )
							acc[i]->iterFlag(itime);
						}
						if (anyActive) acc[i]->finalizeIter(itime);
					}

					//		  outRowFlags += sum(chunk.nrfIfr());

					outRowFlags += my_aipspp_sum(vb.flagRow());
					outDataFlags += my_aipspp_sum(vb.flagCube());

				}  // for (vi ... ) loop over time
				if (didSomething) {
					for (uInt i = 0; i < acc.size(); i++) {
						if (acc[i].get()) acc[i]->finalize();
					}
				}
				for (uInt i = 0; i < acc.size(); i++) {

					if ( active_init(i) ) {

						acc[i]->endFlag();
						// summary mode prints here

						// cerr << "Agent = " << acc[i]->getID() << endl;
						// cerr << "Agent's name = " << acc[i]->name() << endl;
					}
				}
				{
					logSink_p.clearLocally();
					LogIO oss(LogOrigin("Flagger", "run()"), logSink_p);
					os=oss;
				}
			} /* end if not trial run and some agent is active */

			end_of_loop:

			// call endChunk on all agents
			for( uInt i = 0; i<acc.size(); i++ )
				acc[i]->endChunk();

			int field_id = chunk.visBuf().fieldId();
			int spw_id = chunk.visBuf().spectralWindow();
			int scan_number = chunk.visBuf().scan()(0);


			/* Is this the end of the current (field, spw)? */
			new_field_spw = ( !vi.moreChunks() ||
					!scan_looping ||
					! (chunk.visBuf().fieldId() == field_id &&
							chunk.visBuf().spectralWindow() == spw_id &&
							chunk.visBuf().scan()(0) > scan_number )
			);

			if (didSomething && new_field_spw) {

				LogIO osss(LogOrigin("Flagger", "run"),logSink_p);

				stringstream ss;
				ss << "Chunk = " << chunk.nchunk()
				 << ", Field = " << field_id << " (" << chunk.visIter().fieldName() << ")"
				 << ", Spw Id = "  << spw_id << ", Corrs = [" << chunk.getCorrString() << "]"
				 << ", nTime = " << vi.nSubInterval() << ", Total rows = " << totalRows
				 << ", Total data = " << totalData << endl;
				ss << "Input:    "
						<< "  Rows flagged = " << inRowFlags << " "
						<< "(" << 100.0*inRowFlags/totalRows << " %)."
						<< "  Data flagged = " << inDataFlags << " "
						<< "(" << 100.0*inDataFlags/totalData << " %)."
						<< endl;
				ss << "This run: "
						<< "  Rows flagged = " << outRowFlags - inRowFlags << " "
						<< "(" << 100.0*(outRowFlags-inRowFlags)/totalRows << " %)."
						<< "  Data flagged = "  << outDataFlags - inDataFlags << " "
						<< "(" << 100.0*(outDataFlags-inDataFlags)/totalData << " %)."
						<< endl;
				ss << "------------------------------------------------------------------------------------" << endl;

				osss << ss.str() << LogIO::POST;

				/* Reset counters */
				inRowFlags = outRowFlags = totalRows = inDataFlags = outDataFlags = totalData = 0;
			}

			// CAS-2798: moved vi.nextChunk() to here to avoid printing the wrong information above
			vi.nextChunk(); nchunk++;

		} // end loop over chunks

		// get results for all agents
		// (gets just last agent; doesn't work for multiple agents,
		// but only used in mode summary)
		for (uInt i = 0; i < acc.size(); i++) {
			result = acc[i]->getResult();
		}

		if (dbg) {
			cout << "Total number of data chunks : " << nchunk << endl;
		}

	}
	catch(AipsError &x)
	{
		throw;
	}
	catch(std::exception &e)
	{
		throw AipsError(e.what());
	}

	ms.flush();
	//os<<"Flagging complete\n"<<LogIO::POST;

	/* Clear the current flag selections */
	clearflagselections(0);

	return result;
}

// -----------------------------------------------------------------------
// printSummaryReport
// Generates a summary flagging report for current chunk
// -----------------------------------------------------------------------
void Flagger::printSummaryReport (RFChunkStats &chunk)
{
	if (dbg) cout << "Flagger:: printSummaryReport" << endl;
	// generate a short text report in the first pane
	char s[1024];
	sprintf(s,"MS '%s'\nchunk %d (field %s, spw %d)",ms.tableName().chars(),
			chunk.nchunk(),chunk.visIter().fieldName().chars(),chunk.visIter().spectralWindow());
	os << "---------------------------------------------------------------------" << LogIO::POST;
	os<<s<<LogIO::POST;

	// print overall flagging stats
	uInt n=0,n0;

	sprintf(s,"%s, %d channels, %d time slots, %d baselines, %d rows\n",
			chunk.getCorrString().chars(),chunk.num(CHAN),chunk.num(TIME),
			chunk.num(IFR),chunk.num(ROW));
	os<<s<<LogIO::POST;

	// % of rows flagged
	n  = sum(chunk.nrfIfr());
	n0 = chunk.num(ROW);
	sprintf(s,"%d (%0.2f%%) rows are flagged (all baselines/times/chans/corrs in this chunk).",n,n*100.0/n0);
	os<<s<<LogIO::POST;

	// % of data points flagged
	n  = sum(chunk.nfIfrTime());
	n0 = chunk.num(ROW)*chunk.num(CHAN)*chunk.num(CORR);
	sprintf(s,"%d of %d (%0.2f%%) data points are flagged (all baselines/times/chans/corrs in this chunk).",n,n0,n*100.0/n0);
	os<<s<<LogIO::POST;
	//os << "---------------------------------------------------------------------" << LogIO::POST;

	// % flagged per baseline (ifr)
	// % flagged per timestep (itime)
	// // there is info about (ifr,itime)
	// % flagged per antenna (ifr) -> decompose into a1,a2
	// % flagged per channel/corr -> (ich,ifr)
	// % flagged per ( field, spw, array, scan ) are chunks - i think !!!


	// print per-agent flagging summary
	/*
      for( uInt i=0; i<acc.size(); i++ )
      {
      String name(acc[i]->name() + "["+i+"]"+": ");
      String stats( acc[i]->isActive() ? acc[i]->getStats() : String("can't process this chunk") );
      os<<name+stats<<LogIO::POST;
      }
	 */
	if (dbg) cout << "end of.... Flagger:: printSummaryReport" << endl;
}

// -----------------------------------------------------------------------
// printAgentReport
// Generates per-agent reports for current chunk of data
// Meant to be called before doing endChunk() on all the flagging
// agents.
// -----------------------------------------------------------------------
void Flagger::printAgentReports( )
{
	// call each agent to produce summary plots
	for( uInt i=0; i<acc.size(); i++ )
		acc[i]->printFlaggingReport();
}

/* FLAG VERSION SUPPORT */
Bool Flagger::saveFlagVersion(String versionname, String comment, String merge )
{
	try
	{
		FlagVersion fv(originalms.tableName(),"FLAG","FLAG_ROW");
		fv.saveFlagVersion(versionname, comment, merge);
	}
	catch (AipsError x)
	{
		os << LogIO::SEVERE << "Could not save Flag Version : " << x.getMesg() << LogIO::POST;
		throw;
	}
	return true;
}
Bool Flagger::restoreFlagVersion(Vector<String> versionname, String merge )
{
	try
	{
		FlagVersion fv(originalms.tableName(),"FLAG","FLAG_ROW");
		for(Int j=0;j<(Int)versionname.nelements();j++)
			fv.restoreFlagVersion(versionname[j], merge);
	}
	catch (AipsError x)
	{
		os << LogIO::SEVERE << "Could not restore Flag Version : " << x.getMesg() << LogIO::POST;
		return false;
	}
	return true;
}
Bool Flagger::deleteFlagVersion(Vector<String> versionname)
{
	try
	{
		FlagVersion fv(originalms.tableName(),"FLAG","FLAG_ROW");
		for(Int j=0;j<(Int)versionname.nelements();j++)
			fv.deleteFlagVersion(versionname[j]);
	}
	catch (AipsError x)
	{
		os << LogIO::SEVERE << "Could not delete Flag Version : " << x.getMesg() << LogIO::POST;
		return false;
	}
	return true;
}
Bool Flagger::getFlagVersionList(Vector<String> &verlist)
{
	try
	{
		verlist.resize(0);
		Int num;
		FlagVersion fv(originalms.tableName(),"FLAG","FLAG_ROW");
		Vector<String> vlist = fv.getVersionList();

		num = verlist.nelements();
		verlist.resize( num + vlist.nelements() + 1, true );
		verlist[num] = String("\nMS : ") + originalms.tableName() + String("\n");
		for(Int j=0;j<(Int)vlist.nelements();j++)
			verlist[num+j+1] = vlist[j];
	}
	catch (AipsError x)
	{
		os << LogIO::SEVERE << "Could not get Flag Version List : " << x.getMesg() << LogIO::POST;
		return false;
	}
	return true;
}




void Flagger::reform_baselinelist(Matrix<Int> &baselinelist, unsigned nant)
{
	for (unsigned i = 0; (int)i < baselinelist.shape()(1); i++) {
		int a1 = baselinelist(0, i);
		if (a1 < 0) {
			for (unsigned a = 0; a < nant; a++) {
				if (a != (unsigned) (-a1)) {
					IPosition longer = baselinelist.shape();
					longer(1) += 1;
					baselinelist.resize(longer);
					baselinelist(0, longer(1)-1) = a;
					baselinelist(1, longer(1)-1) = baselinelist(1, i);
				}
			}
		}
	}
}

// -----------------------------------------------------------------------
// dprintf
// Function for printfing stuff to a debug stream
// -----------------------------------------------------------------------
int dprintf( LogIO &os,const char *format, ...) 
{
	char str[1024];
	va_list ap;
	va_start(ap, format);
	int ret = vsnprintf(str, 1024, format, ap);
	va_end(ap);
	os << LogIO::DEBUGGING << str << LogIO::POST;
	return ret;
}

} //#end casa namespace
