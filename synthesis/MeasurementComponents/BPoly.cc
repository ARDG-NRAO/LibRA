//# BJonesPoly.cc: Implementation of BJonesPoly.h
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
//# $Id: BJonesPoly.cc,v 19.18 2006/02/03 00:29:52 gmoellen Exp $

#include <synthesis/MeasurementComponents/BPoly.h>
#include <synthesis/MeasurementComponents/MSMetaInfoForCal.h>
#include <synthesis/MeasurementEquations/VisEquation.h>
#include <casa/Logging/LogIO.h>
#include <casa/Utilities/Assert.h>
#include <casa/Exceptions/Error.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/ArrayLogical.h>
#include <ms/MSOper/MSMetaData.h>
#include <ms/MeasurementSets/MSColumns.h>
#include <msvis/MSVis/VisBuffAccumulator.h>
#include <msvis/MSVis/VisBuffGroupAcc.h>
#include <casa/sstream.h>
#include <casa/math.h>
#include <casa/OS/Memory.h>
#include <casa/System/PGPlotter.h>
#include <synthesis/CalTables/BJonesMBuf.h>
#include <synthesis/CalTables/BJonesMCol.h>
#include <synthesis/CalTables/NewCalTable.h>
#include <ms/MSSel/MSSpWindowIndex.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

// Define external CLIC solvers
#define NEED_UNDERSCORES
#if defined(NEED_UNDERSCORES)
#define polyant polyant_
#define splinant splinant_
#define getbspl getbspl_
#define phaseant phaseant_
#define ampliant ampliant_
#define cheb cheb_
#endif

extern "C" { 
  void polyant(Int*, Int*, Int*, Int*, Int*, Int*, Int*, Int*,
               Double*, Double*, Double*, Double*, Double*, Double*,
               Double*, Double* );
  void splinant(Int*, Int*, Int*, Int*, Int*, Int*, Int*, Int*,
		Double*, Double*, Double*, Double*, Double*, Double*,
		Double*, Double*, Double* );
  
  void getbspl(Int*, Double*, Double*, Double*, Double*, Int*);
  
  void phaseant(Int*, Int*, Int*, Int*, Double*, Double*, Double*, Double*, 
		Int*, Double*, Double*);
  
  void ampliant(Int*, Int*, Int*, Int*, Double*, Double*, Double*, Double*, 
		Int*, Double*, Double*);
  
  void cheb(Int*, Double*, Double*, Int*);
}

//----------------------------------------------------------------------------

BJonesPoly::BJonesPoly (VisSet& vs) :
  VisCal(vs),             // virtual base
  VisMueller(vs),         // virtual base
  BJones(vs),             // immediate base
  vs_p(&vs),
  degamp_p(3),
  degphase_p(3),
  visnorm_p(false),
  maskcenter_p(1),
  maskedge_p(5.0),
  maskcenterHalf_p(0),
  maskedgeFrac_p(0.05),
  solTimeStamp(0.0),
  solSpwId(-1),
  solFldId(-1)
{
// Construct from a visibility set
// Input:
//    vs                VisSet&            Visibility set
// Output to private data:
//    vs_p              VisSet&            Visibility set pointer (needed locally)
//    degamp_p          Int                Polynomial degree in amplitude
//    degphase_p        Int                Polynomial degree in phase
//    visnorm           const Bool&        true if pre-normalization of the 
//                                         visibility data over frequency is
//                                         required before solving.
//    maskcenter        const Int&         No. of central channels to mask
//                                         during the solution
//    maskedge          const Float&       Fraction of spectrum to mask at
//                                         either edge during solution
//    maskcenterHalf_p  Int                Central mask half-width
//    maskedgeFrac_p    Float              Fractional edge mask

  // Neither solved nor applied at this point
  setSolved(false);
  setApplied(false);
};

//----------------------------------------------------------------------------

  

BJonesPoly::BJonesPoly (const MSMetaInfoForCal& msmc) :
  VisCal(msmc),             // virtual base
  VisMueller(msmc),         // virtual base
  BJones(msmc),             // immediate base
  vs_p(NULL),
  degamp_p(3),
  degphase_p(3),
  visnorm_p(false),
  maskcenter_p(1),
  maskedge_p(5.0),
  maskcenterHalf_p(0),
  maskedgeFrac_p(0.05),
  solTimeStamp(0.0),
  solSpwId(-1),
  solFldId(-1)
{
// Construct from a visibility set
// Input:
//    msmc              MSMetaInfoForCal&  MS Meta info server
// Output to private data:
//    vs_p              VisSet&            Visibility set pointer (needed locally)
//    degamp_p          Int                Polynomial degree in amplitude
//    degphase_p        Int                Polynomial degree in phase
//    visnorm           const Bool&        true if pre-normalization of the 
//                                         visibility data over frequency is
//                                         required before solving.
//    maskcenter        const Int&         No. of central channels to mask
//                                         during the solution
//    maskedge          const Float&       Fraction of spectrum to mask at
//                                         either edge during solution
//    maskcenterHalf_p  Int                Central mask half-width
//    maskedgeFrac_p    Float              Fractional edge mask

  // Neither solved nor applied at this point
  setSolved(false);
  setApplied(false);
};





void BJonesPoly::setSolve(const Record& solvepar)
{
// Set the solver parameters
// Input:
//    solvepar            const Record&      Solver parameters
// Output to private data:
//    degamp_p          Int                Polynomial degree in amplitude
//    degphase_p        Int                Polynomial degree in phase
//    visnorm           const Bool&        true if pre-normalization of the 
//                                         visibility data over frequency is
//                                         required before solving.
//    maskcenter        const Int&         No. of central channels to mask
//                                         during the solution
//    maskedge          const Float&       Fraction of spectrum to mask at
//                                         either edge during solution
//    maskcenterHalf_p  Int                Central mask half-width
//    maskedgeFrac_p    Float              Fractional edge mask
//

  // Call parent
  SolvableVisCal::setSolve(solvepar);

  // Delete calTableName() if it exists and we are not appending
  //  TBD: move to SVC?
  if (!append()) {
    if (Table::canDeleteTable(calTableName())) {
      Table::deleteTable(calTableName());
    }
    //    else 
    //      throw(AipsError(calTableName()+" exists and can't delete! (plotting or browsing it?)"));
  }

  // Extract the BPOLY-specific solve parameters
  Vector<Int> degree;
  if (solvepar.isDefined("degree")) degree = solvepar.asArrayInt("degree");
  degamp_p = degree(0);
  degphase_p = degree(1);
  if (solvepar.isDefined("visnorm")) visnorm_p = solvepar.asBool("visnorm");
  if (solvepar.isDefined("maskcenter")) maskcenter_p = 
                                       solvepar.asInt("maskcenter");
  if (solvepar.isDefined("maskedge")) maskedge_p = solvepar.asFloat("maskedge");

  // Compute derived mask parameters
  maskcenterHalf_p = maskcenter_p / 2;
  maskedgeFrac_p = maskedge_p / 100.0;

  preavg()=DBL_MAX;

};

//----------------------------------------------------------------------------

