// -*- C++ -*-
//# CFBuffer.h: Definition of the CFBuffer class
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
#ifndef SYNTHESIS_TRANSFORM2_CFBUFFER_H
#define SYNTHESIS_TRANSFORM2_CFBUFFER_H
#include <synthesis/TransformMachines/SynthesisError.h>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <synthesis/TransformMachines2/CFDefs.h>
#include <synthesis/TransformMachines/CFCell.h>
#include <synthesis/TransformMachines2/Utils.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/casa/Utilities/CountedPtr.h>
#include <casacore/casa/Utilities/Sort.h>
#include <casacore/casa/Logging/LogOrigin.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <casacore/casa/Logging/LogSink.h>
#include <casacore/casa/Logging/LogIO.h>

//
// <summary> defines interface for the storage for convolution functions </summary>
// <use visibility=export>
//
// <prerequisite>
// </prerequisite>
//
// <etymology> 
//
//  CFBuffer is the basic in-memory storage for convolution functions
//  as a function of polarization, W-value and frequency at a
//  particular value of Parallactic Angle and baseline type.
//
//</etymology>
//
// <synopsis> 
//
// The CFBuffer class encapsulates the storage and associated
// auxillary information required for the convolution functions.  The
// <linto class=CFStore>CFStore</linkto> class is a collection of
// CFBuffer objects.  A collection of CFStore objects is held and
// managed by the <linto class=FTMachine>FTMachine</linkto> and the
// appropriate one, depending on the casacore::Time/PA value, polarization and
// frequency of the data in the <linkto
// class=VisBuffer2>VisBuffer2</linkto>, is supplied to the <linkto
// class=VisibilityResampler>VisibilityResampler</linkto> object for
// re-sampling the data onto a grid (or vice versa).
//
// Conceptually, this object holds the convolution functions
// parameterized by the properties of the electromagnetic radiation
// (polarization state, frequency and the w-value which is the Fresnel
// term and implicitly a function of the frequency).  The <linkto
// class=CFStore>CFStore</linkto> object holds a list of this object
// index by the telescope related parameters (antenna1,
// antenna2, Parallactic Angle or casacore::Time, etc.).
//
//</synopsis>
//
// <example>
// </example>
//
// <motivation>
//
// To factor out the details of effecient in-memory storage of the
// convolution functions into a separate class.  This class can then
// be optmized by specializations for various types of convolution
// functions and imaging algorithms without the need to change the
// imaging framework.
//
// </motivation>
//

using namespace std;
namespace casacore{

    //    extern template class Array<casacore::CountedPtr<CFBuffer> >;
}

namespace casa {

using namespace casa::vi;
  //  template <class T>
  typedef casacore::Complex TT;
  namespace refim{
    using namespace CFDefs;

  struct CFBStruct {
    CFCStruct *CFBStorage;
    int shape[3];
    casacore::Double *freqValues, *wValues, *pointingOffset;
    casacore::Double fIncr, wIncr;
    casacore::Int **muellerElementsIndex, **muellerElements, 
      **conjMuellerElementsIndex, **conjMuellerElements,
      **conjFreqNdxMap, **freqNdxMap;
    casacore::Int nMueller;
    

    CFCStruct* getCFB(int i, int j, int k)
    { return &(CFBStorage[i + (shape[1]-1)*j + (shape[1]-1)*(shape[2]-1)*k]);}
    //    { return &(CFBStorage[i + (shape[1]-1)*j + (shape[2]-1)*k]);}
  } ;

  class CFBuffer
  {
  public:
    //
    //========================= Administrative Parts ==========================
    //------------------------------------------------------------------
    //
    CFBuffer(): wValues_p(), maxXSupport_p(-1), maxYSupport_p(-1), pointingOffset_p(), cfHitsStats(),
		freqNdxMapsReady_p(false), freqNdxMap_p(), conjFreqNdxMap_p(), cfCacheDirName_p(), maxCFSize_p(-1)
    {};
    
