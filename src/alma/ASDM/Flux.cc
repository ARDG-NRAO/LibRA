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
 * File Flux.cpp
 */

#include <alma/ASDM/Flux.h>
#include <alma/ASDM/DoubleWrapper.h>
#include <alma/ASDM/NumberFormatException.h>

using namespace std;

namespace asdm {

Flux Flux::getFlux(StringTokenizer &t) {
	double value = Double::parseDouble(t.nextToken());
	return Flux (value);
}

bool Flux::isZero() const {
	return value == 0.0;
}

double Flux::fromString(const string& s) {
	return Double::parseDouble(s);
}

string Flux::toString(double x) {
	return Double::toString(x);
}

void Flux::toBin(EndianOSStream& eoss) {
	eoss.writeDouble(	value);
}

void Flux::toBin(const vector<Flux>& flux,  EndianOSStream& eoss) {
	eoss.writeInt((int) flux.size());
	for (unsigned int i = 0; i < flux.size(); i++)
		eoss.writeDouble(flux.at(i).value);
}

void Flux::toBin(const vector<vector<Flux> >& flux,  EndianOSStream& eoss) {
	eoss.writeInt((int) flux.size());
	eoss.writeInt((int) flux.at(0).size());
	for (unsigned int i = 0; i < flux.size(); i++)
		for (unsigned int j = 0; j < flux.at(0).size(); j++)
			eoss.writeDouble(flux.at(i).at(j).value);
}

void Flux::toBin(const vector< vector<vector<Flux> > >& flux,  EndianOSStream& eoss) {
	eoss.writeInt((int) flux.size());
	eoss.writeInt((int) flux.at(0).size());
	eoss.writeInt((int) flux.at(0).at(0).size());	
	for (unsigned int i = 0; i < flux.size(); i++)
		for (unsigned int j = 0; j < flux.at(0).size(); j++)
				for (unsigned int k = 0; k < flux.at(0).at(0).size(); j++)
					eoss.writeDouble(flux.at(i).at(j).at(k).value);
}

Flux Flux::fromBin(EndianIStream & eis) {
	return Flux(eis.readDouble());
}

vector<Flux> Flux::from1DBin(EndianIStream & eis) {
	int dim1 = eis.readInt();
	vector<Flux> result;
	for (int i = 0; i < dim1; i++)
		result.push_back(Flux(eis.readDouble()));
	return result;	
}

vector<vector<Flux > > Flux::from2DBin(EndianIStream & eis) {
	int dim1 = eis.readInt();
	int dim2 = eis.readInt();
	vector< vector<Flux> >result;
	vector <Flux> aux;
	for (int i = 0; i < dim1; i++) {
		aux.clear();
		for (int j = 0; j < dim2; j++)
			aux.push_back(Flux(eis.readDouble()));
		result.push_back(aux);
	}
	return result;	
}

vector<vector<vector<Flux > > > Flux::from3DBin(EndianIStream & eis) {
	int dim1 = eis.readInt();
	int dim2 = eis.readInt();
	int dim3 = eis.readInt();
	vector<vector< vector<Flux> > >result;
	vector < vector<Flux> >aux1;
	vector <Flux> aux2;
	for (int i = 0; i < dim1; i++) {
		aux1.clear();
		for (int j = 0; j < dim2; j++) {
			aux2.clear();
			for (int k = 0; k < dim3; k++)
				aux2.push_back(Flux(eis.readDouble()));
			aux1.push_back(aux2);
		}
		result.push_back(aux1);
	}
	return result;	
}

} // End namespace asdm
