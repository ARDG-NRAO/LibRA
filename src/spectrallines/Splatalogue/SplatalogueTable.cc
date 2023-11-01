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

#include <spectrallines/Splatalogue/SplatalogueTable.h>

#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/Quanta/MVTime.h>
#include <casacore/tables/TaQL/ExprNode.h>

#include <casacore/tables/Tables/ScaColDesc.h>
#include <casacore/tables/Tables/ScalarColumn.h>
#include <iomanip>

#include <iostream>
using namespace std;

using namespace casacore;
namespace casa {

const String SplatalogueTable::SPECIES = "SPECIES";
const String SplatalogueTable::RECOMMENDED = "RECOMMENDED";
const String SplatalogueTable::CHEMICAL_NAME = "CHEMICAL_NAME";
const String SplatalogueTable::FREQUENCY = "FREQUENCY";
const String SplatalogueTable::QUANTUM_NUMBERS = "QUANTUM_NUMBERS";
const String SplatalogueTable::INTENSITY = "INTENSITY";
const String SplatalogueTable::SMU2 = "SMU2";
const String SplatalogueTable::LOGA = "LOGA";
const String SplatalogueTable::EL = "EL";
const String SplatalogueTable::EU = "EU";
const String SplatalogueTable::LINELIST = "LINELIST";
const String SplatalogueTable::ISSPLAT = "isSplat";

const String SplatalogueTable::RECORD_VALUE = "value";
const String SplatalogueTable::RECORD_UNIT = "unit";
const String SplatalogueTable::RECORD_SPECIES = "species";
const String SplatalogueTable::RECORD_RECOMMENDED = "recommended";
const String SplatalogueTable::RECORD_CHEMNAME = "chemname";
const String SplatalogueTable::RECORD_FREQUENCY = "freq";
const String SplatalogueTable::RECORD_QNS = "qns";
const String SplatalogueTable::RECORD_INTENSITY = "intensity";
const String SplatalogueTable::RECORD_SMU2 = "smu2";
const String SplatalogueTable::RECORD_LOGA = "loga";
const String SplatalogueTable::RECORD_EL = "el";
const String SplatalogueTable::RECORD_EU = "eu";
const String SplatalogueTable::RECORD_LINE_LIST = "linelist";

SplatalogueTable::SplatalogueTable(
	SetupNewTable& snt, uInt nrow,
	const String& freqUnit, const String& smu2Unit,
	const String& elUnit, const String& euUnit  
) : Table(snt, nrow), _freqUnit(freqUnit), _smu2Unit(smu2Unit),
	 _elUnit(elUnit), _euUnit(euUnit) {
	_construct(true);
}

SplatalogueTable::SplatalogueTable(
	const String& tablename
) : Table(tablename) {
	_construct(false);
}

SplatalogueTable::SplatalogueTable(
	const Table& table
) : Table(table) {
	_construct(false);
}

String SplatalogueTable::getFrequencyUnit() const {
	return _freqUnit;
}

String SplatalogueTable::list() const {
	ScalarColumn<String> species(*this, SPECIES);
	ScalarColumn<Bool> recommended(*this, RECOMMENDED);
	ScalarColumn<String> chemName(*this, CHEMICAL_NAME);
	ScalarColumn<Double> freq(*this, FREQUENCY);
	ScalarColumn<String> qns(*this, QUANTUM_NUMBERS);
	ScalarColumn<Float> intensity(*this, INTENSITY);
	ScalarColumn<Float> smu2(*this, SMU2);
	ScalarColumn<Float> logA(*this, LOGA);
	ScalarColumn<Float> el(*this, EL);
	ScalarColumn<Float> eu(*this, EU);
	ScalarColumn<String> linelist(*this, LINELIST);

	char cspecies[15], crec[11], cchemName[21], cfreq[12], cqns[21],
		cintensity[10], csmu2[10], clogA[10], cel[10], ceu[10],
		clinelist[11];
	ostringstream os;
	String rec;
	os << SPECIES << "       " << RECOMMENDED << "        "
		<< CHEMICAL_NAME << "   " << FREQUENCY << "     "
		<< QUANTUM_NUMBERS << "  " << INTENSITY << "      "
		<< SMU2 << "      " << LOGA << "        "
		<< EL  << "        " << EU << "  " << LINELIST << endl;
    for (uInt i=0; i<nrow(); i++) {
    	sprintf(cspecies, "%-14.14s", species.asString(i).chars());
    	rec = recommended.asBool(i) ? "*" : " ";
    	sprintf(crec, "%10.10s", rec.chars());
    	sprintf(cchemName, "%-20.20s", chemName.asString(i).chars());
    	sprintf(cfreq, "%11.6f", freq.asdouble(i));
    	sprintf(cqns, "%-20.20s", qns.asString(i).chars());
    	sprintf(cintensity, "%9.5f", intensity.asfloat(i));
    	sprintf(csmu2, "%9.5f", smu2.asfloat(i));
    	sprintf(clogA, "%9.5f", logA.asfloat(i));
    	sprintf(cel, "%9.5f", el.asfloat(i));
    	sprintf(ceu, "%9.5f", eu.asfloat(i));
    	sprintf(clinelist, "%-10.10s", linelist.asString(i).chars());

    	os << cspecies << " " << crec << " " << cchemName
    		<< " " << cfreq << " " << cqns << " " << cintensity
    		<< " " << csmu2 << " " << clogA << " " << cel
    		<< " " << ceu << "  " << clinelist << endl;
	}
	return os.str();
}

namespace {
template <typename T>
class Kluge {

// The latest casacore mode changed the signature to the TableExprNode::getColumnTYPE
// methods.  Since SplatalogueTable::toRecord is the only code that is using them,
// I've installed a very limited kluge to fix this issue.  FYI: The casacore mod changed
// the casacore mode now requires an array of row numbers rather than defaulting to all
// rows(?).  jjacobs 12/17/12


public:

typedef Array<T> (TableExprNode::* Extractor) (const RowNumbers& rownrs) const;

static Array<T>
extract (const TableExprNode & col, Extractor extractor)
{
  Vector<rownr_t> rownrs (col.nrow());
  indgen (rownrs);

  return ((& col) ->* extractor) (RowNumbers(rownrs));

}

};
}

Record SplatalogueTable::toRecord() const {

	Array<String> species = Kluge<String>::extract (col(SPECIES), & TableExprNode::getColumnString);
	Array<Bool> recommended = Kluge<Bool>::extract (col(RECOMMENDED), & TableExprNode::getColumnBool);
	Array<String> chemName = Kluge<String>::extract (col(CHEMICAL_NAME), & TableExprNode::getColumnString);
	Array<Double> freq = Kluge<Double>::extract (col(FREQUENCY), & TableExprNode::getColumnDouble);
	Array<String> qns = Kluge<String>::extract (col(QUANTUM_NUMBERS), & TableExprNode::getColumnString);
	Array<Float> intensity = Kluge<Float>::extract (col(INTENSITY), & TableExprNode::getColumnFloat);
	Array<Float> smu2 = Kluge<Float>::extract (col(SMU2), & TableExprNode::getColumnFloat);
	Array<Float> loga = Kluge<Float>::extract (col(LOGA), & TableExprNode::getColumnFloat);
	Array<Float> el = Kluge<Float>::extract (col(EL), & TableExprNode::getColumnFloat);
	Array<Float> eu = Kluge<Float>::extract (col(EU), & TableExprNode::getColumnFloat);
	Array<String> linelist = Kluge<String>::extract (col(LINELIST), & TableExprNode::getColumnString);

//	Array<String> species = col(SPECIES).getColumnString();
//	Array<Bool> recommended = col(RECOMMENDED).getColumnBool();
//	Array<String> chemName = col(CHEMICAL_NAME).getColumnString();
//	Array<Double> freq = col(FREQUENCY).getColumnDouble();
//	Array<String> qns = col(QUANTUM_NUMBERS).getColumnString();
//	Array<Float> intensity = col(INTENSITY).getColumnFloat();
//	Array<Float> smu2 = col(SMU2).getColumnFloat();
//	Array<Float> loga = col(LOGA).getColumnFloat();
//	Array<Float> el = col(EL).getColumnFloat();
//	Array<Float> eu = col(EU).getColumnFloat();
//	Array<String> linelist = col(LINELIST).getColumnString();

	IPosition idx = IPosition(1, 0);
	Record rec;
	Record qFreq;
	qFreq.define(RECORD_VALUE, 0.0);
	qFreq.define(RECORD_UNIT, _freqUnit);
	Record qSmu2;
	qSmu2.define(RECORD_VALUE, 0.0f);

	qSmu2.define(RECORD_UNIT, _smu2Unit);
	Record qel = qSmu2;
	qel.define(RECORD_UNIT, _elUnit);
	Record qeu = qSmu2;
	qeu.define(RECORD_UNIT, _euUnit);
	for (uInt i=0; i<species.size(); i++) {
		idx[0] = i;
		Record line;
		line.define(RECORD_SPECIES, species(idx));
		line.define(RECORD_RECOMMENDED, recommended(idx));
		line.define(RECORD_CHEMNAME, chemName(idx));
		qFreq.define(RECORD_VALUE, freq(idx));
		line.defineRecord(RECORD_FREQUENCY, qFreq);
		line.define(RECORD_QNS, qns(idx));
		line.define(RECORD_INTENSITY, intensity(idx));
		qSmu2.define(RECORD_VALUE, smu2(idx));
		line.defineRecord(RECORD_SMU2, qSmu2);
		line.define(RECORD_LOGA, loga(idx));
		qel.define(RECORD_VALUE, el(idx));
		line.defineRecord(RECORD_EL, qel);
		qeu.define(RECORD_VALUE, eu(idx));
		line.defineRecord(RECORD_EU, qeu);
		line.define(RECORD_LINE_LIST, linelist(idx));
		rec.defineRecord("*" + String::toString(i), line);
	}
	return rec;
}

void SplatalogueTable::_construct(const Bool setup) {
	if (
		! setup
		&& (
			! keywordSet().isDefined(ISSPLAT)
			|| ! keywordSet().asBool(ISSPLAT)
		)
	) {
		throw AipsError("Table is not a splatalogue table");
	}
	Vector<String> colNames = tableDesc().columnNames();
	Vector<String> reqColNames(11);
	if (colNames.size() != reqColNames.size()) {
		throw AipsError("Table does not have the required number of columns to be a Splatalogue table");
	}

	reqColNames[0] = SPECIES;
	reqColNames[1] = RECOMMENDED;
	reqColNames[2] = CHEMICAL_NAME;
	reqColNames[3] = FREQUENCY;
	reqColNames[4] = QUANTUM_NUMBERS;
	reqColNames[5] = INTENSITY;
	reqColNames[6] = SMU2;
	reqColNames[7] = LOGA;
	reqColNames[8] = EL;
	reqColNames[9] = EU;
	reqColNames[10] = LINELIST;
	for (
		Vector<String>::const_iterator riter=reqColNames.begin();
			riter!=reqColNames.end(); riter++
	) {
		Bool found = false;
		for (
			Vector<String>::const_iterator iter=colNames.begin();
				iter!=colNames.end(); iter++
			) {
			if (*iter == *riter) {
				found = true;
				break;
			}
		}
		if (! found) {
			throw AipsError("Column " + *riter + " does not exist in the input table.");
		}
	}
	// TODO also check column data types
	if (setup) {
		Bool writable = isWritable();
		if (! writable) {
			reopenRW();
		}
		_addKeywords();
	}
	else {
		ScalarColumn<Double> freq(*this, FREQUENCY);
		_freqUnit = freq.keywordSet().asString("Unit");
		ScalarColumn<Float> smu2(*this, SMU2);
		_smu2Unit = smu2.keywordSet().asString("Unit");
		if (_smu2Unit.empty()) {
			_smu2Unit = "Debye2";
		}
		ScalarColumn<Float> el(*this, EL);
		_elUnit = el.keywordSet().asString("Unit");
		if (_elUnit.empty()) {
			_elUnit = "K";
		}
		ScalarColumn<Float> eu(*this, EU);
		_euUnit = eu.keywordSet().asString("Unit");
		if (_euUnit.empty()) {
			_euUnit = "K";
		}
	}
}


void SplatalogueTable::_addKeywords() {
	if (_freqUnit.empty()) {
		_freqUnit = "GHz";
	}
	ScalarColumn<Double> freq(*this, FREQUENCY);
	freq.rwKeywordSet().define("Unit", _freqUnit);
	ScalarColumn<Float> smu2(*this, SMU2);
	smu2.rwKeywordSet().define("Unit", _smu2Unit);
	ScalarColumn<Float> el(*this, EL);
	el.rwKeywordSet().define("Unit", _elUnit);
	ScalarColumn<Float> eu(*this, EU);
	eu.rwKeywordSet().define("Unit", _euUnit);
	rwKeywordSet().define(ISSPLAT, true);
	Time now;
	MVTime mv(now);
	mv.setFormat(MVTime::YMD, 3);
	String timeString = mv.string();
	timeString = String(timeString.before(timeString.size() - 1));
	// These are required by the standard measures loading code.
	rwKeywordSet().define("VS_CREATE", timeString);
	rwKeywordSet().define("VS_DATE", timeString);
	rwKeywordSet().define("VS_VERSION", timeString);
	rwKeywordSet().define("VS_TYPE", "List of spectral lines in Splatalogue format");
	tableInfo().setType("IERS");
}

} //# NAMESPACE CASA - END

