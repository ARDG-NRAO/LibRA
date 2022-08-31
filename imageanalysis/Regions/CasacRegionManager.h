//# RegionManager.h: framework independent class that provides 
//# functionality to tool of same name
//# Copyright (C) 2007
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

#ifndef IMAGEANALYSIS_CASACREGIONMANAGER_H
#define IMAGEANALYSIS_CASACREGIONMANAGER_H

#include <casacore/images/Regions/RegionManager.h>

#include <imageanalysis/IO/RegionTextParser.h>

namespace casa {

/**
 * image component class 
 *
 * This is a casa based class to provide the funtionality to the 
 * casacore::RegionManager Tool
 *
 * @author
 * @version 
 **/

class CasacRegionManager: public casacore::RegionManager {

public:
    const static casacore::String ALL;

    enum StokesControl {
        USE_FIRST_STOKES,
        USE_ALL_STOKES
    };

    CasacRegionManager();

    CasacRegionManager(const casacore::CoordinateSystem& csys);

    CasacRegionManager(const CasacRegionManager&) = delete;

    ~CasacRegionManager();

    CasacRegionManager& operator=(const CasacRegionManager&) = delete;

    // convert to a record a region specified by a rectangular directional <src>box</src>,
    // <src>chans</src>, and <src>stokes</src>, or althernatively a pointer to
    // a region record. If box, chans, or stokes is not blank, <src>regionPtr</src> should
    // be null. Processing happens in the following order:
    // 1. if <src>box</src> is not empty it, along with <src>chans</src> and <src>stokes</src>
    // if specified, are used to determine the returned record. In this case <src>stokes</src>
    // is not altered.
    // 2. else if <src>regionPtr</src> is not null, it is used to determine the returned Record.
    // In this case, stokes may be modified to reflect the stokes parameters included in the
    // corresponding region.
    // 3. else if <src>regionName</src> is not empty, the region file of the same is read and
    // used to create the record, or if <src>regionName</src> is of the form "imagename:regionname" the region
    // record is read from the corresponding iamge. In this case, stokes may be modified
    // to reflect the stokes parameters included in the corresponding region.
    // 4. else <src>chans</src> and <src>stokes</src> are used to determine the region casacore::Record, or
    // if not given, the whole <src>imShape</src> is used.
    // <src>box</src> is specified in comma seperated quadruplets representing blc and trc pixel
    // locations, eg "100, 110, 200, 205". <src>stokes</src> is specified as the concatentation of
    // stokes parameters, eg "IQUV". <src>chans</src> is specified in ways supported by CASA, eg
    // "1~10" for channels 1 through 10.

    casacore::Record fromBCS(
        casacore::String& diagnostics, casacore::uInt& nSelectedChannels, casacore::String& stokes,
        const casacore::Record  * const &regionPtr, const casacore::String& regionName,
        const casacore::String& chans, const StokesControl stokesControl,
        const casacore::String& box, const casacore::IPosition& imShape, const casacore::String& imageName="",
        casacore::Bool verbose=true
    );

    casacore::ImageRegion fromBCS(
            casacore::String& diagnostics, casacore::uInt& nSelectedChannels, casacore::String& stokes,
            const casacore::String& chans, const StokesControl stokesControl,
            const casacore::String& box, const casacore::IPosition& imShape
    ) const;

    static casacore::Record regionFromString(
        const casacore::CoordinateSystem& csys, const casacore::String& regionStr,
        const casacore::String& imageName, const casacore::IPosition& imShape,
        casacore::Bool verbose=true
    );

    // Return the range(s) of spectral channels selected by the specification or the
    // region record (Note only one of <src>specification</src> or <src>regionRec</src>
    // may be specified). <src>imShape</src> is not used if <src>specification</src>
    // is in the "new" format (ie contains "range").
    std::vector<casacore::uInt> setSpectralRanges(
        casacore::uInt& nSelectedChannels,
        const casacore::Record *const regionRec, const casacore::IPosition& imShape=casacore::IPosition(0)
    ) const;

    std::vector<casacore::uInt> setSpectralRanges(
        casacore::String specification, casacore::uInt& nSelectedChannels,
        const casacore::IPosition& imShape=casacore::IPosition(0)
    ) const;

private:

    casacore::String _pairsToString(const std::vector<casacore::uInt>& pairs) const;

    std::vector<casacore::uInt> _setPolarizationRanges(
        casacore::String& specification, const casacore::String& firstStokes, const casacore::uInt nStokes,
        const StokesControl stokesControl
    ) const;

    std::vector<casacore::Double> _setBoxCorners(const casacore::String& box) const;

    casacore::ImageRegion _fromBCS(
            casacore::String& diagnostics,
            const std::vector<casacore::Double>& boxCorners, const std::vector<casacore::uInt>& chanEndPts,
            const std::vector<casacore::uInt>& polEndPts, const casacore::IPosition imShape
    ) const;

    static void _setRegion(
        casacore::Record& regionRecord, casacore::String& diagnostics,
        const casacore::Record* regionPtr
    );

    casacore::String _stokesFromRecord(
        const casacore::Record& region, const StokesControl stokesControl, const casacore::IPosition& shape
    ) const;

    void _setRegion(
        casacore::Record& regionRecord, casacore::String& diagnostics,
        const casacore::String& regionName, const casacore::IPosition& imShape,
        const casacore::String& imageName,
        const casacore::String& prependBox,
        const casacore::String& globalOverrideChans,
        const casacore::String& globalStokesOverride,
        casacore::Bool verbose
    );

    std::vector<casacore::uInt> _spectralRangeFromRangeFormat(
        casacore::uInt& nSelectedChannels, const casacore::String& specification,
        const casacore::IPosition& imShape 
    ) const;

    std::vector<casacore::uInt> _spectralRangeFromRegionRecord(
        casacore::uInt& nSelectedChannels, const casacore::Record *const regionRec,
        const casacore::IPosition& imShape
    ) const;

    // does the image support the setting of a two dimensional box(es).
    // If except is true, an exception will be thrown if this image does not
    // support it. If not, false is returned in that case.
    casacore::Bool _supports2DBox(casacore::Bool except) const;

    std::vector<casacore::uInt> _initSpectralRanges(casacore::uInt& nSelectedChannels, const casacore::IPosition& imShape) const;
};

} // casa namespace
#endif

