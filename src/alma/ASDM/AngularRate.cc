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
 * File AngularRate.cpp
 */

#include <alma/ASDM/AngularRate.h>
#include <alma/ASDM/DoubleWrapper.h>
#include <alma/ASDM/NumberFormatException.h>

using namespace std;

namespace asdm {

AngularRate AngularRate::getAngularRate(StringTokenizer &t) {
	double value = Double::parseDouble(t.nextToken());
	return AngularRate (value);
}

bool AngularRate::isZero() const {
	return value == 0.0;
}

double AngularRate::fromString(const string& s) {
	return Double::parseDouble(s);
}

string AngularRate::toString(double x) {
	return Double::toString(x);
}

void AngularRate::toBin(EndianOSStream& eoss) {
	eoss.writeDouble(	value);
}

void AngularRate::toBin(const vector<AngularRate>& angularRate,  EndianOSStream& eoss) {
	eoss.writeInt((int) angularRate.size());
	for (unsigned int i = 0; i < angularRate.size(); i++)
		eoss.writeDouble(angularRate.at(i).value);
}

void AngularRate::toBin(const vector<vector<AngularRate> >& angularRate,  EndianOSStream& eoss) {
	eoss.writeInt((int) angularRate.size());
	eoss.writeInt((int) angularRate.at(0).size());
	for (unsigned int i = 0; i < angularRate.size(); i++)
		for (unsigned int j = 0; j < angularRate.at(0).size(); j++)
			eoss.writeDouble(angularRate.at(i).at(j).value);
}

void AngularRate::toBin(const vector< vector<vector<AngularRate> > >& angularRate,  EndianOSStream& eoss) {
	eoss.writeInt((int) angularRate.size());
	eoss.writeInt((int) angularRate.at(0).size());
	eoss.writeInt((int) angularRate.at(0).at(0).size());	
	for (unsigned int i = 0; i < angularRate.size(); i++)
		for (unsigned int j = 0; j < angularRate.at(0).size(); j++)
				for (unsigned int k = 0; k < angularRate.at(0).at(0).size(); j++)
					eoss.writeDouble(angularRate.at(i).at(j).at(k).value);
}

AngularRate AngularRate::fromBin(EndianIStream & eis) {
	return AngularRate(eis.readDouble());
}

vector<AngularRate> AngularRate::from1DBin(EndianIStream & eis) {
	int dim1 = eis.readInt();
	vector<AngularRate> result;
	for (int i = 0; i < dim1; i++)
		result.push_back(AngularRate(eis.readDouble()));
	return result;	
}

vector<vector<AngularRate > > AngularRate::from2DBin(EndianIStream & eis) {
	int dim1 = eis.readInt();
	int dim2 = eis.readInt();
	vector< vector<AngularRate> >result;
	vector <AngularRate> aux;
	for (int i = 0; i < dim1; i++) {
		aux.clear();
		for (int j = 0; j < dim2; j++)
			aux.push_back(AngularRate(eis.readDouble()));
		result.push_back(aux);
	}
	return result;	
}

vector<vector<vector<AngularRate > > > AngularRate::from3DBin(EndianIStream & eis) {
	int dim1 = eis.readInt();
	int dim2 = eis.readInt();
	int dim3 = eis.readInt();
	vector<vector< vector<AngularRate> > >result;
	vector < vector<AngularRate> >aux1;
	vector <AngularRate> aux2;
	for (int i = 0; i < dim1; i++) {
		aux1.clear();
		for (int j = 0; j < dim2; j++) {
			aux2.clear();
			for (int k = 0; k < dim3; k++)
				aux2.push_back(AngularRate(eis.readDouble()));
			aux1.push_back(aux2);
		}
		result.push_back(aux1);
	}
	return result;	
}
} // End namespace asdm
