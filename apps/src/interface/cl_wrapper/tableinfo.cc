/**
 * @file
 * This file contains the implementation of the main function and UI function for the TableInfo application.
 * The TableInfo application is a command-line tool that provides information about a MeasurementSet table.
 * It takes input table name and output file name as command-line arguments.
 * It uses the TableInfo_func function to extract information from the input table and write it to the output file.
 */
#include <casa/aips.h>
#include <ms/MSOper/MSSummary.h>
#include <ms/MeasurementSets/MeasurementSet.h>
#include <casa/Logging/LogIO.h>
#include <casa/Logging/StreamLogSink.h>
#include <images/Images/TempImage.h>
#include <images/Images/ImageInterface.h>
#include <images/Images/ImageUtilities.h>
#include <images/Images/ImageOpener.h>
#include <images/Images/ImageSummary.h>
#include <casa/Containers/Record.h>
#include <lattices/Lattices/PagedArray.h>
#include <fstream>
//
#include <cl.h>
#include <clinteract.h>

#include <TableInfo/TableInfo_func.h>

using namespace std;
using namespace casacore;

//
//-------------------------------------------------------------------------
//
#define RestartUI(Label)  {if(clIsInteractive()) {goto Label;}}
//#define RestartUI(Label)  {if(clIsInteractive()) {clRetry();goto Label;}}
//

/**
 * @brief This function retrieves command-line arguments using the cl library and sets the input and output file names.
 * 
 * @param restart A boolean flag indicating whether the function is being called for the first time or being restarted.
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @param MSNBuf A string reference to store the input table name.
 * @param OutBuf A string reference to store the output file name.
 * @param verbose A boolean reference to store the verbosity flag.
 * 
 * @throws AipsError if the input table name is not set.
 */
void UI(Bool restart, int argc, char **argv, 
	string& MSNBuf, string& OutBuf,
	bool& verbose)
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

      MSNBuf=OutBuf="";
      i=1;clgetSValp("table", MSNBuf,i);  
      i=1;clgetSValp("outfile",OutBuf,i);  
      i=1;clgetBValp("verbose",verbose,i);  
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
//
//-------------------------------------------------------------------------
//
int main(int argc, char **argv)
{
  //
  //---------------------------------------------------
  //
  string MSNBuf,OutBuf;
  Bool verbose=0, restartUI=False;;

 RENTER:// UI re-entry point.
  try
    {
      MSNBuf=OutBuf="";
      UI(restartUI,argc, argv, MSNBuf,OutBuf,verbose);
      restartUI = False;
      //
      //---------------------------------------------------
      //
      TableInfo_func(MSNBuf, OutBuf, verbose);
    }
  catch (clError& x)
    {
      x << x.what() << endl;
      restartUI=True;
    }
  //
  // Catch any exception thrown by AIPS++ libs.  Do your cleanup here
  // before returning to the UI (if you choose to).  Without this, all
  // exceptions (AIPS++ or otherwise) are caught in the default
  // exception handler clDefaultErrorHandler (installed by the CLLIB).
  //
  catch (AipsError& x)
    {
      cerr << "###AipsError: " << x.getMesg() << endl;
      restartUI=True;
      //      exit(0);
    }
  if (restartUI) RestartUI(RENTER);
}
