/*
 * DataRecord.h
 *
 *  Created on: Jan 27, 2016
 *      Author: nakazato
 */

#ifndef SINGLEDISH_FILLER_DATARECORD_H_
#define SINGLEDISH_FILLER_DATARECORD_H_

#include <memory>
#include <stdlib.h>

#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/measures/Measures/Stokes.h>
#include <singledishfiller/Filler/FillerUtil.h>

namespace {
template<class T>
void copyStorage(size_t n, T const *src, T *dst) {
  for (size_t i = n; i < n; ++i) {
    dst[i] = src[i];
  }
}

template<class T>
void copyStorage(size_t n, size_t m, size_t stride, T const *src, T *dst) {
  for (size_t i = 0; i < m; ++i) {
    copyStorage(n, src + stride * m, dst + stride * m);
  }
}

}

namespace casa { //# NAMESPACE CASA - BEGIN
namespace sdfiller { //# NAMESPACE SDFILLER - BEGIN

using casacore::SHARE;

struct DataRecord {
  DataRecord() :
      block_size_(16384u), num_data_storage_(block_size_),
      num_tsys_storage_(block_size_), num_tcal_storage_(block_size_),
      data_shape_(1, 0), tsys_shape_(1, 0), tcal_shape_(1, 0),
      data_storage_(new casacore::Float[num_data_storage_]),
      flag_storage_(new casacore::Bool[num_data_storage_]),
      tsys_storage_(new casacore::Float[num_tsys_storage_]),
      tcal_storage_(new casacore::Float[num_tcal_storage_]), direction(2, 2, 0.0),
      direction_slice(casacore::IPosition(2, 2, 1), direction.data(), SHARE),
      direction_vector(direction.column(0)), scan_rate(direction.column(1)),
      data(data_shape_, data_storage_.get(), SHARE),
      flag(data_shape_, flag_storage_.get(), SHARE),
      tsys(tsys_shape_, tsys_storage_.get(), SHARE),
      tcal(tcal_shape_, tcal_storage_.get(), SHARE) {
//    std::cout << "DataRecord::DataRecord()" << std::endl;
    clear();
  }

  ~DataRecord() {
  }

  // method
  void clear() {
//    std::cout << "clear" << std::endl;
    time = -1.0;
    interval = -1.0;
    antenna_id = -1;
    field_id = -1;
    spw_id = -1;
    feed_id = -1;
    scan = -1;
    subscan = -1;
    pol = casacore::Stokes::Undefined;
    intent = "";
    pol_type = "";
    direction = 0.0;
    setDataSize(0);
    setTsysSize(0);
    setTcalSize(0);
    flag_row = true;

    temperature = 0.0f;
    pressure = 0.0f;
    rel_humidity = 0.0f;
    wind_speed = 0.0f;
    wind_direction = 0.0f;
  }

  void setDataSize(size_t n) {
    casacore::Bool redirect = false;
    if (data_shape_[0] != (ssize_t) n) {
//      std::cout << "resize data to " << n << std::endl;
      data_shape_[0] = n;
      redirect = true;
    }
    if (num_data_storage_ < n) {
      size_t new_num_storage = num_data_storage_ + block_size_;
      while (new_num_storage < n) {
        new_num_storage += block_size_;
      }
//      std::cout << "resize data storage to " << new_num_storage << std::endl;
      casacore::Float *new_data_storage = new casacore::Float[new_num_storage];
      copyStorage(data_shape_[0], data_storage_.get(), new_data_storage);
      data_storage_.reset(new_data_storage);
      casacore::Bool *new_flag_storage = new casacore::Bool[new_num_storage];
      copyStorage(data_shape_[0], flag_storage_.get(), new_flag_storage);
      flag_storage_.reset(new_flag_storage);
      num_data_storage_ = new_num_storage;
      redirect = true;
    }
    if (redirect) {
      data.takeStorage(data_shape_, data_storage_.get(), SHARE);
      flag.takeStorage(data_shape_, flag_storage_.get(), SHARE);
    }
  }

