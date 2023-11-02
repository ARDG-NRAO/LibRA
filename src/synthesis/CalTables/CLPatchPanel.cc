//# CTPatchPanel.cc: Implementation of CTPatchPanel.h
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
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#

#include <synthesis/CalTables/CLPatchPanel.h>
#include <synthesis/CalTables/CTInterface.h>
#include <synthesis/CalTables/CTIter.h>

#include <casacore/scimath/Mathematics/InterpolateArray1D.h>
#include <casacore/casa/Utilities/GenSort.h>
#include <casacore/casa/OS/Path.h>

#include <casacore/ms/MSSel/MSSelectableTable.h>
#include <casacore/ms/MSSel/MSSelection.h>
#include <casacore/ms/MSSel/MSSelectionTools.h>

#include <casacore/casa/aips.h>

#define CTPATCHPANELVERB false

//#include <casa/BasicSL/Constants.h>
//#include <casa/OS/File.h>
#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogSink.h>
#include <casacore/casa/Logging/LogIO.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

CalPatchKey::CalPatchKey(IPosition keyids) :
  cpk_(keyids.asVector())
{}

// Lexographical lessthan
Bool CalPatchKey::operator<(const CalPatchKey& other) const {

  // This method does a lexigraphical less-than, wherein negative
  //  elements in either operand behave as equal (reflexively not
  //  less than)

  // Loop over elements in precendence order
  for (Int i=0;i<5;++i) {
    if (cpk_[i]>-1 && other.cpk_[i]>-1 && cpk_[i]!=other.cpk_[i])
      // both non-negative and not equal, so evaluate element <
      return cpk_[i]<other.cpk_[i];
  }
  // All apparently equal so "<" is false
  return false;
}

MSCalPatchKey::MSCalPatchKey(Int obs,Int fld,Int ent,Int spw,Int ant) :
  CalPatchKey(IPosition(5,obs,fld,ent,spw,ant)),
  obs_(obs),fld_(fld),ent_(ent),spw_(spw),ant_(ant)
{}

// text output
String MSCalPatchKey::print() const {
  return "obs="+(obs_<0 ? "*" : String::toString(obs_))+" "
    "fld="+(fld_<0 ? "*" : String::toString(fld_))+" "
    "intent="+(ent_<0 ? "*" : String::toString(ent_))+" "
    "spw="+(spw_<0 ? "*" : String::toString(spw_))+" "
    "ant="+(ant_<0 ? "*" : String::toString(ant_));
}


CTCalPatchKey::CTCalPatchKey(Int clsl,Int obs,Int fld,Int spw,Int ant) :
  CalPatchKey(IPosition(5,clsl,obs,fld,spw,ant)),
  clsl_(clsl),obs_(obs),fld_(fld),spw_(spw),ant_(ant)
{}

// text output
String CTCalPatchKey::print() const {
  return "cl="+(clsl_<0 ? "*" : String::toString(clsl_))+" "
    "obs="+(obs_<0 ? "*" : String::toString(obs_))+" "
    "fld="+(fld_<0 ? "*" : String::toString(fld_))+" "
    "spw="+(spw_<0 ? "*" : String::toString(spw_))+" "
    "ant="+(ant_<0 ? "*" : String::toString(ant_));
}


CalMap::CalMap() :
  vcalmap_()
{}

CalMap::CalMap(const Vector<Int>& calmap) :
  vcalmap_(calmap)
{
  //  cout << "calmap addresses: " << calmap.data() << " " << vcalmap_.data()
  //       << " nrefs= " << calmap.nrefs() << " " << vcalmap_.nrefs() << endl;
}

Int CalMap::operator()(Int msid) const {
  // TBD: reconsider algorithm (maybe just return msid if over-run?)
  Int ncalmap=vcalmap_.nelements();
  return (msid<ncalmap ? vcalmap_(msid) :
	  (ncalmap>0 ? vcalmap_(ncalmap-1) : msid) ); // Avoid going off end
}

Vector<Int> CalMap::ctids(const Vector<Int>& msids) const {
  uInt ncalmap=vcalmap_.nelements();

  // If vector map unspecified, the calling context must work it out
  //  (for obs, fld, it means no specific mapping, just use all avaiable;
  //     for spw, ant, it means [probably] use the same id)
  if (ncalmap<1 ||
      (ncalmap==1 && vcalmap_[0]<0))
    return Vector<Int>(1,-1);

  Vector<Bool> calmask(ncalmap,false);
  if (msids.nelements()==1 &&
      msids[0]<0)
    // MS ids indefinite, so all are ok
    calmask.set(true);
  else {
    // just do the ones that are requested
    for (uInt i=0;i<msids.nelements();++i) {
      const uInt& thismsid=msids(i);
      calmask(thismsid<ncalmap?thismsid:ncalmap-1)=true;
    }
  }
  Vector<Int> reqids;
  reqids=vcalmap_(calmask).getCompressedArray();
  Int nsort=genSort(reqids,Sort::Ascending,(Sort::QuickSort | Sort::NoDuplicates));
  reqids.resize(nsort,true);
  return reqids;
}

Vector<Int> CalMap::msids(Int ctid,const Vector<Int>& superset) const {

  uInt ncalmap=vcalmap_.nelements();

  // If vector map unspecified, return [-1] which signals to calling
  //  context that it must figure it out on its own (all or identity)
  if (ncalmap<1 ||
      (ncalmap==1 && vcalmap_[0]<0) )
    return Vector<Int>(1,-1);

  // mask vcalmap_ by specified ctid...
  Vector<Int> msidlist(ncalmap);
  indgen(msidlist);
  Vector<Bool> msidmask(vcalmap_==ctid);
  // ...and limit to superset, if specified
  if (superset.nelements()>0 && superset[0]>-1)
    for (Int i=0;i<Int(msidmask.nelements());++i)
      if (!anyEQ(superset,i))
	msidmask[i]=false;  // exclude if not in superset

  // Return the ms id list
  return msidlist(msidmask).getCompressedArray();
}


FieldCalMap::FieldCalMap() :
  CalMap(),
  fieldcalmap_("")
{}

FieldCalMap::FieldCalMap(const Vector<Int>& calmap) :
  CalMap(calmap),
  fieldcalmap_("")
{}

FieldCalMap::FieldCalMap(const String fieldcalmap,
			 const MeasurementSet& ms, const NewCalTable& ct) :
  CalMap(),
  fieldcalmap_(fieldcalmap)
{

  if (fieldcalmap_=="nearest")
    // Calculate nearest map
    setNearestFieldMap(ms,ct);
  else
    // Attempt field selection
    setSelectedFieldMap(fieldcalmap,ms,ct);

}

FieldCalMap::FieldCalMap(const String fieldcalmap, 
			 const MeasurementSet& ms, const NewCalTable& ct,
			 String& extfldsel) :
  CalMap(),
  fieldcalmap_(fieldcalmap)
{

  if (fieldcalmap_=="nearest") 
    // Calculate nearest map
    setNearestFieldMap(ms,ct);
  else 
    // Attempt field selection
    setSelectedFieldMap(fieldcalmap,ms,ct,extfldsel);

}

void FieldCalMap::setNearestFieldMap(const MeasurementSet& ms, const NewCalTable& ct) {
  // Access MS and CT columns
  MSFieldColumns msfc(ms.field());
  ROCTColumns ctc(ct);
  setNearestFieldMap(msfc,ctc);
}
void FieldCalMap::setNearestFieldMap(const NewCalTable& ctasms, const NewCalTable& ct) {
  // Access MS and CT columns
  ROCTFieldColumns msfc(ctasms.field());
  ROCTColumns ctc(ct);
  setNearestFieldMap(msfc,ctc);
}

