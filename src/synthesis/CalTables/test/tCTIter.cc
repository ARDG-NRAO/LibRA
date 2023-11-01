//# tNewCalTable.cc: Test program for NewCalTable class
//# Copyright (C) 2011
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or (at your option)
//# any later version.
//#
//# This program is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//# more details.
//#
//# You should have received a copy of the GNU General Public License along
//# with this program; if not, write to the Free Software Foundation, Inc.,
//# 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id: tNewCalTable.cc 15602 2011-07-14 00:03:34Z tak.tsutsumi $

#include <synthesis/CalTables/NewCalTable.h>
#include <synthesis/CalTables/CTMainColumns.h>
#include <synthesis/CalTables/CTIter.h>
#include <casacore/casa/Arrays/Cube.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/iostream.h>
#include <casacore/casa/BasicMath/Math.h>

using namespace casa;
using namespace casacore;

// <summary>
// Test program for CTIter class.
// </summary>

// Control verbosity
#define CTITERTEST_VERBOSE false

Bool foundError = false;

void doTest1 (Bool verbose=false) {

  cout << "****----tCTIter doTest1()----****" << endl;
  
  // Make a testing NewCalTable (Table::Memory)
  uInt nFld(1), nAnt(10), nSpw(2), nObs(1), nScan(1),nTime(3);
  Vector<Int> nChan(nSpw,1);
  Bool disk(verbose);
  NewCalTable tnct("tCTIter1.ct","Complex",
		   nObs,nScan,nTime,
		   nAnt,nSpw,nChan,
		   nFld,
		   0.0,0.0,   // rtime,tint defaults
		   disk,false);

  // some sanity checks on the test NewCalTable
  if (verbose) cout << "Table::Type: " << tnct.tableType() 
		    << " (should be " << Table::Memory << ")"
		    << endl;
  AlwaysAssert( (tnct.tableType() == Table::Memory), AipsError);
  if (verbose) cout << "nrow = " << tnct.nrow() 
		    << "  (should be " << nObs*nScan*nTime*nSpw*nAnt << ")"
		    << endl;
  AlwaysAssert( (tnct.nrow()==nObs*nScan*nTime*nSpw*nAnt), AipsError);
  
  // Set up iteration
  Block<String> sortcol(2);
  sortcol[0]="SPECTRAL_WINDOW_ID";
  sortcol[1]="ANTENNA1";
  ROCTIter nctiter(tnct,sortcol);

  // Count iterations
  //  TBD: provide this service in CalIter itself!
  uInt niter(0);
  while (!nctiter.pastEnd()) {
    niter+=1;
    nctiter.next();
  }

  if (verbose) cout << "niter = " << niter 
		    << " (should be " << nAnt*nSpw << ")"
		    << endl;
  AlwaysAssert( (niter==nAnt*nSpw), AipsError);
  
  // Test individual iterations
  nctiter.reset();
  Vector<NewCalTable> tablist(niter);
  Int iter=0;
  if (verbose) cout << "Testing iteration." << endl;
  while (!nctiter.pastEnd()) {
    Int expectSpw(iter/nAnt);
    Int expectAnt(iter%nAnt);    
    Int thisspw(nctiter.thisSpw());
    Int thisant(nctiter.thisAntenna1());

    NewCalTable tab(nctiter.table());
    tablist(iter)=tab;

    if (verbose) {
      cout << iter 
	   << " nrow=" << nctiter.nrow()
	   << " spw="<<thisspw
	   << " (should be " << expectSpw << ")"
	   << " ant1="<<thisant
	   << " (should be " << expectAnt << ")"
	   << endl;
    }

    AlwaysAssert( (thisspw==expectSpw) , AipsError);
    AlwaysAssert( (thisant==expectAnt) , AipsError);

    nctiter.next();
    ++iter;
  }
  

  // Test recorded reference table contents
  if (verbose) cout << "Testing iteration reference table contents:" << endl;
  for (uInt i=0;i<niter;++i) {
    Int expectSpw(i/nAnt);
    Int expectAnt(i%nAnt);
    
    CTMainColumns mc(tablist(i));
    
    Vector<Int> thisspw=mc.spwId().getColumn();
    Vector<Int> thisant=mc.antenna1().getColumn();

    if (verbose) {
      cout << i << " "
	   << "nrow=" << tablist(i).nrow() << " "
	   << "spw=" << thisspw(0) 
	   << " (should be " << expectSpw << ")"
	   << " ant1=" << thisant(0) 
	   << " (should be " << expectAnt << ")"
	//	   << "paramshape= " << mc.cparam().shape(0) << " "
	//	   << "param= " << mc.cparam()(0) << " "
	   << endl;
    }

    // Tests
    AlwaysAssert( (tablist(i).nrow()==nObs*nScan*nTime) , AipsError);
    AlwaysAssert( allEQ(thisspw,expectSpw) , AipsError);
    AlwaysAssert( allEQ(thisant,expectAnt) , AipsError);

    // TBD: Add param value tests here...

  }
}
void doTest2 (Bool verbose=false) {
  
  cout << "****----tCTIter doTest2()----****" << endl;


  // Test (the writable) CTIter

  // Make a testing NewCalTable (Table::Memory)
  uInt nFld(2), nAnt(10), nSpw(3), nObs(4), nScan(2), nTime(3);
  Vector<Int> nChan(nSpw,1);
  Double rtime(0.0), tint(60.0);  // trigger default times
  Bool disk(verbose);
  NewCalTable tnct("tCTIter2.ct","Complex",
		   nObs,nScan,nTime,
		   nAnt,nSpw,nChan,
		   nFld,
		   rtime,tint,
		   disk,false);


  if (verbose)
    cout << "OBS_ID col ok? " << boolalpha << tnct.tableDesc().isColumn(NCT::fieldName(NCT::OBSERVATION_ID)) << endl;
  AlwaysAssert( tnct.tableDesc().isColumn(NCT::fieldName(NCT::OBSERVATION_ID)), AipsError);

  // some sanity checks on the test NewCalTable
  if (verbose) cout << "Table::Type: " << tnct.tableType() 
		    << " (should be " << Table::Memory << ")" << endl;
  AlwaysAssert( (tnct.tableType() == Table::Memory), AipsError);

  Int nTotRow(tnct.nrow());
  if (verbose) cout << "nrow = " << nTotRow
		    << " (should be " << nObs*nScan*nTime*nSpw*nAnt << ")"
		    << endl;
  AlwaysAssert( (tnct.nrow()==nObs*nScan*nTime*nSpw*nAnt), AipsError);
  
  // Set up iteration
  Block<String> sortcol(3);
  sortcol[0]="OBSERVATION_ID";
  sortcol[1]="SPECTRAL_WINDOW_ID";
  sortcol[2]="ANTENNA1";
  CTIter nctiter(tnct,sortcol);

  // Count iterations
  //  TBD: provide this service in CalIter itself!
  uInt niter(0);
  while (!nctiter.pastEnd()) {
    niter+=1;
    nctiter.next();
  }
  if (verbose) cout << "niter = " << niter 
		    << " (should be " << nObs*nSpw*nAnt << ")"
		    << endl;
  AlwaysAssert( (niter==nObs*nSpw*nAnt), AipsError)
  
  // Test individual iterations
  nctiter.reset();
  Vector<NewCalTable> tablist(niter);
  Int iter=0;
  if (verbose) cout << "Testing iteration." << endl;
  while (!nctiter.pastEnd()) {
    Int expectObs(iter/(nAnt*nSpw));
    Int expectSpw((iter%(nAnt*nSpw))/nAnt);
    Int expectAnt(iter%nAnt);    
    Int thisobs(nctiter.thisObs());
    Int thisspw(nctiter.thisSpw());
    Int thisant(nctiter.thisAntenna1());

    Vector<Int> obsv(nctiter.obs());
    Vector<Int> scanv(nctiter.scan());

    NewCalTable tab(nctiter.table());
    tablist(iter)=tab;

    if (verbose) {
      cout << iter 
	   << " nrow=" << nctiter.nrow()
	   << " (should be " << nTotRow/niter << ")"
	   << " obs="<<thisobs
	   << " (should be " << expectObs << ")"
	   << " spw="<<thisspw
	   << " (should be " << expectSpw << ")"
	   << " ant1="<<thisant
	   << " (should be " << expectAnt << ")"
	   << endl;
    }

    AlwaysAssert( (nctiter.nrow()==Int(nTotRow/niter)) , AipsError);
    AlwaysAssert( (thisobs==expectObs) , AipsError);
    AlwaysAssert( (thisspw==expectSpw) , AipsError);
    AlwaysAssert( (thisant==expectAnt) , AipsError);

    // Play with setable columns
    Cube<Bool> flag(nctiter.flag());
    //    cout << boolalpha << " flag = " << flag << endl;
    if (thisant==Int(nAnt/2))
      flag=true;
    nctiter.setflag(flag);

    nctiter.next();
    ++iter;
  }
  

  // Test recorded reference table contents
  if (verbose) cout << "Testing iteration reference table contents:" << endl;
  for (uInt i=0;i<niter;++i) {
    Int expectObs(i/(nAnt*nSpw));
    Int expectSpw((i%(nAnt*nSpw))/nAnt);
    Int expectAnt(i%nAnt);
    
    CTMainColumns mc(tablist(i));
    
    Vector<Int> thisobs=mc.obsId().getColumn();
    Vector<Int> thisspw=mc.spwId().getColumn();
    Vector<Int> thisant=mc.antenna1().getColumn();

    if (verbose) {
      cout.precision(15);
      cout << i << " "
	   << "nrow=" << tablist(i).nrow() << " "
	   << " (should be " << nScan*nTime << ")"
	   << " obs=" << thisobs(0) 
	   << " (should be " << expectObs << ")"
	   << " spw=" << thisspw(0) 
	   << " (should be " << expectSpw << ")"
	   << " ant1=" << thisant(0) 
	   << " (should be " << expectAnt << ")"
	   << endl;
    }

    // Tests
    AlwaysAssert( (tablist(i).nrow()==nScan*nTime) , AipsError);
    AlwaysAssert( allEQ(thisobs,expectObs) , AipsError);
    AlwaysAssert( allEQ(thisspw,expectSpw) , AipsError);
    AlwaysAssert( allEQ(thisant,expectAnt) , AipsError);

    // Test the flags we wrote
    if (thisant(0)==Int(nAnt/2)) {
      Bool f(Cube<Bool>(mc.flag().getColumn())(0,0,0));
      AlwaysAssert( (f), AipsError);
    }


    // TBD: Add param value tests here...

  }

}

int main ()
{
  try {

    doTest1(CTITERTEST_VERBOSE);
    doTest2(CTITERTEST_VERBOSE);

  } catch (AipsError x) {
    cout << "Unexpected exception: " << x.getMesg() << endl;
    exit(1);
  } catch (...) {
    cout << "Unexpected unknown exception" << endl;
    exit(1);
  }
  if (foundError) {
    exit(1);
  }
  cout << "OK" << endl;
  exit(0);
};
