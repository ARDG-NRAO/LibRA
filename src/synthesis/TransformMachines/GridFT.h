//# GridFT.h: Definition for GridFT
//# Copyright (C) 1996-2012
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

#ifndef SYNTHESIS_GRIDFT_H
#define SYNTHESIS_GRIDFT_H

#include <synthesis/TransformMachines/FTMachine.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/scimath/Mathematics/FFTServer.h>
#include <msvis/MSVis/VisBuffer.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/casa/Containers/Block.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/scimath/Mathematics/ConvolveGridder.h>
#include <casacore/lattices/Lattices/LatticeCache.h>
#include <casacore/lattices/Lattices/ArrayLattice.h>
//#include <synthesis/MeasurementComponents/SynthesisPeek.h>


namespace casacore{

class UVWMachine;
}

namespace casa { //# NAMESPACE CASA - BEGIN

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
// FTMachine is a Machine for Fourier Transforms. GridFT does
// Grid-based Fourier transforms.
// </etymology>
//
// <synopsis> 
// The <linkto class=SkyEquation>SkyEquation</linkto> needs to be able
// to perform Fourier transforms on visibility data. GridFT
// allows efficient Fourier Transform processing using a 
// <linkto class=VisBuffer>VisBuffer</linkto> which encapsulates
// a chunk of visibility (typically all baselines for one time)
// together with all the information needed for processing
// (e.g. UVW coordinates).
//
// Gridding and degridding in GridFT are performed using a
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

class GridFT : public FTMachine {
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
  GridFT();
  GridFT(casacore::Long cachesize, casacore::Int tilesize, casacore::String convType="SF",
	 casacore::Float padding=1.0, casacore::Bool usezero=true, casacore::Bool useDoublePrec=false);
  GridFT(casacore::Long cachesize, casacore::Int tilesize, casacore::String convType,
	 casacore::MPosition mLocation, casacore::Float padding=1.0, casacore::Bool usezero=true, 
	 casacore::Bool useDoublePrec=false);
  GridFT(casacore::Long cachesize, casacore::Int tilesize, casacore::String convType,
	 casacore::MDirection mTangent, casacore::Float padding=1.0, casacore::Bool usezero=true,
	 casacore::Bool useDoublePrec=false);
  GridFT(casacore::Long cachesize, casacore::Int tilesize, casacore::String convType,
	 casacore::MPosition mLocation, casacore::MDirection mTangent, casacore::Float passing=1.0,
	 casacore::Bool usezero=true, casacore::Bool useDoublePrec=false);
  // </group>

  // Construct from a casacore::Record containing the GridFT state
  GridFT(const casacore::RecordInterface& stateRec);

  // Copy constructor
  GridFT(const GridFT &other);

  // Assignment operator
  virtual GridFT &operator=(const GridFT &other);

  virtual ~GridFT();

  virtual FTMachine* cloneFTM();

  // Initialize transform to Visibility plane using the image
  // as a template. The image is loaded and Fourier transformed.
  virtual void initializeToVis(casacore::ImageInterface<casacore::Complex>& image,
		       const VisBuffer& vb);
  
  // Finalize transform to Visibility plane: flushes the image
  // cache and shows statistics if it is being used.
  virtual void finalizeToVis();

  // Initialize transform to Sky plane: initializes the image
  virtual void initializeToSky(casacore::ImageInterface<casacore::Complex>& image,  casacore::Matrix<casacore::Float>& weight,
		       const VisBuffer& vb);

  
  // Finalize transform to Sky plane: flushes the image
  // cache and shows statistics if it is being used. DOES NOT
  // DO THE FINAL TRANSFORM!
  virtual void finalizeToSky();


  // Get actual coherence from grid by degridding
  virtual void get(VisBuffer& vb, casacore::Int row=-1);


  // Put coherence to grid by gridding.
  virtual void put(const VisBuffer& vb, casacore::Int row=-1, casacore::Bool dopsf=false,
	   FTMachine::Type type=FTMachine::OBSERVED);
  
