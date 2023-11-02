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
#ifndef SPLAT_SEARCHENGINE_H
#define SPLAT_SEARCHENGINE_H

#include <casacore/casa/aips.h>

#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Logging/LogIO.h>
#include <spectrallines/Splatalogue/SplatalogueTable.h>

namespace casa {

// <summary>Performs a query on a splatalogue spectral line table</summary>
// <use visibility=export>
// 
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>
// 

// <etymology>
// Searches a splatalogue table.
// </etymology>
//
// <synopsis>
// It is a requirement that users be able to perform searches on spectral
// line tables they import from Splatalogue.
// </synopsis>
//
// <example>
// <srcBlock>
// SearchEngine searcher(...);
// SplatalogueTable *resTable = searcher.search();
// </srcBlock>
// </example>
//
// <motivation>
// A class to for splatalogue spectral line searches.
// </motivation>
//
// <todo asof="">
// </todo>
 

class SearchEngine
{
public:
	// <src>table</src> casacore::Input spectral line table to search.
	// <src>list</src> casacore::List result set of search to logger (and optional logfile)?
	// <src>logfile</src> Logfile to list results to. Only used if list=true. If empty, no logfile is created.
	// <src>append</src> Append results to logfile (true) or overwrite logfile (false) if it exists? Only used if list=true and logfile not empty.
	SearchEngine(
		const SplatalogueTable* const table, const casacore::Bool list,
		const casacore::String& logfile, const casacore::Bool append
	);

	//destuctor
	~SearchEngine();

	// Search the table. It is the caller's responsibility to delete the returned pointer.
	SplatalogueTable* search(
		const casacore::String& resultsTableName, const casacore::Double freqLow, const casacore::Double freqHigh,
		const casacore::Vector<casacore::String>& species, const casacore::Bool recommendedOnly,
		const casacore::Vector<casacore::String>& chemNames, const casacore::Vector<casacore::String>& qns,
		const casacore::Double intensityLow, const casacore::Double intensityHigh,
		const casacore::Double smu2Low, const casacore::Double smu2High,
		const casacore::Double logaLow, const casacore::Double logaHigh,
		const casacore::Double elLow, const casacore::Double elHigh,
		const casacore::Double euLow, const casacore::Double euHigh,
		const casacore::Bool includeRRLs, const casacore::Bool onlyRRLs
	) const;

	// Get the unique species in the table.
	casacore::Vector<casacore::String> uniqueSpecies() const;

	// Get the unique chemical names in the table.
	casacore::Vector<casacore::String> uniqueChemicalNames() const;

private:
	casacore::LogIO *_log;
	const SplatalogueTable *_table;
	casacore::String _logfile;
	const casacore::Bool _list, _append;
	SearchEngine();

	casacore::String _getBetweenClause(
		const casacore::String& col, const casacore::Double low, const casacore::Double high
	) const;

	// If the query table is currently not on disk, this method will temporarily
	// write a copy of it to disk and query it, and then remove the copy from disk.
	// The method returns the query results table.
	casacore::Table _runQuery(const casacore::String& query) const;

	void _logIt(const casacore::String& logString) const;
};


} //# NAMESPACE CASA - END

#endif
