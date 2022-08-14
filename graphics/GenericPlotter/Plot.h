//# Plot.h: Plot objects to be drawn on a canvas.
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
#ifndef PLOT_H_
#define PLOT_H_

#include <graphics/GenericPlotter/PlotData.h>
#include <graphics/GenericPlotter/PlotItem.h>

#include <casacore/casa/BasicSL/String.h>

namespace casa {

// A Plot basically consists of data and customization information.  The data
// is held by the base class whereas any customization should be handled in
// subclasses.
class Plot : public virtual PlotItem {
public:
    Plot() { }

    virtual ~Plot() { }
    
    
    // ABSTRACT METHODS //
    
    // Returns the data associated with this plot.
    virtual PlotDataPtr data() const = 0;
    
    // Should be called whenever the underlying PlotData has changed, to let
    // the plot and its attached canvas know that the plot needs to be redrawn.
    virtual void dataChanged() = 0;
    
    // It's likely that any plot subclass (scatter, bar, function, etc.) will
    // have lines of some sort, so it's safe to have in the base class.
    
    // Returns true if this plot has lines shown, false otherwise.
    virtual bool linesShown() const = 0;
    
    // Sets whether or not lines are shown.  If linesShown is true, the
    // implementation can decide whether to choose the line style shown, or
    // just use the last set PlotLine.
    virtual void setLinesShown(bool linesShown = true) = 0;
    
    // Returns a copy of the line used for this plot.  Note: if lines are not
    // shown, then this behavior is undefined.  The last shown line can be
    // returned, or a line with style NOLINE, or a null pointer, or....
    virtual PlotLinePtr line() const = 0;
    
    // Sets the plot lines to the given line.  Implies setLinesShown(true)
    // unless the given line's style is set to NOLINE.
    virtual void setLine(const PlotLine& line) = 0;
    
    
    // IMPLEMENTED METHODS //
    
    // Convenience methods for setting line.
    // <group>
    virtual void setLine(const PlotLinePtr line) {
        if(!line.null()) setLine(*line);
        else             setLinesShown(false);
    }
    virtual void setLine(const casacore::String& color,
                         PlotLine::Style style = PlotLine::SOLID,
                         double width = 1.0) {
        PlotLinePtr l = line();
        l->setColor(color);
        l->setStyle(style);
        l->setWidth(width);
        setLine(*l);
    }
    virtual void setLineColor(const casacore::String& color) {
        PlotLinePtr l = line();
        l->setColor(color);
        setLine(*l);
    }
    virtual void setLineStyle(PlotLine::Style style) {
        PlotLinePtr l = line();
        l->setStyle(style);
        setLine(*l);
    }
    virtual void setLineWidth(double width) {
        PlotLinePtr l = line();
        l->setWidth(width);
        setLine(*l);
    }
    // </group>
};


// Scatter plot abstract class.  In addition to the abstract methods in Plot, a
// scatter plot subclass should also have symbol customization for the points.
// A scatter plot is expected to consist of PlotPointData.
class ScatterPlot : public virtual Plot {
public:
    ScatterPlot() { }

    virtual ~ScatterPlot() { }
    
    
    // Overrides Plot::data().
    virtual PlotDataPtr data() const { return pointData(); }
    
    // Returns the data value at the given index.  Just a thin layer over the
    // PlotPointData functionality.
    virtual ppoint_t pointAt(unsigned int i) const {
        PlotPointDataPtr data = pointData();
        if(!data.null()) return ppoint_t(data->xAt(i), data->yAt(i));
        else             return ppoint_t(0, 0);
    }
    
    // Implements PlotItem::drawCount().  Provides default implementation that
    // returns the number of plotted points.
    virtual unsigned int drawCount() const { return pointData()->size(); }
    
    
    // ABSTRACT METHODS //
    
    // Returns the point data used for this plot.
    virtual PlotPointDataPtr pointData() const = 0;
    
    // Get/set to connect points in step rather than line
    virtual bool linesStep() const = 0;
    virtual void setLinesStep(bool linesStep = true) = 0;
    
    // Returns true if symbols are shown, false otherwise.
    virtual bool symbolsShown() const = 0;
    
    // Sets whether symbols are shown or not.  If symbolsShown is true, the
    // implementation can decide whether to choose the symbol shown, or just
    // use the last set PlotSymbol.
    virtual void setSymbolsShown(bool symbolsShown = true) = 0;
    
