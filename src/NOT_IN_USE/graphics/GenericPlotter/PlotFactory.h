//# PlotFactory.h: Class to produce implementation-specific plotting objects.
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
#ifndef PLOTFACTORY_H_
#define PLOTFACTORY_H_

#include <graphics/GenericPlotter/Plot.h>
#include <graphics/GenericPlotter/PlotAnnotation.h>
#include <graphics/GenericPlotter/PlotCanvas.h>
#include <graphics/GenericPlotter/PlotCanvasLayout.h>
#include <graphics/GenericPlotter/PlotData.h>
#include <graphics/GenericPlotter/PlotOperation.h>
#include <graphics/GenericPlotter/PlotOptions.h>
#include <graphics/GenericPlotter/PlotShape.h>
#include <graphics/GenericPlotter/Plotter.h>
#include <graphics/GenericPlotter/PlotTool.h>

#include <utility>

namespace casa {

// The idea behind a plot factory is to produce classes that are useful to
// the underlying implementation without having to know what the implementation
// is.  A PlotFactory provides for creation of plot items with the given
// parameters in the given implementation.  Methods that have a "smartDelete"
// parameter pass that flag into the smart pointer constructor, to indicate
// whether the underlying object is deleted upon destruction of all smart
// pointers or not.
class PlotFactory {
public:
    // Constructor.
    PlotFactory();
    
    // Destructor.
    virtual ~PlotFactory();
    
    
    // Support Methods //

    // Returns true if this implementation's PlotCanvas subclass supports
    // threaded drawing, false otherwise.  Threaded drawing is done using a
    // threading library appropriate to the implementation, and keeps progress
    // information synchronized across threads by using PlotOperation objects
    // unique to each canvas (see PlotCanvas::operationDraw()).
    virtual bool canvasHasThreadedDrawing() const = 0;
    
    // Returns true if this implementation's PlotCanvas subclass supports
    // cached layer drawing, false otherwise.  Cached layer drawing means that
    // items that are changed in one layer will ONLY cause a redraw of that
    // layer, and the others are cached and thus do not need to be redrawn.
    virtual bool canvasHasCachedLayerDrawing() const = 0;
    
    // Returns true if this implementation's PlotCanvas subclass has a cached
    // axes stack, false otherwise.  Cached axes stack means that the part of
    // the canvas for a given axes range is cached after the first time it is
    // drawn, so that if the ranges are set to a cached value in the future it
    // will not have to be redrawn.  This is useful for, for example, caching
    // the zoom/pan stack to move quickly along axes ranges already seen.  Note
    // that the cached axes stack will need to be invalidated as needed when
    // plot items on the canvas change and thus require a redraw.
    virtual bool canvasHasCachedAxesStack() const = 0;
    
    
    // Execution Methods //
    
    // Enters the GUI execution loop, if necessary, and returns the result.
    virtual int execLoop() = 0;
    
    // Return which implementation this factory is producing.
    virtual Plotter::Implementation implementation() const = 0;
    
    
    // GUI Objects //
    
    // Return a new instance of a Plotter for this implementation with the
    // given parameters.  The plotter's window title is set to the given.  If
    // showSingleCanvas is true, then the plotter is created with a single
    // canvas; otherwise it has no canvases.  If showGUI is true then the
    // plotter GUI will be shown after creation.  If log event flags are given
    // (bitwise or of PlotLogger::Event) they are passed to the plotter.
    virtual PlotterPtr plotter(const casacore::String& windowTitle = "Plotter",
            bool showSingleCanvas = true, bool showGUI = true,
            int logEventFlags = PlotLogger::NO_EVENTS,
            bool smartDelete = true) const = 0;
    
    // Return a new instance of a Plotter for this implementation, with the
    // given rows and columns of canvases and parameters.  The plotter's window
    // title is set to the given.  If showGUI is true then the plotter GUI will
    // be shown after creation.  If log event flags are given (bitwise or of
    // PlotLogger::Event) they are passed to the plotter.
    virtual PlotterPtr plotter(unsigned int nrows, unsigned int ncols,
            const casacore::String& windowTitle = "Plotter", bool showGUI = true,
            int logEventFlags = PlotLogger::NO_EVENTS,
            bool smartDelete = true) const = 0;
    
