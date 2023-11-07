//# PlotTool.h: Tool class definitions (higher-level event handlers).
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
#ifndef PLOTTOOL_H_
#define PLOTTOOL_H_

#include <graphics/GenericPlotter/PlotEventHandler.h>
#include <graphics/GenericPlotter/PlotOptions.h>
#include <graphics/GenericPlotter/PlotShape.h>
#include <graphics/GenericPlotter/PlotAnnotation.h>

namespace casa {

//# Forward Declarations
class PlotCanvas;
class PlotFactory;
class PlotPanToolNotifier;
class PlotSelectToolNotifier;
class PlotTrackerToolNotifier;
class PlotZoomToolNotifier;



///////////////////////////
// ABSTRACT TOOL CLASSES //
///////////////////////////


// A PlotTool is a higher-level event handler for a PlotCanvas.  The idea is to
// take common tasks which may require multiple events and put them in one
// place.  PlotTools also provide additional functionality in that they can be
// 1) active/inactive, and 2) blocking/non-blocking.  The PlotCanvas will only
// send events to active tools, and will not send events to later tools or
// event handlers if the latest tool was blocking.  In this way a single tool
// can be used to handle ALL user interaction via the GUI at one time, if
// desired.  The PlotTool class itself is abstract and deliberately non-binding
// as it is mainly meant for specialization in its children classes.  PlotTools
// can also specify which coordinate system and two axes they work on, and the
// PlotCanvas is expected to obey these constraints.
class PlotTool {
    friend class PlotCanvas;
    friend class PlotMouseToolGroup;
    
public:
    // Constructor which takes which coordinate system events should be
    // processed in.
    PlotTool(PlotCoordinate::System sys = PlotCoordinate::WORLD);
    
    // Constructor which takes the two axes and the coordinate system which
    // events should be processed in.
    PlotTool(PlotAxis xAxis, PlotAxis yAxis,
            PlotCoordinate::System sys = PlotCoordinate::WORLD);
    
    // Destructor.
    virtual ~PlotTool();
    
    
    // Returns whether this tool is currently active or not.
    virtual bool isActive() const;
    
    // Sets whether this tool is currently active or not.
    virtual void setActive(bool isActive = true);
    
    // Returns whether this tool is blocking or not.  When a PlotCanvas
    // encounters a blocking tool, it is expected to not send events to any
    // other handlers further in the chain.
    virtual bool isBlocking() const;
    
    // Sets whether this tool is blocking or not.
    virtual void setBlocking(bool blocking = true);
    
    // Gets the axes on which the tool operates.
    // <group>
    virtual PlotAxis getXAxis() const;
    virtual PlotAxis getYAxis() const;
    // </group>
    
    // Gets the coordinate system in which the tool wants to process events.
    // Events passed to this tool should use this coordinate system.
    virtual PlotCoordinate::System getCoordinateSystem() const;
    
    // Returns whether the last event was handled or not.  Mostly used for
    // blocking tools so that unused events can be passed along.
    virtual bool lastEventWasHandled() const;
    
    // Resets any internal state such as history/stacks.  Should be called by
    // the PlotCanvas when the state of the canvases changes (axes ranges,
    // adding/deleting items, etc.) and would thus invalidate tool states.
    virtual void reset() { }
    


protected:
    // Attached canvas (or NULL for none).
    PlotCanvas* m_canvas;
    
    // Factory for creating implementation-specific objects.
    PlotFactory* m_factory;
    
    // Whether this tool is active.
    bool m_active;
    
    // Whether this tool is blocking.
    bool m_blocking;
    
    // The tool axes.
    PlotAxis m_xAxis, m_yAxis;
    
    // The tool coordinate system.
    PlotCoordinate::System m_coordSystem;
    
    // Last event handled flag.
    bool m_lastEventHandled;
    
    
    // Returns the canvas this tool is attached to, or NULL for none.
    virtual PlotCanvas* canvas() const;
    
    // Returns a factory that can be used for generating
    // implementation-specific classes, or NULL for none.
    virtual PlotFactory* factory() const;
    
    // Returns true if this tool is attached to a canvas, false otherwise.
    virtual bool isAttached() const;
    
