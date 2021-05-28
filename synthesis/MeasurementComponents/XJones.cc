//# XJones.cc: Implementation of cross-hand phase calibration
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

#include <synthesis/MeasurementComponents/XJones.h>
#include <synthesis/MeasurementComponents/CalCorruptor.h>
#include <synthesis/MeasurementComponents/MSMetaInfoForCal.h>
#include <synthesis/MeasurementComponents/SolveDataBuffer.h>
#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/VisBuffAccumulator.h>
#include <synthesis/CalTables/CTIter.h>
#include <synthesis/MeasurementEquations/VisEquation.h>
#include <scimath/Fitting/LSQFit.h>
#include <scimath/Fitting/LinearFit.h>
#include <scimath/Functionals/CompiledFunction.h>
#include <scimath/Functionals/Polynomial.h>
#include <scimath/Mathematics/AutoDiff.h>
#include <casa/BasicMath/Math.h>
#include <tables/TaQL/ExprNode.h>

#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/MaskArrMath.h>
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
//  XMueller: positiona angle for circulars
//

XMueller::XMueller(VisSet& vs) :
  VisCal(vs),             // virtual base
  VisMueller(vs),         // virtual base
  SolvableVisMueller(vs)    // immediate parent
{
  if (prtlev()>2) cout << "X::X(vs)" << endl;
}

XMueller::XMueller(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),             // virtual base
  VisMueller(msname,MSnAnt,MSnSpw),         // virtual base
  SolvableVisMueller(msname,MSnAnt,MSnSpw)    // immediate parent
{
  if (prtlev()>2) cout << "X::X(msname,MSnAnt,MSnSpw)" << endl;
}

XMueller::XMueller(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),             // virtual base
  VisMueller(msmc),         // virtual base
  SolvableVisMueller(msmc)    // immediate parent
{
  if (prtlev()>2) cout << "X::X(msmc)" << endl;
}

XMueller::XMueller(const Int& nAnt) :
  VisCal(nAnt), 
  VisMueller(nAnt),
  SolvableVisMueller(nAnt)
{
  if (prtlev()>2) cout << "X::X(nAnt)" << endl;
}

XMueller::~XMueller() {
  if (prtlev()>2) cout << "X::~X()" << endl;
}

void XMueller::setApply(const Record& apply) {

  SolvableVisCal::setApply(apply);

  // Force calwt to false 
  calWt()=false;

}


void XMueller::setSolve(const Record& solvepar) {

  cout << "XMueller: parType() = " << this->parType() << endl;

  SolvableVisCal::setSolve(solvepar);

  // Force calwt to false 
  calWt()=false;

  // For X insist preavg is meaningful (5 minutes or user-supplied)
  if (preavg()<0.0)
    preavg()=300.0;

  
  cout << "ct_ = " << ct_ << endl;


}

void XMueller::newselfSolve(VisSet& vs, VisEquation& ve) {

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

        // This forces the data/model/wt I/O, and applies
        //   any prior calibrations
        ve.collapse(vb);

        // If permitted/required by solvable component, normalize
        //if (normalizable())
	//          vb.normalize();

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

      // solve for the R-L phase term in the current VB
      solveOneVB(svb);

      if (solveParOK()(0,0,0))
	logSink() << "Position angle offset solution for " 
		  << msmc().fieldName(currField())
		  << " (spw = " << currSpw() << ") = "
		  << arg(solveCPar()(0,0,0))*180.0/C::pi/2.0
		  << " deg."
		  << LogIO::POST;
      else
	logSink() << "Position angle offset solution for " 
		  << msmc().fieldName(currField())
		  << " (spw = " << currSpw() << ") "
		  << " was not determined (insufficient data)."
		  << LogIO::POST;
	
      nGood++;
    }

    keepNCT();
    
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

    // Do global post-solve tinkering (e.g., phase-only, normalization, etc.)
    //  TBD
    // globalPostSolveTinker();

    // write the table
    storeNCT();

  }

}

void XMueller::calcAllMueller() {

  //  cout << "currMElem().shape() = " << currMElem().shape() << endl;

  // Put the phase factor into the cross-hand diagonals
  //  (1,0) for the para-hands  
  IPosition blc(3,0,0,0);
  IPosition trc(3,0,nChanMat()-1,nElem()-1);
  currMElem()(blc,trc)=Complex(1.0);

  blc(0)=trc(0)=1;
  currMElem()(blc,trc)=currCPar()(0,0,0);
  blc(0)=trc(0)=2;
  currMElem()(blc,trc)=conj(currCPar()(0,0,0));

  blc(0)=trc(0)=3;
  currMElem()(blc,trc)=Complex(1.0);

  currMElemOK()=true;

}


void XMueller::solveOneVB(const VisBuffer& vb) {

  // This just a simple average of the cross-hand
  //  visbilities...

  Complex d,md;
  Float wt,a;
  DComplex rl(0.0),lr(0.0);
  Double sumwt(0.0);
  for (Int irow=0;irow<vb.nRow();++irow) {
    if (!vb.flagRow()(irow) &&
	vb.antenna1()(irow)!=vb.antenna2()(irow)) {

      for (Int ich=0;ich<vb.nChannel();++ich) {
	if (!vb.flag()(ich,irow)) {
	  
	  // A common weight for both crosshands
	  // TBD: we should probably consider this carefully...
	  //  (also in D::guessPar...)
	  wt=Double(vb.weightMat()(1,irow)+
		    vb.weightMat()(2,irow))/2.0;

	  // correct weight for model normalization
	  a=abs(vb.modelVisCube()(1,ich,irow));
	  wt*=(a*a);
	  
	  if (wt>0.0) {
	    // Cross-hands only
	    for (Int icorr=1;icorr<3;++icorr) {
	      md=vb.modelVisCube()(icorr,ich,irow);
	      d=vb.visCube()(icorr,ich,irow);
	      
	      if (abs(d)>0.0) {
		
		if (icorr==1) 
		  rl+=DComplex(Complex(wt)*d/md);
		else
		  lr+=DComplex(Complex(wt)*d/md);
		
		sumwt+=Double(wt);
		
	      } // abs(d)>0
	    } // icorr
	  } // wt>0
	} // !flag
      } // ich
    } // !flagRow
  } // row
  
/*
  cout << "spw = " << currSpw() << endl;
  cout << " rl = " << rl << " " << arg(rl)*180.0/C::pi << endl;
  cout << " lr = " << lr << " " << arg(lr)*180.0/C::pi << endl;
*/

    // combine lr with rl
  rl+=conj(lr);
  
  // Normalize to unit amplitude
  //  (note that the phase result is insensitive to sumwt)
  Double amp=abs(rl);
  if (sumwt>0 && amp>0.0) {
    rl/=DComplex(amp);
    
    solveCPar()=Complex(rl);
    solveParOK()=true;
  }
  
}



// **********************************************************
//  XJones: position angle for circulars (antenna-based
//

XJones::XJones(VisSet& vs) :
  VisCal(vs),             // virtual base
  VisMueller(vs),         // virtual base
  SolvableVisJones(vs)    // immediate parent
{
  if (prtlev()>2) cout << "X::X(vs)" << endl;
}

XJones::XJones(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),             // virtual base
  VisMueller(msname,MSnAnt,MSnSpw),         // virtual base
  SolvableVisJones(msname,MSnAnt,MSnSpw)    // immediate parent
{
  if (prtlev()>2) cout << "X::X(msname,MSnAnt,MSnSpw)" << endl;
}

XJones::XJones(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),             // virtual base
  VisMueller(msmc),         // virtual base
  SolvableVisJones(msmc)    // immediate parent
{
  if (prtlev()>2) cout << "X::X(msmc)" << endl;
}

XJones::XJones(const Int& nAnt) :
  VisCal(nAnt), 
  VisMueller(nAnt),
  SolvableVisJones(nAnt)
{
  if (prtlev()>2) cout << "X::X(nAnt)" << endl;
}

XJones::~XJones() {
  if (prtlev()>2) cout << "X::~X()" << endl;
}

void XJones::setApply(const Record& apply) {

  SolvableVisCal::setApply(apply);

  // Force calwt to false 
  calWt()=false;

}


void XJones::setSolve(const Record& solvepar) {

  SolvableVisCal::setSolve(solvepar);

  // Force calwt to false 
  calWt()=false;

  // For X insist preavg is meaningful (5 minutes or user-supplied)
  if (preavg()<0.0)
    preavg()=300.0;

  // Force refant to none (==-1), because it is meaningless to
  //  apply a refant to an X solution
  if (refant()>-1) {
    logSink() << ".   (Ignoring specified refant for " 
	      << typeName() << " solve.)"
	      << LogIO::POST;
    refantlist().resize(1);
    refantlist()(0)=-1;
  }

}

void XJones::newselfSolve(VisSet& vs, VisEquation& ve) {

  if (prtlev()>4) cout << "   Xj::newselfSolve(ve)" << endl;

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

      // solve for the R-L phase term in the current VB
      solveOneVB(svb);

      if (ntrue(solveParOK())>0) {
	Float ang=arg(sum(solveCPar()(solveParOK()))/Float(ntrue(solveParOK())))*180.0/C::pi;


	logSink() << "Mean CROSS-HAND PHASE solution for " 
		  << msmc().fieldName(currField())
		  << " (spw = " << currSpw() << ") = "
		  << ang
		  << " deg."
		  << LogIO::POST;
      }
      else
	logSink() << "CROSS-HAND PHASE solution for " 
		  << msmc().fieldName(currField())
		  << " (spw = " << currSpw() << ") "
		  << " was not determined (insufficient data)."
		  << LogIO::POST;
	
      nGood++;
    }

    keepNCT();
    
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

    // Do global post-solve tinkering (e.g., phase-only, normalization, etc.)
    //  TBD
    // globalPostSolveTinker();

    // write the table
    storeNCT();
  }

}


void XJones::calcAllJones() {

  //  cout << "currJElem().shape() = " << currJElem().shape() << endl;

  //  put the par in the first position on the diagonal
  //  [p 0]
  //  [0 1]
  

  // Set first element to the parameter
  IPosition blc(3,0,0,0);
  IPosition trc(3,0,nChanMat()-1,nElem()-1);
  currJElem()(blc,trc)=currCPar();
  currJElemOK()(blc,trc)=currParOK();
  
  // Set second diag element to one
  blc(0)=trc(0)=1;
  currJElem()(blc,trc)=Complex(1.0);
  currJElemOK()(blc,trc)=currParOK();

}


