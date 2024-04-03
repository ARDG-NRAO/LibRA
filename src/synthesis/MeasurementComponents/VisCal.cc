//# VisCal.cc: Implementation of VisCal classes
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
//# $Id: VisCal.cc,v 1.15 2006/02/06 19:23:11 gmoellen Exp $

#include <synthesis/MeasurementComponents/VisCal.h>

#include <synthesis/MeasurementComponents/MSMetaInfoForCal.h>
#include <msvis/MSVis/VisBuffer.h>

#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/MaskArrMath.h>
#include <casacore/casa/Arrays/ArrayLogical.h>
#include <casacore/casa/Arrays/ArrayIter.h>
#include <casacore/casa/Arrays/MaskedArray.h>
#include <casacore/scimath/Mathematics/MatrixMathLA.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/Quanta/MVTime.h>
#include <casacore/casa/Quanta/Quantum.h>
#include <casacore/casa/Quanta/QuantumHolder.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/OS/Memory.h>

#include <sstream>

#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogSink.h>

#include <casacore/casa/Quanta/MVTime.h>

#define PRTLEV 0

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

// **********************************************************
//  VisCal Implementations
//

VisCal::VisCal(VisSet& vs) :
  msName_(vs.msName()),
  msmc_( new MSMetaInfoForCal(vs.msName()) ),  // a local one
  delmsmc_(True),                              // must be delete in dtor
  nSpw_(vs.numberSpw()),
  nAnt_(vs.numberAnt()),
  nBln_(0),
  currSpw_(0),
  currTime_(vs.numberSpw(),0.0),
  currScan_(vs.numberSpw(),-1),
  currObs_(vs.numberSpw(),-1),
  currField_(vs.numberSpw(),-1),
  currIntent_(vs.numberSpw(),-1),
  currFreq_(vs.numberSpw(),-1),
  lastTime_(vs.numberSpw(),0.0),
  refTime_(0.0),
  refFreq_(0.0),
  nChanPar_(vs.numberSpw(),1),
  nChanMat_(vs.numberSpw(),1),
  startChan_(vs.numberSpw(),0),
  interval_(0.0),
  applied_(false),
  V_(vs.numberSpw(),NULL),
  currCPar_(vs.numberSpw(),NULL),
  currRPar_(vs.numberSpw(),NULL),
  currParOK_(vs.numberSpw(),NULL),
  PValid_(vs.numberSpw(),false),
  calWt_(false),
  currWtScale_(vs.numberSpw(),NULL),
  prtlev_(PRTLEV),
  extratag_("")
{
  if (prtlev()>2) cout << "VC::VC(vs)" << endl;

  // Initialize
  initVisCal();
}

VisCal::VisCal(String msname,Int MSnAnt,Int MSnSpw) :
  msName_(msname),
  msmc_( new MSMetaInfoForCal(msname) ),       // a local one
  delmsmc_(True),                              // must be delete in dtor
  nSpw_(MSnSpw),
  nAnt_(MSnAnt),
  nBln_(0),
  currSpw_(0),
  currTime_(MSnSpw,0.0),
  currScan_(MSnSpw,-1),
  currObs_(MSnSpw,-1),
  currField_(MSnSpw,-1),
  currIntent_(MSnSpw,-1),
  currFreq_(MSnSpw,-1),
  lastTime_(MSnSpw,0.0),
  refTime_(0.0),
  refFreq_(0.0),
  nChanPar_(MSnSpw,1),
  nChanMat_(MSnSpw,1),
  startChan_(MSnSpw,0),
  interval_(0.0),
  applied_(false),
  V_(MSnSpw,NULL),
  currCPar_(MSnSpw,NULL),
  currRPar_(MSnSpw,NULL),
  currParOK_(MSnSpw,NULL),
  PValid_(MSnSpw,false),
  calWt_(false),
  currWtScale_(MSnSpw,NULL),
  prtlev_(PRTLEV),
  extratag_("")
{
  if (prtlev()>2) cout << "VC::VC(msname,MSnAnt,MSnSpw)" << endl;

  // Initialize
  initVisCal();
}

VisCal::VisCal(const MSMetaInfoForCal& msmc) :
  msName_(msmc.msname()),  // from the msmc (it is still used in some places)
  msmc_(&msmc),
  delmsmc_(False),   //  not local, don't delete
  nSpw_(msmc.nSpw()),
  nAnt_(msmc.nAnt()),
  nBln_(0),
  currSpw_(0),
  currTime_(nSpw_,0.0),
  currScan_(nSpw_,-1),
  currObs_(nSpw_,-1),
  currField_(nSpw_,-1),
  currIntent_(nSpw_,-1),
  currFreq_(nSpw_,-1),
  lastTime_(nSpw_,0.0),
  refTime_(0.0),
  refFreq_(0.0),
  nChanPar_(nSpw_,1),
  nChanMat_(nSpw_,1),
  startChan_(nSpw_,0),
  interval_(0.0),
  applied_(False),
  V_(nSpw_,NULL),
  currCPar_(nSpw_,NULL),
  currRPar_(nSpw_,NULL),
  currParOK_(nSpw_,NULL),
  PValid_(nSpw_,False),
  calWt_(False),
  currWtScale_(nSpw_,NULL),
  prtlev_(PRTLEV),
  extratag_("")
{
  if (prtlev()>2) cout << "VC::VC(MSMetaInfoForCal)" << endl;

  // Initialize
  initVisCal();
}

VisCal::VisCal(const Int& nAnt) :
  msName_(""),
  msmc_(NULL),
  delmsmc_(False),
  nSpw_(1),
  nAnt_(nAnt),
  nBln_(0),
  currSpw_(0),
  currTime_(1,0.0),
  currField_(1,-1),
  currIntent_(1,-1),
  currFreq_(1,-1),
  lastTime_(1,0.0),
  refTime_(0.0),
  refFreq_(0.0),
  nChanPar_(1,1),
  nChanMat_(1,1),
  startChan_(1,0),
  interval_(0.0),
  applied_(false),
  V_(1,NULL),
  currCPar_(1,NULL),
  currRPar_(1,NULL),
  currParOK_(1,NULL),
  PValid_(1,false),
  calWt_(false),
  currWtScale_(1,NULL),
  prtlev_(PRTLEV),
  extratag_("")
{
  if (prtlev()>2) cout << "VC::VC(i,j,k)" << endl;

  // Initialize
  initVisCal();
}

VisCal::~VisCal() {

  if (prtlev()>2) cout << "VC::~VC()" << endl;

  deleteVisCal();

  if (delmsmc_)
    delete msmc_;

}

void VisCal::setApply() {

  if (prtlev()>2) cout << "VC::setApply()" << endl;

  // This is the apply context
  setApplied(true);

  // Establish non-trivial paramter arrays
  for (Int ispw=0;ispw<nSpw();ispw++) {
    currSpw()=ispw;

    if (nChanPar()>0) {
      switch (parType()) {
      case VisCalEnum::COMPLEX: {
	currCPar().resize(nPar(),nChanPar(),nElem());
	currCPar()=Complex(1.0);
	break;
      } 
      case VisCalEnum::REAL: {
	currRPar().resize(nPar(),nChanPar(),nElem());
	currRPar()=1.0;
	break;
      }
      default:
	throw(AipsError("Parameters must be entirely Real or entirely Complex for now!"));
      }
      currParOK().resize(nPar(),nChanPar(),nElem());
    currParOK()=true;
    }
  }

}

void VisCal::setApply(const Record& apply) {
  //  Inputs:
  //    apply           Record&       Contains application params
  //    

  if (prtlev()>2) cout << "VC::setApply(apply)" << endl;

  // Extract calWt
  if (apply.isDefined("calwt"))
    calWt()=apply.asBool("calwt");

  // This is apply context  
  setApplied(true);

  // Initialize flag counting
  initCalFlagCount();


}

void VisCal::setCallib(const Record& callib,const MeasurementSet& /*selms*/) {

  // Forward to setApply
  VisCal::setApply(callib);

}


String VisCal::applyinfo() {

  ostringstream o;
  o << typeName() << " <pre-computed>";

  return String(o);

}

void VisCal::correct(VisBuffer& vb, Bool trial) {

  if (prtlev()>3) cout << "VC::correct(vb)" << endl;

  // Count pre-cal flags
  countInFlag(vb);

  // Call non-in-place version, in-place-wise:
  correct(vb,vb.visCube(),trial);

  // Count post-cal flags
  countOutFlag(vb);

}

