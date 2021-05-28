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

#include <spectrallines/Splatalogue/ListConverter.h>

#include <casa/Quanta/Quantum.h>
#include <casa/Quanta/Unit.h>
#include <casa/OS/RegularFile.h>
#include <casa/IO/RegularFileIO.h>
#include <casa/Utilities/Regex.h>
#include <tables/Tables/ScaColDesc.h>
#include <tables/Tables/ScalarColumn.h>
#include <tables/Tables/SetupNewTab.h>
#include <tables/DataMan/StandardStMan.h>
#include <tables/Tables/TableDesc.h>

using namespace casacore;
namespace casa {

ListConverter::ListConverter(const Vector<String>& filenames, const String& table) :
	_log(new LogIO()), _listFiles(Vector<RegularFile>(0)), _tableName(table), _freqUnit("") {
	RegularFile t(_tableName);
	if (t.exists()) {
		*_log << "File " << _tableName << " already exists and will not be overwritten" << LogIO::EXCEPTION;
	}
	File::FileWriteStatus tableStatus = t.getWriteStatus();
	if(tableStatus == File::NOT_CREATABLE ) {
		*_log << "Table " << _tableName << " cannot be created" << LogIO::EXCEPTION;
	}
	for (Vector<String>::const_iterator iter = filenames.begin(); iter != filenames.end(); iter++) {
		RegularFile file(*iter);
		if (! file.exists()) {
			*_log << "File " << *iter << " does not exist" << LogIO::EXCEPTION;
		}
		if (! file.isReadable()) {
			*_log << "File " << *iter << " does not have read permission" << LogIO::EXCEPTION;
		}
		_listFiles.resize(_listFiles.size()+1, true);
		_listFiles[_listFiles.size() - 1] = file;
	}
}

ListConverter::~ListConverter() {
	delete _log;
}

SplatalogueTable* ListConverter::load() {
	_parseLists();
	SplatalogueTable *newTable = _defineTable(_species.size());
	_addData(newTable);
	newTable->flush();
	return newTable;
}

void ListConverter::_parseLists() {
	_log->origin(LogOrigin("ListConverter", __FUNCTION__));

	uInt transitionIndex = 0;
	Regex blankLine("^[ \n\t\r\v\f]+$");
	Regex lastLine("^Found [0-9]+ rows");
	Regex debye2("D<sup>2</sup>");
	Regex k("(K)");
	Regex cm1("(cm<sup>-1</sup>)");

	_frequency.resize(0);
	for (
		Vector<RegularFile>::const_iterator iter = _listFiles.begin();
		iter != _listFiles.end(); iter++
	) {
		RegularFileIO fileIO(*iter);
		Int bufSize = 10000;
		char *buffer = new char[bufSize];
		int nRead;
		String contents;
		while ((nRead = fileIO.read(bufSize, buffer, false)) == bufSize) {
			String chunk(buffer, bufSize);
			contents += chunk;
		}
		// get the last chunk
		String chunk(buffer, nRead);
		contents += chunk;

		Vector<String> lines = stringToVector(contents, '\n');
		uInt newSize = _frequency.size() + lines.size();
		_species.resize(newSize, true);
		_chemName.resize(newSize, true);
		_qns.resize(newSize, true);
		_lineList.resize(newSize, true);
		_recommended.resize(newSize, true);
		_frequency.resize(newSize, true);
		_intensity.resize(newSize, true);
		_smu2.resize(newSize, true);
		_logA.resize(newSize, true);
		_eL.resize(newSize, true);
		_eU.resize(newSize, true);
		uInt lineCount = 0;
		for(Vector<String>::iterator liter=lines.begin(); liter!=lines.end(); liter++) {
			if (
				liter->empty() || liter->firstchar() == '#'
				|| liter->matches(blankLine) || liter->matches(lastLine)
			) {
				// skip comment
				lineCount++;
				continue;
			}
			uInt tabCount = liter->freq('\t');
			if (tabCount != 10) {
				*_log << "bad format for line " << (lineCount+1)
					<< " in file " << iter->path().baseName() << LogIO::EXCEPTION;
			}
			Vector<String> parts = stringToVector(*liter, '\t');
			for (Vector<String>::iterator viter = parts.begin(); viter != parts.end(); viter++) {
				viter->trim();
			}
			String filename = iter->path().dirName() + "/" + iter->path().baseName();
			String species = parts[0];
			String recommended = parts[1];
			String chemicalName = parts[2];
			String frequency = parts[3];
			String qns = parts[4];
			String intensity = parts[5];
			String smu2 = parts[6];
			String logA = parts[7];
			String eL = parts[8];
			String eU = parts[9];
			String lineList = parts[10];
			if (lineCount == 0) {
				// header line look for units
				Vector<String> freqHeaderParts = stringToVector(frequency, ' ');
				for (
					Vector<String>::const_iterator viter=freqHeaderParts.begin();
					viter!=freqHeaderParts.end(); viter++
				) {
					try {
						Unit freqUnit(*viter);
						Quantity qq(1, freqUnit);
						if (qq.isConform(Unit("Hz"))) {
							_freqUnit = freqUnit.getName();
						}
					}
					catch (AipsError x) {}
				}
				Vector<String> smuHeaderParts = stringToVector(smu2, ' ');
				for (
					Vector<String>::const_iterator viter=smuHeaderParts.begin();
					viter!=smuHeaderParts.end(); viter++
				) {
					try {
						String smu2(*viter);
						if (smu2.matches(debye2)) {
							_smu2Unit = "Debye*Debye";

						}
					}
					catch (AipsError x) {}
				}
				Vector<String> elHeaderParts = stringToVector(eL, ' ');
				for (
					Vector<String>::const_iterator viter=elHeaderParts.begin();
					viter!=elHeaderParts.end(); viter++
				) {
					try {
						String el = *viter;
						if (el.matches(k)) {
							_elUnit = "K";
						}
						else if (el.matches(cm1)) {
							_elUnit = "cm-1";
						}
					}
					catch (AipsError x) {}
				}
				Vector<String> euHeaderParts = stringToVector(eU, ' ');
				for (
					Vector<String>::const_iterator viter=euHeaderParts.begin();
					viter!=euHeaderParts.end(); viter++
				) {
					try {
						String eu = *viter;
						if (eu.matches(k)) {
							_euUnit = "K";
						}
						else if (eu.matches(cm1)) {
							_euUnit = "cm-1";
						}
					}
					catch (AipsError x) {}
				}
				lineCount++;
				continue;
			}

			if (lineCount % 20000 == 0) {
				*_log << LogIO::NORMAL << "Parsing line " << lineCount << " of file "
					<< iter->path().baseName() << LogIO::POST;
			}
			Bool recFreq = recommended.length() > 0;
			if (! frequency.matches(RXdouble) ) {
				*_log << "File " << filename << ", line " << lineCount
					<< ": frequency value " << frequency << " is not numeric"
					<< LogIO::EXCEPTION;
			}
			if (! intensity.matches(RXdouble) ) {
				*_log << "File " << filename << ", line " << lineCount
					<< ": intensity value " << intensity << " is not numeric"
					<< LogIO::EXCEPTION;
			}
			if (! smu2.matches(RXdouble) ) {
			*_log << "File " << filename << ", line " << lineCount
				<< ": S_ij*mu**2 value " << smu2 << " is not numeric"
				<< LogIO::EXCEPTION;
			}
			if (! logA.matches(RXdouble) ) {
			*_log << "File " << filename << ", line " << lineCount
				<< ": log(A) value " << logA << " is not numeric"
				<< LogIO::EXCEPTION;
			}
			if (! eL.matches(RXdouble) ) {
				*_log << "File " << filename << ", line " << lineCount
					<< ": E_l value " << eL << " is not numeric"
					<< LogIO::EXCEPTION;
			}
			if (! eU.matches(RXdouble) ) {
				*_log << "File " << filename << ", line " << lineCount
					<< ": E_u value " << eU << " is not numeric"
					<< LogIO::EXCEPTION;
			}
			_species[transitionIndex] = species;
			_chemName[transitionIndex] = chemicalName;
			_qns[transitionIndex] = qns;
			_lineList[transitionIndex] = lineList;
			_recommended[transitionIndex] = recFreq;
			_frequency[transitionIndex] = String::toDouble(frequency);
			_intensity[transitionIndex] = String::toFloat(intensity);
			_smu2[transitionIndex] = String::toFloat(smu2);
			_logA[transitionIndex] = String::toFloat(logA);
			_eL[transitionIndex] = String::toFloat(eL);
			_eU[transitionIndex] = String::toFloat(eU);
			transitionIndex++;
			lineCount++;
		}
	}
	_species.resize(transitionIndex, true);
	_chemName.resize(transitionIndex, true);
	_qns.resize(transitionIndex, true);
	_lineList.resize(transitionIndex, true);
	_recommended.resize(transitionIndex, true);
	_frequency.resize(transitionIndex, true);
	_intensity.resize(transitionIndex, true);
	_smu2.resize(transitionIndex, true);
	_logA.resize(transitionIndex, true);
	_eL.resize(transitionIndex, true);
	_eU.resize(transitionIndex, true);
}


SplatalogueTable* ListConverter::_defineTable(const uInt nrows) {
	String descName = _tableName + "_desc";
	TableDesc td(descName,TableDesc::Scratch);
	td.addColumn (ScalarColumnDesc<String>(SplatalogueTable::SPECIES, SplatalogueTable::SPECIES));
	td.addColumn (ScalarColumnDesc<Bool>(SplatalogueTable::RECOMMENDED, SplatalogueTable::RECOMMENDED));
	td.addColumn (ScalarColumnDesc<String>(SplatalogueTable::CHEMICAL_NAME, SplatalogueTable::CHEMICAL_NAME));
	td.addColumn (ScalarColumnDesc<Double>(SplatalogueTable::FREQUENCY, SplatalogueTable::FREQUENCY));
	td.addColumn (ScalarColumnDesc<String>(SplatalogueTable::QUANTUM_NUMBERS, SplatalogueTable::QUANTUM_NUMBERS));
	td.addColumn (ScalarColumnDesc<Float>(SplatalogueTable::INTENSITY, SplatalogueTable::INTENSITY));
	td.addColumn (ScalarColumnDesc<Float>(SplatalogueTable::SMU2, SplatalogueTable::SMU2));
	td.addColumn (ScalarColumnDesc<Float>(SplatalogueTable::LOGA, SplatalogueTable::LOGA));
	td.addColumn (ScalarColumnDesc<Float>(SplatalogueTable::EL, SplatalogueTable::EL));
	td.addColumn (ScalarColumnDesc<Float>(SplatalogueTable::EU, SplatalogueTable::EU));
	td.addColumn (ScalarColumnDesc<String>(SplatalogueTable::LINELIST, SplatalogueTable::LINELIST));
	SetupNewTable tableSetup(_tableName, td, Table::New);
	StandardStMan stm;
	tableSetup.bindColumn(SplatalogueTable::SPECIES, stm);
	tableSetup.bindColumn(SplatalogueTable::RECOMMENDED, stm);
	tableSetup.bindColumn(SplatalogueTable::CHEMICAL_NAME, stm);
	tableSetup.bindColumn(SplatalogueTable::FREQUENCY, stm);
	tableSetup.bindColumn(SplatalogueTable::QUANTUM_NUMBERS, stm);
	tableSetup.bindColumn(SplatalogueTable::INTENSITY, stm);
	tableSetup.bindColumn(SplatalogueTable::SMU2, stm);
	tableSetup.bindColumn(SplatalogueTable::LOGA, stm);
	tableSetup.bindColumn(SplatalogueTable::EL, stm);
	tableSetup.bindColumn(SplatalogueTable::EU, stm);
	tableSetup.bindColumn(SplatalogueTable::LINELIST, stm);
	if (_freqUnit.empty()) {
		*_log << LogIO::WARN << "Could not determine frequency unit, assuming GHz" << LogIO::POST;
		_freqUnit = "GHz";
	}
	return new SplatalogueTable(tableSetup, nrows, _freqUnit, _smu2Unit, _elUnit, _euUnit);
}

void ListConverter::_addData(const SplatalogueTable* table) const {
	ScalarColumn<String> col(*table, SplatalogueTable::SPECIES);
	col.putColumn(_species);
	ScalarColumn<Bool> rcol(*table, SplatalogueTable::RECOMMENDED);
	rcol.putColumn(_recommended);
	ScalarColumn<String> cncol(*table, SplatalogueTable::CHEMICAL_NAME);
	cncol.putColumn(_chemName);
	ScalarColumn<Double> fcol(*table,SplatalogueTable::FREQUENCY);
	fcol.putColumn(_frequency);
	ScalarColumn<String> qncol(*table, SplatalogueTable::QUANTUM_NUMBERS);
	qncol.putColumn(_qns);
	ScalarColumn<Float> icol(*table, SplatalogueTable::INTENSITY);
	icol.putColumn(_intensity);
	ScalarColumn<Float> smu2col(*table, SplatalogueTable::SMU2);
	smu2col.putColumn(_smu2);
	ScalarColumn<Float> lacol(*table, SplatalogueTable::LOGA);
	lacol.putColumn(_logA);
	ScalarColumn<Float> elcol(*table, SplatalogueTable::EL);
	elcol.putColumn(_eL);
	ScalarColumn<Float> eucol(*table, SplatalogueTable::EU);
	eucol.putColumn(_eU);
	ScalarColumn<String> llcol(*table, SplatalogueTable::LINELIST);
	llcol.putColumn(_lineList);
}

} //# NAMESPACE CASA - END

