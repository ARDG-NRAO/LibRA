/*
 * SourceRecord.h
 *
 *  Created on: Jan 27, 2016
 *      Author: nakazato
 */

#ifndef SINGLEDISH_FILLER_SOURCERECORD_H_
#define SINGLEDISH_FILLER_SOURCERECORD_H_

#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/measures/Measures/MDirection.h>
#include <casacore/ms/MeasurementSets/MSSource.h>
#include <casacore/ms/MeasurementSets/MSSourceColumns.h>

namespace casa { //# NAMESPACE CASA - BEGIN
namespace sdfiller { //# NAMESPACE SDFILLER - BEGIN

struct SourceRecord {
  typedef casacore::MSSource AssociatingTable;
  typedef casacore::MSSourceColumns AssociatingColumns;

  // mandatory
  casacore::Int source_id;
  casacore::Int spw_id;
  casacore::String name;
  casacore::Double time;
  casacore::Double interval;
  casacore::MDirection direction;
  casacore::Int num_lines;

  // optional
  casacore::String code;
  casacore::Int calibration_group;
  casacore::Vector<casacore::String> transition;
  casacore::Vector<casacore::Double> rest_frequency;
  casacore::Vector<casacore::Double> sysvel;
  casacore::Vector<casacore::Double> proper_motion;

  // method
  void clear() {
    source_id = -1;
    spw_id = -1;
    name = "";
    time = -1.0;
    interval = -1.0;
    direction = casacore::MDirection();
    num_lines = 0;
    code = "";
    calibration_group = -1;
    transition.resize();
    rest_frequency.resize();
    sysvel.resize();
    proper_motion.resize();
  }

  SourceRecord &operator=(SourceRecord const &other) {
    source_id = other.source_id;
    spw_id = other.spw_id;
    name = other.name;
    time = other.time;
    interval = other.interval;
    direction = other.direction;
    num_lines = other.num_lines;
    code = other.code;
    calibration_group = other.calibration_group;
    transition = other.transition;
    rest_frequency = other.rest_frequency;
    sysvel = other.sysvel;
    proper_motion = other.proper_motion;
    return *this;
  }

  void add(AssociatingTable &table, AssociatingColumns &columns) {
    if (columns.nrow() == 0) {
      // set frame info
      casacore::TableRecord &record = columns.direction().rwKeywordSet();
      casacore::Record meas_info = record.asRecord("MEASINFO");
      meas_info.define("Ref", direction.getRefString());
      record.defineRecord("MEASINFO", meas_info);
    }

    table.addRow(1, true);
  }

  casacore::Bool fill(casacore::uInt irow, AssociatingColumns &columns) {
    if (columns.nrow() <= irow) {
      return false;
    }

    columns.sourceId().put(irow, source_id);
    columns.spectralWindowId().put(irow, spw_id);
    columns.name().put(irow, name);
    columns.time().put(irow, time);
    columns.interval().put(irow, interval);
    columns.directionMeas().put(irow, direction);
    columns.numLines().put(irow, num_lines);
    columns.calibrationGroup().put(irow, calibration_group);
    if (code.size() > 0) {
      columns.code().put(irow, code);
    }
    if (transition.size() > 0) {
      columns.transition().put(irow, transition);
    }
    if (rest_frequency.size() > 0) {
      columns.restFrequency().put(irow, rest_frequency);
    }
    if (sysvel.size() > 0) {
      columns.sysvel().put(irow, sysvel);
    }
    if (proper_motion.size() > 0) {
      columns.properMotion().put(irow, proper_motion);
    }

    return true;
  }
};

} //# NAMESPACE SDFILLER - END
} //# NAMESPACE CASA - END

#endif /* SINGLEDISH_FILLER_SOURCERECORD_H_ */
