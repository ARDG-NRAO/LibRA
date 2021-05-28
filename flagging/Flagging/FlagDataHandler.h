//# FlagDataHandler.h: This file contains the interface definition of the FlagDataHandler class.
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

#ifndef FLAGDATAHANDLER_H_
#define FLAGDATAHANDLER_H_

// VI/VB infrastructure
#include <msvis/MSVis/VisibilityIterator2.h>

// TVI framework
#include <msvis/MSVis/AveragingVi2Factory.h>

// Records interface
#include <casa/Containers/Record.h>

// casacore::Data mapping
#include <map>

namespace casa {
  namespace vi {
    enum class VisBufferComponent2;
  }
}

namespace casacore {

  class MSSelection;
  template <class M> class ScalarMeasColumn;
}

namespace casa { //# NAMESPACE CASA - BEGIN

// Type definitions
typedef std::map< std::pair<casacore::Int,casacore::Int>,std::vector<casacore::uInt> >::iterator antennaPairMapIterator;
typedef std::map< casacore::Double,std::vector<casacore::uInt> >::iterator subIntegrationMapIterator;
typedef std::map< casacore::uShort,casacore::uShort >::iterator polartizationMapIterator;
typedef std::map< std::pair<casacore::Int,casacore::Int>,std::vector<casacore::uInt> > antennaPairMap;
typedef std::map< casacore::Double,std::vector<casacore::uInt> > subIntegrationMap;
typedef std::map< casacore::uShort,casacore::uShort > polarizationMap;
typedef std::map< casacore::uInt,casacore::String > polarizationIndexMap;
typedef std::vector< std::vector<casacore::Double> > antennaPointingMap;
typedef std::map< casacore::Int,std::vector<casacore::Double> > scanStartStopMap;
typedef std::map< casacore::Int,casacore::Double > lambdaMap;

const casacore::Complex ImaginaryUnit = casacore::Complex(0,1);

// We need to have the CubeView definition here because its type is used by FlagDataHandler class
template<class T> class CubeView
{

public:

	CubeView(casacore::Cube<T> *parentCube, std::vector<casacore::uInt> *rows = NULL, std::vector<casacore::uInt> *channels = NULL, std::vector<casacore::uInt> *polarizations = NULL)
	{
		parentCube_p = parentCube;
		casacore::IPosition baseCubeShape = parentCube_p->shape();
		reducedLength_p = casacore::IPosition(3);

		if (((polarizations != NULL) and (polarizations->size() > 0)) and
			((channels != NULL) and (channels->size() > 0)) and
			((rows != NULL) and (rows->size() > 0)))
		{
			access_p = &CubeView::accessMapped;
		}
		else if (((polarizations != NULL) and (polarizations->size() > 0)) and
				((channels != NULL) and (channels->size() > 0)))
		{
			access_p = &CubeView::accessIndex12Mapped;
		}
		else if (((polarizations != NULL) and (polarizations->size() > 0)) and
				((rows != NULL) and (rows->size() > 0)))
		{
			access_p = &CubeView::accessIndex13Mapped;
		}
		else if (((channels != NULL) and (channels->size() > 0)) and
				((rows != NULL) and (rows->size() > 0)))
		{
			access_p = &CubeView::accessIndex23Mapped;
		}
		else if ((polarizations != NULL) and (polarizations->size() > 0))
		{
			access_p = &CubeView::accessIndex1Mapped;
		}
		else if ((channels != NULL) and (channels->size() > 0))
		{
			access_p = &CubeView::accessIndex2Mapped;
		}
		else if ((rows != NULL) and (rows->size() > 0))
		{
			access_p = &CubeView::accessIndex3Mapped;
		}
		else
		{
			access_p = &CubeView::accessUnmapped;
		}

		if ((polarizations != NULL) and (polarizations->size() > 0))
		{
			polarizations_p = polarizations;
			reducedLength_p(0) = polarizations_p->size();
		}
		else
		{
			polarizations_p = NULL;
			reducedLength_p(0) = baseCubeShape(0);
		}

		if ((channels != NULL) and (channels->size() > 0))
		{
			channels_p = channels;
			reducedLength_p(1) = channels_p->size();
		}
		else
		{
			channels_p = NULL;
			reducedLength_p(1) = baseCubeShape(1);
		}

		if ((rows != NULL) and (rows->size() > 0))
		{
			rows_p = rows;
			reducedLength_p(2) = rows_p->size();
		}
		else
		{
			rows_p = NULL;
			reducedLength_p(2) = baseCubeShape(2);
		}
	}

    T &operator()(casacore::uInt i1, casacore::uInt i2, casacore::uInt i3)
    {
    	return (*this.*access_p)(i1,i2,i3);
    }

    const casacore::IPosition &shape() const
    {
    	return reducedLength_p;
    }

    void shape(casacore::Int &s1, casacore::Int &s2, casacore::Int &s3) const
    {
    	s1 = reducedLength_p(0);
    	s2 = reducedLength_p(1);
    	s3 = reducedLength_p(2);
    	return;
    }

protected:

    std::vector<casacore::uInt> *createIndex(casacore::uInt size)
    {
        std::vector<casacore::uInt> *index = new std::vector<casacore::uInt>(size);
    	index->clear();
    	for (casacore::uInt i=0; i<size; i++ )
    	{
    		index->push_back(i);
    	}
    	return index;
    }

    T &accessUnmapped(casacore::uInt i1, casacore::uInt i2, casacore::uInt i3)
    {
    	return (*parentCube_p)(i1,i2,i3);
    }

    T &accessMapped(casacore::uInt i1, casacore::uInt i2, casacore::uInt i3)
    {
    	casacore::uInt i1_index = polarizations_p->at(i1);
    	casacore::uInt i2_index = channels_p->at(i2);
    	casacore::uInt i3_index = rows_p->at(i3);
    	return (*parentCube_p)(i1_index,i2_index,i3_index);
    }

    T &accessIndex1Mapped(casacore::uInt i1, casacore::uInt i2, casacore::uInt i3)
    {
    	casacore::uInt i1_index = polarizations_p->at(i1);
    	return (*parentCube_p)(i1_index,i2,i3);
    }

    T &accessIndex2Mapped(casacore::uInt i1, casacore::uInt i2, casacore::uInt i3)
    {
    	casacore::uInt i2_index = channels_p->at(i2);
    	return (*parentCube_p)(i1,i2_index,i3);
    }

    T &accessIndex3Mapped(casacore::uInt i1, casacore::uInt i2, casacore::uInt i3)
    {
    	casacore::uInt i3_index = rows_p->at(i3);
    	return (*parentCube_p)(i1,i2,i3_index);
    }

    T &accessIndex12Mapped(casacore::uInt i1, casacore::uInt i2, casacore::uInt i3)
    {
    	casacore::uInt i1_index = polarizations_p->at(i1);
    	casacore::uInt i2_index = channels_p->at(i2);
    	return (*parentCube_p)(i1_index,i2_index,i3);
    }

    T &accessIndex13Mapped(casacore::uInt i1, casacore::uInt i2, casacore::uInt i3)
    {
    	casacore::uInt i1_index = polarizations_p->at(i1);
    	casacore::uInt i3_index = rows_p->at(i3);
    	return (*parentCube_p)(i1_index,i2,i3_index);
    }

    T &accessIndex23Mapped(casacore::uInt i1, casacore::uInt i2, casacore::uInt i3)
    {
    	casacore::uInt i2_index = channels_p->at(i2);
    	casacore::uInt i3_index = rows_p->at(i3);
    	return (*parentCube_p)(i1,i2_index,i3_index);
    }

private:
    casacore::Cube<T> *parentCube_p;
	std::vector<casacore::uInt> *rows_p;
	std::vector<casacore::uInt> *channels_p;
	std::vector<casacore::uInt> *polarizations_p;
	casacore::IPosition reducedLength_p;
	T &(casa::CubeView<T>::*access_p)(casacore::uInt,casacore::uInt,casacore::uInt);
};

template<class T> class VectorView
{

public:
	VectorView(casacore::Vector<T> *parentVector, std::vector<casacore::uInt> *rows = NULL)
	{
		casacore::IPosition parentVectorShape = parentVector->shape();
		parentVector_p = parentVector;
		reducedLength_p = casacore::IPosition(1);

		if ((rows != NULL) and (rows->size() > 0))
		{
			access_p = &VectorView::accessMapped;
		}
		else
		{
			access_p = &VectorView::accessUnmapped;
		}

		if ((rows != NULL) and (rows->size() > 0))
		{
			rows_p = rows;
			reducedLength_p(0) = rows_p->size();
		}
		else
		{
			rows_p = NULL;
			reducedLength_p(0) = parentVectorShape(0);
		}
	}

    T &operator()(casacore::uInt i1)
    {
    	return (*this.*access_p)(i1);
    }

    const casacore::IPosition &shape() const
    {
    	return reducedLength_p;
    }

    void shape(casacore::Int &s1) const
    {
    	s1 = reducedLength_p(0);
    	return;
    }

protected:

    std::vector<casacore::uInt> *createIndex(casacore::uInt size)
    {
    	std::vector<casacore::uInt> *index = new std::vector<casacore::uInt>(size);
    	index->clear();
    	for (casacore::uInt i=0; i<size; i++ )
    	{
    		index->push_back(i);
    	}
    	return index;
    }

    T &accessUnmapped(casacore::uInt i1)
    {
    	return parentVector_p->operator()(i1);
    }

    T &accessMapped(casacore::uInt i1)
    {
    	casacore::uInt i1_index = rows_p->at(i1);
    	return parentVector_p->operator()(i1_index);
    }

private:
    casacore::Vector<T> *parentVector_p;
	std::vector<casacore::uInt> *rows_p;
	casacore::IPosition reducedLength_p;
	T &(casa::VectorView<T>::*access_p)(casacore::uInt);
};

class VisMapper
{
	typedef casacore::Complex (casa::VisMapper::*corrProduct)(casacore::uInt,casacore::uInt);

public:

