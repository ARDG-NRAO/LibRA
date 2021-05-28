/*
 * Scantable2MSReader.cc
 *
 *  Created on: Jan 5, 2016
 *      Author: nakazato
 */

#include <singledishfiller/Filler/Scantable2MSReader.h>

#include <iostream>
#include <string>

#include <casacore/casa/OS/File.h>
#include <casacore/casa/Containers/Record.h>
#include <casacore/casa/Utilities/Regex.h>
#include <casacore/tables/Tables/TableRecord.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/IO/ArrayIO.h>
#include <casacore/tables/Tables/Table.h>
#include <casacore/measures/Measures/Stokes.h>

using namespace casacore;

namespace {
Double queryAntennaDiameter(String const &name) {
  String capitalized = name;
  capitalized.upcase();
  Double diameter = 0.0;
  if (capitalized.matches(Regex(".*(DV|DA|PM)[0-9]+$"))) {
    diameter = 12.0;
  } else if (capitalized.matches(Regex(".*CM[0-9]+$"))) {
    diameter = 7.0;
  } else if (capitalized.contains("GBT")) {
    diameter = 104.9;
  } else if (capitalized.contains("MOPRA")) {
    diameter = 22.0;
  } else if (capitalized.contains("PKS") || capitalized.contains("PARKS")) {
    diameter = 64.0;
  } else if (capitalized.contains("TIDBINBILLA")) {
    diameter = 70.0;
  } else if (capitalized.contains("CEDUNA")) {
    diameter = 30.0;
  } else if (capitalized.contains("HOBART")) {
    diameter = 26.0;
  } else if (capitalized.contains("APEX")) {
    diameter = 12.0;
  } else if (capitalized.contains("ASTE")) {
    diameter = 10.0;
  } else if (capitalized.contains("NRO")) {
    diameter = 45.0;
  }

  return diameter;
}

template<class T, class U>
U getMapValue(std::map<T, U> const mymap, T const key, U const default_value) {
  auto iter = mymap.find(key);
  if (iter != mymap.end()) {
    return iter->second;
  } else {
    return default_value;
  }
}

String getIntent(Int srctype) {
  static std::map<Int, String> intent_map;
  if (intent_map.size() == 0) {
    String sep1 = "#";
    String sep2 = ",";
    String target = "OBSERVE_TARGET";
    String atmcal = "CALIBRATE_ATMOSPHERE";
    String anycal = "CALIBRATE_SOMETHING";
    String onstr = "ON_SOURCE";
    String offstr = "OFF_SOURCE";
    String pswitch = "POSITION_SWITCH";
    String nod = "NOD";
    String fswitch = "FREQUENCY_SWITCH";
    String sigstr = "SIG";
    String refstr = "REF";
    String hot = "HOT";
    String warm = "WARM";
    String cold = "COLD";
    String unspecified = "UNSPECIFIED";
    String ftlow = "LOWER";
    String fthigh = "HIGHER";
    intent_map[0] = target + sep1 + onstr + sep2 + pswitch;
    intent_map[1] = target + sep1 + offstr + sep2 + pswitch;
    intent_map[2] = target + sep1 + onstr + sep2 + nod;
    intent_map[3] = target + sep1 + onstr + sep2 + fswitch + sep1 + sigstr;
    intent_map[4] = target + sep1 + onstr + sep2 + fswitch + sep1 + refstr;
    intent_map[6] = atmcal + sep1 + offstr + sep2 + unspecified;
    intent_map[7] = atmcal + sep1 + hot + sep2 + unspecified;
    intent_map[8] = atmcal + sep1 + warm + sep2 + unspecified;
    intent_map[9] = atmcal + sep1 + cold + sep2 + unspecified;
    intent_map[10] = atmcal + sep1 + onstr + sep2 + pswitch;
    intent_map[11] = atmcal + sep1 + offstr + sep2 + pswitch;
    intent_map[12] = atmcal + sep1 + onstr + sep2 + nod;
    intent_map[13] = atmcal + sep1 + onstr + sep2 + fswitch + sep1 + sigstr;
    intent_map[14] = atmcal + sep1 + offstr + sep2 + fswitch + sep1 + refstr;
    intent_map[20] = target + sep1 + onstr + sep2 + fswitch + sep1 + ftlow;
    intent_map[21] = target + sep1 + offstr + sep2 + fswitch + sep1 + ftlow;
    intent_map[26] = atmcal + sep1 + offstr + sep2 + fswitch + sep1 + ftlow;
    intent_map[27] = atmcal + sep1 + offstr + sep2 + fswitch + sep1 + ftlow;
    intent_map[28] = atmcal + sep1 + offstr + sep2 + fswitch + sep1 + ftlow;
    intent_map[29] = atmcal + sep1 + offstr + sep2 + fswitch + sep1 + ftlow;
    intent_map[30] = target + sep1 + onstr + sep2 + fswitch + sep1 + fthigh;
    intent_map[31] = target + sep1 + offstr + sep2 + fswitch + sep1 + fthigh;
    intent_map[36] = atmcal + sep1 + offstr + sep2 + fswitch + sep1 + fthigh;
    intent_map[37] = atmcal + sep1 + offstr + sep2 + fswitch + sep1 + fthigh;
    intent_map[38] = atmcal + sep1 + offstr + sep2 + fswitch + sep1 + fthigh;
    intent_map[39] = atmcal + sep1 + offstr + sep2 + fswitch + sep1 + fthigh;
    intent_map[90] = target + sep1 + onstr + sep2 + unspecified;
    intent_map[91] = target + sep1 + offstr + sep2 + unspecified;
    intent_map[92] = anycal + sep1 + offstr + sep2 + unspecified;
  }
  String default_type = "UNKNOWN_INTENT";
  return getMapValue(intent_map, srctype, default_type);
}

Int getSubscan(Int srctype) {
  static std::map<Int, Int> subscan_map;
  if (subscan_map.size() == 0) {
    subscan_map[0] = 1;
    subscan_map[1] = 2;
    subscan_map[2] = 1;
    subscan_map[3] = 1;
    subscan_map[4] = 2;
    subscan_map[6] = 1;
    subscan_map[7] = 2;
    subscan_map[8] = 3;
    subscan_map[9] = 4;
    subscan_map[10] = 5;
    subscan_map[11] = 6;
    subscan_map[12] = 7;
    subscan_map[13] = 8;
    subscan_map[14] = 9;
    subscan_map[20] = 1;
    subscan_map[21] = 2;
    subscan_map[26] = 1;
    subscan_map[27] = 2;
    subscan_map[28] = 3;
    subscan_map[29] = 4;
    subscan_map[30] = 3;
    subscan_map[31] = 4;
    subscan_map[36] = 5;
    subscan_map[37] = 6;
    subscan_map[38] = 7;
    subscan_map[39] = 8;
    subscan_map[90] = 1;
    subscan_map[91] = 2;
    subscan_map[92] = 1;
  }
  Int default_subscan = 1;
  return getMapValue(subscan_map, srctype, default_subscan);
}

casacore::Stokes::StokesTypes getPol(uInt const polno, casacore::String const poltype) {
	casacore::Stokes::StokesTypes stokes = casacore::Stokes::Undefined;
	if (poltype == "linear") {
		switch (polno) {
		case 0:
			stokes = casacore::Stokes::XX;
			break;
		case 1:
			stokes = casacore::Stokes::YY;
			break;
		case 2:
			stokes = casacore::Stokes::XY;
			break;
		case 3:
			stokes = casacore::Stokes::YX;
		}
	} else if (poltype == "circular") {
		switch (polno) {
		case 0:
			stokes = casacore::Stokes::RR;
			break;
		case 1:
			stokes = casacore::Stokes::LL;
			break;
		case 2:
			stokes = casacore::Stokes::RL;
			break;
		case 3:
			stokes = casacore::Stokes::LR;
			break;
		}
	} else if (poltype == "stokes") {
		switch (polno) {
		case 0:
			stokes = casacore::Stokes::I;
			break;
		case 1:
			stokes = casacore::Stokes::Q;
			break;
		case 2:
			stokes = casacore::Stokes::U;
			break;
		case 3:
			stokes = casacore::Stokes::V;
			break;
		}
	} else if (poltype == "linpol") {
		switch (polno) {
		case 0:
			stokes = casacore::Stokes::Plinear;
			break;
		case 1:
			stokes = casacore::Stokes::Pangle;
			break;
		}
	} else {
		throw casacore::AipsError(
				casacore::String("Invalid poltype") + poltype);
	}
	if (stokes == casacore::Stokes::Undefined) {
		throw casacore::AipsError(
				casacore::String("Invalid polno for type ") + poltype);
	}
	return stokes;
}
}

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