  void setTsysSize(size_t n) {
    casacore::Bool redirect = false;
    if (tsys_shape_[0] != (ssize_t) n) {
//      std::cout << "resize tsys to " << n << std::endl;
      tsys_shape_[0] = n;
      redirect = true;
    }
    if (num_tsys_storage_ < n) {
      size_t new_num_storage = num_tsys_storage_ + block_size_;
      while (new_num_storage < n) {
        new_num_storage += block_size_;
      }
//      std::cout << "resize tsys storage to " << new_num_storage << std::endl;
      casacore::Float *new_tsys_storage = new casacore::Float[new_num_storage];
      copyStorage(tsys_shape_[0], tsys_storage_.get(), new_tsys_storage);
      tsys_storage_.reset(new_tsys_storage);
      num_tsys_storage_ = new_num_storage;
      redirect = true;
    }
    if (redirect) {
      tsys.takeStorage(tsys_shape_, tsys_storage_.get(), SHARE);
    }
  }

  void setTcalSize(size_t n) {
    casacore::Bool redirect = false;
    if (tcal_shape_[0] != (ssize_t) n) {
//      std::cout << "resize tcal to " << n << std::endl;
      tcal_shape_[0] = n;
      redirect = true;
    }
    if (num_tcal_storage_ < n) {
      size_t new_num_storage = num_tcal_storage_ + block_size_;
      while (new_num_storage < n) {
        new_num_storage += block_size_;
      }
//      std::cout << "resize tcal storage to " << new_num_storage << std::endl;
      casacore::Float *new_tcal_storage = new casacore::Float[new_num_storage];
      copyStorage(tcal_shape_[0], tcal_storage_.get(), new_tcal_storage);
      tcal_storage_.reset(new_tcal_storage);
      num_tcal_storage_ = new_num_storage;
      redirect = true;
    }
    if (redirect) {
      tcal.takeStorage(tcal_shape_, tcal_storage_.get(), SHARE);
    }
  }

  DataRecord(DataRecord const &other) :
      DataRecord() {
    *this = other;
  }
  DataRecord &operator=(DataRecord const &other) {
    time = other.time;
    interval = other.interval;
    antenna_id = other.antenna_id;
    field_id = other.field_id;
    spw_id = other.spw_id;
    feed_id = other.feed_id;
    scan = other.scan;
    subscan = other.subscan;
    pol = other.pol;
    intent = other.intent;
    pol_type = other.pol_type;
    direction = other.direction;
    direction_slice.takeStorage(direction_slice.shape(), direction.data(),
        SHARE);
    direction_vector.reference(direction.column(0));
    scan_rate.reference(direction.column(1));
    flag_row = other.flag_row;
    setDataSize(other.data_shape_[0]);
    data = other.data;
    flag = other.flag;
    setTsysSize(other.tsys_shape_[0]);
    tsys = other.tsys;
    setTcalSize(other.tcal_shape_[0]);
    tcal = other.tcal;

    temperature = other.temperature;
    pressure = other.pressure;
    rel_humidity = other.rel_humidity;
    wind_speed = other.wind_speed;
    wind_direction = other.wind_direction;

    return *this;
  }

private:
  size_t const block_size_;
  size_t num_data_storage_;
  size_t num_tsys_storage_;
  size_t num_tcal_storage_;
  casacore::IPosition data_shape_;
  casacore::IPosition tsys_shape_;
  casacore::IPosition tcal_shape_;
  std::unique_ptr<casacore::Float[]> data_storage_;
  std::unique_ptr<casacore::Bool[]> flag_storage_;
  std::unique_ptr<casacore::Float[]> tsys_storage_;
  std::unique_ptr<casacore::Float[]> tcal_storage_;

public:
  // mandatory
  casacore::Double time;
  casacore::Double interval;
  casacore::Int antenna_id;
  casacore::Int field_id;
  casacore::Int spw_id;
  casacore::Int feed_id;
  casacore::Int scan;
  casacore::Int subscan;
  casacore::Stokes::StokesTypes pol;
  casacore::String intent;
  casacore::String pol_type;
  casacore::Matrix<casacore::Double> direction;
  casacore::Matrix<casacore::Double> direction_slice;
  casacore::Vector<casacore::Double> direction_vector;
  casacore::Vector<casacore::Double> scan_rate;
  casacore::Vector<casacore::Float> data;
  casacore::Vector<casacore::Bool> flag;
  casacore::Bool flag_row;

