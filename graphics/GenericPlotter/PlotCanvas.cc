//# PlotCanvas.cc: Main drawing area for different plot items to be attached.
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


#include <graphics/GenericPlotter/PlotCanvas.h>
#include <graphics/GenericPlotter/PlotFactory.h>
#include <graphics/GenericPlotter/PlotOptions.h>

using namespace std;

using namespace casacore;
namespace casa {

////////////////////////////
// PLOTCANVAS DEFINITIONS //
////////////////////////////

// Static //

vector<PlotCanvas::LegendPosition> PlotCanvas::allLegendPositions() {
    static vector<LegendPosition> v(8);
    v[0] = INT_URIGHT; v[1] = INT_LRIGHT;
    v[2] = INT_ULEFT;  v[3] = INT_LLEFT;
    v[4] = EXT_RIGHT;  v[5] = EXT_TOP;
    v[6] = EXT_LEFT;   v[7] = EXT_BOTTOM;
    return v;
}

vector<String> PlotCanvas::allLegendPositionStrings() {
    static vector<LegendPosition> p = allLegendPositions();
    static vector<String> v(p.size());
    for(unsigned int i = 0; i < v.size(); i++) v[i] = legendPosition(p[i]);
    return v;
}

String PlotCanvas::legendPosition(LegendPosition p) {
    switch(p) {
    case INT_URIGHT: return "Upper Right";
    case INT_LRIGHT: return "Lower Right";
    case INT_ULEFT:  return "Upper Left";
    case INT_LLEFT:  return "Lower Left";
    case EXT_RIGHT:  return "Out Right";
    case EXT_TOP:    return "Out Top";
    case EXT_LEFT:   return "Out Left";
    case EXT_BOTTOM: return "Out Bottom";
    
    default: return "";
    }
}

PlotCanvas::LegendPosition PlotCanvas::legendPosition(String p, bool* ok) {
    p.downcase();
    if(ok != NULL) *ok = true;
    if(p == "upper right")      return INT_URIGHT;
    else if(p == "upper left")  return INT_ULEFT;
    else if(p == "lower right") return INT_LRIGHT;
    else if(p == "lower left")  return INT_LLEFT;
    else if(p == "out right")   return EXT_RIGHT;
    else if(p == "out left")    return EXT_LEFT;
    else if(p == "out top")     return EXT_TOP;
    else if(p == "out bottom")  return EXT_BOTTOM;
    
    if(ok != NULL) *ok = false;
    return INT_URIGHT;
}

bool PlotCanvas::legendPositionIsInternal(LegendPosition p) {
    return p == INT_URIGHT || p == INT_ULEFT || p == INT_LRIGHT ||
           p == INT_LLEFT;
}

vector<PlotAxis> PlotCanvas::allAxes() {
    vector<PlotAxis> v(4);
    v[0] = X_BOTTOM; v[1] = X_TOP; v[2] = Y_LEFT; v[3] = Y_RIGHT;
    return v;
}

int PlotCanvas::allAxesFlag() {
    int flag = 0;
    vector<PlotAxis> v = allAxes();
    for(unsigned int i = 0; i < v.size(); i++) flag |= v[i];
    return flag;
}

vector<PlotCanvasLayer> PlotCanvas::allLayers() {
    vector<PlotCanvasLayer> v(2);
    v[0] = MAIN; v[1] = ANNOTATION;
    return v;
}

int PlotCanvas::allLayersFlag() {
    int flag = 0;
    vector<PlotCanvasLayer> v = allLayers();
    for(unsigned int i = 0; i < v.size(); i++) flag |= v[i];
    return flag;
}

const String PlotCanvas::OPERATION_DRAW = "draw_items";
const String PlotCanvas::OPERATION_EXPORT = "export";


// Non-Static //

PlotCanvas::PlotCanvas() { }

PlotCanvas::~PlotCanvas() {
    // Calling this here results in a pure virtual method exception.
    // Virtual methods in constructors and destructors do not resolve!
    //operationDraw()->finish();
}
bool PlotCanvas::hasSelectedRectangles(){
	bool selectedRectangles = false;
	if ( !m_standardTools.null() ){
		int rectCount = m_standardTools->getSelectedRectCount();
		if ( rectCount > 0 ){
			selectedRectangles = true;
		}
	}
	return selectedRectangles;
}

vector<PlotRegion> PlotCanvas::getSelectedRects(){
	vector<PlotRegion> regions;
	if ( !m_standardTools.null()){
		regions = m_standardTools->getSelectedRects();
	}
	return regions;
}

void PlotCanvas::clearSelectedRects(){
	if ( !m_standardTools.null()){
		m_standardTools->clearSelectedRects();
	}
}

void PlotCanvas::clearMark() {
  if (!m_standardTools.null()) {
    m_standardTools->clearMark();
  }
}

bool PlotCanvas::isMarkedForFlag() {
  return (!m_standardTools.null()) ? m_standardTools->isMarkedForFlag() : false;
}

bool PlotCanvas::isMarkedForUnflag() {
  return (!m_standardTools.null()) ? m_standardTools->isMarkedForUnflag() : false;
}

bool PlotCanvas::isBackgroundColorChanged() {
  return (!m_standardTools.null()) ? m_standardTools->isBackgroundColorChanged() : false;
}

void PlotCanvas::setAllFlagged() {
  if (!m_standardTools.null()) {
    m_standardTools->setAllFlagged();
  }
}

bool PlotCanvas::hasThreadedDrawing() const {
    PlotFactory* f = implementationFactory();
    bool ret = f != NULL && f->canvasHasThreadedDrawing();
    delete f;
    return ret;
}

bool PlotCanvas::hasCachedLayerDrawing() const {
    PlotFactory* f = implementationFactory();
    bool ret = f != NULL && f->canvasHasCachedLayerDrawing();
    delete f;
    return ret;
}

bool PlotCanvas::hasCachedAxesStack() const {
    PlotFactory* f = implementationFactory();
    bool ret = f != NULL && f->canvasHasCachedAxesStack();
    delete f;
    return ret;
}



void PlotCanvas::setTitleFont(const PlotFontPtr font) {
    if(!font.null()) setTitleFont(*font); 
}


PlotAreaFillPtr PlotCanvas::defaultBackground() const {
  return PlotAreaFillPtr(nullptr);
}

void PlotCanvas::setBackground(const PlotAreaFillPtr areaFill)   {
    if(!areaFill.null()) setBackground(*areaFill); 
}


void PlotCanvas::setBackground(const String& color,
        PlotAreaFill::Pattern pattern)    {
    PlotAreaFillPtr bg = background();
    bg->setColor(color);
    bg->setPattern(pattern);
    setBackground(*bg);
}



bool PlotCanvas::isAxisShown(PlotAxis axis) const { 
	return shownAxes() & axis; 
}


/* DSW: SITE OF X-Y SHORTCIRCUIT */
void PlotCanvas::showAxis(PlotAxis axis, bool show) {
    PlotAxisBitset  axes = shownAxes();
    if (show) axes |=  (PlotAxisBitset)axis;
    else      axes &= ~(PlotAxisBitset)axis;
    showAxes(axes);  /* calls QPCanvas::showAxes() */
}



void PlotCanvas::showAxes(PlotAxis xAxis, PlotAxis yAxis, bool show) {
    PlotAxisBitset combined = ( ((PlotAxisBitset)xAxis) | ((PlotAxisBitset)yAxis) );
    PlotAxisBitset axes = shownAxes();
    if (show) axes |=  combined;
    else      axes &= ~combined;
    showAxes(axes);   /* calls casaqt's QPCanvas::showAxes() */
}



void PlotCanvas::showAllAxes(bool show) {
    if(show) showAxes(X_BOTTOM | X_TOP | Y_LEFT | Y_RIGHT);
    else     showAxes(0);
}



void PlotCanvas::setAxesScales(PlotAxis xAxis, PlotAxisScale xScale,
            PlotAxis yAxis, PlotAxisScale yScale) {
    setAxisScale(xAxis, xScale);
    setAxisScale(yAxis, yScale);
}



void PlotCanvas::showCartesianAxis(PlotAxis mirrorAxis, bool show,
        bool hideNormalAxis) {
    switch(mirrorAxis) {
    case X_BOTTOM: case X_TOP:
        showCartesianAxis(mirrorAxis, Y_LEFT, show, hideNormalAxis);
        break;
    case Y_LEFT: case Y_RIGHT:
        showCartesianAxis(mirrorAxis, X_BOTTOM, show, hideNormalAxis);
        break;
    }
}



void PlotCanvas::showCartesianAxes(bool show, bool hideNormal) {
    showCartesianAxis(X_BOTTOM, show, hideNormal);
    showCartesianAxis(Y_LEFT, show, hideNormal);
}



void PlotCanvas::clearAxesLabels() {
    setAxisLabel(X_BOTTOM, "");
    setAxisLabel(X_TOP, "");
    setAxisLabel(Y_LEFT, "");
    setAxisLabel(Y_RIGHT, "");
}



void PlotCanvas::setAxisFont(PlotAxis axis, const PlotFontPtr font) {
    if(!font.null()) setAxisFont(axis, *font); 
}


PlotRegion PlotCanvas::axesRanges(PlotAxis xAxis, PlotAxis yAxis) const {
    prange_t x = axisRange(xAxis), y = axisRange(yAxis);
    return PlotRegion(PlotCoordinate(x.first, y.second),
                      PlotCoordinate(x.second, y.first));
}



void PlotCanvas::setAxisRange(PlotAxis axis, const prange_t& range){
    setAxisRange(axis, range.first, range.second); 
}



void PlotCanvas::setAxesRanges(PlotAxis xAxis, double xFrom, double xTo,
        PlotAxis yAxis, double yFrom, double yTo) {
    setAxisRange(xAxis, xFrom, xTo);
    setAxisRange(yAxis, yFrom, yTo);
}



void PlotCanvas::setAxesRanges(PlotAxis xAxis, const prange_t& xRange,
        PlotAxis yAxis, const prange_t& yRange) {
    setAxesRanges(xAxis, xRange.first, xRange.second, yAxis, yRange.first,
                  yRange.second);
}



void PlotCanvas::setAxesRegion(PlotAxis xAxis, PlotAxis yAxis,
        const PlotRegion& region) {
    PlotRegion r = region;
    if(region.upperLeft().system() != PlotCoordinate::WORLD ||
       region.lowerRight().system() != PlotCoordinate::WORLD)
        r = convertRegion(region, PlotCoordinate::WORLD);
    setAxesRanges(xAxis, region.upperLeft().x(), region.lowerRight().x(),
                  yAxis, region.lowerRight().y(), region.upperLeft().y());
}

void PlotCanvas::moveAxisRange(PlotAxis axis, double delta) {
    prange_t r = axisRange(axis);
    r.first += delta; r.second += delta;
    setAxisRange(axis, r.first, r.second);
}

void PlotCanvas::moveAxesRanges(PlotAxis xAxis, double xDelta,
        PlotAxis yAxis, double yDelta) {
    prange_t x = axisRange(xAxis), y = axisRange(yAxis);
    x.first += xDelta; x.second += xDelta;
    y.first += yDelta; y.second += yDelta;
    setAxesRanges(xAxis, x.first, x.second, yAxis, y.first, y.second);
}


PlotAxesStack& PlotCanvas::axesStack() { return m_stack; }
const PlotAxesStack& PlotCanvas::axesStack() const { return m_stack; }

bool PlotCanvas::axesStackMove(int delta) {
    PlotAxesStack& stack = axesStack();
    if(!stack.isValid()) return false;
    
    unsigned int index = stack.stackIndex(), size = stack.size();
    
    if(size == 1 || (delta <= 0 && index == 0) ||
       (delta > 0 && index == size - 1)) return false;
    
    PlotRegion r = convertRegion(stack.moveAndReturn(delta),
                                 PlotCoordinate::WORLD);
    setAxesRegion(stack.currentXAxis(), stack.currentYAxis(), r);    
    return true;
}



int PlotCanvas::axesStackLengthLimit() const {
    return axesStack().lengthLimit(); 
}



void PlotCanvas::setAxesStackLengthLimit(int lengthLimit) {
    axesStack().setLengthLimit(lengthLimit); 
}



pair<int, int> PlotCanvas::cachedAxesStackImageSize() const  {
    return pair<int, int>(-1, -1); 
}
    
    
    
void PlotCanvas::setCachedAxesStackImageSize(int width, int height) { 
	(void)width, (void)height; 
}


bool PlotCanvas::plot(PlotPtr plot, bool overplot) {
    if(!overplot) clearPlots();
    return plotItem(plot, MAIN);
}



bool PlotCanvas::plotPoint(PlotPointPtr point){ 
	return plotItem(point, MAIN); 
}


bool PlotCanvas::drawShape(PlotShapePtr shape){ 
	return plotItem(shape, MAIN); 
}


bool PlotCanvas::drawAnnotation(PlotAnnotationPtr annotation) {
    return plotItem(annotation, MAIN); 
}




// Macro for common method definitions.
#define PC_ALL(ITEM,CLASS)                                                    \
vector< CLASS##Ptr > PlotCanvas::all##ITEM##s() const {                       \
    vector<PlotItemPtr> v = allPlotItems();                                   \
    vector< CLASS##Ptr > v2;                                                  \
    for(unsigned int i = 0; i < v.size(); i++)                                \
        if(!v[i].null() && dynamic_cast< CLASS *>(&*v[i]) != NULL)            \
            v2.push_back(v[i]);                                               \
    return v2;                                                                \
}                                                                             \
                                                                              \
