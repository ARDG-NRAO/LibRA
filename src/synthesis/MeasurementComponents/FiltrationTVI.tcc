//# FiltrationTVI.tcc: Template class for data filtering TVI
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the Implied warranty of MERCHANTABILITY or
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
#ifndef _MSVIS_FILTRATIONTVI_TCC_
#define _MSVIS_FILTRATIONTVI_TCC_

#include <synthesis/MeasurementComponents/FiltrationTVI.h>

#include <climits>

#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/Arrays/Array.h>

#include <msvis/MSVis/VisibilityIterator2.h>

using namespace casacore;

namespace {
template<class T>
inline void FiltrateVector(Vector<T> const &feed,
    Vector<bool> const &is_filtrate, Vector<T> &filtrate) {
  AlwaysAssert(feed.nelements() == is_filtrate.nelements(), AipsError);
  // filter_flag: true --> filtrate, false --> residue
  auto const num_filtrates = ntrue(is_filtrate);
  if (num_filtrates == feed.nelements()) {
    filtrate.resize();
    filtrate.reference(feed);
  } else {
    filtrate.resize(ntrue(is_filtrate));
    Int k = 0;
    for (size_t i = 0; i < feed.nelements(); ++i) {
      if (is_filtrate[i]) {
        filtrate[k] = feed[i];
        ++k;
      }
    }
  }
}

//template<class T>
//inline void FiltrateMatrix(Matrix<T> const &feed,
//    Vector<bool> const &is_filtrate, Matrix<T> &filtrate) {
//  AlwaysAssert(feed.conform(is_filtrate), AipsError);
//  // filter_flag: true --> filtrate, false --> residue
//  filtrate.resize(ntrue(is_filtrate));
//  Int k = 0;
//  for (size_t i = 0; i < feed.nelements(); ++i) {
//    if (is_filtrate[i]) {
//      filtrate.column(k) = feed.column(i);
//      ++k;
//    }
//  }
//}
//
//template<class T>
//inline void FiltrateCube(Cube<T> const &feed, Vector<bool> const &is_filtrate,
//    Cube<T> &filtrate) {
//  AlwaysAssert(feed.conform(is_filtrate), AipsError);
//  // filter_flag: true --> filtrate, false --> residue
//  filtrate.resize(ntrue(is_filtrate));
//  Int k = 0;
//  for (size_t i = 0; i < feed.nelements(); ++i) {
//    if (is_filtrate[i]) {
//      filtrate.xyPlane(k) = feed.xyPlane(i);
//      ++k;
//    }
//  }
//}

template<class T>
inline void FiltrateArray(Array<T> const &feed, Vector<bool> const &is_filtrate,
    Array<T> &filtrate) {
  // filter_flag: true --> filtrate, false --> residue
  ssize_t const num_filtrates = ntrue(is_filtrate);
  IPosition shape = feed.shape();
  uInt const ndim = feed.ndim();
  if (num_filtrates == shape[ndim - 1]) {
    filtrate.resize();
    filtrate.reference(feed);
  } else {
    shape[ndim - 1] = num_filtrates;
    filtrate.resize(shape);
    AlwaysAssert((size_t )feed.shape()[ndim - 1] == is_filtrate.nelements(),
        AipsError);
    IPosition iter_axis(1, ndim - 1);
    ArrayIterator<T> from_iter(feed, iter_axis, False);
    ArrayIterator<T> to_iter(filtrate, iter_axis, False);
    for (size_t i = 0; i < is_filtrate.nelements(); ++i) {
      if (is_filtrate[i]) {
        to_iter.array() = from_iter.array();
        to_iter.next();
      }
      from_iter.next();
    }
  }
}

}

