//# StandardVisCal.cc: Implementation of Standard VisCal types
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003,2011
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

#include <synthesis/MeasurementComponents/StandardVisCal.h>
#include <synthesis/MeasurementComponents/CalCorruptor.h>

#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/VisBuffAccumulator.h>
#include <synthesis/CalTables/CTIter.h>
#include <ms/MeasurementSets/MSColumns.h>
#include <synthesis/MeasurementEquations/VisEquation.h>
#include <synthesis/MeasurementComponents/SolveDataBuffer.h>
#include <scimath/Fitting/LSQFit.h>
#include <scimath/Fitting/LinearFit.h>
#include <scimath/Functionals/CompiledFunction.h>
#include <scimath/Functionals/Polynomial.h>
#include <scimath/Mathematics/AutoDiff.h>
#include <casa/BasicMath/Math.h>
#include <tables/TaQL/ExprNode.h>

#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/MatrixMath.h>
#include <casa/BasicSL/String.h>
#include <casa/Utilities/Assert.h>
#include <casa/Utilities/GenSort.h>
#include <casa/Exceptions/Error.h>
#include <casa/OS/Memory.h>
#include <casa/System/Aipsrc.h>

#include <casa/sstream.h>

#include <measures/Measures/MCBaseline.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MEpoch.h>
#include <measures/Measures/MeasTable.h>

#include <casa/Logging/LogMessage.h>
#include <casa/Logging/LogSink.h>
// math.h ?

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN


// **********************************************************
//  PJones
//

PJones::PJones(VisSet& vs) :
  VisCal(vs), 
  VisMueller(vs),
  VisJones(vs),
  pjonestype_(Jones::Diagonal),
  pa_()
{
  if (prtlev()>2) cout << "P::P(vs)" << endl;
}

PJones::PJones(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw), 
  VisMueller(msname,MSnAnt,MSnSpw),
  VisJones(msname,MSnAnt,MSnSpw),
  pjonestype_(Jones::Diagonal),
  pa_()
{
  if (prtlev()>2) cout << "P::P(msname,MSnAnt,MSnSpw)" << endl;
}

PJones::PJones(const MSMetaInfoForCal& msmc) : 
  VisCal(msmc),
  VisMueller(msmc),
  VisJones(msmc),
  pjonestype_(Jones::Diagonal),
  pa_()
{
  if (prtlev()>2) cout << "P::P(msmc)" << endl;
}


PJones::~PJones() {
  if (prtlev()>2) cout << "P::~P()" << endl;
}

// PJones needs to know pol basis and feed pa
void PJones::syncMeta(const VisBuffer& vb) {

  // Call parent (sets currTime())
  VisJones::syncMeta(vb);

  // Basis
  if (vb.corrType()(0)==5)         // Circulars
    pjonestype_=Jones::Diagonal;
  else if (vb.corrType()(0)==9)    // Linears
    pjonestype_=Jones::General;

  // Get parallactic angle from the vb:
  pa().resize(nAnt());
  pa() = vb.feed_pa(currTime());

}

// PJones needs to know pol basis and feed pa
void PJones::syncMeta2(const vi::VisBuffer2& vb) {

  // Call parent (sets currTime())
  VisJones::syncMeta2(vb);

  // Basis
  if (vb.correlationTypes()(0)==5)         // Circulars
    pjonestype_=Jones::Diagonal;
  else if (vb.correlationTypes()(0)==9)    // Linears
    pjonestype_=Jones::General;

  // Get parallactic angle from the vb:
  pa().resize(nAnt());
  pa() = vb.feedPa(currTime());

}

void PJones::calcPar() {

  if (prtlev()>6) cout << "      VC::calcPar()" << endl;

  // Initialize parameter arrays
  currCPar().resize(1,1,nAnt());
  currParOK().resize(1,1,nAnt());
  currParOK()=true;

  // Fill currCPar() with exp(i*pa)
  Float* a=pa().data();
  Complex* cp=currCPar().data();
  Double ang(0.0);
  for (Int iant=0;iant<nAnt();++iant,++a,++cp) {
    ang=Double(*a);
    (*cp) = Complex(cos(ang),sin(ang));  // as a complex number
  }
  //  cout << "ang = " << ang << endl;

  // Pars now valid, matrices not
  validateP();
  invalidateJ();

}

// Calculate a single Jones matrix by some means from parameters
void PJones::calcOneJones(Vector<Complex>& mat, Vector<Bool>& mOk,
			  const Vector<Complex>& par, const Vector<Bool>& pOk ) {

  if (prtlev()>10) cout << "       P::calcOneJones()" << endl;


  if (pOk(0)) {

    switch (jonesType()) {
      // Circular version:
    case Jones::Diagonal: {
      mat(0)=conj(par(0));  // exp(-ia)
      mat(1)=par(0);        // exp(ia)
      mOk=true;
      break;
    }
      // Linear version:
    case Jones::General: {
      Float a=arg(par(0));
      mat(0)=mat(3)=cos(a);
      mat(1)=sin(a);
      mat(2)=-mat(1);
      mOk=true;
      break;
    }
    default:
      throw(AipsError("PJones doesn't know if it is Diag (Circ) or General (Lin)"));
      break;

    }

  }
}



// **********************************************************
//  TJones Implementations
//

TJones::TJones(VisSet& vs) :
  VisCal(vs),             // virtual base
  VisMueller(vs),         // virtual base
  SolvableVisJones(vs),    // immediate parent
  tcorruptor_p(NULL)
{
  if (prtlev()>2) cout << "T::T(vs)" << endl;
}

TJones::TJones(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),             // virtual base
  VisMueller(msname,MSnAnt,MSnSpw),         // virtual base
  SolvableVisJones(msname,MSnAnt,MSnSpw),   // immediate parent
  tcorruptor_p(NULL)
{
  if (prtlev()>2) cout << "T::T(msname,MSnAnt,MSnSpw)" << endl;
}

TJones::TJones(const MSMetaInfoForCal& msmc) : 
  VisCal(msmc),
  VisMueller(msmc),
  SolvableVisJones(msmc),
  tcorruptor_p(NULL)
{
  if (prtlev()>2) cout << "T::T(msmc)" << endl;
}


TJones::TJones(const Int& nAnt) :
  VisCal(nAnt), 
  VisMueller(nAnt),
  SolvableVisJones(nAnt),
  tcorruptor_p(NULL)
{
  if (prtlev()>2) cout << "T::T(nAnt)" << endl;
}

TJones::~TJones() {
  if (prtlev()>2) cout << "T::~T()" << endl;
}

void TJones::guessPar(VisBuffer& vb) {

  if (prtlev()>4) cout << "   T::guessPar(vb)" << endl;

  // Assumes:  1. corrs in canonical order
  //           2. vb has 1 channel (has been freq-averaged)


  // Make an antenna-based guess at T
  //  Correlation membership-dependence
  //  nCorr = 1: use icorr=0
  //  nCorr = 2: use icorr=[0,1]
  //  nCorr = 4: use icorr=[0,3]

  Int nCorr(1);
  Int nDataCorr(vb.visCube().shape()(0));
  Vector<Int> corridx(1,0);
  if (nDataCorr==2) {
    nCorr=2;
    corridx.resize(nCorr);
    corridx(0)=0;
    corridx(1)=1;
  } 
  else if (nDataCorr==4) {
    nCorr=2;
    corridx.resize(nCorr);
    corridx(0)=0;
    corridx(1)=3;
  }

  // Find out which ants are available
  // TBD: count nominal guessant rows, insist not much less than nAnt
  Vector<Bool> antok(nAnt(),false);
  Vector<Bool> rowok(vb.nRow(),false);
  for (Int irow=0;irow<vb.nRow();++irow) {
    // Is this row ok?
    rowok(irow)= (!vb.flagRow()(irow) &&
		  (vb.antenna1()(irow)!=vb.antenna2()(irow)) &&
		  nfalse(vb.flag().column(irow))> 0 );
    if (rowok(irow)) {
      antok(vb.antenna1()(irow))=true;
      antok(vb.antenna2()(irow))=true;
    }
  }

  // Assume refant is the target ant, for starters
  Int guessant(refant());

  // If no refant specified, or no data for refant
  //   base first guess on first good ant
  if (guessant<0 || !antok(guessant)) {
    guessant=0;
    while (antok(guessant)<1) guessant++;
  }

  AlwaysAssert(guessant>-1,AipsError);

  Cube<Complex>& V(vb.visCube());
  Float amp(0.0),ampave(0.0);
  Int namp(0);
  solveCPar()=Complex(0.0);
  for (Int irow=1;irow<vb.nRow();++irow) {  // why not row zero? copied from Calibrator

    if (rowok(irow)) {
      Int a1=vb.antenna1()(irow);
      Int a2=vb.antenna2()(irow);

      // If this row contains the guessant
      if (a1 == guessant || a2==guessant) {
      
	for (Int icorr=0;icorr<nCorr;icorr++) {
	  Complex& Vi(V(corridx(icorr),0,irow));
	  amp=abs(Vi);
	  if (amp>0.0f) {
	    if (a1 == guessant)
	      solveCPar()(0,0,a2)+=(conj(Vi)/amp/Float(nCorr));
	    //	      solveCPar()(0,0,a2)+=(conj(Vi)/Float(nCorr));
	    else
	      solveCPar()(0,0,a1)+=((Vi)/amp/Float(nCorr));
	    //	      solveCPar()(0,0,a1)+=((Vi)/Float(nCorr));
	    
	    ampave+=amp;
	    namp++;
	    //	cout << "          " << abs(Vi) << " " << arg(Vi)*180.0/C::pi << endl;
	  }
	}
      } // guessant in row
    } // rowok
  } // irow

  //  cout << "Guess:" << endl
  //   << "amp = " << amplitude(solveCPar())
  //     << endl;
 

  // Scale them by the mean amplitude

  if (namp>0) {
    ampave/=Float(namp);
    ampave=sqrt(ampave);
    //  solveCPar()*=Complex(ampave);
    solveCPar()/=Complex(ampave);
    solveCPar()(0,0,guessant)=Complex(ampave);
    solveCPar()(LogicalArray(amplitude(solveCPar())==0.0f)) = Complex(ampave);
  }
  else
    solveCPar()=Complex(0.3);

  solveParOK()=true;

  /*
  cout << "Guess:" << endl
       << "amp = " << amplitude(solveCPar())
       << "phase = " << phase(solveCPar())
       << endl;
  */
}


