//# SimplePlotter.h: Concrete plotting class for common or simple use cases.
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
#ifndef SIMPLEPLOTTER_H_
#define SIMPLEPLOTTER_H_

#include <graphics/GenericPlotter/PlotFactory.h>

#include <casacore/casa/Arrays/Matrix.h>

namespace casa {

// SimplePlotter is a concrete class that uses the abstract plotting classes
// to perform common tasks.  SimplePlotter is meant for users who won't need
// any advanced or complicated customization or specialized data types.
class SimplePlotter {
public:
    // Constructor that takes a factory to build plotting objects.
    SimplePlotter(PlotFactoryPtr factory);
    
    // Destructor
    ~SimplePlotter();
    
    
    // Accessor methods
    
    // Returns the factory.
    PlotFactoryPtr getFactory() { return m_factory; }
    
    // Returns the Plotter.
    PlotterPtr getPlotter() { return m_plotter; }
    
    // Returns the PlotCanvas.
    PlotCanvasPtr getCanvas() { return m_canvas; }
    
    // Returns the current PlotLine used to draw the plots.
    PlotLinePtr getLine() { return m_line; }
    
    // Returns the current PlotSymbol used for the plots and points.
    PlotSymbolPtr getSymbol() { return m_symbol; }
    
    // Returns the current PlotAreaFill used for shapes, histograms, etc.
    PlotAreaFillPtr getAreaFill() { return m_areaFill; }
    
    
    // Execution methods
    
    // Enters the execution loop and returns the result.
    int execLoop();
    
    // Holds/Releases drawing on the canvas.  Is NOT recursive.
    // <group>
    void holdDrawing();
    void releaseDrawing();
    // </group>
    
    
    // Plotter customization
    
    // Sets the title of the plotting window to the given.
    void setWindowTitle(const casacore::String& windowTitle);
    
    // Sets the title of the canvas to the given.
    void setCanvasTitle(const casacore::String& canvasTitle);
    
    // Sets the X_BOTTOM and Y_LEFT axes labels to the given.
    void setAxesLabels(const casacore::String& xLabel, const casacore::String& yLabel);
    
    // Show/hide Cartesian axes.  (See PlotCanvas::showCartesianAxis()).
    void showCartesianAxes(bool show = true);
    
    // Implies setAxesAutoRescale(false)
    void setXAxisRange(double from, double to);
    
    // Implies setAxesAutoRescale(false)
    void setYAxisRange(double from, double to);
    
    // When future items are added to the canvas, automatically rescale
    // the axes to show all items.
    void setAxesAutoRescale(bool on = true);
    
    // Automatically rescale the axes to show all items on the canvas.
    void rescaleAxes();
    
    
    // Plot customization
    
    // Turn lines on or off for future plots.
    void showLines(bool showLines = true);
    
    // Turns symbols on or off for future plots and points.
    void showSymbols(bool showSymbols = true);
    
    // Set the line for future plots to the given characteristics.  Color can
    // be hexadecimal form ("000000") or name form ("black").
    void setLine(const casacore::String& color,
            PlotLine::Style style = PlotLine::SOLID,  double width = 1.0);
    
    // Set the symbol for future plots and points to the given characteristics.
    void setSymbol(PlotSymbol::Symbol symbol, const casacore::String& color = "blue",
                   double size = 5, bool outline = true);
    
    // Set the area fill for future histograms, shapes, etc. to the given
    // characteristics.
    void setAreaFill(const casacore::String& color,
                     PlotAreaFill::Pattern pattern = PlotAreaFill::FILL);
    
    
    // Plotting methods
    
    // IMPORTANT: since the data given to the plotting methods is not copied
    // (but rather a reference is used) it is important that the data not
    // go out of scope while the plots are being used!
    
    // Plot the given points, using the current line and symbol.
    // <group>
    ScatterPlotPtr plotxy(double*& x, double*& y, unsigned int n,
                          bool overplot = true);
    ScatterPlotPtr plotxy(float*& x, float*& y, unsigned int n,
                          bool overplot = true);
    ScatterPlotPtr plotxy(int*& x, int*& y, unsigned int n,
                          bool overplot = true);
    
