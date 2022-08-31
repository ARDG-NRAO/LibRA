//# PlotLogger.h: Classes to perform various logging actions for the plotter.
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
#ifndef PLOTLOGGER_H_
#define PLOTLOGGER_H_

#include <graphics/GenericPlotter/PlotOptions.h>

#include <casacore/casa/Logging/LogSinkInterface.h>
#include <casacore/casa/Utilities/CountedPtr.h>

#include <map>
#include <time.h>
#include <vector>

namespace casa {

//# Forward declarations
class PlotCanvas;
class Plotter;


// Superclass for all messages going through the PlotLogger.  This class has
// been refactored to just be a thin layer on top of LogMessage.
class PlotLogMessage : public casacore::LogMessage {
public:
    // Static //
    
    // Default event type.
    static const int DEFAULT_EVENT_TYPE;
    
    
    // Non-Static //
    
    // Constructor which takes an optional priority.
    PlotLogMessage(int eventType = DEFAULT_EVENT_TYPE);
    
    // Constructor which takes the origin(s) and an optional priority.
    PlotLogMessage(const casacore::String& origin1, const casacore::String& origin2,
            int eventType = DEFAULT_EVENT_TYPE);
    
    // Constructor which takes the origin(s), the message, and an optional
    // priority.
    PlotLogMessage(const casacore::String& origin1, const casacore::String& origin2,
            const casacore::String& message, int eventType = DEFAULT_EVENT_TYPE);
    
    // Copy constructor.
    PlotLogMessage(const PlotLogMessage& copy);
    
    // Destructor.
    virtual ~PlotLogMessage();
    
    
    // Returns the event type of this message.
    virtual int eventType() const;
    
protected:
    // Event type, either a value from PlotLogger::Event, or a custom
    // user-defined value.
    int m_eventType;
};


// Used to report time and memory measurements.  This functionality can be
// accessed either directly with a PlotLogMeasurement object or indirectly
// through the PlotLogger class.  Message is:
// END.\tTime: [time] [timeUnits].  casacore::Memory: [memory] [memoryUnits].
// If the measurement has not been ended, calls stopMeasurement() first.
class PlotLogMeasurement : public PlotLogMessage {
public:
    // Static //
    
    // Available time units.  Currently only seconds because that's all that
    // C++ natively supports.
    enum TimeUnit {
        SECOND
    };
    
    // Available memory units.
    enum MemoryUnit {
        BYTE, KILOBYTE, MEGABYTE
    };
    
    // Default units.
    // <group>
    static const TimeUnit DEFAULT_TIME_UNIT;
    static const MemoryUnit DEFAULT_MEMORY_UNIT;
    // </group>
    
    // Get a string representation of the given time/memory unit.
    // <group>
    static casacore::String timeUnits(TimeUnit t);   
    static casacore::String memoryUnits(MemoryUnit m);
    // </group>
    
    
    // Non-Static //
    
    // Constructor which takes the origin(s), optional time and memory
    // units, and an optional priority.  Also calls startMeasurement().
    PlotLogMeasurement(const casacore::String& origin1, const casacore::String& origin2,
                       TimeUnit timeUnit = DEFAULT_TIME_UNIT,
                       MemoryUnit memoryUnit = DEFAULT_MEMORY_UNIT,
                       int eventType = DEFAULT_EVENT_TYPE);
    
    // Copy constructor.
    PlotLogMeasurement(const PlotLogMeasurement& copy);
    
    // Destructor.
    ~PlotLogMeasurement();
    
    
    // Returns the time/memory when the measurement started.
    // <group>
    time_t startTime() const;
    unsigned int startMemory() const;
    // </group>
    
    // Returns the time/memory difference between when the measurement started
    // and when the measurement ended.  Invalid if the measurement was never
    // started and ended.
    // <group>
    double time() const;    
    double memory() const;
    // </group>
    
    // Returns the time/memory units for this measurement.
    // <group>
    TimeUnit timeUnit() const;
    MemoryUnit memoryUnit() const;
    // </group>
   
    // Starts the measurement by setting the start time and memory.
    // Measurement automatically begins when the object is constructed, but
    // can be restarted as desired.
    void startMeasurement();
    
