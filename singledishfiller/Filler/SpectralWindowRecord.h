/*
 * SpectralWindowRecord.h
 *
 *  Created on: Jan 27, 2016
 *      Author: nakazato
 */

#ifndef SINGLEDISH_FILLER_SPECTRALWINDOWRECORD_H_
#define SINGLEDISH_FILLER_SPECTRALWINDOWRECORD_H_

#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/measures/Measures/MDirection.h>
#include <casacore/ms/MeasurementSets/MSSpectralWindow.h>
#include <casacore/ms/MeasurementSets/MSSpWindowColumns.h>

namespace casa { //# NAMESPACE CASA - BEGIN
namespace sdfiller { //# NAMESPACE SDFILLER - BEGIN

struct SpectralWindowRecord {
  typedef casacore::MSSpectralWindow AssociatingTable;
  typedef casacore::MSSpWindowColumns AssociatingColumns;

  // meta
  casacore::Int spw_id;

  // mandatory
  casacore::Int num_chan;
  casacore::Int meas_freq_ref;
  casacore::Double refpix;
  casacore::Double refval;
  casacore::Double increment;

  // optional
  casacore::String name;

  // for dummy entry
  casacore::Int const d_num_chan = 1;
  casacore::Vector<casacore::Double> const d_array = casacore::Vector<casacore::Double>(1, 0.0);
  casacore::Int const d_freq = 0.0;

  // method
  void clear() {
    spw_id = -1;
    name = "";
    meas_freq_ref = -1;
    num_chan = 0;
    refpix = 0.0;
    refval = 0.0;
    increment = 0.0;
  }

  SpectralWindowRecord &operator=(SpectralWindowRecord const &other) {
    spw_id = other.spw_id;
    name = other.name;
    meas_freq_ref = other.meas_freq_ref;
    num_chan = other.num_chan;
    refpix = other.refpix;
    refval = other.refval;
    increment = other.increment;
    return *this;
  }

  void add(AssociatingTable &table, AssociatingColumns &columns) {
    casacore::uInt uspw_id = (casacore::uInt) spw_id;
    casacore::uInt nrow = table.nrow();
    if (nrow <= uspw_id) {
      table.addRow(uspw_id - nrow + 1);
      casacore::uInt new_nrow = table.nrow();
      for (casacore::uInt i = nrow; i < new_nrow - 1; ++i) {
        columns.numChan().put(i, d_num_chan);
        columns.refFrequency().put(i, d_freq);
        columns.totalBandwidth().put(i, d_freq);
        columns.chanFreq().put(i, d_array);
        columns.chanWidth().put(i, d_array);
        columns.effectiveBW().put(i, d_array);
        columns.resolution().put(i, d_array);
      }
    }
  }

  casacore::Bool fill(casacore::uInt /*irow*/, AssociatingColumns &columns) {
    if (spw_id < 0) {
      return false;
    }

    casacore::uInt nrow = columns.nrow();

    if (nrow <= (casacore::uInt) spw_id) {
      return false;
    }

    columns.numChan().put(spw_id, num_chan);
    columns.measFreqRef().put(spw_id, meas_freq_ref);
    casacore::Double tot_bw = num_chan * fabs(increment);
    columns.totalBandwidth().put(spw_id, tot_bw);
    casacore::Double ref_frequency = refval - refpix * increment;
    columns.refFrequency().put(spw_id, ref_frequency);
    casacore::Vector<casacore::Double> freq(num_chan);
    indgen(freq, ref_frequency, increment);
    columns.chanFreq().put(spw_id, freq);
    freq = increment;
    columns.chanWidth().put(spw_id, freq);
    freq = abs(freq);
    columns.effectiveBW().put(spw_id, freq);
    columns.resolution().put(spw_id, freq);
    casacore::Int net_sideband = 0; // USB
    if (increment < 0.0) {
      net_sideband = 1; // LSB
    }
    columns.netSideband().put(spw_id, net_sideband);
    if (name.size() > 0) {
      columns.name().put(spw_id, name);
    }

    return true;
  }
};

} //# NAMESPACE SDFILLER - END
} //# NAMESPACE CASA - END

#endif /* SINGLEDISH_FILLER_SPECTRALWINDOWRECORD_H_ */
