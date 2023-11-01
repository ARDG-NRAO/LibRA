/*
 * SysCalRecord.h
 *
 *  Created on: Jan 27, 2016
 *      Author: nakazato
 */

#ifndef SINGLEDISH_FILLER_SYSCALRECORD_H_
#define SINGLEDISH_FILLER_SYSCALRECORD_H_

#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/OS/Path.h>
#include <casacore/measures/Measures/MPosition.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/ms/MeasurementSets/MSSysCal.h>
#include <casacore/ms/MeasurementSets/MSSysCalColumns.h>

namespace casa { //# NAMESPACE CASA - BEGIN
namespace sdfiller { //# NAMESPACE SDFILLER - BEGIN

struct SysCalRecord {
  typedef casacore::MSSysCal AssociatingTable;
  typedef casacore::MSSysCalColumns AssociatingColumns;

  // mandatory
  casacore::Int antenna_id;
  casacore::Int feed_id;
  casacore::Int spw_id;
  casacore::Double time;
  casacore::Double interval;

  // optional
  casacore::Vector<casacore::Float> tcal;
  casacore::Matrix<casacore::Float> tcal_spectrum;
  casacore::Vector<casacore::Float> tsys;
  casacore::Matrix<casacore::Float> tsys_spectrum;

  // method
  void clear() {
    antenna_id = -1;
    feed_id = -1;
    spw_id = -1;
    time = 0.0;
    interval = 0.0;
    tcal.resize();
    tcal_spectrum.resize();
    tsys.resize();
    tsys_spectrum.resize();
  }

  SysCalRecord &operator=(SysCalRecord const &other) {
    antenna_id = other.antenna_id;
    feed_id = other.feed_id;
    spw_id = other.spw_id;
    time = other.time;
    interval = other.interval;
    tcal = other.tcal;
    tcal_spectrum = other.tcal_spectrum;
    tsys = other.tsys;
    tsys_spectrum = other.tsys_spectrum;
    return *this;
  }

  void add(AssociatingTable &table, AssociatingColumns &/*columns*/) {
    table.addRow(1, true);
  }

  casacore::Bool fill(casacore::uInt irow, AssociatingColumns &columns) {
    if (columns.nrow() <= irow) {
      return false;
    }

    columns.antennaId().put(irow, antenna_id);
    columns.feedId().put(irow, feed_id);
    columns.spectralWindowId().put(irow, spw_id);
    columns.time().put(irow, time);
    columns.interval().put(irow, interval);
    if (tcal.size() > 0) {
      columns.tcal().put(irow, tcal);
    }
    if (tcal_spectrum.size() > 0) {
      columns.tcalSpectrum().put(irow, tcal_spectrum);
    }
    if (tsys.size() > 0) {
      columns.tsys().put(irow, tsys);
    }
    if (tsys_spectrum.size() > 0) {
      columns.tsysSpectrum().put(irow, tsys_spectrum);
    }
    return true;
  }
};

struct SysCalTableRecord {
  enum Status {
    Spectral, Scalar, NotDefined
  };
  SysCalTableRecord(casacore::MeasurementSet *ms, casacore::uInt irow, SysCalRecord const &record) :
      ms_(ms), columns_(ms->sysCal()), irow_(irow) {
    antenna_id = record.antenna_id;
    feed_id = record.feed_id;
    spw_id = record.spw_id;
    casacore::ScalarColumn<casacore::Int> num_chan_column(ms_->spectralWindow(), "NUM_CHAN");
    num_chan = num_chan_column(spw_id);
    if (record.tsys_spectrum.empty()) {
      num_corr = record.tsys.size();
      tsys_status = Status::Scalar;
      if (record.tsys.empty()) {
        tsys_nominal = -1.0f;
        tsys_status = Status::NotDefined;
      } else {
        tsys_nominal = record.tsys[0];
      }
    } else {
      num_corr = record.tsys_spectrum.shape()[0];
      tsys_status = Status::Spectral;
      tsys_nominal = record.tsys_spectrum(0, 0);
    }
    if (record.tcal_spectrum.empty()) {
      tcal_status = Status::Scalar;
      if (record.tcal.empty()) {
        tcal_nominal = -1.0f;
        tcal_status = Status::NotDefined;
      } else {
        tcal_nominal = record.tcal[0];
      }
    } else {
      tcal_status = Status::Spectral;
      tcal_nominal = record.tcal_spectrum(0, 0);
    }
  }
  SysCalTableRecord(SysCalTableRecord const &other) :
      ms_(other.ms_), columns_(ms_->sysCal()), irow_(other.irow_) {
    antenna_id = other.antenna_id;
    feed_id = other.feed_id;
    spw_id = other.spw_id;
    num_chan = other.num_chan;
    num_corr = other.num_corr;
    tsys_status = other.tsys_status;
    tcal_status = other.tcal_status;
    tsys_nominal = other.tsys_nominal;
    tcal_nominal = other.tcal_nominal;
  }
  casacore::Int antenna_id;
  casacore::Int feed_id;
  casacore::Int spw_id;
  casacore::Int num_chan;
  casacore::Int num_corr;
  Status tsys_status;
  Status tcal_status;
  casacore::Float tsys_nominal;
  casacore::Float tcal_nominal;

