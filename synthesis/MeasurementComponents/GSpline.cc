//# GJonesPoly.cc: Implementation of GJonesPoly.h
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
//# $Id: GJonesPoly.cc,v 19.15 2006/02/03 00:29:52 gmoellen Exp $

#include <synthesis/MeasurementComponents/GSpline.h>
#include <synthesis/MeasurementEquations/VisEquation.h>

#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/scimath/Mathematics/MatrixMathLA.h>
#include <casacore/casa/Quanta/MVTime.h>
//#include <casa/Containers/SimOrdMap.h>
#include <casacore/casa/Containers/Block.h>
#include <cmath>
#include <casacore/casa/fstream.h>

#include <casacore/casa/System/PGPlotter.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <msvis/MSVis/VisBuffAccumulator.h>
#include <synthesis/CalTables/GJonesMBuf.h>
#include <synthesis/CalTables/GJonesTable.h>
#include <synthesis/CalTables/CalIter.h>
#include <casacore/tables/Tables/TableUtil.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

using casacore::operator*;

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

GJonesSpline::GJonesSpline (VisSet& vs) :
  VisCal(vs),
  VisMueller(vs),
  GJones(vs),
  vs_p(&vs),
  solveAmp_p(false),
  solvePhase_p(true),
  splinetime_p(7200.0),
  //cacheTimeValid_p(0),
  calBuffer_p(NULL),
  //rawPhaseRemoval_p(false),
  solTimeStamp_p(0.0)
{
// Construct from a visibility set
// Input:
//    vs                VisSet&            Visibility set
// Output to private data:
//    solveAmp_p        Bool               true if mode_p includes amp. soln.
//    solvePhase_p      Bool               true if mode_p includes phase soln.
//    cacheTimeValid_p  Double             Time for which the current
//                                         calibration cache is valid
//    calBuffer_p       GJonesSplineMBuf*  Ptr to the applied cal. buffer
//

  if (prtlev()>2) cout << "GSpline::GSpline(vs)" << endl;

  // Mark the Jones matrix as neither solved for nor applied,
  // pending initialization by setSolver() or setInterpolation()
  setSolved(false);
  setApplied(false);

};

GJonesSpline::GJonesSpline (const MSMetaInfoForCal& msmc) :
  VisCal(msmc),
  VisMueller(msmc),
  GJones(msmc),
  vs_p(NULL),
  solveAmp_p(false),
  solvePhase_p(true),
  splinetime_p(7200.0),
  // cacheTimeValid_p(0),
  calBuffer_p(NULL),
  // rawPhaseRemoval_p(false),
  solTimeStamp_p(0.0)
{
// Construct from a MSMetaInfoForCal
// Input:
//    msmc              MSMetaInfoForCal&  MS Meta info server
// Output to private data:
//    solveAmp_p        Bool               true if mode_p includes amp. soln.
//    solvePhase_p      Bool               true if mode_p includes phase soln.
//    cacheTimeValid_p  Double             Time for which the current
//                                         calibration cache is valid
//    calBuffer_p       GJonesSplineMBuf*  Ptr to the applied cal. buffer
//

  if (prtlev()>2) cout << "GSpline::GSpline(msmc)" << endl;

  // Mark the Jones matrix as neither solved for nor applied,
  // pending initialization by setSolver() or setInterpolation()
  setSolved(false);
  setApplied(false);

};

//----------------------------------------------------------------------------

GJonesSpline::~GJonesSpline () 
{
// Virtual destructor
// Output to private data:
//    calBuffer_p       GJonesSplineMBuf*  Ptr to the applied cal. buffer
//

  if (prtlev()>2) cout << "GSpline::~GSpline(vs)" << endl;

  // Delete the calibration buffer
  if (calBuffer_p) delete(calBuffer_p);
};

//----------------------------------------------------------------------------

void GJonesSpline::setSolve(const Record& solvepar)
{
// Set the solver parameters
// Input:
//    solvepar            const Record&      Solver parameters
// Output to private data:
//    splinetime_p      Double             Spline knot timescale

  if (prtlev()>2) cout << "GSpline::setSolve()" << endl;

  // Call parent for generic pars
  SolvableVisCal::setSolve(solvepar);

  // Total solution interval is always all selecte data (for now)
  interval()=DBL_MAX;

  // Override nominal preavg handling
  //  (avoids interpretting -1 as pre-avg up to full interval)
  if (solvepar.isDefined("preavg")) 
    preavg() = solvepar.asDouble("preavg");

  // Spline-specific pars:
  if (solvepar.isDefined("splinetime")) 
    splinetime_p = solvepar.asDouble("splinetime");
  if (solvepar.isDefined("numpoint"))   
    numpoint_p   = solvepar.asInt("numpoint");
  if (solvepar.isDefined("phasewrap"))    // deg->rad
    phaseWrap_p  = solvepar.asDouble("phasewrap")*C::pi/180.0;

  // Interpret mode for SPLINE case
  //  (apmode now set in SVC::setsolve)
  solveAmp_p = (apmode().contains("A"));
  solvePhase_p = (apmode().contains("P"));

  //  cout << "solveAmp_p = " << solveAmp_p << endl;
  //  cout << "solvePhase_p = " << solvePhase_p << endl;
  //  cout << "calTableName() = " << calTableName() << endl;

  // Mark the Jones matrix as being solved for
  setSolved(true);

  return;
};

//----------------------------------------------------------------------------

void GJonesSpline::setApply(const Record& applypar)
{
// Set the apply parameters
// Input:
//    applypar     const Record&      Interpolation parameters
// Output to private data:
//    applyTable_p      String             Cal. table name containing
//                                         solutions to be applied
//    applySelect_p     String             Selection for the applied
//                                         calibration table
//    applyInterval_p   Double             Interpolation interval
//

  if (prtlev()>2) cout << "GSpline::setApply()" << endl;

  // Extract the parameters
  if (applypar.isDefined("table"))
    calTableName() = applypar.asString("table");
  if (applypar.isDefined("select"))
    calTableSelect() = applypar.asString("select");
  if (applypar.isDefined("t"))
    interval() = applypar.asDouble("t");

  // Attach a calibration buffer and iterator to the calibration 
  // table containing corrections to be applied
  GJonesSplineTable calTable(calTableName(), Table::Update);
  CalIter calIter(calTable);
  // Create the buffer and synchronize with the iterator
  if (calBuffer_p) delete calBuffer_p;
  calBuffer_p = new GJonesSplineMBuf(calIter);
  calBuffer_p->synchronize();
  calBuffer_p->fillCache();

  // Mark the Jones matrix as being applied
  setApplied(true);
  
  return;
};

//----------------------------------------------------------------------------

