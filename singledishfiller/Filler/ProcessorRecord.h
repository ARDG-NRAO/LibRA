/*
 * ProcessorRecord.h
 *
 *  Created on: Jan 27, 2016
 *      Author: nakazato
 */

#ifndef SINGLEDISH_FILLER_PROCESSORRECORD_H_
#define SINGLEDISH_FILLER_PROCESSORRECORD_H_

#include <casacore/casa/BasicSL/String.h>
#include <casacore/ms/MeasurementSets/MSProcessor.h>
#include <casacore/ms/MeasurementSets/MSProcessorColumns.h>

namespace casa { //# NAMESPACE CASA - BEGIN
namespace sdfiller { //# NAMESPACE SDFILLER - BEGIN

struct ProcessorRecord {
  typedef casacore::MSProcessor AssociatingTable;
  typedef casacore::MSProcessorColumns AssociatingColumns;

  // mandatory
  casacore::String type;
  casacore::String sub_type;
  casacore::Int type_id;
  casacore::Int mode_id;

  // method
  void clear() {
    type = "";
    sub_type = "";
    type_id = -1;
    mode_id = -1;
  }

  ProcessorRecord &operator=(ProcessorRecord const &other) {
    type = other.type;
    sub_type = other.sub_type;
    type_id = other.type_id;
    mode_id = other.mode_id;
    return *this;
  }

  void add(AssociatingTable &table, AssociatingColumns &/*columns*/) {
    table.addRow(1, true);
  }

  casacore::Bool fill(casacore::uInt irow, AssociatingColumns &columns) {
    if (columns.nrow() <= irow) {
      return false;
    }

    columns.type().put(irow, type);
    columns.subType().put(irow, sub_type);
    columns.typeId().put(irow, type_id);
    columns.modeId().put(irow, mode_id);

    return true;
  }
};

} //# NAMESPACE SDFILLER - END
} //# NAMESPACE CASA - END

#endif /* SINGLEDISH_FILLER_PROCESSORRECORD_H_ */
