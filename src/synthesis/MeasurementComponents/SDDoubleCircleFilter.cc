//# SDDoubleCircleFilter.cc: Filter for SDDoubleCircleGainCal
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
#include <synthesis/MeasurementComponents/SDDoubleCircleFilter.h>

#include <cassert>

#include <casacore/casa/aips.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/ArrayLogical.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Containers/Record.h>
#include <casacore/casa/Quanta/QuantumHolder.h>

#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/ms/MeasurementSets/MSIter.h>

#include <casacore/tables/Tables/ScalarColumn.h>

#include <casacore/measures/TableMeasures/ArrayQuantColumn.h>
#include <casacore/measures/TableMeasures/ScalarQuantColumn.h>
#include <casacore/measures/TableMeasures/ArrayMeasColumn.h>

#include <msvis/MSVis/VisBuffer2.h>
#include <synthesis/MeasurementComponents/SDDoubleCircleGainCalImpl.h>
#include <synthesis/Utilities/PointingDirectionCalculator.h>
#include <synthesis/Utilities/PointingDirectionProjector.h>

using namespace casacore;

namespace { //# ANONYMOUS NAMESPACE - BEGIN
inline bool isEphemeris(String const &name) {
  // Check if given name is included in MDirection types
  Int nall, nextra;
  const uInt *typ;
  auto *types = MDirection::allMyTypes(nall, nextra, typ);
  auto start_extra = nall - nextra;
  auto capital_name = name;
  capital_name.upcase();

  for (auto i = start_extra; i < nall; ++i) {
    if (capital_name == types[i]) {
      return true;
    }
  }

  return false;
}

Double rad2arcsec(Double value_in_rad) {
  return Quantity(value_in_rad, "rad").getValue("arcsec");
}

} //# ANONYMOUS NAMESPACE - END

