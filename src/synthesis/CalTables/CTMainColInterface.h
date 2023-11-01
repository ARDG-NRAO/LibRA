// -*- C++ -*-
//# CTMainColInterface.h: The generic interface for tables that can be used with MSSelection
//# Copyright (C) 1996,1997,1998,1999,2001
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
//#
//# $Id$

#ifndef CT_CTMAINCOLINTERFACE_H
#define CT_CTMAINCOLINTERFACE_H

#include <casacore/casa/aips.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/Cube.h>
#include <casacore/tables/TaQL/ExprNode.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/ms/MeasurementSets/MSMainEnums.h>
#include <casacore/ms/MSSel/MSSelectionError.h>
#include <casacore/ms/MSSel/MSSelectableTable.h>
#include <synthesis/CalTables/CTMainColumns.h>
namespace casa { //# NAMESPACE CASA - BEGIN


  class CTMainColInterface: public casacore::MSSelectableMainColumn
  {
  public: 
    CTMainColInterface():MSSelectableMainColumn(), ctCols_p(NULL) {};
    CTMainColInterface(const casacore::Table& ctAsTable): casacore::MSSelectableMainColumn(ctAsTable)
    {init(ctAsTable);}

    virtual ~CTMainColInterface() {if (ctCols_p) delete ctCols_p;};

    virtual void init(const casacore::Table& ctAsTable)
    {casacore::MSSelectableMainColumn::init(ctAsTable);ct_p = NewCalTable(ctAsTable); ctCols_p=new ROCTMainColumns(ct_p);}

    virtual const casacore::ArrayColumn<casacore::Bool>& flag() {return ctCols_p->flag();}

      virtual casacore::Bool flagRow(casacore::rownr_t i) {return allTrue(ctCols_p->flag()(i));}
    
    // For now, return timeEPQuant() even for exposureQuant.
    virtual const casacore::ROScalarQuantColumn<casacore::Double>& exposureQuant() {return ctCols_p->timeEPQuant();};
    virtual const casacore::ROScalarQuantColumn<casacore::Double>& timeQuant()     {return ctCols_p->timeQuant();}

    // This is not an MS
    virtual const casacore::MeasurementSet *asMS(){return NULL;};
  private:
    NewCalTable ct_p;
    ROCTMainColumns *ctCols_p;

  };

} //# NAMESPACE CASA - END

#endif
