/*
 * DataAccumulator.h
 *
 *  Created on: Jan 18, 2016
 *      Author: nakazato
 */

#ifndef SINGLEDISH_FILLER_DATAACCUMULATOR_H_
#define SINGLEDISH_FILLER_DATAACCUMULATOR_H_

#include <vector>
#include <cassert>
#include <memory>
#include <algorithm>
#include <numeric>
#include <functional>

#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/IO/ArrayIO.h>
#include <casacore/casa/Containers/Record.h>

#include <casacore/measures/Measures/Stokes.h>

#include <casacore/tables/Tables/TableRecord.h>
#include <singledishfiller/Filler/DataRecord.h>
#include <singledishfiller/Filler/FillerUtil.h>

using namespace casacore;

namespace {
template<class T>
inline void resizeTo(T &array, casacore::IPosition const &shape) {
    if (array.shape() != shape) {
        array.resize(shape, false);
    }
}

template<class T>
inline void setValue1(ssize_t n, T const *src, T *dst) {
    for (ssize_t i = 0; i < n; ++i) {
        dst[i] = src[i];
    }
}

template<class T>
inline void setValueToMatrixColumn(casacore::Vector<T> const &src,
        ssize_t icolumn, casacore::Matrix<T> &dst) {
    casacore::IPosition const &shape = dst.shape();
    ssize_t const nrow = shape[0];
    ssize_t const ncolumn = shape[1];
    if (icolumn >= ncolumn) {
        throw casacore::AipsError("Specified column doesn't exist.");
    }

    casacore::Bool b1, b2;
    T *dst_p = dst.getStorage(b1);
    T *work_p = dst_p + icolumn * nrow;
    T const *src_p = src.getStorage(b2);

    setValue1(nrow, src_p, work_p);

    src.freeStorage(src_p, b2);
    dst.putStorage(dst_p, b1);
}

template<class T, class Executor>
inline void shuffleTransposeMatrix(ssize_t n, size_t offset_src,
        casacore::Matrix<T> const &src, casacore::Matrix<T> &dst,
        std::vector<size_t> src_order = { }) {
    if (offset_src > src.ncolumn() - 1)
        throw casacore::AipsError("offset too large");
    casacore::Bool b1, b2;
    T const *src_p = src.getStorage(b1);
    T *dst_p = dst.getStorage(b2);
    T const *wsrc_p = src_p + offset_src * n;
    T *wdst_p = dst_p;

    Executor::execute(n, wsrc_p, wdst_p, src_order);

    src.freeStorage(src_p, b1);
    dst.putStorage(dst_p, b2);
}

inline void getDefaultOrder(size_t const num_order,
        std::vector<size_t> &order) {
    order.resize(num_order);
    std::iota(order.begin(), order.end(), 0);
}

inline bool setAndCheckOrder(size_t const required_size, size_t const max_value,
        std::vector<size_t> &order) {
    if (order.size() == 0)
        getDefaultOrder(required_size, order);

    if (order.size() < required_size) {
        return false;
    }
    size_t max_idx = order[0];
    for (size_t i = 1; i < order.size(); ++i) {
        max_idx = std::max(max_idx, order[i]);
    }
    return max_idx <= max_value;
}

struct ExecuteMatrix1 {
    template<class T>
    static void execute(ssize_t n, T const *src, T *dst,
            std::vector<size_t> /*in_order*/) {
        setValue1(n, src, dst);
    }
};

struct ExecuteMatrix2 {
    template<class T>
    static void execute(ssize_t n, T const *src, T *dst,
            std::vector<size_t> src_order) {
        // need to set max_value=3 for 4 pol case
        if (!setAndCheckOrder(2, 3, src_order))
            throw casacore::AipsError("got invalid order list");
        T const *row0_p = src + src_order[0] * n;
        T const *row1_p = src + src_order[1] * n;
        for (ssize_t i = 0; i < n; ++i) {
            dst[2 * i] = row0_p[i];
            dst[2 * i + 1] = row1_p[i];
        }
    }
};

struct ExecuteMatrix4X {
    template<class T>
    static void execute(ssize_t /*n*/, T const */*src*/, T */*dst*/,
            std::vector<size_t> /*in_order*/) {
        throw std::runtime_error("");
    }
};

// polarization order assumption (auto, cross, cross, auto)
template<>
inline void ExecuteMatrix4X::execute(ssize_t n, casacore::Bool const *src,
        casacore::Bool *dst, std::vector<size_t> src_order) {
    if (!setAndCheckOrder(4, 3, src_order))
        throw casacore::AipsError("got invalid order list");
    casacore::Bool const *row0_p = src + src_order[0] * n;
    casacore::Bool const *row1_p = src + src_order[1] * n;
    casacore::Bool const *row2_p = src + src_order[2] * n;
    casacore::Bool const *row3_p = src + src_order[3] * n;
    for (ssize_t i = 0; i < n; ++i) {
        dst[4 * i + 0] = row0_p[i];
        casacore::Bool b = row1_p[i] || row2_p[i];
        dst[4 * i + 1] = b;
        dst[4 * i + 2] = b;
        dst[4 * i + 3] = row3_p[i];
    }
}

struct ExecuteMatrix4 {
    template<class T>
    static void execute(ssize_t n, T const *src, T *dst,
            std::vector<size_t> src_order) {
        if (!setAndCheckOrder(4, 3, src_order))
            throw casacore::AipsError("got invalid order list");
        T const *row0_p = src + src_order[0] * n;
        T const *row1_p = src + src_order[1] * n;
        T const *row2_p = src + src_order[2] * n;
        T const *row3_p = src + src_order[3] * n;
        for (ssize_t i = 0; i < n; ++i) {
            dst[4 * i + 0] = row0_p[i];
            dst[4 * i + 1] = row1_p[i];
            dst[4 * i + 2] = row2_p[i];
            dst[4 * i + 3] = row3_p[i];
        }
    }
};

inline void shuffleTransposeMatrix4F2C(ssize_t n,
        casacore::Matrix<casacore::Float> const &src,
        casacore::Matrix<casacore::Complex> &dst,
        std::vector<size_t> src_order = { }) {
    if (!setAndCheckOrder(4, 3, src_order))
        throw casacore::AipsError("got invalid order list");
    casacore::Bool b1, b2;
    casacore::Float const *src_p = src.getStorage(b1);
    casacore::Complex *dst_p = dst.getStorage(b2);

    // polarization order assumption (auto, cross, cross, auto)
    casacore::Float const *row0_p = src_p + src_order[0] * n;
    casacore::Float const *row1_p = src_p + src_order[1] * n;
    casacore::Float const *row2_p = src_p + src_order[2] * n;
    casacore::Float const *row3_p = src_p + src_order[3] * n;
    for (ssize_t i = 0; i < n; ++i) {
        dst_p[4 * i].real(row0_p[i]);
        dst_p[4 * i].imag(0.0f);
        casacore::Float fr = row1_p[i];
        casacore::Float fi = row2_p[i];
        dst_p[4 * i + 1].real(fr);
        dst_p[4 * i + 1].imag(fi);
        dst_p[4 * i + 2].real(fr);
        dst_p[4 * i + 2].imag(-fi);
        dst_p[4 * i + 3].real(row3_p[i]);
        dst_p[4 * i + 3].imag(0.0f);
    }

    src.freeStorage(src_p, b1);
    dst.putStorage(dst_p, b2);
}

////// in-place reorder of MatrixColumn
//template<class T>
//inline void suffleMatrixColumn(casacore::Matrix<T> const &src,
//    std::vector<size_t> order) {
//    ssize_t n = src.nrow();
//    if (checkOrder(1, src.ncolumn()-1, order)) throw AipsError("Invalid order");
//    casacore::Matrix<T> const temp = src;
//  casacore::Bool b1, b2;
//  T const *src_p = src.getStorage(b1);
//  T const *temp_p = temp.getStorage(b2);
//  if (src_p==temp_p) {
//      throw casacore::AipsError("Failed to generate temp storage");
//  }
//  for (ssize_t j = 0; j < order.size(); ++j) {
//      setValue1(n, temp_p[order[j]*n], src_p[j*n]);
//  }
//  src.putStorage(src_p, b1);
//  temp.freeStorage(temp_p, b2);
//}

// get a vector of indices (out_idx) to sort in_data in accending order
template<typename T>
inline void getSortIndex(casacore::Vector<T> in_data,
        std::vector<size_t> &out_idx) {
    casacore::Vector<size_t> idx_vec(in_data.size());
    casacore::indgen(idx_vec);
    idx_vec.tovector(out_idx);

    std::sort(out_idx.begin(), out_idx.end(),
            [&in_data](size_t const &i, size_t const &j) -> bool {return in_data[i] < in_data[j];});
}
}

