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

#ifndef ANNOTATIONS_ANNOTATIONBASE_H
#define ANNOTATIONS_ANNOTATIONBASE_H

#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/casa/Utilities/Regex.h>
#include <casacore/measures/Measures/Stokes.h>

#include <list>

namespace casa {

// <summary>Base class for annotations</summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd">
// </reviewed>

// <synopsis>
// Base class for annotations

// In order to minimize maintainability, many parameters are not
// set in the constructor but can be set by mutator methods.
//
// casacore::Input directions will be converted to the reference frame of the
// input coordinate system upon construction if necessary. The coordinate
// system specified in the constructor should be that associated with the
// image to which the region/annotation is being applied.
// </synopsis>

class AnnotationBase {
public:

	using RGB = std::vector<float>;
    
	// The pairs have longitude as the first member and latitude as the second
	using Direction = casacore::Vector<std::pair<casacore::Quantity,casacore::Quantity> >;

	enum Type {
		// annotations only
		LINE,
		VECTOR,
		TEXT,
		SYMBOL,
		// regions
		RECT_BOX,
		CENTER_BOX,
		ROTATED_BOX,
		POLYGON,
		POLYLINE,
		CIRCLE,
		ANNULUS,
		ELLIPSE
	};

	enum Keyword {
		COORD,
		RANGE,
		FRAME,
		CORR,
		VELTYPE,
		RESTFREQ,
		LINEWIDTH,
		LINESTYLE,
		SYMSIZE,
		SYMTHICK,
		COLOR,
		FONT,
		FONTSIZE,
		FONTSTYLE,
		USETEX,
		LABEL,
		LABELCOLOR,
		LABELPOS,
		LABELOFF,
		UNKNOWN_KEYWORD,
		N_KEYS
	};

	enum LineStyle {
		SOLID,
		DASHED,
		DOT_DASHED,
		DOTTED
	};

	enum FontStyle {
		NORMAL,
		BOLD,
		ITALIC,
		ITALIC_BOLD
	};

    static const RGB BLACK;
    static const RGB BLUE;
    static const RGB CYAN;
    static const RGB GRAY;
    static const RGB GREEN;
    static const RGB MAGENTA;
    static const RGB ORANGE;
    static const RGB RED;
    static const RGB WHITE;
    static const RGB YELLOW;

	static const casacore::String DEFAULT_LABEL;
	static const RGB DEFAULT_COLOR;
	static const LineStyle DEFAULT_LINESTYLE;
	static const casacore::uInt DEFAULT_LINEWIDTH;
	static const casacore::uInt DEFAULT_SYMBOLSIZE;
	static const casacore::uInt DEFAULT_SYMBOLTHICKNESS;
	static const casacore::String DEFAULT_FONT;
	static const casacore::uInt DEFAULT_FONTSIZE;
	static const FontStyle DEFAULT_FONTSTYLE;
	static const casacore::Bool DEFAULT_USETEX;
	static const RGB DEFAULT_LABELCOLOR;
	static const casacore::String DEFAULT_LABELPOS;
	static const std::vector<casacore::Int> DEFAULT_LABELOFF;

	static const casacore::Regex rgbHexRegex;

	virtual ~AnnotationBase();

	Type getType() const;

	static LineStyle lineStyleFromString(const casacore::String& ls);

	// Given a string, return the corresponding annotation type or throw
	// an error if the string does not correspond to an allowed type.
	static Type typeFromString(const casacore::String& type);

	static casacore::String typeToString(const Type type);

	static casacore::String keywordToString(const Keyword key);

	static casacore::String lineStyleToString(const LineStyle linestyle);

	static FontStyle fontStyleFromString(const casacore::String& fs);

	static casacore::String fontStyleToString(const FontStyle fs);

	void setLabel(const casacore::String& label);

	casacore::String getLabel() const;

    // <src>color</src> must either be a recognized color name or
    // a valid rgb hex string, else an expection is thrown
	void setColor(const casacore::String& color);
	
    // color must have three elements all with values between 0 and 255 inclusive
    // or an exception is thrown.
    void setColor(const RGB& color);

    // returns the color name if it is recognized or its rgb hex string 
	casacore::String getColorString() const;

	static casacore::String colorToString(const RGB& color);

    // get the color associated with this object
    RGB getColor() const;

	void setLineStyle(const LineStyle lineStyle);

	LineStyle getLineStyle() const;

	void setLineWidth(const casacore::uInt linewidth);

	casacore::uInt getLineWidth() const;

	void setSymbolSize(const casacore::uInt symbolsize);

	casacore::uInt getSymbolSize() const;

	void setSymbolThickness(const casacore::uInt symbolthickness);

