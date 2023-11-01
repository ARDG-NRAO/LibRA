//# NewCalTable.h: Calibration table access and creation
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
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$

#ifndef CALIBRATION_NEWCALTABLE_H
#define CALIBRATION_NEWCALTABLE_H

#include <casacore/casa/aips.h>
#include <casacore/tables/Tables/Table.h>
#include <casacore/tables/TaQL/ExprNode.h>
#include <casacore/casa/Containers/Record.h>
#include <casacore/casa/OS/Path.h>
#include <casacore/casa/Utilities/Sort.h>
#include <casacore/ms/MeasurementSets/MSObservation.h>
#include <casacore/ms/MeasurementSets/MSAntenna.h>
#include <casacore/ms/MeasurementSets/MSField.h>
#include <casacore/ms/MeasurementSets/MSSpectralWindow.h>
#include <casacore/ms/MeasurementSets/MSHistory.h>
#include <casacore/ms/MeasurementSets/MSObsColumns.h>
#include <casacore/ms/MeasurementSets/MSAntennaColumns.h>
#include <casacore/ms/MeasurementSets/MSFieldColumns.h>
#include <casacore/ms/MeasurementSets/MSSpWindowColumns.h>
#include <casacore/ms/MeasurementSets/MSHistoryColumns.h>
#include <synthesis/CalTables/CTDesc.h>
#include <synthesis/CalTables/CTMainRecord.h>
#include <synthesis/CalTables/CTMainColumns.h>
#include <synthesis/CalTables/VisCalEnum.h>
#include <msvis/MSVis/SimpleSimVi2.h>

namespace casa { //# NAMESPACE CASA - BEGIN


// <summary> 
// NewCalTable: New Calibration table access and creation
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="CalTableDesc">CalTableDesc</linkto> module
// </prerequisite>
//
// <etymology>
// From "calibration" and "table".
// </etymology>
//
// <synopsis>
// The NewCalTable classes provide basic access to calibration tables. This 
// includes the creation of calibration tables, sorting and selection
// capabilities, and basic data access. Specializations for baseline-based,
// time-variable and solvable VisJones types, and sub-types, are provided 
// through inheritance.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// This class is used by other calibration table accessors and iterators.
// </motivation>
//
// <todo asof="11/06/10">
// </todo>


// Typedefs for subtable objects
typedef casacore::MSField CTField;
typedef casacore::MSObservation CTObservation;
typedef casacore::MSAntenna CTAntenna;
typedef casacore::MSSpectralWindow CTSpectralWindow;
typedef casacore::MSHistory CTHistory;
typedef casacore::MSFieldColumns CTFieldColumns;
typedef casacore::MSObservationColumns CTObservationColumns;
typedef casacore::MSAntennaColumns CTAntennaColumns;
typedef casacore::MSSpWindowColumns CTSpWindowColumns;
typedef casacore::MSHistoryColumns CTHistoryColumns;
typedef casacore::MSFieldColumns ROCTFieldColumns;
typedef casacore::MSObservationColumns ROCTObservationColumns;
typedef casacore::MSAntennaColumns ROCTAntennaColumns;
typedef casacore::MSSpWindowColumns ROCTSpWindowColumns;
typedef casacore::MSHistoryColumns ROCTHistoryColumns;

class NewCalTable : public casacore::Table
{

  friend class SolvableVisCal;

 public:
   // Default null constructor, and destructor
   NewCalTable();
   virtual ~NewCalTable();

   // Construct from a specified table name, calibration table descriptor 
   // and table access option. Used for creating new tables.
   NewCalTable (const casacore::String& tableName, CTDesc& ctableDesc,
		casacore::Table::TableOption access = casacore::Table::New, 
		casacore::Table::TableType ttype = casacore::Table::Plain);

   // Construct from setupNewTable 
   NewCalTable (casacore::SetupNewTable& newTab, casacore::uInt nrow = 0, casacore::Bool initialize = false);
   
   // Create an empty NewCalTable conveniently
   NewCalTable(casacore::String tableName,VisCalEnum::VCParType parType,
	       casacore::String typeName,casacore::String msName,casacore::Bool doSingleChan);