vector< CLASS##Ptr > PlotCanvas::layer##ITEM##s(PlotCanvasLayer layer) const {\
    vector<PlotItemPtr> v = layerPlotItems(layer);                            \
    vector< CLASS##Ptr > v2;                                                  \
    for(unsigned int i = 0; i < v.size(); i++)                                \
        if(!v[i].null() && dynamic_cast< CLASS *>(&*v[i]) != NULL)            \
            v2.push_back(v[i]);                                               \
    return v2;                                                                \
}                                                                             \
                                                                              \
void PlotCanvas::removeLast##ITEM () {                                        \
    vector< CLASS##Ptr > v = all##ITEM##s();                                  \
    for(int i = v.size() - 1; i >= 0; i--) {                                  \
        if(!v[i].null()) {                                                    \
            removePlotItem(v[i]);                                             \
            break;                                                            \
        }                                                                     \
    }                                                                         \
}                                                                             \
                                                                              \
void PlotCanvas::clear##ITEM##s() {                                           \
    vector< CLASS##Ptr > v = all##ITEM##s();                                  \
    vector<PlotItemPtr> v2(v.size());                                         \
    for(unsigned int i = 0; i < v.size(); i++) v2[i] = v[i];                  \
    removePlotItems(v2);                                                      \
}

PC_ALL(Plot, Plot)
PC_ALL(Point, PlotPoint)
PC_ALL(Shape, PlotShape)
PC_ALL(Annotation, PlotAnnotation)

