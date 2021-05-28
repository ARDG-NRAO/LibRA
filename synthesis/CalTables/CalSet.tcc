//# CalSet.cc: Implementation of Calibration parameter cache
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

#include <algorithm>
#include <synthesis/CalTables/CalSet.h>

#include <synthesis/CalTables/CalTable2.h>
//#include <synthesis/CalTables/CalTable.h>
#include <synthesis/CalTables/CalDescColumns2.h>
#include <synthesis/CalTables/SolvableVJTable.h>
#include <synthesis/CalTables/CalMainColumns2.h>

#include <synthesis/CalTables/TimeVarVJDesc.h>
#include <synthesis/CalTables/SolvableVJDesc.h>
#include <synthesis/CalTables/SolvableVJMRec.h>
#include <synthesis/CalTables/SolvableVJMCol.h>

#include <tables/Tables/TableDesc.h>
#include <tables/Tables/SetupNewTab.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/ScaColDesc.h>
#include <tables/Tables/ArrColDesc.h>
#include <tables/Tables/ScalarColumn.h>
#include <tables/Tables/ArrayColumn.h>
#include <tables/Tables/RefRows.h>

#include <casacore/casa/Arrays.h>
#include <casacore/scimath/Mathematics/MatrixMathLA.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/OS/Timer.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/OS/Path.h>
#include <casacore/casa/sstream.h>

#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogSink.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// ------------------------------------------------------------------

// From shape only, this is the solve context
template<class T> CalSet<T>::CalSet(const casacore::Int& nSpw) :
  calTableName_(""),
  nSpw_(nSpw),
  nPar_(0),
  nChan_(0),
  nElem_(0),
  nTime_(0),
  spwOK_(nSpw_,false),
  startChan_(nSpw_,0),  
  freq_(nSpw_,NULL),
  MJDStart_(nSpw_,NULL),
  MJDStop_(nSpw_,NULL),
  MJDTimeStamp_(nSpw_,NULL),
  fieldId_(nSpw_,NULL),
  fieldName_(nSpw_,NULL),
  sourceName_(nSpw_,NULL),
  par_(nSpw_,NULL),
  parOK_(nSpw_,NULL),
  parErr_(nSpw_,NULL),
  parSNR_(nSpw_,NULL),
  //  iSolutionOK_(nSpw_,NULL),
  iFit_(nSpw_,NULL),
  iFitwt_(nSpw_,NULL),
  solutionOK_(nSpw_,NULL),
  fit_(nSpw_,NULL),
  fitwt_(nSpw_,NULL)
{
  calTabDesc_=NULL;
  calTab_=NULL;
  svjmcol_=NULL;
};

// From shape only, this is the solve context
template<class T> CalSet<T>::CalSet(const casacore::Int& nSpw,
				    const casacore::Int& nPar,
				    const casacore::Vector<casacore::Int>& nChan,
				    const casacore::Int& nElem,
				    const casacore::Vector<casacore::Int>& nTime) :
  calTableName_(""),
  nSpw_(nSpw),
  nPar_(nPar),
  nChan_(nChan),
  nElem_(nElem),
  nTime_(nTime),
  spwOK_(nSpw_,false),
  startChan_(nSpw_,0),  
  freq_(nSpw_,NULL),
  MJDStart_(nSpw_,NULL),
  MJDStop_(nSpw_,NULL),
  MJDTimeStamp_(nSpw_,NULL),
  fieldId_(nSpw_,NULL),
  fieldName_(nSpw_,NULL),
  sourceName_(nSpw_,NULL),
  par_(nSpw_,NULL),
  parOK_(nSpw_,NULL),
  parErr_(nSpw_,NULL),
  parSNR_(nSpw_,NULL),
  //  iSolutionOK_(nSpw_,NULL),
  iFit_(nSpw_,NULL),
  iFitwt_(nSpw_,NULL),
  solutionOK_(nSpw_,NULL),
  fit_(nSpw_,NULL),
  fitwt_(nSpw_,NULL)
{
  calTabDesc_=NULL;
  calTab_=NULL;
  svjmcol_=NULL;
  // Resize caches
  inflate();  
};


// From existing CalTable name, this is apply context
template<class T> CalSet<T>::CalSet(const casacore::String& calTableName,
				    const casacore::String& select,
				    const casacore::Int& nSpw,
				    const casacore::Int& nPar,
				    const casacore::Int& nElem) : 
  calTableName_(calTableName),
  nSpw_(nSpw),
  nPar_(nPar),
  nChan_(nSpw,0),
  nElem_(nElem),
  nTime_(nSpw,0),
  spwOK_(nSpw_,false),
  startChan_(nSpw_,0),  
  freq_(nSpw_,NULL),
  MJDStart_(nSpw_,NULL),
  MJDStop_(nSpw_,NULL),
  MJDTimeStamp_(nSpw_,NULL),
  fieldId_(nSpw_,NULL),
  fieldName_(nSpw_,NULL),
  sourceName_(nSpw_,NULL),
  par_(nSpw_,NULL),
  parOK_(nSpw_,NULL),
  parErr_(nSpw_,NULL),
  parSNR_(nSpw_,NULL),
  //  iSolutionOK_(nSpw_,NULL),
  iFit_(nSpw_,NULL),
  iFitwt_(nSpw_,NULL),
  solutionOK_(nSpw_,NULL),
  fit_(nSpw_,NULL),
  fitwt_(nSpw_,NULL)
{
  calTabDesc_=NULL;
  calTab_=NULL;
  svjmcol_=NULL;

  // Fill from table
  load(calTableName,select);

  // Set spwOK_ according to available calibration
  setSpwOK();

}

template<class T> CalSet<T>::~CalSet() {
  deflate();
}

template<class T> void CalSet<T>::resize(const casacore::Int& nPar,
					 const casacore::Vector<casacore::Int>& nChan,
					 const casacore::Int& nElem,
					 const casacore::Vector<casacore::Int>& nTime) {
  nPar_=nPar;
  nChan_=nChan;
  nElem_=nElem;
  nTime_=nTime;

  inflate();

}



