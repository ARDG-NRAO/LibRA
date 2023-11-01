//# PolAverageTVI.h: This file contains the implementation of the PolAverageTVI class.
//#
//#  CASA - Common Astronomy Software Applications (http://casa.nrao.edu/)
//#  Copyright (C) Associated Universities, Inc. Washington DC, USA 2011, All rights reserved.
//#  Copyright (C) European Southern Observatory, 2011, All rights reserved.
//#
//#  This library is free software; you can redistribute it and/or
//#  modify it under the terms of the GNU Lesser General Public
//#  License as published by the Free software Foundation; either
//#  version 2.1 of the License, or (at your option) any later version.
//#
//#  This library is distributed in the hope that it will be useful,
//#  but WITHOUT ANY WARRANTY, without even the implied warranty of
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//#  Lesser General Public License for more details.
//#
//#  You should have received a copy of the GNU Lesser General Public
//#  License along with this library; if not, write to the Free Software
//#  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
//#  MA 02111-1307  USA
//# $Id: $
#include <mstransform/TVI/PolAverageTVI.h>

#include <casacore/casa/Arrays/Cube.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Containers/Record.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/Arrays/ArrayIter.h>
#include <casacore/measures/Measures/Stokes.h>
#include <casacore/ms/MeasurementSets/MSDataDescColumns.h>
#include <casacore/ms/MeasurementSets/MSPolColumns.h>

#include <msvis/MSVis/VisBufferComponents2.h>
#include <msvis/MSVis/VisibilityIteratorImpl2.h>

#include <mstransform/TVI/UtilsTVI.h>

using namespace casacore;

namespace {
template<class T>
inline T replaceFlaggedDataWithZero(T v, Bool b) {
  return ((b) ? T(0) : v);
}

struct StokesTransformation {
public:
  template<class T>
  inline static void transformData(Cube<T> const &dataIn,
      Cube<Bool> const &flagIn, Int pid0, Int pid1, Cube<T> &dataOut) {
    if (dataIn.empty()) {
      dataOut.resize();
      return;
    }
    auto const cubeShape = dataIn.shape();
    IPosition const newShape(3, 1, cubeShape[1], cubeShape[2]);
//    size_t const npol = cubeShape[0];
    size_t const nchan = cubeShape[1];
    size_t const nrow = cubeShape[2];
    size_t const nelem = cubeShape[1] * cubeShape[2];
    Cube<T> transformedData(newShape, T(0));
    Cube<Float> weightSum(newShape, 0.0f);

    Int pols[] = { pid0, pid1 };
    for (size_t i = 0; i < 2; ++i) {
      Int ipol = pols[i];
      IPosition start(3, ipol, 0, 0);
      IPosition end(3, ipol, nchan - 1, nrow - 1);
      auto dslice = dataIn(start, end);
      auto fslice = flagIn(start, end);
      Array<Float> weight(dslice.shape());
      Array<T> weightedData(dslice.shape());
      arrayContTransform(dslice, fslice, weightedData,
          replaceFlaggedDataWithZero<T>);
      arrayContTransform(fslice, weight, [](Bool b) {
        return ((b) ? 0.0f : 1.0f);
      });
      transformedData += weightedData;
      weightSum += weight;
    }

    // transformedData, transformedFlag and nAccumulated should be contiguous array
    auto p_tdata = transformedData.data();
    auto p_wsum = weightSum.data();

    for (size_t i = 0; i < nelem; ++i) {
      if (p_wsum[i] > 0.0) {
        p_tdata[i] /= T(p_wsum[i]);
      }
    }

    dataOut.reference(transformedData);
  }

  static inline void AccumulateWeight(Float const wt, Double &wtsum) {
    wtsum += 1.0 / wt;
  }

