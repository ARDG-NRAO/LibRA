//# SolvableVisCalMetaInfo.h: Definitions of interface for SolvableVisCalMetaInfo
//# Copyright (C) 1996,1997,2000,2001,2002,2003
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
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$

#ifndef CALTABLES_CALSETMETAINFO_H
#define CALTABLES_CALSETMETAINFO_H

#include <casacore/casa/aips.h>
#include <casacore/ms/MeasurementSets/MSAntennaColumns.h>
#include <casacore/ms/MeasurementSets/MSAntenna.h>
#include <casacore/ms/MeasurementSets/MSFieldColumns.h>
#include <casacore/ms/MeasurementSets/MSField.h>
#include <casacore/ms/MeasurementSets/MSSpWindowColumns.h>
#include <casacore/ms/MeasurementSets/MSSpectralWindow.h>

// #include <casa/Logging/LogMessage.h>
// #include <casa/Logging/LogSink.h>
// #include <casa/Logging/LogIO.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// **********************************************************
//  CalSetMetaInfo
//

class CalSetMetaInfo {
public:

  CalSetMetaInfo():
    antTableName("ANTENNA"), fieldTableName("FIELD"), spwTableName("SPECTRAL_WINDOW"),
    calTableMSAC(NULL), calTableMSFC(NULL), calTableMSSpC(NULL),
    calAntTable(NULL), calFieldTable(NULL), calSpwTable(NULL)  
  {rootName="";};

  CalSetMetaInfo(const casacore::String& root);
  
  ~CalSetMetaInfo()   {cleanUp();};

  void setRootName(const casacore::String& root) {rootName=root;};
  casacore::String fullSubTableName(casacore::String& subTableName,const casacore::String& base){
    return casacore::Table(base).rwKeywordSet().asTable(subTableName).tableName();};
  casacore::String fullSubTableName(casacore::String& subTableName){
    return fullSubTableName(subTableName,rootName);};

  void reset(const casacore::String& root);
  casacore::String getAntName(casacore::Int ant);
  casacore::String getFieldName(casacore::Int field);
  casacore::String getSpwName(casacore::Int spw);
  casacore::Double  getSpwRefFreq(casacore::Int spw);
  // casacore::Vector<casacore::Double>  getSpwChanWidth(casacore::Int spw);
  casacore::Double  getSpwTotalBandwidth(casacore::Int spw);

  casacore::Vector<casacore::String> getFieldNames();
  casacore::Vector<casacore::String> getAntNames();
  casacore::Vector<casacore::String> getSpwNames();
  casacore::Vector<casacore::Double> getSpwRefFreqs();
  casacore::Bool verify(CalSetMetaInfo& otherCSMI);

  void copyMetaTables(const casacore::String& target);

protected:

  void cleanUp()
  {
    if (calTableMSAC  != NULL) {delete calTableMSAC;  calTableMSAC  = NULL;}
    if (calTableMSFC  != NULL) {delete calTableMSFC;  calTableMSFC  = NULL;}
    if (calTableMSSpC != NULL) {delete calTableMSSpC; calTableMSSpC = NULL;}

    if (calAntTable   != NULL) {delete calAntTable;   calAntTable   = NULL;}
    if (calFieldTable != NULL) {delete calFieldTable; calFieldTable = NULL;}
    if (calSpwTable   != NULL) {delete calSpwTable;   calSpwTable   = NULL;}
  }
private:
  casacore::String rootName, antTableName, fieldTableName, spwTableName;
  casacore::MSAntennaColumns *calTableMSAC;
  casacore::MSFieldColumns *calTableMSFC;
  casacore::MSSpWindowColumns *calTableMSSpC;
  casacore::MSAntenna *calAntTable;
  casacore::MSField *calFieldTable;
  casacore::MSSpectralWindow *calSpwTable;
};




} //# NAMESPACE CASA - END

#endif