unsigned int PlotCanvas::numPlotItems() const { return allPlotItems().size(); }

unsigned int PlotCanvas::numPlots() const { return allPlots().size(); }
unsigned int PlotCanvas::numPoints() const { return allPoints().size(); }
unsigned int PlotCanvas::numShapes() const { return allShapes().size(); }
unsigned int PlotCanvas::numAnnotations() const {
    return allAnnotations().size(); }

unsigned int PlotCanvas::numLayerPlotItems(PlotCanvasLayer layer) const {
    return layerPlotItems(layer).size(); }

unsigned int PlotCanvas::numLayerPlots(PlotCanvasLayer layer) const {
    return layerPlots(layer).size(); }
unsigned int PlotCanvas::numLayerPoints(PlotCanvasLayer layer) const {
    return layerPoints(layer).size(); }
unsigned int PlotCanvas::numLayerShapes(PlotCanvasLayer layer) const {
    return layerShapes(layer).size(); }
unsigned int PlotCanvas::numLayerAnnotations(PlotCanvasLayer layer) const {
    return layerAnnotations(layer).size(); }

void PlotCanvas::removePlotItem(PlotItemPtr item) {
    if(!item.null()) removePlotItems(vector<PlotItemPtr>(1, item)); }

void PlotCanvas::removePlot(PlotPtr plot) { removePlotItem(plot); }
void PlotCanvas::removePoint(PlotPointPtr point) { removePlotItem(point); }
void PlotCanvas::removeShape(PlotShapePtr shape) { removePlotItem(shape); }
void PlotCanvas::removeAnnotation(PlotAnnotationPtr annotation) {
    removePlotItem(annotation); }

