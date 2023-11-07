/*
 * ScantableIterator.h
 *
 *  Created on: Jan 28, 2016
 *      Author: nakazato
 */

#ifndef SINGLEDISH_FILLER_SCANTABLEITERATOR_H_
#define SINGLEDISH_FILLER_SCANTABLEITERATOR_H_

#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Utilities/Compare.h>
#include <casacore/casa/Utilities/Sort.h>

#include <casacore/measures/Measures/MFrequency.h>

#include <casacore/tables/Tables/Table.h>
#include <casacore/tables/Tables/ScalarColumn.h>
#include <casacore/tables/Tables/ArrayColumn.h>
#include <casacore/tables/TaQL/ExprNode.h>
#include <singledishfiller/Filler/FieldRecord.h>
#include <singledishfiller/Filler/FillerUtil.h>
#include <singledishfiller/Filler/SourceRecord.h>
#include <singledishfiller/Filler/SpectralWindowRecord.h>
#include <singledishfiller/Filler/SysCalRecord.h>

using namespace casacore;

namespace {
template<class T>
inline void getDataRangePerId(casacore::Vector<casacore::uInt> const &index_list,
    casacore::Vector<T> const &id_list, size_t start,
    size_t end, std::map<T, casacore::Block<casacore::uInt> > &range_index) {
  casacore::uInt id_prev = id_list[index_list[start]];
  casacore::uInt id_min = index_list[start];
  casacore::uInt id_max = 0;
  casacore::Block < casacore::uInt > current_index(2);
  for (casacore::uInt i = start + 1; i < end; ++i) {
    casacore::uInt j = index_list[i];
    casacore::uInt current_id = id_list[j];
//      std::cout << "weather_id = " << weather_id << " id_prev = " << id_prev
//          << " time range (" << time_min << "," << time_max << ")" << std::endl;
    if (current_id != id_prev) {
      id_max = index_list[i - 1];
      current_index[0] = id_min;
      current_index[1] = id_max;
      range_index[id_prev] = current_index;

      id_prev = current_id;
      id_min = j;
    }
  }
  casacore::uInt last_id = id_list[index_list[end - 1]];
  if (range_index.find(last_id) == range_index.end()) {
    id_max = index_list[end - 1];
    current_index[0] = id_min;
    current_index[1] = id_max;
    range_index[last_id] = current_index;
  }
}

}

