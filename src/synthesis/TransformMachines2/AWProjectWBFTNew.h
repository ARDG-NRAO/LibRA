//# AWProjectWBFTNew.h: Definition for AWProjectWBFTNew
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

#ifndef SYNTHESIS_TRANSFORM2_AWPROJECTWBFTNEW_H
#define SYNTHESIS_TRANSFORM2_AWPROJECTWBFTNEW_H
#define DELTAPA 1.0
#define MAGICPAVALUE -999.0


#include <synthesis/TransformMachines2/AWProjectWBFT.h>

namespace casa { //# NAMESPACE CASA - BEGIN
  
  namespace refim {
  class AWProjectWBFTNew : public AWProjectWBFT {

  public:
    AWProjectWBFTNew(casacore::Int nFacets, casacore::Long cachesize,
		     casacore::CountedPtr<CFCache>& cfcache,
		     casacore::CountedPtr<ConvolutionFunction>& cf,
		     casacore::CountedPtr<VisibilityResamplerBase>& visResampler,
		     casacore::Bool applyPointingOffset=true,
		     /* casacore::Vector<casacore::Float> pointingOffsetSigDev = {10,10}, */
		     vector<float> pointingOffsetSigDev = {10,10},
		     casacore::Bool doPBCorr=true,
		     casacore::Int tilesize=16, 
		     casacore::Float paSteps=5.0, 
		     casacore::Float pbLimit=5e-4,
		     casacore::Bool usezero=false,
		     casacore::Bool conjBeams_p=true,
		     casacore::Bool doublePrecGrid=false):
    AWProjectWBFT(nFacets, cachesize, cfcache, cf, visResampler, applyPointingOffset, pointingOffsetSigDev,
		    doPBCorr, tilesize, paSteps, pbLimit, usezero, conjBeams_p, doublePrecGrid){}

    // Construct from a casacore::Record containing the AWProjectWBFT state
    AWProjectWBFTNew(const casacore::RecordInterface& stateRec):AWProjectWBFT(stateRec){};
    
    // Copy constructor
    //AWProjectWBFTNew(const AWProjectWBFTNew &other):AWProjectWBFT() {operator=(other);};

    virtual casacore::String name() const {return "AWProjectWBFTNew";};
    
    ~AWProjectWBFTNew(){};

    FTMachine* cloneFTM();

    virtual casacore::Bool useWeightImage(){return true;};
    virtual void setDryRun(casacore::Bool val)
    {
      isDryRun=val;
      //cerr << "###### " << isDryRun << endl;
    };

  protected:
    void ftWeightImage(casacore::Lattice<casacore::Complex>& wtImage, 
		       const casacore::Matrix<casacore::Float>& sumWt,
		       const casacore::Bool& doFFTNorm);

  private:

  };
} //# NAMESPACE CASA - END
};
#endif
