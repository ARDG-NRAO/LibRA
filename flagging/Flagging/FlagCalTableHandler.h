//# FlagCalTableHandler.h: This file contains the interface definition of the FlagCalTableHandler class.
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

#ifndef FlagCalTableHandler_H_
#define FlagCalTableHandler_H_

#include <flagging/Flagging/FlagDataHandler.h>

#include <ms/MSSel/MSSelectionTools.h>
#include <msvis/MSVis/StokesVector.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// forward decl of CalTable related classes
class NewCalTable;
class CTInterface;
class CTIter;
class CalBuffer;

namespace vi {

class WeightScaling;

}

#if !defined(__clang__) && defined(__GNUC__) && defined(__GNUC_MINOR__) && \
    (__GNUC__ >= 5 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6))
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#define CASA_WPOP_UNUSED
#endif

class DummyBufferImpl: public vi::VisBuffer2
{
public:
	DummyBufferImpl() {}
	~DummyBufferImpl() {}

	virtual void copy (const VisBuffer2 & /*other*/, casacore::Bool /*fetchIfNeeded*/) {}
    virtual void copyComponents (const VisBuffer2 & /*other*/,
				 const VisBufferComponents2 & /*components*/,
				 casacore::Bool /*allowShapeChange*/ = false,
				 casacore::Bool /*fetchIfNeeded*/ = true) {}
    virtual void copyCoordinateInfo(const VisBuffer2 * /*other*/, casacore::Bool /*includeDirections*/,
   				    casacore::Bool /*allowShapeChange*/ = false,
                    casacore::Bool /*fetchIfNeeded*/ = true) {}

    virtual void setShape (casacore::Int /*nCorrelations*/, casacore::Int /*nChannels*/, casacore::rownr_t /*nRows*/, casacore::Bool) {}
    virtual const vi::VisibilityIterator2 * getVi () const {return NULL;}

    virtual casacore::Bool isAttached () const {return true;}
    virtual casacore::Bool isFillable () const {return true;}

    virtual void writeChangesBack () {}
    virtual void initWeightSpectrum (const casacore::Cube<casacore::Float>&) {}
    virtual void initSigmaSpectrum (const casacore::Cube<casacore::Float>&) {}
    virtual void dirtyComponentsAdd (const VisBufferComponents2 & /*additionalDirtyComponents*/) {}
    virtual void dirtyComponentsAdd (VisBufferComponent2 /*component*/) {}
    virtual void dirtyComponentsClear () {}
    virtual VisBufferComponents2 dirtyComponentsGet () const { VisBufferComponents2 dummy; return dummy;}
    virtual void dirtyComponentsSet (const VisBufferComponents2 & /*dirtyComponents*/) {}
    virtual void dirtyComponentsSet (VisBufferComponent2 /*component*/) {}

    virtual const casacore::Matrix<casacore::Float> & imagingWeight() const {static casacore::Matrix<casacore::Float> dummy; return dummy;}
    void setImagingWeight (const casacore::Matrix<float> &) {}

    virtual casacore::Double getFrequency (casacore::Int /*rowInBuffer*/, casacore::Int /*frequencyIndex*/,
                                 casacore::Int /*frame*/ = FrameNotSpecified) const { static casacore::Double dummy; return dummy;}
    virtual const casacore::Vector<casacore::Double> & getFrequencies (casacore::Int /*rowInBuffer*/,
                                                 casacore::Int /*frame*/ = FrameNotSpecified) const {static casacore::Vector<casacore::Double> dummy; return dummy;}
    virtual casacore::Int getChannelNumber (casacore::Int /*rowInBuffer*/, casacore::Int /*frequencyIndex*/) const {static casacore::Int dummy; return dummy;}
    virtual const casacore::Vector<casacore::Int> & getChannelNumbers (casacore::Int /*rowInBuffer*/) const {static casacore::Vector<casacore::Int> dummy; return dummy;}

    virtual void normalize() {}

    virtual void resetWeightsUsingSigma () {}

