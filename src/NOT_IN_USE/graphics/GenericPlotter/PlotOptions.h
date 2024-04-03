//# PlotOptions.h: Customization classes for plotter objects.
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
#ifndef PLOTOPTIONS_H_
#define PLOTOPTIONS_H_

#include <utility>
#include <cctype>
#include <vector>

#include <casacore/casa/Utilities/CountedPtr.h>
#include <casacore/casa/Containers/Record.h>

namespace casa {


// Typedef for range, which is two doubles (min and max).
typedef std::pair<double, double> prange_t;

// Typedef for size, which is two doubles (width and height).
typedef std::pair<double, double> psize_t;



///////////
// ENUMS //
///////////

enum SortDirection {
	ASCENDING,
	DESCENDING
};


// Enum for the four plot axes.  If this enum is changed, PlotCanvas::allAxes()
// needs to be updated.
// Use this in contexts where one and only one side is specified.
// For combinations of sides, or none, use  PlotAxiBitset
enum PlotAxis {
    X_BOTTOM = 1, X_TOP = 2, Y_LEFT = 4, Y_RIGHT = 8
};


// Set of bit flags to indicate combinations of sides, 
// used (as of this writing) for indicating which axes have visible scales.
// The value of a PlotSideBitset is the bitwise OR of values of PlotSide.
typedef  unsigned int  PlotAxisBitset;

const PlotAxisBitset all_four_sides  = 0xF;
const PlotAxisBitset none_sides      = 0x0;



// Enum for possible axis scales.
enum PlotAxisScale {
    NORMAL,
    LOG10,        // logarithmic scale
    ANGLE,        // display scale values as formatted angles
    DATE_MJ_SEC,  // display scale values as dates (modified julian seconds)
    DATE_MJ_DAY   // display scale values as dates (modified julian days)
};

// Enum for angle formats
enum AngleFormat {
	DECIMAL,  // Decimal Number
	HMS,      // Hours Minutes Seconds
	DMS       // Degrees Arcminutes Arcseconds
};

// Enum for cursors.
enum PlotCursor {
    NORMAL_CURSOR,
    HAND_OPEN,
    HAND_CLOSED,
    CROSSHAIR,
    WAIT,
    TEXT
};

// The canvas is composed of multiple layers, where changing/adding items from
// one layer will not affect the others.  This is mainly used to separate items
// that are costly to draw (i.e. scatter plots with many points) from
// interaction-type items (i.e. annotations and selections).  The layers share
// axes and are otherwise transparent to the user.  If this enum is changed,
// PlotCanvas::allLayers() needs to be updated.
enum PlotCanvasLayer {
    MAIN       = 1, // "Main" or bottom layer.
    ANNOTATION = 2  // Annotations or top layer.
};


//////////////////////
// ABSTRACT CLASSES //
//////////////////////

// Abstract class for colors.  Any implementation of color should be able to
// provide a hexadecimal form of the color (i.e., "000000" for black) and, if
// applicable, a human-readable name (i.e. "black").  In many places throughout
// the plotter, color and Strings are interchangeable; however implementation
// classes should use PlotColors where possible for additional features such as
// alpha blending.
class PlotColor {
public:
    // Constructor.
    PlotColor();
    
    // Destructor.
    virtual ~PlotColor();
    
    
    // ABSTRACT METHODS //
    
    // Returns this color's value in a hexadecimal form, i.e. "000000".
    virtual casacore::String asHexadecimal() const = 0;
    
    // Returns this color's value as a human-readable name if applicable, or an
    // empty casacore::String if inapplicable.
    virtual casacore::String asName() const = 0;
    
    // If the given casacore::String is a hexadecimal value, sets the color to it.
    // Otherwise tries to set the color as a name.  If the given name is
    // invalid, the behavior is undefined but should probably default to a
    // sensible value.
    virtual void setAsHexadecimalOrName(const casacore::String& str) = 0;
    
    // Returns this color's alpha as a value between 0 (transparent) and 1
    // (opaque).
    virtual double alpha() const = 0;
    
    // Sets this color's alpha as a value between 0 (transparent) and 1
    // (opaque).
    virtual void setAlpha(double a) = 0;
    
    
    // CONVENIENCE METHODS //
    
    // Sets this color's value as a hexadecimal value.
    virtual void setAsHexadecimal(const casacore::String& hex);
    
