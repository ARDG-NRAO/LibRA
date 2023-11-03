/*
 * NROReader.h
 *
 *  Created on: May 9, 2016
 *      Author: wataru kawasaki
 */

#ifndef SINGLEDISH_FILLER_NRO2MSREADER_H_
#define SINGLEDISH_FILLER_NRO2MSREADER_H_

#define STRING2CHAR(s) const_cast<char *>((s).c_str())

#include <casacore/measures/Measures/Stokes.h>
#include <casacore/tables/Tables/Table.h>
#include <casacore/tables/Tables/ScaColDesc.h>
#include <casacore/tables/Tables/SetupNewTab.h>

#include <singledishfiller/Filler/ReaderInterface.h>
#include <singledishfiller/Filler/NROData.h>
#include <string>
#include <memory>
#include <functional>

using namespace std;

namespace casa { //# NAMESPACE CASA - BEGIN

// forward declaration
class NROOptionalTables;

class NRO2MSReader final : public ReaderInterface {
public:
  // NROOptionalTables generates optional tables
  // required for NRO data
  using OptionalTables = NROOptionalTables;

  NRO2MSReader(std::string const &scantable_name);
  virtual ~NRO2MSReader();

  // get number of rows
  size_t getNumberOfRows() const override;

  casacore::MDirection::Types getDirectionFrame() const override;

  casacore::Bool isFloatData() const override {
    return true;
  }

  casacore::String getDataUnit() const override {
    return "K";
  }

  // to get OBSERVATION table
  casacore::Bool getObservationRow(sdfiller::ObservationRecord &record) override {
    POST_START;
    casacore::Bool return_value = get_observation_row_(record);
    POST_END;
    return return_value;
  }

  // to get ANTENNA table
  casacore::Bool getAntennaRow(sdfiller::AntennaRecord &record) override {
    POST_START;
    casacore::Bool return_value = get_antenna_row_(record);
    POST_END;
    return return_value;
  }

  // to get PROCESSOR table
  casacore::Bool getProcessorRow(sdfiller::ProcessorRecord &record) override {
    POST_START;
    casacore::Bool return_value = get_processor_row_(record);
    POST_END;
    return return_value;
  }

  // to get SOURCE table
  casacore::Bool getSourceRow(sdfiller::SourceRecord &record) override {
    POST_START;
    casacore::Bool return_value = get_source_row_(record);
    POST_END;
    return return_value;
  }

  // to get FIELD table
  casacore::Bool getFieldRow(sdfiller::FieldRecord &record) override {
    POST_START;
    casacore::Bool return_value = get_field_row_(record);
    POST_END;
    return return_value;
  }

  // to get SOURCE table
  casacore::Bool getSpectralWindowRow(sdfiller::SpectralWindowRecord &record) override {
    POST_START;
    casacore::Bool return_value = get_spw_row_(record);
    POST_END;
    return return_value;
  }

  // for DataAccumulator
  casacore::Bool getData(size_t irow, sdfiller::DataRecord &record) override;

protected:
  void initializeSpecific() override;
  void finalizeSpecific() override;

private:
  FILE *fp_;
  sdfiller::NRODataObsHeader obs_header_;
  void readObsHeader();
  void readScanData(int const irow, sdfiller::NRODataScanData &data);
  void checkEndian();
  template<typename T>
  void convertEndian(T &value) {
    char volatile *first = reinterpret_cast<char volatile *>(&value) ;
    char volatile *last = first + sizeof(T) ;
    std::reverse(first, last) ;
  }

  bool same_endian_;

  template<typename T>
  void readHeader(T &v) {
    if ((int)fread(&v, 1, sizeof(T), fp_) != sizeof(T)) {
      cout << "read failed." << endl;
    }
    if (!same_endian_) {
      convertEndian(v);
    }
  }

  template<typename T>
  void readHeader(T *v, size_t numArray) {
    for (size_t i = 0; i < numArray; ++i) {
      readHeader<T>(v[i]);
    }
  }

  void readHeader(string &v, size_t strLength) {
    v.resize(strLength);
    if (fread(STRING2CHAR(v), 1, strLength, fp_) != strLength) {
      cout << "read failed." << endl;
    }
    v.resize(strlen(v.c_str())); // remove trailing null characters
  }

