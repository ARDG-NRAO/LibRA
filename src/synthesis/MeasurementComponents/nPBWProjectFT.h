//# nPBWProjectFT.h: Definition for nPBWProjectFT
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
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$

#ifndef SYNTHESIS_PBWPROJECTFT_H
#define SYNTHESIS_PBWPROJECTFT_H

#include <synthesis/TransformMachines/FTMachine.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/scimath/Mathematics/FFTServer.h>
#include <msvis/MSVis/VisBuffer.h>
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
#include <synthesis/TransformMachines/VPSkyJones.h>
#include <synthesis/TransformMachines/VLACalcIlluminationConvFunc.h>
#include <synthesis/TransformMachines/VLAIlluminationConvFunc.h>
#include <synthesis/TransformMachines/Utils.h>

//#include <synthesis/MeasurementComponents/EPJones.h>
#include <synthesis/MeasurementComponents/SolvableVisCal.h>
#include <synthesis/MeasurementComponents/ConvFuncDiskCache.h>

namespace casa { //# NAMESPACE CASA - BEGIN
  
  // <summary>  An FTMachine for Gridded Fourier transforms including effects of primary beam and pointing offsets and the w-term</summary>
  
  // <use visibility=export>
  
  // <reviewed reviewer="" date="" tests="" demos="">
  
  // <prerequisite>
  //   <li> <linkto class=FTMachine>FTMachine</linkto> module
  //   <li> <linkto class=SkyEquation>SkyEquation</linkto> module
  //   <li> <linkto class=VisBuffer>VisBuffer</linkto> module
  //   <li> <linto class=EPJones>EPJones</linkto> module
  // </prerequisite>
  //
  // <etymology> 
  // FTMachine is a Machine for Fourier Transforms. Like
  // WProjectFT, nPBWProjectFT does Grid-based Fourier transforms but
  // also includes the effects of primary beam and antenna pointing
  // offsets. 
  // </etymology>
  //
  // <synopsis> 
  //
  // The <linkto class=SkyEquation>SkyEquation</linkto> needs to be
  // able to perform Fourier transforms on visibility
  // data. nPBWProjectFT allows efficient handling of direction
  // dependent effects due to the primary beam and antenna pointing
  // offsets using a <linkto class=VisBuffer>VisBuffer</linkto> which
  // encapsulates a chunk of visibility (typically all baselines for
  // one time) together with all the information needed for processing
  // (e.g. UVW coordinates).
  //
  // Using this FTMachine, errors due antenna pointing offsets can be
  // corrected during deconvolution.  One form of antenna pointing
  // error which is known a-priori is the VLA polarization squint
  // (about 6% of the Primary beam width at any frequency).  For
  // casacore::Stokes imaging, using this FTMachine, the VLA polarization squint
  // and beam polarization can also be corrected.  Also since the
  // effects of antenna pointing errors is strongest in the range of
  // 1-2GHz band (where the sky is not quite empty while the beams are
  // not too large either), this FTMachine can also be setup to
  // correct for the w-term.
  //
  // Switches are provided in the get() method to compute the
  // derivatives with respect to the parameters of the primary beam
  // (only pointing offsets for now).  This is used in the pointing
  // offset solver.
  //
  // See the documentation of other FTMachines for details about the
  // design of the FTMachines in general.
  //
  // </synopsis> 
  //
  // <example>
  // See the example for <linkto class=SkyModel>SkyModel</linkto>.
  // </example>
  //
  // <motivation>
  //
  // Encapsulate the correction of direction dependent effects via
  // visibility plane convolutions with a potentially different
  // convolution function for each baseline.
  //
  // </motivation>
  //
  // <todo asof="2005/07/21">
  //
  // <ul> Include the antenna based complex gain term as well since
  // that can interfere with the effects of pointing offsets.  
  //
  // <ul> Factor out the actual convolution functions as a separate
  // class making FTMachines for various direction dependent effects
  // generic.
  //
  // </todo>
  
  //  class EPJones;
  class SolvableVisJones;
  class nPBWProjectFT : public FTMachine {
  public:
    
