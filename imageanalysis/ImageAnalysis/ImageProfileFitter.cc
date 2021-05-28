
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

#include <imageanalysis/ImageAnalysis/ImageProfileFitter.h>

#include <casacore/casa/Quanta/MVAngle.h>
#include <casacore/casa/Quanta/MVTime.h>
#include <casacore/images/Images/ImageUtilities.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/images/Images/TempImage.h>
#include <casacore/scimath/Mathematics/Combinatorics.h>
#include <casacore/casa/Arrays/ArrayLogical.h>

#include <imageanalysis/ImageAnalysis/ProfileFitResults.h>

#include <imageanalysis/ImageAnalysis/ImageCollapser.h>
#include <imageanalysis/ImageAnalysis/SubImageFactory.h>
#include <imageanalysis/IO/ProfileFitterEstimatesFileParser.h>
#include <imageanalysis/IO/ImageProfileFitterResults.h>

// debug
#include <casacore/casa/OS/PrecTimer.h>

using namespace casacore;
namespace casa {

const String ImageProfileFitter::_class = "ImageProfileFitter";

ImageProfileFitter::ImageProfileFitter(
    const SPCIIF image, const String& region,
    const Record *const &regionPtr,    const String& box,
    const String& chans, const String& stokes,
    const String& mask, const Int axis,
    const uInt ngauss, Bool overwrite
) : ImageTask<Float>(
        image, region, regionPtr, box, chans, stokes,
        mask, "", False
    ),
    _residual(), _model(), _xUnit(), _centerName(),
    _centerErrName(), _fwhmName(), _fwhmErrName(),
    _ampName(), _ampErrName(), _integralName(),
    _integralErrName(), _plpName(), _plpErrName(), _sigmaName(),
    _abscissaDivisorForDisplay("1"), _multiFit(False),
    /*_deleteImageOnDestruct(False),*/ _logResults(True),
    _isSpectralIndex(False), _createResid(False), _overwrite(overwrite),
    _storeFits(True),
    _polyOrder(-1), _fitAxis(axis), _nGaussSinglets(ngauss),
    _nGaussMultiplets(0), _nLorentzSinglets(0), _nPLPCoeffs(0),
    _nLTPCoeffs(0), _minGoodPoints(1), _nProfiles(0), _nAttempted(0), _nSucceeded(0),
    _nConverged(0), _nValid(0), _results(Record()), _nonPolyEstimates(),
    _goodAmpRange(), _goodCenterRange(), _goodFWHMRange(),
    _sigma(), _abscissaDivisor(1.0), _residImage(), _goodPlanes() {
    *_getLog() << LogOrigin(_class, __func__);
    _construct();
    _finishConstruction();
}

ImageProfileFitter::ImageProfileFitter(
    const SPCIIF image, const String& region,
    const Record *const &regionPtr,    const String& box,
    const String& chans, const String& stokes,
    const String& mask, const Int axis,
    const String& estimatesFilename, Bool overwrite
) : ImageTask<Float>(
        image, region, regionPtr, box, chans, stokes,
        mask, "", False
    ),
    _residual(), _model(), _xUnit(), _centerName(),
    _centerErrName(), _fwhmName(), _fwhmErrName(),
    _ampName(), _ampErrName(), _integralName(),
    _integralErrName(), _plpName(), _plpErrName(), _sigmaName(),
    _abscissaDivisorForDisplay("1"), _multiFit(False),
    /*_deleteImageOnDestruct(False),*/ _logResults(True),
    _isSpectralIndex(False), _createResid(False), _overwrite(overwrite),
    _storeFits(True),
    _polyOrder(-1), _fitAxis(axis), _nGaussSinglets(0),
    _nGaussMultiplets(0), _nLorentzSinglets(0), _nPLPCoeffs(0),
    _nLTPCoeffs(0), _minGoodPoints(1), _nProfiles(0), _nAttempted(0), _nSucceeded(0),
    _nConverged(0), _nValid(0), _results(Record()), _nonPolyEstimates(),
    _goodAmpRange(), _goodCenterRange(), _goodFWHMRange(),
    _sigma(), _abscissaDivisor(1.0), _residImage(), _goodPlanes() {
    *_getLog() << LogOrigin(_class, __func__);
    ThrowIf(estimatesFilename.empty(), "Estimates filename cannot be empty");
    ProfileFitterEstimatesFileParser parser(estimatesFilename);
    _nonPolyEstimates = parser.getEstimates();
    _nGaussSinglets = _nonPolyEstimates.nelements();
    *_getLog() << LogIO::NORMAL << "Number of gaussian singlets to fit found to be "
        <<_nGaussSinglets << " in estimates file " << estimatesFilename
        << LogIO::POST;
    _construct();
    _finishConstruction();
}

ImageProfileFitter::ImageProfileFitter(
    const SPCIIF image, const String& region,
    const Record *const &regionPtr,    const String& box,
    const String& chans, const String& stokes,
    const String& mask, const Int axis,
    const SpectralList& spectralList, Bool overwrite
) : ImageTask<Float>(
        image, region, regionPtr, box, chans, stokes,
        mask, "", False
    ),
    _residual(), _model(), _xUnit(), _centerName(),
    _centerErrName(), _fwhmName(), _fwhmErrName(),
    _ampName(), _ampErrName(), _integralName(),
    _integralErrName(), _plpName(), _plpErrName(), _sigmaName(),
    _abscissaDivisorForDisplay("1"), _multiFit(False),
    /* _deleteImageOnDestruct(False),*/ _logResults(True),
    _isSpectralIndex(False), _createResid(False), _overwrite(overwrite),
    _storeFits(True),
    _polyOrder(-1), _fitAxis(axis), _nGaussSinglets(0),
    _nGaussMultiplets(0), _nLorentzSinglets(0), _nPLPCoeffs(0),
    _nLTPCoeffs(0), _minGoodPoints(1), _nProfiles(0), _nAttempted(0), _nSucceeded(0),
    _nConverged(0), _nValid(0), _results(Record()), _nonPolyEstimates(),
    _goodAmpRange(), _goodCenterRange(), _goodFWHMRange(),
    _sigma(), _abscissaDivisor(1.0), _residImage(), _goodPlanes() {
    *_getLog() << LogOrigin(_class, __func__);
    ThrowIf(
        spectralList.nelements() == 0,
        "spectralList cannot be empty"
    );
    _nonPolyEstimates = spectralList;
    _nGaussSinglets = 0;
    _nGaussMultiplets = 0;
    for (uInt i=0; i<_nonPolyEstimates.nelements(); i++) {
        SpectralElement::Types myType = _nonPolyEstimates[i]->getType();
        switch(myType) {
        case SpectralElement::GAUSSIAN:
            _nGaussSinglets++;
            break;
        case SpectralElement::GMULTIPLET:
            _nGaussMultiplets++;
            break;
        case SpectralElement::LORENTZIAN:
            _nLorentzSinglets++;
            break;
        case SpectralElement::POWERLOGPOLY:
            ThrowIf(
                _nonPolyEstimates.nelements() > 1 || _polyOrder > 0,
                "Only a single power logarithmic polynomial may be fit "
                "and it cannot be fit simultaneously with other functions"
            );
            _nPLPCoeffs = _nonPolyEstimates[i]->get().size();
            break;
        case SpectralElement::LOGTRANSPOLY:
            ThrowIf(
                _nonPolyEstimates.nelements() > 1 || _polyOrder > 0,
                "Only a single transformed logarithmic polynomial may "
                "be fit and it cannot be fit simultaneously with other functions"
            );
            _nLTPCoeffs = _nonPolyEstimates[i]->get().size();
            break;
        default:
            ThrowCc(
                "Logic error: Only Gaussian singlets, "
                "Gaussian multiplets, and Lorentzian singlets, or a single power "
                "logarithmic polynomial, or a single log transformed polynomial are "
                "permitted in the spectralList input parameter"
            );
            break;
        }
        if (_nPLPCoeffs  > 0) {
            *_getLog() << LogIO::NORMAL << "Will fit a single power logarithmic polynomial "
                << " from provided spectral element list" << LogIO::POST;
        }
        else if (_nLTPCoeffs  > 0) {
            *_getLog() << LogIO::NORMAL << "Will fit a single logarithmic transformed polynomial "
                << " from provided spectral element list" << LogIO::POST;
        }
        else {
            if (_nGaussSinglets > 0) {
                *_getLog() << LogIO::NORMAL << "Number of Gaussian singlets to fit found to be "
                    << _nGaussSinglets << " from provided spectral element list"
                    << LogIO::POST;
            }
            if (_nGaussMultiplets > 0) {
                *_getLog() << LogIO::NORMAL << "Number of Gaussian multiplets to fit found to be "
                    << _nGaussMultiplets << " from provided spectral element list"
                    << LogIO::POST;
            }
            if (_nLorentzSinglets > 0) {
                *_getLog() << LogIO::NORMAL << "Number of lorentzian singlets to fit found to be "
                    << _nLorentzSinglets << " from provided spectral element list"
                    << LogIO::POST;
            }
        }
    }
    _construct();
    _finishConstruction();
}

ImageProfileFitter::~ImageProfileFitter() {}

Record ImageProfileFitter::fit(Bool doDetailedResults) {
    // do this check here rather than at construction because _polyOrder can be set
    // after construction but before fit() is called
    _checkNGaussAndPolyOrder();
    LogOrigin logOrigin(_class, __func__);
    *_getLog() << logOrigin;
    std::unique_ptr<ImageInterface<Float> > originalSigma;
    {
        _subImage = SubImageFactory<Float>::createSubImageRO(
            *_getImage(), *_getRegion(), _getMask(), 0,
            AxesSpecifier(), _getStretch()
        );
        uInt nUnknowns = _nUnknowns();
        ThrowIf(
            nUnknowns >= _subImage->shape()[_fitAxis],
            "There are not enough points ("
            + String::toString(_subImage->shape()[_fitAxis])
            + ") along the fit axis to fit " + String::toString(nUnknowns)
            + " unknowns"
        );
        if (_sigma.get()) {
            if (! _sigmaName.empty()) {
                originalSigma.reset(_sigma->cloneII());
            }
            std::shared_ptr<const SubImage<Float> > sigmaSubImage = SubImageFactory<Float>::createSubImageRO(
                *_sigma, *_getRegion(), _getMask(), 0, AxesSpecifier(), _getStretch()
            );
            _sigma.reset(
                new TempImage<Float>(
                    sigmaSubImage->shape(), sigmaSubImage->coordinates()
                )
            );
            _sigma->put(sigmaSubImage->get());
        }
    }
    *_getLog() << logOrigin;
    _storeFits = doDetailedResults || ! _centerName.empty()
        || ! _centerErrName.empty() || ! _fwhmName.empty()
        || ! _fwhmErrName.empty() || ! _ampName.empty()
        || ! _ampErrName.empty() || ! _integralName.empty()
        || ! _integralErrName.empty()
        || ! _plpName.empty() || ! _plpErrName.empty()
        || ! _ltpName.empty() || ! _ltpErrName.empty();
    try {
        if (! _multiFit) {
            ImageCollapser<Float> collapser(
                _subImage, IPosition(1, _fitAxis), True,
                ImageCollapserData::MEAN, "", True
            );
            SPIIF x = collapser.collapse();
            // _subImage needs to be a SubImage<Float> object
            _subImage = SubImageFactory<Float>::createSubImageRO(
                *x, Record(), "", _getLog().get(),
                AxesSpecifier(), False
            );
            if (_sigma.get()) {
                Array<Bool> sigmaMask = _sigma->get() != Array<Float>(_sigma->shape(), 0.0f);
                if (anyTrue(! sigmaMask)) {
                    if (_sigma->hasPixelMask()) {
                        sigmaMask = sigmaMask && _sigma->pixelMask().get();
                    }
                    else {
                        _sigma->makeMask("sigmamask", True, True, False);
                    }
                    _sigma->pixelMask().put(sigmaMask);
                }
                ImageCollapser<Float> collapsedSigma(
                    _sigma, IPosition(1, _fitAxis), True,
                    ImageCollapserData::MEAN, "", True
                );
                SPIIF collapsed = collapsedSigma.collapse();
                std::shared_ptr<TempImage<Float> >ctmp = std::dynamic_pointer_cast<TempImage<Float> >(collapsed);
                ThrowIf(! ctmp, "Dynamic cast failed");
                _sigma = ctmp;
            }
        }
        _fitallprofiles();
        *_getLog() << logOrigin;
    }
    catch (const AipsError& x) {
        ThrowCc("Exception during fit: " + x.getMesg());
    }
    ImageProfileFitterResults resultHandler(
        _getLog(), _getImage()->coordinates(), &_fitters,
        _nonPolyEstimates, _subImage, _polyOrder,
        _fitAxis, _nGaussSinglets, _nGaussMultiplets,
        _nLorentzSinglets, _nPLPCoeffs, _nLTPCoeffs, _logResults,
        _multiFit, _getLogFile(), _xUnit, _summaryHeader()
    );
    addHistory(
        logOrigin,    
        resultHandler.logSummary(
            _nProfiles, _nAttempted, _nSucceeded, _nConverged, _nValid
        )
    );
    if (_nPLPCoeffs > 0) {
        resultHandler.setPLPName(_plpName);
        resultHandler.setPLPErrName(_plpErrName);
        resultHandler.setPLPDivisor(_abscissaDivisorForDisplay);
    }
    else if (_nLTPCoeffs > 0) {
        resultHandler.setLTPName(_ltpName);
        resultHandler.setLTPErrName(_ltpErrName);
        resultHandler.setPLPDivisor(_abscissaDivisorForDisplay);
    }
    else if (_nGaussSinglets + _nGaussMultiplets + _nLorentzSinglets > 0) {
        resultHandler.setAmpName(_ampName);
        resultHandler.setAmpErrName(_ampErrName);
        resultHandler.setCenterName(_centerName);
        resultHandler.setCenterErrName(_centerErrName);
        resultHandler.setFWHMName(_fwhmName);
        resultHandler.setFWHMErrName(_fwhmErrName);
        resultHandler.setIntegralName(_integralName);
        resultHandler.setIntegralErrName(_integralErrName);
    }
    if (doDetailedResults) {
        resultHandler.createResults();
        _results = resultHandler.getResults();
    }
    resultHandler.writeImages(_nConverged > 0);
    if (_modelImage) {
        _modelImage = _prepareOutputImage(*_modelImage, _model, _overwrite, True);
    }
    if (_residImage) {
        _residImage = _prepareOutputImage(*_residImage, _residual, _overwrite, True);
    }
    if (originalSigma && ! _sigmaName.empty()) {
        _prepareOutputImage(*originalSigma, _sigmaName, True, True);
    }
    return _results;
}

uInt ImageProfileFitter::_nUnknowns() const {
    uInt n = 0;
    if (_polyOrder >= 0) {
        n += _polyOrder + 1;
    }
    if (_nGaussSinglets > 0) {
        n += 3*_nGaussSinglets;
    }
    uInt nel = _nonPolyEstimates.nelements();
    if (n == 0) {
        return n;
    }
    for (uInt i=0; i<nel; ++i) {
        const SpectralElement *const x = _nonPolyEstimates[i];
        Vector<Bool> fixed = x->fixed();
        Vector<Bool>::const_iterator iter = fixed.begin();
        Vector<Bool>::const_iterator end = fixed.end();
        while (iter != end) {
            if (*iter) {
                --n;
                if (n == 0) {
                    return n;
                }
            }
            ++iter;
        }
    }
    return n;
}


void ImageProfileFitter::setPolyOrder(Int p) {
    ThrowIf(p < 0,"A polynomial cannot have a negative order");
    ThrowIf(
        _nPLPCoeffs > 0,
        "Cannot simultaneously fit a polynomial and "
        "a power logarithmic polynomial."
    );
    ThrowIf(
        _nLTPCoeffs > 0,
        "Cannot simultaneously fit a polynomial and "
        "a logarithmic transformed polynomial"
    );
    _polyOrder = p;
}

void ImageProfileFitter::setGoodAmpRange(const Double minv, const Double maxv) {
    _goodAmpRange.set(
        new std::pair<Double, Double>(min(minv, maxv), max(minv, maxv))
    );
}

void ImageProfileFitter::setGoodCenterRange(const Double minv, const Double maxv) {
    _goodCenterRange.set(
        new std::pair<Double, Double>(min(minv, maxv), max(minv, maxv))
    );
}

void ImageProfileFitter::setGoodFWHMRange(const Double minv, const Double maxv) {
    _goodFWHMRange.set(
        new std::pair<Double, Double>(min(minv, maxv), max(minv, maxv))
    );
}

void ImageProfileFitter::setSigma(const Array<Float>& sigma) {
    std::unique_ptr<TempImage<Float> > temp;
    if (sigma.ndim() == _getImage()->ndim()) {
        temp.reset(new TempImage<Float>(
            sigma.shape(), _getImage()->coordinates())
        );
    }
    else if (sigma.ndim() == 1) {
        SpectralCoordinate sp;
        CoordinateSystem csys;
        csys.addCoordinate(sp);
        temp.reset(new TempImage<Float>(sigma.shape(), csys));
    }
    temp->put(sigma);
    setSigma(temp.get());
}

void ImageProfileFitter::setSigma(const ImageInterface<Float>* const &sigma) {
    if (anyTrue(sigma->get() < Array<Float>(sigma->shape(), 0.0f))) {
        *_getLog() << "All sigma values must be non-negative" << LogIO::EXCEPTION;
    }
    Float mymax = fabs(max(sigma->get()));
    if (
        sigma->ndim() == _getImage()->ndim()
        && sigma->shape() == _getImage()->shape()
    ) {
        SPIIF clone(sigma->cloneII());
        _sigma = std::dynamic_pointer_cast<TempImage<Float> >(clone);
        if (! _sigma) {
            SPIIF x = SubImageFactory<Float>::createImage(
                *sigma, "", Record(), "", False, False ,True, False
            );
            if (x) {
                _sigma = std::dynamic_pointer_cast<TempImage<Float> >(x);
                ThrowIf(
                    ! _sigma,
                    "Unable to create temporary weights image"
                );
            }
        }
        if (mymax != 1) {
            _sigma->put(_sigma->get()/mymax);
        }
    }
    else if (
        sigma->ndim() == _getImage()->ndim()
        || sigma->ndim() == 1
    ) {
        if (sigma->ndim() == _getImage()->ndim()) {
            IPosition expShape(_getImage()->ndim(), 1);
            expShape[_fitAxis] = _getImage()->shape()[_fitAxis];
            ThrowIf(
                sigma->shape() != expShape,
                "If the shape of the standard deviation image differs "
                "from the shape of the input image, the shape of the "
                "standard deviation image must be " + expShape.toString()
            );
        }
        else if (sigma->ndim() == 1) {
            ThrowIf(
                sigma->shape()[0] != _getImage()->shape()[_fitAxis],
                "A one dimensional standard deviation spectrum must have the same "
                "number of pixels as the input image has along its axis to be fit"
            );
        }
        IPosition dataToInsertShape(_getImage()->ndim(), 1);
        dataToInsertShape[_fitAxis] = _getImage()->shape()[_fitAxis];
        _sigma.reset(new TempImage<Float>(_getImage()->shape(), _getImage()->coordinates()));
        Array<Float> dataToInsert(IPosition(1, _getImage()->shape()[_fitAxis]));
        dataToInsert = sigma->get(sigma->ndim() == _getImage()->ndim());
        // normalize
        if (mymax != 1) {
            dataToInsert /= mymax;
        }
        Array<Float> sigmaData = _sigma->get();
        ArrayIterator<Float> iter(sigmaData, IPosition(1, _fitAxis), True);
        while(! iter.pastEnd()) {
            iter.array() = dataToInsert;
            iter.next();
        }
        _sigma->put(sigmaData);
    }
    else {
        ThrowCc("Illegal shape of standard deviation image");
    }
    if (! _sigma->coordinates().near(_getImage()->coordinates())) {
        _sigma->setCoordinateInfo(_getImage()->coordinates());
    }
}

Record ImageProfileFitter::getResults() const {
    return _results;
}

void ImageProfileFitter::setAbscissaDivisor(Double d) {
    if (! _isSpectralIndex) {
        *_getLog() << LogOrigin(_class, __func__);
        *_getLog() << LogIO::WARN << "This object is not configured to fit a "
            << "spectral index function, and so setting the abscissa divisor "
            << "will have no effect in the fitting process." << LogIO::POST;
    }
    _abscissaDivisor = d;
    _abscissaDivisorForDisplay = String::toString(d)
        + _getImage()->coordinates().worldAxisUnits()[_fitAxis];
}

void ImageProfileFitter::setAbscissaDivisor(const Quantity& q) {
    String fitAxisUnit = _getImage()->coordinates().worldAxisUnits()[_fitAxis];
    ThrowIf(
        ! q.isConform(fitAxisUnit),
        "Abscissa divisor unit " + q.getUnit()
        + " is not consistent with fit axis unit"
    );
    if (! _isSpectralIndex) {
        *_getLog() << LogOrigin(_class, __func__);
        *_getLog() << LogIO::WARN << "This object is not configured to fit a spectral index function "
            << "and so setting the abscissa divisor will have no effect in the fitting process."
            << LogIO::POST;
    }
    _abscissaDivisor = q.getValue(fitAxisUnit);
    _abscissaDivisorForDisplay = String::toString(q);
}

std::vector<OutputDestinationChecker::OutputStruct> ImageProfileFitter::_getOutputStruct() {
    std::vector<OutputDestinationChecker::OutputStruct> outputs;
    if (! _model.empty()) {
        OutputDestinationChecker::OutputStruct modelImage;
        modelImage.label = "model image";
        modelImage.outputFile = &_model;
        modelImage.required = True;
        modelImage.replaceable = _overwrite;
        outputs.push_back(modelImage);
    }
    if (! _residual.empty()) {
        OutputDestinationChecker::OutputStruct residImage;
        residImage.label = "residual image";
        residImage.outputFile = &_residual;
        residImage.required = True;
        residImage.replaceable = _overwrite;
        outputs.push_back(residImage);
    }
    return outputs;
}

void ImageProfileFitter::_checkNGaussAndPolyOrder() const {
    ThrowIf(
        _polyOrder < 0
        && (
            _nGaussSinglets + _nGaussMultiplets
            + _nLorentzSinglets
        ) == 0 && ! _isSpectralIndex,
        "Number of non-polynomials is 0 and polynomial order is less than zero. "
        "According to these inputs there is nothing to fit."
    );
}

void ImageProfileFitter::_finishConstruction() {
    LogOrigin logOrigin(_class, __func__);
    _isSpectralIndex = _nPLPCoeffs + _nLTPCoeffs > 0;
    ThrowIf(
        _fitAxis >= (Int)_getImage()->ndim(),
        "Specified fit axis " + String::toString(_fitAxis)
        + " must be less than the number of image axes ("
        + String::toString(_getImage()->ndim()) + ")"
    )
    if (_fitAxis < 0) {
        if (! _getImage()->coordinates().hasSpectralAxis()) {
            _fitAxis = 0;
            *_getLog() << LogIO::WARN << "No spectral coordinate found in image, "
                << "using axis 0 as fit axis" << LogIO::POST;
        }
        else {
            _fitAxis = _getImage()->coordinates().spectralAxisNumber();
            *_getLog() << LogIO::NORMAL << "Using spectral axis (axis " << _fitAxis
                << ") as fit axis" << LogIO::POST;
        }
    }
    //this->_setSupportsLogfile(true);
}
/*
Bool ImageProfileFitter::_inVelocitySpace() const {
    return _fitAxis == _subImage->coordinates().spectralAxisNumber()
        && Quantity(1, _xUnit).isConform("m/s")
        && _subImage->coordinates().spectralCoordinate().restFrequency() > 0;
}
*/

Double ImageProfileFitter::getWorldValue(
    double pixelVal, const IPosition& imPos, const String& units,
    bool velocity, bool wavelength, int tabularIndex, MFrequency::Types freqType ) const {
    Vector<Double> pixel(imPos.size());
    for (uInt i=0; i<pixel.size(); i++) {
        pixel[i] = imPos[i];
    }
    Vector<Double> world(pixel.size());
    // in pixels here
    pixel[_fitAxis] = pixelVal;
    _subImage->coordinates().toWorld(world, pixel);
    SpectralCoordinate spectCoord;
    //Use a tabular index for conversion if one has been specified.
    if ( tabularIndex >= 0 ) {
        const CoordinateSystem& cSys = _subImage->coordinates();
        TabularCoordinate tabCoord = cSys.tabularCoordinate( tabularIndex );
        Vector<Double> worlds = tabCoord.worldValues();
        spectCoord = SpectralCoordinate( freqType, worlds );
    }
    //Use the default spectral axis for conversion
    else {
        spectCoord = _subImage->coordinates().spectralCoordinate();
    }
    Double convertedVal;
    if (velocity) {
        spectCoord.setVelocity( units );
        spectCoord.frequencyToVelocity(convertedVal, world(_fitAxis));
    }
    else if ( wavelength  ) {
        spectCoord.setWavelengthUnit( units );
        Vector<Double> worldVal(1);
        worldVal[0] = world(_fitAxis);
        Vector<Double> waveVal(1);
        spectCoord.frequencyToWavelength(waveVal, worldVal);
        convertedVal = waveVal[0];
    }
    else {
        convertedVal = world(_fitAxis);
    }
    return convertedVal;
}

void ImageProfileFitter::_fitallprofiles() {
    *_getLog() << LogOrigin(_class, __func__);
    // Create output images with a mask. Make them TempImages to start
    // in attempt to improve IO performance, and write them out if necessary
    // at the end
    if (
        ! _model.empty()
        && ! (
            _modelImage = SubImageFactory<Float>::createImage(
                *_subImage, "", Record(), "",
                False, _overwrite ,False, False, True
            )
        )
    ) {
        *_getLog() << LogIO::WARN << "Failed to create model image" << LogIO::POST;
    }
    if (
        (! _residual.empty() || _createResid)
        && ! (
            _residImage = SubImageFactory<Float>::createImage(
                *_subImage, "", Record(), "",
                False, _overwrite ,False, False, True
            )
        )
    ) {
        *_getLog() << LogIO::WARN << "Failed to create residual image" << LogIO::POST;
    }
    Bool showProgress = True;
    _fitProfiles(showProgress);
}

void ImageProfileFitter::_fitProfiles(Bool showProgress) {
    IPosition inShape = _subImage->shape();
    if (_modelImage) {
        AlwaysAssert(inShape.isEqual(_modelImage->shape()), AipsError);
    }
    if (_residImage) {
        AlwaysAssert(inShape.isEqual(_residImage->shape()), AipsError);
    }
    const auto nDim = _subImage->ndim();
    IPosition sliceShape(nDim, 1);
    sliceShape(_fitAxis) = inShape(_fitAxis);
    Array<Float> resultData(sliceShape);
    Array<Bool> resultMask(sliceShape);
    String doppler = "";
    auto csys = _subImage->coordinates();
    _xUnit = csys.spectralCoordinate().worldAxisUnits()[0];
    if (
        ! _isSpectralIndex && _fitAxis == _subImage->coordinates().spectralAxisNumber()
        && _subImage->coordinates().spectralCoordinate().restFrequency() > 0
    ) {
        SpectralCoordinate specCoord = csys.spectralCoordinate();
        _xUnit = specCoord.velocityUnit();
        doppler = MDoppler::showType(
            specCoord.velocityDoppler()
        );
    }
    String errMsg;
    ImageFit1D<Float>::AbcissaType abcissaType;
    auto abscissaUnits = _isSpectralIndex ? "native" : "pix";
    ThrowIf(
        ! ImageFit1D<Float>::setAbcissaState(
            errMsg, abcissaType, csys, abscissaUnits, doppler, _fitAxis
        ), errMsg
    );
    IPosition fitterShape = inShape;
    fitterShape[_fitAxis] = 1;
    if (_storeFits) {
        _fitters.resize(fitterShape);
    }
    _nProfiles = fitterShape.product();
    std::shared_ptr<ProgressMeter> pProgressMeter;
    if (showProgress) {
        ostringstream oss;
        oss << "Fit profiles on axis " << _fitAxis+1;
        pProgressMeter.reset(
            new ProgressMeter(0, _nProfiles, oss.str())
        );
    }
    SPCIIF fitData = _subImage;
    std::set<uInt> myGoodPlanes;
    if (! _goodPlanes.empty()) {
        IPosition origin(_subImage->ndim(), 0);
        Vector<Double> world(_subImage->ndim(), 0);
        csys.toWorld(world, origin);
        const CoordinateSystem imcsys = _getImage()->coordinates();
        Int imageOff = Int(imcsys.toPixel(world)[_fitAxis] + 0.5);
        AlwaysAssert(imageOff >= 0, AipsError);
        std::vector<Int> goodPlanes(_goodPlanes.begin(), _goodPlanes.end());
        if (imageOff > 0) {
            goodPlanes = std::vector<Int>(_goodPlanes.size());
            std::transform(
                _goodPlanes.begin(), _goodPlanes.end(), goodPlanes.begin(),
                bind2nd(minus<Int>(), imageOff)
            );
        }
        std::vector<Int>::iterator iter = goodPlanes.begin();
        while (iter != goodPlanes.end() && *iter < 0) {
            goodPlanes.erase(iter);
            iter = goodPlanes.begin();
        }
        myGoodPlanes = std::set<uInt>(goodPlanes.begin(), goodPlanes.end());
    }
    Bool checkMinPts = fitData->isMasked();
    Array<Bool> fitMask;
    if (checkMinPts) {
        fitMask = (
            partialNTrue(fitData->getMask(False), IPosition(1, _fitAxis))
            >= (long unsigned int) _minGoodPoints
        );
        IPosition oldShape = fitMask.shape();
        IPosition newShape(fitMask.ndim() + 1);
        uInt oldIndex = 0;
        for (uInt i=0; i<newShape.size(); ++i) {
            if (i == (uInt)_fitAxis) {
                newShape[i] = 1;
            }
            else {
                newShape[i] = oldShape[oldIndex];
                ++oldIndex;
            }
        }
        fitMask.assign(fitMask.reform(newShape));
    }
    _loopOverFits(
        fitData, showProgress, pProgressMeter, checkMinPts,
        fitMask, abcissaType, fitterShape, sliceShape,
        myGoodPlanes
    );
}

void ImageProfileFitter::_loopOverFits(
    SPCIIF fitData, Bool showProgress,
    std::shared_ptr<ProgressMeter> progressMeter, Bool checkMinPts,
    const Array<Bool>& fitMask, ImageFit1D<Float>::AbcissaType abcissaType,
    const IPosition& fitterShape, const IPosition& sliceShape,
    const std::set<uInt> goodPlanes
) {
    *_getLog() << LogOrigin(_class, __func__);
    Lattice<Bool>* pFitMask = _modelImage && _modelImage->hasPixelMask()
        && _modelImage->pixelMask().isWritable()
        ? &(_modelImage->pixelMask())
        : 0;
    Lattice<Bool>* pResidMask = _residImage && _residImage->hasPixelMask()
        && _residImage->pixelMask().isWritable()
        ? &(_residImage->pixelMask())
        : 0;
    vector<IPosition> goodPos(0);
    SpectralList newEstimates = _nonPolyEstimates;
    ImageFit1D<Float> fitter = _sigma
        ? ImageFit1D<Float>(fitData, _sigma, _fitAxis)
        : ImageFit1D<Float>(fitData, _fitAxis);
    Bool isSpectral = _fitAxis == _subImage->coordinates().spectralAxisNumber();

    // calculate the abscissa values only once if they will not change
    // with position
    Double *divisorPtr = 0;
    Vector<Double> abscissaValues(0);
    Bool fitSuccess = False;
    if (isSpectral) {
        abscissaValues = fitter.makeAbscissa(abcissaType, True, 0);
        if (_isSpectralIndex) {
            _setAbscissaDivisorIfNecessary(abscissaValues);
            if (_abscissaDivisor != 1) {
                divisorPtr = &_abscissaDivisor;
                abscissaValues /= _abscissaDivisor;
                if (_nLTPCoeffs > 0) {
                    abscissaValues = log(abscissaValues);
                }
            }
        }
    }
    PtrHolder<const PolynomialSpectralElement> polyEl;
    if (_polyOrder >= 0) {
        polyEl.set(new PolynomialSpectralElement(Vector<Double>(_polyOrder + 1, 0)));
        if (newEstimates.nelements() > 0) {
            newEstimates.add(*polyEl);
        }
    }
    uInt nOrigComps = newEstimates.nelements();
    Array<Double> (*xfunc)(const Array<Double>&) = 0;
    Array<Double> (*yfunc)(const Array<Double>&) = 0;
    Bool abscissaSet = ! abscissaValues.empty();
    if (_nLTPCoeffs > 0) {
        if (! abscissaSet) {
            xfunc = casacore::log;
        }
        yfunc = casacore::log;
    }
    if (abscissaSet) {
        fitter.setAbscissa(abscissaValues);
        //abscissaSet = False;
    }
    IPosition inTileShape = fitData->niceCursorShape();
    TiledLineStepper stepper (fitData->shape(), inTileShape, _fitAxis);
    RO_MaskedLatticeIterator<Float> inIter(*fitData, stepper);
    uInt nProfiles = 0;
    Bool hasXMask = ! goodPlanes.empty();
    Bool hasNonPolyEstimates = _nonPolyEstimates.nelements() > 0;
    Bool updateOutput = _modelImage || _residImage;
    Bool storeGoodPos = hasNonPolyEstimates && ! _fitters.empty();
    for (inIter.reset(); ! inIter.atEnd(); ++inIter, ++nProfiles) {
        if (showProgress && /*nProfiles % mark == 0 &&*/ nProfiles > 0) {
            progressMeter->update(Double(nProfiles));
        }
        const IPosition& curPos = inIter.position();
        if (checkMinPts && ! fitMask(curPos)) {
            continue;
        }
        ++_nAttempted;
        fitter.clearList();
        if (abscissaSet) {
            fitter.setData(curPos, yfunc);
        }
        else {
            fitter.setData(
                curPos, abcissaType, True, divisorPtr, xfunc, yfunc
            );
        }
        Bool canFit = _setFitterElements(
            fitter, newEstimates, polyEl, goodPos,
            fitterShape, curPos, nOrigComps
        );
        if (canFit) {
            if (hasXMask) {
                fitter.setXMask(goodPlanes, True);
            }
            try {
                fitSuccess = fitter.fit();
                if (fitSuccess) {
                    if (fitter.converged()) {
                        _flagFitterIfNecessary(fitter);
                        ++_nConverged;
                    }
                    fitSuccess = fitter.isValid();
                    if (fitSuccess) {
                        ++_nValid;
                        if (storeGoodPos) {
                            goodPos.push_back(curPos);
                        }
                    }
                }
            }
            catch (const AipsError& x) {
                fitSuccess = False;
            }
        }
        else {
            fitSuccess = False;
        }
        if (fitter.succeeded()) {
            ++_nSucceeded;
        }
        if (_storeFits) {
            _fitters(curPos).reset(new ProfileFitResults(fitter));
        }
        if (updateOutput) {
            _updateModelAndResidual(
                fitSuccess, fitter, sliceShape,
                curPos, pFitMask, pResidMask
            );
        }
    }
}

void ImageProfileFitter::_updateModelAndResidual(
    Bool fitOK, const ImageFit1D<Float>& fitter,
    const IPosition& sliceShape, const IPosition& curPos,
    Lattice<Bool>* const &pFitMask,
    Lattice<Bool>* const &pResidMask
) const {
    static const Array<Float> failData(sliceShape, NAN);
    static const Array<Bool> failMask(sliceShape, False);
    Array<Bool> resultMask = fitOK
        ? fitter.getDataMask().reform(sliceShape)
        : failMask;
    if (_modelImage) {
        _modelImage->putSlice (
            (fitOK ? fitter.getFit().reform(sliceShape) : failData),
            curPos
        );
        if (pFitMask) {
            pFitMask->putSlice(resultMask, curPos);
        }
    }
    if (_residImage) {
        _residImage->putSlice (
            (fitOK ? fitter.getResidual().reform(sliceShape) : failData),
            curPos
        );
        if (pResidMask) {
            pResidMask->putSlice(resultMask, curPos);
        }
    }
}

Bool ImageProfileFitter::_setFitterElements(
    ImageFit1D<Float>& fitter, SpectralList& newEstimates,
    const PtrHolder<const PolynomialSpectralElement>& polyEl,
    const std::vector<IPosition>& goodPos,
    const IPosition& fitterShape, const IPosition& curPos,
    uInt nOrigComps
) const {
    if (_nonPolyEstimates.nelements() == 0) {
        if (_nGaussSinglets > 0) {
            fitter.setGaussianElements (_nGaussSinglets);
            uInt ng = fitter.getList(False).nelements();
            if (ng != _nGaussSinglets && ! _haveWarnedAboutGuessingGaussians) {
                *this->_getLog() << LogOrigin(getClass(), __func__) << LogIO::WARN;
                if (ng == 0) {
                    *this->_getLog() << "Unable to estimate "
                        << "parameters for any Gaussian singlets. ";
                }
                else {
                    *this->_getLog() << "Only able to estimate parameters for " << ng
                        << " Gaussian singlets. ";
                }
                *this->_getLog() << "If you really want "
                    << _nGaussSinglets << " Gaussian singlets to be fit, "
                    << "you should specify initial parameter estimates for all of them";
                if (_multiFit) {
                    *this->_getLog() << " (additional warnings of this type during "
                        "this run will not be logged)";
                }
                *this->_getLog() << "."    << LogIO::POST;
                _haveWarnedAboutGuessingGaussians = True;
            }
        }
        if (polyEl.ptr()) {
            fitter.addElement(*polyEl);
        }
        else {
            if (fitter.getList(False).nelements() == 0) {
                return False;
            }
        }
    }
    else {
        // user supplied initial estimates
        if (goodPos.size() > 0) {
            IPosition nearest;
            Int minDist2 = 0;
            for (
                IPosition::const_iterator iter=fitterShape.begin();
                iter!=fitterShape.end(); iter++
            ) {
                minDist2 += *iter * *iter;
            }
            for (
                vector<IPosition>::const_reverse_iterator iter=goodPos.rbegin();
                iter != goodPos.rend(); iter++
            ) {
                IPosition diff = curPos - *iter;
                Int dist2 = 0;
                Bool larger = False;
                for (
                    IPosition::const_iterator ipositer=diff.begin();
                    ipositer!=diff.end(); ipositer++
                ) {
                    dist2 += *ipositer * *ipositer;
                    if(dist2 >= minDist2) {
                        larger = True;
                        break;
                    }
                }
                if (
                    _fitters(*iter)->getList().nelements() == nOrigComps
                    && ! larger
                ) {
                    minDist2 = dist2;
                    nearest = *iter;
                    if (minDist2 == 1) {
                        // can't get any nearer than this
                        break;
                    }
                }
            }
            newEstimates = _fitters(nearest)->getList();
        }
        fitter.setElements(newEstimates);
    }
    return True;
}

void ImageProfileFitter::_setAbscissaDivisorIfNecessary(
    const Vector<Double>& abscissaValues
) {
    if (_abscissaDivisor == 0) {
        setAbscissaDivisor(1);
        if (abscissaValues.size() > 0) {
            Double minAbs = min(abs(abscissaValues));
            Double maxAbs = max(abs(abscissaValues));
            Double l = (Int)log10(sqrt(minAbs*maxAbs));
            Double p = std::pow(10.0, l);
            setAbscissaDivisor(p);
        }
    }
    if (_abscissaDivisor != 1) {
        *_getLog() << LogIO::NORMAL << "Dividing abscissa values by "
            << _abscissaDivisor << " before fitting" << LogIO::POST;
    }
}

void ImageProfileFitter::_flagFitterIfNecessary(
    ImageFit1D<Float>& fitter
) const {
    Bool checkComps = _goodAmpRange || _goodCenterRange
        || _goodFWHMRange;
    SpectralList solutions = fitter.getList(True);
    for (uInt i=0; i<solutions.nelements(); ++i) {
        if (
            anyTrue(isNaN(solutions[i]->get()))
            || anyTrue(isNaN(solutions[i]->getError()))
        ) {
            fitter.invalidate();
            return;
        }
        if (checkComps) {
            switch (solutions[i]->getType()) {
            case SpectralElement::GAUSSIAN:
            // allow fall through
            case SpectralElement::LORENTZIAN: {
                if (
                    ! _isPCFSolutionOK(
                        dynamic_cast<
                            const PCFSpectralElement*
                        >(solutions[i])
                    )
                ) {
                    fitter.invalidate();
                    return;
                }
                break;
            }
            case SpectralElement::GMULTIPLET: {
                const GaussianMultipletSpectralElement *gm = dynamic_cast<
                    const GaussianMultipletSpectralElement*
                >(solutions[i]);
                Vector<GaussianSpectralElement> gse = gm->getGaussians();
                for (uInt j=0; j<gse.size(); j++) {
                    if (! _isPCFSolutionOK(&gse[i])) {
                        fitter.invalidate();
                        return;
                    }
                }
                break;
            }
            default:
                continue;
            }
        }
    }
}

Bool ImageProfileFitter::_isPCFSolutionOK(
    const PCFSpectralElement *const &pcf
) const {
    if (_goodAmpRange) {
        Double amp = pcf->getAmpl();
        if (
            amp < _goodAmpRange->first
            || amp > _goodAmpRange->second
            || fabs(pcf->getAmplErr()/amp) > 100
        ) {
            return False;
        }
    }
    if (_goodCenterRange) {
        Double center = pcf->getCenter();
        if (
            center < _goodCenterRange->first
            || center > _goodCenterRange->second
        ) {
            return False;
        }
    }
    if (_goodFWHMRange) {
        Double fwhm = pcf->getFWHM();
        if (
            fwhm < _goodFWHMRange->first
            || fwhm > _goodFWHMRange->second
            || fabs(pcf->getFWHMErr()/fwhm) > 100
        ) {
            return False;
        }
    }
    return True;
}

const Array<std::shared_ptr<ProfileFitResults> >& ImageProfileFitter::getFitters() const{
    return _fitters;
}

}
