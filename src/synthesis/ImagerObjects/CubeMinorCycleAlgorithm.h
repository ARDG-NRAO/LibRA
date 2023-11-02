//# CubeMinorCycleAlgorithm.h: class to deconvolve Cubes in parallel/serial 
//# Copyright (C) 2019
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by
//# the Free Software Foundation; either version 3 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
//# License for more details.
//#
//# https://www.gnu.org/licenses/
//#
//# Queries concerning CASA should be submitted at
//#        https://help.nrao.edu
//#
//#        Postal address: CASA Project Manager 
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$


#ifndef SYNTHESIS_CUBEMINORCYCLEALGORITHM_H
#define SYNTHESIS_CUBEMINORCYCLEALGORITHM_H
#include <synthesis/Parallel/Algorithm.h>
#include <synthesis/Parallel/Applicator.h>
#include <synthesis/ImagerObjects/SynthesisUtilMethods.h>
namespace casa { //# NAMESPACE CASA - BEGIN

	class SIImageStore;

	class CubeMinorCycleAlgorithm : public Algorithm {
	public:
		//Constructor/desctructor
		CubeMinorCycleAlgorithm();
		~CubeMinorCycleAlgorithm();
		//Functions that needs to be overloaded
		void get();

		// Return the results to the controller
		void put();

		// Return the name of the algorithm
		casacore::String &name();
	private:
	  void task();
	  std::shared_ptr<SIImageStore> subImageStore();
          void reset();
          void makeTempImage(std::shared_ptr<casacore::ImageInterface<casacore::Float> >& outptr,  const casacore::String& imagename, const casacore::Int chanBeg, const casacore::Int chanEnd, const casacore::Bool writelock=false);
          void writeBackToFullImage(const casacore::String imagename, const casacore::Int chanBeg, const casacore::Int chanEnd, std::shared_ptr<casacore::ImageInterface<casacore::Float> > subimptr);
	  casacore::String myName_p;
	  SynthesisParamsDeconv decPars_p;
	  casacore::Record iterBotRec_p;
	  casacore::String modelName_p;
	  casacore::String residualName_p;
	  casacore::String psfName_p;
	  casacore::String maskName_p;
          casacore::String pbName_p;
          casacore::String posMaskName_p;
	  casacore::Vector<casacore::Int> chanRange_p;
	  casacore::Record returnRec_p;
          casacore::Record beamsetRec_p;
          casacore::Float psfSidelobeLevel_p;
          casacore::Bool autoMaskOn_p;
          casacore::Vector<casacore::Bool> chanFlag_p;
	  casacore::Record statsRec_p;
	  casacore::Record chanFlagRec_p;
	  casacore::Bool status_p;
	};
	
	
} //# NAMESPACE CASA - END
#endif //SYNTHESIS_CUBEMINORCYCLEALGORITHM_H