    // Constructor: cachesize is the size of the cache in words
    // (e.g. a few million is a good number), tilesize is the
    // size of the tile used in gridding (cannot be less than
    // 12, 16 works in most cases). 
    // <group>
    nPBWProjectFT(casacore::Int nFacets, casacore::Long cachesize, casacore::String& cfCacheDirName,
		  casacore::Bool applyPointingOffset=true,
		  casacore::Bool doPBCorr=true,
		  casacore::Int tilesize=16, 
		  casacore::Float paSteps=5.0, casacore::Float pbLimit=5e-2,
		  casacore::Bool usezero=false);
    // </group>
    
    // Construct from a casacore::Record containing the nPBWProjectFT state
    nPBWProjectFT(const casacore::RecordInterface& stateRec);
    
    // Copy constructor
    nPBWProjectFT(const nPBWProjectFT &other);
    
    // Assignment operator
    nPBWProjectFT &operator=(const nPBWProjectFT &other);
    
    ~nPBWProjectFT();
    
    //   void setEPJones(EPJones* ep_j) {epJ = ep_j;}
    void setEPJones(SolvableVisJones* ep_j) {epJ = ep_j;}
    
    void setDOPBCorrection(casacore::Bool doit=true) {doPBCorrection=doit;};
    // Initialize transform to Visibility plane using the image
    // as a template. The image is loaded and Fourier transformed.
    
    virtual void initializeToVis(casacore::ImageInterface<casacore::Complex>& image,
			 const VisBuffer& vb);
    // This version returns the gridded vis...should be used in conjunction 
    // with the version of 'get' that needs the gridded visdata 
    virtual void initializeToVis(casacore::ImageInterface<casacore::Complex>& image,
			 const VisBuffer& vb, casacore::Array<casacore::Complex>& griddedVis,
			 casacore::Vector<casacore::Double>& uvscale);
    
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
    
    virtual void initVisBuffer(VisBuffer& vb, Type whichVBColumn);
    void initVisBuffer(VisBuffer& vb, Type whichVBColumn, casacore::Int row);

    // Get actual coherence from grid by degridding
    void get(VisBuffer& vb, casacore::Int row=-1);
    
    // Get the coherence from grid return it in the degrid 
    // is used especially when scratch columns are not 
    // present in ms.
    void get(VisBuffer& vb, casacore::Cube<casacore::Complex>& degrid, 
	     casacore::Array<casacore::Complex>& griddedVis, casacore::Vector<casacore::Double>& scale, 
	     casacore::Int row=-1);
    
    void get(VisBuffer& vb, casacore::Cube<casacore::Float>& pointingOffsets, casacore::Int row=-1,
	     Type whichVBColumn=FTMachine::MODEL,casacore::Int Conj=0)
    {
      get(vb,vb,vb,pointingOffsets,row,whichVBColumn,whichVBColumn,Conj,0);
    }
    
    void get(VisBuffer& vb, VisBuffer& gradAzVB,VisBuffer& gradElVB,
	     casacore::Cube<casacore::Float>& pointingOffsets,casacore::Int row=-1,
	     Type whichVBColumn=FTMachine::MODEL,
	     Type whichGradVBColumn=FTMachine::MODEL,
	     casacore::Int Conj=0, casacore::Int doGrad=1) ;
  void nget(VisBuffer& vb,
	    // These offsets should be appropriate for the VB
	    casacore::Array<casacore::Float>& l_off, casacore::Array<casacore::Float>& m_off,
	    casacore::Cube<casacore::Complex>& Mout,
	    casacore::Cube<casacore::Complex>& dMout1,
	    casacore::Cube<casacore::Complex>& dMout2,
	    casacore::Int Conj=0, casacore::Int doGrad=1);
    // Get the coherence from grid return it in the degrid 
    // is used especially when scratch columns are not 
    // present in ms.
    void get(VisBuffer& vb, casacore::Cube<casacore::Complex>& degrid, 
	     casacore::Array<casacore::Complex>& griddedVis, casacore::Vector<casacore::Double>& scale, 
	     casacore::Cube<casacore::Float>& pointingOffsets,casacore::Int row=-1);
    
    
    
    
    
    
    // Put coherence to grid by gridding.
    void put(const VisBuffer&,
	     casacore::TempImage<casacore::Complex>&, casacore::Vector<casacore::Double>&, int,
	     casacore::UVWMachine*, casacore::Bool) 
    {
      //    throw(casacore::AipsError("nPBWProjectFT::put is not implemented"));
    }
    void put(const VisBuffer& vb, casacore::Int row=-1, casacore::Bool dopsf=false,
	     FTMachine::Type type=FTMachine::OBSERVED);
    
