//# WriteMSAlgorithm.h: Test class to write in parallel to a MS
//# Copyright (C) 2019
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by
//# the Free Software Foundation; either version 3 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
//# License for more details.
//#
//# https://www.gnu.org/licenses/
//#
//# Queries concerning CASA should be submitted at
//#        https://help.nrao.edu
//#
//#        Postal address: CASA Project Manager 
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$

#ifndef SYNTHESIS_WRITEMSALGORITHM_H
#define SYNTHESIS_WRITEMSALGORITHM_H

#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/lattices/Lattices/PagedArray.h>
#include <synthesis/Parallel/Algorithm.h>
#include <synthesis/Parallel/Applicator.h>

namespace casa { //# NAMESPACE CASA - BEGIN

//Test class to write in parallel a MS
class WriteMSAlgorithm : public Algorithm {
 public:
  // Default constructor and destructor
  WriteMSAlgorithm();
  ~WriteMSAlgorithm();

  // Get the input data and parameters from the controller
  // 3 things have to be sent msname, rowids, somenumber
  void get();

  // Return the results to the controller
  void put();

  // Return the name of the algorithm
  casacore::String &name();

 private:
  // Local copies of the data and input parameters
  //casacore::PagedArray<casacore::Float> *model_sl_p;
  //casacore::Int          msid;
  casacore::String       myName;
  casacore::String       msName;
  casacore::Vector<casacore::uInt> rowids;
  casacore::Complex   somenumber;
  casacore::Int         status;
  casacore::Int doneSig_p; // need a private variable for serial
  // write the MSset
  void task();
};



} //# NAMESPACE CASA - END

#endif
