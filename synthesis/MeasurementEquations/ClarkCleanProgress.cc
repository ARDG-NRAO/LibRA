//# ClarkCleanProgress.cc: Abstract base class to monitor progress in lattice operations
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
#include <synthesis/MeasurementEquations/ClarkCleanProgress.h>
#include <images/Images/TempImage.h>
#include <casa/System/PGPlotter.h>
#include <casa/BasicMath/Math.h>
#include <casa/Exceptions/Error.h>

#include <casa/sstream.h>


using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

ClarkCleanProgress::ClarkCleanProgress(PGPlotter* pgplotter, Int inc)
  : itsPgplotter(pgplotter),
    currentIndex(0),
    currentTotalIterations(0),
    currentFluxScale(0.0),
    currentMaxResidual(0.0),
    currentMinResidual(0.0),
    plottingIncrement(inc),
    baseFlux(0.0)
{
  iterationNumber.resize(100);
  maxResiduals.resize(100);  
  posResiduals.resize(100);
  negResiduals.resize(100);
  totalFluxes.resize(100);  

  // to prevent trailing vector elements from being plotted
  iterationNumber = 99999999;
}


ClarkCleanProgress::~ClarkCleanProgress()
{
}


// Call back function
Bool ClarkCleanProgress::info(const Bool lastcall,
			      const Int iteration,
			      const Int numberIterations,
			      const Float& maxResid,
			      const IPosition& posMaxResid,
			      const Float& totalFlux0,
			      const Bool majorIteration,
			      const Bool resetBaseFlux) 
{
  LogIO os(LogOrigin("ClarkCleanProgress", "info()", WHERE));

  if(currentIndex == 0) {
    if(hasPGPlotter()) {      
      currentFluxScale = 1.5*maxResid;
      currentMinFluxScale = 0.0;
      currentMaxResidual = 1.5*maxResid;
      currentMinResidual = currentMaxResidual/100;
      currentTotalIterations = numberIterations;
      basicSetUp(numberIterations);
    }
  }

  if (resetBaseFlux && currentIndex != 0) {
    baseFlux = totalFluxes(currentIndex-1);
  }
  Float totalFlux = totalFlux0 + baseFlux;

  // resize if required
  if (currentIndex >= totalFluxes.nelements() ) {
    uInt nn = totalFluxes.nelements(); 
    totalFluxes.resize(2*nn+1, true);
    maxResiduals.resize(2*nn+1, true);
    negResiduals.resize(2*nn+1, true);
    posResiduals.resize(2*nn+1, true);
    iterationNumber.resize(2*nn+1, true);
    // Do this so trailing (ie, unfilled) elements don't get plotted
    for (uInt i=nn; i < 2*nn+1; i++) {
      iterationNumber(i) = 9999999;
    }
  }

  totalFluxes(currentIndex) = totalFlux;
  maxResiduals(currentIndex) = maxResid;
  iterationNumber(currentIndex) = iteration+1;

  if (maxResid > 0) {
    posResiduals(currentIndex) = log10(maxResid);
    negResiduals(currentIndex) = -20;  // ie, out of range
  } else if (maxResid < 0) {
    negResiduals(currentIndex) = log10(abs(maxResid));
    posResiduals(currentIndex) = -20;
  }
  Float myMinFlux = min (0.0, totalFlux);

  currentIndex++;

  if(hasPGPlotter()) {
    if ( lastcall || ( (iteration) % plottingIncrement) == 0) {
      // Then we get to do a plot

      Bool redrawBox = false;

      if ( totalFlux > (0.9*currentFluxScale)) {
	currentFluxScale = abs(3.0 * currentFluxScale);
	redrawBox = true;
      }
      if (myMinFlux < currentMinFluxScale) {
        currentMinFluxScale = -abs( 3.0 * myMinFlux);
        redrawBox = true;
      }
      if (abs(maxResid) < (1.2*currentMinResidual)) {
	currentMinResidual /= 3.0;
	redrawBox = true;
      }
      if ( numberIterations > (Int)currentTotalIterations) {
	currentTotalIterations = numberIterations;
	redrawBox = true;
      }

      if (redrawBox || lastcall || resetBaseFlux) {
	basicSetUp(true);      
	plotVectors();
      } else {
	plotOne(iteration+1, maxResid, totalFlux);
      }
    }
  }

  if (majorIteration) {
    os << "Max Resid = " << maxResid << " at "
       <<  posMaxResid << endl;
    os << "Iteration " << iteration+1 << 
      "   flux [Jy] = " << totalFlux << endl;
  }

  return true;
};

// Call back function
Bool ClarkCleanProgress::finalize()

{
  LogIO os(LogOrigin("ClarkCleanProgress", "info()", WHERE));

  basicSetUp(true);      
  plotVectors();

  os << "Max Resid = " << maxResiduals(currentIndex-1)<<  endl;
  os << "Iteration " <<  iterationNumber(currentIndex-1) << 
    "   flux [Jy] = " << totalFluxes(currentIndex-1) << endl;

  return true;
};

// checks to see if the pointer is non null;
// also checks to see if it is attached to a plot, and
// checks to see if the PGPlotter pointer is dangling
Bool ClarkCleanProgress::hasPGPlotter() 
{
  if (itsPgplotter == 0) {
    return false;
  } else {
    try {
      if (itsPgplotter->isAttached()) {
	return true;
      } else {
	return false;
      }
    } catch (AipsError x) {
      return false;
    } 
  }
  return false;
};


