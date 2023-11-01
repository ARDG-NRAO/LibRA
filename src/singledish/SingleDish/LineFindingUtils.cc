//# --------------------------------------------------------------------
//# LineFindingUtils.tcc: this defines utility functions of line finding
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
#include <algorithm>
#include <casacore/casa/Utilities/Assert.h>
#include <cmath>

#include <libsakura/sakura.h>
#include <singledish/SingleDish/LineFindingUtils.h>

using namespace std;

using namespace casacore;
using namespace casacore;
using namespace casacore;
using namespace casacore;
using namespace casacore;
using namespace casacore;
using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

template <typename DataType> 
size_t LineFinderUtils::binDataAndMask(size_t const num_in,
				       DataType const* in_data,
				       bool const* in_mask,
				       size_t const bin_size,
				       size_t const num_out,
				       DataType* out_data,
				       bool* out_mask,
				       size_t const offset,
				       bool const keepsize)
{
  if (offset+bin_size > num_in-1) return 0;
  // Can NOT offset (>0) when changing size (keepsize=false)
  AlwaysAssert((!keepsize || offset==0), AipsError);
  size_t num_bin = keepsize ? num_in : (num_in-offset)/bin_size;
  AlwaysAssert(num_bin <= num_out, AipsError);
  size_t num_loc_out = keepsize ? bin_size : 1;
  size_t out_idx = 0;
  for (size_t i=offset; (keepsize ? i : i+bin_size-1) < num_in && out_idx < num_bin;) {
    DataType data = static_cast<DataType>(0);
    bool mask = true;
    size_t iend=i+bin_size;
    size_t count=0;
    for (;i<iend && i < num_in; ++i) {
      //Sum up regardless of in_mask because
      //it will be flagged anyway if any of mask=false.
      data += in_data[i];
      mask = mask && in_mask[i];
      ++count;
    }
    data = mask ? data/static_cast<DataType>(count) : static_cast<DataType>(0);
    for (size_t j = 0; j < num_loc_out && out_idx < num_bin; ++j){
      out_mask[out_idx] = mask;
      out_data[out_idx] = data;
      ++out_idx;
    }
  }
  AlwaysAssert(num_bin==out_idx, AipsError);
  return num_bin;
}

void LineFinderUtils::calculateMAD(size_t const num_data,
				   float const in_data[],
				   bool const in_mask[],
				   float mad[])
{
  float median_value = LineFinderUtils::maskedMedian(num_data, in_data, in_mask, 1.0);
  //cout << "median value for MAD = " << median_value << endl;
  for (size_t i = 0; i < num_data; ++i) {
    mad[i] = fabs(in_data[i]-median_value);
  }
}

void LineFinderUtils::createMaskByAThreshold(size_t const num_data,
					     float const in_data[],
					     bool const in_mask[],
					     float const threshold,
					     bool out_mask[])
{
  LIBSAKURA_SYMBOL(Status) status = LIBSAKURA_SYMBOL(SetTrueIfGreaterThanOrEqualsFloat)(num_data, in_data, threshold, out_mask);
  AlwaysAssert(status == LIBSAKURA_SYMBOL(Status_kOK), AipsError);
  for (size_t i = 0; i < num_data; ++i) {
    out_mask[i] = out_mask[i] && in_mask[i];
  }
}

void LineFinderUtils::deBinRanges(size_t const bin_size, size_t const offset,
			          list<pair<size_t,size_t>>& range_list)
{
  for(list<pair<size_t,size_t>>::iterator iter=range_list.begin();
        iter!=range_list.end(); ++iter) {
    (*iter).first = (*iter).first*bin_size + offset;
    (*iter).second = (*iter).second*bin_size+(bin_size-1) + offset;
    }
}

void LineFinderUtils::extendRangeBySign(size_t num_sign, int8_t const* sign, bool const* mask,
					list<pair<size_t,size_t>>& range_list){
  for (list<pair<size_t,size_t>>::iterator iter = range_list.begin();
       iter!=range_list.end(); ++iter) {
    AlwaysAssert((*iter).first <= (*iter).second, AipsError);
    // extend left wing
    int8_t ref_sign = sign[(*iter).first];
    for (size_t i=(*iter).first-1; ; --i) {
      if (mask[i] && sign[i] == ref_sign) (*iter).first=i;
      else break;
      if (i==0) break;
    }
    // extend right wing
    ref_sign = sign[(*iter).second];
    for (size_t i=(*iter).second+1; i<num_sign; ++i) {
      if (mask[i] && sign[i] == ref_sign) (*iter).second=i;
      else break;
    }
  }
}


