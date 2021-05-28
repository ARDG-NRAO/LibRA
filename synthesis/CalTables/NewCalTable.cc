//# NewCalTable.cc: Implementation of NewCalTable.h
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
//# $Id$
//----------------------------------------------------------------------------

#include <synthesis/CalTables/NewCalTable.h>
#include <synthesis/CalTables/CTColumns.h>
#include <synthesis/CalTables/CTMainColumns.h>
#include <ms/MeasurementSets/MeasurementSet.h>
#include <ms/MSOper/MSMetaData.h>
#include <tables/Tables/ScalarColumn.h>
#include <tables/Tables/ScaColDesc.h>
#include <tables/Tables/SetupNewTab.h>
#include <tables/Tables/TableCopy.h>
#include <tables/Tables/TableRow.h>
#include <tables/TaQL/TableParse.h>
#include <tables/Tables/TableInfo.h>
#include <measures/Measures/MEpoch.h>
#include <casa/Arrays.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Logging/LogIO.h>
#include <synthesis/CalTables/CTEnums.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

NewCalTable::NewCalTable() :
  Table()
{
  // Form CTDesc from parameters
  String parTypeStr("Complex");

  CTDesc nctd(parTypeStr,"unknown","unknown","unknown");

  // Form underlying generic Table according to the CTDesc
  SetupNewTable calMainTab("nullNewCalTable.tempMemCalTable",nctd.calMainDesc(),Table::New);
  Table tab(calMainTab, Table::Memory, 0, false); 
  *this = tab;
  
  // Set the table info record
  this->setTableInfo();
};

//----------------------------------------------------------------------------

NewCalTable::~NewCalTable()
{
// Default desctructor
};

//----------------------------------------------------------------------------
NewCalTable::NewCalTable (const String& tableName, CTDesc& ctableDesc,
			  Table::TableOption access, Table::TableType ttype):
              Table()
{
// Construct from a cal table name, descriptor and access option.
// Used for creating new tables.
// Input:
//    tableName        const String&         Cal table name
//    ctableDesc       const CTDesc&   Cal table descriptor
//    access           Table::TableOption    Access mode
//    ttype            Table::TableType      Memory or Plain
//

  if (access == Table::New || access == Table::NewNoReplace ||
      access == Table::Scratch) {

    // Form underlying generic Table according to the supplied desc
    SetupNewTable calMainTab(tableName,ctableDesc.calMainDesc(),access);
    Table tab(calMainTab, ttype, 0, false); 
    *this = tab;

    // Set the table info record
    this->setTableInfo();

    // Form (empty) subtables
    this->createSubTables();
  }
  else
    throw(AipsError("Creating NewCalTable from scratch must use access=Table::New or TableNewNoReplace or Table::Scratch"));
};


//----------------------------------------------------------------------------
NewCalTable::NewCalTable (SetupNewTable& newtab, uInt nrow, Bool initialize):
              Table(newtab, nrow, initialize)
{
// Construct from a cal table name, descriptor and access option.
// Used for creating new tables.
// Input:
//    newtab           SetupNewtable&   
//    nrow             uInt                  n rows
//    initialize       Bool                   
//
};

//----------------------------------------------------------------------------
// Create an empty NewCalTable conveniently
NewCalTable::NewCalTable(String tableName,VisCalEnum::VCParType parType,
			 String typeName,String msName,Bool doSingleChan) : 
  Table()
{
  // Form CTDesc from parameters
  String parTypeStr = ((parType==VisCalEnum::COMPLEX) ? "Complex" : "Float");

  CTDesc nctd(parTypeStr,msName,typeName,"unknown");

  // Form underlying generic Table according to the CTDesc
  SetupNewTable calMainTab(tableName+".tempMemCalTable",nctd.calMainDesc(),Table::New);
  Table tab(calMainTab, Table::Memory, 0, false); 
  *this = tab;
  
  // Set the table info record
  this->setTableInfo();

  // Add (empty) subtables
  this->createSubTables();

  // Copy subtables from the supplied MS
  this->setMetaInfo(msName);

  // Reset Spw channelization, if nec.
  //  (very basic, uses chan n/2 freq)
  if (doSingleChan)
    this->makeSpwSingleChan();

}

//----------------------------------------------------------------------------
NewCalTable::NewCalTable (const String& tableName, Table::TableOption access, 
			  Table::TableType ttype): Table(tableName,access)
{
// Construct from an existing cal table, and access option.
// 
// Input:
//    tableName        const String&         Cal table name
//    access           Table::TableOption    Access mode
//    ttype            Table::TableType      Memory or Plain
//
  //cerr<<"ctor: from existing cal table with name, option"<<endl;

  if (ttype==Table::Memory) 
    *this = this->copyToMemoryTable(tableName+".tempMemCalTable");
 
  if (!this->tableDesc().isColumn(NCT::fieldName(NCT::OBSERVATION_ID)) ||
      !this->keywordSet().isDefined("OBSERVATION")) 
    addPhoneyObs();

  // Attach subtable accessors
  attachSubTables();

};

NewCalTable::NewCalTable (const String& tableName,
    const casacore::TableLock& lockOptions, Table::TableOption access,
    Table::TableType ttype): Table(tableName, lockOptions, access)
{
  // Construct from an existing cal table, with lock, access, and type options.
  if (ttype==Table::Memory) 
    *this = this->copyToMemoryTable(tableName+".tempMemCalTable");
  if (!this->tableDesc().isColumn(NCT::fieldName(NCT::OBSERVATION_ID)) ||
      !this->keywordSet().isDefined("OBSERVATION")) {
    addPhoneyObs();
  }
  attachSubTables();
}
//----------------------------------------------------------------------------