void XJones::solveOneVB(const VisBuffer& vb) {

  // This just a simple average of the cross-hand
  //  visbilities...

  // We are actually solving for all channels simultaneously
  solveCPar().reference(solveAllCPar());
  solveParOK().reference(solveAllParOK());
  solveParErr().reference(solveAllParErr());
  solveParSNR().reference(solveAllParSNR());
  
  // Fill solveCPar() with 1, nominally, and flagged
  solveCPar()=Complex(1.0);
  solveParOK()=false;

  Int nChan=vb.nChannel();

  Complex d /*,md*/;
  Float wt;
  Vector<DComplex> rl(nChan,0.0),lr(nChan,0.0);
  Double sumwt(0.0);
  for (Int irow=0;irow<vb.nRow();++irow) {
    if (!vb.flagRow()(irow) &&
	vb.antenna1()(irow)!=vb.antenna2()(irow)) {

      for (Int ich=0;ich<nChan;++ich) {
	if (!vb.flag()(ich,irow)) {
	  
	  // A common weight for both crosshands
	  // TBD: we should probably consider this carefully...
	  //  (also in D::guessPar...)
	  wt=Double(vb.weightMat()(1,irow)+
		    vb.weightMat()(2,irow))/2.0;

	  // correct weight for model normalization
	  //	  a=abs(vb.modelVisCube()(1,ich,irow));
	  //	  wt*=(a*a);
	  
	  if (wt>0.0) {
	    // Cross-hands only
	    for (Int icorr=1;icorr<3;++icorr) {
	      //	      md=vb.modelVisCube()(icorr,ich,irow);
	      d=vb.visCube()(icorr,ich,irow);
	      
	      if (abs(d)>0.0) {
		
		if (icorr==1) 
		  rl(ich)+=DComplex(Complex(wt)*d);
		//		  rl(ich)+=DComplex(Complex(wt)*d/md);
		else
		  lr(ich)+=DComplex(Complex(wt)*d);
		//		  lr(ich)+=DComplex(Complex(wt)*d/md);
		
		sumwt+=Double(wt);
		
	      } // abs(d)>0
	    } // icorr
	  } // wt>0
	} // !flag
      } // ich
    } // !flagRow
  } // row
  

  //  cout << "spw = " << currSpw() << endl;
  //  cout << " rl = " << rl << " " << phase(rl)*180.0/C::pi << endl;
  //  cout << " lr = " << lr << " " << phase(lr)*180.0/C::pi << endl;

  // Record results
  solveCPar()=Complex(1.0);
  solveParOK()=false;
  for (Int ich=0;ich<nChan;++ich) {
    // combine lr with rl
    rl(ich)+=conj(lr(ich));
  
    // Normalize to unit amplitude
    //  (note that the phase result is insensitive to sumwt)
    Double amp=abs(rl(ich));
    // For now, all antennas get the same solution
    IPosition blc(3,0,0,0);
    IPosition trc(3,0,0,nElem()-1);
    if (sumwt>0 && amp>0.0) {
      rl(ich)/=DComplex(amp);
      blc(1)=trc(1)=ich;
      solveCPar()(blc,trc)=Complex(rl(ich));
      solveParOK()(blc,trc)=true;
    }
  }

  
  if (ntrue(solveParOK())>0) {
    Float ang=arg(sum(solveCPar()(solveParOK()))/Float(ntrue(solveParOK())))*180.0/C::pi;
    
    
    logSink() << "Mean CROSS-HAND PHASE solution for " 
	      << msmc().fieldName(currField())
	      << " (spw = " << currSpw() << ") = "
	      << ang
	      << " deg."
	      << LogIO::POST;
  }
  else
    logSink() << "CROSS-HAND PHASE solution for " 
	      << msmc().fieldName(currField())
	      << " (spw = " << currSpw() << ") "
	      << " was not determined (insufficient data)."
	      << LogIO::POST;
  
}

void XJones::solveOneSDB(SolveDataBuffer& sdb) {

  // This just a simple average of the cross-hand
  //  visbilities...

  // We are actually solving for all channels simultaneously
  solveCPar().reference(solveAllCPar());
  solveParOK().reference(solveAllParOK());
  solveParErr().reference(solveAllParErr());
  solveParSNR().reference(solveAllParSNR());
  
  // Fill solveCPar() with 1, nominally, and flagged
  solveCPar()=Complex(1.0);
  solveParOK()=false;

  Int nChan=sdb.nChannels();

  Complex d /*,md*/;
  Float wt;
  Vector<DComplex> rl(nChan,0.0),lr(nChan,0.0);
  Double sumwt(0.0);
  for (Int irow=0;irow<sdb.nRows();++irow) {
    if (!sdb.flagRow()(irow) &&
	sdb.antenna1()(irow)!=sdb.antenna2()(irow)) {

      for (Int ich=0;ich<nChan;++ich) {
	if (!sdb.flagCube()(1,ich,irow) &&
	    !sdb.flagCube()(2,ich,irow)) {
	  
	  // A common weight for both crosshands
	  // TBD: we should probably consider this carefully...
	  //  (also in D::guessPar...)
	  wt=Double(sdb.weightSpectrum()(1,ich,irow)+
		    sdb.weightSpectrum()(2,ich,irow))/2.0;

	  // correct weight for model normalization
	  //	  a=abs(vb.modelVisCube()(1,ich,irow));
	  //	  wt*=(a*a);
	  
	  if (wt>0.0) {
	    // Cross-hands only
	    for (Int icorr=1;icorr<3;++icorr) {
	      d=sdb.visCubeCorrected()(icorr,ich,irow);
	      
	      if (abs(d)>0.0) {
		
		if (icorr==1) 
		  rl(ich)+=DComplex(Complex(wt)*d);
		else
		  lr(ich)+=DComplex(Complex(wt)*d);
		
		sumwt+=Double(wt);
		
	      } // abs(d)>0
	    } // icorr
	  } // wt>0
	} // !flag
      } // ich
    } // !flagRow
  } // row
  

  //  cout << "spw = " << currSpw() << endl;
  //  cout << " rl = " << rl << " " << phase(rl)*180.0/C::pi << endl;
  //  cout << " lr = " << lr << " " << phase(lr)*180.0/C::pi << endl;

  // Record results
  solveCPar()=Complex(1.0);
  solveParOK()=false;
  for (Int ich=0;ich<nChan;++ich) {
    // combine lr with rl
    rl(ich)+=conj(lr(ich));
  
    // Normalize to unit amplitude
    //  (note that the phase result is insensitive to sumwt)
    Double amp=abs(rl(ich));
    // For now, all antennas get the same solution
    IPosition blc(3,0,0,0);
    IPosition trc(3,0,0,nElem()-1);
    if (sumwt>0 && amp>0.0) {
      rl(ich)/=DComplex(amp);
      blc(1)=trc(1)=ich;
      solveCPar()(blc,trc)=Complex(rl(ich));
      solveParOK()(blc,trc)=true;
    }
  }

  
  if (ntrue(solveParOK())>0) {
    Float ang=arg(sum(solveCPar()(solveParOK()))/Float(ntrue(solveParOK())))*180.0/C::pi;
    
    
    logSink() << "Mean CROSS-HAND PHASE solution for " 
	      << msmc().fieldName(currField())
	      << " (spw = " << currSpw() << ") = "
	      << ang
	      << " deg."
	      << LogIO::POST;
  }
  else
    logSink() << "CROSS-HAND PHASE solution for " 
	      << msmc().fieldName(currField())
	      << " (spw = " << currSpw() << ") "
	      << " was not determined (insufficient data)."
	      << LogIO::POST;
  
}


void XJones::solveOne(SDBList& sdbs) {

  // This just a simple average of the cross-hand
  //  visbilities...

  Int nSDB=sdbs.nSDB();

  //cout << "nSDB=" << nSDB << endl;

  // We are actually solving for all channels simultaneously
  solveCPar().reference(solveAllCPar());
  solveParOK().reference(solveAllParOK());
  solveParErr().reference(solveAllParErr());
  solveParSNR().reference(solveAllParSNR());
  
  // Fill solveCPar() with 1, nominally, and flagged
  solveCPar()=Complex(1.0);
  solveParOK()=false;

  Int nChan=sdbs.nChannels();

  Complex d /*,md*/;
  Float wt;
  Vector<DComplex> rl(nChan,0.0),lr(nChan,0.0);
  Double sumwt(0.0);
  for (Int isdb=0;isdb<nSDB;++isdb) {
    SolveDataBuffer& sdb(sdbs(isdb));
  for (Int irow=0;irow<sdb.nRows();++irow) {
    if (!sdb.flagRow()(irow) &&
	sdb.antenna1()(irow)!=sdb.antenna2()(irow)) {

      for (Int ich=0;ich<nChan;++ich) {
	if (!sdb.flagCube()(1,ich,irow) &&
	    !sdb.flagCube()(2,ich,irow)) {
	  
	  // A common weight for both crosshands
	  // TBD: we should probably consider this carefully...
	  //  (also in D::guessPar...)
	  wt=Double(sdb.weightSpectrum()(1,ich,irow)+
		    sdb.weightSpectrum()(2,ich,irow))/2.0;

	  // correct weight for model normalization
	  //	  a=abs(vb.modelVisCube()(1,ich,irow));
	  //	  wt*=(a*a);
	  
	  if (wt>0.0) {
	    // Cross-hands only
	    for (Int icorr=1;icorr<3;++icorr) {
	      d=sdb.visCubeCorrected()(icorr,ich,irow);
	      
	      if (abs(d)>0.0) {
		
		if (icorr==1) 
		  rl(ich)+=DComplex(Complex(wt)*d);
		else
		  lr(ich)+=DComplex(Complex(wt)*d);
		
		sumwt+=Double(wt);
		
	      } // abs(d)>0
	    } // icorr
	  } // wt>0
	} // !flag
      } // ich
    } // !flagRow
  } // row
  } // isdb

  //  cout << "spw = " << currSpw() << endl;
  //  cout << " rl = " << rl << " " << phase(rl)*180.0/C::pi << endl;
  //  cout << " lr = " << lr << " " << phase(lr)*180.0/C::pi << endl;

  // Record results
  solveCPar()=Complex(1.0);
  solveParOK()=false;
  for (Int ich=0;ich<nChan;++ich) {
    // combine lr with rl
    rl(ich)+=conj(lr(ich));
  
    // Normalize to unit amplitude
    //  (note that the phase result is insensitive to sumwt)
    Double amp=abs(rl(ich));
    // For now, all antennas get the same solution
    IPosition blc(3,0,0,0);
    IPosition trc(3,0,0,nElem()-1);
    if (sumwt>0 && amp>0.0) {
      rl(ich)/=DComplex(amp);
      blc(1)=trc(1)=ich;
      solveCPar()(blc,trc)=Complex(rl(ich));
      solveParOK()(blc,trc)=true;
    }
  }

  
  if (ntrue(solveParOK())>0) {
    Float ang=arg(sum(solveCPar()(solveParOK()))/Float(ntrue(solveParOK())))*180.0/C::pi;
    
    
    logSink() << "Mean CROSS-HAND PHASE solution for " 
	      << msmc().fieldName(currField())
	      << " (spw = " << currSpw() << ") = "
	      << ang
	      << " deg."
	      << LogIO::POST;
  }
  else
    logSink() << "CROSS-HAND PHASE solution for " 
	      << msmc().fieldName(currField())
	      << " (spw = " << currSpw() << ") "
	      << " was not determined (insufficient data)."
	      << LogIO::POST;
  
}

// **********************************************************
//  XfJones: CHANNELIZED position angle for circulars (antenna-based)
//

XfJones::XfJones(VisSet& vs) :
  VisCal(vs),             // virtual base
  VisMueller(vs),         // virtual base
  XJones(vs)              // immediate parent
{
  if (prtlev()>2) cout << "Xf::Xf(vs)" << endl;
}

XfJones::XfJones(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),             // virtual base
  VisMueller(msname,MSnAnt,MSnSpw),         // virtual base
  XJones(msname,MSnAnt,MSnSpw)              // immediate parent
{
  if (prtlev()>2) cout << "Xf::Xf(msname,MSnAnt,MSnSpw)" << endl;
}

XfJones::XfJones(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),             // virtual base
  VisMueller(msmc),         // virtual base
  XJones(msmc)              // immediate parent
{
  if (prtlev()>2) cout << "Xf::Xf(msmc)" << endl;
}