	enum calsolutions {

		CALSOL1=casacore::Stokes::NumberOfTypes,
		CALSOL2,
		CALSOL3,
		CALSOL4
	};

	VisMapper(casacore::String expression,polarizationMap *polMap,CubeView<casacore::Complex> *leftVis,CubeView<casacore::Complex> *rightVis=NULL);
	VisMapper(casacore::String expression,polarizationMap *polMap);
	~VisMapper();

    void setParentCubes(CubeView<casacore::Complex> *leftVis,CubeView<casacore::Complex> *rightVis=NULL);

    std::vector< std::vector<casacore::uInt> > getSelectedCorrelations() { return selectedCorrelations_p;}
    std::vector< string > getSelectedCorrelationStrings() { return selectedCorrelationStrings_p;}

	casacore::Float operator()(casacore::uInt chan, casacore::uInt row);
	casacore::Float operator()(casacore::uInt pol, casacore::uInt chan, casacore::uInt row);

	// Direct access to the complex correlation product
	casacore::Complex correlationProduct(casacore::uInt pol, casacore::uInt chan, casacore::uInt row);

    // NOTE: reducedLength_p is defined as [chan,row,pol]
    const casacore::IPosition &shape() const
    {
    	return reducedLength_p;
    }

    void shape(casacore::Int &chan, casacore::Int &row) const
    {
    	chan = reducedLength_p(0);
    	row = reducedLength_p(1);
    	return;
    }

