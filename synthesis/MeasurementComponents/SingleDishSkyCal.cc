//# SingleDishSkyCal.cc: implements SingleDishSkyCal
//# Copyright (C) 2003
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

//# Includes
#include <iostream>
#include <sstream>
#include <iomanip>
#include <map>
#include <type_traits>

#include <casa/OS/File.h>
#include <casa/Arrays/MaskedArray.h>
#include <casa/Arrays/MaskArrMath.h>
#include <tables/TaQL/TableParse.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/ScalarColumn.h>
#include <ms/MeasurementSets/MeasurementSet.h>
#include <ms/MeasurementSets/MSState.h>
#include <ms/MeasurementSets/MSSpectralWindow.h>
#include <ms/MeasurementSets/MSIter.h>
#include <synthesis/MeasurementComponents/MSMetaInfoForCal.h>
#include <synthesis/MeasurementComponents/SingleDishSkyCal.h>
#include <synthesis/CalTables/CTGlobals.h>
#include <synthesis/CalTables/CTMainColumns.h>
#include <synthesis/CalTables/CTInterface.h>
#include <ms/MSSel/MSSelection.h>
#include <ms/MSSel/MSSelectionTools.h>
#include <synthesis/Utilities/PointingDirectionCalculator.h>
#include <synthesis/Utilities/PointingDirectionProjector.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <libsakura/sakura.h>
#include <cassert>

// Debug Message Handling
// if SDCALSKY_DEBUG is defined, the macro debuglog and
// debugpost point standard output stream (std::cout and
// std::endl so that debug messages are sent to standard
// output. Otherwise, these macros basically does nothing.
// "Do nothing" behavior is implemented in NullLogger
// and its associating << operator below.
//
// Usage:
// Similar to standard output stream.
//
//   debuglog << "Any message" << any_value << debugpost;
//
//#define SDCALSKY_DEBUG

using namespace casacore;

namespace {
struct NullLogger {};

template<class T>
inline NullLogger &operator<<(NullLogger &logger, T /*value*/) {
  return logger;
}

#ifndef SDCALSKY_DEBUG
NullLogger nulllogger;
#endif

struct CommaSeparatedThousands : std::numpunct<char> {
    char_type do_thousands_sep() const override { return ','; }
    string_type do_grouping() const override { return "\3"; }
};

}

#ifdef SDCALSKY_DEBUG
  #define debuglog std::cout
  #define debugpost std::endl
#else
  #define debuglog nulllogger
  #define debugpost 0
#endif