void FieldCalMap::setNearestFieldMap(const MSFieldColumns& msfc, const ROCTColumns& ctc) {

  // Nominally, this many field need a map
  Int nMSFlds=msfc.nrow();
  vcalmap_.resize(nMSFlds);
  vcalmap_.set(-1);  // no map

  // Discern _available_ fields in the CT
  Vector<Int> ctFlds;
  ctc.fieldId().getColumn(ctFlds);
  Int nAvFlds=genSort(ctFlds,Sort::Ascending,(Sort::QuickSort | Sort::NoDuplicates));
  ctFlds.resize(nAvFlds,true);

  // If only one CT field, just use it
  if (nAvFlds==1)
    vcalmap_.set(ctFlds(0));
  else {
    // For each MS field, find the nearest available CT field
    MDirection msdir,ctdir;
    Vector<Double> sep(nAvFlds);
    IPosition ipos(1,0);  // get the first direction stored (no poly yet)
    for (Int iMSFld=0;iMSFld<nMSFlds;++iMSFld) {
      msdir=msfc.phaseDirMeas(iMSFld); // MS fld dir
      sep.set(DBL_MAX);
      for (Int iCTFld=0;iCTFld<nAvFlds;++iCTFld) {
	// Get cal field direction, converted to ms field frame
	ctdir=ctc.field().phaseDirMeas(ctFlds(iCTFld));
	MDirection::Convert(ctdir,msdir.getRef());
	sep(iCTFld)=ctdir.getValue().separation(msdir.getValue());
      }
      // Sort separations
      Vector<uInt> ord;
      Int nsep=genSort(ord,sep,Sort::Ascending,Sort::QuickSort);

      //cout << iMSFld << ":" << endl;
      //cout << "    ord=" << ord << endl;
      //cout << "   nsep=" << nsep << endl;
      //cout << "    sep=" << sep << " " << sep*(180.0/C::pi)<< endl;

      // Trap case of duplication of nearest separation
      if (nsep>1 && sep(ord(1))==sep(ord(0)))
	throw(AipsError("Found more than one field at minimum distance, can't decide!"));

      vcalmap_(iMSFld)=ctFlds(ord(0));
    }
  }
  //  cout << "FCM::setNearestFieldMap:***************  vcalmap_ = " << vcalmap_ << endl;

}

void FieldCalMap::setSelectedFieldMap(const String& fieldsel,
				      const MeasurementSet& ms,
				      const NewCalTable& ct) {

  // Nominally, this many fields need a map
  Int nMSFlds=ms.field().nrow();
  vcalmap_.resize(nMSFlds);
  vcalmap_.set(-1);  // no map

  CTInterface cti(ct);
  MSSelection mss;
  mss.setFieldExpr(fieldsel);
  TableExprNode ten=mss.toTableExprNode(&cti);
  Vector<Int> fieldlist=mss.getFieldList();

  if (fieldlist.nelements()>1)
    throw(AipsError("Field mapping by selection can support only one field."));

  if (fieldlist.nelements()>0)
    vcalmap_.set(fieldlist[0]);

  //  cout << "FCM::setSelectedFieldMap:***************  vcalmap_ = " << vcalmap_ << endl;

}

void FieldCalMap::setSelectedFieldMap(const String& fieldsel,
				      const MeasurementSet& ms,
				      const NewCalTable& ct,
				      String& extfldsel) {

  
  try {

    CTInterface cti(ct);
    MSSelection mss;
    mss.setFieldExpr(fieldsel);
    TableExprNode ten=mss.toTableExprNode(&cti);
    Vector<Int> fieldlist=mss.getFieldList();

    // Nominally, don't apply selection externally
    extfldsel="";

    if (fieldlist.nelements()>1)
      // trigger external selection (multi-field case; see old gainfield)
      extfldsel=fieldsel;
    else if (fieldlist.nelements()==1) {
      // exactly 1 means fldmap=[f]*nfld

      // Nominally, this many fields need a map
      Int nMSFlds=ms.field().nrow();
      vcalmap_.resize(nMSFlds);
      vcalmap_.set(fieldlist[0]);
    }
    else
      // Field selection found no indices
      throw(AipsError(fieldsel+" matches no fields."));

  }
  catch ( AipsError err ) {

    throw(AipsError("Field mapping by selection failure: "+err.getMesg()));

  }


  //  cout << "FCM::setSelectedFieldMap:***************  vcalmap_ = " << vcalmap_ << endl;

  return;
}


CalLibSlice::CalLibSlice(String obs,String fld, String ent, String spw,
			 String tinterp,String finterp,
			 Vector<Int> obsmap, Vector<Int> fldmap,
			 Vector<Int> spwmap, Vector<Int> antmap) :
  obs(obs),fld(fld),ent(ent),spw(spw),
  tinterp(tinterp),finterp(finterp),
  obsmap(obsmap),fldmap(fldmap),spwmap(spwmap),antmap(antmap),extfldsel("")
{}

// Construct from a Record
CalLibSlice::CalLibSlice(const Record& clslice,
			 const MeasurementSet& ms,
			 const NewCalTable& ct) :
  obs(),fld(),ent(),spw(),
  tinterp(),finterp(),
  obsmap(),fldmap(),spwmap(),antmap(),
  extfldsel("")
{

  if (clslice.isDefined("obs")) {
    obs=clslice.asString("obs");
  }
  if (clslice.isDefined("field")) {
    fld=clslice.asString("field");
  }
  if (clslice.isDefined("intent")) {
    ent=clslice.asString("intent");
  }
  if (clslice.isDefined("spw")) {
    spw=clslice.asString("spw");
  }

  if (clslice.isDefined("tinterp")) {
    tinterp=clslice.asString("tinterp");
    if (tinterp=="") {
      tinterp="linear";
    }
  }
  if (clslice.isDefined("finterp")) {
    finterp=clslice.asString("finterp");
  }

  if (clslice.isDefined("obsmap")) {
    //cout << "obsmap.dataType() = " << clslice.dataType("obsmap") << endl;
    if (clslice.dataType("obsmap")==TpArrayInt)
      obsmap=CalMap(Vector<Int>(clslice.asArrayInt("obsmap")));
  }
  if (clslice.isDefined("fldmap")) {
    if (clslice.dataType("fldmap")==TpArrayInt)
      fldmap=FieldCalMap(clslice.asArrayInt("fldmap"));
    if (clslice.dataType("fldmap")==TpString)
      fldmap=FieldCalMap(clslice.asString("fldmap"),ms,ct,extfldsel);
  }
  if (clslice.isDefined("spwmap")) {
    //    cout << "spwmap.dataType() = " << clslice.dataType("spwmap") << endl;
    if (clslice.dataType("spwmap")==TpArrayInt)
      spwmap=CalMap(clslice.asArrayInt("spwmap"));
  }
  if (clslice.isDefined("antmap")) {
    //    cout << "antmap.dataType() = " << clslice.dataType("antmap") << endl;
    if (clslice.dataType("antmap")==TpArrayInt)
      antmap=CalMap(clslice.asArrayInt("antmap"));
  }


}

// Return contents as a Record
Record CalLibSlice::asRecord() {
  Record rec;
  rec.define("obs",obs);
  rec.define("field",fld);
  rec.define("intent",ent);
  rec.define("spw",spw);

  rec.define("tinterp",tinterp);
  rec.define("finterp",finterp);

  rec.define("obsmap",obsmap.vmap());
  rec.define("fldmap",fldmap.vmap());
  rec.define("spwmap",spwmap.vmap());
  rec.define("antmap",antmap.vmap());

  return rec;
}

Bool CalLibSlice::validateCLS(const Record& clslice) {

  String missing("");

  if (!clslice.isDefined("obs"))
    missing+="obs ";
  if (!clslice.isDefined("field"))
    missing+="field ";
  if (!clslice.isDefined("intent"))
    missing+="intent ";
  if (!clslice.isDefined("spw"))
    missing+="spw ";

  if (!clslice.isDefined("tinterp"))
    missing+="tinterp ";
  if (!clslice.isDefined("finterp"))
    missing+="finterp ";

  if (!clslice.isDefined("obsmap"))
    missing+="obsmap ";
  if (!clslice.isDefined("fldmap"))
    missing+="fldmap ";
  if (!clslice.isDefined("spwmap"))
    missing+="swmap ";
  if (!clslice.isDefined("antmap"))
    missing+="antmap";


  if (missing.length()>0)
    throw(AipsError(missing));

  // Everything is ok if we get here
  return true;
}


String CalLibSlice::state() {

  ostringstream o;

  o << "     MS: obs="+obs+" fld="+fld+" intent="+ent+" spw="+spw << endl
    << "     CT: tinterp="+tinterp << " finterp="+finterp << endl
    << "         obsmap=" << obsmap.vmap()
    << "         fldmap=";

  if (extfldsel=="")
    o << fldmap.vmap();
  else
    o << extfldsel+" (by selection)";

  o << endl
    << "         spwmap=" << spwmap.vmap()
    << "         antmap=" << antmap.vmap()
    << endl;

  return String(o);
}


CLPPResult::CLPPResult() :
  result_(),resultFlag_()
{}

CLPPResult::CLPPResult(const IPosition& shape) :
  result_(shape,0.0),
  resultFlag_(shape,true)
{}
CLPPResult::CLPPResult(uInt nPar,uInt nFPar,uInt nChan,uInt nElem) :
  result_(nFPar,nChan,nElem,0.0),
  resultFlag_(nPar,nChan,nElem,true)
{}

CLPPResult& CLPPResult::operator=(const CLPPResult& other) {
  // Avoid Array deep copies!
  result_.reference(other.result_);
  resultFlag_.reference(other.resultFlag_);
  return *this;
}
void CLPPResult::resize(uInt nPar,uInt nFPar,uInt nChan,uInt nElem) {
  result_.resize(nFPar,nChan,nElem);
  result_.set(1.0);
  resultFlag_.resize(nPar,nChan,nElem);
  resultFlag_.set(true);
}

