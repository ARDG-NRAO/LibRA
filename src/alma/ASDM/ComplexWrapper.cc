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
 * File Complex.cpp
 */

#include <alma/ASDM/ComplexWrapper.h>
#include <alma/ASDM/DoubleWrapper.h>
#include <alma/ASDM/NumberFormatException.h>

using namespace std;

namespace asdm {

Complex Complex::getComplex(StringTokenizer &t) {
	double r = Double::parseDouble(t.nextToken());
	double i = Double::parseDouble(t.nextToken());
	return Complex (r,i);
}

Complex Complex::fromString(const string& s) {
	string::size_type n = s.find(' ');
	if (n == string::npos)
		throw NumberFormatException("Not a complex number.");
	double r = Double::parseDouble(s.substr(0,n));
	double i = Double::parseDouble(s.substr(n+1, s.length() - (n + 1)));
	return Complex(r,i);
}

string Complex::toString(const Complex &x) {
	return Double::toString(x.getReal()) + " " + Double::toString(x.getImg());
}

void Complex::toBin(EndianOSStream& eoss) {
	eoss.writeDouble(	real());
	eoss.writeDouble(	imag());
}

void Complex::toBin(const vector<Complex>& cmplx,  EndianOSStream& eoss) {
	eoss.writeInt((int) cmplx.size());
	for (unsigned int i = 0; i < cmplx.size(); i++) {
		eoss.writeDouble(cmplx.at(i).real());
		eoss.writeDouble(cmplx.at(i).imag());
	}
}

void Complex::toBin(const vector<vector<Complex> >& cmplx,  EndianOSStream& eoss) {
	eoss.writeInt((int) cmplx.size());
	eoss.writeInt((int) cmplx.at(0).size());
	for (unsigned int i = 0; i < cmplx.size(); i++)
		for (unsigned int j = 0; j < cmplx.at(0).size(); j++) {
			eoss.writeDouble(cmplx.at(i).at(j).real());
			eoss.writeDouble(cmplx.at(i).at(j).imag());
		}
}

void Complex::toBin(const vector< vector<vector<Complex> > >& cmplx,  EndianOSStream& eoss) {
	eoss.writeInt((int) cmplx.size());
	eoss.writeInt((int) cmplx.at(0).size());
	eoss.writeInt((int) cmplx.at(0).at(0).size());	
	for (unsigned int i = 0; i < cmplx.size(); i++)
		for (unsigned int j = 0; j < cmplx.at(0).size(); j++)
				for (unsigned int k = 0; k < cmplx.at(0).at(0).size(); j++) {
					eoss.writeDouble(cmplx.at(i).at(j).at(k).real());
					eoss.writeDouble(cmplx.at(i).at(j).at(k).imag());
				}
}

Complex Complex::fromBin(EndianIStream & eis) {
	return Complex(eis.readDouble(), eis.readDouble());
}

vector<Complex> Complex::from1DBin(EndianIStream & eis) {
	int dim1 = eis.readInt();
	vector<Complex> result;
	for (int i = 0; i < dim1; i++)
		result.push_back(Complex(eis.readDouble(),  eis.readDouble()));
	return result;	
}

vector<vector<Complex > > Complex::from2DBin(EndianIStream & eis) {
	int dim1 = eis.readInt();
	int dim2 = eis.readInt();
	vector< vector<Complex> >result;
	vector <Complex> aux;
	for (int i = 0; i < dim1; i++) {
		aux.clear();
		for (int j = 0; j < dim2; j++)
			aux.push_back(Complex(eis.readDouble(), eis.readDouble()));
		result.push_back(aux);
	}
	return result;	
}

vector<vector<vector<Complex > > > Complex::from3DBin(EndianIStream & eis) {
	int dim1 = eis.readInt();
	int dim2 = eis.readInt();
	int dim3 = eis.readInt();
	vector<vector< vector<Complex> > >result;
	vector < vector<Complex> >aux1;
	vector <Complex> aux2;
	for (int i = 0; i < dim1; i++) {
		aux1.clear();
		for (int j = 0; j < dim2; j++) {
			aux2.clear();
			for (int k = 0; k < dim3; k++)
				aux2.push_back(Complex(eis.readDouble(), eis.readDouble()));
			aux1.push_back(aux2);
		}
		result.push_back(aux1);
	}
	return result;	
}

} // End namespace asdm