    ScatterPlotPtr plotxy(casacore::Vector<double>& x, casacore::Vector<double>& y,
                          bool overplot = true);
    ScatterPlotPtr plotxy(casacore::Vector<float>& x, casacore::Vector<float>& y,
                          bool overplot = true);
    ScatterPlotPtr plotxy(casacore::Vector<int>& x, casacore::Vector<int>& y,
                          bool overplot = true);
    
    ScatterPlotPtr plotxy(PlotPointDataPtr data, bool overplot = true);
    
    ScatterPlotPtr ploty(double*& y, unsigned int n, bool overplot = true);
    ScatterPlotPtr ploty(float*& y, unsigned int n, bool overplot = true);
    ScatterPlotPtr ploty(int*& y, unsigned int n, bool overplot = true);
    
    ScatterPlotPtr ploty(casacore::Vector<double>& y, bool overplot = true);
    ScatterPlotPtr ploty(casacore::Vector<float>& y, bool overplot = true);
    ScatterPlotPtr ploty(casacore::Vector<int>& y, bool overplot = true);
    
    ScatterPlotPtr ploty(PlotPointDataPtr data, bool overplot = true);
    // </group>
    
    // Display a bar plot for the given data, using the current line and area
    // fill.
    // <group>
    BarPlotPtr barPlot(double*& x, double*& y, unsigned int n,
                       bool overplot = false);
    BarPlotPtr barPlot(float*& x, float*& y, unsigned int n,
                       bool overplot = false);
    BarPlotPtr barPlot(int*& x, int*& y, unsigned int n,
                       bool overplot = false);
    
    BarPlotPtr barPlot(casacore::Vector<double>& x, casacore::Vector<double>& y,
                       bool overplot = false);
    BarPlotPtr barPlot(casacore::Vector<float>& x, casacore::Vector<float>& y,
                       bool overplot = false);
    BarPlotPtr barPlot(casacore::Vector<int>& x, casacore::Vector<int>& y,
                       bool overplot = false);
    
    BarPlotPtr barPlot(PlotPointDataPtr data, bool overplot = false);
    // </group>
    
    // Display a histogram for the given data in the given number of bins,
    // using the current line and area fill.
    // <group>
    BarPlotPtr histogramPlot(double*& data, unsigned int n,
                             unsigned int numBins, bool overplot= false);
    BarPlotPtr histogramPlot(float*& data, unsigned int n,
                             unsigned int numBins, bool overplot= false);
    BarPlotPtr histogramPlot(int*& data, unsigned int n,
                             unsigned int numBins, bool overplot= false);
    BarPlotPtr histogramPlot(casacore::Vector<double>& data, unsigned int numBins,
                             bool overplot = false);
    BarPlotPtr histogramPlot(casacore::Vector<float>& data, unsigned int numBins,
                             bool overplot = false);
    BarPlotPtr histogramPlot(casacore::Vector<int>& data, unsigned int numBins,
                             bool overplot = false);
    
    BarPlotPtr histogramPlot(PlotSingleDataPtr data, unsigned int numBins,
                             bool overplot = false);
    // </group>
    
    // Display a raster or contour plot for the given data, using the current
    // line for the contours if applicable.
    // <group>
    RasterPlotPtr rasterPlot(casacore::Matrix<double>& data, bool overplot = false);
    RasterPlotPtr rasterPlot(casacore::Matrix<float>& data, bool overplot = false);
    RasterPlotPtr rasterPlot(casacore::Matrix<int>& data, bool overplot = false);
    RasterPlotPtr rasterPlot(casacore::Matrix<casacore::uInt>& data, bool overplot = false);
    RasterPlotPtr rasterPlot(casacore::Matrix<double>& data, double fromX, double toX,
                             double fromY, double toY, bool overplot = false);
    RasterPlotPtr rasterPlot(casacore::Matrix<float>& data, double fromX, double toX,
                             double fromY, double toY, bool overplot = false);
    RasterPlotPtr rasterPlot(casacore::Matrix<int>& data, double fromX, double toX,
                             double fromY, double toY, bool overplot = false);
    RasterPlotPtr rasterPlot(casacore::Matrix<casacore::uInt>& data, double fromX, double toX,
                             double fromY, double toY, bool overplot = false);
    RasterPlotPtr rasterPlot(PlotRasterDataPtr data, bool overplot = false);
    
