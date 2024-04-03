//# CubeMakeImageAlgorithm.h: class to grid and make a complex image in parallel/serial 
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


#ifndef SYNTHESIS_CUBEMAKEIMAGEALGORITHM_H
#define SYNTHESIS_CUBEMAKEIMAGEALGORITHM_H
#include <synthesis/Parallel/Algorithm.h>
#include <synthesis/Parallel/Applicator.h>
#include <synthesis/ImagerObjects/SynthesisUtilMethods.h>
namespace casa { //# NAMESPACE CASA - BEGIN

	class SIImageStore;

	class CubeMakeImageAlgorithm : public Algorithm {
	public:
		//Constructor/desctructor
		CubeMakeImageAlgorithm();
		~CubeMakeImageAlgorithm();
		//Functions that needs to be overloaded
		void get();

		// Return the results to the controller
		void put();

		// Return the name of the algorithm
		casacore::String &name();
	private:
		void task();
		
		casacore::String myName_p;
		casacore::Vector<SynthesisParamsSelect> dataSel_p;
		SynthesisParamsImage imSel_p;
		SynthesisParamsGrid gridSel_p;
		casacore::Record ftmRec_p;
		casacore::Int polRep_p;
		casacore::Int imageType_p;
		casacore::Vector<casacore::Int> chanRange_p;
		casacore::Record weightParams_p;
		casacore::Bool status_p;
		casacore::String cimageName_p;
	};
	
	
} //# NAMESPACE CASA - END
#endif //SYNTHESIS_CUBEMAJORCYCLEALGORITHM_H


