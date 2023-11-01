//# Algorithm.h: defines base class Algorithm, a parallel computational unit
//# Copyright (C) 1999,2000
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
//#
//# $Id$
//#! ========================================================================

#ifndef SYNTHESIS_ALGORITHM_H
#define SYNTHESIS_ALGORITHM_H

//# Includes
#include <synthesis/Parallel/Applicator.h>

namespace casacore{

class String;
}

namespace casa { //# NAMESPACE CASA - BEGIN

//# Forward Declarations

extern Applicator applicator;

// <summary>
// Defines a computational unit for parallel processing
// </summary>

// <use visibility=local>   or   <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> SomeClass
//   <li> SomeOtherClass
//   <li> some concept
// </prerequisite>
//
// <etymology>
// </etymology>
//
// <synopsis>
// Base class used to define a computational unit for parallel processing.  
// The user supplies: i) the get() (fetch the data); ii) the task() (do 
// the work); iii) the put() (send the results back); and iv) the name()
// (name of the task).
// </synopsis>
//
// <example>
// </example>
//
// <motivation>
// The algorithm class is used for embarassingly- or nearly-embarassingly
// types of parallel problems.
// </motivation>
//
//
// <todo asof="1999/05/27">
//   <li> Document document document.
// </todo>

//
// Base class.  User always derives off this class
//
class Algorithm {
 public:
  // Default constructor and destructor
  Algorithm() {}
  virtual ~Algorithm(){}

  // Generic apply to execute the parallel task
  void apply() { get();             // Get the input from the controller
                 task();            // Do the work
                 applicator.done(); // Signal that the work is done
                 put();             // Return results to the controller
  };

  // Get the input data and parameters from the controller
  virtual void get() = 0;

  // Return the results to the controller
  virtual void put() = 0;

  // Return the name of the algorithm
  virtual casacore::String &name() = 0;

 protected:
  // Do the work assigned as a parallel task
  virtual void task() = 0;
};


} //# NAMESPACE CASA - END

#endif