  void readHeader(string *v, size_t strLength, size_t numArray) {
    for (size_t i = 0; i < numArray; ++i) {
      readHeader(v[i], strLength);
    }
  }

  struct NROArrayData {
	  int beam_id=-1;
	  casacore::Stokes::StokesTypes stokes_type = casacore::Stokes::Undefined;
	  string pol_name="";
	  int spw_id=-1;
	  bool is_used=false;
	  void set(int16_t const arr_data, string const *pol_data) {
		  // indices in NOSTAR data are 1-base
		  if (arr_data < 1101) {
		    is_used = false;
		    beam_id = -1;
		    spw_id = -1;
		    pol_name = "";
		    stokes_type = casacore::Stokes::Undefined;
		    return;
		  }
		  is_used = true;
		  beam_id = static_cast<int>(arr_data/1000) - 1;
		  int pol_id = static_cast<int>((arr_data % 1000)/100) - 1;
		  spw_id = static_cast<int>(arr_data % 100) -1;
		  pol_name = pol_data[pol_id];
		  stokes_type = casacore::Stokes::type(pol_name);
		  if (stokes_type == casacore::Stokes::Undefined) {
			  throw casacore::AipsError("Got unsupported polarization type\n");
		  }
	  }
	  int getBeamId() const {
		  if (beam_id < 0)
		    throw casacore::AipsError("Array data is not set yet\n");
		  return beam_id;}
	  casacore::Stokes::StokesTypes getPol() const {
		  if (stokes_type == casacore::Stokes::Undefined)
		    throw casacore::AipsError("Array data is not set yet\n");
		  return stokes_type;}
	  int getSpwId() const {
		  if (spw_id < 0)
		    throw casacore::AipsError("Array data is not set yet\n");
		  return spw_id;}
	  string getPolName() const {
		  if (pol_name.size() == 0)
		    throw casacore::AipsError("Array data is not set yet\n");
		  return pol_name;}
	  bool isUsed() const {
	    return is_used;
	  }
  };

  std::vector<NROArrayData> array_mapper_;

  void constructArrayTable();
  bool checkScanArray(string const scan_array, NROArrayData const *header_array);
  // Returns the first array ID whose SPW ID is spwid.
  int getFirstArrayIdWithSpwID(int spwid) {
	  for (int iarr = 0; iarr < obs_header_.ARYNM0 ; ++iarr) {
	    if (spwid == array_mapper_[iarr].spw_id) {
	      return iarr;
	    }
	  }
	  // no array with spwid found
	  throw casacore::AipsError("Internal ERROR: Could not find array ID corresponds to an SPW ID\n");
  }

  int beam_id_counter_;
  int source_spw_id_counter_;
  int spw_id_counter_;
  casacore::Vector<casacore::Double> time_range_sec_;
  int const len_obs_header_ = 15136;
  double getMJD(string const &time);
  double getIntMiddleTimeSec(sdfiller::NRODataScanData const &data);
  double getIntStartTimeSec(int const scanno);
  double getIntEndTimeSec(int const scanno);
  void getFullTimeRange();
  double getMiddleOfTimeRangeSec();

  casacore::Double const posx_ = -3.8710235e6;
  casacore::Double const posy_ =  3.4281068e6;
  casacore::Double const posz_ =  3.7240395e6;

  double getRestFrequency(int const spwno);
  string convertVRefName(string const &vref0);
  void shiftFrequency(string const &vdef,
                      double const v,
		      std::vector<double> &freqs);

  std::vector<double> getSpectrum(int const irow, sdfiller::NRODataScanData const &data);
//  casacore::Int getPolNo(string const &rx);

  std::function<casacore::Bool(sdfiller::AntennaRecord &)> get_antenna_row_;
  std::function<casacore::Bool(sdfiller::FieldRecord &)> get_field_row_;
  std::function<casacore::Bool(sdfiller::ObservationRecord &)> get_observation_row_;
  std::function<casacore::Bool(sdfiller::ProcessorRecord &)> get_processor_row_;
  std::function<casacore::Bool(sdfiller::SourceRecord &)> get_source_row_;
  std::function<casacore::Bool(sdfiller::SpectralWindowRecord &)> get_spw_row_;

