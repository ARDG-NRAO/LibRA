//# AsciiRegionLine.cc
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

#include <imageanalysis/IO/AsciiAnnotationFileLine.h>

using namespace casacore;
namespace casa {

AsciiAnnotationFileLine::AsciiAnnotationFileLine()
	: _type(UNKNOWN_TYPE),
	  _comment(""), _annotationBase(0),
	  _globals() {}


AsciiAnnotationFileLine::AsciiAnnotationFileLine(
	const String& comment
) : _type(COMMENT),
	  _comment(comment),
	  _annotationBase(0),
	  _globals() {
	String b = _comment;
	b.trim();
	if (! b.empty() && ! b.startsWith("#")) {
		_comment = "#" + comment;
	}
}

AsciiAnnotationFileLine::AsciiAnnotationFileLine(
	CountedPtr<const AnnotationBase> annotationBase
) : _type(ANNOTATION), _comment(""),
	_annotationBase(annotationBase),
	_globals() {}

AsciiAnnotationFileLine::AsciiAnnotationFileLine(
	const std::map<AnnotationBase::Keyword, String>& globals
) : _type(GLOBAL), _comment(""),
	_annotationBase(0),
	_globals(globals) {}


AsciiAnnotationFileLine& AsciiAnnotationFileLine::operator= (
	const AsciiAnnotationFileLine& other
) {
	if (this != &other) {
		_type = other._type;
		_comment = other._comment;
		_annotationBase = other._annotationBase;
		_globals = other._globals;
	}
	return *this;
}

String AsciiAnnotationFileLine::getComment() const {
	return _comment;
}

std::map<AnnotationBase::Keyword, String> AsciiAnnotationFileLine::getGloabalParams() const {
	return _globals;
}

CountedPtr<const AnnotationBase> AsciiAnnotationFileLine::getAnnotationBase() const {
	return _annotationBase;
}

AsciiAnnotationFileLine::Type AsciiAnnotationFileLine::getType() const {
	return _type;
}

ostream& AsciiAnnotationFileLine::print(ostream& os) const {
	switch(_type) {
	case COMMENT:
		os << _comment;
		return os;
	case ANNOTATION:
		os << *_annotationBase;
		return os;
	case GLOBAL:
		os << "global " << _globals;
		return os;
	default:
		return os;
	}

}

}