void GJonesSpline::selfGatherAndSolve (VisSet& vs, VisEquation& ve)
{
// Solver for the electronic gain in spline form
// Input:
//    me           VisEquation&         Measurement Equation (ME) in
//                                      which this Jones matrix resides
// Output:
//    solve        Bool                 true is solution succeeded
//                                      else false
//

  if (prtlev()>2) cout << "GSpline::selfGatherAndSolve(vs,ve)" << endl;


  //  cout << "Entering GSpline::solve." << endl;


  LogIO os (LogOrigin("GJonesSpline", "solve()", WHERE));


  os << LogIO::NORMAL
     << "Fitting time-dependent cubic splines."
     << LogIO::POST;
  if (solvePhase_p) {
    os << LogIO::NORMAL
       << "Solving for phase splines with splinetime= " << splinetime_p
       << LogIO::POST;
  }
  if (solveAmp_p) {
    os << LogIO::NORMAL
       << "Solving for amplitude splines with splinetime= " << splinetime_p
       << LogIO::POST;
  }

  // Arrange for iteration over data
  Block<Int> columns;
  // avoid scan iteration
  columns.resize(4);
  columns[0]=MS::ARRAY_ID;
  columns[1]=MS::FIELD_ID;      
  columns[2]=MS::DATA_DESC_ID;
  columns[3]=MS::TIME;
  vs.resetVisIter(columns,interval());
  VisIter& vi(vs.iter());
  VisBuffer vb(vi);

  // Count polarizations
  Int nPH(0);
  for (vi.originChunks(); vi.moreChunks(); vi.nextChunk()) {
    vi.origin();
    Int ncorr(vb.corrType().nelements());
    nPH= max(nPH,min(ncorr,2));
  }

  // nPH has the number of correlations (1 or 2) to process

  // Initialize time-series accumulation buffers for the
  // corrected and corrupted visibility data and associated
  // weights. 
  std::map<String,Int> timeValueMap;
  Vector<Double> timeValues;
  PtrBlock<Matrix<Complex>* > visTimeSeries;
  PtrBlock<Matrix<Double>* > weightTimeSeries;
  Int nTimeSeries = 0;

  // Initialize antenna indices
  Vector<Int> ant1(nBln(), -1);
  Vector<Int> ant2(nBln(), -1);

  Int k=0;
  for (Int a1=0; a1 < nAnt(); a1++) {
    for (Int a2=a1; a2 < nAnt(); a2++,k++) {
      ant1(k) = a1 + 1;
      ant2(k) = a2 + 1;
      //      cout << a1 << " " << a2 << " " << k << " " << blnidx(a1,a2) << endl;
    };
  };

  // Iterate, accumulating the averaged visibility time-series
  Int chunk;
  // Mean reference frequency
  Double meanFreq = 0;
  Int nMeanFreq = 0;

  // With current VisIter sort order, this chunking does
  //   all times for each spw and field
  // Double t0;
  for (chunk=0, vi.originChunks(); vi.moreChunks(); vi.nextChunk(), chunk++) {

    // Extract the current visibility buffer spectral window id.
    // and number for frequency channels
    Int spwid = vi.spectralWindow();
    Int nChan = vs.numberChan()(spwid);
    String fieldName = vi.fieldName();

    os << LogIO::NORMAL 
       << "Accumulating data for:  field= " << fieldName 
       << ", spw= " << spwid
       << ", nchan= " << nChan 
       << LogIO::POST;

    // Compute the corrected and corrupted data at the position of
    // this Jones matrix in the Measurement Equation. The corrupted
    // data are the model visibilities propagated along the ME from
    // the sky to the immediate right of the current Jones matrix.
    // The corrected data are the observed data corrected for all
    // Jones matrices up to the immediate left of the current Jones
    // matrix.

    // Arrange to accumulate
    VisBuffAccumulator vba(nAnt(),preavg(),false);

    vi.origin();
    //    t0=86400.0*floor(vb.time()(0)/86400.0);
    
    // Collapse each timestamp in this chunk according to VisEq
    //  with calibration and averaging
    for (vi.origin(); vi.more(); vi++) {

      // TBD: initialize weights from sigma here?
      
      ve.collapse(vb);
      
      // If permitted/required by solvable component, normalize
      if (normalizable())
        vb.normalize();
      
      // If this solve not freqdep, and channels not averaged yet, do so
      if (!freqDepMat() && vb.nChannel()>1)
	vb.freqAveCubes();

      // Accumulate collapsed vb in a time average
      vba.accumulate(vb);
    }
    vba.finalizeAverage();

    // The VisBuffer to work with in solve
    VisBuffer& svb(vba.aveVisBuff());

    //    cout << " (Chunk accumulated) nrow = " << svb.nRow() << endl;
    //    cout << " nChan = " << svb.nChannel() << endl;

    // NB: The svb VisBuffer has many timestamps in it....

    // index to parallel hands (assumes canonical order)
    Int nCorr = svb.corrType().nelements();
    Vector<Int> polidx(2,0);
    polidx(1)=nCorr-1;
    
    // Accumulate mean frequency of the averaged data
    meanFreq += mean(svb.frequency());
    nMeanFreq++;

    // Iterate over the current accumulated visibility buffer, obtaining
    // a common time series in the visibility data
    for (Int row=0; row < svb.nRow(); row++) {
      // Antenna numbers
      Int ant1num = svb.antenna1()(row);
      Int ant2num = svb.antenna2()(row);

      // Reject auto-correlation data
      if (ant1num != ant2num) {
	// Compute baseline index
	Int baselineIndex = blnidx(ant1num,ant2num);

	// Weight
	Vector<Float> rowWeight(svb.weightMat().column(row));
	if (sum(rowWeight) > 0) {
	  // Map the current time stamp to a time series index
	  // using 0.1 second precision, i.e. time values within
	  // a tenth of a second of each other will be accumulated
	  // within the same bin.
	  MVTime mvt(svb.time()(row) / C::day);
	  String timeKey = mvt.string(MVTime::TIME, 7);
	  Int timeIndex = 0;
	  // Check the time stamp index to this precision
      if (timeValueMap.find(timeKey) != timeValueMap.end( )) {
	    timeIndex = timeValueMap[timeKey];
	  } else {
	    // Create a new time series entry
	    timeIndex = nTimeSeries++;
	    timeValueMap.insert(std::pair<casacore::String, casacore::Int>(timeKey, timeIndex));
	    timeValues.resize(nTimeSeries, true);
	    timeValues(timeIndex) = svb.time()(row);
	    Complex czero(0,0);
	    visTimeSeries.resize(nTimeSeries, true);
	    visTimeSeries[timeIndex] = new Matrix<Complex>(nBln(),nPH, czero);
	    weightTimeSeries.resize(nTimeSeries, true);
	    weightTimeSeries[timeIndex] = new Matrix<Double>(nBln(),nPH, 0);
	  };

	  // Accumulate the current visbility row in the common time series
	  for (Int ip=0;ip<nPH;++ip) {
	    Double dwt=Double(rowWeight(polidx(ip)));
	    (*visTimeSeries[timeIndex])(baselineIndex,ip) +=
	      svb.visCube()(polidx(ip),0,row) * dwt;
	    (*weightTimeSeries[timeIndex])(baselineIndex,ip) += dwt;
	  }
	};
      };
    };
  }; // for (chunk...) iteration

  // Create amplitude, phase and weight arrays per time-slot
  // and interferometer index in the form required by the
  // GILDAS solver, splinant.
  Int nTimes = timeValueMap.size( );

  os << LogIO::NORMAL 
     << "Number of timestamps in data = " << nTimes
     << LogIO::POST;

  Vector<Double> time(nTimes, 0);
  Cube<Double> amp(nTimes, nBln(), nPH, 0);
  Cube<Double> phase(nTimes, nBln(), nPH, 0);
  Cube<Double> weight(nTimes, nBln(),nPH, 0);
  //
  // First create a simple index in order of ascending time ordinate
  Vector<Int> index(nTimes);
  indgen(index);
  for (Int j=0; j < nTimes; j++) {
    for (Int k=j+1; k < nTimes; k++) {
      if (timeValues(index(j)) > timeValues(index(k))) {
	Int swap = index(j);
	index(j) = index(k);
	index(k) = swap;
      };
    };
  };

  // Fill the amplitude, phase and weight arrays with normalized 
  // and scaled vaues from the accumulated time series.
  for (Int t=0; t < nTimes; t++) {
    for (Int baselineIndex=0; baselineIndex < nBln(); ++baselineIndex) {
      // Iterate in ascending time order
      Int indx = index(t);
      // Time
      time(t) = timeValues(indx);

      for (Int ip=0;ip<nPH;++ip) {

	Double wgt = (*weightTimeSeries[indx])(baselineIndex,ip);
	if (wgt > 0) {
	  Complex vis = (*visTimeSeries[indx])(baselineIndex,ip);
	  
	  // Amplitude
	  if (abs(vis) > 0) {
	    amp(t,baselineIndex,ip) = log(abs(vis)/wgt);
	  };

	  // Phase
	  if(solvePhase_p){
	    Double visPhase = arg(vis);
	    visPhase = remainder(visPhase, 2*C::pi);
	    if (visPhase > C::pi) {
	      visPhase -= 2 * C::pi;
	    } else if (visPhase < -C::pi) {
	      visPhase += 2 * C::pi;
	    };
	    phase(t,baselineIndex,ip) = visPhase;
	  }
	  

	  // Weight
	  weight(t,baselineIndex,ip) = wgt;
	  //weight(t,baselineIndex,ip) = 1.0;
	}
      };
    };
  };

  //  cout << "weight = " << weight << endl;
  //  cout << "weight.shape() = " << weight.shape() << endl;

  // Delete the accumulation buffers
  for (Int k=0; k < nTimes; k++) {
    delete(visTimeSeries[k]);
    delete(weightTimeSeries[k]);
  };


  //  Double t0=86400.0*floor(min(time)/86400.0);

  // Compute the number of spline knots
  Vector<Double> knots, ampKnots(1,0.0), phaKnots(1,0.0);
  Int nKnots = getKnots(time, knots);
  
  os << LogIO::NORMAL 
     << "Number of cubic spline control points = " << nKnots-4
     << LogIO::POST;

  os << LogIO::NORMAL 
     << "Number of cubic spline knots = " << nKnots
     << LogIO::POST;

  os << LogIO::NORMAL 
     << "Number of cubic spline segments = " << nKnots-7
     << LogIO::POST;

  if (nTimes < (nKnots-4) ) {
    os << LogIO::SEVERE
       << "Insufficient number of timestamps for cubic splines:" << endl 
       << "  Control points - nTimes > 0  (" << nKnots-4-nTimes << ")"
       << LogIO::POST;
    os << LogIO::SEVERE
       << "Increase splinetime or reduce preavg (if possible), or " << endl
       << "  solve for ordinary G instead."
       << LogIO::POST;
    return;
  };

  solTimeStamp_p = mean(knots);

  // Declare work arrays and returned value
  // arrays to be used by the solver
  Matrix<Double> wk1(4, nTimes);
  Matrix<Double> wk2(4*nAnt(), nKnots*nAnt());
  Vector<Double> wk3(nKnots*nAnt());
  Vector<Double> rmsfit(nBln(),0);

  Double dzero(0);

  // GSpline is nominally dual-pol
  Cube<Double> polyCoeffAmp(nAnt(),nKnots,2, dzero);
  Cube<Double> polyCoeffPhase(nAnt(),nKnots,2, dzero);

  // Fit using spline polynomials (GILDAS splinant)
  Bool dum;
  Int iy;
  // GILDAS solver uses one-relative antenna numbers
  Int rAnt = refant() + 1;
  Int nBL(nBln());
  Int nA(nAnt());

  if (solveAmp_p) {
    // Amplitude solution

    os << LogIO::NORMAL 
       << "Fitting amplitude spline."
       << LogIO::POST;

    ampKnots.resize(knots.shape());
    ampKnots=knots;

    iy = 1;
    
    for (Int ip=0;ip<nPH;++ip) {

      splinant(&iy,
	       &nTimes,
	       &nBL,
	       ant1.getStorage(dum),
	       ant2.getStorage(dum),
	       &rAnt,
	       &nKnots,
	       &nA,
	       time.getStorage(dum),
	       amp.xyPlane(ip).getStorage(dum),
	       weight.xyPlane(ip).getStorage(dum),
	       knots.getStorage(dum),
	       wk1.getStorage(dum),
	       wk2.getStorage(dum),
	       wk3.getStorage(dum),
	       rmsfit.getStorage(dum),
	       polyCoeffAmp.xyPlane(ip).getStorage(dum));
    
      ostringstream o;
      o << calTableName() << ".AMP.pol" << ip << ".log";
      String logfile=o.str();
      writeAsciiLog(logfile, polyCoeffAmp.xyPlane(ip), rmsfit, false);
      //    plotsolve(time, amp, weight, rmsfit, polyCoeffAmp, false);

    }

  }

  if (solvePhase_p){
    // Phase solution

    os << LogIO::NORMAL 
       << "Searching for and correcting phase-wraps on each baseline."
       << LogIO::POST;

    for (Int ip=0;ip<nPH;++ip) {
      
      for(Int bsInd=0; bsInd < nBln() ; ++bsInd){
	
	Int npoi=numpoint_p; 
	if (npoi < 2) npoi=2;
	
	for (Int k1=npoi; k1 < nTimes-npoi; k1=k1+(npoi/2)){
	  Vector<Double> vecAheadPh(npoi), vecBheadPh(npoi);
	  for (Int k=0; k < npoi; ++k){
	    vecBheadPh[k]=phase(k1-k-1,bsInd,ip);
	    vecAheadPh[k]=phase(k1+k, bsInd,ip);
	  }
	  Double medA=median(vecAheadPh);
	  Double medB=median(vecBheadPh);
	  if((medA- medB) >phaseWrap_p)
	    for(Int k=k1; k< nTimes; ++k)
	      phase(k, bsInd,ip) -= 2*C::pi;
	  if((medA-medB) < -phaseWrap_p)
	    for(Int k=k1; k< nTimes; ++k)
	      phase(k, bsInd,ip) += 2*C::pi;
	  
	}
	
	if(nTimes >npoi*2){
	  
	  //the last npoi pts
	  Vector<Double> vecAheadPh(npoi), vecBheadPh(npoi);
	  for (Int k=0; k <npoi; ++k){
	    vecBheadPh[k]=phase(nTimes-2*npoi+k,bsInd,ip);
	    vecAheadPh[k]=phase(nTimes-npoi+k, bsInd,ip);
	  }
	  Double medA=median(vecAheadPh);
	  Double medB=median(vecBheadPh);
	  if((medA- medB) > phaseWrap_p)
	    for(Int k=(nTimes-npoi); k< nTimes; ++k)
	      phase(k, bsInd,ip) -= 2*C::pi;
	  if((medA-medB) < -phaseWrap_p)
	    for(Int k=(nTimes-npoi); k< nTimes; ++k)
	      phase(k, bsInd,ip) += 2*C::pi;
	  
	  // The first npoi points
	  
	  for (Int k=0; k <npoi; ++k){
	    vecBheadPh[k]=phase(k,bsInd,ip);
	    vecAheadPh[k]=phase(npoi+k, bsInd,ip);
	  }
	  medA=median(vecAheadPh);
	  medB=median(vecBheadPh);
	  if((medA- medB) > phaseWrap_p)
	    for(Int k=0; k< npoi; ++k)
	      phase(k, bsInd,ip) += 2*C::pi;
	  if((medA-medB) < -phaseWrap_p)
	    for(Int k=0; k< npoi; ++k)
	      phase(k, bsInd,ip) -= 2*C::pi;
	  
	}
      }
    }

    os << LogIO::NORMAL 
       << "Fitting phase spline."
       << LogIO::POST;

    phaKnots.resize(knots.shape());
    phaKnots=knots;
    iy = 2;

    for (Int ip=0;ip<nPH;++ip) {

      wk1=0.0; wk2=0.0; wk3=0.0;

      splinant(&iy,
	       &nTimes,
	       &nBL,
	       ant1.getStorage(dum),
	       ant2.getStorage(dum),
	       &rAnt,
	       &nKnots,
	       &nA,
	       time.getStorage(dum),
	       phase.xyPlane(ip).getStorage(dum),
	       weight.xyPlane(ip).getStorage(dum),
	       knots.getStorage(dum),
	       wk1.getStorage(dum),
	       wk2.getStorage(dum),
	       wk3.getStorage(dum),
	       rmsfit.getStorage(dum),
	       polyCoeffPhase.xyPlane(ip).getStorage(dum));


      //      cout << "Finished solve." << endl;


      ostringstream o;
      o << calTableName() << ".PHASE.pol" << ip << ".log";
      String logfile=o.str();
      writeAsciiLog(logfile, polyCoeffPhase.xyPlane(ip), rmsfit, true);

      // TBD: make multi-pol plotsolve
      //      plotsolve(time, phase, weight, rmsfit, polyCoeffPhase, true);

    }



  };

  // Update the output calibration table
  Vector<Int> antId(nAnt());
  indgen(antId);
  Vector<Int> fieldIdKeys = fieldIdRange();
  Vector<String> freqGrpName(nAnt(), "");
  Vector<String> polyType(nAnt(), "SPLINE");
  Vector<String> polyMode(nAnt(), apmode());
  if (apmode()=="AP")
    polyMode="PHASAMP";
  if (apmode()=="P")
    polyMode="PHAS";
  if (apmode()=="A")
    polyMode="AMP";

  Vector<Complex> scaleFactor(nAnt(), Complex(1,0));
  Vector<String> phaseUnits(nAnt(), "RADIANS");
  Vector<Int> refAnt(nAnt(), refant());
  
  // Use mean frequency as the reference frequency
  if (nMeanFreq > 0) meanFreq = meanFreq / nMeanFreq;
  Vector<MFrequency> refFreq(nAnt(), MFrequency(Quantity(meanFreq, "Hz")));

  // If not incremental calibration then create an empty
  // calibration buffer to hold the output calibration solutions
  if (!calBuffer_p) {
    newCalBuffer(fieldIdKeys, antId);
  } else {
    // Force an explicit read to cache for all calibration columns
    // (in case the input calibration table is being overwritten
    // in place)
    calBuffer_p->fillCache();

  };

  // Update the calibration table
  //  cout << "ampKnots.nelements() = " << ampKnots.nelements() << endl;
  //  cout << "phaKnots.nelements() = " << phaKnots.nelements() << endl;
  //  cout << "nKnots = " << nKnots << endl;

  //  cout << "polyCoeffPhase.shape() = " << polyCoeffPhase.shape() << endl;
  //  cout << "polyCoeffPhase.reform(IPosition(nAnt(),ampKnots.nelements()*2)).shape() = " 
  //       << polyCoeffPhase.reform(IPosition(2,nAnt(),nKnots*2)).shape() << endl;


  Matrix<Double> ampco(polyCoeffAmp.reform(IPosition(2,nAnt(),nKnots*2)));
  Matrix<Double> phaco(polyCoeffPhase.reform(IPosition(2,nAnt(),nKnots*2)));

  updateCalTable(fieldIdKeys, antId, freqGrpName, polyType, polyMode, 
		 scaleFactor, 
		 ampco,phaco,
		 //		 polyCoeffAmp.xyPlane(0), polyCoeffPhase.xyPlane(0), 
		 phaseUnits, ampKnots, phaKnots, refFreq, refAnt);
	
  return;
};

