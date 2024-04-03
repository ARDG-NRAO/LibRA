//# Predict.h: Parallel model visibility prediction
//# Copyright (C) 1996,1997,1998,1999,2000,2002
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

#ifndef SYNTHESIS_PREDICTALGORITHM_H
#define SYNTHESIS_PREDICTALGORITHM_H

#include <casacore/casa/BasicSL/String.h>
#include <synthesis/Parallel/Algorithm.h>
#include <synthesis/Parallel/Applicator.h>
#include <msvis/MSVis/VisSet.h>
#include <msvis/MSVis/VisSetUtil.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/images/Images/TempImage.h>
#include <synthesis/TransformMachines/GridFT.h>
#include <synthesis/TransformMachines/FTMachine.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> Parallel model visibility prediction </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto module="FTMachine">FTMachine</linkto> module
//    <li> <linkto module="Applicator">Algorithm</linkto> module
// </prerequisite>
//
// <etymology>
// From "parallel model visibility prediction" and "algorithm"
// </etymology>
//
// <synopsis> 
// Used by PSkyEquation to predict model visibility in parallel.
// Fits within the Applicator/Algorithm framework
// </synopsis> 
//
// <example>
// <srcblock>

// </srcblock>
// </example>
//
// <motivation>
// Encapsulates the parallel model visibility prediction
// </motivation>
//
// <todo asof="01/7/20">
// <li> Implement changing SkyJones so that mosaicing can be parallelized
// </todo>

class PredictAlgorithm : public Algorithm {
 public:
  // Default constructor and destructor
  PredictAlgorithm();
  ~PredictAlgorithm();

  // Get the input data and parameters from the controller
  void get();

  // Return the results to the controller
  void put();

  // Return the name of the algorithm
  casacore::String &name();

private:
  // Private data
  casacore::Int model_p;

  casacore::TempImage<casacore::Complex> *cImage_p;
  casacore::String myName_p;
  casacore::String msName_p;
  casacore::Bool incremental_p;
  casacore::TempImage<casacore::Float> psf_p;
  FTMachine* ft_p;
  casacore::MeasurementSet *ms_p ; 

  // The work that a child processor is to do 
  void task();

  // Initialize the visibility grid 
  void initializeGet(const VisBuffer& vb, casacore::Int row, casacore::Int model, 
		     casacore::Bool incremental);

  // FT and put the result into the model visibility column of the visbuffer 
  VisBuffer& getModelVis(VisBuffer& vb, casacore::Int model,
			 casacore::Bool incremental);

};


} //# NAMESPACE CASA - END

#endif





