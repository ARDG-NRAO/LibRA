//# Copyright (C) 1995,1996,1997,1998,1999,2000,2001,2002
//# Associated Universities, Inc. Washington DC, USA.
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
//# You should have received a copy of the GNU Library General Public License
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

#ifndef IMAGEANALYSIS_IMAGEHISTOGRAMSCALCULATOR_TCC
#define IMAGEANALYSIS_IMAGEHISTOGRAMSCALCULATOR_TCC

#include <imageanalysis/ImageAnalysis/ImageHistogramsCalculator.h>

#include <imageanalysis/ImageAnalysis/ImageHistograms.h>

using namespace casacore;
namespace casa {

template <class T> const String ImageHistogramsCalculator<T>::CLASS_NAME
    = "ImageHistogramsCalculator";

template <class T> ImageHistogramsCalculator<T>::ImageHistogramsCalculator(
	const SPCIIT image, const Record *const &region,
	const String& mask
) : ImageTask<T>(
        image, "", region, "", "", "", mask, "", false
    ) {
	this->_construct(true);
}

template <class T> ImageHistogramsCalculator<T>::~ImageHistogramsCalculator() {}

template <class T> Record ImageHistogramsCalculator<T>::compute() const {
    auto log = this->_getLog();
    *log << LogOrigin(getClass(), __func__);
    CountedPtr<ImageRegion> pRegionRegion, pMaskRegion;
    auto image = this->_getImage();
    auto subImage = SubImageFactory<T>::createSubImageRO(
        pRegionRegion, pMaskRegion, *image,
        *this->_getRegion(), this->_getMask(), log.get(),
        AxesSpecifier(), this->_getStretch()
    );
    ImageHistograms<T> histograms(*subImage, *log, true, _disk);
    ThrowIf(
        !histograms.setAxes(Vector<Int>(_axes)), histograms.errorMessage()
    );
    if(
        image->coordinates().hasDirectionCoordinate()
        && image->imageInfo().hasMultipleBeams()
    ) {
        auto dirAxes = image->coordinates().directionAxesNumbers();
        for (auto d: dirAxes) {
            for (auto axis: _axes) {
                if ((Int)axis == d) {
                    *log << LogIO::WARN << "Specified cursor axis " << axis
                         << " is a direction axis and image has per plane "
                         << "beams. Care should be used when interpreting the "
                         << "results." << LogIO::POST;
                    break;
                }
            }
        }
    }
    ThrowIf(! histograms.setNBins(_nbins), histograms.errorMessage());
    Vector<T> const includeRangeV(_includeRange);
    ThrowIf(
        ! histograms.setIncludeRange(includeRangeV), histograms.errorMessage()
    );
    ThrowIf(
        ! histograms.setForm(_doLog10, _cumulative), histograms.errorMessage()
    );
    ThrowIf(
        ! histograms.setStatsList(_listStats), histograms.errorMessage()
    );
    Array<T> values, counts;
    Array<Vector<T>> stats;
    ThrowIf(
        ! histograms.getHistograms(values, counts, stats),
        histograms.errorMessage()
    );
    Array<T> mean(stats.shape());
    Array<T> stddev(stats.shape());
    auto miter = mean.begin();
    auto siter = stddev.begin();
    for (auto& s: stats) {
        *miter = s[LatticeStatsBase::MEAN];
        *siter = s[LatticeStatsBase::SIGMA];
        ++miter;
        ++siter;
    }
    Record rec;
    rec.define("values", values);
    rec.define("counts", counts);
    rec.define("sigma", stddev);
    rec.define("mean", mean);
    return rec;
}

}

#endif


