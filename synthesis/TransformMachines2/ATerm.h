//# ATerm.h: Definition for ATerm
//# Copyright (C) 2007
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

#ifndef SYNTHESIS_TRANSFORM2_ATERM_H
#define SYNTHESIS_TRANSFORM2_ATERM_H


#include <casa/Arrays/Vector.h>
#include <casa/System/Casarc.h>
#include <images/Images/ImageInterface.h>
#include <images/Images/PagedImage.h>
#include <images/Images/TempImage.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <casa/Containers/Block.h>
#include <synthesis/TransformMachines2/CFTerms.h>
#include <synthesis/TransformMachines2/CFStore.h>
#include <synthesis/TransformMachines2/CFStore2.h>
#define CONVSIZE (1024*2)
#define CONVWTSIZEFACTOR 1
#define OVERSAMPLING 20
#define THRESHOLD 1E-4

namespace casa{
  namespace refim{
  using namespace vi;
  // <summary>  
  //  The base class to represent the Aperture-Term of the Measurement Equation. 
  // </summary>
  
  // <use visibility=export>
  // <prerequisite>
  // </prerequisite>
  // <etymology>
  //   A-Term to account for the effects of the antenna primary beam(s).
  // </etymology>
  //
  // <synopsis> 
  // 
  //</synopsis>
  class ATerm: public CFTerms
  {
  public:
    ATerm ();
    virtual ~ATerm () {};

    virtual casacore::String name() = 0;

    virtual void makeFullJones(casacore::ImageInterface<casacore::Complex>& pbImage,
			       const VisBuffer2& vb,
			       casacore::Bool doSquint, casacore::Int& bandID, casacore::Double freqVal)=0;

    virtual void applySky(casacore::ImageInterface<casacore::Float>& outputImages,
			  const VisBuffer2& vb, 
			  const casacore::Bool doSquint=true,
			  const casacore::Int& cfKey=0,
			  const casacore::Int& muellerTerm=0,
			  const casacore::Double freqVal=-1.0) = 0;
    virtual void applySky(casacore::ImageInterface<casacore::Complex>& outputImages,
			  const VisBuffer2& vb, 
			  const casacore::Bool doSquint=true,
			  const casacore::Int& cfKey=0,
			  const casacore::Int& muellerTerm=0,
			  const casacore::Double freqVal=-1.0) = 0;
    virtual void applySky(casacore::ImageInterface<casacore::Complex>& outImages,
			  const casacore::Double& pa,
			  const casacore::Bool doSquint,
			  const casacore::Int& cfKey,
			  const casacore::Int& muellerTerm,
			  const casacore::Double freqVal)=0;

    //
    // Not sure if the following method is requried.  Leaving it in
    // the code for now with an implementation that does nothing.
    //
    // virtual void applySky(casacore::Matrix<casacore::Complex>& screen, const casacore::Int wPixel, 
    // 			  const casacore::Vector<casacore::Double>& sampling,
    // 			  const casacore::Int wConvSize, const casacore::Double wScale,
    // 			  const casacore::Int inner) 
    // {(void)screen; (void)wPixel; (void)sampling; (void)wConvSize; (void)wScale; (void)inner;};

    //
    // Returns a vector of integers that map each row in the given
    // VisBuffer to an index that is used to pick the appropriate
    // convolution function plane.  It also returns the number of
    // unique baselines in the nUnique parameter (unique baselines are
    // defined as the number of baselines each requiring a unique
    // convolution function).
    //
    // This is required for Heterogeneous antenna arrays (like ALMA)
    // and for all arrays where not all antenna aperture illuminations
    // can be treated as identical.
    //
    virtual casacore::Int makePBPolnCoords(const VisBuffer2& vb,
				 const casacore::Int& convSize,
				 const casacore::Int& convSampling,
				 const casacore::CoordinateSystem& skyCoord,
				 const casacore::Int& skyNx, const casacore::Int& skyNy,
				 casacore::CoordinateSystem& feedCoord)
    {
      //      return makePBPolnCoords(vb.corrType(), convSize, convSampling, skyCoord,
      return makePBPolnCoords(vb.correlationTypes(), convSize, convSampling, skyCoord,
			      skyNx, skyNy, feedCoord);
    };
    virtual casacore::Int makePBPolnCoords(const casacore::Vector<casacore::Int>& vbCorrTypes,
				 const casacore::Int& convSize,
				 const casacore::Int& convSampling,
				 const casacore::CoordinateSystem& skyCoord,
				 const casacore::Int& skyNx, const casacore::Int& skyNy,
				 casacore::CoordinateSystem& feedCoord);


