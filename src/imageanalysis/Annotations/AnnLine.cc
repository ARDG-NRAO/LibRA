//# GaussianShape.cc:
//# Copyright (C) 1998,1999,2000
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

#include <imageanalysis/Annotations/AnnLine.h>

using namespace casacore;
namespace casa {

AnnLine::AnnLine(
	const Quantity& xPoint1,
	const Quantity& yPoint1,
	const Quantity& xPoint2,
	const Quantity& yPoint2,
	const String& dirRefFrameString,
	const CoordinateSystem& csys,
	const Quantity& beginFreq,
	const Quantity& endFreq,
	const String& freqRefFrame,
	const String& dopplerString,
	const Quantity& restfreq,
	const Vector<Stokes::StokesTypes>& stokes
) : AnnotationBase(
		LINE, dirRefFrameString, csys, beginFreq, endFreq,
		freqRefFrame, dopplerString, restfreq, stokes
	),
	_inputPoints(AnnotationBase::Direction(2)) {

	_inputPoints(0).first = xPoint1;
	_inputPoints(0).second = yPoint1;
	_inputPoints(1).first = xPoint2;
	_inputPoints(1).second = yPoint2;
	_checkAndConvertDirections(String(__FUNCTION__), _inputPoints);
}

AnnLine::AnnLine(
	const Quantity& xPoint1,
	const Quantity& yPoint1,
	const Quantity& xPoint2,
	const Quantity& yPoint2,
	const CoordinateSystem& csys,
	const Vector<Stokes::StokesTypes>& stokes

) : AnnotationBase(LINE, csys, stokes),
	_inputPoints(AnnotationBase::Direction(2)) {

	_inputPoints(0).first = xPoint1;
	_inputPoints(0).second = yPoint1;
	_inputPoints(1).first = xPoint2;
	_inputPoints(1).second = yPoint2;
	_checkAndConvertDirections(String(__FUNCTION__), _inputPoints);
}

AnnLine& AnnLine::operator= (
	const AnnLine& other
) {
    if (this == &other) {
    	return *this;
    }
    AnnotationBase::operator=(other);
    _inputPoints.resize(other._inputPoints.size());
    _inputPoints = other._inputPoints;
    return *this;
}

Vector<MDirection> AnnLine::getEndPoints() const {
	return getConvertedDirections();
}

ostream& AnnLine::print(ostream &os) const {
	os << "line [["
		<< _printDirection(_inputPoints(0).first, _inputPoints(0).second)
		<< "], ["
		<< _printDirection(_inputPoints(1).first, _inputPoints(1).second)
		<< "]]";
	_printPairs(os);
	return os;
}


}


