/*
 * SDDoubleCircleGainCal.cpp
 *
 *  Created on: Jun 3, 2016
 *      Author: nakazato
 */

#include <synthesis/MeasurementComponents/SDDoubleCircleGainCal.h>
#include <synthesis/MeasurementComponents/StandardVisCal.h>
#include <synthesis/MeasurementComponents/SolvableVisCal.h>
#include <synthesis/MeasurementComponents/SDDoubleCircleGainCalImpl.h>
#include <synthesis/MeasurementComponents/SolveDataBuffer.h>
#include <synthesis/MeasurementEquations/VisEquation.h>
#include <synthesis/Utilities/PointingDirectionCalculator.h>
#include <synthesis/Utilities/PointingDirectionProjector.h>
#include <synthesis/CalTables/CTIter.h>
#include <msvis/MSVis/VisSet.h>

#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/IO/ArrayIO.h>
#include <casacore/casa/Quanta/Quantum.h>
#include <casacore/casa/Quanta/QuantumHolder.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/tables/TaQL/TableParse.h>
#include <casacore/measures/TableMeasures/ScalarQuantColumn.h>
#include <casacore/measures/TableMeasures/ArrayQuantColumn.h>
#include <casacore/ms/MeasurementSets/MSIter.h>

#include <iostream>
#include <sstream>
#include <cassert>
#include <map>

// Debug Message Handling
// if SDGAIN_DEBUG is defined, the macro debuglog and
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
//#define SDGAINDBLC_DEBUG

namespace {
struct NullLogger {
};

template<class T>
inline NullLogger &operator<<(NullLogger &logger, T /*value*/) {
  return logger;
}
}

#ifdef SDGAINDBLC_DEBUG
#define debuglog std::cerr
#define debugpost std::endl
#else
::NullLogger nulllogger;
#define debuglog nulllogger
#define debugpost 0
#endif

