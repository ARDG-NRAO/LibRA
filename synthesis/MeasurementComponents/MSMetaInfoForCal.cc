//# MSMetaInfoForCal.cc: Definition of MSMetaInfoForCal
//# Copyright (C) 2016
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
//#


#include <casacore/ms/MSOper/MSMetaData.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <synthesis/MeasurementComponents/MSMetaInfoForCal.h>
#include <msvis/MSVis/SimpleSimVi2.h>
#include <casa/aips.h>
#include <casa/iostream.h>

using namespace casacore;

namespace casa { //# NAMESPACE CASA - BEGIN

using namespace vi;
  
// Constructor
MSMetaInfoForCal::MSMetaInfoForCal(String msname) : 
  msname_(msname), 
  msOk_(False),
  nAnt_(4),
  nSpw_(1),
  nFld_(1),
  centerFreqs_(0),
  ms_(NULL),
  msmd_(NULL),
  ssp_(NULL)
{

  // If the specified MS is available, make non-trivial meta-info accessors
  if (Table::isReadable(msname_) &&
      Table::tableInfo(msname_).type()=="Measurement Set") {

    ms_ = new MeasurementSet(msname_);

    // 95% of the size of an Int column, or 50 MB (whichever smaller)
    //  This ensures that smallish time-to-Id maps will get cached
    //   (instead of one-off column info needed to calculate them)
    //  This is appropriate for the the calibration context
    Float msmdcache=0.95*4.0f*Float(ms_->nrow())/1e6;
    msmdcache=min(msmdcache,50.0);
    msmd_ = new MSMetaData(ms_,msmdcache);

    // MS seems to be available
    msOk_=True;
    
    // Fill from msmd
    nAnt_=msmd_->nAntennas();
    nSpw_=msmd_->nSpw(True);
    nFld_=msmd_->nFields();

  }
  // else, String output will be spoofed

}

// Construct from a supplied MS object
MSMetaInfoForCal::MSMetaInfoForCal(const MeasurementSet& ms) :
  msname_(ms.tableName()), 
  msOk_(True),      // A good MS was supplied, presumably...
  nAnt_(0),
  nSpw_(0),
  nFld_(0),
  centerFreqs_(0),
  ms_(NULL),        // ... but we won't have our own MS pointer
  msmd_(new MSMetaData(&ms,  // Form MSMetaData directly (not more than 50MB)
		       min(50.0,0.95f*4.0f*Float(ms.nrow())/1e6))),
  ssp_(NULL)
{
  // Fill counters from msmd
  nAnt_=msmd_->nAntennas();
  nSpw_=msmd_->nSpw(True);
  nFld_=msmd_->nFields();

}

MSMetaInfoForCal::MSMetaInfoForCal(uInt nAnt,uInt nSpw,uInt nFld) : 
  msname_("<noms>"), 
  msOk_(False),
  nAnt_(nAnt),
  nSpw_(nSpw),
  nFld_(nFld),
  centerFreqs_(0),
  ms_(NULL),
  msmd_(NULL),
  ssp_(NULL)
{
  // Nothing to do
}


// Construct from SimpleSimVi2Paremeters
//   (useful for testing w/ actual (spoofed) data iteration
MSMetaInfoForCal::MSMetaInfoForCal(const vi::SimpleSimVi2Parameters& sspar) :
  msname_("<noms>"), 
  msOk_(False),
  nAnt_(sspar.nAnt_),
  nSpw_(sspar.nSpw_),
  nFld_(sspar.nField_),
  centerFreqs_(0),
  ms_(NULL),
  msmd_(NULL),
  ssp_(new SimpleSimVi2Parameters(sspar))  // a copy
{
  // Nothing to do
}



// Destructor
MSMetaInfoForCal::~MSMetaInfoForCal()
{
  if (msmd_)
    delete msmd_;
  if (ms_)
    delete ms_;
  if (ssp_)
    delete ssp_;
}

// Return access to MSMetaData object (if avail)
MSMetaData& MSMetaInfoForCal::msmd() const {

  if (msOk_ && msmd_)
    return *msmd_;
  else
    throw(AipsError("MSMetaDataForCal::msmd():  No MSMetaData object available!"));
}

const SimpleSimVi2Parameters& MSMetaInfoForCal::ssp() const {
  if (ssp_)
    return *ssp_;
  else
    throw(AipsError("MSMetaDataForCal::ssp():  No SimpleSimVi2Parameters object available!"));
}

// Antenna name, by index
String MSMetaInfoForCal::antennaName(uInt iant) const {
  if (msOk_ && msmd_) {
    std::map<String, uInt> mymap;
    vector<uInt> ids(1);
    ids[0]=iant;
    uInt nAnt=msmd_->nAntennas();
    if (iant < nAnt)
      return msmd_->getAntennaNames(mymap,ids)[0];
    else {
      throw(AipsError(
		      "Specified iant="+
		      String::toString(iant)+
		      " > nAnt="+
		      String::toString(nAnt)));
    }
  }
  else {
    if (iant < nAnt_)
      return "AntennaId"+String::toString(iant);
    else {
      throw(AipsError(
		      "Specified iant="+
		      String::toString(iant)+
		      " > nAnt="+
		      String::toString(nAnt_)));
    }
  }
}

// Antenna names list, by index
void MSMetaInfoForCal::antennaNames(Vector<String>& antnames) const {
  if (msOk_ && msmd_) {
    std::map<String, uInt> mymap;
    Vector<String> asV(msmd_->getAntennaNames(mymap));
    antnames.reference(asV);
  }
  else {
    antnames.resize(nAnt_);
    for (uInt iant=0;iant<nAnt_;++iant)
      antnames[iant]="AntennaId"+String::toString(iant);
  }
}

// Field name, by index
String MSMetaInfoForCal::spwName(uInt ispw) const {

  if (msOk_ && msmd_) {
    uInt nSpw=msmd_->nSpw(True);
    if (ispw<nSpw)
      return msmd_->getSpwNames()[ispw];
    else {
      throw(AipsError(
		      "Specified ispw="+
		      String::toString(ispw)+
		      " > nSpw="+
		      String::toString(nSpw)));
    }
  }
  else
    if (ispw<nSpw_)
      return "SpwId"+String::toString(ispw);
    else {
      throw(AipsError(
		      "Specified ispw="+
		      String::toString(ispw)+
		      " > nSpw="+
		      String::toString(nSpw_)));
    }

}

// Field name, by index
String MSMetaInfoForCal::fieldName(uInt ifld) const {

  if (msOk_ && msmd_) {
    vector<uInt> ids(1);
    ids[0]=ifld;
    uInt nFld=msmd_->nFields();
    if (ifld<nFld)
      return msmd_->getFieldNamesForFieldIDs(ids)[0];
    else {
      throw(AipsError(
		      "Specified ifld="+
		      String::toString(ifld)+
		      " > nFld="+
		      String::toString(nFld)));
    }
  }
  else
    if (ifld<nFld_)
      return "FieldId"+String::toString(ifld);
    else {
      throw(AipsError(
		      "Specified ifld="+
		      String::toString(ifld)+
		      " > nFld="+
		      String::toString(nFld_)));
    }

}

void MSMetaInfoForCal::fieldNames(Vector<String>& fldnames) const {
  if (msOk_ && msmd_) {
    Vector<String> asV(msmd_->getFieldNames());
    fldnames.reference(asV);
  }
  else {
    fldnames.resize(nFld_);
    for (uInt ifld=0;ifld<nFld_;++ifld)
      fldnames[ifld]="FieldId"+String::toString(ifld);
  }
}

// Field id at time
Int MSMetaInfoForCal::fieldIdAtTime(Double time) const {
  if (msOk_ && msmd_) {
    // Assume 0.1s is adequate time resolution (may need to revisit?)
    return *(msmd_->getFieldsForTimes(time,0.1).begin());
  }
  else
    return -1;  // no information, essentially
}

// Scan number at time
Int MSMetaInfoForCal::scanNumberAtTime(Double time) const {
  if (msOk_ && msmd_) {
    // Assume 0.1s is adequate time resolution (may need to revisit?)
    return *(msmd_->getScansForTimes(time,0.1,-1,-1).begin());
  }
  else
    return -1;  // no information, essentially

}

Double MSMetaInfoForCal::centerFreq(uInt ispw) const {
  
  // If centerFreqs_ not yet filled, do it
  if (centerFreqs_.nelements()==0) {
    if (msOk_ && msmd_) {
      centerFreqs_.resize(nSpw());
      vector<Quantity> centerFreqQ=msmd_->getCenterFreqs();
      for (uInt ispw=0;ispw<nSpw();++ispw) {
	centerFreqs_[ispw]=centerFreqQ[ispw].getValue("GHz");
      }
    }
  }

  // Return value for requested spw, if available
  if ( ispw<centerFreqs_.nelements() ) {
    return centerFreqs_[ispw];
  }
  else {
    return 0.0f;

  }
}


} //# NAMESPACE CASA - END