// Factory method that has Back Compat option
NewCalTable NewCalTable::createCT(const String& tableName, 
				  Table::TableOption access, 
				  Table::TableType ttype, 
				  Bool doBackCompat) {
  // Handle back compat
  if (doBackCompat)
    NewCalTable::CTBackCompat(tableName);

  // Ordinary ctor
  return NewCalTable(tableName,access,ttype);

}
// Factory method that has Back Compat option
NewCalTable* NewCalTable::createCTptr(const String& tableName, 
				     Table::TableOption access, 
				     Table::TableType ttype, 
				     Bool doBackCompat) {
  // Handle back compat
  if (doBackCompat)
    NewCalTable::CTBackCompat(tableName);

  // Ordinary ctor
  return new NewCalTable(tableName,access,ttype);

}

//----------------------------------------------------------------------------
NewCalTable::NewCalTable(String tableName, String CorF,
			 Int nObs, Int nScanPerObs, Int nTimePerScan,
			 Int nAnt, Int nSpw, Vector<Int> nChan, 
			 Int nFld, 
			 Double rTime, Double tint,
			 Bool disk, Bool verbose) :
  Table()
{

  String caltype("");
  if (CorF=="Complex")
    caltype="T";
  else if (CorF=="Float")
    caltype="K";
  else
    throw(AipsError("CorF must be 'Complex' or 'Float'"));

  CTDesc nctd(CorF,"none",caltype,"circ");

  // Form underlying generic Table according to the supplied desc
  SetupNewTable calMainTab(tableName+".tempMemCalTable",nctd.calMainDesc(),Table::New);
  Table tab(calMainTab, Table::Memory, 0, false); 
  *this = tab;
  
  // Set the table info record
  this->setTableInfo();

  // Add (empty) subtables
  this->createSubTables();

  // Fill it generically
  this->fillGenericContents(nObs,nScanPerObs,nTimePerScan,
			    nAnt,nSpw,nChan,
			    nFld,rTime,tint,verbose);

  if (disk) {
    // Write it out to disk
    if (verbose) cout << "Writing out to disk: "+tableName << endl;
    this->writeToDisk(tableName);
  }

}

// Create an empty NewCalTable from a SimpleSimVi2Parameters object
NewCalTable::NewCalTable(String tableName, String CorF, String caltype,
			 const vi::SimpleSimVi2Parameters& ssp,
			 Bool disk,Bool verbose) :
  Table()
{

  if (CorF!="Complex" &&
      CorF!="Float")
    throw(AipsError("CorF must be 'Complex' or 'Float'"));

  CTDesc nctd(CorF,"none",caltype,"circ");

  // Form underlying generic Table according to the supplied desc
  SetupNewTable calMainTab(tableName+".tempMemCalTable",nctd.calMainDesc(),Table::New);
  Table tab(calMainTab, Table::Memory, 0, false); 
  *this = tab;
  
  // Set the table info record
  this->setTableInfo();

  // Add (empty) subtables
  this->createSubTables();

  // Fill generic subtables, according to SimpleSimVi2Parameters
  fillGenericObs(ssp);
  fillGenericField(ssp);
  fillGenericAntenna(ssp);
  fillGenericSpw(ssp);

  if (disk) {
    // Write it out to disk
    if (verbose) cout << "Writing out to disk: "+tableName << endl;
    this->writeToDisk(tableName);
  }

}



//----------------------------------------------------------------------------

NewCalTable::NewCalTable (const Table& table): Table(table)
{
// Construct from an existing table object
  //cerr<<"constructed from an existing newcaltable as table"<<endl;
  attachSubTables();
};

//----------------------------------------------------------------------------

NewCalTable::NewCalTable (const NewCalTable& other): Table(other)
{
// Copy constructor
// Input:
//    other            const NewCalTable&       Existing NewCalTable object
//
   //cerr<<"copy constructor ...."<<endl;
   copyMemCalSubtables(other);
   attachSubTables();
};

//----------------------------------------------------------------------------

NewCalTable& NewCalTable::operator= (const NewCalTable& other)
{
// Assignment operator
// Input:
//    other            const CalTable&       RHS CalTable object
//
  //cerr<<"assignment operator..."<<endl;
  if (this != &other) {
    clearSubtables();
    Table::operator=(other);
    if (!conformant(this->tableDesc()))
        throw (AipsError("NewCalTable( const NewCalTable&) - "
                         "table is not a valid caltable"));
    attachSubTables();
  }
  return *this;
};


//----------------------------------------------------------------------------

// Handle backward compatibility
Bool NewCalTable::CTBackCompat(const String& caltable) {
  
  Bool doBC(false);

  // Detect backward compatibility issues 
  Table tab(caltable,Table::Old);

  // pre-v41 had no OBSERVATION/OBSERVATION_ID
  doBC=(!tab.tableDesc().isColumn(NCT::fieldName(NCT::OBSERVATION_ID)) ||
	!tab.keywordSet().isDefined("OBSERVATION"));

  if (doBC)
    NewCalTable backcompat(caltable,Table::Update,Table::Plain);

  return true;

}


//----------------------------------------------------------------------------
void NewCalTable::setTableInfo() {
      this->tableInfo().setType(TableInfo::type(TableInfo::ME_CALIBRATION));
      this->tableInfo().setSubType(this->tableDesc().getType());
}

