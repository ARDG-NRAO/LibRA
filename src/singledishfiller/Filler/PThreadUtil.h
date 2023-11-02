#ifndef _SINGLEDISH_PTHREAD_UTILS_H_
#define _SINGLEDISH_PTHREAD_UTILS_H_

#include <pthread.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <unistd.h>

using namespace std;

#define THROW_IF(condition, msg) \
  do { \
    if ((condition)) { \
      throw runtime_error((msg)); \
    } \
  } while (false)

namespace casa { //# NAMESPACE CASA - BEGIN
namespace sdfiller { //# NAMESPACE SDFILLER - BEGIN

class Mutex {

public:
  Mutex() :
      mutex_(PTHREAD_MUTEX_INITIALIZER) {
//    cout << "Mutex::Mutex()" << endl;
    int ret = pthread_mutex_init(&mutex_, NULL);
    THROW_IF(ret != 0, "Mutex::Mutex() failed to initalize mutex");
  }
  ~Mutex() noexcept(false) {
//    cout << "Mutex::~Mutex()" << endl;
    int ret = pthread_mutex_destroy(&mutex_);
    THROW_IF(ret != 0, "Mutex::~Mutex() failed to destroy mutex");
  }
  int lock() {
//    cout << "Mutex::lock()" << endl;
    int ret = pthread_mutex_lock(&mutex_);
    THROW_IF(ret != 0, "Mutex::lock() failed to lock mutex");
    return ret;
  }
  int unlock() {
//    cout << "Mutex::unlock()" << endl;
    int ret = pthread_mutex_unlock(&mutex_);
    THROW_IF(ret != 0, "Mutex::unlock() failed to unlock mutex");
    return ret;
  }
  int try_lock() {
//    cout << "Mutex::try_lock()" << endl;
    return pthread_mutex_trylock(&mutex_);
  }

protected:
  pthread_mutex_t mutex_;

  friend class PCondition;
};

class PCondition {
public:
  PCondition(Mutex *mutex) :
      mutex_(&(mutex->mutex_)) {
    int ret = pthread_cond_init(&cond_, NULL);
    THROW_IF(ret != 0,
        "PCondition::PCondition() failed to initialize pthread_cond_t");
  }

  virtual ~PCondition() noexcept(false) {
    int ret = pthread_cond_destroy(&cond_);
    THROW_IF(ret != 0,
        "PCondition::~PCondition() failed to destroy pthread_cond_t");
  }
  int lock() {
//    cout << "PCondition::lock()" << endl;
    return pthread_mutex_lock(mutex_);
  }

  int unlock() {
//    cout << "PCondition::unlock()" << endl;
    return pthread_mutex_unlock(mutex_);
  }

  int wait() {
//    cout << "PCondition::wait()" << endl;
    int ret = pthread_cond_wait(&cond_, mutex_);
    THROW_IF(ret != 0, "PCondition::wait() failed to block pthread_cond_t");
    return ret;
  }

  int signal() {
//    cout << "PCondition::signal()" << endl;
    int ret = pthread_cond_signal(&cond_);
    THROW_IF(ret != 0, "PCondition::signal() failed to release pthread_cond_t");
    return ret;
  }
private:
  pthread_mutex_t *mutex_;
  pthread_cond_t cond_;
};

// implementation of producer consumer model
template<class DataType, ssize_t BufferSize>
class ProducerConsumerModelContext {
public:
  typedef ProducerConsumerModelContext<DataType, BufferSize> _Context;

  // production function
  static void produce(_Context *context, DataType item) {
    context->lock();

    // wait until buffer becomes available for production
    while (context->buffer_is_full()) {
      context->producer_wait();
    }

    assert(!context->buffer_is_full());

    context->push_product(item);

    context->producer_next();

    // send a signal to consumer since something is produced
    context->consumer_signal();

    context->unlock();
  }

  // consumption function
  // return false if no more products available
  // otherwise return true
  static bool consume(_Context *context, DataType *item) {
    context->lock();

    // wait until something is produced
    while (context->buffer_is_empty()) {
      context->consumer_wait();
    }

    assert(!context->buffer_is_empty());

    context->pop_product(item);
    bool more_products = (*item != context->end_of_production_);

    context->consumer_next();

    // send a signal to consumer since there are available slot in buffer
    context->producer_signal();

    context->unlock();

    return more_products;
  }

  // it should be called when production complete
  static void complete_production(_Context *context) {
    produce(context, context->end_of_production_);
  }

  // constructor
  ProducerConsumerModelContext(DataType const terminator) :
      end_of_production_(terminator), num_product_in_buffer_(0),
      producer_index_(0), consumer_index_(0), mutex_(),
      consumer_condition_(&mutex_), producer_condition_(&mutex_) {
    //std::cout << "end_of_production = " << end_of_production_ << std::endl;
  }

  // destructor
  ~ProducerConsumerModelContext() {
  }

  // utility
  template<class T>
  static void locked_print(T msg, _Context *context) {
    context->lock();
    cout << msg << endl;
    context->unlock();
  }

private:
  int lock() {
    return mutex_.lock();
  }

  int unlock() {
    return mutex_.unlock();
  }

  int try_lock() {
    return mutex_.try_lock();
  }

  int producer_wait() {
    return producer_condition_.wait();
  }

  int producer_signal() {
    return producer_condition_.signal();
  }

  int consumer_wait() {
    return consumer_condition_.wait();
  }

  int consumer_signal() {
    return consumer_condition_.signal();
  }

  bool buffer_is_full() {
    return num_product_in_buffer_ >= BufferSize;
  }

  bool buffer_is_empty() {
    return num_product_in_buffer_ <= 0;
  }

  void producer_next() {
    producer_index_++;
    producer_index_ %= BufferSize;
    num_product_in_buffer_++;
  }

  void consumer_next() {
    consumer_index_++;
    consumer_index_ %= BufferSize;
    num_product_in_buffer_--;
  }

  void push_product(DataType item) {
    buffer_[producer_index_] = item;
  }

  void pop_product(DataType *item) {
    *item = buffer_[consumer_index_];
  }

  // terminator data
  // (product == end_of_production_) indicates that production
  // is completed.
  DataType const end_of_production_;
  DataType buffer_[BufferSize];
  ssize_t num_product_in_buffer_;
  ssize_t producer_index_;
  ssize_t consumer_index_;
  Mutex mutex_;
  PCondition consumer_condition_;
  PCondition producer_condition_;
};

void create_thread(pthread_t *tid, pthread_attr_t *attr, void *(*func)(void *),
    void *param);
void join_thread(pthread_t *tid, void **status);

} //# NAMESPACE SDFILLER - END
} //# NAMESPACE CASA - END

#endif /* _SINGLEDISH_PTHREAD_UTILS_H_ */
