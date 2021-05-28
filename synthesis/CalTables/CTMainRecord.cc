//# CTMainRecord.cc: Implementation of CTMainRecord.h
//# Copyright (C) 2011
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
//# $Id$
//----------------------------------------------------------------------------

#include <synthesis/CalTables/CTMainRecord.h>
#include <synthesis/CalTables/CTEnums.h>
#include <casa/Arrays.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

CTMainRecord::CTMainRecord() : itsRecord()
{
// Null constructor
// Output to private data:
//    itsRecord        Record          Underlying record object
//
};

//----------------------------------------------------------------------------

CTMainRecord::CTMainRecord (const Record& inpRec) : itsRecord (inpRec)
{
// Construct from an input record
// Input:
//    inpRec           Record          Input record
// Output to private data:
//    itsRecord        Record          Underlying record object
//
};

//----------------------------------------------------------------------------

const Record& CTMainRecord::record()
{
// Return as record
// Output:
//    record           const Record&   Return underlying record object
// Input from private data:
//    itsRecord        Record          Underlying record object
//
  return itsRecord;
};

//----------------------------------------------------------------------------

void CTMainRecord::addRec (const Record& newRec)
{
// Add to underlying record object
// Input:
//    newRec           Record          Record to be added
// Output to private data:
//    itsRecord        Record          Underlying record object
//
  itsRecord.merge (newRec, RecordInterface::OverwriteDuplicates);
};

//----------------------------------------------------------------------------

