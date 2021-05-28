//# CalSummary.cc:  Helper class for listing a CalTable
//# Copyright (C) 2018
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
//#

#include <synthesis/CalTables/CalSummary.h>
#include <synthesis/CalTables/CalMainColumns.h>
#include <synthesis/CalTables/CalDescColumns.h>
#include <synthesis/CalTables/CalIter.h>
#include <synthesis/CalTables/CalBuffer.h>
#include <synthesis/CalTables/CalSetMetaInfo.h>
#include <synthesis/MeasurementComponents/MSMetaInfoForCal.h>

#include <casacore/casa/Quanta/QVector.h>
#include <casacore/casa/iomanip.h>
#include <casacore/casa/iostream.h>
#include <casacore/casa/OS/Path.h>
#include <casacore/measures/Measures/MeasTable.h>
#include <casacore/ms/MSOper/MSMetaData.h>
#include <casacore/tables/Tables/TableRecord.h>
#include <casacore/casa/Utilities/GenSort.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//
// Constructor assigns pointer.  If CT goes out of scope you
// will get rubbish.  Also sets string to separate subtable output.
//
CalSummary::CalSummary (CalTable& ct)
: pCT(&ct),
  msname_p(""),
  haveMS_p(false),
  dashlin1(replicate("-",80)),
  dashlin2(replicate("=",80))
{ setMSname(); }

CalSummary::CalSummary (CalTable* ct)
: pCT(ct), 
  msname_p(""),
  haveMS_p(false),
  dashlin1(replicate("-",80)),
  dashlin2(replicate("=",80))
{ setMSname(); }

CalSummary::~CalSummary () { }


//
// Retrieve number of rows
//
Int CalSummary::nrow () const
{
    return pCT->nRowMain();
}


//
// Get cal table name
//
const String CalSummary::name () const
{
    return pCT->tableName();
}


//
// Reassign pointer.
//
Bool CalSummary::setCT (CalTable& ct)
{
    CalTable* pTemp;
    pTemp = &ct;
    if (pTemp == 0) {
        return False;
    } else {
        pCT = pTemp;
        setMSname();
        return True;
    }
}

void CalSummary::setMSname() {
  ROCalDescColumns calDescCol(*pCT);
  Path filepath(name());
  String msname(calDescCol.msName()(0)), path(filepath.dirName());
  if (!path.empty())
    path += "/";
  String fullmsname = path + msname;
  if (!msname.empty() && Table::isReadable(fullmsname)) {
    msname_p = fullmsname;
    haveMS_p = true;
  } else {
    haveMS_p = false;
  }
}

//
// List information about a ct to the logger
//
void CalSummary::list (LogIO& os, Bool verbose) const
{
    // List a title for the Summary
    listTitle (os);
    // List the main table and subtables in a useful order and format
    listWhere(os, verbose);
    os << dashlin1 << endl;
    listWhat(os, verbose);
    os << dashlin1 << endl;
    listHow(os, verbose);
    os << dashlin1 << endl;
    // Post it
    os.post();
}

//
// List a title for the Summary
//
void CalSummary::listTitle (LogIO& os) const
{
    os << LogIO::NORMAL;
    // Type of cal table
    casacore::String type("unknown");
    os << dashlin2 << endl << "Cal Table Name: " << this->name();
    type = pCT->type();
    os << "   Cal Type: " << type;
    os << endl << dashlin2 << endl;
}


//
// Convenient table groupings
//
void CalSummary::listWhere (LogIO& os, Bool verbose) const
{
    listObservation (os,verbose);
}

void CalSummary::listWhat (LogIO& os, Bool verbose) const
{
    listMain(os, verbose);
    os << dashlin1 << endl;
    listField(os, verbose);
}

void CalSummary::listHow (LogIO& os, Bool verbose) const
{
    listSpectralWindow(os, verbose);
    os << dashlin1 << endl;
    listAntenna(os, verbose);
}

