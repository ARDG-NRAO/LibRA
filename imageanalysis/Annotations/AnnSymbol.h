//# Copyright (C) 1998,1999,2000,2001
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

#ifndef ANNOTATIONS_ANNSYMBOL_H
#define ANNOTATIONS_ANNSYMBOL_H

#include <imageanalysis/Annotations/AnnotationBase.h>

#include <measures/Measures/MDirection.h>

namespace casa {

// <summary>Represents a symbol annotation</summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd">
// </reviewed>

// <synopsis>

// Represents an ascii symbol annotation
// </synopsis>

class AnnSymbol: public AnnotationBase {
public:

	// allowed symbols
	enum Symbol {
		POINT,
		PIXEL,
		CIRCLE,
		TRIANGLE_DOWN,
		TRIANGLE_UP,
		TRIANGLE_LEFT,
		TRIANGLE_RIGHT,
		TRI_DOWN,
		TRI_UP,
		TRI_LEFT,
		TRI_RIGHT,
		SQUARE,
		PENTAGON,
		STAR,
		HEXAGON1,
		HEXAGON2,
		PLUS,
		X,
		DIAMOND,
		THIN_DIAMOND,
		VLINE,
		HLINE,
		UNKNOWN
	};

	AnnSymbol(
		const casacore::Quantity& x, const casacore::Quantity& y,
		const casacore::String& dirRefFrameString,
		const casacore::CoordinateSystem& csys,
		const casacore::Char symbolChar,
		const casacore::Quantity& beginFreq,
		const casacore::Quantity& endFreq,
		const casacore::String& freqRefFrame,
		const casacore::String& dopplerString,
		const casacore::Quantity& restfreq,
		const casacore::Vector<casacore::Stokes::StokesTypes>& stokes
	);

	AnnSymbol(
		const casacore::Quantity& x, const casacore::Quantity& y,
		const casacore::CoordinateSystem& csys,
		const Symbol symbol,
		const casacore::Vector<casacore::Stokes::StokesTypes>& stokes
	);

	// implicit copy constructor and destructor are fine

	AnnSymbol& operator=(const AnnSymbol& other);

	casacore::MDirection getDirection() const;

	Symbol getSymbol() const;

	static Symbol charToSymbol(const casacore::Char c);

	static casacore::Char symbolToChar(const Symbol s);

	virtual std::ostream& print(std::ostream &os) const;

private:
	AnnotationBase::Direction _inputDirection;
	Symbol _symbol;
	casacore::Char _symbolChar;
	const static casacore::String _class;
	static std::map<casacore::Char, Symbol> _symbolMap;

	static void _initMap();

	void _init(const casacore::Quantity& x, const casacore::Quantity& y);

};



}

#endif