	casacore::uInt getSymbolThickness() const;

	void setFont(const casacore::String& font);

	casacore::String getFont() const;

	void setFontSize(const casacore::uInt fontsize);

	casacore::uInt getFontSize() const;

	void setFontStyle(const FontStyle& fontstyle);

	FontStyle getFontStyle() const;

	void setUseTex(const casacore::Bool usetex);

	casacore::Bool isUseTex() const;

	// is the object a region?
	virtual casacore::Bool isRegion() const;

	// is the object only an annotation? Can only be false if the object
	// is a region
	inline virtual casacore::Bool isAnnotationOnly() const { return true; }

	// set "pix" as valid unit. This should be called externally
	// before creating quantities which have pixel units.
	static void unitInit();

    // <src>color</src> must either be a recognized color name or
    // a valid rgb hex string, else an expection is thrown
	void setLabelColor(const casacore::String& color);

    // color must have three elements all with values between 0 and 255 inclusive
    // or an exception is thrown.
    void setLabelColor(const RGB& color);

    // returns the color name if it is recognized or its rgb hex string

	casacore::String getLabelColorString() const;

    // get the color associated with this object's label
    RGB getLabelColor() const;

    // returns one of top, bottom, left, or right.
	casacore::String getLabelPosition() const;

	// <src>position</src> must have a value in top, bottom, left, or right.
	// case is ignored.
	void setLabelPosition(const casacore::String& position);

	// <src>offset</src> must have two elements
	void setLabelOffset(const std::vector<casacore::Int>& offset);

    std::vector<casacore::Int> getLabelOffset() const;

	virtual std::ostream& print(std::ostream &os) const = 0;

	// These parameters are included at the global scope. Multiple runs
	// on the same object are cumulative; if a key exists in the current
	// settings but not in <src>globalKeys</src> that key will still exist
	// in the globals after setGlobals has run.
	void setGlobals(const casacore::Vector<Keyword>& globalKeys);

	// print a set of keyword value pairs
	static std::ostream& print(
		std::ostream& os, const std::map<Keyword, casacore::String>& params
	);

	// print a line style representation
	static std::ostream& print(
		std::ostream& os, const LineStyle ls
	);

	// print a font style representation
	static std::ostream& print(
		std::ostream& os, const FontStyle fs
	);

	static std::ostream& print(
		std::ostream& os, const Direction d
	);

	// Get a list of the user-friendly color names supported
	static std::list<std::string> colorChoices();

	// get the coordinate system associated with this object.
	// This is the same coordinate system used to construct the object.
	inline const casacore::CoordinateSystem& getCsys() const {
		return _csys;
	}

	// DEPRECATED Please use getConvertedDirections()
	// the pair elements have longitude as the first member and latitude as the second.
	// FIXME make this return of vector of MVDirections
	// Returns the same angles as getConvertedDirections()
	Direction getDirections() const;

	// get the frequency limits converted to the spectral frame of the coordinate
	// system of this object. An empty casacore::Vector implies all applicable frequencies
	// have been selected.
	casacore::Vector<casacore::MFrequency> getFrequencyLimits() const;

	// Get the stokes for which the selection applies. An empty casacore::Vector implies
	// all applicable stokes have been selected.
	casacore::Vector<casacore::Stokes::StokesTypes> getStokes() const;

	// if freqRefFrame=="" -> use the reference frame of the coordinate system
	// if dopplerString=="" -> use the doppler system associated with the coordinate system
	// if restfreq=casacore::Quantity(0, "Hz") -> use the rest frequency associated with the coordinate system
	// Tacitly does nothing if the coordinate system has no spectral axis.
	// Returns true if frequencies actually need to be set and were set.
	virtual casacore::Bool setFrequencyLimits(
		const casacore::Quantity& beginFreq,
		const casacore::Quantity& endFreq,
		const casacore::String& freqRefFrame,
		const casacore::String& dopplerString,
		const casacore::Quantity& restfreq
	);

	// same as getDirections, only returns proper MDirections
	inline const casacore::Vector<casacore::MDirection>& getConvertedDirections() const {
		return _convertedDirections;
	}

protected:
    // <group>
	// if <src>freqRefFrame</src> or <src>dopplerString</src> are empty,
	// the values from the spectral coordinate of csys will be used, if one
	// exists. if restfreq=casacore::Quantity(0, "Hz") -> use the rest frequency
    // associated with the coordinate system.
    // The provided coordinate system should be that of the image to which
    // the region/annotation is being applied.
	AnnotationBase(
		const Type type, const casacore::String& dirRefFrameString,
		const casacore::CoordinateSystem& csys, const casacore::Quantity& beginFreq,
		const casacore::Quantity& endFreq,
		const casacore::String& freqRefFrame,
		const casacore::String& dopplerString,
		const casacore::Quantity& restfreq,
		const casacore::Vector<casacore::Stokes::StokesTypes>& stokes
	);