void PlotCanvas::removeLastPlotItem() {
    vector<PlotItemPtr> v = allPlotItems();
    for(int i = v.size() - 1; i >= 0; i--) {
        if(!v[i].null()) {
            removePlotItem(v[i]);
            break;
        }
    }
}

void PlotCanvas::clearItems() { removePlotItems(allPlotItems()); }

void PlotCanvas::clearLayer(PlotCanvasLayer layer) {
    removePlotItems(layerPlotItems(layer));
}


PlotOperationPtr PlotCanvas::operationDraw() {
    return operationDraw(PlotMutexPtr());
}

PlotOperationPtr PlotCanvas::operationDraw(PlotMutexPtr m) {
    if(m_drawOperation.null())
        m_drawOperation = new PlotOperation(OPERATION_DRAW, mutex());
    if(!m.null()) m_drawOperation->setMutex(m);
    return m_drawOperation;
}

void PlotCanvas::registerDrawWatcher(PlotDrawWatcherPtr watcher) {
    if(watcher.null()) return;
    
    for(unsigned int i = 0; i < m_drawWatchers.size(); i++)
        if(m_drawWatchers[i] == watcher) return;
    m_drawWatchers.push_back(watcher);
}

void PlotCanvas::unregisterDrawWatcher(PlotDrawWatcherPtr watcher) {
    if(watcher.null()) return;
    for(unsigned int i = 0; i < m_drawWatchers.size(); i++) {
        if(m_drawWatchers[i] == watcher) {
            m_drawWatchers.erase(m_drawWatchers.begin() + i);
            return;
        }
    }
}


