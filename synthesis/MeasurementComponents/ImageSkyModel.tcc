#include <casa/OS/HostInfo.h>
#include <synthesis/MeasurementComponents/ImageSkyModel.h>

namespace casa { //# NAMESPACE CASA - BEGIN

#define MEMFACTOR 8.0
#if !(defined (AIPS_64B))
#  undef  MEMFACTOR
#  define MEMFACTOR 18.0
#endif

template<class M>
casacore::TempImage<M>* ImageSkyModel::getTempImage(const casacore::TiledShape& imgShp,
                                          const casacore::CoordinateSystem& imgCoords,
                                          const casacore::uInt nMouthsToFeed)
{
  casacore::TempImage<M>* timgptr = NULL;
  casacore::Double memoryMB = casacore::HostInfo::memoryTotal() / 1024.0 / MEMFACTOR;

  if(nMouthsToFeed > 1) // esp. !0
    memoryMB /= nMouthsToFeed;

  if(memoryMB > 0.0){
    while(!timgptr && memoryMB > 1.0){
      try{
        timgptr = new casacore::TempImage<M> (imgShp, imgCoords, memoryMB);
        AlwaysAssert(timgptr, casacore::AipsError);
      }
      catch(...){
        memoryMB *= 0.5;
      }
    }
  }
  else{
    timgptr = new casacore::TempImage<M> (imgShp, imgCoords, 0.0);
  }
  AlwaysAssert(timgptr, casacore::AipsError);
  return timgptr;
}

} //# NAMESPACE CASA - END