    // Returns a copy of the symbol for this plot.  Note: if symbols are not
    // shown, then this behavior is undefined.  The last shown symbol can be
    // returned, or a symbol with style NOSYMBOL, or a null pointer, or....
    virtual PlotSymbolPtr symbol() const = 0;
    
    // Sets the plot symbols to the given symbol. Implies setSymbolsShown(true)
    // unless the symbol's style is set to NOSYMBOL.
    virtual void setSymbol(const PlotSymbol& symbol) = 0;
    
    
    // IMPLEMENTED METHODS //
    
    // Convenience methods for setting symbol.
    // <group>
    virtual void setSymbol(const PlotSymbolPtr symbol) {
        if(!symbol.null()) setSymbol(*symbol);
        else               setSymbolsShown(false);
    }
    virtual void setSymbol(PlotSymbol::Symbol s) {
        PlotSymbolPtr sym = symbol();
        sym->setSymbol(s);
        setSymbol(*sym);
    }
    virtual void setSymbol(char s) {
        PlotSymbolPtr sym = symbol();
        sym->setSymbol(s);
        setSymbol(*sym);
    }
    virtual void setSymbolSize(double width, double height) {
        PlotSymbolPtr sym = symbol();
        sym->setSize(width, height);
        setSymbol(*sym);
    }
    virtual void setSymbolLine(const PlotLine& line) {
        PlotSymbolPtr sym = symbol();
        sym->setLine(line);
        setSymbol(*sym);
    }
    virtual void setSymbolLine(const PlotLinePtr line) {
        if(!line.null()) setSymbolLine(*line); }
    virtual void setSymbolLine(const casacore::String& color,
            PlotLine::Style style = PlotLine::SOLID, double width = 1.0) {
        PlotSymbolPtr sym = symbol();
        sym->setLine(color, style, width);
        setSymbol(*sym);
    }
    virtual void setSymbolAreaFill(const PlotAreaFill& fill) {
        PlotSymbolPtr sym = symbol();
        sym->setAreaFill(fill);
        setSymbol(*sym);
    }
    virtual void setSymbolAreaFill(const PlotAreaFillPtr fill) {
        if(!fill.null()) setSymbolAreaFill(*fill); }
    virtual void setSymbolAreaFill(const casacore::String& color,
            PlotAreaFill::Pattern pattern = PlotAreaFill::FILL) {
        PlotSymbolPtr sym = symbol();
        sym->setAreaFill(color, pattern);
        setSymbol(*sym);
    }
    // </group>
};


// Subclass of ScatterPlot that adds masking functionality.  The ScatterPlot
// customization methods (lines, symbols, etc.) are expected to apply only to
// unmasked data, while additional methods are provided in MaskedScatterPlot
// to customize the masked data (which is not shown by default).
class MaskedScatterPlot : public virtual ScatterPlot {
public:
    MaskedScatterPlot() { }
    
    virtual ~MaskedScatterPlot() { }
    
    
    // Overrides ScatterPlot::pointData().
    virtual PlotPointDataPtr pointData() const { return maskedData(); }
    
    // Returns whether the data at the given index is masked or not.  Just a
    // thin layer over the PlotMaskedPointData functionality.
    virtual bool maskedAt(unsigned int index) const {
        PlotMaskedPointDataPtr data = maskedData();
        return !data.null() && data->maskedAt(index);
    }
    
    
    // ABSTRACT METHODS //
    
    // Returns the masked data used for this plot.
    virtual PlotMaskedPointDataPtr maskedData() const = 0;
    virtual void clearData() = 0;

    // Returns true if this plot has lines shown for masked points, false
    // otherwise.
    virtual bool maskedLinesShown() const = 0;
    
    // Sets whether or not lines are shown for masked points.  If linesShown is
    // true, the implementation can decide whether to choose the line style
    // shown, or just use the last set PlotLine.
    virtual void setMaskedLinesShown(bool linesShown = true) = 0;
    
    // Returns a copy of the line used for masked points.  Note: if lines are
    // not shown, then this behavior is undefined.  The last shown line can be
    // returned, or a line with style NOLINE, or a null pointer, or....
    virtual PlotLinePtr maskedLine() const = 0;
    
    // Sets the lines for masked points to the given.  Implies
    // setMaskedLinesShown(true) unless the given line's style is set to
    // NOLINE.
    virtual void setMaskedLine(const PlotLine& line) = 0;
    
