//# SDPosInterpolator.cc: Implementation of SDPosInterpolator class
//# Copyright (C) 1997,1998,1999,2000,2001,2002,2003
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$

#include <synthesis/Utilities/SDPosInterpolator.h>

using namespace casacore;
namespace casa {

SDPosInterpolator::SDPosInterpolator(const VisBuffer& vb, const String& pointingDirCol_p) {
  setup(vb, pointingDirCol_p);
}
SDPosInterpolator::SDPosInterpolator(const vi::VisBuffer2& vb, const String& pointingDirCol_p) {
  setup(vb, pointingDirCol_p);
}
SDPosInterpolator::SDPosInterpolator(const Vector<Vector<Double> >& time,
                                     const Vector<Vector<Vector<Double> > >& dir) {
  setup(time, dir);
}
SDPosInterpolator::~SDPosInterpolator() {}

void SDPosInterpolator::setup(const Vector<Vector<Double> >& time,
                              const Vector<Vector<Vector<Double> > >& dir) {
  //(1)get number of pointing data for each antennaID
  Int nant = time.nelements();
  Vector<uInt> nPointingData(nant);
  nPointingData = 0;
  for (Int iant = 0; iant < nant; ++iant) {
    nPointingData(iant) = time(iant).nelements();
  }

  //(2)setup spline coefficients for each antenna ID
  timePointing.resize(nant);
  dirPointing.resize(nant);
  splineCoeff.resize(nant);
  doSplineInterpolation.resize(nant);
  doSplineInterpolation = false;
  for (Int i = 0; i < nant; ++i) {
    if (nPointingData(i) < 4) continue;
    
    doSplineInterpolation(i) = true;
    timePointing(i).resize(nPointingData(i));
    dirPointing(i).resize(nPointingData(i));
    splineCoeff(i).resize(nPointingData(i) - 1);
    for (uInt j = 0; j < dirPointing(i).nelements(); ++j) {
      dirPointing(i)(j).resize(2);
    }
    for (uInt j = 0; j < splineCoeff(i).nelements(); ++j) {
      splineCoeff(i)(j).resize(2);
      splineCoeff(i)(j)(0).resize(4); // x
      splineCoeff(i)(j)(1).resize(4); // y
    }
    
    Int npoi = nPointingData(i);
    for (Int j = 0; j < npoi; ++j) {
      timePointing(i)(j) = time(i)(j);
      for (Int k = 0; k < 2; ++k) {
        dirPointing(i)(j)(k) = dir(i)(j)(k);
      }
    }
      
    calcSplineCoeff(timePointing(i), dirPointing(i), splineCoeff(i));
  }

  //(3) keep time range
  timeRangeStart.resize(nant);
  timeRangeEnd.resize(nant);
  for (Int iant = 0; iant < nant; ++iant) {
    timeRangeStart(iant) = timePointing(iant)(0);
    timeRangeEnd(iant)   = timePointing(iant)(timePointing(iant).nelements()-1);
  }
}

void SDPosInterpolator::setup(const VisBuffer& vb, const String& pointingDirCol_p) {
  const MSPointingColumns& act_mspc = vb.msColumns().pointing();
  auto check_col = [&](Bool isnull){
    if (isnull) {
      cerr << "No " << pointingDirCol_p << " column in POINTING table" << endl;
    }
  };
  std::function<Vector<Double>(Int)> get_direction;

  //(0)check POINTING table and set function to obtain direction data
  if (pointingDirCol_p == "TARGET") {
    get_direction = [&](Int idx){
      return act_mspc.targetMeas(idx).getAngle("rad").getValue();
    };
  } else if (pointingDirCol_p == "POINTING_OFFSET") {
    check_col(act_mspc.pointingOffsetMeasCol().isNull());
    get_direction = [&](Int idx){
      return act_mspc.pointingOffsetMeas(idx).getAngle("rad").getValue();
    };
  } else if (pointingDirCol_p == "SOURCE_OFFSET") {
    check_col(act_mspc.sourceOffsetMeasCol().isNull());
    get_direction = [&](Int idx){
      return act_mspc.sourceOffsetMeas(idx).getAngle("rad").getValue();
    };
  } else if (pointingDirCol_p == "ENCODER") {
    check_col(act_mspc.encoderMeas().isNull());
    get_direction = [&](Int idx){
      return act_mspc.encoderMeas()(idx).getAngle("rad").getValue();
    };
  } else {
    get_direction = [&](Int idx){
      return act_mspc.directionMeas(idx).getAngle("rad").getValue();
    };
  }

  //(1)get number of pointing data for each antennaID
  Int nant = vb.msColumns().antenna().name().nrow();
  Vector<uInt> nPointingData(nant);
  nPointingData = 0;
  Int npoi = act_mspc.time().nrow();
  for (Int i = 0; i < npoi; ++i) {
    nPointingData(act_mspc.antennaId()(i)) += 1;
  }

  //(2)setup spline coefficients for each antenna ID that
  //   appear in the main table (spectral data) if there
  //   are enough number of pointing data (4 or more).
  //   in case there exists antenna ID for which not enough
  //   (i.e., 1, 2 or 3) pointing data are given, linear
  //   interpolation is applied for that antenna ID as
  //   previously done.
  timePointing.resize(nant);
  dirPointing.resize(nant);
  splineCoeff.resize(nant);
  doSplineInterpolation.resize(nant);
  doSplineInterpolation = false;
  for (Int i = 0; i < nant; ++i) {
    if (nPointingData(i) < 4) continue;
    
    doSplineInterpolation(i) = true;
    timePointing(i).resize(nPointingData(i));
    dirPointing(i).resize(nPointingData(i));
    splineCoeff(i).resize(nPointingData(i) - 1);
    for (uInt j = 0; j < dirPointing(i).nelements(); ++j) {
      dirPointing(i)(j).resize(2);
    }
    for (uInt j = 0; j < splineCoeff(i).nelements(); ++j) {
      splineCoeff(i)(j).resize(2);
      splineCoeff(i)(j)(0).resize(4); // x
      splineCoeff(i)(j)(1).resize(4); // y
    }

    //set ptime array etc. need for spline calculation...
    Int tidx = 0;
    for (Int j = 0; j < npoi; ++j) {
      if (act_mspc.antennaId()(j) != i) continue;
      
      timePointing(i)(tidx) = act_mspc.time()(j);
      dirPointing(i)(tidx) = get_direction(j);
      tidx++;
    }
    
    calcSplineCoeff(timePointing(i), dirPointing(i), splineCoeff(i));
  }

  //(3) keep time range
  timeRangeStart.resize(nant);
  timeRangeEnd.resize(nant);
  for (Int iant = 0; iant < nant; ++iant) {
    timeRangeStart(iant) = timePointing(iant)(0);
    timeRangeEnd(iant)   = timePointing(iant)(timePointing(iant).nelements()-1);
  }
}

void SDPosInterpolator::setup(const vi::VisBuffer2& vb, const String& pointingDirCol_p) {
  const MSPointingColumns& act_mspc = vb.subtableColumns().pointing();
  auto check_col = [&](Bool isnull){
    if (isnull) {
      cerr << "No " << pointingDirCol_p << " column in POINTING table" << endl;
    }
  };
  std::function<Vector<Double>(Int)> get_direction;

  //(0)check POINTING table and set function to obtain direction data
  if (pointingDirCol_p == "TARGET") {
    get_direction = [&](Int idx){
      return act_mspc.targetMeas(idx).getAngle("rad").getValue();
    };
  } else if (pointingDirCol_p == "POINTING_OFFSET") {
    check_col(act_mspc.pointingOffsetMeasCol().isNull());
    get_direction = [&](Int idx){
      return act_mspc.pointingOffsetMeas(idx).getAngle("rad").getValue();
    };
  } else if (pointingDirCol_p == "SOURCE_OFFSET") {
    check_col(act_mspc.sourceOffsetMeasCol().isNull());
    get_direction = [&](Int idx){
      return act_mspc.sourceOffsetMeas(idx).getAngle("rad").getValue();
    };
  } else if (pointingDirCol_p == "ENCODER") {
    check_col(act_mspc.encoderMeas().isNull());
    get_direction = [&](Int idx){
      return act_mspc.encoderMeas()(idx).getAngle("rad").getValue();
    };
  } else {
    get_direction = [&](Int idx){
      return act_mspc.directionMeas(idx).getAngle("rad").getValue();
    };
  }

  //(1)get number of pointing data for each antennaID
  Int nant = vb.subtableColumns().antenna().name().nrow();
  Vector<uInt> nPointingData(nant);
  nPointingData = 0;
  Int npoi = act_mspc.time().nrow();
  for (Int i = 0; i < npoi; ++i) {
    nPointingData(act_mspc.antennaId()(i)) += 1;
  }

  //(2)setup spline coefficients for each antenna ID that
  //   appear in the main table (spectral data) if there
  //   are enough number of pointing data (4 or more).
  //   in case there exists antenna ID for which not enough
  //   (i.e., 1, 2 or 3) pointing data are given, linear
  //   interpolation is applied for that antenna ID as
  //   previously done.
  timePointing.resize(nant);
  dirPointing.resize(nant);
  splineCoeff.resize(nant);
  doSplineInterpolation.resize(nant);
  doSplineInterpolation = false;
  for (Int i = 0; i < nant; ++i) {
    if (nPointingData(i) < 4) continue;

    doSplineInterpolation(i) = true;
    timePointing(i).resize(nPointingData(i));
    dirPointing(i).resize(nPointingData(i));
    splineCoeff(i).resize(nPointingData(i) - 1);
    for (uInt j = 0; j < dirPointing(i).nelements(); ++j) {
      dirPointing(i)(j).resize(2);
    }
    for (uInt j = 0; j < splineCoeff(i).nelements(); ++j) {
      splineCoeff(i)(j).resize(2);
      splineCoeff(i)(j)(0).resize(4); // x
      splineCoeff(i)(j)(1).resize(4); // y
    }

    //set ptime array etc. need for spline calculation...
    Int tidx = 0;
    for (Int j = 0; j < npoi; ++j) {
      if (act_mspc.antennaId()(j) != i) continue;

      timePointing(i)(tidx) = act_mspc.time()(j);
      dirPointing(i)(tidx) = get_direction(j);
      tidx++;
    }

    calcSplineCoeff(timePointing(i), dirPointing(i), splineCoeff(i));
  }

  //(3) keep time range
  timeRangeStart.resize(nant);
  timeRangeEnd.resize(nant);
  for (Int iant = 0; iant < nant; ++iant) {
    timeRangeStart(iant) = timePointing(iant)(0);
    timeRangeEnd(iant)   = timePointing(iant)(timePointing(iant).nelements()-1);
  }
}

void SDPosInterpolator::calcSplineCoeff(const Vector<Double>& time,
                                        const Vector<Vector<Double> >& dir,
                                        Vector<Vector<Vector<Double> > >& coeff) {
  Vector<Double> h, vx, vy;
  Vector<Double> a;
  Vector<Double> c;
  Vector<Double> alpha, beta, gamma;
  Vector<Double> wx, wy;
  Vector<Double> ux, uy;

  Int const num_data = time.nelements();
  h.resize(num_data-1);
  vx.resize(num_data-1);
  vy.resize(num_data-1);
  a.resize(num_data-1);
  c.resize(num_data-1);
  alpha.resize(num_data-1);
  beta.resize(num_data-1);
  gamma.resize(num_data-1);
  wx.resize(num_data-1);
  wy.resize(num_data-1);
  ux.resize(num_data);
  uy.resize(num_data);

  h(0) = time(1) - time(0);
  for (Int i = 1; i < num_data-1; ++i) {
    h(i) = time(i+1) - time(i);
    vx(i) = 6.0*((dir(i+1)(0)-dir(i)(0))/h(i) - (dir(i)(0)-dir(i-1)(0))/h(i-1));
    vy(i) = 6.0*((dir(i+1)(1)-dir(i)(1))/h(i) - (dir(i)(1)-dir(i-1)(1))/h(i-1));
    a(i) = 2.0*(time(i+1) - time(i-1));
    c(i) = h(i);
    gamma(i) = c(i);
  }
  alpha(2) = c(1)/a(1);
  for (Int i = 3; i < num_data-1; ++i) {
    alpha(i) = c(i-1)/(a(i-1) - alpha(i-1)*c(i-2));
  }
  beta(1) = a(1);
  for (Int i = 2; i < num_data-2; ++i) {
    beta(i) = c(i)/alpha(i+1);
  }
  beta(num_data-2) = a(num_data-2) - alpha(num_data-2) * c(num_data-3);
  wx(0) = 0.0;
  wx(1) = vx(1);
  wy(0) = 0.0;
  wy(1) = vy(1);
  for (Int i = 2; i < num_data-1; ++i) {
    wx(i) = vx(i) - alpha(i)*wx(i-1);
    wy(i) = vy(i) - alpha(i)*wy(i-1);
  }
  ux(num_data-1) = 0.0;
  uy(num_data-1) = 0.0;
  for (Int i = num_data-2; i >= 1; --i) {
    ux(i) = (wx(i) - gamma(i)*ux(i+1))/beta(i);
    uy(i) = (wy(i) - gamma(i)*uy(i+1))/beta(i);
  }
  ux(0) = 0.0;
  uy(0) = 0.0;

  for (Int i = 0; i < num_data-1; ++i) {
    coeff(i)(0)(0) = dir(i)(0);
    coeff(i)(1)(0) = dir(i)(1);
    coeff(i)(0)(1) = (dir(i+1)(0)-dir(i)(0))/(time(i+1)-time(i)) - (time(i+1)-time(i))*(2.0*ux(i)+ux(i+1))/6.0;
    coeff(i)(1)(1) = (dir(i+1)(1)-dir(i)(1))/(time(i+1)-time(i)) - (time(i+1)-time(i))*(2.0*uy(i)+uy(i+1))/6.0;
    coeff(i)(0)(2) = ux(i)/2.0;
    coeff(i)(1)(2) = uy(i)/2.0;
    coeff(i)(0)(3) = (ux(i+1)-ux(i))/(time(i+1)-time(i))/6.0;
    coeff(i)(1)(3) = (uy(i+1)-uy(i))/(time(i+1)-time(i))/6.0;
  }
}

MDirection SDPosInterpolator::interpolateDirectionMeasSpline(const MSPointingColumns& mspc,
                                                             const Double& time,
                                                             const Int& index,
                                                             const Int& antid) {
  Int lastIndex = timePointing(antid).nelements() - 1;
  Int aindex = lastIndex;
  for (uInt i = 0; i < timePointing(antid).nelements(); ++i) {
    if (time < timePointing(antid)(i)) {
      aindex = i-1;
      break;
    }
  }
  if (aindex < 0) aindex = 0;
  if (lastIndex <= aindex) aindex = lastIndex - 1;

  auto const &coeff = splineCoeff(antid)(aindex);
  Double dt = time - timePointing(antid)(aindex);
  Vector<Double> newdir(2);
  newdir(0) = coeff(0)(0) + coeff(0)(1)*dt + coeff(0)(2)*dt*dt + coeff(0)(3)*dt*dt*dt;
  newdir(1) = coeff(1)(0) + coeff(1)(1)*dt + coeff(1)(2)*dt*dt + coeff(1)(3)*dt*dt*dt;
  
  Quantity rDirLon(newdir(0), "rad");
  Quantity rDirLat(newdir(1), "rad");
  auto const &directionMeasColumn = mspc.directionMeasCol();
  MDirection::Ref rf(directionMeasColumn.measDesc().getRefCode());
  if (directionMeasColumn.measDesc().isRefCodeVariable()) {
    rf = mspc.directionMeas(index).getRef();
  }

  return MDirection(rDirLon, rDirLat, rf);
}

Vector<Vector<Vector<Vector<Double> > > > SDPosInterpolator::getSplineCoeff() {
  return splineCoeff;
}

Bool SDPosInterpolator::inTimeRange(const Double& time, const Int& antid) {
  Bool inrange = false;
  if ((timeRangeStart(antid) <= time) && (time <= timeRangeEnd(antid))) {
    inrange = true;
  }
  return inrange;
}

} //#End casa namespace