namespace casa { //# NAMESPACE CASA - BEGIN

namespace vi { //# NAMESPACE vi - BEGIN

// constructor
SDDoubleCircleFilter::SDDoubleCircleFilter(Record const &configuration) :
    ms_name_(), configuration_(configuration), smooth_(false), central_disk_radius_(
        0.0), timerange_list_() {
  initFilter();
}

String SDDoubleCircleFilter::filterType() const {
  return String("SDDoubleCircle");
}

// isFiltrate returns true if given vb does pass through the filter
bool SDDoubleCircleFilter::isFiltrate(VisBuffer2 const *vb) {
  Vector<bool> is_filtrate;
  int n = isFiltratePerRow(vb, is_filtrate);
  return 0 < n;
}

void SDDoubleCircleFilter::initFilter() {
  // parameters for double circle gain calibration
  if (configuration_.isDefined("smooth")) {
    smooth_ = configuration_.asBool("smooth");
  }
  if (configuration_.isDefined("radius")) {
    String size_string = configuration_.asString("radius");
    QuantumHolder qh;
    String error;
    qh.fromString(error, size_string);
    Quantity q = qh.asQuantity();
    central_disk_radius_ = q.getValue("rad");
  }

  LogIO os(LogOrigin("SDDoubleCircleFilter", "initFilter", WHERE));
  os << LogIO::DEBUGGING << "smooth=" << smooth_ << LogIO::POST;
  os << LogIO::DEBUGGING << "central disk size=" << central_disk_radius_
      << " rad" << "(" << rad2arcsec(central_disk_radius_) << " arcsec)"
      << LogIO::POST;

  if (central_disk_radius_ < 0.0) {
    os << "Negative central disk size is given" << LogIO::EXCEPTION;
  }

}

int SDDoubleCircleFilter::isFiltratePerRow(VisBuffer2 const *vb,
    Vector<bool> &is_filtrate) {
  Vector<Double> current_time = vb->time();
  Vector<Int> field_id = vb->fieldId();
  Vector<Int> antenna_id = vb->antenna1();
  Vector<Int> feed_id = vb->feed1();

  uInt nrows = current_time.nelements();
  if (is_filtrate.nelements() != nrows) {
    is_filtrate.resize(nrows);
  }
  for (uInt i = 0; i < nrows; ++i) {
    is_filtrate[i] = TimeRangeListTool::InRange(
        timerange_list_[TimeRangeKey(field_id[i], antenna_id[i], feed_id[i])],
        current_time[i]);
  }
  return ntrue(is_filtrate);
}

void SDDoubleCircleFilter::syncWith(ViImplementation2 const *vii) {
  if (vii->isNewMs() && ms_name_ != vii->msName()) {
    ms_name_ = vii->msName();

    // execute findDataWithinRadius
    // ---> map<pair(field, antenna, feed), TimeRangeList: list of TimeRange>

    // reset map
    timerange_list_.clear();

    // find time range that observed central region
    SDDoubleCircleGainCalImpl worker;
    LogIO os(LogOrigin("SDDoubleCircleFilter", "syncWith", WHERE));

    Int smoothingSize = -1;    // use default smoothing size
    worker.setCentralRegion(central_disk_radius_);
    if (smooth_) {
      worker.setSmoothing(smoothingSize);
    } else {
      worker.unsetSmoothing();
    }

    // make a map between SOURCE_ID and source NAME
    MeasurementSet const &ms = vii->ms();
    auto const &sourceTable = ms.source();
    ScalarColumn<Int> idCol(sourceTable,
        sourceTable.columnName(MSSource::MSSourceEnums::SOURCE_ID));
    ScalarColumn<String> nameCol(sourceTable,
        sourceTable.columnName(MSSource::MSSourceEnums::NAME));
    std::map<Int, String> sourceMap;
    for (uInt irow = 0; irow < sourceTable.nrow(); ++irow) {
      auto sourceId = idCol(irow);
      if (sourceMap.find(sourceId) == sourceMap.end()) {
        sourceMap[sourceId] = nameCol(irow);
      }
    }

    // make a map between FIELD_ID and SOURCE_ID
    auto const &fieldTable = ms.field();
    idCol.attach(fieldTable,
        fieldTable.columnName(MSField::MSFieldEnums::SOURCE_ID));
    ROArrayMeasColumn<MDirection> dirCol(fieldTable, "REFERENCE_DIR");
    std::map<Int, Int> fieldMap;
    for (uInt irow = 0; irow < fieldTable.nrow(); ++irow) {
      auto sourceId = idCol(irow);
      fieldMap[static_cast<Int>(irow)] = sourceId;
    }

    // access to subtable columns
    ROScalarQuantColumn<Double> antennaDiameterColumn(ms.antenna(),
        "DISH_DIAMETER");
    ROArrayQuantColumn<Double> observingFrequencyColumn(ms.spectralWindow(),
        "CHAN_FREQ");

    // make a list of nchan
    auto const &spw_table = ms.spectralWindow();
    idCol.attach(spw_table, "NUM_CHAN");
    Vector<Int> nChanParList = idCol.getColumn();

    // traverse MS
    Int cols[] = { MS::FIELD_ID, MS::ANTENNA1, MS::FEED1, MS::DATA_DESC_ID };
    Int *colsp = cols;
    Block<Int> sortCols(4, colsp, false);
    MSIter msIter(ms, sortCols, 0.0, false, false);
    for (msIter.origin(); msIter.more(); msIter++) {
      MeasurementSet const currentMS = msIter.table();

      uInt nrow = currentMS.nrow();
      os << LogIO::DEBUGGING << "nrow = " << nrow << LogIO::POST;
      if (nrow == 0) {
        os << LogIO::DEBUGGING << "Skip" << LogIO::POST;
        continue;
      }
      Int ispw = msIter.spectralWindowId();
      if (nChanParList[ispw] == 4) {
        // Skip WVR
        os << LogIO::DEBUGGING << "Skip " << ispw << "(nchan "
            << nChanParList[ispw] << ")" << LogIO::POST;
        continue;
      }
      ScalarColumn<Int> antennaCol(currentMS, "ANTENNA1");
      Int antenna_id = antennaCol(0);
      os << "Process antenna " << antenna_id
          << " spw " << ispw << "(nchan " << nChanParList[ispw] << ")"
          << LogIO::POST;

      Int field_id = msIter.fieldId();

      ScalarColumn<Int> feedCol(currentMS, "FEED1");
      Int feed_id = feedCol(0);
      os << LogIO::DEBUGGING << "FIELD_ID " << field_id << " ANTENNA1 "
          << antenna_id
          //currAnt_
          << " FEED1 " << feed_id << " DATA_DESC_ID "
          << msIter.dataDescriptionId() << LogIO::POST;

      // skip if current (field_id, antenna_id, feed_id) has already been processed
      TimeRangeKey key(field_id, antenna_id, feed_id);
      if (timerange_list_.find(key) != timerange_list_.end()) {
        continue;
      }

      // setup PointingDirectionCalculator
      PointingDirectionCalculator pcalc(currentMS);
      pcalc.setDirectionColumn("DIRECTION");
      pcalc.setFrame("J2000");
      pcalc.setDirectionListMatrixShape(PointingDirectionCalculator::ROW_MAJOR);
      os << LogIO::DEBUGGING << "SOURCE_ID " << fieldMap[field_id]
          << " SOURCE_NAME " << sourceMap[fieldMap[field_id]] << LogIO::POST;
      auto const isEphem = ::isEphemeris(sourceMap[fieldMap[field_id]]);
      Matrix<Double> offset_direction;
      if (isEphem) {
        pcalc.setMovingSource(sourceMap[fieldMap[field_id]]);
        offset_direction = pcalc.getDirection();
      } else {
        pcalc.unsetMovingSource();
        Matrix<Double> direction = pcalc.getDirection();

        // absolute coordinate -> offset from center
        OrthographicProjector projector(1.0f);
        projector.setDirection(direction);
        Vector<MDirection> md = dirCol.convert(field_id, MDirection::J2000);
        //logSink() << "md.shape() = " << md.shape() << LogIO::POST;
        auto const qd = md[0].getAngle("rad");
        auto const d = qd.getValue();
        auto const lat = d[0];
        auto const lon = d[1];
        os << "reference coordinate: lat = " << lat << " lon = " << lon
            << LogIO::POST;
        projector.setReferencePixel(0.0, 0.0);
        projector.setReferenceCoordinate(lat, lon);
        offset_direction = projector.project();
        auto const pixel_size = projector.pixel_size();
        // convert offset_direction from pixel to radian
        offset_direction *= pixel_size;
      }
      //    os << LogIO::DEBUGGING<< "offset_direction = " << offset_direction << LogIO::POST;
      //    Double const *direction_p = offset_direction.data();
      //    for (size_t i = 0; i < 10; ++i) {
      //      os << LogIO::DEBUGGING<< "offset_direction[" << i << "]=" << direction_p[i] << LogIO::POST;
      //    }

      ScalarColumn<Double> timeCol(currentMS, "TIME");
      Vector<Double> time = timeCol.getColumn();
      timeCol.attach(currentMS, "INTERVAL");
      Vector<Double> interval = timeCol.getColumn();

      // tell some basic information to worker object
      Quantity antennaDiameterQuant = antennaDiameterColumn(antenna_id);
      worker.setAntennaDiameter(antennaDiameterQuant.getValue("m"));
      os << LogIO::DEBUGGING << "antenna diameter = "
          << worker.getAntennaDiameter() << "m" << LogIO::POST;
      Vector<Quantity> observingFrequencyQuant = observingFrequencyColumn(ispw);
      Double meanFrequency = 0.0;
      auto numChan = observingFrequencyQuant.nelements();
      os << LogIO::DEBUGGING << "numChan = " << numChan << LogIO::POST;
      assert(numChan > 0);
      if (numChan % 2 == 0) {
        meanFrequency = (observingFrequencyQuant[numChan / 2 - 1].getValue("Hz")
            + observingFrequencyQuant[numChan / 2].getValue("Hz")) / 2.0;
      } else {
        meanFrequency = observingFrequencyQuant[numChan / 2].getValue("Hz");
      }
      //os << LogIO::DEBUGGING << "mean frequency " << meanFrequency.getValue() << " [" << meanFrequency.getFullUnit() << "]" << LogIO::POST;
      os << LogIO::DEBUGGING << "mean frequency " << meanFrequency
          << LogIO::POST;
      worker.setObservingFrequency(meanFrequency);
      os << LogIO::DEBUGGING << "observing frequency = "
          << worker.getObservingFrequency() / 1e9 << "GHz" << LogIO::POST;
      Double primaryBeamSize = worker.getPrimaryBeamSize();
      os << LogIO::DEBUGGING << "primary beam size = "
          << rad2arcsec(primaryBeamSize) << " arcsec" << LogIO::POST;

      auto const effective_radius = worker.getRadius();
      os << "effective radius of the central region = " << effective_radius
          << " arcsec" << " (" << rad2arcsec(effective_radius) << " arcsec)"
          << LogIO::POST;
      if (worker.isSmoothingActive()) {
        auto const effective_smoothing_size =
            worker.getEffectiveSmoothingSize();
        os << "smoothing activated. effective size = "
            << effective_smoothing_size << LogIO::POST;
      } else {
        os << "smoothing deactivated." << LogIO::POST;
      }

      // find data within radius
      TimeRangeList timerange_list;
      bool status = worker.findTimeRange(time, interval, offset_direction,
          timerange_list);
      if (status) {
        timerange_list_[key] = timerange_list;
      }
    }

  }
}

} //# NAMESPACE vi - END

} //# NAMESPACE CASA - END
