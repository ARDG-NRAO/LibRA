//# PlotLogger.cc: Classes to perform various logging actions for the plotter.
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
#include <graphics/GenericPlotter/PlotLogger.h>

#include <casacore/casa/Logging/LogFilter.h>
#include <casacore/casa/Logging/LogSink.h>
#include <casacore/casa/Logging/StreamLogSink.h>
#include <casacore/casa/OS/Memory.h>
#include <graphics/GenericPlotter/Plotter.h>

#include <fstream>
#include <ctime>

using namespace std;

using namespace casacore;
namespace casa {

////////////////////////////////
// PLOTLOGMESSAGE DEFINITIONS //
////////////////////////////////

// Static //

const int PlotLogMessage::DEFAULT_EVENT_TYPE = PlotLogger::MSG_INFO;


// Non-Static //

PlotLogMessage::PlotLogMessage(int eventType) :
        LogMessage(PlotLogger::EVENT_PRIORITY(eventType)),
        m_eventType(eventType) { }

PlotLogMessage::PlotLogMessage(const String& origin1, const String& origin2,
        int eventType) :
        LogMessage(LogOrigin(origin1, origin2),
                   PlotLogger::EVENT_PRIORITY(eventType)),
        m_eventType(eventType) { }

PlotLogMessage::PlotLogMessage(const String& origin1, const String& origin2,
        const String& message, int eventType) :
        LogMessage(message, LogOrigin(origin1, origin2),
                   PlotLogger::EVENT_PRIORITY(eventType)),
        m_eventType(eventType) { }

PlotLogMessage::PlotLogMessage(const PlotLogMessage& copy) : LogMessage(copy),
        m_eventType(copy.eventType()) { }

PlotLogMessage::~PlotLogMessage() { }

int PlotLogMessage::eventType() const { return m_eventType; }


////////////////////////////////////
// PLOTLOGMEASUREMENT DEFINITIONS //
////////////////////////////////////

// Static //

const PlotLogMeasurement::TimeUnit PlotLogMeasurement::DEFAULT_TIME_UNIT =
    SECOND;
const PlotLogMeasurement::MemoryUnit PlotLogMeasurement::DEFAULT_MEMORY_UNIT =
    KILOBYTE;

String PlotLogMeasurement::timeUnits(TimeUnit t) {
    switch(t) {
    case SECOND: return "seconds";
    
    default: return "";
    }
}

String PlotLogMeasurement::memoryUnits(MemoryUnit m) {
    switch(m) {
    case BYTE:     return "bytes";
    case KILOBYTE: return "kilobytes";
    case MEGABYTE: return "megabytes";
    
    default: return "";
    }
}


// Non-Static //

PlotLogMeasurement::PlotLogMeasurement(const String& origin1,
        const String& origin2, TimeUnit timeUnit, MemoryUnit memoryUnit,
        int eventType) : PlotLogMessage(origin1, origin2, eventType),
        m_time(-1), m_memory(0), m_timeUnit(timeUnit),m_memoryUnit(memoryUnit){
    startMeasurement();
}

PlotLogMeasurement::PlotLogMeasurement(const PlotLogMeasurement& copy) :
        PlotLogMessage(copy), m_startTime(copy.m_startTime),
        m_startMemory(copy.m_startMemory), m_time(copy.m_time),
        m_memory(copy.m_memory), m_timeUnit(copy.m_timeUnit),
        m_memoryUnit(copy.m_memoryUnit) { }

PlotLogMeasurement::~PlotLogMeasurement() { }


time_t PlotLogMeasurement::startTime() const { return m_startTime; }
unsigned int PlotLogMeasurement::startMemory() const { return m_startMemory; }
double PlotLogMeasurement::time() const { return m_time; }
double PlotLogMeasurement::memory() const { return m_memory; }
PlotLogMeasurement::TimeUnit PlotLogMeasurement::timeUnit() const {
    return m_timeUnit; }
PlotLogMeasurement::MemoryUnit PlotLogMeasurement::memoryUnit() const {
    return m_memoryUnit; }

void PlotLogMeasurement::startMeasurement() {
    m_startTime = std::time(NULL);
    m_startMemory = Memory::allocatedMemoryInBytes();
}

void PlotLogMeasurement::stopMeasurement() {
    // Get measurement values.
    time_t t = std::time(NULL);
    m_time = t - m_startTime;
    m_memory = ((double)Memory::allocatedMemoryInBytes()) - m_startMemory;
    if(m_memoryUnit == KILOBYTE)      m_memory /= 1024;
    else if(m_memoryUnit == MEGABYTE) m_memory /= 1024 * 1024;
    
    // Set string message.
    stringstream ss;
    ss << "END       Time: ";
    if(m_time >= 0) ss << "+" << m_time << " " << timeUnits(m_timeUnit);
    else ss << "unreported";
    ss << ".  Memory: ";
    if(m_memory >= 0) ss << "+";
    ss << m_memory << " " << memoryUnits(m_memoryUnit) << '.';    
    message(ss.str(), true);

    startMeasurement();
}


///////////////////////////////
// PLOTLOGLOCATE DEFINITIONS //
///////////////////////////////

// Static //

PlotLogLocate PlotLogLocate::canvasLocate(PlotCanvas* canv,
        const PlotRegion& reg) {
    if(canv == NULL)
        return PlotLogLocate("PlotLogLocate", "canvasLocate", reg, NULL);
    
    return PlotLogLocate("PlotLogLocate","canvasLocate",reg,canv->locate(reg));
}


// Non-Static //

PlotLogLocate::PlotLogLocate(const String& origin1, const String& origin2,
        const PlotRegion& locateRegion,
        vector<vector<pair<unsigned int,unsigned int> > >* locatedIndices,
        int eventType, bool deleteIndicesOnDestruction) :
        PlotLogMessage(origin1, origin2, eventType), m_region(locateRegion),
        m_indices(locatedIndices), m_shouldDelete(deleteIndicesOnDestruction) {
    // Generate message.
    stringstream ss;
    unsigned int np = numSearchedPlots(), ni = numLocatedIndices();
    ss << "Locating indices with x in [" << m_region.left() << ", "
       << m_region.right() << "] and y in [" << m_region.bottom() << ", "
       << m_region.top() << ".  Searched " << np << " plots and found " << ni
       << " indices.";
    if(np > 0 && ni > 0) ss << '\n';
    
    bool first = true;
    unsigned int n, from, to;
    for(unsigned int i = 0; i < np; i++) {
        n = m_indices->at(i).size();
        if(n > 0) {
            if(!first) ss << '\n';
            first = false;
        }
        if(np > 0 && n > 0) ss << "Plot " << i << ": ";
        if(n > 0) ss << "[";
        
        for(unsigned int j = 0; j < n; ++j) {
            from = (*m_indices)[i][j].first;
            to = (*m_indices)[i][j].second;
            ss << from;
            if(to != from) ss << "-" << to;
            if(j < n - 1) ss << ", ";
        }
        if(n > 0) ss << "]";
    }
    
    message(ss.str(), true);
}

PlotLogLocate::PlotLogLocate(const PlotLogLocate& copy) : PlotLogMessage(copy),
        m_region(copy.locateRegion()), m_indices(copy.indices()),
        m_shouldDelete(copy.willDeleteIndices()) {
    const_cast<PlotLogLocate&>(copy).m_shouldDelete = false;
}

PlotLogLocate::~PlotLogLocate() {
    if(m_shouldDelete && m_indices != NULL) delete m_indices;
}


const PlotRegion& PlotLogLocate::locateRegion() const { return m_region; }
unsigned int PlotLogLocate::numLocatedIndices() const {
    if(m_indices == NULL) return 0;
    unsigned int n = 0;
    for(unsigned int i = 0; i < m_indices->size(); i++)
        for(unsigned int j = 0; j < m_indices->at(i).size(); j++)
            n += (*m_indices)[i][j].second - (*m_indices)[i][j].first + 1;

    return n;
}

unsigned int PlotLogLocate::numSearchedPlots() const {
    return (m_indices != NULL) ? m_indices->size() : 0; }

vector<vector<pair<unsigned int, unsigned int> > >*
PlotLogLocate::indices() const { return m_indices; }

vector<pair<unsigned int, unsigned int> >*
PlotLogLocate::plotIndices(unsigned int index) const {
    if(m_indices == NULL || index >= m_indices->size()) return NULL;
    else return &(*m_indices)[index];
}

bool PlotLogLocate::willDeleteIndices() const { return m_shouldDelete; }


///////////////////////////////
// PLOTLOGMETHOD DEFINITIONS //
///////////////////////////////

PlotLogMethod::PlotLogMethod(const String& className, const String& methodName,
        bool entering, const String& message, int eventType) :
        PlotLogMessage(className, methodName, eventType) {
    stringstream ss;
    if(entering) ss << "ENTERING.";
    else         ss << "EXITING. ";    
    if(!message.empty()) ss << "  " << message;
    
    PlotLogMessage::message(ss.str(), true);
}

PlotLogMethod::~PlotLogMethod() { }


///////////////////////////////
// PLOTLOGOBJECT DEFINITIONS //
///////////////////////////////

PlotLogObject::PlotLogObject(const String& className, void* address,
        bool creation, const String& message, int eventType) :
        PlotLogMessage(className, creation ? "alloc" : "dealloc", eventType) {
    stringstream ss;
    if(creation) ss << "Creating";
    else         ss << "Destroying";
    ss << " object at " << address << ".";    
    if(!message.empty()) ss << " " << message;
    
    PlotLogMessage::message(ss.str(), true);
}

PlotLogObject::~PlotLogObject() { }


//////////////////////////////////
// PLOTLOGGERFILTER DEFINITIONS //
//////////////////////////////////

PlotLoggerFilter::PlotLoggerFilter(int eventFlags,
        LogMessage::Priority minPriority) : m_eventFlags(eventFlags),
        m_minPriority(minPriority) { }

PlotLoggerFilter::~PlotLoggerFilter() { }

LogFilterInterface* PlotLoggerFilter::clone() const {
    return new PlotLoggerFilter(m_eventFlags, m_minPriority); }

Bool PlotLoggerFilter::pass(const LogMessage& message) const {
    if(message.priority() < m_minPriority) return false;
    
    /*
     * This causes an error when a non-PlotLogMessage is sent into the filter.
     * Cannot dynamic cast because LogMessage is not polymorphic. :(
     * Instead, moved this functionality to PlotLogger::postMessage()...
    try {
        int type = ((const PlotLogMessage&)message).eventType();
        return type <= 0 || m_eventFlags & type;
    } catch(...) { return true; }
    */
    return true;
}

int PlotLoggerFilter::eventFlags() const { return m_eventFlags; }
void PlotLoggerFilter::setEventFlags(int flags) { m_eventFlags = flags; }

LogMessage::Priority PlotLoggerFilter::minimumPriority() const {
    return m_minPriority; }
void PlotLoggerFilter::setMinimumPriority(LogMessage::Priority minPriority) {
    m_minPriority = minPriority; }


////////////////////////////
// PLOTLOGGER DEFINITIONS //
////////////////////////////

// Static //

int PlotLogger::ALL_EVENTS_FLAG() {
    int flag = 0;
    vector<int> v = ALL_EVENTS();
    for(unsigned int i = 0; i < v.size(); i++) flag |= v[i];
    return flag;
}

vector<int> PlotLogger::ALL_EVENTS() {
    vector<int> v(6 + EXTENDED_TYPES.size());
    v[0] = MSG_DEBUG; v[1] = DRAW_TOTAL;
    v[2] = DRAW_INDIVIDUAL; v[3] = METHODS_MAJOR;
    v[4] = OBJECTS_MAJOR; v[5] = EXPORT_TOTAL;
    for(unsigned int i = 6; i < v.size(); i++)
        v[i] = EXTENDED_TYPES[i - 6];
    return v;
}

int PlotLogger::REGISTER_EVENT_TYPE(const String& name,
        LogMessage::Priority priority) {
    static int value = EXPORT_TOTAL;
    value *= 2;
    EXTENDED_TYPES.push_back(value);
    EXTENDED_NAMES.push_back(name);
    SET_EVENT_PRIORITY(value, priority);
    return value;
}

void PlotLogger::UNREGISTER_EVENT_TYPE(int event) {
    for(unsigned int i = 0; i < EXTENDED_TYPES.size(); i++) {
        if(event == EXTENDED_TYPES[i]) {
            EXTENDED_TYPES.erase(EXTENDED_TYPES.begin() + i);
            EXTENDED_NAMES.erase(EXTENDED_NAMES.begin() + i);
        }
    }
}

void PlotLogger::UNREGISTER_EVENT_TYPE(const String& name) {
    for(unsigned int i = 0; i < EXTENDED_TYPES.size(); i++) {
        if(name == EXTENDED_NAMES[i]) {
            EXTENDED_TYPES.erase(EXTENDED_TYPES.begin() + i);
            EXTENDED_NAMES.erase(EXTENDED_NAMES.begin() + i);
        }
    }
}

vector<String> PlotLogger::EVENT_NAMES() {
    vector<int> e = ALL_EVENTS();
    vector<String> v(e.size());
    for(unsigned int i = 0; i < v.size(); i++) v[i] = EVENT(e[i]);
    return v;
}

String PlotLogger::EVENT(int type) {
    if(type == MSG_INFO)             return "MSG_INFO";
    else if(type == MSG_WARN)        return "MSG_WARN";
    else if(type == MSG_ERROR)       return "MSG_ERROR";
    else if(type == MSG_DEBUG)       return "MSG_DEBUG";
    else if(type == DRAW_TOTAL)      return "DRAW_TOTAL";
    else if(type == DRAW_INDIVIDUAL) return "DRAW_INDIVIDUAL";
    else if(type == METHODS_MAJOR)   return "METHODS_MAJOR";
    else if(type == OBJECTS_MAJOR)   return "OBJECTS_MAJOR";
    else if(type == EXPORT_TOTAL)    return "EXPORT_TOTAL";
    else {
        for(unsigned int i = 0; i < EXTENDED_TYPES.size(); i++)
            if(type == EXTENDED_TYPES[i]) return EXTENDED_NAMES[i];
        return "";
    }
}

int PlotLogger::EVENT(const String& name) {
    if(name == EVENT(MSG_INFO))             return MSG_INFO;
    else if(name == EVENT(MSG_WARN))        return MSG_WARN;
    else if(name == EVENT(MSG_ERROR))       return MSG_ERROR;
    else if(name == EVENT(MSG_DEBUG))       return MSG_DEBUG;
    else if(name == EVENT(DRAW_TOTAL))      return DRAW_TOTAL;
    else if(name == EVENT(DRAW_INDIVIDUAL)) return DRAW_INDIVIDUAL;
    else if(name == EVENT(METHODS_MAJOR))   return METHODS_MAJOR;
    else if(name == EVENT(OBJECTS_MAJOR))   return OBJECTS_MAJOR;
    else if(name == EVENT(EXPORT_TOTAL))    return EXPORT_TOTAL;
    else {
        for(unsigned int i = 0; i < EXTENDED_NAMES.size(); i++)
            if(name == EXTENDED_NAMES[i]) return EXTENDED_TYPES[i];
        return NO_EVENTS;
    }
}

int PlotLogger::FLAG_FROM_EVENTS(const vector<int>& events) {
    int flag = 0;
    for(unsigned int i = 0; i < events.size(); i++) flag |= events[i];
    return flag;
}

int PlotLogger::FLAG_FROM_EVENTS(const vector<String>& names) {
    int flag = 0;
    for(unsigned int i = 0; i < names.size(); i++) flag |= EVENT(names[i]);
    return flag;
}

int PlotLogger::FLAG_FROM_PRIORITY(LogMessage::Priority minPriority) {
    int flag = 0;
    vector<int> v = ALL_EVENTS();
    for(unsigned int i = 0; i < v.size(); i++)
        if(EVENT_PRIORITY(v[i]) >= minPriority) flag |= v[i];
    return flag;
}

LogMessage::Priority PlotLogger::EVENT_PRIORITY(int event) {
    if(EVENT_PRIORITIES.find(event) == EVENT_PRIORITIES.end()) {
        LogMessage::Priority p = LogMessage::NORMAL;
        if(event == MSG_DEBUG || event == METHODS_MAJOR ||
           event == OBJECTS_MAJOR)        p = LogMessage::DEBUGGING;
        else if(event == DRAW_INDIVIDUAL) p = LogMessage::NORMAL5;
        else if(event == MSG_WARN)        p = LogMessage::WARN;
        else if(event == MSG_ERROR)       p = LogMessage::SEVERE;
        EVENT_PRIORITIES[event] = p;
    }
    return EVENT_PRIORITIES[event];
}

void PlotLogger::SET_EVENT_PRIORITY(int event, LogMessage::Priority priority) {
    EVENT_PRIORITIES[event] = priority; }


// Subclass of LogFilterInterface that refuses all messages.  Used to disable
// the global log sink temporarily.
// <group>
class NullLogFilter : public LogFilterInterface {
public:
    NullLogFilter() { }
  NullLogFilter(const NullLogFilter& copy): 
    LogFilterInterface()
    { (void)copy; }
    ~NullLogFilter() { }
    LogFilterInterface* clone()   const { 
            return new NullLogFilter(); 
            }
    Bool pass(const LogMessage& message) const   {
            (void)message;
            return false; 
            }
};
// </group>


void PlotLogger::disableGlobalSink() {
    if(DISABLED_GLOBAL_FILTER == NULL) {
        DISABLED_GLOBAL_FILTER = LogSink::globalSink().filter().clone();
        LogSink::globalSink().filter(NullLogFilter());
    }
}

void PlotLogger::enableGlobalSink() {
    if(DISABLED_GLOBAL_FILTER != NULL) {
        LogSink::globalSink().filter(*DISABLED_GLOBAL_FILTER);
        delete DISABLED_GLOBAL_FILTER;
        DISABLED_GLOBAL_FILTER = NULL;
    }
}


vector<int> PlotLogger::EXTENDED_TYPES = vector<int>();

vector<String> PlotLogger::EXTENDED_NAMES = vector<String>();

map<int, LogMessage::Priority> PlotLogger::EVENT_PRIORITIES =
    map<int, LogMessage::Priority>();

LogFilterInterface* PlotLogger::DISABLED_GLOBAL_FILTER = NULL;


// Non-Static //

PlotLogger::PlotLogger(Plotter*, int filterEventFlags,
        LogMessage::Priority filterMinPriority) : 
        m_logger(&LogSink::globalSink(), false),
        m_filter(filterEventFlags, filterMinPriority) {
    m_logger->filter(m_filter);
}

PlotLogger::~PlotLogger() { }

CountedPtr<LogSinkInterface> PlotLogger::sink() { return m_logger; }
const CountedPtr<LogSinkInterface> PlotLogger::sink() const {
    return m_logger; }

LogSinkInterface* PlotLogger::localSinkCopy() const {
    if(m_loggerLocation.empty()) return &LogSink::globalSink();
    else if(m_logger.null()) return NULL;
    else {
        const StreamLogSink* s= dynamic_cast<const StreamLogSink*>(&*m_logger);
        if(s == NULL) return const_cast<LogSinkInterface*>(&*m_logger);
        else return new StreamLogSink(*s);
    }
}

const String& PlotLogger::sinkLocation() const { return m_loggerLocation; }
void PlotLogger::setSinkLocation(const String& logFile) {
    CountedPtr<LogSinkInterface> oldSink = m_logger;
    try {
        if(logFile.empty())
            m_logger = CountedPtr<LogSinkInterface>(&LogSink::globalSink(),
                       false);
        else
            m_logger = new StreamLogSink(LogMessage::NORMAL,
                       new ofstream(logFile.c_str(), ios::app), true);
        m_logger->filter(m_filter);
        m_loggerLocation = logFile;
    } catch(...) {
        m_logger = oldSink;
    }
}


LogMessage::Priority PlotLogger::filterMinPriority() const {
    return m_filter.minimumPriority(); }
void PlotLogger::setFilterMinPriority(PlotLogMessage::Priority minPriority) {
    if(minPriority != m_filter.minimumPriority()) {
        m_filter.setMinimumPriority(minPriority);
        if(!m_logger.null()) m_logger->filter(m_filter);
    }
}

bool PlotLogger::filterEventFlag(int flag) const {
    return flag <= 0 || flag & m_filter.eventFlags(); }
void PlotLogger::setFilterEventFlag(int flag, bool on) {
    int flags = m_filter.eventFlags();    
    if(on && !(flags & flag)) setFilterEventFlags(flags | flag);
    else if(!on && (flags & flag)) setFilterEventFlags(flags & ~flag);
}

int PlotLogger::filterEventFlags() const { return m_filter.eventFlags(); }
void PlotLogger::setFilterEventFlags(int flags) {
    if(flags != m_filter.eventFlags()) {
        m_filter.setEventFlags(flags);
        if(!m_logger.null()) m_logger->filter(m_filter);
    }
}

void PlotLogger::postMessage(const PlotLogMessage& message) {
    try {
        if(!m_logger.null()) {
            // Do events type check here.
            int type = message.eventType();
            if(type <= 0 || m_filter.eventFlags() & type)
                m_logger->postLocally(message);
        }
    } catch(...) { }
}

void PlotLogger::postMessage(const String& origin1, const String& origin2,
        const String& message, int eventType) {
    postMessage(PlotLogMessage(origin1, origin2, message, eventType)); }

PlotLogMessage PlotLogger::markMeasurement(const String& origin1,
        const String& origin2, int eventType, bool postStartMessage) {
    PlotLogMessage startMessage(origin1, origin2,
            "START     Current memory usage: " +
            String::toString(Memory::allocatedMemoryInBytes()/1024.0) + " " +
            PlotLogMeasurement::memoryUnits(PlotLogMeasurement::KILOBYTE)+".",
            eventType);
    if(postStartMessage) postMessage(startMessage);
    m_measurements.push_back(PlotLogMeasurement(origin1, origin2,
            PlotLogMeasurement::DEFAULT_TIME_UNIT,
            PlotLogMeasurement::DEFAULT_MEMORY_UNIT, eventType));
    return startMessage;
}

PlotLogMeasurement PlotLogger::releaseMeasurement(bool postReleaseMessage) {
    if(m_measurements.size() == 0) // invalid
        return PlotLogMeasurement("PlotLogger", "releaseMeasurement");
    
    // Go through and update previous times.    
    PlotLogMeasurement m = m_measurements[m_measurements.size() - 1];
    m_measurements.erase(m_measurements.begin() + m_measurements.size() - 1);
    m.stopMeasurement();
    if(postReleaseMessage) postMessage(m);
    
    return m;
}

PlotLogLocate PlotLogger::locate(PlotCanvas* canvas, const PlotRegion& region,
            int eventType, bool postLocateMessage) {
    if(canvas == NULL) return PlotLogLocate("", "", region, NULL);
    
    vector<vector<pair<unsigned int, unsigned int> > >* res = canvas->locate(
            region);
    PlotLogLocate msg("PlotCanvas", "locate", region, res, eventType, false);
    if(postLocateMessage) postMessage(msg);
    return msg;
}

}
