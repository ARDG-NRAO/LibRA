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

#include <subms.h>
//
//-------------------------------------------------------------------------
//
//
//-------------------------------------------------------------------------

void SubMS_func(const string& MSNBuf, const string& OutMSBuf,
		const string& WhichColStr, const bool& deepCopy,
		const string& fieldStr,const string& timeStr,
		const string& spwStr, const string& baselineStr,
		const string& scanStr, const string& arrayStr,
		const string& uvdistStr,const string& taqlStr,
		const float integ)
{
  //
  //---------------------------------------------------
  //
  //  MSSelection msSelection;
  // try
    {
      //MS ms(MSNBuf,Table::Update),selectedMS(ms);
      MeasurementSet ms(MSNBuf,TableLock(TableLock::AutoNoReadLocking)),selectedMS(ms);

      if (OutMSBuf != "")
	{
	  //
	  // Damn CASA::Strings!
	  //
	  String OutMSName(OutMSBuf), WhichCol(WhichColStr);
	  //	    SubMS splitter(selectedMS);
	  //
	  // SubMS class is not msselection compliant (it's a strange
	  // mix of msselection and selection-by-hand)!
	  //
	  SubMS splitter(ms);
	  Vector<int> nchan(1,10), start(1,0), step(1,1);
	  String CspwStr(spwStr), CfieldStr(fieldStr), CbaselineStr(baselineStr),
	    CscanStr(scanStr), CuvdistStr(uvdistStr), CtaqlStr(taqlStr), CtimeStr(timeStr);
	  splitter.setmsselect(CspwStr, CfieldStr, CbaselineStr, CscanStr, CuvdistStr,
			       CtaqlStr);//, nchan,start, step);

	  splitter.selectTime(integ,CtimeStr);
	  splitter.makeSubMS(OutMSName, WhichCol);
	}
      //      cerr << "Number of selected rows: " << selectedMS.nrow() << endl;
    }
  // catch (clError& x)
  //   {
  //     x << x.what() << endl;
  //     restartUI=true;
  //   }
  // catch (MSSelectionError& x)
  //   {
  //     cerr << "###MSSelectionError: " << x.getMesg() << endl;
  //     restartUI=true;
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
  //     cerr << "###AipsError: " << x.getMesg() << endl;
  //     restartUI=true;
  //   }
  // if (restartUI) RestartUI(RENTER);
}