  static inline void NormalizeWeight(Double const wtsum, Float &wt) {
    wt = 4.0 / wtsum;
  }
};

struct GeometricTransformation {
  template<class T>
  inline static void transformData(Cube<T> const &dataIn,
      Cube<Bool> const &flagIn, Cube<Float> const &weightIn, Int pid0, Int pid1,
      Cube<T> &dataOut) {
    if (dataIn.empty()) {
      dataOut.resize();
      return;
    }
    auto const cubeShape = dataIn.shape();
    IPosition const newShape(3, 1, cubeShape[1], cubeShape[2]);
    Cube<T> transformedData(newShape, T(0));
    Cube<Bool> transformedFlag(newShape, True);
    Cube<Float> weightSum(newShape, 0.0f);
//    size_t const npol = cubeShape[0];
    size_t const nchan = cubeShape[1];
    size_t const nrow = cubeShape[2];
    size_t const nelem = cubeShape[1] * cubeShape[2];

    Int pols[] = { pid0, pid1 };
    for (size_t i = 0; i < 2; ++i) {
      Int ipol = pols[i];
      IPosition start(3, ipol, 0, 0);
      IPosition end(3, ipol, nchan - 1, nrow - 1);
      auto const dslice = dataIn(start, end);
      auto const wslice = weightIn(start, end);
      auto const fslice = flagIn(start, end);
      Array<Float> weight(dslice.shape());
      Array<T> weightedData(dslice.shape());
      arrayContTransform(dslice * wslice, fslice, weightedData,
          replaceFlaggedDataWithZero<T>);
      arrayContTransform(wslice, fslice, weight,
          replaceFlaggedDataWithZero<Float>);
      transformedData += weightedData;
      weightSum += weight;
    }

    // transformedData, transformedFlag and nAccumulated should be contiguous array
    T *p_tdata = transformedData.data();
    Float *p_wsum = weightSum.data();
    for (size_t i = 0; i < nelem; ++i) {
      if (p_wsum[i] > 0.0) {
        p_tdata[i] /= T(p_wsum[i]);
      }
    }

    dataOut.reference(transformedData);
  }

  template<class T>
  inline static void transformData(Cube<T> const &dataIn,
      Cube<Bool> const &flagIn, Matrix<Float> const &weightIn, Int pid0,
      Int pid1, Cube<T> &dataOut) {
//    cout << "start " << __func__ << endl;
    if (dataIn.empty()) {
      dataOut.resize();
      return;
    }
    auto const cubeShape = dataIn.shape();
    IPosition const newShape(3, 1, cubeShape[1], cubeShape[2]);
    Cube<T> transformedData(newShape, T(0));
    Cube<Float> weightSum(newShape, 0.0f);
//    auto const npol = dataIn.shape()[0];
    auto const nchan = dataIn.shape()[1];
    auto const nrow = dataIn.shape()[2];
    auto const nelem = nchan * nrow;

    Int pols[] = { pid0, pid1 };
    for (ssize_t i = 0; i < 2; ++i) {
      Int ipol = pols[i];
      for (ssize_t j = 0; j < nrow; ++j) {
        IPosition start(3, ipol, 0, j);
        IPosition end(3, ipol, nchan - 1, j);
        auto dslice = dataIn(start, end);
        auto fslice = flagIn(start, end);
        auto w = weightIn(ipol, j);
        Array<Float> weight(dslice.shape());
        Array<T> weightedData(dslice.shape());
        arrayContTransform(dslice * w, fslice, weightedData,
            replaceFlaggedDataWithZero<T>);
        arrayContTransform(fslice, weight, [&w](Bool b) {
          return ((b) ? 0.0f: w);
        });
        IPosition tstart(3, 0, 0, j);
        IPosition tend(3, 0, nchan - 1, j);
        Array<T> tdSlice = transformedData(tstart, tend);
        Array<Float> twSlice = weightSum(tstart, tend);
        AlwaysAssert(tdSlice.conform(weightedData), AipsError);
        AlwaysAssert(twSlice.conform(weight), AipsError);
        tdSlice += weightedData;
        twSlice += weight;
      }
    }

    // transformedData, transformedFlag and nAccumulated should be contiguous array
    T *p_tdata = transformedData.data();
    Float *p_wsum = weightSum.data();

    for (ssize_t i = 0; i < nelem; ++i) {
      if (p_wsum[i] > 0.0) {
        p_tdata[i] /= T(p_wsum[i]);
      }
    }

    dataOut.reference(transformedData);
//    cout << "end " << __func__ << endl;
  }

