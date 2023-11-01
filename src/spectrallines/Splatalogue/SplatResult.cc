//# Copyright (C) 2004
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

#include "SplatResult.h"
#include <sstream>
#include <stdio.h>

namespace casa {

SplatResult::SplatResult(int speciesId, const string& species,
		const string& chemicalName, const string& quantumNumbers,
		const pair<double,string>& frequency,
		const pair<double,string>& temperature,
		double smu2, const pair<double,string>& el,
		const pair<double,string>& eu, double logA, double intensity) {
	this->_speciesId = speciesId;
	this->_species = species;
	this->_chemicalName = chemicalName;
	this->_quantumNumbers = quantumNumbers;
	this->_frequency = frequency;
	this->_temperature = temperature;
	this->_smu2 = smu2;
	this->_el = el;
	this->_eu = eu;
	this->_logA = logA;
	this->_intensity = intensity;

}

int SplatResult::getSpeciesId() const {
	return _speciesId;
}

std::string SplatResult::getSpecies() const {
	return _species;
}
std::string SplatResult::getChemicalName() const {
	return _chemicalName;
}
std::string SplatResult::getQuantumNumbers() const {
	return _quantumNumbers;
}

pair<double,std::string> SplatResult::getFrequency() const {
	return _frequency;
}

pair<double,std::string> SplatResult::getTemperature() const {
	return _temperature;
}

double SplatResult::getSmu2() const {
	return _smu2;
}
pair<double,std::string> SplatResult::getEL() const {
	return _el;
}
pair<double,std::string> SplatResult::getEU() const {
	return _eu;
}
double SplatResult::getLogA() const {
	return _logA;
}

double SplatResult::getIntensity() const {
	return _intensity;
}
string SplatResult::toString() const {
	ostringstream os;
	os << "Species:      "<< _species << "\n";
	os << "Chemical Name:"<< _chemicalName << "\n";
	os << "Frequency:    "<< _frequency.first << " "<<_frequency.second<<"\n";
	os << "Temperature:  "<< _temperature.first << " "<<_temperature.second<<"\n";
	os << "Resolved QNs: "<< _quantumNumbers << "\n";
	os << "Intensity:    "<< _intensity << "\n";
	os << "Siju2:        "<< _smu2 << "\n";
	os << "LOGA:         "<< _logA << "\n";
	os << "EL:           "<< _el.first << " "<< _el.second<< "\n";
	os << "EU:           "<< _eu.first << " "<< _eu.second <<"\n";
	return os.str();
}

string SplatResult::toLine( string spacer ) const {
	ostringstream os;
	const int COLUMN_WIDTH = 30;
	const int NUM_COLUMN_WIDTH = 16;
	char speciesStr[COLUMN_WIDTH];
	string formatStr( "%-29.29s");
	string numFormatStr( "%15.6f");
	sprintf( speciesStr, formatStr.c_str(), _species.c_str());
	char chemStr[COLUMN_WIDTH];
	sprintf( chemStr, formatStr.c_str(), _chemicalName.c_str());
	char freqStr[NUM_COLUMN_WIDTH];
	sprintf( freqStr, numFormatStr.c_str(), _frequency.first );
	char tempStr[NUM_COLUMN_WIDTH];
	sprintf( tempStr, numFormatStr.c_str(), _temperature.first);
	char qnsStr[COLUMN_WIDTH];
	sprintf( qnsStr, formatStr.c_str(), _quantumNumbers.c_str() );
	char intStr[NUM_COLUMN_WIDTH];
	sprintf( intStr, numFormatStr.c_str(), _intensity );
	char smu2Str[NUM_COLUMN_WIDTH];
	sprintf( smu2Str, numFormatStr.c_str(), _smu2 );
	char logaStr[NUM_COLUMN_WIDTH];
	sprintf( logaStr, numFormatStr.c_str(), _logA );
	char elStr[NUM_COLUMN_WIDTH];
	sprintf( elStr, numFormatStr.c_str(), _el.first );
	char euStr[NUM_COLUMN_WIDTH];
	sprintf( euStr, numFormatStr.c_str(), _eu.first );
	os << speciesStr << spacer << chemStr << spacer << freqStr << spacer <<
			tempStr << spacer <<
			qnsStr<< spacer << intStr<< spacer << spacer << smu2Str<< spacer
			<< spacer << logaStr<< spacer <<elStr << spacer << euStr << endl;
	return os.str();
}

SplatResult::~SplatResult() {
	// TODO Auto-generated destructor stub
}

} /* namespace casa */
