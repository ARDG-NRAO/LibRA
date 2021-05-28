#ifndef CALIBRATION_VISCALENUM_H
#define CALIBRATION_VISCALENUM_H
namespace casa {
  class VisCalEnum{
  public:
    enum VCParType{COMPLEX=0, REAL, COMPLEXREAL};
    enum MatrixType{GLOBAL,MUELLER,JONES};
  };
};
#endif
