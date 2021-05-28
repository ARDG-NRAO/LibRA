//# AsdmIndex.cc: Index for the ASDM Storage Manager
//# Copyright (C) 2012
//# Associated Universities, Inc. Washington DC, USA.
//# (c) European Southern Observatory, 2012
//# Copyright by ESO (in the framework of the ALMA collaboration)
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have receied a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id: AsdmColumn.h 19324 2011-11-21 07:29:55Z diepen $

//# Includes
#include <asdmstman/AsdmIndex.h>
#include <tables/DataMan/DataManError.h>
#include <casa/IO/AipsIO.h>

using namespace casacore;
namespace casa {

  uInt AsdmIndex::dataSize() const
  {
    int polSize;
    switch (dataType) {
    case 0:
      //polSize = 2*2*nPol;    
      polSize = 2*2;  // cross complex short
      break;
    case 1:
      //polSize = 2*4*nPol;
      polSize = 2*4;    // cross complex int
      break;
    case 3:
      //polSize = 2*4*nPol;
      polSize = 2*4;    // cross complex float
      break;
    case 10:
      // For autocorr, the nr of pol values is special (see getAuto).
      /*
      if (nPol == 3) {
        polSize = 4*4;
      } else if (nPol == 4) {
        polSize = 4*6;
      } else {
        polSize = 4*nPol;
      }
      */
      polSize = 4;
      break;
    default:
      throw DataManError ("AsdmStMan: unknown datatype " +
                          String::toString(dataType));
    }
    return nBl * stepBl * polSize;
  }

  AipsIO& operator<< (AipsIO& os, const AsdmIndex& ix)
  {
    // This is version 1 of the index.
    os << Short(1) << ix.dataType << ix.fileNr
       << ix.nBl << ix.nSpw << ix.nChan << ix.nPol
       << ix.stepBl << ix.iSpw << ix.row << ix.fileOffset << ix.blockOffset;
    os.put (ix.scaleFactors);
    return os;
  }

  AipsIO& operator>> (AipsIO& os, AsdmIndex& ix)
  {
    Short version;
    os >> version >> ix.dataType >> ix.fileNr;
    os >> ix.nBl >> ix.nSpw >> ix.nChan >> ix.nPol
       >> ix.stepBl >> ix.iSpw >> ix.row >> ix.fileOffset >> ix.blockOffset;
    os.get (ix.scaleFactors);
    return os;
  }

  std::ostream& operator<<(std::ostream& os, const AsdmIndex& ix) {
    cout << Short(1)
	 <<":"  << ix.dataType
	 << ":" << ix.fileNr
	 << ":" << ix.nBl
	 << ":" << ix.nSpw
	 << ":" << ix.nChan
	 << ":" << ix.nPol
	 << ":" << ix.stepBl
	 << ":" << ix.iSpw
	 << ":" << ix.row
	 << ":" << ix.fileOffset
	 << ":" << ix.blockOffset;
    cout << ":";
    for (unsigned int i = 0; i < ix.scaleFactors.size(); i++)
      cout << ix.scaleFactors[i] << " ";
    
    return os;
  }


} //# end namespace
