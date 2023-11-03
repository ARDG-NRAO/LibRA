//# PlotEvent.cc: Classes for interaction events.
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
#include <graphics/GenericPlotter/PlotEvent.h>

using namespace std;

using namespace casacore;
namespace casa {

/////////////////////////////////
// PLOTSELECTEVENT DEFINITIONS //
/////////////////////////////////

PlotSelectEvent::PlotSelectEvent(PlotCanvas* canvas, const PlotRegion& region):
        m_canvas(canvas), m_region(region) { }

PlotSelectEvent::~PlotSelectEvent() { }

PlotCanvas* PlotSelectEvent::canvas() const { return m_canvas; }

PlotRegion PlotSelectEvent::region() const { return m_region; }


////////////////////////////////
// PLOTMOUSEEVENT DEFINITIONS //
////////////////////////////////

PlotMouseEvent::PlotMouseEvent(PlotCanvas* canvas, Type t, Button b,
        const PlotCoordinate& c) : m_canvas(canvas), m_type(t), m_button(b),
        m_coord(c) { }

PlotMouseEvent::~PlotMouseEvent() { }

PlotCanvas* PlotMouseEvent::canvas() const { return m_canvas; }

PlotMouseEvent::Type PlotMouseEvent::type() const { return m_type; }

PlotMouseEvent::Button PlotMouseEvent::button() const { return m_button; }

PlotCoordinate PlotMouseEvent::where() const { return m_coord; }


////////////////////////////////
// PLOTWHEELEVENT DEFINITIONS //
////////////////////////////////

PlotWheelEvent::PlotWheelEvent(PlotCanvas* canvas, int d,
    const PlotCoordinate& c) : m_canvas(canvas), m_delta(d), m_coord(c) { }

PlotWheelEvent::~PlotWheelEvent() { }

PlotCanvas* PlotWheelEvent::canvas() const { return m_canvas; }

int PlotWheelEvent::delta() const { return m_delta; }

PlotCoordinate PlotWheelEvent::where() const { return m_coord; }


//////////////////////////////
// PLOTKEYEVENT DEFINITIONS //
//////////////////////////////

PlotKeyEvent::PlotKeyEvent(PlotCanvas* canvas, char key,
        const vector<Modifier>& m): m_canvas(canvas), m_key(key), m_mods(m) { }

PlotKeyEvent::~PlotKeyEvent() { }

PlotCanvas* PlotKeyEvent::canvas() const { return m_canvas; }

char PlotKeyEvent::key() const { return m_key; }

vector<PlotKeyEvent::Modifier> PlotKeyEvent::modifiers() const {
    return m_mods;
}

String PlotKeyEvent::toString() const {
    stringstream ss;
    
    for(unsigned int i = 0; i < m_mods.size(); i++) {
        ss << modifier(m_mods[i]);
        if(i < m_mods.size() - 1) ss << '+';
    }
    ss << m_key;
    
    return ss.str();
}


/////////////////////////////////
// PLOTRESIZEEVENT DEFINITIONS //
/////////////////////////////////

PlotResizeEvent::PlotResizeEvent(PlotCanvas* canvas, int oldW, int oldH,
        int newW, int newH) : m_plotter(NULL), m_canvas(canvas),
        m_old(oldW, oldH), m_new(newW, newH) { }

PlotResizeEvent::PlotResizeEvent(Plotter* plotter, int oldW, int oldH,
        int newW, int newH) : m_plotter(plotter), m_canvas(NULL),
        m_old(oldW, oldH), m_new(newW, newH) { }

PlotResizeEvent::~PlotResizeEvent() { }

PlotCanvas* PlotResizeEvent::canvas() const { return m_canvas; }

Plotter* PlotResizeEvent::plotter() const { return m_plotter; }

pair<int, int> PlotResizeEvent::oldSize() const { return m_old; }

pair<int, int> PlotResizeEvent::newSize() const { return m_new; }


/////////////////////////////////
// PLOTBUTTONEVENT DEFINITIONS //
/////////////////////////////////

PlotButtonEvent::PlotButtonEvent(PlotButton* button) : m_button(button) { }

PlotButtonEvent::~PlotButtonEvent() { }

PlotButton* PlotButtonEvent::button() const { return m_button; }


///////////////////////////////////
// PLOTCHECKBOXEVENT DEFINITIONS //
///////////////////////////////////

PlotCheckboxEvent::PlotCheckboxEvent(PlotCheckbox* cb) : m_checkbox(cb) { }

PlotCheckboxEvent::~PlotCheckboxEvent() { }

PlotCheckbox* PlotCheckboxEvent::checkbox() const { return m_checkbox; }

}
