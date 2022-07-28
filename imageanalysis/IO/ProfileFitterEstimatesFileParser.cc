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

#include <imageanalysis/IO/ProfileFitterEstimatesFileParser.h>

#include <casacore/casa/aips.h>
#include <casacore/casa/IO/RegularFileIO.h>
#include <casacore/casa/Utilities/Regex.h>
#include <casacore/casa/Containers/Record.h>
#include <components/SpectralComponents/GaussianSpectralElement.h>
#include <casacore/images/Images/ImageStatistics.h>


using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

const String ProfileFitterEstimatesFileParser::_class = "FitterEstimatesFileParser";

ProfileFitterEstimatesFileParser::ProfileFitterEstimatesFileParser (
	const String& filename
) : _spectralList(), _fixedValues(0), _log(LogIO()), _peakValues(0),
	_centerValues(0), _fwhmValues(0), _contents("") {
	RegularFile myFile(filename);
	_log << LogOrigin(_class, __FUNCTION__);

	if (! myFile.exists()) {
		_log << "Estimates file " << filename << " does not exist"
			<< LogIO::EXCEPTION;
	}
	if (! myFile.isReadable()) {
		_log << "Estimates file " << filename << " is not readable"
			<< LogIO::EXCEPTION;
	}
	_parseFile(myFile);
	_createSpectralList();

}

ProfileFitterEstimatesFileParser::~ProfileFitterEstimatesFileParser() {}

SpectralList ProfileFitterEstimatesFileParser::getEstimates() const {
	return _spectralList;
}

vector<String> ProfileFitterEstimatesFileParser::getFixed() const {
	return _fixedValues;
}

String ProfileFitterEstimatesFileParser::getContents() const {
	return _contents;
}

void ProfileFitterEstimatesFileParser::_parseFile(
	const RegularFile& myFile
) {
	_log << LogOrigin(_class, __FUNCTION__);

	RegularFileIO fileIO(myFile);
	// I doubt a genuine estimates file will ever have this many characters
	Int bufSize = 4096;
	char *buffer = new char[bufSize];
	int nRead;

	while ((nRead = fileIO.read(bufSize, buffer, false)) == bufSize) {
		_log << LogIO::NORMAL << "read: " << nRead << LogIO::POST;
		String chunk(buffer, bufSize);

		_contents += chunk;
	}
	// get the last chunk
	String chunk(buffer, nRead);
	_contents += chunk;

	Vector<String> lines = stringToVector(_contents, '\n');
	Regex blankLine("^[ \n\t\r\v\f]+$",1000);
	for(Vector<String>::iterator iter=lines.begin(); iter!=lines.end(); iter++) {
		if (iter->empty() || iter->firstchar() == '#' ||  iter->matches(blankLine)) {
			// ignore comments and blank lines
			continue;
		}
		uInt commaCount = iter->freq(',');
		if (commaCount < 2 || commaCount > 3) {
			_log << "bad format for line " << *iter << LogIO::EXCEPTION;
		}
		Vector<String> parts = stringToVector(*iter);
		for (Vector<String>::iterator viter = parts.begin(); viter != parts.end(); viter++) {
			viter->trim();
		}
		String filename = myFile.path().dirName() + "/" + myFile.path().baseName();
		String peak = parts[0];
		String center = parts[1];
		String fwhm = parts[2];

		String fixedMask;

		if (! peak.matches(RXdouble) ) {
			_log << "File " << filename << ", line " << *iter
				<< ": peak value " << peak << " is not numeric"
				<< LogIO::EXCEPTION;
		}
		_peakValues.push_back(String::toDouble(peak));

		if (! center.matches(RXdouble) ) {
			_log << "File " << filename << ", line " << *iter
				<< ": x position value " << center << " is not numeric"
				<< LogIO::EXCEPTION;
		}
		_centerValues.push_back(String::toDouble(center));

		if (! fwhm.matches(RXdouble)) {
			_log << "File " << filename << ", line " << *iter
				<< ": Major axis value " << fwhm << " is not numeric"
				<< LogIO::EXCEPTION;
		}
		_fwhmValues.push_back(String::toDouble(fwhm));

		if (parts.size() == 4) {
			fixedMask = parts[3];
			for (
				String::iterator siter = fixedMask.begin();
				siter != fixedMask.end(); siter++
			) {
				if (
					*siter != 'c' && *siter != 'f' && *siter != 'p'
				) {
					_log << "fixed parameter ID " << String(*siter) << " is not recognized"
						<< LogIO::EXCEPTION;
				}
			}
		}
		_fixedValues.push_back(fixedMask);
	}
}

void ProfileFitterEstimatesFileParser::_createSpectralList() {
	for (uInt i=0; i<_peakValues.size(); i++) {
		GaussianSpectralElement se(
			_peakValues[i],
			_centerValues[i],
			GaussianSpectralElement::sigmaFromFWHM(_fwhmValues[i])
		);
		if (! _fixedValues[i].empty()) {
			se.fixByString(_fixedValues[i]);
		}
		_spectralList.add(se);
	}

}
} //# NAMESPACE CASA - END

