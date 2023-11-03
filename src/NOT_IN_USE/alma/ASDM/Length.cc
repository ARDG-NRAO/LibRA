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
 * File Length.cpp
 */

#include <alma/ASDM/Length.h>
#include <alma/ASDM/DoubleWrapper.h>
#include <alma/ASDM/NumberFormatException.h>

using namespace std;

namespace asdm {

Length Length::getLength(StringTokenizer &t) {
	double value = Double::parseDouble(t.nextToken());
	return Length (value);
}

bool Length::isZero() const {
	return value == 0.0;
}

double Length::fromString(const string& s) {
	return Double::parseDouble(s);
}

string Length::toString(double x) {
	return Double::toString(x);
}

void Length::toBin(EndianOSStream& eoss) {
	eoss.writeDouble(	value);
}

void Length::toBin(const vector<Length>& len,  EndianOSStream& eoss) {
	eoss.writeInt((int) len.size());
	for (unsigned int i = 0; i < len.size(); i++)
		eoss.writeDouble(len.at(i).value);
}

void Length::toBin(const vector<vector<Length> >& len,  EndianOSStream& eoss) {
	eoss.writeInt((int) len.size());
	eoss.writeInt((int) len.at(0).size());
	for (unsigned int i = 0; i < len.size(); i++)
		for (unsigned int j = 0; j < len.at(0).size(); j++)
			eoss.writeDouble(len.at(i).at(j).value);
}

void Length::toBin(const vector< vector<vector<Length> > >& len,  EndianOSStream& eoss) {
	eoss.writeInt((int) len.size());
	eoss.writeInt((int) len.at(0).size());
	eoss.writeInt((int) len.at(0).at(0).size());	
	for (unsigned int i = 0; i < len.size(); i++)
		for (unsigned int j = 0; j < len.at(0).size(); j++)
				for (unsigned int k = 0; k < len.at(0).at(0).size(); j++)
					eoss.writeDouble(len.at(i).at(j).at(k).value);
}

Length Length::fromBin(EndianIStream & eis) {
	return Length(eis.readDouble());
}

vector<Length> Length::from1DBin(EndianIStream & eis) {
	int dim1 = eis.readInt();
	vector<Length> result;
	for (int i = 0; i < dim1; i++)
		result.push_back(Length(eis.readDouble()));
	return result;	
}

vector<vector<Length > > Length::from2DBin(EndianIStream & eis) {
	int dim1 = eis.readInt();
	int dim2 = eis.readInt();
	vector< vector<Length> >result;
	vector <Length> aux;
	for (int i = 0; i < dim1; i++) {
		aux.clear();
		for (int j = 0; j < dim2; j++)
			aux.push_back(Length(eis.readDouble()));
		result.push_back(aux);
	}
	return result;	
}

vector<vector<vector<Length > > > Length::from3DBin(EndianIStream & eis) {
	int dim1 = eis.readInt();
	int dim2 = eis.readInt();
	int dim3 = eis.readInt();
	vector<vector< vector<Length> > >result;
	vector < vector<Length> >aux1;
	vector <Length> aux2;
	for (int i = 0; i < dim1; i++) {
		aux1.clear();
		for (int j = 0; j < dim2; j++) {
			aux2.clear();
			for (int k = 0; k < dim3; k++)
				aux2.push_back(Length(eis.readDouble()));
			aux1.push_back(aux2);
		}
		result.push_back(aux1);
	}
	return result;	
}

} // End namespace asdm