    void shape(casacore::Int &pol, casacore::Int &chan, casacore::Int &row) const
    {
    	chan = reducedLength_p(0);
    	row = reducedLength_p(1);
    	pol = reducedLength_p(2);
    	return;
    }


protected:
    void setExpressionMapping(casacore::String expression,polarizationMap *polMap);
	casacore::Float real(casacore::Complex val) {return val.real();}
	casacore::Float imag(casacore::Complex val) {return val.imag();}
	casacore::Float abs(casacore::Complex val) {return std::abs(val);}
	casacore::Float arg(casacore::Complex val) {return std::arg(val);}
	casacore::Float norm(casacore::Complex val) {return std::norm(val);}
	casacore::Complex leftVis(casacore::uInt pol, casacore::uInt chan, casacore::uInt row);
	casacore::Complex diffVis(casacore::uInt pol, casacore::uInt chan, casacore::uInt row);
	casacore::Complex stokes_i(casacore::uInt pol, casacore::uInt chan);
	casacore::Complex stokes_q(casacore::uInt pol, casacore::uInt chan);
	casacore::Complex stokes_u(casacore::uInt pol, casacore::uInt chan);
	casacore::Complex stokes_v(casacore::uInt pol, casacore::uInt chan);
	casacore::Complex linear_xx(casacore::uInt pol, casacore::uInt chan);
	casacore::Complex linear_yy(casacore::uInt pol, casacore::uInt chan);
	casacore::Complex linear_xy(casacore::uInt pol, casacore::uInt chan);
	casacore::Complex linear_yx(casacore::uInt pol, casacore::uInt chan);
	casacore::Complex circular_rr(casacore::uInt pol, casacore::uInt chan);
	casacore::Complex circular_ll(casacore::uInt pol, casacore::uInt chan);
	casacore::Complex circular_rl(casacore::uInt pol, casacore::uInt chan);
	casacore::Complex circular_lr(casacore::uInt pol, casacore::uInt chan);
	casacore::Complex stokes_i_from_linear(casacore::uInt chan, casacore::uInt row);
	casacore::Complex stokes_q_from_linear(casacore::uInt chan, casacore::uInt row);
	casacore::Complex stokes_u_from_linear(casacore::uInt chan, casacore::uInt row);
	casacore::Complex stokes_v_from_linear(casacore::uInt chan, casacore::uInt row);
	casacore::Complex stokes_i_from_circular(casacore::uInt chan, casacore::uInt row);
	casacore::Complex stokes_q_from_circular(casacore::uInt chan, casacore::uInt row);
	casacore::Complex stokes_u_from_circular(casacore::uInt chan, casacore::uInt row);
	casacore::Complex stokes_v_from_circular(casacore::uInt chan, casacore::uInt row);
	casacore::Complex calsol1(casacore::uInt chan, casacore::uInt row);
	casacore::Complex calsol2(casacore::uInt chan, casacore::uInt row);
	casacore::Complex calsol3(casacore::uInt chan, casacore::uInt row);
	casacore::Complex calsol4(casacore::uInt chan, casacore::uInt row);


private:
	casacore::Float (casa::VisMapper::*applyVisExpr_p)(casacore::Complex);
	casacore::Complex (casa::VisMapper::*getVis_p)(casacore::uInt,casacore::uInt,casacore::uInt);
	// casacore::Complex (casa::VisMapper::*getCorr_p)(casacore::uInt,casacore::uInt);
    std::vector<corrProduct> selectedCorrelationProducts_p;
    std::vector< std::vector<casacore::uInt> > selectedCorrelations_p;
    std::vector<string> selectedCorrelationStrings_p;
	CubeView<casacore::Complex> *leftVis_p;
	CubeView<casacore::Complex> *rightVis_p;
	casacore::IPosition reducedLength_p;
	polarizationMap *polMap_p;
	casacore::String expression_p;
};

class FlagMapper
{

public:

	FlagMapper(casacore::Bool flag,	std::vector < std::vector<casacore::uInt> > selectedCorrelations,
							CubeView<casacore::Bool> *commonFlagsView,
							CubeView<casacore::Bool> *originalFlagsView,
							CubeView<casacore::Bool> *privateFlagsView=NULL,
							VectorView<casacore::Bool> *commonFlagRowView=NULL,
							VectorView<casacore::Bool> *originalFlagRowView=NULL,
							VectorView<casacore::Bool> *privateFlagRowView=NULL);
	FlagMapper(casacore::Bool flag,std::vector< std::vector<casacore::uInt> > selectedCorrelations);
	~FlagMapper();

	void setParentCubes(CubeView<casacore::Bool> *commonFlagsView,CubeView<casacore::Bool> *originalFlagsView,CubeView<casacore::Bool> *privateFlagsView=NULL);
	void setParentFlagRow(VectorView<casacore::Bool> *commonFlagRowView,VectorView<casacore::Bool> *originalFlagRowView,VectorView<casacore::Bool> *privateFlagRowView=NULL);