	// use only if the frame of the input directions is the
	// same as the frame of the coordinate system. All frequencies
	// are used.
	AnnotationBase(
		const Type type, const casacore::CoordinateSystem& csys,
		const casacore::Vector<casacore::Stokes::StokesTypes>& stokes
	);
    // <group>

	// assignment operator
	AnnotationBase& operator= (const AnnotationBase& other);

	static void _checkMixed(
		const casacore::String& origin,
		const Direction& dirs
	);

	casacore::MDirection _directionFromQuantities(
		const casacore::Quantity& q0, const casacore::Quantity& q1
	);

	void _checkAndConvertDirections(
		const casacore::String& origin,
		const Direction& dirs
	);

	virtual void _printPairs(std::ostream& os) const;

	inline const casacore::IPosition& _getDirectionAxes() const {
		return _directionAxes;
	}

	// direction to string, precision of 0.1 mas
	// ra and dec in sexigesimal format, non-equatorial coords in degrees
	casacore::String _printDirection(
		const casacore::Quantity& longitude, const casacore::Quantity& latitude
	) const;

	// convert angle to arcsec, precision 0.1 mas
	static casacore::String _toArcsec(const casacore::Quantity& angle);

	// convert angle to degrees, precision 0.1 mas
	static casacore::String _toDeg(const casacore::Quantity& angle);

	inline void _setParam(const Keyword k, const casacore::String& s) {
		_params[k] = s;
	}

	// return a string representing a pixel value, precision 1.
	static casacore::String _printPixel(const casacore::Double& d);

	casacore::MDirection::Types _getDirectionRefFrame() const { return _directionRefFrame; }

private:
	Type _type;
	casacore::MDirection::Types _directionRefFrame;
	casacore::CoordinateSystem _csys;
	casacore::IPosition _directionAxes;
	casacore::String _label, _font, _labelPos;
    RGB _color, _labelColor;
	FontStyle _fontstyle;
	LineStyle _linestyle;
	casacore::uInt _fontsize, _linewidth, _symbolsize,
		_symbolthickness;
	casacore::Bool _usetex;
	casacore::Vector<casacore::MDirection> _convertedDirections;
	casacore::Vector<casacore::MFrequency> _convertedFreqLimits;
	casacore::Quantity _beginFreq, _endFreq, _restFreq;
	casacore::Vector<casacore::Stokes::StokesTypes> _stokes;
	casacore::MFrequency::Types _freqRefFrame;
	casacore::MDoppler::Types _dopplerType;

	std::map<Keyword, casacore::Bool> _globals;
	std::map<Keyword, casacore::String> _params;
	casacore::Bool _printGlobals;
    std::vector<casacore::Int> _labelOff;

	static casacore::Bool _doneUnitInit, _doneColorInit;
	static std::map<casacore::String, LineStyle> _lineStyleMap;
	static std::map<casacore::String, Type> _typeMap;
	static std::map<string, RGB> _colors;
	static std::map<RGB, string> _rgbNameMap;
	static std::list<std::string> _colorNames;

	const static casacore::String _class;

	void _init();
	void _initParams();

	static void _initColors();

	static RGB _colorStringToRGB(const casacore::String& s);

	static casacore::Bool _isRGB(const RGB& rgb);

	void _testConvertToPixel() const;

	static void _initTypeMap();

	void _checkAndConvertFrequencies();

	casacore::String _printFreqRange() const;

	static casacore::String _printFreq(const casacore::Quantity& freq);

};

inline std::ostream &operator<<(std::ostream& os, const AnnotationBase& annotation) {
	return annotation.print(os);
};

inline std::ostream &operator<<(std::ostream& os, const AnnotationBase::LineStyle& ls) {
	return AnnotationBase::print(os, ls);
};

inline std::ostream &operator<<(std::ostream& os, const AnnotationBase::FontStyle& fs) {
	return AnnotationBase::print(os, fs);
};

inline std::ostream &operator<<(std::ostream& os, const std::map<AnnotationBase::Keyword, casacore::String>& x) {
	return AnnotationBase::print(os, x);
};

inline std::ostream &operator<<(std::ostream& os, const AnnotationBase::Direction x) {
	return AnnotationBase::print(os, x);
};

// Just need a identifiable exception class for exception handling.
class WorldToPixelConversionError : public casacore::AipsError {
public:
	WorldToPixelConversionError(casacore::String msg) : casacore::AipsError(msg) {}
};

}

#endif
