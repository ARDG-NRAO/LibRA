//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002
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

#ifndef IMAGEANALYSIS_IMAGEPOLTASK_H
#define IMAGEANALYSIS_IMAGEPOLTASK_H

#include <imageanalysis/ImageAnalysis/ImageTask.h>

namespace casa {

// <summary>
// Begin moving polarization tasks to ImageTask framework.
// </summary>

// <use visibility=export>

// <prerequisite>
//   <li> <linkto class=casacore::ImageExpr>ImageExpr</linkto>
//   <li> <linkto class=casacore::ImageInterface>ImageInterface</linkto>
// </prerequisite>

// <etymology>
//  Polarimetric analysis of Images
// </etymology>

// <synopsis>
// This class provides polarimetric image analysis capability.
// It takes an image with a casacore::Stokes axis (some combination
// of IQUV is needed) as its input.
//
// Many functions return casacore::ImageExpr objects.  These are
// read-only images.
//
// Sometimes the standard deviation of the noise is needed.
// This is for debiasing polarized intensity images or working out
// error images.  By default it is worked out for you with a
// clipped mean algorithm.  However, you can provide sigma if you
// know it accurately.   It should be the standard deviation of the noise in
// the absence of signal.  You won't measure that very well from
// casacore::Stokes I if it is dynamic range limited.  Better to get it from 
// V or Q or U.  When this class needs the standard deviation of
// the noise, it will try and get it from V or Q and U and finally I.
//
// However, note that the functions sigmaStokes{I,Q,U,V} DO return the standard
// deviation of the noise for that specific casacore::Stokes type.
//
// The casacore::ImageExpr objects returned have the brightness units and ImageInfo
// set.  The MiscInfo (a permanent record) and logSink are not set.
// 
// </synopsis>
//
// <motivation>
// Basic image analysis capability
// </motivation>

// <todo asof="1999/11/01">
//   <li> plotting for function rotationMeasure 
//   <li> some assessment of the curvature of pa-l**2
// </todo>

class ImagePolTask : public ImageTask<casacore::Float> {
public:

    enum StokesTypes {I, Q, U, V};

    ImagePolTask() = delete;

    virtual ~ImagePolTask();

    virtual casacore::String getClass() const;

    /*
    // Get the linearly polarized intensity image and its
    // standard deviation.  If wish to debias the image, you
    // can either provide <src>sigma</src> (the standard
    // deviation of the termal noise ) or if <src>sigma</src> is non-positive,
    // it will  be worked out for you with a clipped mean algorithm.
    // <group>
    casacore::ImageExpr<casacore::Float> linPolInt(
        casacore::Bool debias, casacore::Float clip=10.0, casacore::Float sigma=-1.0
    );
    */

    casacore::Float sigmaLinPolInt(
        casacore::Float clip=10.0, casacore::Float sigma=-1.0
    );
    // </group>

protected:

    ImagePolTask(
        const SPCIIF image, const casacore::String& outname,
        casacore::Bool overwrite
    );

    casacore::Bool _checkQUBeams(
        casacore::Bool requireChannelEquality, casacore::Bool throws=casacore::True
    ) const;

    // Change the casacore::Stokes casacore::Coordinate for the given
    // complex image to be of the specified casacore::Stokes type
    void _fiddleStokesCoordinate(
        casacore::ImageInterface<casacore::Float>& ie,
        casacore::Stokes::StokesTypes type
    ) const;

    std::vector<casacore::Coordinate::Type> _getNecessaryCoordinates() const {
        static const std::vector<casacore::Coordinate::Type> v { casacore::Coordinate::STOKES };
        return v;
    }

    CasacRegionManager::StokesControl _getStokesControl() const {
        return CasacRegionManager::USE_ALL_STOKES;
    }

    SPCIIF _getStokesImage(StokesTypes type) const;

    // Make a LEN for the give types of polarized intensity
    casacore::LatticeExprNode _makePolIntNode(
        casacore::Bool debias, casacore::Float clip, casacore::Float sigma,
        casacore::Bool doLin, casacore::Bool doCirc
    );

    void _maskAndZeroNaNs(SPIIF out);

    void _setDoLinDoCirc(
        casacore::Bool& doLin, casacore::Bool& doCirc,
        casacore::Bool requireI
    ) const;

    void _setInfo(
        casacore::ImageInterface<Float>& im, const StokesTypes stokes
    ) const;

private:

    // These blocks are always size 4, with IQUV in slots 0,1,2,3
    // If your image is IV only, they still use slots 0 and 3
    std::vector<SPIIF> _stokesImage = std::vector<SPIIF>(4);
    std::vector<std::shared_ptr<casacore::LatticeStatistics<casacore::Float>>> _stokesStats
        = std::vector<std::shared_ptr<casacore::LatticeStatistics<casacore::Float>>>(4);
    casacore::Matrix<casacore::Bool> _beamsEqMat
        = casacore::Matrix<casacore::Bool>(4, 4, casacore::False);
    Float _oldClip = 0.0;

    static const casacore::String CLASS_NAME;

    casacore::Bool _checkBeams(
        const std::vector<StokesTypes>& stokes,
        casacore::Bool requireChannelEquality,
        casacore::Bool throws=true
    ) const;

    casacore::Bool _checkIQUBeams(
        casacore::Bool requireChannelEquality, casacore::Bool throws=casacore::True
    ) const;

    casacore::Bool _checkIVBeams(
        casacore::Bool requireChannelEquality, casacore::Bool throws=casacore::True
    ) const;

    void _createBeamsEqMat();

    // Find the casacore::Stokes in the construction image and assign pointers
    void _findStokes();

    void _fiddleStokesCoordinate(
        casacore::CoordinateSystem& cSys, casacore::Stokes::StokesTypes type
    ) const;


    // Make a casacore::SubImage from the construction image for the specified pixel
    // along the specified pixel axis
    SPIIF _makeSubImage(
        casacore::IPosition& blc, casacore::IPosition& trc,
        casacore::Int axis, casacore::Int pix
    ) const;

    // Get the best estimate of the statistical noise. This gives you
    // the standard deviation with outliers from the mean
    // clipped first. The idea is to not be confused by source or dynamic range issues.
    // Generally casacore::Stokes V is empty of sources (not always), then Q and U are generally
    // less bright than I.  So this function first tries V, then Q and U
    // and lastly I to give you its noise estimate
    casacore::Float _sigma (casacore::Float clip=10.0);

    // Find the standard deviation for the casacore::Stokes image specified by the integer index
    casacore::Float _sigma(StokesTypes index, casacore::Float clip);

    // Return I, Q, U or V for specified integer index (0-3)
    casacore::String _stokesName (StokesTypes index) const;

};

}

#endif
