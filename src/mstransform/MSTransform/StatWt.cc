//#
//#  CASA - Common Astronomy Software Applications (http://casa.nrao.edu/)
//#  Copyright (C) Associated Universities, Inc. Washington DC, USA 2011, All
//#  rights reserved.
//#  Copyright (C) European Southern Observatory, 2011, All rights reserved.
//#
//#  This library is free software; you can redistribute it and/or
//#  modify it under the terms of the GNU Lesser General Public
//#  License as published by the Free software Foundation; either
//#  version 2.1 of the License, or (at your option) any later version.
//#
//#  This library is distributed in the hope that it will be useful,
//#  but WITHOUT ANY WARRANTY, without even the implied warranty of
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//#  Lesser General Public License for more details.
//#
//#  You should have received a copy of the GNU Lesser General Public
//#  License along with this library; if not, write to the Free Software
//#  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
//#  MA 02111-1307  USA

#include <mstransform/MSTransform/StatWt.h>

#include <casacore/casa/Containers/ValueHolder.h>
#include <casacore/casa/Quanta/QuantumHolder.h>
#include <casacore/casa/System/ProgressMeter.h>
#include <casacore/ms/MSOper/MSMetaData.h>
#include <casacore/tables/Tables/ArrColDesc.h>
#include <casacore/tables/Tables/TableProxy.h>
#include <casacore/tables/DataMan/TiledShapeStMan.h>

#include <mstransform/MSTransform/StatWtColConfig.h>
#include <mstransform/TVI/StatWtTVI.h>
#include <mstransform/TVI/StatWtTVILayerFactory.h>
#include <msvis/MSVis/ViImplementation2.h>
#include <msvis/MSVis/IteratingParameters.h>

using namespace casacore;

namespace casa { 

StatWt::StatWt(
    MeasurementSet* ms,
    const StatWtColConfig* const statwtColConfig
) : _ms(ms),
    _saf(), _statwtColConfig(statwtColConfig) {
    ThrowIf(! _ms, "Input MS pointer cannot be NULL");
    ThrowIf(
        ! _statwtColConfig,
        "Input column configuration pointer cannot be NULL"
    );
}

StatWt::~StatWt() {}

void StatWt::setOutputMS(const casacore::String& outname) {
    _outname = outname;
}

void StatWt::setTimeBinWidth(const casacore::Quantity& binWidth) {
    _timeBinWidth = vi::StatWtTVI::getTimeBinWidthInSec(binWidth);
}

void StatWt::setTimeBinWidth(Double binWidth) {
    vi::StatWtTVI::checkTimeBinWidth(binWidth);
    _timeBinWidth = binWidth;
}

void StatWt::setCombine(const String& combine) {
    _combine = downcase(combine);
}

void StatWt::setPreview(casacore::Bool preview) {
    _preview = preview;
}

void StatWt::setTVIConfig(const Record& config) {
    _tviConfig = config;
}

Record StatWt::writeWeights() {
    auto mustWriteWt = False;
    auto mustWriteWtSp = False;
    auto mustWriteSig = False;
    auto mustWriteSigSp = False;
    _statwtColConfig->getColWriteFlags(
        mustWriteWt, mustWriteWtSp, mustWriteSig, mustWriteSigSp
    );
    std::shared_ptr<vi::VisibilityIterator2> vi;
    std::shared_ptr<vi::StatWtTVILayerFactory> factory;
    _constructVi(vi, factory);
    vi::VisBuffer2 *vb = vi->getVisBuffer();
    ProgressMeter pm(0, _ms->nrow(), "StatWt Progress");
    uInt64 count = 0;
    for (vi->originChunks(); vi->moreChunks(); vi->nextChunk()) {
        for (vi->origin(); vi->more(); vi->next()) {
            auto nrow = vb->nRows();
            if (_preview) {
                // just need to run the flags to accumulate
                // flagging info
                vb->flagCube();
            }
            else {
                if (mustWriteWtSp) {
                    auto& x = vb->weightSpectrum();
                    ThrowIf(
                        x.empty(),
                        "WEIGHT_SPECTRUM is only partially initialized. "
                        "StatWt cannot deal with such an MS"
                    );
                    vb->setWeightSpectrum(x);
                }
                if (mustWriteSigSp) {
                    auto& x = vb->sigmaSpectrum();
                    ThrowIf(
                        x.empty(),
                        "SIGMA_SPECTRUM is only partially initialized. "
                        "StatWt2 cannot deal with such an MS"
                    );
                    vb->setSigmaSpectrum(x);
                }
                if (mustWriteWt) {
                    vb->setWeight(vb->weight());
                }
                if (mustWriteSig) {
                    vb->setSigma(vb->sigma());
                }
                vb->setFlagCube(vb->flagCube());
                vb->setFlagRow(vb->flagRow());
                vb->writeChangesBack();
            }
            count += nrow;
            pm.update(count);
        }
    }
    if (_preview) {
        LogIO log(LogOrigin("StatWt", __func__));
        log << LogIO::NORMAL
            << "RAN IN PREVIEW MODE. NO WEIGHTS NOR FLAGS WERE CHANGED."
            << LogIO::POST;
    }
    factory->getTVI()->summarizeFlagging();
    Double mean, variance;
    factory->getTVI()->summarizeStats(mean, variance);
    Record ret;
    ret.define("mean", mean);
    ret.define("variance", variance);
    return ret;
}

void StatWt::_constructVi(
    std::shared_ptr<vi::VisibilityIterator2>& vi,
    std::shared_ptr<vi::StatWtTVILayerFactory>& factory
) const {
    // default sort columns are from MSIter and are ARRAY_ID, FIELD_ID,
    // DATA_DESC_ID, and TIME. I'm adding scan and state because, according to
    // the statwt requirements, by default, scan and state changes should mark
    // boundaries in the weights computation.
    // ORDER MATTERS. Columns are sorted in the order they appear in the vector.
    std::vector<Int> scs;
    scs.push_back(MS::ARRAY_ID);
    scs.push_back(MS::DATA_DESC_ID);
    scs.push_back(MS::TIME);
    if (! _combine.contains("scan")) {
        scs.push_back(MS::SCAN_NUMBER);
    }
    if (! _combine.contains("state")) {
        scs.push_back(MS::STATE_ID);
    }
    if (! _combine.contains("field")) {
        scs.push_back(MS::FIELD_ID);
    }
    Block<int> sort(scs.size());
    uInt i = 0;
    for (const auto& col: scs) {
        sort[i] = col;
        ++i;
    }
    vi::SortColumns sc(sort, False);
    vi::IteratingParameters ipar(_timeBinWidth, sc);
    vi::VisIterImpl2LayerFactory data(_ms, ipar, True);
    std::unique_ptr<Record> config(dynamic_cast<Record*>(_tviConfig.clone()));
    factory.reset(new vi::StatWtTVILayerFactory(*config));
    Vector<vi::ViiLayerFactory*> facts(2);
    facts[0] = &data;
    facts[1] = factory.get();
    vi.reset(new vi::VisibilityIterator2(facts));
}

}