  // returns true if two SysCalTableRecord objects are exactly same
  bool operator==(SysCalTableRecord const &record) {
    casacore::String ms_name = casacore::Path(ms_->tableName()).resolvedName();
    casacore::String ms_name_record = casacore::Path(record.ms_->tableName()).resolvedName();
    return ms_name == ms_name_record && irow_ == record.irow_
        && antenna_id == record.antenna_id && feed_id == record.feed_id
        && spw_id == record.spw_id && num_chan == record.num_chan
        && num_corr == record.num_corr && tsys_status == record.tsys_status
        && tcal_status == record.tcal_status
        && tsys_nominal == record.tsys_nominal
        && tcal_nominal == record.tcal_nominal;
  }

  // returns true if given SysCalRecord is effectively the same
  bool operator==(SysCalRecord const &record) {
    bool is_meta_equal = antenna_id == record.antenna_id
        && feed_id == record.feed_id && spw_id == record.spw_id;
    if (!is_meta_equal) {
      return false;
    }

    bool is_tsys_same;
    if (tsys_status == Status::Spectral) {
      is_tsys_same = num_chan > 0
          && (casacore::uInt) num_chan == record.tsys_spectrum.ncolumn() && num_corr > 0
          && (casacore::uInt) num_corr == record.tsys_spectrum.nrow()
          && tsys_nominal == record.tsys_spectrum(0, 0)
          && allEQ(columns_.tsysSpectrum()(irow_), record.tsys_spectrum);
    } else if (tsys_status == Status::Scalar) {
      is_tsys_same = num_corr > 0 && (casacore::uInt) num_corr == record.tsys.size()
          && tsys_nominal == record.tsys[0]
          && allEQ(columns_.tsys()(irow_), record.tsys);
    } else {
      is_tsys_same = record.tsys_spectrum.empty() && record.tsys.empty();
    }

    if (!is_tsys_same) {
      return false;
    }

    bool is_tcal_same;
    if (tcal_status == Status::Spectral) {
      is_tcal_same = num_chan > 0
          && (casacore::uInt) num_chan == record.tcal_spectrum.ncolumn() && num_corr > 0
          && (casacore::uInt) num_corr == record.tcal_spectrum.nrow()
          && tcal_nominal == record.tcal_spectrum(0, 0)
          && allEQ(columns_.tcalSpectrum()(irow_), record.tcal_spectrum);
    } else if (tcal_status == Status::Scalar) {
      is_tcal_same = num_corr > 0 && (casacore::uInt) num_corr == record.tcal.size()
          && tcal_nominal == record.tcal[0]
          && allEQ(columns_.tcal()(irow_), record.tcal);
    } else {
      is_tcal_same = record.tcal_spectrum.empty() && record.tcal.empty();
    }

    if (!is_tcal_same) {
      return false;
    }

    return true;
  }

private:
  casacore::MeasurementSet *ms_;
  casacore::MSSysCalColumns columns_;
  casacore::uInt irow_;
};

} //# NAMESPACE SDFILLER - END
} //# NAMESPACE CASA - END

#endif /* SINGLEDISH_FILLER_SYSCALRECORD_H_ */