    virtual casacore::Bool isNewArrayId () const {static casacore::Bool dummy; return dummy;}
    virtual casacore::Bool isNewFieldId () const {static casacore::Bool dummy; return dummy;}
    virtual casacore::Bool isNewMs() const {static casacore::Bool dummy; return dummy;}
    virtual casacore::Bool isNewSpectralWindow () const {static casacore::Bool dummy; return dummy;}
    virtual casacore::Bool isWritable () const {static casacore::Bool dummy; return dummy;}
    virtual casacore::Int msId() const {static casacore::Int dummy; return dummy;}
    virtual casacore::String msName (casacore::Bool /*stripPath*/ = false) const {casacore::String dummy; return dummy;}
    virtual vi::Subchunk getSubchunk () const {vi::Subchunk dummy; return dummy;}

    virtual const casacore::Vector<casacore::Int> & antenna1 () const {static casacore::Vector<casacore::Int> dummy; return dummy;}
    virtual void setAntenna1 (const casacore::Vector<casacore::Int> & /*value*/) {}
    virtual const casacore::Vector<casacore::Int> & antenna2 () const {static casacore::Vector<casacore::Int> dummy; return dummy;}
    virtual void setAntenna2 (const casacore::Vector<casacore::Int> & /*value*/) {}
    virtual const casacore::Vector<casacore::Int>& arrayId () const {static casacore::Vector<casacore::Int> dummy; return dummy;}
    virtual void setArrayId (const casacore::Vector<casacore::Int>&) {}
    virtual casacore::Int dataDescriptionId () const {static casacore::Int dummy; return dummy;}
    virtual void setDataDescriptionId (casacore::Int /*value*/) {}
    virtual void setDataDescriptionIds(const casacore::Vector<int>&){}
    virtual const casacore::Vector<casacore::MDirection> & direction1 () const {static casacore::Vector<casacore::MDirection> dummy; return dummy;}
    virtual const casacore::Vector<casacore::MDirection> & direction2 () const {static casacore::Vector<casacore::MDirection> dummy; return dummy;}
    virtual const casacore::Vector<casacore::Double> & exposure () const {static casacore::Vector<casacore::Double> dummy; return dummy;}
    virtual void setExposure (const casacore::Vector<casacore::Double> & /*value*/) {}
    virtual const casacore::Vector<casacore::Int> & feed1 () const {static casacore::Vector<casacore::Int> dummy; return dummy;}
    virtual void setFeed1 (const casacore::Vector<casacore::Int> & /*value*/) {}
    virtual const casacore::Vector<casacore::Int> & feed2 () const {static casacore::Vector<casacore::Int> dummy; return dummy;}
    virtual void setFeed2 (const casacore::Vector<casacore::Int> & /*value*/) {}
    virtual const casacore::Vector<casacore::Int>& fieldId () const {static casacore::Vector<casacore::Int> dummy; return dummy;}
    virtual void setFieldId (const casacore::Vector<casacore::Int>&) {}
    virtual const casacore::Matrix<casacore::Bool> & flag () const {static casacore::Matrix<casacore::Bool> dummy; return dummy;}
    virtual void setFlag (const casacore::Matrix<casacore::Bool>& /*value*/) {}
    virtual const casacore::Array<casacore::Bool> & flagCategory () const {static casacore::Array<casacore::Bool> dummy; return dummy;}
    virtual void setFlagCategory (const casacore::Array<casacore::Bool>& /*value*/) {}
    virtual const casacore::Vector<casacore::Array<casacore::Bool>> & flagCategories () const {static casacore::Vector<casacore::Array<casacore::Bool>> dummy; return dummy;}
    virtual void setFlagCategories (const casacore::Vector<casacore::Array<casacore::Bool>>& /*value*/) {}
    virtual const casacore::Cube<casacore::Bool> & flagCube () const {static casacore::Cube<casacore::Bool> dummy; return dummy;}
    virtual void setFlagCube (const casacore::Cube<casacore::Bool>& /*value*/) {}
    virtual const casacore::Vector<casacore::Cube<casacore::Bool>> & flagCubes () const {static casacore::Vector<casacore::Cube<casacore::Bool>> dummy; return dummy;}
    virtual void setFlagCubes (const casacore::Vector<casacore::Cube<casacore::Bool>>& /*value*/) {}
    virtual const casacore::Vector<casacore::Bool> & flagRow () const {static casacore::Vector<casacore::Bool> dummy; return dummy;}
    virtual void setFlagRow (const casacore::Vector<casacore::Bool>& /*value*/) {}
    virtual const casacore::Vector<casacore::Int> & observationId () const {static casacore::Vector<casacore::Int> dummy; return dummy;}
    virtual void setObservationId (const casacore::Vector<casacore::Int> & /*value*/) {}
    virtual const casacore::Vector<casacore::Int> & processorId () const {static casacore::Vector<casacore::Int> dummy; return dummy;}
    virtual void setProcessorId (const casacore::Vector<casacore::Int> & /*value*/) {}
    virtual const casacore::Vector<casacore::Int> & scan () const {static casacore::Vector<casacore::Int> dummy; return dummy;}
    virtual void setScan (const casacore::Vector<casacore::Int> & /*value*/) {}
    virtual const casacore::Matrix<casacore::Float> & sigma () const {static casacore::Matrix<casacore::Float> dummy; return dummy;}
    virtual void setSigma (const casacore::Matrix<casacore::Float> & /*value*/) {}
    virtual const casacore::Vector<casacore::Matrix<casacore::Float>> & sigmas () const {static casacore::Vector<casacore::Matrix<casacore::Float>> dummy; return dummy;}
    virtual void setSigmas (const casacore::Vector<casacore::Matrix<casacore::Float>> & /*value*/) {}
    virtual const casacore::Matrix<casacore::Float> & sigmaMat () const {static casacore::Matrix<casacore::Float> dummy; return dummy;}
    virtual const casacore::Vector<casacore::Int> & stateId () const {static casacore::Vector<casacore::Int> dummy; return dummy;}
    virtual void setStateId (const casacore::Vector<casacore::Int> & /*value*/) {}
    virtual const casacore::Vector<casacore::Double> & time () const {static casacore::Vector<casacore::Double> dummy; return dummy;}
    virtual void setTime (const casacore::Vector<casacore::Double> & /*value*/) {}
    virtual const casacore::Vector<casacore::Double> & timeCentroid () const {static casacore::Vector<casacore::Double> dummy; return dummy;}
    virtual void setTimeCentroid (const casacore::Vector<casacore::Double> & /*value*/) {}
    virtual const casacore::Vector<casacore::Double> & timeInterval () const {static casacore::Vector<casacore::Double> dummy; return dummy;}
    virtual void setTimeInterval (const casacore::Vector<casacore::Double> & /*value*/) {}
    virtual const casacore::Matrix<casacore::Double> & uvw () const {static casacore::Matrix<casacore::Double> dummy; return dummy;}
    virtual void setUvw (const casacore::Matrix<casacore::Double> & /*value*/) {}
    virtual const casacore::Matrix<casacore::Float> & weight () const {static casacore::Matrix<casacore::Float> dummy; return dummy;}
    virtual void setWeight (const casacore::Matrix<casacore::Float>& /*value*/) {}
    virtual const casacore::Vector<casacore::Matrix<casacore::Float>> & weights () const {static casacore::Vector<casacore::Matrix<casacore::Float>> dummy; return dummy;}
    virtual void setWeights (const casacore::Vector<casacore::Matrix<casacore::Float>>& /*value*/) {}
    virtual const casacore::Matrix<casacore::Float> & weightMat () const {static casacore::Matrix<casacore::Float> dummy; return dummy;}
    virtual void setWeightMat (const casacore::Matrix<casacore::Float>& /*value*/) {}
    virtual const casacore::Cube<casacore::Float> & weightSpectrum () const {static casacore::Cube<casacore::Float> dummy; return dummy;}
    virtual void setWeightSpectrum (const casacore::Cube<casacore::Float>& /*value*/) {}
    virtual const casacore::Vector<casacore::Cube<casacore::Float>> & weightSpectra () const {static casacore::Vector<casacore::Cube<casacore::Float>> dummy; return dummy;}
    virtual void setWeightSpectra (const casacore::Vector<casacore::Cube<casacore::Float>>& /*value*/) {}
    virtual const casacore::Cube<float>& sigmaSpectrum() const {static casacore::Cube<casacore::Float> dummy; return dummy;}
    virtual void setSigmaSpectrum(const casacore::Cube<float>&) {}
    virtual const casacore::Vector<casacore::Cube<float>>& sigmaSpectra() const {static casacore::Vector<casacore::Cube<casacore::Float>> dummy; return dummy;}
    virtual void setSigmaSpectra (const casacore::Vector<casacore::Cube<float>>&) {}

