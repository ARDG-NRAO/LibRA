//# AsdmStMan.h: Storage Manager for the main table of a raw ASDM casacore::MS
//# Copyright (C) 2012
//# Associated Universities, Inc. Washington DC, USA.
//# (c) European Southern Observatory, 2012
//# Copyright by ESO (in the framework of the ALMA collaboration)
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
//# $Id: AsdmStMan.h 18108 2011-05-27 07:52:39Z broekema $

#ifndef ASDM_ASDMSTMAN_H
#define ASDM_ASDMSTMAN_H

//# Includes
#include <asdmstman/AsdmIndex.h>
#include <casacore/tables/DataMan/DataManager.h>
#include <casacore/casa/IO/FiledesIO.h>
#include <casacore/casa/Containers/Block.h>
#include <casacore/casa/Containers/Record.h>

namespace casa {

//# Forward Declarations.
class AsdmColumn;

// <summary>
// The Storage Manager for the main table of a raw ASDM casacore::MS
// </summary>

// <use visibility=export>

// <reviewed reviewer="UNKNOWN" date="before2004/08/25" tests="tAsdmStMan.cc">
// </reviewed>

// <prerequisite>
//# Classes you should understand before using this one.
//   <li> The casacore::Table casacore::Data Managers concept as described in module file
//        <linkto module="Tables:Data Managers">Tables.h</linkto>
// </prerequisite>

// <etymology>
// AsdmStMan is the data manager which stores the data for a ASDM MS.
// </etymology>

// <synopsis>
// AsdmStMan is a specific storage manager for the main table of a ASDM MS.
// For performance purposes the raw data from the correlator is directly
// written to a disk file. However, to be able to use the data directly as a
// casacore::MeasurementSet, this specific storage manager is created offering access to
// all mandatory columns in the main table of the MS.
//
// Similar to other storage managers, the AsdmStMan files need to be part of
// the table directory. There are two files:
// <ul>
//  <li> The meta file contains the meta data describing baselines, start time,
//       integration time, etc. It needs to be written as an casacore::AipsIO file.
//       The meta info should also tell the endianness of the data file.
//  <li> The data file consists of NSEQ data blocks each containing:
//   <ul>
//    <li> 4-byte sequence number defining the time stamp.
//    <li> casacore::Complex data with shape [npol,nchan,nbasel].
//    <li> Unsigned short nr of samples used in each data point. It has shape
//         [nchan,nbasel]. It defines WEIGHT_SPECTRUM and FLAG.
//    <li> Filler bytes to align the blocks as given in the meta info.
//   </ul>
//   The sequence numbers are ascending, but there can be holes due to
//   missing time stamps.
// </ul>
// The first versions of the data file can only handle regularly shaped data
// with equal integration times. A future version might be able to deal with
// varying integration times (depending on baseline length).
//
// Most of the casacore::MS columns (like DATA_DESC_ID) are not stored in the data file;
// usually they map to the value 0. This is also true for the UVW column, so
// the UVW coordinates need to be added to the table in a separate step because
// the online system does not have the resources to do it.
//
// All columns are readonly with the exception of DATA.
// </synopsis>

// <motivation>
// The common casacore::Table storage managers are too slow for the possibly high
// output rate of the ASDM correlator.
// </motivation>

// <example>
// The following example shows how to create a table and how to attach
// the storage manager to some columns.
// <srcblock>
//   casacore::SetupNewTable newtab("name.data", tableDesc, casacore::Table::New);
//   AsdmStMan stman;                     // define storage manager
//   newtab.bindColumn ("DATA", stman);    // bind column to st.man.
//   newtab.bindColumn ("FLAG", stman);    // bind column to st.man.
//   casacore::Table tab(newtab);                    // actually create table
// </srcblock>
// </example>

//# <todo asof="$DATE:$">
//# A casacore::List of bugs, limitations, extensions or planned refinements.
//# </todo>


class AsdmStMan : public casacore::DataManager
{
public:
    // Create a Asdm storage manager with the given name.
    // If no name is used, it is set to "AsdmStMan"
  explicit AsdmStMan (const casacore::String& dataManagerName = "AsdmStMan");

  // Create a Asdm storage manager with the given name.
  // The specifications are part of the record (as created by dataManagerSpec).
  AsdmStMan (const casacore::String& dataManagerName, const casacore::Record& spec);
  
  ~AsdmStMan();

  // Clone this object.
  virtual casacore::DataManager* clone() const;
  
  // Get the type name of the data manager (i.e. AsdmStMan).
  virtual casacore::String dataManagerType() const;
  
  // Get the name given to the storage manager (in the constructor).
  virtual casacore::String dataManagerName() const;
  
  // casacore::Record a record containing data manager specifications.
  virtual casacore::Record dataManagerSpec() const;

  // Is this a regular storage manager?
  // It is regular if it allows addition of rows and writing dara in them.
  // <br>We need to return false here.
  virtual casacore::Bool isRegular() const;

  // The storage manager can add rows, but does nothing.
  virtual casacore::Bool canAddRow() const;
  
  // The storage manager cannot delete rows.
  virtual casacore::Bool canRemoveRow() const;
  
  // The storage manager can add columns, which does not really do something.
  virtual casacore::Bool canAddColumn() const;
  
  // Columns can be removed, but it does not do anything at all.
  virtual casacore::Bool canRemoveColumn() const;
  
  // Make the object from the type name string.
  // This function gets registered in the casacore::DataManager "constructor" map.
  // The caller has to delete the object.
  static casacore::DataManager* makeObject (const casacore::String& aDataManType,
                                  const casacore::Record& spec);