	void applyFlag(casacore::uInt chan, casacore::uInt row);
	void applyFlag(casacore::uInt pol, casacore::uInt channel, casacore::uInt row);
	void applyFlagRow(casacore::uInt row);
	void applyFlagInRow(casacore::uInt row);

	casacore::Bool getOriginalFlags(casacore::uInt chan, casacore::uInt row);
	casacore::Bool getModifiedFlags(casacore::uInt chan, casacore::uInt row);
	casacore::Bool getPrivateFlags(casacore::uInt chan, casacore::uInt row);

	casacore::Bool getOriginalFlags(casacore::uInt pol, casacore::uInt channel, casacore::uInt row);
	casacore::Bool getModifiedFlags(casacore::uInt pol, casacore::uInt channel, casacore::uInt row);
	casacore::Bool getPrivateFlags(casacore::uInt pol, casacore::uInt channel, casacore::uInt row);

	// These methods are needed for flag extension
	void setModifiedFlags(casacore::uInt pol, casacore::uInt channel, casacore::uInt row);
	void setPrivateFlags(casacore::uInt pol, casacore::uInt channel, casacore::uInt row);

	casacore::Bool getOriginalFlagRow(casacore::uInt row);
	casacore::Bool getModifiedFlagRow(casacore::uInt row);
	casacore::Bool getPrivateFlagRow(casacore::uInt row);

    const casacore::IPosition &shape() const
    {
    	return reducedLength_p;
    }

    void shape(casacore::Int &chan, casacore::Int &row) const
    {
    	chan = reducedLength_p(0);
    	row = reducedLength_p(1);
    	return;
    }

    void shape(casacore::Int &pol, casacore::Int &chan, casacore::Int &row) const
    {
    	chan = reducedLength_p(0);
    	row = reducedLength_p(1);
    	pol = reducedLength_p(2);
    	return;
    }

    std::vector< std::vector<casacore::uInt> > getSelectedCorrelations() {return selectedCorrelations_p;}

    void activateCheckMode() {applyFlag_p = &FlagMapper::checkCommonFlags;}

    casacore::uInt nSelectedCorrelations() {return nSelectedCorrelations_p;}
    casacore::uInt flagsPerRow() {return flagsPerRow_p;}

protected:

	void setExpressionMapping(std::vector< std::vector<casacore::uInt> > selectedCorrelations);

	// Apply flags to common flag cube
	void applyCommonFlags(casacore::uInt pol, casacore::uInt channel, casacore::uInt row);
	// Apply flags to common and private flag cubes
	void applyPrivateFlags(casacore::uInt pol, casacore::uInt channel, casacore::uInt row);
	// Apply flags to common and private flag cubes
	void checkCommonFlags(casacore::uInt pol, casacore::uInt channel, casacore::uInt row);

	// Apply flags to common flag rows
	void applyCommonFlagRow(casacore::uInt row);
	// Apply flags to common and private flag rows
	void applyPrivateFlagRow(casacore::uInt row);

private:

	casacore::Bool flag_p;
    casacore::IPosition reducedLength_p;
	CubeView<casacore::Bool> *commonFlagsView_p;
	CubeView<casacore::Bool> *originalFlagsView_p;
	CubeView<casacore::Bool> *privateFlagsView_p;
	VectorView<casacore::Bool> *commonFlagRowView_p;
	VectorView<casacore::Bool> *originalFlagRowView_p;
	VectorView<casacore::Bool> *privateFlagRowView_p;
    std::vector< std::vector<casacore::uInt> > selectedCorrelations_p;
	casacore::uInt nSelectedCorrelations_p;
	casacore::uInt flagsPerRow_p;
	void (casa::FlagMapper::*applyFlag_p)(casacore::uInt,casacore::uInt,casacore::uInt);
	void (casa::FlagMapper::*applyFlagRow_p)(casacore::uInt);
};

// <summary>
// A top level class defining the data handling interface for the flagging module
// </summary>
//
// <use visibility=export>
//
// <prerequisite>
//   <li> <linkto class="VisBuffer:description">VisBuffer</linkto>
//   <li> <linkto class="FlagMapper:description">FlagMapper</linkto>
//   <li> <linkto class="VisMapper:description">VisMapper</linkto>
// </prerequisite>
//
// <etymology>
// FlagDataHandler stands for generic data handling (i.e. MSs, CalTables, ...) specific to the flagging module
// </etymology>
//
// <synopsis>
//
// This is a top-level class defining the data handling interface for the flagging module.
// There are various methods (virtual) that must be re-implemented by the specific derived
// classes (e.g. FlagMSHandler, FlagCalTableHandler). These methods essentially cover:
//
// - casacore::Table access (i.e. open/close/iteration/rw)
//
// - casacore::Table selection (i.e. expression parsing, sub-table selection)
//
// Additionally there are public non-virtual methods to:
//
// - Set configuration parameters (pre-sort columns, data selection, time interval, async I/O)
//
// - Enable and access different kinds of mapping (baselines, correlation products, antenna pointing)
//
// Also at this top level there are public members which are used by the FlagAgent classes,
// so that there is no dependency with the specific implementation classes (e.g. FlagMsHandler,
// FlagCalTableHandler), and thus no re-implementation is required at the FlagAgent level.
//
// - VisBuffer (for accessing the data and meta data columns)
//
// - Chunk and casacore::Table flag counters (for statistics)
//
// </synopsis>
//
// <motivation>
// The motivation for the FlagDataHandler class is having all the data operations encapsulated
// in one single class, with a common interface for all types of tables (MSs, CalTables, SingleDish),
// so that no specific specific table type implementation has to be done at the FlagAgent level.
// </motivation>
//
// <example>
// <srcblock>
//
// // The following code sets up a FlagDataHandler with either CalTable or casacore::MS implementation and
// // iterates through the table applying a clip agent, flushing the flags, and extracting summaries.
//
// // IMPORTANT NOTE:
// // The order of FlagDataHandler and FlagAgent initialization is critical to have everything right,
// // in particular data selection must happen before initializing FlagAgents, and iterator generation
// // must be done after initializing FlagAgents, so that each agent can communicate to the FlagDataHandler
// // which columns have to be pre-fetched (async i/o or parallel mode), and what mapping options are necessary.
//
// // NOTE ON ASYNC I/O:
// // Asyncnronous I/O is only enabled for casacore::MS-type tables, but not for CalTables, and it is necessary to switch
// // it on before generating the iterators. Something else to take into account, is that there are 2 global
// // switches at .casarc level which invalidate the application code selection:
// //
// // VisibilityIterator.async.enabled rules over
// // |-> FlagDataHandler.asyncio, and in turns rules over
// //     |-> FlagDataHandler.enableAsyncIO(true)
//
// // Identify table type
// casacore::Table table(msname_p,casacore::TableLock(TableLock::AutoNoReadLocking));
// casacore::TableInfo& info = table.tableInfo();
// casacore::String type=info.type();
//
// // Create derived FlagDataHandler object with corresponding implementation
// FlagDataHandler *fdh_p = NULL;
// if (type == "Measurement Set")
// {
//    fdh_p = new FlagMSHandler(msname_p, FlagDataHandler::COMPLETE_SCAN_UNMAPPED, timeInterval_p);
// }
// else
// {
//    fdh_p = new FlagCalTableHandler(msname_p, FlagDataHandler::COMPLETE_SCAN_UNMAPPED, timeInterval_p);
// }
//
// // NOTE: It is also possible to independently set the iteration approach via the setIterationApproach
// //       method which accepts the following modes, defined in the FlagDataHandler iteration enumeration
// //
// //       COMPLETE_SCAN_MAPPED:
// //       - casacore::Sort by OBSERVATION_ID, ARRAY_ID, SCAN_NUMBER, FIELD_ID, DATA_DESC_ID and TIME
// //       - Group all time steps together, so that there is no sub-chunk iteration
// //       - Generate baseline maps (to iterate trough rows with the same antenna1, antenna2)
// //       - Generate sub-integration maps (to iterate trough rows with the same timestamp)
// //       COMPLETE_SCAN_MAP_SUB_INTEGRATIONS_ONLY:
// //       - casacore::Sort by OBSERVATION_ID, ARRAY_ID, SCAN_NUMBER, FIELD_ID, DATA_DESC_ID and TIME
// //       - Group all time steps together, so that there is no sub-chunk iteration
// //       * Don't generate baseline maps
// //       - Generate sub-integration maps (to iterate trough rows with the same timestamp)
// //       COMPLETE_SCAN_MAP_ANTENNA_PAIRS_ONLY:
// //       - casacore::Sort by OBSERVATION_ID, ARRAY_ID, SCAN_NUMBER, FIELD_ID, DATA_DESC_ID and TIME
// //       - Group all time steps together, so that there is no sub-chunk iteration
// //       - Generate baseline maps (to iterate trough rows with the same antenna1, antenna2)
// //       * Don't generate sub-integration maps
// //       COMPLETE_SCAN_UNMAPPED:
// //       - casacore::Sort by OBSERVATION_ID, ARRAY_ID, SCAN_NUMBER, FIELD_ID, DATA_DESC_ID and TIME
// //       - Group all time steps together, so that there is no sub-chunk iteration
// //       * Don't generate baseline maps
// //       * Don't generate sub-integration maps
// //       COMBINE_SCANS_MAPPED:
// //       - casacore::Sort by OBSERVATION_ID, ARRAY_ID, FIELD_ID, DATA_DESC_ID and TIME
// //       - Group all time steps together, so that there is no sub-chunk iteration
// //       - Generate baseline maps (to iterate trough rows with the same antenna1, antenna2)
// //       - Generate sub-integration maps (to iterate trough rows with the same timestamp)
// //       COMBINE_SCANS_MAP_SUB_INTEGRATIONS_ONLY:
// //       - casacore::Sort by OBSERVATION_ID, ARRAY_ID, FIELD_ID, DATA_DESC_ID and TIME
// //       - Group all time steps together, so that there is no sub-chunk iteration
// //       * Don't generate baseline maps
// //       - Generate sub-integration maps (to iterate trough rows with the same timestamp)
// //       COMBINE_SCANS_MAP_ANTENNA_PAIRS_ONLY:
// //       - casacore::Sort by OBSERVATION_ID, ARRAY_ID, FIELD_ID, DATA_DESC_ID and TIME
// //       - Group all time steps together, so that there is no sub-chunk iteration
// //       - Generate baseline maps (to iterate trough rows with the same antenna1, antenna2)
// //       * Don't generate sub-integration maps
// //       COMBINE_SCANS_UNMAPPED:
// //       - casacore::Sort by OBSERVATION_ID, ARRAY_ID, FIELD_ID, DATA_DESC_ID and TIME
// //       - Group all time steps together, so that there is no sub-chunk iteration
// //       * Don't generate baseline maps
// //       * Don't generate sub-integration maps
// //       ANTENNA_PAIR:
// //       - casacore::Sort by OBSERVATION_ID, ARRAY_ID, FIELD_ID, ANTENNA1, ANTENNA2, DATA_DESC_ID and TIME
// //       - Group all time steps together, so that there is no sub-chunk iteration
// //       * Don't generate baseline maps (they are not necessary because the chunks have constant ANTENNA1,ANTENNA2)
// //       * Don't generate sub-integration maps
// //       SUB_INTEGRATION:
// //       - casacore::Sort by OBSERVATION_ID, ARRAY_ID, SCAN_NUMBER, FIELD_ID, DATA_DESC_ID and TIME
// //       - Don't group all time steps together, so it is necessary to add an inner sub-chunk iteration loop
// //       * Don't generate baseline maps (it is not possible because not all the rows corresponding to a given baseline are available)
// //       * Don't generate sub-integration maps (they are not necessary because the sub-chunks have constant TIME)
// //       ARRAY_FIELD:
// //       - casacore::Sort by OBSERVATION_ID, ARRAY_ID, FIELD_ID, DATA_DESC_ID and TIME
// //       - Don't group all time steps together, so it is necessary to add an inner sub-chunk iteration loop
// //       * Don't generate baseline maps (it is not possible because not all the rows corresponding to a given baseline are available)
// //       * Don't generate sub-integration maps (they are not necessary because the sub-chunks have constant TIME)
// //       * NOTE: This is the iteration approach used by the old flagger framework
//
// // Open table
// fdh_p->open();
//
// // Parse data selection to Flag casacore::Data Handler
// fdh_p->setDataSelection(dataSelection);
//
// // Select data (thus creating selected table)
// fdh_p->selectData();
//
// // Create flagging agent and list
// casacore::Record agentConfig;
// agentConfig.define("mode","clip")
// FlagAgentBase *agent = FlagAgentBase::create(fdh_p,agentConfig);
// FlagAgentList agentList;
// agentList.push_back(agent);
//
// // Switch on/off async i/p
// fdh_p->enableAsyncIO(true);
//
// // Generate table iterator
// fdh_p->generateIterator();
//
// // Start Flag Agent
// agentList.start();
//
// // Iterates over chunks (constant column values)
// while (fdh_p->nextChunk())
// {
//    // Iterates over buffers (time steps)
//    while (fdh_p->nextBuffer())
//    {
//       // Apply flags
//       agentList.apply();
//       // Flush flag cube
//       fdh_p->flushFlags();
//    }
//
//    // Print end-of-chunk statistics
//    agentList.chunkSummary();
// }
//
// // Print total stats from each agent
// agentList.msSummary();
//
// // Stop Flag Agent
// agentList.terminate();
// agentList.join();
//
// // Close MS
// fdh_p->close();
//
// // Clear Flag Agent List
// agentList.clear();
//
// </srcblock>
// </example>
//
// <example>
// <srcblock>
//
// // The following code shows the FlagAgent-FlagDataHandler interaction works internally:
//
// // First of all, at construction time, each agent has to communicate to the FlagDataHandler
// // which columns have to be pre-fetched (for async i/o or parallel mode) and what mapping
// // options are necessary

// // ...for instance in the case of FlagAgentShadow we need Ant1,Ant2,UVW,TimeCentroid and PhaseCenter:
// flagDataHandler_p->preLoadColumn(vi::Antenna1);
// flagDataHandler_p->preLoadColumn(vi::Antenna2);
// flagDataHandler_p->preLoadColumn(vi::uvw);
// flagDataHandler_p->preLoadColumn(vi::TimeCentroid);
// flagDataHandler_p->preLoadColumn(vi::PhaseCenter);
//
// // ...and FlagAgentElevation needs to have the antenna pointing information globally available for each chunk:
// flagDataHandler_p->setMapAntennaPointing(true);
//
// // Then, at iteration time, the FlagAgentBase class has access to the VisBuffer held
// // in the FlagDataHandler, in order to retrieve the meta-data columns needed for the
// // data selection engine (agent-level row filtering).
// // NOTE: The VisBuffer is actually held within an auto-pointer wrapper,
// //       thus there is an additional get() involved when accessing it.
//
// if (spwList_p.size())
// {
//    if (!find(spwList_p,visibilityBuffer_p->get()->spectralWindow())) return false;
// }
//
// // The sorting columns used for the iteration are also accessible to optimize the selection engine:
// // (e.g.: If scan is constant check only 1st row)
// if ( (scanList_p.size()>0) and (find(flagDataHandler_p->sortOrder_p,casacore::MS::SCAN_NUMBER)==true) )
// {
//    if (!find(scanList_p,visibilityBuffer_p->get()->scan()[0])) return false;
// }
//
// // Once that chunk/rows are evaluated as eligible for the flagging process
// // by the data selection engine, the previously booked maps at construction
// // time can be access in order to iterate trough the data as desired:
// // e.g.: Baseline (antenna pairs) iteration
// for (myAntennaPairMapIterator=flagDataHandler_p->getAntennaPairMap()->begin();
//      myAntennaPairMapIterator != flagDataHandler_p->getAntennaPairMap()->end();
//      ++myAntennaPairMapIterator)
// {
//    // NOTE: The following code is also encapsulated in the FlagAgentBase::processAntennaPair(casacore::Int antenna1,casacore::Int antenna2) code
//
//    // From the antenna map we can retrieve the rows corresponding to the baseline defined by the antenna pair
//    std::vector<casacore::uInt> baselineRows = (*flagDataHandler_p->getAntennaPairMap())[std::make_pair(antennaPair.first,antennaPair.second)];
//
//    // This rows can be now inserted in the mapper classes (VisMapper and FlagMapper using the CubeView<T> template class)
//    VisMapper visibilitiesMap = VisMapper(expression_p,flagDataHandler_p->getPolarizationMap());
//    FlagMapper flagsMap = FlagMapper(flag_p,visibilitiesMap.getSelectedCorrelations());
//    setVisibilitiesMap(antennaRows,&visibilitiesMap);
//    setFlagsMap(antennaRows,&flagsMap);
// }
//
// // Finally, after flagging time, the FlagAgent can communicate to the FlagDataHandler
// // that the modified FlagCube has to be flushed to disk, this is a small but very important
// // step in order to avoid unnecessary I/O activity when a chunk is not eligible for flagging
// // or the auto-flagging algorithms don't produce any flags.
//
// // If any row was flag, then we have to flush the flagRow
// if (flagRow_p) flagDataHandler_p->flushFlagRow_p = true;
// // If any flag was raised, then we have to flush the flagCube
// if (visBufferFlags_p>0) flagDataHandler_p->flushFlags_p = true;
//
// </srcblock>
// </example>

class FlagDataHandler
{

public:

	enum iteration {

		COMPLETE_SCAN_MAPPED=0,
		COMPLETE_SCAN_MAP_SUB_INTEGRATIONS_ONLY,
		COMPLETE_SCAN_MAP_ANTENNA_PAIRS_ONLY,
		COMPLETE_SCAN_UNMAPPED,
		COMBINE_SCANS_MAPPED,
		COMBINE_SCANS_MAP_SUB_INTEGRATIONS_ONLY,
		COMBINE_SCANS_MAP_ANTENNA_PAIRS_ONLY,
		COMBINE_SCANS_UNMAPPED,
		ANTENNA_PAIR,
		SUB_INTEGRATION,
		ARRAY_FIELD
	};

	enum tableType {

		MEASUREMENT_SET=0,
		CALIBRATION_TABLE
	};

	// Default constructor
	// NOTE: casacore::Time interval 0 groups all time steps together in one chunk.
	FlagDataHandler(string msname, casacore::uShort iterationApproach = SUB_INTEGRATION, casacore::Double timeInterval = 0);

	// Default destructor
	virtual ~FlagDataHandler();

	// Common casacore::MS/CalTables public interface
	virtual bool open() {return false;}
	virtual bool close() {return false;}
	virtual bool selectData() {return false;}
	virtual bool generateIterator() {return false;}
	virtual bool nextChunk() {return false;}
	virtual bool nextBuffer() {return false;}
	virtual bool flushFlags() {return false;}
	virtual casacore::String getTableName() {return casacore::String("none");}
	virtual bool parseExpression(casacore::MSSelection &/*parser*/) {return true;}
	virtual bool checkIfColumnExists(casacore::String /*column*/) {return true;}
	virtual bool checkIfSourceModelColumnExists() {return false;}
	virtual bool summarySignal() {return true;}

