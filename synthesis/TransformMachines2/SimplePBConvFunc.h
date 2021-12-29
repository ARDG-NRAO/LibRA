//# SimplePBConvFunc.h: Definition for PixelatedConvFunc
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
#ifndef SYNTHESIS_TRANSFORM2_SIMPLEPBCONVFUNC_H
#define SYNTHESIS_TRANSFORM2_SIMPLEPBCONVFUNC_H

#include <casa/Arrays/Vector.h>
#include <synthesis/TransformMachines/PBMathInterface.h>
#include <casa/Containers/Block.h>
#include <casa/Utilities/CountedPtr.h>
#include <msvis/MSVis/VisBufferUtil.h>
#include <synthesis/Utilities/FFT2D.h>
#include <casacore/casa/Arrays/ArrayFwd.h>

#include <wcslib/wcsconfig.h>  /** HAVE_SINCOS **/

#if HAVE_SINCOS
#define SINCOS(a,s,c) sincos(a,&s,&c)
#else
#define SINCOS(a,s,c)                   \
     s = sin(a);                        \
     c = cos(a)
#endif


namespace casacore{

  template<class T> class ImageInterface;
  class CoordinateSystem;
  class DirectionCoordinate;
}

namespace casa{

  // <summary>  A class to support FTMachines get their convolution casacore::Function </summary>
  
  // <use visibility=export>
  // <prerequisite>
  //   <li> <linkto class=VisBuffer>VisBuffer</linkto> module
// </prerequisite>
  // <etymology>
  // "Simple" for cases that does not need parallactic angle and or frequency 
  // dependence ...hence one convolution function per pointing
  // "PB" for primary beam based convolution function
  // </etymology>
  //
  // <synopsis> 
  // FTMachines like WProjection and MosaicFT need convolution functions to 
  // deal with directional dependent issues...
  // this class and related ones provide and cache  such functions for re-use 
  //</synopsis>
  //Forward declarations
  
 
  namespace vi{class VisBuffer2;}


namespace refim{ //namespace for imaging refactor
  class SkyJones;
    

  class SimplePBConvFunc 

    {
    public:
      SimplePBConvFunc();
      SimplePBConvFunc(const PBMathInterface::PBClass typeToUse);
      SimplePBConvFunc(const casacore::RecordInterface& rec, casacore::Bool calcFlux_needed);
      virtual ~SimplePBConvFunc();
      // Inputs are the image, visbuffer, convSampling and skyjones
      // findconv return a cached convvolution function appropriate for this 
      // visbuffer and skyjones ...this one should be superseded 
      // by the one below and call setSkyJones when necessary
      virtual void findConvFunction(const casacore::ImageInterface<casacore::Complex>& , 
				    const vi::VisBuffer2& ,const casacore::Int& ,
			    SkyJones& ,
			    casacore::Matrix<casacore::Complex>& , 
			    casacore::Matrix<casacore::Complex>& , casacore::Int& ,
				    casacore::Int& ){};
      
      ////Returns the convfunctions in the Cubes...the casacore::Matrix rowChanMap maps 
      // the vb.row and channel 
      //to the plane of the convfunc appropriate 
      
      virtual void findConvFunction(const casacore::ImageInterface<casacore::Complex>& iimage, 
				    const vi::VisBuffer2& vb,
				    const casacore::Int& convSampling,
				    const casacore::Vector<casacore::Double>& visFreq,
				    casacore::Array<casacore::Complex>& convFunc,
				    casacore::Array<casacore::Complex>& weightConvFunc,
				    casacore::Vector<casacore::Int>& convsize,
				    casacore::Vector<casacore::Int>& convSupport,
				    casacore::Vector<casacore::Int>& polMap, casacore::Vector<casacore::Int>& chanMap, casacore::Vector<casacore::Int>& rowMap, const casacore::Bool getConjFreqConvFunc=false, const casacore::MVDirection& extraShift=casacore::MVDirection(0.0), const casacore::Bool useExtraShift=casacore::False);
      virtual casacore::ImageInterface<casacore::Float>&  getFluxScaleImage();
      // slice fluxscale image by npol 
      virtual void sliceFluxScale(casacore::Int npol);
      //This is a function to just store the final weight image
      //as FT machines will share this object ...they can get share this too
      virtual void setWeightImage(casacore::CountedPtr<casacore::TempImage<casacore::Float> >& wgtimage);