    // Calculates the measurements from the last starting point to this point,
    // and generates the log message.
    void stopMeasurement();
    
private:
    // Start time
    time_t m_startTime;
    
    // Start memory
    unsigned int m_startMemory;
    
    // casacore::Time and memory differences
    double m_time, m_memory;
    
    // casacore::Time unit
    TimeUnit m_timeUnit;
    
    // casacore::Memory unit
    MemoryUnit m_memoryUnit;
};


// Used to report located indices.  Basically just a container for the results
// of a PlotCanvas::locate.
class PlotLogLocate : public PlotLogMessage {
public:
    // Static //
    
    // Convenience access to PlotCanvas::locate() to return a PlotLogLocate
    // message.
    static PlotLogLocate canvasLocate(PlotCanvas* canvas,
            const PlotRegion& region);
    
    
    // Non-Static //
    
    // Constructor which takes the two origins, the region that was located,
    // and the located indices (see PlotCanvas::locate()).  If
    // deleteIndicesOnDestruction is true, the given indices vector will be
    // deleted when this message is destroyed.  This should be used with care
    // if multiple PlotLogLocates using the same located indices are being
    // used.
    PlotLogLocate(const casacore::String& origin1, const casacore::String& origin2,
            const PlotRegion& locateRegion,
            std::vector<std::vector<std::pair<unsigned int,unsigned int> > >* locatedIndices,
            int eventType = DEFAULT_EVENT_TYPE,
            bool deleteIndicesOnDestruction = true);
    
    // Copy constructor.  NOTE: will set the deleteIndicesOnDestruction flag on
    // the original to false so that the indices won't be deleted from out
    // under the copy.
    PlotLogLocate(const PlotLogLocate& copy);
    
    // Destructor.
    ~PlotLogLocate();
    
    
    // Returns the region that was located.
    const PlotRegion& locateRegion() const;
    
    // Returns the total number of points that were located.
    unsigned int numLocatedIndices() const;
    
    // Returns the number of plots that were searched.
    unsigned int numSearchedPlots() const;
    
    // Returns the located indices.
    std::vector<std::vector<std::pair<unsigned int, unsigned int> > >* indices() const;
    
    // Returns the located indices for the plot at the given index, or NULL for
    // invalid.
    std::vector<std::pair<unsigned int, unsigned int> >* plotIndices(
            unsigned int index) const;
    
    // Returns whether or not this message will delete the indices vector on
    // destruction or not.
    bool willDeleteIndices() const;
    
private:
    // Region.
    PlotRegion m_region;
    
    // Indices.
    std::vector<std::vector<std::pair<unsigned int, unsigned int> > >* m_indices;
    
    // Should delete indices.
    bool m_shouldDelete;
};


// Subclass of PlotLogMessage to unify messages for method entering/exiting.
class PlotLogMethod : public PlotLogMessage {
public:
    // Constructor which takes the class and method names, a flag for whether
    // the method is entering or exiting, and an optional additional message
    // and priority.
    PlotLogMethod(const casacore::String& className, const casacore::String& methodName,
            bool entering, const casacore::String& message = casacore::String(),
            int eventType = DEFAULT_EVENT_TYPE);
    
    // Destructor.
    ~PlotLogMethod();
};


// Subclass of PlotLogMessage to unify messages for object creation/deletion.
class PlotLogObject : public PlotLogMessage {
public:
    // Constructor which takes the class name and object address, a flag for
    // whether the object is being created or destroyed, and an optional
    // additional message and priority.
    PlotLogObject(const casacore::String& className, void* address, bool creation,
            const casacore::String& message = casacore::String(),
            int eventType = DEFAULT_EVENT_TYPE);
    
    // Destructor.
    ~PlotLogObject();
};


// Subclass of casacore::LogFilterInterface to filter on both event flags and minimum
// priority.
class PlotLoggerFilter : public casacore::LogFilterInterface {
public:
    // Constructor which takes optional event flags and minimum priority.
    PlotLoggerFilter(int eventFlags, casacore::LogMessage::Priority minPriority);
    
    // Destructor.
    ~PlotLoggerFilter();
    
