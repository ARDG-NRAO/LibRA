//# --------------------------------------------------------------------
//# LineFindingUtils.h: this defines utility functions of line finding
//# --------------------------------------------------------------------
//# Copyright (C) 2015
//# National Astronomical Observatory of Japan
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
//# $Id$
#ifndef _CASA_LINE_FINDING_UTILS_H_
#define _CASA_LINE_FINDING_UTILS_H_

#include <iostream>
#include <string>
#include <list>
#include <sstream>

#include <casa/aipstype.h>
#include <casa/Arrays/Vector.h>

namespace casa { //# NAMESPACE CASA - BEGIN

class LineFinderUtils {
public:
	/*
	 * Bin data and mask arrays.
	 *
	 * @param[in] num_in : the number of elements in in_data and in_mask
	 * @param[in] in_data : an input data array to bin
	 * @param[in] in_mask : an input mask array to bin
	 * @param[in] bin_size : the number of channels to bin
	 * @param[in] num_out : the number of elements in out_data and out_mask
	 * @param[out] out_data : an array to store binned data
	 * @param[out] out_mask : an array to store binned mask
	 * @param[in] offset : the channel index in in_data and in_mask
	 *         to start binning. offset should be 0 if keepsize=false.
	 * @param[in] keepsize : if true, keep array size after binning.
	 *         otherwise, the output arrays will have (num_in-offset+1)/bin_size channel.
	 *         num_out should be equal to num_in when keepsize=true.
	 */
	template<typename DataType>
	static size_t binDataAndMask(size_t const num_in,
			DataType const in_data[/*num_in*/], bool const in_mask[/*num_in*/],
			size_t const bin_size, size_t const num_out,
			DataType out_data[/*num_out*/], bool out_mask[/*num_out*/],
			size_t const offset = 0, bool const keepsize = false);

	/*
	 * Calculate median absolute deviation (MAD) of a data array
	 * Processes mad[i] = data[i] - (median of valid elements in data)
	 *
	 * @param[in] num_data : the number of elements in @a in_data ,
	 *         @a in_mask , and @a mad.
	 * @param[in] in_data : data array to calculate MAD
	 * @param[in] in_mask : a boolean mask array. the elements
	 *         with mask=false will be ignored in calculating median.
	 * @param[out] mad : an array of MAD values
	 */
	static void calculateMAD(size_t const num_data,
			float const in_data[/*num_data*/], bool const in_mask[/*num_data*/],
			float mad[/*num_data*/]);

	/*
	 * Count the number of true elements in a boolean array.
	 *
	 * @param[in] num_data : the number of elements in @a data
	 * @param[in] data : a boolean array
	 * @return the number of elements with true in @a data
	 */
	inline static size_t countTrue(size_t num_data,
			bool const data[/*num_data*/]) {
		size_t ntrue = 0;
		static_assert(static_cast<uint8_t>(true)==1, "cast of bool failed");
		static_assert(static_cast<uint8_t>(false)==0, "cast of bool failed");
		uint8_t const *data8 = reinterpret_cast<uint8_t const *>(data);
		static_assert(sizeof(data[0])==sizeof(data8[0]),
				"bool and uint8_t has different size");
		for (size_t i = 0; i < num_data; ++i) {
			ntrue += data8[i];
		}
		return ntrue;
	}
	;

	/*
	 * create mask by a threshold value
	 * out_mask[i] = in_mask[i] && (in_data[i] >= threshold)
	 *
	 * @param[in] num_data : the number of elements in @a in_data ,
	 *         @a in_mask , and @a out_mask
	 * @param[in] in_data : a data array
	 * @param[in] in_mask : a boolean mask array
	 * @param[in] threshold : a threshold value to compare with @a in_data
	 * @param[out] out_mask : an output mask array
	 */
	static void createMaskByAThreshold(size_t const num_data,
			float const in_data[/*num_data*/], bool const in_mask[/*num_data*/],
			float const threshold, bool out_mask[/*num_data*/]);
	/*
	 create mask by threshold value array
	 out_mask[i] = in_mask[i] && (in_data[i] >= threshold_array[i])
	 */
	/*     template <typename DataType>  */
	/*     static void createMaskByThresholds(size_t const num_data, */
	/* 				       DataType const in_data[/\*num_data*\/], */
	/* 				       bool const in_mask[/\*num_data*\/], */
	/* 				       DataType const threshold_array[/\*num_data*\/], */
	/* 				       bool out_mask[/\*num_data*\/]); */