  casacore::Bool getAntennaRowImpl(sdfiller::AntennaRecord &record);
  casacore::Bool getFieldRowImpl(sdfiller::FieldRecord &record);
  casacore::Bool getObservationRowImpl(sdfiller::ObservationRecord &record);
  casacore::Bool getProcessorRowImpl(sdfiller::ProcessorRecord &record);
  casacore::Bool getSourceRowImpl(sdfiller::SourceRecord &record);
  casacore::Bool getSpectralWindowRowImpl(sdfiller::SpectralWindowRecord &record);

  template<class _Record>
  casacore::Bool noMoreRowImpl(_Record &) {
    POST_START;POST_END;
    return false;
  }

  // methods that are only accessible from NROOptionalTables
  int getNROArraySize() const {
//    return obs_header_.ARYNM0; //obs_header_.NBEAM * obs_header_.NPOL * obs_header_.NSPWIN;
    return NRO_ARYMAX;
  }
  int getNRONumBeam() const {
    return obs_header_.NBEAM;
  }
  int getNRONumPol() const {
    return obs_header_.NPOL;
  }
  int getNRONumSpw() const {
    return obs_header_.NSPWIN;
  }

  bool isNROArrayUsed(int array_id) const {
    return array_mapper_[array_id].isUsed();
  }
  int getNROArrayBeamId(int array_id) const {
//	  assert(array_id >= 0 && array_id < getNROArraySize());
    return array_mapper_[array_id].getBeamId();
  }
  casacore::Stokes::StokesTypes getNROArrayPol(int array_id) const {
//	  assert(array_id >= 0 && array_id < getNROArraySize());
    return array_mapper_[array_id].getPol();
  }
  int getNROArraySpwId(int array_id) const {
//	  assert(array_id >= 0 && array_id < getNROArraySize());
    return array_mapper_[array_id].getSpwId();
  }

  // friend: NROOptionalTables
  friend NROOptionalTables;
};

// OptionalTables class for NRO data
class NROOptionalTables {
public:
  static void Generate(casacore::Table &table, NRO2MSReader const &reader) {
    // generate NRO_ARRAY table
    Generate_NRO_ARRAY(table, reader);
  }

private:
  static void Generate_NRO_ARRAY(casacore::Table &table, NRO2MSReader const &reader) {
    casacore::String const nro_tablename = "NRO_ARRAY";

    casacore::TableDesc td(nro_tablename, casacore::TableDesc::Scratch);
    td.addColumn(casacore::ScalarColumnDesc<casacore::Int>("ARRAY"));
    td.addColumn(casacore::ScalarColumnDesc<casacore::Int>("BEAM"));
    td.addColumn(casacore::ScalarColumnDesc<casacore::Int>("POLARIZATION"));
    td.addColumn(casacore::ScalarColumnDesc<casacore::Int>("SPECTRAL_WINDOW"));
    casacore::String tabname = table.tableName() + "/" + nro_tablename;
    casacore::SetupNewTable newtab(tabname, td, casacore::Table::Scratch);
    table.rwKeywordSet().defineTable(nro_tablename,
        casacore::Table(newtab, reader.getNROArraySize()));

    casacore::Table nro_table = table.rwKeywordSet().asTable(nro_tablename);
    casacore::ScalarColumn<int> arr(nro_table, "ARRAY");
    casacore::ScalarColumn<int> bea(nro_table, "BEAM");
    casacore::ScalarColumn<int> pol(nro_table, "POLARIZATION");
    casacore::ScalarColumn<int> spw(nro_table, "SPECTRAL_WINDOW");
    for (int iarr = 0; iarr < reader.getNROArraySize(); ++iarr) {
      arr.put(iarr, iarr);
      if (reader.isNROArrayUsed(iarr)) {
        bea.put(iarr, reader.getNROArrayBeamId(iarr));
        pol.put(iarr, reader.getNROArrayPol(iarr));
        spw.put(iarr, reader.getNROArraySpwId(iarr));
      } else {
        // array is not used, fill with -1
        bea.put(iarr, -1);
        pol.put(iarr, -1);
        spw.put(iarr, -1);
      }
    }
  }
};

} //# NAMESPACE CASA - END

#endif /* SINGLEDISH_FILLER_NRO2MSREADER_H_ */
