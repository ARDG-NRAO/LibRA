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

#include <imageanalysis/ImageAnalysis/ImageRegridder.h>

#include <imageanalysis/ImageAnalysis/ImageFactory.h>
#include <imageanalysis/ImageAnalysis/ImageMetaData.h>
#include <imageanalysis/ImageAnalysis/SubImageFactory.h>
#include <casacore/images/Images/ImageConcat.h>
#include <casacore/images/Images/ImageRegrid.h>
#include <casacore/scimath/Mathematics/Geometry.h>

#include <casacore/casa/BasicSL/STLIO.h>
#include <memory>

namespace casa {

template<class T> const String  ImageRegridder<T>::_class = "ImageRegridder";

template<class T> ImageRegridder<T>::ImageRegridder(
    const SPCIIT image, const casacore::Record *const regionRec,
    const casacore::String& maskInp, const casacore::String& outname,
    casacore::Bool overwrite, const casacore::CoordinateSystem& csysTo,
    const casacore::IPosition& axes, const casacore::IPosition& shape
) : ImageRegridderBase<T>(
        image, regionRec, maskInp, outname,
        overwrite, csysTo, axes, shape
    ), _debug(0) {}

template<class T> ImageRegridder<T>::ImageRegridder(
    const SPCIIT image, const casacore::String& outname,
    const SPCIIT templateIm, const casacore::IPosition& axes,
    const casacore::Record *const regionRec, const casacore::String& maskInp,
    casacore::Bool overwrite, const casacore::IPosition& shape
)  : ImageRegridderBase<T>(
        image, regionRec, maskInp, outname, overwrite,
        templateIm->coordinates(), axes, shape
    ),
    _debug(0) {}

template<class T> ImageRegridder<T>::~ImageRegridder() {}

template<class T> SPIIT ImageRegridder<T>::regrid() const {
    _subimage = SubImageFactory<T>::createImage(
        *this->_getImage(), "", *this->_getRegion(), this->_getMask(),
        this->_getDropDegen(), false, false, this->_getStretch()
    );
    auto regridByVel = false;
    const auto axes = this->_getAxes();
    auto hasMultipleBeams = this->_getImage()->imageInfo().hasMultipleBeams();
    const auto& csys = this->_getImage()->coordinates();
    if (
        (this->_getSpecAsVelocity() || hasMultipleBeams)
        && csys.hasSpectralAxis()
        && this->_getTemplateCoords().hasSpectralAxis()
    ) {
        auto inputSpecAxis = csys.spectralAxisNumber(false);
        auto isInputSpecDegen = _subimage->shape()[inputSpecAxis] == 1;
        if (axes.empty()) {
            ThrowIf(
                hasMultipleBeams,
                "An image with multiple beams cannot be regridded along the "
                "spectral axis. You may wish to convolve all channels to a "
                "common resolution and retry"
            );
            if (! isInputSpecDegen && this->_getSpecAsVelocity()) {
                regridByVel = true;
            }
        }
        else {
            auto specAxis = csys.spectralAxisNumber();
            for (uInt i=0; i<axes.size(); ++i) {
                if (axes[i] == specAxis) {
                    ThrowIf(
                        hasMultipleBeams,
                        "An image with multiple beams cannot be regridded "
                        "along the spectral axis. You may wish to convolve all "
                        "channels to a common resolution and retry"
                    );
                    if (! isInputSpecDegen && this->_getSpecAsVelocity()) {
                        regridByVel = true;
                    }
                    break;
                }
            }
        }
    }
    auto workIm = regridByVel ? this->_regridByVelocity() : this->_regrid();
    return this->_prepareOutputImage(*workIm);
}

template<class T> SPIIT ImageRegridder<T>::_regrid() const {
    if (! _subimage) {
        // for when this method is called directly by regridByVelocity
        _subimage = SubImageFactory<T>::createImage(
            *this->_getImage(), "", *this->_getRegion(), this->_getMask(),
            this->_getDropDegen(), false, false, this->_getStretch()
        );
    }
    *this->_getLog() << LogOrigin(_class, __func__);
    ThrowIf(
        ImageMask::isAllMaskFalse(*_subimage),
        "All selected pixels are masked"
    );
    const auto csysFrom = _subimage->coordinates();
    CoordinateSystem csysTo = this->_getTemplateCoords();
    csysTo.setObsInfo(csysFrom.obsInfo());
    std::set<Coordinate::Type> coordsToRegrid;
    auto csys = ImageRegrid<T>::makeCoordinateSystem(
        *this->_getLog(), coordsToRegrid, csysTo, csysFrom, this->_getAxes(),
        _subimage->shape(), false
    );
    ThrowIf(
        csys.nPixelAxes() != this->_getShape().size(),
        "The number of pixel axes in the output shape and Coordinate System "
        "must be the same. Shape has size "
        + casacore::String::toString(this->_getShape().size()) + ". Output "
        "coordinate system has "
        + casacore::String::toString(csys.nPixelAxes()) + " axes"
    );
    this->_checkOutputShape(*_subimage, coordsToRegrid);
    SPIIT workIm(new TempImage<T>(this->_getKludgedShape(), csys));
    ImageUtilities::copyMiscellaneous(*workIm, *_subimage);
    String maskName("");
    ImageMaskAttacher::makeMask(
        *workIm, maskName, true, true, *this->_getLog(), true
    );
    ThrowIf (
        ! this->_doImagesOverlap(_subimage, workIm),
        "There is no overlap between the (region chosen in) the input image"
        " and the output image with respect to the axes being regridded."
    );
    if (
        coordsToRegrid.find(Coordinate::SPECTRAL) != coordsToRegrid.end()
        && fabs(csys.spectralCoordinate().increment()[0])
            > fabs(csysFrom.spectralCoordinate().increment()[0])
    ) {
        *this->_getLog() << LogOrigin(getClass(), __func__) << LogIO::WARN
            << "Warning: template/imagename relative channel size is "
            << fabs(
                csys.spectralCoordinate().increment()[0]
                /csysFrom.spectralCoordinate().increment()[0]
            ) << LogIO::POST;
        *this->_getLog() << LogOrigin(getClass(), __func__)
            << LogIO::WARN << "imregrid/ia.regrid() interpolates over spectral "
            << "channels and does not average channels together. Noise in your "
            << "resulting image will be the noise in the original individual "
            << "channels, not the averaged channel noise. To average output "
            << "channels together, use specsmooth (or ia.boxcar() or "
            << "ia.hanning() to smooth the spectral axis of your input cube to "
            << "close to desired resolution and use imregrid/ia.regrid() to "
            << "regrid it to the desired spectral coordinate grid."
            << LogIO::POST;
    }
    ImageRegrid<T> ir;
    ir.showDebugInfo(_debug);
    ir.disableReferenceConversions(! this->_getDoRefChange());
    ir.regrid(
        *workIm, this->_getMethod(), this->_getAxes(), *_subimage,
        this->_getReplicate(), this->_getDecimate(), true,
        this->_getForceRegrid()
    );
    if (! this->_getOutputStokes().empty()) {
        workIm = this->_decimateStokes(workIm);
    }
    ThrowIf(
        workIm->hasPixelMask() && ImageMask::isAllMaskFalse(*workIm),
        "All output pixels are masked"
        + casacore::String(
            this->_getDecimate() > 1 && this->_regriddingDirectionAxes()
            ? ". You might want to try decreasing the value of decimate if you "
                "are regridding direction axes"
            : ""
        )
    );
    if (this->_getNReplicatedChans() > 1) {
        // spectral channel needs to be replicated _nReplicatedChans times,
        // and spectral coordinate of the template needs to be copied to the
        // output.
        IPosition finalShape = this->_getKludgedShape();
        Int specAxisNumber = workIm->coordinates().spectralAxisNumber(false);
        auto fillerPixels = workIm->get();
        const auto fillerMask = workIm->pixelMask().get();
        finalShape[specAxisNumber] = this->_getNReplicatedChans();
        SPIIT replicatedIm(new casacore::TempImage<T>(finalShape, csys));
        std::dynamic_pointer_cast<casacore::TempImage<T>>(replicatedIm)->attachMask(
            casacore::ArrayLattice<Bool>(finalShape)
        );
        auto& pixelMask = replicatedIm->pixelMask();
        auto n = this->_getNReplicatedChans();
        IPosition where(finalShape.size(), 0);
        for (uInt i=0; i<n; ++i) {
            where[specAxisNumber] = i;
            replicatedIm->putSlice(fillerPixels, where);
            pixelMask.putSlice(fillerMask, where);
        }
        auto spTo = this->_getTemplateCoords().spectralCoordinate();
        auto csysFinal = replicatedIm->coordinates();
        csysFinal.replaceCoordinate(spTo, csysFinal.spectralCoordinateNumber());
        replicatedIm->setCoordinateInfo(csysFinal);
        workIm = replicatedIm;
    }
    return workIm;
}

template<class T> SPIIT ImageRegridder<T>::_decimateStokes(SPIIT workIm) const {
    ImageMetaData<T> md(workIm);
    if (this->_getOutputStokes().size() >= md.nStokes()) {
        return workIm;
    }
    CasacRegionManager rm(workIm->coordinates());
    casacore::String diagnostics;
    casacore::uInt nSelectedChannels = 0;
    if (this->_getOutputStokes().size() == 1) {
        auto stokes = this->_getOutputStokes()[0];
        auto region = rm.fromBCS(
            diagnostics, nSelectedChannels, stokes,
            "", CasacRegionManager::USE_FIRST_STOKES,
            "", workIm->shape()
        ).toRecord("");
        return SubImageFactory<T>::createImage(
            *workIm, "", region, "", false, false, false, false
        );
    }
    else {
        // Only include the wanted stokes
        std::shared_ptr<casacore::ImageConcat<T> > concat(
            new casacore::ImageConcat<T>(
                workIm->coordinates().polarizationAxisNumber(false)
            )
        );
        for(casacore::String stokes: this->_getOutputStokes()) {
            auto region = rm.fromBCS(
                diagnostics, nSelectedChannels, stokes,
                "", CasacRegionManager::USE_FIRST_STOKES,
                "", workIm->shape()
            ).toRecord("");
            concat->setImage(
                *SubImageFactory<T>::createImage(
                    *workIm, "", region, "", false, false, false, false
                ), true
            );
        }
        return concat;
    }
}

template<class T> void ImageRegridder<T>::_checkOutputShape(
    const casacore::SubImage<T>& subImage,
    const std::set<casacore::Coordinate::Type>& coordsToRegrid
) const {
    const auto& csysFrom = subImage.coordinates();
    std::set<casacore::Coordinate::Type> coordsNotToRegrid;
    auto nCoordinates = csysFrom.nCoordinates();
    auto inputShape = subImage.shape();
    auto axes = this->_getAxes();
    auto outputAxisOrder = axes;
    for (uInt i=axes.size(); i<this->_getKludgedShape().size(); ++i) {
        outputAxisOrder.append(casacore::IPosition(1, i));
    }
    const auto coordsToRegridEnd = coordsToRegrid.end();
    for (uInt i=0; i<nCoordinates; ++i) {
        const auto coordType = csysFrom.coordinate(i).type();
        if (coordsToRegrid.find(coordType) == coordsToRegridEnd) {
            auto coordAxes = csysFrom.worldAxes(i);
            for(casacore::uInt oldAxis: coordAxes) {
                casacore::uInt count = 0;
                for(casacore::uInt newAxis: outputAxisOrder) {
                    ThrowIf(
                        newAxis == oldAxis
                        && inputShape[oldAxis]
                            != this->_getKludgedShape()[count],
                        "Input axis " + casacore::String::toString(oldAxis)
                        + " (coordinate type "
                        + casacore::Coordinate::typeToString(coordType)
                        + "), which will not be regridded and corresponds to"
                        + "output axis casacore::String::toString(newAxis), "
                        + "has length "
                        + casacore::String::toString(inputShape[oldAxis])
                        + " where as the specified length of the corresponding "
                        + "output axis is "
                        + casacore::String::toString(
                            this->_getKludgedShape()[count]
                        ) + ". If a coordinate is not regridded, its input and "
                        + "output axes must have the same length."
                    );
                    ++count;
                }
            }
        }
    }
}

template<class T> SPIIT ImageRegridder<T>::_regridByVelocity() const {
    const auto csysTo = this->_getTemplateCoords();
    const auto specCoordTo = csysTo.spectralCoordinate();
    const auto specCoordFrom
        = this->_getImage()->coordinates().spectralCoordinate();
    ThrowIf(
        specCoordTo.frequencySystem(true)
        != specCoordFrom.frequencySystem(true),
        "Image to be regridded has different frequency system from template "
        "coordinate system."
    );
    ThrowIf(
        specCoordTo.restFrequency() == 0,
        "Template spectral coordinate rest frequency is 0, "
        "so cannot regrid by velocity."
    );
    ThrowIf(
        specCoordFrom.restFrequency() == 0,
        "Input image spectral coordinate rest frequency is 0, so cannot regrid "
        "by velocity."
    );
    std::unique_ptr<casacore::CoordinateSystem> csys(
        dynamic_cast<casacore::CoordinateSystem *>(csysTo.clone())
    );
    auto templateSpecCoord = csys->spectralCoordinate();
    std::unique_ptr<casacore::CoordinateSystem> coordClone(
        dynamic_cast<casacore::CoordinateSystem *>(
            _subimage->coordinates().clone()
        )
    );
    auto newSpecCoord = coordClone->spectralCoordinate();
    casacore::Double newVelRefVal = 0;
    std::pair<casacore::Double, casacore::Double> toVelLimits;
    auto inSpecAxis = coordClone->spectralAxisNumber(false);
    casacore::Double newVelInc = 0.0;
    for (casacore::uInt i=0; i<2; ++i) {
        // i == 0 => csysTo, i == 1 => csysFrom
        auto *cs = i == 0 ? csys.get() : coordClone.get();
        // create and replace the coordinate system's spectral coordinate with
        // a linear coordinate which describes the velocity axis. In this way
        // we can regrid by velocity.
        Int specCoordNum = cs->spectralCoordinateNumber();
        auto specCoord = cs->spectralCoordinate();
        if (
            specCoord.frequencySystem(false) != specCoord.frequencySystem(true)
        ) {
            // the underlying conversion system is different from the overlying
            // one, so this is pretty confusing. We want the underlying one also
            // be the overlying one before we regrid.
            casacore::Vector<casacore::Double> newRefVal;
            auto newRefPix = specCoord.referencePixel()[0];
            specCoord.toWorld(
                newRefVal,
                casacore::Vector<casacore::Double>(1, newRefPix)
            );
            casacore::Vector<casacore::Double> newVal;
            specCoord.toWorld(
                newVal, casacore::Vector<casacore::Double>(1, newRefPix+1)
            );
            specCoord = SpectralCoordinate(
                specCoord.frequencySystem(true), newRefVal[0],
                (newVal[0] - newRefVal[0]), newRefPix, specCoord.restFrequency()
            );
            if (cs == coordClone.get()) {
                newSpecCoord = specCoord;
            }
        }
        casacore::Double freqRefVal = specCoord.referenceValue()[0];
        casacore::Double velRefVal;
        ThrowIf(
            ! specCoord.frequencyToVelocity(velRefVal, freqRefVal),
            "Unable to determine reference velocity"
        );
        casacore::Double vel0 = 0;
        casacore::Double vel1 = 0;
        ThrowIf(
            ! specCoord.pixelToVelocity(vel0, 0.0)
            || ! specCoord.pixelToVelocity(vel1, 1.0),
            "Unable to determine velocity increment"
        );
        if (i == 0) {
            toVelLimits.first = vel0;
            specCoord.pixelToVelocity(
                toVelLimits.second, this->_getShape()[inSpecAxis] - 1
            );
            if (toVelLimits.first > toVelLimits.second) {
                std::swap(toVelLimits.first, toVelLimits.second);
            }
        }
        if (i == 1) {
            std::pair<casacore::Double, casacore::Double> fromVelLimits;
            specCoord.pixelToVelocity(fromVelLimits.first, 0);
            specCoord.pixelToVelocity(
                fromVelLimits.second, _subimage->shape()[inSpecAxis] - 1
            );
            if (fromVelLimits.first > fromVelLimits.second) {
                std::swap(fromVelLimits.first, fromVelLimits.second);
            }
            ThrowIf(
                (
                    fromVelLimits.first > toVelLimits.second
                    && ! casacore::near(fromVelLimits.first, toVelLimits.second)
                )
                || (
                    fromVelLimits.second < toVelLimits.first
                    && ! casacore::near(fromVelLimits.second, toVelLimits.first)
                ),
                "Request to regrid by velocity, but input and output velocity "
                "coordinates do not overlap"
            );
        }
        casacore::Matrix<casacore::Double> pc(1, 1, 0);
        pc.diagonal() = 1.0;
        casacore::LinearCoordinate lin(
            casacore::Vector<casacore::String>(1, "velocity"),
            specCoord.worldAxisUnits(),
            casacore::Vector<casacore::Double>(1, velRefVal),
            casacore::Vector<casacore::Double>(1, vel1 - vel0),
            pc, specCoord.referencePixel()
        );
        // don't bother checking the return value of the replaceCoordinate call
        // as it will always be false because the replaced and replacement
        // coordinate types differ, but the coordinate will be replaced anyway.
        // Yes I find it nonintuitive and am scratching my head regarding the
        // usefulness of the return value as well. Just check that replacement
        // coordinate is equal to the coordinate we expect.
        cs->replaceCoordinate(lin, specCoordNum);
        ThrowIf(
            ! lin.near(cs->linearCoordinate(specCoordNum)),
            "Replacement linear coordinate does not match "
            "original linear coordinate because "
            + lin.errorMessage()
        );
        if (cs == csys.get()) {
            newVelRefVal = velRefVal;
            newVelInc = vel1 - vel0;
        }
        else {
            _subimage->setCoordinateInfo(*cs);
        }
    }
    // do not pass the region or the mask, the maskedClone has already had the
    // region and mask applied
    ImageRegridder regridder(
        _subimage, 0, "", this->_getOutname(), this->_getOverwrite(), *csys,
        this->_getAxes(), this->_getShape()
    );
    regridder.setConfiguration(*this);
    auto outImage = regridder._regrid();
    // replace the temporary linear coordinate with the saved spectral
    // coordinate
    std::unique_ptr<casacore::CoordinateSystem> newCoords(
        dynamic_cast<casacore::CoordinateSystem *>(
            outImage->coordinates().clone()
        )
    );
    // make frequencies correct
    casacore::Double newRefFreq;
    ThrowIf(
        ! newSpecCoord.velocityToFrequency(
            newRefFreq, newVelRefVal
        ),
        "Unable to determine new reference frequency"
    );
    // get the new frequency increment
    casacore::Double newFreq;
    ThrowIf (
        ! newSpecCoord.velocityToFrequency(newFreq, newVelRefVal + newVelInc),
        "Unable to determine new frequency increment"
    );
    ThrowIf (
        ! newSpecCoord.setReferenceValue(Vector<Double>(1, newRefFreq)),
        "Unable to set new reference frequency"
    );
    ThrowIf (
        ! newSpecCoord.setIncrement((Vector<Double>(1, newFreq - newRefFreq))),
        "Unable to set new frequency increment"
    );
    ThrowIf(
        ! newSpecCoord.setReferencePixel(
            templateSpecCoord.referencePixel()
        ), "Unable to set new reference pixel"
    );
    ThrowIf(
        ! newCoords->replaceCoordinate(
            newSpecCoord,
            _subimage->coordinates().linearCoordinateNumber())
        && ! newSpecCoord.near(newCoords->spectralCoordinate()),
        "Unable to replace coordinate for velocity regridding"
    );
    outImage->setCoordinateInfo(*newCoords);
    return outImage;
}

template<class T> Bool ImageRegridder<T>::_doImagesOverlap(
    SPCIIT image0, SPCIIT image1
) const {
    const auto csys0 = image0->coordinates();
    const auto csys1 = image1->coordinates();
    auto shape0 = image0->shape();
    auto shape1 = image1->shape();
    ImageMetaData<T> md0(image0);
    ImageMetaData<T> md1(image1);
    Bool overlap = false;
    if (
        csys0.hasDirectionCoordinate()
        && csys1.hasDirectionCoordinate()
    ) {
        const auto dc0 = csys0.directionCoordinate();
        auto dc1 = csys1.directionCoordinate();
        auto dirShape0 = md0.directionShape();
        auto dirShape1 = md1.directionShape();
        auto inc0 = dc0.increment();
        auto inc1 = dc1.increment();
        auto units0 = dc0.worldAxisUnits();
        auto units1 = dc1.worldAxisUnits();
        auto reallyBig = false;
        casacore::Quantity extent;
        casacore::Quantity oneDeg(1, "deg");
        for (uInt i=0; i<2; ++i) {
            extent = casacore::Quantity(dirShape0[i]*abs(inc0[i]), units0[i]);
            if (extent > oneDeg) {
                reallyBig = true;
                break;
            }
            extent = casacore::Quantity(dirShape1[i]*abs(inc1[i]), units1[i]);
            if (extent > oneDeg) {
                reallyBig = true;
                break;
            }
        }
        if (reallyBig) {
            *this->_getLog() << LogOrigin("ImageRegridder", __func__)
                << LogIO::WARN << "At least one of the images "
                << "exceeds one degree on at one side, not checking "
                << "for direction plane overlap." << LogIO::POST;
        }
        else {
            auto sameFrame = dc0.directionType(true) == dc1.directionType(true);
            if (!sameFrame) {
                dc1.setReferenceConversion(dc0.directionType(true));
            }
            auto corners0 = _getDirectionCorners(dc0, dirShape0);
            auto corners1 = _getDirectionCorners(dc1, dirShape1);
            overlap = _doRectanglesIntersect(corners0, corners1);
            if (! overlap) {
                return false;
            }
        }
    }
    if (
        csys0.hasSpectralAxis()
        && csys1.hasSpectralAxis()
    ) {
        const auto sp0 = csys0.spectralCoordinate();
        const auto sp1 = csys1.spectralCoordinate();
        auto nChan0 = md0.nChannels();
        auto nChan1 = md1.nChannels();
        casacore::Double world;
        sp0.toWorld(world, 0);
        auto end00 = world;
        sp0.toWorld(world, nChan0 - 1);
        auto end01 = world;
        sp1.toWorld(world, 0);
        auto end10 = world;
        sp1.toWorld(world, nChan1 - 1);
        auto end11 = world;
        auto minmax0 = minmax(end00, end01);
        auto minmax1 = minmax(end10, end11);
        if (
            (
                minmax0.second < minmax1.first
                && ! casacore::near(minmax0.second, minmax1.first)
            )
            || (
                minmax1.second < minmax0.first
                && ! casacore::near(minmax1.second, minmax0.first)
            )
        ) {
            return false;
        }
    }
    return true;
}

template<class T>
casacore::Vector<std::pair<casacore::Double, casacore::Double>>
ImageRegridder<T>::_getDirectionCorners(
    const casacore::DirectionCoordinate& dc,
    const casacore::IPosition& directionShape
) {
    casacore::Vector<casacore::Double> world;
    casacore::Vector<casacore::Double> pixel(2, 0);
    auto units = dc.worldAxisUnits();
    dc.toWorld(world, pixel);
    casacore::Vector<std::pair<casacore::Double, casacore::Double> > corners(4);
    for (uInt i=0; i<4; ++i) {
        switch(i) {
        case 0:
            // blcx, blcy
            pixel.set(0);
            break;
        case 1:
            // trcx, blcy
            pixel[0] = directionShape[0];
            pixel[1] = 0;
            break;
        case 2:
            // trcx, trcy
            pixel[0] = directionShape[0];
            pixel[1] = directionShape[1];
            break;
        case 3:
            // blcx, trcy
            pixel[0] = 0;
            pixel[1] = directionShape[1];
            break;
        default:
            ThrowCc("Logic Error: This code should never be reached");
            break;
        }
        dc.toWorld(world, pixel);
        auto x = casacore::Quantity(world[0], units[0]).getValue("rad");
        if (fabs(x) >= casacore::C::_2pi) {
            // resolve 2pi ambiguities for x (longitude) coordinate
            x = fmod(x, C::_2pi);
        }
        if (x < 0) {
            // ensure longitude is > 0
            x += casacore::C::_2pi;
        }
        corners[i].first = x;
        corners[i].second = casacore::Quantity(
            world[1], units[1]
        ).getValue("rad");
    }
    auto diff0 = abs(corners[1].first - corners[0].first);  
    auto diff1 = abs(corners[1].first - C::_2pi - corners[0].first);
    if (diff0 > diff1) {
        // image straddles longitude 0 and we have to rationalize the
        // longitude trc coordinate
        corners[1].first -= casacore::C::_2pi;
        corners[2].first -= casacore::C::_2pi;
    }
    return corners;
}

template<class T> casacore::Bool ImageRegridder<T>::_doRectanglesIntersect(
    const casacore::Vector<
        std::pair<casacore::Double, casacore::Double>
    >& corners0,
    const casacore::Vector<
        std::pair<casacore::Double, casacore::Double>
    >& corners1
) {
    auto minx0 = corners0[0].first;
    auto maxx0 = minx0;
    auto miny0 = corners0[0].second;
    auto maxy0 = miny0;
    auto minx1 = corners1[0].first;
    auto maxx1 = minx1;
    auto miny1 = corners1[0].second;
    auto maxy1 = miny1;
    for (casacore::uInt i=1; i<4; ++i) {
        minx0 = min(minx0, corners0[i].first);
        maxx0 = max(maxx0, corners0[i].first);
        miny0 = min(miny0, corners0[i].second);
        maxy0 = max(maxy0, corners0[i].second);

        minx1 = min(minx1, corners1[i].first);
        maxx1 = max(maxx1, corners1[i].first);
        miny1 = min(miny1, corners1[i].second);
        maxy1 = max(maxy1, corners1[i].second);
    }
    if (
        minx0 > maxx1 || maxx0 < minx1
        || miny0 > maxy1 || maxy0 < miny1
    ) {
        // bounds check shows images do not intersect
        return false;
    }
    else if (
        (minx0 >= minx1 && maxx0 <= maxx1 && miny0 >= miny1 && maxy0 <= maxy1)
        || (minx0 < minx1 && maxx0 > maxx1 && miny0 < miny1 && maxy0 > maxy1)
    ) {
        // one image lies completely inside the other
        return true;
    }
    else {
        // determine intersection
        // FIXME There are more efficient algorithms. See eg
        // the Shamos-Hoey Algorithm
        // http://geomalgorithms.com/a09-_intersect-3.html#Pseudo-Code%3a%20S-H
        for (casacore::uInt i=0; i<4; ++i) {
            casacore::Vector<casacore::Double> start0(2, corners0[i].first);
            start0[1] = corners0[i].second;
            casacore::Vector<casacore::Double> end0(
                2,
                i == 3 ? corners0[0].first
                    : corners0[i+1].first
            );
            end0[1] = i == 3 ? corners0[0].second : corners0[i+1].second;

            for (uInt j=0; j<4; ++j) {
                casacore::Vector<casacore::Double> start1(2, corners1[j].first);
                start1[1] = corners1[j].second;
                casacore::Vector<casacore::Double> end1(
                    2,
                    j == 3 ? corners1[0].first
                        : corners1[j+1].first
                );
                end1[1] = j == 3 ? corners1[0].second : corners1[j+1].second;
                if (
                    casacore::Geometry::doLineSegmentsIntersect(
                        start0[0], start0[1], end0[0], end0[1],
                        start1[0], start1[1], end1[0], end1[1]
                    )
                ) {
                    return true;
                    break;
                }
            }
        }
    }
    return false;
}

}
