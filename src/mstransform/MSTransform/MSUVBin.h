/*
 * MSUVBin.h: This file contains the interface definition of the MSUVBin class.
//#
//#  CASA - Common Astronomy Software Applications (http://casa.nrao.edu/)

 //# Copyright (C) 2014
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU  General Public License as published by
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

//#        Postal address:
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#

 *  Created on: Feb 3, 2014
 *      Author: kgolap
 */

#ifndef MSTRANSFORM_MSUVBIN_H
#define MSTRANSFORM_MSUVBIN_H
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/VisBufferUtil.h>
#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/VisBufferUtil.h>
namespace casa { //# NAMESPACE CASA - BEGIN


class MSUVBin {
public:
	MSUVBin();
	//npol should be only 1, 2 (parallel hands)  or 4
	MSUVBin(const casacore::MDirection& phaseCenter, const casacore::Int nx,
		const casacore::Int ny, const casacore::Int nchan, const casacore::Int npol, casacore::Quantity cellx, casacore::Quantity celly, casacore::Quantity freqStart, casacore::Quantity freqStep, casacore::Float memFraction=0.5, casacore::Bool dow=false, casacore::Bool doflag=false);
	//Constructor to bin into an existing gridded ms
	MSUVBin (casacore::MeasurementSet& ms);
	//The following can be called multiple times to setup multiple input MS
	virtual casacore::Bool selectData(const casacore::String& msname, const casacore::String& spw = "", const casacore::String& field = "",
			const casacore::String& baseline = "", const casacore::String& scan = "",
			const casacore::String& uvrange = "", const casacore::String& taql = "",
			const casacore::String& subarray = "", const casacore::String& correlation = "",
			const casacore::String& intent = "", const casacore::String& obs = "");
	//void setInputMS(const casacore::MeasurementSet& ms);
	//void setInputMS(const casacore::Block<const casacore::MeasurementSet*> mssPtr);
	void setOutputMS(const casacore::String& msname);
	//forceDiskUsage is to avoid using in memory gridding even if there is
	casacore::Bool fillOutputMS();
	virtual ~MSUVBin();
	//Helper function for creating casacore::MDirection from a string
	static casacore::Bool String2MDirection(const casacore::String& theString,
					            casacore::MDirection& theMeas, const casacore::String msname="");

private:
	static casacore::Int sepCommaEmptyToVectorStrings(casacore::Vector<casacore::String>& retStr,
			  const casacore::String& str);
	casacore::Bool fillSmallOutputMS();
	casacore::Bool fillNewBigOutputMS();
	casacore::Bool fillBigOutputMS();
	casacore::Int recoverGridInfo(const casacore::String& msname);
	void storeGridInfo();
	void createOutputMS(const casacore::Int nrrows);
	casacore::Int makeUVW(const casacore::Double reffreq, casacore::Vector<casacore::Double>& incr, casacore::Vector<casacore::Int>& cent, casacore::Matrix<casacore::Double>&uvw);
	void locateuvw(casacore::Matrix<casacore::Int>& locuv, const casacore::Vector<casacore::Double>& increment,
			const casacore::Vector<casacore::Int>& center, const casacore::Matrix<casacore::Double>& uvw);
	void gridData(const vi::VisBuffer2& vb, casacore::Cube<casacore::Complex>& grid,
			casacore::Matrix<casacore::Float>& wght, casacore::Cube<casacore::Float>& wghtSpec,
			casacore::Cube<casacore::Bool>& flag, casacore::Vector<casacore::Bool>& rowFlag, casacore::Matrix<casacore::Double>& uvw, casacore::Vector<casacore::Int>& ant1,
			casacore::Vector<casacore::Int>& ant2, casacore::Vector<casacore::Double>& time, const casacore::Matrix<casacore::Int>& locuv);
	void gridData(const vi::VisBuffer2& vb, casacore::Cube<casacore::Complex>& grid,
			casacore::Matrix<casacore::Float>& wght, casacore::Cube<casacore::Float>& wghtSpec,
			casacore::Cube<casacore::Bool>& flag, casacore::Vector<casacore::Bool>& rowFlag, casacore::Matrix<casacore::Double>& uvw, casacore::Vector<casacore::Int>& ant1,
			casacore::Vector<casacore::Int>& ant2, casacore::Vector<casacore::Double>& time, const casacore::Int startchan, const casacore::Int endchan);
	void gridDataConv(const vi::VisBuffer2& vb, casacore::Cube<casacore::Complex>& grid,
		       casacore::Matrix<casacore::Float>& /*wght*/, casacore::Cube<casacore::Complex>& wghtSpec,
		       casacore::Cube<casacore::Bool>& flag, casacore::Vector<casacore::Bool>& rowFlag, casacore::Matrix<casacore::Double>& uvw, casacore::Vector<casacore::Int>& ant1,
		       casacore::Vector<casacore::Int>& ant2, casacore::Vector<casacore::Double>& timeCen, const casacore::Int startchan, const casacore::Int endchan, 
		       const casacore::Cube<casacore::Complex>& convFunc, const casacore::Vector<casacore::Int>& convSupport, const casacore::Double wScale, const casacore::Int convSampling);
	void gridDataConvThr(const vi::VisBuffer2& vb, casacore::Cube<casacore::Complex>& grid,
			     casacore::Cube<casacore::Complex>& wghtSpec,
		       casacore::Cube<casacore::Bool>& flag, casacore::Vector<casacore::Bool>& rowFlag, casacore::Matrix<casacore::Double>& uvw, casacore::Vector<casacore::Int>& ant1,
		       casacore::Vector<casacore::Int>& ant2, casacore::Vector<casacore::Double>& timeCen, const casacore::Int startchan, const casacore::Int endchan, 
		       const casacore::Cube<casacore::Complex>& convFunc, const casacore::Vector<casacore::Int>& convSupport, const casacore::Double wScale, const casacore::Int convSampling);
	void inplaceGridData(const vi::VisBuffer2& vb);
	void inplaceLargeBW(const vi::VisBuffer2& vb);
	void inplaceSmallBW(const vi::VisBuffer2& vb);
	void makeCoordsys();
	void weightSync();
	void multiThrLoop(const casacore::Int outchan, const vi::VisBuffer2& vb, casacore::Double refFreq,  
			 casacore::Vector<casacore::Float> scale, casacore::Bool hasCorrected,casacore::Bool needRot, 
			 const casacore::Vector<casacore::Double>& phasor, const casacore::Vector<casacore::Double>& visFreq, 
			 const casacore::Double& fracbw,  
			 casacore::Complex*& grid, casacore::Complex*& wghtSpec, casacore::Bool*& flag, 
			 casacore::Bool*& rowFlag, casacore::Double*& uvw, casacore::Int*& ant1,
			  casacore::Int*& ant2, casacore::Double*& timeCen, casacore::Double*& sumWeight, casacore::Double*& numvis, 
			  const casacore::Int startchan, 
			 const casacore::Int endchan, const casacore::Cube<casacore::Complex>& convFunc, const casacore::Vector<casacore::Int>& convSupport, const casacore::Double wScale, const casacore::Int convSampling );
	void locateFlagFromGrid(vi::VisBuffer2& vb, casacore::Cube<casacore::Bool>& datFlag,
				casacore::Cube<casacore::Float>& wghtSpec,
				casacore::Cube<casacore::Bool>& flag, casacore::Vector<casacore::Bool>& rowFlag, casacore::Matrix<casacore::Double>& uvw, casacore::Vector<casacore::Int>& ant1,
				casacore::Vector<casacore::Int>& ant2, casacore::Vector<casacore::Double>& timeCen, const casacore::Int startchan, const casacore::Int endchan);
	// returns a false if either no channel map or pol map onto grid
	casacore::Bool datadescMap(const vi::VisBuffer2& vb, casacore::Double& fracbw);
	casacore::Bool datadescMap(const VisBuffer& vb);
	casacore::Bool saveData(const casacore::Cube<casacore::Complex>& grid, const casacore::Cube<casacore::Bool>&flag, const casacore::Vector<casacore::Bool>& rowFlag,
					const casacore::Cube<casacore::Float>&wghtSpec, const casacore::Matrix<casacore::Float>& wght,
					const casacore::Matrix<casacore::Double>& uvw, const casacore::Vector<casacore::Int>& ant1,
					const casacore::Vector<casacore::Int>& ant2, const casacore::Vector<casacore::Double>& time);

