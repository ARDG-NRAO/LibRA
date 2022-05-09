//# SDAtmosphereCorrectionTVI.h: This file contains the implementation of the SDAtmosphereCorrectionTVI class.
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
#include <mstransform/TVI/SDAtmosphereCorrectionTVI.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#include <algorithm>
#include <cmath>
#include <functional>
#include <iomanip>
#include <limits>
#include <vector>

#include <casacore/casa/Arrays/Cube.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/BasicSL/Constants.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Containers/Record.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/Arrays/ArrayIter.h>
#include <casacore/casa/OS/File.h>
#include <casacore/casa/Quanta/MVTime.h>
#include <casacore/casa/Utilities/Sort.h>
#include <casacore/casa/Utilities/BinarySearch.h>
#include <casacore/measures/Measures/Stokes.h>
#include <casacore/scimath/Functionals/Interpolate1D.h>
#include <casacore/scimath/Functionals/ScalarSampledFunctional.h>
#include <casacore/ms/MeasurementSets/MSDataDescColumns.h>
#include <casacore/ms/MeasurementSets/MSSpWindowColumns.h>
#include <casacore/ms/MeasurementSets/MSPolColumns.h>
#include <casacore/ms/MSOper/MSMetaData.h>
#include <casacore/tables/TaQL/ExprNode.h>

#include <msvis/MSVis/VisBufferComponents2.h>
#include <msvis/MSVis/VisibilityIteratorImpl2.h>

#include <mstransform/TVI/UtilsTVI.h>

using namespace casacore;

namespace {

// for TDM spw: resolve 1 channel into 5 subchannels for ATM model
constexpr unsigned int kNumResolveTDM = 5u;

constexpr float kNoCache = -1.0f;
constexpr int kInvalidIndex = -1;
constexpr double kValueUnset = std::numeric_limits<double>::quiet_NaN();
inline bool isValueUnset(double const v) {return std::isnan(v);}

class ScopeGuard {
  public:
    explicit ScopeGuard(std::function<void()> f) : func_(f) {}
    ScopeGuard(ScopeGuard const &s) = delete;
    void operator=(ScopeGuard const &s) = delete;
    ~ScopeGuard() {
      func_();
    }
  private:
    std::function<void()> func_;
};

inline String timeToString(Double timeInSec) {
  return MVTime(Quantity(timeInSec, "s")).string(MVTime::FITS);
}

inline void sortTime(Vector<Double> &timeData, Vector<uInt> &indexVector) {
  Bool b = false;
  Double const *p = timeData.getStorage(b);
  ScopeGuard guard([&]() {
    timeData.freeStorage(p, b);
  });
  Sort sort(p, timeData.nelements());
  sort.sort(indexVector, timeData.nelements());
}

inline uInt makeUnique(Vector<Double> const &data, Vector<uInt> const &sortIndex, Vector<uInt> &uniqueIndex) {
  uniqueIndex.resize(data.nelements());
  assert(data.nelements() == sortIndex.nelements());
  uInt n = 0;
  uniqueIndex[n++] = sortIndex[0];
  Double prev = data[sortIndex[0]];
  for (uInt i = 0; i < data.nelements(); ++i) {
    uInt j = sortIndex[i];
    Double current = data[j];
    if (current != prev) {
      uniqueIndex[n++] = j;
    }
    prev = current;
  }
  uniqueIndex.resize(n, True);
  return n;
}

inline Vector<Double> getMedianDataPerTime(uInt const n, Vector<uInt> const &uniqueVector,
  Vector<uInt> const &indexVector, Vector<Double> const &data) {
  Vector<Double> out(n);
  Bool const sorted = False;
  Bool const takeEvenMean = True;
  Bool const inPlace = True;
  assert(n > 0);
  for (uInt i = 0; i < n; ++i) {
    uInt const iStart = uniqueVector[i];
    uInt const iEnd = (i < n - 1) ? uniqueVector[i + 1] : indexVector.nelements();
    uInt const nElem = iEnd - iStart;
    Vector<Double> tmp(nElem);
    for (uInt j = 0; j < nElem; ++j) {
      tmp[j] = data[indexVector[j + iStart]];
    }
    out[i] = median(tmp, sorted, takeEvenMean, inPlace);
  }
  return out;
}

inline std::pair<Int, Int> findNearestIndex(Vector<Double> const &data, Double const target) {
  Bool found = false;
  // data is sorted in ascending order
  Vector<Double> diff(data.nelements());
  for (uInt i = 0; i < diff.nelements(); ++i) {
    diff[i] = data[i] - data[0];
  }
  Int index = binarySearch(found, data, target, data.nelements());
  Int prev = kInvalidIndex;
  Int next = kInvalidIndex;
  if (found) {
    prev = index;
    next = index;
  } else if (index >= 0) {
    prev = index - 1;
    next = index;
  } else {
    throw AipsError("error in findNearest");
  }
  return std::make_pair(prev, next);
}

// implementation of np.convolve(mode='same')
inline Vector<Double> convolve1DTriangle(Vector<Double> const &in) {
  constexpr unsigned int kNumKernel = 3u;
  constexpr Double kKernelTriangle[] = {0.25, 0.5, 0.25};
  unsigned int const n = in.nelements();
  assert(n >= kNumKernel);
  Vector<Double> out(n, 0.0);
  // symmetric kernel
  out[0] = kKernelTriangle[0] * in[1] + kKernelTriangle[1] * in[0];
  for (unsigned int i = 1; i < n - 1; ++i) {
    out[i] = kKernelTriangle[0] * (in[i - 1] + in[i + 1]) + kKernelTriangle[1] * in[i];
  }
  out[n - 1] = kKernelTriangle[0] * in[n - 2] + kKernelTriangle[1] * in[n - 1];
  return out;
}

inline Vector<Double> convolve1DHanning(Vector<Double> const &in) {
  // normalized spectral response for Hanning window, FWHM=10
  constexpr unsigned int kNumKernel = 29u;
  constexpr unsigned int kIndexKernelCenter = kNumKernel / 2u;
  constexpr Double kKernelHanning[] = {
    -0.00098041, -0.00202866, -0.00265951, -0.00222265,
    0.00000000, 0.00465696, 0.01217214, 0.02260546, 0.03556241,
    0.05017949, 0.06519771, 0.07911911, 0.09042184, 0.09779662,
    0.10035898, 0.09779662, 0.09042184, 0.07911911, 0.06519771,
    0.05017949, 0.03556241, 0.02260546, 0.01217214, 0.00465696,
    0.00000000, -0.00222265, -0.00265951, -0.00202866, -0.00098041
  };
  unsigned int const n = in.nelements();
  assert(n >= kNumKernel);
  // Vector<Double> out(n, 0.0);
  std::unique_ptr<Double[]> out(new Double[n]);
  Bool b;
  Double const *in_p = in.getStorage(b);
  // symmetric kernel
  for (unsigned int i = 0u; i < kIndexKernelCenter; ++i) {
    out[i] = 0.0;
    for (unsigned int j = 0u; j < kIndexKernelCenter + i + 1; ++j) {
      unsigned int k = kIndexKernelCenter - i + j;
      out[i] += in_p[j] * kKernelHanning[k];
    }
  }
  for (unsigned int i = kIndexKernelCenter; i < n - kIndexKernelCenter; ++i) {
    out[i] = 0.0;
    for (unsigned int k = 0u; k < kNumKernel; ++k) {
      unsigned int j = i - kIndexKernelCenter + k;
      out[i] += in_p[j] * kKernelHanning[k];
    }
  }
  for (unsigned int i = n - kIndexKernelCenter; i < n; ++i) {
    out[i] = 0.0;
    for (unsigned int j = i - kIndexKernelCenter; j < n; ++j) {
      unsigned int k = j - (i - kIndexKernelCenter);
      out[i] += in_p[j] * kKernelHanning[k];
    }
  }
  in.freeStorage(in_p, b);
  return Vector<Double>(IPosition(1, n), out.release(), TAKE_OVER);
}

inline Vector<Double> getTrjSkySpec(atm::SkyStatus *skyStatus, unsigned int const nchan) {
  // TODO: OMP parallelization
  Vector<Double> trj(nchan);
  for (unsigned int i = 0; i < nchan; ++i) {
    trj[i] = skyStatus->getTrjSky(i).get(atm::Temperature::UnitKelvin);
  }
  return trj;
}

inline Vector<Double> getTauSpec(atm::SkyStatus *skyStatus, unsigned int const nchan, Double const airMass) {
  // TODO: OMP parallelization
  Vector<Double> tau(nchan);
  for(unsigned int i = 0; i < nchan; ++i) {
    tau[i] = (skyStatus->getDryOpacity(i).get(atm::Opacity::UnitNeper) +
              skyStatus->getWetOpacity(i).get(atm::Opacity::UnitNeper))
             * airMass;
  }
  return tau;
}

inline Vector<Double> getCorrectionFactor(
  Vector<Double> const &tSkyOn, Vector<Double> const &tSkyOff, Vector<Double> const &tauOff) {
  Vector<Double> factor(tSkyOn.nelements());
  // TODO: OMP/SIMD parallelization
  for (size_t i = 0; i < tSkyOn.nelements(); ++i) {
    factor[i] = (tSkyOn[i] - tSkyOff[i]) * std::exp(tauOff[i]);
  }
  return factor;
}

// TODO: add target_clones attribute?
template<class T>
inline void transformData(
  std::vector<Vector<Double> > const &correctionFactorList,
  std::vector<Int> const &indexForCorrection,
  Cube<T> &inout,
  int const numThreads
) {
  IPosition const shape = inout.shape();
  assert(shape[2] == static_cast<Int>(indexForCorrection.size()));
  Bool b1;
  T *p = inout.getStorage(b1);
  // TODO: optimize condition for multi-threading
  #pragma omp parallel for num_threads(numThreads) if(numThreads > 1 && shape[2] > 10 && shape[1] > 500)
  for (ssize_t ir = 0; ir < shape[2]; ++ir) {
    Int const index = indexForCorrection[ir];
    if (index >= 0) {
      Vector<Double> const &correctionFactor = correctionFactorList[index];
      Bool b2;
      Double const *cp = correctionFactor.getStorage(b2);
      ssize_t const offset = shape[0] * shape[1] * ir;
      for (ssize_t ic = 0; ic < shape[1]; ++ic) {
        ssize_t idx = offset + shape[0] * ic;
        for (ssize_t ip = 0; ip < shape[0]; ++ip) {
          p[idx + ip] = p[idx + ip] - cp[ic];
        }
      }
      correctionFactor.freeStorage(cp, b2);
    }
  }
  inout.putStorage(p, b1);
}

bool isCloseDouble(Double const a, Double const b) {
  constexpr Double kToleranceDouble = std::numeric_limits<double>::epsilon();
  Double const relativeDiff = (abs(a) < kToleranceDouble) ? abs(b - a) : abs((b - a) / a);
  return relativeDiff < kToleranceDouble;
}

inline Vector<Int> getScienceSpw(MeasurementSet const &ms) {
  MSMetaData msmd(&ms, kNoCache);
  std::set<uInt> fdmSpws = msmd.getFDMSpw();
  std::set<uInt> tdmSpws = msmd.getTDMSpw();
  std::set<uInt> onSourceSpws = msmd.getSpwsForIntent("OBSERVE_TARGET#ON_SOURCE");
  std::set<uInt> fdmTdmSpws;
  std::set_union(fdmSpws.begin(), fdmSpws.end(),
                 tdmSpws.begin(), tdmSpws.end(),
                 std::inserter(fdmTdmSpws, fdmTdmSpws.end()));
  std::vector<Int> ss;
  std::set_intersection(onSourceSpws.begin(), onSourceSpws.end(),
                        fdmTdmSpws.begin(), fdmTdmSpws.end(),
                        std::back_inserter(ss));
  return Vector<Int>(ss);
}

} // anonymous namespace

