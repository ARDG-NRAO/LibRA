//# PSTerm.h: Definition for PSTerm
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

#ifndef SYNTHESIS_TRANSFORM2_PSTERM_H
#define SYNTHESIS_TRANSFORM2_PSTERM_H

#include <synthesis/TransformMachines2/Utils.h>

#include <casacore/casa/Arrays/Vector.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/images/Images/TempImage.h>
#include <casacore/scimath/Mathematics/ConvolveGridder.h>
#include <synthesis/TransformMachines2/CFStore.h>
#include <synthesis/TransformMachines2/CFTerms.h>

namespace casa{
  namespace refim{
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
  class PSTerm: public CFTerms
  {
  public:
    PSTerm(): CFTerms() {};
    PSTerm (const casacore::IPosition shape, 
	    const casacore::Vector<casacore::Double>& uvScale,
	    const casacore::Vector<casacore::Double>& uvOffset,
	    const casacore::Double& psScale):psCtor_p() 
    {init(shape,uvScale,uvOffset,psScale);};

    virtual ~PSTerm () {};

    void init(const casacore::IPosition shape, 
	      const casacore::Vector<casacore::Double>& uvScale,
	      const casacore::Vector<casacore::Double>& uvOffset,
	      const casacore::Double& psScale);
    void reinit(const casacore::IPosition shape, 
		const casacore::Vector<casacore::Double>& uvScale,
		const casacore::Vector<casacore::Double>& uvOffset,
		const casacore::Double& psScale);
    casacore::Matrix<casacore::Complex>& operator=(casacore::Matrix<casacore::Complex>&);
    casacore::Matrix<casacore::Complex>& operator*=(casacore::Matrix<casacore::Complex>&);

    void applySky(casacore::Matrix<casacore::Complex>& screen,casacore::Bool multiply=false);
    void applySky(casacore::Matrix<casacore::Complex>& screen, 
		  const casacore::Vector<casacore::Double>& sampling,
		  const casacore::Int inner);
    void normalizeImage(casacore::Lattice<casacore::Complex>& skyImage,
			const casacore::Matrix<casacore::Float>& weights);
    virtual casacore::String name() {return casacore::String("PS Term");};

    //----------------------------------------------------------
    // Implementation of pure-virtual methods of CFTerms parent class.
    // CFTerms (and its derived classes) ned clean-up so that this
    // kind of code is not required everywhere).
    int getVisParams(const VisBuffer2& vb,const casacore::CoordinateSystem& skyCoord=casacore::CoordinateSystem()) 
    {(void)vb;(void)skyCoord;return 0;};

    void setPolMap(const casacore::Vector<casacore::Int>& polMap) {(void)polMap;};

    virtual casacore::Float getSupportThreshold() {return 1e-3;};

    void applySky(casacore::ImageInterface<casacore::Float>&,// outputImages,
		  const VisBuffer2&,// vb, 
		  const casacore::Bool,// doSquint=true,
		  const casacore::Int&,// cfKey=0,
		  const casacore::Int&,// muellerTerm=0,
		  const casacore::Double /*freqVal=-1*/) 
    {};

    void applySky(casacore::ImageInterface<casacore::Complex>&,// outputImages,
		  const VisBuffer2&,// vb, 
		  const casacore::Bool,// doSquint=true,
		  const casacore::Int&,// cfKey=0,
		  const casacore::Int&,// muellerTerm=0,
		  const casacore::Double /*freqVal= -1*/ ) 
    {};

    casacore::Vector<casacore::Int> vbRow2CFKeyMap(const VisBuffer2& /*vb*/, casacore::Int& /*nUnique*/) 
    {casacore::Vector<casacore::Int> tt;return tt;};

    casacore::Int makePBPolnCoords(const VisBuffer2&,// vb,
			 const casacore::Int&,// convSize,
			 const casacore::Int&,// convSampling,
			 const casacore::CoordinateSystem&,// skyCoord,
			 const casacore::Int&,// skyNx, 
			 const casacore::Int&,// skyNy,
			 casacore::CoordinateSystem&// feedCoord
			 ) 
    {return 0;};

    casacore::Int getConvSize() {return 0;};
    casacore::Int getOversampling() {return 20;};
    void setConvSize(const casacore::Int) {};
    casacore::Float getConvWeightSizeFactor() {return 1.0;};

    casacore::Bool rotationallySymmetric() {return true;};

  private:
    // spheroidal function
    casacore::CountedPtr<casacore::ConvolveGridder<casacore::Double, casacore::Complex> > psCtor_p;
    casacore::Vector<casacore::Double>  support_p;
    casacore::Vector<casacore::Int> sampling_p;
    casacore::Double psScale_p;
    casacore::IPosition shape_p;
    casacore::Vector<casacore::Double> uvScale_p;
    casacore::Vector<casacore::Double> uvOffset_p;
  };
  };
};

#endif
