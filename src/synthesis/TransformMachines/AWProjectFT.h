//# AWProjectFT.h: Definition for AWProjectFT
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

#ifndef SYNTHESIS_AWPROJECTFT_H
#define SYNTHESIS_AWPROJECTFT_H

#include <synthesis/TransformMachines/VLACalcIlluminationConvFunc.h>
#include <synthesis/TransformMachines/VLAIlluminationConvFunc.h>
#include <synthesis/TransformMachines/AWVisResampler.h>
//#include <synthesis/MeasurementComponents/ConvolutionFunction.h>
#include <synthesis/TransformMachines/EVLAConvFunc.h>
#include <synthesis/MeasurementComponents/SolvableVisCal.h>
#include <synthesis/TransformMachines/VPSkyJones.h>
#include <synthesis/TransformMachines/FTMachine.h>
#include <synthesis/TransformMachines/PolOuterProduct.h>
//#include <synthesis/MeasurementComponents/CFCache.h>
#include <synthesis/TransformMachines/Utils.h>

#include <casacore/scimath/Mathematics/FFTServer.h>
#include <msvis/MSVis/VisBuffer.h>

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
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>

#include <synthesis/TransformMachines/AWConvFunc.h>
#include <synthesis/TransformMachines/AWConvFuncEPJones.h>
#include <synthesis/TransformMachines/ATerm.h>