namespace {
inline void fillNChanParList(casacore::String const &msName,
casacore::Vector<casacore::Int> &nChanParList) {
  casacore::MeasurementSet const ms(msName);
  casacore::MSSpectralWindow const &msspw = ms.spectralWindow();
  casacore::ScalarColumn<casacore::Int> nchanCol(msspw, "NUM_CHAN");
  debuglog << "nchanCol=" << nchanCol.getColumn() << debugpost;
  nChanParList = nchanCol.getColumn()(
  casacore::Slice(0, nChanParList.nelements()));
}

template<class T>
inline casacore::String toString(casacore::Vector<T> const &v) {
  std::ostringstream oss;
  oss << "[";
  std::string delimiter = "";
  for (size_t i = 0; i < v.nelements(); ++i) {
    oss << delimiter << v[i];
    delimiter = ",";
  }
  oss << "]";
  return casacore::String(oss);
}

inline casacore::String selectOnSourceAutoCorr(
casacore::MeasurementSet const &ms) {
  debuglog << "selectOnSource" << debugpost;
  casacore::String taqlForState(
      "SELECT FLAG_ROW FROM $1 WHERE UPPER(OBS_MODE) ~ m/^OBSERVE_TARGET#ON_SOURCE/");
  casacore::Table stateSel = casacore::tableCommand(taqlForState, ms.state());
  auto stateIdList = stateSel.rowNumbers();
  debuglog << "stateIdList = " << stateIdList << debugpost;
  std::ostringstream oss;
  oss << "SELECT FROM $1 WHERE ANTENNA1 == ANTENNA2 && STATE_ID IN "
      << toString(stateIdList)
      << " ORDER BY FIELD_ID, ANTENNA1, FEED1, DATA_DESC_ID, TIME";
  return casacore::String(oss);
}

class DataColumnAccessor {
public:
  DataColumnAccessor(casacore::Table const &ms,
  casacore::String const colName = "DATA") :
      dataCol_(ms, colName) {
  }
  casacore::Matrix<casacore::Float> operator()(size_t irow) {
    return casacore::real(dataCol_(irow));
  }
  casacore::Cube<casacore::Float> getColumn() {
    return casacore::real(dataCol_.getColumn());
  }
private:
  DataColumnAccessor() {
  }
  casacore::ArrayColumn<casacore::Complex> dataCol_;
};

class FloatDataColumnAccessor {
public:
  FloatDataColumnAccessor(casacore::Table const &ms) :
      dataCol_(ms, "FLOAT_DATA") {
  }
  casacore::Matrix<casacore::Float> operator()(size_t irow) {
    return dataCol_(irow);
  }
  casacore::Cube<casacore::Float> getColumn() {
    return dataCol_.getColumn();
  }
private:
  FloatDataColumnAccessor() {
  }
  casacore::ArrayColumn<casacore::Float> dataCol_;
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

inline void updateWeight(casa::NewCalTable &ct) {
  casa::CTMainColumns ctmc(ct);

  // simply copy FPARAM
  for (size_t irow = 0; irow < ct.nrow(); ++irow) {
    ctmc.weight().put(irow, real(ctmc.cparam()(irow)));
  }
}

casacore::Double rad2arcsec(casacore::Double value_in_rad) {
  return casacore::Quantity(value_in_rad, "rad").getValue("arcsec");
}
}

using namespace casacore;
namespace casa {
SDDoubleCircleGainCal::SDDoubleCircleGainCal(VisSet &vs) :
    VisCal(vs),             // virtual base
    VisMueller(vs),         // virtual base
    GJones(vs), central_disk_size_(0.0), smooth_(True), currAnt_() {
}

SDDoubleCircleGainCal::SDDoubleCircleGainCal(const MSMetaInfoForCal& msmc) :
    VisCal(msmc),             // virtual base
    VisMueller(msmc),         // virtual base
    GJones(msmc), central_disk_size_(0.0), smooth_(True), currAnt_() {
}

SDDoubleCircleGainCal::~SDDoubleCircleGainCal() {
}

void SDDoubleCircleGainCal::setSolve() {
  central_disk_size_ = 0.0;
  smooth_ = True;

  // call parent setSolve
  SolvableVisCal::setSolve();

  // solint forcibly set to 'int'
  solint() = "int";
}

void SDDoubleCircleGainCal::setSolve(const Record &solve) {
  // parameters for double circle gain calibration
  if (solve.isDefined("smooth")) {
    smooth_ = solve.asBool("smooth");
  }
  if (solve.isDefined("radius")) {
    String size_string = solve.asString("radius");
    QuantumHolder qh;
    String error;
    qh.fromString(error, size_string);
    Quantity q = qh.asQuantity();
    central_disk_size_ = q.getValue("rad");
  }

  logSink() << LogIO::DEBUGGING << "smooth=" << smooth_ << LogIO::POST;
  logSink() << LogIO::DEBUGGING << "central disk size=" << central_disk_size_
      << " rad" << "(" << rad2arcsec(central_disk_size_) << " arcsec)"
      << LogIO::POST;

  if (central_disk_size_ < 0.0) {
    logSink() << "Negative central disk size is given" << LogIO::EXCEPTION;
  }

  // call parent setSolve
  SolvableVisCal::setSolve(solve);

  // solint forcibly set to 'int'
  solint() = "int";
}

String SDDoubleCircleGainCal::solveinfo() {
  ostringstream o;
  o << typeName() << ": " << calTableName() << " smooth="
      << (smooth_ ? "True" : "False") << endl << " radius="
      << central_disk_size_;
  if (central_disk_size_ == 0.0) {
    o << " (half of primary beam will be used)";
  }
  o << endl;
  return String(o);
}

void SDDoubleCircleGainCal::globalPostSolveTinker() {

  // apply generic post-solve stuff - is it necessary?
  SolvableVisCal::globalPostSolveTinker();

  // double-circle gain calibration is implemented here
  // assuming that given caltable (on memory) has complete
  // set of spectral data required for the calibration

  // setup worker_
  ROScalarQuantColumn<Double> antennaDiameterColumn(ct_->antenna(),
      "DISH_DIAMETER");
  ROArrayQuantColumn<Double> observingFrequencyColumn(ct_->spectralWindow(),
      "CHAN_FREQ");
  Int smoothingSize = -1;// use default smoothing size
  worker_.setCentralRegion(central_disk_size_);
  if (smooth_) {
    worker_.setSmoothing(smoothingSize);
  } else {
    worker_.unsetSmoothing();
  }

  // sort caltable by TIME
  NewCalTable sorted(ct_->sort("TIME"));
  Block<String> col(3);
  col[0] = "SPECTRAL_WINDOW_ID";
  col[1] = "FIELD_ID";
  col[2] = "ANTENNA1";
  //col[3] = "FEED1";
  CTIter ctiter(sorted,col);

  Vector<rownr_t> to_be_removed;
  while (!ctiter.pastEnd()) {
    Int const thisAntenna = ctiter.thisAntenna1();
    Quantity antennaDiameterQuant = antennaDiameterColumn(thisAntenna); // nominal
    worker_.setAntennaDiameter(antennaDiameterQuant.getValue("m"));
    debuglog<< "antenna diameter = " << worker_.getAntennaDiameter() << "m" << debugpost;
    Int const thisSpw = ctiter.thisSpw();
    Vector<Quantity> observingFrequencyQuant = observingFrequencyColumn(thisSpw);
    Double meanFrequency = 0.0;
    auto numChan = observingFrequencyQuant.nelements();
    debuglog<< "numChan = " << numChan << debugpost;
    assert(numChan > 0);
    if (numChan % 2 == 0) {
      meanFrequency = (observingFrequencyQuant[numChan / 2 - 1].getValue("Hz")
          + observingFrequencyQuant[numChan / 2].getValue("Hz")) / 2.0;
    } else {
      meanFrequency = observingFrequencyQuant[numChan / 2].getValue("Hz");
    }
    //debuglog << "mean frequency " << meanFrequency.getValue() << " [" << meanFrequency.getFullUnit() << "]" << debugpost;
    debuglog<< "mean frequency " << meanFrequency << debugpost;
    worker_.setObservingFrequency(meanFrequency);
    debuglog<< "observing frequency = " << worker_.getObservingFrequency() / 1e9 << "GHz" << debugpost;
    Double primaryBeamSize = worker_.getPrimaryBeamSize();
    debuglog<< "primary beam size = " << rad2arcsec(primaryBeamSize) << " arcsec" << debugpost;

    // get table entry sorted by TIME
    Vector<Double> time(ctiter.time());
    Cube<Complex> p(ctiter.cparam());
    Cube<Bool> fl(ctiter.flag());

    // take real part of CPARAM
    Cube<Float> preal = real(p);

    // execute double-circle gain calibration
    worker_.doCalibrate(time, preal, fl);

    // if gain calibration fail (typically due to insufficient
    // number of data points), go to next iteration step
    if (preal.empty()) {
      // add row numbers to the "TO-BE-REMOVED" list
      auto rows = ctiter.table().rowNumbers();
      size_t nelem = to_be_removed.nelements();
      size_t nelem_add = rows.nelements();
      to_be_removed.resize(nelem + nelem_add, True);
      to_be_removed(Slice(nelem, nelem_add)) = rows;
      ctiter.next();
      continue;
    }

    // set real part of CPARAM
    setReal(p, preal);

    // record result
    ctiter.setcparam(p);
    ctiter.setflag(fl);
    ctiter.next();
  }

  // remove rows registered to the "TO-BE-REMOVED" list
  if (to_be_removed.nelements() > 0) {
    ct_->removeRow(to_be_removed);
  }
}

void SDDoubleCircleGainCal::keepNCT() {
  // Call parent to do general stuff
  GJones::keepNCT();

  if (prtlev()>4)
    cout << " SDDoubleCircleGainCal::keepNCT" << endl;

  // Set proper antenna id
  Vector<Int> a1(nAnt());
  a1 = currAnt_;
  //indgen(a1);

  // We are adding to the most-recently added rows
  RefRows rows(ct_->nrow() - nElem(), ct_->nrow() - 1, 1);

  // Write to table
  CTMainColumns ncmc(*ct_);
  ncmc.antenna1().putColumnCells(rows, a1);
}

void SDDoubleCircleGainCal::syncWtScale()
{
  currWtScale().resize(currJElem().shape());

  // We use simple (pre-inversion) square of currJElem
  Cube<Float> cWS(currWtScale());
  cWS=real(currJElem()*conj(currJElem()));
  cWS(!currJElemOK())=1.0;
}

void SDDoubleCircleGainCal::selfGatherAndSolve(VisSet& vs,
    VisEquation& /* ve */) {
  SDDoubleCircleGainCalImpl sdcalib;
  debuglog << "SDDoubleCircleGainCal::selfGatherAndSolve()" << debugpost;

  // TODO: implement pre-application of single dish caltables

  debuglog << "nspw = " << nSpw() << debugpost;
  fillNChanParList(msName(), nChanParList());
  debuglog << "nChanParList=" << nChanParList() << debugpost;

  // Create a new caltable to fill up
  createMemCalTable();

  // Setup shape of solveAllRPar
  nElem() = 1;
  currAnt_.resize(nElem());
  currAnt_ = -1;
  initSolvePar();

  // re-initialize calibration solution to 0.0 and calibration flags to false
  for (Int ispw=0;ispw<nSpw();++ispw) {
    currSpw() = ispw;
    solveAllParOK() = false;
    solveAllCPar() = Complex(0.0);
  }

  // Pick up OFF spectra using STATE_ID
  auto const msSel = vs.iter().ms();
  debuglog << "configure data selection for specific calibration mode"
      << debugpost;
  auto const taql = selectOnSourceAutoCorr(msSel);
  debuglog << "taql = \"" << taql << "\"" << debugpost;
  MeasurementSet msOnSource(tableCommand(taql, msSel));
  logSink() << LogIO::DEBUGGING << "msSel.nrow()=" << msSel.nrow()
      << " msOnSource.nrow()=" << msOnSource.nrow() << LogIO::POST;
  if (msOnSource.nrow() == 0) {
    throw AipsError("No reference integration in the data.");
  }
  String dataColName =
      (msOnSource.tableDesc().isColumn("FLOAT_DATA")) ? "FLOAT_DATA" : "DATA";
  logSink() << LogIO::DEBUGGING << "dataColName = " << dataColName
      << LogIO::POST;

  if (msOnSource.tableDesc().isColumn("FLOAT_DATA")) {
    executeDoubleCircleGainCal<FloatDataColumnAccessor>(msOnSource);
  } else {
    executeDoubleCircleGainCal<DataColumnAccessor>(msOnSource);
  }

  //assignCTScanField(*ct_, msName());

  // update weight
  updateWeight(*ct_);

  // store caltable
  storeNCT();
}

void SDDoubleCircleGainCal::selfSolveOne(SDBList &sdbs) {
  // do nothing at this moment
  auto const nSDB = sdbs.nSDB();
  debuglog << "nSDB = " << nSDB << debugpost;
  for (Int i = 0; i < nSDB; ++i) {
    auto const &sdb = sdbs(i);
    debuglog << "SDB" << i << ": ";
    debuglog << "fld " << sdb.fieldId()
        << " ant " << sdb.antenna1()
        << " spw " << sdb.spectralWindow();
    auto current_precision = cerr.precision();
    cerr.precision(16);
    debuglog << " time " << sdb.time() << debugpost;
    cerr.precision(current_precision);
  }
  AlwaysAssert(nSDB == 1, AipsError);
  // DebugAssert(nSDB == 1, AipsError);

  auto &sdb = sdbs(0);
  debuglog << "spw = " << sdb.spectralWindow()[0] << " antenna1 = "
      << sdb.antenna1()[0] << "," << sdb.antenna2()[0] << " nRows = "
      << sdb.nRows() << debugpost;

  debuglog << "solveAllCPar().shape() = " << solveAllCPar().shape()
      << debugpost;
  debuglog << "visCube.shape() = " << sdb.visCubeCorrected().shape()
      << debugpost;

  auto const &corrected = sdb.visCubeCorrected();
  auto const &flag = sdb.flagCube();

  if (!corrected.conform(solveAllCPar())) {
    // resize solution array
    nElem() = sdb.nRows();
    currAnt_.resize(nElem());
    sizeSolveParCurrSpw(sdb.nChannels());
  }

  solveAllCPar() = Complex(1.0);
  solveAllParOK() = false;
  currAnt_ = sdb.antenna1();

  size_t const numCorr = corrected.shape()[0];
  for (size_t iCorr = 0; iCorr < numCorr; ++iCorr) {
    solveAllCPar().yzPlane(iCorr) = corrected.yzPlane(iCorr);
    solveParOK().yzPlane(iCorr) = !flag.yzPlane(iCorr);
    solveAllParErr().yzPlane(iCorr) = 0.1; // TODO
    solveAllParSNR().yzPlane(iCorr) = 1.0; // TODO
  }
}

template<class Accessor>
void SDDoubleCircleGainCal::executeDoubleCircleGainCal(
    MeasurementSet const &ms) {
  logSink() << LogOrigin("SDDoubleCircleGainCal", __FUNCTION__, WHERE);
  // setup worker class
      SDDoubleCircleGainCalImpl worker;

      Int smoothingSize = -1;// use default smoothing size
      worker.setCentralRegion(central_disk_size_);
      if (smooth_) {
        worker.setSmoothing(smoothingSize);
      } else {
        worker.unsetSmoothing();
      }

//      ArrayColumn<Double> uvwColumn(ms, "UVW");
//      Matrix<Double> uvw = uvwColumn.getColumn();
//      debuglog<< "uvw.shape " << uvw.shape() << debugpost;
//
      // make a map between SOURCE_ID and source NAME
      auto const &sourceTable = ms.source();
      ScalarColumn<Int> idCol(sourceTable,
          sourceTable.columnName(MSSource::MSSourceEnums::SOURCE_ID));
      ScalarColumn<String> nameCol(sourceTable,
          sourceTable.columnName(MSSource::MSSourceEnums::NAME));
      std::map<Int, String> sourceMap;
      for (uInt irow = 0; irow < sourceTable.nrow(); ++irow) {
        auto sourceId = idCol(irow);
        if (sourceMap.find(sourceId) == sourceMap.end()) {
          sourceMap[sourceId] = nameCol(irow);
        }
      }

      // make a map between FIELD_ID and SOURCE_ID
      auto const &fieldTable = ms.field();
      idCol.attach(fieldTable,
          fieldTable.columnName(MSField::MSFieldEnums::SOURCE_ID));
      ROArrayMeasColumn<MDirection> dirCol(fieldTable, "REFERENCE_DIR");
      std::map<Int, Int> fieldMap;
      for (uInt irow = 0; irow < fieldTable.nrow(); ++irow) {
        auto sourceId = idCol(irow);
        fieldMap[static_cast<Int>(irow)] = sourceId;
      }

      // access to subtable columns
      ROScalarQuantColumn<Double> antennaDiameterColumn(ms.antenna(),
          "DISH_DIAMETER");
      ROArrayQuantColumn<Double> observingFrequencyColumn(ms.spectralWindow(),
          "CHAN_FREQ");

      // traverse MS
      Int cols[] = {MS::FIELD_ID, MS::ANTENNA1, MS::FEED1, MS::DATA_DESC_ID};
      Int *colsp = cols;
      Block<Int> sortCols(4, colsp, false);
      MSIter msIter(ms, sortCols, 0.0, false, false);
      for (msIter.origin(); msIter.more(); msIter++) {
        MeasurementSet const currentMS = msIter.table();

        uInt nrow = currentMS.nrow();
        debuglog<< "nrow = " << nrow << debugpost;
        if (nrow == 0) {
          debuglog<< "Skip" << debugpost;
          continue;
        }
        Int ispw = msIter.spectralWindowId();
        if (nChanParList()[ispw] == 4) {
          // Skip WVR
          debuglog<< "Skip " << ispw
          << "(nchan " << nChanParList()[ispw] << ")"
          << debugpost;
          continue;
        }
        logSink() << "Process spw " << ispw
        << "(nchan " << nChanParList()[ispw] << ")" << LogIO::POST;

        Int ifield = msIter.fieldId();
        ScalarColumn<Int> antennaCol(currentMS, "ANTENNA1");
        //currAnt_ = antennaCol(0);
        Int iantenna = antennaCol(0);
        ScalarColumn<Int> feedCol(currentMS, "FEED1");
        debuglog<< "FIELD_ID " << msIter.fieldId()
        << " ANTENNA1 " << iantenna//currAnt_
        << " FEED1 " << feedCol(0)
        << " DATA_DESC_ID " << msIter.dataDescriptionId()
        << debugpost;

        // setup PointingDirectionCalculator
        PointingDirectionCalculator pcalc(currentMS);
        pcalc.setDirectionColumn("DIRECTION");
        pcalc.setFrame("J2000");
        pcalc.setDirectionListMatrixShape(PointingDirectionCalculator::ROW_MAJOR);
    debuglog<< "SOURCE_ID " << fieldMap[ifield] << " SOURCE_NAME " << sourceMap[fieldMap[ifield]] << debugpost;
    auto const isEphem = ::isEphemeris(sourceMap[fieldMap[ifield]]);
    Matrix<Double> offset_direction;
    if (isEphem) {
      pcalc.setMovingSource(sourceMap[fieldMap[ifield]]);
      offset_direction = pcalc.getDirection();
    } else {
      pcalc.unsetMovingSource();
      Matrix<Double> direction = pcalc.getDirection();

      // absolute coordinate -> offset from center
      OrthographicProjector projector(1.0f);
      projector.setDirection(direction);
      Vector<MDirection> md = dirCol.convert(ifield, MDirection::J2000);
      //logSink() << "md.shape() = " << md.shape() << LogIO::POST;
      auto const qd = md[0].getAngle("rad");
      auto const d = qd.getValue();
      auto const lat = d[0];
      auto const lon = d[1];
      logSink() << "reference coordinate: lat = " << lat << " lon = " << lon << LogIO::POST;
      projector.setReferencePixel(0.0, 0.0);
      projector.setReferenceCoordinate(lat, lon);
      offset_direction = projector.project();
      auto const pixel_size = projector.pixel_size();
      // convert offset_direction from pixel to radian
      offset_direction *= pixel_size;
    }
//    debuglog<< "offset_direction = " << offset_direction << debugpost;
//    Double const *direction_p = offset_direction.data();
//    for (size_t i = 0; i < 10; ++i) {
//      debuglog<< "offset_direction[" << i << "]=" << direction_p[i] << debugpost;
//    }

    ScalarColumn<Double> timeCol(currentMS, "TIME");
    Vector<Double> time = timeCol.getColumn();
    Accessor dataCol(currentMS);
    Cube<Float> data = dataCol.getColumn();
    ArrayColumn<Bool> flagCol(currentMS, "FLAG");
    Cube<Bool> flag = flagCol.getColumn();
//    debuglog<< "data = " << data << debugpost;

    Vector<Double> gainTime;
    Cube<Float> gain;
    Cube<Bool> gain_flag;

    // tell some basic information to worker object
    Quantity antennaDiameterQuant = antennaDiameterColumn(iantenna);
    worker.setAntennaDiameter(antennaDiameterQuant.getValue("m"));
    debuglog<< "antenna diameter = " << worker.getAntennaDiameter() << "m" << debugpost;
    Vector<Quantity> observingFrequencyQuant = observingFrequencyColumn(ispw);
    Double meanFrequency = 0.0;
    auto numChan = observingFrequencyQuant.nelements();
    debuglog<< "numChan = " << numChan << debugpost;
    assert(numChan > 0);
    if (numChan % 2 == 0) {
      meanFrequency = (observingFrequencyQuant[numChan / 2 - 1].getValue("Hz")
          + observingFrequencyQuant[numChan / 2].getValue("Hz")) / 2.0;
    } else {
      meanFrequency = observingFrequencyQuant[numChan / 2].getValue("Hz");
    }
    //debuglog << "mean frequency " << meanFrequency.getValue() << " [" << meanFrequency.getFullUnit() << "]" << debugpost;
    debuglog<< "mean frequency " << meanFrequency << debugpost;
    worker.setObservingFrequency(meanFrequency);
    debuglog<< "observing frequency = " << worker.getObservingFrequency() / 1e9 << "GHz" << debugpost;
    Double primaryBeamSize = worker.getPrimaryBeamSize();
    debuglog<< "primary beam size = " << rad2arcsec(primaryBeamSize) << " arcsec" << debugpost;

    auto const effective_radius = worker.getRadius();
    logSink() << "effective radius of the central region = " << effective_radius << " arcsec"
        << " (" << rad2arcsec(effective_radius) << " arcsec)"<< LogIO::POST;
    if (worker.isSmoothingActive()) {
      auto const effective_smoothing_size = worker.getEffectiveSmoothingSize();
      logSink() << "smoothing activated. effective size = " << effective_smoothing_size << LogIO::POST;
    }
    else {
      logSink() << "smoothing deactivated." << LogIO::POST;
    }

    // execute calibration
    worker.calibrate(data, flag, time, offset_direction, gainTime, gain, gain_flag);
    //debuglog<< "gain_time = " << gain_time << debugpost;
    //debuglog<<"gain = " << gain << debugpost;
    size_t numGain = gainTime.nelements();
    debuglog<< "number of gain " << numGain << debugpost;

    currSpw() = ispw;

    // make sure storage and flag for calibration solution allocated
    // for the current spw are properly initialized
    solveAllCPar() = Complex(0.0);
    solveAllParOK() = false;

    currField() = ifield;
    currAnt_ = iantenna;
    debuglog << "antenna is " << currAnt_ << debugpost;

    size_t numCorr = gain.shape()[0];
    Slice const chanSlice(0, numChan);
    for (size_t i = 0; i < numGain; ++i) {
      refTime() = gainTime[i];
      Slice const rowSlice(i, 1);
      for (size_t iCorr = 0; iCorr < numCorr; ++iCorr) {
        Slice const corrSlice(iCorr, 1);
        auto cparSlice = solveAllCPar()(corrSlice, chanSlice, Slice(0, 1));
        convertArray(cparSlice, gain(corrSlice, chanSlice, rowSlice));
        solveAllParOK()(corrSlice, chanSlice, Slice(0, 1)) = !gain_flag(corrSlice, chanSlice, rowSlice);
        solveAllParErr().yzPlane(iCorr) = 0.1; // TODO
        solveAllParSNR().yzPlane(iCorr) = 1.0; // TODO
      }

      keepNCT();
    }

  }
}
}
