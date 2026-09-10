// # Copyright (C) 2021,2026
// # Associated Universities, Inc. Washington DC, USA.
// #
// # This library is free software; you can redistribute it and/or modify it
// # under the terms of the GNU Library General Public License as published by
// # the Free Software Foundation; either version 2 of the License, or (at your
// # option) any later version.
// #
// # This library is distributed in the hope that it will be useful, but WITHOUT
// # ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// # FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
// # License for more details.is
// #
// # You should have received a copy of the GNU Library General Public License
// # along with this library; if not, write to the Free Software Foundation,
// # Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
// #
// # Correspondence concerning this should be addressed as follows:
// #        Postal address: National Radio Astronomy Observatory
// #                        1003 Lopezville Road,
// #                        Socorro, NM - 87801, USA
// #
// # $Id$

#include <taylor.h>
#include <cmath>
#include <algorithm>
#include <functional>

using namespace taylor;

namespace
{

    struct ImageAxes
    {
        int dirX = -1;
        int dirY = -1;
        int stokes = -1;
        int freq = -1;          ///< spectral PIXEL axis
        int spectralCoord = -1; ///< spectral COORDINATE index
    };

    /**
     * Resolve and verify the axis layout.
     *
     * The accumulation loops index shape[2] for polarization and shape[3] for
     * channel.  (RA, Dec, Stokes, Frequency) 4D layout.  
     */
    ImageAxes resolveAxes(LogIO &log_l, const CoordinateSystem &coords,
                          const IPosition &shape, const std::string &imgName)
    {
        ImageAxes axes;

        if (shape.size() != 4)
        {
            log_l << "Image " << imgName << " has " << Int(shape.size())
                  << " axes; the Taylor pipeline requires 4 (RA, Dec, Stokes, Frequency)."
                  << LogIO::EXCEPTION;
        }

        int spectralCoord = -1, spectralPixelAxis = -1;
        if (!librautils::findSpectralAxis(coords, shape, spectralCoord, spectralPixelAxis))
        {
            log_l << "Image " << imgName << " has no usable spectral pixel axis."
                  << LogIO::EXCEPTION;
        }

        const int dirCoord = coords.findCoordinate(Coordinate::DIRECTION);
        if (dirCoord < 0)
        {
            log_l << "Image " << imgName << " has no direction coordinate." << LogIO::EXCEPTION;
        }
        const Vector<Int> dirAxes = coords.pixelAxes(dirCoord);
        if (dirAxes.nelements() != 2)
        {
            log_l << "Image " << imgName << " direction coordinate does not span two pixel axes."
                  << LogIO::EXCEPTION;
        }

        const int stokesCoord = coords.findCoordinate(Coordinate::STOKES);
        if (stokesCoord < 0)
        {
            log_l << "Image " << imgName << " has no Stokes coordinate." << LogIO::EXCEPTION;
        }
        const Vector<Int> stokesAxes = coords.pixelAxes(stokesCoord);
        if (stokesAxes.nelements() != 1)
        {
            log_l << "Image " << imgName << " Stokes coordinate does not span one pixel axis."
                  << LogIO::EXCEPTION;
        }

        axes.dirX = dirAxes[0];
        axes.dirY = dirAxes[1];
        axes.stokes = stokesAxes[0];
        axes.freq = spectralPixelAxis;
        axes.spectralCoord = spectralCoord;

        if (axes.dirX != 0 || axes.dirY != 1 || axes.stokes != 2 || axes.freq != 3)
        {
            log_l << "Image " << imgName << " has a non-canonical axis order (direction "
                  << axes.dirX << "," << axes.dirY << " stokes " << axes.stokes
                  << " frequency " << axes.freq
                  << ").  The Taylor pipeline requires (RA, Dec, Stokes, Frequency)."
                  << LogIO::EXCEPTION;
        }

        return axes;
    }

    // ----------------------------------------------------------------------
    // Small shared helpers
    // ----------------------------------------------------------------------

    /// Open an image, creating it zero filled if it is not on disk yet.
    std::shared_ptr<PagedImage<float>> openOrCreate(LogIO &log_l, const std::string &imgName,
                                                    const IPosition &shape,
                                                    const CoordinateSystem &coord,
                                                    const std::string &what)
    {
        if (!librautils::imageExists(imgName))
        {
            log_l << what << " image " << imgName
                  << " does not exist on disk.  Creating it and initializing pixels to zero."
                  << LogIO::WARN << LogIO::POST;
            PagedImage<float> created(shape, coord, imgName);
            Array<float> zeros(shape);
            zeros.set(0.0f);
            created.put(zeros);
            created.unlock();
            created.flush();
        }
        else
        {
            log_l << what << " image " << imgName
                  << " exists on disk.  Will load from disk to be filled."
                  << LogIO::NORMAL << LogIO::POST;
        }
        return std::make_shared<PagedImage<float>>(imgName);
    }

    /// Signature of the per-image lattice expression built by applyImageExpr.
    using ExprFn = std::function<LatticeExpr<float>(ImageInterface<float> & cube,
                                                    ImageInterface<float> & pb,
                                                    ImageInterface<float> * avgpb)>;

