//# PlotShape.h: Different shape classes that can be drawn on a canvas.
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
#ifndef PLOTSHAPE_H_
#define PLOTSHAPE_H_

#include <graphics/GenericPlotter/PlotItem.h>

#include <vector>

namespace casa {

// Abstract class for any shape which contains common functionality.
class PlotShape : public virtual PlotItem {
public:
    // Constructor.
    PlotShape() { }
    
    // Destructor.
    virtual ~PlotShape() { }
    
    
    // Implements PlotItem::drawCount().  Provides default implementation that
    // returns 1.
    virtual unsigned int drawCount() const { return 1; }
    
    
    // ABSTRACT METHODS //
    
    // Returns the coordinates for this shape.  The number and order depends
    // on the specific shape subclass.
    virtual std::vector<PlotCoordinate> coordinates() const = 0;
    
    // Sets this shape's coordinates to the given.  Must be in the same size
    // and order as that returned by coordinates().  Specific to each shape
    // subclass.
    virtual void setCoordinates(const std::vector<PlotCoordinate>& c) = 0;
    
    // Returns true if a line is shown for this shape, false otherwise.
    virtual bool lineShown() const = 0;
    
    // Sets whether a line is shown for this shape or not.
    virtual void setLineShown(bool line = true) = 0;
    
    // Returns a copy of the line used for this shape.
    virtual PlotLinePtr line() const = 0;
    
    // Sets this shape's line to the given.
    virtual void setLine(const PlotLine& line) = 0;
    
    // Returns true if this shape has an area fill, false otherwise.
    virtual bool areaFilled() const = 0;
    
    // Sets whether or not this shape has an area fill.
    virtual void setAreaFilled(bool area = true) = 0;
    
    // Returns a copy of the area fill for this shape.
    virtual PlotAreaFillPtr areaFill() const = 0;
    
    // Sets this shape's area fill to the given.
    virtual void setAreaFill(const PlotAreaFill& fill) = 0;
    
    
    // IMPLEMENTED METHODS //
    
    // Convenience methods for setting the line for this shape.
    // <group>
    virtual void setLine(const PlotLinePtr l) {
        if(!l.null()) setLine(*l);
        else          setLineShown(false);
    }
    virtual void setLine(const casacore::String& color,
            PlotLine::Style style = PlotLine::SOLID, double width = 1.0) {
        PlotLinePtr l = line();
        l->setColor(color);
        l->setStyle(style);
        l->setWidth(width);
        setLine(*l);
    }
    // </group>
    
    // Convenience methods for setting the area fill.
    // <group>
    virtual void setAreaFill(const PlotAreaFillPtr f) {
        if(!f.null()) setAreaFill(*f);
        else          setAreaFilled(false);
    }
    virtual void setAreaFill(const casacore::String& color,
            PlotAreaFill::Pattern pattern = PlotAreaFill::FILL) {
        PlotAreaFillPtr f = areaFill();
        f->setColor(color);
        f->setPattern(pattern);
        setAreaFill(*f);
    }
    // </group>
};


// Specialization of PlotShape for a rectangle.  Getting/setting coordinates
// MUST be in the order: [upperLeft, lowerRight].
class PlotShapeRectangle : public virtual PlotShape {
public:
    // Constructor.
    PlotShapeRectangle() { }
    
    // Destructor.
    virtual ~PlotShapeRectangle() { }
    
    // Sets the rectangle coordinates to the given.
    virtual void setRectCoordinates(const PlotCoordinate& upperLeft,
                                    const PlotCoordinate& lowerRight) = 0;
};


// Specialization of PlotShape for an ellipse.  Getting/setting coordinates
// MUST be in the order: [center, radii].
class PlotShapeEllipse : public virtual PlotShape {
public:
    // Constructor.
    PlotShapeEllipse() { }
    
    // Destructor.
    virtual ~PlotShapeEllipse() { }
    
    
    // Sets the ellipse coordinates to the given.
    virtual void setEllipseCoordinates(const PlotCoordinate& center,
                                       const PlotCoordinate& radii) = 0;
    