    // Attaches this tool to the given canvas.  Detaches from current canvas
    // if necessary.
    virtual void attach(PlotCanvas* canvas);
    
    // Detaches this tool from its canvas.
    virtual void detach();
};

typedef casacore::CountedPtr<PlotTool> PlotToolPtr;




// A PlotMouseTool is a specialization of PlotTool that handles all mouse
// events.  It is abstract, and combines all mouse event handling methods into
// one for convenience.
class PlotMouseTool : public virtual PlotTool,
                      public virtual PlotSelectEventHandler,
                      public virtual PlotClickEventHandler, 
                      public virtual PlotMousePressEventHandler,
                      public virtual PlotMouseReleaseEventHandler,
                      public virtual PlotMouseDragEventHandler,
                      public virtual PlotMouseMoveEventHandler,
                      public virtual PlotWheelEventHandler {
public:
    // Constructor which takes the tool's coordinate system.
    PlotMouseTool(PlotCoordinate::System coordSys = PlotCoordinate::WORLD) :
            PlotTool(coordSys) { }
    
    // Constructor which takes the tool's axes and coordinate system.
    PlotMouseTool(PlotAxis xAxis, PlotAxis yAxis,
            PlotCoordinate::System coordSys = PlotCoordinate::WORLD) :
            PlotTool(xAxis, yAxis, coordSys) { }
    
    // Destructor.
    virtual ~PlotMouseTool() { }


    // Event handling methods.
    // <group>
    virtual void handleSelect(const PlotSelectEvent& event) {
        handleMouseEvent(event); }
    virtual void handleClick(const PlotClickEvent& event) {
        handleMouseEvent(event); }
    virtual void handleMousePress(const PlotMousePressEvent& event) {
        handleMouseEvent(event); }
    virtual void handleMouseRelease(const PlotMouseReleaseEvent& event) {
        handleMouseEvent(event); }
    virtual void handleMouseDrag(const PlotMouseDragEvent& event) {
        handleMouseEvent(event); }
    virtual void handleMouseMove(const PlotMouseMoveEvent& event) {
        handleMouseEvent(event); }
    virtual void handleWheel(const PlotWheelEvent& event) {
        handleMouseEvent(event); }
    // </group>
    
    
    // ABSTRACT METHODS //
    
    // Handles the given mouse event.  Guaranteed to be one of the mouse
    // events (select, click, press, release, drag, move, wheel).  The
    // implementing class should also update the last event handled flag as
    // necessary.
    virtual void handleMouseEvent(const PlotEvent& event) = 0;
};
INHERITANCE_POINTER2(PlotMouseTool, PlotMouseToolPtr, PlotTool, PlotToolPtr)






///////////////////////////
// CONCRETE TOOL CLASSES //
///////////////////////////




// Was in PlotStandardMouseToolGroup, but needed here, so it can be  passed
// to setActiveTool, which needs it to adjust the Select tool to either work
// normal or as Subtraction tool.

// Enum for standard tools in group.
enum ToolCode {
    SELECT_TOOL,   
    SUBTRACT_TOOL,   
    ZOOM_TOOL, 
    PAN_TOOL, 
    FLAGALL_TOOL,
    NONE_TOOL
};
    




// A PlotSelectTool is a concrete subclass of PlotMouseTool that mainly handles
// select events.  Note that plotting implementations may wish to override
// this class with an implementation-specific version that may be more
// efficient.  PlotSelectTool is responsible for:
// 1) managing the select line (the line show while the user is click-dragging
//    to select a region on the canvas) and cursors,
// 2) keeping track of selected regions,
// 3) showing/hiding the regions on the canvas, and
// 4) notifying any interested classes whenever the selected regions changes.
class PlotSelectTool : public virtual PlotMouseTool {
public:
    // Constructor which takes the tool's coordinate system.
    PlotSelectTool(PlotCoordinate::System system = PlotCoordinate::WORLD);
    
    // Constructor which takes the tool's axes and coordinate system.
    PlotSelectTool(PlotAxis xAxis, PlotAxis yAxis,
                   PlotCoordinate::System system = PlotCoordinate::WORLD);
    
