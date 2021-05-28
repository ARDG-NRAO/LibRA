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
#ifndef SPLAT_LISTCONVERTER_H
#define SPLAT_LISTCONVERTER_H

#include <casa/aips.h>

#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/String.h>
#include <casa/Logging/LogIO.h>
#include <casa/OS/RegularFile.h>
#include <spectrallines/Splatalogue/SplatalogueTable.h>

namespace casa {

// <summary>Converts a list or lists dumped from the splatalogue web interface to a casa table.</summary>
// <use visibility=export>
// 
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>
// 

// <etymology>
// Converts a splatalogue line list to a CASA table.
// </etymology>
//
// <synopsis>
// It is a requirement that users be able to convert splatalogue line
// lists to CASA tables for manipulation of these data within CASA.
// </synopsis>
//
// <example>
// <srcBlock>
// casacore::Vector<casacore::String> files(2);
// files[0] = "myfile1";
// files[1] = "myfile2";
// Converter converter(files, "mytable");
// converter.convert();
// </srcBlock>
// </example>
//
// <motivation>
// A class to for splatalogue list conversion.
// </motivation>
//
// <todo asof="">
// </todo>
 

class ListConverter {
public:
	// Constructor
	ListConverter(const casacore::Vector<casacore::String>& filename, const casacore::String& table);

	//destuctor
	~ListConverter();

	// load the lists into the table. It is the caller's responsibility to
	// delete the returned pointer.
	SplatalogueTable* load();

private:
	casacore::LogIO *_log;
	ListConverter();
	casacore::Vector<casacore::RegularFile> _listFiles;
	casacore::String _tableName, _freqUnit, _smu2Unit, _elUnit, _euUnit;
	casacore::Vector<casacore::String> _species, _chemName, _qns, _lineList;
	casacore::Vector<casacore::Bool> _recommended;
	casacore::Vector<casacore::Double> _frequency;
	casacore::Vector<casacore::Float> _intensity, _smu2, _logA, _eL, _eU;

	void _parseLists();
	SplatalogueTable* _defineTable(const casacore::uInt nrows);
	void _addData(const SplatalogueTable* table) const;
};


} //# NAMESPACE CASA - END

#endif