namespace casa { //# NAMESPACE CASA - BEGIN

class ScantableIteratorBase {
public:
  ScantableIteratorBase() :
      current_iter_(0), num_iter_(0) {
  }
  virtual ~ScantableIteratorBase() {
  }
  void initialize(size_t num_iter) {
    num_iter_ = num_iter;
    current_iter_ = 0;
  }
  bool moreData() const {
    return current_iter_ < num_iter_;
  }
  void next() {
    ++current_iter_;
  }

protected:
  size_t current_iter_;

private:
  size_t num_iter_;
};

template<class _Product, class _Record>
class ScantableIteratorInterface: public ScantableIteratorBase {
public:
  using Product = _Product;
  using Record = _Record;
  ScantableIteratorInterface(casacore::Table const &table) :
      ScantableIteratorBase(), main_table_(table) {
  }
  virtual ~ScantableIteratorInterface() {}
  virtual void getEntry(Record &record) = 0;
  virtual void getProduct(Product *p) = 0;
protected:
  casacore::Table const main_table_;
};

class ScantableFrequenciesIterator final :
  public ScantableIteratorInterface<std::map<casacore::Int, casacore::Int>, sdfiller::SpectralWindowRecord> {
public:
  ScantableFrequenciesIterator(casacore::Table const &table) :
      ScantableIteratorInterface(table) {
    casacore::TableRecord const &header = main_table_.keywordSet();
    sub_table_ = header.asTable("FREQUENCIES");
    //size_t nrow = sub_table_.nrow();
    casacore::ScalarColumn < casacore::uInt > ifno_column(main_table_, "IFNO");
    casacore::Vector < casacore::uInt > ifno_list = ifno_column.getColumn();
    casacore::Sort sorter;
    sorter.sortKey(ifno_list.data(), TpUInt);
    casacore::Vector < casacore::uInt > index_vector;
    casacore::uInt n = sorter.sort(index_vector, ifno_list.nelements(),
        casacore::Sort::HeapSort | casacore::Sort::NoDuplicates);

    initialize(n);
    ifno_list_.resize(n);
    for (casacore::uInt i = 0; i < n; ++i) {
      ifno_list_[i] = ifno_list[index_vector[i]];
    }

    casacore::ScalarColumn < casacore::uInt > freq_id_column(main_table_, "FREQ_ID");

    // attach columns
    id_column_.attach(sub_table_, "ID");
    refpix_column_.attach(sub_table_, "REFPIX");
    refval_column_.attach(sub_table_, "REFVAL");
    increment_column_.attach(sub_table_, "INCREMENT");
    id_list_ = id_column_.getColumn();
  }
  virtual ~ScantableFrequenciesIterator() {
  }

  void getEntry(Record &record) override {
    size_t const irow = current_iter_;
//    std::cout << "getEntry for row " << irow << std::endl;
    casacore::Int spw_id = ifno_list_[irow];
    casacore::Table subtable = main_table_(main_table_.col("IFNO") == (casacore::uInt) spw_id, 1);
    casacore::ScalarColumn < casacore::uInt > freq_id_column(subtable, "FREQ_ID");
    casacore::uInt freq_id = freq_id_column(0);
    casacore::Int jrow = -1;
    for (casacore::uInt i = 0; i < id_list_.size(); ++i) {
      if (id_list_[i] == freq_id) {
        jrow = (casacore::Int) i;
        break;
      }
    }
    casacore::ArrayColumn<casacore::uChar> flag_column(subtable, "FLAGTRA");
    casacore::Int num_chan = flag_column.shape(0)[0];
    casacore::String freq_frame = sub_table_.keywordSet().asString("BASEFRAME");
    casacore::MFrequency::Types frame_type;
    casacore::Bool status = casacore::MFrequency::getType(frame_type, freq_frame);
    if (!status) {
      frame_type = casacore::MFrequency::N_Types;
    }
    casacore::Double refpix = refpix_column_(jrow);
    casacore::Double refval = refval_column_(jrow);
    casacore::Double increment = increment_column_(jrow);
//    std::cout << "spw " << spw_id << " nchan " << num_chan << " mfr "
//        << (casacore::Int) frame_type << " (" << freq_frame << ") ref " << refpix << ", "
//        << refval << ", " << increment << std::endl;
    record.spw_id = spw_id;
    record.num_chan = num_chan;
    record.meas_freq_ref = frame_type;
    record.refpix = refpix;
    record.refval = refval;
    record.increment = increment;

    // update product
    product_[spw_id] = num_chan;
  }
  void getProduct(Product *p) override {
    if (p) {
      for (auto iter = product_.begin(); iter != product_.end(); ++iter) {
        (*p)[iter->first] = iter->second;
      }
    }
  }

private:
  casacore::Table sub_table_;
  casacore::ScalarColumn<casacore::uInt> id_column_;
  casacore::ScalarColumn<casacore::Double> refpix_column_;
  casacore::ScalarColumn<casacore::Double> refval_column_;
  casacore::ScalarColumn<casacore::Double> increment_column_;
  casacore::Vector<casacore::uInt> ifno_list_;
  casacore::Vector<casacore::uInt> id_list_;
  Product product_;
};

class ScantableFieldIterator final :
  public ScantableIteratorInterface<std::map<casacore::String, casacore::Int>, sdfiller::FieldRecord> {
public:
  ScantableFieldIterator(casacore::Table const &table) :
      ScantableIteratorInterface(table), row_list_(), is_reserved_(), field_column_(
          main_table_, "FIELDNAME"), source_column_(main_table_, "SRCNAME"), time_column_(
          main_table_, "TIME"), direction_column_(main_table_, "SRCDIRECTION"), scanrate_column_(
          main_table_, "SCANRATE"), direction_storage_(2, 2, 0.0) {
    casacore::Vector < casacore::String > field_name_list = field_column_.getColumn();
    casacore::Sort sorter;
    sorter.sortKey(field_name_list.data(), TpString);
    casacore::uInt n = sorter.sort(row_list_, field_name_list.size(),
        casacore::Sort::QuickSort | casacore::Sort::NoDuplicates);
    is_reserved_.resize(n);
    is_reserved_ = false;
    initialize(n);
  }

  virtual ~ScantableFieldIterator() {
  }

  void getEntry(Record &record) override {
    casacore::uInt const irow = row_list_[current_iter_];
    casacore::String field_name_with_id = field_column_(irow);
    auto pos = field_name_with_id.find("__");
    auto defaultFieldId =
        [&]() {
          casacore::Int my_field_id = 0;
          while (is_reserved_[my_field_id] && (casacore::uInt)my_field_id < is_reserved_.size()) {
            my_field_id++;
          }
          if ((casacore::uInt)my_field_id >= is_reserved_.size()) {
            throw casacore::AipsError("Internal inconsistency in FIELD_ID numbering");
          }
          is_reserved_[my_field_id] = true;
          return my_field_id;
        };
    if (pos != casacore::String::npos) {
      record.name = field_name_with_id.substr(0, pos);
      casacore::Int field_id = casacore::String::toInt(field_name_with_id.substr(pos + 2));
      if (field_id < 0) {
        record.field_id = defaultFieldId();
      } else if ((casacore::uInt) field_id >= is_reserved_.size()
          || !is_reserved_[field_id]) {
        record.field_id = field_id;
        is_reserved_[field_id] = true;
      } else {
        record.field_id = defaultFieldId();
      }
    } else {
      record.name = field_name_with_id;
      record.field_id = defaultFieldId();
    }
    record.time = time_column_(irow) * 86400.0;
    record.source_name = source_column_(irow);
    record.frame = casacore::MDirection::J2000;
    casacore::Matrix < Double
        > direction(direction_storage_(casacore::IPosition(2, 0, 0), casacore::IPosition(2, 1, 0)));
//    std::cout << "direction = " << direction << " (shape " << direction.shape()
//        << ")" << std::endl;
    direction_storage_.column(0) = direction_column_(irow);
    if (scanrate_column_.isDefined(irow)) {
      casacore::Vector < casacore::Double > scan_rate = scanrate_column_(irow);
      if (anyNE(scan_rate, 0.0)) {
        direction_storage_.column(1) = scan_rate;
        direction.reference(direction_storage_);
      }
    }
//    std::cout << "direction = " << direction << " (shape " << direction.shape()
//        << ")" << std::endl;
    record.direction = direction;

    // update product
    product_[field_name_with_id] = record.field_id;
  }
  void getProduct(Product *p) override {
    if (p) {
      for (auto iter = product_.begin(); iter != product_.end(); ++iter) {
        (*p)[iter->first] = iter->second;
      }
    }
  }

private:
  casacore::Vector<casacore::uInt> row_list_;
  casacore::Vector<casacore::Bool> is_reserved_;
  casacore::ScalarColumn<casacore::String> field_column_;
  casacore::ScalarColumn<casacore::String> source_column_;
  casacore::ScalarColumn<casacore::Double> time_column_;
  casacore::ArrayColumn<casacore::Double> direction_column_;
  casacore::ArrayColumn<casacore::Double> scanrate_column_;
  casacore::Matrix<casacore::Double> direction_storage_;
  Product product_;
};

class ScantableSourceIterator final :
  public ScantableIteratorInterface<void *, sdfiller::SourceRecord> {
public:
  ScantableSourceIterator(casacore::Table const &table) :
      ScantableIteratorInterface(table), name_column_(main_table_, "SRCNAME"), direction_column_(
          main_table_, "SRCDIRECTION"), proper_motion_column_(main_table_,
          "SRCPROPERMOTION"), sysvel_column_(main_table_, "SRCVELOCITY"), molecule_id_column_(
          main_table_, "MOLECULE_ID"), ifno_column_(main_table_, "IFNO"), molecules_table_(), row_list_(), source_id_map_() {
    casacore::TableRecord const &header = main_table_.keywordSet();
    molecules_table_ = header.asTable("MOLECULES");
    restfrequency_column_.attach(molecules_table_, "RESTFREQUENCY");
    molecule_name_column_.attach(molecules_table_, "NAME");
    casacore::Vector < casacore::String > source_name_list = name_column_.getColumn();
    casacore::Vector < casacore::uInt > ifno_list = ifno_column_.getColumn();
    casacore::Sort sorter;
    sorter.sortKey(source_name_list.data(), TpString);
    casacore::Vector < casacore::uInt > unique_vector;
    casacore::uInt num_unique = sorter.sort(unique_vector, source_name_list.size(),
        casacore::Sort::QuickSort | casacore::Sort::NoDuplicates);
    for (casacore::uInt i = 0; i < num_unique; ++i) {
      source_id_map_[name_column_(unique_vector[i])] = (casacore::Int) i;
    }
    casacore::Sort sorter2;
    sorter2.sortKey(source_name_list.data(), TpString);
    sorter2.sortKey(ifno_list.data(), TpUInt);
    unique_vector.resize();
    num_unique = sorter2.sort(row_list_, source_name_list.size(),
        casacore::Sort::QuickSort | casacore::Sort::NoDuplicates);
//    for (casacore::uInt i = 0; i < num_unique; ++i) {
//      std::cout << i << ": SRCNAME \"" << name_column_(row_list_[i])
//          << "\" IFNO " << ifno_column_(row_list_[i]) << std::endl;
//    }
    initialize(num_unique);

    // generate molecule_id_map_
    casacore::ScalarColumn < casacore::uInt > id_column(molecules_table_, "ID");
    casacore::Vector < casacore::uInt > molecule_id_list = id_column.getColumn();
    for (casacore::uInt i = 0; i < id_column.nrow(); ++i) {
      molecule_id_map_[molecule_id_list[i]] = i;
    }

    // generate sorted_index_
    casacore::uInt nrow_main = main_table_.nrow();
    casacore::ScalarColumn < casacore::String > srcname_column(main_table_, "SRCNAME");
    casacore::ScalarColumn < casacore::uInt > ifno_column(main_table_, "IFNO");
    casacore::ScalarColumn < casacore::Double > time_column(main_table_, "TIME");
    casacore::ScalarColumn < casacore::Double > &interval_column = time_column;
    casacore::Sort sorter3;
    casacore::Vector < casacore::String > srcname_list = srcname_column.getColumn();
    casacore::Vector < casacore::Double > time_list = time_column.getColumn();
    sorter3.sortKey(srcname_list.data(), TpString, 0, casacore::Sort::Ascending);
    sorter3.sortKey(ifno_list.data(), TpUInt, 0, casacore::Sort::Ascending);
    sorter3.sortKey(time_list.data(), TpDouble, 0, casacore::Sort::Ascending);
    casacore::Vector < casacore::uInt > index_list;
    sorter3.sort(index_list, nrow_main, casacore::Sort::QuickSort);

    std::vector<size_t> srcname_boundary;
    srcname_boundary.push_back(0u);
    casacore::String current = srcname_list[index_list[0]];
    for (casacore::uInt i = 1; i < nrow_main; ++i) {
      casacore::String name = srcname_list[index_list[i]];
      if (current != name) {
        srcname_boundary.push_back(i);
        current = name;
      }
    }
    srcname_boundary.push_back(nrow_main);

    constexpr double kDay2Sec = 86400.0;
    interval_column.attach(main_table_, "INTERVAL");
    time_range_.clear();
    for (size_t i = 0; i < srcname_boundary.size() - 1; ++i) {
      casacore::String name = srcname_list[index_list[srcname_boundary[i]]];
      size_t start = srcname_boundary[i];
      size_t end = srcname_boundary[i + 1];
      std::map<casacore::uInt, casacore::Block<casacore::Double> > range;
      std::map<casacore::uInt, casacore::Block<casacore::uInt> > range_index;
      getDataRangePerId(index_list, ifno_list, start, end,
          range_index);
      for (auto iter = range_index.begin(); iter != range_index.end(); ++iter) {
        casacore::Block<casacore::uInt> idx = iter->second;
        casacore::Block<casacore::Double> time_range(2);
        time_range[0] = time_list[idx[0]] * kDay2Sec
            - 0.5 * interval_column(idx[0]);
        time_range[1] = time_list[idx[1]] * kDay2Sec
            + 0.5 * interval_column(idx[1]);
        range[iter->first] = time_range;
      }
      time_range_[name] = range;
    }

//    for (auto i = time_range_.begin(); i != time_range_.end(); ++i) {
//      std::cout << "SRCNAME \"" << i->first << "\": " << std::endl;
//      for (auto j = i->second.begin(); j != i->second.end(); ++j) {
//        std::cout << "    " << j->first << ": " << j->second[0] << " "
//            << j->second[1] << std::endl;
//      }
//    }
  }

  virtual ~ScantableSourceIterator() {
  }

  void getEntry(sdfiller::SourceRecord &record) override {
    casacore::uInt const irow = row_list_[current_iter_];
    casacore::uInt const ifno = ifno_column_(irow);
    record.name = name_column_(irow);
    record.source_id = source_id_map_[record.name];
    record.spw_id = ifno;    //ifno_column_(irow);
    record.direction = casacore::MDirection(
        casacore::Quantum<casacore::Vector<casacore::Double> >(direction_column_(irow), "rad"),
        casacore::MDirection::J2000);
    record.proper_motion = proper_motion_column_(irow);
    casacore::uInt molecule_id = molecule_id_column_(irow);
    auto iter = molecule_id_map_.find(molecule_id);
    if (iter != molecule_id_map_.end()) {
      casacore::uInt jrow = iter->second;
      if (restfrequency_column_.isDefined(jrow)) {
        record.rest_frequency = restfrequency_column_(jrow);
      }
      if (molecule_name_column_.isDefined(jrow)) {
        record.transition = molecule_name_column_(jrow);
      }
    }
    // 2016/02/04 TN
    // comment out the following else block since if no ID is found in
    // molecule_id_map_ it indicates that there is no corresponding
    // entry in MOLECULES table for given MOLECULE_ID. Serch result is
    // always empty table.
//    else {
//      casacore::Table t = molecules_table_(molecules_table_.col("ID") == molecule_id, 1);
//      if (t.nrow() == 1) {
//        casacore::ArrayColumn<casacore::Double> rest_freq_column(t, "RESTFREQUENCY");
//        casacore::ArrayColumn<casacore::String> molecule_name_column(t, "NAME");
//        if (rest_freq_column.isDefined(0)) {
//          record.rest_frequency = rest_freq_column(0);
//        }
//        if (molecule_name_column.isDefined(0)) {
//          record.transition = molecule_name_column(0);
//        }
//      }
//    }
    casacore::Double sysvel = sysvel_column_(irow);
    record.num_lines = record.rest_frequency.size();
    record.sysvel = casacore::Vector < casacore::Double > (record.num_lines, sysvel);

//    casacore::Table t = main_table_(
//        main_table_.col("SRCNAME") == record.name
//            && main_table_.col("IFNO") == record.spw_id);
//    time_column_.attach(t, "TIME");
//    casacore::Vector < casacore::Double > time_list = time_column_.getColumn();
//    casacore::Sort sorter;
//    sorter.sortKey(time_list.data(), TpDouble);
//    casacore::Vector < casacore::uInt > index_vector;
//    casacore::uInt n = sorter.sort(index_vector, time_list.size());
//    interval_column_.attach(t, "INTERVAL");
//    constexpr double kDay2Sec = 86400.0;
//    casacore::Double time_min = time_list[index_vector[0]] * kDay2Sec
//        - 0.5 * interval_column_(index_vector[0]);
//    casacore::Double time_max = time_list[index_vector[n - 1]] * kDay2Sec
//        + 0.5 * interval_column_(index_vector[n - 1]);
    casacore::Block<casacore::Double> const &time_range = time_range_[record.name][ifno];
    casacore::Double time_min = time_range[0];
    casacore::Double time_max = time_range[1];
    record.time = 0.5 * (time_min + time_max);
    record.interval = (time_max - time_min);
  }
  void getProduct(Product */*p*/) override {

  }

private:
  casacore::ScalarColumn<casacore::String> name_column_;
  casacore::ArrayColumn<casacore::Double> direction_column_;
  casacore::ArrayColumn<casacore::Double> proper_motion_column_;
  casacore::ScalarColumn<casacore::Double> sysvel_column_;
  casacore::ScalarColumn<casacore::uInt> molecule_id_column_;
  casacore::ScalarColumn<casacore::uInt> ifno_column_;
  casacore::ScalarColumn<casacore::Double> time_column_;
  casacore::ScalarColumn<casacore::Double> interval_column_;
  casacore::ArrayColumn<casacore::Double> restfrequency_column_;
  casacore::ArrayColumn<casacore::String> molecule_name_column_;
  casacore::Table molecules_table_;
  casacore::Vector<casacore::uInt> row_list_;
  std::map<casacore::String, casacore::Int> source_id_map_;
  std::map<casacore::uInt, casacore::uInt> molecule_id_map_;
  std::map<casacore::String, std::map<casacore::uInt, casacore::Block<casacore::Double> > > time_range_;
};

} //# NAMESPACE CASA - END

#endif /* SINGLEDISH_FILLER_SCANTABLEITERATOR_H_ */