#include <casacore/casa/OS/Timer.h>

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
  // WProjectFT, AWProjectFT does Grid-based Fourier transforms but
  // also includes the effects of primary beam and antenna pointing
  // offsets. 
  // </etymology>
  //
  // <synopsis> 
  //
  // The <linkto class=SkyEquation>SkyEquation</linkto> needs to be
  // able to perform Fourier transforms on visibility
  // data. AWProjectFT allows efficient handling of direction
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
  class AWProjectFT : public FTMachine {
  public:
    static ATerm* createTelescopeATerm(const casacore::String& telescopeName, 
				       const casacore::Bool& isATermOn);
    static casacore::CountedPtr<ConvolutionFunction> makeCFObject(const casacore::String& telescopeName,
							const casacore::Bool aTermOn,
							const casacore::Bool psTermOn,
							const casacore::Bool wTermOn,
							const casacore::Bool mTermOn,
							const casacore::Bool wBAWP,
							const casacore::Bool conjBeams);
    AWProjectFT();
    
    // Constructor: cachesize is the size of the cache in words
    // (e.g. a few million is a good number), tilesize is the
    // size of the tile used in gridding (cannot be less than
    // 12, 16 works in most cases). 
    // <group>
    AWProjectFT(casacore::Int nFacets, casacore::Long cachesize, 
		casacore::CountedPtr<CFCache>& cfcache,
		casacore::CountedPtr<ConvolutionFunction>& cf,
		casacore::CountedPtr<VisibilityResamplerBase>& visResampler,
		casacore::Bool applyPointingOffset=true,
		casacore::Bool doPBCorr=true,
		casacore::Int tilesize=16, 
		casacore::Float pbLimit=5e-4,
		casacore::Bool usezero=false,
		casacore::Bool conjBeams_p=true,
		casacore::Bool doublePrecGrid=false,
		PolOuterProduct::MuellerType muellerType=PolOuterProduct::FULL);
    // </group>
    
    // Construct from a casacore::Record containing the AWProjectFT state
    AWProjectFT(const casacore::RecordInterface& stateRec);
    
    // Copy constructor
    AWProjectFT(const AWProjectFT &other);
    
    // Assignment operator
    AWProjectFT &operator=(const AWProjectFT &other);
    
    ~AWProjectFT();
    
    //   void setEPJones(EPJones* ep_j) {epJ = ep_j;}
    void setEPJones(SolvableVisJones* ep_j) {epJ_p = ep_j;}
    
    virtual void setDOPBCorrection(casacore::Bool doit=true) {doPBCorrection=doit;};
    virtual casacore::Bool getDOPBCorrection() {return doPBCorrection;};
    virtual void setConjBeams(casacore::Bool useit=true) {conjBeams_p=useit;};
    virtual casacore::Bool getConjBeams() {return conjBeams_p;};

    virtual casacore::Float getPBLimit() {return pbLimit_p;};
    // Initialize transform to Visibility plane using the image
    // as a template. The image is loaded and Fourier transformed.
    
    void setObservatoryLocation(const casacore::MPosition& mLocation) {mLocation_p=mLocation;};
    
    // Vectorized version of initializeToVis.  Required since
    // MultiTermFTM needs vectorized version. And this is implemented
    // in FTMachine (the baseclass).  And one relies on static_cast in
    // Imager::createFTMachine() (or is it in CSE?) to cast the
    // pointer to specific types such that this methods gets called
    // when the FTMachine pointer is of type AWProjectFT.
    virtual void initializeToVis(casacore::Block<casacore::CountedPtr<casacore::ImageInterface<casacore::Complex> > > & compImageVec,
				 casacore::PtrBlock<casacore::SubImage<casacore::Float> *> & modelImageVec, 
				 casacore::PtrBlock<casacore::SubImage<casacore::Float> *>& weightImageVec, 
				 casacore::PtrBlock<casacore::SubImage<casacore::Float> *>& fluxScaleVec, 
				 casacore::Block<casacore::Matrix<casacore::Float> >& weightsVec,
				 const VisBuffer& vb);

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
      //    throw(casacore::AipsError("AWProjectFT::put is not implemented"));
    }
    void put(const VisBuffer& vb, casacore::Int row=-1, casacore::Bool dopsf=false,
	     FTMachine::Type type=FTMachine::OBSERVED);
    
    // Make the entire image using a ROVisIter
    virtual void makeImage(FTMachine::Type,
			   ROVisibilityIterator&,
			   casacore::ImageInterface<casacore::Complex>&,
			   casacore::Matrix<casacore::Float>&) {};

    // Make the entire image
    void makeImage(FTMachine::Type type,
		   VisSet& vs,
		   casacore::ImageInterface<casacore::Complex>& image,
		   casacore::Matrix<casacore::Float>& weight);
    
    // Get the final image: do the Fourier transform and
    // grid-correct, then optionally normalize by the summed weights
    virtual casacore::ImageInterface<casacore::Complex>& getImage(casacore::Matrix<casacore::Float>&, casacore::Bool normalize=true);

    // Get the final weights image
    void getWeightImage(casacore::ImageInterface<casacore::Float>&, casacore::Matrix<casacore::Float>&);
    
    // Save and restore the AWProjectFT to and from a record
    casacore::Bool toRecord(casacore::RecordInterface& outRec,  casacore::Bool withImage=false);
    casacore::Bool fromRecord(const casacore::RecordInterface& inRec);
    
    // Can this FTMachine be represented by Fourier convolutions?
    casacore::Bool isFourier() {return true;}
    
    //  casacore::Bool changed(const VisBuffer& vb) {return vpSJ->changed(vb,1);};
    //    casacore::Bool changed(const VisBuffer& vb) {return false;}
    
    virtual casacore::Int findPointingOffsets(const VisBuffer&, casacore::Array<casacore::Float>&, casacore::Array<casacore::Float>&,
				    casacore::Bool Evaluate=true);
    virtual casacore::Int findPointingOffsets(const VisBuffer&, casacore::Cube<casacore::Float>&,
				    casacore::Array<casacore::Float>&, casacore::Array<casacore::Float>&,
				    casacore::Bool Evaluate=true);
    virtual casacore::Double getVBPA(const VisBuffer& vb) 
    {
      // if (!rotateApertureOTP_p) return currentCFPA;
      // else return getPA(vb);
      return getPA(vb);
    };
    casacore::MDirection::Convert makeCoordinateMachine(const VisBuffer&,
					      const casacore::MDirection::Types&,
					      const casacore::MDirection::Types&,
					      casacore::MEpoch& last);
    //
    // Make a sensitivity image (sensitivityImage), given the gridded
    // weights (wtImage).  These are related to each other by a
    // Fourier transform and normalization by the sum-of-weights
    // (sumWt) and normalization by the product of the 2D FFT size
    // along each axis.  If doFFTNorm=false, normalization by the FFT
    // size is not done.  If sumWt is not provided, normalization by
    // the sum of weights is also not done.
    //
    virtual void makeSensitivityImage(casacore::Lattice<casacore::Complex>&,// wtImage,
				      casacore::ImageInterface<casacore::Float>&,// sensitivityImage,
				      const casacore::Matrix<casacore::Float>&,// sumWt=casacore::Matrix<casacore::Float>(),
				      const casacore::Bool& ) {};
    virtual void makeSensitivityImage(const VisBuffer& vb, const casacore::ImageInterface<casacore::Complex>& imageTemplate,
				      casacore::ImageInterface<casacore::Float>& sensitivityImage);

    //
    // Given the sky image (Fourier transform of the visibilities),
    // sum of weights and the sensitivity image, this method replaces
    // the skyImage with the normalized image of the sky.
    //
    // These are now implement in the FTMachine base class.  
    //
    // These can't be left here since now the design depends on
    // getting the correct version called due to static type casting
    // of FTM pointer and not on inheretance and and specialization
    // via overloading.

    // virtual void normalizeImage(casacore::Lattice<casacore::Complex>& skyImage,
    // 				const casacore::Matrix<casacore::Double>& sumOfWts,
    // 				casacore::Lattice<casacore::Float>& sensitivityImage,
    // 				casacore::Bool fftNorm=true);
    // virtual void normalizeImage(casacore::Lattice<casacore::Complex>& skyImage,
    // 				const casacore::Matrix<casacore::Double>& sumOfWts,
    // 				casacore::Lattice<casacore::Float>& sensitivityImage,
    // 				casacore::Lattice<casacore::Complex>& sensitivitySqImage,
    // 				casacore::Bool fftNorm=true);
    
    virtual casacore::ImageInterface<casacore::Float>& getSensitivityImage() {return *avgPB_p;}
    virtual casacore::Matrix<casacore::Double>& getSumOfWeights() {return sumWeight;};
    virtual casacore::Matrix<casacore::Double>& getSumOfCFWeights() {return sumCFWeight;};

    void makeConjPolMap(const VisBuffer& vb, const casacore::Vector<casacore::Int> cfPolMap, casacore::Vector<casacore::Int>& conjPolMap);
    //    casacore::Vector<casacore::Int> makeConjPolMap(const VisBuffer& vb);
    void makeCFPolMap(const VisBuffer& vb, const casacore::Vector<casacore::Int>& cfstokes, casacore::Vector<casacore::Int>& polM);
    //    void reset() {vpSJ->reset();}
    void reset() {paChangeDetector.reset();}

    void setPAIncrement(const casacore::Quantity &computePAIncr, const casacore::Quantity &rotateOTFPAIncr);

    casacore::Vector<casacore::Int>& getPolMap() {return polMap;};
    virtual casacore::String name() const { return "AWProjectFT";};
    virtual casacore::Bool verifyAvgPB(casacore::ImageInterface<casacore::Float>& pb, casacore::ImageInterface<casacore::Float>& sky)
    {return verifyShapes(pb.shape(),sky.shape());}

    virtual casacore::Bool verifyAvgPB(casacore::ImageInterface<casacore::Float>& pb, casacore::ImageInterface<casacore::Complex>& sky)
    {return verifyShapes(pb.shape(),sky.shape());}

    virtual casacore::Bool verifyShapes(casacore::IPosition shape0, casacore::IPosition shape1);

    inline virtual casacore::Float pbFunc(const casacore::Float& a, const casacore::Float& limit) 
    {casacore::Float tt=sqrt(a);return  (abs(tt) >= limit)?tt:1.0;};
    // {if (abs(a) >= limit) return (a);else return 1.0;};

    inline virtual casacore::Complex pbFunc(const casacore::Complex& a, const casacore::Float& limit) 
    {if (abs(a)>=limit) return (a); else return casacore::Complex(1.0,0.0);};

    virtual void setMiscInfo(const casacore::Int qualifier)
    {
      sensitivityPatternQualifier_p=qualifier;
      sensitivityPatternQualifierStr_p = ".tt"+casacore::String::toString(sensitivityPatternQualifier_p);
    }
    virtual void ComputeResiduals(VisBuffer&vb, casacore::Bool useCorrected);
    void makeWBCFWt(CFStore2& cfs,const casacore::Double imRefFreq);

    CFBStruct cfbst_pub;
    // Image Scaling and offset
    casacore::Vector<casacore::Double> uvScale, uvOffset;
  protected:
    
    casacore::Int nint(casacore::Double val) {return casacore::Int(floor(val+0.5));};
    // Locate convolution functions on the disk
    //    casacore::Int locateConvFunction(const casacore::Int Nw, const casacore::Float pa);
    //    void cacheConvFunction(casacore::Int which, casacore::Array<casacore::Complex>& cf, casacore::CoordinateSystem& coord);
    // Find the convolution function
    void findConvFunction(const casacore::ImageInterface<casacore::Complex>& image,
			  const VisBuffer& vb);
    
    // Get the appropriate data pointer
    casacore::Array<casacore::Complex>* getDataPointer(const casacore::IPosition&, casacore::Bool);
    
    void ok();
    
    void init();
    //    virtual void initPolInfo(const VisBuffer& vb);
    // Is this record on Grid? check both ends. This assumes that the
    // ends bracket the middle
    casacore::Bool recordOnGrid(const VisBuffer& vb, casacore::Int rownr) const;
    
    // Padding in FFT
    casacore::Float padding_p;
    
    casacore::Int nWPlanes_p;
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
    
    // // Image Scaling and offset
    // casacore::Vector<casacore::Double> uvScale, uvOffset;
    
   
    //    casacore::DirectionCoordinate directionCoord;
    casacore::MDirection::Convert* pointingToImage;
    
    // Grid/degrid zero spacing points?
    casacore::Bool usezero_p;
    
    //    casacore::CountedPtr<ConvolutionFunction> telescopeConvFunc_p;
    //    CFStore cfs_p, cfwts_p;
    // casacore::Array<casacore::Complex> convFunc_p, convWeights_p;
    //
    // casacore::Vector to hold the support size info. for the convolution
    // functions pointed to by the elements of convFunctions_p.  The
    // co-ordinates of this array are (W-term, Poln, PA).
    //
    casacore::Int // convSize,
      convSampling, wConvSize, lastIndex_p;
    
    //
    // The average PB for sky image normalization
    //
    casacore::CountedPtr<casacore::ImageInterface<casacore::Float> > avgPB_p;
    casacore::CountedPtr<casacore::ImageInterface<casacore::Complex> > avgPBSq_p;
    //
    // No. of vis. polarization planes used in making the user defined
    // casacore::Stokes images
    //
    casacore::Int maxConvSupport;
    //
    // Percentage of the peak of the PB after which the image is set
    // to zero.
    //
    casacore::CountedPtr<SolvableVisJones> epJ_p;
    casacore::Double sigma;
    casacore::Int Nant_p, doPointing;
    casacore::Bool doPBCorrection, makingPSF, conjBeams_p;
    
    //    casacore::CountedPtr<CFCache> cfCache_p;
    ParAngleChangeDetector paChangeDetector;
    casacore::Double rotateOTFPAIncr_p, computePAIncr_p;

    casacore::Unit Second, Radian, Day;
    casacore::Array<casacore::Float> l_offsets,m_offsets;
    casacore::Vector<casacore::Float> pbPeaks, paList;

    casacore::Double currentCFPA, cfRefFreq_p, imRefFreq_p;
    casacore::Float lastPAUsedForWtImg;
    casacore::Bool pbNormalized_p;
    casacore::Vector<casacore::Bool> paNdxProcessed_p;
    //
    //----------------------------------------------------------------------
    //
    virtual void normalizeAvgPB();
    virtual void normalizeAvgPB(casacore::ImageInterface<casacore::Complex>& inImage,
				casacore::ImageInterface<casacore::Float>& outImage);
    virtual void resampleDataToGrid(casacore::Array<casacore::Complex>& griddedData, VBStore& vbs, 
				    const VisBuffer& vb, casacore::Bool& dopsf);
    virtual void resampleDataToGrid(casacore::Array<casacore::DComplex>& griddedData, VBStore& vbs, 
				    const VisBuffer& vb, casacore::Bool& dopsf);
    virtual void resampleGridToData(VBStore& vbs, casacore::Array<casacore::Complex>& griddedData,
				    const VisBuffer& vb);

    virtual void makeThGridCoords(VBStore& vbs, const casacore::Vector<casacore::Int>& gridShape);
    virtual void setupVBStore(VBStore& vbs,
			      const VisBuffer& vb,
			      const casacore::Matrix<casacore::Float>& imagingweight,
			      const casacore::Cube<casacore::Complex>& visData,
			      const casacore::Matrix<casacore::Double>& uvw,
			      const casacore::Cube<casacore::Int>& flagCube,
			      const casacore::Vector<casacore::Double>& dphase,
			      const casacore::Bool& doPSF,
			      const casacore::Vector<casacore::Int> &gridShape);

    //    AWVisResampler visResampler_p;
    casacore::CountedPtr<VisibilityResamplerBase> visResampler_p;
    casacore::Int sensitivityPatternQualifier_p;
    casacore::String sensitivityPatternQualifierStr_p;
    CFStore rotatedConvFunc_p;
    //casacore::CountedPtr<CFStore2> cfs2_p, cfwts2_p;
    casacore::Vector<casacore::Int> ConjCFMap_p, CFMap_p;

    casacore::Timer timer_p;
    casacore::Double runTime1_p;

    PolOuterProduct::MuellerType muellerType_p;

    Int previousSPWID_p;

#include "AWProjectFT.FORTRANSTUFF.INC"
  };
} //# NAMESPACE CASA - END

#endif