    // Convenience method for creating a plotter with a single, given canvas.
    // The plotter's window title is set to the given.  If showGUI is true then
    // the plotter GUI will be shown after creation.  If log event flags are
    // given (bitwise or of PlotLogger::Event) they are passed to the plotter.
    // DEFAULT IMPLEMENTATION.
    virtual PlotterPtr plotter(PlotCanvasPtr canvas,
            const casacore::String& windowTitle = "Plotter", bool showGUI = true,
            int logEventFlags = PlotLogger::NO_EVENTS,
            bool smartDelete = true);
    
    // Return a new instance of a PlotCanvas for this implementation.
    virtual PlotCanvasPtr canvas(bool smartDelete = true) const = 0;
    
    // Return a new instance of a PlotPanel for this implementation.
    virtual PlotPanelPtr panel(bool smartDelete = true) const = 0;
    
    // Return a new instance of a PlotButton with the given text for this
    // implementation.  If isText is true, the given string should be
    // displayed on the button as text; otherwise it is a path to an image
    // file.  If toggleable is true, then a toggleable button is returned.
    virtual PlotButtonPtr button(const casacore::String& str, bool isText = true,
            bool toggleable = false, bool smartDelete = true) const = 0;
    
    // Return a new instance of a PlotCheckbox with the given text for this
    // implementation.
    virtual PlotCheckboxPtr checkbox(const casacore::String& str,
            bool smartDelete = true) const = 0;
    
    
    // PlotItem Objects //
    
    // Returns a new instance of a ScatterPlot for this implementation with
    // the given PlotPointData.  IMPORTANT: the returned ScatterPlot should be
    // of the correct type to handle the given data.  For example, if the data
    // is of type PlotMaskedPointData, the returned scatter plot should be able
    // to be cast to a MaskedScatterPlot, and similarly for PlotErrorData and
    // ErrorPlot.  If the given data is both masked and error, the returned
    // plot should be able to handle both as well.
    virtual ScatterPlotPtr scatterPlot(PlotPointDataPtr data,
            const casacore::String& title = "Scatter Plot",
            bool smartDelete = true) const = 0;
    
    // Convenience methods for specialized scatter plot and data classes.
    // Since the scatterPlot method should be able to handle the different
    // subclasses of data and return something of the proper type, this should
    // be fine.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual MaskedScatterPlotPtr maskedPlot(PlotMaskedPointDataPtr data,
            const casacore::String& title = "Masked Plot", bool smartDelete= true) const;
    virtual ErrorPlotPtr errorPlot(PlotErrorDataPtr data,
            const casacore::String& title = "Error Plot", bool smartDelete= true) const;
    virtual ColoredPlotPtr coloredPlot(PlotBinnedDataPtr data,
            const casacore::String& title = "Colored Plot", bool smartDelete=true) const;
    // </group>
    
    // Returns a new instance of a BarPlot for this implementation with the
    // given PlotPointData and optional title.
    virtual BarPlotPtr barPlot(PlotPointDataPtr data,
            const casacore::String& title = "Bar Plot", bool smartDelete= true) const= 0;
    
    // Returns a new instance of a BarPlot set to use histogram data for this
    // implementation with the given PlotSinglePointData and number of bins.
    // DEFAULT IMPLEMENTATION.
    virtual BarPlotPtr histogramPlot(PlotSingleDataPtr data,
            unsigned int numBins, const casacore::String& title = "Histogram Plot",
            bool smartDelete = true) const;
    
    // Returns a new instance of a RasterPlot for this implementation with the
    // given data and optional title and format.
    virtual RasterPlotPtr rasterPlot(PlotRasterDataPtr data,
            const casacore::String& title = "Raster Plot",
            PlotRasterData::Format format = PlotRasterData::RGB32,
            bool smartDelete = true) const = 0;
    
    // Returns a new instance of a RasterPlot for this implementation with the
    // given data and contour levels and optional title and format.
    // DEFAULT IMPLEMENTATION.
    virtual RasterPlotPtr contourPlot(PlotRasterDataPtr data,
            const std::vector<double>& contours,
            const casacore::String& title = "Contour Plot",
            PlotRasterData::Format format = PlotRasterData::RGB32,
            bool smartDelete = true) const;
    
    // Returns a new instance of a RasterPlot for this implementation
    // interpreted as a spectrogram with the given data and optional title.
    // DEFAULT IMPLEMENTATION.
    virtual RasterPlotPtr spectrogramPlot(PlotRasterDataPtr data,
            const casacore::String& title = "Spectrogram", bool smartDelete= true) const;
    