// (caltable only, mainly for testing)
CLPatchPanel::CLPatchPanel(const String& ctname,
			   const Record& callib,
			   VisCalEnum::MatrixType mtype,
			   Int nPar,
			   const CTTIFactoryPtr cttifactoryptr) :
  ct_(NewCalTable::createCT(ctname,Table::Old,Table::Memory)),
  ctasms_(NewCalTable::createCT(ctname,Table::Old,Table::Memory)),
  ms_(),
  mtype_(mtype),
  isCmplx_(false),
  nPar_(nPar),
  nFPar_(nPar),

  nChanIn_(),
  freqIn_(),
  refFreqIn_(),

  nMSObs_(ct_.observation().nrow()),
  nMSFld_(ct_.field().nrow()),
  nMSSpw_(ct_.spectralWindow().nrow()),
  nMSAnt_(ct_.antenna().nrow()),
  nMSElem_(0),  // set non-trivially in ctor body
  nCTObs_(ct_.observation().nrow()),
  nCTFld_(ct_.field().nrow()),
  nCTSpw_(ct_.spectralWindow().nrow()),
  nCTAnt_(ct_.antenna().nrow()),
  nCTElem_(0), // set non-trivially in ctor body
  lastresadd_(nMSSpw_,NULL),
  cttifactoryptr_(cttifactoryptr)

  //  elemMap_(),
  //  conjTab_(),

{
  if (CTPATCHPANELVERB) cout << "CLPatchPanel::CLPatchPanel(<no MS>)" << endl;

  //  ia1dmethod_=ftype(freqType_);
  //  cout << "ia1dmethod_ = " << ia1dmethod_ << endl;

  switch(mtype_) {
  case VisCalEnum::GLOBAL: {

    throw(AipsError("CLPatchPanel::ctor: No non-Mueller/Jones yet."));

    nCTElem_=1;
    nMSElem_=1;
    break;
  }
  case VisCalEnum::MUELLER: {
    nCTElem_=nCTAnt_*(nCTAnt_+1)/2;
    nMSElem_=nMSAnt_*(nMSAnt_+1)/2;
    break;
  }
  case VisCalEnum::JONES: {
    nCTElem_=nCTAnt_;
    nMSElem_=nMSAnt_;
    break;
  }
  }

  // How many _Float_ parameters?
  isCmplx_=(ct_.keywordSet().asString("ParType")=="Complex");
  if (isCmplx_) // Complex input
    nFPar_*=2;  // interpolating 2X as many Float values

  // Set CT channel/freq info
  CTSpWindowColumns ctspw(ct_.spectralWindow());
  ctspw.numChan().getColumn(nChanIn_);
  freqIn_.resize(nCTSpw_);
  for (uInt iCTspw=0;iCTspw<ctspw.nrow();++iCTspw)
    ctspw.chanFreq().get(iCTspw,freqIn_(iCTspw),true);
  ctspw.refFrequency().getColumn(refFreqIn_);

  // Initialize maps
  //  (ctids, msids, result arrays)
  // TBD

  //void CLPatchPanel::unpackCLinstance(CalLibSlice cls) {

  // Loop over callib slices
  //  (a callib slice is one MS selection and interp setup for the present caltable)
  uInt nkey=callib.nfields();
  Int icls=-1;
  for (uInt ikey=0;ikey<nkey;++ikey) {

    // skip
    if (callib.type(ikey)!=TpRecord)
      continue;

    ++icls;

    // The current callib slice
    CalLibSlice cls(callib.asRecord(ikey));

    logsink_ << LogIO::NORMAL << ".   " << icls << ":" << endl
	     << cls.state() << LogIO::POST;

    // Apply callib instance MS selection to the "MS" (in this case it is a CT)
    NewCalTable clsms(ctasms_);
    this->selectOnCT(clsms,ctasms_,cls.obs,cls.fld,cls.spw,"");

    // Extract the MS indices we must calibrate
    Vector<Int> reqMSobs(1,-1),reqMSfld(1,-1);
    if (cls.obs.length()>0)
      reqMSobs.reference(this->getCLuniqueObsIds(clsms));
    if (cls.fld.length()>0)
      reqMSfld.reference(this->getCLuniqueFldIds(clsms));
    Vector<Int> reqMSspw(this->getCLuniqueSpwIds(clsms));
    Vector<Int> reqMSant(nMSAnt_);
    indgen(reqMSant);

    //    cout << "reqMSobs = " << reqMSobs << endl;
    //    cout << "reqMSfld = " << reqMSfld << endl;
    //    cout << "reqMSspw = " << reqMSspw << endl;
    //    cout << "reqMSant = " << reqMSant << endl;

    // The intent list from the callib instance, to be used to index the msci_
    Vector<Int> theseMSint(1,-1);  // Details TBD

    //    cout << "theseMSint = " << theseMSint << endl;

    // WE DO TIME-ISH (OBS,FLD) AXES IN OUTER LOOPS

    // The net CT obs required for the MS obs according to the obsmap
    //  (in principle, this may contain CT obs ids that aren't available)
    Vector<Int> reqCTobs=cls.obsmap.ctids(reqMSobs);

    //    cout << "reqCTobs = " << reqCTobs << endl;

    // For each required CT obs (and thus the MS obs ids requiring it)
    NewCalTable obsselCT(ct_);
    for (uInt iCTobs=0;iCTobs<reqCTobs.nelements();++iCTobs) {
      Int& thisCTobs=reqCTobs(iCTobs);

      // Apply thisCTobs selection to the CT
      //   (only if a meaningful obsid is specified)
      if (thisCTobs!=-1)
	this->selectOnCT(obsselCT,ct_,String::toString(thisCTobs),"","","");

      // The MS obss to be calibrated by thisCTobs (limited by the req superset)
      //  (could be [-1], which means all)
      Vector<Int> theseMSobs=cls.obsmap.msids(thisCTobs,reqMSobs);
      if (theseMSobs.nelements()==1 && theseMSobs[0]<0)
	theseMSobs.reference(reqMSobs);
      //      cout << " thisCTobs = " << thisCTobs << ": theseMSobs = " << theseMSobs << endl;

      // Apply theseMSobs selection to the MS
      // TBD:  reqMSfld = ...

      // The net CT fld required for the MS fld according to the fldmap
      //  (in principle, this may contain CT fld ids that aren't available)
      // NB: currently all [-1] or singles; "some" is TBD
      Vector<Int> reqCTfld=cls.fldmap.ctids(reqMSfld);

      //      cout << " reqCTfld = " << reqCTfld << endl;


      // For each required CT fld:
      NewCalTable fldselCT(obsselCT);
      for (uInt iCTfld=0;iCTfld<reqCTfld.nelements();++iCTfld) {
	Int& thisCTfld=reqCTfld(iCTfld);   // TBD: generalize to multiple fields?

	// Apply thisCTfld selection to the CT
	if (thisCTfld!=-1)
	  this->selectOnCT(fldselCT,obsselCT,"",String::toString(thisCTfld),"","");


	// The MS flds to be calibrated by thisCTfld
	//  (could be [-1], which means all)
	Vector<int> theseMSfld=cls.fldmap.msids(thisCTfld,reqMSfld);
	if (theseMSfld.nelements()==1 && theseMSfld[0]<0)
	  theseMSfld.reference(reqMSfld);


	//	cout << "  thisCTfld = " << thisCTfld << ": theseMSfld = " << theseMSfld << endl;

	// Apply theseMSfld selection to the MS
	// TBD: reqMSspw = ...

	//  ...AND HARDWARE AXES (SPW,ANT) IN INNER LOOPS

	// For each required _MS_ spw:
	NewCalTable spwselCT(fldselCT);
	for (uInt iMSspw=0;iMSspw<reqMSspw.nelements();++iMSspw) {
	  Int& thisMSspw=reqMSspw(iMSspw);
	  Int thisCTspw=cls.spwmap(thisMSspw);
	  if (thisCTspw<0) thisCTspw=thisMSspw; // MUST BE DEFINITE!

	  // Apply thisCTspw selection to CT
	  this->selectOnCT(spwselCT,fldselCT,"","",String::toString(thisCTspw),"");

	  // Create time-dep interp result container
	  CTCalPatchKey iclTres(icls,thisCTobs,thisCTfld,thisMSspw,-1);
	  clTres_[iclTres]=CLPPResult(nPar_,nFPar_,nChanIn_[thisCTspw],nMSElem_);

	  NewCalTable antselCT(spwselCT);
	  for (uInt iMSant=0;iMSant<reqMSant.nelements();++iMSant) {
	    Int& thisMSant=reqMSant(iMSant);
	    Int thisCTant=cls.antmap(thisMSant);
	    if (thisCTant<0) thisCTant=thisMSant;

	    // Apply thisCTant selection to CT
	    this->selectOnCT(antselCT,spwselCT,"","","",String::toString(thisCTant));

	    //  (if null, warn and continue, or throw?)

	    // Make the Cal Interpolator (icls is the CL slice index):
	    CTCalPatchKey ici0(icls,thisCTobs,thisCTfld,thisCTspw,thisCTant);  // all CT indices
	    CTCalPatchKey ici1(icls,thisCTobs,thisCTfld,thisMSspw,thisMSant);  // spw,ant are MS indices
	    //   (NB: Must use thisMSspw,thisMSant above to avoid duplication in resolved spwmap,antmap)

	    if (ci_.count(ici1)<1) {
	      ciname_[ici1]=ici0.print()+" rows="+String::toString(antselCT.nrow());
	      Array<Float> r(clTres_[iclTres].result(thisMSant));
	      Array<Bool> rf(clTres_[iclTres].resultFlag(thisMSant));
	      ci_[ici1]=(*cttifactoryptr_)(antselCT,cls.tinterp,r,rf);
	      //	      cout << "Creating: CT("<<ici1.print() << ") --> CT(" << ici0.print() << ")" << endl;


	    }
	    else
	      throw(AipsError("Attempted duplicate CTCalPatchKey!"));

	    // Now distribute this CTTimeInterp1 instance to all relevant MS indices
	    for (uInt iMSobs=0;iMSobs<theseMSobs.nelements();++iMSobs) {
	      Int& thisMSobs=theseMSobs(iMSobs);
	      for (uInt iMSfld=0;iMSfld<theseMSfld.nelements();++iMSfld) {
		Int& thisMSfld=theseMSfld(iMSfld);
		for (uInt iMSint=0;iMSint<theseMSint.nelements();++iMSint) {
		  Int& thisMSint=theseMSint(iMSint);

		  MSCalPatchKey ims(thisMSobs,thisMSfld,thisMSint,thisMSspw,thisMSant);
		  if (msci_.count(ims)<1) {
		    msciname_[ims]=ciname_[ici1];
		    msci_[ims]=ci_[ici1];
		  }
		  else
		    throw(AipsError("Attempted duplicate MSCalPatchKey!"));

		  //		  cout << " Patching: MS(" << ims.print() << ") --> CT(" << ici0.print() << ")" << endl;

		  // Link these obs,fld,ant,spw to the correct results object
		  //  (as a group over antennas; should move this out of ant loop, really)
		  if (iMSant==0) {
		    MSCalPatchKey imsgroup(thisMSobs,thisMSfld,thisMSint,thisMSspw,-1);
		    msTres_[imsgroup]=clTres_[iclTres];
		    msFres_[imsgroup]=CLPPResult(); // this will be resized on-demand
		    ctspw_[imsgroup]=thisCTspw;
		    finterp_[imsgroup]=cls.finterp;
		  }
		} // iMSint
	      } // iMSfld
	    } // iMSobs

	  } // iMSant
	} // iMSspw
      } // iCTfld
    } // iCTobs

  } // icls

} // ctor