  // optional
  casacore::Vector<casacore::Float> tsys;
  casacore::Vector<casacore::Float> tcal;

  casacore::Float temperature;
  casacore::Float pressure;
  casacore::Float rel_humidity;
  casacore::Float wind_speed;
  casacore::Float wind_direction;
};

struct MSDataRecord {
  MSDataRecord() :
      block_size_(131072u),
      num_data_storage_(block_size_),
      num_tsys_storage_(block_size_),
      num_tcal_storage_(block_size_),
      data_shape_(2, 0, 0),
      tsys_shape_(2, 0, 0),
      tcal_shape_(2, 0, 0),
      corr_type_shape_(1, 0),
      corr_type_storage_(4),
      data_storage_(malloc(num_data_storage_ * sizeof(casacore::Complex))),
      flag_storage_(malloc(num_data_storage_ * sizeof(casacore::Bool))),
      tsys_storage_(malloc(num_tsys_storage_ * sizeof(casacore::Float))),
      tcal_storage_(malloc(num_tcal_storage_ * sizeof(casacore::Float))),
      sigma_storage_(new casacore::Float[4]),
      is_float_(false),
      corr_type(corr_type_shape_, corr_type_storage_.storage(), SHARE),
      direction(2, 2, 0.0),
      direction_slice(casacore::IPosition(2, 2, 1), direction.data(), SHARE),
      float_data(data_shape_, reinterpret_cast<casacore::Float *>(data_storage_.get()),
          SHARE),
      complex_data(data_shape_,
          reinterpret_cast<casacore::Complex *>(data_storage_.get()), SHARE),
      flag(data_shape_, reinterpret_cast<casacore::Bool *>(flag_storage_.get()), SHARE),
      sigma(corr_type_shape_, sigma_storage_.get(), SHARE), weight(sigma),
      tsys(tsys_shape_, reinterpret_cast<casacore::Float *>(tsys_storage_.get()), SHARE),
      tcal(tcal_shape_, reinterpret_cast<casacore::Float *>(tcal_storage_.get()), SHARE) {
    if (!data_storage_ || !flag_storage_ || !tsys_storage_ || !tcal_storage_) {
      throw casacore::AipsError("Failed to allocate memory.");
    }
    for (size_t i = 0; i < 4; ++i) {
      sigma_storage_[i] = 1.0f;
    }
    clear();
  }

  ~MSDataRecord() {
  }

  // method
  void clear() {
    time = -1.0;
    interval = -1.0;
    antenna_id = -1;
    field_id = -1;
    spw_id = -1;
    feed_id = -1;
    scan = -1;
    subscan = -1;
    intent = "";
    pol_type = "";
    num_pol = 0;
    direction = 0.0;
    setDataSize(0, 0);
    setTsysSize(0, 0);
    setTcalSize(0, 0);
    flag_row = true;
    is_float_ = false;

    temperature = 0.0f;
    pressure = 0.0f;
    rel_humidity = 0.0f;
    wind_speed = 0.0f;
    wind_direction = 0.0f;
  }

  casacore::Bool isFloat() const {
    return is_float_;
  }

  void setFloat() {
    is_float_ = true;
  }

  void setComplex() {
    is_float_ = false;
  }