bool PlotCanvas::selectLineShown() const {
    PlotLinePtr line = selectLine();
    return !line.null() && line->style() != PlotLine::NOLINE;
}

void PlotCanvas::setSelectLine(const PlotLinePtr line) {
    if(!line.null()) setSelectLine(*line);
    else             setSelectLineShown(false);
}

void PlotCanvas::setSelectLine(const String& color, PlotLine::Style style,
        double width) {
    PlotLinePtr line = selectLine();
    line->setColor(color);
    line->setStyle(style);
    line->setWidth(width);
    setSelectLine(*line);
}


void PlotCanvas::showGrid(bool showAll) {
    showGrid(showAll, showAll, showAll, showAll); }

void PlotCanvas::showGridMajor(bool show) {
    showGrid(show, gridXMinorShown(), show, gridYMinorShown()); }

void PlotCanvas::showGridMinor(bool show) {
    showGrid(gridXMajorShown(), show, gridYMajorShown(), show); }

bool PlotCanvas::gridXMajorShown() const {
    bool show;
    gridShown(&show);
    return show;
}

void PlotCanvas::showGridXMajor(bool s) {
    showGrid(s, gridXMinorShown(), gridYMajorShown(), gridYMinorShown()); }

bool PlotCanvas::gridXMinorShown() const {
    bool show;
    gridShown(NULL, &show);
    return show;
}

void PlotCanvas::showGridXMinor(bool s) {
    showGrid(gridXMajorShown(), s, gridYMajorShown(), gridYMinorShown()); }

bool PlotCanvas::gridYMajorShown() const {
    bool show;
    gridShown(NULL, NULL, &show);
    return show;
}

void PlotCanvas::showGridYMajor(bool s) {
    showGrid(gridXMajorShown(), gridXMinorShown(), s, gridYMinorShown()); }

bool PlotCanvas::gridYMinorShown() const {
    bool show;
    gridShown(NULL, NULL, NULL, &show);
    return show;
}

void PlotCanvas::showGridYMinor(bool s) {
    showGrid(gridXMajorShown(), gridXMinorShown(), gridYMajorShown(), s); }

void PlotCanvas::setGridMajorLine(const PlotLinePtr line) {
    if(!line.null()) setGridMajorLine(*line);
    else             showGridMajor(false);
}
void PlotCanvas::setGridMajorLine(const String& color, PlotLine::Style style,
        double width) {
    PlotLinePtr line = gridMajorLine();
    line->setColor(color);
    line->setStyle(style);
    line->setWidth(width);
    setGridMajorLine(*line);
}

void PlotCanvas::setGridMinorLine(const PlotLinePtr line) {
    if(!line.null()) setGridMinorLine(*line);
    else             showGridMinor(false);
}

void PlotCanvas::setGridMinorLine(const String& color, PlotLine::Style style,
        double width) {
    PlotLinePtr line = gridMinorLine();
    line->setColor(color);
    line->setStyle(style);
    line->setWidth(width);
    setGridMinorLine(*line);
}


void PlotCanvas::setLegendLine(const PlotLinePtr line) {
    if(!line.null()) setLegendLine(*line);
    else             setLegendLine("000000", PlotLine::NOLINE);
}

void PlotCanvas::setLegendLine(const String& color, PlotLine::Style style,
        double width) {
    PlotLinePtr line = legendLine();
    line->setColor(color);
    line->setStyle(style);
    line->setWidth(width);
    setLegendLine(*line);
}

void PlotCanvas::setLegendFill(const PlotAreaFillPtr area) {
    if(!area.null()) setLegendFill(*area);
    else             setLegendFill("000000", PlotAreaFill::NOFILL);
}

void PlotCanvas::setLegendFill(const String& color,
        PlotAreaFill::Pattern pattern) {
    PlotAreaFillPtr fill = legendFill();
    fill->setColor(color);
    fill->setPattern(pattern);
    setLegendFill(*fill);
}

void PlotCanvas::setLegendFont(const PlotFontPtr font) {
    if(!font.null()) setLegendFont(*font); }


PlotOperationPtr PlotCanvas::operationExport()    {
    return operationExport(PlotMutexPtr()); 
}



PlotOperationPtr PlotCanvas::operationExport(PlotMutexPtr m) {
    if(m_exportOperation.null())
        m_exportOperation = new PlotOperation(OPERATION_EXPORT, mutex());
    if(!m.null()) m_exportOperation->setMutex(m);
    return m_exportOperation;
}