    // Convenience methods for setting line for masked points.
    // <group>
    virtual void setMaskedLine(const PlotLinePtr line) {
        if(!line.null()) setMaskedLine(*line);
        else             setMaskedLinesShown(false);
    }
    virtual void setMaskedLine(const casacore::String& color,
                         PlotLine::Style style = PlotLine::SOLID,
                         double width = 1.0) {
        PlotLinePtr l = maskedLine();
        l->setColor(color);
        l->setStyle(style);
        l->setWidth(width);
        setMaskedLine(*l);
    }
    virtual void setMaskedLineColor(const casacore::String& color) {
        PlotLinePtr l = maskedLine();
        l->setColor(color);
        setMaskedLine(*l);
    }
    virtual void setMaskedLineStyle(PlotLine::Style style) {
        PlotLinePtr l = maskedLine();
        l->setStyle(style);
        setMaskedLine(*l);
    }
    virtual void setMaskedLineWidth(double width) {
        PlotLinePtr l = maskedLine();
        l->setWidth(width);
        setMaskedLine(*l);
    }
    // Get/set connect masked points in step rather than line
    virtual bool maskedLinesStep() const = 0;
    virtual void setMaskedLinesStep(bool linesStep = true) = 0;
    
    // </group>
    
    // Returns true if symbols are shown for masked points, false otherwise.
    virtual bool maskedSymbolsShown() const = 0;
    
    // Sets whether symbols are shown or not for masked points.  If
    // symbolsShown is true, the implementation can decide whether to choose
    // the symbol shown, or just use the last set PlotSymbol.
    virtual void setMaskedSymbolsShown(bool symbolsShown = true) = 0;
    
    // Returns a copy of the symbol for masked points.  Note: if symbols are
    /// not shown, then this behavior is undefined.  The last shown symbol can
    // be returned, or a symbol with style NOSYMBOL, or a null pointer, or....
    virtual PlotSymbolPtr maskedSymbol() const = 0;
    
    // Sets the symbols for masked points to the given. Implies
    // setMaskedSymbolsShown(true) unless the symbol's style is set to
    // NOSYMBOL.
    virtual void setMaskedSymbol(const PlotSymbol& symbol) = 0;
    
    // Convenience methods for setting symbol for masked points.
    // <group>
    virtual void setMaskedSymbol(const PlotSymbolPtr symbol) {
        if(!symbol.null()) setMaskedSymbol(*symbol);
        else               setMaskedSymbolsShown(false);
    }
    virtual void setMaskedSymbol(PlotSymbol::Symbol s) {
        PlotSymbolPtr sym = maskedSymbol();
        sym->setSymbol(s);
        setMaskedSymbol(*sym);
    }
    virtual void setMaskedSymbol(char s) {
        PlotSymbolPtr sym = maskedSymbol();
        sym->setSymbol(s);
        setMaskedSymbol(*sym);
    }
    virtual void setMaskedSymbolSize(double width, double height) {
        PlotSymbolPtr sym = maskedSymbol();
        sym->setSize(width, height);
        setMaskedSymbol(*sym);
    }
    virtual void setMaskedSymbolLine(const PlotLine& line) {
        PlotSymbolPtr sym = maskedSymbol();
        sym->setLine(line);
        setMaskedSymbol(*sym);
    }
    virtual void setMaskedSymbolLine(const PlotLinePtr line) {
        if(!line.null()) setMaskedSymbolLine(*line); }
    virtual void setMaskedSymbolLine(const casacore::String& color,
            PlotLine::Style style = PlotLine::SOLID, double width = 1.0) {
        PlotSymbolPtr sym = maskedSymbol();
        sym->setLine(color, style, width);
        setMaskedSymbol(*sym);
    }
    virtual void setMaskedSymbolAreaFill(const PlotAreaFill& fill) {
        PlotSymbolPtr sym = maskedSymbol();
        sym->setAreaFill(fill);
        setMaskedSymbol(*sym);
    }
    virtual void setMaskedSymbolAreaFill(const PlotAreaFillPtr fill) {
        if(!fill.null()) setMaskedSymbolAreaFill(*fill); }
    virtual void setMaskedSymbolAreaFill(const casacore::String& color,
            PlotAreaFill::Pattern pattern = PlotAreaFill::FILL) {
        PlotSymbolPtr sym = maskedSymbol();
        sym->setAreaFill(color, pattern);
        setMaskedSymbol(*sym);
    }
    // </group>
};


// An error plot is a scatter plot with error bars drawn.  It is expected to
// consist of PlotErrorData.
class ErrorPlot : public virtual ScatterPlot {
public:
    ErrorPlot() { }
    
