//# Partition.h: this defines Partition which creates an casacore::MS that is a subset of
//# an casacore::MS without any changes to the subtables.
//# 
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
#include <casacore/ms/MeasurementSets/MSMainColumns.h>
#include <casacore/ms/MeasurementSets/MSMainEnums.h>
//#include <msvis/MSVis/VisIterator.h>
#include <casacore/casa/aips.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/casa/Arrays/Vector.h>
//#include <casa/Utilities/CountedPtr.h>
#include <msvis/MSVis/SubMS.h>

#ifndef MSVIS_PARTITION_H
namespace casacore{

template<class T> class ArrayColumn;
}

namespace casa { //# NAMESPACE CASA - BEGIN

#define MSVIS_PARTITION_H

// <summary>
// Partition makes a subset of an existing casacore::MS without remapping any indices
// or resizing any subtables (unlike SubMS).
// </summary>

// <visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> MeasurementSet
//   <li> SubMS
// </prerequisite>
//
// <etymology>
// Partition an MS.
// </etymology>
//
// <synopsis>
// The order of operations (as in ms::partition()) is:
//      ctor
//      setmsselect
//      selectTime
//      makePartition
// </synopsis>

//casacore::Bool isAllColumns(const casacore::Vector<casacore::MS::PredefinedColumns>& colNames);

class Partition
{
public:
  Partition(casacore::String& theMS, casacore::Table::TableOption option = casacore::Table::Old);
  
  // construct from an MS
  Partition(casacore::MeasurementSet& ms);

  virtual ~Partition();
  
  // Change or Set the casacore::MS this casacore::MSSelector refers to.
  void setMS(casacore::MeasurementSet& ms);

  // Select spw and channels for each spw.
  // It returns true on success and false on failure.
  casacore::Bool selectSpw(const casacore::String& spwstr);

  //select casacore::Time and time averaging or regridding
  //void selectTime();

  //select stuff using msselection syntax ...time is left out
  // call it separately with timebin
  casacore::Bool setmsselect(const casacore::String& spw="", const casacore::String& field="", 
		   const casacore::String& baseline="", const casacore::String& scan="",
                   const casacore::String& uvrange="", const casacore::String& taql="", 
		   const casacore::String& subarray="", const casacore::String& intent="",
                   const casacore::String& obs="");

  // Select source or field
  casacore::Bool selectSource(const casacore::Vector<casacore::Int>& fieldid);
  
  // Select Antennas to split out  
  void selectAntenna(casacore::Vector<casacore::Int>& antennaids, casacore::Vector<casacore::String>& antennaSel)
  {
    antennaSel_p = SubMS::pickAntennas(antennaId_p, antennaSelStr_p,
				       antennaids, antennaSel);
  } 

  // Select array IDs to use.
  void selectArray(const casacore::String& subarray);

  //select time parameters
  void selectTime(casacore::Double timeBin=-1.0, casacore::String timerng="");

  //void selectSource(casacore::Vector<casacore::String> sourceid);

  //Method to set if a phase Center rotation is needed
  //void setPhaseCenter(casacore::Int fieldid, casacore::MDirection& newPhaseCenter);

  //Method to make the partition.
  //
  //TileShape of size 1 can have 2 values [0], and [1] ...these are used in to
  //determine the tileshape by using MSTileLayout. Otherwise it has to be a
  //vector size 3 e.g [4, 15, 351] => a tile shape of 4 stokes, 15 channels 351
  //rows.
  //
  // combine sets combine_p.  (Columns to ignore while time averaging.)
  //
  casacore::Bool makePartition(casacore::String& outname, casacore::String& whichDataCol,
		     const casacore::Vector<casacore::Int>& tileShape=casacore::Vector<casacore::Int>(1, 0),
		     const casacore::String& combine="");

  //Method to make a scratch partition and even in memory if posssible
  //Useful if temporary subselection/averaging is necessary
  // It'll be in memory if the basic output ms is less than half of 
  // memory reported by casacore::HostInfo unless forced to by user...
  virtual casacore::MeasurementSet* makeScratchPartition(const casacore::Vector<casacore::MS::PredefinedColumns>& whichDataCols, 
  				   const casacore::Bool forceInMemory=false);
  // In this form whichDataCol gets passed to parseColumnNames().
  virtual casacore::MeasurementSet* makeScratchPartition(const casacore::String& whichDataCol, 
  				   const casacore::Bool forceInMemory=false);

  // This sets up a default new ms
  // Declared static as it can be called directly outside of Partition.
  // Therefore it is not dependent on any member variable.
  static casacore::MeasurementSet* setupMS(const casacore::String& msname, const casacore::MeasurementSet& inms,
				 const casacore::Int nchan, const casacore::Int npol, const casacore::String& telescop,
                                 const casacore::Vector<casacore::MS::PredefinedColumns>& colNamesTok,
  				 const casacore::Int obstype=0);

