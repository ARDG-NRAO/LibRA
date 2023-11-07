//# Plotter.h: Highest level plotting object that holds one or more canvases.
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
#ifndef PLOTTER_H_
#define PLOTTER_H_

#include <graphics/GenericPlotter/PlotCanvasLayout.h>
#include <graphics/GenericPlotter/PlotLogger.h>
#include <graphics/GenericPlotter/PlotPanel.h>
#include <graphics/GenericPlotter/PageHeaderDataModel.h>

namespace casa {

//# Forward Declarations
class PlotFactory;

// A Plotter can be thought of as a frame that holds one or more PlotCanvases
// in a configuration determined by a given PlotCanvasLayout.  It also has
// some top-level and main-window functionality.
class Plotter {
public:
    // Static //
    
    // This enum should contain all known implementations.
    enum Implementation {
        QWT,
        
        // Default plotter implementation is set here.
        DEFAULT = QWT
    };
    
    // Default implementation-specific GUI panels that can be turned on or off.
    enum DefaultPanel {
        HAND_TOOLS,  // buttons/tools to: select regions, zoom, and pan;
                     // (optionally) turn on/off tracker;
                     // (optionally) show/hide and position legend
        EXPORT_TOOLS // buttons/tools to export the canvases to a file
    };
    
    // The default date format to use for the plotter.  See dateFormat().
    static const casacore::String DEFAULT_DATE_FORMAT;
    
    // The default relative date format to use for the plotter.  See
    // relativeDateFormat().
    static const casacore::String DEFAULT_RELATIVE_DATE_FORMAT;
    
    // Returns a casacore::String for the given date value using the given format (see
    // dateFormat()) and scale.  If isRelative is true, the value is treated as
    // a relative value (i.e., +X seconds past a reference date); otherwise it
    // is treated as an absolute value.  For relative values years, months, and
    // days are ignored.
	// Default precision for seconds is 4; use secPrecision to change this
    static casacore::String formattedDateString(const casacore::String& format,
			double value, PlotAxisScale scale, bool isRelative = false,
			int secPrecision=-1);
    
    
    // Non-Static //
    
    // Constructor.
    Plotter();
    
    // Destructor.
    virtual ~Plotter();
    
    
    // ABSTRACT METHODS //
    
    // Top-Level GUI methods //
    
    // Shows/hides the plotter GUI.
    virtual void showGUI(bool showGUI = true) = 0;
    
    // Returns the current size of the plotter GUI in pixels (width x height).
    virtual std::pair<int, int> size() const = 0;
    
    // Sets the plotter GUI size in pixels
    virtual void setSize(int width, int height) = 0;

    // Sets width=height but saves rectangular ratio
    virtual void makeSquarePlot(bool square, bool wave) = 0;

    //Set the size of cached axes stack image
    virtual void setCanvasCachedAxesStackImageSize( int width, int height );

    // Returns the plotter window's title.
    virtual casacore::String windowTitle() const = 0;
    
    // Sets the plotter window's title to the given.
    virtual void setWindowTitle(const casacore::String& newTitle) = 0;
    
    // Returns the size of the canvas area (i.e., minus bordering panels) in
    // pixels (width x height).
    virtual std::pair<int, int> canvasAreaSize() const = 0;
    
    // If the plotter has a single canvas, sets the size of the canvas to the
    // given.  If resizeWindow is true, the plotter is resized to fit the new
    // canvas size.
    virtual void setCanvasSize(int width, int height,
                               bool resizeWindow = true) = 0;
    
    // Returns the DPI used in the GUI display.
    virtual int displayDPI() const = 0;
    
    // Returns true if the plotter window can be casted to a QWidget, false
    // otherwise.
    virtual bool isQWidget() const = 0;
    
    // Gets/sets cursor for the whole plotter.  Can be overridden by individual
    // canvases.
    // <group>
    virtual PlotCursor cursor() const = 0;
    virtual void setCursor(PlotCursor cursor) = 0;
    // </group>
    
    // Refreshes the plotter GUI.
    virtual void refresh() = 0;
    
    //Forces the script client to refresh.
    virtual void updateScriptGui() = 0;

    // Closes the plotter window.
    virtual void close() = 0;
   
    
    // Canvas Layout methods //
    
    // Returns the current layout, or a null pointer if none has been set.
    virtual PlotCanvasLayoutPtr canvasLayout() = 0;
    
    // Sets the canvas layout to the given.  Clears out old canvases as needed.
    virtual void setCanvasLayout(PlotCanvasLayoutPtr layout) = 0;
    
    // Method for when the layout has changed (i.e. changed canvases, etc.).
    // This should only be used by the layout currently being used by the
    // plotter.
    virtual void canvasLayoutChanged(PlotCanvasLayout& layout) = 0;
    
    // Gets/Sets the date format for the plotter and all current and future
    // canvases.  This format should be used anywhere date values are displayed
    // to the user, such as for axis ticks and tracker tools.  A format can
    // consist of the following tags:
    // * %y : year
    // * %m : month
    // * %d : day of month
    // * %h : hours
    // * %n : minutes
    // * %s : seconds
    // The format can optionally have the following tags:
    // * %pX : precision to display for seconds, with X being an integer; if X
    //         is less than zero, the default is used.  Applies to any seconds
    //         tags that are AFTER the precision tag.
    // <group>
    virtual const casacore::String& dateFormat() const = 0;
    virtual void setDateFormat(const casacore::String& dateFormat) = 0;
    // </group>
    