    // Set's this color's value to the given named color.
    virtual void setAsName(const casacore::String& name);
    
    
    // RECORD METHODS //
    
    // Gets/Sets the color as a Record.
    // <group>
    virtual casacore::Record toRecord() const;
    virtual void fromRecord(const casacore::Record& record);
    // </group>
    
    
    // OPERATORS //
    
    // Assigns the value of the given PlotColor to this one.
    virtual PlotColor& operator=(const PlotColor& rh);
    
    // Returns true if this PlotColor is equal to the given; false otherwise.
    virtual bool operator==(const PlotColor& rh) const;

    // Returns true if this PlotColor is NOT equal to the given; false
    // otherwise.
    virtual bool operator!=(const PlotColor& rh) const;
    
protected:
    // casacore::Record key names.
    // <group>
    static const casacore::String REC_HEXADECIMAL; // String
    static const casacore::String REC_ALPHA;       // double
    // </group>
};
typedef casacore::CountedPtr<PlotColor> PlotColorPtr;


// Abstract class for fonts.  A font has a family, size, color, bold, italics,
// and underline properties.
class PlotFont {
public:
    // Constructor.
    PlotFont();
    
    // Destructor.
    virtual ~PlotFont();
    
    
    // ABSTRACT METHODS //
    
    // Returns the point size of this font, or -1 if the size was set in
    // pixels.
    virtual double pointSize() const = 0;
    
    // Sets the point size of this font to the given.
    virtual void setPointSize(double size) = 0;
    
    // Returns the pixel size of this font, or -1 if the size was set in
    // points.
    virtual int pixelSize() const = 0;
    
    // Sets the pixel size of this font to the given.
    virtual void setPixelSize(int size) = 0;
    
    // Returns the font family.
    virtual casacore::String fontFamily() const = 0;
    
    // Sets the font family to the given.
    virtual void setFontFamily(const casacore::String& font) = 0;
    
    // Returns a copy of the color for this font.
    virtual PlotColorPtr color() const = 0;
    
    // Sets the color of this font to the given.
    virtual void setColor(const PlotColor& color) = 0;
    
    // Gets/sets whether this font is italicized, bolded, and/or underlined,
    // respectively.
    // <group>
    virtual bool italics() const = 0;
    virtual void setItalics(bool i = true) = 0;
    virtual bool bold() const = 0;    
    virtual void setBold(bool b = true) = 0;    
    virtual bool underline() const = 0;    
    virtual void setUnderline(bool u = true) = 0;
    // </group>
    
    
    // CONVENIENCE METHODS //
    
    // Convenience methods for setting color.
    // <group>
    virtual void setColor(const PlotColorPtr c);
    virtual void setColor(const casacore::String& col);
    // </group>
    
    
    // RECORD METHODS //
    
    // Gets/Sets the color as a Record.
    // <group>
    virtual casacore::Record toRecord() const;
    virtual void fromRecord(const casacore::Record& record);
    // </group>
    
    
    // OPERATORS //
    
    // Assigns the value of the given PlotFont to this one.
    virtual PlotFont& operator=(const PlotFont& rh);
    
    // Returns true if this PlotFont is equal to the given; false otherwise.
    virtual bool operator==(const PlotFont& rh) const;

    // Returns true if this PlotFont is NOT equal to the given; false
    // otherwise.
    virtual bool operator!=(const PlotFont& rh) const;
    
protected:
    // casacore::Record key names.
    // <group>
    static const casacore::String REC_POINTSIZE; // double
    static const casacore::String REC_PIXELSIZE; // int
    static const casacore::String REC_FAMILY;    // String
    static const casacore::String REC_COLOR;     // Record
    static const casacore::String REC_ITALICS;   // bool
    static const casacore::String REC_BOLD;      // bool
    static const casacore::String REC_UNDERLINE; // bool
    // </group>
};
typedef casacore::CountedPtr<PlotFont> PlotFontPtr;


// Abstract class for area fill.  An area fill consists of a color and a
// pattern.
class PlotAreaFill {
public:
    // Pattern enum, similar in spirit to
    // http://doc.trolltech.com/4.3/qt.html#BrushStyle-enum .
    enum Pattern {
        FILL, MESH1, MESH2, MESH3, NOFILL
    };
    
    // Constructor
    PlotAreaFill();
    
