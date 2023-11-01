//# AMueller.cc: Implementation of AMueller
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

#include <synthesis/CalTables/CalDescColumns.h>
#include <synthesis/CalTables/CTColumns.h>
#include <synthesis/CalTables/CTMainColumns.h>
//#include <ms/MeasurementSets/MSColumns.h>
#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/VisBuffGroupAcc.h>
#include <msvis/MSVis/VBContinuumSubtractor.h>
#include <synthesis/MeasurementComponents/CalCorruptor.h>
#include <synthesis/MeasurementComponents/AMueller.h>
#include <synthesis/MeasurementEquations/VisEquation.h>

#include <msvis/MSVis/VisBuffer2.h>


using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN


// **********************************************************
//  AMueller
//


AMueller::AMueller(VisSet& vs) :
  VisCal(vs),             // virtual base
  VisMueller(vs),         // virtual base
  MMueller(vs),            // immediate parent
  fitorder_p(0),
  doSub_p(true),
  nCorr_p(-1)
{
  if (prtlev()>2) cout << "A::A(vs)" << endl;

  init();
}

AMueller::AMueller(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),             // virtual base
  VisMueller(msname,MSnAnt,MSnSpw),         // virtual base
  MMueller(msname,MSnAnt,MSnSpw),            // immediate parent
  fitorder_p(0),
  doSub_p(true),
  nCorr_p(-1)
{
  if (prtlev()>2) cout << "A::A(msname,MSnAnt,MSnSpw)" << endl;

  init();
}


AMueller::AMueller(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),             // virtual base
  VisMueller(msmc),         // virtual base
  MMueller(msmc),            // immediate parent
  fitorder_p(0),
  doSub_p(true),
  nCorr_p(-1)
{
  if (prtlev()>2) cout << "A::A(msmc)" << endl;

  init();
}


AMueller::AMueller(const Int& nAnt) :
  VisCal(nAnt),
  VisMueller(nAnt),
  MMueller(nAnt),            // immediate parent
  fitorder_p(0),
  doSub_p(true),
  nCorr_p(-1)
{
  if (prtlev()>2) cout << "A::A(nAnt)" << endl;

  init();
}

AMueller::~AMueller() {
  if (prtlev()>2) cout << "A::~A()" << endl;
}

void AMueller::init()
{
  lofreq_p.resize(nSpw());
  hifreq_p.resize(nSpw());
  totnumchan_p.resize(nSpw());
  spwApplied_p.resize(nSpw());
  
  lofreq_p = -1.0;
  hifreq_p = -1.0;
  totnumchan_p = 0;
  spwApplied_p = false;
}

void AMueller::setSolve(const Record& solvepar) {

  // Call parent
  MMueller::setSolve(solvepar);

  // Extract the AMueller-specific solve parameters
  if(solvepar.isDefined("fitorder"))
    fitorder_p = solvepar.asInt("fitorder");

  nChanParList() = fitorder_p + 1;  // Orders masquerade as output chans.

  // Override preavg 
  // (solver will fail if we don't average completely in each solint)
  preavg()=DBL_MAX;
}

void AMueller::setSolveChannelization(VisSet& vs)
{
  Vector<Int> startDatChan(vs.startChan());

  // If fitorder_p != 0, this is frequency dependent.
  if(fitorder_p){
    // AMueller keeps its polynomial orders where channels would normally go,
    // and typically (advisedly) the number of orders is << the number of data
    // channels.  *Otherwise* the overall par shape follows the data shape.
    nChanParList() = fitorder_p + 1;  // Deja vu from setSolve().
    startChanList() = startDatChan;

    // However, for solving, we will only consider one channel at a time:
    nChanMatList() = 1;
  }
  else {
    // Pars are not themselves channel-dependent
    nChanParList() = 1;

    // Check if matrices may still be freq-dep:
    if (freqDepMat()) {
      // cal is an explicit f(freq) (e.g., like delay)
      nChanMatList()  = vs.numberChan();
      startChanList() = startDatChan;
    } else {
      // cal has no freq dep at all
      nChanMatList()  = Vector<Int>(nSpw(),1);
      startChanList() = Vector<Int>(nSpw(),0);
    }
  }

  // At this point:
  //  1. nChanParList() represents the number of coefficients per polynomial,
  //     appropriate for shaping the CalSet.
  //  2. nChanMatList() represents the per-Spw matrix channel axis length to
  //     be used during the solve, independent of the parameter channel
  //     axis length.  In the solve context, nChanMat()>1 when there is
  //     more than one channel of data upon which the (single channel)
  //     solve parameters depend (e.g. polynomial order != 1)
}

