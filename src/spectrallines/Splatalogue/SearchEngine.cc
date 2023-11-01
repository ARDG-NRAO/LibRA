//# MSContinuumSubtractor.cc:  Subtract continuum from spectral line data
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

#include <spectrallines/Splatalogue/SearchEngine.h>

#include <casacore/casa/Containers/Record.h>
#include <casacore/casa/IO/FiledesIO.h>

#include <casacore/casa/OS/File.h>
#include <imageanalysis/ImageAnalysis/ImageInputProcessor.h>
#include <casacore/tables/Tables/ScalarColumn.h>
#include <casacore/tables/TaQL/TableParse.h>
#include <fcntl.h>
#include <memory>

#include <iostream>
using namespace std;

using namespace casacore;
namespace casa {

SearchEngine::SearchEngine(
	const SplatalogueTable* const table, const Bool list,
	const String& logfile, const Bool append
) : _log(new LogIO), _table(table), _logfile(logfile),
	_list(list), _append(append){
	if (!logfile.empty()) {
        OutputDestinationChecker::OutputStruct logfile;
        logfile.label = "logfile";
        logfile.outputFile = &_logfile;
        logfile.required = true;
        logfile.replaceable = true;
        vector<OutputDestinationChecker::OutputStruct> output(1);
        output[0] = logfile;
        OutputDestinationChecker::checkOutputs(&output, *_log);
	}
}

SearchEngine::~SearchEngine() {
	delete _log;
}

SplatalogueTable* SearchEngine::search(
	const String& resultsTableName, const Double freqLow, const Double freqHigh,
	const Vector<String>& species, const Bool recommendedOnly,
	const Vector<String>& chemNames, const Vector<String>& qns,
	const Double intensityLow, const Double intensityHigh,
	const Double smu2Low, const Double smu2High,
	const Double logaLow, const Double logaHigh,
	const Double elLow, const Double elHigh,
	const Double euLow, const Double euHigh,
	const Bool includeRRLs, const Bool onlyRRLs
) const {
	LogOrigin origin("SearchEngine", __FUNCTION__);
	*_log << origin;
	if (! resultsTableName.empty()) {
		File tab(resultsTableName);
		if (! tab.canCreate()) {
			*_log << "Cannot create table " << resultsTableName << LogIO::EXCEPTION;
		}
	}
	ostringstream query;
	query << "SELECT FROM "  << _table->tableName();
	query << " WHERE ";

	query << "(" << _getBetweenClause(SplatalogueTable::FREQUENCY, freqLow, freqHigh) << ")";
	if (species.size() > 0) {
		query << " AND (" << SplatalogueTable::SPECIES << " IN (";
		for (uInt i=0; i<species.size(); i++) {
			query << "'" << species[i] << "'";
			if (i != species.size() - 1) {
				query << ", ";
			}
		}
		query << "))";
	}

	if (recommendedOnly) {
		query << " AND (" << SplatalogueTable::RECOMMENDED << ")";
	}

	if (chemNames.size() > 0) {
		query << " AND (" << SplatalogueTable::CHEMICAL_NAME << " IN (";
		for (uInt i=0; i<chemNames.size(); i++) {
			query << "'" << chemNames[i] << "'";
			if (i != chemNames.size() - 1) {
				query << ", ";
			}
		}
		query << "))";
	}
	if (qns.size() > 0) {
		query << " AND (" << SplatalogueTable::QUANTUM_NUMBERS << " IN (";
		for (uInt i=0; i<qns.size(); i++) {
			query << "'" << qns[i] << "'";
			if (i != qns.size() - 1) {
				query << ", ";
			}
		}
		query << "))";
	}
	String rrlPortion = (includeRRLs || onlyRRLs)
		? "LINELIST = 'Recomb'"
		: "";
	ostringstream nonRRLPortion;
	if (! onlyRRLs) {
		nonRRLPortion << "(LINELIST != 'Recomb')";
		if (intensityLow < intensityHigh) {
			nonRRLPortion << " AND " << _getBetweenClause(
				SplatalogueTable::INTENSITY, intensityLow, intensityHigh
			);
		}
		if (smu2Low < smu2High) {
			nonRRLPortion << " AND " << _getBetweenClause(
				SplatalogueTable::SMU2, smu2Low, smu2High
			);
		}
		if (logaLow < logaHigh) {
			nonRRLPortion << " AND " << _getBetweenClause(
				SplatalogueTable::LOGA, logaLow, logaHigh
			);
		}
		if (elLow < elHigh) {
			nonRRLPortion << " AND " << _getBetweenClause(
				SplatalogueTable::EL, elLow, elHigh
			);
		}
		if (euLow < euHigh) {
			nonRRLPortion << " AND " << _getBetweenClause(
				SplatalogueTable::EU, euLow, euHigh
			);
		}
	}
	if (onlyRRLs) {
		query << " AND " << rrlPortion;
	}
	else if (includeRRLs) {
		query << " AND ((" << rrlPortion << ") OR (" << nonRRLPortion.str() << "))";
	}
	else {
		query << " AND " << nonRRLPortion.str();
	}
	query << " ORDER BY " << SplatalogueTable::FREQUENCY;

	Table resTable = _runQuery(query.str());
	std::unique_ptr<SplatalogueTable> resSplatTable(new SplatalogueTable(resTable));
	if (!resultsTableName.empty()) {
		resSplatTable->rename(resultsTableName, Table::NewNoReplace);
		resSplatTable->flush(true, true);
	}
	if (_list) {
		_logIt(resSplatTable->list());
	}
	return resSplatTable.release();
}

Vector<String> SearchEngine::uniqueSpecies() const {
	String query = "SELECT UNIQUE(" + SplatalogueTable::SPECIES
		+ ") FROM " + _table->tableName() + " ORDER BY " + SplatalogueTable::SPECIES;
	// Table resTable = tableCommand(query).table();
	Table resTable = _runQuery(query);

	String logString;
	ScalarColumn<String> species(resTable, SplatalogueTable::SPECIES);
	Vector<String> vSpecies(species.nrow());
	for (uInt i=0; i<species.nrow(); i++) {
		vSpecies[i] = species(i);
		logString += species(i) + "\n";
	}
	_logIt(logString);
	return vSpecies;
}

Vector<String> SearchEngine::uniqueChemicalNames() const {
	String query = "SELECT UNIQUE(" + SplatalogueTable::CHEMICAL_NAME
		+ ") FROM " + _table->tableName() + " ORDER BY " + SplatalogueTable::CHEMICAL_NAME;
	// Table resTable = tableCommand(query).table();
	Table resTable = _runQuery(query);

	ScalarColumn<String> chemNames(resTable, SplatalogueTable::CHEMICAL_NAME);
	Vector<String> vChemNames(chemNames.nrow());
	String logString;
	for (uInt i=0; i<chemNames.nrow(); i++) {
		vChemNames[i] = chemNames(i);
		logString += chemNames(i) + "\n";
	}
	_logIt(logString);
	return vChemNames;
}

String SearchEngine::_getBetweenClause(
	const String& col, const Double low, const Double high
) const {
	ostringstream os;
	os << col << " BETWEEN " << low << " AND " << high;
	return os.str();
}

Table SearchEngine::_runQuery(const String& query) const {
	String tablename = _table->tableName();
	File file(tablename);
	Bool dump = false;
	String queryCopy = query;
	if (! file.exists()) {
		*_log << LogIO::NORMAL << "Flushing a copy of " << tablename << " to disk so it can be queried" << LogIO::POST;
		Path newName = File::newUniqueName(".");
		uInt pos = query.find(tablename);
		uInt length = tablename.length();
		tablename = newName.absoluteName();
		_table->deepCopy(tablename, Table::Scratch, true, Table::AipsrcEndian, false);
		dump = true;
		cout << "query " << query << endl;
		cout << "new table " << tablename << endl;
		String begin = query.substr(0, pos);
		String end = query.substr(pos+length, query.length());
		queryCopy = begin + tablename + end;
		cout << "new query " << queryCopy << endl;

	}
	Table resTable = tableCommand(queryCopy).table();
	if (dump) {
		Table t(tablename);
		t.markForDelete();
		//*_log << LogIO::NORMAL << "Removing temporary disk copy " << tablename << LogIO::POST;
		//Table::deleteTable(tablename, true);
	}
	return resTable;
}

void SearchEngine::_logIt(const String& logString) const {
	LogOrigin origin("SearchEngine", __FUNCTION__);
	*_log << origin << logString << LogIO::POST;
	if (! _list) {
		return;
	}
	if (! _logfile.empty()) {
    	File log(_logfile);
    	switch (File::FileWriteStatus status = log.getWriteStatus()) {
    	case File::OVERWRITABLE:
    		if (_append) {
    			Int fd = open(_logfile.c_str(), O_RDWR | O_APPEND);
    			FiledesIO fio(fd, _logfile.c_str());
    			fio.write(logString.length(), logString.c_str());
    			FiledesIO::close(fd);
    			*_log << LogIO::NORMAL << "Appended results to file "
    					<< _logfile << LogIO::POST;
    		}
    		// no break here to fall through to the File::CREATABLE block if logFileAppend is false
    	case File::CREATABLE:
    		if (status == File::CREATABLE || ! _append) {
    			// can fall through from previous case block so status can be File::OVERWRITABLE
    			String action = (status == File::OVERWRITABLE) ? "Overwrote" : "Created";
    			Int fd = FiledesIO::create(_logfile.c_str());
    			FiledesIO fio (fd, _logfile.c_str());
    			fio.write(logString.length(), logString.c_str());
    			FiledesIO::close(fd);
    			*_log << LogIO::NORMAL << action << " file "
    					<< _logfile << " with new log file"
    					<< LogIO::POST;
    		}
    		break;
    	default:
    		// checks to see if the log file is not creatable or not writeable should have already been
    		// done and if so _logFileName set to the empty string so this method wouldn't be called in
    		// those cases.
    		*_log << "Programming logic error. This block should never be reached" << LogIO::EXCEPTION;
    	}
    }

}


} //# NAMESPACE CASA - END

