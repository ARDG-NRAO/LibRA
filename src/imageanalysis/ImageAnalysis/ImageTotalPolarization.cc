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

#include <imageanalysis/ImageAnalysis/ImageTotalPolarization.h>

using namespace casacore;
namespace casa {

const String ImageTotalPolarization::CLASS_NAME = "ImageTotalPolarization";

ImageTotalPolarization::ImageTotalPolarization(
    const SPCIIF image, const String& outname, Bool overwrite
) : ImagePolTask(image, outname, overwrite) {}

ImageTotalPolarization::~ImageTotalPolarization() {}

SPIIF ImageTotalPolarization::compute() {
    *_getLog() << LogOrigin(CLASS_NAME, __func__);
    ImageExpr<Float> expr = _totPolInt();
    auto out = _prepareOutputImage(expr);
    if (_debias) {
        _maskAndZeroNaNs(out);
    }
    return out;
}

String ImageTotalPolarization::getClass() const {
    return CLASS_NAME;
}

void ImageTotalPolarization::setClip(Float clip) {
    _clip = clip;
}

void ImageTotalPolarization::setDebias(Bool debias) {
    _debias = debias;
}

void ImageTotalPolarization::setSigma(Float sigma) {
    _sigma = sigma;
}

ImageExpr<Float> ImageTotalPolarization::_totPolInt() {
    *_getLog() << LogOrigin(CLASS_NAME, __func__, WHERE);
    Bool doLin, doCirc;
    _setDoLinDoCirc(doLin, doCirc, False);
    // Make node.
    LatticeExprNode node = _makePolIntNode(_debias, _clip, _sigma, doLin, doCirc);
    // Make expression
    LatticeExpr<Float> le(node);
    ImageExpr<Float> ie(le, "totalPolarizedIntensity");
    ie.setUnits(_getImage()->units());     // Dodgy. The beam is now rectified
    StokesTypes stokes = _getStokesImage(Q) ? Q : _getStokesImage(U) ? U : V;
    _setInfo(ie, stokes);
    // Fiddle Stokes coordinate in ImageExpr
    _fiddleStokesCoordinate(ie, doCirc ? Stokes::Ptotal : Stokes::Plinear);
    return ie;
}

}