//----------------------------------------------------------------------------
void NewCalTable::createSubTables() {
      
  // Names
  String  calObsName=this->tableName()+"/OBSERVATION";
  String  calAntennaName=this->tableName()+"/ANTENNA";
  String  calFieldName=this->tableName()+"/FIELD";
  String  calSpectralWindowName=this->tableName()+"/SPECTRAL_WINDOW";
  String  calHistoryName=this->tableName()+"/HISTORY";
  
  Table::TableOption access(Table::TableOption(this->tableOption()));
  Table::TableType type(this->tableType());

  // Assign them to keywords
  SetupNewTable obstab(calObsName,CTObservation::requiredTableDesc(),access); 
  this->rwKeywordSet().defineTable("OBSERVATION", Table(obstab,type));
  observation_p = CTObservation(this->keywordSet().asTable("OBSERVATION"));

  SetupNewTable antennatab(calAntennaName,CTAntenna::requiredTableDesc(),access); 
  this->rwKeywordSet().defineTable("ANTENNA", Table(antennatab,type));
  antenna_p = CTAntenna(this->keywordSet().asTable("ANTENNA"));

  SetupNewTable fieldtab(calFieldName,CTField::requiredTableDesc(),access); 
  this->rwKeywordSet().defineTable("FIELD", Table(fieldtab,type));
  field_p = CTField(this->keywordSet().asTable("FIELD"));

  SetupNewTable spwtab(calSpectralWindowName,CTSpectralWindow::requiredTableDesc(),access); 
  this->rwKeywordSet().defineTable("SPECTRAL_WINDOW", Table(spwtab,type));
  spectralWindow_p = CTSpectralWindow(this->keywordSet().asTable("SPECTRAL_WINDOW"));
  
  SetupNewTable histab(calHistoryName,CTHistory::requiredTableDesc(),access); 
  this->rwKeywordSet().defineTable("HISTORY", Table(histab,type));
  history_p = CTHistory(this->keywordSet().asTable("HISTORY"));
};


//----------------------------------------------------------------------------
void NewCalTable::attachSubTables()
{

  if (this->keywordSet().isDefined("OBSERVATION"))
    observation_p = CTObservation(this->keywordSet().asTable("OBSERVATION"));

  if (this->keywordSet().isDefined("ANTENNA"))
    antenna_p = CTAntenna(this->keywordSet().asTable("ANTENNA"));

  if (this->keywordSet().isDefined("FIELD"))
    field_p = CTField(this->keywordSet().asTable("FIELD"));

  if (this->keywordSet().isDefined("SPECTRAL_WINDOW"))
    spectralWindow_p = CTSpectralWindow(this->keywordSet().asTable("SPECTRAL_WINDOW"));

  if (this->keywordSet().isDefined("HISTORY"))
    history_p = CTHistory(this->keywordSet().asTable("HISTORY"));

}

//----------------------------------------------------------------------------
void NewCalTable::clearSubtables()
{
   observation_p=CTObservation();
   antenna_p=CTAntenna();
   field_p=CTField();
   spectralWindow_p=CTSpectralWindow();
   history_p = CTHistory();
}
//----------------------------------------------------------------------------
void NewCalTable::copyMemCalSubtables(const NewCalTable & other)
{
   copyMemCalSubtable(other.observation_p, observation_p);
   copyMemCalSubtable(other.antenna_p, antenna_p);
   copyMemCalSubtable(other.field_p, field_p);
   copyMemCalSubtable(other.spectralWindow_p, spectralWindow_p);
   copyMemCalSubtable(other.history_p, history_p);
}
//----------------------------------------------------------------------------
void NewCalTable::copyMemCalSubtable(const Table & otherCalsubtable, Table & calSubtable )
{
  //if (! otherCalsubtable.isNull () && otherCalsubtable.tableType() == Table::Memory){
  if (! otherCalsubtable.isNull ()){
        calSubtable = otherCalsubtable;
    }
}

//----------------------------------------------------------------------------
Bool NewCalTable::isComplex() {
  return (this->keywordSet().asString("ParType")=="Complex");
}

//----------------------------------------------------------------------------
String NewCalTable::polBasis() {
  return this->keywordSet().asString("PolBasis");
}

//----------------------------------------------------------------------------
String NewCalTable::CASAvers() {
  if (this->keywordSet().fieldNumber("CASA_Version")==-1)
    // Handle non-existent keyword
    return String("Unknown");
  else
    return this->keywordSet().asString("CASA_Version");
}

//----------------------------------------------------------------------------
Record NewCalTable::getRowMain (const Int& jrow)
{
// Get a row from cal_main
// Input:
//    jrow             const Int&            Row number
// Output:
//    getRowMain       Record                Row record
//
  ROTableRow trow (*this);
  trow.get (jrow);
  return trow.record();
};

//----------------------------------------------------------------------------
void NewCalTable::putRowMain (const Int& jrow, CTMainRecord& tableRec)
{
// Get a row from cal_main
// Input:
//    jrow             const Int&            Row number
//    tableRec         const CalMainRecord&  Table record 
//
// Add rows as required
  Int nMaxRow = this->nrow();
  Int nAdd = jrow - nMaxRow + 1;
  if (nAdd > 0) {
    this->addRow (nAdd);
  };

// Write the record
  TableRow trow (*this);
  TableRecord trec = tableRec.record();
  trow.putMatchingFields (jrow, trec);
};

//----------------------------------------------------------------------------
void NewCalTable::setMetaInfo(const String& msName)
{
// set Meta data info:
// put parent MS name and (for now) make copy of Antenna, Field, and SpW 
// sub-tables.
  MeasurementSet inms(msName);
  const MSObservation msobstab = inms.observation();
  const MSAntenna msantab = inms.antenna();
  const MSField msfldtab = inms.field();
  const MSSpectralWindow msspwtab = inms.spectralWindow();

  // deep copy subtables from an MS to NCT 
  // by TableCopy::copyRows
  //copy obs table
  CTObservation calobstab(this->observation());
  TableCopy::copyRows(calobstab,msobstab);
  //copy antenna table
  CTAntenna calantab(this->antenna());
  TableCopy::copyRows(calantab,msantab);

  //copy field table
  CTField calfldtab(this->field());
  TableCopy::copyRows(calfldtab,msfldtab);

  // copy nominal Eph object directions into CTField
  handleEphObj(msfldtab,msName);

  //copy spectralWindow table
  CTSpectralWindow calspwtab(this->spectralWindow());
  TableCopy::copyRows(calspwtab,msspwtab);

  // Record only the basename of the MS 
  this->rwKeywordSet().define(RecordFieldId("MSName"),Path(msName).baseName());
}