    // Implements casacore::LogFilterInterface::clone().
    casacore::LogFilterInterface* clone() const;
    
    // Implements casacore::LogFilterInterface::pass().
    casacore::Bool pass(const casacore::LogMessage& message) const;
    
    // Gets/Sets the event flags.
    // <group>
    int eventFlags() const;
    void setEventFlags(int flags);
    // </group>
    
    // Gets/Sets the minimum priority.
    // <group>
    casacore::LogMessage::Priority minimumPriority() const;
    void setMinimumPriority(casacore::LogMessage::Priority minPriority);
    // </group>
    
private:
    // Event flags.
    int m_eventFlags;
    
    // Minimum priority.
    casacore::LogMessage::Priority m_minPriority;
};


// A PlotLogger is used to log messages to an underlying CASA log object, as
// well as provide access to different logging functionality like measurements.
// PlotLogger is associated with a single Plotter object and should be used by
// all children of that Plotter (canvases, plot items, etc.) to report their
// behavior if the proper flag is turned on.  The logger can also filter out
// messages by priority.  Therefore a message must BOTH have its event type
// flag turned on, AND meet the priority minimum in order to be posted to the
// log.  The exception to this is for the MSG_* event types, which are logged
// as long as they meet the filtered priority requirement.  The actual log can
// either be the global sink, or can be set to a file.
class PlotLogger {
public:
    // Static //
    
    // Event types //
    
    // Event types that are always allowed.
    // <group>
    static const int MSG_INFO  = -1;
    static const int MSG_WARN  = -2;
    static const int MSG_ERROR = -3;
    // </group>
    
    // Miscellaneous debugging messages.
    static const int MSG_DEBUG       = 1;
    
    // Replotting/redrawing the whole GUI.
    static const int DRAW_TOTAL      = 2;
    
    // Replotting/redrawing each plot item.
    static const int DRAW_INDIVIDUAL = 4;
    
    // Entering/exiting major methods.
    static const int METHODS_MAJOR   = 8;
    
    // Creation/deletion of major objects.
    static const int OBJECTS_MAJOR   = 16;
    
    // Exporting canvases to file.
    static const int EXPORT_TOTAL    = 32;
    
    
    // No events.
    static const int NO_EVENTS = 0;
    
    // All events as a flag.
    static int ALL_EVENTS_FLAG();
    
    // All events as a vector.
    static std::vector<int> ALL_EVENTS();
    
    
    // Registers an extended event type with the given name and optional
    // priority and returns its value.
    static int REGISTER_EVENT_TYPE(const casacore::String& name,
            casacore::LogMessage::Priority priority = casacore::LogMessage::NORMAL);
    
    // Unregisters the given extended event type.  If a priority has been set,
    // it is NOT removed.
    // <group>
    static void UNREGISTER_EVENT_TYPE(int event);
    static void UNREGISTER_EVENT_TYPE(const casacore::String& name);
    // </group>
    
    // Returns all the event names.
    static std::vector<casacore::String> EVENT_NAMES();
    
    // Converts between an event type and its name.
    // <group>
    static casacore::String EVENT(int type);
    static int EVENT(const casacore::String& name);
    // </group>
    
    // Returns an event flag from the given vector.
    // <group>
    static int FLAG_FROM_EVENTS(const std::vector<int>& events);
    static int FLAG_FROM_EVENTS(const std::vector<casacore::String>& names);
    // </group>
    
    // Returns an event flag for all events that meet the given minimum
    // priority.
    static int FLAG_FROM_PRIORITY(casacore::LogMessage::Priority minPriority);
    
    
    // Gets/Sets the message priority for the given log event.  Uses a default
    // if the event has not been set.
    // <group>
    static casacore::LogMessage::Priority EVENT_PRIORITY(int event);
    static void SET_EVENT_PRIORITY(int event, casacore::LogMessage::Priority priority);
    // </group>
    
    
    // Disables the global sink until enableGlobalSink() is called.  Can be
    // used when something posts undesirably to both the local and global logs.
    static void disableGlobalSink();
    
    // Re-enables the global sink.  See disableGlobalSink().
    static void enableGlobalSink();
    
    
    // Non-Static //
    
