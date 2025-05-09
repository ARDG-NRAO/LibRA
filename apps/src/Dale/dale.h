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

#ifndef DALE_H
#define DALE_H

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
namespace Dale
{
	void dale(const std::string& imageName, 
		  const std::string& wtImageName,
		  const std::string& sowImageName,
		  const std::string& normtype,
		  const std::string& imType,
		  const float& pblimit,
		  //const float& psfcutoff,
		  const bool& computePB);
  //const bool& normalize_weight);
}

void UI(bool restart, int argc, char **argv, bool interactive,
        std::string& imageName, 
        std::string& wtImageName, 
        std::string& sowImageName, 
        string& normtype, string& imType,
        float& pblimit, 
        //float& psfcutoff,
        bool& computePB);// bool& normalize_weight);


#endif
