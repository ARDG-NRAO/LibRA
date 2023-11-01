//# UtilsTVI.h: This file contains the interface definition of the MSTransformManager class.
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

#ifndef UtilsTVI_H_
#define UtilsTVI_H_

// casacore containers
#include <casacore/casa/Arrays/Cube.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/VectorIter.h>
#include <casacore/casa/Containers/Record.h>

// Measurement Set
#include <casacore/ms/MeasurementSets/MeasurementSet.h>

// Standard Lib
#include <map>
#include <float.h>

// NOTE: See implementation include below


namespace casa { //# NAMESPACE CASA - BEGIN

namespace vi { //# NAMESPACE VI - BEGIN


//////////////////////////////////////////////////////////////////////////
// DataCubeHolderBase class
//////////////////////////////////////////////////////////////////////////

class DataCubeHolderBase
{

public:

	DataCubeHolderBase() {}
	virtual ~DataCubeHolderBase() {}
	virtual DataCubeHolderBase * selfReference() = 0;
	virtual void setMatrixIndex(casacore::uInt matrixIndex) = 0;
	virtual void setVectorIndex(casacore::uInt vectorIndex) = 0;
	casacore::uInt getMatrixIndex();
	casacore::uInt getVectorIndex();
	casacore::IPosition & getCubeShape();
	casacore::IPosition & getMatrixShape();
	casacore::IPosition & getVectorShape();

  // Methods controlling internal iteration
  // gmoellen (2017Mar06)
  virtual void setupVecIter() = 0;
  virtual void reset() = 0;
  virtual void next() = 0;
  virtual casacore::Bool pastEnd() = 0;

protected:

	casacore::uInt matrixIndex_p;
	casacore::uInt vectorIndex_p;
	casacore::IPosition cubeShape_p;
	casacore::IPosition matrixShape_p;
	casacore::IPosition vectorShape_p;
};

//////////////////////////////////////////////////////////////////////////
// DataCubeHolder class
//////////////////////////////////////////////////////////////////////////

template <class T> class DataCubeHolder : public DataCubeHolderBase
{

public:

        DataCubeHolder(casacore::Cube<T> &dataCube) 
	  : veciter_p(0)
	{
		cube_p.reference(dataCube);
		cubeShape_p = cube_p.shape();
	}

	DataCubeHolder(const casacore::Cube<T> &dataCube)
	  : veciter_p(0)
	{
		cube_p.reference(dataCube);
		cubeShape_p = cube_p.shape();
	}

	DataCubeHolder(casacore::Matrix<T> &dataMatrix)
	  : veciter_p(0)
	{
		matrix_p.reference(dataMatrix);
		matrixShape_p = matrix_p.shape();
	}

	DataCubeHolder(const casacore::Matrix<T> &dataMatrix)
	  : veciter_p(0)
	{
		matrix_p.reference(dataMatrix);
		matrixShape_p = matrix_p.shape();
	}

	DataCubeHolder(casacore::Vector<T> &dataVector)
	  : veciter_p(0)
	{
		vector_p.reference(dataVector);
		vectorShape_p = vector_p.shape();
	}

	DataCubeHolder(const casacore::Vector<T> &dataVector)
	  : veciter_p(0)
	{
		vector_p.reference(dataVector);
		vectorShape_p = vector_p.shape();
	}

  // Destructor must delete the iterator
  // gmoellen (2017Mar06)
virtual ~DataCubeHolder()
  {
    if (veciter_p) delete veciter_p;
  }


	casacore::Matrix<T> & getMatrix() {return matrix_p;}
	casacore::Vector<T> & getVector() {return vector_p;}

	void setMatrixIndex(casacore::uInt matrixIndex)
	{
		matrix_p.resize(); // Resize to 0 to avoid shape conformance problems
		matrixIndex_p = matrixIndex;
		matrix_p.reference(cube_p.xyPlane(matrixIndex));
		matrixShape_p = matrix_p.shape();

		return;
	}

	void setVectorIndex(casacore::uInt vectorIndex)
	{
		vector_p.resize(); // Resize to 0 to avoid shape conformance problems
		vectorIndex_p = vectorIndex;
		vector_p.reference(matrix_p.row(vectorIndex));
		vectorShape_p = vector_p.shape();

		return;
	}

	DataCubeHolderBase * selfReference()
	{
		DataCubeHolder<T> *selfRef = new DataCubeHolder<T>(cube_p);
		return static_cast<DataCubeHolderBase*>(selfRef);
	}


  // Methods controlling internal iteration
  // gmoellen (2017Mar06)
  virtual void setupVecIter() {
    // Construct the iterator, selecting the channel axis cursor
    if (veciter_p) delete veciter_p;
    veciter_p = new casacore::VectorIterator<T>(cube_p,1);  // normally deleted in dtor
    // refer vector_p to the iterator's vector; this will stay sync'd
    vector_p.reference(veciter_p->vector());
    vectorShape_p = vector_p.shape(); 
    // NB: matrix_p refers to nothing (forever, in this context)
  }
  // NB: the reference calls below can be avoided if vector_p is a _c++_ reference
  //     initialzed in the DCH ctor to reference veciter_p's internal Vector
  virtual void reset() {veciter_p->reset(); vector_p.reference(veciter_p->vector());}
  virtual void next() {veciter_p->next(); vector_p.reference(veciter_p->vector());}
  virtual casacore::Bool pastEnd() {return veciter_p->pastEnd(); }

protected:

	casacore::Cube<T> cube_p;
	casacore::Matrix<T> matrix_p;
	casacore::Vector<T> vector_p;

  // Iterator for cube_p
  // gmoellen (2017Mar06)
  casacore::VectorIterator<T> *veciter_p;

};

//////////////////////////////////////////////////////////////////////////
// DataCubeMap class
//////////////////////////////////////////////////////////////////////////

class DataCubeMap
{

public:

