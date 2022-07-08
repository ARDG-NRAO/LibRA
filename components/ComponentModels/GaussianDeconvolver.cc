//# ImageMetaData.cc: Meta information for Images
//# Copyright (C) 2009
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
//# $Id: ImageMetaData.cc 20886 2010-04-29 14:06:56Z gervandiepen $

#include <components/ComponentModels/GaussianDeconvolver.h>
#include <casacore/casa/BasicMath/Math.h>
#include <casacore/scimath/Mathematics/GaussianBeam.h>

using namespace casacore;
namespace casa {

Bool GaussianDeconvolver::deconvolve(
	Angular2DGaussian& deconvolvedSize,
	const Angular2DGaussian& convolvedSize,
	const GaussianBeam& beam
) {
	Unit radians(String("rad"));
	Unit positionAngleModelUnit = deconvolvedSize.getPA(false).getFullUnit();
	Unit majorAxisModelUnit = deconvolvedSize.getMajor().getFullUnit();
	Unit minorAxisModelUnit = deconvolvedSize.getMinor().getFullUnit();

	// Get values in radians
	Double majorSource = convolvedSize.getMajor().getValue(radians);
	Double minorSource = convolvedSize.getMinor().getValue(radians);
	Double thetaSource = convolvedSize.getPA(true).getValue(radians);
	Double majorBeam = beam.getMajor().getValue(radians);
	Double minorBeam = beam.getMinor().getValue(radians);
	Double thetaBeam = beam.getPA(true).getValue(radians);
	// Do the sums

	Double alpha  = square(majorSource*cos(thetaSource)) +
		square(minorSource*sin(thetaSource)) -
		square(majorBeam*cos(thetaBeam)) -
		square(minorBeam*sin(thetaBeam));
	Double beta   = square(majorSource*sin(thetaSource)) +
		square(minorSource*cos(thetaSource)) -
		square(majorBeam*sin(thetaBeam)) -
		square(minorBeam*cos(thetaBeam));
	Double gamma  = 2 * ( (square(minorSource)-square(majorSource))*sin(thetaSource)*cos(thetaSource) -
		(square(minorBeam)-square(majorBeam))*sin(thetaBeam)*cos(thetaBeam) );
	// Set result in radians

	Double s = alpha + beta;
	Double t = sqrt(square(alpha-beta) + square(gamma));
	Double limit = min(majorSource,minorSource);
	limit = min(limit,majorBeam);
	limit = min(limit,minorBeam);
	limit = 0.1*limit*limit;

	if(alpha<0.0 || beta<0.0 || s<t) {
		if(0.5*(s-t)<limit && alpha>-limit && beta>-limit) {
			// Point source. Fill in values of beam
			deconvolvedSize = GaussianBeam(
				Quantity(beam.getMajor().get(majorAxisModelUnit)),
				Quantity(beam.getMinor().get(minorAxisModelUnit)),
				Quantity(beam.getPA(true).get(positionAngleModelUnit))
			);
			// unwrap
			deconvolvedSize.setPA(deconvolvedSize.getPA(true));
			return true;
		}
		else {
			throw AipsError("Source may be only (slightly) resolved in one direction");
		}
	}
	Quantity majax(sqrt(0.5*(s+t)), radians);
	majax.convert(majorAxisModelUnit);
	Quantity minax(sqrt(0.5*(s-t)), radians);
	minax.convert(minorAxisModelUnit);
	Quantity pa(
		abs(gamma)+abs(alpha-beta) == 0.0
			? 0.0
			: 0.5*atan2(-gamma,alpha-beta),
		radians);
	pa.convert(positionAngleModelUnit);
	deconvolvedSize = GaussianBeam(majax, minax, pa);
	// unwrap
	deconvolvedSize.setPA(deconvolvedSize.getPA(true));
	return false;
}



} //# NAMESPACE CASA - END

