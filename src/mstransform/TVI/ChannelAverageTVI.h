//# ChannelAverageTVI.h: This file contains the interface definition of the MSTransformManager class.
//#
//#  CASA - Common Astronomy Software Applications (http://casa.nrao.edu/)
//#  Copyright (C) Associated Universities, Inc. Washington DC, USA 2011, All rights reserved.
//#  Copyright (C) European Southern Observatory, 2011, All rights reserved.
//#
//#  This library is free software; you can redistribute it and/or
//#  modify it under the terms of the GNU Lesser General Public
//#  License as published by the Free software Foundation; either
//#  version 2.1 of the License, or (at your option) any later version.
//#
//#  This library is distributed in the hope that it will be useful,
//#  but WITHOUT ANY WARRANTY, without even the implied warranty of
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//#  Lesser General Public License for more details.
//#
//#  You should have received a copy of the GNU Lesser General Public
//#  License along with this library; if not, write to the Free Software
//#  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
//#  MA 02111-1307  USA
//# $Id: $

#ifndef ChannelAverageTVI_H_
#define ChannelAverageTVI_H_

// Base class
#include <mstransform/TVI/FreqAxisTVI.h>

//#define REPORT_TIMING

namespace casa { //# NAMESPACE CASA - BEGIN

namespace vi { //# NAMESPACE VI - BEGIN

//////////////////////////////////////////////////////////////////////////
// ChannelAverageTVI class
//////////////////////////////////////////////////////////////////////////

class ChannelAverageTVI : public FreqAxisTVI
{

public:

	ChannelAverageTVI(ViImplementation2 * inputVii, const casacore::Record &configuration);
	// Report the the ViImplementation type
	virtual casacore::String ViiType() const override { return casacore::String("ChannelAverage( ")+getVii()->ViiType()+" )"; };

#ifdef _OPENMP
  virtual void originChunks(casacore::Bool forceRewind) {  
    Tfl_=Tws_=Tss_=Tcd_=Tmd_=Tchave_=0.0;  getVii()->originChunks(forceRewind); } 
 #ifdef REPORT_TIMING
  virtual ~ChannelAverageTVI() {
    cout << "ChannelAverageTVI: " << endl
	 << " T_flagCube            =" << Tfl_ << endl 
	 << " T_weightSpectrum      =" << Tws_ << endl
	 << " T_sigmaSpectrum       =" << Tss_ << endl
	 << " T_visibilityCorrected =" << Tcd_ << endl
	 << " T_visibilityModel     =" << Tmd_ << endl
	 << " T_total               =" << Tfl_+Tws_+Tss_+Tcd_+Tmd_
	 << endl;
  }
 #endif
#endif

    void flag(casacore::Cube<casacore::Bool>& flagCube) const override;
    void floatData (casacore::Cube<casacore::Float> & vis) const override;
    void visibilityObserved (casacore::Cube<casacore::Complex> & vis) const override;
    void visibilityCorrected (casacore::Cube<casacore::Complex> & vis) const override;
    void visibilityModel (casacore::Cube<casacore::Complex> & vis) const override;
    void weightSpectrum(casacore::Cube<casacore::Float> &weightSp) const override;
    void sigmaSpectrum (casacore::Cube<casacore::Float> &sigmaSp) const override;

    casacore::Bool weightSpectrumExists () const override {return true;}
    casacore::Bool sigmaSpectrumExists () const override {return true;}
    
    casacore::Vector<casacore::Double> getFrequencies (	casacore::Double time, casacore::Int frameOfReference,
    								casacore::Int spectralWindowId, casacore::Int msId) const override;
    casacore::Vector<casacore::Double> getChanWidths ( casacore::Double time, casacore::Int frameOfReference,
    								casacore::Int spectralWindowId, casacore::Int msId) const override;


    void writeFlag (const casacore::Cube<casacore::Bool> & flag) override;

protected:

    void propagateChanAvgFlags (const casacore::Cube<casacore::Bool> &avgFlagCube, casacore::Cube<casacore::Bool> &expandedFlagCube);
    casacore::Bool parseConfiguration(const casacore::Record &configuration);
    void initialize();

	casacore::Vector<casacore::Int> chanbin_p;
	// (back)propagate flags as flagdata likes it (CAS-12737): existing ones are always preserved
	bool flagdataFlagPropagation_p;
	mutable std::map<casacore::Int,casacore::uInt > spwChanbinMap_p; // Must be accessed from const methods

#ifdef _OPENMP
  mutable casacore::Double Tfl_,Tws_,Tcd_,Tmd_,Tss_,Tchave_;
#endif
};

//////////////////////////////////////////////////////////////////////////
// ChannelAverageTVIFactory class
//////////////////////////////////////////////////////////////////////////

class ChannelAverageTVIFactory : public ViFactory
{

public:

	ChannelAverageTVIFactory(casacore::Record &configuration,
	                         ViImplementation2 *inputVII);

protected:

	vi::ViImplementation2 * createVi () const;

	casacore::Record configuration_p;
	ViImplementation2 *inputVii_p;;
};

//////////////////////////////////////////////////////////////////////////
// ChannelAverageTVILayerFactory class  (for _recursive_ layering)
//////////////////////////////////////////////////////////////////////////

class ChannelAverageTVILayerFactory : public ViiLayerFactory
{

public:

  ChannelAverageTVILayerFactory(casacore::Record &configuration);

  virtual ~ChannelAverageTVILayerFactory() {};

protected:

  
  virtual ViImplementation2 * createInstance(ViImplementation2* vii0) const;

  const casacore::Record configuration_p;

};

//////////////////////////////////////////////////////////////////////////
// ChannelAverageTransformEngine class
//////////////////////////////////////////////////////////////////////////


template<class T> class ChannelAverageKernel; // Forward declaration

template<class T> class ChannelAverageTransformEngine : public FreqAxisTransformEngine2<T>
{

	using FreqAxisTransformEngine2<T>::inputData_p;
	using FreqAxisTransformEngine2<T>::outputData_p;

public:

	ChannelAverageTransformEngine	(	ChannelAverageKernel<T> *kernel,
										DataCubeMap *inputData,
										DataCubeMap *outputData,
										casacore::uInt width);

  // package transformation of all corr,row spectra in the in/outputData_p
  // gmoellen (2017Mar06)
  void transformAll();

	void transform();

protected:

	casacore::uInt width_p;
	// This member has to be a pointer, otherwise there
	// are compile time problems due to the fact that
	// it is a pure virtual class.
	ChannelAverageKernel<T> *chanAvgKernel_p;
};

//////////////////////////////////////////////////////////////////////////
// ChannelAverageKernel class
//////////////////////////////////////////////////////////////////////////

template<class T> class ChannelAverageKernel
{

public:

	virtual void kernel(DataCubeMap *inputData,
						DataCubeMap *outputData,
						casacore::uInt startInputPos,
						casacore::uInt outputPos,
						casacore::uInt width) = 0;
};

//////////////////////////////////////////////////////////////////////////
// PlainChannelAverageKernel class
//   (numerical averaging, ignoring flags)
//////////////////////////////////////////////////////////////////////////

template<class T> class PlainChannelAverageKernel : public ChannelAverageKernel<T>
{

public:

	void kernel(	DataCubeMap *inputData,
					DataCubeMap *outputData,
					casacore::uInt startInputPos,
					casacore::uInt outputPos,
					casacore::uInt width);

};

//////////////////////////////////////////////////////////////////////////
// FlaggedChannelAverageKernel class
//   (numerical averaging, respecting flags)
//////////////////////////////////////////////////////////////////////////

template<class T> class FlaggedChannelAverageKernel : public ChannelAverageKernel<T>
{

public:

	void kernel(DataCubeMap *inputData,
				DataCubeMap *outputData,
				casacore::uInt startInputPos,
				casacore::uInt outputPos,
				casacore::uInt width);
};

//////////////////////////////////////////////////////////////////////////
// WeightedChannelAverageKernel class
//   (weighted averaging, respecting flags)
//////////////////////////////////////////////////////////////////////////

template<class T> class WeightedChannelAverageKernel : public ChannelAverageKernel<T>
{

public:

	void kernel(DataCubeMap *inputData,
				DataCubeMap *outputData,
				casacore::uInt startInputPos,
				casacore::uInt outputPos,
				casacore::uInt width);
};

//////////////////////////////////////////////////////////////////////////
// LogicalANDKernel class
//////////////////////////////////////////////////////////////////////////

template<class T> class LogicalANDKernel : public ChannelAverageKernel<T>
{

public:

	void kernel(DataCubeMap *inputData,
				DataCubeMap *outputData,
				casacore::uInt startInputPos,
				casacore::uInt outputPos,
				casacore::uInt width);
};

//////////////////////////////////////////////////////////////////////////
// ChannelAccumulationKernel class
//   (numerical accumulation, respecting flags)
//////////////////////////////////////////////////////////////////////////

template<class T> class ChannelAccumulationKernel : public ChannelAverageKernel<T>
{

public:

	void kernel(DataCubeMap *inputData,
				DataCubeMap *outputData,
				casacore::uInt startInputPos,
				casacore::uInt outputPos,
				casacore::uInt width);
};

} //# NAMESPACE VI - END

} //# NAMESPACE CASA - END

#endif /* ChannelAverageTVI_H_ */