	/*
	 * create sign array by a threshold value
	 * sign[i] = +1 (in_data[i] > threshold)
	 *         = 0  (in_data[i] == threshold)
	 *         = -1 (in_data[i] < threshold)
	 *
	 * @param[in] num_data : the number of elements of @a in_data and @a sign
	 * @param[in] in_data : an input data array to calculate sign
	 * @param[in] threshold : a threshold to calculate sign
	 * @param[out] sign : an output array
	 */
	template<typename DataType>
	inline static void createSignByAThreshold(size_t const num_data,
			DataType const in_data[/*num_data*/], DataType const threshold,
			int8_t sign[/*num_data*/]) {
		for (size_t i = 0; i < num_data; ++i) {
			sign[i] = signCompare(in_data[i], threshold);
		}
	}
	;

	/*
	 * create sign array by data and threshold arrays
	 * sign[i] = +1 (in_data[i] > threshold_array[i])
	 *         = 0  (in_data[i] == threshold_array[i])
	 *         = -1 (in_data[i] < threshold_array[i])
	 *
	 * @param[in] num_data : the number of elements of @a in_data ,
	 *         @a threshod_array, and @a sign
	 * @param[in] in_data : an input data array to calculate sign
	 * @param[in] threshold_array : a threshold array to calculate sign
	 * @param[out] sign : an output array
	 */
	template<typename DataType>
	inline static void createSignByThresholds(size_t const num_data,
			DataType const in_data[/*num_data*/],
			DataType const threshold_array[/*num_data*/],
			int8_t sign[/*num_data*/]) {
		for (size_t i = 0; i < num_data; ++i) {
			sign[i] = signCompare(in_data[i], threshold_array[i]);
		}
	}
	;

	/*
	 * Convert channel idxs of line ranges of binned array to the ones before binning
	 *
	 * @param[in] bin_size : the number of channels binned
	 * @param[in] offset : the offset channel idx
	 * @param[in,out] range_list : the list of line channel ranges to convert
	 */
	static void deBinRanges(size_t const bin_size, size_t const offset,
			std::list<std::pair<size_t, size_t>>& range_list);

	/*
	 * Extend for line wing
	 * Line ranges are extended while sign has the same value as the range
	 * AND mask=true.
	 *
	 * @param[in] num_sign : the number of elements in sign and mask array
	 * @param[in] sign : an array with the values either -1, 0, or 1.
	 *         It indicates how far a line could be extended.
	 * @param[in] mask : a boolean mask. The line range will be truncated
	 *         at the channel, mask=false.
	 * @param[in,out] range_list : a list of line channel ranges
	 */
	static void extendRangeBySign(size_t num_sign,
			int8_t const sign[/*num_sign*/], bool const mask[/*num_sign*/],
			std::list<std::pair<size_t, size_t>>& range_list);

	/*
	 * Get median of an sorted array.
	 * @param[in] num_data: the number of elements from which median is calculated
	 * @param[in] data: values should be sorted in ascending order.
	 * @param[in] Must neighther have Inf nor Nan in elements 0-num_data-1.
	 *
	 * When the number of elements is odd the function returns data[num_data/2].
	 * Otherwise, average of middle two elements, i.e, (data[num_data/2]+data[num_data/2-1])/2
	 */
	template<typename DataType>
	inline static DataType getMedianOfSorted(size_t const num_data,
			DataType const data[/*num_data*/]) {
		return (data[num_data / 2] + data[num_data / 2 - ((num_data + 1) % 2)])
				/ static_cast<DataType>(2);
	}
	;

	/*
	 * Get median of an array taking mask into account.
	 *
	 * @param[in] num_data : the number of elements in data and mask arrays
	 * @param[in] data : data array to calculate median from
	 * @param[in] mask : mask array. Only elements with mask[i]=true is
	 *         taken into account to calculate median
	 * @param[in] fraction : a fraction of valid channels to calculate median
	 *         lower data are used.
	 */
	static float maskedMedian(size_t num_data, float const* data,
			bool const mask[/*num_data*/], float fraction = 1.0);

