//# PlotOperation.cc: Classes for managing large, threaded plot operations.
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
#include <graphics/GenericPlotter/PlotOperation.h>

using namespace casacore;
namespace casa {

///////////////////////////////
// PLOTOPERATION DEFINITIONS //
///////////////////////////////

PlotOperation::PlotOperation(const String& name, PlotMutexPtr mutex) :
        m_name(name), m_cancelRequested(false), m_mutex(mutex) {
    reset();
}

PlotOperation::~PlotOperation() { }


#define PO_GETTER(TYPE, NAME)                                                 \
    m_mutex->lock();                                                          \
    TYPE temp = NAME;                                                         \
    m_mutex->unlock();                                                        \
    return temp;

String PlotOperation::name() const { PO_GETTER(String, m_name) } 
bool PlotOperation::inProgress() const { PO_GETTER(bool, m_inProgress) }
bool PlotOperation::isFinished() const { PO_GETTER(bool, m_isFinished) }
unsigned int PlotOperation::currentProgress() const {
    PO_GETTER(unsigned int, m_currentProgress) }
String PlotOperation::currentStatus() const {
    PO_GETTER(String, m_currentStatus) }
bool PlotOperation::cancelRequested() const {
    PO_GETTER(bool, m_cancelRequested) }


#define PO_SETTER(NAME, TEMP, VAL)                                            \
    if( TEMP != VAL ) {                                                       \
        m_mutex->lock();                                                      \
        NAME = VAL;                                                           \
        m_mutex->unlock();                                                    \
        notifyWatchers();                                                     \
    }

void PlotOperation::setInProgress(bool val) {
    PO_SETTER(m_inProgress, inProgress(), val) }
void PlotOperation::setIsFinished(bool val) {
    PO_SETTER(m_isFinished, isFinished(), val) }
void PlotOperation::setCurrentProgress(unsigned int val) {
    if(val > 100) val = 100;
    PO_SETTER(m_currentProgress, currentProgress(), val)
}
void PlotOperation::setCurrentStatus(const String& val) {
    PO_SETTER(m_currentStatus, currentStatus(), val) }
void PlotOperation::setCancelRequested(bool val) {
    PO_SETTER(m_cancelRequested, cancelRequested(), val); }

void PlotOperation::setMutex(PlotMutexPtr mutex) { m_mutex = mutex; }

void PlotOperation::addWatcher(PlotOperationWatcher* watcher) {
    if(watcher == NULL) return;
    for(unsigned int i = 0; i < m_watchers.size(); i++)
        if(m_watchers[i] == watcher) return;
    m_watchers.push_back(watcher);
}

void PlotOperation::removeWatcher(PlotOperationWatcher* watcher) {
    if(watcher == NULL) return;
    for(unsigned int i = 0; i < m_watchers.size(); i++) {
        if(m_watchers[i] == watcher) {
            m_watchers.erase(m_watchers.begin() + i);
            return;
        }
    }
}

void PlotOperation::reset() {
    m_inProgress = false;
    m_isFinished = false;
    m_currentProgress = 0;
    m_currentStatus = "";
    m_cancelRequested = false;
    notifyWatchers();
}

void PlotOperation::finish() {
    m_inProgress = false;
    m_isFinished = true;
    m_currentProgress = 100;
    m_currentStatus = "Finished.";
    notifyWatchers();
}


void PlotOperation::notifyWatchers() const {
    for(unsigned int i = 0; i < m_watchers.size(); i++)
        m_watchers[i]->operationChanged(*this);
}

}