    /**
     * The guarded in-place edit shared by applyPB, removeFreqDepPB and
     * applyFreqDepPB.  Those three differed in exactly four substantive lines:
     * the log origin, which PB goes with which cube, the lattice expression,
     * and the success message.
     *
     * `pbnames` is either one image shared by every cube, or one per cube.
     * `avgpbname` is optional; when given it must exist and match shapes.
     */
    void applyImageExpr(LogIO &log_l,
                        const std::vector<std::string> &cubenames,
                        const std::vector<std::string> &pbnames,
                        const std::string &avgpbname,
                        const ExprFn &makeExpr,
                        const std::string &what)
    {
        if (cubenames.empty())
        {
            log_l << what << ": no cube images given." << LogIO::EXCEPTION;
        }
        if (pbnames.size() != 1 && pbnames.size() != cubenames.size())
        {
            log_l << what << ": expected either one shared PB or one PB per cube ("
                  << Int(cubenames.size()) << "), got " << Int(pbnames.size()) << "."
                  << LogIO::EXCEPTION;
        }

        std::shared_ptr<PagedImage<float>> avgPBImage;
        if (!avgpbname.empty())
        {
            if (!librautils::imageExists(avgpbname))
            {
                log_l << "avgPB image " << avgpbname << " does not exist on disk." << LogIO::EXCEPTION;
            }
            avgPBImage = std::make_shared<PagedImage<float>>(avgpbname);
        }

        for (const auto &name : pbnames)
        {
            if (!librautils::imageExists(name))
            {
                log_l << "PB image " << name << " does not exist on disk." << LogIO::EXCEPTION;
            }
        }

        for (size_t i = 0; i < cubenames.size(); ++i)
        {
            if (!librautils::imageExists(cubenames[i]))
            {
                log_l << "Cube image " << cubenames[i] << " does not exist on disk." << LogIO::EXCEPTION;
            }

            PagedImage<float> cubeImage(cubenames[i]);
            PagedImage<float> pbImage(pbnames.size() == 1 ? pbnames[0] : pbnames[i]);

            if (pbImage.shape() != cubeImage.shape())
            {
                log_l << "Shape mismatch between PB image and cube image " << cubenames[i]
                      << LogIO::EXCEPTION;
            }
            if (avgPBImage && avgPBImage->shape() != cubeImage.shape())
            {
                log_l << "Shape mismatch between avgPB image and cube image " << cubenames[i]
                      << LogIO::EXCEPTION;
            }

            cubeImage.copyData(makeExpr(cubeImage, pbImage, avgPBImage.get()));
            cubeImage.flush();
            cubeImage.unlock();

            log_l << "Successfully applied " << what << " to " << cubenames[i]
                  << LogIO::NORMAL << LogIO::POST;
        }
    }

    /**
     * Reduce a set of same-shaped images to one, pixel by pixel.
     *
     * Both mean and median run through here, and every image is read into
     * memory exactly once.  The previous median path indexed 4D lattices with a
     * one dimensional IPosition, which does not address a pixel of a 4D
     * lattice, and re-read from disk once per pixel per image.
     */
    Array<float> reduceAcrossImages(const std::vector<std::string> &names,
                                    const IPosition &shape, bool median)
    {
        std::vector<Array<float>> planes;
        planes.reserve(names.size());
        for (const auto &name : names)
        {
            PagedImage<float> img(name);
            planes.push_back(img.get());
            img.unlock();
        }

        Array<float> result(shape);
        const size_t npix = shape.product();
        std::vector<float> values(planes.size());

        float *out = result.data();
        std::vector<const float *> in(planes.size());
        for (size_t k = 0; k < planes.size(); ++k)
        {
            in[k] = planes[k].data();
        }

        for (size_t i = 0; i < npix; ++i)
        {
            for (size_t k = 0; k < in.size(); ++k)
            {
                values[k] = in[k][i];
            }
            if (median)
            {
                std::sort(values.begin(), values.end());
                const size_t mid = values.size() / 2;
                out[i] = (values.size() % 2 == 0)
                             ? (values[mid - 1] + values[mid]) / 2.0f
                             : values[mid];
            }
            else
            {
                // Accumulate in float, in input order, then divide by the count
                // as a float.  This reproduces the LatticeExpr chain the mean
                // path used to build (sum = p0; sum = sum + p1; ...; sum / n)
                // exactly; accumulating in double would round differently and
                // silently move every avgPB in the pipeline.
                float sum = values[0];
                for (size_t k = 1; k < values.size(); ++k)
                {
                    sum += values[k];
                }
                out[i] = sum / static_cast<float>(values.size());
            }
        }
        return result;
    }
} 

// --------------------------------------------------------------------------
// Construction and configuration
// --------------------------------------------------------------------------

Taylor::Taylor(const std::string &reffreq, int nterms, float pblimit,
               ImageType imtype, bool overwrite)
    : reffreq_(reffreq), nterms_(nterms), pblimit_(pblimit),
      imtype_(imtype), overwrite_(overwrite)
{
}

int Taylor::outputTermCount() const
{
    // The PSFs fill the upper triangle of the Hessian, which needs 2*nterms-1
    // elements.  The sumwt terms weight that same Hessian, so they follow it.
    return (imtype_ == PSF || imtype_ == SUMWT) ? 2 * nterms_ - 1 : nterms_;
}