//----------------------------------------------------------------------------

void GJonesSpline::calcPar() {

// Calculate new gain parameters (in this case, the G matrix elements)

  if (prtlev()>6) cout << "      GSpline::calcPar()" << endl;

  LogIO os (LogOrigin("GJonesSpline", "calcPar", WHERE));

  Double freqRatio=1.0;
  Double vbFreqHz = 1.0e9*mean(currFreq());

  //  cout << "vbFreqHz = " << vbFreqHz << endl;

  currCPar().resize(nPar(),1,nAnt());
  currParOK().resize(nPar(),1,nAnt());
  currParOK()=false;

  // Compute them, per antenna
  for (Int iant=0; iant < nAnt(); iant++) {

    // Match this antenna id. and the visibility buffer field id. 
    // in the calibration buffer
    Vector<Int> matchingRows = 
      calBuffer_p->matchAntenna1AndFieldId(iant,currField());

    if (matchingRows.nelements() > 0) {
      // Matching calibration solution found
      Int row = matchingRows(0);
      Vector<Double> ampVal(2,1.0);
      Vector<Double> phaseVal(2,0.0);
      Complex gain(calBuffer_p->scaleFactor()(row));
      String mode = calBuffer_p->polyMode()(row);

      //      cout << "gain = " << gain << endl;

      // Compute the ratio between the calibration solution 
      // reference frequency and the mean observed frequency
      // of the visibility data to be corrected
      IPosition refFreqPos = calBuffer_p->refFreqMeas().shape();
      refFreqPos = 0;
      refFreqPos.setLast(IPosition(1,row));
      MFrequency refFreq = calBuffer_p->refFreqMeas()(refFreqPos);
      Double refFreqHz = refFreq.get("Hz").getValue();
      freqRatio = abs(refFreqHz) > 0 ? vbFreqHz / refFreqHz : 1.0;
      //      cout << "freqRatio = " << freqRatio << endl;

      // Compute amplitude polynomial
      if (mode.contains("AMP") || mode.contains("A&P")) {
	// Extract amplitude spline polynomial knots
	IPosition ampKnotsPos = calBuffer_p->splineKnotsAmp().shape();
	ampKnotsPos = 0;
	Int nAmpKnotsPos = ampKnotsPos.nelements();
	ampKnotsPos[nAmpKnotsPos-1] = row;
	Int nKnots = calBuffer_p->nKnotsAmp()(row);
	Vector<Double> ampKnots(nKnots);
	for (Int k=0; k < nKnots; k++) {
	  ampKnotsPos[nAmpKnotsPos-2] = k;
	  ampKnots(k) = calBuffer_p->splineKnotsAmp()(ampKnotsPos);
	};
	
	// Extract amplitude spline polynomial coefficients
	IPosition ampCoeffPos = calBuffer_p->polyCoeffAmp().shape();
	ampCoeffPos = 0;
	Int nAmpCoeffPos = ampCoeffPos.nelements();
	ampCoeffPos[nAmpCoeffPos-1] = row;
	Int nPoly = calBuffer_p->nPolyAmp()(row);
	Vector<Double> ampCoeff(2*nPoly);
	for (Int k=0; k < 2*nPoly; k++) {
	  ampCoeffPos[nAmpCoeffPos-2] = k;
	  ampCoeff(k) = calBuffer_p->polyCoeffAmp()(ampCoeffPos);
	};

	//	cout << "ampCoeff = " << ampCoeff << endl;

	// Compute amplitude spline polynomial value
	Vector<Double> ac;
	for (Int i=0;i<2;++i) {
	  ac.reference(ampCoeff(IPosition(1,i*nPoly),
				IPosition(1,(i+1)*nPoly-1)));
	  ampVal(i) *= exp(getSplineVal(currTime(), ampKnots, ac));
	}
	//	cout << iant << " ampVal = " << ampVal << endl;
      };

      // Compute phase polynomial
      if (mode.contains("PHAS") || mode.contains("A&P")) {

	  // Extract phase spline polynomial knots
	  IPosition phaseKnotsPos = calBuffer_p->splineKnotsPhase().shape();
	  phaseKnotsPos = 0;
	  Int nPhaseKnotsPos = phaseKnotsPos.nelements();
	  phaseKnotsPos[nPhaseKnotsPos-1] = row;
	  Int nKnots = calBuffer_p->nKnotsPhase()(row);
	  Vector<Double> phaseKnots(nKnots);
	  for (Int k=0; k < nKnots; k++) {
	    phaseKnotsPos[nPhaseKnotsPos-2] = k;
	    phaseKnots(k) = calBuffer_p->splineKnotsPhase()(phaseKnotsPos);
	  };
	  
	  // Extract phase spline polynomial coefficients
	  IPosition phaseCoeffPos = calBuffer_p->polyCoeffPhase().shape();
	  phaseCoeffPos = 0;
	  Int nPhaseCoeffPos = phaseCoeffPos.nelements();
	  phaseCoeffPos[nPhaseCoeffPos-1] = row;
	  Int nPoly = calBuffer_p->nPolyPhase()(row);
	  Vector<Double> phaseCoeff(2*nPoly);
	  for (Int k=0; k < 2*nPoly; k++) {
	    phaseCoeffPos[nPhaseCoeffPos-2] = k;
	    phaseCoeff(k) = calBuffer_p->polyCoeffPhase()(phaseCoeffPos);
	  };

	  //	  cout << "phaseCoeff = " << phaseCoeff << endl;
	  
	  // Compute phase spline polynomial value
	  Vector<Double> pc;
	  for (Int i=0;i<2;++i) {
	    pc.reference(phaseCoeff(IPosition(1,i*nPoly),
				    IPosition(1,(i+1)*nPoly-1)));

	    //	    cout << "pc = " << pc << endl;

	    phaseVal(i) = getSplineVal(currTime(), phaseKnots, pc);
	  
	    // Handle gildas sign convention on spline phases
	    phaseVal(i) = -phaseVal(i);
	  
	    // Scale by the ratio of the observing frequency of the 
	    // data to be corrected and the reference frequency of the
	    // calibration solution
	    phaseVal(i) *= freqRatio;
	  }
      };

      //      cout << "phaseVal = " << phaseVal << endl;
      
      // Fill the (matrix element) parameters
      for (Int i=0;i<2;++i) {
	currCPar()(i,0,iant) = gain * ampVal(i) * Complex(cos(phaseVal(i)), 
							  sin(phaseVal(i)) );
	currParOK()(i,0,iant) = true;
      }
      
    };
  };

  //cout << "currCPar() = " << currCPar() << endl;

  validateP();
  invalidateJ();

  return;
};

