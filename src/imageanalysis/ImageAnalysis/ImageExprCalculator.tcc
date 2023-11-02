#include "ImageExprCalculator.h"


#include <casacore/casa/BasicSL/String.h>
#include <casacore/images/Images/ImageBeamSet.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/ImageProxy.h>

#include <imageanalysis/ImageTypedefs.h>
#include <imageanalysis/ImageAnalysis/ImageFactory.h>
#include <imageanalysis/ImageAnalysis/PixelValueManipulator.h>

#include <iomanip>

namespace casa {

template<class T> ImageExprCalculator<T>::ImageExprCalculator(
    const casacore::String& expression, const casacore::String& outname, casacore::Bool overwrite
) : _expr(expression), _outname(outname),
    _overwrite(overwrite), _log() {
    ThrowIf(_expr.empty(), "You must specify an expression");
    if (! outname.empty() && ! overwrite) {
        //casacore::NewFile validfile;
        casacore::String errmsg;
        ThrowIf(
            ! casacore::NewFile().valueOK(outname, errmsg), errmsg
        );
    }
}

template<class T> SPIIT ImageExprCalculator<T>::compute() const {
    _log << casacore::LogOrigin(getClass(), __func__);
    casacore::Record regions;

    // Get casacore::LatticeExprNode (tree) from parser.  Substitute possible
    // object-id's by a sequence number, while creating a
    // casacore::LatticeExprNode for it.  Convert the GlishRecord containing
    // regions to a casacore::PtrBlock<const casacore::ImageRegion*>.

    casacore::Block<casacore::LatticeExprNode> temps;
    casacore::String exprName;
    casacore::PtrBlock<const casacore::ImageRegion*> tempRegs;
    _makeRegionBlock(tempRegs, regions);
    casacore::LatticeExprNode node = casacore::ImageExprParse::command(_expr, temps, tempRegs);
    // Delete the ImageRegions (by using an empty casacore::Record).
    _makeRegionBlock(tempRegs, casacore::Record());
    _checkImages();
    // Get the shape of the expression
    const casacore::IPosition shape = node.shape();

    // Get the casacore::CoordinateSystem of the expression
    const casacore::LELAttribute attr = node.getAttribute();
    const casacore::LELLattCoordBase* lattCoord = &(attr.coordinates().coordinates());
    ThrowIf(
        ! lattCoord->hasCoordinates()
        || lattCoord->classname() != "LELImageCoord",
        "Images in expression have no coordinates"
    );
    const casacore::LELImageCoord* imCoord =
        dynamic_cast<const casacore::LELImageCoord*> (lattCoord);
    AlwaysAssert (imCoord != 0, casacore::AipsError);
    casacore::CoordinateSystem csys = imCoord->coordinates();
    //DataType type = node.dataType();
    SPIIT computedImage = _imagecalc(
        node, shape, csys, imCoord
    );
    computedImage->flush();
    return computedImage;
}

template<class T> void ImageExprCalculator<T>::compute2(
    SPIIT image, const casacore::String& expr, casacore::Bool verbose
) {
    casacore::LogIO log;
    log << casacore::LogOrigin("ImageExprCalculator", __func__);
    ThrowIf(expr.empty(), "You must specify an expression");
    casacore::Record regions;
    casacore::Block<casacore::LatticeExprNode> temps;
    casacore::PtrBlock<const casacore::ImageRegion*> tempRegs;
    PixelValueManipulator<T>::makeRegionBlock(tempRegs, regions);
    auto node = casacore::ImageExprParse::command(expr, temps, tempRegs);
    auto type = node.dataType();
    casacore::Bool isReal = casacore::isReal(type);
    ostringstream os;
    os << type;
    ThrowIf(
        ! isReal && ! isComplex(type),
        "Unsupported node data type " + os.str()
    );
    ThrowIf(
        isComplex(image->dataType()) && isReal,
        "Resulting image is real valued but"
        "the attached image is complex valued"
    );
    ThrowIf(
        casacore::isReal(image->dataType()) && isComplex(type),
        "Resulting image is complex valued but"
        "the attached image is real valued"
    );
    if (verbose) {
        log << casacore::LogIO::WARN << "Overwriting pixel values "
            << "of the currently attached image"
            << casacore::LogIO::POST;
    }
    _calc(image, node);
}

template<class T> void ImageExprCalculator<T>::_calc(
    std::shared_ptr<casacore::ImageInterface<T> > image,
    const casacore::LatticeExprNode& node
) {
    // Get the shape of the expression and check it matches that
    // of the output image
    if (! node.isScalar() && ! image->shape().isEqual(node.shape())) {
        // const auto shapeOut = node.shape();
        //if (! image->shape().isEqual(shapeOut)) {
            ostringstream os;
            os << "The shape of the expression does not conform "
                << "with the shape of the output image"
                << "Expression shape = " << node.shape()
                << "Image shape = " << image->shape();
            throw casacore::AipsError(os.str());
        //}
    }
    // Get the casacore::CoordinateSystem of the expression and check it matches
    // that of the output image
    casacore::LogIO mylog;
    if (! node.isScalar()) {
        const auto attr = node.getAttribute();
        const auto lattCoord = &(attr.coordinates().coordinates());
        if (!lattCoord->hasCoordinates() || lattCoord->classname()
                != "LELImageCoord") {
            // We assume here that the output coordinates are ok
            mylog << casacore::LogIO::WARN
                    << "Images in expression have no coordinates"
                    << casacore::LogIO::POST;
        }
        else {
            const auto imCoord =
                    dynamic_cast<const casacore::LELImageCoord*> (lattCoord);
            AlwaysAssert (imCoord != 0, casacore::AipsError);
            const auto& cSysOut = imCoord->coordinates();
            if (! image->coordinates().near(cSysOut)) {
                // Since the output image has coordinates, and the shapes
                // have conformed, just issue a warning
                mylog << casacore::LogIO::WARN
                        << "The coordinates of the expression do not conform "
                        << endl;
                mylog << "with the coordinates of the output image" << endl;
                mylog << "Proceeding with output image coordinates"
                        << casacore::LogIO::POST;
            }
        }
    }
    // Make a casacore::LatticeExpr and see if it is masked
    casacore::Bool exprIsMasked = node.isMasked();
    if (exprIsMasked) {
        if (! image->isMasked()) {
            // The image does not have a default mask set.  So try and make it one.
            casacore::String maskName = "";
            ImageMaskAttacher::makeMask(*image, maskName, true, true, mylog, true);
        }
    }

    // Evaluate the expression and fill the output image and mask
    if (node.isScalar()) {
        casacore::LatticeExprNode node2 = isReal(node.dataType())
            ? toFloat(node) : toComplex(node);
        // If the scalar value is masked, there is nothing
        // to do.
        if (! exprIsMasked) {
            if (image->isMasked()) {
                // We implement with a LEL expression of the form
                // iif(mask(image)", value, image)
                auto node3 = iif(mask(*image), node2, *image);
                image->copyData(casacore::LatticeExpr<T> (node3));
            }
            else {
                // Just set all values to the scalar. There is no mask to
                // worry about.
                image->copyData(casacore::LatticeExpr<T> (node2));
            }
        }
    }
    else {
        if (image->isMasked()) {
            // We implement with a LEL expression of the form
            // iif(mask(image)", expr, image)
            auto node3 = iif(mask(*image), node, *image);
            image->copyData(casacore::LatticeExpr<T> (node3));
        }
        else {
            // Just copy the pixels from the expression to the output.
            // There is no mask to worry about.
            image->copyData(casacore::LatticeExpr<T> (node));
        }
    }
}

template<class T> SPIIT ImageExprCalculator<T>::_imagecalc(
    const casacore::LatticeExprNode& node, const casacore::IPosition& shape,
    const casacore::CoordinateSystem& csys, const casacore::LELImageCoord* const imCoord
) const {
    _log << casacore::LogOrigin(getClass(), __func__);

    // Create casacore::LatticeExpr create mask if needed
    casacore::LatticeExpr<T> latEx(node);
    SPIIT image;
    casacore::String exprName;
    // Construct output image - an casacore::ImageExpr or a PagedImage
    if (_outname.empty()) {
        image.reset(new casacore::ImageExpr<T> (latEx, exprName));
        ThrowIf(! image, "Failed to create ImageExpr");
    }
    else {
        _log << casacore::LogIO::NORMAL << "Creating image `" << _outname
            << "' of shape " << shape << casacore::LogIO::POST;
        try {
            image.reset(new casacore::PagedImage<T> (shape, csys, _outname));
        }
        catch (const casacore::TableError& te) {
            ThrowIf(
                _overwrite,
                "Caught TableError. This often means "
                "the table you are trying to overwrite has been opened by "
                "another method and so cannot be overwritten at this time. "
                "Try closing it and rerunning"
            );
        }
        ThrowIf(! image, "Failed to create PagedImage");

        // Make mask if needed, and copy data and mask
        if (latEx.isMasked()) {
            casacore::String maskName("");
            ImageMaskAttacher::makeMask(*image, maskName, false, true, _log, true);
        }
        casacore::LatticeUtilities::copyDataAndMask(_log, *image, latEx);
    }

    // Copy miscellaneous stuff over
    auto copied = false;
    casacore::Unit unit;
    if (! _copyMetaDataFromImage.empty()) {
        // do nonexistant file handling here because users want a special message
        ThrowIf(
            ! casacore::File(_copyMetaDataFromImage).isReadable(),
            "Cannot access " + _copyMetaDataFromImage
            + " so cannot copy its metadata to output image"
        );
        auto imagePtrs = ImageFactory::fromFile(_copyMetaDataFromImage);
        SPCIIF imageF;
        SPCIIC imageC;
        std::tie(imageF, imageC, std::ignore, std::ignore) = imagePtrs;
        ThrowIf( ! (imageF || imageC), "Unsupported image pixel data type");
        if (imageF || imageC) {
            if (imageF) {
                image->setMiscInfo(imageF->miscInfo());
                image->setImageInfo(imageF->imageInfo());
                image->setCoordinateInfo(imageF->coordinates());
                unit = imageF->units();
            } 
            else {
                image->setMiscInfo(imageC->miscInfo());
                image->setImageInfo(imageC->imageInfo());
                image->setCoordinateInfo(imageC->coordinates());
                unit = imageC->units();
            }
            copied = true;
        }
    }
    if (! copied) {
        image->setMiscInfo(imCoord->miscInfo());
        image->setImageInfo(imCoord->imageInfo());
    }
    if (_expr.contains("spectralindex")) {
        image->setUnits("");
    }
    else if (_expr.contains(casacore::Regex("pa\\(*"))) {
        image->setUnits("deg");
        casacore::Vector<casacore::Int> newstokes(1);
        newstokes = casacore::Stokes::Pangle;
        casacore::StokesCoordinate scOut(newstokes);
        casacore::CoordinateSystem cSys = image->coordinates();
        casacore::Int iStokes = cSys.findCoordinate(casacore::Coordinate::STOKES, -1);
        cSys.replaceCoordinate(scOut, iStokes);
        image->setCoordinateInfo(cSys);
    }
    else {
        image->setUnits(copied ? unit : imCoord->unit());
    }
    return image;
}

template<class T> void ImageExprCalculator<T>::_makeRegionBlock(
    casacore::PtrBlock<const casacore::ImageRegion*>& regions,
    const casacore::Record& Regions
) {
    for (casacore::uInt j = 0; j < regions.nelements(); j++) {
        delete regions[j];
    }
    regions.resize(0, true, true);
    casacore::uInt nreg = Regions.nfields();
    if (nreg > 0) {
        regions.resize(nreg);
        regions.set(static_cast<casacore::ImageRegion*> (0));
        for (casacore::uInt i = 0; i < nreg; i++) {
            regions[i] = casacore::ImageRegion::fromRecord(Regions.asRecord(i), "");
        }
    }
}

template<class T> void ImageExprCalculator<T>::_checkImages() const {
    auto images = casacore::ImageExprParse::getImageNames();
    if (images.size() <= 1) {
        return;
    }
    unique_ptr<casacore::String> unit;
    unique_ptr<casacore::ImageBeamSet> beamSet;
    unique_ptr<casacore::Vector<casacore::String>> axisNames;
    for (auto& image: images) {
        if (casacore::File(image).exists()) {
            try {
                casacore::ImageProxy myImage(image, "", vector<casacore::ImageProxy>());
                if (myImage.getLattice()) {
                    auto myUnit = myImage.unit();
                    if (unit) {
                        if (myUnit != *unit) {
                            _log << casacore::LogIO::WARN << "image units are not the same: '"
                                << *unit << "' vs '" << myUnit << "'. Proceed with caution. "
                                << "Output image metadata will be copied from "
                                << (
                                    _copyMetaDataFromImage.empty()
                                    ? "one of the input images since imagemd was not specified"
                                    : ("image " + _copyMetaDataFromImage)
                                ) << casacore::LogIO::POST;
                            break;
                        }
                    }
                    else {
                        unit.reset(new casacore::String(myUnit));
                    }
                    casacore::ImageBeamSet mybs = myImage.imageInfoObject().getBeamSet();
                    if (beamSet) {
                        if (mybs != *beamSet) {
                            ostringstream oss;
                            if (mybs.shape() == beamSet->shape()) {
                                String s0, s1;
                                // figure out precision to present unequal beams, CAS-13081
                                uInt i = 5;
                                for (; i<100; i += 5) {
                                    ostringstream os0;
                                    ostringstream os1;
                                    os0 << std::setprecision(i) << mybs;
                                    os1 << std::setprecision(i) << *beamSet;
                                    s0 = os0.str();
                                    s1 = os1.str();
                                    if (s0 != s1) {
                                        break;
                                    }
                                }
                                oss << std::setprecision(i) << "image beams are not the same: "
                                    << mybs << " vs " << *beamSet;
                            }
                            else {
                                oss << "image beam set shapes are not the same " << mybs << " vs " << *beamSet;
                            }
                            _log << casacore::LogIO::WARN << oss.str() << casacore::LogIO::POST;
                            break;
                        }
                    }
                    else {
                        beamSet.reset(new casacore::ImageBeamSet(mybs));
                    }
                    auto myAxes = myImage.coordSysObject().worldAxisNames();
                    if (axisNames) {
                        if (myAxes.size() != axisNames->size()) {
                            _log << casacore::LogIO::WARN << "Number of axes in input images differs"
                                << casacore::LogIO::POST;
                            break;
                        }
                        auto iter = begin(myAxes);
                        for (const auto& axis: *axisNames) {
                            if (axis != *iter) {
                                _log << casacore::LogIO::WARN << "Axes ordering and/or axes names "
                                    << "in input images differs:" << *axisNames << " vs "
                                    << myAxes << casacore::LogIO::POST;
                                break;
                            }
                            ++iter;
                        }
                    }
                    else {
                        axisNames.reset(new casacore::Vector<casacore::String>(myAxes));
                    }

                }
            }
            catch (const casacore::AipsError&) {}
        }
    }
}

}
