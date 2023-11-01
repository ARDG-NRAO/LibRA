//# MSTransformBufferImpl.h: This file contains the interface definition of the MSTransformBufferImpl.h class.
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

#ifndef MSTransformBufferImpl_H_
#define MSTransformBufferImpl_H_

// Where VisBufferImpl2 interface is defined
#include <msvis/MSVis/VisBufferImpl2.h>

// Class containing the actual transformation logic
#include <mstransform/MSTransform/MSTransformManager.h>

namespace casa {

class DataCubeHolderBase
{

public:

	DataCubeHolderBase() {}
	virtual ~DataCubeHolderBase() {}
	virtual void setMatrixIndex(casacore::uInt matrixIndex) = 0;
	virtual void setVectorIndex(casacore::uInt vectorIndex) = 0;
	casacore::uInt getMatrixIndex() {return matrixIndex_p;}
	casacore::uInt getVectorIndex() {return vectorIndex_p;}
	casacore::IPosition & getMatrixShape() {return matrixShape_p;}
	casacore::IPosition & getVectorShape() {return vectorShape_p;}

protected:

	casacore::uInt matrixIndex_p;
	casacore::uInt vectorIndex_p;
	casacore::IPosition matrixShape_p;
	casacore::IPosition vectorShape_p;
};

template <class T> class DataCubeHolder : public DataCubeHolderBase
{

public:

	DataCubeHolder(casacore::Cube<T> &dataCube) {cube_p.reference(dataCube);}
	~DataCubeHolder() {}

	casacore::Matrix<T> & getMatrix() {return matrix_p;}
	casacore::Vector<T> & getVector() {return vector_p;}

	void setMatrixIndex(casacore::uInt matrixIndex)
	{
		matrix_p.resize(); // Resize to 0 to avoid shape conformance problems
		matrixIndex_p = matrixIndex;
		matrix_p.reference(cube_p.xyPlane(matrixIndex));
		matrixShape_p = matrix_p.shape();
	}

	void setVectorIndex(casacore::uInt vectorIndex)
	{
		vector_p.resize(); // Resize to 0 to avoid shape conformance problems
		vectorIndex_p = vectorIndex;
		vector_p.reference(matrix_p.row(vectorIndex));
		vectorShape_p = vector_p.shape();
	}

protected:

	casacore::Cube<T> cube_p;
	casacore::Matrix<T> matrix_p;
	casacore::Vector<T> vector_p;
};

class DataCubeMap
{

public:

	DataCubeMap() {dataCubeMap_p.clear();}
	~DataCubeMap() {dataCubeMap_p.clear();}

	void add(casacore::MS::PredefinedColumns key,DataCubeHolderBase* dataCubeHolder){dataCubeMap_p[key] = dataCubeHolder;}

	void setWindowShape(casacore::IPosition windowShape) {windowShape_p = windowShape;}
	casacore::IPosition & getWindowShape() {return windowShape_p;}

	template <class T> casacore::Vector<T> & getVector(casacore::MS::PredefinedColumns key)
	{
		DataCubeHolder<T> *flagCubeHolder = static_cast< DataCubeHolder<T>* >(dataCubeMap_p[key]);
		return flagCubeHolder->getVector();
	}

	template <class T> casacore::Matrix<T> & getMatrix(casacore::MS::PredefinedColumns key)
	{
		DataCubeHolder<T> *flagCubeHolder = static_cast< DataCubeHolder<T>* >(dataCubeMap_p[key]);
		return flagCubeHolder->getVector();
	}

	void setMatrixIndex(casacore::uInt rowIndex)
	{
		for (dataCubeMapIter_p = dataCubeMap_p.begin();dataCubeMapIter_p!= dataCubeMap_p.end();dataCubeMapIter_p++)
		{
			dataCubeMapIter_p->second->setMatrixIndex(rowIndex);
		}
	}

	void setVectorIndex(casacore::uInt vectorIndex)
	{
		for (dataCubeMapIter_p = dataCubeMap_p.begin();dataCubeMapIter_p!= dataCubeMap_p.end();dataCubeMapIter_p++)
		{
			dataCubeMapIter_p->second->setVectorIndex(vectorIndex);
		}
	}

	casacore::IPosition & getMatrixShape()
	{
		return dataCubeMap_p.begin()->second->getMatrixShape();
	}

	casacore::IPosition & getVectorShape()
	{
		return dataCubeMap_p.begin()->second->getVectorShape();
	}


protected:

	casacore::IPosition windowShape_p;
	std::map<casacore::MS::PredefinedColumns, DataCubeHolderBase*> dataCubeMap_p;
	std::map<casacore::MS::PredefinedColumns, DataCubeHolderBase*>::iterator dataCubeMapIter_p;
};

typedef void (casa::MSTransformBufferImpl::*TransformFunction)(	vi::VisBuffer2 *vb,
																DataCubeMap &inputDataMap,
																DataCubeMap &outputDataMap) const;

typedef void (casa::MSTransformBufferImpl::*TransformKernel)(	vi::VisBuffer2 *vb,
																DataCubeMap &inputDataMap,
																DataCubeMap &outputDataMap,
																casacore::IPosition &inputPos,
																casacore::IPosition &outputPos,
																casacore::IPosition &kernelShape) const;

typedef void (casa::MSTransformBufferImpl::*TransformKernel1D)(	vi::VisBuffer2 *vb,
																DataCubeMap &inputDataMap,
																DataCubeMap &outputDataMap,
																casacore::uInt &inputPos,
																casacore::uInt &outputPos,
																casacore::uInt &kernelSize) const;

class MSTransformBufferImpl : public vi::VisBufferImpl2
{

public:

	MSTransformBufferImpl(MSTransformManager *manager);
	~MSTransformBufferImpl() {};

	void resetState();
	void setRowIdOffset(casacore::uInt rowOffset) {rowIdOffset_p = rowOffset;}
	void shiftRowIdOffset(casacore::Int nRows) {rowIdOffset_p += nRows;}

	void generateWeights() const;

	// Re-indexable Vectors
    const casacore::Vector<casacore::Int> & dataDescriptionIds () const; // [nR]
    const casacore::Vector<casacore::Int> & spectralWindows () const; // [nR]
    const casacore::Vector<casacore::Int> & observationId () const; // [nR]
    const casacore::Vector<casacore::Int> & arrayId () const; // [nR]
    const casacore::Vector<casacore::Int> & fieldId () const; // [nR]
    const casacore::Vector<casacore::Int> & stateId () const; // [nR]
    const casacore::Vector<casacore::Int> & antenna1 () const; // [nR]
    const casacore::Vector<casacore::Int> & antenna2 () const; // [nR]

	// Not-Re-indexable Vectors
    const casacore::Vector<casacore::Int> & scan () const; // [nR]
    const casacore::Vector<casacore::Int> & processorId () const; // [nR]
    const casacore::Vector<casacore::Int> & feed1 () const; // [nR]
    const casacore::Vector<casacore::Int> & feed2 () const; // [nR]
    const casacore::Vector<casacore::Double> & time () const; // [nR]
    const casacore::Vector<casacore::Double> & timeCentroid () const; // [nR]
    const casacore::Vector<casacore::Double> & timeInterval () const; // [nR]

    // Average-able vectors
    const casacore::Vector<casacore::Double> & exposure () const; // [nR]
    const casacore::Vector<casacore::Bool> & flagRow () const; // [nR]

    const casacore::Matrix<casacore::Double> & uvw () const; // [3,nR]
    const casacore::Matrix<casacore::Float> & weight () const; // [nC, nR]
    const casacore::Matrix<casacore::Float> & sigma () const; // [nC, nR]
    const casacore::Cube<casacore::Bool> & flagCube () const; // [nC,nF,nR]
    const casacore::Cube<casacore::Complex> & visCube () const; // [nC,nF,nR]
    const casacore::Cube<casacore::Complex> & visCubeCorrected () const; // [nC,nF,nR]
    const casacore::Cube<casacore::Complex> & visCubeModel () const; // [nC,nF,nR]
    const casacore::Cube<casacore::Float> & visCubeFloat () const; // [nC,nF,nR]
    const casacore::Cube<casacore::Float> & weightSpectrum () const; // [nC,nF,nR]
    const casacore::Cube<casacore::Float> & sigmaSpectrum () const; // [nC,nF,nR]
    const casacore::Array<casacore::Bool> & flagCategory () const; // [nC,nF,nCategories,nR]

	casacore::IPosition getShape () const;
	casacore::rownr_t nRows () const;
	casacore::Int nChannels () const;
	casacore::Int nCorrelations () const;
	casacore::Int nAntennas () const;

	// For plotms
    const casacore::Vector<casacore::Float> & feedPa (casacore::Double time) const; // [nA]
    casacore::Float parang0(casacore::Double time) const;
    const casacore::Vector<casacore::Float> & parang(casacore::Double time) const; // [nA]
    casacore::MDirection azel0(casacore::Double time) const;
    const casacore::Vector<casacore::MDirection> & azel(casacore::Double time) const; // [nA]
    casacore::Double hourang(casacore::Double time) const;
    const casacore::Vector<casacore::MDirection> & direction1() const; // ??

    casacore::Vector<casacore::Int> getCorrelationTypes () const;
    const casacore::Vector<casacore::Int> & correlationTypes () const;
    casacore::Vector<casacore::Stokes::StokesTypes> getCorrelationTypesDefined () const;
    casacore::Vector<casacore::Stokes::StokesTypes> getCorrelationTypesSelected () const;