   // Construct from a specified table name, and access option. Used
   // for accessing existing tables.
   NewCalTable (const casacore::String& tableName, 
		casacore::Table::TableOption access = casacore::Table::Old, 
		casacore::Table::TableType ttype = casacore::Table::Memory);
   // control locking option
   NewCalTable (const casacore::String& tableName, 
        const casacore::TableLock& lockOptions,
		casacore::Table::TableOption access = casacore::Table::Old,
		casacore::Table::TableType ttype = casacore::Table::Memory);

   static NewCalTable createCT(const casacore::String& tableName, 
			       casacore::Table::TableOption access, 
			       casacore::Table::TableType ttype, 
			       casacore::Bool doBackCompat=true);

   static NewCalTable* createCTptr(const casacore::String& tableName, 
				   casacore::Table::TableOption access, 
				   casacore::Table::TableType ttype, 
				   casacore::Bool doBackCompat=true);



   // Construct from an existing table object
   NewCalTable (const casacore::Table& table);

   // Copy constructor
   NewCalTable (const NewCalTable& other);

   // Create a vanilla NewCalTable from shapes (for testing purposes)
   //  Default rTime is 2012/01/06/12:00:00
   NewCalTable(casacore::String tableName,casacore::String CorF,
	       casacore::Int nObs=1, casacore::Int nScanPerObs=1,casacore::Int nTimePerScan=1,
	       casacore::Int nAnt=1, casacore::Int nSpw=1, casacore::Vector<casacore::Int> nChan=casacore::Vector<casacore::Int>(1,1), 
	       casacore::Int nFld=1, 
	       casacore::Double rTime=0.0, casacore::Double tint=0.0,
	       casacore::Bool disk=false, casacore::Bool verbose=false);

   // Create an empty NewCalTable from a SimpleSimVi2Parameters object
   // NB: only for testing, for now
   NewCalTable(casacore::String tableName, casacore::String CorF, casacore::String caltype,
	       const vi::SimpleSimVi2Parameters& ssp,
	       casacore::Bool disk=false,casacore::Bool verbose=false);

   // Assignment operator
   NewCalTable& operator= (const NewCalTable& other);

   // Handle backward compatibility
   static casacore::Bool CTBackCompat(const casacore::String& caltable);

   // Set the casacore::TableInfo record
   void setTableInfo();

   // initialize reference to subtables
   void createSubTables();
   void attachSubTables();
   void clearSubtables();

   void copyMemCalSubtables(const NewCalTable& other);
   void copyMemCalSubtable(const casacore::Table& otherSubtable, casacore::Table& subTable);

   // casacore::Complex parameters?  (else casacore::Float)
   casacore::Bool isComplex();

   // Report the polarization basis (from header record)
   casacore::String polBasis();

   // Report the CASA version at which the NCT was created (from header)
   casacore::String CASAvers();

   // Get a row from cal_main
   casacore::Record getRowMain (const casacore::Int& jrow);
    
   // Put a row to cal_main
   void putRowMain (const casacore::Int& jrow, CTMainRecord& tableRec);

   // Get rows of the data from the main table
   //   void fillRowsMain ();

   // Set Meta data info from a parent casacore::MS 
   void setMetaInfo (const casacore::String& msName);

   //validate input table desc. over required caltable desc
   static casacore::Bool conformant(const casacore::TableDesc& tabDesc);

   // save to disk
   void writeToDisk(const casacore::String& tableName); 

   CTObservation& observation() {return observation_p;}
   CTAntenna& antenna() {return antenna_p;}
   CTField& field() {return field_p;}
   CTSpectralWindow& spectralWindow() {return spectralWindow_p;}
   CTHistory& history() {return history_p;}
   const CTObservation& observation() const {return observation_p;}
   const CTAntenna& antenna() const {return antenna_p;}
   const CTField& field() const {return field_p;}
   const CTSpectralWindow& spectralWindow() const {return spectralWindow_p;}
   const CTHistory& history() const {return history_p;}

   // Provide RO access to 
   //   inline ROCTMainColumns mcols() const {return ROCTMainColumns(*this);};

