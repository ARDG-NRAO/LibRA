//# VLAFiller.h: 
//# Copyright (C) 1999,2000,2001,2003
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

#ifndef NRAO_VLAFILLER_H
#define NRAO_VLAFILLER_H

#include <casacore/casa/aips.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/IPosition.h>
#include <casacore/casa/Containers/Block.h>
#include <casacore/casa/Containers/Record.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/measures/Measures/MBaseline.h>
#include <casacore/measures/Measures/MDirection.h>
#include <casacore/measures/Measures/Muvw.h>
#include <casacore/measures/Measures/MFrequency.h>
#include <casacore/measures/Measures/MeasConvert.h>
#include <casacore/measures/Measures/MeasFrame.h>
#include <casacore/measures/Measures/Stokes.h>
#include <casacore/tables/DataMan/TiledDataStManAccessor.h>
#include <nrao/VLA/VLAEnum.h>
#include <nrao/VLA/VLAFilterSet.h>
#include <nrao/VLA/VLALogicalRecord.h>

#include <unordered_map>
#include <string>

#include <casa/namespace.h>
namespace casacore{
class Path;
class MPosition;
}

namespace casa { //# NAMESPACE CASA - BEGIN
} //# NAMESPACE CASA - END

struct IterationStatus;

// <summary>Functions to convert from VLA archive format to a casacore::MeasurementSet</summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> SomeClass
//   <li> SomeOtherClass
//   <li> some concept
// </prerequisite>
//
// <etymology>
// </etymology>
//
// <synopsis>
// </synopsis>
//
// <example>
// </example>
//
// <motivation>
// </motivation>
//
// <templating arg=T>
//    <li>
//    <li>
// </templating>
//
// <thrown>
//    <li>
//    <li>
// </thrown>
//
// <todo asof="yyyy/mm/dd">
//   <li> add this feature
//   <li> fix this bug
//   <li> start discussion of this possible extension
// </todo>

class VLAFiller: public MSColumns
{
public:
  // Construct a VLAFiller object that will data from the specified input, and
  // writes it to the supplied MeasurementSet.
  // We set the default tolerance for frequency to be 6 times of the
  // channel width and also give user the ability to pass in a tolerance
  // for frequency into vlafillerfromdisk(). The default tolerance of frequency
  // works for dataset G192. But for dataset NGC7538, one has to give a tolerance
  // as larger as 60 times its channel width ( 60000000Hz ). For other dataset, the
  // user has to try out the proper tolerance.
  VLAFiller(casacore::MeasurementSet& output, VLALogicalRecord& input, casacore::Double freqTolerance=0.0, casacore::Bool autocorr=false, const casacore::String& antnamescheme="new", const casacore::Bool& applyTsys=true);

  // Does nothing special
  ~VLAFiller();

  // Set the filter that will be used to restrict which input records get
  // copied to the output MS.
  void setFilter(const VLAFilterSet& filter);

  // fill the supplied casacore::MeasurementSet from the the supplied VLABuffer. The
  // number/type of messages describing the progress sent to the logger are
  // controlled using the verbose argument. If verbose is -1 or less then no
  // messages are sent to the logger. If verbose is zero then a summary is sent
  // to the logger just before this function completes. If verbose is one then
  // a message is sent for every record copied to the output MS. If verbose is
  // two a message is sent every second record and so on.
  void fill(casacore::Int verbose=1);

  // Construct an empty casacore::MeasurementSet with the supplied table name. Throw
  // an exception (casacore::AipsError) if the specified casacore::Table already exists unless the
  // overwrite argument is set to true.
  static casacore::MeasurementSet emptyMS(const casacore::Path& tableName, 
 				const casacore::Bool overwrite=false);
  
  // Open the casacore::MeasurementSet with the supplied name. Throw an exception
  // (casacore::AipsError) if the specified casacore::Table does not exist. By default the casacore::Table is
  // opened for read/write access. The Type/SubType MUST be "Measurement
  // Set/VLA" ie., the casacore::MS must have been created with this class. Otherwise an
  // exception is thrown.
  static casacore::MeasurementSet openMS(const casacore::Path& tableName, 
 			       const casacore::Bool readonly=false);
  