// Local Functions
namespace {
inline Vector<rownr_t> getOffStateIdList(MeasurementSet const &ms) {
  String taql("SELECT FLAG_ROW FROM $1 WHERE UPPER(OBS_MODE) ~ m/^OBSERVE_TARGET#OFF_SOURCE/");
  Table stateSel = tableCommand(taql, ms.state());
  Vector<rownr_t> stateIdList = stateSel.rowNumbers();
  debuglog << "stateIdList[" << stateIdList.nelements() << "]=";
  for (size_t i = 0; i < stateIdList.nelements(); ++i) {
    debuglog << stateIdList[i] << " ";
  }
  debuglog << debugpost;
  return stateIdList;
}

template<class T>
inline std::string toString(casacore::Vector<T> const &v) {
  std::ostringstream oss;
  oss << "[";
  std::string delimiter = "";
  for (size_t i = 0; i < v.nelements(); ++i) {
    oss << delimiter << v[i];
    delimiter = ",";
  }
  oss << "]";
  return oss.str();
}

// unused
/*
inline casacore::String configureTaqlString(casacore::String const &msName, casacore::Vector<casacore::uInt> stateIdList) {
  std::ostringstream oss;
  oss << "SELECT FROM " << msName << " WHERE ANTENNA1 == ANTENNA2 && STATE_ID IN "
      << toString(stateIdList)
      << " ORDER BY FIELD_ID, ANTENNA1, FEED1, DATA_DESC_ID, TIME";
  return casacore::String(oss);
}
*/

inline void fillNChanParList(casacore::MeasurementSet const &ms, casacore::Vector<casacore::Int> &nChanParList) {
  casacore::MSSpectralWindow const &msspw = ms.spectralWindow();
  casacore::ScalarColumn<casacore::Int> nchanCol(msspw, "NUM_CHAN");
  debuglog << "nchanCol=" << toString(nchanCol.getColumn()) << debugpost;
  nChanParList = nchanCol.getColumn()(casacore::Slice(0,nChanParList.nelements()));
  debuglog << "nChanParList=" << nChanParList << debugpost;
}

inline void updateWeight(casa::NewCalTable &ct) {
  casa::CTMainColumns ctmc(ct);
  casacore::ArrayColumn<casacore::Double> chanWidthCol(ct.spectralWindow(), "CHAN_WIDTH");
  casacore::Vector<casacore::Int> chanWidth(chanWidthCol.nrow());
  for (size_t irow = 0; irow < chanWidthCol.nrow(); ++irow) {
    casacore::Double chanWidthVal = chanWidthCol(irow)(casacore::IPosition(1,0));
    chanWidth[irow] = abs(chanWidthVal);
  }
  for (size_t irow = 0; irow < ct.nrow(); ++irow) {
    casacore::Int spwid = ctmc.spwId()(irow);
    casacore::Double width = chanWidth[spwid];
    casacore::Double interval = ctmc.interval()(irow);
    casacore::Float weight = width * interval;
    casacore::Matrix<casacore::Float> weightMat(ctmc.fparam().shape(irow), weight);
    ctmc.weight().put(irow, weightMat);
  }
}

class DataColumnAccessor {
public:
  DataColumnAccessor(casacore::Table const &ms,
		     casacore::String const colName="DATA")
    : dataCol_(ms, colName) {
  }
  casacore::Matrix<casacore::Float> operator()(size_t irow) {
    return casacore::real(dataCol_(irow));
  }
private:
  DataColumnAccessor() {}
  casacore::ArrayColumn<casacore::Complex> dataCol_;
};    

class FloatDataColumnAccessor {
public:
  FloatDataColumnAccessor(casacore::Table const &ms)
    : dataCol_(ms, "FLOAT_DATA") {
  }
  casacore::Matrix<casacore::Float> operator()(size_t irow) {
    return dataCol_(irow);
  }
private:
  FloatDataColumnAccessor() {}
  casacore::ArrayColumn<casacore::Float> dataCol_;
};

inline  casacore::Int nominalDataDesc(casacore::MeasurementSet const &ms, casacore::Int const ant)
{
  casacore::Int goodDataDesc = -1;
  casacore::ScalarColumn<casacore::Int> col(ms.spectralWindow(), "NUM_CHAN");
  casacore::Vector<casacore::Int> nchanList = col.getColumn();
  size_t numSpw = col.nrow();
  casacore::Vector<casacore::Int> spwMap(numSpw);
  col.attach(ms.dataDescription(), "SPECTRAL_WINDOW_ID");
  for (size_t i = 0; i < col.nrow(); ++i) {
    spwMap[col(i)] = i;
  }
  casacore::ScalarColumn<casacore::String> obsModeCol(ms.state(), "OBS_MODE");
  casacore::Regex regex("^OBSERVE_TARGET#ON_SOURCE.*");
  for (size_t ispw = 0; ispw < numSpw; ++ispw) {
    if (nchanList[ispw] == 4) {
      // this should be WVR
      continue;
    }
    else {
      std::ostringstream oss;
      oss << "SELECT FROM $1 WHERE ANTENNA1 == " << ant
	  << " && ANTENNA2 == " << ant << " && DATA_DESC_ID == " << spwMap[ispw]
	  << " ORDER BY STATE_ID";
      casacore::MeasurementSet msSel(casacore::tableCommand(oss.str(), ms));
      col.attach(msSel, "STATE_ID");
      casacore::Vector<casacore::Int> stateIdList = col.getColumn();
      casacore::Int previousStateId = -1;
      for (size_t i = 0; i < msSel.nrow(); ++i) {
	casacore::Int stateId = stateIdList[i];
	if (stateId != previousStateId) {
	  casacore::String obsmode = obsModeCol(stateId);
	  if (regex.match(obsmode.c_str(), obsmode.size()) != casacore::String::npos) {
	    goodDataDesc = spwMap[ispw];
	    break;
	  }
	}
	previousStateId = stateId;
      }
    }

    if (goodDataDesc >= 0)
      break;
  }
  return goodDataDesc;
}

inline casacore::Vector<size_t> detectGap(casacore::Vector<casacore::Double> timeList)
{
  size_t n = timeList.size();
  casacore::Vector<casacore::Double> timeInterval = timeList(casacore::Slice(1, n-1)) - timeList(casacore::Slice(0, n-1));
  casacore::Double medianTime = casacore::median(timeInterval);
  casacore::Double const threshold = medianTime * 5.0;
  casacore::Vector<size_t> gapIndexList(casacore::IPosition(1, n/2 + 2), new size_t[n/2+2], casacore::TAKE_OVER);
  gapIndexList[0] = 0;
  size_t gapIndexCount = 1;
  for (size_t i = 0; i < timeInterval.size(); ++i) {
    if (timeInterval[i] > threshold) {
      gapIndexList[gapIndexCount] = i + 1;
      gapIndexCount++;
    }
  }
  if (gapIndexList[gapIndexCount] != n) {
    gapIndexList[gapIndexCount] = n;
    gapIndexCount++;
  }
  debuglog << "Detected " << gapIndexCount << " gaps." << debugpost;
  casacore::Vector<size_t> ret(casacore::IPosition(1, gapIndexCount), gapIndexList.data(), casacore::COPY);
  debuglog << "gapList=" << toString(ret) << debugpost;
  return ret;
}

struct DefaultRasterEdgeDetector
{
  static size_t N(size_t numData, casacore::Float const /*fraction*/, casacore::Int const /*num*/)
  {
    debuglog << "DefaultRasterEdgeDetector" << debugpost;
    return max((size_t)1, static_cast<size_t>(sqrt(numData + 1) - 1));
  }
};

struct FixedNumberRasterEdgeDetector
{
  static size_t N(size_t /*numData*/, casacore::Float const /*fraction*/, casacore::Int const num)
  {
    debuglog << "FixedNumberRasterEdgeDetector" << debugpost;
    if (num < 0) {
      throw casacore::AipsError("Negative number of edge points.");
    }
    return (size_t)num;
  }
};

struct FixedFractionRasterEdgeDetector
{
  static casacore::Int N(size_t numData, casacore::Float const fraction, casacore::Int const /*num*/)
  {
    debuglog << "FixedFractionRasterEdgeDetector" << debugpost;
    return max((size_t)1, static_cast<size_t>(numData * fraction));
  }
};

template<class Detector>
inline casacore::Vector<casacore::Double> detectEdge(casacore::Vector<casacore::Double> timeList, casacore::Double const interval, casacore::Float const fraction, casacore::Int const num)
{
  // storage for time range for edges (at head and tail)
  // [(start of head edge), (end of head edge),
  //  (start of tail edge), (end of tail edge)]
  casacore::Vector<casacore::Double> edgeList(4);
  size_t numList = timeList.size();
  size_t numEdge = Detector::N(numList, fraction, num);
  debuglog << "numEdge = " << numEdge << debugpost;
  if (numEdge == 0) {
    throw casacore::AipsError("Zero edge points.");
  }
  else if (timeList.size() > numEdge * 2) {
    edgeList[0] = timeList[0] - 0.5 * interval;
    edgeList[1] = timeList[numEdge-1] + 0.5 * interval;
    edgeList[2] = timeList[numList-numEdge] - 0.5 * interval;
    edgeList[3] = timeList[numList-1] + 0.5 * interval;
  }
  else {
    std::ostringstream oss;
    oss << "Too many edge points (" << 2.0 * numEdge << " out of "
        << timeList.size() << " points)";
    throw casacore::AipsError(oss.str());
    // edgeList[0] = timeList[0] - 0.5 * interval;
    // edgeList[1] = timeList[numList-1] + 0.5 * interval;
    // edgeList[2] = edgeList[0];
    // edgeList[3] = edgeList[2];
  }
  return edgeList;
}
  
inline casacore::Vector<casacore::String> detectRaster(casacore::MeasurementSet const &ms,
					       casacore::Int const ant,
					       casacore::Float const fraction,
					       casacore::Int const num)
{
  casacore::Int dataDesc = nominalDataDesc(ms, ant);
  debuglog << "nominal DATA_DESC_ID=" << dataDesc << debugpost;
  assert(dataDesc >= 0);
  if (dataDesc < 0) {
    return casacore::Vector<casacore::String>();
  }

  std::ostringstream oss;
  oss << "SELECT FROM $1 WHERE ANTENNA1 == " << ant
      << " && ANTENNA2 == " << ant << " && FEED1 == 0 && FEED2 == 0"
      << " && DATA_DESC_ID == " << dataDesc
      << " ORDER BY TIME";
  debuglog << "detectRaster: taql=" << oss.str() << debugpost;
  casacore::MeasurementSet msSel(casacore::tableCommand(oss.str(), ms));
  casacore::ScalarColumn<casacore::Double> timeCol(msSel, "TIME");
  casacore::ScalarColumn<casacore::Double> intervalCol(msSel, "INTERVAL");
  casacore::Vector<casacore::Double> timeList = timeCol.getColumn();
  casacore::Double interval = casacore::min(intervalCol.getColumn());
  casacore::Vector<size_t> gapList = detectGap(timeList);
  casacore::Vector<casacore::String> edgeAsTimeRange(gapList.size() * 2);
  typedef casacore::Vector<casacore::Double> (*DetectorFunc)(casacore::Vector<casacore::Double>, casacore::Double const,  casacore::Float const, casacore::Int const);
  DetectorFunc detect = NULL;
  if (num > 0) {
    detect = detectEdge<FixedNumberRasterEdgeDetector>;
  }
  else if (fraction > 0.0) {
    detect = detectEdge<FixedFractionRasterEdgeDetector>;
  }
  else {
    detect = detectEdge<DefaultRasterEdgeDetector>;
  }
  for (size_t i = 0; i < gapList.size()-1; ++i) {
    size_t startRow = gapList[i];
    size_t endRow = gapList[i+1];
    size_t len = endRow - startRow;
    debuglog << "startRow=" << startRow << ", endRow=" << endRow << debugpost;
    casacore::Vector<casacore::Double> oneRow = timeList(casacore::Slice(startRow, len));
    casacore::Vector<casacore::Double> edgeList = detect(oneRow, interval, fraction, num);
    std::ostringstream s;
    s << std::setprecision(16) << "TIME BETWEEN " << edgeList[0] << " AND " << edgeList[1];
    edgeAsTimeRange[2*i] = s.str();
    s.str("");
    s << std::setprecision(16) << "TIME BETWEEN " << edgeList[2] << " AND " << edgeList[3];
    edgeAsTimeRange[2*i+1] = s.str();
    debuglog << "Resulting selection: (" << edgeAsTimeRange[2*i] << ") || ("
	     << edgeAsTimeRange[2*i+1] << ")" << debugpost;
  }
  return edgeAsTimeRange;
}

// Formula for weight scaling factor, WF
// 1. only one OFF spectrum is used (i.e. no interpolation)
//
//     sigma = sqrt(sigma_ON^2 + sigma_OFF^2)
//           = sigma_ON * sqrt(1 + tau_ON / tau_OFF)
//
//     weight = 1 / sigma^2
//            = 1 / sigma_ON^2 * tau_OFF / (tau_ON + tau_OFF)
//            = weight_ON * tau_OFF / (tau_ON + tau_OFF)
//
//     WF = tau_OFF / (tau_ON + tau_OFF)
//
struct SimpleWeightScalingScheme
{
  static casacore::Float SimpleScale(casacore::Double exposureOn, casacore::Double exposureOff)
  {
    return exposureOff / (exposureOn + exposureOff);
  }
};
// 2. two OFF spectrum is used (linear interpolation)
//
//     sigma_OFF = {(t_OFF2 - t_ON)^2 * sigma_OFF1^2
//                    + (t_ON - t_OFF1)^2 * sigma_OFF2^2}
//                  / (t_OFF2 - t_OFF1)^2
//
//     sigma = sqrt(sigma_ON^2 + sigma_OFF^2)
//           = sigma_ON * sqrt(1 + tau_ON / (t_OFF2 - t_OFF1)^2
//                              * {(t_OFF2 - t_ON)^2 / tau_OFF1
//                                  + (t_ON - t_OFF1)^2 / tau_OFF2})
//
//     weight = weight_ON / (1 + tau_ON / (t_OFF2 - t_OFF1)^2
//                            * {(t_OFF2 - t_ON)^2 / tau_OFF1
//                                + (t_ON - t_OFF1)^2 / tau_OFF2})
//
//     WF = 1.0 / (1 + tau_ON / (t_OFF2 - t_OFF1)^2
//                  * {(t_OFF2 - t_ON)^2 / tau_OFF1
//                      + (t_ON - t_OFF1)^2 / tau_OFF2})
//
struct LinearWeightScalingScheme : public SimpleWeightScalingScheme
{
  static casacore::Float InterpolateScale(casacore::Double timeOn, casacore::Double exposureOn,
                                     casacore::Double timeOff1, casacore::Double exposureOff1,
                                     casacore::Double timeOff2, casacore::Double exposureOff2)
  {
    casacore::Double dt = timeOff2 - timeOff1;
    casacore::Double dt1 = timeOn - timeOff1;
    casacore::Double dt2 = timeOff2 - timeOn;
    return 1.0f / (1.0f + exposureOn / (dt * dt)
                   * (dt2 * dt2 / exposureOff1 + dt1 * dt1 / exposureOff2));
  }
};

// 3. two OFF spectrum is used (nearest interpolation)
//
// formulation is same as case 1.
//
struct NearestWeightScalingScheme : public SimpleWeightScalingScheme
{
  static casacore::Float InterpolateScale(casacore::Double timeOn, casacore::Double exposureOn,
                                     casacore::Double timeOff1, casacore::Double exposureOff1,
                                     casacore::Double timeOff2, casacore::Double exposureOff2)
  {
    casacore::Double dt1 = abs(timeOn - timeOff1);
    casacore::Double dt2 = abs(timeOff2 - timeOn);
    return (dt1 <= dt2) ?
      SimpleScale(exposureOn, exposureOff1)
      : SimpleScale(exposureOn, exposureOff2);
  }
};

inline bool isEphemeris(casacore::String const &name) {
  // Check if given name is included in MDirection types
  casacore::Int nall, nextra;
  const casacore::uInt *typ;
  auto *types = casacore::MDirection::allMyTypes(nall, nextra, typ);
  auto start_extra = nall - nextra;
  auto capital_name = name;
  capital_name.upcase();

  for (auto i = start_extra; i < nall; ++i) {
    if (capital_name == types[i]) {
      return true;
    }
  }

  return false;
}

// Set number of correlations per spw
inline void setNumberOfCorrelationsPerSpw(casacore::MeasurementSet const &ms,
    casacore::Vector<casacore::Int> &nCorr) {
  casacore::ScalarColumn<casacore::Int> spwIdCol(ms.dataDescription(), "SPECTRAL_WINDOW_ID");
  casacore::ScalarColumn<casacore::Int> polIdCol(ms.dataDescription(), "POLARIZATION_ID");
  casacore::ScalarColumn<casacore::Int> numCorrCol(ms.polarization(), "NUM_CORR");
  auto numSpw = ms.spectralWindow().nrow();
  auto const spwIdList = spwIdCol.getColumn();
  auto const polIdList = polIdCol.getColumn();
  auto const numCorrList = numCorrCol.getColumn();
  if (nCorr.size() != numSpw) {
    nCorr.resize(numSpw);
  }
  // initialize number of correlations with 0
  nCorr = 0;
  for (auto i = 0u; i < spwIdList.size(); ++i) {
    auto const spwId = spwIdList[i];
    auto const polId = polIdList[i];
    auto const numCorr = numCorrList[polId];
    nCorr[spwId] = numCorr;
  }
}
}

