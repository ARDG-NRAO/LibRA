//# MSContinuumSubtractor.h: Fit & subtract continuum from spectral line data
//# Copyright (C) 2004
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
//#
#ifndef SPLAT_SPLATALOGUETABLE_H
#define SPLAT_SPLATALOGUETABLE_H

#include <casa/aips.h>

#include <casa/Arrays/Vector.h>
#include <casa/Containers/Record.h>
#include <tables/Tables/Table.h>

namespace casa {

// <summary>Representation of a Splatalogue casacore::Table</summary>
// <use visibility=export>
// 
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>
// 

// <etymology>
// Splatalogue Table
// </etymology>
//
// <synopsis>
// A table containing Splatalogue fields and metadata which holds Splatalogue data.
// </synopsis>
//
// <example>
// <srcBlock>

// </srcBlock>
// </example>
//
// <motivation>
//
// </motivation>
//
// <todo asof="">
// </todo>
 
class SplatalogueTable : public casacore::Table {
public:

	const static casacore::String SPECIES;
	const static casacore::String RECOMMENDED;
	const static casacore::String CHEMICAL_NAME;
	const static casacore::String FREQUENCY;
	const static casacore::String QUANTUM_NUMBERS;
	const static casacore::String INTENSITY;
	const static casacore::String SMU2;
	const static casacore::String LOGA;
	const static casacore::String EL;
	const static casacore::String EU;
	const static casacore::String LINELIST;
	const static casacore::String ISSPLAT;

	const static casacore::String RECORD_VALUE;
	const static casacore::String RECORD_UNIT;
	const static casacore::String RECORD_SPECIES;
	const static casacore::String RECORD_RECOMMENDED;
	const static casacore::String RECORD_CHEMNAME;
	const static casacore::String RECORD_FREQUENCY;
	const static casacore::String RECORD_QNS;
	const static casacore::String RECORD_INTENSITY;
	const static casacore::String RECORD_SMU2;
	const static casacore::String RECORD_LOGA;
	const static casacore::String RECORD_EL;
	const static casacore::String RECORD_EU;
	const static casacore::String RECORD_LINE_LIST;

	SplatalogueTable(
		casacore::SetupNewTable& snt, casacore::uInt nrow,
		const casacore::String& freqUnit, const casacore::String& smu2Unit,
		const casacore::String& elUnit, const casacore::String& euUnit  
	);

	// open an existing splatalogue table. Checks are done to ensure the table
	// is indeed a SplatalogueTable and not just a generic table. If

	SplatalogueTable(const casacore::String& tablename);

	// copy semantics. Tests are done to insure the input table is a splatalogue table.
	SplatalogueTable(const casacore::Table& table);

	casacore::String getFrequencyUnit() const;

	// Pretty print table contents to a String.
	casacore::String list() const;

	casacore::Record toRecord() const;

private:
	casacore::String _freqUnit, _smu2Unit, _elUnit, _euUnit;

	SplatalogueTable();
	//void _defineTable(const casacore::String& tablename, const casacore::uInt nrow);
	void _construct(const casacore::Bool setup);

	void _addKeywords();


};


} //# NAMESPACE CASA - END

#endif