PlotRegion PlotCanvas::convertRegion(const PlotRegion& region,
        PlotCoordinate::System newSystem) const {
    PlotCoordinate upperLeft = convertCoordinate(region.upperLeft(),
                               newSystem);
    PlotCoordinate lowerRight = convertCoordinate(region.lowerRight(),
                                newSystem);
    return PlotRegion(upperLeft, lowerRight);
}


vector<double>
PlotCanvas::annotationWidthHeightDescent(const PlotAnnotationPtr annot) const {
    if(annot.null()) return vector<double>();
    else return textWidthHeightDescent(annot->text(), annot->font());
}

PlotMutexPtr PlotCanvas::mutex() const {
    PlotFactory* f = implementationFactory();
    PlotMutexPtr m = f->mutex();
    delete f;
    return m;
}

vector<vector<pair<unsigned int, unsigned int> > >*
PlotCanvas::locate(const PlotRegion& r) const {
    PlotRegion region = convertRegion(r, PlotCoordinate::WORLD);
    
    vector<PlotPtr> plots = allPlots();
    
    vector<vector<pair<unsigned int, unsigned int> > >* v =
        new vector<vector<pair<unsigned int, unsigned int> > >(plots.size());
    if(plots.size() == 0) return v;
    
    PlotPointDataPtr data;
    double xLeft = region.upperLeft().x(), xRight = region.lowerRight().x(),
           yTop = region.upperLeft().y(), yBottom = region.lowerRight().y();
    double x, y;
    vector<pair<unsigned int, unsigned int> >* v2;
    unsigned int rangeLow = 0;
    bool inRange;
    
    for(unsigned int i = 0; i < plots.size(); i++) {
        data = plots[i]->data();
        if(data.null()) continue; // shouldn't happen
        
        v2 = &v->at(i);
        inRange = false;
        for(unsigned int j = 0; j < data->size(); j++) {
            x = data->xAt(j);
            y = data->yAt(j);
            
            if(x >= xLeft && x <= xRight && y <= yTop && y >= yBottom) {
                if(!inRange) {
                    rangeLow = j;
                    inRange = true;
                }
            } else if(inRange) {
                v2->push_back(pair<unsigned int, unsigned int>(rangeLow,
                                                               j - 1));
                inRange = false;
            }
        }
        
        // catch last range
        if(inRange) {
            v2->push_back(pair<unsigned int, unsigned int>(rangeLow,
                                                           data->size() - 1));
        }
    }
    
    return v;
}

void PlotCanvas::locateAndLog(const PlotRegion& region,
        PlotLoggerPtr logger) const {
    if(logger.null()) return;
    vector<vector<pair<unsigned int, unsigned int> > >* res = locate(region);
    PlotLogLocate msg("PlotCanvas", "locate", region, res);
    logger->postMessage(msg);
}


void PlotCanvas::registerMouseTool(PlotMouseToolPtr tool, bool activate,
        bool blocking) {
    if(tool.null()) return;
    
    tool->setActive(activate);
    tool->setBlocking(blocking);
    
    for(unsigned int i = 0; i < m_mouseTools.size(); i++)
        if(m_mouseTools[i] == tool) return;
    
    m_mouseTools.push_back(tool);
    tool->attach(this);
}

vector<PlotMouseToolPtr> PlotCanvas::allMouseTools() const {
    return m_mouseTools; }

vector<PlotMouseToolPtr> PlotCanvas::activeMouseTools() const {
    vector<PlotMouseToolPtr> v;
    for(unsigned int i = 0; i < m_mouseTools.size(); i++)
        if(m_mouseTools[i]->isActive()) v.push_back(m_mouseTools[i]);
    return v;
}

void PlotCanvas::unregisterMouseTool(PlotMouseToolPtr tool) {
    if(tool.null()) return;
    for(unsigned int i = 0; i < m_mouseTools.size(); i++) {
        if(m_mouseTools[i] == tool) {
            m_mouseTools.erase(m_mouseTools.begin() + i);
            tool->detach();
            return;
        }
    }
}

PlotStandardMouseToolGroupPtr PlotCanvas::standardMouseTools() {
    if(m_standardTools.null()) {
        PlotFactory* f = implementationFactory();
        m_standardTools = f->standardMouseTools();
        delete f;
        registerMouseTool(m_standardTools, false, true);
    }
    return m_standardTools;
}


// Macro for the handlers because they got very repetitive.
#define PC_HANDLER1(TYPE,MEMBER)                                              \
void PlotCanvas::register##TYPE##Handler(Plot##TYPE##EventHandlerPtr handler, \
        PlotCoordinate::System system) {                                      \
    if(handler.null()) return;                                                \
    for(unsigned int i = 0; i < MEMBER .size(); i++)                          \
        if( MEMBER [i].first == handler) return;                              \
    MEMBER .push_back(pair<Plot##TYPE##EventHandlerPtr,                       \
            PlotCoordinate::System>(handler, system));                        \
}                                                                             \
                                                                              \
