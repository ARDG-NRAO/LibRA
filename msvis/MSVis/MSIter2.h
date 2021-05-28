//# MSIter2.h: MSIter w/ smarter interval handling
//# Copyright (C) 1996,1999,2000,2001,2002
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
//# $Id$

#ifndef MS_MSITER2_H
#define MS_MSITER2_H

#include <casacore/casa/aips.h>
#include <casacore/ms/MeasurementSets/MSIter.h>

namespace casa { //# NAMESPACE CASA - BEGIN
namespace vi {  //# NAMESPACE VI - BEGIN

//# forward decl
//class MSColumns;
//class TableIterator;

// <summary>
// Small helper class to specify an 'interval' comparison
// </summary>
// <synopsis>
// Small helper class to specify an 'interval' comparison for table iteration
// by time interval.
// </synopsis>
class MSSmartInterval : public casacore::MSInterval
{
public:
  explicit MSSmartInterval(casacore::Double interval,
			   casacore::Vector<casacore::Double>& timeboundaries); 
  virtual ~MSSmartInterval() {}
  virtual void setOffset(casacore::Double offset);
  virtual int comp(const void * obj1, const void * obj2) const;

private:

    // Local versions of MSInterval::interval_p,offset_p so we
    //   can avoid function calls and obj copies in comp()
    casacore::Double interval2_;
    mutable casacore::Double offset2_;

    // A list of enforced interval time boundaries (e.g., scan bounds).
    //   the interval grid will be re-registered to each 
    //   of the values in this Vector as supplied times cross them
    casacore::Vector<casacore::Double> timeBounds_;
    int nBounds_;

    // Is specified interval effectively zero?
    casacore::Bool zeroInterval_;

    // Utility variables used in comp
    mutable casacore::Bool found_;
    mutable int bidx_;
    mutable double t1_,t2_;

};

// <summary> 
// An iterator class for MeasurementSets
// </summary>
 
// <use visibility=export>
 
// <prerequisite>
//   <li> <linkto class="MeasurementSet:description">MeasurementSet</linkto> 
// </prerequisite>
//
// <etymology>
// MSIter stands for the MeasurementSet Iterator class.
// </etymology>
//
// <synopsis> 
// An MSIter is a class to traverse a MeasurementSet in various orders.  It
// automatically adds four predefined sort columns to your selection of sort
// columns (see constructor) so that it can keep track of changes in frequency
// or polarization setup, field position and sub-array.  Note that this can
// cause iterations to occur in a different way from what you would expect, see
// examples below.  MSIter implements iteration by time interval for the use of
// e.g., calibration tasks that want to calculate solutions over some interval
// of time.  You can iterate over multiple MeasurementSets with this class.
// </synopsis> 
//
// <example>
// <srcblock>
// // The following code iterates by by ARRAY_ID, FIELD_ID, DATA_DESC_ID and
// // TIME (all implicitly added columns) and then by baseline (antenna pair),
// // in 3000s intervals.
// MeasurementSet ms("3C273XC1.ms"); 
// Block<int> sort(2);
//        sort[0] = MS::ANTENNA1;
//        sort[1] = MS::ANTENNA2;
// Double timeInterval = 3000;
// MSIter msIter(ms,sort,timeInteval);
// for (msIter.origin(); msIter.more(); msIter++) {
// // print out some of the iteration state
//    cout << msIter.fieldId() << endl;
//    cout << msIter.fieldName() << endl;
//    cout << msIter.dataDescriptionId() << endl;
//    cout << msIter.frequency0() << endl;
//    cout << msIter.table().nrow() << endl;
//    process(msIter.table()); // process the data in the current iteration
// }
// // Output shows only 1 row at a time because the table is sorted on TIME
// // first and ANTENNA1, ANTENNA2 next and each baseline occurs only once per 
// // TIME stamp. The interval has no effect in this case.
// </srcblock>
// </example>

// <example>
// <srcblock>
// // The following code iterates by baseline (antenna pair), TIME, and,
// // implicitly, by ARRAY_ID, FIELD_ID and DATA_DESC_ID in 3000s
// // intervals.
// MeasurementSet ms("3C273XC1.ms"); 
// Block<int> sort(3);
//        sort[0] = MS::ANTENNA1;
//        sort[1] = MS::ANTENNA2;
//        sort[2] = MS::TIME;
// Double timeInterval = 3000;
// MSIter msIter(ms,sort,timeInteval);
// for (msIter.origin(); msIter.more(); msIter++) {
// // print out some of the iteration state
//    cout << msIter.fieldId() << endl;
//    cout << msIter.fieldName() << endl;
//    cout << msIter.dataDescriptionId() << endl;
//    cout << msIter.frequency0() << endl;
//    cout << msIter.table().nrow() << endl;
//    process(msIter.table()); // process the data in the current iteration
// // Now the output shows 7 rows at a time, all with identical ANTENNA1
// // and ANTENNA2 values and TIME values within a 3000s interval.
// }
// </srcblock>
// </example>
//
// <motivation>
// This class was originally part of the VisibilityIterator class, but that 
// class was getting too large and complicated. By splitting out the toplevel
// iteration into this class the code is much easier to understand. It is now
// also available through the ms tool.
// </motivation>
//
// <todo>
// multiple observatories in a single MS are not handled correctly (need to
// sort on observation id and check observatory name to set position frame)
// </todo>

class MSIter2 : public casacore::MSIter
{
public:

  // Default constructor - useful only to assign another iterator later.
  // Use of other member functions on this object is likely to dump core.
  MSIter2();

  MSIter2(const casacore::MeasurementSet& ms, const casacore::Block<int>& sortColumns, 
	  double timeInterval=0, bool addDefaultSortColumns=true,
	  bool storeSorted=true);

  // Same as above with multiple MSs as input.
  MSIter2(const casacore::Block<casacore::MeasurementSet>& mss, const casacore::Block<int>& sortColumns, 
	  double timeInterval=0, bool addDefaultSortColumns=true,
	  bool storeSorted=true);

  // Copy construct. This calls the assigment operator.
  MSIter2(const MSIter2& other);

  // Destructor
  virtual ~MSIter2();
  
  // Assigment. This will reset the iterator to the origin.
  MSIter2 & operator=(const MSIter2 &other);

  // Reset iterator to start of data
  //  This specialization resets the time-compare object 
  //   before calling parent
  virtual void origin();
 
protected:
  // handle the construction details
  void construct2(const casacore::Block<int>& sortColumns, casacore::Bool addDefaultSortColumns);

  // set the iteration state
  virtual void setState();

  virtual void discernEnforcedTimeBounds(casacore::Vector<casacore::Double>& solbounds,
					 casacore::Bool scanBounded);
  virtual void discernEnforcedTimeBounds(casacore::Vector<casacore::Double>& solbounds,
					 casacore::Bool scanBounded,casacore::Bool fieldBounded);
  virtual void discernEnforcedTimeBounds(casacore::Vector<casacore::Double>& solbounds,
					 casacore::Bool scanBounded,casacore::Bool fieldBounded,
					 casacore::Double dt);

};

} //# NAMESPACE VI - END
} //# NAMESPACE CASA - END

#endif
