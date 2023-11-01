//# CEMemProgress.h:  monitor progress in Mem
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

#ifndef SYNTHESIS_CEMEMPROGRESS_H
#define SYNTHESIS_CEMEMPROGRESS_H
 
//# Includes
#include <casacore/casa/aips.h>
#include <casacore/casa/Quanta/Quantum.h>
#include <casacore/lattices/Lattices/Lattice.h>
#include <casacore/lattices/Lattices/TempLattice.h>
#include <casacore/casa/Arrays/IPosition.h>
#include <casacore/casa/Arrays/Vector.h>

namespace casacore{

class PGPlotter;
// the class ProgressMeter sounded bad. This abstract class serves
}

namespace casa { //# NAMESPACE CASA - BEGIN

//# Forward Declarations

// <summary>
// Class to monitor progress in MEM deconvolution
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <synopsis>
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


class CEMemProgress {
public:

  enum EntType {
    ENTROPY=0,
    EMPINESS=1
  };
  


  CEMemProgress(casacore::PGPlotter* pgplotter=0);

  virtual ~CEMemProgress();
  
 // people usually don't worry about maximum residual with MEM; we'll do it, though

  void info(const casacore::Bool lastcall,
	    const casacore::Int iteration,
	    const casacore::Int numberIterations,
	    const casacore::Lattice<casacore::Float>& model,
	    const casacore::Lattice<casacore::Float>& resid,
	    const casacore::Float& maximumRes,         
	    const casacore::IPosition& posMaximum,
	    const casacore::Float& totalFlux,
	    const casacore::Float& sigma,
	    const casacore::Float& normGrad,
	    const casacore::Float& entropy);
  
protected:

private:

  void basicSetUp(casacore::Bool doPlot = false);

  void plotOne(const casacore::Int iteration, 
	       const casacore::Float sigma, const casacore::Float maxResid, 
	       const casacore::Float flux);

  casacore::PGPlotter* itsPgplotter;

  casacore::Vector<casacore::Float> itsIterationNumbers;
  casacore::Vector<casacore::Float> itsTotalFluxes;
  casacore::Vector<casacore::Float> itsMaxResiduals;
  casacore::Vector<casacore::Float> itsSigmas;
  casacore::Vector<casacore::Float> itsNormGrads;
  casacore::Vector<casacore::Float> itsEntropies;
  casacore::uInt itsCurrentIndex;
  casacore::uInt itsCurrentTotalIterations;
  casacore::Float itsCurrentFluxScale;
  casacore::Float itsCurrentMaxResidualScale;
  casacore::Float itsCurrentSigmaScale;        
  casacore::Float itsCurrentNormGradScale;        
  casacore::Float itsCurrentEntropyScale;        
};



} //# NAMESPACE CASA - END

#endif