vector<Plot##TYPE##EventHandlerPtr> PlotCanvas::all##TYPE##Handlers() const { \
    vector<Plot##TYPE##EventHandlerPtr> v( MEMBER .size());                   \
    for(unsigned int i = 0; i < v.size(); i++) v[i] = MEMBER [i].first;       \
    return v;                                                                 \
}                                                                             \
                                                                              \
void PlotCanvas::unregister##TYPE##Handler(Plot##TYPE##EventHandlerPtr hndlr){\
    for(unsigned int i = 0; i < MEMBER .size(); i++) {                        \
        if( MEMBER [i].first == hndlr) {                                      \
            MEMBER .erase( MEMBER .begin() + i);                              \
            break;                                                            \
        }                                                                     \
    }                                                                         \
}

// Second macro which doesn't have the PlotCoordiate::System stuff.
#define PC_HANDLER2(TYPE,MEMBER)                                              \
void PlotCanvas::register##TYPE##Handler(Plot##TYPE##EventHandlerPtr handler){\
    if(handler.null()) return;                                                \
    for(unsigned int i = 0; i < MEMBER .size(); i++)                          \
        if( MEMBER [i] == handler) return;                                    \
    MEMBER .push_back(handler);                                               \
}                                                                             \
                                                                              \
vector<Plot##TYPE##EventHandlerPtr> PlotCanvas::all##TYPE##Handlers() const { \
    return MEMBER; }                                                          \
                                                                              \
void PlotCanvas::unregister##TYPE##Handler(Plot##TYPE##EventHandlerPtr hndlr){\
    for(unsigned int i = 0; i < MEMBER .size(); i++) {                        \
        if( MEMBER [i] == hndlr) {                                            \
            MEMBER .erase( MEMBER .begin() + i);                              \
            break;                                                            \
        }                                                                     \
    }                                                                         \
}

PC_HANDLER1(Select, m_selectHandlers)
PC_HANDLER1(Click, m_clickHandlers)
PC_HANDLER1(MousePress, m_pressHandlers)
PC_HANDLER1(MouseRelease, m_releaseHandlers)
PC_HANDLER1(MouseDrag, m_dragHandlers)
PC_HANDLER1(MouseMove, m_moveHandlers)
PC_HANDLER1(Wheel, m_wheelHandlers)
PC_HANDLER2(Key, m_keyHandlers)
PC_HANDLER2(Resize, m_resizeHandlers)


// Protected Methods //

void PlotCanvas::resetMouseTools() {
    for(unsigned int i = 0; i < m_mouseTools.size(); i++)
        m_mouseTools[i]->reset();
}

bool PlotCanvas::notifyDrawWatchers(PlotOperationPtr drawOperation,
        bool drawingIsThreaded, int drawnLayersFlag) {
    bool ret = true;
    for(unsigned int i = 0; i < m_drawWatchers.size(); i++)
        ret &= m_drawWatchers[i]->canvasDrawBeginning(drawOperation,
                drawingIsThreaded, drawnLayersFlag);
    return ret;
}

#define PC_EVENT_HELPER1(MEMBER)                                              \
    vector<PlotMouseToolPtr> active = activeMouseTools();                     \
    if( MEMBER .size() == 0 && active.size() == 0) return false;              \

#define PC_SELECT_HELPER                                                      \
    PlotRegion wreg = convertRegion(selectedRegion, PlotCoordinate::WORLD),   \
        nreg= convertRegion(selectedRegion, PlotCoordinate::NORMALIZED_WORLD),\
        preg= convertRegion(selectedRegion, PlotCoordinate::PIXEL);           \
    PlotSelectEvent pe(this, preg), we(this, wreg), ne(this, nreg);

#define PC_MOUSE_HELPER(EVENT)                                                \
    PlotCoordinate wc = convertCoordinate(coord, PlotCoordinate::WORLD),      \
        nc = convertCoordinate(coord, PlotCoordinate::NORMALIZED_WORLD),      \
        pc = convertCoordinate(wc, PlotCoordinate::PIXEL);                    \
    EVENT we(this, button, wc), ne(this, button, nc), pe(this, button, pc);   \

#define PC_WHEEL_HELPER                                                       \
    PlotCoordinate wc = convertCoordinate(coord, PlotCoordinate::WORLD),      \
        nc = convertCoordinate(coord, PlotCoordinate::NORMALIZED_WORLD),      \
        pc = convertCoordinate(wc, PlotCoordinate::PIXEL);                    \
    PlotWheelEvent we(this, delta, wc), ne(this, delta, nc),                  \
        pe(this, delta, pc);

