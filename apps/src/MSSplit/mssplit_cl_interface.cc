/**
 * This file contains the implementation of the MSSplit command line interface.
 * It provides a UI function to interact with the user and parse command line arguments.
 * The UI function takes in various parameters such as input and output table names, selection criteria, etc.
 * The main function initializes these parameters and calls the UI function to start the interaction with the user.
 */


#include <cl.h> // C++ized version
#include <clinteract.h>

#include <MSSplit/mssplit.h>

//
//-------------------------------------------------------------------------
//
#define RestartUI(Label)  {if(clIsInteractive()) {goto Label;}}
//#define RestartUI(Label)  {if(clIsInteractive()) {clRetry();goto Label;}}
//

/**
 * @brief This function is responsible for handling user input for the MSSplit application.
 * 
 * @param restart A boolean value indicating whether the application is being restarted.
 * @param argc An integer value indicating the number of command line arguments.
 * @param argv A pointer to a character array containing the command line arguments.
 * @param MSNBuf A reference to a string variable to store the input table name.
 * @param OutMSBuf A reference to a string variable to store the output table name.
 * @param deepCopy A boolean reference to indicate whether to perform a deep copy.
 * @param fieldStr A reference to a string variable to store the field selection.
 * @param timeStr A reference to a string variable to store the time selection.
 * @param spwStr A reference to a string variable to store the spectral window selection.
 * @param baselineStr A reference to a string variable to store the baseline selection.
 * @param scanStr A reference to a string variable to store the scan selection.
 * @param arrayStr A reference to a string variable to store the array selection.
 * @param uvdistStr A reference to a string variable to store the uvdist selection.
 * @param taqlStr A reference to a string variable to store the TAQL selection.
 * @param polnStr A reference to a string variable to store the polarization selection.
 * @param stateObsModeStr A reference to a string variable to store the state observation mode selection.
 * @param observationStr A reference to a string variable to store the observation selection.
 * 
 * @throws AipsError if the input table name is not set.
 */
void UI(Bool restart, int argc, char **argv, string& MSNBuf, string& OutMSBuf, bool& deepCopy,
	string& fieldStr, string& timeStr, string& spwStr, string& baselineStr,
	string& scanStr, string& arrayStr, string& uvdistStr,string& taqlStr, string& polnStr,
	string& stateObsModeStr, string& observationStr)
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
      MSNBuf=OutMSBuf=timeStr=baselineStr=uvdistStr=scanStr=arrayStr=polnStr=stateObsModeStr="";
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
    }
  catch (clError x)
    {
      x << x << endl;
      clRetry();
    }
  if (MSNBuf=="")
    throw(AipsError("Input table name not set."));
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
  Bool restartUI=False;;

 RENTER:// UI re-entry point.
  //
  //---------------------------------------------------
  //
  // Make a new scope, outside of which there should be no tables left open.
  //
  {
    try
      {
	MSNBuf=OutMSBuf=fieldStr=timeStr=spwStr=baselineStr=
	  uvdistStr=taqlStr=scanStr=arrayStr=polnStr=stateObsModeStr=observationStr="";
	deepCopy=0;
	fieldStr=spwStr="*";
	fieldStr=spwStr="";
	UI(restartUI,argc, argv, MSNBuf,OutMSBuf, deepCopy,
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
  restartUI=True;RestartUI(RENTER);
}
