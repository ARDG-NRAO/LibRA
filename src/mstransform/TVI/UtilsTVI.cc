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

#include <mstransform/TVI/UtilsTVI.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

namespace vi { //# NAMESPACE VI - BEGIN

//////////////////////////////////////////////////////////////////////////
// DataCubeHolderBase class
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
uInt DataCubeHolderBase::getMatrixIndex()
{
	return matrixIndex_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
uInt DataCubeHolderBase::getVectorIndex()
{
	return vectorIndex_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
IPosition & DataCubeHolderBase::getCubeShape()
{
	return cubeShape_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
IPosition & DataCubeHolderBase::getMatrixShape()
{
	return matrixShape_p;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
IPosition & DataCubeHolderBase::getVectorShape()
{
	return vectorShape_p;
}

//////////////////////////////////////////////////////////////////////////
// DataCubeMap class
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
DataCubeMap::DataCubeMap()
{
	dataCubeMap_p.clear();
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
DataCubeMap::DataCubeMap(DataCubeMap& other)
{
	dataCubeMap_p.clear();

	for (dataCubeMapIter_p = other.dataCubeMap_p.begin();dataCubeMapIter_p!= other.dataCubeMap_p.end();dataCubeMapIter_p++)
	{
		add(dataCubeMapIter_p->first,dataCubeMapIter_p->second->selfReference());
	}

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
DataCubeMap::~DataCubeMap()
{
	dataCubeMap_p.clear();
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void DataCubeMap::add(MS::PredefinedColumns key,DataCubeHolderBase* dataCubeHolder)
{
	dataCubeMap_p[key] = dataCubeHolder;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void DataCubeMap::add(MS::PredefinedColumns key,DataCubeHolderBase &dataCubeHolder)
{
	dataCubeMap_p[key] = &dataCubeHolder;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Bool DataCubeMap::present(MS::PredefinedColumns key)
{
	if (dataCubeMap_p.find(key) != dataCubeMap_p.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void DataCubeMap::setMatrixIndex(uInt rowIndex)
{
	for (dataCubeMapIter_p = dataCubeMap_p.begin();dataCubeMapIter_p!= dataCubeMap_p.end();dataCubeMapIter_p++)
	{
		dataCubeMapIter_p->second->setMatrixIndex(rowIndex);
	}

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void DataCubeMap::setVectorIndex(uInt vectorIndex)
{
	for (dataCubeMapIter_p = dataCubeMap_p.begin();dataCubeMapIter_p!= dataCubeMap_p.end();dataCubeMapIter_p++)
	{
		dataCubeMapIter_p->second->setVectorIndex(vectorIndex);
	}

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
IPosition & DataCubeMap::getCubeShape()
{
	return dataCubeMap_p.begin()->second->getCubeShape();
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
IPosition & DataCubeMap::getMatrixShape()
{
	return dataCubeMap_p.begin()->second->getMatrixShape();
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
IPosition & DataCubeMap::getVectorShape()
{
	return dataCubeMap_p.begin()->second->getVectorShape();
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
size_t DataCubeMap::nelements()
{
	return dataCubeMap_p.size();
}


// Methods controlling iteration
// gmoellen (2017Mar06)
void DataCubeMap::setupVecIter() 
{
  for (dataCubeMapIter_p = dataCubeMap_p.begin();
       dataCubeMapIter_p!= dataCubeMap_p.end();
       dataCubeMapIter_p++)
    {
      dataCubeMapIter_p->second->setupVecIter();
    }
  return;
}
void DataCubeMap::reset() 
{
  for (dataCubeMapIter_p = dataCubeMap_p.begin();
       dataCubeMapIter_p!= dataCubeMap_p.end();
       dataCubeMapIter_p++)
    {
      dataCubeMapIter_p->second->reset();
    }
  return;
}

void DataCubeMap::next()
{
  for (dataCubeMapIter_p = dataCubeMap_p.begin();
       dataCubeMapIter_p!= dataCubeMap_p.end();
       dataCubeMapIter_p++)
    {
      dataCubeMapIter_p->second->next();
    }
  return;
}

casacore::Bool DataCubeMap::pastEnd()
{
  // All elements in the map are sync'd, so just ask the first one
  return dataCubeMap_p.begin()->second->pastEnd();
}


//////////////////////////////////////////////////////////////////////////
// Convenience methods
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void accumulateWeightCube (const Cube<Float> &weightCube, const Cube<Bool> &flags, Matrix<Float> &result)
{
	IPosition shape = weightCube.shape();
	uInt nRows = shape(2);
	uInt nCorrelations = shape (0);

	result.resize(nCorrelations,nRows,false);

	Vector<Float> currentVector;
	for (uInt row=0; row < nRows; row++)
	{
		currentVector.reference(result.column(row));
		accumulateWeightMatrix (weightCube.xyPlane(row), flags.xyPlane(row),currentVector);
	}

    return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void accumulateWeightMatrix (const Matrix<Float> &weightMatrix, const Matrix<Bool> &flags, Vector<Float> &result)
{
    IPosition shape = weightMatrix.shape();
    result.resize(shape(0),false);
    Vector<uInt> samples(shape(0),0);
    uInt nCorrelations = shape (0);
    uInt nChannels = shape (1);

    for (uInt correlation = 0; correlation < nCorrelations; correlation++)
    {
        int nSamples = 0;
        float sum = 0;
        bool accumulatorFlag = true;

        for (uInt channel=0; channel< nChannels; channel++)
        {
            Bool inputFlag = flags(correlation,channel);
            // true/true or false/false
            if (accumulatorFlag == inputFlag)
            {
                nSamples ++;
                sum += weightMatrix (correlation, channel);
            }
            // true/false: Reset accumulation when accumulator switches from flagged to unflagged
            else if ( accumulatorFlag and ! inputFlag )
            {
                accumulatorFlag = false;
                nSamples = 1;
                sum = weightMatrix (correlation, channel);
            }
            // else ignore case where accumulator is valid and data is not

        }

        result (correlation) = sum / (nSamples != 0 ? nSamples : 1);
    }

    return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void accumulateFlagCube (const Cube<Bool> &flagCube, Vector<Bool> &flagRow)
{
	// Get original shape
	IPosition shape = flagCube.shape();
	size_t nCorr = shape(0);
	size_t nChan = shape(1);
	size_t nRows = shape(2);

	// Reshape flag cube to match the input shape
	flagRow.resize(nRows,false);
	flagRow = false;

	Bool rowFlagValue = false;
	for (size_t row_i =0;row_i<nRows;row_i++)
	{
		rowFlagValue = true;
		for (size_t chan_i =0;chan_i<nChan;chan_i++)
		{
			if (rowFlagValue)
			{
				for (size_t corr_i =0;corr_i<nCorr;corr_i++)
				{
					if (not flagCube(corr_i,chan_i,row_i))
					{
						rowFlagValue = false;
						break;
					}
				}
			}
			else
			{
				break;
			}
		}
		flagRow(row_i) = rowFlagValue;
	}

	return;
}


} //# NAMESPACE VI - END

} //# NAMESPACE CASA - END

