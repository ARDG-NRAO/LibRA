//# CEMemProgress.cc:  monitor progress in Mem
//# Copyright (C) 1997,1998,1999,2000,2001,2003
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
//# $Id$


//# Includes
#include <synthesis/MeasurementEquations/CEMemProgress.h>
#include <casacore/images/Images/TempImage.h>
#include <casacore/casa/System/PGPlotter.h>
#include <casacore/casa/BasicMath/Math.h>

#include <sstream>


using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

CEMemProgress::CEMemProgress(PGPlotter* pgplotter)
  : itsPgplotter(pgplotter),
    itsCurrentIndex(0),
    itsCurrentTotalIterations(0),
    itsCurrentFluxScale(0.0),
    itsCurrentMaxResidualScale(0.0),
    itsCurrentSigmaScale(0.0),    
    itsCurrentNormGradScale(0.0),    
    itsCurrentEntropyScale(0.0)
{
  itsIterationNumbers.resize(100);
  itsMaxResiduals.resize(100);  
  itsTotalFluxes.resize(100);  
  itsSigmas.resize(100);
  itsNormGrads.resize(100);
  itsEntropies.resize(100);

  // to prevent trailing vector elements from being plotted
  itsIterationNumbers = 999999999;
}

CEMemProgress::~CEMemProgress()
{
  //  if(itsPgplotter) delete itsPgplotter; itsPgplotter=0;
}

// NOTE:  currently, we only display totalFlux, sigma, and maxResidual.
// normGrad and entropy are not displayed, but their vectors and scales
// are fully processed in case we want to diplay them in the future.
//
  void CEMemProgress::info(const Bool /*lastcall*/,
			 const Int iteration,
			 const Int numberIterations,
                           const Lattice<Float>& /*model*/,
                           const Lattice<Float>& /*resid*/,
			 const Float& maxResid,
                           const IPosition& /*posMaxResid*/,
			 const Float& totalFlux,
			 const Float& sigma,
			 const Float& normGrad,
			 const Float& entropy)
{

  LogIO os(LogOrigin("CEMemProgress", "info()", WHERE));


  if(itsCurrentIndex == 0) {
    if(itsPgplotter) {
      
      itsCurrentFluxScale = 2.0*(max(maxResid, totalFlux));
      itsCurrentMaxResidualScale = 1.1*maxResid;
      itsCurrentSigmaScale = 1.1*sigma;
      itsCurrentTotalIterations = numberIterations;
      itsCurrentEntropyScale = entropy;
      itsCurrentNormGradScale =  10.0*normGrad;
      basicSetUp(numberIterations);

    }
  }

  if (itsCurrentIndex >= itsTotalFluxes.nelements() ) {
    uInt nn = itsTotalFluxes.nelements();
    
    itsTotalFluxes.resize(2*nn+1, true);
    itsMaxResiduals.resize(2*nn+1, true);
    itsSigmas.resize(2*nn+1, true);
    itsNormGrads.resize(2*nn+1, true);
    itsEntropies.resize(2*nn+1, true);


    Vector<Float> inr(itsIterationNumbers);
    itsIterationNumbers.resize(2*nn+1);
    // to prevent trailing vector elements from being plotted
    itsIterationNumbers = 99999999;
    for (uInt i=0;i<nn;i++) {
      itsIterationNumbers(i) = inr(i);
    }
  }

  itsTotalFluxes(itsCurrentIndex) = totalFlux;
  itsMaxResiduals(itsCurrentIndex) = maxResid;
  itsIterationNumbers(itsCurrentIndex) = iteration + 1;
  itsSigmas(itsCurrentIndex) = sigma;
  itsNormGrads(itsCurrentIndex) = normGrad;
  itsEntropies(itsCurrentIndex) = entropy;
  itsCurrentIndex++;

  if(itsPgplotter) {
    Bool rePlot = false;
    if ( totalFlux > (0.9*itsCurrentFluxScale)) {
      rePlot = true;
      itsCurrentFluxScale *= 2.0;
    }
    if ( numberIterations > (Int)itsCurrentTotalIterations) {
      itsCurrentTotalIterations = numberIterations;
      rePlot = true;
    }
    // If we ever plot normGrad, we will need to test/rest its
    // Scale and rePlot


    if (rePlot) {      
      basicSetUp(true);      
    } else {
      plotOne(iteration+1, sigma, maxResid, totalFlux);
    }
  }  
};