	casacore::Bool saveData(const casacore::Cube<casacore::Complex>& grid, const casacore::Cube<casacore::Bool>&flag, const casacore::Vector<casacore::Bool>& rowFlag,
					const casacore::Cube<casacore::Float>&wghtSpec,
					const casacore::Matrix<casacore::Double>& uvw, const casacore::Vector<casacore::Int>& ant1,
		      const casacore::Vector<casacore::Int>& ant2, const casacore::Vector<casacore::Double>& time, const casacore::Int startchan, const casacore::Int endchan, 	const casacore::Cube<casacore::Float>& imagwghtSpec=casacore::Cube<casacore::Float>());
	void fillSubTables();
	void fillFieldTable();
	void copySubtable(const casacore::String& tabName, const casacore::Table& inTab,
	                    const casacore::Bool noRows=false);
	void fillDDTables();
	void setTileCache();
	void makeSFConv(casacore::Cube<casacore::Complex>&convFunc, 
			casacore::Vector<casacore::Int>& convSupport, casacore::Double& wScale, casacore::Int& convSampling, 
			casacore::Int& convSize);
	void makeWConv(vi::VisibilityIterator2& iter,casacore::Cube<casacore::Complex>& convFunc, 
		       casacore::Vector<casacore::Int>& convSupport,
			casacore::Double& wScale, casacore::Int& convSampling, casacore::Int& convSize );
	casacore::CoordinateSystem csys_p;
	casacore::Int nx_p, ny_p, nchan_p, npol_p;
	casacore::Double freqStart_p, freqStep_p;
	casacore::Vector<casacore::Int> chanMap_p, polMap_p;
	casacore::Vector<casacore::Vector<casacore::Int> > chanMapRev_p;
	casacore::MDirection phaseCenter_p;
	casacore::Vector<casacore::Double> deltas_p;
	casacore::Vector<casacore::Int> whichStokes_p;
	casacore::Bool existOut_p;
	casacore::String outMSName_p;
	casacore::CountedPtr<casacore::MeasurementSet> outMsPtr_p;
	casacore::Block<const casacore::MeasurementSet *> mss_p;
	VisBufferUtil vbutil_p;
	casacore::Float memFraction_p;
	casacore::Bool doW_p, doFlag_p;
	casacore::Matrix<casacore::Double> numVis_p;
	casacore::Matrix<casacore::Double> sumWeight_p;

}; // end class MSUVBin
} //# NAMESPACE CASA - END
#endif /* MSTRANSFORM_MSUVBIN_H */
