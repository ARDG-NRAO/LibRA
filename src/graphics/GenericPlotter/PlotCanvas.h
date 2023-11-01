//# PlotCanvas.h: Main drawing area for different plot items to be attached.
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
#ifndef PLOTCANVAS_H_
#define PLOTCANVAS_H_

#include <graphics/GenericPlotter/Plot.h>
#include <graphics/GenericPlotter/PlotAnnotation.h>
#include <graphics/GenericPlotter/PlotEventHandler.h>
#include <graphics/GenericPlotter/PlotItem.h>
#include <graphics/GenericPlotter/PlotLogger.h>
#include <graphics/GenericPlotter/PlotOperation.h>
#include <graphics/GenericPlotter/PlotOptions.h>
#include <graphics/GenericPlotter/PlotShape.h>
#include <graphics/GenericPlotter/PlotTool.h>

#include <casacore/casa/BasicSL/String.h>

namespace casa {

//# Forward Declarations
class PlotFactory;


// Abstract parent for any class that wants to be notified when the canvas
// is about to start drawing.  This class is needed because a canvas could
// start redrawing for a number of reasons (user-requested, items changing the
// scale, resizing, ...) and instead of forcing applications to catch all cases
// this class can be used to notify when a drawing has begun for any reason.
class PlotDrawWatcher {
public:
    // Constructor.
    PlotDrawWatcher() { }
    
    // Destructor.
    virtual ~PlotDrawWatcher() { }
    
    
    // This method should be called by the canvas whenever drawing is about to
    // begin.  The watcher is given:
    // * A pointer to the PlotOperation object which is updated with progress
    //   information,
    // * A flag indicating whether the drawing will happen in its own thread or
    //   not,
    // * A flag indicating which layers are going to be drawn (bitwise-or of
    //   PlotCanvasLayer values).
    // The method should return true UNLESS the watcher wants to stop the
    // canvas from drawing.  For example, if a canvas does not have threaded
    // drawing, the watcher may want to stop drawing initially in order to
    // spawn a new thread and then redraw.
    virtual bool canvasDrawBeginning(PlotOperationPtr drawOperation,
            bool drawingIsThreaded, int drawnLayersFlag) = 0;
};
typedef casacore::CountedPtr<PlotDrawWatcher> PlotDrawWatcherPtr;


// PlotCanvas is an area for different PlotItems to be attached and drawn.  It
// also has axes and other properties.  It's one of the more important plotting
// classes.
class PlotCanvas {
public:
    // Static //
    
    // Position that the legend can be in, relative to the canvas.
    enum LegendPosition {
        INT_URIGHT, // inside the canvas, on the upper right
        INT_LRIGHT, // inside the canvas, on the lower right
        INT_ULEFT,  // inside the canvas, on the upper left
        INT_LLEFT,  // inside the canvas, on the lower left
        EXT_RIGHT,  // outside the canvas, on the right
        EXT_TOP,    // outside the canvas, on the top
        EXT_LEFT,   // outside the canvas, on the left
        EXT_BOTTOM  // outside the canvas, on the bottom
    };
    
    // Returns all legend positions.
    // <group>
    static std::vector<LegendPosition> allLegendPositions(); 
    static std::vector<casacore::String> allLegendPositionStrings();
    // </group>
    
    // Converts between legend positions and their casacore::String representations.
    // <group>
    static casacore::String legendPosition(LegendPosition p); 
    static LegendPosition legendPosition(casacore::String p, bool* ok = NULL);
    // </group>
    
    // Indicates whether the given legend position is internal to the canvas or
    // not.
    static bool legendPositionIsInternal(LegendPosition p);
    
    // Returns a std::vector containing all values in the PlotAxis enum, ordered
    // from lowest value to highest value.
    static std::vector<PlotAxis> allAxes();
    
    // Returns the or'ed value of all PlotAxis enum values.
    static int allAxesFlag();
    
    // Returns a vector containing all values in the PlotCanvasLayer enum,
    // ordered from lowest value to highest value.
    static std::vector<PlotCanvasLayer> allLayers();
    
    // Returns the or'ed value of all PlotCanvasLayer enum values.
    static int allLayersFlag();
    
    // Convenient access to operation names.
    // <group>
    static const casacore::String OPERATION_DRAW;
    static const casacore::String OPERATION_EXPORT;
    // </group>
    
    
    // Non-Static //
    
    // Constructor.
    PlotCanvas();
    
    // Destructor.
    virtual ~PlotCanvas();

    
    // Support Methods //
    
    // See PlotFactory::canvasHasThreadedDrawing().
    // DEFAULT IMPLEMENTATION.
    virtual bool hasThreadedDrawing() const;
    
    // See PlotFactory::canvasHasCachedLayerDrawing().
    // DEFAULT IMPLEMENTATION.
    virtual bool hasCachedLayerDrawing() const;
    
    // See PlotFactory::hasCachedAxesStack().
    // DEFAULT IMPLEMENTATION.
    virtual bool hasCachedAxesStack() const;
    
    
    // GUI Methods //
    