	DataCubeMap();
	DataCubeMap(DataCubeMap& other);
        virtual ~DataCubeMap();

	void add(casacore::MS::PredefinedColumns key,DataCubeHolderBase* dataCubeHolder);
	void add(casacore::MS::PredefinedColumns key,DataCubeHolderBase &dataCubeHolder);

	casacore::Bool present(casacore::MS::PredefinedColumns key);

	template <class T> casacore::Vector<T> & getVector(casacore::MS::PredefinedColumns key)
	{
		DataCubeHolder<T> *dataCubeHolder = static_cast< DataCubeHolder<T>* >(dataCubeMap_p[key]);
		return dataCubeHolder->getVector();
	}

	template <class T> casacore::Matrix<T> & getMatrix(casacore::MS::PredefinedColumns key)
	{
		DataCubeHolder<T> *dataCubeHolder = static_cast< DataCubeHolder<T>* >(dataCubeMap_p[key]);
		return dataCubeHolder->getVector();
	}

	void setMatrixIndex(casacore::uInt rowIndex);
	void setVectorIndex(casacore::uInt vectorIndex);

	casacore::IPosition & getCubeShape();
	casacore::IPosition & getMatrixShape();
	casacore::IPosition & getVectorShape();

	size_t nelements();

  // Methods controlling iteration
  // gmoellen (2017Mar06)
  virtual void setupVecIter(); 
  void reset();
  void next();
  casacore::Bool pastEnd();


protected:

	std::map<casacore::MS::PredefinedColumns, DataCubeHolderBase*> dataCubeMap_p;
	std::map<casacore::MS::PredefinedColumns, DataCubeHolderBase*>::iterator dataCubeMapIter_p;
};


//////////////////////////////////////////////////////////////////////////
// Convenience methods
//////////////////////////////////////////////////////////////////////////

inline casacore::Float weightToSigma (casacore::Float weight)
{
	return weight > FLT_MIN ? 1.0 / std::sqrt (weight) : -1.0;
}

inline casacore::Float sigmaToWeight (casacore::Float sigma)
{
	return sigma > FLT_MIN ? 1.0 / (sigma * sigma) : 0.0;
}

void accumulateWeightCube (	const casacore::Cube<casacore::Float> &weightCube,
							const casacore::Cube<casacore::Bool> &flags,
							casacore::Matrix<casacore::Float> &result);

void accumulateWeightMatrix (	const casacore::Matrix<casacore::Float> &weightMatrix,
								const casacore::Matrix<casacore::Bool> &flags,
								casacore::Vector<casacore::Float> &result);

void accumulateFlagCube (	const casacore::Cube<casacore::Bool> &flagCube,
							casacore::Vector<casacore::Bool> &flagRow);



} //# NAMESPACE VI - END

} //# NAMESPACE CASA - END


#endif /* UtilsTVI_H_ */