void  CEMemProgress::basicSetUp(Bool doPlot)
{

  Float xMax = Float(itsCurrentTotalIterations)*1.15;
  Float xMin = -0.05*Float(itsCurrentTotalIterations);

if(itsPgplotter){
  itsPgplotter->sch(0.6);
  itsPgplotter->sci(1);
  itsPgplotter->page();
  itsPgplotter->svp(0.06, 0.94, 0.64, 0.92);
  itsPgplotter->swin(xMin, xMax, 0.0, itsCurrentSigmaScale);
  itsPgplotter->box("BCST", 0, 0, "BCNST", 0, 0);
  itsPgplotter->lab(" ", "sigma", "MEM Iteration Progress");
  itsPgplotter->iden();
  {
    itsPgplotter->sci(1);
    ostringstream oos;
    oos << "Sigma ";
    itsPgplotter->text(0.85*xMax,
                       (0.9*itsCurrentSigmaScale), oos);
  }
  if (doPlot) {
    itsPgplotter->pt(itsIterationNumbers, itsSigmas, 2);
  }


  // middle graph
  itsPgplotter->sci(1);
  itsPgplotter->svp(0.06, 0.94, 0.36, 0.64);
  itsPgplotter->swin(xMin, xMax, 0.0, itsCurrentMaxResidualScale);
  itsPgplotter->box("BCST", 0, 0, "BCNST", 0, 0);
  itsPgplotter->lab(" ", "Peak Resid (Jy)", " ");
  {
    ostringstream oos;
    oos << "MaxRes ";
    itsPgplotter->text(0.85*xMax,
		       (0.9*itsCurrentMaxResidualScale), oos);
  }
  if (doPlot) {
    itsPgplotter->pt(itsIterationNumbers, itsMaxResiduals, 2);
  }

  // lower graph
  itsPgplotter->sci(3);
  itsPgplotter->svp(0.06, 0.94, 0.09, 0.36);
  itsPgplotter->swin(xMin, xMax, 0.0, itsCurrentFluxScale);
  itsPgplotter->box("BCNST", 0, 0, "BCNST", 0, 0);
  itsPgplotter->lab("Number of iterations", "Total Flux", " ");
  {
    ostringstream oos;
    oos << "Total Flux ";
    itsPgplotter->text(0.85*xMax,
		       (0.9*itsCurrentFluxScale), oos);
  }
  if (doPlot) {
    itsPgplotter->pt(itsIterationNumbers, itsTotalFluxes, 2);
  }
}


};


void  CEMemProgress::plotOne(const Int iteration, 
			     const Float sigma, const Float resid, 
			     const Float flux)
{

  Float xMax = Float(itsCurrentTotalIterations)*1.15;
  Float xMin = -0.05*Float(itsCurrentTotalIterations);

  Vector<Float> x(1);
  Vector<Float> y(1);
  x(0) = iteration;
if(itsPgplotter){
  // top graph
  itsPgplotter->sch(0.6);
  itsPgplotter->sci(1);
  itsPgplotter->svp(0.06, 0.94, 0.64, 0.92);
  itsPgplotter->swin(xMin, xMax, 0.0, itsCurrentSigmaScale);
  y(0) = sigma;
  itsPgplotter->pt(x,y,2);

  // middle graph
  itsPgplotter->sci(1);
  itsPgplotter->svp(0.06, 0.94, 0.36, 0.64);
  itsPgplotter->swin(xMin, xMax, 0.0, itsCurrentMaxResidualScale);
  y(0) = resid;
  itsPgplotter->pt(x,y,2);

  // lower graph
  itsPgplotter->sci(3);
  itsPgplotter->svp(0.06, 0.94, 0.09, 0.36);
  itsPgplotter->swin(xMin, xMax, 0.0, itsCurrentFluxScale);
  y(0) = flux;
  itsPgplotter->pt(x,y,2);
}
};



} //# NAMESPACE CASA - END