    // Returns the x- and y- radii as a PlotCoordinate.
    virtual PlotCoordinate radii() const = 0;
    
    // Sets the x- and y- radii to the given.
    virtual void setRadii(const PlotCoordinate& radii) = 0;
    
    // Returns the center point.
    virtual PlotCoordinate center() const = 0;
    
    // Sets the center to the given.
    virtual void setCenter(const PlotCoordinate& center) = 0;
};


// Specialization of PlotShape for a polygon.
class PlotShapePolygon : public virtual PlotShape {
public:
    // Constructor.
    PlotShapePolygon() { }
    
    // Destructor.
    virtual ~PlotShapePolygon() { }
    
    // Overrides PlotShape::drawCount().  Provides default implementation that
    // returns the number of vertices.
    virtual unsigned int drawCount() const { return coordinates().size(); }
};


// Specialization of PlotShape for a line.  A line consists of an axis and a
// location.  For example, a line at 5 on the X_BOTTOM axis would draw a
// continuous line at x = 5.  Getting/setting coordinates MUST be in the order:
// [location], where location has the value for x for X_BOTTOM or X_TOP or as y
// for Y_LEFT or Y_RIGHT and is in world coordinates.
class PlotShapeLine : public virtual PlotShape {
public:
    // Constructor.
    PlotShapeLine() { }
    
    // Destructor.
    virtual ~PlotShapeLine() { }

    // Sets the line location to the given.
    virtual void setLineCoordinates(double location, PlotAxis axis) = 0;
    
    // Returns the line location.
    virtual double location() const = 0;
    
    // Returns the line axis.
    virtual PlotAxis axis() const = 0;
};


// Specialization of PlotShape for an arrow.  An arrow is a line segment
// connecting two points, with optional arrows at either end.  PlotShape's line
// methods apply both to the line segment and the outline of the arrow; the
// areaFill methods apply to the arrow if applicable.  Getting/setting
// coordinates MUST be in the order: [from, to].
class PlotShapeArrow : public virtual PlotShape {
public:
    // Arrow style.
    enum Style {
        TRIANGLE, // Filled triangle, 45-degree angle
        V_ARROW,  // Two lines forming a V, 45-degree angle
        NOARROW   // No arrow
    };
    
    
    // Constructor.
    PlotShapeArrow() { }
    
    // Destructor.
    virtual ~PlotShapeArrow() { }

    // Sets the arrow coordinates to the given.
    virtual void setArrowCoordinates(const PlotCoordinate& from,
                                     const PlotCoordinate& to) = 0;
    
    // Gets the arrow style on the from/to points.
    // <group>
    virtual Style arrowStyleFrom() const = 0;
    virtual Style arrowStyleTo() const = 0;
    // </group>
    
    // Sets the arrow style(s) to the given.
    // <group>
    virtual void setArrowStyleFrom(Style style) = 0;
    virtual void setArrowStyleTo(Style style) = 0;
    virtual void setArrowStyles(Style from, Style to) {
        setArrowStyleFrom(from);
        setArrowStyleTo(to);
    }
    // </group>
    
    // Returns the arrow size/length.
    virtual double arrowSize() const = 0;
    
    // Sets the arrow size/length to the given.
    virtual void setArrowSize(double size) = 0;
};


// Specialization of PlotShape for a path.  A path is a series of lines
// connecting the given points.  (Like a polygon, but not connecting the first
// and last points, or filled in.)
class PlotShapePath : public virtual PlotShape {
public:
    // Constructor.
    PlotShapePath() { }
    
    // Destructor.
    virtual ~PlotShapePath() { }
    
    // Overrides PlotShape::drawCount().  Provides default implementation that
    // returns the number of path points.
    virtual unsigned int drawCount() const { return coordinates().size(); }
};


// Specialization of PlotShape for an arc.  An arc has a start coordinate,
// width, height, start angle, span angle, and orientation.  Getting/setting
// coordinates MUST be in the order: [start, widthHeight].  The other
// attributes must be set manually.
class PlotShapeArc : public virtual PlotShape {
public:
    // Constructor.
    PlotShapeArc() { }
    