    CFBuffer(casacore::Int maxXSup, casacore::Int maxYSup):
      wValues_p(), maxXSupport_p(maxXSup), maxYSupport_p(maxYSup), pointingOffset_p(), cfHitsStats(),
      freqNdxMapsReady_p(false), freqNdxMap_p(), conjFreqNdxMap_p(), cfCacheDirName_p()
    {
      // storage_p.resize(1,1,1); 
      // storage_p(0,0,0) = new casacore::Array<TT>(dataPtr);
      // coordSys_p.resize(1,1,1); 
      // coordSys_p(0,0,0) = cs;
    };
    
    ~CFBuffer() 
    {
      //cerr << "############### " << "~CFBuffer() called" << endl;
      // casacore::LogIO log_l(casacore::LogOrigin("CFBuffer","~CFBuffer[R&D]"));
      // log_l << "CF Hits stats gathered: " << cfHitsStats << endl;
    };
    
    casacore::CountedPtr<CFBuffer> clone();
    void allocCells(const casacore::Cube<casacore::CountedPtr<CFCell> >& cells);
    void setParams(const CFBuffer& other);
    //
    //============================= casacore::Functional Parts ============================
    //------------------------------------------------------------------
    //
    //    CFBuffer& operator=(const CFBuffer& other);
    //
    // Get the single convolution function as an casacore::Array<T> for the
    // supplied value of the frequency and the muellerElement.
    // Mueller element is essentially the polarization product, but
    // can be any of the of 16 elements of the outer product.
    //
    //-------------------------------------------------------------------------
    //
    inline casacore::Int nChan() {return nChan_p;}
    inline casacore::Int nW() {return nW_p;}
    inline casacore::Int nMuellerElements() {return nPol_p;}
    inline casacore::IPosition shape() {casacore::IPosition shp(3,nChan_p, nW_p, nPol_p); return shp;}
    
    inline casacore::Vector<casacore::Double> getFreqList() {return freqValues_p;};
    inline casacore::Vector<casacore::Double> getWList() {return wValues_p;};
    
    CFCell& getCFCell(const casacore::Double& freqVal, const casacore::Double& wValue, 
		      const casacore::Int & muellerElement); 
    // muellerElement: (i,j) of the Mueller Matrix
    casacore::CountedPtr<CFCell>& getCFCellPtr(const casacore::Double& freqVal, const casacore::Double& wValue, 
				     const casacore::Int & muellerElement); 
    CFCell& operator()(const casacore::Int& i, const casacore::Int& j, const casacore::Int& k) {return *cfCells_p(i,j,k);}
    CFCell& getCFCell(const casacore::Int& i, const casacore::Int& j, const casacore::Int& k);

    casacore::CountedPtr<CFCell >& getCFCellPtr(const casacore::Int& i, const casacore::Int& j, const casacore::Int& k);
    
    //=========================================================================
    casacore::Array<TT>& getCF(const casacore::Double& freqVal, const casacore::Double& wValue, 
		     const casacore::Int & muellerElement)
    {return *(getCFCell(freqVal, wValue, muellerElement).storage_p);}
    // muellerElement: (i,j) of the Mueller Matrix
    
    casacore::CountedPtr<casacore::Array<TT> >& getCFPtr(const casacore::Double& freqVal, const casacore::Double& wValue, 
				     const casacore::Int & muellerElement) 
    {return getCFCellPtr(freqVal, wValue, muellerElement)->storage_p;}
    
    casacore::Array<TT>& getCF(const casacore::Int& i, const casacore::Int& j, const casacore::Int& k)
    {return *(getCFCell(i,j,k).storage_p);}
    