void VisCal::correct2(vi::VisBuffer2& vb, Bool trial, Bool doWtSp, Bool dosync) {

  if (prtlev()>3) cout << "VC::correct2(vb)" << endl;


  // If calibration is unavailable, just return!
  if (!calAvailable(vb)) {
    return;
  }

  // Reaching here, calibration is available, so do it!

  // Count pre-cal flags
  countInFlag2(vb);

  // Bring calibration up-to-date w/ the vb
  if (dosync)
    syncCal2(vb,true);

  // Organize for weight calibration
  Cube<Float> wtcube;
  if (doWtSp) {
    // refer directly to weightSpectrum
    wtcube.reference(vb.weightSpectrum());
  }
  else {
    // refer to unchan'd weight matrix, rendered as Cube
    IPosition sh2=vb.weight().shape();
    IPosition sh3(3,sh2[0],1,sh2[1]);
    wtcube.reference(vb.weight().reform(sh3));
  }

  // Refer to corrected data
  Cube<Complex> vCC;
  vCC.reference(vb.visCubeCorrected());

  // Apply the calibration
  applyCal2(vb,vCC,wtcube,trial);

  // Count post-cal flags
  countOutFlag2(vb);

}

void VisCal::corrupt(VisBuffer& vb) {

  if (prtlev()>3) cout << "VC::corrupt(vb)" << endl;

  // Call non-in-place version, in-place-wise:
  corrupt(vb,vb.modelVisCube());
}

void VisCal::corrupt2(vi::VisBuffer2& vb) {

  if (prtlev()>3) cout << "VC::corrupt(vb2)" << endl;

  // If calibration is unavailable, just return!
  if (!calAvailable(vb)) {
    return;
  }

  // Call non-in-place version, in-place-wise:
  Cube<Complex> m;
  m.reference(vb.visCubeModel());  // access model non-const
  corrupt2(vb,m);
}

void VisCal::correct(VisBuffer& vb, Cube<Complex>& Vout, Bool trial) {

  if (prtlev()>3) cout << " VC::correct(vb,Vout)" << endl;
  
  // Prepare output Cube<Complex> for its own in-place apply:
  //  (this is a no-op if referencing same storage)
  Vout = vb.visCube();

  // Bring calibration up-to-date with the vb, 
  //   with inversion turned ON
  syncCal(vb,true);

  // Call generic row-by-row apply, with inversion turned ON
  applyCal(vb,Vout,trial);

}

// void VisCal::corrupt(VisBuffer& vb, Cube<Complex>& Mout) {
void VisCal::corrupt(VisBuffer& vb, Cube<Complex>& Mout) {

  if (prtlev()>3) cout << " VC::corrupt()" << endl;


  // Ensure weight calibration off internally for corrupt
  //   (corruption doesn't re-scale the data!)
  Bool userCalWt=calWt();
  calWt()=false;

  // Prepare output Cube<Complex> for its own in-place apply:
  //  (this is a no-op if referencing same storage)
  Mout = vb.modelVisCube();

  // Bring calibration up-to-date with the vb, 
  //   with inversion turned OFF
  syncCal(vb,false);

  // Call generic row-by-row apply, with inversion turned OFF
  applyCal(vb,Mout);

  // Restore user's calWt()
  calWt()=userCalWt;

}

void VisCal::corrupt2(vi::VisBuffer2& vb, Cube<Complex>& Mout) {

  if (prtlev()>3) cout << " VC::corrupt2(VB2)" << endl;


  // Ensure weight calibration off internally for corrupt
  //   (corruption doesn't re-scale the data!)
  Bool userCalWt=calWt();
  calWt()=False;

  // Prepare output Cube<Complex> for its own in-place apply:
  //  (this is a no-op if referencing same storage)
  Mout = vb.visCubeModel();

  // Bring calibration up-to-date with the vb, 
  //   with inversion turned OFF
  syncCal2(vb,False);

  // Call generic row-by-row apply, with inversion turned OFF
  Cube<Float> w0;  // place-holder
  applyCal2(vb,Mout,w0);

  // Restore user's calWt()
  calWt()=userCalWt;

}

// VI2-related refactorings-----------

void VisCal::setMeta(Int obs, Int scan, Double time,
		     Int spw, const Vector<Double>& freq,
		     Int fld) {

  // Call internal method
  syncMeta(spw,time,fld,freq,freq.nelements());	
  currScan()=scan;
  currObs()=obs;
  
}

// Setup solvePar shape for a spw
void VisCal::sizeApplyParCurrSpw(Int nVisChan) {

  // Sizes the solvePar arrays for the currSpw()
  
  if (prtlev()>3) cout << " VC::sizeApplyParCurrSpw()" << endl;

  // Use nVisChan only for freqDepPar() types
  Int nChan = ( freqDepPar() ? nVisChan : 1);

  // Keep old way informed (needed?)
  nChanPar()=nChan;

  // Now, size the arrays:

  IPosition parsh(3,nPar(),nChan,nElem());  // multi-chan
  switch (parType()) {
  case VisCalEnum::COMPLEX: {
    currCPar().resize(parsh);
    currCPar()=Complex(1.0);
    break;
  }
  case VisCalEnum::REAL: {
    currRPar().resize(parsh);
    currRPar()=0.0;
    break;
  }
  default:
    throw(AipsError("Internal error(Calibrater Module): Unsupported parameter type "
		    "COMPLEXREAL found in VC::sizeApplyParCurrSpw()"));
  }
    
  currParOK().resize(parsh);
  currParOK()=True;

}

void VisCal::setDefApplyParCurrSpw(Bool sync, Bool doInv) {

  // TBD: generalize for type-dep def values, etc.
  switch (parType()) {
  case VisCalEnum::COMPLEX: {
    AlwaysAssert(currCPar().nelements()>0,AipsError);
    currCPar().set(1.0);
    break;
  }
  case VisCalEnum::REAL: {
    AlwaysAssert(currRPar().nelements()>0,AipsError);
    currRPar().set(0.0);
    break;
  }
  default:
    throw(AipsError("Internal error(Calibrater Module): Unsupported parameter type "
		    "COMPLEXREAL found in VC::setDefApplyParCurrSpw()"));
  }

  currParOK().set(True);
  validateP();
  
  if (sync)
    syncCal(doInv);

}

// Set parameters to specified values in the currSpw(),
//   and optionally sync matrices
void VisCal::setApplyParCurrSpw(const Cube<Complex> cpar,
				bool sync, bool doInv) {

  switch (parType()) {
  case VisCalEnum::COMPLEX: {
    AlwaysAssert(currCPar().nelements()>0,AipsError);
    AlwaysAssert(currCPar().shape()==cpar.shape(),AipsError);  // shapes must match
    currCPar()=cpar;
    break;
  }
  case VisCalEnum::REAL: {
    throw(AipsError("Cannot set real parameters with complext values!"));
    break;
  }
  default:
    throw(AipsError("Internal error(Calibrater Module): Unsupported parameter type "
		    "COMPLEXREAL found in VC::setDefApplyParCurrSpw()"));
  }

  currParOK().set(True);
  validateP();
  
  // Synchronize matrices
  if (sync)
    syncCal(doInv);

}

void VisCal::setApplyParCurrSpw(const Cube<float> rpar,
				bool sync, bool doInv) {

  switch (parType()) {
  case VisCalEnum::REAL: {
    AlwaysAssert(currRPar().nelements()>0,AipsError);
    AlwaysAssert(currRPar().shape()==rpar.shape(),AipsError);  // shapes must match
    currRPar()=rpar;
    break;
  }
  case VisCalEnum::COMPLEX: {
    throw(AipsError("Cannot set complex parameters with real values!"));
    break;
  }
  default:
    throw(AipsError("Internal error(Calibrater Module): Unsupported parameter type "
		    "COMPLEXREAL found in VC::setDefApplyParCurrSpw()"));
  }

  currParOK().set(True);
  validateP();
  
  // Synchronize matrices
  if (sync)
    syncCal(doInv);

}


void VisCal:: initCalFlagCount() {
  ndataIn_=nflagIn_=nflagOut_=0;
}

Record VisCal::actionRec() {
  Record cf;
  cf.define("type",typeName());
  if (isApplied()) {
    cf.define("ndata",ndataIn_);
    cf.define("nflagIn",nflagIn_);
    cf.define("nflagOut",nflagOut_);
  }
  return cf;
}


