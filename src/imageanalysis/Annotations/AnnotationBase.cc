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

#include <imageanalysis/Annotations/AnnotationBase.h>

#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/Quanta/MVAngle.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>
#include <casacore/measures/Measures/MCDirection.h>
#include <casacore/measures/Measures/VelocityMachine.h>
#include <casacore/casa/Quanta/UnitMap.h>

#include <iomanip>

using namespace std;

using namespace casacore;
namespace casa {

const AnnotationBase::RGB AnnotationBase::BLACK(3, 0.0);
const AnnotationBase::RGB AnnotationBase::BLUE{0.0,0.0,255.0};
const AnnotationBase::RGB AnnotationBase::CYAN{255.0,255.0,0.0};
const AnnotationBase::RGB AnnotationBase::GRAY{190.0,190.0,190.0};
const AnnotationBase::RGB AnnotationBase::GREEN{0.0,255.0,0.0};
const AnnotationBase::RGB AnnotationBase::MAGENTA{255.0,0.0,255.0};
const AnnotationBase::RGB AnnotationBase::ORANGE{255.0,165.0,0.0};
const AnnotationBase::RGB AnnotationBase::RED{255.0,0.0,0.0};
const AnnotationBase::RGB AnnotationBase::WHITE(3, 255.0);
const AnnotationBase::RGB AnnotationBase::YELLOW{255.0,255.0,0.0};

const String AnnotationBase::DEFAULT_LABEL = "";
const AnnotationBase::RGB AnnotationBase::DEFAULT_COLOR = AnnotationBase::GREEN;
const AnnotationBase::LineStyle AnnotationBase::DEFAULT_LINESTYLE = AnnotationBase::SOLID;
const uInt AnnotationBase::DEFAULT_LINEWIDTH = 1;
const uInt AnnotationBase::DEFAULT_SYMBOLSIZE = 1;
const uInt AnnotationBase::DEFAULT_SYMBOLTHICKNESS = 1;
const String AnnotationBase::DEFAULT_FONT = "Helvetica";
const uInt AnnotationBase::DEFAULT_FONTSIZE = 10;
const AnnotationBase::FontStyle AnnotationBase::DEFAULT_FONTSTYLE = AnnotationBase::BOLD;
const Bool AnnotationBase::DEFAULT_USETEX = false;
const AnnotationBase::RGB AnnotationBase::DEFAULT_LABELCOLOR = AnnotationBase::GREEN;
const String AnnotationBase::DEFAULT_LABELPOS = "top";
const vector<Int> AnnotationBase::DEFAULT_LABELOFF = vector<Int>(2, 0);

const String AnnotationBase::_class = "AnnotationBase";

std::list<string> AnnotationBase::_colorNames;

Bool AnnotationBase::_doneUnitInit = false;
Bool AnnotationBase::_doneColorInit = false;

map<String, AnnotationBase::Type> AnnotationBase::_typeMap;
map<String, AnnotationBase::LineStyle> AnnotationBase::_lineStyleMap;

map<string, AnnotationBase::RGB> AnnotationBase::_colors;
map<AnnotationBase::RGB, string> AnnotationBase::_rgbNameMap;

const Regex AnnotationBase::rgbHexRegex("([0-9]|[a-f]){6}");


AnnotationBase::AnnotationBase(
	const Type type, const String& dirRefFrameString,
	const CoordinateSystem& csys, const Quantity& beginFreq,
	const Quantity& endFreq,
	const String& freqRefFrame,
	const String& dopplerString,
	const Quantity& restfreq,
	const Vector<Stokes::StokesTypes>& stokes
)
: _type(type), _csys(csys), _label(DEFAULT_LABEL),_font(DEFAULT_FONT),
  _labelPos(DEFAULT_LABELPOS), _color(DEFAULT_COLOR), _labelColor(DEFAULT_LABELCOLOR),
  _fontstyle(DEFAULT_FONTSTYLE), _linestyle(DEFAULT_LINESTYLE), _fontsize(DEFAULT_FONTSIZE),
  _linewidth(DEFAULT_LINEWIDTH), _symbolsize(DEFAULT_SYMBOLSIZE),
  _symbolthickness(DEFAULT_SYMBOLTHICKNESS), _usetex(DEFAULT_USETEX),