namespace casa { //# NAMESPACE CASA - BEGIN
namespace sdfiller { //# NAMESPACE SDFILLER - BEGIN

class DataAccumulator;

class DataChunk {
public:
    friend DataAccumulator;

    DataChunk(casacore::String const &poltype) :
            num_pol_max_(4), num_chan_(0), data_(), flag_(), flag_row_(
                    num_pol_max_, false), tsys_(), tcal_(), weight_(
                    num_pol_max_, 1.0f), sigma_(weight_), poltype_(poltype), valid_pcorr_(), pcorr_type_(), get_chunk_(
                    nullptr), get_num_pol_(nullptr) {
        POST_START;

        setPolType(poltype);

        POST_END;
    }

    virtual ~DataChunk() {
    }

    casacore::String getPolType() const {
        return poltype_;
    }

    void resetPolType(casacore::String const &poltype) {
        initialize(num_chan_);
        setPolType(poltype);
    }

    casacore::uInt getNumPol() const {
        return get_num_pol_();
    }

    void initialize(size_t num_chan) {
        num_chan_ = num_chan;
        casacore::IPosition const shape(2, num_chan_, num_pol_max_);
        ::resizeTo(data_, shape);
        ::resizeTo(flag_, shape);
        ::resizeTo(tsys_, shape);
        ::resizeTo(tcal_, shape);
        pcorr_type_.resize(0);
        tsys_ = -1.0f;
        tcal_ = -1.0f;
    }