    // Destructor
    virtual ~PlotAreaFill();
    
    
    // ABSTRACT METHODS //
    
    // Returns a copy of the color in this area fill.
    virtual PlotColorPtr color() const = 0;
    
    // Sets the area fill color to the given.
    virtual void setColor(const PlotColor& color) = 0;
    
    // Returns the pattern for this area fill.
    virtual Pattern pattern() const = 0;
    
    // Sets the pattern for this area fill to the given.
    virtual void setPattern(Pattern pattern) = 0;
    virtual void setPattern( const casacore::String& descriptor );
    
    // CONVENIENCE METHODS //
    
    // Convenience methods for setting color.
    // <group>
    virtual void setColor(const PlotColorPtr c);
    virtual void setColor(const casacore::String& co);
    // </group>
    
    
    // RECORD METHODS //
    
    // Gets/Sets the color as a Record.
    // <group>
    virtual casacore::Record toRecord() const;
    virtual void fromRecord(const casacore::Record& record);
    // </group>
    
    
    // OPERATORS //
    
    // Assigns the value of the given PlotAreaFill to this one.
    virtual PlotAreaFill& operator=(const PlotAreaFill& rh);
    
    // Returns true if this PlotAreaFill is equal to the given; false
    // otherwise.
    virtual bool operator==(const PlotAreaFill& rh) const;

    // Returns true if this PlotAreaFill is NOT equal to the given; false
    // otherwise.
    virtual bool operator!=(const PlotAreaFill& rh) const;
    
protected:
    // casacore::Record key names.
    // <group>
    static const casacore::String REC_COLOR;   // Record
    static const casacore::String REC_PATTERN; // int
    // </group>
};
typedef casacore::CountedPtr<PlotAreaFill> PlotAreaFillPtr;


// Abstract class for a line.  A line has a color, style, and width.
class PlotLine {
public:
    // Static //
    
    // Line styles.
    enum Style {
        SOLID, DASHED, DOTTED, NOLINE
    };
    
    
    // Non-Static //
    
    // Constructor.
    PlotLine();
    
    // Destructor.
    virtual ~PlotLine();
    
    
    // ABSTRACT METHODS // 
    
    // Returns this line's width.
    virtual double width() const = 0;
    
    // Sets the width to the given.
    virtual void setWidth(double width) = 0;
    
    // Returns this line's style.
    virtual Style style() const = 0;
    
    // Sets the style to the given.
    virtual void setStyle(Style style) = 0;
    
    // Returns a copy of the color used for this line.
    virtual PlotColorPtr color() const = 0;
    
    // Sets this line's color to the given.
    virtual void setColor(const PlotColor& color) = 0;
    
    
    // CONVENIENCE METHODS //
    
    // Convenience methods for setting color.
    // <group>
    virtual void setColor(const PlotColorPtr c);
    virtual void setColor(const casacore::String& col);
    // </group>
    
    
    // RECORD METHODS //
    
    // Gets/Sets the color as a Record.
    // <group>
    virtual casacore::Record toRecord() const;
    virtual void fromRecord(const casacore::Record& record);
    // </group>
    
    
    // OPERATORS //
    
    // Assigns the value of the given PlotLine to this one.
    virtual PlotLine& operator=(const PlotLine& rh);
    
    // Returns true if this PlotLine is equal to the given; false otherwise.
    virtual bool operator==(const PlotLine& rh) const;

    // Returns true if this PlotLine is NOT equal to the given; false
    // otherwise.
    virtual bool operator!=(const PlotLine& rh) const;
    
protected:
    // casacore::Record key names.
    // <group>
    static const casacore::String REC_WIDTH; // double
    static const casacore::String REC_STYLE; // int
    static const casacore::String REC_COLOR; // Record
    // </group>
};
typedef casacore::CountedPtr<PlotLine> PlotLinePtr;


// Abstract class for a symbol.  A symbol has a style, size, line, and area
// fill.
class PlotSymbol {
public:
    // Static //
    
    // Symbol style.
    enum Symbol {
        CHARACTER,               // for char symbols
        CIRCLE, SQUARE, DIAMOND, // standard shapes
        PIXEL,                   // draw a single pixel
        NOSYMBOL,                // don't show symbols
        AUTOSCALING              // autoscaling symbol
    };
    
    
    // Non-Static //
    
    // Constructor.
    PlotSymbol();
    
