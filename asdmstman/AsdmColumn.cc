//# AsdmColumn.cc: A column in the ASDM Storage Manager
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
//# You should have receied a copy of the GNU Library General Public License
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
//# $Id: AsdmColumn.cc 19324 2011-11-21 07:29:55Z diepen $

#include <asdmstman/AsdmColumn.h>
#include <casacore/casa/Arrays/Array.h>

using namespace casacore;
using namespace casa;

using namespace casacore;
namespace casa {

  AsdmColumn::~AsdmColumn()
  {}
  Bool AsdmColumn::isWritable() const
  {
    // We return true even though the column is not writable. If an actual write is done,
    // an exception will be thrown. This ensures that the AsdmStMan will work with MSMainColumns.
    // The alternative solutions would cause too much code duplication.
    return true;
  }
  void AsdmColumn::setShapeColumn (const IPosition&)
  {}
  void AsdmColumn::prepareCol()
  {}


  AsdmDataColumn::~AsdmDataColumn()
  {}
  IPosition AsdmDataColumn::shape (uInt rownr)
  {
    return itsParent->getShape (rownr);
  }
  void AsdmDataColumn::getArrayComplexV (uInt rownr, Array<Complex>* dataPtr)
  {
    Bool deleteIt;
    Complex* data = dataPtr->getStorage(deleteIt);
    itsParent->getData (rownr, data);
    dataPtr->putStorage (data, deleteIt);
  }

  void AsdmDataColumn::getSliceComplexV (uInt rowNumber, const Slicer & slicer,
                                         Array<casacore::Complex> * destination)
  {
     // Create an array to hold the entire table cell.

    Array<Complex> entireCell (shape (rowNumber));

    // Load the entire cell into an array

    Bool deleteIt;
    Complex * entireCellData = entireCell.getStorage(deleteIt); // get pointer to storage

    itsParent->getData (rowNumber, entireCellData); // fill storage with data

    entireCell.putStorage (entireCellData, deleteIt); // "return" storage

    // Transfer the data specified by the slicer into the destination array.

    destination->assign (entireCell (slicer));
  }

  AsdmFloatDataColumn::~AsdmFloatDataColumn()
  {}
  IPosition AsdmFloatDataColumn::shape (uInt rownr)
  {
    return itsParent->getShape (rownr);
  }
  void AsdmFloatDataColumn::getArrayfloatV (uInt rownr, Array<Float>* dataPtr)
  {
    Bool deleteIt;
    Float* data = dataPtr->getStorage(deleteIt);
    itsParent->getData (rownr, data);
    dataPtr->putStorage (data, deleteIt);
  }

  void AsdmFloatDataColumn::getSlicefloatV (uInt rowNumber, const Slicer & slicer,
					    Array<casacore::Float> * destination)
  {
     // Create an array to hold the entire table cell.

    Array<Float> entireCell (shape (rowNumber));

    // Load the entire cell into an array

    Bool deleteIt;
    Float * entireCellData = entireCell.getStorage(deleteIt); // get pointer to storage

    itsParent->getData (rowNumber, entireCellData); // fill storage with data

    entireCell.putStorage (entireCellData, deleteIt); // "return" storage

    // Transfer the data specified by the slicer into the destination array.

    destination->assign (entireCell (slicer));
  }




  AsdmFlagColumn::~AsdmFlagColumn()
  {}
  IPosition AsdmFlagColumn::shape (uInt rownr)
  {
    return itsParent->getShape (rownr);
  }
  void AsdmFlagColumn::getArrayBoolV (uInt, Array<Bool>* dataPtr)
  {
    *dataPtr = false;
  }

  void AsdmFlagColumn::getSliceBoolV (uInt /*rowNumber*/, const Slicer & /* slicer */,
                                       Array<casacore::Bool> * destination)
  {
    * destination = false;
  }


  AsdmWeightColumn::~AsdmWeightColumn()
  {}
  IPosition AsdmWeightColumn::shape (uInt rownr)
  {
    return IPosition(1, itsParent->getShape(rownr)[0]);
  }
  void AsdmWeightColumn::getArrayfloatV (uInt, Array<Float>* dataPtr)
  {
    *dataPtr = float(1);
  }
    void AsdmWeightColumn::getSlicefloatV (uInt /*rowNumber*/, const Slicer & /* slicer */,
                                         Array<casacore::Float> * destination)
  {
    * destination = 1.0f;
  }


  AsdmSigmaColumn::~AsdmSigmaColumn()
  {}
  IPosition AsdmSigmaColumn::shape (uInt rownr)
  {
    return IPosition(1, itsParent->getShape(rownr)[0]);
  }
  void AsdmSigmaColumn::getArrayfloatV (uInt, Array<Float>* dataPtr)
  {
    *dataPtr = float(1);
  }

  void AsdmSigmaColumn::getSlicefloatV (uInt /*rowNumber*/, const Slicer & /* slicer */,
                                        Array<casacore::Float> * destination)
  {
    * destination = 1.0f;
  }


} //# end namespace
