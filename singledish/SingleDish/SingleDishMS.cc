#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <sys/time.h>
#include <vector>

#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/Vector.h>
#include <casa/Containers/Allocator.h>
#include <casa/Containers/Block.h>
#include <casa/Logging/LogIO.h>
#include <casa/Logging/LogOrigin.h>
#include <casa/Quanta/MVTime.h>
#include <casa/Utilities/Assert.h>
#include <casa/Utilities/GenSort.h>
#include <ms/MeasurementSets/MSSpectralWindow.h>
#include <ms/MSSel/MSSelection.h>
#include <ms/MSSel/MSSelectionTools.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <msvis/MSVis/VisSetUtil.h>
#include <scimath/Fitting/GenericL2Fit.h>
#include <scimath/Fitting/NonLinearFitLM.h>
#include <scimath/Functionals/CompiledFunction.h>
#include <scimath/Functionals/CompoundFunction.h>
#include <scimath/Functionals/Function.h>
#include <scimath/Functionals/Gaussian1D.h>
#include <scimath/Functionals/Lorentzian1D.h>
#include <scimath/Mathematics/Convolver.h>
#include <scimath/Mathematics/VectorKernel.h>
#include <singledish/SingleDish/BaselineTable.h>
#include <singledish/SingleDish/BLParameterParser.h>
#include <singledish/SingleDish/LineFinder.h>
#include <singledish/SingleDish/SingleDishMS.h>
#include <stdcasa/StdCasa/CasacSupport.h>
#include <tables/Tables/ScalarColumn.h>

// for importasap and importnro
#include <singledishfiller/Filler/NRO2MSReader.h>
#include <singledishfiller/Filler/Scantable2MSReader.h>
#include <singledishfiller/Filler/SingleDishMSFiller.h>

#define _ORIGIN LogOrigin("SingleDishMS", __func__, WHERE)

namespace {
  // Max number of rows to get in each iteration
  constexpr casacore::Int kNRowBlocking = 1000;
  // Sinusoid
  constexpr int SinusoidWaveNumber_kUpperLimit = -999;
  // Weight
  constexpr size_t WeightIndex_kStddev = 0;
  constexpr size_t WeightIndex_kRms = 1;
  constexpr size_t WeightIndex_kNum = 2;

double gettimeofday_sec() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + (double) tv.tv_usec * 1.0e-6;
}

using casa::vi::VisBuffer2;
using casacore::Matrix;
using casacore::Cube;
using casacore::Float;
using casacore::Complex;
using casacore::AipsError;

template<class CUBE_ACCESSOR>
struct DataAccessorInterface {
  static void GetCube(VisBuffer2 const &vb, Cube<Float> &cube) {
    real(cube, CUBE_ACCESSOR::GetVisCube(vb));
  }
  static void GetSlice(VisBuffer2 const &vb, size_t const iPol,
      Matrix<Float> &cubeSlice) {
    real(cubeSlice, CUBE_ACCESSOR::GetVisCube(vb).yzPlane(iPol));
  }
};

struct DataAccessor: public DataAccessorInterface<DataAccessor> {
  static Cube<Complex> GetVisCube(VisBuffer2 const &vb) {
    return vb.visCube();
  }
};

struct CorrectedDataAccessor: public DataAccessorInterface<CorrectedDataAccessor> {
  static Cube<Complex> GetVisCube(VisBuffer2 const &vb) {
    return vb.visCubeCorrected();
  }
};

struct FloatDataAccessor {
  static void GetCube(VisBuffer2 const &vb, Cube<Float> &cube) {
    cube = GetVisCube(vb);
  }
  static void GetSlice(VisBuffer2 const &vb, size_t const iPol,
      Matrix<Float> &cubeSlice) {
    cubeSlice = GetVisCube(vb).yzPlane(iPol);
  }
private:
  static Cube<Float> GetVisCube(VisBuffer2 const &vb) {
    return vb.visCubeFloat();
  }
};

inline void GetCubeFromData(VisBuffer2 const &vb, Cube<Float> &cube) {
  DataAccessor::GetCube(vb, cube);
}

inline void GetCubeFromCorrected(VisBuffer2 const &vb, Cube<Float> &cube) {
  CorrectedDataAccessor::GetCube(vb, cube);
}

inline void GetCubeFromFloat(VisBuffer2 const &vb, Cube<Float> &cube) {
  FloatDataAccessor::GetCube(vb, cube);
}

inline void GetCubeDefault(VisBuffer2 const& /*vb*/, Cube<Float>& /*cube*/) {
  throw AipsError("Data accessor for VB2 is not properly configured.");
}

inline void compute_weight(size_t const num_data,
                           float const data[/*num_data*/],
                           bool const mask[/*num_data*/],
                           std::vector<float>& weight) {
  for (size_t i = 0; i < WeightIndex_kNum; ++i) {
    weight[i] = 0.0;
  }

  int num_data_effective = 0;
  double sum = 0.0;
  double sum_sq = 0.0;
  for (size_t i = 0; i < num_data; ++i) {
    if (mask[i]) {
      num_data_effective++;
      sum += data[i];
      sum_sq += data[i] * data[i];
    }
  }

  if (num_data_effective > 0) {
    double factor = 1.0 / static_cast<double>(num_data_effective);
    double mean = sum * factor;
    double mean_sq = sum_sq * factor;

    std::vector<double> variance(WeightIndex_kNum);
    variance[WeightIndex_kStddev] = mean_sq - mean * mean;
    variance[WeightIndex_kRms] = mean_sq;

    auto do_compute_weight = [&](size_t idx) {
      if (variance[idx] > 0.0) {
        weight[idx] = static_cast<float>(1.0 / variance[idx]);
      } else {
        LogIO os(_ORIGIN);
        os << "Weight set to 0 for a bad data." << LogIO::WARN;
      }
    };

    do_compute_weight(WeightIndex_kStddev);
    do_compute_weight(WeightIndex_kRms);
  }
}

} // anonymous namespace

