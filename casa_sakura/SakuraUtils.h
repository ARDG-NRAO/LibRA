#ifndef _CASA_SAKURA_UTILS_H_
#define _CASA_SAKURA_UTILS_H_

#include <iostream>
#include <string>

//#include <libsakura/sakura.h>

#include <casa/aipstype.h>

namespace casa { //# NAMESPACE CASA - BEGIN

class SakuraUtils {
public:
  static casacore::Bool InitializeSakura();
  static void CleanUpSakura();
  static casacore::Bool IsSakuraInitialized();
private:
  static casacore::Bool is_initialized_;
};

class SakuraDriver {
public:
  SakuraDriver();
  ~SakuraDriver();
private:
  static int num_instances_;
};    

} //# NAMESPACE CASA - END
  
#endif /* _CASA_SAKURA_UTILS_H_ */
