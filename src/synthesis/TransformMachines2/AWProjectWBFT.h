//# AWProjectWBFT.h: Definition for AWProjectWBFT
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

#ifndef SYNTHESIS_TRANSFORM2_AWPROJECTWBFT_H
#define SYNTHESIS_TRANSFORM2_AWPROJECTWBFT_H
#define DELTAPA 1.0
#define MAGICPAVALUE -999.0


#include <synthesis/TransformMachines2/AWProjectFT.h>

namespace casa { //# NAMESPACE CASA - BEGIN
  
  class EPJones;

  namespace refim {
  class AWProjectWBFT : public AWProjectFT {

  public:
    //
    // Constructor: cachesize is the size of the cache in words
    // (e.g. a few million is a good number), tilesize is the size of
    // the tile used in gridding (cannot be less than 12, 16 works in
    // most cases).  
    // <group>
    //
    AWProjectWBFT(casacore::Int nFacets, casacore::Long cachesize,
		  casacore::CountedPtr<CFCache>& cfcache,
		  casacore::CountedPtr<ConvolutionFunction>& cf,
		  casacore::CountedPtr<VisibilityResamplerBase>& visResampler,
		  casacore::Bool applyPointingOffset=true,
		  /* casacore::Vector<casacore::Float> pointingOffsetSigDev={10,10}, */
		  vector<float> pointingOffsetSigDev={10,10},
		  casacore::Bool doPBCorr=true,
		  casacore::Int tilesize=16, 
		  casacore::Float paSteps=5.0, 
		  casacore::Float pbLimit=5e-4,
		  casacore::Bool usezero=false,
		  casacore::Bool conjBeams_p=true,
		  casacore::Bool doublePrecGrid=false);
    // </group>
    
    // Construct from a casacore::Record containing the AWProjectWBFT state
    AWProjectWBFT(const casacore::RecordInterface& stateRec);
    
    // Copy constructor
    AWProjectWBFT(const AWProjectWBFT &other):AWProjectFT(other) {operator=(other);};

    // Assignment operator
    AWProjectWBFT &operator=(const AWProjectWBFT &other);
    
    ~AWProjectWBFT() {};

    virtual casacore::String name() const {return "AWProjectWBFT";};


    casacore::Int findPointingOffsets(const VisBuffer2& vb,
				      casacore::Array<casacore::Float> &l_off,
				      casacore::Array<casacore::Float> &m_off,
				      casacore::Bool Evaluate);
    void normalizeAvgPB();
    //
    // This method is called from AWProjectFT to compute the
    // sensitivity image by accumulating in the image domain
    // (i.e. directly accumulate the Primay Beam functions).  This is
    // called from findConvFunction() so that sensitivity pattern is
    // also pre-computed along with the convolution functions.  This
    // in-turn calls the ATerm::makeAverageResponse().
    //
    // For AWProjectWBFT class of FTMachines, this just issues a log
    // message indicating that this is only setting up things for
    // accumulation of weight images in the first gridding cycle.  The
    // actual sensitivity patterns are computed by overloaded function
    // below.
    //
    virtual void makeSensitivityImage(const VisBuffer2& vb, 
				      const casacore::ImageInterface<casacore::Complex>& imageTemplate,
				      casacore::ImageInterface<casacore::Float>& sensitivityImage);
    virtual void makeSensitivityImage(const VisBuffer2& vb, 
				      const casacore::ImageInterface<casacore::DComplex>& imageTemplate,
				      casacore::ImageInterface<casacore::Float>& sensitivityImage);
    //
    // In AWProjectWBFT and its derivatives, sensitivity image is
    // computed by accumulating weight functions (images) during the
    // first gridding cycle.  AWProjectFT::makeSensitivityImage() is
    // overloaded in AWProjectWBFT and only issues a log message.
    //
    // The following method is used to Fourier transform normalize the
    // accumulated weight images.  doFFTNorm when true, the FFT
    // normalization (by pixel volume) is also done.
    //
    template <class T>
    void makeWBSensitivityImage(casacore::Lattice<T>& wtImage,
				casacore::ImageInterface<casacore::Float>& sensitivityImage,
				const casacore::Matrix<casacore::Float>& sumWt=casacore::Matrix<casacore::Float>(),
				const casacore::Bool& doFFTNorm=true);
    void makeWBSensitivitySqImage(casacore::Lattice<casacore::Complex>& wtImage,
				  casacore::ImageInterface<casacore::Complex>& sensitivitySqImage,
				  const casacore::Matrix<casacore::Float>& sumWt=casacore::Matrix<casacore::Float>(),
				  const casacore::Bool& doFFTNorm=true);