    void clear() {
        num_chan_ = 0;
        pcorr_type_.resize(0);
    }

    bool readyToWrite() {
        return true;
    }

    bool accumulate(DataRecord const &record) {
        POST_START;

        if (!isValidRecord(record)) {
            return false;
        }

        if (num_pol_max_ < pcorr_type_.size()) {
            return false;
        }

        casacore::Vector<casacore::Float> const &data = record.data;
        if (num_chan_ == 0) {
            size_t num_chan = data.size();
            initialize(num_chan);
        }

        // Polarization
        casacore::Stokes::StokesTypes pcorr = record.pol;
        // check for consistency between pol_type and stokes
        bool consistent = false;
        for (size_t i = 0; i < valid_pcorr_.size(); ++i) {
            consistent |= (valid_pcorr_[i] == (Int) pcorr);
        }
        if (!consistent) {
            throw casacore::AipsError(
                    "Got inconsistent polarization with poltype");
        }
        // check if pcorr is the new corr type to be added
        for (size_t i = 0; i < pcorr_type_.size(); ++i) {
            if (pcorr == pcorr_type_[i]) {
                throw casacore::AipsError(
                        "DataChunk already has polarization "
                                + casacore::Stokes::name(pcorr));
            }
        }
        // new polarization type
        pcorr_type_.push_back(pcorr);
        // Matrices in DataRecord is stacked in the order of accumulation
        size_t const colid = pcorr_type_.size() - 1;

        casacore::Vector<casacore::Bool> const &flag = record.flag;
        casacore::Bool flagrow = record.flag_row;

        if (data.shape() != flag.shape()) {
            return false;
        }

        casacore::Vector<casacore::Float> tsys;
        if (!record.tsys.empty()) {
//      std::cout << "tsys is not empty: " << record.tsys[0] << std::endl;
            tsys.assign(record.tsys);
        }
        casacore::Vector<casacore::Float> tcal;
        if (!record.tcal.empty()) {
//      std::cout << "tcal is not empty: " << record.tcal << std::endl;
            tcal.assign(record.tcal);
        }

        if (data.nelements() != num_chan_) {
            return false;
        }

        // Filling Matrices[chan, pol] in DataRecord. The Matrices are column (channel) major
        //data_.column(colid) = data;
        ::setValueToMatrixColumn(data, colid, data_);
        //flag_.column(colid) = flag;
        ::setValueToMatrixColumn(flag, colid, flag_);
        flag_row_[colid] = flagrow;
        if (tsys.size() == num_chan_) {
            //tsys_.column(polid) = tsys;
            ::setValueToMatrixColumn(tsys, colid, tsys_);
        } else if (!tsys.empty()) {
            tsys_(0, colid) = tsys[0];
        }
        if (tcal.size() == num_chan_) {
            //tcal_.column(polid) = tcal;
            ::setValueToMatrixColumn(tcal, colid, tcal_);
        } else if (!tcal.empty()) {
            tcal_(0, colid) = tcal[0];
        }

        return true;
    }

    bool get(MSDataRecord &record) {
        bool return_value = get_chunk_(record);
        return return_value;
    }

private:
    // Functions to return if accumulated corr types are conformant set, e.g.,
    // liner polarization should have XX and YY for dual pol.
    // assumption: pcorr_type_ has StokesTypes consistent with poltype_ and
    // not redundant (assured by accumulate())
    bool isFullPol() const {
        return pcorr_type_.size() == 4;
    }
    bool isDualPol() const {
        if (pcorr_type_.size() != 2)
            return false;
        casacore::Int pcorr0 = valid_pcorr_[0];
        casacore::Int pcorr1 = valid_pcorr_[valid_pcorr_.size() - 1];
        return (pcorr_type_[0] == pcorr0 && pcorr_type_[1] == pcorr1)
                || (pcorr_type_[0] == pcorr1 && pcorr_type_[1] == pcorr0);
    }
    bool isSinglePol0() const {
        return (pcorr_type_.size() == 1
                && valid_pcorr_[0] == (Int) pcorr_type_[0]);
    }
    bool isSinglePol1() const {
        return (pcorr_type_.size() == 1
                && valid_pcorr_[valid_pcorr_.size() - 1] == (Int) pcorr_type_[0]);
    }
    bool isValidRecord(DataRecord const &record) {
        return !record.data.empty() && !record.flag.empty();
    }
    void setPolType(casacore::String const &poltype) {
        POST_START;

        poltype_ = poltype;
        if (poltype_ == "linear") {
            get_chunk_ = [&](MSDataRecord &r) {return DataChunk::getLinear(r);};
            get_num_pol_ = [&]() {return DataChunk::getNumPolLinear();};
            valid_pcorr_.resize(4);
            valid_pcorr_[0] = casacore::Stokes::XX;
            valid_pcorr_[1] = casacore::Stokes::XY;
            valid_pcorr_[2] = casacore::Stokes::YX;
            valid_pcorr_[3] = casacore::Stokes::YY;
        } else if (poltype_ == "circular") {
            get_chunk_ = [&](MSDataRecord &r) {return DataChunk::getCircular(r);};
            get_num_pol_ = [&]() {return DataChunk::getNumPolCircular();};
            valid_pcorr_.resize(4);
            valid_pcorr_[0] = casacore::Stokes::RR;
            valid_pcorr_[1] = casacore::Stokes::RL;
            valid_pcorr_[2] = casacore::Stokes::LR;
            valid_pcorr_[3] = casacore::Stokes::LL;
        } else if (poltype_ == "stokes") {
            get_chunk_ = [&](MSDataRecord &r) {return DataChunk::getStokes(r);};
            get_num_pol_ = [&]() {return DataChunk::getNumPolStokes();};
            valid_pcorr_.resize(4);
            valid_pcorr_[0] = casacore::Stokes::I;
            valid_pcorr_[1] = casacore::Stokes::Q;
            valid_pcorr_[2] = casacore::Stokes::U;
            valid_pcorr_[3] = casacore::Stokes::V;
        } else if (poltype_ == "linpol") {
            get_chunk_ = [&](MSDataRecord &r) {return DataChunk::getLinpol(r);};
            get_num_pol_ = [&]() {return DataChunk::getNumPolLinpol();};
            valid_pcorr_.resize(2);
            valid_pcorr_[0] = casacore::Stokes::Plinear;
            valid_pcorr_[1] = casacore::Stokes::Pangle;
        } else {
            throw casacore::AipsError(
                    casacore::String("Invalid poltype") + poltype);
        }

        POST_END;
    }
    size_t const num_pol_max_;
    size_t num_chan_;
    casacore::Matrix<casacore::Float> data_;
    casacore::Matrix<casacore::Bool> flag_;
    casacore::Vector<casacore::Bool> flag_row_;
    casacore::Matrix<casacore::Float> tsys_;
    casacore::Matrix<casacore::Float> tcal_;
    casacore::Vector<casacore::Float> weight_;
    casacore::Vector<casacore::Float> sigma_;

