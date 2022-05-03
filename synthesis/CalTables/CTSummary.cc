//# CTSummary.cc:  Helper class for listing a NewCalTable
//# Copyright (C) 2017
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

#include <synthesis/CalTables/CTSummary.h>
#include <synthesis/CalTables/NewCalTable.h>
#include <synthesis/CalTables/CTMainColumns.h>
#include <synthesis/CalTables/CTColumns.h>
#include <synthesis/CalTables/CTIter.h>

#include <casacore/casa/Quanta/QVector.h>
#include <casacore/measures/Measures/MeasTable.h>
#include <casacore/casa/iomanip.h>
#include <casacore/casa/iostream.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//
// Constructor assigns pointer.  If NCT goes out of scope you
// will get rubbish.  Also sets string to separate subtable output.
//
CTSummary::CTSummary (const NewCalTable& nct)
: pNCT(&nct), 
  dashlin1(replicate("-",80)),
  dashlin2(replicate("=",80))
{}

CTSummary::CTSummary (const NewCalTable* nct)
: pNCT(nct), 
  dashlin1(replicate("-",80)),
  dashlin2(replicate("=",80))
{}

CTSummary::~CTSummary ()
{ }


//
// Retrieve number of rows
//
Int CTSummary::nrow () const
{
    return pNCT->nrow();
}


//
// Get ms name
//
String CTSummary::name () const
{
    return pNCT->tableName();
}


//
// Reassign pointer.
//
Bool CTSummary::setNCT (const NewCalTable& nct)
{
    const NewCalTable* pTemp;
    pTemp = &nct;
    if (pTemp == 0) {
        return False;
    } else {
        pNCT = pTemp;
        return True;
    }
}


//
// List information about a ct to the logger
//
void CTSummary::list (LogIO& os, Bool verbose) const
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
void CTSummary::listTitle (LogIO& os) const
{
    os << LogIO::NORMAL;
    // Type of cal table
    casacore::String type("unknown");
    os << dashlin2 << endl << "Cal Table Name: " << this->name();
    if (pNCT->keywordSet().isDefined("VisCal")) {
        type = pNCT->keywordSet().asString("VisCal");
        // List the CT name and type as the title of the Summary
        os << "   Cal Type: " << type;
    }
    os << endl << dashlin2 << endl;
}


//
// Convenient table groupings
//
void CTSummary::listWhere (LogIO& os, Bool verbose) const
{
    listObservation (os,verbose);
}

void CTSummary::listWhat (LogIO& os, Bool verbose) const
{
    listMain(os, verbose);
    os << dashlin1 << endl;
    listField(os, verbose);
}

void CTSummary::listHow (LogIO& os, Bool verbose) const
{
    listSpectralWindow(os, verbose);
    os << dashlin1 << endl;
    listAntenna(os, verbose);
}

