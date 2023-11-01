//# PlotPanel.h: Custom plot panels and buttons.
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
#ifndef PLOTPANEL_H_
#define PLOTPANEL_H_

#include <graphics/GenericPlotter/PlotEventHandler.h>

namespace casa {

// Abstract superclass for any widget that goes on a PlotPanel.
class PlotWidget {
public:
    // Constructor.
    PlotWidget() { }
    
    // Destructor.
    virtual ~PlotWidget() { }
    
    // Returns true if the widget is currently enabled, false otherwise.
    // A widget button should be grayed out or not interact-able.
    virtual bool isEnabled() const = 0;
    
    // Enables/disables the widget.
    virtual void setEnabled(bool enabled = true) = 0;
    
    // Returns true if the widget is currently visible.
    virtual bool isVisible() const = 0;
    
    // Show/hide the widget.
    virtual void setVisible(bool visible = true) = 0;
    
    // Returns the tooltip for this widget.
    virtual casacore::String tooltip() const = 0;
    
    // Sets the tooltip for this widget.
    virtual void setTooltip(const casacore::String& text) = 0;
};
typedef casacore::CountedPtr<PlotWidget> PlotWidgetPtr;


// Generic class for a button that goes on a PlotPanel.  A button has
// properties that can be set, as well as registration for event
// handlers.  A button can only be seen/interacted with on a PlotPanel.
class PlotButton : public virtual PlotWidget {
public:
    // Constructor.
    PlotButton() { }
    
    // Destructor.
    virtual ~PlotButton() { }
    
    // Returns true if text is being shown on the button, false otherwise.
    virtual bool textShown() const = 0;
    
    // Show/hide the set text on the button.
    virtual void showText(bool show = true) = 0;
    
    // Currently set button text.  (May not be displayed.)
    virtual casacore::String text() const = 0;
    
    // Sets the button text.
    virtual void setText(const casacore::String& text) = 0;
    
    // Returns true if an image is being shown on the button, false otherwise.
    virtual bool imageShown() const = 0;
    
    // Show/hide the set image on the button.
    virtual void showImage(bool show = true) = 0;
    
    // Set the image path to be shown on the button.
    virtual void setImagePath(const casacore::String& imgPath) = 0;
    
    // Returns true if the button is "toggleable", false otherwise.  A button
    // that is toggleable sticks down when pushed, then comes back up when
    // pushed again.  The state can be determined with isToggled().
    virtual bool isToggleable() const = 0;
    
    // Sets whether this button is "toggleable" or not.
    virtual void setToggleable(bool toggleable = true) = 0;
    
    // Returns whether this button is in a toggled state or not.
    virtual bool isToggled() const = 0;
    
    // Sets whether this button is in a toggled state or not.  (Does not
    // affect buttons that are not toggleable.)
    virtual void setToggled(bool toggled = true) = 0;
    
    // Register the given event handler for this button.
    virtual void registerHandler(PlotButtonEventHandlerPtr handler) = 0;
    
    // Returns all event handlers currently registered on this button.
    virtual std::vector<PlotButtonEventHandlerPtr> allHandlers() const = 0;
    
    // Unregisters the given event handler.
    virtual void unregisterHandler(PlotButtonEventHandlerPtr handler) = 0;
};


// Generic class for a checkbox that goes on a PlotPanel.  A checkbox has
// properties that can be set, as well as registration for event
// handlers.  A checkbox can only be seen/interacted with on a PlotPanel.
class PlotCheckbox : public virtual PlotWidget {
public:
    // Constructor.
    PlotCheckbox() { }
    
    // Destructor.
    virtual ~PlotCheckbox() { }
    
    // Returns the text for this checkbox.
    virtual casacore::String text() const = 0;
    
    // Sets the text for this checkbox.
    virtual void setText(const casacore::String& text) = 0;
    
    // Returns true if the checkbox is currently checked, false otherwise.
    virtual bool isChecked() const = 0;
    
    // Sets whether the checkbox is checked or not.
    virtual void setChecked(bool checked = true) = 0;
    
    // Register the given event handler for this checkbox.
    virtual void registerHandler(PlotCheckboxEventHandlerPtr handler) = 0;
    
    // Returns all event handlers currently registered on this button.
    virtual std::vector<PlotCheckboxEventHandlerPtr> allHandlers() const = 0;
    
    // Unregisters the given event handler.
    virtual void unregisterHandler(PlotCheckboxEventHandlerPtr handler) = 0;
};


// A PlotPanel is a panel that goes on the bottom of the plot window.  A
// single panel can contain multiple widgets.
class PlotPanel {
public:
    PlotPanel() { }
    
    virtual ~PlotPanel() { }
    
    // Returns all PlotWidgets currently on the panel.
    virtual std::vector<PlotWidgetPtr> widgets() const = 0;
    
    // Adds the given widget to this panel.
    virtual int addWidget(PlotWidgetPtr widget) = 0;
    
    // Clears all widgets on the panel.
    virtual void clearWidgets() = 0;
    
    // Removes the given widget from this panel.
    virtual void removeWidget(PlotWidgetPtr widget) = 0;
    
    // Removes the widget at the given index from this panel.
    virtual void removeWidget(int index) = 0;
};


///////////////////////////////
// SMART POINTER DEFINITIONS //
///////////////////////////////

INHERITANCE_POINTER2(PlotButton, PlotButtonPtr, PlotWidget, PlotWidgetPtr)
INHERITANCE_POINTER2(PlotCheckbox, PlotCheckboxPtr, PlotWidget, PlotWidgetPtr)
typedef casacore::CountedPtr<PlotPanel> PlotPanelPtr;

}

#endif /*PLOTPANEL_H_*/