    // Constructor which takes the Plotter this logger is associated with.  The
    // global log sink is used, and the minimum priority filter is set.
    PlotLogger(Plotter* plotter, int filterEventFlags = NO_EVENTS,
            casacore::LogMessage::Priority filterMinPriority = casacore::LogMessage::DEBUGGING);
    
    // Destructor.
    virtual ~PlotLogger();
    
    
    // Log IO Methods //
    
    // Gets the log sink interface.
    // <group>
    casacore::CountedPtr<casacore::LogSinkInterface> sink();
    const casacore::CountedPtr<casacore::LogSinkInterface> sink() const;
    // </group>
    
    // Gets a copy of the log sink interface, IF it is not the global.
    casacore::LogSinkInterface* localSinkCopy() const;
    
    // Gets/Sets the log sink file location.  If the filename is empty, it
    // means the global sink.
    // <group>
    const casacore::String& sinkLocation() const;
    void setSinkLocation(const casacore::String& logFile);
    // </group>
    
    // Returns true if the logger is currently using the global sink, false
    // otherwise.
    bool usingGlobalSink() const { return sinkLocation().empty(); }
    
    
    // Filtering Methods //
    
    // Gets/Sets the log filter priority level.
    // <group>
    casacore::LogMessage::Priority filterMinPriority() const;
    void setFilterMinPriority(PlotLogMessage::Priority minPriority);
    // </group>
    
    // Gets/Sets the log filter event flag for a single event type.
    // <group>
    bool filterEventFlag(int flag) const;
    void setFilterEventFlag(int flag, bool on);
    // </group>
    
    // Gets/Sets the log filter event flag(s).  The flag should be a bitwise-or
    // of one or more values in PlotLogger events and any extended event types.
    // <group>
    int filterEventFlags() const;
    void setFilterEventFlags(int flags);
    // <group>
    
    
    // Message Methods //
    
    // Posts the given message to the underlying log sink.
    // <group>
    void postMessage(const PlotLogMessage& message);
    void postMessage(const casacore::String& origin1, const casacore::String& origin2,
                     const casacore::String& message,
                     int eventType = PlotLogMessage::DEFAULT_EVENT_TYPE);
    // </group>
    
    
    // Measurement Methods //
    
    // Marks the logger to begin a time/memory measurement.  Measurement marks
    // can be recursive.  Returns a generic message saying that measurement has
    // begun, which will be also posted to the log if postStartMessage is true.
    PlotLogMessage markMeasurement(const casacore::String& origin1,const casacore::String& origin2,
            int eventType = PlotLogMessage::DEFAULT_EVENT_TYPE,
            bool postStartMessage = true);
    
    // Gets the measurement since the last mark.  The message will also be
    // posted to the log if postReleaseMessage is true.
    PlotLogMeasurement releaseMeasurement(bool postReleaseMessage = true);
    
    
    // Locate Methods //
    
    // Calls locate on the given canvas and returns the result as a message
    // that will also be posted if postLocateMessage is true.
    PlotLogLocate locate(PlotCanvas* canvas, const PlotRegion& region,
            int eventType = MSG_INFO, bool postLocateMessage = true);
    
private:
    // Log sink.
    casacore::CountedPtr<casacore::LogSinkInterface> m_logger;
    
    // Log filter.
    PlotLoggerFilter m_filter;
    
    // Log sink location.
    casacore::String m_loggerLocation;
    
    // Current measurement marks.
    std::vector<PlotLogMeasurement> m_measurements;

    
    // Static //
    
    // Registered extended types.
    static std::vector<int> EXTENDED_TYPES;
    
    // Registered extended type names.
    static std::vector<casacore::String> EXTENDED_NAMES;
    
    // casacore::Map from log event to priority.
    static std::map<int, casacore::LogMessage::Priority> EVENT_PRIORITIES;
    
    // Disabled old global filter, or null.
    static casacore::LogFilterInterface* DISABLED_GLOBAL_FILTER;
};
typedef casacore::CountedPtr<PlotLogger> PlotLoggerPtr;

}

#endif /* PLOTLOGGER_H_ */