void NewCalTable::handleEphObj(const MSField& msfldtab,const String& msName) {

  // If EPHEMERIS_ID column exists in the MS FIELD subtable, we
  //   may have some Eph Objects to get directions for.
  const String ephColName = MSField::columnName(MSField::EPHEMERIS_ID);
  if (msfldtab.actualTableDesc().isColumn(ephColName)) {
    MSFieldColumns msfcol(msfldtab);
    Vector<Int> ephid=msfcol.ephemerisId().getColumn();

    // Do nothing if no eph objects
    if (max(ephid)<0) return;
    
    const MeasurementSet ms(msName);
    MSMetaData msmd(&ms,-1.0f);
    CTFieldColumns ctfcol(field_p);
    for (uInt i=0;i<ephid.nelements();++i) {
      if (ephid(i)>-1) {
	Matrix<Double> dir(2,1,0.0);
	// Calculate nominal position from the MS
	dir=msmd.getReferenceDirection(i).getAngle().getValue();
	// Write the nominal position into the caltable dir columns
	ctfcol.referenceDir().put(i,dir);
	ctfcol.phaseDir().put(i,dir);
	ctfcol.delayDir().put(i,dir);
      }
    }
  }
}


//----------------------------------------------------------------------------
Bool NewCalTable::conformant(const TableDesc& tabDesc)
{
// Check if input table description is confomrant with
// the new caltable format (or should I named this "validate" ...as 
// in MS case...)
  Bool eqDType=false;
  CTDesc calTD = CTDesc(false);  // opt out of OBS_ID, because we aren't insisting on it yet
  TableDesc requiredCalTD = calTD.calMainDesc();
  Bool isCalTableDesc = tabDesc.columnDescSet().isSuperset(requiredCalTD.columnDescSet(), eqDType);
  if (!isCalTableDesc) {
    cerr<<"NewCalTable::conformant: tabDesc is not superset of requiredCalMain"<<endl;
  };
  Vector<String> colNames(requiredCalTD.columnNames());
  Vector<String> incolNames(tabDesc.columnNames());
  uInt ncols = colNames.nelements();
  for (uInt j=0; j < ncols; j++) {
  }
  Bool check = true;
  for (uInt i=0; i < ncols; i++) {
    TableRecord keySet = tabDesc[colNames(i)].keywordSet();
    TableRecord reqKeySet = requiredCalTD[colNames(i)].keywordSet();
    if (reqKeySet.isDefined("QuantumUnits")) {
      check = keySet.isDefined("QuantumUnits");
      if (!check) {
        cerr<<"NewCalTable::conformant: column:"<<colNames(i)<<" does not have a unit"<<endl;
      }
      else {
        check =  allEQ(keySet.asArrayString("QuantumUnits"), reqKeySet.asArrayString("QuantumUnits"));
        if (!check) {
          cerr<<"NewCalTable::conformant column:"<<colNames(i)
               <<" has an invalid QuantumUnits:"<<keySet.asArrayString("QuantumUnits")<<endl;
        }
      }
    }
  }
  return isCalTableDesc && check;
};

//----------------------------------------------------------------------------
void NewCalTable::writeToDisk(const String& outTableName)
{
  Block<String> sortcols(4);
  sortcols[0]="SPECTRAL_WINDOW_ID"; 
  sortcols[1]="TIME"; 
  sortcols[2]="ANTENNA1"; 
  sortcols[3]="ANTENNA2"; 
  Table sorted = this->sort(sortcols,Sort::Ascending,Sort::HeapSort);
  sorted.deepCopy(outTableName,Table::New);
};

Complex NewCalTable::NCTtestvalueC(Int iant,Int ispw,Double ich,Double time,Double refTime,Double tint) {

  Double a=1.0 + Double(iant)/10.0 + Double(ispw)/100.0 + ich/10000.0;
  Double dt=(time-refTime)/tint;
  Double p=dt+ich/100.0; 
  return Complex(Float(a*cos(p)),Float(a*sin(p)));

}

Float NewCalTable::NCTtestvalueF(Int iant,Int ispw,Double ich,Double time,Double refTime,Double tint) {

  Double dt=(time-refTime)/tint;
  Double a=dt + Double(iant)/10.0 + Double(ispw)/100.0 + ich/10000.0;
  return Float(a);

}