    // Destructor.
    virtual ~PlotSelectTool();
    
    // Adds the given notifier.  This object will be notified when the list
    // of selected regions changes (either by adding one from the mouse, or
    // clearing them).
    virtual void addNotifier(PlotSelectToolNotifier* notifier);
    
    // Sets the selection line to the given.
    // <group>
    virtual void setSelectLine(PlotLinePtr line);
    virtual void setSubtractLine(PlotLinePtr line);
    virtual void setSelectLine(bool on = true);
    // </group>
    
    // Sets the attributes for drawing selected regions.
    // <group>
    virtual void setDrawRects(bool on = true);
    virtual void setRectLine(PlotLinePtr line);
    virtual void setRectFill(PlotAreaFillPtr fill);
    // </group>
    
    // Selected regions.
    // <group>
    virtual unsigned int numSelectedRects() const;
    virtual void getSelectedRects( 
                std::vector<double>& upperLeftXs,
                std::vector<double>& upperLeftYs, 
                std::vector<double>& lowerRightXs,
                std::vector<double>& lowerRightYs,
                PlotCoordinate::System system = PlotCoordinate::WORLD)  const;
    virtual std::vector<PlotRegion> getSelectedRects(
                PlotCoordinate::System system = PlotCoordinate::WORLD)  const;
    virtual void clearSelectedRects();
    virtual int getSelectedRectCount();
    // </group>
    
    // Overrides PlotTool::setActive().
    virtual void setActive(bool active = true);
    
    // Implements PlotMouseTool::handleMouseEvent().
    virtual void handleMouseEvent(const PlotEvent& event);
    
    bool inSubtractionMode()      { return m_subtraction_mode; }



    bool m_subtraction_mode;


protected:
    // Notifiers.
    std::vector<PlotSelectToolNotifier*> m_notifiers;
    
    // Copy of selection line to set on the canvas, or NULL if none has been
    // set.
    PlotLinePtr m_selLine;
    PlotLinePtr m_subLine;
    
    // Whether or not to draw selected regions on the canvas.
    bool m_drawRects;
    
    // Line for drawing selected regions, or NULL if none has been set.
    PlotLinePtr m_rectLine;
    
    // Area fill for drawing selected regions, or NULL if none has been set.
    PlotAreaFillPtr m_rectFill;
    
    // Selected regions.
    std::vector<PlotShapeRectanglePtr> m_rects;
    

    
    // Overrides PlotTool::attach().
    virtual void attach(PlotCanvas* canvas);
    
    // Overrides PlotTool::detach().
    virtual void detach();
};


INHERITANCE_POINTER(PlotSelectTool, PlotSelectToolPtr, PlotMouseTool,
                    PlotMouseToolPtr, PlotTool, PlotToolPtr)









// A PlotZoomTool is a concrete subclass of PlotMouseTool that provides
// convenient zooming functionality.  Standard behavior is to zoom on a
// select event, go through the zoom stack on a wheel event, go to the zoom
// stack base on a right click, and zoom in 50% centered on a double-click.
// Note that plotting implementations may wish to override this class with an
// implementation-specific version that may be more efficient.  A PlotZoomTool
// is responsible for:
// 1) managing behavior described above,
// 2) managing a zoom stack,
// 3) managing the canvas's select line/cursor, and
// 3) notifying interested objects when the zoom changes.
class PlotZoomTool : public virtual PlotMouseTool {
public:
    // Constructor which takes the tool's coordinate system.
    PlotZoomTool(PlotCoordinate::System sys = PlotCoordinate::WORLD);
    
    // Constructor which takes the tool's axes and coordinate system.
    PlotZoomTool(PlotAxis xAxis, PlotAxis yAxis,
                 PlotCoordinate::System sys = PlotCoordinate::WORLD);
    
    // Destructor.
    virtual ~PlotZoomTool();
    
    // Adds the given notifier.  This object will be notified when the zoom
    // changes.
    virtual void addNotifier(PlotZoomToolNotifier* notifier);
    
    // Sets the selection line to the given.
    // <group>
    virtual void setSelectLine(PlotLinePtr line);
    virtual void setSelectLine(bool on = true);
    // </group>
    
