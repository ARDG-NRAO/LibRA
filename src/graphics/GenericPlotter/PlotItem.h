//# PlotItem.h: Top of hierarchy for objects that can be drawn on a canvas.
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
#ifndef PLOTITEM_H_
#define PLOTITEM_H_

#include <graphics/GenericPlotter/PlotOptions.h>

namespace casa {

//# Forward Declarations
class PlotCanvas;

// PlotItem is the superclass of any object that is meant to be placed on the
// canvas (plots, annotations, shapes, etc.).  A PlotItem is assumed to have an
// x and y axis for drawing on the canvas.
class PlotItem {
public:
    // Constructor.
    PlotItem();
    
    // Destructor.
    virtual ~PlotItem();

    
    // Returns the canvas this item is currently attached to, or NULL for none.
    virtual PlotCanvas* canvas() const = 0;
    
    // Returns a human-readable title for identification.  Doesn't have to
    // be unique. Used for (for example) legends and other user-interactions.
    // Note: PlotItems that have no titles should not be shown on the legend.
    virtual casacore::String title() const = 0;
    
    // Sets this items's title to the given.
    virtual void setTitle(const casacore::String& newTitle) = 0;
    
    // Returns true if this item is valid, false otherwise.  Although validity
    // varies between items, canvases should NOT draw items which are invalid.
    virtual bool isValid() const = 0;
    
    // Returns whether this item can be directly casted to a QWidget or not.
    virtual bool isQWidget() const = 0;
    
    // Returns the item's axes.
    // <group>
    virtual PlotAxis xAxis() const = 0;
    virtual PlotAxis yAxis() const = 0;
    // </group>

    // Sets the item's axes.
    // <group>
    virtual void setXAxis(PlotAxis x) = 0;
    virtual void setYAxis(PlotAxis y) = 0;
    // </group>
    
    // Sets both the item's axes at once.
    // DEFAULT IMPLEMENTATION.
    virtual void setAxes(PlotAxis x, PlotAxis y);
    
    // Returns the "draw count" for this item, which subjectively means how
    // many indexed draw operations this item needs to make.  This is used to
    // make comparative judgments about how long each item will take to draw,
    // and to possibly divide up the work for larger items.  For example, a
    // plot with 1000 data points might return 1000, while a square might
    // return 1.  Whatever number is returned, the item should expect to be
    // able to draw given two indexes; for example, a plot should be able to
    // draw from index 0 to index 100.
    virtual unsigned int drawCount() const = 0;
    
    // Returns the number of draw segments for this item, using the given
    // segment threshold.  See DrawCount().
    // DEFAULT IMPLEMENTATION.
    virtual unsigned int drawSegments(unsigned int segmentThreshold) const;
};
typedef casacore::CountedPtr<PlotItem> PlotItemPtr;

}

#endif /*PLOTITEM_H_*/
