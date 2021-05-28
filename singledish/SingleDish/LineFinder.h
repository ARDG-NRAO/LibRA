//# --------------------------------------------------------------------
//# LineFinder.h: this defines utility functions of line finding
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
#ifndef _CASA_LINEFINDER_H_
#define _CASA_LINEFINDER_H_

#include <list>

#include <casa/aipstype.h>
#include <casa/Arrays/Vector.h>

namespace casa { //# NAMESPACE CASA - BEGIN

namespace linefinder {
/*
 * Find spectral lines of a spectrum using a threshold based on median
 * absolute deviation (MAD).
 * The median of lower 80% of MAD values are used to define the
 * threshold of line channels.
 *
 * @param[in] num_data : the number of elements in data and mask vector.
 * @param[in] data : a float vector of data to detect lines. should be aligned.
 * @param[in] mask : a boolean vector which indecates if the corresponding elements
 *        in data should be used in line detection.
 *        Note mask is modified in the function. Edge channels are masked.
 * @param[in] threshold: a multiplication factor to define threshold.
 * @param[in] max_iteration : the maximum number of iteration to detect line,
 *        and eliminate the line channels to redefine MAD array.
 * @param[in] minwidth : minimum line width (in channel) to detect as lines
 * @param[in] maxwidth : maximum line width (in channel) to detect as lines
 * @param[in] avg_limit : maximum width of channel binning to run line detection.
 * @param[in] edge : the number of elements at the begining and end of data array
 *         that should be ignored in line detection.
 * @return a list of line channel ranges
 *
 * [overview of line detection algorithm]
 * 1. Median absolute deviation of each data element is calculated and
 *    defined as MAD array.
 * 2. Calculate median of MAD array. Only lower 80% of valid elements
 *    (mask=true and not in lines) are used to calculate the value.
 * 3. Compare each element of MAD array and the product of
 *    threshold and median of MAD. Elements that exceeds
 *    the threshold value are detected as channels.
 * 4. Line range modifications,e.g., extend line ranges for wing component,
 *    merge overlapping lines, and check if line meets maxwidth and
 *    minwidth criteria.
 * 5. Go to step 1 and repeat line detection by removing line channels
 *    from MAD calculations. stop iteration either if loop reaches
 *    max_iteration, no lines found in the bin level, no new line channel
 *    is detected, or MAD threshold increased compared to the previous
 *    iteration.
 * 6. bin data with 4 times coarser width and repeat line detection again
 *    untill bin width reaches to avg_limit.
 *    Merge line ranges detected in the current bin level to previous result.
 */
std::list<std::pair<size_t, size_t>> MADLineFinder(size_t const num_data,
		float const data[/*num_data*/], bool mask[/*num_data*/],
		float const threshold, uint8_t max_iteration, size_t const minwidth,
		size_t const maxwidth, size_t const avg_limit,
		std::pair<size_t, size_t> edge);

/*
 Create a boolean mask vector from ranges.
 mask : a boolean vector that stores output mask. see below for
 the values set to each elements of mask.
 ranges : a list of start and end idices pairs to set value in mask.
 invert : a flag to define values of elements inbetween ranges.
 the elements of mask vector whose values are inbetween
 one of start end idices in ranges list are set to
 true if invert=false (default), otherwise, false.
 initialize : whether or not initialize all elements in mask vector.
 when initialize=true, mask array is initialized by
 false if invert=false (default) or true if invert=true.
 when initilize=false, only elements inbetween ranges
 list will be modified.
 */
void getMask(size_t const num_mask, bool mask[/*num_mask*/],
		std::list<std::pair<size_t, size_t>>& ranges, bool invert = false,
		bool initialize = true);
}
} //# NAMESPACE CASA - END

#endif /* _CASA_LINEFINDER_H_ */