    //PlotSymbol(const PlotSymbol& copy);

    // Destructor.
    virtual ~PlotSymbol();
    
    
    // ABSTRACT METHODS //
    
    // Returns the size, in pixels, of this symbol.  If this symbol is a
    // character, the height corresponds to the font size (in either pixels or
    // points, see heightIsPixel()).
    virtual psize_t size() const = 0;
    
    // Sets the size of the symbol in pixels.  The heightIsPixel parameter is
    // used for character symbols and indicates whether the given height is in
    // points or pixels.
    virtual void setSize(double width, double height,
            bool heightIsPixel = true) = 0;
    
    // Gets/Sets whether the set height is in pixels or points, ONLY for
    // character symbols.
    // <group>
    virtual bool heightIsPixel() const = 0;    
    virtual void setHeightIsPixel(bool pixel = true) = 0;
    // </group>
    
    // Returns the symbol style.
    virtual Symbol symbol() const = 0;
    
    // Returns the character for this symbol.  Invalid if the style is not
    // CHARACTER.
    virtual char symbolChar() const = 0;
    
    // Returns the character unicode for this symbol.  Invalid if the style is
    // not CHARACTER.
    virtual unsigned short symbolUChar() const = 0;
    
    // Sets the symbol style to the given.
    virtual void setSymbol(Symbol symbol) = 0;
    virtual void setSymbol( const casacore::String& descriptor );
    
    // Sets the symbol character to the given.  Implies setSymbol(CHARACTER).
    virtual void setSymbol(char c) = 0;
    
    // Sets the symbol character unicode to the given.  Implies
    // setSymbol(CHARACTER).
    virtual void setUSymbol(unsigned short unicode) = 0;
    
    // Returns a copy of the line for the outline of this symbol.  Does not
    // apply to character or pixel symbols.
    virtual PlotLinePtr line() const = 0;
    
    // Sets the outline of this symbol to the given.  Does not apply to
    // character or pixel symbols.
    virtual void setLine(const PlotLine& color) = 0;
    
    // Returns a copy of the area fill for this symbol.  Does not apply to
    // character or pixel symbols.
    virtual PlotAreaFillPtr areaFill() const = 0;
    
    // Sets the area fill of this symbol to the given.  Does not apply to
    // character or pixel symbols.
    virtual void setAreaFill(const PlotAreaFill& fill) = 0;
    
    
    // CONVENIENCE METHODS //
    
    // Convenience method for setting size.
    virtual void setSize(psize_t size);
    
    // Returns true if this symbol is set to a character or not.
    virtual bool isCharacter() const;
    
    // Convenience methods for setting the line.
    // <group>
    virtual void setLine(const PlotLinePtr l);
    virtual void setLine(const casacore::String& color,
                         PlotLine::Style style = PlotLine::SOLID,
                         double width = 1.0);
    // </group>
    
    // Convenience methods for setting area fill.
    // <group>
    virtual void setAreaFill(const PlotAreaFillPtr a);
    virtual void setAreaFill(const casacore::String& color,
                 PlotAreaFill::Pattern pattern = PlotAreaFill::FILL);
    // </group>
    
    // Convenience method for setting color of both line and area fill.
    // <group>
    virtual void setColor(const PlotColor& color);
    virtual void setColor(const PlotColorPtr color);
    virtual void setColor(const casacore::String& color);
    casacore::String getColor() const;
    // </group>
    
    
    // RECORD METHODS //
    
    // Gets/Sets the color as a Record.
    // <group>
    virtual casacore::Record toRecord() const;
    virtual void fromRecord(const casacore::Record& record);
    // </group>
    
    // OPERATORS //
    
    // Assigns the value of the given PlotSymbol to this one.
    virtual PlotSymbol& operator=(const PlotSymbol& rh);
    
    // Returns true if this PlotSymbol is equal to the given; false otherwise.
    virtual bool operator==(const PlotSymbol& rh) const;

