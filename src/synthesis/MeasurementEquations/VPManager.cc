//# VPManager.cc: Implementation of VPManager.h
//# Copyright (C) 1996-2016
//# Associated Universities, Inc. Washington DC, USA.
//# Copyright by ESO (in the framework of the ALMA collaboration)
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or (at your option)
//# any later version.
//#
//# This program is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//# more details.
//#
//# You should have received a copy of the GNU General Public License along
//# with this program; if not, write to the Free Software Foundation, Inc.,
//# 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$


#include <casacore/casa/Containers/Record.h>
#include <casacore/casa/Exceptions/Error.h>
#include <iostream>
#include <casacore/tables/Tables/ScalarColumn.h>
#include <casacore/tables/Tables/ScaColDesc.h>
#include <casacore/tables/Tables/ScaRecordColDesc.h>
#include <casacore/tables/Tables/Table.h>
#include <casacore/tables/Tables/SetupNewTab.h>
#include <casacore/tables/TaQL/TableParse.h>
#include <casacore/tables/Tables/TableDesc.h>
#include <casacore/casa/Quanta/QuantumHolder.h>
#include <casacore/casa/Quanta/Quantum.h>
#include <casacore/measures/Measures/MDirection.h>
#include <casacore/measures/Measures/MeasureHolder.h>
#include <synthesis/MeasurementEquations/VPManager.h>
#include <synthesis/TransformMachines/PBMathInterface.h>
#include <synthesis/TransformMachines/PBMath.h>
#include <synthesis/TransformMachines/SynthesisError.h>
#include <synthesis/TransformMachines/ALMACalcIlluminationConvFunc.h>
#include <casacore/casa/Logging.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Logging/LogSink.h>
#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/OS/Directory.h>
#include <casacore/images/Images/PagedImage.h>
#include <unistd.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

  VPManager* VPManager::instance_p = 0;
  std::recursive_mutex VPManager::mutex_p; // to permit calls in same thread

  VPManager::VPManager(Bool verbose):
    vplist_p(),
    aR_p()
  {
    reset(verbose);
  }


  VPManager* VPManager::Instance(){
    if(instance_p==0){
      std::lock_guard<std::recursive_mutex> locker(mutex_p);
      if(instance_p==0){
	instance_p = new VPManager();
      }
    }
    return instance_p;
  }

  void VPManager::reset(Bool verbose)
  {

    std::lock_guard<std::recursive_mutex> locker(mutex_p);

    vplist_p = Record();
    vplistdefaults_p.clear();
    aR_p.init();

    LogIO os;
    os << LogOrigin("VPManager", "reset");

    String telName;
    for(Int pbtype = static_cast<Int>(PBMath::DEFAULT) + 1;
	pbtype < static_cast<Int>(PBMath::NONE); ++pbtype){
      PBMath::nameCommonPB(static_cast<PBMath::CommonPB>(pbtype), telName);
      if ( vplistdefaults_p.find(telName) != vplistdefaults_p.end( ) ) vplistdefaults_p[telName] = -1;
      else vplistdefaults_p.insert(std::pair<casacore::String, casacore::Int >(telName,-1));
    }

    // check for available AntennaResponses tables in the Observatories table
    Vector<String> obsName = MeasTable::Observatories();
    for(uInt i=0; i<obsName.size(); i++){

      String telName = obsName(i);

      String antRespPath;
      if(!MeasTable::AntennaResponsesPath(antRespPath, telName)) {
	// unknown observatory
	continue;
      }
      else{ // remember the corresponding telescopes as special vplist entries
	if(!aR_p.init(antRespPath)){
	  if(verbose){
	    os << LogIO::WARN
	       << "Invalid path defined in Observatories table for \"" << telName << "\":" << endl
	       << antRespPath << endl
	       << LogIO::POST;
	  }
	}
	else{
	  // init successful
	  Record rec;
	  rec.define("name", "REFERENCE");
	  rec.define("isVP", PBMathInterface::NONE);
	  rec.define("telescope", telName);
	  rec.define("antresppath", antRespPath);
	  
	  if(verbose){
	    os << LogIO::NORMAL
	       << "Will use " << telName << " antenna responses from table " 
	       << antRespPath << LogIO::POST;
	  }

      auto vpiter = vplistdefaults_p.find(telName);
	  if( vpiter != vplistdefaults_p.end( ) ){ // there is already a vp for this telName
	    Int ifield = vpiter->second;
	    if(ifield>=0){
	      Record rrec = vplist_p.rwSubRecord(ifield);
	      rrec.define("dopb", false);
	    }
	    vplistdefaults_p.erase(vpiter);	    
	  }
	  if ( vplistdefaults_p.find(telName) != vplistdefaults_p.end( ) ) vplistdefaults_p[telName] = vplist_p.nfields();
	  else vplistdefaults_p.insert(std::pair<casacore::String, casacore::Int >(telName,vplist_p.nfields())); 
	  rec.define("dopb", true);
	  
	  vplist_p.defineRecord(vplist_p.nfields(), rec);
	}
      }
    }	
    if(verbose){
      os << LogIO::NORMAL << "VPManager initialized." << LogIO::POST;
    }

  }


  Bool VPManager::saveastable(const String& tablename){
    
    std::lock_guard<std::recursive_mutex> locker(mutex_p);

    TableDesc td("vptable", "1", TableDesc::Scratch);
    td.addColumn(ScalarColumnDesc<String>("telescope"));
    td.addColumn(ScalarColumnDesc<Int>("antenna"));
    td.addColumn(ScalarRecordColumnDesc("pbdescription"));
    SetupNewTable aTab(tablename, td, Table::New);

    Table tb(aTab, Table::Plain, vplist_p.nfields());
    ScalarColumn<String> telcol(tb, "telescope");
    ScalarColumn<Int> antcol(tb, "antenna");
    ScalarColumn<TableRecord> pbcol(tb, "pbdescription");
    for (uInt k=0; k < vplist_p.nfields(); ++k){
      TableRecord antRec(vplist_p.asRecord(k));
      String tel=antRec.asString("telescope");
      telcol.put(k, tel);
      antcol.put(k,k);
      pbcol.put(k,antRec);       
    }

    // create subtable for the vplistdefaults
    TableDesc td2("vplistdefaultstable", "1", TableDesc::Scratch);
    td2.addColumn(ScalarColumnDesc<String>("tel_and_anttype"));
    td2.addColumn(ScalarColumnDesc<Int>("vplistnum"));
    SetupNewTable defaultsSetup(tablename+"/VPLIST_DEFAULTS", td2, Table::New);
    
    Table tb2(defaultsSetup, Table::Plain, vplistdefaults_p.size( ));
    ScalarColumn<String> telcol2(tb2, "tel_and_anttype");
    ScalarColumn<Int> listnumcol(tb2, "vplistnum");
    uInt keycount = 0;
    for ( auto iter = vplistdefaults_p.begin( ); iter != vplistdefaults_p.end( ); ++iter, ++keycount ){
      telcol2.put(keycount, iter->first);
      listnumcol.put(keycount, iter->second);
    }

    tb.rwKeywordSet().defineTable("VPLIST_DEFAULTS", tb2);

    tb2.flush();
    tb.flush();
    return true;
  }


  Bool VPManager::loadfromtable(const String& tablename){

    std::lock_guard<std::recursive_mutex> locker(mutex_p);

    LogIO os(LogOrigin("vpmanager", "loadfromtable"));

    Table tb(tablename);
    ScalarColumn<TableRecord> pbcol(tb, "pbdescription");

    Table tb2;

    if (tb.keywordSet().isDefined("VPLIST_DEFAULTS")) {
      tb2 = tb.keywordSet().asTable("VPLIST_DEFAULTS");
    }
    else{
      os << "Format error: table " << tablename 
	 << " does not contain a VPLIST_DEFAULTS subtable." << LogIO::POST;
      return false;
    }

    ScalarColumn<String> telcol2(tb2, "tel_and_anttype");
    ScalarColumn<Int> listnumcol(tb2, "vplistnum");

    Record tempvplist;
    std::map<String, Int> tempvplistdefaults;

    for (uInt k=0; k < tb.nrow(); k++){
      tempvplist.defineRecord(k, Record(pbcol(k)));
    }
    for (uInt k=0; k < tb2.nrow(); k++){
      Int vplistnum =  listnumcol(k);
      if((vplistnum < -1) || (vplistnum>=(Int)tempvplist.nfields())){
	os << "Error: invalid vplist number " << vplistnum 
	   << " in subtable VPLIST_DEFAULTS of table " 
	   << tablename << endl
	   << "Valid value range is -1 to " << (Int)tempvplist.nfields()-1 
	   << LogIO::POST;
	return false;
      }
      if ( tempvplistdefaults.find(telcol2(k)) != tempvplistdefaults.end( ) ) tempvplistdefaults[telcol2(k)] = vplistnum;
      else tempvplistdefaults.insert(std::pair<casacore::String, casacore::Int >(telcol2(k), vplistnum));
    }

    // overwrite existing information
    vplist_p = tempvplist;
    vplistdefaults_p = tempvplistdefaults;

    os << "Loaded " << tb.nrow() << " VP definitions and " << tb2.nrow() 
       << " VP default settings from table " << tablename << LogIO:: POST;

    return true;

  }


  Bool VPManager::summarizevps(const Bool verbose) {

    std::lock_guard<std::recursive_mutex> locker(mutex_p);

    LogIO os(LogOrigin("vpmanager", "summarizevps"));

    os << LogIO::NORMAL << "Voltage patterns internally defined in CASA (* = global default for this telescope):\n"
       << "  Telescope: Class"
       << LogIO::POST;
    String telName;
    String pbClassName;
    for(Int pbtype = static_cast<Int>(PBMath::DEFAULT) + 1;
        pbtype < static_cast<Int>(PBMath::NONE); ++pbtype){
      PBMath::nameCommonPB(static_cast<PBMath::CommonPB>(pbtype), telName);
      auto vpiter = vplistdefaults_p.find(telName);
      if ( vpiter != vplistdefaults_p.end( ) && vpiter->second == -1 ) {
	os << LogIO::NORMAL << " * ";
      }
      else{
	os << LogIO::NORMAL << "   ";
      }
      os << telName;
      try{
	PBMath(static_cast<PBMath::CommonPB>(pbtype)).namePBClass(pbClassName);
	os << ": " << pbClassName << LogIO::POST;
      }
      catch(AipsError){
	os <<  ": not available" << LogIO::POST;
      }
    }
    
    os << LogIO::NORMAL << "\nExternally defined voltage patterns (* = global default for this telescope):" << LogIO::POST;
    if (vplist_p.nfields() > 0) {
      os << "VP#     Tel        VP Type " << LogIO::POST;
      for (uInt i=0; i < vplist_p.nfields(); ++i){
	TableRecord antRec(vplist_p.asRecord(i));
	String telName = antRec.asString("telescope");
    auto vpiter = vplistdefaults_p.find(telName);
	if ( vpiter != vplistdefaults_p.end( ) && ((Int)i == vpiter->second) ) {
	  os << i << "   * ";
	}
	else{
	  os << i << "     ";
	}
        os << String(telName+ "           ").resize(11);
	os << antRec.asString("name");
	
	if(antRec.asString("name")=="REFERENCE"){
	  os << ": " << antRec.asString("antresppath");
	}
	else{
	  // antenna types
	  uInt counter=0;
	  os << " (used for antenna types ";
	  for ( auto iter = vplistdefaults_p.begin( ); iter != vplistdefaults_p.end( ); ++iter ) {
	    String aDesc = iter->first;
	    if(telName == telFromAntDesc(aDesc)
	       && ((Int)i == iter->second)
	       ){
	      if(counter>0){
		os << ", ";
	      }
	      if(antTypeFromAntDesc(aDesc).empty()){
		os << "any";
	      }
	      else{
		os << "\"" << antTypeFromAntDesc(aDesc) << "\"";
	      }
	      counter++;
	    }
	  }
	  os << ")";
	}

	os << LogIO::POST;
        if (verbose) {
	  ostringstream oss;
	  antRec.print(oss);
	  os << oss.str() << LogIO::POST;
        }
      }
    } else {
      os << "\tNone" << LogIO::POST;
    }

    return true;
  }

  Bool VPManager::setcannedpb(const String& tel, 
			      const String& other, 
			      const Bool dopb,
			      const String& commonpb,
			      const Bool dosquint, 
			      const Quantity& paincrement, 
			      const Bool usesymmetricbeam,
			      Record& rec){

    std::lock_guard<std::recursive_mutex> locker(mutex_p);

    rec = Record();
    rec.define("name", "COMMONPB");
    rec.define("isVP", PBMathInterface::COMMONPB);
    if (tel=="OTHER") {
      rec.define("telescope", other);
    } else {
      rec.define("telescope", tel);
    }
    rec.define("dopb", dopb);
    rec.define("commonpb", commonpb);
    rec.define("dosquint", dosquint);
    String error;
    Record tempholder;
    QuantumHolder(paincrement).toRecord(error, tempholder);
    rec.defineRecord("paincrement", tempholder);
    rec.define("usesymmetricbeam", usesymmetricbeam);

    if(dopb){
        auto key = rec.asString(rec.fieldNumber("telescope"));
        if ( vplistdefaults_p.find(key) != vplistdefaults_p.end( ) ) vplistdefaults_p[key] = vplist_p.nfields( );
        else vplistdefaults_p.insert(std::pair<casacore::String, casacore::Int >(key, vplist_p.nfields( )));
    } 

    vplist_p.defineRecord(vplist_p.nfields(), rec);

    return true;
  }

  Bool VPManager::setpbairy(const String& tel, 
			    const String& other, 
			    const Bool dopb, const Quantity& dishdiam, 
			    const Quantity& blockagediam, 
			    const Quantity& maxrad, 
			    const Quantity& reffreq, 
			    MDirection& squintdir, 
			    const Quantity& squintreffreq,const Bool dosquint, 
			    const Quantity& paincrement, 
			    const Bool usesymmetricbeam,
			    Record& rec){

    std::lock_guard<std::recursive_mutex> locker(mutex_p);

    rec=Record();
    rec.define("name", "AIRY");
    rec.define("isVP", PBMathInterface::AIRY);
    if(tel=="OTHER"){
      rec.define("telescope", other);
    }
    else{
      rec.define("telescope", tel);
    }
    rec.define("dopb", dopb);
    String error;
    {Record tempholder; 
      QuantumHolder(dishdiam).toRecord(error, tempholder);
      rec.defineRecord("dishdiam", tempholder);
    }
    {Record tempholder; 
      QuantumHolder(blockagediam).toRecord(error, tempholder);
      rec.defineRecord("blockagediam", tempholder);
    }
    { Record tempholder; 
      QuantumHolder(maxrad).toRecord(error, tempholder);
      rec.defineRecord("maxrad", tempholder);
    }
    {Record tempholder; 
      QuantumHolder(reffreq).toRecord(error, tempholder);
      rec.defineRecord("reffreq", tempholder);
    }
    rec.define("isthisvp", false);
    {Record tempholder; 
      MeasureHolder(squintdir).toRecord(error, tempholder);
      rec.defineRecord("squintdir", tempholder);
    }
    {Record tempholder; 
      QuantumHolder(squintreffreq).toRecord(error, tempholder);
      rec.defineRecord("squintreffreq", tempholder);
    }
    rec.define("dosquint", dosquint);
    {Record tempholder; 
      QuantumHolder(paincrement).toRecord(error, tempholder);
      rec.defineRecord("paincrement", tempholder);
    }
    rec.define("usesymmetricbeam", usesymmetricbeam);
    
    if(dopb){
        auto key = rec.asString(rec.fieldNumber("telescope"));
        if ( vplistdefaults_p.find(key) != vplistdefaults_p.end( ) ) vplistdefaults_p[key] = vplist_p.nfields( );
        else vplistdefaults_p.insert(std::pair<casacore::String, casacore::Int >(key, vplist_p.nfields( )));
    } 

    vplist_p.defineRecord(vplist_p.nfields(), rec); 

    return true;

  }

  Bool VPManager::setpbcospoly(const String& telescope,
			       const String& othertelescope,
			       const Bool dopb, const Vector<Double>& coeff,
			       const Vector<Double>& scale,
			       const Quantity& maxrad,
			       const Quantity& reffreq,
			       const String& isthispb,
			       MDirection& squintdir,
			       const Quantity& squintreffreq,
			       const Bool dosquint,
			       const Quantity& paincrement,
			       const Bool usesymmetricbeam,
			       Record& rec) {

    std::lock_guard<std::recursive_mutex> locker(mutex_p);

    rec=Record();
    rec.define("name", "COSPOLY");
    rec.define("isVP", PBMathInterface::COSPOLY);
    if(telescope=="OTHER"){
      rec.define("telescope", othertelescope);
    }
    else{
      rec.define("telescope", telescope);
    }
    rec.define("dopb", dopb);
    rec.define("coeff", coeff);
    rec.define("scale", scale);
    String error;
    { Record tempholder; 
      QuantumHolder(maxrad).toRecord(error, tempholder);
      rec.defineRecord("maxrad", tempholder);
    }
    {Record tempholder; 
      QuantumHolder(reffreq).toRecord(error, tempholder);
      rec.defineRecord("reffreq", tempholder);
    }
    if(isthispb=="PB" || isthispb=="pb"){
      rec.define("isthisvp", false);
    }
    else{
      rec.define("isthisvp", true);
    }
    {Record tempholder; 
      MeasureHolder(squintdir).toRecord(error, tempholder);
      rec.defineRecord("squintdir", tempholder);
    }
    {Record tempholder; 
      QuantumHolder(squintreffreq).toRecord(error, tempholder);
      rec.defineRecord("squintreffreq", tempholder);
    }
    rec.define("dosquint", dosquint);
    {Record tempholder; 
      QuantumHolder(paincrement).toRecord(error, tempholder);
      rec.defineRecord("paincrement", tempholder);
    }
    rec.define("usesymmetricbeam", usesymmetricbeam);

    if(dopb){
        auto key = rec.asString(rec.fieldNumber("telescope"));
        if ( vplistdefaults_p.find(key) != vplistdefaults_p.end( ) ) vplistdefaults_p[key] = vplist_p.nfields( );
        else vplistdefaults_p.insert(std::pair<casacore::String, casacore::Int >(key, vplist_p.nfields( )));
    } 

    vplist_p.defineRecord(vplist_p.nfields(), rec); 

    return true;

  }

  Bool VPManager::setpbgauss(const String& tel, const String& other, 
			     const Bool dopb,
			     const Quantity& halfwidth, const Quantity maxrad, 
			     const Quantity& reffreq, 
			     const String& isthispb, 
			     MDirection& squintdir, 
			     const Quantity& squintreffreq,
			     const Bool dosquint, 
			     const Quantity& paincrement, 
			     const Bool usesymmetricbeam,
			     Record& rec){

    std::lock_guard<std::recursive_mutex> locker(mutex_p);

    rec=Record();
    rec.define("name", "GAUSS");
    rec.define("isVP", PBMathInterface::GAUSS);
    if(tel=="OTHER"){
      rec.define("telescope", other);
    }
    else{
      rec.define("telescope", tel);
    }
    rec.define("dopb", dopb);
    Quantity hpw=halfwidth;
    if(isthispb=="PB" || isthispb=="pb"){
      hpw.setValue(hpw.getValue()/2.0);
    }
    String error;
    {Record tempholder; 
    QuantumHolder(hpw).toRecord(error, tempholder);
    rec.defineRecord("halfwidth", tempholder);
    }
    {Record tempholder; 
      QuantumHolder(maxrad).toRecord(error, tempholder);
      rec.defineRecord("maxrad", tempholder);
    }
    {Record tempholder; 
      QuantumHolder(reffreq).toRecord(error, tempholder);
      rec.defineRecord("reffreq", tempholder);
    }
    if(isthispb=="PB" || isthispb=="pb"){
      rec.define("isthisvp", false);
    }
    else{
      rec.define("isthisvp", true);
    }
    {Record tempholder; 
      MeasureHolder(squintdir).toRecord(error, tempholder);
      rec.defineRecord("squintdir", tempholder);
    }
    {Record tempholder; 
      QuantumHolder(squintreffreq).toRecord(error, tempholder);
      rec.defineRecord("squintreffreq", tempholder);
    }
    rec.define("dosquint", dosquint);
    {Record tempholder; 
      QuantumHolder(paincrement).toRecord(error, tempholder);
      rec.defineRecord("paincrement", tempholder);
    }
    rec.define("usesymmetricbeam", usesymmetricbeam);

    if(dopb){
        auto key = rec.asString(rec.fieldNumber("telescope"));
        if ( vplistdefaults_p.find(key) != vplistdefaults_p.end( ) ) vplistdefaults_p[key] = vplist_p.nfields( );
        else vplistdefaults_p.insert(std::pair<casacore::String, casacore::Int >(key, vplist_p.nfields( )));
    } 

    vplist_p.defineRecord(vplist_p.nfields(), rec); 

    return true;

  }

  Bool VPManager::setpbinvpoly(const String& telescope,
			       const String& othertelescope,
			       const Bool dopb, const Vector<Double>& coeff,
			       const Quantity& maxrad,
			       const Quantity& reffreq,
			       const String& isthispb,
			       MDirection& squintdir,
			       const Quantity& squintreffreq,
			       const Bool dosquint,
			       const Quantity& paincrement,
			       const Bool usesymmetricbeam,
			       Record& rec) {

    std::lock_guard<std::recursive_mutex> locker(mutex_p);

    rec=Record();
    rec.define("name", "IPOLY");
    rec.define("isVP", PBMathInterface::IPOLY);
    if(telescope=="OTHER"){
      rec.define("telescope", othertelescope);
    }
    else{
      rec.define("telescope", telescope);
    }
    rec.define("dopb", dopb);
    rec.define("coeff", coeff);
    String error;
    {Record tempholder; 
      QuantumHolder(maxrad).toRecord(error, tempholder);
      rec.defineRecord("maxrad", tempholder);
    }
    {Record tempholder; 
      QuantumHolder(reffreq).toRecord(error, tempholder);
      rec.defineRecord("reffreq", tempholder);
    }
    if(isthispb=="PB" || isthispb=="pb"){
      rec.define("isthisvp", false);
    }
    else{
      rec.define("isthisvp", true);
    }
    {Record tempholder; 
      MeasureHolder(squintdir).toRecord(error, tempholder);
      rec.defineRecord("squintdir", tempholder);
    }
    {Record tempholder; 
      QuantumHolder(squintreffreq).toRecord(error, tempholder);
      rec.defineRecord("squintreffreq", tempholder);
    }
    rec.define("dosquint", dosquint);
    {Record tempholder; 
      QuantumHolder(paincrement).toRecord(error, tempholder);
      rec.defineRecord("paincrement", tempholder);
    }
    rec.define("usesymmetricbeam", usesymmetricbeam);

    if(dopb){
        auto key = rec.asString(rec.fieldNumber("telescope"));
        if ( vplistdefaults_p.find(key) != vplistdefaults_p.end( ) ) vplistdefaults_p[key] = vplist_p.nfields( );
        else vplistdefaults_p.insert(std::pair<casacore::String, casacore::Int >(key, vplist_p.nfields( )));
    } 

    vplist_p.defineRecord(vplist_p.nfields(), rec); 

    return true;


  }
  Bool VPManager::setpbinvpoly(const String& telescope,
			       const String& othertelescope,
			       const Bool dopb, const Matrix<Double>& coeff,
			       const Vector<Double>& freqs,
			       const Quantity& maxrad,
			       const Quantity& reffreq,
			       const String& isthispb,
			       MDirection& squintdir,
			       const Quantity& squintreffreq,
			       const Bool dosquint,
			       const Quantity& paincrement,
			       const Bool usesymmetricbeam,
			       Record& rec) {

    std::lock_guard<std::recursive_mutex> locker(mutex_p);

    rec=Record();
    rec.define("name", "IPOLY");
    rec.define("isVP", PBMathInterface::IPOLY);
    if(telescope=="OTHER"){
      rec.define("telescope", othertelescope);
    }
    else{
      rec.define("telescope", telescope);
    }
    rec.define("dopb", dopb);
    rec.define("coeff", coeff);
    rec.define("fitfreqs", freqs);
    String error;
    {Record tempholder; 
      QuantumHolder(maxrad).toRecord(error, tempholder);
      rec.defineRecord("maxrad", tempholder);
    }
    {Record tempholder; 
      QuantumHolder(reffreq).toRecord(error, tempholder);
      rec.defineRecord("reffreq", tempholder);
    }
    if(isthispb=="PB" || isthispb=="pb"){
      rec.define("isthisvp", false);
    }
    else{
      rec.define("isthisvp", true);
    }
    {Record tempholder; 
      MeasureHolder(squintdir).toRecord(error, tempholder);
      rec.defineRecord("squintdir", tempholder);
    }
    {Record tempholder; 
      QuantumHolder(squintreffreq).toRecord(error, tempholder);
      rec.defineRecord("squintreffreq", tempholder);
    }
    rec.define("dosquint", dosquint);
    {Record tempholder; 
      QuantumHolder(paincrement).toRecord(error, tempholder);
      rec.defineRecord("paincrement", tempholder);
    }
    rec.define("usesymmetricbeam", usesymmetricbeam);

    if(dopb){
        auto key = rec.asString(rec.fieldNumber("telescope"));
        if ( vplistdefaults_p.find(key) != vplistdefaults_p.end( ) ) vplistdefaults_p[key] = vplist_p.nfields( );
        else vplistdefaults_p.insert(std::pair<casacore::String, casacore::Int >(key, vplist_p.nfields( )));
    } 

    vplist_p.defineRecord(vplist_p.nfields(), rec); 

    return true;


  }

  Bool VPManager::setpbnumeric(const String& telescope,
			       const String& othertelescope,
			       const Bool dopb, const Vector<Double>& vect,
			       const Quantity& maxrad,
			       const Quantity& reffreq,
			       const String& isthispb,
			       MDirection& squintdir,
			       const Quantity& squintreffreq,
			       const Bool dosquint,
			       const Quantity& paincrement,
			       const Bool usesymmetricbeam,
			       Record &rec) {

    std::lock_guard<std::recursive_mutex> locker(mutex_p);

    rec=Record();
    rec.define("name", "NUMERIC");
    rec.define("isVP", PBMathInterface::NUMERIC);
    if(telescope=="OTHER"){
      rec.define("telescope", othertelescope);
    }
    else{
      rec.define("telescope", telescope);
    }
    rec.define("dopb", dopb);
    rec.define("vect", vect);
    String error;
    {Record tempholder; 
      QuantumHolder(maxrad).toRecord(error, tempholder);
      rec.defineRecord("maxrad", tempholder);
    }
    {Record tempholder; 
      QuantumHolder(reffreq).toRecord(error, tempholder);
      rec.defineRecord("reffreq", tempholder);
    }
    if(isthispb=="PB" || isthispb=="pb"){
      rec.define("isthisvp", false);
    }
    else{
      rec.define("isthisvp", true);
    }
    {Record tempholder; 
      MeasureHolder(squintdir).toRecord(error, tempholder);
      rec.defineRecord("squintdir", tempholder);
    }
    {Record tempholder; 
      QuantumHolder(squintreffreq).toRecord(error, tempholder);
      rec.defineRecord("squintreffreq", tempholder);
    }
    rec.define("dosquint", dosquint);
    {Record tempholder; 
      QuantumHolder(paincrement).toRecord(error, tempholder);
      rec.defineRecord("paincrement", tempholder);
    }
    rec.define("usesymmetricbeam", usesymmetricbeam);

    if(dopb){
        auto key = rec.asString(rec.fieldNumber("telescope"));
        if ( vplistdefaults_p.find(key) != vplistdefaults_p.end( ) ) vplistdefaults_p[key] = vplist_p.nfields( );
        else vplistdefaults_p.insert(std::pair<casacore::String, casacore::Int >(key, vplist_p.nfields( )));
    } 

    vplist_p.defineRecord(vplist_p.nfields(), rec); 

    return true;

  }

  Bool VPManager::setpbimage(const String& tel, 
			     const String& other, 
			     const Bool dopb, 
			     const String& realimage, 
			     const String& imagimage,
			     const String& compleximage,
			     const Vector<String>& antennaNames,
			     Record& rec){

    std::lock_guard<std::recursive_mutex> locker(mutex_p);

    rec=Record();
    rec.define("name", "IMAGE");
    rec.define("isVP", PBMathInterface::IMAGE);
    if(tel=="OTHER"){
      rec.define("telescope", other);
    }
    else{
      rec.define("telescope", tel);
    }
    rec.define("dopb", dopb);
    rec.define("isthisvp", true);
    if(compleximage==""){
      rec.define("realimage", realimage);
      rec.define("imagimage", imagimage);
    }
    else{
      rec.define("compleximage", compleximage);
    }
    rec.define("antennanames", antennaNames);

    if(dopb){
        auto key = rec.asString(rec.fieldNumber("telescope"));
        if ( vplistdefaults_p.find(key) != vplistdefaults_p.end( ) ) vplistdefaults_p[key] = vplist_p.nfields( );
        else vplistdefaults_p.insert(std::pair<casacore::String, casacore::Int >(key, vplist_p.nfields( )));
    } 

    vplist_p.defineRecord(vplist_p.nfields(), rec); 
    
    return true;
    
  }

  Bool VPManager::imagepbinfo(Vector<Vector<String> >& names, Vector<Record>& imagebeams){
    
    std::lock_guard<std::recursive_mutex> locker(mutex_p);
    Int nRec=vplist_p.nfields();
    Bool retval=false;
    names=Vector<Vector<String> >(0);
    imagebeams=Vector<Record>(0);
    for(Int k=0; k< nRec; ++k){
      Record elrec=vplist_p.asRecord(k);
      
      if(elrec.isDefined("name") && elrec.asString("name")== String("IMAGE")){
	names.resize(names.nelements()+1, true);
	imagebeams.resize(names.nelements(), true);
	Vector<String> localstr;
	elrec.get("antennanames", localstr);
	names(names.nelements()-1)=localstr;
	imagebeams[names.nelements()-1]=elrec;
	retval=true;
      }
    }

    return retval;
     

  }

  Bool VPManager::setpbpoly(const String& telescope,
			    const String& othertelescope,
			    const Bool dopb, const Vector<Double>& coeff,
			    const Quantity& maxrad,
			    const Quantity& reffreq,
			    const String& isthispb,
			    MDirection& squintdir,
			    const Quantity& squintreffreq, const Bool dosquint,
			    const Quantity& paincrement,
			    const Bool usesymmetricbeam,
			    Record &rec) {

    std::lock_guard<std::recursive_mutex> locker(mutex_p);

    rec=Record();
    rec.define("name", "POLY");
    rec.define("isVP", PBMathInterface::POLY);
    if(telescope=="OTHER"){
      rec.define("telescope", othertelescope);
    }
    else{
      rec.define("telescope", telescope);
    }
    rec.define("dopb", dopb);
    rec.define("coeff", coeff);
    String error;
    {
      Record tempholder; 
      QuantumHolder(maxrad).toRecord(error, tempholder);
      rec.defineRecord("maxrad", tempholder);
    }
    {Record tempholder;
      QuantumHolder(reffreq).toRecord(error, tempholder);
      rec.defineRecord("reffreq", tempholder);
    }
    if(isthispb=="PB" || isthispb=="pb"){
      rec.define("isthisvp", false);
    }
    else{
      rec.define("isthisvp", true);
    }
    {Record tempholder;
      MeasureHolder(squintdir).toRecord(error, tempholder);
      rec.defineRecord("squintdir", tempholder);
    }
    {Record tempholder;
      QuantumHolder(squintreffreq).toRecord(error, tempholder);
      rec.defineRecord("squintreffreq", tempholder);
    }
    rec.define("dosquint", dosquint);
    {Record tempholder;
      QuantumHolder(paincrement).toRecord(error, tempholder);
      rec.defineRecord("paincrement", tempholder);
    }
    rec.define("usesymmetricbeam", usesymmetricbeam);

    if(dopb){
        auto key = rec.asString(rec.fieldNumber("telescope"));
        if ( vplistdefaults_p.find(key) != vplistdefaults_p.end( ) ) vplistdefaults_p[key] = vplist_p.nfields( );
        else vplistdefaults_p.insert(std::pair<casacore::String, casacore::Int >(key, vplist_p.nfields( )));
    } 

    vplist_p.defineRecord(vplist_p.nfields(), rec); 

    return true;

  }

  Bool VPManager::setpbantresptable(const String& telescope, const String& othertelescope,
				    const Bool dopb, const String& tablepath){

    std::lock_guard<std::recursive_mutex> locker(mutex_p);

    Record rec;
    rec.define("name", "REFERENCE");
    rec.define("isVP", PBMathInterface::NONE);
    if(telescope=="OTHER"){
      rec.define("telescope", othertelescope);
    }
    else{
      rec.define("telescope", telescope);
    }
    rec.define("dopb", dopb);
    rec.define("antresppath", tablepath);
    
    if(dopb){
        auto key = rec.asString(rec.fieldNumber("telescope"));
        if ( vplistdefaults_p.find(key) != vplistdefaults_p.end( ) ) vplistdefaults_p[key] = vplist_p.nfields( );
        else vplistdefaults_p.insert(std::pair<casacore::String, casacore::Int >(key, vplist_p.nfields( )));
    } 

    vplist_p.defineRecord(vplist_p.nfields(), rec); 

    return true;

  }
  

  Bool VPManager::setuserdefault(const Int vplistfield, // (-1 = reset to standard default, -2 = unset)
				 const String& telescope,
				 const String& antennatype){     

    std::lock_guard<std::recursive_mutex> locker(mutex_p);

    LogIO os;
    os <<  LogOrigin("VPManager", "setuserdefault");

    if((vplistfield < -2) || ((Int)(vplist_p.nfields()) <= vplistfield)){
      os << LogIO::SEVERE << "Vplist number " << vplistfield << " invalid." << endl
	 << "Valid entries are -2 (none), -1 (default), up to " << vplist_p.nfields()-1
	 << LogIO::POST;
      return false;
    }

    String antennaDesc = antennaDescription(telescope, antennatype);

    if(vplistfield>=0){
      const Record rec = vplist_p.subRecord(vplistfield);
      // check if this is a valid VP for this telescope
      String telName;
      const Int telFieldNumber=rec.fieldNumber("telescope");
      if (telFieldNumber!=-1){
	telName = rec.asString(telFieldNumber);
	if(telFromAntDesc(telName)!=telescope){
	  os << LogIO::SEVERE << " entry " << vplistfield << " does not point ot a valid VP for " << telescope
	     << LogIO::POST;
	  return false;
	}
      }
      Record srec = vplist_p.rwSubRecord(vplistfield);
      srec.define("dopb", true);
    }
    // unset set an existing default 
    auto vpiter = vplistdefaults_p.find(antennaDesc);
    if( vpiter != vplistdefaults_p.end( ) ){ 
      vplistdefaults_p.erase(vpiter);
    }
    if(vplistfield>-2){ // (-2 means don't set a default)
        if ( vplistdefaults_p.find(antennaDesc) != vplistdefaults_p.end( ) ) vplistdefaults_p[antennaDesc] = vplistfield;
        else vplistdefaults_p.insert(std::pair<casacore::String, casacore::Int >(antennaDesc,vplistfield));
    }

    return true;

  }

  Bool VPManager::getuserdefault(Int& vplistfield,
				 const String& telescope,
				 const String& antennatype){

    std::lock_guard<std::recursive_mutex> locker(mutex_p);

    String antDesc = antennaDescription(telescope, antennatype);

    auto vpiter = vplistdefaults_p.find(antDesc);
    if( vpiter != vplistdefaults_p.end( ) ){
      vplistfield = vpiter->second;
    } else {
        auto tpiter = vplistdefaults_p.find(telescope);
        if( tpiter != vplistdefaults_p.end( ) ) { // found global entry
            vplistfield = tpiter->second;
        } else {
            vplistfield = -2;
            return false;
        }
    }

    return true;

  }



  // fill vector with the names of the antenna types with available voltage patterns satisfying the given constraints
  Bool VPManager::getanttypes(Vector<String>& anttypes,
			      const String& telescope,
			      const MEpoch& obstime,
			      const MFrequency& freq, 
			      const MDirection& obsdirection // default: Zenith
			      ){

    std::lock_guard<std::recursive_mutex> locker(mutex_p);

    LogIO os;
    os << LogOrigin("VPManager", "getanttypes");

    Bool rval=false;

    anttypes.resize(0);

    Int ifield = -2;
    Bool isReference = true;

    // check for global response
    getuserdefault(ifield,telescope,""); 
    
    if(ifield==-1){ // internally defined PB does not distinguish antenna types
      anttypes.resize(1);
      anttypes(0) = "";
      rval = true;
    }
    else if(ifield>=0){ // externally defined PB 
      TableRecord antRec(vplist_p.asRecord(ifield));
      String thename = antRec.asString("name");
      if(thename=="REFERENCE"){ // points to an AntennaResponses table
	// query the antenna responses
	String antRespPath = antRec.asString("antresppath");
	if(!aR_p.init(antRespPath)){
	  os << LogIO::SEVERE
	     << "Invalid path defined in vpmanager for \"" << telescope << "\":" << endl
	     << antRespPath << endl
	     << LogIO::POST;
	}
	else{ // init successful

	  // construct a proper MFrequency 
	  MFrequency::Types fromFrameType;
	  MFrequency::getType(fromFrameType, freq.getRefString());
	  MPosition obsPos;
	  MFrequency::Ref fromFrame;
	  MFrequency mFreq = freq;
	  if(fromFrameType!=MFrequency::TOPO){
	    if(!MeasTable::Observatory(obsPos,telescope)){
	      os << LogIO::SEVERE << "\"" << telescope << "\" is not listed in the Observatories table."
		 << LogIO::POST;
	      return false;
	    }
	    fromFrame = MFrequency::Ref(fromFrameType, MeasFrame(obsdirection, obsPos, obstime));
	    mFreq = MFrequency(freq.get(Unit("Hz")), fromFrame);
	  } 

	  if(aR_p.getAntennaTypes(anttypes,
				  telescope, // (the observatory name, e.g. "ALMA" or "ACA")
				  obstime,
				  mFreq,
				  AntennaResponses::ANY, // the requested function type
				  obsdirection)){ // success
	    rval = true;
	  }
	}
      }
      else{ // we don't have a reference response
	isReference = false;
      }
    }

    if(ifield==-2 or !isReference){ // no global response or reference
      uInt count = 0;
      for( auto iter = vplistdefaults_p.begin( ); iter != vplistdefaults_p.end( ); ++iter ) {
	String aDesc = iter->first;
	if(telescope == telFromAntDesc(aDesc)){
	  String aType = antTypeFromAntDesc(aDesc);
	  Bool tFound = false;
	  for(uInt j=0; j<anttypes.size(); j++){
	    if(aType==anttypes(j)){ // already in list?
	      tFound = true;
	      break;
	    }
	  }
	  if(!tFound){
	    rval = true;
	    count++;
	    anttypes.resize(count, true);
	    anttypes(count-1) = aType;
	  }
	}
      } // end for i
    } 

    return rval;

  }


  // return number of voltage patterns satisfying the given constraints
  Int VPManager::numvps(const String& telescope,
			const MEpoch& obstime,
			const MFrequency& freq, 
			const MDirection& obsdirection // default: Zenith
			){

    std::lock_guard<std::recursive_mutex> locker(mutex_p);

    LogIO os;
    os << LogOrigin("VPManager", "numvps");

    Vector<String> antTypes;

    getanttypes(antTypes, telescope, obstime, freq, obsdirection);

    return antTypes.size();

  }


    // get the voltage pattern satisfying the given constraints
  Bool VPManager::getvp(Record &rec,
			const String& telescope,
			const MEpoch& obstime,
			const MFrequency& freq, 
			const String& antennatype, // default: "" 
			const MDirection& obsdirection){ // default is the Zenith

    std::lock_guard<std::recursive_mutex> locker(mutex_p);

    LogIO os;
    os << LogOrigin("VPManager", "getvp");
    
    Int ifield = -2;
    if(!getuserdefault(ifield,telescope,antennatype)){
      return false;
    }

    rec = Record();
    Int rval=false;

    String antDesc = antennaDescription(telescope, antennatype);

    if(ifield==-1){ // internally defined PB does not distinguish antenna types
	
      rec.define("name", "COMMONPB");
      rec.define("isVP", PBMathInterface::COMMONPB);
      rec.define("telescope", telescope);
      rec.define("dopb", true);
      rec.define("commonpb", telescope);
      rec.define("dosquint", false);
      String error;
      Record tempholder;
      QuantumHolder(Quantity(10.,"deg")).toRecord(error, tempholder);
      rec.defineRecord("paincrement", tempholder);
      rec.define("usesymmetricbeam", false);
	
      rval = true;
	
    }
    else if(ifield>=0){ // externally defined PB
      TableRecord antRec(vplist_p.asRecord(ifield));
      String thename = antRec.asString("name");
      if(thename=="REFERENCE"){ // points to an AntennaResponses table

	// query the antenna responses
	String antRespPath = antRec.asString("antresppath");
	if(!aR_p.init(antRespPath)){
	  os << LogIO::SEVERE
	     << "Invalid path defined in vpmanager for \"" << telescope << "\":" << endl
	     << antRespPath << endl
	     << LogIO::POST;
	  return false;
	}
	// init successful
	String functionImageName;
	uInt funcChannel;
	MFrequency nomFreq;
	MFrequency loFreq;
	MFrequency hiFreq;
	AntennaResponses::FuncTypes fType;
	MVAngle rotAngOffset;

	// construct a proper MFrequency 
	MFrequency::Types fromFrameType;
	MFrequency::getType(fromFrameType, freq.getRefString());
	MPosition obsPos;
	MFrequency::Ref fromFrame;
	MFrequency mFreq = freq;
	if(fromFrameType!=MFrequency::TOPO){
	  if(!MeasTable::Observatory(obsPos,telescope)){
	    os << LogIO::SEVERE << "\"" << telescope << "\" is not listed in the Observatories table."
	       << LogIO::POST;
	    return false;
	  }
	  fromFrame = MFrequency::Ref(fromFrameType, MeasFrame(obsdirection, obsPos, obstime));
	  mFreq = MFrequency(freq.get(Unit("Hz")), fromFrame);
	} 
	
	if(!aR_p.getImageName(functionImageName, // the path to the image
			      funcChannel, // the channel to use in the image  
			      nomFreq, // nominal frequency of the image (in the given channel)
			      loFreq, // lower end of the validity range
			      hiFreq, // upper end of the validity range
			      fType, // the function type of the image
			      rotAngOffset, // the response rotation angle offset
			      /////////////////////
			      telescope,
			      obstime,
			      mFreq,
			      AntennaResponses::ANY, // the requested function type
			      antennatype,
			      obsdirection)
	   ){
	  rec = Record();
	  return false;
	}
	
	// getImageName was successful
	
	// construct record
	rec = Record();
	Unit uHz("Hz");
	switch(fType){
	case AntennaResponses::AIF: // complex aperture illumination function
	  os << LogIO::WARN << "Responses type AIF provided for " << telescope << " in " << endl
	     << antRespPath << endl
	     << " not yet supported."
	     << LogIO::POST;
	  rval = false;
	  break;
	case AntennaResponses::EFP: // complex electric field pattern
	  rec.define("name", "IMAGE");
	  rec.define("isVP", PBMathInterface::IMAGE);
	  rec.define("telescope", telescope);
	  rec.define("dopb", true);
	  rec.define("isthisvp", true);
	  rec.define("compleximage", functionImageName);
	  rec.define("channel", funcChannel);
	  rec.define("reffreq", nomFreq.get(uHz).getValue());
	  rec.define("minvalidfreq", loFreq.get(uHz).getValue());
	  rec.define("maxvalidfreq", hiFreq.get(uHz).getValue());
	  rval = true;
	  break;
	case AntennaResponses::VP: // real voltage pattern
	  rec.define("name", "IMAGE");
	  rec.define("isVP", PBMathInterface::IMAGE);
	  rec.define("telescope", telescope);
	  rec.define("dopb", true);
	  rec.define("isthisvp", true);
	  rec.define("realimage", functionImageName);
	  rec.define("channel", funcChannel);
	  rec.define("reffreq", nomFreq.get(uHz).getValue());
	  rec.define("minvalidfreq", loFreq.get(uHz).getValue());
	  rec.define("maxvalidfreq", hiFreq.get(uHz).getValue());
	  rval = true;
	  break;
	case AntennaResponses::VPMAN: // the function is available in casa via the vp manager, i.e. use COMMONPB
	  // same as if ifield == -1
	  rec.define("name", "COMMONPB");
	  rec.define("isVP", PBMathInterface::COMMONPB);
	  rec.define("telescope", telescope);
	  rec.define("dopb", true);
	  rec.define("isthisvp", false);
	  rec.define("commonpb", telescope);
	  rec.define("dosquint", false);
	  {
	    String error;
	    Record tempholder;
	    QuantumHolder(Quantity(10.,"deg")).toRecord(error, tempholder);
	    rec.defineRecord("paincrement", tempholder);
	  }
	  rec.define("usesymmetricbeam", false);
	  rval = true;
	  break;
	case AntennaResponses::INTERNAL: // the function is generated using the BeamCalc class
	  {
	    String antRayPath = functionImageName;

	    Double refFreqHz = 0.; // the TOPO ref freq in Hz

	    // determine TOPO reference frequency
	    if(fromFrameType!=MFrequency::TOPO){
	      MFrequency::Ref toFrame = MFrequency::Ref(MFrequency::TOPO, MeasFrame(obsdirection, obsPos, obstime));
	      MFrequency::Convert freqTrans(uHz, fromFrame, toFrame);
	      refFreqHz = freqTrans(mFreq.get(uHz).getValue()).get(uHz).getValue();
	      cout << "old freq " << mFreq.get(uHz).getValue() << ", new freq " << refFreqHz << endl;
	    }
	    else{
	      refFreqHz = mFreq.get(uHz).getValue();
	    }

	    String beamCalcedImagePath = "./BeamCalcTmpImage_"+telescope+"_"+antennatype+"_"
	      +String::toString(refFreqHz/1E6)+"MHz";
	  
	    // calculate the beam
	    
	    if(!(telescope=="ALMA" || telescope=="ACA" || telescope =="OSF")){
	      os << LogIO::WARN << "Responses type INTERNAL provided for \"" << telescope << " in " << endl
		 << antRespPath << endl
		 << " not yet supported."
		 << LogIO::POST;
	      rval = false;
	    }
	    else{ // telescope=="ALMA" || telescope=="ACA" || telescope =="OSF"
	      try{
		// handle preexisting beam image
		Directory f(beamCalcedImagePath);
		if(f.exists()){
		  os << LogIO::NORMAL << "Will re-use VP image \"" << beamCalcedImagePath << "\"" << LogIO::POST;
		}
		else{
		  CoordinateSystem coordsys;
	      
		  // DirectionCoordinate
		  Matrix<Double> xform(2,2);                                    
		  xform = 0.0; xform.diagonal() = 1.0;                          
		  DirectionCoordinate dirCoords(MDirection::AZELGEO,                  
						Projection(Projection::SIN),        
						0.0, 0.0,
						-0.5*C::pi/180.0/3600.0 * 5E11/refFreqHz, 
						0.5*C::pi/180.0/3600.0 * 5E11/refFreqHz,        
						xform,                              
						128., 128.);  // 256/2.
		  Vector<String> units(2); 
		  //units = "deg";                       
		  //dirCoords.setWorldAxisUnits(units);                               
		  
		  // StokesCoordinate
		  Vector<Int> stoks(4);
		  stoks(0) = Stokes::XX;
		  stoks(1) = Stokes::XY;
		  stoks(2) = Stokes::YX;
		  stoks(3) = Stokes::YY;
		  StokesCoordinate stokesCoords(stoks);	
		  
		  // SpectralCoordinate
		  SpectralCoordinate spectralCoords(MFrequency::TOPO,           
						    refFreqHz,                 
						    1.0E+3, // dummy increment                  
						    0,                             
						    refFreqHz);          
		  units.resize(1);
		  units = "Hz";
		  spectralCoords.setWorldAxisUnits(units);
		  
		  coordsys.addCoordinate(dirCoords);
		  coordsys.addCoordinate(stokesCoords);
		  coordsys.addCoordinate(spectralCoords);
		  
		  TiledShape ts(IPosition(4,256,256,4,1));
		  PagedImage<Complex> im(ts, coordsys, beamCalcedImagePath);
		  im.set(Complex(1.0,0.0));
		  // set XY and YX to zero
		  IPosition windowShape(4,im.shape()(0), im.shape()(1), 1, im.shape()(3));
		  LatticeStepper stepper(im.shape(), windowShape);
		  LatticeIterator<Complex> it(im, stepper);
		  Int planeNumber = 0;
		  for (it.reset(); !it.atEnd(); it++) {
		    if(planeNumber==1 || planeNumber==2){
		      it.woCursor() = Complex(0.,0.);
		    }
		    planeNumber++;
		  }
		  
		  // perform the ray tracing
		  ALMACalcIlluminationConvFunc almaPB;
		  Long cachesize=(HostInfo::memoryTotal(true)/8)*1024;
		  almaPB.setMaximumCacheSize(cachesize);
		  almaPB.setAntRayPath(antRayPath);
		  almaPB.applyVP(im, telescope, obstime, antennatype, antennatype, 
				 MVFrequency(refFreqHz), 
				 rotAngOffset.radian(), // the parallactic angle offset
				 true); // doSquint
		} // endif exists
	      } catch (AipsError x) {
		os << LogIO::SEVERE
		   << "BeamCalc failed with message " << endl
		   << "   " << x.getMesg()
		   << LogIO::POST;
		return false;
	      }
	    
	      // construct record
	      rec.define("name", "IMAGE");
	      rec.define("isVP", PBMathInterface::IMAGE);
	      rec.define("isthisvp", true);
	      rec.define("telescope", telescope);
	      rec.define("dopb", true);
	      rec.define("compleximage", beamCalcedImagePath);
	      rec.define("channel", 0);
	      rec.define("antennatype", antennatype);
	      rec.define("reffreq", refFreqHz);
	      rec.define("minvalidfreq", refFreqHz-refFreqHz/10.*5.);
	      rec.define("maxvalidfreq", refFreqHz+refFreqHz/10.*5.);
	      rval = true;
	    }
	  }
	  break;
	case AntennaResponses::NA: // not available
	default:
	  rval = false;
	  break;
	} // end switch(ftype)
      } 
      else{ // we have a PBMath response
      
	rec = vplist_p.subRecord(ifield);
	rval = true;
	
      } // end if internally defined
    }
    
    return rval;

  }

  // get the voltage pattern without giving observation parameters
  Bool VPManager::getvp(Record &rec,
			const String& telescope,
			const String& antennatype // default: "" 
			){ 

    std::lock_guard<std::recursive_mutex> locker(mutex_p);

    LogIO os;
    os << LogOrigin("VPManager", "getvp2");

    MEpoch obstime;
    MFrequency freq;
    MDirection obsdirection;
    
    Int ifield = -2;
    if(!getuserdefault(ifield,telescope,antennatype)){
      return false;
    }

    rec = Record();
    Int rval=false;

    if(ifield==-1){ // internally defined PB, obs parameters ignored 
      rval = getvp(rec, telescope, obstime, freq, antennatype, obsdirection);
    }
    else if(ifield>=0){ // externally defined PB
      TableRecord antRec(vplist_p.asRecord(ifield));
      String thename = antRec.asString("name");
      if(thename=="REFERENCE"){ // points to an AntennaResponses table
	os << LogIO::SEVERE
	   << "Need to provide observation parameters time, frequency, and direction to access AntennaResponses table."
	   << LogIO::POST;
	return false;
      }
      else{ // we have a PBMath response
	rec = vplist_p.subRecord(ifield);
	rval = true;
      } 
    }// end if internally defined
    
    return rval;

  }

  Bool VPManager::getvps(Vector<Record> & unique_out_rec_list, // the list of unique beam records
			 Vector<Vector<uInt> >& beam_index, // indices to the above vector in sync with AntennaNames
			 const String& telescope,
			 const Vector<MEpoch>& inpTimeRange, // only elements 0 and 1 are used; if 1 is not present it is assumed to be inf
			 const Vector<MFrequency>& inpFreqRange, // must contain at least one element; beams will be provided for each element 
			 const Vector<String>& AntennaNames, // characters 0 and 1 are used for ALMA to determine the antenna type
			 const MDirection& obsdirection){ // default is the Zenith

    LogIO os;
    os << LogOrigin("VPManager", "getvp3");

    Bool rval=true;

    if (inpTimeRange.size()==0){
      os << LogIO::WARN	 << "No observation time provided." << LogIO::POST;
      return false;
    }
    if (inpFreqRange.size()==0){
      os << LogIO::WARN	 << "No observation frequency provided." << LogIO::POST;
      return false;
    }
    if (AntennaNames.size()==0){
      os << LogIO::WARN	 << "No antenna names provided." << LogIO::POST;
      return false;
    }

    Bool checkTimeDep = false;
    Unit uS("s");
    if(inpTimeRange.size()>1 && (inpTimeRange[0].get(uS) < inpTimeRange[1].get(uS))){
      checkTimeDep = true;
    }

    unique_out_rec_list.resize(0);
    beam_index.resize(0);

    vector<Record> recList;
    vector<vector<uInt> > beamIndex;

    if(!(telescope=="ALMA" || telescope=="ACA" || telescope=="OSF")){
      os << LogIO::WARN << "Don't know how to determine antenna type from antenna name for telescope " << telescope << LogIO::POST;
      os << LogIO::WARN << "Will try to use whole name ..." << LogIO::POST;
    }

    for(uInt k=0; k<AntennaNames.size(); k++){

      vector<uInt> index;

      for(uInt i=0; i<inpFreqRange.size(); i++){

	Record rec0;

	MEpoch obstime = inpTimeRange[0];
	MFrequency freq=inpFreqRange[i];

	String antennatype=AntennaNames[k];
	if(telescope=="ALMA" || telescope=="ACA" || telescope=="OSF"){
	  antennatype=antennatype(0,2);
	}
	//else {} // warning is given above, outside of the loop

	if (getvp(rec0, telescope, obstime, freq, antennatype, obsdirection)){

	  if(checkTimeDep){ // check if there is a step in time
	    Record recOther;
	    if (getvp(recOther, telescope, inpTimeRange[1], freq, antennatype, obsdirection)){
	      if(!vpRecIsIdentical(rec0,recOther)){
		os << LogIO::WARN << "Primary beam changes during observation time range." << LogIO::POST;
		return false;
	      }
	    }
	  }

	  Bool notPresent=true;
	  for(uInt j=0; j<recList.size(); j++){ // check if we already have this PB in our collection
	    if(vpRecIsIdentical(rec0,recList[j])){
	      index.push_back(j);
	      notPresent=false;
	      break;
	    }
	  }
	  if(notPresent){ // put new beam into the list
	    index.push_back(recList.size());
	    recList.push_back(rec0);
	  }
	  
	}
	else{
	  rval=false;
	  break;
	}
	
	if(!rval) break;

      } // end for i

      beamIndex.push_back(index);

    } // end for k

    if(rval){
      unique_out_rec_list=Vector<Record>(recList);
      beam_index.resize(beamIndex.size());
      for(uInt i=0; i<beamIndex.size(); i++){
        beam_index[i] = Vector<uInt>(beamIndex[i]);
      }
    }

    return rval;

  }

  Bool VPManager::vpRecIsIdentical(const Record& rec0, const Record& rec1){
    
    Bool rval = false;
    if( (rec0.nfields() == rec1.nfields()) &&
	rec0.asString("name") == rec1.asString("name")
	){
      
      if(rec0.isDefined("compleximage")){
	if(rec1.isDefined("compleximage") &&
	   rec0.asString("compleximage") == rec1.asString("compleximage") &&
	   rec0.asDouble("reffreq") == rec1.asDouble("reffreq")){
	  rval = true;
	}
      }
      else if(rec0.isDefined("realimage")){
	if(rec1.isDefined("realimage") &&
	   rec0.asString("realimage") == rec1.asString("realimage") &&
	   rec0.asDouble("reffreq") == rec1.asDouble("reffreq")){
	  rval = true;
	}
      }
      else{
	rval=true;
      }

      Vector<String> fnames(5);
      fnames[0]="dosquint";
      fnames[1]="usesymmetricbeam";
      fnames[2]="isthisvp";
      fnames[3]="isVP";
      fnames[4]="dopb";
      for(uInt i=0; i<fnames.size(); i++){
	if( rval && rec0.isDefined(fnames[i])){
	  rval=false;
	  if(rec1.isDefined(fnames[i]) &&
	     (rec0.asBool(fnames[i]) == rec1.asBool(fnames[i]))
	     ){
	    rval = true;
	  }
	}
      }
    } // end if

    return rval;
  }


} //# NAMESPACE CASA - END