    virtual const casacore::Cube<casacore::Complex> & visCube () const {static casacore::Cube<casacore::Complex> dummy; return dummy;}
    virtual void setVisCube(const casacore::Complex & /*c*/) {}
    virtual void setVisCube (const casacore::Cube<casacore::Complex> &){}
    virtual const casacore::Vector<casacore::Cube<casacore::Complex>> & visCubes () const {static casacore::Vector<casacore::Cube<casacore::Complex>> dummy; return dummy;}
    virtual void setVisCubes (const casacore::Vector<casacore::Cube<casacore::Complex>> &){}
    virtual const casacore::Matrix<CStokesVector> & vis () const {static casacore::Matrix<CStokesVector> dummy; return dummy;}
    virtual void setVis (casacore::Matrix<CStokesVector> &) {}
    virtual const casacore::Cube<casacore::Complex> & visCubeCorrected () const {static casacore::Cube<casacore::Complex> dummy; return dummy;}
    virtual void setVisCubeCorrected (const casacore::Cube<casacore::Complex> &) {}
    virtual const casacore::Vector<casacore::Cube<casacore::Complex>> & visCubesCorrected () const {static casacore::Vector<casacore::Cube<casacore::Complex>> dummy; return dummy;}
    virtual void setVisCubesCorrected (const casacore::Vector<casacore::Cube<casacore::Complex>> &) {}
    virtual const casacore::Matrix<CStokesVector> & visCorrected () const {static casacore::Matrix<CStokesVector> dummy; return dummy;}
    virtual void setVisCorrected (const casacore::Matrix<CStokesVector> &) {}
    virtual const casacore::Cube<casacore::Float> & visCubeFloat () const {static casacore::Cube<casacore::Float> dummy; return dummy;}
    virtual void setVisCubeFloat (const casacore::Cube<casacore::Float> &) {}
    virtual const casacore::Vector<casacore::Cube<casacore::Float>> & visCubesFloat () const {static casacore::Vector<casacore::Cube<casacore::Float>> dummy; return dummy;}
    virtual void setVisCubesFloat (const casacore::Vector<casacore::Cube<casacore::Float>> &) {}
    virtual const casacore::Cube<casacore::Complex> & visCubeModel () const {static casacore::Cube<casacore::Complex> dummy; return dummy;}
    virtual void setVisCubeModel(const casacore::Complex & /*c*/) {}
    virtual void setVisCubeModel(const casacore::Cube<casacore::Complex>& /*vis*/) {}
    virtual void setVisCubeModel(const casacore::Vector<casacore::Float>& /*stokes*/) {}
    virtual const casacore::Vector<casacore::Cube<casacore::Complex>> & visCubesModel () const {static casacore::Vector<casacore::Cube<casacore::Complex>> dummy; return dummy;}
    virtual void setVisCubesModel(const casacore::Complex & /*c*/) {}
    virtual void setVisCubesModel(const casacore::Vector<casacore::Cube<casacore::Complex>>& /*vis*/) {}
    virtual const casacore::Matrix<CStokesVector> & visModel () const {static casacore::Matrix<CStokesVector> dummy; return dummy;}
    virtual void setVisModel (casacore::Matrix<CStokesVector> &) {}