    casacore::String poltype_;
    casacore::Vector<casacore::Int> valid_pcorr_;
    std::vector<casacore::Stokes::StokesTypes> pcorr_type_;
    std::function<bool(MSDataRecord &)> get_chunk_;
    std::function<casacore::uInt()> get_num_pol_;

    // Tsys and Tcal assignment for 2 & 4 pols. Auto-correlation components of pol should be used.
    void setTsys2(MSDataRecord &record, std::vector<size_t> order = { }) {
        // safeguard to avoid processing empty array
        if (tsys_.empty()) {
            return;
        }

        assert(tsys_.ncolumn() == num_pol_max_);
        if (!setAndCheckOrder(2, tsys_.ncolumn() - 1, order)) {
            throw casacore::AipsError("got invalid order list");
        }
        size_t apol0 = order[0], apol1 = order[order.size() - 1];

        // clear Tsys
        record.setTsysSize(0, 0);

        if (num_chan_ == 1) {
            record.setTsysSize(2, 1);
            record.tsys(0, 0) = tsys_(0, apol0);
            record.tsys(1, 0) = tsys_(0, apol1);
        } else if (casacore::anyGT(tsys_, 0.0f)) {
            // valid spectral Tsys data are available
            casacore::IPosition const startpos0(2, 1, apol0);
            casacore::IPosition const endpos0(2, num_chan_ - 1, apol0);
            casacore::IPosition const startpos1(2, 1, apol1);
            casacore::IPosition const endpos1(2, num_chan_ - 1, apol1);
            if (casacore::anyGT(tsys_(startpos0, endpos0), 0.0f)
                || casacore::anyGT(tsys_(startpos1, endpos1), 0.0f)) {
                // spectral Tsys
                record.setTsysSize(2, num_chan_);
                shuffleTransposeMatrix<casacore::Float, ExecuteMatrix2>(
                        num_chan_, 0, tsys_, record.tsys, { apol0, apol1 });
            } else {
                // valid Tsys data are only for channel 0
                // so treat as scalar Tsys
                record.setTsysSize(2, 1);
                record.tsys(0, 0) = tsys_(0, apol0);
                record.tsys(1, 0) = tsys_(0, apol1);
            }
        }
    }

    void setTcal2(MSDataRecord &record, std::vector<size_t> order = { }) {
        // safeguard to avoid processing empty array
        if (tcal_.empty()) {
            return;
        }

        assert(tcal_.ncolumn() == num_pol_max_);
        if (!setAndCheckOrder(2, tcal_.ncolumn()-1, order)) {
            throw casacore::AipsError("got invalid order list");
        }
        size_t apol0 = order[0], apol1 = order[order.size() - 1];

        // clear Tcal
        record.setTcalSize(0, 0);

        if (num_chan_ == 1) {
            record.setTcalSize(2, 1);
            record.tcal(0, 0) = tcal_(0, apol0);
            record.tcal(1, 0) = tcal_(0, apol1);
        } else if (casacore::anyGT(tcal_, 0.0f)) {
            // valid spectral Tcal data are available
            casacore::IPosition const startpos0(2, 1, apol0);
            casacore::IPosition const endpos0(2, num_chan_ - 1, apol0);
            casacore::IPosition const startpos1(2, 1, apol1);
            casacore::IPosition const endpos1(2, num_chan_ - 1, apol1);
            if (casacore::anyGT(tcal_(startpos0, endpos0), 0.0f)
                || casacore::anyGT(tcal_(startpos1, endpos1), 0.0f)) {
                // spectral Tcal
                record.setTcalSize(2, num_chan_);
                shuffleTransposeMatrix<casacore::Float, ExecuteMatrix2>(
                        num_chan_, 0, tcal_, record.tcal, { apol0, apol1 });
            } else {
                // valid Tcal data are only for channel 0
                // so treat as scalar Tcal
                record.setTcalSize(2, 1);
                record.tcal(0, 0) = tcal_(0, apol0);
                record.tcal(1, 0) = tcal_(0, apol1);
            }
        }
    }

