//# BaselineType.h: Definition of the BaselineType class
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
//
#ifndef SYNTHESIS_TRANSFORM2_BASELINETYPE_H
#define SYNTHESIS_TRANSFORM2_BASELINETYPE_H

#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <synthesis/TransformMachines2/CFBuffer.h>
#include <synthesis/TransformMachines2/PhaseGrad.h>
#include <casa/Utilities/CountedPtr.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN
  namespace refim{
  //
  //-------------------------------------------------------------------------------------------
  //
  class BaselineType
  {
  public:
    BaselineType();

    ~BaselineType();

    BaselineType& operator=(const BaselineType& other);
    
    /* inline const int& getNumBlGrps_p(){return blGroup_p;}; */
    /* inline const int& mapAntIDToAntType(const casacore::Int& /\*ant*\/) {return antID_p;}; */
    /* inline void setTelescopeType(const casacore::String& telescopeName){telescopeName_p = telescopeName;}; */
    int numPhaseGrads(const int& nG);

    casacore::Matrix<casacore::Complex> setBLPhaseGrad(const casacore::CountedPtr<PointingOffsets>& pointingOffset_p,
						       const casacore::CountedPtr<CFBuffer>& cfb,
						       const vi::VisBuffer2& vb,
						       const int& row,
						       const double& sigmaDev);


    casacore::Matrix<vector<int> > findAntennaGroups(const vi::VisBuffer2& vb, 
						     const casacore::CountedPtr<PointingOffsets>& pointingOffsets_p, 
						     const double& sigmaDev);
    vector<vector<double> >pairSort(const vector<int>& antArray, const vector< vector<double> >& poArray);

    int returnIdx(const vector<int>& inpArray, const int& searchVal);


     void cacheAntGroups(const casacore::Matrix<vector<int> > antennaGroups_p);
     void makeVBRow2BLGMap(const vi::VisBuffer2& vb);
     vector<bool> getPOPG(){return blNeedsNewPOPG_p;};
     vector<int> getVBRow2BLMap() {return vbRow2BLMap_p;}
     void setDoPointing(const bool& dop=false) {doPointing_p = dop;}
     void setCachedGroups(const bool& cachedGrps){cachedGroups_p = cachedGrps;};
     bool getCachedGroups(){return cachedGroups_p;};
     pair<int,int> getAntGrpPairs(const int& row){return antPair_p[row];};
     void  setCachedAntennaPO(const casacore::Vector<casacore::Vector<casacore::Double> > cachedPointingOffsets){cachedPointingOffsets_p.assign(cachedPointingOffsets);};
     casacore::Vector<casacore::Vector<casacore::Double> > getCachedAntennaPO(){return cachedAntPointingOffsets_p;};

     casacore::Vector<casacore::Vector<double> > stl2DVecToCasa2DVec (const vector<vector<double> >& stlVec);
  private:  
     
     int vbRows_p;
     bool doPointing_p, newPhaseGradComputed_p, cachedGroups_p;
     casacore::Vector< casacore::CountedPtr<PhaseGrad> >vectorPhaseGradCalculator_p;
     casacore::Int PO_DEBUG_P, totalGroups_p;
     casacore::Matrix< vector<int> > antennaGroups_p, cachedAntennaGroups_p;
     casacore::Matrix< vector < vector <double> > > antennaPO_p, cachedAntennaPO_p;
     vector<bool> blNeedsNewPOPG_p;
     vector<int> vbRow2BLMap_p;
     casacore::Matrix<int> mapAntGrp_p, mapBLGroup_p, cachedmapBLGroup_p;
     casacore::Vector<casacore::Vector<casacore::Double> > cachedPointingOffsets_p, cachedAntPointingOffsets_p;
     vector< pair <int, int> > antPair_p;
     int binsx_p, binsy_p;
     int deltaBinsX_p, deltaBinsY_p;
   };
  //
  //-------------------------------------------------------------------------------------------
  //
  };
};
#endif