void Taylor::checkRefFreqRange(LogIO &log_l, double refval, const std::string &reffreq_str,
                               const std::vector<double> &midFreqs)
{
    if (midFreqs.empty())
    {
        log_l << "Cannot check the reference frequency range: no mid-frequencies were computed."
              << LogIO::EXCEPTION;
    }

    const double minMid = *std::min_element(midFreqs.begin(), midFreqs.end());
    const double maxMid = *std::max_element(midFreqs.begin(), midFreqs.end());

    if (refval < minMid || refval > maxMid)
    {
        log_l << "WARNING: Reference frequency " << reffreq_str << " (" << refval
              << " Hz) is not within the range of the cube mid-frequencies (" << minMid
              << " Hz to " << maxMid << " Hz)." << LogIO::WARN << LogIO::POST;
    }
}

// --------------------------------------------------------------------------
// Image slicing
// --------------------------------------------------------------------------

std::shared_ptr<ImageInterface<float>> Taylor::getImageSlice(const std::string &imgName,
                                                             const int facet, const int nfacets,
                                                             const int chan, const int nchanchunks,
                                                             const int pol, const int npolchunks)
{
    LogIO os(LogOrigin("Taylor", "getImageSlice"));

    const int nx_facets = static_cast<int>(sqrt(static_cast<double>(nfacets)));
    if ((facet > (nfacets - 1)) || (facet < 0))
    {
        os << "Illegal facet " << facet << LogIO::EXCEPTION;
    }

    PagedImage<float> image(imgName);
    IPosition imshp = image.shape();
    IPosition blc(imshp.nelements(), 0);
    IPosition trc = imshp - 1;
    IPosition inc(imshp.nelements(), 1);

    // Facets
    const int facetx = facet % nx_facets;
    const int facety = (facet - facetx) / nx_facets;
    const int sizex = imshp(0) / nx_facets;
    const int sizey = imshp(1) / nx_facets;
    blc(1) = facety * sizey;
    trc(1) = blc(1) + sizey - 1;
    blc(0) = facetx * sizex;
    trc(0) = blc(0) + sizex - 1;

    // Pol chunks
    const int sizepol = imshp(2) / npolchunks;
    blc(2) = pol * sizepol;
    trc(2) = blc(2) + sizepol - 1;

    // Chan chunks
    const int sizechan = imshp(3) / nchanchunks;
    blc(3) = chan * sizechan;
    trc(3) = blc(3) + sizechan - 1;

    LCBox::verify(blc, trc, inc, imshp);
    Slicer imslice(blc, trc, inc, Slicer::endIsLast);

    std::shared_ptr<ImageInterface<float>> referenceImage(new SubImage<float>(image, imslice, true));
    {
        LatticeLocker lock1(*referenceImage, FileLocker::Write);
        referenceImage->setMiscInfo(image.miscInfo());
        referenceImage->setUnits(image.units());
    }

    return referenceImage;
}

// --------------------------------------------------------------------------
// Way In
// --------------------------------------------------------------------------

void Taylor::validateCubeToTaylorInputs(LogIO &log_l,
                                        const std::vector<std::string> &cubenames,
                                        const std::vector<std::string> &mtnames,
                                        const std::vector<std::string> &pbnames,
                                        const std::vector<std::string> &sumwtnames) const
{
    if (nterms_ < 1)
    {
        log_l << "nterms must be at least 1, got " << nterms_ << "." << LogIO::EXCEPTION;
    }
    if (imtype_ == INVALID)
    {
        log_l << "imtype was not set." << LogIO::EXCEPTION;
    }
    if (cubenames.empty())
    {
        log_l << "No cube images given." << LogIO::EXCEPTION;
    }
    if (mtnames.empty() || mtnames[0].empty())
    {
        log_l << "No Taylor image prefix given." << LogIO::EXCEPTION;
    }
    if (reffreq_.empty())
    {
        log_l << "No reference frequency given." << LogIO::EXCEPTION;
    }

    // The pbnames are exactly one per-SPW PB per cube, in the same order,
    // with the avgPB appended last.  
    if (pbnames.size() != cubenames.size() + 1)
    {
        log_l << "Expected one PB per cube image plus the avgPB appended last, i.e. "
              << Int(cubenames.size() + 1) << " entries, but got " << Int(pbnames.size())
              << "." << LogIO::EXCEPTION;
    }

    if (imtype_ != SUMWT && sumwtnames.size() != cubenames.size())
    {
        log_l << "Expected one sumwt image per cube image (" << Int(cubenames.size())
              << "), got " << Int(sumwtnames.size()) << "." << LogIO::EXCEPTION;
    }
}

