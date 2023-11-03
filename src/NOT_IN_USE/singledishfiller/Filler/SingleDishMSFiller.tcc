/*
 * SingleDishMSFiller.tcc
 *
 *  Created on: Jan 8, 2016
 *      Author: nakazato
 */

#ifndef SINGLEDISH_FILLER_SINGLEDISHMSFILLER_TCC_
#define SINGLEDISH_FILLER_SINGLEDISHMSFILLER_TCC_

#include <singledishfiller/Filler/SingleDishMSFiller.h>

#include <iostream>
#include <map>
#include <memory>

#include <casacore/casa/Arrays/Cube.h>
#include <casacore/casa/OS/File.h>
#include <casacore/casa/Utilities/Regex.h>
#include <casacore/casa/Utilities/Sort.h>

#include <casacore/measures/Measures/Stokes.h>

#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/ms/MeasurementSets/MSDataDescColumns.h>
#include <casacore/ms/MeasurementSets/MSPolColumns.h>
#include <casacore/ms/MeasurementSets/MSPointingColumns.h>
#include <casacore/ms/MeasurementSets/MSStateColumns.h>
#include <casacore/ms/MeasurementSets/MSFeedColumns.h>
#include <casacore/ms/MeasurementSets/MSSourceColumns.h>

#include <casacore/tables/Tables/Table.h>
#include <casacore/tables/Tables/ScalarColumn.h>
#include <casacore/tables/Tables/SetupNewTab.h>
#include <singledishfiller/Filler/AntennaRecord.h>
#include <singledishfiller/Filler/FieldRecord.h>
#include <singledishfiller/Filler/FillerUtil.h>
#include <singledishfiller/Filler/ObservationRecord.h>
#include <singledishfiller/Filler/ProcessorRecord.h>
#include <singledishfiller/Filler/SourceRecord.h>
#include <singledishfiller/Filler/SpectralWindowRecord.h>
#include <singledishfiller/Filler/SysCalRecord.h>
#include <singledishfiller/Filler/WeatherRecord.h>

using namespace casacore;

#define ARRAY_BLOCK_SIZE 1024

namespace {

template<class _Table, class _Record, class _Reader>
inline void fillTable(_Table &table, _Record &record, _Reader const &reader) {
  POST_START;

  typename _Record::AssociatingColumns columns(table);

  size_t irow = 0;
  record.clear();
  for (casacore::Bool more_rows = reader(record); more_rows == true;
      more_rows = reader(record)) {
    record.add(table, columns);
    record.fill(irow, columns);
    ++irow;
    record.clear();
  }

  POST_END;
}

template<class _Table, class _Columns, class _Comparer, class _Updater>
inline casacore::Int updateTable(_Table &mytable, _Columns &mycolumns,
    _Comparer const &comparer, _Updater const &updater) {
  POST_START;

  casacore::Int id = -1;
  if (mycolumns.nrow() >= (casacore::uInt) INT_MAX) {
    throw casacore::AipsError("Too much row in table");
  }
  for (casacore::uInt i = 0; i < mycolumns.nrow(); ++i) {
    if (comparer(mycolumns, i)) {
      id = (casacore::Int) i;
    }
  }
  if (id < 0) {
    id = mycolumns.nrow();
    mytable.addRow(1, true);
    updater(mycolumns, id);
  }

  POST_END;
  return id;
}

template<class _Columns, class _Record>
inline void updateSubtable(_Columns &columns, casacore::uInt irow,
    _Record const &record) {
  // only update timestamp and interval
  casacore::Double time_org = columns.time()(irow);
  casacore::Double interval_org = columns.interval()(irow);

  casacore::Double time_min_org = time_org - interval_org / 2.0;
  casacore::Double time_max_org = time_org + interval_org / 2.0;

  casacore::Double time_min_in = record.time - record.interval / 2.0;
  casacore::Double time_max_in = record.time + record.interval / 2.0;

  casacore::Double time_min_new = min(time_min_org, time_min_in);
  casacore::Double time_max_new = max(time_max_org, time_max_in);

  if (time_min_new != time_min_org || time_max_new != time_max_org) {
    casacore::Double time_new = (time_min_new + time_max_new) / 2.0;
    casacore::Double interval_new = time_max_new - time_min_new;
    columns.time().put(irow, time_new);
    columns.interval().put(irow, interval_new);
  }
}

inline void makeSourceMap(casacore::MSSource const &table, casacore::Record &source_map) {
  POST_START;

  casacore::ScalarColumn<casacore::String> name_column(table, "NAME");
  casacore::ScalarColumn<casacore::Int> id_column(table, "SOURCE_ID");
  casacore::Vector<casacore::Int> id = id_column.getColumn();

  casacore::Sort sorter;
  sorter.sortKey(id.data(), TpInt);
  casacore::Vector<casacore::uInt> unique_vector;
  casacore::uInt num_id = sorter.sort(unique_vector, id.nelements(),
      casacore::Sort::HeapSort | casacore::Sort::NoDuplicates);
  for (casacore::uInt i = 0; i < num_id; ++i) {
    casacore::uInt irow = unique_vector[i];
    casacore::String const source_name = name_column(irow);
    casacore::Int const source_id = id[irow];
    source_map.define(source_name, source_id);
  }

  POST_END;
}

} // anonymous namespace

using namespace casacore;

