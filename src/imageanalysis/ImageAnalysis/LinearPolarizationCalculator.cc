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

#include <imageanalysis/ImageAnalysis/LinearPolarizationCalculator.h>

using namespace casacore;
namespace casa {

const String LinearPolarizationCalculator::CLASS_NAME = "LinearPolarizationCalculator";

LinearPolarizationCalculator::LinearPolarizationCalculator(
    const SPCIIF image, const String& outname, Bool overwrite
) : ImagePolTask(image, outname, overwrite) {}

LinearPolarizationCalculator::~LinearPolarizationCalculator() {}

SPIIF LinearPolarizationCalculator::compute() {
    *_getLog() << LogOrigin(CLASS_NAME, __func__, WHERE);
    auto q = _getStokesImage(Q);
    auto u = _getStokesImage(U);
    ThrowIf(
        ! q || ! u,
        "This image does not have Stokes Q and/or U so cannot compute linear polarization"
    );
    _checkQUBeams(false);
    auto node = _makePolIntNode(_debias, _clip, _sigma, True, False);
    // Make expression
    LatticeExpr<Float> le(node);
    ImageExpr<Float> ie(le, String("LinearlyPolarizedIntensity"));
    ie.setUnits(_getImage()->units());
    _setInfo(ie, Q);
    // Fiddle Stokes coordinate in ImageExpr
    _fiddleStokesCoordinate(ie, Stokes::Plinear);
    auto out = _prepareOutputImage(ie);
    if (_debias) {
        _maskAndZeroNaNs(out);
    }
    return out;
}

String LinearPolarizationCalculator::getClass() const {
    return CLASS_NAME;
}


void LinearPolarizationCalculator::setClip(Float clip) {
    _clip = clip;
}

void LinearPolarizationCalculator::setDebias(Bool debias) {
    _debias = debias;
}

void LinearPolarizationCalculator::setSigma(Float sigma) {
    _sigma = sigma;
}

}