	// Set the iteration approach
	void setIterationApproach(casacore::uShort iterationApproach);

	// Set casacore::Data Selection parameters
	bool setDataSelection(casacore::Record record);

	// Set time interval (also known as ntime)
	void setTimeInterval(casacore::Double timeInterval);

	// Methods to switch on/off async i/o
	void enableAsyncIO(casacore::Bool enable);

	// Pre-Load columns (in order to avoid parallelism problems when not using
	// async i/o, and also to know what columns to pre-fetch in async i/o mode)
	void preLoadColumn(VisBufferComponent2 column);
	void preFetchColumns();

	// Stop iterating
	void stopIteration() {stopIteration_p = true;};

	// As requested by Urvashi R.V. provide access to the original and modified flag cubes
	casacore::Cube<casacore::Bool> * getModifiedFlagCube() {return &modifiedFlagCube_p;}
	casacore::Cube<casacore::Bool> * getOriginalFlagCube() {return &originalFlagCube_p;}
	casacore::Vector<casacore::Bool> * getModifiedFlagRow() {return &modifiedFlagRow_p;}
	casacore::Vector<casacore::Bool> * getOriginalFlagRow() {return &originalFlagRow_p;}

	// Functions to switch on/off mapping functions
	void setMapAntennaPairs(bool activated);
	void setMapSubIntegrations(bool activated);
	void setMapPolarizations(bool activated);
	void setMapAntennaPointing(bool activated);
	void setScanStartStopMap(bool activated);
	void setScanStartStopFlaggedMap(bool activated);
    void setTimeAverageIter(bool activated);
    void setChanAverageIter(casacore::Vector<casacore::Int> chanbin);

	// Accessors for the mapping functions
	antennaPairMap * getAntennaPairMap() {return antennaPairMap_p;}
	subIntegrationMap * getSubIntegrationMap() {return subIntegrationMap_p;}
	polarizationMap * getPolarizationMap() {return polarizationMap_p;}
	polarizationIndexMap * getPolarizationIndexMap() {return polarizationIndexMap_p;}
	antennaPointingMap * getMapAntennaPointing() {return antennaPointingMap_p;}
	scanStartStopMap * getMapScanStartStop() {return scanStartStopMap_p;}
	lambdaMap * getLambdaMap() {return lambdaMap_p;}

	void setProfiling(casacore::Bool value) {profiling_p=value;}

