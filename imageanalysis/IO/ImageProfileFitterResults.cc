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
//# $Id: $

#include <imageanalysis/IO/ImageProfileFitterResults.h>

#include <casacore/casa/Arrays/ArrayLogical.h>
#include <casacore/casa/Utilities/Precision.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/coordinates/Coordinates/LinearCoordinate.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>
#include <casacore/coordinates/Coordinates/StokesCoordinate.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/scimath/Mathematics/Combinatorics.h>

#include <imageanalysis/ImageAnalysis/ImageCollapser.h>
#include <imageanalysis/ImageAnalysis/ProfileFitResults.h>
#include <imageanalysis/IO/LogFile.h>

using namespace casacore;
namespace casa {

const String ImageProfileFitterResults::_class = "ImageProfileFitterResults";
const String ImageProfileFitterResults::_CONVERGED = "converged";
const String ImageProfileFitterResults::_SUCCEEDED = "succeeded";
const String ImageProfileFitterResults::_VALID = "valid";

const uInt ImageProfileFitterResults::_nOthers = 2;
const uInt ImageProfileFitterResults::_gsPlane = 0;
const uInt ImageProfileFitterResults::_lsPlane = 1;


ImageProfileFitterResults::ImageProfileFitterResults(
    const std::shared_ptr<LogIO> log, const CoordinateSystem& csysIm,
    const Array<std::shared_ptr<ProfileFitResults> >* const &fitters, const SpectralList& nonPolyEstimates,
    const std::shared_ptr<const SubImage<Float> > subImage, Int polyOrder, Int fitAxis,
    uInt nGaussSinglets, uInt nGaussMultiplets, uInt nLorentzSinglets,
    uInt nPLPCoeffs, uInt nLTPCoeffs,
    Bool logResults, Bool multiFit, const std::shared_ptr<LogFile> logfile,
    const String& xUnit, const String& summaryHeader
) : _logResults(logResults), _multiFit(multiFit),
    _doVelocity(
        fitAxis == subImage->coordinates().spectralAxisNumber()
        && Quantity(1, xUnit).isConform("m/s")
        && subImage->coordinates().spectralCoordinate().restFrequency() > 0
    ), _xUnit(xUnit), _summaryHeader(summaryHeader),
    _nGaussSinglets(nGaussSinglets), _nGaussMultiplets(nGaussMultiplets),
    _nLorentzSinglets(nLorentzSinglets), _nPLPCoeffs(nPLPCoeffs),_nLTPCoeffs(nLTPCoeffs),
    _fitters(fitters),
    _nonPolyEstimates(nonPolyEstimates), _subImage(subImage), _polyOrder(polyOrder),
    _fitAxis(fitAxis), _logfile(logfile), _log(log), _csysIm(csysIm) {}

ImageProfileFitterResults::~ImageProfileFitterResults() {}

Record ImageProfileFitterResults::getResults() const {
    return _results;
}

void ImageProfileFitterResults::createResults() {
    _setResults();
    _resultsToLog();
}

std::unique_ptr<vector<vector<Array<Double> > > > ImageProfileFitterResults::_createPCFArrays() const {
    std::unique_ptr<vector<vector<Array<Double> > > > pcfArrays(
        new vector<vector<Array<Double> > >(
            NGSOLMATRICES, vector<Array<Double> >(_nGaussMultiplets+_nOthers)
        )
    );
    uInt nSubcomps = 0;
    uInt compCount = 0;
    Double fNAN = casacore::doubleNaN();

    Array<Double> blank;
    IPosition fShape = _fitters->shape();
    for (uInt i=0; i<_nGaussMultiplets + _nOthers; i++) {
        if (i == _gsPlane) {
            // gaussian singlets go in position 0
            nSubcomps = _nGaussSinglets;
        }
        else if (i == _lsPlane) {
            // lorentzian singlets go in position 1
            nSubcomps = _nLorentzSinglets;
        }
        else {
            // gaussian multiplets go in positions 2 to 1 + _nGaussianMultiplets
            while (
                _nonPolyEstimates[compCount]->getType() != SpectralElement::GMULTIPLET
            ) {
                compCount++;
            }
            nSubcomps = dynamic_cast<const GaussianMultipletSpectralElement*>(
                    _nonPolyEstimates[compCount]
            )->getGaussians().size();
            compCount++;
        }
        IPosition blankSize(1, nSubcomps);
        blankSize.prepend(fShape);
        blank.resize(blankSize, False);
        blank = fNAN;
        for (uInt k=0; k<NGSOLMATRICES; k++) {
            (*pcfArrays)[k][i] = blank.copy();
        }
    }
    return pcfArrays;
}

vector<String> ImageProfileFitterResults::logSummary(
    uInt nProfiles, uInt nAttempted, uInt nSucceeded, uInt nConverged, uInt nValid
) {
    vector<String> ret;
    *_log << LogOrigin(_class, __func__);
    ostringstream oss;
    oss << "Number of profiles       = " << nProfiles;
    String str = oss.str();
    *_log << LogIO::NORMAL << str << LogIO::POST;
    _writeLogfile(str + "\n", True, False);
    ret.push_back(str);
    oss.str("");
    oss << "Number of fits attempted = " << nAttempted;
    str = oss.str();
    *_log << LogOrigin(_class, __func__);
    *_log << LogIO::NORMAL << str << LogIO::POST;
    _writeLogfile(str + "\n", False, False);
    ret.push_back(str);
    oss.str("");
    oss << "Number succeeded         = " << nSucceeded;
    str = oss.str();
    *_log << LogOrigin(_class, __func__);
    *_log << LogIO::NORMAL << str << LogIO::POST;
    _writeLogfile(str + "\n", False, False);
    ret.push_back(str);
    oss.str("");
    oss << "Number converged         = " << nConverged;
    str = oss.str();
    *_log << LogOrigin(_class, __func__);
    *_log << LogIO::NORMAL << str << LogIO::POST;
    _writeLogfile(str + "\n", False, False);
    ret.push_back(str);
    oss.str("");
    oss << "Number valid             = " << nValid << endl;
    str = oss.str();
    *_log << LogOrigin(_class, __func__);
    *_log << LogIO::NORMAL << str << LogIO::POST;
    _writeLogfile(str + "\n", False, False);
    ret.push_back(str);
    return ret;
}

Bool ImageProfileFitterResults::_setAxisTypes() {
    const CoordinateSystem& csysSub = _subImage->coordinates();

    const DirectionCoordinate *dcoord = csysSub.hasDirectionCoordinate()
        ? &csysSub.directionCoordinate() : 0;
    String directionType = dcoord ? MDirection::showType(dcoord->directionType()) : "";
    const SpectralCoordinate *spcoord = csysSub.hasSpectralAxis()
        ? &csysSub.spectralCoordinate() : 0;
    const StokesCoordinate *polcoord = csysSub.hasPolarizationCoordinate()
        ? &csysSub.stokesCoordinate() : 0;
    Vector<String> axisNames = csysSub.worldAxisNames();
    for (uInt i=0; i<axisNames.size(); i++) {
        axisNames[i].upcase();
    }
    Bool hasLat = False;
    Bool hasLong = False;
    _axisTypes = vector<axisType>(axisNames.size(), NAXISTYPES);
    for (uInt i=0; i<axisNames.size(); i++) {
        if ((Int)i != _fitAxis) {
            if (
                dcoord
                && (axisNames[i].startsWith("RIG") || axisNames[i].startsWith("LONG"))
            ) {
                _axisTypes[i] = LONGITUDE;
                hasLat = True;
            }
            else if (
                dcoord
                && (axisNames[i].startsWith("DEC") || axisNames[i].startsWith("LAT"))
            ) {
                _axisTypes[i] = LATITUDE;
                hasLong = True;
            }
            else if (
                spcoord
                && (axisNames[i].startsWith("FREQ") || axisNames[i].startsWith("VEL"))
            ) {
                _axisTypes[i] = FREQUENCY;
            }
            else if (
                polcoord
                && axisNames[i].startsWith("STO")
            ) {
                _axisTypes[i] = POLARIZATION;
            }
        }
    }
    return hasLat && hasLong;
}

void ImageProfileFitterResults::_marshalFitResults(
    Array<Bool>& attemptedArr, Array<Bool>& successArr,
    Array<Bool>& convergedArr, Array<Bool>& validArr,
    Array<String>& typeMat, Array<Int>& niterArr,
    Array<Int>& nCompArr, std::unique_ptr<vector<vector<Array<Double> > > >& pcfArrays,
    vector<Array<Double> >& plpArrays, vector<Array<Double> >& ltpArrays,
    Bool returnDirection, Array<String>& directionInfo
) {
    IPosition inTileShape = _subImage->niceCursorShape();
    TiledLineStepper stepper (_subImage->shape(), inTileShape, _fitAxis);
    RO_MaskedLatticeIterator<Float> inIter(*_subImage, stepper);
    const CoordinateSystem& csysSub = _subImage->coordinates();
    const DirectionCoordinate *dcoord = csysSub.hasDirectionCoordinate()
        ? &csysSub.directionCoordinate() : 0;
    String directionType = dcoord ? MDirection::showType(dcoord->directionType()) : "";
    const SpectralCoordinate *spcoord = csysSub.hasSpectralAxis()
        ? &csysSub.spectralCoordinate() : 0;
    const StokesCoordinate *polcoord = csysSub.hasPolarizationCoordinate()
        ? &csysSub.stokesCoordinate() : 0;
    IPosition pixel;
    Double increment = fabs(_fitAxisIncrement());
    for (
        inIter.reset();    ! inIter.atEnd(); ++inIter
    ) {
        IPosition pixel = inIter.position();
        std::shared_ptr<const ProfileFitResults> fitter = (*_fitters)(pixel);
        if (! fitter) {
            continue;
        }
        attemptedArr(pixel) = fitter->getList().nelements() > 0;
        successArr(pixel) = fitter->succeeded();
        convergedArr(pixel) = attemptedArr(pixel) && successArr(pixel)
            && fitter->converged();
        validArr(pixel) = convergedArr(pixel) && fitter->isValid();
        _doWorldCoords(
            directionInfo, csysSub, pixel, dcoord, spcoord,
            polcoord, returnDirection, directionType
        );
        if (validArr(pixel)) {
            _processSolutions(
                typeMat, niterArr, nCompArr, pixel, fitter,
                pcfArrays, plpArrays, ltpArrays, increment
            );
        }
    }
}

void ImageProfileFitterResults::_processSolutions(
    Array<String>& typeMat, Array<Int>& niterArr,
    Array<Int>& nCompArr, const IPosition& pixel,
    std::shared_ptr<const ProfileFitResults> fitter,
    std::unique_ptr<vector<vector<Array<Double> > > >& pcfArrays,
    vector<Array<Double> >& plpArrays, vector<Array<Double> >& ltpArrays,
    Double increment
) {
    // mask(pixel) = anyTrue(inIter.getMask());
    niterArr(pixel) = (Int)fitter->getNumberIterations();
    nCompArr(pixel) = (Int)fitter->getList().nelements();
    SpectralList solutions = fitter->getList();
    uInt gCount = 0;
    uInt gmCount = 0;
    uInt lseCount = 0;
    uInt nSolutions = solutions.nelements();
    for (uInt i=0; i<nSolutions; i++) {
        SpectralElement::Types type = solutions[i]->getType();
        IPosition tPos(1, i);
        tPos.prepend(pixel);
        typeMat(tPos) = SpectralElement::fromType(type);
        switch (type) {
        case SpectralElement::POLYNOMIAL:
            break;
        case SpectralElement::GAUSSIAN:
            // allow fall through because gaussians and lorentzians use common code
        case SpectralElement::LORENTZIAN:
        {
            const PCFSpectralElement *pcf = dynamic_cast<
                const PCFSpectralElement*
            >(solutions[i]);
            uInt plane = _lsPlane;
            uInt col = lseCount;
            // if block because we allow case fall through
            if (type == SpectralElement::LORENTZIAN) {
                lseCount++;
            }
            else {
                plane = _gsPlane;
                col = gCount;
                gCount++;
            }
            _insertPCF(
                *pcfArrays, pixel, *pcf, plane, col,
                increment
            );
        }
        break;
        case SpectralElement::GMULTIPLET:
        {
            const GaussianMultipletSpectralElement *gm = dynamic_cast<
                const GaussianMultipletSpectralElement*
            >(solutions[i]);
            const Vector<GaussianSpectralElement> g(gm->getGaussians());
            for (uInt k=0; k<g.size(); k++) {
                _insertPCF(
                    *pcfArrays, pixel, g[k], gmCount+2,
                    k, increment
                );
            }
            gmCount++;
        }
        break;
        case SpectralElement::POWERLOGPOLY:
        {
            Vector<Double> sols = solutions[i]->get();
            Vector<Double> errs = solutions[i]->getError();
            for (uInt j=0; j<_nPLPCoeffs; j++) {
                // here
                IPosition arrIdx(1, j);
                arrIdx.prepend(pixel);
                plpArrays[SPXSOL](arrIdx) = sols[j];
                plpArrays[SPXERR](arrIdx) = errs[j];
            }
        }
        break;
        case SpectralElement::LOGTRANSPOLY:
        {
            auto sols = solutions[i]->get();
            auto errs = solutions[i]->getError();
            for (uInt j=0; j<_nLTPCoeffs; j++) {
                IPosition arrIdx(1, j);
                arrIdx.prepend(pixel);
                ltpArrays[SPXSOL](arrIdx) = sols[j];
                ltpArrays[SPXERR](arrIdx) = errs[j];
            }
        }
        break;
        default:
            ThrowCc(
                "Logic Error: Unhandled Spectral Element type"
            );
            break;
        }
    }
}

void ImageProfileFitterResults::_doWorldCoords(
    Array<String>& directionInfo, const CoordinateSystem& csysSub,
    const IPosition& pixel, const DirectionCoordinate* const &dcoord,
    const SpectralCoordinate * const &spcoord,
    const StokesCoordinate* const &polcoord, Bool returnDirection,
    const String& directionType
) {
    Vector<Double> world;
    if (csysSub.toWorld(world, pixel)) {
        String latitude, longitude;
        for (uInt i=0; i<world.size(); i++) {
            // The Coordinate::format() calls have the potential of modifying the
            // input unit so this needs to be reset at the beginning of the loop
            // rather than outside the loop
            String emptyUnit = "";
            if ((Int)i != _fitAxis) {
                if (_axisTypes[i] == LONGITUDE) {
                    longitude = dcoord->format(emptyUnit, Coordinate::DEFAULT, world[i], 0, True, True);
                    IPosition x(1, LONGITUDE);
                    x.append(pixel);
                    _worldCoords(x) = longitude;
                }
                else if (_axisTypes[i] == LATITUDE) {
                    latitude = dcoord->format(emptyUnit, Coordinate::DEFAULT, world[i], 1, True, True);
                    IPosition x(1, LATITUDE);
                    x.append(pixel);
                    _worldCoords(x) = latitude;
                }
                else if (_axisTypes[i] == FREQUENCY) {
                    IPosition x(1, FREQUENCY);
                    x.append(pixel);
                    _worldCoords(x) = spcoord->format(emptyUnit, Coordinate::DEFAULT, world[i], 0, True, True);
                }
                else if (_axisTypes[i] == POLARIZATION) {
                    IPosition x(1, POLARIZATION);
                    x.append(pixel);
                    _worldCoords(x) = polcoord->format(emptyUnit, Coordinate::DEFAULT, world[i], 0, True, True);
                }
            }
        }
        if (returnDirection) {
            directionInfo(pixel) = directionType + " "
                + longitude + " " + latitude;
        }
    }
    else {
        ThrowCc(
            "Could not convert pixel to world coordinate: "
                + csysSub.errorMessage()
        );
    }
}

void ImageProfileFitterResults::_writeLogfile(const String& str, Bool open, Bool close) {
    if (_logfile.get() != 0) {
        _logfile->write(str, open, close);
    }
}

void ImageProfileFitterResults::_setResults() {
    LogOrigin logOrigin(_class, __func__);
    Double fNAN = casacore::doubleNaN();
    uInt nComps = _nGaussSinglets + _nGaussMultiplets + _nLorentzSinglets;
    if (_polyOrder >= 0) {
        nComps++;
    }
    if (_nPLPCoeffs > 0) {
        nComps++;
    }
    if (_nLTPCoeffs > 0) {
        nComps++;
    }
    IPosition fitterShape = _fitters->shape();
    Array<Bool> attemptedArr(fitterShape, False);
    Array<Bool> successArr(fitterShape, False);
    Array<Bool> convergedArr(fitterShape, False);
    Array<Bool> validArr(fitterShape, False);
    IPosition wcShape(1, (Int)NAXISTYPES);
    wcShape.append(fitterShape);
    _worldCoords = Array<String>(wcShape, String(""));
    Array<Int> niterArr(fitterShape, -1);
    // pfcArrays. Zeroth structure (zeroth vector) index corresponds to
    // solution type (amp, center, etc). First structure (first vector)
    // index corresponds to type of component (Gaussian, Lorentzian, Gaussian
    // multiplet number). Second to n-2 structure (first m Array) indices
    // correspond location in the _fitters array. Final structure index
    // corresponds to the sub component number (eg for multiple singlets or
    // for gaussian multiplet components
    std::unique_ptr<vector<vector<Array<Double> > > > pcfArrays = _createPCFArrays();
    IPosition bShape(1, max(_nPLPCoeffs, _nLTPCoeffs));
    bShape.prepend(fitterShape);
    Array<Double> blank(bShape, fNAN);
    // plpArrays and ltpArrays have the solution type as the zeroth (vector)
    // index. The next n indices (the first n indices of the Array) correspond to the position in the _fitters
    // array. The final index of the structure (also the final index of the Array) corresponds to
    // the componenet number being solved for.
    vector<Array<Double> > plpArrays(NSPXSOLMATRICES);
    vector<Array<Double> > ltpArrays(NSPXSOLMATRICES);
    for (uInt i=0; i<NSPXSOLMATRICES; i++) {
        // because assignmet of Arrays is by reference, which is horribly confusing
        if (_nPLPCoeffs > 0) {
            plpArrays[i] = blank.copy();
        }
        if (_nLTPCoeffs > 0) {
            ltpArrays[i] = blank.copy();
        }
    }
    IPosition typeShape(1, nComps);
    typeShape.prepend(fitterShape);
    Array<String> typeArr(typeShape, String("UNDEF"));
    Array<Int> nCompArr(fitterShape, -1);
    Bool returnDirection = _setAxisTypes();
    Array<String> directionInfo(fitterShape);
    _marshalFitResults(
        attemptedArr, successArr,
        convergedArr, validArr,
        typeArr, niterArr,
        nCompArr, pcfArrays, plpArrays, ltpArrays,
        returnDirection, directionInfo
    );
    String key;
    _results.define("attempted", attemptedArr);
    _results.define(_SUCCEEDED, successArr);
    _results.define(_CONVERGED, convergedArr);
    _results.define(_VALID, validArr);
    _results.define("niter", niterArr);
    _results.define("ncomps", nCompArr);
    if (returnDirection) {
        _results.define("direction", directionInfo);
    }
    _results.define("xUnit", _xUnit);
    const String yUnit = _subImage->units().getName();
    _results.define("yUnit", yUnit);
    _results.define( "type", typeArr);
    for (uInt i=0; i<_nGaussMultiplets+_nOthers; ++i) {
        if (i == _gsPlane && _nGaussSinglets == 0) {
            continue;
        }
        else if (i == _lsPlane && _nLorentzSinglets == 0) {
            continue;
        }
        Record rec;
        rec.define("center", (*pcfArrays)[CENTER][i]);
        rec.define("fwhm", (*pcfArrays)[FWHM][i]);
        rec.define("amp", (*pcfArrays)[AMP][i]);
        rec.define("integral", (*pcfArrays)[INTEGRAL][i]);
        rec.define("centerErr", (*pcfArrays)[CENTERERR][i]);
        rec.define("fwhmErr", (*pcfArrays)[FWHMERR][i]);
        rec.define("ampErr", (*pcfArrays)[AMPERR][i]);
        rec.define("integralErr", (*pcfArrays)[INTEGRALERR][i]);
        String description = i == _gsPlane
            ? "Gaussian singlets results"
            : i == _lsPlane
              ? "Lorentzian singlets"
              : "Gaussian multiplet number " + String::toString(i-1) + " results";
        rec.define("description", description);
        _results.defineRecord(_getTag(i), rec);
    }
    if (_nPLPCoeffs > 0) {
        Record rec;
        rec.define("solution", plpArrays[SPXSOL]);
        rec.define("error", plpArrays[SPXERR]);
        _results.defineRecord("plp", rec);
    }
    if (_nLTPCoeffs > 0) {
        Record rec;
        rec.define("solution", ltpArrays[SPXSOL]);
        rec.define("error", ltpArrays[SPXERR]);
        _results.defineRecord("ltp", rec);
    }
}

String ImageProfileFitterResults::_getTag(const uInt i) const {
    return i == _gsPlane
        ? "gs"
        : i == _lsPlane
            ? "ls"
            : "gm" + String::toString(i-2);
}

void ImageProfileFitterResults::_insertPCF(
    vector<vector<Array<Double> > >& pcfArrays,
    const IPosition& pixel, const PCFSpectralElement& pcf,
    const uInt idx, const uInt col,
    const Double increment
) const {
    IPosition x = pixel;
    x.append(IPosition(1, col));
    pcfArrays[CENTER][idx](x) = _centerWorld(pcf, pixel);
    pcfArrays[FWHM][idx](x) = pcf.getFWHM() * increment;
    pcfArrays[AMP][idx](x) = pcf.getAmpl();
    pcfArrays[CENTERERR][idx](x) = pcf.getCenterErr() * increment;
    pcfArrays[FWHMERR][idx](x) = pcf.getFWHMErr() * increment;
    pcfArrays[AMPERR][idx](x) = pcf.getAmplErr();
    pcfArrays[INTEGRAL][idx](x) = pcf.getIntegral() * increment;
    pcfArrays[INTEGRALERR][idx](x) = pcf.getIntegralErr() * increment;
}

Array<Bool> ImageProfileFitterResults::_replicateMask(
    const Array<Bool>& array, Int n
) {
    uInt axis = array.ndim() - 1;
    IPosition newShape = array.shape();
    newShape[axis] = n;
    Array<Bool> extended(newShape);
    IPosition begin(newShape.size(), 0);
    IPosition end = newShape - 1;
    for (Int j=0; j<n; j++) {
        begin[axis] = j;
        end[axis] = j;
        extended(begin, end) = array;
    }
    return extended;
}

void ImageProfileFitterResults::writeImages(Bool someConverged) const {
    Bool writeSolutionImages = (
        ! _centerName.empty() || ! _centerErrName.empty()
        || ! _fwhmName.empty() || ! _fwhmErrName.empty()
        || ! _ampName.empty() || ! _ampErrName.empty()
        || ! _integralName.empty() || ! _integralErrName.empty()
        || ! _plpName.empty() || ! _plpErrName.empty()
        || ! _ltpName.empty() || ! _ltpErrName.empty()
    );
    if (
        ! _multiFit && writeSolutionImages
    ) {
        *_log << LogIO::WARN << "This was not a multi-pixel fit request so solution "
            << "images will not be written" << LogIO::POST;
    }
    if (
        _multiFit && writeSolutionImages
    ) {
        if (
            _nGaussSinglets + _nGaussMultiplets + _nLorentzSinglets + _nPLPCoeffs + _nLTPCoeffs == 0
        ) {
            *_log << LogIO::WARN << "No functions for which solution images are supported were fit "
                << "so no solution images will be written" << LogIO::POST;
        }
        else {
            if (someConverged) {
                IPosition axes(1, _fitAxis);
                ImageCollapser<Float> collapser(
                    _subImage, axes, False, ImageCollapserData::ZERO, String(""), False
                );
                std::shared_ptr<TempImage<Float> > tmp = std::dynamic_pointer_cast<TempImage<Float> >(
                    collapser.collapse()
                );
                ThrowIf(! tmp, "Unable to perform dynamic cast");
                std::shared_ptr<TempImage<Float> > myTemplate(tmp);
                const String yUnit = _subImage->units().getName();
                Array<Bool>    mask(_fitters->shape(), False);
                IPosition inTileShape = _subImage->niceCursorShape();
                TiledLineStepper stepper (_subImage->shape(), inTileShape, _fitAxis);
                RO_MaskedLatticeIterator<Float> inIter(*_subImage, stepper);
                for (
                    inIter.reset();    ! inIter.atEnd(); ++inIter
                ) {
                    mask(inIter.position()) = anyTrue(inIter.getMask());
                }
                _writeImages(myTemplate->coordinates(), mask, yUnit);
            }
            else {
                *_log << LogIO::WARN << "No solutions converged, solution images will not be written"
                    << LogIO::POST;
            }
        }
    }
}

void ImageProfileFitterResults::_writeImages(
    const CoordinateSystem& xcsys,
    const Array<Bool>& mask, const String& yUnit
) const {
    map<String, String> mymap;
    map<String, String> unitmap;
    mymap["center"] = _centerName;
    mymap["centerErr"] = _centerErrName;
    mymap["fwhm"] = _fwhmName;
    mymap["fwhmErr"] = _fwhmErrName;
    mymap["amp"] = _ampName;
    mymap["ampErr"] = _ampErrName;
    mymap["integral"] = _integralName;
    mymap["integralErr"] = _integralErrName;
    unitmap["center"] = _xUnit;
    unitmap["centerErr"] = _xUnit;
    unitmap["fwhm"] = _xUnit;
    unitmap["fwhmErr"] = _xUnit;
    unitmap["amp"] = yUnit;
    unitmap["ampErr"] = yUnit;
    unitmap["integral"] = _xUnit + "." + yUnit;
    unitmap["integralErr"] = _xUnit + "." + yUnit;
    for (uInt i=0; i<_nGaussMultiplets+_nOthers; i++) {
        if (i == _gsPlane && _nGaussSinglets == 0) {
            continue;
        }
        else if (i == _lsPlane && _nLorentzSinglets == 0) {
            continue;
        }
        String id = _getTag(i);
        for (const auto& p : mymap) {
            String imagename = p.second;
            String suffix = i == _gsPlane
                ? ""
                : i == _lsPlane
                  ? "_ls"
                  : _nGaussMultiplets <= 1
                    ? "_gm"
                    : "_gm" + String::toString(i-_nOthers);
            imagename += suffix;
            if (! p.second.empty()) {
                _makeSolutionImages(
                    imagename, xcsys,
                    _results.asRecord(id).asArrayDouble(p.first),
                    unitmap.find(p.first)->second, mask
                );
            }
        }
    }
    if (
        (_nPLPCoeffs > 0 && (! _plpName.empty() || ! _plpErrName.empty()))
        || (_nLTPCoeffs > 0 && (! _ltpName.empty() || ! _ltpErrName.empty()))
    ) {
        String type = _results.isDefined("plp") ? "plp" : "ltp";
        if (_nPLPCoeffs > 0 && ! _plpName.empty()) {
            _makeSolutionImages(
                _plpName, xcsys,
                _results.asRecord("plp").asArrayDouble("solution"),
                "", mask
            );
        }
        if (_nPLPCoeffs > 0 && ! _plpErrName.empty()) {
            _makeSolutionImages(
                _plpErrName, xcsys,
                _results.asRecord("plp").asArrayDouble("error"),
                "", mask
            );
        }
        if (_nLTPCoeffs > 0 && ! _ltpName.empty()) {
            _makeSolutionImages(
                _ltpName, xcsys,
                _results.asRecord("ltp").asArrayDouble("solution"),
                "", mask
            );
        }
        if (_nLTPCoeffs > 0 && ! _ltpErrName.empty()) {
            _makeSolutionImages(
                _ltpErrName, xcsys,
                _results.asRecord("ltp").asArrayDouble("error"),
                "", mask
            );
        }
    }
}
/*
Bool ImageProfileFitterResults::_inVelocitySpace() const {
    return _fitAxis == _subImage->coordinates().spectralAxisNumber()
        && Quantity(1, _xUnit).isConform("m/s")
        && _subImage->coordinates().spectralCoordinate().restFrequency() > 0;
}
*/

Double ImageProfileFitterResults::_fitAxisIncrement() const {
    if (_doVelocity) {
        Vector<Double> pixels(2);
        pixels[0] = 0;
        pixels[1] = 1;
        Vector<Double> velocities(2);
        _subImage->coordinates().spectralCoordinate().pixelToVelocity(
            velocities, pixels
        );
        return velocities[1] - velocities[0];
    }
    else {
        return _subImage->coordinates().increment()[_fitAxis];
    }
}

const Vector<Double> ImageProfileFitterResults::getPixelCenter(uint index) const {
    Vector<Double> pos;
    if ( index < _pixelPositions.size()) {
        pos = _pixelPositions[index];
    }
    return pos;
}

Double ImageProfileFitterResults::_centerWorld(
    const PCFSpectralElement& solution, const IPosition& imPos
) const {
    Vector<Double> pixel(imPos.size());
    for (uInt i=0; i<pixel.size(); i++) {
        pixel[i] = imPos[i];
    }
    Vector<Double> world(pixel.size());
    // in pixels here
    pixel[_fitAxis] = solution.getCenter();
    _subImage->coordinates().toWorld(world, pixel);
    if (_doVelocity) {
        Double velocity;
        _subImage->coordinates().spectralCoordinate().frequencyToVelocity(velocity, world(_fitAxis));
        return velocity;
    }
    else {
        return world(_fitAxis);
    }
}

void ImageProfileFitterResults::_resultsToLog() {
    if (! _logResults && _logfile.get() == 0) {
        return;
    }
    ostringstream summary;
    summary << "****** Fit performed at " << Time().toString() << "******" << endl << endl;
    summary << _summaryHeader;
    if (_nPLPCoeffs + _nLTPCoeffs == 0) {
        if (_goodAmpRange.size() == 2) {
            summary << "       --- valid amplitude range: " << _goodAmpRange << endl;
        }
        if (_goodCenterRange.size() == 2) {
            summary << "       --- valid center range: " << _goodCenterRange << endl;
        }
        if (_goodFWHMRange.size() == 2) {
            summary << "       --- valid FWHM range: " << _goodFWHMRange << endl;
        }
        summary << "       --- number of Gaussian singlets: " << _nGaussSinglets << endl;
        summary << "       --- number of Gaussian multiplets: " << _nGaussMultiplets << endl;
        if (_nGaussMultiplets > 0) {
            for (uInt i=0; i<_nGaussMultiplets; i++) {
                Array<Double> amp = _results.asRecord("gm" + String::toString(i)).asArrayDouble(AMP);
                uInt n = amp.shape()[amp.ndim()-1];
                summary << "           --- number of components in Gaussian multiplet "
                    << i << ": " << n << endl;
            }
        }
        if (_polyOrder >= 0) {
            summary << "       --- polynomial order:    " << _polyOrder << endl;
        }
        else {
            summary << "       --- no polynomial fit " << endl;
        }
    }
    if (_multiFit) {
        summary << "       --- Multiple profiles fit, one per pixel over selected region" << endl;
    }
    else {
        summary << "       --- One profile fit, averaged over several pixels if necessary" << endl;
    }
    if (_logResults) {
        *_log << LogIO::NORMAL << summary.str() << LogIO::POST;
    }
    _writeLogfile(summary.str(), False, False);
    IPosition inTileShape = _subImage->niceCursorShape();
    TiledLineStepper stepper (_subImage->shape(), inTileShape, _fitAxis);
    RO_MaskedLatticeIterator<Float> inIter(*_subImage, stepper);
    CoordinateSystem csysSub = _subImage->coordinates();
    Vector<Double> worldStart;
    ThrowIf(
        ! csysSub.toWorld(worldStart, inIter.position()),
        csysSub.errorMessage()
    );
    Vector<Double> pixStart;
    ThrowIf(
        ! _csysIm.toPixel(pixStart, worldStart),
        _csysIm.errorMessage()
    );
    if (_multiFit) {
        for (uInt i=0; i<pixStart.size(); i++) {
            pixStart[i] = (Int)std::floor( pixStart[i] + 0.5);
        }
    }
    Vector<Double> imPix(pixStart.size());
    Vector<Double> world;
    IPosition subimPos;
    SpectralList solutions;
    String axisUnit = _csysIm.worldAxisUnits()[_fitAxis];
    Int pixPrecision = _multiFit ? 0 : 3;
    _pixelPositions.resize( _fitters->size());
    uint index = 0;
    for (
        inIter.reset();
        ! inIter.atEnd();
        inIter++
    ) {
        subimPos = inIter.position();
        const std::shared_ptr<ProfileFitResults> fitter = (*_fitters)(subimPos);
        if (! fitter) {
            continue;
        }
        summary.str("");
        Int basePrecision = summary.precision(1);
        summary.precision(basePrecision);
        if (csysSub.toWorld(world, subimPos)) {
            summary << "Fit   :" << endl;
            for (uInt i=0; i<world.size(); i++) {
                if ((Int)i != _fitAxis) {
                    IPosition x(1, _axisTypes[i]);
                    x.append(subimPos);
                    if (_axisTypes[i] == LONGITUDE) {
                        summary << "    RA           :   "
                            << _worldCoords(x) << endl;
                    }
                    else if (_axisTypes[i] == LATITUDE) {
                        summary << "    Dec          :  "
                            << _worldCoords(x) << endl;
                    }
                    else if (_axisTypes[i] == FREQUENCY) {
                        summary << "    Freq         : "
                            << _worldCoords(x) <<  endl;
                    }
                    else if (_axisTypes[i] == POLARIZATION) {
                        summary << "    Stokes       : " << _worldCoords(x) << endl;
                    }
                }
            }
        }
        else {
            ThrowCc(csysSub.errorMessage());
        }
        for (uInt i=0; i<pixStart.size(); i++) {
            imPix[i] = pixStart[i] + subimPos[i];
        }
        _pixelPositions[index] = imPix;
        summary.setf(ios::fixed);
        summary << setprecision(pixPrecision) << "    Pixel        : [";
        for (uInt i=0; i<imPix.size(); i++) {
            if (i == (uInt)_fitAxis) {
                summary << " *";
            }
            else {
                summary << imPix[i];
            }
            if (i != imPix.size()-1) {
                summary << ", ";
            }
        }
        summary << "]" << setprecision(basePrecision) << endl;
        summary.unsetf(ios::fixed);
        Bool attempted = fitter->getList().nelements() > 0;
        summary << "    Attempted    : "
            << (attempted ? "YES" : "NO") << endl;
        if (attempted) {
            String converged = fitter->converged() ? "YES" : "NO";
            summary << "    Converged    : " << converged << endl;
            if (fitter->converged()) {
                summary << "    Iterations   : " << fitter->getNumberIterations() << endl;
                String valid = fitter->isValid() ? "YES" : "NO";
                summary << "    Valid        : " << valid << endl;
                if (fitter->isValid()) {
                    solutions = fitter->getList();
                    for (uInt i=0; i<solutions.nelements(); i++) {
                        SpectralElement::Types type = solutions[i]->getType();
                        if (solutions.nelements() > 1) {
                            summary << "    Results for component " << i << ":" << endl;
                        }
                        switch(type) {
                        case SpectralElement::GAUSSIAN:
                            // allow fall through; gaussians and lorentzians use the same
                            // method for output
                        case SpectralElement::LORENTZIAN:
                            {
                                const PCFSpectralElement *pcf
                                    = dynamic_cast<const PCFSpectralElement*>(solutions[i]);
                                summary << _pcfToString(
                                    pcf, _csysIm, world.copy(), subimPos
                                );
                            }
                            break;
                        case SpectralElement::GMULTIPLET:
                            {
                                const GaussianMultipletSpectralElement *gm
                                    = dynamic_cast<const GaussianMultipletSpectralElement*>(solutions[i]);
                                summary << _gaussianMultipletToString(
                                    *gm, _csysIm, world.copy(), subimPos
                                );
                                break;
                            }
                        case SpectralElement::POLYNOMIAL:
                            {
                                const PolynomialSpectralElement *p
                                    = dynamic_cast<const PolynomialSpectralElement*>(solutions[i]);
                                summary << _polynomialToString(*p, _csysIm, imPix, world);
                            }
                            break;
                        case SpectralElement::POWERLOGPOLY:
                            {
                                const PowerLogPolynomialSpectralElement *p
                                    = dynamic_cast<const PowerLogPolynomialSpectralElement*>(solutions[i]);
                                summary << _powerLogPolyToString(*p);
                            }
                            break;
                        case SpectralElement::LOGTRANSPOLY:
                            {
                                const LogTransformedPolynomialSpectralElement *p
                                = dynamic_cast<const LogTransformedPolynomialSpectralElement*>(solutions[i]);
                                summary << _logTransPolyToString(*p);
                            }
                            break;
                        default:
                            ThrowCc("Logic Error: Unhandled spectral element type");
                            break;
                        }
                    }
                }
            }
        }
        if (_logResults) {
            *_log << LogIO::NORMAL << summary.str() << endl << LogIO::POST;
        }
        _writeLogfile(summary.str(), False, False);
    }
    if (_logfile.get() != 0) {
        _logfile->close();
    }
}

String ImageProfileFitterResults::_elementToString(
    const Double value, const Double error,
    const String& unit, Bool isFixed
) const {
    Unit myUnit(unit);
    Vector<String> unitPrefix;
    String outUnit;
    Quantity qVal(value, unit);
    Quantity qErr(error, unit);
    if (myUnit.getValue() == UnitVal::ANGLE) {
        Vector<String> angUnits(5);
        angUnits[0] = "deg";
        angUnits[1] = "arcmin";
        angUnits[2] = "arcsec";
        angUnits[3] = "marcsec";
        angUnits[4] = "uarcsec";
        for (uInt i=0; i<angUnits.size(); i++) {
            outUnit = angUnits[i];
            if (fabs(qVal.getValue(outUnit)) > 1) {
                qVal.convert(outUnit);
                qErr.convert(outUnit);
                break;
            }
        }
    }
    // some optical images have very weird units that start with numbers
    else if (
        unit.empty() || Quantity(1, myUnit).isConform(Quantity(1, "m/s"))
        || isdigit(unit[0])
    ) {
        // do nothing
    }
    else {
        Vector<String> unitPrefix(10);
        unitPrefix[0] = "T";
        unitPrefix[1] = "G";
        unitPrefix[2] = "M";
        unitPrefix[3] = "k";
        unitPrefix[4] = "";
        unitPrefix[5] = "m";
        unitPrefix[6] = "u";
        unitPrefix[7] = "n";
        unitPrefix[8] = "p";
        unitPrefix[9] = "f";

        for (auto prefix: unitPrefix) {
            outUnit = prefix + unit;
            if (fabs(qVal.getValue(outUnit)) > 1) {
                qVal.convert(outUnit);
                qErr.convert(outUnit);
                break;
            }
        }
    }
    Vector<Double> valErr(2);
    valErr[0] = qVal.getValue();
    valErr[1] = qErr.getValue();

    uInt precision = precisionForValueErrorPairs(valErr, Vector<Double>());
    ostringstream out;
    out << std::fixed << setprecision(precision);
    out << qVal.getValue();
    if (isFixed && qErr.getValue() == 0) {
        out << " (fixed)";
    }
    else {
        out << " +/- " << qErr.getValue();
    }
    out << " " << qVal.getUnit();
    return out.str();
}

String ImageProfileFitterResults::_pcfToString(
    const PCFSpectralElement *const &pcf, const CoordinateSystem& csys,
    const Vector<Double>& world, const IPosition& imPos,
    const Bool showTypeString, const String& indent
) const {
    Vector<Double> myWorld = world;
    String yUnit = _subImage->units().getName();
    ostringstream summary;
    Vector<Bool> fixed = pcf->fixed();
    if (showTypeString) {
        summary << indent << "        Type     : "
            << SpectralElement::fromType(pcf->getType()) << endl;
    }
    summary << indent << "        Peak     : "
        << _elementToString(
            pcf->getAmpl(), pcf->getAmplErr(), yUnit, fixed[0]
        ) << endl;
    Double center = _centerWorld(
        *pcf, imPos
    );

    Double increment = fabs(_fitAxisIncrement());

    Double centerErr = pcf->getCenterErr() * increment;
    Double fwhm = pcf->getFWHM() * increment;
    Double fwhmErr = pcf->getFWHMErr() * increment;

    Double pCenter = 0;
    Double pCenterErr = 0;
    Double pFWHM = 0;
    Double pFWHMErr = 0;
    Int specCoordIndex = csys.findCoordinate(Coordinate::SPECTRAL);
    Bool convertedCenterToPix = True;
    Bool convertedFWHMToPix = True;
    if (_doVelocity) {
        if (csys.spectralCoordinate(specCoordIndex).velocityToPixel(pCenter, center)) {
            Double nextVel;
            csys.spectralCoordinate(specCoordIndex).pixelToVelocity(nextVel, pCenter+1);
            Double velInc = fabs(center - nextVel);
            pCenterErr = centerErr/velInc;
            pFWHM = abs(fwhm/velInc);
            pFWHMErr = abs(fwhmErr/velInc);
        }
        else {
            convertedCenterToPix = False;
            convertedFWHMToPix = False;
        }
    }
    else {
        Vector<Double> pixel(myWorld.size());
        myWorld[_fitAxis] = center;
        Double delta = csys.increment()[_fitAxis];
        if (csys.toPixel(pixel, myWorld)) {
            pCenter = pixel[_fitAxis];
            pCenterErr = abs(centerErr/delta);
        }
        else {
            convertedCenterToPix = False;
        }
        pFWHM = abs(fwhm/delta);
        pFWHMErr = abs(fwhmErr/delta);
    }
    summary << indent << "        Center   : "
        << _elementToString(
            center, centerErr, _xUnit, fixed[1]
        ) << endl;
    if (convertedCenterToPix) {
        summary << indent << "                   "
            << _elementToString(
                pCenter, pCenterErr, "pixel", fixed[1]
            ) << endl;
    }
    else {
        summary << indent << "                  Could not convert world to pixel for center" << endl;
    }
    summary << indent << "        FWHM     : "
        << _elementToString(
            fwhm, fwhmErr, _xUnit, fixed[2]
        )
        << endl;
    if (convertedFWHMToPix) {
        summary << indent << "                   "
            << _elementToString(pFWHM, pFWHMErr, "pixel", fixed[2])
            << endl;
    }
    else {
        summary << indent << "                  Could not convert FWHM to pixel" << endl;
    }
    Double integral = pcf->getIntegral()*increment;
    Double integralErr = pcf->getIntegralErr()*increment;
    String integUnit = (Quantity(1.0 ,yUnit)*Quantity(1.0, _xUnit)).getUnit();
    summary << indent << "        Integral : "
        << _elementToString(integral, integralErr, integUnit, fixed[0] && fixed[2])
        << endl;
    if (fwhm/increment <= 3) {
        summary << indent << "WARNING: The FWHM is only " << (fwhm/increment)
            << " times the channel width. Be aware that instrumental channelization effects"
            << " may be important." << endl;
    }
    return summary.str();
}

String ImageProfileFitterResults::_gaussianMultipletToString(
    const GaussianMultipletSpectralElement& gm,
    const CoordinateSystem& csys,
    const Vector<Double>& world, const IPosition& imPos
) const {
    Vector<GaussianSpectralElement> g(gm.getGaussians());
    ostringstream summary;
    summary << "        Type     : GAUSSIAN MULTIPLET" << endl;
    for (uInt i=0; i<g.size(); i++) {
        summary << "        Results for subcomponent "
            << i << ":" << endl;
        summary
            << _pcfToString(&g[i], csys, world, imPos, False, "    ")
            << endl;
    }
    return summary.str();
}

String ImageProfileFitterResults::_polynomialToString(
    const PolynomialSpectralElement& poly, const CoordinateSystem& csys,
    const Vector<Double>& imPix, const Vector<Double>& world
) const {
    ostringstream summary;
    summary << "        Type: POLYNOMIAL" << endl;
    Vector<Double> parms, errs;
    poly.get(parms);
    poly.getError(errs);
    uInt n = parms.size();
    for (uInt j=0; j<n; j++) {
        String unit = _subImage->units().getName();
        if (j > 0) {
            if (j == 1) {
                unit += "/(pixel)";
            }
            else {
                unit += "/((pixel)" + String::toString(j) + ")";
            }
        }
        summary << "         c" << j << " : "
            << _elementToString(parms[j], errs[j], unit, false) << endl;
    }
    // coefficients in pixel coordinates
    Double x0;
    Double deltaX = _fitAxisIncrement();
    if (_doVelocity) {
        csys.spectralCoordinate().pixelToVelocity(x0, 0);
    }
    else {
        Vector<Double> p0 = imPix;
        p0[_fitAxis] = 0;
        Vector<Double> world0 = world;
        csys.toWorld(world0, p0);
        x0 = world0[_fitAxis];
       // deltaX = csys.increment()[_fitAxis];
    }
    Vector<Double> pCoeff(n, 0);
    Vector<Double> pCoeffErr(n, 0);
    for (uInt j=0; j<n; j++) {
        Double sumsq = 0;
        for (uInt k=j; k<n; k++) {
            Double multiplier = Combinatorics::choose(k, k-j)
				* casacore::pow(x0, Float(k - j))
				* casacore::pow(1/deltaX, Float(k));
            if ((k-j) % 2 == 1) {
                multiplier *= -1;
            }
            pCoeff[j] += multiplier * parms[k];
            Double errCoeff = multiplier * errs[k];
            sumsq += errCoeff * errCoeff;
        }
        pCoeffErr[j] = casacore::sqrt(sumsq);
        summary << "         c" << j << " : ";
        String unit = _subImage->units().getName();
        if (j > 0 ) {
            if (j == 1) {
                unit += "/(" + _xUnit + ")";
            }
            else {
                unit += "/((" + _xUnit + ")" + String::toString(j) + ")";
            }
        }
        summary << _elementToString(pCoeff[j], pCoeffErr[j], unit, false) << endl;
    }
    return summary.str();
}


String ImageProfileFitterResults::_powerLogPolyToString(
    const PowerLogPolynomialSpectralElement& plp
) const {
    ostringstream summary;
    summary << "    Type         : POWER LOGARITHMIC POLYNOMIAL" << endl;
    summary << "    Function     : c0*(x/D)**(c1";
    uInt nElements = plp.get().size();
    for (uInt i=2; i<nElements; i++) {
        summary << " + c" << i << "*ln(x/D)";
        if (i > 2) {
            summary << "**" << (i - 1);
        }
    }
    summary << ")" << endl;
    summary << "    D            : " << _plpDivisor << endl;
    Vector<Double> parms = plp.get();
    Vector<Double> errs = plp.getError();
    Vector<Bool> fixed = plp.fixed();

    for (uInt j=0; j<parms.size(); j++) {
        summary << "    c" << j << "           : "
            << _elementToString(parms[j], errs[j], "", fixed[j]) << endl;
    }
    return summary.str();
}

String ImageProfileFitterResults::_logTransPolyToString(
    const LogTransformedPolynomialSpectralElement& ltp
) const {
    ostringstream summary;
    summary << "    Type         : LOGARITHMIC TRANSFORMED POLYNOMIAL" << endl;
    summary << "    Function     : ln(y) = c0";
    uInt nElements = ltp.get().size();
    for (uInt i=1; i<nElements; i++) {
        summary << " + c" << i << "*ln(x/D)";
        if (i > 2) {
            summary << "**" << i;
        }
    }
    summary << ")" << endl;
    summary << "    D            : " << _plpDivisor << endl;
    Vector<Double> parms = ltp.get();
    Vector<Double> errs = ltp.getError();
    Vector<Bool> fixed = ltp.fixed();

    for (uInt j=0; j<parms.size(); j++) {
        summary << "    c" << j << "           : "
            << _elementToString(parms[j], errs[j], "", fixed[j]) << endl;
    }
    return summary.str();
}

void ImageProfileFitterResults::_makeSolutionImages(
    const String& name, const CoordinateSystem& csys,
    const Array<Double>& values, const String& unit,
    const Array<Bool>& mask
) {
    auto valuesShape = values.shape();
    Vector<Float> dataCopy(values.size());
    Vector<Double>::const_iterator iter;
    // isNaN(Array<Double>&) works, isNaN(Array<Float>&) gives spurious results
    Array<Bool> nanInfMask = ! (isNaN(values) || isInf(values));
    Vector<Float>::iterator jiter = dataCopy.begin();
    for (iter=values.begin(); iter!=values.end(); ++iter, ++jiter) {
        *jiter = (Float)*iter;
    }
    auto dc = dataCopy.reform(valuesShape);
    uInt nImages = valuesShape.last();
    auto imageShape = valuesShape.getFirst(valuesShape.size() - 1);
    IPosition start(values.ndim(), 0);
    IPosition end = valuesShape - 1;
    end[end.size() - 1] = 0;
    for (uInt i=0; i<nImages; ++i) {
        auto myname = nImages == 0
            ? name
            : name + "_" + String::toString(i);
        PagedImage<Float> image(imageShape, csys, myname);
        start[start.size() - 1] = i;
        end[end.size() - 1] = i;
        auto imageVals = dc(start, end);
        Array<Double> doubleValues = values(start, end);
        // isNaN(Array<Double>&) works, isNaN(Array<Float>&) gives spurious results
        // Its important to use isInf on the float values not the double values
        Array<Bool> nanInfMask = ! (isNaN(doubleValues) || isInf(imageVals));
        // remove the last axis
        imageVals.removeDegenerate(values.ndim() -1);
        image.put(imageVals);
        nanInfMask.removeDegenerate(values.ndim() -1);
        Bool hasPixMask = ! allTrue(mask);
        Bool hasNanMask = ! allTrue(nanInfMask);
        if (hasNanMask || hasPixMask) {
            Array<Bool> resMask(imageShape);
            String maskName = image.makeUniqueRegionName(
                String("mask"), 0
            );
            image.makeMask(maskName, True, True, False);
            if (hasPixMask) {
                resMask = mask;
                if (hasNanMask) {
                    resMask = resMask && nanInfMask;
                }
            }
            else {
                resMask = nanInfMask;
            }
            (&image.pixelMask())->put(resMask);
        }
        image.setUnits(Unit(unit));
    }
}

}
