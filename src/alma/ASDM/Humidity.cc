/*
 * ALMA - Atacama Large Millimeter Array
 * (c) European Southern Observatory, 2002
 * (c) Associated Universities Inc., 2002
 * Copyright by ESO (in the framework of the ALMA collaboration),
 * Copyright by AUI (in the framework of the ALMA collaboration),
 * All rights reserved.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307  USA
 *
 * File Humidity.cpp
 */

#include <alma/ASDM/Humidity.h>
#include <alma/ASDM/DoubleWrapper.h>
#include <alma/ASDM/NumberFormatException.h>

using namespace std;

namespace asdm {

Humidity Humidity::getHumidity(StringTokenizer &t) {
	double value = Double::parseDouble(t.nextToken());
	return Humidity (value);
}

bool Humidity::isZero() const {
	return value == 0.0;
}

double Humidity::fromString(const string& s) {
	return Double::parseDouble(s);
}

string Humidity::toString(double x) {
	return Double::toString(x);
}

void Humidity::toBin(EndianOSStream& eoss) {
	eoss.writeDouble(	value);
}

void Humidity::toBin(const vector<Humidity>& humidity,  EndianOSStream& eoss) {
	eoss.writeInt((int) humidity.size());
	for (unsigned int i = 0; i < humidity.size(); i++)
		eoss.writeDouble(humidity.at(i).value);
}

void Humidity::toBin(const vector<vector<Humidity> >& humidity,  EndianOSStream& eoss) {
	eoss.writeInt((int) humidity.size());
	eoss.writeInt((int) humidity.at(0).size());
	for (unsigned int i = 0; i < humidity.size(); i++)
		for (unsigned int j = 0; j < humidity.at(0).size(); j++)
			eoss.writeDouble(humidity.at(i).at(j).value);
}

void Humidity::toBin(const vector< vector<vector<Humidity> > >& humidity,  EndianOSStream& eoss) {
	eoss.writeInt((int) humidity.size());
	eoss.writeInt((int) humidity.at(0).size());
	eoss.writeInt((int) humidity.at(0).at(0).size());	
	for (unsigned int i = 0; i < humidity.size(); i++)
		for (unsigned int j = 0; j < humidity.at(0).size(); j++)
				for (unsigned int k = 0; k < humidity.at(0).at(0).size(); j++)
					eoss.writeDouble(humidity.at(i).at(j).at(k).value);
}

Humidity Humidity::fromBin(EndianIStream & eis) {
	return Humidity(eis.readDouble());
}

vector<Humidity> Humidity::from1DBin(EndianIStream & eis) {
	int dim1 = eis.readInt();
	vector<Humidity> result;
	for (int i = 0; i < dim1; i++)
		result.push_back(Humidity(eis.readDouble()));
	return result;	
}

vector<vector<Humidity > > Humidity::from2DBin(EndianIStream & eis) {
	int dim1 = eis.readInt();
	int dim2 = eis.readInt();
	vector< vector<Humidity> >result;
	vector <Humidity> aux;
	for (int i = 0; i < dim1; i++) {
		aux.clear();
		for (int j = 0; j < dim2; j++)
			aux.push_back(Humidity(eis.readDouble()));
		result.push_back(aux);
	}
	return result;	
}

vector<vector<vector<Humidity > > > Humidity::from3DBin(EndianIStream & eis) {
	int dim1 = eis.readInt();
	int dim2 = eis.readInt();
	int dim3 = eis.readInt();
	vector<vector< vector<Humidity> > >result;
	vector < vector<Humidity> >aux1;
	vector <Humidity> aux2;
	for (int i = 0; i < dim1; i++) {
		aux1.clear();
		for (int j = 0; j < dim2; j++) {
			aux2.clear();
			for (int k = 0; k < dim3; k++)
				aux2.push_back(Humidity(eis.readDouble()));
			aux1.push_back(aux2);
		}
		result.push_back(aux1);
	}
	return result;	
}


} // End namespace asdm
