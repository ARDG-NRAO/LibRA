//# CalSetMetaInfo.cc: Implementation of CalSetMetaInfo classes
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
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

#include <synthesis/CalTables/CalSetMetaInfo.h>

#include <msvis/MSVis/VisBuffer.h>

#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/ArrayIter.h>
#include <casacore/scimath/Mathematics/MatrixMathLA.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/Quanta/MVTime.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/OS/Memory.h>
#include <casacore/casa/Utilities/GenSort.h>
#include <casacore/casa/Quanta/Quantum.h>
#include <casacore/casa/Quanta/QuantumHolder.h>
#include <casacore/ms/MeasurementSets/MSAntennaColumns.h>
#include <casacore/ms/MeasurementSets/MSFieldColumns.h>
#include <casacore/ms/MeasurementSets/MSSpWindowColumns.h>
#include <sstream>
#include <iostream>
#include <iomanip>

#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogSink.h>

#include <fstream>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN
  //
  //----------------------------------------------------------------
  //
  CalSetMetaInfo::CalSetMetaInfo(const String& root):
    rootName(root),
    antTableName("ANTENNA"), 
    fieldTableName("FIELD"), 
    spwTableName("SPECTRAL_WINDOW"),
    calTableMSAC(NULL), calTableMSFC(NULL), calTableMSSpC(NULL),
    calAntTable(NULL), calFieldTable(NULL), calSpwTable(NULL)
  {
    antTableName   = "ANTENNA";
    fieldTableName = "FIELD";
    spwTableName   = "SPECTRAL_WINDOW";
    reset(root);
  }

  void CalSetMetaInfo::reset(const String& root)
  {
    cleanUp();
    setRootName(root);

    calAntTable = new MSAntenna(fullSubTableName(antTableName));
    calTableMSAC = new MSAntennaColumns(*calAntTable);

    calFieldTable = new MSField(fullSubTableName(fieldTableName));
    calTableMSFC = new MSFieldColumns(*calFieldTable);

    calSpwTable = new MSSpectralWindow(fullSubTableName(spwTableName));
    calTableMSSpC = new MSSpWindowColumns(*calSpwTable);
  }
  //
  //----------------------------------------------------------------
  //
  String CalSetMetaInfo::getAntName(Int ant)
  {
    return (*calTableMSAC).name().getColumn()(ant);
  }
  //
  //----------------------------------------------------------------
  //
  Vector<String> CalSetMetaInfo::getAntNames()
  {
    return Vector<String>((*calTableMSAC).name().getColumn());
  }
  //
  //----------------------------------------------------------------
  //
  String CalSetMetaInfo::getFieldName(Int field)
  {
    return (*calTableMSFC).name().getColumn()(field);
  }
  //
  //----------------------------------------------------------------
  //
  Vector<String> CalSetMetaInfo::getFieldNames()
  {
    return Vector<String>((*calTableMSFC).name().getColumn());
  }
  //
  //----------------------------------------------------------------
  //
  String CalSetMetaInfo::getSpwName(Int spw)
  {
	return (*calTableMSSpC).name().getColumn()(spw);
  }
  //
  //----------------------------------------------------------------
  //
  Vector<String> CalSetMetaInfo::getSpwNames()
  {
    return Vector<String>((*calTableMSSpC).name().getColumn());
  }
  //
  //----------------------------------------------------------------
  //
  Double CalSetMetaInfo::getSpwRefFreq(Int spw)
  {
    return (*calTableMSSpC).refFrequency().getColumn()(spw);
  }
  //
  //----------------------------------------------------------------
  //
  Vector<Double> CalSetMetaInfo::getSpwRefFreqs()
  {
    return Vector<Double>((*calTableMSSpC).refFrequency().getColumn());
  }
  //
  //----------------------------------------------------------------
  //
  /*
  Vector<Double> CalSetMetaInfo::getSpwChanWidth(Int spw)
  {
    return (*calTableMSSpC).chanWidth().getColumn()(spw);
  }
  */
  //
  //----------------------------------------------------------------
  //
  Double CalSetMetaInfo::getSpwTotalBandwidth(Int spw)
  {
    return (*calTableMSSpC).totalBandwidth().getColumn()(spw);
  }
  //
  //----------------------------------------------------------------
  //
  void CalSetMetaInfo::copyMetaTables(const String& target)
  {
    {
      String name(target+"/ANTENNA");
      MSAntenna antTable(fullSubTableName(antTableName));
      antTable.deepCopy(fullSubTableName(antTableName,target), Table::New);
    }
    {
      String name(target+"/FIELD");
      MSField fieldTable(fullSubTableName(fieldTableName));
      fieldTable.deepCopy(fullSubTableName(fieldTableName,target), Table::New);
    }
    {
      String name(target+"/SPECTRAL_WINDOW");
      MSSpectralWindow spwTable(fullSubTableName(spwTableName));
      spwTable.deepCopy(fullSubTableName(spwTableName,target), Table::New);
    }
  }
  //
  //----------------------------------------------------------------
  //
  // Verify that the entires in the meta-info tables of the CalSet are 
  // identical to those in the meta-info tables of the otherCSMI (typically
  // of the MS being processed).
  // 
  // This requirement for the entires to be identical is too
  // restrictive (I (SB) can think of simple real-life cases where
  // this restriction will show up).  However, this is a good first
  // step.
  //
  Bool CalSetMetaInfo::verify(CalSetMetaInfo& otherCSMI)
  {
    Bool spwVerified=false, antennaVerified=false, fieldVerified=false;
    ostringstream spwMesg, antMesg, fieldMesg;
    // Verify that the entires in the SPW table are identical
    {
      Vector<String> thisSpwNames=getSpwNames(),
	otherSpwNames=otherCSMI.getSpwNames();
      Vector<Double> thisSpwRefFreq=getSpwRefFreqs(),
	otherSpwRefFreq(otherCSMI.getSpwRefFreqs());

      uInt n=thisSpwNames.nelements();
      spwVerified = (n <= otherSpwNames.nelements());
      uInt i=0;
      if (spwVerified)
	for(i=0;i<n;i++)
	  {
	    if (thisSpwNames(i) != otherSpwNames(i))
	      {spwVerified = false;break;}
	    if (thisSpwRefFreq(i) != otherSpwRefFreq(i))
	      {spwVerified = false;break;}
	  }
      if (!spwVerified)
	spwMesg << "CalSetMetaInfo::verify: "
	        << "Spectral Window " << i << " is not identical in the CalTable and the MS.";
    }
    // Verify that the entires in the ANTENNA table are identical
    {
      Vector<String> thisAntNames=getAntNames(),
	otherAntNames=otherCSMI.getAntNames();
      uInt n=thisAntNames.nelements();
      antennaVerified = (n <= otherAntNames.nelements());
      uInt i=0;
      if (antennaVerified)
	for(i=0;i<n;i++)
	  {
	    if (thisAntNames(i) != otherAntNames(i))
	      {antennaVerified = false;break;}
	  }
      if (!antennaVerified)
	  antMesg << "CalSetMetaInfo::verify: "
		  << "Antenna " << i << " is not identical in the CalTable and the MS.";
    }
    // Verify that the entires in the FIELD table are identical
    {
      Vector<String> thisFieldNames=getFieldNames(),
	otherFieldNames=otherCSMI.getFieldNames();
      uInt n=thisFieldNames.nelements();
      fieldVerified = (n <= otherFieldNames.nelements());
      uInt i = 0;
      if (fieldVerified)
	for(i=0;i<n;i++)
	  {
	    if (thisFieldNames(i) != otherFieldNames(i))
	      {fieldVerified = false;break;}
	  }
      if (!fieldVerified)
	  fieldMesg << "CalSetMetaInfo::verify: "
		    << "Field " << i << " is not identical in the CalTable and the MS.";
    }
    if ((!spwVerified) || (!antennaVerified) || (!fieldVerified))
      {
	ostringstream mesg;
	if (spwMesg.str() != "")  mesg << spwMesg.str() << endl;
	if (antMesg.str() != "")  mesg << antMesg.str() << endl;
	if (fieldMesg.str() != "")mesg << fieldMesg.str() << endl;
	throw(AipsError(mesg.str()));
      }
    return (spwVerified & antennaVerified & fieldVerified);
  }

} //# NAMESPACE CASA - END