    // Returns true if this PlotSymbol is NOT equal to the given; false
    // otherwise.
    virtual bool operator!=(const PlotSymbol& rh) const;
    
protected:
    // casacore::Record key names.
    // <group>
    static const casacore::String REC_WIDTH;         // double
    static const casacore::String REC_HEIGHT;        // double
    static const casacore::String REC_HEIGHTISPIXEL; // bool
    static const casacore::String REC_SYMBOL;        // int
    static const casacore::String REC_UCHAR;         // int (no ushort in Records)
    static const casacore::String REC_LINE;          // Record
    static const casacore::String REC_AREAFILL;      // Record
    static const casacore::String REC_COLOR;			//String
    // </group>

private:
    const casacore::String DEFAULT_COLOR;
    casacore::String currentColor;
};
typedef casacore::CountedPtr<PlotSymbol> PlotSymbolPtr;


//////////////////////////////
// CONCRETE UTILITY CLASSES //
//////////////////////////////

// casacore::Coordinate on the canvas surface (i.e., the part where the actual plots
// are, which doesn't include things like axes, titles, etc.).  A coordinate
// has two values and a system.
class PlotCoordinate {
public:
    // Static //
    
    // casacore::Coordinate system.
    enum System {
        WORLD,            // in the units of the axes
        NORMALIZED_WORLD, // [0 ... 1] value that maps to a "percentage" of
                          // world units
        PIXEL             // pixels right and below the upper left corner of
                          // the canvas
    };
    
    
    // Non-Static //
    
    // Default constructor.
    PlotCoordinate();
    
    // Parameterized constructor.
    PlotCoordinate(double dx, double dy, System s = WORLD);
    
    // Copy constructor.
    PlotCoordinate(const PlotCoordinate& c);
    
    // Destructor.
    ~PlotCoordinate();
    
    
    // Accessors //
    
    // Returns the coordinate system.
    System system() const;
    
    // Returns the x value.
    double x() const;
    
    // Returns the y value.
    double y() const;
    
    
    // Operators //
    
    // Assigns the value of the given PlotCoordinate to this one.
    PlotCoordinate& operator=(const PlotCoordinate& rh);
    
    // Returns true if this PlotCoordinate is equal to the given; false
    // otherwise.
    bool operator==(const PlotCoordinate& rh) const;
    
    // Returns true if this PlotCoordinate is NOT equal to the given; false
    // otherwise.
    bool operator!=(const PlotCoordinate& rh) const;
    
private:
    // casacore::Coordinate system.
    System m_system;
    
    // X value.
    double m_x;
    
    // Y value.
    double m_y;
};


// A PlotRegion is basically just a wrapper for two PlotCoordinates: an upper
// left coordinate and a lower right coordinate.
class PlotRegion {
public:
    // Default constructor.
    PlotRegion();
    
    // Parameterized constructor.
    PlotRegion(const PlotCoordinate& upperLeft,
            const PlotCoordinate& lowerRight);
    
    // Copy constructor.
    PlotRegion(const PlotRegion& copy);
    
    // Destructor.
    ~PlotRegion();
    
    
    // Returns the upper left coordinate.
    const PlotCoordinate& upperLeft() const;
    
    // Returns the lower right coordinate.
    const PlotCoordinate& lowerRight() const;
    
    // Returns the top y value.
    double top() const;
    
    // Returns the bottom y value.
    double bottom() const;
    
    // Returns the left x value.
    double left() const;
    
    // Returns the right x value.
    double right() const;
    
    // Returns true if the region is valid, false otherwise.
    bool isValid() const;
    
private:
    // Upper-left coordinate.
    PlotCoordinate m_upperLeft;
    
    // Lower-right coordinate.
    PlotCoordinate m_lowerRight;
};


// A PlotAxesStack is basically a list of PlotRegions as well as axis
// information that provides stack functionality such as a current index, and
// moving up and down the stack.  A valid stack has a "base" followed by zero
// or more PlotRegions.  A stack can optionally have a limit on its length;
// adding to the stack after it reaches its length will remove the oldest
// members after the base.  The smallest value this limit can be is 2, (base +
// 1 value).
class PlotAxesStack {
public:
    // Constructor for empty stack.  Length limits with values <= 1 mean no
    // limit.
    PlotAxesStack(int lengthLimit = -1);
    
    // Destructor.
    ~PlotAxesStack();
    
    
    // Gets/Sets the length limit on this stack.  Values <= 1 mean no limit.
    // <group>
    int lengthLimit() const;
    void setLengthLimit(int lengthLimit);
    void clearLengthLimit() { setLengthLimit(-1); }
    // </group>
    
    // Returns whether the stack is valid (has size > 0) or not.
    bool isValid() const;
    
    // Returns the current stack index.
    unsigned int stackIndex() const;
    
