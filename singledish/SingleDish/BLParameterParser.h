//# BLParameterParser.h: this code perses baseline fitting parameters
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
#ifndef _CASA_BLPARAMETER_PARSER_H_
#define _CASA_BLPARAMETER_PARSER_H_

#include <string>
#include <map>
#include <vector>
//#include<cstdint> //with c++11 support
#include <climits>
#include <sstream>

#include <casa/aipstype.h>
#include <casa/Arrays/Vector.h>
#include <casa/Logging/LogIO.h>
#include <casa/Logging/LogOrigin.h>
#include <libsakura/sakura.h>
#include <singledish/SingleDish/BaselineTable.h>

namespace casa { //# NAMESPACE CASA - BEGIN

struct LineFinderParameter {
  LineFinderParameter(bool const use_lf=false, float const thresh=5.0,
                      std::vector<size_t> const &edges=std::vector<size_t>(2,0),
                      size_t const chavglim=0)
  {
    use_line_finder=use_lf;
    threshold = thresh;
    chan_avg_limit = chavglim;
    for (size_t iedge=0; iedge < 2; ++ iedge) {
      edge[iedge] = edges[iedge % edges.size()] ;
    }
  }
  bool use_line_finder;
  float threshold;
  size_t edge[2];
  size_t chan_avg_limit;
};

struct BLParameterSet {
  BLParameterSet(string const blmask="", uint16_t const nfit_max=0,
                 float const clipthres=3.0,
                 LineFinderParameter lf_param=LineFinderParameter(),
                 size_t const bl_type = BaselineType_kNumElements,
                 uint16_t const fit_order = USHRT_MAX, //UINT16_MAX,
                 size_t const num_piece = USHRT_MAX, //SIZE_MAX,
                 std::vector<size_t> const &nwaves = std::vector<size_t>()
                 )
  {
    baseline_mask = blmask;
    num_fitting_max = nfit_max;
    clip_threshold_sigma = clipthres;
    line_finder = lf_param;
    baseline_type = bl_type;
    order = fit_order;
    npiece = num_piece;
    nwave = nwaves;
  }

  void PrintSummary() {
    casacore::LogIO os(casacore::LogOrigin("BLParameterSet","PrintSummary"));
    os << "- mask: " << baseline_mask << casacore::LogIO::POST;
    os << "- clip: iteration=" << num_fitting_max
       << ", threshold=" << clip_threshold_sigma << casacore::LogIO::POST;
    os 
      << "- line finder: "
      << (line_finder.use_line_finder==1 ? "true" : "false")
      << ", threshold=" << line_finder.threshold
      << ", edge=[" << line_finder.edge[0] << ","
      << line_finder.edge[1] << "], chan_average="
      << line_finder.chan_avg_limit << casacore::LogIO::POST;
    os << "- baseline: type=" << baseline_type
       << ", order=" << order << ", npiece=" << npiece
       << casacore::LogIO::POST;
  }

  string baseline_mask;
  uint16_t num_fitting_max;
  float clip_threshold_sigma;
  LineFinderParameter line_finder;
  size_t baseline_type;
  uint16_t order; //initialize with invalid parameters
  size_t npiece;
  std::vector<size_t> nwave;
};


class BLParameterParser {
public:

  explicit BLParameterParser(string const file_name);
  ~BLParameterParser();

  //Returns false if there is no fitting parameters for the row and pol.
  //Else, returns a baseline fitting parameter of a certain
  //row and pol IDs in MS
  bool GetFitParameter(size_t const rowid,size_t const polid,
                       BLParameterSet &bl_param);

  //Returns the name of file that stores 
  inline string get_file_name(){return blparam_file_;};
  //Returns a list of baseline type in the file
  inline std::vector<size_t> get_function_types(){return baseline_types_;};
  //Returns the maximum fit order of specified baseline type to construct BaselineContext
  uint16_t get_max_order(size_t const bltype);

protected:
  //private:
  void initialize();
  void Clearup();
  // parse a file
  void parse(string const file_name);
  // split string by separator character
  void SplitLine(string const &linestr, char const separator,
                 std::vector<string> &strvec);
  // convert a line of string to a BLParameterSet data structure
  void ConvertLineToParam(string const &linestr, size_t &rowid,
                          size_t &polid, BLParameterSet &paramset);
  //Returns order or npiece in BLParameterSet structure depending on datatype.
  uint16_t GetTypeOrder(BLParameterSet const &bl_param);

  // Member variables
  string blparam_file_;
  std::map<const std::pair<size_t, size_t>, BLParameterSet*> bl_parameters_;
  std::vector<size_t> baseline_types_;
  uint16_t max_orders_[BaselineType_kNumElements];
  // The enum for columns in fitting parameter file
  typedef enum {BLParameters_kRow = 0,
                BLParameters_kPol,
                BLParameters_kMask,
                BLParameters_kNumIteration,
                BLParameters_kClipThreshold,
                BLParameters_kLineFinder,
                BLParameters_kLFThreshold,
                BLParameters_kLeftEdge,
                BLParameters_kRightEdge,
                BLParameters_kChanAverageLim,
                BLParameters_kBaselineType,
                BLParameters_kOrder,
                BLParameters_kNPiece,
                BLParameters_kNWave,
                BLParameters_kNumElements
  } BLParameters;

  //
  template<typename DataType>
  inline DataType ConvertString(string const &svalue)
  {
    DataType out_value;
    std::istringstream istr(svalue.data());
    istr >> out_value;
    return out_value;
  };

}; // class BLParameterParser -END

struct BLIdSet {};

class BLTableParser : public BLParameterParser {
 public:
  explicit BLTableParser(string const file_name);
  ~BLTableParser();
  bool GetFitParameterIdx(double const time, double const interval, 
                          size_t const scanid, size_t const beamid, 
                          size_t const antid, size_t const spwid, size_t &idx);
  void GetFitParameterByIdx(size_t const idx, size_t const ipol, 
                            bool &apply,
                            casacore::Vector<double> &coeff,
                            casacore::Vector<size_t> &boundary,
                            std::vector<bool> &masklist,
                            BLParameterSet &bl_param);
 private:
  void initialize();
  void parse();
  //Returns order or npiece in BLParameterSet structure depending on datatype.
  uint16_t GetTypeOrder(size_t const &baseline_type, 
                        casacore::uInt const irow, casacore::uInt const ipol);
  BaselineTable *bt_;
  std::map<string, std::vector<double> > sortedTimes_;
  std::map<string, std::vector<casacore::uInt> > timeSortedIdx_;
  std::map<string, size_t> numRows_;
};

} //# NAMESPACE CASA - END
  
#endif /* _CASA_BLPARAMETER_PARSER_H_ */