// (MS sensitive)
CLPatchPanel::CLPatchPanel(const String& ctname,
			   const MeasurementSet& ms,
			   const Record& callib,
			   VisCalEnum::MatrixType mtype,
			   Int nPar,
			   const CTTIFactoryPtr cttifactoryptr) :
  ct_(NewCalTable::createCT(ctname,Table::Old,Table::Memory)),
  ctasms_(), // null, in this context
  ms_(ms),
  mtype_(mtype),
  isCmplx_(false),
  nPar_(nPar),
  nFPar_(nPar),

  nChanIn_(),
  freqIn_(),
  refFreqIn_(),

  nMSObs_(ms_.observation().nrow()),
  nMSFld_(ms_.field().nrow()),
  nMSSpw_(ms_.spectralWindow().nrow()),
  nMSAnt_(ms_.antenna().nrow()),
  nMSElem_(0),  // set non-trivially in ctor body
  nCTObs_(ct_.observation().nrow()),
  nCTFld_(ct_.field().nrow()),
  nCTSpw_(ct_.spectralWindow().nrow()),
  nCTAnt_(ct_.antenna().nrow()),
  nCTElem_(0), // set non-trivially in ctor body
  lastresadd_(nMSSpw_,NULL),
  cttifactoryptr_(cttifactoryptr)

  //  elemMap_(),
  //  conjTab_(),