namespace casa { //# NAMESPACE CASA - BEGIN

namespace vi { //# NAMESPACE VI - BEGIN
//////////
// SDAtmosphereCorrectionTVI
/////////
SDAtmosphereCorrectionTVI::SDAtmosphereCorrectionTVI(ViImplementation2 *inputVII,
  Record const &configuration) :
    TransformingVi2(inputVII),
    processSpwList_(),
    gainFactorList_(),
    userPressureValue_(kValueUnset),
    userTemperatureValue_(kValueUnset),
    userRelHumidityValue_(kValueUnset),
    userPwvValue_(kValueUnset),
    configuration_(configuration),
    offSourceTime_(),
    elevationInterpolator_(),
    pwvData_(),
    atmTime_(),
    atmTemperatureData_(),
    atmPressureData_(),
    atmRelHumidityData_(),
    isTdmSpw_(),
    doSmooth_(),
    nchanPerSpw_(),
    currentSpwId_(kInvalidIndex),
    transformSubchunk_(false),
    atmType_(2),
    atmSkyStatusPerSpw_(),
    correctionFactorList_(),
    indexForCorrection_(),
    numThreads_(1)
{

  // Initialize attached VisBuffer
  LogIO os(LogOrigin("SDAtmosphereCorrectionTVI", __func__, WHERE));
  setVisBuffer(createAttachedVisBuffer(VbRekeyable));

  // set numThreads_
#ifdef _OPENMP
  // TODO: optimize number of threads
  // setup number of threads for OpenMP
  if (configuration.isDefined("nthreads")) {
    numThreads_ = configuration.asInt("nthreads");
  }

  if (numThreads_ < 0) {
    constexpr int kNumThreads = 8;
    int const numProcessors = omp_get_num_procs();
    numThreads_ = min(kNumThreads, numProcessors);
  }
  os << "Setting numThreads_ to " << numThreads_ << LogIO::POST;
#endif
}

void SDAtmosphereCorrectionTVI::origin() {
  TransformingVi2::origin();

  // Synchronize own VisBuffer
  configureNewSubchunk();

  // compute all correction factors for subchunk at once
  updateCorrectionFactorInAdvance();
}

void SDAtmosphereCorrectionTVI::next() {
  TransformingVi2::next();

  // Synchronize own VisBuffer
  configureNewSubchunk();

  // compute all correction factors for subchunk at once
  updateCorrectionFactorInAdvance();
}

void SDAtmosphereCorrectionTVI::initializeChunk() {
  // setup some cache values
  currentSpwId_ = dataDescriptionSubtablecols().spectralWindowId().get(dataDescriptionId());

  LogIO os(LogOrigin("SDAtmosphereCorrectionTVI", __func__, WHERE));
  auto const subchunkId = getSubchunkId();
  os << "Process chunk " << subchunkId.first << "-" << subchunkId.second
     << " SPW " << currentSpwId_ << LogIO::POST;
}

void SDAtmosphereCorrectionTVI::originChunks(Bool forceRewind) {
  TransformingVi2::originChunks(forceRewind);

  // initialization
  initializeAtmosphereCorrection(configuration_);

  initializeChunk();

  // initialize Atmosphere Transmission Model
  initializeAtmosphereModel(configuration_);
}

void SDAtmosphereCorrectionTVI::nextChunk() {
  TransformingVi2::nextChunk();

  initializeChunk();
}

void SDAtmosphereCorrectionTVI::visibilityCorrected(Cube<Complex> & vis) const {
  if (getVii()->existsColumn(VisBufferComponent2::VisibilityCorrected)) {
    getVii()->visibilityCorrected(vis);
    if (transformSubchunk_) {
      transformData(correctionFactorList_, indexForCorrection_, vis, numThreads_);
    }
  } else {
    vis.resize();
  }
}

void SDAtmosphereCorrectionTVI::visibilityModel(Cube<Complex> & vis) const {
  if (getVii()->existsColumn(VisBufferComponent2::VisibilityModel)) {
    getVii()->visibilityModel(vis);
    if (transformSubchunk_) {
      transformData(correctionFactorList_, indexForCorrection_, vis, numThreads_);
    }
  } else {
    vis.resize();
  }
}

void SDAtmosphereCorrectionTVI::visibilityObserved(Cube<Complex> & vis) const {
  if (getVii()->existsColumn(VisBufferComponent2::VisibilityObserved)) {
    getVii()->visibilityObserved(vis);
    if (transformSubchunk_) {
      transformData(correctionFactorList_, indexForCorrection_, vis, numThreads_);
    }
  } else {
    vis.resize();
  }
}

void SDAtmosphereCorrectionTVI::floatData(casacore::Cube<casacore::Float> & fcube) const {
  if (getVii()->existsColumn(VisBufferComponent2::FloatData)) {
    getVii()->floatData(fcube);
    if (transformSubchunk_) {
      transformData(correctionFactorList_, indexForCorrection_, fcube, numThreads_);
    }
  } else {
    fcube.resize();
  }
}

void SDAtmosphereCorrectionTVI::initializeAtmosphereCorrection(Record const &configuration)
{
  LogIO os(LogOrigin("SDAtmosphereCorrectionTVI", __func__, WHERE));

  // processing spws
  if (!configuration.isDefined("processspw")) {
    os << "ERROR in configuration: processing spw list (processspw) must be given."
       << LogIO::EXCEPTION;
  }
  processSpwList_ = configuration.asArrayInt("processspw");
  os << "processspw (input) = " << processSpwList_ << LogIO::POST;

  // gain factor
  if (!configuration.isDefined("gainfactor")) {
    os << "ERROR in configuration: gain factor list (gainfactor) must be given."
       << LogIO::EXCEPTION;
  }
  gainFactorList_ = configuration.asArrayDouble("gainfactor");
  os << "gainfactor = " << gainFactorList_ << LogIO::POST;

  // refefence antenna ID
  if (!configuration.isDefined("refant")) {
    os << "ERROR in configuration: reference antenna (refant) must be given."
       << LogIO::EXCEPTION;
  }
  Int const referenceAntenna = configuration.asInt("refant");
  os << "reference antenna ID = " << referenceAntenna << LogIO::POST;

  // raw MS name must be given
  if (!configuration.isDefined("inputms")) {
    os << "ERROR in configuration: raw MS name (inputms) must be given."
       << LogIO::EXCEPTION;

  }
  String const rawMs = configuration.asString("inputms");
  os << "input MS = \"" << rawMs << "\"" << LogIO::POST;

  Vector<SpwId> scienceSpws = getScienceSpw(MeasurementSet(rawMs));

  // udpate processSpwList_: exclude non-science spws
  std::vector<SpwId> updated;
  std::copy_if(processSpwList_.begin(), processSpwList_.end(),
    std::back_inserter(updated),
    [&](SpwId x) -> bool {return anyEQ(scienceSpws, x);}
  );
  processSpwList_.assign(Vector<SpwId>(updated));

  auto const nameColumn = spectralWindowSubtablecols().name();
  auto const nchanColumn = spectralWindowSubtablecols().numChan();

  // number of channels per baseband
  std::map<Int, uInt> nchanPerBB;
  std::map<SpwId, Int> spwBBMap;
  for (auto i = scienceSpws.begin(); i != scienceSpws.end(); ++i) {
    SpwId const spw = *i;
    String const name = nameColumn(spw);
    Int nchan = nchanColumn(spw);
    auto const pos = name.find("#BB_");
    auto const pos2 = name.find("#", pos + 1);
    if (pos != String::npos && pos + 4 < name.size()) {
      Int bb = String::toInt(name.substr(pos + 4, pos2 - pos - 4));
      // cout << "SPW " << spw << ": name \"" << name
      //      << "\" substr " << name.substr(pos + 4, pos2 - pos - 4)
      //      << " pos " << pos + 4 << " pos2 " << pos2
      //      << " bb " << bb << endl;
      spwBBMap[spw] = bb;
      auto j = nchanPerBB.find(bb);
      if (j == nchanPerBB.end()) {
        nchanPerBB[bb] = nchan;
      } else {
        nchanPerBB[bb] += nchan;
      }
    }
  }

  for (auto i = processSpwList_.begin(); i != processSpwList_.end(); ++i) {
    SpwId const spw = *i;

    // number of channels for processing spw
    nchanPerSpw_[spw] = nchanColumn(spw);

    // number of total channels for baseband associated with processing spw
    auto j = spwBBMap.find(spw);
    auto const baseband = (j == spwBBMap.end()) ? 999u : j->second;
    auto const nchanBB = (j == spwBBMap.end()) ? 0u : nchanPerBB[baseband];

    // check if spw is TDM
    isTdmSpw_[spw] = (nchanBB == 128u || nchanBB == 256u);

    // smoothing control
    Int const polId = dataDescriptionSubtablecols().polarizationId().get(dataDescriptionId());
    Int const numPol = polarizationSubtablecols().numCorr().get(polId);
    uInt numChanPol = nchanBB * numPol;
    doSmooth_[spw] = (numChanPol == 256u || numChanPol == 8192u);
    if (isTdmSpw_[spw]) {
      os << "Spw " << spw << " seems to be TDM-like. "
         << "More accurate Hanning smoothing is applied.  istdm=True" << LogIO::POST;
    }
    os << "Spw " << spw << " in BB_" << baseband << " "
       << "(total Nchan within BB is " << numChanPol << ", "
       << " sp avg likely " << ((doSmooth_[spw]) ? "not " : " ") << "applied). "
       << "dosmooth=" << ((doSmooth_[spw]) ? "True" : "False") << LogIO::POST;
  }

  // notification on non-processing spw
  MSMetaData msmd(&ms(), kNoCache);
  std::set<uInt> allSpwIds = msmd.getSpwIDs();
  std::set<uInt> nonProcessingSpws;
  std::set_difference(allSpwIds.begin(), allSpwIds.end(),
                      processSpwList_.begin(), processSpwList_.end(),
                      std::inserter(nonProcessingSpws, nonProcessingSpws.begin()));
  if (nonProcessingSpws.size() > 0) {
    os << LogIO::WARN << "SPW"
       << ((nonProcessingSpws.size() == 1u) ? " " : "s ");
    for (auto i = nonProcessingSpws.begin(); i != nonProcessingSpws.end(); ++i) {
      os << *i << " ";
    }
    os << ((nonProcessingSpws.size() == 1u) ? "is" : "are")
       << " output but not corrected" << LogIO::POST;
  }

  // read MAIN table (OFF_SOURCE time)
  readMain(rawMs);

  // read POINTING table
  readPointing(rawMs, referenceAntenna);

  // read ASDM_CALWVR and ASDM_CALATMOSPHERE tables
  readAsdmAsIsTables(rawMs);

}

void SDAtmosphereCorrectionTVI::initializeAtmosphereModel(Record const &configuration) {
  LogIO os(LogOrigin("SDAtmosphereCorrectionTVI", __func__, WHERE));

  // AtmProfile

  // for tentative default ATM data
  Vector<Double> timeData;
  time(timeData);
  Int currentAtmTimeIndex = 0;
  os << "atmTime_ size " << atmTime_.size() << LogIO::POST;
  for (uInt i = 0; i < atmTime_.size(); ++i) {
    os.output() << std::setprecision(16);
    os << " atmTime_[" << i << "] = " << atmTime_[i] << " (current Time " << timeData[0] << ")" << LogIO::POST;
  }

  std::pair<Int, Int> pair = findNearestIndex(atmTime_, timeData[0]);
  currentAtmTimeIndex = (0 <= pair.first) ? pair.first : pair.second;
  if (currentAtmTimeIndex < 0 || atmTime_.size() <= static_cast<uInt>(currentAtmTimeIndex)) {
    os << "Failed to obtain valid time index for atmosphere measurement."
       << LogIO::EXCEPTION;
  }

  // altitude
  if (!configuration.isDefined("siteAltitude")) {
    os << "ERROR in configuration: site altitude (in metre) must be given."
       << LogIO::EXCEPTION;
  }
  atm::Length altitude(configuration.asDouble("siteAltitude"), atm::Length::UnitMeter);
  os << "site altitude = " << altitude.get() << LogIO::POST;

  // pressure (mbar)
  double defaultPressureValue = kValueUnset;
  if (configuration.isDefined("pressure")) {
    userPressureValue_ = configuration.asDouble("pressure");
    defaultPressureValue = userPressureValue_;
    os << "user pressure = " << userPressureValue_ << LogIO::POST;
  } else {
    userPressureValue_ = kValueUnset;
    if (atmPressureData_.nelements() > 0) {
      defaultPressureValue = atmPressureData_[currentAtmTimeIndex];
    }
    os << "default pressure value = " << defaultPressureValue << LogIO::POST;
  }

  if (isValueUnset(defaultPressureValue)) {
    os << "ERROR in configuration: No useful pressure value exists."
       << LogIO::EXCEPTION;
  }

  atm::Pressure defaultPressure(defaultPressureValue, atm::Pressure::UnitMilliBar);

  // temperature (K)
  double defaultTemperatureValue = kValueUnset;
  if (configuration.isDefined("temperature")) {
    userTemperatureValue_ = configuration.asDouble("temperature");
    defaultTemperatureValue = userTemperatureValue_;
    os << "user temperature = " << userTemperatureValue_ << LogIO::POST;
  } else {
    userTemperatureValue_ = kValueUnset;
    if (atmTemperatureData_.nelements() > 0) {
      defaultTemperatureValue = atmTemperatureData_[currentAtmTimeIndex];
    }
    os << "default temperature value = " << defaultTemperatureValue << LogIO::POST;
  }

  if (isValueUnset(defaultTemperatureValue)) {
    os << "ERROR in configuration: No useful temperature value exists."
       << LogIO::EXCEPTION;
  }

  atm::Temperature defaultTemperature(defaultTemperatureValue, atm::Temperature::UnitKelvin);

  // lapse rate (K/km)
  double tropoLapseRate = (configuration.isDefined("lapseRate")) ?
    configuration.asDouble("lapseRate") : -5.6;
  os << "temperature lapse rate = " << tropoLapseRate << LogIO::POST;

  // relative humidity
  double defaultRelHumidityValue = kValueUnset;
  if (configuration.isDefined("humidity")) {
    userRelHumidityValue_ = configuration.asDouble("humidity");
    defaultRelHumidityValue = userRelHumidityValue_;
    os << "user humidity = " << userRelHumidityValue_ << LogIO::POST;
  } else {
    userRelHumidityValue_ = kValueUnset;
    if (atmRelHumidityData_.nelements() > 0) {
      defaultRelHumidityValue = atmRelHumidityData_[currentAtmTimeIndex];
    }
    os << "default humidity value = " << defaultRelHumidityValue << LogIO::POST;
  }

  if (isValueUnset(defaultRelHumidityValue)) {
    os << "ERROR in configuration: No useful humidity value exists."
       << LogIO::EXCEPTION;
  }

  atm::Humidity defaultRelHumidity(defaultRelHumidityValue, atm::Humidity::UnitPercent);

  // PWV (mm)
  double defaultPwvValue = kValueUnset;
  if (configuration.isDefined("pwv")) {
    userPwvValue_ = configuration.asDouble("pwv");
    defaultPwvValue = userPwvValue_;
    os << "user pwv = " << userPwvValue_ << LogIO::POST;
  } else {
    userPwvValue_ = kValueUnset;
    if (pwvData_.nelements() > 0) {
      defaultPwvValue = pwvData_[currentAtmTimeIndex];
    }
    os << "default pwv value = " << defaultPwvValue << LogIO::POST;
  }

  if (isValueUnset(defaultPwvValue)) {
    os << "ERROR in configuration: No useful pwv value exists."
       << LogIO::EXCEPTION;
  }

  // scale height (km)
  Double scaleHeightValue = (configuration.isDefined("scaleHeight")) ?
    configuration.asDouble("scaleHeight") : 2.0;
  atm::Length wvScaleHeight(scaleHeightValue, atm::Length::UnitKiloMeter);
  os << "scale height = " << wvScaleHeight.get() << LogIO::POST;

  // pressure step (mbar)
  Double pressureStepValue = (configuration.isDefined("pressureStep")) ?
    configuration.asDouble("pressureStep") : 10.0;
  atm::Pressure pressureStep(pressureStepValue, atm::Pressure::UnitMilliBar);
  os << "pressure step = " << pressureStep.get(atm::Pressure::UnitMilliBar) << LogIO::POST;

  // pressure step factor
  double pressureStepFactor = (configuration.isDefined("pressureStepFactor")) ?
    configuration.asDouble("pressureStepFactor") : 1.2;
  os << "pressure step factor = " << pressureStepFactor << LogIO::POST;

  // maximum altitude (km)
  double maxAltitude = (configuration.isDefined("maxAltitude")) ?
    configuration.asDouble("maxAltitude") : 120.0;
  atm::Length topAtmProfile(maxAltitude, atm::Length::UnitKiloMeter);
  os << "max ATM altitude = " << topAtmProfile.get() << LogIO::POST;

  // ATM type
  atmType_ = (configuration.isDefined("atmType")) ?
    configuration.asuInt("atmType") : 2;
  os << "ATM type = " << atmType_ << LogIO::POST;

  // layerBoundaries and layerTemperatures
  std::vector<atm::Length> layerBoundaries;
  std::vector<atm::Temperature> layerTemperatures;
  if (configuration.isDefined("layerBoundaries") &&
      configuration.isDefined("layerTemperatures")) {
    Vector<Double> lB = configuration.asArrayDouble("layerBoundaries");
    Vector<Double> lT = configuration.asArrayDouble("layerTemperatures");
    std::transform(lB.begin(), lB.end(), std::back_inserter(layerBoundaries),
      [](Double x) {return atm::Length(x, atm::Length::UnitMeter);});
    std::transform(lT.begin(), lT.end(), std::back_inserter(layerTemperatures),
      [](Double x) {return atm::Temperature(x, atm::Temperature::UnitKelvin);});
    if (layerBoundaries.size() != layerTemperatures.size()) {
      os << "ERROR: list length of layerboundaries and layertemperature should be the same." << LogIO::EXCEPTION;
    }
  }
  os << "user-defined layer:" << endl;
  if (layerBoundaries.size() > 0) {
    for (size_t i = 0; i < layerBoundaries.size(); ++i) {
      os << "  Height " << layerBoundaries[i].get(atm::Length::UnitMeter)
         << " Temperature " << layerTemperatures[i].get(atm::Temperature::UnitKelvin)
         << endl;
    }
    os << LogIO::POST;
  } else {
    os << "NONE" << LogIO::POST;
  }

  os << LogIO::DEBUGGING << "creating ATMProfile start " << LogIO::POST;
  atm::AtmProfile atmProfile(
    altitude,
    defaultPressure,
    defaultTemperature,
    tropoLapseRate,
    defaultRelHumidity,
    wvScaleHeight,
    pressureStep,
    pressureStepFactor,
    topAtmProfile,
    atmType_,
    layerBoundaries,
    layerTemperatures
  );
  os << LogIO::DEBUGGING << "creating ATMProfile end " << LogIO::POST;


  // disable threshold so that any subtle changes in
  // temperature/pressure/humidity/pwv cause to udpate
  // the model
  os << LogIO::DEBUGGING << "disabling thresholds start " << LogIO::POST;
  atmProfile.setBasicAtmosphericParameterThresholds(
    atm::Length(0.0, atm::Length::UnitMeter),
    atm::Pressure(0.0, atm::Pressure::UnitMilliBar),
    atm::Temperature(0.0, atm::Temperature::UnitKelvin),
    0.0,
    atm::Humidity(0.0, atm::Humidity::UnitPercent),
    atm::Length(0.0, atm::Length::UnitMeter)
  );
  os << LogIO::DEBUGGING << "disabling thresholds end " << LogIO::POST;

  std::map<SpwId, Vector<Double> > channelFreqsPerSpw;
  std::map<SpwId, Vector<Double> > channelWidthsPerSpw;
  for (auto i = processSpwList_.begin(); i != processSpwList_.end(); ++i) {
    SpwId const spw = *i;
    channelFreqsPerSpw[spw] = spectralWindowSubtablecols().chanFreq().get(spw);
    channelWidthsPerSpw[spw] = spectralWindowSubtablecols().chanWidth().get(spw);
  }

  int const numThreads = min(numThreads_, static_cast<int>(processSpwList_.size()));
  #pragma omp parallel for num_threads(numThreads) if(numThreads > 1)
  for (unsigned int i = 0; i < processSpwList_.size(); ++i) {
    SpwId const spw = processSpwList_[i];
    #pragma omp critical (logging)
    {
      os << "creating SkyStatus for spw " << spw << LogIO::POST;
    }
    Vector<Double> cf = channelFreqsPerSpw[spw];
    Vector<Double> cw = channelWidthsPerSpw[spw];
    unsigned int nchan = cf.nelements();

    unsigned int refChan = (nchan - 1) / 2;
    double centerFreq = cf[refChan];
    Double chanSep = (nchan == 1u) ? cw[0] : (cf[nchan - 1] - cf[0]) / static_cast<Double>(nchan - 1);
    if (isTdmSpw_[spw]) {
      // configure 5x finer spectral grid than native one
      // align frequency for channel 0
      chanSep /= static_cast<Double>(kNumResolveTDM);
      nchan *= kNumResolveTDM;
      refChan = refChan * kNumResolveTDM + (kNumResolveTDM - 1) / 2u;
      centerFreq += static_cast<Double>(((kNumResolveTDM - 1) / 2u)) * chanSep;
    }

    // SpectralGrid
    #pragma omp critical (logging)
    {
      os.output() << std::setprecision(16);
      os << "SpectralGrid for spw " << spw << ": nchan " << nchan
         << " refchan " << refChan << " center freq "
         << centerFreq << " chan sep " << chanSep << LogIO::POST;
    }
    atm::SpectralGrid spectralGrid(nchan, refChan, centerFreq, chanSep);

    // SkyStatus
    #pragma omp critical (logging)
    {
      os << "initializing SkyStatus for " << spw << LogIO::POST;
    }
    atm::RefractiveIndexProfile profile(spectralGrid, atmProfile);
    atmSkyStatusPerSpw_[spw].reset(new atm::SkyStatus(profile));
    atmSkyStatusPerSpw_[spw]->setUserWH2O(atm::Length(defaultPwvValue, atm::Length::UnitMilliMeter));
  }
  os << "DONE Initializing SkyStatus" << LogIO::POST;

}

Bool SDAtmosphereCorrectionTVI::updateSkyStatus(atm::SkyStatus &skyStatus, Int atmTimeIndex) {
  LogIO os(LogOrigin("SDAtmosphereCorrectionTVI", __func__, WHERE));
  // os << "updateSkyStatus for SPW " << currentSpwId_ << LogIO::POST;

  bool isSkyStatusOutdated = false;
  Double currentTemperatureValue = userTemperatureValue_;
  if (isValueUnset(currentTemperatureValue)) {
    Double const val = atmTemperatureData_[atmTimeIndex];
    Double const val2 = skyStatus.getGroundTemperature().get(atm::Temperature::UnitKelvin);
    if (!isCloseDouble(val, val2)) {
      isSkyStatusOutdated = true;
    }
    currentTemperatureValue = val;
  }

  Double currentPressureValue = userPressureValue_;
  if (isValueUnset(currentPressureValue)) {
    Double const val = atmPressureData_[atmTimeIndex];
    Double const val2 = skyStatus.getGroundPressure().get(atm::Pressure::UnitMilliBar);
    if (!isCloseDouble(val, val2)) {
      isSkyStatusOutdated = true;
    }
    currentPressureValue = val;
  }

  Double currentRelHumidityValue = userRelHumidityValue_;
  if (isValueUnset(currentRelHumidityValue)) {
    Double const val = atmRelHumidityData_[atmTimeIndex];
    Double const val2 = skyStatus.getRelativeHumidity().get(atm::Humidity::UnitPercent);
    if (!isCloseDouble(val, val2)) {
      isSkyStatusOutdated = true;
    }
    currentRelHumidityValue = val;
  }

  if (isSkyStatusOutdated) {
    // os << "updating SkyStatus for SPW " << currentSpwId_ << LogIO::POST;
    skyStatus.setBasicAtmosphericParameters(
      skyStatus.getAltitude(),
      atm::Pressure(currentPressureValue, atm::Pressure::UnitMilliBar),
      atm::Temperature(currentTemperatureValue, atm::Temperature::UnitKelvin),
      skyStatus.getTropoLapseRate(),
      atm::Humidity(currentRelHumidityValue, atm::Humidity::UnitPercent),
      skyStatus.getWvScaleHeight()
    );
  }

  // PYTHON IMPLEMENTATION
  // share time information with CalAtmosphere table
  Bool isPwvOutdated = False;
  if (isValueUnset(userPwvValue_)) {
    Double const val = pwvData_[atmTimeIndex];
    Double const val2 = skyStatus.getUserWH2O().get(atm::Length::UnitMilliMeter);
    if (!isCloseDouble(val, val2)) {
      // cout << "PWV is different " << val << " vs " << val2 << endl;
      // os.output() << std::setprecision(16);
      // os << "PWV val = " << val << " val2 = " << val2 << LogIO::POST;
      isPwvOutdated = True;
      // os << "updating PWV value for SPW " << currentSpwId_ << LogIO::POST;
      skyStatus.setUserWH2O(atm::Length(val, atm::Length::UnitMilliMeter));
    }
  }

  // cout << "time " << currentTime_
  //      << " Temperature " << atmTemperatureData_[atmTimeIndex] << "K "
  //      << " Pressure " << atmPressureData_[atmTimeIndex] << "mbar "
  //      << " Humidity " << atmRelHumidityData_[atmTimeIndex] << "%" << endl;

  // os << "DONE updateSkyStatus for SPW " << currentSpwId_ << LogIO::POST;

    return (isSkyStatusOutdated || isPwvOutdated);
}

Vector<Double> SDAtmosphereCorrectionTVI::updateCorrectionFactor(atm::SkyStatus &p, Double const currentTime) {
  // #pragma omp critical (logging)
  // {
  //   cout << std::setprecision(16)
  //        << "time " << currentTime << " thread " << omp_get_thread_num() << endl;
  // }
  // LogIO os(LogOrigin("SDAtmosphereCorrectionTVI", __func__, WHERE));

  // find OFF_SOURCE time stamps that brackets current time stamp
  std::pair<Int, Int> nearestIndex = findNearestIndex(offSourceTime_, currentTime);
  Int const i0 = nearestIndex.first;
  Int const i1 = nearestIndex.second;
  Double const offSourceTimePrev =
    (0 <= i0 && static_cast<uInt>(i0) < offSourceTime_.size()) ?
    offSourceTime_[i0] : kValueUnset;
  Double const offSourceTimeNext =
    (0 <= i1 && static_cast<uInt>(i1) < offSourceTime_.size()) ?
    offSourceTime_[i1] : kValueUnset;
  // cout << "nearest: " << std::setprecision(16) << offSourceTimePrev << "~" << offSourceTimeNext << endl;

  // elevation
  Double const elevationOn = elevationInterpolator_(currentTime);

  // cout << "interpolated elevation" << endl;
  Double elevationOff = kValueUnset;
  if (isValueUnset(offSourceTimePrev) && isValueUnset(offSourceTimeNext)) {
    #pragma omp critical
    {
      LogIO os(LogOrigin("SDAtmosphereCorrectionTVI", __func__, WHERE));
      os << "No valid OFF_SOURCE observaton." << LogIO::EXCEPTION;
    }
  } else if (isValueUnset(offSourceTimePrev)) {
    elevationOff = elevationInterpolator_(offSourceTimeNext);
  } else if (isValueUnset(offSourceTimeNext)) {
    elevationOff = elevationInterpolator_(offSourceTimePrev);
  } else {
    Double const elevationOffPrev = elevationInterpolator_(offSourceTimePrev);
    // cout << "prev: " << elevationOffPrev << endl;
    Double const elevationOffNext = elevationInterpolator_(offSourceTimeNext);
    // cout << "next: " << elevationOffNext << endl;
    elevationOff =
      ((offSourceTimeNext - currentTime) * elevationOffPrev +
       (currentTime - offSourceTimePrev) * elevationOffNext) /
      (offSourceTimeNext - offSourceTimePrev);
  }
  // cout << "ON: " << elevationOn << endl;
  // cout << "OFF: " << elevationOffPrev << endl;
  // cout << "time " << std::setprecision(16) << currentTime << " elON " << elevationOn << " elOFF " << elevationOff << endl;

  // opacity
  unsigned int const nchan = p.getNumChan();
  Double const airMassOn = 1.0 / cos(C::pi_2 - elevationOn);
  p.setAirMass(airMassOn);
  Vector<Double> trjSkySpecOn = getTrjSkySpec(&p, nchan);
  Double const airMassOff = 1.0 / cos(C::pi_2 - elevationOff);
  p.setAirMass(airMassOff);
  Vector<Double> trjSkySpecOff = getTrjSkySpec(&p, nchan);
  Vector<Double> tauSpecOff = getTauSpec(&p, nchan, airMassOff);
  Vector<Double> correctionFactor = getCorrectionFactor(trjSkySpecOn, trjSkySpecOff, tauSpecOff);

  // constexpr size_t ic = 0;
  // cout << std::setprecision(16)
  //      << "time = " << currentTime
  //      << " TrjON = " << trjSkySpecOn[ic]
  //      << " TrjOFF = " << trjSkySpecOff[ic]
  //      << " tauOFF = " << tauSpecOff[ic]
  //      << " factor = " << correctionFactor[ic] << " (without gain factor)"
  //      << endl;

  // current spw
  Vector<Double> returnValue;
  if (isTdmSpw_[currentSpwId_]) {
    // cout << "SPW " << currentSpwId_ << " is TDM " << endl;
    Vector<Double> smoothedCorrectionFactor = convolve1DHanning(correctionFactor);
    // atm frequency grid is 5x finer than native frequency grid
    uInt const nativeNumChan = nchanPerSpw_[currentSpwId_];
    returnValue.resize(nativeNumChan);
    for (uInt i = 0; i < nativeNumChan; ++i) {
      returnValue[i] = smoothedCorrectionFactor[i * kNumResolveTDM];
    }
  } else if (doSmooth_[currentSpwId_]) {
    returnValue = convolve1DTriangle(correctionFactor);
  } else {
    returnValue.reference(correctionFactor);
  }

  // apply gain factor
  returnValue *= gainFactorList_[currentSpwId_];

  return returnValue;
}

void SDAtmosphereCorrectionTVI::updateCorrectionFactorInAdvance() {
  LogIO os(LogOrigin("SDAtmosphereCorrectionTVI", __func__, WHERE));

  // no transform by default
  transformSubchunk_ = false;

  if (allNE(processSpwList_, currentSpwId_)) {
    os << LogIO::DEBUGGING << "SPW " << currentSpwId_ << " is not in the processing SPW list" << LogIO::POST;
    return;
  }

  auto const subchunkId = getSubchunkId();
  os << LogIO::DEBUGGING << "Updating correction factor for SPW " << currentSpwId_
     << " subchunk " << subchunkId.first << "-" << subchunkId.second << LogIO::POST;

  Vector<Double> timeData;
  Vector<uInt> indexVector;
  Vector<uInt> uniqueIndex;
  time(timeData);
  sortTime(timeData, indexVector);
  makeUnique(timeData, indexVector, uniqueIndex);
  Vector<Int> stateIdData;
  stateId(stateIdData);
  std::vector<Double> timeListForCorrection;
  timeListForCorrection.reserve(uniqueIndex.size());
  os << LogIO::DEBUGGING
     << "Number of subchunk rows is " << timeData.size() << LogIO::POST;
  os << LogIO::DEBUGGING
     << "There are " << uniqueIndex.size()
     << " unique timestamps in the current subchunk" << LogIO::POST;
  for (uInt i = 0; i < uniqueIndex.size(); ++i) {
    uInt j = uniqueIndex[i];
    Int const sid = stateIdData[j];
    static std::string const startstr("OBSERVE_TARGET#ON_SOURCE");
    casacore::String s = stateSubtablecols().obsMode().get(sid);
    bool isOnSource = s.startsWith(startstr);
    bool isPrecedingAtmDataExists = (atmTime_[0] <= timeData[j]);
    if (isOnSource && isPrecedingAtmDataExists) {
      os.output() << std::setprecision(16);
      os << LogIO::DEBUGGING << "adding " << timeData[j] << LogIO::POST;
      timeListForCorrection.push_back(timeData[j]);
    }
  }
  os << LogIO::DEBUGGING
     << "There are " << timeListForCorrection.size() << " ON_SOURCE "
     << "timestamps in the current subchunk" << LogIO::POST;

  if (timeListForCorrection.size() == 0) {
    return;
  }

  correctionFactorList_.resize(timeListForCorrection.size());

  std::map<Int, std::vector<unsigned int> > groupByAtmTimeIndex;
  for (unsigned int i = 0; i < timeListForCorrection.size(); ++i) {
    Double const currentTime = timeListForCorrection[i];
    std::pair<Int, Int> pair = findNearestIndex(atmTime_, currentTime);
    Int const currentAtmTimeIndex = pair.first;
    if (currentAtmTimeIndex < 0 || atmTime_.nelements() <= static_cast<uInt>(currentAtmTimeIndex)) {
      os << "Internal Error: wrong ON_SOURCE time." << LogIO::EXCEPTION;
    }
    groupByAtmTimeIndex[currentAtmTimeIndex].push_back(i);
  }

  atm::SkyStatus &skyStatus = *(atmSkyStatusPerSpw_[currentSpwId_].get());
  for (auto iter = groupByAtmTimeIndex.begin(); iter != groupByAtmTimeIndex.end(); ++iter) {
    Int const atmTimeIndex = iter->first;
    std::vector<unsigned int> timeIndexList = iter->second;
    Bool const isSkyStatusUpdated = updateSkyStatus(skyStatus, atmTimeIndex);
    if (isSkyStatusUpdated) {
    os << "Initializing ATM profile. SPW " << currentSpwId_
       << " Data timestamp " << timeToString(timeListForCorrection[timeIndexList[0]])
       << " weather data timestamp " << timeToString(atmTime_[atmTimeIndex])
       << LogIO::POST;
    }
    atm::SkyStatus ss(skyStatus);
    int const numThreads = min(numThreads_, static_cast<int>(timeIndexList.size()));
    #pragma omp parallel for firstprivate(ss) num_threads(numThreads) if(numThreads > 1)
    for (unsigned int i = 0; i < timeIndexList.size(); ++i) {
      Double const t = timeListForCorrection[timeIndexList[i]];
      Vector<Double> correctionFactor = updateCorrectionFactor(ss, t);
      correctionFactorList_[timeIndexList[i]].assign(correctionFactor);
      // #pragma omp critical (logging)
      // {
      //   os << "THREAD " << omp_get_thread_num() << ": ";
      //   os.output() << std::setprecision(16);
      //   os << "time " << t << " index " << i
      //      << " correctionFactor[0] " << correctionFactor[0] << LogIO::POST;
      // }
    }
  }

  indexForCorrection_.resize(timeData.size());
  for (unsigned int i = 0; i < timeData.size(); ++i) {
    Double const t = timeData[i];
    auto const pos = std::find(timeListForCorrection.begin(), timeListForCorrection.end(), t);
    if (pos == timeListForCorrection.end()) {
      indexForCorrection_[i] = kInvalidIndex;
    } else {
      indexForCorrection_[i] = std::distance(timeListForCorrection.begin(), pos);
    }
    // os << "time " << t << " index for correction " << indexForCorrection_[i] << LogIO::POST;
  }

  // turn on transformation if any rows require correction
  if (std::any_of(indexForCorrection_.begin(), indexForCorrection_.end(),
                  [](casacore::Int x) {return x != kInvalidIndex;})) {
    transformSubchunk_ = true;
  }
}

void SDAtmosphereCorrectionTVI::readMain(String const &msName) {
  MeasurementSet msObj(msName);
  MSMetaData meta(&msObj, kNoCache);
  std::set<Double> timeOffSourceSet = meta.getTimesForIntent("OBSERVE_TARGET#OFF_SOURCE");
  std::vector<Double> timeOffSource(timeOffSourceSet.begin(), timeOffSourceSet.end());
  size_t numBoundaries = 0;
  Vector<uInt> idx1(timeOffSource.size());
  Double cache = timeOffSource[0];
  idx1[numBoundaries++] = 0;
  for (uInt i = 0; i < timeOffSource.size(); ++i) {
    Double diff = timeOffSource[i] - cache;
    // PYTHON IMPLEMENTATION
    // threshold (1s) is taken from original script by T. Sawada
    if (diff > 1.0) {
      idx1[numBoundaries++] = i;
    }
    cache = timeOffSource[i];
  }
  Vector<uInt> idx2(numBoundaries);
  for (uInt i = 0; i < numBoundaries - 1; ++i) {
    idx2[i] = idx1[i + 1] - 1;
  }
  idx2[numBoundaries - 1] = timeOffSource.size() - 1;
  offSourceTime_.resize(numBoundaries);
  for (size_t i = 0; i < numBoundaries; ++i) {
    offSourceTime_[i] = (timeOffSource[idx1[i]] + timeOffSource[idx2[i]]) / 2.0;
  }
  Vector<Double> tmp(numBoundaries);
  for (auto i = 0u; i < numBoundaries; ++i) {
    tmp[i] = offSourceTime_[i] - offSourceTime_[0];
  }
}

void SDAtmosphereCorrectionTVI::readPointing(String const &msName, Int const referenceAntenna) {
  MeasurementSet const ms(msName);
  uInt numAntennas = ms.antenna().nrow();
  if (referenceAntenna < 0 || numAntennas <= static_cast<uInt>(referenceAntenna)) {
    LogIO os(LogOrigin("SDAtmosphereCorrectionTVI", __func__, WHERE));
    os << "ERROR: reference antenna " << referenceAntenna << " is invalid." << LogIO::EXCEPTION;
  }

  LogIO os(LogOrigin("SDAtmosphereCorrectionTVI", __func__, WHERE));
  Table const pointingTable = ms.pointing();
  Table selected = pointingTable(pointingTable.col("ANTENNA_ID") == referenceAntenna);
  Vector<Double> const elevationTime = ScalarColumn<Double>(selected, "TIME").getColumn();
  ArrayColumn<Double> directionColumn(selected, "DIRECTION");
  auto const columnKeywords = directionColumn.keywordSet();
  String const directionRef = columnKeywords.asRecord("MEASINFO").asString("Ref");
  if (!directionRef.startsWith("AZEL")) {
    LogIO os(LogOrigin("SDAtmosphereCorrectionTVI", __func__, WHERE));
    os << "Direction reference " << directionRef << " is not supported. "
       << "Reference should be AZEL-like." << LogIO::EXCEPTION;
  }
  Array<Double> directionArray = directionColumn.getColumn();
  Array<Double> elevationArray = Cube<Double>(directionArray).yzPlane(1);
  Vector<Double> const elevationData(elevationArray);
  elevationInterpolator_ = Interpolate1D<Double, Double>(
    ScalarSampledFunctional<Double>(elevationTime),
    ScalarSampledFunctional<Double>(elevationData), True, True);
  elevationInterpolator_.setMethod(Interpolate1D<Double, Double>::linear);
  // os.output() << std::setprecision(16);
  // os << "elevationTime = " << elevationTime << LogIO::POST;
  // os << "elevationData = " << elevationData << LogIO::POST;
}

void SDAtmosphereCorrectionTVI::readAsdmAsIsTables(String const &msName) {
  Vector<Double> pwvTimeLocal;
  Vector<Double> atmTimeLocal;
  Vector<Double> pwvDataLocal;
  Vector<Double> atmTemperatureDataLocal;
  Vector<Double> atmPressureDataLocal;
  Vector<Double> atmRelHumidityDataLocal;

  File const calWvrTableName(msName + "/ASDM_CALWVR");
  LogIO os(LogOrigin("SDAtmosphereCorrectionTVI", __func__, WHERE));
  os << "CALWVR table \"" << calWvrTableName.path().absoluteName() << "\"" << LogIO::POST;
  if (calWvrTableName.exists()) {
    Table const calWvrTable(calWvrTableName.path().absoluteName());
    pwvTimeLocal.reference(ScalarColumn<Double>(calWvrTable, "startValidTime").getColumn());
    pwvDataLocal.reference(ScalarColumn<Double>(calWvrTable, "water").getColumn());
  }

  File const calAtmosphereTableName(msName + "/ASDM_CALATMOSPHERE");
  os << "CALATMOSPHERE table \"" << calAtmosphereTableName.path().absoluteName() << "\"" << LogIO::POST;
  if (calAtmosphereTableName.exists()) {
    Table const calAtmosphereTable(calAtmosphereTableName.path().absoluteName());
    atmTimeLocal.reference(ScalarColumn<Double>(calAtmosphereTable, "startValidTime").getColumn());
    atmTemperatureDataLocal.reference(ScalarColumn<Double>(calAtmosphereTable, "groundTemperature").getColumn());
    atmPressureDataLocal.reference(ScalarColumn<Double>(calAtmosphereTable, "groundPressure").getColumn());
    atmRelHumidityDataLocal.reference(ScalarColumn<Double>(calAtmosphereTable, "groundRelHumidity").getColumn());
  }

  if (atmTimeLocal.nelements() > 0) {
    Vector<uInt> uniqueVector;
    Vector<uInt> indexVector;
    sortTime(atmTimeLocal, indexVector);
    uInt n = makeUnique(atmTimeLocal, indexVector, uniqueVector);
    atmTime_.resize(n);
    for (uInt i = 0; i < n; ++i) {
      atmTime_[i] = atmTimeLocal[indexVector[uniqueVector[i]]];
    }
    atmTemperatureData_ = getMedianDataPerTime(n, uniqueVector, indexVector, atmTemperatureDataLocal);
    // Pa -> mbar (=hPa)
    atmPressureData_ = getMedianDataPerTime(n, uniqueVector, indexVector, atmPressureDataLocal) / 100.0;
    atmRelHumidityData_ = getMedianDataPerTime(n, uniqueVector, indexVector, atmRelHumidityDataLocal);

    // PYTHON IMPLEMENTATION
    // use time stamp in CALATMOSPHERE table
    std::vector<uInt> ivec;
    std::vector<uInt> uvec;
    for (uInt i = 0; i < n; ++i) {
      Vector<Double> diff = abs(pwvTimeLocal - atmTime_[i]);
      Double minDiff = min(diff);
      uvec.push_back(ivec.size());
      for (uInt j = 0; j < pwvTimeLocal.nelements(); ++j) {
        if (diff[j] == minDiff) {
          ivec.push_back(j);
        }
      }
    }
    Vector<uInt> indexVectorPwv(ivec);
    Vector<uInt> uniqueVectorPwv(uvec);
    os << LogIO::DEBUGGING << "ivec = " << indexVectorPwv << LogIO::POST;
    os << LogIO::DEBUGGING << "uvec = " << uniqueVectorPwv << LogIO::POST;
    // m -> mm
    pwvData_ = getMedianDataPerTime(n, uniqueVectorPwv, indexVectorPwv, pwvDataLocal) * 1000.0;
  }

  if (atmTime_.nelements() == 0) {
    os << "No Atmosphere measurements. Unable to apply offline ATM correction."
       << LogIO::EXCEPTION;
  } else {
    for (uInt i = 0; i < atmTime_.nelements(); ++i) {
      os << "PWV = " << pwvData_[i] << "mm, "
         << "T = " << atmTemperatureData_[i] << "K, "
         << "P = " << atmPressureData_[i] << "hPa, "
         << "H = " << atmRelHumidityData_[i] << "% "
         << "at " << timeToString(atmTime_[i]) << LogIO::POST;
    }
  }
}

//////////
// SDAtmosphereCorrectionTVIFactory
/////////
SDAtmosphereCorrectionVi2Factory::SDAtmosphereCorrectionVi2Factory(Record const &configuration,
    ViImplementation2 *inputVII) :
    ViFactory(),
    inputVII_p(inputVII),
    configuration_p(configuration) {
}

SDAtmosphereCorrectionVi2Factory::SDAtmosphereCorrectionVi2Factory(Record const &configuration,
    MeasurementSet const *ms, SortColumns const sortColumns,
    Double timeInterval, Bool isWritable) :
    ViFactory(),
    inputVII_p(nullptr),
    configuration_p(configuration) {
  inputVII_p = new VisibilityIteratorImpl2(Block<MeasurementSet const *>(1, ms),
      sortColumns, timeInterval, isWritable);
}

ViImplementation2 * SDAtmosphereCorrectionVi2Factory::createVi() const {
  if (inputVII_p->getNMs() != 1) {
    throw AipsError("ERROR: Multiple or zero MS inputs are given. SDAtmosphereCorrectionTVI works with only one MS.");
  }
  return new SDAtmosphereCorrectionTVI(inputVII_p, configuration_p);
}

SDAtmosphereCorrectionTVILayerFactory::SDAtmosphereCorrectionTVILayerFactory(
    Record const &configuration) :
    ViiLayerFactory(),
    configuration_p(configuration) {
}

ViImplementation2*
SDAtmosphereCorrectionTVILayerFactory::createInstance(ViImplementation2* vii0) const {
  // Make the SDAtmosphereCorrectionTVI, using supplied ViImplementation2, and return it
  SDAtmosphereCorrectionVi2Factory factory(configuration_p, vii0);
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
