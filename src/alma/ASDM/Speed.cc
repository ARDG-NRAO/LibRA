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
 * File Speed.cpp
 */

#include <alma/ASDM/Speed.h>
#include <alma/ASDM/DoubleWrapper.h>
#include <alma/ASDM/NumberFormatException.h>

using namespace std;

namespace asdm {

Speed Speed::getSpeed(StringTokenizer &t) {
	double value = Double::parseDouble(t.nextToken());
	return Speed (value);
}

bool Speed::isZero() const {
	return value == 0.0;
}

double Speed::fromString(const string& s) {
	return Double::parseDouble(s);
}

string Speed::toString(double x) {
	return Double::toString(x);
}

void Speed::toBin(EndianOSStream& eoss) {
	eoss.writeDouble(	value);
}

void Speed::toBin(const vector<Speed>& speed,  EndianOSStream& eoss) {
	eoss.writeInt((int) speed.size());
	for (unsigned int i = 0; i < speed.size(); i++)
		eoss.writeDouble(speed.at(i).value);
}

void Speed::toBin(const vector<vector<Speed> >& speed,  EndianOSStream& eoss) {
	eoss.writeInt((int) speed.size());
	eoss.writeInt((int) speed.at(0).size());
	for (unsigned int i = 0; i < speed.size(); i++)
		for (unsigned int j = 0; j < speed.at(0).size(); j++)
			eoss.writeDouble(speed.at(i).at(j).value);
}

void Speed::toBin(const vector< vector<vector<Speed> > >& speed,  EndianOSStream& eoss) {
	eoss.writeInt((int) speed.size());
	eoss.writeInt((int) speed.at(0).size());
	eoss.writeInt((int) speed.at(0).at(0).size());	
	for (unsigned int i = 0; i < speed.size(); i++)
		for (unsigned int j = 0; j < speed.at(0).size(); j++)
				for (unsigned int k = 0; k < speed.at(0).at(0).size(); j++)
					eoss.writeDouble(speed.at(i).at(j).at(k).value);
}

Speed Speed::fromBin(EndianIStream & eis) {
	return Speed(eis.readDouble());
}

vector<Speed> Speed::from1DBin(EndianIStream & eis) {
	int dim1 = eis.readInt();
	vector<Speed> result;
	for (int i = 0; i < dim1; i++)
		result.push_back(Speed(eis.readDouble()));
	return result;	
}

vector<vector<Speed > > Speed::from2DBin(EndianIStream & eis) {
	int dim1 = eis.readInt();
	int dim2 = eis.readInt();
	vector< vector<Speed> >result;
	vector <Speed> aux;
	for (int i = 0; i < dim1; i++) {
		aux.clear();
		for (int j = 0; j < dim2; j++)
			aux.push_back(Speed(eis.readDouble()));
		result.push_back(aux);
	}
	return result;	
}

vector<vector<vector<Speed > > > Speed::from3DBin(EndianIStream & eis) {
	int dim1 = eis.readInt();
	int dim2 = eis.readInt();
	int dim3 = eis.readInt();
	vector<vector< vector<Speed> > >result;
	vector < vector<Speed> >aux1;
	vector <Speed> aux2;
	for (int i = 0; i < dim1; i++) {
		aux1.clear();
		for (int j = 0; j < dim2; j++) {
			aux2.clear();
			for (int k = 0; k < dim3; k++)
				aux2.push_back(Speed(eis.readDouble()));
			aux1.push_back(aux2);
		}
		result.push_back(aux1);
	}
	return result;	
}

} // End namespace asdm