    // Gets/Sets the date format for relative values (i.e., for reference
    // values on axes) for the plotter and all current and future canvases.
    // This format should be used anywhere relative date values are displayed
    // to the user, such as for axis ticks when a reference value is set.  See
    // dateFormat() for information on the format.
    // <group>
    virtual const casacore::String& relativeDateFormat() const = 0;
    virtual void setRelativeDateFormat(const casacore::String& dateFormat) = 0;
    // </group>
    
    
    // Panel methods //
    
    // Returns whether or not the given default panel is shown.
    virtual bool defaultPanelShown(DefaultPanel panel) = 0;
    
    // Shows/hides the given default panel.  Note: the default panels are for
    // convenience's sake and are left completely up to the implementation.
    virtual void showDefaultPanel(DefaultPanel panel, bool show = true) = 0;
    
    // Adds the given plot panel and returns its index.
    virtual int addPanel(PlotPanelPtr panel) = 0;
    
    // Returns all plot panels currently shown.
    virtual std::vector<PlotPanelPtr> allPanels() = 0;
    
    // Returns the number of plot panels currently on the plotter.
    virtual unsigned int numPanels() = 0;
    
    // Returns the plot panel at the given index, or a null pointer for an
    // invalid index.
    virtual PlotPanelPtr getPanel(int index) = 0;
    
    // Returns the index of the given plot panel, or -1 if not on canvas or
    // null.
    virtual int panelIndex(PlotPanelPtr panel) = 0;
    
    // Clears all plot panels from the plotter.
    virtual void clearPanels() = 0;

    // Removes the given plot panel from the plotter.
    virtual void removePanel(PlotPanelPtr panel) = 0;
    
    // Removes the plot panel with the given index from the plotter.
    virtual void removePanel(int id) = 0;
    
    // Removes the last-added plot panel from the plotter.
    virtual void removeLastPanel() = 0;
    
    
    //Use a common x-/y- axis when plotting multiple graphs.
    void setCommonAxisX(casacore::Bool commonAxis );
    void setCommonAxisY(casacore::Bool commonAxis );
    bool isCommonAxisX() const;
    bool isCommonAxisY() const;
    void setAxisLocation( PlotAxis xLocation, PlotAxis yLocation );
    PlotAxis getAxisLocationX() const;
    PlotAxis getAxisLocationY() const;

    // Plotting Functionality methods //
    
    // Returns the implementation of this plotter.
    virtual Implementation implementation() const = 0;
    
    // Returns a new instance of a PlotFactory that can create plot items for
    // this implementation.  It is the caller's responsibility to delete the
    // PlotFactory when finished.
    virtual PlotFactory* implementationFactory() const = 0;
    
    // Exports the plotter (all canvases) using the given format.
    virtual bool exportToFile(const PlotExportFormat& format) = 0;
    
    // Shows a file chooser dialog and returns the absolute filename that the
    // user chooses.  If a directory is given, start the dialog there.  If the
    // user cancels, an empty casacore::String is returned.
    virtual casacore::String fileChooserDialog(const casacore::String& title = "File Chooser",
                                     const casacore::String& directory = "") = 0;

    
    // Event Handling methods //
    
    // Registers the given resize event handler with this plotter.
    virtual void registerResizeHandler(PlotResizeEventHandlerPtr handler) = 0;
    
    // Returns a list of all registered resize event handlers for this plotter.
    virtual std::vector<PlotResizeEventHandlerPtr> allResizeHandlers() const = 0;
    
    // Unregisters the given resize event handler with this plotter.
    virtual void unregisterResizeHandler(PlotResizeEventHandlerPtr handler) =0;
    
    
    // IMPLEMENTED METHODS //
    
    // See PlotCanvasLayout::canvasAt().
    virtual PlotCanvasPtr canvasAt(const PlotLayoutCoordinate& coord);
    
    // See PlotCanvasLayout::canvas().
    virtual PlotCanvasPtr canvas();
    
    // Sets the layout to a single layout with the given canvas.
    virtual void setCanvas(PlotCanvasPtr canvas);
    
    // See PlotLogger::filterMinPriority().
    casacore::LogMessage::Priority logFilterMinPriority() const;
    
    // See PlotLogger::setFilterMinPriority().
    void setLogFilterMinPriority(PlotLogMessage::Priority minPriority);
    
    // See PlotLogger::eventFlag().
    virtual bool logFilterEventFlag(int flag) const;
    
    // See PlotLogger::setEventFlag().
    virtual void setLogFilterEventFlag(int flag, bool on);
    
    // See PlotLogger::eventFlags().
    virtual int logFilterEventFlags() const;
    
    // See PlotLogger::setEventFlags().
    virtual void setLogFilterEventFlags(int flags);
    
    // Gets the PlotLogger associated with this Plotter.  There should be
    // exactly one PlotLogger per Plotter.
    virtual PlotLoggerPtr logger() const;
    
    virtual bool exportPlot(const PlotExportFormat& format) = 0;
    bool isVisible(PlotCanvasPtr& canvas );

    virtual void refreshPageHeader() = 0;

    virtual void refreshPageHeaderDataModel(PageHeaderDataModelPtr dataModel) = 0 ;

protected:
    // Logger.
    PlotLoggerPtr m_logger;
//    PlotterControllerPtr m_controller;
    casacore::Bool commonAxisX;
    casacore::Bool commonAxisY;
    PlotAxis axisLocationX;
    PlotAxis axisLocationY;
};
typedef casacore::CountedPtr<Plotter> PlotterPtr;

}

#endif /*PLOTTER_H_*/
