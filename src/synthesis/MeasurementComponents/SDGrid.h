//# SDGrid.h: Definition for SDGrid
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
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

#ifndef SYNTHESIS_SDGRID_H
#define SYNTHESIS_SDGRID_H

#define SDGRID_PERFS
#if defined(SDGRID_PERFS) 
#include <iostream>
#include <string>
#include <chrono>
#endif

#include <casacore/casa/Arrays/Array.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Containers/Block.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/lattices/Lattices/ArrayLattice.h>
#include <casacore/lattices/Lattices/LatticeCache.h>
#include <casacore/measures/Measures/Measure.h>
#include <casacore/measures/Measures/MDirection.h>
#include <casacore/measures/Measures/MPosition.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/VisibilityIterator.h>
#include <casacore/scimath/Mathematics/FFTServer.h>

#include <synthesis/TransformMachines/FTMachine.h>
#include <synthesis/TransformMachines/SkyJones.h>
#include <synthesis/Utilities/SDPosInterpolator.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> An FTMachine for Gridding Single Dish data
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class=FTMachine>FTMachine</linkto> module
//   <li> <linkto class=SkyEquation>SkyEquation</linkto> module
//   <li> <linkto class=VisBuffer>VisBuffer</linkto> module
// </prerequisite>
//
// <etymology>
// FTMachine is a Machine for Fourier Transforms. SDGrid does
// Single Dish gridding in a similar way
// </etymology>
//
// <synopsis> 
// The <linkto class=SkyEquation>SkyEquation</linkto> needs to be able
// to perform Fourier transforms on visibility data and to grid
// single dish data.
// SDGrid allows efficient Single Dish processing using a 
// <linkto class=VisBuffer>VisBuffer</linkto> which encapsulates
// a chunk of visibility (typically all baselines for one time)
// together with all the information needed for processing
// (e.g. direction coordinates).
//
// Gridding and degridding in SDGrid are performed using a
// novel sort-less algorithm. In this approach, the gridded plane is
// divided into small patches, a cache of which is maintained in memory
// using a general-purpose <linkto class=casacore::LatticeCache>LatticeCache</linkto> class. As the (time-sorted)
// visibility data move around slowly in the image plane, patches are
// swapped in and out as necessary. Thus, optimally, one would keep at
// least one patch per scan line of data.
//
// A grid cache is defined on construction. If the gridded image plane is smaller
// than this, it is kept entirely in memory and all gridding and
// degridding is done entirely in memory. Otherwise a cache of tiles is
// kept an paged in and out as necessary. Optimally the cache should be
// big enough to hold all polarizations and frequencies for one
// complete scan line.
// The paging rate will then be small. As the cache size is
// reduced below this critical value, paging increases. The algorithm will
// work for only one patch but it will be very slow!
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

#if defined(SDGRID_PERFS)
namespace sdgrid_perfs {
class ChronoStat {
public:
    using Clock = std::chrono::steady_clock;
    using Duration = Clock::duration;
    using TimePoint = Clock::time_point;

    ChronoStat(const std::string & name = "");
    const std::string& name() const;
    void setName(const std::string& name);
    void start();
    void stop();
    bool isEmpty() const;
    Duration lapsSum() const;
    Duration lapsMin() const;
    Duration lapsMax() const;
    Duration lapsMean() const;
    unsigned int lapsCount() const;
    unsigned int nOverflows() const;
    unsigned int nUnderflows() const;
    std::string json() const;

private:
    std::string name_;
    bool started_;
    unsigned int n_laps_;
    unsigned int n_overflows_;
    unsigned int n_underflows_;

    TimePoint lap_start_time_;
    Duration laps_sum_;
    Duration laps_min_;
    Duration laps_max_;
    
    std::string quote(const std::string& s) const;
};

std::ostream& operator<<(std::ostream &os, const ChronoStat &c);

class StartStop {
public:
    StartStop(ChronoStat &c);
    ~StartStop();
private:
    ChronoStat& c_;
};

}

#endif
class SDGrid : public FTMachine {
public:

  // Constructor: cachesize is the size of the cache in words
  // (e.g. a few million is a good number), tilesize is the
  // size of the tile used in gridding (cannot be less than
  // 12, 16 works in most cases), and convType is the type of
  // gridding used (SF is prolate spheriodal wavefunction,
  // and BOX is plain box-car summation). mLocation is
  // the position to be used in some phase rotations. If
  // mTangent is specified then the uvw rotation is done for
  // that location iso the image center. userSupport is to allow 
  // larger support for the convolution if the user wants it ..-1 will 
  // use the default  i.e 1 for BOX and 3 for others
  // USEIMAGINGWEIGHT
  // The parameter useImagingWeight in the constructors is to explicitly  
  // use vb.imagingweight while gridding,
  // When doing just SD imaging then setting it to false is fine (in fact recommended as vb.imagingweight
  // is set to zero if any pol is flagged this may change later .....today being 2014/08/06) 
  // when using it in conjuction with interferometer gridding then set useImagingWeight to true
  // this is to allow for proper non natural weighting scheme while imaging
  // <group>
  SDGrid(SkyJones& sj, casacore::Int cachesize, casacore::Int tilesize,
	 casacore::String convType="BOX", casacore::Int userSupport=-1, casacore::Bool useImagingWeight=false);
  SDGrid(casacore::MPosition& ml, SkyJones& sj, casacore::Int cachesize,
	 casacore::Int tilesize, casacore::String convType="BOX", casacore::Int userSupport=-1,
	 casacore::Float minweight=0., casacore::Bool clipminmax=false, casacore::Bool useImagingWeight=false);
  SDGrid(casacore::Int cachesize, casacore::Int tilesize,
	 casacore::String convType="BOX", casacore::Int userSupport=-1, casacore::Bool useImagingWeight=false);
  SDGrid(casacore::MPosition& ml, casacore::Int cachesize, casacore::Int tilesize,
	 casacore::String convType="BOX", casacore::Int userSupport=-1, casacore::Float minweight=0., casacore::Bool clipminmax=false,
	 casacore::Bool useImagingWeight=false);
  SDGrid(casacore::MPosition& ml, casacore::Int cachesize, casacore::Int tilesize,
	 casacore::String convType="TGAUSS", casacore::Float truncate=-1.0, 
	 casacore::Float gwidth=0.0, casacore::Float jwidth=0.0, casacore::Float minweight=0., casacore::Bool clipminmax=false,
	 casacore::Bool useImagingWeight=false);
  // </group>

  // Copy constructor
  SDGrid(const SDGrid &other);

  // Assignment operator
  SDGrid &operator=(const SDGrid &other);

  ~SDGrid();

  // Initialize transform to Visibility plane using the image
  // as a template. The image is loaded and Fourier transformed.
  void initializeToVis(casacore::ImageInterface<casacore::Complex>& image,
		       const VisBuffer& vb);

  // Finalize transform to Visibility plane: flushes the image
  // cache and shows statistics if it is being used.
  void finalizeToVis();

  // Initialize transform to Sky plane: initializes the image
  void initializeToSky(casacore::ImageInterface<casacore::Complex>& image,  casacore::Matrix<casacore::Float>& weight,
		       const VisBuffer& vb);

  // Finalize transform to Sky plane: flushes the image
  // cache and shows statistics if it is being used. DOES NOT
  // DO THE FINAL TRANSFORM!
  void finalizeToSky();

  // Get actual coherence from grid by degridding
  void get(VisBuffer& vb, casacore::Int row=-1);

  // Put coherence to grid by gridding.
  void put(const VisBuffer& vb, casacore::Int row=-1, casacore::Bool dopsf=false,
	   FTMachine::Type type=FTMachine::OBSERVED);

  // Make the entire image using a ROVisIter...
  // This is an overload for FTMachine version as 
  //SDGrid now does everything in memory
  // so for large cube ..proceed by slices that fit in memory here.
  virtual void makeImage(FTMachine::Type type,
			 ROVisibilityIterator& vi,
			 casacore::ImageInterface<casacore::Complex>& image,
			 casacore::Matrix<casacore::Float>& weight);

  // Get the final image: 
  //  optionally normalize by the summed weights
  casacore::ImageInterface<casacore::Complex>& getImage(casacore::Matrix<casacore::Float>&, casacore::Bool normalize=true);
  virtual void normalizeImage(casacore::Lattice<casacore::Complex>& /*skyImage*/,
			      const casacore::Matrix<casacore::Double>& /*sumOfWts*/,
			      casacore::Lattice<casacore::Float>& /*sensitivityImage*/,
			      casacore::Bool /*fftNorm*/)
    {throw(casacore::AipsError("SDGrid::normalizeImage() called"));}

  // Get the final weights image
  void getWeightImage(casacore::ImageInterface<casacore::Float>&, casacore::Matrix<casacore::Float>&);

  // Has this operator changed since the last application?
  virtual casacore::Bool changed(const VisBuffer& vb);
  virtual void setMiscInfo(const casacore::Int qualifier){(void)qualifier;};
  virtual void ComputeResiduals(VisBuffer& /*vb*/, casacore::Bool /*useCorrected*/) {};

  virtual casacore::String name() const;

  // Enable/disable SDGrid::Cache
  void setEnableCache(casacore::Bool doEnable);

private:

  // Find the Primary beam and convert it into a convolution buffer
  void findPBAsConvFunction(const casacore::ImageInterface<casacore::Complex>& image,
			    const VisBuffer& vb);

  SkyJones* sj_p;

  // Get the appropriate data pointer
  casacore::Array<casacore::Complex>* getDataPointer(const casacore::IPosition&, casacore::Bool);
  casacore::Array<casacore::Float>* getWDataPointer(const casacore::IPosition&, casacore::Bool);

  void ok();

  void init();

  // Image cache
  casacore::LatticeCache<casacore::Complex> * imageCache;
  casacore::LatticeCache<casacore::Float> * wImageCache;

  // Sizes
  casacore::Int cachesize, tilesize;

  // Is this tiled?
  casacore::Bool isTiled;

  // Storage for weights
  casacore::ImageInterface<casacore::Float>* wImage;

  // casacore::Array lattice
  casacore::Lattice<casacore::Complex> * arrayLattice;
  casacore::Lattice<casacore::Float> * wArrayLattice;

  // Lattice. For non-tiled gridding, this will point to arrayLattice,
  //  whereas for tiled gridding, this points to the image
  casacore::Lattice<casacore::Complex>* lattice;
  casacore::Lattice<casacore::Float>* wLattice;

  casacore::String convType;

  // Useful IPositions
  casacore::IPosition centerLoc, offsetLoc;

  // casacore::Array for non-tiled gridding
  casacore::Array<casacore::Float> wGriddedData;


  casacore::DirectionCoordinate directionCoord;

  casacore::MDirection::Convert* pointingToImage;

  // Stores - for spectra in the current MeasurementSet row -
  // coordinates of antenna's direction at data-taking time (ms.MAIN.TIME),
  // projected on image's spatial geometric plane.
  casacore::Vector<casacore::Double> xyPos;

  // Keep track of xyPos member's validity
  struct MaskedPixelRef {
      MaskedPixelRef(casacore::Vector<casacore::Double>& xy, casacore::Bool isValid = false);
      MaskedPixelRef& operator=(const MaskedPixelRef &other);
      casacore::Vector<casacore::Double>& xy;
      casacore::Bool isValid;
  };
  MaskedPixelRef rowPixel;

  //Original xypos of moving source
  casacore::Vector<casacore::Double> xyPosMovingOrig_p;

  casacore::MDirection worldPosMeas;

  casacore::Cube<casacore::Int> flags;

  casacore::Vector<casacore::Float> convFunc;
  casacore::Int convSampling;
  casacore::Int convSize;
  casacore::Int convSupport;
  casacore::Int userSetSupport_p;
  
  casacore::Float truncate_p;
  casacore::Float gwidth_p;
  casacore::Float jwidth_p;

  casacore::Float minWeight_p;

  casacore::Int lastIndex_p;
  casacore::Vector<casacore::Int> lastIndexPerAnt_p;
  casacore::Bool useImagingWeight_p;
  casacore::Int lastAntID_p;
  casacore::Int msId_p;

  casacore::Bool isSplineInterpolationReady;
  SDPosInterpolator* interpolator;

  // for minmax clipping
  casacore::Bool clipminmax_;
  casacore::Array<casacore::Complex> gmin_;
  casacore::Array<casacore::Complex> gmax_;
  casacore::Array<casacore::Float> wmin_;
  casacore::Array<casacore::Float> wmax_;
  casacore::Array<casacore::Int> npoints_;
  void clipMinMax();

  casacore::Int getIndex(const casacore::MSPointingColumns& mspc, const casacore::Double& time,
	       const casacore::Double& interval=-1.0, const casacore::Int& antid=-1);

  casacore::Bool getXYPos(const VisBuffer& vb, casacore::Int row);

  //get the casacore::MDirection from a chosen column of pointing table
  casacore::MDirection directionMeas(const casacore::MSPointingColumns& mspc, const casacore::Int& index);
  casacore::MDirection directionMeas(const casacore::MSPointingColumns& mspc, const casacore::Int& index, const casacore::Double& time);
  casacore::MDirection interpolateDirectionMeas(const casacore::MSPointingColumns& mspc, const casacore::Double& time,
                                  const casacore::Int& index, const casacore::Int& index1, const casacore::Int& index2);

  void pickWeights(const VisBuffer&vb, casacore::Matrix<casacore::Float>& weight);

  // Cache
  struct MaskedPixel {
      MaskedPixel(
          casacore::Double x = 0.0,
          casacore::Double y = 0.0,
          casacore::Bool isValid = false
      );
      casacore::Double x;
      casacore::Double y;
      casacore::Bool isValid;
  };

