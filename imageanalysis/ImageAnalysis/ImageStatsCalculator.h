//# tSubImage.cc: Test program for class SubImage
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
//# $Id: tSubImage.cc 20567 2009-04-09 23:12:39Z gervandiepen $

#ifndef IMAGEANALYSIS_IMAGESTATSCALCULATOR_H
#define IMAGEANALYSIS_IMAGESTATSCALCULATOR_H

#include <imageanalysis/ImageAnalysis/ImageStatsBase.h>

#include <casa/namespace.h>

namespace casacore{

template <class T> class CountedPtr;
}

namespace casa {

template <class T> class ImageStatsCalculator: public ImageStatsBase<T> {
    // <summary>
    // Top level class used for statistics calculations
    // </summary>

    // <reviewed reviewer="" date="" tests="" demos="">
    // </reviewed>

    // <prerequisite>
    // </prerequisite>

    // <etymology>
    // Image statistics calculator
    // </etymology>

    // <synopsis>
    // Top level class used for statistics calculations
    // </synopsis>

public:

    ImageStatsCalculator(
        const SPCIIT image, const casacore::Record *const &regionPtr,
        const casacore::String& maskInp,
        casacore::Bool beVerboseDuringConstruction=false
    );

    ~ImageStatsCalculator();

    casacore::Record calculate();

    void forceNewStorage() { this->_resetStats(); }

    inline casacore::String getClass() const {return _class;}

    inline void setAxes(const casacore::Vector<casacore::Int>& axes) {
        _axes.assign(axes); casacore::GenSort<casacore::Int>::sort(_axes);
    }

    void setDisk(casacore::Bool d);

    // Set range of pixel values to include in the
    // calculation. Should be a two element Vector
    void setIncludePix(const casacore::Vector<T>& inc) {
        _includepix.assign(inc);
    }

    // Set range of pixel values to exclude from the
    // calculation. Should be a two element Vector
    void setExcludePix(const casacore::Vector<T>& exc) {
        _excludepix.assign(exc);
    }

    // casacore::List stats to logger? If you want no logging you should set this to false in addition to
    // calling setVerbosity()
    inline void setList(casacore::Bool l) {_list = l;}

    void setRobust(casacore::Bool r);

    void setVerbose(casacore::Bool v);

    // moved from ImageAnalysis
    // if messageStore != 0, log messages, stripped of time stampe and priority, will also
    // be placed in this parameter and returned to caller for eg logging to file.
    casacore::Record statistics(
         std::vector<casacore::String> *const &messageStore=0
    );

    const static String SIGMA;

protected:

       CasacRegionManager::StokesControl _getStokesControl() const {
           return CasacRegionManager::USE_ALL_STOKES;
       }

    std::vector<OutputDestinationChecker::OutputStruct> _getOutputStruct() {
        return std::vector<OutputDestinationChecker::OutputStruct>(0);
    }

    std::vector<casacore::Coordinate::Type> _getNecessaryCoordinates() const {
        return std::vector<casacore::Coordinate::Type>(0);
    }

    casacore::Bool _hasLogfileSupport() const { return true; }

    inline casacore::Bool _supportsMultipleRegions() const {return true;}

private:
    casacore::CountedPtr<casacore::ImageRegion> _oldStatsRegion = nullptr;
    casacore::CountedPtr<casacore::ImageRegion> _oldStatsMask = nullptr;
    casacore::Vector<casacore::Int> _axes = casacore::Vector<casacore::Int>();
    casacore::Vector<T> _includepix = casacore::Vector<T>();
    casacore::Vector<T> _excludepix = casacore::Vector<T>();
    casacore::Bool _list = casacore::False;
    casacore::Bool _disk = casacore::False;
    casacore::Bool _robust = casacore::False;
    casacore::Bool _verbose = casacore::False;
    std::shared_ptr<const casacore::SubImage<T>> _subImage = nullptr;
    static const casacore::String _class;

    // moved from ImageAnalysis
    // See if the combination of the 'region' and
    // 'mask' ImageRegions have changed
    static casacore::Bool _haveRegionsChanged (
        casacore::ImageRegion* newRegion,
        casacore::ImageRegion* newMask,
        casacore::ImageRegion* oldRegion,
        casacore::ImageRegion* oldMask
    );

    void _logStartup(
        std::vector<String> *const &messageStore, const String& alg,
        const casacore::IPosition& blc, const casacore::IPosition& trc,
        const casacore::String& blcf, const casacore::String trcf
    ) const;

    void _reportDetailedStats(
        const std::shared_ptr<const casacore::ImageInterface<T>> tempIm,
        const casacore::Record& retval
    );

    template <class U> void _removePlanes(
        Array<U>& arr, uInt axis, const std::set<uInt>& planes
    ) const;

    // remove values for which there were no points, CAS-10183
    void _sanitizeDueToRegionSelection(Record& retval) const;

};

}

#ifndef AIPS_NO_TEMPLATE_SRC
#include <imageanalysis/ImageAnalysis/ImageStatsCalculator.tcc>
#endif

#endif
