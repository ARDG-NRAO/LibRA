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


#include <chip.h>
#include <Utilities/utils.h>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <unistd.h>
#include <casacore/casa/OS/DirectoryIterator.h>
#include <casacore/casa/OS/File.h>
#include <casacore/casa/OS/Path.h>


//
//-------------------------------------------------------------------------
using namespace utils;
using namespace std;
namespace Chip
{
  void chip(std::vector<std::string>& imageName, std::string& outputImage,
	    const bool overWrite,
	    const bool resetOutputImage,
	    const string& stats)
  {
    //
    //---------------------------------------------------
    //
    // String type, tableName, targetName, weightName, sumwtName, pbName;
    // String subType;
    // bool isValidGather, isGatherPSF, isGatherResidual;

    LogIO logio(LogOrigin("Chip","chip"));

    // Reset target image if an existing image has to overwritten by
    // the accumulated images.
    bool reset_target=overWrite && resetOutputImage;
    
    if (reset_target) logio << "Output image will be reset" << LogIO::POST;
    logio << "Gathering " << imageName.size() << LogIO::POST;
    for(auto name : imageName) logio << name << " "; logio << LogIO::POST;

    //
    // Some obvious error checks to save users from themselves
    //
    try
      {
	if (utils::imageExists(outputImage) &&  (!overWrite))
	  logio << "Output image exists.  Won't overwrite it." << LogIO::EXCEPTION;

	// Cost of this loop scales as imageName.size().  Can this become a performance bottl%eneck?
	for(auto name: imageName)
	  {
	    Table table(name,TableLock(TableLock::AutoNoReadLocking));
	    TableInfo& info = table.tableInfo();
	    string type=info.type();
	    string subType = info.subType();
	    if (type != "Image")
	      logio << name+string(" is not an image!") << LogIO::EXCEPTION;
	  }
	//
	// By this place in the program, all user error should be taken
	// care of, hopefully.
	//

	CoordinateSystem imCSys;
	IPosition imShape;
	CountedPtr<PagedImage<float>> targetImage;

	// Construct the target image based on the first image in the
	// imageName list if overWrite=false.  Else expect and open
	// existing output image.
	if (!overWrite)
	  {
	    // In a sperate scope so that the image does not hang around
	    // in the RAM
	    PagedImage<float> partImage(imageName[0]);
	    imCSys = partImage.coordinates();
	    imShape = partImage.shape();
	    targetImage = new PagedImage<float>(imShape,imCSys,outputImage);
	  }
	else
	  targetImage=new PagedImage<float>(outputImage);
	
	vector<string> inputImageNames=imageName;
	string imageExt("");
	vector<string> subVec={imageName[0]};
	utils::addImages(*targetImage,
			     subVec,
			     imageExt,
			     reset_target,
			     logio);

	// Add the rest of the images without resetting the target
	// image.
	reset_target=false;
	imageName.erase(imageName.begin());
	utils::addImages(*targetImage,
			     imageName,
			     imageExt,
			     reset_target,
			     logio);
	if (stats=="outputonly")
	  utils::printImageMax(outputImage, *targetImage, logio);
	else if (stats=="inputonly")
	  utils::printImageMax(inputImageNames, logio);
	else if (stats=="all")
	  {
	    utils::printImageMax(inputImageNames, logio);
	    utils::printImageMax(outputImage, *targetImage, logio);
	  }
      }
    catch(AipsError& e)
      {
	logio << e.what() << LogIO::EXCEPTION;
      }
  }
};