void Taylor::createTermImages(LogIO &log_l, const std::string &prefix,
                              const std::vector<int> &templateShape,
                              const CoordinateSystem &templateCoord,
                              double minFreq, double maxFreq, int spectralCoordIndex) const
{
 
    IPosition shape(templateShape.size());
    for (size_t i = 0; i < templateShape.size(); ++i)
    {
        shape(i) = templateShape[i];
    }
    shape(3) = 1; // Taylor terms are single channel.

    CoordinateSystem coord = templateCoord;
    SpectralCoordinate speccoord = coord.spectralCoordinate(spectralCoordIndex);
    speccoord.setReferencePixel(Vector<double>(1, 0.0));
    speccoord.setReferenceValue(Vector<double>(1, (minFreq + maxFreq) / 2.0));
    speccoord.setIncrement(Vector<double>(1, std::max(1e-9, maxFreq - minFreq)));
    coord.replaceCoordinate(speccoord, spectralCoordIndex);

    for (int tt = 0; tt < outputTermCount(); ++tt)
    {
        const std::string imgName = prefix + ".tt" + std::to_string(tt);

        bool recreate = !librautils::imageExists(imgName);
        if (!recreate)
        {
            // Read the metadata in a scope that closes before any delete.
            // I got bit by this.
            IPosition existingShape;
            {
                PagedImage<float> existing(imgName);
                existingShape = existing.shape();
                existing.unlock();
            }
            if (existingShape != shape)
            {
                log_l << "Taylor image " << imgName << " has shape " << existingShape
                      << " but " << shape << " is required.  Recreating it."
                      << LogIO::WARN << LogIO::POST;
                TableUtil::deleteTable(imgName);
                recreate = true;
            }
        }

        if (recreate)
        {
            log_l << "Creating Taylor image " << imgName << " and initializing it to zero."
                  << LogIO::NORMAL << LogIO::POST;
            PagedImage<float> created(shape, coord, imgName);
            Array<float> zeros(shape);
            zeros.set(0.0f);
            created.put(zeros);
            created.unlock();
            created.flush();
        }
        else
        {
            PagedImage<float> existing(imgName);
            existing.setCoordinateInfo(coord);
            Array<float> zeros(existing.shape());
            zeros.set(0.0f);
            existing.put(zeros);
            existing.unlock();
            existing.flush();
        }
    }
}

void Taylor::createPBMasks(LogIO &log_l, const std::string &prefix) const
{
    log_l << "Creating PB masks with pblimit=" << pblimit_ << LogIO::NORMAL << LogIO::POST;

    for (int tt = 0; tt < outputTermCount(); ++tt)
    {
        const std::string imgName = prefix + ".tt" + std::to_string(tt);

        // A mask that failed to be written used to be a warning,
        // which left an unmasked PB indistinguishable from a masked one.
        PagedImage<float> pb_taylor_img(imgName);

        if (pb_taylor_img.hasPixelMask())
        {
            pb_taylor_img.removeRegion("mask0");
            log_l << "Removed existing mask from " << imgName << LogIO::NORMAL << LogIO::POST;
        }

        LatticeExpr<bool> pbmask(iif(pb_taylor_img > std::abs(pblimit_), true, false));

        // One makeMask call, not two.  

        pb_taylor_img.makeMask("mask0", true, true, true, true);

        pb_taylor_img.pixelMask().copyData(pbmask);
        pb_taylor_img.pixelMask().unlock();
        pb_taylor_img.unlock();

        log_l << "Created PB mask for " << imgName << " with threshold "
              << std::abs(pblimit_) << LogIO::NORMAL << LogIO::POST;
    }
}

