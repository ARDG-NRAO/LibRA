//# CFTerms.h: Definition for CFTerms
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

#ifndef SYNTHESIS_TRANSFORM2_CFTERMS_H
#define SYNTHESIS_TRANSFORM2_CFTERMS_H


#include <casa/Arrays/Vector.h>
#include <images/Images/ImageInterface.h>
#include <images/Images/PagedImage.h>
#include <images/Images/TempImage.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <casa/Containers/Block.h>

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
  class CFTerms
  {
  public:
    enum OpCodes {NOOP=0,NORMAL};
    CFTerms ():opCode_p(NORMAL) {};
    virtual ~CFTerms () {};

    virtual void setOpCode(OpCodes code) {opCode_p = code;}
    virtual casacore::Bool isNoOp() {return opCode_p==NOOP;};

    virtual casacore::String name() = 0;

    virtual void applySky(casacore::ImageInterface<casacore::Float>& outputImages,
			  const VisBuffer2& vb, 
			  const casacore::Bool doSquint=true,
			  const casacore::Int& cfKey=0,
			  const casacore::Int& muellerTerms=0,
			  const casacore::Double freqVal=-1) = 0;
    virtual void applySky(casacore::ImageInterface<casacore::Complex>& outputImages,
			  const VisBuffer2& vb, 
			  const casacore::Bool doSquint=true,
			  const casacore::Int& cfKey=0,
			  const casacore::Int& muellerTerms=0,
			  const casacore::Double freqVal=-1) = 0;
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
    virtual casacore::Vector<casacore::Int> vbRow2CFKeyMap(const VisBuffer2& vb, casacore::Int& nUnique) = 0;
    virtual casacore::Int makePBPolnCoords(const VisBuffer2& vb,
				 const casacore::Int& convSize,
				 const casacore::Int& convSampling,
				 const casacore::CoordinateSystem& skyCoord,
				 const casacore::Int& skyNx, const casacore::Int& skyNy,
				 casacore::CoordinateSystem& feedCoord) = 0;

    virtual casacore::Int getConvSize() = 0;
    virtual void setConvSize(const casacore::Int val) = 0;
    virtual casacore::Int getOversampling() = 0;

    virtual casacore::Float getConvWeightSizeFactor() = 0;
    virtual casacore::Float getSupportThreshold() = 0;
    
    virtual void normalizeImage(casacore::Lattice<casacore::Complex>& skyImage,
				const casacore::Matrix<casacore::Float>& weights) = 0;

    virtual int getVisParams(const VisBuffer2& vb, const casacore::CoordinateSystem& skyCoord=casacore::CoordinateSystem()) = 0;
    //
    // The mapping from VisBuffer polarizations map to the Image plane
    // polarization.  The latter is determined by the user input,
    // which is passed to the FTMachine in Imager.cc
    //
    // The map is available in the FTMachine which uses this method to
    // set the map for the CFTerms object.
    //
    virtual void setPolMap(const casacore::Vector<casacore::Int>& polMap) {polMap_p_base.resize(0);polMap_p_base=polMap;}
    virtual void getPolMap(casacore::Vector<casacore::Int>& polMap) {polMap.resize(0); polMap = polMap_p_base;};

    virtual casacore::Bool rotationallySymmetric() = 0;

    void setBandName(const casacore::String& bandName) {bandName_p = bandName;};
    casacore::String& getBandName() {return bandName_p;};

  protected:
    casacore::LogIO& logIO() {return logIO_p;}
    casacore::LogIO logIO_p;
    casacore::Vector<casacore::Int> polMap_p_base;
    OpCodes opCode_p;
    casacore::String telescopeName_p, bandName_p;
  };

  }///refim
}//casa
#endif