  // Return an empty casacore::MeasurementSet with the supplied table name. Creates a
  // empty measurement set unless one with the specified name already exists
  // and overwrite is false. Then it opens the existing measurement set for
  // read/write access.
  static casacore::MeasurementSet getMS(const casacore::Path& tableName, 
			      const casacore::Bool overwrite=false);
  
  // Set the stop Parameters for filling
   void setStopParams(casacore::String &, casacore::String &);
private:
  // Read one record from the input and copies it to the casacore::MS if it is not
  // filtered out. Returns false if a record could not be read.
  casacore::Bool fillOne();

  // Send to the logger info about how the filler is going
  void logCurrentRecord(IterationStatus& counts);
  void logChanges(IterationStatus& counts);

  // Send to the logger info about how much data is in the (sub)tables
  void summarise();

  //# Add an entry to the antenna subtable
  casacore::uInt addAntenna(const casacore::MPosition& antennaPos, casacore::uInt whichAnt);

  //# Add an entry to the feed subtable
  void addFeed(casacore::uInt whichAnt);

  //# Add an entry to the field subtable
  casacore::uInt addField(const casacore::MDirection& refDir);

  //# Add an entry to the pointing subtable
  casacore::uInt addPointing(const casacore::MDirection& antennaDir,
		   const casacore::MDirection& fieldDir, casacore::uInt whichAnt);
  // # Add an entry to the doppler dubtable
  casacore::uInt addDoppler( const VLAEnum::CDA cda ); 
  //# Add an entry to the spectral-window subtable
  casacore::uInt addSpectralWindow(const VLAEnum::CDA cda, const casacore::MFrequency& refFreq,
			 const casacore::uInt nChan,
			 const casacore::Double bandwidth,
			 const casacore::uInt ifChain);

  //# Add an entry to the polarization subtable
  casacore::uInt addPolarization(const casacore::Vector<casacore::Stokes::StokesTypes>& pol);

  //# Add an entry to the data description subtable
  casacore::uInt addDataDescription(casacore::uInt spwId, casacore::uInt polId);

  //# Add an entry in the source subtable
  casacore::uInt addSource(const casacore::MDirection& dir );

  //# Add a hypercube to the columns that use the Tiled casacore::Data Storage manager.
  void addHypercubes(casacore::uInt nPol, casacore::uInt nChan);

  //# Add the specified number of rows tp the specified hypercubes
  void extendHypercubes(const casacore::Block<casacore::uInt>& nPol, const casacore::Block<casacore::uInt>& nChan, 
			casacore::uInt rows);
  
  //# Reconcile duplicate field names
  void fixFieldDuplicates(casacore::MSField& msFld);

  //# Determine whether to stop the fill for on-line filling
  casacore::String projectCode;
  casacore::MVEpoch stopTime;
  casacore::Bool checkStop;
  casacore::Bool fillStarted;
  casacore::Bool stopFilling(VLALogicalRecord &);
  //# pol index RR=0, RL=1, LR=2, LL needed just for index data
  casacore::Int polIndexer(casacore::Stokes::StokesTypes& stokes);

  //# Makes sure the type (epoch) is valid, logs a warning (once) if not
  //# assumes B1950_VLA if not valid
  casacore::MDirection::Types validEpoch(casacore::MDirection::Types mdType);

  //# Contains a logical record
  VLALogicalRecord itsRecord;

  //# Only logical records that match the specified criteria will be used.
  VLAFilterSet itsInputFilters;

  //# The output MS
  casacore::MeasurementSet itsMS;
  