    void setTsys1(MSDataRecord &record, ssize_t start_src) {
        if (tsys_.empty()) {
            // no Tsys data. do nothing.
            return;
        } else if (num_chan_ == 1) {
            record.setTsysSize(1, 1);
            record.tsys(0, 0) = tsys_(0, start_src);
        } else if (casacore::anyGT(tsys_, 0.0f)) {
            casacore::IPosition const startpos(2, 1, start_src);
            casacore::IPosition const endpos(2, num_chan_ - 1, start_src);
            if (casacore::anyGT(tsys_(startpos, endpos), 0.0f)) {
                // should be spectral Tsys
                record.setTsysSize(1, num_chan_);
                shuffleTransposeMatrix<casacore::Float, ExecuteMatrix1>(num_chan_,
                    start_src, tsys_, record.tsys);
            } else {
                // valid Tsys data are only for channel 0
                // so treat as scalar Tsys
                record.setTsysSize(1, 1);
                record.tsys(0, 0) = tsys_(0, start_src);
            }
        }
    }

    void setTcal1(MSDataRecord &record, ssize_t start_src) {
        if (tcal_.empty()) {
            // no Tcal data. do nothing.
            return;
        } else if (num_chan_ == 1) {
            record.setTcalSize(1, 1);
            record.tcal(0, 0) = tcal_(0, start_src);
        } else if (casacore::anyGT(tcal_, 0.0f)) {
            casacore::IPosition const startpos(2, 1, start_src);
            casacore::IPosition const endpos(2, num_chan_ - 1, start_src);
            if (casacore::anyGT(tcal_(startpos, endpos), 0.0f)) {
                // should be spectral Tcal
                record.setTcalSize(1, num_chan_);
                shuffleTransposeMatrix<casacore::Float, ExecuteMatrix1>(num_chan_,
                    start_src, tcal_, record.tcal);
            } else {
                // valid Tcal data are only for channel 0
                // so treat as scalar Tcal
                record.setTcalSize(1, 1);
                record.tcal(0, 0) = tcal_(0, start_src);
            }
        }
    }

    bool getLinear(MSDataRecord &record) {
        POST_START;

        casacore::Vector<casacore::Int> const col_stokes(pcorr_type_);
        std::vector<size_t> pol_idx_order;
        getSortIndex(col_stokes, pol_idx_order);

        casacore::Vector<casacore::Float> weight;
        casacore::Vector<casacore::Float> sigma;
        if (isFullPol()) {
            // POL 0, 1, 2, and 3
//      std::cout << "set data/flag" << std::endl;
            record.setComplex();
            record.setDataSize(4, num_chan_);
            shuffleTransposeMatrix4F2C(num_chan_, data_, record.complex_data,
                    pol_idx_order);
            shuffleTransposeMatrix<casacore::Bool, ExecuteMatrix4X>(num_chan_,
                    0, flag_, record.flag, pol_idx_order);
            record.flag_row = anyEQ(flag_row_, true);
//      std::cout << "weight = " << record.weight << std::endl;

//      std::cout << "set tsys" << std::endl;
            setTsys2(record, pol_idx_order);

//      std::cout << "set tcal " << tcal_ << std::endl;
            setTcal2(record, pol_idx_order);

            record.num_pol = 4;
        } else if (isDualPol()) {
            // POL 0 and 1
//      std::cout << "set data/flag" << std::endl;
            record.setFloat();
            record.setDataSize(2, num_chan_);
            shuffleTransposeMatrix<casacore::Float, ExecuteMatrix2>(num_chan_,
                    0, data_, record.float_data, pol_idx_order);
            shuffleTransposeMatrix<casacore::Bool, ExecuteMatrix2>(num_chan_, 0,
                    flag_, record.flag, pol_idx_order);
            record.flag_row = flag_row_[0] || flag_row_[1];
//      std::cout << "weight = " << record.weight << std::endl;

//      std::cout << "set tsys" << std::endl;
            setTsys2(record, pol_idx_order);

//      std::cout << "set tcal " << tcal_ << std::endl;
            setTcal2(record, pol_idx_order);

            record.num_pol = 2;
        } else if (isSinglePol0()) {
            // only POL 0
//      std::cout << "set data/flag (pol 0)" << std::endl;
            record.setFloat();
            record.setDataSize(1, num_chan_);
            shuffleTransposeMatrix<casacore::Float, ExecuteMatrix1>(num_chan_,
                    0, data_, record.float_data, pol_idx_order);
            shuffleTransposeMatrix<casacore::Bool, ExecuteMatrix1>(num_chan_, 0,
                    flag_, record.flag, pol_idx_order);
            record.flag_row = flag_row_(0);

            setTsys1(record, 0);

//      std::cout << "set tcal " << tcal_ << std::endl;
            setTcal1(record, 0);

            record.num_pol = 1;
        } else if (isSinglePol1()) {
            // only POL 1
//      std::cout << "set data/flag (pol 1)" << std::endl;
            record.setFloat();
            record.setDataSize(1, num_chan_);
            shuffleTransposeMatrix<casacore::Float, ExecuteMatrix1>(num_chan_,
                    0, data_, record.float_data, pol_idx_order);
            shuffleTransposeMatrix<casacore::Bool, ExecuteMatrix1>(num_chan_, 0,
                    flag_, record.flag, pol_idx_order);
            record.flag_row = flag_row_(1);

            setTsys1(record, 0);

//      std::cout << "set tcal " << tcal_ << std::endl;
            setTcal1(record, 0);

            record.num_pol = 1;
        } else if (pcorr_type_.size() == 0) {
//      std::cout << "DataChunk is not ready for get" << std::endl;
            return false;
        } else {// means strange combination of polarization types in DataChunk
            throw AipsError("non-conforming combination of polarization accumulated");
        }
        for (casacore::Int i = 0; i < record.num_pol; ++i) {
            record.corr_type[i] = col_stokes[pol_idx_order[i]];
        }

        POST_END;
        return true;
    }