    RasterPlotPtr contourPlot(casacore::Matrix<double>& data, casacore::Vector<double>& contours,
                              bool overplot = false);
    RasterPlotPtr contourPlot(casacore::Matrix<float>& data, casacore::Vector<float>& contours,
                              bool overplot = false);
    RasterPlotPtr contourPlot(casacore::Matrix<int>& data, casacore::Vector<int>& contours,
                              bool overplot = false);
    RasterPlotPtr contourPlot(casacore::Matrix<casacore::uInt>& data, casacore::Vector<casacore::uInt>& contours,
                              bool overplot = false);
    RasterPlotPtr contourPlot(casacore::Matrix<double>& data, double fromX, double toX,
                              double fromY,double toY,casacore::Vector<double>& contours,
                              bool overplot = false);
    RasterPlotPtr contourPlot(casacore::Matrix<float>& data, double fromX, double toX,
                              double fromY,double toY, casacore::Vector<float>& contours,
                              bool overplot = false);
    RasterPlotPtr contourPlot(casacore::Matrix<int>& data, double fromX, double toX,
                              double fromY, double toY, casacore::Vector<int>& contours,
                              bool overplot = false);
    RasterPlotPtr contourPlot(casacore::Matrix<casacore::uInt>& data, double fromX, double toX,
                              double fromY, double toY, casacore::Vector<casacore::uInt>& contours,
                              bool overplot = false);
    RasterPlotPtr contourPlot(PlotRasterDataPtr data, std::vector<double>& contours,
                              bool overplot = false);

    RasterPlotPtr spectrogram(casacore::Matrix<double>& data, bool overplt = false);
    RasterPlotPtr spectrogram(casacore::Matrix<float>& data, bool overplot = false);
    RasterPlotPtr spectrogram(casacore::Matrix<int>& data, bool overplot = false);
    RasterPlotPtr spectrogram(casacore::Matrix<casacore::uInt>& data, bool overplot = false);
    RasterPlotPtr spectrogram(casacore::Matrix<double>& data, double fromX, double toX,
                              double fromY, double toY, bool overplot = false);
    RasterPlotPtr spectrogram(casacore::Matrix<float>& data, double fromX, double toX,
                              double fromY, double toY, bool overplot = false);
    RasterPlotPtr spectrogram(casacore::Matrix<int>& data, double fromX, double toX,
                              double fromY, double toY, bool overplot = false);
    RasterPlotPtr spectrogram(casacore::Matrix<casacore::uInt>& data, double fromX, double toX,
                              double fromY, double toY, bool overplot = false);
    RasterPlotPtr spectrogram(PlotRasterDataPtr data, bool overplot = false);
    
    RasterPlotPtr spectrogram(casacore::Matrix<double>& d, casacore::Vector<double>& contours,
                              bool overplot = false);
    RasterPlotPtr spectrogram(casacore::Matrix<float>& data,casacore::Vector<float>& contours,
                              bool overplot = false);
    RasterPlotPtr spectrogram(casacore::Matrix<int>& data, casacore::Vector<int>& contours,
                              bool overplot = false);
    RasterPlotPtr spectrogram(casacore::Matrix<casacore::uInt>& data, casacore::Vector<casacore::uInt>& contours,
                              bool overplot = false);
    RasterPlotPtr spectrogram(casacore::Matrix<double>& d, double fromX, double toX,
                              double fromY,double toY,casacore::Vector<double>& contours,
                              bool overplot = false);
    RasterPlotPtr spectrogram(casacore::Matrix<float>& data,double fromX,double toX,
                              double fromY,double toY, casacore::Vector<float>& contours,
                              bool overplot = false);
    RasterPlotPtr spectrogram(casacore::Matrix<int>& data, double fromX, double toX,
                              double fromY, double toY, casacore::Vector<int>& contours,
                              bool overplot = false);
    RasterPlotPtr spectrogram(casacore::Matrix<casacore::uInt>& data, double fromX,double toX,
                              double fromY, double toY, casacore::Vector<casacore::uInt>& contours,
                              bool overplot = false);
    RasterPlotPtr spectrogram(PlotRasterDataPtr data, std::vector<double>& contours,
                              bool overplot = false);
    // </group>
    
    // Plot a point at the given location, using the current symbol.
    PlotPointPtr plotPoint(double x, double y);
    
    
    // Shapes, Annotations, etc.
    
