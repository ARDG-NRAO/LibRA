//# tLineFinder.cc: this defines tests of SingleDishMS
//#
//# Copyright (C) 2014,2015
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
#include <iostream>
#include <sstream>
#include <list>
#include <cassert>


#include <casacore/casa/Arrays/Vector.h>
#include <casacore/tables/Tables/Table.h>
#include <casacore/tables/Tables/ArrayColumn.h>
#include <casacore/tables/Tables/ScalarColumn.h>

#include <singledish/SingleDish/LineFindingUtils.h>
#include <singledish/SingleDish/LineFinder.h>

using namespace casacore;
using namespace casa;
using namespace std;

template <typename DataType>
string print_array(size_t const num_data, DataType const* data) {
  cout << "got " << num_data << " elements" << endl;
  ostringstream oss;
  oss << "[";
  if (num_data > 0)
    oss << data[0] ;
  for (size_t i=1; i<num_data; ++i) 
    oss << ", " << data[i];
  oss << "]";
  return oss.str();
}

void print_line(list<pair<size_t,size_t>> &line_list) {
  cout << "Number of Lines: " << line_list.size() << endl;
  for (list<pair<size_t,size_t>>::iterator iter = line_list.begin();
       iter!=line_list.end(); ++iter) {
    cout << "- [ " << (*iter).first << ", " << (*iter).second << " ]" << endl;
  }
}