    casacore::CountedPtr<casacore::Array<TT> >& getCFPtr(const casacore::Int& i, const casacore::Int& j, const casacore::Int& k)
    {return getCFCellPtr(i,j,k)->storage_p;}
    
    
    //
    // Get the parameters of a the CFs indexed by values.  The version
    // which returns also the casacore::Coordinate System associated with the
    // CFs are slow (casacore::CoordinateSystem::operator=() is surprisingly
    // expensive!).  So do not use this in tight loops.  If it is
    // required, use the version without the co-ordinate system below.
    //
    void getParams(casacore::CoordinateSystem& cs, casacore::Float& sampling, 
		   casacore::Int& xSupport, casacore::Int& ySupport, casacore::String& bandName,
		   const casacore::Double& freqVal, const casacore::Double& wValue, 
		   const casacore::Int& muellerElement);
    //-------------------------------------------------------------------------
    // Get CF by directly indexing in the list of CFs (data vector)
    inline void getParams(casacore::CoordinateSystem& cs, casacore::Float& sampling, 
			  casacore::Int& xSupport, casacore::Int& ySupport, casacore::String& bandName,
			  const casacore::Int& i, const casacore::Int& j, const casacore::Int& k)
    {
      cs = cfCells_p(i,j,k)->coordSys_p;
      sampling = cfCells_p(i,j,k)->sampling_p;
      xSupport = cfCells_p(i,j,k)->xSupport_p;
      ySupport = cfCells_p(i,j,k)->ySupport_p;
      bandName = cfCells_p(i,j,k)->bandName_p;
    }
    void getParams(casacore::Double& freqVal, casacore::Float& sampling, 
		   casacore::Int& xSupport, casacore::Int& ySupport, 
		   const casacore::Int& iFreq, const casacore::Int& iW, const casacore::Int& iPol)
    {
      sampling = cfCells_p(iFreq,iW,iPol)->sampling_p;
      xSupport = cfCells_p(iFreq,iW,iPol)->xSupport_p;
      ySupport = cfCells_p(iFreq,iW,iPol)->ySupport_p;
      freqVal = freqValues_p(iFreq);
    }
    
    inline void getCoordList(casacore::Vector<casacore::Double>& freqValues, casacore::Vector<casacore::Double>& wValues,
			     PolMapType& muellerElementsIndex, PolMapType& muellerElements, 
			     PolMapType& conjMuellerElementsIndex, PolMapType& conjMuellerElements, 
			     casacore::Double& fIncr, casacore::Double& wIncr)
    {
      freqValues.assign(freqValues_p);wValues.assign(wValues_p);
      muellerElements.assign(muellerElements_p);         muellerElementsIndex.assign(muellerElementsIndex_p);
      conjMuellerElements.assign(conjMuellerElements_p); conjMuellerElementsIndex.assign(conjMuellerElementsIndex_p);
      fIncr = freqValIncr_p; wIncr = wValIncr_p;
    }
    
    casacore::Int nearestNdx(const casacore::Double& val, const casacore::Vector<casacore::Double>& valList, const casacore::Double& incr);
    
    casacore::Int nearestFreqNdx(const casacore::Double& freqVal) ;
    
    inline casacore::Int nearestWNdx(const casacore::Double& wVal) 
    {
      //      return SynthesisUtils::nint(sqrt(wValIncr_p*abs(wVal)));
      return max(0,min((int)(sqrt(wValIncr_p*abs(wVal))),(int)wValues_p.nelements())-1);
      // Int ndx=(int)(sqrt(wValIncr_p*abs(wVal)));
      // if ((uInt)ndx >= wValues_p.nelements())
      // 	cerr << endl << endl << ndx << " " <<  wVal << " " << wValIncr_p << endl << endl;
      // return min(ndx,wValues_p.nelements()-1);
    }
    
    casacore::Double nearest(casacore::Bool& found, const casacore::Double& val, const casacore::Vector<casacore::Double>& valList, const casacore::Double& incr);
    
    inline casacore::Double nearestFreq(casacore::Bool& found, const casacore::Double& freqVal)
    {return nearest(found, freqVal, freqValues_p, freqValIncr_p);}
    
    inline casacore::Double nearestWVal(casacore::Bool& found, const casacore::Double& wVal)
    {return nearest(found, wVal, wValues_p, wValIncr_p);}
    
