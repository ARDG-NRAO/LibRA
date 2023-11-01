#ifndef _SYNTHESYS_POINTING_DIRECTION_PROJECTOR_H_
#define _SYNTHESYS_POINTING_DIRECTION_PROJECTOR_H_

#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Logging/LogIO.h>

namespace casa {
// Partially projection code from ASAP GenericEdgeDetector
class Projector {
public:
  Projector();
  virtual ~Projector() {
  }
  void setDirection(const casacore::Matrix<casacore::Double> &dir);
  void setReferenceCoordinate(casacore::Double const lat, casacore::Double const lon);
  void setReferencePixel(casacore::Double const refx, casacore::Double const refy);
  void unsetReferenceCoordinate();
  void unsetReferencePixel();
  casacore::Bool isReferenceCoordinateSet() const {return user_defined_center_;}
  casacore::Bool isReferencePixelSet() const {return user_defined_pcenter_;}
  void getUserDefinedReferenceCoordinate(casacore::Double &lat, casacore::Double &lon) {
    lat = cenx_user_;
    lon = ceny_user_;
  }
  void getUserDefinedReferencePixel(casacore::Double &refx, casacore::Double &refy) {
    refx = pcenx_user_;
    refy = pceny_user_;
  }
  virtual const casacore::Matrix<casacore::Double>& project() = 0;
protected:
  // From asap/src/MathUtils
  void rotateRA(casacore::Vector<casacore::Double> &v);
  // casacore::Input data
  casacore::Matrix<casacore::Double> dir_;

  // logging
  casacore::LogIO os_;

private:
  // private attributes
  casacore::Double cenx_user_;
  casacore::Double ceny_user_;
  casacore::Bool user_defined_center_;
  casacore::Double pcenx_user_;
  casacore::Double pceny_user_;
  casacore::Bool user_defined_pcenter_;
};

class OrthographicProjector: public Projector {
public:
  OrthographicProjector(casacore::Float pixel_scale = 0.5);
  virtual ~OrthographicProjector();
  const casacore::Matrix<casacore::Double>& project();
  const casacore::Vector<casacore::Double>& p_center() const {
    return p_center_;
  }
  const casacore::Vector<casacore::Double>& p_size() const {
    return p_size_;
  }
  casacore::Double pixel_size() const {
    return dy_;
  }

private:
  void scale_and_center();
  // options
  casacore::Float pixel_scale_;
  // pixel info
  casacore::Double cenx_;
  casacore::Double ceny_;
  casacore::Double pcenx_;
  casacore::Double pceny_;
  casacore::uInt nx_;
  casacore::uInt ny_;
  casacore::Double dx_;
  casacore::Double dy_;

  // storage for projection
  casacore::Matrix<casacore::Double> pdir_;

  // projection parameters computed from input directions
  casacore::Vector<casacore::Double> p_center_;
  casacore::Vector<casacore::Double> p_size_;
};

} // end of namespace casa

#endif /* _SYNTHESYS_POINTING_DIRECTION_PROJECTOR_H_ */
