
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

#include <casa/Utilities/Sort.h>
#include <casa/iostream.h>

#include <images/Images/FITSImage.h>
#include <images/Images/MIRIADImage.h>
#include <images/Regions/WCBox.h>
#include <imageanalysis/ImageAnalysis/ImageMetaData.h>

#include <measures/Measures/Stokes.h>

namespace casa {

template<class T> void ImageInputProcessor::process(
	casacore::Record& regionRecord, casacore::String& diagnostics,
	std::vector<OutputDestinationChecker::OutputStruct> * const outputStruct, casacore::String& stokes,
	SPCIIT image,
	const casacore::Record* regionPtr, const casacore::String& regionName,
	const casacore::String& box, const casacore::String& chans,
	const CasacRegionManager::StokesControl& stokesControl, const casacore::Bool& allowMultipleBoxes,
	const std::vector<casacore::Coordinate::Type> *const &requiredCoordinateTypes, casacore::Bool verbose
) {
	_process(
		regionRecord, diagnostics, outputStruct, stokes,
		image, regionPtr, regionName, box, chans, stokesControl,
		allowMultipleBoxes, requiredCoordinateTypes, verbose
	);
}

template<class T> void ImageInputProcessor::_process(
    casacore::Record& regionRecord,
    casacore::String& diagnostics,
    std::vector<OutputDestinationChecker::OutputStruct> * const outputStruct,
    casacore::String& stokes, SPCIIT image,
    const casacore::Record *const &regionPtr,
    const casacore::String& regionName, const casacore::String& box,
    const casacore::String& chans, const CasacRegionManager::StokesControl& stokesControl,
    const casacore::Bool& allowMultipleBoxes,
    const std::vector<casacore::Coordinate::Type> *const &requiredCoordinateTypes,
    casacore::Bool verbose
) {
	casacore::LogOrigin origin("ImageInputProcessor", __func__);
    *_log << origin;
    if (outputStruct != 0) {
        OutputDestinationChecker::checkOutputs(outputStruct, *_log);

    }
    *_log << origin;
    if (requiredCoordinateTypes) {
    	for (
    		vector<casacore::Coordinate::Type>::const_iterator iter = requiredCoordinateTypes->begin();
    		iter != requiredCoordinateTypes->end(); iter++
    	) {
    		ThrowIf(
    			image->coordinates().findCoordinate(*iter) < 0,
    			"Image " + image->name() + " does not have required coordinate "
				+ casacore::Coordinate::typeToString(*iter)
    		);
    	}
    }
	ImageMetaData<T> md(image);
	_nSelectedChannels = md.nChannels();
	CasacRegionManager regionMgr(image->coordinates());
	regionRecord = regionMgr.fromBCS(
		diagnostics, _nSelectedChannels, stokes,
		regionPtr, regionName, chans,
		stokesControl, box, image->shape(), image->name(),
		verbose
	);
	// FIXME an exception can be incorrectly thrown for a union of two regions
	// in which one defines a single region in direction space and the
	// defines spectral extent
    ThrowIf(
    	! allowMultipleBoxes && regionRecord.isDefined("regions"),
    	"Only a single n-dimensional rectangular region is supported"
    );
    _processHasRun = true;
}

template<class T> void ImageInputProcessor::_setRegion(
	casacore::Record& regionRecord, casacore::String& diagnostics,
	const casacore::ImageInterface<T> *image, const casacore::String& regionName
) const {
	// region name provided
	casacore::File myFile(regionName);
	if (myFile.exists()) {
		casacore::Record *rec = casacore::RegionManager::readImageFile(regionName, "");
		regionRecord = *rec;
		delete rec;
		diagnostics = "Region read from file " + regionName;
	}
	else {

		casacore::ImageRegion imRegion;
		casacore::Regex otherImage("(.*)+:(.*)+");
		try {
			casacore::String imagename;
			if (regionName.matches(otherImage)) {
				casacore::String res[2];
				casacore::split(regionName, res, 2, ":");
				imagename = res[0];
				casacore::PagedImage<casacore::Float> other(imagename);
				imRegion = other.getRegion(res[1]);
			}
			else {
				imRegion = image->getRegion(regionName);
				imagename = image->name();
			}
		    regionRecord = casacore::Record(imRegion.toRecord(""));
		    diagnostics = "Used region " + regionName + " from image "
		    		+ imagename + " table description";
		}
		catch (const casacore::AipsError& x) {
			ThrowCc(
				"Unable to open region file or region table description "
				+ regionName
			);
		}
	}
}
}
 