void TJones::guessPar(SDBList& sdbs,const Bool&) {

  if (prtlev()>4) cout << "   T::guessPar(sdbs)" << endl;

  // Assumes:  1. corrs in canonical order
  
  // Use just the first SDB in the SDBList, for now
  SolveDataBuffer& sdb(sdbs(0));
  
  AlwaysAssert(sdb.nChannels()==1,AipsError);
    
  // Make an antenna-based guess at T
  //  Correlation membership-dependence
  //  nCorr = 1: use icorr=0
  //  nCorr = 2: use icorr=[0,1]
  //  nCorr = 4: use icorr=[0,3]
  
  Int nCorr(1);
  Int nDataCorr(sdb.nCorrelations());
  Vector<Int> corridx(1,0);
  if (nDataCorr==2) {
    nCorr=2;
    corridx.resize(nCorr);
    corridx(0)=0;
    corridx(1)=1;
  } 
  else if (nDataCorr==4) {
    nCorr=2;
    corridx.resize(nCorr);
    corridx(0)=0;
    corridx(1)=3;
  }
  
  // Find out which ants are available
  // TBD: count nominal guessant rows, insist not much less than nAnt
  Int nRow=sdb.nRows();
  Vector<Bool> antok(nAnt(),False);
  Vector<Bool> rowok(nRow,False);
  for (Int irow=0;irow<nRow;++irow) {
    const Int& a1(sdb.antenna1()(irow));
    const Int& a2(sdb.antenna2()(irow));
    // Is this row ok?
    rowok(irow)= (!sdb.flagRow()(irow) &&
		  (a1!=a2) &&
		  nfalse(sdb.flagCube().xyPlane(irow))> 0 );
    if (rowok(irow)) {
      antok(a1)=True;
      antok(a2)=True;
    }
  }
  
  // Assume refant is the target ant, for starters
  Int guessant(refant());
  
  // If no refant specified, or no data for refant
  //   base first guess on first good ant
  if (guessant<0 || !antok(guessant)) {
    guessant=0;
    while (antok(guessant)<1) guessant++;
  }
  
  AlwaysAssert(guessant>-1,AipsError);
  
  const Cube<Complex>& V(sdb.visCubeCorrected());
  Float amp(0.0),ampave(0.0);
  Int namp(0);
  solveCPar()=Complex(0.0);
  for (Int irow=1;irow<nRow;++irow) {  // why not row zero? copied from Calibrator
    
    if (rowok(irow)) {
      const Int& a1(sdb.antenna1()(irow));
      const Int& a2(sdb.antenna2()(irow));
      
      // If this row contains the guessant
      if (a1 == guessant || a2==guessant) {
	
	for (Int icorr=0;icorr<nCorr;icorr++) {
	  const Complex& Vi(V(corridx(icorr),0,irow));
	  amp=abs(Vi);
	  if (amp>0.0f) {
	    if (a1 == guessant)
	      solveCPar()(0,0,a2)+=(conj(Vi)/amp/Float(nCorr));
	    //        solveCPar()(0,0,a2)+=(conj(Vi)/Float(nCorr));
	    else
	      solveCPar()(0,0,a1)+=((Vi)/amp/Float(nCorr));
	    //        solveCPar()(0,0,a1)+=((Vi)/Float(nCorr));
            
	    ampave+=amp;
	    namp++;
	    //  cout << "          " << abs(Vi) << " " << arg(Vi)*180.0/C::pi << endl;
	  }
	}
      } // guessant in row
    } // rowok
  } // irow
  
  //  cout << "Guess:" << endl
  //   << "amp = " << amplitude(solveCPar())
  //     << endl;
  

  // Scale them by the mean amplitude

  if (namp>0) {
    ampave/=Float(namp);
    ampave=sqrt(ampave);
    //  solveCPar()*=Complex(ampave);
    solveCPar()/=Complex(ampave);
    solveCPar()(0,0,guessant)=Complex(ampave);
    solveCPar()(LogicalArray(amplitude(solveCPar())==0.0f)) = Complex(ampave);
  }
  else
    solveCPar()=Complex(0.3);
  
  solveParOK()=True;
  
  /*
    cout << "Guess:" << endl
    << "amp = " << amplitude(solveCPar())
    << "phase = " << phase(solveCPar())
    << endl;
  */
}

// Fill the trivial DJ matrix elements
void TJones::initTrivDJ() {

  if (prtlev()>4) cout << "   T::initTrivDJ" << endl;

  // Must be trivial
  AlwaysAssert((trivialDJ()),AipsError);

  // This is the unit matrix
  //  TBD: could we use a Jones::Unit type instead?
  diffJElem().resize(IPosition(4,1,1,1,1));
  diffJElem()=Complex(1.0);

}

  





//============================================================


void TJones::createCorruptor(const VisIter& vi, const Record& simpar, const Int nSim) {
  LogIO os(LogOrigin("T", "createCorruptor()", WHERE));

  tcorruptor_p = new AtmosCorruptor(nSim);
  corruptor_p=tcorruptor_p;

  // call generic parent to set corr,spw,etc info
  SolvableVisCal::createCorruptor(vi,simpar,nSim);
  
  Int Seed(1234);
  if (simpar.isDefined("seed")) {    
    Seed=simpar.asInt("seed");
  }
  
  Int rxType(0); // 0=2SB, 1=DSB
  if (simpar.isDefined("rxType")) {    
    rxType=simpar.asInt("rxType");
  }
  
  Float Beta(1.1); // exponent for generalized 1/f noise
  if (simpar.isDefined("beta")) {    
    Beta=simpar.asFloat("beta");
  }
  
  if (simpar.isDefined("mean_pwv"))
    tcorruptor_p->mean_pwv() = simpar.asFloat("mean_pwv");
  
  if (tcorruptor_p->mean_pwv()<=0)
    throw(AipsError("AtmCorruptor attempted initialization with undefined PWV"));
  
  if (simpar.isDefined("mode")) {    
    if (prtlev()>2) 
      cout << "initializing T:Corruptor with mode " << simpar.asString("mode") << endl;
       String simMode=simpar.asString("mode");
    
    if (simMode == "test")
      tcorruptor_p->initialize(rxType);
    else if (simMode == "individual" or simMode == "screen") {

      Float Scale(1.); // RELATIVE scale of fluctuations (to mean_pwv)
      if (simpar.isDefined("delta_pwv")) {
	if (simpar.asFloat("delta_pwv")>1.) {
	  Scale=1.;
	  os << LogIO::WARN << " decreasing PWV fluctuation magnitude to 100% of the mean PWV " << LogIO::POST;  
	} else {
	  Scale=simpar.asFloat("delta_pwv");
	}
      } else {
	os << LogIO::WARN << " setting PWV fluctuation magnitude to 15% of the mean PWV " << LogIO::POST;  
	Scale=0.15;
      }
      
      os << " PWV fluctuations = " << Scale << " of mean PWV which is " << simpar.asFloat("mean_pwv") << "mm " << LogIO::POST;  
      
      
      // slot_times for a fBM-based corruption need to be even even if solTimes are not
      // so will define startTime and stopTime and reset nsim() here.
      
      if (simpar.isDefined("startTime")) {    
	corruptor_p->startTime() = simpar.asDouble("startTime");	
      } else {
	throw(AipsError("start/stop time not defined"));
      }
      if (simpar.isDefined("stopTime")) {    
	corruptor_p->stopTime() = simpar.asDouble("stopTime");
      } else {
	throw(AipsError("start/stop time not defined"));
      }

      // RI todo T::createCorr make min screen granularity a user parameter
      Float fBM_interval=max(interval(),10.); // generate screens on >10s intervals
      if (prtlev()>2) cout<<"set fBM_interval"<<" to "<<fBM_interval<<" startTime="<<corruptor_p->startTime()<<" stopTime="<<corruptor_p->stopTime()<<endl;

      corruptor_p->setEvenSlots(fBM_interval);

      if (simpar.asString("mode") == "individual") 
	tcorruptor_p->initialize(Seed,Beta,Scale,rxType);
      else if (simpar.asString("mode") == "screen") {
	const MSAntennaColumns& antcols(vi.msColumns().antenna());
	if (simpar.isDefined("windspeed")) {
	  tcorruptor_p->windspeed()=simpar.asFloat("windspeed");
	  tcorruptor_p->initialize(Seed,Beta,Scale,rxType,antcols);
	} else
	  throw(AipsError("Unknown wind speed for T:Corruptor"));        
      }

    } else if (simMode == "tsys-atm" or simMode == "tsys-manual") {
      // NEW 20100818 change from Mf to Tf
      // M corruptor initialization didn't matter M or Mf here - it checks mode in 
      // the Atmoscorruptor init.
      tcorruptor_p->initialize(vi,simpar,VisCal::T,rxType); 
      extraTag()="NoiseScale"; // collapseForSim catches this
    
    } else 
	throw(AipsError("Unknown mode for T:Corruptor"));        
  } else {
    throw(AipsError("No Mode specified for T:Corruptor."));
  }  
}





// **********************************************************
//  TfJones Implementations
//

TfJones::TfJones(VisSet& vs) :
  VisCal(vs),             // virtual base
  VisMueller(vs),         // virtual base
  TJones(vs)              // immediate parent
{
  if (prtlev()>2) cout << "Tf::Tf(vs)" << endl;
}

TfJones::TfJones(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),             // virtual base
  VisMueller(msname,MSnAnt,MSnSpw),         // virtual base
  TJones(msname,MSnAnt,MSnSpw)              // immediate parent
{
  if (prtlev()>2) cout << "Tf::Tf(msname,MSnAnt,MSnSpw)" << endl;
}

TfJones::TfJones(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),             // virtual base
  VisMueller(msmc),         // virtual base
  TJones(msmc)              // immediate parent
{
  if (prtlev()>2) cout << "Tf::Tf(msmc)" << endl;
}


