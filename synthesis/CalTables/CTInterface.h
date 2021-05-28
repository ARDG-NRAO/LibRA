//# CTInterface.h: Class to present a CalTable with casacore::MS interface
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

#ifndef CT_CTINTERFACE_H
#define CT_CTINTERFACE_H

#include <casa/aips.h>
#include <casa/BasicSL/String.h>
#include <ms/MSSel/MSSelectableTable.h>
#include <ms/MeasurementSets/MSMainEnums.h>
#include <synthesis/CalTables/NewCalTable.h>
#include <synthesis/CalTables/CTEnums.h>
#include <synthesis/CalTables/CTMainColInterface.h>
//#include <ms/MeasurementSets/MSDataDescription.h>
#include <tables/Tables/SetupNewTab.h>
#include <tables/Tables/TableDesc.h>
namespace casa { //# NAMESPACE CASA - BEGIN

  class CTInterface: public casacore::MSSelectableTable
  {
  public:
    CTInterface():fakeDDSubTable(), ctMainCols_p(NULL) {};
    CTInterface(const casacore::Table& table);

    virtual ~CTInterface();
    virtual const CTObservation& observation()        {return asCT()->observation();}
    virtual const CTAntenna& antenna()                {return asCT()->antenna();}
    virtual const CTField& field()                    {return asCT()->field();}
    virtual const CTSpectralWindow& spectralWindow()  {return asCT()->spectralWindow();}
    virtual casacore::Bool isMS()                               {return false;};

    // BASELINE_BASED, PURE_ANTENNA_BASED, REF_ANTENNA_BASED
    virtual casacore::MSSelectableTable::MSSDataType dataType();

    virtual const casacore::MSDataDescription& dataDescription();
    virtual casacore::String columnName(casacore::MSMainEnums::PredefinedColumns nameEnum);
    //    virtual const casacore::MSObservation& observation();

    virtual const casacore::MeasurementSet* asMS() 
    {
      //{throw(casacore::AipsError("asMS() called from CTInterface class"));}
      //cerr << "asMS() called from CTInterface class" << endl;
      //return static_cast<const casacore::MeasurementSet *>(table());
      return NULL;
    }
    virtual const NewCalTable* asCT();
    virtual casacore::MSSelectableMainColumn* mainColumns()
    {ctMainCols_p = new CTMainColInterface(*table_p); return ctMainCols_p;};
  private:
    casacore::MSDataDescription fakeDDSubTable;
    void makeDDSubTable();
    CTMainColInterface *ctMainCols_p;
  };
} //# NAMESPACE CASA - END

#endif
