/*
 * SDDoubleCircleGainCal.cpp
 *
 *  Created on: May 31, 2016
 *      Author: nakazato
 */

//#include "SDDoubleCircleGainCal.h"
#include "SDDoubleCircleGainCalImpl.h"

#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/IO/ArrayIO.h>
#include <casacore/scimath/Functionals/Interpolate1D.h>
#include <casacore/scimath/Functionals/ScalarSampledFunctional.h>
#include <casacore/casa/BasicSL/Constants.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Quanta/Quantum.h>

#include <casacore/casa/Utilities/Sort.h>

#include <cassert>
#include <cmath>

using namespace casacore;

#define LOG logger_ << LogOrigin("SDDoubleCircleGainCal", __FUNCTION__, WHERE)
#define POSTLOG LogIO::POST

namespace { // anonymous namespace START
// primary beam size based on observing frequency and antenna diameter in radian
inline Double getPrimaryBeamSize(Double const observing_frequency,
    Double const antenna_diameter) {
  Double beam_size = -1.0;
  constexpr Double kFactorALMA = 1.13; // measured factor for ALMA
  Double const speed_of_light = C::c;
  //Double const rad2arcsec = 180.0 / C::pi * 3600.0;

  if (observing_frequency <= 0.0 || antenna_diameter <= 0.0) {
    beam_size = 0.0;
  } else {
    beam_size = kFactorALMA * speed_of_light // * rad2arcsec
    / (antenna_diameter * observing_frequency);
  }
  return beam_size;
}

// default smoothing size based on the radius of central region
// radius will be given in unit of radian but the following formula
// seems to assume radius in unit of arcsec
inline Int getDefaultSmoothingSize(Double const radius) {
  auto const radius_in_arcsec = Quantity(radius, "rad").getValue("arcsec");
  return 2 * static_cast<Int>(round(radius_in_arcsec / 1.5)) + 1;
}

// average
inline Double average(size_t const index_from, size_t const index_to,
    Vector<Double> const &data) {
  Double sum = 0.0;
  assert(index_from <= index_to);
  //cout << "number of data to be averaged " << index_to - index_from << endl;
  for (size_t i = index_from; i < index_to; ++i) {
    sum += data[i];
  }
  return sum / static_cast<Double>(index_to - index_from);
}
inline Double average(size_t const index_from, size_t const index_to,
    Vector<Double> const &data, Vector<Bool> const &flag) {
  Double sum = 0.0;
  size_t count = 0;
  assert(index_from <= index_to);
  //cout << "number of data to be averaged " << index_to - index_from << endl;
  for (size_t i = index_from; i < index_to; ++i) {
    if (flag[i] == false) {
      sum += data[i];
      count++;
    }
  }

  if (count == 0) {
    return 0.0;
  }

  return sum / static_cast<Double>(count);
}

// smoothing
inline void smooth(Int const smooth_size, Vector<Double> const &data,
    Vector<Double> &smoothed_data) {
  // TODO replace with sakura function
  assert(data.nelements() == smoothed_data.nelements());
  size_t num_data = data.nelements();
  if (smooth_size < 2 || static_cast<size_t>(smooth_size) >= num_data) {
    //cout << "no smoothing" << endl;
    smoothed_data = data;
  } else {
    size_t left_edge = (smooth_size + 1) / 2 - 1;
    size_t right_edge = smooth_size / 2 + 1;
    for (size_t i = 0; i < left_edge; ++i) {
      size_t l = 0;
      size_t r = i + right_edge;
      //cout << "i = " << i << ": l, r = " << l << "," << r << endl;
      smoothed_data[i] = average(l, r, data);
    }
    for (size_t i = left_edge; i < num_data - right_edge; ++i) {
      size_t l = i - left_edge;
      size_t r = i + right_edge;
      //cout << "i = " << i << ": l, r = " << l << "," << r << endl;
      smoothed_data[i] = average(l, r, data);
    }
    for (size_t i = num_data - right_edge; i < num_data; ++i) {
      size_t l = i - left_edge;
      size_t r = num_data;
      //cout << "i = " << i << ": l, r = " << l << "," << r << endl;
      smoothed_data[i] = average(l, r, data);
    }
  }
}

inline void smooth(Int const smooth_size, Vector<Double> const &data,
    Vector<Bool> const &flag, Vector<Double> &smoothed_data) {
  // TODO replace with sakura function
  assert(data.nelements() == smoothed_data.nelements());
  size_t num_data = data.nelements();
  if (smooth_size < 2 || static_cast<size_t>(smooth_size) >= num_data) {
    //cout << "no smoothing" << endl;
    smoothed_data = data;
  } else {
    size_t left_edge = (smooth_size + 1) / 2 - 1;
    size_t right_edge = smooth_size / 2 + 1;
    for (size_t i = 0; i < left_edge; ++i) {
      size_t l = 0;
      size_t r = i + right_edge;
      //cout << "i = " << i << ": l, r = " << l << "," << r << endl;
      if (flag[i] == true) {
        smoothed_data[i] = data[i];
      } else {
        smoothed_data[i] = average(l, r, data, flag);
      }
    }
    for (size_t i = left_edge; i < num_data - right_edge; ++i) {
      size_t l = i - left_edge;
      size_t r = i + right_edge;
      //cout << "i = " << i << ": l, r = " << l << "," << r << endl;
      if (flag[i] == true) {
        smoothed_data[i] = data[i];
      } else {
        smoothed_data[i] = average(l, r, data, flag);
      }
    }
    for (size_t i = num_data - right_edge; i < num_data; ++i) {
      size_t l = i - left_edge;
      size_t r = num_data;
      //cout << "i = " << i << ": l, r = " << l << "," << r << endl;
      if (flag[i] == true) {
        smoothed_data[i] = data[i];
      } else {
        smoothed_data[i] = average(l, r, data, flag);
      }
    }
  }
}

// interpolation
// unused
/*
inline void interpolateLinear(Vector<Double> const &x0,
    Vector<Double> const &y0, Vector<Double> const &x1, Vector<Double> &y1) {
  // TODO replace with sakura function (need to add double precision version of interpolation function)
  Interpolate1D<Double, Double> interpolator(
      ScalarSampledFunctional<Double>(x0), ScalarSampledFunctional<Double>(y0),
      true, true);
  interpolator.setMethod(Interpolate1D<Double, Double>::linear);
  for (size_t i = 0; i < x1.nelements(); ++i) {
    y1[i] = interpolator(x1[i]);
  }
}
*/
// utility
inline size_t toUnsigned(ssize_t const v) {
  assert(v >= 0);
  return static_cast<size_t>(v);
}
} // anonymous namespace END