//
// SUBTABLES
//
void CalSummary::listMain (LogIO& os, Bool verbose) const
{
    if (nrow() <= 0) {
        os << "The MAIN table is empty: there are no data!!!" << endl << LogIO::POST;
        return;
    }
    // to get columns from main and subtables
    ROCalMainColumns ctmain(*pCT);
    ROCalDescColumns ctdesc(*pCT);

    // Time:
    casacore::Vector<casacore::Double> times = ctmain.time().getColumn();
    casacore::Double startTime = times(0);
    casacore::Double stopTime = times(times.size()-1);
    casacore::Double exposTime = stopTime - startTime;
    casacore::String timeref = ctmain.time().keywordSet().subRecord("MEASINFO").asString("Ref");
    // Output info
    os << "  Data records: " << nrow() << "       Total elapsed time = "
       << exposTime << " seconds" << endl
       << "   Observed from   "
       << MVTime(startTime/C::day).string(MVTime::DMY,7)
       << "   to   "
       << MVTime(stopTime/C::day).string(MVTime::DMY,7)
       << " (" << timeref << ")"
       << endl << endl;
    os << LogIO::POST;

    if (verbose) {
        // Field names from MS:
        casacore::Vector<casacore::String> fieldnames;
        if (haveMS_p) {
          MSMetaInfoForCal msmeta(msname_p);
          msmeta.fieldNames(fieldnames);
        }
        // Spw Ids from CAL_DESC table
        casacore::Array<casacore::Int> spwids(ctdesc.spwId().getColumn());

        // Field widths for printing:
        casacore::Int width_lead(2),
            width_scan(4),
            width_time(22),
            width_fieldId(5),
            width_fieldname(20),
            width_nrow(10);

        // Sort order
        casacore::Vector<casacore::Int> icols(3);
        icols[0] = MSC::OBSERVATION_ID;
        icols[1] = MSC::SCAN_NUMBER;
        icols[2] = MSC::FIELD_ID;
        CalIter* iter = new CalIter(*pCT, icols);
        CalBuffer* buff = new CalBuffer(*iter);
        // values from main table
        casacore::Vector<casacore::Int> obs = buff->obsId();
        casacore::Vector<casacore::Double> times = buff->time();
        casacore::Vector<casacore::Int> scans = buff->scanNo();
        casacore::Vector<casacore::Int> fieldIds = buff->fieldId();
        casacore::Vector<casacore::Double> intervals = buff->interval();
        casacore::Vector<casacore::Int> calDescIds = buff->calDescId();
        casacore::Int lastobs(-2);     // print obsid when changes
        casacore::Double lastday(0.0), lastTime(0.0); // print date when changes
        casacore::Int lastScan(-2), lastField(-2), lastCaldesc(-1), lastInt(-1);
        casacore::Int rowTotal(1);
        delete iter;
        delete buff;
        for (uInt i=0; i<obs.size(); ++i) {
            Int thisobs = obs(i);
            // print heading for each obs id
            if (thisobs != lastobs) {
                lastobs = thisobs;
                lastday = 0.0; // force printing date
                os << endl << "   ObservationID = " << thisobs << endl;
                os << LogIO::POST;
                // print header for output columns
                casacore::String datetime = "  Date        Time ( ";
                // variable-len timeref string
                datetime.replace(20,timeref.length(),timeref);
                datetime.replace(20+timeref.length(),1,")");
                os << datetime;
                os << "  Scan  FldId FieldName           nRows       ";
                os << "SpwIds   Interval" << endl;
                os << LogIO::POST;
            }

            // get values for this iteration
            Double time = times(i);
            casacore::Double day = floor(MVTime(time/C::day).day());
            casacore::Int scan = scans(i);
            casacore::Int fieldid = fieldIds(i);
            casacore::Int caldescid = calDescIds(i);
            casacore::Int interval = intervals(i);

            bool lastrow = (i==obs.size()-1);
            // print line when something changes and at end
            if ((time!=lastTime) || (scan!=lastScan) || (fieldid!=lastField) ||
                (caldescid!=lastCaldesc) || (interval!=lastInt) || lastrow) {
              if (rowTotal>1) { // but not first row
                os.output().precision(3);
                os.output().setf(ios::right, ios::adjustfield);
                os.output().width(width_lead); os << "  ";
                // Timerange beginning time
                os.output().width(width_time);
                if (day!=lastday) { // print date
                  os << MVTime(lastTime/C::day).string(MVTime::DMY,7);
                } else {            // omit date
                  os << MVTime(lastTime/C::day).string(MVTime::TIME,7);
                }
                // Scan
                os.output().width(width_lead); os << "  ";
                os.output().setf(ios::right, ios::adjustfield);
                os.output().width(width_scan);
                os << lastScan;
                // FldId
                os.output().width(width_lead); os << "  ";
                os.output().setf(ios::right, ios::adjustfield);
                os.output().width(width_fieldId);
                casacore::String fieldname("");
                os << lastField << " ";
                if (lastField >= 0 && !fieldnames.empty())
                    fieldname = fieldnames(lastField);
                // FieldName
                os.output().setf(ios::left, ios::adjustfield);
                if (fieldname.length()>20)
                  fieldname.replace(19,1,'*');
                os.output().width(width_fieldname);
                os << fieldname.at(0,20);
                // nRows
                os.output().width(width_nrow);
                os.output().setf(ios::left, ios::adjustfield);
                if (!lastrow) --rowTotal;
                os << rowTotal;
                // SpwIds
                os.output().width(width_lead); os << "  ";
                Slicer spwSlicer = Slicer(Slice(), Slice(lastCaldesc));
                casacore::Vector<casacore::Int> spws = spwids(spwSlicer);
                os << spws;
                // Interval
                os.output().width(width_lead); os << "  ";
                os << "   " << interval;
                os << endl;
                os << LogIO::POST;

                lastday = day; // next lastday is this day
                rowTotal = 1;  // restart counter
              }
              lastTime = time;
              lastScan = scan;
              lastField = fieldid;
              lastCaldesc = caldescid;
              lastInt = interval;
            }
            ++rowTotal;
        }
        os << LogIO::POST;
    }
    clearFormatFlags(os);
}

