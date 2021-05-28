//# CalSet.cc: Implementation of Calibration parameter cache
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
//# Associated Universities, Inc. Washington DC, USA.
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

#include <synthesis/CalTables/CTGlobals.h>
#include <synthesis/CalTables/NewCalTable.h>
#include <synthesis/CalTables/CTIter.h>
#include <synthesis/CalTables/RIorAParray.h>

#include <casa/Arrays.h>
#include <casa/BasicSL/String.h>
#include <casa/Utilities/GenSort.h>
#include <casa/Exceptions/Error.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/TableIter.h>
#include <tables/Tables/TableVector.h>

#include <casa/sstream.h>

#include <casa/Logging/LogMessage.h>
#include <casa/Logging/LogSink.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN


void smoothCT(NewCalTable ct,
	      const String& smtype,
	      const Double& smtime,
	      Vector<Int> selfields) {

  // Complex parameters?
  Bool cmplx=ct.isComplex();

  // half-width
  Double thw(smtime/2.0);

  // Workspace
  Vector<Double> times;
  Vector<Float> p,newp;
  Vector<Bool> pOK, newpOK;

  Cube<Float> fpar;
  Cube<Bool> fparok,newfparok;

  Vector<Bool> mask;

  IPosition blc(3,0,0,0), fblc(3,0,0,0);
  IPosition trc(3,0,0,0), ftrc(3,0,0,0);
  IPosition vec(1,0);

  Block<String> cols(4);
  cols[0]="SPECTRAL_WINDOW_ID";
  cols[1]="FIELD_ID";
  cols[2]="ANTENNA1";
  cols[3]="ANTENNA2";
  CTIter ctiter(ct,cols);

  while (!ctiter.pastEnd()) {

    Int nSlot=ctiter.nrow();
    Int ifld=ctiter.thisField();

    // Only if more than one slot in this spw _AND_
    //  field is among those requested (if any)
    if (nSlot>1 && 
	(selfields.nelements()<1 || anyEQ(selfields,ifld))) {

      //UNUSED: Int ispw=ctiter.thisSpw();
      
      vec(0)=nSlot;
      trc(2)=ftrc(2)=nSlot-1;

      times.assign(ctiter.time());

      // Extract Float info
      if (cmplx)
        fpar.assign(ctiter.casfparam("AP"));
      else
        fpar.assign(ctiter.fparam());

      fparok.assign(!ctiter.flag());
      newfparok.assign(fparok);
      IPosition fsh(fpar.shape());

      // For each channel
      for (int ichan=0;ichan<fsh(1);++ichan) {
	blc(1)=trc(1)=fblc(1)=ftrc(1)=ichan;
	// For each param (pol)
	for (Int ipar=0;ipar<fsh(0);++ipar) {
	  blc(0)=trc(0)=ipar;
	  fblc(0)=ftrc(0)=ipar/(cmplx?2:1);
	  
	  // Reference slices of par/parOK
	  p.reference(fpar(blc,trc).reform(vec));
	  newp.assign(p);
	  pOK.reference(fparok(fblc,ftrc).reform(vec));
	  newpOK.reference(newfparok(fblc,ftrc).reform(vec));

       /*
	    cout << ispw << " "
		 << ichan << " "
		 << ipar << " "
		 << "p.shape() = " << p.shape() << " "
		 << "pOK.shape() = " << pOK.shape() << " "
		 << endl;
       */

	    
	  Vector<Bool> mask;
	  for (Int i=0;i<nSlot;++i) {
	    // Make mask
	    mask = pOK;
	    mask = (mask && ( (times >  (times(i)-thw)) && 
			      (times <= (times(i)+thw)) ) );

	    // Avoid explicit zeros, for now
	    //	    mask = (mask && amp>=FLT_MIN);


	    //cout << "    " << ifld << " " << i << " " << idx(i) << " ";
	    //for (Int j=0;j<mask.nelements();++j)
	    //  cout << mask(j);
	    //cout << endl;
	    
	    if (ntrue(mask)>0) {
	      if (smtype=="mean") {
		newp(i)=mean(p(mask));
	      }
	      else if (smtype=="median") {
		newp(i)= median(p(mask),false);
	      }
	      newpOK(i)=true;
	    }
	    else 
	      newpOK(i)=false;
	    
	  } // i
	  // keep new ok info
	  p=newp;
	} // ipar
      } // ichan

      // Put info back
      if (cmplx)
        ctiter.setcparam(RIorAPArray(fpar).c());
      else
        ctiter.setfparam(fpar);

      ctiter.setflag(!newfparok);

    } // nSlot>1

    ctiter.next();
  } // ispw
	
}

