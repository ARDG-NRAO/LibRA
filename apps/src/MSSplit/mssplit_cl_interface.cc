/**
 * This file contains the implementation of the MSSplit command line interface.
 * It provides a UI function to interact with the user and parse command line arguments.
 * The UI function takes in various parameters such as input and output table names, selection criteria, etc.
 * The main function initializes these parameters and calls the UI function to start the interaction with the user.
 */


#include <cl.h> // C++ized version
#include <clinteract.h>

#include <mssplit.h>

//
//-------------------------------------------------------------------------
//
#define RestartUI(Label)  {if(clIsInteractive()) {goto Label;}}
//#define RestartUI(Label)  {if(clIsInteractive()) {clRetry();goto Label;}}
//


void UI(Bool restart, int argc, char **argv, bool interactive, string& MSNBuf, string& OutMSBuf, bool& deepCopy,
	string& fieldStr, string& timeStr, string& spwStr, string& baselineStr,
	string& scanStr, string& arrayStr, string& uvdistStr,string& taqlStr, string& polnStr,
	string& stateObsModeStr, string& observationStr)
{
  clSetPrompt(interactive);

  if (!restart)
    {
      BeginCL(argc,argv);
      clInteractive(0);
    }
  //else
  // clRetry();
  REENTER:
  try
    {
      int i;
      // can't init the follwoing; otherwise, the values passed through the non-interactive UI would be reset.
      //MSNBuf=OutMSBuf=timeStr=baselineStr=uvdistStr=scanStr=arrayStr=polnStr=stateObsModeStr="";
      i=1;clgetSValp("ms", MSNBuf,i);  
      i=1;clgetSValp("outms",OutMSBuf,i);  
      i=1;clgetBValp("deepcopy",deepCopy,i);
      clgetFullValp("field",fieldStr);
      clgetFullValp("time",timeStr);  
      clgetFullValp("spw",spwStr);  
      clgetFullValp("poln",polnStr);  
      clgetFullValp("baseline",baselineStr);  
      clgetFullValp("scan",scanStr);  
      clgetFullValp("array",arrayStr);  
      clgetFullValp("uvdist",uvdistStr);  
      clgetFullValp("stateobsmode",stateObsModeStr);  
      clgetFullValp("observation",observationStr);  
      dbgclgetFullValp("taql",taqlStr);  
      EndCL();

      // do some input parameter checking now.
      string mesgs;

      if (MSNBuf=="")
       mesgs += "Input MS name not set.\n";

      if (mesgs != "")
       clThrowUp(mesgs,"###Fatal", CL_FATAL);
    }
  catch (clError& x)
    {
      x << x << endl;
      if (x.Severity() == CL_FATAL) exit(1);
      //clRetry();
      RestartUI(REENTER);
    }
}

int main(int argc, char **argv)
{
  //
  //---------------------------------------------------
  //
  //  MSSelection msSelection;
  string MSNBuf,OutMSBuf,fieldStr,timeStr,spwStr,baselineStr,
    uvdistStr,taqlStr,scanStr,arrayStr, polnStr,stateObsModeStr,
    observationStr;
  Bool deepCopy=0;
  Bool restartUI=False;

 REENTER:// UI re-entry point.
  //
  //---------------------------------------------------
  //
  // Make a new scope, outside of which there should be no tables left open.
  //
  {
    try
      {
	MSNBuf=OutMSBuf=fieldStr=timeStr=spwStr=baselineStr=
	  uvdistStr=taqlStr=scanStr=arrayStr=polnStr=
	  stateObsModeStr=observationStr=fieldStr="";
	spwStr="*";
	deepCopy=0;

        bool interactive = true;

	UI(restartUI,argc, argv, interactive, MSNBuf,OutMSBuf, deepCopy,
	   fieldStr,timeStr,spwStr,baselineStr,scanStr,arrayStr,
	   uvdistStr,taqlStr,polnStr,stateObsModeStr,observationStr);
	restartUI = False;

	MSSplit_func(MSNBuf,OutMSBuf, deepCopy,fieldStr,timeStr,
		     spwStr,baselineStr,scanStr,arrayStr,uvdistStr,
		     taqlStr,polnStr,stateObsModeStr,observationStr);
      }
    catch (clError& x)
      {
    	x << x.what() << endl;
    	restartUI=True;
      }
    catch (MSSelectionError& x)
      {
    	cerr << "###MSSelectionError: " << x.getMesg() << endl;
    	restartUI=True;
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
    	restartUI=True;
      }
  }

  showTableCache();
  //if (restartUI) 
  //restartUI=True;
  //RestartUI(REENTER);
}