Int AMueller::sizeUpSolve(VisSet& vs, Vector<Int>& nChunkPerSol)
{
  sortVisSet(vs);
  VisIter& vi(vs.iter());

  nCorr_p = fitorder_p ? vi.nCorr() : 2;

  // Would SolvableVisCal be better here?  After all the need for this
  // specialization started with MMueller::nPar()...
  return MMueller::sizeUpSolve(vs, nChunkPerSol);
}

void AMueller::selfSolveOne(VisBuffGroupAcc& vbga)
{
  // Solver for the polynomial continuum fit.  It is overkill for fitorder_p ==
  // 0, and not used in that case.

  LogIO os(LogOrigin("AMueller", "selfSolveOne()", WHERE));
  VBContinuumSubtractor vbcs;

  // Initialize
  if(lofreq_p[currSpw()] < 0.0){  // 1st time for this spw, so let vbga
    vbcs.initFromVBGA(vbga);      // provide the info.
    lofreq_p[currSpw()] = vbcs.getLowFreq();
    hifreq_p[currSpw()] = vbcs.getHighFreq();
    totnumchan_p[currSpw()] = vbcs.getTotNumChan();
  }
  else                            // Reuse the prev vals for consistency.
    vbcs.init(solveParOK().shape(), nAnt() - 1, totnumchan_p[currSpw()],
              lofreq_p[currSpw()], hifreq_p[currSpw()]);

  vbcs.fit(vbga, fitorder_p, MS::DATA, solveCPar(), solveParOK(),
           false, false, !append());
}

void AMueller::storeNCT() {

  // Update the freq info
  if(fitorder_p != 0){    // Store lofreq_p[currSpw()] and hifreq_p[currSpw()]

    // Access SPW subtable
    CTColumns ncc(*ct_);

    // We are expecting nSpw rows...
    AlwaysAssert( Int(ncc.spectralWindow().chanFreq().nrow())==nSpw(), AipsError);

    // We store the freq domain as a pair of values in chanFreq(),
    //  and reset numChan()=fitorder_p

    Vector<Double> freqrange(2);
    for (Int ispw=0;ispw<nSpw();++ispw) {
      // Only if values are ok...
      if (lofreq_p[ispw]>0.0 &&
	  hifreq_p[ispw]>0.0) {
	freqrange(0)=lofreq_p[ispw];
	freqrange(1)=hifreq_p[ispw];
	ncc.spectralWindow().chanFreq().setShape(ispw,IPosition(1,2));
	ncc.spectralWindow().chanFreq().put(ispw,freqrange);
	ncc.spectralWindow().numChan().put(ispw,fitorder_p+1);
	ncc.spectralWindow().flagRow().put(ispw,false);
      }
      else
	// Mark this spw flagged (no solutions in main
	ncc.spectralWindow().flagRow().put(ispw,true);
    }
  }

  // Now call conventional store
  MMueller::storeNCT();

}

void AMueller::hurl(const String& origin, const String& msg)
{
  LogIO os(LogOrigin("AMueller", origin, WHERE));
  
  os << msg << LogIO::EXCEPTION;
}

void AMueller::setApply(const Record& applypar)
{
  LogIO os(LogOrigin("AMueller", "setApply()", WHERE));
  
  if(applypar.isDefined("table")){
    calTableName() = applypar.asString("table");
    verifyCalTable(calTableName());

    {
      // TBD: Improve this kluge!
      //  e.g., fitorder from a keyword, etc.
      NewCalTable ct0(calTableName(),Table::Old,Table::Plain);
      ROCTMainColumns ncmc(ct0);
      IPosition sh=ncmc.cparam().shape(0);
      nCorr_p = sh(0);
      fitorder_p= sh(1)-1;
      nChanParList().set(sh(1));
    }
  }
  else
    os << "AMueller::setApply(Record&) needs the record to have a table"
       << LogIO::EXCEPTION;


  // Call parent to get the general stuff
  MMueller::setApply(applypar);

  // Now get some uvcontsub-specific stuff

  // Extract freq domain for non-trivial fits
  if(fitorder_p != 0){

    // Access to SPW subtable..
    ROCTColumns ncc(*ct_);
    Vector<Bool> spwflrow;
    ncc.spectralWindow().flagRow().getColumn(spwflrow);
    Int nspw=spwflrow.nelements();

    // TBD: What if caltable contains more or less than nSpw() (from MS) spws?

    lofreq_p.resize(nspw);
    hifreq_p.resize(nspw);

    Vector<Double> freqrange(2);
    if (spwMap()(0)==-999) {  // auto-fan-out
      Int ispw=0;
      while (spwflrow(ispw)) ++ispw;  // find first good spw
      ncc.spectralWindow().chanFreq().get(ispw,freqrange);
      lofreq_p.set(freqrange(0));
      hifreq_p.set(freqrange(1));
    }
    else {  // fill lo/hifreq per spw (only good ones)
      for (Int ispw=0;ispw<nspw;++ispw) {
	if (!spwflrow(ispw)) {
	  ncc.spectralWindow().chanFreq().get(ispw,freqrange);
	  lofreq_p[ispw]=freqrange(0);
	  hifreq_p[ispw]=freqrange(1);
	}
      }
    }

  }

}