TfJones::TfJones(const Int& nAnt) :
  VisCal(nAnt), 
  VisMueller(nAnt),
  TJones(nAnt)
{
  if (prtlev()>2) cout << "Tf::Tf(nAnt)" << endl;
}

TfJones::~TfJones() {
  if (prtlev()>2) cout << "Tf::~Tf()" << endl;
}


// **********************************************************
//  GJones Implementations
//

GJones::GJones(VisSet& vs) :
  VisCal(vs),             // virtual base
  VisMueller(vs),         // virtual base
  SolvableVisJones(vs),    // immediate parent
  gcorruptor_p(NULL)
{
  if (prtlev()>2) cout << "G::G(vs)" << endl;
}

GJones::GJones(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),             // virtual base
  VisMueller(msname,MSnAnt,MSnSpw),         // virtual base
  SolvableVisJones(msname,MSnAnt,MSnSpw),    // immediate parent
  gcorruptor_p(NULL)
{
  if (prtlev()>2) cout << "G::G(msname,MSnAnt,MSnSpw)" << endl;
}

GJones::GJones(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),             // virtual base
  VisMueller(msmc),         // virtual base
  SolvableVisJones(msmc),    // immediate parent
  gcorruptor_p(NULL)
{
  if (prtlev()>2) cout << "G::G(msmc)" << endl;
}

GJones::GJones(const Int& nAnt) :
  VisCal(nAnt), 
  VisMueller(nAnt),
  SolvableVisJones(nAnt),
  gcorruptor_p(NULL)
{
  if (prtlev()>2) cout << "G::G(nAnt)" << endl;
}

GJones::~GJones() {
  if (prtlev()>2) cout << "G::~G()" << endl;
}

void GJones::setSolve(const Record& solve) {

  // call parent to get general stuff
  SolvableVisJones::setSolve(solve);

  // parse solmode and rmsthresh
  solmode()=String("");
  if (solve.isDefined("solmode"))
    solmode()=solve.asString("solmode");
  solmode().upcase();
  rmsthresh().resize(0);
  if (solve.isDefined("rmsthresh")) {
    Vector<Double> rmsth;
    rmsth=solve.asArrayDouble("rmsthresh");  // parse from record as Vector<Double>
    Int nrms=rmsth.nelements();
    rmsthresh().resize(0);
    if (nrms>0) {
      rmsthresh().resize(nrms);
      convertArray(rmsthresh(), rmsth); // convert into Float array
    }
  }
  Int nrms=rmsthresh().nelements();
  if (nrms==0 && solmode().contains("R")) {
    rmsthresh().assign(Vector<Float>(std::vector<Float>({7.0,5.0,4.0,3.5,3.0,2.8,2.6,2.4,2.2,2.5})));
  }

  if (typeName()=="G Jones" || typeName()=="T Jones") {
    if (solmode().contains("L1"))
      logSink() << "Doing L1 solution." << LogIO::POST;
    if (solmode().contains("R"))
      logSink() << "Doing iterative outlier rejection using thresholds=" << rmsthresh() << LogIO::POST;
  }


}


void GJones::guessPar(VisBuffer& vb) {

  if (prtlev()>4) cout << "   G::guessPar(vb)" << endl;

  // Make a guess at antenna-based G
  //  Correlation membership-dependencexm
  //  assumes corrs in canonical order
  //  nCorr = 1: use icorr=0
  //  nCorr = 2: use icorr=[0,1]
  //  nCorr = 4: use icorr=[0,3]

  Int nCorr(2);
  Int nDataCorr(vb.visCube().shape()(0));
  Vector<Int> corridx(nCorr,0);
  if (nDataCorr==2) {
    corridx(0)=0;
    corridx(1)=1;
  } 
  else if (nDataCorr==4) {
    corridx(0)=0;
    corridx(1)=3;
  }

  Int guesschan(vb.visCube().shape()(1)-1);

  //  cout << "guesschan = " << guesschan << endl;
  //  cout << "nCorr = " << nCorr << endl;
  //  cout << "corridx = " << corridx << endl;


  // Find out which ants are available
  Vector<Int> antok(nAnt(),0);
  Vector<Bool> rowok(vb.nRow(),false);
  for (Int irow=0;irow<vb.nRow();++irow) {
    // Is this row ok
    rowok(irow)= (!vb.flagRow()(irow) &&
		  vb.antenna1()(irow)!=vb.antenna2()(irow) &&
		  nfalse(vb.flag().column(irow))> 0 );
    if (rowok(irow)) {
      antok(vb.antenna1()(irow))++;
      antok(vb.antenna2()(irow))++;
    }
  }

  // Assume refant is the target ant, for starters
  Int guessant(refant());
  //  Int guessant(-1);

  // If no refant specified, or no data for refant
  //   base first guess on first good ant
  if (guessant<0 || antok(guessant)<1) {
    guessant=0;
    while (antok(guessant)<1) guessant++;
  }

  //  cout << "antok = " << antok << endl;

  //  cout << "guessant = " << guessant << "  (" << currSpw() << ")" << endl;

  AlwaysAssert(guessant>-1,AipsError);

  Cube<Complex>& V(vb.visCube());
  Float amp(0.0),ampave(0.0);
  Int namp(0);
  solveCPar()=Complex(0.0);
  for (Int irow=1;irow<vb.nRow();++irow) {

    if (rowok(irow) && !vb.flag()(guesschan,irow)) {
      Int a1=vb.antenna1()(irow);
      Int a2=vb.antenna2()(irow);

      // If this row contains the guessant
      if (a1 == guessant || a2==guessant) {

	//	cout << a1 << " " << a2 << " " 
	//	     << vb.visCube().xyPlane(irow).column(guesschan) << " "
	//	     << amplitude(vb.visCube().xyPlane(irow).column(guesschan)) << " "
	//	     << endl;

	for (Int icorr=0;icorr<nCorr;icorr++) {
	  Complex& Vi(V(corridx(icorr),guesschan,irow));
	  amp=abs(Vi);
	  if (amp>0.0f) {
	    if (a1 == guessant)
	      solveCPar()(icorr,0,a2)=conj(Vi);
	    else
	      solveCPar()(icorr,0,a1)=(Vi);
	      
	    ampave+=amp;
	    namp++;
	  }
	}
      } // guessant
    } // rowok
  } // irow

  // Scale them by the mean amplitude

  if (namp>0) {
    ampave/=Float(namp);
    ampave=sqrt(ampave);
    //  solveCPar()*=Complex(ampave);
    solveCPar()/=Complex(ampave);
    solveCPar()(0,0,guessant)=solveCPar()(1,0,guessant)=Complex(ampave);
    solveCPar()(LogicalArray(amplitude(solveCPar())==0.0f)) = Complex(ampave);
  }
  else
    solveCPar()=Complex(0.3);

  solveParOK()=true;

  //For scalar data, Set "other" pol soln to zero
  if (nDataCorr == 1)
    solveCPar()(IPosition(3,1,0,0),IPosition(3,1,0,nAnt()-1))=Complex(0.0);

  /*
  cout << "Guess:" << endl;
  cout << "amplitude(solveCPar())   = " << amplitude(solveCPar()) << endl;
  cout << "phases       = " << phase(solveCPar())*180.0/C::pi << endl;
  cout << "solveParOK() = " << solveParOK() << endl;
  */
}

void GJones::guessPar(SDBList& sdbs, const Bool& corrDepFlags) {

  if (prtlev()>4) cout << "   G::guessPar(sdbs)" << endl;

  // Use just the first SDB in the SDBList, for now
  SolveDataBuffer& sdb(sdbs(0));

  // Make a guess at antenna-based G
  //  Correlation membership-dependencexm
  //  assumes corrs in canonical order
  //  nCorr = 1: use icorr=0
  //  nCorr = 2: use icorr=[0,1]
  //  nCorr = 4: use icorr=[0,3]

  Int nCorr(2);
  Int nDataCorr(sdb.nCorrelations());
  Vector<Int> corridx(nCorr,0);
  if (nDataCorr==2) {
    corridx(0)=0;
    corridx(1)=1;
  } 
  else if (nDataCorr==4) {
    corridx(0)=0;
    corridx(1)=3;
  }

  Int guesschan(sdb.nChannels()-1);

  /*
  cout << "guesschan = " << guesschan << endl;
  cout << "nCorr = " << nCorr << endl;
  cout << "corridx = " << corridx << endl;
  */

  // Find out which ants are available
  Int nRow=sdb.nRows();
  Vector<Int> antok(nAnt(),0);
  Vector<Bool> rowok(nRow,False);
  for (Int irow=0;irow<nRow;++irow) {

    const Int& a1(sdb.antenna1()(irow));
    const Int& a2(sdb.antenna2()(irow));

    // Is this row ok?
    rowok(irow)= (!sdb.flagRow()(irow) &&
		  a1!=a2);

    if (!corrDepFlags) {
      // All relevant correlations must be good
      for (Int icorr=0;icorr<nCorr;++icorr)
	rowok(irow)=(rowok(irow) && !sdb.flagCube()(corridx[icorr],guesschan,irow));
    }

    if (rowok(irow)) {
      antok(a1)++;
      antok(a2)++;
    }
  }

  // Assume refant is the target ant, for starters
  Int guessant(refant());
  //  Int guessant(-1);

  // If no refant specified, or no data for refant
  //   base first guess on first good ant
  if (guessant<0 || antok(guessant)<1) {
    guessant=0;
    while (antok(guessant)<1) guessant++;
  }

  //  cout << "antok = " << antok << endl;

  //  cout << "guessant = " << guessant << "  (" << currSpw() << ")" << endl;

  AlwaysAssert(guessant>-1,AipsError);

  const Cube<Complex>& V(sdb.visCubeCorrected());
  Float amp(0.0),ampave(0.0);
  Int namp(0);
  solveCPar()=Complex(0.0);
  for (Int irow=0;irow<nRow;++irow) {

    if (rowok(irow)) {
      const Int& a1=sdb.antenna1()(irow);
      const Int& a2=sdb.antenna2()(irow);

      // If this row contains the guessant
      if (a1 == guessant || a2==guessant) {

	//      cout << a1 << " " << a2 << " " 
	//           << sdb.visCube().xyPlane(irow).column(guesschan) << " "
	//           << amplitude(sdb.visCube().xyPlane(irow).column(guesschan)) << " "
	//           << endl;

        for (Int icorr=0;icorr<nCorr;icorr++) {
          const Complex& Vi(V(corridx(icorr),guesschan,irow));
          amp=abs(Vi);
          if (amp>0.0f) {
            if (a1 == guessant)
              solveCPar()(icorr,0,a2)=conj(Vi);
            else
              solveCPar()(icorr,0,a1)=(Vi);
              
            ampave+=amp;
            namp++;
          }
        }
      } // guessant
    } // rowok
  } // irow

  // Scale them by the mean amplitude

  if (namp>0) {
    ampave/=Float(namp);
    ampave=sqrt(ampave);
    //  solveCPar()*=Complex(ampave);
    solveCPar()/=Complex(ampave);
    solveCPar()(0,0,guessant)=solveCPar()(1,0,guessant)=Complex(ampave);
    solveCPar()(LogicalArray(amplitude(solveCPar())==0.0f)) = Complex(ampave);
  }
  else
    solveCPar()=Complex(0.3);

  solveParOK()=True;

  //For scalar data, Set "other" pol soln to zero
  if (nDataCorr == 1)
    solveCPar()(IPosition(3,1,0,0),IPosition(3,1,0,nAnt()-1))=Complex(0.0);

  /*
    cout << "Guess:" << endl;
    cout << "amplitude(solveCPar())   = " << amplitude(solveCPar()) << endl;
    cout << "phases       = " << phase(solveCPar())*180.0/C::pi << endl;
    cout << "solveParOK() = " << solveParOK() << endl;
  */
}

