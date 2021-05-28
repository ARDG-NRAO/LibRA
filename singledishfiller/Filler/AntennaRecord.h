/*
 * AntennaRecord.h
 *
 *  Created on: Jan 27, 2016
 *      Author: nakazato
 */

#ifndef SINGLEDISH_FILLER_ANTENNARECORD_H_
#define SINGLEDISH_FILLER_ANTENNARECORD_H_

#include <casacore/casa/BasicSL/String.h>
#include <casacore/measures/Measures/MPosition.h>
#include <casacore/ms/MeasurementSets/MSAntenna.h>
#include <casacore/ms/MeasurementSets/MSAntennaColumns.h>

namespace casa { //# NAMESPACE CASA - BEGIN
namespace sdfiller { //# NAMESPACE SDFILLER - BEGIN

struct AntennaRecord {
  typedef casacore::MSAntenna AssociatingTable;
  typedef casacore::MSAntennaColumns AssociatingColumns;

  // mandatory
  casacore::String name;
  casacore::MPosition position;
  casacore::String type;
  casacore::String mount;
  casacore::Double dish_diameter;

  // optional
  casacore::String station;
  casacore::MPosition offset;

  // method
  void clear() {
    name = "";
    position = casacore::MPosition();
    type = "";
    mount = "";
    dish_diameter = -1.0;
    station = "";
    offset = casacore::MPosition();
  }

  AntennaRecord &operator=(AntennaRecord const &other) {
    name = other.name;
    position = other.position;
    type = other.type;
    mount = other.mount;
    dish_diameter = other.dish_diameter;
    station = other.station;
    offset = other.offset;
    return *this;
  }

  void add(AssociatingTable &table, AssociatingColumns &/*columns*/) {
    table.addRow(1, true);
  }

  casacore::Bool fill(casacore::uInt irow, AssociatingColumns &columns) {
    if (columns.nrow() <= irow) {
      return false;
    }

    columns.name().put(irow, name);
    columns.positionMeas().put(irow, position);
    columns.type().put(irow, type);
    columns.mount().put(irow, mount);
    columns.dishDiameter().put(irow, dish_diameter);
    if (station.size() > 0) {
      columns.station().put(irow, station);
    }
    if (anyNE(offset.getAngle().getValue(), 0.0)) {
      columns.offsetMeas().put(irow, offset);
    }
    return true;
  }
};

} //# NAMESPACE SDFILLER - END
} //# NAMESPACE CASA - END

#endif /* SINGLEDISH_FILLER_ANTENNARECORD_H_ */