//----------------------------------------------------------------------------
void NewCalTable::fillGenericContents(Int nObs, Int nScanPerObs,Int nTimePerScan,
				      Int nAnt, Int nSpw, Vector<Int> nChan, 
				      Int nFld, 
				      Double rTime, Double tint,
				      Bool verbose) {

  // Cope with unspecified time info
  if (rTime==0.0) rTime=4832568000.0;
  if (tint==0.0) tint=60.0;

  if (verbose) {
    cout << nFld << " "
	 << nAnt << " "
	 << nSpw << " "
	 << nChan << " "
	 << nObs << " "
	 << nScanPerObs << " "
	 << nTimePerScan << " "
	 << endl;
    cout.precision(15);
  }
  
  // fill subtables
  fillGenericObs(nObs);
  fillGenericField(nFld);
  fillGenericAntenna(nAnt);
  fillGenericSpw(nSpw,nChan);

  // The per-solution antenna indices
  Vector<Int> antlist(nAnt);
  indgen(antlist);
  Int refant=0;  // for ANTENNA2

  // T-like
  Int nPar(1);

  Double thistime(rTime-tint);  // first sample will be at rTime

  CTMainColumns ncmc(*this);

  Int thisscan(0);
  for (Int iobs=0;iobs<nObs;++iobs) {
    if (verbose) cout << "Obs=" << iobs << endl;
    Int thisfield(-1);
    for (Int iscan=0;iscan<nScanPerObs;++iscan) {
      thisscan+=1;    //  unique scans
      thisfield+=1;   // each scan is a new field
      thisfield=thisfield%nFld;   // never more than nFld-1
      if (verbose) cout << " Scan=" << thisscan << "  Field=" <<thisfield << endl; 
      for (Int itime=0;itime<nTimePerScan;++itime) {
	thistime+=tint; // every tint
	if (verbose) cout << "  Time="<< thistime << endl;

	for (Int ispw=0;ispw<nSpw;++ispw) {

	  if (verbose) cout << "   Spw=" << ispw << endl;
	  
	  // add rows
	  Int nAddRows=nAnt;
	  RefRows rows(this->nrow(),this->nrow()+nAddRows-1,1); 
	  this->addRow(nAddRows);

	  if (verbose) cout << "    Adding " << nAnt << " rows; total=" << this->nrow() << endl;
	  
	  // fill columns in new rows
	  ncmc.time().putColumnCells(rows,Vector<Double>(nAddRows,thistime));
	  ncmc.interval().putColumnCells(rows,Vector<Double>(nAddRows,tint));
	  ncmc.fieldId().putColumnCells(rows,Vector<Int>(nAddRows,thisfield));
	  ncmc.spwId().putColumnCells(rows,Vector<Int>(nAddRows,ispw));
	  ncmc.antenna1().putColumnCells(rows,antlist);
	  ncmc.antenna2().putColumnCells(rows,Vector<Int>(nAddRows,refant));
	  ncmc.obsId().putColumnCells(rows,Vector<Int>(nAddRows,iobs));
	  ncmc.scanNo().putColumnCells(rows,Vector<Int>(nAddRows,thisscan));


	  if (isComplex()) {
	    Cube<Complex> par(nPar,nChan(ispw),nAddRows);
	    for (Int iant=0;iant<nAnt;++iant) {
	      for (Int ich=0;ich<nChan(ispw);++ich) {
		par.xyPlane(iant).column(ich)=NCTtestvalueC(iant,ispw,ich,thistime,rTime,tint);
	      }
	    }
	    ncmc.cparam().putColumnCells(rows,par);
	  }
	  else {
	    Cube<Float> par(nPar,nChan(ispw),nAddRows);
	    for (Int iant=0;iant<nAnt;++iant) {
	      for (Int ich=0;ich<nChan(ispw);++ich) {
		par.xyPlane(iant).column(ich)=NCTtestvalueF(iant,ispw,ich,thistime,rTime,tint);
	      }
	    }
	    ncmc.fparam().putColumnCells(rows,par);
	  }

	  Cube<Float> parerr(nPar,nChan(ispw),nAddRows);
	  parerr=0.001;
	  ncmc.paramerr().putColumnCells(rows,parerr);
	  Cube<Float> snr(nPar,nChan(ispw),nAddRows);
	  snr=999.0;
	  ncmc.snr().putColumnCells(rows,snr);
	  Cube<Float> wt(nPar,nChan(ispw),nAddRows);
	  wt=1.0;
	  ncmc.weight().putColumnCells(rows,wt);
	  Cube<Bool> flag(nPar,nChan(ispw),nAddRows);
	  flag=false;
	  ncmc.flag().putColumnCells(rows,flag);
	}
      }
    }
  }
  
}

//----------------------------------------------------------------------------
void NewCalTable::fillGenericObs(Int nObs) { 

  this->observation().addRow(nObs);
  MSObservationColumns oc(this->observation());
  Vector<Double> tr(2,0.0);
  tr(1)=7609161600.0;  // the year 2100 (forever-ish)
  for (Int iobs=0;iobs<nObs;++iobs) {
    oc.timeRange().put(iobs,tr);
    oc.observer().put(iobs,String("unknown"));
    oc.project().put(iobs,String("unknown"));
    oc.telescopeName().put(iobs,String("unknown"));
    oc.flagRow().put(iobs,false);
  }
}

//----------------------------------------------------------------------------
void NewCalTable::fillGenericField(Int nFld) { 

  this->field().addRow(nFld);
  MSFieldColumns fc(this->field());
  for (Int ifld=0;ifld<nFld;++ifld) {
    fc.name().put(ifld,("Field_"+String::toString(ifld)));
    fc.flagRow().put(ifld,false);
  }
}



//----------------------------------------------------------------------------
void NewCalTable::fillGenericAntenna(Int nAnt) {

  this->antenna().addRow(nAnt);
  MSAntennaColumns ac(this->antenna());
  for (Int iant=0;iant<nAnt;++iant) {
    ac.name().put(iant,("Antenna_"+String::toString(iant)));
    ac.station().put(iant,("Station_"+String::toString(iant)));
    ac.type().put(iant,"GROUND-BASED");
    ac.mount().put(iant,"ALT-AZ");
    ac.dishDiameter().put(iant,25.0);
    ac.offset().put(iant,Vector<Double>(3,0.0));
    ac.position().put(iant,Vector<Double>(3,0.0));
    ac.flagRow().put(iant,false);
  }

}


//----------------------------------------------------------------------------
void NewCalTable::fillGenericSpw(Int nSpw,Vector<Int>& nChan) {

  this->spectralWindow().addRow(nSpw);
  MSSpWindowColumns sc(this->spectralWindow());
  for (Int ispw=0;ispw<nSpw;++ispw) {
    Double refFreq(60.0e9+Double(ispw)*1.0e9);  // Every GHz
    Double width(1.0e6);  // 1 MHz channels
    Vector<Double> chanfreq(nChan(ispw),refFreq);
    for (Int ich=0;ich<nChan(ispw);++ich) chanfreq(ich)+=(Double(ich)+0.5)*width;

    sc.name().put(ispw,("SPW_"+String::toString(ispw)));
    sc.refFrequency().put(ispw,refFreq);
    sc.numChan().put(ispw,nChan(ispw));
    sc.chanFreq().put(ispw,chanfreq);
    
    Vector<Double> res(nChan(ispw),width);

    sc.chanWidth().put(ispw,res);
    sc.effectiveBW().put(ispw,res);
    sc.resolution().put(ispw,res);
    sc.totalBandwidth().put(ispw,sum(res));
    sc.flagRow().put(ispw,false);
  }
}