  //# The reference frame of the observation is cached here and used when
  //# conversions from the observed frame to the one used in the MS.
  casacore::MeasFrame itsFrame;
  //# The reference frame of the observation whose spw is identified different 
  //# from what in the SPECTRAL_WINDOWS table and been added in. So the current
  //# length of casacore::Vector equal the number of row in table SPECTRAL WINDOW( no use).
  //# casacore::Vector<casacore::MeasFrame> theirFrames;
  //# This is the direction reference for the Measurement set. It is cached
  //# here.
  casacore::MDirection::Types itsMSDirType;
  //# This is the reference frame of the last record copied. Initially it is
  //# set to a meaningless value.
  casacore::MDirection::Types itsDirType;
  //# This converter is used to convert from the direction types on the archive
  //# to the one in the Measurement set. Its is cached to prevent it
  //# continually having to be recreated. It is never used if all the data on
  //# the archive and all the data in the casacore::MS have the same direction type.
  casacore::MDirection::Convert itsDirCtr;

  //# This converter is used to convert from the direction types on the archive
  //# to Az, El. It is only needed for holography data.
  casacore::MDirection::Convert itsAzElCtr;

  //# This converter is used to convert the UVW coordinates from the type on
  //# the archive to the one in the Measurement set. It is cached to
  //# prevent it continually having to be recreated. It is never used if all
  //# the data on the archive and all the data in the casacore::MS have the same
  //# direction type.
  casacore::Muvw::Convert itsUvwCtr;

  //# This converter is used to convert from the channel frequency types on the
  //# archive to the one in the Measurement set. Its is cached hear to prevent
  //# it continually having to be recreated. It is only used if
  //# data in the archive was observed with online Doppler tracking.
  casacore::MFrequency::Convert itsFreqCtr;

  //# This converter is used to the VLA baselines which are in the HADEC frame
  //#to the ITRF frame so that they can be properly added with the VLA reference
  //# position, which is also in the ITRF frame.
  casacore::MBaseline::Convert itsBlCtr;

  //# The field Id for the most recently copied record. Negative if no record
  //# has been copied.
  casacore::Block<casacore::Int> itsFldId;

  //# The antenna Id's for all the antennas in the most recently copied
  //# record. Zero length if no record has been copied.
  casacore::Block<casacore::Int> itsAntId;

  //# The spectral window Id's for all the CDA's in the most recently copied
  //# record. Always of length 4. Contains -1 is the CDA is unused.
  casacore::Block<casacore::Int> itsSpId;

  //# The polarization Id for the most recently copied record. Negative if no
  //# record has been copied. There is one for each Spectral Id.
  casacore::Block<casacore::Int> itsPolId;

  //# The data description Id's for the most recently copied record. Negative
  //# if no record has been copied.
  casacore::Block<Block<casacore::Int> > itsDataId;

  //# The length of these blocks is set to the maximum number of
  //# subarrays(4). The itsNewScan block is true if the field, spectral window
  //# etc. has changed and the itsScan casacore::Block indicates the scan number.
  casacore::Bool itsNewScan;
  casacore::Block<casacore::Int> itsScan;
  //# A string, containing projkect names, that is accumulated when filling and
  //# written to the OBSERVATION subtable.
  casacore::String itsProject;
  //# The place to log messages
  casacore::LogIO itsLog;

  //# These data members needed because of the use of the TiledData Storage
  //# Manager. It may be possible to remove these when the casacore::TiledShape storage
  //# manager can be used (the holdup is currently in VisSet).
  casacore::TiledDataStManAccessor itsDataAcc;
  casacore::Record itsTileId;
  casacore::TiledDataStManAccessor itsSigmaAcc;
  casacore::TiledDataStManAccessor itsFlagAcc;
  casacore::TiledDataStManAccessor itsImagingWeightAcc;
  casacore::TiledDataStManAccessor itsModDataAcc;
  casacore::TiledDataStManAccessor itsCorrDataAcc;
  casacore::TiledDataStManAccessor itsChanFlagAcc;
  casacore::Block<casacore::IPosition> itsDataShapes;
  casacore::Double itsFreqTolerance;
  casacore::Bool itsApplyTsys;
  casacore::Bool itsNewAntName;
  casacore::Bool itsKeepAutoCorr;
  casacore::Bool itsEVLAisOn;
  casacore::Bool itsInitEpoch;
  casacore::Bool itsRevBeenWarned;
  std::unordered_map<std::string, bool> itsTransferWarned;
  bool itsNoPolInfoWarned;
  bool itsZeroEpochWarned;
};
#endif