    // Gets the zoom stack.
    virtual std::vector<PlotRegion> getZoomStack(PlotCoordinate::System sytem =
                                            PlotCoordinate::WORLD) const;
    
    // Gets the zoom stack index.
    virtual unsigned int getStackIndex() const;
    
    // Overrides PlotTool::setActive().
    virtual void setActive(bool active = true);
    
    // Implements PlotMouseTool::handleMouseEvent().
    virtual void handleMouseEvent(const PlotEvent& event);
    
    // Overrides PlotTool::reset().
    virtual void reset();
    
protected:
    // Notifiers.
    std::vector<PlotZoomToolNotifier*> m_notifiers;
    
    // Copy of canvas selection line, or NULL if none has been set.
    PlotLinePtr m_selLine;
    
    // Common canvas stack.
    PlotAxesStack* m_stack;
    
    
    // Overrides PlotTool::attach().
    virtual void attach(PlotCanvas* canvas);
    
    // Overrides PlotTool::detach().
    virtual void detach();
    
    // Notifies all registered listeners that the zoom has changed.
    virtual void notifyWatchers();
};
INHERITANCE_POINTER(PlotZoomTool, PlotZoomToolPtr, PlotMouseTool,
                    PlotMouseToolPtr, PlotTool, PlotToolPtr)




// A PlotPanTool is a concrete subclass of PlotMouseTool that provides
// convenient panning functionality.  Standard behavior is to pan the canvas
// on a drag event, go through the pan stack on a wheel event, and go to the
// pan stack base on a right click.  Note that plotting implementations may
// wish to override this class with an implementation-specific version that may
// be more efficient.  A PlotPanTool is responsible for:
// 1) managing behavior described above,
// 2) managing a pan stack,
// 3) managing the canvas's cursor, and
// 4) notifying interested objects when the pan changes.
class PlotPanTool : public virtual PlotMouseTool {
public:
    // Constructor which takes the tool's coordinate system.
    PlotPanTool(PlotCoordinate::System sys = PlotCoordinate::WORLD);
    
    // Constructor which takes the tool's axes and coordinate system.
    PlotPanTool(PlotAxis xAxis, PlotAxis yAxis,
                PlotCoordinate::System sys = PlotCoordinate::WORLD);
    
    // Destructor.
    virtual ~PlotPanTool();
    
    // Adds the given notifier.  This object will be notified when the pan
    // changes.
    virtual void addNotifier(PlotPanToolNotifier* notifier);
    
    // Gets the pan stack.
    virtual std::vector<PlotRegion> getPanStack(PlotCoordinate::System system =
                                           PlotCoordinate::WORLD) const;
    
    // Gets the pan stack index.
    virtual unsigned int getStackIndex() const;
    
    // Overrides PlotTool::setActive().
    virtual void setActive(bool active = true);
    
    // Implements PlotMouseTool::handleMouseEvent().
    virtual void handleMouseEvent(const PlotEvent& event);
    
    // Overrides PlotTool::reset().
    virtual void reset();
    
protected:
    // Notifiers.
    std::vector<PlotPanToolNotifier*> m_notifiers;
    
    // Whether we're in dragging mode or not.
    bool m_inDraggingMode;
    
    // Last coordinate in dragging mode.
    PlotCoordinate m_lastCoord;
    
    // Common canvas stack.
    PlotAxesStack* m_stack;
    
    
    // Overrides PlotTool::attach().
    virtual void attach(PlotCanvas* canvas);
    
    // Overrides PlotTool::detach().
    virtual void detach();
    
    // Notifies all registered listeners that the pan has changed.
    virtual void notifyWatchers();
};
INHERITANCE_POINTER(PlotPanTool, PlotPanToolPtr, PlotMouseTool,
                    PlotMouseToolPtr, PlotTool, PlotToolPtr)




// A PlotTrackerTool is a concrete subclass of PlotMouseTool that provides
// convenient tracker functionality.  Note that plotting implementations may
// wish to override this class with an implementation-specific version that may
// be more efficient.  A PlotTrackerTool can:
// 1) show a label with the current position hovering over the mouse,
// 2) let an external class handle the tracking via notifications, or
// 3) both.
class PlotTrackerTool : public virtual PlotMouseTool {
    friend class PlotStandardMouseToolGroup; // Why is this necessary to access
                                             // attach() and detach()? >:(
    
public:
    // Static //
    
