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
#include "SearcherFactory.h"
#include <spectrallines/Splatalogue/Searcher.h>
#include <spectrallines/Splatalogue/SQLiteSearch/SearcherSQLite.h>
#include <casatools/Config/State.h>
#include <casa/Logging/LogIO.h>
#include <iostream>
using namespace std;

using namespace casacore;
namespace casa {

String SearcherFactory::getLocation( bool local ) {
	String defaultDatabasePath;
	if ( local ){

		//Note:: When the SQLite code (and database get put into its
		//own library, finding the location of the database should be
		//removed from here and put into the DatabaseConnector.
		const string splatdb = "ephemerides/splatalogue.db";
		Bool foundDatabase = false;
		string resolvepath;
		resolvepath = casatools::get_state( ).resolve(splatdb);
		if ( resolvepath != splatdb ) {
			foundDatabase = true;
			defaultDatabasePath = resolvepath;
		} else {
			const string splatable = "ephemerides/SplatDefault.tbl";
			foundDatabase = Aipsrc::find(defaultDatabasePath, "user.ephemerides.SplatDefault.tbl");
			if( !foundDatabase ){
				foundDatabase = Aipsrc::findDir(defaultDatabasePath, "data/" + splatable);
			}

			if ( foundDatabase ) {
				const String tableName = "SplatDefault.tbl";
				int index = defaultDatabasePath.find(tableName, 0);
				int tableNameSize = tableName.length();
				defaultDatabasePath.replace(index, tableNameSize, "splatalogue.db");
			}
		}
	}

	static bool logged = false;
	if ( logged == false ) {
		logged = true;
		LogIO os (LogOrigin ("SearcherFactory", "getLocation"));
		if ( defaultDatabasePath.size( ) == 0 ) {
			os << LogIO::WARN <<
			  "could not find splatalog offline database" <<
			  LogIO::POST;
		} else {
			os <<
			  "found splatalog offline database: " <<
			  defaultDatabasePath <<
			  LogIO::POST;
		}
	}
	return defaultDatabasePath;
}

Searcher* SearcherFactory::getSearcher( bool local ){
	Searcher* searcher = NULL;
	if ( local ){
		String defaultDatabasePath = getLocation( local );
		if (! defaultDatabasePath.empty()){
			searcher = new SearcherSQLite(defaultDatabasePath.c_str() );
			if ( !searcher->isConnected()){
				delete searcher;
				searcher = NULL;
			}
		}
	}
	else {
		cout << "Only local database searches are currently supported"<<endl;
	}
	return searcher;
}

SearcherFactory::SearcherFactory(){
}

SearcherFactory::~SearcherFactory(){
}

using namespace casacore;
} /* namespace casa */