    virtual ~ErrorPlot() { }

    
    // Overrides ScatterPlot::pointData().
    virtual PlotPointDataPtr pointData() const { return errorData(); }
    
    
    // ABSTRACT METHODS //
    
    // Returns the error data used for this plot.
    virtual PlotErrorDataPtr errorData() const = 0;
    
    // Returns whether the error bar line is shown or not.
    virtual bool errorLineShown() const = 0;
    
    // Sets whether the error bar line is shown.  If show is true, the
    // implementation can decide whether to choose the line shown, or just
    // use the last set PlotLine.
    virtual void setErrorLineShown(bool show = true) = 0;
    
    // Returns the line used to draw the error bars.
    virtual PlotLinePtr errorLine() const = 0;
    
    // Sets the line used to draw the error bars.
    virtual void setErrorLine(const PlotLine& line) = 0;
    
    // Convenience methods for setting error line.
    // <group>
    virtual void setErrorLine(const PlotLinePtr line) {
        if(!line.null()) setErrorLine(*line);
        else             setErrorLineShown(false);
    }
    virtual void setErrorLine(const casacore::String& color,
                              PlotLine::Style style = PlotLine::SOLID,
                              double width = 1.0) {
        PlotLinePtr line = errorLine();
        line->setColor(color);
        line->setStyle(style);
        line->setWidth(width);
        setErrorLine(*line);
    }
    virtual void setErrorLineColor(const casacore::String& color) {
        PlotLinePtr line = errorLine();
        line->setColor(color);
        setErrorLine(*line);
    }
    virtual void setErrorLineStyle(PlotLine::Style style) {
        PlotLinePtr line = errorLine();
        line->setStyle(style);
        setErrorLine(*line);
    }
    virtual void setErrorLineWidth(double width) {
        PlotLinePtr line = errorLine();
        line->setWidth(width);
        setErrorLine(*line);
    }
    // </group>
    
    // Returns the "cap" size of the error bar.  The cap is the perpendicular
    // line at the end of the error bar (that makes the error bar look like an
    // I rather than a |).
    virtual unsigned int errorCapSize() const = 0;
    
    // Sets the error bar cap size in pixels.
    virtual void setErrorCapSize(unsigned int capSize) = 0;
};


// An color plot is a scatter plot with differentiated colors for points in
// different bins.  It is expected to consist of PlotBinnedData.
class ColoredPlot : public virtual ScatterPlot {
public:
    // Constructor.
    ColoredPlot() { }
    
    // Destructor.
    virtual ~ColoredPlot() { }

    
    // Overrides ScatterPlot::pointData().
    virtual PlotPointDataPtr pointData() const { return binnedColorData(); }
    
    
    // ABSTRACT METHODS //
    
    // Returns the binned data used for this plot.
    virtual PlotBinnedDataPtr binnedColorData() const = 0;
    
    // Returns the color to use for the bin at the given index.  The color
    // applies to the symbol fill color.
    virtual PlotColorPtr colorForBin(unsigned int bin) const = 0;
    
    // Sets the color to use for the bin at the given index.  The color applies
    // to the symbol fill color.
    virtual void setColorForBin(unsigned int bin, const PlotColorPtr color) = 0;
};


// Bar plot abstract class.  It is expected to take data in the form of
// PlotPointData.  The line methods in Plot are used for the bar outlines.
class BarPlot : public virtual Plot {
public:
    BarPlot() { }
    
    virtual ~BarPlot() { }

    
    // Overrides Plot::data().
    PlotDataPtr data() const { return pointData(); }
    
    // Returns the data value at the given index.  Just a thin layer over the
    // PlotPointData functionality.
    virtual ppoint_t pointAt(unsigned int i) const {
        PlotPointDataPtr data = pointData();
        if(!data.null()) return ppoint_t(data->xAt(i), data->yAt(i));
        else             return ppoint_t(0, 0);
    }
    
    // Implements PlotItem::drawCount().  Provides default implementation that
    // returns the number of plotted points.
    virtual unsigned int drawCount() const { return pointData()->size(); }
    
    
    // ABSTRACT METHODS //
    