// Fill the trivial DJ matrix elements
void GJones::initTrivDJ() {

  if (prtlev()>4) cout << "   G::initTrivDJ" << endl;

  // Must be trivial
  AlwaysAssert((trivialDJ()),AipsError);

  //  1 0     0 0
  //  0 0  &  0 1
  // 
  if (diffJElem().nelements()==0) {
    diffJElem().resize(IPosition(4,2,2,1,1));
    diffJElem()=0.0;
    diffJElem()(IPosition(4,0,0,0,0))=Complex(1.0);
    diffJElem()(IPosition(4,1,1,0,0))=Complex(1.0);
  }

}


void GJones::createCorruptor(const VisIter& vi, const Record& simpar, const Int nSim) {
{

  LogIO os(LogOrigin("G", "createCorruptor()", WHERE));  
  if (prtlev()>2) cout << "   G::createCorruptor()" << endl;

  gcorruptor_p = new GJonesCorruptor(nSim);
  corruptor_p = gcorruptor_p;

  // call generic parent to set corr,spw,etc info
  SolvableVisCal::createCorruptor(vi,simpar,nSim);
  
  Int Seed(1234);
  if (simpar.isDefined("seed")) {    
    Seed=simpar.asInt("seed");
  }

  if (simpar.isDefined("tsys")) {
    gcorruptor_p->tsys() = simpar.asFloat("tsys");
  } 
  
  if (simpar.isDefined("mode")) {    
    if (prtlev()>2)
      cout << "initializing GCorruptor with mode " << simpar.asString("mode") << endl;
    
    // slot_times for a fBM-based corruption need to be even even if solTimes are not
    // so will define startTime and stopTime and reset nsim() here.
    
    if (simpar.isDefined("startTime")) {    
      corruptor_p->startTime() = simpar.asDouble("startTime");
    } else {
      throw(AipsError("start/stop time not defined"));
    }
    if (simpar.isDefined("stopTime")) {    
      corruptor_p->stopTime() = simpar.asDouble("stopTime");
    } else {
      throw(AipsError("start/stop time not defined"));
    }
        
    if (simpar.asString("mode")=="fbm") {

      Float Beta(1.1); // exponent for generalized 1/f noise
      if (simpar.isDefined("beta")) {    
	Beta=simpar.asFloat("beta");
      }
      
      Float Scale(.15); // scale of fluctuations 
      if (simpar.isDefined("amplitude")) {
	Scale=simpar.asFloat("amplitude");
	if (Scale>=.9) {
	  os << LogIO::WARN << " decreasing gain fluctuations from " << Scale << " to 0.9 " << LogIO::POST;  
	  Scale=.9;
	}
      }

      Float fBM_interval=max(interval(),5.); // generate screens on 5s intervals or longer
      corruptor_p->setEvenSlots(fBM_interval);
      gcorruptor_p->initialize(Seed,Beta,Scale);
    
    } else if (simpar.asString("mode")=="random") {

      Complex Scale(0.1,0.1); // scale of fluctuations 
      if (simpar.isDefined("camp")) {
	Scale=simpar.asComplex("camp");
      }
      gcorruptor_p->initialize(Seed,Scale);

    } else throw AipsError("incompatible mode "+simpar.asString("mode"));
      
    
  } else 
    throw(AipsError("Unknown mode for GJonesCorruptor"));        
 }
}







// **********************************************************
//  BJones Implementations
//

BJones::BJones(VisSet& vs) :
  VisCal(vs),             // virtual base
  VisMueller(vs),         // virtual base
  GJones(vs),             // immediate parent
  maxchangap_p(0)
{
  if (prtlev()>2) cout << "B::B(vs)" << endl;
}

BJones::BJones(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),             // virtual base
  VisMueller(msname,MSnAnt,MSnSpw),         // virtual base
  GJones(msname,MSnAnt,MSnSpw),             // immediate parent
  maxchangap_p(0)
{
  if (prtlev()>2) cout << "B::B(msname,MSnAnt,MSnSpw)" << endl;
}

BJones::BJones(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),             // virtual base
  VisMueller(msmc),         // virtual base
  GJones(msmc),             // immediate parent
  maxchangap_p(0)
{
  if (prtlev()>2) cout << "B::B(msmc)" << endl;
}

BJones::BJones(const Int& nAnt) :
  VisCal(nAnt), 
  VisMueller(nAnt),
  GJones(nAnt),
  maxchangap_p(0)
{
  if (prtlev()>2) cout << "B::B(nAnt)" << endl;
}

BJones::~BJones() {
  if (prtlev()>2) cout << "B::~B()" << endl;
}

void BJones::setSolve(const Record& solve) {

  // call parent to get general stuff
  GJones::setSolve(solve);

  if (solmode()!="") {
    solmode()="";
    cout << "solmode options not yet supported for B solutions; ignoring." << endl;
  }

  // get max chan gap from user
  maxchangap_p=0;
  if (solve.isDefined("maxgap"))
    maxchangap_p=solve.asInt("maxgap");

}

void BJones::normalize() {

  // Only if we have a CalTable, and it is not empty
  if (ct_ && ct_->nrow()>0) {

    // TBD: trap attempts to normalize a caltable containing FPARAM (non-Complex)?

    logSink() << "Normalizing solutions per spw, time, ant, pol with " << solNorm().normtypeString()
	      << " in amplitude (and center channel in phase)."
	      << LogIO::POST;

    // Bandpass is normalized per spw, time, antenna, and pol
    Block<String> col(3);
    col[0]="SPECTRAL_WINDOW_ID";
    col[1]="TIME";
    col[2]="ANTENNA1";
    CTIter ctiter(*ct_,col);

    // Cube iteration axes are pol and ant (ant is degenerate)
    IPosition itax(2,0,2);
   
    while (!ctiter.pastEnd()) {
      Cube<Bool> fl(ctiter.flag());
      
      if (nfalse(fl)>0) {
	Cube<Complex> p(ctiter.cparam());
	ArrayIterator<Complex> soliter(p,itax,false);
	ArrayIterator<Bool> fliter(fl,itax,false);
	Int ipol(0);
	while (!soliter.pastEnd()) {
	  Complex normfactor=normSolnArray(soliter.array(),!fliter.array(),true); // Do phase
	  logSink() << " Normalization factor (" << solNorm().normtypeString() << ") for"
		    << " spw=" << ctiter.thisSpw() 
		    << " time=" << MVTime(ctiter.thisTime()/C::day).string(MVTime::YMD,7)
		    << " ant=" << ctiter.thisAntenna1() 
		    << " pol " << ipol%2
		    << " = " << abs(normfactor) << ", " << arg(normfactor)*180.0/C::pi << "deg"
		    << LogIO::POST;
	  soliter.next();
	  fliter.next();
	  ++ipol;
	}
	
	// record result...	
	ctiter.setcparam(p);
      }
      ctiter.next();
    }
  }

}

void BJones::globalPostSolveTinker() {

  // Call parent to do more general things
  SolvableVisJones::globalPostSolveTinker();

  // Fill gaps in channels, if necessary
  if (maxchangap_p>0)
    fillChanGaps();

}

void BJones::fillChanGaps() {

  // TBD: Can this be consolidated with normalization (should be done before norm!)

  logSink() << "Filling in flagged solution channels by interpolation." 
	    << LogIO::POST;

  // Iteration axes (norm per spw, pol, ant, timestamp)
  IPosition itax(3,0,2,3);


  // Only if we have a CalTable, and it is not empty
  if (ct_ && ct_->nrow()>0) {

    // TBD: trap attempts to normalize a caltable containing FPARAM (non-Complex)?

    logSink() << "Normalizing solutions per spw, pol, ant, time." 
	      << LogIO::POST;

    // In this generic version, one normalization factor per spw
    Block<String> col(3);
    col[0]="SPECTRAL_WINDOW_ID";
    col[1]="TIME";
    col[2]="ANTENNA1";
    CTIter ctiter(*ct_,col);

    // Cube iteration axes are pol and ant
    IPosition itax(2,0,2);
   
    while (!ctiter.pastEnd()) {
      Cube<Bool> fl(ctiter.flag());
      
      if (nfalse(fl)>0) {
	Cube<Complex> p(ctiter.cparam());
	ArrayIterator<Complex> soliter(p,itax,false);
	ArrayIterator<Bool> fliter(fl,itax,false);
	Array<Bool> sok;
	while (!soliter.pastEnd()) {
	  Array<Bool> thfl(fliter.array());
	  sok.assign(!thfl);
	  fillChanGapArray(soliter.array(),sok);
	  thfl=!sok;  // sok is revised (shapes necessarily match)
	  soliter.next();
	  fliter.next();
	}
	
	// record result...	
	ctiter.setcparam(p);
	ctiter.setflag(fl);
      }
      ctiter.next();
    }
  }
}