void CalSummary::listAntenna (LogIO& os, Bool verbose) const
{
    // Get unique antenna ids  present in the main table
    ROCalMainColumns ctmain(*pCT);
    std::vector<casacore::Int> ant1col = ctmain.antenna1().getColumn().tovector();
    std::set<casacore::Int> antIds = std::set<casacore::Int>(ant1col.begin(), ant1col.end());
    uInt nAnt = antIds.size();
    os << "Antennas: " << nAnt << endl;

    // Antenna info from MS
    casacore::Vector<casacore::String> antnames, stations;
    if (!haveMS_p) {
      os << "MS is not available for detailed antenna listing" << LogIO::POST;
    } else {
      MSMetaInfoForCal msmeta(msname_p);
      msmeta.antennaNames(antnames);
      stations = msmeta.msmd().getAntennaStations();

      if (verbose) {
        // Detailed antenna list
        casacore::String indent("  ");
        casacore::uInt indwidth(5),
            namewidth(6),
            statwidth(10),
            diamwidth(5),
            latwidth(13),
            longwidth(14),
            offsetwidth(14),
            positionwidth(17);
        casacore::Int diamprec(1);

        // Antenna info from MS
        MSMetaInfoForCal msmeta(msname_p);
        QVector<Double> diameters = msmeta.msmd().getAntennaDiameters();
        vector<MPosition> positions = msmeta.msmd().getAntennaPositions();
        vector<QVector<Double> > offsets = msmeta.msmd().getAntennaOffsets();
        Bool posIsITRF = (positions[0].type() == MPosition::ITRF);

        // Get observatory position info for antenna offsets 
        // *not read from antenna table OFFSET column!*
        casacore::MPosition obsPos;
        casacore::String telname;
        bool doOffset = getObservatoryPosition(obsPos, telname);
        casacore::Double rObs = 0.0, longObs = 0.0, latObs = 0.0;
        if (!doOffset) {
            os << "Warning: Telescope name '" << telname << "' is not recognized by CASA.  Cannot compute offsets." << endl << endl;
        } else {
            casacore::Vector<casacore::Double> obsXYZ = obsPos.get("m").getValue();
            casacore::Double xo(obsXYZ[0]), yo(obsXYZ[1]), zo(obsXYZ[2]);
            rObs = sqrt(xo*xo + yo*yo + zo*zo);
            casacore::Vector<casacore::Double> obsLongLat = obsPos.getAngle("rad").getValue();
            longObs = obsLongLat[0];
            latObs = obsLongLat[1];
        }

        os.output().setf(ios::fixed, ios::floatfield);
        os.output().setf(ios::left, ios::adjustfield);
        // Write the column headings:
        os << indent;
        os.output().width(indwidth);    os << "ID";
        os.output().width(namewidth);   os << "Name";
        os.output().width(statwidth);   os << "Station";
        os.output().width(diamwidth+4); os << "Diam.";
        os.output().width(longwidth);   os << "Long.";
        os.output().width(latwidth);    os << "Lat.";
        if (doOffset) {
            os.output().width(3 * offsetwidth);
            os << "       Offset from array center (m)";
        }
        os.output().width(3 * positionwidth);
        os << "         ITRF Geocentric coordinates (m)";
        os << endl << indent;
        os.output().width(indwidth + namewidth + statwidth + diamwidth + 4 + longwidth + latwidth);
        os << " ";
        os.output().setf(ios::right, ios::adjustfield);
        if (doOffset) {
            os.output().width(offsetwidth); os << "East";
            os.output().width(offsetwidth); os << "North";
            os.output().width(offsetwidth); os << "Elevation";
        }
        os.output().width(positionwidth); os << "x";
        os.output().width(positionwidth); os << "y";
        os.output().width(positionwidth); os << "z";
        os << endl;
        // Iterator through antennas and log info for each
        const static Unit diamUnit("m");
        std::set<Int>::const_iterator iter = antIds.begin();
        std::set<Int>::const_iterator end = antIds.end();
        for (; iter!=end; ++iter) {
          Int antId = *iter;
          // diameter
          const Quantity& diam = diameters[antId];
          // xyz
          casacore::Vector<casacore::Double> xyz = positions[antId].get("m").getValue();
          casacore::Double x = xyz(0);
          casacore::Double y = xyz(1);
          casacore::Double z = xyz(2);
          casacore::Double rAnt = sqrt(x*x + y*y + z*z);
          // offsets
          const MPosition& mLongLat = positions[antId];
          casacore::Vector<casacore::Double> antOffset;
          if (doOffset) {
              casacore::Vector<casacore::Double> longLatRad = mLongLat.getAngle("rad").getValue();
              casacore::Double longAnt = longLatRad(0);
              casacore::Double latAnt = longLatRad(1);
              casacore::Vector<casacore::Double> offset(3);
              offset[0] = (longAnt - longObs) * rObs * cos(latObs);
              offset[1] = (latAnt - latObs) * rObs;
              offset[2] = rAnt - rObs;
              casacore::QVD qoffset(offset, "m");
              antOffset = qoffset.getValue("m");
          }
          // lat/long
          MVAngle mvLong = mLongLat.getAngle().getValue()(0);
          MVAngle mvLat = mLongLat.getAngle().getValue()(1);
          if (!posIsITRF) {
              MeasConvert<MPosition> toItrf(positions[antId], MPosition::ITRF);
              positions[antId] = toItrf(positions[antId]);
          }
          // write the row
          os.output().setf(ios::left, ios::adjustfield);
          os << indent;
          // ID, Name, Station
          os.output().width(indwidth);  os << antId;
          os.output().width(namewidth); os << antnames(antId);
          os.output().width(statwidth); os << stations(antId);
          // Diam.
          os.output().precision(diamprec);
          os.output().width(diamwidth);
          os << diam.getValue(diamUnit) << "m   ";
          // Long. and Lat.
          os.output().width(longwidth);
          os << mvLong.string(MVAngle::ANGLE,7);
          os.output().width(latwidth);
          os << mvLat.string(MVAngle::DIG2,7);
          if (doOffset) {
              // Offset from array center (m) (East, North, Elevation)
              os.output().setf(ios::right, ios::adjustfield);
              os.output().precision(4);
              os.output().width(offsetwidth); os << antOffset(0);
              os.output().width(offsetwidth); os << antOffset(1);
              os.output().width(offsetwidth); os << antOffset(2);
          }
          // ITRF Geocentric coordinates (m) (x, y, z)
          os.output().precision(6);
          os.output().width(positionwidth); os << x;
          os.output().width(positionwidth); os << y;
          os.output().width(positionwidth); os << z;
          os << endl;
        }
      } else {
        // Horizontal list of the stations names:
	    os << " ('name'@'station') " <<  endl;
        casacore::String line, leader;
        // track last id of previous line for leader
        casacore::Int lastIdInLine = *antIds.begin() - 1;
        std::set<casacore::Int>::const_iterator iter = antIds.begin();
        std::set<casacore::Int>::const_iterator end = antIds.end();
        casacore::Int maxAnt = *std::max_element(antIds.begin(), antIds.end());
        for (; iter!=end; ++iter) {
            Int antId = *iter;
            // Build the line
            if (!antnames.empty()) 
                line = line + "'" + antnames(antId) + "'" + "@";
            if (!stations.empty()) 
                line = line + "'" + stations(antId) + "'";
            // Add comma if not at the end
            if (antId != maxAnt)  line = line + ", ";
            if (line.length() > 55 || antId == maxAnt) {
                // This line is finished, dump it after the line leader
                leader = String::toString(lastIdInLine+1) + "-" + String::toString(antId) +": ";
                os << "   ID=";
                os.output().setf(ios::right, ios::adjustfield);
                os.output().width(8); os << leader;
                os << line << endl;
                line = "";
                lastIdInLine = antId;
            }
        }
    }
  }
  os << LogIO::POST;
  clearFormatFlags(os);
}