  void setDataSize(size_t n, size_t m) {
    casacore::Bool redirect = false;
    if (data_shape_[0] != (ssize_t) n) {
//      std::cout << "resize data to " << n << std::endl;
      data_shape_[0] = n;
      redirect = true;
    }
    if (data_shape_[1] != (ssize_t) m) {
      data_shape_[1] = m;
      redirect = true;
    }
    if (num_data_storage_ < n * m) {
      size_t new_num_storage = num_data_storage_ + block_size_;
      while (new_num_storage < n * m) {
        new_num_storage += block_size_;
      }
//      std::cout << "resize data storage to " << new_num_storage << std::endl;
      void *new_data_storage = malloc(new_num_storage * sizeof(casacore::Complex));
      if (!new_data_storage) {
        throw casacore::AipsError("Failed to allocate memory.");
      }
      if (is_float_) {
        copyStorage(data_shape_[0], data_shape_[1], 4,
            reinterpret_cast<casacore::Float *>(data_storage_.get()),
            reinterpret_cast<casacore::Float *>(new_data_storage));
      } else {
        copyStorage(data_shape_[0], data_shape_[1], 4,
            reinterpret_cast<casacore::Complex *>(data_storage_.get()),
            reinterpret_cast<casacore::Complex *>(new_data_storage));
      }
      data_storage_.reset(new_data_storage);
      void *new_flag_storage = malloc(new_num_storage * sizeof(casacore::Bool));
      if (!new_flag_storage) {
        throw casacore::AipsError("Failed to allocate memory.");
      }
      copyStorage(data_shape_[0], data_shape_[1], 4,
          reinterpret_cast<casacore::Bool *>(flag_storage_.get()),
          reinterpret_cast<casacore::Bool *>(new_flag_storage));
      flag_storage_.reset(new_flag_storage);
      num_data_storage_ = new_num_storage;
      redirect = true;
    }
    if (redirect) {
      corr_type_shape_[0] = data_shape_[0];
      corr_type.takeStorage(corr_type_shape_, corr_type_storage_.storage(),
          SHARE);
      sigma.takeStorage(corr_type_shape_, sigma_storage_.get(), SHARE);
      float_data.takeStorage(data_shape_,
          reinterpret_cast<casacore::Float *>(data_storage_.get()), SHARE);
      complex_data.takeStorage(data_shape_,
          reinterpret_cast<casacore::Complex *>(data_storage_.get()), SHARE);
      flag.takeStorage(data_shape_,
          reinterpret_cast<casacore::Bool *>(flag_storage_.get()), SHARE);
    }
  }

  void setTsysSize(size_t n, size_t m) {
    casacore::Bool redirect = false;
    if (tsys_shape_[0] != (ssize_t) n) {
//      std::cout << "resize tsys to " << n << std::endl;
      tsys_shape_[0] = n;
      redirect = true;
    }
    if (tsys_shape_[1] != (ssize_t) m) {
//      std::cout << "resize tsys to " << n << std::endl;
      tsys_shape_[1] = m;
      redirect = true;
    }
    if (num_tsys_storage_ < n * m) {
      size_t new_num_storage = num_tsys_storage_ + block_size_;
      while (new_num_storage < n * m) {
        new_num_storage += block_size_;
      }
//      std::cout << "resize tsys storage to " << new_num_storage << std::endl;
      void *new_tsys_storage = malloc(new_num_storage * sizeof(casacore::Float));
      copyStorage(tsys_shape_[0], tsys_shape_[1], 4,
          reinterpret_cast<casacore::Float *>(tsys_storage_.get()),
          reinterpret_cast<casacore::Float *>(new_tsys_storage));
      tsys_storage_.reset(new_tsys_storage);
      num_tsys_storage_ = new_num_storage;
      redirect = true;
    }
    if (redirect) {
      tsys.takeStorage(tsys_shape_,
          reinterpret_cast<casacore::Float *>(tsys_storage_.get()), SHARE);
    }
  }

  void setTcalSize(size_t n, size_t m) {
    casacore::Bool redirect = false;
    if (tcal_shape_[0] != (ssize_t) n) {
//      std::cout << "resize tcal to " << n << std::endl;
      tcal_shape_[0] = n;
      redirect = true;
    }
    if (tcal_shape_[1] != (ssize_t) m) {
//      std::cout << "resize tcal to " << n << std::endl;
      tcal_shape_[1] = m;
      redirect = true;
    }
    if (num_tcal_storage_ < n * m) {
      size_t new_num_storage = num_tcal_storage_ + block_size_;
      while (new_num_storage < n * m) {
        new_num_storage += block_size_;
      }
//      std::cout << "resize tcal storage to " << new_num_storage << std::endl;
      void *new_tcal_storage = malloc(new_num_storage * sizeof(casacore::Float));
      copyStorage(tcal_shape_[0], tcal_shape_[1], 4,
          reinterpret_cast<casacore::Float *>(tcal_storage_.get()),
          reinterpret_cast<casacore::Float *>(new_tcal_storage));
      tcal_storage_.reset(new_tcal_storage);
      num_tcal_storage_ = new_num_storage;
      redirect = true;
    }
    if (redirect) {
      tcal.takeStorage(tcal_shape_,
          reinterpret_cast<casacore::Float *>(tcal_storage_.get()), SHARE);
    }
  }