void VisCal::state() {

  cout << "===========================================================" << endl;
  if (prtlev()>3) cout << "VC::state():" << endl;
  cout << boolalpha;
  cout << " Type=" << typeName() << " Enum=" << type() << endl;
  cout << "  nSpw= " << nSpw() 
       << "; nAnt= " << nAnt()
       << "; nBln= " << nBln()
       << "; nPar= " << nPar() 
       << "; nElem= " << nElem()
       << "; nCalMat= " << nCalMat() << endl;
  cout << "  isApplied= " << isApplied()
       << ";  isSolvable= " << isSolvable() << endl;
  cout << "  freqDepPar= " << freqDepPar()
       << ";  freqDepMat= " << freqDepMat()
       << ";  timeDepMat= " << timeDepMat() << endl;
  cout << "  interval= " << interval() << endl;
  cout << "  currSpw= " << currSpw() << endl;
  cout << "  nChanPar= " << nChanPar()
       << "; nChanMat= " << nChanMat()
       << "; startChan= " << startChan() << endl;
  cout << "  lastTime= " << lastTime()
       << "; currTime= " << currTime()
       << "; refTime= " << refTime() << endl;
  cout << "  refFreq= " << refFreq() << endl;
  cout << "  currField=" << currField() << endl;
  cout << "  currIntent=" << currIntent() << endl;
  cout << "  PValid() = " << PValid() << endl;
  cout << "  currCPar().shape() = " << currCPar().shape()
       << " (" << currCPar().data() << ")" << endl;
  cout << "  currRPar().shape() = " << currRPar().shape()
       << " (" << currRPar().data() << ")" << endl;
  cout << "  currParOK().shape() = " << currParOK().shape()
       << " (" << currParOK().data() << ") "
       << " (ntrue=" << ntrue(currParOK()) << ")" << endl;

  cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -" << endl;

}

void VisCal::currMetaNote() {

  cout << "   ("
       << "time=" << MVTime(refTime()/C::day).string(MVTime::YMD,7)
       << " field=" << currField()
       << " spw=" << currSpw()
       << ")"
       << endl;

}

// VisCal PROTECTED:


void VisCal::countInFlag(const VisBuffer& vb) {
  Int ncorr=vb.nCorr();
  ndataIn_+=(vb.flag().nelements()*ncorr);
  nflagIn_+=(ntrue(vb.flag())*ncorr);
}
void VisCal::countInFlag2(const vi::VisBuffer2& vb) {
  ndataIn_+=(vb.flagCube().nelements());
  nflagIn_+=(ntrue(vb.flagCube()));
}

void VisCal::countOutFlag(const VisBuffer& vb){
  nflagOut_+=ntrue(vb.flag())*vb.nCorr(); 
}

void VisCal::countOutFlag2(const vi::VisBuffer2& vb){
  nflagOut_+=ntrue(vb.flagCube());
}

void VisCal::syncCal(const VisBuffer& vb,
		     const Bool& doInv) {
  
  if (prtlev()>4) cout << "   VC::syncCal(vb)" << endl;

  // Set current meta date from the VisBuffer
  syncMeta(vb);

  // Check the current calibration for validity
  checkCurrCal();

  // Procede with generalized sync of calibration
  syncCal(doInv);

}

void VisCal::syncCal2(const vi::VisBuffer2& vb,
		      const Bool& doInv) {
  
  if (prtlev()>4) cout << "   VC::syncCal2(vb)" << endl;

  // Set current meta date from the VisBuffer
  syncMeta2(vb);

  // Check the current calibration for validity
  checkCurrCal();

  // Procede with generalized sync of calibration
  syncCal(doInv);


}


void VisCal::syncCal(VisCal& vc) {
  
  if (prtlev()>4) cout << "   VC::syncCal(vc)" << endl;

  //  cout << "    VisCal::syncCal(VisCal): " << currCPar().data() 
  //       << endl;

  // Set current meta date from the VisCal
  syncMeta(vc);

  // Check the current calibration for validity
  checkCurrCal();

  // Procede with generalized sync of calibration
  syncCal(false);

  //  cout << "    VisCal::syncCal(VisCal): " << currCPar().data() 
  //       << endl;

}


void VisCal::syncMeta(const VisBuffer& vb) {
  
  if (prtlev()>4) cout << "    VC::syncMeta(vb)" << endl;
 
  syncMeta(vb.spectralWindow(),
	   vb.time()(0),
	   vb.fieldId(),
	   vb.frequency(),
	   vb.nChannel());

  currObs()=vb.observationId()(0);


  // Ensure VisVector for data acces has correct form
  Int ncorr(vb.corrType().nelements());
  if (V().type() != ncorr)
    V().setType(visType(ncorr));

}

void VisCal::syncMeta2(const vi::VisBuffer2& vb) {
  
  if (prtlev()>4) cout << "    VC::syncMeta2(vb)" << endl;
 
  syncMeta(vb.spectralWindows()(0),
	   vb.time()(0),
	   vb.fieldId()(0),
	   vb.getFrequencies(0),
	   vb.nChannels());

  currScan()=vb.scan()(0);
  currObs()=vb.observationId()(0);
  currIntent()=vb.stateId()(0);

  // Ensure VisVector for data acces has correct form
  Int ncorr(vb.nCorrelations());
  if (V().type() != ncorr)
    V().setType(visType(ncorr));

}

void VisCal::syncMeta(VisCal& vc) {
  
  if (prtlev()>4) cout << "    VC::syncMeta(vc)" << endl;


  syncMeta(vc.currSpw(),
	   vc.currTime(),
	   vc.currField(),
	   vc.currFreq(),
	   vc.nChanMat());  // the number of channels to be applied to

  currObs()=vc.currObs();


  // TBD: Ensure nElem matches (here?)

}

void VisCal::syncMeta(const Int& spw,
		      const Double& time,
		      const Int& field,
		      const Vector<Double>& freq,
		      const Int& nchan) {

  if (prtlev()>4) cout << "     VC::syncMeta(...)" << endl;

  // Remember which spw this is---EVERYTHING below pivots on this
  currSpw()=spw;

  // Remember other meta-data
  currTime()=time;
  currField()=field;

  currFreq().resize(); 
  currFreq()=freq;
  currFreq()/=1.0e9;  // In GHz

  // Use center channel for now
  refFreq()=currFreq()(nchan/2);

  // Ensure cal matrix channelization is correct vis-a-vis the target channelization
  //  (in solve context, this is handled differently, outside this method)
  if (isApplied()) setCalChannelization(nchan);

  if (prtlev()>5) cout << "      meta: t,fld,freq=" << time << field << freq << endl;
}

void VisCal::setCalChannelization(const Int& nChanDat) {

  if (prtlev()>4) cout << "     VC::setCalChannelization()" << endl;

  // This method sets up cal Matrix channelization for the current 
  //  spw according to the channel count supplied by the caller.
  //  (Presumably the data channelization)

  // Ensure matrix channel axis length is correct
  if (freqDepMat())
    
    // If parameters are channelized
    if (freqDepPar())
      // follow parameter channelization
      //  (e.g., B)
      //      nChanMat() = nChanPar();  
      nChanMat() = nChanDat;   // NEWCALTABLE
    else
      // cal is f(freq) for each data channel:
      //  (e.g., fringe-fitting)
      nChanMat() = nChanDat;

  else {
    // Matrices are single channel
    //  (e.g., G)
    nChanMat()=1;

    //    cout << "nChanPar() = " << nChanPar() << endl;

    // So must be parameters:
    AlwaysAssert((!freqDepPar()),AipsError);
    // NCT    AlwaysAssert((nChanPar()==1),AipsError);

  }

}

void VisCal::checkCurrCal() {
  // Based on meta-data changes, determine mainly if
  //  new calibration PARAMETERS should be sought
  //  NB: A finding of "true" does not necessarily 
  //   mean new pars will be found by CalInterp (we are
  //   here only forcing it to try)
  //  NB: this method may also invalidateCalMat(), to
  //   trigger matrix reformation, independent of whether
  //   new pars are found ultimately found by CalInterp

  if (prtlev()>4) cout << "    VC::checkCurrCal: " << endl;

  // We potentially need new calibration (for currSpw) IF...

  // ...timestamp has changed
  if (currTime() != lastTime() ) {
    lastTime()=currTime();
    invalidateP();  // merely signals need to check (doesn't break reference)

    // If matrices are time-dependent within solution interval,
    //   a time change ALWAYS invalidates them
    //   (i.e., PValid=F may not yield _new_ parameters in
    //    calcPar, but this forces matrix re-sync anyway)
    if (timeDepMat()) invalidateCalMat();

    //if (prtlev()>5 and timeDepMat()) cout << "      invalidateCalMat() " << endl;

  }

}