    // Returns a casacore::String for the given position in the given format, with the
    // given canvas and axes.
    static casacore::String formattedString(const casacore::String& format, double x, double y,
                       PlotCanvas* canvas, PlotAxis xAxis, PlotAxis yAxis);
    
    
    // Non-Static //
    
    // Constructor which takes the tool's coordinate system.
    PlotTrackerTool(PlotCoordinate::System sys = PlotCoordinate::WORLD);
        
    // Constructor which takes the tool's axes and coordinate system.
    PlotTrackerTool(PlotAxis xAxis, PlotAxis yAxis,
                    PlotCoordinate::System sys = PlotCoordinate::WORLD);
    
    // Destructor.
    virtual ~PlotTrackerTool();
    
    // Adds the given notifier.  This object will be notified when the tracker
    // changes (and a new coordinate is ready for display).
    virtual void addNotifier(PlotTrackerToolNotifier* notifier);
    
    // Returns true if the tracker text is drawn on the canvas, false otherwise.
    virtual bool drawsText() const;
    
    // Sets whether the tracker will draw the text on the canvas or not.
    virtual void setDrawText(bool draw = true);
    
    // Sets the tracker text format to the given.  The following tags can be
    // used in the format:
    // * %%x%% : x value
    // * %%y%% : y values
    // * %%pX%% : sets the precision to X for any following numbers.
    // NOTICE: if the x or y value is a date, the date format set on the
    // canvas this tool is attached to will be used to display the value.
    // Default format is "(%%x%%, %%y%%)".
    virtual void setFormat(const casacore::String& format);
    
    // Returns the formatted tracker text for the given position.
    virtual casacore::String formattedString(double x, double y) {
        return formattedString(m_format, x, y, m_canvas, m_xAxis, m_yAxis); }
    
    // Returns the annotation used to store the coordinates/text.
    virtual PlotAnnotationPtr getAnnotation();
    
    // Gets the tracker's current position.
    virtual PlotCoordinate getCoordinate(PlotCoordinate::System =
                                         PlotCoordinate::WORLD) const;
    
    // Overrides PlotTool::setActive().
    virtual void setActive(bool active = true);
    
    // Implements PlotMouseTool::handleMouseEvent().
    virtual void handleMouseEvent(const PlotEvent& event);
    
    int getSelectedRectCount() const;
    std::vector<PlotRegion> getSelectedRects() const;

protected:
    // Notifiers.
    std::vector<PlotTrackerToolNotifier*> m_notifiers;
    
    // Annotation that holds current position (even if not drawn on canvas).
    PlotAnnotationPtr m_annotation;
    
    // Whether to draw the annotation or not.
    bool m_drawText;
    
    // Tracker text format.
    casacore::String m_format;
    
    // Overrides PlotTool::attach().
    virtual void attach(PlotCanvas* canvas);
    
    // Overrides PlotTool::detach().
    virtual void detach();
    
    // Notifies all registered listeners that the tracker has changed.
    virtual void notifyWatchers();
    
    
    // Static //
    
    // casacore::Format constants.
    // <group>
    static const casacore::String FORMAT_DIVIDER;
    static const casacore::String FORMAT_X, FORMAT_Y;
    static const casacore::String FORMAT_PRECISION;
    static const casacore::String DEFAULT_FORMAT;
    // </group>
};
INHERITANCE_POINTER(PlotTrackerTool, PlotTrackerToolPtr, PlotMouseTool,
                    PlotMouseToolPtr, PlotTool, PlotToolPtr)


// A PlotFlagAllTool is a concrete subclass of PlotMouseTool that handles
// one-click data flag functionality.
// PlotFlagAllTool is responsible for:
// 1) the behavior described above
class PlotFlagAllTool : public virtual PlotMouseTool {
public:
    // enum for canvas state
    enum PPFlagType {
      PPFLAG_FLAG,
      PPFLAG_UNFLAG,
      PPFLAG_NONE
    };