{
  if (CTPATCHPANELVERB) cout << "CLPatchPanel::CLPatchPanel(<w/MS>)" << endl;

  //  ia1dmethod_=ftype(freqType_);
  //  cout << "ia1dmethod_ = " << ia1dmethod_ << endl;

    
  switch(mtype_) {
  case VisCalEnum::GLOBAL: {

    throw(AipsError("CLPatchPanel::ctor: No non-Mueller/Jones yet."));

    nCTElem_=1;
    nMSElem_=1;
    break;
  }
  case VisCalEnum::MUELLER: {
    nCTElem_=nCTAnt_*(nCTAnt_+1)/2;
    nMSElem_=nMSAnt_*(nMSAnt_+1)/2;
    break;
  }
  case VisCalEnum::JONES: {
    nCTElem_=nCTAnt_;
    nMSElem_=nMSAnt_;
    break;
  }
  }

  // How many _Float_ parameters?
  isCmplx_=(ct_.keywordSet().asString("ParType")=="Complex");
  if (isCmplx_) // Complex input
    nFPar_*=2;  // interpolating 2X as many Float values

  // Set CT channel/freq info
  CTSpWindowColumns ctspw(ct_.spectralWindow());
  ctspw.numChan().getColumn(nChanIn_);
  freqIn_.resize(nCTSpw_);
  for (uInt iCTspw=0;iCTspw<ctspw.nrow();++iCTspw)
    ctspw.chanFreq().get(iCTspw,freqIn_(iCTspw),true);
  ctspw.refFrequency().getColumn(refFreqIn_);

  // Initialize maps
  //  (ctids, msids, result arrays)
  // TBD

  //void CLPatchPanel::unpackCLinstance(CalLibSlice cls) {

  // Loop over callib slices
  //  (a callib slice is one MS selection and interp setup for the present caltable)
  uInt nkey=callib.nfields();
  Int icls=-1;
  for (uInt ikey=0;ikey<nkey;++ikey) {

    // CalTable name might be needed below (eg, for log messages)
    String ctname=Path(ct_.getPartNames()[0]).baseName().before(".tempMemCalTable");

    // Ignore non-Record members in the callib
    if (callib.type(ikey)!=TpRecord)
      continue;

    ++icls;

    // The current callib slice
    CalLibSlice cls(callib.asRecord(ikey),ms_,ct_);


    // Reference to the cal table that we'll use below
    NewCalTable ct0(ct_);
    if (cls.extfldsel!="") {
      // Select on the reference table
      try {
	this->selectOnCT(ct0,ct_,"",cls.extfldsel,"","");
      } catch ( MSSelectionError err ) {
	// Selection failed somehow:
	throw(AipsError("Problem selecting for multi-field field mapping ('"+cls.extfldsel+"') in caltable="+ctname+":  "+err.getMesg()));
      }

    }

    // Apply callib instance MS selection to the incoming (selected MS)
    MeasurementSet clsms(ms_);

    // Trap Null selection exceptions, as they are not needed downstream
    try {
      this->selectOnMS(clsms,ms_,cls.obs,cls.fld,cls.ent,cls.spw,"");
    }
    catch ( MSSelectionNullSelection x ) {

      // Warn in logger that this slice doesn't match anything
      //  in the selected MS
      //String msname=Path(ms_.tableName()).baseName();
      String msname=Path(ms_.getPartNames()[0]).baseName();
      logsink_ << LogIO::WARN
	       << ".   The following callib entry matches no data" << endl
	       << ".   in the selected MS ("+msname+") and will be ignored:" << endl
	       << ".   " << icls << ":" << endl
	       << cls.state() << LogIO::POST;

      // Just jump to next slice (cal maps for this MS selection unneeded)
      continue;
    }

    // Report this relevant cal lib slice to the logger
    logsink_ << LogIO::NORMAL << ".   " << icls << ":" << endl
	     << cls.state() << LogIO::POST;

    // What MS indices will be calibrated by this CL instance?

    //  TBD: we will want to calculate these within the loops below
    //  so that per-obs and per-fld subsets will be correctly resolved
    //  (and, e.g., nearest field can be obs-dep, etc.)
    //  (gmoellen 2018/02/05:  still true?  Doesn't selection above
    //    reduce requirements to the union of obs/fld/intent accounted
    //    for by the current cl?)

    // OBS Ids in selected MS to be calibrated by this cl instance
    Vector<Int> reqMSobs(1,-1); // assume all, indescriminately
    if (cls.obs.length()>0)
      // if CL is obs-specific, we must not be indescriminate
      reqMSobs.reference(this->getMSuniqueIds(clsms,"obs"));
    //cout << "reqMSobs = " << reqMSobs << endl;

    // FIELD Ids in selected MS to be calibrated by this cl instance
    Vector<Int> reqMSfld(1,-1); // assume all, indescriminately
    if (cls.fld.length()>0)  // if selected, maybe we only need a subset
      // if CL is fld-specific, we must not be indescriminate
      reqMSfld.reference(this->getMSuniqueIds(clsms,"fld"));
    //cout << "reqMSfld = " << reqMSfld << endl;

    // INTENT Ids in selected MS to be calibrated by this cl instance
    Vector<Int> reqMSint(1,-1); // assume all
    if (cls.ent.length()>0)
      reqMSint.reference(this->getMSuniqueIds(clsms,"intent"));
    //cout << "reqMSint = " << reqMSint << endl;
    Vector<Int> theseMSint(reqMSint);

    // SPW Ids in selected MS to be calibrated by this cl instance
    // We are never indescriminate about spw
    Vector<Int> reqMSspw(this->getMSuniqueIds(clsms,"spw"));
    //cout << "reqMSspw = " << reqMSspw << endl;

    // ANTENNA in selected MS to be calibrated by this cl instance
    // We are never indescriminate about ant
    //  (For now, we will do all MS ants)
    Vector<Int> reqMSant(nMSAnt_);
    indgen(reqMSant);
    //cout << "reqMSant = " << reqMSant << endl;

    // SLICE CalTable by OBS, FIELD, SPW, ANT, and map to
    //   the corresponding MS indicies

    // WE DO TIME-ISH (OBS,FLD) AXES IN OUTER LOOPS

    NewCalTable obsselCT(ct0);

    // The net CT obs required for the MS obs according to the obsmap
    //   We will create separate interpolator groups for each
    Vector<Int> reqCTobs=cls.obsmap.ctids(reqMSobs);
    //cout << "reqCTobs = " << reqCTobs << endl;
    // For each required CT obs (and thus the MS obs ids requiring it)
    for (uInt iCTobs=0;iCTobs<reqCTobs.nelements();++iCTobs) {
      Int& thisCTobs=reqCTobs(iCTobs);

      // The MS OBSs (subset of reqMSobs) to be calibrated by thisCTobs
      //  (could be [-1], which means all)
      Vector<Int> theseMSobs=cls.obsmap.msids(thisCTobs,reqMSobs);
      if (theseMSobs.nelements()==1 && theseMSobs[0]<0)
	theseMSobs.reference(reqMSobs);
      //cout << " thisCTobs = " << thisCTobs << ": theseMSobs = " << theseMSobs << endl;

      // Apply thisCTobs selection to the CT
      //   (only if a meaningful obsid is specified)
      try {
	if (thisCTobs!=-1) 
	  this->selectOnCT(obsselCT,ct0,String::toString(thisCTobs),"","","");
      }
      catch (...) {  //  MSSelectionNullSelection x ) {

	// Required CT obs does not exist in the caltable
	recordBadMSIndices(theseMSobs,reqMSfld,reqMSint,Vector<Int>(1,-1));  // all spws
	continue;  // jump to next CT obs
      }


      // The net CT fld required for the MS fld according to the fldmap
      // NB: currently all [-1] or singles; "some" is TBD
      Vector<Int> reqCTfld=cls.fldmap.ctids(reqMSfld);
      //cout << " reqCTfld = " << reqCTfld << endl;

      // For each required CT fld:
      NewCalTable fldselCT(obsselCT);
      for (uInt iCTfld=0;iCTfld<reqCTfld.nelements();++iCTfld) {
	Int& thisCTfld=reqCTfld(iCTfld);   // TBD: generalize to multiple fields?

	// The MS flds to be calibrated by thisCTfld
	//  (could be [-1], which means all)
	Vector<int> theseMSfld=cls.fldmap.msids(thisCTfld,reqMSfld);
	if (theseMSfld.nelements()==1 && theseMSfld[0]<0)
	  theseMSfld.reference(reqMSfld);

	//cout << "  thisCTfld = " << thisCTfld << ": theseMSfld = " << theseMSfld << endl;

	// Apply thisCTfld selection to the CT
	try {
	  if (thisCTfld!=-1)
	    this->selectOnCT(fldselCT,obsselCT,"",String::toString(thisCTfld),"","");
	}
	catch (...) {  //  MSSelectionNullSelection x ) {

	  // Required CT fld does not exist in the caltable (for current CT obs)
	  recordBadMSIndices(theseMSobs,theseMSfld,reqMSint,Vector<Int>(1,-1));  // all spws
	  continue;  // jump to next fld
	}

	//  ...AND HARDWARE AXES (SPW,ANT) IN INNER LOOPS

	// For each required _MS_ spw:
	NewCalTable spwselCT(fldselCT);
	for (uInt iMSspw=0;iMSspw<reqMSspw.nelements();++iMSspw) {
	  Int& thisMSspw=reqMSspw(iMSspw);
	  Int thisCTspw=cls.spwmap(thisMSspw);
	  if (thisCTspw<0) thisCTspw=thisMSspw; // MUST BE DEFINITE!

	  //cout << "   thisCTspw=" << thisCTspw << "--> thisMSspw="<<thisMSspw<<endl;

	  // Apply thisCTspw selection to CT
	  try {
	    this->selectOnCT(spwselCT,fldselCT,"","",String::toString(thisCTspw),"");
	  }
	  catch (...) {  //  MSSelectionNullSelection x ) {

	    // Required CT spw does not exist in the caltable (for current CT obs, fld)
	    recordBadMSIndices(theseMSobs,theseMSfld,reqMSint,Vector<Int>(1,thisMSspw));  // current spw
	    continue;  // jump to next spw
	  }

	  // If this selection fails (zero rows), and exception is thrown.
	  //  What is the state of antselCT?
	  //       Is it still the unselected-upon spwselCT?
	  //       Or is an empty table?



	  // Create time-dep interp result container
	  //  Indexed by CTobs, CTfld, MSspw (for all antennas)
	  CTCalPatchKey iclTres(icls,thisCTobs,thisCTfld,thisMSspw,-1);
	  clTres_[iclTres]=CLPPResult(nPar_,nFPar_,nChanIn_[thisCTspw],nMSElem_);

	  NewCalTable antselCT(spwselCT);
    Bool doLinkResults(true);  // initialize true, so it will happen if relevant code reached
	  for (uInt iMSant=0;iMSant<reqMSant.nelements();++iMSant) {
	    Int& thisMSant=reqMSant(iMSant);
	    Int thisCTant=cls.antmap(thisMSant);
	    if (thisCTant<0) thisCTant=thisMSant;

	    // Apply thisCTant selection to CT
	    try {
	      this->selectOnCT(antselCT,spwselCT,"","","",String::toString(thisCTant));
	    }
	    catch ( MSSelectionNullSelection x ) {
	      // Log a warning about the missing antenna
	      logsink_ << LogIO::WARN << "     Found no calibration for MS ant Id=" << thisMSant << " (CT ant Id=" << thisCTant << ")"
		       << " in MS spw Id=" << thisMSspw << " (CT spw Id=" << thisCTspw << ") (" << ctname << ")"
		       << LogIO::POST;
	      // Step to next antenna
	      continue;
	    }

	    // Make the Cal Interpolator (icls is the CL slice index):
	    CTCalPatchKey ici0(icls,thisCTobs,thisCTfld,thisCTspw,thisCTant);  // all CT indices
	    CTCalPatchKey ici1(icls,thisCTobs,thisCTfld,thisMSspw,thisMSant);  // spw,ant are MS indices
	    //   (NB: Must use thisMSspw,thisMSant above to avoid duplication in resolved spwmap,antmap)

	    if (ci_.count(ici1)<1) {
	      ciname_[ici1]=ici0.print()+" rows="+String::toString(antselCT.nrow());
	      Array<Float> r(clTres_[iclTres].result(thisMSant));
	      Array<Bool> rf(clTres_[iclTres].resultFlag(thisMSant));
	      ci_[ici1]=(*cttifactoryptr_)(antselCT,cls.tinterp,r,rf);
	      //if (iMSant==0) cout << "   Creating: CT("<<ici1.print() << ") --> CT(" << ici0.print() << ")  (all antennas)" << endl;
	    }
	    else
	      throw(AipsError("Attempted duplicate CTCalPatchKey!"));

	    // Now distribute this CTTimeInterp1 instance to all relevant MS indices
	    for (uInt iMSobs=0;iMSobs<theseMSobs.nelements();++iMSobs) {
	      Int& thisMSobs=theseMSobs(iMSobs);
	      for (uInt iMSfld=0;iMSfld<theseMSfld.nelements();++iMSfld) {
		Int& thisMSfld=theseMSfld(iMSfld);
		for (uInt iMSint=0;iMSint<theseMSint.nelements();++iMSint) {
		  Int& thisMSint=theseMSint(iMSint);

		  MSCalPatchKey ims(thisMSobs,thisMSfld,thisMSint,thisMSspw,thisMSant);
		  if (msci_.count(ims)<1) {
		    msciname_[ims]=ciname_[ici1];
		    msci_[ims]=ci_[ici1];
		  }
		  else
		    throw(AipsError("Attempted duplicate MSCalPatchKey!"));

		  //if (doLinkResults)
		  //  cout << " Patching: MS(" << ims.print() << ") --> CT(" << ici0.print() << ")" << endl;

		  // Link these obs,fld,ant,spw to the correct results object
		  //  (as a group over antennas; should move this out of ant loop, really)
      if (doLinkResults) {
		    MSCalPatchKey imsgroup(thisMSobs,thisMSfld,thisMSint,thisMSspw,-1);
		    msTres_[imsgroup]=clTres_[iclTres];
		    msFres_[imsgroup]=CLPPResult(); // this will be resized on-demand
		    ctspw_[imsgroup]=thisCTspw;
		    finterp_[imsgroup]=cls.finterp;
		  }
		} // iMSint
	      } // iMSfld
	    } // iMSobs
        doLinkResults = False; // Don't do it again
	  } // iMSant
	} // iMSspw
      } // iCTfld
    } // iCTobs


  } // icls



} // ctor

