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

#include "DatabaseConnector.h"
#include <sys/stat.h>
#include <sqlite3.h>
#include <iostream>
using namespace std;

namespace casa {

DatabaseConnector* DatabaseConnector::connection = NULL;
bool DatabaseConnector::successfulOpen = true;

string DatabaseConnector::databasePath = "";

DatabaseConnector::DatabaseConnector( const string& path ) {
	databasePath = path;
	int rc = sqlite3_open( databasePath.c_str(), &db );
	if ( rc != SQLITE_OK ){
		successfulOpen = false;
		cout << "Problem opening database: "<<sqlite3_errmsg(db) << endl;
		sqlite3_close( db );
	}
}

string DatabaseConnector::getCreatedDate(){
	struct tm* clock;
	struct stat attrib;
	stat(databasePath.c_str(), &attrib );
	clock = gmtime(&(attrib.st_mtime));
	char* timeStr = asctime( clock );
	string fileDate( timeStr );
	return fileDate;
}

sqlite3* DatabaseConnector::getDatabase( const string& path){
	if ( connection == NULL ){
		connection = new DatabaseConnector( path );
	}
	sqlite3* dbConnection = NULL;
	if ( successfulOpen ){
		dbConnection =connection->db;
	}
	return dbConnection;
}

DatabaseConnector::~DatabaseConnector() {
	if ( db != NULL ){
		sqlite3_close( db );
	}
}

}
