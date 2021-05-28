//# PlotAnnotation.h: Annotation class that can be drawn on a canvas.
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
#ifndef PLOTANNOTATION_H_
#define PLOTANNOTATION_H_

#include <graphics/GenericPlotter/PlotItem.h>

#include <casa/BasicSL/String.h>

namespace casa {

// PlotAnnotation is an abstraction of text written directly on the canvas.  An
// annotation has text, font, orientation, outline, and background properties.
class PlotAnnotation : public virtual PlotItem {
public:
    // Constructor.
    PlotAnnotation() { }
    
    // Destructor.
    virtual ~PlotAnnotation() { }
    
    
    // Implements PlotItem::drawCount().  Provides default implementation that
    // returns 1.
    virtual unsigned int drawCount() const { return 1; }
    
    // ABSTRACT METHODS //
    
    // Returns the text of the annotation.
    virtual casacore::String text() const = 0;
    
    // Sets the text of the annotation.
    virtual void setText(const casacore::String& newText) = 0;
    
    // Returns a copy of the font used in the annotation.
    virtual PlotFontPtr font() const = 0;
    
    // Sets the annotation font to the given.
    virtual void setFont(const PlotFont& font) = 0;
    
    // Returns orientation in counterclockwise degrees (between 0 and 360).
    virtual int orientation() const = 0;
    
    // Sets the orientation in counterclockwise degrees (between 0 and 360).
    virtual void setOrientation(int orientation) = 0;
    
    // Returns the coordinate where the annotation is located on the canvas it
    // is attached to.
    virtual PlotCoordinate coordinate() const = 0;
    
    // Sets the location of the annotation to the given.
    virtual void setCoordinate(const PlotCoordinate& coord) = 0;
    
    // Returns true if an outline is shown around the annotation, false
    // otherwise.
    virtual bool outlineShown() const = 0;
    
    // Sets whether an outline is shown around the annotation.
    virtual void setOutlineShown(bool show = true) = 0;
    
    // Returns a copy of the line used to draw the outline for this annotation.
    virtual PlotLinePtr outline() const = 0;
    
    // Sets the outline to the given line.
    virtual void setOutline(const PlotLine& line) = 0;
    
    // Returns a copy of the area fill used for the annotation's background.
    virtual PlotAreaFillPtr background() const = 0;
    
    // Sets the annotation's background to the given.
    virtual void setBackground(const PlotAreaFill& area) = 0;
    
    
    // IMPLEMENTED METHODS //
    
    // Convenience methods for setting font.
    // <group>
    virtual void setFont(const PlotFontPtr font) {
        if(!font.null()) setFont(*font); }
    virtual void setFontColor(const casacore::String& color) {
        PlotFontPtr f = font();
        f->setColor(color);
        setFont(*f);
    }
    // </group>
    
    // Convenience methods for setting the outline.
    // <group>
    virtual void setOutline(const PlotLinePtr line) {
        if(!line.null()) setOutline(*line);
        else             setOutlineShown(false);
    }
    virtual void setOutline(const casacore::String& color,
            PlotLine::Style style = PlotLine::SOLID, double width = 1.0) {
        PlotLinePtr line = outline();
        line->setColor(color);
        line->setStyle(style);
        line->setWidth(width);
        setOutline(*line);
    }
    // </group>
    
    // Convenience methods for setting the background.
    // <group>
    virtual void setBackground(const PlotAreaFillPtr area) {
        if(!area.null()) setBackground(*area); }
    virtual void setBackground(const casacore::String& color,
            PlotAreaFill::Pattern pattern = PlotAreaFill::FILL) {
        PlotAreaFillPtr bg = background();
        bg->setColor(color);
        bg->setPattern(pattern);
        setBackground(*bg);
    }
    // </group>
};
INHERITANCE_POINTER2(PlotAnnotation, PlotAnnotationPtr, PlotItem, PlotItemPtr)

}

#endif /*PLOTANNOTATION_H_*/
