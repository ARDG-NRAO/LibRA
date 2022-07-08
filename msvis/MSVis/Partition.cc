//# Partition.cc 
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

#include <msvis/MSVis/Partition.h>
#include <msvis/MSVis/SubMS.h>
#include <casacore/ms/MSSel/MSSelection.h>
//#include <ms/MSSel/MSTimeGram.h>
//#include <tables/TaQL/ExprNode.h>
#include <casacore/tables/Tables/RefRows.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <casacore/ms/MeasurementSets/MSMainColumns.h>
#include <casacore/coordinates/Coordinates/CoordinateUtil.h>
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
#include <msvis/MSVis/VisSet.h>
#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/VisChunkAverager.h>
#include <msvis/MSVis/VisIterator.h>
//#include <msvis/MSVis/VisibilityIterator.h>
#include <casacore/tables/DataMan/IncrementalStMan.h>
#include <casacore/tables/Tables/ScalarColumn.h>
#include <casacore/tables/Tables/ScaColDesc.h>
#include <casacore/tables/Tables/SetupNewTab.h>
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
#include <casacore/measures/Measures/MeasTable.h>
#include <casacore/casa/Quanta/MVTime.h>

using namespace casacore;
namespace casa {

//typedef ROVisibilityIterator ROVisIter;
//typedef VisibilityIterator VisIter;

Partition::Partition(String& theMS, Table::TableOption option) :
		  ms_p(MeasurementSet(theMS, option)),
		  mssel_p(ms_p),
		  msc_p(NULL),
		  mscIn_p(NULL),
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
		  maxnchan_p(1),
		  maxncorr_p(1)
{
}

Partition::Partition(MeasurementSet& ms) :
		  ms_p(ms),
		  mssel_p(ms_p),
		  msc_p(NULL),
		  mscIn_p(NULL),
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
		  maxnchan_p(1),
		  maxncorr_p(1)
{
}

Partition::~Partition()
{
	if(!msOut_p.isNull())
		msOut_p.flush();

	delete msc_p;
	msc_p = NULL;

	delete mscIn_p;
	mscIn_p = NULL;

	msOut_p = MeasurementSet();

	// parseColumnNames unavoidably has a static String and Vector<MS::PredefinedColumns>.
	// Collapse them down to free most of that memory.
	SubMS::parseColumnNames("None");
}

Bool Partition::selectSpw(const String& spwstr)
{
	LogIO os(LogOrigin("Partition", "selectSpw()"));

	// Partitioning by definition requires the output subtables to be the same as
	// the subset ones, with the side-effect that channel selection is not
	// allowed.  spw selection is OK, though, so frisk spwstr for colons.
	if(spwstr.contains(":")){
		os << LogIO::SEVERE
				<< "Partitioning does not permit selecting by channel.  (Selecting by spw is OK)\n"
				<< LogIO::POST;
		return false;
	}

	MSSelection mssel;
	String myspwstr(spwstr == "" ? "*" : spwstr);

	mssel.setSpwExpr(myspwstr);

	// Each row should have spw, start, stop, step
	Matrix<Int> chansel = mssel.getChanList(&ms_p, 1);

	if(chansel.nrow() > 0){         // Use myspwstr if it selected anything...
		spw_p   = chansel.column(0);
	}
	else{                            // select everything
		MSSpWindowColumns mySpwTab(ms_p.spectralWindow());

		spw_p.resize(mySpwTab.nrow());
		indgen(spw_p);
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
		std::set<Int>::iterator bsend = badSelSpwSlots.end();

		uInt j = 0;
		for(uInt k = 0; k < nSelSpw; ++k){
			if(badSelSpwSlots.find(k) == bsend){
				spwc[j] = spw_p[k];
				++j;
			}
		}
		spw_p.resize(ngoodSelSpwSlots);
		spw_p = spwc;
	}
	return true;
}

Bool Partition::setmsselect(const String& spw, const String& field,
		const String& baseline, const String& scan,
		const String& uvrange, const String& taql,
		const String& subarray, const String& intent,
		const String& obs)
{
	LogIO os(LogOrigin("Partition", "setmsselect()"));
	Bool  ok;

	String myspwstr(spw == "" ? "*" : spw);
	Record selrec = ms_p.msseltoindex(myspwstr, field);

	ok = selectSource(selrec.asArrayInt("field"));

	// All of the requested selection functions will be tried, even if an
	// earlier one has indicated its failure.  This allows all of the selection
	// strings to be tested, yielding more complete feedback for the user
	// (fewer retries).  This is a matter of taste, though.  If the selections
	// turn out to be slow, this function should return on the first false.

	if(!selectSpw(myspwstr)){
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

	return ok;
}

Bool Partition::selectSource(const Vector<Int>& fieldid)
{
	LogIO os(LogOrigin("Partition", "selectSource()"));
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

void Partition::selectArray(const String& subarray)
{
	arrayExpr_p = subarray;
	if(arrayExpr_p == "")      // Zap any old ones.
		arrayId_p.resize();
	// else arrayId_p will get set in makeSelection().
}

void Partition::selectTime(Double timeBin, String timerng)
{  
	timeBin_p   = timeBin;
	timeRange_p = timerng;
}


Bool Partition::makePartition(String& msname, String& colname,
		const Vector<Int>& tileShape, const String& combine)
{
	LogIO os(LogOrigin("Partition", "makePartition()"));
	try{
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
		const Vector<MS::PredefinedColumns> colNamesTok = SubMS::parseColumnNames(colname,
				ms_p);

		if(!makeSelection()){
			os << LogIO::SEVERE
					<< "Failed on selection: the combination of spw, field, antenna, correlation, "
					<< "and timerange may be invalid."
					<< LogIO::POST;
			ms_p = MeasurementSet();
			return false;
		}
		mscIn_p = new MSColumns(mssel_p);
		// Note again the parseColumnNames() a few lines back that stops setupMS()
		// from being called if the MS doesn't have the requested columns.
		MeasurementSet* outpointer=0;

		if(tileShape.nelements() == 3){
			outpointer = setupMS(msname, mssel_p, maxnchan_p, maxncorr_p,
					colNamesTok, tileShape);

		}
		// the following calls MSTileLayout...  disabled for now because it
		// forces tiles to be the full spw bandwidth in width (gmoellen, 2010/11/07)
		/*
      else if((tileShape.nelements()==1) && (tileShape[0]==0 || tileShape[0]==1)){
      outpointer = setupMS(msname, nchan_p[0], ncorr_p[0],
      mscIn_p->observation().telescopeName()(0),
      colNamesTok, tileShape[0]);
      }
		 */
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

				outpointer = setupMS(msname, mssel_p, maxnchan_p, maxncorr_p,
						colNamesTok, dataTileShape);

			}
			else
				//Sweep all other cases of bad tileshape to a default one.
				//  (this probably never happens)
				outpointer = setupMS(msname, mssel_p, maxnchan_p, maxncorr_p,
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
	}
	catch(AipsError x){
		ms_p=MeasurementSet();
		throw(x);
	}
	catch(...){
		ms_p=MeasurementSet();
		throw(AipsError("Unknown exception caught"));
	}

}

MeasurementSet* Partition::makeScratchPartition(const String& colname,
		const Bool forceInMemory)
{
	return makeScratchPartition(SubMS::parseColumnNames(colname, ms_p),
			forceInMemory);
}

MeasurementSet* Partition::makeScratchPartition(const Vector<MS::PredefinedColumns>& whichDataCols,
		const Bool forceInMemory)
{
	LogIO os(LogOrigin("Partition", "makeScratchPartition()"));

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
	String msname=AppInfo::workFileName(uInt(sizeInMB), "TempPartition");

	MeasurementSet* outpointer = setupMS(msname, mssel_p, maxnchan_p, maxncorr_p,
			mscIn_p->observation().telescopeName()(0),
			whichDataCols);

	outpointer->markForDelete();
	//Hmmmmmm....memory......
	if(sizeInMB <  (Double)(HostInfo::memoryTotal())/(2048.0)
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

Bool Partition::fillAllTables(const Vector<MS::PredefinedColumns>& datacols)
{
	LogIO os(LogOrigin("Partition", "fillAllTables()"));
	Bool success = true;

	// Copy the subtables before doing anything with the main table.  Otherwise
	// MSColumns won't work.

	// fill or update
	Timer timer;

	// Force the Measures frames for all the time type columns to have the same
	// reference as the TIME column of the main table.
	// Disable the empty table check (with false) because some of the subtables
	// (like POINTING) might already have been written.
	// However, empty tables are still empty after setting up the reference codes
	// here.
	msc_p = new MSMainColumns(msOut_p);
	msc_p->setEpochRef(MEpoch::castType(mscIn_p->timeMeas().getMeasRef().getType()),
			false);

	// UVW is the only other Measures column in the main table.
	msc_p->uvwMeas().setDescRefCode(Muvw::castType(mscIn_p->uvwMeas().getMeasRef().getType()));

	if(timeBin_p <= 0.0)
		success &= fillMainTable(datacols);
	else
		success &= doTimeAver(datacols);
	return success;
}

Bool Partition::makeSelection()
{
	LogIO os(LogOrigin("Partition", "makeSelection()"));

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
		if(ms_p.nrow() == sorted.nrow())
			elms = &sorted;
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

	TableExprNode exprNode = thisSelection.toTableExprNode(elms);
	selTimeRanges_p = thisSelection.getTimeList();

	// Find the maximum # of channels and correlations, for setting the
	// tileshape.  VisIterator is horrible if the tileshape is too large.
	// Partition does not use VisIterator, so the max # of chans and corrs may
	// not be optimal.  However, the # of chans and corrs for DDID 0 may be very
	// misrepresentative of the MS in general.
	{
		ScalarColumn<Int> polId(elms->dataDescription(),
				MSDataDescription::columnName(MSDataDescription::POLARIZATION_ID));
		ScalarColumn<Int> spwId(elms->dataDescription(),
				MSDataDescription::columnName(MSDataDescription::SPECTRAL_WINDOW_ID));
		ScalarColumn<Int> ncorr(elms->polarization(),
				MSPolarization::columnName(MSPolarization::NUM_CORR));
		ScalarColumn<Int> nchan(elms->spectralWindow(),
				MSSpectralWindow::columnName(MSSpectralWindow::NUM_CHAN));

		uInt nddids = polId.nrow();
		uInt nSelSpws = spw_p.nelements();

		maxnchan_p = 1;
		maxncorr_p = 1;
		for(uInt ddid = 0; ddid < nddids; ++ddid){
			Int spw = spwId(ddid);
			for(uInt k = 0; k < nSelSpws; ++k){
				if(spw == spw_p[k]){			// If spw is selected...
					if(nchan(spw) > maxnchan_p)
						maxnchan_p = nchan(spw);
					if(ncorr(polId(ddid)) > maxncorr_p)
						maxncorr_p = ncorr(polId(ddid));
				}
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

	if(mssel_p.nrow() < ms_p.nrow()){
		os << LogIO::NORMAL
				<< mssel_p.nrow() << " out of " << ms_p.nrow() << " rows are going to be"
				<< " considered due to the selection criteria."
				<< LogIO::POST;
	}
	return true;
}

MeasurementSet* Partition::setupMS(const String& outFileName, const MeasurementSet& inms,
		const Int nchan, const Int nCorr, const String& telescop,
		const Vector<MS::PredefinedColumns>& colNames,
		const Int obstype)
{
	//Choose an appropriate tileshape
	IPosition dataShape(2, nCorr, nchan);
	IPosition tileShape = MSTileLayout::tileShape(dataShape, obstype, telescop);
	return setupMS(outFileName, inms, nchan, nCorr, colNames, tileShape.asVector());
}
MeasurementSet* Partition::setupMS(const String& outFileName, const MeasurementSet& inms,
		const Int nchan, const Int nCorr,
		const Vector<MS::PredefinedColumns>& colNamesTok,
		const Vector<Int>& tshape)
{

	if(tshape.nelements() != 3)
		throw(AipsError("TileShape has to have 3 elements ") );

	// This is more to shush a compiler warning than to warn users.
	LogIO os(LogOrigin("Partition", "setupMS()"));
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
	const Bool mustWriteOnlyToData = SubMS::mustConvertToData(ncols, colNamesTok);

	if (mustWriteOnlyToData)
	{

		MS::addColumnToDesc(td, MS::DATA, 2);
		String hcolName=String("Tiled")+String("DATA");
		td.defineHypercolumn(hcolName, 3,
				stringToVector("DATA"));
		tiledDataNames.resize(1);
		tiledDataNames[0] = hcolName;

	}
	else{

		tiledDataNames.resize(ncols);
		for(uInt i = 0; i < ncols; ++i){
			// Unfortunately MS::PredefinedColumns aren't ordered so that I can just check if
			// colNamesTok[i] is in the "data range".
			if(colNamesTok[i] == MS::DATA ||
					colNamesTok[i] == MS::MODEL_DATA ||
					colNamesTok[i] == MS::CORRECTED_DATA ||
					colNamesTok[i] == MS::FLOAT_DATA ||
					colNamesTok[i] == MS::LAG_DATA)
				MS::addColumnToDesc(td, colNamesTok[i], 2);
			else
				throw(AipsError(MS::columnName(colNamesTok[i]) +
						" is not a recognized data column "));

			String hcolName = String("Tiled") + MS::columnName(colNamesTok[i]);
			td.defineHypercolumn(hcolName, 3,
					stringToVector(MS::columnName(colNamesTok[i])));
			tiledDataNames[i] = hcolName;
		}

	}

	// add this optional column because random group fits has a
	// weight per visibility
	MS::addColumnToDesc(td, MS::WEIGHT_SPECTRUM, 2);

	//     td.defineHypercolumn("TiledDATA",3,
	//                           stringToVector(MS::columnName(MS::DATA)));
	td.defineHypercolumn("TiledFlag",3,
			stringToVector(MS::columnName(MS::FLAG)));
	td.defineHypercolumn("TiledFlagCategory",4,
			stringToVector(MS::columnName(MS::FLAG_CATEGORY)));
	td.defineHypercolumn("TiledWgtSpectrum",3,
			stringToVector(MS::columnName(MS::WEIGHT_SPECTRUM)));
	td.defineHypercolumn("TiledUVW",2,
			stringToVector(MS::columnName(MS::UVW)));
	td.defineHypercolumn("TiledWgt",2,
			stringToVector(MS::columnName(MS::WEIGHT)));
	td.defineHypercolumn("TiledSigma", 2,
			stringToVector(MS::columnName(MS::SIGMA)));


	SetupNewTable newtab(outFileName, td, Table::New);

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

	// Bind the DATA, FLAG & WEIGHT_SPECTRUM columns to the tiled stman

	if (mustWriteOnlyToData){
		TiledShapeStMan tiledStMan1Data("TiledDATA",tileShape);

		newtab.bindColumn(MS::columnName(MS::DATA), tiledStMan1Data);
	}
	else{
		for(uInt i = 0; i < ncols; ++i){
			TiledShapeStMan tiledStMan1Data(tiledDataNames[i], tileShape);

			newtab.bindColumn(MS::columnName(colNamesTok[i]), tiledStMan1Data);
		}

	}
	newtab.bindColumn(MS::columnName(MS::FLAG),tiledStMan1f);
	newtab.bindColumn(MS::columnName(MS::FLAG_CATEGORY),tiledStMan1fc);
	newtab.bindColumn(MS::columnName(MS::WEIGHT_SPECTRUM),tiledStMan2);

	newtab.bindColumn(MS::columnName(MS::UVW),tiledStMan3);
	newtab.bindColumn(MS::columnName(MS::WEIGHT),tiledStMan4);
	newtab.bindColumn(MS::columnName(MS::SIGMA),tiledStMan5);

	// Copy the subtables to the output table BEFORE converting it to an MS or
	// adding (additional) locking.
	Table realtab(newtab);
	TableCopy::copySubTables(realtab, inms);
	realtab.flush();

	// avoid lock overheads by locking the table permanently
	TableLock lock(TableLock::AutoLocking);
	MeasurementSet *ms = new MeasurementSet(realtab.tableName(), lock, Table::Update);

	{ // Set the TableInfo

		TableInfo& info(ms->tableInfo());
		info.setType(TableInfo::type(TableInfo::MEASUREMENTSET));
		info.setSubType(String("UVFITS"));
		info.readmeAddLine
		("This is a measurement set Table holding astronomical observations");

	}

	return ms;
}

// This method is currently not called in Partition.  It should really be called
// in setupMS, but that has been made into a static method and it cannot be
// called there.  The ms argument is unused, but it is there to preserve the
// signature, even though it causes a compiler warning.
//
void Partition::verifyColumns(const MeasurementSet&, // ms,
		const Vector<MS::PredefinedColumns>& colNames)
{
	LogIO os(LogOrigin("Partition", "verifyColumns()"));
	for(uInt i = 0; i < colNames.nelements(); ++i){
		if(!ms_p.tableDesc().isColumn(MS::columnName(colNames[i]))){
			ostringstream ostr;
			ostr << "Desired column (" << MS::columnName(colNames[i])
			<< ") not found in the input MS (" << ms_p.tableName() << ").";
			throw(AipsError(ostr.str()));
		}
	}
}

Bool Partition::fillAccessoryMainCols(){
	LogIO os(LogOrigin("Partition", "fillAccessoryMainCols()"));
	uInt nrows = mssel_p.nrow();

	msOut_p.addRow(nrows, true);

	//#ifdef COPYTIMER
	Timer timer;
	timer.mark();
	//#endif
	msc_p->antenna1().putColumn(mscIn_p->antenna1().getColumn());
	msc_p->antenna2().putColumn(mscIn_p->antenna2().getColumn());
	os << LogIO::DEBUG1
			<< "Copying ANTENNA* took " << timer.real() << "s."
			<< LogIO::POST;

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

	timer.mark();
	msc_p->dataDescId().putColumn(mscIn_p->dataDescId().getColumn());
	msc_p->fieldId().putColumn(mscIn_p->fieldId().getColumn());
	msc_p->arrayId().putColumn(mscIn_p->arrayId().getColumn());
	msc_p->stateId().putColumn(mscIn_p->stateId().getColumn());
	msc_p->processorId().putColumn(mscIn_p->processorId().getColumn());
	msc_p->observationId().putColumn(mscIn_p->observationId().getColumn());
	os << LogIO::DEBUG1
			<< "Copying DDID, FIELD, ARRAY_ID, STATE, PROC, and OBS took "
			<< timer.real() << "s."
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

	timer.mark();
	return true;
}

Bool Partition::fillMainTable(const Vector<MS::PredefinedColumns>& colNames)
{  
	LogIO os(LogOrigin("Partition", "fillMainTable()"));
	Bool success = true;
	Timer timer;

	fillAccessoryMainCols();

	//Deal with data
	ArrayColumn<Complex> data;
	Vector<MS::PredefinedColumns> complexCols;
	const Bool doFloat = SubMS::sepFloat(colNames, complexCols);
	const uInt nDataCols = complexCols.nelements();
	const Bool writeToDataCol = SubMS::mustConvertToData(nDataCols, complexCols);

	// timer.mark();
	// msc_p->weight().putColumn(mscIn_p->weight());
	// os << LogIO::DEBUG1
	//    << "Copying weight took " << timer.real() << "s."
	//    << LogIO::POST;
	// timer.mark();
	// msc_p->sigma().putColumn(mscIn_p->sigma());
	// os << LogIO::DEBUG1
	//    << "Copying SIGMA took " << timer.real() << "s."
	//    << LogIO::POST;

	// timer.mark();
	// msc_p->flag().putColumn(mscIn_p->flag());
	// os << LogIO::DEBUG1
	//    << "Copying FLAG took " << timer.real() << "s."
	//    << LogIO::POST;

	os << LogIO::DEBUG1;
	Bool didFC = false;
	timer.mark();
	if(!mscIn_p->flagCategory().isNull() &&
			mscIn_p->flagCategory().isDefined(0)){
		IPosition fcshape(mscIn_p->flagCategory().shape(0));
		IPosition fshape(mscIn_p->flag().shape(0));

		// I don't know or care how many flag categories there are.
		if(fcshape(0) == fshape(0) && fcshape(1) == fshape(1)){
			msc_p->flagCategory().putColumn(mscIn_p->flagCategory());
			os << "Copying FLAG_CATEGORY took " << timer.real() << "s.";
			didFC = true;
		}
	}
	if(!didFC)
		os << "Deciding not to copy FLAG_CATEGORY took " << timer.real() << "s.";
	os << LogIO::POST;

	timer.mark();
	copyDataFlagsWtSp(complexCols, writeToDataCol);
	if(doFloat)
		msc_p->floatData().putColumn(mscIn_p->floatData());

	os << LogIO::DEBUG1
			<< "Total data read/write time = " << timer.real()
			<< LogIO::POST;

	return success;
}

Bool Partition::getDataColumn(ArrayColumn<Complex>& data,
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

Bool Partition::getDataColumn(ArrayColumn<Float>& data,
		const MS::PredefinedColumns colName)
{
	LogIO os(LogOrigin("Partition", "getDataColumn()"));

	if(colName != MS::FLOAT_DATA)
		os << LogIO::WARN
		<< "Using FLOAT_DATA (because it has type Float) instead of the requested "
		<< colName
		<< LogIO::POST;

	data.reference(mscIn_p->floatData());
	return true;
}

Bool Partition::putDataColumn(MSColumns& msc, ArrayColumn<Complex>& data, 
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

Bool Partition::copyDataFlagsWtSp(const Vector<MS::PredefinedColumns>& colNames,
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

	Matrix<Float> wtmat;
	viIn.originChunks();                                // Makes me feel better.
	const Bool doWtSp(viIn.existsWeightSpectrum());
	Cube<Float> wtsp;

	uInt ninrows = mssel_p.nrow();
	ProgressMeter meter(0.0, ninrows * 1.0, "partition", "rows copied", "", "",
			true, 1);
	uInt inrowsdone = 0;  // only for the meter.

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
			viIn.weightMat(wtmat);
			viOut.setWeightMat(wtmat);
			viIn.sigmaMat(wtmat);           // Yes, I'm reusing wtmat.
			viOut.setSigmaMat(wtmat);
			if(doWtSp){
				viIn.weightSpectrum(wtsp);
				viOut.setWeightSpectrum(wtsp);
			}

			for(Int colnum = colNames.nelements(); colnum--;){
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
	msOut_p.flush();
	return true;
}

Bool Partition::putDataColumn(MSColumns& msc, ArrayColumn<Float>& data, 
		const MS::PredefinedColumns colName,
		const Bool writeToDataCol)
{
	LogIO os(LogOrigin("Partition", "putDataColumn()"));

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

Bool Partition::doTimeAver(const Vector<MS::PredefinedColumns>& dataColNames)
{
	LogIO os(LogOrigin("Partition", "doTimeAver()"));

	os << LogIO::DEBUG1 // helpdesk ticket from Oleg Smirnov (ODU-232630)
			<< "Before msOut_p.addRow(): "
			<< Memory::allocatedMemoryInBytes() / (1024.0 * 1024.0) << " MB"
			<< LogIO::POST;

	Vector<MS::PredefinedColumns> cmplxColLabels;
	const Bool doFloat = SubMS::sepFloat(dataColNames, cmplxColLabels);
	const uInt nCmplx = cmplxColLabels.nelements();
	if(doFloat && cmplxColLabels.nelements() > 0)           // 2010-12-14
		os << LogIO::WARN
		<< "Using VisibilityIterator to average both FLOAT_DATA and another DATA column is extremely experimental."
		<< LogIO::POST;

	ArrayColumn<Complex> *outCmplxCols = new ArrayColumn<Complex>[nCmplx];
	getDataColMap(msc_p, outCmplxCols, nCmplx, cmplxColLabels);

	// We may need to watch for chunks (timebins) that should be split because of
	// changes in scan, etc. (CAS-2401).  The old split way would have
	// temporarily shortened timeBin, but vi.setInterval() does not work without
	// calling vi.originChunks(), so that approach does not work with
	// VisibilityIterator.  Instead, get VisibilityIterator's sort (which also
	// controls how the chunks are split) to do the work.

	// Already separated by the chunking.
	//const Bool watch_array(!combine_p.contains("arr")); // Pirate talk for "array".

	const Bool watch_scan(!combine_p.contains("scan"));
	const Bool watch_state(!combine_p.contains("state"));
	const Bool watch_obs(!combine_p.contains("obs"));
	uInt n_cols_to_watch = 4;     // At least.

	if(watch_scan)
		++n_cols_to_watch;
	if(watch_state)
		++n_cols_to_watch;
	if(watch_obs)
		++n_cols_to_watch;

	Block<Int> sort(n_cols_to_watch);
	uInt colnum = 1;

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
	sort[colnum] = MS::DATA_DESC_ID;
	++colnum;
	sort[colnum] = MS::TIME;
	++colnum;
	if(watch_obs)
		sort[colnum] = MS::OBSERVATION_ID;

	// MSIter tends to produce output INTERVALs that are longer than the
	// requested interval length, by ~0.5 input integrations for a random
	// timeBin_p.  Giving it timeBin_p - 0.5 * interval[0] removes the bias and
	// brings it almost in line with binTimes() (which uses -0.5 *
	// interval[bin_start]).
	//
	// late April 2011: MSIter removed the bias, which threw off the correction.
	//
	ROVisibilityIterator vi(mssel_p, sort,
			//timeBin_p - 0.5 * mscIn_p->interval()(0));
			timeBin_p);
	//vi.slurp();
	//cerr << "Finished slurping." << endl;

	const Bool doSpWeight = vi.existsWeightSpectrum();

	//os << LogIO::NORMAL2 << "outNrow = " << msOut_p.nrow() << LogIO::POST;
	uInt rowsdone = 0;    // Output rows, used for the RefRows.

	uInt ninrows = mssel_p.nrow();
	ProgressMeter meter(0.0, ninrows * 1.0, "partition", "rows averaged", "", "",
			true, 1);
	uInt inrowsdone = 0;  // only for the meter.

	VisChunkAverager vca(dataColNames, doSpWeight);

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

			//    relabelIDs();

			// avb.freqAveCubes();  // Watch out, weight must currently be handled separately.

			// // Fill in the nonaveraging values from slotv0.
			msc_p->antenna1().putColumnCells(rowstoadd, avb.antenna1());
			msc_p->antenna2().putColumnCells(rowstoadd, avb.antenna2());

			Vector<Int> arrID(rowsnow);
			arrID.set(avb.arrayId());
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
			msc_p->interval().putColumnCells(rowstoadd, avb.timeInterval());

			msc_p->observationId().putColumnCells(rowstoadd, avb.observationId());
			msc_p->processorId().putColumnCells(rowstoadd, avb.processorId());

			msc_p->scanNumber().putColumnCells(rowstoadd, avb.scan());
			msc_p->sigma().putColumnCells(rowstoadd, avb.sigmaMat());

			msc_p->stateId().putColumnCells(rowstoadd, avb.stateId());

			msc_p->time().putColumnCells(rowstoadd, avb.time());
			msc_p->timeCentroid().putColumnCells(rowstoadd, avb.timeCentroid());
			msc_p->uvw().putColumnCells(rowstoadd, avb.uvwMat());
			msc_p->weight().putColumnCells(rowstoadd, avb.weightMat());
			if(doSpWeight)
				msc_p->weightSpectrum().putColumnCells(rowstoadd,
						avb.weightSpectrum());
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
			<< "Post binning memory: "
			<< Memory::allocatedMemoryInBytes() / (1024.0 * 1024.0) << " MB"
			<< LogIO::POST;

	if(rowsdone < 1){
		os << LogIO::WARN
				<< "No rows were written.  Is all the selected input flagged?"
				<< LogIO::POST;
		return false;
	}
	return true;
}

void Partition::getDataColMap(MSMainColumns* msc, ArrayColumn<Complex>* mapper,
		uInt ntok,
		const Vector<MS::PredefinedColumns>& colEnums)
{
	// Set up a map from dataColumn indices to ArrayColumns in the output.
	// mapper has to be a pointer (gasp!), not a Vector, because
	// Vector<ArrayColumn<Complex> > mapper(ntok) would implicitly call
	// .resize(), which uses =, which is banned for ArrayColumn.

	if(SubMS::mustConvertToData(ntok, colEnums)){
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

} //#End casa namespace
