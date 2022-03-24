//# Reweighter.h: this defines Reweighter which reweights a selected part of an
//# casacore::MS based on the properties of a (possibly different) selection of it.
//# Copyright (C) 2011
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
//#
//# $Id$
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <casacore/ms/MeasurementSets/MSMainEnums.h>
//#include <msvis/MSVis/VisIterator.h>
#include <msvis/MSVis/SubMS.h>
#include <msvis/MSVis/VisBufferComponents.h>
#include <casacore/casa/aips.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/casa/Arrays/Vector.h>
//#include <casa/Utilities/CountedPtr.h>
#include <map>
#include <set>
#include <vector>


#ifndef MSVIS_REWEIGHTER_H
namespace casacore{

class MSSelection; // #include <ms/MSSel/MSSelection.h>
template<class T> class ArrayColumn;
}

namespace casa { //# NAMESPACE CASA - BEGIN

#define MSVIS_REWEIGHTER_H

// <summary>
//   Reweighter reweights a selected part of an casacore::MS based on the properties of a
//   (possibly different) selection of it.
// </summary>

// <visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> MeasurementSet
// </prerequisite>
//
// <etymology>
// Reweighter ...it reweights.
// </etymology>
//
// <synopsis>
// The order of operations (as in ms::split()) is:
//      ctor
//      setmsselect
//      selectTime
//      makeReweighter
// </synopsis>

// These forward declarations are so the corresponding .h files don't have to
// be included in this .h file, but it's only worth it if a lot of other files
// include this file.


class Reweighter
{
public:
  Reweighter(const casacore::String& theMS,
             const casacore::Bool dorms=false,           // Reweight by rms or stddev?
             const casacore::uInt minsamp=1);            // Minimum # of visibilities for measuring a scatter.
  
  virtual ~Reweighter();
  
  // Select spw and channels for each spw in spwstr.
  // Returns true on success and false on failure.
  casacore::Bool selectSpw(std::set<casacore::Int>& spwset, casacore::Vector<casacore::Int>& chanStartv,
                 casacore::Vector<casacore::Int>& chanEndv, casacore::Vector<casacore::Int>& chanStepv,
                 const casacore::String& spwstr);

  // Setup polarization selection (for now, only from available correlations -
  // no casacore::Stokes transformations.)
  casacore::Bool selectCorrelations(const casacore::String& corrstr);

  //select stuff using msselection syntax ...time is left out
  // call it separately with timebin
  // Returns a success value.
  casacore::Bool setmsselect(const casacore::String& fitspw="", const casacore::String& outspw="",
                   const casacore::String& field="",  const casacore::String& baseline="",
                   const casacore::String& scan="",
		   const casacore::String& subarray="", const casacore::String& correlation="",
                   const casacore::String& intent="", const casacore::String& obs="");

  // Select source or field
  casacore::Bool selectSource(const casacore::Vector<casacore::Int>& fieldid);
  
  void selectAntenna(const casacore::Vector<casacore::Int>& antennaids,
		     const casacore::Vector<casacore::String>& antennaSel)
  {
    antennaSel_p = SubMS::pickAntennas(antennaId_p, antennaSelStr_p,
                                       antennaids, antennaSel);
  }
  
  // Select array IDs to use.
  void selectArray(const casacore::String& subarray) {arrayExpr_p = subarray;}

  //select time parameters
  void selectTime(casacore::Double timeBin=-1.0, casacore::String timerng="");

  //void selectSource(casacore::Vector<casacore::String> sourceid);

  // Set the selection string for line-free channels.
  void setFitSpw(const casacore::String& fitspw) {fitspw_p = fitspw;}
  // Set the selection string for output spws (not channels!).
  void setOutSpw(const casacore::String& outspw) {outspw_p = outspw;}
  
  // Do the reweighting!
  casacore::Bool reweight(casacore::String& colname, const casacore::String& combine);

  //void verifyColumns(const casacore::MeasurementSet& ms, const casacore::Vector<casacore::MS::PredefinedColumns>& colNames);