  _convertedFreqLimits(0), _stokes(stokes),
  _globals(map<Keyword, Bool>()), _params(map<Keyword, String>()),
  _printGlobals(false), _labelOff(DEFAULT_LABELOFF) {
	ThrowIf(
		! csys.hasDirectionCoordinate(),
		"Coordinate system has no direction coordinate"
	);
	const uInt *oname;
	Int nall, nex;
	const auto *tname = MDirection::allMyTypes(nall, nex, oname);
	// Because MDirection::getType() only does minimal match, bogus strings
	// can tacitly be let through, so we do a more rigorous check here.
	ThrowIf(
		find( tname, tname+nall, dirRefFrameString) == tname+nall,
		"Unknown direction reference frame '" + dirRefFrameString + "'"
	);
	ThrowIf (
		! MDirection::getType(_directionRefFrame, dirRefFrameString),
		"Unknown direction reference frame " + dirRefFrameString
	);
	setFrequencyLimits(
		beginFreq, endFreq, freqRefFrame,
		dopplerString, restfreq
	);
	_init();
}

AnnotationBase::AnnotationBase(
	const Type type, const CoordinateSystem& csys,
	const Vector<Stokes::StokesTypes>& stokes
)
: _type(type), _csys(csys), _label(DEFAULT_LABEL),
  _font(DEFAULT_FONT), _labelPos(DEFAULT_LABELPOS),
  _color(DEFAULT_COLOR), _labelColor(DEFAULT_LABELCOLOR),
  _fontstyle(DEFAULT_FONTSTYLE), _linestyle(DEFAULT_LINESTYLE),
  _fontsize(DEFAULT_FONTSIZE),
  _linewidth(DEFAULT_LINEWIDTH), _symbolsize(DEFAULT_SYMBOLSIZE),
  _symbolthickness(DEFAULT_SYMBOLTHICKNESS), _usetex(DEFAULT_USETEX),
  _convertedFreqLimits(0), _beginFreq(Quantity(0, "Hz")), _endFreq(Quantity(0, "Hz")),
	_restFreq(Quantity(0, "Hz")), _stokes(stokes),
  _globals(map<Keyword, Bool>()), _params(map<Keyword, String>()),
  _printGlobals(false), _labelOff(DEFAULT_LABELOFF)
 {
	String preamble = String(__FUNCTION__) + ": ";
	if (!csys.hasDirectionCoordinate()) {
		throw AipsError(
			preamble + "Coordinate system has no direction coordinate"
		);
	}
	_directionRefFrame = _csys.directionCoordinate().directionType();
	_init();
}

AnnotationBase::~AnnotationBase() {}

AnnotationBase& AnnotationBase::operator= (
	const AnnotationBase& other
) {
    if (this == &other) {
    	return *this;
    }
    _type = other._type;
    _directionRefFrame = other._directionRefFrame;
    _csys = other._csys;
    _directionAxes.resize(other._directionAxes.nelements());
    _directionAxes = other._directionAxes;
    _convertedFreqLimits.assign(other._convertedFreqLimits);
    _beginFreq = other._beginFreq;
    _endFreq = other._endFreq;
    _restFreq = other._restFreq;
    _stokes.assign(other._stokes);
    _freqRefFrame = other._freqRefFrame;
    _dopplerType = other._dopplerType;
    _label = other._label;
    _color = other._color;
    _font = other._font;
    _fontsize = other._fontsize;
    _fontstyle = other._fontstyle;
    _linestyle = other._linestyle;
    _linewidth = other._linewidth;
    _symbolsize = other._symbolsize;
    _symbolthickness = other._symbolthickness;
    _usetex = other._usetex;
    _convertedDirections.assign(other._convertedDirections);
    _globals = other._globals;
    _params = other._params;
    _printGlobals = other._printGlobals;
    return *this;
}

void AnnotationBase::_init() {
	String preamble = _class + ": " + String(__FUNCTION__) + ": ";
	_initColors();
	if (
		_directionRefFrame != _csys.directionCoordinate().directionType(false)
		&& _directionRefFrame != MDirection::B1950
		&& _directionRefFrame != MDirection::B1950_VLA
		&& _directionRefFrame != MDirection::BMEAN
		&& _directionRefFrame != MDirection::DEFAULT
		&& _directionRefFrame != MDirection::ECLIPTIC
		&& _directionRefFrame != MDirection::GALACTIC
		&& _directionRefFrame != MDirection::J2000
		&& _directionRefFrame != MDirection::SUPERGAL
		&& _directionRefFrame != MDirection::ICRS
	) {
		throw AipsError(preamble
			+ "Unsupported coordinate frame for regions "
			+ MDirection::showType(_directionRefFrame)
		);
	}
	_params[COORD] = MDirection::showType(_directionRefFrame);
	_directionAxes = IPosition(_csys.directionAxesNumbers());

	uInt nStokes = _stokes.size();
	if (nStokes > 0) {
		ostringstream os;
		os << "[";
		for (uInt i=0; i< nStokes; i++) {
			os << Stokes::name(_stokes[i]);
			if (i != _stokes.size() - 1) {
				os << ", ";
			}
		}
		os << "]";
		_params[CORR] = os.str();
	}
	for(uInt i=0; i<N_KEYS; i++) {
		_globals[(Keyword)i] = false;
	}
	_initParams();
}

void AnnotationBase::_initParams() {
	_params[LINEWIDTH] = String::toString(_linewidth);
	_params[LINESTYLE] = lineStyleToString(_linestyle);
	_params[SYMSIZE] = String::toString(_symbolsize);
	_params[SYMTHICK] = String::toString(_symbolthickness);
	_params[COLOR] = getColorString();
	_params[FONT] = _font;
	_params[FONTSIZE] = String::toString(_fontsize);
	_params[FONTSTYLE] = fontStyleToString(_fontstyle);
	_params[USETEX] = _usetex ? "true" : "false";
	if (! _label.empty()) {
		_params[LABEL] = _label;
	}
}

void AnnotationBase::unitInit() {
	if (! _doneUnitInit) {
		UnitMap::putUser("pix",UnitVal(1.0), "pixel units");
		UnitMap::putUser("channel",UnitVal(1.0), "channel number");
        UnitMap::putUser("chan",UnitVal(1.0), "channel number");
		_doneUnitInit = true;
	}
}


Bool AnnotationBase::setFrequencyLimits(
	const Quantity& beginFreq,
	const Quantity& endFreq,
	const String& freqRefFrame,
	const String& dopplerString,
	const Quantity& restfreq
) {
	String preamble(_class + ": " + String(__FUNCTION__) + ": ");
    if (beginFreq.getValue() == 0 && endFreq.getValue() == 0) {
        return false;
    }
	if (! getCsys().hasSpectralAxis()) {
		return false;
	}
    if ( beginFreq.getUnit().empty() && endFreq.getUnit().empty()) {
        throw AipsError(
            preamble + "Neither frequency specified has units. Both must"
        );
    }
	if (! beginFreq.getUnit().empty() && endFreq.getUnit().empty()) {
		throw AipsError(
			preamble + "beginning frequency specified but ending frequency not. "
			+ "Both must specified or both must be unspecified."
		);
	}
	if (beginFreq.getUnit().empty() && ! endFreq.getUnit().empty()) {
		throw AipsError(
			preamble + "ending frequency specified but beginning frequency not. "
			+ "Both must specified or both must be unspecified."
		);
	}
	if (! beginFreq.getUnit().empty()) {
		if (! beginFreq.isConform(endFreq)) {
			throw AipsError(
				preamble + "Beginning freq units (" + beginFreq.getUnit()
				+ ") do not conform to ending freq units (" + endFreq.getUnit()
				+ ") but they must."
			);
		}

		if (
			! beginFreq.isConform("Hz")
			&& ! beginFreq.isConform("m/s")
			&& ! beginFreq.isConform("pix")
		) {
			throw AipsError(
				preamble
				+ "Invalid frequency unit " + beginFreq.getUnit()
			);
		}
		if (beginFreq.isConform("m/s") && restfreq.getValue() <= 0) {
			throw AipsError(
				preamble
				+ "Beginning and ending velocities supplied but no restfreq specified"
			);
		}
		if (freqRefFrame.empty()) {
			_freqRefFrame = getCsys().spectralCoordinate().frequencySystem();
		}
		else if (! MFrequency::getType(_freqRefFrame, freqRefFrame)) {
			throw AipsError(
				preamble
				+ "Unknown frequency frame code "
				+ freqRefFrame
			);
		}
		else {
			_setParam(AnnotationBase::FRAME, freqRefFrame);
		}
		if (dopplerString.empty()) {
			_dopplerType = getCsys().spectralCoordinate().velocityDoppler();
		}
		else if (! MDoppler::getType(_dopplerType, dopplerString)) {
			throw AipsError(
				preamble + "Unknown doppler code " + dopplerString
			);
		}
		else {
			_setParam(AnnotationBase::VELTYPE, dopplerString);
		}
		_beginFreq = beginFreq;
		_endFreq = endFreq;
		_restFreq = restfreq;
		_setParam(AnnotationBase::RANGE, _printFreqRange());
		_setParam(AnnotationBase::RESTFREQ, _printFreq(_restFreq));

		_checkAndConvertFrequencies();
		return true;
	}
	return false;
}


Vector<MFrequency> AnnotationBase::getFrequencyLimits() const {
	return _convertedFreqLimits;
}

Vector<Stokes::StokesTypes> AnnotationBase::getStokes() const {
	return _stokes;
}

void AnnotationBase::_checkAndConvertFrequencies() {
	const CoordinateSystem& csys = getCsys();
	const SpectralCoordinate spcoord = csys.spectralCoordinate();
    MFrequency::Types cFrameType = spcoord.frequencySystem(false);
	MDoppler::Types cDopplerType = spcoord.velocityDoppler();
	_convertedFreqLimits.resize(2);
    for (Int i=0; i<2; i++) {
		Quantity qFreq = i == 0 ? _beginFreq : _endFreq;
		String unit = qFreq.getUnit();
		if (qFreq.isConform("pix")) {
			Int spectralAxisNumber = csys.spectralAxisNumber(true);
			String unit = csys.worldAxisUnits()[spectralAxisNumber];
			Double world;
			if (! spcoord.toWorld(world, qFreq.getValue())) {
				ostringstream os;
				os << String(__FUNCTION__) << ": Unable to convert pixel to world value "
					<< "for spectral coordinate";
				throw AipsError(os.str());
			}
			if (_freqRefFrame != cFrameType) {
				LogIO log;
				log << LogOrigin(String(__FUNCTION__)) << LogIO::WARN
					<< ": Frequency range given in pixels but supplied frequency ref frame ("
					<< MFrequency::showType(_freqRefFrame) << ") differs from that of "
					<< "the provided coordinate system (" << MFrequency::showType(cFrameType)
					<< "). The provided frequency range will therefore be assumed to already "
					<< "be in the coordinate system frequency reference frame and no conversion "
					<< "will be done" << LogIO::POST;
			}
			if (_dopplerType != cDopplerType) {
				LogIO log;
				log << LogOrigin(String(__FUNCTION__)) << LogIO::WARN
					<< ": Frequency range given in pixels but supplied doppler type ("
					<< MDoppler::showType(_dopplerType) << ") differs from that of "
					<< "the provided coordinate system (" << MDoppler::showType(cDopplerType)
					<< "). The provided frequency range will therefore be assumed to already "
					<< "be in the coordinate system doppler and no conversion "
					<< "will be done" << LogIO::POST;
			}
			_freqRefFrame = cFrameType;
			_dopplerType = cDopplerType;
			_convertedFreqLimits[i] = MFrequency(
				Quantity(world, unit),
				_freqRefFrame
			);
		}
		else if (qFreq.isConform("m/s")) {
			MFrequency::Ref freqRef(_freqRefFrame);
			MDoppler::Ref velRef(_dopplerType);
			VelocityMachine vm(freqRef, Unit("GHz"),
				MVFrequency(_restFreq),
				velRef, unit
			);
			qFreq = vm(qFreq);
			_convertedFreqLimits[i] = MFrequency(qFreq, _freqRefFrame);
			if (_dopplerType != cDopplerType) {
				MDoppler dopplerConversion = MDoppler::Convert(_dopplerType, cDopplerType)();
				_convertedFreqLimits[i] = MFrequency::fromDoppler(
					dopplerConversion,
					_convertedFreqLimits[i].get("Hz"), cFrameType
				);
			}
		}
		else if ( qFreq.isConform("Hz")) {
			_convertedFreqLimits[i] = MFrequency(qFreq, _freqRefFrame);
		}
		else {
			throw AipsError("Logic error. Bad spectral unit "
				+ unit
				+ " somehow made it to a place where it shouldn't have"
			);
		}
		if (_freqRefFrame != cFrameType) {
			Vector<Double> refDirection = csys.directionCoordinate().referenceValue();
			Vector<String> directionUnits = csys.directionCoordinate().worldAxisUnits();
			MDirection refDir(
				Quantity(refDirection[0], directionUnits[0]),
				Quantity(refDirection[1], directionUnits[1]),
				getCsys().directionCoordinate().directionType()
			);
			MFrequency::Ref inFrame(_freqRefFrame, MeasFrame(refDir));
			MFrequency::Ref outFrame(cFrameType, MeasFrame(refDir));
			MFrequency::Convert converter(inFrame, outFrame);
			_convertedFreqLimits[i] = converter(_convertedFreqLimits[i]);
		}
	}
}


String AnnotationBase::_printFreqRange() const {
	ostringstream os;
	os << "["
		<< _printFreq(_beginFreq) << ", "
		<< _printFreq(_endFreq) << "]";
	return os.str();
}

String AnnotationBase::_printFreq(const Quantity& freq) {
	if (freq.isConform("pix")) {
		return _printPixel(freq.getValue());
	}
	ostringstream os;
	os << std::fixed;
	if (freq.isConform("km/s")) {
		os << std::setprecision(4) << freq.getValue("km/s") << "km/s";
	}
	else {
		os << std::setprecision(3) << freq.getValue("MHz") << "MHz";
	}
	return os.str();
}

AnnotationBase::Type AnnotationBase::getType() const {
	return _type;
}

void AnnotationBase::_initTypeMap() {
	_typeMap["line"] = LINE;
	_typeMap["vector"] = VECTOR;
	_typeMap["text"] = TEXT;
	_typeMap["symbol"] = SYMBOL;
	_typeMap["box"] = RECT_BOX;
	_typeMap["rectangularbox"] = RECT_BOX;
	_typeMap["centerbox"] = CENTER_BOX;
	_typeMap["rotatedbox"] = ROTATED_BOX;
	_typeMap["rotbox"] = ROTATED_BOX;
	_typeMap["poly"] = POLYGON;
	_typeMap["polygon"] = POLYGON;
	_typeMap["circle"] = CIRCLE;
	_typeMap["annulus"] = ANNULUS;
	_typeMap["ellipse"] = ELLIPSE;
}

AnnotationBase::Type AnnotationBase::typeFromString(
	const String& type
) {
	if (_typeMap.size() == 0) {
		_initTypeMap();
	}
	String cType = type;
	cType.downcase();
	cType.trim();
	if (_typeMap.find(cType) == _typeMap.end()) {
		throw AipsError(type + " is not a supported annotation type");
	}
	return _typeMap.at(cType);
}

String AnnotationBase::typeToString(const AnnotationBase::Type type) {
	if (_typeMap.size() == 0) {
		_initTypeMap();
	}
	for (
		map<String, Type>::const_iterator iter = _typeMap.begin();
		iter != _typeMap.end(); iter++
	) {
		if (iter->second == type) {
			return iter->first;
		}

	}
	throw AipsError(
		_class + "::" + __FUNCTION__ + ": Logic error. Type "
		+ String::toString(type) + " not handled"
	);
}


AnnotationBase::LineStyle AnnotationBase::lineStyleFromString(const String& ls) {
	if (_lineStyleMap.size() == 0) {
		_lineStyleMap["-"] = SOLID;
		_lineStyleMap["--"] = DASHED;
		_lineStyleMap["-."] = DOT_DASHED;
		_lineStyleMap[":"] = DOTTED;
	}
	String cls = ls;
	cls.trim();
	if (cls.empty()) {
		return DEFAULT_LINESTYLE;
	}
	if (_lineStyleMap.find(cls) == _lineStyleMap.end()) {
		throw AipsError(
			ls + " is not a supported line style"
		);
	}
	return _lineStyleMap.at(cls);
}

AnnotationBase::FontStyle
AnnotationBase::fontStyleFromString(const String& fs) {
	String cfs = fs;
	cfs.downcase();
	cfs.trim();
	// FIXME when nothing to do and feeling anal, turn this into
	// a static map
	if (cfs.empty()) {
		return DEFAULT_FONTSTYLE;
	}
	else if (cfs == "normal") {
		return NORMAL;
	}
	else if (cfs == "bold") {
		return BOLD;
	}
	else if (cfs == "italic") {
		return ITALIC;
	}
	else if (cfs == "bold-italic") {
		return ITALIC_BOLD;
	}
	else {
		throw AipsError(
			fs + " is not a supported font style"
		);
	}
}

String AnnotationBase::fontStyleToString(
	const AnnotationBase::FontStyle fs
) {
	switch (fs) {
	case NORMAL: return "normal";
	case BOLD: return "bold";
	case ITALIC: return "italic";
	case ITALIC_BOLD: return "itatlic_bold";
	default:
		throw AipsError(
			_class + ": " + String(__FUNCTION__) + ": "
			+ ": Logic error, should never have gotten here"
		);
	}
}

void AnnotationBase::setLabel(const String& s) {
	_label = s;
	if (_label.empty()) {
		if (_params.find(LABEL) != _params.end()) {
			_params.erase(LABEL);
		}
	}
	else {
		_params[LABEL] = _label;
	}
}

String AnnotationBase::getLabel() const {
	return _label;
}

Bool AnnotationBase::_isRGB(const AnnotationBase::RGB& rgb) {
	if (rgb.size() != 3) {
		return false;
	}
	for (RGB::const_iterator iter=rgb.begin(); iter!=rgb.end(); iter++) {
		if (*iter < 0 || *iter > 255) {
			return false;
		}
	}
	return true;
}

AnnotationBase::RGB AnnotationBase::_colorStringToRGB(const String& s) {
    String c = s;
    c.trim();
    c.downcase();
    if (_colors.find(c) != _colors.end()) {
    	return _colors.find(c)->second;
    }
    else if (c.find(rgbHexRegex) != String::npos) {
    	RGB rgb(3);
    	for (uInt i=0; i<3; i++) {
    		String comp = s.substr(2*i, 2);
    		int hexInt;
    		sscanf(comp.c_str(), "%x", &hexInt );
    		rgb[i] = hexInt;
    	}
    	return rgb;
    }
    else {
        throw AipsError("Unrecognized color specification " + s);
    }
}

void AnnotationBase::setColor(const String& s) {
	_color = _colorStringToRGB(s);
	_params[COLOR] = colorToString(_color);
}

void AnnotationBase::setColor(const RGB& rgb) {
	if (! _isRGB(rgb)) {
		throw AipsError(
			_class + "::" + __FUNCTION__
				+ ": input vector is not a valid RGB representation"
		);
	}
	_color = rgb;
	_params[COLOR] = colorToString(_color);
}

AnnotationBase::RGB AnnotationBase::getColor() const {
	return _color;
}

String AnnotationBase::getColorString() const {
	return colorToString(_color);
}

String AnnotationBase::colorToString(const AnnotationBase::RGB& color) {
	if (! _isRGB(color)) {
		throw AipsError(
			_class + "::" + __FUNCTION__
				+ ": input vector is not a valid RGB representation"
		);
	}
	if (_rgbNameMap.find(color) != _rgbNameMap.end()) {
		return _rgbNameMap.find(color)->second;
	}
	else {
		ostringstream oss;
		oss << hex << std::setw(2) << std::setfill('0') << (Int)floor(color[0] + 0.5)
			<< hex << std::setw(2) << std::setfill('0') << (Int)floor(color[1] + 0.5)
			<< hex << std::setw(2) << std::setfill('0') << (Int)floor(color[2] + 0.5);
		String rgbString = oss.str();
		rgbString.downcase();
		return rgbString;
	}
}

void AnnotationBase::setLabelColor(const String& color) {
	_labelColor = _colorStringToRGB(color);
	_params[LABELCOLOR] = color;
}

void AnnotationBase::setLabelColor(const RGB& color) {
	if (! _isRGB(color)) {
		throw AipsError(
			_class + "::" + __FUNCTION__
				+ ": input vector is not a valid RGB representation"
		);
	}
	_labelColor = color;
	_params[LABELCOLOR] = colorToString(_labelColor);
}

String AnnotationBase::getLabelColorString() const {
	return colorToString(_labelColor);
}

AnnotationBase::RGB AnnotationBase::getLabelColor() const {
	return _labelColor;
}

void AnnotationBase::setLineStyle(const LineStyle s) {
	_linestyle = s;
	_params[LINESTYLE] = lineStyleToString(_linestyle);
}

AnnotationBase::LineStyle AnnotationBase::getLineStyle() const {
	return _linestyle;
}

void AnnotationBase::setLineWidth(const uInt s) {
	_linewidth = s;
	_params[LINEWIDTH] = String::toString(_linewidth);
}

uInt AnnotationBase::getLineWidth() const {
	return _linewidth;
}

void AnnotationBase::setSymbolSize(const uInt s) {
	_symbolsize = s;
	_params[SYMSIZE] = String::toString(_symbolsize);
}

uInt AnnotationBase::getSymbolSize() const {
	return _symbolsize;
}

void AnnotationBase::setSymbolThickness(const uInt s) {
	_symbolthickness = s;
	_params[SYMTHICK] = String::toString(_symbolthickness);
}

uInt AnnotationBase::getSymbolThickness() const {
	return _symbolthickness;
}

void AnnotationBase::setFont(const String& s) {
	_font = s;
	_params[FONT] = _font;
}

String AnnotationBase::getFont() const {
	return _font;
}

void AnnotationBase::setFontSize(const uInt s) {
	_fontsize = s;
	_params[FONTSIZE] = String::toString(_fontsize);
}

uInt AnnotationBase::getFontSize() const {
	return _fontsize;
}

void AnnotationBase::setFontStyle(const AnnotationBase::FontStyle& fs) {
	_fontstyle = fs;
	_params[FONTSTYLE] = fontStyleToString(_fontstyle);
}

AnnotationBase::FontStyle AnnotationBase::getFontStyle() const {
	return _fontstyle;
}

void AnnotationBase::setUseTex(const Bool s) {
	_usetex = s;
	_params[USETEX] = _usetex ? "true" : "false";
}

Bool AnnotationBase::isUseTex() const {
	return _usetex;
}

String AnnotationBase::getLabelPosition() const {
	return _labelPos;
}

void AnnotationBase::setLabelPosition(const String& position) {
	String c = position;
	c.trim();
	c.downcase();
	if (
		c != "top" && c != "bottom"
		&& c != "left" && c != "right"
	) {
		throw AipsError(
			_class + "::" + __FUNCTION__
				+ ": Unknown label position " + position
		);
	}
	_labelPos = c;
	_params[LABELPOS] = _labelPos;
}

void AnnotationBase::setLabelOffset(const vector<Int>& offset) {
	if (offset.size() != 2) {
		throw AipsError(
			_class + "::" + __FUNCTION__
				+ ": Number of elements in label offset must be exactly 2, not "
				+ String(offset.size())
		);
	}
	_labelOff = offset;
	_params[LABELOFF] = "[" + String::toString(offset[0]) + ", " + String::toString(offset[1]) + "]";
}

vector<Int> AnnotationBase::getLabelOffset() const {
	return _labelOff;
}

Bool AnnotationBase::isRegion() const {
	return false;
}

void AnnotationBase::setGlobals(
	const Vector<Keyword>& globalKeys
) {
	for (
		Vector<Keyword>::const_iterator iter=globalKeys.begin();
		iter != globalKeys.end(); iter++) {
			_globals[*iter] = true;
	}
}

String AnnotationBase::keywordToString(
	const Keyword key
) {
	switch(key) {
	case COORD: return "coord";
	case RANGE: return "range";
	case FRAME: return "frame";
	case CORR: return "corr";
	case VELTYPE: return "veltype";
	case RESTFREQ: return "restfreq";
	case LINEWIDTH: return "linewidth";
	case LINESTYLE: return "linestyle";
	case SYMSIZE: return "symsize";
	case SYMTHICK: return "symthick";
	case COLOR: return "color";
	case FONT: return "font";
	case FONTSIZE: return "fontsize";
	case FONTSTYLE: return "fontstyle";
	case USETEX: return "usetex";
	case LABEL: return "label";
	case LABELCOLOR: return "labelcolor";
	case LABELPOS: return "labelpos";
	case LABELOFF: return "labeloff";
	case UNKNOWN_KEYWORD:
	case N_KEYS:
	default:
		throw AipsError(
			_class + "::" + __FUNCTION__
			+ ": Logic error: No string representation for Keyword " + String(key)
		);
	}
}

String AnnotationBase::lineStyleToString(
	const LineStyle style
) {
	switch(style) {
	case SOLID: return "-";
	case DASHED: return "--";
	case DOT_DASHED: return "-.";
	case DOTTED: return ":";
	default:
		ThrowCc(
			"Logic error: No string representation for LineStyle "
			+ String::toString(style)
		);
	}
}

ostream& AnnotationBase::print(
	ostream& os, const LineStyle ls
) {
	os << lineStyleToString(ls);
	return os;
}

ostream& AnnotationBase::print(
	ostream& os, const FontStyle fs
) {
	os << fontStyleToString(fs);
	return os;
}

ostream& AnnotationBase::print(
	ostream& os, const map<Keyword, String>& params
) {
	if (params.size() == 0) {
		return os;
	}
	Bool hasLabel = params.find(LABEL) != params.end();
	for (
		map<Keyword, String>::const_iterator iter=params.begin();
		iter!=params.end(); iter++
	) {
		Keyword key = iter->first;
		if (! iter->second.empty()) {
			if (
				! hasLabel && (
					key == LABELCOLOR || key == LABELPOS
					|| key == LABELOFF
				)
			) {
				continue;
			}
			if (iter != params.begin()) {
				os << ", ";
			}
			String quote = key == LABEL
				|| (
					iter->second.contains(' ')
					&& (key != RANGE && key != CORR && key != LABELOFF)
				)
				? "\"" : "";
			os << keywordToString((Keyword)iter->first)
				<< "=" << quote << iter->second << quote;
		}
	}
	return os;
}

ostream& AnnotationBase::print(
	ostream& os, const Direction d
) {
	for (uInt i=0; i<d.size(); i++) {
		os << i << ": " << d[i].first << ", " << d[i].second << endl;
	}
	return os;
}

void AnnotationBase::_printPairs(ostream &os) const {
	map<Keyword, String> x = _params;
	if (! _printGlobals) {
		for (
			map<Keyword, String>::const_iterator iter = _params.begin();
			iter != _params.end(); iter++
		) {
			Keyword k = iter->first;
			if (_globals.find(k) != _globals.end() && _globals.at(k)) {
				x.erase(k);
			}
		}
	}
	if (x.size() > 0) {
		os << " " << x;
	}
}

void AnnotationBase::_checkMixed(
	const String& origin, const AnnotationBase::Direction& quantities
) {
	Bool isWorld = false;
	Bool isPixel = false;
	Quantity qArg;
	for (
		Direction::const_iterator iter = quantities.begin();
		iter != quantities.end(); iter++
	) {
		for (uInt i=0; i<2; i++) {
			Quantity tQ = i == 0 ? iter->first : iter->second;
			Bool pix = tQ.getUnit() == "pix";
			Bool world = ! pix;
			isWorld = isWorld || world;
			isPixel = isPixel || pix;
			if (isPixel && isWorld) {
				throw AipsError(
					origin
					+ ": Mixed world and pixel coordinates not supported"
				);
			}
		}
	}
}

MDirection AnnotationBase::_directionFromQuantities(
	const Quantity& q0, const Quantity& q1
) {
	ostringstream oss;
	oss << q0 << ", " << q1;
	Quantity d0 = q0;
	Quantity d1 = q1;

	String value = oss.str();
	if (q0.getUnit() == "pix") {
		// both quantities are in pix, this check should
		// have been done prior to calling this method
		Vector<Double> pixel(_csys.nPixelAxes(), 0);
		pixel[_directionAxes[0]] = q0.getValue();
		pixel[_directionAxes[1]] = q1.getValue();
		Vector<Double> world;
		_csys.toWorld(world, pixel);
		Vector<String> axesUnits = _csys.worldAxisUnits();
		d0 = Quantity(world[_directionAxes[0]], axesUnits[_directionAxes[0]]);
		d1 = Quantity(world[_directionAxes[1]], axesUnits[_directionAxes[1]]);
		MDirection::Types csysDirectionType = _csys.directionCoordinate().directionType(false);
		if (_directionRefFrame != csysDirectionType) {
			LogIO log;
			log << LogOrigin(String(__FUNCTION__)) << LogIO::WARN
				<< ": Direction quantities specified in pixels but specified direction reference "
				<< "frame (" << MDirection::showType(_directionRefFrame) << ") is different from "
				<< "the reference frame (" << MDirection::showType(csysDirectionType)
				<< ") of the coordinate system. The reference frame of the coordinate system "
				<< "will be used and the direction coordinates will not be transformed"
				<< LogIO::POST;
		}
		_directionRefFrame = csysDirectionType;
	}
	try {
		return MDirection(d0, d1, _directionRefFrame);
	}
	catch (const AipsError& x) {
		throw AipsError(
			_class + "::" + String(__FUNCTION__) + ": Error converting direction ("
			+ value + ") to MDirection: " + x.getMesg()
		);
	}
}

void AnnotationBase::_checkAndConvertDirections(
	const String& origin, const AnnotationBase::Direction& quantities
) {
	_checkMixed(origin, quantities);
	MDirection::Types csysDirectionRefFrame = _csys.directionCoordinate().directionType(false);
	Bool needsConverting = _directionRefFrame != csysDirectionRefFrame;
	_convertedDirections.resize(quantities.size());
	for (uInt i=0; i<quantities.size(); i++) {
		_convertedDirections[i] = _directionFromQuantities(quantities(i).first, quantities(i).second);
		if (needsConverting) {
			_convertedDirections[i] = MDirection::Convert(_convertedDirections[i], csysDirectionRefFrame)();
		}
	}
	// check this now because if converting from world to pixel fails when
	// regions are being formed, it will wreak havoc
	_testConvertToPixel();
}

AnnotationBase::Direction AnnotationBase::getDirections() const {
	Direction res(_convertedDirections.size());
	for (uInt i=0; i<res.size(); i++) {
		Quantum<Vector<Double> > angles = _convertedDirections[i].getAngle();
		String unit = angles.getUnit();
		Vector<Double> vals = angles.getValue();
		res[i].first = Quantity(vals[0], unit);
		res[i].second = Quantity(vals[1], unit);

	}
	return res;
}

void AnnotationBase::_initColors() {
	if (_doneColorInit) {
		return;
	}
	_colors.insert(make_pair("black", BLACK));
	_colors.insert(make_pair("blue", BLUE));
	_colors.insert(make_pair("cyan", CYAN));
	_colors.insert(make_pair("gray", GRAY));
	_colors.insert(make_pair("green", GREEN));
	_colors.insert(make_pair("magenta", MAGENTA));
	_colors.insert(make_pair("orange", ORANGE));
	_colors.insert(make_pair("red", RED));
	_colors.insert(make_pair("white", WHITE));
	_colors.insert(make_pair("yellow", YELLOW));

	for (
		map<string, RGB>::const_iterator iter=_colors.begin();
			iter != _colors.end(); iter++
		) {
		_rgbNameMap[iter->second] = iter->first;
		_colorNames.push_back(iter->first);
	}
    _doneColorInit = true;
}

std::list<std::string> AnnotationBase::colorChoices() {
	_initColors();
	return _colorNames;
}

void AnnotationBase::_testConvertToPixel() const {
	Vector<Double> pixel(2);
	Vector<Double> world(2);
	const auto units = _csys.worldAxisUnits();
	const auto end = _convertedDirections.end();
	for (auto iter = _convertedDirections.begin(); iter != end; ++iter) {
		world[0] = iter->getAngle().getValue(units[0])[0];
		world[1] = iter->getAngle().getValue(units[1])[1];
		if (! _csys.directionCoordinate().toPixel(pixel, world)) {
			ostringstream oss;
			oss << "Could not convert world coordinate " << world << "to pixel";
			throw (WorldToPixelConversionError(oss.str()));
		}
	}
}

String AnnotationBase::_printDirection(
	const Quantity& longitude, const Quantity& latitude
) const {
	if (longitude.getUnit() == "pix") {
		ostringstream os;
		os << _printPixel(longitude.getValue())
			<< ", "
			<< _printPixel(latitude.getValue());
		return os.str();
	}
	MDirection::Types frame;
	MDirection::getType(frame, _params.find(COORD)->second);
	if (
		frame == MDirection::J2000
		|| frame == MDirection::B1950
		|| frame == MDirection::JMEAN
		|| frame == MDirection::JTRUE
		|| frame == MDirection::B1950_VLA
		|| frame == MDirection::BMEAN
		|| frame == MDirection::BTRUE
	) {
		// equatorial coordinates in sexigesimal
		MVAngle x(longitude);
		MVAngle y(latitude);
		return x.string(MVAngle::TIME_CLEAN, 11) + ", " + y.string(MVAngle::ANGLE, 10);
	}
	else {
		// non-equatorial coordinates in degrees
		return _toDeg(longitude) + ", " + _toDeg(latitude);
	}
}

String AnnotationBase::_toArcsec(const Quantity& angle) {
	ostringstream os;
	if (angle.getUnit() == "pix") {
		os << _printPixel(angle.getValue());
	}
	else {
		os << std::fixed << std::setprecision(4)
			<< angle.getValue("arcsec") << "arcsec";
	}
	return os.str();
}

String AnnotationBase::_toDeg(const Quantity& angle) {
	ostringstream os;
	if (angle.getUnit() == "pix") {
		os << _printPixel(angle.getValue());
	}
	else {
		os << std::fixed << std::setprecision(8)
			<< angle.getValue("deg") << "deg";
	}
	return os.str();
}

String AnnotationBase::_printPixel(const Double& d) {
	ostringstream os;
	os << std::fixed << std::setprecision(1)
		<< d << "pix";
	return os.str();
}


}



