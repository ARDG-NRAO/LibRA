//# WProjectFT.h: Definition for WProjectFT
//# Copyright (C) 2003-2016
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU General Public License
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

#ifndef SYNTHESIS_TRANSFORM2_WPROJECTFT_H
#define SYNTHESIS_TRANSFORM2_WPROJECTFT_H

#include <synthesis/TransformMachines2/FTMachine.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/scimath/Mathematics/FFTServer.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/casa/Containers/Block.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/scimath/Mathematics/ConvolveGridder.h>
#include <casacore/lattices/Lattices/LatticeCache.h>
#include <casacore/lattices/Lattices/ArrayLattice.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <casacore/measures/Measures/Measure.h>
#include <casacore/measures/Measures/MDirection.h>
#include <casacore/measures/Measures/MPosition.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>

namespace casacore{

template <class K, class V> class SimpleOrderedMap;
template <class T> class PtrBlock;
template <class T> class CountedPtr;
}

namespace casa { //# NAMESPACE CASA - BEGIN


namespace refim { //# namespace for imaging refactor
class   WPConvFunc; 

// <summary>  An FTMachine for Gridded Fourier transforms </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class=FTMachine>FTMachine</linkto> module
//   <li> <linkto class=SkyEquation>SkyEquation</linkto> module
//   <li> <linkto class=VisBuffer>VisBuffer</linkto> module
// </prerequisite>
//
// <etymology>
// FTMachine is a Machine for Fourier Transforms. WProjectFT does
// Grid-based Fourier transforms.
// </etymology>
//
// <synopsis> 
// The <linkto class=SkyEquation>SkyEquation</linkto> needs to be able
// to perform Fourier transforms on visibility data. WProjectFT
// allows efficient Fourier Transform processing using a 
// <linkto class=VisBuffer>VisBuffer</linkto> which encapsulates
// a chunk of visibility (typically all baselines for one time)
// together with all the information needed for processing
// (e.g. UVW coordinates).
//
// Gridding and degridding in WProjectFT are performed using a
// novel sort-less algorithm. In this approach, the gridded plane is
// divided into small patches, a cache of which is maintained in memory
// using a general-purpose <linkto class=casacore::LatticeCache>LatticeCache</linkto> class. As the (time-sorted)
// visibility data move around slowly in the Fourier plane, patches are
// swapped in and out as necessary. Thus, optimally, one would keep at
// least one patch per baseline.  
//
// A grid cache is defined on construction. If the gridded uv plane is smaller
// than this, it is kept entirely in memory and all gridding and
// degridding is done entirely in memory. Otherwise a cache of tiles is
// kept an paged in and out as necessary. Optimally the cache should be
// big enough to hold all polarizations and frequencies for all
// baselines. The paging rate will then be small. As the cache size is
// reduced below this critical value, paging increases. The algorithm will
// work for only one patch but it will be very slow!
//
// This scheme works well for arrays having a moderate number of
// antennas since the saving in space goes as the ratio of
// baselines to image size. For the ATCA, VLBA and WSRT, this ratio is
// quite favorable. For the VLA, one requires images of greater than
// about 200 pixels on a side to make it worthwhile.
//
// The FFT step is done plane by plane for images having less than
// 1024 * 1024 pixels on each plane, and line by line otherwise.
//
// The gridding and degridding steps are implemented in Fortran
// for speed. In gridding, the visibilities are added onto the
// grid points in the neighborhood using a weighting function.
// In degridding, the value is derived by a weight summ of the
// same points, using the same weighting function.
// </synopsis> 
//
// <example>
// See the example for <linkto class=SkyModel>SkyModel</linkto>.
// </example>
//
// <motivation>
// Define an interface to allow efficient processing of chunks of 
// visibility data
// </motivation>
//
// <todo asof="97/10/01">
// <ul> Deal with large VLA spectral line case 
// </todo>

class WProjectFT : public FTMachine {
public:

  // Constructor: cachesize is the size of the cache in words
  // (e.g. a few million is a good number), tilesize is the
  // size of the tile used in gridding (cannot be less than
  // 12, 16 works in most cases). 
  // <group>
  WProjectFT(
	   casacore::Int nFacets, casacore::Long cachesize, casacore::Int tilesize=16, 
	   casacore::Bool usezero=true, casacore::Bool useDoublePrec=false, const casacore::Double minW=-1.0, const casacore::Double maxW=-1.0, const casacore::Double rmsW=-1.0);
  //Constructor without tangent direction
  WProjectFT(casacore::Int nFacets, casacore::MPosition mLocation,
	     casacore::Long cachesize, casacore::Int tilesize=16, 
	     casacore::Bool usezero=true, casacore::Float padding=1.0, casacore::Bool useDoublePrec=false, const casacore::Double minW=-1.0, const casacore::Double maxW=-1.0, const casacore::Double rmsW=-1.0);
  //Deprecated no longer need ms in constructor
  WProjectFT(
	     casacore::Int nFacets, casacore::MDirection mTangent, casacore::MPosition mLocation,
	     casacore::Long cachesize, casacore::Int tilesize=16, 
	     casacore::Bool usezero=true, casacore::Float padding=1.0, casacore::Bool useDoublePrec=false, const casacore::Double minW=-1.0, const casacore::Double maxW=-1.0, const casacore::Double rmsW=-1.0);
  // </group>

  // Construct from a casacore::Record containing the WProjectFT state
  WProjectFT(const casacore::RecordInterface& stateRec);

  // Copy constructor
  WProjectFT(const WProjectFT &other);

  // Assignment operator
  WProjectFT &operator=(const WProjectFT &other);

  ~WProjectFT();

  //clone to FTMachine pointer
  virtual FTMachine* cloneFTM();
  // Initialize transform to Visibility plane using the image
  // as a template. The image is loaded and Fourier transformed.
  void initializeToVis(casacore::ImageInterface<casacore::Complex>& image,
  		       const vi::VisBuffer2& vb);
  
  // Finalize transform to Visibility plane: flushes the image
  // cache and shows statistics if it is being used.
  void finalizeToVis();

  // Initialize transform to Sky plane: initializes the image
  void initializeToSky(casacore::ImageInterface<casacore::Complex>& image,  casacore::Matrix<casacore::Float>& weight,
  		       const vi::VisBuffer2& vb);
  // Finalize transform to Sky plane: flushes the image
  // cache and shows statistics if it is being used. DOES NOT
  // DO THE FINAL TRANSFORM!
  void finalizeToSky();

  // Get actual coherence from grid by degridding
  void get(vi::VisBuffer2& vb, casacore::Int row=-1);

  // Put coherence to grid by gridding.
  void put(const vi::VisBuffer2& vb, casacore::Int row=-1, casacore::Bool dopsf=false,
	   FTMachine::Type type=FTMachine::OBSERVED);

  // Make the entire image
  void makeImage(FTMachine::Type type,
		 vi::VisibilityIterator2& vi,
		 casacore::ImageInterface<casacore::Complex>& image,
		 casacore::Matrix<casacore::Float>& weight);
  
  // Get the final image: do the Fourier transform and
  // grid-correct, then optionally normalize by the summed weights
  casacore::ImageInterface<casacore::Complex>& getImage(casacore::Matrix<casacore::Float>&, casacore::Bool normalize=true);
  virtual void normalizeImage(casacore::Lattice<casacore::Complex>& /*skyImage*/,
			      const casacore::Matrix<casacore::Double>& /*sumOfWts*/,
			      casacore::Lattice<casacore::Float>& /*sensitivityImage*/,
			      casacore::Bool /*fftNorm*/)
    {throw(casacore::AipsError("WProjectFT::normalizeImage() called"));}
 
