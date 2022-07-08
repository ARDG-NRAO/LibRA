
//# Copyright (C) 1998,1999,2000,2001,2003
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or (at your option)
//# any later version.
//#
//# This program is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//# more details.
//#
//# You should have received a copy of the GNU General Public License along
//# with this program; if not, write to the Free Software Foundation, Inc.,
//# 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#

#include <imageanalysis/ImageAnalysis/ImageInputProcessor.h>

#include <casacore/casa/Utilities/Sort.h>
#include <iostream>

#include <casacore/images/Images/FITSImage.h>
#include <casacore/images/Images/ImageUtilities.h>
#include <casacore/images/Images/MIRIADImage.h>
#include <casacore/images/Regions/WCBox.h>
#include <imageanalysis/ImageAnalysis/ImageMetaData.h>

#include <casacore/measures/Measures/Stokes.h>

#include <casacore/lattices/LRegions/LCSlicer.h>

using namespace casacore;
namespace casa {

ImageInputProcessor::ImageInputProcessor()
: _log(new LogIO()), _processHasRun(false),
  _nSelectedChannels(0)
{}

ImageInputProcessor::~ImageInputProcessor() {
	delete _log;
}

uInt ImageInputProcessor::nSelectedChannels() const {
	if (! _processHasRun) {
	    *_log << LogOrigin("ImageInputProcessor", __func__);
		*_log << "Programming logic error, ImageInputProcessor::process() must be called "
			<< "before ImageInputProcessor::" << __func__ << "()" << LogIO::EXCEPTION;
	}
	return _nSelectedChannels;
}

String ImageInputProcessor::_stokesFromRecord(
	const Record& region, const CoordinateSystem& csys
) const {
	// FIXME This implementation is incorrect for complex, recursive records
    String stokes = "";
 	if(csys.hasPolarizationCoordinate()) {
 		Int polAxis = csys.polarizationAxisNumber();
 		uInt stokesBegin, stokesEnd;
 		std::unique_ptr<ImageRegion> imreg(ImageRegion::fromRecord(region, ""));
 		Array<Float> blc, trc;
 		Bool oneRelAccountedFor = false;
 		if (imreg->isLCSlicer()) {
 			blc = imreg->asLCSlicer().blc();
 			trc = imreg->asLCSlicer().trc();
	 		stokesBegin = (uInt)((Vector<Float>)blc)[polAxis];
	 		stokesEnd = (uInt)((Vector<Float>)trc)[polAxis];
	 		oneRelAccountedFor = true;
 		}
 		else if (RegionManager::isPixelRegion(*(ImageRegion::fromRecord(region, "")))) {
			region.toArray("blc", blc);
			region.toArray("trc", trc);
	 		stokesBegin = (uInt)((Vector<Float>)blc)[polAxis];
	 		stokesEnd = (uInt)((Vector<Float>)trc)[polAxis];
		}
		else {
			Record blcRec = region.asRecord("blc");
			Record trcRec = region.asRecord("trc");
			stokesBegin = (Int)blcRec.asRecord(
				String("*" + String::toString(polAxis - 1))
			).asDouble("value");
			stokesEnd = (Int)trcRec.asRecord(
				String("*" + String::toString(polAxis - 1))
			).asDouble("value");
		}

 		if (! oneRelAccountedFor && region.isDefined("oneRel") && region.asBool("oneRel")) {
 			stokesBegin--;
 			stokesEnd--;
 		}
 		for (uInt i=stokesBegin; i<=stokesEnd; i++) {
 			stokes += csys.stokesAtPixel(i);
 		}
 	}
 	return stokes;
}

void ImageInputProcessor::_setRegion(
	Record& regionRecord, String& diagnostics,
	const Record* regionPtr
) const {
 	// region record pointer provided
 	regionRecord = *(regionPtr->clone());
 	// set stokes from the region record
 	diagnostics = "used provided region record";
}

String ImageInputProcessor::_pairsToString(const std::vector<uInt>& pairs) const {
	ostringstream os;
	uInt nPairs = pairs.size()/2;
	for (uInt i=0; i<nPairs; i++) {
		os << pairs[2*i] << " - " << pairs[2*i + 1];
		if (i < nPairs - 1) {
			os << "; ";
		}
	}
	return os.str();
}

}
 