    // Make the entire image
    void makeImage(FTMachine::Type type,
		   VisSet& vs,
		   casacore::ImageInterface<casacore::Complex>& image,
		   casacore::Matrix<casacore::Float>& weight);
    
    // Get the final image: do the Fourier transform and
    // grid-correct, then optionally normalize by the summed weights
    virtual casacore::ImageInterface<casacore::Complex>& getImage(casacore::Matrix<casacore::Float>&, casacore::Bool normalize=true);
    virtual void normalizeImage(casacore::Lattice<casacore::Complex>& /*skyImage*/,
			      const casacore::Matrix<casacore::Double>& /*sumOfWts*/,
			      casacore::Lattice<casacore::Float>& /*sensitivityImage*/,
			      casacore::Bool /*fftNorm*/)
    {throw(casacore::AipsError("nPBWProjectFT::normalizeImage() called"));}
    
    // Get the final weights image
    void getWeightImage(casacore::ImageInterface<casacore::Float>&, casacore::Matrix<casacore::Float>&);
    
    // Save and restore the nPBWProjectFT to and from a record
    casacore::Bool toRecord(casacore::String& error, casacore::RecordInterface& outRec, 
		  casacore::Bool withImage=false, const casacore::String diskimage="");
    casacore::Bool fromRecord(casacore::String& error, const casacore::RecordInterface& inRec);
    
    // Can this FTMachine be represented by Fourier convolutions?
    casacore::Bool isFourier() {return true;}
    
    //  casacore::Bool changed(const VisBuffer& vb) {return vpSJ->changed(vb,1);};
    casacore::Bool changed(const VisBuffer& ) {return false;}
    
    virtual casacore::Int findPointingOffsets(const VisBuffer&, casacore::Array<casacore::Float>&, casacore::Array<casacore::Float>&,
				    casacore::Bool Evaluate=true);
    virtual casacore::Int findPointingOffsets(const VisBuffer&, casacore::Cube<casacore::Float>&,
			    casacore::Array<casacore::Float>&, casacore::Array<casacore::Float>&,
			    casacore::Bool Evaluate=true);
    virtual casacore::Double getVBPA(const VisBuffer& vb) 
    {
      // if (!rotateAperture_p) return currentCFPA;
      // else return getPA(vb);
      return getPA(vb);
    };
    casacore::MDirection::Convert makeCoordinateMachine(const VisBuffer&,
					      const casacore::MDirection::Types&,
					      const casacore::MDirection::Types&,
					      casacore::MEpoch& last);
    /*
      void makePB(const VisBuffer& vb,
      casacore::TempImage<casacore::Float>& PB,
      casacore::IPosition& shape,casacore::CoordinateSystem& coord);
      
      void makeAveragePB(const VisBuffer& vb, 
      const casacore::ImageInterface<casacore::Complex>& image,
      casacore::Int& polInUse,
      casacore::TempImage<casacore::Float>& PB,
      casacore::TempImage<casacore::Float>& avgPB);
    */
    //
    // Make a sensitivity image (sensitivityImage), given the gridded
    // weights (wtImage).  These are related to each other by a
    // Fourier transform and normalization by the sum-of-weights
    // (sumWt) and normalization by the product of the 2D FFT size
    // along each axis.  If doFFTNorm=false, normalization by the FFT
    // size is not done.  If sumWt is not provided, normalization by
    // the sum of weights is also not done.
    //
    virtual void makeSensitivityImage(casacore::Lattice<casacore::Complex>& wtImage,
				      casacore::ImageInterface<casacore::Float>& sensitivityImage,
				      const casacore::Matrix<casacore::Float>& sumWt=casacore::Matrix<casacore::Float>(),
				      const casacore::Bool& doFFTNorm=true);
    virtual casacore::Bool makeAveragePB0(const VisBuffer& vb, 
		       const casacore::ImageInterface<casacore::Complex>& image,
		       casacore::Int& polInUse,
		       casacore::TempImage<casacore::Float>& avgPB);
    /*
    void makeAveragePB(const VisBuffer& vb, 
		       const casacore::ImageInterface<casacore::Complex>& image,
		       casacore::Int& polInUse,
		       casacore::TempImage<casacore::Float>& avgPB);
    */
    void makeConjPolMap(const VisBuffer& vb, const casacore::Vector<casacore::Int> cfPolMap, casacore::Vector<casacore::Int>& conjPolMap);
    //    casacore::Vector<casacore::Int> makeConjPolMap(const VisBuffer& vb);
    void makeCFPolMap(const VisBuffer& vb, casacore::Vector<casacore::Int>& polM);
    //    void reset() {vpSJ->reset();}
    void reset() {paChangeDetector.reset();}

