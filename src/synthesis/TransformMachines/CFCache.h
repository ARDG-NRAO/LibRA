// -*- C++ -*-
//# ConvFuncDiskCache.cc: Definition of the ConvFuncDiskCache class
//# Copyright (C) 1997,1998,1999,2000,2001,2002,2003
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
#ifndef SYNTHESIS_CFCACHE_H
#define SYNTHESIS_CFCACHE_H

#include <casacore/casa/Arrays/Matrix.h>
#include <msvis/MSVis/VisBuffer.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/TempImage.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/OS/Directory.h>
#include <casacore/casa/Logging/LogSink.h>
#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/lattices/Lattices/LatticeCache.h>
#include <casacore/lattices/Lattices/ArrayLattice.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <synthesis/TransformMachines/VPSkyJones.h>
#include <synthesis/TransformMachines/CFStore.h>
#include <synthesis/TransformMachines/CFStore2.h>
#include <synthesis/TransformMachines/CFDefs.h>
#include <synthesis/TransformMachines/Utils.h>
// #include <casa/Tables/Table.h>

namespace casa { //# NAMESPACE CASA - BEGIN
  using namespace CFDefs;
  // <summary> 
  //
  // An object to manage the caches of pre-computed convolution
  // functions on the disk and in the memory.
  //
  // </summary>
  
  // <use visibility=export>
  
  // <reviewed reviewer="" date="" tests="" demos="">
  
  // <prerequisite>
  //  CFStore class
  // </prerequisite>
  //
  // <etymology> 
  //
  // CFCache is an object, to write convolution
  // functions from the memory cache to the disk cache, and
  // search/load the disk cache for convolution functions for a give
  // Parallactic Angle.  
  //
  //</etymology>
  //
  // <synopsis> 
  //
  // FTMachines uses pre-computed convolution functions for gridding
  // (FTMachine::put()) and prediction (FTMachine::get()).  For
  // <linkto class=PBWProjectFT>PBWProjectFT</linkto>, the convolution
  // function computation is expensive.  Once computed, it is
  // efficient to cache these functions as a function of Parallactic
  // Angle and the value of the w-term (if significant). 
  //
  // CFCache class provides interface to the disk cache and
  // is used in <linkto class=PBWProjectFT>PBWProjectFT</linkto> to
  // search and load convolution functions from the disk.  If a new
  // convolution function is computed in <linkto
  // class=PBWProjectFT>PBWProjectFT</linkto>, the disk cache is
  // updated using the services of this class as well.
  //
  // </synopsis> 
  //
  // <example>
  // </example>
  //
  // <motivation>
  //
  // Factor out the code for managing convolution function caches
  // (memory and disk caches) from the FTMachine code.  This is a
  // specialized service, and has nothing to do with the details of
  // gridding and prediction of visibilities (which is the function of
  // FTMachines).
  //
  // </motivation>
  //
  // <todo asof="2005/07/21">
  //
  // <ul> Generalize to handle convolution functions for inhomogeneous
  // arrays and multi-feed antennas.
  //
  // </todo>
  //
  //----------------------------------------------------------------------
  class CFCacheTable
  {
  public:
    CFCacheTable(): freqList(), wList(), muellerList(), cfNameList() {};
    ~CFCacheTable() {};
    
    CFCacheTable& operator=(const CFCacheTable& other)
    {
      //      if (other != *this)
	{
	  freqList = other.freqList;
	  wList = other.wList;
	  muellerList = other.muellerList;
	  cfNameList = other.cfNameList;
	}
      return *this;
    }

    void init()
    {freqList.resize(0); wList.resize(0); muellerList.resize(0); cfNameList.resize(0);}

    std::vector<casacore::Double> freqList, wList;
    std::vector<casacore::Int> muellerList;
    std::vector<casacore::String> cfNameList;
  };
  //
  //----------------------------------------------------------------------
  //
  class CFCache
  {
  public:
    typedef casacore::Vector< CFStore > CFStoreCacheType;
    typedef casacore::Vector< CFStore2 > CFStoreCacheType2;
    typedef std::vector<CFCacheTable> CFCacheTableType;

    CFCache(const char *cfDir="CF"):
      memCache2_p(), memCacheWt2_p(),memCache_p(), memCacheWt_p(), 
      cfCacheTable_p(), XSup(), YSup(), paList(), 
      paList_p(), key2IndexMap(),
      Dir(""), WtImagePrefix(""), cfPrefix(cfDir), aux("aux.dat"), paCD_p(), avgPBReady_p(False),
      avgPBReadyQualifier_p(""), OTODone_p(False), loadPixBuf_p(True)
    {};
    CFCache& operator=(const CFCache& other);
    ~CFCache();
    //
    // Method to set the disk cache directory name
    //
    void setCacheDir(const char *dir) {Dir = casacore::String(dir);}
    casacore::String getCacheDir(CFCDirType dirType=USERDIR) {(void)dirType;return Dir;};