void VisCal::syncCal(const Bool& doInv) {

  if (prtlev()>4) cout << "    VC::syncCal(doInv)" << endl;

  //  cout << "     VisCal::syncCal(doInv): " << currCPar().data() 
  //       << endl;

  // Synchronize the parameters
  syncPar();

  //  cout << "     VisCal::syncCal(doInv): " << currCPar().data() 
  //       << endl;

  if (!PValid()) 
    // TBD:  Improve this message (with some meta data)
    throw(AipsError("Could not find any calibration parameters."));

  // Synchronize the matrices
  syncCalMat(doInv);

  //  cout << "     VisCal::syncCal(doInv): " << currCPar().data()
  //       << endl;

}



void VisCal::syncPar() {

  if (prtlev()>5) cout << "     VC::syncPar()  (PValid()=" 
		       << PValid() << ")" << endl;

  // If we require new parameters, calculate them by some means (e.g., interpolation):
  if (!PValid()) calcPar();
    
}

void VisCal::calcPar() {

  if (prtlev()>6) cout << "      VC::calcPar()" << endl;

  // Ensure we have some parameters
  if (parType()==VisCalEnum::COMPLEX && (currCPar().shape()!=IPosition(3,nPar(),nChanPar(),nElem()) ||
				currParOK().shape()!=IPosition(3,nPar(),nChanPar(),nElem())) ) {
    cout << "currCPar()   = " << currCPar() << endl;
    cout << "currParOK() = " << currParOK() << endl;
    throw(AipsError("No (complex) parameters available!"));
  }
  else if (parType()==VisCalEnum::REAL && (currRPar().shape()!=IPosition(3,nPar(),nChanPar(),nElem()) ||
				     currParOK().shape()!=IPosition(3,nPar(),nChanPar(),nElem())) ) {
    cout << "currRPar()   = " << currRPar() << endl;
    cout << "currParOK() = " << currParOK() << endl;
    throw(AipsError("No (real) parameters available!"));
  }
  else if (parType()==VisCalEnum::COMPLEXREAL)
    throw(AipsError("We can't handle combined Real and Complex parameters yet."));
  else
    // Parameters appear to be available, so signal validation
    validateP();
  
  // Force subsequent matrix calculation
  //  (Specializations will only do this if actual _new_ parameters
  //   have been found)
  invalidateCalMat();

}


// VisCal PRIVATE method implementations........................... 

void VisCal::initVisCal() {

  if (prtlev()>2) cout << " VC::initVisCal()" << endl;

  // Number of baselines (including ACs)
  nBln_ = nAnt_*(nAnt_+1)/2;

  for (Int ispw=0;ispw<nSpw(); ispw++) {
    currCPar_[ispw] = new Cube<Complex>();
    currRPar_[ispw] = new Cube<Float>();
    currParOK_[ispw] = new Cube<Bool>();
    V_[ispw] = new VisVector(VisVector::Four);
    currWtScale_[ispw] = new Cube<Float>();
  }

}

void VisCal::deleteVisCal() {

  if (prtlev()>2) cout << " VC::deleteVisCal()" << endl;

  for (Int ispw=0; ispw<nSpw(); ispw++) {
    if (currCPar_[ispw])     delete currCPar_[ispw];
    if (currRPar_[ispw])     delete currRPar_[ispw];
    if (currParOK_[ispw])    delete currParOK_[ispw];
    if (V_[ispw]) delete V_[ispw];
    if (currWtScale_[ispw]) delete currWtScale_[ispw];
  }
  currCPar_=NULL;
  currRPar_=NULL;
  currParOK_=NULL;
  V_=NULL;
  currWtScale_=NULL;
}

void VisCal::setCurrField(const Int& ifld) {
  currField_.set(ifld);
}

// **********************************************************
//  VisMueller Implementations
//

VisMueller::VisMueller(VisSet& vs) :
  VisCal(vs),
  M_(vs.numberSpw(),NULL),
  currMElem_(vs.numberSpw(),NULL),
  currMElemOK_(vs.numberSpw(),NULL),
  MValid_(vs.numberSpw(),false)
{

  if (prtlev()>2) cout << "VM::VM(vs)" << endl;

  initVisMueller();
}

VisMueller::VisMueller(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),
  M_(MSnSpw,NULL),
  currMElem_(MSnSpw,NULL),
  currMElemOK_(MSnSpw,NULL),
  MValid_(MSnSpw,false)
{

  if (prtlev()>2) cout << "VM::VM(msname,MSnAnt,MSnSpw)" << endl;

  initVisMueller();
}

VisMueller::VisMueller(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),
  M_(nSpw(),NULL),
  currMElem_(nSpw(),NULL),
  currMElemOK_(nSpw(),NULL),
  MValid_(nSpw(),False)
{

  if (prtlev()>2) cout << "VM::VM(MSMetaInfoForCal)" << endl;

  initVisMueller();
}


VisMueller::VisMueller(const Int& nAnt) :
  VisCal(nAnt),
  M_(1,NULL),
  currMElem_(1,NULL),
  currMElemOK_(1,NULL),
  MValid_(1,false)
{
  if (prtlev()>2) cout << "VM::VM(i,j,k)" << endl;

  initVisMueller();
}

VisMueller::~VisMueller() {
  if (prtlev()>2) cout << "VM::~VM()" << endl;

  deleteVisMueller();
}

// Report the VisMueller portion of the state
void VisMueller::state() {

  // Call parent
  VisCal::state();

  if (applyByMueller()) {
    if (prtlev()>3) cout << "VM::state()" << endl;
    cout << boolalpha;
    //    cout << "This is an baseline-based (Mueller) calibration." << endl;
    cout << "  applyByMueller=" << applyByMueller() << endl;
    cout << "  muellerType= " << muellerType() << endl;
    cout << "  trivialMuellerElem= " << trivialMuellerElem() << endl;
    cout << "  MValid() = " << MValid() << endl;

    cout << "  currMElem().shape() = " << currMElem().shape()
	 << " (" << currMElem().data() << ")" << endl;
    cout << "  currMElemOK().shape() = " << currMElemOK().shape()
	 << " (" << currMElemOK().data() << ") "
	 << " (ntrue=" << ntrue(currMElemOK()) << ")" << endl;


    cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -" << endl;
  }
}

// Apply this calibration to VisBuffer visibilities
void VisMueller::applyCal(VisBuffer& vb, Cube<Complex>& Vout,
			  Bool trial) {

  if (prtlev()>3) cout << "  VM::applyCal()" << endl;

  // CURRENTLY ASSUMES ONLY *ONE* TIMESTAMP IN VISBUFFER
  /*
  cout << "VM::applyCal: type= " << typeName() << "  trial = " 
       << boolalpha << trial << " calWt = " << calWt() 
       << "  freqDepPar() = " << freqDepPar()
       << "  freqDepMat() = " << freqDepMat()
       << endl;
  */

  // Data info/indices
  Int* dataChan;
  Bool* flagR=&vb.flagRow()(0);
  Bool* flag=&vb.flag()(0,0);
  Int* a1=&vb.antenna1()(0);
  Int* a2=&vb.antenna2()(0);
  Matrix<Float> wtmat;

  // Access to weights
  if (calWt() && !trial) 
    wtmat.reference(vb.weightMat());

  ArrayIterator<Float> wt(wtmat,1);
  Vector<Float> wtvec;

  if (V().type()==VisVector::One) {
    M().setScalarData(true);
  }
  else
    M().setScalarData(false);

  // iterate rows
  Int& nRow(vb.nRow());
  Int& nChanDat(vb.nChannel());
  Int ibln;
  for (Int row=0; row<nRow; row++,flagR++,a1++,a2++) {
    
    // The basline number
    ibln=blnidx(*a1,*a2);
    
    // Solution channel registration
    Int solCh0(0);
    dataChan=&vb.channel()(0);
    
    // If cal _parameters_ are not freqDep (e.g., a delay)
    //  the startChan() should be the same as the first data channel
    if (freqDepMat() && !freqDepPar())
      startChan()=(*dataChan);
    
    // Solution and data array registration
    M().sync(currMElem()(0,solCh0,ibln),currMElemOK()(0,solCh0,ibln));
    if (!trial)
      V().sync(Vout(0,0,row));
    
    
    for (Int chn=0; chn<nChanDat; chn++,flag++,V()++,dataChan++) {
      
      if (trial) 
	M().applyFlag(*flag);
      else 
	// data and solution ok, do the apply
	M().apply(V(),*flag);

      // inc soln ch axis if freq-dependent 
      if (freqDepMat()) 
	M()++; 

    } // chn

    // If requested update the weights
    if (calWt() && !trial) {
      wtvec.reference(wt.array());
      updateWt(wtvec,*a1,*a2);
    }

    if (calWt() && !trial)
      wt.next();

  }

}