XfJones::XfJones(const Int& nAnt) :
  VisCal(nAnt), 
  VisMueller(nAnt),
  XJones(nAnt)
{
  if (prtlev()>2) cout << "Xf::Xf(nAnt)" << endl;
}

XfJones::~XfJones() {
  if (prtlev()>2) cout << "Xf::~Xf()" << endl;
}

void XfJones::initSolvePar() {

  SolvableVisJones::initSolvePar();
  return;

}



// **********************************************************
//  XparangJones Implementations
//

XparangJones::XparangJones(VisSet& vs) :
  VisCal(vs),             // virtual base
  VisMueller(vs),         // virtual base
  XJones(vs),             // immediate parent
  QU_(),
  QURec_()
{
  if (prtlev()>2) cout << "Xparang::Xparang(vs)" << endl;
}

XparangJones::XparangJones(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),             // virtual base
  VisMueller(msname,MSnAnt,MSnSpw),         // virtual base
  XJones(msname,MSnAnt,MSnSpw),             // immediate parent
  QU_(),
  QURec_()
{
  if (prtlev()>2) cout << "Xparang::Xparang(msname,MSnAnt,MSnSpw)" << endl;
}

XparangJones::XparangJones(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),             // virtual base
  VisMueller(msmc),         // virtual base
  XJones(msmc),             // immediate parent
  QU_(),
  QURec_()
{
  if (prtlev()>2) cout << "Xparang::Xparang(msmc)" << endl;
}

XparangJones::XparangJones(const Int& nAnt) :
  VisCal(nAnt), 
  VisMueller(nAnt),
  XJones(nAnt),
  QU_(),
  QURec_()
{
  if (prtlev()>2) cout << "Xparang::Xparang(nAnt)" << endl;
}

XparangJones::~XparangJones() {
  if (prtlev()>2) cout << "Xparang::~Xparang()" << endl;
}

void XparangJones::setApply(const Record& apply) {

  XJones::setApply(apply);

  // Force calwt to false 
  calWt()=false;

}



void XparangJones::selfGatherAndSolve(VisSet& vs, VisEquation& ve) {

  if (prtlev()>4) cout << "   GlnXph::selfGatherAndSolve(ve)" << endl;

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

    // Establish meta-data for this interval
    //  (some of this may be used _during_ solve)
    //  (this sets currSpw() in the SVC)
    Bool vbOk=syncSolveMeta(svb,-1);

    if (vbOk && svb.nRow()>0) {

      // solve for the X-Y phase term in the current VB
      solveOneVB(svb);

      nGood++;
    }

    keepNCT();
    
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

    // Do global post-solve tinkering (e.g., phase-only, normalization, etc.)
    globalPostSolveTinker();

    // write the table
    storeNCT();
  }

}

// Handle trivial vbga
void XparangJones::selfSolveOne(VisBuffGroupAcc& vbga) {

  // Expecting only on VB in the vbga (with many times)
  if (vbga.nBuf()!=1)
    throw(AipsError("XparangJones can't process multi-vb vbga."));

  // Call single-VB specialized solver with the one vb
  this->solveOneVB(vbga(0));

}

// SDBList (VI2) version
void XparangJones::selfSolveOne(SDBList& sdbs) {

  // Expecting multiple SDBs (esp. in time)
  // (2020Oct01: insufficient data caught later in solveOne(sdbs))
  //  if (sdbs.nSDB()==1)
  //    throw(AipsError("XparangJones needs multiple SDBs"));

  // Call single-VB specialized solver with the one vb
  this->solveOne(sdbs);

}

// Solve for the X-Y phase from the cross-hand's slope in R/I
void XparangJones::solveOneVB(const VisBuffer& vb) {

  // ensure
  if (QU_.shape()!=IPosition(2,2,nSpw())) {
    QU_.resize(2,nSpw());
    QU_.set(0.0);
  }

  Int thisSpw=spwMap()(vb.spectralWindow());
  
  // We are actually solving for all channels simultaneously
  solveCPar().reference(solveAllCPar());
  solveParOK().reference(solveAllParOK());
  solveParErr().reference(solveAllParErr());
  solveParSNR().reference(solveAllParSNR());
  
  // Fill solveCPar() with 1, nominally, and flagged
  solveCPar()=Complex(1.0);
  solveParOK()=false;

  Int nChan=vb.nChannel();
  //  if (nChan>1)
  //    throw(AipsError("X-Y phase solution NYI for channelized data"));

  // Find number of timestamps in the VB
  Vector<uInt> ord;
  Int nTime=genSort(ord,vb.time(),Sort::Ascending,Sort::NoDuplicates);

  Matrix<Double> x(nTime,nChan,0.0),y(nTime,nChan,0.0),wt(nTime,nChan,0.0),sig(nTime,nChan,0.0);
  Matrix<Bool> mask(nTime,nChan,false);

  mask.set(false);
  Complex v(0.0);
  Float wt0(0.0);
  Int iTime(-1);
  Double currtime(-1.0);
  for (Int irow=0;irow<vb.nRow();++irow) {
    if (!vb.flagRow()(irow) &&
	vb.antenna1()(irow)!=vb.antenna2()(irow)) {

      // Advance time index when we see a new time
      if (vb.time()(irow)!=currtime) {
	++iTime;
	currtime=vb.time()(irow); // remember the new current time
      }

      // Weights not yet chan-dep
      wt0=(vb.weightMat()(1,irow)+vb.weightMat()(2,irow));
      if (wt0>0.0) {

	for (Int ich=0;ich<nChan;++ich) {
	  if (!vb.flag()(ich,irow)) {
	    v=vb.visCube()(1,ich,irow)+conj(vb.visCube()(2,ich,irow));
	    x(iTime,ich)+=Double(wt0*real(v));
	    y(iTime,ich)+=Double(wt0*imag(v));
	    wt(iTime,ich)+=Double(wt0);
	  }
	}
      }
    }
  }

  // Normalize data by accumulated weights
  for (Int itime=0;itime<nTime;++itime) {
    for (Int ich=0;ich<nChan;++ich) {
      if (wt(itime,ich)>0.0) {
	x(itime,ich)/=wt(itime,ich);
	y(itime,ich)/=wt(itime,ich);
	sig(itime,ich)=sqrt(1.0/wt(itime,ich));
	mask(itime,ich)=true;
      }
      else
	sig(itime,ich)=DBL_MAX;    // ~zero weight
    }
  }

  // Solve for each channel
  Vector<Complex> Cph(nChan);
  Cph.set(Complex(1.0,0.0));
  Float currAmb(1.0);
  Bool report(false);
  for (Int ich=0;ich<nChan;++ich) {

    if (sum(wt.column(ich))>0.0) {
      report=true;
      LinearFit<Double> phfitter;
      Polynomial<AutoDiff<Double> > line(1);
      phfitter.setFunction(line);
      Vector<Bool> m(mask.column(ich));

      // Fit shallow and steep, and always prefer shallow
      
      // Assumed shallow solve:
      Vector<Double> solnA;
      solnA.assign(phfitter.fit(x.column(ich),y.column(ich),sig.column(ich),&m));

      // Assumed steep solve:
      Vector<Double> solnB;
      solnB.assign(phfitter.fit(y.column(ich),x.column(ich),sig.column(ich),&m));

      Double Xph(0.0);
      if (abs(solnA(1))<abs(solnB(1))) {
	Xph=atan(solnA(1));
      }
      else {
	Xph=atan(1.0/solnB(1));
      }

      Cph(ich)=currAmb*Complex(DComplex(cos(Xph),sin(Xph)));

      // Watch for and remove ambiguity changes which can
      //  occur near Xph~= +/-90 deg (the atan above can jump)
      //  (NB: this does _not_ resolve the amb; it merely makes
      //   it consistent within the spw)
      if (ich>0) {
	// If Xph changes by more than pi/2, probably a ambig jump...
	Float dang=abs(arg(Cph(ich)/Cph(ich-1)));
	if (dang > (C::pi/2.)) {
	  Cph(ich)*=-1.0;   // fix this one
	  currAmb*=-1.0;    // reverse currAmb, so curr amb is carried forward
	  //	  cout << "  Found XY phase ambiguity jump at chan=" << ich << " in spw=" << currSpw();
	}
      }

      //      cout << " (" << currAmb << ")";
      //      cout << endl;


      // Set all antennas with this X-Y phase (as a complex number)
      solveCPar()(Slice(0,1,1),Slice(ich,1,1),Slice())=Cph(ich);
      solveParOK()(Slice(),Slice(ich,1,1),Slice())=true;
    }
    else {
      solveCPar()(Slice(0,1,1),Slice(ich,1,1),Slice())=Complex(1.0);
      solveParOK()(Slice(),Slice(ich,1,1),Slice())=false;
    }
  }

  if (report)
    cout << endl 
	 << "Spw = " << thisSpw
	 << " (ich=" << nChan/2 << "/" << nChan << "): " << endl
	 << " X-Y phase = " << arg(Cph[nChan/2])*180.0/C::pi << " deg." << endl;
      

  // Now fit for the source polarization
  {

    Vector<Double> wtf(nTime,0.0),sigf(nTime,0.0),xf(nTime,0.0),yf(nTime,0.0);
    Vector<Bool> maskf(nTime,false);
    Float wt0;
    Complex v;
    Double currtime(-1.0);
    Int iTime(-1);
    for (Int irow=0;irow<vb.nRow();++irow) {
      if (!vb.flagRow()(irow) &&
	  vb.antenna1()(irow)!=vb.antenna2()(irow)) {
	
	if (vb.time()(irow)!=currtime) {
	  // Advance time index when we see a new time
	  ++iTime;
	  currtime=vb.time()(irow); // remember the new current time
	}

	// Weights not yet chan-dep
	wt0=(vb.weightMat()(1,irow)+vb.weightMat()(2,irow));
	if (wt0>0.0) {
	  for (Int ich=0;ich<nChan;++ich) {
	    
	    if (!vb.flag()(ich,irow)) {
	      // Correct x-hands for xy-phase and add together
	      v=vb.visCube()(1,ich,irow)/Cph(ich)+vb.visCube()(2,ich,irow)/conj(Cph(ich));
	      xf(iTime)+=Double(wt0*2.0*(vb.feed_pa(vb.time()(irow))(0)));
	      yf(iTime)+=Double(wt0*real(v)/2.0);
	      wtf(iTime)+=Double(wt0);
	    }
	  }
	}
      }
    }
    
    // Normalize data by accumulated weights
    for (Int itime=0;itime<nTime;++itime) {
      if (wtf(itime)>0.0) {
	xf(itime)/=wtf(itime);
	yf(itime)/=wtf(itime);
	sigf(itime)=sqrt(1.0/wtf(itime));
	maskf(itime)=true;
      }
      else
	sigf(itime)=DBL_MAX;    // ~zero weight
    }
    
    // p0=Q, p1=U, p2 = real part of net instr pol offset
    //  x is TWICE the parallactic angle
    CompiledFunction<AutoDiff<Double> > fn;
    fn.setFunction("-p0*sin(x) + p1*cos(x) + p2");

    LinearFit<Double> fitter;
    fitter.setFunction(fn);
    
    Vector<Double> soln=fitter.fit(xf,yf,sigf,&maskf);
    
    QU_(0,thisSpw) = soln(0);
    QU_(1,thisSpw) = soln(1);

    cout << " Fractional Poln: "
	 << "Q = " << QU_(0,thisSpw) << ", "
	 << "U = " << QU_(1,thisSpw) << "; "
	 << "P = " << sqrt(soln(0)*soln(0)+soln(1)*soln(1)) << ", "
	 << "X = " << atan2(soln(1),soln(0))*90.0/C::pi << "deg."
	 << endl;
    cout << " Net (over baselines) instrumental polarization: " 
	 << soln(2) << endl;

  }	

}