    casacore::Double getFrequency (casacore::Int rowInBuffer, casacore::Int frequencyIndex, casacore::Int frame = FrameNotSpecified) const;
    const casacore::Vector<casacore::Double> & getFrequencies (casacore::Int rowInBuffer,casacore::Int frame = FrameNotSpecified) const;
    casacore::Int getChannelNumber (casacore::Int rowInBuffer, casacore::Int frequencyIndex) const;
    const casacore::Vector<casacore::Int> & getChannelNumbers (casacore::Int rowInBuffer) const;
    casacore::Vector<casacore::Int> getChannelNumbersSelected (casacore::Int outputChannelIndex) const;

    const casacore::Vector<casacore::rownr_t> & rowIds () const;

    // Rotate visibility phase for given vector (dim = nrow of vb) of phases (metres)
    void phaseCenterShift(const casacore::Vector<casacore::Double>& phase);
    // Rotate visibility phase for phase center offsets (arcsecs)
    void phaseCenterShift(casacore::Double dx, casacore::Double dy);

    const casacore::MDirection& phaseCenter () const;
    const casacore::MFrequency::Types & freqRefFrameType () const;

protected:

    casacore::MFrequency::Convert generateFreqRefTranEngine (casacore::Double time,casacore::Int outputRefFrame,casacore::Bool toObservedFrame) const;

    void transformDataCube(	vi::VisBuffer2 *vb,
    						DataCubeMap &inputDataCubeMap,
    						DataCubeMap &outputDataCubeMap,
    						TransformFunction funcPointer) const;

    void channelAverage(	vi::VisBuffer2 *vb,
    						DataCubeMap &inputDataCubeMap,
    						DataCubeMap &outputDataCubeMap) const;

    void decimationWindow(	vi::VisBuffer2 *vb,
    						DataCubeMap &inputDataCubeMap,
    						DataCubeMap &outputDataCubeMap,
    						TransformKernel1D kernelPointer) const;

    void flagAverageKernel(	vi::VisBuffer2 *vb,
    						DataCubeMap &inputDataCubeMap,
    						DataCubeMap &outputDataCubeMap,
    						casacore::uInt &inputPos,
    						casacore::uInt &outputPos,
    						casacore::uInt &kernelSize) const;

private:

	MSTransformManager *manager_p;
	casacore::ArrayColumn<casacore::Double> spwFrequencies_p;
	map<casacore::uInt,casacore::uInt> inputOutputSPWIndexMap_p;
	casacore::uInt rowIdOffset_p;

	// OTF frequency transformation
	casacore::MDirection phaseCenter_p;
	casacore::MPosition observatoryPosition_p;
	casacore::ArrayMeasColumn<casacore::MFrequency> spwRefRame_p;

	// Phase shifting
	casacore::Bool applyPhaseShifting_p;
	casacore::Double dx_p, dy_p;

	// NONE datacol handling
	casacore::Bool noneDataCol_p;

	mutable casacore::Vector<casacore::Int> observationId_p;
	mutable casacore::Vector<casacore::Int> arrayId_p;
	mutable casacore::Vector<casacore::Int> scan_p;
	mutable casacore::Vector<casacore::Int> stateId_p;
	mutable casacore::Vector<casacore::Int> fieldId_p;
	mutable casacore::Vector<casacore::Int> dataDescriptionIds_p;
	mutable casacore::Vector<casacore::Int> spectralWindows_p;
	mutable casacore::Vector<casacore::Int> processorId_p;
	mutable casacore::Vector<casacore::Int> antenna1_p;
	mutable casacore::Vector<casacore::Int> antenna2_p;
	mutable casacore::Vector<casacore::Int> feed1_p;
	mutable casacore::Vector<casacore::Int> feed2_p;
	mutable casacore::Vector<casacore::Bool> flagRow_p;
	mutable casacore::Vector<casacore::Double> time_p;
	mutable casacore::Vector<casacore::Double> timeCentroid_p;
	mutable casacore::Vector<casacore::Double> timeInterval_p;
	mutable casacore::Vector<casacore::Double> exposure_p;
	mutable casacore::Matrix< casacore::Double> uvw_p;
	mutable casacore::Matrix<casacore::Float> weight_p;
	mutable casacore::Matrix<casacore::Float> sigma_p;
	mutable casacore::Cube<casacore::Bool> flagCube_p;
	mutable casacore::Cube<casacore::Complex> visCube_p;
	mutable casacore::Cube<casacore::Complex> visCubeCorrected_p;
	mutable casacore::Cube<casacore::Complex> visCubeModel_p;
	mutable casacore::Cube<casacore::Float> visCubeFloat_p;
	mutable casacore::Cube<casacore::Float> weightSpectrum_p;
	mutable casacore::Cube<casacore::Float> sigmaSpectrum_p;
	mutable casacore::Array<casacore::Bool> flagCategory_p;
	mutable casacore::Vector<casacore::Float> feedPa_p;
	mutable casacore::Vector<casacore::Float> parang_p;
	mutable casacore::Vector<casacore::MDirection> azel_p;
	mutable casacore::Vector<casacore::Double> frequencies_p;
	mutable casacore::Vector<casacore::Int> channelNumbers_p;
	mutable map< casacore::Int,casacore::Vector<casacore::Int> > outputInputChannelMap_p;
	mutable casacore::Vector<casacore::rownr_t> rowIds_p;
	mutable casacore::IPosition shape_p;
	mutable casacore::uInt nRows_p;
	mutable casacore::uInt nChannels_p;
	mutable casacore::uInt nCorrelations_p;
	mutable casacore::uInt nAntennas_p;
	mutable casacore::MFrequency::Types freqRefFrameType_p;