using namespace sdfiller;

Scantable2MSReader::Scantable2MSReader(std::string const &scantable_name) :
    ReaderInterface(scantable_name), main_table_(nullptr), tcal_table_(),
    weather_table_(), scan_column_(), cycle_column_(), ifno_column_(),
    polno_column_(), beam_column_(), flagrow_column_(), time_column_(),
    interval_column_(), srctype_column_(), data_column_(), flag_column_(),
    direction_column_(), scanrate_column_(), fieldname_column_(),
    tsys_column_(), tcal_id_column_(), sorted_rows_(),
    get_antenna_row_([&](sdfiller::AntennaRecord &r) {return Scantable2MSReader::getAntennaRowImpl(r);}),
    get_field_row_([&](sdfiller::FieldRecord &r) {return Scantable2MSReader::getFieldRowImpl(r);}),
    get_observation_row_([&](sdfiller::ObservationRecord &r) {return Scantable2MSReader::getObservationRowImpl(r);}),
    get_processor_row_([&](sdfiller::ProcessorRecord &r) {return Scantable2MSReader::getProcessorRowImpl(r);}),
    get_source_row_([&](sdfiller::SourceRecord &r) {return Scantable2MSReader::getSourceRowImpl(r);}),
    get_spw_row_([&](sdfiller::SpectralWindowRecord &r) {return Scantable2MSReader::getSpectralWindowRowImpl(r);}),
    field_iter_(nullptr), freq_iter_(nullptr), source_iter_(nullptr) {
//  std::cout << "Scantabl2MSReader::Scantable2MSReader" << std::endl;
}