// Solve for the cross-hand phase from the cross-hand's slope in R/I
void XparangJones::solveOne(SDBList& sdbs) {

  //cout << "solvePol() = " << solvePol() << endl;
  //  cout << boolalpha;
  //  cout << "normalizable() = " << this->normalizable() << endl;
  //  cout << "divideByStokesIModelForSolve() = " << this->divideByStokesIModelForSolve() << endl;

  // ensure
  if (QU_.shape()!=IPosition(2,2,nSpw())) {
    QU_.resize(2,nSpw());
    QU_.set(0.0);
  }

  Int thisSpw=sdbs.aggregateSpw();
  
  // We are actually solving for all channels simultaneously
  solveCPar().reference(solveAllCPar());
  solveParOK().reference(solveAllParOK());
  solveParErr().reference(solveAllParErr());
  solveParSNR().reference(solveAllParSNR());
  
  // Fill solveCPar() with 1, nominally, and flagged
  solveCPar()=Complex(1.0);
  solveParOK()=false;

  Int nChan=sdbs.nChannels();

  // Number of datapoints in fit is the number of SDBs
  Int nSDB=sdbs.nSDB();


  // This uniformizes the baseline-dep flags over all times (sdbs)
  //  (~ensures minimally undistorted solution below, which uses average over baseline)
  String extBLmessage;
  Int nGoodSDB(0);
  nGoodSDB=sdbs.extendCrossHandBaselineFlags(extBLmessage);

  if (sdbs.polBasis()==String("LIN")) {

  logSink() << "Solving for Cross-hand Phase and calibrator linear polarization in the LINEAR basis in spw=" << thisSpw<< LogIO::POST;

  // Report surviving data from extendCrossHandBaselineFlags
  logSink() << " " << extBLmessage << LogIO::POST;

  // Trap insufficient data case (linear; Q,U + real offset in real part)
  if (nGoodSDB<3) 
    throw(AipsError("For a viable solution, the Xfparang+QU solve requires at least THREE distinct unflagged data segments in time/parallactic angle in the LINEAR basis.  Cannot proceed."));

  Matrix<Double> x(nSDB,nChan,0.0),y(nSDB,nChan,0.0),wt(nSDB,nChan,0.0),sig(nSDB,nChan,0.0);
  Matrix<Bool> mask(nSDB,nChan,false);

  mask.set(false);
  Complex v(0.0);
  Float wt0(0.0);
  
  for (Int isdb=0;isdb<nSDB;++isdb) {
    SolveDataBuffer& sdb(sdbs(isdb));

    for (Int irow=0;irow<sdb.nRows();++irow) {
      if (!sdb.flagRow()(irow) &&
	  sdb.antenna1()(irow)!=sdb.antenna2()(irow)) {
	
	for (Int ich=0;ich<nChan;++ich) {
	  if (!sdb.flagCube()(1,ich,irow)) {
	    wt0=sdb.weightSpectrum()(1,ich,irow);
	    v=sdb.visCubeCorrected()(1,ich,irow);
	    x(isdb,ich)+=Double(wt0*real(v));
	    y(isdb,ich)+=Double(wt0*imag(v));
	    wt(isdb,ich)+=Double(wt0);
	  }
	  if (!sdb.flagCube()(2,ich,irow)) {
	    wt0=sdb.weightSpectrum()(2,ich,irow);
	    v=conj(sdb.visCubeCorrected()(2,ich,irow));
	    x(isdb,ich)+=Double(wt0*real(v));
	    y(isdb,ich)+=Double(wt0*imag(v));
	    wt(isdb,ich)+=Double(wt0);
	  }
	}
      }
    }
  }

  // Normalize data by accumulated weights
  for (Int isdb=0;isdb<nSDB;++isdb) {
    for (Int ich=0;ich<nChan;++ich) {
      if (wt(isdb,ich)>0.0) {
	x(isdb,ich)/=wt(isdb,ich);
	y(isdb,ich)/=wt(isdb,ich);
	sig(isdb,ich)=sqrt(1.0/wt(isdb,ich));
	mask(isdb,ich)=true;
      }
      else
	sig(isdb,ich)=DBL_MAX;    // ~zero weight
    }
  }

  // Solve for each channel
  Vector<Complex> Cph(nChan);
  Cph.set(Complex(1.0,0.0));
  Float currAmb(1.0);
  Bool report(false);
  for (Int ich=0;ich<nChan;++ich) {

    if (sum(wt.column(ich))>0.0) {
      //      report=true;
      LinearFit<Double> phfitter;
      Polynomial<AutoDiff<Double> > line(1);
      phfitter.setFunction(line);
      Vector<Bool> m(mask.column(ich));

      // Fit shallow and steep, and always prefer shallow
      
      // Assumed shallow solve:
      Vector<Double> solnA;
      solnA.assign(phfitter.fit(x.column(ich),y.column(ich),sig.column(ich),&m));

      // Assumed steep solve:
      Vector<Double> solnB;
      solnB.assign(phfitter.fit(y.column(ich),x.column(ich),sig.column(ich),&m));

      Double Xph(0.0);
      if (abs(solnA(1))<abs(solnB(1))) {
	Xph=atan(solnA(1));
      }
      else {
	Xph=atan(1.0/solnB(1));
      }

      Cph(ich)=currAmb*Complex(DComplex(cos(Xph),sin(Xph)));

      // Watch for and remove ambiguity changes which can
      //  occur near Xph~= +/-90 deg (the atan above can jump)
      //  (NB: this does _not_ resolve the absolute amb; it merely makes
      //   it consistent within the spw over channels)
      if (ich>0) {
	// If Xph changes by more than pi/2, probably a ambig jump...
	Float dang=abs(arg(Cph(ich)/Cph(ich-1)));
	if (dang > (C::pi/2.)) {
	  Cph(ich)*=-1.0;   // fix this one
	  currAmb*=-1.0;    // reverse currAmb, so curr amb is carried forward
	  //cout << "  Found XY phase ambiguity jump at chan=" << ich << " in spw=" << currSpw() << endl;
	}
      }

      //      cout << " (" << currAmb << ")";
      //      cout << endl;


      // Set all antennas with this X-Y phase (as a complex number)
      solveCPar()(Slice(0,1,1),Slice(ich,1,1),Slice())=Cph(ich);
      solveParOK()(Slice(),Slice(ich,1,1),Slice())=true;
    }
    else {
      solveCPar()(Slice(0,1,1),Slice(ich,1,1),Slice())=Complex(1.0);
      solveParOK()(Slice(),Slice(ich,1,1),Slice())=false;
    }
  } // ichan 

  // Calculate correlation with model data (real part only!), insist it is positive
  {


    //logSink() << "Attempting 180 deg ambiguity resolution by comparison with specified linear polarization model." << LogIO::POST;
      
    Vector<Double> wtf(nSDB,0.0); //,sigf(nSDB,0.0),xf(nSDB,0.0),yf(nSDB,0.0);
    //Vector<Bool> maskf(nSDB,false);
    Float wt0;
    Complex v,m;
    Double Cp(0.0), Cn(0.0);
    Double wtsum(0.0);

    for (Int isdb=0;isdb<nSDB;++isdb) {
      SolveDataBuffer& sdb(sdbs(isdb));

      for (Int irow=0;irow<sdb.nRows();++irow) {
	if (!sdb.flagRow()(irow) &&
	    sdb.antenna1()(irow)!=sdb.antenna2()(irow)) {
	  
	  for (Int ich=0;ich<nChan;++ich) {
	    
	    if (!sdb.flagCube()(1,ich,irow)) {
	      // Correct x-hands for xy-phase and add together
	      wt0=sdb.weightSpectrum()(1,ich,irow);
	      v=sdb.visCubeCorrected()(1,ich,irow)/Cph(ich);
	      m=sdb.visCubeModel()(1,ich,irow);
	      Cp+=(wt0*real(v)*real(m));
	      Cn+=(wt0*real(v)*real(m)*(-1.0));
	      wtsum+=Double(wt0);
	    }
	    if (!sdb.flagCube()(2,ich,irow)) {
	      // Correct x-hands for xy-phase and add together
	      wt0=sdb.weightSpectrum()(2,ich,irow);
	      v=sdb.visCubeCorrected()(2,ich,irow)/conj(Cph(ich));
	      m=sdb.visCubeModel()(2,ich,irow);
	      Cp+=(wt0*real(v)*real(m));
	      Cn+=(wt0*real(v)*real(m)*(-1.0));
	      wtsum+=Double(wt0);
	    }
	  }
	}
      }
    }


    //cout << "Cp,Cn = " << Cp << " " << Cn << endl;

    if ( Cn > Cp ) {
      logSink() << " NB: 180 deg ambiguity detected and corrected!" << LogIO::POST;
      Complex swap(-1.0,0.0);
      Cph*=swap;
      
      MaskedArray<Complex> sCP(solveCPar()(solveParOK()));
      sCP*=swap;

    }


  }


  if (ntrue(solveParOK())>0) {
    Float ang=arg(sum(solveCPar()(solveParOK()))/Float(ntrue(solveParOK())))*180.0/C::pi;

    logSink() << " Fld = " << msmc().fieldName(currField())
	      << ", Spw = " << thisSpw
	      << " (ich=" << nChan/2 << "/" << nChan << "): " //<< endl
	      << " CROSS-HAND PHASE = " << arg(Cph[nChan/2])*180.0/C::pi << " deg."
	      << " (Mean = " << ang << ")"
	      << LogIO::POST;
  }
  else
    logSink() << " Fld = " << msmc().fieldName(currField())
	      << ", Spw = " << thisSpw
	      << " (ich=" << nChan/2 << "/" << nChan << "): " << endl
	      << " CROSS-HAND PHASE was not determined (insufficient data)."
	      << LogIO::POST;

  if (report)
    cout << endl 
	 << "Spw = " << thisSpw
	 << " (ich=" << nChan/2 << "/" << nChan << "): " // << endl
	 << " X-Y phase = " << arg(Cph[nChan/2])*180.0/C::pi << " deg." << endl;
      


  // Now fit for the source polarization
  {

    Vector<Double> wtf(nSDB,0.0),sigf(nSDB,0.0),xf(nSDB,0.0),yf(nSDB,0.0);
    Vector<Bool> maskf(nSDB,false);
    Float wt0;
    Complex v;
    for (Int isdb=0;isdb<nSDB;++isdb) {
      SolveDataBuffer& sdb(sdbs(isdb));

      for (Int irow=0;irow<sdb.nRows();++irow) {
	if (!sdb.flagRow()(irow) &&
	    sdb.antenna1()(irow)!=sdb.antenna2()(irow)) {
	  
	  Float fpa(sdb.feedPa()(0));  // assumes same for all antennas!
	  
	  for (Int ich=0;ich<nChan;++ich) {
	    
	    if (!sdb.flagCube()(1,ich,irow)) {
	      // Correct x-hands for xy-phase and add together
	      wt0=sdb.weightSpectrum()(1,ich,irow);
	      v=sdb.visCubeCorrected()(1,ich,irow)/Cph(ich);
	      xf(isdb)+=Double(wt0*2.0*(fpa));
	      yf(isdb)+=Double(wt0*real(v));
	      wtf(isdb)+=Double(wt0);
	    }
	    if (!sdb.flagCube()(2,ich,irow)) {
	      // Correct x-hands for xy-phase and add together
	      wt0=sdb.weightSpectrum()(2,ich,irow);
	      v=sdb.visCubeCorrected()(2,ich,irow)/conj(Cph(ich));
	      xf(isdb)+=Double(wt0*2.0*(fpa));
	      yf(isdb)+=Double(wt0*real(v));
	      wtf(isdb)+=Double(wt0);
	    }
	  }
	}
      }
    }
    
    // Normalize data by accumulated weights
    for (Int isdb=0;isdb<nSDB;++isdb) {
      if (wtf(isdb)>0.0) {
	xf(isdb)/=wtf(isdb);
	yf(isdb)/=wtf(isdb);
	sigf(isdb)=sqrt(1.0/wtf(isdb));
	maskf(isdb)=true;
      }
      else
	sigf(isdb)=DBL_MAX;    // ~zero weight
    }
    
    // p0=Q, p1=U, p2 = real part of net instr pol offset
    //  x is TWICE the parallactic angle
    CompiledFunction<AutoDiff<Double> > fn;
    fn.setFunction("-p0*sin(x) + p1*cos(x) + p2");

    LinearFit<Double> fitter;
    fitter.setFunction(fn);
    
    Vector<Double> soln=fitter.fit(xf,yf,sigf,&maskf);

    srcPolPar().resize(2);
    srcPolPar()(0)=soln(0);
    srcPolPar()(1)=soln(1);
        
    QU_(0,thisSpw) = soln(0);
    QU_(1,thisSpw) = soln(1);

    Float &Q(QU_(0,thisSpw)), &U(QU_(1,thisSpw));


    logSink() << " Fractional Poln: "
	      << "Q = " << Q << ", "
	      << "U = " << U << "; "
	      << "P = " << sqrt(Q*Q+U*U) << ", "
	      << "X = " << atan2(U,Q)*90.0/C::pi << "deg."
	      << LogIO::POST;
    logSink() << "  Net (over baselines) instrumental polarization (real part): " 
	      << soln(2) << LogIO::POST;

  } // poln solve scope	

  }
  else if (sdbs.polBasis()==String("CIRC")) {

  logSink() << "Solving for Cross-hand Phase and calibrator linear polarization in the CIRCULAR basis in spw="<<thisSpw << LogIO::POST;

  // Report surviving data from extendCrossHandBaselineFlags
  logSink() << " " << extBLmessage << LogIO::POST;

  // Trap insufficient data case (circular; |P| and a complex offset from complex data, cf law of cosines for isoceles triangle)
  if (nGoodSDB<2) 
    throw(AipsError("For a viable solution, the Xfparang+QU solve requires at least TWO distinct unflagged data segments in time/parallactic angle in the CIRCULAR basis.  Cannot proceed."));

  Matrix<Complex> V(nSDB,nChan,0.0),M(nSDB,nChan,0.0);
  Matrix<Float> Wt(nSDB,nChan,0.0); // ,sig(nSDB,nChan,0.0);
  Matrix<Bool> mask(nSDB,nChan,false);

  mask.set(false);
  Complex v(0.0),m(0.0);
  Float wt0(0.0);
  
  for (Int isdb=0;isdb<nSDB;++isdb) {
    SolveDataBuffer& sdb(sdbs(isdb));

    for (Int irow=0;irow<sdb.nRows();++irow) {
      if (!sdb.flagRow()(irow) &&
	  sdb.antenna1()(irow)!=sdb.antenna2()(irow)) {
	
	for (Int ich=0;ich<nChan;++ich) {
	  if (!sdb.flagCube()(1,ich,irow)) {
	    wt0=sdb.weightSpectrum()(1,ich,irow);
	    v=sdb.visCubeCorrected()(1,ich,irow);
	    m=sdb.visCubeModel()(1,ich,irow);
	    V(isdb,ich)+=Complex(wt0*v);
	    M(isdb,ich)+=Complex(wt0*m);
	    Wt(isdb,ich)+=wt0;
	  }
	  if (!sdb.flagCube()(2,ich,irow)) {
	    wt0=sdb.weightSpectrum()(2,ich,irow);
	    v=conj(sdb.visCubeCorrected()(2,ich,irow));
	    m=conj(sdb.visCubeModel()(2,ich,irow));
	    V(isdb,ich)+=Complex(wt0*v);
	    M(isdb,ich)+=Complex(wt0*m);
	    Wt(isdb,ich)+=wt0;
	  }
	}
      }
    }
  }

  // Normalize data by accumulated weights
  for (Int isdb=0;isdb<nSDB;++isdb) {
    for (Int ich=0;ich<nChan;++ich) {
      if (Wt(isdb,ich)>0.0) {
	V(isdb,ich)/=Wt(isdb,ich);
	M(isdb,ich)/=Wt(isdb,ich);
	mask(isdb,ich)=true;
      }
    }
  }

  // Now solve for cross-hand phase in each channel
  Vector<Complex> Cph(nChan);
  Cph.set(Complex(1.0,0.0));
  for (Int ich=0;ich<nChan;++ich) {

    if (sum(Wt.column(ich))>0.0) {

      LSQFit fit(2,LSQComplex());
      Vector<Complex> ce(2);
      ce(0)=Complex(1.0);
      
      for (int isdb=0;isdb<nSDB;++isdb) {
	ce(1)=M(isdb,ich);
	fit.makeNorm(ce.data(),Wt(isdb,ich),V(isdb,ich),LSQFit::COMPLEX);
      }

      uInt rank;
      Bool ok = fit.invert(rank);
      DComplex sol[2];
      if (ok)
	fit.solve(sol);
      else
	throw(AipsError("Source polarization solution is singular; try solving for D-terms only."));

      //cout << "ich=" << ich << "  sol = " << sol[0] << "," << sol[1] << "   P=" << abs(sol[1]) << "   X=" << arg(sol[1])*180/C::pi << endl;

      Cph[ich]=sol[1]/abs(sol[1]);   // phase-only as complex number

      // Set all antennas with this X-Y phase (as a complex number)
      solveCPar()(Slice(0,1,1),Slice(ich,1,1),Slice())=Cph(ich);
      solveParOK()(Slice(),Slice(ich,1,1),Slice())=true;
    }
    else {
      // In sufficient data, phase=0.0, flagged
      solveCPar()(Slice(0,1,1),Slice(ich,1,1),Slice())=Complex(1.0);
      solveParOK()(Slice(),Slice(ich,1,1),Slice())=false;
    }
  }  // ICH

  if (ntrue(solveParOK())>0) {
    Float ang=arg(sum(solveCPar()(solveParOK()))/Float(ntrue(solveParOK())))*180.0/C::pi;

    logSink() << " Fld = " << msmc().fieldName(currField())
	      << ", Spw = " << thisSpw
	      << " (ich=" << nChan/2 << "/" << nChan << "): "  // << endl
	      << " CROSS-HAND PHASE = " << arg(Cph[nChan/2])*180.0/C::pi << " deg."
	      << " (Mean = " << ang << ")"
	      << LogIO::POST;
  }
  else
    logSink() << " Fld = " << msmc().fieldName(currField())
	      << ", Spw = " << thisSpw
	      << " (ich=" << nChan/2 << "/" << nChan << "): " << endl
	      << " CROSS-HAND PHASE was not determined (insufficient data)."
	      << LogIO::POST;

  if (false)
    cout << endl 
	 << "Spw = " << thisSpw
	 << " (ich=" << nChan/2 << "/" << nChan << "): " // << endl
	 << " X-Y phase = " << arg(Cph[nChan/2])*180.0/C::pi << " deg." << endl;
      


  // Now solve for QU (chan-aved)
  {
    LSQFit fit(2,LSQComplex());
    Vector<Complex> ce(2);
    ce(0)=Complex(1.0);
      
    Vector<Complex> V2(nSDB,Complex(0.0)), M2(nSDB,Complex(0.0));
    Vector<Float> Wt2(nSDB,0.0f);
    for (int isdb=0;isdb<nSDB;++isdb) {
      // Accumulate along channel axis
      for (Int ich=0;ich<nChan;++ich) {
	if (Wt(isdb,ich)>0.0f) {
	  V2[isdb]+=(Wt(isdb,ich)*V(isdb,ich)/Cph[ich]);   // include cross-hand phase correction
	  M2[isdb]+=(Wt(isdb,ich)*M(isdb,ich)/abs(M(isdb,ich)));   // Divide by user-supplied |P|
	  Wt2[isdb]+=Wt(isdb,ich);
	}
      }
      // finalize averages
      if (Wt2[isdb]>0.0) {
	V2[isdb]/=Wt2[isdb];
	M2[isdb]/=Wt2[isdb];
      }
      
      ce(1)=M2(isdb);
      fit.makeNorm(ce.data(),Wt2(isdb),V2(isdb),LSQFit::COMPLEX);

    }

    uInt rank;
    Bool ok = fit.invert(rank);
    DComplex sol[2];
    if (ok)
      fit.solve(sol);
    else
      throw(AipsError("Source polarization solution is singular; try solving for D-terms only."));

    //cout << "sol = " << sol[0] << "," << sol[1] << "   P=" << abs(sol[1]) << "   X=" << arg(sol[1])*90.0/C::pi << endl;

    srcPolPar().resize(2);
    srcPolPar()(0)=real(sol[1]);
    srcPolPar()(1)=imag(sol[1]);
        
    QU_(0,thisSpw) = real(sol[1]);
    QU_(1,thisSpw) = imag(sol[1]);

    Float &Q(QU_(0,thisSpw)), &U(QU_(1,thisSpw));

    logSink() << " Fractional Poln: "
	      << "Q = " << Q << ", "
	      << "U = " << U << "; "
	      << "P = " << sqrt(Q*Q+U*U) << ", "
	      << "X = " << atan2(U,Q)*90.0/C::pi << "deg."
	      << LogIO::POST;
    logSink() << " Net (over baselines) instrumental polarization: " 
	      << abs(sol[0]) << LogIO::POST;


  } // Circ QU solve

  } // basis clauses
  else {

    throw(AipsError("Cannot solve for cross-hand phase, don't know basis"));
  }

  // Add this QU result to the QURec_

  Vector<Float> fStokes(4,0.0f);  // Fractional Stokes vector
  fStokes(0)=1.0;
  fStokes(1)=QU_(0,thisSpw);  // Q
  fStokes(2)=QU_(1,thisSpw);  // U

  ostringstream os;
  os << "Spw" << thisSpw;
  String spwName(os);
  String fldName(msmc().fieldName(currField()));
  Record fld;
  if (QURec_.isDefined(fldName))
    fld=QURec_.asRecord(fldName);
  fld.define(spwName,fStokes);
  
  // Add average
  Int nspw=fld.nfields();  // the number of spws recorded in the record so far
  Vector<Float> QUave(4,0.0f);
  Int nave(0);
  for (Int ispw=0;ispw<nspw;++ispw) {
    String fieldname=fld.name(ispw);
    if (fieldname!="SpwAve") {
      QUave+=fld.asArrayFloat(ispw);
      ++nave;
    }
  }
  QUave/=Float(nave);
  fld.define("SpwAve",QUave);

  // Replace this fld's record in QURec_
  QURec_.defineRecord(fldName,fld);

}