    void setPAIncrement(const casacore::Quantity &paIncrement);

    casacore::Vector<casacore::Int>& getPolMap() {return polMap;};
    virtual casacore::String name() const { return "PBWProjectFT";};
    virtual casacore::Bool verifyAvgPB(casacore::ImageInterface<casacore::Float>& pb, casacore::ImageInterface<casacore::Float>& sky)
    {return verifyShapes(pb.shape(),sky.shape());}
    virtual casacore::Bool verifyAvgPB(casacore::ImageInterface<casacore::Float>& pb, casacore::ImageInterface<casacore::Complex>& sky)
    {return verifyShapes(pb.shape(),sky.shape());}
    virtual casacore::Bool verifyShapes(casacore::IPosition shape0, casacore::IPosition shape1);
    casacore::Bool findSupport(casacore::Array<casacore::Complex>& func, casacore::Float& threshold, casacore::Int& origin, casacore::Int& R);
    void makeAntiAliasingOp(casacore::Vector<casacore::Complex>& val, const casacore::Int len);
    void makeAntiAliasingCorrection(casacore::Vector<casacore::Complex>& correction, 
				    const casacore::Vector<casacore::Complex>& op, 
				    const casacore::Int nx);
    void applyAntiAliasingOp(casacore::ImageInterface<casacore::Complex>& cf, 
			     casacore::Vector<casacore::IPosition>& offset,
			     casacore::Int op=0, 
			     casacore::Bool Square=false);
    void applyAntiAliasingOp(casacore::ImageInterface<casacore::Float>& cf, 
			     casacore::Vector<casacore::IPosition>& offset,
			     casacore::Int op=0, 
			     casacore::Bool Square=false);
    void correctAntiAliasing(casacore::Lattice<casacore::Complex>& cf);
    virtual void setMiscInfo(const casacore::Int qualifier){(void)qualifier;};
    virtual void ComputeResiduals(VisBuffer&/*vb*/, casacore::Bool /*useCorrected*/) {};

  protected:
    
    // Padding in FFT
    casacore::Float padding_p;
    
    casacore::Int nint(casacore::Double val){return casacore::Int(floor(val+0.5));};
    
    // Make the PB part of the convolution function
    casacore::Int makePBPolnCoords(//const casacore::ImageInterface<casacore::Complex>& image,
			 casacore::CoordinateSystem& coord, const VisBuffer& vb);
    // Locate convolution functions on the disk
    casacore::Int locateConvFunction(casacore::Int Nw, casacore::Int polInUse, const VisBuffer& vb, casacore::Float &pa);
    void cacheConvFunction(casacore::Int which, casacore::Array<casacore::Complex>& cf, casacore::CoordinateSystem& coord);
    // Find the convolution function
    void findConvFunction(const casacore::ImageInterface<casacore::Complex>& image,
			  const VisBuffer& vb);
    void makeConvFunction(const casacore::ImageInterface<casacore::Complex>& image,
			  const VisBuffer& vb, casacore::Float pa);
    
    casacore::Int nWPlanes_p;
    
    // Get the appropriate data pointer
    casacore::Array<casacore::Complex>* getDataPointer(const casacore::IPosition&, casacore::Bool);
    
    void ok();
    