    // Constructor which takes the tool's coordinate system.
    PlotFlagAllTool(PlotCoordinate::System system = PlotCoordinate::WORLD);

    // Constructor which takes the tool's axes and coordinate system.
    PlotFlagAllTool(PlotAxis xAxis, PlotAxis yAxis,
                   PlotCoordinate::System system = PlotCoordinate::WORLD);

    // Destructor.
    virtual ~PlotFlagAllTool();

    // Implements PlotMouseTool::handleMouseEvent().
    virtual void handleMouseEvent(const PlotEvent& event);

    // Sets the attributes for updating background
    virtual void setUpdateBackground(bool on = true);

    // Inquiry if update of background is active
    virtual bool isUpdateBackgroundActive();

    // Manipulate mark
    void clearMark();

    // Inquiry if it is marked
    bool isMarkedForFlag() const;
    bool isMarkedForUnflag() const;

    // Inquiry if bgcolor is changed
    bool isBackgroundColorChanged() const;

    //
    void setAllFlagged();

protected:
    // boolean flag for whether update of background is active
    bool m_draw;

    // boolean flag for background color
    bool m_bgcolor_changed;

    // boolean flag for whether canvas is marked for flag
    PlotFlagAllTool::PPFlagType m_marked;

    // keep default background setting
    PlotAreaFillPtr m_defaultBackground;

private:
    // internal methods for shared operations
    void markAsFlag();
    void markAsUnflag();

};
INHERITANCE_POINTER(PlotFlagAllTool, PlotFlagAllToolPtr, PlotMouseTool,
                    PlotMouseToolPtr, PlotTool, PlotToolPtr)


///////////////////////////////
// TOOL NOTIFICATION CLASSES //
///////////////////////////////


// Interface for objects that want to be notified when the selection tool
// changes.
class PlotSelectToolNotifier {
    friend class PlotSelectTool;
    
public:
    PlotSelectToolNotifier() { }
    virtual ~PlotSelectToolNotifier() { }
    
protected:
    // This method is called AFTER the selection has been added.
    virtual void notifySelectionAdded(PlotSelectTool& tool) = 0;
};


// Interface for objects that want to be notified when the zoom tool
// changes.
class PlotZoomToolNotifier {
    friend class PlotZoomTool;
    
public:
    PlotZoomToolNotifier() { }
    virtual ~PlotZoomToolNotifier() { }
    
protected:
    // This method is called AFTER the canvas has been zoomed.
    virtual void notifyZoomChanged(PlotZoomTool& tool) = 0;
};


// Interface for objects that want to be notified when the pan tool
// changes.
class PlotPanToolNotifier {
    friend class PlotPanTool;
    
public:
    PlotPanToolNotifier() { }
    virtual ~PlotPanToolNotifier() { }
    
protected:
    // This method is called AFTER the canvas has been panned.
    virtual void notifyPanChanged(PlotPanTool& tool) = 0;
};


// Interface for objects that want to be notified when the tracker tool
// changes.
class PlotTrackerToolNotifier {
    friend class PlotTrackerTool;
    
public:
    PlotTrackerToolNotifier() { }
    virtual ~PlotTrackerToolNotifier() { }
    
protected:
    // This method is called AFTER the tracker has been updated.
    virtual void notifyTrackerChanged(PlotTrackerTool& tool) = 0;
};


////////////////////////
// TOOL GROUP CLASSES //
////////////////////////


// A PlotMouseToolGroup provides an interface for a group of PlotMouseTools
// where only one (or none) is active at a time.
class PlotMouseToolGroup : public virtual PlotMouseTool {
public:
    // Constructor for empty group.
    PlotMouseToolGroup();
    
    // Destructor.
    virtual ~PlotMouseToolGroup();    
       
    // Returns the number of tools in the group.
    unsigned int numTools() const;    
    
    // Returns the tools in the group.
    std::vector<PlotMouseToolPtr> tools() const;
    
    // Adds the given tool to the group and returns its index.  If makeActive
    // is true, the given tool becomes the group's active tool.
    // Note (dec 2010): used to take 2nd arg, boolean, to make tool active.
    // This is confusing design.  Caller of addTool should just call  setActiveTool(tool)
    // after calling addTool() if it wants the tool to become active.
    // In practice, source code does not anywhere call addTool with make_active=true.
    unsigned int addTool(PlotMouseToolPtr tool);
    