    // Returns the stack size.
    unsigned int size() const;
    
    // Returns a copy of the stack.
    std::vector<PlotRegion> stack() const;
    
    // Returns a copy of the stack axes.  The first item is for x, the second
    // for y.
    std::vector<std::pair<PlotAxis, PlotAxis> > stackAxes() const;
    
    // Resets the stack and sets the stack base to the given.
    void setBase(const PlotRegion& base, PlotAxis xAxis, PlotAxis yAxis);
    
    // Adds the given region to the stack.
    void addRegion(const PlotRegion& region, PlotAxis xAxis, PlotAxis yAxis);
    
    // Clears the stack, including the base if keepBase is false.
    void clearStack(bool keepBase = false);
    
    // Returns the current region in the stack.
    PlotRegion currentRegion() const;
    
    // Returns the x-axis for the current region in the stack.
    PlotAxis currentXAxis() const;
    
    // Returns the y-axis for the current region in the stack.
    PlotAxis currentYAxis() const;
    
    // Moves the stack index the given delta.  If delta is negative, the index
    // goes backwards; if delta is positive, the index goes forward.  If delta
    // is zero, the index goes to the base.
    void move(int delta);
    
    // Moves the stack index the given delta (see move()) and returns the
    // current region.
    PlotRegion moveAndReturn(int delta);
    
private:
    // Length limit.
    int m_lengthLimit;
    
    // Region stack.
    std::vector<PlotRegion> m_stack;
    
    // Axes stack.
    std::vector<std::pair<PlotAxis, PlotAxis> > m_axes;
    
    // casacore::Stack index.
    unsigned int m_stackIndex;
    
    
    // Shrinks the region and axes stack to the given size, discarding the
    // oldest UNLESS the stack index is in the elements to be discarded.  In
    // this case, the element referenced by the index is also kept.
    void shrinkStacks(unsigned int n);
};


// PlotExportFormat contains parameters for exporting a canvas to a file.
class PlotExportFormat {
public:
    // Static //
    
    // The type of file/export.
    enum Type {
        JPG, PNG, PS, PDF, TEXT,
        NUM_FMTS
    };
    
    // Whether to have high resolution or not.
    enum Resolution {
        SCREEN, // basically a screen shot
        HIGH    // "high" resolution
    };
    
    // Converts to/from a Type and its casacore::String representation.
    // <group>
    static casacore::String exportFormat(Type t);
    static Type exportFormat(casacore::String t, bool* ok = NULL);
    // </group>
    
    // Gives/converts the standard extension for export types.
    // <group>
    static casacore::String extensionFor(Type t);   
    static Type typeForExtension(casacore::String file, bool* ok = NULL);
    // </group>
    
    // Returns all supported export formats.
    // <group>
    static std::vector<Type> supportedFormats();  
    static std::vector<casacore::String> supportedFormatStrings();
    // </group>
    
    // Returns all supported image formats.
    // <group>
    static std::vector<Type> supportedImageFormats();
    static std::vector<casacore::String> supportedImageFormatStrings();
    // </group>
    
    
    // Non-Static //
    
    // Sets up a format with the given type and location.  Default resolution
    // is SCREEN; default dpi is -1 (unspecified); default width and height
    // are -1 (unspecified).  Unspecified values are left up to the plotting
    // implementation.
    PlotExportFormat(Type t, const casacore::String& file);
    
    // Destructor.
    ~PlotExportFormat();
    
