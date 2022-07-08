//# ConvolutionFunction.h: Definition for ConvolutionFunction
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

#ifndef SYNTHESIS_TRANSFORM2_CONVOLUTIONFUNCTION_H
#define SYNTHESIS_TRANSFORM2_CONVOLUTIONFUNCTION_H

#include <synthesis/TransformMachines2/CFTerms.h>
#include <synthesis/TransformMachines2/CFStore.h>
#include <synthesis/TransformMachines2/CFStore2.h>
#include <synthesis/TransformMachines2/VB2CFBMap.h>
#include <synthesis/TransformMachines2/PolOuterProduct.h>
#include <synthesis/TransformMachines2/PointingOffsets.h>
#include <synthesis/TransformMachines2/Utils.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/TempImage.h>
#include <casacore/casa/Logging/LogOrigin.h>
#include <casacore/casa/Logging/LogSink.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Arrays/Vector.h>
#define CF_TYPE casacore::Double

namespace casa{
  namespace refim{
  // <summary>  
  //  The base class to compute convolution functions for convolutional gridding. 
  // </summary>
  
  // <use visibility=export>
  // <prerequisite>
  // </prerequisite>
  // <etymology>
  //   Class to encapsulate the convolution function for convolutional gridding.
  // </etymology>
  //
  // <synopsis> Standard method of re-sampling data to or from a
  //
  //  regular grid is done by convolutional gridding.  This requires a
  //  convolution function which a finte support size and well behaved
  //  function in the Fourier domain.  For standard gridding, the
  //  Prolate Spheroidal function are used.  Convolution functions
  //  used in casacore::Projection algorithms (like W-casacore::Projection, A-casacore::Projection,
  //  etc. and their combinations) each require potentially different
  //  mechanisms to compute.  These are implemented in separate
  //  classes in the Synthesis module.  Since these are used in common
  //  framework for gridding and de-gridding, these are all derived
  //  from a common base class.  ConvolutionFunction (this class) is
  //  that base class.
  //
  //  Most of the methods in this base class are pure virtual.  I.e.,
  //  only surviving offsprings (derived classes) of this class will
  //  be those that will have the wisdom that they methods represent.
  //
  // </synopsis>

  class ConvolutionFunction
  {
  public:
    ConvolutionFunction():
      nDim(2),logIO_p(), spwChanSelFlag_p(), spwFreqSelection_p(),
      computeCFAngleRad_p(360.0*M_PI/180.0), rotateCFOTFAngleRad_p(0.1)
    {};
    ConvolutionFunction(casacore::Int dim): 
      nDim(dim),logIO_p(), spwChanSelFlag_p(), spwFreqSelection_p(),
      computeCFAngleRad_p(360.0*M_PI/180.0), rotateCFOTFAngleRad_p(0.1)
    {nDim=dim;};
    virtual ~ConvolutionFunction();
    
    // Set the dimention of the convolution function.
    virtual void setDimension(casacore::Int n){nDim = n;};

    // Given the pixel co-ordinates and an offset values, this returns
    // the value of the convolution function.  This is however not
    // used anywhere yet (and is therefore also not a pure virtual
    // function).
    virtual CF_TYPE getValue(casacore::Vector<CF_TYPE>& , casacore::Vector<CF_TYPE>& ) {return 0.0;};

    // A support function which, for now, returns and integer ID
    // corresponding to the on-sky frequency of the supplied VisBuffer.
    virtual int getVisParams(const VisBuffer2& vb,const casacore::CoordinateSystem& skyCoord=casacore::CoordinateSystem())=0;

