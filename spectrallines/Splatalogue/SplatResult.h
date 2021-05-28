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

#ifndef SPLATRESULT_H_
#define SPLATRESULT_H_
#include <string>
using namespace std;

namespace casa {

class SplatResult {
public:
	SplatResult( int speciesId, const string& species,
			const string& chemicalName,
			const string& quantumNumbers, const pair<double,string>& frequency,
			const pair<double,string>& temperature,
			double smu2, const pair<double,string>& el,
			const pair<double,string>& eu, double logA, double intensity  );
	int getSpeciesId() const;
	std::string getSpecies() const;
	std::string getChemicalName() const;
	std::string getQuantumNumbers() const;
	pair<double,std::string> getFrequency() const;
	pair<double,std::string> getTemperature() const;
	pair<double,std::string> getEL() const;
	pair<double,std::string> getEU() const;
	double getLogA() const;
	double getSmu2() const;
	double getIntensity() const;
	string toString() const;
	string toLine(string spacer = " ") const;
	virtual ~SplatResult();
private:
	std::string _species;
	std::string _chemicalName;
	std::string _quantumNumbers;
	int _speciesId;
	pair<double,std::string> _frequency;
	pair<double,std::string> _temperature;
	pair<double,std::string> _el;
	pair<double,std::string> _eu;
	double _smu2;
	double _logA;
	double _intensity;
};

} /* namespace casa */
#endif /* SPLATRESULT_H_ */
