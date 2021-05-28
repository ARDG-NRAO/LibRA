//# PlotEvent.h: Classes for interaction events.
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
#ifndef PLOTEVENT_H_
#define PLOTEVENT_H_

#include <graphics/GenericPlotter/PlotOptions.h>

namespace casa {

//# Forward Declarations
class Plotter;
class PlotCanvas;
class PlotButton;
class PlotCheckbox;

// Base class for an event.
class PlotEvent {
public:
    PlotEvent() { }
    
    virtual ~PlotEvent() { }
    
    // Return the origin of the object that spawned this event.
    virtual void* origin() const = 0;
};


// Event for when the user selects a region on a plot canvas with the mouse.
class PlotSelectEvent : public virtual PlotEvent {
public:
    // Constructor that takes the originating canvas and the selected region.
    PlotSelectEvent(PlotCanvas* canvas, const PlotRegion& region);
    
    // Destructor.
    ~PlotSelectEvent();
    
    // Overrides PlotEvent::origin().
    void* origin() const { return canvas(); }
    
    // Returns the canvas upon which the selection was made.
    PlotCanvas* canvas() const;
    
    // Returns the region that was selected.
    PlotRegion region() const;
    
protected:
    PlotCanvas* m_canvas; // Canvas
    PlotRegion m_region;  // Selected Region
};


// Generic mouse event where the canvas is the origin, and the type, button,
// and location of the event are provided.
class PlotMouseEvent : public virtual PlotEvent {
public:
    // Mouse event types.
    enum Type {
        CLICK,   // press and release (can be double)
        PRESS,   // press
        RELEASE, // release
        DRAG,    // mouse movement while button is down
        MOVE     // mouse movement any time
    };
    
    // Mouse button types that we care about.
    enum Button {
        SINGLE,  // left button, single click
        DOUBLE,  // left button, double click
        CONTEXT, // right button
        MIDDLE   // middle button
    };
    
    // Constructor which takes the originating canvas, type, button, and
    // location.
    PlotMouseEvent(PlotCanvas* canvas, Type type, Button button,
                   const PlotCoordinate& coord);
    
    // Destructor.
    virtual ~PlotMouseEvent();
    
    // Overrides PlotEvent::origin().
    void* origin() const { return canvas(); }
    
    // Canvas origin of event.
    PlotCanvas* canvas() const;
    
    // Type of event.
    Type type() const;
    
    // Button of event.
    Button button() const;
    
    // Location of event.
    PlotCoordinate where() const;
    
protected:
    PlotCanvas* m_canvas;   // Canvas
    Type m_type;            // Type
    Button m_button;        // Button
    PlotCoordinate m_coord; // Location
};


// Convenience class for mouse clicks.
class PlotClickEvent : public PlotMouseEvent {
public:    
    PlotClickEvent(PlotCanvas* canvas, Button button,
            const PlotCoordinate& coord) :
            PlotMouseEvent(canvas, CLICK, button, coord) { }
    
    ~PlotClickEvent() { }
};

// Convenience class for mouse presses.
class PlotMousePressEvent : public PlotMouseEvent {
public:
    PlotMousePressEvent(PlotCanvas* canvas, Button button,
            const PlotCoordinate& coord) :
            PlotMouseEvent(canvas, PRESS, button, coord) { }
    
    ~PlotMousePressEvent() { }
};

// Convenience class for mouse releases.
class PlotMouseReleaseEvent : public PlotMouseEvent {
public:
    PlotMouseReleaseEvent(PlotCanvas* canvas, Button button,
            const PlotCoordinate& coord) :
            PlotMouseEvent(canvas, RELEASE, button, coord) { }
    
    ~PlotMouseReleaseEvent() { }
};

// Convenience class for mouse drags.
class PlotMouseDragEvent : public PlotMouseEvent {
public:
    PlotMouseDragEvent(PlotCanvas* canvas, Button button,
            const PlotCoordinate& coord) :
            PlotMouseEvent(canvas, DRAG, button, coord) { }
    
    ~PlotMouseDragEvent() { }
};

// Convenience class for mouse moves.
class PlotMouseMoveEvent : public PlotMouseEvent {
public:
    PlotMouseMoveEvent(PlotCanvas* canvas, Button button,
            const PlotCoordinate& coord) :
            PlotMouseEvent(canvas, MOVE, button, coord) { }
    
    ~PlotMouseMoveEvent() { }
};


// Event for when the user scrolls the scroll wheel.  The scroll event contains
// a "delta" which is negative when the user scrolls back and positive when
// the user scrolls forward.  The number indicates how many "clicks" the user
// scrolled.
class PlotWheelEvent : public virtual PlotEvent {
public:
    // Constructor which takes the originating canvas, the wheel delta, and the
    // location.
    PlotWheelEvent(PlotCanvas* canvas, int delta, const PlotCoordinate& c);
    