namespace casa { //# NAMESPACE CASA - BEGIN
//
// SingleDishSkyCal
//
  
// Constructor
SingleDishSkyCal::SingleDishSkyCal(VisSet& vs)
  : VisCal(vs),
    SolvableVisCal(vs),
    currAnt_(-1),
    engineC_(vs.numberSpw(), NULL),
    engineF_(vs.numberSpw(), NULL),
    currentSky_(vs.numberSpw(), NULL),
    currentSkyOK_(vs.numberSpw(), NULL),
    nCorr_(nSpw())
{
  debuglog << "SingleDishSkyCal::SingleDishSkyCal(VisSet& vs)" << debugpost;
  append() = false;

  initializeSky();
  initializeCorr();
}

SingleDishSkyCal::SingleDishSkyCal(const MSMetaInfoForCal& msmc)
  : VisCal(msmc),
    SolvableVisCal(msmc),
    currAnt_(-1),
    engineC_(msmc.nSpw(), NULL),
    engineF_(msmc.nSpw(), NULL),
    currentSky_(msmc.nSpw(), NULL),
    currentSkyOK_(msmc.nSpw(), NULL),
    nCorr_(nSpw())
{
  debuglog << "SingleDishSkyCal::SingleDishSkyCal(const MSMetaInfoForCal& msmc)" << debugpost;
  append() = False;

  initializeSky();
  initializeCorr();
}

SingleDishSkyCal::SingleDishSkyCal(const Int& nAnt)
  : VisCal(nAnt),
    SolvableVisCal(nAnt),
    currAnt_(-1),
    engineC_(1, NULL),
    engineF_(1, NULL),
    currentSky_(1, NULL),
    currentSkyOK_(1, NULL),
    nCorr_(nSpw())
{
  debuglog << "SingleDishSkyCal::SingleDishSkyCal(const Int& nAnt)" << debugpost;
  append() = false;

  initializeSky();
  initializeCorr();
}

// Destructor
SingleDishSkyCal::~SingleDishSkyCal()
{
  debuglog << "SingleDishSkyCal::~SingleDishSkyCal()" << debugpost;

  finalizeSky();
}

void SingleDishSkyCal::guessPar(VisBuffer& /*vb*/)
{
}

void SingleDishSkyCal::differentiate( CalVisBuffer & /*cvb*/)
{
}

void SingleDishSkyCal::differentiate(VisBuffer& /*vb*/, Cube<Complex>& /*V*/,     
                                     Array<Complex>& /*dV*/, Matrix<Bool>& /*Vflg*/)
{
}

void SingleDishSkyCal::accumulate(SolvableVisCal* /*incr*/,
                                  const Vector<Int>& /*fields*/)
{
}

void SingleDishSkyCal::diffSrc(VisBuffer& /*vb*/, Array<Complex>& /*dV*/)
{
}

void SingleDishSkyCal::fluxscale(const String& /*outfile*/,
                                 const Vector<Int>& /*refFieldIn*/,
                                 const Vector<Int>& /*tranFieldIn*/,
                                 const Vector<Int>& /*inRefSpwMap*/,
                                 const Vector<String>& /*fldNames*/,
                                 const Float& /*inGainThres*/,
                                 const String& /*antSel*/,
                                 const String& /*timerangeSel*/,
                                 const String& /*scanSel*/,
                                 fluxScaleStruct& /*oFluxScaleStruct*/,
                                 const String& /*oListFile*/,
                                 const Bool& /*incremental*/,
                                 const Int& /*fitorder*/,
                                 const Bool& /*display*/)
{
}

void SingleDishSkyCal::listCal(const Vector<Int> /*ufldids*/, const Vector<Int> /*uantids*/,
                               const Matrix<Int> /*uchanids*/,
                               //const Int& /*spw*/, const Int& /*chan*/,
                               const String& /*listfile*/, const Int& /*pagerows*/)
{
}

void SingleDishSkyCal::setApply(const Record& apply)
{
  // Override interp
  // default frequency interpolation option is 'linearflag'
  Record applyCopy(apply);
  if (applyCopy.isDefined("interp")) {
    String interp = applyCopy.asString("interp");
    if (!interp.contains(',')) {
      //fInterpType() = "linearflag";
      String newInterp = interp + ",linearflag";
      applyCopy.define("interp", newInterp);
    }
  }
  else {
    applyCopy.define("interp", "linear,linearflag");
  }
  
  // call parent method
  SolvableVisCal::setApply(applyCopy);
}

void SingleDishSkyCal::fillCalibrationTable(casacore::MeasurementSet const &reference_data){
    String dataColName = (reference_data.tableDesc().isColumn("FLOAT_DATA")) ? "FLOAT_DATA" : "DATA";

    if ( dataColName == "FLOAT_DATA")
        fillCalibrationTable<FloatDataColumnAccessor>(reference_data);
    else
        fillCalibrationTable<DataColumnAccessor>(reference_data);
}

template<class DataRealComponentAccessor>
void SingleDishSkyCal::fillCalibrationTable(MeasurementSet const &ms) {
  Int cols[] = {MS::FIELD_ID, MS::ANTENNA1, MS::FEED1,
		MS::DATA_DESC_ID};
  Int *colsp = cols;
  Block<Int> sortCols(4, colsp, false);
  MSIter msIter(ms, sortCols, 0.0, false, false);
  for (msIter.origin(); msIter.more(); msIter++) {
    Table const current = msIter.table();
    uInt nrow = current.nrow();
    debuglog << "nrow = " << nrow << debugpost;
    if (nrow == 0) {
      debuglog << "Skip" << debugpost;
      continue;
    }
    Int ispw = msIter.spectralWindowId();
    if (nChanParList()[ispw] == 4) {
      // Skip WVR
      debuglog << "Skip " << ispw
	       << "(nchan " << nChanParList()[ispw] << ")"
	       << debugpost;
      continue;
    }
    debuglog << "Process " << ispw
	     << "(nchan " << nChanParList()[ispw] << ")"
	     << debugpost;
    
    Int ifield = msIter.fieldId();
    ScalarColumn<Int> antennaCol(current, "ANTENNA1");
    currAnt_ = antennaCol(0);
    ScalarColumn<Int> feedCol(current, "FEED1");
    debuglog << "FIELD_ID " << msIter.fieldId()
	     << " ANTENNA1 " << currAnt_
	     << " FEED1 " << feedCol(0)
	     << " DATA_DESC_ID " << msIter.dataDescriptionId()
	     << debugpost;
    ScalarColumn<Double> timeCol(current, "TIME");
    ScalarColumn<Double> exposureCol(current, "EXPOSURE");
    ScalarColumn<Double> intervalCol(current, "INTERVAL");
    DataRealComponentAccessor dataCol(current);
    ArrayColumn<Bool> flagCol(current, "FLAG");
    ScalarColumn<Bool> flagRowCol(current, "FLAG_ROW");
    Vector<Double> timeList = timeCol.getColumn();
    Vector<Double> exposure = exposureCol.getColumn();
    Vector<Double> interval = intervalCol.getColumn();
    Vector<Double> timeInterval(timeList.nelements());
    Slice slice1(0, nrow - 1);
    Slice slice2(1, nrow - 1);
    timeInterval(slice1) = timeList(slice2) - timeList(slice1);
    timeInterval[nrow-1] = DBL_MAX;
    IPosition cellShape = flagCol.shape(0);
    size_t nelem = cellShape.product();
    Matrix<Float> dataSum(cellShape, new Float[nelem], TAKE_OVER);
    Matrix<Float> weightSum(cellShape, new Float[nelem], TAKE_OVER);
    dataSum = 0.0f;
    weightSum = 0.0f;
    Matrix<Bool> resultMask(cellShape, new Bool[nelem], TAKE_OVER);
    resultMask = true;
    Vector<Bool> flagRow = flagRowCol.getColumn();
    Double threshold = 1.1;
    Double timeCentroid = 0.0;
    size_t numSpectra = 0;
    Double effectiveExposure = 0.0;
    for (uInt i = 0; i < nrow; ++i) {
      if (flagRow(i)) {
	continue;
      }

      numSpectra++;
      timeCentroid += timeList[i];
      effectiveExposure += exposure[i];
      
      Matrix<Bool> mask = !flagCol(i);
      MaskedArray<Float> mdata(dataCol(i), mask);
      MaskedArray<Float> weight(Matrix<Float>(mdata.shape(), exposure[i]), mask);
      dataSum += mdata * weight;
      weightSum += weight;

      Double gap = 2.0 * timeInterval[i] /
	(interval[i] + interval[(i < nrow-1)?i+1:i]);
      if (gap > threshold) {
        debuglog << "flush accumulated data at row " << i << debugpost;
	// Here we can safely use data() since internal storeage
	// is contiguous
	Float *data_ = dataSum.data();
	const Float *weight_ = weightSum.data();
	Bool *flag_ = resultMask.data();
	for (size_t j = 0; j < dataSum.nelements(); ++j) {
	  if (weight_[j] == 0.0f) {
	    data_[j] = 0.0;
	    flag_[j] = false;
	  }
	  else {
	    data_[j] /= weight_[j];
	  }
	}

	currSpw() = ispw;
	currField() = ifield;
	timeCentroid /= (Double)numSpectra;
	refTime() = timeCentroid;
	interval_ = effectiveExposure;

	debuglog << "spw " << ispw << ": solveAllRPar.shape=" << solveAllRPar().shape() << " nPar=" << nPar() << " nChanPar=" << nChanPar() << " nElem=" << nElem() << debugpost;
	
  size_t const nCorr = dataSum.shape()[0];
	Cube<Float> const rpar = dataSum.addDegenerate(1);
	Cube<Bool> const parOK = resultMask.addDegenerate(1);
	for (size_t iCorr = 0; iCorr < nCorr; ++iCorr) {
	  solveAllRPar().yzPlane(iCorr) = rpar.yzPlane(iCorr);
	  solveAllParOK().yzPlane(iCorr) = parOK.yzPlane(iCorr);
	  solveAllParErr().yzPlane(iCorr) = 0.1; // TODO: this is tentative
	  solveAllParSNR().yzPlane(iCorr) = 1.0; // TODO: this is tentative
	}

	keepNCT();

	dataSum = 0.0f;
	weightSum = 0.0f;
	resultMask = true;
	timeCentroid = 0.0;
	numSpectra = 0;
	effectiveExposure = 0.0;
      }
    }
  }
}

void SingleDishSkyCal::keepNCT() {
  // Call parent to do general stuff
  //  This adds nElem() rows
  SolvableVisCal::keepNCT();

  // We are adding to the most-recently added rows
  RefRows rows(ct_->nrow()-nElem(),ct_->nrow()-1,1);
  Vector<Int> ant(nElem(), currAnt_);

  // update ANTENNA1 and ANTENNA2 with appropriate value
  CTMainColumns ncmc(*ct_);
  ncmc.antenna1().putColumnCells(rows,ant);
  ncmc.antenna2().putColumnCells(rows,ant);

  // update INTERVAL
  ncmc.interval().putColumnCells(rows,interval_);
}    

void SingleDishSkyCal::initSolvePar()
{
  debuglog << "SingleDishSkyCal::initSolvePar()" << debugpost;
  for (Int ispw=0;ispw<nSpw();++ispw) {
    
    currSpw()=ispw;

    switch(parType()) {
    case VisCalEnum::REAL: {
      solveAllRPar().resize(nPar(),nChanPar(),nElem());
      solveAllRPar()=0.0;
      solveRPar().reference(solveAllRPar());
      break;
    }
    default: {
      throw(AipsError("Internal error(Calibrater Module): Unsupported parameter type "
		      "found in SingleDishSkyCal::initSolvePar()"));
      break;
    }
    }//switch

    solveAllParOK().resize(solveAllRPar().shape());
    solveAllParErr().resize(solveAllRPar().shape());
    solveAllParSNR().resize(solveAllRPar().shape());
    solveAllParOK()=false;
    solveAllParErr()=0.0;
    solveAllParSNR()=0.0;
    solveParOK().reference(solveAllParOK());
    solveParErr().reference(solveAllParErr());
    solveParSNR().reference(solveAllParSNR());
  }
  currSpw()=0;
  currAnt_ = 0;

  interval_.resize(nElem());
}

void SingleDishSkyCal::syncMeta2(const vi::VisBuffer2& vb)
{
  // call method in parent class
  VisCal::syncMeta2(vb);

  // fill interval array with exposure
  interval_.reference(vb.exposure());
  debuglog << "SingleDishSkyCal::syncMeta2 interval_= " << interval_ << debugpost;

  setNumberOfCorrelationsPerSpw(vb.ms(), nCorr_);
  debuglog << "nCorr_ = " << nCorr_ << debugpost;
  debuglog << "currSpw() = " << currSpw() << debugpost;
  debuglog << "nPar() = " << nPar() << debugpost;
}

void SingleDishSkyCal::syncCalMat(const Bool &/*doInv*/)
{
  debuglog << "SingleDishSkyCal::syncCalMat" << debugpost;
  debuglog << "nAnt()=" << nAnt() << ", nElem()=" << nElem() << ", nBln()=" << nBln() << debugpost;
  debuglog << "Spw " << currSpw() << "nchanmat, ncalmat" << nChanMat() << ", " << nCalMat() << debugpost;
  debuglog << "nChanPar = " << nChanPar() << debugpost;
  currentSky().resize(2, nChanMat(), nCalMat());
  currentSky().unique();
  currentSkyOK().resize(currentSky().shape());
  currentSkyOK().unique();
  debuglog << "currentSkyOK.shape()=" << currentSkyOK().shape() << debugpost;

  // sky data from caltable
  debuglog << "currRPar().shape()=" << currRPar().shape() << debugpost;
  if (currRPar().shape().product() > 0) {
    debuglog << "currRPar() = " << currRPar().xzPlane(0) << debugpost;
  }

  convertArray(currentSky(), currRPar());
  currentSkyOK() = currParOK();
  debuglog << "currentTime() = " << setprecision(16) << currTime() << debugpost;
  debuglog << "currentSky() = " << currentSky().xzPlane(0) << debugpost;
  debuglog << "currentSkyOK() = " << currentSkyOK().xzPlane(0) << debugpost;

  // weight calibration
  if (calWt())
    syncWtScale();

  debuglog << "SingleDishSkyCal::syncCalMat DONE" << debugpost;
}
  
void SingleDishSkyCal::syncDiffMat()
{
  debuglog << "SingleDishSkyCal::syncDiffMat()" << debugpost;
}

void SingleDishSkyCal::syncWtScale()
{
  debuglog << "syncWtScale" << debugpost;
  
  // allocate necessary memory 
  currWtScale().resize(currentSky().shape());
  currWtScale() = 1.0;

  // Calculate the weight scaling
  if (tInterpType() == "nearest") {
    calcWtScale<NearestWeightScalingScheme>();
  }
  else {
    calcWtScale<LinearWeightScalingScheme>();
  }
  
  debuglog << "syncWtScale DONE" << debugpost;
}

template<class ScalingScheme>
void SingleDishSkyCal::calcWtScale()
{
  debuglog << "calcWtScale<ScalingScheme>" << debugpost;
  auto const key = std::make_pair(currObs(), currSpw());
  debuglog << "for obs " << currObs() << " and spw " << currSpw() << debugpost;
  decltype(wtScaleData_)::iterator iter = wtScaleData_.find(key);
  map<Int, Matrix<Double> > wtMap;
  if (iter == wtScaleData_.end()) {
    debuglog << "construct weight scaling data for obs " << currObs() << " spw " << currSpw() << debugpost;
    debuglog << "number of antennas = " << nAnt() << debugpost;
    for (Int iAnt = 0; iAnt < nAnt(); ++iAnt) {
      CTInterface cti(*ct_);
      MSSelection mss;
      mss.setObservationExpr(String::toString(currObs()));
      mss.setSpwExpr(String::toString(currSpw()));
      mss.setAntennaExpr(String::toString(iAnt) + "&&&");
      TableExprNode ten = mss.toTableExprNode(&cti);
      NewCalTable temp;
      try {
        getSelectedTable(temp, *ct_, ten, "");
      } catch (AipsError x) {
        continue;
      }
      temp = temp.sort("TIME");
      wtMap.emplace(std::piecewise_construct,
          std::forward_as_tuple(iAnt),
          std::forward_as_tuple(2, temp.nrow()));
      Matrix<Double> &arr = wtMap.at(iAnt);
      debuglog << "ant " << iAnt << " arr.shape = " << arr.shape() << debugpost;
      ScalarColumn<Double> col(temp, "TIME");
      auto timeCol = arr.row(0);
      col.getColumn(timeCol, False);
      col.attach(temp, "INTERVAL");
      auto intervalCol = arr.row(1);
      debuglog << "timeCol.size() == " << timeCol.size() << " intervalCol.size() = " << intervalCol.size() << debugpost;
      col.getColumn(intervalCol, False);
    }
    wtScaleData_.insert(std::make_pair(key, wtMap));
  } else {
    wtMap = iter->second;
  }

  {
    // for debugging
    debuglog << "wtMap keys: ";
    for (decltype(wtMap)::iterator i = wtMap.begin(); i != wtMap.end(); ++i) {
      debuglog << i->first << " ";
    }
    debuglog << debugpost;
  }

  for (Int iAnt = 0; iAnt < nAnt(); ++iAnt) {
    decltype(wtMap)::iterator i = wtMap.find(iAnt);
    if (i == wtMap.end()) {
      continue;
    }
    auto const &mat = i->second;
    debuglog << "matrix shape " << mat.shape() << debugpost;
    Vector<Double> const &timeCol = mat.row(0);
    Vector<Double> const &intervalCol = mat.row(1);
    size_t nrow = timeCol.size();
    debuglog << "timeCol = " << timeCol << debugpost;
    debuglog << "intervalCol = " << intervalCol << debugpost;
    debuglog << "iAnt " << iAnt << " nrow=" << nrow << debugpost;
    if (currTime() < timeCol[0]) {
      debuglog << "Use nearest OFF weight (0)" << debugpost;
      currWtScale().xyPlane(iAnt) = ScalingScheme::SimpleScale(interval_[iAnt], intervalCol[0]);
    }
    else if (currTime() > timeCol[nrow-1]) {
      debuglog << "Use nearest OFF weight (" << nrow-1 << ")" << debugpost;
      currWtScale().xyPlane(iAnt) = ScalingScheme::SimpleScale(interval_[iAnt], intervalCol[nrow-1]);
    }
    else {
      debuglog << "Use interpolated OFF weight" << debugpost;
      for (size_t irow = 0; irow < nrow ; ++irow) {
        if (currTime() == timeCol[irow]) {
          currWtScale().xyPlane(iAnt) = ScalingScheme::SimpleScale(interval_[iAnt], intervalCol[irow]);
          break;
        }
        else if (currTime() < timeCol[irow]) {
          currWtScale().xyPlane(iAnt) = ScalingScheme::InterpolateScale(currTime(), interval_[iAnt],
                                                                       timeCol[irow-1], intervalCol[irow-1],
                                                                       timeCol[irow], intervalCol[irow]);
          break;
        }
      }
    }
  }
  debuglog << "currWtScale() = " << currWtScale().xzPlane(0) << debugpost;

  debuglog << "calcWtScale<ScalingScheme> DONE" << debugpost;
}
  
Float SingleDishSkyCal::calcPowerNorm(Array<Float>& /*amp*/, const Array<Bool>& /*ok*/)
{
  return 0.0f;
}

void SingleDishSkyCal::applyCal(VisBuffer& /*vb*/, Cube<Complex>& /*Vout*/, Bool /*trial*/)
{
  throw AipsError("Single dish calibration doesn't support applyCal. Please use applyCal2");
}

void SingleDishSkyCal::applyCal2(vi::VisBuffer2 &vb, Cube<Complex> &Vout, Cube<Float> &Wout,
                                 Bool trial)
{
  debuglog << "SingleDishSkyCal::applycal2" << debugpost;
  debuglog << "nrow, nchan=" << vb.nRows() << "," << vb.nChannels() << debugpost;
  debuglog << "antenna1: " << vb.antenna1() << debugpost;
  debuglog << "antenna2: " << vb.antenna2() << debugpost;
  debuglog << "spw: " << vb.spectralWindows() << debugpost;
  debuglog << "field: " << vb.fieldId() << debugpost;

  // References to VB2's contents' _data_
  Vector<Bool> flagRv(vb.flagRow());
  Vector<Int>  a1v(vb.antenna1());
  Vector<Int>  a2v(vb.antenna2());
  Cube<Bool> flagCube(vb.flagCube());
  Cube<Complex> visCube(Vout);
  ArrayIterator<Float> wt(Wout,2);
  Matrix<Float> wtmat;

  // Data info/indices
  Int* dataChan;
  Bool* flagR=&flagRv(0);
  Int* a1=&a1v(0);
  Int* a2=&a2v(0);
  
  // iterate rows
  Int nRow=vb.nRows();
  Int nChanDat=vb.nChannels();
  Vector<Int> dataChanv(vb.getChannelNumbers(0));  // All rows have same chans
  //    cout << currSpw() << " startChan() = " << startChan() << " nChanMat() = " << nChanMat() << " nChanDat="<<nChanDat <<endl;

  // setup calibration engine
  engineC().setNumChannel(nChanDat);
  engineC().setNumPolarization(vb.nCorrelations());

  debuglog << "typesize=" << engineC().typesize() << debugpost;

  // Matrix slice of visCube
  // TODO: storage must be aligned for future use
  Matrix<Complex> visCubeSlice;
  Matrix<Bool> flagCubeSlice;
  
  for (Int row=0; row<nRow; row++,flagR++,a1++,a2++) {
    debuglog << "spw: " << currSpw() << " antenna: " << *a1 << debugpost;
    assert(*a1 == *a2);
    
    // Solution channel registration
    Int solCh0(0);
    dataChan=&dataChanv(0);
      
    // If cal _parameters_ are not freqDep (e.g., a delay)
    //  the startChan() should be the same as the first data channel
    if (freqDepMat() && !freqDepPar())
      startChan()=(*dataChan);

    // Solution and data array registration
    engineC().sync(currentSky()(0,solCh0,*a1), currentSkyOK()(0,solCh0,*a1));
    visCubeSlice.reference(visCube.xyPlane(row));
    flagCubeSlice.reference(flagCube.xyPlane(row));

    if (trial) {
      // only update flag info
      engineC().flag(flagCubeSlice);
    }
    else {
      // apply calibration
      engineC().apply(visCubeSlice, flagCubeSlice);
    }
    
    // If requested, update the weights
    if (!trial && calWt()) {
      wtmat.reference(wt.array());
      updateWt2(wtmat,*a1);
    }
    
    if (!trial)
      wt.next();
    
  }
}

void SingleDishSkyCal::selfGatherAndSolve(VisSet& vs, VisEquation& /*ve*/)
{
    debuglog << "SingleDishSkyCal::selfGatherAndSolve()" << debugpost;

    MeasurementSet const &user_selection = vs.iter().ms();

    debuglog << "nspw = " << nSpw() << debugpost;
    // Get and store the number of channels per spectral window
    fillNChanParList(MeasurementSet(msName()), nChanParList());

    // Get and store the number of correlations per spectral window
    setNumberOfCorrelationsPerSpw(user_selection, nCorr_);
    debuglog << "nCorr_ = " << nCorr_ << debugpost;

    // Create a new in-memory calibration table to fill up
    createMemCalTable();

    // Setup shape of solveAllRPar
    nElem() = 1;
    initSolvePar();

    // Select from user selection reference data associated with science target
    MeasurementSet reference_data = selectReferenceData(user_selection);
    logSink().origin(LogOrigin("SingleDishSkyCal","selfGatherAndSolve"));
    {
        // Log row numbers in a readable way
        std::ostringstream msg;
        auto * us_num_fmt = new CommaSeparatedThousands();
        // It looks like a locale constructed this way takes ownership
        // of the std::Facet * pointer it is passed, and *deletes* it when destructed.
        // So we must NOT delete us_num_fmt.
        std::locale us_like_locale(std::locale::classic(), us_num_fmt);
        msg.imbue(us_like_locale);
        msg << "Selected: " << std::right << std::setw(10) << reference_data.nrow() << " rows of reference data" << '\n'
            << "out of  : " << std::right << std::setw(10) << user_selection.nrow() << " rows of user-selected data";
        logSink() << msg.str() << LogIO::POST;
    }
    logSink().origin(LogOrigin());
    if (reference_data.nrow() == 0)
        throw AipsError("No reference integration found in user-selected data. Please double-check your data selection criteria.");

    // Fill observing-mode-dependent columns of the calibration table
    // Implementation is observing-mode-specific
    fillCalibrationTable(reference_data);

    // Fill remaining columns of the calibration table,
    // which are computed the same way for all observing modes
    // ---- 1) FIELD_ID, SCAN_NUMBER, OBSERVATION_ID columns
    assignCTScanField(*ct_, msName());

    // ---- 2) WEIGHT column
    // update weight without Tsys
    // formula is chanWidth [Hz] * interval [sec]
    updateWeight(*ct_);

    // Store calibration table on disk
    storeNCT();
}

void SingleDishSkyCal::initializeSky()
{
  debuglog << "SingleDishSkyCal::initializeSky()" << debugpost;
  for (Int ispw=0;ispw<nSpw(); ispw++) {
    currentSky_[ispw] = new Cube<Complex>();
    currentSkyOK_[ispw] = new Cube<Bool>();
    engineC_[ispw] = new SkyCal<Complex, Complex>();
  }
}

void SingleDishSkyCal::finalizeSky()
{
  for (Int ispw=0;ispw<nSpw(); ispw++) {
    if (currentSky_[ispw]) delete currentSky_[ispw];
    if (currentSkyOK_[ispw]) delete currentSkyOK_[ispw];
    if (engineC_[ispw]) delete engineC_[ispw];
    if (engineF_[ispw]) delete engineF_[ispw];
  }

}

void SingleDishSkyCal::initializeCorr()
{
  File msPath(msName());
  if (msPath.exists()) {
    setNumberOfCorrelationsPerSpw(MeasurementSet(msName()), nCorr_);
  } else {
    nCorr_ = 1;
  }
}

void SingleDishSkyCal::updateWt2(Matrix<Float> &weight, const Int &antenna1)
{
  // apply weight scaling factor
  Matrix<Float> const factor = currWtScale().xyPlane(antenna1);
  debuglog << "factor.shape() = " << factor.shape() << debugpost;
  debuglog << "weight.shape() = " << weight.shape() << debugpost;
  debuglog << "weight = " << weight << debugpost;

  auto const wtShape = weight.shape();
  size_t const nCorr = wtShape[0];
  size_t const nChan = wtShape[1];
  // for each correlation
  for (size_t iCorr = 0; iCorr < nCorr; ++iCorr) {
    auto wSlice = weight.row(iCorr);
    auto const fSlice = factor.row(iCorr);
    if (fSlice.nelements() == nChan) {
      wSlice *= fSlice;
    } else if (nChan == 1) {
      // take mean of spectral weight factor to apply it to scalar weight
      wSlice *= mean(fSlice);
    } else {
      throw AipsError("Shape mismatch between input weight and weight scaling factor");
    }
  }
}

//
// SingleDishPositionSwitchCal
//
  
// Constructor
SingleDishPositionSwitchCal::SingleDishPositionSwitchCal(VisSet& vs)
  : VisCal(vs),
    SingleDishSkyCal(vs)
{
  debuglog << "SingleDishPositionSwitchCal::SingleDishPositionSwitchCal(VisSet& vs)" << debugpost;
}

SingleDishPositionSwitchCal::SingleDishPositionSwitchCal(const MSMetaInfoForCal& msmc)
  : VisCal(msmc),
    SingleDishSkyCal(msmc)
{
  debuglog << "SingleDishPositionSwitchCal::SingleDishPositionSwitchCal(const MSMetaInfoForCal& msmc)" << debugpost;
}
  
SingleDishPositionSwitchCal::SingleDishPositionSwitchCal(const Int& nAnt)
  : VisCal(nAnt),
    SingleDishSkyCal(nAnt)
{
  debuglog << "SingleDishPositionSwitchCal::SingleDishPositionSwitchCal(const Int& nAnt)" << debugpost;
}

// Destructor
SingleDishPositionSwitchCal::~SingleDishPositionSwitchCal()
{
  debuglog << "SingleDishPositionSwitchCal::~SingleDishPositionSwitchCal()" << debugpost;
}

MeasurementSet SingleDishPositionSwitchCal::selectReferenceData(MeasurementSet const &user_selection)
{
    std::ostringstream qry;
    constexpr auto eol = '\n';
    qry << "using style python" << eol;
    qry << "with [" << eol
        << "select" << eol
        << "    [select TELESCOPE_NAME from ::OBSERVATION][OBSERVATION_ID] as TELESCOPE_NAME," << eol;
    // Purposively not using TAQL's default mscal UDF library alias for derivedmscal
    // to workaround a bug in casacore UDFBase::createUDF
    qry << "    derivedmscal.spwcol('NUM_CHAN') as NUM_CHAN" << eol
        << "from" << eol
        << "    $1" << eol
        << "] as metadata" << eol
        << "select * from $1 , metadata" << eol
        << "where " << eol;
    // Row contains single-dish auto-correlation data,
    qry << "    ( ANTENNA1 == ANTENNA2 ) and" << eol ;
    qry << "    ( FEED1 == FEED2 ) and" << eol ;

    // has not been marked as invalid,
    qry << "    ( not(FLAG_ROW) ) and " << eol ;
    // holds at least 1 data marked as valid,
    qry << "    ( not(all(FLAG)) ) and " << eol;
    // ---- Note: both conditions above are required because FLAG and FLAG_ROW are not synchronized:
    //            a valid row (FLAG_ROW=False) may contain only invalid data: all(FLAG)=True

    // has observational intent: OBSERVE_TARGET#OFF_SOURCE,
    qry << "    ( STATE_ID in [ " << eol
        << "          select rowid() " << eol
        << "          from ::STATE" << eol
        << "          where " << eol
        << "              upper(OBS_MODE) ~ m/^OBSERVE_TARGET#OFF_SOURCE/ " << eol
        << "          ]" << eol
        << "    ) and" << eol;
    // excluding - for ALMA - rows from Water Vapor Radiometers spectral windows, which have 4 channels
    qry << "    (" << eol
        << "        ( metadata.TELESCOPE_NAME != 'ALMA' ) or" << eol
        << "        (" << eol
        << "            ( metadata.TELESCOPE_NAME == 'ALMA' ) and" << eol
        << "            ( metadata.NUM_CHAN != 4 )" << eol
        << "        )" << eol
        << "     )" << eol;
    debuglog << "SingleDishPositionSwitchCal::selectReferenceData(): taql query:" << eol
             << qry.str() << debugpost;
    return MeasurementSet(tableCommand(qry.str(), user_selection));
}

void SingleDishPositionSwitchCal::fillCalibrationTable(casacore::MeasurementSet const &reference_data){
    String dataColName = (reference_data.tableDesc().isColumn("FLOAT_DATA")) ? "FLOAT_DATA" : "DATA";

    if ( dataColName == "FLOAT_DATA")
        fillCalibrationTable<FloatDataColumnAccessor>(reference_data);
    else
        fillCalibrationTable<DataColumnAccessor>(reference_data);
}

template<class DataRealComponentAccessor>
void SingleDishPositionSwitchCal::fillCalibrationTable(casacore::MeasurementSet const &reference_data)
{
    debuglog << "SingleDishPositionSwitchCal::fillCalibrationTable()" << debugpost;

    // Sort columns define the granularity at which we average data obtained
    // by observing the reference position associated with the science target
    constexpr size_t nSortColumns = 8;
    Int columns[nSortColumns] = {
        MS::OBSERVATION_ID,
        MS::PROCESSOR_ID,
        MS::FIELD_ID,
        MS::ANTENNA1,
        MS::FEED1,
        MS::DATA_DESC_ID,
        MS::SCAN_NUMBER,
        MS::STATE_ID
    };
    Int *columnsPtr = columns;
    Block<Int> sortColumns(nSortColumns, columnsPtr, false);

    // Iterator
    constexpr Bool doGroupAllTimesTogether = true;
    constexpr Double groupAllTimesTogether = doGroupAllTimesTogether ? 0.0 : -1.0;

    constexpr Bool doAddDefaultSortColumns = false;
    constexpr Bool doStoreSortedTableOnDisk = false;

    MSIter msIter(reference_data, sortColumns,
            groupAllTimesTogether, doAddDefaultSortColumns, doStoreSortedTableOnDisk);

    // Main loop: compute values of calibration table's columns
    for (msIter.origin(); msIter.more(); msIter++) {
        const auto iterTable = msIter.table();
        const auto iterRows = iterTable.nrow();
        if (iterRows == 0) continue;

        // TIME column of calibration table: mean of selected MAIN.TIME
        ScalarColumn<Double> timeCol(iterTable, "TIME");
        refTime() = mean(timeCol.getColumn());

        // FIELD_ID column of calibration table
        currSpw() = msIter.spectralWindowId();

        // SPECTRAL_WINDOW_ID column of calibration table
        currField() = msIter.fieldId();

        // ANTENNA1, ANTENNA2 columns of calibration table
        ScalarColumn<Int> antenna1Col(iterTable, "ANTENNA1");
        currAnt_ = antenna1Col(0);

        // INTERVAL column of calibration table: sum of selected MAIN.EXPOSURE
        ScalarColumn<Double> exposureCol(iterTable, "EXPOSURE");
        const auto exposure = exposureCol.getColumn();
        interval_ = sum(exposure);

        // SCAN_NUMBER, OBSERVATION_ID columns of calibration table
        // Not computed/updated here
#ifdef SDCALSKY_DEBUG
        ScalarColumn<Int> scanNumberCol(iterTable, "SCAN_NUMBER");
        const auto scan_number = scanNumberCol(0);
        ScalarColumn<Int> stateIdCol(iterTable, "STATE_ID");
        const auto state_id = stateIdCol(0);
        cout << "field=" << currField()
             << " ant=" << currAnt_
             << " ddid=" << msIter.dataDescriptionId()
             << " spw=" << currSpw()
             << " scan_number=" << scan_number
             << " state_id=" << state_id
             << " nrows=" << iterRows
             << endl;
#endif

        // FPARAM column of calibration table: weighted mean of valid data, weight = exposure
        // + PARAMERR, FLAG, SNR
        // ---- Get data shape from FLAG column
        ArrayColumn<Bool> flagCol(iterTable, "FLAG");
        const auto dataShape = flagCol.shape(0);
        const auto nCorr = dataShape[0];
        const auto nChannels = dataShape[1];
        // ---- Initialize accumulators
        Matrix<Float> weightedMean(nCorr, nChannels, 0.0f);
        Matrix<Float> weightsSums(nCorr, nChannels, 0.0f);
        // ---- Compute weighted mean of valid data
        DataRealComponentAccessor dataAccessor(iterTable);
        for (std::remove_const<decltype(iterRows)>::type iterRow = 0; iterRow < iterRows ; iterRow++){
            Matrix<Bool> dataIsValid = not flagCol(iterRow);
            MaskedArray<Float> validData(dataAccessor(iterRow), dataIsValid);
            const auto rowExposure = static_cast<Float>(exposure[iterRow]);
            weightedMean += validData * rowExposure;
            MaskedArray<Float> validWeight(Matrix<Float>(validData.shape(), rowExposure), dataIsValid);
            weightsSums += validWeight;
        }
        const Matrix<Bool> weightsSumsIsNonZero = ( weightsSums != 0.0f );
        weightedMean /= MaskedArray<Float>(weightsSums,weightsSumsIsNonZero);
        // ---- Update solveAll*() members
        const Cube<Float> realParam = weightedMean.addDegenerate(1);
        const Cube<Bool> realParamIsValid = weightsSumsIsNonZero.addDegenerate(1);
        for (std::remove_const<decltype(nCorr)>::type corr = 0; corr < nCorr; corr++) {
          solveAllRPar().yzPlane(corr) = realParam.yzPlane(corr); // FPARAM
          solveAllParOK().yzPlane(corr) = realParamIsValid.yzPlane(corr);  // not FLAG
          solveAllParErr().yzPlane(corr) = 0.1; // PARAMERR. TODO: this is tentative
          solveAllParSNR().yzPlane(corr) = 1.0; // SNR. TODO: this is tentative
        }

        // WEIGHT column of calibration table
        //

        // Update in-memory calibration table
        keepNCT();
    }
}

//
// SingleDishRasterCal
//
  
// Constructor
SingleDishRasterCal::SingleDishRasterCal(VisSet& vs)
  : VisCal(vs),
    SingleDishSkyCal(vs),
    fraction_(0.1),
    numEdge_(-1)
{
  debuglog << "SingleDishRasterCal::SingleDishRasterCal(VisSet& vs)" << debugpost;
}

SingleDishRasterCal::SingleDishRasterCal(const MSMetaInfoForCal& msmc)
  : VisCal(msmc),
    SingleDishSkyCal(msmc),
    fraction_(0.1),
    numEdge_(-1)
{
  debuglog << "SingleDishRasterCal::SingleDishRasterCal(const MSMetaInfoForCal& msmc)" << debugpost;
}

SingleDishRasterCal::SingleDishRasterCal(const Int& nAnt)
  : VisCal(nAnt),
    SingleDishSkyCal(nAnt)
{
  debuglog << "SingleDishRasterCal::SingleDishRasterCal(const Int& nAnt)" << debugpost;
}

// Destructor
SingleDishRasterCal::~SingleDishRasterCal()
{
  debuglog << "SingleDishRasterCal::~SingleDishRasterCal()" << debugpost;
}

void SingleDishRasterCal::setSolve(const Record& solve)
{
  // edge detection parameter for otfraster mode
  if (solve.isDefined("fraction")) {
    fraction_ = solve.asFloat("fraction");
  }
  if (solve.isDefined("numedge")) {
    numEdge_ = solve.asInt("numedge");
  }

  logSink() << "fraction=" << fraction_ << endl
            << "numedge=" << numEdge_ << LogIO::POST;
  
  // call parent setSolve
  SolvableVisCal::setSolve(solve);
}
  
MeasurementSet SingleDishRasterCal::selectReferenceData(MeasurementSet const &ms)
{
  debuglog << "SingleDishRasterCal::selectReferenceData" << debugpost;
  const Record specify;
  std::ostringstream oss;
  oss << "SELECT FROM $1 WHERE ";
  String delimiter = "";
  // for (Int iant = 0; iant < nAnt(); ++iant) {
  //   Vector<String> timeRangeList = detectRaster(msName(), iant, fraction_, numEdge_);
  //   debuglog << "timeRangeList=" << ::toString(timeRangeList) << debugpost;
  //   oss << delimiter;
  //   oss << "(ANTENNA1 == " << iant << " && ANTENNA2 == " << iant << " && (";
  //   String separator = "";
  //   for (size_t i = 0; i < timeRangeList.size(); ++i) {
  //     if (timeRangeList[i].size() > 0) { 
  //   	oss << separator << "(" << timeRangeList[i] << ")";
  //   	separator = " || ";
  //     }
  //   }
  //   oss << "))";
  //   debuglog << "oss.str()=" << oss.str() << debugpost;
  //   delimiter = " || ";
  // }
  // use ANTENNA 0 for reference antenna
  Vector<String> timeRangeList = detectRaster(ms, 0, fraction_, numEdge_);
  debuglog << "timeRangeList=" << ::toString(timeRangeList) << debugpost;
  oss << delimiter;
  oss << "(ANTENNA1 == ANTENNA2 && (";
  String separator = "";
  for (size_t i = 0; i < timeRangeList.size(); ++i) {
    if (timeRangeList[i].size() > 0) { 
      oss << separator << "(" << timeRangeList[i] << ")";
      separator = " || ";
    }
  }
  oss << "))";
  debuglog << "oss.str()=" << oss.str() << debugpost;
  
  oss //<< ")"
      << " ORDER BY FIELD_ID, ANTENNA1, FEED1, DATA_DESC_ID, TIME";
  return MeasurementSet(tableCommand(oss.str(), ms));
}

//
// SingleDishOtfCal
//
  
// Constructor
SingleDishOtfCal::SingleDishOtfCal(VisSet& vs)
  : VisCal(vs),
    SingleDishSkyCal(vs),
    fraction_(0.1),
	pixel_scale_(0.5),
	msSel_(vs.iter().ms())
{
  debuglog << "SingleDishOtfCal::SingleDishOtfCal(VisSet& vs)" << debugpost;
}

/*
SingleDishOtfCal::SingleDishOtfCal(const MSMetaInfoForCal& msmc)
  : VisCal(msmc),
    SingleDishSkyCal(msmc),
    fraction_(0.1),
	pixel_scale_(0.5),
	msSel_(vs.iter().ms()) ************need MS!
{
  debuglog << "SingleDishOtfCal::SingleDishOtfCal(VisSet& vs)" << debugpost;
}
*/
void SingleDishOtfCal::setSolve(const Record& solve)
{
  // edge detection parameter for otfraster mode
  if (solve.isDefined("fraction")) {
    fraction_ = solve.asFloat("fraction");
  }

  logSink() << "fraction=" << fraction_ << LogIO::POST;

  // call parent setSolve
  SolvableVisCal::setSolve(solve);
}

/*
SingleDishOtfCal::SingleDishOtfCal(const Int& nAnt)
  : VisCal(nAnt),
    SingleDishSkyCal(nAnt)
{
  debuglog << "SingleDishOtfCal::SingleDishOtfCal(const Int& nAnt)" << debugpost;
}
*/

// Destructor
SingleDishOtfCal::~SingleDishOtfCal()
{
  debuglog << "SingleDishOtfCal::~SingleDishOtfCal()" << debugpost;
}

MeasurementSet SingleDishOtfCal::selectReferenceData(MeasurementSet const &ms)
{
  PointingDirectionCalculator calc(ms);
  calc.setDirectionListMatrixShape(PointingDirectionCalculator::ROW_MAJOR);

  // Check the coordinates system type used to store the pointing measurements
  const MSPointing& tbl_pointing = ms.pointing();
  MSPointingColumns pointing_cols(tbl_pointing);
  const ROArrayMeasColumn< MDirection >& direction_cols =  pointing_cols.directionMeasCol();
  const MeasRef<MDirection>& direction_ref_frame = direction_cols.getMeasRef();
  uInt ref_frame_type = direction_ref_frame.getType();

  // If non-celestial coordinates (AZEL*) are used, convert to celestial ones
  switch (ref_frame_type) {
  case MDirection::AZEL : // Fall through
  case MDirection::AZELSW :
  case MDirection::AZELGEO :
  case MDirection::AZELSWGEO : {
        const String& ref_frame_name = MDirection::showType(ref_frame_type);
        debuglog << "Reference frame of pointings coordinates is non-celestial: " << ref_frame_name << debugpost;
        String j2000(MDirection::showType(MDirection::J2000));
        debuglog << "Pointings coordinates will be converted to: " << j2000 << debugpost;
      calc.setFrame(j2000);
      }
  }
  // Extract edge pointings for each (field_id,antenna,spectral window) triple
  // MeasurementSet 2 specification / FIELD table:
  //   . FIELD_ID column is removed
  //   . FIELD table is directly indexed using the FIELD_ID value in MAIN
  const MSField& tbl_field = ms.field();
  const String &field_col_name_str = tbl_field.columnName(MSField::MSFieldEnums::SOURCE_ID);
  ScalarColumn<Int> source_id_col(tbl_field, field_col_name_str);
  const MSAntenna& tbl_antenna = ms.antenna();
  const String &col_name_str = tbl_antenna.columnName(MSAntenna::MSAntennaEnums::NAME);
  ScalarColumn<String> antenna_name(tbl_antenna,col_name_str);
  const MSSpectralWindow& tbl_spectral_window = ms.spectralWindow();

  // make a map between SOURCE_ID and source NAME
  const MSSource &tbl_source = ms.source();
  ScalarColumn<Int> id_col(tbl_source, tbl_source.columnName(MSSource::MSSourceEnums::SOURCE_ID));
  ScalarColumn<String> name_col(tbl_source, tbl_source.columnName(MSSource::MSSourceEnums::NAME));
  std::map<Int, String> source_map;
  for (uInt irow = 0; irow < tbl_source.nrow(); ++irow) {
    auto source_id = id_col(irow);
    if (source_map.find(source_id) == source_map.end()) {
      source_map[source_id] = name_col(irow);
    }
  }

  Vector<casacore::rownr_t> rowList;

  for (uInt field_id=0; field_id < tbl_field.nrow(); ++field_id){
    String field_sel(casacore::String::toString<uInt>(field_id));
    String const source_name = source_map.at(source_id_col(field_id));

    // Set ephemeris flag if source name is the one recognized as a moving source
    if (isEphemeris(source_name)) {
      calc.setMovingSource(source_name);
    }
    else {
      calc.unsetMovingSource();
    }

    for (uInt ant_id=0; ant_id < tbl_antenna.nrow(); ++ant_id){
      String ant_sel(antenna_name(ant_id) + "&&&");
      for (uInt spw_id=0; spw_id < tbl_spectral_window.nrow(); ++spw_id){
        String spw_sel(casacore::String::toString<uInt>(spw_id));
        // Filter user selection by (field_id,antenna,spectral window) triple
        try {
          calc.selectData(ant_sel,spw_sel,field_sel);
        }
        catch (AipsError& e) { // Empty selection
          // Note: when the underlying MSSelection is empty
          // MSSelection internally catches an MSSelectionError error
          // but does not re-throw it. It throws instead an AipsError
          // copy-constructed from the MSSelectionError
          continue;
        }
        debuglog << "field_id: " << field_id
             << " ant_id: "  << ant_id
             << " spw: "     << spw_id
                 << "  ==> selection rows: " << calc.getNrowForSelectedMS() << debugpost;
        // Get time-interpolated celestial pointing directions for the filtered user selection
        Matrix<Double> pointings_dirs = calc.getDirection();
        // Project directions onto image plane
        // pixel_scale_ :
        //   . hard-coded to 0.5 in constructor
        //   . is applied to the median separation of consecutive pointing directions by the projector
        //   . projector pixel size = 0.5*directions_median
        debuglog << "pixel_scale:" << pixel_scale_ << debugpost;
        OrthographicProjector p(pixel_scale_);
        p.setDirection(pointings_dirs);
        const Matrix<Double> &pointings_coords = p.project();
        // Extract edges of the observed region for the (field_id,antenna,spectral window) triple
        Vector<Double> pointings_x(pointings_coords.row(0).copy());
        Vector<Double> pointings_y(pointings_coords.row(1).copy());
        Vector<Bool> is_edge(pointings_coords.ncolumn(),false);
        double pixel_size = 0.0;
        {
          // CAS-9956
          // Mitigation of memory usage due to unexpectedly large number of pixels.
          // Currently CreateMaskNearEdgeDouble requires 2*sizeof(size_t)*num_pixels bytes
          // of memory. If this value exceeds 2GB, the mitigation will be activated.
          Double const num_pixels = p.p_size()[0] * p.p_size()[1];
          auto const estimated_memory = num_pixels * 2 * sizeof(size_t);
          constexpr Double kMaxMemory = 2.0e9;
          if (estimated_memory >= kMaxMemory && pixel_scale_ < 1.0) {
            LogIO os;
            os << LogOrigin("PointingDirectionProjector", "scale_and_center", WHERE);
            os << LogIO::WARN << "Estimated Memory: " << estimated_memory << LogIO::POST;
            os << LogIO::WARN << "Mitigation of memory usage is activated. pixel scale is set to 1.0" << LogIO::POST;
            // pixel_size can be set to 2.0 since projection grid spacing is estimated from half of median separation
            // between neighboring pixels so that pixel_width will become about 1.0 if pixel_size is 0.
            pixel_size = 2.0;
            os << LogIO::WARN << "pixel_size is set to " << pixel_size << LogIO::POST;
           }
        }
        // libsakura 2.0: setting pixel_size=0.0 means that CreateMaskNearEdgeDouble will
        //   . compute the median separation of consecutive pointing coordinates
        //   . use an "edge detection pixel size" = 0.5*coordinates_median (pixel scale hard-coded to 0.5)
        debuglog << "sakura library function call: parameters info:" << debugpost;
        debuglog << "in: fraction: " << fraction_ << debugpost;
        debuglog << "in: pixel size: " << pixel_size << debugpost;
        debuglog << "in: pixels count: (nx = " << p.p_size()[0] << " , ny = " << p.p_size()[1] << debugpost;
        debuglog << "in: pointings_coords.ncolumn(): " << pointings_coords.ncolumn() << debugpost;
        LIBSAKURA_SYMBOL(Status) status = LIBSAKURA_SYMBOL(CreateMaskNearEdgeDouble)(
          fraction_, pixel_size,
        pointings_coords.ncolumn(), pointings_x.data(), pointings_y.data(),
          nullptr /* blc_x */, nullptr /* blc_y */,
          nullptr /* trc_x */, nullptr /* trc_y */,
        is_edge.data());
        bool edges_detection_ok = ( status == LIBSAKURA_SYMBOL(Status_kOK) );
        if ( ! edges_detection_ok ) {
          debuglog << "sakura error: status=" << status << debugpost;
        }
        AlwaysAssert(edges_detection_ok,AipsError);
        // Compute ROW ids of detected edges. ROW "ids" are ROW ids in the MS filtered by user selection.
        auto index_2_rowid = calc.getRowIdForOriginalMS();
        size_t edges_count = ntrue(is_edge);
        size_t rowListIndex = rowList.size();
        rowList.resize(rowList.size() + edges_count, True);
        for (size_t i = 0; i < is_edge.size(); ++i){
          if ( is_edge[i] ) {
            rowList[rowListIndex] = index_2_rowid[i]; // i;
            ++rowListIndex;
          }
        }
        debuglog << "edges_count=" << edges_count << debugpost;
        AlwaysAssert(edges_count > 0, AipsError);
#ifdef SDCALSKY_DEBUG
        stringstream fname;
        fname << calTableName().c_str() << ".edges."
                << field_id << "_" << ant_id << "_" << spw_id
          << ".csv" ;
        debuglog << "Save pointing directions and coordinates to:" << debugpost;
        debuglog << fname.str() << debugpost;
        ofstream ofs(fname.str());
        AlwaysAssert(ofs.good(), AipsError);
        ofs << "row_id,field_id,ant_id,spw_id,triple_key,dir_0,dir_1,coord_0,coord_1,edge_0,edge_1,is_edge" << endl;
        const auto &d0 =  pointings_dirs.row(0);
        const auto &d1 =  pointings_dirs.row(1);
        const auto &c0 =  pointings_coords.row(0);
        const auto &c1 =  pointings_coords.row(1);
        for (uInt j=0; j<d0.size(); j++) {
          ofs << index_2_rowid[j] << ","
            << field_id << "," << ant_id << "," << spw_id << ","
            << field_id << "_" << ant_id << "_" << spw_id << ","
              << d0(j) << "," << d1(j) << ","
              << c0(j) << "," << c1(j) << "," ;
          if ( is_edge[j] ) ofs << c0(j) << "," << c1(j) << "," << 1 << endl;
          else ofs << ",," << 0 << endl;
        }
#endif
      }
    }
  }
  Bool have_off_spectra = (rowList.size() > 0);
  AlwaysAssert(have_off_spectra, AipsError);
  MeasurementSet msSel = ms(rowList);
  debuglog << "rowList = " << rowList << debugpost;
  return msSel;
}

} //# NAMESPACE CASA - END