    bool getCircular(MSDataRecord &record) {
        return getLinear(record);
    }

    bool getStokes(MSDataRecord &record) {
        POST_START;

        casacore::Vector<casacore::Int> const col_stokes(pcorr_type_);
        std::vector<size_t> pol_idx_order;
        getSortIndex(col_stokes, pol_idx_order);

        record.setFloat();
        if (isFullPol()) {
            record.setDataSize(4, num_chan_);
            shuffleTransposeMatrix<casacore::Float, ExecuteMatrix4>(num_chan_,
                    0, data_, record.float_data, pol_idx_order);
            shuffleTransposeMatrix<casacore::Bool, ExecuteMatrix4>(num_chan_, 0,
                    flag_, record.flag, pol_idx_order);
            record.flag_row = anyTrue(flag_row_);

            record.num_pol = 4;
        } else if (isSinglePol0()) {
            record.setDataSize(1, num_chan_);
            shuffleTransposeMatrix<casacore::Float, ExecuteMatrix1>(num_chan_,
                    0, data_, record.float_data, pol_idx_order);
            shuffleTransposeMatrix<casacore::Bool, ExecuteMatrix1>(num_chan_, 0,
                    flag_, record.flag, pol_idx_order);
            record.flag_row = flag_row_[0];

            record.num_pol = 1;
        } else if (pcorr_type_.size() == 0) {
            return false;
        } else {// means strange combination of polarization types in DataChunk
            throw AipsError("non-conforming combination of polarization accumulated");
        }
        for (casacore::Int i = 0; i < record.num_pol; ++i) {
            record.corr_type[i] = col_stokes[pol_idx_order[i]];
        }

        POST_END;
        return true;
    }

    bool getLinpol(MSDataRecord &record) {
        POST_START;

        casacore::Vector<casacore::Int> const col_stokes(pcorr_type_);
        std::vector<size_t> pol_idx_order;
        getSortIndex(col_stokes, pol_idx_order);

        record.setFloat();
        if (isDualPol()) {
            // POL 0 and 1
            record.setDataSize(2, num_chan_);
            shuffleTransposeMatrix<casacore::Float, ExecuteMatrix2>(num_chan_,
                    0, data_, record.float_data, pol_idx_order);
            shuffleTransposeMatrix<casacore::Bool, ExecuteMatrix2>(num_chan_, 0,
                    flag_, record.flag, pol_idx_order);
            record.flag_row = flag_row_[0] || flag_row_[1];
            record.num_pol = 2;
        } else if (isSinglePol0()) {
            record.setDataSize(1, num_chan_);
            shuffleTransposeMatrix<casacore::Float, ExecuteMatrix1>(num_chan_,
                    0, data_, record.float_data, pol_idx_order);
            shuffleTransposeMatrix<casacore::Bool, ExecuteMatrix1>(num_chan_, 0,
                    flag_, record.flag, pol_idx_order);
            record.flag_row = flag_row_[0];

            record.num_pol = 1;
        } else if (pcorr_type_.size() == 0) {
            return false;
        } else {// means strange combination of polarization types in DataChunk
            throw AipsError("non-conforming combination of polarization accumulated");
        }
        for (casacore::Int i = 0; i < record.num_pol; ++i) {
            record.corr_type[i] = col_stokes[pol_idx_order[i]];
        }

        POST_END;
        return true;
    }

    casacore::uInt getNumPolLinear() const {
        if (isFullPol()) {
            return 4;
        } else if (isDualPol()) {
            return 2;
        } else if (isSinglePol0() || isSinglePol1()) {
            return 1;
        } else {
            return 0;
        }
    }

