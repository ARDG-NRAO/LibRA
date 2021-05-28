//# ClarkCleanProgress.h: Abstract base class to monitor progress in lattice operations
//# Copyright (C) 1997,1998,1999,2000
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

#ifndef SYNTHESIS_CLARKCLEANPROGRESS_H
#define SYNTHESIS_CLARKCLEANPROGRESS_H
 
//# Includes
#include <casacore/casa/aips.h>
#include <casacore/casa/Quanta/Quantum.h>
#include <casacore/lattices/Lattices/Lattice.h>
#include <casacore/lattices/Lattices/TempLattice.h>
#include <casacore/casa/Arrays/IPosition.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/ArrayFwd.h>

namespace casacore{

class PGPlotter;
// the class ProgressMeter sounded bad. This abstract class serves
}

namespace casa { //# NAMESPACE CASA - BEGIN

//# Forward Declarations

// <summary>
// Abstract base class to monitor progress in lattice operations
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <synopsis>
// This is an abstract base class for classes to monitor the
// progress of an operation on a Lattice. The default implementation
// offered by this class does nothing.
// However, a derived class could show the progress using for example
// a <linkto class=casacore::ProgressMeter>ProgressMeter</linkto>. A derived
// class should override the virtual functions from this class.
//
// The user of the ClarkCleanProgress object should first call
// function <src>init</src> with the total number of steps
// that are to be done.   Thereafter, after each step has been
// executed, function <src>nstepsDone</src> should be called
// after each step.  Finally, function <src>done</src> should
// be called.
// </synopsis>

// <example>
// <srcblock>
// </srcblock>
// </example>

// <motivation>
// Since operations on Lattices can take a while, it can be useful
// to show the progress. However, making module Lattices dependent on
// as a bridge between the casacore::Lattice module and the casacore::ProgressMeter class
// (or any other class showing the progress).
// </motivation>
//
//# <todo asof="1997/08/01">   
//#   <li> 
//# </todo>


class ClarkCleanProgress {
public:
  ClarkCleanProgress(casacore::PGPlotter* pgplotter=0, casacore::Int plotterIncrement=10);

  // delete its PGPlotter
  virtual ~ClarkCleanProgress();
  
  // Plot the information if it has a PGPlotter
  casacore::Bool info(const casacore::Bool lastcall,
	    const casacore::Int iteration,
	    const casacore::Int numberIterations,
	    const casacore::Float& maximum,
	    const casacore::IPosition& posMaximum,
	    const casacore::Float& totalFlux,
	    const casacore::Bool majorIteration,
	    const casacore::Bool resetBase=false);
  
  // Finish up any aspects of the plot which need to be finished
  casacore::Bool finalize();

  // returns true if we have a live pgplotter_pointer
  // Use: do a     try { progress_pointer->hasPGPlotter(); }
  casacore::Bool hasPGPlotter();

protected:

private:

  // draw the outline of the plot;
  // If doplot = true, redraw all the past data as well.
  // THIS ASSUMES WE HAVE A VALID PGPLOTTER
  void basicSetUp(casacore::Bool doPlot = false);

  // plot just the current points
  // THIS ASSUMES WE HAVE A VALID PGPLOTTER
  void plotOne(const casacore::Int iteration, 
	       const casacore::Float resid, const casacore::Float flux);

  // replot all points, but don't redraw plotter
  // THIS ASSUMES WE HAVE A VALID PGPLOTTER
  void plotVectors();

  casacore::PGPlotter* itsPgplotter;

  casacore::Vector<casacore::Float> iterationNumber;   	
  casacore::Vector<casacore::Float> maxResiduals;		
  casacore::Vector<casacore::Float> posResiduals;		
  casacore::Vector<casacore::Float> negResiduals;		
  casacore::Vector<casacore::Float> totalFluxes;
  casacore::uInt currentIndex;
  casacore::uInt currentTotalIterations;
  casacore::Float currentFluxScale;
  casacore::Float currentMinFluxScale;
  casacore::Float currentMaxResidual;
  casacore::Float currentMinResidual;

  // we are going to plot all points, but not all at once; every Increment iterations
  casacore::Int plottingIncrement;

  casacore::Float baseFlux;

};



} //# NAMESPACE CASA - END

#endif
