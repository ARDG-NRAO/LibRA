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
#include <assert.h>

#include <casacore/casa/Containers/ValueHolder.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/OS/Path.h>
#include <casacore/measures/TableMeasures/TableMeasDesc.h>
#include <casacore/measures/TableMeasures/TableMeasRefDesc.h>
#include <casacore/measures/TableMeasures/TableMeasValueDesc.h>
#include <singledish/SingleDish/BaselineTable.h>
#include <stdcasa/StdCasa/CasacSupport.h>
#include <casacore/tables/Tables/TableDesc.h>
#include <casacore/tables/Tables/SetupNewTab.h>
#include <casacore/tables/Tables/ArrColDesc.h>
#include <casacore/tables/Tables/ScaColDesc.h>
#include <casacore/tables/Tables/TableRecord.h>
#include <casacore/tables/Tables/TableProxy.h>

using namespace casacore;
using namespace casacore;
using namespace casacore;
using namespace casacore;
using namespace casacore;
using namespace casacore;
using namespace casacore;
namespace casa {

const String BaselineTable::name_ = "APPLY_BASELINE";

BaselineTable::BaselineTable(const MeasurementSet& parent)
{
  TableDesc td("", "1", TableDesc::Scratch);
  td.addColumn(ScalarColumnDesc<uInt>("SCANNO"));
  td.addColumn(ScalarColumnDesc<uInt>("BEAMNO"));
  td.addColumn(ScalarColumnDesc<uInt>("ANTNO"));
  td.addColumn(ScalarColumnDesc<uInt>("IFNO"));
  td.addColumn(ScalarColumnDesc<uInt>("FREQ_ID"));
  td.addColumn(ScalarColumnDesc<Double>("TIME"));
  TableMeasRefDesc measRef(MEpoch::UTC); // UTC as default
  TableMeasValueDesc measVal(td, "TIME");
  TableMeasDesc<MEpoch> mepochCol(measVal, measRef);
  mepochCol.write(td);
  String tabname = parent.tableName()+"/"+name_;
  SetupNewTable aNewTab(tabname, td, Table::Scratch);
  table_ = Table(aNewTab, Table::Memory);
  attachBaseColumns();

  table_.rwKeywordSet().define("VERSION", 1);
  table_.rwKeywordSet().define("MSName", parent.tableName());
  table_.rwKeywordSet().define("ApplyType", "NONE");
  table_.rwKeywordSet().defineTable("FREQUENCIES", parent.spectralWindow());
  table_.tableInfo().setType("ApplyTable");
  originaltable_ = table_;

  setup();
}

BaselineTable::BaselineTable(const String &name)
{
  table_ = Table(name, Table::Update);
  attachBaseColumns();
  originaltable_ = table_;
  attachOptionalColumns();
}

BaselineTable::~BaselineTable()
{
}

void BaselineTable::attach()
{
  attachBaseColumns();
  attachOptionalColumns();
}

void BaselineTable::attachBaseColumns()
{
  scanCol_.attach(table_, "SCANNO");
  beamCol_.attach(table_, "BEAMNO");
  antCol_.attach(table_, "ANTNO");
  ifCol_.attach(table_, "IFNO");
  timeCol_.attach(table_, "TIME");
  timeMeasCol_.attach(table_, "TIME");
  freqidCol_.attach(table_, "FREQ_ID");
}
void BaselineTable::setup()
{
  table_.addColumn(ArrayColumnDesc<Bool>("APPLY"));
  table_.addColumn(ArrayColumnDesc<uInt>("FUNC_TYPE"));
  table_.addColumn(ArrayColumnDesc<Int>("FUNC_PARAM"));
  table_.addColumn(ArrayColumnDesc<Float>("FUNC_FPARAM"));
  table_.addColumn(ArrayColumnDesc<uInt>("MASKLIST"));
  table_.addColumn(ArrayColumnDesc<Float>("RESULT"));
  table_.addColumn(ArrayColumnDesc<Float>("RMS"));
  table_.addColumn(ScalarColumnDesc<uInt>("NCHAN"));
  table_.addColumn(ArrayColumnDesc<Float>("CLIP_THRESHOLD"));
  table_.addColumn(ArrayColumnDesc<uInt>("CLIP_ITERATION"));
  table_.addColumn(ArrayColumnDesc<Bool>("USE_LF"));
  table_.addColumn(ArrayColumnDesc<Float>("LF_THRESHOLD"));
  table_.addColumn(ArrayColumnDesc<uInt>("LF_AVERAGE"));
  table_.addColumn(ArrayColumnDesc<uInt>("LF_EDGE"));

  table_.rwKeywordSet().define("ApplyType", "BASELINE");

  attachOptionalColumns();
}

void BaselineTable::attachOptionalColumns()
{
  applyCol_.attach(table_, "APPLY");
  ftypeCol_.attach(table_, "FUNC_TYPE");
  fparCol_.attach(table_, "FUNC_PARAM");
  ffparCol_.attach(table_, "FUNC_FPARAM");
  maskCol_.attach(table_, "MASKLIST");
  resCol_.attach(table_, "RESULT");
  rmsCol_.attach(table_, "RMS");
  nchanCol_.attach(table_, "NCHAN");
  cthresCol_.attach(table_, "CLIP_THRESHOLD");
  citerCol_.attach(table_, "CLIP_ITERATION");
  uselfCol_.attach(table_, "USE_LF");
  lfthresCol_.attach(table_, "LF_THRESHOLD");
  lfavgCol_.attach(table_, "LF_AVERAGE");
  lfedgeCol_.attach(table_, "LF_EDGE");

  ftypeCol_.rwKeywordSet().define("Polynomial",   BaselineType_kPolynomial);
  ftypeCol_.rwKeywordSet().define("Chebyshev",    BaselineType_kChebyshev);
  ftypeCol_.rwKeywordSet().define("Cubic Spline", BaselineType_kCubicSpline);
  ftypeCol_.rwKeywordSet().define("Sinusoid",     BaselineType_kSinusoid);
}

void BaselineTable::save(const std::string &filename)
{
  String inname(filename);
  Path path(inname);
  inname = path.expandedName();
  table_.deepCopy(inname, Table::New);
}

bool BaselineTable::getApply(uInt irow, uInt ipol) const
{
  Vector<Bool> apply = applyCol_.get(irow);
  return static_cast<bool>(apply[ipol]);
}

std::vector<bool> BaselineTable::getMask(uInt irow, uInt ipol)
{
  uInt nchan = nchanCol_.get(irow);
  Matrix<uInt> masklist = maskCol_.get(irow);
  return getMaskFromMaskList(nchan, masklist.row(ipol));
}

uint BaselineTable::getBaselineType(uInt irow, uInt ipol) const
{
  Vector<uInt> ftype = ftypeCol_.get(irow);
  return static_cast<uint>(ftype[ipol]);
}

int BaselineTable::getFPar(uInt irow, uInt ipol) const
{
  Vector<Int> fpar = fparCol_.get(irow);
  return static_cast<int>(fpar[ipol]);
}

void BaselineTable::setbasedata(uInt irow, uInt scanno, uInt beamno, uInt antno, 
				uInt ifno, uInt freqid, Double time)
{
  scanCol_.put(irow, scanno);
  beamCol_.put(irow, beamno);
  antCol_.put(irow, antno);
  ifCol_.put(irow, ifno);
  timeCol_.put(irow, time);
  freqidCol_.put(irow, freqid);
}

void BaselineTable::setdata(uInt irow, uInt scanno, 
			    uInt beamno, uInt antno, uInt ifno, 
			    uInt freqid, Double time, 
			    Array<Bool> apply, 
		            Array<uInt> ftype,
		            Array<Int> fpar, 
		            Array<Float> ffpar, 
		            Array<uInt> mask,
			    Array<Float> res,
		            Array<Float> rms, 
		            uInt nchan, 
		            Array<Float> cthres,
		            Array<uInt> citer, 
			    Array<Bool> uself, 
		            Array<Float> lfthres, 
		            Array<uInt> lfavg, 
		            Array<uInt> lfedge)
{
  if (irow >= (uInt)nrow()) {
    stringstream ss;
    ss << "row index out of range[irow=" << irow << "][nrow=" << nrow() << "]";
    throw AipsError(ss.str());
  }

  setbasedata(irow, scanno, beamno, antno, ifno, freqid, time);
  applyCol_.put(irow, apply);
  ftypeCol_.put(irow, ftype);
  fparCol_.put(irow, fpar);
  ffparCol_.put(irow, ffpar);
  maskCol_.put(irow, mask);
  resCol_.put(irow, res);
  rmsCol_.put(irow, rms);
  nchanCol_.put(irow, nchan);
  cthresCol_.put(irow, cthres);
  citerCol_.put(irow, citer);
  uselfCol_.put(irow, uself);
  lfthresCol_.put(irow, lfthres);
  lfavgCol_.put(irow, lfavg);
  lfedgeCol_.put(irow, lfedge);
}

void BaselineTable::appenddata(uInt scanno, uInt beamno, 
			       uInt antno, uInt ifno, 
			       uInt freqid, Double time, 
			       Array<Bool> apply, 
			       Array<uInt> ftype, 
			       Array<Int> fpar, 
			       Array<Float> ffpar, 
			       Array<uInt> mask,
			       Array<Float> res,
			       Array<Float> rms,
			       uInt nchan, 
			       Array<Float> cthres,
			       Array<uInt> citer, 
			       Array<Bool> uself, 
			       Array<Float> lfthres, 
			       Array<uInt> lfavg, 
			       Array<uInt> lfedge)
{
  uInt irow = nrow();
  table_.addRow(1, true);
  setdata(irow, scanno, beamno, antno, ifno, freqid, time, 
	  apply, ftype, fpar, ffpar, mask, res, rms, 
	  nchan, cthres, citer, uself, lfthres, lfavg, lfedge);
}

void BaselineTable::appendbasedata(int scanno, int beamno, 
				   int antno, int ifno, 
				   int freqid, Double time)
{
  uInt irow = nrow();
  table_.addRow(1, true);
  setbasedata(irow, uInt(scanno), uInt(beamno), uInt(antno), 
	      uInt(ifno), uInt(freqid), time);
}

void BaselineTable::setresult(uInt irow, 
			      Vector<Float> res, 
			      Array<Float> rms)
{
  resCol_.put(irow, res);
  rmsCol_.put(irow, rms);
}

uInt BaselineTable::getNChan(int irow)
{
  return nchanCol_.get(irow);
}

uInt BaselineTable::nchan(uInt ifno)
{
  uInt tmp = ifno;
  return tmp;
}

std::vector<bool> BaselineTable::getMaskFromMaskList(uInt const nchan, Vector<uInt> const& masklist)
{
  if (masklist.size() % 2 != 0) {
    throw(AipsError("masklist must have even number of elements."));
  }

  std::vector<bool> res((int)nchan, false);

  for (uInt j = 0; j < masklist.size(); j += 2) {
    for (int i = masklist[j]; i <= min((Int)nchan-1, (Int)masklist[j+1]); ++i) {
      res[i] = true;
    }
    if (masklist[j+1] == nchan-1) {
      break;
    }
  }

  return res;
}
}
