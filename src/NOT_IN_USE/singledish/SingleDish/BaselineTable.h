//# BaselineTable.cc: this code defines baseline table
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
#ifndef SD_BASELINETABLE_H
#define SD_BASELINETABLE_H

#include <casacore/casa/aips.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/measures/Measures/MEpoch.h>
#include <casacore/measures/TableMeasures/ScalarMeasColumn.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/tables/Tables/Table.h>
#include <casacore/tables/Tables/ScalarColumn.h>
#include <casacore/tables/Tables/ArrayColumn.h>
#include <libsakura/sakura.h>

#define BaselineType_kPolynomial     0
#define BaselineType_kChebyshev      1
#define BaselineType_kCubicSpline    2
#define BaselineType_kSinusoid       3
#define BaselineType_kNumElements    4

namespace casa {

class BaselineTable {
public:
  BaselineTable() {;}
  BaselineTable(const casacore::MeasurementSet& parent);
  BaselineTable(const casacore::String &name);

  virtual ~BaselineTable();

  void setup();
  const casacore::String& name() const {return name_;};

  const casacore::Table& table() const { return table_; }
  casacore::Table table() { return table_; }
  void attach();
  void attachBaseColumns();
  void attachOptionalColumns();

  casacore::uInt nrow() {return table_.nrow();}

  casacore::uInt getScan(casacore::uInt irow) const {return scanCol_.get(irow);}
  casacore::uInt getBeam(casacore::uInt irow) const {return beamCol_.get(irow);}
  casacore::uInt getAntenna(casacore::uInt irow) const {return antCol_.get(irow);}
  uint getSpw(casacore::uInt irow) const {return static_cast<uint>(ifCol_.get(irow));}
  double getTime(casacore::uInt irow) const {return static_cast<double>(timeCol_.get(irow));}

  bool getApply(casacore::uInt irow, casacore::uInt ipol) const;
  std::vector<bool> getMask(casacore::uInt irow, casacore::uInt ipol);
  uint getBaselineType(casacore::uInt irow, casacore::uInt ipol) const;
  int getFPar(casacore::uInt irow, casacore::uInt ipol) const;

  void setdata(casacore::uInt irow, casacore::uInt scanno, casacore::uInt beamno, 
	       casacore::uInt antno, casacore::uInt ifno, 
               casacore::uInt freqid, casacore::Double time, 
	       casacore::Array<casacore::Bool> apply,
               casacore::Array<casacore::uInt> ftype, 
	       casacore::Array<casacore::Int> fpar, 
	       casacore::Array<casacore::Float> ffpar, 
               casacore::Array<casacore::uInt> mask,
	       casacore::Array<casacore::Float> res,
               casacore::Array<casacore::Float> rms, 
               casacore::uInt nchan, 
	       casacore::Array<casacore::Float> cthres,
               casacore::Array<casacore::uInt> citer, 
	       casacore::Array<casacore::Bool> uself,
	       casacore::Array<casacore::Float> lfthres, 
	       casacore::Array<casacore::uInt> lfavg, 
	       casacore::Array<casacore::uInt> lfedge);
  void appenddata(casacore::uInt scanno, casacore::uInt beamno, 
		  casacore::uInt antno, casacore::uInt ifno, 
                  casacore::uInt freqid, casacore::Double time, 
		  casacore::Array<casacore::Bool> apply,
		  casacore::Array<casacore::uInt> ftype, 
		  casacore::Array<casacore::Int> fpar, 
		  casacore::Array<casacore::Float> ffpar, 
		  casacore::Array<casacore::uInt> mask,
		  casacore::Array<casacore::Float> res,
		  casacore::Array<casacore::Float> rms, 
		  casacore::uInt nchan, 
		  casacore::Array<casacore::Float> cthres,
		  casacore::Array<casacore::uInt> citer, 
		  casacore::Array<casacore::Bool> uself,
		  casacore::Array<casacore::Float> lfthres, 
		  casacore::Array<casacore::uInt> lfavg, 
		  casacore::Array<casacore::uInt> lfedge);
  void appendbasedata(int scanno, int beamno, int antno, int ifno, 
		      int freqid, casacore::Double time);
  void setresult(casacore::uInt irow, 
		 casacore::Vector<casacore::Float> res, 
		 casacore::Array<casacore::Float> rms);
  void save(const std::string &filename);
  casacore::uInt nchan(casacore::uInt ifno);