    // Returns a new instance of a RasterPlot for this implementation
    // interpreted as a spectrogram with the given data and contour levels and
    // optional title.
    // DEFAULT IMPLEMENTATION.
    virtual RasterPlotPtr contouredSpectrogramPlot(PlotRasterDataPtr data,
            const std::vector<double>& cont,
            const casacore::String& title = "Spectrogram Contours",
            bool smartDelete = true) const;
    
    // Return a new instance of a PlotAnnotation for this implementation with
    // the given text and coordinates.
    virtual PlotAnnotationPtr annotation(const casacore::String& text,
            const PlotCoordinate& coord, bool smartDelete = true) const = 0;
    
    // Convenience method for return an annotation with the given world
    // coordinates.
    // DEFAULT IMPLEMENTATION.
    virtual PlotAnnotationPtr annotation(const casacore::String& text, double x,
            double y, bool smartDelete = true) const;
    
    // Return a new instance of a PlotShapeRectangle for this implementation
    // with the given coordinates.
    virtual PlotShapeRectanglePtr shapeRectangle(const PlotCoordinate& upperLeft,
            const PlotCoordinate& lowerRight, bool smartDelete= true) const= 0;
    
    // Convenience method for returning a rectangle with the given world
    // coordinates.
    // DEFAULT IMPLEMENTATION.
    virtual PlotShapeRectanglePtr shapeRectangle(double left, double top,
            double right, double bottom, bool smartDelete = true) const;
    
    // Return a new instance of a PlotShapeEllipse for this implementation
    // with the given coordinates and radii.
    virtual PlotShapeEllipsePtr shapeEllipse(const PlotCoordinate& center,
            const PlotCoordinate& radii, bool smartDelete = true) const = 0;
    
    // Convenience method for returning an ellipse with the given world
    // coordinates.
    virtual PlotShapeEllipsePtr shapeEllipse(double x, double y,
            double xRadius, double yRadius, bool smartDelete = true) const;
    
    // Return a new instance of a PlotShapePolygon for this implementation
    // with the given coordinates.
    virtual PlotShapePolygonPtr shapePolygon(
            const std::vector<PlotCoordinate>& coords,
            bool smartDelete = true) const = 0;
    
    // Convenience method for returning a polygon with the given world
    // coordinates.
    // DEFAULT IMPLEMENTATION.
    virtual PlotShapePolygonPtr shapePolygon(const std::vector<double>& x,
            const std::vector<double>& y, bool smartDelete = true) const;
    
    // Returns a new instance of a PlotShapeLine for this implementation
    // at the given location.
    virtual PlotShapeLinePtr shapeLine(double location, PlotAxis axis,
            bool smartDelete = true) const = 0;
    
    // Returns a new instance of a PlotShapeArrow for this implementation
    // at the given coordinates with the given arrow style.
    virtual PlotShapeArrowPtr shapeArrow(const PlotCoordinate& from,
            const PlotCoordinate& to, PlotShapeArrow::Style fromArrow =
            PlotShapeArrow::NOARROW, PlotShapeArrow::Style toArrow =
            PlotShapeArrow::V_ARROW, bool smartDelete = true) const = 0;
    