#define PC_EVENT_HELPER2(MEMBER,TYPE)                                         \
    for(unsigned int i = 0; i < active.size(); i++) {                         \
        switch(active[i]->getCoordinateSystem()) {                            \
        case PlotCoordinate::WORLD: active[i]->handle##TYPE(we); break;       \
        case PlotCoordinate::PIXEL: active[i]->handle##TYPE(pe); break;       \
        case PlotCoordinate::NORMALIZED_WORLD:                                \
            active[i]->handle##TYPE(ne); break;                               \
        default: continue;                                                    \
        }                                                                     \
        if(active[i]->isBlocking()&& active[i]->lastEventWasHandled())        \
            return true;                                                      \
    }                                                                         \
                                                                              \
    for(unsigned int i = 0; i < MEMBER.size(); i++) {                         \
        if(MEMBER[i].second == PlotCoordinate::WORLD)                         \
            MEMBER[i].first->handle##TYPE(we);                                \
        else if(MEMBER[i].second == PlotCoordinate::PIXEL)                    \
            MEMBER[i].first->handle##TYPE(pe);                                \
        else if(MEMBER[i].second == PlotCoordinate::NORMALIZED_WORLD)         \
            MEMBER[i].first->handle##TYPE(ne);                                \
    }                                                                         \
    return true;

#define PC_SELECT(MEMBER,TYPE)                                                \
    PC_EVENT_HELPER1(MEMBER)                                                  \
    PC_SELECT_HELPER                                                          \
    PC_EVENT_HELPER2(MEMBER,TYPE)

#define PC_MOUSE(MEMBER,EVENT,TYPE)                                           \
    PC_EVENT_HELPER1(MEMBER)                                                  \
    PC_MOUSE_HELPER(EVENT)                                                    \
    PC_EVENT_HELPER2(MEMBER,TYPE)

#define PC_WHEEL(MEMBER,TYPE)                                                 \
    PC_EVENT_HELPER1(MEMBER)                                                  \
    PC_WHEEL_HELPER                                                           \
    PC_EVENT_HELPER2(MEMBER,TYPE)

bool PlotCanvas::notifySelectHandlers(const PlotRegion& selectedRegion) {
    PC_SELECT(m_selectHandlers,Select)
}

bool PlotCanvas::notifyClickHandlers(PlotMouseEvent::Button button,
        const PlotCoordinate& coord) {
    PC_MOUSE(m_clickHandlers,PlotClickEvent,Click)
}

bool PlotCanvas::notifyPressHandlers(PlotMouseEvent::Button button,
        const PlotCoordinate& coord) {
    PC_MOUSE(m_pressHandlers,PlotMousePressEvent,MousePress)
}

bool PlotCanvas::notifyReleaseHandlers(PlotMouseEvent::Button button,
        const PlotCoordinate& coord) {
    PC_MOUSE(m_releaseHandlers,PlotMouseReleaseEvent,MouseRelease)
}

bool PlotCanvas::notifyDragHandlers(PlotMouseEvent::Button button,
        const PlotCoordinate& coord) {
    PC_MOUSE(m_dragHandlers,PlotMouseDragEvent,MouseDrag)
}

bool PlotCanvas::notifyMoveHandlers(PlotMouseEvent::Button button,
        const PlotCoordinate& coord) {
    PC_MOUSE(m_moveHandlers,PlotMouseMoveEvent,MouseMove)
}

bool PlotCanvas::notifyWheelHandlers(int delta, const PlotCoordinate& coord) {
    PC_WHEEL(m_wheelHandlers,Wheel)
}

#define PC_EVENT(MEMBER,EVENT,TYPE,...)                                       \
    if(MEMBER.size() == 0) return false;                                      \
    EVENT ev(this, __VA_ARGS__);                                              \
    for(unsigned int i = 0; i < MEMBER.size(); i++)                           \
        MEMBER[i]->handle##TYPE(ev);                                          \
    return true;

bool PlotCanvas::notifyKeyHandlers(char key,
        const vector<PlotKeyEvent::Modifier>& modifiers) {
    PC_EVENT(m_keyHandlers,PlotKeyEvent,Key,key,modifiers)
}

bool PlotCanvas::notifyResizeHandlers(int oldWidth, int oldHeight,
        int newWidth, int newHeight) {
    PC_EVENT(m_resizeHandlers,PlotResizeEvent,Resize,oldWidth,oldHeight,
             newWidth,newHeight)
}

}