    casacore::uInt getNumPolCircular() const {
        return getNumPolLinear();
    }

    casacore::uInt getNumPolStokes() const {
        if (isFullPol()) {
            return 4;
        } else if (isSinglePol0()) {
            return 1;
        } else {
            return 0;
        }
    }

    casacore::uInt getNumPolLinpol() const {
        if (isDualPol()) {
            return 2;
        } else if (isSinglePol0()) {
            return 1;
        } else {
            return 0;
        }
    }
};

class DataAccumulator {
private:
    struct DataAccumulatorKey {
        casacore::Int antenna_id;
        casacore::Int field_id;
        casacore::Int feed_id;
        casacore::Int spw_id;
        casacore::String pol_type;
        casacore::String intent;

        template<class T, class C>
        bool comp(T const &a, T const &b, C const &c) const {
            if (a < b) {
                return true;
            } else if (a == b) {
                return c();
            } else {
                return false;
            }
        }

        bool operator()(DataAccumulatorKey const &lhs,
                DataAccumulatorKey const &rhs) const {
            return comp(lhs.antenna_id, rhs.antenna_id,
                    [&]() {return comp(lhs.field_id, rhs.field_id,
                                [&]() {return comp(lhs.feed_id, rhs.feed_id,
                                            [&]() {return comp(lhs.spw_id, rhs.spw_id,
                                                        [&]() {return comp(lhs.pol_type, rhs.pol_type,
                                                                    [&]() {return comp(lhs.intent, rhs.intent,
                                                                                []() {return false;});});});});});});
        }
    };

public:
    DataAccumulator() :
            pool_(), antenna_id_(), spw_id_(), field_id_(), feed_id_(), scan_(), subscan_(), intent_(), direction_(), interval_(), indexer_(), time_(
                    -1.0), is_free_() {
    }

    virtual ~DataAccumulator() {
        POST_START;

        for (auto iter = pool_.begin(); iter != pool_.end(); ++iter) {
            delete *iter;
        }

        POST_END;
    }

    size_t getNumberOfChunks() const {
        return pool_.size();
    }

    size_t getNumberOfActiveChunks() const {
        return std::count_if(pool_.begin(), pool_.end(),
                [](DataChunk * const &c) {
                    return c->getNumPol() > 0;
                });
    }

    bool queryForGet(DataRecord const &record) const {
        casacore::Double const time = record.time;
        bool is_ready = (0.0 <= time_) && !(time_ == time);
        return is_ready;
    }

    bool queryForGet(casacore::Double const &time) const {
        bool is_ready = (0.0 <= time_) && !(time_ == time);
        return is_ready;
    }

    void clear() {
        for (auto iter = pool_.begin(); iter != pool_.end(); ++iter) {
            (*iter)->clear();
        }
        time_ = -1.0;
    }

    bool get(size_t ichunk, MSDataRecord &record) {
        POST_START;

        if (pool_.size() == 0) {
            return false;
        } else if (ichunk >= pool_.size()) {
            return false;
        }
        bool status = pool_[ichunk]->get(record);
//    std::cout << "get Chunk status = " << status << std::endl;
        if (!status) {
            record.clear();
            return status;
        }
        record.time = time_;
        record.pol_type = pool_[ichunk]->getPolType();
        record.antenna_id = antenna_id_[ichunk];
        record.spw_id = spw_id_[ichunk];
        record.field_id = field_id_[ichunk];
        record.feed_id = feed_id_[ichunk];
        record.scan = scan_[ichunk];
        record.subscan = subscan_[ichunk];
        record.intent = intent_[ichunk];
        record.direction = direction_[ichunk];
        record.interval = interval_[ichunk];
        record.temperature = temperature_[ichunk];
        record.pressure = pressure_[ichunk];
        record.rel_humidity = rel_humidity_[ichunk];
        record.wind_speed = wind_speed_[ichunk];
        record.wind_direction = wind_direction_[ichunk];

        POST_END;
        return status;
    }