    // Draw an annotation (text) on the canvas at the given point.
    PlotAnnotationPtr annotation(double x, double y, const casacore::String& text);
    
    // Draw a rectangle from the given upper left point to the given
    // lower right point.
    PlotShapeRectanglePtr rectangle(double left, double top,
                                    double right, double bottom);
    
    // Draw an ellipse with the given point as the center and the given
    // x and y radii.
    // <group>
    PlotShapeEllipsePtr ellipse(double centerX, double centerY,
                                double xRadius, double yRadius);
    PlotShapeEllipsePtr ellipse(double x, double y, double radius);
    // </group>
    
    // Draw a line at the given x value.
    PlotShapeLinePtr xLine(double value);
    
    // Draw a line at the given y value.
    PlotShapeLinePtr yLine(double value);
    
    // Draw an arrow from the given point to the given point
    PlotShapeArrowPtr arrow(double xFrom, double yFrom,
                            double xTo, double yTo);
    
    // Draw a line segment from the given point to the given point
    PlotShapeArrowPtr lineSegment(double xFrom, double yFrom,
                                  double xTo, double yTo);
    
    
    // Clearing Methods
    
    // Clear all items currently on the canvas.
    void clear();
    
    // Clear just the points that have been accumulated using plotPoint calls.
    void clearPoints();
    
    
    // Interaction Methods
    
    // Show or hide default "hand tools" panel - i.e., zooming, panning, etc.
    // See Plotter::DefaultPanel::HAND_TOOLS.
    void showDefaultHandTools(bool show = true);
    
    // Show or hide default "export tools" panel - i.e., saving to a file.
    // See Plotter::DefaultPanel::EXPORT_TOOLS.
    void showDefaultExportTools(bool show = true);
    
    // Returns all selected regions in the canvas' selected region list.  This
    // list will contain all user-selected regions since either its
    // construction or the last call to clearSelectedRegions().
    std::vector<PlotRegion> allSelectedRegions();
    
    // Clears the canvas' list of selected regions.
    void clearSelectedRegions();
    
    
    // Export Methods
    
    // Show a file chooser dialog with the given optional window title and
    // starting directory.  Returns the absolute filename that the user
    // selected, or an empty casacore::String if they pushed "Cancel".
    casacore::String fileChooserDialog(const casacore::String& title = "File Chooser",
                             const casacore::String& directory = "");
    
    // Exports the plotter to a PDF file at the given location.  If highQuality
    // is false, a screenshot-like export is used.  Dots per inch can be set
    // using dpi.
    bool exportPDF(const casacore::String& location, bool highQuality = false,
                   int dpi = -1);
    
    // Exports the plotter to a PS file at the given location.  If highQuality
    // is false, a screenshot-like export is used.  Dots per inch can be set
    // using dpi.
    bool exportPS(const casacore::String& location, bool highQuality = false,
                  int dpi = -1);
    
    // Exports the plotter to a JPG file at the given location.  If highQuality
    // is false, a screenshot-like export is used.  Width and height of the
    // image can be set.
    bool exportJPG(const casacore::String& location, bool highQuality = false,
                   int width = -1, int height = -1);
    
    // Exports the plotter to a PNG file at the given location.  If highQuality
    // is false, a screenshot-like export is used.  Width and height of the
    // image can be set.
    bool exportPNG(const casacore::String& location, bool highQuality = false,
                   int width = -1, int height = -1);
    
    // Exports the plotter using the given format.
    bool exportToFile(const PlotExportFormat& format);
    
private:
    // Factory
    PlotFactoryPtr m_factory;
    
    // Plotter, with single canvas
    PlotterPtr m_plotter;
    
    // Canvas
    PlotCanvasPtr m_canvas;
    
    // Mouse tools
    PlotStandardMouseToolGroupPtr m_mouseTools;
    
    // Points accumulated using plotPoint calls.
    std::vector<PlotPointPtr> m_accumulatedPoints;
    
    // Line for future plots.
    PlotLinePtr m_line;
    
    // Symbol for future plots and points.
    PlotSymbolPtr m_symbol;
    
    // Area fill for future histograms, shapes, etc.
    PlotAreaFillPtr m_areaFill;
};

typedef casacore::CountedPtr<SimplePlotter> SimplePlotterPtr;

}

#endif /*SIMPLEPLOTTER_H_*/