//----------------------------------------------------------------------------
void NewCalTable::fillGenericObs(const vi::SimpleSimVi2Parameters& /*ssp*/) { 
  this->fillGenericObs(1); // Just one, for now
}

//----------------------------------------------------------------------------
void NewCalTable::fillGenericField(const vi::SimpleSimVi2Parameters& ssp) { 
  this->fillGenericField(ssp.nField_);
}

//----------------------------------------------------------------------------
void NewCalTable::fillGenericAntenna(const vi::SimpleSimVi2Parameters& ssp) { 
  this->fillGenericAntenna(ssp.nAnt_);
}


//----------------------------------------------------------------------------
void NewCalTable::fillGenericSpw(const vi::SimpleSimVi2Parameters& ssp) { 

  // Gather info from the SimpleSimVi2Parameters
  const Int& nSpw(ssp.nSpw_);
  const Vector<Int>& nChan(ssp.nChan_);

  this->spectralWindow().addRow(nSpw);
  MSSpWindowColumns sc(this->spectralWindow());
  for (Int ispw=0;ispw<nSpw;++ispw) {
    const Double& refFreq(ssp.refFreq_(ispw));
    const Double& width(ssp.df_(ispw));
    Vector<Double> chanfreq(nChan(ispw),refFreq);
    for (Int ich=0;ich<nChan(ispw);++ich) chanfreq(ich)+=(Double(ich)+0.5)*width;

    sc.name().put(ispw,("SPW_"+String::toString(ispw)));
    sc.refFrequency().put(ispw,refFreq);
    sc.numChan().put(ispw,nChan(ispw));
    sc.chanFreq().put(ispw,chanfreq);
    
    Vector<Double> res(nChan(ispw),width);

    sc.chanWidth().put(ispw,res);
    sc.effectiveBW().put(ispw,res);
    sc.resolution().put(ispw,res);
    sc.totalBandwidth().put(ispw,sum(res));
    sc.flagRow().put(ispw,false);
  }
}



//----------------------------------------------------------------------------
void NewCalTable::fillAntBasedMainRows(uInt nrows, 
				       Double time,Double interval,
				       Int fieldId,uInt spwId,Int scanNo,
				       const Vector<Int>& ant1list, Int refant,
				       const Cube<Complex>& cparam,
				       const Cube<Bool>& flag,
				       const Cube<Float>& paramErr,
				       const Cube<Float>& snr) {

  // Forward to obsId-capable method with obsID=0
  this->fillAntBasedMainRows(nrows,
			     time,interval,
			     fieldId,spwId,0,scanNo,
			     ant1list,refant,
			     cparam,flag,paramErr,snr);

}

void NewCalTable::fillAntBasedMainRows(uInt nrows, 
				       Double time,Double interval,
				       Int fieldId,uInt spwId,Int obsId, Int scanNo,
				       const Vector<Int>& ant1list, Int refant,
				       const Cube<Complex>& cparam,
				       const Cube<Bool>& flag,
				       const Cube<Float>& paramErr,
				       const Cube<Float>& snr) {
  
  // Verify that we are Complex
  TableRecord keywords=this->keywordSet();
  if (!keywords.isDefined("ParType") ||
      keywords.asString("ParType")!="Complex")
    throw(AipsError("NewCalTable::fillAntBasedMainRows: NewCalTable's ParType is not Complex"));

  // First, verify internal consistency
  IPosition csh=cparam.shape();

  // Data must conform to specified nrows
  AlwaysAssert( (cparam.nplane()==nrows), AipsError);
  AlwaysAssert( (cparam.shape()==flag.shape()), AipsError);

  // Stat arrays much match cparam shape
  if (paramErr.nelements()>0) 
    AlwaysAssert( (paramErr.shape()==cparam.shape()), AipsError);
  if (snr.nelements()>0)
    AlwaysAssert( (snr.shape()==cparam.shape()), AipsError);

  // Specified indices must be rational
  AlwaysAssert( (spwId<this->spectralWindow().nrow()), AipsError);
  AlwaysAssert( (fieldId<Int(this->field().nrow())), AipsError);

  // Handle ant1list
  Vector<Int> ant1;
  if (ant1list.nelements()>0) {
    AlwaysAssert( (min(ant1list)>0), AipsError); // must be definite
    AlwaysAssert( (max(ant1list)<Int(this->antenna().nrow())), AipsError);
    ant1.reference(ant1list);
  }
  else {
    // Generate the ant1 list
    ant1.resize(nrows);
    indgen(ant1);
  }

  // All seems well, so add rows
  RefRows rows(this->nrow(),this->nrow()+nrows-1,1);
  this->addRow(nrows);
  
  CTMainColumns mc(*this);
  
  // Meta-info (these are uniform for all rows)
  mc.time().putColumnCells(rows,Vector<Double>(nrows,time));
  mc.fieldId().putColumnCells(rows,Vector<Int>(nrows,fieldId));
  mc.spwId().putColumnCells(rows,Vector<Int>(nrows,spwId));
  mc.obsId().putColumnCells(rows,Vector<Int>(nrows,obsId));
  mc.scanNo().putColumnCells(rows,Vector<Int>(nrows,scanNo));
  mc.interval().putColumnCells(rows,Vector<Double>(nrows,interval));
  
  // Antenna
  mc.antenna1().putColumnCells(rows,ant1);
  mc.antenna2().putColumnCells(rows,Vector<Int>(nrows,refant));  // uniform

  // Complex CPARAM column
  mc.cparam().putColumnCells(rows,cparam);

  // Fill stats
  mc.flag().putColumnCells(rows,flag);
  if (paramErr.nelements()>0)
    mc.paramerr().putColumnCells(rows,paramErr);
  else
    // zeros, w/ correct shape
    mc.paramerr().putColumnCells(rows,Cube<Float>(csh,0.0));
  if (snr.nelements()>0) 
    mc.snr().putColumnCells(rows,snr);
  else
    // ones, w/ correct shape
    mc.paramerr().putColumnCells(rows,Cube<Float>(csh,1.0));

}

