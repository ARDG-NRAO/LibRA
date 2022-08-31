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

#ifndef SYNTHESIS_TRANSFORM2_NOOPATERM_H
#define SYNTHESIS_TRANSFORM2_NOOPATERM_H


#include <casacore/casa/Arrays/Vector.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/images/Images/TempImage.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <casacore/casa/Containers/Block.h>
#include <synthesis/TransformMachines2/CFTerms.h>
#include <synthesis/TransformMachines2/CFStore.h>
#include <synthesis/TransformMachines2/CFStore2.h>

namespace casa{
  // <summary>  
  //  The base class to represent the Aperture-Term of the Measurement Equation. 
  // </summary>
  
  // <use visibility=export>
  // <prerequisite>
  // </prerequisite>
  // <etymology>
  //   A NoOp A-Term which does nothing but keeps the framework unchanged.  Used when no A-term corrections are required.
  // </etymology>
  //
  // <synopsis> 
  // 
  //</synopsis>
  namespace refim{
  class NoOpATerm: public ATerm
  {
  public:
    NoOpATerm (): ATerm() {};
    virtual ~NoOpATerm () {};

    virtual casacore::String name() {return casacore::String("NoOpATerm");}

    virtual void makeFullJones(casacore::ImageInterface<casacore::Complex>&,// pbImage,
			       const VisBuffer2&,// vb,
			       casacore::Bool,// doSquint, 
			       casacore::Int&,// bandID, 
			       casacore::Double// freqVal
			       ) 
    {};

    virtual void applySky(casacore::ImageInterface<casacore::Float>& ,//outputImages,
			  const VisBuffer2& ,//vb, 
			  const casacore::Bool ,//doSquint=true,
			  const casacore::Int& ,//cfKey=0,
			  const casacore::Int& ,//muellerTerm=0,
			  const casacore::Double // freqVal=-1
			  ) 
    {};
    virtual void applySky(casacore::ImageInterface<casacore::Complex>& ,//outputImages,
			  const VisBuffer2& ,//vb, 
			  const casacore::Bool ,//doSquint=true,
			  const casacore::Int& ,//cfKey=0,
			  const casacore::Int& ,//muellerTerm=0,
			  const casacore::Double // freqVal=-1
			  ) 
    {};

    virtual void applySky(casacore::ImageInterface<casacore::Complex>&,// outImages,
			  const casacore::Double&,// pa,
			  const casacore::Bool,// doSquint,
			  const casacore::Int&,// cfKey,
			  const casacore::Int&,// muellerTerm,
			  const casacore::Double// freqVal=-1.0
			  )
    {};

    casacore::Int getBandID(const casacore::Double&, //freq 
			    const casacore::String&, //telescopeName
			    const casacore::String& //bandName
			    )
    {return 0;};

    void cacheVBInfo(const casacore::String& /*telescopeName*/, const casacore::Float& /*diameter*/) {};
    void cacheVBInfo(const VisBuffer2& )    {};

    virtual void normalizeImage(casacore::Lattice<casacore::Complex>& ,//skyImage,
				const casacore::Matrix<casacore::Float>& // weights
				) 
    {};

    virtual int getVisParams(const VisBuffer2& ,// vb
			     const casacore::CoordinateSystem& // skyCoord=casacore::CoordinateSystem()
			     ) 
    {return 0;};
    
    virtual void rotate(const VisBuffer2& ,//vb, 
			CFCell&, // cfs
			const casacore::Double& // rotAngle
			) 
    {};
    virtual void rotate2(const VisBuffer2& ,//vb, 
			CFCell&, // basecfs
			CFCell&, // cfs
			const casacore::Double& // rotAngle
			) 
    {};
    //
    // As the name indicates, this class should always return true
    //
    virtual casacore::Bool isNoOp() {return true;};

    //
    // Method used in the framework for other CFTerms as well.  These are now all in the base class.
    //
    // virtual casacore::Int getConvSize() {};
    // virtual casacore::Vector<casacore::Int> vbRow2CFKeyMap(const VisBuffer2& vb, casacore::Int& nUnique) 
    //     {casacore::Vector<casacore::Int> tmp; tmp.resize(vb.nRow()); tmp=0; nUnique=1; return tmp;}

    // virtual casacore::Int makePBPolnCoords(const VisBuffer2& vb,
    // 				 const casacore::Int& convSize,
    // 				 const casacore::Int& convSampling,
    // 				 const casacore::CoordinateSystem& skyCoord,
    // 				 const casacore::Int& skyNx, const casacore::Int& skyNy,
    // 				 casacore::CoordinateSystem& feedCoord) {throw(casacore::AipsError("NoOpATerm::makePBPolnCoords() called"));};

    // virtual casacore::Float getConvWeightSizeFactor()           {return 1.0;};
    // virtual casacore::Int getOversampling()                     {return 20;};
    virtual casacore::Float getSupportThreshold()               {return 1e-3;};
    // virtual casacore::Int mapAntIDToAntType(const casacore::Int& /*ant*/) {return 0;};
    // virtual void setPolMap(const casacore::Vector<casacore::Int>& polMap) {polMap_p_base.resize(0);polMap_p_base=polMap;}
    // virtual void getPolMap(casacore::Vector<casacore::Int>& polMap)       {polMap.resize(0); polMap = polMap_p_base;};
    // virtual casacore::Vector<casacore::Int> getAntTypeList()              {casacore::Vector<casacore::Int> tt(1); tt=0;return tt;};
    virtual casacore::Bool rotationallySymmetric() {return false;};
  };

};
};
#endif
