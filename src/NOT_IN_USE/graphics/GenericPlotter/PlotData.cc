//# PlotData.cc: Classes to represent data for plots.
//# Copyright (C) 2009
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
#include <graphics/GenericPlotter/PlotData.h>

using namespace casacore;
namespace casa {

///////////////////////////////
// PLOTPOINTDATA DEFINITIONS //
///////////////////////////////

void PlotPointData::xAndYAt(unsigned int index, double& x, double& y) const {
    x = xAt(index);
    y = yAt(index);
}


////////////////////////////////////
// PLOTMASKEDPOINTDATADEFINITIONS //
////////////////////////////////////

void PlotMaskedPointData::xyAndMaskAt(unsigned int index, double& x, double& y,
        bool& mask) const {
    xAndYAt(index, x, y);
    mask = maskedAt(index);
}


///////////////////////////////
// PLOTERRORDATA DEFINITIONS //
///////////////////////////////

void PlotErrorData::xyAndErrorsAt(unsigned int index, double& x, double& y,
        double& xLeftError, double& xRightError, double& yBottomError,
        double& yTopError) const {
    xAndYAt(index, x, y);
    xLeftError = xLeftErrorAt(index);
    xRightError = xRightErrorAt(index);
    yBottomError = yBottomErrorAt(index);
    yTopError = yTopErrorAt(index);
}


///////////////////////////////////////
// PLOTHISTOGRAMDATAIMPL DEFINITIONS //
///////////////////////////////////////

PlotHistogramData::PlotHistogramData(PlotSingleDataPtr data,
        unsigned int numBins) : m_data(data) {
    recalculateBins(numBins);
}

PlotHistogramData::~PlotHistogramData() { }


bool PlotHistogramData::isValid() const {
    return !m_data.null() && m_data->isValid() && m_bins.size() > 0; }

bool PlotHistogramData::willDeleteData() const {
    return !m_data.null() && m_data->willDeleteData(); }

void PlotHistogramData::setDeleteData(bool del) {
    if(!m_data.null()) m_data->setDeleteData(del); }

double PlotHistogramData::xAt(unsigned int i) const {
    return (m_ranges[i].second + m_ranges[i].first) / 2;
}

double PlotHistogramData::yAt(unsigned int i) const { return m_bins[i]; }

bool PlotHistogramData::minsMaxes(double& xMin, double& xMax, double& yMin,
        double& yMax) {
    if(m_ranges.size() == 0) return false;
    xMin = m_ranges[0].first; xMax = m_ranges[m_ranges.size() - 1].second;
    yMin = 0;                 yMax = m_max;
    return true;
}

void PlotHistogramData::recalculateBins(unsigned int numBins) {
    if(numBins == m_bins.size()) return;
    
    m_bins.resize(0);
    m_ranges.resize(0);
    m_max = 0;
    if(m_data.null() || !m_data->isValid() || m_data->size() == 0 ||
       numBins == 0) return;
    
    m_bins.resize(numBins, 0);
    m_ranges.resize(numBins);
    
    // Find min/max.
    double temp = m_data->at(0);
    double min = temp, max = temp;
    unsigned int size = m_data->size();
    for(unsigned int i = 1; i < size; i++) {
        temp = m_data->at(i);
        if(temp < min) min = temp;
        if(temp > max) max = temp;
    }
    
    // Set ranges.
    m_ranges[0].first = min;
    temp = (max - min) / numBins;
    for(unsigned int i = 0; i < numBins; i++) {
        m_ranges[i].second = min + (temp * (i + 1));
        if(i < numBins - 1) m_ranges[i + 1].first = m_ranges[i].second;
    }
    
    // Find bin counts.
    for(unsigned int i = 0; i < size; i++) {
        temp = m_data->at(i);
        
        for(unsigned int j = 0; j < numBins; j++) {
            if(temp <= m_ranges[j].second || j == numBins - 1) {
                m_bins[j]++;
                break;
            }
        }
    }
    
    // Find max bin count.
    m_max = m_bins[0];
    for(unsigned int i = 1; i < numBins; i++)
        if(m_bins[i] > m_max) m_max = m_bins[i];
}

unsigned int PlotHistogramData::numBins() const { return m_bins.size(); }

prange_t PlotHistogramData::rangeAt(unsigned int i) const {
    return m_ranges[i]; }

}
