//# WTerm.h: Definition for WTerm
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

#ifndef SYNTHESIS_WTERM_H
#define SYNTHESIS_WTERM_H


#include <casacore/casa/Arrays/Vector.h>
#include <msvis/MSVis/VisBuffer.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/images/Images/TempImage.h>
#include <synthesis/TransformMachines/CFTerms.h>


namespace casa{
  // <summary>  A class to apply the W-Term to the given image </summary>
  
  // <use visibility=export>
  // <prerequisite>
  // </prerequisite>
  // <etymology>
  // W-Term to account for non co-planar baselines
  // </etymology>
  //
  // <synopsis> 
  // 
  //</synopsis>
  class WTerm: public CFTerms
  {
  public:
    WTerm () : CFTerms() {};
    ~WTerm () {};

    virtual void applySky(casacore::Matrix<casacore::Complex>& screen, 
			  const casacore::Int wPixel, 
			  const casacore::Vector<casacore::Double>& sampling,
			  const casacore::Double wScale,
			  const casacore::Int inner);
    virtual void applySky(casacore::Matrix<casacore::Complex>& screen, 
			  const casacore::Vector<casacore::Double>& sampling,
			  const casacore::Double wValue,
			  const casacore::Int inner);
    int getVisParams(const VisBuffer& vb,const casacore::CoordinateSystem& skyCoord=casacore::CoordinateSystem()) 
    {(void)vb;(void)skyCoord;return 0;};
    void setPolMap(const casacore::Vector<casacore::Int>& polMap) {(void)polMap;};
    virtual casacore::Float getSupportThreshold() {return 1e-3;};

    // WTerm normalizes the image be unity
    virtual void normalizeImage(casacore::Lattice<casacore::Complex>& skyImage,
				const casacore::Matrix<casacore::Float>& weights) 
    {(void)skyImage;(void)weights;};
    virtual casacore::String name() {return casacore::String("W Term");};
    //
    // The following functions are not required for W-Term but need to
    // be implemented here since they are pure virtuals in CFTerms
    // base class.
    void applySky(casacore::ImageInterface<casacore::Float>&,// outputImages,
		  const VisBuffer&,// vb, 
		  const casacore::Bool,/* doSquint=true,*/
		  const casacore::Int&,/* cfKey=0,*/
	          const casacore::Int&,/* muellerTerm=0,*/
		  const casacore::Double /*freqVal*=-1*/) 
    {};
    void applySky(casacore::ImageInterface<casacore::Complex>&,// outputImages,
		  const VisBuffer&,// vb, 
		  const casacore::Bool,/* doSquint=true,*/
		  const casacore::Int&,/* cfKey=0,*/
		  const casacore::Int&,/* muellerTerm=0,*/
		  const casacore::Double /*freqVal=-1*/) 
    {};
    casacore::Vector<casacore::Int> vbRow2CFKeyMap(const VisBuffer& vb, casacore::Int& nUnique) 
    {(void)vb;(void)nUnique;return casacore::Vector<casacore::Int>();};
    casacore::Int makePBPolnCoords(const VisBuffer& vb,
			 const casacore::Int& convSize,
			 const casacore::Int& convSampling,
			 const casacore::CoordinateSystem& skyCoord,
			 const casacore::Int& skyNx, const casacore::Int& skyNy,
			 casacore::CoordinateSystem& feedCoord) 
    {
      (void)vb;(void)convSize;(void)convSampling;(void)skyCoord;(void)skyNx;(void)skyNy;(void)feedCoord;
      return 0;
    };

    casacore::Bool rotationallySymmetric() {return true;};


    casacore::Int getConvSize() {return 0;};
    casacore::Int getOversampling() {return 20;};
    casacore::Float getConvWeightSizeFactor() {return 1.0;};
  };

};

#endif
