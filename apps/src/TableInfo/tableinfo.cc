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


#include <tableinfo.h>
#include <casacore/casa/IO/AipsIO.h>
using namespace casacore;

//
//-------------------------------------------------------------------------

void TableInfo_func(const string& MSNBuf,
		    const string& OutBuf,
		    const bool& verbose)
{
  //
  //---------------------------------------------------
  //

  try
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
      else
	{
	  Record rec;
	  try {
	    AipsIO rrfile;
	    rrfile.open(MSNBuf, ByteIO::Old);
	    rrfile >> rec;
	  } catch (AipsError &) {
	    try {
	      Table tab(MSNBuf, Table::Update);
	      rec = tab.keywordSet().asRecord("record");
	    } catch (AipsError &) {
	      Table tab(MSNBuf, Table::Old);
	      rec = tab.keywordSet().asRecord("record");
	    }
	  }
	  rec.print(cerr);
	  cerr << "------------------------------" << endl;
	}
    }
  // catch (clError& x)
  //   {
  //     x << x.what() << endl;
  //     //      restartUI=True;
  //   }
  //
  // Catch any exception thrown by AIPS++ libs.  Do your cleanup here
  // before returning to the UI (if you choose to).  Without this, all
  // exceptions (AIPS++ or otherwise) are caught in the default
  // exception handler clDefaultErrorHandler (installed by the CLLIB).
  //
  catch (AipsError& x)
    {
      cerr << "###AipsError: " << x.getMesg() << endl;
      //      restartUI=True;
      //      exit(0);
    }
  // if (restartUI) RestartUI(RENTER);
}
