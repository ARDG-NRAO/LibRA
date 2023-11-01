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

#include <imageanalysis/Annotations/AnnText.h>

using namespace casacore;
namespace casa {

AnnText::AnnText(
	const Quantity& xPos, const Quantity& yPos,
	const String& dirRefFrameString,
	const CoordinateSystem& csys,
	const String& text,
	const Quantity& beginFreq,
	const Quantity& endFreq,
	const String& freqRefFrame,
	const String& dopplerString,
	const Quantity& restfreq,
	const Vector<Stokes::StokesTypes>& stokes
) : AnnotationBase(
		TEXT, dirRefFrameString, csys, beginFreq,
		endFreq, freqRefFrame, dopplerString, restfreq, stokes
	),
	_inputDirection(AnnotationBase::Direction(1)),
	_text(text) {
	_init(xPos, yPos);
}

AnnText::AnnText(
	const Quantity& xPos, const Quantity& yPos,
	const CoordinateSystem& csys,
	const String& text,
	const Vector<Stokes::StokesTypes>& stokes
) : AnnotationBase(TEXT, csys, stokes),
	_inputDirection(AnnotationBase::Direction(1)),
	_text(text) {
	_init(xPos, yPos);
}

AnnText& AnnText::operator= (
	const AnnText& other
) {
    if (this == &other) {
    	return *this;
    }
    AnnotationBase::operator=(other);
    _inputDirection.resize(other._inputDirection.nelements());
    _inputDirection = other._inputDirection;
    _text = other._text;
    return *this;
}

void AnnText::_init(const Quantity& x, const Quantity& y) {
	_inputDirection[0].first = x;
	_inputDirection[0].second = y;
	_checkAndConvertDirections(
		String(__FUNCTION__), _inputDirection
	);
}

MDirection AnnText::getDirection() const {
	return getConvertedDirections()[0];
}

String AnnText::getText() const {
	return _text;
}

ostream& AnnText::print(ostream &os) const {
	os << "text [["
		<< _printDirection(_inputDirection[0].first, _inputDirection[0].second)
		<< "], \"" << _text << "\"]";
	_printPairs(os);
	return os;
}

}