  static inline void AccumulateWeight(Float const wt, Double &wtsum) {
    wtsum += wt;
  }

  static inline void NormalizeWeight(Double const wtsum, Float &wt) {
    wt = wtsum;
  }
};

inline Float weight2Sigma(Float x) {
  return 1.0 / sqrt(x);
}

template<class WeightHandler>
inline void transformWeight(Array<Float> const &weightIn, Int pid0, Int pid1,
    Array<Float> &weightOut) {
//  cout << "start " << __func__ << endl;
  if (weightIn.empty()) {
//    cout << "input weight is empty" << endl;
    weightOut.resize();
    return;
  }
  IPosition const shapeIn = weightIn.shape();
  IPosition shapeOut(shapeIn);
  // set length of polarization axis to 1
  shapeOut[0] = 1;
//  cout << "shapeIn = " << shapeIn << " shapeOut = " << shapeOut << endl;

  // initialization
  weightOut.resize(shapeOut);
  weightOut = 0.0f;

  ssize_t numPol = shapeIn[0];
  Int64 numElemPerPol = shapeOut.product();
//  cout << "numElemPerPol = " << numElemPerPol << endl;
//  cout << "numPol = " << numPol << endl;

  Bool b;
  Float const *p_wIn = weightIn.getStorage(b);
  Float *p_wOut = weightOut.data();

  Int pols[] = { pid0, pid1 };
  for (Int64 i = 0; i < numElemPerPol; ++i) {
    ssize_t offsetIndex = i * numPol;
    Double sum = 0.0;
    for (ssize_t j = 0; j < 2; ++j) {
      Int ipol = pols[j];
      WeightHandler::AccumulateWeight(p_wIn[offsetIndex + ipol], sum);
    }
    WeightHandler::NormalizeWeight(sum, p_wOut[i]);
  }

  weightIn.freeStorage(p_wIn, b);
}
} // anonymous namespace