    // Destructor.
    virtual ~PlotShapeArc() { }

    
    // Returns the start coordinate.
    virtual PlotCoordinate startCoordinate() const = 0;
    
    // Sets the start coordinate to the given.
    virtual void setStartCoordinate(const PlotCoordinate& coord) = 0;
    
    // Returns the width and height as a PlotCoordinate.
    virtual PlotCoordinate widthHeight() const = 0;
    
    // Sets the width and height to the given.
    virtual void setWidthHeight(double width, double height) {
        setWidthHeight(PlotCoordinate(width, height, PlotCoordinate::WORLD)); }
    
    // Sets the width and height as a PlotCoordinate.
    virtual void setWidthHeight(const PlotCoordinate& widthHeight) = 0;
    
    // Returns the start angle.
    virtual int startAngle() const = 0;
    
    // Sets the start angle.
    virtual void setStartAngle(int startAngle) = 0;
    
    // Returns the span angle.
    virtual int spanAngle() const = 0;
    
    // Sets the span angle.
    virtual void setSpanAngle(int spanAngle) = 0;
    
    // Returns the orientation.
    virtual int orientation() const = 0;
    
    // Sets the orientation.
    virtual void setOrientation(int o) = 0;
};


// Abstract class for a single point on the canvas (not descended from
// PlotShape).
class PlotPoint : public virtual PlotItem {
public:
    // Constructor.
    PlotPoint() { }
    
    // Destructor.
    virtual ~PlotPoint() { }
    
    
    // Implements PlotItem::drawCount().  Provides default implementation that
    // returns 1.
    virtual unsigned int drawCount() const { return 1; }
    
    
    // ABSTRACT METHODS //
    
    // Returns the location of the point.
    virtual PlotCoordinate coordinate() const = 0;
    
    // Sets the location of the point to the given.
    virtual void setCoordinate(const PlotCoordinate& coordinate) = 0;
    
    // Returns a copy of the symbol used to draw the point.
    virtual PlotSymbolPtr symbol() const = 0;
 
    // Sets the symbol used to draw the point.
    virtual void setSymbol(const PlotSymbol& symbol) = 0;
    
    
    // IMPLEMENTED METHODS //
    
    // Convenience methods for setting the symbol.
    // </group>
    virtual void setSymbol(const PlotSymbolPtr symbol) {
        if(!symbol.null()) setSymbol(*symbol); }
    virtual void setSymbol(PlotSymbol::Symbol sym) {
        PlotSymbolPtr s = symbol();
        s->setSymbol(sym);
        setSymbol(*s);
    }
    // </group>
};


///////////////////////////////
// SMART POINTER DEFINITIONS //
///////////////////////////////

INHERITANCE_POINTER2(PlotShape, PlotShapePtr, PlotItem, PlotItemPtr)
INHERITANCE_POINTER(PlotShapeRectangle, PlotShapeRectanglePtr, PlotShape,
                    PlotShapePtr, PlotItem, PlotItemPtr)
INHERITANCE_POINTER(PlotShapeEllipse, PlotShapeEllipsePtr, PlotShape,
                    PlotShapePtr, PlotItem, PlotItemPtr)
INHERITANCE_POINTER(PlotShapePolygon, PlotShapePolygonPtr, PlotShape,
                    PlotShapePtr, PlotItem, PlotItemPtr)
INHERITANCE_POINTER(PlotShapeLine, PlotShapeLinePtr, PlotShape, PlotShapePtr,
                    PlotItem, PlotItemPtr)
INHERITANCE_POINTER(PlotShapeArrow, PlotShapeArrowPtr, PlotShape,
                    PlotShapePtr, PlotItem, PlotItemPtr)
INHERITANCE_POINTER(PlotShapePath, PlotShapePathPtr, PlotShape, PlotShapePtr,
                    PlotItem, PlotItemPtr)
INHERITANCE_POINTER(PlotShapeArc, PlotShapeArcPtr, PlotShape, PlotShapePtr,
                    PlotItem, PlotItemPtr)
INHERITANCE_POINTER2(PlotPoint, PlotPointPtr, PlotItem, PlotItemPtr)

}

#endif /*PLOTSHAPE_H_*/