    //-------------------------------------------------------------------------
    //
    // Generate a map for the given frequency and Mueller element list
    // to the index in the internal list of CFs.  This can be used in
    // tight loops to get get direct access to the required CF.
    //
    void makeCFBufferMap(const casacore::Vector<casacore::Double>& freqVals, 
			 const casacore::Vector<casacore::Double>& wValues,
			 const MuellerMatrixType& muellerElements);
    //-------------------------------------------------------------------------
    //
    // Add a Convolution casacore::Function with associated parameters.
    //
    void addCF(casacore::Array<TT>*, //dataPtr, 
	       casacore::CoordinateSystem&,// cs, 
	       casacore::Float& ,//sampling, 
	       casacore::Int& ,//xSupport, 
	       casacore::Int& ,//ySupport,
	       casacore::Double& ,//freqValue, 
	       casacore::Double& ,//wValue, 
	       casacore::Int& //muellerElement
	       )
    {throw(casacore::AipsError("CFBuffer::addCF called"));}
    //-------------------------------------------------------------------------
    //
    void resize(const casacore::IPosition& size) {cfCells_p.resize(size);};
    void resize(const casacore::Double& wIncr, const casacore::Double& freqIncr,
		const casacore::Vector<casacore::Double>& wValues, 
		const casacore::Vector<casacore::Double>& freqValues,
		const PolMapType& muellerElements,
		const PolMapType& muellerElementsIndex,
		const PolMapType& conjMuellerElements,
		const PolMapType& conjMuellerElementsIndex);
    casacore::Int noOfMuellerElements(const PolMapType& muellerElements);
    //-------------------------------------------------------------------------
    // Set only the CF parameters.  Return to index of the CF that was set.
    //
    casacore::RigidVector<casacore::Int, 3> setParams(const casacore::Int& inu, const casacore::Int& iw, const casacore::Int& ipx, const casacore::Int& ipy,
					    const casacore::Double& freqValue,
					    const casacore::Double& wValue,
					    const casacore::Int& muellerElement,
					    casacore::CoordinateSystem& cs,
					    const casacore::TableRecord& miscInfo);


    casacore::RigidVector<casacore::Int, 3> setParams(const casacore::Int& i, const casacore::Int& j, const casacore::Int& ipx, const casacore::Int& ipy,
						      const casacore::Double& freqValue, const casacore::String& bandName,
						      const casacore::Double& wValue, 
						      const casacore::Int& muellerElement,
						      casacore::CoordinateSystem& cs,
						      casacore::Float& sampling,
						      casacore::Int& xSupport, casacore::Int& ySupport,
						      const casacore::String& fileName=casacore::String(),
						      const casacore::Double& conjFreq=0.0,
						      const casacore::Int& conjPol=-1,
						      const casacore::String& telescopeName=casacore::String(),
						      const casacore::Float& diameter=25.0);
    // casacore::RigidVector<casacore::Int, 3> setParams(const casacore::Int& inu, const casacore::Int& iw, const casacore::Int& muellerElement,
    // 				  const casacore::TableRecord& miscInfo);
    void setPointingOffset(const casacore::Vector<casacore::Vector<casacore::Double> >& offset) 
    {pointingOffset_p.assign(offset);};
    casacore::Vector<casacore::Vector<casacore::Double> > getPointingOffset() {return pointingOffset_p;};
    //
    // Also set the size of the CF in x and y.
    //
    void setParams(casacore::Int& nx, casacore::Int& ny, casacore::CoordinateSystem& cs, casacore::Float& sampling, 
		   casacore::Int& xSupport, casacore::Int& ySupport, 
		   const casacore::Double& freqVal, const casacore::Double& wValue, 
		   const casacore::Int& muellerElement,
		   const casacore::String& fileName);
    void setPA(casacore::Float& pa);
    void setDir(const casacore::String& Dir) {cfCacheDirName_p=Dir;}
    void clear();
    const casacore::String& getCFCacheDir() {return cfCacheDirName_p;};

    casacore::RigidVector<casacore::Int,3> getIndex(const casacore::Double& freqVal, const casacore::Double& wValue, 
				const casacore::Int& muellerElement);
    //-------------------------------------------------------------------------
    //
    // Copy just the parameters from other to this.
    //
    void copyParams(const CFBuffer& other)
    {
      cfCells_p = other.cfCells_p;
      // coordSys_p = other.coordSys_p; sampling_p.assign(other.sampling_p); 
      // xSupport_p.assign(other.xSupport_p); ySupport_p.assign(other.ySupport_p);
      maxXSupport_p=other.maxXSupport_p;  maxYSupport_p=other.maxYSupport_p; 
    }
    //-------------------------------------------------------------------------
    //
    // Write the description of the storage on the supplied ostream.
    // Used mostly for debugging, but might be useful for user
    // feedback/logging.
    //
    void show(const char *Mesg=NULL,ostream &os=cerr);
    //
    // Returns true if the internal storage is not yet initialized.
    //
    casacore::Bool null() {return (cfCells_p.nelements() == 0);};
    