    void init();
    //    casacore::Int getVisParams();
    casacore::Int getVisParams(const VisBuffer& vb);
    // Is this record on Grid? check both ends. This assumes that the
    // ends bracket the middle
    casacore::Bool recordOnGrid(const VisBuffer& vb, casacore::Int rownr) const;
    
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
    //casacore::Lattice<casacore::Complex> * arrayLattice;
    casacore::CountedPtr<casacore::Lattice<casacore::Complex> > arrayLattice;
    
    // Lattice. For non-tiled gridding, this will point to arrayLattice,
    //  whereas for tiled gridding, this points to the image
    //casacore::Lattice<casacore::Complex>* lattice;
    casacore::CountedPtr<casacore::Lattice<casacore::Complex> > lattice;
    
    casacore::Float maxAbsData;
    
    // Useful IPositions
    casacore::IPosition centerLoc, offsetLoc;
    
    // Image Scaling and offset
    casacore::Vector<casacore::Double> uvScale, uvOffset;
    
    // casacore::Array for non-tiled gridding
    casacore::Array<casacore::Complex> griddedData;
    
    // Pointing columns
    casacore::MSPointingColumns* mspc;
    
    // Antenna columns
    casacore::MSAntennaColumns* msac;
    
    casacore::DirectionCoordinate directionCoord;
    
    casacore::MDirection::Convert* pointingToImage;
    
    casacore::Vector<casacore::Double> xyPos;
    
    casacore::MDirection worldPosMeas;
    
    casacore::Int priorCacheSize;
    
    // Grid/degrid zero spacing points?
    casacore::Bool usezero_p;
    
    casacore::Array<casacore::Complex> convFunc;
    casacore::Array<casacore::Complex> convWeights;
    casacore::CoordinateSystem convFuncCS_p;
    casacore::Int convSize;
    //
    // casacore::Vector to hold the support size info. for the convolution
    // functions pointed to by the elements of convFunctions_p.  The
    // co-ordinates of this array are (W-term, Poln, PA).
    //
    casacore::Int convSampling;
    casacore::Cube<casacore::Int> convSupport, convWtSupport;
    //
    // Holder for the pointers to the convolution functions. Each
    // convolution function itself is a complex 3D array (U,V,W) per
    // PA.
    //
    casacore::PtrBlock < casacore::Array<casacore::Complex> *> convFuncCache, convWeightsCache;
    //    casacore::Array<casacore::Complex>* convFunc_p;

    //
    // The index into the conv. func. cache for the current conv. func.
    // 
    casacore::Int PAIndex;
    //
    // If true, all convolution functions are in the cache.
    //
    casacore::Bool convFuncCacheReady;

    casacore::Int wConvSize;
    
    casacore::Int lastIndex_p;
    
    casacore::Int getIndex(const casacore::MSPointingColumns& mspc, const casacore::Double& time,
		 const casacore::Double& interval);
    
    casacore::Bool getXYPos(const VisBuffer& vb, casacore::Int row);
    //    VPSkyJones *vpSJ;
    //
    // The PA averaged (and potentially antenna averaged) PB for
    // normalization
    //
    casacore::TempImage<casacore::Float> avgPB;
    //
    // No. of vis. polarization planes used in making the user defined
    // casacore::Stokes images
    //
    casacore::Int polInUse, bandID_p;
    casacore::Int maxConvSupport;
    //
    // Percentage of the peak of the PB after which the image is set
    // to zero.
    //
    casacore::Float pbLimit_p;

    //    EPJones *epJ;
    SolvableVisJones *epJ;
    casacore::Double HPBW, Diameter_p, sigma;
    casacore::Int Nant_p;
    casacore::Int doPointing;
    casacore::Bool doPBCorrection;
    casacore::Bool makingPSF;
    
