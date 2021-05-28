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
#ifndef SEARCHERSQLITE_H_
#define SEARCHERSQLITE_H_

#include <spectrallines/Splatalogue/Searcher.h>
#include <spectrallines/Splatalogue/SplatResult.h>

class sqlite3;
class sqlite3_stmt;

using namespace std;

namespace casa {
/**
 * Searches a local sqlite database for molecular lines meeting the specified
 * search criteria.
 */

class SearcherSQLite : public Searcher{
public:
	SearcherSQLite( const string& databasePath);

	/**
	 * Returns a string containing (columnName, columnType) information
	 * for all the columns in the table.
	 */
	string tableInfo( const string& tableName, string& errorMsg ) const;

	/**
	 * Returns the date the database file was installed.
	 */
	string getCreatedDate() const;
	virtual bool isConnected() const;
	virtual void stopSearch();

	//Set all the search parameters back to their defaults.
	virtual void reset();

	//Search Paramaters
	virtual void setChemicalNames( const std::vector<string>& chemNames );
	virtual void setSpeciesNames( const std::vector<string>& speciesNames );
	/**
	 * Units are assumed to be MHz.
	 */
	virtual void setFrequencyRange( double minValue, double maxValue );
	virtual void setIntensityRange( double minValue, double maxValue );
	virtual void setSmu2Range( double minValue, double maxValue );
	virtual void setLogaRange( double minValue, double maxValue );
	virtual void setElRange( double minValue, double maxValue );
	virtual void setEuRange( double minValue, double maxValue );
	virtual void setQNS( const std::vector<string>& qns );


	//Astronomical Filters
	virtual void setFilterTop20( bool filter = true );
	virtual void setFilterPlanetaryAtmosphere( bool filter = true );
	virtual void setFilterHotCores( bool filter = true );
	virtual void setFilterDarkClouds( bool filter = true );
	virtual void setFilterDiffuseClouds( bool filter = true );
	virtual void setFilterComets( bool filter = true );
	virtual void setFilterAgbPpnPn( bool filter = true );
	virtual void setFilterExtragalactic( bool filter = true );

	//Performing the Search
	/**
	 * The offset is used when there is a limit on the number of rows
	 * that will be returned from the database.  In the case that the
	 * number of rows that meet the search criteria exceeds the search
	 * limit, the offset indicates the starting index of the rows that
	 * are returned.
	 */
	virtual std::vector<SplatResult> doSearch( string& errorMsg, int offset );
	virtual long doSearchCount( string& errorMsg );
	/**
	 * Sets the maximum number of rows that will be returned as a result
	 * of a database search.  Setting this value to a negative or zero
	 * value means that there will be no limit on the number of search results
	 * from the database.
	 */
	virtual void setSearchResultLimit( int limit );
	virtual ~SearcherSQLite();

private:
	bool executeQuery( sqlite3_stmt*& statement, const string& query,
		string& errorMsg ) const;
	/*
	 * Constructs the SQL for a SELECT query based on the search parameters
	 * that have been previously set.  If 'countOnly' is set to true, the
	 * query will return the number of rows matching the search criteria;
	 * otherwise, it will all columns in the rows matching the search criteria.
	 * The 'offset' parameter indicates the starting index for the first row
	 * matching the search criteria.
	 */
	string prepareQuery( bool countOnly, int offset ) const;
	std::string getTrue() const;
	string numToString( double number ) const;
	string getBetweenClause( const string& columnName, double low, double high) const;
	string getInClause( const string& columnName, const std::vector<string>& values ) const;
	string getLikeClause( const string& columnName, const std::vector<string>& values ) const;

	//Set-up
	sqlite3* db;

	//Search parameters
	double minValueFreq;
	double maxValueFreq;
	double minValueIntensity;
	double maxValueIntensity;
	double minValueSmu2;
	double maxValueSmu2;
	double minValueLoga;
	double maxValueLoga;
	double minValueEl;
	double maxValueEl;
	double minValueEu;
	double maxValueEu;
	//bool recommendedOnly;
	std::vector<string> speciesNames;
	std::vector<string> chemicalNames;
	std::vector<string> qns;


	enum FILTER_LIST { FILTER_TOP_20, FILTER_PLANETARY_ATMOSPHERE, FILTER_HOT_CORES,
		FILTER_DARK_CLOUDS, FILTER_DIFFUSE_CLOUDS, FILTER_COMETS, FILTER_AGB_PPN_PN,
		FILTER_EXTRAGALACTIC, END_FILTERS };
	std::vector<bool> filters;
	static std::vector<string> filterNames;

	//casacore::Table Names
	const static std::string TABLE_MAIN;
	const static std::string TABLE_SPECIES;

	//casacore::Table columns
	const static std::string FREQUENCY_COLUMN;
	const static std::string TEMPERATURE_COLUMN;
	const static std::string SPECIES_ID_COLUMN;
	const static std::string SPECIES_COLUMN;
	const static std::string SMU2_COLUMN;
	const static std::string EL_COLUMN;
	const static std::string EU_COLUMN;
	const static std::string LOGA_COLUMN;
	const static std::string INTENSITY_COLUMN;
	const static std::string RESOLVED_QNS_COLUMN;
	const static std::string CHEMICAL_NAME_COLUMN;
	enum TableColumns { SPECIES_ID_COL, SPECIES_NAME_COL, CHEMICAL_NAME_COL,
		FREQUENCY_COL, TEMPERATURE_COL, RESOLVED_QNS_COL, INTENSITY_COL, SMU2_COL, LOGA_COL, EL_COL,
		EU_COL, END_COL };
	static std::vector<string> resultColumns;


	const static std::string FILTER_KNOWN_AST_COLUMN;
	const static std::string FILTER_PLANET_COLUMN;
	const static std::string FILTER_HOTCORE_COLUMN;
	const static std::string FILTER_DIFFUSECLOUD_COLUMN;
	const static std::string FILTER_DARKCLOUD_COLUMN;
	const static std::string FILTER_COMET_COLUMN;
	const static std::string FILTER_EXTRAGALACTIC_COLUMN;
	const static std::string FILTER_AGB_PPN_PN_COLUMN;
	const static std::string FILTER_TOP20_COLUMN;


	//SQL Constants
	const static std::string FROM;
	const static std::string SELECT;
	const static std::string BETWEEN;
	const static std::string AND;
	const static std::string OPEN_PAREN;
	const static std::string CLOSE_PAREN;
	const static std::string SINGLE_QUOTE;
	const static std::string COMMA;
	const static std::string PERIOD;
	const static std::string EQUALS;
	const static std::string IN;
	const static std::string LIKE;
	const static std::string OR;

	const static int DEFAULT_VALUE;

	//Limiting the number of rows returned by a search.
	int rowLimit;
};
}
#endif /* SEARCHLOCAL_H_ */