void BJonesPoly::setApply(const Record& applypar)
{
// Set the applypar parameters

// Call parent (loadMemCalTable is now overloaded)

//  nChanParList()=vs_p->numberChan();
//  startChanList()=vs_p->startChan();

  BJones::setApply(applypar);

  // The old BPOLY never used calWt=true; preserve
  //  this behavior for now
  calWt()=false;

  /*

  // Extract the parameters
  if (applypar.isDefined("table"))
    calTableName() = applypar.asString("table");
  if (applypar.isDefined("select"))
    calTableSelect() = applypar.asString("select");
  if (applypar.isDefined("t"))
    interval() = applypar.asDouble("t");
  if (applypar.isDefined("interp"))
    tInterpType()=applypar.asString("interp");

  // Protect against non-specific interp
  if (tInterpType()=="")
    tInterpType()="linear";

  indgen(spwMap());
  if (applypar.isDefined("spwmap")) {
    Vector<Int> spwmap(applypar.asArrayInt("spwmap"));
    if (allGE(spwmap,0)) {
      // User has specified a valid spwmap
      if (spwmap.nelements()==1)
        spwMap()=spwmap(0);
      else
        spwMap()(IPosition(1,0),IPosition(1,spwmap.nelements()-1))=spwmap;
      // TBD: Report non-trivial spwmap to logger.
      //      cout << "Note: spwMap() = " << spwMap() << endl;
    }
  }
  AlwaysAssert(allGE(spwMap(),0),AipsError);

  //  cout << "BPOLY: spwMap() = " << spwMap()<< endl;

  // Follow the selected data, for the moment
  // NB: this requires setdata prior to setapply!
  nChanParList()=vs_p->numberChan();
  startChanList()=vs_p->startChan();

  // If selection is non-trivial, complain for now
  if (calTableSelect()!="") {
    LogIO os (LogOrigin("BJonesPoly", "setApply()", WHERE));
    
    os << LogIO::WARN
       << "Selection on BPOLY caltables not yet supported."
       << LogIO::POST;
  }

  // Fill the bandpass correction cache from the applied cal. table
  load(calTableName());

  // Signal apply context
  setApplied(true);
  setSolved(false);

  */

};

//----------------------------------------------------------------------------