using namespace casacore;
namespace casa {

SingleDishMS::SingleDishMS() : msname_(""), sdh_(0) {
  initialize();
}

SingleDishMS::SingleDishMS(string const& ms_name) : msname_(ms_name), sdh_(0) {
  LogIO os(_ORIGIN);
  initialize();
}

SingleDishMS::~SingleDishMS() {
  if (sdh_) {
    delete sdh_;
    sdh_ = 0;
  }
  msname_ = "";
}

void SingleDishMS::initialize() {
  in_column_ = MS::UNDEFINED_COLUMN;
  //  out_column_ = MS::UNDEFINED_COLUMN;
  doSmoothing_ = false;
  doAtmCor_ = false;
  visCubeAccessor_ = GetCubeDefault;
}

bool SingleDishMS::close() {
  LogIO os(_ORIGIN);
  os << "Detaching from SingleDishMS" << LogIO::POST;

  if (sdh_) {
    delete sdh_;
    sdh_ = 0;
  }
  msname_ = "";

  return true;
}

////////////////////////////////////////////////////////////////////////
///// Common utility functions
////////////////////////////////////////////////////////////////////////
void SingleDishMS::setSelection(Record const &selection, bool const verbose) {
  LogIO os(_ORIGIN);
  if (!selection_.empty()) // selection is set before
    os << "Discard old selection and setting new one." << LogIO::POST;
  if (selection.empty()) // empty selection is passed
    os << "Resetting selection." << LogIO::POST;

  selection_ = selection;
  // Verbose output
  bool any_selection(false);
  if (verbose && !selection_.empty()) {
    String timeExpr(""), antennaExpr(""), fieldExpr(""), spwExpr(""),
           uvDistExpr(""), taQLExpr(""), polnExpr(""), scanExpr(""), arrayExpr(""),
           obsExpr(""), intentExpr("");
    timeExpr = get_field_as_casa_string(selection_, "timerange");
    antennaExpr = get_field_as_casa_string(selection_, "antenna");
    fieldExpr = get_field_as_casa_string(selection_, "field");
    spwExpr = get_field_as_casa_string(selection_, "spw");
    uvDistExpr = get_field_as_casa_string(selection_, "uvdist");
    taQLExpr = get_field_as_casa_string(selection_, "taql");
    polnExpr = get_field_as_casa_string(selection_, "correlation");
    scanExpr = get_field_as_casa_string(selection_, "scan");
    arrayExpr = get_field_as_casa_string(selection_, "array");
    intentExpr = get_field_as_casa_string(selection_, "intent");
    obsExpr = get_field_as_casa_string(selection_, "observation");
    // selection Summary
    os << "[Selection Summary]" << LogIO::POST;
    if (obsExpr != "") {
      any_selection = true;
      os << "- Observation: " << obsExpr << LogIO::POST;
    }
    if (antennaExpr != "") {
      any_selection = true;
      os << "- Antenna: " << antennaExpr << LogIO::POST;
    }
    if (fieldExpr != "") {
      any_selection = true;
      os << "- Field: " << fieldExpr << LogIO::POST;
    }
    if (spwExpr != "") {
      any_selection = true;
      os << "- SPW: " << spwExpr << LogIO::POST;
    }
    if (polnExpr != "") {
      any_selection = true;
      os << "- Pol: " << polnExpr << LogIO::POST;
    }
    if (scanExpr != "") {
      any_selection = true;
      os << "- Scan: " << scanExpr << LogIO::POST;
    }
    if (timeExpr != "") {
      any_selection = true;
      os << "- Time: " << timeExpr << LogIO::POST;
    }
    if (intentExpr != "") {
      any_selection = true;
      os << "- Intent: " << intentExpr << LogIO::POST;
    }
    if (arrayExpr != "") {
      any_selection = true;
      os << "- Array: " << arrayExpr << LogIO::POST;
    }
    if (uvDistExpr != "") {
      any_selection = true;
      os << "- UVDist: " << uvDistExpr << LogIO::POST;
    }
    if (taQLExpr != "") {
      any_selection = true;
      os << "- TaQL: " << taQLExpr << LogIO::POST;
    }
    {// reindex
      Int ifield;
      ifield = selection_.fieldNumber(String("reindex"));
      if (ifield > -1) {
        Bool reindex = selection_.asBool(ifield);
        os << "- Reindex: " << (reindex ? "ON" : "OFF" ) << LogIO::POST;
      }
    }
    if (!any_selection)
      os << "No valid selection parameter is set." << LogIO::WARN;
  }
}

void SingleDishMS::setAverage(Record const &average, bool const verbose) {
  LogIO os(_ORIGIN);
  if (!average_.empty()) // average is set before
    os << "Discard old average and setting new one." << LogIO::POST;
  if (average.empty()) // empty average is passed
    os << "Resetting average." << LogIO::POST;

  average_ = average;

  if (verbose && !average_.empty()) {
    LogIO os(_ORIGIN);
    Int ifield;
    ifield = average_.fieldNumber(String("timeaverage"));
    os << "[Averaging Settings]" << LogIO::POST;
    if (ifield < 0 || !average_.asBool(ifield)) {
      os << "No averaging will be done" << LogIO::POST;
      return;
    }

    String timebinExpr(""), timespanExpr(""), tweightExpr("");
    timebinExpr = get_field_as_casa_string(average_, "timebin");
    timespanExpr = get_field_as_casa_string(average_, "timespan");
    tweightExpr = get_field_as_casa_string(average_, "tweight");
    if (timebinExpr != "") {
      os << "- Time bin: " << timebinExpr << LogIO::POST;
    }
    if (timespanExpr != "") {
      os << "- Time span: " << timespanExpr << LogIO::POST;
    }
    if (tweightExpr != "") {
      os << "- Averaging weight: " << tweightExpr << LogIO::POST;
    }

  }
}

void SingleDishMS::setPolAverage(Record const &average, bool const verbose) {
  LogIO os(_ORIGIN);
  if (!pol_average_.empty()) // polaverage is set before
    os << "Discard old average and setting new one." << LogIO::POST;
  if (average.empty()) // empty polaverage is passed
    os << "Resetting average." << LogIO::POST;

  pol_average_ = average;

  if (verbose && !pol_average_.empty()) {
    LogIO os(_ORIGIN);
    Int ifield;
    ifield = pol_average_.fieldNumber(String("polaverage"));
    os << "[Polarization Averaging Settings]" << LogIO::POST;
    if (ifield < 0 || !pol_average_.asBool(ifield)) {
      os << "No polarization averaging will be done" << LogIO::POST;
      return;
    }

    String polAverageModeExpr("");
    polAverageModeExpr = get_field_as_casa_string(pol_average_, "polaveragemode");
    if (polAverageModeExpr != "") {
      os << "- Mode: " << polAverageModeExpr << LogIO::POST;
    }
  }
}

String SingleDishMS::get_field_as_casa_string(Record const &in_data,
                                              string const &field_name) {
  Int ifield;
  ifield = in_data.fieldNumber(String(field_name));
  if (ifield > -1)
    return in_data.asString(ifield);
  return "";
}

bool SingleDishMS::prepare_for_process(string const &in_column_name,
                                       string const &out_ms_name) {
  // Sort by single dish default
  return prepare_for_process(in_column_name, out_ms_name, Block<Int>(), true);
}

bool SingleDishMS::prepare_for_process(string const &in_column_name,
                                       string const &out_ms_name,
                                       Block<Int> const &sortColumns,
                                       bool const addDefaultSortCols) {
  LogIO os(_ORIGIN);
  AlwaysAssert(msname_ != "", AipsError);
  // define a column to read data from
  string in_column_name_lower = in_column_name;
  std::transform(
    in_column_name_lower.begin(),
    in_column_name_lower.end(),
    in_column_name_lower.begin(),
    [](unsigned char c) {return std::tolower(c);}
  );
  if (in_column_name_lower == "float_data") {
    in_column_ = MS::FLOAT_DATA;
    visCubeAccessor_ = GetCubeFromFloat;
  } else if (in_column_name_lower == "corrected") {
    in_column_ = MS::CORRECTED_DATA;
    visCubeAccessor_ = GetCubeFromCorrected;
  } else if (in_column_name_lower == "data") {
    in_column_ = MS::DATA;
    visCubeAccessor_ = GetCubeFromData;
  } else {
    throw(AipsError("Invalid data column name"));
  }
  // destroy SDMSManager
  if (sdh_)
    delete sdh_;
  // Configure record
  Record configure_param(selection_);
  format_selection(configure_param);
  configure_param.define("inputms", msname_);
  configure_param.define("outputms", out_ms_name);
  String in_name(in_column_name);
  in_name.upcase();
  configure_param.define("datacolumn", in_name);
  // merge averaging parameters
  configure_param.merge(average_);
  // The other available keys
  // - buffermode, realmodelcol, usewtspectrum, tileshape,
  // - chanaverage, chanbin, useweights,
  // - combinespws, ddistart, hanning
  // - regridms, phasecenter, restfreq, outframe, interpolation, nspw,
  // - mode, nchan, start, width, veltype,
  // - timeaverage, timebin, timespan, maxuvwdistance

  // smoothing
  configure_param.define("smoothFourier", doSmoothing_);

  // merge polarization averaging parameters
  configure_param.merge(pol_average_);

  // offline ATM correction
  configure_param.define("atmCor", doAtmCor_);
  configure_param.merge(atmCorConfig_);

  // Generate SDMSManager
  sdh_ = new SDMSManager();

  // Configure SDMSManager
  sdh_->configure(configure_param);

  ostringstream oss;
  configure_param.print(oss);
  String str(oss.str());
  os << LogIO::DEBUG1 << " Configuration Record " << LogIO::POST;
  os << LogIO::DEBUG1 << str << LogIO::POST;
  // Open the MS and select data
  sdh_->open();
  sdh_->getOutputMs()->flush();
  // set large timebin if not averaging
  Double timeBin;
  int exists = configure_param.fieldNumber("timebin");
  if (exists < 0) {
    // Avoid TIME col being added to sort columns in MSIter::construct.
    // TIME is automatically added to sort column when
    // timebin is not 0, even if addDefaultSortCols=false.
    timeBin = 0.0;
  } else {
    String timebin_string;
    configure_param.get(exists, timebin_string);
    timeBin = casaQuantity(timebin_string).get("s").getValue();

    Int ifield;
    ifield = configure_param.fieldNumber(String("timeaverage"));
    Bool average_time = ifield < 0 ? false : configure_param.asBool(ifield);
    if (timeBin < 0 || (average_time && timeBin == 0.0))
      throw(AipsError("time bin should be positive"));
  }
  // set sort column
  sdh_->setSortColumns(sortColumns, addDefaultSortCols, timeBin);
  // Set up the Data Handler
  sdh_->setup();
  return true;
}

void SingleDishMS::finalize_process() {
  initialize();
  if (sdh_) {
    sdh_->close();
    delete sdh_;
    sdh_ = 0;
  }
}

void SingleDishMS::format_selection(Record &selection) {
  // At this moment sdh_ is not supposed to be generated yet.
  LogIO os(_ORIGIN);
  // format spw
  String const spwSel(get_field_as_casa_string(selection, "spw"));
  selection.define("spw", spwSel == "" ? "*" : spwSel);

  // Select only auto-correlation
  String autoCorrSel("");
  os << "Formatting antenna selection to select only auto-correlation"
     << LogIO::POST;
  String const antennaSel(get_field_as_casa_string(selection, "antenna"));
  os << LogIO::DEBUG1 << "Input antenna expression = " << antennaSel
     << LogIO::POST;
  if (antennaSel == "") { //Antenna selection is NOT set
    autoCorrSel = String("*&&&");
  } else { //User defined antenna selection
    MeasurementSet MSobj = MeasurementSet(msname_);
    MeasurementSet* theMS = &MSobj;
    MSSelection theSelection;
    theSelection.setAntennaExpr(antennaSel);
    TableExprNode exprNode = theSelection.toTableExprNode(theMS);
    Vector<Int> ant1Vec = theSelection.getAntenna1List();
    os << LogIO::DEBUG1 << ant1Vec.nelements()
       << " antenna(s) are selected. ID = ";
    for (uInt i = 0; i < ant1Vec.nelements(); ++i) {
      os << ant1Vec[i] << ", ";
      if (autoCorrSel != "")
        autoCorrSel += ";";
      autoCorrSel += String::toString(ant1Vec[i]) + "&&&";
    }
    os << LogIO::POST;
  }
  os << LogIO::DEBUG1 << "Auto-correlation selection string: " << autoCorrSel
     << LogIO::POST;

  selection.define("antenna", autoCorrSel);
}

void SingleDishMS::get_data_cube_float(vi::VisBuffer2 const &vb,
                                       Cube<Float> &data_cube) {
//  if (in_column_ == MS::FLOAT_DATA) {
//    data_cube = vb.visCubeFloat();
//  } else { //need to convert Complex cube to Float
//    Cube<Complex> cdata_cube(data_cube.shape());
//    if (in_column_ == MS::DATA) {
//      cdata_cube = vb.visCube();
//    } else { //MS::CORRECTED_DATA
//      cdata_cube = vb.visCubeCorrected();
//    }
//    // convert Complext to Float
//    convertArrayC2F(data_cube, cdata_cube);
//  }
  (*visCubeAccessor_)(vb, data_cube);
}

void SingleDishMS::convertArrayC2F(Array<Float> &to,
                                   Array<Complex> const &from) {
  if (to.nelements() == 0 && from.nelements() == 0) {
    return;
  }
  if (to.shape() != from.shape()) {
    throw(ArrayConformanceError("Array shape differs"));
  }
  Array<Complex>::const_iterator endFrom = from.end();
  Array<Complex>::const_iterator iterFrom = from.begin();
  for (Array<Float>::iterator iterTo = to.begin(); iterFrom != endFrom; ++iterFrom, ++iterTo) {
    *iterTo = iterFrom->real();
  }
}

std::vector<string> SingleDishMS::split_string(string const &s, char delim) {
  std::vector<string> elems;
  string item;
  for (size_t i = 0; i < s.size(); ++i) {
    char ch = s.at(i);
    if (ch == delim) {
      if (!item.empty()) {
        elems.push_back(item);
      }
      item.clear();
    } else {
      item += ch;
    }
  }
  if (!item.empty()) {
    elems.push_back(item);
  }
  return elems;
}

bool SingleDishMS::file_exists(string const &filename) {
  FILE *fp;
  if ((fp = fopen(filename.c_str(), "r")) == NULL)
    return false;
  fclose(fp);
  return true;
}

void SingleDishMS::parse_spw(string const &in_spw,
                             Vector<Int> &rec_spw,
                             Matrix<Int> &rec_chan,
                             Vector<size_t> &nchan,
                             Vector<Vector<Bool> > &mask,
                             Vector<bool> &nchan_set) {
  Record selrec = sdh_->getSelRec(in_spw);
  rec_spw = selrec.asArrayInt("spw");
  rec_chan = selrec.asArrayInt("channel");
  nchan.resize(rec_spw.nelements());
  mask.resize(rec_spw.nelements());
  nchan_set.resize(rec_spw.nelements());
  for (size_t i = 0; i < nchan_set.nelements(); ++i) {
    nchan_set(i) = false;
  }
}

void SingleDishMS::get_nchan_and_mask(Vector<Int> const &rec_spw,
                                      Vector<Int> const &data_spw,
                                      Matrix<Int> const &rec_chan,
                                      size_t const num_chan,
                                      Vector<size_t> &nchan,
                                      Vector<Vector<Bool> > &mask,
                                      Vector<bool> &nchan_set,
                                      bool &new_nchan) {
  new_nchan = false;
  for (size_t i = 0; i < rec_spw.nelements(); ++i) {
    //get nchan by spwid and set to nchan[]
    for (size_t j = 0; j < data_spw.nelements(); ++j) {
      if ((!nchan_set(i)) && (data_spw(j) == rec_spw(i))) {
        bool found = false;
        for (size_t k = 0; k < nchan.nelements(); ++k) {
          if (!nchan_set(k))
            continue;
          if (nchan(k) == num_chan)
            found = true;
        }
        if (!found) {
          new_nchan = true;
        }
        nchan(i) = num_chan;
        nchan_set(i) = true;
        break;
      }
    }
    if (!nchan_set(i))
      continue;
    mask(i).resize(nchan(i));
    // generate mask
    get_mask_from_rec(rec_spw(i), rec_chan, mask(i), true);
  }
}

void SingleDishMS::get_mask_from_rec(Int spwid,
                                     Matrix<Int> const &rec_chan,
                                     Vector<Bool> &mask,
                                     bool initialize) {
  if (initialize) {
    for (size_t j = 0; j < mask.nelements(); ++j) {
      mask(j) = false;
    }
  }
  //construct a list of (start, end, stride, start, end, stride, ...)
  //from rec_chan for the spwid
  std::vector<uInt> edge;
  edge.clear();
  for (size_t j = 0; j < rec_chan.nrow(); ++j) {
    if (rec_chan.row(j)(0) == spwid) {
      edge.push_back(rec_chan.row(j)(1));
      edge.push_back(rec_chan.row(j)(2));
      edge.push_back(rec_chan.row(j)(3));
    }
  }
  //generate mask
  for (size_t j = 0; j < edge.size()-2; j += 3) {
    for (size_t k = edge[j]; k <= edge[j + 1] && k < mask.size(); k += edge[j + 2]) {
      mask(k) = true;
    }
  }
}

void SingleDishMS::get_masklist_from_mask(size_t const num_chan,
                                          bool const *mask,
                                          Vector<uInt> &masklist) {
  size_t const max_num_masklist = num_chan + 1;
  masklist.resize(max_num_masklist);  // clear
  uInt last_idx = num_chan - 1;
  uInt num_masklist = 0;
  auto append = [&](uInt i){
    masklist[num_masklist] = i;
    num_masklist++;
  };

  if (mask[0]) {
    append(0);
  }
  for (uInt i = 1; i < last_idx; ++i) {
    if (!mask[i]) continue;

    // The following if-statements must be judged independently.
    // Don't put them together as a single statement by connecting with '||'.
    if (!mask[i - 1]) {
      append(i);
    }
    if (!mask[i + 1]) {
      append(i);
    }
  }
  if (mask[last_idx]) {
    if ((1 <= last_idx) && (!mask[last_idx - 1])) {
      append(last_idx);
    }
    append(last_idx);
  }
  masklist.resize(num_masklist, true);
}

void SingleDishMS::get_baseline_context(size_t const bltype,
                                        uint16_t order,
                                        size_t num_chan,
                                        Vector<size_t> const &nchan,
                                        Vector<bool> const &nchan_set,
                                        Vector<size_t> &ctx_indices,
                                        std::vector<LIBSAKURA_SYMBOL(LSQFitContextFloat) *> &bl_contexts) {
  size_t idx = 0;
  bool found = false;
  for (size_t i = 0; i < nchan.nelements(); ++i) {
    if ((nchan_set[i])&&(nchan[i] == num_chan)) {
      idx = bl_contexts.size();
      found = true;
      break;
    }
  }
  if (found) {
    for (size_t i = 0; i < nchan.nelements(); ++i) {
      if ((nchan_set[i])&&(nchan[i] == num_chan)) {
        ctx_indices[i] = idx;
      }
    }

    LIBSAKURA_SYMBOL(LSQFitContextFloat) *context;
    LIBSAKURA_SYMBOL(Status) status = LIBSAKURA_SYMBOL(Status_kNG);
    if ((bltype == BaselineType_kPolynomial)||(bltype == BaselineType_kChebyshev)) {
      status = LIBSAKURA_SYMBOL(CreateLSQFitContextPolynomialFloat)(static_cast<LIBSAKURA_SYMBOL(LSQFitType)>(bltype),
                                                                    static_cast<uint16_t>(order),
                                                                    num_chan, &context);
    } else if (bltype == BaselineType_kCubicSpline) {
      status = LIBSAKURA_SYMBOL(CreateLSQFitContextCubicSplineFloat)(static_cast<uint16_t>(order),
                                                                     num_chan, &context);
    } else if (bltype == BaselineType_kSinusoid) {
      status = LIBSAKURA_SYMBOL(CreateLSQFitContextSinusoidFloat)(static_cast<uint16_t>(order),
                                                                  num_chan, &context);
    }
    check_sakura_status("sakura_CreateLSQFitContextFloat", status);
    bl_contexts.push_back(context);
  }
}

void SingleDishMS::get_baseline_context(size_t const bltype,
                                        uint16_t order,
                                        size_t num_chan,
                                        size_t ispw,
                                        Vector<size_t> &ctx_indices,
                                        std::vector<size_t> & ctx_nchans,
                                        std::vector<LIBSAKURA_SYMBOL(LSQFitContextFloat) *> &bl_contexts) {
  AlwaysAssert(bl_contexts.size() == ctx_nchans.size() || bl_contexts.size() == ctx_nchans.size()-1 , AipsError);
  size_t idx = 0;
  bool found = false;
  for (size_t i = 0; i < bl_contexts.size(); ++i) {
    if (ctx_nchans[i] == num_chan) {
      idx = i;
      found = true;
      break;
    }
  }
  if (found) {
    // contexts with the valid number of channels already exists.
    // just update idx to bl_contexts and return.
    ctx_indices[ispw] = idx;
    return;
  }
  // contexts with the number of channels is not yet in bl_contexts.
  // Need to create a new context.
  ctx_indices[ispw] = bl_contexts.size();
  LIBSAKURA_SYMBOL(LSQFitContextFloat) *context;
  LIBSAKURA_SYMBOL(Status) status = LIBSAKURA_SYMBOL(Status_kNG);
  if ((bltype == BaselineType_kPolynomial)||(bltype == BaselineType_kChebyshev)) {
    status = LIBSAKURA_SYMBOL(CreateLSQFitContextPolynomialFloat)(static_cast<LIBSAKURA_SYMBOL(LSQFitType)>(bltype),
                                                                  static_cast<uint16_t>(order),
                                                                  num_chan, &context);
  } else if (bltype == BaselineType_kCubicSpline) {
    status = LIBSAKURA_SYMBOL(CreateLSQFitContextCubicSplineFloat)(static_cast<uint16_t>(order),
                                                                   num_chan, &context);
  } else if (bltype == BaselineType_kSinusoid) {
    status = LIBSAKURA_SYMBOL(CreateLSQFitContextSinusoidFloat)(static_cast<uint16_t>(order),
                                                                num_chan, &context);
  }
  check_sakura_status("sakura_CreateLSQFitContextFloat", status);
  bl_contexts.push_back(context);
  if (ctx_nchans.size() != bl_contexts.size()) {
    ctx_nchans.push_back(num_chan);
  }
  AlwaysAssert(bl_contexts.size() == ctx_nchans.size(), AipsError);
}

void SingleDishMS::destroy_baseline_contexts(std::vector<LIBSAKURA_SYMBOL(LSQFitContextFloat) *> &bl_contexts) {
  LIBSAKURA_SYMBOL(Status) status;
  for (size_t i = 0; i < bl_contexts.size(); ++i) {
    status = LIBSAKURA_SYMBOL(DestroyLSQFitContextFloat)(bl_contexts[i]);
    check_sakura_status("sakura_DestoyBaselineContextFloat", status);
  }
}

void SingleDishMS::check_sakura_status(string const &name,
                                       LIBSAKURA_SYMBOL(Status) const status) {
  if (status == LIBSAKURA_SYMBOL(Status_kOK)) return;

  ostringstream oss;
  oss << name << "() failed -- ";
  if (status == LIBSAKURA_SYMBOL(Status_kNG)) {
    oss << "NG";
  } else if (status == LIBSAKURA_SYMBOL(Status_kInvalidArgument)) {
    oss << "InvalidArgument";
  } else if (status == LIBSAKURA_SYMBOL(Status_kNoMemory)) {
    oss << "NoMemory";
  } else if (status == LIBSAKURA_SYMBOL(Status_kUnknownError)) {
    oss << "UnknownError";
  }
  throw(AipsError(oss.str()));
}

void SingleDishMS::check_baseline_status(LIBSAKURA_SYMBOL(LSQFitStatus) const bl_status) {
  if (bl_status != LIBSAKURA_SYMBOL(LSQFitStatus_kOK)) {
    throw(AipsError("baseline fitting isn't successful."));
  }
}

void SingleDishMS::get_spectrum_from_cube(Cube<Float> &data_cube,
                                          size_t const row,
                                          size_t const plane,
                                          size_t const num_data,
                                          float out_data[]) {
  AlwaysAssert(static_cast<size_t>(data_cube.ncolumn()) == num_data, AipsError);
  for (size_t i = 0; i < num_data; ++i)
    out_data[i] = static_cast<float>(data_cube(plane, i, row));
}

void SingleDishMS::set_spectrum_to_cube(Cube<Float> &data_cube,
                                        size_t const row,
                                        size_t const plane,
                                        size_t const num_data,
                                        float *in_data) {
  AlwaysAssert(static_cast<size_t>(data_cube.ncolumn()) == num_data, AipsError);
  for (size_t i = 0; i < num_data; ++i)
    data_cube(plane, i, row) = static_cast<Float>(in_data[i]);
}

void SingleDishMS::get_weight_matrix(vi::VisBuffer2 const &vb,
                                     Matrix<Float> &weight_matrix) {
  weight_matrix = vb.weight();
}

void SingleDishMS::set_weight_to_matrix(Matrix<Float> &weight_matrix,
                                        size_t const row,
                                        size_t const plane,
                                        float in_weight) {
  weight_matrix(plane, row) = static_cast<Float>(in_weight);
}

void SingleDishMS::get_flag_cube(vi::VisBuffer2 const &vb,
                                 Cube<Bool> &flag_cube) {
  flag_cube = vb.flagCube();
}

void SingleDishMS::get_flag_from_cube(Cube<Bool> &flag_cube,
                                      size_t const row,
                                      size_t const plane,
                                      size_t const num_flag,
                                      bool out_flag[]) {
  AlwaysAssert(static_cast<size_t>(flag_cube.ncolumn()) == num_flag, AipsError);
  for (size_t i = 0; i < num_flag; ++i)
    out_flag[i] = static_cast<bool>(flag_cube(plane, i, row));
}

void SingleDishMS::set_flag_to_cube(Cube<Bool> &flag_cube,
                                    size_t const row,
                                    size_t const plane,
                                    size_t const num_flag,
                                    bool *in_flag) {
  AlwaysAssert(static_cast<size_t>(flag_cube.ncolumn()) == num_flag, AipsError);
  for (size_t i = 0; i < num_flag; ++i)
    flag_cube(plane, i, row) = static_cast<Bool>(in_flag[i]);
}

void SingleDishMS::flag_spectrum_in_cube(Cube<Bool> &flag_cube,
                                         size_t const row,
                                         size_t const plane) {
  uInt const num_flag = flag_cube.ncolumn();
  for (uInt ichan = 0; ichan < num_flag; ++ichan)
    flag_cube(plane, ichan, row) = true;
}

bool SingleDishMS::allchannels_flagged(size_t const num_flag,
                                       bool const* flag) {
  bool res = true;
  for (size_t i = 0; i < num_flag; ++i) {
    if (!flag[i]) {
      res = false;
      break;
    }
  }
  return res;
}

size_t SingleDishMS::NValidMask(size_t const num_mask, bool const* mask) {
  std::size_t nvalid = 0;
  // the assertion lines had better be replaced with static_assert when c++11 is supported
  AlwaysAssert(static_cast<std::size_t>(true) == 1, AipsError);
  AlwaysAssert(static_cast<std::size_t>(false) == 0, AipsError);
  for (size_t i = 0; i < num_mask; ++i) {
    nvalid += static_cast<std::size_t>(mask[i]);
  }
  return nvalid;
}

void SingleDishMS::split_bloutputname(string str) {
  char key = ',';
  vector<size_t> v;
  for (size_t i = 0; i < str.size(); ++i) {
    char target = str[i];
    if (key == target) {
      v.push_back(i);
    }
  }

  //cout << "comma " << v.size() << endl;
  //cout << "v[1] " << v[1] << endl;
  //cout <<  "v.size()-1 " <<  v.size()-1 << endl;
  //cout << "v[1]+1 " << v[1]+1 << endl;
  //cout << "str.size()-v[1]-1 " << str.size()-v[1]-1 << endl;
  //cout << "str.substr(v[1]+1, str.size()-v[1]-1) " << str.substr(v[1]+1, str.size()-v[1]-1) << endl;

  string ss;
  bloutputname_csv.clear();
  bloutputname_text.clear();
  bloutputname_table.clear();

  if (0 != v[0]) {
    bloutputname_csv = str.substr(0, v[0]);
    ss = str.substr(0, v[0]);
  }
  if (v[0] + 1 != v[1]) {
    bloutputname_text = str.substr(v[0] + 1, v[1] - v[0] - 1);
  }
  if (v[1] != str.size() - 1) {
    bloutputname_table = str.substr(v[1] + 1, str.size() - v[1] - 1);
  }
}

size_t SingleDishMS::get_num_coeff_bloutput(size_t const bltype,
                                            size_t order,
                                            size_t &num_coeff_max) {
  size_t num_coeff = 0;
  switch (bltype) {
  case BaselineType_kPolynomial:
  case BaselineType_kChebyshev:
    break;
  case BaselineType_kCubicSpline:
    num_coeff = order + 1;
    break;
  case BaselineType_kSinusoid:
    break;
  default:
    throw(AipsError("Unsupported baseline type."));
  }
  if (num_coeff_max < num_coeff) {
    num_coeff_max = num_coeff;
  }

  return num_coeff;
}

vector<int> SingleDishMS::string_to_list(string const &wn_str, char const delim) {
  vector<int> wn_list;
  wn_list.clear();
  vector<size_t> delim_position;
  delim_position.clear();
  for (size_t i = 0; i < wn_str.size(); ++i) {
    if (wn_str[i] == delim) {
      delim_position.push_back(i);
    }
  }
  delim_position.push_back(wn_str.size());
  size_t start_position = 0;
  for (size_t i = 0; i < delim_position.size(); ++i) {
    size_t end_position = delim_position[i];
    size_t length = end_position - start_position;
    if (length > 0) {
      wn_list.push_back(std::atoi(wn_str.substr(start_position, length).c_str()));
    }
    start_position = end_position + 1;
  }
  return wn_list;
}

void SingleDishMS::get_effective_nwave(std::vector<int> const &addwn,
                                       std::vector<int> const &rejwn,
                                       int const wn_ulimit,
                                       std::vector<int> &effwn) {
  effwn.clear();
  if (addwn.size() < 1) {
    throw AipsError("addwn has no elements.");
  }

  auto up_to_nyquist_limit = [&](std::vector<int> const &v){ return ((v.size() == 2) && (v[1] == SinusoidWaveNumber_kUpperLimit)); };
  auto check_rejwn_add = [&](int const v){
    bool add = (0 <= v) && (v <= wn_ulimit); // check v in range
    for (size_t i = 0; i < rejwn.size(); ++i) {
      if (rejwn[i] == v) {
        add = false;
        break;
      }
    }
    if (add) {
      effwn.push_back(v);
    }
  };

  if (up_to_nyquist_limit(addwn)) {
    if (up_to_nyquist_limit(rejwn)) {
      if (addwn[0] < rejwn[0]) {
        for (int wn = addwn[0]; wn < rejwn[0]; ++wn) {
          if ((0 <= wn) && (wn <= wn_ulimit)) {
            effwn.push_back(wn);
          }
        }
      } else {
        throw(AipsError("No effective wave number given for sinusoidal fitting."));
      }
    } else {
      for (int wn = addwn[0]; wn <= wn_ulimit; ++wn) {
        check_rejwn_add(wn);
      }
    }
  } else {
    if (up_to_nyquist_limit(rejwn)) {
      int maxwn = rejwn[0] - 1;
      if (maxwn < 0) {
        throw(AipsError("No effective wave number given for sinusoidal fitting."));
      }
      for (size_t i = 0; i < addwn.size(); ++i) {
        if ((0 <= addwn[i]) && (addwn[i] <= maxwn)) {
          effwn.push_back(addwn[i]);
        }
      }
    } else {
      for (size_t i = 0; i < addwn.size(); ++i) {
        check_rejwn_add(addwn[i]);
      }
    }
  }
  if (effwn.size() == 0) {
    throw(AipsError("No effective wave number given for sinusoidal fitting."));
  }
}

void SingleDishMS::finalise_effective_nwave(std::vector<int> const &blparam_eff_base,
                                            std::vector<int> const &blparam_exclude,
                                            int const &blparam_upperlimit,
                                            size_t const &num_chan,
                                            float const *spec,
                                            bool const *mask,
                                            bool const &applyfft,
                                            string const &fftmethod,
                                            string const &fftthresh_str,
                                            std::vector<size_t> &blparam_eff) {
  blparam_eff.resize(blparam_eff_base.size());
  copy(blparam_eff_base.begin(), blparam_eff_base.end(), blparam_eff.begin());

  if (applyfft) {
    string fftthresh_attr;
    float fftthresh_sigma;
    int fftthresh_top;
    parse_fftthresh(fftthresh_str, fftthresh_attr, fftthresh_sigma, fftthresh_top);
    std::vector<int> blparam_fft;
    select_wavenumbers_via_fft(num_chan, spec, mask, fftmethod, fftthresh_attr,
                               fftthresh_sigma, fftthresh_top, blparam_upperlimit,
                               blparam_fft);
    merge_wavenumbers(blparam_eff_base, blparam_fft, blparam_exclude, blparam_eff);
  }
}

void SingleDishMS::parse_fftthresh(string const& fftthresh_str,
                                   string& fftthresh_attr,
                                   float& fftthresh_sigma,
                                   int& fftthresh_top) {
  size_t idx_sigma = fftthresh_str.find("sigma");
  size_t idx_top   = fftthresh_str.find("top");

  if (idx_top == 0) {
    std::istringstream is(fftthresh_str.substr(3));
    is >> fftthresh_top;
    fftthresh_attr = "top";
  } else if (idx_sigma == fftthresh_str.size() - 5) {
    std::istringstream is(fftthresh_str.substr(0, fftthresh_str.size() - 5));
    is >> fftthresh_sigma;
    fftthresh_attr = "sigma";
  } else {
    bool is_number = true;
    for (size_t i = 0; i < fftthresh_str.size()-1; ++i) {
      char ch = (fftthresh_str.substr(i, 1).c_str())[0];
      if (!(isdigit(ch) || (fftthresh_str.substr(i, 1) == "."))) {
        is_number = false;
        break;
      }
    }
    if (is_number) {
      std::istringstream is(fftthresh_str);
      is >> fftthresh_sigma;
      fftthresh_attr = "sigma";
    } else {
      throw(AipsError("fftthresh has a wrong value"));
    }
  }
}

void SingleDishMS::select_wavenumbers_via_fft(size_t const num_chan,
                                              float const *spec,
                                              bool const *mask,
                                              string const &fftmethod,
                                              string const &fftthresh_attr,
                                              float const fftthresh_sigma,
                                              int const fftthresh_top,
                                              int const blparam_upperlimit,
                                              std::vector<int> &blparam_fft) {
  blparam_fft.clear();
  std::vector<float> fourier_spec;
  if (fftmethod == "fft") {
    exec_fft(num_chan, spec, mask, false, true, fourier_spec);
  } else {
    throw AipsError("fftmethod must be 'fft' for now.");
  }

  int fourier_spec_size = static_cast<int>(fourier_spec.size());
  if (fftthresh_attr == "sigma") {
    float mean  = 0.0;
    float mean2 = 0.0;
    for (int i = 0; i < fourier_spec_size; ++i) {
      mean  += fourier_spec[i];
      mean2 += fourier_spec[i] * fourier_spec[i];
    }
    mean  /= static_cast<float>(fourier_spec_size);
    mean2 /= static_cast<float>(fourier_spec_size);
    float thres = mean + fftthresh_sigma * float(sqrt(mean2 - mean * mean));

    for (int i = 0; i < fourier_spec_size; ++i) {
      if ((i <= blparam_upperlimit)&&(thres <= fourier_spec[i])) {
        blparam_fft.push_back(i);
      }
    }
  } else if (fftthresh_attr == "top") {
    int i = 0;
    while (i < fftthresh_top) {
      float max = 0.0;
      int max_idx = 0;
      for (int j = 0; j < fourier_spec_size; ++j) {
        if (max < fourier_spec[j]) {
          max = fourier_spec[j];
          max_idx = j;
        }
      }
      fourier_spec[max_idx] = 0.0;
      if (max_idx <= blparam_upperlimit) {
        blparam_fft.push_back(max_idx);
        ++i;
      }
    }
  } else {
    throw AipsError("fftthresh is wrong.");
  }
}

void SingleDishMS::exec_fft(size_t const num_chan,
                            float const *in_spec,
                            bool const *in_mask,
                            bool const get_real_imag,
                            bool const get_ampl_only,
                            std::vector<float> &fourier_spec) {
  Vector<Float> spec;
  interpolate_constant(static_cast<int>(num_chan), in_spec, in_mask, spec);

  FFTServer<Float, Complex> ffts;
  Vector<Complex> fftres;
  ffts.fft0(fftres, spec);

  float norm = static_cast<float>(2.0/static_cast<double>(num_chan));
  fourier_spec.clear();
  if (get_real_imag) {
    for (size_t i = 0; i < fftres.size(); ++i) {
      fourier_spec.push_back(real(fftres[i]) * norm);
      fourier_spec.push_back(imag(fftres[i]) * norm);
    }
  } else {
    for (size_t i = 0; i < fftres.size(); ++i) {
      fourier_spec.push_back(abs(fftres[i]) * norm);
      if (!get_ampl_only) fourier_spec.push_back(arg(fftres[i]));
    }
  }
}

void SingleDishMS::interpolate_constant(int const num_chan,
                                        float const *in_spec,
                                        bool const *in_mask,
                                        Vector<Float> &spec) {
  spec.resize(num_chan);
  for (int i = 0; i < num_chan; ++i) {
    spec[i] = in_spec[i];
  }
  int idx_left = -1;
  int idx_right = -1;
  bool masked_region = false;

  for (int i = 0; i < num_chan; ++i) {
    if (!in_mask[i]) {
      masked_region = true;
      idx_left = i;
      while (i < num_chan) {
        if (in_mask[i]) break;
        idx_right = i;
        ++i;
      }
    }

    if (masked_region) {
      // execute interpolation as the following criteria:
      // (1) for a masked region inside the spectrum, replace the spectral
      //     values with the mean of those at the two channels just outside
      //     the both edges of the masked region.
      // (2) for a masked region at the spectral edge, replace the values
      //     with the one at the nearest non-masked channel.
      //     (ZOH, but bilateral)
      Float interp = 0.0;
      int idx_left_next = idx_left - 1;
      int idx_right_next = idx_right + 1;
      if (idx_left_next < 0) {
        if (idx_right_next < num_chan) {
          interp = in_spec[idx_right_next];
        } else {
          throw AipsError("Bad data. all channels are masked.");
        }
      } else {
        interp = in_spec[idx_left_next];
        if (idx_right_next < num_chan) {
          interp = (interp + in_spec[idx_right_next]) / 2.0;
        }
      }

      if ((0 <= idx_left) && (idx_left < num_chan) && (0 <= idx_right) && (idx_right < num_chan)) {
        for (int j = idx_left; j <= idx_right; ++j) {
          spec[j] = interp;
        }
      }

      masked_region = false;
    }
  }
}

void SingleDishMS::merge_wavenumbers(std::vector<int> const &blparam_eff_base,
                                     std::vector<int> const &blparam_fft,
                                     std::vector<int> const &blparam_exclude,
                                     std::vector<size_t> &blparam_eff) {
  for (size_t i = 0; i < blparam_fft.size(); ++i) {
    bool found = false;
    for (size_t j = 0; j < blparam_eff_base.size(); ++j) {
      if (blparam_eff_base[j] == blparam_fft[i]) {
        found = true;
        break;
      }
    }
    if (!found) { //new value to add
      //but still need to check if it is to be excluded
      bool found_exclude = false;
      for (size_t j = 0; j < blparam_exclude.size(); ++j) {
        if (blparam_exclude[j] == blparam_fft[i]) {
          found_exclude = true;
          break;
        }
      }
      if (!found_exclude) {
        blparam_eff.push_back(blparam_fft[i]);
      }
    }
  }

  if (1 < blparam_eff.size()) {
    sort(blparam_eff.begin(), blparam_eff.end());
    unique(blparam_eff.begin(), blparam_eff.end());
  }
}

template<typename Func0, typename Func1, typename Func2, typename Func3>
void SingleDishMS::doSubtractBaseline(string const& in_column_name,
                                      string const& out_ms_name,
                                      string const& out_bloutput_name,
                                      bool const& do_subtract,
                                      string const& in_spw,
                                      bool const& update_weight,
                                      string const& sigma_value,
                                      LIBSAKURA_SYMBOL(Status)& status,
                                      std::vector<LIBSAKURA_SYMBOL(LSQFitContextFloat) *> &bl_contexts,
                                      size_t const bltype,
                                      vector<int> const& blparam,
                                      vector<int> const& blparam_exclude,
                                      bool const& applyfft,
                                      string const& fftmethod,
                                      string const& fftthresh,
                                      float const clip_threshold_sigma,
                                      int const num_fitting_max,
                                      bool const linefinding,
                                      float const threshold,
                                      int const avg_limit,
                                      int const minwidth,
                                      vector<int> const& edge,
                                      Func0 func0,
                                      Func1 func1,
                                      Func2 func2,
                                      Func3 func3,
                                      LogIO os) {
  os << LogIO::DEBUG2 << "Calling SingleDishMS::doSubtractBaseline " << LogIO::POST;

  // in_ms = out_ms
  // in_column = [FLOAT_DATA|DATA|CORRECTED_DATA], out_column=new MS
  // no iteration is necessary for the processing.
  // procedure
  // 1. iterate over MS
  // 2. pick single spectrum from in_column (this is not actually necessary for simple scaling but for exibision purpose)
  // 3. fit baseline to each spectrum and subtract it
  // 4. put single spectrum (or a block of spectra) to out_column

  Block<Int> columns(1);
  columns[0] = MS::DATA_DESC_ID;
  prepare_for_process(in_column_name, out_ms_name, columns, false);
  vi::VisibilityIterator2 *vi = sdh_->getVisIter();
  vi->setRowBlocking(kNRowBlocking);
  vi::VisBuffer2 *vb = vi->getVisBuffer();

  split_bloutputname(out_bloutput_name);
  bool write_baseline_csv = (bloutputname_csv != "");
  bool write_baseline_text = (bloutputname_text != "");
  bool write_baseline_table = (bloutputname_table != "");
  ofstream ofs_csv;
  ofstream ofs_txt;
  BaselineTable *bt = 0;

  if (write_baseline_csv) {
    ofs_csv.open(bloutputname_csv.c_str());
  }
  if (write_baseline_text) {
    ofs_txt.open(bloutputname_text.c_str(), std::ios_base::out | std::ios_base::app);
  }
  if (write_baseline_table) {
    bt = new BaselineTable(vi->ms());
  }

  Vector<Int> recspw;
  Matrix<Int> recchan;
  Vector<size_t> nchan;
  Vector<Vector<Bool> > in_mask;
  Vector<bool> nchan_set;
  parse_spw(in_spw, recspw, recchan, nchan, in_mask, nchan_set);

  Vector<size_t> ctx_indices(nchan.nelements(), 0ul);
  std::vector<int> blparam_eff_base;
  auto wn_ulimit_by_rejwn = [&](){
    return ((blparam_exclude.size() == 2) &&
            (blparam_exclude[1] == SinusoidWaveNumber_kUpperLimit)); };

  std::vector<float> weight(WeightIndex_kNum);
  size_t const var_index = (sigma_value == "stddev") ? WeightIndex_kStddev : WeightIndex_kRms;

  for (vi->originChunks(); vi->moreChunks(); vi->nextChunk()) {
    for (vi->origin(); vi->more(); vi->next()) {
      Vector<Int> scans = vb->scan();
      Vector<Double> times = vb->time();
      Vector<Int> beams = vb->feed1();
      Vector<Int> antennas = vb->antenna1();
      Vector<Int> data_spw = vb->spectralWindows();
      size_t const num_chan = static_cast<size_t>(vb->nChannels());
      size_t const num_pol = static_cast<size_t>(vb->nCorrelations());
      size_t const num_row = static_cast<size_t>(vb->nRows());
      Cube<Float> data_chunk(num_pol, num_chan, num_row, Array<Float>::uninitialized);
      Vector<float> spec(num_chan, Array<float>::uninitialized);
      Cube<Bool> flag_chunk(num_pol, num_chan, num_row, Array<Bool>::uninitialized);
      Vector<bool> flag(num_chan, Array<bool>::uninitialized);
      Vector<bool> mask(num_chan, Array<bool>::uninitialized);
      Vector<bool> mask_after_clipping(num_chan, Array<bool>::uninitialized);
      float *spec_data = spec.data();
      bool *flag_data = flag.data();
      bool *mask_data = mask.data();
      bool *mask_after_clipping_data = mask_after_clipping.data();
      Matrix<Float> weight_matrix(num_pol, num_row, Array<Float>::uninitialized);

      auto get_wavenumber_upperlimit = [&](){ return static_cast<int>(num_chan) / 2 - 1; };

      uInt final_mask[num_pol];
      uInt final_mask_after_clipping[num_pol];
      final_mask[0] = 0;
      final_mask[1] = 0;
      final_mask_after_clipping[0] = 0;
      final_mask_after_clipping[1] = 0;

      bool new_nchan = false;
      get_nchan_and_mask(recspw, data_spw, recchan, num_chan, nchan, in_mask, nchan_set, new_nchan);
      if (new_nchan) {
        int blparam_max = blparam[blparam.size() - 1];
        if (bltype == BaselineType_kSinusoid) {
          int nwave_ulimit = get_wavenumber_upperlimit();
          get_effective_nwave(blparam, blparam_exclude, nwave_ulimit, blparam_eff_base);
          blparam_max = blparam_eff_base[blparam_eff_base.size() - 1];
        }
        if ((bltype != BaselineType_kSinusoid) || (!applyfft) || wn_ulimit_by_rejwn()) {
          get_baseline_context(bltype,
                               static_cast<uint16_t>(blparam_max),
                               num_chan, nchan, nchan_set,
                               ctx_indices, bl_contexts);
        }
      } else {
        int last_nchan_set_idx = nchan_set.size() - 1;
        for (int i = nchan_set.size()-1; i >= 0; --i) {
          if (nchan_set[i]) break;
          --last_nchan_set_idx;
        }
        if (0 < last_nchan_set_idx) {
          for (int i = 0; i < last_nchan_set_idx; ++i) {
            if (nchan[i] == nchan[last_nchan_set_idx]) {
              ctx_indices[last_nchan_set_idx] = ctx_indices[i];
              break;
            }
          }
        }
      }

      // get data/flag cubes (npol*nchan*nrow) from VisBuffer
      get_data_cube_float(*vb, data_chunk);
      get_flag_cube(*vb, flag_chunk);

      // get weight matrix (npol*nrow) from VisBuffer
      if (update_weight) {
        get_weight_matrix(*vb, weight_matrix);
      }

      // loop over MS rows
      for (size_t irow = 0; irow < num_row; ++irow) {
        size_t idx = 0;
        for (size_t ispw = 0; ispw < recspw.nelements(); ++ispw) {
          if (data_spw[irow] == recspw[ispw]) {
            idx = ispw;
            break;
          }
        }

        //prepare variables for writing baseline table
        Array<Bool> apply_mtx(IPosition(2, num_pol, 1), true);
        Array<uInt> bltype_mtx(IPosition(2, num_pol, 1), (uInt)bltype);
        //Array<Int> fpar_mtx(IPosition(2, num_pol, 1), (Int)blparam[blparam.size()-1]);
        std::vector<std::vector<size_t> > fpar_mtx_tmp(num_pol);
        std::vector<std::vector<double> > ffpar_mtx_tmp(num_pol);
        std::vector<std::vector<uInt> > masklist_mtx_tmp(num_pol);
        std::vector<std::vector<double> > coeff_mtx_tmp(num_pol);

        Array<Float> rms_mtx(IPosition(2, num_pol, 1), (Float)0);
        Array<Float> cthres_mtx(IPosition(2, num_pol, 1), Array<Float>::uninitialized);
        Array<uInt> citer_mtx(IPosition(2, num_pol, 1), Array<uInt>::uninitialized);
        Array<Bool> uself_mtx(IPosition(2, num_pol, 1), Array<Bool>::uninitialized);
        Array<Float> lfthres_mtx(IPosition(2, num_pol, 1), Array<Float>::uninitialized);
        Array<uInt> lfavg_mtx(IPosition(2, num_pol, 1), Array<uInt>::uninitialized);
        Array<uInt> lfedge_mtx(IPosition(2, num_pol, 2), Array<uInt>::uninitialized);

        size_t num_apply_true = 0;
        size_t num_fpar_max = 0;
        size_t num_ffpar_max = 0;
        size_t num_masklist_max = 0;
        size_t num_coeff_max = 0;

        // loop over polarization
        for (size_t ipol = 0; ipol < num_pol; ++ipol) {
          // get a channel mask from data cube
          // (note that the variable 'mask' is flag in the next line
          // actually, then it will be converted to real mask when
          // taking AND with user-given mask info. this is just for
          // saving memory usage...)
          get_flag_from_cube(flag_chunk, irow, ipol, num_chan, flag_data);
          // skip spectrum if all channels flagged
          if (allchannels_flagged(num_chan, flag_data)) {
            apply_mtx[0][ipol] = false;
            continue;
          }

          // convert flag to mask by taking logical NOT of flag
          // and then operate logical AND with in_mask
          for (size_t ichan = 0; ichan < num_chan; ++ichan) {
            mask_data[ichan] = in_mask[idx][ichan] && (!(flag_data[ichan]));
          }
          // get a spectrum from data cube
          get_spectrum_from_cube(data_chunk, irow, ipol, num_chan, spec_data);
          // line finding. get baseline mask (invert=true)
          if (linefinding) {
            findLineAndGetMask(num_chan, spec_data, mask_data, threshold,
                avg_limit, minwidth, edge, true, mask_data);
          }

          std::vector<size_t> blparam_eff;

          size_t num_coeff;
          if (bltype == BaselineType_kSinusoid) {
            int nwave_ulimit = get_wavenumber_upperlimit();
            finalise_effective_nwave(blparam_eff_base, blparam_exclude,
                                     nwave_ulimit,
                                     num_chan, spec_data, mask_data,
                                     applyfft, fftmethod, fftthresh,
                                     blparam_eff);
            size_t blparam_eff_size = blparam_eff.size();
            if (blparam_eff[0] == 0) {
                num_coeff = blparam_eff_size * 2 - 1;
            } else {
                num_coeff = blparam_eff_size * 2;
            }
          } else if (bltype == BaselineType_kCubicSpline) {
            blparam_eff.resize(1);
            blparam_eff[0] = blparam[blparam.size() - 1];
            num_coeff = blparam_eff[0] * 4;
          } else { // poly, chebyshev
            blparam_eff.resize(1);
            blparam_eff[0] = blparam[blparam.size() - 1];
            status =
              LIBSAKURA_SYMBOL(GetNumberOfCoefficientsFloat)(bl_contexts[ctx_indices[idx]],
                                                             blparam_eff[0],
                                                             &num_coeff);
            check_sakura_status("sakura_GetNumberOfCoefficients", status);
          }

          // Final check of the valid number of channels
          size_t num_min =
            (bltype == BaselineType_kCubicSpline) ? blparam[blparam.size()-1] + 3 : num_coeff;
          if (NValidMask(num_chan, mask_data) < num_min) {
            flag_spectrum_in_cube(flag_chunk, irow, ipol);
            apply_mtx[0][ipol] = false;
            os << LogIO::WARN
               << "Too few valid channels to fit. Skipping Antenna "
               << antennas[irow] << ", Beam " << beams[irow] << ", SPW "
               << data_spw[irow] << ", Pol " << ipol << ", Time "
               << MVTime(times[irow] / 24. / 3600.).string(MVTime::YMD, 8)
               << LogIO::POST;
            continue;
          }
          // actual execution of single spectrum
          float rms;
          if (write_baseline_text || write_baseline_csv || write_baseline_table) {
            num_apply_true++;

            if (num_coeff_max < num_coeff) {
              num_coeff_max = num_coeff;
            }
            Vector<double> coeff(num_coeff);
            double *coeff_data = coeff.data();

            //---GetBestFitBaselineCoefficientsFloat()...
            //func0(ctx_indices[idx], num_chan, blparam_eff, spec_data, mask_data, num_coeff, coeff_data, mask_after_clipping_data, &rms);
            LIBSAKURA_SYMBOL(LSQFitContextFloat) *context = nullptr;
            if ((bltype != BaselineType_kSinusoid) || (!applyfft) || wn_ulimit_by_rejwn()) {
              context = bl_contexts[ctx_indices[idx]];
            }
            func0(context, num_chan, blparam_eff, spec_data, mask_data, num_coeff, coeff_data, mask_after_clipping_data, &rms);

            for (size_t i = 0; i < num_chan; ++i) {
              if (mask_data[i] == false) {
                final_mask[ipol] += 1;
              }
              if (mask_after_clipping_data[i] == false) {
                final_mask_after_clipping[ipol] += 1;
              }
            }

            //set_array_for_bltable(fpar_mtx_tmp)
            size_t num_fpar = blparam_eff.size();
            fpar_mtx_tmp[ipol].resize(num_fpar);
            if (num_fpar_max < num_fpar) {
              num_fpar_max = num_fpar;
            }
            fpar_mtx_tmp[ipol].resize(num_fpar);
            for (size_t ifpar = 0; ifpar < num_fpar; ++ifpar) {
              fpar_mtx_tmp[ipol][ifpar] = blparam_eff[ifpar];
            }

            //---set_array_for_bltable(ffpar_mtx_tmp)
            func1(ipol, ffpar_mtx_tmp, num_ffpar_max);

            //set_array_for_bltable<double, Float>(ipol, num_coeff, coeff_data, coeff_mtx);
            coeff_mtx_tmp[ipol].resize(num_coeff);
            for (size_t icoeff = 0; icoeff < num_coeff; ++icoeff) {
              coeff_mtx_tmp[ipol][icoeff] = coeff_data[icoeff];
            }

            Vector<uInt> masklist;
            get_masklist_from_mask(num_chan, mask_after_clipping_data, masklist);
            if (masklist.size() > num_masklist_max) {
              num_masklist_max = masklist.size();
            }
            masklist_mtx_tmp[ipol].clear();
            for (size_t imask = 0; imask < masklist.size(); ++imask) {
              masklist_mtx_tmp[ipol].push_back(masklist[imask]);
            }

            //---SubtractBaselineUsingCoefficientsFloat()...
            //func2(ctx_indices[idx], num_chan, fpar_mtx_tmp[ipol], spec_data, num_coeff, coeff_data);
            func2(context, num_chan, fpar_mtx_tmp[ipol], spec_data, num_coeff, coeff_data);

            rms_mtx[0][ipol] = rms;

            cthres_mtx[0][ipol] = clip_threshold_sigma;
            citer_mtx[0][ipol] = (uInt)num_fitting_max - 1;
            uself_mtx[0][ipol] = false;
            lfthres_mtx[0][ipol] = 0.0;
            lfavg_mtx[0][ipol] = 0;
            for (size_t iedge = 0; iedge < 2; ++iedge) {
              lfedge_mtx[iedge][ipol] = 0;
            }
          } else {
            //---SubtractBaselineFloat()...
            //func3(ctx_indices[idx], num_chan, blparam_eff, num_coeff, spec_data, mask_data, &rms);
            LIBSAKURA_SYMBOL(LSQFitContextFloat) *context = nullptr;
            if ((bltype != BaselineType_kSinusoid) || (!applyfft) || wn_ulimit_by_rejwn()) {
              context = bl_contexts[ctx_indices[idx]];
            }
            func3(context, num_chan, blparam_eff, num_coeff, spec_data, mask_data, mask_after_clipping_data, &rms);
          }
          // set back a spectrum to data cube
          if (do_subtract) {
            set_spectrum_to_cube(data_chunk, irow, ipol, num_chan, spec_data);
          }

          if (update_weight) {
            compute_weight(num_chan, spec_data, mask_after_clipping_data, weight);
            set_weight_to_matrix(weight_matrix, irow, ipol, weight.at(var_index));
          }

        } // end of polarization loop

        // output results of fitting
        if (num_apply_true == 0) continue;

        Array<Int> fpar_mtx(IPosition(2, num_pol, num_fpar_max),
                            Array<Int>::uninitialized);
        set_matrix_for_bltable<size_t, Int>(num_pol, num_fpar_max,
                                            fpar_mtx_tmp, fpar_mtx);
        Array<Float> ffpar_mtx(IPosition(2, num_pol, num_ffpar_max),
                               Array<Float>::uninitialized);
        set_matrix_for_bltable<double, Float>(num_pol, num_ffpar_max,
                                              ffpar_mtx_tmp, ffpar_mtx);
        Array<uInt> masklist_mtx(IPosition(2, num_pol, num_masklist_max),
                                 Array<uInt>::uninitialized);
        set_matrix_for_bltable<uInt, uInt>(num_pol, num_masklist_max,
                                           masklist_mtx_tmp, masklist_mtx);
        Array<Float> coeff_mtx(IPosition(2, num_pol, num_coeff_max),
                               Array<Float>::uninitialized);
        set_matrix_for_bltable<double, Float>(num_pol, num_coeff_max,
                                              coeff_mtx_tmp, coeff_mtx);
        Matrix<uInt> masklist_mtx2 = masklist_mtx;
        Matrix<Bool> apply_mtx2 = apply_mtx;

        if (write_baseline_table) {
          bt->appenddata((uInt)scans[irow], (uInt)beams[irow], (uInt)antennas[irow],
                         (uInt)data_spw[irow], 0, times[irow],
                         apply_mtx, bltype_mtx, fpar_mtx, ffpar_mtx, masklist_mtx,
                         coeff_mtx, rms_mtx, (uInt)num_chan, cthres_mtx, citer_mtx,
                         uself_mtx, lfthres_mtx, lfavg_mtx, lfedge_mtx);
        }

        if (write_baseline_text) {
          for (size_t ipol = 0; ipol < num_pol; ++ipol) {
            if (apply_mtx2(ipol, 0) == false) continue;

            ofs_txt << "Scan" << '[' << (uInt)scans[irow] << ']' << ' '
                    << "Beam" << '[' << (uInt)beams[irow] << ']' << ' '
                    << "Spw"  << '[' << (uInt)data_spw[irow] << ']' << ' '
                    << "Pol"  << '[' << ipol << ']' << ' '
                    << "Time" << '[' << MVTime(times[irow]/ 24. / 3600.).string(MVTime::YMD, 8) << ']'
                    << endl;
            ofs_txt << endl;
            ofs_txt << "Fitter range = " << '[';

            for (size_t imasklist = 0; imasklist < num_masklist_max/2; ++imasklist) {
              if (imasklist == 0) {
                ofs_txt << '['  << masklist_mtx2(ipol, 2 * imasklist) << ';'
                        << masklist_mtx2(ipol, 2 * imasklist + 1) << ']';
              }
              if (imasklist >= 1
                  && (0 != masklist_mtx2(ipol, 2 * imasklist)
                      && 0 != masklist_mtx2(ipol, 2 * imasklist + 1))) {
                ofs_txt << ",[" << masklist_mtx2(ipol, 2 * imasklist) << ','
                        << masklist_mtx2(ipol, 2 * imasklist + 1) << ']';
              }
            }

            ofs_txt << ']' << endl;
            ofs_txt << endl;

            Matrix<uInt> bltype_mtx2 = bltype_mtx[0][ipol];
            Matrix<Int> fpar_mtx2 = fpar_mtx[0][ipol];
            Matrix<Float> rms_mtx2 = rms_mtx[0][ipol];
            string bltype_name;

            string blparam_name =" order = ";
            if (bltype_mtx2(0, 0) == (uInt)0) {
              bltype_name = "poly";
            } else if (bltype_mtx2(0, 0) == (uInt)1) {
              bltype_name = "chebyshev";
            } else if (bltype_mtx2(0, 0) == (uInt)2) {
                blparam_name = " npiece = ";
                bltype_name = "cspline";
            } else if (bltype_mtx2(0, 0) == (uInt)3) {
                blparam_name = " nwave = ";
                bltype_name = "sinusoid";
            }

            ofs_txt << "Baseline parameters  Function = "
                    << bltype_name.c_str() << " " << blparam_name;
            Matrix<Int> fpar_mtx3 = fpar_mtx;
            if (bltype_mtx2(0,0) == (uInt)3) {
              for (size_t num = 0; num < num_fpar_max; ++num) {
                ofs_txt << fpar_mtx3(ipol, num) << " ";
              }
              ofs_txt << endl;
            } else {
              ofs_txt << fpar_mtx2(0, 0) << endl;
            }

            ofs_txt << endl;
            ofs_txt << "Results of baseline fit" << endl;
            ofs_txt << endl;
            Matrix<Float> coeff_mtx2 = coeff_mtx;

            if (bltype_mtx2(0,0) == (uInt)0 || bltype_mtx2(0,0) == (uInt)1 || bltype_mtx2(0,0) == (uInt)2){
              for (size_t icoeff = 0; icoeff < num_coeff_max; ++icoeff) {
                ofs_txt << "p" << icoeff << " = " << setprecision(8) << coeff_mtx2(ipol, icoeff) << "  ";
              }
            } else if (bltype_mtx2(0,0) == (uInt)3) {
              size_t wn=0;
              string c_s ="s";
              //if (blparam[0] == 0) {
              if (fpar_mtx3(ipol, wn) == 0) {
                ofs_txt << "c" << fpar_mtx3(ipol, wn) << " = " <<setw(13)<<left<< setprecision(8) << coeff_mtx2(ipol, 0) << "  ";
                wn = 1;
                //for (size_t icoeff = 1; icoeff < num_coeff_max; ++icoeff) {
                for (size_t icoeff = 1; icoeff < coeff_mtx_tmp[ipol].size(); ++icoeff) {
                  ofs_txt << c_s << fpar_mtx3(ipol, wn) << " = " <<setw(13)<<left<< setprecision(8) << coeff_mtx2(ipol, icoeff) << "  ";
                  c_s == "s" ? (c_s = "c") : (c_s = "s");
                  if (icoeff % 2 == 0) {
                    ++wn;
                  }
                }
              } else {
                wn = 0;
                //for (size_t icoeff = 0; icoeff < num_coeff_max; ++icoeff) {
                for (size_t icoeff = 0; icoeff < coeff_mtx_tmp[ipol].size(); ++icoeff) {
                  ofs_txt << c_s << fpar_mtx3(ipol, wn) << " = " <<setw(13)<<left<< setprecision(8) << coeff_mtx2(ipol, icoeff) << "  ";
                  c_s == "s" ? (c_s = "c") : (c_s = "s");
                  if (icoeff % 2 != 0) {
                    ++wn;
                  }
                }
              }
            }

            ofs_txt << endl;
            ofs_txt << endl;
            ofs_txt << "rms = ";
            ofs_txt << setprecision(8) << rms_mtx2(0, 0) << endl;
            ofs_txt << endl;
            ofs_txt << "Number of clipped channels = "
                    << final_mask_after_clipping[ipol] - final_mask[ipol] << endl;
            ofs_txt << endl;
            ofs_txt << "------------------------------------------------------"
                    << endl;
            ofs_txt << endl;
          }
        }

        if (write_baseline_csv) {
          for (size_t ipol = 0; ipol < num_pol; ++ipol) {
            if (apply_mtx2(ipol, 0) == false) continue;

            ofs_csv << (uInt)scans[irow] << ',' << (uInt)beams[irow] << ','
                    << (uInt)data_spw[irow] << ',' << ipol << ','
                    << setprecision(12) << times[irow] << ',';
            ofs_csv << '[';
            for (size_t imasklist = 0; imasklist < num_masklist_max / 2; ++imasklist) {
              if (imasklist == 0) {
                ofs_csv << '[' << masklist_mtx2(ipol, 2 * imasklist) << ';'
                        << masklist_mtx2(ipol, 2 * imasklist + 1) << ']';
              }
              if (imasklist >= 1
                  && (0 != masklist_mtx2(ipol, 2 * imasklist)
                      && 0 != masklist_mtx2(ipol, 2 * imasklist + 1))) {
                ofs_csv << ";[" << masklist_mtx2(ipol, 2 * imasklist) << ';'
                        << masklist_mtx2(ipol, 2 * imasklist + 1) << ']';
              }
            }
            ofs_csv << ']' << ',';
            Matrix<uInt> bltype_mtx2 = bltype_mtx[0][ipol];
            string bltype_name;
            if (bltype_mtx2(0, 0) == (uInt)0) {
              bltype_name = "poly";
            } else if (bltype_mtx2(0, 0) == (uInt)1) {
              bltype_name = "chebyshev";
            } else if (bltype_mtx2(0, 0) == (uInt)2) {
              bltype_name = "cspline";
            } else if (bltype_mtx2(0, 0) == (uInt)3) {
              bltype_name = "sinusoid";
            }

            Matrix<Int> fpar_mtx2 = fpar_mtx;
            if (bltype_mtx2(0, 0) == (uInt)3) {
              ofs_csv << bltype_name.c_str() << ',' << fpar_mtx2(ipol, 0);
              for (size_t i = 1; i < num_fpar_max; ++i) {
                ofs_csv << ';' << fpar_mtx2(ipol, i);
              }
              ofs_csv << ',';
            } else {
              ofs_csv << bltype_name.c_str() << ',' << fpar_mtx2(ipol, 0)
                      << ',';
            }

            Matrix<Float> coeff_mtx2 = coeff_mtx;
            if (bltype_mtx2(0, 0) == (uInt)3) {
              for (size_t icoeff = 0; icoeff < coeff_mtx_tmp[ipol].size(); ++icoeff) {
                ofs_csv << setprecision(8) << coeff_mtx2(ipol, icoeff) << ',';
              }
            } else {
              for (size_t icoeff = 0; icoeff < num_coeff_max; ++icoeff) {
                ofs_csv << setprecision(8) << coeff_mtx2(ipol, icoeff) << ',';
              }
            }

            Matrix<Float> rms_mtx2 = rms_mtx;
            ofs_csv << setprecision(8) << rms_mtx2(ipol, 0) << ',';
            ofs_csv << final_mask_after_clipping[ipol] - final_mask[ipol];
            ofs_csv << endl;
          }
        }
      } // end of chunk row loop
      // write back data cube to VisBuffer
      if (do_subtract) {
        if (update_weight) {
          sdh_->fillCubeToOutputMs(vb, data_chunk, &flag_chunk, &weight_matrix);
        } else {
          sdh_->fillCubeToOutputMs(vb, data_chunk, &flag_chunk);
        }
      }
    } // end of vi loop
  } // end of chunk loop

  if (write_baseline_table) {
    bt->save(bloutputname_table);
    delete bt;
  }
  if (write_baseline_csv) {
    ofs_csv.close();
  }
  if (write_baseline_text) {
    ofs_txt.close();
  }

  finalize_process();
  destroy_baseline_contexts(bl_contexts);

  //double tend = gettimeofday_sec();
  //std::cout << "Elapsed time = " << (tend - tstart) << " sec." << std::endl;
}

////////////////////////////////////////////////////////////////////////
///// Atcual processing functions
////////////////////////////////////////////////////////////////////////

//Subtract baseline using normal or Chebyshev polynomials
void SingleDishMS::subtractBaseline(string const& in_column_name,
                                    string const& out_ms_name,
                                    string const& out_bloutput_name,
                                    bool const& do_subtract,
                                    string const& in_spw,
                                    bool const& update_weight,
                                    string const& sigma_value,
                                    string const& blfunc,
                                    int const order,
                                    float const clip_threshold_sigma,
                                    int const num_fitting_max,
                                    bool const linefinding,
                                    float const threshold,
                                    int const avg_limit,
                                    int const minwidth,
                                    vector<int> const& edge) {
  vector<int> order_vect;
  order_vect.push_back(order);
  vector<int> blparam_exclude_dummy;
  blparam_exclude_dummy.clear();

  LogIO os(_ORIGIN);
  os << "Fitting and subtracting polynomial baseline order = " << order
     << LogIO::POST;
  if (order < 0) {
    throw(AipsError("order must be positive or zero."));
  }

  LIBSAKURA_SYMBOL(Status) status;
  LIBSAKURA_SYMBOL(LSQFitStatus) bl_status;
  std::vector<LIBSAKURA_SYMBOL(LSQFitContextFloat) *> bl_contexts;
  bl_contexts.clear();
  size_t bltype = BaselineType_kPolynomial;
  string blfunc_lower = blfunc;
  std::transform(
    blfunc_lower.begin(),
    blfunc_lower.end(),
    blfunc_lower.begin(),
    [](unsigned char c) {return std::tolower(c);}
  );
  if (blfunc_lower == "chebyshev") {
    bltype = BaselineType_kChebyshev;
  }

  doSubtractBaseline(in_column_name,
                     out_ms_name,
                     out_bloutput_name,
                     do_subtract,
                     in_spw,
                     update_weight,
                     sigma_value,
                     status,
                     bl_contexts,
                     bltype,
                     order_vect,
                     blparam_exclude_dummy,
                     false,
                     "",
                     "",
                     clip_threshold_sigma,
                     num_fitting_max,
                     linefinding,
                     threshold,
                     avg_limit,
                     minwidth,
                     edge,
                     [&](LIBSAKURA_SYMBOL(LSQFitContextFloat) const *context,
                         size_t const num_chan, std::vector<size_t> const &/*nwave*/,
                         float *spec, bool const *mask, size_t const /*num_coeff*/, double *coeff,
                         bool *mask_after_clipping, float *rms){
                       status = LIBSAKURA_SYMBOL(LSQFitPolynomialFloat)(
                         context, static_cast<uint16_t>(order_vect[0]),
                         num_chan, spec, mask, clip_threshold_sigma, num_fitting_max,
                         order_vect[0] + 1, coeff, nullptr, nullptr, mask_after_clipping, rms, &bl_status);
                       check_sakura_status("sakura_LSQFitPolynomialFloat", status);
                       if (bl_status != LIBSAKURA_SYMBOL(LSQFitStatus_kOK)) {
                         throw(AipsError("baseline fitting isn't successful."));
                       }
                     },
                     [&](size_t ipol, std::vector<std::vector<double> > &ffpar_mtx_tmp, size_t &num_ffpar_max) {
                       size_t num_ffpar = get_num_coeff_bloutput(bltype, 0, num_ffpar_max);
                       ffpar_mtx_tmp[ipol].resize(num_ffpar);
                     },
                     [&](LIBSAKURA_SYMBOL(LSQFitContextFloat) const *context,
                         size_t const num_chan, std::vector<size_t> const &/*nwave*/,
                         float *spec, size_t const /*num_coeff*/, double *coeff){
                       status = LIBSAKURA_SYMBOL(SubtractPolynomialFloat)(
                         context, num_chan, spec, order_vect[0] + 1, coeff, spec);
                       check_sakura_status("sakura_SubtractPolynomialFloat", status);},
                     [&](LIBSAKURA_SYMBOL(LSQFitContextFloat) const *context,
                         size_t const num_chan, std::vector<size_t> const &/*nwave*/,
                         size_t const /*num_coeff*/, float *spec, bool const *mask, bool *mask_after_clipping, float *rms){
                       status = LIBSAKURA_SYMBOL(LSQFitPolynomialFloat)(
                         context, static_cast<uint16_t>(order_vect[0]),
                         num_chan, spec, mask, clip_threshold_sigma, num_fitting_max,
                         order_vect[0] + 1, nullptr, nullptr, spec, mask_after_clipping, rms, &bl_status);
                       check_sakura_status("sakura_LSQFitPolynomialFloat", status);
                       if (bl_status != LIBSAKURA_SYMBOL(LSQFitStatus_kOK)) {
                         throw(AipsError("baseline fitting isn't successful."));
                       }
                     },
                     os
                     );
}

//Subtract baseline using natural cubic spline
void SingleDishMS::subtractBaselineCspline(string const& in_column_name,
                                           string const& out_ms_name,
                                           string const& out_bloutput_name,
                                           bool const& do_subtract,
                                           string const& in_spw,
                                           bool const& update_weight,
                                           string const& sigma_value,
                                           int const npiece,
                                           float const clip_threshold_sigma,
                                           int const num_fitting_max,
                                           bool const linefinding,
                                           float const threshold,
                                           int const avg_limit,
                                           int const minwidth,
                                           vector<int> const& edge) {
  vector<int> npiece_vect;
  npiece_vect.push_back(npiece);
  vector<int> blparam_exclude_dummy;
  blparam_exclude_dummy.clear();

  LogIO os(_ORIGIN);
  os << "Fitting and subtracting cubic spline baseline npiece = " << npiece
      << LogIO::POST;
  if (npiece <= 0) {
    throw(AipsError("npiece must be positive."));
  }

  LIBSAKURA_SYMBOL(Status) status;
  LIBSAKURA_SYMBOL(LSQFitStatus) bl_status;
  std::vector<LIBSAKURA_SYMBOL(LSQFitContextFloat) *> bl_contexts;
  bl_contexts.clear();
  size_t const bltype = BaselineType_kCubicSpline;
  Vector<size_t> boundary(npiece+1, Array<size_t>::uninitialized);
  size_t *boundary_data = boundary.data();

  doSubtractBaseline(in_column_name,
                     out_ms_name,
                     out_bloutput_name,
                     do_subtract,
                     in_spw,
                     update_weight,
                     sigma_value,
                     status,
                     bl_contexts,
                     bltype,
                     npiece_vect,
                     blparam_exclude_dummy,
                     false,
                     "",
                     "",
                     clip_threshold_sigma,
                     num_fitting_max,
                     linefinding,
                     threshold,
                     avg_limit,
                     minwidth,
                     edge,
                     [&](LIBSAKURA_SYMBOL(LSQFitContextFloat) const *context,
                         size_t const num_chan, std::vector<size_t> const &/*nwave*/,
                         float *spec, bool const *mask, size_t const /*num_coeff*/, double *coeff,
                         bool *mask_after_clipping, float *rms) {
                       status = LIBSAKURA_SYMBOL(LSQFitCubicSplineFloat)(
                         context, static_cast<uint16_t>(npiece_vect[0]),
                         num_chan, spec, mask, clip_threshold_sigma, num_fitting_max,
                         reinterpret_cast<double (*)[4]>(coeff), nullptr, nullptr,
                         mask_after_clipping, rms, boundary_data, &bl_status);
                       check_sakura_status("sakura_LSQFitCubicSplineFloat", status);
                       if (bl_status != LIBSAKURA_SYMBOL(LSQFitStatus_kOK)) {
                         throw(AipsError("baseline fitting isn't successful."));
                       }
                     },
                     [&](size_t ipol, std::vector<std::vector<double> > &ffpar_mtx_tmp, size_t &num_ffpar_max) {
                       size_t num_ffpar = get_num_coeff_bloutput(
                         bltype, npiece_vect[0], num_ffpar_max);
                       ffpar_mtx_tmp[ipol].resize(num_ffpar);
                       for (size_t ipiece = 0; ipiece < num_ffpar; ++ipiece) {
                         ffpar_mtx_tmp[ipol][ipiece] = boundary_data[ipiece];
                       }
                     },
                     [&](LIBSAKURA_SYMBOL(LSQFitContextFloat) const *context,
                         size_t const num_chan, std::vector<size_t> const &/*nwave*/,
                         float *spec, size_t const /*num_coeff*/, double *coeff) {
                       status = LIBSAKURA_SYMBOL(SubtractCubicSplineFloat)(
                         context, num_chan, spec, npiece_vect[0],
                         reinterpret_cast<double (*)[4]>(coeff), boundary_data, spec);
                       check_sakura_status("sakura_SubtractCubicSplineFloat", status);},
                     [&](LIBSAKURA_SYMBOL(LSQFitContextFloat) const *context,
                         size_t const num_chan, std::vector<size_t> const &/*nwave*/,
                         size_t const /*num_coeff*/, float *spec, bool const *mask, bool *mask_after_clipping, float *rms) {
                       status = LIBSAKURA_SYMBOL(LSQFitCubicSplineFloat)(
                         context, static_cast<uint16_t>(npiece_vect[0]),
                         num_chan, spec, mask, clip_threshold_sigma, num_fitting_max,
                         nullptr, nullptr, spec, mask_after_clipping, rms, boundary_data, &bl_status);
                       check_sakura_status("sakura_LSQFitCubicSplineFloat", status);
                       if (bl_status != LIBSAKURA_SYMBOL(LSQFitStatus_kOK)) {
                         throw(AipsError("baseline fitting isn't successful."));
                       }
                     },
                     os
                     );
}


void SingleDishMS::subtractBaselineSinusoid(string const& in_column_name,
                                            string const& out_ms_name,
                                            string const& out_bloutput_name,
                                            bool const& do_subtract,
                                            string const& in_spw,
                                            bool const& update_weight,
                                            string const& sigma_value,
                                            string const& addwn0,
                                            string const& rejwn0,
                                            bool const applyfft,
                                            string const fftmethod,
                                            string const fftthresh,
                                            float const clip_threshold_sigma,
                                            int const num_fitting_max,
                                            bool const linefinding,
                                            float const threshold,
                                            int const avg_limit,
                                            int const minwidth,
                                            vector<int> const& edge) {
  char const delim = ',';
  vector<int> addwn = string_to_list(addwn0, delim);
  vector<int> rejwn = string_to_list(rejwn0, delim);

  LogIO os(_ORIGIN);
  os << "Fitting and subtracting sinusoid baseline with wave numbers " << addwn0 << LogIO::POST;
  if (addwn.size() == 0) {
    throw(AipsError("addwn must contain at least one element."));
  }

  LIBSAKURA_SYMBOL(Status) status;
  LIBSAKURA_SYMBOL(LSQFitStatus) bl_status;
  std::vector<LIBSAKURA_SYMBOL(LSQFitContextFloat) *> bl_contexts;
  bl_contexts.clear();
  LIBSAKURA_SYMBOL(LSQFitContextFloat) *context = nullptr;
  size_t bltype = BaselineType_kSinusoid;

  auto wn_ulimit_by_rejwn = [&](){
    return ((rejwn.size() == 2) &&
            (rejwn[1] == SinusoidWaveNumber_kUpperLimit)); };
  auto par_spectrum_context = [&](){
    return (applyfft && !wn_ulimit_by_rejwn());
  };
  auto prepare_context = [&](LIBSAKURA_SYMBOL(LSQFitContextFloat) const *context0,
                             size_t const num_chan, std::vector<size_t> const &nwave){
    if (par_spectrum_context()) {
      status = LIBSAKURA_SYMBOL(CreateLSQFitContextSinusoidFloat)(
                 static_cast<uint16_t>(nwave[nwave.size()-1]),
                 num_chan, &context);
      check_sakura_status("sakura_CreateLSQFitContextSinusoidFloat", status);
    } else {
      context = const_cast<LIBSAKURA_SYMBOL(LSQFitContextFloat) *>(context0);
    }
  };
  auto clear_context = [&](){
    if (par_spectrum_context()) {
      status = LIBSAKURA_SYMBOL(DestroyLSQFitContextFloat)(context);
      check_sakura_status("sakura_DestoyBaselineContextFloat", status);
      context = nullptr;
    }
  };

  doSubtractBaseline(in_column_name,
                     out_ms_name,
                     out_bloutput_name,
                     do_subtract,
                     in_spw,
                     update_weight,
                     sigma_value,
                     status,
                     bl_contexts,
                     bltype,
                     addwn,
                     rejwn,
                     applyfft,
                     fftmethod,
                     fftthresh,
                     clip_threshold_sigma,
                     num_fitting_max,
                     linefinding,
                     threshold,
                     avg_limit,
                     minwidth,
                     edge,
                     [&](LIBSAKURA_SYMBOL(LSQFitContextFloat) const *context0,
                         size_t const num_chan, std::vector<size_t> const &nwave,
                         float *spec, bool const *mask, size_t const num_coeff, double *coeff,
                         bool *mask_after_clipping, float *rms) {
                       prepare_context(context0, num_chan, nwave);
                       status = LIBSAKURA_SYMBOL(LSQFitSinusoidFloat)(
                         context, nwave.size(), &nwave[0],
                         num_chan, spec, mask, clip_threshold_sigma, num_fitting_max,
                         num_coeff, coeff, nullptr, nullptr, mask_after_clipping, rms, &bl_status);
                       check_sakura_status("sakura_LSQFitSinusoidFloat", status);
                       check_baseline_status(bl_status);
                     },
                     [&](size_t ipol, std::vector<std::vector<double> > &ffpar_mtx_tmp, size_t &num_ffpar_max) {
                       size_t num_ffpar = get_num_coeff_bloutput(bltype, addwn.size(), num_ffpar_max);
                       ffpar_mtx_tmp[ipol].resize(num_ffpar);
                     },
                     [&](LIBSAKURA_SYMBOL(LSQFitContextFloat) const *context0,
                         size_t const num_chan, std::vector<size_t> const &nwave,
                         float *spec, size_t num_coeff, double *coeff) {
                       if (!par_spectrum_context()) {
                         context = const_cast<LIBSAKURA_SYMBOL(LSQFitContextFloat) *>(context0);
                       }
                       status = LIBSAKURA_SYMBOL(SubtractSinusoidFloat)(
                         context, num_chan, spec, nwave.size(), &nwave[0],
                         num_coeff, coeff, spec);
                       check_sakura_status("sakura_SubtractSinusoidFloat", status);
                       clear_context();
                     },
                     [&](LIBSAKURA_SYMBOL(LSQFitContextFloat) const *context0,
                         size_t const num_chan, std::vector<size_t> const &nwave,
                         size_t const num_coeff, float *spec, bool const *mask, bool *mask_after_clipping, float *rms) {
                       prepare_context(context0, num_chan, nwave);
                       status = LIBSAKURA_SYMBOL(LSQFitSinusoidFloat)(
                         context, nwave.size(), &nwave[0],
                         num_chan, spec, mask, clip_threshold_sigma, num_fitting_max,
                         num_coeff, nullptr, nullptr, spec, mask_after_clipping, rms, &bl_status);
                       check_sakura_status("sakura_LSQFitSinusoidFloat", status);
                       check_baseline_status(bl_status);
                       clear_context();
                     },
                     os
                     );
}

// Apply baseline table to MS
void SingleDishMS::applyBaselineTable(string const& in_column_name,
                                      string const& in_bltable_name,
                                      string const& in_spw,
                                      bool const& update_weight,
                                      string const& sigma_value,
                                      string const& out_ms_name) {
  LogIO os(_ORIGIN);
  os << "Apply baseline table " << in_bltable_name << " to MS. " << LogIO::POST;

  if (in_bltable_name == "") {
    throw(AipsError("baseline table is not given."));
  }

  // parse fitting parameters in the text file
  BLTableParser parser(in_bltable_name);
  std::vector<size_t> baseline_types = parser.get_function_types();
  map<size_t const, uint16_t> max_orders;
  for (size_t i = 0; i < baseline_types.size(); ++i) {
    max_orders[baseline_types[i]] = parser.get_max_order(baseline_types[i]);
  }
  { //DEBUG output
    os << LogIO::DEBUG1 << "spw ID = " << in_spw << LogIO::POST;
    os << LogIO::DEBUG1 << "Baseline Types = " << baseline_types << LogIO::POST;
    os << LogIO::DEBUG1 << "Max Orders:" << LogIO::POST;
    map<size_t const, uint16_t>::iterator iter = max_orders.begin();
    while (iter != max_orders.end()) {
      os << LogIO::DEBUG1 << "- type " << (*iter).first << ": "
          << (*iter).second << LogIO::POST;
      ++iter;
    }
  }

  Block<Int> columns(1);
  columns[0] = MS::DATA_DESC_ID;  // (CAS-9918, 2017/4/27 WK)   //columns[0] = MS::TIME;
  prepare_for_process(in_column_name, out_ms_name, columns, false);
  vi::VisibilityIterator2 *vi = sdh_->getVisIter();
  vi->setRowBlocking(kNRowBlocking);
  vi::VisBuffer2 *vb = vi->getVisBuffer();

  Vector<Int> recspw;
  Matrix<Int> recchan;
  Vector<size_t> nchan;
  Vector<Vector<Bool> > in_mask;
  Vector<bool> nchan_set;
  parse_spw(in_spw, recspw, recchan, nchan, in_mask, nchan_set);

  // Baseline Contexts reservoir
  // key: BaselineType
  // value: a vector of Sakura_BaselineContextFloat for various nchans
  Vector<size_t> ctx_indices(nchan.nelements(), 0ul);
  map<size_t const, std::vector<LIBSAKURA_SYMBOL(LSQFitContextFloat) *> > context_reservoir;
  map<size_t const, uint16_t>::iterator iter = max_orders.begin();
  while (iter != max_orders.end()) {
    context_reservoir[(*iter).first] = std::vector<LIBSAKURA_SYMBOL(LSQFitContextFloat) *>();
    ++iter;
  }

  LIBSAKURA_SYMBOL(Status) status;

  std::vector<float> weight(WeightIndex_kNum);
  size_t const var_index = (sigma_value == "stddev") ? WeightIndex_kStddev : WeightIndex_kRms;

  for (vi->originChunks(); vi->moreChunks(); vi->nextChunk()) {
    for (vi->origin(); vi->more(); vi->next()) {
      Vector<Int> scans = vb->scan();
      Vector<Double> times = vb->time();
      Vector<Double> intervals = vb->timeInterval();
      Vector<Int> beams = vb->feed1();
      Vector<Int> antennas = vb->antenna1();
      Vector<Int> data_spw = vb->spectralWindows();
      size_t const num_chan = static_cast<size_t>(vb->nChannels());
      size_t const num_pol = static_cast<size_t>(vb->nCorrelations());
      size_t const num_row = static_cast<size_t>(vb->nRows());
      Cube<Float> data_chunk(num_pol, num_chan, num_row);
      Vector<float> spec(num_chan);
      float *spec_data = spec.data();
      Cube<Bool> flag_chunk(num_pol, num_chan, num_row);
      Vector<bool> flag(num_chan);
      bool *flag_data = flag.data();
      Vector<bool> mask(num_chan);
      bool *mask_data = mask.data();
      Matrix<Float> weight_matrix(num_pol, num_row, Array<Float>::uninitialized);

      bool new_nchan = false;
      get_nchan_and_mask(recspw, data_spw, recchan, num_chan, nchan, in_mask, nchan_set, new_nchan);
      if (new_nchan) {
        map<size_t const, uint16_t>::iterator iter = max_orders.begin();
        while (iter != max_orders.end()) {
          get_baseline_context((*iter).first, (*iter).second,
                               num_chan, nchan, nchan_set,
                               ctx_indices, context_reservoir[(*iter).first]);
          ++iter;
        }
      }
      // get data/flag cubes (npol*nchan*nrow) from VisBuffer
      get_data_cube_float(*vb, data_chunk);
      get_flag_cube(*vb, flag_chunk);

      // get weight matrix (npol*nrow) from VisBuffer
      if (update_weight) {
        get_weight_matrix(*vb, weight_matrix);
      }

      // loop over MS rows
      for (size_t irow = 0; irow < num_row; ++irow) {
        size_t idx = 0;
        for (size_t ispw = 0; ispw < recspw.nelements(); ++ispw) {
          if (data_spw[irow] == recspw[ispw]) {
            idx = ispw;
            break;
          }
        }

        // find a baseline table row (index) corresponding to this MS row
        size_t idx_fit_param;
        if (!parser.GetFitParameterIdx(times[irow], intervals[irow],
            scans[irow], beams[irow], antennas[irow], data_spw[irow],
            idx_fit_param)) {
          for (size_t ipol = 0; ipol < num_pol; ++ipol) {
            flag_spectrum_in_cube(flag_chunk, irow, ipol); //flag
          }
          continue;
        }

        // loop over polarization
        for (size_t ipol = 0; ipol < num_pol; ++ipol) {
          bool apply;
          Vector<double> coeff;
          Vector<size_t> boundary;
          std::vector<bool> mask_bltable;
          BLParameterSet fit_param;
          parser.GetFitParameterByIdx(idx_fit_param, ipol, apply, coeff, boundary,
                                      mask_bltable, fit_param);
          if (!apply) {
            flag_spectrum_in_cube(flag_chunk, irow, ipol); //flag
            continue;
          }

          // get a channel mask from data cube
          // (note that the variable 'mask' is flag in the next line
          // actually, then it will be converted to real mask when
          // taking AND with user-given mask info. this is just for
          // saving memory usage...)
          get_flag_from_cube(flag_chunk, irow, ipol, num_chan, flag_data);

          // skip spectrum if all channels flagged
          if (allchannels_flagged(num_chan, flag_data)) {
            continue;
          }

          // get a spectrum from data cube
          get_spectrum_from_cube(data_chunk, irow, ipol, num_chan, spec_data);

          // actual execution of single spectrum
          map<size_t const, std::vector<LIBSAKURA_SYMBOL(LSQFitContextFloat) *> >::iterator
          iter = context_reservoir.find(fit_param.baseline_type);
          if (iter == context_reservoir.end())
            throw(AipsError("Invalid baseline type detected!"));
          LIBSAKURA_SYMBOL(LSQFitContextFloat) * context =
              (*iter).second[ctx_indices[idx]];
          //cout << "Got context for type " << (*iter).first << ": idx=" << ctx_indices[idx] << endl;

          double *coeff_data = coeff.data();
          size_t *boundary_data = boundary.data();
          string subtract_funcname;
          switch (static_cast<size_t>(fit_param.baseline_type)) {
          case BaselineType_kPolynomial:
          case BaselineType_kChebyshev:
            status = LIBSAKURA_SYMBOL(SubtractPolynomialFloat)(
	      context, num_chan, spec_data, coeff.size(), coeff_data, spec_data);
            subtract_funcname = "sakura_SubtractPolynomialFloat";
            break;
          case BaselineType_kCubicSpline:
            status = LIBSAKURA_SYMBOL(SubtractCubicSplineFloat)(
	      context, num_chan, spec_data, boundary.size()-1,
              reinterpret_cast<double (*)[4]>(coeff_data),
              boundary_data, spec_data);
            subtract_funcname = "sakura_SubtractCubicSplineFloat";
            break;
          default:
            throw(AipsError("Unsupported baseline type."));
          }
          check_sakura_status(subtract_funcname, status);

          // set back a spectrum to data cube
          set_spectrum_to_cube(data_chunk, irow, ipol, num_chan, spec_data);

          if (update_weight) {
            // convert flag to mask by taking logical NOT of flag
            // and then operate logical AND with in_mask and with mask from bltable
            for (size_t ichan = 0; ichan < num_chan; ++ichan) {
              mask_data[ichan] = in_mask[idx][ichan] && (!(flag_data[ichan])) && mask_bltable[ichan];
            }
            compute_weight(num_chan, spec_data, mask_data, weight);
            set_weight_to_matrix(weight_matrix, irow, ipol, weight.at(var_index));
          }
        } // end of polarization loop

      } // end of chunk row loop
      // write back data and flag cube to VisBuffer
      if (update_weight) {
        sdh_->fillCubeToOutputMs(vb, data_chunk, &flag_chunk, &weight_matrix);
      } else {
        sdh_->fillCubeToOutputMs(vb, data_chunk, &flag_chunk);
      }
    } // end of vi loop
  } // end of chunk loop

  finalize_process();
  // destroy baseline contexts
  map<size_t const, std::vector<LIBSAKURA_SYMBOL(LSQFitContextFloat) *> >::iterator ctxiter = context_reservoir.begin();
  while (ctxiter != context_reservoir.end()) {
    destroy_baseline_contexts (context_reservoir[(*ctxiter).first]);
    ++ctxiter;
  }
}

// Fit line profile
void SingleDishMS::fitLine(string const& in_column_name,
                           string const& in_spw,
                           string const& /* in_pol */,
                           string const& fitfunc,
                           string const& in_nfit,
                           bool const linefinding,
                           float const threshold,
                           int const avg_limit,
                           int const minwidth,
                           vector<int> const& edge,
                           string const& tempfile_name,
                           string const& temp_out_ms_name) {

  // in_column = [FLOAT_DATA|DATA|CORRECTED_DATA]
  // no iteration is necessary for the processing.
  // procedure
  // 1. iterate over MS
  // 2. pick single spectrum from in_column (this is not actually necessary for simple scaling but for exibision purpose)
  // 3. fit Gaussian or Lorentzian profile to each spectrum
  // 4. write fitting results to outfile

  LogIO os(_ORIGIN);
  os << "Fitting line profile with " << fitfunc << LogIO::POST;

  if (file_exists(temp_out_ms_name)) {
    throw(AipsError("temporary ms file unexpectedly exists."));
  }
  if (file_exists(tempfile_name)) {
    throw(AipsError("temporary file unexpectedly exists."));
  }

  Block<Int> columns(1);
  columns[0] = MS::DATA_DESC_ID;
  prepare_for_process(in_column_name, temp_out_ms_name, columns, false);
  vi::VisibilityIterator2 *vi = sdh_->getVisIter();
  vi->setRowBlocking(kNRowBlocking);
  vi::VisBuffer2 *vb = vi->getVisBuffer();
  ofstream ofs(tempfile_name);

  Vector<Int> recspw;
  Matrix<Int> recchan;
  Vector<size_t> nchan;
  Vector<Vector<Bool> > in_mask;
  Vector<bool> nchan_set;
  parse_spw(in_spw, recspw, recchan, nchan, in_mask, nchan_set);

  std::vector<size_t> nfit;
  if (linefinding) {
    os << "Defining line ranges using line finder. nfit will be ignored." << LogIO::POST;
  } else {
    std::vector<string> nfit_s = split_string(in_nfit, ',');
    nfit.resize(nfit_s.size());
    for (size_t i = 0; i < nfit_s.size(); ++i) {
      nfit[i] = std::stoi(nfit_s[i]);
    }
  }

  size_t num_spec = 0;
  size_t num_noline = 0;
  for (vi->originChunks(); vi->moreChunks(); vi->nextChunk()) {
    for (vi->origin(); vi->more(); vi->next()) {
      Vector<Int> scans = vb->scan();
      Vector<Double> times = vb->time();
      Vector<Int> beams = vb->feed1();
      Vector<Int> antennas = vb->antenna1();

      Vector<Int> data_spw = vb->spectralWindows();
      size_t const num_chan = static_cast<size_t>(vb->nChannels());
      size_t const num_pol = static_cast<size_t>(vb->nCorrelations());
      size_t const num_row = static_cast<size_t>(vb->nRows());
      Cube<Float> data_chunk(num_pol, num_chan, num_row);
      Vector<float> spec(num_chan);
      Cube<Bool> flag_chunk(num_pol, num_chan, num_row);
      Vector<bool> mask(num_chan);
      // CAUTION!!!
      // data() method must be used with special care!!!
      float *spec_data = spec.data();
      bool *mask_data = mask.data();

      bool new_nchan = false;
      get_nchan_and_mask(recspw, data_spw, recchan, num_chan, nchan, in_mask,
          nchan_set, new_nchan);

      // get data/flag cubes (npol*nchan*nrow) from VisBuffer
      get_data_cube_float(*vb, data_chunk);
      get_flag_cube(*vb, flag_chunk);

      // loop over MS rows
      for (size_t irow = 0; irow < num_row; ++irow) {
        size_t idx = 0;
        for (size_t ispw = 0; ispw < recspw.nelements(); ++ispw) {
          if (data_spw[irow] == recspw[ispw]) {
            idx = ispw;
            break;
          }
        }

        std::vector<size_t> fitrange_start;
        fitrange_start.clear();
        std::vector<size_t> fitrange_end;
        fitrange_end.clear();
        for (size_t i = 0; i < recchan.nrow(); ++i) {
          if (recchan.row(i)(0) == data_spw[irow]) {
            fitrange_start.push_back(recchan.row(i)(1));
            fitrange_end.push_back(recchan.row(i)(2));
          }
        }
        if (!linefinding && nfit.size() != fitrange_start.size()) {
          throw(AipsError(
              "the number of elements of nfit and fitranges specified in spw must be identical."));
        }

        // loop over polarization
        for (size_t ipol = 0; ipol < num_pol; ++ipol) {
          // get a channel mask from data cube
          // (note that the variable 'mask' is flag in the next line
          // actually, then it will be converted to real mask when
          // taking AND with user-given mask info. this is just for
          // saving memory usage...)
          get_flag_from_cube(flag_chunk, irow, ipol, num_chan, mask_data);
          // skip spectrum if all channels flagged
          if (allchannels_flagged(num_chan, mask_data)) {
            continue;
          }
          ++num_spec;

          // convert flag to mask by taking logical NOT of flag
          // and then operate logical AND with in_mask
          for (size_t ichan = 0; ichan < num_chan; ++ichan) {
            mask_data[ichan] = in_mask[idx][ichan] && (!(mask_data[ichan]));
          }
          // get a spectrum from data cube
          get_spectrum_from_cube(data_chunk, irow, ipol, num_chan, spec_data);

          // line finding. get fit mask (invert=false)
          if (linefinding) {
            list<pair<size_t, size_t>> line_ranges
              = findLineAndGetRanges(num_chan, spec_data, mask_data,
                                     threshold, avg_limit, minwidth,
                                     edge, false);
            if (line_ranges.size()==0) {
              ++num_noline;
              continue;
            }
            size_t nline = line_ranges.size();
            fitrange_start.resize(nline);
            fitrange_end.resize(nline);
            nfit.resize(nline);
            auto range=line_ranges.begin();
            for (size_t iline=0; iline<nline; ++iline){
              fitrange_start[iline] = (*range).first;
              fitrange_end[iline] = (*range).second;
              nfit[iline] = 1;
              ++range;
            }
          }

          Vector<Float> x_;
          x_.resize(num_chan);
          Vector<Float> y_;
          y_.resize(num_chan);
          Vector<Bool> m_;
          m_.resize(num_chan);
          for (size_t ichan = 0; ichan < num_chan; ++ichan) {
            x_[ichan] = static_cast<Float>(ichan);
            y_[ichan] = spec_data[ichan];
          }
          Vector<Float> parameters_;
          Vector<Float> error_;

          PtrBlock<Function<Float>*> funcs_;
          std::vector<std::string> funcnames_;
          std::vector<int> funccomponents_;
          std::string expr;
          if (fitfunc == "gaussian") {
            expr = "gauss";
          } else if (fitfunc == "lorentzian") {
            expr = "lorentz";
          }

          bool any_converged = false;
          for (size_t ifit = 0; ifit < nfit.size(); ++ifit) {
            if (nfit[ifit] == 0)
              continue;

            if (0 < ifit)
              ofs << ":";

            //extract spec/mask within fitrange
            for (size_t ichan = 0; ichan < num_chan; ++ichan) {
              if ((fitrange_start[ifit] <= ichan)
                  && (ichan <= fitrange_end[ifit])) {
                m_[ichan] = mask_data[ichan];
              } else {
                m_[ichan] = false;
              }
            }

            //initial guesss
            Vector<Float> peak;
            Vector<Float> cent;
            Vector<Float> fwhm;
            peak.resize(nfit[ifit]);
            cent.resize(nfit[ifit]);
            fwhm.resize(nfit[ifit]);
            if (nfit[ifit] == 1) {
              Float sum = 0.0;
              Float max_spec = fabs(y_[fitrange_start[ifit]]);
              Float max_spec_x = x_[fitrange_start[ifit]];
              bool is_positive = true;
              for (size_t ichan = fitrange_start[ifit];
                   ichan <= fitrange_end[ifit]; ++ichan) {
                sum += y_[ichan];
                if (max_spec < fabs(y_[ichan])) {
                  max_spec = fabs(y_[ichan]);
                  max_spec_x = x_[ichan];
                  is_positive = (fabs(y_[ichan]) == y_[ichan]);
                }
              }
              peak[0] = max_spec * (is_positive ? 1 : -1);
              cent[0] = max_spec_x;
              fwhm[0] = fabs(sum / max_spec * 0.7);
            } else {
              size_t x_start = fitrange_start[ifit];
              size_t x_width = (fitrange_end[ifit] - fitrange_start[ifit]) / nfit[ifit];
              size_t x_end = x_start + x_width;
              for (size_t icomp = 0; icomp < nfit[ifit]; ++icomp) {
                if (icomp == nfit[ifit] - 1) {
                  x_end = fitrange_end[ifit] + 1;
                }

                Float sum = 0.0;
                Float max_spec = fabs(y_[x_start]);
                Float max_spec_x = x_[x_start];
                bool is_positive = true;
                for (size_t ichan = x_start; ichan < x_end; ++ichan) {
                  sum += y_[ichan];
                  if (max_spec < fabs(y_[ichan])) {
                    max_spec = fabs(y_[ichan]);
                    max_spec_x = x_[ichan];
                    is_positive = (fabs(y_[ichan]) == y_[ichan]);
                  }
                }
                peak[icomp] = max_spec * (is_positive ? 1 : -1);
                cent[icomp] = max_spec_x;
                fwhm[icomp] = fabs(sum / max_spec * 0.7);

                x_start += x_width;
                x_end += x_width;
              }
            }

            //fitter setup
            funcs_.resize(nfit[ifit]);
            funcnames_.clear();
            funccomponents_.clear();
            for (size_t icomp = 0; icomp < funcs_.nelements(); ++icomp) {
              if (expr == "gauss") {
                funcs_[icomp] = new Gaussian1D<Float>();
              } else if (expr == "lorentz") {
                funcs_[icomp] = new Lorentzian1D<Float>();
              }
              (funcs_[icomp]->parameters())[0] = peak[icomp]; //initial guess (peak)
              (funcs_[icomp]->parameters())[1] = cent[icomp]; //initial guess (centre)
              (funcs_[icomp]->parameters())[2] = fwhm[icomp]; //initial guess (fwhm)
              funcnames_.push_back(expr);
              funccomponents_.push_back(3);
            }

            //actual fitting
            NonLinearFitLM<Float> fitter;
            CompoundFunction<Float> func;
            for (size_t icomp = 0; icomp < funcs_.nelements(); ++icomp) {
              func.addFunction(*funcs_[icomp]);
            }
            fitter.setFunction(func);
            fitter.setMaxIter(50 + 10 * funcs_.nelements());
            fitter.setCriteria(0.001);      // Convergence criterium

            parameters_.resize();
            parameters_ = fitter.fit(x_, y_, &m_);
            any_converged |= fitter.converged();
            // if (!fitter.converged()) {
            //   throw(AipsError("Failed in fitting. Fitter did not converge."));
            // }
            error_.resize();
            error_ = fitter.errors();

            //write best-fit parameters to tempfile/outfile
            for (size_t icomp = 0; icomp < funcs_.nelements(); ++icomp) {
              if (0 < icomp)
                ofs << ":";
              size_t offset = 3 * icomp;
              ofs.precision(4);
              ofs.setf(ios::fixed);
              ofs << scans[irow] << ","     // scanID
                  << times[irow] << ","     // time
                  << antennas[irow] << ","  // antennaID
                  << beams[irow] << ","     // beamID
                  << data_spw[irow] << ","  // spwID
                  << ipol << ",";           // polID
              ofs.precision(8);
              ofs << parameters_[offset + 1] << "," << error_[offset + 1] << "," // cent
                  << parameters_[offset + 0] << "," << error_[offset + 0] << "," // peak
                  << parameters_[offset + 2] << "," << error_[offset + 2]; // fwhm
            }
          }        //end of nfit loop
          ofs << "\n";
          // count up spectra w/o any line fit
          if (!any_converged) ++num_noline;

        }        //end of polarization loop
      }        // end of MS row loop
    }        //end of vi loop
  }        //end of chunk loop

  if (num_noline==num_spec) {
    os << LogIO::WARN
       << "Fitter did not converge on any fit components." << LogIO::POST;
  }
  else if (num_noline > 0) {
    os << "No convergence for fitting to " << num_noline
       << " out of " << num_spec << " spectra" << LogIO::POST;
  }

  finalize_process();
  ofs.close();
}

//Subtract baseline by per spectrum fitting parameters
void SingleDishMS::subtractBaselineVariable(string const& in_column_name,
                                            string const& out_ms_name,
                                            string const& out_bloutput_name,
                                            bool const& do_subtract,
                                            string const& in_spw,
                                            bool const& update_weight,
                                            string const& sigma_value,
                                            string const& param_file,
                                            bool const& verbose) {

  LogIO os(_ORIGIN);
  os << "Fitting and subtracting baseline using parameters in file "
     << param_file << LogIO::POST;

  Block<Int> columns(1);
  columns[0] = MS::DATA_DESC_ID;
  prepare_for_process(in_column_name, out_ms_name, columns, false);
  vi::VisibilityIterator2 *vi = sdh_->getVisIter();
  vi->setRowBlocking(kNRowBlocking);
  vi::VisBuffer2 *vb = vi->getVisBuffer();

  split_bloutputname(out_bloutput_name);
  bool write_baseline_csv = (bloutputname_csv != "");
  bool write_baseline_text = (bloutputname_text != "");
  bool write_baseline_table = (bloutputname_table != "");
  ofstream ofs_csv;
  ofstream ofs_txt;
  BaselineTable *bt = 0;

  if (write_baseline_csv) {
    ofs_csv.open(bloutputname_csv.c_str());
  }
  if (write_baseline_text) {
    ofs_txt.open(bloutputname_text.c_str(), std::ios::app);
  }
  if (write_baseline_table) {
    bt = new BaselineTable(vi->ms());
  }

  Vector<Int> recspw;
  Matrix<Int> recchan;
  Vector<size_t> nchan;
  Vector<Vector<Bool> > in_mask;
  Vector<bool> nchan_set;
  parse_spw(in_spw, recspw, recchan, nchan, in_mask, nchan_set);

  // parse fitting parameters in the text file
  BLParameterParser parser(param_file);
  std::vector<size_t> baseline_types = parser.get_function_types();
  map<size_t const, uint16_t> max_orders;
  for (size_t i = 0; i < baseline_types.size(); ++i) {
    max_orders[baseline_types[i]] = parser.get_max_order(baseline_types[i]);
  }
  { //DEBUG ouput
    os << LogIO::DEBUG1 << "Baseline Types = " << baseline_types << LogIO::POST;
    os << LogIO::DEBUG1 << "Max Orders:" << LogIO::POST;
    map<size_t const, uint16_t>::iterator iter = max_orders.begin();
    while (iter != max_orders.end()) {
      os << LogIO::DEBUG1 << "- type " << (*iter).first << ": "
         << (*iter).second << LogIO::POST;
      ++iter;
    }
  }

  // Baseline Contexts reservoir
  // key: Sakura_BaselineType enum,
  // value: a vector of Sakura_BaselineContextFloat for various nchans
  map<size_t const, std::vector<LIBSAKURA_SYMBOL(LSQFitContextFloat) *> > context_reservoir;
  {
    map<size_t const, uint16_t>::iterator iter = max_orders.begin();
    while (iter != max_orders.end()) {
      context_reservoir[(*iter).first] = std::vector<LIBSAKURA_SYMBOL(LSQFitContextFloat) *>();
      ++iter;
    }
  }

  Vector<size_t> ctx_indices(recspw.size(), 0ul);
  //stores the number of channels of corresponding elements in contexts list.
  // WORKAROUND for absense of the way to get num_bases_data in context.
  vector<size_t> ctx_nchans;

  LIBSAKURA_SYMBOL(Status) status;
  LIBSAKURA_SYMBOL(LSQFitStatus) bl_status;

  std::vector<float> weight(WeightIndex_kNum);
  size_t const var_index = (sigma_value == "stddev") ? WeightIndex_kStddev : WeightIndex_kRms;

  for (vi->originChunks(); vi->moreChunks(); vi->nextChunk()) {
    for (vi->origin(); vi->more(); vi->next()) {
      Vector<Int> scans = vb->scan();
      Vector<Double> times = vb->time();
      Vector<Int> beams = vb->feed1();
      Vector<Int> antennas = vb->antenna1();
      Vector<Int> data_spw = vb->spectralWindows();
      size_t const num_chan = static_cast<size_t>(vb->nChannels());
      size_t const num_pol = static_cast<size_t>(vb->nCorrelations());
      size_t const num_row = static_cast<size_t>(vb->nRows());
      auto orig_rows = vb->rowIds();
      Cube<Float> data_chunk(num_pol, num_chan, num_row);
      Vector<float> spec(num_chan);
      Cube<Bool> flag_chunk(num_pol, num_chan, num_row);
      Vector<bool> flag(num_chan);
      Vector<bool> mask(num_chan);
      Vector<bool> mask_after_clipping(num_chan);
      // CAUTION!!!
      // data() method must be used with special care!!!
      float *spec_data = spec.data();
      bool *flag_data = flag.data();
      bool *mask_data = mask.data();
      bool *mask_after_clipping_data = mask_after_clipping.data();
      Matrix<Float> weight_matrix(num_pol, num_row, Array<Float>::uninitialized);

      uInt final_mask[num_pol];
      uInt final_mask_after_clipping[num_pol];
      final_mask[0] = 0;
      final_mask[1] = 0;
      final_mask_after_clipping[0] = 0;
      final_mask_after_clipping[1] = 0;

      bool new_nchan = false;
      get_nchan_and_mask(recspw, data_spw, recchan, num_chan, nchan, in_mask, nchan_set, new_nchan);
      // check if context should be created once per chunk
      // in the first actual excution of baseline.
      bool check_context = true;

      // get data/flag cubes (npol*nchan*nrow) from VisBuffer
      get_data_cube_float(*vb, data_chunk);
      get_flag_cube(*vb, flag_chunk);

      // get weight matrix (npol*nrow) from VisBuffer
      if (update_weight) {
        get_weight_matrix(*vb, weight_matrix);
      }

      // loop over MS rows
      for (size_t irow = 0; irow < num_row; ++irow) {
        size_t idx = 0;
        for (size_t ispw = 0; ispw < recspw.nelements(); ++ispw) {
          if (data_spw[irow] == recspw[ispw]) {
            idx = ispw;
            break;
          }
        }

        //prepare varables for writing baseline table
        Array<Bool> apply_mtx(IPosition(2, num_pol, 1), true);
        Array<uInt> bltype_mtx(IPosition(2, num_pol, 1));
        Array<Int> fpar_mtx(IPosition(2, num_pol, 1));
        std::vector<std::vector<double> > ffpar_mtx_tmp(num_pol);
        std::vector<std::vector<uInt> > masklist_mtx_tmp(num_pol);
        std::vector<std::vector<double> > coeff_mtx_tmp(num_pol);
        Array<Float> rms_mtx(IPosition(2, num_pol, 1));
        Array<Float> cthres_mtx(IPosition(2, num_pol, 1));
        Array<uInt> citer_mtx(IPosition(2, num_pol, 1));
        Array<Bool> uself_mtx(IPosition(2, num_pol, 1));
        Array<Float> lfthres_mtx(IPosition(2, num_pol, 1));
        Array<uInt> lfavg_mtx(IPosition(2, num_pol, 1));
        Array<uInt> lfedge_mtx(IPosition(2, num_pol, 2));

        size_t num_apply_true = 0;
        size_t num_ffpar_max = 0;
        size_t num_masklist_max = 0;
        size_t num_coeff_max = 0;
        std::vector<size_t> numcoeff(num_pol);

        // loop over polarization
        for (size_t ipol = 0; ipol < num_pol; ++ipol) {
          // get a channel mask from data cube
          // (note that the variable 'mask' is flag in the next line
          // actually, then it will be converted to real mask when
          // taking AND with user-given mask info. this is just for
          // saving memory usage...)
          get_flag_from_cube(flag_chunk, irow, ipol, num_chan, flag_data);
          // skip spectrum if all channels flagged
          if (allchannels_flagged(num_chan, flag_data)) {
            os << LogIO::DEBUG1 << "Row " << orig_rows[irow] << ", Pol " << ipol
               << ": All channels flagged. Skipping." << LogIO::POST;
            apply_mtx[0][ipol] = false;
            continue;
          }

          // convert flag to mask by taking logical NOT of flag
          // and then operate logical AND with in_mask
          for (size_t ichan = 0; ichan < num_chan; ++ichan) {
            mask_data[ichan] = in_mask[idx][ichan] && (!(flag_data[ichan]));
          }
          // get fitting parameter
          BLParameterSet fit_param;
          if (!parser.GetFitParameter(orig_rows[irow], ipol, fit_param)) { //no fit requrested
            flag_spectrum_in_cube(flag_chunk, irow, ipol);
            os << LogIO::DEBUG1 << "Row " << orig_rows[irow] << ", Pol " << ipol
               << ": Fit not requested. Skipping." << LogIO::POST;
            apply_mtx[0][ipol] = false;
            continue;
          }
          if (verbose) {
            os << "Fitting Parameter" << LogIO::POST;
            os << "[ROW " << orig_rows[irow] << " (nchan " << num_chan << ")" << ", POL" << ipol << "]"
               << LogIO::POST;
            fit_param.PrintSummary();
          }
          // Create contexts when actually subtract baseine for the first time (if not yet exist)
          if (check_context) {
            // Generate context for all necessary baseline types
            map<size_t const, uint16_t>::iterator iter = max_orders.begin();
            while (iter != max_orders.end()) {
              get_baseline_context((*iter).first, (*iter).second, num_chan, idx,
                                   ctx_indices, ctx_nchans, context_reservoir[(*iter).first]);
              ++iter;
            }
            check_context = false;
          }
          // get mask from BLParameterset and create composit mask
          if (fit_param.baseline_mask != "") {
            stringstream local_spw;
            local_spw << data_spw[irow] << ":" << fit_param.baseline_mask;
            Record selrec = sdh_->getSelRec(local_spw.str());
            Matrix<Int> local_rec_chan = selrec.asArrayInt("channel");
            Vector<Bool> local_mask(num_chan, false);
            get_mask_from_rec(data_spw[irow], local_rec_chan, local_mask, false);
            for (size_t ichan = 0; ichan < num_chan; ++ichan) {
              mask_data[ichan] = mask_data[ichan] && local_mask[ichan];
            }
          }
          // check for composit mask and flag if no valid channel to fit
          if (NValidMask(num_chan, mask_data) == 0) {
            flag_spectrum_in_cube(flag_chunk, irow, ipol);
            apply_mtx[0][ipol] = false;
            os << LogIO::DEBUG1 << "Row " << orig_rows[irow] << ", Pol " << ipol
               << ": No valid channel to fit. Skipping" << LogIO::POST;
            continue;
          }
          // get a spectrum from data cube
          get_spectrum_from_cube(data_chunk, irow, ipol, num_chan, spec_data);

          // get baseline context
          map<size_t const, std::vector<LIBSAKURA_SYMBOL(LSQFitContextFloat) *> >::iterator
          iter = context_reservoir.find(fit_param.baseline_type);
          if (iter == context_reservoir.end()) {
            throw(AipsError("Invalid baseline type detected!"));
          }
          LIBSAKURA_SYMBOL(LSQFitContextFloat) * context = (*iter).second[ctx_indices[idx]];

          // Number of coefficients to fit this spectrum
          size_t num_coeff;
          size_t bltype = static_cast<size_t>(fit_param.baseline_type);
          switch (bltype) {
          case BaselineType_kPolynomial:
          case BaselineType_kChebyshev:
            status = LIBSAKURA_SYMBOL(GetNumberOfCoefficientsFloat)(context, fit_param.order, &num_coeff);
            check_sakura_status("sakura_GetNumberOfCoefficientsFloat", status);
            break;
          case BaselineType_kCubicSpline:
            num_coeff = 4 * fit_param.npiece;
            break;
          default:
            throw(AipsError("Unsupported baseline type."));
          }
          numcoeff[ipol] = num_coeff;

          // Final check of the valid number of channels
          size_t num_min = (bltype == BaselineType_kCubicSpline) ? fit_param.npiece + 3 : num_coeff;
          if (NValidMask(num_chan, mask_data) < num_min) {
            flag_spectrum_in_cube(flag_chunk, irow, ipol);
            apply_mtx[0][ipol] = false;
            os << LogIO::WARN
               << "Too few valid channels to fit. Skipping Antenna "
               << antennas[irow] << ", Beam " << beams[irow] << ", SPW "
               << data_spw[irow] << ", Pol " << ipol << ", Time "
               << MVTime(times[irow] / 24. / 3600.).string(MVTime::YMD, 8)
               << LogIO::POST;
            continue;
          }

          // actual execution of single spectrum
          float rms;
          size_t num_boundary = 0;
          if (bltype == BaselineType_kCubicSpline) {
            num_boundary = fit_param.npiece+1;
          }
          Vector<size_t> boundary(num_boundary,Array<size_t>::uninitialized);
          size_t *boundary_data = boundary.data();

          if (write_baseline_text || write_baseline_csv || write_baseline_table) {
            num_apply_true++;
            bltype_mtx[0][ipol] = (uInt)fit_param.baseline_type;
            Int fpar_tmp;
            switch (bltype) {
            case BaselineType_kPolynomial:
            case BaselineType_kChebyshev:
              fpar_tmp = (Int)fit_param.order;
              break;
            case BaselineType_kCubicSpline:
              fpar_tmp = (Int)fit_param.npiece;
              break;
            default:
              throw(AipsError("Unsupported baseline type."));
            }
            fpar_mtx[0][ipol] = fpar_tmp;

            if (num_coeff > num_coeff_max) {
              num_coeff_max = num_coeff;
            }
            Vector<double> coeff(num_coeff);
            // CAUTION!!!
            // data() method must be used with special care!!!
            double *coeff_data = coeff.data();
            string get_coeff_funcname;
            switch (bltype) {
            case BaselineType_kPolynomial:
            case BaselineType_kChebyshev:
              status = LIBSAKURA_SYMBOL(LSQFitPolynomialFloat)(
                context, fit_param.order,
                num_chan, spec_data, mask_data,
                fit_param.clip_threshold_sigma, fit_param.num_fitting_max,
                num_coeff, coeff_data, nullptr, nullptr,
                mask_after_clipping_data, &rms, &bl_status);

              for (size_t i = 0; i < num_chan; ++i) {
                if (mask_data[i] == false) {
                  final_mask[ipol] += 1;
                }
                if (mask_after_clipping_data[i] == false) {
                  final_mask_after_clipping[ipol] += 1;
                }
              }

              get_coeff_funcname = "sakura_LSQFitPolynomialFloat";
              break;
            case BaselineType_kCubicSpline:
              status = LIBSAKURA_SYMBOL(LSQFitCubicSplineFloat)(
                context, fit_param.npiece,
                num_chan, spec_data, mask_data,
                fit_param.clip_threshold_sigma, fit_param.num_fitting_max,
                reinterpret_cast<double (*)[4]>(coeff_data), nullptr, nullptr,
                mask_after_clipping_data, &rms, boundary_data, &bl_status);

              for (size_t i = 0; i < num_chan; ++i) {
                if (mask_data[i] == false) {
                  final_mask[ipol] += 1;
                }
                if (mask_after_clipping_data[i] == false) {
                  final_mask_after_clipping[ipol] += 1;
                }
              }

              get_coeff_funcname = "sakura_LSQFitCubicSplineFloat";
              break;
            default:
              throw(AipsError("Unsupported baseline type."));
            }
            check_sakura_status(get_coeff_funcname, status);

            size_t num_ffpar = get_num_coeff_bloutput(fit_param.baseline_type, fit_param.npiece, num_ffpar_max);
            ffpar_mtx_tmp[ipol].clear();
            for (size_t ipiece = 0; ipiece < num_ffpar; ++ipiece) {
              ffpar_mtx_tmp[ipol].push_back(boundary_data[ipiece]);
            }

            coeff_mtx_tmp[ipol].clear();
            for (size_t icoeff = 0; icoeff < num_coeff; ++icoeff) {
              coeff_mtx_tmp[ipol].push_back(coeff_data[icoeff]);
            }

            Vector<uInt> masklist;
            get_masklist_from_mask(num_chan, mask_after_clipping_data, masklist);
            if (masklist.size() > num_masklist_max) {
              num_masklist_max = masklist.size();
            }
            masklist_mtx_tmp[ipol].clear();
            for (size_t imask = 0; imask < masklist.size(); ++imask) {
              masklist_mtx_tmp[ipol].push_back(masklist[imask]);
            }

            string subtract_funcname;
            switch (fit_param.baseline_type) {
            case BaselineType_kPolynomial:
            case BaselineType_kChebyshev:
              status = LIBSAKURA_SYMBOL(SubtractPolynomialFloat)(
                  context, num_chan, spec_data, num_coeff, coeff_data,
                  spec_data);
              subtract_funcname = "sakura_SubtractPolynomialFloat";
              break;
            case BaselineType_kCubicSpline:
              status = LIBSAKURA_SYMBOL(SubtractCubicSplineFloat)(
                  context,
                  num_chan, spec_data, fit_param.npiece, reinterpret_cast<double (*)[4]>(coeff_data),
                  boundary_data, spec_data);
              subtract_funcname = "sakura_SubtractCubicSplineFloat";
              break;
            default:
              throw(AipsError("Unsupported baseline type."));
            }
            check_sakura_status(subtract_funcname, status);

            rms_mtx[0][ipol] = rms;

            cthres_mtx[0][ipol] = fit_param.clip_threshold_sigma;
            citer_mtx[0][ipol] = (uInt)fit_param.num_fitting_max - 1;
            uself_mtx[0][ipol] = (Bool)fit_param.line_finder.use_line_finder;
            lfthres_mtx[0][ipol] = fit_param.line_finder.threshold;
            lfavg_mtx[0][ipol] = fit_param.line_finder.chan_avg_limit;
            for (size_t iedge = 0; iedge < 2; ++iedge) {
              lfedge_mtx[iedge][ipol] = fit_param.line_finder.edge[iedge];
            }

          } else {
            string subtract_funcname;
            switch (fit_param.baseline_type) {
            case BaselineType_kPolynomial:
            case BaselineType_kChebyshev:
              status = LIBSAKURA_SYMBOL(LSQFitPolynomialFloat)(
                context, fit_param.order,
                num_chan, spec_data, mask_data,
                fit_param.clip_threshold_sigma, fit_param.num_fitting_max,
                num_coeff, nullptr, nullptr, spec_data,
                mask_after_clipping_data, &rms, &bl_status);
              subtract_funcname = "sakura_LSQFitPolynomialFloat";
              break;
            case BaselineType_kCubicSpline:
              status = LIBSAKURA_SYMBOL(LSQFitCubicSplineFloat)(
                context, fit_param.npiece,
                num_chan, spec_data, mask_data,
                fit_param.clip_threshold_sigma, fit_param.num_fitting_max,
                nullptr, nullptr, spec_data,
                mask_after_clipping_data, &rms, boundary_data, &bl_status);
              subtract_funcname = "sakura_LSQFitCubicSplineFloat";
              break;
            default:
              throw(AipsError("Unsupported baseline type."));
            }
            check_sakura_status(subtract_funcname, status);
          }
          // set back a spectrum to data cube
          if (do_subtract) {
            set_spectrum_to_cube(data_chunk, irow, ipol, num_chan, spec_data);
          }

          if (update_weight) {
            compute_weight(num_chan, spec_data, mask_data, weight);
            set_weight_to_matrix(weight_matrix, irow, ipol, weight.at(var_index));
          }
        } // end of polarization loop

        // output results of fitting
        if (num_apply_true == 0) continue;

        Array<Float> ffpar_mtx(IPosition(2, num_pol, num_ffpar_max));
        set_matrix_for_bltable<double, Float>(num_pol, num_ffpar_max,
                                              ffpar_mtx_tmp, ffpar_mtx);
        Array<uInt> masklist_mtx(IPosition(2, num_pol, num_masklist_max));
        set_matrix_for_bltable<uInt, uInt>(num_pol, num_masklist_max,
                                           masklist_mtx_tmp, masklist_mtx);
        Array<Float> coeff_mtx(IPosition(2, num_pol, num_coeff_max));
        set_matrix_for_bltable<double, Float>(num_pol, num_coeff_max,
                                              coeff_mtx_tmp, coeff_mtx);
        Matrix<uInt> masklist_mtx2 = masklist_mtx;
        Matrix<Bool> apply_mtx2 = apply_mtx;

        if (write_baseline_table) {
          bt->appenddata((uInt)scans[irow], (uInt)beams[irow], (uInt)antennas[irow],
                         (uInt)data_spw[irow], 0, times[irow],
                         apply_mtx, bltype_mtx, fpar_mtx, ffpar_mtx, masklist_mtx,
                         coeff_mtx, rms_mtx, (uInt)num_chan, cthres_mtx, citer_mtx,
                         uself_mtx, lfthres_mtx, lfavg_mtx, lfedge_mtx);
        }

        if (write_baseline_text) {
          for (size_t ipol = 0; ipol < num_pol; ++ipol) {
            if (apply_mtx2(ipol, 0) == false) continue;

            ofs_txt << "Scan" << '[' << (uInt)scans[irow] << ']' << ' '
                    << "Beam" << '[' << (uInt)beams[irow] << ']' << ' '
                    << "Spw"  << '[' << (uInt)data_spw[irow] << ']' << ' '
                    << "Pol"  << '[' << ipol << ']' << ' '
                    << "Time" << '[' << MVTime(times[irow]/ 24. / 3600.).string(MVTime::YMD, 8) << ']'
                    << endl;
            ofs_txt << endl;
            ofs_txt << "Fitter range = " << '[';

            for (size_t imasklist = 0; imasklist < num_masklist_max / 2; ++imasklist) {
              if (imasklist == 0) {
                ofs_txt << '[' << masklist_mtx2(ipol, 2 * imasklist) << ';'
                        << masklist_mtx2(ipol, 2 * imasklist + 1) << ']';
              }
              if (imasklist >= 1
                  && (0 != masklist_mtx2(ipol, 2 * imasklist)
                      && 0 != masklist_mtx2(ipol, 2 * imasklist + 1))) {
                ofs_txt << ",[" << masklist_mtx2(ipol, 2 * imasklist) << ','
                        << masklist_mtx2(ipol, 2 * imasklist + 1) << ']';
              }
            }

            ofs_txt << ']' << endl;
            ofs_txt << endl;

            Matrix<uInt> bltype_mtx2 = bltype_mtx[0][ipol];
            Matrix<Int> fpar_mtx2 = fpar_mtx[0][ipol];
            Matrix<Float> rms_mtx2 = rms_mtx[0][ipol];
            string bltype_name;
            string blparam_name = " order = ";
            if (bltype_mtx2(0, 0) == (uInt)0) {
              bltype_name = "poly";
            } else if (bltype_mtx2(0, 0) == (uInt)1) {
              bltype_name = "chebyshev";
            } else if (bltype_mtx2(0, 0) == (uInt)2) {
              bltype_name = "cspline";
              blparam_name = " npiece = ";
            }

            ofs_txt << "Baseline parameters  Function = "
                    << bltype_name << " " << blparam_name
                    << fpar_mtx2(0, 0) << endl;
            ofs_txt << endl;
            ofs_txt << "Results of baseline fit" << endl;
            ofs_txt << endl;
            Matrix<Float> coeff_mtx2 = coeff_mtx;
            for (size_t icoeff = 0; icoeff < numcoeff[ipol]; ++icoeff) {
              ofs_txt << "p" << icoeff << " = "
                      << setprecision(8) << coeff_mtx2(ipol, icoeff) << "  ";
            }
            ofs_txt << endl;
            ofs_txt << endl;
            ofs_txt << "rms = ";
            ofs_txt << setprecision(8) << rms_mtx2(0, 0) << endl;
            ofs_txt << endl;
            ofs_txt << "Number of clipped channels = "
                    << final_mask_after_clipping[ipol] - final_mask[ipol] << endl;
            ofs_txt << endl;
            ofs_txt << "------------------------------------------------------"
                    << endl;
            ofs_txt << endl;
          }
        }

        if (write_baseline_csv) {
          for (size_t ipol = 0; ipol < num_pol; ++ipol) {
            if (apply_mtx2(ipol, 0) == false) continue;

            ofs_csv << (uInt)scans[irow] << ',' << (uInt)beams[irow] << ','
                    << (uInt)data_spw[irow] << ',' << ipol << ','
                    << setprecision(12) << times[irow] << ',';
            ofs_csv << '[';
            for (size_t imasklist = 0; imasklist < num_masklist_max / 2; ++imasklist) {
              if (imasklist == 0) {
                ofs_csv << '[' << masklist_mtx2(ipol, 2 * imasklist) << ';'
                        << masklist_mtx2(ipol, 2 * imasklist + 1) << ']';
              }
              if (imasklist >= 1
                  && (0 != masklist_mtx2(ipol, 2 * imasklist)
                      && 0 != masklist_mtx2(ipol, 2 * imasklist + 1))) {
                ofs_csv << ";[" << masklist_mtx2(ipol, 2 * imasklist) << ';'
                        << masklist_mtx2(ipol, 2 * imasklist + 1) << ']';
              }
            }

            ofs_csv << ']' << ',';
            Matrix<uInt> bltype_mtx2 = bltype_mtx[0][ipol];
            string bltype_name;
            if (bltype_mtx2(0, 0) == (uInt)0) {
              bltype_name = "poly";
            } else if (bltype_mtx2(0, 0) == (uInt)1) {
              bltype_name = "chebyshev";
            } else if (bltype_mtx2(0, 0) == (uInt)2) {
              bltype_name = "cspline";
            }

            Matrix<Int> fpar_mtx2 = fpar_mtx;
            Matrix<Float> coeff_mtx2 = coeff_mtx;
            ofs_csv << bltype_name.c_str() << ',' << fpar_mtx2(ipol, 0)
                    << ',';
            for (size_t icoeff = 0; icoeff < numcoeff[ipol]; ++icoeff) {
              ofs_csv << setprecision(8) << coeff_mtx2(ipol, icoeff) << ',';
            }
            Matrix<Float> rms_mtx2 = rms_mtx;
            ofs_csv << setprecision(8) << rms_mtx2(ipol, 0) << ',';
            ofs_csv << final_mask_after_clipping[ipol] - final_mask[ipol];
            ofs_csv << endl;
          }
        }
      } // end of chunk row loop
      // write back data and flag cube to VisBuffer
      if (update_weight) {
        sdh_->fillCubeToOutputMs(vb, data_chunk, &flag_chunk, &weight_matrix);
      } else {
        sdh_->fillCubeToOutputMs(vb, data_chunk, &flag_chunk);
      }
    } // end of vi loop
  } // end of chunk loop

  if (write_baseline_csv) {
    ofs_csv.close();
  }
  if (write_baseline_text) {
    ofs_txt.close();
  }
  if (write_baseline_table) {
    bt->save(bloutputname_table);
    delete bt;
  }

  finalize_process();
  // destroy baseline contexts
  map<size_t const, std::vector<LIBSAKURA_SYMBOL(LSQFitContextFloat) *> >::iterator ctxiter = context_reservoir.begin();
  while (ctxiter != context_reservoir.end()) {
    destroy_baseline_contexts (context_reservoir[(*ctxiter).first]);
    ++ctxiter;
  }
} //end subtractBaselineVariable

list<pair<size_t, size_t>> SingleDishMS::findLineAndGetRanges(size_t const num_data,
                                                              float const* data,
                                                              bool * mask,
                                                              float const threshold,
                                                              int const avg_limit,
                                                              int const minwidth,
                                                              vector<int> const& edge,
                                                              bool const invert) {
  // input value check
  AlwaysAssert(minwidth > 0, AipsError);
  AlwaysAssert(avg_limit >= 0, AipsError);
  size_t max_iteration = 10;
  size_t maxwidth = num_data;
  AlwaysAssert(maxwidth > static_cast<size_t>(minwidth), AipsError);
  // edge handling
  pair<size_t, size_t> lf_edge;
  if (edge.size() == 0) {
    lf_edge = pair<size_t, size_t>(0, 0);
  } else if (edge.size() == 1) {
    AlwaysAssert(edge[0] >= 0, AipsError);
    lf_edge = pair<size_t, size_t>(static_cast<size_t>(edge[0]), static_cast<size_t>(edge[0]));
  } else {
    AlwaysAssert(edge[0] >= 0 && edge[1] >= 0, AipsError);
    lf_edge = pair<size_t, size_t>(static_cast<size_t>(edge[0]), static_cast<size_t>(edge[1]));
  }
  // line detection
  list<pair<size_t, size_t>> line_ranges = linefinder::MADLineFinder(num_data,
      data, mask, threshold, max_iteration, static_cast<size_t>(minwidth),
      maxwidth, static_cast<size_t>(avg_limit), lf_edge);
  // debug output
  LogIO os(_ORIGIN);
  os << LogIO::DEBUG1 << line_ranges.size() << " lines found: ";
  for (list<pair<size_t, size_t>>::iterator iter = line_ranges.begin();
      iter != line_ranges.end(); ++iter) {
    os << "[" << (*iter).first << ", " << (*iter).second << "] ";
  }
  os << LogIO::POST;
  if (invert) { // eliminate edge channels from output mask
    if (lf_edge.first > 0)
      line_ranges.push_front(pair<size_t, size_t>(0, lf_edge.first - 1));
    if (lf_edge.second > 0)
      line_ranges.push_back(
          pair<size_t, size_t>(num_data - lf_edge.second, num_data - 1));
  }
  return line_ranges;
}

void SingleDishMS::findLineAndGetMask(size_t const num_data,
                                      float const* data,
                                      bool const* in_mask,
                                      float const threshold,
                                      int const avg_limit,
                                      int const minwidth,
                                      vector<int> const& edge,
                                      bool const invert,
                                      bool* out_mask) {
  // copy input mask to output mask vector if necessary
  if (in_mask != out_mask) {
    for (size_t i = 0; i < num_data; ++i) {
      out_mask[i] = in_mask[i];
    }
  }
  // line finding
  list<pair<size_t, size_t>> line_ranges
    = findLineAndGetRanges(num_data, data, out_mask, threshold,
                           avg_limit, minwidth, edge, invert);
  // line mask creation (do not initialize in case of baseline mask)
  linefinder::getMask(num_data, out_mask, line_ranges, invert, !invert);
}

void SingleDishMS::smooth(string const &kernelType,
                          float const kernelWidth,
                          string const &columnName,
                          string const &outMSName) {
  LogIO os(_ORIGIN);
  os << "Input parameter summary:" << endl << "   kernelType = " << kernelType
      << endl << "   kernelWidth = " << kernelWidth << endl
      << "   columnName = " << columnName << endl << "   outMSName = "
      << outMSName << LogIO::POST;

  // Initialization
  doSmoothing_ = true;
  prepare_for_process(columnName, outMSName);

  // configure smoothing
  sdh_->setSmoothing(kernelType, kernelWidth);
  sdh_->initializeSmoothing();

  // get VI/VB2 access
  vi::VisibilityIterator2 *visIter = sdh_->getVisIter();
  visIter->setRowBlocking(kNRowBlocking);
  vi::VisBuffer2 *vb = visIter->getVisBuffer();

  double startTime = gettimeofday_sec();

  for (visIter->originChunks(); visIter->moreChunks(); visIter->nextChunk()) {
    for (visIter->origin(); visIter->more(); visIter->next()) {
      sdh_->fillOutputMs(vb);
    }
  }

  double endTime = gettimeofday_sec();
  os << LogIO::DEBUGGING
     << "Elapsed time for VI/VB loop: " << endTime - startTime << " sec"
     << LogIO::POST;

  // Finalization
  finalize_process();
}

void SingleDishMS::atmcor(Record const &config, string const &columnName, string const &outMSName) {
  LogIO os(_ORIGIN);
  os << LogIO::DEBUGGING
     << "Input parameter summary:" << endl
     << "   columnName = " << columnName << endl << "   outMSName = "
     << outMSName << LogIO::POST;

  // Initialization
  doAtmCor_ = true;
  atmCorConfig_ = config;
  os << LogIO::DEBUGGING << "config summry:";
  atmCorConfig_.print(os.output(), 25, "    ");
  os << LogIO::POST;
  Block<Int> sortCols(4);
  sortCols[0] = MS::OBSERVATION_ID;
  sortCols[1] = MS::ARRAY_ID;
  sortCols[2] = MS::FEED1;
  sortCols[3] = MS::DATA_DESC_ID;
  prepare_for_process(columnName, outMSName, sortCols, False);

  // get VI/VB2 access
  vi::VisibilityIterator2 *visIter = sdh_->getVisIter();
  // for parallel processing: set row blocking (common multiple of 3 and 4)
  // TODO: optimize row blocking size
  constexpr rownr_t kNrowBlocking = 360u;
  std::vector<Int> antenna1 = ScalarColumn<Int>(visIter->ms(), "ANTENNA1").getColumn().tovector();
  std::sort(antenna1.begin(), antenna1.end());
  auto const result = std::unique(antenna1.begin(), antenna1.end());
  Int const nAntennas = std::distance(antenna1.begin(), result);
  visIter->setRowBlocking(kNrowBlocking * nAntennas);
  os << "There are " << nAntennas << " antennas in MAIN table. "
     << "Set row-blocking size " << kNrowBlocking * nAntennas
     << LogIO::POST;
  vi::VisBuffer2 *vb = visIter->getVisBuffer();

  double startTime = gettimeofday_sec();

  for (visIter->originChunks(); visIter->moreChunks(); visIter->nextChunk()) {
    for (visIter->origin(); visIter->more(); visIter->next()) {
      sdh_->fillOutputMs(vb);
    }
  }

  double endTime = gettimeofday_sec();
  os << LogIO::DEBUGGING
     << "Elapsed time for VI/VB loop: " << endTime - startTime << " sec"
     << LogIO::POST;

  // Finalization
  finalize_process();
}


bool SingleDishMS::importAsap(string const &infile, string const &outfile, bool const parallel) {
  bool status = true;
  try {
    SingleDishMSFiller<Scantable2MSReader> filler(infile, parallel);
    filler.fill();
    filler.save(outfile);
  } catch (AipsError &e) {
    LogIO os(_ORIGIN);
    os << LogIO::SEVERE << "Exception occurred." << LogIO::POST;
    os << LogIO::SEVERE << "Original Message: \n" << e.getMesg() << LogIO::POST;
    os << LogIO::DEBUGGING << "Detailed Stack Trace: \n" << e.getStackTrace() << LogIO::POST;
    status = false;
  } catch (...) {
    LogIO os(_ORIGIN);
    os << LogIO::SEVERE << "Unknown exception occurred." << LogIO::POST;
    status = false;
  }
  return status;
}

bool SingleDishMS::importNRO(string const &infile, string const &outfile, bool const parallel) {
  bool status = true;
  try {
    SingleDishMSFiller<NRO2MSReader> filler(infile, parallel);
    filler.fill();
    filler.save(outfile);
  } catch (AipsError &e) {
    LogIO os(_ORIGIN);
    os << LogIO::SEVERE << "Exception occurred." << LogIO::POST;
    os << LogIO::SEVERE << "Original Message: \n" << e.getMesg() << LogIO::POST;
    os << LogIO::DEBUGGING << "Detailed Stack Trace: \n" << e.getStackTrace() << LogIO::POST;
    status = false;
  } catch (...) {
    LogIO os(_ORIGIN);
    os << LogIO::SEVERE << "Unknown exception occurred." << LogIO::POST;
    status = false;
  }
  return status;
}

}  // End of casa namespace.