void CTMainRecord::defineTime (const Double& time)
{
// Define the TIME field value (expressed as a Double)
// Input:
//    time             const Double&         TIME value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (NCT::fieldName (NCT::TIME), time);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CTMainRecord::defineTime (const MEpoch& time)
{
// Define the TIME field value (expressed as an MEpoch)
// Input:
//    time             const MEpoch&         TIME value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (NCT::fieldName (NCT::TIME), time.getValue().get());
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CTMainRecord::defineInterval (const Double& interval)
{
// Define the INTERVAL field value
// Input:
//    interval         const Double&         INTERVAL value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (NCT::fieldName (NCT::INTERVAL), interval);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CTMainRecord::defineAntenna1 (const Int& antenna1)
{
// Define the ANTENNA1 field value
// Input:
//    antenna1         const Int&            ANTENNA1 value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (NCT::fieldName (NCT::ANTENNA1), antenna1);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CTMainRecord::defineAntenna2 (const Int& antenna2)
{
// Define the ANTENNA2 field value
// Input:
//    antenna2         const Int&            ANTENNA2 value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (NCT::fieldName (NCT::ANTENNA2), antenna2);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CTMainRecord::defineFieldId (const Int& fieldId)
{
// Define the FIELD_ID field value
// Input:
//    fieldId          const Int&            FIELD_ID value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (NCT::fieldName (NCT::FIELD_ID), fieldId);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CTMainRecord::defineSpwId (const Int& spwId)
{
// Define the SPECTRAL_WINDOW_ID field value
// Input:
//    spwId          const Int&            SPECTRAL_WINDOW_ID value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (NCT::fieldName (NCT::SPECTRAL_WINDOW_ID), spwId);
      addRec (newRec);
};

//----------------------------------------------------------------------------

void CTMainRecord::defineScanNo (const Int& scanNo)
{
// Define the SCAN_NUMBER field value
// Input:
//    scanNo           const Int&            SCAN_NUMBER value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (NCT::fieldName (NCT::SCAN_NUMBER), scanNo);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CTMainRecord::defineCParam (const Array<Complex>& param)
{
// Define the CPARAM field value
// Input:
//    param           const Array<Complex>&    cal solution param values
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define ("CPARAM", param);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CTMainRecord::defineFParam (const Array<Float>& param)
{
// Define the FPARAM field value
// Input:
//    param           const Array<Float>&    cal solution param values
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define ("FPARAM", param);
  addRec (newRec);
};


//----------------------------------------------------------------------------

void CTMainRecord::defineParamerr (const Array<Float>& paramerr)
{
// Define the PARAMERR field value
// Input:
//    paramerr         const Array<Float>&   cal solution param err values
// Output to private data:
//    itsRecord        Record                Underlying record object
//
   Record newRec;
   newRec.define ("PARAMERR", paramerr);
   addRec (newRec);
};

//----------------------------------------------------------------------------

void CTMainRecord::defineFlag (const Array<Bool>& flag)
{
// Define the FLAG field value
// Input:
//    flag             const Array<Bool>&   cal solution param err values
// Output to private data:
//    itsRecord        Record                Underlying record object
//
   Record newRec;
   newRec.define ("FLAG", flag);
   addRec (newRec);
};

//----------------------------------------------------------------------------

void CTMainRecord::defineSnr (const Array<Float>& snr)
{
// Define the SNR field value
// Input:
//    SNR             const Array<Float>&    cal solution snr values
// Output to private data:
//    itsRecord        Record                Underlying record object
//
   Record newRec;
   newRec.define ("SNR", snr);
   addRec (newRec);
};

//----------------------------------------------------------------------------

void CTMainRecord::defineWeight (const Array<Float>& weight)
{
// Define the Weight field value
// Input:
//    weight           const Array<Float>&  cal solution weight values
// Output to private data:
//    itsRecord        Record                Underlying record object
//
   Record newRec;
   newRec.define ("Weight", weight);
   addRec (newRec);
};

//----------------------------------------------------------------------------

void CTMainRecord::getTime (Double& time)
{
// Get the TIME field value
// Output:
//    time             Double&               TIME value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (NCT::fieldName (NCT::TIME), time);
};

//----------------------------------------------------------------------------

void CTMainRecord::getInterval (Double& interval)
{
// Get the INTERVAL field value
// Output:
//    interval         Double&               INTERVAL value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (NCT::fieldName (NCT::INTERVAL), interval);
};

//----------------------------------------------------------------------------

void CTMainRecord::getAntenna1 (Int& antenna1)
{
// Get the ANTENNA1 field value
// Output:
//    antenna1         Int&                  ANTENNA1 value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (NCT::fieldName (NCT::ANTENNA1), antenna1);
};

//----------------------------------------------------------------------------

void CTMainRecord::getAntenna2 (Int& antenna2)
{
// Get the ANTENNA2 field value
// Output:
//    antenna2         Int&                  ANTENNA2 value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (NCT::fieldName (NCT::ANTENNA2), antenna2);
};

//----------------------------------------------------------------------------

void CTMainRecord::getFieldId (Int& fieldId)
{
// Get the FIELD_ID field value
// Output:
//    fieldId          Int&                  FIELD_ID value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (NCT::fieldName (NCT::FIELD_ID), fieldId);
};

//----------------------------------------------------------------------------

void CTMainRecord::getSpwId (Int& spwId)
{
// Get the SPECTRAL_WINDOW_ID field value
// Output:
//    spwId          Int&                  SPECTRAL_WINDOW_ID value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (NCT::fieldName (NCT::SPECTRAL_WINDOW_ID), spwId);
};

//----------------------------------------------------------------------------

void CTMainRecord::getScanNo (Int& scanNo)
{
// Get the SCAN_NUMBER field value
// Output:
//    scanNo           Int&                  SCAN_NUMBER value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (NCT::fieldName (NCT::SCAN_NUMBER), scanNo);
};

//----------------------------------------------------------------------------

void CTMainRecord::getParam (Array<Complex>& param)
{
// Get the PARAM field value
// Output:
//    param           Array<Complex>&                  PARAM value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get ("PARAM", param);
};

//----------------------------------------------------------------------------

void CTMainRecord::getParamerr (Array<Float>& paramerr)
{
// Get the PARAMERR field value
// Output:
//    paramerr        Array<Float>&          paramerr value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get ("PARAMERR", paramerr);
};

//----------------------------------------------------------------------------

void CTMainRecord::getFlag (Array<Bool>& flag)
{
// Get the FLAG field value
// Output:
//    flag        Array<Bool>&          flag value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get ("FLAG", flag);
};

//----------------------------------------------------------------------------

void CTMainRecord::getSnr (Array<Float>& snr)
{
// Get the FLAG field value
// Output:
//    snr        Array<Float>&          snr value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get ("SNR", snr);
};

//----------------------------------------------------------------------------

void CTMainRecord::getWeight (Array<Float>& weight)
{
// Get the WEIGHT field value
// Output:
//    weight        Array<Float>&          weight value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get ("WEIGHT", weight);
};


} //# NAMESPACE CASA - END