void XparangJones::globalPostSolveTinker() {

    // Add QU info the caltable keywords
    TableRecord& tr(ct_->rwKeywordSet());
    Record qu;
    qu.define("QU",QU_);
    tr.defineRecord("QU",qu);

}

Record XparangJones::solveActionRec() {

  // Add QU info to QURec_, so Calibrater can extract and return
  Record sAR;
  sAR=SolvableVisCal::solveActionRec();
  sAR.defineRecord("fStokes",QURec_);

  return sAR;

}

// **********************************************************
//  XfparangJones Implementations
//

// Constructor
XfparangJones::XfparangJones(VisSet& vs)  :
  VisCal(vs),             // virtual base
  VisMueller(vs),         // virtual base
  XparangJones(vs)        // immediate parent
{
  if (prtlev()>2) cout << "Xfparangf::Xfparang(vs)" << endl;
}

XfparangJones::XfparangJones(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),             // virtual base
  VisMueller(msname,MSnAnt,MSnSpw),         // virtual base
  XparangJones(msname,MSnAnt,MSnSpw)        // immediate parent
{
  if (prtlev()>2) cout << "Xfparang::Xfparang(msname,MSnAnt,MSnSpw)" << endl;
}

XfparangJones::XfparangJones(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),             // virtual base
  VisMueller(msmc),         // virtual base
  XparangJones(msmc)        // immediate parent
{
  if (prtlev()>2) cout << "Xfparang::Xfparang(msmc)" << endl;
}