    casacore::Cube<casacore::CountedPtr<CFCell> >& getStorage() {return cfCells_p;};
    void makePersistent(const char *dir, const char *cfName="");
    
    void primeTheCache();
    void initMaps(const VisBuffer2& vb,const casacore::Matrix<casacore::Double>& freqSelection,const casacore::Double& imRefFreq);
    void initPolMaps(PolMapType& polMap, PolMapType& conjPolMap);
    //
    // For CUDA kernel
    //
    void getFreqNdxMaps(casacore::Vector<casacore::Vector<casacore::Int> >& freqNdx, casacore::Vector<casacore::Vector<casacore::Int> >& conjFreqNdx);
    inline casacore::Int nearestFreqNdx(const casacore::Int& spw, const casacore::Int& chan, const casacore::Bool conj=false)
    {
      // Single pixel along the freq. axis but chan, spw > 0 indicates
      // use of W-only projection on a wide-band data.
      if (nChan_p == 1) return 0; 
      if (conj) return conjFreqNdxMap_p[spw][chan];
      else  return freqNdxMap_p[spw][chan];
    }
    
    void getAsStruct(CFBStruct& st);
    
    static void initCFBStruct(CFBStruct& cfbSt) 
    {
      cfbSt.CFBStorage=NULL;
      cfbSt.freqValues=NULL;
      cfbSt.wValues=NULL;
      cfbSt.muellerElementsIndex=NULL;
      cfbSt.muellerElements=NULL;
      cfbSt.conjMuellerElementsIndex=NULL;
      cfbSt.conjMuellerElements=NULL;
      cfbSt.shape[0]=cfbSt.shape[1]=cfbSt.shape[2]=0;
      cfbSt.fIncr=cfbSt.wIncr=0.0;
    }
    void fill(const casacore::Int& nx, const casacore::Int& ny, 
	      const casacore::Vector<casacore::Double>& freqValues,
	      const casacore::Vector<casacore::Double>& wValues,
	      const PolMapType& muellerElements);
    
    casacore::IPosition getShape() {return cfCells_p.shape();}

    int getMaxCFSize();
    bool finitePointingOffsets();
    //
    //============================= Protected Parts ============================
    //------------------------------------------------------------------
    //
  protected:
    //
    // The storage buffer for the pixel values in CFCell is casacore::Array<T>
    // rather than casacore::Matrix<T> to accomodate rotationally symmetric CFs
    // (like the Prolate Spheroidal) which can be held as a casacore::Vector of
    // values.
    //
    casacore::Cube<casacore::CountedPtr<CFCell> > cfCells_p;// freqValues x wValues x muellerElements
    casacore::Vector<casacore::Double> wValues_p, freqValues_p;
    PolMapType muellerElements_p, muellerElementsIndex_p,conjMuellerElements_p,conjMuellerElementsIndex_p; 
    casacore::Double wValIncr_p, freqValIncr_p;
    MuellerMatrixType muellerMask_p;
    
    casacore::Int nPol_p, nChan_p, nW_p, maxXSupport_p, maxYSupport_p;
    casacore::Vector<casacore::Vector<casacore::Double> > pointingOffset_p;
    casacore::Cube<casacore::Int> cfHitsStats;
    casacore::Bool freqNdxMapsReady_p;
    casacore::Vector<casacore::Vector<casacore::Int> > freqNdxMap_p, conjFreqNdxMap_p;
    void ASSIGNVVofI(casacore::Int** &target,casacore::Vector<casacore::Vector<casacore::Int> >& source, casacore::Bool& doAlloc);
    casacore::String cfCacheDirName_p;

    int maxCFSize_p;
  };

  // declare a commonly used template extern
  
} // end namesapce refim
} // end namespace casa

#endif