bool CalSummary::getObservatoryPosition(casacore::MPosition& obspos,
        casacore::String& name) const {
    // Returns observatory position in ITRF
    bool obsposOk(false);
    MSMetaInfoForCal msmeta(msname_p);
    // get telescope name
    name = msmeta.msmd().getObservatoryNames()[0];
    obsposOk = MeasTable::Observatory(obspos, name);
    if (obsposOk && (obspos.type() != MPosition::ITRF)) {
        MeasConvert<MPosition> toItrf(obspos, MPosition::ITRF);
        obspos = toItrf(obspos);
    }
    return obsposOk;
}

void CalSummary::listField (LogIO& os, Bool /*verbose*/) const
{
  // Get unique field ids  present in the main table
  ROCalMainColumns ctmain(*pCT);
  std::vector<casacore::Int> fieldcol = ctmain.fieldId().getColumn().tovector();
  std::set<casacore::Int> fieldIds = std::set<casacore::Int>(fieldcol.begin(), fieldcol.end());
  uInt nfield = fieldIds.size();

  os << "Fields: " << nfield << endl;
  if (!haveMS_p)
      os << "MS is not available for detailed field listing" << LogIO::POST;
  else {
    casacore::Int widthLead(2),
        widthField(6),
        widthCode(5),
        widthName(20),
        widthRA(16),
        widthDec(16),
        widthType(8),
        widthSrc(6);

    // Print heading
    os.output().setf(ios::left, ios::adjustfield);
    os.output().width(widthLead);   os << "  ";
    os.output().width(widthField);  os << "FldId";
    if (haveMS_p) {
      os.output().width(widthCode);   os << "Code";
      os.output().width(widthName);   os << "Name";
      os.output().width(widthRA);     os << "RA";
      os.output().width(widthDec);    os << " Decl";
      os.output().width(widthType);   os << "Epoch";
      os.output().width(widthSrc);    os << "SrcId";
    }
    os << endl;

    // get values from MS
    casacore::Vector<casacore::String> fieldNames, codes;
    vector<MDirection> phaseDirs;
    vector<Int> sourceIds;
    if (haveMS_p) {
      MSMetaInfoForCal msmeta(msname_p);
      msmeta.fieldNames(fieldNames);
      codes = msmeta.msmd().getFieldCodes();
      phaseDirs = msmeta.msmd().getPhaseDirs();
      sourceIds = msmeta.msmd().getFieldTableSourceIDs();
    } 

    // loop through fields
    std::set<Int>::const_iterator fiter = fieldIds.begin();
    std::set<Int>::const_iterator fend = fieldIds.end();
    static const MEpoch ezero(Quantity(0, "s"));
    for (; fiter!=fend; ++fiter) {
        Int fieldId = *fiter;
        if (fieldId >=0) {
            os.output().setf(ios::left, ios::adjustfield);
            os.output().width(widthLead);  os << "  ";
            os.output().width(widthField); os << fieldId;
            if (haveMS_p) {
              casacore::MDirection mRaDec = phaseDirs[fieldId];
              MVAngle mvRa = mRaDec.getAngle().getValue()(0);
              MVAngle mvDec = mRaDec.getAngle().getValue()(1);
              String name = fieldNames(fieldId);
              if (name.length()>20) 
                name.replace(19,1,"*");
              os.output().width(widthCode);  os << codes(fieldId);
              os.output().width(widthName);  os << name.at(0,20);
              os.output().width(widthRA);    os << mvRa(0.0).string(MVAngle::TIME,12);
              os.output().width(widthDec);   os << mvDec.string(MVAngle::DIG2,11);
              os.output().width(widthType);
              os << MDirection::showType(mRaDec.getRefPtr()->getType());
              os.output().width(widthSrc);  os << sourceIds[fieldId];
            }
            os << endl;
        } else {
            os.output().setf(ios::left, ios::adjustfield);
            os.output().width(widthLead);  os << "  ";
            os.output().width(widthField); os << fieldId;
            os << endl;
        }
    }
    os << LogIO::POST;
  }
  clearFormatFlags(os);
}