    casacore::Unit Second, Radian, Day;
    casacore::Array<casacore::Float> l_offsets,m_offsets;
    casacore::Int noOfPASteps;
    casacore::Vector<casacore::Float> pbPeaks;
    casacore::Bool pbNormalized,resetPBs,rotateAperture_p;
    casacore::Vector<casacore::Float> paList;
    ConvFuncDiskCache cfCache;
    casacore::Double currentCFPA;
    ParAngleChangeDetector paChangeDetector;
    casacore::Vector<casacore::Int> cfStokes;
    casacore::Vector<casacore::Complex> Area;
    casacore::Double cfRefFreq_p;
    casacore::Bool avgPBSaved;
    casacore::Bool avgPBReady;
    casacore::Vector<casacore::Complex> antiAliasingOp,antiAliasingCorrection;
    casacore::Float lastPAUsedForWtImg;
    //    VLACalcIlluminationConvFunc vlaPB;
    //
    //----------------------------------------------------------------------
    //
    virtual void normalizeAvgPB();
    virtual void runFortranGet(casacore::Matrix<casacore::Double>& uvw,casacore::Vector<casacore::Double>& dphase,
		       casacore::Cube<casacore::Complex>& visdata,
		       casacore::IPosition& s,
		       //casacore::Cube<casacore::Complex>& gradVisAzData,
		       //casacore::Cube<casacore::Complex>& gradVisElData,
		       //casacore::IPosition& gradS,
		       casacore::Int& Conj,
		       casacore::Cube<casacore::Int>& flags,casacore::Vector<casacore::Int>& rowFlags,
		       casacore::Int& rownr,casacore::Vector<casacore::Double>& actualOffset,
		       casacore::Array<casacore::Complex>* dataPtr,
		       casacore::Int& aNx, casacore::Int& aNy, casacore::Int& npol, casacore::Int& nchan,
		       VisBuffer& vb,casacore::Int& Nant_p, casacore::Int& scanNo,
		       casacore::Double& sigma,
		       casacore::Array<casacore::Float>& raoffsets,
		       casacore::Array<casacore::Float>& decoffsets,
		       casacore::Double area,
		       casacore::Int& doGrad,casacore::Int paIndex);
    virtual void runFortranPut(casacore::Matrix<casacore::Double>& uvw,casacore::Vector<casacore::Double>& dphase,
		       const casacore::Complex& visdata_p,
		       casacore::IPosition& s,
		       //casacore::Cube<casacore::Complex>& gradVisAzData,
		       //casacore::Cube<casacore::Complex>& gradVisElData,
		       //casacore::IPosition& gradS,
		       casacore::Int& Conj,
		       casacore::Cube<casacore::Int>& flags,casacore::Vector<casacore::Int>& rowFlags,
		       const casacore::Matrix<casacore::Float>& weight,
		       casacore::Int& rownr,casacore::Vector<casacore::Double>& actualOffset,
		       casacore::Array<casacore::Complex>& dataPtr,
		       casacore::Int& aNx, casacore::Int& aNy, casacore::Int& npol, casacore::Int& nchan,
		       const VisBuffer& vb,casacore::Int& Nant_p, casacore::Int& scanNo,
		       casacore::Double& sigma,
		       casacore::Array<casacore::Float>& raoffsets,
		       casacore::Array<casacore::Float>& decoffsets,
		       casacore::Matrix<casacore::Double>& sumWeight,
		       casacore::Double& area,
		       casacore::Int& doGrad,
		       casacore::Int& doPSF,casacore::Int paIndex);
  void runFortranGetGrad(casacore::Matrix<casacore::Double>& uvw,casacore::Vector<casacore::Double>& dphase,
			 casacore::Cube<casacore::Complex>& visdata,
			 casacore::IPosition& s,
			 casacore::Cube<casacore::Complex>& gradVisAzData,
			 casacore::Cube<casacore::Complex>& gradVisElData,
			 //			 casacore::IPosition& gradS,
			 casacore::Int& Conj,
			 casacore::Cube<casacore::Int>& flags,casacore::Vector<casacore::Int>& rowFlags,
			 casacore::Int& rownr,casacore::Vector<casacore::Double>& actualOffset,
			 casacore::Array<casacore::Complex>* dataPtr,
			 casacore::Int& aNx, casacore::Int& aNy, casacore::Int& npol, casacore::Int& nchan,
			 VisBuffer& vb,casacore::Int& Nant_p, casacore::Int& scanNo,
			 casacore::Double& sigma,
			 casacore::Array<casacore::Float>& l_off,
			 casacore::Array<casacore::Float>& m_off,
			 casacore::Double area,
			 casacore::Int& doGrad,
			 casacore::Int paIndex);
  };
  
  //void saveImmage(casacore::TempImage<casacore::Complex>& convFunc, casacore::Int wConvSize);
} //# NAMESPACE CASA - END

#endif