  // Make the entire image
  void makeImage(FTMachine::Type type,
		 VisSet& vs,
		 casacore::ImageInterface<casacore::Complex>& image,
		 casacore::Matrix<casacore::Float>& weight);
  
  // Get the final image: do the Fourier transform and
  // grid-correct, then optionally normalize by the summed weights
  casacore::ImageInterface<casacore::Complex>& getImage(casacore::Matrix<casacore::Float>&, casacore::Bool normalize=true);
  virtual void normalizeImage(casacore::Lattice<casacore::Complex>& /*skyImage*/,
			      const casacore::Matrix<casacore::Double>& /*sumOfWts*/,
			      casacore::Lattice<casacore::Float>& /*sensitivityImage*/,
			      casacore::Bool /*fftNorm*/)
    {throw(casacore::AipsError("GridFT::normalizeImage() called"));}

  // Get the final weights image
  void getWeightImage(casacore::ImageInterface<casacore::Float>&, casacore::Matrix<casacore::Float>&);

  // Save and restore the GridFT to and from a record
  virtual casacore::Bool toRecord(casacore::String& error, casacore::RecordInterface& outRec, 
			casacore::Bool withImage=false, const casacore::String diskimage="");
  virtual casacore::Bool fromRecord(casacore::String& error, const casacore::RecordInterface& inRec);

  // Can this FTMachine be represented by Fourier convolutions?
  virtual casacore::Bool isFourier() {return true;}

  virtual void setNoPadding(casacore::Bool nopad){noPadding_p=nopad;};
  virtual void modifyConvFunc(const casacore::Vector<casacore::Double>& convFunc, casacore::Int convSupport, casacore::Int convSampling);
  virtual casacore::String name() const;
  virtual void setMiscInfo(const casacore::Int qualifier){(void)qualifier;};
  virtual void ComputeResiduals(VisBuffer&/*vb*/, casacore::Bool /*useCorrected*/) {};

protected:


  // Padding in FFT
  casacore::Float padding_p;

  // Get the appropriate data pointer
  casacore::Array<casacore::Complex>* getDataPointer(const casacore::IPosition&, casacore::Bool);

  virtual void ok();

  virtual void init();

  //Prepare the grid for degridding
  virtual void prepGridForDegrid();

  // Is this record on Grid? check both ends. This assumes that the
  // ends bracket the middle
  casacore::Bool recordOnGrid(const VisBuffer& vb, casacore::Int rownr) const;


  // Image cache
  casacore::LatticeCache<casacore::Complex> * imageCache;

  // Sizes
  casacore::Long cachesize;
  casacore::Int  tilesize;

  // Gridder
  casacore::ConvolveGridder<casacore::Double, casacore::Complex>* gridder;

  // Is this tiled?
  casacore::Bool isTiled;

  // casacore::Array lattice
  casacore::CountedPtr<casacore::Lattice<casacore::Complex> > arrayLattice;

  // Lattice. For non-tiled gridding, this will point to arrayLattice,
  //  whereas for tiled gridding, this points to the image
  casacore::CountedPtr<casacore::Lattice<casacore::Complex> > lattice;

  casacore::String convType;

  casacore::Float maxAbsData;

  // Useful IPositions
  casacore::IPosition centerLoc, offsetLoc;

  // Image Scaling and offset
  casacore::Vector<casacore::Double> uvScale, uvOffset;

 
  casacore::Int priorCacheSize;

  // Grid/degrid zero spacing points?

  casacore::Bool usezero_p;

  //force no padding
  casacore::Bool noPadding_p;

  //Check if using put that avoids non-necessary reads
  casacore::Bool usePut2_p;

  //machine name
  casacore::String machineName_p;

  casacore::Double timemass_p, timegrid_p, timedegrid_p;
  casacore::Vector<casacore::Double> convFunc_p;
  casacore::Int convSampling_p, convSupport_p;
  //  casa::async::SynthesisAsyncPeek *peek;
};

} //# NAMESPACE CASA - END

#endif
