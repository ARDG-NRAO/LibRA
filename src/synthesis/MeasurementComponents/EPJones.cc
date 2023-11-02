//# EPJones.cc: Implementation of EPJOnes VisCal type
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

#include <synthesis/MeasurementComponents/EPJones.h>

#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/VisBuffAccumulator.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <synthesis/MeasurementEquations/VisEquation.h>
#include <synthesis/TransformMachines/Utils.h>
#include <synthesis/MeasurementComponents/SteepestDescentSolver.h>
#include <casacore/casa/Quanta/Quantum.h>
#include <casacore/casa/Quanta/QuantumHolder.h>

#include <casacore/tables/TaQL/ExprNode.h>

#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/OS/Memory.h>
#include <casacore/casa/System/Aipsrc.h>

#include <sstream>

#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogSink.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN
  
  
  // **********************************************************
  //  EPJones
  //
  EPJones::EPJones(VisSet& vs):
    VisCal(vs), 
    VisMueller(vs),
    SolvableVisJones(vs),
    pointPar_(),
    /*ms_p(0), */ vs_p(&vs),
    maxTimePerSolution(0), minTimePerSolution(10000000), avgTimePerSolution(0),
    timer(), polMap_p(), tolerance_p(1e-9), gain_p(0.01), niter_p(250)
  {
    if (prtlev()>2) cout << "EP::EP(vs)" << endl;
    pbwp_p = NULL;
    //   String msName = vs.msName();
    //   ms_p = new MeasurementSet(msName);
    setParType(VisCalEnum::REAL);
  }
  EPJones::EPJones(VisSet& vs, MeasurementSet&) :
    VisCal(vs), 
    VisMueller(vs),
    SolvableVisJones(vs),
    pointPar_(),
    /* ms_p(&ms),*/ vs_p(&vs),
    maxTimePerSolution(0), minTimePerSolution(10000000), avgTimePerSolution(0),
    timer(), polMap_p(), tolerance_p(1e-12), gain_p(0.01), niter_p(500)
  {
    if (prtlev()>2) cout << "EP::EP(vs)" << endl;
    pbwp_p = NULL;
    //   String msName = vs.msName();
    //   ms_p = new MeasurementSet(msName);
    setParType(VisCalEnum::REAL);
  }
  //
  //-----------------------------------------------------------------------
  //  
  EPJones::~EPJones() 
  {
    if (prtlev()>2) cout << "EP::~EP()" << endl;
  }
  //
  //-----------------------------------------------------------------------
  //  
  void EPJones::makeComplexGrid(TempImage<Complex>& Grid, 
				PagedImage<Float>& ModelImage,
				VisBuffer& vb)
  {
    Vector<Int> whichStokes(0);
    CoordinateSystem cimageCoord =
      StokesImageUtil::CStokesCoord(//cimageShape,
				    ModelImage.coordinates(),
				    whichStokes,
				    StokesImageUtil::CIRCULAR);
    
    Grid.resize(IPosition(ModelImage.ndim(),
			  ModelImage.shape()(0),
			  ModelImage.shape()(1),
			  ModelImage.shape()(2),
			  ModelImage.shape()(3)));
    
    Grid.setCoordinateInfo(cimageCoord);
    
    Grid.setMiscInfo(ModelImage.miscInfo());
    StokesImageUtil::From(Grid,ModelImage);
    
    if(vb.polFrame()==MSIter::Linear) 
      StokesImageUtil::changeCStokesRep(Grid,StokesImageUtil::LINEAR);
    else StokesImageUtil::changeCStokesRep(Grid,StokesImageUtil::CIRCULAR);
  }
  //
  //-----------------------------------------------------------------------
  //  
  void EPJones::setModel(const String& modelImageName)
  {
    ROVisIter& vi(vs_p->iter());
    VisBuffer vb(vi);
    
    PagedImage<Float> modelImage(modelImageName);
    makeComplexGrid(targetVisModel_,modelImage,vb);
    vi.originChunks();
    vi.origin();
    pbwp_p->initializeToVis(targetVisModel_,vb);

    polMap_p = pbwp_p->getPolMap();
    //    cout << "Pol Map = " << polMap_p << endl;
  }
  //
  //-----------------------------------------------------------------------
  //  
  void EPJones::setSolve(const Record& solve)
  {
    Int nFacets=1; Long cachesize=200000000;
    Float paInc=1.0; // 1 deg.
    String cfCacheDirName("tmpCFCache.dir");
    Bool doPBCorr=true, applyPointingOffsets=true;
    if (solve.isDefined("cfcache"))
      cfCacheDirName=solve.asString("cfcache");
    if (solve.isDefined("painc"))
      paInc=solve.asDouble("painc");
    if (solve.isDefined("pbcorr"))
      doPBCorr=solve.asBool("pbcorr");
    
    if (pbwp_p) delete pbwp_p;
    
    pbwp_p = new nPBWProjectFT(//*ms_p, 
    			       nFacets, 
    			       cachesize,
    			       cfCacheDirName,
    			       applyPointingOffsets,  
    			       doPBCorr,   //Bool do PB correction before prediction
    			       16,         //Int tilesize=16 
    			       paInc       //Float paSteps=1.0
    			       );
    logSink() << LogOrigin("EPJones","setSolve") 
    	      << "Using nPBWProjectFT for residual and derivative computations"
    	      << LogIO::POST;

    // pbwp_p = new PBMosaicFT(*ms_p, 
    // 			    nFacets, 
    // 			    cachesize,
    // 			    cfCacheDirName,
    // 			    applyPointingOffsets,  
    // 			    doPBCorr,   //Bool do PB correction before prediction
    // 			    16,         //Int tilesize=16 
    // 			    paInc);     //Float paSteps=1.0
    // logSink() << LogOrigin("EPJones","setSolve") 
    // 	      << "Using PBMosaicFT for residual and derivative computations"
    // 	      << LogIO::POST;
    casacore::Quantity patol(paInc,"deg");
    logSink() << LogOrigin("EPJones","setSolve") 
	      << "Parallactic Angle tolerance set to " << patol.getValue("deg") << " deg" 
	      << LogIO::POST;
    pbwp_p->setPAIncrement(patol);
    pbwp_p->setEPJones((SolvableVisJones *)this);
    
    //    pbwp_p->setPAIncrement(paInc);
    //
    // What the HELL does the following correspond to? It's not a syntax error!
    //  azOff(IPosition(1,nAnt())), elOff(IPosition(1,nAnt()));
    
    //  azOff.resize(IPosition(1,nAnt()));
    //  elOff.resize(IPosition(1,nAnt()));
    //  azOff = elOff = 0.0;
    pointPar_.resize(nPar(),1,nAnt());
    pointPar_ = 0;
    
    //    calTableName_="test";
    //  SolvableVisCal::setSolve(solve);
    
//     if (solve.isDefined("t"))
//       interval()=solve.asFloat("t");
     if (solve.isDefined("solint"))
       solint()=solve.asString("solint");
    
    QuantumHolder qhsolint;
    String error;
    Quantity qsolint;
    qhsolint.fromString(error,solint());
    if (error.length()!=0)
      throw(AipsError("EPJones::setsolve(): Unrecognized units for solint."));
    qsolint=qhsolint.asQuantumDouble();
    if (qsolint.isConform("s"))
      interval()=qsolint.get("s").getValue();
    else 
      {
	// assume seconds
	interval()=qsolint.getValue();
	solint()=solint()+"s";
      }

    if (solve.isDefined("preavg"))
      preavg()=solve.asFloat("preavg");
    
    if (solve.isDefined("refant")) {
      refantlist().resize();
      refantlist()=solve.asArrayInt("refant");
    }

    if (solve.isDefined("phaseonly"))
      if (solve.asBool("phaseonly"))
	apmode()="phaseonly";
    
    if (solve.isDefined("table"))
      calTableName()=solve.asString("table");
    
    if (solve.isDefined("append"))
      append()=solve.asBool("append");
    
    // TBD: Warn if table exists (and append=F)!
    
    // If normalizable & preavg<0, use inteval for preavg 
    //  (or handle this per type, e.g. D)
    // TBD: make a nice log message concerning preavg
    // TBD: make this work better with solnboundary par
    //
    if (preavg()<0.0) {
      if (interval()>0.0)
	// use interval
	preavg()=interval();
      else
	// scan-based, so max out preavg() to get full-chunk time-average
	preavg()=DBL_MAX;
    }
    // This is the solve context
    setSolved(true);
    setApplied(false);
    //  SolvableVisCal::setSolve(solve);
    rcs_ = new CalSet<Float>(nSpw());
  };
  //
  //-----------------------------------------------------------------------
  //  
  void EPJones::guessPar(VisBuffer& /*vb*/)
  {
    pointPar_=0;
    //  solveRPar() = 0;
  }
  //
  //-----------------------------------------------------------------------
  //  
  Cube<Float>& EPJones::loadPar() 
  {
    return pointPar_;
  }
  //
  //-----------------------------------------------------------------------
  //  
  // Specialized setapply extracts image info ("extracts image info"?)
  //
  void EPJones::setApply(const Record& apply) 
  {
    // Call generic
    //
    // Yet again, this assumes a complex CalSet!
    //    SolvableVisCal::setApply(applypar);
  if (prtlev()>2) cout << "EPJones::setApply(apply)" << endl;

  // Call VisCal version for generic stuff
  // Sets the value returned by currSpw().
  // Resizes currCPar or currRPar to nPar() x nChanPar() x nElem()
  // Resizes currParOK() to nChanPar() x nElem()
  // Set currParOK() = true
  //
  VisCal::setApply(apply);

  // Collect Cal table parameters
  if (apply.isDefined("table")) {
    calTableName()=apply.asString("table");
    verifyCalTable(calTableName());
  }

  if (apply.isDefined("select"))
    calTableSelect()=apply.asString("select");

  // Does this belong here?
  if (apply.isDefined("append"))
    append()=apply.asBool("append");

  // Collect interpolation parameters
  if (apply.isDefined("interp"))
    tInterpType()=apply.asString("interp");

  // TBD: move spw to VisCal version?
  if (apply.isDefined("spwmap")) {
    Vector<Int> spwmap(apply.asArrayInt("spwmap"));
    spwMap()(IPosition(1,0),IPosition(1,spwmap.nelements()-1))=spwmap;
  }

  // TBD: move interval to VisCal version?
  if (apply.isDefined("t"))
    interval()=apply.asFloat("t");

  // This is apply context  
  setApplied(true);
  setSolved(false);

  //  TBD:  "Arranging to apply...."


  // Create CalSet, from table
  switch(parType())
    {
    case VisCalEnum::REAL:
      {
	rcs_ = new CalSet<Float>(calTableName(),calTableSelect(),nSpw(),nPar(),nElem());
	//	rcs().initCalTableDesc(typeName(),parType());
	break;
      }
    default:
      throw(AipsError("Unsupported parameter type found in EPJones::setapply().  Bailing out."));
    }    

  }
  //
  //-----------------------------------------------------------------------
  //  
  void EPJones::applyCal(VisBuffer& vb, Cube<Complex>& Mout) 
  {
    //
    // Inflate model data in VB, Mout references it
    //  (In this type, model data is always re-calc'd from scratch)
    //
    vb.modelVisCube(true);
    Mout.reference(vb.modelVisCube());
  }
  //
  //-----------------------------------------------------------------------
  //  
  void EPJones::differentiate(VisBuffer& vb,
			      Cube<Complex>& Mout,
			      Array<Complex>& dMout,
			      Matrix<Bool>& Mflg) 
  {
    Int nCorr(2); // TBD
    //
    // Load the offsets from the internal EPJones storage
    // (ultimately change the data structures to not need these copies)
    //
    IPosition ndx(1);
    Cube<Float> pointingOffsets(nPar(),1,nAnt());
    for(ndx(0)=0;ndx(0)<nAnt();ndx(0)++)
      for(Int j=0;j<nPar();j++)
	{
	  // Use solveRPar()(nPar,0,Ant)
	  pointingOffsets(j,0,ndx(0)) = solveRPar()(j,0,ndx(0));//pointPar_(0,0,ndx(0));
	}
    //
    // Size differentiated model
    //
    dMout.resize(IPosition(5,nCorr,nPar(),1,vb.nRow(),2));
    //
    // Model vis shape must match visibility
    //
    vb.modelVisCube(false);
    Mout.reference(vb.modelVisCube());
    
    //
    // Compute the corrupted model and the derivatives.
    //  
    /*
      Cube<Complex> dVAz(Mout.shape()), dVEl(Mout.shape());
      pbwp_p->nget(vb, azOff, elOff, Mout,dVAz, dVEl,0,1);
    */
    VisBuffer dAZVB(vb), dELVB(vb);
    Cube<Complex> dVAz, dVEl;
    dAZVB.modelVisCube().resize(vb.modelVisCube().shape());
    dELVB.modelVisCube().resize(vb.modelVisCube().shape());
    dVAz.reference(dAZVB.modelVisCube()); 
    dVEl.reference(dELVB.modelVisCube());
    
    pbwp_p->get(vb, dAZVB, dELVB, pointingOffsets);
    
    //   for(Int i=0;i<vb.modelVisCube().shape()(2);i++)
    //     {
    //       cout << i 
    // 	   << " " << vb.modelVisCube()(0,0,i) 
    // 	   << " " << vb.modelVisCube()(1,0,i)
    // 	   << " " << vb.visCube()(0,0,i) 
    // 	   << " " << vb.visCube()(1,0,i)
    // 	   << " " << vb.flag()(0,i) 
    // 	   << " " << vb.flag()(1,i) 
    // 	   << " " << vb.antenna1()(i) 
    // 	   << " " << vb.antenna2()(i) 
    // 	   << " " << vb.flagRow()(i) 
    
    // 	   << endl;
    //     }
    
    //
    // For now, copy the derivatives to the required data structure.
    //
    for(Int j=0;j<nCorr;j++)
      for(Int i=0;i<vb.nRow();i++)
	{
	  dMout(IPosition(5,j,0,0,i,0))=dVAz(j,0,i);
	  dMout(IPosition(5,j,1,0,i,0))=dVEl(j,0,i);
	  //
	  // Not sure if the following is what's needed by the solver
	  // 
	  dMout(IPosition(5,j,0,0,i,1))=conj(dVAz(j,0,i));
	  dMout(IPosition(5,j,1,0,i,1))=conj(dVEl(j,0,i));
	  dMout(IPosition(5,j,0,0,i,1))=(dVAz(j,0,i));
	  dMout(IPosition(5,j,1,0,i,1))=(dVEl(j,0,i));
	}
    
    Mflg.reference(vb.flag());
  }
  //
  //-----------------------------------------------------------------------
  //  
  void EPJones::differentiate(VisBuffer& vb,VisBuffer& dAZVB,VisBuffer& dELVB,
			      Matrix<Bool>& Mflg) 
  {
    //
    // Load the offsets from the internal EPJones storage
    // (ultimately change the data structures to not need these copies)
    //
    IPosition ndx(1);
    Cube<Float> pointingOffsets(nPar(),1,nAnt());
    for(ndx(0)=0;ndx(0)<nAnt();ndx(0)++)
      for(Int j=0;j<nPar();j++)
	{
	  pointingOffsets(j,0,ndx(0)) = solveRPar()(0,0,ndx(0));//pointPar_(0,0,ndx(0));
	}
    //
    // Model vis shape must match visibility
    //
    vb.modelVisCube(false);
    //
    // Compute the corrupted model and the derivatives.
    //  
    dAZVB = vb;
    dELVB = vb;
    Cube<Complex> dVAz, dVEl;
    dAZVB.modelVisCube().resize(vb.modelVisCube().shape());
    dELVB.modelVisCube().resize(vb.modelVisCube().shape());
    dVAz.reference(dAZVB.modelVisCube()); 
    dVEl.reference(dELVB.modelVisCube());
    
    pbwp_p->get(vb, dAZVB, dELVB, pointingOffsets);
    
    //  cout << pointingOffsets << endl;
    //   for(Int i=0;i<vb.modelVisCube().shape()(2);i++)
    //     {
    //       cout << "Model: " << i 
    // 	   << " " << abs(vb.modelVisCube()(0,0,i))
    // 	   << " " << arg(vb.modelVisCube()(0,0,i))*57.295
    // 	//	   << " " << vb.modelVisCube()(1,0,i)
    // 	   << " " << abs(vb.visCube()(0,0,i))
    // 	   << " " << arg(vb.visCube()(0,0,i))*57.295
    // 	//	   << " " << vb.visCube()(1,0,i)
    // 	   << " " << vb.flag()(0,i) 
    // 	//	   << " " << vb.flag()(1,i) 
    // 	   << " " << vb.antenna1()(i) 
    // 	   << " " << vb.antenna2()(i) 
    // 	   << " " << vb.uvw()(i)(0)
    // 	   << " " << vb.uvw()(i)(1)
    // 	   << " " << vb.uvw()(i)(2)
    // 	   << " " << vb.flagRow()(i) 
    // 	   << " " << vb.flagCube()(0,0,i) 
    // 	//	   << " " << vb.flagCube()(1,0,i) 
    // 	   << " " << vb.weight()(i)
    // 	   << endl;
    //     }
    //   exit(0);
    Mflg.reference(vb.flag());
  }
  
  void EPJones::keep(const Int& slot)
  {
    if (prtlev()>4) cout << " SVC::keep(i)" << endl;
    
    if (slot<rcs().nTime(currSpw())) 
      {
	rcs().fieldId(currSpw())(slot)=currField();
	rcs().time(currSpw())(slot)=refTime();
	//
	// Only stop-start diff matters
	//  TBD: change CalSet to use only the interval
	//  TBD: change VisBuffAcc to calculate exposure properly
	//
	rcs().startTime(currSpw())(slot)=0.0;
	rcs().stopTime(currSpw())(slot)=interval();
	//
	// For now, just make these non-zero:
	//
	rcs().iFit(currSpw()).column(slot)=1.0;
	rcs().iFitwt(currSpw()).column(slot)=1.0;
	rcs().fit(currSpw())(slot)=1.0;
	rcs().fitwt(currSpw())(slot)=1.0;
      
	IPosition blc4(4,0,       focusChan(),0,        slot);
	IPosition trc4(4,nPar()-1,focusChan(),nElem()-1,slot);
	rcs().par(currSpw())(blc4,trc4).nonDegenerate(3) = solveRPar();
	
	IPosition blc3(3,focusChan(),0,        slot);
	IPosition trc3(3,focusChan(),nElem()-1,slot);
	rcs().parOK(currSpw())(blc4,trc4).nonDegenerate(3)= solveParOK();
	rcs().parErr(currSpw())(blc4,trc4).nonDegenerate(3)= solveParErr();
	rcs().parSNR(currSpw())(blc4,trc4).nonDegenerate(3)= solveParSNR();
	rcs().solutionOK(currSpw())(slot) = anyEQ(solveParOK(),true);
	
      }
    else
      throw(AipsError("SVJ::keep: Attempt to store solution in non-existent CalSet slot"));
  }
  //
  //-----------------------------------------------------------------------
  //  
  void EPJones::setSolve() 
  {
    if (prtlev()>2)  cout << "EPJ::setSolve()" << endl;
    
    interval()=10.0;
    refant()=-1;
    apmode()="<none>";
    calTableName()="<none>";
    
    // This is the solve context
    setSolved(true);
    setApplied(false);
    
    // Create a pristine CalSet
    //  TBD: move this to inflate()?
    rcs_ = new CalSet<Float>(nSpw());
  }
  //
  //-----------------------------------------------------------------------
  //  
  void EPJones::inflate(const Vector<Int>& nChan,const Vector<Int>& startChan,
			const Vector<Int>& nSlot) 
  {
    if (prtlev()>3) cout << "  EPJ::inflate(,,)" << endl;
    //
    // Size up the CalSet
    //
    rcs().resize(nPar(),nChan,nElem(),nSlot);
    rcs().setStartChan(startChan);
  }
  //
  //-----------------------------------------------------------------------
  //  
  void EPJones::initSolve(VisSet& vs) 
  {
    if (prtlev()>2) cout << "EPJ::initSolve(vs)" << endl;
    
    // Determine solving channelization according to VisSet & type
    setSolveChannelization(vs);
    
    // Nominal spwMap in solve is identity
    spwMap().resize(vs.numberSpw());
    indgen(spwMap());

    // Inflate the CalSet according to VisSet
    SolvableVisCal::inflate(vs);
    
    //
    rcs().initCalTableDesc(typeName(),parType());
    
    // Size the solvePar arrays
    initSolvePar();
  }
  //
  //-----------------------------------------------------------------------
  //  
  void EPJones::initSolvePar() 
  {
    if (prtlev()>3) cout << " EPJ::initSolvePar()" << endl;
    
    for (Int ispw=0;ispw<nSpw();++ispw) 
      {
	currSpw()=ispw;
      
	//	cout << "EPJ::initSolvePar(): " << solveRPar().shape() << " " << solveParOK().shape() << endl;
	solveRPar().resize(nPar(),1,nAnt());
	solveParOK().resize(nPar(),1,nAnt());
	solveParErr().resize(nPar(),1,nAnt());
	
	solveRPar()=(0.0);
	solveParOK()=true;

	solveParSNR().resize(nPar(),1,nAnt());
	solveParSNR()=0.0;
      }

    currSpw()=0;
  }
  //
  //-----------------------------------------------------------------------
  //  
  void EPJones::store() 
  {
    if (prtlev()>3) cout << " EPJ::store()" << endl;
    
    if (append())
      logSink() << "Appending solutions to table: " << calTableName()
		<< LogIO::POST;
    else
      logSink() << "Writing solutions to table: " << calTableName()
		<< LogIO::POST;
    
    rcs().store(calTableName(),typeName(),append());
  }
  //
  //-----------------------------------------------------------------------
  //  
  void EPJones::store(const String& table,const Bool& append) 
  {
    if (prtlev()>3) cout << " EPJ::store(table,append)" << endl;
    
    // Override tablename
    calTableName()=table;
    SolvableVisCal::append()=append;
    
    // Call conventional store
    store();
  }
  //
  //-----------------------------------------------------------------------
  //  
  Bool EPJones::verifyForSolve(VisBuffer& vb) 
  {
    //  cout << "verifyForSolve..." << endl;
    
    Int nAntForSolveFinal(-1);
    Int nAntForSolve(0);
    
    // We will count baselines and weights per ant
    //   and set solveParOK accordingly
    Vector<Int> blperant(nAnt(),0);
    Vector<Double> wtperant(nAnt(),0.0);
    Vector<Bool> antOK(nAnt(),false);
    
    
    while (nAntForSolve!=nAntForSolveFinal) 
      {
	nAntForSolveFinal=nAntForSolve;
	nAntForSolve=0;
	// TBD: optimize indexing with pointers in the following
	blperant=0;
	wtperant=0.0;
	for (Int irow=0;irow<vb.nRow();++irow) 
	  {
	    Int a1=vb.antenna1()(irow);
	    Int a2=vb.antenna2()(irow);
	    if (!vb.flagRow()(irow) && a1!=a2) 
	      {
		if (!vb.flag()(focusChan(),irow)) 
		  {
		    blperant(a1)+=1;
		    blperant(a2)+=1;
	    
		    wtperant(a1)+=Double(sum(vb.weightMat().column(irow)));
		    wtperant(a2)+=Double(sum(vb.weightMat().column(irow)));
		  }
	      }
	  }
      
	antOK=false;
	for (Int iant=0;iant<nAnt();++iant) 
	  {
	    if (blperant(iant)>3 &&
		wtperant(iant)>0.0) 
	      {
		// This antenna is good, keep it
		nAntForSolve+=1;
		antOK(iant)=true;
	      }
	    else 
	      {
		// This antenna under-represented; flag it
		vb.flagRow()(vb.antenna1()==iant)=true;
		vb.flagRow()(vb.antenna2()==iant)=true;
	      }
	  }
	//    cout << "blperant     = " << blperant << endl;
	//  cout << "wtperant = " << wtperant << endl;
	//    cout << "nAntForSolve = " << nAntForSolve << " " << antOK << endl;
      }
    // We've converged on the correct good antenna count
    nAntForSolveFinal=nAntForSolve;
    
    // Set a priori solution flags  
    solveParOK() = false;
    for (Int iant=0;iant<nAnt();++iant)
      if (antOK(iant))
	// This ant ok
	solveParOK().xyPlane(iant) = true;
      else
	// This ant not ok, set soln to zero
	solveRPar().xyPlane(iant)=0.0;
    
    //  cout << "antOK = " << antOK << endl;
    //  cout << "solveParOK() = " << solveParOK() << endl;
    //  cout << "amp(solvePar()) = " << amplitude(solvePar()) << endl;
    
    return (nAntForSolve>3);
  }
  //
  //-----------------------------------------------------------------------
  //  
  void EPJones::postSolveMassage(const VisBuffer& vb)
  {
    Array<Float> sol;
    Double PA = getPA(vb);
    Float dL, dM;
    IPosition ndx(3,0,0,0);
    
    for(ndx(2)=0;ndx(2)<nAnt();ndx(2)++)
      {
	ndx(0)=0;      dL = solveRPar()(ndx);
	ndx(0)=1;      dM = solveRPar()(ndx);
	ndx(0)=0;
	solveRPar()(ndx) = dL*cos(PA) - dM*sin(PA);
	ndx(0)=1;
	solveRPar()(ndx) = dL*sin(PA) - dM*cos(PA);
      }
  };
  //
  //-----------------------------------------------------------------------
  //  
  void EPJones::printActivity(const Int slotNo, const Int fieldId, 
			      const Int spw, const Int nSolutions)
  {
    Int nSlots, nMesg;

    nSlots = rcs().nTime(spw);
    
    Double timeTaken = timer.all();
    if (maxTimePerSolution < timeTaken) maxTimePerSolution = timeTaken;
    if (minTimePerSolution > timeTaken) minTimePerSolution = timeTaken;
    avgTimePerSolution += timeTaken;
    Double avgT =  avgTimePerSolution/(nSolutions>0?nSolutions:1);
    //
    // Boost the no. of messages printed if the next message, based on
    // the average time per solution, is going to appear after a time
    // longer than my (SB) patience would permit!  The limit of
    // patience is set to 10 min.
    //
    Float boost = avgT*printFraction(nSlots)*nSlots/(10*60.0);
    boost = (boost < 1.0)? 1.0:boost;
    nMesg = (Int)(nSlots*printFraction(nSlots)/boost);
    nMesg = (nMesg<1?1:nMesg);
    
    Int tmp=abs(nSlots-slotNo); Bool print;
    print = false;
    if ((slotNo == 0) || (slotNo == nSlots-1))  print=true;
    else if ((tmp > 0 ) && ((slotNo+1)%nMesg ==0)) print=true;
    else print=false;

    if (print)
      {
	Int f = (Int)(100*(slotNo+1)/(nSlots>0?nSlots:1));
	logSink()<< LogIO::NORMAL 
		 << "Spw=" << spw << " slot=" << slotNo << "/" << nSlots 
                 << " field=" << fieldId << ". Done " << f << "%"
		 << " Time taken per solution (max/min/avg): "
		 << maxTimePerSolution << "/" 
		 << (minTimePerSolution<0?1:minTimePerSolution) << "/"
		 << avgT << " sec" << LogIO::POST;
      }
  }
  //
  //-----------------------------------------------------------------------
  //  
  void EPJones::selfGatherAndSolve(VisSet& vs, VisEquation& ve)
  {
    //
    // Create the solver
    //
    SteepestDescentSolver sds(nPar(),polMap_p,niter_p,tolerance_p);
    logSink() << LogOrigin("EPJones","selfGatherAndSolve")
	      << "Pol map = " << polMap_p << endl;
    sds.setGain(gain_p);
    //sds.setGain(1);
    //
    // Inform logger/history
    //
    logSink() << LogOrigin("EPJones", "selfGatherAndSolve") << "Solving for " << typeName()
	      << LogIO::POST;
    //
    // Arrange for iteration over data - set up the VisIter and the VisBuffer
    //
    Block<Int> columns;
    if (interval()==0.0) 
      {
	columns.resize(5);
	columns[0]=MS::ARRAY_ID;
	columns[1]=MS::SCAN_NUMBER;
	columns[2]=MS::FIELD_ID;
	columns[3]=MS::DATA_DESC_ID;
	columns[4]=MS::TIME;
      } 
    else 
      {
	columns.resize(4);
	columns[0]=MS::ARRAY_ID;
	columns[1]=MS::FIELD_ID;
	columns[2]=MS::DATA_DESC_ID;
	columns[3]=MS::TIME;
      }
    vs.resetVisIter(columns,interval());
    VisIter& vi(vs.iter());
    VisBuffer vb(vi);
    
    //
    // Make an initial guess for the solutions
    //
    guessPar(vb);
    initSolve(vs);
    
    Vector<Int> islot(vs.numberSpw(),0);
    Int nGood(0);
    
    for (vi.originChunks(); vi.moreChunks(); vi.nextChunk()) 
      {
	Int spw(vi.spectralWindow());
	// The following now works on the first VB only.  Is this right?
	Bool vbOk=syncSolveMeta(vb,vi.fieldId());
	
	if (vbOk) 
	  {
	    Bool totalGoodSol(false);
	    for (Int ich=nChanPar()-1;ich>-1;--ich) 
	      {
		focusChan()=ich;
		Bool goodSoln;
		timer.mark();
// 		goodSoln = (sds.solve(ve, *this, svb,nAnt(),islot(spw))<0)?false:True;
		goodSoln = (sds.solve2(ve,vi,*this, nAnt(),islot(spw))<0)?false:True;
//   		{
//   		  cout << "Solutions = " << MVTime(vb.time()(0)/86400.0) 
//   		       << solveRPar()*57.295*3600 << " "
//   		       << endl;
//   		}	
		if (goodSoln) 
		  {
		    //
		    // Apply transform (if any) to the solutions.
		    //
//  		    postSolveMassage(vb);
		    totalGoodSol=true;
		    keep(islot(spw));
		    printActivity(islot(spw),vi.fieldId(),spw,nGood);	      
		  }
	      } // parameter channels
	    if (totalGoodSol)	nGood++;
	  } // vbOK
	islot(spw)++;
      } // chunks
    logSink() << "  Found " << nGood << " good " 
	      << typeName() << " solutions." 
	      << LogIO::POST;
    store();
  };
  //
  //-----------------------------------------------------------------------
  //  
  void EPJones::diffResiduals(VisIter& vi, VisEquation& /*ve*/,
			      VisBuffer& residuals,
			      VisBuffer& dAZVB,
			      VisBuffer& dELVB,
			      Matrix<Bool>& Mflg)
  {
    VisBuffAccumulator resAvgr(nAnt(),preavg(),false),
      dRes1Avgr(nAnt(), preavg(), false),
      dRes2Avgr(nAnt(), preavg(), false);
    VisBuffer vb(vi);
    IPosition shp;
    //
    // Load the offsets from the internal EPJones storage
    // (ultimately change the data structures to not need these copies)
    //
    //    Cube<Float> pointingOffsets(2,1,nAnt());
//     Int nchan=1;
//     Cube<Float> pointingOffsets(nPar(),nchan,nAnt());
//     for(Int ant=0;ant<nAnt();ant++)
//       for(Int par=0;par<nPar();par++)
// 	for(Int chan=0;chan<nchan;chan++)
// 	  pointingOffsets(par,chan,ant) = solveRPar()(par,chan,ant);//pointPar_(0,0,ndx(0));

    //    cout << "EPJ: " << pointingOffsets << endl;
    //
    // Model vis shape must match visibility
    //
    residuals.modelVisCube(false);

    residuals.modelVisCube() = Complex(0,0);
    dAZVB.modelVisCube() = dELVB.modelVisCube() = Complex(0,0);  
    static Int j=0;
/*
    cout << "EPJ::diff: " << residuals.modelVisCube().shape() << " "
                          << vb.visCube().shape() << " " 
                          << vb.modelVisCube().shape() << " "
                          << dAZVB.modelVisCube().shape() << " "
                          << dELVB.modelVisCube().shape() << endl;
*/
    for (vi.origin(); vi.more(); vi++) 
      {
	//	ve.collapse(vb);
	dAZVB = dELVB = vb;
	shp = vb.modelVisCube().shape();

	// Use the target VBs as temp. storage as well 
	// (reduce the max. mem. footprint)
	dAZVB.modelVisCube().resize(shp);
	dELVB.modelVisCube().resize(shp);
	vb.modelVisCube() = dAZVB.modelVisCube() = dELVB.modelVisCube() = Complex(0,0);

	//	pbwp_p->get(vb, dAZVB, dELVB, pointingOffsets);
	pbwp_p->get(vb, dAZVB, dELVB, solveRPar());
	//	pbwp_p->get(vb);
	/*
	//
	// Debugging code.
	// This can be slow.  So comment it out for production line code.
	//
	String mesg;
	ostringstream mesg2;
	mesg2 << " VB no. " << j << " in time integration in EJones::diffResiduals "
	      << "Pointing offsets = " << pointingOffsets;
	if (isVBNaN(vb,mesg)) throw(AipsError("VB has NaN "+mesg+String(mesg2.str().c_str())));
	if (isVBNaN(dAZVB,mesg)) throw(AipsError("AZVB has NaN "+mesg+String(mesg2.str().c_str())));
	if (isVBNaN(dELVB,mesg)) throw(AipsError("ELVB has NaN "+mesg+String(mesg2.str().c_str())));
	*/
	//	if (j == 4)
/*
	  {
	    //	    pbwp_p->get(vb, dAZVB, dELVB, pointingOffsets);
	    cout << "chunk==========================================" << endl;
	    cout << vb.modelVisCube().shape() << " " << vb.visCube().shape() << " " << vb.flag().shape() 
		 << vb.flagRow().shape() << " " << vb.flagCube().shape() 
                 << solveRPar().shape() << " "
	         << endl;
	    Int m=1;
	    for(Int i=0;i<vb.modelVisCube().shape()(2);i++)
	    //for(Int i=0;i<2;i++)
	      {
		cout << "EPJ Residual: " << i 
		     << " " << getCurrentTimeStamp(vb)/1e9-4.68002
		     << " " << vb.modelVisCube()(m,0,i)
		     << " " << vb.visCube()(m,0,i)
		     << " " << vb.modelVisCube()(m,0,i)-vb.visCube()(m,0,i) 
		     << " " << vb.antenna1()(i)<< "-" << vb.antenna2()(i) 
 		     << " " << vb.flag()(0,i) 
 		     << " " << vb.flagRow()(i) 
 		     << " " << vb.flagCube()(m,0,i) 
		     << " " << solveRPar()(0,0,vb.antenna1()(i))
		     << " " << solveRPar()(0,0,vb.antenna2()(i))
		     << " " << solveRPar()(1,0,vb.antenna1()(i))
		     << " " << solveRPar()(1,0,vb.antenna2()(i))
		     << " " << solveRPar()(2,0,vb.antenna1()(i))
		     << " " << solveRPar()(2,0,vb.antenna2()(i))
		     << endl;
	      }
	    //	    exit(0);
	  }
*/

	vb.modelVisCube() -= vb.visCube();  // Residual = VModel - VObs
	//	vb.modelVisCube() -= vb.correctedVisCube();  // Residual = VModel - VObs

	resAvgr.accumulate(vb);
	dRes1Avgr.accumulate(dAZVB);
	dRes2Avgr.accumulate(dELVB);
	j++;
      }
    

    resAvgr.finalizeAverage();
    dRes1Avgr.finalizeAverage();
    dRes2Avgr.finalizeAverage();
    //
    // First copy the internals of the averaged VisBuffers (i.e, Time,
    // UVW, Weights, etc.)
    //
    residuals = resAvgr.aveVisBuff();
    dAZVB = dRes1Avgr.aveVisBuff();
    dELVB = dRes2Avgr.aveVisBuff();
    //
    // Now resize the modelVisCube() of the target VisBuffers (Not
    // resizing the LHS in LHS=RHS of CASA Arrays must be leading to
    // extra code of this type all over the place)
    //
    //    shp = resAvgr.aveVisBuff().modelVisCube().shape();  
    //
    // The data cubes...
    //
    residuals.modelVisCube().resize(resAvgr.aveVisBuff().modelVisCube().shape());
    dAZVB.modelVisCube().resize(dRes1Avgr.aveVisBuff().modelVisCube().shape());
    dELVB.modelVisCube().resize(dRes2Avgr.aveVisBuff().modelVisCube().shape());
    // The flag cubes..
    residuals.flagCube().resize(resAvgr.aveVisBuff().flagCube().shape());
    dAZVB.flagCube().resize(dRes1Avgr.aveVisBuff().flagCube().shape());
    dELVB.flagCube().resize(dRes2Avgr.aveVisBuff().flagCube().shape());
    // The flags...
    residuals.flag().resize(resAvgr.aveVisBuff().flag().shape());
    dAZVB.flag().resize(dRes1Avgr.aveVisBuff().flag().shape());
    dELVB.flag().resize(dRes2Avgr.aveVisBuff().flag().shape());
    // The row flags....
    residuals.flagRow().resize(resAvgr.aveVisBuff().flagRow().shape());
    dAZVB.flagRow().resize(dRes1Avgr.aveVisBuff().flagRow().shape());
    dELVB.flagRow().resize(dRes2Avgr.aveVisBuff().flagRow().shape());

    residuals.weight().resize(resAvgr.aveVisBuff().weight().shape());
    //
    // Now copy the modelVisCube() from the averaged VisBuffers to the
    // target VisBuffers().
    //
    // The data cubes...
    residuals.modelVisCube() = resAvgr.aveVisBuff().modelVisCube();
    dAZVB.modelVisCube()     = dRes1Avgr.aveVisBuff().modelVisCube();
    dELVB.modelVisCube()     = dRes2Avgr.aveVisBuff().modelVisCube();

    // The flag cubes...    
    residuals.flagCube() = resAvgr.aveVisBuff().flagCube();
    dAZVB.flagCube()     = dRes1Avgr.aveVisBuff().flagCube();
    dELVB.flagCube()     = dRes2Avgr.aveVisBuff().flagCube();
    // The flags...
    residuals.flag() = resAvgr.aveVisBuff().flag();
    dAZVB.flag()     = dRes1Avgr.aveVisBuff().flag();
    dELVB.flag()     = dRes2Avgr.aveVisBuff().flag();
    // The row flags...
    residuals.flagRow() = resAvgr.aveVisBuff().flagRow();
    dAZVB.flagRow()     = dRes1Avgr.aveVisBuff().flagRow();
    dELVB.flagRow()     = dRes2Avgr.aveVisBuff().flagRow();

    residuals.weight() = resAvgr.aveVisBuff().weight();
    //
    // Average the residuals and the derivates in frequency.
    //
    residuals.freqAveCubes();
    dAZVB.freqAveCubes();
    dELVB.freqAveCubes();
    /*
    residuals=dAZVB=dELVB=vb;
    shp = vb.modelVisCube().shape();  
    residuals.modelVisCube().resize(shp);
    dAZVB.modelVisCube().resize(shp);
    dELVB.modelVisCube().resize(shp);
    
    residuals.modelVisCube() = vb.modelVisCube();
    dAZVB.modelVisCube()     = dAZVB.modelVisCube();
    dELVB.modelVisCube()     = dELVB.modelVisCube();
    */
/*
	  {
	    //	    pbwp_p->get(vb, dAZVB, dELVB, pointingOffsets);
	    cout << "chunk==========================================" << endl;
	    cout << residuals.modelVisCube().shape() << " " 
		 << residuals.visCube().shape() << " " 
		 << residuals.flag().shape() 
		 << residuals.flagRow().shape() << " " 
		 << residuals.flagCube().shape() << endl;
	    Int m=1;
	    for(Int i=0;i<residuals.modelVisCube().shape()(2);i++)
	    //for(Int i=0;i<2;i++)
	      {
		cout << "EPJ AvgResidual: " << i 
		     << " " << getCurrentTimeStamp(vb)/1e9-4.68002
		     << " " << residuals.modelVisCube()(m,0,i)
		     << " " << residuals.visCube()(m,0,i)
		     << " " << residuals.modelVisCube()(m,0,i)-residuals.visCube()(m,0,i) 
		     << " " << residuals.antenna1()(i)<< "-" << residuals.antenna2()(i) 
 		     << " " << residuals.flag()(0,i) 
 		     << " " << residuals.flagRow()(i) 
 		     << " " << residuals.flagCube()(m,0,i) 
		     << " " << solveRPar()(0,0,residuals.antenna1()(i))
		     << " " << solveRPar()(0,0,residuals.antenna2()(i))
		     << " " << solveRPar()(1,0,residuals.antenna1()(i))
		     << " " << solveRPar()(1,0,residuals.antenna2()(i))
		     << " " << solveRPar()(2,0,residuals.antenna1()(i))
		     << " " << solveRPar()(2,0,residuals.antenna2()(i))
		     << endl;
	      }
	    //	    exit(0);
	  }
*/
    Mflg.reference(residuals.flag());  
    //    shp = residuals.flag().shape();
  }
  
  //
  // Quick-n-dirty implementation - to be replaced by use of CalInterp
  // class if-and-when that's ready for use
  //
  void  EPJones::nearest(const Double thisTime, Array<Float>& vals)
  {
    Array<Float>  par  = getOffsets(currSpw());
    Vector<Double> time = getTime(currSpw());
    //    Int nant=nAnt();
    uInt nTimes = time.nelements(), slot=0;
    Double dT=abs(time[0]-thisTime);
    IPosition shp=par.shape();

    for(uInt i=0;i<nTimes;i++)
      if (abs(time[i]-thisTime) < dT)
	{
	  dT = abs(time[i]-thisTime);
	  slot=i;
	}

    if (slot >= nTimes) throw(AipsError("EPJones::nearest(): Internal problem - "
					"nearest slot is out of range"));
    Array<Float> tmp=par(IPosition(4,0,0,0,slot), IPosition(4,shp[0]-1,shp[1]-1,shp[2]-1,slot));
    vals.resize();
    vals = tmp;
  }

