//# CTPatchedInterp.cc: Implementation of CTPatchedInterp.h
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

#include <synthesis/CalTables/CTPatchedInterp.h>
#include <synthesis/CalTables/CTIter.h>
#include <casacore/scimath/Mathematics/InterpolateArray1D.h>
#include <casacore/casa/OS/Path.h>
#include <casacore/casa/Utilities/GenSort.h>
#include <casacore/casa/aips.h>

#define CTPATCHEDINTERPVERB false

#define NEAREST InterpolateArray1D<Double,Float>::nearestNeighbour
#define LINEAR InterpolateArray1D<Double,Float>::linear
#define CUBIC InterpolateArray1D<Double,Float>::cubic
#define SPLINE InterpolateArray1D<Double,Float>::spline

#include <casacore/ms/MSOper/MSMetaData.h>
#include <synthesis/MeasurementComponents/MSMetaInfoForCal.h>
#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogIO.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

// Ctor
CTPatchedInterp::CTPatchedInterp(NewCalTable& ct,
				 VisCalEnum::MatrixType mtype,
				 Int nPar,
				 const String& timetype,
				 const String& freqtype,
				 const String& fieldtype,
				 Vector<Int> spwmap,
				 Vector<Int> fldmap,
				 const CTTIFactoryPtr cttifactoryptr) :
  ct_(ct),
  msmc_(NULL),
  mtype_(mtype),
  isCmplx_(false),
  nPar_(nPar),
  nFPar_(nPar),
  timeType_(timetype),
  freqTypeStr_(freqtype),
  relativeFreq_(freqtype.contains("rel")),
  freqInterpMethod0_(ftype(freqTypeStr_)),
  freqInterpMethod_(freqInterpMethod0_),
  freqInterpMethodVec_(),
  byObs_(timetype.contains("perobs")), // detect slicing by obs
  byField_(fieldtype=="nearest" || fieldtype=="map"), 
  nChanIn_(),
  freqIn_(),
  nMSObs_(1), // byObs_?ct.observation().nrow():1),  // assume CT shapes for MS shapes
  nMSFld_(ct.field().nrow()),                 
  nMSSpw_(ct.spectralWindow().nrow()),
  nMSAnt_(ct.antenna().nrow()),
  altFld_(),
  nCTObs_(1), // byObs_?ct.observation().nrow():1),
  nCTFld_(byField_?ct.field().nrow():1),
  nCTSpw_(ct.spectralWindow().nrow()),
  nCTAnt_(ct.antenna().nrow()),
  nCTElem_(0),
  spwInOK_(),
  fldMap_(),
  spwMap_(),
  antMap_(),
  elemMap_(),
  conjTab_(),
  result_(),
  resFlag_(),
  tI_(),
  tIdel_(),
  lastFld_(ct.spectralWindow().nrow(),-1),
  lastObs_(ct.spectralWindow().nrow(),-1),
  cttifactoryptr_(cttifactoryptr)
{
  if (CTPATCHEDINTERPVERB) cout << "CTPatchedInterp::CTPatchedInterp(<no MS>)" << endl;

  //  cout << "freqInterpMethod_ = " << freqInterpMethod_ << endl;

  freqInterpMethodVec_.resize(nMSSpw_);
  freqInterpMethodVec_.set(freqInterpMethod0_);
    
  switch(mtype_) {
  case VisCalEnum::GLOBAL: {

    throw(AipsError("CTPatchedInterp::ctor: No non-Mueller/Jones yet."));

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
  if (isCmplx_)  // Complex input
    nFPar_*=2;  // interpolating 2X as many Float values


  // Set channel/freq info
  CTSpWindowColumns ctspw(ct_.spectralWindow());
  ctspw.numChan().getColumn(nChanIn_);
  freqIn_.resize(nCTSpw_);
  for (uInt iCTspw=0;iCTspw<ctspw.nrow();++iCTspw) {
    ctspw.chanFreq().get(iCTspw,freqIn_(iCTspw),true);
    if (relativeFreq_) {
      Vector<Double>& fIn(freqIn_(iCTspw));
      fIn-=mean(fIn); //  assume mean freq is center, and apply offset
      // Flip LSB
      if (fIn.nelements()>1 && fIn(0)>fIn(1)) {
	//cout << " spw=" << iCTspw << " LSB!" << endl;
	fIn*=Double(-1.0);
      }
    }
    //cout << " freqIn_(" << iCTspw << ")=" << freqIn_(iCTspw) << endl;
  }

  // Manage 'byObs_' carefully
  if (byObs_) {

    Int nMSObs=ct_.observation().nrow(); // assume CT shapes for MS shapes
    Int nCTObs=ct_.observation().nrow();

    // Count _available_ obsids in caltable
    ROCTMainColumns ctmc(ct_);
    Vector<Int> obsid;
    ctmc.obsId().getColumn(obsid);
    Int nctobsavail=genSort(obsid,Sort::Ascending,
			    (Sort::QuickSort | Sort::NoDuplicates));


    LogIO log;
    ostringstream msg;
    
    if (nctobsavail==1) {
      byObs_=false;
      msg << "Only one ObsId found in "
	  << Path(ct_.tableName()).baseName().before(".tempMemCal")
	  << "; ignoring 'perobs' interpolation.";
      log << msg.str() << LogIO::WARN;
    }
    else {

      // Verify consistency between CT and MS
      if (nctobsavail==nCTObs &&
	  nctobsavail==nMSObs) {
	// Everything ok
	nCTObs_=nCTObs;
	nMSObs_=nMSObs;
      }
      else {
	// only 1 obs, or available nobs doesn't match MS
	byObs_=false;
	msg << "Multiple ObsIds found in "
	    << Path(ct_.tableName()).baseName().before(".tempMemCal")
	    << ", but they do not match the MS ObsIds;"
	    << " turning off 'perobs'.";
	log << msg.str() << LogIO::WARN;
      }
    }

  }

  // Initialize caltable slices
  sliceTable();

  // Set spwmap
  setSpwMap(spwmap);

  // Set fldmap
  if (byField_) {
    if (fieldtype=="map")
      setFldMap(fldmap);  // Use specified map
    else
      setFldMap(ct.field());  // Use CalTable's fields ('nearest')
  }
  else
    setDefFldMap();

  // Set defaultmaps
  setDefAntMap();
  setElemMap();

  // Resize working arrays
  result_.resize(nMSSpw_,nMSFld_,nMSObs_);
  resFlag_.resize(nMSSpw_,nMSFld_,nMSObs_);
  timeResult_.resize(nMSSpw_,nMSFld_,nMSObs_);
  timeResFlag_.resize(nMSSpw_,nMSFld_,nMSObs_);
  freqResult_.resize(nMSSpw_,nMSFld_,nMSObs_);
  freqResFlag_.resize(nMSSpw_,nMSFld_,nMSObs_);

  // Figure out where we can duplicate field interpolators
  calcAltFld();

  // Setup mapped interpolators
  // TBD: defer this to later, so that spwmap, etc. can be revised
  //   before committing to the interpolation engines
  makeInterpolators();

  //  state();
}

CTPatchedInterp::CTPatchedInterp(NewCalTable& ct,
				 VisCalEnum::MatrixType mtype,
				 Int nPar,
				 const String& timetype,
				 const String& freqtype,
				 const String& fieldtype,
				 const MeasurementSet& ms,
                 const MSMetaInfoForCal& msmc,
				 Vector<Int> spwmap,
				 const CTTIFactoryPtr cttifactoryptr) :
  ct_(ct),
  msmc_(&msmc),
  mtype_(mtype),
  isCmplx_(false),
  nPar_(nPar),
  nFPar_(nPar),
  timeType_(timetype),
  freqTypeStr_(freqtype),
  relativeFreq_(freqtype.contains("rel")),
  freqInterpMethod0_(ftype(freqTypeStr_)),
  freqInterpMethod_(freqInterpMethod0_),
  freqInterpMethodVec_(),
  byObs_(timetype.contains("perobs")), // detect slicing by obs
  byField_(fieldtype=="nearest"),  // for now we are NOT slicing by field
  nChanIn_(),
  freqIn_(),
  nMSObs_(1), // byObs_?ms.observation().nrow():1),
  nMSFld_(ms.field().nrow()),  
  nMSSpw_(ms.spectralWindow().nrow()),
  nMSAnt_(ms.antenna().nrow()),
  altFld_(),
  nCTObs_(1),  // byObs_?ct.observation().nrow():1),
  nCTFld_(byField_?ct.field().nrow():1),
  nCTSpw_(ct.spectralWindow().nrow()),
  nCTAnt_(ct.antenna().nrow()),
  nCTElem_(0),
  spwInOK_(),
  fldMap_(),
  spwMap_(),
  antMap_(),
  elemMap_(),
  conjTab_(),
  result_(),
  resFlag_(),
  tI_(),
  tIdel_(),
  lastFld_(ms.spectralWindow().nrow(),-1),
  lastObs_(ms.spectralWindow().nrow(),-1),
  cttifactoryptr_(cttifactoryptr)
{

  if (CTPATCHEDINTERPVERB) cout << "CTPatchedInterp::CTPatchedInterp(CT,MS)" << endl;

  //freqInterpMethod_=ftype(freqTypeStr_);

  freqInterpMethodVec_.resize(nMSSpw_);
  freqInterpMethodVec_.set(freqInterpMethod0_);

  //  cout << "freqInterpMethod_ = " << freqInterpMethod_ << endl;

  switch(mtype_) {
  case VisCalEnum::GLOBAL: {

    throw(AipsError("CTPatchedInterp::ctor: No non-Mueller/Jones yet."));

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
  isCmplx_=(ct_.keywordSet().asString("ParType")=="Complex");  // Complex input
  if (isCmplx_)
    nFPar_*=2;  // interpolating 2X as many Float values

  // Set channel/freq info
  CTSpWindowColumns ctspw(ct_.spectralWindow());
  ctspw.numChan().getColumn(nChanIn_);
  freqIn_.resize(nCTSpw_);
  for (uInt iCTspw=0;iCTspw<ctspw.nrow();++iCTspw) {
    ctspw.chanFreq().get(iCTspw,freqIn_(iCTspw),true);
    if (relativeFreq_) {
      Vector<Double>& fIn(freqIn_(iCTspw));
      fIn-=mean(fIn); //  assume mean freq is center, and apply offset
      // Flip LSB
      if (fIn.nelements()>1 && fIn(0)>fIn(1)) {
	//cout << " spw=" << iCTspw << " LSB!" << endl;
	fIn*=Double(-1.0);
      }
    }
    //cout << " freqIn_(" << iCTspw << ")=" << freqIn_(iCTspw) << endl;
  }

  // Manage 'byObs_' carefully
  if (byObs_) {
    Int nMSObs=ms.observation().nrow();
    Int nCTObs=ct_.observation().nrow();

    // Count _available_ obsids in caltable
    ROCTMainColumns ctmc(ct_);
    Vector<Int> obsid;
    ctmc.obsId().getColumn(obsid);
    Int nctobsavail=genSort(obsid,Sort::Ascending,
			    (Sort::QuickSort | Sort::NoDuplicates));


    LogIO log;
    ostringstream msg;
    
    if (nctobsavail==1) {
      byObs_=false;
      msg << "Only one ObsId found in "
	  << Path(ct_.tableName()).baseName().before(".tempMemCal")
	  << "; ignoring 'perobs' interpolation.";
      log << msg.str() << LogIO::WARN;
    }
    else {

      // Verify consistency between CT and MS
      if (nctobsavail==nCTObs &&
	  nctobsavail==nMSObs) {
	// Everything ok
	nCTObs_=nCTObs;
	nMSObs_=nMSObs;
      }
      else {
	// only 1 obs, or available nobs doesn't match MS
	byObs_=false;
	msg << "Multiple ObsIds found in "
	    << Path(ct_.tableName()).baseName().before(".tempMemCal")
	    << ", but they do not match the MS ObsIds;"
	    << " turning off 'perobs'.";
	log << msg.str() << LogIO::WARN;
      }
    }
  }

  // Initialize caltable slices
  sliceTable();

  // Set spwmap
  setSpwMap(spwmap);

  // Set fldmap
  if (byField_)
    setFldMap(ms.field());  // on a trial basis
  else
    setDefFldMap();

  // Set defaultmaps
  setDefAntMap();
  setElemMap();

  // Resize working arrays
  result_.resize(nMSSpw_,nMSFld_,nMSObs_);
  resFlag_.resize(nMSSpw_,nMSFld_,nMSObs_);
  timeResult_.resize(nMSSpw_,nMSFld_,nMSObs_);
  timeResFlag_.resize(nMSSpw_,nMSFld_,nMSObs_);
  freqResult_.resize(nMSSpw_,nMSFld_,nMSObs_);
  freqResFlag_.resize(nMSSpw_,nMSFld_,nMSObs_);

  // Figure out where we can duplicate field interpolators
  calcAltFld();
  
  // Setup mapped interpolators
  // TBD: defer this to later, so that spwmap, etc. can be revised
  //   before committing to the interpolation engines
  makeInterpolators();

  //  state();

}

CTPatchedInterp::CTPatchedInterp(NewCalTable& ct,
				 VisCalEnum::MatrixType mtype,
				 Int nPar,
				 const String& timetype,
				 const String& freqtype,
				 const String& fieldtype,
				 const MSColumns& mscol,
				 Vector<Int> spwmap,
				 const CTTIFactoryPtr cttifactoryptr) :
  ct_(ct),
  msmc_(NULL),
  mtype_(mtype),
  isCmplx_(false),
  nPar_(nPar),
  nFPar_(nPar),
  timeType_(timetype),
  freqTypeStr_(freqtype),
  relativeFreq_(freqtype.contains("rel")),
  freqInterpMethod0_(ftype(freqTypeStr_)),
  freqInterpMethod_(freqInterpMethod0_),
  freqInterpMethodVec_(),
  byObs_(false),                // turn off for old-fashioned
  byField_(fieldtype=="nearest"),  // for now we are NOT slicing by field
  nChanIn_(),
  freqIn_(),
  nMSFld_(mscol.field().nrow()),  
  nMSSpw_(mscol.spectralWindow().nrow()),
  nMSAnt_(mscol.antenna().nrow()),
  altFld_(),
  nCTFld_(byField_?ct.field().nrow():1),
  nCTSpw_(ct.spectralWindow().nrow()),
  nCTAnt_(ct.antenna().nrow()),
  nCTElem_(0),
  spwInOK_(),
  fldMap_(),
  spwMap_(),
  antMap_(),
  elemMap_(),
  conjTab_(),
  result_(),
  resFlag_(),
  tI_(),
  tIdel_(),
  lastFld_(mscol.spectralWindow().nrow(),-1),
  lastObs_(mscol.spectralWindow().nrow(),-1),
  cttifactoryptr_(cttifactoryptr)
{
  if (CTPATCHEDINTERPVERB) cout << "CTPatchedInterp::CTPatchedInterp(mscol)" << endl;

  //  cout << "freqInterpMethod_ = " << freqInterpMethod_ << endl;
  freqInterpMethodVec_.resize(nMSSpw_);
  freqInterpMethodVec_.set(freqInterpMethod0_);

  switch(mtype_) {
  case VisCalEnum::GLOBAL: {

    throw(AipsError("CTPatchedInterp::ctor: No non-Mueller/Jones yet."));

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
  if (isCmplx_)  // Complex input
    nFPar_*=2;  // interpolating 2X as many Float values

  // Set channel/freq info
  CTSpWindowColumns ctspw(ct_.spectralWindow());
  ctspw.numChan().getColumn(nChanIn_);
  freqIn_.resize(nCTSpw_);
  for (uInt iCTspw=0;iCTspw<ctspw.nrow();++iCTspw) {
    ctspw.chanFreq().get(iCTspw,freqIn_(iCTspw),true);
    if (relativeFreq_) {
      Vector<Double>& fIn(freqIn_(iCTspw));
      fIn-=mean(fIn); //  assume mean freq is center, and apply offset
      // Flip LSB
      if (fIn.nelements()>1 && fIn(0)>fIn(1)) {
	//cout << " spw=" << iCTspw << " LSB!" << endl;
	fIn*=Double(-1.0);
      }
    }
    //    cout << " freqIn_(" << iCTspw << ")=" << freqIn_(iCTspw) << endl;
  }

  // Initialize caltable slices
  sliceTable();

  // Set spwmap
  setSpwMap(spwmap);

  // Set fldmap
  if (byField_)
    setFldMap(mscol.field());  // on a trial basis
  else
    setDefFldMap();


  // Set defaultmaps
  setDefAntMap();
  setElemMap();

  // Resize working arrays
  result_.resize(nMSSpw_,nMSFld_,nMSObs_);
  resFlag_.resize(nMSSpw_,nMSFld_,nMSObs_);
  timeResult_.resize(nMSSpw_,nMSFld_,nMSObs_);
  timeResFlag_.resize(nMSSpw_,nMSFld_,nMSObs_);
  freqResult_.resize(nMSSpw_,nMSFld_,nMSObs_);
  freqResFlag_.resize(nMSSpw_,nMSFld_,nMSObs_);

  // Figure out where we can duplicate field interpolators
  calcAltFld();

  // Setup mapped interpolators
  // TBD: defer this to later, so that spwmap, etc. can be revised
  //   before committing to the interpolation engines
  makeInterpolators();

  //  state();

}
    
CTPatchedInterp::CTPatchedInterp(NewCalTable& ct,
                 VisCalEnum::MatrixType mtype,
                 Int nPar,
                 const String& timetype,
                 const String& freqtype,
                 const String& fieldtype,
                 const MeasurementSet& ms,
                 Vector<Int> spwmap,
                 const CTTIFactoryPtr cttifactoryptr) :
  ct_(ct),
  msmc_(NULL),
  mtype_(mtype),
  isCmplx_(false),
  nPar_(nPar),
  nFPar_(nPar),
  timeType_(timetype),
  freqTypeStr_(freqtype),
  relativeFreq_(freqtype.contains("rel")),
  freqInterpMethod0_(ftype(freqTypeStr_)),
  freqInterpMethod_(freqInterpMethod0_),
  freqInterpMethodVec_(),
  byObs_(timetype.contains("perobs")), // detect slicing by obs
  byField_(fieldtype=="nearest"),  // for now we are NOT slicing by field
  nChanIn_(),
  freqIn_(),
  nMSObs_(1), // byObs_?ms.observation().nrow():1),
  nMSFld_(ms.field().nrow()),
  nMSSpw_(ms.spectralWindow().nrow()),
  nMSAnt_(ms.antenna().nrow()),
  altFld_(),
  nCTObs_(1),  // byObs_?ct.observation().nrow():1),
  nCTFld_(byField_?ct.field().nrow():1),
  nCTSpw_(ct.spectralWindow().nrow()),
  nCTAnt_(ct.antenna().nrow()),
  nCTElem_(0),
  spwInOK_(),
  fldMap_(),
  spwMap_(),
  antMap_(),
  elemMap_(),
  conjTab_(),
  result_(),
  resFlag_(),
  tI_(),
  tIdel_(),
  lastFld_(ms.spectralWindow().nrow(),-1),
  lastObs_(ms.spectralWindow().nrow(),-1),
  cttifactoryptr_(cttifactoryptr)
{
    
  if (CTPATCHEDINTERPVERB) cout << "CTPatchedInterp::CTPatchedInterp(CT,MS)" << endl;
    
  //freqInterpMethod_=ftype(freqTypeStr_);
    
  freqInterpMethodVec_.resize(nMSSpw_);
  freqInterpMethodVec_.set(freqInterpMethod0_);
    
  //  cout << "freqInterpMethod_ = " << freqInterpMethod_ << endl;
    
  switch(mtype_) {
    case VisCalEnum::GLOBAL: {
            
      throw(AipsError("CTPatchedInterp::ctor: No non-Mueller/Jones yet."));
            
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
    isCmplx_=(ct_.keywordSet().asString("ParType")=="Complex");  // Complex input
    if (isCmplx_)
        nFPar_*=2;  // interpolating 2X as many Float values
    
    // Set channel/freq info
    CTSpWindowColumns ctspw(ct_.spectralWindow());
    ctspw.numChan().getColumn(nChanIn_);
    freqIn_.resize(nCTSpw_);
    for (uInt iCTspw=0;iCTspw<ctspw.nrow();++iCTspw) {
        ctspw.chanFreq().get(iCTspw,freqIn_(iCTspw),true);
        if (relativeFreq_) {
            Vector<Double>& fIn(freqIn_(iCTspw));
            fIn-=mean(fIn); //  assume mean freq is center, and apply offset
            // Flip LSB
            if (fIn.nelements()>1 && fIn(0)>fIn(1)) {
                //cout << " spw=" << iCTspw << " LSB!" << endl;
                fIn*=Double(-1.0);
            }
        }
        //cout << " freqIn_(" << iCTspw << ")=" << freqIn_(iCTspw) << endl;
    }
    
    // Manage 'byObs_' carefully
    if (byObs_) {
        Int nMSObs=ms.observation().nrow();
        Int nCTObs=ct_.observation().nrow();
        
        // Count _available_ obsids in caltable
        ROCTMainColumns ctmc(ct_);
        Vector<Int> obsid;
        ctmc.obsId().getColumn(obsid);
        Int nctobsavail=genSort(obsid,Sort::Ascending,
                                (Sort::QuickSort | Sort::NoDuplicates));
        
        
        LogIO log;
        ostringstream msg;
        
        if (nctobsavail==1) {
            byObs_=false;
            msg << "Only one ObsId found in "
            << Path(ct_.tableName()).baseName().before(".tempMemCal")
            << "; ignoring 'perobs' interpolation.";
            log << msg.str() << LogIO::WARN;
        }
        else {
            
            // Verify consistency between CT and MS
            if (nctobsavail==nCTObs &&
                nctobsavail==nMSObs) {
                // Everything ok
                nCTObs_=nCTObs;
                nMSObs_=nMSObs;
            }
            else {
                // only 1 obs, or available nobs doesn't match MS
                byObs_=false;
                msg << "Multiple ObsIds found in "
                << Path(ct_.tableName()).baseName().before(".tempMemCal")
                << ", but they do not match the MS ObsIds;"
                << " turning off 'perobs'.";
                log << msg.str() << LogIO::WARN;
            }
        }
    }
    
    // Initialize caltable slices
    sliceTable();
    
    // Set spwmap
    setSpwMap(spwmap);
    
    // Set fldmap
    if (byField_)
        setFldMap(ms.field());  // on a trial basis
    else
        setDefFldMap();
    
    // Set defaultmaps
    setDefAntMap();
    setElemMap();
    
    // Resize working arrays
    result_.resize(nMSSpw_,nMSFld_,nMSObs_);
    resFlag_.resize(nMSSpw_,nMSFld_,nMSObs_);
    timeResult_.resize(nMSSpw_,nMSFld_,nMSObs_);
    timeResFlag_.resize(nMSSpw_,nMSFld_,nMSObs_);
    freqResult_.resize(nMSSpw_,nMSFld_,nMSObs_);
    freqResFlag_.resize(nMSSpw_,nMSFld_,nMSObs_);
    
    // Figure out where we can duplicate field interpolators
    calcAltFld();
    
    // Setup mapped interpolators
    // TBD: defer this to later, so that spwmap, etc. can be revised
    //   before committing to the interpolation engines
    makeInterpolators();
    
    //  state();
    
}


// Destructor
CTPatchedInterp::~CTPatchedInterp() {

  if (CTPATCHEDINTERPVERB) cout << "CTPatchedInterp::~CTPatchedInterp()" << endl;

  {  
    IPosition sh(tI_.shape());
    for (Int l=0;l<sh(3);++l)
      for (Int k=0;k<sh(2);++k)
	for (Int j=0;j<sh(1);++j)
	  for (Int i=0;i<sh(0);++i) {
	    IPosition ip(4,i,j,k,l);
	    if (tIdel_(ip))
	      delete tI_(ip);
	  }
    tI_.resize();
  }
  {
    IPosition sh(ctSlices_.shape());
    for (Int l=0;l<sh(3);++l)
      for (Int k=0;k<sh(2);++k)
	for (Int j=0;j<sh(1);++j)
	  for (Int i=0;i<sh(0);++i) {
	    IPosition ip(4,i,j,k,l);
	    if (ctSlices_(ip)) 
	      delete ctSlices_(ip);
	  }
    ctSlices_.resize();
  }
}

Bool CTPatchedInterp::interpolate(Int msobs, Int msfld, Int msspw, Double time, Double freq) {

  if (CTPATCHEDINTERPVERB) cout << "CTPatchedInterp::interpolate(...)" << endl;

  Bool newcal(false);
  IPosition ip(4,0,msspw,msfld,thisobs(msobs));

  // Loop over _output_ elements
  for (Int iMSElem=0;iMSElem<nMSElem_;++iMSElem) {
    // Call fully _patched_ time-interpolator, keeping track of 'newness'
    //  fills timeResult_/timeResFlag_ implicitly
    ip(0)=iMSElem;
    
    if (!tI_(ip)) {
      //cout << "Flagging: " << ip << endl;
      newcal=true;
    }
    else {
      if (freq>0.0)
	newcal|=tI_(ip)->interpolate(time,freq);
      else
	newcal|=tI_(ip)->interpolate(time);
    }
  }

  // Whole result referred to time result:
  result_(msspw,msfld,thisobs(msobs)).reference(timeResult_(msspw,msfld,thisobs(msobs)));
  resFlag_(msspw,msfld,thisobs(msobs)).reference(timeResFlag_(msspw,msfld,thisobs(msobs)));

  // Detect if obs or fld changed, and cal is obs- or fld-dep
  Bool diffobsfld(false);
  diffobsfld|=(byField_ && msfld!=lastFld_(msspw));   // field-dep, and field changed
  diffobsfld|=(byObs_ && msobs!=lastObs_(msspw));     // obs-dep, and obs changed
  newcal|=diffobsfld;  // update newcal for return

  // Remember for next pass
  lastFld_(msspw)=msfld;
  lastObs_(msspw)=msobs;

  return newcal;
}

Bool CTPatchedInterp::interpolate(Int msobs, Int msfld, Int msspw, Double time, const Vector<Double>& freq) {

  if (CTPATCHEDINTERPVERB) cout << "CTPatchedInterp::interpolate(...,freq)" << endl;

  // obsid non-degenerate only if byObs_

  // The number of requested channels
  uInt nMSChan=freq.nelements();

  // Ensure freq result Array is properly sized
  if (freqResult_(msspw,msfld,thisobs(msobs)).nelements()==0) {
     Int thisAltFld=altFld_(msfld);
     if (freqResult_(msspw,thisAltFld,thisobs(msobs)).nelements()==0) {
       freqResult_(msspw,thisAltFld,thisobs(msobs)).resize(nFPar_,nMSChan,nMSElem_);
       freqResFlag_(msspw,thisAltFld,thisobs(msobs)).resize(nPar_,nMSChan,nMSElem_);
       freqResFlag_(msspw,thisAltFld,thisobs(msobs)).set(true);
     }
     if (thisAltFld!=msfld) {
       freqResult_(msspw,msfld,thisobs(msobs)).reference(freqResult_(msspw,thisAltFld,thisobs(msobs)));
       freqResFlag_(msspw,msfld,thisobs(msobs)).reference(freqResFlag_(msspw,thisAltFld,thisobs(msobs)));
     }
  }


  
  
  {

    // CAS-11744
    // In the following, we verify that there are sufficient soln channels to 
    // do the requested freq-dep interpolation.  If there are not, we
    // change the freq-dep interp mode to one that will work (linear or
    // nearest, for nchan=2 or 1, respectively), and issue a logger warning.
    // This is necessary specifically for the current _flag_ interpolation code.
    // The warning is thought warranted since freq-dep calibration (B, Tsys)
    // usually has adequate nchan, and if it does not, this may be unexpected
    // (e.g., use of too large a freq solint).
    
    // Check freq sampling adequate for specified freq interpolation
    Int nSolChan=timeResult_(msspw,msfld,thisobs(msobs)).shape()(1);
    LogIO log;
    ostringstream msg;

    switch (freqInterpMethod0_) {
    case LINEAR: {
      if (nSolChan<2 && freqInterpMethodVec_(msspw)!=NEAREST) {
	freqInterpMethodVec_(msspw)=NEAREST;  // change to nearest for this msspw
	ostringstream spwmapstr;
	spwmapstr << " (mapped to CT spw=" << spwMap_(msspw) << ")";
	msg << "In caltable "
	    << Path(ct_.tableName()).baseName().before(".tempMemCal")
	    << " (" << ct_.keywordSet().asString("VisCal") << ")"
	    << ":" << endl
	    << " Insufficient solution channel sampling (nchan=" << nSolChan << ") for frequency-dependent LINEAR interpolation " << endl
	    << " of calibration for MS spw=" << msspw
	    << ( msspw!=spwMap_(msspw) ? String(spwmapstr) : "" ) 
	    << "; using NEAREST instead.";
	log << LogOrigin("CTPatchedInterp","interpolate") << msg.str() << LogIO::WARN;
      }
      break;
    }
    case CUBIC:
    case SPLINE: {
      if (nSolChan<2 && freqInterpMethodVec_(msspw)>NEAREST) {
	freqInterpMethodVec_(msspw)=NEAREST;  // change to nearest for this msspw
	ostringstream spwmapstr;
	spwmapstr << " (mapped to CT spw=" << spwMap_(msspw) << ")";
	msg << "In caltable "
	    << Path(ct_.tableName()).baseName().before(".tempMemCal")
	    << " (" << ct_.keywordSet().asString("VisCal") << ")"
	    << ":" << endl
	    << " Insufficient solution channel sampling (nchan=" << nSolChan << ") for frequency-dependent CUBIC/SPLINE interpolation " << endl
	    << " of calibration for MS spw=" << msspw
	    << ( msspw!=spwMap_(msspw) ? String(spwmapstr) : "" ) 
	    << "; using NEAREST instead.";
	log << LogOrigin("CTPatchedInterp","interpolate") << msg.str() << LogIO::WARN;
      }
      else if (nSolChan<4 && freqInterpMethodVec_(msspw)>LINEAR) {
	freqInterpMethodVec_(msspw)=LINEAR;  // change to nearest for this msspw
	ostringstream spwmapstr;
	spwmapstr << " (mapped to CT spw=" << spwMap_(msspw) << ")";
	msg << "In caltable "
	    << Path(ct_.tableName()).baseName().before(".tempMemCal")
	    << " (" << ct_.keywordSet().asString("VisCal") << ")"
	    << ":" << endl
	    << " Insufficient solution channel sampling (nchan=" << nSolChan << ") for frequency-dependent CUBIC/SPLINE interpolation " << endl
	    << " of calibration for MS spw=" << msspw
	    << ( msspw!=spwMap_(msspw) ? String(spwmapstr) : "" ) 
	    << "; using LINEAR instead.";
	log << LogOrigin("CTPatchedInterp","interpolate") << msg.str() << LogIO::WARN;
      }
      break;
    }
    default: {
      break;
    }
    }
  }

  // Use the msspw-specific one!
  freqInterpMethod_=freqInterpMethodVec_(msspw);

  Bool newcal(false);
  IPosition ip(4,0,msspw,msfld,thisobs(msobs));
  // Loop over _output_ antennas
  for (Int iMSElem=0;iMSElem<nMSElem_;++iMSElem) {
    // Call time interpolation calculation; resample in freq if new
    //   (fills timeResult_/timeResFlag_ implicitly)
    ip(0)=iMSElem;
    if (!tI_(ip)) {
      //      if (iMSElem==0) cout << "Flagging: " << ip << endl;
      newcal=true;
    }
    else {

      if (tI_(ip)->interpolate(time)) { 
	// Resample in frequency
	Matrix<Float> fR(freqResult_(msspw,msfld,thisobs(msobs)).xyPlane(iMSElem));
	Matrix<Bool> fRflg(freqResFlag_(msspw,msfld,thisobs(msobs)).xyPlane(iMSElem));
	Matrix<Float> tR(timeResult_(msspw,msfld,thisobs(msobs)).xyPlane(iMSElem));
	Matrix<Bool> tRflg(timeResFlag_(msspw,msfld,thisobs(msobs)).xyPlane(iMSElem));
	resampleInFreq(fR,fRflg,freq,tR,tRflg,freqIn_(spwMap_(msspw)));
	
	// Calibration is new
	newcal=true;
      }
    }
  }

  // Whole result referred to freq result:
  result_(msspw,msfld,thisobs(msobs)).reference(freqResult_(msspw,msfld,thisobs(msobs)));
  resFlag_(msspw,msfld,thisobs(msobs)).reference(freqResFlag_(msspw,msfld,thisobs(msobs)));

  // Detect if obs or fld changed, and cal is obs- or fld-dep
  Bool diffobsfld(false);
  diffobsfld|=(byField_ && msfld!=lastFld_(msspw));   // field-dep, and field changed
  diffobsfld|=(byObs_ && msobs!=lastObs_(msspw));     // obs-dep, and obs changed
  newcal|=diffobsfld;  //  update newcal for return

  /*
  if (newcal) {
    Double t0(86400.0*floor(time/86400.0));
    cout << boolalpha
	 << "fld="<<msfld
	 << " obs="<<thisobs(msobs)
	 << " spw="<<msspw
	 << " time="<< time-t0
	 << " diffobsfld=" << diffobsfld
	 << " new=" << newcal
	 << " tI_(ip)=" << tI_(ip)
	 << " chan="<< nMSChan/2
	 << " result=" << result_(msspw,msfld,thisobs(msobs))(0,nMSChan/2,0)
	 << " addr=" << &result_(msspw,msfld,thisobs(msobs))(0,nMSChan/2,0)
	 << endl;
  }
  */
  // Remember for next pass
  lastFld_(msspw)=msfld;
  lastObs_(msspw)=msobs;

  return newcal;
}

// spwOK info for users
Bool CTPatchedInterp::spwOK(Int spw) const {

  if (spw<Int(spwMap_.nelements()))
    return this->spwInOK(spwMap_(spw));

  // Something wrong...
  return false;

}
Bool CTPatchedInterp::spwInOK(Int spw) const {

  if (spw<Int(spwInOK_.nelements()))
    return spwInOK_(spw);

  // Something wrong
  return false;

}


  // Report state
void CTPatchedInterp::state() {

  if (CTPATCHEDINTERPVERB) cout << "CTPatchedInterp::state()" << endl;

  cout << "-state--------" << endl;
  cout << " ct_      = " << Path(ct_.tableName()).baseName().before(".tempMemCal") << endl;
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
  cout << " fldMap_  = " << fldMap_ << endl;
  cout << " spwMap_  = " << spwMap_ << endl;
  cout << " antMap_  = " << antMap_ << endl;
  cout << " byObs_   = " << byObs_ << endl;
  cout << " byField_ = " << byField_ << endl;
  cout << " altFld_  = " << altFld_ << endl;
  cout << " timeType_ = " << timeType_ << endl;
  cout << " freqTypeStr_ = " << freqTypeStr_ << endl;
  cout << " freqInterpMethod0_ = " << freqInterpMethod0_ << endl;
  cout << " freqInterpMethodVec_ = " << freqInterpMethodVec_ << endl;
  cout << " spwInOK_ = " << spwInOK_ << endl;
}

void CTPatchedInterp::sliceTable() {

  if (CTPATCHEDINTERPVERB) cout << "  CTPatchedInterp::sliceTable()" << endl;

  // This method generates per-spw, per-antenna (and eventually per-field?)
  //   caltables.  

  // Ensure time sort of input table
  //  TBD (here or inside loop?)

  // Indexed by the fields, spws, ants in the cal table (pre-mapped)
  ctSlices_.resize(IPosition(4,nCTElem_,nCTSpw_,nCTFld_,nCTObs_));
  ctSlices_.set(NULL);

  // Initialize spwInOK_
  spwInOK_.resize(nCTSpw_);
  spwInOK_.set(false);

  // Set up iterator
  //  TBD: handle baseline-based case!
  Block<String> sortcol;
  Int addobs( (byObs_ ? 1 : 0) ); // slicing by obs?
  Int addfld( (byField_ ? 1 : 0) ); // slicing by field?

  switch(mtype_) {
  case VisCalEnum::GLOBAL: {

    throw(AipsError("CTPatchedInterp::sliceTable: No non-Mueller/Jones yet."));

    sortcol.resize(1+addobs+addfld);
    if (byObs_) sortcol[0]="OBSERVATION_ID";  // slicing by obs
    if (byField_) sortcol[0+addobs]="FIELD_ID";  // slicing by field
    sortcol[0+addobs+addfld]="SPECTRAL_WINDOW_ID";
    ROCTIter ctiter(ct_,sortcol);
    while (!ctiter.pastEnd()) {
      Int ispw=ctiter.thisSpw();
      Int ifld = (byField_ ? ctiter.thisField() : 0); // use 0 if not slicing by field
      Int iobs = (byObs_ ? ctiter.thisObs() : 0); // use 0 if not slicing by obs
      IPosition ip(4,0,ispw,ifld,iobs);
      ctSlices_(ip)= new NewCalTable(ctiter.table());
      spwInOK_(ispw)=(spwInOK_(ispw) || ctSlices_(ip)->nrow()>0);
      ctiter.next();
    }
    break;
  }
  case VisCalEnum::MUELLER: {
    sortcol.resize(3+addobs+addfld);
    if (byObs_) sortcol[0]="OBSERVATION_ID";  // slicing by obs
    if (byField_) sortcol[0+addobs]="FIELD_ID";  // slicing by field
    sortcol[0+addobs+addfld]="SPECTRAL_WINDOW_ID";
    sortcol[1+addobs+addfld]="ANTENNA1";
    sortcol[2+addobs+addfld]="ANTENNA2";
    ROCTIter ctiter(ct_,sortcol);
    while (!ctiter.pastEnd()) {
      Int ispw=ctiter.thisSpw();
      Int iant1=ctiter.thisAntenna1();
      Int iant2=ctiter.thisAntenna2();
      Int ibln=blnidx(iant1,iant2,nCTAnt_);
      Int ifld = (byField_ ? ctiter.thisField() : 0); // use 0 if not slicing by field
      Int iobs = (byObs_ ? ctiter.thisObs() : 0); // use 0 if not slicing by obs
      IPosition ip(4,ibln,ispw,ifld,iobs);
      ctSlices_(ip)=new NewCalTable(ctiter.table());
      spwInOK_(ispw)=(spwInOK_(ispw) || ctSlices_(ip)->nrow()>0);
      ctiter.next();
    }    
    break;
  }
  case VisCalEnum::JONES: {
    sortcol.resize(2+addobs+addfld);
    if (byObs_) sortcol[0]="OBSERVATION_ID";  // slicing by obs
    if (byField_) sortcol[0+addobs]="FIELD_ID";  // slicing by field
    sortcol[0+addobs+addfld]="SPECTRAL_WINDOW_ID";
    sortcol[1+addobs+addfld]="ANTENNA1";
    ROCTIter ctiter(ct_,sortcol);
    while (!ctiter.pastEnd()) {
      Int ispw=ctiter.thisSpw();
      Int iant=ctiter.thisAntenna1();
      Int ifld = (byField_ ? ctiter.thisField() : 0); // use 0 if not slicing by field
      Int iobs = (byObs_ ? ctiter.thisObs() : 0); // use 0 if not slicing by obs
      IPosition ip(4,iant,ispw,ifld,iobs);
      ctSlices_(ip)= new NewCalTable(ctiter.table());
      spwInOK_(ispw)=(spwInOK_(ispw) || ctSlices_(ip)->nrow()>0);
      ctiter.next();
    }    
    break;
  }
  }

}

// Initialize by iterating over the supplied table
void CTPatchedInterp::makeInterpolators() {

  if (CTPATCHEDINTERPVERB) cout << "  CTPatchedInterp::initialize()" << endl;

  // cal table name for messages
  Path pathname(ct_.tableName());
  String tabname=pathname.baseName().before(".tempMem");

  // Size/initialize interpolation engines
  IPosition tIsize(4,nMSElem_,nMSSpw_,nMSFld_,nMSObs_);
  tI_.resize(tIsize);
  tI_.set(NULL);
  tIdel_.resize(tIsize);
  tIdel_.set(false);

  Bool reportBadSpw(false);
  for (Int iMSObs=0;iMSObs<nMSObs_;++iMSObs) {
  for (Int iMSFld=0;iMSFld<nMSFld_;++iMSFld) {

    if (altFld_(iMSFld)==iMSFld) {

      //      cout << "Making  interpolators for        " << iMSFld << " (mapped from " << fldMap_(iMSFld) << ")" << endl;

      for (Int iMSSpw=0;iMSSpw<nMSSpw_;++iMSSpw) { 
	
	// Only if the required CT spw is available
	//  (spwmap applied in spwOK method)
	if (this->spwOK(iMSSpw)) {
	  
	  // Size up the timeResult_ Cube (NB: channel shape matches Cal Table)
	  if (timeResult_(iMSSpw,iMSFld,iMSObs).nelements()==0) {
	    timeResult_(iMSSpw,iMSFld,iMSObs).resize(nFPar_,nChanIn_(spwMap_(iMSSpw)),nMSElem_);
	    timeResFlag_(iMSSpw,iMSFld,iMSObs).resize(nPar_,nChanIn_(spwMap_(iMSSpw)),nMSElem_);
	    timeResFlag_(iMSSpw,iMSFld,iMSObs).set(true);
	  }
	  for (Int iMSElem=0;iMSElem<nMSElem_;++iMSElem) {
	    // Realize the mapping 
	    IPosition ictip(4,elemMap_(iMSElem),spwMap_(iMSSpw),fldMap_(iMSFld),iMSObs);
	    IPosition tIip(4,iMSElem,iMSSpw,iMSFld,iMSObs);
	    Matrix<Float> tR(timeResult_(iMSSpw,iMSFld,iMSObs).xyPlane(iMSElem));
	    Matrix<Bool> tRf(timeResFlag_(iMSSpw,iMSFld,iMSObs).xyPlane(iMSElem));

	    // If the ct slice exists, set up an interpolator
	    if (ctSlices_(ictip)) {
	      NewCalTable& ict(*ctSlices_(ictip));
	      if (!ict.isNull()) {
		tI_(tIip)=(*cttifactoryptr_)(ict,timeType_,tR,tRf);
		tIdel_(tIip)=true;
	      }
	    }
	    else {
	      // the required ct slice is empty, so arrange to flag it
	      tI_(tIip)=NULL; 
	      tR.set(0.0);
	      tRf.set(true);
            
          // Get Obs Id from summary, from third set of keys get scan(s)
          // Get antenna from scan and fields for scan
          // Logic check -->
          // Do you have obs -> is your field in scans? Spw in field? antenna in scan?
          // If all true print below, else don't print anything
            
          Record summary = msmc_->msmd().getSummary();
          std::set<uInt> spws = msmc_->msmd().getSpwsForField(iMSFld);
            
          LogIO log;
          ostringstream msg;
            
          if(spws.find(iMSSpw) != spws.end() &&
             summary.isDefined("observationID=" + String::toString(iMSObs)))
          {
              
              // casa log post
              msg  << "MS obs=" << iMSObs
               << ",fld=" << iMSFld
               << ",spw=" << iMSSpw
               << ",ant=" << iMSElem
               << " cannot be calibrated by " << tabname
               << " as mapped, and will be flagged in this process.";
              log << msg.str() << LogIO::WARN;
          }
	    }
	  } // iMSElem
	} // spwOK
	else
	  reportBadSpw=true;
      } // iMSSpw

    } // not re-using
    else {
      // Point to an existing interpolator group
      Int thisAltFld=altFld_(iMSFld);

      //      cout << "Reusing interpolators from " << thisAltFld << " for " << iMSFld << " (mapped to   " << fldMap_(iMSFld) << ")" << endl;

      for (Int iMSSpw=0;iMSSpw<nMSSpw_;++iMSSpw) { 
	timeResult_(iMSSpw,iMSFld,iMSObs).reference(timeResult_(iMSSpw,thisAltFld,iMSObs));
	timeResFlag_(iMSSpw,iMSFld,iMSObs).reference(timeResFlag_(iMSSpw,thisAltFld,iMSObs));
	for (Int iMSElem=0;iMSElem<nMSElem_;++iMSElem) {
	  IPosition tIip0(4,iMSElem,iMSSpw,iMSFld,iMSObs),tIip1(4,iMSElem,iMSSpw,thisAltFld,iMSObs);
	  tI_(tIip0)=tI_(tIip1);
	}
      }
    }
  } // iMSFld
  } // iMSObs


  if (reportBadSpw) {
    cout << "The following MS spws have no corresponding cal spws in " << tabname << ": ";
    for (Int iMSSpw=0;iMSSpw<nMSSpw_;++iMSSpw)
      //  (spwmap applied in spwOK method)
      if (!this->spwOK(iMSSpw)) cout << iMSSpw << " ";
    cout << endl;
  }

}



void CTPatchedInterp::setFldMap(const MSField& msfld) {
  
  MSFieldColumns fcol(msfld);
  setFldMap(fcol);

}

void CTPatchedInterp::setFldMap(const MSFieldColumns& fcol) {

   // Set the default fldmap
   setDefFldMap();
   //   cout << "Nominal fldMap_ = " << fldMap_ << endl;

   ROCTColumns ctcol(ct_);

   // Discern _available_ fields in the CT
   Vector<Int> ctFlds;
   ctcol.fieldId().getColumn(ctFlds);
   Int nAvFlds=genSort(ctFlds,Sort::Ascending,(Sort::QuickSort | Sort::NoDuplicates));
   ctFlds.resize(nAvFlds,true);

   //cout << "nAvFlds = " << nAvFlds << endl;
   //cout << "ctFlds  = " << ctFlds << endl;

   // If only one CT field, just use it

   Vector<Double> fseps(0);
   if (nAvFlds==1) 
     fldMap_.set(ctFlds(0));
   else {
     // For each MS field, find the nearest available CT field 
     Int nMSFlds=fcol.nrow();
     fseps.resize(nMSFlds);
     fseps.set(0.0);
     MDirection msdir,ctdir;
     Vector<Double> sep(nAvFlds);
     IPosition ipos(1,0);  // get the first direction stored (no poly yet)
     for (Int iMSFld=0;iMSFld<nMSFlds;++iMSFld) {
       msdir=fcol.phaseDirMeas(iMSFld);
       //cout << iMSFld << ":" << msdir.getValue() << endl;
       sep.set(DBL_MAX);
       for (Int iCTFld=0;iCTFld<nAvFlds;++iCTFld) {
	 // Get cal field direction, converted to ms field frame
	 ctdir=ctcol.field().phaseDirMeas(ctFlds(iCTFld));
	 MDirection::Convert(ctdir,msdir.getRef());
	 //cout << "   c:" << ctFlds(iCTFld) << ":" << ctdir.getValue() << endl;
	 sep(iCTFld)=ctdir.getValue().separation(msdir.getValue());
       }
       // Sort separations
       Vector<uInt> ord;
       Int nsep=genSort(ord,sep,Sort::Ascending,(Sort::QuickSort | Sort::Ascending));

       /*
       cout << "    ord=" << ord << endl;
       cout << "   nsep=" << nsep << endl;
       cout << "    sep=" << sep << " " << sep*(180.0/C::pi)<< endl;
       */

       // Trap case of duplication of nearest separation
       if (nsep>1 && sep(ord(1))==sep(ord(0)))
	 throw(AipsError("Found more than one field at minimum distance, can't decide!"));
       
       fseps(iMSFld)=sep(ord(0));
       fldMap_(iMSFld)=ctFlds(ord(0));
     }   
   }
   fseps*=(180.0/C::pi);
   LogIO log;
   ostringstream msg;
   msg << "Calibration field mapping for "
       << Path(ct_.tableName()).baseName().before(".tempMemCal")
       << " (via gainfield='nearest'): "
       << fldMap_ << endl
       << " Separations (deg): " << fseps;
   log << msg.str() << LogIO::POST;

   //   cout << ct_.tableName() << ": fldMap_ = " << fldMap_ << endl;
}   

void CTPatchedInterp::setFldMap(Vector<Int>& fldmap) {

  // Set the default spwmap first, then we'll ammend it
  setDefFldMap();

  Int nfld=fldmap.nelements();

  // Must specify no more than needed, but at least one
  AlwaysAssert(nfld>0,AipsError);
  AlwaysAssert(nfld<=nMSFld_,AipsError);

  // Discern _available_ fields in the CT
  ROCTColumns ctcol(ct_);
  Vector<Int> ctFlds;
  ctcol.fieldId().getColumn(ctFlds);
  Int nAvFlds=genSort(ctFlds,Sort::Ascending,(Sort::QuickSort | Sort::NoDuplicates));
  ctFlds.resize(nAvFlds,true);
  
  for (Int i=0;i<nfld;++i) {
    if (!anyEQ(ctFlds,fldmap(i)))
      throw(AipsError("Specified fldmap contains an unavailable field: "+String(fldmap(i))));
    else
      fldMap_(i)=fldmap(i);
  }
  if (nfld<nMSFld_)
  // Fill in the rest with last-specified
    fldMap_(Slice(nfld,nMSFld_-nfld,1))=fldMap_(nfld-1);


   LogIO log;
   ostringstream msg;
   msg << "Calibration field mapping for "
       << Path(ct_.tableName()).baseName().before(".tempMemCal")
       << " (via user specification): "
       << fldMap_;
   log << msg.str() << LogIO::POST;

}

// Calculate fldmap redundancy
void CTPatchedInterp::calcAltFld() {

  altFld_.resize(nMSFld_);
  for (Int iMSFld=0;iMSFld<nMSFld_;++iMSFld) {
     altFld_(iMSFld)=iMSFld;  // nominally
     for (Int ifld=0;ifld<iMSFld;++ifld) 
       if (fldMap_(ifld)==fldMap_(iMSFld))
         //altFld_(iMSFld)=ifld;  
	 altFld_(iMSFld)=altFld_(ifld);
  }
  /*
  cout << "------------" << endl;
  cout << "fldMap_ = " << fldMap_ << endl;
  cout << "altFld_ = " << altFld_ << endl;
  */
}



void CTPatchedInterp::setSpwMap(Vector<Int>& spwmap) {

  // Set the default spwmap first, then we'll ammend it
  setDefSpwMap();

  Int nspec=spwmap.nelements();

  // Do nothing, if nothing specified (and rely on default)
  if (nspec==0) return;

  // Do nothing f a single -1 is specified
  if (nspec==1 && spwmap(0)==-1) return;


  // Alert user if too many spws specified
  if (spwmap.nelements()>uInt(nMSSpw_))
    throw(AipsError("Specified spwmap has more elements ("+String::toString(spwmap.nelements())+") than the number of spectral windows in the MS ("+String::toString(nMSSpw_)+")."));
    
  // Handle auto-fanout
  if (spwmap(0)==-999) {
    // Use first OK spw for all MS spws
    Int gspw(0);
    while (!spwInOK(gspw)) ++gspw;
    spwMap_.set(gspw);
  }
  else {
    // First trap out-of-range values
    if (anyLT(spwmap,0))
      throw(AipsError("Please specify positive indices in spwmap."));
    if (anyGE(spwmap,nCTSpw_)) {
      ostringstream o;
      o << "Please specify spw indices <= maximum available ("
	<< (nCTSpw_-1) << " in " << Path(ct_.tableName()).baseName().before(".tempMemCal") << ")";
      throw(AipsError(o.str()));
    }

    // Now fill from spwmap
    if (nspec==1)
      // Use one value for all
      spwMap_.set(spwmap(0));
    else {
      // set as many as are specified
      IPosition blc(1,0);
      IPosition trc(1,min(nspec-1,nMSSpw_-1));
      spwMap_(blc,trc)=spwmap(blc,trc);
    }
  }

  //cout << "CTPatchedInterp::setSpwMap: Realized spwMap_ = " << spwMap_ << endl;

}


// Resample in frequency
void CTPatchedInterp::resampleInFreq(Matrix<Float>& fres,Matrix<Bool>& fflg,const Vector<Double>& fout,
				     Matrix<Float>& tres,Matrix<Bool>& tflg,const Vector<Double>& fin) {

  if (CTPATCHEDINTERPVERB) cout << "  CTPatchedInterp::resampleInFreq(...)" << endl;

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

    // Set flags carefully
    resampleFlagsInFreq(fflgi,fout,tflgi,fin);


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

    InterpolateArray1D<Double,Float>::interpolate(slfresi,slfout,mfin,mtresi,freqInterpMethod_);

  }
}

void CTPatchedInterp::resampleFlagsInFreq(Vector<Bool>& flgout,const Vector<Double>& fout,
					  Vector<Bool>& flgin,const Vector<Double>& fin) {

  //  cout << "resampleFlagsInFreq" << endl;

  Vector<Double> finGHz=fin/1e9;

  // Handle chan-dep flags
  if (freqTypeStr_.contains("flag")) {
    
    // Determine implied mode-dep flags indexed by channel registration
    uInt nflg=flgin.nelements();
    Vector<Bool> flreg(nflg,false);
    switch (freqInterpMethod_) {
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
      switch (freqInterpMethod_) {
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

void CTPatchedInterp::setElemMap() {
 
  // Ensure the antMap_ is set
  if (antMap_.nelements()!=uInt(nMSAnt_))
    setDefAntMap();

  // Handle cases
  switch(mtype_) {
  case VisCalEnum::GLOBAL: {

    throw(AipsError("CTPatchedInterp::sliceTable: No non-Mueller/Jones yet."));

    // There is only 1
    AlwaysAssert(nMSElem_==1,AipsError);
    elemMap_.resize(nMSElem_);
    elemMap_.set(0);

    break;
  }
  case VisCalEnum::MUELLER: {
    elemMap_.resize(nMSElem_);
    conjTab_.resize(nMSElem_);
    conjTab_.set(false);
    Int iMSElem(0),a1in(0),a2in(0);
    for (Int iMSAnt=0;iMSAnt<nMSAnt_;++iMSAnt) {
      a1in=antMap_(iMSAnt);
      for (Int jAntOut=iMSAnt;jAntOut<nMSAnt_;++jAntOut) {
	a2in=antMap_(jAntOut);
	if (a1in<=a2in)
	  elemMap_(iMSElem)=blnidx(a1in,a2in,nMSAnt_);
	else {
	  elemMap_(iMSElem)=blnidx(a2in,a1in,nMSAnt_);
	  conjTab_(iMSElem)=true;  // we must conjugate Complex params!
	}
	++iMSElem;
      } // jAntOut
    } // iMSAnt
    break;
  }    
  case VisCalEnum::JONES: {
    // Just reference the antMap_
    elemMap_.reference(antMap_);
    break;
  }
  } // switch
}


InterpolateArray1D<Double,Float>::InterpolationMethod CTPatchedInterp::ftype(String& strtype) {
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
