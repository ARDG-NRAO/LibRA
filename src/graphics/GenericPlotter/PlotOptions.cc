//# PlotOptions.cc: Customization classes for plotter objects.
//# Copyright (C) 2008
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
//# $Id: $
#include <graphics/GenericPlotter/PlotOptions.h>

using namespace std;

using namespace casacore;
namespace casa {





///////////////////////////
// PLOTCOLOR DEFINITIONS //
///////////////////////////

const String PlotColor::REC_HEXADECIMAL = "hexadecimal";
const String PlotColor::REC_ALPHA = "alpha";

PlotColor::PlotColor() { }
PlotColor::~PlotColor() { }

void PlotColor::setAsHexadecimal(const String& hex) {
    setAsHexadecimalOrName(hex); }
void PlotColor::setAsName(const String& name) { setAsHexadecimalOrName(name); }

Record PlotColor::toRecord() const {
    Record rec;
    rec.define(REC_HEXADECIMAL, asHexadecimal());
    rec.define(REC_ALPHA, alpha());
    return rec;
}

void PlotColor::fromRecord(const Record& record) {
    if(record.isDefined(REC_HEXADECIMAL) &&
       record.dataType(REC_HEXADECIMAL) == TpString)
        setAsHexadecimal(record.asString(REC_HEXADECIMAL));
    
    if(record.isDefined(REC_ALPHA) && record.dataType(REC_ALPHA) == TpDouble)
        setAlpha(record.asDouble(REC_ALPHA));
}

PlotColor& PlotColor::operator=(const PlotColor& rh) {
    setAsHexadecimal(rh.asHexadecimal());
    setAlpha(rh.alpha());
    return *this;
}

bool PlotColor::operator==(const PlotColor& rh) const {
    return asHexadecimal() == rh.asHexadecimal() && alpha() == rh.alpha(); }
bool PlotColor::operator!=(const PlotColor& rh) const{ return !(*this == rh); }


//////////////////////////
// PLOTFONT DEFINITIONS //
//////////////////////////

const String PlotFont::REC_POINTSIZE = "pointSize";
const String PlotFont::REC_PIXELSIZE = "pixelSize";
const String PlotFont::REC_FAMILY = "family";
const String PlotFont::REC_COLOR = "color";
const String PlotFont::REC_ITALICS = "italics";
const String PlotFont::REC_BOLD = "bold";
const String PlotFont::REC_UNDERLINE = "underline";

PlotFont::PlotFont() { }
PlotFont::~PlotFont() { }

void PlotFont::setColor(const PlotColorPtr c) { if(!c.null()) setColor(*c); }
void PlotFont::setColor(const String& col) {
    PlotColorPtr c = color();
    c->setAsHexadecimalOrName(col);
    setColor(*c);
}

Record PlotFont::toRecord() const {
    Record rec;
    rec.define(REC_POINTSIZE, pointSize());
    rec.define(REC_PIXELSIZE, pixelSize());
    rec.define(REC_FAMILY, fontFamily());
    PlotColorPtr c = color();
    if(!c.null()) rec.defineRecord(REC_COLOR, color()->toRecord());
    rec.define(REC_ITALICS, italics());
    rec.define(REC_BOLD, bold());
    rec.define(REC_UNDERLINE, underline());
    return rec;
}

void PlotFont::fromRecord(const Record& record) {
    if(record.isDefined(REC_POINTSIZE) &&
       record.dataType(REC_POINTSIZE) == TpDouble)
        setPointSize(record.asDouble(REC_POINTSIZE));
    
    if(record.isDefined(REC_PIXELSIZE) &&
       record.dataType(REC_PIXELSIZE) == TpInt)
        setPixelSize(record.asInt(REC_PIXELSIZE));
    
    if(record.isDefined(REC_FAMILY) && record.dataType(REC_FAMILY) == TpString)
        setFontFamily(record.asString(REC_FAMILY));
    
    if(record.isDefined(REC_COLOR) && record.dataType(REC_COLOR) == TpRecord) {
        PlotColorPtr c = color();
        if(!c.null()) {
            c->fromRecord(record.asRecord(REC_COLOR));
            setColor(c);
        }
    }
    
    if(record.isDefined(REC_ITALICS) && record.dataType(REC_ITALICS) == TpBool)
        setItalics(record.asBool(REC_ITALICS));
    
    if(record.isDefined(REC_BOLD) && record.dataType(REC_BOLD) == TpBool)
        setItalics(record.asBool(REC_BOLD));
    
    if(record.isDefined(REC_UNDERLINE)&&record.dataType(REC_UNDERLINE)==TpBool)
        setUnderline(record.asBool(REC_UNDERLINE));
}

PlotFont& PlotFont::operator=(const PlotFont& rh) {
    setPointSize(rh.pointSize());
    setPixelSize(rh.pixelSize());
    setFontFamily(rh.fontFamily());
    setColor(rh.color());
    setItalics(rh.italics());
    setBold(rh.bold());
    setUnderline(rh.underline());
    return *this;
}

bool PlotFont::operator==(const PlotFont& rh) const {
    return pointSize() == rh.pointSize() &&
           pixelSize() == rh.pixelSize() &&
           fontFamily() == rh.fontFamily() &&
           *color() == *rh.color() && italics() == rh.italics() &&
           bold() == rh.bold() && underline() == rh.underline();
}
bool PlotFont::operator!=(const PlotFont& rh) const { return !(*this == rh); }


//////////////////////////////
// PLOTAREAFILL DEFINITIONS //
//////////////////////////////

const String PlotAreaFill::REC_COLOR   = "color";
const String PlotAreaFill::REC_PATTERN = "pattern";

PlotAreaFill::PlotAreaFill() { }
PlotAreaFill::~PlotAreaFill() { }

void PlotAreaFill::setColor(const PlotColorPtr c){ if(!c.null()) setColor(*c);}
void PlotAreaFill::setColor(const String& co) {
    PlotColorPtr c = color();
    c->setAsHexadecimalOrName(co);
    setColor(*c);
}

void PlotAreaFill::setPattern( const String& descriptor ){
	 if(descriptor == "fill") {
		 setPattern(PlotAreaFill::FILL);
	 }
	 else if(descriptor == "mesh1") {
	     setPattern(PlotAreaFill::MESH1);
	 }
	 else if(descriptor == "mesh2") {
	     setPattern(PlotAreaFill::MESH2);
	 }
	 else if(descriptor == "mesh3") {
	     setPattern(PlotAreaFill::MESH3);
	 }
	 else {
		 cout << "Unrecognized fill pattern: "<<descriptor.c_str()<<endl;
	 }
}

Record PlotAreaFill::toRecord() const {
    Record rec;
    PlotColorPtr c = color();
    if(!c.null()) rec.defineRecord(REC_COLOR, c->toRecord());
    rec.define(REC_PATTERN, (int)pattern());
    return rec;
}

void PlotAreaFill::fromRecord(const Record& record) {
    if(record.isDefined(REC_COLOR) && record.dataType(REC_COLOR) == TpRecord) {
        PlotColorPtr c = color();
        if(!c.null()) {
            c->fromRecord(record.asRecord(REC_COLOR));
            setColor(c);
        }
    }
    
    if(record.isDefined(REC_PATTERN) && record.dataType(REC_PATTERN) == TpInt)
        setPattern((Pattern)record.asInt(REC_PATTERN));
}

PlotAreaFill& PlotAreaFill::operator=(const PlotAreaFill& rh) {
    setColor(rh.color());
    setPattern(rh.pattern());
    return *this;
}

bool PlotAreaFill::operator==(const PlotAreaFill& rh) const {
    if(pattern() == rh.pattern() && pattern() == NOFILL){
    	return true;
    }
    return *color() == *rh.color() && pattern() == rh.pattern();
}
bool PlotAreaFill::operator!=(const PlotAreaFill& rh) const {
    return !(*this == rh); }


//////////////////////////
// PLOTLINE DEFINITIONS //
//////////////////////////

const String PlotLine::REC_WIDTH = "width";
const String PlotLine::REC_STYLE = "style";
const String PlotLine::REC_COLOR = "color";

PlotLine::PlotLine() { }
PlotLine::~PlotLine() { }

void PlotLine::setColor(const PlotColorPtr c) { if(!c.null()) setColor(*c); }
void PlotLine::setColor(const String& col) {
    PlotColorPtr c = color();
    c->setAsHexadecimalOrName(col);
    setColor(*c);
}

Record PlotLine::toRecord() const {
    Record rec;
    rec.define(REC_WIDTH, width());
    rec.define(REC_STYLE, (int)style());
    PlotColorPtr c = color();
    if(!c.null()) rec.defineRecord(REC_COLOR, c->toRecord());
    return rec;
}

void PlotLine::fromRecord(const Record& record) {
    if(record.isDefined(REC_WIDTH) && record.dataType(REC_WIDTH) == TpDouble)
        setWidth(record.asDouble(REC_WIDTH));
    
    if(record.isDefined(REC_STYLE) && record.dataType(REC_STYLE) == TpInt)
        setStyle((Style)record.asInt(REC_STYLE));
    
    if(record.isDefined(REC_COLOR) && record.dataType(REC_COLOR) == TpRecord) {
        PlotColorPtr c = color();
        if(!c.null()) {
            c->fromRecord(record.asRecord(REC_COLOR));
            setColor(c);
        }
    }
}

PlotLine& PlotLine::operator=(const PlotLine& rh) {
    setWidth(rh.width());
    setStyle(rh.style());
    setColor(rh.color());
    return *this;
}

bool PlotLine::operator==(const PlotLine& rh) const {
    if(style() == rh.style() && style() == NOLINE) return true;
    if(width() == rh.width() && width() == 0) return true;
    return width() == rh.width() && style() == rh.style() &&
           *color() == *rh.color();
}

bool PlotLine::operator!=(const PlotLine& rh) const { return !(*this == rh); }


////////////////////////////
// PLOTSYMBOL DEFINITIONS //
////////////////////////////

const String PlotSymbol::REC_WIDTH         = "width";
const String PlotSymbol::REC_HEIGHT        = "height";
const String PlotSymbol::REC_HEIGHTISPIXEL = "heightIsPixel";
const String PlotSymbol::REC_SYMBOL        = "symbol";
const String PlotSymbol::REC_UCHAR         = "symbolUChar";
const String PlotSymbol::REC_LINE          = "line";
const String PlotSymbol::REC_AREAFILL      = "areaFill";
const String PlotSymbol::REC_COLOR         = "symbolColor";

PlotSymbol::PlotSymbol():DEFAULT_COLOR("black") {
	currentColor = DEFAULT_COLOR;
}
PlotSymbol::~PlotSymbol() { }

void PlotSymbol::setSize(psize_t size) {
    setSize(size.first, size.second); }

bool PlotSymbol::isCharacter() const { return symbol() == CHARACTER; }

void PlotSymbol::setLine(const PlotLinePtr l) {
    if(!l.null()) setLine(*l);
    else          setLine(DEFAULT_COLOR, PlotLine::NOLINE);
}

void PlotSymbol::setLine(const String& color, PlotLine::Style style,
        double width) {
    PlotLinePtr l = line();
    l->setColor(color);
    l->setStyle(style);
    l->setWidth(width);
    setLine(*l);
}

void PlotSymbol::setAreaFill(const PlotAreaFillPtr a) {
    if(!a.null()) setAreaFill(*a);
    else          setAreaFill(DEFAULT_COLOR, PlotAreaFill::NOFILL);
}



void PlotSymbol::setAreaFill(const String& color,
        PlotAreaFill::Pattern pattern) {
    PlotAreaFillPtr a = areaFill();
    a->setColor(color);
    a->setPattern(pattern);
    setAreaFill(*a);
}

void PlotSymbol::setColor(const PlotColor& color) {
    PlotLinePtr l = line();
    l->setColor(color);
    setLine(*l);
    
    PlotAreaFillPtr a = areaFill();
    a->setColor(color);
    setAreaFill(*a);
    currentColor = color.asHexadecimal();
}

void PlotSymbol::setColor(const PlotColorPtr color) {
    if(!color.null()) setColor(*color);
}

String PlotSymbol::getColor() const {
	return currentColor;
}

void PlotSymbol::setColor(const String& color) {
    PlotLinePtr l = line();
    l->setColor(color);
    setLine(*l);
    
    PlotAreaFillPtr a = areaFill();
    a->setColor(color);
    setAreaFill(*a);
	currentColor = color;

}

Record PlotSymbol::toRecord() const {
    Record rec;
    
    psize_t s = size();
    rec.define(REC_WIDTH, s.first);
    rec.define(REC_HEIGHT, s.second);
    
    rec.define(REC_HEIGHTISPIXEL, heightIsPixel());
    rec.define(REC_SYMBOL, (int)symbol());
    rec.define(REC_UCHAR, (int)symbolUChar());
    rec.define(REC_COLOR, currentColor);
    
    PlotLinePtr l = line();
    if(!l.null()) rec.defineRecord(REC_LINE, l->toRecord());
    
    PlotAreaFillPtr a = areaFill();
    if(!a.null()) rec.defineRecord(REC_AREAFILL, a->toRecord());

    return rec;
}

void PlotSymbol::setSymbol( const String& descriptor ){
	if(descriptor == "pixel") {
		setSymbol(PlotSymbol::PIXEL);
	}
	else if(descriptor == "autoscaling") {
		setSymbol(PlotSymbol::AUTOSCALING);
	}
	else if(descriptor== "circle") {
		setSymbol(PlotSymbol::CIRCLE);
	}
	else if(descriptor == "square") {
		setSymbol(PlotSymbol::SQUARE);
	}
	else if(descriptor == "diamond") {
		setSymbol(PlotSymbol::DIAMOND);
	}
	else if(descriptor == "nosymbol"){
		setSymbol(PlotSymbol::NOSYMBOL );
	}
	else {
		cout << "Unrecognized PlotSymbol: "<<descriptor.c_str()<<endl;
	}
}

void PlotSymbol::fromRecord(const Record& record) {
    psize_t s = size();
    if(record.isDefined(REC_WIDTH) && record.dataType(REC_WIDTH) == TpDouble)
        s.first = record.asDouble(REC_WIDTH);
    if(record.isDefined(REC_HEIGHT) && record.dataType(REC_HEIGHT) == TpDouble)
        s.second = record.asDouble(REC_HEIGHT);
    setSize(s);
    
    if(record.isDefined(REC_HEIGHTISPIXEL) &&
       record.dataType(REC_HEIGHTISPIXEL) == TpBool)
        setHeightIsPixel(record.asBool(REC_HEIGHTISPIXEL));
    
    if(record.isDefined(REC_UCHAR) && record.dataType(REC_UCHAR) == TpInt)
        setUSymbol((unsigned short)record.asInt(REC_UCHAR));
    
    if(record.isDefined(REC_SYMBOL) && record.dataType(REC_SYMBOL) == TpInt)
        setSymbol((Symbol)record.asInt(REC_SYMBOL));
    
    if(record.isDefined(REC_LINE) && record.dataType(REC_LINE) == TpRecord) {
        PlotLinePtr l = line();
        if(!l.null()) {
            l->fromRecord(record.asRecord(REC_LINE));
            setLine(l);
        }
    }
    
    if(record.isDefined(REC_AREAFILL) &&
       record.dataType(REC_AREAFILL) == TpRecord) {
        PlotAreaFillPtr a = areaFill();
        if(!a.null()) {
            a->fromRecord(record.asRecord(REC_AREAFILL));
            setAreaFill(a);
        }
    }
    if (record.isDefined(REC_COLOR) && record.dataType(REC_COLOR) == TpString){
    	this->setColor( record.asString( REC_COLOR));
    }

}

PlotSymbol& PlotSymbol::operator=(const PlotSymbol& rh) {
    setSize(rh.size());
    setHeightIsPixel(rh.heightIsPixel());
    setUSymbol(rh.symbolUChar());
    setSymbol(rh.symbol());
    setLine(rh.line());
    setAreaFill(rh.areaFill());
    setColor( rh.getColor());
    return *this;
}

bool PlotSymbol::operator==(const PlotSymbol& rh) const {
    if(symbol() == rh.symbol() && symbol() == NOSYMBOL) return true;
    if(size() == rh.size() && size() == psize_t(0, 0)) return true;
    bool symbolsEqual =  size() == rh.size() && symbol() == rh.symbol() &&
           (isCharacter() ? symbolChar() == rh.symbolChar() : true) &&
           (isCharacter() ? heightIsPixel() == rh.heightIsPixel() : true) &&
           *line() == *rh.line() && *areaFill() == *rh.areaFill();// &&
           //getColor() == rh.getColor();

    return symbolsEqual;
}

bool PlotSymbol::operator!=(const PlotSymbol& rh) const {
    return !(*this == rh); }


////////////////////////////////
// PLOTCOORDINATE DEFINITIONS //
////////////////////////////////

PlotCoordinate::PlotCoordinate() : m_system(WORLD), m_x(0), m_y(0) { }

PlotCoordinate::PlotCoordinate(double dx, double dy, System s): m_system(s),
        m_x(dx), m_y(dy) { }

PlotCoordinate::PlotCoordinate(const PlotCoordinate& c) { operator=(c); }

PlotCoordinate::~PlotCoordinate() { }

PlotCoordinate::System PlotCoordinate::system() const { return m_system; }
double PlotCoordinate::x() const { return m_x; }
double PlotCoordinate::y() const { return m_y; }

PlotCoordinate& PlotCoordinate::operator=(const PlotCoordinate& rh) {
    m_system = rh.system();
    m_x = rh.x();
    m_y = rh.y();
    return *this;
}

bool PlotCoordinate::operator==(const PlotCoordinate& rh) const {
    return m_system == rh.system() && m_x == rh.x() && m_y == rh.y(); }

bool PlotCoordinate::operator!=(const PlotCoordinate& rh) const {
    return !(*this == rh); }


////////////////////////////
// PLOTREGION DEFINITIONS //
////////////////////////////

PlotRegion::PlotRegion() { }

PlotRegion::PlotRegion(const PlotCoordinate& upperLeft,
        const PlotCoordinate& lowerRight) : m_upperLeft(upperLeft),
        m_lowerRight(lowerRight) { }

PlotRegion::PlotRegion(const PlotRegion& copy) : m_upperLeft(copy.upperLeft()),
        m_lowerRight(copy.lowerRight()) { }

PlotRegion::~PlotRegion() { }

const PlotCoordinate& PlotRegion::upperLeft() const { return m_upperLeft; }

const PlotCoordinate& PlotRegion::lowerRight() const { return m_lowerRight; }

double PlotRegion::top() const { return m_upperLeft.y(); }

double PlotRegion::bottom() const { return m_lowerRight.y(); }

double PlotRegion::left() const { return m_upperLeft.x(); }

double PlotRegion::right() const { return m_lowerRight.x(); }

bool PlotRegion::isValid() const {
    return m_upperLeft.system() == m_lowerRight.system() &&
           m_upperLeft.x() < m_lowerRight.x() &&
           m_upperLeft.y() > m_lowerRight.y();
}


///////////////////////////////
// PLOTAXESSTACK DEFINITIONS //
///////////////////////////////

PlotAxesStack::PlotAxesStack(int lengthLimit) : m_lengthLimit(lengthLimit),
        m_stackIndex(0) {
    if(m_lengthLimit <= 1) m_lengthLimit = -1;
}

PlotAxesStack::~PlotAxesStack() { }


int PlotAxesStack::lengthLimit() const { return m_lengthLimit; }
void PlotAxesStack::setLengthLimit(int lengthLimit) {
    if(lengthLimit <= 1) lengthLimit = -1;
    if(m_lengthLimit != lengthLimit) {
        m_lengthLimit = lengthLimit;
        if(lengthLimit > 0 && m_stack.size() > (unsigned int)lengthLimit)
            shrinkStacks((unsigned int)lengthLimit);
    }
}

bool PlotAxesStack::isValid() const { return m_stack.size() > 0; }
unsigned int PlotAxesStack::stackIndex() const { return m_stackIndex; }
unsigned int PlotAxesStack::size() const { return m_stack.size(); }

vector<PlotRegion> PlotAxesStack::stack() const { return m_stack; }
vector<pair<PlotAxis, PlotAxis> > PlotAxesStack::stackAxes() const {
    return m_axes; }

void PlotAxesStack::setBase(const PlotRegion& base, PlotAxis xAxis,
        PlotAxis yAxis) {

    m_stack.resize(0); // so next line sets element zero!
    m_stack.resize(1, base);
    m_axes.resize(0);  // so next line sets element zero!
    m_axes.resize(1, pair<PlotAxis, PlotAxis>(xAxis, yAxis));
    m_stackIndex = 0;

}

void PlotAxesStack::addRegion(const PlotRegion& region, PlotAxis xAxis,
        PlotAxis yAxis) {
    if(m_stack.size() == 0) setBase(region, xAxis, yAxis);
    else {
        unsigned int newSize = m_stackIndex + 2;
        
        if(m_lengthLimit > 0 && newSize > (unsigned int)m_lengthLimit) {
            // resize by discarding old values
            shrinkStacks((unsigned int)m_lengthLimit);
            
            // Check for special indexing case.  Otherwise we override the
            // current stack index member.
            if(m_stackIndex < (unsigned int)(m_lengthLimit - 1))
                m_stackIndex++;
            
        } else if(m_stack.size() != newSize) {
            m_stack.resize(newSize);
            m_axes.resize(newSize);
            m_stackIndex++;
            
        } else m_stackIndex++;
        
        m_stack[m_stackIndex] = region;
        m_axes[m_stackIndex] = pair<PlotAxis, PlotAxis>(xAxis, yAxis);
    }
}

void PlotAxesStack::clearStack(bool keepBase) {
    if(m_stack.size() == 0) return;
    m_stack.resize(keepBase ? 1 : 0);
    m_axes.resize(keepBase ? 1 : 0);
    m_stackIndex = 0;
}

PlotRegion PlotAxesStack::currentRegion() const {
    if(isValid()) return m_stack[m_stackIndex];
    else          return PlotRegion();
}

PlotAxis PlotAxesStack::currentXAxis() const {
    if(isValid()) return m_axes[m_stackIndex].first;
    else          return X_BOTTOM;
}

PlotAxis PlotAxesStack::currentYAxis() const {
    if(isValid()) return m_axes[m_stackIndex].second;
    else          return Y_LEFT;
}

void PlotAxesStack::move(int delta) {
    if(!isValid()) return;
    if(delta == 0) delta = -m_stackIndex;
    int temp = m_stackIndex + delta;
    if(temp < 0) temp = 0;
    else if(temp >= (int)m_stack.size()) temp = m_stack.size() - 1;
    m_stackIndex = (unsigned int)temp;
}

PlotRegion PlotAxesStack::moveAndReturn(int delta) {
    move(delta);
    return currentRegion();
}