void CLPatchPanel::recordBadMSIndices(const Vector<Int>& obs, const Vector<Int>& fld,
				      const Vector<Int>& ent, const Vector<Int>& spw) {


  // Record bad _MS_ indices
  for (uInt iobs=0;iobs<obs.nelements();++iobs) {
    for (uInt ifld=0;ifld<fld.nelements();++ifld) {
      for (uInt ient=0;ient<ent.nelements();++ient) {
	for (uInt ispw=0;ispw<spw.nelements();++ispw) {

	  MSCalPatchKey ims(obs[iobs],fld[ifld],ent[ient],spw[ispw],-1);  // All ants
	  if (badmsciname_.count(ims)<1) {
	    badmsciname_[ims]=ims.print();
	    //cout << "   Bad MS indices: " << ims.print() << endl;
	  }
	}
      }
    }
  }
  return;
}


void CLPatchPanel::selectOnCT(NewCalTable& ctout,const NewCalTable& ctin,
			      const String& obs, const String& fld,
			      const String& spw, const String& ant1) {

  String taql("");
  if (ant1.length()>0)
    taql="ANTENNA1=="+ant1;

  //  cout << "taql = >>" << taql << "<<" << endl;

  // Forward to generic method (sans intent)
  CTInterface cti(ctin);
  this->selectOnCTorMS(ctout,cti,obs,fld,"",spw,"",taql);

}

void CLPatchPanel::selectOnMS(MeasurementSet& msout,const MeasurementSet& msin,
			      const String& obs, const String& fld,
			      const String& ent,
			      const String& spw, const String& ant) {

  // Forward to generic method
  MSInterface msi(msin);
  this->selectOnCTorMS(msout,msi,obs,fld,ent,spw,ant,"");

}
void CLPatchPanel::selectOnCTorMS(Table& ctout,MSSelectableTable& msst,
				  const String& obs, const String& fld,
				  const String& ent,
				  const String& spw, const String& ant,
				  const String& taql) {

  MSSelection mss;
  if (obs.length()>0)
    mss.setObservationExpr(obs);
  if (fld.length()>0)
    mss.setFieldExpr(fld);
  if (ent.length()>0)
    mss.setStateExpr(ent);
  if (spw.length()>0)
    mss.setSpwExpr(spw);
  if (ant.length()>0)
    mss.setAntennaExpr(ant); // this will not behave as required for Jones caltables (its bl-based selection!)
  if (taql.length()>0)
    mss.setTaQLExpr(taql);


  TableExprNode ten=mss.toTableExprNode(&msst);
  getSelectedTable(ctout,*(msst.table()),ten,"");

}

Vector<Int> CLPatchPanel::getCLuniqueIds(NewCalTable& ct, String vcol) {

  ROCTMainColumns ctmc(ct);

  // Extract the requested column as a Vector
  Vector<Int> colv;
  if (vcol=="obs")
    ctmc.obsId().getColumn(colv);
  else if (vcol=="fld")
    ctmc.fieldId().getColumn(colv);
  else if (vcol=="spw")
    ctmc.spwId().getColumn(colv);
  else
    throw(AipsError("Unsupported column in CLPatchPanel::getCLuniqueIds"));

  // Reduce to a unique list
  uInt nuniq=genSort(colv,Sort::Ascending,(Sort::QuickSort | Sort::NoDuplicates));
  colv.resize(nuniq,true);

  return colv;

}

Vector<Int> CLPatchPanel::getMSuniqueIds(MeasurementSet& ms, String which) {

  MSColumns msc(ms);

  // Extract the requested column as a Vector
  Vector<Int> colv;
  if (which=="obs")
    msc.observationId().getColumn(colv);
  else if (which=="fld")
    msc.fieldId().getColumn(colv);
  else if (which=="intent")
    msc.stateId().getColumn(colv);
  else if (which=="spw")
    msc.dataDescId().getColumn(colv);  // these are actually ddids!
  else
    throw(AipsError("Unsupported column in CLPatchPanel::getCLuniqueIds"));

  // Reduce to a unique list
  uInt nuniq=genSort(colv,Sort::Ascending,(Sort::QuickSort | Sort::NoDuplicates));
  colv.resize(nuniq,true);

  // In case of spw, translate from ddid
  if (which=="spw") {
    Vector<Int> spwids;
    msc.dataDescription().spectralWindowId().getColumn(spwids);
    for (uInt i=0;i<colv.nelements();++i)
      colv[i]=spwids[colv[i]];
  }

  // return the value
  return colv;

}



// Destructor
CLPatchPanel::~CLPatchPanel() {

  if (CTPATCHPANELVERB) cout << "CLPatchPanel::~CLPatchPanel()" << endl;

  // Delete the atomic interpolators
  for (std::map<CTCalPatchKey,CTTimeInterp1*>::iterator it=ci_.begin(); it!=ci_.end(); ++it)
    delete it->second;

}

// Is specific calibration explicitly available for a obs,fld,intent,spw,ant combination?
Bool CLPatchPanel::calAvailable(casacore::Int msobs, casacore::Int msfld, casacore::Int msent,
				casacore::Int msspw, casacore::Int msant) {

  const MSCalPatchKey key(msobs,msfld,msent,msspw,msant);

  Bool avail=msTres_.count(key)>0;

  //  if (!avail) {
  //    cout << Path(ct_.tableName()).baseName().before(".tempMem") << " stepped over: " << key.print() << endl;
  //  }

  return avail;

}

// The specified MS indices are "OK" if not recorded as expected-but-absent as per
//   the callibrary specification.  Expected-but-absent occurs when a CL entry
//   indicates (even implicitly) that the MS index combination is supported but
//   the required CalTable indices (via *map params) are not actually available
//   in the CalTable, e.g., a missing spw.
//  Such data cannot be calibrated and this CL cannot be agnostic about it...
// Note that MSIndicesOK can return true for MS index combinations for which
//  calibration is not actually available, if the CL entry does not purport
//  to support calibrating them.  In such cases, calAvailable() returns false, and
//  this CL is agnostic w.r.t. such data and lets it pass thru unchanged.
//  Returns TRUE when the specified MS indices are calibrateable or passable.
Bool CLPatchPanel::MSIndicesOK(casacore::Int msobs, casacore::Int msfld, casacore::Int msent,
			       casacore::Int msspw, casacore::Int msant) {

  const MSCalPatchKey key(msobs,msfld,msent,msspw,msant);
  Bool bad=badmsciname_.count(key)>0;
  //  if (bad) {
  //    cout << Path(ct_.tableName()).baseName().before(".tempMem") << " should but can't calibrate: " << key.print() << endl;
  // }

  // Return TRUE if NOT bad
  return !bad;

}

Bool CLPatchPanel::interpolate(Cube<Complex>& resultC, Cube<Bool>& resFlag,
			       Int msobs, Int msfld, Int msent, Int msspw,
			       Double time, Double freq) {

  Bool newcal(false);

  // resultC and resFlag will be unchanged if newcal remains false
  Cube<Float> f; // temporary to reference Float interpolation result
  newcal=interpolate(f,resFlag,msobs,msfld,msent,msspw,time,freq);
  if (newcal)
    // convert to complex and have resultC take over ownership
    resultC.reference(RIorAPArray(f).c());

  return newcal;  // If T, calling scope should act nontrivially, if necessary

}