  // Register the class name and the static makeObject "constructor".
  // This will make the engine known to the table system.
  static void registerClass();


  // Get the data shape.
  casacore::IPosition getShape (casacore::uInt rownr);

  // Get data.
  void getData (casacore::uInt rownr, casacore::Complex* buf);

  // Get float data
  void getData (casacore::uInt rownr, casacore::Float* buf);

  casacore::uInt getAsdmStManVersion() const
    { return itsVersion; }

  // access the references to the ASDM BDFs
  void getBDFNames(casacore::Block<casacore::String>& bDFNames);

  // overwrite the BDFNames (casacore::Block needs to have same size as original,
  // returns false otherwise)
  casacore::Bool setBDFNames(casacore::Block<casacore::String>& bDFNames);

  // overwrite the index with the information presently stored in the
  // data manager
  void writeIndex();

private:
  // Copy constructor cannot be used.
  AsdmStMan (const AsdmStMan& that);

  // Assignment cannot be used.
  AsdmStMan& operator= (const AsdmStMan& that);
  
  // Flush and optionally fsync the data.
  // It does nothing, and returns false.
  virtual casacore::Bool flush (casacore::AipsIO&, casacore::Bool doFsync);
  
  // Let the storage manager create files as needed for a new table.
  // This allows a column with an indirect array to create its file.
  virtual void create (casacore::uInt nrrow);
  
  // Open the storage manager file for an existing table.
  virtual void open (casacore::uInt nrrow, casacore::AipsIO&); //# should never be called

  // Prepare the columns (needed for UvwColumn).
  virtual void prepare();

  // Resync the storage manager with the new file contents.
  // It does nothing.
  virtual void resync (casacore::uInt nrrow);

  // Reopen the storage manager files for read/write.
  // It does nothing.
  virtual void reopenRW();
  
  // The data manager will be deleted (because all its columns are
  // requested to be deleted).
  // So clean up the things needed (e.g. delete files).
  virtual void deleteManager();

  // Add rows to the storage manager.
  // It cannot do it, so it does nothing.
  // This function will be called, because this storage manager is not the
  // only one used in an ASDM MS.
  virtual void addRow (casacore::uInt nrrow);
  
  // Delete a row from all columns.
  // It cannot do it, so throws an exception.
  virtual void removeRow (casacore::uInt rowNr);
  
  // Do the final addition of a column.
  // It won't do anything.
  virtual void addColumn (casacore::DataManagerColumn*);
  
  // Remove a column from the data file.
  // It won't do anything.
  virtual void removeColumn (casacore::DataManagerColumn*);
  
  // Create a column in the storage manager on behalf of a table column.
  // The caller has to delete the newly created object.
  // <group>
  // Create a scalar column.
  virtual casacore::DataManagerColumn* makeScalarColumn (const casacore::String& aName,
					       int aDataType,
					       const casacore::String& aDataTypeID);
  // Create a direct array column.
  virtual casacore::DataManagerColumn* makeDirArrColumn (const casacore::String& aName,
					       int aDataType,
					       const casacore::String& aDataTypeID);
  // Create an indirect array column.
  virtual casacore::DataManagerColumn* makeIndArrColumn (const casacore::String& aName,
					       int aDataType,
					       const casacore::String& aDataTypeID);
  // </group>

  // Initialize by reading the index file and opening the BDFs.
  void init();

  // Close the currently open BDF file.
  void closeBDF();

  // Return the entry number in the index containing the row.
  casacore::uInt searchIndex (casacore::Int64 rownr);

  // Return the index block containing the row.
  // It sets itsIndexEntry to that block.
  const AsdmIndex& findIndex (casacore::Int64 rownr);

  // Get data from the buffer.
  // <group>
  void getShort (const AsdmIndex&, casacore::Complex* buf, casacore::uInt bl, casacore::uInt spw);
  void getInt   (const AsdmIndex&, casacore::Complex* buf, casacore::uInt bl, casacore::uInt spw);
  void getFloat (const AsdmIndex&, casacore::Complex* buf, casacore::uInt bl, casacore::uInt spw);
  void getAuto  (const AsdmIndex&, casacore::Complex* buf, casacore::uInt bl);
  void getAuto  (const AsdmIndex&, casacore::Float* buf, casacore::uInt bl);
  // </group>


  // set transposeBLNum_v
  void setTransposeBLNum(casacore::uInt nBl);

  //# Declare member variables.
  // Name of data manager.
  casacore::String itsDataManName;
  // The column objects.
  std::vector<AsdmColumn*>    itsColumns;
  casacore::Block<casacore::String>          itsBDFNames;
  casacore::FiledesIO*             itsBDF;
  int                    itsFD;
  int                    itsOpenBDF;
  casacore::Int64                  itsFileOffset;
  casacore::Bool   itsDoSwap;       //# true = byte-swapping is needed
  casacore::Record itsSpec;         //# casacore::Data manager properties
  casacore::uInt   itsVersion;      //# Version of AsdmStMan casacore::MeasurementSet
  //# Fields to keep track of last block accessed.
  casacore::Int64  itsStartRow;     //# First row of data block
  casacore::Int64  itsEndRow;       //# First row of next data block
  casacore::uInt   itsIndexEntry;   //# Index entry number of current data block
  std::vector<char>      itsData;
  std::vector<AsdmIndex> itsIndex;
  std::vector<casacore::Int64>     itsIndexRows;

  casacore::uInt              itsNBl;
  std::vector<casacore::uInt>      itsTransposeBLNum_v;
};


} //# end namespace

#endif

