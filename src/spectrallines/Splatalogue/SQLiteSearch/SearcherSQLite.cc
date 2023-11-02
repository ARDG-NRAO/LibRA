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
#include "SearcherSQLite.h"


#include <spectrallines/Splatalogue/SQLiteSearch/DatabaseConnector.h>
#include <sqlite3.h>
#include <iostream>
#include <sstream>


using namespace std;

namespace casa {
const std::string SearcherSQLite::TABLE_SPECIES = "species";
const std::string SearcherSQLite::TABLE_MAIN = "main";

const std::string SearcherSQLite::SPECIES_ID_COLUMN = "species_id"; //Type 1
const std::string SearcherSQLite::FREQUENCY_COLUMN = "orderedfreq"; //Type 2
const std::string SearcherSQLite::TEMPERATURE_COLUMN ="upper_state_energy_K";
const std::string SearcherSQLite::SPECIES_COLUMN = "s_name"; //Type 3
const std::string SearcherSQLite::SMU2_COLUMN = "sijmu2";
const std::string SearcherSQLite::LOGA_COLUMN = "aij";
const std::string SearcherSQLite::EL_COLUMN = "lower_state_energy";
const std::string SearcherSQLite::EU_COLUMN = "upper_state_energy";
const std::string SearcherSQLite::RESOLVED_QNS_COLUMN = "resolved_QNs"; //Type 3
const std::string SearcherSQLite::INTENSITY_COLUMN = "intintensity"; //Type 5
const std::string SearcherSQLite::CHEMICAL_NAME_COLUMN = "chemical_name";

std::vector<string> SearcherSQLite::resultColumns(END_COL);

const std::string SearcherSQLite::FILTER_KNOWN_AST_COLUMN = "known_ast_molecules";
const std::string SearcherSQLite::FILTER_PLANET_COLUMN = "planet";
const std::string SearcherSQLite::FILTER_HOTCORE_COLUMN = "ism_hotcore";
const std::string SearcherSQLite::FILTER_DIFFUSECLOUD_COLUMN = "ism_diffusecloud";
const std::string SearcherSQLite::FILTER_DARKCLOUD_COLUMN = "ism_darkcloud";
const std::string SearcherSQLite::FILTER_COMET_COLUMN = "comet";
const std::string SearcherSQLite::FILTER_EXTRAGALACTIC_COLUMN = "extragalactic";
const std::string SearcherSQLite::FILTER_AGB_PPN_PN_COLUMN = "AGB_PPN_PN";
const std::string SearcherSQLite::FILTER_TOP20_COLUMN = "Top20";

std::vector<string> SearcherSQLite::filterNames( END_FILTERS );

const std::string SearcherSQLite::AND = " AND ";
const std::string SearcherSQLite::OPEN_PAREN = "(";
const std::string SearcherSQLite::CLOSE_PAREN = ")";
const std::string SearcherSQLite::SINGLE_QUOTE = "'";
const std::string SearcherSQLite::COMMA = ", ";
const std::string SearcherSQLite::PERIOD = ".";
const std::string SearcherSQLite::EQUALS = " = ";
const std::string SearcherSQLite::IN = " IN ";
const std::string SearcherSQLite::SELECT = "SELECT ";
const std::string SearcherSQLite::FROM = " FROM ";
const std::string SearcherSQLite::BETWEEN = " BETWEEN ";
const std::string SearcherSQLite::LIKE = " LIKE ";
const std::string SearcherSQLite::OR = " OR ";

const int SearcherSQLite::DEFAULT_VALUE = -1;

SearcherSQLite::SearcherSQLite( const string& databasePath):
		filters(END_FILTERS),
		rowLimit(25){
	db = DatabaseConnector::getDatabase(databasePath);
	if ( db != NULL ){
		/*string errorMsg;
		string mainTableInfo = this->tableInfo( TABLE_MAIN, errorMsg );
		cout << "Main table: "<<mainTableInfo << endl;
		string speciesTableInfo = this->tableInfo( TABLE_SPECIES, errorMsg );
		cout << "Species table: "<<speciesTableInfo << endl;
		*/

		resultColumns[SPECIES_ID_COL] = TABLE_MAIN + PERIOD + SPECIES_ID_COLUMN;
		resultColumns[SPECIES_NAME_COL] = SPECIES_COLUMN;
		resultColumns[CHEMICAL_NAME_COL] = CHEMICAL_NAME_COLUMN;
		resultColumns[FREQUENCY_COL] = FREQUENCY_COLUMN;
		resultColumns[TEMPERATURE_COL]= TEMPERATURE_COLUMN;
		resultColumns[RESOLVED_QNS_COL] = RESOLVED_QNS_COLUMN;
		resultColumns[INTENSITY_COL] = INTENSITY_COLUMN;
		resultColumns[SMU2_COL] = SMU2_COLUMN;
		resultColumns[LOGA_COL] = LOGA_COLUMN;
		resultColumns[EL_COL] = EL_COLUMN;
		resultColumns[EU_COL] = EU_COLUMN;

		filterNames[FILTER_TOP_20] = FILTER_TOP20_COLUMN;
		filterNames[FILTER_PLANETARY_ATMOSPHERE] = FILTER_PLANET_COLUMN;
		filterNames[FILTER_HOT_CORES] = FILTER_HOTCORE_COLUMN;
		filterNames[FILTER_DARK_CLOUDS] = FILTER_DARKCLOUD_COLUMN;
		filterNames[FILTER_DIFFUSE_CLOUDS] = FILTER_DIFFUSECLOUD_COLUMN;
		filterNames[FILTER_COMETS]=FILTER_COMET_COLUMN;
		filterNames[FILTER_AGB_PPN_PN] = FILTER_AGB_PPN_PN_COLUMN;
		filterNames[FILTER_EXTRAGALACTIC] = FILTER_EXTRAGALACTIC_COLUMN;
	}
	else {
		cout << "Could not open database from path "<<databasePath.c_str()<<endl;
	}
	reset();
}

bool SearcherSQLite::isConnected() const {
	bool connected = false;
	if ( db != NULL ){
		connected = true;
	}
	return connected;
}

void SearcherSQLite::setSearchResultLimit( int limit ){
	rowLimit = limit;
}


//*********************************************************************
//                    Search Parameters
//*********************************************************************

void SearcherSQLite::reset(){
	chemicalNames.clear();
	speciesNames.clear();
	qns.clear();
	for ( int i = 0; i < static_cast<int>(filters.size()); i++ ){
		filters[i] = false;
	}
	minValueFreq = DEFAULT_VALUE;
	maxValueFreq = DEFAULT_VALUE;
	minValueIntensity = DEFAULT_VALUE;
	maxValueIntensity = DEFAULT_VALUE;
	minValueSmu2 = DEFAULT_VALUE;
	maxValueSmu2 = DEFAULT_VALUE;
	minValueLoga = DEFAULT_VALUE;
	maxValueLoga = DEFAULT_VALUE;
	minValueEl = DEFAULT_VALUE;
	maxValueEl = DEFAULT_VALUE;
	minValueEu = DEFAULT_VALUE;
	maxValueEu = DEFAULT_VALUE;
}

void SearcherSQLite::setChemicalNames( const vector<string>& chemNames ){
	chemicalNames = chemNames;
}

void SearcherSQLite::setSpeciesNames( const vector<string>& species ){
	speciesNames = species;
}

void SearcherSQLite::setFrequencyRange( double minVal, double maxVal ){
	if ( minVal < maxVal ){
		minValueFreq = minVal;
		maxValueFreq = maxVal;
	}
	else {
		minValueFreq = maxVal;
		maxValueFreq = minVal;
	}
}

void SearcherSQLite::setIntensityRange( double minValue, double maxValue ){
	if ( minValue < maxValue ){
		minValueIntensity = minValue;
		maxValueIntensity = maxValue;
	}
	else {
		minValueIntensity = maxValue;
		maxValueIntensity = minValue;
	}
}

void SearcherSQLite::setSmu2Range( double minValue, double maxValue ){
	if ( minValue < maxValue ){
		minValueSmu2 = minValue;
		maxValueSmu2 = maxValue;
	}
	else {
		minValueSmu2 = maxValue;
		maxValueSmu2 = minValue;
	}
}

void SearcherSQLite::setLogaRange( double minValue, double maxValue ){
	if ( minValue < maxValue ){
		minValueLoga = minValue;
		maxValueLoga = maxValue;
	}
	else {
		minValueLoga = maxValue;
		maxValueLoga = minValue;
	}
}

void SearcherSQLite::setElRange( double minValue, double maxValue ){
	if ( minValue < maxValue ){
		minValueEl = minValue;
		maxValueEl = maxValue;
	}
	else {
		minValueEl = maxValue;
		maxValueEl = minValue;
	}
}

void SearcherSQLite::setEuRange( double minValue, double maxValue ){
	if ( minValue < maxValue ){
		minValueEu = minValue;
		maxValueEu = maxValue;
	}
	else {
		minValueEu = maxValue;
		maxValueEu = minValue;
	}
}

void SearcherSQLite::setQNS( const vector<string>& qns ){
	this->qns = qns;
}

//*********************************************************************
//                     Filters
//*********************************************************************

void SearcherSQLite::setFilterTop20( bool filter ){
	filters[FILTER_TOP_20] = filter;
}
void SearcherSQLite::setFilterPlanetaryAtmosphere( bool filter ){
	filters[FILTER_PLANETARY_ATMOSPHERE] = filter;
}
void SearcherSQLite::setFilterHotCores( bool filter ){
	filters[FILTER_HOT_CORES] = filter;
}
void SearcherSQLite::setFilterDarkClouds( bool filter ){
	filters[FILTER_DARK_CLOUDS] = filter;
}
void SearcherSQLite::setFilterDiffuseClouds( bool filter ){
	filters[FILTER_DIFFUSE_CLOUDS] = filter;
}
void SearcherSQLite::setFilterComets( bool filter ){
	filters[FILTER_COMETS] = filter;
}
void SearcherSQLite::setFilterAgbPpnPn( bool filter ){
	filters[FILTER_AGB_PPN_PN] = filter;
}
void SearcherSQLite::setFilterExtragalactic( bool filter ){
	filters[FILTER_EXTRAGALACTIC] = filter;
}


//**********************************************************************
//                  Database Information
//**********************************************************************

string SearcherSQLite::tableInfo( const string& tableName, string& errorMsg ) const{
	string query;
	query.append( SELECT );
	query.append( "*");
	query.append( FROM );
	query.append( tableName );
	query.append(" LIMIT 1" );
	sqlite3_stmt* statement = NULL;
	bool successfulQuery = executeQuery( statement, query, errorMsg );
	string tableInfo;
	if ( successfulQuery ){
		//Returns one row for each column in the table
		int columnCount = sqlite3_column_count( statement );
		while( sqlite3_step( statement ) == SQLITE_ROW ){
			for ( int j = 0; j < columnCount; j++ ){
				const char* columnName = sqlite3_column_name( statement, j );
				int columnType = sqlite3_column_type( statement, j );
				string rowInfo( columnName );
				rowInfo.append( " ");
				rowInfo.append( numToString(columnType) );
				rowInfo.append( "\n");
				tableInfo.append( rowInfo );
			}
		}
	}
	else {
		errorMsg = sqlite3_errmsg( db );
		//cout << "Error obtaining table information "<<errorMsg<< endl;
	}
	return tableInfo;
}

string SearcherSQLite::getCreatedDate() const {
	return DatabaseConnector::getCreatedDate();
}



//************************************************************************
//                    Querying the Database
//************************************************************************

void SearcherSQLite::stopSearch(){
	sqlite3_interrupt( db );
}

long SearcherSQLite::doSearchCount( string& errorMsg ){
	string query = prepareQuery( true, 0 );
	sqlite3_stmt* statement;
	long resultCount = 0;
	bool success =  executeQuery( statement, query, errorMsg );
	if ( success ){
		int result = sqlite3_step( statement );
		if( result == SQLITE_ROW ){
			resultCount = sqlite3_column_int( statement, 0 );
		}
	}
	else {
		errorMsg = sqlite3_errmsg( db );
		cout << "Error was " << errorMsg << endl;
	}
	//cout << "Result count="<<resultCount<<endl;
	sqlite3_finalize(statement);
	return resultCount;
}

vector<SplatResult> SearcherSQLite::doSearch( string& errorMsg, int offset ){
	string query = prepareQuery( false, offset );
	sqlite3_stmt* statement;
	vector<SplatResult> results;
	bool success =  executeQuery( statement, query, errorMsg );
	if ( success ){
		while( sqlite3_step( statement ) == SQLITE_ROW ){

			int speciesId = sqlite3_column_int( statement, SPECIES_ID_COL);
			string chemicalName = reinterpret_cast<const char*>(sqlite3_column_text( statement, CHEMICAL_NAME_COL ));
			string speciesName = reinterpret_cast<const char*>(sqlite3_column_text( statement, SPECIES_NAME_COL ));
			double frequency = sqlite3_column_double( statement, FREQUENCY_COL );
			double temperature = sqlite3_column_double( statement, TEMPERATURE_COL );
			double smu2 = sqlite3_column_double( statement, SMU2_COL );
			double loga = sqlite3_column_double( statement, LOGA_COL );
			double el = sqlite3_column_double( statement, EL_COL );
			double eu = sqlite3_column_double( statement, EU_COL );
			string resolvedQNs = reinterpret_cast<const char*>(sqlite3_column_text( statement, RESOLVED_QNS_COL ));
			double intensity = sqlite3_column_double( statement, INTENSITY_COL );
			pair<double,string> freqPair( frequency, "MHz");
			pair<double,string> tempPair( temperature, "K");
			pair<double,string> elPair( el, "cm-1" );
			pair<double,string> euPair( eu, "cm-1" );
			SplatResult result( speciesId, speciesName, chemicalName, resolvedQNs,
						freqPair, tempPair, smu2, elPair, euPair, loga, intensity );
			results.push_back( result );
			//cout << result.toLine();
		}
	}
	else {
		errorMsg = sqlite3_errmsg( db );
		//cout << "Error was " << errorMsg << endl;
	}
	sqlite3_finalize(statement);
	return results;
}

bool SearcherSQLite:: executeQuery( sqlite3_stmt*& statement, const string& query,
		string& errorMsg ) const {
	bool success = true;
	int status =  sqlite3_prepare( db, query.c_str(), -1, &statement, NULL);
	if ( status != SQLITE_OK ){
		success = false;
		errorMsg = sqlite3_errmsg( db );
	}
	return success;
}

string SearcherSQLite::prepareQuery( bool countOnly, int offset ) const {
	std::string query;
	query.append( SELECT );
	if ( countOnly ){
		query.append( "Count()");
	}
	else {
		int resultColumnCount = resultColumns.size();
		for ( int i = 0; i < resultColumnCount; i++ ){
			query.append( resultColumns[i]);
			if ( i != resultColumnCount - 1 ){
				query.append( COMMA );
			}
		}
	}
	query.append( FROM );
	query.append( TABLE_MAIN );
	query.append( COMMA );
	query.append( TABLE_SPECIES );
	query.append( " WHERE ");

	//Join the two tables together based on the species id
	query.append( TABLE_MAIN );
	query.append( PERIOD );
	query.append( SPECIES_ID_COLUMN );
	query.append( " == " );
	query.append( TABLE_SPECIES );
	query.append( PERIOD );
	query.append( SPECIES_ID_COLUMN );

	//Frequency
	if ( minValueFreq >= 0 && maxValueFreq >= 0 && minValueFreq <= maxValueFreq ){
		query.append( getBetweenClause( FREQUENCY_COLUMN, minValueFreq, maxValueFreq ) );
	}



	//Species
	if (speciesNames.size() > 0) {
		//query.append( getInClause( SPECIES_COLUMN, speciesNames ) );
		query.append( getLikeClause( SPECIES_COLUMN, speciesNames) );
	}

	//Chemical Names
	if (chemicalNames.size() > 0) {
		query.append( getInClause( CHEMICAL_NAME_COLUMN, chemicalNames ));
	}

	//Resolved Quantum Numbers
	if (qns.size() > 0) {
		query.append( getInClause( RESOLVED_QNS_COLUMN, qns ));
	}

	//Intensity
	if (minValueIntensity < maxValueIntensity) {
		query.append( getBetweenClause( INTENSITY_COLUMN, minValueIntensity, maxValueIntensity));
	}

	//SMU2
	if ( minValueSmu2 < maxValueSmu2 ) {
		query.append( getBetweenClause( SMU2_COLUMN, minValueSmu2, maxValueSmu2));
	}

	//LOGA
	if ( minValueLoga < maxValueLoga ) {
		query.append( getBetweenClause( LOGA_COLUMN, minValueLoga, maxValueLoga ) );
	}

	//EL
	if ( minValueEl < maxValueEl ) {
		query.append( getBetweenClause( EL_COLUMN, minValueEl, maxValueEl ));
	}

	//EU
	if ( minValueEu < maxValueEu ) {
		query.append( getBetweenClause( EU_COLUMN, minValueEu, maxValueEu ));
	}

	//Astronomical Filters
	for ( int i = 0; i < static_cast<int>(filters.size()); i++ ){
		if ( filters[i] ){
			query.append( AND );
			query.append( filterNames[i]);
			query.append( EQUALS );
			query.append( getTrue());
		}
	}


	if ( !countOnly ){
		if ( rowLimit > 0 ){
			query.append( " LIMIT ");
			query.append( numToString(rowLimit) );
			if ( offset > 0 ){
				query.append( " OFFSET ");
				query.append( numToString(offset) );
			}
		}
	}
	//cout << "Query: "<<query.c_str()<<endl;
	return query;
}



//******************************************************************
//                       Utility
//******************************************************************
string SearcherSQLite::numToString( double number ) const {
	string result;
	ostringstream convert;
	convert << number;
	result = convert.str();
	return result;
}


std::string numToString( float val ){
	std::stringstream stream;
	std::string str;
	stream << val;
	str = stream.str();
	return str;
}


std::string SearcherSQLite::getTrue() const {
	return "1";
}

string SearcherSQLite::getBetweenClause( const string& columnName,
		double low, double high) const {
	string betweenClause;
	betweenClause.append( AND );
	betweenClause.append( OPEN_PAREN );
	betweenClause.append( columnName );
	betweenClause.append( BETWEEN );
	betweenClause.append( numToString( low ) );
	betweenClause.append( AND );
	betweenClause.append( numToString( high ) );
	betweenClause.append( CLOSE_PAREN );
	return betweenClause;
}



string SearcherSQLite::getInClause( const string& columnName, const vector<string>& values ) const {
	string inClause;
	inClause.append( AND );
	inClause.append( OPEN_PAREN );
	inClause.append( columnName );
	inClause.append( IN );
	inClause.append( OPEN_PAREN );
	int valueCount = values.size();
	for (int i=0; i<valueCount; i++) {
		inClause.append( SINGLE_QUOTE );
		inClause.append( values[i] );
		inClause.append( SINGLE_QUOTE );
		if (i != valueCount - 1) {
			inClause.append( ", ");
		}
	}
	inClause.append( CLOSE_PAREN );
	inClause.append( CLOSE_PAREN );
	return inClause;
}


string SearcherSQLite::getLikeClause( const string& columnName, const vector<string>& values ) const {
	string likeClause;
	likeClause.append( AND );
	likeClause.append( OPEN_PAREN );

	//This was written so that for users searching by chemical formula,
	//they could enter 'CO' and return entries such as 'CO v=0' and
	//'CO v=1', which are in the species column.
	int valueCount = values.size();
	for (int i=0; i<valueCount; i++) {
		likeClause.append( columnName );
		likeClause.append( LIKE );
		likeClause.append( SINGLE_QUOTE );
		likeClause.append( values[i]);
		likeClause.append( SINGLE_QUOTE );

		likeClause.append( OR );
		likeClause.append( columnName );
		likeClause.append( LIKE );
		likeClause.append( SINGLE_QUOTE );
		likeClause.append( values[i] + " %");
		likeClause.append( SINGLE_QUOTE );
	}
	likeClause.append( CLOSE_PAREN );
	return likeClause;
}


SearcherSQLite::~SearcherSQLite() {
	// TODO Auto-generated destructor stub
}

}