    // Removes the given tool from the group, and returns true on success.
    // <group>
    bool removeTool(PlotMouseToolPtr tool);    
    bool removeTool(unsigned int index) { return removeTool(toolAt(index)); }
    // </group>
    
    // Returns the tool at the given index, or NULL for invalid.
    PlotMouseToolPtr toolAt(unsigned int index) const;
    
    // Returns the index of the given tool, or numTools() for invalid.
    unsigned int indexOf(PlotMouseToolPtr tool) const;
    
    // Returns true if the given tool is in this group, false otherwise.
    bool containsTool(PlotMouseToolPtr tool) const {
        return indexOf(tool) < m_tools.size(); }
    
    // Returns the currently active tool, or NULL for none.
    PlotMouseToolPtr activeTool() const { return m_activeTool; }
    
    // Sets the active tool to the given.  If the given tool is not in the
    // group it is first added.
    // Toolcode is optional - meaningful only if tool has double usage, like
    // Select tool which doubles as the Subtraction tool.  Otherwise, just stuff NONE_TOOL
    // in for that arg.
    void setActiveTool(PlotMouseToolPtr tool,  ToolCode toolcode=NONE_TOOL);
    
    // Sets the active tool to the one at the given index.
    void setActiveTool(unsigned int index, ToolCode c=NONE_TOOL)   { 
            setActiveTool(toolAt(index), c); 
            }
    
    // Overrides PlotTool::setActive().
    void setActive(bool isActive = true);
    
    // Overrides PlotTool::setBlocking().
    void setBlocking(bool blocking = true);    
    
    // Implements PlotMouseTool::handleMouseEvent().
    void handleMouseEvent(const PlotEvent& event);
    
    // Overrides PlotMouseTool's event handling methods.
    // <group>
    void handleSelect(const PlotSelectEvent& event);
    void handleClick(const PlotClickEvent& event);
    void handleMousePress(const PlotMousePressEvent& event);
    void handleMouseRelease(const PlotMouseReleaseEvent& event);
    void handleMouseDrag(const PlotMouseDragEvent& event);
    void handleMouseMove(const PlotMouseMoveEvent& event);
    void handleWheel(const PlotWheelEvent& event);
    // </group>
    
    // Overrides PlotTool::getXAxis().
    PlotAxis getXAxis() const;
    
    // Overrides PlotTool::getYAxis().
    PlotAxis getYAxis() const;
    
    // Overrides PlotTool::getCoordinateSystem().
    PlotCoordinate::System getCoordinateSystem() const;
    
    // Overrides PlotTool::lastEventWasHandled().
    bool lastEventWasHandled() const;
    
    // Overrides PlotTool::reset().
    void reset();
    
protected:
    // All tools.
    std::vector<PlotMouseToolPtr> m_tools;
    
    // Active tool (or NULL for no active tool).
    PlotMouseToolPtr m_activeTool;
    
    // Overrides PlotTool::attach().
    virtual void attach(PlotCanvas* canvas);
    
    // Overrides PlotTool::detach().
    virtual void detach();
};
INHERITANCE_POINTER(PlotMouseToolGroup, PlotMouseToolGroupPtr, PlotMouseTool,
                    PlotMouseToolPtr, PlotTool, PlotToolPtr)


// PlotStandardMouseToolGroup is a specialized PlotMouseToolGroup where the
// tools in the group are:
// 1) select,
// 2) zoom, and
// 3) pan.
// A tracker is also provided that is not in the group so that it can be active
// at the same time other tools are active.
class PlotStandardMouseToolGroup : public PlotMouseToolGroup {
public:
    // Static //
    
    // Non-Static //
    
    // Constructor which creates default tools with the given coordinate
    // system, and sets the active tool to the given.
    PlotStandardMouseToolGroup(ToolCode activeTool = NONE_TOOL,
            PlotCoordinate::System system = PlotCoordinate::WORLD);
    