// Apply this calibration to VisBuffer visibilities
void VisMueller::applyCal2(vi::VisBuffer2& vb, 
			   Cube<Complex>& Vout, Cube<Float>& Wout,
			   Bool trial) {

  if (prtlev()>3) cout << "  VM::applyCal()" << endl;

  // CURRENTLY ASSUMES ONLY *ONE* TIMESTAMP IN VISBUFFER
  /*
  cout << "VM::applyCal: type= " << typeName() << "  trial = " 
       << boolalpha << trial << " calWt = " << calWt() 
       << "  freqDepPar() = " << freqDepPar()
       << "  freqDepMat() = " << freqDepMat()
       << endl;
  */

  Vector<Bool> flagRv(vb.flagRow());
  Vector<Int>  a1v(vb.antenna1());
  Vector<Int>  a2v(vb.antenna2());
  Cube<Bool> flagCube(vb.flagCube());
  Cube<Complex> visCube(Vout);
  ArrayIterator<Float> wt(Wout,2);
  Matrix<Float> wtmat;

  // Data info/indices
  Int* dataChan;
  Bool* flagR=&flagRv(0);
  Int* a1=&a1v(0);
  Int* a2=&a2v(0);

  // Ensure VisVector for data acces has correct form
  Int ncorr(vb.nCorrelations());
  if (V().type() != ncorr)
    V().setType(visType(ncorr));

  if (V().type()==VisVector::One) {
    M().setScalarData(true);
  }
  else
    M().setScalarData(false);

  // iterate rows
  Int nRow=vb.nRows();
  Int nChanDat=vb.nChannels();
  Int ibln;
  Vector<Int> dataChanv(vb.getChannelNumbers(0));  // All rows have same chans
  for (Int row=0; row<nRow; row++,flagR++,a1++,a2++) {
    
    // The basline number
    ibln=blnidx(*a1,*a2);
    
    // Solution channel registration
    Int solCh0(0);
    dataChan=&dataChanv(0);
    
    // If cal _parameters_ are not freqDep (e.g., a delay)
    //  the startChan() should be the same as the first data channel
    if (freqDepMat() && !freqDepPar())
      startChan()=(*dataChan);
    
    // Solution and data array registration
    M().sync(currMElem()(0,solCh0,ibln),currMElemOK()(0,solCh0,ibln));
    V().sync(visCube(0,0,row),flagCube(0,0,row));
    
    for (Int chn=0; chn<nChanDat; chn++,V()++,dataChan++) {
      
      if (trial) 
	M().flag(V());
      else 
	// data and solution ok, do the apply
	M().apply(V());

      // inc soln ch axis if freq-dependent 
      if (freqDepMat()) 
	M()++; 

    } // chn

    // If requested update the weights
    /*
    if (calWt() && !trial) {
      wtvec.reference(wt.array());
      updateWt(wtvec,*a1,*a2);
    }

    if (calWt() && !trial)
      wt.next();
    */
  }

}

void VisMueller::syncCalMat(const Bool& doInv) {

  if (prtlev()>5) cout << "     VM::syncCalMat()" 
		       << " (MValid()=" << MValid() << ")" << endl;

  // If Mueller matrices now invalid, re-sync them
  if (!MValid()) syncMueller(doInv);

}

void VisMueller::syncMueller(const Bool& doInv) {

  // RI
  //prtlev()=10;

  if (prtlev()>6) cout << "      VM::syncMueller()" << endl;

  // If Mueller pars are just the matrix elements:
  if (trivialMuellerElem()) {
    // Matrix Elem cache references par cache
    currMElem().reference(currCPar());
    currMElemOK().reference(currParOK());

    // EXCEPT, ensure uniqueness if taking the inverse
    //   (this makes a copy, can we avoid?)
    if (doInv) currMElem().unique();
  }
  else {
    // Make local matrix element cache the correct size:
    currMElem().resize(muellerNPar(this->muellerType()),nChanMat(),nCalMat());
    currMElem().unique();    // Ensure uniqueness!

    // OK is the shape of the M matrix itself
    currMElemOK().resize(muellerNPar(this->muellerType()),nChanMat(),nCalMat());
    currMElemOK().unique();
    currMElemOK()=false;
    
    // The matrix state is invalid until we actually calculate them
    invalidateM();

    // And calculate the matrix elements for all baselines
    calcAllMueller();

  }

  // weight calibration
  if (calWt()) syncWtScale();

  // Ensure Mueller matrix renderer is correct
  createMueller();

  // Invert, if requested
  if (doInv) invMueller();

  // Set matrix elements by their ok flags
  setMatByOk();

  // Mueller matrices now valid
  validateM();

}

// Calculate Mueller matrices for all baselines
void VisMueller::calcAllMueller() {

  if (prtlev()>6) cout << "       VM::calcAllMueller" << endl;

  // Should handle OK flags in this method, and only
  //  do Mueller calc if OK

  Vector<Complex> oneMueller;
  Vector<Bool> oneMOK;
  Vector<Complex> onePar;
  Vector<Bool> onePOK;

  ArrayIterator<Complex> Miter(currMElem(),1);
  ArrayIterator<Bool>    MOKiter(currMElemOK(),1);
  ArrayIterator<Complex> Piter(currCPar(),1);
  ArrayIterator<Bool>    POKiter(currParOK(),1);
  
  // All required baselines
  for (Int ibln=0; ibln<nCalMat(); ibln++) {

    // The following assumes that nChanPar()=1 or nChanMat()

    for (Int ich=0; ich<nChanMat(); ich++) {

      oneMueller.reference(Miter.array());
      oneMOK.reference(MOKiter.array());
      onePar.reference(Piter.array());
      onePOK.reference(POKiter.array());
      
      // TBD  What if calcOneMueller needs freq value info?
      
      calcOneMueller(oneMueller,oneMOK,onePar,onePOK);

      // Advance iterators, as required
      Miter.next();
      MOKiter.next();
      if (freqDepPar()) {
	Piter.next();
	POKiter.next();
      }

    }

    // Step to next baseline's pars if we didn't in channel loop
    if (!freqDepPar()) {
      Piter.next();
      POKiter.next();
    }
  }

}

void VisMueller::calcOneMueller(Vector<Complex>& /*mat*/, Vector<Bool>& /*mOk*/,
				const Vector<Complex>& /*par*/, const Vector<Bool>& /*pOk*/) {

  if (prtlev()>10) cout << "        VM::calcOneMueller()" << endl;

  // If Mueller matrix is trivial, shouldn't get here
  if (trivialMuellerElem()) 
    throw(AipsError("Trivial Mueller Matrix logic error."));

  // Otherwise, this method apparently hasn't been specialized, as required
  else
    throw(AipsError("Unknown non-trivial Mueller-from-parameter calculation requested."));

}

void VisMueller::invMueller() {

  // This method only called in apply context!
  AlwaysAssert((isApplied()),AipsError);

  if (prtlev()>6) cout << "       VM::invMueller()" << endl;

  M().sync(currMElem()(0,0,0),currMElemOK()(0,0,0));
  for (Int ibln=0;ibln<nCalMat();ibln++) 
    for (Int ichan=0; ichan<nChanMat(); ++ichan, M()++) 
      // If matrix elements look ok so far, attempt to invert
      //  (if invert fails, matrix is zeroed and meOk is set accordingly)
      M().invert();

}

void VisMueller::setMatByOk() {

  // This method only called in apply context!
  AlwaysAssert((isApplied()),AipsError);

  if (prtlev()>6) 
    cout << "       VM::setMatByOk()" << endl;

  M().sync(currMElem()(0,0,0),currMElemOK()(0,0,0));
  for (Int ibln=0;ibln<nCalMat();ibln++) 
    for (Int ichan=0; ichan<nChanMat(); ++ichan, M()++) 
      // If matrix elements look ok so far, attempt to invert
      //  (if invert fails, matrix is zeroed and meOk is set accordingly)
      M().setMatByOk();

}