    // Returns the data used for the plot.
    virtual PlotPointDataPtr pointData() const = 0;
    
    // Returns whether or not the bars have an area fill or not.
    virtual bool areaIsFilled() const = 0;
    
    // Sets whether the bars have an area fill.  If fill is true, the
    // implementation can decide whether to choose the area fill, or just
    // use the last set PlotAreaFill.
    virtual void setAreaFilled(bool fill = true) = 0;
    
    // Returns a copy of the area fill used for the bar interiors.
    virtual PlotAreaFillPtr areaFill() const = 0;
    
    // Sets the area fill used for the bars to the given.
    virtual void setAreaFill(const PlotAreaFill& areaFill) = 0;
    
    
    // IMPLEMENTED METHODS //
    
    // Convenience methods for setting area fill.
    // <group>
    virtual void setAreaFill(const PlotAreaFillPtr areaFill) {
        if(!areaFill.null()) setAreaFill(*areaFill);
        else                 setAreaFilled(false);
    }
    virtual void setAreaFill(const casacore::String& color,
                          PlotAreaFill::Pattern pattern = PlotAreaFill::FILL) {
        PlotAreaFillPtr fill = areaFill();
        fill->setColor(color);
        fill->setPattern(pattern);
        setAreaFill(*fill);
    }
    // </group>
};


// Plot used to show raster (image-like) data.  Expected to use PlotRasterData.
// The line methods in Plot are used for contour lines (if any).  A RasterPlot
// can either use the data values directly as colors, or automatically pick
// colors based on a range of values and a colorbar.  (See PlotRasterData.)
class RasterPlot : public virtual Plot {
public:    
    RasterPlot() { }
    
    virtual ~RasterPlot() { }

    
    // Overrides Plot::data().
    PlotDataPtr data() const { return rasterData(); }
    
    // Returns the data at the given point.  Just a thin layer over data
    // functionality.
    virtual double valueAt(double x, double y) const {
        PlotRasterDataPtr data = rasterData();
        if(!data.null()) return data->valueAt(x, y);
        else             return 0;
    }
    
    
    // ABSTRACT METHODS //
    
    // Returns the data used.
    virtual PlotRasterDataPtr rasterData() const = 0;
    
    // Sets the x range.
    virtual void setXRange(double from, double to) = 0;
    
    // Sets the y range.
    virtual void setYRange(double from, double to) = 0;
    
    // Returns the data format.  See PlotRasterData.
    virtual PlotRasterData::Format dataFormat() const = 0;
    
    // Sets the data format.  See PlotRasterData.
    virtual void setDataFormat(PlotRasterData::Format f) = 0;
    
    // Returns the data origin.  See PlotRasterData.
    virtual PlotRasterData::Origin dataOrigin() const = 0;
    
    // Sets the data origin.  See PlotRasterData.
    virtual void setDataOrigin(PlotRasterData::Origin o) = 0;
    
    // Returns the contour line levels, if any.
    virtual std::vector<double> contourLines() const = 0;
    
    // Sets the contour line levels.
    virtual void setContourLines(const std::vector<double>& lines) = 0;
};


/* Smart pointer definitions */

INHERITANCE_POINTER2(Plot, PlotPtr, PlotItem, PlotItemPtr)
INHERITANCE_POINTER(ScatterPlot, ScatterPlotPtr, Plot, PlotPtr, PlotItem,
                    PlotItemPtr)
INHERITANCE_POINTER(MaskedScatterPlot, MaskedScatterPlotPtr, ScatterPlot,
                    ScatterPlotPtr, PlotItem, PlotItemPtr)
INHERITANCE_POINTER(ErrorPlot, ErrorPlotPtr, ScatterPlot, ScatterPlotPtr,
                    PlotItem, PlotItemPtr)
INHERITANCE_POINTER(ColoredPlot, ColoredPlotPtr, ScatterPlot, ScatterPlotPtr,
                    PlotItem, PlotItemPtr)
INHERITANCE_POINTER(BarPlot, BarPlotPtr, Plot, PlotPtr, PlotItem, PlotItemPtr)
INHERITANCE_POINTER(RasterPlot, RasterPlotPtr, Plot, PlotPtr, PlotItem,
                    PlotItemPtr)

}

#endif /*PLOT_H_*/