    // This method computes the convolution function and the
    // convolution function used for gridding the weights (typically
    // these are the same) and returns them in the cfs and cfwts
    // parameters.  The required information about the image and
    // visibility parameters is dervided from the given image and
    // VisBuffer objects.  wConvSize is the number of w-term planes
    // and pa is the Parallactic Angle in radians for which the
    // convolution function(s) are computed.
    virtual void makeConvFunction(const casacore::ImageInterface<casacore::Complex>& image,
				  const VisBuffer2& vb,
				  const casacore::Int wConvSize,
				  const casacore::CountedPtr<PolOuterProduct>& pop,
				  const casacore::Float pa, 
				  const casacore::Float dpa, 
				  const casacore::Vector<casacore::Double>& uvScale, const casacore::Vector<casacore::Double>& uvOffset,
				  const casacore::Matrix<casacore::Double>& vbFreqSelection,
				  CFStore2& cfs,
				  CFStore2& cfwts,
				  casacore::Bool fillCF=true) = 0;
    // This method computes the average response function.  This is
    // typically image-plane equivalent of the convolution functions,
    // averaged over various axis.  The precise averaging will be
    // implementation dependent in the derived classes.
    virtual casacore::Bool makeAverageResponse(const VisBuffer2& vb, 
				     const casacore::ImageInterface<casacore::Complex>& image,
				     casacore::ImageInterface<casacore::Float>& theavgPB,
				     casacore::Bool reset=true) = 0;
    virtual casacore::Bool makeAverageResponse(const VisBuffer2& vb, 
				     const casacore::ImageInterface<casacore::Complex>& image,
				     casacore::ImageInterface<casacore::Complex>& theavgPB,
				     casacore::Bool reset=true) = 0;

    //
    virtual void setPolMap(const casacore::Vector<casacore::Int>& polMap) = 0;
    virtual void setSpwSelection(const casacore::Cube<casacore::Int>& spwChanSelFlag) {spwChanSelFlag_p.assign(spwChanSelFlag);}
    virtual void setSpwFreqSelection(const casacore::Matrix<casacore::Double>& spwFreqSel) {spwFreqSelection_p.assign(spwFreqSel);}
    virtual void setRotateCF(const casacore::Double& computeCFAngleRad, const casacore::Double& rotateOTF) 
    {computeCFAngleRad_p=computeCFAngleRad; rotateCFOTFAngleRad_p = rotateOTF;};

    //    virtual void setFeedStokes(const casacore::Vector<casacore::Int>& feedStokes) = 0;
    virtual casacore::Bool findSupport(casacore::Array<casacore::Complex>& func, casacore::Float& threshold,casacore::Int& origin, casacore::Int& R)=0;
    virtual casacore::Vector<casacore::Double> findPointingOffset(const casacore::ImageInterface<casacore::Complex>& image,
					      const VisBuffer2& vb) = 0;

    // virtual void setParams(const casacore::Vector<casacore::Int>& polMap, const casacore::Vector<casacore::Int>& feedStokes)
    // {setPolMap(polMap); setFeedStokes(feedStokes);};

    //    virtual void prepareConvFunction(const VisBuffer2& vb, CFStore2& cfs)=0;
    //virtual void prepareConvFunction(const VisBuffer2& vb, VBRow2CFBMapType& theMap)=0;
    virtual void prepareConvFunction(const VisBuffer2& vb, VB2CFBMap& theMap)=0;
    virtual casacore::Matrix<casacore::Int> makeBaselineList(const casacore::Vector<casacore::Int>& antList);
    virtual casacore::Int mapAntIDToAntType(const casacore::Int& /*ant*/) {return 0;};
    virtual void setMiscInfo(const casacore::RecordInterface& /*params*/) {};
    virtual casacore::CountedPtr<CFTerms> getTerm(const casacore::String& /*name*/) {return NULL;}
    virtual int getOversampling(){return 1;};

    // virtual casacore::Vector<casacore::Vector<casacore::Double> > findPointingOffset(const casacore::ImageInterface<casacore::Complex>& image,
    // 								  const VisBuffer2& vb, const casacore::Bool& doPointing) = 0;



  private:
    casacore::Int nDim;
  protected:
    casacore::LogIO& logIO() {return logIO_p;}
    casacore::LogIO logIO_p;
    casacore::Cube<casacore::Int> spwChanSelFlag_p;
    casacore::Matrix<casacore::Double> spwFreqSelection_p;
    casacore::Double computeCFAngleRad_p, rotateCFOTFAngleRad_p;
  };
  } // end namespace refim
} // end namespace casa

#endif