	virtual casacore::MDirection azel0 (casacore::Double /*time*/) const {static casacore::MDirection dummy; return dummy;}
	virtual casacore::Vector<casacore::MDirection> & azel(casacore::Double /*time*/) const {static casacore::Vector<casacore::MDirection> dummy; return dummy;}
	virtual const casacore::Vector<casacore::SquareMatrix<casacore::Complex, 2> > & cjones () const {static casacore::Vector<casacore::SquareMatrix<casacore::Complex, 2> > dummy; return dummy;}
	virtual const casacore::Vector<casacore::Int> & correlationTypes () const {static casacore::Vector<casacore::Int> dummy; return dummy;}
	virtual casacore::Vector<casacore::Float> & feedPa(casacore::Double /*time*/) const {static casacore::Vector<casacore::Float> dummy; return dummy;}
	virtual const casacore::Vector<casacore::Float> & feedPa1 () const {static casacore::Vector<casacore::Float> dummy; return dummy;}
	virtual const casacore::Vector<casacore::Float> & feedPa2 () const {static casacore::Vector<casacore::Float> dummy; return dummy;}
	virtual casacore::Double hourang(casacore::Double /*time*/) const {static casacore::Double dummy; return dummy;}
	virtual casacore::Int nAntennas () const {static casacore::Int dummy; return dummy;}
	virtual casacore::Int nChannels () const {static casacore::Int dummy; return dummy;}
	virtual casacore::Int nCorrelations () const {static casacore::Int dummy; return dummy;}
    virtual casacore::rownr_t nRows () const {static casacore::Int dummy; return dummy;}
    virtual casacore::rownr_t nShapes () const {return 1;}
    virtual const casacore::Vector<casacore::rownr_t>& nRowsPerShape () const {static casacore::Vector<casacore::rownr_t> dummy ; return dummy;}
    virtual const casacore::Vector<casacore::Int>& nChannelsPerShape () const {static casacore::Vector<casacore::Int> dummy ; return dummy;}
    virtual const casacore::Vector<casacore::Int>& nCorrelationsPerShape () const {static casacore::Vector<casacore::Int> dummy ; return dummy;}
	virtual casacore::Float parang0(casacore::Double /*time*/) const {static casacore::Float dummy; return dummy;}
	virtual casacore::Vector<casacore::Float> & parang(casacore::Double /*time*/) const {static casacore::Vector<casacore::Float> dummy; return dummy;}
	virtual const casacore::MDirection& phaseCenter () const {static casacore::MDirection dummy; return dummy;}
	virtual casacore::Int polarizationFrame () const {static casacore::Int dummy; return dummy;}
	virtual casacore::Int polarizationId () const {static casacore::Int dummy; return dummy;}
	virtual const casacore::Vector<casacore::rownr_t> & rowIds () const {static casacore::Vector<casacore::rownr_t> dummy; return dummy;}
	virtual casacore::Int spectralWindow () const {static casacore::Int dummy; return dummy;}
	virtual const casacore::Vector<casacore::Int> & spectralWindows () const {static casacore::Vector<casacore::Int> dummy; return dummy;}
        virtual void setSpectralWindows (const casacore::Vector<casacore::Int> & /*spectralWindows*/) {}
        virtual casacore::Bool modelDataIsVirtual () const { return false;}