// Apply this calibration to VisBuffer visibilities
void AMueller::applyCal(VisBuffer& vb, Cube<Complex>& Vout,Bool trial)
{
  LogIO os(LogOrigin("AMueller", "applyCal()", WHERE));

  if(fitorder_p == 0){
    VisMueller::applyCal(vb, Vout, trial);
  }
  else{
    if(prtlev() > 3)
      os << "  AMueller::applyCal()" << LogIO::POST;

    if (trial)
      throw(AipsError("trial apply not supported by AMueller with fitorder_p>0"));
    
    Int cspw = currSpw();
    VBContinuumSubtractor vbcs;

    if (lofreq_p[cspw]>0.0 &&
	hifreq_p[cspw]>0.0)
      vbcs.init(currCPar().shape(), nAnt() - 1, totnumchan_p[cspw],
		lofreq_p[cspw], hifreq_p[cspw]);
    else
      throw(AipsError("AMueller::applyCal: Bad freq domain info."));

    // Correct DATA (will write out to CORRECTED_DATA later)
    MS::PredefinedColumns whichcol = MS::DATA;
    if(!vbcs.apply(vb, whichcol, currCPar(), currParOK(), doSub_p,
                   !spwApplied_p[cspw]))
      throw(AipsError("Could not place the continuum-subtracted data in "
                      + MS::columnName(whichcol)));
    spwApplied_p[cspw] = true;
  }
}


// Apply this calibration to VisBuffer visibilities
void AMueller::applyCal2(vi::VisBuffer2& vb, 
                         casacore::Cube<casacore::Complex>& Vout,casacore::Cube<casacore::Float>& Wout,
                         casacore::Bool trial)

{
  LogIO os(LogOrigin("AMueller", "applyCal2()", WHERE));

  if(fitorder_p == 0){
    VisMueller::applyCal2(vb, Vout, Wout, trial);
  }
  else{
    if(prtlev() > 3)
      os << "  AMueller::applyCal2()" << LogIO::POST;

    if (trial)
      throw(AipsError("trial apply not supported by AMueller with fitorder_p>0"));
    
    Int cspw = currSpw();
    VBContinuumSubtractor vbcs;

    if (lofreq_p[cspw]>0.0 &&
	hifreq_p[cspw]>0.0)
      vbcs.init(currCPar().shape(), nAnt() - 1, totnumchan_p[cspw],
		lofreq_p[cspw], hifreq_p[cspw]);
    else
      throw(AipsError("AMueller::applyCal2: Bad freq domain info."));

    // Correct Vout
    if(!vbcs.apply2(vb, Vout, currCPar(), currParOK(), doSub_p,
                   !spwApplied_p[cspw]))
      throw(AipsError("Error applying continuum-subtraction"));

    spwApplied_p[cspw] = true;
  }
}

void AMueller::corrupt(VisBuffer& vb)
{
  LogIO os(LogOrigin("AMueller", "corrupt()", WHERE));

  if(prtlev() > 3)
    os << LogIO::NORMAL << "  A::corrupt()" << LogIO::POST;

  // Initialize model data to zero, so corruption contains
  //  only the AMueller solution
  //  TBD: may wish to make this user togglable.
  vb.setModelVisCube(Complex(0.0));

  if(fitorder_p == 0){
    // Call general version:
    VisMueller::corrupt(vb);
  }
  else{
    // Ensure weight calibration off internally for corrupt
    //   (corruption doesn't re-scale the data!)
    Bool userCalWt=calWt();
    calWt()=false;

    // Bring calibration up-to-date with the vb, with inversion turned OFF
    syncCal(vb,false);

    Int cspw = currSpw();
    VBContinuumSubtractor vbcs;
    if (lofreq_p[cspw]>0.0 &&
	hifreq_p[cspw]>0.0)
      vbcs.init(currCPar().shape(), nAnt() - 1, totnumchan_p[cspw],
		lofreq_p[cspw], hifreq_p[cspw]);
    else
      throw(AipsError("AMueller::applyCal: Bad freq domain info."));
    
    MS::PredefinedColumns whichcol = MS::MODEL_DATA;
    if(!vbcs.apply(vb, whichcol, currCPar(), currParOK(), false,
                   !spwApplied_p[cspw]))
      throw(AipsError("Could not place the continuum estimate in "
                      + MS::columnName(whichcol)));
    spwApplied_p[cspw] = true;
    // Restore user's calWt()
    calWt()=userCalWt; 
  }
}