Scantable2MSReader::~Scantable2MSReader() {
//  std::cout << "Scantabl2MSReader::~Scantable2MSReader" << std::endl;
}

void Scantable2MSReader::initializeSpecific() {
//  std::cout << "Scantabl2MSReader::initialize" << std::endl;
  File f(name_);
  if (f.exists() and f.isDirectory()) {
    main_table_.reset(new Table(name_, Table::Old));
  } else {
    throw AipsError("Input data doesn't exist or is invalid");
  }

  // subtables
  tcal_table_ = main_table_->keywordSet().asTable("TCAL");
  weather_table_ = main_table_->keywordSet().asTable("WEATHER");

  // attach columns
  scan_column_.attach(*main_table_, "SCANNO");
  cycle_column_.attach(*main_table_, "CYCLENO");
  ifno_column_.attach(*main_table_, "IFNO");
  polno_column_.attach(*main_table_, "POLNO");
  beam_column_.attach(*main_table_, "BEAMNO");
  flagrow_column_.attach(*main_table_, "FLAGROW");
  time_column_.attach(*main_table_, "TIME");
  interval_column_.attach(*main_table_, "INTERVAL");
  srctype_column_.attach(*main_table_, "SRCTYPE");
  data_column_.attach(*main_table_, "SPECTRA");
  flag_column_.attach(*main_table_, "FLAGTRA");
  direction_column_.attach(*main_table_, "DIRECTION");
  scanrate_column_.attach(*main_table_, "SCANRATE");
  fieldname_column_.attach(*main_table_, "FIELDNAME");
  tsys_column_.attach(*main_table_, "TSYS");
  tcal_id_column_.attach(*main_table_, "TCAL_ID");
  weather_id_column_.attach(*main_table_, "WEATHER_ID");

  tcal_column_.attach(tcal_table_, "TCAL");

  temperature_column_.attach(weather_table_, "TEMPERATURE");
  pressure_column_.attach(weather_table_, "PRESSURE");
  humidity_column_.attach(weather_table_, "HUMIDITY");
  wind_speed_column_.attach(weather_table_, "WINDSPEED");
  wind_direction_column_.attach(weather_table_, "WINDAZ");

  // get sort index
  Sort s;
  Vector<Double> time_list = time_column_.getColumn();
  s.sortKey(time_list.data(), TpDouble, 0, Sort::Ascending);
  Vector<uInt> beamno_list = beam_column_.getColumn();
  s.sortKey(beamno_list.data(), TpUInt, 0, Sort::Ascending);
  Vector<uInt> ifno_list = ifno_column_.getColumn();
  s.sortKey(ifno_list.data(), TpUInt, 0, Sort::Ascending);
  Vector<uInt> polno_list = polno_column_.getColumn();
  s.sortKey(polno_list.data(), TpUInt, 0, Sort::Ascending);
  uInt n = s.sort(sorted_rows_, main_table_->nrow());
  if (n != main_table_->nrow()) {
    AipsError("Internal problem when sorting rows");
  }
//  for (uInt i = 0; i < n; ++i) {
//    uInt j = sorted_rows_[i];
//    printf("id %3u row %3u T %10.3f B %2u S %2u P %2u\n", i, j, time_list[j],
//        beamno_list[j], ifno_list[j], polno_list[j]);
//  }

  // TCAL_ID mapping
  ScalarColumn<uInt> id_column(tcal_table_, "ID");
  tcal_id_map_.clear();
  for (uInt i = 0; i < tcal_table_.nrow(); ++i) {
    tcal_id_map_[id_column(i)] = i;
  }

  // WEATHER_ID mapping
  id_column.attach(weather_table_, "ID");
  weather_id_map_.clear();
  for (uInt i = 0; i < weather_table_.nrow(); ++i) {
    weather_id_map_[id_column(i)] = i;
  }

  // polarization type from header
  pol_type_ = main_table_->keywordSet().asString("POLTYPE");
}