    // Returns the canvas size in pixels (width x height).
    virtual std::pair<int, int> size() const = 0;
    virtual void setMinimumSize( int width, int height ) = 0;
    virtual void setMinimumSizeHint( int width, int height ) = 0;
    virtual void show() = 0;
    virtual void hide() = 0;
    virtual bool isDrawing( )= 0;
    // Returns the text of the canvas title, or an empty casacore::String if none is
    // shown.  The title is shown above the plotting area.
    virtual casacore::String title() const = 0;
    
    // Sets the text of the canvas title to the given.  Setting the title to an
    // empty string should remove the title from the canvas.
    virtual void setTitle(const casacore::String& title) = 0;
    
    // Returns a copy of the font used for the canvas title.  If no title is
    // shown this behavior is undefined.
    virtual PlotFontPtr titleFont() const = 0;
    
    // Set the font of the canvas title to the given.
    virtual void setTitleFont(const PlotFont& font) = 0;
    
    // Convenience method for setting title font.
    // DEFAULT IMPLEMENTATION.
    virtual void setTitleFont(const PlotFontPtr font);
    
    // Returns a copy of the canvas background.
    virtual PlotAreaFillPtr background() const = 0;
    virtual PlotAreaFillPtr defaultBackground() const;
    
    // Sets the background of the canvas to the given.
    virtual void setBackground(const PlotAreaFill& areaFill) = 0;
    
    // Convenience methods for setting background.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual void setBackground(const PlotAreaFillPtr areaFill);
    virtual void setBackground(const casacore::String& color,
            PlotAreaFill::Pattern pattern = PlotAreaFill::FILL);
    // </group>
    
    // Gets/sets the cursor for this canvas.
    // <group>
    virtual PlotCursor cursor() const = 0;
    virtual void setCursor(PlotCursor cursor) = 0;
    // </group>    
    
    // Refreshes the GUI, which includes redrawing plot items as needed even if
    // drawing is currently being held.
    virtual void refresh() = 0;
    
    // Refreshes the GUI, ONLY for the layers as indicated by the flag.  The
    // flag should be a bitwise-or of PlotCanvasLayer values.
    virtual void refresh(int drawLayersFlag) = 0;
    
    // Indicates whether a cast to QWidget would succeed for this object.
    virtual bool isQWidget() const = 0;
    
    
    // Axes Methods //
    
    // Returns a bitwise-or of PlotAxis values corresponding to which sides are shown
    virtual PlotAxisBitset  shownAxes() const = 0;
    
    // Shows/Hides axes based on the given bitset, which should be a bitwise-or
    // of PlotAxis values.
    virtual void showAxes(PlotAxisBitset axes) = 0;
    
    
    // Returns true if the given axis is shown, false otherwise.
    // DEFAULT IMPLEMENTATION.
    virtual bool isAxisShown(PlotAxis axis) const;
    
    
    // Shows/hides the given axis.
    // DEFAULT IMPLEMENTATION.
    virtual void showAxis(PlotAxis axis, bool show = true);
    virtual void setCommonAxes( bool commonX, bool commonY ) = 0;
    
    // Convenience method for showing/hiding two axes at once.
    // DEFAULT IMPLEMENTATION.
    virtual void showAxes(PlotAxis xAxis, PlotAxis yAxis, bool show = true);
    
    // Convenience method for showing/hiding all four axes at once.
    // DEFAULT IMPLEMENTATION.
    virtual void showAllAxes(bool show);
    
    // Returns the scale for the given axis.
    virtual PlotAxisScale axisScale(PlotAxis axis) const = 0;
    
    // Sets the scale for the given axis and rescales accordingly.  May reset
    // tool stacks.
    virtual void setAxisScale(PlotAxis axis, PlotAxisScale scale, casacore::uInt base=10) = 0;
    
    // Sets/gets the sort direction of the scale for the given axis
    virtual bool setAxisScaleSortDirection(PlotAxis axis, SortDirection direction) = 0;
    virtual std::pair<bool,SortDirection> axisScaleSortDirection(PlotAxis axis) const = 0;

    // Sets/gets the angle format of the scale for the given axis
    virtual void setAxisScaleAngleFormat(PlotAxis axis, AngleFormat format) = 0;
    virtual AngleFormat axisScaleAngleFormat(PlotAxis axis) const = 0;

    // Convenience method for setting the axis scale for two axes at once.  May
    // reset tool stacks.
    // DEFAULT IMPLEMENTATION.
    virtual void setAxesScales(PlotAxis xAxis, PlotAxisScale xScale,
            PlotAxis yAxis, PlotAxisScale yScale);
    
    // Gets/Sets the reference values for the given axis.  If a reference value
    // is set for an axis, its tick labels use an offset from that value
    // instead of the actual tick value.  For example, if a reference value is
    // set to 15, instead of ticks reading "14 15 16" they would read "-1 0 1".
    // <group>
    virtual bool axisReferenceValueSet(PlotAxis axis) const = 0;
    virtual double axisReferenceValue(PlotAxis axis) const = 0;
    virtual void setAxisReferenceValue(PlotAxis axis, bool on,
    		double value = 0) = 0;
    // </group>
    
    // Returns whether the given Cartesian axis is shown or not.  (See
    // showCartesianAxis().)
    virtual bool cartesianAxisShown(PlotAxis axis) const = 0;
    