    virtual void setFillable (casacore::Bool /*isFillable*/) {}
    virtual const casacore::Vector<casacore::Int> & dataDescriptionIds () const {static casacore::Vector<casacore::Int> dummy; return dummy;}
    virtual casacore::Vector<casacore::Int> getCorrelationTypes () const {static casacore::Vector<casacore::Int> dummy; return dummy;}
    virtual void validateShapes () const {}

    virtual casacore::Float getWeightScaled (casacore::Int /*row*/) const { return 0;}
    virtual casacore::Float getWeightScaled (casacore::Int /*correlation*/, casacore::Int /*row*/) const { return 0;}
    virtual casacore::Float getWeightScaled (casacore::Int /*correlation*/, casacore::Int /*channel*/, casacore::Int /*row*/) const { return 0;}
    virtual casacore::Float getSigmaScaled (casacore::Int /* row */) const { return 0;}
    virtual casacore::Float getSigmaScaled (casacore::Int /* correlation */, casacore::Int /* row */) const { return 0;}
    virtual casacore::Float getSigmaScaled (casacore::Int /* correlation */, casacore::Int /* channel */, casacore::Int /* row */) const { return 0;}
    virtual casacore::CountedPtr<vi::WeightScaling> getWeightScaling () const { return 0; }

    virtual void phaseCenterShift(const casacore::Vector<casacore::Double>& /* phase */) {};
    virtual void phaseCenterShift(casacore::Double /* dx */, casacore::Double /* dy */) {};


protected:

    virtual void configureNewSubchunk (casacore::Int /*msId*/, const casacore::String & /*msName*/, casacore::Bool /*isNewMs*/,
                                       casacore::Bool /*isNewArrayId*/, casacore::Bool /*isNewFieldId*/,
                                       casacore::Bool /*isNewSpectralWindow*/, const vi::Subchunk & /*subchunk*/,
                                       const casacore::Vector<casacore::rownr_t>& nRowsPerShape,
                                       const casacore::Vector<casacore::Int>& nChannelsPerShape,
                                       const casacore::Vector<casacore::Int>& nCorrelationsPerShape,
                                       const casacore::Vector<casacore::Int> & /*correlations*/,
                                       const casacore::Vector<casacore::Stokes::StokesTypes> &,
                                       const casacore::Vector<casacore::Stokes::StokesTypes> &,
                                       casacore::CountedPtr <vi::WeightScaling> /*weightScaling*/) {}
    virtual void invalidate() {}
    virtual casacore::Bool isRekeyable () const {static casacore::Bool dummy; return dummy;}
    virtual void setRekeyable (casacore::Bool /*isRekeable*/) {}

};

#ifdef CASA_WPOP
#pragma GCC diagnostic pop
#define CASA_WPOP
#endif


class CTCache
{

public:
	CTCache(CTIter *calIter);
	~CTCache();

	casacore::Int arrayId();
	casacore::Int fieldId();
	casacore::Int spectralWindow();
	casacore::Vector<casacore::Int>& scan();
	casacore::Vector<casacore::Double>& time();
	casacore::Vector<casacore::Int>& antenna1();
	casacore::Vector<casacore::Int>& antenna2();
	casacore::Cube<casacore::Bool>& flagCube();
	casacore::Vector<casacore::Int>& getChannelNumbers(casacore::Int rowInBuffer);
	casacore::Vector<casacore::Double>& getFrequencies(casacore::Int rowInBuffer,casacore::Int frame = vi::VisBuffer2::FrameNotSpecified);
	casacore::Int nRowChunk();
	casacore::Int nRows();
	casacore::Int nChannels();
	casacore::Int nCorrelations();

	casacore::Cube<casacore::Complex>& visCube();
	casacore::Cube<casacore::Complex>& visCubeCorrected();
	casacore::Cube<casacore::Complex>& visCubeModel();

	// Convenient public methods for compatibility with casacore::MS-like interface
	casacore::Vector<casacore::Int>& observationId();
	casacore::Vector<casacore::Int>& correlationTypes();

	// Methods for efficient synchronization with CTIter
	void invalidate();

private:

	CTIter *calIter_p;
	casacore::Int field0_p;
	casacore::Int spw0_p;
	casacore::Vector<casacore::Int> field_p;
	casacore::Vector<casacore::Int> spw_p;
	casacore::Vector<casacore::Int> scan_p;
	casacore::Vector<casacore::Double> time_p;
	casacore::Vector<casacore::Int> antenna1_p;
	casacore::Vector<casacore::Int> antenna2_p;
	casacore::Cube<casacore::Bool> flagCube_p;
	casacore::Vector<casacore::Int> channel_p;
	casacore::Vector<casacore::Double> frequency_p;
	casacore::Cube<casacore::Complex> cparam_p;	  // This is an actual casacore::Cube<casacore::Complex>
	casacore::Cube<casacore::Complex> fparam_p;   // This is in reality a casacore::Cube<casacore::Float>
	casacore::Cube<casacore::Complex> snr_p;      // This is in reality a casacore::Cube<casacore::Float>
	casacore::Int nRowChunk_p;
	casacore::Int nRow_p;
	casacore::Int nChannel_p;
	casacore::Int nCorr_p;

	// Convenient members for compatibility with casacore::MS-like interface
	casacore::Vector<casacore::Int> observationId_p;
	casacore::Vector<casacore::Int> corrType_p;

	// Members for efficient synchronization with CTIter
	casacore::Bool CTfieldIdOK_p;
	casacore::Bool CTspectralWindowOK_p;
	casacore::Bool CTscanOK_p;
	casacore::Bool CTtimeOK_p;
	casacore::Bool CTantenna1OK_p;
	casacore::Bool CTantenna2OK_p;
	casacore::Bool CTflagCubeOk_p;
	casacore::Bool CTobservationIdOK_p;
	casacore::Bool CTcorrTypeOK_p;
	casacore::Bool CTchannelOK_p;
	casacore::Bool CTfrequencyOK_p;
	casacore::Bool CTVisCubeOK_p;
	casacore::Bool CTcorrectedVisCubeOK_p;
	casacore::Bool CTmodelVisCubeOK_p;
	casacore::Bool CTnRowChunkOK_p;
	casacore::Bool CTnRowOK_p;
	casacore::Bool CTnChannelOK_p;
	casacore::Bool CTnCorrOK_p;

