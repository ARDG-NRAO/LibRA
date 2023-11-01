//#
//#  CASA - Common Astronomy Software Applications (http://casa.nrao.edu/)
//#  Copyright (C) Associated Universities, Inc. Washington DC, USA 2011, All rights reserved.
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

#include <mstransform/MSTransform/StatWtColConfig.h>

#include <casacore/tables/DataMan/TiledShapeStMan.h>
#include <casacore/tables/Tables/ArrColDesc.h>
#include <casacore/tables/Tables/ArrayColumn.h>

#include <msvis/MSVis/IteratingParameters.h>
#include <msvis/MSVis/LayeredVi2Factory.h>
#include <stdcasa/variant.h>

using namespace casacore;
using namespace casac;

namespace casa { 

StatWtColConfig::StatWtColConfig(
    casacore::MeasurementSet* ms, casacore::MeasurementSet* selMS, Bool preview,
    const String& dataColumn, const variant& chanbin
) : _ms(ms), _selMS(selMS), _preview(preview), _dataColumn(dataColumn) {
    ThrowIf(_dataColumn.empty(), "data column cannot be empty");
    _dataColumn.downcase();
    ThrowIf (
        ! (
            _dataColumn.startsWith("c")
            || _dataColumn.startsWith("d")
            || _dataColumn.startsWith("residual")
            || _dataColumn.startsWith("residual_")
        ),
        "Unsupported value for data column: " + dataColumn
    );
    _possiblyWriteSigma = _dataColumn.startsWith("d")
        || _dataColumn.startsWith("residual_");
    auto chanBinType = chanbin.type();
    ThrowIf(
        (chanBinType == variant::BOOLVEC && ! chanbin.toBoolVec().empty())
        && chanBinType != variant::STRING && chanBinType != variant::INT,
        "Unsupported data type for chanbin"
    );
    _doChanBin = (
            chanBinType == variant::STRING && chanbin.toString() != "spw"
        ) || chanBinType == variant::INT;
    _determineFlags();
    _initSpecColsIfNecessary();
}

StatWtColConfig::~StatWtColConfig() {}

void StatWtColConfig::getColWriteFlags(
    casacore::Bool& mustWriteWt, casacore::Bool& mustWriteWtSp,
    casacore::Bool& mustWriteSig, casacore::Bool& mustWriteSigSp
) const {
    mustWriteWt = _mustWriteWt;
    mustWriteWtSp = _mustWriteWtSp;
    mustWriteSig = _mustWriteSig;
    mustWriteSigSp = _mustWriteSigSp;
}

void StatWtColConfig::_determineFlags() {
    _mustWriteSig = _possiblyWriteSigma && ! _preview;
    Bool hasSigSp = False;
    Bool sigSpIsInitialized = False;
    _hasSpectrumIsSpectrumInitialized(
        hasSigSp, sigSpIsInitialized, MS::SIGMA_SPECTRUM
    );
    _mustWriteSigSp = _mustWriteSig && (sigSpIsInitialized || _doChanBin);
    _mustWriteWt = ! _preview
        && (
            ! _mustWriteSig
            || (
                _mustWriteSig
                && ! _ms->isColumn(MSMainEnums::CORRECTED_DATA)
            )
        );
    Bool hasWtSp = False;
    Bool wtSpIsInitialized = False;
    _hasSpectrumIsSpectrumInitialized(
        hasWtSp, wtSpIsInitialized, MS::WEIGHT_SPECTRUM
    );
    _mustWriteWtSp = _mustWriteWt && (wtSpIsInitialized || _doChanBin);
    static const auto colNameWtSp = MS::columnName(MS::WEIGHT_SPECTRUM);
    static const auto descWtSp = "weight spectrum";
    static const auto mgrWtSp = "TiledWgtSpectrum";
    _dealWithSpectrumColumn(
        hasWtSp, _mustWriteWtSp, _mustInitWtSp, _mustWriteWt,
        colNameWtSp, descWtSp, wtSpIsInitialized, mgrWtSp
    );
    static const auto colNameSigSp = MS::columnName(MS::SIGMA_SPECTRUM);
    static const auto descSigSp = "sigma spectrum";
    static const auto mgrSigSp = "TiledSigSpectrum";
    _dealWithSpectrumColumn(
        hasSigSp, _mustWriteSigSp, _mustInitSigSp, _mustWriteSig,
        colNameSigSp, descSigSp, sigSpIsInitialized, mgrSigSp
    );
    LogIO log(LogOrigin("StatWtColConfig", __func__));
    if (_mustWriteWt) {
        if (_mustWriteSig) {
            log << LogIO::NORMAL
                << "CORRECTED_DATA is not present. Updating the "
                << "SIGMA/SIGMA_SPECTRUM and WEIGHT/WEIGHT_SPECTRUM values "
                << "based on calculations using the DATA column."
                << LogIO::POST;
        }
        else {
            log << LogIO::NORMAL
                << "Updating the WEIGHT/WEIGHT_SPECTRUM values. SIGMA/SIGMA_SPECTRUM "
                << "values will not be recalculated as they are related to the values "
                << "in the DATA column." << LogIO::POST;
        }
    }
    else if (_mustWriteSig) {
        log << LogIO::NORMAL
            << "Updating the SIGMA/SIGMA_SPECTRUM values. WEIGHT/WEIGHT_SPECTRUM will "
            << "not be recalculated as they are related to the values in the "
            << "CORRECTED_DATA column." << LogIO::POST;
    }
}

void StatWtColConfig::_initSpecColsIfNecessary() {
    if (! _mustInitWtSp && ! _mustInitSigSp) {
        return;
    }
    LogIO log(LogOrigin("StatWtColConfig", __func__));
    if (_mustInitWtSp) {
        log << LogIO::NORMAL
            << "Fully initializing WEIGHT_SPECTRUM column"
            << LogIO::POST;
    }
    if (_mustInitWtSp) {
        log << LogIO::NORMAL
            << "Fully initializing SIGMA_SPECTRUM column"
            << LogIO::POST;
    }
    std::vector<Int> scs;
    scs.push_back(MS::ARRAY_ID);
    scs.push_back(MS::DATA_DESC_ID);
    scs.push_back(MS::TIME);
    Block<int> sort(scs.size());
    uInt i = 0;
    for (const auto& col: scs) {
        sort[i] = col;
        ++i;
    }
    vi::SortColumns sc(sort, False);
    vi::IteratingParameters ipar;
    for (uInt i=0; i<2; ++i) {
        if (i == 1 && _ms == _selMS) {
            break;
        }
        // FIXME, in some cases, this still doesn't add the needed columns
        // to _selMS
        MeasurementSet* ms = i == 0 ? _ms : _selMS;
        vi::VisIterImpl2LayerFactory mydata(ms, ipar, True);
        Vector<vi::ViiLayerFactory*> facts(1);
        facts[0] = &mydata;
        vi::VisibilityIterator2 vi(facts);
        vi::VisBuffer2 *vb = vi.getVisBuffer();
        for (vi.originChunks(); vi.moreChunks(); vi.nextChunk()) {
            for (vi.origin(); vi.more(); vi.next()) {
                auto nrow = vb->nRows();
                auto nchan = vb->nChannels();
                auto ncor = vb->nCorrelations();
                if (_mustInitWtSp) {
                    Cube<Float> newsp(ncor, nchan, nrow, 0);
                    _setEqual(newsp, vb->weight());
                    vb->initWeightSpectrum(newsp);
                }
                if (_mustInitSigSp) {
                    Cube<Float> newsp(ncor, nchan, nrow, 0);
                    _setEqual(newsp, vb->sigma());
                    vb->initSigmaSpectrum(newsp);
                }
                vb->writeChangesBack();
                if (_mustInitWtSp) {
                    AlwaysAssert(vb->weightSpectrum().size() > 0, AipsError);
                }
                if (_mustInitSigSp) {
                    AlwaysAssert(vb->sigmaSpectrum().size() > 0, AipsError);
                }
            }
        }
    }
}

void StatWtColConfig::_setEqual(
    Cube<Float>& newsp, const Matrix<Float>& col
) {
    IPosition start(3, 0);
    auto shape = newsp.shape();
    auto end = shape - 1;
    for (Int corr=0; corr<shape[0]; ++corr) {
        for (Int row=0; row<shape[2]; ++row) {
            start[0] = corr;
            end[0] = corr;
            start[2] = row;
            end[2] = row;
            newsp(start, end) = col(corr, row);
        }
    }
}

void StatWtColConfig::_hasSpectrumIsSpectrumInitialized(
    bool& hasSpectrum, bool& spectrumIsInitialzed,
    MS::PredefinedColumns col
) const {
    hasSpectrum = _ms->isColumn(col);
    if (! hasSpectrum) {
        // no column, so it is obviously not initialized
        spectrumIsInitialzed = False;
        return;
    }
    ArrayColumn<Float> column(*_ms, MS::columnName(col));
    try {
        column.get(0);
        // we were able to get a row, so its initialized.
        spectrumIsInitialzed = True;
    }
    catch (const AipsError& x) {
        // attempt to get first row failed, its not initialized.
        spectrumIsInitialzed = False;
    }
}

void StatWtColConfig::_dealWithSpectrumColumn(
    Bool& hasSpec, Bool& mustWriteSpec, Bool& mustInitSpec,
    Bool mustWriteNonSpec, const String& colName, const String& descName,
    Bool specIsInitialized, const String& mgrName
) {
    // this conditional structure supports the
    // case of ! hasSpec && ! mustWriteSpec, in which case,
    // nothing need be done
    if (! hasSpec) {
        if (mustWriteSpec) {
            // we must create spectrum column
            hasSpec = True;
            mustInitSpec = True;
            // from Calibrater.cc
            // Nominal default tile shape
            IPosition dts(3, 4, 32, 1024);
            // Discern DATA's default tile shape and use it
            const auto dminfo = _ms->dataManagerInfo();
            for (uInt i=0; i<dminfo.nfields(); ++i) {
                Record col = dminfo.asRecord(i);
                if (anyEQ(col.asArrayString("COLUMNS"), String("DATA"))) {
                    dts = IPosition(
                        col.asRecord("SPEC").asArrayInt("DEFAULTTILESHAPE")
                    );
                    break;
                }
            }
            // Add the column
            String colWtSp = colName;
            TableDesc tdWtSp;
            tdWtSp.addColumn(ArrayColumnDesc<Float>(colWtSp, descName, 2));
            TiledShapeStMan wtSpStMan(mgrName, dts);
            _ms->addColumn(tdWtSp, wtSpStMan);
        }
    }
    else if (mustWriteNonSpec) {
        if (specIsInitialized) {
            // it's initialized, so even if we are using the full
            // spw for binning, we still need to update *_SPECTRUM
            mustWriteSpec = True;
        }
        else {
            // it's not initialized, so we aren't going to write to it unless
            // chanbin has been specified to be less than the spw width
            mustInitSpec = mustWriteSpec;
        }
    }
}

}