void NewCalTable::setSpwFreqs(Int spw, const Vector<Double>& freq, 
			      const Vector<Double>& chanwidth) { 


  MSSpWindowColumns spwcol(this->spectralWindow());

  AlwaysAssert( (spw<Int(spwcol.nrow())), AipsError );
  
  uInt nchan=freq.nelements();
  IPosition sh(1,nchan);

  spwcol.numChan().put(spw,nchan);
  spwcol.chanFreq().setShape(spw,sh);
  spwcol.chanFreq().put(spw,freq);
  
  if (chanwidth.nelements()==0) {

    if (nchan>1) {
      Double width=freq(1)-freq(0);
      Vector<Double> widthV(nchan,width);
      
      spwcol.chanWidth().setShape(spw,sh);
      spwcol.chanWidth().put(spw,widthV);
      spwcol.resolution().setShape(spw,sh);
      spwcol.resolution().put(spw,widthV);
      spwcol.effectiveBW().setShape(spw,sh);
      spwcol.effectiveBW().put(spw,widthV);
  
      Double totalBW=nchan*width;
      spwcol.totalBandwidth().put(spw,totalBW);
    }
    else
      throw(AipsError("NewCalTable::setSpwFreqs: Problem resetting SPECTRAL_WINDOW info"));


  }
  else {
    AlwaysAssert( (chanwidth.nelements()==nchan), AipsError);
    
    spwcol.chanWidth().setShape(spw,sh);
    spwcol.chanWidth().put(spw,chanwidth);
    spwcol.resolution().setShape(spw,sh);
    spwcol.resolution().put(spw,chanwidth);
    spwcol.effectiveBW().setShape(spw,sh);
    spwcol.effectiveBW().put(spw,chanwidth);

    spwcol.totalBandwidth().put(spw,sum(chanwidth));

  }


}

// Set FLAG_ROW in SPECTRAL_WINDOW subtable for spws absent in MAIN
//  (this enables append to permit revision when appropriate)
void NewCalTable::flagAbsentSpws() {

  CTColumns ctcol(*this);

  // Extract unique spws in MAIN
  Vector<Int> spwids;
  ctcol.spwId().getColumn(spwids);
  Int nspw=genSort(spwids,Sort::Ascending,(Sort::QuickSort | Sort::NoDuplicates));
  spwids.resize(nspw,true);

  // Revise SPW FLAG_ROW
  Vector<Bool> spwfr;
  ctcol.spectralWindow().flagRow().getColumn(spwfr);
  spwfr.set(true);
  for (Int ispw=0;ispw<nspw;++ispw) {
    uInt thisspw=spwids(ispw);
    if (thisspw<spwfr.nelements()) 
      spwfr(thisspw)=false;  // unflagged
    else
      throw(AipsError("NewCalTable::flagAbsentSpws: Main table contains spwids not in SpW subtable"));
  }

  // Put FLAG_ROW back
  ctcol.spectralWindow().flagRow().putColumn(spwfr);

}
// Merge SPW subtable rows from another NewCalTable
void NewCalTable::mergeSpwMetaInfo(const NewCalTable& other) {

  // Access this' SPW subtable
  CTSpWindowColumns spwcols(this->spectralWindow());

  // Access other's SPW subtable
  ROCTSpWindowColumns ospwcols(other.spectralWindow());

  // Just make sure that they have the same number of rows:
  //  (should be guaranteed by having been derived from the same MS)
  if (spwcols.nrow()!=ospwcols.nrow())
    throw(AipsError("NewCalTable::mergeSpwMetaInfo: Attempt to merge unrelated SPW subtables."));

  // Loop over other's spws
  uInt onspw=ospwcols.nrow();
  for (uInt ispw=0;ispw<onspw;++ispw) {

    // Only if other's ispw is not flagged...
    if (!ospwcols.flagRow()(ispw)) {

      try {

	// Certain meta info must already be equivalent
	AlwaysAssert( spwcols.name()(ispw)==ospwcols.name()(ispw), AipsError);
	AlwaysAssert( spwcols.measFreqRef()(ispw)==ospwcols.measFreqRef()(ispw), AipsError);
	AlwaysAssert( spwcols.netSideband()(ispw)==ospwcols.netSideband()(ispw), AipsError);
	AlwaysAssert( spwcols.ifConvChain()(ispw)==ospwcols.ifConvChain()(ispw), AipsError);
	AlwaysAssert( spwcols.freqGroup()(ispw)==ospwcols.freqGroup()(ispw), AipsError);
	AlwaysAssert( spwcols.freqGroupName()(ispw)==ospwcols.freqGroupName()(ispw), AipsError);
      }
      catch ( AipsError x ) {
	throw(AipsError("Spw meta information incongruent; cannot merge it for append."));
      }

      // ...and this' ispw _is_ flagged
      if (spwcols.flagRow()(ispw)) {

	// Copy from other to this:
	Int nchan=ospwcols.numChan()(ispw);
	IPosition sh(1,nchan);

	spwcols.numChan().put(ispw,nchan);
	spwcols.chanFreq().setShape(ispw,sh);
	spwcols.chanFreq().put(ispw,ospwcols.chanFreq()(ispw));
	spwcols.chanWidth().setShape(ispw,sh);
	spwcols.chanWidth().put(ispw,ospwcols.chanWidth()(ispw));
	spwcols.resolution().setShape(ispw,sh);
	spwcols.resolution().put(ispw,ospwcols.resolution()(ispw));
	spwcols.effectiveBW().setShape(ispw,sh);
	spwcols.effectiveBW().put(ispw,ospwcols.effectiveBW()(ispw));

	spwcols.refFrequency().put(ispw,ospwcols.refFrequency()(ispw));
	spwcols.totalBandwidth().put(ispw,ospwcols.totalBandwidth()(ispw));

	// this' row now unflagged
	spwcols.flagRow().put(ispw,false);

      }
      else {

	try {
	
	  // Assert equivalence in all meaningful rows
	  AlwaysAssert( spwcols.numChan()(ispw)==ospwcols.numChan()(ispw), AipsError);
	  AlwaysAssert( allEQ(spwcols.chanFreq()(ispw),ospwcols.chanFreq()(ispw)), AipsError);
	  AlwaysAssert( allEQ(spwcols.chanWidth()(ispw),ospwcols.chanWidth()(ispw)), AipsError);
	  AlwaysAssert( allEQ(spwcols.resolution()(ispw),ospwcols.resolution()(ispw)), AipsError);
	  AlwaysAssert( allEQ(spwcols.effectiveBW()(ispw),ospwcols.effectiveBW()(ispw)), AipsError);
	  AlwaysAssert( spwcols.refFrequency()(ispw)==ospwcols.refFrequency()(ispw), AipsError);
	  AlwaysAssert( spwcols.totalBandwidth()(ispw)==ospwcols.totalBandwidth()(ispw), AipsError);
	}
	catch ( AipsError err ) {
	  throw(AipsError("Error merging spw="+String::toString(ispw)+"'s meta info"));
	}
      }
    } 

  } // ispw


}