	// Convenience methods and members to by-pass const methods issues
	casacore::Int& fillSpectralWindow();
	casacore::Int& fillFieldId();
	casacore::Int& fillnRowChunk();

};


class CTBuffer: public DummyBufferImpl
{

public:

	CTBuffer(CTIter *ctIter) {ctCache_p = new CTCache(ctIter);invalidate();}
	~CTBuffer() {}

	virtual casacore::Bool existsColumn (VisBufferComponent2 id) const {
		return id == VisBufferComponent2::ArrayId
			|| id == VisBufferComponent2::FieldId
			|| id == VisBufferComponent2::SpectralWindows
			|| id == VisBufferComponent2::Scan
			|| id == VisBufferComponent2::Time
			|| id == VisBufferComponent2::Antenna1
			|| id == VisBufferComponent2::Antenna2
			|| id == VisBufferComponent2::FlagCube
			|| id == VisBufferComponent2::VisibilityCubeObserved
			|| id == VisBufferComponent2::VisibilityCubeCorrected
			|| id == VisBufferComponent2::VisibilityCubeModel
			|| id == VisBufferComponent2::ObservationId
			|| id == VisBufferComponent2::CorrType;
	}

	const casacore::Vector<casacore::Int> & arrayId() const {arrayId_p.assign (casacore::Vector<casacore::Int> (time().nelements(), ctCache_p->arrayId())); return arrayId_p;}
	const casacore::Vector<casacore::Int> & fieldId() const {fieldId_p.assign (casacore::Vector<casacore::Int> (time().nelements(), ctCache_p->fieldId())); return fieldId_p;}
	casacore::Int spectralWindow() const {return ctCache_p->spectralWindow();}
	const casacore::Vector<casacore::Int>& scan() const {return ctCache_p->scan();}
	const casacore::Vector<casacore::Double>& time() const {return ctCache_p->time();}
	const casacore::Vector<casacore::Int>& antenna1() const {return ctCache_p->antenna1();}
	const casacore::Vector<casacore::Int>& antenna2() const {return ctCache_p->antenna2();}
	const casacore::Cube<casacore::Bool>& flagCube() const {return ctCache_p->flagCube();}
	const casacore::Vector<casacore::Int>& getChannelNumbers(casacore::Int rowInBuffer) const {return ctCache_p->getChannelNumbers(rowInBuffer);}
	casacore::Vector<casacore::Int> getChannelNumbersSelected (casacore::Int /* outputChannelIndex */) const { throw casacore::AipsError ("Not Implemented");}
	const casacore::Vector<casacore::Double>& getFrequencies(casacore::Int rowInBuffer,casacore::Int /*frame*/ = vi::VisBuffer2::FrameNotSpecified) const {return ctCache_p->getFrequencies(rowInBuffer);}
	casacore::rownr_t nRows() const {return ctCache_p->nRows();}
	casacore::Int nChannels() const {return ctCache_p->nChannels();}
	casacore::Int nCorrelations() const {return ctCache_p->nCorrelations();}
	const casacore::Vector<casacore::Int> &  spectralWindows () const {
	    spectralWindows_p.assign (casacore::Vector<casacore::Int> (time().nelements(), ctCache_p->spectralWindow())); return spectralWindows_p;}


	const casacore::Cube<casacore::Complex>& visCube() const {return ctCache_p->visCube();}
	const casacore::Cube<casacore::Complex>& visCubeCorrected() const {return ctCache_p->visCubeCorrected();}
	const casacore::Cube<casacore::Complex>& visCubeModel() const {return ctCache_p->visCubeModel();}

	// Convenient public methods for compatibility with casacore::MS-like interface
	const casacore::Vector<casacore::Int>& observationId() const {return ctCache_p->observationId();}
	const casacore::Vector<casacore::Int>& correlationTypes() const {return ctCache_p->correlationTypes();}
	casacore::Vector<casacore::Stokes::StokesTypes> getCorrelationTypesDefined () const { throw casacore::AipsError ("Not Implemented");}
	casacore::Vector<casacore::Stokes::StokesTypes> getCorrelationTypesSelected () const { throw casacore::AipsError ("Not Implemented");}

