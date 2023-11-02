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
 * File Interval.cpp
 */

#include <alma/ASDM/Interval.h>
#include <alma/ASDM/LongWrapper.h>
#include <alma/ASDM/NumberFormatException.h>

using namespace std;

namespace asdm {

Interval Interval::getInterval(StringTokenizer &t) {
	int64_t value = Long::parseLong(t.nextToken());
	return Interval (value);
}

bool Interval::isZero() const {
	return value == 0L;
}

int64_t Interval::fromString(const string& s) {
	return Long::parseLong(s);
}

string Interval::toString(int64_t x) {
	return Long::toString(x);
}

void  Interval::toBin(EndianOSStream& eoss) {
	eoss.writeLongLong(value);	
}

void Interval::toBin(vector<Interval> interval, EndianOSStream& eoss) {
	eoss.writeInt((int) interval.size());
	for (unsigned int i = 0; i < interval.size(); i++) 
		eoss.writeLongLong(interval.at(i).value);
}

void Interval::toBin(vector<vector<Interval> >interval, EndianOSStream& eoss) {
	eoss.writeInt((int) interval.size());
	eoss.writeInt((int) interval.at(0).size());
	for (unsigned int i = 0; i < interval.size(); i++)
		for (unsigned int j = 0; j < interval.at(0).size(); j++)		 
			eoss.writeLongLong(interval.at(i).at(j).value);
}

 Interval Interval::fromBin(EndianIStream& eis) {
 	return (Interval(eis.readLongLong()));
 }
 
 vector<Interval> Interval::from1DBin(EndianIStream& eis) {
 	vector<Interval> result;
 	int dim = eis.readInt();
 	for (int i = 0; i < dim; i++)
 		result.push_back(eis.readLongLong());
 	return result;	
 }

vector< vector<Interval> >Interval::from2DBin(EndianIStream& eis) {
 	vector<vector<Interval> >result;
 	int dim1 = eis.readInt(); 
	int dim2 = eis.readInt(); 
	vector<Interval> aux;	
 	for (int i = 0; i < dim1; i++) {
 		aux.clear();
 		for (int j = 0; j < dim2; j++)
 			aux.push_back(eis.readLongLong());
 		result.push_back(aux);		
 	}
 	return result;	
}
} // End namespace asdm
