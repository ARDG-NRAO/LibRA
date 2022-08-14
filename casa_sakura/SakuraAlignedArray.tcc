#include <cassert>
#include <iostream>

#include <libsakura/sakura.h>
//#include <libsakura/config.h>

#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Logging/LogOrigin.h>

namespace casa {
template<typename T>
inline SakuraAlignedArray<T>::SakuraAlignedArray(size_t num_data) : 
  num_data_(num_data) {
  casacore::LogIO logger(casacore::LogOrigin("SakuraAlignedArray", "SakuraAlignedArray", WHERE));
  logger << casacore::LogIO::DEBUGGING << "Constructing SakuraAlignedArray..." << casacore::LogIO::POST;

  initialize();

  logger << casacore::LogIO::DEBUGGING << "  Initial Address = " << storage_ << casacore::LogIO::POST;
  logger << casacore::LogIO::DEBUGGING << "  Aligned Address = " << data << casacore::LogIO::POST;
}

template<typename T>
inline SakuraAlignedArray<T>::SakuraAlignedArray(casacore::Vector<T> const &in_vector) : 
  num_data_(in_vector.nelements()) {
  casacore::LogIO logger(casacore::LogOrigin("SakuraAlignedArray", "SakuraAlignedArray", WHERE));
  logger << casacore::LogIO::DEBUGGING << "Constructing SakuraAlignedArray..." << casacore::LogIO::POST;

  initialize();

  T *ptr = data;
  for (size_t i = 0; i < num_data_; ++i) {
    ptr[i] = in_vector(i);
  }

  logger << casacore::LogIO::DEBUGGING << "  Initial Address = " << storage_ << casacore::LogIO::POST;
  logger << casacore::LogIO::DEBUGGING << "  Aligned Address = " << data << casacore::LogIO::POST;
}

template<typename T>
inline void SakuraAlignedArray<T>::initialize() {
  storage_ = nullptr;
  data = nullptr;
  casaVector = nullptr;

  size_t size_required = sizeof(T) * num_data_;
  size_t size_of_arena = size_required + LIBSAKURA_SYMBOL(GetAlignment)() - 1;
  storage_ = malloc(size_of_arena);
  if (storage_ == nullptr) {
    data = nullptr;
    throw std::bad_alloc();
  }
  data = reinterpret_cast<T *>(LIBSAKURA_SYMBOL(AlignAny)(
				size_of_arena, storage_, size_required));
  assert(data != nullptr);
  assert(LIBSAKURA_SYMBOL(IsAligned)(data));

  casacore::Vector<T> alignedCasaVector = casacore::Vector<T>(casacore::IPosition(1, num_data_), data,
                                                              casacore::SHARE);
  assert(data == alignedCasaVector.cbegin());
  casaVector = &alignedCasaVector;
}

template<typename T>
inline SakuraAlignedArray<T>::~SakuraAlignedArray() {
  casacore::LogIO logger(casacore::LogOrigin("SakuraAlignedArray", "~SakuraAlignedArray", WHERE));
  logger << casacore::LogIO::DEBUGGING << "Destructing SakuraAlignedArray..." << casacore::LogIO::POST;

  free(storage_);
}

}  // End of casa namespace.