	mutable casacore::Bool observationIdOk_p;
	mutable casacore::Bool arrayIdOk_p;
	mutable casacore::Bool scanOk_p;
	mutable casacore::Bool stateIdOk_p;
	mutable casacore::Bool fieldIdOk_p;
	mutable casacore::Bool dataDescIdOk_p;
	mutable casacore::Bool spectralWindowsOk_p;
	mutable casacore::Bool processorIdOk_p;
	mutable casacore::Bool antenna1Ok_p;
	mutable casacore::Bool antenna2Ok_p;
	mutable casacore::Bool feed1Ok_p;
	mutable casacore::Bool feed2Ok_p;
	mutable casacore::Bool flagRowOk_p;
	mutable casacore::Bool timeOk_p;
	mutable casacore::Bool timeCentroidOk_p;
	mutable casacore::Bool timeIntervalOk_p;
	mutable casacore::Bool exposureOk_p;
	mutable casacore::Bool uvwOk_p;
	mutable casacore::Bool weightOk_p;
	mutable casacore::Bool sigmaOk_p;
	mutable casacore::Bool flagCubeOk_p;
	mutable casacore::Bool visCubeOk_p;
	mutable casacore::Bool visCubeCorrectedOk_p;
	mutable casacore::Bool visCubeModelOk_p;
	mutable casacore::Bool visCubeFloatOk_p;
	mutable casacore::Bool weightSpectrumOk_p;
	mutable casacore::Bool sigmaSpectrumOk_p;
	mutable casacore::Bool flagCategoryOk_p;
	mutable casacore::Bool feedPaOk_p;
	mutable casacore::Bool parangOk_p;
	mutable casacore::Bool azelOk_p;
	mutable casacore::Bool frequenciesOk_p;
	mutable casacore::Bool channelNumbersOk_p;
	mutable casacore::Bool channelNumbersSelectedOk_p;
	mutable casacore::Bool rowIdsOk_p;
	mutable casacore::Bool shapeOk_p;
	mutable casacore::Bool nRowsOk_p;
	mutable casacore::Bool nChannelsOk_p;
	mutable casacore::Bool nCorrelationsOk_p;
	mutable casacore::Bool nAntennasOk_p;
	mutable casacore::Bool freqRefFrameTypeOk_p;

	mutable casacore::Bool observationIdTransformed_p;
	mutable casacore::Bool arrayIdTransformed_p;
	mutable casacore::Bool scanTransformed_p;
	mutable casacore::Bool stateIdTransformed_p;
	mutable casacore::Bool fieldIdTransformed_p;
	mutable casacore::Bool dataDescIdTransformed_p;
	mutable casacore::Bool spectralWindowsTransformed_p;
	mutable casacore::Bool processorIdTransformed_p;
	mutable casacore::Bool antenna1Transformed_p;
	mutable casacore::Bool antenna2Transformed_p;
	mutable casacore::Bool feed1Transformed_p;
	mutable casacore::Bool feed2Transformed_p;
	mutable casacore::Bool flagRowTransformed_p;
	mutable casacore::Bool uvwTransformed_p;
	mutable casacore::Bool weightTransformed_p;
	mutable casacore::Bool sigmaTransformed_p;
	mutable casacore::Bool timeTransformed_p;
	mutable casacore::Bool timeCentroidTransformed_p;
	mutable casacore::Bool timeIntervalTransformed_p;
	mutable casacore::Bool exposureTransformed_p;
	mutable casacore::Bool feedPaTransformed_p;
	mutable casacore::Bool parangTransformed_p;
	mutable casacore::Bool azelTransformed_p;
	mutable casacore::Bool frequenciesTransformed_p;
	mutable casacore::Bool channelNumbersTransformed_p;
	mutable casacore::Bool rowIdsTransformed_p;

};

} //# NAMESPACE CASA - END


#endif /* MSTransformBufferImpl_H_ */

