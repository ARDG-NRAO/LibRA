//# ReadMSAlgorithm.h: Test class to read in parallel a MS
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

#ifndef SYNTHESIS_READMSALGORITHM_H
#define SYNTHESIS_READMSALGORITHM_H

#include <casa/BasicSL/String.h>
#include <casa/Arrays/Array.h>
#include <lattices/Lattices/PagedArray.h>
#include <synthesis/Parallel/Algorithm.h>
#include <msvis/MSVis/VisSet.h>
#include <msvis/MSVis/VisSetUtil.h>

namespace casa { //# NAMESPACE CASA - BEGIN

//Test class to read in parallel a MS
class ReadMSAlgorithm : public Algorithm {
 public:
  // Default constructor and destructor
  ReadMSAlgorithm();
  ~ReadMSAlgorithm();

  // Get the input data and parameters from the controller
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
  casacore::Int         status;
  //casacore::Int cache_p;

  // Read the MSset
  void task();
};



} //# NAMESPACE CASA - END

#endif
