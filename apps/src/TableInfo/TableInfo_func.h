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

using namespace std;
using namespace casacore;

//
//-------------------------------------------------------------------------
//
void TableInfo_func(const string& MSNBuf="",
		    const string& OutBuf="",
		    const bool& verbose=false)
{
  //
  //---------------------------------------------------
  //

  // try
    {
      //
      //---------------------------------------------------
      //
      String type;
      {
	Table table(MSNBuf,TableLock(TableLock::AutoNoReadLocking));
	TableInfo& info = table.tableInfo();
	type=info.type();
      }
      ofstream ofs(OutBuf.c_str());
      
      ostringstream os;
      //      StreamLogSink sink(LogMessage::NORMAL, ofs);
      LogSink sink(LogMessage::NORMAL,&os);
      LogIO logio(sink);
      //      ostream os(logio.output());

      //
      // We need polymorphic TableSummary class.
      //
      if (type=="Measurement Set")
	{
	  //	  MS ms(MSNBuf,Table::Update);
	  MS ms(MSNBuf,TableLock(TableLock::AutoNoReadLocking));
	  
	  MSSummary mss(ms);
	  
	  mss.list(logio, verbose!=0);
	  ofs << (os.str().c_str()) << endl;
//	  exit(0);
	}
      else if (type=="Image")
      	{
      	  LatticeBase* lattPtr = ImageOpener::openImage (MSNBuf);
      	  ImageInterface<Float> *fImage;
      	  ImageInterface<Complex> *cImage;

      	  fImage = dynamic_cast<ImageInterface<Float>*>(lattPtr);
      	  cImage = dynamic_cast<ImageInterface<Complex>*>(lattPtr);

      	  ostringstream oss;	      
      	  Record miscInfoRec;
      	  if (fImage != 0)
      	    {
      	      logio << "Image data type  : Float" << LogIO::NORMAL;
	      ImageSummary<Float> ims(*fImage);
      	      Vector<String> list = ims.list(logio);
	      ofs << (os.str().c_str()) << endl;
	      miscInfoRec=fImage->miscInfo();
      	    }
      	  else if (cImage != 0)
      	    {
      	      logio << "Image data type  : Complex" << LogIO::NORMAL;
      	      ImageSummary<Complex> ims(*cImage);
      	      Vector<String> list = ims.list(logio);
      	      ofs << (os.str().c_str()) << endl;
      	      miscInfoRec=cImage->miscInfo();
      	    }
      	  else
      	    logio << "Unrecognized image data type." << LogIO::EXCEPTION;

      	  if (miscInfoRec.nfields() > 0)
      	    {
      	      logio << endl << "Attached miscellaneous Information : " << endl << LogIO::NORMAL;
      	      miscInfoRec.print(oss,25," MiscInfo : ");
      	      logio << oss.str() << LogIO::NORMAL;
      	    }
       	}
    }
  // catch (clError& x)
  //   {
  //     x << x.what() << endl;
  //     restartUI=True;
  //   }
  // //
  // // Catch any exception thrown by AIPS++ libs.  Do your cleanup here
  // // before returning to the UI (if you choose to).  Without this, all
  // // exceptions (AIPS++ or otherwise) are caught in the default
  // // exception handler clDefaultErrorHandler (installed by the CLLIB).
  // //
  // catch (AipsError& x)
  //   {
  //     cerr << "###AipsError: " << x.getMesg() << endl;
  //     restartUI=True;
  //     //      exit(0);
  //   }
  // if (restartUI) RestartUI(RENTER);
}
