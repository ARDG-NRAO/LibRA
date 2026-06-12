// -*- C++ -*-
//# CFCacheHelper.h: Definition for CFCacheHelper functions
//# Copyright (C) 2021
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
//# Correspondence concerning this should be addressed as follows:
//#        Postal address: National Radio Astronomy Observatory
//#                        1003 Lopezville Road,
//#                        Socorro, NM - 87801, USA
//#
//# $Id$
#ifndef SYNTHESIS_TRANSFORM2_CFCACHEHELPER_H
#define SYNTHESIS_TRANSFORM2_CFCACHEHELPER_H
#include <casacore/measures/Measures/Stokes.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/TempImage.h>
#include <casacore/casa/Utilities/CountedPtr.h>
// #include <casacore/tables/TaQL/ExprNode.h>

#include <synthesis/TransformMachines2/PolOuterProduct.h>
#include <synthesis/TransformMachines2/CFStore2.h>
#include <synthesis/TransformMachines2/CFCache.h>
#include <synthesis/TransformMachines2/AWConvFunc.h>
#include <libracore/DataBase.h>

using namespace casa;
using namespace casa::refim;
using namespace casacore;
using namespace std;

namespace casa
{
  namespace refim
  {
    namespace SynthesisUtils
    {
      enum CFCHelperCodes {MAKE_CFCFS, MAKE_WTCFS, MAKE_BOTHCFS};

      CountedPtr<refim::PolOuterProduct>
      setPOP(vi::VisBuffer2 &vb2,
	     Vector<casacore::Stokes::StokesTypes> visPolMap,
	     Vector<int> polMap,
	     std::string &stokes, std::string &mType);

      void fillCFS_inmemory(const string& cfCacheName,
			    CountedPtr<casa::refim::CFStore2> cfs2_l,
			    CountedPtr<casa::refim::CFStore2> cfswt2_l,
			    Vector<double>& uvOffset,
			    const bool& psTerm,
			    const bool& aTerm,
			    const bool& conjBeams);

      void makeCFS_inmemory(DataBase& db,
			    CountedPtr<casa::refim::CFStore2> cfs2_l,
			    CountedPtr<casa::refim::CFStore2> cfswt2_l,
			    refim::ConvolutionFunction& awcf_l,
			    const TempImage<Complex>& cgrid,
			    int& nW, float& pa, float& dpa,
			    const Vector<double>& uvScale,
			    const Vector<double>& uvOffset,
			    const string cfCacheName="",
			    std::string stokes=std::string("I"),
			    std::string mType="");

      std::tuple<CountedPtr<casa::refim::CFStore2>,
		 CountedPtr<casa::refim::CFStore2>>
      constructCFS(refim::CFCache* cfCacheObj,
		   const std::string& cfCacheName,
		   const std::vector<std::string>& cfList,
		   const std::vector<std::string>& wtCFList,
		   const std::string& mode,
		   const double& pa,
		   const double& dpa,
		   const CFCHelperCodes whichCFS=MAKE_BOTHCFS);
    }
  }
}
#endif