void VisMueller::createMueller() {

  if (prtlev()>6) cout << "       VM::createMueller()" << endl;

  // Delete current Mueller if wrong type
  Mueller::MuellerType mtype(this->muellerType());
  if (M_[currSpw()] &&
      M().type() != mtype)
    delete M_[currSpw()];
  
  // If needed, construct the correct Mueller
  if (!M_[currSpw()])
    M_[currSpw()] = casa::createMueller(mtype);


  // Nominal synchronization is with currMElem()(0,0,0);
  M().sync(currMElem()(0,0,0),currMElemOK()(0,0,0));

  if (prtlev()>6) cout << "       currMElem()(0,0,0) = " << currMElem()(0,0,0) << endl;
  if (prtlev()>6) cout << "       currMElem()(0,0,1) = " << currMElem()(0,0,1) << endl;

      
}

void VisMueller::syncWtScale() {

  // Ensure proper size according to Mueller matrix type
  switch (this->muellerType()) {
  case Mueller::Scalar: 
  case Mueller::Diagonal: {
    Int nWtCorr=muellerNPar(muellerType());
    currWtScale().resize(nWtCorr,1,nBln());
    break;
  }
  default: {
    // Only diag and scalar versions can adjust weights
    //    cout<< "Turning off calWt()" << endl;
    calWt()=false;
    return;
    break;
  }
  }

  // Calculate the weight scaling
  calcWtScale();
}


void VisMueller::calcWtScale() {

  // Assumes currMElem hasn't yet been inverted

  // Insist on single channel operation
  AlwaysAssert( (nChanMat()==1), AipsError );

  Cube<Float> cWS;
  cWS.reference(currWtScale());

  // Simple pre-inversion square of currMElem
  cWS=real(currMElem()*conj(currMElem()));
  cWS(!currMElemOK())=1.0;

}

void VisMueller::updateWt(Vector<Float>& wt,const Int& a1,const Int& a2) {

  // Assumes single channel
  Vector<Float> ws(currWtScale().xyPlane(blnidx(a1,a2)).column(0));

  switch (V().type()) {
  case VisVector::One: {
    wt(0)*=ws(0);
  }
  case VisVector::Two: {
    for (uInt ip=0;ip<2;++ip) 
      wt(ip)*=ws(ip*3);
  }
  default: {
  for (uInt ip=0;ip<wt.nelements();++ip)
    wt(ip)*=ws(ip);
  }
  }

}


void VisMueller::initVisMueller() {

  if (prtlev()>2) cout << " VM::initVisMueller()" << endl;

  for (Int ispw=0;ispw<nSpw(); ispw++) {
    currMElem_[ispw] = new Cube<Complex>();
    currMElemOK_[ispw] = new Cube<Bool>();
  }
}

void VisMueller::deleteVisMueller() {

  if (prtlev()>2) cout << " VM::deleteVisMueller()" << endl;

  for (Int ispw=0; ispw<nSpw(); ispw++) {
    if (currMElem_[ispw])   delete currMElem_[ispw];
    if (currMElemOK_[ispw]) delete currMElemOK_[ispw];
    if (M_[ispw]) delete M_[ispw];
  }
  currMElem_=NULL;
  currMElemOK_=NULL;
  M_=NULL;
}




// **********************************************************
//  VisJones Implementations
//

VisJones::VisJones(VisSet& vs) :
  VisCal(vs), VisMueller(vs),
  J1_(vs.numberSpw(),NULL),
  J2_(vs.numberSpw(),NULL),
  currJElem_(vs.numberSpw(),NULL),
  currJElemOK_(vs.numberSpw(),NULL),
  JValid_(vs.numberSpw(),false)
{
  if (prtlev()>2) cout << "VJ::VJ(vs)" << endl;

  initVisJones();
}

VisJones::VisJones(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw), 
  VisMueller(msname,MSnAnt,MSnSpw),
  J1_(MSnSpw,NULL),
  J2_(MSnSpw,NULL),
  currJElem_(MSnSpw,NULL),
  currJElemOK_(MSnSpw,NULL),
  JValid_(MSnSpw,false)
{
  if (prtlev()>2) cout << "VJ::VJ(msname,MSnAnt,MSnSpw)" << endl;

  initVisJones();
}


VisJones::VisJones(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),
  VisMueller(msmc),
  J1_(nSpw(),NULL),
  J2_(nSpw(),NULL),
  currJElem_(nSpw(),NULL),
  currJElemOK_(nSpw(),NULL),
  JValid_(nSpw(),False)
{
  if (prtlev()>2) cout << "VJ::VJ(MSMetaInfoForCal)" << endl;

  initVisJones();
}

VisJones::VisJones(const Int& nAnt) :
  VisCal(nAnt), 
  VisMueller(nAnt),
  J1_(1,NULL),
  J2_(1,NULL),
  currJElem_(1,NULL),
  currJElemOK_(1,NULL),
  JValid_(1,false)
{
  if (prtlev()>2) cout << "VJ::VJ(i,j,k)" << endl;

  initVisJones();
}

VisJones::~VisJones() {
  if (prtlev()>2) cout << "VJ::~VJ()" << endl;

  deleteVisJones();
}

Mueller::MuellerType VisJones::muellerType() {

  // Ask Mueller to give us the answer:
  return casa::muellerType(jonesType(),V().type());
  
}

void VisJones::state() {

  // Call parent
  VisMueller::state();

  if (applyByJones()) {
    if (prtlev()>3) cout << "VJ::state()" << endl;
    cout << boolalpha;
    //    cout << "This is an antenna-based (Jones) calibration." << endl;
    cout << "  applyByJones=" << applyByJones() << endl;
    cout << "  jonesType= " << jonesType() << endl;
    cout << "  trivialJonesElem= " << trivialJonesElem() << endl;
    cout << "  JValid() = " << JValid() << endl;

    cout << "  currJElem().shape() = " << currJElem().shape()
	 << " (" << currJElem().data() << ")" << endl;
    cout << "  currJElemOK().shape() = " << currJElemOK().shape()
	 << " (" << currJElemOK().data() << ") "
	 << " (ntrue=" << ntrue(currJElemOK()) << ")" << endl;

    cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -" << endl;
  }
}

// VisJones: PROTECTED methods


// Apply this calibration to VisBuffer visibilities
void VisJones::applyCal(VisBuffer& vb, Cube<Complex>& Vout,
			Bool trial) {

  if (prtlev()>3) cout << "  VJ::applyCal()" << endl;

  // CURRENTLY ASSUMES ONLY *ONE* TIMESTAMP IN VISBUFFER

  /*
  cout << "VJ::applyCal: type= " << typeName() << "  trial = " 
       << boolalpha << trial << " calWt = " << calWt() 
       << "  freqDepPar() = " << freqDepPar()
       << "  freqDepMat() = " << freqDepMat()
       << endl;
  */

  if (applyByMueller()) 
    VisMueller::applyCal(vb,Vout);
  else {

    // TBD: applyByJones()=true necessarily

    // Data info/indices
    Int* dataChan;
    Bool* flagR=&vb.flagRow()(0);
    Bool* flag=&vb.flag()(0,0);
    Int* a1=&vb.antenna1()(0);
    Int* a2=&vb.antenna2()(0);
    Matrix<Float> wtmat;

    // Prepare to cal weights
    if (!trial)
      wtmat.reference(vb.weightMat());
  
    ArrayIterator<Float> wt(wtmat,1);
    Vector<Float> wtvec;

    // Alert Jones matrices to whether data is scalar or not
    //  (this is relevant only for proper handling of flags
    //   in case of scalar data, for now)
    if (V().type()==VisVector::One) {
      J1().setScalarData(true);
      J2().setScalarData(true);
    }
    else {
      J1().setScalarData(false);
      J2().setScalarData(false);
    }

    // iterate rows
    Int& nRow(vb.nRow());
    Int& nChanDat(vb.nChannel());
    //    cout << currSpw() << " startChan() = " << startChan() << " nChanMat() = " << nChanMat() << " nChanDat="<<nChanDat <<endl;
    for (Int row=0; row<nRow; row++,flagR++,a1++,a2++) {
      
      // Solution channel registration
      Int solCh0(0);
      dataChan=&vb.channel()(0);
      
      // If cal _parameters_ are not freqDep (e.g., a delay)
      //  the startChan() should be the same as the first data channel
      if (freqDepMat() && !freqDepPar())
	startChan()=(*dataChan);

      // Solution and data array registration
      J1().sync(currJElem()(0,solCh0,*a1),currJElemOK()(0,solCh0,*a1));
      J2().sync(currJElem()(0,solCh0,*a2),currJElemOK()(0,solCh0,*a2));
      if (!trial)
	V().sync(Vout(0,0,row));


      for (Int chn=0; chn<nChanDat; chn++,flag++,V()++,dataChan++) {
	  
	if (trial) {
	  // only update flag info
	  J1().applyFlag(*flag);
	  J2().applyFlag(*flag);
	}
	else {
	  // if this data channel unflagged
	  J1().applyRight(V(),*flag);
	  J2().applyLeft(V(),*flag);
	}
	  
	// inc soln ch axis if freq-dependent (and next dataChan within soln)
	if (freqDepMat()) {
	  J1()++; 
	  J2()++; 
	}
	  
      } // chn
	

      // If requested, update the weights
      if (!trial && calWt()) {
	wtvec.reference(wt.array());
	updateWt(wtvec,*a1,*a2);
      }

      if (!trial)
	wt.next();

    }
    
  }

}