    void setWtImagePrefix(const char *prefix) {WtImagePrefix = prefix;}
    casacore::String getWtImagePrefix() {return WtImagePrefix;};

    void setLazyFill(const Bool& val);
    Bool isLazyFillOn() {return loadPixBuf_p;};
    //
    // Method to initialize the internal memory cache.
    //
    void initCache();
    void initCache2(casacore::Bool verbose=false, casacore::Float selectedPA=400.0, casacore::Float dPA=-1.0);
    void initCacheFromList2(const casacore::String& path, 
			    const casacore::Vector<casacore::String>& cfFileNames, 
			    const casacore::Vector<casacore::String>& cfWtFileNames, 
			    casacore::Float selectedPA, casacore::Float dPA,
			    const casacore::Int verbose=1);
    void initPolMaps(PolMapType& polMap, PolMapType& conjPolMap);
    inline casacore::Bool OTODone() {return OTODone_p;}
    //
    // Compute the size of the memory cache in bytes
    //
    casacore::Long size();
    //
    // Method to set the class to caluclate the differential
    // Parallactic Angle.  The ParAngleChangeDetector also holds the
    // delta PA value (user defined).
    //
    void setPAChangeDetector(const ParAngleChangeDetector& paCD) {paCD_p = paCD;};
    //
    // Methods to cache the convolution function.
    //
    // Top level method interfacing with the CFStore object
    //-------------------------------------------------------------------
    void cacheConvFunction(CFStore& cfs, 
			   casacore::String nameQualifier="",casacore::Bool savePA=true)
    {cacheConvFunction(cfs.pa, cfs,nameQualifier,savePA);}
    //-------------------------------------------------------------------
    // One level lower - the Parallactic angle can be separately
    // provided.
    void cacheConvFunction(const casacore::Quantity pa, CFStore& cfs,
			   casacore::String nameQualifier="",casacore::Bool savePA=true)
    {cacheConvFunction(pa.getValue("rad"), cfs, nameQualifier,savePA);}
    //-------------------------------------------------------------------
    // The Parallactic angle as a floating point number in radians.
    void cacheConvFunction(const casacore::Float pa, CFStore& cfs, 
			   casacore::String nameQualifier="",casacore::Bool savePA=true);
    //-------------------------------------------------------------------
    // Lowest level - all information about CFStore is explicitly
    // provided as basic types
    casacore::Int cacheConvFunction(casacore::Int which, const casacore::Float& pa, CFType& cf, 
			  casacore::CoordinateSystem& coords, casacore::CoordinateSystem& ftcoords, 
			  casacore::Int& convSize, 
			  casacore::Vector<casacore::Int>& xConvSupport, casacore::Vector<casacore::Int>& yConvSupport, 
			  casacore::Float convSampling, casacore::String nameQualifier="",casacore::Bool savePA=true);
    //-------------------------------------------------------------------
    // Methods to sarch for a convolution function in the caches (disk
    // or memory) for the give Parallactic Angle value.
    //
    casacore::Bool searchConvFunction(casacore::Int& which, const casacore::Quantity pa, const casacore::Quantity dPA )
    {return searchConvFunction(which, pa.getValue("rad"), dPA.getValue("rad"));};

    casacore::Bool searchConvFunction(casacore::Int& which, const casacore::Float pa, const casacore::Float dPA );
    //
    // Lower level method to load a convolution function from the disk.
    //
    casacore::Int loadFromDisk(casacore::Int where, casacore::Float pa, casacore::Float dPA,
		     casacore::Int Nx, CFStoreCacheType & convFuncCache,
		     CFStore& cfs, casacore::String nameQualifier="");
    //
    // Method to locate a convolution function for the given w-term
    // index and PA value.  This is the top level function that must
    // be used by the clients.  This uses searchConvFunction() and
    // loadFromDisk() methods and the private methods to return a
    // convolution function.
    //
    // Returns CFDefs::NOTCACHED if the convolution function was not
    // found in the cache, CFDefs::MEMCACHE or CFDefs::DISKCACHE if
    // the function was found in memory or disk cache respectively.
    //
    casacore::Int locateConvFunction(CFStore& cfs, CFStore& cftws, const casacore::Int Nw, 
			   const casacore::Quantity pa, const casacore::Quantity dPA, 
			   const casacore::Int mosXPos=0, const casacore::Int mosYPos=0)
    {return locateConvFunction(cfs, cftws, Nw,pa.getValue("rad"), dPA.getValue("rad"),mosXPos,mosYPos);};

    casacore::Int locateConvFunction(CFStore& cfs, const casacore::Int Nw, 
			   const casacore::Quantity pa, const casacore::Quantity dPA, 
			   const casacore::String& nameQualifier="",
			   const casacore::Int mosXPos=0, const casacore::Int mosYPos=0)
    {return locateConvFunction(cfs, Nw,pa.getValue("rad"), dPA.getValue("rad"),nameQualifier, mosXPos,mosYPos);};

