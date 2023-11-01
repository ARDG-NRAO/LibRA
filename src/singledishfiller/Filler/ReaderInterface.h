/*
 * ReaderInterface.h
 *
 *  Created on: Jan 5, 2016
 *      Author: nakazato
 */

#ifndef SINGLEDISH_FILLER_READERINTERFACE_H_
#define SINGLEDISH_FILLER_READERINTERFACE_H_

// std includes
#include <string>

#include <casacore/casa/Containers/Record.h>
#include <casacore/measures/Measures/MDirection.h>
#include <casacore/tables/Tables/TableRecord.h>
#include <casacore/measures/Measures/Stokes.h>
#include <casacore/tables/Tables/Table.h>
#include <casacore/tables/Tables/ScaColDesc.h>
#include <casacore/tables/Tables/SetupNewTab.h>

#include <singledishfiller/Filler/AntennaRecord.h>
#include <singledishfiller/Filler/DataRecord.h>
#include <singledishfiller/Filler/FieldRecord.h>
#include <singledishfiller/Filler/FillerUtil.h>
#include <singledishfiller/Filler/ObservationRecord.h>
#include <singledishfiller/Filler/ProcessorRecord.h>
#include <singledishfiller/Filler/SourceRecord.h>
#include <singledishfiller/Filler/SpectralWindowRecord.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// forward declaration
template<class T> class NullOptionalTables;

// NonCopyable Mixin (CRTP)
template<class T>
class NonCopyable {
protected:
  NonCopyable() {
  }
  ~NonCopyable() {
  }
private:
  NonCopyable(NonCopyable const &) {
  }
  T &operator=(T const &) {
  }
};

class ReaderInterface: private NonCopyable<ReaderInterface> {
public:
  // OptionalTables must have a public static method, Generate,
  // whose signature is as follows:
  //
  // static void Generate(casacore::Table &, Reader const &)
  //
  // OptionalTables::Generate is responsible for generating
  // optional subtables that stores supplemental information
  // not being able to stored into any other tables.
  // NullOptionalTables is an implementation of "do nothing".
  using OptionalTables = NullOptionalTables<ReaderInterface>;

  ReaderInterface(std::string const &name) :
    name_(name) {
  }

  virtual ~ReaderInterface() {
  }

  std::string const &getName() const {
    return name_;
  }

  // getDataUnit should return a string representing the unit
  // of the data. If physical unit is not yet set (e.g. before
  // calibration), please return empty sting.
  virtual casacore::String getDataUnit() const = 0;

  // isFloatData should return
  // *True* if resulting MS should have FLOAT_DATA column, while
  // *False* if the MS require DATA column instead
  virtual casacore::Bool isFloatData() const = 0;

  // getDirectionFrame should return appropriate direction
  // frame information as a form of MDirection::Types enum
  virtual casacore::MDirection::Types getDirectionFrame() const = 0;

  void initialize() {
    initializeCommon();
    initializeSpecific();
  }

  void finalize() {
    finalizeCommon();
    finalizeSpecific();
  }

  // get number of rows for MAIN table
  virtual size_t getNumberOfRows() const = 0;

  // to get OBSERVATION table
  // The method should return true if row entry is available.
  // If it return false, row will be invalid so it should not be used.
  virtual casacore::Bool getObservationRow(sdfiller::ObservationRecord &record) = 0;

  // to get ANTENNA table
  // The method should return true if row entry is available.
  // If it return false, row will be invalid so it should not be used.
  virtual casacore::Bool getAntennaRow(sdfiller::AntennaRecord &record) = 0;

  // to get PROCESSOR table
  // The method should return true if row entry is available.
  // If it return false, row will be invalid so it should not be used.
  virtual casacore::Bool getProcessorRow(sdfiller::ProcessorRecord &record) = 0;

  // to get SOURCE table
  // The method should return true if row entry is available.
  // If it return false, row will be invalid so it should not be used.
  virtual casacore::Bool getSourceRow(sdfiller::SourceRecord &row) = 0;

  // to get FIELD table
  // The method should return true if row entry is available.
  // If it return false, row will be invalid so it should not be used.
  virtual casacore::Bool getFieldRow(sdfiller::FieldRecord &row) = 0;

  // to get SPECTRAL WINDOW table
  // The method should return true if row entry is available.
  // If it return false, row will be invalid so it should not be used.
  virtual casacore::Bool getSpectralWindowRow(sdfiller::SpectralWindowRecord &row) = 0;

  // for DataAccumulator
  virtual casacore::Bool getData(size_t irow, sdfiller::DataRecord &record) = 0;

protected:
  virtual void initializeSpecific() = 0;
  virtual void finalizeSpecific() = 0;

  std::string const name_;

private:
  // common initialization/finalization actions
  void initializeCommon() {
  }
  void finalizeCommon() {
  }
};

// empty OptionalTables class
template<class Reader>
class NullOptionalTables {
public:
  static void Generate(casacore::Table &/*table*/, Reader const &/*reader*/) {
    //std::cout << "This is default. NullOptionalTables::Generate" << std::endl;
  }
};

} //# NAMESPACE CASA - END

#endif /* SINGLEDISH_FILLER_READERINTERFACE_H_ */