    bool accumulate(DataRecord const &record) {
        POST_START;

        if (!isValidRecord(record)) {
//      std::cout << "record is not a valid one" << std::endl;
            return false;
        }

        casacore::Double time = record.time;
        if (time_ < 0.0) {
            time_ = time;
        }
        if (time_ != time) {
//      std::cout << "timestamp mismatch" << std::endl;
            return false;
        }
        casacore::Int antennaid = record.antenna_id;
        casacore::Int spwid = record.spw_id;
        casacore::Int fieldid = record.field_id;
        casacore::Int feedid = record.feed_id;
        casacore::Int scan = record.scan;
        casacore::Int subscan = record.subscan;
        casacore::String intent = record.intent;
        casacore::String poltype = record.pol_type;
        DataAccumulatorKey key;
        key.antenna_id = record.antenna_id;
        key.field_id = record.field_id;
        key.feed_id = record.feed_id;
        key.spw_id = record.spw_id;
        key.intent = record.intent;
        key.pol_type = record.pol_type;
        casacore::Matrix<casacore::Double> const &direction = record.direction;
        casacore::Double interval = record.interval;
        casacore::Float temperature = record.temperature;
        casacore::Float pressure = record.pressure;
        casacore::Float rel_humidity = record.rel_humidity;
        casacore::Float wind_speed = record.wind_speed;
        casacore::Float wind_direction = record.wind_direction;
        bool status = false;
        auto iter = indexer_.find(key);
//        std::cout << "(ant, spw, pol, pol_type, field, feed, intent) = ("
//                << key.antenna_id << ", " << key.spw_id << ", " << record.pol<< ", " << key.pol_type << ", " << key.field_id << ", " << key.feed_id
//                << ", " << key.intent << ", "
//                << std::endl;
        if (iter != indexer_.end()) {
            casacore::uInt index = iter->second;
            status = pool_[index]->accumulate(record);
            if (status) {
                antenna_id_[index] = antennaid;
                spw_id_[index] = spwid;
                field_id_[index] = fieldid;
                feed_id_[index] = feedid;
                scan_[index] = scan;
                subscan_[index] = subscan;
                intent_[index] = intent;
                direction_[index].assign(direction);
                interval_[index] = interval;
                temperature_[index] = temperature;
                pressure_[index] = pressure;
                rel_humidity_[index] = rel_humidity;
                wind_speed_[index] = wind_speed;
                wind_direction_[index] = wind_direction;
            }
        } else {
            pool_.push_back(new DataChunk(poltype));
            antenna_id_.push_back(-1);
            spw_id_.push_back(-1);
            field_id_.push_back(-1);
            feed_id_.push_back(-1);
            scan_.push_back(-1);
            subscan_.push_back(-1);
            intent_.push_back("");
            direction_.push_back(casacore::Matrix<casacore::Double>());
            interval_.push_back(-1.0);
            temperature_.push_back(0.0f);
            pressure_.push_back(0.0f);
            rel_humidity_.push_back(0.0f);
            wind_speed_.push_back(0.0f);
            wind_direction_.push_back(0.0f);
            casacore::uInt index = pool_.size() - 1;
            indexer_[key] = index;
            status = pool_[index]->accumulate(record);
            if (status) {
                antenna_id_[index] = antennaid;
                spw_id_[index] = spwid;
                field_id_[index] = fieldid;
                feed_id_[index] = feedid;
                scan_[index] = scan;
                subscan_[index] = subscan;
                intent_[index] = intent;
                direction_[index].assign(direction);
                interval_[index] = interval;
                temperature_[index] = temperature;
                pressure_[index] = pressure;
                rel_humidity_[index] = rel_humidity;
                wind_speed_[index] = wind_speed;
                wind_direction_[index] = wind_direction;
            }
        }

//    std::cout << "status = " << status << std::endl;
//    std::cout << "key (a" << key.antenna_id << ",f" << key.field_id << ",s"
//        << key.spw_id << ",i" << key.intent << ",p" << key.pol_type << ",d"
//        << key.feed_id << "(index " << indexer_[key] << "): TIME="
//        << time_ << " INTERVAL=" << interval << " polno=" << record.pol << std::endl;
        POST_END;
        return status;
    }

    casacore::String getPolType(size_t ichunk) const {
        assert(ichunk < pool_.size());
        return pool_[ichunk]->getPolType();
    }

    casacore::uInt getNumPol(size_t ichunk) const {
        assert(ichunk < pool_.size());
        return pool_[ichunk]->getNumPol();
    }

private:
    bool isValidRecord(DataRecord const &record) {
//    std::cout << record.time << " " << record.interval << " "
//        << record.antenna_id << " " << record.field_id << " " << record.feed_id
//        << " " << record.spw_id << " " << record.scan << " " << record.subscan
//        << " " << record.direction << std::endl;
        return record.time > 0.0 && record.interval > 0.0
                && record.antenna_id >= 0 && record.field_id >= 0
                && record.feed_id >= 0 && record.spw_id >= 0 && record.scan >= 0
                && record.subscan >= 0 && !record.direction.empty();
    }

    std::vector<DataChunk *> pool_;
    std::vector<casacore::Int> antenna_id_;
    std::vector<casacore::Int> spw_id_;
    std::vector<casacore::Int> field_id_;
    std::vector<casacore::Int> feed_id_;
    std::vector<casacore::Int> scan_;
    std::vector<casacore::Int> subscan_;
    std::vector<casacore::String> intent_;
    std::vector<casacore::Matrix<casacore::Double> > direction_;
    std::vector<casacore::Double> interval_;
    std::vector<casacore::Float> temperature_;
    std::vector<casacore::Float> pressure_;
    std::vector<casacore::Float> rel_humidity_;
    std::vector<casacore::Float> wind_speed_;
    std::vector<casacore::Float> wind_direction_;
    std::map<DataAccumulatorKey, casacore::uInt, DataAccumulatorKey> indexer_;
    casacore::Double time_;
    std::vector<bool> is_free_;
};

} //# NAMESPACE SDFILLER - END
} //# NAMESPACE CASA - END

#endif /* SINGLEDISH_FILLER_DATAACCUMULATOR_H_ */