void BJones::fillChanGapArray(Array<Complex>& sol,
			      Array<Bool>& solOK) {

  // TBD: Do this with InterpolateArray1D a la CTPatchedInterp::resampleInFreq

  // Make the arrays 1D
  Vector<Complex> solv(sol.reform(IPosition(1,sol.nelements())));
  Vector<Bool> solOKv(solOK.reform(IPosition(1,solOK.nelements())));

  Int nChan(solv.nelements());
  Bool done(false);
  Int ich(0), ch1(-1), ch2(-1);
  Int dch(1);
  Float a0, da, a, p0, dp, p, fch;

  // Advance to first unflagged channel
  while(!solOKv(ich) && ich<nChan) ++ich;

  // Found no unflagged channels, so signal escape
  if (ich==nChan) done=true;

  // done turns true if we reach nChan, and nothing more to do
  while (!done) {

    // Advance to next flagged channel
    while(solOKv(ich) && ich<nChan) ++ich;

    if (ich<nChan) {

      // Left boundary of gap
      ch1=ich-1;     // (NB: above logic prevents ch1 < 0)

      // Find right boundary:
      ch2=ich+1;
      while (!solOKv(ch2) && ch2<nChan) ++ch2;

      if (ch2<nChan) {
	
	// The span of the interpolation (in channels)
	dch=ch2-ch1;

	//cout << ch1 << " " << ch2 << " " << dch << endl;

	// Interpolate only if gap is narrower than maxchangap_p
	if (dch<=maxchangap_p+1) {

	  // calculate interp params
	  a0=abs(solv(ch1));
	  da=abs(solv(ch2))-a0;
	  p0=arg(solv(ch1));
	  dp=arg(solv(ch2))-p0;
	  if (dp>C::pi) dp-=(2*C::pi);
	  if (dp<-C::pi) dp+=(2*C::pi);	

	  //cout << a0 << " " << da << " " << p0 << " " << dp << endl;


	  // interpolate the intervening channels
	  while (ich<ch2) {
	    fch=Float(ich-ch1)/Float(dch);
	    a=a0 + da*fch;
	    p=p0 + dp*fch;

	    // cout << " " << ich << " " << a << " " << p << endl;

	    solv(ich)=a*Complex(cos(p),sin(p));
	    solOKv(ich)=true;
	    ++ich;
	  }

	  // Begin looking for new gap on next round
	  ++ich;
	  
	}
	else
	  // we skipped a gap, look beyond it on next round
	  ich=ch2+1;
      }
      else
	// Reached nChan looking for ch2
	done=true;
    }
    else
      // Reach nChan looking for gaps 
      done=true;
  } // done

}

void BJones::calcWtScale() {

  //  cout << "BJones::calcWtScale()" << endl;


  // Access pre-chan-interp'd bandpass amplitude/flags
  Cube<Float> amps;
  Cube<Bool> ampfl;


  if (cpp_) {
    Cube<Float> tr;
    cpp_->getTresult(tr,ampfl,currObs(),currField(),-1,currSpw());
    amps.reference(tr(Slice(0,2,2),Slice(),Slice()));
  }
  else if (ci_) {
    amps=Cube<Float>(ci_->tresultF(currObs(),currField(),currSpw()))(Slice(0,2,2),Slice(),Slice());
    ampfl=ci_->tresultFlag(currObs(),currField(),currSpw());
  }
  else
    // BPOLY?
    throw(AipsError("Can't BJones::calcWtScale because there is no solution interpolation...."));
    

  // Initialize
  currWtScale()=1.0;

  
  IPosition ash(amps.shape());
  ash(1)=1;  // only one channel in the weights
  Cube<Float> cWS(currWtScale());

  IPosition it3(2,0,2);
  ArrayIterator<Float> A(amps,it3,false);
  ArrayIterator<Bool> Aok(ampfl,it3,false);
  ArrayIterator<Float> cWSi(cWS,it3,false);
    
  while (!A.pastEnd()) {

    // the weight scale factor is just the square of the 
    //   freq-dep normalization
    cWSi.array()*=(float)pow(calcPowerNorm(A.array(),!Aok.array()),2);

    A.next();
    Aok.next();
    cWSi.next();

  }

}





// **********************************************************
//  JJones Implementations
//

JJones::JJones(VisSet& vs) :
  VisCal(vs),             // virtual base
  VisMueller(vs),         // virtual base
  SolvableVisJones(vs)    // immediate parent
{
  if (prtlev()>2) cout << "J::J(vs)" << endl;
}

JJones::JJones(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),             // virtual base
  VisMueller(msname,MSnAnt,MSnSpw),         // virtual base
  SolvableVisJones(msname,MSnAnt,MSnSpw)    // immediate parent
{
  if (prtlev()>2) cout << "J::J(msname,MSnAnt,MSnSpw)" << endl;
}

JJones::JJones(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),             // virtual base
  VisMueller(msmc),         // virtual base
  SolvableVisJones(msmc)    // immediate parent
{
  if (prtlev()>2) cout << "J::J(msmc)" << endl;
}

JJones::JJones(const Int& nAnt) :
  VisCal(nAnt), 
  VisMueller(nAnt),
  SolvableVisJones(nAnt)
{
  if (prtlev()>2) cout << "J::J(nAnt)" << endl;
}

JJones::~JJones() {
  if (prtlev()>2) cout << "J::~J()" << endl;
}

void JJones::setSolve(const Record& solvepar) {

  // Call parent
  SolvableVisJones::setSolve(solvepar);

  // For J insist preavg is meaningful (5 minutes or user-supplied)
  if (preavg()<0.0)
    preavg()=300.0;

}


void JJones::guessPar(VisBuffer& vb) {

  if (prtlev()>4) cout << "   ::guessPar(vb)" << endl;

  // Make a guess at antenna-based J
  //  Correlation membership-dependence
  //  assumes corrs in canonical order
  //  nCorr = 1: use icorr=0
  //  nCorr = 2: use icorr=[0,1]
  //  nCorr = 4: use icorr=[0,3]

  // This method sets the off-diag = 0.0,
  //  and the on-diag as if this were G

  Int nCorr(2);
  Int nDataCorr(vb.visCube().shape()(0));
  Vector<Int> corridx(nCorr,0);
  if (nDataCorr==2) {
    corridx(0)=0;
    corridx(1)=1;
  } 
  else if (nDataCorr==4) {
    corridx(0)=0;
    corridx(1)=3;
  }

  Cube<Complex>& V(vb.visCube());
  Float amp(0.0),ampave(0.0);
  Int namp(0);
  solveCPar()=Complex(0.0);
  for (Int irow=1;irow<nAnt();++irow) {

    for (Int icorr=0;icorr<nCorr;icorr++) {
      Complex& Vi(V(corridx(icorr),0,irow));
      amp=abs(Vi);
      if (amp>0.0f) {
	solveCPar()(3*icorr,0,irow)=(conj(Vi)/amp);
	ampave+=amp;
	namp++;
	//	cout << "          " << abs(Vi) << " " << arg(Vi)*180.0/C::pi << endl;
      }
    }

  }

  // Scale them by the mean amplitude
  ampave/=Float(namp);
  ampave=sqrt(ampave);
  solveCPar()*=Complex(ampave);
  solveParOK()=true;

  //  cout << "post-guess:" << endl;
  //  cout << "solveCPar()   = " << solveCPar() << endl;
  //  cout << "phases       = " << phase(solveCPar())*180.0/C::pi << endl;
  //  cout << "solveParOK() = " << solveParOK() << endl;

}

// Fill the trivial DJ matrix elements
void JJones::initTrivDJ() {

  if (prtlev()>4) cout << "   J::initTrivDJ" << endl;

  // Must be trivial
  AlwaysAssert((trivialDJ()),AipsError);

  //  1 0     0 1     0 0     0 0
  //  0 0     0 0     1 0     0 1

  diffJElem().resize(IPosition(4,4,4,1,1));
  diffJElem()=0.0;
  diffJElem()(IPosition(4,0,0,0,0))=Complex(1.0);
  diffJElem()(IPosition(4,1,1,0,0))=Complex(1.0);
  diffJElem()(IPosition(4,2,2,0,0))=Complex(1.0);
  diffJElem()(IPosition(4,3,3,0,0))=Complex(1.0);

}