//
// SUBTABLES
//
void CTSummary::listMain (LogIO& os, Bool verbose) const
{
    if (nrow()<=0) {
        os << "The MAIN table is empty: there are no data!!!" << endl << LogIO::POST;
        return;
    }
    // to get columns from main and subtables
    ROCTMainColumns ctmain(*pNCT);
    ROCTColumns ctsub(*pNCT);

    // Time:
    casacore::Vector<casacore::Double> times = ctmain.time().getColumn();
    casacore::Double startTime = times(0);
    casacore::Double stopTime = times(times.size()-1);
    casacore::Double exposTime = stopTime - startTime;
    MVTime startMVT(startTime/86400.0), stopMVT(stopTime/86400.0);
    casacore::String timeref = ctmain.time().keywordSet().subRecord("MEASINFO").asString("Ref");
    // Output info
    os << "  Data records: " << nrow() << "       Total elapsed time = "
       << exposTime << " seconds" << endl
       << "   Observed from   "
       << MVTime(startTime/C::day).string(MVTime::DMY,7)  //startMVT.string()
       << "   to   "
       << MVTime(stopTime/C::day).string(MVTime::DMY,7)  // stopMVT.string()
       << " (" << timeref << ")"
       << endl << endl;
    os << LogIO::POST;

    if (verbose) {
        // Field names:
        casacore::Vector<casacore::String> fieldnames = ctsub.field().name().getColumn();
        // Spw Ids
        casacore::Vector<casacore::Int> specwindids(ctmain.spwId().getColumn());

        // Field widths for printing:
        casacore::Int width_lead(2),
            width_scan(4),
            width_btime(22),
            width_etime(10),
            width_fieldId(5),
            width_fieldname(20),
            width_nrow(10);

        // Set up iteration over OBSID
        casacore::Block<casacore::String> icols(3);
        icols[0] = "OBSERVATION_ID";
        icols[1] = "SCAN_NUMBER";
        icols[2] = "FIELD_ID";
        ROCTIter* iter = new ROCTIter(*pNCT, icols);
        iter->reset();
        casacore::Int obsid(-2);       // print obsid when changes
        casacore::Double lastday(0.0); // print date when changes
        while (!iter->pastEnd()) {
            casacore::Int thisobs = iter->thisObs();
            if (thisobs != obsid) {
                obsid = thisobs;
                lastday = 0.0; // force printing date
                os << endl << "   ObservationID = " << thisobs << endl;
                os << LogIO::POST;
                // print header for output columns
                casacore::String datetime = "  Date        Timerange (              ";
                // variable-len timeref string
                datetime.replace(25,timeref.length(),timeref);
                datetime.replace(25+timeref.length(),1,")");
                os << datetime;
                os << "Scan  FldId FieldName           nRows       ";
                os << "SpwIds   Avg Intervals" << endl;
                os << LogIO::POST;
            }
            os.output().precision(3);
            // get values for this iteration
            casacore::Vector<casacore::Double> times = iter->time();
            casacore::Double btime = times(0);
            casacore::Double etime = times(times.size()-1);
            casacore::Double day = floor(MVTime(btime/C::day).day());
            casacore::Int scan = iter->thisScan();
            casacore::Int fieldid = iter->thisField();
            casacore::String fieldname;
            if (fieldid >= 0)
                fieldname = fieldnames(fieldid);
            casacore::Int nrow = iter->nrow();
            std::vector<casacore::Int> spws = iter->spw().tovector();
            std::set<casacore::Int> spwset = std::set<casacore::Int>(spws.begin(), spws.end());
            std::vector<casacore::Double> intervals = iter->interval().tovector();
            std::set<casacore::Double> intervalset = std::set<casacore::Double>(intervals.begin(), intervals.end());
            // print line
            os.output().setf(ios::right, ios::adjustfield);
            os.output().width(width_lead); os << "  ";
            // Timerange beginning time
            os.output().width(width_btime);
            if (day!=lastday) { // print date
                os << MVTime(btime/C::day).string(MVTime::DMY,7);
            } else {            // omit date
                os << MVTime(btime/C::day).string(MVTime::TIME,7);
            }
            os.output().width(3); os << " - ";
            // Timerange end time
            os.output().width(width_etime);
            os << MVTime(etime/C::day).string(MVTime::TIME,7);
            // Scan
            os.output().width(width_lead); os << "  ";
            os.output().setf(ios::right, ios::adjustfield);
            os.output().width(width_scan);
            os << scan;
            // FldId
            os.output().width(width_lead); os << "  ";
            os.output().setf(ios::right, ios::adjustfield);
            os.output().width(width_fieldId);
            os << fieldid << " ";
            // FieldName
            os.output().setf(ios::left, ios::adjustfield);
            if (fieldname.length()>20)
                fieldname.replace(19,1,'*');
            os.output().width(width_fieldname);
            os << fieldname.at(0,20);
            // nRows
            os.output().width(width_nrow);
            os.output().setf(ios::left, ios::adjustfield);
            os << nrow;
            // SpwIds
            os.output().width(width_lead); os << "  ";
            os << spwset;  // set includes "[ ]"
            // Avg Intervals
            os.output().width(width_lead); os << "  ";
            os << intervalset;
            os << endl;
            os << LogIO::POST;
            
            // next lastday is this day
            lastday = day;
            iter->next();
        } // while
        os << "(nRows = Total number of rows per scan) " << endl;
        os << LogIO::POST;
    }
    clearFormatFlags(os);
}