// Inflate cache to proper size
template<class T> void CalSet<T>::inflate() {
  
  // Construct shaped pointed-to objects in cache

  // TODO:
  //  Consider initialization value (per type)

  // Delete exiting cache
  deflate();

  for (casacore::Int ispw=0; ispw<nSpw_; ispw++) {
    casacore::uInt ntime=nTime_(ispw);
    if (ntime > 0) {

      freq_[ispw]         = new casacore::Vector<casacore::Double>(nChan_(ispw),0.0);
      
      MJDStart_[ispw]     = new casacore::Vector<casacore::Double>(ntime,0.0);
      MJDStop_[ispw]      = new casacore::Vector<casacore::Double>(ntime,0.0);
      MJDTimeStamp_[ispw] = new casacore::Vector<casacore::Double>(ntime,0.0);
      fieldName_[ispw]    = new casacore::Vector<casacore::String>(ntime,"");
      sourceName_[ispw]   = new casacore::Vector<casacore::String>(ntime,"");
      fieldId_[ispw]      = new casacore::Vector<casacore::Int>(ntime,-1);

      casacore::IPosition parshape(4,nPar_,nChan_(ispw),nElem_,ntime);
      par_[ispw]     = new casacore::Array<T>(parshape,(T)1.0);
      parOK_[ispw]   = new casacore::Array<casacore::Bool>(parshape,false);
      parErr_[ispw]  = new casacore::Array<casacore::Float>(parshape,0.0f);
      parSNR_[ispw]  = new casacore::Array<casacore::Float>(parshape,0.0f);

      //      iSolutionOK_[ispw]  = new casacore::Matrix<casacore::Bool>(nElem_,ntime,false);
      iFit_[ispw]         = new casacore::Matrix<casacore::Float>(nElem_,ntime,0.0f);
      iFitwt_[ispw]       = new casacore::Matrix<casacore::Float>(nElem_,ntime,0.0f);
      solutionOK_[ispw]   = new casacore::Vector<casacore::Bool>(ntime,false);
      fit_[ispw]          = new casacore::Vector<casacore::Float>(ntime,0.0f);
      fitwt_[ispw]        = new casacore::Vector<casacore::Float>(ntime,0.0f);
    }
  }

}




template<class T> void CalSet<T>::deflate() {
  
  // Delete parameter memory

  for (casacore::Int ispw=0; ispw<nSpw_; ispw++) {
    if (MJDStart_[ispw])     delete MJDStart_[ispw];
    if (MJDStop_[ispw])      delete MJDStop_[ispw];
    if (MJDTimeStamp_[ispw]) delete MJDTimeStamp_[ispw];
    if (fieldName_[ispw])    delete fieldName_[ispw];
    if (sourceName_[ispw])   delete sourceName_[ispw];
    if (fieldId_[ispw])      delete fieldId_[ispw];
    if (par_[ispw])          delete par_[ispw];
    if (parOK_[ispw])        delete parOK_[ispw];
    if (parErr_[ispw])       delete parErr_[ispw];
    if (parSNR_[ispw])       delete parSNR_[ispw];
    //    if (iSolutionOK_[ispw])  delete iSolutionOK_[ispw];
    if (iFit_[ispw])         delete iFit_[ispw];
    if (iFitwt_[ispw])       delete iFitwt_[ispw];
    if (solutionOK_[ispw])   delete solutionOK_[ispw];
    if (fit_[ispw])          delete fit_[ispw];
    if (fitwt_[ispw])        delete fitwt_[ispw];
    MJDStart_[ispw]=NULL;
    MJDStop_[ispw]=NULL;
    MJDTimeStamp_[ispw]=NULL;
    fieldName_[ispw]=NULL;
    sourceName_[ispw]=NULL;
    fieldId_[ispw]=NULL;
    par_[ispw]=NULL;
    parOK_[ispw]=NULL;
    parErr_[ispw]=NULL;
    parSNR_[ispw]=NULL;
    //    iSolutionOK_[ispw]=NULL;
    iFit_[ispw]=NULL;
    iFitwt_[ispw]=NULL;
    solutionOK_[ispw]=NULL;
    fit_[ispw]=NULL;
    fitwt_[ispw]=NULL;
  }
}