  casacore::Matrix<casacore::Bool> getApply() {return applyCol_.getColumn();}
  void setApply(int irow, int ipol, bool apply);
  casacore::Matrix<casacore::Int> getFuncParam() {return fparCol_.getColumn();}
  casacore::Matrix<casacore::Int> getFuncParam(casacore::uInt irow) {return fparCol_.get(irow);}
  casacore::Matrix<casacore::Float> getFuncFParam() {return ffparCol_.getColumn();}
  casacore::Matrix<casacore::Float> getFuncFParam(casacore::uInt irow) {return ffparCol_.get(irow);}
  casacore::Matrix<casacore::uInt> getMaskList() {return maskCol_.getColumn();}
  casacore::Matrix<casacore::uInt> getMaskList(casacore::uInt irow) {return maskCol_.get(irow);}
  casacore::Matrix<casacore::Float> getResult() {return resCol_.getColumn();}
  casacore::Matrix<casacore::Float> getResult(casacore::uInt irow) {return resCol_.get(irow);}
  casacore::Matrix<casacore::Float> getRms() {return rmsCol_.getColumn();}
  casacore::Vector<casacore::uInt> getNChan() {return nchanCol_.getColumn();}
  casacore::uInt getNChan(int irow);
  casacore::Matrix<casacore::Float> getClipThreshold() {return cthresCol_.getColumn();}
  casacore::Matrix<casacore::uInt> getClipIteration() {return citerCol_.getColumn();}
  casacore::Matrix<casacore::Bool> getUseLineFinder() {return uselfCol_.getColumn();}
  casacore::Matrix<casacore::Float> getLineFinderThreshold() {return lfthresCol_.getColumn();}
  casacore::Matrix<casacore::uInt> getLineFinderChanAvg() {return lfavgCol_.getColumn();}
  casacore::Matrix<casacore::uInt> getLineFinderEdge() {return lfedgeCol_.getColumn();}

private:
  void setbasedata(casacore::uInt irow, casacore::uInt scanno, casacore::uInt beamno, casacore::uInt antno, 
		   casacore::uInt ifno, casacore::uInt freqid, casacore::Double time);
  casacore::Table table_, originaltable_;
  casacore::ScalarColumn<casacore::uInt> scanCol_, beamCol_, antCol_, ifCol_, freqidCol_;
  casacore::ScalarColumn<casacore::Double> timeCol_;
  casacore::MEpoch::ScalarColumn timeMeasCol_;
  static const casacore::String name_;

  casacore::ArrayColumn<casacore::Bool> applyCol_;
  casacore::ArrayColumn<casacore::uInt> ftypeCol_;
  casacore::ArrayColumn<casacore::Int> fparCol_;
  casacore::ArrayColumn<casacore::Float> ffparCol_;
  casacore::ArrayColumn<casacore::uInt> maskCol_;
  casacore::ArrayColumn<casacore::Float> resCol_;
  casacore::ArrayColumn<casacore::Float> rmsCol_;
  casacore::ScalarColumn<casacore::uInt> nchanCol_;
  casacore::ArrayColumn<casacore::Float> cthresCol_;
  casacore::ArrayColumn<casacore::uInt> citerCol_;
  casacore::ArrayColumn<casacore::Bool> uselfCol_;
  casacore::ArrayColumn<casacore::Float> lfthresCol_;
  casacore::ArrayColumn<casacore::uInt> lfavgCol_;
  casacore::ArrayColumn<casacore::uInt> lfedgeCol_;

  std::vector<bool> getMaskFromMaskList(casacore::uInt const nchan, casacore::Vector<casacore::uInt> const& masklist);
};

}

#endif
