//# ConjugateGradientSolver.cc: Implementation of an iterative ConjugateGradientSolver
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
//# $Id: ConjugateGradientSolver.cc,v 1.4 2006/05/02 15:59:22 sbhatnag Exp $

#include <synthesis/MeasurementComponents/SteepestDescentSolver.h>
#include <synthesis/TransformMachines/Utils.h>
#include <casa/Arrays/MatrixMath.h>
using namespace casacore;
namespace casa {

  SteepestDescentSolver::SteepestDescentSolver(Int nParams,Vector<Int> polMap,
					       Int nIter, Double tol):
    Iterate(), polMap_p(polMap)
  {
    setNumberIterations(nIter);
    setTolerance(tol);
    setMaxParams(nParams);
  };
  //
  //---------------------------------------------------------------------------
  // Get a vector of visibilities from all baselines with antenna
  // whichAnt.  It returns N-1 values (fills in the complex conjugate
  // part as well).
  //
  Vector<Complex> SteepestDescentSolver::getVj(const VisBuffer& vb, Int NAnt, Int whichAnt,
					       Int whichPol,
					       Double& sumWt,Int negate,Int weighted)
  {
    Vector<Int> ant1, ant2;
    Vector<Complex> Vj;
    Vector<Int> nPoints;
    ant1 = vb.antenna1();
    ant2 = vb.antenna2();
    Vj.resize(NAnt);
    nPoints.resize(NAnt);
    Int J=0,N;
    IPosition ndx(3,0);
    Vj = 0;
    nPoints = 0;
    N = vb.nRow();
    Double wt;
    sumWt = 0;
    //
    // It will be most useful to use
    // both RR and LL, and both cases are useful: (1) compute joind RR
    // and LL chisq, and (2) compute separate Chisq for RR and LL.
    // The CalTable stuff below all this is however not yet capable of
    // writing a caltable with 2 real parameters for 2 polarizations.
    //
    // Currently computing chisq for only the first polarization in
    // vis. ArrayColumn from the MS.
    //
    ndx=0;ndx(0)=whichPol;
    Int i;
    //#pragma omp parallel default(shared) private(i)
    {
      //#pragma omp for 
      //      for(ndx(2);ndx(2)<N;ndx(2)++)
      for(i=ndx(2);i<N;i++)
	{
	  ndx(2)=i;
	  if ((!vb.flagRow()(ndx(2)))     && 
	      (!vb.flag()(0,ndx(2)))    &&
	      //	  (!vb.flagCube()(ndx))   &&
	      ((ant1[ndx(2)] != ant2[ndx(2)]) && 
	       ((ant1[ndx(2)] == whichAnt)     || 
		(ant2[ndx(2)] == whichAnt))
	       ))
	    {
	      wt = vb.weight()(ndx(2));
	      sumWt += wt;
	    
	      if (!weighted) wt = 1.0;

	      J=(ant1[ndx[2]]!=whichAnt)?ant1[ndx(2)]:ant2[ndx(2)];
	      nPoints[J]++;
	      if (ant1[ndx(2)] > ant2[ndx(2)])  
		Vj[J] += Complex(wt,0)*vb.modelVisCube()(ndx);
	      else
		if (negate)
		  Vj[J] += -Complex(wt,0)*(vb.modelVisCube()(ndx));
		else
		  Vj[J] += Complex(wt,0)*conj(vb.modelVisCube()(ndx));
	      J++;
	    }
	}
    }
    //#pragma omp parallel default(shared) private(i)
    //#pragma omp for 
    for(i=0;i<NAnt;i++) 
      if (nPoints[i] <= 0) Vj[i] = 0.0;
	
    return Vj;
  }
  //
  //---------------------------------------------------------------------------
  // Compute the penalty function (also called the Goodness-of-fit criteria).
  // For us, its the Chi-square function.
  //
  Double SteepestDescentSolver::getGOF(const VisBuffer& residual, Int& whichPol, Double& sumWt, const char* /*msg*/)
  {
    Double Chisq=0.0;
    Int nRow=residual.nRow();
    IPosition vbShape(residual.modelVisCube().shape());
    //Int nPoints;
    //nPoints=0;
    sumWt=0.0;

    IPosition ndx(3,0);
    ndx(0)=whichPol;
    for (ndx(2)=0;ndx(2)<nRow;ndx(2)++)
      {
	if (
//	    (!residual.flagCube()(ndx))   && 
	    (!residual.flag()(0,ndx(2)))  &&
	    (!residual.flagRow()(ndx(2))) &&
	    (residual.antenna1()(ndx(2)) != residual.antenna2()(ndx(2)))
	    )
	  {
	    DComplex Vis;
	    Float wt;
	    
	    Vis = residual.modelVisCube()(ndx);
	    wt  = residual.weight()(ndx(2));
/*
  	    cout << ndx(2)                      << " " 
 		 << Vis     << " " 
  		 << wt                          << " " 
 		 << residual.antenna1()(ndx(2)) << "-" << residual.antenna2()(ndx(2)) << " "
 		 << residual.flagCube()(ndx)    << " " 
 		 << residual.flag()(0,ndx(2))   << " "
 		 << residual.flagRow()(ndx(2))  << " "
 		 << endl;
*/
 	    sumWt += Double(wt);
	    Chisq += Double(wt*real(Vis*conj(Vis)));
	   // nPoints++;
	    /*
	    if (isnan(Chisq)) 
	      {
		ostringstream os;
		os << "###Error: Chisq evaluates to a NaN.  SumWt = " << sumWt 
		   << ndx(2) << " " << Vis << " " << wt << " " 
		   << residual.antenna1()(ndx(2)) << " " << residual.antenna2()(ndx(2));
		cout << residual.modelVisCube() << endl;
		throw(AipsError(os.str().c_str()));
	      }
	    */
	  }
      }
    //    exit(0);
    //    if (nPoints>0) sumWt /= nPoints;
    //cout << "CHISQ CHISQ = " << Chisq/sumWt << " " << Chisq << " " << sumWt << endl;
    /*
    ndx(0)=whichPol;
    for (ndx(2)=0;ndx(2)<nRow;ndx(2)++)
      //    for (ndx(2)=0;ndx(2)<2;ndx(2)++)
      {
	Int i = ndx(2),p=ndx(0);
	  if (
	      (!residual.flagRow()(ndx(2)))   && 
	      (!residual.flag()(0,ndx(2)))  &&
	      //	      (!residual.flagCube()(ndx)) &&
	      (residual.antenna1()(ndx(2)) != residual.antenna2()(ndx(2)))
	      )
	    cout << "SDS: Residual: " << msg << " " << i << " " << whichPol
		 << " " << getCurrentTimeStamp(residual)-4.46635e9
		 << " " << residual.modelVisCube()(0,0,i)
		 << " " << residual.modelVisCube()(1,0,i)
		 << " " << residual.visCube()(0,0,i)
		 << " " << residual.visCube()(1,0,i)
		 << " " << residual.antenna1()(i)<< "-" << residual.antenna2()(i) 
		 << " " << residual.flag()(0,i) 
		 << " " << residual.flagRow()(i) 
		 << " " << residual.flagCube()(p,0,i) 
		 << " " << residual.weight()(i)
		 << endl;
      }
    cout << "Chisq = " << Chisq << " " << sumWt << " " << Chisq/sumWt << endl;
    */
    //    exit(0);
    if (sumWt > 0) return Chisq/sumWt;
    else return 0.0;
  }
  //
  //---------------------------------------------------------------------------
  //Given the VisEquation, this iteratively sovles for the parameters
  //of the supplied VisJones for the time-stamp given by SlotNo.  nAnt
  //is the number of antennas per time stamp.
  //
  Double SteepestDescentSolver::solve(VisEquation& ve, EPJones& epj, VisBuffer& vb, 
				      Int nAnt, Int SlotNo)
  {
    Cube<Float> oldOffsets;
    Vector<Complex> ResidualVj, dAzVj, dElVj;
    Double Chisq0,Chisq,sumWt;
    //Double Time;
    //static Double Time0;
    Double AzHDiag,ElHDiag;
    Timer timer;
    Double localGain;
    Int iPol=0;
    IPosition ndx(2,2,nAnt);

    localGain = gain();
    ResidualVj.resize(nAnt);
    dAzVj.resize(nAnt);
    dElVj.resize(nAnt);

    ndx(0)=1;
    Chisq0=0.0;
    epj.guessPar(vb);
    epj.solveRPar() = -0.0001;
    residual_p=vb;
    residual_p.visCube().resize(vb.visCube().shape());
    residual_p.modelVisCube().resize(vb.modelVisCube().shape());

    residual_p.visCube() = vb.visCube();

    ve.diffResiduals(residual_p, gradient0_p, gradient1_p,flags);

    try
      {
	Chisq = getGOF(residual_p,iPol,sumWt);
      }
    catch (AipsError &x)
      {
	cout << x.getMesg() << endl;
	return -1;
      }

    if (sumWt == 0) return -Chisq;

    //Time = getCurrentTimeStamp(residual_p);
    //if (SlotNo == 0) Time0 = Time;

    timer.mark();
    Int iter;
    //Bool Converged=false;
    Vector<Bool> antFlagged;
    antFlagged.resize(nAnt);
    logIO() << LogOrigin("PointingCal","SDS::solve()") 
	    << "Solution interval = " << SlotNo 
	    << " Initial Chisq = " << Chisq 
	    << " SumOfWt = " << sumWt << LogIO::NORMAL3 << LogIO::POST;
    
    for (iter=0;iter<numberIterations();iter++)
      {
	oldOffsets = epj.solveRPar();//Guess;
	  {
	    for(Int ant=0;ant<nAnt;ant++)
	      {
		ResidualVj = getVj(residual_p, nAnt,ant,iPol,sumWt,0,0);//Get a weighted list
		dAzVj      = getVj(gradient0_p,nAnt,ant,iPol,sumWt,0,1);// Get an un-weighted list
		dElVj      = getVj(gradient1_p,nAnt,ant,iPol,sumWt, 0,1);

		antFlagged[ant]=false;
		if (sumWt > 0)
		  {
		    ndx(1)=ant;

		    Double coVar1, coVar2;
		    coVar1 = coVar2 = 0;
 		    if (sumWt > 0) coVar1 = real(innerProduct(dAzVj,dAzVj))/(sumWt*sumWt);
 		    if (sumWt > 0) coVar2 = real(innerProduct(dElVj,dElVj))/(sumWt*sumWt);
// 		    if (sumWt > 0) coVar1 = real(innerProduct(dAzVj,dAzVj));
// 		    if (sumWt > 0) coVar2 = real(innerProduct(dElVj,dElVj));
		    AzHDiag = ElHDiag = 0;
		    AzHDiag = 1.0/coVar1;
		    ElHDiag = 1.0/coVar2;

		    epj.solveRPar()(0,iPol,ant) = epj.solveRPar()(0,iPol,ant)-2*AzHDiag*localGain*
		      real(innerProduct(ResidualVj,dAzVj))/sumWt;
		    epj.solveRPar()(1,iPol,ant) = epj.solveRPar()(1,iPol,ant)-2*ElHDiag*localGain*
		      real(innerProduct(ResidualVj,dElVj))/sumWt;

//  		    epj.solveRPar()(0,0,ant) = epj.solveRPar()(0,0,ant)-2*AzHDiag*localGain*
//  		      real(innerProduct(ResidualVj,dAzVj));
//  		    epj.solveRPar()(1,0,ant) = epj.solveRPar()(1,0,ant)-2*ElHDiag*localGain*
//  		      real(innerProduct(ResidualVj,dElVj));
		  }
		else
		  antFlagged[ant]=true;

	      }
	    //
	    // Compute the residuals and the gradients with the
	    // updated solutions and check for convergence.
	    //
	    //	    epj.setAntPar(SlotNo,Guess);
	    ve.diffResiduals(residual_p,gradient0_p,gradient1_p,flags);
	    Chisq0 = Chisq;
	    //	    Chisq  = getGOF(residual,iPol,sumWt)/sumWt;
	    try
	      {
		Chisq  = getGOF(residual_p,iPol,sumWt);
	      }
	    catch (AipsError &x)
	      {
		logIO() << LogOrigin("PointingCal","SDS::solve()") 
			<< x.getMesg() 
			<< LogIO::POST;
		return -1;
	      }
	  }
	  Double dChisq;
	  dChisq = (Chisq0-Chisq);
	  //	  cout << iter << " " << Chisq0 << " " << Chisq << " " << dChisq << endl;
	  if ((fabs(dChisq) < tolerance()))
	    {
	      //Converged=true;
	      break;
	    }
	  if ((dChisq < tolerance()))// && (iter > 0))
	    {
	      localGain /= 2.0;
	      epj.solveRPar() = oldOffsets;
	      ve.diffResiduals(residual_p,gradient0_p,gradient1_p,flags);
	      //	      Chisq  = getGOF(residual,iPol,sumWt)/sumWt;
	      try
		{
		  Chisq  = getGOF(residual_p,iPol,sumWt);
		}
	      catch (AipsError &x)
		{
		  logIO() << LogOrigin("PointingCal","SDS::solve") 
			  << x.getMesg() 
			  << LogIO::POST;
		  return -1;
		}
	      Chisq0 = Chisq;
	    }
	  else
	    {
	      oldOffsets = epj.solveRPar();
	    }
      }
//     logIO() << LogOrigin("PointingCal","SDS::solve") 
// 	    << "Solution interval = " << SlotNo << " Final Chisq = " << Chisq 
// 	    << LogIO::NORMAL3 << LogIO::POST;

    //
    // Finalize the solutions in VisJones internal cache.
    //
    Chisq = fabs(Chisq0-Chisq);

    logIO() << LogIO::NORMAL3 << "No. of iterations used: " 
	    << iter
	    << " Time total, per iteration:  " 
	    << timer.all() << ", " << timer.all()/(iter+1) << " sec" << LogIO::POST;


    return (Chisq < tolerance()? Chisq:-Chisq);
  }