template<class T> void CalSet<T>::load (const casacore::String& file, 
					const casacore::String& select)
{
  // Load data from a calibration table
  // casacore::Input:
  //    file         const casacore::String&       Cal table name
  //    select       const casacore::String&       Selection string
  //


  //  cout << "CalSet::load...(nSpw_=" << nSpw_ << ")" << endl;

  casacore::Timer timer;
  timer.mark();
  
  casacore::LogMessage message(casacore::LogOrigin("CalSet","load"));
  
  // Decode the Jones matrix type
  /*
  casacore::Int jonesType = 0;
  if (nPar_ == 1) jonesType = 1;
  if (nPar_ == 2) jonesType = 2;
  if (nPar_ == 4) jonesType = 3;
  */

 // Open, select, sort the calibration table
  SolvableVisJonesTable svjtab(file);
  svjtab.select2(select);

  // Get no. of antennas and time slots
  casacore::Int numberAnt = svjtab.maxAntenna() + 1;

  //  cout << "Initial selection: " << timer.all_usec()/1.0e6 << endl;
  timer.mark();

  AlwaysAssert(numberAnt==nElem_,casacore::AipsError)

  casacore::Int nDesc=svjtab.nRowDesc();
  casacore::Vector<casacore::Int> spwmap(nDesc,-1);

  casacore::Vector<casacore::Int> nRowPerCDI;
  svjtab.rowsPerCalDescId(nRowPerCDI);

  //  cout << "Optimized CDI count: " << timer.all_usec()/1.0e6 << endl;
  timer.mark();

  for (casacore::Int idesc=0;idesc<nDesc;idesc++) {

    // This cal desc
    CalDescRecord* calDescRec = new CalDescRecord (svjtab.getRowDesc(idesc));

    // Get this spw ID
    casacore::Vector<casacore::Int> spwlist;
    calDescRec->getSpwId(spwlist);
    casacore::Int nSpw; spwlist.shape(nSpw);
    if (nSpw > 1) {};  // ERROR!!!  Should only be one spw per cal desc!
    spwmap(idesc)=spwlist(0);

    // Trap spwids that do not occur in the MS
    // (Since we rely on the casacore::MS meta info for the calibration solutions,
    //  we cannot identify spws that do not occur in the MS.)
    if (spwlist(0)>nSpw_-1)
      throw(casacore::AipsError("Caltable '"+file+"' contains spw = "+
		      casacore::String::toString(spwlist(0))+
		      " which does not occur in the MS.  Cannot proceed."));

    // In next few steps, need to watch for repeat spws in new cal descs!!

    // Get number of channels this spw
    casacore::Vector<casacore::Int> nchanlist;

    calDescRec->getNumChan(nchanlist);
    nChan_(spwmap(idesc))=nchanlist(0);

    // Get channel range / start channel
    casacore::Cube<casacore::Int> chanRange;
    calDescRec->getChanRange(chanRange);
    startChan_(spwmap(idesc))=chanRange(0,0,0);

    // Get slot count for this desc's spw
    nTime_(spwmap(idesc))=nRowPerCDI(idesc)/numberAnt;

    delete calDescRec;  
  }

  //  cout << "CalDesc sizeup: " << timer.all_usec()/1.0e6 << endl;
  //  cout << "nTime_ = " << nTime_ << endl;

  timer.mark();

  // At this point, we know how big our slot-dep caches must be
  //  (in private data), so initialize them
  inflate();

  // Remember if we found and filled any solutions
  casacore::Bool solfillok(false);

  //  cout << "CalSet inflated: " << timer.all_usec()/1.0e6 << endl;


  // Fill per caldesc
  casacore::Double ttime(0.0);
  for (casacore::Int idesc=0;idesc<nDesc;idesc++) {

    //    cout << "CDI = " << idesc << " "<< flush;

    timer.mark();

    casacore::Int thisSpw=spwmap(idesc);
      
    // Reopen and globally select caltable
    //SolvableVisJonesTable svjtabspw(file);
    CalTable2 svjtabspw(file);
    svjtabspw.select2(select);

    //    cout << " Sel: " << timer.all_usec()/1.0e6 << flush;

    // isolate this caldesc:
    std::ostringstream selectstr;
    selectstr << "CAL_DESC_ID == " << idesc;
    casacore::String caldescsel; caldescsel = selectstr.str();
    svjtabspw.select2(caldescsel);

    //    cout << " CDIsel: " << timer.all_usec()/1.0e6 << flush;

    //    cout << "Sorting..." << endl;
    casacore::Block<casacore::String> scol(1);
    scol[0]="TIME";
    svjtabspw.sort2(scol);

    //    cout << " casacore::Sort: " << timer.all_usec()/1.0e6 << flush;

    casacore::Int nrow = svjtabspw.nRowMain();
    casacore::IPosition out(3,0,0,0);   // par, chan, row
    casacore::IPosition in(4,0,0,0,0);  // par, chan, ant, slot
    if (nrow>0) {

      // Found some solutions to fill
      solfillok=true;

      // Ensure sorted on time
      casacore::Block <casacore::String> sortCol(1,"TIME");
      svjtabspw.sort2(sortCol);
      
      // Extract the gain table columns
      //ROSolvableVisJonesMCol svjmcol(svjtabspw);
      ROSolvableCalSetMCol<T> *svjmcol;
      svjmcol= new ROSolvableCalSetMCol<T>(svjtabspw);

      casacore::Vector<casacore::Int>    calDescId;  (*svjmcol).calDescId().getColumn(calDescId);
      casacore::Vector<casacore::Double> time;       (*svjmcol).time().getColumn(time);
      casacore::Vector<casacore::Double> interval;   (*svjmcol).interval().getColumn(interval);
      casacore::Vector<casacore::Int>    antenna1;   (*svjmcol).antenna1().getColumn(antenna1);
      //      cout << "antennas = " << antenna1 << endl;
      casacore::Vector<casacore::Int>    fieldId;    (*svjmcol).fieldId().getColumn(fieldId);
      //      cout << "fieldId = " << fieldId << endl;
      casacore::Vector<casacore::String> fieldName;  (*svjmcol).fieldName().getColumn(fieldName);
      casacore::Vector<casacore::String> sourceName; (*svjmcol).sourceName().getColumn(sourceName);
      casacore::Vector<casacore::Bool>   totalSolOk; (*svjmcol).totalSolnOk().getColumn(totalSolOk);
      casacore::Vector<casacore::Float>  totalFit;   (*svjmcol).totalFit().getColumn(totalFit);
      casacore::Vector<casacore::Float>  totalFitWt; (*svjmcol).totalFitWgt().getColumn(totalFitWt);
      casacore::Array<T> gain;       (*svjmcol).gain().getColumn(gain);
      casacore::Cube<casacore::Bool>     solOk;      (*svjmcol).solnOk().getColumn(solOk);
      casacore::Cube<casacore::Float>    fit;        (*svjmcol).fit().getColumn(fit);
      casacore::Cube<casacore::Float>    fitWt;      (*svjmcol).fitWgt().getColumn(fitWt);
      casacore::Cube<casacore::Bool>     flag;       (*svjmcol).flag().getColumn(flag);
      casacore::Cube<casacore::Float>    snr;        (*svjmcol).snr().getColumn(snr);

      // Read the calibration information
      casacore::Double /*lastTime(-1.0),*/ thisTime(0.0), thisInterval(0.0);
      casacore::Int islot(-1);
      casacore::Int iant;

      for (casacore::Int irow=0; irow<nrow; irow++) {
	out(2)=irow;

	thisTime=time(irow);
	
	// If this is a new solution
	if (irow%numberAnt==0) {

	  islot++;
	  in(3)=islot;
	  
	  thisInterval=interval(irow);
	  (*MJDTimeStamp_[thisSpw])(islot) = thisTime;
	  (*MJDStart_[thisSpw])(islot) = thisTime - thisInterval / 2.0;
	  (*MJDStop_[thisSpw])(islot) = thisTime + thisInterval / 2.0;
	  (*fieldId_[thisSpw])(islot) = fieldId(irow);
	  (*fieldName_[thisSpw])(islot) = fieldName(irow);
	  (*sourceName_[thisSpw])(islot) = sourceName(irow);
	  
	  (*solutionOK_[thisSpw])(islot) = totalSolOk(irow);
	  (*fit_[thisSpw])(islot) = totalFit(irow);
	  (*fitwt_[thisSpw])(islot) = totalFitWt(irow);
	  
	  //lastTime = thisTime;
	};
	
	iant=antenna1(irow);
	in(2)=iant;

	//	(*iSolutionOK_[thisSpw])(iant,islot) = solOk(0,0,irow);
	(*iFit_[thisSpw])(iant,islot) = fit(0,0,irow);
	(*iFitwt_[thisSpw])(iant,islot) = fitWt(0,0,irow);
	
	for (casacore::Int ichan=0; ichan<nChan_(thisSpw); ichan++) {


	  out(1)=in(1)=ichan;

	  for (casacore::Int ipar=0; ipar<nPar_; ipar++) {
	    in(0)=out(0)=ipar;
	    (*par_[thisSpw])(in)=gain(out);
	    (*parOK_[thisSpw])(in) = (solOk(out) && !flag(out));
	    (*parSNR_[thisSpw])(in) = snr(out);
	  }
	}
	
      } // irow
    } // nrow>0

    //    cout << "cs fields = " << *fieldId_[thisSpw] << endl;

    casacore::Double itime=timer.all_usec()/1.0e6;
    ttime+=itime;

    //    cout << " Totals: " << itime << " " << ttime << endl;

  } // idesc

  // If we found no solutions in selected table, abort:
  if (!solfillok) {
      throw(casacore::AipsError(" Specified cal table selection selects no solutions in this table.  Please review setapply settings."));
  }


};