void Scantable2MSReader::finalizeSpecific() {
//  std::cout << "Scantabl2MSReader::finalize" << std::endl;
}

Bool Scantable2MSReader::getAntennaRowImpl(AntennaRecord &record) {
//  std::cout << "Scantabl2MSReader::getAntennaRowImpl" << std::endl;
  TableRecord const &header = main_table_->keywordSet();

  // header antenna name is constructed as <ObservatoryName>//<AntennaName>@<StationName>
  String header_antenna_name = header.asString("AntennaName");
  String::size_type separator = header_antenna_name.find("//");
  String antenna_name;
  if (separator != String::npos) {
    antenna_name = header_antenna_name.substr(separator + 2);
  } else {
    antenna_name = header_antenna_name;
  }
  separator = antenna_name.find("@");
  if (separator != String::npos) {
    record.station = antenna_name.substr(separator + 1);
    record.name = antenna_name.substr(0, separator);
  } else {
    record.station = "";
    record.name = antenna_name;
  }
  record.dish_diameter = queryAntennaDiameter(record.name);
  record.type = "GROUND-BASED";
  record.mount = "ALT-AZ";

  Vector<Double> antenna_position_value = header.asArrayDouble(
      "AntennaPosition");
  Vector<Quantity> antenna_position_quant(3);
  antenna_position_quant[0] = Quantity(antenna_position_value[0], "m");
  antenna_position_quant[1] = Quantity(antenna_position_value[1], "m");
  antenna_position_quant[2] = Quantity(antenna_position_value[2], "m");
  record.position = MPosition(MVPosition(antenna_position_quant),
      MPosition::ITRF);

  // only one entry so redirect function pointer to noMoreRowImpl
  get_antenna_row_ = [&](sdfiller::AntennaRecord &r) {return Scantable2MSReader::noMoreRowImpl<AntennaRecord>(r);};

  return true;
}

Bool Scantable2MSReader::getObservationRowImpl(ObservationRecord &record) {
//  std::cout << "Scantabl2MSReader::getObservationRowImpl" << std::endl;

  ScalarColumn<Double> column(*main_table_, "TIME");
  Vector<Double> time_list = column.getColumn();
  if (record.time_range.size() != 2) {
    record.time_range.resize(2);
  }
  minMax(record.time_range[0], record.time_range[1], time_list);
  TableRecord const &header = main_table_->keywordSet();
  record.observer = header.asString("Observer");
  record.project = header.asString("Project");
  // header antenna name is constructed as <ObservatoryName>//<AntennaName>@<StationName>
  String header_antenna_name = header.asString("AntennaName");
  String::size_type separator = header_antenna_name.find("//");
  if (separator != String::npos) {
    record.telescope_name = header_antenna_name.substr(0, separator);
  } else {
    separator = header_antenna_name.find("@");
    record.telescope_name = header_antenna_name.substr(0, separator);
  }

  // only one entry so redirect function pointer to noMoreRowImpl
  get_observation_row_ = [&](sdfiller::ObservationRecord &r) {return Scantable2MSReader::noMoreRowImpl<ObservationRecord>(r);};

  return true;
}