    virtual casacore::Vector<casacore::Int> vbRow2CFKeyMap(const VisBuffer2& vb, casacore::Int& nUnique)
    {casacore::Vector<casacore::Int> tmp; tmp.resize(vb.nRows()); tmp=0; nUnique=1; return tmp;}

    virtual void getPolMap(casacore::Vector<casacore::Int>& polMap) {polMap.resize(0); polMap = polMap_p_base;};
    virtual casacore::Vector<casacore::Int> getAntTypeList() {casacore::Vector<casacore::Int> tt(1);tt(0)=0;return tt;};

    virtual void setConvSize(const casacore::Int convSize) {cachedConvSize_p=convSize;}
    virtual casacore::Int getConvSize()  {return cachedConvSize_p;};
    // {
    //   casacore::Int defaultConvSize=CONVSIZE;
    //   defaultConvSize= SynthesisUtils::getenv("CONVSIZE",CONVSIZE);
    //   // if (envStr != "")
    //   // 	{
    //   // 	  sscanf(envStr.c_str,"%d",&defaultConvSize);
    //   cerr << "ConvFuncSize set to " << defaultConvSize << endl;
    //   // 	}
    //   return defaultConvSize;
    // };

    virtual casacore::Int getOversampling() {return cachedOverSampling_p;}
    // {
    //   casacore::Int defaultOverSampling=OVERSAMPLING;
    //   char *envStr;
    //   if ((envStr = getenv("OVERSAMPLING")) != NULL)
    // 	{
    // 	  sscanf(envStr,"%d",&defaultOverSampling);
    // 	  cerr << "Oversampling set to " << defaultOverSampling << endl;
    // 	}
    //   return defaultOverSampling;
    // }
    virtual casacore::Float getConvWeightSizeFactor() {return CONVWTSIZEFACTOR;};
    virtual casacore::Float getSupportThreshold() {return THRESHOLD;};

    // virtual casacore::Vector<casacore::Int> vbRow2CFKeyMap(const VisBuffer2& vb, casacore::Int& nUnique) = 0;
    // virtual casacore::Int getConvSize() = 0;
    // virtual casacore::Int getOversampling() = 0;
    // virtual casacore::Float getConvWeightSizeFactor() = 0;
    // virtual casacore::Float getSupportThreshold() = 0;

    virtual void normalizeImage(casacore::Lattice<casacore::Complex>& skyImage,
				const casacore::Matrix<casacore::Float>& weights) 
    {
      (void)skyImage;(void)weights;
      throw(casacore::AipsError("Make ATerm::normalizeImage() pure virtual and implement in specializations"));
    };

    virtual void cacheVBInfo(const VisBuffer2& vb) = 0;
    virtual void cacheVBInfo(const casacore::String& telescopeName, const casacore::Float& diameter)=0;
    virtual casacore::Int getBandID(const casacore::Double& freq, const casacore::String& telescopeName, const casacore::String& bandName) = 0;
    virtual int getVisParams(const VisBuffer2& vb, const casacore::CoordinateSystem& skyCoord=casacore::CoordinateSystem()) = 0;
    //
    // The mapping from VisBuffer polarizations map to the Image plane
    // polarization.  The latter is determined by the user input,
    // which is passed to the FTMachine in Imager.cc
    //
    // The map is available in the FTMachine which uses this method to
    // set the map for the ATerm object.
    //
    virtual void setPolMap(const casacore::Vector<casacore::Int>& polMap) {polMap_p_base.resize(0);polMap_p_base=polMap;}
    //    virtual void rotate(const VisBuffer2& vb, CFStore2& cfs)=0;
    virtual void rotate(const VisBuffer2& vb, CFCell& cfc, const casacore::Double& rotAngleIncrement=5.0)=0;
    virtual void rotate2(const VisBuffer2& vb, CFCell& baseCFS, CFCell& cfc, const casacore::Double& rotAngleIncrement=5.0)=0;
    virtual casacore::Int mapAntIDToAntType(const casacore::Int& /*ant*/) {return 0;};
    casacore::String getTelescopeName() {return telescopeName_p;};
    virtual casacore::Bool rotationallySymmetric() {return true;};

  protected:
    casacore::LogIO& logIO() {return logIO_p;}
    casacore::LogIO logIO_p;
    casacore::Vector<casacore::Int> polMap_p_base;
    casacore::Int cachedOverSampling_p, cachedConvSize_p;

    casacore::Float Diameter_p, Nant_p, HPBW, sigma;
  };
  };
};

#endif