  MSDataRecord(MSDataRecord const &other) :
      MSDataRecord() {
    *this = other;
  }
  MSDataRecord &operator=(MSDataRecord const &other) {
    time = other.time;
    interval = other.interval;
    antenna_id = other.antenna_id;
    field_id = other.field_id;
    spw_id = other.spw_id;
    feed_id = other.feed_id;
    scan = other.scan;
    subscan = other.subscan;
    intent = other.intent;
    pol_type = other.pol_type;
    num_pol = other.num_pol;
    direction = other.direction;
    direction_slice.takeStorage(direction_slice.shape(), direction.data(),
        SHARE);
    flag_row = other.flag_row;
    setDataSize(other.data_shape_[0], other.data_shape_[1]);
    corr_type = other.corr_type;
    if (other.is_float_) {
      float_data = other.float_data;
    } else {
      complex_data = other.complex_data;
    }
    flag = other.flag;
    setTsysSize(other.tsys_shape_[0], other.tsys_shape_[1]);
    tsys = other.tsys;
    setTcalSize(other.tcal_shape_[0], other.tcal_shape_[1]);
    tcal = other.tcal;

    temperature = other.temperature;
    pressure = other.pressure;
    rel_humidity = other.rel_humidity;
    wind_speed = other.wind_speed;
    wind_direction = other.wind_direction;

    return *this;
  }

private:
  size_t const block_size_;
  size_t num_data_storage_;
  size_t num_tsys_storage_;
  size_t num_tcal_storage_;
  casacore::IPosition data_shape_;
  casacore::IPosition tsys_shape_;
  casacore::IPosition tcal_shape_;
  casacore::IPosition corr_type_shape_;
  casacore::Block<casacore::Int> corr_type_storage_;
  std::unique_ptr<void, sdfiller::Deleter> data_storage_;
  std::unique_ptr<void, sdfiller::Deleter> flag_storage_;
  std::unique_ptr<void, sdfiller::Deleter> tsys_storage_;
  std::unique_ptr<void, sdfiller::Deleter> tcal_storage_;
  std::unique_ptr<casacore::Float[]> sigma_storage_;
  casacore::Bool is_float_;

public:
// mandatory
  casacore::Double time;
  casacore::Double interval;
  casacore::Int antenna_id;
  casacore::Int field_id;
  casacore::Int spw_id;
  casacore::Int feed_id;
  casacore::Int scan;
  casacore::Int subscan;
  casacore::Int num_pol;
  casacore::String intent;
  casacore::String pol_type;
  casacore::Vector<casacore::Int> corr_type;
  casacore::Matrix<casacore::Double> direction;
  casacore::Matrix<casacore::Double> direction_slice;
  casacore::Matrix<casacore::Float> float_data;
  casacore::Matrix<casacore::Complex> complex_data;
  casacore::Matrix<casacore::Bool> flag;
  casacore::Bool flag_row;
  casacore::Vector<casacore::Float> sigma;
  casacore::Vector<casacore::Float> &weight;

  // optional
  casacore::Matrix<casacore::Float> tsys;
  casacore::Matrix<casacore::Float> tcal;

  casacore::Float temperature;
  casacore::Float pressure;
  casacore::Float rel_humidity;
  casacore::Float wind_speed;
  casacore::Float wind_direction;

};

} //# NAMESPACE SDFILLER - END
} //# NAMESPACE CASA - END

#endif /* SINGLEDISH_FILLER_DATARECORD_H_ */