Bool Scantable2MSReader::getProcessorRowImpl(ProcessorRecord &/*record*/) {
//  std::cout << "Scantabl2MSReader::getProcessorRowImpl" << std::endl;

  // just add empty row once

  // only one entry so redirect function pointer to noMoreRowImpl
  get_processor_row_ = [&](sdfiller::ProcessorRecord &r) {return Scantable2MSReader::noMoreRowImpl<ProcessorRecord>(r);};

  return true;
}

Bool Scantable2MSReader::getSourceRowImpl(SourceRecord &record) {
  return getRowImplTemplate(source_iter_, record, get_source_row_);
}

Bool Scantable2MSReader::getFieldRowImpl(FieldRecord &record) {
  return getRowImplTemplate(field_iter_, record, get_field_row_, &field_map_);
}

Bool Scantable2MSReader::getSpectralWindowRowImpl(
    SpectralWindowRecord &record) {
  return getRowImplTemplate(freq_iter_, record, get_spw_row_, &num_chan_map_);
}

Bool Scantable2MSReader::getData(size_t irow, DataRecord &record) {
//  std::cout << "Scantable2MSReader::getData(irow=" << irow << ")" << std::endl;

  if (irow >= main_table_->nrow()) {
    return false;
  }

  constexpr double kDay2Sec = 86400.0;
  uInt index = sorted_rows_[irow];
//  std::cout << "Accessing row " << index << std::endl;

  time_column_.get(index, record.time);
  record.time *= kDay2Sec;
  interval_column_.get(index, record.interval);
//  std::cout << "TIME=" << record.time << " INTERVAL=" << record.interval
//      << std::endl;
  Int srctype = srctype_column_(index);
  record.intent = getIntent(srctype);
  record.scan = (Int) scan_column_(index);
  record.subscan = getSubscan(srctype);
  String field_name = fieldname_column_.get(index);
  record.field_id = field_map_[field_name];
  record.antenna_id = (Int) 0;
  direction_column_.get(index, record.direction_vector);
  scanrate_column_.get(index, record.scan_rate);
  record.feed_id = (Int) beam_column_(index);
  record.spw_id = (Int) ifno_column_(index);
  uInt polno;
  polno_column_.get(index, polno);
  record.pol = getPol(polno, pol_type_);
  record.pol_type = pol_type_;
//  std::cout << "set data size to " << num_chan_map_[record.spw_id] << " shape "
//      << record.data.shape() << std::endl;
  record.setDataSize(num_chan_map_[record.spw_id]);
  data_column_.get(index, record.data);
  Vector<uChar> flag = flag_column_(index);
  convertArray(record.flag, flag);
  uInt flagrow = flagrow_column_(index);
  Bool bflagrow = (flagrow != 0);
  record.flag_row = bflagrow;

  if (tsys_column_.isDefined(index)) {
//    std::cout << "set tsys size to " << tsys_column_.shape(index)[0]
//        << " shape " << record.tsys.shape() << std::endl;
    record.setTsysSize(tsys_column_.shape(index)[0]);
    Vector<Float> tsys = tsys_column_(index);
    if (!allEQ(tsys, 1.0f) || !allEQ(tsys, 0.0f)) {
      record.tsys = tsys;
    }
  }

  uInt tcal_id = tcal_id_column_(index);
  auto iter = tcal_id_map_.find(tcal_id);
  if (iter != tcal_id_map_.end()) {
    uInt tcal_row = iter->second;
//    std::cout << "set tsys size to " << tcal_column.shape(0)[0] << " shape "
//        << record.tcal.shape() << std::endl;
    record.setTcalSize(tcal_column_.shape(tcal_row)[0]);
    Vector<Float> tcal = tcal_column_(tcal_row);
    if (!allEQ(tcal, 1.0f) || !allEQ(tcal, 0.0f)) {
      record.tcal = tcal;
    }
  }

  uInt weather_id = weather_id_column_(index);
  auto iter2 = weather_id_map_.find(weather_id);
  if (iter2 != weather_id_map_.end()) {
    uInt weather_row = iter2->second;
    record.temperature = temperature_column_(weather_row);
    record.pressure = pressure_column_(weather_row);
    record.rel_humidity = humidity_column_(weather_row);
    record.wind_speed = wind_speed_column_(weather_row);
    record.wind_direction = wind_direction_column_(weather_row);
  }

  return true;
}

} //# NAMESPACE CASA - END