    // Show or hide the given Cartesian axis.  A Cartesian axis is different
    // from a normal axis in that it is inside the graph, corresponding to the
    // (0,0) location of the plot(s), rather than outside the graph.  The
    // secondary axis is the axis at which the mirrored axis will be placed at
    // the 0 value of.  For example, to show the Cartesian X_BOTTOM axis you'd
    // likely want to mirror the axis at Y_LEFT = 0, so that would be the
    // secondary axis.  If hideNormalAxis is true, it is equivalent to
    // showAxis(mirrorAxis, !hideNormalAxis).
    virtual void showCartesianAxis(PlotAxis mirrorAxis, PlotAxis secondaryAxis,
            bool show = true, bool hideNormalAxis = true) = 0;
    
    // Convenience methods for most common Cartesian axes cases.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual void showCartesianAxis(PlotAxis mirrorAxis, bool show = true,
                                   bool hideNormalAxis = true);
    virtual void showCartesianAxes(bool show = true, bool hideNormal = true);
    // </group>
    
    // Returns the label for the given axis, or an empty casacore::String if none is
    // shown.
    virtual casacore::String axisLabel(PlotAxis axis) const = 0;
    
    // Sets the label of an axis title to the given.  Setting the title to an
    // empty string should remove the title from the axis.
    virtual void setAxisLabel(PlotAxis axis, const casacore::String& title) = 0;
    
    // Convenience method for clearing labels from all axes.
    // DEFAULT IMPLEMENTATION.
    virtual void clearAxesLabels();
    
    // Returns a copy of the font used for the given axis label.  If no title
    // is shown this behavior is undefined.
    virtual PlotFontPtr axisFont(PlotAxis axis) const = 0;
    
    // Set the font of the axis title to the given.
    virtual void setAxisFont(PlotAxis axis, const PlotFont& font) = 0;
    
    // Convenience method for setting axis font.
    // DEFAULT IMPLEMENTATION.
    virtual void setAxisFont(PlotAxis axis, const PlotFontPtr font);
    
    // Returns true if a color bar is shown on the given axis, false otherwise.
    // A color bar should only be shown when one of more of the items on the
    // canvas is a RasterPlot or a SpectrogramPlot.  If there are multiple
    // raster or spectrograms, behavior is undefined.
    virtual bool colorBarShown(PlotAxis axis = Y_RIGHT) const = 0;
    
    // Shows/hides a color bar is shown on the given axis.  (See
    // colorBarShown().)
    virtual void showColorBar(bool show = true, PlotAxis axis = Y_RIGHT) = 0;
    
    
    // Axes Range Methods //
    
    // Returns the displayed range of the given axis, in world coordinates.
    virtual prange_t axisRange(PlotAxis axis) const = 0;
    
    // Returns the displayed ranges of the given axes, in world coordinates.
    // DEFAULT IMPLEMENTATION.
    virtual PlotRegion axesRanges(PlotAxis xAxis, PlotAxis yAxis) const;
    
    // Sets the displayed range of the given axis, in world coordinates.
    // Implies setAxesAutoRescale(false), and does NOT reset tool stacks since
    // the tools may be using this method.
    virtual void setAxisRange(PlotAxis axis, double from, double to) = 0;
    
    // Swap scale's boundaries of the given axis
    virtual void invertAxis(PlotAxis axis) = 0;

    // See setAxisRange(PlotAxis, double, double).
    // DEFAULT IMPLEMTNATION.
    virtual void setAxisRange(PlotAxis axis, const prange_t& range);
    