XfparangJones::XfparangJones(const Int& nAnt) :
  VisCal(nAnt), 
  VisMueller(nAnt),
  XparangJones(nAnt)
{
  if (prtlev()>2) cout << "Xfparang::Xfparang(nAnt)" << endl;
}

XfparangJones::~XfparangJones() {
  if (prtlev()>2) cout << "Xfparang::~Xfparang()" << endl;
}


// **********************************************************
//  PosAngJones Implementations
//

PosAngJones::PosAngJones(VisSet& vs) :
  VisCal(vs),             // virtual base
  VisMueller(vs),         // virtual base
  XJones(vs),             // immediate parent
  jonestype_(Jones::Diagonal)
{
  if (prtlev()>2) cout << "PosAng::PosAng(vs)" << endl;
}

PosAngJones::PosAngJones(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),             // virtual base
  VisMueller(msname,MSnAnt,MSnSpw),         // virtual base
  XJones(msname,MSnAnt,MSnSpw),             // immediate parent
  jonestype_(Jones::Diagonal)
{
  if (prtlev()>2) cout << "PosAng::PosAng(msname,MSnAnt,MSnSpw)" << endl;
}

PosAngJones::PosAngJones(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),             // virtual base
  VisMueller(msmc),         // virtual base
  XJones(msmc),             // immediate parent
  jonestype_(Jones::Diagonal)
{
  if (prtlev()>2) cout << "PosAng::PosAng(msmc)" << endl;
}

PosAngJones::PosAngJones(const Int& nAnt) :
  VisCal(nAnt), 
  VisMueller(nAnt),
  XJones(nAnt),
  jonestype_(Jones::Diagonal)
{
  if (prtlev()>2) cout << "PosAng::PosAng(nAnt)" << endl;
}

PosAngJones::~PosAngJones() {
  if (prtlev()>2) cout << "PosAng::~PosAng()" << endl;
}

void PosAngJones::setApply(const Record& apply) {

  SolvableVisCal::setApply(apply);

  // Force calwt to false 
  calWt()=false;

}

void PosAngJones::setSolve(const Record& solvepar) {

  SolvableVisCal::setSolve(solvepar);

  // Force calwt to false 
  calWt()=false;

  // For X insist preavg is meaningful (5 minutes or user-supplied)
  if (preavg()<0.0)
    preavg()=30.0;

  // Force refant to none (==-1), because it is meaningless to
  //  apply a refant to an X solution
  if (refant()>-1) {
    logSink() << ".   (Ignoring specified refant for " 
	      << typeName() << " solve.)"
	      << LogIO::POST;
    refantlist().resize(1);
    refantlist()(0)=-1;
  }

}

// PJones needs to know pol basis and feed pa
void PosAngJones::syncMeta(const VisBuffer& vb) {

  // Call parent (sets currTime())
  VisJones::syncMeta(vb);

  // Basis
  if (vb.corrType()(0)==5)         // Circulars
    jonestype_=Jones::Diagonal;
  else if (vb.corrType()(0)==9)    // Linears
    jonestype_=Jones::General;

}

// PJones needs to know pol basis and feed pa
void PosAngJones::syncMeta2(const vi::VisBuffer2& vb) {

  // Call parent (sets currTime())
  VisJones::syncMeta2(vb);

  // Basis
  if (vb.correlationTypes()(0)==5)         // Circulars
    jonestype_=Jones::Diagonal;
  else if (vb.correlationTypes()(0)==9)    // Linears
    jonestype_=Jones::General;

}

