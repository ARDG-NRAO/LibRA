//# FiltrationTVI.h: Template class for data filtering TVI
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

#ifndef _MSVIS_FILTRATIONTVI_H_
#define _MSVIS_FILTRATIONTVI_H_

#include <casacore/casa/aips.h>
#include <casacore/casa/Containers/Record.h>
#include <msvis/MSVis/TransformingVi2.h>
#include <msvis/MSVis/VisibilityIterator2.h>

#include <map>
#include <vector>
#include <memory>

#include <casacore/measures/Measures/Stokes.h>

namespace casacore {
// forward declaration
//class Record;
}

namespace casa { //# NAMESPACE CASA - BEGIN

namespace vi { //# NAMESPACE vi - BEGIN

//# forward decl
//class ViFactory;
class ViiLayerFactory;
class FiltrationTVIFactory;

// Filtering type
class FilteringType {
public:
  // Filtering type enum
  enum FilteringTypeEnum {
    SDDoubleCircleFilter,
    NumFilteringTypes,
    NoTypeFilter
  };

  static casacore::String toString(FilteringTypeEnum type) {
    casacore::String type_string = "";

    switch (type) {
    case SDDoubleCircleFilter:
      type_string = "SDDoubleCircleFilter";
      break;
    default:
      type_string = "InvalidFilter";
      break;
    }

    return type_string;
  }
};


// <summary>
// FiltrationTVI is an implementation of data filtering
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class="TransformingVi2">TransformingVi2</linkto>
// </prerequisite>
//
// <etymology>
//
// </etymology>
//
// <synopsis>
// FiltrationTVI works as a visibility filter, returning only necessary data chunk
// when iterating through the data. It takes filter implementation
// class as its template argument, and work with it to filter out unwanted
// data chunk. You can change the behavior of the class by simply replacing
// filter implementation with the one that fits your usecase.
// </synopsis>
//
// <example>
// <code>
// //
// </code>
// </example>
//
// <motivation>
// For single dish calibration, we need a mechanism to select the data chunk
// necessary for calibration. This selection associates with the calibration
// type so it is completely different from user-specified selection.
// </motivation>
//
// <thrown>
//    <li>
//    <li>
// </thrown>
//
// <todo asof="1997/05/30">
// </todo>

template<class Filter>
class FiltrationTVI: public TransformingVi2 {

public:

  // Destructor

  virtual ~FiltrationTVI();

  // Report the the ViImplementation type
  virtual casacore::String ViiType() const {
    return casacore::String("FiltrationTVI<") + filter_p->filterType() + ">( "
        + getVii()->ViiType() + " )";
  }
  ;

  //   +==================================+
  //   |                                  |
  //   | Iteration Control and Monitoring |
  //   |                                  |
  //   +==================================+

  // Methods to control and monitor subchunk iteration

  virtual void origin();
  virtual void next();

  // Methods to control chunk iterator

  virtual void originChunks (casacore::Bool forceRewind = false);
  virtual void nextChunk ();

  // Return the number of rows in the current iteration
  // FiltrationTVI may change the number of rows when given vb
  // is partly filtrate
  virtual casacore::rownr_t nRows() const;

  // Return the row ids as from the original root table. This is useful
  // to find correspondance between a given row in this iteration to the
  // original ms row
  virtual void getRowIds(casacore::Vector<casacore::rownr_t> & rowids) const;

  //   +=========================+
  //   |                         |
  //   | Subchunk casacore::Data Accessors |
  //   |                         |
  //   +=========================+

  // Return antenna1

  virtual void antenna1(casacore::Vector<casacore::Int> & ant1) const;

  // Return antenna2

  virtual void antenna2(casacore::Vector<casacore::Int> & ant2) const;

  // Return actual time interval

  virtual void exposure(casacore::Vector<double> & expo) const;

  // Return feed1

  virtual void feed1(casacore::Vector<casacore::Int> & fd1) const;

  // Return feed2

  virtual void feed2(casacore::Vector<casacore::Int> & fd2) const;

  // Return the current FieldId

  virtual void fieldIds(casacore::Vector<casacore::Int>&) const;

  // Return the current ArrayId

  virtual void arrayIds(casacore::Vector<casacore::Int>&) const;

  // Return flag for each polarization, channel and row

  virtual void flag(casacore::Cube<casacore::Bool> & flags) const;

  // Return flag for each channel & row

  virtual void flag(casacore::Matrix<casacore::Bool> & flags) const;

