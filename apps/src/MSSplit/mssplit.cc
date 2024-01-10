// -*- C++ -*-
// # mssplit.cc
// # Copyright (C) 2021
// # Associated Universities, Inc. Washington DC, USA.
// #
// # This library is free software; you can redistribute it and/or modify it
// # under the terms of the GNU Library General Public License as published by
// # the Free Software Foundation; either version 2 of the License, or (at your
// # option) any later version.
// #
// # This library is distributed in the hope that it will be useful, but WITHOUT
// # ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// # FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
// # License for more details.is
// #
// # You should have received a copy of the GNU Library General Public License
// # along with this library; if not, write to the Free Software Foundation,
// # Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
// #
// # Correspondence concerning this should be addressed as follows:
// #        Postal address: National Radio Astronomy Observatory
// #                        1003 Lopezville Road,
// #                        Socorro, NM - 87801, USA
// #
// # $Id$

#include <MSSplit/mssplit.h>


void showTableCache()
{
  const TableCache& cache = PlainTable::tableCache();
  Vector<String> lockedTables = cache.getTableNames();

  Int n=lockedTables.nelements();
  if(n > 0)
    cout << endl << "####WARNING!!!!: The Table Cache has the following " << n << " entries:"  << endl;
  
  for (Int i=0; i<n; ++i) 
    cout << "    " << i << ": \"" <<  lockedTables(i) << "\"" << endl;
}


void printBaselineList(Matrix<Int> list,ostream& os)
{
  os << "Baselines = ";
  IPosition shp=list.shape();
  for(Int j=0;j<shp(1);j++)
    {
      for(Int i=0;i<shp(0);i++)
	os << list(i,j) << " ";
      os << endl << "            " ;
    }
  os << endl;
}


void printInfo(casacore::MSSelection& msSelection)
{
  cout << "Ant1         = " << msSelection.getAntenna1List() << endl;
  cout << "Ant2         = " << msSelection.getAntenna2List() << endl;
  //  cout << "Baselines    = " << msSelection.getBaselineList() << endl;
  cout << "Field        = " << msSelection.getFieldList()    << endl;
  cout << "SPW          = " << msSelection.getSpwList()      << endl;
  cout << "Chan         = " << msSelection.getChanList(NULL,1,True)     << endl;
  cout << "Freq         = " << msSelection.getChanFreqList(NULL,True)     << endl;
  cout << "Scan         = " << msSelection.getScanList()     << endl;
  cout << "StateObsMode = " << msSelection.getStateObsModeList()     << endl;
  cout << "Array        = " << msSelection.getSubArrayList() << endl;
  cout << "Time         = " << msSelection.getTimeList()     << endl;
  cout << "UVRange      = " << msSelection.getUVList()       << endl;
  cout << "UV in meters = " << msSelection.getUVUnitsList()  << endl;
  cout << "DDIDs(Poln)  = " << msSelection.getDDIDList()     << endl;
  cout << "DDIDs(SPW)   = " << msSelection.getSPWDDIDList()     << endl;
  cout << "PolMap       = "; for(auto m : msSelection.getPolMap()) cout << m.first << " " << m.second << endl;cout << endl;
  cout << "CorrMap       = "; for(auto m : msSelection.getCorrMap()) cout << m.first << " " << m.second << endl;cout << endl;
  //  cout << "CorrMap      = " << msSelection.getCorrMap( )     << endl;
  cout << "StateList    = " << msSelection.getStateObsModeList() << endl;
  cout << "ObservationIDList    = " << msSelection.getObservationList() << endl;
  //  printBaselineList(msSelection.getBaselineList(),cout);
}


void MSSplit_func(const string& MSNBuf, const string& OutMSBuf, const bool& deepCopy, const string& fieldStr,
		 const string& timeStr, const string &spwStr, const string& baselineStr,
		 const string& uvdistStr, const string& taqlStr, const string& scanStr,
		 const string& arrayStr, const string& polnStr, const string& stateObsModeStr,
		 const string& observationStr)

{
  //
  // Make a new scope, outside of which there should be no tables left open.
  //
  {
    // try
    //   {
	MS ms(MSNBuf,TableLock(TableLock::AutoNoReadLocking)),selectedMS(ms);
	//
	// Setup the MSSelection thingi
	//
    
	MSInterface msInterface(ms);
	casacore::MSSelection msSelection;
	casacore::MSSelectionLogError mssLEA,mssLES;
	msSelection.setErrorHandler(casacore::MSSelection::ANTENNA_EXPR, &mssLEA);
	msSelection.setErrorHandler(casacore::MSSelection::STATE_EXPR, &mssLES);

    	// msSelection.reset(ms,MSSelection::PARSE_NOW,
    	// 			timeStr,baselineStr,fieldStr,spwStr,
    	// 			uvdistStr,taqlStr,polnStr,scanStr,arrayStr,
    	// 			stateObsModeStr,observationStr);
    	msSelection.reset(msInterface,MSSelection::PARSE_NOW,
    			  timeStr,baselineStr,fieldStr,spwStr,
    			  uvdistStr,taqlStr,polnStr,scanStr,arrayStr,
    			  stateObsModeStr,observationStr);
    	// TableExprNode ten=msSelection.toTableExprNode(&msInterface);
    	// cerr << "TEN rows = " << ten.nrow() << endl;
    	printInfo(msSelection);
	
    	if (!msSelection.getSelectedMS(selectedMS))
    	  {
    	    cerr << "###Informational:  Nothing selected.  ";
    	    if (OutMSBuf != "")
    	      cout << "New MS not written." << endl;
    	    else
    	      cout << endl;
    	  }
    	else
    	  if (OutMSBuf != "")
	    {
	      if (deepCopy) selectedMS.deepCopy(OutMSBuf,Table::New);
	      else          selectedMS.rename(OutMSBuf,Table::New);
	    }
    	cerr << "Number of selected rows: " << selectedMS.nrow() << endl;
      }
    // catch (MSSelectionError& x)
    //   {
    // 	cerr << "###MSSelectionError: " << x.getMesg() << endl;
    //   }
    //
    // Catch any exception thrown by AIPS++ libs.  Do your cleanup here
    // before returning to the UI (if you choose to).  Without this, all
    // exceptions (AIPS++ or otherwise) are caught in the default
    // exception handler (which is installed by the CLLIB as the
    // clDefaultErrorHandler).
    //
    // catch (AipsError& x)
    //   {
    // 	cerr << "###AipsError: " << x.getMesg() << endl;
    // 	restartUI=True;
    //   }
  //}
  //  showTableCache();
}
