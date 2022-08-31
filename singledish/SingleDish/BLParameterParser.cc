//# BLParameterParser.cc: this code perses baseline fitting parameters
//# Copyright (C) 2015
//# National Astronomical Observatory of Japan
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
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
//# $Id$
#include <fstream>
#include <iostream>

#include <casacore/casa/Utilities/Assert.h>
#include <singledish/SingleDish/BLParameterParser.h>

using namespace std;

#define _ORIGIN LogOrigin("BLParameterParser", __func__, WHERE)

using namespace casacore;
using namespace casacore;
using namespace casacore;
using namespace casacore;
using namespace casacore;
using namespace casacore;
using namespace casacore;
namespace casa {

BLParameterParser::BLParameterParser(string const file_name)
{
  initialize();
  parse(file_name);
  blparam_file_ = file_name;
}

BLParameterParser::~BLParameterParser()
{
  Clearup();
}

void BLParameterParser::Clearup()
{
  if (!bl_parameters_.empty()) {
    map<const pair<size_t, size_t> , BLParameterSet*>::iterator
      iter = bl_parameters_.begin();
    while (iter != bl_parameters_.end()) {
      delete (*iter).second;
      ++iter;
    }
    bl_parameters_.clear();
  }
}

void BLParameterParser::initialize()
{
  if (!bl_parameters_.empty()) Clearup();
  baseline_types_.resize(0);
  // initialize max orders
  size_t num_type = BaselineType_kNumElements;
  for (size_t i=0; i<num_type; ++i) {
    max_orders_[i] = 0;
  }
}

uint16_t BLParameterParser::get_max_order(size_t const bltype)
{
  
  for (size_t i = 0; i < baseline_types_.size(); ++i) {
    if (bltype == baseline_types_[i]) {
      //bltype is in file
      return max_orders_[bltype];
    }
  }
  // bltype is not in file
  throw(AipsError("The baseline type is not in file."));
}

void BLParameterParser::parse(string const file_name)
{
  LogIO os(_ORIGIN);
  ifstream ifs(file_name.c_str(),ifstream::in);
  AlwaysAssert(!ifs.fail(), AipsError);
  string linestr;
  while (getline(ifs, linestr)) {
    // parse line here to construct BLParameterSet
    // The order should be
    // ROW,POL,MASK,NITERATION,CLIP_THRES,LF,LF_THRES,LEDGE,REDGE,CHANAVG,BL_TYPE,ORDER,N_PIECE,NWAVE
    // size_t,1,string,uint16_t,float,bool,float,size_t,size_t,size_t,sinusoidal,uint16_t,size_t,vector<size_t>
    //skip line starting with '#'
    if (linestr.empty() || linestr[0]=='#') continue;
    BLParameterSet *bl_param = new BLParameterSet();
    size_t row_idx, pol_idx;
    ConvertLineToParam(linestr, row_idx, pol_idx, *bl_param);
    bl_parameters_[make_pair(row_idx, pol_idx)] = bl_param;
    //Parameter summary output (Debugging purpose only)
    if (false) {
      os << "Summary of parsed Parameter" << LogIO::POST;
      os << "[ROW" << row_idx << ", POL" << pol_idx << "]"
	 << LogIO::POST;
      bl_param->PrintSummary();
    }
    // update bealine_types_ list
    size_t curr_type_idx = static_cast<size_t>(bl_param->baseline_type);
    bool new_type = true;
    for (size_t i=0; i<baseline_types_.size(); ++i){
      if (bl_param->baseline_type==baseline_types_[i]){
	new_type = false;
	break;
      }
    }
    if (new_type) baseline_types_.push_back(bl_param->baseline_type);
    // update max_orders_
    size_t curr_order = GetTypeOrder(*bl_param);
    if (curr_order > max_orders_[curr_type_idx])
      max_orders_[curr_type_idx] = curr_order;
  }
}

void BLParameterParser::SplitLine(string const &linestr,
                                  char const separator,
                                  vector<string> &strvec)
{
  istringstream iss(linestr);
  string selem;
  size_t ielem(0);
  while(getline(iss, selem, separator)) {
    if (ielem >= strvec.size()) {
      strvec.resize(ielem+1);
    }
    strvec[ielem] = selem;
    ++ielem;
  }
}

void BLParameterParser::ConvertLineToParam(string const &linestr,
                                           size_t &rowid, size_t &polid,
                                           BLParameterSet &paramset){
  //split a line by ',' and make a vector of strings
  std::vector<string> svec(BLParameters_kNumElements,"");
  SplitLine(linestr,',', svec);
  // parse mandatory data
  rowid = ConvertString<size_t>(svec[BLParameters_kRow]);
  polid = ConvertString<size_t>(svec[BLParameters_kPol]);
  paramset.baseline_mask = svec[BLParameters_kMask];
  size_t num_piece=USHRT_MAX;//SIZE_MAX;
  string const bltype_str = svec[BLParameters_kBaselineType];
  if (bltype_str == "cspline")
  {
    if (svec[BLParameters_kNPiece].size()==0)
      throw(AipsError("Baseline type 'cspline' requires num_piece value."));
    num_piece = ConvertString<size_t>(svec[BLParameters_kNPiece]);
    // CreateBaselineContext does not supoort elements number > max(uint16_t)
    if (num_piece > USHRT_MAX)
      throw(AipsError("num_piece is larger than max of uint16_t"));
    paramset.npiece = num_piece;
    paramset.baseline_type = static_cast<LIBSAKURA_SYMBOL(LSQFitType)>(BaselineType_kCubicSpline);
  }
  else if (bltype_str == "sinusoid")
  {
    // sinusoid is not supported yet
    throw(AipsError("Unsupported baseline type, sinusoid"));
  }
  else
  { // poly or chebyshev
    if (svec[BLParameters_kOrder].size()==0)
      throw(AipsError("Baseline type 'poly' and 'chebyshev' require order value."));
    paramset.baseline_type = bltype_str == "chebyshev" ?
      static_cast<LIBSAKURA_SYMBOL(LSQFitType)>(BaselineType_kChebyshev) :
      static_cast<LIBSAKURA_SYMBOL(LSQFitType)>(BaselineType_kPolynomial);
    paramset.order = ConvertString<uint16_t>(svec[BLParameters_kOrder]);
  }
  // parse clipping parameters
  if (svec[BLParameters_kNumIteration].size() == 0)
    throw(AipsError("Number of maximum clip iteration is mandatory"));
  paramset.num_fitting_max
    = ConvertString<uint16_t>(svec[BLParameters_kNumIteration]) + 1;
  if (svec[BLParameters_kClipThreshold].size()>0)
    paramset.clip_threshold_sigma
      = ConvertString<float>(svec[BLParameters_kClipThreshold]);
  // parse line finder parameter
  LineFinderParameter &lf_param = paramset.line_finder;
  lf_param.use_line_finder = svec[BLParameters_kLineFinder]=="true" ? true : false;;
  if (lf_param.use_line_finder)
  { // use line finder
    if (svec[BLParameters_kLFThreshold].size()>0)
    {
      lf_param.threshold
	= ConvertString<float>(svec[BLParameters_kLFThreshold]);
    }
    vector<size_t> edge(2,0);
    if (svec[BLParameters_kLeftEdge].size() > 0)
      lf_param.edge[0] = ConvertString<size_t>(svec[BLParameters_kLeftEdge]);
    if (svec[BLParameters_kRightEdge].size() > 0)
      lf_param.edge[1] = ConvertString<size_t>(svec[BLParameters_kRightEdge]);
    if (svec[BLParameters_kChanAverageLim].size()>0)
    {
      lf_param.chan_avg_limit
	= ConvertString<size_t>(svec[BLParameters_kChanAverageLim]);
    }
  }
}

uint16_t BLParameterParser::GetTypeOrder(BLParameterSet const &bl_param)
{
  size_t const type = static_cast<size_t>(bl_param.baseline_type);
  switch (type)
  {
  case BaselineType_kPolynomial:
  case BaselineType_kChebyshev:
    return bl_param.order;
    break;
  case BaselineType_kCubicSpline:
    AlwaysAssert(bl_param.npiece<=USHRT_MAX, AipsError);//UINT16_MAX);
    return static_cast<uint16_t>(bl_param.npiece);
    break;
//   case BaselineType_kSinusoidal:
//     return static_cast<size_t>(bl_param.nwave.size()); <== must be max of nwave elements
//     break;
  default:
    throw(AipsError("Unsupported baseline type."));
  }
}

bool BLParameterParser::GetFitParameter(size_t const rowid,size_t const polid, BLParameterSet &bl_param)
{
  map<const pair<size_t, size_t> ,BLParameterSet*>::iterator
    iter = bl_parameters_.begin();
  iter = bl_parameters_.find(make_pair(rowid, polid));
  if (iter==bl_parameters_.end()) {
    // no matching element
    return false;
  }
  bl_param = *(*iter).second;
  return true;
}

BLTableParser::BLTableParser(string const file_name) : BLParameterParser(file_name)
{
  initialize();
  blparam_file_ = file_name;
  bt_ = new BaselineTable(file_name);
  parse();
}

BLTableParser::~BLTableParser()
{
  delete bt_;
  bt_ = 0;
}

void BLTableParser::initialize()
{
  baseline_types_.resize(0);
  // initialize max orders
  size_t num_type = BaselineType_kNumElements;
  for (size_t i = 0; i < num_type; ++i) {
    max_orders_[i] = 0;
  }
}

uint16_t BLTableParser::GetTypeOrder(size_t const &baseline_type, 
                                     uInt const irow, uInt const ipol)
{
  switch (baseline_type)
  {
  case BaselineType_kPolynomial:
  case BaselineType_kChebyshev:
    {
      return static_cast<uint16_t>(bt_->getFPar(irow, ipol));
      break;
    }
  case BaselineType_kCubicSpline:
    {
      uInt npiece = bt_->getFPar(irow, ipol);
      AlwaysAssert(npiece <= USHRT_MAX, AipsError);//UINT16_MAX);
      return static_cast<uint16_t>(npiece);
      break;
    }
//   case BaselineType_kSinusoidal:
//     return static_cast<size_t>(nwave.size());
//     break;
  default:
    throw(AipsError("Unsupported baseline type."));
  }
}

void BLTableParser::parse()
{
  uInt const npol = 2;
  size_t nrow = bt_->nrow();
  std::map<string, std::map<double, uInt> > sorted;
  for (uInt irow = 0; irow < nrow; ++irow) {
    stringstream ss;
    ss << bt_->getSpw(irow) << "," 
       << bt_->getAntenna(irow) << "," 
       << bt_->getBeam(irow);
    sorted[ss.str()][bt_->getTime(irow)] = irow;
    for (uInt ipol = 0; ipol < npol; ++ipol) {
      if (!bt_->getApply(irow, ipol)) continue;
      LIBSAKURA_SYMBOL(LSQFitType) curr_type_idx = 
	static_cast<LIBSAKURA_SYMBOL(LSQFitType)>(bt_->getBaselineType(irow, ipol));
      bool new_type = true;
      for (size_t i = 0; i < baseline_types_.size(); ++i){
	if (curr_type_idx == baseline_types_[i]){
	  new_type = false;
	  break;
	}
      }
      if (new_type) baseline_types_.push_back(curr_type_idx);
      // update max_orders_
      size_t curr_order = GetTypeOrder(curr_type_idx, irow, ipol);
      if (curr_order > max_orders_[curr_type_idx]) {
	max_orders_[curr_type_idx] = curr_order;
      }
    }
  }

  for (std::map<string, std::map<double, uInt> >::iterator it = sorted.begin(); it != sorted.end(); ++it) {
    sortedTimes_[it->first].clear();
    timeSortedIdx_[it->first].clear();
    numRows_[it->first] = 0;
    size_t num_rows = 0;
    for (std::map<double, uInt>::iterator it0 = sorted[it->first].begin(); it0 != sorted[it->first].end(); ++it0) {
      sortedTimes_[it->first].push_back(it0->first);
      timeSortedIdx_[it->first].push_back(it0->second);
      num_rows++;
    }
    numRows_[it->first] = num_rows;
  }
}

bool BLTableParser::GetFitParameterIdx(double const time, double const interval, 
                                       size_t const scanid, size_t const beamid, 
                                       size_t const antid, size_t const spwid, 
                                       size_t &idx)
{
  bool found = false;
  stringstream ss;
  ss << spwid << "," << antid << "," << beamid;
  string key = ss.str();
  if (numRows_.count(key) == 0) {
    return false;
  }
  uInt idx_top = 0;
  uInt idx_end = numRows_[key] - 1;
  uInt idx_mid = (idx_top + idx_end)/2;
  double time_top = sortedTimes_[key][idx_top];
  double time_end = sortedTimes_[key][idx_end];
  double time_mid = sortedTimes_[key][idx_mid];
  if ((time < time_top)||(time_end < time)) { // out of range.
    return false;
  }
  size_t idx0 = 0;
  //binary search in time-sorted table.
  while (idx_end - idx_top > 1) {
    if (time_top == time) {
      idx0 = idx_top;
      found = true;
      break;
    } else if (time_mid == time) {
      idx0 = idx_mid;
      found = true;
      break;
    } else if (time_end == time) {
      idx0 = idx_end;
      found = true;
      break;
    } else if (time_mid < time) {
      idx_top = idx_mid;
      time_top = time_mid;
    } else {
      idx_end = idx_mid;
      time_end = time_mid;
    }
    idx_mid = (idx_top + idx_end)/2;
    time_mid = sortedTimes_[key][idx_mid];
  }
  if (!found) {
    //only one or two candidates should exist now. 
    //check if there is one with time identical to the specified time
    for (size_t i = idx_top; i <= idx_end; ++i) {
      if ((time - interval < sortedTimes_[key][i])&&
(sortedTimes_[key][i] < time + interval)) {
	idx0 = i;
	found = true;
	break;
      }
    }
    if (!found) {
      return false;
    }
  }
  //finally, validate the candidate using its ids.
  idx = timeSortedIdx_[key][idx0];
  found = (scanid == bt_->getScan(idx))&&
          (beamid == bt_->getBeam(idx))&&
          (spwid  == bt_->getSpw(idx));

  return found;
}

/*
//simply search from the first line. not using binary search.
bool BLTableParser::GetFitParameterIdx(double const time, double const interval, 
				       size_t const scanid, size_t const beamid, 
				       size_t const spwid, size_t &idx)
{
  bool found = false;
  uInt idx_end = bt_->nrow() - 1;
  double bt_time;
  uInt bt_scanid;
  uInt bt_beamid;
  uInt bt_spwid;
  for (uInt i = 0; i <= idx_end; ++i) {
    bt_time = bt_->getTime(i);
    bt_scanid = bt_->getScan(i);
    bt_beamid = bt_->getBeam(i);
    bt_spwid = bt_->getSpw(i);
    if ((bt_time-interval < time) && (time < bt_time+interval) &&
	(scanid == bt_scanid) && (beamid == bt_beamid) &&
	(spwid == bt_spwid)) {
      idx = i;
      found = true;
      break;
    }
  }
  return found;
}
*/

void BLTableParser::GetFitParameterByIdx(size_t const idx, size_t const ipol, 
                                         bool &apply,
                                         Vector<double> &coeff,
                                         Vector<size_t> &boundary,
                                         std::vector<bool> &masklist,
                                         BLParameterSet &bl_param)
{
  apply = bt_->getApply(idx, ipol);
  if (!apply) return;
  size_t const type = bt_->getBaselineType(idx, ipol);
  bl_param.baseline_type = type;
  Vector<Int> fpar(bt_->getFuncParam(idx)[0]);
  switch (type) {
  case BaselineType_kPolynomial:
  case BaselineType_kChebyshev:
    bl_param.order = fpar[ipol];
    coeff.resize(bl_param.order + 1);
    for (size_t i = 0; i < coeff.size(); ++i) {
      coeff[i] = bt_->getResult(idx).row(ipol)[i];
    }
    break;
  case BaselineType_kCubicSpline:
    bl_param.npiece = fpar[ipol];
    boundary.resize(bl_param.npiece + 1);
    for (size_t i = 0; i < boundary.size(); ++i) {
      boundary[i] = bt_->getFuncFParam(idx).row(ipol)[i];
    }
    coeff.resize(bl_param.npiece * 4);
    for (size_t i = 0; i < coeff.size(); ++i) {
      coeff[i] = bt_->getResult(idx).row(ipol)[i];
    }
    break;
  default:
    throw(AipsError("Unsupported baseline type."));
  }
  masklist = bt_->getMask(idx, ipol);
}


} //# NAMESPACE CASA - END
