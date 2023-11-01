#include <imageanalysis/ImageAnalysis/StatImageCreator.h>

#include <imageanalysis/Annotations/AnnCenterBox.h>
#include <imageanalysis/Annotations/AnnCircle.h>
#include <casacore/lattices/LEL/LatticeExpr.h>

namespace casa {

StatImageCreator::StatImageCreator(
    const SPCIIF image, const Record *const region,
	const String& mask, const String& outname, Bool overwrite
) : ImageStatsBase(image, region, mask, outname, overwrite) {
    this->_construct();
	auto da = _getImage()->coordinates().directionAxesNumbers();
    _dirAxes[0] = da[0];
    _dirAxes[1] = da[1];
	setAnchorPosition(0, 0);
}

void StatImageCreator::setRadius(const Quantity& radius) {
    ThrowIf(
        ! (radius.getUnit().startsWith("pix") || radius.isConform("rad")),
        "radius units " + radius.getUnit() + " must be pixel or angular"
    );
    _xlen = radius;
    _ylen = Quantity(0,"");
}

void StatImageCreator::setRectangle(
    const Quantity& xLength, const Quantity& yLength
) {
    ThrowIf(
        ! (xLength.getUnit().startsWith("pix") || xLength.isConform("rad")),
        "xLength units " + xLength.getUnit() + " must be pixel or angular"
    );
    ThrowIf(
        ! (yLength.getUnit().startsWith("pix") || yLength.isConform("rad")),
        "xLength units " + yLength.getUnit() + " must be pixel or angular"
    );
    _xlen = xLength;
    _ylen = yLength;
}

void StatImageCreator::setAnchorPosition(Int x, Int y) {
    Vector<Double> anchorPixel(_getImage()->ndim(), 0);
    anchorPixel[_dirAxes[0]] = x;
    anchorPixel[_dirAxes[1]] = y;
    _anchor = _getImage()->coordinates().toWorld(anchorPixel);
}

void StatImageCreator::useReferencePixelAsAnchor() {
    const auto refPix = _getImage()->coordinates().referencePixel();
    Int x = round(refPix[_dirAxes[0]]);
    Int y = round(refPix[_dirAxes[1]]);
    *_getLog() << LogIO::NORMAL << LogOrigin("StatImageCreator", __func__)
        << "Anchor being set at pixel [" << x << "," << y
        << "], at/near image reference pixel." << LogIO::POST;
    setAnchorPosition(x, y);
}

void StatImageCreator::setGridSpacing(uInt x, uInt y) {
    _grid.first = x;
    _grid.second = y;
}

SPIIF StatImageCreator::compute() {
    static const AxesSpecifier dummyAxesSpec;
    *_getLog() << LogOrigin(getClass(), __func__);
    auto mylog = _getLog().get();
    auto subImageRO = SubImageFactory<Float>::createSubImageRO(
        *_getImage(), *_getRegion(), _getMask(),
        mylog, dummyAxesSpec, _getStretch()
    );
    auto subImageRW = SubImageFactory<Float>::createImage(
        *_getImage(), "", *_getRegion(), _getMask(),
        dummyAxesSpec, False, False, _getStretch()
    );
    _doMask = ! ImageMask::isAllMaskTrue(*subImageRO);
    const auto imshape = subImageRO->shape();
    const auto xshape = imshape[_dirAxes[0]];
    const auto yshape = imshape[_dirAxes[1]];
    const auto& csys = subImageRO->coordinates();
    auto anchorPixel = csys.toPixel(_anchor);
    Int xanchor = rint(anchorPixel[_dirAxes[0]]);
    Int yanchor = rint(anchorPixel[_dirAxes[1]]);
    // ensure xanchor and yanchor are positive
    if (xanchor < 0) {
        // ugh, mod of a negative number in C++ doesn't do what I want it to
        // integer division
        xanchor += (abs(xanchor)/_grid.first + 1)*_grid.first;
    }
    if (yanchor < 0) {
        yanchor += (abs(yanchor)/_grid.second + 1)*_grid.second;
    }
    // xstart and ystart are the pixel location in the
    // subimage of the lower left corner of the grid,
    // ie they are the pixel location of the grid point
    // with the smallest non-negative x and y values
    Int xstart = xanchor % _grid.first;
    Int ystart = yanchor % _grid.second;
    if (xstart < 0) {
        xstart += _grid.first;
    }
    if (ystart < 0) {
        ystart += _grid.second;
    }
    Array<Float> tmp;
    // integer division
    auto nxpts = (xshape - xstart)/_grid.first;
    if ((xshape - xstart) % _grid.first != 0) {
        ++nxpts;
    }
    auto nypts = (yshape - ystart)/ _grid.second;
    if ((yshape - ystart) % _grid.second != 0) {
        ++nypts;
    }
    IPosition storeShape = imshape;
    storeShape[_dirAxes[0]] = nxpts;
    storeShape[_dirAxes[1]] = nypts;
    auto interpolate = _grid.first > 1 || _grid.second > 1;
    TempImage<Float>* writeTo = dynamic_cast<TempImage<Float> *>(subImageRW.get());
    std::unique_ptr<TempImage<Float>> store;
    if (interpolate) {
        store.reset(new TempImage<Float>(storeShape, csys));
        store->set(0.0);
        if (_doMask) {
            store->attachMask(ArrayLattice<Bool>(storeShape));
            store->pixelMask().set(True);
        }
        writeTo = store.get();
    }
    _computeStat(*writeTo, subImageRO, nxpts, nypts, xstart, ystart);
    if (_doProbit) {
        writeTo->copyData((LatticeExpr<Float>)(*writeTo * C::probit_3_4));
    }
    if (interpolate) {
        _doInterpolation(
            subImageRW, *store, subImageRO,
            nxpts, nypts, xstart,  ystart
        );
    }
    auto res = _prepareOutputImage(*subImageRW);
    return res;
}

void StatImageCreator::_computeStat(
    TempImage<Float>& writeTo, SPCIIF subImage,
    uInt nxpts, uInt nypts, Int xstart, Int ystart
) {
    std::shared_ptr<Array<Bool>> regionMask;
    uInt xBlcOff = 0;
    uInt yBlcOff = 0;
    uInt xChunkSize = 0;
    uInt yChunkSize = 0;
    _nominalChunkInfo(
        regionMask,  xBlcOff, yBlcOff,
        xChunkSize, yChunkSize, subImage
    );
    Array<Bool> regMaskCopy;
    if (regionMask) {
        regMaskCopy = regionMask->copy();
        if (! writeTo.hasPixelMask()) {
            ArrayLattice<Bool> mymask(writeTo.shape());
            mymask.set(True);
            writeTo.attachMask(mymask);
        }
    }
    auto imshape = subImage->shape();
    auto ndim = imshape.size();
    IPosition chunkShape(ndim, 1);
    chunkShape[_dirAxes[0]] = xChunkSize;
    chunkShape[_dirAxes[1]] = yChunkSize;
    auto xsize = imshape[_dirAxes[0]];
    auto ysize = imshape[_dirAxes[1]];
    IPosition planeShape(imshape.size(), 1);
    planeShape[_dirAxes[0]] = xsize;
    planeShape[_dirAxes[1]] = ysize;
    RO_MaskedLatticeIterator<Float> lattIter (
        *subImage, planeShape, True
    );
    String algName;
    auto alg = _getStatsAlgorithm(algName);
    std::set<StatisticsData::STATS> statToCompute;
    statToCompute.insert(_statType);
    alg->setStatsToCalculate(statToCompute);
    auto ngrid = nxpts*nypts;
    auto nPts = ngrid;
    IPosition loopAxes;
    for (uInt i=0; i<ndim; ++i) {
        if (i != _dirAxes[0] && i != _dirAxes[1]) {
            loopAxes.append(IPosition(1, i));
            nPts *= imshape[i];
        }
    }
    auto doCircle = _ylen.getValue() <= 0;
    *_getLog() << LogOrigin(getClass(), __func__) << LogIO::NORMAL
        << "Using ";
    if (doCircle) {
        *_getLog() << "circular region of radius " << _xlen;
    }
    else {
        *_getLog() << "rectangular region of specified dimensions " << _xlen
            << " x " << _ylen;
    }
    *_getLog() << " (because of centering and "
            << "rounding to use whole pixels, actual dimensions of bounding box are "
            << xChunkSize << " pix x " << yChunkSize << " pix";
    if (doCircle) {
        *_getLog() << " and there are " << ntrue(*regionMask)
            << " good pixels in the circle that are being used";
    }
    *_getLog() << ") to choose pixels for computing " << _statName
        << " using the " << algName << " algorithm around each of "
        << ngrid << " grid points in " << (nPts/ngrid) << " planes." << LogIO::POST;
    auto imageChunkShape = chunkShape;
    _doStatsLoop(
        writeTo, lattIter, nxpts, nypts, xstart, ystart,
        xBlcOff, yBlcOff, xChunkSize, yChunkSize, imshape,
        chunkShape, regionMask, alg, regMaskCopy, loopAxes, nPts
    );
}

void StatImageCreator::_doStatsLoop(
    TempImage<Float>& writeTo, RO_MaskedLatticeIterator<Float>& lattIter,
    uInt nxpts, uInt nypts, Int xstart, Int ystart, uInt xBlcOff,
    uInt yBlcOff, uInt xChunkSize, uInt yChunkSize, const IPosition& imshape,
    const IPosition& chunkShape, std::shared_ptr<Array<Bool>> regionMask,
    std::shared_ptr<
        StatisticsAlgorithm<
            Double, Array<Float>::const_iterator, Array<Bool>::const_iterator,
            Array<Float>::const_iterator
        >
    >& alg, const Array<Bool>& regMaskCopy, const IPosition& loopAxes, uInt nPts
) {
    auto ximshape = imshape[_dirAxes[0]];
    auto yimshape = imshape[_dirAxes[1]];
    auto ndim = imshape.size();
    IPosition planeBlc(ndim, 0);
    auto& xPlaneBlc = planeBlc[_dirAxes[0]];
    auto& yPlaneBlc = planeBlc[_dirAxes[1]];
    auto planeTrc = planeBlc + chunkShape - 1;
    auto& xPlaneTrc = planeTrc[_dirAxes[0]];
    auto& yPlaneTrc = planeTrc[_dirAxes[1]];
    uInt nCount = 0;
    auto hasLoopAxes = ! loopAxes.empty();
    ProgressMeter pm(0, nPts, "Processing stats at grid points");
    // Vector rather than IPosition because blc values can be negative
    Vector<Int> blc(ndim, 0);
    blc[_dirAxes[0]] = xstart - xBlcOff;
    blc[_dirAxes[1]] = ystart - yBlcOff;
    for (lattIter.atStart(); ! lattIter.atEnd(); ++lattIter) {
        auto plane = lattIter.cursor();
        auto lattMask = lattIter.getMask();
        auto checkGrid = ! allTrue(lattMask);
        auto outPos = lattIter.position();
        auto& xOutPos = outPos[_dirAxes[0]];
        auto& yOutPos = outPos[_dirAxes[1]];
        // inPos is the position of the current grid point
        // This is the position in the current chunk, not the entire lattice
        auto inPos = plane.shape() - 1;
        inPos[_dirAxes[0]] = xstart;
        inPos[_dirAxes[1]] = ystart;
        auto& xInPos = inPos[_dirAxes[0]];
        auto& yInPos = inPos[_dirAxes[1]];
        Int yblc = ystart - yBlcOff;
        for (uInt yCount=0; yCount<nypts; ++yCount, yblc+=_grid.second, yInPos+=_grid.second) {
            yOutPos = yCount;
            yPlaneBlc = max(0, yblc);
            yPlaneTrc = min(
                yblc + (Int)yChunkSize - 1, (Int)yimshape - 1
            );
            IPosition regMaskStart(ndim, 0);
            auto& xRegMaskStart = regMaskStart[_dirAxes[0]];
            auto& yRegMaskStart = regMaskStart[_dirAxes[1]];
            auto regMaskLength = regMaskStart;
            auto& xRegMaskLength = regMaskLength[_dirAxes[0]];
            auto& yRegMaskLength = regMaskLength[_dirAxes[1]];
            Bool yDoMaskSlice = False;
            if (regionMask) {
                regMaskLength = regionMask->shape();
                if (yblc < 0) {
                    yRegMaskStart = -yblc;
                    yRegMaskLength += yblc;
                    yDoMaskSlice = True;
                }
                else if (yblc + yChunkSize > yimshape) {
                    yRegMaskLength = yimshape - yblc;
                    yDoMaskSlice = True;
                }
            }
            xInPos = xstart;
            Int xblc = xstart - xBlcOff;
            for (uInt xCount=0; xCount<nxpts; ++xCount, xblc+=_grid.first, xInPos+=_grid.first) {
                Float res = 0;
                xOutPos = xCount;
                if (checkGrid && ! lattMask(inPos)) {
                    // grid point is masked, no computation should be done
                    writeTo.putAt(res, outPos);
                    writeTo.pixelMask().putAt(False, outPos);
                }
                else {
                    xPlaneBlc = max(0, xblc);
                    xPlaneTrc = min(
                        xblc + (Int)xChunkSize - 1, (Int)ximshape - 1
                    );
                    std::shared_ptr<Array<Bool>> subRegionMask;
                    if (regionMask) {
                        auto doMaskSlice = yDoMaskSlice;
                        xRegMaskStart = 0;
                        if (xblc < 0) {
                            xRegMaskStart = -xblc;
                            xRegMaskLength = regionMask->shape()[_dirAxes[0]] + xblc;
                            doMaskSlice = True;
                        }
                        else if (xblc + xChunkSize > ximshape) {
                            regMaskLength[_dirAxes[0]] = ximshape - xblc;
                            doMaskSlice = True;
                        }
                        else {
                            xRegMaskLength = xChunkSize;
                        }
                        if (doMaskSlice) {
                            Slicer sl(regMaskStart, regMaskLength);
                            subRegionMask.reset(new Array<Bool>(regMaskCopy(sl)));
                        }
                        else {
                            subRegionMask.reset(new Array<Bool>(regMaskCopy));
                        }
                    }
                    auto maskChunk = lattMask(planeBlc, planeTrc).copy();
                    if (subRegionMask) {
                        maskChunk = maskChunk && *subRegionMask;
                    }
                    if (anyTrue(maskChunk)) {
                        auto chunk = plane(planeBlc, planeTrc);
                        if (allTrue(maskChunk)) {
                            alg->setData(chunk.begin(), chunk.size());
                        }
                        else {
                            alg->setData(chunk.begin(), maskChunk.begin(), chunk.size());
                        }
                        res = alg->getStatistic(_statType);
                    }
                    else {
                        // no pixels in region on which to do stats, mask grid point
                        writeTo.pixelMask().putAt(False, outPos);
                    }
                    writeTo.putAt(res, outPos);
                }
            }
            nCount += nxpts;
            pm.update(nCount);
        }
        if (hasLoopAxes) {
            Bool done = False;
            uInt idx = 0;
            while (! done) {
                auto targetAxis = loopAxes[idx];
                ++blc[targetAxis];
                if (blc[targetAxis] == imshape[targetAxis]) {
                    blc[targetAxis] = 0;
                    ++idx;
                    done = idx == loopAxes.size();
                }
                else {
                    done = True;
                }
            }
        }
    }
}

std::shared_ptr<
    StatisticsAlgorithm<Double, Array<Float>::const_iterator,
    Array<Bool>::const_iterator>
>
StatImageCreator::_getStatsAlgorithm(String& algName) const {
    auto ac = _getAlgConf();
    switch(ac.algorithm) {
    case StatisticsData::CLASSICAL:
        algName = "classical";
        return std::shared_ptr<
            ClassicalStatistics<Double, Array<Float>::const_iterator,
            Array<Bool>::const_iterator>
        >(
            new ClassicalStatistics<
                Double, Array<Float>::const_iterator,
                Array<Bool>::const_iterator
            >()
        );
    case StatisticsData::CHAUVENETCRITERION:
        algName = "Chauvenet criterion/z-score";
        return std::shared_ptr<ChauvenetCriterionStatistics<
            Double, Array<Float>::const_iterator,
            Array<Bool>::const_iterator>
        >(
            new ChauvenetCriterionStatistics<
                Double, Array<Float>::const_iterator,
                Array<Bool>::const_iterator
            >(
                ac.zs, ac.mi
            )
        );
    default:
        ThrowCc("Unsupported statistics algorithm");
    }
}

void StatImageCreator::_nominalChunkInfo(
    std::shared_ptr<Array<Bool>>& templateMask, uInt& xBlcOff, uInt& yBlcOff,
    uInt& xChunkSize, uInt& yChunkSize, SPCIIF subimage
) const {
    Double xPixLength = 0;
    const auto& csys = subimage->coordinates();
    if (_xlen.getUnit().startsWith("pix")) {
        xPixLength = _xlen.getValue();
    }
    else {
        const auto& dc = csys.directionCoordinate();
        auto units = dc.worldAxisUnits();
        auto inc = dc.increment();
        Quantity worldPixSize(abs(inc[0]), units[0]);
        xPixLength = _xlen.getValue("rad")/worldPixSize.getValue("rad");
    }
    const auto shape = subimage->shape();
    ThrowIf(
        xPixLength >= shape[_dirAxes[0]] - 4,
        "x region length is nearly as large as or larger than the subimage extent of "
        + String::toString(shape[_dirAxes[0]]) + " pixels. Such a configuration is not supported"
    );
    ThrowIf(
        xPixLength <= 0.5,
        "x region length is only " + String::toString(xPixLength)
        + " pixels. Such a configuration is not supported"
    );
    Double yPixLength = xPixLength;
    if (_ylen.getValue() > 0) {
        if (_ylen.getUnit().startsWith("pix")) {
            yPixLength = _ylen.getValue();
        }
        else {
            const auto& dc = csys.directionCoordinate();
            auto units = dc.worldAxisUnits();
            auto inc = dc.increment();
            Quantity worldPixSize(abs(inc[1]), units[1]);
            yPixLength = _ylen.getValue("rad")/worldPixSize.getValue("rad");
        }
    }
    ThrowIf(
        yPixLength >= shape[_dirAxes[1]] - 4,
        "y region length is nearly as large as or larger than the subimage extent of "
        + String::toString(_dirAxes[1]) + " pixels. Such a configuration is not supported"
    );
    ThrowIf(
        yPixLength <= 0.5,
        "y region length is only " + String::toString(yPixLength)
        + " pixels. Such a configuration is not supported"
    );
    IPosition templateShape(shape.size(), 1);
    templateShape[_dirAxes[0]] = shape[_dirAxes[0]];
    templateShape[_dirAxes[1]] = shape[_dirAxes[1]];
    TempImage<Float> templateImage(templateShape, csys);
    // integer division, xq, yq must have integral values
    uInt xcenter = templateShape[_dirAxes[0]]/2;
    uInt ycenter = templateShape[_dirAxes[1]]/2;
    Quantity xq(xcenter, "pix");
    Quantity yq(ycenter, "pix");
    IPosition centerPix(templateShape.size(), 0);
    centerPix[_dirAxes[0]] = xcenter;
    centerPix[_dirAxes[1]] = ycenter;
    auto world = csys.toWorld(centerPix);
    static const Vector<Stokes::StokesTypes> dummyStokes;
    Record reg;
    if (_ylen.getValue() > 0) {
        AnnCenterBox box(
            xq, yq, _xlen, _ylen, csys, templateShape, dummyStokes
        );
        reg = box.getRegion()->toRecord("");
    }
    else {
        AnnCircle circle(
            xq, yq, _xlen, csys, templateShape, dummyStokes
        );
        reg = circle.getRegion()->toRecord("");
    }
    static const AxesSpecifier dummyAxesSpec;
    auto chunkImage = SubImageFactory<Float>::createSubImageRO(
        templateImage, reg, "", nullptr, dummyAxesSpec, False
    );
    auto blcOff = chunkImage->coordinates().toPixel(world);
    xBlcOff = rint(blcOff[_dirAxes[0]]);
    yBlcOff = rint(blcOff[_dirAxes[1]]);
    auto chunkShape = chunkImage->shape();
    xChunkSize = chunkShape[_dirAxes[0]];
    yChunkSize = chunkShape[_dirAxes[1]];
    templateMask.reset();
    if (chunkImage->isMasked()) {
        auto mask = chunkImage->getMask();
        if (! allTrue(mask)) {
            templateMask.reset(new Array<Bool>(mask));
        }
    }
}

void StatImageCreator::_doInterpolation(
    SPIIF output, TempImage<Float>& store,
    SPCIIF subImage, uInt nxpts, uInt nypts,
    Int xstart, Int ystart
) const {
    const auto imshape = subImage->shape();
    const auto xshape = imshape[_dirAxes[0]];
    const auto yshape = imshape[_dirAxes[1]];
    const auto ndim = subImage->ndim();
    *_getLog() << LogOrigin(getClass(), __func__)
        << LogIO::NORMAL << "Interpolate using "
        << _interpName << " algorithm." << LogIO::POST;
    Matrix<Float> result(xshape, yshape);
    Matrix<Bool> resultMask;
    if (_doMask) {
        resultMask.resize(IPosition(2, xshape, yshape));
        resultMask.set(True);
    }
    Matrix<Float> storage(nxpts, nypts);
    Matrix<Bool> storeMask(nxpts, nypts);
    std::pair<uInt, uInt> start;
    start.first = xstart;
    start.second = ystart;
    if (ndim == 2) {
        store.get(storage);
        store.getMask(storeMask);
        _interpolate(result, resultMask, storage, storeMask, start);
        output->put(result);
        if (_doMask) {
            output->pixelMask().put(resultMask && subImage->pixelMask().get());
        }
    }
    else {
        // get each direction plane in the storage lattice at each chan/stokes
        IPosition cursorShape(ndim, 1);
        cursorShape[_dirAxes[0]] = nxpts;
        cursorShape[_dirAxes[1]] = nypts;
        auto axisPath = _dirAxes;
        axisPath.append((IPosition::otherAxes(ndim, _dirAxes)));
        LatticeStepper stepper(store.shape(), cursorShape, axisPath);
        Slicer slicer(stepper.position(), stepper.endPosition(), Slicer::endIsLast);
        IPosition myshape(ndim, 1);
        myshape[_dirAxes[0]] = xshape;
        myshape[_dirAxes[1]] = yshape;
        for (stepper.reset(); ! stepper.atEnd(); stepper++) {
            auto pos = stepper.position();
            slicer.setStart(pos);
            slicer.setEnd(stepper.endPosition());
            storage = store.getSlice(slicer, True);
            storeMask = store.getMaskSlice(slicer, True);
            _interpolate(result, resultMask, storage, storeMask, start);
            output->putSlice(result, pos);
            if (_doMask) {
                output->pixelMask().putSlice(
                    resultMask && subImage->pixelMask().getSlice(pos, myshape, True),
                    pos
                );
            }
        }
    }
}

void StatImageCreator::setInterpAlgorithm(Interpolate2D::Method alg) {
    switch (alg) {
    case Interpolate2D::CUBIC:
        _interpName = "CUBIC";
        break;
    case Interpolate2D::LANCZOS:
        _interpName = "LANCZOS";
        break;
    case Interpolate2D::LINEAR:
        _interpName = "LINEAR";
        break;
    case Interpolate2D::NEAREST:
        _interpName = "NEAREST";
        break;
    default:
        ThrowCc("Unhandled interpolation method " + String::toString(alg));
    }
    _interpolater = Interpolate2D(alg);
}

void StatImageCreator::setStatType(StatisticsData::STATS s) {
    _statType = s;
    _statName = StatisticsData::toString(s);
    _statName.upcase();
}

void StatImageCreator::setStatType(const String& s) {
    String m = s;
    StatisticsData::STATS stat;
    m.downcase();
    if (m.startsWith("i")) {
        stat = StatisticsData::INNER_QUARTILE_RANGE;
    }
    else if (m.startsWith("max")) {
        stat = StatisticsData::MAX;
    }
    else if (m.startsWith("mea")) {
        stat = StatisticsData::MEAN;
    }
    else if (m.startsWith("meda") || m.startsWith("mad") || m.startsWith("x")) {
        stat = StatisticsData::MEDABSDEVMED;
    }
    else if (m.startsWith("medi")) {
        stat = StatisticsData::MEDIAN;
    }
    else if (m.startsWith("mi")) {
        stat = StatisticsData::MIN;
    }
    else if (m.startsWith("n")) {
        stat = StatisticsData::NPTS;
    }
    else if (m.startsWith("q1")) {
        stat = StatisticsData::FIRST_QUARTILE;
    }
    else if (m.startsWith("q3")) {
        stat = StatisticsData::THIRD_QUARTILE;
    }
    else if (m.startsWith("r")) {
        stat = StatisticsData::RMS;
    }
    else if (m.startsWith("si") || m.startsWith("st")) {
        stat = StatisticsData::STDDEV;
    }
    else if (m.startsWith("sums")) {
        stat = StatisticsData::SUMSQ;
    }
    else if (m.startsWith("sum")) {
        stat = StatisticsData::SUM;
    }
    else if (m.startsWith("v")) {
        stat = StatisticsData::VARIANCE;
    }
    else {
        ThrowCc("Statistic " + s + " not supported.");
    }
    _doProbit = m.startsWith("x");
    setStatType(stat);
}

void StatImageCreator::_interpolate(
    Matrix<Float>& result, Matrix<Bool>& resultMask,
    const Matrix<Float>& storage,
    const Matrix<Bool>& storeMask,
    const std::pair<uInt, uInt>& start
) const {
    auto shape = result.shape();
    auto imax = shape[0];
    auto jmax = shape[1];
    // x values change fastest in the iterator
    auto iter = result.begin();
    auto miter = resultMask.begin();
    // xcell and ycell are the current positions within the current
    // grid cell represented by an integer modulo pointsPerCell
    auto xCell = start.first == 0 ? 0 : _grid.first - start.first;
    auto yCell = start.second == 0 ? 0 : _grid.second - start.second;
    Int xStoreInt = start.first == 0 ? 0 : -1;
    Int yStoreInt = start.second == 0 ? 0 : -1;
    Double xStoreFrac = (Double)xCell/(Double)_grid.first;
    Double yStoreFrac = (Double)yCell/(Double)_grid.second;
    Vector<Double> storeLoc(2);
    storeLoc[0] = xStoreInt + xStoreFrac;
    storeLoc[1] = yStoreInt + yStoreFrac;
    for (uInt j=0; j<jmax; ++j, ++yCell) {
        Bool onYGrid = yCell == 0;
        if (yCell == _grid.second) {
            yCell = 0;
            ++yStoreInt;
            yStoreFrac = 0;
            onYGrid = True;
        }
        else {
            yStoreFrac = (Double)yCell/(Double)_grid.second;
        }
        storeLoc[1] = yStoreInt + yStoreFrac;
        xCell = start.first == 0 ? 0 : _grid.first - start.first;
        xStoreInt = start.first == 0 ? 0 : -1;
        xStoreFrac = (Double)xCell/(Double)_grid.first;
        for (uInt i=0; i<imax; ++i, ++xCell) {
            Bool onXGrid = xCell == 0;
            if (xCell == _grid.first) {
                xCell = 0;
                ++xStoreInt;
                onXGrid = True;
            }
            if (onXGrid && onYGrid) {
                // exactly on a grid point, no interpolation needed
                // just copy value directly from storage matrix
                *iter = storage(xStoreInt, yStoreInt);
                if (_doMask) {
                    *miter = storeMask(xStoreInt, yStoreInt);
                }
            }
            else {
                xStoreFrac = (Double)xCell/(Double)_grid.first;
                storeLoc[0] = xStoreInt + xStoreFrac;
                if (! _interpolater.interp(*iter, storeLoc, storage, storeMask)) {
                    ThrowIf(
                        ! _doMask,
                        "Logic error: bad interpolation but there is no mask to set"
                    );
                    *miter = False;
                }
            }
            ++iter;
            if (_doMask) {
                ++miter;
            }
        }
    }
}

}