void Taylor::cubeToTaylorSum(const std::vector<std::string> &cubenames,
                             const std::vector<std::string> &mtnames,
                             const std::vector<std::string> &pbnames,
                             const std::vector<std::string> &sumwtnames)
{
    LogIO log_l(LogOrigin("Taylor", "cubeToTaylorSum"));

    validateCubeToTaylorInputs(log_l, cubenames, mtnames, pbnames, sumwtnames);

    std::vector<std::vector<int>> cubeShapes, pbShapes, sumwtShapes;
    std::vector<std::vector<double>> cubeFreqs, pbFreqs, sumwtFreqs;

    librautils::checkAndGetImageInfo(log_l, cubenames, "Cube", cubeShapes, cubeFreqs, true);
    librautils::checkAndGetImageInfo(log_l, pbnames, "PB", pbShapes, pbFreqs);
    if (imtype_ == SUMWT)
    {
        if (!sumwtnames.empty())
        {
            log_l << "Ignoring sumwtImage as imtype is SUMWT." << LogIO::WARN << LogIO::POST;
        }
    }
    else
    {
        librautils::checkAndGetImageInfo(log_l, sumwtnames, "Sumwt", sumwtShapes, sumwtFreqs);
        librautils::checkShapesConsistency(log_l, sumwtShapes, "Sumwt");
    }
    librautils::checkShapesConsistency(log_l, cubeShapes, "Cube");
    librautils::checkShapesConsistency(log_l, pbShapes, "PB");

    const int outnterms = outputTermCount();
    log_l << "Writing " << outnterms << " Taylor terms." << LogIO::NORMAL << LogIO::POST;

    const double refval = librautils::parseRefFreq(log_l, reffreq_);
    const std::vector<double> midFreqs = librautils::computeMidFrequencies(cubeFreqs);
    if (midFreqs.empty())
    {
        log_l << "No mid-frequencies could be computed from the cube images." << LogIO::EXCEPTION;
    }
    const double minFreq = *std::min_element(midFreqs.begin(), midFreqs.end());
    const double maxFreq = *std::max_element(midFreqs.begin(), midFreqs.end());
    log_l << "Mid frequencies span " << minFreq << " Hz to " << maxFreq << " Hz."
          << LogIO::NORMAL << LogIO::POST;

    // The Way Out already checked this; an out-of-band reffreq on the Way In
    // silently produced extrapolated weights.
    checkRefFreqRange(log_l, refval, reffreq_, midFreqs);

    const std::vector<double> weights = librautils::computeWeights(midFreqs, refval);
    for (size_t i = 0; i < weights.size(); ++i)
    {
        log_l << "Weight for cube " << i << " (from midFreq " << midFreqs[i] << "): "
              << weights[i] << LogIO::NORMAL << LogIO::POST;
    }

    CoordinateSystem coord;
    IPosition firstCubeShape;
    {
        PagedImage<float> firstCubeImage(cubenames[0]);
        coord = firstCubeImage.coordinates();
        firstCubeShape = firstCubeImage.shape();
        firstCubeImage.unlock();
    }
    const ImageAxes axes = resolveAxes(log_l, coord, firstCubeShape, cubenames[0]);

    const std::string prefix = mtnames[0];
    createTermImages(log_l, prefix, cubeShapes[0], coord, minFreq, maxFreq, axes.spectralCoord);

    // PB and SUMWT accumulate the cube alone; everything else is premultiplied
    // by the per-SPW PB.  CASA writes this as `if (imtype < 2)` in
    // SynthesisUtilMethods::cubeToTaylorSum; 
    // MODEL is a libra-only type and keeps the behavior it has always
    // had, so the only type whose treatment changes here is SUMWT.
    //
    // Multiplying SUMWT a [1,1,1,1]
    // sumwt image against a full size PB makes the expression take the PB's
    // shape, and copyData then asserts against the [1,1,1,1] Taylor term.
    const bool premultiplyByPB = (imtype_ != PB && imtype_ != SUMWT);

    for (int pol = 0; pol < firstCubeShape(axes.stokes); ++pol)
    {
        std::vector<std::shared_ptr<ImageInterface<float>>> mt_subims(outnterms);
        std::vector<float> wtsum(outnterms, 0.0f);

        for (int tt = 0; tt < outnterms; ++tt)
        {
            const std::string imgName = prefix + ".tt" + std::to_string(tt);
            mt_subims[tt] = getImageSlice(imgName, 0, 1, 0, 1, pol, firstCubeShape(axes.stokes));
            mt_subims[tt]->copyData(LatticeExpr<float>(0.0f));
            mt_subims[tt]->flush();
        }

        for (size_t idx = 0; idx < cubenames.size(); ++idx)
        {
            const std::vector<int> &currentCubeShape = cubeShapes[idx];
            const std::vector<int> &currentPbShape = pbShapes[idx];

            // Open the sumwt image once per cube rather than once per channel.
            std::shared_ptr<PagedImage<float>> sumwtImage;
            if (imtype_ != SUMWT)
            {
                sumwtImage = std::make_shared<PagedImage<float>>(sumwtnames[idx]);
            }

            for (int chan = 0; chan < currentCubeShape[axes.freq]; ++chan)
            {
                std::shared_ptr<ImageInterface<float>> cube_subim =
                    getImageSlice(cubenames[idx], 0, 1, chan, currentCubeShape[axes.freq],
                                  pol, currentCubeShape[axes.stokes]);

                std::shared_ptr<ImageInterface<float>> pb_subim;
                if (premultiplyByPB)
                {
                    pb_subim = getImageSlice(pbnames[idx], 0, 1, chan, currentPbShape[axes.freq],
                                             pol, currentPbShape[axes.stokes]);
                }

                float lsumwt = 1.0f;
                if (imtype_ != SUMWT)
                {
                    IPosition pos_sumwt(sumwtImage->shape());
                    pos_sumwt(axes.dirX) = 0;
                    pos_sumwt(axes.dirY) = 0;
                    pos_sumwt(axes.stokes) = pol;
                    pos_sumwt(axes.freq) = chan;
                    lsumwt = sumwtImage->getAt(pos_sumwt);
                }

                const double current_cube_weight = weights[idx];

                for (int tt = 0; tt < outnterms; ++tt)
                {
                    const double fac = std::pow(current_cube_weight, tt);

                    LatticeExpr<float> cube_subim_masked =
                        iif(isNaN(*cube_subim), 0.0f, *cube_subim);

                    if (premultiplyByPB)
                    {
                        mt_subims[tt]->copyData(LatticeExpr<float>(
                            (*mt_subims[tt]) + ((fac)*cube_subim_masked * (*pb_subim) * lsumwt)));
                    }
                    else
                    {
                        mt_subims[tt]->copyData(LatticeExpr<float>(
                            (*mt_subims[tt]) + ((fac)*cube_subim_masked * lsumwt)));
                    }
                    mt_subims[tt]->flush();

                    wtsum[tt] += lsumwt;
                }
            }

            if (sumwtImage)
            {
                sumwtImage->unlock();
            }
        }

        for (int tt = 0; tt < outnterms; ++tt)
        {
            const std::string imgName = prefix + ".tt" + std::to_string(tt);

            if (wtsum[tt] == 0.0f || std::isnan(wtsum[tt]))
            {
                // The term is undefined.  Leaving the un-normalized accumulation
                // on disk would look like a real answer.
                log_l << "wtsum for " << imgName << " (pol " << pol << ") is " << wtsum[tt]
                      << "; the term is undefined and will be zeroed."
                      << LogIO::WARN << LogIO::POST;
                mt_subims[tt]->copyData(LatticeExpr<float>(0.0f));
                mt_subims[tt]->flush();
                continue;
            }

            log_l << "wtsum[" << tt << "] = " << wtsum[tt] << LogIO::NORMAL << LogIO::POST;

            LatticeExprNode preNormMaxNode = max(*mt_subims[tt]);
            log_l << "Pre-normalization max for " << imgName << " (pol " << pol << ") is "
                  << preNormMaxNode.getFloat() << LogIO::NORMAL << LogIO::POST;

            LatticeExpr<float> normed;
            if ((imtype_ == PSF || imtype_ == RESIDUAL) && pblimit_ > 0.0f)
            {
                // Way-In avgPB divide.  This turns the Sault weighted sum
                // S*avgPB^2 into the flat noise RHS S*avgPB that CASA's mtmfs is
                // also given.  pbnames.back() is the avgPB by contract.
                std::shared_ptr<ImageInterface<float>> pb_subim_norm =
                    getImageSlice(pbnames.back(), 0, 1, 0, 1, pol, firstCubeShape(axes.stokes));

                normed = LatticeExpr<float>(iif(*pb_subim_norm > pblimit_,
                                                (*mt_subims[tt]) / wtsum[tt] / (*pb_subim_norm),
                                                0.0f));
                log_l << "Applied the avgPB divide with pblimit=" << pblimit_ << " for "
                      << imgName << " (pol " << pol << ")" << LogIO::NORMAL << LogIO::POST;
            }
            else
            {
                normed = LatticeExpr<float>((*mt_subims[tt]) / wtsum[tt]);
            }
            mt_subims[tt]->copyData(normed);
            mt_subims[tt]->flush();

            LatticeExprNode postNormMaxNode = max(*mt_subims[tt]);
            log_l << "Post-normalization max for " << imgName << " (pol " << pol << ") is "
                  << postNormMaxNode.getFloat() << LogIO::NORMAL << LogIO::POST;
        }

        for (int tt = 0; tt < outnterms; ++tt)
        {
            mt_subims[tt]->unlock();
        }
    }

    if (imtype_ == PB && pblimit_ > 0.0f)
    {
        createPBMasks(log_l, prefix);
    }
}