namespace casa { //# NAMESPACE CASA - BEGIN

using namespace sdfiller;

static constexpr ssize_t CONTEXT_BUFFER_SIZE = 10;
static constexpr ssize_t FILLER_STORAGE_SIZE = CONTEXT_BUFFER_SIZE + 2;
typedef sdfiller::ProducerConsumerModelContext<ssize_t, CONTEXT_BUFFER_SIZE> PCMContext;

extern PCMContext *g_context_p;
extern casa::sdfiller::DataRecord *g_storage_p;

template<class T>
void SingleDishMSFiller<T>::create_context() {
  static_assert(0 < FILLER_STORAGE_SIZE && FILLER_STORAGE_SIZE < SSIZE_MAX,
      "FILLER_STORAGE_SIZE is wrong value");
  static_assert(CONTEXT_BUFFER_SIZE + 2 <= FILLER_STORAGE_SIZE,
      "FILLER_STORAGE_SIZE < CONTEXT_BUFFER_SIZE + 2");

  constexpr ssize_t END_OF_PRODUCTION = -1;
  if (!g_context_p) {
    g_context_p = new PCMContext(END_OF_PRODUCTION);
  }
  if (!g_storage_p) {
    g_storage_p = new DataRecord[FILLER_STORAGE_SIZE];
  }
}

template<class T>
void SingleDishMSFiller<T>::destroy_context() {
  if (g_context_p) {
    delete g_context_p;
    g_context_p = nullptr;
  }
  if (g_storage_p) {
    delete[] g_storage_p;
    g_storage_p = nullptr;
  }
}

template<class T>
void *SingleDishMSFiller<T>::consume(void *arg) {
  POST_START;

  try {
    auto filler = reinterpret_cast<SingleDishMSFiller<T> *>(arg);
    auto reader = filler->reader_.get();

//      std::ostringstream oss;

    size_t nrow = reader->getNumberOfRows();
    assert(nrow < SIZE_MAX);
    DataAccumulator accumulator;

//      oss << "consume: nrow = " << nrow;
//      PCMContext::locked_print(oss.str(), g_context_p);

    ssize_t storage_index;
    for (size_t irow = 0; irow < nrow + 1; ++irow) {
//        oss.str("");
//        oss << "consume: start row " << irow;
//        PCMContext::locked_print(oss.str(), g_context_p);
      bool more_products = PCMContext::consume(g_context_p, &storage_index);
      assert(storage_index < FILLER_STORAGE_SIZE);

//        oss.str("");
//        oss << "consume index " << storage_index << " more_products = "
//            << more_products;
//        PCMContext::locked_print(oss.str(), g_context_p);

      if (more_products) {
        DataRecord *record = &g_storage_p[storage_index];

//          oss.str("");
//          oss << "Accumulate record: time = " << record->time << " interval = "
//              << record->interval;
//          PCMContext::locked_print(oss.str(), g_context_p);

        casacore::Bool is_ready = accumulator.queryForGet(record->time);
        if (is_ready) {
          filler->flush(accumulator);
        }
        casacore::Bool astatus = accumulator.accumulate(*record);
        (void) astatus;

//          oss.str("");
//          oss << "astatus = " << astatus;
//          PCMContext::locked_print(oss.str(), g_context_p);
      } else {
        break;
      }
    }

//      PCMContext::locked_print("Final flush", g_context_p);
    filler->flush(accumulator);
  } catch (std::runtime_error &e) {
    std::ostringstream oss;
    oss << "Exception: " << e.what();
    PCMContext::locked_print(oss.str(), g_context_p);
  } catch (...) {
    PCMContext::locked_print("Unknown exception", g_context_p);
  }

  POST_END;
  pthread_exit(0);
}

template<class T>
void *SingleDishMSFiller<T>::produce(void *arg) {
  POST_START;

  try {
    auto filler = reinterpret_cast<SingleDishMSFiller<T> *>(arg);
    auto reader = filler->reader_.get();

//      std::ostringstream oss;

    size_t nrow = reader->getNumberOfRows();

//      oss << "produce: nrow = " << nrow;
//      PCMContext::locked_print(oss.str(), g_context_p);

    ssize_t storage_index = 0;

    for (size_t irow = 0; irow < nrow; ++irow) {

      DataRecord *record = &g_storage_p[storage_index];
      casacore::Bool status = reader->getData(irow, *record);

//        oss.str("");
//        oss << "irow " << irow << " status " << status << std::endl;
//        oss << "   TIME=" << record->time << " INTERVAL=" << record->interval
//            << std::endl;
//        oss << "status = " << status;
//        PCMContext::locked_print(oss.str(), g_context_p);

      if (status) {
//          oss.str("");
//          oss << "produce index " << storage_index;
//          PCMContext::locked_print(oss.str(), g_context_p);

        PCMContext::produce(g_context_p, storage_index);
      } else {
        break;
      }

      storage_index++;
      storage_index %= FILLER_STORAGE_SIZE;
    }

//      PCMContext::locked_print("Done production", g_context_p);

    PCMContext::complete_production(g_context_p);

  } catch (std::runtime_error &e) {
    std::ostringstream oss;
    oss << "Exception: " << e.what();
    PCMContext::locked_print(oss.str(), g_context_p);

    PCMContext::complete_production(g_context_p);
  } catch (...) {
    PCMContext::locked_print("Unknown exception", g_context_p);

    PCMContext::complete_production(g_context_p);
  }

  POST_END;
  pthread_exit(0);
}

template<class T>
void SingleDishMSFiller<T>::fillMainMT(SingleDishMSFiller<T> *filler) {
  POST_START;

  pthread_t consumer_id;
  pthread_t producer_id;

  // create context
  SingleDishMSFiller<T>::create_context();

  try {
    sdfiller::create_thread(&consumer_id, NULL, SingleDishMSFiller<T>::consume,
        filler);
    sdfiller::create_thread(&producer_id, NULL, SingleDishMSFiller<T>::produce,
        filler);

    sdfiller::join_thread(&consumer_id, NULL);
    sdfiller::join_thread(&producer_id, NULL);
  } catch (...) {
    SingleDishMSFiller<T>::destroy_context();
    throw;
  }

  SingleDishMSFiller<T>::destroy_context();

  POST_END;
}

template<class T>
SingleDishMSFiller<T>::SingleDishMSFiller(std::string const &name,
    bool parallel) :
    ms_(), ms_columns_(), data_description_columns_(), feed_columns_(),
    pointing_columns_(), polarization_columns_(), syscal_columns_(),
    state_columns_(), weather_columns_(), reader_(new T(name)),
    is_float_(false), data_key_(), reference_feed_(-1), pointing_time_(),
    pointing_time_max_(), pointing_time_min_(), num_pointing_time_(),
    syscal_list_(), subscan_list_(), polarization_type_pool_(), weather_list_(),
    parallel_(parallel) {
}

template<class T>
SingleDishMSFiller<T>::~SingleDishMSFiller() {
}

template<class T>
void SingleDishMSFiller<T>::fill() {
  POST_START;

  // initialization
  initialize();

  // Fill tables that can be processed prior to main loop
  fillPreProcessTables();

  // main loop
  casacore::LogIO os(casacore::LogOrigin("SingleDishMSFiller", "fill", WHERE));
  if (parallel_) {
    os << "Parallel execution of fillMain" << casacore::LogIO::POST;
    SingleDishMSFiller<T>::fillMainMT(this);
  } else {
    os << "Serial execution of fillMain" << casacore::LogIO::POST;
    fillMain();
  }

  // Fill tables that must be processed after main loop
  fillPostProcessTables();

  // finalization
  finalize();

  POST_END;
}

template<class T>
void SingleDishMSFiller<T>::initialize() {
  POST_START;

  // initialize reader
  reader_->initialize();

  // query if the data is complex or float
  is_float_ = reader_->isFloatData();
  if (is_float_) {
//      std::cout << "data column is FLOAT_DATA" << std::endl;
    data_key_ = "FLOAT_DATA";
  } else {
//      std::cout << "data column is DATA" << std::endl;
    data_key_ = "DATA";
  }

  // setup MS
  setupMS();

  // frame information
  casacore::MDirection::Types direction_frame = reader_->getDirectionFrame();
  auto mytable = ms_->pointing();
  casacore::ArrayColumn<casacore::Double> direction_column(mytable, "DIRECTION");
  casacore::TableRecord &record = direction_column.rwKeywordSet();
  casacore::Record meas_info = record.asRecord("MEASINFO");
  casacore::String ref_string = casacore::MDirection::showType(direction_frame);
  meas_info.define("Ref", ref_string);
  record.defineRecord("MEASINFO", meas_info);

  POST_END;
}

template<class T>
void SingleDishMSFiller<T>::finalize() {
  POST_START;

  // finalize reader
  reader_->finalize();

  POST_END;
}

template<class T>
void SingleDishMSFiller<T>::save(std::string const &name) {
  POST_START;

#ifdef SINGLEDISHMSFILLER_DEBUG
  std::cout << "Saving MS as \"" << name << "\"" << std::endl;
  std::cout << "current working directory is \"" << casacore::Path().absoluteName()
  << "\"" << std::endl;
#endif

  ms_->deepCopy(name, casacore::Table::New);

#ifdef SINGLEDISHMSFILLER_DEBUG
  casacore::File file(name);
  casacore::Bool name_exists = file.exists();
  if (name_exists) {
    std::cout << "file successfully created" << std::endl;
  } else {
    std::cout << "failed to create file" << std::endl;
  }
#endif

  POST_END;
}

template<class T>
void SingleDishMSFiller<T>::setupMS() {
//  std::cout << "Start " << __PRETTY_FUNCTION__ << std::endl;

  casacore::String const dunit = reader_->getDataUnit();

  casacore::TableDesc ms_main_description = casacore::MeasurementSet::requiredTableDesc();
  if (is_float_) {
    casacore::MeasurementSet::addColumnToDesc(ms_main_description,
        casacore::MSMainEnums::FLOAT_DATA, 2);
    ms_main_description.rwColumnDesc(MS::columnName(MS::FLOAT_DATA)).rwKeywordSet().define("UNIT", dunit);
  } else {
    casacore::MeasurementSet::addColumnToDesc(ms_main_description, casacore::MSMainEnums::DATA, 2);
    ms_main_description.rwColumnDesc(MS::columnName(MS::DATA)).rwKeywordSet().define("UNIT", dunit);
  }

  casacore::String const scratch_table_name = casacore::File::newUniqueName(".",
      "SingleDishMSFillerTemp").originalName();
  casacore::SetupNewTable newtab(scratch_table_name, ms_main_description, casacore::Table::Scratch);

  ms_.reset(new casacore::MeasurementSet(newtab));

  // create subtables
  casacore::TableDesc ms_antenna_description = casacore::MSAntenna::requiredTableDesc();
  casacore::SetupNewTable ms_antenna_table(ms_->antennaTableName(),
      ms_antenna_description, casacore::Table::Scratch);
  ms_->rwKeywordSet().defineTable(
      casacore::MeasurementSet::keywordName(casacore::MeasurementSet::ANTENNA),
      casacore::Table(ms_antenna_table));

  casacore::TableDesc ms_data_desc_description = casacore::MSDataDescription::requiredTableDesc();
  casacore::SetupNewTable ms_data_desc_table(ms_->dataDescriptionTableName(),
      ms_data_desc_description, casacore::Table::Scratch);
  ms_->rwKeywordSet().defineTable(
      casacore::MeasurementSet::keywordName(casacore::MeasurementSet::DATA_DESCRIPTION),
      casacore::Table(ms_data_desc_table));

  casacore::TableDesc ms_doppler_description = casacore::MSDoppler::requiredTableDesc();
  casacore::SetupNewTable ms_doppler_table(ms_->dopplerTableName(),
      ms_doppler_description, casacore::Table::Scratch);
  ms_->rwKeywordSet().defineTable(
      casacore::MeasurementSet::keywordName(casacore::MeasurementSet::DOPPLER),
      casacore::Table(ms_doppler_table));

  casacore::TableDesc ms_feed_description = casacore::MSFeed::requiredTableDesc();
  casacore::SetupNewTable ms_feed_table(ms_->feedTableName(), ms_feed_description,
      casacore::Table::Scratch);
  ms_->rwKeywordSet().defineTable(
      casacore::MeasurementSet::keywordName(casacore::MeasurementSet::FEED), casacore::Table(ms_feed_table));

  casacore::TableDesc ms_field_description = casacore::MSField::requiredTableDesc();
  casacore::SetupNewTable ms_field_table(ms_->fieldTableName(), ms_field_description,
      casacore::Table::Scratch);
  ms_->rwKeywordSet().defineTable(
      casacore::MeasurementSet::keywordName(casacore::MeasurementSet::FIELD),
      casacore::Table(ms_field_table));

  casacore::TableDesc ms_flag_cmd_description = casacore::MSFlagCmd::requiredTableDesc();
  casacore::SetupNewTable ms_flag_cmd_table(ms_->flagCmdTableName(),
      ms_flag_cmd_description, casacore::Table::Scratch);
  ms_->rwKeywordSet().defineTable(
      casacore::MeasurementSet::keywordName(casacore::MeasurementSet::FLAG_CMD),
      casacore::Table(ms_flag_cmd_table));

  casacore::TableDesc ms_freq_offset_description = casacore::MSFreqOffset::requiredTableDesc();
  casacore::SetupNewTable ms_freq_offset_table(ms_->freqOffsetTableName(),
      ms_freq_offset_description, casacore::Table::Scratch);
  ms_->rwKeywordSet().defineTable(
      casacore::MeasurementSet::keywordName(casacore::MeasurementSet::FREQ_OFFSET),
      casacore::Table(ms_freq_offset_table));

  casacore::TableDesc ms_history_description = casacore::MSHistory::requiredTableDesc();
  casacore::SetupNewTable ms_history_table(ms_->historyTableName(),
      ms_history_description, casacore::Table::Scratch);
  ms_->rwKeywordSet().defineTable(
      casacore::MeasurementSet::keywordName(casacore::MeasurementSet::HISTORY),
      casacore::Table(ms_history_table));

  casacore::TableDesc ms_observation_description = casacore::MSObservation::requiredTableDesc();
  casacore::SetupNewTable ms_observation_table(ms_->observationTableName(),
      ms_observation_description, casacore::Table::Scratch);
  ms_->rwKeywordSet().defineTable(
      casacore::MeasurementSet::keywordName(casacore::MeasurementSet::OBSERVATION),
      casacore::Table(ms_observation_table));

  casacore::TableDesc ms_pointing_description = casacore::MSPointing::requiredTableDesc();
  casacore::SetupNewTable ms_pointing_table(ms_->pointingTableName(),
      ms_pointing_description, casacore::Table::Scratch);
  ms_->rwKeywordSet().defineTable(
      casacore::MeasurementSet::keywordName(casacore::MeasurementSet::POINTING),
      casacore::Table(ms_pointing_table));

  casacore::TableDesc ms_polarization_description = casacore::MSPolarization::requiredTableDesc();
  casacore::SetupNewTable ms_polarization_table(ms_->polarizationTableName(),
      ms_polarization_description, casacore::Table::Scratch);
  ms_->rwKeywordSet().defineTable(
      casacore::MeasurementSet::keywordName(casacore::MeasurementSet::POLARIZATION),
      casacore::Table(ms_polarization_table));

  casacore::TableDesc ms_processor_description = casacore::MSProcessor::requiredTableDesc();
  casacore::SetupNewTable ms_processor_table(ms_->processorTableName(),
      ms_processor_description, casacore::Table::Scratch);
  ms_->rwKeywordSet().defineTable(
      casacore::MeasurementSet::keywordName(casacore::MeasurementSet::PROCESSOR),
      casacore::Table(ms_processor_table));

  casacore::TableDesc ms_source_description = casacore::MSSource::requiredTableDesc();
  casacore::MSSource::addColumnToDesc(ms_source_description, casacore::MSSourceEnums::TRANSITION,
      1);
  casacore::MSSource::addColumnToDesc(ms_source_description,
      casacore::MSSourceEnums::REST_FREQUENCY, 1);
  casacore::MSSource::addColumnToDesc(ms_source_description, casacore::MSSourceEnums::SYSVEL, 1);
  casacore::SetupNewTable ms_source_table(ms_->sourceTableName(), ms_source_description,
      casacore::Table::Scratch);
  ms_->rwKeywordSet().defineTable(
      casacore::MeasurementSet::keywordName(casacore::MeasurementSet::SOURCE),
      casacore::Table(ms_source_table));

  casacore::TableDesc ms_spectral_window_description =
      casacore::MSSpectralWindow::requiredTableDesc();
  casacore::SetupNewTable ms_spectral_window_table(ms_->spectralWindowTableName(),
      ms_spectral_window_description, casacore::Table::Scratch);
  ms_->rwKeywordSet().defineTable(
      casacore::MeasurementSet::keywordName(casacore::MeasurementSet::SPECTRAL_WINDOW),
      casacore::Table(ms_spectral_window_table));

  casacore::TableDesc ms_state_description = casacore::MSState::requiredTableDesc();
  casacore::SetupNewTable ms_state_table(ms_->stateTableName(), ms_state_description,
      casacore::Table::Scratch);
  ms_->rwKeywordSet().defineTable(
      casacore::MeasurementSet::keywordName(casacore::MeasurementSet::STATE),
      casacore::Table(ms_state_table));

  casacore::TableDesc ms_syscal_description = casacore::MSSysCal::requiredTableDesc();
  casacore::MSSysCal::addColumnToDesc(ms_syscal_description, casacore::MSSysCalEnums::TCAL_SPECTRUM,
      2);
  casacore::MSSysCal::addColumnToDesc(ms_syscal_description, casacore::MSSysCalEnums::TCAL, 1);
  casacore::MSSysCal::addColumnToDesc(ms_syscal_description, casacore::MSSysCalEnums::TSYS_SPECTRUM,
      2);
  casacore::MSSysCal::addColumnToDesc(ms_syscal_description, casacore::MSSysCalEnums::TSYS, 1);
  casacore::SetupNewTable ms_syscal_table(ms_->sysCalTableName(), ms_syscal_description,
      casacore::Table::Scratch);
  ms_->rwKeywordSet().defineTable(
      casacore::MeasurementSet::keywordName(casacore::MeasurementSet::SYSCAL),
      casacore::Table(ms_syscal_table));

  casacore::TableDesc ms_weather_description = casacore::MSWeather::requiredTableDesc();
  casacore::MSWeather::addColumnToDesc(ms_weather_description,
      casacore::MSWeatherEnums::TEMPERATURE);
  casacore::MSWeather::addColumnToDesc(ms_weather_description, casacore::MSWeatherEnums::PRESSURE);
  casacore::MSWeather::addColumnToDesc(ms_weather_description,
      casacore::MSWeatherEnums::REL_HUMIDITY);
  casacore::MSWeather::addColumnToDesc(ms_weather_description,
      casacore::MSWeatherEnums::WIND_SPEED);
  casacore::MSWeather::addColumnToDesc(ms_weather_description,
      casacore::MSWeatherEnums::WIND_DIRECTION);
  casacore::SetupNewTable ms_weather_table(ms_->weatherTableName(),
      ms_weather_description, casacore::Table::Scratch);
  ms_->rwKeywordSet().defineTable(
      casacore::MeasurementSet::keywordName(casacore::MeasurementSet::WEATHER),
      casacore::Table(ms_weather_table));

  ms_->initRefs();

  // Set up MSMainColumns
  ms_columns_.reset(new casacore::MSMainColumns(*ms_));

  // Set up MSDataDescColumns
  data_description_columns_.reset(
      new casacore::MSDataDescColumns(ms_->dataDescription()));

  // Set up MSFeedColumns
  feed_columns_.reset(new casacore::MSFeedColumns(ms_->feed()));

  // Set up MSPointingColumns
  pointing_columns_.reset(new casacore::MSPointingColumns(ms_->pointing()));

  // Set up MSPolarizationColumns
  polarization_columns_.reset(new casacore::MSPolarizationColumns(ms_->polarization()));

  // Set up MSSysCalColumns
  syscal_columns_.reset(new casacore::MSSysCalColumns(ms_->sysCal()));

  // Set up MSStateColumns
  state_columns_.reset(new casacore::MSStateColumns(ms_->state()));

  // Set up MSWeatherColumns
  weather_columns_.reset(new casacore::MSWeatherColumns(ms_->weather()));

//  std::cout << "End " << __PRETTY_FUNCTION__ << std::endl;
}

template<class T>
void SingleDishMSFiller<T>::fillPreProcessTables() {
  POST_START;

  // fill OBSERVATION table
  fillObservation();

  // fill ANTENNA table
  fillAntenna();

  // fill PROCESSOR table
  fillProcessor();

  // fill SOURCE table
  fillSource();

  // fill FIELD table
  fillField();

  // fill SPECTRAL_WINDOW table
  fillSpectralWindow();

  // Generate optional tables if necessary
  OptionalTables::Generate(*ms_, *reader_);

  POST_END;
}

template<class T>
void SingleDishMSFiller<T>::fillPostProcessTables() {
  POST_START;

  // fill HISTORY table
  fillHistory();

  // flush POINTING entry
  sortPointing();

  POST_END;
}

template<class T>
void SingleDishMSFiller<T>::fillMain() {
  POST_START;

  size_t nrow = reader_->getNumberOfRows();
  DataAccumulator accumulator;
  DataRecord record;
//    std::cout << "nrow = " << nrow << std::endl;
  for (size_t irow = 0; irow < nrow; ++irow) {
    casacore::Bool status = reader_->getData(irow, record);
//      std::cout << "irow " << irow << " status " << status << std::endl;
//      std::cout << "   TIME=" << record.time << " INTERVAL=" << record.interval
//          << std::endl;
//      std::cout << "status = " << status << std::endl;
    if (status) {
      casacore::Bool is_ready = accumulator.queryForGet(record.time);
      if (is_ready) {
        flush(accumulator);
      }
      casacore::Bool astatus = accumulator.accumulate(record);
      (void) astatus;
//        std::cout << "astatus = " << astatus << std::endl;
    }
  }

  flush(accumulator);

  POST_END;
}

template<class T>
void SingleDishMSFiller<T>::fillAntenna() {
  POST_START;

  auto mytable = ms_->antenna();
  AntennaRecord record;
  ::fillTable(mytable, record,
      [&](AntennaRecord &record) {return reader_->getAntennaRow(record);});

  // initialize POINTING table related stuff
  casacore::uInt nrow = mytable.nrow();
  num_pointing_time_.resize(nrow);
  for (casacore::uInt i = 0; i < nrow; ++i) {
    pointing_time_[i] = casacore::Vector<casacore::Double>(ARRAY_BLOCK_SIZE, -1.0);
    pointing_time_min_[i] = -1.0;
    pointing_time_max_[i] = 1.0e30;
    num_pointing_time_[i] = 0;
  }

  POST_END;
}

template<class T>
void SingleDishMSFiller<T>::fillObservation() {
  POST_START;

  ObservationRecord record;
  ::fillTable(ms_->observation(), record,
      [&](ObservationRecord &record) {return reader_->getObservationRow(record);});

  POST_END;
}

template<class T>
void SingleDishMSFiller<T>::fillProcessor() {
  POST_START;

  ProcessorRecord record;
  ::fillTable(ms_->processor(), record,
      [&](ProcessorRecord &record) {return reader_->getProcessorRow(record);});

  POST_END;
}

template<class T>
void SingleDishMSFiller<T>::fillSource() {
  POST_START;

  SourceRecord record;
  ::fillTable(ms_->source(), record,
      [&](SourceRecord &record) {return reader_->getSourceRow(record);});

  POST_END;
}

template<class T>
void SingleDishMSFiller<T>::fillField() {
  POST_START;

  auto mytable = ms_->field();
  FieldRecord record;
  record.table = mytable;

  // make (name,id) map for SOURCE table
  ::makeSourceMap(ms_->source(), record.source_map);

  ::fillTable(mytable, record,
      [&](FieldRecord &record) {return reader_->getFieldRow(record);});

  POST_END;
}

template<class T>
void SingleDishMSFiller<T>::fillHistory() {
  POST_START;

  // HISTORY table should be filled by upper-level
  // application command (e.g. importscantable)
//    ms_->history().addRow(1, true);
//    casacore::Vector<casacore::String> cols(2);
//    cols[0] = "APP_PARAMS";
//    cols[1] = "CLI_COMMAND";
//    casacore::TableRow row(ms_->history(), cols, true);
//    // TODO: fill HISTORY row here
//    casacore::TableRecord record = row.record();
//    record.print(std::cout);
//    row.put(0, record);

  POST_END;
}

template<class T>
void SingleDishMSFiller<T>::fillSpectralWindow() {
  POST_START;

  auto mytable = ms_->spectralWindow();
  SpectralWindowRecord record;

  ::fillTable(mytable, record,
      [&](SpectralWindowRecord &record) {return reader_->getSpectralWindowRow(record);});

  POST_END;
}

template<class T>
casacore::Int SingleDishMSFiller<T>::updatePolarization(casacore::Vector<casacore::Int> const &corr_type,
    casacore::Int const &num_pol) {
  casacore::uInt num_corr = corr_type.size();
  if (num_pol < 1 || num_corr != (uInt) num_pol) {
    throw casacore::AipsError("Internal inconsistency in number of correlations");
  }
  casacore::MSPolarization &mytable = ms_->polarization();
  //casacore::MSPolarizationColumns mycolumns(mytable);
  casacore::Matrix<casacore::Int> const corr_product(2, num_pol, 0);
  auto comparer = [&](casacore::MSPolarizationColumns const &columns, casacore::uInt i) {
    casacore::Bool match = allEQ(columns.corrType()(i), corr_type);
    return match;
  };
  auto updater = [&](casacore::MSPolarizationColumns &columns, casacore::uInt i) {
    columns.numCorr().put(i, num_pol);
    columns.corrType().put(i, corr_type);
    columns.corrProduct().put(i, corr_product);
  };
  casacore::Int polarization_id = ::updateTable(mytable, *(polarization_columns_.get()),
      comparer, updater);
  return polarization_id;
}

template<class T>
Int SingleDishMSFiller<T>::updateDataDescription(casacore::Int const &polarization_id,
    casacore::Int const &spw_id) {
  if (polarization_id < 0 || spw_id < 0) {
    throw casacore::AipsError("Invalid ids for DATA_DESCRIPTION");
  }
  casacore::MSDataDescription &mytable = ms_->dataDescription();
  //casacore::MSDataDescColumns mycolumns(mytable);
  auto comparer = [&](casacore::MSDataDescColumns const &columns, casacore::uInt i) {
    casacore::Bool match = (columns.polarizationId()(i) == polarization_id)
    && (columns.spectralWindowId()(i) == spw_id);
    return match;
  };
  auto updater = [&](casacore::MSDataDescColumns &columns, casacore::uInt i) {
    columns.polarizationId().put(i, polarization_id);
    columns.spectralWindowId().put(i, spw_id);
  };
  casacore::Int data_desc_id = ::updateTable(mytable, *(data_description_columns_.get()),
      comparer, updater);

  return data_desc_id;
}

template<class T>
Int SingleDishMSFiller<T>::updateState(casacore::Int const &subscan,
    casacore::String const &obs_mode) {
  casacore::MSState &mytable = ms_->state();
  static casacore::Regex const regex("^OBSERVE_TARGET#ON_SOURCE");
  //static std::vector<casacore::Int> subscan_list;
  auto comparer =
      [&](casacore::MSStateColumns &columns, casacore::uInt i) {
        casacore::Bool match = (subscan == subscan_list_[i]) && (obs_mode == columns.obsMode()(i));
        return match;
      };
  auto updater = [&](casacore::MSStateColumns &columns, casacore::uInt i) {
    columns.subScan().put(i, subscan);
    columns.obsMode().put(i, obs_mode);
    casacore::Bool is_signal = obs_mode.matches(regex);
    columns.sig().put(i, is_signal);
    columns.ref().put(i, !is_signal);

    subscan_list_.push_back(subscan);
  };
  casacore::Int state_id = ::updateTable(mytable, *(state_columns_.get()), comparer,
      updater);
  return state_id;
}

template<class T>
Int SingleDishMSFiller<T>::updateFeed(casacore::Int const &feed_id, casacore::Int const &spw_id,
    casacore::String const &pol_type) {
  constexpr casacore::Int num_receptors = 2;
  static casacore::String const linear_type_arr[2] = { "X", "Y" };
  static casacore::Vector<casacore::String> linear_type(linear_type_arr, 2, SHARE);
  static casacore::String const circular_type_arr[2] = { "R", "L" };
  static casacore::Vector<casacore::String> circular_type(circular_type_arr, 2, SHARE);
  static casacore::Matrix<casacore::Complex> const pol_response(num_receptors, num_receptors,
      casacore::Complex(0));
  casacore::Vector<casacore::String> *polarization_type = nullptr;
  if (pol_type == "linear") {
    polarization_type = &linear_type;
  } else if (pol_type == "circular") {
    polarization_type = &circular_type;
  } else {
    polarization_type = &linear_type;
  }
  //static std::vector< casacore::Vector<casacore::String> *> polarization_type_pool;

  casacore::String polarization_type_arr[2] = { "X", "Y" };
  casacore::Vector<casacore::String> polarization_type_storage(polarization_type_arr, 2, SHARE);
  casacore::Matrix<casacore::Double> const beam_offset(2, num_receptors, 0.0);
  casacore::Vector<casacore::Double> const receptor_angle(num_receptors, 0.0);
  casacore::Vector<casacore::Double> const position(3, 0.0);
  auto comparer = [&](casacore::MSFeedColumns &columns, casacore::uInt i) {
    casacore::Vector<casacore::String> *current_polarization_type = polarization_type_pool_[i];
    casacore::Bool match = allEQ(*polarization_type, *current_polarization_type) &&
    (feed_id == columns.feedId()(i)) &&
    (spw_id == columns.spectralWindowId()(i));
    return match;
  };
  auto updater = [&](casacore::MSFeedColumns &columns, casacore::uInt i) {
    // TODO: 2016/01/26 TN
    // Here I regard "multi-beam receiver" as multi-feed, single-beam receivers
    // in casacore::MS v2 notation. So, BEAM_ID is always 0 and FEED_ID is beam identifier
    // for "multi-beam" receivers so far. However, if we decide to import beams
    // as separated virtual antennas, FEED_ID must always be 0.
      columns.feedId().put(i, feed_id);
      columns.beamId().put(i, 0);
      columns.spectralWindowId().put(i, spw_id);
      columns.polarizationType().put(i, *polarization_type);
      columns.beamOffset().put(i, beam_offset);
      columns.receptorAngle().put(i, receptor_angle);
      columns.position().put(i, position);
      columns.polResponse().put(i, pol_response);

      polarization_type_pool_.push_back(polarization_type);
    };
  casacore::Int feed_row = ::updateTable(ms_->feed(), *(feed_columns_.get()), comparer,
      updater);

  // reference feed
  if (reference_feed_ < 0) {
    reference_feed_ = feed_id;
  }

  return feed_row;
}

template<class T>
Int SingleDishMSFiller<T>::updatePointing(casacore::Int const &antenna_id,
    casacore::Int const &feed_id, casacore::Double const &time, casacore::Double const &interval,
    casacore::Matrix<casacore::Double> const &direction) {
  POST_START;

  if (reference_feed_ != feed_id) {
    return -1;
  }

  auto &mytable = ms_->pointing();
  casacore::uInt nrow = mytable.nrow();

  casacore::uInt *n = &num_pointing_time_[antenna_id];
  casacore::Double *time_max = &pointing_time_max_[antenna_id];
  casacore::Double *time_min = &pointing_time_min_[antenna_id];
  casacore::Vector<casacore::Double> *time_list = &pointing_time_[antenna_id];

  auto addPointingRow =
      [&]() {
        mytable.addRow(1, true);
        pointing_columns_->time().put(nrow, time);
        pointing_columns_->interval().put(nrow, interval);
        pointing_columns_->antennaId().put(nrow, antenna_id);
        if (direction.ncolumn() == 1 || allEQ(direction.column(1), 0.0)) {
          pointing_columns_->numPoly().put(nrow, 0);
          pointing_columns_->direction().put(nrow, direction(casacore::IPosition(2,0,0), casacore::IPosition(2,1,0)));
        } else {
          pointing_columns_->direction().put(nrow, direction);
          casacore::Int num_poly = direction.shape()[1] - 1;
          pointing_columns_->numPoly().put(nrow, num_poly);
        }
        // add timestamp to the list
        casacore::uInt nelem = time_list->nelements();
        if (nelem <= *n) {
          time_list->resize(nelem + ARRAY_BLOCK_SIZE, true);
        }
        (*time_list)[*n] = time;
        // increment number of pointing entry
        *n += 1;
      };

  if (*n == 0) {
    addPointingRow();
    *time_min = time;
    *time_max = time;
  } else if (time < *time_min) {
    addPointingRow();
    *time_min = time;
  } else if (*time_max < time) {
    addPointingRow();
    *time_max = time;
  } else if (allNE(*time_list, time)) {
    addPointingRow();
  }

  POST_END;

  return -1;
}

template<class T>
void SingleDishMSFiller<T>::updateWeather(casacore::Int const &antenna_id,
    casacore::Double const &time, casacore::Double const &interval,
    MSDataRecord const &data_record) {
  WeatherRecord record;
  record.clear();
  record.antenna_id = antenna_id;
  record.time = time;
  record.interval = interval;
  record.temperature = data_record.temperature;
  record.pressure = data_record.pressure;
  record.rel_humidity = data_record.rel_humidity;
  record.wind_speed = data_record.wind_speed;
  record.wind_direction = data_record.wind_direction;
  auto &mytable = ms_->weather();
  auto pos = std::find(weather_list_.begin(), weather_list_.end(), record);
  if (pos == weather_list_.end()) {
    weather_list_.push_back(record);
    casacore::uInt irow = mytable.nrow();
    mytable.addRow(1, true);
    record.fill(irow, *(weather_columns_.get()));
  } else {
    auto irow = std::distance(weather_list_.begin(), pos);
    updateWeather(*(weather_columns_.get()), irow, record);
  }
}

template<class T>
void SingleDishMSFiller<T>::updateWeather(casacore::MSWeatherColumns &columns, casacore::uInt irow,
    WeatherRecord const &record) {
  ::updateSubtable(columns, irow, record);
}

template<class T>
void SingleDishMSFiller<T>::updateSysCal(casacore::Int const &antenna_id,
    casacore::Int const &feed_id, casacore::Int const &spw_id, casacore::Double const &time,
    casacore::Double const &interval, MSDataRecord const &data_record) {
  POST_START;

  SysCalRecord record;
  record.clear();
  record.antenna_id = antenna_id;
  record.feed_id = feed_id;
  record.spw_id = spw_id;
  record.time = time;
  record.interval = interval;

  //casacore::Bool tcal_empty = false;
  casacore::Bool tsys_empty = false;

  if (data_record.tcal.empty() || allEQ(data_record.tcal, 1.0f)
      || allEQ(data_record.tcal, 0.0f)) {
    //tcal_empty = true;
  } else {
//      std::cout << "tcal seems to be valid " << data_record.tcal << std::endl;
    if (data_record.float_data.shape() == data_record.tcal.shape()) {
      record.tcal_spectrum.assign(data_record.tcal);
    } else {
      casacore::Matrix<casacore::Float> tcal = data_record.tcal;
      if (!tcal.empty()) {
        record.tcal.assign(tcal.column(0));
      }
    }
  }
  if (data_record.tsys.empty() || allEQ(data_record.tsys, 1.0f)
      || allEQ(data_record.tsys, 0.0f)) {
    tsys_empty = true;
  } else {
    if (data_record.float_data.shape() == data_record.tsys.shape()) {
      record.tsys_spectrum.assign(data_record.tsys);
    } else {
      casacore::Matrix<casacore::Float> tsys = data_record.tsys;
      if (!tsys.empty()) {
        record.tsys.assign(tsys.column(0));
      }
    }
  }

  // do not add entry if Tsys is empty
  //if (tcal_empty && tsys_empty) {
  if (tsys_empty) {
    return;
  }

  auto &mytable = ms_->sysCal();
  auto pos = std::find(syscal_list_.begin(), syscal_list_.end(), record);
  if (pos == syscal_list_.end()) {
    casacore::uInt irow = mytable.nrow();
    mytable.addRow(1, true);
    record.fill(irow, *(syscal_columns_.get()));
    syscal_list_.push_back(SysCalTableRecord(ms_.get(), irow, record));
  } else {
    auto irow = std::distance(syscal_list_.begin(), pos);
    updateSysCal(*(syscal_columns_.get()), irow, record);
  }

  POST_END;
}

template<class T>
void SingleDishMSFiller<T>::updateSysCal(casacore::MSSysCalColumns &columns, casacore::uInt irow,
    SysCalRecord const &record) {
  ::updateSubtable(columns, irow, record);
}

template<class T>
void SingleDishMSFiller<T>::updateMain(casacore::Int const &antenna_id, casacore::Int field_id,
    casacore::Int feedId, casacore::Int dataDescriptionId, casacore::Int stateId, casacore::Int const &scan_number,
    casacore::Double const &time, MSDataRecord const &dataRecord) {
  POST_START;

  // constant stuff
  static casacore::Vector<casacore::Double> const uvw(3, 0.0);
  static casacore::Array<casacore::Bool> const flagCategory(casacore::IPosition(3, 0, 0, 0));

  // target row id
  casacore::uInt irow = ms_->nrow();

  // add new row
  //ms_->addRow(1, true);
  ms_->addRow(1, false);

  ms_columns_->uvw().put(irow, uvw);
  ms_columns_->flagCategory().put(irow, flagCategory);
  ms_columns_->antenna1().put(irow, antenna_id);
  ms_columns_->antenna2().put(irow, antenna_id);
  ms_columns_->fieldId().put(irow, field_id);
  ms_columns_->feed1().put(irow, feedId);
  ms_columns_->feed2().put(irow, feedId);
  ms_columns_->dataDescId().put(irow, dataDescriptionId);
  ms_columns_->stateId().put(irow, stateId);
  ms_columns_->scanNumber().put(irow, scan_number);
  ms_columns_->time().put(irow, time);
  ms_columns_->timeCentroid().put(irow, time);
  casacore::Double const &interval = dataRecord.interval;
  ms_columns_->interval().put(irow, interval);
  ms_columns_->exposure().put(irow, interval);

  if (is_float_) {
    casacore::Matrix<casacore::Float> floatData;
    if (dataRecord.isFloat()) {
      floatData.reference(dataRecord.float_data);
    } else {
      floatData.assign(real(dataRecord.complex_data));
    }
    ms_columns_->floatData().put(irow, floatData);
  } else {
    casacore::Matrix<casacore::Complex> data;
    if (dataRecord.isFloat()) {
      data.assign(
          makeComplex(dataRecord.float_data,
              casacore::Matrix<casacore::Float>(dataRecord.float_data.shape(), 0.0f)));
    } else {
      data.reference(dataRecord.complex_data);
    }
    ms_columns_->data().put(irow, data);
  }

  ms_columns_->flag().put(irow, dataRecord.flag);
  ms_columns_->flagRow().put(irow, dataRecord.flag_row);
  ms_columns_->sigma().put(irow, dataRecord.sigma);
  ms_columns_->weight().put(irow, dataRecord.weight);

  POST_END;
}

template<class T>
void SingleDishMSFiller<T>::flush(DataAccumulator &accumulator) {
  POST_START;

  size_t nchunk = accumulator.getNumberOfChunks();
//    std::cout << "nchunk = " << nchunk << std::endl;

  if (nchunk == 0) {
    return;
  }

  for (size_t ichunk = 0; ichunk < nchunk; ++ichunk) {
    casacore::Bool status = accumulator.get(ichunk, record_);
//      std::cout << "accumulator status = " << std::endl;
    if (status) {
      casacore::Double time = record_.time;
      casacore::Int antenna_id = record_.antenna_id;
      casacore::Int spw_id = record_.spw_id;
      casacore::Int feed_id = record_.feed_id;
      casacore::Int field_id = record_.field_id;
      casacore::Int scan = record_.scan;
      casacore::Int subscan = record_.subscan;
      casacore::String pol_type = record_.pol_type;
      casacore::String obs_mode = record_.intent;
      casacore::Int num_pol = record_.num_pol;
      casacore::Vector<casacore::Int> &corr_type = record_.corr_type;
      casacore::Int polarization_id = updatePolarization(corr_type, num_pol);
      updateFeed(feed_id, spw_id, pol_type);
      casacore::Int data_desc_id = updateDataDescription(polarization_id, spw_id);
      casacore::Int state_id = updateState(subscan, obs_mode);
      casacore::Matrix<casacore::Double> &direction = record_.direction;
      casacore::Double interval = record_.interval;

      // updatePointing must be called after updateFeed
      updatePointing(antenna_id, feed_id, time, interval, direction);

      updateSysCal(antenna_id, feed_id, spw_id, time, interval, record_);

      updateWeather(antenna_id, time, interval, record_);

      updateMain(antenna_id, field_id, feed_id, data_desc_id, state_id, scan,
          time, record_);
    }
  }
//    std::cout << "clear accumulator" << std::endl;
  accumulator.clear();

  POST_END;
}

template<class T>
void SingleDishMSFiller<T>::sortPointing() {
  POST_START;

  // deallocate POINTING table related stuff
  pointing_time_.clear();
  pointing_time_min_.clear();
  pointing_time_max_.clear();
  num_pointing_time_.resize();

  auto mytable = ms_->pointing();
  casacore::MSPointingColumns mycolumns(mytable);
  casacore::uInt nrow = mytable.nrow();
  casacore::Vector<casacore::Int> antenna_id_list = mycolumns.antennaId().getColumn();
  casacore::Vector<casacore::Double> time_list = mycolumns.time().getColumn();
  casacore::Sort sorter;
  sorter.sortKey(antenna_id_list.data(), TpInt);
  sorter.sortKey(time_list.data(), TpDouble);
  casacore::Vector<casacore::uInt> index_vector;
  sorter.sort(index_vector, nrow);

  size_t storage_size = nrow * 2 * sizeof(casacore::Double);
  std::unique_ptr<void, sdfiller::Deleter> storage(malloc(storage_size));

  // sort TIME
  {
    casacore::Vector<casacore::Double> sorted(casacore::IPosition(1, nrow),
        reinterpret_cast<casacore::Double *>(storage.get()), SHARE);
    for (casacore::uInt i = 0; i < nrow; ++i) {
      sorted[i] = time_list[index_vector[i]];
    }
    mycolumns.time().putColumn(sorted);
  }
  // sort ANTENNA_ID
  {
    casacore::Vector<casacore::Int> sorted(casacore::IPosition(1, nrow),
        reinterpret_cast<casacore::Int *>(storage.get()), SHARE);
    for (casacore::uInt i = 0; i < nrow; ++i) {
      sorted[i] = antenna_id_list[index_vector[i]];
    }
    mycolumns.antennaId().putColumn(sorted);
  }

  // sort NUM_POLY
  {
    casacore::Vector<casacore::Int> num_poly_list(antenna_id_list);
    mycolumns.numPoly().getColumn(num_poly_list);
    casacore::Vector<casacore::Int> sorted(casacore::IPosition(1, nrow),
        reinterpret_cast<casacore::Int *>(storage.get()), SHARE);
    for (casacore::uInt i = 0; i < nrow; ++i) {
      sorted[i] = antenna_id_list[index_vector[i]];
    }
    mycolumns.numPoly().putColumn(sorted);
  }

  // sort DIRECTION
  {
    std::map<casacore::uInt, casacore::Matrix<casacore::Double> > direction;
    for (casacore::uInt i = 0; i < nrow; ++i) {
      direction[i] = mycolumns.direction()(i);
    }
    for (casacore::uInt i = 0; i < nrow; ++i) {
      mycolumns.direction().put(i, direction[index_vector[i]]);
    }
  }

  // sort INTERVAL
  {
    casacore::Vector<casacore::Double> interval_list = mycolumns.interval().getColumn();
    for (casacore::uInt i = 0; i < nrow; ++i) {
      mycolumns.interval().put(i, interval_list[index_vector[i]]);
    }
  }
  POST_END;
}

} //# NAMESPACE CASA - END

#endif /* SINGLEDISH_FILLER_SINGLEDISHMSFILLER_TCC_ */