    // Convenience methods for returning arrows with the given world
    // coordinates.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual PlotShapeArrowPtr shapeArrow(double fromX, double fromY,
            double toX, double toY, PlotShapeArrow::Style fromArrow =
            PlotShapeArrow::NOARROW, PlotShapeArrow::Style toArrow =
            PlotShapeArrow::V_ARROW, bool smartDelete = true) const;
    virtual PlotShapeArrowPtr shapeLineSegment(const PlotCoordinate& from,
            const PlotCoordinate& to, bool smartDelete = true) const;
    virtual PlotShapeArrowPtr shapeLineSegment(double fromX, double fromY,
            double toX, double toY, bool smartDelete = true) const;
    // </group>
    
    // Returns a new instance of a PlotShapePath for this implementation
    // with the given coordinates.
    virtual PlotShapePathPtr shapePath(
            const std::vector<PlotCoordinate>& coords,
            bool smartDelete = true) const = 0;
    
    // Convenience method for returning a path with the given world
    // coordinates.
    // DEFAULT IMPLEMENTATION.
    virtual PlotShapePathPtr shapePath(const std::vector<double>& x,
            const std::vector<double>& y, bool smartDelete = true) const;
    
    // Returns a new instance of a PlotShapeArc for this implementation
    // with the given start position, width and height, start angle, and span
    // angle.
    virtual PlotShapeArcPtr shapeArc(const PlotCoordinate& start,
            const PlotCoordinate& widthHeight, int startAngle,
            int spanAngle, bool smartDelete = true) const = 0;    
    
    // Returns a new instance of a PlotPoint for this implementation at the
    // given coordinates.
    virtual PlotPointPtr point(const PlotCoordinate& coord,
            bool smartDelete = true) const = 0;
    
    // Convenience methods for returning a point with the given world
    // coordinates.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual PlotPointPtr point(double x, double y, bool smartDelete=true)const;
    virtual PlotPointPtr point(float x, float y, bool smartDelete= true) const;
    virtual PlotPointPtr point(int x, int y, bool smartDelete = true) const;
    virtual PlotPointPtr point(unsigned int x, unsigned int y,
            bool smartDelete = true) const;
    // </group>
    
    
    // Customization Objects //
    
    // Color could be a name (i.e. "black") or a hex value (i.e. "000000").
    virtual PlotColorPtr color(const casacore::String& color,
            bool smartDelete = true) const = 0;
    
    // Make a copy of the given color for this implementation.
    virtual PlotColorPtr color(const PlotColor& copy,
            bool smartDelete = true) const = 0;
    
    // Convenience method for returning a copy of the given color.
    // DEFAULT IMPLEMENTATION.
    virtual PlotColorPtr color(const PlotColorPtr copy,
            bool smartDelete = true) const;
    
    // Returns a list of all the named colors that the implementation supports.
    virtual std::vector<casacore::String> allNamedColors() const = 0;
    
    // Return a new font with the given characteristics.  Color can either be
    // in hexadecimal form or name form.
    virtual PlotFontPtr font(const casacore::String& family = "Arial",
            double pointSize = 12, const casacore::String& color = "000000",
            bool bold = false, bool italics = false,
            bool underline = false, bool smartDelete = true) const = 0;
    
    // Make a copy of the given font for this implementation.
    virtual PlotFontPtr font(const PlotFont& copy,
            bool smartDelete = true) const = 0;
    
    // Convenience method for returning a copy of the given font.
    // DEFAULT IMPLEMENTATION.
    virtual PlotFontPtr font(const PlotFontPtr copy,
            bool smartDelete = true) const;
    
    // Returns a new area fill with the given color and pattern.  Color can
    // either be in hexadecimal form or name form.
    virtual PlotAreaFillPtr areaFill(const casacore::String& color,
            PlotAreaFill::Pattern pattern = PlotAreaFill::FILL,
            bool smartDelete = true) const = 0;
    
    // Returns a copy of the given area fill for this implementation.
    virtual PlotAreaFillPtr areaFill(const PlotAreaFill& copy,
            bool smartDelete = true) const = 0;
    
    // Convenience method for returning a copy of the given area fill.
    // DEFAULT IMPLEMENTATION.
    virtual PlotAreaFillPtr areaFill(const PlotAreaFillPtr copy,
            bool smartDelete = true) const;
    
    // Returns a new line with the given color, style, and width.  Color can
    // either be in hexadecimal form or name form.
    virtual PlotLinePtr line(const casacore::String& color,
            PlotLine::Style style = PlotLine::SOLID, double width = 1.0,
            bool smartDelete = true) const = 0;
    
    // Make a copy of the given line for this implementation.
    virtual PlotLinePtr line(const PlotLine& copy,
            bool smartDelete = true) const = 0;
    
    // Convenience method for returning a copy of the given line.
    // DEFAULT IMPLEMENTATION.
    virtual PlotLinePtr line(const PlotLinePtr copy,
            bool smartDelete = true) const;
    virtual PlotSymbolPtr createSymbol (const casacore::String& descriptor, int size, const casacore::String& color,
    			const casacore::String& fillPattern, bool outline );
    // Returns a new symbol with the given style.
    virtual PlotSymbolPtr symbol(PlotSymbol::Symbol style,
            bool smartDelete = true) const = 0;
    
    // Returns a new symbol with the given character.
    // DEFAULT IMPLEMENTATION.
    virtual PlotSymbolPtr symbol(char sym, bool smartDelete = true) const;
    
    // Return a new symbol with the given unicode #.
    // DEFAULT IMPLEMENTATION.
    virtual PlotSymbolPtr uSymbol(unsigned short unicode,
            bool smartDelete = true) const;
    
    // Make a copy of the given symbol for this implementation.
    virtual PlotSymbolPtr symbol(const PlotSymbol& copy,
            bool smartDelete = true) const = 0;
    
    // Convenience method for returning a copy of the given symbol.
    // DEFAULT IMPLEMENTATION.
    virtual PlotSymbolPtr symbol(const PlotSymbolPtr copy,
            bool smartDelete = true) const;
    
    
    // Tool Objects //
    
    // Returns a standard mouse tool group for this implementation.
    // DEFAULT IMPLEMENTATION.
    virtual PlotStandardMouseToolGroupPtr standardMouseTools(
            ToolCode activeTool = NONE_TOOL,  
            bool smartDelete = true)    const;
    
    // Returns a standard mouse tool group for this implementation on the given
    // axes.
    // DEFAULT IMPLEMENTATION.
    virtual PlotStandardMouseToolGroupPtr standardMouseTools(PlotAxis xAxis,
            PlotAxis yAxis, PlotCoordinate::System sys,
            ToolCode activeTool = NONE_TOOL,  
            bool smartDelete = true)   const;
    
    // Returns tools for this implementation.  Implementations that provide
    // their own PlotTool subclasses should override these methods.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual PlotSelectToolPtr selectTool(bool smartDelete = true) const;
    virtual PlotZoomToolPtr zoomTool(bool smartDelete = true) const;
    virtual PlotPanToolPtr panTool(bool smartDelete = true) const;
    virtual PlotFlagAllToolPtr flagAllTool(bool smartDelete = true) const;
    virtual PlotTrackerToolPtr trackerTool(bool smartDelete = true) const;
    virtual PlotSelectToolPtr selectTool(PlotAxis xAxis, PlotAxis yAxis,
            PlotCoordinate::System system, bool smartDelete = true) const;
    virtual PlotZoomToolPtr zoomTool(PlotAxis xAxis, PlotAxis yAxis,
            PlotCoordinate::System system, bool smartDelete = true) const;
    virtual PlotPanToolPtr panTool(PlotAxis xAxis, PlotAxis yAxis,
            PlotCoordinate::System system, bool smartDelete = true) const;
    virtual PlotFlagAllToolPtr flagAllTool(PlotAxis xAxis, PlotAxis yAxis,
            PlotCoordinate::System system, bool smartDelete = true) const;
    virtual PlotTrackerToolPtr trackerTool(PlotAxis xAxis, PlotAxis yAxis,
            PlotCoordinate::System system, bool smartDelete = true) const;
    // </group>
    
    
    // Operations //
    
    // Returns a new PlotMutex for this implementation.
    virtual PlotMutexPtr mutex(bool smartDelete = true) const = 0;
    
    
    // casacore::Data Objects //
    
