//# PlotCanvasLayout.h: Different layouts for PlotCanvases.
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
#ifndef PLOTCANVASLAYOUT_H_
#define PLOTCANVASLAYOUT_H_

#include <graphics/GenericPlotter/PlotCanvas.h>

namespace casa {

//# Forward Declarations
class Plotter;


//////////////////
// BASE CLASSES //
//////////////////

// A coordinate for layouts.  Each layout implementation should also provide
// a coordinate implementation.
class PlotLayoutCoordinate {
public:
    PlotLayoutCoordinate() { }
    
    virtual ~PlotLayoutCoordinate() { }
};

// Base class for any layout.
class PlotCanvasLayout {
public:
    // Constructor.
    PlotCanvasLayout() : m_plotter(NULL) { }
    
    // Destructor.
    virtual ~PlotCanvasLayout() { }
    
    
    // Returns the plotter this layout is used in, or NULL for none.
    virtual Plotter* plotter() const { return m_plotter; }
    
    // Attaches this layout to the given plotter.  The plotter's
    // layoutOptionsChanged method can then be called when the layout changes.
    virtual void attach(Plotter* p) { m_plotter = p; }
    
    
    // ABSTRACT METHODS //
    
    // Returns true if the given coordinate is valid for this layout.
    virtual bool coordIsValid(const PlotLayoutCoordinate& coord) const = 0;
    
    // turn the coordinate into an index.  the index should >= 0 and < the
    // total number of canvases in the layout.
    virtual int coordToIndex(const PlotLayoutCoordinate& coord) const = 0;
    
    // Sets the canvas at the given coordinate to the given canvas.
    virtual void setCanvasAt(const PlotLayoutCoordinate& coord,
            PlotCanvasPtr canvas) = 0;
    
    // Returns the canvas at the given coordinate.
    virtual PlotCanvasPtr canvasAt(const PlotLayoutCoordinate& coord) const =0;
    
    // For single layouts, returns the canvas; otherwise returns the "first".
    virtual PlotCanvasPtr canvas() const = 0;
    
    // Returns all canvases in this layout.
    virtual std::vector<PlotCanvasPtr> allCanvases() const = 0;
    
    // Indicates whether the layout is valid.
    virtual bool isValid() const = 0;
    
    // Gets/sets the spacing between the canvases in the layout.  May not be
    // valid for all layout types.  The implementation for Plotter should use
    // this attribute appropriately.
    // <group>
    virtual unsigned int spacing() const = 0;
    virtual void setSpacing(unsigned int spacing) = 0;
    // </group>
    
protected:
    Plotter* m_plotter; // Plotter
};


///////////////////////////
// SINGLE LAYOUT CLASSES //
///////////////////////////

// PlotLayoutSingle is basically just a wrapper for a single canvas.
class PlotLayoutSingle : public virtual PlotCanvasLayout {
public:
    // Constructor which takes the canvas.
    PlotLayoutSingle(PlotCanvasPtr c);
    
    // Destructor.
    ~PlotLayoutSingle();
    
    
    // Implements PlotCanvasLayout::coordIsValid().
    bool coordIsValid(const PlotLayoutCoordinate& ) const { return true; }
    
    // Implements PlotCanvasLayout::coordToIndex().
    int coordToIndex(const PlotLayoutCoordinate& ) const { return 0; }
    
    // Implements PlotCanvasLayout::setCanvasAt().
    void setCanvasAt(const PlotLayoutCoordinate& coord, PlotCanvasPtr c);
    
    // Sets this layout's canvas to the given.
    void setCanvas(PlotCanvasPtr canvas);
    
    // Implements PlotCanvasLayout::canvas().
    PlotCanvasPtr canvas() const;
    
    // Implements PlotCanvasLayout::canvasAt().
    PlotCanvasPtr canvasAt(const PlotLayoutCoordinate& coord) const;
    
    // Implements PlotCanvasLayout::allCanvases().
    std::vector<PlotCanvasPtr> allCanvases() const;
    
    // Implements PlotCanvasLayout::isValid().
    bool isValid() const;
    
    // Implements PlotCanvasLayout::spacing().
    unsigned int spacing() const { return 0; }
    
    // Implements PlotCanvasLayout::setSpacing().
    void setSpacing(unsigned int ) { }
    
protected:
    PlotCanvasPtr m_canvas; // Canvas.
};


/////////////////////////
// GRID LAYOUT CLASSES //
/////////////////////////

// casacore::Coordinate for a grid layout, which consists of a row and column.
class PlotGridCoordinate : public virtual PlotLayoutCoordinate {
public:
    PlotGridCoordinate(unsigned int r, unsigned int c): row(r), col(c) { }
    
    ~PlotGridCoordinate() { }
    
    unsigned int row;
    unsigned int col;
};

// An n x m grid of canvases.
class PlotLayoutGrid : public virtual PlotCanvasLayout {
public:
    // Constructor which takes the number of rows and columns.
    PlotLayoutGrid(unsigned int rows, unsigned int cols);    
    
    // Destructor.
    ~PlotLayoutGrid();
    
    // Returns the number of rows.
    unsigned int rows() const;
    
    // Returns the number of columns.
    unsigned int cols() const;
    
    // Implements PlotCanvasLayout::coordIsValid().
    bool coordIsValid(const PlotLayoutCoordinate& coord) const;
    
    // Implements PlotCanvasLayout::coordToIndex().
    int coordToIndex(const PlotLayoutCoordinate& coord) const;
    
    // Implements PlotCanvasLayout::setCanvasAt().
    void setCanvasAt(const PlotLayoutCoordinate& coord, PlotCanvasPtr canvas);
    
    // Implements PlotCanvasLayout::canvasAt().
    PlotCanvasPtr canvasAt(const PlotLayoutCoordinate& coord) const;
    
    // Implements PlotCanvasLayout::canvas().
    PlotCanvasPtr canvas() const;
    
    // Implements PlotCanvasLayout::allCanvases().
    std::vector<PlotCanvasPtr> allCanvases() const;
    
    // Implements PlotCanvasLayout::isValid().
    bool isValid() const;
    
    // Implements PlotCanvasLayout::spacing().
    unsigned int spacing() const;
    
    // Implements PlotCanvasLayout::setSpacing().
    void setSpacing(unsigned int spacing);
    
protected:
    unsigned int m_rows;                     // rows
    unsigned int m_cols;                     // columns
    std::vector<std::vector<PlotCanvasPtr> > m_panels; // canvases
    unsigned int m_spacing;                  // spacing
};


///////////////////////////////
// SMART POINTER DEFINITIONS //
///////////////////////////////

typedef casacore::CountedPtr<PlotCanvasLayout> PlotCanvasLayoutPtr;
INHERITANCE_POINTER2(PlotLayoutSingle, PlotLayoutSinglePtr, PlotCanvasLayout,
                     PlotCanvasLayoutPtr)
INHERITANCE_POINTER2(PlotLayoutGrid, PlotLayoutGridPtr, PlotCanvasLayout,
                     PlotCanvasLayoutPtr)

}

#endif /*PLOTCANVASLAYOUT_H_*/