void  ClarkCleanProgress::basicSetUp(Bool doPlot)
{

  Float logMinRes = log10(abs(currentMinResidual));
  Float logMaxRes = log10(abs(currentMaxResidual));
  Float deltaY = abs(logMaxRes - logMinRes);
  logMaxRes += 0.05*deltaY;
  logMinRes -= 0.05*deltaY;
  Float xMax = Float(currentTotalIterations)*1.15;
  Float xMin = -0.05*Float(currentTotalIterations);

  if(itsPgplotter){

  itsPgplotter->sch(0.6);
  itsPgplotter->sci(1);
  itsPgplotter->page();
  itsPgplotter->svp(0.06, 0.94, 0.64, 0.92);
  itsPgplotter->swin(xMin, xMax, logMinRes, logMaxRes);
  itsPgplotter->box("BCST", 0, 0, "BCNLST", 0, 0);
  itsPgplotter->lab(" ", "+ Peak Resid (Jy)", "Components subtracted");
  itsPgplotter->iden();
  {
    itsPgplotter->sci(1);
    ostringstream oos;
    oos << "MaxRes ";
    itsPgplotter->text(0.85*xMax,
                       (logMaxRes - 0.1*deltaY),
                       oos);
  }
  if (doPlot) {
    itsPgplotter->pt(iterationNumber, posResiduals, 2);
  }


  // middle graph
  itsPgplotter->sci(1);
  itsPgplotter->svp(0.06, 0.94, 0.36, 0.64);
  itsPgplotter->swin(xMin, xMax, logMaxRes, logMinRes);
  itsPgplotter->box("BCST", 0, 0, "BCNLST", 0, 0);
  itsPgplotter->lab(" ", "- Peak Resid (Jy)", " ");
  {
    ostringstream oos;
    oos << "-MaxNegRes ";
    itsPgplotter->text(0.85*xMax,
                       (logMaxRes - 0.1*deltaY),
                       oos);
  }
  if (doPlot) {
    itsPgplotter->pt(iterationNumber, negResiduals, 2);
  }

  // lower graph
  itsPgplotter->sci(3);
  itsPgplotter->svp(0.06, 0.94, 0.09, 0.36);
  itsPgplotter->swin(xMin, xMax, currentMinFluxScale, currentFluxScale);
  itsPgplotter->box("BCNST", 0, 0, "BCNST", 0, 0);
  itsPgplotter->lab("Number of iterations", "Total Flux", " ");
  if (doPlot) {
    itsPgplotter->pt(iterationNumber, totalFluxes, 2);
  }

  }

};

void ClarkCleanProgress::plotVectors()
{
  Float logMinRes = log10(abs(currentMinResidual));
  Float logMaxRes = log10(abs(currentMaxResidual));
  Float deltaY = abs(logMaxRes - logMinRes);
  logMaxRes += 0.05*deltaY;
  logMinRes -= 0.05*deltaY;
  Float xMax = Float(currentTotalIterations)*1.15;
  Float xMin = -0.05*Float(currentTotalIterations);

  if(itsPgplotter){
  itsPgplotter->sch(0.6);

    // top graph
    itsPgplotter->sci(1);
    itsPgplotter->svp(0.06, 0.94, 0.64, 0.92);
    itsPgplotter->swin(xMin, xMax, logMinRes, logMaxRes);
    itsPgplotter->pt(iterationNumber, posResiduals, 2);

    // middle graph
    itsPgplotter->sci(1);
    itsPgplotter->svp(0.06, 0.94, 0.36, 0.64);
    itsPgplotter->swin(xMin, xMax, logMaxRes, logMinRes);
    itsPgplotter->pt(iterationNumber, negResiduals, 2);


  // lower graph
  itsPgplotter->sci(3);
  itsPgplotter->svp(0.06, 0.94, 0.09, 0.36);
  itsPgplotter->swin(xMin, xMax, 0.0, currentFluxScale);
  itsPgplotter->pt(iterationNumber, totalFluxes, 2);
  }
};



void  ClarkCleanProgress::plotOne(const Int iteration, 
				  const Float resid, const Float flux)
{

  Float logMinRes = log10(abs(currentMinResidual));
  Float logMaxRes = log10(abs(currentMaxResidual));
  Float deltaY = abs(logMaxRes - logMinRes);
  logMaxRes += 0.05*deltaY;
  logMinRes -= 0.05*deltaY;
  Float xMax = Float(currentTotalIterations)*1.15;
  Float xMin = -0.05*Float(currentTotalIterations);

  Vector<Float> x(1);
  Vector<Float> y(1);
  x(0) = iteration;
  if(itsPgplotter){
  itsPgplotter->sch(0.6);
  if (resid > 0) {
    // top graph
    itsPgplotter->sci(1);
    itsPgplotter->svp(0.06, 0.94, 0.64, 0.92);
    itsPgplotter->swin(xMin, xMax, logMinRes, logMaxRes);
    y(0) = log10(resid);
    itsPgplotter->pt(x,y,2);
  } else if (resid < 0) {
    // middle graph
    itsPgplotter->sci(1);
    itsPgplotter->svp(0.06, 0.94, 0.36, 0.64);
    itsPgplotter->swin(xMin, xMax, logMaxRes, logMinRes);
    y(0) = log10(abs(resid));
    itsPgplotter->pt(x,y,2);
  }

  // lower graph
  itsPgplotter->sci(3);
  itsPgplotter->svp(0.06, 0.94, 0.09, 0.36);
  itsPgplotter->swin(xMin, xMax, 0.0, currentFluxScale);
  y(0) = flux;
  itsPgplotter->pt(x,y,2);
  }
};


} //# NAMESPACE CASA - END