void ANoise::createCorruptor(const VisIter& vi, const Record& simpar, const Int nSim)
{
  if (prtlev()>2) cout << " AN::createCorruptor()" << endl;
  AlwaysAssert((isSimulated()),AipsError);

  acorruptor_p = new ANoiseCorruptor();
  corruptor_p = acorruptor_p;

  // call generic parent to set corr,spw,etc info
  SolvableVisCal::createCorruptor(vi,simpar,nSim);

  Int Seed(1234);
  if (simpar.isDefined("seed")) {    
    Seed=simpar.asInt("seed");
  }

  Float Amp(1.0);
  if (simpar.isDefined("amplitude")) {    
    Amp=simpar.asFloat("amplitude");
  }

  acorruptor_p->initialize(Seed,Amp);

  String Mode("calc"); // calc means multiply by 1/sqrt(dnu dt)
  if (simpar.isDefined("mode")) {    
    Mode=simpar.asString("mode");
  }

  acorruptor_p->mode()=Mode;

  if (prtlev()>2) cout << " ~AN::createCorruptor()" << endl;

}


ANoise::ANoise(VisSet& vs) :
  VisCal(vs),             // virtual base
  VisMueller(vs),         // virtual base
  SolvableVisMueller(vs)  // immediate parent
{
  if (prtlev()>2) cout << "ANoise::ANoise(vs)" << endl;
}

ANoise::ANoise(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),             // virtual base
  VisMueller(msname,MSnAnt,MSnSpw),         // virtual base
  SolvableVisMueller(msname,MSnAnt,MSnSpw)  // immediate parent
{
  if (prtlev()>2) cout << "ANoise::ANoise(msname,MSnAnt,MSnSpw)" << endl;
}

ANoise::ANoise(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),             // virtual base
  VisMueller(msmc),         // virtual base
  SolvableVisMueller(msmc)  // immediate parent
{
  if (prtlev()>2) cout << "ANoise::ANoise(msmc)" << endl;
}

ANoise::ANoise(const Int& nAnt) :
  VisCal(nAnt),
  VisMueller(nAnt),
  SolvableVisMueller(nAnt)
{
  if (prtlev()>2) cout << "ANoise::ANoise(nAnt)" << endl;
}

ANoise::~ANoise() {
  if (prtlev()>2) cout << "ANoise::~ANoise()" << endl;
}


// Calculate Mueller matrices for all baselines
void ANoise::calcAllMueller() {

  if (prtlev()>6) cout << "        AN::calcAllMueller" << endl;

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
  Int iant1(0), iant2(-1);
  for (Int ibln=0; ibln<nCalMat(); ibln++) {
    // update antenna numbers of the current baseline (assumes iant1 <= iant2)
    iant2++;
    if (iant2 == nAnt()) {
      iant1++;
      iant2 = iant1;
    }
    corruptor_p->currAnt()=iant1;
    corruptor_p->currAnt2()=iant2;

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




void ANoise::calcOneMueller(Vector<Complex>& mat, Vector<Bool>& mOk,
			    const Vector<Complex>& par, const Vector<Bool>& pOk) {
  
  if(prtlev() > 10)
    cout << "        AN::calcOneMueller()\n"
         << "par: " << par                     // These are more to remove a compiler
         << "\npOk: " << pOk                   // warning (par & pOK unused)
         << endl;                              // than anything.

  // If Mueller matrix is trivial, shouldn't get here
  if (trivialMuellerElem()) 
    throw(AipsError("Trivial Mueller Matrix logic error."));
  else {
    Int len=0;
    mat.shape(len);
    for (Int i=0; i<len; i++) {
      mat[i]=acorruptor_p->simPar(); // single complex #
      mOk[i]=true;
    }    
  }
}



} //# NAMESPACE CASA - END