/*

// **********************************************************
//  MMueller: baseline-based (closure) solution
//

MMueller::MMueller(VisSet& vs) :
  VisCal(vs),             // virtual base
  VisMueller(vs),         // virtual base
  SolvableVisMueller(vs)    // immediate parent
{
  if (prtlev()>2) cout << "M::M(vs)" << endl;
}

MMueller::MMueller(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),             // virtual base
  VisMueller(msname,MSnAnt,MSnSpw),         // virtual base
  SolvableVisMueller(msname,MSnAnt,MSnSpw)    // immediate parent
{
  if (prtlev()>2) cout << "M::M(msname,MSnAnt,MSnSpw)" << endl;
}

MMueller::MMueller(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),             // virtual base
  VisMueller(msmc),         // virtual base
  SolvableVisMueller(msmc)  // immediate parent
{
  if (prtlev()>2) cout << "M::M(msmc)" << endl;
}

MMueller::MMueller(const Int& nAnt) :
  VisCal(nAnt), 
  VisMueller(nAnt),
  SolvableVisMueller(nAnt)
{
  if (prtlev()>2) cout << "M::M(nAnt)" << endl;
}

MMueller::~MMueller() {
  if (prtlev()>2) cout << "M::~M()" << endl;
}

void MMueller::setApply(const Record& apply) {

  SolvableVisCal::setApply(apply);

  // Force calwt to false for now
  calWt()=false;

}

void MMueller::newselfSolve(VisSet& vs, VisEquation& ve) {

  if (prtlev()>4) cout << "   M::selfSolve(ve)" << endl;

  // Inform logger/history
  logSink() << "Solving for " << typeName()
            << LogIO::POST;

  // Initialize the svc according to current VisSet
  //  (this counts intervals, sizes CalSet)
  Vector<Int> nChunkPerSol;
  Int nSol = sizeUpSolve(vs,nChunkPerSol);
  
  // Create the Caltable
  createMemCalTable();

  // The iterator, VisBuffer
  VisIter& vi(vs.iter());
  VisBuffer vb(vi);

  //  cout << "nSol = " << nSol << endl;
  //  cout << "nChunkPerSol = " << nChunkPerSol << endl;

  Vector<Int> slotidx(vs.numberSpw(),-1);

  Int nGood(0);
  vi.originChunks();
  for (Int isol=0;isol<nSol && vi.moreChunks();++isol) {

    // Arrange to accumulate
    VisBuffAccumulator vba(nAnt(),preavg(),false);
    
    for (Int ichunk=0;ichunk<nChunkPerSol(isol);++ichunk) {

      // Current _chunk_'s spw
      Int spw(vi.spectralWindow());

      // Abort if we encounter a spw for which a priori cal not available
      if (!ve.spwOK(spw))
        throw(AipsError("Pre-applied calibration not available for at least 1 spw. Check spw selection carefully."));


      // Collapse each timestamp in this chunk according to VisEq
      //  with calibration and averaging
      for (vi.origin(); vi.more(); vi++) {

        // Force read of the field Id
        vb.fieldId();

	// Apply the channel mask
	this->applyChanMask(vb);

        // This forces the data/model/wt I/O, and applies
        //   any prior calibrations
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
      // Advance the VisIter, if possible
      if (vi.moreChunks()) vi.nextChunk();

    }

    // Finalize the averged VisBuffer
    vba.finalizeAverage();

    // The VisBuffer to solve with
    VisBuffer& svb(vba.aveVisBuff());

    // Make data amp- or phase-only
    enforceAPonData(svb);

    // Establish meta-data for this interval
    //  (some of this may be used _during_ solve)
    //  (this sets currSpw() in the SVC)
    Bool vbOk=syncSolveMeta(svb,-1);

    Int thisSpw=spwMap()(svb.spectralWindow());
    slotidx(thisSpw)++;

    // We are actually solving for all channels simultaneously
    solveCPar().reference(solveAllCPar());
    solveParOK().reference(solveAllParOK());
    solveParErr().reference(solveAllParErr());
    solveParSNR().reference(solveAllParSNR());

    // Fill solveCPar() with 1, nominally, and flagged
    solveCPar()=Complex(1.0);
    solveParOK()=false;
    
    if (vbOk && svb.nRow()>0) {

      // Insist that channel,row shapes match
      IPosition visshape(svb.visCube().shape());
      AlwaysAssert(solveCPar().shape().getLast(2)==visshape.getLast(2),AipsError);
      
      // Zero flagged data
      IPosition vblc(3,0,0,0);
      IPosition vtrc(visshape);  vtrc-=1;      
      Int nCorr(visshape(0));
      for (Int i=0;i<nCorr;++i) {
	vblc(0)=vtrc(0)=i;
	svb.visCube()(vblc,vtrc).reform(visshape.getLast(2))(svb.flag())=Complex(1.0);
      }
      
      // Form correct slice of visCube to copy to solveCPar
      IPosition vcblc(3,0,0,0);
      IPosition vctrc(svb.visCube().shape()); vctrc-=1;
      IPosition vcstr(3,1,1,1);

      IPosition spblc(3,0,0,0);
      IPosition sptrc(solveCPar().shape()); sptrc-=1;

      IPosition flshape(svb.flag().shape());
      
      switch (nCorr) {
      case 1: {
	// fill 1st par only
	spblc(0)=sptrc(0)=0; 
	solveCPar()(spblc,sptrc)=svb.visCube();
	// first pol flags
	solveParOK()(spblc,sptrc).reform(flshape)=!svb.flag();
	break;
      }
      case 2: {
	// shapes match
	solveCPar()=svb.visCube();
	spblc(0)=sptrc(0)=0; 
	solveParOK()(spblc,sptrc).reform(flshape)=!svb.flag();
	spblc(0)=sptrc(0)=1; 
	solveParOK()(spblc,sptrc).reform(flshape)=!svb.flag();

	break;
      }
      case 4: {
	// Slice visCube with stride
	vcstr(0)=3;
	solveCPar()=svb.visCube()(vcblc,vctrc,vcstr);
	spblc(0)=sptrc(0)=0; 
	solveParOK()(spblc,sptrc).reform(flshape)=!svb.flag();
	spblc(0)=sptrc(0)=1; 
	solveParOK()(spblc,sptrc).reform(flshape)=!svb.flag();

	break;
      }
      default:
	throw(AipsError("Problem in MMueller::selfSolve."));
	break;
      }

      nGood++;

      // record in in-memory caltable
      keepNCT();
    }
  }
  
  logSink() << "  Found good "
            << typeName() << " solutions in "
            << nGood << " intervals."
            << LogIO::POST;

  // Store whole of result in a caltable
  if (nGood==0)
    logSink() << "No output calibration table written."
              << LogIO::POST;
  else {

    // Do global post-solve tinkering (e.g., normalization)
    globalPostSolveTinker();

    // write the table
    storeNCT();
  }

}

void MMueller::globalPostSolveTinker() {

  // normalize, if requested
  if (solnorm()) normalize();

}

void MMueller::createCorruptor(const VisIter& vi, const Record& simpar, const Int nSim) 
{
  LogIO os(LogOrigin("MM", "createCorruptor()", WHERE));

  if (prtlev()>2) cout << "   MM::createCorruptor()" << endl;
  os << LogIO::DEBUG1 << "   MM::createCorruptor()" 
     << LogIO::POST;

  atmcorruptor_p = new AtmosCorruptor();
  corruptor_p = atmcorruptor_p;

  // call generic parent to set corr,spw,etc info
  SolvableVisCal::createCorruptor(vi,simpar,nSim);

  Int rxType(0); // 0=2SB, 1=DSB
  if (simpar.isDefined("rxType")) {    
    rxType=simpar.asInt("rxType");
  }
 
  // this is the M type corruptor - maybe we should make the corruptor 
  // take the VC as an argument
  atmcorruptor_p->initialize(vi,simpar,VisCal::M,rxType); 
}






// **********************************************************
//  MfMueller: freq-dep MMueller
//

MfMueller::MfMueller(VisSet& vs) :
  VisCal(vs),             // virtual base
  VisMueller(vs),         // virtual base
  MMueller(vs)            // immediate parent
{
  if (prtlev()>2) cout << "Mf::Mf(vs)" << endl;
}

MfMueller::MfMueller(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),             // virtual base
  VisMueller(msname,MSnAnt,MSnSpw),         // virtual base
  MMueller(msname,MSnAnt,MSnSpw)            // immediate parent
{
  if (prtlev()>2) cout << "Mf::Mf(msname,MSnAnt,MSnSpw)" << endl;
}

MfMueller::MfMueller(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),             // virtual base
  VisMueller(msmc),         // virtual base
  MMueller(msmc)            // immediate parent
{
  if (prtlev()>2) cout << "Mf::Mf(msmc)" << endl;
}

MfMueller::MfMueller(const Int& nAnt) :
  VisCal(nAnt), 
  VisMueller(nAnt),
  MMueller(nAnt)
{
  if (prtlev()>2) cout << "Mf::Mf(nAnt)" << endl;
}

MfMueller::~MfMueller() {
  if (prtlev()>2) cout << "Mf::~Mf()" << endl;
}

void MfMueller::normalize() {

  // This is just like BJones
  // TBD:  consolidate (via generalized SVC::normalize(Block<String> cols) )

  // Only if we have a CalTable, and it is not empty
  if (ct_ && ct_->nrow()>0) {

    // TBD: trap attempts to normalize a caltable containing FPARAM (non-Complex)?

    logSink() << "Normalizing solutions per spw, pol, baseline, time"
	      << LogIO::POST;

    Block<String> col(4);
    col[0]="SPECTRAL_WINDOW_ID";
    col[1]="TIME";
    col[2]="ANTENNA1";
    col[3]="ANTENNA2";
    CTIter ctiter(*ct_,col);

    // Cube iteration axes are pol and ant
    IPosition itax(2,0,2);
   
    while (!ctiter.pastEnd()) {
      Cube<Bool> fl(ctiter.flag());
      
      if (nfalse(fl)>0) {
	Cube<Complex> p(ctiter.cparam());
	ArrayIterator<Complex> soliter(p,itax,false);
	ArrayIterator<Bool> fliter(fl,itax,false);
	while (!soliter.pastEnd()) {
	  normSolnArray(soliter.array(),!fliter.array(),true); // Do phase
	  soliter.next();
	  fliter.next();
	}
	
	// record result...	
	ctiter.setcparam(p);
      }
      ctiter.next();
    }
  }
  cout << "End of MfMueller::normalize()" << endl;
}

*/

// **********************************************************
//  TOpac
//

TOpac::TOpac(VisSet& vs) :
  VisCal(vs), 
  VisMueller(vs),
  TJones(vs),
  za_()
{
  if (prtlev()>2) cout << "TOpac::TOpac(vs)" << endl;
}

TOpac::TOpac(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw), 
  VisMueller(msname,MSnAnt,MSnSpw),
  TJones(msname,MSnAnt,MSnSpw),
  za_()
{
  if (prtlev()>2) cout << "TOpac::TOpac(msname,MSnAnt,MSnSpw)" << endl;
}

TOpac::TOpac(const MSMetaInfoForCal& msmc) :
  VisCal(msmc), 
  VisMueller(msmc),
  TJones(msmc),
  za_()
{
  if (prtlev()>2) cout << "TOpac::TOpac(msmc)" << endl;
}

TOpac::~TOpac() {
  if (prtlev()>2) cout << "TOpac::~TOpac()" << endl;
}

