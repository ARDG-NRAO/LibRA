/*
 * Scantable2MSFiller.h
 *
 *  Created on: Jan 5, 2016
 *      Author: nakazato
 */

#ifndef SINGLEDISH_FILLER_SINGLEDISHMSFILLER_H_
#define SINGLEDISH_FILLER_SINGLEDISHMSFILLER_H_

#include <string>
#include <memory>
#include <map>

#include <casacore/casa/OS/File.h>
#include <casacore/casa/OS/Path.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/casa/IO/ArrayIO.h>
#include <casacore/measures/Measures/MDirection.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/ms/MeasurementSets/MSMainColumns.h>
#include <casacore/ms/MeasurementSets/MSDataDescColumns.h>
#include <casacore/ms/MeasurementSets/MSSysCalColumns.h>
#include <casacore/ms/MeasurementSets/MSPointingColumns.h>
#include <casacore/ms/MeasurementSets/MSPolColumns.h>
#include <casacore/ms/MeasurementSets/MSFeedColumns.h>
#include <casacore/ms/MeasurementSets/MSStateColumns.h>
#include <casacore/ms/MeasurementSets/MSWeatherColumns.h>
#include <casacore/tables/Tables/TableRow.h>
#include <casacore/tables/Tables/ArrayColumn.h>
#include <casacore/tables/Tables/ScalarColumn.h>
#include <casacore/tables/Tables/ScaColDesc.h>
#include <singledishfiller/Filler/DataAccumulator.h>
#include <singledishfiller/Filler/FillerUtil.h>
#include <singledishfiller/Filler/PThreadUtil.h>
#include <singledishfiller/Filler/SysCalRecord.h>
#include <singledishfiller/Filler/WeatherRecord.h>

namespace casa { //# NAMESPACE CASA - BEGIN
namespace sdfiller {
class DataAccumulator;
}

template<typename Reader>
class SingleDishMSFiller {
public:
  // static methods for parallel processing
  inline static void create_context();
  inline static void destroy_context();
  static void *consume(void *arg);
  static void *produce(void *arg);
  inline static void fillMainMT(SingleDishMSFiller<Reader> *filler);

  // constructor
  SingleDishMSFiller(std::string const &name, bool parallel=false);

  // destructor
  ~SingleDishMSFiller();

  // access to reader object
  Reader const &getReader() const {
    return *reader_;
  }

  std::string const &getDataName() const {
    return reader_->getName();
  }

  // top level method to fill casacore::MS by reading input data
  void fill();

  // save
  void save(std::string const &name);

private:
  // optional table generator
  using OptionalTables = typename Reader::OptionalTables;

  // initialization
  void initialize();

  // finalization
  void finalize();

  // setup casacore::MS as Scratch table
  // The table will be non-Scratch when it is saved
  void setupMS();

  // fill tables that can be processed prior to main loop
  void fillPreProcessTables();

  // fill tables that must be processed after main loop
  void fillPostProcessTables();

  // fill MAIN table
  void fillMain();

  // flush accumulated data
  inline void flush(sdfiller::DataAccumulator &accumulator);

  void sortPointing();

  // Fill subtables
  // fill ANTENNA table
  void fillAntenna();

  // fill OBSERVATION table
  void fillObservation();

  // fill PROCESSOR table
  void fillProcessor();

  // fill SOURCE table
  void fillSource();

  // fill SOURCE table
  void fillField();

  // fill SPECTRAL_WINDOW table
  void fillSpectralWindow();

  // fill HISTORY table
  void fillHistory();

  // update POLARIZATION table
  // @param[in] corr_type polarization type list
  // @param[in] num_pol number of polarization components
  // @return polarization id
  inline casacore::Int updatePolarization(casacore::Vector<casacore::Int> const &corr_type,
      casacore::Int const &num_pol);

  // update DATA_DESCRIPTION table
  // @param[in] polarization_id polarization id
  // @param[in] spw_id spectral window id
  // @return data description id
  inline casacore::Int updateDataDescription(casacore::Int const &polarization_id,
      casacore::Int const &spw_id);

