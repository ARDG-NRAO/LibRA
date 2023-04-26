#include <casa/aips.h>
#include <ms/MSSel/MSSelection.h>
#include <ms/MSSel/MSSelectionError.h>
#include <ms/MSSel/MSSelectionTools.h>
#include <msvis/MSVis/SubMS.h>
#include <cl.h> // C++ized version
#include <clinteract.h>

using namespace std;
using namespace casa;
using namespace casacore;
//
//-------------------------------------------------------------------------
//
#define RestartUI(Label)  {if(clIsInteractive()) {goto Label;}}
//#define RestartUI(Label)  {if(clIsInteractive()) {clRetry();goto Label;}}
//
void UI(bool restart, int argc, char **argv, string& MSNBuf, string& OutMSBuf, 
	string& WhichColStr,int& deepCopy,string& fieldStr, string& timeStr, 
	string& spwStr, string& baselineStr,string& scanStr, string& arrayStr, 
	string& uvdistStr,string& taqlStr,float& integ)
{
  if (!restart)
    {
      BeginCL(argc,argv);
      clInteractive(0);
    }
  else
   clRetry();
  try
    {
      int i;
      MSNBuf=OutMSBuf=timeStr=baselineStr=uvdistStr=scanStr=arrayStr="";
      i=1;clgetSValp("ms", MSNBuf,i);  
      i=1;clgetSValp("outms",OutMSBuf,i);  
      clgetFullValp("datacolumn",WhichColStr);
      //      i=1;clgetIValp("deepcopy",deepCopy,i);
      clgetFullValp("field",fieldStr);
      clgetFullValp("time",timeStr);  
      i=1;clgetFValp("integ",integ,i);
      clgetFullValp("spw",spwStr);  
      clgetFullValp("baseline",baselineStr);  
      clgetFullValp("scan",scanStr);  
      clgetFullValp("array",arrayStr);  
      clgetFullValp("uvdist",uvdistStr);  
      dbgclgetFullValp("taql",taqlStr);  
      clSetOptions("datacolumn", {"data","model","corrected","all","(a list of comma-separated names)"});

      EndCL();
    }
  catch (clError x)
    {
      x << x << endl;
      clRetry();
    }
  if (MSNBuf == "")
    throw(clError("Need an input MS name", "###UserError"));
    
  if (OutMSBuf == "")
    throw(clError("Need an output MS name", "###UserError"));
}
//
//-------------------------------------------------------------------------
//
int main(int argc, char **argv)
{
  //
  //---------------------------------------------------
  //
  //  MSSelection msSelection;
  string MSNBuf,OutMSBuf,WhichColStr="data",fieldStr="*",timeStr,spwStr="*",
    baselineStr,uvdistStr, taqlStr,scanStr,arrayStr,corrStr;
  int deepCopy=0;
  bool restartUI=false;;
  float integ=-1;
 RENTER:// UI re-entry point.
  //
  //---------------------------------------------------
  //
  try
    {
  MSNBuf=OutMSBuf=WhichColStr=fieldStr=timeStr=spwStr=baselineStr=uvdistStr=
    taqlStr=scanStr=corrStr=arrayStr="";
  WhichColStr="data"; fieldStr="*"; spwStr="*";
  deepCopy=0;
  UI(restartUI,argc, argv, MSNBuf,OutMSBuf, WhichColStr, deepCopy,
     fieldStr,timeStr,spwStr,baselineStr,scanStr,arrayStr,uvdistStr,taqlStr,integ);
  restartUI = false;
  corrStr.resize(0);

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
  catch (clError& x)
    {
      x << x.what() << endl;
      restartUI=true;
    }
  catch (MSSelectionError& x)
    {
      cerr << "###MSSelectionError: " << x.getMesg() << endl;
      restartUI=true;
    }
  //
  // Catch any exception thrown by AIPS++ libs.  Do your cleanup here
  // before returning to the UI (if you choose to).  Without this, all
  // exceptions (AIPS++ or otherwise) are caught in the default
  // exception handler (which is installed by the CLLIB as the
  // clDefaultErrorHandler).
  //
  catch (AipsError& x)
    {
      cerr << "###AipsError: " << x.getMesg() << endl;
      restartUI=true;
    }
  if (restartUI) RestartUI(RENTER);
}