void CTSummary::listAntenna (LogIO& os, Bool verbose) const
{
    // Is antenna table present?
    ROCTColumns ctc(*pNCT);
    const ROCTAntennaColumns& ctAC(ctc.antenna());
    if (ctAC.nrow()<=0) {
        os << "The ANTENNA table is empty" << endl;
    } else {
        // Determine antennas  present in the main table
        ROCTMainColumns ctmain(*pNCT);
        std::vector<casacore::Int> ant1col = ctmain.antenna1().getColumn().tovector();
        std::set<casacore::Int> antIds = std::set<casacore::Int>(ant1col.begin(), ant1col.end());
        std::vector<casacore::Int> ant2col = ctmain.antenna2().getColumn().tovector();
        std::set<casacore::Int> ant2ids = std::set<casacore::Int>(ant2col.begin(), ant2col.end());
        for (std::set<Int>::const_iterator iter=ant2ids.begin(); iter!= ant2ids.end(); ++iter) {
            antIds.insert(*iter);
        }
        antIds.erase(-1); // remove invalid antenna id
        uInt nAnt = antIds.size();
        casacore::Vector<casacore::String> names = ctAC.name().getColumn();
        casacore::Vector<casacore::String> stations = ctAC.station().getColumn();
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

            // Position and diameter values for each antenna
            casacore::ROScalarMeasColumn<casacore::MPosition> antPos = ctAC.positionMeas();
            Bool posIsITRF = (antPos(0).getRef().getType() == MPosition::ITRF);
            casacore::ROScalarQuantColumn<casacore::Double> diameters = ctAC.dishDiameterQuant();
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
            // Write the title:
            casacore::String title("Antennas: "+String::toString(nAnt)+":");
            os << title << endl;
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
              const Quantity& diam = diameters(antId);
              // xyz
              casacore::Vector<casacore::Double> xyz = antPos(antId).get("m").getValue();
              casacore::Double x = xyz(0);
              casacore::Double y = xyz(1);
              casacore::Double z = xyz(2);
              casacore::Double rAnt = sqrt(x*x + y*y + z*z);
              // offsets
              const MPosition& mLongLat = antPos(antId);
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
                  MeasConvert<MPosition> toItrf(antPos(antId), MPosition::ITRF);
                  antPos(antId) = toItrf(antPos(antId));
              }
              // write the row
              os.output().setf(ios::left, ios::adjustfield);
              os << indent;
              // ID, Name, Station
              os.output().width(indwidth);  os << antId;
              os.output().width(namewidth); os << names(antId);
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
            os << "Antennas: " << nAnt << " ('name'@'station') " <<  endl;
            casacore::String line, leader;
            // track last id of previous line for leader
            casacore::Int lastIdInLine = *antIds.begin() - 1;
            std::set<casacore::Int>::const_iterator iter = antIds.begin();
            std::set<casacore::Int>::const_iterator end = antIds.end();
            casacore::Int maxAnt = *std::max_element(antIds.begin(), antIds.end());
            for (; iter!=end; ++iter) {
                Int antId = *iter;
                // Build the line
                line = line + "'" + names(antId) + "'" + "@";
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

bool CTSummary::getObservatoryPosition(casacore::MPosition& obspos,
        casacore::String& name) const {
    // Returns observatory position in ITRF
    // get telescope name
    bool obsposOk;
    ROCTColumns ctc(*pNCT);
    const ROCTObservationColumns& ctOC(ctc.observation());
    name = ctOC.telescopeName().getColumn()(0);
    obsposOk = MeasTable::Observatory(obspos, name);
    if (obsposOk && (obspos.getRef().getType() != MPosition::ITRF)) {
        MeasConvert<MPosition> toItrf(obspos, MPosition::ITRF);
        obspos = toItrf(obspos);
    }
    return obsposOk;
}

void CTSummary::listField (LogIO& os, Bool verbose) const
{
    // Is field table present?
    ROCTColumns ctc(*pNCT);
    const ROCTFieldColumns& ctFC(ctc.field());
    if (ctFC.nrow() <= 0) {
        os << "The FIELD table is empty" << endl;
    } else {
        // only list fields in main table
        ROCTMainColumns ctmain(*pNCT);
        std::vector<casacore::Int> fieldcol = ctmain.fieldId().getColumn().tovector();
        std::set<casacore::Int> fieldIds = std::set<casacore::Int>(fieldcol.begin(), fieldcol.end());
        os << "Fields: " << fieldIds.size() << endl;
        casacore::Int widthLead(2),
            widthField(6),
            widthCode(5),
            widthName(20),
            widthRA(16),
            widthDec(16),
            widthType(8),
            widthSrc(6);

        if (verbose) {}  // null, always same output

        // Line is    ID Code Name Name RA Dec Type
        os.output().setf(ios::left, ios::adjustfield);
        os.output().width(widthLead);   os << "  ";
        os.output().width(widthField);  os << "FldId";
        os.output().width(widthCode);   os << "Code";
        os.output().width(widthName);   os << "Name";
        os.output().width(widthRA);     os << "RA";
        os.output().width(widthDec);    os << " Decl";
        os.output().width(widthType);   os << "Epoch";
        os.output().width(widthSrc);    os << "SrcId";
        os << endl;
        // get column values
        casacore::Vector<casacore::String> fieldNames = ctFC.name().getColumn();
        casacore::Vector<casacore::String> codes = ctFC.code().getColumn();
        casacore::ROArrayMeasColumn<casacore::MDirection> phaseDirs = ctFC.phaseDirMeasCol();
        casacore::Vector<casacore::Int> sourceIds = ctFC.sourceId().getColumn();
        // loop through fields
        std::set<Int>::const_iterator fiter = fieldIds.begin();
        std::set<Int>::const_iterator fend = fieldIds.end();
        static const MEpoch ezero(Quantity(0, "s"));
        //vector<Int> sourceIDs = _msmd->getFieldTableSourceIDs();
        for (; fiter!=fend; ++fiter) {
            Int fieldId = *fiter;
            if (fieldId >=0) {
                casacore::MDirection mRaDec = phaseDirs(fieldId)(IPosition(1,0));
                MVAngle mvRa = mRaDec.getAngle().getValue()(0);
                MVAngle mvDec = mRaDec.getAngle().getValue()(1);
                String name = fieldNames(fieldId);
                if (name.length()>20) 
                    name.replace(19,1,"*");
                os.output().setf(ios::left, ios::adjustfield);
                os.output().width(widthLead);  os << "  ";
                os.output().width(widthField); os << fieldId;
                os.output().width(widthCode);  os << codes(fieldId);
                os.output().width(widthName);  os << name.at(0,20);
                os.output().width(widthRA);    os << mvRa(0.0).string(MVAngle::TIME,12);
                os.output().width(widthDec);   os << mvDec.string(MVAngle::DIG2,11);
                os.output().width(widthType);
                os << MDirection::showType(mRaDec.getRefPtr()->getType());
                os.output().width(widthSrc);  os << sourceIds[fieldId];
                os << endl;
            } else {
                os.output().setf(ios::left, ios::adjustfield);
                os.output().width(widthLead);  os << "  ";
                os.output().width(widthField); os << fieldId;
                os << endl;
            }
        }
    }
    os << endl << LogIO::POST;
    clearFormatFlags(os);
}

void CTSummary::listObservation (LogIO& os, Bool verbose) const
{
    ROCTColumns ctc(*pNCT);
    const ROCTObservationColumns& ctOC(ctc.observation());
    if (ctOC.nrow() <= 0) {
        os << "The OBSERVATION table is empty" << endl;
    } else {
      os << "   Observer: " << ctOC.observer()(0) << "  "
         << "   Project: " << ctOC.project()(0) << "  " << endl;
      os << "   Observation: " << ctOC.telescopeName()(0) << endl << endl;
      if (verbose) {
        // only list obsids in main table
        ROCTMainColumns ctmain(*pNCT);
        std::vector<casacore::Int> obsidcol = ctmain.obsId().getColumn().tovector();
        std::set<casacore::Int> obsIds = std::set<casacore::Int>(obsidcol.begin(), obsidcol.end());
        // for timeranges
        casacore::IPosition first(1,0);
        casacore::IPosition second(1,1);
        casacore::String timeref = ctOC.timeRange().keywordSet().subRecord("MEASINFO").asString("Ref");
        // time range (Date) format is e.g. 00:00:46.0-01:43:50.2
        casacore::Int widthTel(10), widthDate(36), widthObs(20), widthProj(15);
        // column headings
        os.output().setf(ios::left, ios::adjustfield);
        os.output().width(widthTel);    os << "Telescope";
        casacore::String timeHeader = "Timerange (" + timeref + ")";
        os.output().width(widthDate);  os << timeHeader;
        os.output().width(widthObs);    os << "Observer";
        os.output().width(widthProj);    os << "Project";
        os << endl;
        // get columns
        casacore::Vector<casacore::String> telname = ctOC.telescopeName().getColumn();
        casacore::Array<casacore::Double> timerange = ctOC.timeRange().getColumn();
        casacore::Vector<casacore::String> observer = ctOC.observer().getColumn();
        casacore::Vector<casacore::String> project = ctOC.project().getColumn();
        // print each obsid row
        std::set<casacore::Int>::const_iterator iter = obsIds.begin();
        std::set<casacore::Int>::const_iterator end = obsIds.end();
        for (; iter!=end; ++iter) {
          casacore::Int obsid = *iter;
          if (obsid >= 0) {
            os.output().setf(ios::left, ios::adjustfield);
            os.output().width(widthTel);  os << telname(obsid);
            casacore::Double start = timerange(IPosition(2, 0, obsid));
            casacore::Double end = timerange(IPosition(2, 1, obsid));
            casacore::String range = MVTime(start/C::day).string(MVTime::YMD,7) + " - " 
              + MVTime(end/C::day).string(MVTime::TIME,7);
            os.output().width(widthDate); os << range;
            os.output().width(widthObs);  os << observer(obsid);
            os.output().width(widthProj); os << project(obsid);
            os << endl;
          }
        }
      }
    }
    os << LogIO::POST;
    clearFormatFlags(os);
}


String formatTime(const Double time) {
    MVTime mvtime(Quantity(time, "s"));
    Time t=mvtime.getTime();
    ostringstream os;
    os << t;
    return os.str();
}

void CTSummary::listHistory (LogIO& os) const
{
    // Create a history object
    ROCTHistoryColumns ctHist(pNCT->history());

    if (ctHist.nrow()<=0) {
        os << "The HISTORY table is empty" << endl;
    }
    else {
        uInt nmessages = ctHist.time().nrow();
        os << "History table entries: " << nmessages << endl << LogIO::POST;
        const ScalarColumn<Double> &theTimes((ctHist.time()));
        const ScalarColumn<String> &messOrigin((ctHist.origin()));
        const ScalarColumn<String> &messString((ctHist.message()));
        const ScalarColumn<String> &messPriority((ctHist.priority()));
        for (uInt i=0 ; i < nmessages; i++) {
            Quantity tmpq(theTimes(i), "s");
            MVTime mvtime(tmpq);
            Time messTime(mvtime.getTime());
            LogMessage::Priority itsPriority(LogMessage::NORMAL);
            if(messPriority(i) == "DEBUGGING"){
                itsPriority = LogMessage::DEBUGGING;
            } else if(messPriority(i) == "DEBUG2"){
                itsPriority = LogMessage::DEBUG2;
            } else if(messPriority(i) == "DEBUG1"){
                itsPriority = LogMessage::DEBUG1;
            } else if(messPriority(i) == "NORMAL5" ||
                      messPriority(i) == "INFO5"){
                itsPriority = LogMessage::NORMAL5;
            } else if(messPriority(i) == "NORMAL4" ||
                      messPriority(i) == "INFO4"){
                itsPriority = LogMessage::NORMAL4;
            } else if(messPriority(i) == "NORMAL3" ||
                      messPriority(i) == "INFO3"){
                itsPriority = LogMessage::NORMAL3;
            } else if(messPriority(i) == "NORMAL2" ||
                      messPriority(i) == "INFO2"){
                itsPriority = LogMessage::NORMAL2;
            } else if(messPriority(i) == "NORMAL1" ||
                      messPriority(i) == "INFO1"){
                itsPriority = LogMessage::NORMAL1;
            } else if(messPriority(i) == "NORMAL" ||
                      messPriority(i) == "INFO"){
                itsPriority = LogMessage::NORMAL;
            } else if(messPriority(i) == "WARN"){
                itsPriority = LogMessage::WARN;
            } else if(messPriority(i) == "SEVERE"){
                itsPriority = LogMessage::SEVERE;
            }
            LogOrigin orhist(messOrigin(i));
            LogMessage histMessage(messString(i), orhist.taskName("listHistory"), itsPriority);
            histMessage.messageTime(messTime);
            os.post(histMessage);
        }
        os << LogIO::POST;
    }
}

void CTSummary::listSpectralWindow (LogIO& os, Bool verbose) const
{
    ROCTColumns ctc(*pNCT);
    const ROCTSpWindowColumns& ctSW(ctc.spectralWindow());

    if (verbose) {}    //null; always the same output

    if (ctSW.refFrequency().nrow()<=0) {
        os << "The SPECTRAL_WINDOW table is empty" << endl;
    } else {
        casacore::uInt nrow = ctSW.refFrequency().nrow();
        os << "Spectral Windows: " << nrow << endl;
        // Header: SpwId  Ref.Freq    #Chans  Resolution  TotalBW
        casacore::Int widthLead(2), widthId(7), widthFreq(18), widthNumChan(8);
        os.output().setf(ios::left, ios::adjustfield);
        os.output().width(widthLead);    os << "  ";
        os.output().width(widthId); os << "SpwId";
        os.output().width(widthFreq);    os << "Ref.Freq (MHz)";
        os.output().width(widthNumChan); os << "#Chans";
        os.output().width(widthFreq);    os << "Resolution (kHz)";
        os.output().width(widthFreq);    os << "TotalBW (kHz)";
        os << endl;

        // For each row of the SpWin subtable, write the info
        for (uInt row=0; row<nrow; row++) {
            os.output().setf(ios::left, ios::adjustfield);
            os.output().width(widthLead); os << "  ";
            os.output().width(widthId); os << row;
            os.output().width(widthFreq);
            os << ctSW.refFrequency()(row) / 1.0e6;
            os.output().width(widthNumChan);
            os << ctSW.numChan()(row);
            os.output().width(widthFreq);
            os << ctSW.resolution()(row)(IPosition(1,0))/1000.0;
            os.output().width(widthFreq);
            os << ctSW.totalBandwidth()(row)/1000.0;
            os << endl;
        }
    }
    os << LogIO::POST;
    clearFormatFlags(os);
}

void CTSummary::listTables (LogIO& os, Bool verbose) const
{
    // Get nrows for each table (=-1 if table absent)
    casacore::Vector<casacore::Int> tableRows(6);
    tableRows(0) = nrow();
    tableRows(1) = pNCT->observation().nrow();
    tableRows(2) = pNCT->antenna().nrow();
    tableRows(3) = pNCT->field().nrow();
    tableRows(4) = pNCT->spectralWindow().nrow();
    tableRows(5) = pNCT->history().nrow();

    casacore::Vector<casacore::String> tableStrings(6);
    tableStrings(0) = "MAIN";
    tableStrings(1) = "OBSERVATION";
    tableStrings(2) = "ANTENNA";
    tableStrings(3) = "FIELD";
    tableStrings(4) = "SPECTRAL_WINDOW";
    tableStrings(5) = "HISTORY";

    // Just to make things read better
    casacore::Vector<casacore::String> rowStrings(6);
    rowStrings = " rows";
    for (uInt i=0; i<6; i++) {
        if (tableRows(i)==1) rowStrings(i) = " row";
        // if table exists, but empty:
        if (tableRows(i)==0) rowStrings(i) = " <empty>";
        // if table absent:
        if (tableRows(i)==-1) rowStrings(i) = "<absent>";
    }

    if (verbose) {
        os << "   Tables" << endl;
        for (uInt i=0; i<6; i++) {
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
        for (uInt i=0; i<6; i++) {
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
void CTSummary::clearFormatFlags(LogIO& os) const
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

