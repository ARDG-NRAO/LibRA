#include <cassert>
#include <memory>
#include <iostream>

#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/Logging.h>
#include <casacore/measures/Measures/MDirection.h>
#include <casacore/measures/TableMeasures/ScalarMeasColumn.h>
#include <casacore/ms/MSSel/MSSelection.h>
#include <casacore/tables/TaQL/ExprNode.h>

#include <synthesis/Utilities/PointingDirectionCalculator.h>
#include <synthesis/Utilities/SingleDishBeamUtil.h>

using namespace std;
using namespace casacore;

#define _ORIGIN LogOrigin("SingleDishBeamUtil", __func__, WHERE)

namespace casa { //# NAMESPACE CASA - BEGIN

SingleDishBeamUtil::SingleDishBeamUtil(const MeasurementSet &ms,
				       const String &referenceFrame,
				       const String &movingSource,
				       const String &pointingColumn,
				       const String &antenna)
       : referenceFrame_(referenceFrame), movingSource_(movingSource),
	 pointingColumn_(pointingColumn), antSel_(antenna)
{
  ms_ = new MeasurementSet(ms);
  directionUnit_ = Unit("rad");
}

Bool SingleDishBeamUtil::getMapPointings(Matrix<Double> &pointingList) {
    try {
        PointingDirectionCalculator calc(*ms_);

        calc.setDirectionColumn(pointingColumn_);
	calc.selectData(antSel_);
        calc.setFrame(referenceFrame_);
        MDirection::Types refType = MDirection::J2000; // any non planet value
        Bool status = False;
        status = MDirection::getType(refType, movingSource_);
        Bool doMovingSourceCorrection = (status == True &&
                MDirection::N_Types < refType &&
                refType < MDirection::N_Planets);
        if (doMovingSourceCorrection) {
            calc.setMovingSource(movingSource_);
        }
        calc.setDirectionListMatrixShape(PointingDirectionCalculator::COLUMN_MAJOR);

        pointingList = calc.getDirection();
	directionUnit_ = Unit("rad");
        Vector<Double> longitude = pointingList.column(0);
        Vector<Double> latitude = pointingList.column(1);

	if (longitude.size() < 2) return True; // no need for boundary check.

        // Diagnose if longitude values are divided by periodic boundary surface
        // (+-pi or 0, 2pi)
        // In this case, mean of longitude should be around 0 (+-pi) or pi (0,2pi)
        // and stddev of longitude array be around pi.
        Double longitudeMean = mean(longitude);
        Double longitudeStddev = stddev(longitude);
        if (longitudeStddev > 2.0 / 3.0 * C::pi) {
            // likely to be the case
            if (abs(longitudeMean) < 0.5 * C::pi) {
                // periodic boundary surface would be +-pi
                for (size_t i = 0; i < longitude.nelements(); ++i) {
                    if (longitude[i] < 0.0) {
                        longitude[i] += C::_2pi;
                    }
                }
            }
            else if (abs(longitudeMean - C::pi) < 0.5 * C::pi ) {
                // periodic boundary surface would be 0,2pi
                for (size_t i = 0; i < longitude.nelements(); ++i) {
                    if (longitude[i] < C::pi) {
                        longitude[i] += C::_2pi;
                    }
                }
            }
        }
    }
    catch (AipsError &e) {
        LogIO os(LogOrigin("Imager", "getMapPointings", WHERE));
        os << LogIO::SEVERE << "Failed due to the rror \"" << e.getMesg() << "\"" << LogIO::POST;
        return False;
    }
    catch (...) {
        LogIO os(LogOrigin("Imager", "getMapPointings", WHERE));
        os << LogIO::SEVERE << "Failed due to unknown error" << LogIO::POST;
        throw;
        return False;
    }
    return True;
}