  /*
void PlotAxesStack::summary() {

  cout << "Stack listing ";
    
  if (size()>0) {
    cout << "(" << size() << "):" << endl;
    
    for (uint i=0;i<size();++i)
      cout << i << " " 
	   << stack()[i].left() << " "
	   << stack()[i].right()  << " "
	   << stack()[i].bottom()  << " "
	   << stack()[i].top() << " "
	   << endl;
  }
  else
    cout << "(none)" << endl;

}

  */


void PlotAxesStack::shrinkStacks(unsigned int n) {
    if(n == 0 || m_stack.size() >= n) return;
    
    // Shorten the stacks.
    unsigned int diff = m_stack.size() - n;
    vector<PlotRegion> stack(n);
    vector<pair<PlotAxis, PlotAxis> > axes(n);
    
    // Copy over bases.
    stack[0] = m_stack[0];
    axes[0] = m_axes[0];

    if(m_stackIndex > 0 && m_stackIndex <= diff) {
        // Current index is in the part that is to be discarded, so
        // keep that member and copy over the rest, discarding the
        // oldest.
        stack[1] = m_stack[m_stackIndex];
        axes[1] = m_axes[m_stackIndex];
        
        for(unsigned int i = 2; i < n; i++) {
            stack[i] = m_stack[i + diff];
            axes[i] = m_axes[i + diff];
        }
        
        // Update stack index.
        m_stackIndex = 1;
        
    } else {
        // Copy over the rest, discarding the oldest.
        for(unsigned int i = 1; i < n; i++) {
            stack[i] = m_stack[i + diff];
            axes[i] = m_axes[i + diff];
        }
        
        // Update the stack index if not at base.
        if(m_stackIndex > 0) m_stackIndex -= diff;
    }
    
    m_stack = stack;
    m_axes = axes;
}


//////////////////////////////////
// PLOTEXPORTFORMAT DEFINITIONS //
//////////////////////////////////

PlotExportFormat::PlotExportFormat(Type t, const String& file) : type(t),
        verbose(true), location(file), resolution(SCREEN), dpi(-1),
		width(-1), height(-1) { }

PlotExportFormat::~PlotExportFormat() { }

String PlotExportFormat::exportFormat(Type t) {
    switch(t) {
    case PNG: return "PNG";
    case JPG: return "JPG";
    case PS: return "PS";
    case PDF: return "PDF";
    case TEXT: return "TEXT";
    default: return "";
    }
}

PlotExportFormat::Type PlotExportFormat::exportFormat(String t, bool* ok) {
    t.downcase();
    
    if(ok != NULL) *ok = true;
    if(t == "png")                     return PNG;
    else if(t == "jpg" || t == "jpeg") return JPG;
    else if(t == "ps")                 return PS;
    else if(t == "pdf")                return PDF;
    else if(t == "txt" || t == "text" || t == "csv")
                                       return TEXT;
    
    // error
    if(ok != NULL) *ok = false;
    return PNG;
}

String PlotExportFormat::extensionFor(Type t) {
    switch(t) {
    case PNG: return "png";
    case JPG: return "jpg";
    case PS: return "ps";
    case PDF: return "pdf";
    case TEXT: return "txt";
    default: return "";
    }
}

PlotExportFormat::Type PlotExportFormat::typeForExtension(String f, bool* ok) {
    f.downcase();
    unsigned int n = f.size();
    if(ok != NULL) *ok = true;
    if(f.matches("png", n - 3))      return PNG;
    else if(f.matches("jpg", n - 3) || f.matches("jpeg", n - 4))
                                     return JPG;
    else if(f.matches("ps", n - 2))  return PS;
    else if(f.matches("pdf", n - 3)) return PDF;
    else if(f.matches("txt", n - 3) || f.matches("text", n - 4) ||
            f.matches("csv", n - 3))
                                     return TEXT;
    
    // error
    if(ok != NULL) *ok = false;
    return PNG;
}

vector<PlotExportFormat::Type> PlotExportFormat::supportedFormats() {
    vector<Type> v(NUM_FMTS);
    v[0] = PNG;
    v[1] = JPG;
    v[2] = PS;
    v[3] = PDF;
    v[4] = TEXT;
    return v;
}    
vector<String> PlotExportFormat::supportedFormatStrings() {
    vector<Type> v = supportedFormats();
    vector<String> s(v.size());
    for(unsigned int i = 0; i < s.size(); i++) s[i] = exportFormat(v[i]);
    return s;
}

vector<PlotExportFormat::Type> PlotExportFormat::supportedImageFormats() {
    vector<Type> v(2);
    v[0] = PNG;
    v[1] = JPG;
    return v;
}

vector<String> PlotExportFormat::supportedImageFormatStrings() {
    vector<Type> v = supportedImageFormats();
    vector<String> s(v.size());
    for(unsigned int i = 0; i < s.size(); i++) s[i] = exportFormat(v[i]);
    return s;
}

}