void NewCalTable::addHistoryMessage(String app, String message) {

  Int row=this->history().nrow();
  this->history().addRow(1);

  MSHistoryColumns hcol(this->history());

  // Add the current data
  Time date;
  MEpoch now(MVEpoch(date.modifiedJulianDay()),MEpoch::Ref(MEpoch::UTC));
  hcol.timeMeas().put(row,now);

  // The application
  hcol.application().put(row,app);

  // Write the message
  hcol.message().put(row,message);

  // Fill in some other columns with emptiness
  hcol.objectId().put(row,-1);
  hcol.observationId().put(row,-1);

}

void NewCalTable::makeSpwSingleChan() {

  MSSpWindowColumns spwcol(this->spectralWindow());

  // Reset each spw to a single channel
  for (uInt ispw=0;ispw<spwcol.nrow();++ispw) {

    Int nchan;
    spwcol.numChan().get(ispw,nchan);
    IPosition ip(1,1);
    if (nchan>1) {
      Vector<Double> midFreq;
      spwcol.chanFreq().get(ispw,midFreq);
      midFreq(0)=midFreq(nchan/2);
      midFreq.resize(1,true);
      Double totBW;
      spwcol.totalBandwidth().get(ispw,totBW);
      Vector<Double> totBWv(1,totBW);
      
      spwcol.numChan().put(ispw,1);

      spwcol.chanFreq().setShape(ispw,ip);
      spwcol.chanFreq().put(ispw,midFreq);

      spwcol.chanWidth().setShape(ispw,ip);
      spwcol.chanWidth().put(ispw,totBWv);
      spwcol.effectiveBW().setShape(ispw,ip);
      spwcol.effectiveBW().put(ispw,totBWv);
      spwcol.resolution().setShape(ispw,ip);
      spwcol.resolution().put(ispw,totBWv);

    }
  }
}

void NewCalTable::addPhoneyObs() {

  TableType ntype(this->tableType());

  ostringstream msg;
  msg << "Found pre-v4.1 caltable (" << this->tableName() << "); attempting to update..." << endl;

  // If absent, add OBSERVATION_ID column and OBSERVATION
  //  ONLY if caltable is writable (on disk) or a Memory table
  if ( (ntype==Table::Plain && this->isWritable()) ||
       (ntype==Table::Memory) ) {

    // Add phoney OBSERVATION_ID column and fill with zeros
    if (!this->tableDesc().isColumn(NCT::fieldName(NCT::OBSERVATION_ID))) {
      ScalarColumnDesc<Int> obscoldesc(NCT::fieldName (NCT::OBSERVATION_ID),ColumnDesc::Direct);
      this->addColumn(obscoldesc,false);
      CTMainColumns mc(*this);
      mc.obsId().fillColumn(0);
    }

    // Add dummy OBSERVATION subtable with 1 row
    if (!this->keywordSet().isDefined("OBSERVATION")) {
      String  calObsName=this->tableName()+"/OBSERVATION";
      Table::TableOption access(Table::NewNoReplace);
      SetupNewTable obstab(calObsName,CTObservation::requiredTableDesc(),access);
      this->rwKeywordSet().defineTable("OBSERVATION", Table(obstab,ntype));  // same type as parent table
      observation_p = CTObservation(this->keywordSet().asTable("OBSERVATION"));
      fillGenericObs(1);
    }

    msg << "SUCCEEDED: trivial OBSERVATION/OBSERVATION_ID have been added.";

    LogIO log;
    log << msg.str() << LogIO::WARN;

  }
  else {
    msg << "FAILED: caltable is not writable.";
    msg << " Please run cb.updatecaltable on this caltable, OR";
    msg << "  regenerate this caltable in v4.1 or later.";
    throw(AipsError(msg.str()));
  }

}



} //# NAMESPACE CASA - END