Bool CLPatchPanel::interpolate(Cube<Complex>& resultC, Cube<Bool>& resFlag,
			       Int msobs, Int msfld, Int msent, Int msspw,
			       Double time, const Vector<Double>& freq) {

  Bool newcal(false);

  // resultC and resFlag will be unchanged if newcal remains false
  Cube<Float> f; // temporary to reference Float interpolation result
  newcal=interpolate(f,resFlag,msobs,msfld,msent,msspw,time,freq);

  if (newcal)
    // convert to complex and have resultC take over ownership
    resultC.reference(RIorAPArray(f).c());

  return newcal;  // If T, calling scope should act nontrivially, if necessary

}


Bool CLPatchPanel::interpolate(Cube<Float>& resultR, Cube<Bool>& resFlag,
			       Int msobs, Int msfld, Int msent, Int msspw,
			       Double time, Double freq) {

  if (CTPATCHPANELVERB) cout << "CLPatchPanel::interpolate(...)" << endl;

  // NB: resultR and resFlag will be unchanged if newcal remains false
  Bool newcal(false);

  // Suppled arrays reference the result (if available)
  MSCalPatchKey ires(msobs,msfld,msent,msspw,-1);

  // Trap lack of available calibration for requested obs,fld,intent,spw
  if (msTres_.count(ires)==0) {
    throw(AipsError("No calibration arranged for "+ires.print()+
		    " in callib for caltable="+
		    Path(ct_.tableName()).baseName().before(".tempMemCalTable") ));
  }

  // If result_ is at a new address (cf last time, this spw), treat as new
  //  TBD: do this is a less obscure way... (e.g., invent the CLCalGroup(nant))
  if (lastresadd_(msspw)!=msTres_[ires].result_.data())
    newcal=true;
  lastresadd_(msspw)=msTres_[ires].result_.data();

  // Loop over _output_ elements
  for (Int iMSElem=0;iMSElem<nMSElem_;++iMSElem) {
    // Call fully _patched_ time-interpolator, keeping track of 'newness'
    //  fills ctTres_ implicitly
    MSCalPatchKey ims(msobs,msfld,msent,msspw,iMSElem);
    if (msci_.count(ims)>0) {
      if (freq>0.0)
	newcal|=msci_[ims]->interpolate(time,freq);
      else
	newcal|=msci_[ims]->interpolate(time);
    }
  }

  if (newcal) {
    resultR.reference(msTres_[ires].result_);
    resFlag.reference(msTres_[ires].resultFlag_);
  }

  return newcal; // If true, calling scope should act
}


Bool CLPatchPanel::interpolate(Cube<Float>& resultR, Cube<Bool>& resFlag,
			       Int msobs, Int msfld, Int msent, Int msspw,
			       Double time, const Vector<Double>& freq) {

  if (CTPATCHPANELVERB) cout << "CLPatchPanel::interpolate(Cube<F>,...,Vector<D>)" << endl;

  // NB: resultR and resFlag will be unchanged if newcal remains false
  Bool newcal(false);

  // Suppled arrays reference the result (if available)
  MSCalPatchKey ires(msobs,msfld,msent,msspw,-1);

  // Trap lack of available calibration for requested obs,fld,intent,spw
  if (msTres_.count(ires)==0) {
    throw(AipsError("No calibration arranged for "+ires.print()+
		    " in callib for caltable="+
		    Path(ct_.tableName()).baseName().before(".tempMemCalTable") ));
  }

  // If result_ is at a new address (cf last time, this msspw), treat as new
  //  TBD: do this is a less obscure way...
  if (lastresadd_(msspw)!=msTres_[ires].result_.data())
    newcal=true;
  lastresadd_(msspw)=msTres_[ires].result_.data();

  // Sometimes we need to force the freq interp, even if the time-interp isn't new
  Bool forceFinterp=false || newcal;

  // The follow occurs unnecessarily in mosaics when newcal=false (i.e., when resampleInFreq won't be called below)
  uInt nMSChan=freq.nelements();    // The number of requested channels
  if (msFres_.count(ires)>0) {
    if (msFres_[ires].result_.nelements()==0 ||
	msFres_[ires].result(0).ncolumn()!=nMSChan) {
      msFres_[ires].resize(nPar_,nFPar_,nMSChan,nMSElem_);
    }
  }
  else
    throw(AipsError("Error finding msFres_ in CLPatchPanel::interpolate(C<F>,...,V<D>)"));

  // Loop over _output_ antennas
  for (Int iMSElem=0;iMSElem<nMSElem_;++iMSElem) {
    // Call time interpolation calculation; resample in freq if new
    //   (fills msTRes_ implicitly)

    MSCalPatchKey ims(msobs,msfld,msent,msspw,iMSElem);
    if (msci_.count(ims)>0) {
      if (msci_[ims]->interpolate(time) || forceFinterp) {

	// Resample in frequency
	Matrix<Float>   fR( msFres_[ires].result(iMSElem) );
	Matrix<Bool> fRflg( msFres_[ires].resultFlag(iMSElem) );
	Matrix<Float>   tR( msTres_[ires].result(iMSElem) );
	Matrix<Bool> tRflg( msTres_[ires].resultFlag(iMSElem) );
	resampleInFreq(fR,fRflg,freq,tR,tRflg,freqIn_(ctspw_[ires]),finterp_[ires]);

	// Calibration is new
	newcal=true;
      }
    }
  }

  if (newcal) {
    // Supplied arrays to reference the result
    resultR.reference(msFres_[ires].result_);
    resFlag.reference(msFres_[ires].resultFlag_);
  }

  return newcal;
}

Bool CLPatchPanel::getTresult(Cube<Float>& resultR, Cube<Bool>& resFlag,
			      Int obs, Int fld, Int ent, Int spw) {

  MSCalPatchKey mskey(obs,fld,ent,spw,-1);

  if (msTres_.count(mskey)==0)
    throw(AipsError("No calibration arranged for "+mskey.print()+
		    " in callib for caltable="+
		    Path(ct_.tableName()).baseName().before(".tempMemCalTable") ));

  // Reference the requested Cube
  resultR.reference(msTres_[mskey].result_);
  resFlag.reference(msTres_[mskey].resultFlag_);

  return true;

}




void CLPatchPanel::listmappings() {

  cout << "CalTable: " << ct_.tableName() << endl;
  cout << "There are " << ci_.size() << " cal interpolation engines." <<  endl;
  cout << "There are " << msci_.size() << " unique MS id combinations mapped to them:" <<  endl;
  for (std::map<MSCalPatchKey,String>::iterator it=msciname_.begin(); it!=msciname_.end(); ++it)
    cout << "MS (" << it->first.print() << ") --> CL (" << it->second << ")" << endl;

  cout << endl << "There are " << badmsciname_.size() << " expected but ABSENT MS id combinations (all ants):" << endl;
  for (std::map<MSCalPatchKey,String>::iterator it=badmsciname_.begin(); it!=badmsciname_.end(); ++it)
    cout << "MS (" << it->first.print() << ")" << endl;

  cout << endl;

}




  // Report state
void CLPatchPanel::state() {

  if (CTPATCHPANELVERB) cout << "CLPatchPanel::state()" << endl;

  cout << "-state--------" << endl;
  cout << " ct_      = " << ct_.tableName() << endl;
  cout << boolalpha;
  cout << " isCmplx_ = " << isCmplx_ << endl;
  cout << " nPar_    = " << nPar_ << endl;
  cout << " nFPar_   = " << nFPar_ << endl;
  cout << " nMSObs_  = " << nMSObs_ << endl;
  cout << " nMSFld_  = " << nMSFld_ << endl;
  cout << " nMSSpw_  = " << nMSSpw_ << endl;
  cout << " nMSAnt_  = " << nMSAnt_ << endl;
  cout << " nMSElem_ = " << nMSElem_ << endl;
  cout << " nCTObs_  = " << nCTObs_ << endl;
  cout << " nCTFld_  = " << nCTFld_ << endl;
  cout << " nCTSpw_  = " << nCTSpw_ << endl;
  cout << " nCTAnt_  = " << nCTAnt_ << endl;
  cout << " nCTElem_ = " << nCTElem_ << endl;

  //  cout << " timeType_ = " << timeType_ << endl;
  //  cout << " freqType_ = " << freqType_ << endl;
}