// Macro for method declarations for different permutations of the default data
// objects for different types.
#define PF_DATA_DEC(TYPE)                                                     \
    virtual PlotPointDataPtr data(TYPE *& y, unsigned int n,                  \
            bool shouldDelete = true) const;                                  \
    virtual PlotPointDataPtr data(casacore::Vector< TYPE >& y,                          \
            bool shouldDelete = false) const;                                 \
    virtual PlotPointDataPtr data(std::vector< TYPE >& y,                          \
            bool shouldDelete = false) const;                                 \
    virtual PlotPointDataPtr data(TYPE *& x, TYPE *& y, unsigned int n,       \
            bool shouldDelete = true) const;                                  \
    virtual PlotPointDataPtr data(casacore::Vector< TYPE >& x, casacore::Vector< TYPE >& y,       \
            bool shouldDelete = false) const;                                 \
    virtual PlotPointDataPtr data(std::vector< TYPE >& x, std::vector< TYPE >& y,       \
            bool shouldDelete = false) const;                                 \
    virtual PlotSingleDataPtr singleData(TYPE *& data, unsigned int n,        \
            bool shouldDelete = true) const;                                  \
    virtual PlotSingleDataPtr singleData(casacore::Vector< TYPE >& data,                \
            bool shouldDelete = false) const;                                 \
    virtual PlotSingleDataPtr singleData(std::vector< TYPE >& data,                \
            bool shouldDelete = false) const;                                 \
    virtual PlotPointDataPtr histogramData(TYPE *& data, unsigned int n,      \
            unsigned int numBins, bool shouldDel = true) const;               \
    virtual PlotPointDataPtr histogramData(std::vector< TYPE >& data,              \
            unsigned int numBins, bool shouldDel = false) const;              \
    virtual PlotPointDataPtr histogramData(casacore::Vector< TYPE >& data,              \
            unsigned int numBins, bool shouldDel = false) const;              \
    virtual PlotMaskedPointDataPtr data(TYPE *& x, TYPE*& y, bool*& mask,     \
            unsigned int n, bool shouldDelete = true) const;                  \
    virtual PlotMaskedPointDataPtr data(casacore::Vector< TYPE >& x, casacore::Vector< TYPE >& y, \
            casacore::Vector<bool>& mask, bool shouldDelete = true) const;              \
    virtual PlotMaskedPointDataPtr data(std::vector< TYPE >& x, std::vector< TYPE >& y, \
            std::vector<bool>& mask, bool shouldDelete = true) const;              \
    virtual PlotErrorDataPtr data(TYPE *& x, TYPE *& y, unsigned int n,       \
            TYPE xLeftError, TYPE xRightError, TYPE yBottomError,             \
            TYPE yTopError, bool shouldDelete = true) const;                  \
    virtual PlotErrorDataPtr data(casacore::Vector< TYPE >& x, casacore::Vector< TYPE >& y,       \
            TYPE xLeftError, TYPE xRightError, TYPE yBottomError,             \
            TYPE yTopError, bool shouldDelete = true) const;                  \
    virtual PlotErrorDataPtr data(std::vector< TYPE >& x, std::vector< TYPE >& y,       \
            TYPE xLeftError, TYPE xRightError, TYPE yBottomError,             \
            TYPE yTopError, bool shouldDelete = true) const;                  \
    virtual PlotErrorDataPtr data(TYPE *& x, TYPE *& y, TYPE *& xLeftError,   \
            TYPE *& xRightError, TYPE *& yBottomError, TYPE *& yTopError,     \
            unsigned int n, bool shouldDelete = true) const;                  \
    virtual PlotErrorDataPtr data(casacore::Vector< TYPE >& x, casacore::Vector< TYPE >& y,       \
            casacore::Vector< TYPE >& xLeftError, casacore::Vector< TYPE >& xRightError,          \
            casacore::Vector< TYPE >& yBottomError, casacore::Vector< TYPE >& yTopError,          \
            bool shouldDelete = false) const;                                 \
    virtual PlotErrorDataPtr data(std::vector< TYPE >& x, std::vector< TYPE >& y,       \
            std::vector< TYPE >& xLeftError, std::vector< TYPE >& xRightError,          \
            std::vector< TYPE >& yBottomError, std::vector< TYPE >& yTopError,          \
            bool shouldDelete = false) const;                                 \
    virtual PlotRasterDataPtr data(casacore::Matrix< TYPE >& data,                      \
            bool shouldDelete = false) const;                                 \
    virtual PlotRasterDataPtr data(casacore::Matrix< TYPE >& data, double fromX,        \
            double toX, double fromY, double toY,                             \
            bool shouldDelete = false) const;
    
    // Returns data objects for doubles.
    // DEFAULT IMPLEMENTATION.
    // <group>
    PF_DATA_DEC(double)
    // </group>
    
    // Returns data objects for floats.
    // DEFAULT IMPLEMENTATION.
    // <group>
    PF_DATA_DEC(float)
    // </group>
    
    // Returns data objects for ints.
    // DEFAULT IMPLEMENTATION.
    // <group>
    PF_DATA_DEC(int)
    // </group>

    // Returns data objects for unsigned ints.
    // DEFAULT IMPLEMENTATION.
    // <group>
    PF_DATA_DEC(unsigned int)
    // </group>
    
    // Returns a histogram data object for the given PlotSingleData and number
    // of bins.
    // DEFAULT IMPLEMENTATION.
    virtual PlotPointDataPtr histogramData(PlotSingleDataPtr data,
            unsigned int numBins) const;
};
typedef casacore::CountedPtr<PlotFactory> PlotFactoryPtr;

}

#endif /*PLOTFACTORY_H_*/
