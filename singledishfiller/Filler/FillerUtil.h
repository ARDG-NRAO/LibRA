/*
 * FillerUtil.h
 *
 *  Created on: Feb 3, 2016
 *      Author: nakazato
 */
#include <cstdlib>

#ifndef SINGLEDISH_FILLER_FILLERUTIL_H_
#define SINGLEDISH_FILLER_FILLERUTIL_H_

//#define SINGLEDISHMSFILLER_DEBUG
#ifdef SINGLEDISHMSFILLER_DEBUG
#define POST_START std::cout << "Start " << __PRETTY_FUNCTION__ << std::endl
#define POST_END std::cout << "End " << __PRETTY_FUNCTION__ << std::endl
#else
#define POST_START
#define POST_END
#endif

namespace casa { //# NAMESPACE CASA - BEGIN
namespace sdfiller { //# NAMESPACE SDFILLER - BEGIN
struct Deleter {
  void operator()(void *p) {
    if (p) {
      free(p);
    }
  }
};
} //# NAMESPACE SDFILLER - BEGIN
} //# NAMESPACE CASA - END

#endif /* SINGLEDISH_FILLER_FILLERUTIL_H_ */