void assignCTScanField(NewCalTable& ct, String msName, 
		       Bool doField, Bool doScan, Bool doObs) {

  // TBD: verify msName is present and is an MS

  // Arrange to iterate only on SCAN  (and SPW?)
  Table mstab(msName,Table::Old);

  // How many scans in total?
  TableVector<Int> allscansTV(mstab,"SCAN_NUMBER");
  Vector<Int> allscans=allscansTV.makeVector();
  Int nScan=genSort(allscans,Sort::Ascending,(Sort::QuickSort | Sort::NoDuplicates));

  //  cout << "Found " << nScan << " scans in " << msName << "." << endl;

  // Workspace
  Vector<Int> scanlist(nScan,-1);
  Vector<Double> timelo(nScan,DBL_MIN);
  Vector<Double> timehi(nScan,DBL_MAX);
  Vector<Int> fieldlist(nScan,-1);
  Vector<Int> obslist(nScan,-1);
  Vector<uInt> ord;
  {
    Block<String> cols(1);
    cols[0]="SCAN_NUMBER";
    TableIterator mstiter(mstab,cols);
    
    // Get time boundares and fields for each scan
    Int iscan(0);
    while (!mstiter.pastEnd()) {
      Table thistab(mstiter.table());
      
      Int scan=TableVector<Int>(thistab,"SCAN_NUMBER")(0);
      scanlist(iscan)=scan;
      
      fieldlist(iscan)=TableVector<Int>(thistab,"FIELD_ID")(0);
      obslist(iscan)=TableVector<Int>(thistab,"OBSERVATION_ID")(0);
      
      Vector<Double> times=TableVector<Double>(thistab,"TIME").makeVector();
      timelo(iscan)=min(times)-1e-5;
      timehi(iscan)=max(times)+1e-5;
      
      mstiter.next();
      ++iscan;
    }


    // Ensure time orderliness
    genSort(ord,timehi);

    /*
    cout << "scanlist=" << scanlist << endl;
    cout << "fieldlist=" << fieldlist << endl;
    cout << "timelo=" << timelo-timelo(0) << endl;
    cout << "timehi=" << timehi-timelo(0) << endl;
    cout << "ord.nelements() = " << ord.nelements() << endl;
    cout << "ord = " << ord << endl;
    */

  }

  //UNUSED: Double rTime=timelo(ord(0));

  // Now iterate throught the NCT and set field and scan according to time
  Block<String> cols(1);
  cols[0]="TIME";
  CTIter ctiter(ct,cols);

  Int itime(0);
  Int thisObs(0);
  Int thisScan(0);
  Int thisField(0);
  while (!ctiter.pastEnd()) {
    Double thisTime=ctiter.thisTime();

    //    cout.precision(12);
    //    cout << "thisTime = " << thisTime-rTime << endl;

    // If time before first MS time, just use first
    if (thisTime<timehi(ord(0))) {
      itime=0;
      //      cout << " Pre: ";
    }
    // If time after last MS time, use last
    else if (thisTime>timelo(ord(nScan-1))) {
      itime=nScan-1;
      //      cout << " Post: ";
    }
    else if (thisTime>timehi(ord(itime))) {

      // Isolate correct time index
      while (thisTime>timehi(ord(itime))&& itime<nScan) {
	//	cout << itime << " " << thisTime-rTime << ">" << timehi(ord(itime))-rTime << endl;
	++itime;
      }
      //      cout << " Found: ";
    }
    //else 
      //      cout << " Still: ";

    thisObs=obslist(ord(itime));
    thisScan=scanlist(ord(itime));
    thisField=fieldlist(ord(itime));

    /*
    cout << " itime=" << itime << " "
	 << timelo(ord(itime))-rTime << " < "
	 << thisTime-rTime << " < "
	 << timehi(ord(itime))-rTime
	 << " s=" << thisScan << " f=" << thisField << endl;
    */

    // Set the field and scan
    if (doField) 
      ctiter.setfield(thisField);
    if (doScan) 
      ctiter.setscan(thisScan);
    if (doObs) 
      ctiter.setobs(thisObs);
    
    ctiter.next();
  }

}

} //# NAMESPACE CASA - END