void PosAngJones::calcAllJones() {

  //if (prtlev()>6) cout << "       PosAng::calcAllJones()" << endl;

  // Should handle OK flags in this method, and only
  //  do Jones calc if OK

  Vector<Complex> oneJones;
  Vector<Bool> oneJOK;
  Vector<Float> onePar;
  Vector<Bool> onePOK;

  ArrayIterator<Complex> Jiter(currJElem(),1);
  ArrayIterator<Bool>    JOKiter(currJElemOK(),1);
  ArrayIterator<Float>   Piter(currRPar(),1);
  ArrayIterator<Bool>    POKiter(currParOK(),1);

  for (Int iant=0; iant<nAnt(); iant++) {

    for (Int ich=0; ich<nChanMat(); ich++) {
      
      oneJones.reference(Jiter.array());
      oneJOK.reference(JOKiter.array());
      onePar.reference(Piter.array());
      onePOK.reference(POKiter.array());

      // Calculate the Jones matrix
      calcOneJonesRPar(oneJones,oneJOK,onePar,onePOK);

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

// Calculate a single Jones matrix by some means from parameters
void PosAngJones::calcOneJonesRPar(Vector<Complex>& mat, Vector<Bool>& mOk,
				   const Vector<Float>& par, const Vector<Bool>& pOk ) {

  if (prtlev()>10) cout << "       PosAng::calcOneJones()" << endl;

  if (pOk(0)) {

    switch (jonesType()) {
      // Circular version:
    case Jones::Diagonal: {
      mat(0)=Complex(cos(par(0)), sin(par(0)));
      mat(1)=conj(mat(0));       
      mOk=true;
      break;
    }
      // Linear version:
    case Jones::General: {
      const Float a=-par(0);
      mat(0)=mat(3)=cos(a);
      mat(1)=sin(a);
      mat(2)=-mat(1);
      mOk=true;
      break;
    }
    default:
      throw(AipsError("PosAngJones doesn't know if it is Diag (Circ) or General (Lin)"));
      break;

    }

  }
}


void PosAngJones::solveOne(SDBList& sdbs) {

  // This just a simple average of the cross-hand
  //  visbilities...

  Int nSDB=sdbs.nSDB();

  // We are actually solving for all channels simultaneously
  solveRPar().reference(solveAllRPar());
  solveParOK().reference(solveAllParOK());
  solveParErr().reference(solveAllParErr());
  solveParSNR().reference(solveAllParSNR());
  
  // Fill solveCPar() with 1, nominally, and flagged
  solveRPar()=0.0;
  solveParOK()=false;

  Int nChan=sdbs.nChannels();

  String polBasis=sdbs.polBasis();

  Complex d,md;
  Float wt;
  Vector<DComplex> RL(nChan,0.0);
  Vector<Double> sumwt(nChan,0.0);
  for (Int isdb=0;isdb<nSDB;++isdb) {
    SolveDataBuffer& sdb(sdbs(isdb));
    for (Int irow=0;irow<sdb.nRows();++irow) {
      if (!sdb.flagRow()(irow) &&
	  sdb.antenna1()(irow)!=sdb.antenna2()(irow)) {
	
	for (Int ich=0;ich<nChan;++ich) {

	  if (polBasis=="CIRC") {
	    if (!sdb.flagCube()(1,ich,irow) &&
		!sdb.flagCube()(2,ich,irow)) {
	    
	      // A common weight for both crosshands
	      // TBD: we should probably consider this carefully...
	      //  (also in D::guessPar...)
	      wt=Double(sdb.weightSpectrum()(1,ich,irow)+
			sdb.weightSpectrum()(2,ich,irow))/2.0;
	      
	      if (wt>0.0) {
		// Cross-hands only
		for (Int icorr=1;icorr<3;++icorr) {
		  d=sdb.visCubeCorrected()(icorr,ich,irow);
		  md=sdb.visCubeModel()(icorr,ich,irow);
		  
		  if (abs(d)>0.0 && abs(md)>0.0) {
		    
		    if (icorr==1) 
		      RL(ich)+=DComplex(Complex(wt)*d/md);
		    else
		      RL(ich)+=conj(DComplex(Complex(wt)*d/md));
		    
		    sumwt(ich)+=Double(wt);
		    
		  } // abs(d)>0
		} // icorr
	      } // wt>0
	    } // !flag
	  } // CIRC
	  else if (polBasis=="LIN") {
	    // Need all 4 correlations
	    if (!sdb.flagCube()(0,ich,irow) &&
		!sdb.flagCube()(1,ich,irow) &&
		!sdb.flagCube()(2,ich,irow) &&
		!sdb.flagCube()(3,ich,irow)) {
	    
	      // A common weight  (correct?)
	      wt=Double(sdb.weightSpectrum()(0,ich,irow)+
			sdb.weightSpectrum()(1,ich,irow)+
			sdb.weightSpectrum()(2,ich,irow)+
			sdb.weightSpectrum()(3,ich,irow))/4.0;
	      
	      if (wt>0.0) {

		Complex oneI(0.0,1.0);
		const Cube<Complex>& vCC(sdb.visCubeCorrected());
		Complex Q( (vCC(0,ich,irow)-vCC(3,ich,irow))/2.0 );
		Complex U( (vCC(1,ich,irow)+vCC(2,ich,irow))/2.0 );
		Complex d(Q+oneI*U);
		
		const Cube<Complex> vCM(sdb.visCubeModel());
		Complex Qm( (vCM(0,ich,irow)-vCM(3,ich,irow))/2.0 );
		Complex Um( (vCM(1,ich,irow)+vCM(2,ich,irow))/2.0 );
		Complex md(Qm+oneI*Um);

		if (abs(d)>0.0 && abs(md)>0.0) {
		  RL(ich)+=DComplex(Complex(wt)*d/md);
		  sumwt(ich)+=Double(wt);
		} // abs(d)>0

	      } // wt>0
	    } // !flag
	  } // LIN

	} // ich
      } // !flagRow
    } // row
  } // isdb

  //  cout << "spw = " << currSpw() << endl;

  // Record results
  for (Int ich=0;ich<nChan;++ich) {
  
    // Normalize to unit amplitude
    //  (note that the phase result is insensitive to sumwt)
    Double amp=abs(RL(ich));
    // For now, all antennas get the same solution
    IPosition blc(3,0,ich,0);
    IPosition trc(3,0,ich,nElem()-1);
    if (sumwt(ich)>0 && amp>0.0) {
      RL(ich)/=amp;
      solveRPar()(blc,trc)=arg(RL(ich))/2.0;
      solveParOK()(blc,trc)=true;
    }
    else
      RL(ich)=0.0;
  }

  if (ntrue(solveParOK())>0) {
    Float ang=arg(sum(RL))*90.0/C::pi;
    logSink() << "Mean POSITION ANGLE OFFSET solution for " 
	      << msmc().fieldName(currField())
	      << " (spw = " << currSpw() << ") = "
	      << ang
	      << " deg."
	      << LogIO::POST;
  }
  else
    logSink() << "POSITION ANGLE OFFSET solution for " 
	      << msmc().fieldName(currField())
	      << " (spw = " << currSpw() << ") "
	      << " was not determined (insufficient data)."
	      << LogIO::POST;
  
}




// **********************************************************
//  GlinXphJones Implementations
//

GlinXphJones::GlinXphJones(VisSet& vs) :
  VisCal(vs),             // virtual base
  VisMueller(vs),         // virtual base
  GJones(vs),             // immediate parent
  QU_()
{
  if (prtlev()>2) cout << "GlinXph::GlinXph(vs)" << endl;
}

GlinXphJones::GlinXphJones(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),             // virtual base
  VisMueller(msname,MSnAnt,MSnSpw),         // virtual base
  GJones(msname,MSnAnt,MSnSpw),             // immediate parent
  QU_()
{
  if (prtlev()>2) cout << "GlinXph::GlinXph(msname,MSnAnt,MSnSpw)" << endl;
}

GlinXphJones::GlinXphJones(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),             // virtual base
  VisMueller(msmc),         // virtual base
  GJones(msmc),             // immediate parent
  QU_()
{
  if (prtlev()>2) cout << "GlinXph::GlinXph(msmc)" << endl;
}

GlinXphJones::GlinXphJones(const Int& nAnt) :
  VisCal(nAnt), 
  VisMueller(nAnt),
  GJones(nAnt),
  QU_()
{
  if (prtlev()>2) cout << "GlinXph::GlinXph(nAnt)" << endl;
}

GlinXphJones::~GlinXphJones() {
  if (prtlev()>2) cout << "GlinXph::~GlinXph()" << endl;
}

void GlinXphJones::setApply(const Record& apply) {

  GJones::setApply(apply);

  // Force calwt to false 
  calWt()=false;

}



void GlinXphJones::selfGatherAndSolve(VisSet& vs, VisEquation& ve) {

  if (prtlev()>4) cout << "   GlnXph::selfGatherAndSolve(ve)" << endl;

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

    // Establish meta-data for this interval
    //  (some of this may be used _during_ solve)
    //  (this sets currSpw() in the SVC)
    Bool vbOk=syncSolveMeta(svb,-1);

    if (vbOk && svb.nRow()>0) {

      // solve for the X-Y phase term in the current VB
      solveOneVB(svb);

      nGood++;
    }

    keepNCT();
    
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

    // Do global post-solve tinkering (e.g., phase-only, normalization, etc.)
    globalPostSolveTinker();

    // write the table
    storeNCT();
  }

}

// Handle trivial vbga
void GlinXphJones::selfSolveOne(VisBuffGroupAcc& vbga) {

  // Expecting only on VB in the vbga (with many times)
  if (vbga.nBuf()!=1)
    throw(AipsError("GlinXphJones can't process multi-vb vbga."));

  // Call single-VB specialized solver with the one vb
  this->solveOneVB(vbga(0));

}

// SDBList (VI2) version
void GlinXphJones::selfSolveOne(SDBList& sdbs) {

  // Expecting multiple SDBs (esp. in time)
  if (sdbs.nSDB()==1)
    throw(AipsError("GlinXphJones needs multiple SDBs"));

  // Call single-VB specialized solver with the one vb
  this->solveOne(sdbs);

}

// Solve for the X-Y phase from the cross-hand's slope in R/I
void GlinXphJones::solveOneVB(const VisBuffer& vb) {

  // ensure
  if (QU_.shape()!=IPosition(2,2,nSpw())) {
    QU_.resize(2,nSpw());
    QU_.set(0.0);
  }

  Int thisSpw=spwMap()(vb.spectralWindow());
  
  // We are actually solving for all channels simultaneously
  solveCPar().reference(solveAllCPar());
  solveParOK().reference(solveAllParOK());
  solveParErr().reference(solveAllParErr());
  solveParSNR().reference(solveAllParSNR());
  
  // Fill solveCPar() with 1, nominally, and flagged
  solveCPar()=Complex(1.0);
  solveParOK()=false;

  Int nChan=vb.nChannel();
  //  if (nChan>1)
  //    throw(AipsError("X-Y phase solution NYI for channelized data"));

  // Find number of timestamps in the VB
  Vector<uInt> ord;
  Int nTime=genSort(ord,vb.time(),Sort::Ascending,Sort::NoDuplicates);

  Matrix<Double> x(nTime,nChan,0.0),y(nTime,nChan,0.0),wt(nTime,nChan,0.0),sig(nTime,nChan,0.0);
  Matrix<Bool> mask(nTime,nChan,false);

  mask.set(false);
  Complex v(0.0);
  Float wt0(0.0);
  Int iTime(-1);
  Double currtime(-1.0);
  for (Int irow=0;irow<vb.nRow();++irow) {
    if (!vb.flagRow()(irow) &&
	vb.antenna1()(irow)!=vb.antenna2()(irow)) {

      // Advance time index when we see a new time
      if (vb.time()(irow)!=currtime) {
	++iTime;
	currtime=vb.time()(irow); // remember the new current time
      }

      // Weights not yet chan-dep
      wt0=(vb.weightMat()(1,irow)+vb.weightMat()(2,irow));
      if (wt0>0.0) {

	for (Int ich=0;ich<nChan;++ich) {
	  if (!vb.flag()(ich,irow)) {
	    v=vb.visCube()(1,ich,irow)+conj(vb.visCube()(2,ich,irow));
	    x(iTime,ich)+=Double(wt0*real(v));
	    y(iTime,ich)+=Double(wt0*imag(v));
	    wt(iTime,ich)+=Double(wt0);
	  }
	}
      }
    }
  }

  // Normalize data by accumulated weights
  for (Int itime=0;itime<nTime;++itime) {
    for (Int ich=0;ich<nChan;++ich) {
      if (wt(itime,ich)>0.0) {
	x(itime,ich)/=wt(itime,ich);
	y(itime,ich)/=wt(itime,ich);
	sig(itime,ich)=sqrt(1.0/wt(itime,ich));
	mask(itime,ich)=true;
      }
      else
	sig(itime,ich)=DBL_MAX;    // ~zero weight
    }
  }

  // Solve for each channel
  Vector<Complex> Cph(nChan);
  Cph.set(Complex(1.0,0.0));
  Float currAmb(1.0);
  Bool report(false);
  for (Int ich=0;ich<nChan;++ich) {

    if (sum(wt.column(ich))>0.0) {
      report=true;
      LinearFit<Double> phfitter;
      Polynomial<AutoDiff<Double> > line(1);
      phfitter.setFunction(line);
      Vector<Bool> m(mask.column(ich));

      // Fit shallow and steep, and always prefer shallow
      
      // Assumed shallow solve:
      Vector<Double> solnA;
      solnA.assign(phfitter.fit(x.column(ich),y.column(ich),sig.column(ich),&m));

      // Assumed steep solve:
      Vector<Double> solnB;
      solnB.assign(phfitter.fit(y.column(ich),x.column(ich),sig.column(ich),&m));

      Double Xph(0.0);
      if (abs(solnA(1))<abs(solnB(1))) {
	Xph=atan(solnA(1));
      }
      else {
	Xph=atan(1.0/solnB(1));
      }

      Cph(ich)=currAmb*Complex(DComplex(cos(Xph),sin(Xph)));

      // Watch for and remove ambiguity changes which can
      //  occur near Xph~= +/-90 deg (the atan above can jump)
      //  (NB: this does _not_ resolve the amb; it merely makes
      //   it consistent within the spw)
      if (ich>0) {
	// If Xph changes by more than pi/2, probably a ambig jump...
	Float dang=abs(arg(Cph(ich)/Cph(ich-1)));
	if (dang > (C::pi/2.)) {
	  Cph(ich)*=-1.0;   // fix this one
	  currAmb*=-1.0;    // reverse currAmb, so curr amb is carried forward
	  //	  cout << "  Found XY phase ambiguity jump at chan=" << ich << " in spw=" << currSpw();
	}
      }

      //      cout << " (" << currAmb << ")";
      //      cout << endl;


      // Set all antennas with this X-Y phase (as a complex number)
      solveCPar()(Slice(0,1,1),Slice(ich,1,1),Slice())=Cph(ich);
      solveParOK()(Slice(),Slice(ich,1,1),Slice())=true;
    }
    else {
      solveCPar()(Slice(0,1,1),Slice(ich,1,1),Slice())=Complex(1.0);
      solveParOK()(Slice(),Slice(ich,1,1),Slice())=false;
    }
  }

  if (report)
    cout << endl 
	 << "Spw = " << thisSpw
	 << " (ich=" << nChan/2 << "/" << nChan << "): " << endl
	 << " X-Y phase = " << arg(Cph[nChan/2])*180.0/C::pi << " deg." << endl;
      

  // Now fit for the source polarization
  {

    Vector<Double> wtf(nTime,0.0),sigf(nTime,0.0),xf(nTime,0.0),yf(nTime,0.0);
    Vector<Bool> maskf(nTime,false);
    Float wt0;
    Complex v;
    Double currtime(-1.0);
    Int iTime(-1);
    for (Int irow=0;irow<vb.nRow();++irow) {
      if (!vb.flagRow()(irow) &&
	  vb.antenna1()(irow)!=vb.antenna2()(irow)) {
	
	if (vb.time()(irow)!=currtime) {
	  // Advance time index when we see a new time
	  ++iTime;
	  currtime=vb.time()(irow); // remember the new current time
	}

	// Weights not yet chan-dep
	wt0=(vb.weightMat()(1,irow)+vb.weightMat()(2,irow));
	if (wt0>0.0) {
	  for (Int ich=0;ich<nChan;++ich) {
	    
	    if (!vb.flag()(ich,irow)) {
	      // Correct x-hands for xy-phase and add together
	      v=vb.visCube()(1,ich,irow)/Cph(ich)+vb.visCube()(2,ich,irow)/conj(Cph(ich));
	      xf(iTime)+=Double(wt0*2.0*(vb.feed_pa(vb.time()(irow))(0)));
	      yf(iTime)+=Double(wt0*real(v)/2.0);
	      wtf(iTime)+=Double(wt0);
	    }
	  }
	}
      }
    }
    
    // Normalize data by accumulated weights
    for (Int itime=0;itime<nTime;++itime) {
      if (wtf(itime)>0.0) {
	xf(itime)/=wtf(itime);
	yf(itime)/=wtf(itime);
	sigf(itime)=sqrt(1.0/wtf(itime));
	maskf(itime)=true;
      }
      else
	sigf(itime)=DBL_MAX;    // ~zero weight
    }
    
    // p0=Q, p1=U, p2 = real part of net instr pol offset
    //  x is TWICE the parallactic angle
    CompiledFunction<AutoDiff<Double> > fn;
    fn.setFunction("-p0*sin(x) + p1*cos(x) + p2");

    LinearFit<Double> fitter;
    fitter.setFunction(fn);
    
    Vector<Double> soln=fitter.fit(xf,yf,sigf,&maskf);
    
    QU_(0,thisSpw) = soln(0);
    QU_(1,thisSpw) = soln(1);

    cout << " Fractional Poln: "
	 << "Q = " << QU_(0,thisSpw) << ", "
	 << "U = " << QU_(1,thisSpw) << "; "
	 << "P = " << sqrt(soln(0)*soln(0)+soln(1)*soln(1)) << ", "
	 << "X = " << atan2(soln(1),soln(0))*90.0/C::pi << "deg."
	 << endl;
    cout << " Net (over baselines) instrumental polarization: " 
	 << soln(2) << endl;

  }	

}

// Solve for the X-Y phase from the cross-hand's slope in R/I
void GlinXphJones::solveOne(SDBList& sdbs) {

  // ensure
  if (QU_.shape()!=IPosition(2,2,nSpw())) {
    QU_.resize(2,nSpw());
    QU_.set(0.0);
  }

  Int thisSpw=sdbs.aggregateSpw();
  
  // We are actually solving for all channels simultaneously
  solveCPar().reference(solveAllCPar());
  solveParOK().reference(solveAllParOK());
  solveParErr().reference(solveAllParErr());
  solveParSNR().reference(solveAllParSNR());
  
  // Fill solveCPar() with 1, nominally, and flagged
  solveCPar()=Complex(1.0);
  solveParOK()=false;

  Int nChan=sdbs.nChannels();

  // Number of datapoints in fit is the number of SDBs
  Int nSDB=sdbs.nSDB();

  Matrix<Double> x(nSDB,nChan,0.0),y(nSDB,nChan,0.0),wt(nSDB,nChan,0.0),sig(nSDB,nChan,0.0);
  Matrix<Bool> mask(nSDB,nChan,false);

  mask.set(false);
  Complex v(0.0);
  Float wt0(0.0);
  
  for (Int isdb=0;isdb<nSDB;++isdb) {
    SolveDataBuffer& sdb(sdbs(isdb));

    for (Int irow=0;irow<sdb.nRows();++irow) {
      if (!sdb.flagRow()(irow) &&
	  sdb.antenna1()(irow)!=sdb.antenna2()(irow)) {
	
	for (Int ich=0;ich<nChan;++ich) {
	  if (!sdb.flagCube()(1,ich,irow)) {
	    wt0=sdb.weightSpectrum()(1,ich,irow);
	    v=sdb.visCubeCorrected()(1,ich,irow);
	    x(isdb,ich)+=Double(wt0*real(v));
	    y(isdb,ich)+=Double(wt0*imag(v));
	    wt(isdb,ich)+=Double(wt0);
	  }
	  if (!sdb.flagCube()(2,ich,irow)) {
	    wt0=sdb.weightSpectrum()(2,ich,irow);
	    v=conj(sdb.visCubeCorrected()(2,ich,irow));
	    x(isdb,ich)+=Double(wt0*real(v));
	    y(isdb,ich)+=Double(wt0*imag(v));
	    wt(isdb,ich)+=Double(wt0);
	  }
	}
      }
    }
  }

  // Normalize data by accumulated weights
  for (Int isdb=0;isdb<nSDB;++isdb) {
    for (Int ich=0;ich<nChan;++ich) {
      if (wt(isdb,ich)>0.0) {
	x(isdb,ich)/=wt(isdb,ich);
	y(isdb,ich)/=wt(isdb,ich);
	sig(isdb,ich)=sqrt(1.0/wt(isdb,ich));
	mask(isdb,ich)=true;
      }
      else
	sig(isdb,ich)=DBL_MAX;    // ~zero weight
    }
  }

  // Solve for each channel
  Vector<Complex> Cph(nChan);
  Cph.set(Complex(1.0,0.0));
  Float currAmb(1.0);
  Bool report(false);
  for (Int ich=0;ich<nChan;++ich) {

    if (sum(wt.column(ich))>0.0) {
      report=true;
      LinearFit<Double> phfitter;
      Polynomial<AutoDiff<Double> > line(1);
      phfitter.setFunction(line);
      Vector<Bool> m(mask.column(ich));

      // Fit shallow and steep, and always prefer shallow
      
      // Assumed shallow solve:
      Vector<Double> solnA;
      solnA.assign(phfitter.fit(x.column(ich),y.column(ich),sig.column(ich),&m));

      // Assumed steep solve:
      Vector<Double> solnB;
      solnB.assign(phfitter.fit(y.column(ich),x.column(ich),sig.column(ich),&m));

      Double Xph(0.0);
      if (abs(solnA(1))<abs(solnB(1))) {
	Xph=atan(solnA(1));
      }
      else {
	Xph=atan(1.0/solnB(1));
      }

      Cph(ich)=currAmb*Complex(DComplex(cos(Xph),sin(Xph)));

      // Watch for and remove ambiguity changes which can
      //  occur near Xph~= +/-90 deg (the atan above can jump)
      //  (NB: this does _not_ resolve the amb; it merely makes
      //   it consistent within the spw)
      if (ich>0) {
	// If Xph changes by more than pi/2, probably a ambig jump...
	Float dang=abs(arg(Cph(ich)/Cph(ich-1)));
	if (dang > (C::pi/2.)) {
	  Cph(ich)*=-1.0;   // fix this one
	  currAmb*=-1.0;    // reverse currAmb, so curr amb is carried forward
	  //	  cout << "  Found XY phase ambiguity jump at chan=" << ich << " in spw=" << currSpw();
	}
      }

      //      cout << " (" << currAmb << ")";
      //      cout << endl;


      // Set all antennas with this X-Y phase (as a complex number)
      solveCPar()(Slice(0,1,1),Slice(ich,1,1),Slice())=Cph(ich);
      solveParOK()(Slice(),Slice(ich,1,1),Slice())=true;
    }
    else {
      solveCPar()(Slice(0,1,1),Slice(ich,1,1),Slice())=Complex(1.0);
      solveParOK()(Slice(),Slice(ich,1,1),Slice())=false;
    }
  }

  if (report)
    cout << endl 
	 << "Spw = " << thisSpw
	 << " (ich=" << nChan/2 << "/" << nChan << "): " << endl
	 << " X-Y phase = " << arg(Cph[nChan/2])*180.0/C::pi << " deg." << endl;
      

  // Now fit for the source polarization
  {

    Vector<Double> wtf(nSDB,0.0),sigf(nSDB,0.0),xf(nSDB,0.0),yf(nSDB,0.0);
    Vector<Bool> maskf(nSDB,false);
    Float wt0;
    Complex v;
    for (Int isdb=0;isdb<nSDB;++isdb) {
      SolveDataBuffer& sdb(sdbs(isdb));

      for (Int irow=0;irow<sdb.nRows();++irow) {
	if (!sdb.flagRow()(irow) &&
	    sdb.antenna1()(irow)!=sdb.antenna2()(irow)) {
	  
	  Float fpa(sdb.feedPa()(0));  // assumes same for all antennas!
	  
	  for (Int ich=0;ich<nChan;++ich) {
	    
	    if (!sdb.flagCube()(1,ich,irow)) {
	      // Correct x-hands for xy-phase and add together
	      wt0=sdb.weightSpectrum()(1,ich,irow);
	      v=sdb.visCubeCorrected()(1,ich,irow)/Cph(ich);
	      xf(isdb)+=Double(wt0*2.0*(fpa));
	      yf(isdb)+=Double(wt0*real(v));
	      wtf(isdb)+=Double(wt0);
	    }
	    if (!sdb.flagCube()(2,ich,irow)) {
	      // Correct x-hands for xy-phase and add together
	      wt0=sdb.weightSpectrum()(2,ich,irow);
	      v=sdb.visCubeCorrected()(2,ich,irow)/conj(Cph(ich));
	      xf(isdb)+=Double(wt0*2.0*(fpa));
	      yf(isdb)+=Double(wt0*real(v));
	      wtf(isdb)+=Double(wt0);
	    }
	  }
	}
      }
    }
    
    // Normalize data by accumulated weights
    for (Int isdb=0;isdb<nSDB;++isdb) {
      if (wtf(isdb)>0.0) {
	xf(isdb)/=wtf(isdb);
	yf(isdb)/=wtf(isdb);
	sigf(isdb)=sqrt(1.0/wtf(isdb));
	maskf(isdb)=true;
      }
      else
	sigf(isdb)=DBL_MAX;    // ~zero weight
    }
    
    // p0=Q, p1=U, p2 = real part of net instr pol offset
    //  x is TWICE the parallactic angle
    CompiledFunction<AutoDiff<Double> > fn;
    fn.setFunction("-p0*sin(x) + p1*cos(x) + p2");

    LinearFit<Double> fitter;
    fitter.setFunction(fn);
    
    Vector<Double> soln=fitter.fit(xf,yf,sigf,&maskf);

    srcPolPar().resize(2);
    srcPolPar()(0)=soln(0);
    srcPolPar()(1)=soln(1);
        
    QU_(0,thisSpw) = soln(0);
    QU_(1,thisSpw) = soln(1);

    cout << " Fractional Poln: "
	 << "Q = " << QU_(0,thisSpw) << ", "
	 << "U = " << QU_(1,thisSpw) << "; "
	 << "P = " << sqrt(soln(0)*soln(0)+soln(1)*soln(1)) << ", "
	 << "X = " << atan2(soln(1),soln(0))*90.0/C::pi << "deg."
	 << endl;
    cout << " Net (over baselines) instrumental polarization: " 
	 << soln(2) << endl;

  }	

}

void GlinXphJones::globalPostSolveTinker() {

    // Add QU info the the keywords
    TableRecord& tr(ct_->rwKeywordSet());
    Record qu;
    qu.define("QU",QU_);
    tr.defineRecord("QU",qu);

}


// **********************************************************
//  GlinXphfJones Implementations
//

// Constructor
GlinXphfJones::GlinXphfJones(VisSet& vs)  :
  VisCal(vs),             // virtual base
  VisMueller(vs),         // virtual base
  GlinXphJones(vs)        // immediate parent
{
  if (prtlev()>2) cout << "GlinXphf::GlinXphf(vs)" << endl;
}

GlinXphfJones::GlinXphfJones(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),             // virtual base
  VisMueller(msname,MSnAnt,MSnSpw),         // virtual base
  GlinXphJones(msname,MSnAnt,MSnSpw)        // immediate parent
{
  if (prtlev()>2) cout << "GlinXphf::GlinXphf(msname,MSnAnt,MSnSpw)" << endl;
}

GlinXphfJones::GlinXphfJones(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),             // virtual base
  VisMueller(msmc),         // virtual base
  GlinXphJones(msmc)        // immediate parent
{
  if (prtlev()>2) cout << "GlinXphf::GlinXphf(msmc)" << endl;
}

GlinXphfJones::GlinXphfJones(const Int& nAnt) :
  VisCal(nAnt), 
  VisMueller(nAnt),
  GlinXphJones(nAnt)
{
  if (prtlev()>2) cout << "GlinXphf::GlinXphf(nAnt)" << endl;
}

GlinXphfJones::~GlinXphfJones() {
  if (prtlev()>2) cout << "GlinXphf::~GlinXphf()" << endl;
}






} //# NAMESPACE CASA - END