//----------------------------------------------------------------------------

void GJonesSpline::newCalBuffer (const Vector<Int>& fieldIdKeys,
				 const Vector<Int>& antennaId)
{
// Create and fill and empty output calibration buffer
// Input:
//    fieldIdKeys      const Vec<Int>&        Field id.'s to span
//    antennaId        const Vec<Int>&        Antenna id.'s to span
// Output to private data:
//    calBuffer_p      GJonesSplineMBuf*      Calibration buffer
//
  LogIO os (LogOrigin("GJonesSpline", "newCalBuffer", WHERE));

  // Delete calibration buffer if it already exists
  if (calBuffer_p) delete calBuffer_p;

  // Initialize the GJonesSpline calibration buffer, spanning the 
  // antenna id.'s and field id.'s specified.
  Vector<Int> key(2);
  key(0) = MSC::ANTENNA1;
  key(1) = MSC::FIELD_ID;
  Block<Vector<Int> > keyvals(2);
  keyvals[0] = antennaId;
  keyvals[1] = fieldIdKeys;
  calBuffer_p = new GJonesSplineMBuf(key, keyvals);

  return;
};

//----------------------------------------------------------------------------

Int GJonesSpline::getKnots (const Vector<Double>& times, Vector<Double>& knots)
{
// Compute the number and location of the spline knots
// Input:
//    times        const Vector<Double>&    Vector of time values
// Output:
//    knots        Vector<Double>&          Knot positions
//    getKnots     Int                      Number of knots
//
  LogIO os (LogOrigin("GJonesSpline", "getKnots()", WHERE));

  // Use algorithm in GILDAS, with user-defined timescale
  Int n=times.nelements();
  Int nSeg = Int(0.5 + (times(n-1)-times(0))/splinetime_p);
  nSeg = max(1,nSeg);     // never fewer than 1, obviously
  Int ncenterKnots = nSeg -1;
  Double step = (times(n-1) - times(0)) / nSeg;

  os << LogIO::NORMAL
     << "Gridded splinetime = " << step << " sec."
     << LogIO::POST;

  Int numOfknots = ncenterKnots + 8; 
  //cout << "numOfknots" << numOfknots << endl;
  knots.resize(numOfknots);
  knots(0)=knots(1) = knots(2) = knots(3) = times(0);
  knots(numOfknots-1) = knots(numOfknots-2) = knots(numOfknots-3) =
    knots(numOfknots-4)=times(n-1);
  for(Int k=0; k < ncenterKnots; k++){
    knots(k+4) = times(0)+ (k + 1) * step;
  }

  return numOfknots;
};

