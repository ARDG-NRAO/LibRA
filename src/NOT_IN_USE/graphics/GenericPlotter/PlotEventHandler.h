//# PlotEventHandler.h: Class to handle different types of interaction events.
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
#ifndef PLOTEVENTHANDLER_H_
#define PLOTEVENTHANDLER_H_

#include <graphics/GenericPlotter/PlotEvent.h>

namespace casa {

// Base class for a handler for PlotSelectEvent.
class PlotSelectEventHandler {
public:
    PlotSelectEventHandler() { }
    
    virtual ~PlotSelectEventHandler() { }
    
    // Handle the given event.
    virtual void handleSelect(const PlotSelectEvent& event) = 0;
};

// Base class for a handler for PlotClickEvent.
class PlotClickEventHandler {
public:
    PlotClickEventHandler() { }
    
    virtual ~PlotClickEventHandler() { }
    
    // Handle the given event.
    virtual void handleClick(const PlotClickEvent& event) = 0;
};

// Base class for a handler for PlotMousePressEvent.
class PlotMousePressEventHandler {
public:
    PlotMousePressEventHandler() { }
    
    virtual ~PlotMousePressEventHandler() { }
    
    // Handle the given event.
    virtual void handleMousePress(const PlotMousePressEvent& event) = 0;
};

// Base class for a handler for PlotMouseReleaseEvent.
class PlotMouseReleaseEventHandler {
public:
    PlotMouseReleaseEventHandler() { }
    
    virtual ~PlotMouseReleaseEventHandler() { }
    
    // Handle the given event.
    virtual void handleMouseRelease(const PlotMouseReleaseEvent& event) = 0;
};

// Base class for a handler for PlotMouseDragEvent.
class PlotMouseDragEventHandler {
public:
    PlotMouseDragEventHandler() { }
    
    virtual ~PlotMouseDragEventHandler() { }
    
    // Handle the given event.
    virtual void handleMouseDrag(const PlotMouseDragEvent& event) = 0;
};

// Base class for a handler for a PlotMouseMoveEvent.
class PlotMouseMoveEventHandler {
public:
    PlotMouseMoveEventHandler() { }
    
    virtual ~PlotMouseMoveEventHandler() { }
    
    // Handle the given event.
    virtual void handleMouseMove(const PlotMouseMoveEvent& event) = 0;
};

// Base class for a handler for PlotWheelEvent.
class PlotWheelEventHandler {
public:
    PlotWheelEventHandler() { }
    
    virtual ~PlotWheelEventHandler() { }
    
    // Handle the given event
    virtual void handleWheel(const PlotWheelEvent& event) = 0;
};

// Base class for a handler for PlotKeyEvent.
class PlotKeyEventHandler {
public:
    PlotKeyEventHandler() { }
    
    virtual ~PlotKeyEventHandler() { }
    
    // Handle the given event.
    virtual void handleKey(const PlotKeyEvent& event) = 0;
};

// Base class for a handler for PlotResizeEvent.
class PlotResizeEventHandler {
public:
    PlotResizeEventHandler() { }
    
    virtual ~PlotResizeEventHandler() { }
    
    // Handle the given event.
    virtual void handleResize(const PlotResizeEvent& event) = 0;
};

// Base class for a handler for PlotButtonEvent.
class PlotButtonEventHandler {
public:
    PlotButtonEventHandler() { }
    
    virtual ~PlotButtonEventHandler() { }
    
    // Handle the given event.
    virtual void handleButton(const PlotButtonEvent& event) = 0;
};

// Base class for a handler for PlotCheckboxEvent.
class PlotCheckboxEventHandler {
public:
    PlotCheckboxEventHandler() { }
    
    virtual ~PlotCheckboxEventHandler() { }
    
    // Handle the given event.
    virtual void handleCheckbox(const PlotCheckboxEvent& event) = 0;
};


// Smart Pointer Definitions //

typedef casacore::CountedPtr<PlotSelectEventHandler> PlotSelectEventHandlerPtr;
typedef casacore::CountedPtr<PlotClickEventHandler> PlotClickEventHandlerPtr;
typedef casacore::CountedPtr<PlotMousePressEventHandler> PlotMousePressEventHandlerPtr;
typedef casacore::CountedPtr<PlotMouseReleaseEventHandler>
        PlotMouseReleaseEventHandlerPtr;
typedef casacore::CountedPtr<PlotMouseDragEventHandler> PlotMouseDragEventHandlerPtr;
typedef casacore::CountedPtr<PlotMouseMoveEventHandler> PlotMouseMoveEventHandlerPtr;
typedef casacore::CountedPtr<PlotWheelEventHandler> PlotWheelEventHandlerPtr;
typedef casacore::CountedPtr<PlotKeyEventHandler> PlotKeyEventHandlerPtr;
typedef casacore::CountedPtr<PlotResizeEventHandler> PlotResizeEventHandlerPtr;
typedef casacore::CountedPtr<PlotButtonEventHandler> PlotButtonEventHandlerPtr;
typedef casacore::CountedPtr<PlotCheckboxEventHandler> PlotCheckboxEventHandlerPtr;

}

#endif /*PLOTEVENTHANDLER_H_*/
