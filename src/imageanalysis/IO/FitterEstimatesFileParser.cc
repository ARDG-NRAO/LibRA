//# Copyright (C) 1996,1997,1998,1999,2001,2002
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
//# $Id:  $

#include <imageanalysis/IO/FitterEstimatesFileParser.h>

#include <casacore/casa/aips.h>
#include <casacore/casa/IO/RegularFileIO.h>
#include <casacore/casa/Utilities/Regex.h>
#include <casacore/casa/Containers/Record.h>
#include <components/ComponentModels/ConstantSpectrum.h>
#include <components/ComponentModels/Flux.h>
#include <components/ComponentModels/GaussianShape.h>
#include <casacore/coordinates/Coordinates/CoordinateUtil.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/images/Images/ImageStatistics.h>


using namespace casacore;
namespace casa {

FitterEstimatesFileParser::~FitterEstimatesFileParser() {}

ComponentList FitterEstimatesFileParser::getEstimates() const {
	return _componentList;
}

Vector<String> FitterEstimatesFileParser::getFixed() const {
	return _fixedValues;
}

String FitterEstimatesFileParser::getContents() const {
	return _contents;
}

void FitterEstimatesFileParser::_parseFile(
	const RegularFile& myFile
) {
	_log->origin(LogOrigin("FitterEstimatesFileParser",__func__));

	RegularFileIO fileIO(myFile);
	// I doubt a genuine estimates file will ever have this many characters
	Int bufSize = 4096;
	char *buffer = new char[bufSize];
	int nRead;

	while ((nRead = fileIO.read(bufSize, buffer, false)) == bufSize) {
		*_log << LogIO::NORMAL << "read: " << nRead << LogIO::POST;
		String chunk(buffer, bufSize);

		_contents += chunk;
	}
	// get the last chunk
	String chunk(buffer, nRead);
	_contents += chunk;

	Vector<String> lines = stringToVector(_contents, '\n');
	Regex blankLine("^[ \n\t\r\v\f]+$",1000);
	uInt componentIndex = 0;
	Vector<String>::iterator end = lines.end();
	String filename = myFile.path().dirName() + "/" + myFile.path().baseName();
	for(Vector<String>::iterator iter=lines.begin(); iter!=end; iter++) {
		if (iter->empty() || iter->firstchar() == '#' ||  iter->matches(blankLine)) {
			// ignore comments and blank lines
			continue;
		}
		uInt commaCount = iter->freq(',');
		ThrowIf(
			commaCount < 5 || commaCount > 6,
			"bad format for line " + *iter
		);
		Vector<String> parts = stringToVector(*iter);
		for (Vector<String>::iterator viter = parts.begin(); viter != parts.end(); viter++) {
			viter->trim();
		}
		String peak = parts[0];
		String xpos = parts[1];
		String ypos = parts[2];
		String maj = parts[3];
		String min = parts[4];
		String pa = parts[5];

		String fixedMask;
		_peakValues.resize(componentIndex + 1, true);
		_xposValues.resize(componentIndex + 1, true);
		_yposValues.resize(componentIndex + 1, true);
		_majValues.resize(componentIndex + 1, true);
		_minValues.resize(componentIndex + 1, true);
		_paValues.resize(componentIndex + 1, true);
		_fixedValues.resize(componentIndex + 1, true);

		ThrowIf(
			! peak.matches(RXdouble),
			"File " + filename + ", line " + *iter
				+ ": peak value " + peak + " is not numeric"
		);
		_peakValues(componentIndex) = String::toDouble(peak);

		if (! xpos.matches(RXdouble) ) {
			*_log << "File " << filename << ", line " << *iter
				<< ": x position value " << xpos << " is not numeric"
				<< LogIO::EXCEPTION;
		}
		_xposValues(componentIndex) = String::toDouble(xpos);

		if (! ypos.matches(RXdouble) ) {
			*_log << "File " << filename << ", line " << *iter
				<< ": y position value " << ypos << " is not numeric"
				<< LogIO::EXCEPTION;
		}
		_yposValues(componentIndex) = String::toDouble(ypos);

		Quantity majQuantity;
		ThrowIf(
			! readQuantity(majQuantity, maj),
			"File " + filename + ", line " + *iter
				+ ": Major axis value " + maj + " is not a quantity"
		);
		_majValues(componentIndex) = majQuantity;

		Quantity minQuantity;
		ThrowIf(
			! readQuantity(minQuantity, min),
			"File " + filename + ", line " + *iter
				+ ": Major axis value " + min + " is not a quantity"
		);
		_minValues(componentIndex) = minQuantity;

		Quantity paQuantity;
		ThrowIf(
			! readQuantity(paQuantity, pa),
			"File " + filename + ", line " + *iter
				+ ": Position angle value " + pa + " is not a quantity"
		);
		_paValues(componentIndex) = paQuantity;

		if (parts.size() == 7) {
			fixedMask = parts[6];
			for (
				String::iterator siter = fixedMask.begin();
				siter != fixedMask.end(); siter++
			) {
				if (
					*siter != 'a' && *siter != 'b' && *siter != 'f'
					&& *siter != 'p' && *siter != 'x' && *siter != 'y'
				) {
					*_log << "fixed parameter ID " << String(*siter) << " is not recognized"
						<< LogIO::EXCEPTION;
				}
			}
			_fixedValues(componentIndex) = fixedMask;
		}
		_fixedValues(componentIndex) = fixedMask;
		componentIndex++;
	}
	ThrowIf(componentIndex == 0, "No valid estmates were found in file " + filename);
}

}