int main(int argc, char *argv[]) {
  bool dobin(true), domask(true), doline(true), domaskline(true), dolinefinder(false);
  if (dobin)
  {// Test binning
    cout << "\n**********\nTest binning\n**********" << endl;
    size_t const num_data=20;
    float data[num_data];
    bool mask[num_data];
    for (size_t i = 0; i<num_data; ++i) {
      data[i] = static_cast<float>(i);
      mask[i] = (i%4 != 0) ? true : false;
    }
    cout << "data = " << print_array<float>(num_data, data) << endl;
    cout << "mask = " << print_array<bool>(num_data, mask) << endl;
    size_t const bin_size = 3;
    size_t offset = 1;
    size_t num_out = (num_data-offset)/bin_size;
    bool keepsize=false;
    float out_data1[num_out];
    bool out_mask1[num_out];
    cout << "[Binning]" << endl;
    cout << "Bin = " << bin_size << ", offset = " << offset
	 << ", keepsize = " << (keepsize ? "true" : "false") << endl;
    size_t nout1 = LineFinderUtils::binDataAndMask<float>(num_data, data, mask, bin_size, num_out, out_data1, out_mask1, offset, keepsize);
    cout << "returned" << endl;
    cout << "out_data = " << print_array<float>(nout1, out_data1) << endl;
    cout << "out_mask = " << print_array<bool>(nout1, out_mask1) << endl;
    keepsize = true;
    offset = 0;
    float out_data2[num_data];
    bool out_mask2[num_data];
    cout << "\n[keepsize]" << endl;
    cout << "Bin = " << bin_size << ", offset = " << offset
	 << ", keepsize = " << (keepsize ? "true" : "false") << endl;
    size_t nout2 = LineFinderUtils::binDataAndMask<float>(num_data, data, mask, bin_size, num_data, out_data2, out_mask2, offset, keepsize);
    cout << "out_data = " << print_array<float>(nout2, out_data2) << endl;
    cout << "out_mask = " << print_array<bool>(nout2, out_mask2) << endl;
  }

  if (domask)
  {// Test mask creation
    cout << "\n**********\nTest mask creation\n**********" << endl;
    size_t const num_data = 20;
    Vector<float> data(num_data);
    Vector<float> mad(num_data);
    Vector<bool> in_mask(num_data);
    Vector<bool> out_mask(num_data);
    float* dptr = data.data();
    bool* mptr = in_mask.data();
    for (size_t i = 0 ; i < num_data; ++i) {
      dptr[i] = float(i);
      mptr[i] = (i%4 != 0) ? true : false;
    }
    LineFinderUtils::calculateMAD(num_data,data.data(),in_mask.data(),mad.data());
    cout << "[MAD array]" << endl;
    cout << "data = " << print_array<float>(num_data, data.data()) << endl;
    cout << "in_mask = " << print_array<bool>(num_data, in_mask.data()) << endl;
    cout << "MAD = " << print_array<float>(num_data, mad.data()) << endl;

    float threshold = 5.0;
    cout << "\n[Mask creation]" << endl;
    cout << "MAD = " << print_array<float>(num_data, mad.data()) << endl;
    cout << "in_mask = " << print_array<bool>(num_data, in_mask.data()) << endl;
    cout << "threshold = " << threshold << endl;
    LineFinderUtils::createMaskByAThreshold(num_data, mad.data(), in_mask.data(), threshold, out_mask.data());
    cout << "out_mask = " << print_array<bool>(num_data, out_mask.data()) << endl;
    cout << "\n[Sign creation]" << endl;
    Vector<int8_t> signval(num_data);
    LineFinderUtils::createSignByAThreshold(num_data, mad.data(),
					    threshold,signval.data());
    cout << "data = " << print_array<float>(num_data, mad.data()) << endl;
    cout << "threshold = " << threshold << endl;
    cout << "sign = " << print_array<int8_t>(num_data, signval.data()) << endl;

    cout << "\n[Mask to line list]" << endl;
    list<pair<size_t,size_t>> lines;
    cout << "in_mask = " << print_array<bool>(num_data, in_mask.data()) << endl;
    LineFinderUtils::maskToRangesList(num_data, in_mask.data(), lines);
    print_line(lines);
    
    for (size_t i = 0; i<num_data; ++i) {
      in_mask[i] = (i%4 != 1) ? true : false;
    }
    cout << "\nin_mask = " << print_array<bool>(num_data, in_mask.data()) << endl;
    LineFinderUtils::maskToRangesList(num_data, in_mask.data(), lines);
    print_line(lines);
  }
  if (doline)
  { // Test handling of line_list
    cout << "\n**********\nTest line list handling\n**********" << endl;
    list<pair<size_t,size_t>> mylines;
    mylines.push_back(pair<size_t,size_t>(0,2));
    mylines.push_back(pair<size_t,size_t>(5,10));
    mylines.push_back(pair<size_t,size_t>(20,70));
    mylines.push_back(pair<size_t,size_t>(72,80));
    cout << "[ORIGINAL]" << endl;
    print_line(mylines);
    LineFinderUtils::rejectWideRange(20,mylines);
    cout << "[Regect wide (>20)]" << endl;
    print_line(mylines);
    LineFinderUtils::rejectNarrowRange(3,mylines);
    cout << "[Regect narrow (<3)]" << endl;
    print_line(mylines);
    size_t binsize = 4;
    size_t offset = 1;  
    LineFinderUtils::deBinRanges(binsize, offset, mylines);
    cout << "[debin (bin=" << binsize << ", offset=" << offset << ")]" << endl;
    print_line(mylines);
    // Merge overlap in a list
    cout << "\nMerge overlap in a list" << endl;
    mylines.clear();
    mylines.push_back(pair<size_t,size_t>(5,8));
    mylines.push_back(pair<size_t,size_t>(4,6));
    mylines.push_back(pair<size_t,size_t>(12,14));
    mylines.push_back(pair<size_t,size_t>(11,15));
    mylines.push_back(pair<size_t,size_t>(20,23));
    mylines.push_back(pair<size_t,size_t>(23,25));
    cout << "[ORIGINAL]" << endl;
    print_line(mylines);
    LineFinderUtils::mergeOverlappingRanges(mylines);
    cout << "[Merged]" << endl;
    print_line(mylines);
    // Merge overlap in two lists
    mylines.clear();
    mylines.push_back(pair<size_t,size_t>(5,10));
    mylines.push_back(pair<size_t,size_t>(15,20));
    mylines.push_back(pair<size_t,size_t>(25,30));
    mylines.push_back(pair<size_t,size_t>(35,40));
    mylines.push_back(pair<size_t,size_t>(45,50));
    mylines.push_back(pair<size_t,size_t>(55,60));
    list<pair<size_t,size_t>> new_lines;
    new_lines.push_back(pair<size_t,size_t>(0,2));
    new_lines.push_back(pair<size_t,size_t>(12,13));
    new_lines.push_back(pair<size_t,size_t>(22,27));
    new_lines.push_back(pair<size_t,size_t>(37,39));
    new_lines.push_back(pair<size_t,size_t>(42,52));
    new_lines.push_back(pair<size_t,size_t>(57,62));
    new_lines.push_back(pair<size_t,size_t>(67,69));
    cout << "[ORIGINAL]" << endl;
    cout << "to = ";
    print_line(mylines);
    cout << "from = ";
    print_line(new_lines);
    LineFinderUtils::mergeOverlapInTwoLists(mylines, new_lines);
    cout << "out = ";
    print_line(mylines);

    cout << "\nMultiple overlap" << endl;
    mylines.clear();
    mylines.push_back(pair<size_t,size_t>(5,10));
    mylines.push_back(pair<size_t,size_t>(15,20));
    mylines.push_back(pair<size_t,size_t>(25,30));
    mylines.push_back(pair<size_t,size_t>(35,40));
    mylines.push_back(pair<size_t,size_t>(45,50));
    mylines.push_back(pair<size_t,size_t>(55,60));
    new_lines.clear();
    new_lines.push_back(pair<size_t,size_t>(2,17));
    new_lines.push_back(pair<size_t,size_t>(22,42));
    new_lines.push_back(pair<size_t,size_t>(47,62));
    cout << "to = ";
    print_line(mylines);
    cout << "from = ";
    print_line(new_lines);
    LineFinderUtils::mergeOverlapInTwoLists(mylines, new_lines);
    cout << "out = ";
    print_line(mylines);
  }

  if(domaskline)
  {
    cout << "\n**********\nTest line list handling with mask\n**********" << endl;
    size_t num_data(20);
    Vector<int8_t> sign(num_data);
    Vector<bool> mask(num_data);
    for (size_t i = 0; i<num_data; ++i) {
      sign[i] = ((i/6) % 2)==0 ? 1:-1;
      mask[i] = (i % 5)==0 ? false : true;
    }
    cout << "[Extend by sign]" << endl;
    cout << "sign = " << print_array<int8_t>(num_data, sign.data()) << endl;
    cout << "in_mask = " << print_array<bool>(num_data, mask.data()) << endl;
    list<pair<size_t,size_t>> mylines;
    mylines.push_back(pair<size_t,size_t>(2,2));
    mylines.push_back(pair<size_t,size_t>(7,7));
    mylines.push_back(pair<size_t,size_t>(12,12));
    cout << "input line:" << endl;
    print_line(mylines);
    LineFinderUtils::extendRangeBySign(num_data, sign.data(), mask.data(), mylines);
    cout << "extended line:" << endl;
    print_line(mylines);

    cout << "[Merge Lines by false]" << endl;
    mylines.clear();
    mylines.push_back(pair<size_t,size_t>(5,10));
    mylines.push_back(pair<size_t,size_t>(16,18));
    for (size_t i = 0; i < num_data; ++i) {
      mask[i] = (i>mylines.front().second && i<mylines.back().first)? false : true;
    }
    size_t maxgap = 5;
    cout << "max gap = " << maxgap << endl;
    cout << "mask = " << print_array<bool>(num_data, mask.data()) << endl;
    cout << "input line = ";
    print_line(mylines);
    LineFinderUtils::mergeGapByFalse(num_data, mask.data(), maxgap, mylines);
    cout << "output line: ";
    print_line(mylines);
    // change max gap to merge. This time lines would not be merged.
    maxgap = 4;
    mylines.clear();
    mylines.push_back(pair<size_t,size_t>(5,10));
    mylines.push_back(pair<size_t,size_t>(16,18));
    cout << "max gap = " << maxgap << endl;
    cout << "mask = " << print_array<bool>(num_data, mask.data()) << endl;
    cout << "input line = ";
    print_line(mylines);
    LineFinderUtils::mergeGapByFalse(num_data, mask.data(), maxgap, mylines);
    cout << "output line: ";
    print_line(mylines);
  }
  if (dolinefinder) {
    cout << "\n**********\nTest line finding\n**********" << endl;
    string table_name = "/almadev/work/reg_test/IRC+10216_rawACSmod_cal";
    //string table_name = "/almadev/work/OrionS_rawACSmod_calTPave.asap";
    size_t row_idx = 0;
    cout << "Table: " << table_name << endl;
    cout << "idx: " << row_idx << endl;
    Table mytab(table_name, Table::Old);
    assert(row_idx<mytab.nrow());
    ScalarColumn<unsigned int> flagRCol(mytab, "FLAGROW");
    assert(flagRCol.get(row_idx)==0);
    ArrayColumn<float> specCol(mytab, "SPECTRA");
    ArrayColumn<uint8_t> flagCol(mytab, "FLAGTRA");
    Vector<float> specvec(specCol.get(row_idx));
    Vector<uint8_t> flagvec(flagCol.get(row_idx));
    size_t num_data(specvec.nelements());
    cout << "nchan: " << num_data << endl;
    Vector<float> data(num_data);
    Vector<bool> mask(num_data);
    for (size_t i=0; i<num_data; ++i) {
      data[i] = specvec[i];
      mask[i] = (flagvec[i]==static_cast<uint8_t>(0));
    }
    pair<size_t,size_t> edge(500,500);
    list<pair<size_t,size_t>> line_list = \
      linefinder::MADLineFinder(num_data, data.data(), mask.data(), 5.0, 10.0, 3, 1000, 4, edge);
    cout << "[Line finding result]" << endl;
    print_line(line_list);
  }

  return 0;
}