void BJonesPoly::selfSolveOne(VisBuffGroupAcc& vbga)
{
// Solver for the polynomial bandpass solution

// TODO:
//   1. Make pointers private, make delete function and use it 
//   2. Use antenna names
//


  LogIO os (LogOrigin("BJonesPoly", "selfSolveOne()", WHERE));

  os << LogIO::NORMAL
     << "THIS IS THE NEW MULTI-SPW-FLEXIBLE VERSION"
     << LogIO::POST;

  os << LogIO::NORMAL
     << "Fitting bandpass amplitude and phase polynomials."
     << LogIO::POST;
  os << LogIO::NORMAL 
     << "Polynomial degree for amplitude is " << degamp_p 
     << LogIO::POST;
  os << LogIO::NORMAL 
     << "Polynomial degree for phase is " << degphase_p 
     << LogIO::POST;

  // Bool to short-circuit operation
  Bool ok(true);

  // Initialize the baseline index
  Vector<Int> ant1(nBln(), -1);
  Vector<Int> ant2(nBln(), -1);
  Vector<Int> numFreqChan(nSpw(), 0);

  // make gridded freq array
  // The minimum number of frequency channels required for a solution
  //  is the number of coefficients in the fit.  For a gridded spectrum,
  //  filled from irregularly spaced input spectral windows, it is possible
  //  that only very few channels get filled.  So, we will be conservative
  //  and make a gridded spectrum with 2*ncoeff channels, where ncoeff is
  //  the maximum of the number of coefficients requested for the phase and
  //  amp solutions. We will then check to make sure that a sufficient
  //  number of gridded slots will be filled by the input frequency channels.


  Int nPH(0);
  Double minfreq(DBL_MAX), maxfreq(0.0), maxdf(0.0);
  PtrBlock<Vector<Double>*> freq; freq.resize(nSpw()); freq=0;

  for (Int ibuf=0;ibuf<vbga.nBuf();++ibuf) {

    CalVisBuffer& cvb(vbga(ibuf));

    Int spwid=cvb.spectralWindow();
    numFreqChan(spwid) = cvb.nChannel();
    freq[spwid] = new Vector<Double>;
    (*freq[spwid])=cvb.frequency();
    Double df2=abs((*freq[spwid])(1)-(*freq[spwid])(0))/2.0;
    maxdf=max(maxdf,2.0*df2);
    minfreq=min(minfreq,min((*freq[spwid])));
    maxfreq=max(maxfreq,max((*freq[spwid])));

    nPH= max(nPH,min(cvb.nCorr(),2));
  }
  minfreq=minfreq-maxdf/2.0;
  maxfreq=maxfreq+maxdf/2.0;

  // minfreq is the low edge of the lowest channel
  // maxfreq is the high edge of the highest channel
  // nPH is the number of parallel-hand correlations present

  //  cout << "freq, corr: " << minfreq << " " << maxfreq << " " << nPH << endl;

  Double freqstep;
  Int nFreqGrid;

  // Derive grid spacing/number from requested poly degree
  // FOR NOW, it is less error-prone to use input spacing
  //  nFreqGrid=2*(max(degamp_p,degphase_p)+1);
  //  nFreqGrid=max(nFreqGrid,16);  // no fewer than 16, in any case.
  //  freqstep=((maxfreq-minfreq)/Double(nFreqGrid));

  // Grid spacing is (multiple of?) maximum input channel spacing
  freqstep=maxdf;
  nFreqGrid = Int ((maxfreq-minfreq)/freqstep+0.5);

  // Fill the gridded frequency list
  Vector<Double> totalFreq(nFreqGrid,0.0);
  for (Int i=0;i<nFreqGrid;i++) {
    totalFreq(i)=minfreq + freqstep*(Double(i)+0.5);
  }

  // Populate the frequency grid with the input frequency channels,
  //  and demand that enough (?) will get filled.
  Vector<Bool> freqGridOk(nFreqGrid,false);
  for (Int ispw=0;ispw<nSpw();ispw++) {
    if (freq[ispw]) {
      for (Int ichan=0;ichan<numFreqChan(ispw);ichan++) {
	Int chanidx=(Int) floor(((*freq[ispw])(ichan)-minfreq)/freqstep);
	if(chanidx >= nFreqGrid){
	  cout << "spw " << ispw <<" " <<  chanidx << endl;
	}
	freqGridOk(chanidx)=true;
      }
    }
  }

  // Sanity check on polynomial order and grid count
  Int nok=ntrue(freqGridOk);
  if (nok < (degamp_p+1) ) {
    os << LogIO::SEVERE 
       << "Selected spectral window(s) nominally fill only " << nok << " grid points." 
       << LogIO::POST;
    os << LogIO::SEVERE 
       << "Reduce degamp by at least " << degamp_p+1-nok << " and try again." 
       << LogIO::POST;
    ok=false;
  }
  if (nok < (degphase_p+1) ) {
    os << LogIO::SEVERE 
       << "Selected spectral window(s) nominally fill only " << nok << " grid points." 
       << LogIO::POST;
    os << LogIO::SEVERE 
       << "Reduce degphase by at least " << degphase_p+1-nok << " and try again." 
       << LogIO::POST;
    ok=false;
  }
  // If either degree vs nGrid test failed, quit here
  if (!ok) throw(AipsError("Invalid polynomial degree specification"));

  // Report spectral information
  os << LogIO::NORMAL 
     << "Spectral grid for fit will have " << nFreqGrid
     << " points spaced by " << freqstep/1000.0 << " kHz."
     << LogIO::POST;

  os << LogIO::NORMAL 
     << "Polynomial solution will be valid over frequency range: "
     << totalFreq(0) << "-" << totalFreq(nFreqGrid-1) << " Hz."
     << LogIO::POST;

  os << LogIO::NORMAL 
     << "Total bandwidth: "
     << freqstep*Double(nFreqGrid)/1000.0 << " kHz."
     << LogIO::POST;


  // We must keep track of good ants and baselines
  Vector<Bool> antok(nAnt(),false);
  Matrix<Bool> bslok(nAnt(),nAnt(),false);
  Int nGoodBasl=0;
  Matrix<Int> bslidx(nAnt(),nAnt(),-1);
  Matrix<Int> antOkChan(nFreqGrid, nAnt(),0);
  Vector<Int> ant1num, ant2num;  // clic solver antenna numbers (1-based, contiguous)
  ant1num.resize(nBln());
  ant2num.resize(nBln());
  Vector<Int> ant1idx, ant2idx;  // MS.ANTENNA indices (for plot, storage)
  ant1idx.resize(nBln());
  ant2idx.resize(nBln());

  PtrBlock<Matrix<Complex>*> accumVis(nPH,NULL);
  PtrBlock<Matrix<Double>*> accumWeight(nPH,NULL);
  PtrBlock<Vector<Complex>*> normVis(nPH,NULL);
  PtrBlock<Vector<Double>*> normWeight(nPH,NULL);

  for (Int i=0;i<nPH;++i) {
    accumVis[i] = new Matrix<Complex>(nFreqGrid, nBln()); (*accumVis[i])=Complex(0.0,0.0);
    accumWeight[i] = new Matrix<Double>(nFreqGrid, nBln()); (*accumWeight[i])=0.0;
    normVis[i] = new Vector<Complex>(nBln()); (*normVis[i])=Complex(0.0,0.0);
    normWeight[i] = new Vector<Double>(nBln()); (*normWeight[i])=0.0;
  }

  Vector<Int> indexSpw;

  // By constraint, this solver should see data only from one sideband.
  // Pick up the frequency group name from the current spectral window
  // accordingly.
  String freqGroup("");

  solTimeStamp=refTime();
  solFldId=currField();
  solSpwId=currSpw();

  // Filter data from VBs to FORTRAN arrays for the solver
  for (Int ibuf=0;ibuf<vbga.nBuf();++ibuf) {

    CalVisBuffer& cvb(vbga(ibuf));

    // Extract the current visibility buffer spectral window id.
    // and number for frequency channels

    Int spwid = cvb.spectralWindow();
    Int nChan = cvb.nChannel();
    Int nCorr = cvb.nCorr();
    freqGroup = freqGrpName(spwid);

    Vector<Int> polidx(2,0);
    polidx(1)=nCorr-1;   // TBD: should be pol-sensitive!

    // Data and model values
    Complex data;

    // Compute the amplitude and phase spectrum for this spectral window
    for (Int row=0; row < cvb.nRow(); row++) {
      // Antenna indices
      Int a1 = cvb.antenna1()(row);
      Int a2 = cvb.antenna2()(row);
      Vector<Double> rowwt(2,0.0);
      for (Int iph=0;iph<nPH;++iph)
	rowwt(iph) = cvb.weightMat()(polidx(iph),row);

      // Reject auto-correlation data, zero weights, fully-flagged spectra
      if ( (a1 != a2) && 
           (sum(rowwt) > 0) && 
	   nfalse(cvb.flag().column(row)) > 0 ) {

	// These ants, this baseline is ok (there is at least some good data here)
	antok(a1)=true;
	antok(a2)=true;
	if (!bslok(a1,a2)) {   // first visit to this baseline
	  bslok(a1,a2)=true;
	  bslidx(a1,a2)=nGoodBasl++;
	  ant1idx(bslidx(a1,a2))=a1;
	  ant2idx(bslidx(a1,a2))=a2;
	}

	// Loop over the frequency channels
	for (Int ichan=0; ichan < nChan; ichan++) {
	  // Reject flagged/masked channels
	  if (!cvb.flag()(ichan,row) && !maskedChannel(ichan, nChan)) {

	    for (Int iph=0;iph<nPH;++iph) {
	    
	      data = cvb.visCube()(polidx(iph),ichan,row);
	      
	      // accumulate accumVis, accumWeight
	      if (abs(data) > 0 && rowwt(iph)>0.0) {
		Vector<Double> freq1=cvb.frequency();
		Int chanidx=(Int) floor((freq1(ichan)-minfreq)/freqstep);
		(*accumVis[iph])(chanidx,bslidx(a1,a2))+=(Complex(rowwt(iph),0.0)*data);
		(*accumWeight[iph])(chanidx,bslidx(a1,a2))+=rowwt(iph);
		
		// count this channel good for these ants
		antOkChan(chanidx,a1)++;
		antOkChan(chanidx,a2)++;
		
		// Accumulate data normalizing factor (visnorm)
		(*normVis[iph])(bslidx(a1,a2))+=(Complex(rowwt(iph),0.0)*data);
		(*normWeight[iph])(bslidx(a1,a2))+=rowwt(iph);
		
		// cout << row << " " << ichan << " " << chanidx << " "
		//   << data << " " << rowwt << " "
		//   << accumVis(chanidx,bslidx(a1,a2)) << " "
		//   << accumWeight(chanidx,bslidx(a1,a2)) << endl;
	      }

	    }

	  };
	};
      };
    };
  }; // for (ibuf...)

  //  cout << "solFldId     = " << solFldId << endl;
  //  cout << "solSpwId     = " << solSpwId << endl;
  //  cout << "solTimeStamp = " << MVTime(solTimeStamp/C::day).string( MVTime::YMD,7) << endl;

  // Delete freq PtrBlock
  for (Int ispw=0;ispw<nSpw();ispw++) {
    if (freq[ispw]) { delete freq[ispw]; freq[ispw]=0; }
  }

  // Form a contiguous 1-based antenna index list 
  Vector<Int> antnum(nAnt(),-1);
  Int antcount=0;
  Int refantenna(-1);
  for (Int iant=0;iant<nAnt();iant++) {
    if (antok(iant)) {
      antnum(iant)=(++antcount);

      // detect reference antenna with revised counting
      if (iant==refant())
	refantenna = antnum(iant);
    }
  }

  // Use first antenna as refant if requested one not ok
  if (refantenna<1)
    refantenna=1;

  //  cout << boolalpha << "antok = " << antok << endl;
  //  cout << "antnum = " << antnum << endl;

  for (Int ibl=0;ibl<nGoodBasl;ibl++) {
    ant1num(ibl)=antnum(ant1idx(ibl));
    ant2num(ibl)=antnum(ant2idx(ibl));
  }
  ant1num.resize(nGoodBasl,true);
  ant2num.resize(nGoodBasl,true);
  ant1idx.resize(nGoodBasl,true);
  ant2idx.resize(nGoodBasl,true);  


  Int nGoodAnt=Int(ntrue(antok));

  os << LogIO::NORMAL 
     << "Found data for " << nGoodBasl 
     << " baselines among " << nGoodAnt
     << " antennas."
     << LogIO::POST;

  //  cout << "antOkChan = " << antOkChan << endl;

  // BPoly is nominally dual-pol
  Matrix<Double> ampCoeff(nAnt(), 2*(degamp_p+1),0.0);       // solutions stored here later
  Matrix<Double> phaseCoeff(nAnt(), 2*(degphase_p+1), 0.0);  // solutions stored here later

  PtrBlock<Matrix<Double>*> totalWeight(nPH,NULL), totalPhase(nPH,NULL), totalAmp(nPH,NULL);

  for (Int iph=0;iph<nPH;++iph) {

    totalAmp[iph] = new Matrix<Double>(nFreqGrid, nGoodBasl); (*totalAmp[iph])=0.0;
    totalPhase[iph] = new Matrix<Double>(nFreqGrid, nGoodBasl); (*totalPhase[iph])=0.0;
    totalWeight[iph] = new Matrix<Double>(nFreqGrid, nGoodBasl); (*totalWeight[iph])=0.0;

    for (Int ibl=0;ibl<nGoodBasl;ibl++) {
      ant1num(ibl)=antnum(ant1idx(ibl));
      ant2num(ibl)=antnum(ant2idx(ibl));
      if ( (*normWeight[iph])(ibl)> 0.0)
	(*normVis[iph])(ibl)/=(static_cast<Complex>((*normWeight[iph])(ibl)));
      for (Int ichan=0;ichan<nFreqGrid;ichan++) {
	Double &wt=(*accumWeight[iph])(ichan,ibl);
	// insist at least 2 baselines with good data for these antennas in this channel
	if (wt > 0 &&
	    antOkChan(ichan,ant1idx(ibl)) > 1 &&   
	    antOkChan(ichan,ant2idx(ibl)) > 1 ) {
	  (*accumVis[iph])(ichan,ibl)/= (static_cast<Complex>(wt));
	  
	  // If requested, normalize the data, if possible
	  if (visnorm_p) {
	    if (abs((*normVis[iph])(ibl))>0.0 )
	      (*accumVis[iph])(ichan,ibl)/=(*normVis[iph])(ibl);
	    else
	      (*accumVis[iph])(ichan,ibl)=0.0; // causes problems in polyant?
	  }
  
	  (*totalAmp[iph])(ichan,ibl)=static_cast<Double>(log(abs((*accumVis[iph])(ichan,ibl))));
	  // Note phase sign convention!
	  (*totalPhase[iph])(ichan,ibl)=static_cast<Double>(-1.0*arg((*accumVis[iph])(ichan,ibl)));
	  (*totalWeight[iph])(ichan,ibl)=wt;
	}
      }

    }

    if ( accumVis[iph] )    delete accumVis[iph];
    if ( accumWeight[iph] ) delete accumWeight[iph];
    if ( normVis[iph] )     delete normVis[iph];
    if ( normWeight[iph] )  delete normWeight[iph];
    accumVis[iph]=NULL;
    accumWeight[iph]=NULL;
    normVis[iph]=NULL;
    normWeight[iph]=NULL;
        
    // First fit the bandpass amplitude
    os << LogIO::NORMAL 
       << "Fitting amplitude polynomial."
       << LogIO::POST;
    
    Int degree = degamp_p + 1;
    Int iy = 1;
    Bool dum;
    Vector<Double> rmsAmpFit2(nGoodBasl,0.0);
    Matrix<Double> ampCoeff2(nGoodAnt, degree, 1.0);
   
    {
      // Create work arrays
      Vector<Double> wk1(degree);
      Vector<Double> wk2(degree*degree*nGoodAnt*nGoodAnt);
      Vector<Double> wk3(degree*nGoodAnt);
      
      // Call the CLIC solver for amplitude
      polyant(&iy,
	      &nFreqGrid,
	      &nGoodBasl,
	      ant1num.getStorage(dum),
	      ant2num.getStorage(dum),
	      &refantenna,
	      &degree,
	      &nGoodAnt,
	      totalFreq.getStorage(dum),
	      totalAmp[iph]->getStorage(dum),
	      totalWeight[iph]->getStorage(dum),
	      wk1.getStorage(dum),
	      wk2.getStorage(dum),
	      wk3.getStorage(dum),
	      rmsAmpFit2.getStorage(dum),
	      ampCoeff2.getStorage(dum));
    }

    if (totalAmp[iph]) delete totalAmp[iph];
    totalAmp[iph]=NULL;

    os << LogIO::NORMAL 
       << "Per-baseline RMS log(Amp) statistics: (min/mean/max) = "
       << min(rmsAmpFit2) << "/" << mean(rmsAmpFit2) << "/" << max(rmsAmpFit2)
       << LogIO::POST;
    
    // Now fit the bandpass phase
    os << LogIO::NORMAL 
       << "Fitting phase polynomial."
       << LogIO::POST;
    
    // Call the CLIC solver for phase
    degree = degphase_p + 1;
    iy = 2;
    Vector<Double> rmsPhaseFit2(nGoodBasl,0.0);
    Matrix<Double> phaseCoeff2(nGoodAnt, degree, 123.0);
    
    {
      // Create work arrays
      Vector<Double> wk6(degree);
      Vector<Double> wk7(degree*degree*nGoodAnt*nGoodAnt);
      Vector<Double> wk8(degree*nGoodAnt);
      
      polyant(&iy,
	      &nFreqGrid,
	      &nGoodBasl,
	      ant1num.getStorage(dum),
	      ant2num.getStorage(dum),
	      &refantenna,
	      &degree,
	      &nGoodAnt,
	      totalFreq.getStorage(dum),
	      totalPhase[iph]->getStorage(dum),
	      totalWeight[iph]->getStorage(dum),
	      wk6.getStorage(dum),
	      wk7.getStorage(dum),
	      wk8.getStorage(dum),
	      rmsPhaseFit2.getStorage(dum),
	      phaseCoeff2.getStorage(dum));
    }

    if (totalPhase[iph]) delete totalPhase[iph];
    if (totalWeight[iph]) delete totalWeight[iph];
    totalPhase[iph]=NULL;
    totalWeight[iph]=NULL;

    os << LogIO::NORMAL 
       << "Per baseline RMS phase (deg) statistics: (min/mean/max) = "
       << min(rmsPhaseFit2)*180.0/C::pi << "/" 
       << mean(rmsPhaseFit2)*180.0/C::pi << "/" 
       << max(rmsPhaseFit2)*180.0/C::pi
       << LogIO::POST;
    
    // Expand solutions into full antenna list
    IPosition iplo(1,iph*(degphase_p+1));
    IPosition iphi(1,(iph+1)*(degphase_p+1)-1);
    IPosition ialo(1,iph*(degamp_p+1));
    IPosition iahi(1,(iph+1)*(degamp_p+1)-1);
    
    for (Int iant=0;iant<nAnt();iant++) {
      if (antok(iant)) {
	ampCoeff.row(iant)(ialo,iahi)=ampCoeff2.row(antnum(iant)-1);
	phaseCoeff.row(iant)(iplo,iphi)=phaseCoeff2.row(antnum(iant)-1);
      }
    }

    // plot amplitude and phase baseline data/solutions
    //  plotsolve2(totalFreq, totalAmp, totalPhase, totalWeight, ant1idx, ant2idx,
    //	     rmsAmpFit2, ampCoeff, rmsPhaseFit2, phaseCoeff);
    
  } // iph

  Int nChanTotal=nFreqGrid;
  Double meanfreq=mean(totalFreq);
  
  // Compute the reference frequency and reference phasor
  Vector<Complex> refAmp;
  Vector<MFrequency> refFreq;
  refAmp.resize(nAnt());
  refFreq.resize(nAnt());  
  for (Int k=0; k < nAnt(); k++) {
    refAmp(k) = Complex(1.0,0); 
    refFreq(k) = MFrequency(Quantity(meanfreq, "Hz"));
  }; 
  
  // Frequency range
  Double loFreq = totalFreq(0);
  Double hiFreq = totalFreq(nChanTotal-1);
  
  Matrix<Double> validDomain(nAnt(), 2);
  validDomain.column(0) = loFreq;
  validDomain.column(1) = hiFreq;
  // TBD: 
  //  Double edgefreq(maskedge_p*(hiFreq-loFreq));
  //  validDomain.column(0) = loFreq + edgefreq;
  //  validDomain.column(1) = hiFreq - edgefreq;
  
  // Normalize the output calibration solutions if required
  Vector<Complex> scaleFactor(nAnt(), Complex(1,0));

  if (solnorm()) {
    os << LogIO::NORMAL 
       << "Normalizing antenna-based solutions."
       << LogIO::POST;

    Vector<Double> Ca, Cp;
    for (Int iph=0;iph<nPH;++iph) {

      IPosition iplo(1,iph*(degphase_p+1));
      IPosition iphi(1,(iph+1)*(degphase_p+1)-1);
      IPosition ialo(1,iph*(degamp_p+1));
      IPosition iahi(1,(iph+1)*(degamp_p+1)-1);

      // Loop over antenna
      for (Int iant=0; iant < nAnt(); iant++) {

	Ca.reference(ampCoeff.row(iant)(ialo,iahi));
	Cp.reference(phaseCoeff.row(iant)(iplo,iphi));


	// Normalize mean phasor across the spectrum
	Int nChAve(0);
	Complex meanPha(0,0);
	Double meanAmp(0);
	for (Int chan=0; chan < nChanTotal; chan++) {
	  // only use channels that participated in the fit
	  if (antOkChan(chan,iant) > 3) {

	    nChAve++;
	    Double ampval = getChebVal(Ca, loFreq, hiFreq,
				       totalFreq(chan));
	    Double phaseval = getChebVal(Cp, loFreq, hiFreq,
					 totalFreq(chan));

	    meanPha += Complex(cos(phaseval), sin(phaseval));
	    meanAmp += exp(ampval);

	  };
	}
	// Normalize by adjusting the zeroth order term
	if (nChAve>0) {
	  meanPha/=Complex(nChAve);
	  meanAmp/=Double(nChAve);

	  //	  cout << "mean B = " << meanAmp << " " << arg(meanPha)*180.0/C::pi << endl;
	  
	  Ca(0)-=2.0*log(meanAmp);
	  Cp(0)-=2.0*arg(meanPha);
	}
      };
    }
  };
    
  // Update the output calibration table
  Vector<Int> antId(nAnt());
  indgen(antId);
  Vector<String> polyType(nAnt(), "CHEBYSHEV");
  Vector<String> phaseUnits(nAnt(), "RADIANS");
  Vector<Int> refAnt(nAnt(), refant());
  
  updateCalTable (freqGroup, antId, polyType, scaleFactor, validDomain,
		  ampCoeff, phaseCoeff, phaseUnits, refAmp, refFreq, refAnt);

  // After first call, append must be true in case we have more to 
  //  write.
  append()=true;
  
};