  // Description:
  // A cache aimed at storing expensive (spectra pixels) computation results,
  // which can be re-used across consecutive iterations
  // over the same input MeasurementSets.
  // Designed to be VisibilityIterator-friendly.
  // Since it is currently not aimed to be re-used elsewhere,
  // it is "welded" to its SDGrid container.
  //
  // Motivation:
  // CASA sdimaging task always iterates twice over the same input MeasurementSets,
  // to compute a normal image and a weight image.
  class Cache {
  public:
      enum class AccessMode {
          READ,
          WRITE
      };

  public:
      Cache(SDGrid &parent);

      static const casacore::String& className();

      Cache& operator=(const Cache &other);

      void open(AccessMode accessMode);
      void close();
      void clear();

      casacore::Bool isEmpty() const;
      casacore::Bool isReadable() const;
      casacore::Bool isWriteable() const;

      // Synchronize the cache with the VisibilityIterator.
      // Must be called exactly once each time the VisibilityIterator
      // starts iterating over a new MS (including the first one)
      void newMS(const casacore::MeasurementSet& ms);

      // Cache spatial pixel computation result for the current row.
      // Must be called exactly once per MeasurementSet row.
      void storeRowPixel();

      // Load from the cache Image's spatial pixel for the current row.
      // Must be called exactly once per MeasurementSet row.
      void loadRowPixel();

  private:
      void rewind();

      SDGrid& sdgrid;

      using Pixels = std::vector<MaskedPixel>;

      struct MsCache {
          MsCache(const casacore::String& msPath, const casacore::String& msTableName, casacore::rownr_t nRows);
          casacore::Bool isConsistent() const;
          casacore::String msPath;
          casacore::String msTableName;
          casacore::rownr_t nRows;
          Pixels pixels;
      };

      using MsCaches = std::vector<MsCache>;

      MsCaches msCaches;

      // State Control
      casacore::Bool isOpened;
      AccessMode accessMode;
      casacore::Bool canRead;
      casacore::Bool canWrite;

      // Input/Output
      // ---- Storing to the cache
      const MaskedPixelRef& inputPixel;
      Pixels *msPixels;

      // ---- Loading from the cache
      MaskedPixelRef& outputPixel;
      MsCaches::const_iterator msCacheReadIterator;
      Pixels::const_iterator pixelReadIterator;

  };

  Cache cache;
  casacore::Bool cacheIsEnabled;

// Description:
// Helper class handling SDGrid::Cache open/close.
// A CacheManager does nothing if it is not on duty.
// Otherwise, it opens the cache on construction,
// and closes it on destruction.
//
// Motivation:
// Synchronization matters most when dealing with caches.
  class CacheManager {
  public:
      CacheManager(Cache &cache,
        casacore::Bool onDuty=false, 
        Cache::AccessMode accessMode=Cache::AccessMode::READ);
      ~CacheManager();
  private:
      Cache& cache;
      casacore::Bool onDuty;
      Cache::AccessMode accessMode;
  };

// Description:
// Helper class for storing data into SDGrid::Cache.
// A CacheWriter does nothing if it is not on duty.
// Otherwise, it stores on destruction
// the pixel of spectra in the current row
// of the current MeasurementSet.
//
// Motivation:
// SDGrid::getXYPos() has a lot of branches.

  class CacheWriter {
  public:
      CacheWriter(Cache &cache,
        casacore::Bool onDuty=false);
      ~CacheWriter();
  private:
      Cache& cache;
      casacore::Bool onDuty;
  };

  //for debugging
  //FILE *pfile;

  void dumpConvolutionFunction(const casacore::String &outfile, const casacore::Vector<casacore::Float> &f) const;

  void initPerfs();
  void collectPerfs();
  void nextChunk(ROVisibilityIterator &vi);
#if defined(SDGRID_PERFS)
  sdgrid_perfs::ChronoStat cNextChunk;
  sdgrid_perfs::ChronoStat cMatchAllSpwChans;
  sdgrid_perfs::ChronoStat cMatchChannel;
  sdgrid_perfs::ChronoStat cPickWeights;
  sdgrid_perfs::ChronoStat cInterpolateFrequencyToGrid;
  sdgrid_perfs::ChronoStat cSearchValidPointing;
  sdgrid_perfs::ChronoStat cComputeSplines;
  sdgrid_perfs::ChronoStat cResetFrame;
  sdgrid_perfs::ChronoStat cInterpolateDirection;
  sdgrid_perfs::ChronoStat cConvertDirection;
  sdgrid_perfs::ChronoStat cComputeDirectionPixel;
  sdgrid_perfs::ChronoStat cHandleMovingSource;
  sdgrid_perfs::ChronoStat cGridData;
#endif
};

} //# NAMESPACE CASA - END

#endif