void CalSummary::listObservation (LogIO& os, Bool /*verbose*/) const
{
    // Get unique obs ids  present in the main table
    ROCalMainColumns ctmain(*pCT);
    std::vector<casacore::Int> obscol = ctmain.obsId().getColumn().tovector();
    std::set<casacore::Int> obsIds = std::set<casacore::Int>(obscol.begin(), obscol.end());
    os << "Observation IDs in main table: " << obsIds << endl;

    Int nrow = pCT->nRowObservation();
    if (nrow <= 0) {
        os << "The OBSERVATION table is empty";
    } else {
        os << "Observation table entries: " << nrow << LogIO::POST;
        for (Int irow=0 ; irow < nrow; irow++) {
            Record obsRow = pCT->getRowObservation(irow);
            String observer = obsRow.asString("OBSERVER");
            if (!observer.empty())
                os << "   Observer: " << observer;
            String project = obsRow.asString("PROJECT");
            if (!project.empty())
                os << "   Project: " << project;
            String telescope = obsRow.asString("TELESCOPE_NAME");
            if (!telescope.empty())
                os << "   Telescope: " << telescope;
			// TIME_RANGE values don't match main table or ms!
        }
    }
    os << LogIO::POST;
    clearFormatFlags(os);
}

void CalSummary::listHistory (LogIO& os) const
{
    Int nrow = pCT->nRowHistory();
    if (nrow <= 0) {
        os << "The HISTORY table is empty" << endl;
    } else {
        os << "History table entries: " << nrow << endl << LogIO::POST;
        for (Int irow=0 ; irow < nrow; irow++) {
            Record histRow = pCT->getRowHistory(irow);
            os << "ROW " << irow << "  ";
            os << "PARMS: " << histRow.asString("CAL_PARMS") << endl;
            os << "TABLES: " << histRow.asString("CAL_TABLES") << endl;
            os << "SELECT: " << histRow.asString("CAL_SELECT") << endl;
            os << "NOTES: " << histRow.asString("CAL_NOTES") << endl;
        }
    }
    os << LogIO::POST;
}