// "Calculate" current parameters
void BJonesPoly::calcPar() {

  // Currently, we only support a single bandpass solution
  //   (i.e., no time-dep), so if any currParOK() (for currSpw),
  //   then we have a good solution

  // If any 
  //  if (sum(currParOK())>0 )
  //    validateP();
  //  else
  //    throw(AipsError("No calibration available for current Spw!"));

  // Call parent
  BJones::calcPar();

}


//----------------------------------------------------------------------------
    
void BJonesPoly::updateCalTable (const String& freqGrpName, 
				 const Vector<Int>& antennaId,
				 const Vector<String>& polyType, 
				 const Vector<Complex>& scaleFactor,
				 const Matrix<Double>& validDomain,
				 const Matrix<Double>& polyCoeffAmp,
				 const Matrix<Double>& polyCoeffPhase,
				 const Vector<String>& phaseUnits,
				 const Vector<Complex>& sideBandRef,
				 const Vector<MFrequency>& refFreq, 
				 const Vector<Int>& refAnt)
{
// Update the output calibration table to include the current soln. parameters
// Input:
//    freqGrpName     const String&             Freq. group name
//    antennaId       const Vector<Int>&        Antenna id. for each soln.
//    polyType        const Vector<String>&     Polynomial type (per soln.)
//    scaleFactor     const Vector<Complex>&    Scale factor (per soln.)
//    validDomain     const Matrix<Double>&     Valid domain [x_0, x_1] 
//                                              (per solution)
//    polyCoeffAmp    const Matrix<Double>&     Polynomial amplitude 
//                                              coefficients (per soln.)
//    polyCoeffPhase  const Matrix<Double>&     Polynomial phase coefficients
//                                              (per solution)
//    phaseUnits      const Vector<String>&     Phase units (per solution)
//    sideBandRef     const Vector<Complex>&    Sideband reference phasor
//                                              (per solution)
//    refFreq         const Vec<MFrequency>&    Sideband reference frequency
//                                              (per solution)
//    refAnt          const Vector<Int>&        Reference antenna (per soln.)
// Input from private data:
//    solveTable_p    String                    Output calibration table name
//

  LogIO os (LogOrigin("BJonesPoly", "updateCalTable()", WHERE));

  // Open the caltable
  Table::TableOption accessMode = Table::New;
  if (append() && Table::isWritable(calTableName())) {
    accessMode = Table::Update;
  }
  BJonesPolyTable calTable(calTableName(), accessMode);

  Int nDesc=calTable.nRowDesc();
  Int idesc=0;
  Int thisDesc=-1;
  while (idesc<nDesc && thisDesc<0) {
    Vector<Int> spw;
    CalDescRecord calDescRec(calTable.getRowDesc(idesc));
    calDescRec.getSpwId(spw);

    if (spw(0)==solSpwId)
      thisDesc=idesc;

    ++idesc;
  }
  if (thisDesc<0)
    thisDesc=nDesc;

  // Fill the bandpass solution parameters to a BJonesPoly calibration
  // buffer spanning the antenna id.'s
  Vector<Int> key(1, MSCalEnums::ANTENNA1);
  Block<Vector<Int> > keyvals(1, antennaId);
  BJonesPolyMBuf buffer(key, keyvals);

  // Add each solution to the calibration buffer
  for (uInt k=0; k < antennaId.nelements(); k++) {
    buffer.putAntGain(antennaId(k), freqGrpName, polyType(k), scaleFactor(k),
		      validDomain.row(k), 
		      //		      polyCoeffAmp.row(k).nelements(),
		      //		      polyCoeffPhase.row(k).nelements(),
		      degamp_p+1,degphase_p+1,
		      polyCoeffAmp.row(k), polyCoeffPhase.row(k), 
		      phaseUnits(k), sideBandRef(k), refFreq(k), refAnt(k));
  };

  buffer.fieldId().set(solFldId);
  buffer.calDescId().set(thisDesc);
  buffer.timeMeas().set(MEpoch(MVEpoch(solTimeStamp/86400.0)));

  os << LogIO::NORMAL 
     << "Storing calibration in table " << calTableName()
     << LogIO::POST;

  // Append the calibration buffer to the output calibration table
  buffer.append(calTable);

  // Only update CAL_DESC if a new row required
  if (thisDesc==nDesc) {
    CalDescRecord cdr;
    cdr.defineSpwId(Vector<Int>(1,solSpwId));
    cdr.defineMSName(Path(msName()).baseName());
    calTable.putRowDesc(thisDesc,cdr);
  }
  
  return;
}

