//# SDDoubleCircleFilter.h: Filter for SDDoubleCircleGainCal
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the Implied warranty of MERCHANTABILITY or
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

#ifndef _MSVIS_SD_DOUBLE_CIRCLE_FILTER_H_
#define _MSVIS_SD_DOUBLE_CIRCLE_FILTER_H_

#include <map>

#include <casacore/casa/aips.h>
#include <casacore/casa/Containers/Block.h>

#include <msvis/MSVis/ViImplementation2.h>
#include <synthesis/MeasurementComponents/SDDoubleCircleGainCalImpl.h>
#include <casacore/casa/Arrays/ArrayFwd.h>

namespace casacore {
// forward declaration
class MeasurementSet;
class Record;
class String;
}

namespace casa { //# NAMESPACE CASA - BEGIN

namespace vi { //# NAMESPACE vi - BEGIN

// forward declaration
class VisBuffer2;

class SDDoubleCircleFilter {
public:
  // constructor
  SDDoubleCircleFilter(casacore::Record const &configuration);

  // destructor
  ~SDDoubleCircleFilter() {
  }

  // return string representation of the filter type
  casacore::String filterType() const;

  // filter query
  // isResidue returns true if given vb doesn't pass through the filter
  bool isResidue(VisBuffer2 const *vb) {
    return !isFiltrate(vb);
  }

  // isFiltrate returns true if given vb does pass through the filter
  // (either fully and partly)
  bool isFiltrate(VisBuffer2 const *vb);

  // row-wise filtration information
  // it fills in is_filtrate vector (resize if necessary)
  // and returns number of rows that pass through the filter
  int isFiltratePerRow(VisBuffer2 const *vb, casacore::Vector<bool> &is_filtrate);

  // sync with VI2
  void syncWith(ViImplementation2 const *vii);

private:
  void initFilter();

  casacore::String ms_name_;
  casacore::Record const &configuration_;

  // parameter for filtering
  bool smooth_;
  casacore::Double central_disk_radius_;

  // time range list that observed central region
  std::map<TimeRangeKey, TimeRangeList, TimeRangeKey::Less> timerange_list_;
};

} //# NAMESPACE vi - END

} //# NAMESPACE CASA - END

#endif  // _MSVIS_SD_DOUBLE_CIRCLE_FILTER_H_