//----------------------------------------------------------------------------

void GJonesSpline::updateCalTable (const Vector<Int>& fieldIdKeys,
				   const Vector<Int>& antennaId,
				   const Vector<String>& freqGrpName,
				   const Vector<String>& polyType,
				   const Vector<String>& polyMode,
				   const Vector<Complex>& scaleFactor,
				   const Matrix<Double>& polyCoeffAmp,
				   const Matrix<Double>& polyCoeffPhase,
				   const Vector<String>& phaseUnits,
				   const Vector<Double>& splineKnotsAmp,
				   const Vector<Double>& splineKnotsPhase,
				   const Vector<MFrequency>& refFreq,
				   const Vector<Int>& refAnt)
{
// Update the output calibration table
// Input:
//    fieldIdKeys      const Vec<Int>&        Field id.'s to span
//    antennaId        const Vec<Int>&        Antenna id. for each solution
//    freqGrpName      const Vec<String>&     Freq. group name (per soln.)
//    polyType         const Vec<String>&     Polynomial type (per soln.)
//    polyMode         const Vec<String>&     Polynomial mode (per soln.)
//    scaleFactor      const Vec<Complex>&    Polynomial scale factor 
//                                            (per solution)
//    polyCoeffAmp     const Matrix<Double>&  Polynomial amplitude 
//                                            coefficients (per solution)
//    polyCoeffPhase   const Matrix<Double>&  Polynomial phase coefficients
//                                            (per solution)
//    phaseUnits       const Vec<String>&     Phase units
//    splineKnotsAmp   const Vector<Double>&  Amp. spline knot positions
//                                            (same for all solutions)
//    splineKnotsPhase const Vector<Double>&  Phase spline knot positions
//                                            (same for all solutions)
//    refFreq          const Vec<MFreq>&      Reference freq. (per soln.)
//    refAnt           const Vec<Int>&        Reference antenna (per soln.)
//
  LogIO os (LogOrigin("GJonesSpline", "updateCalTable", WHERE));

  // Add each solution to the calibration buffer
  for (uInt k=0; k < antennaId.nelements(); k++) {
    for (uInt j=0; j < fieldIdKeys.nelements(); j++) {
      // Find matching rows for this antenna and field id.
      Vector<Int> matchingRows = 
	calBuffer_p->matchAntenna1AndFieldId(antennaId(k), fieldIdKeys(j));

      // Update the matching rows in the calibration buffer
      calBuffer_p->fillMatchingRows(matchingRows, freqGrpName(k), polyType(k), 
				    polyMode(k), scaleFactor(k), 
				    polyCoeffAmp.row(k).nelements()/2,
				    polyCoeffPhase.row(k).nelements()/2,
				    polyCoeffAmp.row(k), polyCoeffPhase.row(k),
				    phaseUnits(k), splineKnotsAmp.nelements(),
				    splineKnotsPhase.nelements(),
				    splineKnotsAmp, splineKnotsPhase,
				    refFreq(k), refAnt(k));
    };
  };

  //  calBuffer_p->fieldId().set(-1);
  calBuffer_p->calDescId().set(0);
  //  cout << "Time = " << MVTime(solTimeStamp_p/C::day).string(MVTime::YMD,7) << endl;
  calBuffer_p->timeMeas().set(MEpoch(MVEpoch(solTimeStamp_p/86400.0)));

  // Delete the output calibration table if it already exists
  if (calTableName()!="" && TableUtil::canDeleteTable(calTableName())) {
    TableUtil::deleteTable(calTableName());
  };

  os << LogIO::NORMAL
     << "Storing solutions in table " << calTableName()
     << LogIO::POST;

  // Write the calibration buffer to the output calibration table
  Table::TableOption accessMode = Table::New;
  if (Table::isWritable(calTableName())) accessMode = Table::Update;
  GJonesSplineTable calTable(calTableName(), accessMode);
  calBuffer_p->append(calTable);


  // Add CAL_DESC
  CalDescRecord cdr;
  cdr.defineSpwId(Vector<Int>(1,-1));
  cdr.defineMSName(Path(msName()).baseName());
  calTable.putRowDesc(0,cdr);

  return;
};