    // Sets the displayed ranges of the given axes, in world coordinates.
    // Implies setAxesAutoRescale(false), and does NOT reset tool stacks since
    // the tools may be using this method.  It is recommended that subclasses
    // reimplement this to be more efficient.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual void setAxesRanges(PlotAxis xAxis, double xFrom, double xTo,
                               PlotAxis yAxis, double yFrom, double yTo);    
    virtual void setAxesRanges(PlotAxis xAxis, const prange_t& xRange,
            PlotAxis yAxis, const prange_t& yRange);
    // </group>
    
    // Sets the displayed range of the given axes. Implies
    // setAxesAutoRescale(false),  and does NOT reset tool stacks since the
    // tools may be using this method.
    // DEFAULT IMPLEMENTATION.
    virtual void setAxesRegion(PlotAxis xAxis, PlotAxis yAxis,
            const PlotRegion& region);
    
    // Moves (pans) the axes range by the given delta, in world coordinates.
    // Implies setAxesAutoRescale(false), and does NOT reset tool stacks since
    // the tools may be using these methods.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual void moveAxisRange(PlotAxis axis, double delta);
    virtual void moveAxesRanges(PlotAxis xAxis, double xDelta,
                                PlotAxis yAxis, double yDelta);
    // </group>
    
    // Returns true if the axes are set to autorescale when a new plot item is
    // added, false otherwise.
    virtual bool axesAutoRescale() const = 0;
    
    // Sets auto-rescaling for future plot items.  If autoRescale is true,
    // implies setAxesRatioLocked(false).
    virtual void setAxesAutoRescale(bool autoRescale = true) = 0;
    
    // Rescales the axis to show all plot items on the canvas.  Implies
    // setAxesAutoRescale(false), and may reset PlotTools.
    virtual void rescaleAxes() = 0;
    
    // Returns true if the axes ratio is locked, false otherwise.  If the
    // ratio is locked, resizing one axis will resize the others
    // proportionally.
    virtual bool axesRatioLocked() const = 0;
    
    // Sets whether the axes ratio is locked or not.
    virtual void setAxesRatioLocked(bool locked = true) = 0;
    
    
    // casacore::Stack Methods //
    
    // Returns a single PlotAxesStack associated with this canvas.  Note that
    // the canvas itself doesn't modify/use this stack AT ALL; it is expressly
    // for outside tools/classes/etc to have a single stack per canvas.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual PlotAxesStack& axesStack();
    virtual const PlotAxesStack& axesStack() const;
    // </group>
    
    // Convenience method to move along the PlotAxesStack associated with this
    // canvas (see axesStack()).  Calls the stack's moveAndReturn method, and
    // then sets the axes ranges to the returned PlotRegion.  If delta is 0,
    // the stack is moved to its base; otherwise it moves forward or backward
    // from the current stack index (see PlotAxesStack::moveAndReturn()).
    // Returns true if the operation succeeded, false otherwise (for invalid
    // delta).
    // DEFAULT IMPLEMENTATION.
    virtual bool axesStackMove(int delta);
    
    // Gets/Sets the length limit on the PlotAxesStack associated with this
    // canvas.  See PlotAxesStack::lengthLimit() and
    // PlotAxesStack::setLengthLimit().
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual int axesStackLengthLimit() const;
    virtual void setAxesStackLengthLimit(int lengthLimit);
    // </group>
    
    // Gets/Sets the size limit, in kilobytes, on the draw cache for the axes
    // stack associated with this canvas (see
    // PlotFactory::canvasHasCachedAxesStack()).  A zero or negative value
    // means no limit.  NOTE: Only valid for implementations that supports a
    // cached axes stack.
    // <group>
    virtual int cachedAxesStackSizeLimit() const = 0;
    virtual void setCachedAxesStackSizeLimit(int sizeInKilobytes) = 0;
    // </group>
    
    // Gets/Sets the size of the image used for the cached axes stack, in
    // pixels.  The default implementation is set to (-1, -1) which means that
    // the images are set to the current size of the canvas.  It could be
    // useful to set this value to a large size so that, for example, resizing
    // wouldn't require a redraw of the whole canvas but rather just resizing
    // the larger image to a smaller one.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual std::pair<int, int> cachedAxesStackImageSize() const;
    virtual void setCachedAxesStackImageSize(int width, int height);
    // </group>
    
    
    // Plot Methods //
    
    // Adds the given plot item to this canvas, if not null or otherwise
    // invalid.  Returns whether the adding succeeded or not.  May reset
    // registered PlotTools.
    virtual bool plotItem(PlotItemPtr item, PlotCanvasLayer layer = MAIN) = 0;
    
    // Item-specific add methods.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual bool plot(PlotPtr plot, bool overplot = true);
    virtual bool plotPoint(PlotPointPtr point);
    virtual bool drawShape(PlotShapePtr shape);
    virtual bool drawAnnotation(PlotAnnotationPtr annotation);
    // </group>
    
    // Returns all plot items currently on the canvas.
    virtual std::vector<PlotItemPtr> allPlotItems() const = 0;
    
    // Item-specific all methods.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual std::vector<PlotPtr> allPlots() const;
    virtual std::vector<PlotPointPtr> allPoints() const;
    virtual std::vector<PlotShapePtr> allShapes() const;
    virtual std::vector<PlotAnnotationPtr> allAnnotations() const;
    // </group>
    
    // Returns the number of plot items currently on the canvas.
    // DEFAULT IMPLEMENTATION.
    virtual unsigned int numPlotItems() const;
    
    // Item-specific num methods.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual unsigned int numPlots() const;
    virtual unsigned int numPoints() const;
    virtual unsigned int numShapes() const;
    virtual unsigned int numAnnotations() const;
    // </group>
    
    // Returns all plot items currently on the given layer of the canvas.
    virtual std::vector<PlotItemPtr> layerPlotItems(PlotCanvasLayer layer) const= 0;
    
    // Item-specific layer methods.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual std::vector<PlotPtr> layerPlots(PlotCanvasLayer layer) const;
    virtual std::vector<PlotPointPtr> layerPoints(PlotCanvasLayer layer) const;
    virtual std::vector<PlotShapePtr> layerShapes(PlotCanvasLayer layer) const;
    virtual std::vector<PlotAnnotationPtr> layerAnnotations(PlotCanvasLayer l)const;
    // </group>
    
    // Returns the number of plot items currently on the given layer of the
    // canvas.
    // DEFAULT IMPLEMENTATION.
    virtual unsigned int numLayerPlotItems(PlotCanvasLayer layer) const;
    
    // Item-specific layer num methods.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual unsigned int numLayerPlots(PlotCanvasLayer layer) const;
    virtual unsigned int numLayerPoints(PlotCanvasLayer layer) const;
    virtual unsigned int numLayerShapes(PlotCanvasLayer layer) const;
    virtual unsigned int numLayerAnnotations(PlotCanvasLayer layer) const;
    // </group>    
    
    // Removes the given item from this canvas (if valid) and updates the GUI.
    // May reset registered PlotTools.
    // DEFAULT IMPLEMENTATION.
    virtual void removePlotItem(PlotItemPtr item);
    
    // Item-specific remove methods.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual void removePlot(PlotPtr plot);
    virtual void removePoint(PlotPointPtr point);
    virtual void removeShape(PlotShapePtr shape);
    virtual void removeAnnotation(PlotAnnotationPtr annotation);
    // </group>
    
    // Removes the given items from this canvas (if valid) and updates the GUI.
    // May reset registered PlotTools.
    virtual void removePlotItems(const std::vector<PlotItemPtr>& items) = 0;
    
    // Removes the last plot item added to the canvas and updates the GUI.  May
    // reset registered PlotTools.
    // DEFAULT IMPLEMENTATION.
    virtual void removeLastPlotItem();
    
    // Item-specific remove last methods.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual void removeLastPlot();
    virtual void removeLastPoint();
    virtual void removeLastShape();
    virtual void removeLastAnnotation();
    // </group>
    
    // Clears all plot items from this canvas and updates the GUI.  May reset
    // registered PlotTools.
    // DEFAULT IMPLEMENTATION.
    virtual void clearItems();
    
    // Item-specific clear methods.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual void clearPlots();
    virtual void clearPoints();
    virtual void clearShapes();
    virtual void clearAnnotations();
    // </group>
    
    // Clears all plot items from the given layer on this canvas and updates
    // the GUI.  May reset registered PlotTools.
    // DEFAULT IMPLEMENTATION.
    virtual void clearLayer(PlotCanvasLayer layer);
    
    
    // Draw Methods //
    
    // Hold or release drawing of plot items on the canvas.  This can be used,
    // for example, when adding many items to a canvas to avoid redrawing the
    // canvas until all items are on.  Hold/release are NOT recursive.
    // <group>
    virtual void holdDrawing() = 0;
    virtual void releaseDrawing() = 0;
    virtual bool drawingIsHeld() const = 0;
    // </group>
    
    // Registers/Unregisters the given draw watcher for this canvas.  See
    // documentation for PlotDrawWatcher.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual void registerDrawWatcher(PlotDrawWatcherPtr watcher);    
    virtual void unregisterDrawWatcher(PlotDrawWatcherPtr watcher);
    // </group>
    
    // Returns a PlotOperation object that can be used to track drawing
    // progress across threads.  There must be one PlotOperation for drawing
    // per canvas.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual PlotOperationPtr operationDraw();
    virtual PlotOperationPtr operationDraw(PlotMutexPtr mutex);
    // </group>
    
    
    // Selection Methods //
    
    // Returns true if a line is shown for the selection tool, false otherwise.
    // DEFAULT IMPLEMENTATION.
    virtual bool selectLineShown() const;
    
    // Sets whether a line is shown for the selection tool.
    virtual void setSelectLineShown(bool shown = true) = 0;
    
    // returns a copy of the line for the selection tool
    virtual PlotLinePtr selectLine() const = 0;
    
    // set the line for the selection tool
    virtual void setSelectLine(const PlotLine& line) = 0;
    
    // Convenience methods for setting selection line.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual void setSelectLine(const PlotLinePtr line);
    virtual void setSelectLine(const casacore::String& color,
                               PlotLine::Style style = PlotLine::SOLID,
                               double width = 1.0);
    // </group>
    
    
    // Grid Methods //
    
    // Returns true if any grid lines are shown, false otherwise.  If bool
    // parameters are given, they will be set for if each line group is shown
    // or not.
    virtual bool gridShown(bool* xMajor = NULL, bool* xMinor = NULL,
            bool* yMajor = NULL, bool* yMinor = NULL) const = 0;
    
    // Sets whether the grid is shown, for each of the given line groups.
    virtual void showGrid(bool xMajor, bool xMinor, bool yMajor,bool yMinor)=0;
    
    // Convenience methods for showing/hiding the grid.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual void showGrid(bool showAll = true);
    virtual void showGridMajor(bool show = true);
    virtual void showGridMinor(bool show = true);
    // </group>
    
    // Gets/Sets whether major grid lines are shown for the x-axis.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual bool gridXMajorShown() const;
    virtual void showGridXMajor(bool s = true);
    // </group>
    
    // Gets/Sets  whether minor grid lines are shown for the x-axis.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual bool gridXMinorShown() const;
    virtual void showGridXMinor(bool s = true);
    // </group>
    
    // Gets/Sets whether major grid lines are shown for the y-axis.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual bool gridYMajorShown() const;
    virtual void showGridYMajor(bool s = true);
    // </group>

    // Gets/Sets whether minor grid lines are shown for the y-axis.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual bool gridYMinorShown() const;
    virtual void showGridYMinor(bool s = true);
    // </group>
    
    // Returns a copy of the line used to draw the major grid lines.
    virtual PlotLinePtr gridMajorLine() const = 0;
    
    // Sets the line used to draw the major grid lines.
    virtual void setGridMajorLine(const PlotLine& line) = 0;
    
    // Returns a copy of the line used to draw the minor grid lines.
    virtual PlotLinePtr gridMinorLine() const = 0;
    
    // Sets the line used to draw the minor grid lines.
    virtual void setGridMinorLine(const PlotLine& line) = 0;
    
    // Convenience methods for setting major/minor grid lines.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual void setGridMajorLine(const PlotLinePtr line);
    virtual void setGridMajorLine(const casacore::String& color,
            PlotLine::Style style = PlotLine::SOLID, double width = 1.0);
    virtual void setGridMinorLine(const PlotLinePtr line);
    virtual void setGridMinorLine(const casacore::String& color,
            PlotLine::Style style = PlotLine::SOLID, double width = 1.0);
    // </group>
    
    
    // Legend Methods //
    
    // Returns whether the legend is shown or not
    virtual bool legendShown() const = 0;
    
    // Show/hide the legend.  If on is true, show the legend in the given
    // position.
    virtual void showLegend(bool on = true,
                            LegendPosition position = EXT_BOTTOM) = 0;
    
    // Returns the position of the legend.  Note: if the legend is hidden,
    // this behavior is undefined.
    virtual LegendPosition legendPosition() const = 0;
    
    // Sets the position of the legend, if it is shown.
    virtual void setLegendPosition(LegendPosition position) = 0;
    
    // Returns a copy of the line used to outline the legend.
    virtual PlotLinePtr legendLine() const = 0;
    
    // Sets the line used to outline the legend.
    virtual void setLegendLine(const PlotLine& line) = 0;
    
    // Convenience methods for setting legend line.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual void setLegendLine(const PlotLinePtr line);
    virtual void setLegendLine(const casacore::String& color,
            PlotLine::Style style = PlotLine::SOLID, double width = 1.0);
    // </group>
    
    // Returns a copy of the area fill used for the legend background
    virtual PlotAreaFillPtr legendFill() const = 0;
    
    // Sets the area fill used for the legend background
    virtual void setLegendFill(const PlotAreaFill& area) = 0;
    
    // Convenience methods for setting legend background.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual void setLegendFill(const PlotAreaFillPtr area);
    virtual void setLegendFill(const casacore::String& color,
            PlotAreaFill::Pattern pattern = PlotAreaFill::FILL);
    // </group>
    
    // Returns a copy of the font used for the legend text.
    virtual PlotFontPtr legendFont() const = 0;
    
    // Sets the font used for the legend text.
    virtual void setLegendFont(const PlotFont& font) = 0;
    
    // Convenience method for setting legend font.
    // DEFAULT IMPLEMENTATION.
    virtual void setLegendFont(const PlotFontPtr font);


    // Miscellaneous Plotting Functionality //
    
    // Returns true if autoincrementcolors is on, false otherwise.  (See
    // setAutoIncrementColors().)
    virtual bool autoIncrementColors() const = 0;
    
    // Sets whether adding new overplotted plots should automatically
    // "increment" colors or not.  If this property is true, each new
    // overplotted plot should have a different color.  The specific handling
    // of this behavior is left up to the implementation.
    virtual void setAutoIncrementColors(bool autoInc = true) = 0;

    // Exports this canvas to the given format and returns true on success.
    virtual bool exportToFile(const PlotExportFormat& format) = 0;
    
    // Returns a PlotOperation object that can be used to track export
    // progress across threads.  There must be one PlotOperation for export
    // per canvas.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual PlotOperationPtr operationExport();
    virtual PlotOperationPtr operationExport(PlotMutexPtr mutex);
    // </group>
    
    // Shows a file chooser dialog and return the absolute filename that the
    // user chooses.  If a directory is given, start the dialog there.  If the
    // user cancels the dialog, an empty casacore::String should be returned.
    virtual casacore::String fileChooserDialog(const casacore::String& title = "File Chooser",
                                     const casacore::String& directory = "") = 0;
    
    // Gets/Sets the date format for this canvas.  See Plotter::dateFormat().
    // <group>
    virtual const casacore::String& dateFormat() const = 0;
    virtual void setDateFormat(const casacore::String& dateFormat) = 0;
    // </group>
    
    // Gets/Sets the date format for values relative to a reference value for
    // this canvas.  See Plotter::relativeDateFormat().
    // <group>
    virtual const casacore::String& relativeDateFormat() const = 0;
    virtual void setRelativeDateFormat(const casacore::String& dateFormat) = 0;
    // </group>

    // Converts the given coordinate into a coordinate with the given system.
    virtual PlotCoordinate convertCoordinate(const PlotCoordinate& coord,
           PlotCoordinate::System newSystem = PlotCoordinate::WORLD) const = 0;
    
    bool hasSelectedRectangles();
    std::vector<PlotRegion> getSelectedRects();
    void clearSelectedRects();

    // methods related to per-panel flag mode
    void clearMark();
    bool isMarkedForFlag();
    bool isMarkedForUnflag();
    bool isBackgroundColorChanged();
    void setAllFlagged();

    // Converts the given region into a region with the given system.
    // DEFAULT IMPLEMENTATION.
    virtual PlotRegion convertRegion(const PlotRegion& region,
                                     PlotCoordinate::System newSystem) const;
    
    // Returns the width, height, and descent for the given text in the given
    // font.  Note: this was put into place for use in a matplotlib backend,
    // but probably doesn't have much use outside that.
    virtual std::vector<double> textWidthHeightDescent(const casacore::String& text,
            PlotFontPtr font) const = 0;
    
    // Returns the width, height, and descent for the given annotation.  Note:
    // this was put into place for use in a matplotlib backend, but probably
    // doesn't have much use outside that.
    // DEFAULT IMPLEMENTATION.
    virtual std::vector<double> annotationWidthHeightDescent(
            const PlotAnnotationPtr annot) const;
    
    // Returns the implementation of this canvas as a Plotter::Implementation
    // value.
    virtual int implementation() const = 0;
    
    // Returns a new instance of a PlotFactory that can create plot items for
    // this implementation.  It is the caller's responsibility to delete the
    // PlotFactory when finished.
    virtual PlotFactory* implementationFactory() const = 0;
    
    // Returns a mutex appropriate for this implementation.
    // DEFAULT IMPLEMENTATION.
    virtual PlotMutexPtr mutex() const;
    
    // Locate data indices that have values within the given region such that:
    // x >= region.upperLeft().x() && x <= region.lowerRight().x() &&
    // y <= region.upperLeft().y() && y >= region.lowerRight().y().
    // The returned vector has length equal to allPlots().size() (in other
    // words, each plot on the canvas has an index in the returned vector).
    // Each element of the returned vector is a list of all the data indices
    // that are in the region.  Each element in this list is a range of indices.
    // For plots that have no data in the region, the list is empty.
    // 
    // For example, say there are two plots on the canvas.  In the first plot,
    // indices 1, 2, 3, 7, and 10 are in the region; in the second plot,
    // indices 6 and 7 are in the region.  The returned value would be a vector
    // of size 2.  The first element would be the list [(1, 3), (7, 7),
    // (10, 10)] while the second element would be the list [(6, 7)].
    // DEFAULT IMPLEMENTATION.
    virtual std::vector<std::vector<std::pair<unsigned int, unsigned int> > >* locate(
            const PlotRegion& region) const;
    
    // Calls locate() and posts the result to the given log.
    // DEFAULT IMPLEMENTATION.
    virtual void locateAndLog(const PlotRegion& region,
            PlotLoggerPtr logger) const;
    
    
    // Tool Methods //
    
    // Manage mouse tools.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual void registerMouseTool(PlotMouseToolPtr tool, bool activate = true,
            bool blocking = false);
    virtual std::vector<PlotMouseToolPtr> allMouseTools() const;
    virtual std::vector<PlotMouseToolPtr> activeMouseTools() const;
    virtual void unregisterMouseTool(PlotMouseToolPtr tool);
    // </group>
    
    // Returns the standard mouse tools associated with this canvas.  If none
    // is associated, creates and associates one that is non-active.
    // DEFAULT IMPLEMENTATION.
    virtual PlotStandardMouseToolGroupPtr standardMouseTools();
    
    
    // Event Handler Methods //
    
    // Manage select handlers.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual void registerSelectHandler(PlotSelectEventHandlerPtr handler,
            PlotCoordinate::System system = PlotCoordinate::WORLD);
    virtual std::vector<PlotSelectEventHandlerPtr> allSelectHandlers() const;
    virtual void unregisterSelectHandler(PlotSelectEventHandlerPtr handler);
    // </group>
    
    // Manage click handlers.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual void registerClickHandler(PlotClickEventHandlerPtr handler,
            PlotCoordinate::System system = PlotCoordinate::WORLD);
    virtual std::vector<PlotClickEventHandlerPtr> allClickHandlers() const;
    virtual void unregisterClickHandler(PlotClickEventHandlerPtr handler);
    // </group>
    
    // Manage mouse press handlers.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual void registerMousePressHandler(PlotMousePressEventHandlerPtr hndlr,
            PlotCoordinate::System system = PlotCoordinate::WORLD);
    virtual std::vector<PlotMousePressEventHandlerPtr>
            allMousePressHandlers() const;
    virtual void unregisterMousePressHandler(
            PlotMousePressEventHandlerPtr handler);
    // </group>
    
    // Manage mouse release handlers.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual void registerMouseReleaseHandler(
            PlotMouseReleaseEventHandlerPtr handler,
            PlotCoordinate::System system = PlotCoordinate::WORLD);
    virtual std::vector<PlotMouseReleaseEventHandlerPtr>
            allMouseReleaseHandlers() const;
    virtual void unregisterMouseReleaseHandler(
            PlotMouseReleaseEventHandlerPtr handler);
    // </group>
    
    // Manage mouse drag handlers.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual void registerMouseDragHandler(PlotMouseDragEventHandlerPtr h,
            PlotCoordinate::System system = PlotCoordinate::WORLD);
    virtual std::vector<PlotMouseDragEventHandlerPtr> allMouseDragHandlers() const;
    virtual void unregisterMouseDragHandler(PlotMouseDragEventHandlerPtr h);
    // </group>
    
    // Manage mouse move handlers.  NOTE: since this can be costly, it should
    // be avoided if possible.  Implementation canvases are expected to
    // conserve resources if no move handlers are registered.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual void registerMouseMoveHandler(PlotMouseMoveEventHandlerPtr h,
            PlotCoordinate::System system = PlotCoordinate::WORLD);
    virtual std::vector<PlotMouseMoveEventHandlerPtr> allMouseMoveHandlers() const;
    virtual void unregisterMouseMoveHandler(PlotMouseMoveEventHandlerPtr h);
    // </group>
    
    // Manage wheel handlers.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual void registerWheelHandler(PlotWheelEventHandlerPtr handler,
            PlotCoordinate::System system = PlotCoordinate::WORLD);
    virtual std::vector<PlotWheelEventHandlerPtr> allWheelHandlers() const;
    virtual void unregisterWheelHandler(PlotWheelEventHandlerPtr handler);
    // </group>
    
    // Manage key handlers.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual void registerKeyHandler(PlotKeyEventHandlerPtr handler);
    virtual std::vector<PlotKeyEventHandlerPtr> allKeyHandlers() const;
    virtual void unregisterKeyHandler(PlotKeyEventHandlerPtr handler);
    // </group>
    
    // Manage resize handlers.
    // DEFAULT IMPLEMENTATION.
    // <group>
    virtual void registerResizeHandler(PlotResizeEventHandlerPtr handler);
    virtual std::vector<PlotResizeEventHandlerPtr> allResizeHandlers() const;
    virtual void unregisterResizeHandler(PlotResizeEventHandlerPtr handler);
    // </group>
    
