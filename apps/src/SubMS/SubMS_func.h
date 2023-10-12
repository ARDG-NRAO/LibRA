#include <casa/aips.h>
#include <ms/MSSel/MSSelection.h>
#include <ms/MSSel/MSSelectionError.h>
#include <ms/MSSel/MSSelectionTools.h>
#include <msvis/MSVis/SubMS.h>

using namespace std;
using namespace casa;
using namespace casacore;
//
//-------------------------------------------------------------------------
//
//
//-------------------------------------------------------------------------
//
/**
 * @fn void SubMS_func(const std::string& MSName, const std::string& subMSName, const std::string& fieldStr, const std::string& spwStr, const std::string& uvDistStr)
 * @brief Creates a subset of a MeasurementSet.
 * @param MSName The name of the original MeasurementSet.
 * @param subMSName The name of the subset MeasurementSet to be created.
 * @param fieldStr The field string to select specific fields.
 * @param spwStr The spectral window string to select specific spectral windows.
 * @param uvDistStr The UV distance string to select specific UV distances.
 */
void SubMS_func(const string& MSNBuf="", const string& OutMSBuf="",
		const string& WhichColStr="data", const bool& deepCopy=false,
		const string& fieldStr="*",const string& timeStr="",
		const string& spwStr="*", const string& baselineStr="",
		const string& scanStr="", const string& arrayStr="",
		const string& uvdistStr="",const string& taqlStr="",
		const float integ=-1)
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
