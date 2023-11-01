//# PlotCanvasLayout.cc: Different layouts for PlotCanvases.
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
#include <graphics/GenericPlotter/PlotCanvasLayout.h>
#include <graphics/GenericPlotter/Plotter.h>

using namespace casacore;
namespace casa {

//////////////////////////////////
// PLOTLAYOUTSINGLE DEFINITIONS //
//////////////////////////////////

PlotLayoutSingle::PlotLayoutSingle(PlotCanvasPtr c) : m_canvas(c) { }

PlotLayoutSingle::~PlotLayoutSingle() { }

void PlotLayoutSingle::setCanvasAt(const PlotLayoutCoordinate& coord,
                                   PlotCanvasPtr c)  
                                   {
    (void)coord;
    m_canvas = c;

    if(m_plotter != NULL) m_plotter->canvasLayoutChanged(*this);
}

void PlotLayoutSingle::setCanvas(PlotCanvasPtr canvas) {
    m_canvas = canvas;
    if(m_plotter != NULL) m_plotter->canvasLayoutChanged(*this);
}



PlotCanvasPtr PlotLayoutSingle::canvas() const { 
    return m_canvas; 
}



PlotCanvasPtr PlotLayoutSingle::canvasAt(const PlotLayoutCoordinate& coord)
                                         const { 
    (void)coord;
    return m_canvas; 
}



std::vector<PlotCanvasPtr> PlotLayoutSingle::allCanvases() const {
    std::vector<PlotCanvasPtr> v(m_canvas.null() ? 0 : 1);
    if(!m_canvas.null()) v[0] = m_canvas;
    return v;
}

bool PlotLayoutSingle::isValid() const { return !m_canvas.null(); }






////////////////////////////////
// PLOTLAYOUTGRID DEFINITIONS //
////////////////////////////////

PlotLayoutGrid::PlotLayoutGrid(unsigned int rows, unsigned int cols):
        m_rows(rows), m_cols(cols), m_panels(m_rows), m_spacing(0) {
    if(rows > 0 && cols > 0) {
        for(unsigned int i = 0; i < m_rows; i++)
            m_panels[i].resize(m_cols);
    }
}


PlotLayoutGrid::~PlotLayoutGrid() { }

unsigned int PlotLayoutGrid::rows() const { return m_rows; }

unsigned int PlotLayoutGrid::cols() const { return m_cols; }


bool PlotLayoutGrid::coordIsValid(const PlotLayoutCoordinate& coord) const {
    const PlotGridCoordinate* c = dynamic_cast<const
                                  PlotGridCoordinate*>(&coord);
    if(c != NULL) return c->row < m_rows && c->col < m_cols;
    else return false;
}

int PlotLayoutGrid::coordToIndex(const PlotLayoutCoordinate& coord) const {    
    const PlotGridCoordinate* c = dynamic_cast<const
                                  PlotGridCoordinate*>(&coord);
    if(c != NULL && coordIsValid(coord)) return (c->row * m_cols) + c->col;
    else return -1;
}

void PlotLayoutGrid::setCanvasAt(const PlotLayoutCoordinate& coord,
                                 PlotCanvasPtr canvas) {
    if(!canvas.null() && coordIsValid(coord)) {
        const PlotGridCoordinate* c = dynamic_cast<const
                                      PlotGridCoordinate*>(&coord);
        m_panels[c->row][c->col] = canvas;

        if(m_plotter != NULL) m_plotter->canvasLayoutChanged(*this);
    }
}

PlotCanvasPtr PlotLayoutGrid::canvasAt(const PlotLayoutCoordinate& co) const {
    if(coordIsValid(co)) {
        const PlotGridCoordinate* c = dynamic_cast<const
                                      PlotGridCoordinate*>(&co);
        if(c != NULL) return m_panels[c->row][c->col];
    }
    return PlotCanvasPtr();
}

PlotCanvasPtr PlotLayoutGrid::canvas() const {
    if(m_panels.size() > 0 && m_panels[0].size() > 0) return m_panels[0][0];
    else return PlotCanvasPtr();
}

std::vector<PlotCanvasPtr> PlotLayoutGrid::allCanvases() const {
    unsigned int count = 0;
    for(unsigned int i = 0; i < m_rows; i++)
        for(unsigned int j = 0; j < m_cols; j++)
            if(!m_panels[i][j].null())
                count++;

    std::vector<PlotCanvasPtr> v(count);

    count = 0;
    for(unsigned int i = 0; i < m_rows; i++)
        for(unsigned int j = 0; j < m_cols; j++)
            if(!m_panels[i][j].null())
                v[count++] = m_panels[i][j];

    return v;
}

bool PlotLayoutGrid::isValid() const {
    if(m_rows <= 0 || m_cols <= 0) return false;
    for(unsigned int i = 0; i < m_panels.size(); i++)
        for(unsigned int j = 0; j < m_panels[i].size(); j++)
            if(m_panels[i][j].null())
                return false;
    return true;
}

unsigned int PlotLayoutGrid::spacing() const { return m_spacing; }

void PlotLayoutGrid::setSpacing(unsigned int spacing) {
    m_spacing = spacing;
    if(m_plotter != NULL) m_plotter->canvasLayoutChanged(*this);
}

}