void TOpac::setApply(const Record& applypar) {

  // TBD: Handle spwmap properly?

  // Prepare zenith angle storage
  za().resize(nAnt());
  za().set(0.0);

  String table("");
  if (applypar.isDefined("table"))
    table=applypar.asString("table");

  if (table!="")
    // Attempt new-fashioned opacity table
    SolvableVisCal::setApply(applypar);
  else {

    // We are applying
    setSolved(false);
    setApplied(true);

    LogMessage message;
    { ostringstream o;
      o<< "Invoking opacity application without a caltable (e.g., " << endl
       << " via opacity=[...] in calibration tasks) will soon be disabled." << endl
       << " Please begin using gencal to generate an opacity caltable, " << endl
       << " and then supply that table in the standard manner." << endl;
      message.message(o);
      message.priority(LogMessage::WARN);
      logSink().post(message);
    }

    // Detect and extract opacities from applypar record (old way)
    if (applypar.isDefined("opacity")) {
      opacity_.resize();
      opacity_=applypar.asArrayDouble("opacity");
    }
    Int nopac=opacity_.nelements();
    
    if (nopac>0 && sum(opacity_)>0.0) {

      // Old-fashioned opacity_ is non-trivial, so adopt them
      
      if (nopac<nSpw()) {
	// Resize (with copy) to match nSpw, 
	//  duplicating last specified entry
	opacity_.resize(nSpw(),true);
	opacity_(IPosition(1,nopac),IPosition(1,nSpw()-1))=opacity_(nopac-1);
      }
    
      Int oldspw; oldspw=currSpw();
      for (Int ispw=0;ispw<nSpw();++ispw) {
	currSpw()=ispw;
	currRPar().resize(1,1,nAnt());
	currRPar()=Float(opacity_(ispw));
	currParOK().resize(1,1,nAnt());
	currParOK()=true;
      }
      currSpw()=oldspw;
      
    }
    else
      throw(AipsError("No opacity info specified."));
  }

}

String TOpac::applyinfo() {

  if (opacity_.nelements()==0)
    return TJones::applyinfo();
  else {
    ostringstream o;
    o << typeName();
    o << ": opacity=" << opacity_;
    o << boolalpha
    << " calWt=" << calWt();
    return String(o);
  }
}

// TOpac needs zenith angle (old VB version)
void TOpac::syncMeta(const VisBuffer& vb) {

  // Call parent (sets currTime())
  TJones::syncMeta(vb);

  // Current time's zenith angle...
  za().resize(nAnt());
  Vector<MDirection> antazel(vb.azel(currTime()));
  Double* a=za().data();
  for (Int iant=0;iant<nAnt();++iant,++a) 
    (*a)=C::pi_2 - antazel(iant).getAngle().getValue()(1);

}

// TOpac needs zenith angle  (VB2 version) 
void TOpac::syncMeta2(const vi::VisBuffer2& vb) {

  // Call parent (sets currTime())
  TJones::syncMeta2(vb);

  // Current time's zenith angle...
  za().resize(nAnt());
  Vector<MDirection> antazel(vb.azel(currTime()));
  Double* a=za().data();
  for (Int iant=0;iant<nAnt();++iant,++a) 
    (*a)=C::pi_2 - antazel(iant).getAngle().getValue()(1);

}


void TOpac::calcPar() {

  if (prtlev()>6) cout << "      TOpac::calcPar()" << endl;
  
  // If we are interpolating from a table, get opacity(time)
  if (ci_ || cpp_)
    SolvableVisCal::calcPar();
  else
    throw(AipsError("Error in TOpac::calcPar()"));

  // Pars now valid, matrices not yet
  validateP();
  invalidateJ();  // Force new calculation of za-dep matrix elements

}


void TOpac::calcAllJones() {

  if (prtlev()>6) cout << "       TOpac::calcAllJones()" << endl;

  // Nominally no opacity
  currJElem()=Complex(1.0);
  currJElemOK()=currParOK();

  Complex* J=currJElem().data();
  Float*  op=currRPar().data();
  Bool*   opok=currParOK().data();
  Double* a=za().data();
  for (Int iant=0; iant<nAnt(); ++iant,++J,++op,++opok,++a) {
    if ((*opok) && (*a)<C::pi_2) 
      (*J) = Complex(sqrt(exp(-1.0 * Double(*op)/cos(*a))));
  }

}


// **********************************************************
//  TfOpac
//

TfOpac::TfOpac(VisSet& vs) :
  VisCal(vs),             // virtual base
  VisMueller(vs),         // virtual base
  TOpac(vs)              // immediate parent
{
  if (prtlev()>2) cout << "TfOpac::TfOpac(vs)" << endl;
}

TfOpac::TfOpac(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),             // virtual base
  VisMueller(msname,MSnAnt,MSnSpw),         // virtual base
  TOpac(msname,MSnAnt,MSnSpw)              // immediate parent
{
  if (prtlev()>2) cout << "TfOpac::TfOpac(msname,MSnAnt,MSnSpw)" << endl;
}

TfOpac::TfOpac(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),             // virtual base
  VisMueller(msmc),         // virtual base
  TOpac(msmc)              // immediate parent
{
  if (prtlev()>2) cout << "TfOpac::TfOpac(msmc)" << endl;
}


//TfOpac::TfOpac(const Int& nAnt) :
//  VisCal(nAnt), 
//  VisMueller(nAnt),
//  TOpac(nAnt)
//{
//  if (prtlev()>2) cout << "TfOpac::TfOpac(nAnt)" << endl;
//}

TfOpac::~TfOpac() {
  if (prtlev()>2) cout << "TfOpac::~TfOpac()" << endl;
}

void TfOpac::calcAllJones() {

  if (prtlev()>6) cout << "       TfOpac::calcAllJones()" << endl;

  // Nominally no opacity
  currJElem()=Complex(1.0);
  currJElemOK()=currParOK();

  Complex* J=currJElem().data();
  Float*  op=currRPar().data();
  Bool*   opok=currParOK().data();
  Double* a=za().data();
  for (Int iant=0; iant<nAnt(); ++iant,++a) {
    for (Int ich=0; ich<nChanMat(); ich++, J++, op++, opok++) {
      if ((*opok) && (*a)<C::pi_2) 
        (*J) = Complex(sqrt(exp(-1.0 * Double(*op)/cos(*a))));
    }
  }
}


void TfOpac::calcWtScale() {

  // Initialize - not used for TfOpac, but we need to overwrite the
  // single channel version or it will throw an exception
  currWtScale()=1.0;

}


// **********************************************************
//  MMueller: baseline-based (closure) solution
//

MMueller::MMueller(VisSet& vs) :
  VisCal(vs),             // virtual base
  VisMueller(vs),         // virtual base
  SolvableVisMueller(vs),    // immediate parent
  useGenGathSolve_p(false)  // VisSet-driven ctor
{
  if (prtlev()>2) cout << "M::M(vs)" << endl;
}

MMueller::MMueller(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),             // virtual base
  VisMueller(msname,MSnAnt,MSnSpw),         // virtual base
  SolvableVisMueller(msname,MSnAnt,MSnSpw),    // immediate parent
  useGenGathSolve_p(true)  // modern ctor
{
  if (prtlev()>2) cout << "M::M(msname,MSnAnt,MSnSpw)" << endl;
}

MMueller::MMueller(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),             // virtual base
  VisMueller(msmc),         // virtual base
  SolvableVisMueller(msmc),  // immediate parent
  useGenGathSolve_p(true)  // modern ctor
{
  if (prtlev()>2) cout << "M::M(msmc)" << endl;
}

MMueller::MMueller(const Int& nAnt) :
  VisCal(nAnt), 
  VisMueller(nAnt),
  SolvableVisMueller(nAnt),
  useGenGathSolve_p(true)  // modern ctor
{
  if (prtlev()>2) cout << "M::M(nAnt)" << endl;
}

MMueller::~MMueller() {
  if (prtlev()>2) cout << "M::~M()" << endl;
}

void MMueller::setApply(const Record& apply) {

  SolvableVisCal::setApply(apply);

  // Force calwt to false for now
  calWt()=false;

}

