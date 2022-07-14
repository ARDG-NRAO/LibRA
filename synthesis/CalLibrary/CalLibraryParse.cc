//# CalLibraryParse.cc: Class to hold results from cal library parser
//# Copyright (C) 1994,1995,1997,1998,1999,2000,2001,2003
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

#include <synthesis/CalLibrary/CalLibraryParse.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Logging/LogSink.h>

using namespace casacore;
namespace casa {

CalLibraryParse* CalLibraryParse::thisCalLibParser = 0x0;
//Record* CalLibraryParse::callibRec_;


CalLibraryParse::CalLibraryParse():
   callibRec_(NULL) {
       reset();
}

CalLibraryParse::~CalLibraryParse() {
    delete callibRec_;
}

void CalLibraryParse::reset() {
    //if (CalLibraryParse::callibRec_ != 0x0) delete CalLibraryParse::callibRec_;
    //CalLibraryParse::callibRec_ = 0x0;
    if (callibRec_ != NULL) delete callibRec_;
    callibRec_ = new Record();
    resetCaltable();
}

void CalLibraryParse::resetCaltable() {
    caltableName_ = "";
    mapName_ = "";
    mapList_.resize(0);
    calwt_ = true;
    resetParamRecord();
}

void CalLibraryParse::resetParamRecord() {
    paramRec_.define("field", "");
    paramRec_.define("intent", "");
    paramRec_.define("spw", "");
    paramRec_.define("obs", "");
    paramRec_.define("tinterp", "");
    paramRec_.define("finterp", "");
    paramRec_.define("reach", "");
    addDefaultMap("antmap");
    addDefaultMap("fldmap");
    addDefaultMap("obsmap");
    addDefaultMap("spwmap");
}

void CalLibraryParse::addDefaultMap(String key) {
    if (paramRec_.isDefined(key) && (paramRec_.dataType(key) == TpString))
        paramRec_.removeField(key);
    paramRec_.define(key, mapList_);
}

void CalLibraryParse::issueKeywordWarning(String key) {
    LogIO logIO;
    stringstream ss;
    ss << "Cal Library: undefined keyword " << key << " will be ignored.";
    logIO << ss.str() << LogIO::WARN << LogIO::POST;
}

void CalLibraryParse::addStringParam(String key, String val) {
    if (paramRec_.isDefined(key)) {
        if (paramRec_.dataType(key) == TpArrayInt) paramRec_.removeField(key);
        val.gsub("'", "\"");  // repl ' with " around field
        paramRec_.define(key, val);
    } else if (key == "caltable") {
        caltableName_ = val;
    } else {
        issueKeywordWarning(key);
    }
}

void CalLibraryParse::addBoolParam(String key, Bool val) {
    if (key == "calwt")
        calwt_ = val;
    else
        issueKeywordWarning(key);
}

void CalLibraryParse::addMapParam(String mapname, Int val) {
    if (paramRec_.isDefined(mapname)) {
        mapName_ = mapname;
        mapList_.resize(1);
        mapList_[0] = val;
    } else {
        issueKeywordWarning(mapname);
    }
}

void CalLibraryParse::addMapParam(Int val) {
    size_t mapsize = mapList_.size();
    mapList_.resize(mapsize + 1, true);
    mapList_[mapsize] = val;
}

void CalLibraryParse::addMap() {
    paramRec_.define(mapName_, mapList_);
    mapList_.resize(0);
}

void CalLibraryParse::addCaltable() {
    // triggered by endl
    if (!caltableName_.empty()) {
        uInt calIndex = 0;
        Record caltableRec;

        if (callibRec_->isDefined(caltableName_)) {
            caltableRec = callibRec_->asRecord(caltableName_);
            // Remove calwt and then add it as the last field
            Bool calwt = caltableRec.asBool("calwt");
            caltableRec.removeField("calwt"); // this has to be last!
            calIndex = caltableRec.nfields();
            caltableRec.defineRecord(String::toString(calIndex), paramRec_);
            caltableRec.define("calwt", calwt); // put it back in!
            callibRec_->defineRecord(caltableName_, caltableRec);
        } else {
            caltableRec.defineRecord(String::toString(0), paramRec_);
            caltableRec.define("calwt", calwt_);
            callibRec_->defineRecord(caltableName_, caltableRec);
        }
        resetCaltable(); // get ready for the next one!
    }
}

const Record* CalLibraryParse::record() {
    return callibRec_;
}

}