void CalSummary::listSpectralWindow (LogIO& os, Bool /*verbose*/) const
{
    ROCalDescColumns ctdesc(*pCT);
	casacore::Array<casacore::Int> spwIdArray = ctdesc.spwId().getColumn();
	IPosition vshape(1, spwIdArray.nelements());
	casacore::Vector<casacore::Int> spwIds(spwIdArray.reform(vshape));
    Int nSpws = GenSort<Int>::sort(spwIds, Sort::Ascending, Sort::NoDuplicates);
    spwIds.resize(nSpws, true);
    os << "Spectral Windows: " << nSpws;
	Bool noSpws = (nSpws==1 && spwIds(0)==-1);
	if (noSpws)
      os << " " << spwIds;
	os << endl;

	if (!noSpws) {
	  if (haveMS_p) {
        MSMetaInfoForCal msmeta(msname_p);
        vector<MFrequency> refFreqs = msmeta.msmd().getRefFreqs();
	    vector<uInt> nChans = msmeta.msmd().nChans();
	    vector<Double> totalBWs = msmeta.msmd().getBandWidths();
	    vector<QVD> resolution = msmeta.msmd().getChanResolutions(False);

        // Header: SpwId  Ref.Freq    #Chans  Resolution  TotalBW
        casacore::Int widthLead(2), widthId(7), widthFreq(13), widthFrqNum(7), widthNumChan(8);
        os.output().setf(ios::left, ios::adjustfield);
        os.output().width(widthLead);    os << "  ";
        os.output().width(widthId); os << "SpwId";
        os.output().width(widthFreq);    os << "Ref.Freq";
        os.output().width(widthNumChan); os << "#Chans";
        os.output().width(widthFreq);    os << "Resolution";
        os.output().width(widthFreq);    os << "TotalBW";
        os << endl;

        for (Int ispw=0; ispw<nSpws; ispw++) {
		  Int spwId = spwIds(ispw);
          os.output().setf(ios::left, ios::adjustfield);
          os.output().width(widthLead); os << "  ";
          os.output().width(widthId); os << spwId;
		  if (spwId >= 0) {
            os.output().width(widthFrqNum);
            os << refFreqs[spwId].get("MHz");   // MFreqency
            os.output().width(widthLead); os << "  ";
            os.output().width(widthNumChan);
            os << nChans[spwId];
            os.output().width(widthFrqNum);
            os << resolution[spwId][0].get("kHz");  // QVD
            os.output().width(widthLead);    os << "  ";
            os.output().width(widthLead);    os << "  ";
            os.output().width(widthFrqNum);
            os << totalBWs[spwId] / 1000.0;  // convert Hz to kHz
			os << " kHz";
            os << endl;
          }
        }
      } else {
        os << "MS is not available for detailed spw listing" << endl;
	  }
	}
    os << LogIO::POST;
    clearFormatFlags(os);
}