  // update STATE table
  // @param[in] subscan subscan number
  // @param[in] obs_mode observing mode string
  // @return state id
  inline casacore::Int updateState(casacore::Int const &subscan, casacore::String const &obs_mode);

  // update FEED table
  // @param[in] feed_id feed ID
  // @param[in] spw_id spectral window ID
  // @param[in] pol_type polarization type
  // @return feed row number
  inline casacore::Int updateFeed(casacore::Int const &feed_id, casacore::Int const &spw_id,
      casacore::String const &pol_type);

  // update POINTING table
  // @param[in] antenna_id antenna id
  // @param[in] time time stamp
  // @param[in] direction pointing direction
  inline casacore::Int updatePointing(casacore::Int const &antenna_id, casacore::Int const &feed_id,
      casacore::Double const &time, casacore::Double const &interval,
      casacore::Matrix<casacore::Double> const &direction);

  inline void updateWeather(casacore::Int const &antenna_id, casacore::Double const &time,
      casacore::Double const &interval, sdfiller::MSDataRecord const &data_record);

  inline void updateWeather(casacore::MSWeatherColumns &columns, casacore::uInt irow,
      sdfiller::WeatherRecord const &record);

  inline void updateSysCal(casacore::Int const &antenna_id, casacore::Int const &feed_id,
      casacore::Int const &spw_id, casacore::Double const &time, casacore::Double const &interval,
      sdfiller::MSDataRecord const &data_record);

  inline void updateSysCal(casacore::MSSysCalColumns &columns, casacore::uInt irow,
      sdfiller::SysCalRecord const &record);

  // update MAIN table
  // @param[in] fieldId field id
  // @param[in] feedId feed id
  // @param[in] dataDescriptionId data description id
  // @param[in] stateId state id
  // @param[in] mainSpec main table row specification except id
  inline void updateMain(casacore::Int const &antenna_id, casacore::Int field_id, casacore::Int feedId,
      casacore::Int dataDescriptionId, casacore::Int stateId, casacore::Int const &scan_number,
      casacore::Double const &time, sdfiller::MSDataRecord const &dataRecord);

  std::unique_ptr<casacore::MeasurementSet> ms_;
  std::unique_ptr<casacore::MSMainColumns> ms_columns_;
  std::unique_ptr<casacore::MSDataDescColumns> data_description_columns_;
  std::unique_ptr<casacore::MSFeedColumns> feed_columns_;
  std::unique_ptr<casacore::MSPointingColumns> pointing_columns_;
  std::unique_ptr<casacore::MSPolarizationColumns> polarization_columns_;
  std::unique_ptr<casacore::MSSysCalColumns> syscal_columns_;
  std::unique_ptr<casacore::MSStateColumns> state_columns_;
  std::unique_ptr<casacore::MSWeatherColumns> weather_columns_;
  std::unique_ptr<Reader> reader_;
  bool is_float_;
  casacore::String data_key_;

  // for POINTING table
  casacore::Int reference_feed_;
  std::map<casacore::Int, casacore::Vector<casacore::Double>> pointing_time_;
  std::map<casacore::Int, casacore::Double> pointing_time_max_;
  std::map<casacore::Int, casacore::Double> pointing_time_min_;
  casacore::Vector<casacore::uInt> num_pointing_time_;

  // for SYSCAL table
  std::vector<sdfiller::SysCalTableRecord> syscal_list_;

  // for STATE table
  std::vector<casacore::Int> subscan_list_;

  // for FEED table
  std::vector<casacore::Vector<casacore::String> *> polarization_type_pool_;

  // for WEATHER table
  std::vector<sdfiller::WeatherRecord> weather_list_;

  // casacore::Data storage to interact with DataAccumulator
  sdfiller::MSDataRecord record_;

  // for parallel processing
  casacore::Bool const parallel_;
}
;

} //# NAMESPACE CASA - END

#include <singledishfiller/Filler/SingleDishMSFiller.tcc>

#endif /* SINGLEDISH_FILLER_SINGLEDISHMSFILLER_H_ */