// --------------------------------------------------------------------------
// Way Out
// --------------------------------------------------------------------------

void Taylor::taylorToCube(const std::vector<std::string> &mtnames,
                          const std::vector<std::string> &cubenames,
                          const std::vector<std::string> &pbnames)
{
    LogIO log_l(LogOrigin("Taylor", "taylorToCube"));

    if (nterms_ < 1)
    {
        log_l << "nterms must be at least 1, got " << nterms_ << "." << LogIO::EXCEPTION;
    }
    if (cubenames.empty())
    {
        log_l << "No cube images given." << LogIO::EXCEPTION;
    }
    if (static_cast<int>(mtnames.size()) < nterms_)
    {
        log_l << "Expected at least " << nterms_ << " Taylor images, got "
              << Int(mtnames.size()) << "." << LogIO::EXCEPTION;
    }
    if (reffreq_.empty())
    {
        log_l << "No reference frequency given." << LogIO::EXCEPTION;
    }

    std::vector<std::vector<int>> mtShapes, cubeShapes, pbShapes;
    std::vector<std::vector<double>> mtFreqs, cubeFreqs, pbFreqs;

    librautils::checkAndGetImageInfo(log_l, mtnames, "MT", mtShapes, mtFreqs);
    librautils::checkAndGetImageInfo(log_l, cubenames, "Cube", cubeShapes, cubeFreqs, false, true);
    if (!pbnames.empty())
    {
        librautils::checkAndGetImageInfo(log_l, pbnames, "PB", pbShapes, pbFreqs);
        librautils::checkShapesConsistency(log_l, pbShapes, "PB");
    }
    librautils::checkShapesConsistency(log_l, cubeShapes, "Cube");

    const double refval = librautils::parseRefFreq(log_l, reffreq_);
    const std::vector<double> midFreqs = librautils::computeMidFrequencies(cubeFreqs);
    if (midFreqs.empty())
    {
        log_l << "No mid-frequencies could be computed from the cube images." << LogIO::EXCEPTION;
    }
    checkRefFreqRange(log_l, refval, reffreq_, midFreqs);

    ImageAxes axes;
    {
        PagedImage<float> firstCubeImage(cubenames[0]);
        axes = resolveAxes(log_l, firstCubeImage.coordinates(), firstCubeImage.shape(), cubenames[0]);
        firstCubeImage.unlock();
    }

    for (size_t idx = 0; idx < cubenames.size(); ++idx)
    {
        const std::vector<int> &currentCubeShape = cubeShapes[idx];

        for (int chan = 0; chan < currentCubeShape[axes.freq]; ++chan)
        {
            for (int pol = 0; pol < currentCubeShape[axes.stokes]; ++pol)
            {
                std::shared_ptr<ImageInterface<float>> cube_subim =
                    getImageSlice(cubenames[idx], 0, 1, chan, currentCubeShape[axes.freq],
                                  pol, currentCubeShape[axes.stokes]);

                cube_subim->copyData(LatticeExpr<float>(0.0f));

                for (int tt = 0; tt < nterms_; ++tt)
                {
                    // Accept both "foo.tt0" and the "foo" prefix.
                    const std::string suffix = ".tt" + std::to_string(tt);
                    const std::string imgName =
                        (mtnames[tt].find(suffix) != std::string::npos) ? mtnames[tt]
                                                                        : mtnames[tt] + suffix;

                    // The Taylor image's spectral axis is length 1, so always
                    // take channel 0 from it.
                    std::shared_ptr<ImageInterface<float>> mt_subim =
                        getImageSlice(imgName, 0, 1, 0, 1, pol, currentCubeShape[axes.stokes]);

                    const double wt = (midFreqs[idx] - refval) / refval;
                    const double fac = std::pow(wt, tt);

                    cube_subim->copyData(
                        LatticeExpr<float>((*cube_subim) + (fac) * (*mt_subim)));

                    mt_subim->flush();
                    mt_subim->unlock();
                }
                cube_subim->flush();
                cube_subim->unlock();
            }
        }
    }

    // Remove the avgPB baked in on the Way In.  The caller then applies the
    // per-SPW PB via applyPB("multiply").
    if (!pbnames.empty())
    {
        applyPB(pbnames[0], cubenames, "divide");
    }
}