// Resample in frequency
void CLPatchPanel::resampleInFreq(Matrix<Float>& fres,Matrix<Bool>& fflg,const Vector<Double>& fout,
				  Matrix<Float>& tres,Matrix<Bool>& tflg,const Vector<Double>& fin,
				  String finterp) {

  if (CTPATCHPANELVERB) cout << "  CLPatchPanel::resampleInFreq(...)" << endl;

  // if no good solutions coming in, return flagged
  if (nfalse(tflg)==0) {
    fflg.set(true);
    return;
  }

  Int flparmod=nFPar_/nPar_;    // for indexing the flag Matrices on the par axis

  Bool unWrapPhase=flparmod>1;

  //  cout << "nFPar_,nPar_,flparmod = " << nFPar_ << "," << nPar_ << "," << flparmod << endl;

  fres=0.0;

  for (Int ifpar=0;ifpar<nFPar_;++ifpar) {

    // Slice by par (each Matrix row)
    Vector<Float> fresi(fres.row(ifpar)), tresi(tres.row(ifpar));
    Vector<Bool> fflgi(fflg.row(ifpar/flparmod)), tflgi(tflg.row(ifpar/flparmod));

    // Mask time result by flags
    Vector<Double> mfin=fin(!tflgi).getCompressedArray();

    if (mfin.nelements()==0) {
      //   cout << ifpar << " All chans flagged!" << endl;
      // Everything flagged this par
      //  Just flag, zero and go on to the next one
      fflgi.set(true);
      fresi.set(0.0);
      continue;
    }

    mfin/=1.0e9; // in GHz
    Vector<Float> mtresi=tresi(!tflgi).getCompressedArray();

    // Trap case of same in/out frequencies
    if (fout.nelements()==mfin.nelements() && allNear(fout,mfin,1.e-10)) {
      // Just copy
      fresi=mtresi;
      fflgi.set(false);  // none are flagged
      continue;
    }

    if (ifpar%2==1 && unWrapPhase) {
      for (uInt i=1;i<mtresi.nelements();++i) {
        while ( (mtresi(i)-mtresi(i-1))>C::pi ) mtresi(i)-=C::_2pi;
        while ( (mtresi(i)-mtresi(i-1))<-C::pi ) mtresi(i)+=C::_2pi;
      }
    }


    // TBD: ensure phases tracked on freq axis...

    // TBD: handle flags carefully!
    //      (i.e., flag gaps larger than user's "freach")
    // For now,just unset them
    fflgi.set(false);


    // Set flags carefully
    resampleFlagsInFreq(fflgi,fout,tflgi,fin,finterp);


    // Always use nearest on edges
    // TBD: trap cases where frequencies don't overlap at all
    //     (fout(hi)<mfin(0) || fout(lo)> mfin(ihi))..... already OK (lo>hi)?
    // TBD: optimize the following by forming Slices in the
    //     while loops and doing Array assignment once afterwords

    Int nfreq=fout.nelements();
    Int lo=0;
    Int hi=fresi.nelements()-1;
    Double inlo(mfin(0));
    Int ihi=mtresi.nelements()-1;
    Double inhi(mfin(ihi));

    // Handle 'nearest' extrapolation in sideband-dep way
    Bool inUSB(inhi>inlo);
    Bool outUSB(fout(hi)>fout(lo));
    if (inUSB) {
      if (outUSB) {
	while (lo<nfreq && fout(lo)<=inlo) fresi(lo++)=mtresi(0);
	while (hi>-1 && fout(hi)>=inhi) fresi(hi--)=mtresi(ihi);
      }
      else { // "outLSB"
	while (lo<nfreq && fout(lo)>=inhi) fresi(lo++)=mtresi(ihi);
	while (hi>-1 && fout(hi)<=inlo) fresi(hi--)=mtresi(0);
      }
    }
    else {  // "inLSB"
      if (outUSB) {
	while (lo<nfreq && fout(lo)<=inhi) fresi(lo++)=mtresi(ihi);
	while (hi>-1 && fout(hi)>=inlo) fresi(hi--)=mtresi(0);
      }
      else {  // "outLSB"
	while (lo<nfreq && fout(lo)>=inlo) fresi(lo++)=mtresi(0);
	while (hi>-1 && fout(hi)<=inhi) fresi(hi--)=mtresi(ihi);
      }
    }

    //    cout << "lo, hi = " << lo << ","<<hi << endl;

    if (lo>hi) continue; // Frequencies didn't overlap, nearest was used

    // Use InterpolateArray1D to fill in the middle
    IPosition blc(1,lo), trc(1,hi);
    Vector<Float> slfresi(fresi(blc,trc));
    Vector<Double> slfout(fout(blc,trc));

    InterpolateArray1D<Double,Float>::interpolate(slfresi,slfout,mfin,mtresi,this->ftype(finterp));

  }
}



void CLPatchPanel::resampleFlagsInFreq(Vector<Bool>& flgout,const Vector<Double>& fout,
				       Vector<Bool>& flgin,const Vector<Double>& fin,
				       String finterp) {

  //  cout << "resampleFlagsInFreq" << endl;

#define NEAREST InterpolateArray1D<Double,Float>::nearestNeighbour
#define LINEAR InterpolateArray1D<Double,Float>::linear
#define CUBIC InterpolateArray1D<Double,Float>::cubic
#define SPLINE InterpolateArray1D<Double,Float>::spline


  // Handle chan-dep flags
  if (finterp.contains("flag")) {

    InterpolateArray1D<Double,Float>::InterpolationMethod interpMeth=this->ftype(finterp);

    Vector<Double> finGHz=fin/1e9;

    // Determine implied mode-dep flags indexed by channel registration
    uInt nflg=flgin.nelements();
    Vector<Bool> flreg(nflg,false);
    switch (interpMeth) {
    case NEAREST: {
      // Just use input flags
      flreg.reference(flgin);
      break;
    }
    case LINEAR: {
      for (uInt i=0;i<nflg-1;++i)
        flreg[i]=(flgin[i] || flgin[i+1]);
      flreg[nflg-1]=flreg[nflg-2];
      break;
    }
    case CUBIC:
    case SPLINE: {
      for (uInt i=1;i<nflg-2;++i)
        flreg[i]=(flgin[i-1] || flgin[i] || flgin[i+1] || flgin[i+2]);
      flreg[0]=flreg[1];
      flreg[nflg-2]=flreg[nflg-3];
      flreg[nflg-1]=flreg[nflg-3];
      break;
    }
    }



    // Now step through requested chans, setting flags
    uInt ireg=0;
    uInt nflgout=flgout.nelements();
    for (uInt iflgout=0;iflgout<nflgout;++iflgout) {

      // Find nominal registration (the _index_ just left)
      Bool exact(false);
      ireg=binarySearch(exact,finGHz,fout(iflgout),nflg,0);

      // If registration is exact, assign verbatim
      // NB: the calibration value calculation occurs agnostically w.r.t. flags,
      //     so the calculated value should also match
      // TBD: Add "|| near(finGHz[ireg],fout(iflgout),1e-10) in case "exact" has
      //      precision issues?
      if (exact) {
	flgout[iflgout]=flgin[ireg];
	continue;
      }

      // Not exact, so carefully handle bracketing
      if (ireg>0)
        ireg-=1;
      ireg=min(ireg,nflg-1);

      //while (finGHz(ireg)<=fout(iflgout) && ireg<nflg-1) {
      //  ireg+=1;  // USB specific!
      //}
      //if (ireg>0 && finGHz(ireg)!=fout(iflgout)) --ireg;  // registration is one sample prior

      // refine registration by interp type
      switch (interpMeth) {
      case NEAREST: {
        // nearest might be forward sample
        if ( ireg<(nflg-1) &&
             abs(fout[iflgout]-finGHz[ireg])>abs(finGHz[ireg+1]-fout[iflgout]) )
          ireg+=1;
        break;
      }
      case LINEAR: {
        if (ireg==(nflg-1)) // need one more sample to the right
          ireg-=1;
        break;
      }
      case CUBIC:
      case SPLINE: {
        if (ireg==0) ireg+=1;  // need one more sample to the left
        if (ireg>(nflg-3)) ireg=nflg-3;  // need two more samples to the right
        break;
      }
      }

      // Assign effective flag
      flgout[iflgout]=flreg[ireg];

      /*
      cout << iflgout << " "
           << ireg << " "
           << flreg[ireg]
           << endl;
      */

    }

  }
  else
    // We are interp/extrap-olating gaps absolutely
    flgout.set(false);

}

InterpolateArray1D<Double,Float>::InterpolationMethod CLPatchPanel::ftype(String& strtype) {
  if (strtype.contains("nearest"))
    return InterpolateArray1D<Double,Float>::nearestNeighbour;
  if (strtype.contains("linear"))
    return InterpolateArray1D<Double,Float>::linear;
  if (strtype.contains("cubic"))
    return InterpolateArray1D<Double,Float>::cubic;
  if (strtype.contains("spline"))
    return InterpolateArray1D<Double,Float>::spline;

  //  cout << "Using linear for freq interpolation as last resort." << endl;
  return InterpolateArray1D<Double,Float>::linear;


}


} //# NAMESPACE CASA - END