  // Same as above except allowing manual tileshapes
  static casacore::MeasurementSet* setupMS(const casacore::String& msname, const casacore::MeasurementSet& inms,
				 const casacore::Int nchan, const casacore::Int npol,
                                 const casacore::Vector<casacore::MS::PredefinedColumns>& colNamesTok,
  				 const casacore::Vector<casacore::Int>& tileShape=casacore::Vector<casacore::Int>(1,0));
  
  void verifyColumns(const casacore::MeasurementSet& ms, const casacore::Vector<casacore::MS::PredefinedColumns>& colNames);
private:
  //method that returns the selected ms (?! - but it's Boolean - RR)
  casacore::Bool makeSelection();

  // (Sub)table fillers.
  casacore::Bool fillAllTables(const casacore::Vector<casacore::MS::PredefinedColumns>& colNames);
  casacore::Bool fillMainTable(const casacore::Vector<casacore::MS::PredefinedColumns>& colNames);

  //  casacore::Bool writeDiffSpwShape(const casacore::Vector<casacore::MS::PredefinedColumns>& colNames);
  casacore::Bool fillAccessoryMainCols();

  // *** Private member functions ***
  casacore::Bool getDataColumn(casacore::ArrayColumn<casacore::Complex>& data,
                     const casacore::MS::PredefinedColumns colName);
  casacore::Bool getDataColumn(casacore::ArrayColumn<casacore::Float>& data,
                     const casacore::MS::PredefinedColumns colName);
  casacore::Bool putDataColumn(casacore::MSColumns& msc, casacore::ArrayColumn<casacore::Complex>& data,
                     const casacore::MS::PredefinedColumns datacol,
                     const casacore::Bool writeToDataCol=false);
  casacore::Bool putDataColumn(casacore::MSColumns& msc, casacore::ArrayColumn<casacore::Float>& data,
                     const casacore::MS::PredefinedColumns datacol,
                     const casacore::Bool writeToDataCol=false);

  // This method uses VisIter for efficient copy mode data transfer
  casacore::Bool copyDataFlagsWtSp(const casacore::Vector<casacore::MS::PredefinedColumns>& colNames,
                         const casacore::Bool writeToDataCol);

  // Used in a couple of places to estimate how much memory to grab.
  casacore::Double n_bytes() {return mssel_p.nrow() * maxnchan_p * maxncorr_p *
                           sizeof(casacore::Complex);}

  // Read the input, time average it to timeBin_p, and write the output.
  casacore::Bool doTimeAver(const casacore::Vector<casacore::MS::PredefinedColumns>& dataColNames);

  // Fills mapper[ntok] with a map from dataColumn indices to ArrayColumns in
  // the output.  mapper must have ntok slots!
  static void getDataColMap(casacore::MSMainColumns* msc, casacore::ArrayColumn<casacore::Complex>* mapper,
			    casacore::uInt ntok,
			    const casacore::Vector<casacore::MS::PredefinedColumns>& colEnums);

  // *** Member variables ***

  // Initialized* by ctors.  (Maintain order both here and in ctors.)
  //  * not necessarily to anything useful.
  casacore::MeasurementSet ms_p, mssel_p;
  casacore::MSMainColumns * msc_p;		// columns of msOut_p
  casacore::MSColumns * mscIn_p;
  casacore::Bool   antennaSel_p;		// Selecting by antenna?
  casacore::Double timeBin_p;
  casacore::String scanString_p,          // Selects scans by #number#.  Historically named.
         intentString_p,        // Selects scans by string.  scanString_p was taken.
         obsString_p,           // casacore::String for observationID selection.
         uvrangeString_p, taqlString_p;
  casacore::String timeRange_p, arrayExpr_p, corrString_p;
  casacore::String combine_p;          // Should time averaging not split bins by
                             // scan #, observation, and/or state ID?
                             // Must be lowercase at all times.
  casacore::Int maxnchan_p,    // The maximum # of channels and correlations for each
      maxncorr_p;    // selected DDID.  (casacore::Int because NUM_CHAN and NUM_CORR
                     // are casacore::Int instead of uInt.)

  // Uninitialized by ctors.
  casacore::MeasurementSet msOut_p;
  casacore::Vector<casacore::Int> spw_p;      // Selected spw numbers
  casacore::Vector<casacore::Int> fieldid_p;
  casacore::Vector<casacore::String> antennaSelStr_p;
  casacore::Vector<casacore::Int> antennaId_p;
  casacore::Vector<casacore::Int> arrayId_p;
  casacore::Matrix<casacore::Double> selTimeRanges_p;
};

} //# NAMESPACE CASA - END

#endif