  Double SteepestDescentSolver::solve2(VisEquation& ve, VisIter& vi, EPJones& epj, 
				       Int nAnt, Int SlotNo)
  {
    Cube<Float> bestSolution;
    Vector<Complex> ResidualVj, dAzVj, dElVj;
    Vector<Bool> antFlagged;
    Double Chisq = MAXFLOAT;
    Double dChisq, bestChisq, sumWt, /*Time,*/ AzHDiag, ElHDiag,localGain, coVar1, coVar2, 
      stepSize0,stepSize1;
    Int iPol=0, nPol, iter, axis0=0,axis1=0, iCorr;
    Bool Converged=false;
    Timer timer;

    nPol=polMap_p.nelements();

    ResidualVj.resize(nAnt);
    dAzVj.resize(nAnt);
    dElVj.resize(nAnt);
	
    epj.guessPar();
    for(Int ia=0;ia<nAnt;ia++)
      {

	// epj.solveRPar()(0,0,ia) = -0.001;
	// epj.solveRPar()(2,0,ia) =  0.001;
	// epj.solveRPar()(1,0,ia) =  0.001;
	// epj.solveRPar()(3,0,ia) = -0.001;
	//
	// Solved values for the squint
	//
	// epj.solveRPar()(0,0,ia) = -0.000139;
	// epj.solveRPar()(2,0,ia) =  5.25e-6;
	// epj.solveRPar()(1,0,ia) =  4.96e-5;
	// epj.solveRPar()(3,0,ia) = -0.000139;
      }
    for(Int ip=0;ip<nPol;ip++)
      {
	Converged=false;
	localGain = gain();

	iPol=polMap_p[ip];
	if (iPol >= 0) 
	  {
	    iCorr=iPol;
	    iCorr=iPol=ip;
	    //@#$%@#$%@#%@#%@#%@#%@#%
	    // iPol = ip;
	    //@#$%@#$%@#%@#%@#%@#%@#%
	    
	    epj.diffResiduals(vi,ve,residual_p, gradient0_p, gradient1_p, flags);
	    Chisq = getGOF(residual_p,iCorr,sumWt,"Init");
	    if (sumWt == 0) return -Chisq;
	    
	    bestChisq = Chisq;
	    bestSolution.resize(epj.solveRPar().shape());
	    bestSolution = epj.solveRPar();	
	    //epj.guessPar();
	    //Time = getCurrentTimeStamp(residual_p);
	    timer.mark();
	    antFlagged.resize(nAnt);
	    logIO() << LogOrigin("PointingCal","SDS::solve2") 
		    << "Solution interval = " << SlotNo 
		    << " Initial Chisq = " << Chisq 
		    << " Polarization = " << iPol 
		    << " SumOfWt = " << sumWt 
		    << LogIO::NORMAL << LogIO::POST;
	    if (iPol==0) {axis0=0;axis1=2;}
	    else {axis0=1; axis1=3;}
	    if (iPol==0) {axis0=1;axis1=3;}
	    else {axis0=0; axis1=2;}
	    iter=-1;
	    for (iter=0;iter<numberIterations();iter++)
	      {
		//
		// Update the current position
		//
		for(Int ant=0;ant<nAnt;ant++)
		  {
		    // cerr << "Init: " << ant << " " 
		    // 	 << epj.solveRPar()(axis0,0,ant) << "  "
		    // 	 << epj.solveRPar()(axis1,0,ant) << endl;
		    ResidualVj = getVj(residual_p, nAnt,ant,iCorr,sumWt,0,0);
		    dAzVj      = getVj(gradient0_p,nAnt,ant,iCorr,sumWt,0,1);
		    dElVj      = getVj(gradient1_p,nAnt,ant,iCorr,sumWt,0,1);
		    
		    antFlagged[ant]=false;
		    if (sumWt > 0)
		      {
			stepSize0 = stepSize1 = coVar1 = coVar2 = 0;
			if (sumWt > 0) 
			  {
			    coVar1 = real(innerProduct(dAzVj,dAzVj))/(sumWt*sumWt);
			    coVar2 = real(innerProduct(dElVj,dElVj))/(sumWt*sumWt);
			  }
			AzHDiag = ElHDiag = 0;
			
			if (coVar1 > 0)
			  {
			    AzHDiag  = 1.0/coVar1;
			    stepSize0 = real(innerProduct(ResidualVj,dAzVj))/sumWt;
			    //			    cerr << "Ant:"  << ant << " " << stepSize0 << " ";
			    stepSize0 *= -2*AzHDiag*localGain;
			    //			    cerr << stepSize0 << "    ";
			    epj.solveRPar()(axis0,0,ant) = epj.solveRPar()(axis0,0,ant) + stepSize0;
			    //			epj.solveRPar()(axis0,0,ant) = 0.001;
			  }
			if (coVar2 > 0)
			  {
			    ElHDiag  = 1.0/coVar2;
			    stepSize1 = real(innerProduct(ResidualVj,dElVj))/sumWt;
			    //			    cerr << stepSize1 << " ";
			    stepSize1 *= -2*ElHDiag*localGain;
			    epj.solveRPar()(axis1,0,ant) = epj.solveRPar()(axis1,0,ant) + stepSize1;
			    // cerr << stepSize1 << "     " 
			    // 	 << epj.solveRPar()(axis0,0,ant) << "  "
			    // 	 << epj.solveRPar()(axis1,0,ant) << endl;
			    //			epj.solveRPar()(axis1,0,ant) = -0.001;
			  }
			/*
			  cout << "SDS Sol Loop = " 
			  << iPol << " " << iter << " " << ant << " " << axis0 << " " << axis1 << " " 
			  << epj.solveRPar()(axis0,0,ant) << " "
			  << epj.solveRPar()(axis1,0,ant) << " "
			  << stepSize0 << " " << stepSize1 << " "
			  << endl;
			*/
		      }
		    else
		      {
			antFlagged[ant]=true;
			epj.solveRPar()(axis0,0,ant)=0.0;//0.001;
			epj.solveRPar()(axis1,0,ant)=0.0;//-0.001;
		      }
		    //epj.solveRPar()(axis0,0,ant)=0.001;
		    //epj.solveRPar()(axis1,0,ant)=-0.001;
		    
		  }
		//
		// Compute the residuals and the gradients with the
		// updated solutions and check for convergence.
		//
		epj.diffResiduals(vi,ve,residual_p, gradient0_p, gradient1_p, flags);
		Chisq  = getGOF(residual_p,iCorr,sumWt,"Final");
		/*
		  for(Int kk=0;kk<nAnt;kk++)
		  cout << "SDS Sol Iter = " 
		  << iPol << " " << iter << " " << axis0 << " " << axis1 << " " << kk << " "
		  << epj.solveRPar()(axis0,0,kk) << " "
		  << epj.solveRPar()(axis1,0,kk) 
		  << Chisq << " " << bestChisq
		  << endl;
		*/
		dChisq = (bestChisq-Chisq);
		/*
		  cout << iter << " " << iPol << " " << Chisq << " " << bestChisq
		  << " " << dChisq << " " << " " << bestChisq << " " << localGain << endl;
		*/
		
		if ((fabs(dChisq) < tolerance()))
		  {
		    Converged=true;
		    epj.setRPar(bestSolution);
		    Chisq = bestChisq;
		    break;
		  }
		if (Chisq >= bestChisq)
		  {
		    // Backtracking....
		    localGain /= 2.0;
		    epj.setRPar(bestSolution); //oldOffsets;
		    
		    // 		IPosition shp(bestSolution.shape());
		    // 		for(Int ii=0;ii<shp(2);ii++)
		    // 		  for(Int jj=0;jj<shp(0);jj++)
		    // 		    cout << "BT: " << bestSolution(jj,0,ii) << " " 
		    // 			 << epj.solveRPar()(jj,0,ii) << endl;
		    
		    epj.diffResiduals(vi,ve,residual_p, gradient0_p, gradient1_p, flags);
		    
		    Chisq  = getGOF(residual_p,iCorr,sumWt,"BACK");
		    
		    // 		cout << "BT Chisq: " << Chisq << " " 
		    // 		     << sum(bestSolution-epj.solveRPar()) 
		    // 		     << " " << residual.nRow() << endl;
		    // 		if (Chisq != bestChisq)
		    // 		  {
		    // 		    IPosition ndx(3,0);
		    
		    // 		    for(Int i=0;i<residual.nRow();i++)
		    // 		      {
		    // 			ndx(2)=i;
		    // 			//        if (residual.flag()(0,i) == 0)
		    // 			for(Int xx=0;xx<2;xx++)
		    // 			  {
		    // 			    ndx(0)=xx;
		    // 			    if (
		    // 				(!residual.flagCube()(ndx))   &&
		    // 				(!residual.flag()(xx,ndx(2)))  &&
		    // 				(!residual.flagRow()(ndx(2))) &&
		    // 				(residual.antenna1()(ndx(2)) != residual.antenna2()(ndx(2)))
		    // 				)
		    // 			      cout << xx<<"*";
		    // 			  }
		    
		    // 			cout << "SDS: Residual: " << i    
		    // 			     << " " << residual.modelVisCube()(0,0,i) 
		    // 			     << " " << residual.modelVisCube()(1,0,i)
		    // 			     << " " << residual.visCube()(0,0,i) 
		    // 			     << " " << residual.visCube()(1,0,i)
		    // 			     << " " << residual.flagRow()(i) 
		    // 			     << " " << residual.flag()(0,i) 
		    // 			     << " " << residual.flag()(1,i) 
		    // 			     << " " << residual.flagCube()(0,0,i) 
		    // 			     << " " << residual.flagCube()(1,0,i) 
		    // 			     << " " << residual.antenna1()(i) 
		    // 			     << " " << residual.antenna2()(i) 
		    // 			     << endl;
		    // 		      }
		    // 		    exit(0);
		    // 		  }
		  }
		else
		  {
		    bestSolution = epj.solveRPar();
		    bestChisq = Chisq;
		  }
		if (localGain < 1e-20) {Converged = true; break;};
	      }
	    //         epj.solveRPar() = bestSolution;	
	    // 	{
	    // 	  IPosition shp(bestSolution.shape());
	    // 	  cout << "-------------------------------------------------------------------------" << endl;
	    // 	  for(Int ii=0;ii<shp(2);ii++)
	    // 	    {
	    // 	      cout << "Sol: ";
	    // 	      for(Int jj=0;jj<shp(0);jj++)
	    // 		cout << ii << " " << jj << " " << bestSolution(jj,0,ii);
	    // 	      cout << " " << endl;
	    // 	    }
	    // 	}
	    epj.diffResiduals(vi,ve,residual_p,gradient0_p,gradient1_p,flags);
	    Chisq = getGOF(residual_p,iCorr,sumWt);
	    logIO() << LogOrigin("PointingCal","SDS::solve2") 
		    << "Solution interval = " << SlotNo 
		    << " Final Chisq = " << Chisq 
		    << " Polarization = " << iPol 
		    << " NIter = " << iter
		    << LogIO::NORMAL << LogIO::POST;
	    logIO() << LogIO::NORMAL3 << "No. of iterations used: " 
		    << iter
		    << " Time total, per iteration:  " 
		    << timer.all() << ", " << timer.all()/(iter+1) << " sec" << LogIO::POST;
	    
	  } // if (iPol >= 0)
      } // for(Int ip=0; ip<nPol; ip++)
    //    return (Chisq < tolerance()? Chisq:-Chisq);
    return (Converged ? Chisq: -Chisq);
  }
  
};