void CalSummary::listTables (LogIO& os, Bool verbose) const
{
    // Get nrows for each table (=-1 if table absent)
    uInt nTables = 4;
    casacore::Vector<casacore::Int> tableRows(nTables);
    tableRows(0) = pCT->nRowMain();
    tableRows(1) = pCT->nRowDesc();
    tableRows(2) = pCT->nRowHistory();
    tableRows(3) = pCT->nRowObservation();

    casacore::Vector<casacore::String> tableStrings(nTables);
    tableStrings(0) = "MAIN";
    tableStrings(1) = "CAL_DESC";
    tableStrings(2) = "CAL_HISTORY";
    tableStrings(3) = "OBSERVATION";

    // Just to make things read better
    casacore::Vector<casacore::String> rowStrings(6);
    rowStrings = " rows";
    for (uInt i=0; i<nTables; i++) {
        if (tableRows(i)==1) rowStrings(i) = " row";
        // if table exists, but empty:
        if (tableRows(i)==0) rowStrings(i) = " <empty>";
        // if table absent:
        if (tableRows(i)==-1) rowStrings(i) = "<absent>";
    }

    if (verbose) {
        os << "   Tables" << endl;
        for (uInt i=0; i<nTables; i++) {
            os.output().setf(ios::left, ios::adjustfield);
            os.output().width(3);  os << "   ";
            os.output().width(20); os << tableStrings(i);
            os.output().setf(ios::right, ios::adjustfield);
            os.output().width(8);  os << tableRows(i);
            os.output().setf(ios::left, ios::adjustfield);
            os.output().width(10); os << rowStrings(i);
            os << endl;
        }
    } else {
        os << "   Tables (rows):    (-1 = table absent)" << endl;
        for (uInt i=0; i<nTables; i++) {
            casacore::String tableinfo = tableStrings(i) + " (" +
                casacore::String::toString(tableRows(i)) + ")";
            os.output().setf(ios::left, ios::adjustfield);
            casacore::Int infowidth = 15;
            if (i==1 || i==4) infowidth = 25;
            os.output().width(infowidth);
            os << tableinfo;
            if ((i+1) % 3==0) os << endl;
        }
    }
    os << LogIO::POST;
    clearFormatFlags(os);
}

//
// Clear all the formatting flags
//
void CalSummary::clearFormatFlags(LogIO& os) const
{
    os.output().unsetf(ios::left);
    os.output().unsetf(ios::right);
    os.output().unsetf(ios::internal);

    os.output().unsetf(ios::dec);
    os.output().unsetf(ios::oct);
    os.output().unsetf(ios::hex);

    os.output().unsetf(ios::showbase | ios::showpos | ios::uppercase | ios::showpoint);

    os.output().unsetf(ios::scientific);
    os.output().unsetf(ios::fixed);
}

} //# NAMESPACE CASA - END

