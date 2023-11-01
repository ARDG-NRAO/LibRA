// -*- C++ -*-
//# MakeCFArray.h: Definition of the MakeCFArray class
//# Copyright (C) 1997,1998,1999,2000,2001,2002,2003
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
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$

#ifndef SYNTHESIS_TRANSFORM2_MAKECFARRAY_H
#define SYNTHESIS_TRANSFORM2_MAKECFARRAY_H
#include <vector> // std::vector
#include <tuple>
#include <casacore/casa/Arrays/Vector.h> // casa::Vector
#include <msvis/MSVis/VisBuffer2.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <synthesis/TransformMachines2/CFStore2.h>
#include <synthesis/TransformMachines2/CFBuffer.h>
#include <synthesis/TransformMachines2/CFDefs.h>
#include <synthesis/TransformMachines2/MyCFArray.h>
#include <synthesis/TransformMachines2/MyCFArrayShape.h>
#include <hpg/hpg.hpp>
#include <hpg/hpg_indexing.hpp>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN
  namespace refim{
    
    //
    // Global methods
    //--------------------------------------------------------------------------------------------
    //
    //-------------------------------------------------------------------------
    //
    Complex* getConvFuncArray(const double& vbPA, Vector<int>& cfShape,
			      Vector<int>& support,
			      int& muellerElement,
			      casa::refim::CFBuffer& cfb,
			      double& wVal, int& fndx, int& wndx,
			      const casa::refim::PolMapType& mNdx,
			      const casa::refim::PolMapType& conjMNdx,
			      const int& ipol, const uint& mCol,
			      const double& paTolerance);
    double makeAWLists_p(casa::refim::CFBuffer& cfb,
			 const bool& wbAWP,
			 const uint& user_wprojplanes,
			 const ImageInterface<Float>& skyImage,
			 const double& spwRefFreq,
			 const int vbSPW,
			 Vector<Int>& wNdxList, Vector<Int>& spwNdxList);
    //
    // The MakeCFArray class definition
    //-------------------------------------------------------------------------
    //
    class MakeCFArray
    {
    public:
      MakeCFArray(const PolMapType& mndx, const PolMapType& conj_mndx)
	:mndx_p(mndx), conj_mndx_p(conj_mndx), cachedVBSpw_p(-1), cachedFieldID_p(-1),
	 initialized_p(false) {};
      ~MakeCFArray() {};
      
      //
      //--------------------------------------------------------------------------------------------
      //
      std::tuple<bool,                   // If new CFs were loaded (reloadCFs)
		 int,                    // The SPW ID for which the CFs were loaded
		 hpg::CFSimpleIndexer,   // The corresponding CFSI.  This should be used in AWVRHPG to fill hpg::VisData
		 std::shared_ptr<hpg::RWDeviceCFArray>> // The RWDeviceCFArray.  Use it if reloadCFs==true
      makeRWDArray(const bool& wbAWP, const int& nw,
		   const ImageInterface<Float>& skyImage,
		   const Vector<int>& polMap,
		   CountedPtr<casa::refim::CFStore2> cfs2_l,
		   const int& vbSpw_l, const double& spwRefFreq,
		   const int& nDataPol,
		   //VisBuffer2 *vb_l,
		   //CountedPtr<vi::VisibilityIterator2> vi2,
		   Vector<Int>& wNdxList, Vector<Int>& spwNdxList);
      //
      //--------------------------------------------------------------------------------------------
      //
      std::tuple<hpg::CFSimpleIndexer, std::shared_ptr<hpg::RWDeviceCFArray>>
      makeRWDCFA_p(casa::refim::MyCFArrayShape& cfArrayShape,
		   const double& vbPA, const double& imRefFreq,
		   casa::refim::CFBuffer& cfb,
		   const Int& nGridPol, const Int& nDataPol,
		   const Vector<Int>& polMap,
		   Vector<Int>& wNdxList,
		   Vector<Int>& spwNdxList);
      
    private:
      PolMapType mndx_p, conj_mndx_p;
      int cachedVBSpw_p;
      int cachedFieldID_p;
      bool initialized_p;
    };
  } //# NAMESPACE refim - END
} //# NAMESPACE CASA - END
#endif //