// --------------------------------------------------------------------------
// Derived products
// --------------------------------------------------------------------------

void Taylor::computeAlpha(const std::vector<std::string> &mtnames,
                          const std::vector<std::string> &alphaimages)
{
    LogIO log_l(LogOrigin("Taylor", "computeAlpha"));

    if (nterms_ < 2)
    {
        log_l << "computeAlpha needs at least 2 Taylor terms, got " << nterms_ << "."
              << LogIO::EXCEPTION;
    }
    if (mtnames.empty() || mtnames[0].empty())
    {
        log_l << "No Taylor image prefix given." << LogIO::EXCEPTION;
    }
    if (alphaimages.empty() || alphaimages[0].empty())
    {
        log_l << "No output image prefix given." << LogIO::EXCEPTION;
    }

    // mtnames[0] is a prefix, exactly as it is for cubeToTaylorSum.
    const std::string prefix = mtnames[0];
    std::vector<std::string> termNames;
    for (int tt = 0; tt < nterms_; ++tt)
    {
        termNames.push_back(prefix + ".tt" + std::to_string(tt));
    }

    std::vector<std::vector<int>> mtShapes;
    std::vector<std::vector<double>> mtFreqs;
    librautils::checkAndGetImageInfo(log_l, termNames, "MT", mtShapes, mtFreqs);
    librautils::checkShapesConsistency(log_l, mtShapes, "MT");

    IPosition shape;
    CoordinateSystem coord;
    ImageAxes axes;
    {
        PagedImage<float> first(termNames[0]);
        shape = first.shape();
        coord = first.coordinates();
        axes = resolveAxes(log_l, coord, shape, termNames[0]);
        first.unlock();
    }

    const std::string alphaName = alphaimages[0] + ".alpha";
    const bool wantBeta = (nterms_ > 2);
    if (wantBeta && alphaimages.size() < 2)
    {
        log_l << "computeAlpha with nterms > 2 needs a second output name for beta."
              << LogIO::EXCEPTION;
    }
    const std::string betaName = wantBeta ? alphaimages[1] + ".beta" : "";

    openOrCreate(log_l, alphaName, shape, coord, "Alpha")->unlock();
    if (wantBeta)
    {
        openOrCreate(log_l, betaName, shape, coord, "Beta")->unlock();
    }

    for (int pol = 0; pol < shape(axes.stokes); ++pol)
    {
        for (int chan = 0; chan < shape(axes.freq); ++chan)
        {
            std::vector<std::shared_ptr<ImageInterface<float>>> mt_subims(nterms_);
            for (int tt = 0; tt < nterms_; ++tt)
            {
                mt_subims[tt] = getImageSlice(termNames[tt], 0, 1, chan, shape(axes.freq),
                                              pol, shape(axes.stokes));
            }

            std::shared_ptr<ImageInterface<float>> alpha_subim =
                getImageSlice(alphaName, 0, 1, chan, shape(axes.freq), pol, shape(axes.stokes));
            LatticeExpr<float> alpha((*mt_subims[1]) / (*mt_subims[0]));
            alpha_subim->copyData(alpha);

            if (wantBeta)
            {
                std::shared_ptr<ImageInterface<float>> beta_subim =
                    getImageSlice(betaName, 0, 1, chan, shape(axes.freq), pol, shape(axes.stokes));
                beta_subim->copyData(LatticeExpr<float>(
                    (*mt_subims[2] / *mt_subims[0]) - (alpha * (alpha - 1) / 2)));
                beta_subim->flush();
                beta_subim->unlock();
            }

            alpha_subim->flush();
            alpha_subim->unlock();
        }
    }

    log_l << "Alpha" << (wantBeta ? " and Beta images" : " image") << " computed and saved to disk."
          << LogIO::NORMAL << LogIO::POST;
}