//----------------------------------------------------------------------------

Double BJonesPoly::getChebVal (const Vector<Double>& coeff, 
			       const Double& xinit, const Double& xfinal,
			       const Double& x) 
{
// Compute a Chebyshev polynomial value using the CLIC library
// Input:
//    coeff       const Vector<Double>&       Chebyshev coefficients
//    xinit       const Double&               Domain start
//    xfinal      const Double&               Domain end
//    x           const Double&               x-ordinate
// Output:
//    getChebVal  Double                      Chebyshev polynomial value
//
  // Re-scale x-ordinate
  Double xcap=0; 
  xcap=((x-xinit)-(xfinal-x))/(xfinal-xinit);

  // Compute polynomial
  Int deg=coeff.shape().asVector()(0);
  Vector<Double> val(deg);
  Bool check;
  Int checkval;
  cheb(&deg,
       &xcap,
       val.getStorage(check),
       &checkval);

  Double soly=0.0;
  for (Int mm=0; mm< deg; mm++){
    soly+= coeff[mm]*val[mm];
  }

  return soly;
}

//----------------------------------------------------------------------------
    
Bool BJonesPoly::maskedChannel (const Int& chan, const Int& nChan) 
{
// Check if a given channel is masked or not
// Input:
//    chan               const Int&            Channel number
//    nChan              const Int&            No. of channels in spectrum
// Output:
//    maskedChannel      Bool                  Returns true if channel lies
//                                             in edge or center mask
//
  // Initialization
  Bool masked = false;
  Int loChan = nChan / 2 - maskcenterHalf_p;
  Int hiChan = loChan + maskcenter_p;

  // Check mask at center of spectrum
  if ((chan >= loChan) && (chan < hiChan)) {
    masked = true;
  };

  // Check mask at edge of spectrum
  if ((chan < (nChan*maskedgeFrac_p)) || (chan > nChan*(1-maskedgeFrac_p))) {
    masked = true;
  };

  return masked;
};

