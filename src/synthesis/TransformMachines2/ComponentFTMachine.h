//# ComponentFTMachine.h: Definition for ComponentFTMachine
//# Copyright (C) 1996,1997,1998,1999,2000,2001
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
//# $Id$

#ifndef SYNTHESIS_TRANSFORM2_COMPONENTFTMACHINE_H
#define SYNTHESIS_TRANSFORM2_COMPONENTFTMACHINE_H

#include <components/ComponentModels/SkyComponent.h>
#include <casacore/measures/Measures/MDirection.h>
#include <casacore/measures/Measures/MPosition.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Logging/LogSink.h>
#include <casacore/casa/Logging/LogMessage.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// Forward declarations
class ComponentList;
namespace vi{class VisBuffer2;}

namespace refim{ // refactor namespace


// <summary> 
// ComponentFTMachine: a abstract base class defining the interface
// for a Fourier Transform Machine to be used in the 
// <linkto class=SkyEquation>SkyEquation</linkto> for the
// transformation of SkyComponents
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class=SkyModel>SkyModel</linkto> module
//   <li> <linkto class=SkyEquation>SkyEquation</linkto> module
//   <li> <linkto class=VisBuffer>VisBuffer</linkto> module
//   <li> <linkto class=SkyComponent>SkyComponent</linkto> class
// </prerequisite>
//
// <etymology>
// FTMachine is a Machine for Fourier Transforms
// </etymology>
//
// <synopsis> 
// The <linkto class=SkyEquation>SkyEquation</linkto> needs to be able
// to perform Fourier transforms on visibility data. FTMachine
// allows efficient Fourier Transform processing using a 
// <linkto class=VisBuffer>VisBuffer</linkto> which encapsulates
// a chunk of visibility (typically all baselines for one time)
// together with all the information needed for processing
// (e.g. UVW coordinates).
// </synopsis> 
//
// <example>
// A simple example of an ComponentFTMachine is found in 
// <linkto class=GridFT>SimpleComponentFTMachine</linkto>.
// See the example for <linkto class=SkyModel>SkyModel</linkto>.
// </example>
//
// <motivation>
// Define an interface to allow efficient processing of chunks of 
// visibility data
//
// </motivation>
//
// <todo asof="98/01/25">
// </li> SmearedComponentFTMachine to take into account
// time and bandwidth smearing
// </todo>




class ComponentFTMachine {
public:

  ComponentFTMachine();

  ComponentFTMachine(const ComponentFTMachine& other);

  ComponentFTMachine& operator=(const ComponentFTMachine& other);

  virtual ~ComponentFTMachine();

  // Get actual coherence : this is the only virtual method
  virtual void get(vi::VisBuffer2& vb, SkyComponent& component, casacore::Int row=-1) = 0;
  // Get actual coherence : this is the other only virtual method
  virtual void get(vi::VisBuffer2& vb, const ComponentList& componentList, casacore::Int row=-1) = 0;

  // Rotate the uvw from the observed phase center to the
  // desired phase center.
  void rotateUVW(casacore::Matrix<casacore::Double>& uvw, casacore::Vector<casacore::Double>& dphase,
		 const vi::VisBuffer2& vb, const casacore::MDirection& mDesired);
  // A version that use pointers to contigous matrix and vector 
  void rotateUVW(casacore::Double*& uvw, casacore::Double*& dphase, const casacore::Int nrows,
		 const vi::VisBuffer2& vb, const casacore::MDirection& mDesired);
  // Set number of threads to use when predicting. Setting it to -1 
  // basically tell openmp to use the number it can get
  void setnumthreads(const casacore::Int numthreads);
protected:

  casacore::LogIO logIO_p;

  casacore::LogIO& logIO();

  // Default Position used for phase rotations
  casacore::MPosition mLocation_p;
  
  virtual void ok();
  casacore::Int numthreads_p;

};

} // end namespace refim
} //# NAMESPACE CASA - END

#endif
