//# PlotFactory.cc
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
#include <graphics/GenericPlotter/PlotFactory.h>

using namespace casacore;
namespace casa {

/////////////////////////////
// PLOTFACTORY DEFINITIONS //
/////////////////////////////

PlotFactory::PlotFactory() { }

PlotFactory::~PlotFactory() { }


PlotterPtr PlotFactory::plotter(PlotCanvasPtr canvas,const String& windowTitle,
        bool showGUI, int logEventFlags, bool smartDelete) {
    PlotterPtr p = plotter(windowTitle, false, showGUI, logEventFlags,
                           smartDelete);
    if(!canvas.null()) p->setCanvasLayout(new PlotLayoutSingle(canvas));
    return p;
}


MaskedScatterPlotPtr PlotFactory::maskedPlot(PlotMaskedPointDataPtr data,
        const String& title, bool smartDelete) const {
    return scatterPlot(data, title, smartDelete); }

ErrorPlotPtr PlotFactory::errorPlot(PlotErrorDataPtr data,
        const String& title, bool smartDelete) const {
    return scatterPlot(data, title, smartDelete); }

ColoredPlotPtr PlotFactory::coloredPlot(PlotBinnedDataPtr data,
        const String& title, bool smartDelete) const {
    return scatterPlot(data, title, smartDelete); }

BarPlotPtr PlotFactory::histogramPlot(PlotSingleDataPtr data,
        unsigned int numBins, const String& title, bool smartDelete) const {
    return barPlot(new PlotHistogramData(data, numBins), title, smartDelete); }

RasterPlotPtr PlotFactory::contourPlot(PlotRasterDataPtr data,
        const std::vector<double>& contours, const String& title,
        PlotRasterData::Format format, bool smartDelete) const {
    RasterPlotPtr p = rasterPlot(data, title, format, smartDelete);
    p->setContourLines(contours);
    return p;
}

RasterPlotPtr PlotFactory::spectrogramPlot(PlotRasterDataPtr data,
        const String& title, bool smartDelete) const{
    return rasterPlot(data, title, PlotRasterData::SPECTROGRAM, smartDelete); }

RasterPlotPtr PlotFactory::contouredSpectrogramPlot(PlotRasterDataPtr data,
        const std::vector<double>& cont, const String& title,bool smartDelete)const{
    RasterPlotPtr p = rasterPlot(data, title, PlotRasterData::SPECTROGRAM,
                                 smartDelete);
    p->setContourLines(cont);
    return p;
}

PlotAnnotationPtr PlotFactory::annotation(const String& text, double x,
        double y, bool smartDelete) const {
    return annotation(text, PlotCoordinate(x, y, PlotCoordinate::WORLD),
                      smartDelete);
}

PlotShapeRectanglePtr PlotFactory::shapeRectangle(double left, double top,
        double right, double bottom, bool smartDelete) const {
    return shapeRectangle(PlotCoordinate(left, top, PlotCoordinate::WORLD),
            PlotCoordinate(right, bottom, PlotCoordinate::WORLD),
            smartDelete);
}

PlotShapeEllipsePtr PlotFactory::shapeEllipse(double x, double y,
        double xRadius, double yRadius, bool smartDelete) const {
    return shapeEllipse(PlotCoordinate(x, y, PlotCoordinate::WORLD),
            PlotCoordinate(xRadius, yRadius, PlotCoordinate::WORLD),
            smartDelete);
}

PlotShapePolygonPtr PlotFactory::shapePolygon(const std::vector<double>& x,
        const std::vector<double>& y, bool smartDelete) const {
    std::vector<PlotCoordinate> c(min((uInt)x.size(), (uInt)y.size()));
    for(unsigned int i = 0; i < c.size(); i++)
        c[i] = PlotCoordinate(x[i], y[i], PlotCoordinate::WORLD);
    return shapePolygon(c, smartDelete);
}

PlotShapeArrowPtr PlotFactory::shapeArrow(double fromX, double fromY,
        double toX, double toY, PlotShapeArrow::Style fromArrow,
        PlotShapeArrow::Style toArrow, bool smartDelete) const {
    return shapeArrow(PlotCoordinate(fromX, fromY, PlotCoordinate::WORLD),
                      PlotCoordinate(toX, toY, PlotCoordinate::WORLD),
                      fromArrow, toArrow, smartDelete);
}

PlotShapeArrowPtr PlotFactory::shapeLineSegment(const PlotCoordinate& from,
        const PlotCoordinate& to, bool smartDelete) const {
    return shapeArrow(from, to, PlotShapeArrow::NOARROW,
                      PlotShapeArrow::NOARROW, smartDelete);
}

PlotShapeArrowPtr PlotFactory::shapeLineSegment(double fromX, double fromY,
        double toX, double toY, bool smartDelete) const {
    return shapeArrow(PlotCoordinate(fromX, fromY, PlotCoordinate::WORLD),
                      PlotCoordinate(toX, toY, PlotCoordinate::WORLD),
                      PlotShapeArrow::NOARROW, PlotShapeArrow::NOARROW,
                      smartDelete);
}

PlotShapePathPtr PlotFactory::shapePath(const std::vector<double>& x,
        const std::vector<double>& y, bool smartDelete) const {
    std::vector<PlotCoordinate> c(min((uInt)x.size(), (uInt)y.size()));
    for(unsigned int i = 0; i < c.size(); i++)
        c[i] = PlotCoordinate(x[i], y[i], PlotCoordinate::WORLD);
    return shapePath(c, smartDelete);
}

PlotPointPtr PlotFactory::point(double x, double y, bool smartDelete) const {
    return point(PlotCoordinate(x, y, PlotCoordinate::WORLD),smartDelete); }

PlotPointPtr PlotFactory::point(float x, float y, bool smartDelete) const {
    return point(PlotCoordinate(x, y, PlotCoordinate::WORLD),smartDelete); }

PlotPointPtr PlotFactory::point(int x, int y, bool smartDelete) const {
    return point(PlotCoordinate(x, y, PlotCoordinate::WORLD),smartDelete); }

PlotPointPtr PlotFactory::point(unsigned int x, unsigned int y,
        bool smartDelete) const {
    return point(PlotCoordinate(x, y, PlotCoordinate::WORLD),smartDelete); }


PlotColorPtr PlotFactory::color(const PlotColorPtr copy,
        bool smartDelete) const {
    if(!copy.null()) return color(*copy, smartDelete);
    else             return copy;
}

PlotFontPtr PlotFactory::font(const PlotFontPtr copy, bool smartDelete) const {
    if(!copy.null()) return font(*copy, smartDelete);
    else             return copy;
}

PlotAreaFillPtr PlotFactory::areaFill(const PlotAreaFillPtr copy,
        bool smartDelete) const {
    if(!copy.null()) return areaFill(*copy, smartDelete);
    else             return copy;
}

PlotLinePtr PlotFactory::line(const PlotLinePtr copy, bool smartDelete) const {
    if(!copy.null()) return line(*copy, smartDelete);
    else             return copy;
}

PlotSymbolPtr PlotFactory::symbol(char sym, bool smartDelete) const {
    PlotSymbolPtr s = symbol(PlotSymbol::CHARACTER, smartDelete);
    s->setSymbol(sym);
    return s;
}

PlotSymbolPtr PlotFactory::createSymbol (const String& descriptor,
		Int size, const String& color,
    	const String& fillPattern, bool outline ){
	PlotSymbolPtr ps = symbol(PlotSymbol::NOSYMBOL);
	ps->setSymbol( descriptor );
	ps->setSize( size, size );
	ps->setColor( color );
	PlotAreaFillPtr paf = areaFill(color, PlotAreaFill::NOFILL);
	paf ->setPattern( fillPattern );
	ps->setAreaFill( paf );
	if(outline) {
		ps->setLine("black");
	}
	return ps;
}

PlotSymbolPtr PlotFactory::uSymbol(unsigned short unicode,
        bool smartDelete) const {
    PlotSymbolPtr s = symbol(PlotSymbol::CHARACTER, smartDelete);
    s->setUSymbol(unicode);
    return s;
}

PlotSymbolPtr PlotFactory::symbol(const PlotSymbolPtr copy,
        bool smartDelete) const {
    if(!copy.null()) return symbol(*copy, smartDelete);
    else             return copy;
}



PlotStandardMouseToolGroupPtr   PlotFactory::standardMouseTools(
        ToolCode activeTool, 
        bool smartDelete) const    {
            
    PlotSelectToolPtr sel = selectTool();
    sel->setSelectLine(line("black", PlotLine::SOLID, 1.0));
    sel->setRectLine(line("black", PlotLine::SOLID, 1.0));
    sel->setRectFill(areaFill("black", PlotAreaFill::MESH3));
    return PlotStandardMouseToolGroupPtr(new PlotStandardMouseToolGroup(
            sel, zoomTool(), panTool(), flagAllTool(), trackerTool(), activeTool),
            smartDelete);
}




PlotStandardMouseToolGroupPtr   PlotFactory::standardMouseTools(
        PlotAxis xAxis,
        PlotAxis yAxis, 
        PlotCoordinate::System sys,
        ToolCode activeTool, 
        bool smartDelete) const    {

    PlotSelectToolPtr sel = selectTool(xAxis, yAxis, sys);
    sel->setSelectLine(line("black", PlotLine::SOLID, 1.0));
    sel->setSubtractLine(line("violet", PlotLine::DASHED, 2.0));   //DSW: if compiles ok, change to #9020C8
    sel->setRectLine(line("black", PlotLine::SOLID, 1.0));
    sel->setRectFill(areaFill("black", PlotAreaFill::MESH3));
    return PlotStandardMouseToolGroupPtr(new PlotStandardMouseToolGroup(
            sel, zoomTool(xAxis, yAxis, sys), panTool(xAxis, yAxis, sys),
            flagAllTool(xAxis, yAxis, sys),
            trackerTool(xAxis, yAxis, sys), activeTool), smartDelete);
}

PlotSelectToolPtr PlotFactory::selectTool(bool smartDelete) const {
    return PlotSelectToolPtr(new PlotSelectTool(), smartDelete); }

PlotZoomToolPtr PlotFactory::zoomTool(bool smartDelete) const { 
    return PlotZoomToolPtr(new PlotZoomTool(), smartDelete); }

PlotPanToolPtr PlotFactory::panTool(bool smartDelete) const {
    return PlotPanToolPtr(new PlotPanTool(), smartDelete); }

PlotFlagAllToolPtr PlotFactory::flagAllTool(bool smartDelete) const {
    return PlotFlagAllToolPtr(new PlotFlagAllTool(), smartDelete); }

PlotTrackerToolPtr PlotFactory::trackerTool(bool smartDelete) const {
    return PlotTrackerToolPtr(new PlotTrackerTool(), smartDelete); }

PlotSelectToolPtr PlotFactory::selectTool(PlotAxis xAxis, PlotAxis yAxis,
        PlotCoordinate::System system, bool smartDelete) const {
    return PlotSelectToolPtr(new PlotSelectTool(xAxis, yAxis, system),
            smartDelete);
}

PlotZoomToolPtr PlotFactory::zoomTool(PlotAxis xAxis, PlotAxis yAxis,
        PlotCoordinate::System system, bool smartDelete) const {
    return PlotZoomToolPtr(new PlotZoomTool(xAxis, yAxis, system),
            smartDelete);
}

PlotPanToolPtr PlotFactory::panTool(PlotAxis xAxis, PlotAxis yAxis,
        PlotCoordinate::System system, bool smartDelete) const {
    
    (void)smartDelete;
    return PlotPanToolPtr(new PlotPanTool(xAxis, yAxis, system), false); }

PlotFlagAllToolPtr PlotFactory::flagAllTool(PlotAxis xAxis, PlotAxis yAxis,
        PlotCoordinate::System system, bool smartDelete) const {
    return PlotFlagAllToolPtr(new PlotFlagAllTool(xAxis, yAxis, system),
        smartDelete);
}

PlotTrackerToolPtr PlotFactory::trackerTool(PlotAxis xAxis, PlotAxis yAxis,
        PlotCoordinate::System system, bool smartDelete) const {
    return PlotTrackerToolPtr(new PlotTrackerTool(xAxis, yAxis, system),
            smartDelete);
}


// Macro for method definitions for PF_DATA_DEC declarations.
#define PF_DATA_DEF(TYPE)                                                     \
PlotPointDataPtr PlotFactory::data(TYPE *& y, unsigned int n,                 \
        bool shouldDelete) const {                                            \
    return new PlotPointDataImpl< TYPE >(y, n, shouldDelete); }               \
                                                                              \
PlotPointDataPtr PlotFactory::data(Vector< TYPE >& y,                         \
        bool shouldDelete) const {                                            \
    return new PlotPointDataImpl< TYPE >(y, shouldDelete); }                  \
                                                                              \
PlotPointDataPtr PlotFactory::data(std::vector< TYPE >& y,                    \
        bool shouldDelete) const {                                            \
    return new PlotPointDataImpl< TYPE >(y, shouldDelete); }                  \
                                                                              \
PlotPointDataPtr PlotFactory::data(TYPE *& x, TYPE *& y, unsigned int n,      \
        bool shouldDelete) const {                                            \
    return new PlotPointDataImpl< TYPE >(x, y, n, shouldDelete); }            \
                                                                              \
PlotPointDataPtr PlotFactory::data(Vector< TYPE >& x, Vector< TYPE >& y,      \
        bool shouldDelete) const {                                            \
    return new PlotPointDataImpl< TYPE >(x, y, shouldDelete); }               \
                                                                              \
PlotPointDataPtr PlotFactory::data(std::vector< TYPE >& x, std::vector< TYPE >& y,      \
        bool shouldDelete) const {                                            \
    return new PlotPointDataImpl< TYPE >(x, y, shouldDelete); }               \
                                                                              \
PlotSingleDataPtr PlotFactory::singleData(TYPE *& data, unsigned int n,       \
        bool shouldDelete) const {                                            \
    return new PlotSingleDataImpl< TYPE >(data, n, shouldDelete); }           \
                                                                              \
PlotSingleDataPtr PlotFactory::singleData(Vector< TYPE >& data,               \
        bool shouldDelete) const {                                            \
    return new PlotSingleDataImpl< TYPE >(data, shouldDelete); }              \
                                                                              \
PlotSingleDataPtr PlotFactory::singleData(std::vector< TYPE >& data,               \
        bool shouldDelete) const {                                            \
    return new PlotSingleDataImpl< TYPE >(data, shouldDelete); }              \
                                                                              \
PlotPointDataPtr PlotFactory::histogramData(TYPE *& data, unsigned int n,     \
        unsigned int numBins, bool shouldDel) const {                         \
    return new PlotHistogramData(singleData(data, n, shouldDel), numBins); }  \
                                                                              \
PlotPointDataPtr PlotFactory::histogramData(Vector< TYPE >& data,             \
        unsigned int numBins, bool shouldDel) const {                         \
    return new PlotHistogramData(singleData(data, shouldDel), numBins); }     \
                                                                              \
PlotPointDataPtr PlotFactory::histogramData(std::vector< TYPE >& data,             \
        unsigned int numBins, bool shouldDel) const {                         \
    return new PlotHistogramData(singleData(data, shouldDel), numBins); }     \
                                                                              \
PlotMaskedPointDataPtr PlotFactory::data(TYPE *& x, TYPE*& y, bool*& mask,    \
        unsigned int n, bool shouldDelete) const {                            \
    return new PlotMaskedPointDataImpl< TYPE >(x, y, mask, n, shouldDelete); }\
                                                                              \
PlotMaskedPointDataPtr PlotFactory::data(Vector< TYPE >& x, Vector< TYPE >& y,\
        Vector<bool>& mask, bool shouldDelete) const {                        \
    return new PlotMaskedPointDataImpl< TYPE >(x, y, mask, shouldDelete); }   \
                                                                              \
PlotMaskedPointDataPtr PlotFactory::data(std::vector< TYPE >& x, std::vector< TYPE >& y,\
        std::vector<bool>& mask, bool shouldDelete) const {                        \
    return new PlotMaskedPointDataImpl< TYPE >(x, y, mask, shouldDelete); }   \
                                                                              \
PlotErrorDataPtr PlotFactory::data(TYPE *& x, TYPE *& y, unsigned int n,      \
        TYPE xLeftError, TYPE xRightError, TYPE yBottomError, TYPE yTopError, \
        bool shouldDelete) const {                                            \
    return new PlotScalarErrorDataImpl< TYPE >(x, y, n, xLeftError,           \
            xRightError, yBottomError, yTopError, shouldDelete); }            \
                                                                              \
PlotErrorDataPtr PlotFactory::data(Vector< TYPE >& x, Vector< TYPE >& y,      \
        TYPE xLeftError, TYPE xRightError, TYPE yBottomError, TYPE yTopError, \
        bool shouldDelete) const {                                            \
    return new PlotScalarErrorDataImpl< TYPE >(x, y, xLeftError,              \
            xRightError, yBottomError, yTopError, shouldDelete); }            \
                                                                              \
PlotErrorDataPtr PlotFactory::data(std::vector< TYPE >& x, std::vector< TYPE >& y,      \
        TYPE xLeftError, TYPE xRightError, TYPE yBottomError, TYPE yTopError, \
        bool shouldDelete) const {                                            \
    return new PlotScalarErrorDataImpl< TYPE >(x, y, xLeftError,              \
            xRightError, yBottomError, yTopError, shouldDelete); }            \
                                                                              \
PlotErrorDataPtr PlotFactory::data(TYPE *& x, TYPE *& y, TYPE *& xLeftError,  \
        TYPE *& xRightError, TYPE *& yBottomError, TYPE *& yTopError,         \
        unsigned int n, bool shouldDelete) const {                            \
    return new PlotErrorDataImpl< TYPE >(x, y, xLeftError, xRightError,       \
            yBottomError, yTopError, n, shouldDelete); }                      \
                                                                              \
PlotErrorDataPtr PlotFactory::data(Vector< TYPE >& x, Vector< TYPE >& y,      \
        Vector< TYPE >& xLeftError, Vector< TYPE >& xRightError,              \
        Vector< TYPE >& yBottomError, Vector< TYPE >& yTopError,              \
        bool shouldDelete) const {                                            \
    return new PlotErrorDataImpl< TYPE >(x, y, xLeftError, xRightError,       \
            yBottomError, yTopError, shouldDelete); }                         \
                                                                              \
PlotErrorDataPtr PlotFactory::data(std::vector< TYPE >& x, std::vector< TYPE >& y,      \
        std::vector< TYPE >& xLeftError, std::vector< TYPE >& xRightError,              \
        std::vector< TYPE >& yBottomError, std::vector< TYPE >& yTopError,              \
        bool shouldDelete) const {                                            \
    return new PlotErrorDataImpl< TYPE >(x, y, xLeftError, xRightError,       \
            yBottomError, yTopError, shouldDelete); }                         \
                                                                              \
PlotRasterDataPtr PlotFactory::data(Matrix< TYPE >& data,                     \
        bool shouldDelete) const {                                            \
    return new PlotRasterMatrixData< TYPE >(data, shouldDelete); }            \
                                                                              \
PlotRasterDataPtr PlotFactory::data(Matrix< TYPE >& data, double fromX,       \
        double toX, double fromY, double toY, bool shouldDelete) const {      \
    PlotRasterMatrixData< TYPE >* d = new PlotRasterMatrixData< TYPE > (      \
            data, shouldDelete);                                              \
    d->setXRange(fromX, toX);                                                 \
    d->setYRange(fromY, toY);                                                 \
    return d;                                                                 \
}
    
PF_DATA_DEF(double)

PF_DATA_DEF(float)

PF_DATA_DEF(int)

PF_DATA_DEF(unsigned int)
    
PlotPointDataPtr PlotFactory::histogramData(PlotSingleDataPtr data,
        unsigned int numBins) const {
    return new PlotHistogramData(data, numBins); }

}