   static casacore::Complex NCTtestvalueC(casacore::Int iant,casacore::Int ispw,casacore::Double ich,casacore::Double time,casacore::Double refTime,casacore::Double tint);
   static casacore::Float NCTtestvalueF(casacore::Int iant,casacore::Int ispw,casacore::Double ich,casacore::Double time,casacore::Double refTime,casacore::Double tint);

   // Fill in antenna-based solutions
   //  (obsId-less version calls obsId-capable version w/ obsId=0)
   void fillAntBasedMainRows(casacore::uInt nrows, 
			     casacore::Double time,casacore::Double interval,
			     casacore::Int fieldId,casacore::uInt spwId,casacore::Int scanNo,
			     const casacore::Vector<casacore::Int>& ant1list, casacore::Int refant,
			     const casacore::Cube<casacore::Complex>& cparam,
			     const casacore::Cube<casacore::Bool>& flag,
			     const casacore::Cube<casacore::Float>& paramErr,
			     const casacore::Cube<casacore::Float>& snr);
   void fillAntBasedMainRows(casacore::uInt nrows, 
			     casacore::Double time,casacore::Double interval,
			     casacore::Int fieldId,casacore::uInt spwId,casacore::Int obsId,casacore::Int scanNo,
			     const casacore::Vector<casacore::Int>& ant1list, casacore::Int refant,
			     const casacore::Cube<casacore::Complex>& cparam,
			     const casacore::Cube<casacore::Bool>& flag,
			     const casacore::Cube<casacore::Float>& paramErr,
			     const casacore::Cube<casacore::Float>& snr);

   // Reset spw freq info
   void setSpwFreqs(casacore::Int spw, const casacore::Vector<casacore::Double>& freq, 
		    const casacore::Vector<casacore::Double>& chanwidth=casacore::Vector<casacore::Double>());

   // Set FLAG_ROW in SPECTRAL_WINDOW subtable for spws absent in MAIN
   void flagAbsentSpws();

   // Merge SPW subtable rows from another NewCalTable
   void mergeSpwMetaInfo(const NewCalTable& other);

   // Add a line to the HISTORY table
   void addHistoryMessage(casacore::String app="",casacore::String message="none");

 private:

   // Services for generic test table ctor
   //  Default rTime is 2012/01/06/12:00:00
   void fillGenericContents(casacore::Int nObs=1, casacore::Int nScanPerObs=1, casacore::Int nTimePerScan=1,
			    casacore::Int nAnt=1, casacore::Int nSpw=1, casacore::Vector<casacore::Int> nChan=casacore::Vector<casacore::Int>(1,1), 
			    casacore::Int nFld=1, 
			    casacore::Double rTime=0.0, casacore::Double tint=0.0,
			    casacore::Bool verbose=false);
   void fillGenericObs(casacore::Int nObs);
   void fillGenericField(casacore::Int nFld);
   void fillGenericAntenna(casacore::Int nAnt);
   void fillGenericSpw(casacore::Int nSpw,casacore::Vector<casacore::Int>& nChan);
     
   // Init subtables from SimpleSimVi2Parameters
   void fillGenericObs(const vi::SimpleSimVi2Parameters& ssp);
   void fillGenericField(const vi::SimpleSimVi2Parameters& ssp);
   void fillGenericAntenna(const vi::SimpleSimVi2Parameters& ssp);
   void fillGenericSpw(const vi::SimpleSimVi2Parameters& ssp);

   // Force Spw subtable to be all nchan=1
   //  (very basic; uses chan n/2 freq)
   void makeSpwSingleChan();

   // Handle pre-4.1 caltables that don't have OBS_ID
   //  (by adding a phoney one with a single OBS_ID
   void addPhoneyObs();

   // Handle Eph objects...
   void handleEphObj(const casacore::MSField& msfldtab,const casacore::String& msName);


   // The subtables
   CTObservation observation_p;
   CTAntenna antenna_p;
   CTField field_p;
   CTSpectralWindow spectralWindow_p;
   CTHistory history_p;

 };

} //# NAMESPACE CASA - END

#endif