    casacore::Int locateConvFunction(CFStore& cfs, CFStore& cfwts,
			   const casacore::Int Nw, const casacore::Float pa, const casacore::Float dPA,
			   const casacore::Int mosXPos=0, const casacore::Int mosYPos=0);

    casacore::Int locateConvFunction(CFStore& cfs, const casacore::Int Nw, const casacore::Float pa, const casacore::Float dPA, 
			   const casacore::String& nameQualifier="",
			   const casacore::Int mosXPos=0, const casacore::Int mosYPos=0);

    casacore::TableRecord getCFParams(const casacore::String& fileName,
		     casacore::Array<casacore::Complex>& pixelBuffer,
		     casacore::CoordinateSystem& coordSys, 
		     casacore::Double& sampling,
		     casacore::Double& paVal,
		     casacore::Int& xSupport, casacore::Int& ySupport,
		     casacore::Double& fVal, casacore::Double& wVal, casacore::Int& mVal,
		     casacore::Double& conjFreq, casacore::Int& conjPoln,
		     casacore::Bool loadPixels=true);
    //
    // Methods to write the auxillary information from the memory
    // cache to the disk cache.  Without this call, the disk cache
    // might not be complete.  It is safe to call this method at
    // anytime during the life of this object.
    //
    void flush();
    void flush(casacore::ImageInterface<casacore::Float>& avgPB, casacore::String qualifier=casacore::String(""));
    casacore::Int loadAvgPB(casacore::ImageInterface<casacore::Float>& avgPB, casacore::String qualifier=casacore::String(""));
    casacore::Int loadAvgPB(casacore::CountedPtr<casacore::ImageInterface<casacore::Float> > & avgPB, casacore::String qualifier=casacore::String(""))
    {if (avgPB.null()) avgPB = new casacore::TempImage<casacore::Float>(); return loadAvgPB(*avgPB,qualifier);};

    // loadAvgPB calls the method below if WtImgPrefix was set.
    casacore::Int loadWtImage(casacore::ImageInterface<casacore::Float>& avgPB, casacore::String qualifier);

    casacore::Bool avgPBReady(const casacore::String& qualifier=casacore::String("")) 
    {return (avgPBReady_p && (avgPBReadyQualifier_p == qualifier));};

    void summarize(CFStoreCacheType2& memCache, const casacore::String& message, const casacore::Bool cfsInfo=true);

    CFStoreCacheType2 memCache2_p, memCacheWt2_p;

  private:
    CFStoreCacheType memCache_p, memCacheWt_p;
    CFCacheTableType cfCacheTable_p;

    casacore::Matrix<casacore::Int> XSup, YSup;
    casacore::Vector<casacore::Float> paList, Sampling;
    std::vector<casacore::Float> paList_p;
    casacore::Matrix<casacore::Float> key2IndexMap; // Nx2 [PAVal, Freq]
    casacore::String Dir, WtImagePrefix, cfPrefix, aux;
    ParAngleChangeDetector paCD_p;
    //
    // Internal method to convert the direction co-ordinates of the
    // given casacore::CoordinateSystem to its Fourier conjuguate co-ordinates.
    //
    void makeFTCoordSys(const casacore::CoordinateSystem& coords,
			const casacore::Int& convSize,
			const casacore::Vector<casacore::Double>& ftRef,
			casacore::CoordinateSystem& ftCoords);
    //
    // Internal method to add the given convolution function to the
    // memory cache.
    //
    casacore::Int addToMemCache(CFStoreCacheType& cfCache, 
		      casacore::Float pa, CFType* cf, casacore::CoordinateSystem& coords,
		      casacore::Vector<casacore::Int>& xConvSupport,
		      casacore::Vector<casacore::Int>& yConvSupport,
		      casacore::Float convSampling);
    CFStoreCacheType& getMEMCacheObj(const casacore::String& nameQualifier);

    void fillCFSFromDisk(const casacore::Directory dirObj, const casacore::String& pattern, 
			 CFStoreCacheType2& memStore, casacore::Bool showInfo=false, 
			 casacore::Float selectPAVal=400.0, casacore::Float dPA=-1.0,
			 const casacore::Int verbose=1);
    void fillCFListFromDisk(const casacore::Vector<casacore::String>& fileNames, const casacore::String& CFCDir,
			    CFStoreCacheType2& memStore,
			    casacore::Bool showInfo, casacore::Float selectPAVal, casacore::Float dPA,
			    const casacore::Int verbose=1);

    casacore::Bool avgPBReady_p;
    casacore::String avgPBReadyQualifier_p;
    casacore::Bool OTODone_p, loadPixBuf_p;
  };
}

#endif