//----------------------------------------------------------------------------
    
void BJonesPoly::loadMemCalTable (String applyTable,String /*field*/)
{

// Load and cache the polynomial bandpass corrections
// Input:
//    applyTable      const String&            Cal. table to be applied
// Output to protected data:
//    Antenna and interferometer bandpass correction caches 
//    in the BJones parent class.
//

// NB: For the moment we will read the Chebychev params and calculate
//   a sampled (complex) bandpass on a per-channel basis. Downstream,
//   BPOLY will thus behave as if it were an ordinary B (except for the
//   fact that currently it CANNOT be time-dependent).

  // Generate a NCT in memory to hold the BPOLY as a B
  ct_=new NewCalTable("BpolyAsB.temp",VisCalEnum::COMPLEX,"B Jones",msName(),false);

  // Open the BJonesPoly calibration table
  BJonesPolyTable calTable(applyTable, Table::Update);

  // Ensure sort on TIME, so CalSet is filled in order
  Block <String> sortCol(3);
  sortCol[0]="CAL_DESC_ID";
  sortCol[1]="TIME";
  sortCol[2]="ANTENNA1";
  calTable.sort2(sortCol);

  // Attach a calibration table columns accessor
  BJonesPolyMCol col(calTable);

  // Fill the bandpass correction cache
  Int nrows = calTable.nRowMain();
  Int nDesc = calTable.nRowDesc();

  // A matrix to show which spws to be calibrated by each caldesc
  Vector<Int> spwmap(nDesc,-1);
  for (Int idesc=0;idesc<nDesc;++idesc) {

      // This cal desc
    CalDescRecord calDescRec(calTable.getRowDesc(idesc));

    // Get this spw ID
    Vector<Int> spwId;
    calDescRec.getSpwId(spwId);
    Int nSpw; spwId.shape(nSpw);
    if (nSpw > 1) {};  // ERROR!!!  Should only be one spw per cal desc!
    spwmap(idesc)=spwId(0);

    currSpw()=spwId(0);
    Vector<Double> freq = freqAxis(currSpw());
    
    startChan()=0;
    nChanPar()=freq.nelements(); // currSpw

    // Set SPW subtable freqs
    //IPosition blc(1,startChan()), trc(1,startChan()+nChanPar()-1);
    ct_->setSpwFreqs(currSpw(),freq); // (blc,trc));

  }
  
  // We will fill in a _sampled_ bandpass from the Cheby coeffs
  for (Int ispw=0; ispw<nSpw(); ispw++) {
    currSpw()=ispw;
    //    currCPar().resize(nPar(),nChanPar(),nAnt());
    //    currCPar()=Complex(1.0);
    //    currParOK().resize(nPar(),nChanPar(),nAnt());
    //    currParOK()=false;
    invalidateP();
    invalidateCalMat();      
  }

  // Inflate the solve arrays, so we can fill them
  initSolvePar();

  for (Int row=0; row < nrows; row++) {

    Int idesc = col.calDescId().asInt(row);
 
    Double thisTime=col.time().asdouble(row);
    Int thisSpw=spwmap(idesc);

    // Antenna id.
    Int antennaId = col.antenna1().asInt(row);

    currSpw()=thisSpw;
    refTime()=thisTime;

    // Frequency group name
    String freqGrpName = col.freqGrpName().asString(row);

    // Extract the polynomial scale factor
    Complex factor = col.scaleFactor().asComplex(row);

    // Extract the valid domain for the polynomial
    Vector<Double> freqDomain(2);
    col.validDomain().get(row, freqDomain);

    // Extract the polynomial coefficients in amplitude and phase
    Int nAmp = col.nPolyAmp().asInt(row);
    Int nPhase = col.nPolyPhase().asInt(row);
    Matrix<Double> ampCoeff(nAmp,2);
    Matrix<Double> phaseCoeff(nPhase,2);
    Array<Double> ampCoeffArray, phaseCoeffArray;
    col.polyCoeffAmp().get(row, ampCoeffArray);
    col.polyCoeffPhase().get(row, phaseCoeffArray);

    IPosition ampPos = ampCoeffArray.shape();
    ampPos = 0;
    for (Int k=0; k < 2*nAmp; k++) {
      ampPos.setLast(IPosition(1,k));
      ampCoeff(k%nAmp,k/nAmp) = ampCoeffArray(ampPos);
    };

    IPosition phasePos = phaseCoeffArray.shape();
    phasePos = 0;
    for (Int k=0; k < 2*nPhase; k++) {
      phasePos.setLast(IPosition(1,k));
      phaseCoeff(k%nPhase,k/nPhase) = phaseCoeffArray(phasePos);
    };

    // Loop over all spectral window id.'s in this frequency group
    //    Vector<Int> spwIds = spwIdsInGroup(freqGrpName);

      // This cal desc
    CalDescRecord calDescRec(calTable.getRowDesc(idesc));

    // Get this spw ID
    Vector<Int> spwIds;
    calDescRec.getSpwId(spwIds);

      
    Vector<Double> freq = freqAxis(currSpw());
    Int nChan=freq.nelements();

    Double x1 = freqDomain(0);
    Double x2 = freqDomain(1);
    
    for (Int ipol=0;ipol<2;ipol++) {
      
      Vector<Double> ac(ampCoeff.column(ipol));
      Vector<Double> pc(phaseCoeff.column(ipol));
      
      // Only do non-triv calc if coeffs are non-triv
      if (anyNE(ac,Double(0.0)) ||
	  anyNE(pc,Double(0.0)) ) {
	
	// Loop over frequency channel
	for (Int chan=0; chan < nChan; ++chan) {
	  
	  Double ampval(1.0),phaseval(0.0);
	  // only if in domain, calculate Cheby
	  Double thisfreq(freq(chan));
	  if (thisfreq >=x1 && thisfreq <= x2) {
	    ampval = getChebVal(ac, x1, x2, thisfreq);
	    phaseval = getChebVal(pc, x1, x2, thisfreq);
	    solveAllCPar()(ipol,chan,antennaId)= factor *
	      Complex(exp(ampval)) * Complex(cos(phaseval),sin(phaseval));
	    solveAllParOK()(ipol,chan,antennaId)= true;
	  }
	  else {
	    // Unflagged unit calibration for now
	    solveAllCPar()(ipol,chan,antennaId)= Complex(1.0);
	    solveAllParOK()(ipol,chan,antennaId)= true;
	  }	      
	}	   
      }
      
    } // ipol

    // Every nAnt rows, store the result
    if ((row+1)%nAnt()==0) {
      ct_->fillAntBasedMainRows(nAnt(),refTime(),0.0,-1,currSpw(),
                                -1,Vector<Int>(),-1,
                                solveAllCPar(),!solveAllParOK(),
                                solveAllParErr(),solveAllParSNR());

      // reset arrays
      solveAllCPar().set(Complex(1.0));
      solveAllParOK().set(false);
      solveAllParErr().set(0.0);
      solveAllParSNR().set(1.0);

    }

  }   // rows

  /*
  String cTN;
  cTN=calTableName();
  calTableName()=calTableName()+".BpolyAsB";
  storeNCT();
  calTableName()=cTN;
  */

  return;
}