	// Methods for efficient synchronization with CTIter
	void invalidate() {ctCache_p->invalidate();}

    casacore::Vector<bool>& flagRowRef() {throw casacore::AipsError ("Not implemented " , __FILE__, __LINE__);}
    casacore::Cube<bool>& flagCubeRef() {throw casacore::AipsError ("Not implemented " , __FILE__, __LINE__);}
    casacore::Vector<casacore::Cube<bool>>& flagCubesRef() {throw casacore::AipsError ("Not implemented " , __FILE__, __LINE__);}
    casacore::Cube<std::complex<float> >& visCubeRef() {throw casacore::AipsError ("Not implemented " , __FILE__, __LINE__);}
    casacore::Vector<casacore::Cube<std::complex<float>>>& visCubesRef() {throw casacore::AipsError ("Not implemented " , __FILE__, __LINE__);}
    casacore::Cube<std::complex<float> >& visCubeCorrectedRef() {throw casacore::AipsError ("Not implemented " , __FILE__, __LINE__);}
    casacore::Vector<casacore::Cube<std::complex<float>>>& visCubesCorrectedRef() {throw casacore::AipsError ("Not implemented " , __FILE__, __LINE__);}
    casacore::Cube<std::complex<float> >& visCubeModelRef() {throw casacore::AipsError ("Not implemented " , __FILE__, __LINE__);}
    casacore::Vector<casacore::Cube<std::complex<float>>>& visCubesModelRef() {throw casacore::AipsError ("Not implemented " , __FILE__, __LINE__);}
    casacore::Cube<float >& sigmaSpectrumRef() {throw casacore::AipsError ("Not implemented " , __FILE__, __LINE__);}
    casacore::Vector<casacore::Cube<float>>& sigmaSpectraRef() {throw casacore::AipsError ("Not implemented " , __FILE__, __LINE__);}
    casacore::Cube<float >& weightSpectrumRef() {throw casacore::AipsError ("Not implemented " , __FILE__, __LINE__);}
    casacore::Vector<casacore::Cube<float>>& weightSpectraRef() {throw casacore::AipsError ("Not implemented " , __FILE__, __LINE__);}
    casacore::IPosition getShape () const { return casacore::IPosition ();}

private:

        mutable casacore::Vector<casacore::Int> arrayId_p;
        mutable casacore::Vector<casacore::Int> fieldId_p;
        mutable casacore::Vector<casacore::Int> spectralWindows_p;
    // CTIter *calIter_p;
	CTCache *ctCache_p;

};

// Flag casacore::Data Handler class definition
class FlagCalTableHandler: public FlagDataHandler
{

public:

	// Default constructor
	// NOTE: casacore::Time interval 0 groups all time steps together in one chunk.
	FlagCalTableHandler(string msname, casacore::uShort iterationApproach = SUB_INTEGRATION, casacore::Double timeInterval = 0);

	// Default destructor
	~FlagCalTableHandler();

	// Open CalTable
	bool open();

	// Close CalTable
	bool close();

	// Generate selected CalTable
	bool selectData();

	// Parse casacore::MSSelection expression
	bool parseExpression(casacore::MSSelection &parser);

	// Generate CalIter
	bool generateIterator();

	// Move to next chunk
	bool nextChunk();

	// Move to next buffer
	bool nextBuffer();

	// Write flag cube into MS
	bool flushFlags();

	// Provide table name (for flag version)
	casacore::String getTableName();

	// Check if a given column is available in the cal table
	bool checkIfColumnExists(casacore::String column);

	// Signal true when a progress summary has to be printed
	bool summarySignal();

private:

	// Mapping functions
	void generateScanStartStopMap();

	// Convenient private methods for compatibility with casacore::MS-like interface
	casacore::Block<casacore::String> getSortColumns(casacore::Block<casacore::Int> intCols);

	// CatTable selection objects
	NewCalTable *selectedCalTable_p;
	NewCalTable *originalCalTable_p;
	CTInterface *calTableInterface_p;

	// Calibration casacore::Table iterator and buffer
	CTIter *calIter_p;
	CTBuffer *calBuffer_p;

};

} //# NAMESPACE CASA - END

#endif /* FlagCalTableHandler_H_ */