//----------------------------------------------------------------------------

Double GJonesSpline::getSplineVal (Double x, 
				   Vector<Double>& knots,
				   Vector<Double>& coeff)
{
// Compute a polynomial spline value using the GILDAS routine getbspl
// Input:
//    x             Double                   Value at which to compute spline
//    knots         Vector<Double>&          Knot locations
//    coeff         Vector<Double>&          Spline coefficients
// Output:
//    getSplineVal  Double                   Computed spline value
//
  LogIO os (LogOrigin("GJonesSpline", "getSplineVal()", WHERE));

  // Use GILDAS library routine, getbspl
  Int numOfknots=knots.nelements();
  Int failflag;
  Bool dum;
  Double yval;
  getbspl(&numOfknots, 
	  knots.getStorage(dum),
	  coeff.getStorage(dum),
	  &x,
	  &yval,
	  &failflag);
  return yval;
}

//----------------------------------------------------------------------------
void GJonesSpline::plotsolve(const Vector<Double>& x, 
			   const Matrix<Double>& yall, 
			   const Matrix<Double>& weightall, 
			   const Vector<Double>& errall, 
			   Matrix<Double>& coeff, Bool phasesoln){



  // Function to plot and compare Bandpass data and solution
  // Input:
  // x - vector of frequecies
  // yall - matrix of data - shape is yall(freqindex, baselineindex)
  // weightall - matrix of weight; same shape as yall
  // errall - error of fits for each baseline
  // coeff - matrix coefficients of polynomial fits -shape coeff(nant, degree)
  // phasesoln - either phase or amplitude plot


  LogIO os (LogOrigin("GJonesSpline", "plotsolve()", WHERE));

  String device;
  device = calTableName();
  if (phasesoln){
    device = device + ".PHASE.ps/cps";
  }
  else{
    device = device + ".AMP.ps/cps";
  }

  os << LogIO::NORMAL 
     << "Generating plot file: " << device
     << LogIO::POST;

  PGPlotter pg(device);
  pg.subp(4,3);
  Vector<Double> knots;
  getKnots(x,knots);
  Int numpoints= max(x.shape().asVector());
  Int numplotpoints=20*numpoints;
  Int num_valid_points=0;
  Vector<Float> x1(numpoints);
  Vector<Float> y1(numpoints);
  Vector<Float> errarray(numpoints); 
  Vector<Double>  xval(numplotpoints);
  Vector<Float> xfloatval(numplotpoints);
  Vector<Float> soly1(numplotpoints);
  Vector<Double> y, weight;
  Double err;
  Int num_ant = nAnt();
  Vector<Double> ant1coeff, ant2coeff; 

  //  Float max_data, min_data, max_err;
  //  max_err=max(errall);
  //  max_data=max(yall)+1.5*max_err;
  //  min_data=min(yall)-1.5*max_err;

  for (Int ant1=0; ant1 < num_ant; ++ant1){
    for (Int ant2=ant1+1; ant2 < num_ant; ++ant2){
      Int basnum=ant1*num_ant-ant1*(ant1+1)/2+ant2-1-ant1;

      x1.resize(numpoints);
      y1.resize(numpoints);
      errarray.resize(numpoints);
      y.resize(); 
      weight.resize();
      ant1coeff=coeff.row(ant1);
      ant2coeff=coeff.row(ant2);
      y=yall.column(basnum);
      weight=weightall.column(basnum);  
      err=errall[basnum];
      num_valid_points=0;

      for(Int k=0; k < numpoints; k++){
	if (weight(k)>0){
	  x1(num_valid_points)=x(k)-x(0);
	  y1(num_valid_points)=y(k);
	  if(phasesoln==true){
	    y1(num_valid_points)=remainder(y1(num_valid_points), 2*C::pi);
	    y1(num_valid_points)=y1(num_valid_points)/C::pi*180.0;
	    errarray(num_valid_points)=Float(err)*180.0/C::pi;
	  } 
	  else{
	    y1(num_valid_points)=exp(y1(num_valid_points));
	    errarray(num_valid_points)=Float(err)*y1(num_valid_points);
	  }
	  num_valid_points+=1;
	}; // weight(k) > 0
      }; // for k=0
      
      if(num_valid_points == 0){
      	cout << "No valid point on baselines with antennas " 
	     <<  ant1 << " & " << ant2 << endl;
	return;  
      }

      for(Int k=0; k < numplotpoints; k++){
	xval[k]= (k)*((x[numpoints-1]-x[0]))/Double(numplotpoints-1)
	  +(x[0]);	
	
	if(phasesoln==true){
	  soly1(k)=0; 
	  Double xx, yy1, yy2;
	  xx=xval[k];
	  yy1=getSplineVal(xx, knots, ant1coeff);
	  yy2=getSplineVal(xx, knots, ant2coeff);
	  soly1(k)=yy2-yy1;
	  while(soly1(k) > C::pi){
	    soly1(k) -= 2.0*(C::pi);
	  }
	  while(soly1(k) < (-(C::pi))){
	    soly1(k) += 2.0*(C::pi);
	  }
	  soly1(k)=soly1(k)/(C::pi)*180.0;
		      
	}
	else{

	  Double xx, yy1, yy2;
	  xx=xval[k];
	  yy1=getSplineVal(xx, knots, ant1coeff);
	  yy2=getSplineVal(xx, knots, ant2coeff);  
	  soly1[k]=(yy1+yy2);
	  soly1[k]=exp(soly1[k]);
	  
	  
	  
	}
	
	xfloatval(k)=Float(xval(k)-xval(0));
      }


      //cout << " Mean of sol " << mean(soly1) << endl;
      x1.resize(num_valid_points, true);
      y1.resize(num_valid_points, true);
      errarray.resize(num_valid_points, true);
      pg.sci(1);

      Float max_data, min_data, max_err;
      max_err=max(errarray);
      max_data=max(y1);
      min_data=min(y1);
      max_data=max(max_data, max(soly1))+1.5*max_err;
      min_data=min(min_data, min(soly1))-1.5*max_err;

      Float min_x= min(xfloatval);
      Float max_x= max(xfloatval);

      Float edge=(max_x -min_x)*0.05;
      min_x-=edge;
      max_x+=edge;

      pg.sch(1.5);
      //      pg.env(min_x, max_x, min_data, max_data, 0, 0);
      pg.page();
      pg.svp(0.1,0.9,0.15,0.9);
      pg.swin(min_x,max_x,min_data,max_data);
      pg.box("BCNST",0.0,0,"BCNST",0.0,0);

      ostringstream oos;
      if(phasesoln){
	oos << "G SPLINE phase for baseline " << ant1+1 << " & " << ant2+1 ;
	pg.lab("Time in s", "Phase in deg", " ");
	pg.mtxt("T",0.75,0.5,0.5,oos);
      }
      else{
	oos << "G SPLINE amplitude for baseline " << ant1+1 << " & " << ant2+1 ;
	pg.lab("Time in s", "Amplitude", " ");
	pg.mtxt("T",0.75,0.5,0.5,oos);
      }
      pg.sch(0.75);
      pg.sci(2);
      pg.errb(6, x1, y1, errarray, 2.0);
      pg.sci(3);
      pg.pt(x1, y1, 17);
      pg.sci(4);
      pg.line(xfloatval, soly1);

    }
  }

}