	/*
	 * Convert boolean channel mask to channel index ranges.
	 * For example:
	 *    mask = {F, T, T, F, T} -> [[1,2], [4,4]]
	 * @param[in] num_mask : the number of elements in mask
	 * @param[in] mask : a boolean array of mask
	 * @param[out] out_range : a list of line channel range
	 */
	static void maskToRangesList(size_t const num_mask,
			bool const mask[/*num_mask*/],
			std::list<std::pair<size_t, size_t>>& out_range);

	/*
	 * Merge line ranges if the ranges are separated only by flagged
	 * channels, i.e., mask=false.
	 *
	 * @param[in] num_mask : the number of elements in mask
	 * @param[in] mask : a boolean array to store mask
	 * @param[in] maxgap : the maximum separation of line channels to merge
	 * @param[in,out] range_list : a list of line channel ranges
	 *
	 */
	static void mergeGapByFalse(size_t const num_mask,
			bool const mask[/*num_mask*/], size_t const maxgap,
			std::list<std::pair<size_t, size_t>>& range_list);

	/*
	 * Merge line ranges if the ranges are overlapped.
	 *
	 * @param[in] range_list : a list of line range to modify
	 *         The list should be sorted in ascending order of idx
	 *         for both for first and second elements of pairs.
	 */
	static void mergeOverlappingRanges(
			std::list<std::pair<size_t, size_t>>& range_list);

	/*
	 * Merge line ranges if the ranges two lists are overlapped.
	 *
	 * @param[in,out] to : a list of line range to merge
	 * @param[in] from : a list of line range to merge
	 * Both lists should be sorted in ascending order of idx
	 * for both for first and second elements of pairs.
	 */
	static void mergeOverlapInTwoLists(std::list<std::pair<size_t, size_t>>& to,
			std::list<std::pair<size_t, size_t>>& from);

	/*
	 * Merge line ranges if the gap between lines are smaller than
	 * a certain fraction of narrower line.
	 * @param[in] fraction : the fraction to narrower line
	 * @param[in] maxwidth :
	 * @param[in] range_list : a list of line range to modify
	 *         The list should be sorted in ascending order of idx
	 *         for both for first and second elements of pairs.
	 */
	static void mergeSmallGapByFraction(double const fraction,
			size_t const maxwidth,
			std::list<std::pair<size_t, size_t>>& range_list);

	/*
	 * Remove line ranges larger than maxwidth from the list.
	 *
	 * @param[in] maxwidth : maximum line width allowed
	 * @param[in] range_list : a list of line range to test
	 */
	static void rejectWideRange(size_t const maxwidth,
			std::list<std::pair<size_t, size_t>>& range_list);

	/*
	 * Remove line ranges smaller than minwidth from the list.
	 *
	 * @param[in] minwidth : minimum line width allowed
	 * @param[in] range_list : a list of line range to test
	 */
	static void rejectNarrowRange(size_t const minwidth,
			std::list<std::pair<size_t, size_t>>& range_list);
	/*     template <typename DataType> */
	/*       static void splitLines(DataType const& in_data, bool const& in_mask, */
	/* 			     std::list<std::pair<size_t,size_t>> const& in_range, */
	/* 			     std::list<std::pair<size_t,size_t>>& out_range); */

	inline static std::string FormatLineString(std::list<std::pair<size_t,size_t>> &line_list) {
	  std::ostringstream oss;
	  oss << "number of Lines = " << line_list.size() << std::endl;
	  for (std::list<std::pair<size_t,size_t>>::iterator iter = line_list.begin();
	       iter!=line_list.end(); ++iter) {
	    oss << "- [ " << (*iter).first << ", " << (*iter).second << " ] (width: "
	        << (*iter).second-(*iter).first+1 << ")" << std::endl;
	  }
	  return oss.str();
	};

private:
	/*
	 * Merge a line range to a list of line range taking into account the overlap.
	 */
	static size_t mergeARangeToList(
			std::list<std::pair<size_t, size_t>>& range_list,
			std::pair<size_t, size_t>& new_range, size_t const cursor = 0);

	/*
	 * Compare data and threshold and returns
	 * 1 if data > threshold
	 * 0 if data == threshold
	 * -1 if data < threshold
	 */
	template<typename DataType>
	inline static int8_t signCompare(DataType const& data,
			DataType const& threshold) {
		if (data > threshold)
			return 1;
		else if (data < threshold)
			return -1;
		else
			return 0;
	}
	;

};

} //# NAMESPACE CASA - END

#endif /* _CASA_LINE_FINDING_UTILS_H_ */