//----------------------------------------------------------------------------
    
Double BJonesPoly::meanFrequency (const Vector<Int>& spwid) 
{
// Compute the bandwidth-weighted average frequency of a set of spw id.'s
// Input:
//    spwid           const Vector<Int>&       Spectral window id.'s
// Input from private data:
//    vs_p             VisSet*                  Current visibility set
// Output:
//    meanFrequency   Double                   Mean frequency (as Double)
//

  if (!vs_p) throw(AipsError("Error in BJonesPoly::meanFrequency"));

  const MSColumns& mscol(vs_p->iter().msColumns());
  const MSSpWindowColumns& spwcol(mscol.spectralWindow());
  const ArrayColumn<Double>& frequencies(spwcol.chanFreq());
  const ScalarColumn<Double>& totalbw(spwcol.totalBandwidth());

  Int numspw=spwid.shape().asVector()(0);

  Double meanFreq=0.0;
  Double sumbw=0.0;
  for (Int k=0; k<numspw; k++){
    meanFreq = meanFreq +
      sum(frequencies(spwid(k)))*totalbw(spwid(k)) / 
      (frequencies(spwid(k)).nelements());
    sumbw = sumbw + totalbw(spwid(k));
  }
  // Compute the mean frequency
  meanFreq=meanFreq/sumbw;
  return meanFreq;
}

//----------------------------------------------------------------------------
    
String BJonesPoly::freqGrpName (const Int& spwId) 
{
// Return the frequency group name for a given spectral window id.
// Input:
//    spwId           const Int&               Spectral window id.
// Input from private data:
//    vs_p             VisSet*                  Current visibility set
// Output:
//    freqGrpName     String                   Frequency group name
//

  if (!vs_p) throw(AipsError("Error in BJonesPoly::freqGrpName"));

  const MSColumns& mscol(vs_p->iter().msColumns());
  const MSSpWindowColumns& spwCol(mscol.spectralWindow());

  return spwCol.freqGroupName().asString(spwId);
}

//----------------------------------------------------------------------------
    
Vector<Int> BJonesPoly::spwIdsInGroup (const String& freqGrpName) 
{
// Return the spw. id.'s in a freq. group of a given name
// Input:
//    freqGrpName     const String&            Frequency group name
// Input from private data:
//    vs_p             VisSet*                  Current visibility set
// Output:
//    spwIdsInGroup   Vector<Int>              Spw. id.'s in freq. group
//
  // Open a SPECTRAL_WINDOW sub-table index
  if (!vs_p) throw(AipsError("Error in BJones::spwIdsInGroup"));
  MeasurementSet ms(vs_p->msName().before("/SELECTED"));
  MSSpWindowIndex spwIndex(ms.spectralWindow());
  return spwIndex.matchFreqGrpName(freqGrpName);
}

//----------------------------------------------------------------------------
    
Vector<Double> BJonesPoly::freqAxis (const Int& spwId) 
{
// Return the frequency axis values for a given spectral window id.
// Input:
//    spwId           const Int&               Spectral window id.
// Input from private data:
//    vs_p             VisSet*                  Current visibility set
// Output:
//    freqAxis        Vector<Double>           Frequency axis values
//

  Vector<Double> freqVal;
  if (vs_p) {
    const MSColumns& mscol(vs_p->iter().msColumns());
    const MSSpWindowColumns& spwCol(mscol.spectralWindow());
    
    spwCol.chanFreq().get(spwId, freqVal);
  }
  else {
    // Try msmc...
    const MSColumns& mscol(*(msmc().msmd().getMS()));
    const MSSpWindowColumns& spwCol(mscol.spectralWindow());
    spwCol.chanFreq().get(spwId, freqVal);
  }

  return freqVal;
}


//----------------------------------------------------------------------------
void BJonesPoly::plotsolve2(const Vector<Double>& x, 
			    const Matrix<Double>& ampdata, 
			    const Matrix<Double>& phadata, 
			    const Matrix<Double>& wtdata, 
			    const Vector<Int>& ant1idx, const Vector<Int>& ant2idx, 
			    const Vector<Double>& amperr, Matrix<Double>& ampcoeff, 
			    const Vector<Double>& phaerr, Matrix<Double>& phacoeff) 