void MMueller::newselfSolve(VisSet& vs, VisEquation& ve) {

  if (prtlev()>4) cout << "   M::selfSolve(ve)" << endl;

  AlwaysAssert(!useGenGathSolve_p,AipsError);

  // Inform logger/history
  logSink() << "Solving for " << typeName()
            << LogIO::POST;

  // Initialize the svc according to current VisSet
  //  (this counts intervals, sizes CalSet)
  Vector<Int> nChunkPerSol;
  Int nSol = sizeUpSolve(vs,nChunkPerSol);
  
  // Create the Caltable
  createMemCalTable();

  // The iterator, VisBuffer
  VisIter& vi(vs.iter());
  VisBuffer vb(vi);

  //  cout << "nSol = " << nSol << endl;
  //  cout << "nChunkPerSol = " << nChunkPerSol << endl;

  Vector<Int> slotidx(vs.numberSpw(),-1);

  Int nGood(0);
  vi.originChunks();
  for (Int isol=0;isol<nSol && vi.moreChunks();++isol) {

    // Arrange to accumulate
    VisBuffAccumulator vba(nAnt(),preavg(),false);
    
    for (Int ichunk=0;ichunk<nChunkPerSol(isol);++ichunk) {

      // Current _chunk_'s spw
      Int spw(vi.spectralWindow());

      // Abort if we encounter a spw for which a priori cal not available
      if (!ve.spwOK(spw))
        throw(AipsError("Pre-applied calibration not available for at least 1 spw. Check spw selection carefully."));


      // Collapse each timestamp in this chunk according to VisEq
      //  with calibration and averaging
      for (vi.origin(); vi.more(); vi++) {

        // Force read of the field Id
        vb.fieldId();

	// Apply the channel mask
	this->applyChanMask(vb);

        // This forces the data/model/wt I/O, and applies
        //   any prior calibrations
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
      // Advance the VisIter, if possible
      if (vi.moreChunks()) vi.nextChunk();

    }

    // Finalize the averged VisBuffer
    vba.finalizeAverage();

    // The VisBuffer to solve with
    VisBuffer& svb(vba.aveVisBuff());

    // Make data amp- or phase-only
    enforceAPonData(svb);

    // Establish meta-data for this interval
    //  (some of this may be used _during_ solve)
    //  (this sets currSpw() in the SVC)
    Bool vbOk=syncSolveMeta(svb,-1);

    Int thisSpw=spwMap()(svb.spectralWindow());
    slotidx(thisSpw)++;

    // We are actually solving for all channels simultaneously
    solveCPar().reference(solveAllCPar());
    solveParOK().reference(solveAllParOK());
    solveParErr().reference(solveAllParErr());
    solveParSNR().reference(solveAllParSNR());

    // Fill solveCPar() with 1, nominally, and flagged
    solveCPar()=Complex(1.0);
    solveParOK()=false;
    
    if (vbOk && svb.nRow()>0) {

      // Insist that channel,row shapes match
      IPosition visshape(svb.visCube().shape());
      AlwaysAssert(solveCPar().shape().getLast(2)==visshape.getLast(2),AipsError);
      
      // Zero flagged data
      IPosition vblc(3,0,0,0);
      IPosition vtrc(visshape);  vtrc-=1;      
      Int nCorr(visshape(0));
      for (Int i=0;i<nCorr;++i) {
	vblc(0)=vtrc(0)=i;
	svb.visCube()(vblc,vtrc).reform(visshape.getLast(2))(svb.flag())=Complex(1.0);
      }
      
      // Form correct slice of visCube to copy to solveCPar
      IPosition vcblc(3,0,0,0);
      IPosition vctrc(svb.visCube().shape()); vctrc-=1;
      IPosition vcstr(3,1,1,1);

      IPosition spblc(3,0,0,0);
      IPosition sptrc(solveCPar().shape()); sptrc-=1;

      IPosition flshape(svb.flag().shape());
      
      switch (nCorr) {
      case 1: {
	// fill 1st par only
	spblc(0)=sptrc(0)=0; 
	solveCPar()(spblc,sptrc)=svb.visCube();
	// first pol flags
	solveParOK()(spblc,sptrc).reform(flshape)=!svb.flag();
	break;
      }
      case 2: {
	// shapes match
	solveCPar()=svb.visCube();
	spblc(0)=sptrc(0)=0; 
	solveParOK()(spblc,sptrc).reform(flshape)=!svb.flag();
	spblc(0)=sptrc(0)=1; 
	solveParOK()(spblc,sptrc).reform(flshape)=!svb.flag();

	break;
      }
      case 4: {
	// Slice visCube with stride
	vcstr(0)=3;
	solveCPar()=svb.visCube()(vcblc,vctrc,vcstr);
	spblc(0)=sptrc(0)=0; 
	solveParOK()(spblc,sptrc).reform(flshape)=!svb.flag();
	spblc(0)=sptrc(0)=1; 
	solveParOK()(spblc,sptrc).reform(flshape)=!svb.flag();

	break;
      }
      default:
	throw(AipsError("Problem in MMueller::selfSolve."));
	break;
      }

      nGood++;

      // record in in-memory caltable
      keepNCT();
    }
  }
  
  logSink() << "  Found good "
            << typeName() << " solutions in "
            << nGood << " intervals."
            << LogIO::POST;

  // Store whole of result in a caltable
  if (nGood==0)
    logSink() << "No output calibration table written."
              << LogIO::POST;
  else {

    // Do global post-solve tinkering (e.g., normalization)
    globalPostSolveTinker();

    // write the table
    storeNCT();
  }

}

void MMueller::globalPostSolveTinker() {

  // normalize, if requested
  if (solnorm()) normalize();

}

void MMueller::selfSolveOne(SDBList& sdbs) {

  AlwaysAssert(useGenGathSolve_p,AipsError);
  
  // Call the implementation...
  this->solveOne(sdbs);

}

void MMueller::createCorruptor(const VisIter& vi, const Record& simpar, const Int nSim) 
{
  LogIO os(LogOrigin("MM", "createCorruptor()", WHERE));

  if (prtlev()>2) cout << "   MM::createCorruptor()" << endl;
  os << LogIO::DEBUG1 << "   MM::createCorruptor()" 
     << LogIO::POST;

  atmcorruptor_p = new AtmosCorruptor();
  corruptor_p = atmcorruptor_p;

  // call generic parent to set corr,spw,etc info
  SolvableVisCal::createCorruptor(vi,simpar,nSim);

  Int rxType(0); // 0=2SB, 1=DSB
  if (simpar.isDefined("rxType")) {    
    rxType=simpar.asInt("rxType");
  }
 
  // this is the M type corruptor - maybe we should make the corruptor 
  // take the VC as an argument
  atmcorruptor_p->initialize(vi,simpar,VisCal::M,rxType); 
}

void MMueller::solveOne(SDBList& sdbs) {

  AlwaysAssert(useGenGathSolve_p,AipsError);
  
  // This just a simple average of the parallel-hand

  Int nSDB=sdbs.nSDB();

  //cout << "nSDB=" << nSDB << endl;

  // We are actually solving for all channels simultaneously
  solveCPar().reference(solveAllCPar());
  solveParOK().reference(solveAllParOK());
  solveParErr().reference(solveAllParErr());
  solveParSNR().reference(solveAllParSNR());
  
  // Fill solveCPar() with 0, nominally, and flagged
  solveCPar()=Complex(0.0);
  solveParOK()=false;

  Cube<Float> sumwt(solveCPar().shape(),0.0f);
  Int nChan=sdbs.nChannels();
  AlwaysAssert(nChan==nChanPar(),AipsError);  // channelization should be consistent
  Int nCorr=sdbs.nCorrelations();
  Int dCorr=( nCorr==4 ? 3 : 1 );

  for (Int isdb=0;isdb<nSDB;++isdb) {
    SolveDataBuffer& sdb(sdbs(isdb));
    
    Int nRow=sdb.nRows();

    // Zero flagged data
    Cube<Complex> vc(sdb.visCubeCorrected()); // non-const access
    vc(sdb.flagCube())=Complex(0.0);

    const Vector<Int>& a1(sdb.antenna1()), a2(sdb.antenna2());

    Cube<Complex> vis,sol;
    Cube<Float> wt,swt;
    Cube<Bool> fl,solok;
    Slice chsl; // whole slice on chan axis
    for (Int irow=0;irow<nRow;++irow) {
      Slice vrsl(irow,1,1), srsl(blnidx(a1(irow),a2(irow)),1,1);
      for (Int icor=0;icor<nCorr;icor+=dCorr) {
	Slice vcsl(icor,1,1), scsl(icor%2,1,1);

	vis.reference(sdb.visCubeCorrected()(vcsl,chsl,vrsl));
	wt.reference(sdb.weightSpectrum()(vcsl,chsl,vrsl));
	fl.reference(sdb.flagCube()(vcsl,chsl,vrsl));
	sol.reference(solveCPar()(scsl,chsl,srsl));
	solok.reference(solveParOK()(scsl,chsl,srsl));
	swt.reference(sumwt(scsl,chsl,srsl));

	// Accumulate
	sol+=(vis*wt);
	swt+=wt;
	solok|=!fl;
      }
    }
  }

  // Normalize
  sumwt(!solveParOK())=(1.0);
  solveCPar()/=sumwt;

  // TBD....  (move to MMueller::postSolveTinker, since this is wrong for AMueller!)
  //solveCPar()(!solveParOK())=Complex(1.0); // set flagged ones to 1.0

}


// **********************************************************
//  MfMueller: freq-dep MMueller
//

MfMueller::MfMueller(VisSet& vs) :
  VisCal(vs),             // virtual base
  VisMueller(vs),         // virtual base
  MMueller(vs)            // immediate parent
{
  if (prtlev()>2) cout << "Mf::Mf(vs)" << endl;
}

MfMueller::MfMueller(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),             // virtual base
  VisMueller(msname,MSnAnt,MSnSpw),         // virtual base
  MMueller(msname,MSnAnt,MSnSpw)            // immediate parent
{
  if (prtlev()>2) cout << "Mf::Mf(msname,MSnAnt,MSnSpw)" << endl;
}

MfMueller::MfMueller(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),             // virtual base
  VisMueller(msmc),         // virtual base
  MMueller(msmc)            // immediate parent
{
  if (prtlev()>2) cout << "Mf::Mf(msmc)" << endl;
}

MfMueller::MfMueller(const Int& nAnt) :
  VisCal(nAnt), 
  VisMueller(nAnt),
  MMueller(nAnt)
{
  if (prtlev()>2) cout << "Mf::Mf(nAnt)" << endl;
}

MfMueller::~MfMueller() {
  if (prtlev()>2) cout << "Mf::~Mf()" << endl;
}

void MfMueller::normalize() {

  // This is just like BJones
  // TBD:  consolidate (via generalized SVC::normalize(Block<String> cols) )

  // Only if we have a CalTable, and it is not empty
  if (ct_ && ct_->nrow()>0) {

    // TBD: trap attempts to normalize a caltable containing FPARAM (non-Complex)?

    logSink() << "Normalizing solutions per spw, pol, baseline, time"
	      << LogIO::POST;

    Block<String> col(4);
    col[0]="SPECTRAL_WINDOW_ID";
    col[1]="TIME";
    col[2]="ANTENNA1";
    col[3]="ANTENNA2";
    CTIter ctiter(*ct_,col);

    // Cube iteration axes are pol and ant
    IPosition itax(2,0,2);
   
    while (!ctiter.pastEnd()) {
      Cube<Bool> fl(ctiter.flag());
      
      if (nfalse(fl)>0) {
	Cube<Complex> p(ctiter.cparam());
	ArrayIterator<Complex> soliter(p,itax,false);
	ArrayIterator<Bool> fliter(fl,itax,false);
	while (!soliter.pastEnd()) {
	  normSolnArray(soliter.array(),!fliter.array(),true); // Do phase
	  soliter.next();
	  fliter.next();
	}
	
	// record result...	
	ctiter.setcparam(p);
      }
      ctiter.next();
    }
  }
  cout << "End of MfMueller::normalize()" << endl;
}

} //# NAMESPACE CASA - END
