/*
 * SDDoubleCircleGainCal.h
 *
 *  Created on: May 31, 2016
 *      Author: nakazato
 */

#ifndef SYNTHESIS_MEASUREMENTCOMPONENTS_SDDOUBLECIRCLEGAINCALIMPL_H_
#define SYNTHESIS_MEASUREMENTCOMPONENTS_SDDOUBLECIRCLEGAINCALIMPL_H_

#include <map>
#include <list>

#include <casacore/casa/aipstype.h>
#include <casacore/casa/Arrays/Cube.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/BasicSL/Constants.h>
#include <casacore/casa/Containers/Block.h>

namespace casa { // namespace casa START

class TimeRangeKey {
public:
  struct Less {
    bool operator()(TimeRangeKey const &a, TimeRangeKey const &b) const {
      for (size_t i = 0; i < 3; ++i) {
        if (a.meta_[i] != b.meta_[i]) {
          return a.meta_[i] < b.meta_[i];
        }
      }
      // a.meta_ == b.meta_
      return false;
    }
  };
  TimeRangeKey(casacore::Int const field_id, casacore::Int const antenna_id,
  casacore::Int const feed_id) :
      meta_(3) {
    meta_[0] = field_id;
    meta_[1] = antenna_id;
    meta_[2] = feed_id;
  }
  ~TimeRangeKey() = default;
private:
  casacore::Block<casacore::Int> meta_;
  TimeRangeKey() = delete;
};

typedef std::pair<casacore::Double, casacore::Double> TimeRange;
typedef std::list<TimeRange> TimeRangeList;

class TimeRangeListTool {
public:
  static bool InRange(TimeRange const &timerange, casacore::Double const current_time) {
    auto const time_from = timerange.first * (1.0 - casacore::C::dbl_epsilon);
    auto const time_to = timerange.second * (1.0 + casacore::C::dbl_epsilon);
    return time_from < current_time && current_time < time_to;
  }

  static bool InRange(TimeRangeList const &timerange_list, casacore::Double const current_time) {
    for (auto iter = timerange_list.begin(); iter != timerange_list.end(); ++iter) {
      if (InRange(*iter, current_time)) {
        return true;
      }
    }
    return false;
  }
};

class SDDoubleCircleGainCalImpl {
public:
  SDDoubleCircleGainCalImpl();
  virtual ~SDDoubleCircleGainCalImpl();

  // getter
  // get size of the central region in radian
  casacore::Double getCentralRegion() const {
    return central_region_;
  }
  casacore::Bool isSmoothingActive() const {
    return do_smooth_;
  }
  // get smoothing size
  casacore::Int getSmoothingSize() const {
    return smooth_size_;
  }
  // get observing frequency in Hz
  casacore::Double getObservingFrequency() const {
    return observing_frequency_;
  }
  // get antenna diameter in meter
  casacore::Double getAntennaDiameter() const {
    return antenna_diameter_;
  }

  // primvary beam size in radian
  casacore::Double getPrimaryBeamSize() const;

  // default smoothing size
  casacore::Int getDefaultSmoothingSize() const;

  // get radius of the central region in radian
  casacore::Double getRadius();

  // get effective smoothing size
  casacore::Int getEffectiveSmoothingSize();

  // setter
  // set radius of the central region in radian
  void setCentralRegion(casacore::Double value) {
    central_region_ = value;
  }

  // activate smoothing and set smoothing size
  void setSmoothing(casacore::Int size) {
    do_smooth_ = true;
    smooth_size_ = size;
  }

  // deactivate smoothing
  void unsetSmoothing() {
    do_smooth_ = false;
    smooth_size_ = -1;
  }

  // set observing frequency in Hz
  void setObservingFrequency(casacore::Double value) {
    observing_frequency_ = value;
  }

  // set antenna diameter in meter
  void setAntennaDiameter(casacore::Double value) {
    antenna_diameter_ = value;
  }

  // gain calibration
  // based on Stephen White's IDL script
  void calibrate(casacore::Cube<casacore::Float> const &data,
  casacore::Vector<casacore::Double> const &time,
  casacore::Matrix<casacore::Double> const &direction,
  casacore::Vector<casacore::Double> &gain_time,
  casacore::Cube<casacore::Float> &gain);
  // subspecies that take into account flag (false: valid, true: invalid)
  void calibrate(casacore::Cube<casacore::Float> const &data,
  casacore::Cube<casacore::Bool> const &flag,
  casacore::Vector<casacore::Double> const &time,
  casacore::Matrix<casacore::Double> const &direction,
  casacore::Vector<casacore::Double> &gain_time,
  casacore::Cube<casacore::Float> &gain,
  casacore::Cube<casacore::Bool> &gain_flag);

  // gain calibration implementation
  void doCalibrate(casacore::Vector<casacore::Double> &gain_time,
  casacore::Cube<casacore::Float> &gain,
  casacore::Cube<casacore::Bool> &gain_flag);

  // find time range that observed central region
  bool findTimeRange(casacore::Vector<casacore::Double> const &time,
  casacore::Vector<casacore::Double> const &interval,
  casacore::Matrix<casacore::Double> const &direction,
      TimeRangeList &timerange);

  // apply gain factor
//  void apply(casacore::Vector<casacore::Double> const &gain_time,
//      casacore::Cube<casacore::Float> const &gain,
//      casacore::Vector<casacore::Double> const &time,
//      casacore::Cube<casacore::Float> &data);

private:
  // radius of the central region [rad]
  casacore::Double central_region_;

  // flag for smoothing
  casacore::Bool do_smooth_;

  // smoothing size
  casacore::Int smooth_size_;

  // parameter for primary beam size determination
  // observing frequency [Hz]
  casacore::Double observing_frequency_;

  // antenna diameter [m]
  casacore::Double antenna_diameter_;

  // logger
  casacore::LogIO logger_;

  // find data within radius
  void findDataWithinRadius(casacore::Double const radius,
  casacore::Vector<casacore::Double> const &time,
  casacore::Cube<casacore::Float> const &data,
  casacore::Matrix<casacore::Double> const &direction,
  casacore::Vector<casacore::Double> &gain_time,
  casacore::Cube<casacore::Float> &gain);
  void findDataWithinRadius(casacore::Double const radius,
  casacore::Vector<casacore::Double> const &time,
  casacore::Cube<casacore::Float> const &data,
  casacore::Cube<casacore::Bool> const &flag,
  casacore::Matrix<casacore::Double> const &direction,
  casacore::Vector<casacore::Double> &gain_time,
  casacore::Cube<casacore::Float> &gain,
  casacore::Cube<casacore::Bool> &gain_flag);
};

} // namespace casa END

#endif /* SYNTHESIS_MEASUREMENTCOMPONENTS_SDDOUBLECIRCLEGAINCALIMPL_H_ */