// Apply this calibration to VisBuffer visibilities
void VisJones::applyCal2(vi::VisBuffer2& vb, 
			 Cube<Complex>& Vout, Cube<Float>& Wout,
			 Bool trial) {

  if (prtlev()>3) cout << "  VJ::applyCal()" << endl;

  // CURRENTLY ASSUMES ONLY *ONE* TIMESTAMP IN VISBUFFER

  /*
  cout << "VJ::applyCal: type= " << typeName() << "  trial = " 
       << boolalpha << trial << " calWt = " << calWt() 
       << "  freqDepPar() = " << freqDepPar()
       << "  freqDepMat() = " << freqDepMat()
       << endl;
  */

  if (applyByMueller()) 
    throw(AipsError("applyByMueller call from VisJones::applyCal2 NYI."));
  //    VisMueller::applyCal(vb,Vout);
  else {

    // TBD: applyByJones()=true necessarily

    // References to VB2's contents' _data_
    Vector<Bool> flagRv(vb.flagRow());
    Vector<Int>  a1v(vb.antenna1());
    Vector<Int>  a2v(vb.antenna2());
    Cube<Bool> flagCube(vb.flagCube());
    Cube<Complex> visCube(Vout);
    ArrayIterator<Float> wt(Wout,2);
    Matrix<Float> wtmat;

    // Data info/indices
    Int* dataChan;
    Bool* flagR=&flagRv(0);
    Int* a1=&a1v(0);
    Int* a2=&a2v(0);

    // Ensure VisVector for data acces has correct form
    Int ncorr(vb.nCorrelations());
    if (V().type() != ncorr)
      V().setType(visType(ncorr));


    // Alert Jones matrices to whether data is scalar or not
    //  (this is relevant only for proper handling of flags
    //   in case of scalar data, for now)
    if (V().type()==VisVector::One) {
      J1().setScalarData(true);
      J2().setScalarData(true);
    }
    else {
      J1().setScalarData(false);
      J2().setScalarData(false);
    }

    // iterate rows
    Int nRow=vb.nRows();
    Int nChanDat=vb.nChannels();
    Vector<Int> dataChanv(vb.getChannelNumbers(0));  // All rows have same chans
    //    cout << currSpw() << " startChan() = " << startChan() << " nChanMat() = " << nChanMat() << " nChanDat="<<nChanDat <<endl;
    for (Int row=0; row<nRow; row++,flagR++,a1++,a2++) {
      
      // Solution channel registration
      Int solCh0(0);
      dataChan=&dataChanv(0);
      
      // If cal _parameters_ are not freqDep (e.g., a delay)
      //  the startChan() should be the same as the first data channel
      if (freqDepMat() && !freqDepPar())
	startChan()=(*dataChan);

      // Solution and data array registration
      J1().sync(currJElem()(0,solCh0,*a1),currJElemOK()(0,solCh0,*a1));
      J2().sync(currJElem()(0,solCh0,*a2),currJElemOK()(0,solCh0,*a2));
      V().sync(visCube(0,0,row),flagCube(0,0,row));

      for (Int chn=0; chn<nChanDat; chn++,V()++,dataChan++) {
	  
	if (trial) {
	  // only update flag info
	  J1().flagRight(V());
	  J2().flagLeft(V());
	}
	else {
	  // if this data channel unflagged
	  J1().applyRight(V());
	  J2().applyLeft(V());
	}
	  
	// inc soln ch axis if freq-dependent (and next dataChan within soln)
	if (freqDepMat()) {
	  J1()++; 
	  J2()++; 
	}
	  
      } // chn
	

      // If requested, update the weights
      if (!trial && calWt()) {
	wtmat.reference(wt.array());
	updateWt2(wtmat,*a1,*a2);
      }

      if (!trial)
	wt.next();

    }
    
  }

}


void VisJones::syncCalMat(const Bool& doInv) {

  if (prtlev()>5) cout << "     VJ::syncCalMat()"
		       << " (JValid()=" << JValid() << ")" << endl;

  //  cout << "      VisCal::syncCalMat(doInv): " << currCPar().data() <<" "<< currJElem().data()
  //       << endl;

  // If necessary, synchronize the Jones matrices
  if (!JValid()) syncJones(doInv);

  //  cout << "      VisCal::syncCalMat(doInv): " << currCPar().data() <<" "<< currJElem().data()
  //       << endl;

  // Be sure sync'd matrices at their origin 
  J1().origin();
  J2().origin();

  // If requested and necessary, synchronize the Mueller matrices
  //   (NEVER invert Muellers, as Jones already have been)
  if (applyByMueller() && !MValid()) syncMueller(false);

}


void VisJones::syncJones(const Bool& doInv) {

  if (prtlev()>6) cout << "      VJ::syncJones()" << endl;

  // If Jones pars are just the matrix elements:
  if (trivialJonesElem()) {
    
    // Matrix Elem cache references par cache
    currJElem().reference(currCPar());
    currJElemOK().reference(currParOK());

    // EXCEPT, ensure uniqueness if taking the inverse
    //   (this makes a copy, can we avoid?)
    if (doInv) {
      //cout << "  Unique: " << currJElem().data() << "-->";
      currJElem().unique();
      //cout << currJElem().data() << endl;
    }
  }
  else {

    // Make local matrix element cache the correct size:
    currJElem().resize(jonesNPar(jonesType()),nChanMat(),nAnt());
    currJElem().unique();    // Ensure uniqueness!

    // OK matches size of the J matrix itself
    currJElemOK().resize(jonesNPar(jonesType()),nChanMat(),nAnt());
    currJElem().unique();    // Ensure uniqueness!
    currJElem()=false;

    // The matrix state is invalid until we actually calculate them
    invalidateJ();

    // And calculate the matrix elements for all baselines
    calcAllJones();

  }

  /*
  if (type()==VisCal::B) {
    cout << typeName() << ": currJElem().shape() = " << currJElem().shape() << endl;
    cout << "currJElem() = " << currJElem() << endl;
    cout << "currJElemOK() = " << currJElemOK() << endl;
    cout << "freqDepMat() = " << boolalpha << freqDepMat() << endl;
  }
  */

  // Pre-inv syncWtScale:
  if (calWt()) syncWtScale();

  // Ensure Jones Matrix renders are ok
  this->createJones();

  // Invert, if requested
  if (doInv) invJones();

  // Set matrix elements according to OK flags
  setMatByOk();

  // Jones matrices now valid
  validateJ();
  invalidateM();   // still invalid

}

void VisJones::calcAllJones() {

  if (prtlev()>6) cout << "       VJ::calcAllJones()" << endl;

  // Should handle OK flags in this method, and only
  //  do Jones calc if OK

  Vector<Complex> oneJones;
  Vector<Bool> oneJOK;
  Vector<Complex> onePar;
  Vector<Bool> onePOK;

  ArrayIterator<Complex> Jiter(currJElem(),1);
  ArrayIterator<Bool>    JOKiter(currJElemOK(),1);
  ArrayIterator<Complex> Piter(currCPar(),1);
  ArrayIterator<Bool>    POKiter(currParOK(),1);
  
  for (Int iant=0; iant<nAnt(); iant++) {

    // The following assumes that nChanPar()=1 or nChanMat()

    for (Int ich=0; ich<nChanMat(); ich++) {
      
      oneJones.reference(Jiter.array());
      oneJOK.reference(JOKiter.array());
      onePar.reference(Piter.array());
      onePOK.reference(POKiter.array());

      // Calculate the Jones matrix
      calcOneJones(oneJones,oneJOK,onePar,onePOK);
      
      // Advance iterators
      Jiter.next();
      JOKiter.next();
      if (freqDepPar()) {
	Piter.next();
	POKiter.next();
      }

    }
    // Step to next antenns's pars if we didn't in channel loop
    if (!freqDepPar()) {
      Piter.next();
      POKiter.next();
    }
  }
}