namespace casa { //# NAMESPACE CASA - BEGIN

namespace vi { //# NAMESPACE VI - BEGIN
//////////
// Base Class
// PolAverageTVI
/////////
PolAverageTVI::PolAverageTVI(ViImplementation2 *inputVII) :
    TransformingVi2(inputVII) {
  configurePolAverage();

  // Initialize attached VisBuffer
  setVisBuffer(createAttachedVisBuffer(VbRekeyable));
}

PolAverageTVI::~PolAverageTVI() {
}

void PolAverageTVI::origin() {
  TransformingVi2::origin();

  // Configure the correlations per shape
  configureShapes();

  // Synchronize own VisBuffer
  configureNewSubchunk();

  // reconfigure if necessary
  reconfigurePolAverageIfNecessary();

  // warn if current dd is inappropriate for polarization averaging
  warnIfNoTransform();
}

void PolAverageTVI::next() {
  TransformingVi2::next();

  // Configure the correlations per shape
  configureShapes();

  // Synchronize own VisBuffer
  configureNewSubchunk();

  // reconfigure if necessary
  reconfigurePolAverageIfNecessary();

  // warn if current dd is inappropriate for polarization averaging
  warnIfNoTransform();
}

void PolAverageTVI::configureShapes() {
    Vector <Int> corrs = getCorrelations ();
    Int nCorrs = corrs.nelements();

    nCorrelationsPerShape_ = casacore::Vector<casacore::Int> (1, nCorrs);
}

void PolAverageTVI::warnIfNoTransform() {
  if (!doTransform_[dataDescriptionId()]) {
    auto const vb = getVii()->getVisBuffer();
    LogIO os(LogOrigin("PolAverageTVI", __func__, WHERE));
    String msg("Skip polarization average because");
    if (vb->nCorrelations() == 1) {
      msg += " number of polarizations is 1.";
    } else if (anyEQ(vb->correlationTypes(), (Int) Stokes::I)) {
      msg += " polarization type is Stokes.";
    } else {
      msg += " no valid polarization components are found.";
    }
    os << LogIO::WARN << msg << LogIO::POST;
  }
}

void PolAverageTVI::corrType(Vector<Int> & corrTypes) const {
  if (doTransform_[dataDescriptionId()]) {
    // Always return (Stokes::I)
    Vector<Int> myCorrTypes(1, (Int) Stokes::I);
    corrTypes.reference(myCorrTypes);
  } else {
    getVii()->corrType(corrTypes);
  }
}

void PolAverageTVI::flagRow(Vector<Bool> & rowflags) const {
  Cube<Bool> const &flags = getVisBuffer()->flagCube();
  accumulateFlagCube(flags, rowflags);
}

void PolAverageTVI::flag(Cube<Bool> & flags) const {
  auto const vb = getVii()->getVisBuffer();
  Cube<Bool> originalFlags = vb->flagCube();
  Int ddid = dataDescriptionId();

  if (doTransform_[ddid]) {
    auto const cubeShape = originalFlags.shape();
    IPosition const newShape(3, 1, cubeShape[1], cubeShape[2]);
    Cube<Bool> transformedFlags(newShape, True);
    // accumulate first polarization component
    IPosition start(3, polId0_[ddid], 0, 0);
    IPosition end(3, polId0_[ddid], cubeShape[1] - 1, cubeShape[2] - 1);
    transformedFlags = originalFlags(start, end);

    // accumulate second polarization component
    start[0] = polId1_[ddid];
    end[0] = polId1_[ddid];
    transformedFlags &= originalFlags(start, end);
    flags.reference(transformedFlags);
  } else {
    flags.reference(originalFlags);
  }
}

void PolAverageTVI::flag(Matrix<Bool> & flags) const {
  Cube<Bool> transformedFlags;
  flag(transformedFlags);

  flags.reference(transformedFlags.yzPlane(0));
}

void PolAverageTVI::jonesC(Vector<SquareMatrix<Complex, 2> > &cjones) const {
  if (doTransform_[dataDescriptionId()]) {
    throw AipsError("PolAverageTVI::jonesC should not be called.");
  } else {
    getVii()->jonesC(cjones);
  }
}

void PolAverageTVI::sigma(Matrix<Float> & sigmat) const {
  if (weightSpectrumExists()) {
    Cube<Float> const &sigmaSp = getVisBuffer()->sigmaSpectrum();
    Cube<Bool> const &flag = getVisBuffer()->flagCube();
    accumulateWeightCube(sigmaSp, flag, sigmat);
  } else {
    if (doTransform_[dataDescriptionId()]) {
      weight(sigmat);
      arrayTransformInPlace(sigmat, ::weight2Sigma);
    } else {
      getVii()->sigma(sigmat);
    }
  }
}

void PolAverageTVI::visibilityCorrected(Cube<Complex> & vis) const {
  if (getVii()->existsColumn(VisBufferComponent2::VisibilityCorrected)) {
    Cube<Complex> dataCube;
    getVii()->visibilityCorrected(dataCube);
    if (doTransform_[dataDescriptionId()]) {
      Cube<Bool> flagCube;
      getVii()->flag(flagCube);
      transformComplexData(dataCube, flagCube, vis);
    } else {
      vis.reference(dataCube);
    }
  } else {
    vis.resize();
  }
}

void PolAverageTVI::visibilityModel(Cube<Complex> & vis) const {
  if (getVii()->existsColumn(VisBufferComponent2::VisibilityModel)) {
    Cube<Complex> dataCube;
    getVii()->visibilityModel(dataCube);
    if (doTransform_[dataDescriptionId()]) {
      Cube<Bool> flagCube;
      getVii()->flag(flagCube);
      transformComplexData(dataCube, flagCube, vis);
    } else {
      vis.reference(dataCube);
    }
  } else {
    vis.resize();
  }
}

void PolAverageTVI::visibilityObserved(Cube<Complex> & vis) const {
  if (getVii()->existsColumn(VisBufferComponent2::VisibilityObserved)) {
    Cube<Complex> dataCube;
    getVii()->visibilityObserved(dataCube);
    if (doTransform_[dataDescriptionId()]) {
      Cube<Bool> flagCube;
      getVii()->flag(flagCube);
      transformComplexData(dataCube, flagCube, vis);
    } else {
      vis.reference(dataCube);
    }
  } else {
    vis.resize();
  }
}

void PolAverageTVI::floatData(casacore::Cube<casacore::Float> & fcube) const {
  if (getVii()->existsColumn(VisBufferComponent2::FloatData)) {
    Cube<Float> dataCube;
    getVii()->floatData(dataCube);
    if (doTransform_[dataDescriptionId()]) {
      Cube<Bool> flagCube;
      getVii()->flag(flagCube);
      transformFloatData(dataCube, flagCube, fcube);
    } else {
      fcube.reference(dataCube);
    }
  } else {
    fcube.resize();
  }
}

IPosition PolAverageTVI::visibilityShape() const {
  IPosition cubeShape = getVii()->visibilityShape();
  if (doTransform_[dataDescriptionId()]) {
    // Length of polarization (Stokes) axis is always 1 after polarizaton averaging
    cubeShape[0] = 1;
  }
  return cubeShape;
}

void PolAverageTVI::weight(Matrix<Float> & wtmat) const {
  if (weightSpectrumExists()) {
    Cube<Float> const &weightSp = getVisBuffer()->weightSpectrum();
    Cube<Bool> const &flag = getVisBuffer()->flagCube();
    accumulateWeightCube(weightSp, flag, wtmat);
  } else {
    Matrix<Float> wtmatOrg;
    getVii()->weight(wtmatOrg);
    if (doTransform_[dataDescriptionId()]) {
      transformWeight(wtmatOrg, wtmat);
    } else {
      wtmat.reference(wtmatOrg);
    }
  }
}

void PolAverageTVI::weightSpectrum(Cube<Float> & wtsp) const {
  if (weightSpectrumExists()) {
    Cube<Float> wtspOrg;
    getVii()->weightSpectrum(wtspOrg);
    if (doTransform_[dataDescriptionId()]) {
      transformWeight(wtspOrg, wtsp);
    } else {
      wtsp.reference(wtspOrg);
    }
  } else {
    wtsp.resize();
  }
}

void PolAverageTVI::sigmaSpectrum(Cube<Float> & wtsp) const {
  if (sigmaSpectrumExists()) {
    if (doTransform_[dataDescriptionId()]) {
      // sigma = (weight)^-1/2
      weightSpectrum(wtsp);
      arrayTransformInPlace(wtsp, ::weight2Sigma);
    } else {
      getVii()->sigmaSpectrum(wtsp);
    }
  } else {
    wtsp.resize();
  }
}

const VisImagingWeight & PolAverageTVI::getImagingWeightGenerator() const {
  if (doTransform_[dataDescriptionId()]) {
    throw AipsError(
        "PolAverageTVI::getImagingWeightGenerator should not be called.");
  }

  return getVii()->getImagingWeightGenerator();
}

Vector<Int> PolAverageTVI::getCorrelations() const {
  if (doTransform_[dataDescriptionId()]) {
    // Always return (Stokes::I)
    return Vector<Int>(1, Stokes::I);
  } else {
    return getVii()->getCorrelations();
  }
}

Vector<Stokes::StokesTypes> PolAverageTVI::getCorrelationTypesDefined() const {
  if (doTransform_[dataDescriptionId()]) {
    // Always return (Stokes::I)
    return Vector<Stokes::StokesTypes>(1, Stokes::I);
  } else {
    return getVii()->getCorrelationTypesDefined();
  }
}

Vector<Stokes::StokesTypes> PolAverageTVI::getCorrelationTypesSelected() const {
  if (doTransform_[dataDescriptionId()]) {
    // Always return (Stokes::I)
    return Vector<Stokes::StokesTypes>(1, Stokes::I);
  } else {
    return getVii()->getCorrelationTypesSelected();
  }
}

const casacore::Vector<casacore::Int>&
PolAverageTVI::nCorrelationsPerShape() const {
  return nCorrelationsPerShape_;
}

void PolAverageTVI::configurePolAverage() {
  MeasurementSet const &ms = getVii()->ms();
  auto const &msdd = ms.dataDescription();
  MSDataDescColumns msddcols(msdd);
  uInt ndd = msddcols.nrow();
  Vector<Int> polIds = msddcols.polarizationId().getColumn();
  doTransform_.resize(ndd);
  polId0_.resize(ndd);
  polId1_.resize(ndd);
  auto const &mspol = ms.polarization();
  MSPolarizationColumns mspolcols(mspol);
  doTransform_ = False;
  for (uInt idd = 0; idd < ndd; ++idd) {
    Vector<Int> corrType = mspolcols.corrType()(polIds[idd]);
    polId0_[idd] = -1;
    polId1_[idd] = -1;
    for (size_t i = 0; i < corrType.size(); ++i) {
      auto stokesType = Stokes::type(corrType[i]);
      if (stokesType == Stokes::XX || stokesType == Stokes::RR) {
        polId0_[idd] = i;
      } else if (stokesType == Stokes::YY || stokesType == Stokes::LL) {
        polId1_[idd] = i;
      }
    }
    doTransform_[idd] = (polId0_[idd] >= 0 && polId1_[idd] >= 0);
  }
}

//////////
// GeometricPolAverageTVI
/////////
GeometricPolAverageTVI::GeometricPolAverageTVI(ViImplementation2 *inputVII) :
    PolAverageTVI(inputVII) {
}

GeometricPolAverageTVI::~GeometricPolAverageTVI() {
}

void GeometricPolAverageTVI::transformComplexData(Cube<Complex> const &dataIn,
    Cube<Bool> const &flagIn, Cube<Complex> &dataOut) const {
  Int ddid = dataDescriptionId();
  Int pid0 = polId0_[ddid];
  Int pid1 = polId1_[ddid];
  transformData(dataIn, flagIn, pid0, pid1, dataOut);
}

void GeometricPolAverageTVI::transformFloatData(Cube<Float> const &dataIn,
    Cube<Bool> const &flagIn, Cube<Float> &dataOut) const {
  Int ddid = dataDescriptionId();
  Int pid0 = polId0_[ddid];
  Int pid1 = polId1_[ddid];
  transformData(dataIn, flagIn, pid0, pid1, dataOut);
}

void GeometricPolAverageTVI::transformWeight(Array<Float> const &weightIn,
    Array<Float> &weightOut) const {
  Int ddid = dataDescriptionId();
  Int pid0 = polId0_[ddid];
  Int pid1 = polId1_[ddid];
  ::transformWeight<GeometricTransformation>(weightIn, pid0, pid1, weightOut);
}

template<class T>
void GeometricPolAverageTVI::transformData(Cube<T> const &dataIn,
    Cube<Bool> const &flagIn, Int pid0, Int pid1, Cube<T> &dataOut) const {
  if (weightSpectrumExists()) {
    Cube<Float> weightSp;
    getVii()->weightSpectrum(weightSp);
    ::GeometricTransformation::transformData<T>(dataIn, flagIn, weightSp, pid0,
        pid1, dataOut);
  } else {
    Matrix<Float> weightMat;
    getVii()->weight(weightMat);
    ::GeometricTransformation::transformData<T>(dataIn, flagIn, weightMat, pid0,
        pid1, dataOut);
  }
}

//////////
// StokesPolAverageTVI
/////////
StokesPolAverageTVI::StokesPolAverageTVI(ViImplementation2 *inputVII) :
    PolAverageTVI(inputVII) {
}

StokesPolAverageTVI::~StokesPolAverageTVI() {
}

void StokesPolAverageTVI::transformComplexData(Cube<Complex> const &dataIn,
    Cube<Bool> const &flagIn, Cube<Complex> &dataOut) const {
  Int ddid = dataDescriptionId();
  Int pid0 = polId0_[ddid];
  Int pid1 = polId1_[ddid];
  transformData(dataIn, flagIn, pid0, pid1, dataOut);
}

void StokesPolAverageTVI::transformFloatData(Cube<Float> const &dataIn,
    Cube<Bool> const &flagIn, Cube<Float> &dataOut) const {
  Int ddid = dataDescriptionId();
  Int pid0 = polId0_[ddid];
  Int pid1 = polId1_[ddid];
  transformData(dataIn, flagIn, pid0, pid1, dataOut);
}

void StokesPolAverageTVI::transformWeight(Array<Float> const &weightIn,
    Array<Float> &weightOut) const {
  Int ddid = dataDescriptionId();
  Int pid0 = polId0_[ddid];
  Int pid1 = polId1_[ddid];
  ::transformWeight<StokesTransformation>(weightIn, pid0, pid1, weightOut);
}

template<class T>
void StokesPolAverageTVI::transformData(Cube<T> const &dataIn,
    Cube<Bool> const &flagIn, Int pid0, Int pid1, Cube<T> &dataOut) const {
  ::StokesTransformation::transformData<T>(dataIn, flagIn, pid0, pid1, dataOut);
}

//////////
// PolAverageTVIFactory
/////////
PolAverageVi2Factory::PolAverageVi2Factory(Record const &configuration,
    ViImplementation2 *inputVII) :
    inputVII_p(inputVII), mode_(AveragingMode::DEFAULT) {
  inputVII_p = inputVII;

  mode_ = PolAverageVi2Factory::GetAverageModeFromConfig(configuration);
}

PolAverageVi2Factory::PolAverageVi2Factory(Record const &configuration,
    MeasurementSet const *ms, SortColumns const sortColumns,
    Double timeInterval, Bool isWritable) :
    inputVII_p(nullptr), mode_(AveragingMode::DEFAULT) {
  inputVII_p = new VisibilityIteratorImpl2(Block<MeasurementSet const *>(1, ms),
      sortColumns, timeInterval, isWritable);

  mode_ = PolAverageVi2Factory::GetAverageModeFromConfig(configuration);
}

PolAverageVi2Factory::~PolAverageVi2Factory() {
}

ViImplementation2 * PolAverageVi2Factory::createVi() const {
  if (mode_ == AveragingMode::GEOMETRIC) {
    return new GeometricPolAverageTVI(inputVII_p);
  } else if (mode_ == AveragingMode::STOKES) {
    return new StokesPolAverageTVI(inputVII_p);
  }

  throw AipsError("Invalid Averaging Mode for PolAverageTVI.");

  return nullptr;
}

PolAverageTVILayerFactory::PolAverageTVILayerFactory(
    Record const &configuration) :
    ViiLayerFactory() {
  configuration_p = configuration;
}

ViImplementation2*
PolAverageTVILayerFactory::createInstance(ViImplementation2* vii0) const {
  // Make the PolAverageTVI, using supplied ViImplementation2, and return it
  PolAverageVi2Factory factory(configuration_p, vii0);
  ViImplementation2 *vii = nullptr;
  try {
    vii = factory.createVi();
  } catch (...) {
    if (vii0) {
      delete vii0;
    }
    throw;
  }
  return vii;
}
} // # NAMESPACE VI - END
} // #NAMESPACE CASA - END