void LineFinderUtils::maskToRangesList(size_t const num_mask, bool const* mask,
				       list<pair<size_t,size_t>>& out_range)
{
  static_assert(static_cast<int8_t>(true)==1, "cast of bool failed");
  static_assert(static_cast<int8_t>(false)==0, "cast of bool failed");
  out_range.clear();
  if (num_mask==0) return;
  size_t istart=num_mask;
  if (mask[0]) istart=0;
  for (size_t i=1; i<num_mask; ++i) {
    int8_t diff = static_cast<int8_t>(mask[i]) - static_cast<int8_t>(mask[i-1]);
    if (diff==1) { // start of new line
      AlwaysAssert(istart==num_mask, AipsError);
      istart=i; 
    }
    else if (diff==-1) { // end of line was i-1 chan
      AlwaysAssert(istart<num_mask, AipsError);
      out_range.push_back(pair<size_t,size_t>(istart, i-1));
      istart=num_mask;
    }
  }
  if (mask[num_mask-1]) {
    AlwaysAssert(istart<num_mask, AipsError);
    out_range.push_back(pair<size_t,size_t>(istart, num_mask-1));
  }
}

void LineFinderUtils::mergeGapByFalse(size_t const num_mask, bool const* mask,
				      size_t const maxgap,
				      list<pair<size_t,size_t>>& range_list)
{
  if (range_list.size() < 2) return; // nothing to do
  list<pair<size_t,size_t>>::iterator iter=range_list.begin();
  size_t from=(*iter).second+1;
  list<pair<size_t,size_t>> temp_list;
  temp_list.push_back(*iter);
  ++iter;
  for( ; iter!=range_list.end(); ++iter) {
    size_t to=(*iter).first;
    if (to-from > maxgap) { // do not merge if gap is too large
      temp_list.push_back(*iter);
      continue;
    }
    bool merge=true;
    for (size_t i=from; i<to && i < num_mask; ++i) {
      if (mask[i]) { // do not merge if any of channel in betwee the line 
	merge = false;
	break;
      }
    }
    from=(*iter).second;
    if (merge) {
      temp_list.back().second = (*iter).second;
    }
    else {
      temp_list.push_back(*iter);
    }
  }
  range_list.clear();
  range_list.splice(range_list.end(), temp_list);
}

float LineFinderUtils::maskedMedian(size_t num_data, float const data[],
				     bool const mask[], float fraction)
{

  Vector<float> local_data(IPosition(1, num_data), const_cast<float *>(data), COPY);
//  for (size_t i = 0 ; i < num_data; ++i){
//    local_data.data[i] = data[i];
//  }
  float *local_data_p = local_data.data();
  size_t num_valid(num_data+1);
  LIBSAKURA_SYMBOL(Status) status = LIBSAKURA_SYMBOL(SortValidValuesDenselyFloat)(num_data, mask, local_data_p, &num_valid);
  AlwaysAssert(status == LIBSAKURA_SYMBOL(Status_kOK), AipsError);
  AlwaysAssert(num_valid <= num_data, AipsError);
  if (fraction<1.0)
    num_valid = static_cast<size_t>(num_valid*fraction);
  float median_value = LineFinderUtils::getMedianOfSorted<float>(num_valid,
								 local_data_p);
  return median_value;
  
}

void LineFinderUtils::mergeOverlappingRanges(list<pair<size_t,size_t>>& range_list)
{
  if (range_list.size() < 2) return; // nothing to do
  list<pair<size_t,size_t>> temp_list;
  list<pair<size_t,size_t>>::iterator iter=range_list.begin();
  temp_list.push_back(*iter);
  ++iter;
  for ( ; iter!=range_list.end(); ++iter) {
    // always start searching from the begining of temp_list
    //size_t dummy_cursor = 
    mergeARangeToList(temp_list, (*iter));
  }
  range_list.clear();
  range_list.splice(range_list.end(), temp_list);
}


void LineFinderUtils::mergeOverlapInTwoLists(list<pair<size_t,size_t>>& to,
			    list<pair<size_t,size_t>>& from)
{
  if (from.size()==0) return; // nothing to do
  if (to.size()==0) { // replace from with to
    to.splice(to.end(), from);
    return;
  }
  size_t cursor = 0;
  for (list<pair<size_t,size_t>>::iterator from_iter=from.begin();
       from_iter!=from.end(); ++from_iter) {
    cursor = mergeARangeToList(to, *from_iter, cursor);
  }
}