void VisJones::calcOneJones(Vector<Complex>& /*mat*/, Vector<Bool>& /*mOk*/,
			    const Vector<Complex>& /*par*/, const Vector<Bool>& /*pOk*/ ) {

  if (prtlev()>10) cout << "        VJ::calcOneJones()" << endl;

  // If Jones matrices are trivial, should never reach here!
  if (trivialJonesElem())
    throw(AipsError("Trivial Jones Matrix logic error."));

  // Otherwise, this method apparently hasn't been specialized, as required
  else
    throw(AipsError("Unknown non-trivial Jones-from-parameter calculation requested."));

}

void VisJones::invJones() {

  if (prtlev()>6) cout << "       VJ::invJones()" << endl;

  J1().sync(currJElem()(0,0,0),currJElemOK()(0,0,0));
  for (Int iant=0;iant<nAnt();iant++) 
    for (Int ichan=0; ichan<nChanMat();++ichan,J1()++) 
      // If matrix elements look ok so far, attempt to invert
      //  (if invert fails, currJElemOK will be set accordingly)
      J1().invert();

}

void VisJones::setMatByOk() {

  if (prtlev()>6) 
    cout << "       VJ::setMatByOk" << endl;

  J1().sync(currJElem()(0,0,0),currJElemOK()(0,0,0));
  for (Int iant=0;iant<nAnt();iant++) 
    for (Int ichan=0; ichan<nChanMat();++ichan,J1()++) 
      // If matrix elements look ok so far, attempt to invert
      //  (if invert fails, currJElemOK will be set accordingly)
      J1().setMatByOk();

}

void VisJones::calcAllMueller() {
  
  if (prtlev()>6) cout << "       VJ::calcAllMueller()" << endl;

  M().sync(currMElem()(0,0,0));
  for (Int a1=0;a1<nAnt();++a1) {
    J1().sync(currJElem()(0,0,a1),currJElemOK()(0,0,a1));
    for (Int a2=a1;a2<nAnt();++a2) {
      J2().sync(currJElem()(0,0,a2),currJElemOK()(0,0,a2));
      for (Int ich=0;ich<nChanMat();ich++,J1()++,J2()++,M()++)
	M().fromJones(J1(),J2());
    }
  }

}
 
void VisJones::createJones() {

  if (prtlev()>6) cout << "       VJ::createJones()" << endl;

  // Delete current Jones if wrong type
  Jones::JonesType jtype(jonesType());

  if (J1_[currSpw()] &&
      J1().type() != jtype)
    delete J1_[currSpw()];

  if (J2_[currSpw()] &&
      J2().type() != jtype)
    delete J2_[currSpw()];
  
  // If needed, construct the correct Jones
  if (!J1_[currSpw()])
    J1_[currSpw()] = casa::createJones(jtype);

  if (!J2_[currSpw()])
    J2_[currSpw()] = casa::createJones(jtype);
      

  // Nominal synchronization is with currJElem()(0,0,0):
  J1().sync(currJElem()(0,0,0),currJElemOK()(0,0,0));
  J2().sync(currJElem()(0,0,0),currJElemOK()(0,0,0));

}

void VisJones::syncWtScale() {

  //  cout << "VJ::syncWtScale (" << typeName() << ")" << endl;


  // Ensure proper size according to Jones matrix type
  switch (this->jonesType()) {
  case Jones::Scalar: 
  case Jones::Diagonal: {
    Int nWtScale=jonesNPar(jonesType());
    currWtScale().resize(nWtScale,1,nAnt());
    break;
  }
  default: {
    // Only diag and scalar versions can adjust weights
    //    cout<< "Turning off calWt()" << endl;
    calWt()=false;
    return;
    break;
  }
  }

   // Calculate the weight scale factors (specializable)
  calcWtScale();


  //  cout << "VJ::syncWtScale: currWtScale() = " << currWtScale() << endl;

}

void VisJones::calcWtScale() {

  // Assumes currJElem has not yet been inverted!

  // Insist on single channel operation
  AlwaysAssert( (nChanMat()==1), AipsError );

  // Just a reference
  Cube<Float> cWS(currWtScale());

  // We use simple (pre-inversion) square of currJElem
  cWS=real(currJElem()*conj(currJElem()));
  cWS(!currJElemOK())=1.0;

}

void VisJones::updateWt(Vector<Float>& wt,const Int& a1,const Int& a2) {

  Vector<Float> ws1(currWtScale().xyPlane(a1).column(0));
  Vector<Float> ws2(currWtScale().xyPlane(a2).column(0));

/*
  if (a1==0 && a2==1) {
    cout << "jonestype() = " << jonesType() 
	 << " jonesNPar(jonesType()) = " << jonesNPar(jonesType()) 
	 << " nPar() = " << nPar() 
	 << endl;
    cout << currSpw() << " "
	 << a1 << " "
	 << a2 << " "
	 << wt << " "
	 << ws1 << " "
	 << ws2 << " "
	 << ws1(0/nPar()) << " "
	 << ws2(0%nPar()) << " ";
  }
*/

  switch(jonesType()) {
  case Jones::Scalar: {
    // pol-indep corrections very simple; all correlations
    //  corrected by same value
    Float ws=(ws1(0)*ws2(0));
    wt*=ws;
    break;
  }
  case Jones::Diagonal: {
    switch (V().type()) {
    case VisVector::Two: {
      for (uInt ip=0;ip<2;++ip) 
	wt(ip)*=(ws1(ip)*ws2(ip));
      break;
    }
    default: {
      // NB: This always will apply the first weight scale info the a single corr
      for (uInt ip=0;ip<wt.nelements();++ip) {
	wt(ip)*=ws1(ip/2);
	wt(ip)*=ws2(ip%2);
      }
      break;
    }
    }
    break;
  }
  default:
    // We don't calibrate weights for General Jones matrices (yet)
    break;
  }

  /*
  if (a1==0 && a2==1)
    cout << " ---> " << wt << endl;
  */
}

// WEIGHT_SPECTRUM-capable version
void VisJones::updateWt2(Matrix<Float>& wt,const Int& a1,const Int& a2) {

  Int nVco=wt.shape()[0];  // ==nChanDat()?
  Int nVch=wt.shape()[1];  // ==nCorrDat()?

  Matrix<Float> ws1(currWtScale().xyPlane(a1));
  Matrix<Float> ws2(currWtScale().xyPlane(a2));

  Int nCch=ws1.shape()[1];  //  ==nChanMat()?

  // Channel counts either idential, or single-chan calibration
  //  alwaysAssert( nVch==nCch || nCch==1, AipsError);

  switch(jonesType()) {
  case Jones::Scalar: {
    // pol-indep corrections very simple; all correlations
    //  corrected by same value, per channel
    for (Int ich=0;ich<nVch;++ich) {
      Int iCch=ich%nCch;  // 0 or ich
      Float ws=(ws1(0,iCch)*ws2(0,iCch));
      Matrix<Float> wtm(wt(Slice(),Slice(ich,1,1)));
      wtm*=ws;
    }
    break;
  }
  case Jones::Diagonal: {

    Int nCorrPerPol=max(nVco/2,1);  // >=1
    for (Int ich=0;ich<nVch;++ich) {
      Int iCch=ich%nCch;  // 0 or ich
      for (Int ico=0;ico<nVco;++ico) 
	wt(ico,ich) *= ( ws1(ico/nCorrPerPol,iCch) *
			 ws2(ico%2,iCch) );
    }
    break;
  }
  default:
    // We don't calibrate weights for General Jones matrices (yet)
    break;
  }

}

void VisJones::initVisJones() {

  if (prtlev()>2) cout << " VJ::initVisJones()" << endl;

  for (Int ispw=0;ispw<nSpw(); ispw++) {
    currJElem_[ispw] = new Cube<Complex>();
    currJElemOK_[ispw] = new Cube<Bool>();
  }
}

void VisJones::deleteVisJones() {
  
  if (prtlev()>2) cout << " VJ::deleteVisJones()" << endl;

  for (Int ispw=0; ispw<nSpw(); ispw++) {
    if (currJElem_[ispw])   delete currJElem_[ispw];   
    if (currJElemOK_[ispw]) delete currJElemOK_[ispw]; 
    if (J1_[ispw])          delete J1_[ispw];
    if (J2_[ispw])          delete J2_[ispw];

  }
  currJElem_=NULL;
  currJElemOK_=NULL;
  J1_=NULL;
  J2_=NULL;
}

} //# NAMESPACE CASA - END

