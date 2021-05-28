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


#ifndef IMAGEANALYSIS_IMAGEFITTERRESULTS_TCC
#define IMAGEANALYSIS_IMAGEFITTERRESULTS_TCC

#include <imageanalysis/IO/ImageFitterResults.h>

#include <casa/BasicSL/STLIO.h>
#include <casa/OS/File.h>
#include <casa/Quanta/QLogical.h>
#include <casa/Utilities/Precision.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <images/Images/ImageInterface.h>

#include <components/ComponentModels/GaussianShape.h>
#include <imageanalysis/IO/LogFile.h>

#include <iomanip>

using namespace casacore;

namespace casa {

template <class T> const String ImageFitterResults<T>::_class = "ImageFitterResults";

template <class T> vector<String> ImageFitterResults<T>::_prefixesWithCenti = vector<String>();

template <class T> vector<String> ImageFitterResults<T>::_prefixes = vector<String>();

template <class T> ImageFitterResults<T>::ImageFitterResults(
	SPCIIT image, std::shared_ptr<LogIO> log
) : _image(image), _log(log), _bUnit(image->units().getName()) {}

template <class T> ImageFitterResults<T>::~ImageFitterResults() {}

template <class T> void ImageFitterResults<T>::writeNewEstimatesFile(const String& filename) const {
	ostringstream out;
	uInt ndim = _image->ndim();
	const CoordinateSystem csys = _image->coordinates();
	Vector<Int> dirAxesNumbers = csys.directionAxesNumbers();
	Vector<Double> world(ndim, 0), pixel(ndim, 0);
	csys.toWorld(world, pixel);
	*_log << LogOrigin(_class, __func__);
	uInt n = _convolvedList.nelements();
	for (uInt i=0; i<n; ++i) {
		MDirection mdir = _convolvedList.getRefDirection(i);
		Quantity lat = mdir.getValue().getLat("rad");
		Quantity longitude = mdir.getValue().getLong("rad");
		world[dirAxesNumbers[0]] = longitude.getValue();
		world[dirAxesNumbers[1]] = lat.getValue();
		if (csys.toPixel(pixel, world)) {
			out << _peakIntensities[i].getValue() << ", "
			    << pixel[0] << ", " << pixel[1] << ", "
			    << _majorAxes[i] << ", " << _minorAxes[i] << ", "
			    << _positionAngles[i] << endl;
		}
		else {
			*_log << LogIO::WARN << "Unable to calculate pixel location of "
				<< "component number " << i << " so cannot write new estimates"
				<< "file" << LogIO::POST;
			return;
		}
	}
	String output = out.str();
	File estimates(filename);
	String action = (estimates.getWriteStatus() == File::OVERWRITABLE) ? "Overwrote" : "Created";
	LogFile newEstimates(filename);
	newEstimates.write(output, true, true);
	*_log << LogIO::NORMAL << action << " file "
		<< filename << " with new estimates file"
		<< LogIO::POST;
}

template <class T> void ImageFitterResults<T>::writeCompList(
	ComponentList& list, const String& compListName,
	CompListWriteControl writeControl
) const {
	if (compListName.empty()) {
		return;
	}
	switch (writeControl) {
	case NO_WRITE:
		return;
	case WRITE_NO_REPLACE:
	{
		File file(compListName);
		if (file.exists()) {
			LogOrigin logOrigin(_class, __func__);
			*_log << logOrigin;
			*_log << LogIO::WARN << "Requested persistent component list " << compListName
				<< " already exists and user does not wish to overwrite it so "
				<< "the component list will not be written" << LogIO::POST;
			return;
		}
	}
	// allow fall through
	case OVERWRITE: {
		Path path(compListName);
		list.rename(path, Table::New);
		*_log << LogIO::NORMAL << "Wrote component list table "
			<< compListName << LogIO::POST;
	}
	return;
	default:
		// should never get here
		return;
	}
}

template <class T> String ImageFitterResults<T>::resultsHeader(
	const String& chans, const Vector<uInt>& chanVec,
	const String& region, const String& mask,
	std::shared_ptr<std::pair<T, T>> includePixelRange,
	std::shared_ptr<std::pair<T, T>> excludePixelRange,
	const String& estimates
) const {
	ostringstream summary;
	ostringstream chansoss;
	if (! chans.empty()) {
		chansoss << chans;
	}
	else if (chanVec.size() == 2) {
		if (chanVec[0] == chanVec[1]) {
			chansoss << chanVec[0];
		}
		else {
			chansoss << chanVec[0] << "-" << chanVec[1];
		}
	}
	summary << "****** Fit performed at " << Time().toString() << "******" << endl << endl;
	summary << "Input parameters ---" << endl;
	summary << "       --- imagename:           " << _image->name() << endl;
	summary << "       --- region:              " << region << endl;
	summary << "       --- channel:             " << chansoss.str() << endl;
	summary << "       --- stokes:              " << _stokes << endl;
	summary << "       --- mask:                " << mask << endl;
	summary << "       --- include pixel range: [";
	if (includePixelRange) {
		ostringstream os;
		// os << *includePixelRange;
		casacore::operator<<(os, *includePixelRange);
		summary << os.str();
	}
	summary << "]" << endl;
	summary << "       --- exclude pixel range: [";
	if (excludePixelRange) {
			ostringstream os;
			// os << *excludePixelRange;
			casacore::operator<<(os, *excludePixelRange);
			summary << os.str();
		}
		summary << "]" << endl;
	if (! estimates.empty()) {
		summary << "       --- initial estimates:   Peak, X, Y, a, b, PA" << endl;
		summary << "                                " << estimates << endl;
	}
	return summary.str();
}

template <class T> String ImageFitterResults<T>::fluxToString(
	uInt compNumber, Bool hasBeam
) const {
	auto unitPrefix = unitPrefixes(false);
	ostringstream fluxes;
	Quantity fluxDensity = _fluxDensities[compNumber];
	Quantity fluxDensityError = _fluxDensityErrors[compNumber];
	Vector<String> polarization = _convolvedList.getStokes(compNumber);
	Quantity intensityToFluxConversion = _bUnit.contains("/beam")
	    ? Quantity(1.0, "beam")
	    : Quantity(1.0, "pixel");
	String baseUnit = "Jy";
	Bool hasTemperatureUnits = fluxDensity.isConform("K*rad2");
	if (hasTemperatureUnits) {
		String areaUnit = "rad*rad";
		baseUnit = "K." + areaUnit;
		intensityToFluxConversion.setUnit(areaUnit);
	}
	String usedPrefix;
	String unit;
	uInt n = unitPrefix.size();
	for (uInt i=0; i<n; ++i) {
		usedPrefix = unitPrefix[i];
		unit = usedPrefix + baseUnit;
		if (fluxDensity.getValue(unit) > 1) {
			fluxDensity.convert(unit);
			fluxDensityError.convert(unit);
			break;
		}
	}
	Vector<Double> fd(2);
	fd[0] = fluxDensity.getValue();
	fd[1] = fluxDensityError.getValue();
	Quantity peakIntensity = _peakIntensities[compNumber];
	Quantity tmpFlux = peakIntensity * intensityToFluxConversion;
	tmpFlux.convert(baseUnit);

	Quantity peakIntensityError = _peakIntensityErrors[compNumber];
	Quantity tmpFluxError = peakIntensityError * intensityToFluxConversion;
	uInt precision = 0;
	fluxes << "Flux ---" << endl;

	if (hasBeam) {
		precision = precisionForValueErrorPairs(fd, Vector<Double>());
		fluxes << std::fixed << std::setprecision(precision);
		fluxes << "       --- Integrated:   " << fluxDensity.getValue();
		if (
			_fixed[compNumber].contains("f") && _fixed[compNumber].contains("a")
			&& _fixed[compNumber].contains("b")
		) {
			fluxes << " " << fluxDensity.getUnit() << " (fixed)" << endl;
		}
		else {
			fluxes << " +/- " << fluxDensityError << endl;
		}
	}
	for (uInt i=0; i<n; ++i) {
		usedPrefix = unitPrefix[i];
		String unit = usedPrefix + tmpFlux.getUnit();
		if (tmpFlux.getValue(unit) > 1) {
			tmpFlux.convert(unit);
			tmpFluxError.convert(unit);
			break;
		}
	}
	peakIntensity = Quantity(
		tmpFlux.getValue(),
		tmpFlux.getUnit() + "/" + intensityToFluxConversion.getUnit()
	);
	peakIntensityError = Quantity(tmpFluxError.getValue(), peakIntensity.getUnit());
	if (hasTemperatureUnits) {
		peakIntensity.setUnit(usedPrefix + "K");
		peakIntensityError.setUnit(usedPrefix + "K");
	}
	Vector<Double> pi(2);
	pi[0] = peakIntensity.getValue();
	pi[1] = peakIntensityError.getValue();
	precision = precisionForValueErrorPairs(pi, Vector<Double>());
	fluxes << std::fixed << std::setprecision(precision);
	fluxes << "       --- Peak:         " << peakIntensity.getValue();
	if (_fixed[compNumber].contains("f")) {
		fluxes << " " << peakIntensity.getUnit() << " (fixed)" << endl;
	}
	else {
		fluxes << " +/- " << peakIntensityError << endl;
	}
	fluxes << "       --- Polarization: " << _stokes << endl;
	return fluxes.str();
}

template <class T> std::vector<String> ImageFitterResults<T>::unitPrefixes(Bool includeCenti) {
	if (_prefixes.empty()) {
#if __cplusplus >= 201103L
		_prefixesWithCenti = std::vector<String> {"T","G","M","k","","c","m","u","n"};
		_prefixes = std::vector<String> {"T","G","M","k","","m","u","n"};
#else
		_prefixesWithCenti = list_of("T")("G")("M")("k")("")("c")("m")("u")("n");
		_prefixes = list_of("T")("G")("M")("k")("")("m")("u")("n");
#endif
	}
	if (includeCenti) {
		return _prefixesWithCenti;
	}
	else {
		return _prefixes;
	}
}

template <class T> void ImageFitterResults<T>::writeSummaryFile(
    const String& filename, const CoordinateSystem& csys
) const {
    ostringstream oss;
    auto fluxUnit = _fluxDensities[0].getUnit();
    uInt planeWidth = 6;
    uInt fluxWidth = max(12, fluxUnit.size());
    auto peakUnit = _peakIntensities[0].getUnit();
    auto peakWidth = max(12, peakUnit.size());
    String ref = _convolvedList.getRefDirection(0).getRefString();
    String longLabel, latLabel;
    if (ref == "J2000" || ref == "B1950") {
        longLabel = "RA" + ref;
        latLabel = "Dec" + ref;
    }
    else {
        longLabel = "Long" + ref;
        latLabel = "Lat" + ref;
    }
    auto longErrLabel = longLabel + "err";
    auto latErrLabel = latLabel + "err";

    uInt longWidth = max(16, longLabel.size());
    uInt latWidth = max(16, latLabel.size());
    uInt longErrWidth = max(16, longErrLabel.size());
    uInt latErrWidth = max(16, latErrLabel.size());
    uInt sizeWidth = 12;
    uInt freqWidth = 15;
    auto doDecon = _deconvolvedList.nelements() > 0;
    auto doFreq = csys.hasSpectralAxis();

    oss << "# " << std::setw(planeWidth) << std::right << " " << " "
        << std::setw(fluxWidth) << std::right << fluxUnit << " "
        << std::setw(fluxWidth) << std::right << fluxUnit << " "
        << std::setw(peakWidth) << std::right << peakUnit << " "
        << std::setw(peakWidth) << std::right << peakUnit << " "
        << std::setw(longWidth) << std::right << "deg" << " "
        << std::setw(latWidth) << std::right << "deg" << " "
        << std::setw(longErrWidth) << std::right << "arcsec" << " "
        << std::setw(latErrWidth) << std::right << "arcsec" << " "
        << std::setw(sizeWidth) << std::right << "arcsec" << " "
        << std::setw(sizeWidth) << std::right << "arcsec" << " "
        << std::setw(sizeWidth) << std::right << "deg" << " "
        << std::setw(sizeWidth) << std::right << "arcsec" << " "
        << std::setw(sizeWidth) << std::right << "arcsec" << " "
        << std::setw(sizeWidth) << std::right << "deg" << " ";
    if (doDecon) {
        oss << std::setw(sizeWidth) << std::right << "arcsec" << " "
            << std::setw(sizeWidth) << std::right << "arcsec" << " "
            << std::setw(sizeWidth) << std::right << "deg" << " "
            << std::setw(sizeWidth) << std::right << "arcsec" << " "
            << std::setw(sizeWidth) << std::right << "arcsec" << " "
            << std::setw(sizeWidth) << std::right << "deg" << " ";
    }
    if (doFreq) {
        oss << std::setw(freqWidth) << std::right << "GHz" << " ";
    }
    oss << endl;

    oss << "# " << std::setw(planeWidth) << std::right << "Plane" << " "
        << std::right << std::setw(fluxWidth) << _stokes << " "
        << std::right << std::setw(fluxWidth) << (_stokes + "err") << " "
        << std::right << std::setw(peakWidth) << "Peak" << " "
        << std::right << std::setw(peakWidth) << "PeakErr" << " "
        << std::right << std::setw(longWidth) << longLabel << " "
        << std::right << std::setw(latWidth) << latLabel << " "
        << std::right << std::setw(longErrWidth) << longErrLabel << " "
        << std::right << std::setw(latErrWidth) << latErrLabel << " "
        << std::setw(sizeWidth) << std::right << "ConMaj" << " "
        << std::setw(sizeWidth) << std::right << "ConMin" << " "
        << std::setw(sizeWidth) << std::right << "ConPA" << " "
        << std::setw(sizeWidth) << std::right << "ConMajErr" << " "
        << std::setw(sizeWidth) << std::right << "ConMinErr" << " "
        << std::setw(sizeWidth) << std::right << "ConPAErr" << " ";
    if (doDecon) {
        oss << std::setw(sizeWidth) << std::right << "DeconMaj" << " "
            << std::setw(sizeWidth) << std::right << "DeconMin" << " "
            << std::setw(sizeWidth) << std::right << "DeconPA" << " "
            << std::setw(sizeWidth) << std::right << "DeconMajErr" << " "
            << std::setw(sizeWidth) << std::right << "DeconMinErr" << " "
            << std::setw(sizeWidth) << std::right << "DeconPAErr" << " ";
    }
    if (doFreq) {
        oss << std::setw(freqWidth) << std::right << "Freq" << " ";
    }
    oss << endl;

    auto n = _convolvedList.nelements();
    for (uInt i=0; i<n; ++i) {
        const auto* comp = _convolvedList.getShape(i);
        if (comp->type() != ComponentType::GAUSSIAN) {
            continue;
        }
        auto angle = comp->refDirection().getAngle().getValue("deg");
        auto longErr = comp->refDirectionErrorLong().getValue("arcsec");
        auto latErr = comp->refDirectionErrorLat().getValue("arcsec");
        auto gauss = dynamic_cast<const GaussianShape*>(comp);
        auto conMajor = gauss->majorAxis().getValue("arcsec");
        auto conMinor = gauss->minorAxis().getValue("arcsec");
        auto conPA = gauss->positionAngle().getValue("deg");
        auto conMajErr = gauss->majorAxisError().getValue("arcsec");
        auto conMinErr = gauss->minorAxisError().getValue("arcsec");
        auto conPAErr = gauss->positionAngleError().getValue("deg");
        oss << "  " << std::setw(planeWidth) << std::right << std::noshowpos << _channels[i] << " "
            << std::setw(fluxWidth) << std::right << std::scientific
                << std::setprecision(fluxWidth-7) << std::showpos << _fluxDensities[i].getValue(fluxUnit) << " "
            << std::setw(fluxWidth) << std::right << std::scientific
                << std::setprecision(fluxWidth-6) << std::noshowpos << _fluxDensityErrors[i].getValue(fluxUnit) << " "
            << std::setw(peakWidth) << std::right << std::scientific
                << std::setprecision(peakWidth-7) << std::showpos << _peakIntensities[i].getValue() << " "
            << std::setw(peakWidth) << std::right << std::scientific
                << std::setprecision(fluxWidth-6) << std::noshowpos << _peakIntensityErrors[i].getValue() << " "
            << std::setw(longWidth) << std::right << std::scientific
                << std::setprecision(longWidth-7) << std::showpos << angle[0] << " "
            << std::setw(latWidth) << std::right << std::scientific
                << std::setprecision(latWidth-7) << std::showpos << angle[1] << " "
            << std::setw(longErrWidth) << std::right << std::scientific
                << std::setprecision(longErrWidth-6) << std::noshowpos << longErr << " "
            << std::setw(latErrWidth) << std::right << std::scientific
                << std::setprecision(latWidth-6) << std::noshowpos << latErr << " "
            << std::setw(sizeWidth) << std::right << std::scientific
                << std::setprecision(sizeWidth-6) << std::noshowpos << conMajor << " "
            << std::setw(sizeWidth) << std::right << std::scientific
                << std::setprecision(sizeWidth-6) << std::noshowpos << conMinor << " "
            << std::setw(sizeWidth) << std::right << std::scientific
                << std::setprecision(sizeWidth-7) << std::showpos << conPA << " "
            << std::setw(sizeWidth) << std::right << std::scientific
                << std::setprecision(sizeWidth-6) << std::noshowpos << conMajErr << " "
            << std::setw(sizeWidth) << std::right << std::scientific
                << std::setprecision(sizeWidth-6) << std::noshowpos << conMinErr << " "
            << std::setw(sizeWidth) << std::right << std::scientific
                << std::setprecision(sizeWidth-6) << std::noshowpos << conPAErr << " ";
        if (doDecon) {
            const auto* decGauss = dynamic_cast<const GaussianShape*>(
                _deconvolvedList.getShape(i)
            );
            Double deconMajor = 0;
            Double deconMinor = 0;
            Double deconPA = 0;
            Double deconMajErr = 0;
            Double deconMinErr = 0;
            Double deconPAErr = 0;
            if (decGauss) {
                deconMajor = decGauss->majorAxis().getValue("arcsec");
                deconMinor = decGauss->minorAxis().getValue("arcsec");
                deconPA = decGauss->positionAngle().getValue("deg");
                deconMajErr = decGauss->majorAxisError().getValue("arcsec");
                deconMinErr = decGauss->minorAxisError().getValue("arcsec");
                deconPAErr = decGauss->positionAngleError().getValue("deg");
            }
            oss << std::setw(sizeWidth) << std::right << std::scientific
                << std::setprecision(sizeWidth-6) << std::noshowpos << deconMajor << " "
            << std::setw(sizeWidth) << std::right << std::scientific
                << std::setprecision(sizeWidth-6) << std::noshowpos << deconMinor << " "
            << std::setw(sizeWidth) << std::right << std::scientific
                << std::setprecision(sizeWidth-7) << std::showpos << deconPA << " "
            << std::setw(sizeWidth) << std::right << std::scientific
                << std::setprecision(sizeWidth-6) << std::noshowpos << deconMajErr << " "
            << std::setw(sizeWidth) << std::right << std::scientific
                << std::setprecision(sizeWidth-6) << std::noshowpos << deconMinErr << " "
            << std::setw(sizeWidth) << std::right << std::scientific
                << std::setprecision(sizeWidth-6) << std::noshowpos << deconPAErr << " ";
        }
        if (doFreq) {
            Double freq;
            csys.spectralCoordinate().toWorld(freq, _channels[i]);
            oss << std::setw(freqWidth) << std::right << std::scientific
                << std::setprecision(freqWidth-6) << std::noshowpos << freq << " ";
        }
        oss << endl;
    }
    LogFile summary(filename);
    summary.write(oss.str(), true, true);
}

}

#endif