    // Public Members
    // <group>
    Type type;             // export type
	bool verbose;          // verbose text export
    casacore::String location;       // export location
    Resolution resolution; // export resolution
    int dpi;               // export dpi (if applicable)
    int width;             // export width (if applicable)
    int height;            // export height (if applicable)
    // </group>
};


//////////////////////////
// SMART POINTER MACROS //
//////////////////////////

// This is painful but necessary to have transparent smart pointers that
// support hierarchies and inheritance.  See examples in other files.
// cname = class name (to declare smart pointer for),
// cptrname = name for pointer (usually cname + Ptr),
// pname = immediate parent class name
// pptrname = name for pointer of parent class (usually pname + Ptr),
// gname = "grandparent" class name (or highest smart pointer in hierarchy),
// gptrname = grandparent pointer name (usually gname + Ptr)
#define INHERITANCE_POINTER(cname, cptrname, pname, pptrname, gname, gptrname)\
    class cptrname : public pptrname {                                        \
    public:                                                                   \
        cptrname () : pptrname () { }                                         \
        cptrname ( cname * val, bool del = true ) : pptrname() {                           \
            gname * v = dynamic_cast< gname *>(val);                          \
            if(v != NULL) gptrname ::operator=( gptrname (v, del));           \
        }                                                                     \
        cptrname ( const gptrname & val ) : pptrname () {                     \
            const cname * v = dynamic_cast<const cname *>(                    \
                              val.operator->());                              \
            if(v != NULL) gptrname ::operator=(val);                          \
        }                                                                     \
        cname & operator*() {                                                 \
            return dynamic_cast< cname &>(**(( gptrname *)this));             \
        }                                                                     \
        cname * operator->() {                                                \
            return dynamic_cast< cname *>((( gptrname *)this)->operator->()); \
        }                                                                     \
        const cname & operator*() const {                                     \
            return dynamic_cast<const cname &>(**(( gptrname * )this));       \
        }                                                                     \
        const cname * operator->() const {                                    \
            return dynamic_cast<const cname *>(                               \
                   (( gptrname *)this)->operator->());                        \
        }                                                                     \
        cptrname & operator=(const gptrname & val) {                          \
            const cname * v = dynamic_cast<const cname *>(val.operator->());  \
            if(v != NULL) (( gptrname *)this)->operator=(val);                \
            return *this;                                                     \
        }                                                                     \
        cptrname & operator=( gname * val) {                                  \
            cname * v = dynamic_cast< cname *>(val);                          \
            if(v != NULL) (( gptrname *)this)->operator=(val);                \
            return *this;                                                     \
        }                                                                     \
    };

// Convenience macro.
#define INHERITANCE_POINTER2(cname, cptrname, pname, pptrname) \
    INHERITANCE_POINTER(cname, cptrname, pname, pptrname, pname, pptrname)

// Macro for when the child class is a template.
#define INHERITANCE_TPOINTER(cname, cptrname, pname, pptrname, gname,gptrname)\
    template <class T> class cptrname : public pptrname {                     \
    public:                                                                   \
        cptrname () : pptrname () { }                                         \
        cptrname ( cname <T>* val, bool del = true ) : pptrname(){            \
            gname * v = dynamic_cast< gname *>(val);                          \
            if(v != NULL) gptrname ::operator=( gptrname (v, del));           \
        }                                                                     \
        cptrname ( const gptrname & val ) : pptrname () {                     \
            const cname <T>* v = dynamic_cast<const cname <T>*>(              \
                              val.operator->());                              \
            if(v != NULL) gptrname ::operator=(val);                          \
        }                                                                     \
        cptrname ( const cptrname <T> & val ) : pptrname() {                               \
            gptrname ::operator=((const gptrname &)val);                      \
        }                                                                     \
        cname <T>& operator*() {                                              \
            return dynamic_cast< cname <T>&>(**(( gptrname *)this));          \
        }                                                                     \
        cname <T>* operator->() {                                             \
            return dynamic_cast< cname <T>*>(                                 \
                    (( gptrname *)this)->operator->());                       \
        }                                                                     \
        const cname <T>& operator*() const {                                  \
            return dynamic_cast<const cname <T>&>(**(( gptrname * )this));    \
        }                                                                     \
        const cname <T>* operator->() const {                                 \
            return dynamic_cast<const cname <T>*>(                            \
                   (( gptrname *)this)->operator->());                        \
        }                                                                     \
        cptrname <T>& operator=(const gptrname & val) {                       \
            const cname <T>* v = dynamic_cast<const cname <T>*>(              \
                                 val.operator->());                           \
            if(v != NULL) (( gptrname *)this)->operator=(val);                \
            return *this;                                                     \
        }                                                                     \
        cptrname <T>& operator=( gname * val) {                               \
            cname <T>* v = dynamic_cast< cname <T>*>(val);                    \
            if(v != NULL) (( gptrname *)this)->operator=(val);                \
            return *this;                                                     \
        }                                                                     \
    };

// Convenience macro.
#define INHERITANCE_TPOINTER2(cname, cptrname, pname, pptrname) \
    INHERITANCE_TPOINTER(cname, cptrname, pname, pptrname, pname, pptrname)

}

#endif /*PLOTOPTIONS_H_*/