  // Return flags for each polarization, channel, category, and row.

  virtual void flagCategory(
  casacore::Array<casacore::Bool> & flagCategories) const;

  // Return row flag

  virtual void flagRow(casacore::Vector<casacore::Bool> & rowflags) const;

  // Return the OBSERVATION_IDs

  virtual void observationId(casacore::Vector<casacore::Int> & obsids) const;

  // Return the PROCESSOR_IDs

  virtual void processorId(casacore::Vector<casacore::Int> & procids) const;

  // Return scan number

  virtual void scan(casacore::Vector<casacore::Int> & scans) const;

  // Return the STATE_IDs

  virtual void stateId(casacore::Vector<casacore::Int> & stateids) const;

  // Return feed configuration matrix for specified antenna

  virtual void jonesC(
      casacore::Vector<casacore::SquareMatrix<casacore::Complex, 2> > & cjones) const;

  // Return sigma

  virtual void sigma(casacore::Matrix<casacore::Float> & sigmat) const;

  // Return current SpectralWindow

  virtual void spectralWindows(casacore::Vector<casacore::Int> & spws) const;

  // Return MJD midpoint of interval.

  virtual void time(casacore::Vector<double> & t) const;

  // Return MJD centroid of interval.

  virtual void timeCentroid(casacore::Vector<double> & t) const;

  // Return nominal time interval

  virtual void timeInterval(casacore::Vector<double> & ti) const;

  // Return u,v and w (in meters)

  virtual void uvw(casacore::Matrix<double> & uvwmat) const;

  // Return the visibilities as found in the casacore::MS, casacore::Cube (npol,nchan,nrow).

  virtual void visibilityCorrected(
  casacore::Cube<casacore::Complex> & vis) const;
  virtual void visibilityModel(casacore::Cube<casacore::Complex> & vis) const;
  virtual void visibilityObserved(
  casacore::Cube<casacore::Complex> & vis) const;

  // Return FLOAT_DATA as a casacore::Cube (npol, nchan, nrow) if found in the MS.

  virtual void floatData(casacore::Cube<casacore::Float> & fcube) const;

  // Return the shape of the visibility Cube

  virtual casacore::IPosition visibilityShape() const;

  // Return weight

  virtual void weight(casacore::Matrix<casacore::Float> & wtmat) const;

  // Return weightspectrum (a weight for each channel)

  virtual void weightSpectrum(casacore::Cube<casacore::Float> & wtsp) const;
  virtual void sigmaSpectrum(casacore::Cube<casacore::Float> & wtsp) const;

  //   +=========================+
  //   |                         |
  //   | Chunk and casacore::MS Level casacore::Data |
  //   |                         |
  //   +=========================+

  virtual void dataDescriptionIds(casacore::Vector<casacore::Int> &) const;

protected:

  FiltrationTVI(ViImplementation2 * inputVi, casacore::Record const &configuration);

private:
  // Filtration operation
  // increment the iterator until given subchunk passes through the filter
  void filter();
  void filterChunk();

  casacore::Record configuration_p;
  Filter *filter_p;
  casacore::Int num_filtrates_p;
  casacore::Vector<casacore::Bool> is_filtrate_p;

  // boolean flag for each subchunk in the current chunk (True: valid, False: invalid)
  casacore::Vector<bool> is_valid_subchunk_p;

  friend FiltrationTVIFactory;
};

// factory
class FiltrationTVIFactory: public ViFactory {

public:
  // Constructor
  FiltrationTVIFactory(casacore::Record const &configuration,
      ViImplementation2 *inputVII);
//  FiltrationTVIFactory(casacore::Record const &configuration,
//  casacore::MeasurementSet const *ms, SortColumns const sortColumns,
//  casacore::Double timeInterval, casacore::Bool isWritable);

  // Destructor
  ~FiltrationTVIFactory();

  ViImplementation2 * createVi() const;

private:
  ViImplementation2 *inputVII_p;
  casacore::Record configuration_p;
};

class FiltrationTVILayerFactory: public ViiLayerFactory {

public:
  FiltrationTVILayerFactory(casacore::Record const &configuration);
  virtual ~FiltrationTVILayerFactory() {
  }

protected:

  ViImplementation2 * createInstance(ViImplementation2* vii0) const;

  casacore::Record configuration_p;

};

} // end namespace vi

} //# NAMESPACE CASA - END

#endif // _MSVIS_FILTRATIONTVI_H_