    // Destructor.
    ~PlotWheelEvent();
    
    // Overrides PlotEvent::origin().
    void* origin() const { return canvas(); }
    
    // Canvas that was scrolled upon.
    PlotCanvas* canvas() const;
    
    // Scroll delta.
    int delta() const;
    
    // Where the scroll happened.
    PlotCoordinate where() const;
    
protected:
    PlotCanvas* m_canvas;   // Canvas
    int m_delta;            // Scroll delta
    PlotCoordinate m_coord; // Location
};


// Event for when the user makes a keyboard command when a canvas has focus.
// An example of a keyboard command would be "ctrl+s" or "n" or "shift+g" or
// "F3".  Note that this event is only valid for key commands that can be
// represented with a char (or, in the case of the F1-F12 keys, the F modifier
// plus the character representation of the number).
class PlotKeyEvent : public virtual PlotEvent {
public:
    // Modifier for a key press
    enum Modifier {
        CONTROL, SHIFT, ALT, F
    };
    
    // Constructor that takes the originating canvas, the key pressed, and any
    // modifiers.
    PlotKeyEvent(PlotCanvas* canvas, char key, const std::vector<Modifier>& mods);
    
    // Destructor.
    ~PlotKeyEvent();
    
    // Overrides PlotEvent::origin().
    void* origin() const { return canvas(); }
    
    // Returns the originating canvas.
    PlotCanvas* canvas() const;
    
    // Returns the pressed character key.
    char key() const;
    
    // Returns the key modifiers.
    std::vector<Modifier> modifiers() const;
    
    // Returns a casacore::String representation of this key event.
    casacore::String toString() const;
    
    
    // Converts between KeyModifier and its casacore::String representation.
    // <group>
    static casacore::String modifier(Modifier f) {
        switch(f) {
        case CONTROL: return "Ctrl";
        case ALT: return "Alt";
        case SHIFT: return "Shift";
        case F: return "F";
        
        default: return "?";
        }
    }    
    static Modifier modifier(casacore::String f) {
        f.downcase();
        
        if(f == "ctrl" || f == "control") return CONTROL;
        if(f == "shift") return SHIFT;
        if(f == "alt") return ALT;
        if(f == "f" || f == "function") return F;
        
        return F;
    }
    // </group>
    
protected:
    PlotCanvas* m_canvas;    // Canvas
    char m_key;              // Character
    std::vector<Modifier> m_mods; // Modifiers
};


// Event for when a canvas or plotter is resized.
class PlotResizeEvent : public virtual PlotEvent {
public:
    // Canvas resize constructor, which takes the old and new sizes.
    PlotResizeEvent(PlotCanvas* canvas, int oldWidth, int oldHeight,
                    int newWidth, int newHeight);
    
    // Plotter resize constructor, which takes the old and new sizes.
    PlotResizeEvent(Plotter* plotter, int oldWidth, int oldHeight,
                    int newWidth, int newHeight);
    
    // Destructor.
    ~PlotResizeEvent();
    
    // Overrides PlotEvent::origin().
    void* origin() const {
        if(canvas() != NULL) return canvas();
        else                 return plotter();
    }
    
    // Canvas that was resized, or NULL if it was a plotter.
    PlotCanvas* canvas() const;
    
    // Plotter that was resize, or NULL if it was a canvas.
    Plotter* plotter() const;
    
    // Old size, in pixels.
    std::pair<int, int> oldSize() const;
    
    // New size, in pixels.
    std::pair<int, int> newSize() const;
    
protected:
    Plotter* m_plotter;   // Plotter (or NULL)
    PlotCanvas* m_canvas; // Canvas (or NULL)
    std::pair<int, int> m_old; // Old size
    std::pair<int, int> m_new; // New size
};


// Event for when a PlotButton is pressed.
class PlotButtonEvent : public virtual PlotEvent {
public:
    // Constructor which takes the originating button.
    PlotButtonEvent(PlotButton* button);
    
    // Destructor.
    ~PlotButtonEvent();
    
    // Overrides PlotEvent::origin().
    void* origin() const { return button(); }
    
    // Returns the button that was clicked.
    PlotButton* button() const;
    
protected:
    PlotButton* m_button; // button
};

// Event for when a PlotCheckbox is clicked.
class PlotCheckboxEvent : public virtual PlotEvent {
public:
    // Constructor which takes the originating checkbox.
    PlotCheckboxEvent(PlotCheckbox* checkbox);
    
    // Destructor.
    ~PlotCheckboxEvent();
    
    // Overrides PlotEvent::origin().
    void* origin() const { return checkbox(); }
    
    // Returns the checkbox that was clicked.
    PlotCheckbox* checkbox() const;
    
protected:
    PlotCheckbox* m_checkbox; // checkbox
};

}

#endif /*PLOTEVENT_H_*/