    // Constructor which creates default tools with the given coordinate
    // system and axes, and sets the active tool to the given.
    PlotStandardMouseToolGroup(PlotAxis xAxis, PlotAxis yAxis,
            ToolCode activeTool = NONE_TOOL,
            PlotCoordinate::System system = PlotCoordinate::WORLD);
    
    // Constructor which uses the given tools (or creates default tools if the
    // given ones are invalid), and sets the active tool to the given.
    PlotStandardMouseToolGroup(PlotSelectToolPtr selectTool,
                               PlotZoomToolPtr zoomTool,
                               PlotPanToolPtr panTool,
                               PlotFlagAllToolPtr flagAllTool,
                               PlotTrackerToolPtr trackerTool,
                               ToolCode activeTool = NONE_TOOL);
    
    // Destructor.
    ~PlotStandardMouseToolGroup();
    
    // Gets/sets the active standard tool.
    // <group>
    void setActiveTool(ToolCode tool);
    ToolCode activeToolType() const;
    // </group>
    
    // Provides access to the tracker.
    // <group>
    void turnTracker(bool on);
    bool trackerIsOn() const;
    void turnTrackerDrawText(bool on);
    bool trackerDrawsText() const;
    // </group>
    
    int getSelectedRectCount();
    std::vector<PlotRegion> getSelectedRects();
    void clearSelectedRects();

    // methods related to per-panel flag mode
    void clearMark();
    bool isMarkedForFlag();
    bool isMarkedForUnflag();
    bool isBackgroundColorChanged();
    void setAllFlagged();

    // Provides access to the individual tools.  Note: this should be avoided
    // if possible.
    // <group>
    PlotSelectToolPtr selectTool();
    PlotZoomToolPtr zoomTool();
    PlotPanToolPtr panTool();
    PlotFlagAllToolPtr flagAllTool();
    PlotTrackerToolPtr trackerTool();
    // </group>
    
    // Overrides PlotMouseToolGroup handler methods to give events to the
    // tracker first.
    // <group>
    void handleMouseEvent(const PlotEvent& event) {
        if(m_tracker->isActive()) m_tracker->handleMouseEvent(event);
        PlotMouseToolGroup::handleMouseEvent(event); }
        
    void handleSelect(const PlotSelectEvent& event) {
        if(m_tracker->isActive()) m_tracker->handleSelect(event);
        PlotMouseToolGroup::handleSelect(event); }
        
    void handleClick(const PlotClickEvent& event) {
        if(m_tracker->isActive()) m_tracker->handleClick(event);
        PlotMouseToolGroup::handleClick(event); }
        
    void handleMousePress(const PlotMousePressEvent& event) {
        if(m_tracker->isActive()) m_tracker->handleMousePress(event);
        PlotMouseToolGroup::handleMousePress(event); }
        
    void handleMouseRelease(const PlotMouseReleaseEvent& event) {
        if(m_tracker->isActive()) m_tracker->handleMouseRelease(event);
        PlotMouseToolGroup::handleMouseRelease(event); }
        
    void handleMouseDrag(const PlotMouseDragEvent& event) {
        if(m_tracker->isActive()) m_tracker->handleMouseDrag(event);
        PlotMouseToolGroup::handleMouseDrag(event); }
        
    void handleMouseMove(const PlotMouseMoveEvent& event) {
        if(m_tracker->isActive()) m_tracker->handleMouseMove(event);
        PlotMouseToolGroup::handleMouseMove(event); }
        
    void handleWheel(const PlotWheelEvent& event) {
        if(m_tracker->isActive()) m_tracker->handleWheel(event);
        PlotMouseToolGroup::handleWheel(event); }
    // </group>
    
protected:
    // Overrides PlotMouseToolGroup::attach().
    void attach(PlotCanvas* canvas);
    
    // Overrides PlotMouseToolGroup::detach().
    void detach();
    
private:
    // Tracker.
    PlotTrackerToolPtr m_tracker;
};
INHERITANCE_POINTER(PlotStandardMouseToolGroup, PlotStandardMouseToolGroupPtr,
                    PlotMouseToolGroup, PlotMouseToolGroupPtr,
                    PlotTool, PlotToolPtr)

}

#endif /* PLOTTOOL_H_ */