namespace casa { // namespace casa START
SDDoubleCircleGainCalImpl::SDDoubleCircleGainCalImpl() :
    central_region_(-1.0), do_smooth_(false), smooth_size_(-1), observing_frequency_(
        0.0), antenna_diameter_(0.0), logger_() {
}

SDDoubleCircleGainCalImpl::~SDDoubleCircleGainCalImpl() {
}

Double SDDoubleCircleGainCalImpl::getPrimaryBeamSize() const {
  // ::getPrimaryBeamSize returns the size in radian
  return ::getPrimaryBeamSize(observing_frequency_, antenna_diameter_);
}

Int SDDoubleCircleGainCalImpl::getDefaultSmoothingSize() const {
  Int default_size = -1;
  if (central_region_ > 0.0) {
    default_size = ::getDefaultSmoothingSize(central_region_);
  } else {
    default_size = ::getDefaultSmoothingSize(0.5 * getPrimaryBeamSize());
  }
  return default_size;
}

void SDDoubleCircleGainCalImpl::calibrate(Cube<Float> const &data,
    Vector<Double> const &time, Matrix<Double> const &direction,
    Vector<Double> &gain_time, Cube<Float> &gain) {

  // radius of the central region
  Double radius = getRadius();

  LOG << "radius = " << radius << POSTLOG;

  // select data within radius
  auto const data_shape = data.shape();
  size_t const num_pol = ::toUnsigned(data_shape[0]);
  size_t const num_chan = ::toUnsigned(data_shape[1]);
  assert(time.nelements() == uInt(data_shape[2]));
  assert(direction.shape()[1] == data_shape[2]);
  assert(direction.shape()[0] == 2);
  findDataWithinRadius(radius, time, data, direction, gain_time, gain);
  size_t num_gain = gain_time.nelements();
  LOG << "num_gain = " << num_gain << POSTLOG;

  //LOG << "indices within radius: " << within_radius << POSTLOG;

  if (num_gain < 100) {
    LOG << LogIO::WARN << "Probably not enough points for gain calibration: "
        << num_gain << endl << "Skipping..." << POSTLOG;
    gain_time.resize();
    gain.resize();
    return;
  }

  // for each spectral data
  Vector<Double> work_data(num_gain);
  Vector<Double> smoothed_data;
  if (do_smooth_) {
    smoothed_data.resize(num_gain);
  }
  for (size_t ipol = 0; ipol < num_pol; ++ipol) {
    for (size_t ichan = 0; ichan < num_chan; ++ichan) {
      for (size_t idata = 0; idata < num_gain; ++idata) {
        work_data[idata] = gain(ipol, ichan, idata);
      }

//      LOG << "work_data[" << ipol << "," << ichan << "]=" << work_data
//          << POSTLOG;

      // smoothing if necessary
      if (do_smooth_) {
        Int smooth_size = getEffectiveSmoothingSize();
        if (smooth_size < 2 || static_cast<size_t>(smooth_size) >= num_gain) {
          LOG << LogIO::WARN
              << "data is not smoothed since smoothing size is invalid: "
              << smooth_size << " (number of data " << num_gain << ")"
              << POSTLOG;
        }
        smooth(smooth_size, work_data, smoothed_data);
      } else {
        LOG << "no smoothing" << POSTLOG;
        smoothed_data.reference(work_data);
      }

      LOG << LogIO::DEBUGGING << "smoothed_data[" << ipol << "," << ichan
          << "]=" << smoothed_data << POSTLOG;

      LOG << LogIO::DEBUGGING << "mean value = " << mean(smoothed_data)
          << POSTLOG;

      // derive gain factor: mean(smoothed_data) / smoothed_data
      work_data = mean(smoothed_data) / smoothed_data;

//      LOG << "gfactor[" << ipol << "," << ichan << "]=" << work_data
//          << POSTLOG;

      // conversion for G type calibration
      work_data = 1.0 / sqrt(work_data);

//      LOG << "fparam[" << ipol << "," << ichan << "]=" << work_data
//          << POSTLOG;

      for (size_t idata = 0; idata < num_gain; ++idata) {
        gain(ipol, ichan, idata) = work_data[idata];
      }
    }
  }
}

void SDDoubleCircleGainCalImpl::calibrate(Cube<Float> const &data,
    Cube<Bool> const &flag, Vector<Double> const &time,
    Matrix<Double> const &direction, Vector<Double> &gain_time,
    Cube<Float> &gain, Cube<Bool> &gain_flag) {

  // radius of the central region
  Double radius = getRadius();

  LOG << "radius = " << radius << POSTLOG;

  // select data within radius
  auto const data_shape = data.shape();
  assert(time.nelements() == uInt(data_shape[2]));
  assert(direction.shape()[1] == data_shape[2]);
  assert(direction.shape()[0] == 2);
  findDataWithinRadius(radius, time, data, flag, direction, gain_time, gain,
      gain_flag);
  doCalibrate(gain_time, gain, gain_flag);
}

void SDDoubleCircleGainCalImpl::doCalibrate(Vector<Double> &gain_time,
    Cube<Float> &gain, Cube<Bool> &gain_flag) {

  size_t const num_gain = gain_time.nelements();
  LOG << "num_gain = " << num_gain << POSTLOG;

  size_t const num_pol = ::toUnsigned(gain.shape()[0]);
  size_t const num_chan = ::toUnsigned(gain.shape()[1]);
  assert(gain.shape()[2] == num_gain);

  //LOG << "indices within radius: " << within_radius << POSTLOG;

  if (num_gain < 100) {
    LOG << LogIO::WARN << "Probably not enough points for gain calibration: "
        << num_gain << endl << "Skipping..." << POSTLOG;
    gain_time.resize();
    gain.resize();
    gain_flag.resize();
    return;
  }

  // for each spectral data
  Vector<Double> work_data(num_gain);
  Vector<Bool> work_flag(num_gain);
  Vector<Double> smoothed_data(num_gain);
  for (size_t ipol = 0; ipol < num_pol; ++ipol) {
    for (size_t ichan = 0; ichan < num_chan; ++ichan) {
      for (size_t idata = 0; idata < num_gain; ++idata) {
        work_data[idata] = gain(ipol, ichan, idata);
        work_flag[idata] = gain_flag(ipol, ichan, idata);
      }

//      LOG << "work_data[" << ipol << "," << ichan << "]=" << work_data
//          << POSTLOG;

      // smoothing if necessary
      if (do_smooth_) {
        Int smooth_size = getEffectiveSmoothingSize();
        if (smooth_size < 2 || static_cast<size_t>(smooth_size) >= num_gain) {
          LOG << LogIO::WARN
              << "data is not smoothed since smoothing size is invalid: "
              << smooth_size << " (number of data " << num_gain << ")"
              << POSTLOG;
        }
        smooth(smooth_size, work_data, work_flag, smoothed_data);
      } else {
        LOG << "no smoothing" << POSTLOG;
        smoothed_data = work_data;
      }

//      LOG << LogIO::DEBUGGING << "smoothed_data[" << ipol << "," << ichan
//          << "]=" << smoothed_data << POSTLOG;

      // derive gain factor: mean(smoothed_data) / smoothed_data
      //work_data = mean(smoothed_data) / smoothed_data;
      auto mean_value = ::average(0, num_gain, smoothed_data, work_flag);

      //work_data = mean_value / smoothed_data;
      if (mean_value < 0.0) {
        LOG << LogIO::WARN
            << "Negative reference value for gain calibration is found. "
            << "No valid calibration solution will be provided" << POSTLOG;
        work_data = 0.0;
        work_flag = True;
      } else {
        for (size_t idata = 0; idata < num_gain; ++idata) {
          if (work_data[idata] != 0.0) {
            work_data[idata] = mean_value / smoothed_data[idata];
          }
          else {
            work_data[idata] = 0.0;
            work_flag[idata] = True;
          }
        }

        LOG << LogIO::DEBUGGING << "mean value = " << mean_value
        << " (simple mean " << mean(smoothed_data) << ")" << POSTLOG;
      }

//      LOG << LogIO::DEBUGGING << "gfactor[" << ipol << "," << ichan << "]=" << work_data
//          << POSTLOG;

      // conversion for G type calibration
      //work_data = 1.0 / sqrt(work_data);
      for (size_t idata = 0; idata < num_gain; ++idata) {
        if (work_data[idata] > 0.0) {
          work_data[idata] = 1.0 / sqrt(work_data[idata]);
        } else {
          work_data[idata] = 0.0;
          work_flag[idata] = True;
        }
      }

//      LOG << LogIO::DEBUGGING << "fparam[" << ipol << "," << ichan << "]=" << work_data
//          << POSTLOG;

      for (size_t idata = 0; idata < num_gain; ++idata) {
        gain(ipol, ichan, idata) = work_data[idata];
        gain_flag(ipol, ichan, idata) = work_flag[idata];
      }
    }
  }
}

Double SDDoubleCircleGainCalImpl::getRadius() {
  // radius of the central region
  Double radius = central_region_;
  if (radius <= 0.0) {
    // use default value: primary beam size
    radius = 0.5 * getPrimaryBeamSize();
  }
  if (radius <= 0.0) {
    ostringstream ss;
    ss << "Size of central region is not properly set: " << radius;
    LOG << LogIO::SEVERE << ss.str() << POSTLOG;
    throw AipsError(ss.str());
  }

  return radius;
}

Int SDDoubleCircleGainCalImpl::getEffectiveSmoothingSize() {
  LOG << "do smoothing with size " << smooth_size_ << POSTLOG;

  Int smooth_size = smooth_size_;
  if (smooth_size_ < 0) {
    smooth_size = getDefaultSmoothingSize();
    LOG << "default smoothing size will be used: " << smooth_size
    << POSTLOG;
  }

  return smooth_size;
}

void SDDoubleCircleGainCalImpl::findDataWithinRadius(Double const radius,
    Vector<Double> const &time, Cube<Float> const &data,
    Matrix<Double> const &direction, Vector<Double> &gain_time,
    Cube<Float> &gain) {
  Cube<Bool> flag(data.shape(), False);
  Cube<Bool> gain_flag;
  findDataWithinRadius(radius, time, data, flag, direction, gain_time, gain,
      gain_flag);
}

void SDDoubleCircleGainCalImpl::findDataWithinRadius(Double const radius,
    Vector<Double> const &time, Cube<Float> const &data, Cube<Bool> const &flag,
    Matrix<Double> const &direction, Vector<Double> &gain_time,
    Cube<Float> &gain, Cube<Bool> &gain_flag) {
  size_t num_data = ::toUnsigned(direction.shape()[1]);
  // find data within radius
  Vector<size_t> data_index(num_data);
  size_t num_gain = 0;
  for (size_t i = 0; i < num_data; ++i) {
    Double x = direction(0, i);
    Double y = direction(1, i);
    Double r2 = x * x + y * y;
    if (r2 <= radius * radius) {
      data_index[num_gain] = i;
      num_gain++;
    }
  }

  // store data for calibration
  gain_time.resize(num_gain);
  IPosition gain_shape(data.shape());
  gain_shape[2] = num_gain;
  gain.resize(gain_shape);
  gain_flag.resize(gain_shape);
  for (size_t i = 0; i < num_gain; ++i) {
    size_t j = data_index[i];
    gain_time[i] = time[j];
    gain.xyPlane(i) = data.xyPlane(j);
    gain_flag.xyPlane(i) = flag.xyPlane(j);
  }
}

bool SDDoubleCircleGainCalImpl::findTimeRange(Vector<Double> const &time,
    Vector<Double> const &interval, Matrix<Double> const &direction,
    TimeRangeList &timerange) {
  // radius of the central region
  Double radius = getRadius();

  // find data within radius
  size_t num_all_data = time.nelements();
  Vector<size_t> data_index(num_all_data);
  size_t num_data = 0;
  for (size_t i = 0; i < num_all_data; ++i) {
    Double x = direction(0, i);
    Double y = direction(1, i);
    Double r2 = x * x + y * y;
    if (r2 <= radius * radius) {
      data_index[num_data] = i;
      num_data++;
    }
  }

  // list of time and interval
  Vector<Double> time_sel(num_data);
  Vector<Double> interval_sel(num_data);
  for (size_t i = 0; i < num_data; ++i) {
    time_sel[i] = time[data_index[i]];
    interval_sel[i] = interval[data_index[i]];
  }

  // sort
  Sort s(time_sel.data(), num_data);
  Vector<uInt> index_vector;
  uInt num_sorted = s.sort(index_vector, num_data);
  if (num_sorted != index_vector.nelements()) {
    return false;
  }

  // conversion from the list of timestamp to TimeRangeList
  timerange.clear();
  Double time_from = time_sel[index_vector[0]] - 0.5 * interval_sel[index_vector[0]];
  Double time_to = time_sel[index_vector[0]] + 0.5 * interval_sel[index_vector[0]];
  auto iter = index_vector.begin();
  for (++iter; iter != index_vector.end(); ++iter) {
    Double current_from = time_sel[*iter] - 0.5 * interval_sel[*iter];
    Double current_to = time_sel[*iter] + 0.5 * interval_sel[*iter];
    if (abs(current_from - time_to) < time_to * C::dbl_epsilon) {
      time_to = current_to;
    } else {
      TimeRange range(time_from, time_to);
      timerange.push_back(range);
      time_from = current_from;
      time_to = current_to;
    }
  }
  TimeRange range(time_from, time_to);
  timerange.push_back(range);

  return true;
}

//void SDDoubleCircleGainCalImpl::apply(Vector<Double> const &gain_time,
//    Cube<Float> const &gain, Vector<Double> const &time, Cube<Float> &data) {
//  // TODO implement
//  // not necessary to implement? reuse G type application?
//}
}// namespace casa END