  Bool SingleDishBeamUtil::getPointingSamplingRaster(Quantum<Vector<Double>> &sampling, Quantity &positionAngle) {
    LogIO os(_ORIGIN);
    os << "calculating sampling interval assuming raster scan." << LogIO::POST;
    try {
      Vector<Double> samplingVal(2, 0.0);
      // Get time sorted pointing (sort by ANTENNA, TIME)
      Matrix<Double> pointingList;
      ThrowIf (!getMapPointings(pointingList), "Failed to get map pointings");
      os << "got " << pointingList.column(0).size() << " pointings of " << antSel_  << LogIO::POST;
      // Get timestamps
      Block<String> sortColumns(2);
      sortColumns[0] = "ANTENNA1";
      sortColumns[1] = "TIME";
      MSSelection thisSelection;
      thisSelection.setAntennaExpr(antSel_);
      TableExprNode exprNode = thisSelection.getTEN(&(*ms_));
      if (exprNode.isNull()) {
	throw(AipsError("Invalid antenna selection"));
      }
      MeasurementSet tmp = (*ms_)(exprNode);
      CountedPtr<MeasurementSet> sortedMS = new MeasurementSet(tmp.sort(sortColumns));
      AlwaysAssert(sortedMS->nrow() == pointingList.column(0).size(), AipsError);
      ScalarMeasColumn<MEpoch> timeColumn_;
      timeColumn_.attach(*sortedMS, "TIME");
      // Get time and pointings of unique time stamp per antenna
      Vector<uInt> uniqueAntTimeIdx(sortedMS->nrow());
      Vector<Double> uniqueAntTimes(sortedMS->nrow());
      Double currentTime = timeColumn_.convert(0, MEpoch::UTC).get("s").getValue();
      uInt itime = 0;
      // Initial time stamp
      uniqueAntTimeIdx(itime) = 0;
      uniqueAntTimes(itime) = currentTime;
      ++itime;
      for (uInt i = 1; i < sortedMS->nrow(); ++i) {
	Double nextTime = timeColumn_.convert(i, MEpoch::UTC).get("s").getValue();
	if (abs(nextTime-currentTime) > 1.e-14*currentTime) {
	  uniqueAntTimeIdx(itime) = i;
	  uniqueAntTimes(itime) = nextTime;
	  ++itime;
	}
	currentTime = nextTime;
      }
      if (itime != uniqueAntTimeIdx.size()) {
	uniqueAntTimeIdx.resize(itime, True);
	uniqueAntTimes.resize(itime, True); 
      }
      os << LogIO::DEBUG1 << uniqueAntTimeIdx.size() << " unique time stamps"  << LogIO::POST;
      if (uniqueAntTimes.size() == 1) {
	samplingVal = 0.0;
	sampling = Quantum< Vector<Double> >(samplingVal, directionUnit_);
	positionAngle = Quantity(0.0, "rad");
	os << LogIO::NORMAL
	   << "Got only one pointing. exiting without calculating sampling interval."
	   << LogIO::POST;
	return True;
      }
      Vector<Double> longitude(uniqueAntTimes.size());
      Vector<Double> latitude(longitude.size());
      {
	Vector<Double> all_longitude = pointingList.column(0); // time sorted pointing list
	Vector<Double> all_latitude = pointingList.column(1);
	for (uint i = 0; i < uniqueAntTimeIdx.size(); ++i) {
	  longitude(i) = all_longitude[uniqueAntTimeIdx[i]];
	  latitude(i) = all_latitude[uniqueAntTimeIdx[i]];
	}
      }
      // calculate pointing interval assuming RASTER
      Vector<Double> delta_lon(longitude.size()-1);
      Vector<Double> delta_lat(delta_lon.size());
      Double min_lat, max_lat;
      minMax(min_lat, max_lat, latitude);
      Double center_lat = 0.5*(min_lat+max_lat);
      for (size_t i=0; i < delta_lon.size(); ++i) {
	Double dlon = (longitude[i+1]-longitude[i])*cos(center_lat);
	delta_lon[i] = abs(dlon) > 1.e-8 ? dlon : 1.e-8;
	delta_lat[i] = latitude[i+1]-latitude[i];
      }
      {	// sampling interval along scan row
	Vector<Double> delta2(delta_lon.size());
	delta2 = square(delta_lon) + square(delta_lat);
	samplingVal(0) = sqrt(median(delta2));
	os << LogIO::DEBUG1 << "sampling interval along scan: " << samplingVal(0)
	   << " " << directionUnit_.getName() << LogIO::POST;
      }
      { // position angle
	Vector<Double> delta_tan(delta_lon.size());
	delta_tan = delta_lat/delta_lon;
	Double positionAngleVal = atan(median(delta_tan));
	positionAngleVal = std::isfinite(positionAngleVal) ? positionAngleVal: 0.5*C::pi;
	positionAngle = Quantity(positionAngleVal, "rad");
	os << LogIO::DEBUG1 << "position angle of scan direction: " << positionAngle << LogIO::POST;
      }
      { // sampling interval orthogonal to scan row
	vector<uInt> gap_idx(0);
	uInt numAntGap = 0;
	os << LogIO::DEBUG1 << "start analysing raster pattern by time gap " << LogIO::POST;
	{// detect raster gap by time interval
	  Vector<Double> deltaTime(uniqueAntTimes.size()-1);
	  Vector<Double> positiveTimeGap(deltaTime.size());
	  uInt itime = 0;
	  for (uInt i = 0; i < deltaTime.size(); ++i) {
	    deltaTime[i] = uniqueAntTimes[i+1] - uniqueAntTimes[i];
	    if (deltaTime[i] > 0.0) {
	      positiveTimeGap[itime] = deltaTime[i];
	      ++itime;
	    }
	  }
	  positiveTimeGap.resize(itime, True);
	  Double medianInterval5 = Double(5)*median(positiveTimeGap);
	  os << LogIO::DEBUG1 << "Gap interval threshold = " << medianInterval5  << LogIO::POST;
	  for (uInt i = 0; i < deltaTime.size(); ++i) {
	    if (deltaTime[i] > medianInterval5) {// raster row gap
	      gap_idx.push_back(i);
	    } else if (deltaTime[i] < 0.0 || i == deltaTime.size()-1) { // antenna gap
	      gap_idx.push_back(i);
	      ++numAntGap;
	    }
	  }
	}
	if (gap_idx.size()==numAntGap) {// no gap detected.
	  os << LogIO::NORMAL << "No time gap found in scans. The scan pattern may not be RASTER. Median sampling interval will be returned." << LogIO::POST;
	  samplingVal(1) = 0.0;
	  sampling = Quantum< Vector<Double> >(samplingVal, directionUnit_);
	  os << "sampling interval: " << sampling << ", pa: " << positionAngle  << LogIO::POST;
	  return True;
	}
	os << LogIO::DEBUG1 << gap_idx.size() << " raster rows detected" << LogIO::POST;
	//os << LogIO::DEBUGGING << "gap idx = " << Vector<uInt>(gap_idx) << LogIO::POST;
	// a unit vector of orthogonal direction
	Vector<Double> orth_vec(2);
	orth_vec(0) = cos(positionAngle.getValue("rad")+0.5*C::pi);
	orth_vec(1) = sin(positionAngle.getValue("rad")+0.5*C::pi);
	os << LogIO::DEBUG1 << "orthogonal vector = " << orth_vec << LogIO::POST;
	// median lon, lat in each raster row
	Vector<Double> typical_lon(gap_idx.size());
	Vector<Double> typical_lat(gap_idx.size());
	for (uInt i = 0; i < gap_idx.size(); ++i) {
	  uInt start_idx, num_dump;
	  if (i==0) start_idx = 0;
	  else start_idx = gap_idx[i-1] + 1;
	  num_dump = gap_idx[i]-start_idx + 1;
	  typical_lon(i) = median(longitude(Slice(start_idx, num_dump)));
	  typical_lat(i) = median(latitude(Slice(start_idx, num_dump)));
	}
	os << LogIO::DEBUG1 << "Typical longitude of scan row (first 10 at max) = "
	   << typical_lon(Slice(0, min(typical_lon.size(), 10))) << LogIO::POST;
	os << LogIO::DEBUG1 << "Typical latitude of scan row (first 10 at max) = "
	   << typical_lat(Slice(0, min(typical_lat.size(), 10))) << LogIO::POST;
	Vector<Double> orth_dist(typical_lon.size()-1);
	for (uInt i = 0; i < typical_lon.size()-1; ++i) {
	  Double delta_row_lon, delta_row_lat;
	  delta_row_lon = typical_lon(i) - typical_lon(i+1);
	  delta_row_lat = typical_lat(i) - typical_lat(i+1);
	  // product of orthogonal vector and row gap vector
	  orth_dist(i) = abs(delta_row_lon*orth_vec(0) + 
			     delta_row_lat*orth_vec(1));
	}
	samplingVal(1) = median(orth_dist);
	os << LogIO::DEBUG1 << "sampling interval between scan row: " << samplingVal(1)
	   << " " << directionUnit_.getName() << LogIO::POST;
      }
      sampling = Quantum<Vector<Double>>(samplingVal, directionUnit_);
    }
    catch (AipsError &e) {
        os << LogIO::SEVERE << "Failed due to the rror \"" << e.getMesg() << "\"" << LogIO::POST;
        return False;
    }
    catch (...) {
        os << LogIO::SEVERE << "Failed due to unknown error" << LogIO::POST;
        throw;
        return False;
    }
    os << LogIO::NORMAL << "sampling interval: " << sampling
       << ", pa: " << positionAngle  << LogIO::POST;
    return True;
}

} //# NAMESPACE CASA - END
