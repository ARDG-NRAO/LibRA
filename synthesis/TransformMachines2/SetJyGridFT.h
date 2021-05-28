//# SetJyGridFT.h: Definition for GridFT
//# Copyright (C) 2012-2014
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU  General Public
//# License for more details.
//#
//# You should have received a copy of the GNU  General Public License
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

#ifndef SYNTHESIS_TRANSFORM2_SETJYGRIDFT_H
#define SYNTHESIS_TRANSFORM2_SETJYGRIDFT_H

#include <synthesis/TransformMachines2/GridFT.h>


namespace casa { //# NAMESPACE CASA - BEGIN
namespace vi { class VisBuffer2;}

namespace refim { //#namespace for imaging refactor
// <summary>  An FTMachine for Gridded Fourier transforms specializing in setjy frequency scaling </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class=FTMachine>GridFT</linkto> module
//   <li> <linkto class=SkyEquation>SkyEquation</linkto> module
//   <li> <linkto class=VisBuffer>VisBuffer</linkto> module
// </prerequisite>
//
// <etymology>
// Inheriting from GridFT which does
// Grid-based Fourier transforms.
// special case for setjy style frequency scaling
// </etymology>
//
// <synopsis> 

// </synopsis> 
//
// <example>
// </example>
//
// <motivation>
// Define an interface to allow efficient processing of chunks of 
// visibility data
// </motivation>
//
// <todo asof="2012/05/08">
//
// </todo>

class SetJyGridFT : public GridFT {
public:

  // Constructor: cachesize is the size of the cache in words
  // (e.g. a few million is a good number), tilesize is the
  // size of the tile used in gridding (cannot be less than
  // 12, 16 works in most cases), and convType is the type of
  // gridding used (SF is prolate spheriodal wavefunction,
  // and BOX is plain box-car summation). mLocation is
  // the position to be used in some phase rotations. If
  // mTangent is specified then the uvw rotation is done for
  // that location iso the image center.
  // <group>

  SetJyGridFT(casacore::Long cachesize, casacore::Int tilesize, casacore::String convType,
	 casacore::MPosition mLocation, casacore::MDirection mTangent, casacore::Float passing=1.0,
	 casacore::Bool usezero=true, casacore::Bool useDoublePrec=false,
	      const casacore::Vector<casacore::Double>& freqscale=casacore::Vector<casacore::Double>(1, 0.0), const casacore::Vector<casacore::Double>& scale=casacore::Vector<casacore::Double>(1, 1.0));
  // </group>

  // Construct from a casacore::Record containing the GridFT state
  SetJyGridFT(const casacore::RecordInterface& stateRec);

  // Copy constructor
  SetJyGridFT(const SetJyGridFT &other);

  // Assignment operator
  SetJyGridFT &operator=(const SetJyGridFT &other);

  virtual ~SetJyGridFT();

  //clone FTM
  virtual FTMachine* cloneFTM();
  // Initialize transform to Visibility plane using the image
  // as a template. The image is loaded and Fourier transformed.
  virtual void initializeToVis(casacore::ImageInterface<casacore::Complex>& image,
			       const vi::VisBuffer2& vb);
  
  // Finalize transform to Visibility plane: flushes the image
  // cache and shows statistics if it is being used.
  //void finalizeToVis();

  // Get actual coherence from grid by degridding
  void get(vi::VisBuffer2& vb, casacore::Int row=-1);

  
  // Save and restore the GridFT to and from a record
  virtual casacore::Bool toRecord(casacore::String& error, casacore::RecordInterface& outRec, 
			casacore::Bool withImage=false, const casacore::String diskimage="");
  virtual casacore::Bool fromRecord(casacore::String& error, const casacore::RecordInterface& inRec);

  virtual void setScale(const casacore::Vector<casacore::Double>& freq, const casacore::Vector<casacore::Double>& scale);

  virtual casacore::String name() const;


protected:




  //Prepare the grid for degridding
  //void prepGridForDegrid();

  casacore::Vector<casacore::Double> freqscale_p;
  casacore::Vector<casacore::Double> scale_p;
  casacore::Vector<casacore::Double> interpscale_p;



};
} //# end of namespace refim
} //# NAMESPACE CASA - END

#endif
