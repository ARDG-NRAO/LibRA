//# MSChecker.cc: Some MS specific Utilities
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

#include <msvis/MSVis/MSChecker.h>

#include <casacore/tables/Tables/ConcatScalarColumn.h>

using namespace casacore;
namespace casa {
MSChecker::MSChecker(const MeasurementSet& ms) : _ms(ms) {}

void MSChecker::checkReferentialIntegrity() const {
    Int nrows = 0;
    String colname, tablename;
    uInt nAnts = _ms.antenna().nrow();
    for (uInt i=0; i<5; ++i) {
        switch (i) {
        case 0:
            nrows = nAnts;
            tablename = _ms.antenna().tableName();
            colname = _ms.columnName(MSMainEnums::ANTENNA1);
            break;
        case 1:
            nrows = nAnts;
            tablename = _ms.antenna().tableName();
            colname = _ms.columnName(MSMainEnums::ANTENNA2);
            break;
        case 2:
            nrows = _ms.dataDescription().nrow();
            tablename = _ms.dataDescription().tableName();
            colname = _ms.columnName(MSMainEnums::DATA_DESC_ID);
            break;
        case 3:
            nrows = _ms.field().nrow();
            tablename = _ms.field().tableName();
            colname = _ms.columnName(MSMainEnums::FIELD_ID);
            break;
        case 4:
            nrows = _ms.observation().nrow();
            tablename = _ms.observation().tableName();
            colname = _ms.columnName(MSMainEnums::OBSERVATION_ID);
            break;
        default:
            break;
        }
        Vector<Int> data;
        ScalarColumn<Int>(_ms, colname).getColumn(data);
        ThrowIf(
            anyGE(data, nrows),
            "Illegal " + colname + " value " + String::toString(max(data))
            + " found in main table. " + tablename + " only has "
            + String::toString(nrows) + " rows (IDs)"
        );
    }
}

}