  // Fills polIDs with a map from DDID to polID, and corrTypes with a map from
  // polID to corrTypes.
  // The return value is meaningless for now.
  static casacore::Bool getCorrTypes(casacore::Vector<casacore::Int>& polIDs,
                           casacore::Vector<casacore::Vector<casacore::Int> >& corrTypes,
                           const casacore::MSColumns& msc);
  
 protected:

  //method that returns the selected ms (?! - but it's Boolean - RR)
  casacore::Bool makeSelection();

  // *** Private member functions ***

  // Picks a reference to DATA, MODEL_DATA, CORRECTED_DATA, or LAG_DATA out
  // of ms_p.  FLOAT_DATA is not included because it is not natively complex. 
  const casacore::ArrayColumn<casacore::Complex>& right_column(const casacore::MSColumns *ms_p,
                                             const casacore::MS::PredefinedColumns datacol);

  // Sets sort to a casacore::Block of columns that a VisibilityIterator should sort by,
  // according to combine_p.  Columns that should never be combined in the
  // calling function, i.e. spw for time averaging, should be listed in
  // uncombinable.
  //
  // verbose: log a message on error.
  //
  // Returns whether or not there were any conflicts between combine_p and
  // uncombinable.
  casacore::Bool setSortOrder(casacore::Block<casacore::Int>& sort, const casacore::String& uncombinable="",
                    const casacore::Bool verbose=true) const;

  // Returns whether col is (not in combine_p) || in uncombinable.
  // Columns that should never be combined in the
  // calling function, i.e. spw for time averaging, should be listed in
  // uncombinable.
  //
  // verbose: log a message on error.
  //
  // conflict is set to true if there is a conflict between combine_p and
  // uncombinable.
  casacore::Bool shouldWatch(casacore::Bool& conflict, const casacore::String& col,
                   const casacore::String& uncombinable="",
                   const casacore::Bool verbose=true) const;

  void makeUnionSpw();

  // Defaults to DATA.
  casacore::MS::PredefinedColumns dataColStrToEnum(const casacore::String& col);

  // *** Member variables ***

  // Initialized* by ctors.  (Maintain order both here and in ctors.)
  //  * not necessarily to anything useful.
  casacore::MeasurementSet ms_p, mssel_p;
  casacore::Bool dorms_p;                 // Reweight by rms or stddev?
  casacore::uInt minsamp_p;               // Minimum # of visibilities for measuring a scatter.
  casacore::MSColumns *msc_p;
  casacore::Bool antennaSel_p;		// Selecting by antenna?
  casacore::Double timeBin_p;
  casacore::String scanString_p,          // Selects scans by #number#.  Historically named.
         intentString_p,        // Selects scans by string.  scanString_p was taken.
         obsString_p;            // casacore::String for observationID selection.
  casacore::String timeRange_p, arrayExpr_p, corrString_p;
  casacore::String combine_p;          // Should time averaging not split bins by
                             // scan #, observation, and/or state ID?
                             // Must be lowercase at all times.
  casacore::String fitspw_p;           // Selection string for line-free channels.
  casacore::String outspw_p;           // Selection string for ddids to reweight.
  casacore::Vector<casacore::Int> unionspw_p;    // All the spws in fitspw_p or outspw_p.

  // Uninitialized by ctors.
  casacore::Vector<casacore::String> antennaSelStr_p;
  casacore::Vector<casacore::Int> antennaId_p;

  casacore::Vector<casacore::Int> fieldId_p;

  // casacore::Map from DDID to polID, filled in getCorrTypes().
  casacore::Vector<casacore::Int> polIDs_p;

  // casacore::Map from polID to corrTypes, filled in getCorrTypes().
  casacore::Vector<casacore::Vector<casacore::Int> > corrTypes_p;

  casacore::Vector<casacore::Vector<casacore::Slice> > chanSlices_p;  // Used by VisIterator::selectChannel()
  casacore::Vector<casacore::Slice> corrSlice_p;
  casacore::Vector<casacore::Vector<casacore::Slice> > corrSlices_p;  // Used by VisIterator::selectCorrelation()
  casacore::Matrix<casacore::Double> selTimeRanges_p;

  std::set<casacore::Int> fitspwset_p, outspwset_p;
  casacore::Vector<casacore::Int> fitStart_p, fitEnd_p, fitStep_p;
};

} //# NAMESPACE CASA - END

#endif