size_t LineFinderUtils::mergeARangeToList(list<pair<size_t,size_t>>& range_list,
				  pair<size_t,size_t>& new_range,
				  size_t const cursor)
{
  AlwaysAssert(new_range.first<=new_range.second, AipsError);
  // cursor should be zero if range_list is empty
  AlwaysAssert(range_list.size()>0 || cursor==0, AipsError);
  if (range_list.size()==0) {//just add new range to range_list;
    range_list.push_back(new_range);
    return cursor;
  }
  AlwaysAssert(cursor < range_list.size(), AipsError);
  // add range at the end
  if (new_range.first > range_list.back().second) {
    range_list.push_back(new_range);
    return range_list.size()-1;
  }
  // move iterator to the start point of the search
  list<pair<size_t,size_t>>::iterator start_cursor = range_list.begin();
  if (cursor > 0) {
    for (size_t i=1; i<cursor; ++i) {
      ++start_cursor;
    }
    AlwaysAssert(new_range.first > (*start_cursor).second, AipsError);
    ++start_cursor;
  }
  // add range to the begining
  if (new_range.second < (*start_cursor).first) {
    range_list.insert(start_cursor, new_range);
    return cursor+1;
  }
  size_t out_cursor = cursor;
  // new_range is in some where after cursor position of range_list.
  // find segment in range_list (final) list where overlap starts
  list<pair<size_t,size_t>>::iterator insert_begin=range_list.end();
  bool any_overlap = false;
  for (list<pair<size_t,size_t>>::iterator iter=start_cursor;
       iter!=range_list.end(); ++iter, ++out_cursor) {
    if ((*iter).first > new_range.second) {
      // the range exceeded new_range. No overlap. Insert before this range.
      range_list.insert(iter, new_range);
      return out_cursor;
    } else if ((*iter).second < new_range.first) { //|| (*iter).first > new_range.second) {
      // still too small ranges
      continue;
    } else {//overlap started
      insert_begin = iter;
      any_overlap = true;
      break;
    }
  }
  // An overlap should be found.
  AlwaysAssert(any_overlap, AipsError);
  // find segment in range_list (final) list where overlap ends
  list<pair<size_t,size_t>>::iterator insert_end=range_list.end();
  for (list<pair<size_t,size_t>>::iterator iter=insert_begin;
       iter!=range_list.end(); ++iter) {
    if ((*iter).first <= new_range.second) {
      insert_end = iter;
    }
    else break; // the range exceed new_range.
  }
  // there was an overlap
  list<pair<size_t,size_t>> temp_list;
  // preceeding elements (including the first overlapped range)
  temp_list.splice(temp_list.end(), range_list, range_list.begin(), insert_begin);
  // overlaping segment
  pair<size_t,size_t> unified_range(std::min((*insert_begin).first, new_range.first),
				    std::max((*insert_end).second, new_range.second));
  temp_list.push_back(unified_range);
  // trailing elements
  if (insert_end != range_list.end()){
    temp_list.splice(temp_list.end(),range_list,++insert_end, range_list.end());
  }
  range_list.clear();
  range_list.splice(range_list.end(), temp_list);
  return out_cursor;
}

void LineFinderUtils::mergeSmallGapByFraction(double const fraction,
					      size_t const maxwidth,
					      list<pair<size_t,size_t>>& range_list)
{
  if (range_list.size() < 2) return; // nothing to do
  list<pair<size_t,size_t>> temp_list;
  list<pair<size_t,size_t>>::iterator iter=range_list.begin();
  temp_list.push_back(*iter);
  ++iter;
  for (; iter != range_list.end(); ++iter) {
    size_t curr_width = (*iter).second - (*iter).first;
    size_t prev_width = temp_list.back().second - temp_list.back().first;
    size_t gap = (*iter).first-temp_list.back().second;
    if (gap < static_cast<size_t>(std::min(prev_width, curr_width)*fraction) && \
	(*iter).second-temp_list.back().first < maxwidth) {
      temp_list.back().second = (*iter).second;
    }
    else {
      temp_list.push_back(*iter);
    }
  }
  range_list.clear();
  range_list.splice(range_list.end(), temp_list);
}

void LineFinderUtils::rejectWideRange(size_t const maxwidth,
                                      list<pair<size_t,size_t>>& range_list)
{
    list<pair<size_t, size_t> >  temp_list;
    for(list<pair<size_t,size_t>>::iterator iter=range_list.begin();
        iter!=range_list.end(); ++iter) {
      AlwaysAssert((*iter).second >= (*iter).first, AipsError);
      size_t width = (*iter).second - (*iter).first + 1;
	if (width <= maxwidth) {
            temp_list.push_back(*iter);
        }
    }
    range_list.clear();
    range_list.merge(temp_list);
}

void LineFinderUtils::rejectNarrowRange(size_t const minwidth,
                                        list<pair<size_t,size_t>>& range_list)
{
  AlwaysAssert(minwidth>0, AipsError);
    list<pair<size_t, size_t> >  temp_list;
    for(list<pair<size_t,size_t>>::iterator iter=range_list.begin();
        iter!=range_list.end(); ++iter) {
      AlwaysAssert((*iter).second >= (*iter).first, AipsError);
      size_t width = (*iter).second - (*iter).first + 1;
	if (width >= minwidth) {
            temp_list.push_back(*iter);
        }
    }
    range_list.clear();
    range_list.merge(temp_list);
}

template size_t LineFinderUtils::binDataAndMask<float>(size_t const num_in,
						       float const in_data[],
						       bool const in_mask[],
						       size_t const bin_size,
						       size_t const num_out,
						       float out_data[],
						       bool out_mask[],
						       size_t const offset,
						       bool const keepsize);

} //# NAMESPACE CASA - END
