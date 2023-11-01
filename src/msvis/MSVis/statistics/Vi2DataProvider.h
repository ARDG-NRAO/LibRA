// -*- mode: c++ -*-
//# Copyright (C) 1996,1997,1998,1999,2000,2002,2003,2015
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
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//
// Base class of data providers based on casacore::StatsDataProvider, backed by a
// VisibilityIterator2 instances.
//
#ifndef MSVIS_STATISTICS_VI2_DATA_PROVIDER_H_
#define MSVIS_STATISTICS_VI2_DATA_PROVIDER_H_

#include <casacore/casa/aips.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/ms/MeasurementSets/MSMainEnums.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <msvis/MSVis/VisBufferComponents2.h>
#include <msvis/MSVis/statistics/Vi2StatsFlagsIterator.h>
#include <msvis/MSVis/statistics/Vi2StatsWeightsIterator.h>
#include <msvis/MSVis/statistics/Vi2StatsSigmasIterator.h>
#include <msvis/MSVis/statistics/Vi2StatisticsIteratee.h>
#include <casacore/scimath/StatsFramework/StatisticsAlgorithm.h>
#include <casacore/scimath/StatsFramework/StatsDataProvider.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <cassert>

namespace casa {

// casacore::StatsDataProvider template class backed by VisibilityIterator2
// instances. These data providers operate on a single casacore::MS column over
// all vi2 sub-chunks in a dataset composed of one or more chunks selected when
// the data provider is instantiated. In other words, the data sample for
// statistics generated with a Vi2DataProvider instance is the data from a
// single column in a single dataset of consecutive vi2 chunks. It is intended
// that the user set up the VisibilityIterator2 appropriately to select the
// desired data sample for computing statistics. Consecutive chunks produced by
// an iterator may be merged by the data provider to produce a single
// dataset. The iteration over an MS, and the computation of statistics on each
// dataset driven using a Vi2StatisticsIteratee instance, may be done as
// follows:
//
// Vi2DataProvider *dp; // given
// casacore::StatisticsAlgorithm statistics; // given
// class DoStuff : public Vi2StatisticsIteratee {
//   ... // constructor, probably needs some sort of accumulator
//   void nextDataset(casacore::StatisticsAlgorithm stats,
//                    const std::unordered_map<int,std::string> *colVals) {
//     stats.getStatistics()...;
//   }
// }
// DoStuff doStuff;
// dp->foreachDataset(statistics, doStuff);
//
// Note that the AccumType template parameter value of
// casacore::StatsDataProvider is derived from the DataIterator parameter value
// of this template, imposing a constraint on the classes that can be used for
// DataIterator.
//
template <class DataIterator, class MaskIterator, class WeightsIterator>
class Vi2DataProvider
	: public casacore::StatsDataProvider<typename DataIterator::AccumType,
	                                     DataIterator,
	                                     MaskIterator,
	                                     WeightsIterator> {

public:
	// Define typedefs for some template parameters. This is primarily to
	// support instantiating "...Statistics" instances of the proper type given
	// only an instance of this type. [AccumType is an exception, in that it's
	// also needed by the (macro) definition of DataRanges.]
	typedef typename DataIterator::AccumType AccumType;
	typedef DataIterator DataIteratorType;
	typedef MaskIterator MaskIteratorType;
	typedef WeightsIterator WeightsIteratorType;

	Vi2DataProvider(
		vi::VisibilityIterator2 *vi2,
		const std::set<casacore::MSMainEnums::PredefinedColumns>
		    &mergedColumns_,
		vi::VisBufferComponent2 component,
		casacore::Bool omit_flagged_data,
		casacore::Bool use_data_weights)
		: vi2(vi2)
		, component(component)
		, use_data_weights(use_data_weights)
		, omit_flagged_data(omit_flagged_data) {

		// Attach the casacore::MS columns to vi2 by calling originChunks(). Not
		// the most direct method, but attaching the columns is required in many
		// cases to pass existsColumn() test.
		vi2->originChunks();
		if (!vi2->existsColumn(component))
			throw casacore::AipsError("Column is not present");

		for (auto &&i : mergedColumns_)
			mergedColumns.insert(columnNames.at(i));
	}

	Vi2DataProvider(Vi2DataProvider&& other)
		: vi2(other.vi2)
		, mergedColumns(other.mergedColumns)
		, datasetIndex(other.datasetIndex)
		, datasetChunkOrigin(other.datasetChunkOrigin)
		, followingChunkDatasetIndex(other.followingChunkDatasetIndex)
		, currentChunk(other.currentChunk)
		, component(other.component)
		, use_data_weights(other.use_data_weights)
		, omit_flagged_data(other.omit_flagged_data)
		, data_iterator(other.data_iterator)
		, weights_iterator(other.weights_iterator)
		, mask_iterator(other.mask_iterator) {
		other.vi2 = nullptr;
	}

	Vi2DataProvider& operator=(Vi2DataProvider&& other) {
		vi2 = other.vi2;
		mergedColumns = other.mergedColumns;
		datasetIndex = other.datasetIndex;
		datasetChunkOrigin = other.datasetChunkOrigin;
		followingChunkDatasetIndex = other.followingChunkDatasetIndex;
		currentChunk = other.currentChunk;
		component = other.component;
		use_data_weights = other.use_data_weights;
		omit_flagged_data = other.omit_flagged_data;
		data_iterator = other.data_iterator;
		weights_iterator = other.weights_iterator;
		mask_iterator = other.mask_iterator;
		other.vi2 = nullptr;
		return *this;
	}

	// Increment the data provider to the sub-chunk within the dataset.
	void operator++() {
		if (atEnd())
			throw casacore::AipsError(
				"Data provider increment beyond end of dataset");
		vi2->next();
		reset_iterators();
		if (!vi2->more() && followingChunkDatasetIndex == datasetIndex)
			nextDatasetChunk();
	}

	// Is this the last sub-chunk within the dataset?
	casacore::Bool atEnd() const {
		return followingChunkDatasetIndex != datasetIndex && !vi2->more();
	}

	// Take any actions necessary to finalize the provider. This will be called
	// when atEnd() returns true.
	void finalize() {};

	casacore::uInt64 getCount() {
		return getData().getCount();
	}

	// Get the current sub-chunk
	DataIterator getData() {
		if (!data_iterator)
			data_iterator =
				std::unique_ptr<DataIterator>(new DataIterator(dataArray()));
		return *data_iterator;
	}

	casacore::uInt getStride() {
		return 1;
	};

	casacore::Bool hasMask() const {
		return omit_flagged_data;
	};

	// Get the associated mask of the current sub-chunk. Only called if
	// hasMask() returns true.
	MaskIterator getMask() {
		if (!mask_iterator)
			mask_iterator = std::unique_ptr<MaskIterator>(
				new MaskIterator(vi2->getVisBuffer()));
		return *mask_iterator;
	};

	// Get the stride for the current mask (only called if hasMask() returns
	// true). Will always return 1 in this implementation.
	casacore::uInt getMaskStride() {
		return 1;
	};

	casacore::Bool hasWeights() const {
		return use_data_weights;
	};

	// Get the associated weights of the current sub-chunk. Only called if
	// hasWeights() returns true;
	WeightsIterator getWeights() {
		if (!weights_iterator)
			weights_iterator = std::unique_ptr<WeightsIterator>(
				new WeightsIterator(vi2->getVisBuffer()));
		return *weights_iterator;
	};

	casacore::Bool hasRanges() const {
		return false; // TODO: is this correct in all cases?
	};

	// Get the associated range(s) of the current sub-chunk. Only called if
	// hasRanges() returns true, which will never be the case in this
	// implementation.
	DataRanges
	getRanges() {
		DataRanges result;
		return result;
	};

	// If the associated data set has ranges, are these include (return true) or
	// exclude (return false) ranges?
	casacore::Bool isInclude() const {
		return false;
	};

	// Reset the provider to point to the first sub-chunk of the dataset.
	void reset() {
		if (currentChunk != datasetChunkOrigin) {
			vi2->originChunks();
			currentChunk = 0;
			initChunk();
			updateFollowingChunkDatasetIndex();
			while (currentChunk != datasetChunkOrigin)
				nextDatasetChunk();
		} else {
			initChunk();
			updateFollowingChunkDatasetIndex();
		}
	}

	void foreachDataset(
		casacore::StatisticsAlgorithm<AccumType,DataIteratorType,MaskIteratorType,WeightsIteratorType>& statistics,
		Vi2StatisticsIteratee<DataIterator,WeightsIterator,MaskIterator>& iteratee) {

		datasetIndex = -1;
		followingChunkDatasetIndex = 0;
		while (nextDataset()) {
			std::unique_ptr<std::unordered_map<int,std::string> >
				columnValues(mkColumnValues());
			statistics.setDataProvider(this);
			iteratee.nextDataset(statistics, columnValues.get());
		};
	}

protected:

	std::unique_ptr<vi::VisibilityIterator2> vi2;

	vi::VisBufferComponent2 component;

	std::unique_ptr<const DataIterator> data_iterator;

	const casacore::Bool use_data_weights;

	std::unique_ptr<const WeightsIterator> weights_iterator;

	const casacore::Bool omit_flagged_data;

	std::unique_ptr<const MaskIterator> mask_iterator;

	int datasetIndex;

	unsigned datasetChunkOrigin;

	int followingChunkDatasetIndex;

	unsigned currentChunk;

	std::unordered_set<string> mergedColumns;

private:

	void
	reset_iterators() {
		data_iterator.reset();
		weights_iterator.reset();
		mask_iterator.reset();
	}

	void
	nextDatasetChunk() {
		vi2->nextChunk();
		++currentChunk;
		if (vi2->moreChunks())
			initChunk();
		updateFollowingChunkDatasetIndex();
	}

	bool
	nextDataset() {
		++datasetIndex;
		assert(followingChunkDatasetIndex == datasetIndex);
		if (datasetIndex == 0) {
			vi2->originChunks();
			currentChunk = 0;

		} else {
			vi2->nextChunk();
			++currentChunk;
		}
		if (vi2->moreChunks())
			initChunk();
		updateFollowingChunkDatasetIndex();
		datasetChunkOrigin = currentChunk;
		return vi2->moreChunks();
	}

	void
	initChunk() {
		vi2->origin();
		reset_iterators();
	}

	virtual const casacore::Array<typename DataIterator::DataType>& dataArray() = 0;

	void
	updateFollowingChunkDatasetIndex() {
		if (!vi2->moreChunks() || mergedColumns.count(vi2->keyChange()) == 0)
			followingChunkDatasetIndex = datasetIndex + 1;
		else
			followingChunkDatasetIndex = datasetIndex;
	}

	std::unique_ptr<std::unordered_map<int,std::string> >
	mkColumnValues() {
		const vi::VisBuffer2 *vb = vi2->getVisBuffer();
		return std::unique_ptr<std::unordered_map<int,std::string> >(
			new std::unordered_map<int,std::string> {
				{casacore::MSMainEnums::PredefinedColumns::ARRAY_ID,
						"ARRAY_ID=" + std::to_string(vb->arrayId()[0])},
				{casacore::MSMainEnums::PredefinedColumns::FIELD_ID,
						"FIELD_ID=" + std::to_string(vb->fieldId()[0])},
				{casacore::MSMainEnums::PredefinedColumns::DATA_DESC_ID,
						"DATA_DESC_ID="
						+ std::to_string(vb->dataDescriptionIds()[0])},
				{casacore::MSMainEnums::PredefinedColumns::TIME,
						"TIME="
						+ std::to_string(vb->time()[0] - vb->timeInterval()[0] / 2)},
				{casacore::MSMainEnums::PredefinedColumns::SCAN_NUMBER,
						"SCAN_NUMBER=" + std::to_string(vb->scan()[0])},
				{casacore::MSMainEnums::PredefinedColumns::STATE_ID,
						"STATE_ID=" + std::to_string(vb->stateId()[0])}
			});
	}

	std::map<casacore::MSMainEnums::PredefinedColumns,std::string> columnNames = {
		{casacore::MSMainEnums::PredefinedColumns::ARRAY_ID, "ARRAY_ID"},
		{casacore::MSMainEnums::PredefinedColumns::FIELD_ID, "FIELD_ID"},
		{casacore::MSMainEnums::PredefinedColumns::DATA_DESC_ID, "DATA_DESC_ID"},
		{casacore::MSMainEnums::PredefinedColumns::TIME, "TIME"}};
};

// casacore::Data provider template for row-based casacore::MS columns (i.e, not visibilities) using
// the 'weights' column for data weights. In most instances, you would not
// weight the data in these columns, but the Vi2DataProvider template
// requires that a WeightsIterator be provided.
template<class DataIterator>
using Vi2WeightsRowDataProvider =
	Vi2DataProvider<
	DataIterator,Vi2StatsFlagsRowIterator,Vi2StatsWeightsRowIterator>;

// casacore::Data provider template for row-based casacore::MS columns (i.e, not visibilities) using
// the 'sigma' column for data weights (appropriately transformed). In most
// instances, you would not weight the data in these columns, but the
// Vi2DataProvider template requires that a WeightsIterator be provided.
template<class DataIterator>
using Vi2SigmasRowDataProvider =
	Vi2DataProvider<
	DataIterator,Vi2StatsFlagsRowIterator,Vi2StatsSigmasRowIterator>;

// casacore::Data provider template for cube-based casacore::MS columns (i.e, the visibilities)
// using the 'weights' column for data weights.
template<class DataIterator>
using Vi2WeightsCubeDataProvider =
	Vi2DataProvider<
	DataIterator,Vi2StatsFlagsCubeIterator,Vi2StatsWeightsCubeIterator>;

// casacore::Data provider template for cube-based casacore::MS columns (i.e, the visibilities)
// using the 'sigma' column for data weights (appropriately transformed).
template<class DataIterator>
using Vi2SigmasCubeDataProvider =
	Vi2DataProvider<
	DataIterator,Vi2StatsFlagsCubeIterator,Vi2StatsSigmasCubeIterator>;

} // end namespace casa

#endif // MSVIS_STATISTICS_VI2_DATA_PROVIDER_H_