void EPJones::printRPar()
{
     Int n=solveRPar().shape()(2);
     for(Int i=0;i<n;i++)
	cout << solveRPar()(0,0,i) << " "
             << solveRPar()(1,0,i) << " "
             << solveRPar()(2,0,i) << " "
             << solveRPar()(3,0,i) 
             << endl;
}

} //# NAMESPACE CASA - END


//  cout << pointingOffsets << endl;
//   for(Int i=0;i<vb.modelVisCube().shape()(2);i++)
//     {
//       cout << "Model: " << i 
// 	   << " " << abs(vb.modelVisCube()(0,0,i))
// 	   << " " << arg(vb.modelVisCube()(0,0,i))*57.295
// 	//	   << " " << vb.modelVisCube()(1,0,i)
// 	   << " " << abs(vb.visCube()(0,0,i))
// 	   << " " << arg(vb.visCube()(0,0,i))*57.295
// 	//	   << " " << vb.visCube()(1,0,i)
// 	   << " " << vb.flag()(0,i) 
// 	//	   << " " << vb.flag()(1,i) 
// 	   << " " << vb.antenna1()(i) 
// 	   << " " << vb.antenna2()(i) 
// 	   << " " << vb.uvw()(i)(0)
// 	   << " " << vb.uvw()(i)(1)
// 	   << " " << vb.uvw()(i)(2)
// 	   << " " << vb.flagRow()(i) 
// 	   << " " << vb.flagCube()(0,0,i) 
// 	//	   << " " << vb.flagCube()(1,0,i) 
// 	   << " " << vb.weight()(i)
// 	   << endl;
//     }
//   exit(0);