    //
    // Method used to make normalized image from gridded visibilites.
    // This calls makeSensitivityImage() to make the sensitivity image
    // and AWProjectFT::getImage() to make the image from gridded
    // visibilites.  AWProjectFT::getImage() internally calls
    // normalizeImage() which uses the sensitivty image computed by
    // makeSensitivtyImage().
    //
    virtual casacore::ImageInterface<casacore::Complex>& getImage(casacore::Matrix<casacore::Float>&,
								  casacore::Bool normalize=true);
    //
    // Method used to convert the pixel value of the PB image, passed
    // as pbPixValue, to a value used for PB-normalization.
    // Typically, this will depend on the units of the "PB image"
    // constructed by the makeSensitivtyImage() methods. pbLimit is
    // the fractional pb-gain below which imaging is not required
    // (this value is typically the user-defined parameter in the
    // private member variable pbLimit_p).
    //
    inline virtual casacore::Float pbFunc(const casacore::Float& /*pbPixValue*/, const casacore::Float& /*pbLimit*/) 
    {return  1.0;};
    //   {casacore::Float tt=(pbPixValue);return  (abs(tt) >= pbLimit)?tt:1.0;};
 

   //    {casacore::Float tt=sqrt(pbPixValue);return  (abs(tt) >= pbLimit)?tt:1.0;};

    virtual void finalizeToSky();
    virtual void initializeToSky(casacore::ImageInterface<casacore::Complex>& image,
				 casacore::Matrix<casacore::Float>& weight,
				 const VisBuffer2& vb);

    void setObservatoryLocation(const casacore::MPosition& mLocation) {mLocation_p=mLocation;};

    virtual casacore::Bool verifyShapes(casacore::IPosition shape0, casacore::IPosition shape1)
    {(void)shape0; (void)shape1;return false;};

    //
    // Returns true if accumulation during gridding to compute the
    // average PB must be done.
    //
    virtual casacore::Bool computeAvgPB(const casacore::Double& /*actualPA*/, const casacore::Double& /*lastPAUsedForWtImg*/) 
    {return (avgPBReady_p==false);};

    // virtual void setMiscInfo(const casacore::Int qualifier)
    // {sensitivityPatternQualifier_p=qualifier;}
    //    {qualifier_p = qualifier;taylorQualifier_p = "_MFS_"+casacore::String::toString(qualifier_p)+"_";};

    //  virtual void ComputeResiduals(VisBuffer2&vb, casacore::Bool useCorrected) {};
    virtual void setCFCache(casacore::CountedPtr<CFCache>& cfc, const casacore::Bool resetCFC=true);
    void gridImgWeights(const VisBuffer2& vb);
    

  protected:
    template <class T>
    void ftWeightImage(casacore::Lattice<T>& wtImage, 
		       const casacore::Matrix<casacore::Float>& sumWt,
		       const casacore::Bool& doFFTNorm);

    virtual void resampleDataToGrid(casacore::Array<casacore::Complex>& griddedData,VBStore& vbs, 
				    const VisBuffer2& vb, casacore::Bool& dopsf);
    virtual void resampleDataToGrid(casacore::Array<casacore::DComplex>& griddedData,VBStore& vbs, 
				    const VisBuffer2& vb, casacore::Bool& dopsf);
    //    virtual void resampleGridToData(VBStore& vbs, const VisBuffer2& vb);
    template <class T>
    void resampleCFToGrid(casacore::Array<T>& wtsGrid, 
			  VBStore& vbs, const VisBuffer2& vb);

    casacore::Bool resetPBs_p, wtImageFTDone_p;

  private:
    casacore::String tt_pp;
    casacore::Vector<casacore::Int> fieldIds_p;
    casacore::TempImage<casacore::Complex> griddedWeights;
    casacore::TempImage<casacore::DComplex> griddedWeights_D;
    CFStore rotatedCFWts_p;
    casacore::CountedPtr<VisibilityResamplerBase> visResamplerWt_p;
    casacore::Bool oneTimeMessage_p;
  };
} //# NAMESPACE CASA - END
};
#endif