//---------------------------------------------------------------------------

Vector<Int> GJonesSpline::fieldIdRange()
{
// Return all field id.'s in the underlying MS
// Output:
//    fieldIdRange      Vector<Int>        All FIELD_ID's in the MS
//
  // Open the FIELD sub-table

  if (!vs_p) throw(AipsError("Error in GJonesSpline::fieldIdRange()"));

  const MSColumns& mscol(vs_p->iter().msColumns());
  const MSFieldColumns& fldCol(mscol.field());

  // Fill vector containing all field id.'s
  Vector<Int> retval(fldCol.nrow());
  indgen(retval);

  return retval;
};

//----------------------------------------------------------------------------

/*  TBD... (no raw phase processing, for now)

void GJonesSpline::setRawPhaseVisSet(VisSet& vs){

  Block<Int> columns(0);
  
  rawvs_p = new VisSet (vs, columns, DBL_MAX);
  rawPhaseRemoval_p=true;
  fillRawPhaseBuff();
}

void GJonesSpline::fillRawPhaseBuff(){

  //Make a phase array of BaselineIndex and TimeIndex of raw phases
  LogIO os (LogOrigin("GJonesSpline", "fillRawPhaseBuff()", WHERE));


 // Use the iterator from the underlying visibility set,
  // and attach a visibility data buffer
  VisIter& vi(rawvs_p->iter());
  VisBuffer vb(vi);

  // Initialize stuff
  // SimpleOrderedMap<String,Int> timeValueMap(0);
  Vector<Double> timeValues;
  PtrBlock<Vector<Complex>* > rawVisTimeSeries;
  PtrBlock<Vector<Double>* > weightTimeSeries;
  Int nTimeSeries = 0;

  // Initialize the baseline index
  Int nAnt = vs_p->numberAnt();
  Int nBasl = nAnt * (nAnt - 1) / 2;


  // Iterate, accumulating the averaged visibility time-series
  Int chunk;

  for (chunk=0, vi.originChunks(); vi.moreChunks(); vi.nextChunk(), chunk++) {
    // Extract the current visibility buffer spectral window id.
    // and number for frequency channels
    Int spwid = vi.spectralWindow();
    Int nChan = rawvs_p->numberChan()(spwid);
    String fieldName = vi.fieldName();

    os << LogIO::NORMAL << "Slot= " << (chunk+1) << ", field= "
       << fieldName << ", spw= " << spwid+1 
       << ", nchan= " << nChan << LogIO::POST;

    //average the buffer across frequency
   for(vi.origin(); vi.more(); vi++){
 
     vb.visibility()=vb.correctedVisibility();
     vb.freqAverage();

     //     cout << "AFTER freq aver length" << vb.correctedVisibility().shape().asVector() << endl;

    // Iterate of the current visibility buffer, accumulating
    // a common time series in the visibility data
    for (Int row=0; row < vb.nRow(); row++) {
      // Antenna numbers
      Int ant1num = vb.antenna1()(row);
      Int ant2num = vb.antenna2()(row);

      // Reject auto-correlation data
      if (vb.antenna1()(row) != vb.antenna2()(row)) {
	// Compute baseline index
	Int baselineIndex = ant1num * nAnt - ant1num * (ant1num + 1) / 2 +
	  ant2num - 1 - ant1num;

	// Weight
	Double rowWeight = vb.weight()(row);
	if (rowWeight > 0) {
	  // Map the current time stamp to a time series index
	  // using 0.1 second precision, i.e. time values within
	  // a tenth of a second of each other will be accumulated
	  // within the same bin.
	  MVTime mvt(vb.time()(row) / C::day);
	  String timeKey = mvt.string(MVTime::TIME, 7);
	  //	  cout << "FILL Bef timekey "<< timeKey << " ntimeseries " << nTimeSeries << endl;
	  Int timeIndex = 0;
	  // Check the time stamp index to this precision
	  if (timeValueMap_p.isDefined(timeKey)) {
	    timeIndex = timeValueMap_p(timeKey);
	  } else {
	    // Create a new time series entry
	    timeIndex = nTimeSeries++;
	    //	    cout << "FILL timekey "<< timeKey << " index " << timeIndex << endl;
	    timeValueMap_p.define(timeKey, timeIndex);
	    timeValues.resize(nTimeSeries, true);
	    timeValues(timeIndex) = vb.time()(row);
	    rawVisTimeSeries.resize(nTimeSeries, true);
	    Complex czero(0,0);
	    rawVisTimeSeries[timeIndex] = new Vector<Complex>(nBasl, czero);
	    weightTimeSeries.resize(nTimeSeries, true);
	    weightTimeSeries[timeIndex] = new Vector<Double>(nBasl, 0);
	  };

	  //CAUTION going to use corrected
	  // Accumulate the current visbility row in the common time series
	  (*rawVisTimeSeries[timeIndex])(baselineIndex) +=
	    vb.visibility()(0,row)(0) * rowWeight;
	  (*weightTimeSeries[timeIndex])(baselineIndex) += rowWeight;
	};
      };
    };
    };
  }; // for (chunk...) iteration

  Int nTimes = timeValueMap_p.ndefined();
  rawPhase_p.resize(nTimes, nBasl);
  for (Int k=0; k< nTimes; ++k){
    for(Int j=0; j< nBasl; ++j){
      //rawPhase_p(k, j)=arg((*(rawVisTimeSeries[k]))(j));
      rawPhase_p(k,j)=arg(((*rawVisTimeSeries[k])(j))
      		  / ((*weightTimeSeries[k])(j)));

    }

  }



}


Double GJonesSpline::getRawPhase(Int ant1, Int ant2, Double time){

  if (ant1 == ant2) return 0.0;
  Int nAnt = vs_p->numberAnt();
  Bool flip=false;
  if(ant1 > ant2){
    Int tmp=ant1;
    ant1=ant2;
    ant2=tmp;
    flip=true;
  }
  Int baselineIndex = ant1 * nAnt - ant1 * (ant1 + 1) / 2 +
                      ant2 - 1 - ant1;

  MVTime mvt(time / C::day);
  String timeKey = mvt.string(MVTime::TIME, 7);
  Int timeIndex=timeValueMap_p(timeKey);
  // if (ant1==0 && ant2==1) 
  //   cout << "phase 1 2 " << rawPhase_p(timeIndex, baselineIndex)*180.0/C::pi << " Time " << timeKey << endl;
  //   cout << " Timekey " << timeKey << "Timeindex " << timeIndex << endl;
  if(flip){ 
    return -rawPhase_p(timeIndex, baselineIndex);
  }
  else {
    return rawPhase_p(timeIndex, baselineIndex);
  }

}

*/

void GJonesSpline::writeAsciiLog(const String& filename, const Matrix<Double>& coeff, const Vector<Double>& rmsFit, Bool phasesoln){

  Int numAnt=coeff.shape().asVector()(0);
  const char* fil=filename.chars();
  ofstream outLog(fil);

  if(phasesoln){
    outLog << "*****PHASE summary********"<< endl;
  }
  else{
    outLog << "*****AMPLITUDE summary********"<< endl;
  }
    outLog << "Antenna based spline coefficients " << endl;
  for (Int k=0; k < numAnt; k++){
    outLog << "  " << k+1 << "   " << coeff.row(k) << endl;
    
  }


  outLog << " RMS Fit per baseline " << endl;
  outLog << "Antenna1      Antenna2      rmsfit" << endl;
  for (Int ant1=0; ant1 < numAnt; ++ant1){
    for (Int ant2=ant1+1; ant2 < numAnt; ++ant2){
      Int basnum=ant1*numAnt-ant1*(ant1+1)/2+ant2-1-ant1;
      outLog << "   " << ant1+1 << "           " << ant2+1 << "            " 
	     << rmsFit[basnum] << endl;  
    }
  }
}

} //# NAMESPACE CASA - END

