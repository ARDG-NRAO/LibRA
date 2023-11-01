/*
 * FieldRecord.h
 *
 *  Created on: Jan 27, 2016
 *      Author: nakazato
 */

#ifndef SINGLEDISH_FILLER_FIELDRECORD_H_
#define SINGLEDISH_FILLER_FIELDRECORD_H_

#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/measures/Measures/MDirection.h>
#include <casacore/ms/MeasurementSets/MSField.h>
#include <casacore/ms/MeasurementSets/MSFieldColumns.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// forward decleration
template<class Reader>
class SingleDishMSFiller;

namespace sdfiller { //# NAMESPACE SDFILLER - BEGIN

struct FieldRecord {
  typedef casacore::MSField AssociatingTable;
  typedef casacore::MSFieldColumns AssociatingColumns;

private:
  // external resource
  casacore::Record source_map;
  AssociatingTable table;

public:
  // meta
  casacore::Int field_id;
  casacore::String source_name;
  casacore::MDirection::Types frame;

  // mandatory
  casacore::String name;
  casacore::Double time;
  casacore::Matrix<casacore::Double> direction;

  // optional
  casacore::String code;

  // for dummy entry
  casacore::Int const d_source_id = -1;
  casacore::String const d_name = "";
  casacore::Int const d_num_poly = 0;
  casacore::Double const d_time = 0.0;
  casacore::Matrix<casacore::Double> const d_direction = casacore::Matrix<casacore::Double>(2, 1, 0.0);

  // method
  void clear() {
    field_id = -1;
    source_name = "";
    name = "";
    time = -1.0;
    direction.resize();
    frame = casacore::MDirection::N_Types;
    code = "";
  }

  FieldRecord &operator=(FieldRecord const &other) {
    source_map = other.source_map;
    table = other.table;
    field_id = other.field_id;
    source_name = other.source_name;
    frame = other.frame;
    name = other.name;
    time = other.time;
    direction = other.direction;
    code = other.code;
    return *this;
  }

  void add(AssociatingTable &table, AssociatingColumns &columns) {
    casacore::uInt ufield_id = (casacore::uInt) field_id;
    casacore::uInt nrow = table.nrow();
    if (nrow == 0) {
      // set direction frame information
      setFrame(columns);
    }
    if (nrow <= ufield_id) {
      table.addRow(ufield_id - nrow + 1);
      casacore::uInt new_nrow = table.nrow();
      for (casacore::uInt i = nrow; i < new_nrow - 1; ++i) {
        columns.sourceId().put(i, d_source_id);
        columns.name().put(i, d_name);
        columns.time().put(i, d_time);
        columns.numPoly().put(i, d_num_poly);
        columns.delayDir().put(i, d_direction);
        columns.phaseDir().put(i, d_direction);
        columns.referenceDir().put(i, d_direction);
      }
    }
  }

  casacore::Bool fill(casacore::uInt /*irow*/, AssociatingColumns &columns) {
    if (field_id < 0) {
      return false;
    }

    casacore::uInt nrow = columns.nrow();
    if (nrow == 0) {
      // set direction frame information
      setFrame(columns);
    }

    if (table.nrow() <= (casacore::uInt) field_id) {
      return false;
    }

    columns.name().put(field_id, name);
    columns.time().put(field_id, time);
    casacore::Int source_id = -1;
    if (source_map.isDefined(source_name)) {
      source_id = source_map.asInt(source_name);
    }
    columns.sourceId().put(field_id, source_id);
    columns.delayDir().put(field_id, direction);
    columns.phaseDir().put(field_id, direction);
    columns.referenceDir().put(field_id, direction);
    casacore::Int num_poly = direction.shape()[1] - 1;
    columns.numPoly().put(field_id, num_poly);
    if (code.size() > 0) {
      columns.code().put(field_id, code);
    }
    return true;
  }

private:
  void setFrame(AssociatingColumns &columns) {
    casacore::TableRecord &record = columns.delayDir().rwKeywordSet();
    casacore::Record meas_info = record.asRecord("MEASINFO");
    casacore::String direction_frame = casacore::MDirection::showType(frame);
    meas_info.define("Ref", direction_frame);
    record.defineRecord("MEASINFO", meas_info);
    columns.phaseDir().rwKeywordSet().defineRecord("MEASINFO", meas_info);
    columns.referenceDir().rwKeywordSet().defineRecord("MEASINFO", meas_info);
  }

  template<class Reader>
  friend class casa::SingleDishMSFiller;
};

} //# NAMESPACE SDFILLER - END
} //# NAMESPACE CASA - END

#endif /* SINGLEDISH_FILLER_FIELDRECORD_H_ */