namespace casa { //# NAMESPACE CASA - BEGIN

namespace vi { //# NAMESPACE vi - BEGIN

// forward declaration
template<class Filter>
class FiltrationTVI;

// FiltrationTVI implementation
template<class Filter>
FiltrationTVI<Filter>::FiltrationTVI(ViImplementation2 * inputVi,
    Record const &configuration) :
    TransformingVi2(inputVi), configuration_p(configuration), filter_p(0), num_filtrates_p(
        0), is_filtrate_p(), is_valid_subchunk_p() {
  // new filter
//  MeasurementSet const &ms = getVii()->ms();
  filter_p = new Filter(configuration_p);

  // Initialize attached VisBuffer
  setVisBuffer(createAttachedVisBuffer(VbRekeyable));
}

template<class Filter>
FiltrationTVI<Filter>::~FiltrationTVI() {
  if (filter_p) {
    delete filter_p;
  }
}

template<class Filter>
void FiltrationTVI<Filter>::origin() {
  getVii()->origin();
//  cout << __func__ << ": subchunkId = " << getSubchunkId().subchunk() << endl;

  // Synchronize own VisBuffer -- is it required?
  //configureNewSubchunk();

  // filtration
  filter();

//  if (more()) {
//    cout << __func__ << ": there is a subchunk passed through the filter ";
//    Vector<uInt> rowIds;
//    getVii()->getRowIds(rowIds);
//    cout << " is_filtrate_p = " << is_filtrate_p << ", rowIds = " << rowIds << endl;
//  } else {
//    cout << __func__ << ": no subchunk remaining after filtration" << endl;
//    cout << __func__ << ": is_filtrate_p = " << is_filtrate_p << endl;
//  }
}

template<class Filter>
void FiltrationTVI<Filter>::next() {
  // next subchunk
  // must call next at least one time
  getVii()->next();

  // filtration
  filter();

//  cout << __func__ << ": subchunkId = " << getSubchunkId().subchunk() << endl;

//  if (more()) {
//    cout << __func__ << ": there is a subchunk passed through the filter ";
//    Vector<uInt> rowIds;
//    getVii()->getRowIds(rowIds);
//    cout << " is_filtrate_p = " << is_filtrate_p << ", rowIds = " << rowIds << endl;
//  } else {
//    cout << __func__ << ": no subchunk remaining after filtration" << endl;
//    cout << __func__ << ": is_filtrate_p = " << is_filtrate_p << endl;
//  }
}

template<class Filter>
void FiltrationTVI<Filter>::originChunks(Bool forceRewind) {
  TransformingVi2::originChunks(forceRewind);

  // sync
  filter_p->syncWith(this);

  filterChunk();

  getVii()->origin();

//  cout << __func__ << ": chunkId = " << getSubchunkId().chunk() << endl;
}

template<class Filter>
void FiltrationTVI<Filter>::nextChunk() {
  TransformingVi2::nextChunk();

  // sync
  filter_p->syncWith(this);

  filterChunk();

  getVii()->origin();

//  cout << __func__ << ": chunkId = " << getSubchunkId().chunk() << endl;
}

template<class Filter>
rownr_t FiltrationTVI<Filter>::nRows() const {
  return num_filtrates_p;
}

template<class Filter>
void FiltrationTVI<Filter>::getRowIds(Vector<rownr_t> &rowids) const {
  Vector<rownr_t> org;
  getVii()->getRowIds(org);
  ::FiltrateVector(org, is_filtrate_p, rowids);
}

template<class Filter>
void FiltrationTVI<Filter>::antenna1(Vector<Int> &ant1) const {
  Vector<Int> org;
  getVii()->antenna1(org);
  ::FiltrateVector(org, is_filtrate_p, ant1);
}

template<class Filter>
void FiltrationTVI<Filter>::antenna2(Vector<Int> &ant2) const {
  Vector<Int> org;
  getVii()->antenna2(org);
  ::FiltrateVector(org, is_filtrate_p, ant2);
}

template<class Filter>
void FiltrationTVI<Filter>::exposure(Vector<double> &expo) const {
  Vector<double> org;
  getVii()->exposure(org);
  ::FiltrateVector(org, is_filtrate_p, expo);
}

template<class Filter>
void FiltrationTVI<Filter>::feed1(Vector<Int> &fd1) const {
  Vector<Int> org;
  getVii()->feed1(org);
  ::FiltrateVector(org, is_filtrate_p, fd1);
}

template<class Filter>
void FiltrationTVI<Filter>::feed2(Vector<Int> &fd2) const {
  Vector<Int> org;
  getVii()->feed2(org);
  ::FiltrateVector(org, is_filtrate_p, fd2);
}

template<class Filter>
void FiltrationTVI<Filter>::fieldIds(Vector<Int> &fld) const {
  Vector<Int> org;
  getVii()->fieldIds(org);
  ::FiltrateVector(org, is_filtrate_p, fld);
}

template<class Filter>
void FiltrationTVI<Filter>::arrayIds(Vector<Int> &arr) const {
  Vector<Int> org;
  getVii()->arrayIds(org);
  ::FiltrateVector(org, is_filtrate_p, arr);
}

template<class Filter>
void FiltrationTVI<Filter>::flag(Cube<Bool> &flags) const {
  Cube<Bool> org;
  getVii()->flag(org);
  ::FiltrateArray(org, is_filtrate_p, flags);
}

template<class Filter>
void FiltrationTVI<Filter>::flag(Matrix<Bool> &flags) const {
  Matrix<Bool> org;
  getVii()->flag(org);
  ::FiltrateArray(org, is_filtrate_p, flags);
}

template<class Filter>
void FiltrationTVI<Filter>::flagCategory(Array<Bool> &flagCategories) const {
  Array<Bool> org;
  getVii()->flagCategory(org);
  ::FiltrateArray(org, is_filtrate_p, flagCategories);
}

template<class Filter>
void FiltrationTVI<Filter>::flagRow(Vector<Bool> &rowflags) const {
  Vector<Bool> org;
  getVii()->flagRow(org);
  ::FiltrateVector(org, is_filtrate_p, rowflags);
}

template<class Filter>
void FiltrationTVI<Filter>::observationId(Vector<Int> &obsids) const {
  Vector<Int> org;
  getVii()->observationId(org);
  ::FiltrateVector(org, is_filtrate_p, obsids);
}

template<class Filter>
void FiltrationTVI<Filter>::processorId(Vector<Int> &procids) const {
  Vector<Int> org;
  getVii()->processorId(org);
  ::FiltrateVector(org, is_filtrate_p, procids);
}

template<class Filter>
void FiltrationTVI<Filter>::scan(Vector<Int> &scans) const {
  Vector<Int> org;
  getVii()->scan(org);
  ::FiltrateVector(org, is_filtrate_p, scans);
}

template<class Filter>
void FiltrationTVI<Filter>::stateId(Vector<Int> &stateids) const {
  Vector<Int> org;
  getVii()->stateId(org);
  ::FiltrateVector(org, is_filtrate_p, stateids);
}

template<class Filter>
void FiltrationTVI<Filter>::jonesC(
    Vector<SquareMatrix<Complex, 2> > &cjones) const {
  Vector<SquareMatrix<Complex, 2>> org;
  getVii()->jonesC(org);
  ::FiltrateVector(org, is_filtrate_p, cjones);
}

template<class Filter>
void FiltrationTVI<Filter>::sigma(Matrix<Float> &sigmat) const {
  Matrix<Float> org;
  getVii()->sigma(org);
  ::FiltrateArray(org, is_filtrate_p, sigmat);
}

template<class Filter>
void FiltrationTVI<Filter>::spectralWindows(Vector<Int> &spws) const {
  Vector<Int> org;
  getVii()->spectralWindows(org);
  ::FiltrateVector(org, is_filtrate_p, spws);
}

template<class Filter>
void FiltrationTVI<Filter>::time(Vector<double> &t) const {
  Vector<double> org;
  getVii()->time(org);
  ::FiltrateVector(org, is_filtrate_p, t);
}

template<class Filter>
void FiltrationTVI<Filter>::timeCentroid(Vector<double> &t) const {
  Vector<double> org;
  getVii()->timeCentroid(org);
  ::FiltrateVector(org, is_filtrate_p, t);
}

template<class Filter>
void FiltrationTVI<Filter>::timeInterval(Vector<double> &ti) const {
  Vector<double> org;
  getVii()->timeInterval(org);
  ::FiltrateVector(org, is_filtrate_p, ti);
}

template<class Filter>
void FiltrationTVI<Filter>::uvw(Matrix<double> &uvwmat) const {
  Matrix<double> org;
  getVii()->uvw(org);
  ::FiltrateArray(org, is_filtrate_p, uvwmat);
}

template<class Filter>
void FiltrationTVI<Filter>::visibilityCorrected(Cube<Complex> &vis) const {
  Cube<Complex> org;
  getVii()->visibilityCorrected(org);
  ::FiltrateArray(org, is_filtrate_p, vis);
}

template<class Filter>
void FiltrationTVI<Filter>::visibilityModel(Cube<Complex> &vis) const {
  Cube<Complex> org;
  getVii()->visibilityModel(org);
  ::FiltrateArray(org, is_filtrate_p, vis);
}

template<class Filter>
void FiltrationTVI<Filter>::visibilityObserved(Cube<Complex> &vis) const {
  Cube<Complex> org;
  getVii()->visibilityObserved(org);
  ::FiltrateArray(org, is_filtrate_p, vis);
}

template<class Filter>
void FiltrationTVI<Filter>::floatData(Cube<Float> &fcube) const {
  Cube<Float> org;
  getVii()->floatData(org);
  ::FiltrateArray(org, is_filtrate_p, fcube);
}

template<class Filter>
IPosition FiltrationTVI<Filter>::visibilityShape() const {
  IPosition shape = getVii()->visibilityShape();
  shape[shape.nelements() - 1] = num_filtrates_p;
  return shape;
}

template<class Filter>
void FiltrationTVI<Filter>::weight(Matrix<Float> &wtmat) const {
  Matrix<Float> org;
  getVii()->weight(org);
  ::FiltrateArray(org, is_filtrate_p, wtmat);
}

template<class Filter>
void FiltrationTVI<Filter>::weightSpectrum(Cube<Float> &wtsp) const {
  Cube<Float> org;
  getVii()->weightSpectrum(org);
  ::FiltrateArray(org, is_filtrate_p, wtsp);
}

template<class Filter>
void FiltrationTVI<Filter>::sigmaSpectrum(Cube<Float> &sigmasp) const {
  Cube<Float> org;
  getVii()->sigmaSpectrum(org);
  ::FiltrateArray(org, is_filtrate_p, sigmasp);
}

template<class Filter>
void FiltrationTVI<Filter>::dataDescriptionIds(Vector<Int> &ddids) const {
  Vector<Int> org;
  getVii()->dataDescriptionIds(org);
  ::FiltrateVector(org, is_filtrate_p, ddids);
}

template<class Filter>
void FiltrationTVI<Filter>::filter() {
  auto const vii = getVii();
  auto const vb = vii->getVisBuffer();
  for (; vii->more() && !is_valid_subchunk_p(vii->getSubchunkId().subchunk()); vii->next()) {
    // Synchronize own VisBuffer -- is it required to do inside the loop?
    //configureNewSubchunk();
  }

  // update filter information
  num_filtrates_p = filter_p->isFiltratePerRow(vb, is_filtrate_p);

  // Synchronize own VisBuffer
  if(vii->more())
    configureNewSubchunk();
}

template<class Filter>
void FiltrationTVI<Filter>::filterChunk() {
  size_t const block_size = max(getVii()->nRowsInChunk(), (size_t)100);
  if (is_valid_subchunk_p.nelements() < block_size) {
    is_valid_subchunk_p.resize(block_size);
  }
  is_valid_subchunk_p = false;

  // increment iterator while the chunk doesn't contain valid subchunk
  for (auto vii = getVii(); vii->moreChunks(); vii->nextChunk()) {
    is_valid_subchunk_p = false;
    for (vii->origin(); vii->more(); vii->next()) {
      size_t const subchunk_id = vii->getSubchunkId().subchunk();
      size_t const n = is_valid_subchunk_p.nelements();
      size_t m = n;
      while (m < subchunk_id) {
        is_valid_subchunk_p.resize(m + block_size);
        m = is_valid_subchunk_p.nelements();
      }
      is_valid_subchunk_p(Slice(n, m - n)) = false;
      is_valid_subchunk_p[subchunk_id] = filter_p->isFiltrate(vii->getVisBuffer());
//      cout << __func__ << ": chunk " << vii->getSubchunkId().chunk() << " subchunk " << subchunk_id
//          << " is_valid " << is_valid_subchunk_p[subchunk_id] << endl;
    }

    if (anyTrue(is_valid_subchunk_p)) {
      break;
    }
  }

}

} //# NAMESPACE vi - END

} //# NAMESPACE CASA - END

#endif // _MSVIS_FILTRATIONTVI_TCC_
