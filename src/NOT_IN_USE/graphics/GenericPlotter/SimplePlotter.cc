//# SimplePlotter.cc: Concrete plotting class for common or simple use cases.
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
#include <graphics/GenericPlotter/SimplePlotter.h>
#include <casacore/casa/BasicMath/Math.h>
#include <vector>

using namespace casacore;
namespace casa {

///////////////////////////////
// SIMPLEPLOTTER DEFINITIONS //
///////////////////////////////

// Constructors/Destructors //

SimplePlotter::SimplePlotter(PlotFactoryPtr factory): m_factory(factory) {
    if(!m_factory.null()) {
        m_plotter = m_factory->plotter();
        m_canvas = m_plotter->canvas();
        m_mouseTools = m_canvas->standardMouseTools();
        
        // Set up defaults
        m_line = factory->line("black", PlotLine::SOLID, 1.0);
        m_symbol = factory->symbol(PlotSymbol::DIAMOND);
        m_symbol->setLine(m_line);
        m_symbol->setAreaFill("blue");
        m_areaFill = factory->areaFill("black", PlotAreaFill::MESH3);
    } else {
        String error = "SimplePlotter::SimplePlotter(): Invalid plotter "
                       "implementation.  If you're trying to use qwt, make"
                       " sure you have the library installed and the "
                       "AIPS_HAS_QWT compiler flag turned on.";
        throw error;
    }
}

SimplePlotter::~SimplePlotter() { }


// Execution Methods //

int SimplePlotter::execLoop() { return m_factory->execLoop(); }

void SimplePlotter::holdDrawing() { m_canvas->holdDrawing(); }

void SimplePlotter::releaseDrawing() { m_canvas->releaseDrawing(); }


// Plotter Customization Methods //

void SimplePlotter::setWindowTitle(const String& windowTitle) {
    m_plotter->setWindowTitle(windowTitle);
}

void SimplePlotter::setCanvasTitle(const String& canvasTitle) {
    m_canvas->setTitle(canvasTitle);
}

void SimplePlotter::setAxesLabels(const String& xLabel, const String& yLabel) {
    m_canvas->setAxisLabel(X_BOTTOM, xLabel);
    m_canvas->setAxisLabel(Y_LEFT, yLabel);
}

void SimplePlotter::showCartesianAxes(bool show) {
    m_canvas->showCartesianAxes(show, show);
}

void SimplePlotter::setXAxisRange(double from, double to) {
    m_canvas->setAxisRange(X_BOTTOM, from, to);
}

void SimplePlotter::setYAxisRange(double from, double to) {
    m_canvas->setAxisRange(Y_LEFT, from, to);
}

void SimplePlotter::setAxesAutoRescale(bool on) {
    m_canvas->setAxesAutoRescale(on);
}

void SimplePlotter::rescaleAxes() {
    m_canvas->rescaleAxes();
}


// Plot/Shape Customization Methods //

void SimplePlotter::showLines(bool showLines) {
    m_line->setStyle(showLines ? PlotLine::SOLID : PlotLine::NOLINE);
}

void SimplePlotter::showSymbols(bool showSymbols) {
    m_symbol->setSymbol(showSymbols ? PlotSymbol::DIAMOND :
                                      PlotSymbol::NOSYMBOL);
}

void SimplePlotter::setLine(const String& color, PlotLine::Style style,
        double width) {
    m_line->setColor(color);
    m_line->setStyle(style);
    m_line->setWidth(width);
}

void SimplePlotter::setSymbol(PlotSymbol::Symbol symbol, const String& color,
        double size, bool outline) {
    m_symbol->setSymbol(symbol);
    m_symbol->setAreaFill(color, PlotAreaFill::FILL);
    m_symbol->setSize(size, size);
    m_symbol->setLine("black", (outline ? PlotLine::SOLID : PlotLine::NOLINE),
                      1.0);
}

void SimplePlotter::setAreaFill(const String& color,
        PlotAreaFill::Pattern pattern) {
    m_areaFill->setColor(color);
    m_areaFill->setPattern(pattern);
}


// Plot Methods //

ScatterPlotPtr SimplePlotter::plotxy(double*& x, double*& y, unsigned int n,
                                     bool overplot) {
    if(m_factory.null()) return ScatterPlotPtr();
    else return plotxy(m_factory->data(x, y, n), overplot);
}

ScatterPlotPtr SimplePlotter::plotxy(float*& x, float*& y, unsigned int n,
                                     bool overplot) {
    if(m_factory.null()) return ScatterPlotPtr();
    else return plotxy(m_factory->data(x, y, n), overplot);
}

ScatterPlotPtr SimplePlotter::plotxy(int*& x, int*& y, unsigned int n,
                                     bool overplot) {
    if(m_factory.null()) return ScatterPlotPtr();
    else return plotxy(m_factory->data(x, y, n), overplot);
}

ScatterPlotPtr SimplePlotter::plotxy(Vector<double>& x, Vector<double>& y,
                                     bool overplot) {
    if(m_factory.null()) return ScatterPlotPtr();
    else return plotxy(m_factory->data(x, y), overplot);
}

ScatterPlotPtr SimplePlotter::plotxy(Vector<float>& x, Vector<float>& y,
                                     bool overplot) {
    if(m_factory.null()) return ScatterPlotPtr();
    else return plotxy(m_factory->data(x, y), overplot);
}

ScatterPlotPtr SimplePlotter::plotxy(Vector<int>& x, Vector<int>& y,
                                     bool overplot) {
    if(m_factory.null()) return ScatterPlotPtr();
    else return plotxy(m_factory->data(x, y), overplot);
}

ScatterPlotPtr SimplePlotter::plotxy(PlotPointDataPtr data, bool overplot) {
    if(m_factory.null() || data.null() || !data->isValid())
        return ScatterPlotPtr();
    
    ScatterPlotPtr plot = m_factory->scatterPlot(data);
    plot->setLine(m_line);
    plot->setSymbol(m_symbol);    
    m_canvas->plot(plot, overplot);
    
    return plot;
}

ScatterPlotPtr SimplePlotter::ploty(double*& y, unsigned int n,
                                    bool overplot) {
    if(m_factory.null()) return ScatterPlotPtr();
    else return ploty(m_factory->data(y, n), overplot);
}

ScatterPlotPtr SimplePlotter::ploty(float*& y, unsigned int n,
                                    bool overplot) {
    if(m_factory.null()) return ScatterPlotPtr();
    else return ploty(m_factory->data(y, n), overplot);
}

ScatterPlotPtr SimplePlotter::ploty(int*& y, unsigned int n,
                                    bool overplot) {
    if(m_factory.null()) return ScatterPlotPtr();
    else return ploty(m_factory->data(y, n), overplot);
}

ScatterPlotPtr SimplePlotter::ploty(Vector<double>& y, bool overplot) {
    if(m_factory.null()) return ScatterPlotPtr();
    else return ploty(m_factory->data(y), overplot);
}

ScatterPlotPtr SimplePlotter::ploty(Vector<float>& y, bool overplot) {
    if(m_factory.null()) return ScatterPlotPtr();
    else return ploty(m_factory->data(y), overplot);
}

ScatterPlotPtr SimplePlotter::ploty(Vector<int>& y, bool overplot) {
    if(m_factory.null()) return ScatterPlotPtr();
    else return ploty(m_factory->data(y), overplot);
}

ScatterPlotPtr SimplePlotter::ploty(PlotPointDataPtr data, bool overplot) {
    if(m_factory.null() || data.null() || !data->isValid())
        return ScatterPlotPtr();
    
    ScatterPlotPtr plot = m_factory->scatterPlot(data);
    plot->setLine(m_line);
    plot->setSymbol(m_symbol);    
    m_canvas->plot(plot, overplot);
    
    return plot;
}

BarPlotPtr SimplePlotter::barPlot(double*& x, double*& y, unsigned int n,
                                  bool overplot) {
    if(m_factory.null()) return BarPlotPtr();
    else return barPlot(m_factory->data(x, y, n), overplot);
}

BarPlotPtr SimplePlotter::barPlot(float*& x, float*& y, unsigned int n,
                                  bool overplot) {
    if(m_factory.null()) return BarPlotPtr();
    else return barPlot(m_factory->data(x, y, n), overplot);
}

BarPlotPtr SimplePlotter::barPlot(int*& x, int*& y, unsigned int n,
                                  bool overplot) {
    if(m_factory.null()) return BarPlotPtr();
    else return barPlot(m_factory->data(x, y, n), overplot);
}

BarPlotPtr SimplePlotter::barPlot(Vector<double>& x, Vector<double>& y,
                                  bool overplot) {
    if(m_factory.null()) return BarPlotPtr();
    else return barPlot(m_factory->data(x, y), overplot);
}

BarPlotPtr SimplePlotter::barPlot(Vector<float>& x, Vector<float>& y,
                                  bool overplot) {
    if(m_factory.null()) return BarPlotPtr();
    else return barPlot(m_factory->data(x, y), overplot);
}

BarPlotPtr SimplePlotter::barPlot(Vector<int>& x, Vector<int>& y,
                                  bool overplot) {
    if(m_factory.null()) return BarPlotPtr();
    else return barPlot(m_factory->data(x, y), overplot);
}

BarPlotPtr SimplePlotter::barPlot(PlotPointDataPtr data, bool overplot) {
    if(data.null()|| !data->isValid() || m_factory.null()) return BarPlotPtr();
    
    BarPlotPtr bar = m_factory->barPlot(data);
    bar->setLine(m_line);
    bar->setAreaFill(m_areaFill);
    if(!overplot) m_canvas->clearPlots();
    m_canvas->plot(bar);
    return bar;
}

BarPlotPtr SimplePlotter::histogramPlot(double*& data, unsigned int n,
        unsigned int bins, bool overplot) {
    if(m_factory.null()) return BarPlotPtr();
    else return barPlot(m_factory->histogramData(data, n, bins), overplot);
}

BarPlotPtr SimplePlotter::histogramPlot(float*& data, unsigned int n,
        unsigned int bins, bool overplot) {
    if(m_factory.null()) return BarPlotPtr();
    else return barPlot(m_factory->histogramData(data, n, bins), overplot);
}

BarPlotPtr SimplePlotter::histogramPlot(int*& data, unsigned int n,
        unsigned int bins, bool overplot) {
    if(m_factory.null()) return BarPlotPtr();
    else return barPlot(m_factory->histogramData(data, n, bins), overplot);
}

BarPlotPtr SimplePlotter::histogramPlot(Vector<double>& data,
        unsigned int bins, bool overplot) {
    if(m_factory.null()) return BarPlotPtr();
    else return barPlot(m_factory->histogramData(data, bins), overplot);
}

BarPlotPtr SimplePlotter::histogramPlot(Vector<float>& data,
        unsigned int bins, bool overplot) {
    if(m_factory.null()) return BarPlotPtr();
    else return barPlot(m_factory->histogramData(data, bins), overplot);
}

BarPlotPtr SimplePlotter::histogramPlot(Vector<int>& data,
        unsigned int bins, bool overplot) {
    if(m_factory.null()) return BarPlotPtr();
    else return barPlot(m_factory->histogramData(data, bins), overplot);
}

BarPlotPtr SimplePlotter::histogramPlot(PlotSingleDataPtr data,
        unsigned int bins, bool overplot) {
    if(m_factory.null()) return BarPlotPtr();
    else return barPlot(m_factory->histogramData(data, bins), overplot);
}

RasterPlotPtr SimplePlotter::rasterPlot(Matrix<double>& d, bool overplot) {
    if(m_factory.null()) return RasterPlotPtr();
    else return rasterPlot(PlotRasterDataPtr(new PlotRasterMatrixData<double>(
            d)), overplot);
}

RasterPlotPtr SimplePlotter::rasterPlot(Matrix<float>& d, bool overplot) {
    if(m_factory.null()) return RasterPlotPtr();
    else return rasterPlot(PlotRasterDataPtr(new PlotRasterMatrixData<float>(
            d)), overplot);
}

RasterPlotPtr SimplePlotter::rasterPlot(Matrix<int>& data, bool overplot) {
    if(m_factory.null()) return RasterPlotPtr();
    else return rasterPlot(PlotRasterDataPtr(new PlotRasterMatrixData<int>(
            data)), overplot);
}

RasterPlotPtr SimplePlotter::rasterPlot(Matrix<uInt>& data, bool overplot) {
    if(m_factory.null()) return RasterPlotPtr();
    else return rasterPlot(PlotRasterDataPtr(new PlotRasterMatrixData<uInt>(
            data)), overplot);
}

RasterPlotPtr SimplePlotter::rasterPlot(Matrix<double>& data, double fromX,
        double toX, double fromY, double toY, bool overplot) {
    if(m_factory.null()) return RasterPlotPtr();
    
    PlotRasterDataPtr d(new PlotRasterMatrixData<double>(data));
    d->setXRange(fromX, toX);
    d->setYRange(fromY, toY);
    return rasterPlot(d, overplot);
}

RasterPlotPtr SimplePlotter::rasterPlot(Matrix<float>& data, double fromX,
        double toX, double fromY, double toY, bool overplot) {
    if(m_factory.null()) return RasterPlotPtr();
    
    PlotRasterDataPtr d(new PlotRasterMatrixData<float>(data));
    d->setXRange(fromX, toX);
    d->setYRange(fromY, toY);
    return rasterPlot(d, overplot);
}

RasterPlotPtr SimplePlotter::rasterPlot(Matrix<int>& data, double fromX,
        double toX, double fromY, double toY, bool overplot) {
    if(m_factory.null()) return RasterPlotPtr();
    
    PlotRasterDataPtr d(new PlotRasterMatrixData<int>(data));
    d->setXRange(fromX, toX);
    d->setYRange(fromY, toY);
    return rasterPlot(d, overplot);
}

RasterPlotPtr SimplePlotter::rasterPlot(Matrix<uInt>& data, double fromX,
        double toX, double fromY, double toY, bool overplot) {
    if(m_factory.null()) return RasterPlotPtr();
    
    PlotRasterDataPtr d(new PlotRasterMatrixData<uInt>(data));
    d->setXRange(fromX, toX);
    d->setYRange(fromY, toY);
    return rasterPlot(d, overplot);
}

RasterPlotPtr SimplePlotter::rasterPlot(PlotRasterDataPtr data, bool overplot){
    if(data.null() || !data->isValid() || m_factory.null())
        return RasterPlotPtr();
    
    RasterPlotPtr raster = m_factory->rasterPlot(data);
    raster->setLine(m_line);
    m_canvas->plot(raster, overplot);
    return raster;
}

RasterPlotPtr SimplePlotter::contourPlot(Matrix<double>& d,
        Vector<double>& contours, bool overplot) {
    if(m_factory.null()) return RasterPlotPtr();
    
    std::vector<double> v(contours.size());
    for(unsigned int i = 0; i < v.size(); i++)
        v[i] = static_cast<double>(contours[i]);
    return contourPlot(PlotRasterDataPtr(new PlotRasterMatrixData<double>(d)),
                       v, overplot);
}

RasterPlotPtr SimplePlotter::contourPlot(Matrix<float>& d,
        Vector<float>& contours, bool overplot) {
    if(m_factory.null()) return RasterPlotPtr();
    
    std::vector<double> v(contours.size());
    for(unsigned int i = 0; i < v.size(); i++)
        v[i] = static_cast<double>(contours[i]);
    return contourPlot(PlotRasterDataPtr(new PlotRasterMatrixData<float>(d)),
                       v, overplot);
}

RasterPlotPtr SimplePlotter::contourPlot(Matrix<int>& data,
        Vector<int>& contours, bool overplot) {
    if(m_factory.null()) return RasterPlotPtr();
    
    std::vector<double> v(contours.size());
    for(unsigned int i = 0; i < v.size(); i++)
        v[i] = static_cast<double>(contours[i]);
    return contourPlot(PlotRasterDataPtr(new PlotRasterMatrixData<int>(data)),
                       v, overplot);
}

RasterPlotPtr SimplePlotter::contourPlot(Matrix<uInt>& data,
        Vector<uInt>& contours, bool overplot) {
    if(m_factory.null()) return RasterPlotPtr();
    
    std::vector<double> v(contours.size());
    for(unsigned int i = 0; i < v.size(); i++)
        v[i] = static_cast<double>(contours[i]);
    return contourPlot(PlotRasterDataPtr(new PlotRasterMatrixData<uInt>(data)),
                       v, overplot);
}

RasterPlotPtr SimplePlotter::contourPlot(Matrix<double>& data, double fromX,
        double toX, double fromY, double toY, Vector<double>& contours,
        bool overplot) {
    if(m_factory.null()) return RasterPlotPtr();
    
    std::vector<double> v(contours.size());
    for(unsigned int i = 0; i < v.size(); i++)
        v[i] = static_cast<double>(contours[i]);
    PlotRasterDataPtr d(new PlotRasterMatrixData<double>(data));
    d->setXRange(fromX, toX);
    d->setYRange(fromY, toY);
    return contourPlot(d, v, overplot);
}

RasterPlotPtr SimplePlotter::contourPlot(Matrix<float>& data, double fromX,
        double toX, double fromY,double toY, Vector<float>& contours,
        bool overplot) {
    if(m_factory.null()) return RasterPlotPtr();
    
    std::vector<double> v(contours.size());
    for(unsigned int i = 0; i < v.size(); i++)
        v[i] = static_cast<double>(contours[i]);
    PlotRasterDataPtr d(new PlotRasterMatrixData<float>(data));
    d->setXRange(fromX, toX);
    d->setYRange(fromY, toY);
    return contourPlot(d, v, overplot);
}

RasterPlotPtr SimplePlotter::contourPlot(Matrix<int>& data, double fromX,
        double toX, double fromY, double toY, Vector<int>& contours,
        bool overplot) {
    if(m_factory.null()) return RasterPlotPtr();
    
    std::vector<double> v(contours.size());
    for(unsigned int i = 0; i < v.size(); i++)
        v[i] = static_cast<double>(contours[i]);
    PlotRasterDataPtr d(new PlotRasterMatrixData<int>(data));
    d->setXRange(fromX, toX);
    d->setYRange(fromY, toY);
    return contourPlot(d, v, overplot);
}

RasterPlotPtr SimplePlotter::contourPlot(Matrix<uInt>& data, double fromX,
        double toX, double fromY, double toY, Vector<uInt>& contours,
        bool overplot) {
    if(m_factory.null()) return RasterPlotPtr();
    
    std::vector<double> v(contours.size());
    for(unsigned int i = 0; i < v.size(); i++)
        v[i] = static_cast<double>(contours[i]);
    PlotRasterDataPtr d(new PlotRasterMatrixData<uInt>(data));
    d->setXRange(fromX, toX);
    d->setYRange(fromY, toY);
    return contourPlot(d, v, overplot);
}

RasterPlotPtr SimplePlotter::contourPlot(PlotRasterDataPtr data,
        std::vector<double>& contours, bool overplot) {
    if(contours.size() == 0) return rasterPlot(data, overplot);
    
    if(data.null() || !data->isValid() || m_factory.null())
        return RasterPlotPtr();
    
    RasterPlotPtr raster = m_factory->rasterPlot(data);
    raster->setLine(m_line);
    raster->setContourLines(contours);
    m_canvas->plot(raster, overplot);
    return raster;
}

RasterPlotPtr SimplePlotter::spectrogram(Matrix<double>& d,bool overplot){
    if(m_factory.null()) return RasterPlotPtr();
    else return spectrogram(PlotRasterDataPtr(new PlotRasterMatrixData<double>(
            d)), overplot);
}

RasterPlotPtr SimplePlotter::spectrogram(Matrix<float>& d, bool overplot){
    if(m_factory.null()) return RasterPlotPtr();
    else return spectrogram(PlotRasterDataPtr(new PlotRasterMatrixData<float>(
            d)), overplot);
}

RasterPlotPtr SimplePlotter::spectrogram(Matrix<int>& data,bool overplot){
    if(m_factory.null()) return RasterPlotPtr();
    else return spectrogram(PlotRasterDataPtr(new PlotRasterMatrixData<int>(
            data)), overplot);
}

RasterPlotPtr SimplePlotter::spectrogram(Matrix<uInt>& data,
        bool overplot) {
    if(m_factory.null()) return RasterPlotPtr();
    else return spectrogram(PlotRasterDataPtr(new PlotRasterMatrixData<uInt>(
            data)), overplot);
}

RasterPlotPtr SimplePlotter::spectrogram(Matrix<double>& data,
        double fromX, double toX, double fromY, double toY, bool overplot) {
    if(m_factory.null()) return RasterPlotPtr();
    
    PlotRasterDataPtr d(new PlotRasterMatrixData<double>(data));
    d->setXRange(fromX, toX);
    d->setYRange(fromY, toY);
    return spectrogram(d, overplot);
}

RasterPlotPtr SimplePlotter::spectrogram(Matrix<float>& data,double fromX,
        double toX, double fromY, double toY, bool overplot) {
    if(m_factory.null()) return RasterPlotPtr();
    
    PlotRasterDataPtr d(new PlotRasterMatrixData<float>(data));
    d->setXRange(fromX, toX);
    d->setYRange(fromY, toY);
    return spectrogram(d, overplot);
}

RasterPlotPtr SimplePlotter::spectrogram(Matrix<int>& data, double fromX,
        double toX, double fromY, double toY, bool overplot) {
    if(m_factory.null()) return RasterPlotPtr();
    
    PlotRasterDataPtr d(new PlotRasterMatrixData<int>(data));
    d->setXRange(fromX, toX);
    d->setYRange(fromY, toY);
    return spectrogram(d, overplot);
}

RasterPlotPtr SimplePlotter::spectrogram(Matrix<uInt>& data, double fromX,
        double toX, double fromY, double toY, bool overplot) {
    if(m_factory.null()) return RasterPlotPtr();
    
    PlotRasterDataPtr d(new PlotRasterMatrixData<uInt>(data));
    d->setXRange(fromX, toX);
    d->setYRange(fromY, toY);
    return spectrogram(d, overplot);
}

RasterPlotPtr SimplePlotter::spectrogram(PlotRasterDataPtr data,
        bool overplot) {
    if(data.null() || !data->isValid() || m_factory.null())
        return RasterPlotPtr();
    
    RasterPlotPtr spect = m_factory->spectrogramPlot(data);
    spect->setLine(m_line);
    m_canvas->plot(spect, overplot);
    return spect;
}

RasterPlotPtr SimplePlotter::spectrogram(Matrix<double>& d,
        Vector<double>& contours, bool overplot) {
    if(m_factory.null()) return RasterPlotPtr();
    
    std::vector<double> v(contours.size());
    for(unsigned int i = 0; i < v.size(); i++)
        v[i] = static_cast<double>(contours[i]);
    return spectrogram(PlotRasterDataPtr(new PlotRasterMatrixData<double>(d)),
                       v, overplot);
}

RasterPlotPtr SimplePlotter::spectrogram(Matrix<float>& d,
        Vector<float>& contours, bool overplot) {
    if(m_factory.null()) return RasterPlotPtr();
    
    std::vector<double> v(contours.size());
    for(unsigned int i = 0; i < v.size(); i++)
        v[i] = static_cast<double>(contours[i]);
    return spectrogram(PlotRasterDataPtr(new PlotRasterMatrixData<float>(d)),
                       v, overplot);
}

RasterPlotPtr SimplePlotter::spectrogram(Matrix<int>& data,
        Vector<int>& contours, bool overplot) {
    if(m_factory.null()) return RasterPlotPtr();
    
    std::vector<double> v(contours.size());
    for(unsigned int i = 0; i < v.size(); i++)
        v[i] = static_cast<double>(contours[i]);
    return spectrogram(PlotRasterDataPtr(new PlotRasterMatrixData<int>(data)),
                       v, overplot);
}

RasterPlotPtr SimplePlotter::spectrogram(Matrix<uInt>& data,
        Vector<uInt>& contours, bool overplot) {
    if(m_factory.null()) return RasterPlotPtr();
    
    std::vector<double> v(contours.size());
    for(unsigned int i = 0; i < v.size(); i++)
        v[i] = static_cast<double>(contours[i]);
    return spectrogram(PlotRasterDataPtr(new PlotRasterMatrixData<uInt>(data)),
                       v, overplot);
}

RasterPlotPtr SimplePlotter::spectrogram(Matrix<double>& da, double fromX,
        double toX, double fromY, double toY, Vector<double>& contours,
        bool overplot) {
    if(m_factory.null()) return RasterPlotPtr();
    
    std::vector<double> v(contours.size());
    for(unsigned int i = 0; i < v.size(); i++)
        v[i] = static_cast<double>(contours[i]);
    PlotRasterDataPtr d(new PlotRasterMatrixData<double>(da));
    d->setXRange(fromX, toX);
    d->setYRange(fromY, toY);
    return spectrogram(d, v, overplot);
}

RasterPlotPtr SimplePlotter::spectrogram(Matrix<float>& data,double fromX,
        double toX, double fromY,double toY, Vector<float>& contours,
        bool overplot) {
    if(m_factory.null()) return RasterPlotPtr();
    
    std::vector<double> v(contours.size());
    for(unsigned int i = 0; i < v.size(); i++)
        v[i] = static_cast<double>(contours[i]);
    PlotRasterDataPtr d(new PlotRasterMatrixData<float>(data));
    d->setXRange(fromX, toX);
    d->setYRange(fromY, toY);
    return spectrogram(d, v, overplot);
}

RasterPlotPtr SimplePlotter::spectrogram(Matrix<int>& data, double fromX,
        double toX, double fromY, double toY, Vector<int>& contours,
        bool overplot) {
    if(m_factory.null()) return RasterPlotPtr();
    
    std::vector<double> v(contours.size());
    for(unsigned int i = 0; i < v.size(); i++)
        v[i] = static_cast<double>(contours[i]);
    PlotRasterDataPtr d(new PlotRasterMatrixData<int>(data));
    d->setXRange(fromX, toX);
    d->setYRange(fromY, toY);
    return spectrogram(d, v, overplot);
}

RasterPlotPtr SimplePlotter::spectrogram(Matrix<uInt>& data, double fromX,
        double toX, double fromY, double toY, Vector<uInt>& contours,
        bool overplot) {
    if(m_factory.null()) return RasterPlotPtr();
    
    std::vector<double> v(contours.size());
    for(unsigned int i = 0; i < v.size(); i++)
        v[i] = static_cast<double>(contours[i]);
    PlotRasterDataPtr d(new PlotRasterMatrixData<uInt>(data));
    d->setXRange(fromX, toX);
    d->setYRange(fromY, toY);
    return spectrogram(d, v, overplot);
}

RasterPlotPtr SimplePlotter::spectrogram(PlotRasterDataPtr data,
        std::vector<double>& contours, bool overplot) {
    if(contours.size() == 0) return spectrogram(data, overplot);
    
    if(data.null() || !data->isValid() || m_factory.null())
        return RasterPlotPtr();
    
    RasterPlotPtr spect = m_factory->spectrogramPlot(data);
    spect->setLine(m_line);
    spect->setContourLines(contours);
    m_canvas->plot(spect, overplot);
    return spect;
}

PlotPointPtr SimplePlotter::plotPoint(double x, double y) {
    if(m_factory.null()) return PlotPointPtr();
    
    PlotPointPtr point = m_factory->point(x, y);
    point->setSymbol(m_symbol);
    m_canvas->plotPoint(point);
    m_accumulatedPoints.push_back(point);
    
    return point;
}


// Shapes, Annotations, etc. //

PlotAnnotationPtr SimplePlotter::annotation(double x, double y,
        const String& text) {
    PlotAnnotationPtr annotation =  m_factory->annotation(text, x, y);
    m_canvas->drawAnnotation(annotation);
    return annotation;
}

PlotShapeRectanglePtr SimplePlotter::rectangle(double left, double top,
        double right, double bottom) {
    PlotShapeRectanglePtr rect = m_factory->shapeRectangle(left, top,
                                                           right, bottom);
    rect->setLine(m_line);
    rect->setAreaFill(m_areaFill);
    m_canvas->drawShape(rect);
    return rect;
}

PlotShapeEllipsePtr SimplePlotter::ellipse(double centerX, double centerY,
        double xRadius, double yRadius) {
    PlotShapeEllipsePtr ellipse = m_factory->shapeEllipse(centerX, centerY,
                                                          xRadius, yRadius);
    ellipse->setLine(m_line);
    ellipse->setAreaFill(m_areaFill);
    m_canvas->drawShape(ellipse);
    return ellipse;
}

PlotShapeEllipsePtr SimplePlotter::ellipse(double x, double y, double radius) {
    return ellipse(x, y, radius, radius);
}

PlotShapeLinePtr SimplePlotter::xLine(double value) {
    PlotShapeLinePtr line = m_factory->shapeLine(value, X_BOTTOM);
    line->setLine(m_line);
    m_canvas->drawShape(line);
    return line;
}

PlotShapeLinePtr SimplePlotter::yLine(double value) {
    PlotShapeLinePtr line = m_factory->shapeLine(value, Y_LEFT);
    line->setLine(m_line);
    m_canvas->drawShape(line);
    return line;
}

PlotShapeArrowPtr SimplePlotter::arrow(double xFrom, double yFrom,
                                       double xTo, double yTo) {
    PlotShapeArrowPtr arrow = m_factory->shapeArrow(xFrom, yFrom, xTo, yTo);
    arrow->setLine(m_line);
    arrow->setAreaFill(m_areaFill);
    m_canvas->drawShape(arrow);
    return arrow;
}

PlotShapeArrowPtr SimplePlotter::lineSegment(double xFrom, double yFrom,
                                             double xTo, double yTo) {
    PlotShapeArrowPtr line = m_factory->shapeLineSegment(xFrom, yFrom,
                                                         xTo, yTo);
    line->setLine(m_line);
    m_canvas->drawShape(line);
    return line;
}


// Clearing Methods //

void SimplePlotter::clear() {
    m_canvas->clearItems();
    m_accumulatedPoints.clear();
}

void SimplePlotter::clearPoints() {
    for(unsigned int i = 0; i < m_accumulatedPoints.size(); i++)
        m_canvas->removePoint(m_accumulatedPoints[i]);
    m_accumulatedPoints.clear();
}


// Interaction Methods //

void SimplePlotter::showDefaultHandTools(bool show) {
    m_plotter->showDefaultPanel(Plotter::HAND_TOOLS, show);
}

void SimplePlotter::showDefaultExportTools(bool show) {
    m_plotter->showDefaultPanel(Plotter::EXPORT_TOOLS, show);
}

std::vector<PlotRegion> SimplePlotter::allSelectedRegions() {
    return m_mouseTools->selectTool()->getSelectedRects();
}

void SimplePlotter::clearSelectedRegions() {
    m_mouseTools->selectTool()->clearSelectedRects();
}


// Export Methods //

String SimplePlotter::fileChooserDialog(const String& title,
        const String& directory) {
    return m_plotter->fileChooserDialog(title, directory);
}

bool SimplePlotter::exportPDF(const String& location, bool highQuality,
        int dpi) {
    PlotExportFormat format(PlotExportFormat::PDF, location);
    if(highQuality) format.resolution = PlotExportFormat::HIGH;
    else format.resolution = PlotExportFormat::SCREEN;
    format.dpi = dpi;
    return exportToFile(format);
}

bool SimplePlotter::exportPS(const String& location, bool highQuality,
        int dpi) {
    PlotExportFormat format(PlotExportFormat::PS, location);
    if(highQuality) format.resolution = PlotExportFormat::HIGH;
    else format.resolution = PlotExportFormat::SCREEN;
    format.dpi = dpi;
    return exportToFile(format);
}

bool SimplePlotter::exportJPG(const String& location, bool highQuality,
        int width, int height) {
    PlotExportFormat format(PlotExportFormat::JPG, location);
    if(highQuality) format.resolution = PlotExportFormat::HIGH;
    else format.resolution = PlotExportFormat::SCREEN;
    format.width = width;
    format.height = height;
    return exportToFile(format);
}

bool SimplePlotter::exportPNG(const String& location, bool highQuality,
        int width, int height) {
    PlotExportFormat format(PlotExportFormat::PNG, location);
    if(highQuality) format.resolution = PlotExportFormat::HIGH;
    else format.resolution = PlotExportFormat::SCREEN;
    format.width = width;
    format.height = height;
    return exportToFile(format);
}

bool SimplePlotter::exportToFile(const PlotExportFormat& format) {
    return m_canvas->exportToFile(format);
}

}
