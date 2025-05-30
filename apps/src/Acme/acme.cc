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


#include <acme.h>

#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <unistd.h>
#include <casacore/casa/OS/DirectoryIterator.h>
#include <casacore/casa/OS/File.h>
#include <casacore/casa/OS/Path.h>
#include <librautils/utils.h>


//
//-------------------------------------------------------------------------

namespace Acme
{

  void acme(std::string& imageName, const string& stats)
  {
    //
    //---------------------------------------------------
    //

    LogIO logio(LogOrigin("Acme","acme"));

    
    //
    // Some obvious error checks to save users from themselves
    //
    try
      {

	  {
	    Table table(imageName,TableLock(TableLock::AutoNoReadLocking));
	    TableInfo& info = table.tableInfo();
	    string type = info.type();
	    if (type != "Image")
	    	throw(AipsError(imageName + string(" is not an image!")));
	  }
		//
		// By this place in the program, all user error should be taken
		// care of, hopefully.
		//

		LatticeBase *targetPtr;
		ImageInterface<Float> *targetImage;

	    if (! librautils::imageExists(imageName))
        	throw(AipsError(string("Image ") + imageName + string(" does not exist.")));

		targetPtr = ImageOpener::openImage (imageName);
		targetImage = dynamic_cast<ImageInterface<Float>*>(targetPtr);

		if (stats=="checkamp")
		{
			librautils::checkImageMax(imageName, *targetImage, logio);
		}
		else if (stats=="all")
		{
			logio << "Stats report not yet implemented." << LogIO::POST;
		}
      }
    catch(AipsError& e)
    {
		logio << e.what() << LogIO::POST;
		exit(-1);
    }
  }
};