      virtual void setSkyJones(SkyJones* sj);
      virtual void setVBUtil(casacore::CountedPtr<VisBufferUtil> vbutil) {vbutil_p=vbutil;};
      virtual  casacore::CountedPtr<VisBufferUtil> getVBUtil() {return vbutil_p;};
      casacore::Bool findSupport(casacore::Array<casacore::Complex>& /*func*/, casacore::Float& /*threshold*/,casacore::Int& /*origin*/, casacore::Int& /*R*/) 
      {throw(casacore::AipsError("SimplePBConvFunc::findSupport() not implemented"));};
      virtual casacore::Bool makeAverageResponse(const vi::VisBuffer2& /*vb*/, 
				       const casacore::ImageInterface<casacore::Complex>& /*image*/,
				       casacore::ImageInterface<casacore::Float>& /*theavgPB*/,
				       casacore::Bool /*reset=true*/)
      {throw(casacore::AipsError("SimplePBConvFunc::makeAverageRes() called"));};

      //Serialization
     virtual  casacore::Bool toRecord(casacore::RecordInterface& outRec);
      //From record 
      //set calcfluxneeded to true if flux scale need to be computed
      virtual casacore::Bool fromRecord(casacore::String& err, const casacore::RecordInterface& rec, casacore::Bool calcFluxneeded=false);
      //give possibility to erase history
      virtual void reset();
      virtual casacore::String name() {return casacore::String("SimplePBConvFunc");};
      void setUsePointing(casacore::Bool usepointing){usePointingTable_p=usepointing;};
    protected:
      SkyJones* sj_p;
      casacore::TempImage<casacore::Float> fluxScale_p;
      casacore::Int nx_p; 
      casacore::Int ny_p;
      casacore::Int nchan_p;
      casacore::Int npol_p;
      casacore::CoordinateSystem csys_p;
      casacore::DirectionCoordinate dc_p;
      casacore::MDirection::Convert pointToPix_p;
      casacore::MeasFrame pointFrame_p;
      casacore::MEpoch::Types timeMType_p;
      casacore::Unit timeUnit_p;
      casacore::Int directionIndex_p;
      casacore::MDirection direction1_p;
      casacore::MDirection direction2_p;
      casacore::Vector<casacore::Double> thePix_p;
      casacore::Bool filledFluxScale_p;
      casacore::Vector<casacore::Bool> doneMainConv_p;
      casacore::Bool calcFluxScale_p;
      std::map<casacore::String, casacore::Int> vbConvIndex_p;
      virtual casacore::Int convIndex(const vi::VisBuffer2& vb, const casacore::uInt nchan);
      std::map<casacore::String, casacore::Int> ant1PointVal_p;
      casacore::Vector<casacore::MDirection> ant1PointingCache_p;
      const casacore::MDirection& pointingDirAnt1(const vi::VisBuffer2& vb);
      virtual void storeImageParams(const casacore::ImageInterface<casacore::Complex>& iimage, const vi::VisBuffer2& vb);
      virtual void findUsefulChannels(casacore::Vector<casacore::Int>& chanMap, casacore::Vector<casacore::Double>& chanFreqs,  const vi::VisBuffer2& vb, const casacore::Vector<casacore::Double>& visFreq);
      //return the direction pixel corresponding to a direction
      virtual void toPix(const vi::VisBuffer2& vb, const casacore::MVDirection& extraShift=casacore::MVDirection(0.0), const casacore::Bool useExtraShift=casacore::False);
      FFT2D ft_p;
      casacore::CountedPtr<casacore::TempImage<casacore::Float> > convWeightImage_p;
      casacore::String bandName_p;
      casacore::CountedPtr<VisBufferUtil> vbutil_p;
      casacore::Bool usePointingTable_p;
    private:
      casacore::Bool checkPBOfField(const vi::VisBuffer2& vb);
      void addPBToFlux(const vi::VisBuffer2& vb);
      std::map <casacore::String, casacore::Int> convFunctionMap_p;
      casacore::Int actualConvIndex_p;
      PBMathInterface::PBClass pbClass_p;

      casacore::Matrix<casacore::Complex> convFunc_p;
      casacore::Matrix<casacore::Complex> weightConvFunc_p;
      casacore::Matrix<casacore::Complex> convSave_p;
      casacore::Matrix<casacore::Complex> weightSave_p;
      casacore::Int convSize_p; 
      casacore::Int convSupport_p;
      //These are Arrays of 5 dimension (x, y, npol, nchan, nrow)
      //Thus every baseline may have its own.
      casacore::Block <casacore::CountedPtr<casacore::Array<casacore::Complex> > > convFunctions_p;
      casacore::Block <casacore::CountedPtr<casacore::Array<casacore::Complex> > > convWeights_p;
      casacore::Block<casacore::CountedPtr<casacore::Vector<casacore::Int> > > convSizes_p;
      casacore::Block <casacore::CountedPtr<casacore::Vector<casacore::Int> > > convSupportBlock_p;
      casacore::Matrix<casacore::Bool> pointingPix_p;
      
    };
  }; //end of refim namespace
};// end of namespace
#endif