  // Get the final weights image
  void getWeightImage(casacore::ImageInterface<casacore::Float>&, casacore::Matrix<casacore::Float>&);

  // Save and restore the WProjectFT to and from a record
  casacore::Bool toRecord(casacore::String& error, casacore::RecordInterface& outRec, 
		casacore::Bool withImage=false, const casacore::String diskimage="");
  casacore::Bool fromRecord(casacore::String& error, const casacore::RecordInterface& inRec);
  
  // Can this FTMachine be represented by Fourier convolutions?
  casacore::Bool isFourier() {return true;}


  // Return name of this machine

  casacore::String name() const;

  // Copy convolution function etc to another FT machine
  // necessary if ft and ift are distinct but can share convfunctions

  void setConvFunc(casacore::CountedPtr<WPConvFunc>& pbconvFunc);
  casacore::CountedPtr<WPConvFunc>& getConvFunc();
  virtual void setMiscInfo(const casacore::Int qualifier){(void)qualifier;};
  virtual void ComputeResiduals(vi::VisBuffer2& /*vb*/, casacore::Bool /*useCorrected*/) {};
  //Helper function to calculate min, max, rms of W in the data set
  static void wStat(vi::VisibilityIterator2& vi, casacore::Double& minW, casacore::Double& maxW, casacore::Double& rmsW); 

protected:

  // Padding in FFT
  casacore::Float padding_p;

  casacore::Int nint(casacore::Double val) {return casacore::Int(floor(val+0.5));};

  // Find the convolution function
  void findConvFunction(const casacore::ImageInterface<casacore::Complex>& image,
			const vi::VisBuffer2& vb);

  casacore::Int nWPlanes_p;

  // Get the appropriate data pointer
  casacore::Array<casacore::Complex>* getDataPointer(const casacore::IPosition&, casacore::Bool);

  void ok();

  void init();

  void prepGridForDegrid();
  // Is this record on Grid? check both ends. This assumes that the
  // ends bracket the middle
  //casacore::Bool recordOnGrid(const vi::VisBuffer2& vb, casacore::Int rownr) const;


  // Image cache
  casacore::LatticeCache<casacore::Complex> * imageCache;

  // Sizes
  casacore::Long cachesize;
  casacore::Int tilesize;

  // Gridder
  casacore::ConvolveGridder<casacore::Double, casacore::Complex>* gridder;

  // Is this tiled?
  casacore::Bool isTiled;

  // casacore::Array lattice
  casacore::CountedPtr<casacore::Lattice<casacore::Complex> > arrayLattice;

  // Lattice. For non-tiled gridding, this will point to arrayLattice,
  //  whereas for tiled gridding, this points to the image
  casacore::CountedPtr<casacore::Lattice<casacore::Complex> > lattice;

  casacore::Float maxAbsData;

  // Useful IPositions
  casacore::IPosition centerLoc, offsetLoc;

  // Image Scaling and offset
  casacore::Vector<casacore::Double> uvScale, uvOffset;
  casacore::Double savedWScale_p;


  // Grid/degrid zero spacing points?
  casacore::Bool usezero_p;

  casacore::Cube<casacore::Complex> convFunc;
  casacore::Int convSampling;
  casacore::Int convSize;
  casacore::Vector<casacore::Int> convSupport;

  casacore::Vector<casacore::Int> convSizes_p;


  casacore::Int wConvSize;

  casacore::Int lastIndex_p;

  casacore::Int getIndex(const casacore::MSPointingColumns& mspc, const casacore::Double& time,
	       const casacore::Double& interval);

  casacore::String machineName_p;

  casacore::CountedPtr<WPConvFunc> wpConvFunc_p;
  casacore::Double timemass_p, timegrid_p, timedegrid_p;
  casacore::Double minW_p, maxW_p, rmsW_p;
};
} // end namespace refim
} //# NAMESPACE CASA - END

#endif
