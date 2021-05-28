//# Copyright (C) 1995,1997,1998,1999,2000,2001,2002,2003
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

#include <components/ComponentModels/GaussianDeconvolver.h>
#include <imageanalysis/ImageAnalysis/CasaImageBeamSet.h>

using namespace casacore;
namespace casa {

CasaImageBeamSet::CasaImageBeamSet() : ImageBeamSet() {}

CasaImageBeamSet::CasaImageBeamSet(const Matrix<GaussianBeam>& beams)
	: ImageBeamSet(beams) {}

CasaImageBeamSet::CasaImageBeamSet(const GaussianBeam& beam)
	: ImageBeamSet(beam) {}

CasaImageBeamSet::CasaImageBeamSet(
	uInt nchan, uInt nstokes, const GaussianBeam& beam
) : ImageBeamSet(nchan, nstokes, beam) {}

CasaImageBeamSet::CasaImageBeamSet(const CasaImageBeamSet& other)
	: ImageBeamSet(other) {}

CasaImageBeamSet::CasaImageBeamSet(const ImageBeamSet& other)
	: ImageBeamSet(other) {}

CasaImageBeamSet::~CasaImageBeamSet() {}

CasaImageBeamSet& CasaImageBeamSet::operator=(const CasaImageBeamSet& other) {
	ImageBeamSet::operator=(other);
	return *this;
}


const String& CasaImageBeamSet::className() {
	static const String c = "CasaImageBeamSet";
	return c;
}

GaussianBeam CasaImageBeamSet::getCommonBeam() const {
	if (empty()) {
		throw AipsError("This beam set is empty.");
	}
	const Matrix<GaussianBeam> beams = getBeams();
	if (allTrue(beams == GaussianBeam::NULL_BEAM)) {
		throw AipsError("All beams are null.");
	}
	if (allTrue(beams == beams(IPosition(2, 0)))) {
		return beams(IPosition(2, 0));
	}
	BeamIter end = beams.end();
	Bool largestBeamWorks = true;
	GaussianBeam junk;
	GaussianBeam problemBeam;
	GaussianBeam maxBeam = getMaxAreaBeam();
	//Double myMajor = maxBeam.getMajor("arcsec");
	// Double myMinor = maxBeam.getMinor("arcsec");

	for (
		BeamIter iBeam = beams.begin();
		iBeam != end; iBeam++
	) {
		if (*iBeam != maxBeam && !iBeam->isNull()) {
			//myMajor = max(myMajor, iBeam->getMajor("arcsec"));
			//myMinor = max(myMinor, iBeam->getMinor("arcsec"));
			try {
				if (GaussianDeconvolver::deconvolve(junk, maxBeam, *iBeam)) {
					largestBeamWorks = false;
					problemBeam = *iBeam;
				}
			}
			catch (const AipsError& x) {
				largestBeamWorks = false;
				problemBeam = *iBeam;
			}
		}
	}
	if (largestBeamWorks) {
		return maxBeam;
	}

	// transformation 1, rotate so one of the ellipses' major axis lies
	// along the x axis. Ellipse A is _maxBeam, ellipse B is problemBeam,
	// ellipse C is our wanted ellipse

	Double tB1 = problemBeam.getPA("rad", true) - maxBeam.getPA("rad", true);

	if (abs(tB1) == C::pi / 2) {
		Bool maxHasMajor = maxBeam.getMajor("arcsec")
				>= problemBeam.getMajor("arcsec");
		// handle the situation of right angles explicitly because things blow up otherwise
		Quantity major =
				maxHasMajor ? maxBeam.getMajor() : problemBeam.getMajor();
		Quantity minor =
				maxHasMajor ? problemBeam.getMajor() : maxBeam.getMajor();
		Quantity pa =
				maxHasMajor ? maxBeam.getPA(true) : problemBeam.getPA(true);
		return GaussianBeam(major, minor, pa);
	}

	Double aA1 = maxBeam.getMajor("arcsec");
	Double bA1 = maxBeam.getMinor("arcsec");
	Double aB1 = problemBeam.getMajor("arcsec");
	Double bB1 = problemBeam.getMinor("arcsec");

	// transformation 2: Squeeze along the x axis and stretch along y axis so
	// ellipse A becomes a circle, preserving its area
	Double aA2 = sqrt(aA1 * bA1);
	Double bA2 = aA2;
	Double p = aA2 / aA1;
	Double q = bA2 / bA1;

	// ellipse B's parameters after transformation 2
	Double aB2, bB2, tB2;

	_transformEllipseByScaling(aB2, bB2, tB2, aB1, bB1, tB1, p, q);

	// Now the enclosing transformed ellipse, C, has semi-major axis equal to aB2,
	// minor axis is aA2 == bA2, and the pa is tB2
	Double aC2 = aB2;
	Double bC2 = aA2;
	Double tC2 = tB2;

	// Now reverse the transformations, first transforming ellipse C by shrinking the coordinate
	// system of transformation 2 yaxis and expanding its xaxis to return to transformation 1.

	Double aC1, bC1, tC1;
	_transformEllipseByScaling(aC1, bC1, tC1, aC2, bC2, tC2, 1 / p, 1 / q);

	// now rotate by _maxBeam.getPA() to get the untransformed enclosing ellipse

	Double aC = aC1;
	Double bC = bC1;
	Double tC = tC1 + maxBeam.getPA("rad", true);

	// confirm that we can indeed convolve both beams with the enclosing ellipse
	GaussianBeam newMaxBeam = GaussianBeam(Quantity(aC, "arcsec"),
			Quantity(bC, "arcsec"), Quantity(tC, "rad"));
	// Sometimes (due to precision issues I suspect), the found beam has to be increased slightly
	// so our deconvolving method doesn't fail
	Bool ok = false;
	while (!ok) {
		try {
			if (GaussianDeconvolver::deconvolve(junk, newMaxBeam, maxBeam)) {
				throw AipsError();
			}
			if (GaussianDeconvolver::deconvolve(junk, newMaxBeam, problemBeam)) {
				throw AipsError();
			}
			ok = true;
		}
		catch (const AipsError& x) {
			// deconvolution issues, increase the enclosing beam size slightly
			aC *= 1.001;
			bC *= 1.001;
			newMaxBeam = GaussianBeam(Quantity(aC, "arcsec"),
					Quantity(bC, "arcsec"), Quantity(tC, "rad"));
		}
	}
	// create a new beam set to run this method on, replacing _maxBeam with ellipse C

	CasaImageBeamSet newBeamSet(*this);
	Array<GaussianBeam> newBeams = beams.copy();
	newBeams(getMaxAreaBeamPosition()) = newMaxBeam;
	newBeamSet.setBeams(newBeams);

	return newBeamSet.getCommonBeam();
}

void CasaImageBeamSet::_transformEllipseByScaling(
	Double& transformedMajor,
	Double& transformedMinor, Double& transformedPA, Double major,
	Double minor, Double pa, Double xScaleFactor, Double yScaleFactor
) {
	Double mycos = cos(pa);
	Double mysin = sin(pa);
	Double cos2 = mycos * mycos;
	Double sin2 = mysin * mysin;
	Double major2 = major * major;
	Double minor2 = minor * minor;
	Double a = cos2 / (major2) + sin2 / (minor2);
	Double b = -2 * mycos * mysin * (1 / (major2) - 1 / (minor2));
	Double c = sin2 / (major2) + cos2 / (minor2);

	Double xs = xScaleFactor * xScaleFactor;
	Double ys = yScaleFactor * yScaleFactor;

	Double r = a / xs;
	Double s = b * b / (4 * xs * ys);
	Double t = c / ys;

	Double u = r - t;
	Double u2 = u * u;

	Double f1 = u2 + 4 * s;
	Double f2 = sqrt(f1) * abs(u);

	Double j1 = (f2 + f1) / f1 / 2;
	Double j2 = (-f2 + f1) / f1 / 2;

	Double k1 = (j1 * r + j1 * t - t) / (2 * j1 - 1);
	Double k2 = (j2 * r + j2 * t - t) / (2 * j2 - 1);

	Double c1 = sqrt(1 / k1);
	Double c2 = sqrt(1 / k2);

	if (c1 == c2) {
		// the transformed ellipse is a circle
		transformedMajor = sqrt(k1);
		transformedMinor = transformedMajor;
		transformedPA = 0;
	} else if (c1 > c2) {
		// c1 is the major axis and so j1 is the solution for the corresponding pa
		// of the transformed ellipse
		transformedMajor = c1;
		transformedMinor = c2;
		transformedPA = (pa >= 0 ? 1 : -1) * acos(sqrt(j1));
	} else {
		// c2 is the major axis and so j2 is the solution for the corresponding pa
		// of the transformed ellipse
		transformedMajor = c2;
		transformedMinor = c1;
		transformedPA = (pa >= 0 ? 1 : -1) * acos(sqrt(j2));
	}
}

}