protected:    
    // One-per-canvas axes stack.
    PlotAxesStack m_stack;
    
    // One-per-canvas operations.
    // <group>
    PlotOperationPtr m_drawOperation;
    PlotOperationPtr m_exportOperation;
    // </group>
    
    // Draw watchers.
    std::vector<PlotDrawWatcherPtr> m_drawWatchers;
    
    // One-per-canvas standard mouse tools.
    PlotStandardMouseToolGroupPtr m_standardTools;
    
    // Registered mouse tools.
    std::vector<PlotMouseToolPtr> m_mouseTools;
    
    // Event handlers.
    // <group>
    std::vector<std::pair<PlotSelectEventHandlerPtr, PlotCoordinate::System> >
        m_selectHandlers;
    std::vector<std::pair<PlotClickEventHandlerPtr, PlotCoordinate::System> >
        m_clickHandlers;
    std::vector<std::pair<PlotMousePressEventHandlerPtr, PlotCoordinate::System> >
        m_pressHandlers;
    std::vector<std::pair<PlotMouseReleaseEventHandlerPtr, PlotCoordinate::System> >
        m_releaseHandlers;
    std::vector<std::pair<PlotMouseDragEventHandlerPtr, PlotCoordinate::System> >
        m_dragHandlers;
    std::vector<std::pair<PlotMouseMoveEventHandlerPtr, PlotCoordinate::System> >
        m_moveHandlers;
    std::vector<std::pair<PlotWheelEventHandlerPtr, PlotCoordinate::System> >
        m_wheelHandlers;
    std::vector<PlotKeyEventHandlerPtr> m_keyHandlers;
    std::vector<PlotResizeEventHandlerPtr> m_resizeHandlers;
    // </group>
    
    
    // Resets mouse tools.
    void resetMouseTools();
    
    // Notifies any registered draw watchers that a draw is about to begin, and
    // returns the AND of the PlotDrawWatcher::canvasDrawBeginning() results.
    // See PlotDrawWatcher::canvasDrawBeginning() for parameters.
    bool notifyDrawWatchers(PlotOperationPtr drawOperation,
            bool drawingIsThreaded, int drawnLayersFlag);
    
    // Notification methods for event handlers.  Returns true if one mouse tool
    // or event handler was available to handle the event, false otherwise.
    // <group>
    bool notifySelectHandlers(const PlotRegion& selectedRegion);
    bool notifyMouseHandlers(PlotMouseEvent::Type type,
            PlotMouseEvent::Button button, const PlotCoordinate& coord);
    bool notifyClickHandlers(PlotMouseEvent::Button button,
            const PlotCoordinate& coord);
    bool notifyPressHandlers(PlotMouseEvent::Button button,
            const PlotCoordinate& coord);
    bool notifyReleaseHandlers(PlotMouseEvent::Button button,
            const PlotCoordinate& coord);
    bool notifyDragHandlers(PlotMouseEvent::Button button,
            const PlotCoordinate& coord);
    bool notifyMoveHandlers(PlotMouseEvent::Button button,
            const PlotCoordinate& coord);
    bool notifyWheelHandlers(int delta, const PlotCoordinate& coord);
    bool notifyKeyHandlers(char key,
            const std::vector<PlotKeyEvent::Modifier>& modifiers);
    bool notifyResizeHandlers(int oldWidth, int oldHeight,
            int newWidth, int newHeight);
    // </group>
};
typedef casacore::CountedPtr<PlotCanvas> PlotCanvasPtr;

}

#endif /*PLOTCANVAS_H_*/