void Taylor::computeAvgPB(const std::vector<std::string> &pbnames, const std::string &avgpbname,
                          const std::string &minfreqpbname, const std::string &avgpbmode)
{
    LogIO log_l(LogOrigin("Taylor", "computeAvgPB"));

    if (pbnames.empty())
    {
        log_l << "No PB images given." << LogIO::EXCEPTION;
    }
    if (avgpbname.empty())
    {
        log_l << "No avgpbname given." << LogIO::EXCEPTION;
    }
    if (avgpbmode != "mean" && avgpbmode != "median")
    {
        log_l << "Unknown avgpbmode '" << avgpbmode << "'.  Must be 'mean' or 'median'."
              << LogIO::EXCEPTION;
    }

    std::vector<std::vector<int>> pbShapes;
    std::vector<std::vector<double>> pbFreqs;
    librautils::checkAndGetImageInfo(log_l, pbnames, "PB", pbShapes, pbFreqs);
    librautils::checkShapesConsistency(log_l, pbShapes, "PB");

    IPosition shape;
    CoordinateSystem coord;
    {
        PagedImage<float> first(pbnames[0]);
        shape = first.shape();
        coord = first.coordinates();
        first.unlock();
    }

    // Identify the lowest frequency PB, and copy it out if asked for.
    double minfreq = std::numeric_limits<double>::max();
    double maxfreq = 0.0;
    std::string minfreqname;
    for (size_t i = 0; i < pbFreqs.size(); ++i)
    {
        if (pbFreqs[i].empty())
        {
            continue;
        }
        if (pbFreqs[i].front() < minfreq)
        {
            minfreq = pbFreqs[i].front();
            minfreqname = pbnames[i];
        }
        if (pbFreqs[i].back() > maxfreq)
        {
            maxfreq = pbFreqs[i].back();
        }
    }
    if (minfreqname.empty())
    {
        log_l << "None of the PB images carry frequency information." << LogIO::EXCEPTION;
    }
    log_l << "Minimum frequency is " << minfreq << " Hz and maximum is " << maxfreq << " Hz."
          << LogIO::NORMAL << LogIO::POST;

    if (!minfreqpbname.empty() && !librautils::imageExists(minfreqpbname))
    {
        log_l << "Copying the lowest frequency PB " << minfreqname << " to " << minfreqpbname
              << LogIO::NORMAL << LogIO::POST;
        PagedImage<float> minfreqImage(minfreqname);
        PagedImage<float> minfreqPBImage(minfreqImage.shape(), minfreqImage.coordinates(),
                                         minfreqpbname);
        minfreqPBImage.copyData(minfreqImage);
        minfreqPBImage.flush();
        minfreqPBImage.unlock();
    }

    std::shared_ptr<PagedImage<float>> avgpbImage =
        openOrCreate(log_l, avgpbname, shape, coord, "Average PB");

    avgpbImage->put(reduceAcrossImages(pbnames, shape, avgpbmode == "median"));
    avgpbImage->flush();
    avgpbImage->unlock();

    log_l << (avgpbmode == "median" ? "Median" : "Mean") << " PB image computed and saved to disk."
          << LogIO::NORMAL << LogIO::POST;
}

// --------------------------------------------------------------------------
// PB application
// --------------------------------------------------------------------------

void Taylor::applyPB(const std::string &pbname, const std::vector<std::string> &cubenames,
                     const std::string &mode)
{
    LogIO log_l(LogOrigin("Taylor", "applyPB"));

    if (mode != "multiply" && mode != "divide")
    {
        log_l << "Invalid mode '" << mode << "'.  Must be 'multiply' or 'divide'."
              << LogIO::EXCEPTION;
    }

    const float pblimit = pblimit_;
    const bool multiply = (mode == "multiply");

    applyImageExpr(
        log_l, cubenames, {pbname}, "",
        [multiply, pblimit](ImageInterface<float> &cube, ImageInterface<float> &pb,
                            ImageInterface<float> *) -> LatticeExpr<float> {
            if (multiply)
            {
                return LatticeExpr<float>(cube * pb);
            }
            return LatticeExpr<float>(iif(pb > pblimit, cube / pb, 0.0f));
        },
        "applyPB (" + mode + ")");
}

void Taylor::removeFreqDepPB(const std::string &avgpbname,
                             const std::vector<std::string> &cubenames,
                             const std::vector<std::string> &pbnames)
{
    LogIO log_l(LogOrigin("Taylor", "removeFreqDepPB"));

    if (cubenames.size() != pbnames.size())
    {
        log_l << "cubenames and pbnames must have the same length." << LogIO::EXCEPTION;
    }

    const float pblimit = pblimit_;
    applyImageExpr(
        log_l, cubenames, pbnames, avgpbname,
        [pblimit](ImageInterface<float> &cube, ImageInterface<float> &pb,
                  ImageInterface<float> *avgpb) -> LatticeExpr<float> {
            return LatticeExpr<float>(iif(pb > pblimit, (*avgpb) * cube / pb, 0.0f));
        },
        "removeFreqDepPB");
}

void Taylor::applyFreqDepPB(const std::string &avgpbname,
                            const std::vector<std::string> &cubenames,
                            const std::vector<std::string> &pbnames)
{
    LogIO log_l(LogOrigin("Taylor", "applyFreqDepPB"));

    if (cubenames.size() != pbnames.size())
    {
        log_l << "cubenames and pbnames must have the same length." << LogIO::EXCEPTION;
    }

    const float pblimit = pblimit_;
    applyImageExpr(
        log_l, cubenames, pbnames, avgpbname,
        [pblimit](ImageInterface<float> &cube, ImageInterface<float> &pb,
                  ImageInterface<float> *avgpb) -> LatticeExpr<float> {
            // NOTE: CASA (SynthesisUtilMethods::applyFreqDepPB) guards on
            // pb_chan > pblimit, not avgPB > pblimit.  The difference only
            // affects edge pixels where pb_chan < pblimit < avgPB.
            return LatticeExpr<float>(iif((*avgpb) > pblimit, pb * cube / (*avgpb), 0.0f));
        },
        "applyFreqDepPB");
}
