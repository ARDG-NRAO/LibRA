//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
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

#include <imageanalysis/ImageAnalysis/LinearPolarizationAngleCalculator.h>

using namespace casacore;
namespace casa {

const String LinearPolarizationAngleCalculator::CLASS_NAME = "LinearPolarizationAngleCalculator";

LinearPolarizationAngleCalculator::LinearPolarizationAngleCalculator(
    const SPCIIF image, const String& outname, Bool overwrite
) : ImagePolTask(image, outname, overwrite) {}

LinearPolarizationAngleCalculator::~LinearPolarizationAngleCalculator() {}

SPIIF LinearPolarizationAngleCalculator::compute(Bool radians) {
    *_getLog() << LogOrigin(CLASS_NAME, __func__, WHERE);
    auto q = _getStokesImage(Q);
    auto u = _getStokesImage(U);
    ThrowIf (
        ! q || ! u,
        "This image does not have Stokes Q and/or U so cannot "
        "compute linear polarization angle"
    );
    _checkQUBeams(False);
    // Make expression. LEL function "pa" returns degrees
    Float fac = radians ? C::pi / 180.0 : 1.0;
    LatticeExprNode node(fac*pa(*u, *q));
    LatticeExpr<Float> le(node);
    ImageExpr<Float> ie(le, String("LinearlyPolarizedPositionAngle"));
    ie.setUnits(Unit(radians ? "rad" : "deg"));
    ImageInfo ii = _getImage()->imageInfo();
    if (ii.hasMultipleBeams()) {
        *this->_getLog() << LogIO::WARN << "The input image has "
            << "multiple beams. Because these beams can vary with "
            << "stokes/polarization, they will not be copied to the "
            << "output image" << LogIO::POST;
        ii.removeRestoringBeam();
    }
    ie.setImageInfo(ii);
    _fiddleStokesCoordinate(ie, Stokes::Pangle);
    return _prepareOutputImage(ie);
}

String LinearPolarizationAngleCalculator::getClass() const {
    return CLASS_NAME;
}

}
