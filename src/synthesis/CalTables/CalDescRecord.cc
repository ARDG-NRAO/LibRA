//# CalDescRecord.cc: Implementation of CalDescRecord.h
//# Copyright (C) 1996,1997,1998,2003
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

#include <synthesis/CalTables/CalDescRecord.h>
#include <msvis/MSVis/MSCalEnums.h>
#include <casacore/casa/Arrays.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

CalDescRecord::CalDescRecord() : itsRecord()
{
// Null constructor
// Output to private data:
//    itsRecord        Record          Underlying record object
//
};

//----------------------------------------------------------------------------

CalDescRecord::CalDescRecord (const Record& inpRec) : itsRecord (inpRec)
{
// Construct from an input record
// Input:
//    inpRec           Record          Input record
// Output to private data:
//    itsRecord        Record          Underlying record object
//
};

//----------------------------------------------------------------------------

const Record& CalDescRecord::record()
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

void CalDescRecord::addRec (const Record& newRec)
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

void CalDescRecord::defineNumSpw (const Int& numSpw)
{
// Define the NUM_SPW field value
// Input:
//    numSpw           const Int&            NUM_SPW value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::NUM_SPW), numSpw);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalDescRecord::defineNumChan (const Array <Int>& numChan)
{
// Define the NUM_CHAN field value
// Input:
//    numChan          const Array <Int>&    NUM_CHAN value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::NUM_CHAN), numChan);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalDescRecord::defineNumReceptors (const Int& numReceptors)
{
// Define the NUM_RECEPTORS field value
// Input:
//    numReceptors     const Int&            NUM_RECEPTORS value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::NUM_RECEPTORS), numReceptors);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalDescRecord::defineNJones (const Int& nJones)
{
// Define the N_JONES field value
// Input:
//    nJones           const Int&            N_JONES value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::N_JONES), nJones);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalDescRecord::defineSpwId (const Array <Int>& spwId)
{
// Define the SPECTRAL_WINDOW_ID field value
// Input:
//    spwId            const Array <Int>&    SPECTRAL_WINDOW_ID value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::SPECTRAL_WINDOW_ID), spwId);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalDescRecord::defineChanFreq (const Array <Double>& chanFreq)
{
// Define the CHAN_FREQ field value
// Input:
//    chanFreq         const Array <Double>&   CHAN_FREQ value
// Output to private data:
//    itsRecord        Record                  Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::CHAN_FREQ), chanFreq);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalDescRecord::defineChanWidth (const Array <Double>& chanWidth)
{
// Define the CHAN_WIDTH field value
// Input:
//    chanWidth        const Array <Double>&   CHAN_WIDTH value
// Output to private data:
//    itsRecord        Record                  Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::CHAN_WIDTH), chanWidth);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalDescRecord::defineChanRange (const Array <Int>& chanRange)
{
// Define the CHAN_RANGE field value
// Input:
//    chanRange        const Array <Int>&      CHAN_RANGE value
// Output to private data:
//    itsRecord        Record                  Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::CHAN_RANGE), chanRange);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalDescRecord::definePolznType (const Array <String>& polznType)
{
// Define the POLARIZATION_TYPE field value
// Input:
//    polznType        const Array <String>&   POLARIZATION_TYPE value
// Output to private data:
//    itsRecord        Record                  Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::POLARIZATION_TYPE), polznType);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalDescRecord::defineJonesType (const String& jonesType)
{
// Define the JONES_TYPE field value
// Input:
//    jonesType        const String&         JONES_TYPE value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::JONES_TYPE), jonesType);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalDescRecord::defineMSName (const String& msName)
{
// Define the MS_NAME field value
// Input:
//    msName           const String&         MS_NAME value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::MS_NAME), msName);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalDescRecord::getNumSpw (Int& numSpw)
{
// Get the NUM_SPW field value
// Output:
//    numSpw           Int&                  NUM_SPW value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::NUM_SPW), numSpw);
};

//----------------------------------------------------------------------------

void CalDescRecord::getNumChan (Array <Int>& numChan)
{
// Get the NUM_CHAN field value
// Output:
//    numChan          Array <Int>&          NUM_CHAN value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::NUM_CHAN), numChan);
};

//----------------------------------------------------------------------------

void CalDescRecord::getNumReceptors (Int& numReceptors)
{
// Get the NUM_RECEPTORS field value
// Output:
//    numReceptors     Int&                  NUM_RECEPTORS value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::NUM_RECEPTORS), numReceptors);
};

//----------------------------------------------------------------------------

void CalDescRecord::getNJones (Int& nJones)
{
// Get the N_JONES field value
// Output:
//    nJones           Int&                  N_JONES value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::N_JONES), nJones);
};

//----------------------------------------------------------------------------

void CalDescRecord::getSpwId (Array <Int>& spwId)
{
// Get the SPECTRAL_WINDOW_ID field value
// Output:
//    spwId            Array <Int>&          SPECTRAL_WINDOW_ID value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::SPECTRAL_WINDOW_ID), spwId);
};

//----------------------------------------------------------------------------

void CalDescRecord::getChanFreq (Array <Double>& chanFreq)
{
// Get the CHAN_FREQ field value
// Output:
//    chanFreq         Array <Double>&       CHAN_FREQ value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::CHAN_FREQ), chanFreq);
};

//----------------------------------------------------------------------------

void CalDescRecord::getChanWidth (Array <Double>& chanWidth)
{
// Get the CHAN_WIDTH field value
// Output:
//    chanWidth        Array <Double>&       CHAN_WIDTH value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::CHAN_WIDTH), chanWidth);
};

//----------------------------------------------------------------------------

void CalDescRecord::getChanRange (Array <Int>& chanRange)
{
// Get the CHAN_RANGE field value
// Output:
//    chanRange        Array <Int>&          CHAN_RANGE value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::CHAN_RANGE), chanRange);
};

//----------------------------------------------------------------------------

void CalDescRecord::getPolznType (Array <String>& polznType)
{
// Get the POLARIZATION_TYPE field value
// Output:
//    polznType        Array <String>&       POLARIZATION_TYPE value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::POLARIZATION_TYPE), polznType);
};

//----------------------------------------------------------------------------

void CalDescRecord::getJonesType (String& jonesType)
{
// Get the JONES_TYPE field value
// Output:
//    jonesType        String&               JONES_TYPE value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::JONES_TYPE), jonesType);
};

//----------------------------------------------------------------------------

void CalDescRecord::getMSName (String& msName)
{
// Get the MS_NAME field value
// Output:
//    msName           String&               MS_NAME value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::MS_NAME), msName);
};

//----------------------------------------------------------------------------

} //# NAMESPACE CASA - END