	// Get a casacore::Float visCube and return a casacore::Complex one
	casacore::Cube<casacore::Complex>& weightVisCube();
	casacore::Cube<casacore::Complex> weight_spectrum_p;

	// Make the logger public to that we can use it from FlagAgentBase::create
	casacore::LogIO *logger_p;

	// Measurement set section
	casacore::String tablename_p;
	casacore::MSSelection *measurementSetSelection_p;
	casacore::Vector<casacore::String> *antennaNames_p;
	std::map< string, std::pair<casacore::Int,casacore::Int> > baselineToAnt1Ant2_p;
	std::map< std::pair<casacore::Int,casacore::Int>, string > Ant1Ant2ToBaseline_p;
	casacore::ScalarMeasColumn<casacore::MPosition> *antennaPositions_p;
	casacore::Vector<casacore::Double> *antennaDiameters_p;
	casacore::Vector<casacore::String> *fieldNames_p;
	std::vector<casacore::String> *corrProducts_p;

	// RO Visibility Iterator
	VisBufferComponents2 *prefetchColumns_p;
	// Iteration counters
	size_t processedRows_p;
	size_t chunkNo_p;
	size_t bufferNo_p;

	// FlagDataHanler-FlagAgents interaction
	bool flushFlags_p;
	bool flushFlagRow_p;
	casacore::uInt64 chunkCounts_p;
	casacore::uInt64 progressCounts_p;
	casacore::uInt64 msCounts_p;
	bool printChunkSummary_p;

	// control report/logging frequency for the per-agent partial (ongoing) summaries
	casacore::uInt summaryThreshold_p;
	const casacore::uInt summaryThresholdInc_p = 10;
	// control report/logging frequency for the per-chunk progress report lines
	casacore::uInt chunkLineThreshold_p;
	const casacore::uInt chunkLineThresholdInc_p = 10;

	casacore::uShort tableTye_p;
	casacore::Bool loadProcessorTable_p;

	// PROCESSOR sub-table section
	casacore::Vector<bool> isCorrelatorType_p;
	bool processorTableExist_p;


	// Visibility Buffer
	// WARNING: The attach mechanism only works with pointers or
	// referenced variables. Otherwise the VisBuffer is created
	// and attached, but when it is assigned to the member it is
	// detached because of the dynamically called destructor
	vi::VisBuffer2 *visibilityBuffer_p;

	// Vis buffer characteristics (constant values)
	bool groupTimeSteps_p;
	casacore::Block<int> sortOrder_p;

    // casacore::Time average iterator parameters
    casacore::Bool enableTimeAvg_p;
    casacore::Bool enableChanAvg_p;
    casacore::Double timeAverageBin_p;
    casacore::Vector <casacore::Int> chanAverageBin_p;
    casacore::String dataColumnType_p;
    vi::AveragingOptions timeAvgOptions_p;
    casacore::Record chanAvgOptions_p;

protected:

	// Common casacore::MS/CalTables private interface
	virtual void generateAntennaPairMap();
	virtual void generateSubIntegrationMap();
	virtual void generatePolarizationsMap();
	virtual void generateAntennaPointingMap();
	virtual void generateScanStartStopMap();

	// casacore::Data Selection ranges
	bool anySelection_p;
	bool inrowSelection_p;
	casacore::String arraySelection_p;
	casacore::String fieldSelection_p;
	casacore::String scanSelection_p;
	casacore::String timeSelection_p;
	casacore::String spwSelection_p;
	casacore::String baselineSelection_p;
	casacore::String uvwSelection_p;
	casacore::String polarizationSelection_p;
	casacore::String scanIntentSelection_p;
	casacore::String observationSelection_p;

	// Async I/O stuff
	bool asyncio_enabled_p;
	// Pre-Load columns (in order to avoid parallelism problems when not using
	// async i/o, and also to know what columns to pre-fetch in async i/o mode)
    std::vector<VisBufferComponent2> preLoadColumns_p;

	// Iteration parameters
	casacore::uShort iterationApproach_p;
	casacore::Double timeInterval_p;
	// Slurp flag
	bool slurp_p;
	// Iteration initialization parameters
	bool chunksInitialized_p;
	bool buffersInitialized_p;
	bool iteratorGenerated_p;
	bool stopIteration_p;

	// Flag Cubes
	casacore::Cube<casacore::Bool> originalFlagCube_p;
	casacore::Cube<casacore::Bool> modifiedFlagCube_p;

	// FlagRows
	casacore::Vector<casacore::Bool> originalFlagRow_p;
	casacore::Vector<casacore::Bool> modifiedFlagRow_p;

	// Mapping members
	antennaPairMap *antennaPairMap_p;
	subIntegrationMap *subIntegrationMap_p;
	polarizationMap *polarizationMap_p;
	polarizationIndexMap *polarizationIndexMap_p;
	antennaPointingMap *antennaPointingMap_p;
	scanStartStopMap *scanStartStopMap_p;
	lambdaMap *lambdaMap_p;
	bool mapAntennaPairs_p;
	bool mapSubIntegrations_p;
	bool mapPolarizations_p;
	bool mapAntennaPointing_p;
	bool mapScanStartStop_p;
	bool mapScanStartStopFlagged_p;

	// Stats members
	bool stats_p;
	casacore::uLong cubeAccessCounter_p;
	double cubeAccessTime_p;
	casacore::uLong cubeAccessCounterTotal_p;
	double cubeAccessTimeTotal_p;

	// Profiling
	bool profiling_p;
};

} //# NAMESPACE CASA - END

#endif /* FLAGDATAHANDLER_H_ */