{
  // Function to plot and compare Bandpass data and solution
  // Input:
  // x - vector of frequecies
  // ampdata - matrix of data - shape is yall(freqindex, baselineindex)
  // phadata - matrix of data - shape is yall(freqindex, baselineindex)
  // wtdata - matrix of weight; same shape as yall
  // ant1idx - indices for antenna1 
  // ant2idx - indices for antenna2
  // amperr - baseline-based amplitude fit errors 
  // ampcoeff - antenna-based amplitude poly coefficients
  // phaerr - baseline-based phase fit errors
  // phacoeff - antenna-based phase poly coefficients

  LogIO os (LogOrigin("BJonesPoly", "plotsolve2()", WHERE));

  String device;
  device=calTableName() + ".ps/cps";

  os << LogIO::NORMAL 
     << "Generating plot file:" << device
     << LogIO::POST;

  PGPlotter pg(device);
  pg.subp(4,3);

  Int ndatapts= max(x.shape().asVector());
  Int nmodelpts=4*(ndatapts-2)-1;
  Int num_valid_points=0;

  Int numbasl=ampdata.shape()(1);

  Vector<Float> x1(ndatapts);
  Vector<Float> amp1(ndatapts);
  Vector<Float> pha1(ndatapts);
  Vector<Float> amperr1(ndatapts); 
  Vector<Float> phaerr1(ndatapts); 
  Vector<Double> weight;

  Vector<Double>  xval(nmodelpts);
  Vector<Float> xfloatval(nmodelpts);

  Vector<Float> amp2a(nmodelpts);
  Vector<Float> amp2b(nmodelpts);
  Vector<Float> amp2(nmodelpts);
  Vector<Float> pha2a(nmodelpts);
  Vector<Float> pha2b(nmodelpts);
  Vector<Float> pha2(nmodelpts);

  Double minfreq,maxfreq;
  minfreq=min(x); 
  maxfreq=max(x); 

  Double dmodfreq;
  dmodfreq = (maxfreq-minfreq)/Double(ndatapts-1)/4;

  Vector<Double> ant1ampcoeff, ant2ampcoeff; 
  Vector<Double> ant1phacoeff, ant2phacoeff; 

  Vector<Float> meanamp(numbasl,0.0);
  Vector<Float> meanampmod(numbasl,0.0);
  Vector<Float> meanpha(numbasl,0.0);
  Vector<Float> meanphamod(numbasl,0.0);
  for (Int ibl=0; ibl<numbasl; ibl++) {

    // refresh all plotting variables
    x1.resize(ndatapts);
    amp1.resize(ndatapts);
    pha1.resize(ndatapts);
    amperr1.resize(ndatapts);
    phaerr1.resize(ndatapts);
    weight.resize();
    weight=wtdata.column(ibl);  

    num_valid_points=0;
    

    for(Int k=0; k < ndatapts; k++){
      if (weight(k)>0){
	x1(num_valid_points)=(x(k)-minfreq)/1e3;
	
	amp1(num_valid_points)=exp(ampdata.column(ibl)(k));
	amperr1(num_valid_points)=Float(amperr(ibl))*amp1(num_valid_points);

	pha1(num_valid_points)=remainder(phadata.column(ibl)(k), 2*C::pi)/C::pi*180.0;
	phaerr1(num_valid_points)=Float(phaerr(ibl))/C::pi*180.0;
	  
	num_valid_points+=1;
      }
    }
      
    if (num_valid_points > 0){

      // resize data arrays according to num_valid_points
      x1.resize(num_valid_points, true);
      amp1.resize(num_valid_points, true);
      amperr1.resize(num_valid_points, true);
      pha1.resize(num_valid_points, true);
      phaerr1.resize(num_valid_points, true);

      ant1ampcoeff=ampcoeff.row(ant1idx(ibl));
      ant2ampcoeff=ampcoeff.row(ant2idx(ibl));
      ant1phacoeff=phacoeff.row(ant1idx(ibl));
      ant2phacoeff=phacoeff.row(ant2idx(ibl));
      
      for(Int k=0; k < nmodelpts; k++){

	xval[k]= Double(k-1)*dmodfreq + x[1];	
	
	// Float version, offset & scaled, for plotting
	xfloatval(k)=Float((xval(k)-minfreq)/1.0e3);

	pha2(k)=0;
	pha2a(k)=getChebVal(ant1phacoeff,x(0),x(ndatapts-1), xval[k]);
	pha2b(k)=getChebVal(ant2phacoeff,x(0),x(ndatapts-1), xval[k]);
	pha2(k)=pha2b(k)-pha2a(k);
	          
	pha2a(k)=remainder(pha2a(k),2*C::pi)*180.0/C::pi;
	pha2b(k)=remainder(pha2b(k),2*C::pi)*180.0/C::pi;
	pha2(k)=remainder(pha2(k),2*C::pi)*180.0/C::pi;


	amp2a(k)=getChebVal(ant1ampcoeff,x(0),x(ndatapts-1), xval(k));
	amp2b(k)=getChebVal(ant2ampcoeff,x(0),x(ndatapts-1), xval(k));
	amp2(k)=amp2b(k) + amp2a(k);
                  
	amp2a(k)=exp(amp2a(k));
	amp2b(k)=exp(amp2b(k));
	amp2(k)=exp(amp2(k));

      }

      meanamp(ibl) = mean(amp1);
      meanampmod(ibl) = mean(amp2);
      meanpha(ibl) = mean(pha1);
      meanphamod(ibl) = mean(pha2);


 /*
      cout << ant1idx(ibl) << "-" << ant2idx(ibl) << "  Means: " 
	   << mean(amp1) << " "
	   << mean(amp2) << " "
	   << mean(pha1) << " "
	   << mean(pha2) << endl;
      cout << "means    = " 
	   << mean(meanamp) << " "
	   << mean(meanampmod)  << " "
	   << mean(meanpha)  << " "
	   << mean(meanphamod)  << " "
	   << endl;
 */

      //cout << " Mean of sol " << mean(soly1) << endl;

      Float min_x= 0.0;
      Float max_x= Float((maxfreq-minfreq)/1.0e3);

      Float min_amp, max_amp, min_pha, max_pha;
      min_amp=min( (min(amp1)-3.0*max(amperr1)), min(amp2) );
      max_amp=max( (max(amp1)+3.0*max(amperr1)), max(amp2) );
      min_pha=min( (min(pha1)-3.0*max(phaerr1)), min(pha2) );
      max_pha=max( (max(pha1)+3.0*max(phaerr1)), max(pha2) );

      min_amp=min(amp1)-1.0*max(amperr1);
      max_amp=max(amp1)+1.0*max(amperr1);
      min_pha=min(pha1)-1.0*max(phaerr1);
      max_pha=max(pha1)+1.0*max(phaerr1);

      Float damp=0.1*(max_amp-min_amp);
      Float dpha=0.1*(max_pha-min_pha);
      min_amp-=damp;
      max_amp+=damp;
      min_pha-=dpha;
      max_pha+=dpha;
	

      pg.page();

      // arrange labels
      ostringstream titlelab, xlab, ampdeg, phadeg, pharms, amprms;
      xlab.precision(12);
      xlab << "Frequency in kHz (-" << minfreq/1.0e9 << " GHz)";
      titlelab << "B polynomial for baseline " 
	       << ant1idx(ibl) << " & " << ant2idx(ibl);
      ampdeg << "degree = " << degamp_p;
      phadeg << "degree = " << degphase_p;
      pharms << "rms = " << phaerr1(0);
      amprms << "rms = " << amperr1(0);


      // plot phase in upper half
      pg.sci(1); pg.sch(1.5);
      //      pg.svp(0.13,0.87,0.5,0.85);
      pg.svp(0.10,0.90,0.525,0.90);
      pg.swin(min_x,max_x,min_pha,max_pha);
      pg.box("BCST",0.0,0,"BCNST",0.0,0);

      pg.sch(0.75); 
      pg.sci(2);
      pg.errb(6, x1, pha1, phaerr1, 2.0);
      pg.sci(3);
      pg.pt(x1, pha1, 17);
      pg.sci(4); pg.sls(1);
      pg.line(xfloatval, pha2);
/*
      pg.sci(4); pg.sls(4);  // plot each antenna soln
      pg.line(xfloatval, pha2a);
      pg.line(xfloatval, pha2b);
      pg.sls(1);
*/
      pg.sci(1); pg.sch(1.5);
      pg.lab(" ", "Phase (deg)", " ");
      pg.mtxt("T",0.75,0.5,0.5,titlelab);
      pg.sch(1.2);
      pg.mtxt("T",-1.5,0.95,1.0,phadeg);
      pg.mtxt("B",-1.5,0.95,1.0,pharms);


      // plot amp in upper half
      pg.sci(1); pg.sch(1.5);

      pg.svp(0.10,0.90,0.15,0.525);
      pg.swin(min_x,max_x,min_amp,max_amp);
      pg.box("BCNST",0.0,0,"BCNST",0.0,0);
      pg.sch(0.75);
      pg.sci(2);
      pg.errb(6, x1, amp1, amperr1, 2.0);
      pg.sci(3);
      pg.pt(x1, amp1, 17);
      pg.sci(4); pg.sls(1);
      pg.line(xfloatval, amp2);
/*
      pg.sci(4); pg.sls(4);  // plot each antenna soln
      pg.line(xfloatval, amp2a);
      pg.line(xfloatval, amp2b);
      pg.sls(1);
*/
      pg.sci(1) ; pg.sch(1.5);
      pg.lab(xlab, "Amp", " ");
      pg.sch(1.2);
      pg.mtxt("T",-1.5,0.95,1.0,ampdeg);
      pg.mtxt("B",-1.5,0.95,1.0,amprms);


    } else {
      // shouldn't get here because this wouldn't be one of the good baselines
      pg.sci(1);
      pg.env(0.0,1.0,0.0,1.0,0,-2);
      pg.sch(2.0);
      ostringstream oos;
      oos << "No data for baseline " << ant1idx(ibl) << " & " << ant2idx(ibl);
      pg.ptxt(0.5,0.5,0.0,0.5,oos);
      pg.sch(1.0);
    }
  }
}

//---------------------------------------------------------------------------

} //# NAMESPACE CASA - END