template<class T> void CalSet<T>::initCalTableDesc(const casacore::String& type, const casacore::Int& parType)
{
  if (calTabDesc_) {delete calTabDesc_;calTabDesc_=NULL;}
  calTabDesc_=new CalTableDesc2(type,parType);
}

template<class T> void CalSet<T>::attach()
{
}


template<class T> void CalSet<T>::store (const casacore::String& file, 
					 const casacore::String& type,
					 const casacore::Bool& append,
					 const casacore::String& msname)
{
  // Write the solutions to an output calibration table
  // casacore::Input:
  //    file           casacore::String        Cal table name
  //    append         casacore::Bool          Append if true, else overwrite
  //

  // total rows to be written per Spw
  casacore::Vector<casacore::Int> nRow(nSpw_,0);
  for (casacore::Int iSpw=0;iSpw<nSpw_;iSpw++) 
    if (solutionOK_[iSpw])
      nRow(iSpw)=nElem_*ntrue(*(solutionOK_[iSpw]));

  // Escape if nothing to write
  if (sum(nRow)==0)
    throw(casacore::AipsError("No valid calibration solutions; no table written."));

  // Initialization:
  // No. of rows in cal_main, cal_desc and cal_history
  casacore::Int nMain = 0; 
  casacore::Int nDesc = 0;
  //casacore::Int nHist = 0;
  
  if (calTabDesc_ == NULL)
    {
      std::ostringstream str;
      str << "CalSet::store(): Perhaps CalSet.initCalTableDesc() was not called "
	  << "before calling CalSet::store()?"
	  << "  Jones = " << type << "   File = " << file;
      
      throw(casacore::AipsError(str.str()));
    }

  calTabDesc_->addDesc(calTabDesc_->defaultFitDesc(),calTabDesc_->calMainDesc());

  // Calibration table
  //  SolvableVisJonesTable *tab;
  
  // Open the output file if it already exists and is being appended to.
  if (calTab_) delete calTab_; calTab_=NULL;

  if (append && casacore::Table::isWritable (file)) {
    //    tab  = new SolvableVisJonesTable (file, casacore::Table::Update);
    calTab_  = new CalTable2 (file, *calTabDesc_, casacore::Table::Update);
    nMain = calTab_->nRowMain();
    nDesc = calTab_->nRowDesc();
    //nHist = calTab_->nRowHistory();
  } else {
    // Create a new calibration table
    casacore::Table::TableOption access = casacore::Table::New;
    //    tab = new SolvableVisJonesTable (file, type, access);
    calTab_ = new CalTable2 (file, *calTabDesc_, access);
  };
  
  // Write every spw w/ max number of channels 
  //  (eventually, CalTable should permit variable-shape cols)
  casacore::Int maxNumChan(1);
  for (casacore::Int iSpw=0; iSpw<nSpw_; iSpw++) 
    if (par_[iSpw]!=NULL) 
      maxNumChan=std::max(maxNumChan,nChan_(iSpw));

  // Some default values
  casacore::Double dzero = 0;
  casacore::IPosition ip(2,1,maxNumChan);

  // CalDesc Sub-table records
  CalDescRecord* descRec;
  casacore::Vector<casacore::Int> calDescNum(nSpw_); calDescNum=-1;
  for (casacore::Int iSpw=0; iSpw<nSpw_; iSpw++) {

    // Write a CalDesc for each spw which has solutions
    // Note: CalDesc index != SpwId, in general

    if (nRow(iSpw)>0 && par_[iSpw]!=NULL) {

      // Access to existing CAL_DESC columns:
      CalDescColumns2 cd(*calTab_);

      // Check if this spw already in CAL_DESC 
      //      cout << "spwCol = " << cd.spwId().getColumn() << endl;

      casacore::Bool newCD(true);
      for (casacore::Int iCD=0;iCD<nDesc;iCD++) {

	casacore::IPosition iCDip(1,0);
	if ( iSpw==(cd.spwId()(iCD))(iCDip) ) {
	  // Don't need new CAL_DESC entry
	  newCD=false;
	  calDescNum(iSpw)=iCD;
	  break;
	}
      }

      if (newCD) {

	// Cal_desc fields
	casacore::Vector <casacore::Int> spwId(1,iSpw);
	casacore::Matrix <casacore::Double> chanFreq(ip, dzero); 
	casacore::Matrix <casacore::Double> chanWidth(ip, dzero);
	casacore::Array <casacore::String> polznType(ip, casacore::String(""));
	casacore::Cube <casacore::Int> chanRange(casacore::IPosition(3,2,1,maxNumChan), 0);
	casacore::Vector <casacore::Int> numChan(1,nChan_(iSpw));
	for (casacore::Int ichan=0; ichan<nChan_(iSpw); ichan++) {
	  chanRange(0,0,ichan)=startChan_(iSpw);
	  chanRange(1,0,ichan)=startChan_(iSpw) + nChan_(iSpw) -1;
	}
	
	// Fill the cal_desc record
	descRec = new CalDescRecord;
	descRec->defineNumSpw (1);
	descRec->defineNumChan (numChan);
	descRec->defineNumReceptors (2);
	descRec->defineNJones (2);
	descRec->defineSpwId (spwId);
	descRec->defineChanFreq (chanFreq);
	descRec->defineChanWidth (chanWidth);
	descRec->defineChanRange (chanRange);
	descRec->definePolznType (polznType);
	descRec->defineJonesType ("full");
	descRec->defineMSName (casacore::Path(msname).baseName());
	//	descRec->defineMSName ("");
	
	// Write the cal_desc record

	calTab_->putRowDesc (nDesc, *descRec);
	delete descRec;
	
	// This spw will have this calDesc index in main table
	calDescNum(iSpw) = nDesc;
	nDesc++;
      }

    }
    
  }


  // Now write MAIN table in column-wise fashion
  
  // Starting row in this slot

  for (casacore::Int iSpw=0; iSpw<nSpw_; iSpw++) {

    // Write table for spws which have solutions
    if (par_[iSpw]!=NULL) {

      // Create references to cal data for this spw
      casacore::Vector<casacore::Bool>    thisSolOK;        thisSolOK.reference(*(solutionOK_[iSpw]));
      casacore::Vector<casacore::Double>  thisMJDTimeStamp; thisMJDTimeStamp.reference(*(MJDTimeStamp_[iSpw]));
      casacore::Vector<casacore::Double>  thisMJDStart;     thisMJDStart.reference(*(MJDStart_[iSpw]));
      casacore::Vector<casacore::Double>  thisMJDStop;      thisMJDStop.reference(*(MJDStop_[iSpw]));
      casacore::Vector<casacore::Int>     thisFieldId;      thisFieldId.reference(*(fieldId_[iSpw]));
      casacore::Vector<casacore::String>  thisFieldName;    thisFieldName.reference(*(fieldName_[iSpw]));
      casacore::Vector<casacore::String>  thisSourceName;   thisSourceName.reference(*(sourceName_[iSpw]));
      casacore::Vector<casacore::Float>   thisFit;          thisFit.reference(*(fit_[iSpw]));
      casacore::Vector<casacore::Float>   thisFitwt;        thisFitwt.reference(*(fitwt_[iSpw]));
      //      casacore::Array<casacore::Complex>  thisAntGain;      thisAntGain.reference(*(par_[iSpw]));
      casacore::Array<T>  thisAntGain;      thisAntGain.reference(*(par_[iSpw]));
      //      casacore::Matrix<casacore::Bool>    thisISolutionOK;  thisISolutionOK.reference(*(iSolutionOK_[iSpw]));
      casacore::Matrix<casacore::Float>   thisIFit;         thisIFit.reference(*(iFit_[iSpw]));
      casacore::Matrix<casacore::Float>   thisIFitwt;       thisIFitwt.reference(*(iFitwt_[iSpw]));

      casacore::Int thisnRow=nRow(iSpw);
      
      // Only if there are rows to add to table
      if (thisnRow > 0) {

	// These are constant columns (with boring values, currently)
	casacore::Vector<casacore::Double> timeEP(thisnRow,0.0);
	casacore::Vector<casacore::Int> feed1(thisnRow,0);
	casacore::Vector<casacore::Int> arrayId(thisnRow,0);
	casacore::Vector<casacore::Int> obsId(thisnRow,0);
	casacore::Vector<casacore::Int> scanNum(thisnRow,0);
	casacore::Vector<casacore::Int> procId(thisnRow,0);
	casacore::Vector<casacore::Int> stateId(thisnRow,0);
	casacore::Vector<casacore::Int> phaseId(thisnRow,0);
	casacore::Vector<casacore::Int> pulsarBin(thisnRow,0);
	casacore::Vector<casacore::Int> pulsarGateId(thisnRow,0);
	casacore::Vector<casacore::Int> freqGroup(thisnRow,0);
	casacore::Vector<casacore::Int> calHistId(thisnRow,0);
	
	// This is constant
	casacore::Vector<casacore::Int> calDescId(thisnRow,calDescNum(iSpw));
	
	// These are constant per slot
	//   (these cols should be incremental)
	casacore::Vector<casacore::Double> time(thisnRow,0.0);
	casacore::Vector<casacore::Double> interval(thisnRow,0.0);
	casacore::Vector<casacore::Int>    fieldId(thisnRow,0);
	casacore::Vector<casacore::String> fieldName(thisnRow,"");
	casacore::Vector<casacore::String> sourceName(thisnRow,"");
	casacore::Vector<casacore::Bool>   totalSolOk(thisnRow,false);
	casacore::Vector<casacore::Float>  totalFit(thisnRow,0.0);
	casacore::Vector<casacore::Float>  totalFitWt(thisnRow,0.0);
	
	// These vary
	casacore::Vector<casacore::Int>    antenna1(thisnRow,0);
	casacore::Cube<T>  gain(casacore::IPosition(3,nPar(),maxNumChan,thisnRow),T(0.0));
	casacore::Cube<casacore::Bool>     solOk(nPar(),maxNumChan,thisnRow,false);
	casacore::Cube<casacore::Float>    fit(1,maxNumChan,thisnRow,0.0);
	casacore::Cube<casacore::Float>    fitWt(1,maxNumChan,thisnRow,0.0);
	casacore::Cube<casacore::Bool>     flag(nPar(),maxNumChan,thisnRow,false);
	casacore::Cube<casacore::Float>    snr(nPar(),maxNumChan,thisnRow,false);

	casacore::IPosition out(3,0,0,0);   // par, chan, row
	casacore::IPosition in(4,0,0,0,0);  // par, chan, ant, slot
	casacore::Int thisRow(0);
	for (casacore::Int islot = 0; islot < nTime_(iSpw); islot++) {
	  in(3)=islot;
	  if (thisSolOK(islot)) {
	    
	    // Fill slot-constant cols:
	    casacore::Slice thisSlice(thisRow,nElem_);
	    time(thisSlice)=thisMJDTimeStamp(islot);
	    casacore::Double dt=(thisMJDStop(islot) - thisMJDStart(islot));
	    if (dt<0.0) dt=1.0;
	    interval(thisSlice)=dt;
	    fieldId(thisSlice)=thisFieldId(islot);
	    fieldName(thisSlice)=thisFieldName(islot);
	    sourceName(thisSlice)=thisSourceName(islot);
	    totalSolOk(thisSlice)=thisSolOK(islot);
	    totalFit(thisSlice)=thisFit(islot);
	    totalFitWt(thisSlice)=thisFitwt(islot);
	    
	    // Loop over the number of antennas
	    for (casacore::Int iant = 0; iant < nElem_; iant++) {
	      out(2)=thisRow;
	      in(2)=iant;
	      // Antenna index
	      antenna1(thisRow)=iant;
	      
	      gain.xyPlane(thisRow)(casacore::IPosition(2,0,0),casacore::IPosition(2,nPar()-1,nChan_(iSpw)-1))=
		thisAntGain(casacore::IPosition(4,0,0,iant,islot),
			    casacore::IPosition(4,nPar()-1,nChan_(iSpw)-1,iant,islot)).nonDegenerate(2);

	      // Per-chan fit pars
	      for (casacore::Int ichan=0; ichan<nChan_(iSpw); ichan++) {
		// Gain stats  (slot constant, per spw?)
		//solOk(0,ichan,thisRow) = thisISolutionOK(iant,islot);
		fit(0,ichan,thisRow) = thisIFit(iant,islot);
		fitWt(0,ichan,thisRow) = thisIFitwt(iant,islot);


		for (casacore::Int ipar=0; ipar<nPar(); ++ipar) {
		  solOk(ipar,ichan,thisRow) = parOK(iSpw)(casacore::IPosition(4,ipar,ichan,iant,islot));
		  flag(ipar,ichan,thisRow) = !solOk(ipar,ichan,thisRow);
		  snr(ipar,ichan,thisRow) = parSNR(iSpw)(casacore::IPosition(4,ipar,ichan,iant,islot));
		} // ipar
	      } // ichan
	      
	      // next time round is next row
	      thisRow++;
	    }; // iant

	    
	  }; // thisSolOK(islot)
	}; // islot
	
	// Now push everything to the disk table
	calTab_->addRowMain(thisnRow);
	//	SolvableVisJonesMCol svjmcol(*tab);
	if (svjmcol_) {delete svjmcol_; svjmcol_=NULL;}
	svjmcol_ = new SolvableCalSetMCol<T>(*calTab_);

	casacore::RefRows refRows(nMain,nMain+thisnRow-1);
	svjmcol_->time().putColumnCells(refRows,time);
	svjmcol_->timeEP().putColumnCells(refRows,timeEP);
	svjmcol_->interval().putColumnCells(refRows,interval);
	svjmcol_->antenna1().putColumnCells(refRows,antenna1);
	svjmcol_->feed1().putColumnCells(refRows,feed1);
	svjmcol_->fieldId().putColumnCells(refRows,fieldId);
	svjmcol_->arrayId().putColumnCells(refRows,arrayId);
	svjmcol_->obsId().putColumnCells(refRows,obsId);
	svjmcol_->scanNo().putColumnCells(refRows,scanNum);
	svjmcol_->processorId().putColumnCells(refRows,procId);
	svjmcol_->stateId().putColumnCells(refRows,stateId);
	svjmcol_->phaseId().putColumnCells(refRows,phaseId);
	svjmcol_->pulsarBin().putColumnCells(refRows,pulsarBin);
	svjmcol_->pulsarGateId().putColumnCells(refRows,pulsarGateId);
	svjmcol_->freqGrp().putColumnCells(refRows,freqGroup);
	svjmcol_->fieldName().putColumnCells(refRows,fieldName);
	svjmcol_->sourceName().putColumnCells(refRows,sourceName);
	svjmcol_->gain().putColumnCells(refRows,gain);
	svjmcol_->totalSolnOk().putColumnCells(refRows,totalSolOk);
	svjmcol_->totalFit().putColumnCells(refRows,totalFit);
	svjmcol_->totalFitWgt().putColumnCells(refRows,totalFitWt);
	svjmcol_->solnOk().putColumnCells(refRows,solOk);
	svjmcol_->fit().putColumnCells(refRows,fit);
	svjmcol_->fitWgt().putColumnCells(refRows,fitWt);
	svjmcol_->calDescId().putColumnCells(refRows,calDescId);
	svjmcol_->calHistoryId().putColumnCells(refRows,calHistId);
	svjmcol_->flag().putColumnCells(refRows,flag);
	svjmcol_->snr().putColumnCells(refRows,snr);
	
	nMain = calTab_->nRowMain();
	
      } // thisnRow > 0
    } // par_[iSpw]!=NULL
  } // iSpw

  delete calTab_;

};
/*
template<class T> void CalSet<T>::store (const casacore::String& file, 
					 const casacore::String& type,
					 const casacore::String& msname,
					 const casacore::Bool& append)
{
  // Write the solutions to an output calibration table
  // casacore::Input:
  //    file           casacore::String        Cal table name
  //    append         casacore::Bool          Append if true, else overwrite
  //

  // total rows to be written per Spw
  casacore::Vector<casacore::Int> nRow(nSpw_,0);
  for (casacore::Int iSpw=0;iSpw<nSpw_;iSpw++) 
    if (solutionOK_[iSpw])
      nRow(iSpw)=nElem_*ntrue(*(solutionOK_[iSpw]));

  // Escape if nothing to write
  if (sum(nRow)==0)
    throw(casacore::AipsError("No valid calibration solutions; no table written."));

  // Initialization:
  // No. of rows in cal_main, cal_desc and cal_history
  casacore::Int nMain = 0; 
  casacore::Int nDesc = 0;
  //casacore::Int nHist = 0;
  
  // Calibration table
  SolvableVisJonesTable *tab;
  
  // Open the output file if it already exists and is being appended to.
  if (append && casacore::Table::isWritable (file)) {
    tab  = new SolvableVisJonesTable (file, casacore::Table::Update);
    nMain = tab->nRowMain();
    nDesc = tab->nRowDesc();
    //nHist = tab->nRowHistory();
  } else {
    // Create a new calibration table
    casacore::Table::TableOption access = casacore::Table::New;
    tab = new SolvableVisJonesTable (file, type, access);
  };
  
  // Write every spw w/ max number of channels 
  //  (eventually, CalTable should permit variable-shape cols)
  casacore::Int maxNumChan(1);
  for (casacore::Int iSpw=0; iSpw<nSpw_; iSpw++) 
    if (par_[iSpw]!=NULL) 
      maxNumChan=max(maxNumChan,nChan_(iSpw));

  // Some default values
  casacore::Double dzero = 0;
  casacore::IPosition ip(2,1,maxNumChan);

  // CalDesc Sub-table records
  CalDescRecord* descRec;
  casacore::Vector<casacore::Int> calDescNum(nSpw_); calDescNum=-1;
  for (casacore::Int iSpw=0; iSpw<nSpw_; iSpw++) {

    // Write a CalDesc for each spw which has solutions
    // Note: CalDesc index != SpwId, in general

    if (nRow(iSpw)>0 && par_[iSpw]!=NULL) {

      // Access to existing CAL_DESC columns:
      CalDescColumns cd(*tab);

      // Check if this spw already in CAL_DESC 
      //      cout << "spwCol = " << cd.spwId().getColumn() << endl;

      casacore::Bool newCD(true);
      for (casacore::Int iCD=0;iCD<nDesc;iCD++) {

	casacore::IPosition iCDip(1,0);
	if ( iSpw==(cd.spwId()(iCD))(iCDip) ) {
	  // Don't need new CAL_DESC entry
	  newCD=false;
	  calDescNum(iSpw)=iCD;
	  break;
	}
      }

      if (newCD) {

	// Cal_desc fields
	casacore::Vector <casacore::Int> spwId(1,iSpw);
	casacore::Matrix <casacore::Double> chanFreq(ip, dzero); 
	casacore::Matrix <casacore::Double> chanWidth(ip, dzero);
	casacore::Array <casacore::String> polznType(ip, "");
	casacore::Cube <casacore::Int> chanRange(casacore::IPosition(3,2,1,maxNumChan), 0);
	casacore::Vector <casacore::Int> numChan(1,nChan_(iSpw));
	for (casacore::Int ichan=0; ichan<nChan_(iSpw); ichan++) {
	  chanRange(0,0,ichan)=startChan_(iSpw);
	  chanRange(1,0,ichan)=startChan_(iSpw) + nChan_(iSpw) -1;
	}
	
	// Fill the cal_desc record
	descRec = new CalDescRecord;
	descRec->defineNumSpw (1);
	descRec->defineNumChan (numChan);
	descRec->defineNumReceptors (2);
	descRec->defineNJones (2);
	descRec->defineSpwId (spwId);
	descRec->defineChanFreq (chanFreq);
	descRec->defineChanWidth (chanWidth);
	descRec->defineChanRange (chanRange);
	descRec->definePolznType (polznType);
	descRec->defineJonesType ("full");
	descRec->defineMSName (casacore::Path(msname).baseName());
	
	// Write the cal_desc record

	tab->putRowDesc (nDesc, *descRec);
	delete descRec;
	
	// This spw will have this calDesc index in main table
	calDescNum(iSpw) = nDesc;
	nDesc++;
      }

    }
    
  }


  // Now write MAIN table in column-wise fashion
  
  // Starting row in this slot

  for (casacore::Int iSpw=0; iSpw<nSpw_; iSpw++) {

    // Write table for spws which have solutions
    if (par_[iSpw]!=NULL) {

      // Create references to cal data for this spw
      casacore::Vector<casacore::Bool>    thisSolOK;        thisSolOK.reference(*(solutionOK_[iSpw]));
      casacore::Vector<casacore::Double>  thisMJDTimeStamp; thisMJDTimeStamp.reference(*(MJDTimeStamp_[iSpw]));
      casacore::Vector<casacore::Double>  thisMJDStart;     thisMJDStart.reference(*(MJDStart_[iSpw]));
      casacore::Vector<casacore::Double>  thisMJDStop;      thisMJDStop.reference(*(MJDStop_[iSpw]));
      casacore::Vector<casacore::Int>     thisFieldId;      thisFieldId.reference(*(fieldId_[iSpw]));
      casacore::Vector<casacore::String>  thisFieldName;    thisFieldName.reference(*(fieldName_[iSpw]));
      casacore::Vector<casacore::String>  thisSourceName;   thisSourceName.reference(*(sourceName_[iSpw]));
      casacore::Vector<casacore::Float>   thisFit;          thisFit.reference(*(fit_[iSpw]));
      casacore::Vector<casacore::Float>   thisFitwt;        thisFitwt.reference(*(fitwt_[iSpw]));
      casacore::Array<casacore::Complex>  thisAntGain;      thisAntGain.reference(*(par_[iSpw]));
      //      casacore::Matrix<casacore::Bool>    thisISolutionOK;  thisISolutionOK.reference(*(iSolutionOK_[iSpw]));
      casacore::Matrix<casacore::Float>   thisIFit;         thisIFit.reference(*(iFit_[iSpw]));
      casacore::Matrix<casacore::Float>   thisIFitwt;       thisIFitwt.reference(*(iFitwt_[iSpw]));

      casacore::Int thisnRow=nRow(iSpw);
      
      // Only if there are rows to add to table
      if (thisnRow > 0) {

	// These are constant columns (with boring values, currently)
	casacore::Vector<casacore::Double> timeEP(thisnRow,0.0);
	casacore::Vector<casacore::Int> feed1(thisnRow,0);
	casacore::Vector<casacore::Int> arrayId(thisnRow,0);
	casacore::Vector<casacore::Int> obsId(thisnRow,0);
	casacore::Vector<casacore::Int> scanNum(thisnRow,0);
	casacore::Vector<casacore::Int> procId(thisnRow,0);
	casacore::Vector<casacore::Int> stateId(thisnRow,0);
	casacore::Vector<casacore::Int> phaseId(thisnRow,0);
	casacore::Vector<casacore::Int> pulsarBin(thisnRow,0);
	casacore::Vector<casacore::Int> pulsarGateId(thisnRow,0);
	casacore::Vector<casacore::Int> freqGroup(thisnRow,0);
	casacore::Vector<casacore::Int> calHistId(thisnRow,0);
	
	// This is constant
	casacore::Vector<casacore::Int> calDescId(thisnRow,calDescNum(iSpw));
	
	// These are constant per slot
	//   (these cols should be incremental)
	casacore::Vector<casacore::Double> time(thisnRow,0.0);
	casacore::Vector<casacore::Double> interval(thisnRow,0.0);
	casacore::Vector<casacore::Int>    fieldId(thisnRow,0);
	casacore::Vector<casacore::String> fieldName(thisnRow,"");
	casacore::Vector<casacore::String> sourceName(thisnRow,"");
	casacore::Vector<casacore::Bool>   totalSolOk(thisnRow,false);
	casacore::Vector<casacore::Float>  totalFit(thisnRow,0.0);
	casacore::Vector<casacore::Float>  totalFitWt(thisnRow,0.0);
	
	// These vary
	casacore::Vector<casacore::Int>    antenna1(thisnRow,0);
	casacore::Cube<casacore::Complex>  gain(casacore::IPosition(3,nPar(),maxNumChan,thisnRow),casacore::Complex(0.0,0.0));
	casacore::Cube<casacore::Bool>     solOk(nPar(),maxNumChan,thisnRow,false);
	casacore::Cube<casacore::Float>    fit(1,maxNumChan,thisnRow,0.0);
	casacore::Cube<casacore::Float>    fitWt(1,maxNumChan,thisnRow,0.0);
	casacore::Cube<casacore::Bool>     flag(nPar(),maxNumChan,thisnRow,false);
	casacore::Cube<casacore::Float>    snr(nPar(),maxNumChan,thisnRow,false);

	casacore::IPosition out(3,0,0,0);   // par, chan, row
	casacore::IPosition in(4,0,0,0,0);  // par, chan, ant, slot
	casacore::Int thisRow(0);
	for (casacore::Int islot = 0; islot < nTime_(iSpw); islot++) {
	  in(3)=islot;
	  if (thisSolOK(islot)) {
	    
	    // Fill slot-constant cols:
	    casacore::Slice thisSlice(thisRow,nElem_);
	    time(thisSlice)=thisMJDTimeStamp(islot);
	    casacore::Double dt=(thisMJDStop(islot) - thisMJDStart(islot));
	    if (dt<0.0) dt=1.0;
	    interval(thisSlice)=dt;
	    fieldId(thisSlice)=thisFieldId(islot);
	    fieldName(thisSlice)=thisFieldName(islot);
	    sourceName(thisSlice)=thisSourceName(islot);
	    totalSolOk(thisSlice)=thisSolOK(islot);
	    totalFit(thisSlice)=thisFit(islot);
	    totalFitWt(thisSlice)=thisFitwt(islot);
	    
	    // Loop over the number of antennas
	    for (casacore::Int iant = 0; iant < nElem_; iant++) {
	      out(2)=thisRow;
	      in(2)=iant;
	      // Antenna index
	      antenna1(thisRow)=iant;
	      

	      gain.xyPlane(thisRow)(casacore::IPosition(2,0,0),casacore::IPosition(2,nPar()-1,nChan_(iSpw)-1))=
		thisAntGain(casacore::IPosition(4,0,0,iant,islot),casacore::IPosition(4,nPar()-1,nChan_(iSpw)-1,iant,islot)).nonDegenerate(2);

	      
	      // Per-chan fit pars
	      for (casacore::Int ichan=0; ichan<nChan_(iSpw); ichan++) {
		// Gain stats  (slot constant, per spw?)
		//solOk(0,ichan,thisRow) = thisISolutionOK(iant,islot);
		fit(0,ichan,thisRow) = thisIFit(iant,islot);
		fitWt(0,ichan,thisRow) = thisIFitwt(iant,islot);


		for (casacore::Int ipar=0; ipar<nPar(); ++ipar) {
		  solOk(ipar,ichan,thisRow) = parOK(iSpw)(casacore::IPosition(4,ipar,ichan,iant,islot));
		  flag(ipar,ichan,thisRow) = !solOk(ipar,ichan,thisRow);
		  snr(ipar,ichan,thisRow) = parSNR(iSpw)(casacore::IPosition(4,ipar,ichan,iant,islot));
		}
	      }
	      
	      // next time round is next row
	      thisRow++;
	    };
	    
	  };
	};
	
	// Now push everything to the disk table
	tab->addRowMain(thisnRow);
	SolvableVisJonesMCol svjmcol(*tab);
	
	casacore::RefRows refRows(nMain,nMain+thisnRow-1);
	svjmcol.time().putColumnCells(refRows,time);
	svjmcol.timeEP().putColumnCells(refRows,timeEP);
	svjmcol.interval().putColumnCells(refRows,interval);
	svjmcol.antenna1().putColumnCells(refRows,antenna1);
	svjmcol.feed1().putColumnCells(refRows,feed1);
	svjmcol.fieldId().putColumnCells(refRows,fieldId);
	svjmcol.arrayId().putColumnCells(refRows,arrayId);
	svjmcol.obsId().putColumnCells(refRows,obsId);
	svjmcol.scanNo().putColumnCells(refRows,scanNum);
	svjmcol.processorId().putColumnCells(refRows,procId);
	svjmcol.stateId().putColumnCells(refRows,stateId);
	svjmcol.phaseId().putColumnCells(refRows,phaseId);
	svjmcol.pulsarBin().putColumnCells(refRows,pulsarBin);
	svjmcol.pulsarGateId().putColumnCells(refRows,pulsarGateId);
	svjmcol.freqGrp().putColumnCells(refRows,freqGroup);
	svjmcol.fieldName().putColumnCells(refRows,fieldName);
	svjmcol.sourceName().putColumnCells(refRows,sourceName);
	svjmcol.gain().putColumnCells(refRows,gain);
	svjmcol.totalSolnOk().putColumnCells(refRows,totalSolOk);
	svjmcol.totalFit().putColumnCells(refRows,totalFit);
	svjmcol.totalFitWgt().putColumnCells(refRows,totalFitWt);
	svjmcol.solnOk().putColumnCells(refRows,solOk);
	svjmcol.fit().putColumnCells(refRows,fit);
	svjmcol.fitWgt().putColumnCells(refRows,fitWt);
	svjmcol.calDescId().putColumnCells(refRows,calDescId);
	svjmcol.calHistoryId().putColumnCells(refRows,calHistId);
	svjmcol.flag().putColumnCells(refRows,flag);
	svjmcol.snr().putColumnCells(refRows,snr);
	
	
	nMain = tab->nRowMain();
	
      }
    }
  }

  delete tab;

};
*/
} //# NAMESPACE CASA - END

