//# FlagAgentSummary.cc: This file contains the implementation of the FlagAgentSummary class.
//#
//#  CASA - Common Astronomy Software Applications (http://casa.nrao.edu/)
//#  Copyright (C) Associated Universities, Inc. Washington DC, USA 2011, All rights reserved.
//#  Copyright (C) European Southern Observatory, 2011, All rights reserved.
//#
//#  This library is free software; you can redistribute it and/or
//#  modify it under the terms of the GNU Lesser General Public
//#  License as published by the Free software Foundation; either
//#  version 2.1 of the License, or (at your option) any later version.
//#
//#  This library is distributed in the hope that it will be useful,
//#  but WITHOUT ANY WARRANTY, without even the implied warranty of
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//#  Lesser General Public License for more details.
//#
//#  You should have received a copy of the GNU Lesser General Public
//#  License along with this library; if not, write to the Free Software
//#  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
//#  MA 02111-1307  USA
//# $Id: $

#include <flagging/Flagging/FlagAgentSummary.h>

#include <casacore/measures/TableMeasures/ScalarMeasColumn.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

FlagAgentSummary::FlagAgentSummary(FlagDataHandler *dh, Record config):
		        FlagAgentBase(dh,config,ROWS_PREPROCESS_BUFFER,false)
{
    arrayId = 0;
    fieldId = 0;
    spw = 0;
    scan = 0;
    observationId = 0;

    arrayId_str = String("");
    fieldId_str = String("");
    spw_str = String("");
    observationId_str = String("");

    spwChannelCounts = false;
    spwPolarizationCounts = false;
    baselineCounts = false;
    fieldCounts = false;
    display_p = String("none");

    setAgentParameters(config);

    currentSummary = NULL;
    fieldSummaryMap.clear();
    if (fieldCounts)
    {
    	currentSummary = NULL;
    }
    else
    {
    	currentSummary = new summary();
    }

    // Request loading polarization map to FlagDataHandler
    flagDataHandler_p->setMapPolarizations(true);

    // Request pre-loading array,field,spw, scan, observation, antenna1, antenna2
    flagDataHandler_p->preLoadColumn(VisBufferComponent2::ArrayId);
    flagDataHandler_p->preLoadColumn(VisBufferComponent2::FieldId);
    flagDataHandler_p->preLoadColumn(VisBufferComponent2::Scan);
    flagDataHandler_p->preLoadColumn(VisBufferComponent2::ObservationId);
    flagDataHandler_p->preLoadColumn(VisBufferComponent2::SpectralWindows);
    flagDataHandler_p->preLoadColumn(VisBufferComponent2::Antenna1);
    flagDataHandler_p->preLoadColumn(VisBufferComponent2::Antenna2);
    //flagDataHandler_p->preLoadColumn(vi::Freq);
}

FlagAgentSummary::~FlagAgentSummary()
{
    if (fieldCounts)
    {
        for(const auto &iter : fieldSummaryMap)
        {
            delete iter.second;
        }
        fieldSummaryMap.clear();
    }
    else
    {
        delete currentSummary;
    }

    // Compiler automagically calls FlagAgentBase::~FlagAgentBase()
}

void
FlagAgentSummary::setAgentParameters(Record config)
{
    logger_p->origin(LogOrigin(agentName_p,__FUNCTION__,WHERE));

    int exists;

    exists = config.fieldNumber ("spwchan");
    if (exists >= 0)
    {
        if( config.type(exists) != TpBool )
        {
            throw( AipsError ( "Parameter 'spwchan' must be of type 'bool'" ) );
        }

        spwChannelCounts = config.asBool("spwchan");
    }
    else
    {
        spwChannelCounts = false;
    }

    if (spwChannelCounts)
    {
        *logger_p << LogIO::NORMAL << " Spw-Channel count activated " << LogIO::POST;
    }
/*
    else
    {
        *logger_p << LogIO::NORMAL << " Spw-Channel count deactivated " << LogIO::POST;
    }
*/

    exists = config.fieldNumber ("spwcorr");
    if (exists >= 0)
    {
        if( config.type(exists) != TpBool )
        {
            throw( AipsError ( "Parameter 'spwcorr' must be of type 'bool'" ) );
        }

        spwPolarizationCounts = config.asBool("spwcorr");
    }
    else
    {
        spwPolarizationCounts = false;
    }

    if (spwPolarizationCounts)
    {
        *logger_p << LogIO::NORMAL << " Spw-Correlation count activated " << LogIO::POST;
    }
/*
    else
    {
        *logger_p << LogIO::NORMAL << " Spw-Correlation count deactivated " << LogIO::POST;
    }
*/

    exists = config.fieldNumber ("basecnt");
    if (exists >= 0)
    {
        if( config.type(exists) != TpBool )
        {
            throw( AipsError ( "Parameter 'basecnt' must be of type 'bool'" ) );
        }

        baselineCounts = config.asBool("basecnt");
    }
    else
    {
        baselineCounts = false;
    }

    if (baselineCounts)
    {
        *logger_p << LogIO::NORMAL << " Baseline count activated " << LogIO::POST;
    }
/*
    else
    {
        *logger_p << LogIO::NORMAL << " Baseline count deactivated " << LogIO::POST;
    }
*/

    exists = config.fieldNumber ("fieldcnt");
    if (exists >= 0)
    {
        if( config.type(exists) != TpBool )
        {
            throw( AipsError ( "Parameter 'fieldcnt' must be of type 'bool'" ) );
        }

        fieldCounts = config.asBool("fieldcnt");
    }
    else
    {
    	fieldCounts = false;
    }

    if (fieldCounts)
    {
        *logger_p << LogIO::NORMAL << " Field breakdown activated " << LogIO::POST;
    }
/*
    else
    {
        *logger_p << LogIO::NORMAL << " Field breakdown deactivated " << LogIO::POST;
    }
*/

	exists = config.fieldNumber ("display");
	if (exists >= 0)
	{
		if( config.type(exists) != TpString )
		{
			throw( AipsError ( "Parameter 'display' must be of type 'string'" ) );
		}

		display_p = config.asString("display");
		*logger_p << LogIO::NORMAL << " display is: " << display_p << LogIO::POST;
	}


    return;
}

void
FlagAgentSummary::preProcessBuffer(const vi::VisBuffer2 &visBuffer)
{
    arrayId = visBuffer.arrayId()(0);
    arrayId_str = std::to_string(arrayId);

    fieldId = visBuffer.fieldId()(0);
    // Transform fieldId into field name using the corresponding subtable
    fieldId_str = flagDataHandler_p->fieldNames_p->operator()(fieldId);

    spw = visBuffer.spectralWindows()(0);
    spw_str = std::to_string(spw);

    observationId = visBuffer.observationId()[0];
    observationId_str = std::to_string(observationId);

    // Read in channel-frequencies.
    // RVU : I'm not sure if this should go here, or in the FlagDataHandler so that all agents get it.
    if (spwChannelCounts) {
        Vector<Double> flist(visBuffer.getFrequencies(0,MFrequency::TOPO));
        for(Int i=0;i<(Int) flist.nelements();i++)
            frequencyList[spw].push_back(flist[i]);
    }

    if (fieldCounts)
    {
        if (fieldSummaryMap.find(fieldId_str) != fieldSummaryMap.end())
        {
        	currentSummary = fieldSummaryMap[fieldId_str];
        }
        else
        {
        	summary *newsummary = new summary();
        	fieldSummaryMap.insert(std::pair<std::string, summary*>(fieldId_str,newsummary) );
        	currentSummary = fieldSummaryMap[fieldId_str];
        }
    }

    bufferTotal = 0;
    bufferFlags = 0;
}

bool
FlagAgentSummary::computeRowFlags(const vi::VisBuffer2 &visBuffer, FlagMapper &flags,
                                  uInt row)
{
    const Int antenna1 = visBuffer.antenna1()[row];
    const Int antenna2 = visBuffer.antenna2()[row];
    const auto antenna1Name = flagDataHandler_p->antennaNames_p->operator()(antenna1);
    const auto antenna2Name = flagDataHandler_p->antennaNames_p->operator()(antenna2);

    // Get scan for each particular row to cover for the "combine scans" case
    const auto scan = visBuffer.scan()[row];
    const auto scan_str = std::to_string(scan);

    // Compute totals
    Int nChannels, nRows;
    flags.shape(nChannels,nRows);
    const vector< vector<uInt> > polarizations = flags.getSelectedCorrelations();
    const Int nPolarizations = polarizations.size();
    const uInt64 rowTotal = nChannels*nPolarizations;

    // Initialize polarization counts
    Int pol_i = 0;;
    vector<uInt64> polarizationsBreakdownFlags(nPolarizations, 0);

    // Iterate through channels
    Bool flag;
    Int channel_i = 0;
    uInt64 rowFlags = 0;
    uInt64 channelFlags = 0;
    for (channel_i=0; channel_i<nChannels; ++channel_i)
    {
        channelFlags = 0;
        for (pol_i=0; pol_i < nPolarizations; ++pol_i)
        {
            flag = flags.getModifiedFlags(pol_i,channel_i,row);
            channelFlags += flag;
            polarizationsBreakdownFlags[pol_i] += flag;
        }
        rowFlags += channelFlags;

        if (spwChannelCounts)
        {
            currentSummary->accumChanneltotal[spw][channel_i] += nPolarizations;
            currentSummary->accumChannelflags[spw][channel_i] += channelFlags;
        }
    }

    // Update polarization counts
    const polarizationIndexMap *toPolarizationIndexMap =
        flagDataHandler_p->getPolarizationIndexMap();
    for (pol_i=0; pol_i < nPolarizations; ++pol_i)
    {
        const auto &polarization_str = (*toPolarizationIndexMap).at(polarizations[pol_i][0]);
        currentSummary->accumtotal["correlation"][polarization_str] += nChannels;
        currentSummary->accumflags["correlation"][polarization_str] += polarizationsBreakdownFlags[pol_i];

        if (spwPolarizationCounts)
        {
            currentSummary->accumPolarizationtotal[spw][polarization_str] += nChannels;
            currentSummary->accumPolarizationflags[spw][polarization_str] += polarizationsBreakdownFlags[pol_i];
        }
    }

    // Update row counts in fields that require row specific info (like scan, antenna and,
    // optionally, baseline)
    currentSummary->accumtotal["scan"][scan_str] += rowTotal;
    currentSummary->accumflags["scan"][scan_str] += rowFlags;

    currentSummary->accumtotal["antenna"][antenna1Name] += rowTotal;
    currentSummary->accumflags["antenna"][antenna1Name] += rowFlags;

    if (antenna1 != antenna2)
    {
    	currentSummary->accumtotal["antenna"][antenna2Name] += rowTotal;
        currentSummary->accumflags["antenna"][antenna2Name] += rowFlags;
    }

    if ( baselineCounts )
    {
        const auto baseline = antenna1Name + "&&" + antenna2Name;
    	currentSummary->accumtotal["baseline"][baseline] += rowTotal;
        currentSummary->accumflags["baseline"][baseline] += rowFlags;
        currentSummary->accumAntScantotal[antenna1][scan] += rowTotal;
        currentSummary->accumAntScanflags[antenna1][scan] += rowFlags;
        if (antenna1 != antenna2)
        {
            currentSummary->accumAntScantotal[antenna2][scan] += rowTotal;
            currentSummary->accumAntScanflags[antenna2][scan] += rowFlags;
        }
    }

    // keep updating buffer totals for postProcessBuffer()
    bufferTotal += rowTotal;
    bufferFlags += rowFlags;

    return false;
}

void
FlagAgentSummary::postProcessBuffer() {
    // Update here the summary fields that do not need to be updated on a row per row basis
    // (in computeRowFlags which would otherwise repeat all this many more times than needed)
    // The main reason to put these here is that this is much faster (CAS-12714)
    currentSummary->accumtotal["array"][arrayId_str] += bufferTotal;
    currentSummary->accumflags["array"][arrayId_str] += bufferFlags;

    currentSummary->accumtotal["field"][fieldId_str] += bufferTotal;
    currentSummary->accumflags["field"][fieldId_str] += bufferFlags;

    currentSummary->accumtotal["spw"][spw_str] += bufferTotal;
    currentSummary->accumflags["spw"][spw_str] += bufferFlags;

    currentSummary->accumtotal["observation"][observationId_str] += bufferTotal;
    currentSummary->accumflags["observation"][observationId_str] += bufferFlags;

    currentSummary->accumTotalCount += bufferTotal;
    currentSummary->accumTotalFlags += bufferFlags;
}

FlagReport
FlagAgentSummary::getReport()
{
    // Make the flagreport list
    FlagReport summarylist("list");

    // Add the standard summary dictionary as a report of type 'summary'
//    summarylist.addReport( FlagReport("summary", agentName_p, getResult())  );
    summarylist.addReport( FlagReport("summary", summaryName_p, getResult())  );

    //////// Note : Calculate extra views only if the user has asked for it.
    /////// If returning only summary report, do the following.
    ////////  return FlagReport("summary", agentName_p, getResult());

    // Add a list of reports from the flag-count dictionary
    if ( (display_p == String("report")) or (display_p == String("both")) )
    {
        summarylist.addReport ( buildFlagCountPlots() );
    }

    //         // Make a report (or a list of them )for a view, and add it to the list
    //         FlagReport viewrep("plotline",agentName_p,"title","xaxis","yaxis")
    //         viewrep.addData(xdata,ydata,"label");
    //         summarylist.addReport( viewRep );
    //
    return summarylist;
}

FlagReport
FlagAgentSummary::buildFlagCountPlots()
{
    logger_p->origin(LogOrigin(agentName_p,__FUNCTION__,WHERE));
    *logger_p << LogIO::NORMAL << "Generating flag count reports for display" << LogIO::POST;

    FlagReport countRepList("list");

    // (1) Plot of fraction flagged vs frequency (only if spwchan==true)
    if( spwChannelCounts )
    {
        std::pair<string, double> freqUnit("GHz",1e+9);

        FlagReport subRep1 = FlagReport("plotpoints",summaryName_p,"Percentage Flagged",
                "Frequency ("+freqUnit.first+")", "% Flagged");

        for (const auto  &key1 : currentSummary->accumChanneltotal)
        {
            Int nCh=currentSummary->accumChanneltotal[key1.first].size();

            Vector<Float> freqVals(nCh), flagPercent(nCh);
            uInt chCount=0;
            for (const auto &key2 : key1.second)
            {
                // read the frequency value for this channel.
                freqVals[chCount] = frequencyList[key1.first][key2.first] / freqUnit.second;

                // calculate the percentage flagged for this channel
                if( key2.second > 0 )
                {
                    flagPercent[chCount] = 100.0 *
                            (Double) currentSummary->accumChannelflags[key1.first][key2.first] /
                            (Double) key2.second;
                }
                else
                {
                    flagPercent[chCount] = 0.0;
                }

                // Increment channel counter
                chCount++;
            }

            subRep1.addData("line", freqVals,flagPercent,"",Vector<Float>(),
                    "spw"+String::toString(key1.first));

        }

        countRepList.addReport( subRep1 );
    }

    // (2) Plot of fraction flagged vs antenna-position
    Int nAnt=currentSummary->accumtotal["antenna"].size();
    if(nAnt>0) // Perhaps put a parameter to control this ?
    {
        Vector<Float> antPosX(nAnt), antPosY(nAnt), radius(nAnt);
        Int antCount=0;
        const Vector<double> xyzOrigin = (flagDataHandler_p->antennaPositions_p->operator()(0))
                                                                               .getValue().getValue();

        FlagReport subRep2 = FlagReport("plotpoints",summaryName_p,"Percentage Flagged",
                "X meters (ITRF)", "Y meters (ITRF)");

        for (const auto antkey : currentSummary->accumtotal["antenna"])
        {
            Int antId = 0; //antCount; // this needs to find the antenna-id for the antenna name.... aaaaah.
            for(antId=0; antId<(Int) flagDataHandler_p->antennaNames_p->nelements(); antId++)
            {
                if( flagDataHandler_p->antennaNames_p->operator()(antId)
                    == String(antkey.first) ) break;
            }

            const Vector<double> xyz = (flagDataHandler_p->antennaPositions_p->operator()(antId))
                                                                                .getValue().getValue();
            antPosX[antCount] = xyz[0]-xyzOrigin[0];
            antPosY[antCount] = xyz[1]-xyzOrigin[1];
            radius[antCount] = 200.0 *
                    (Double) currentSummary->accumflags["antenna"][antkey.first]/
                    (Double) antkey.second;
            antCount++;
        }
        subRep2.addData("scatter", antPosX,antPosY,"circle",radius,"");
        countRepList.addReport( subRep2 );
    }

    Int nBase= baselineCounts? 0:currentSummary->accumtotal["baseline"].size();

    // (3) Plot of fraction flagged vs baseline-length
    if(nBase>0) // Perhaps put a parameter to control this ?
    {
        Vector<Float> baselineLength(nBase), flagFraction(nBase);
        Int baseCount=0;
        FlagReport subRep3 = FlagReport("plotpoints",summaryName_p,"Percentage Flagged per baseline",
                "Baseline Length (m)", "% Flagged");

        for (const auto &basekey : currentSummary->accumtotal["baseline"])
        {
            Int antId1 = 0, antId2=0;
            String antName1,antName2;
            antName1 = antName2 = String(basekey.first);
            antName1 = antName1.before("&&");
            antName2 = antName2.after("&&");
            for(Int antId=0; antId<(Int) flagDataHandler_p->antennaNames_p->nelements(); antId++)
            {
                if( flagDataHandler_p->antennaNames_p->operator()(antId) == antName1 ) antId1 = antId;
                if( flagDataHandler_p->antennaNames_p->operator()(antId) == antName2 ) antId2 = antId;
            }

            const Vector<double> xyz1 = (flagDataHandler_p->antennaPositions_p->operator()(antId1))
                                                                                 .getValue().getValue();
            const Vector<double> xyz2 = (flagDataHandler_p->antennaPositions_p->operator()(antId2))
                                                                                 .getValue().getValue();
            baselineLength[baseCount] = sqrt( fabs( (xyz1[0]-xyz2[0])*(xyz1[0]-xyz2[0]) +
                    (xyz1[1]-xyz2[1])*(xyz1[1]-xyz2[1]) +
                    (xyz1[2]-xyz2[2])*(xyz1[2]-xyz2[2]) ) );
            flagFraction[baseCount] = 100.0 *
                    (Double) currentSummary->accumflags["baseline"][basekey.first]/
                    (Double) basekey.second;
            baseCount++;
        }
        subRep3.addData("scatter", baselineLength,flagFraction,"",Vector<Float>(),"");
        countRepList.addReport( subRep3 );
    }

    // jagonzal: CAS-3450
    if(nBase>0)
    {
        Int totalNAnt = flagDataHandler_p->antennaNames_p->size();
        // Add ant1xant2 summary views
        FlagReport subRep4 = FlagReport("plotraster",summaryName_p,"% Flagged per baseline", "Antenna 1", "Antenna 2");
        Array<Float> ant1ant2View( IPosition(2, totalNAnt, totalNAnt) , (Float)0);
        std::pair<Int,Int> ant1ant2;
        Float percentageFlagged;
        for (const auto &basekey : currentSummary->accumtotal["baseline"])
        {
            ant1ant2 = flagDataHandler_p->baselineToAnt1Ant2_p[basekey.first];
            percentageFlagged = (Float)100*((Double)currentSummary->accumflags["baseline"][basekey.first] /
                                            (Double)currentSummary->accumtotal["baseline"][basekey.first]);
            ant1ant2View(IPosition(2, ant1ant2.first, ant1ant2.second)) = percentageFlagged;
            ant1ant2View(IPosition(2, ant1ant2.second, ant1ant2.first)) = percentageFlagged;

        }

        subRep4.addData(ant1ant2View);
        countRepList.addReport( subRep4 );

        // Add ant1xscan summary views
        FlagReport subRep5 = FlagReport("plotraster",summaryName_p,"% Flagged per antenna and scan", "Scan relative index", "% Flagged per antenna");

        // NOTE: We need to handle directly the storage array, because it seems that the dimension steps are switched
        Array<Float> antScanView( IPosition(2, currentSummary->accumflags["scan"].size(),totalNAnt) , (Float)0);
        Bool deleteIt = false;
        Float* antScanViewPtr = antScanView.getStorage(deleteIt);


        uInt scanIdx,antennaIdx = 0;
        for (const auto &antkey : currentSummary->accumAntScantotal)
        {
            scanIdx = 0;
            for (const auto scankey : antkey.second)
            {
                percentageFlagged = (Float)100*((Double)currentSummary->accumAntScanflags[antkey.first][scankey.first] /
                                                (Double)currentSummary->accumAntScantotal[antkey.first][scankey.first]);
                antScanViewPtr[totalNAnt*scanIdx + antkey.first] = percentageFlagged;
                scanIdx += 1;
            }
            antennaIdx += 1;
        }

        subRep5.addData(antScanView);
        countRepList.addReport( subRep5 );
    }


    return countRepList;
}


Record
FlagAgentSummary::getResult()
{
    logger_p->origin(LogOrigin(agentName_p,__FUNCTION__,WHERE));

    Record result;

    if (fieldCounts)
    {
        for(const auto &iter : fieldSummaryMap)
    	{
            Record subresult;
            currentSummary = iter.second;
            getResultCore(subresult);
            result.defineRecord(iter.first, subresult);
    	}
    }
    else
    {
    	getResultCore(result);
    }

    return result;
}

void
FlagAgentSummary::getResultCore(Record &result)
{
	if (fieldCounts)
	{
		string field = currentSummary->accumtotal["field"].begin()->first;
		string spaces(field.size(),'=');
		*logger_p << LogIO::NORMAL << "======" << spaces << "==========" << LogIO::POST;
	    *logger_p << LogIO::NORMAL << "Field " << field << " breakdown" << LogIO::POST;
		*logger_p << LogIO::NORMAL << "======" << spaces << "==========" << LogIO::POST;
	}

    if (spwChannelCounts)
    {
        Record stats_key1;

        for (const auto &key1 : currentSummary->accumChanneltotal)
        {
            // Transform spw id into string
            stringstream spw_stringStream;
            spw_stringStream << key1.first;

            for (const auto &key2 : key1.second)
            {
                Record stats_key2;

                stats_key2.define("flagged", (Double) currentSummary->accumChannelflags[key1.first][key2.first]);
                stats_key2.define("total", (Double) key2.second);

                // Transform channel id into string
                stringstream channel_stringStream;
                channel_stringStream << key2.first;

                // Construct spw:channel string as first key
                stats_key1.defineRecord(spw_stringStream.str() + ":" + channel_stringStream.str(), stats_key2);
                // Calculate percentage flagged
                stringstream percentage;
                percentage.precision(3);
                //				percentage.fixed;
                if( key2.second > 0 )
                {
                    percentage << " (" << 100.0 *
                            (Double) currentSummary->accumChannelflags[key1.first][key2.first]/
                            (Double) key2.second << "%)";
                }

                *logger_p 	<< LogIO::NORMAL
                        << " Spw:" << key1.first << " Channel:" << key2.first
                        << " flagged: " <<  (Double) currentSummary->accumChannelflags[key1.first][key2.first]
                                         << " total: " <<  (Double) key2.second
                                         << percentage.str()
                                         << LogIO::POST;
            }

        }

        result.defineRecord("spw:channel", stats_key1);
    }

    if (spwPolarizationCounts)
    {
        Record stats_key1;

        for (const auto &key1 : currentSummary->accumPolarizationtotal)
        {
            // Transform spw id into string
            stringstream spw_stringStream;
            spw_stringStream << key1.first;

            for (const auto &key2 : key1.second)
            {
                Record stats_key2;

                stats_key2.define("flagged", (Double) currentSummary->accumPolarizationflags[key1.first][key2.first]);
                stats_key2.define("total", (Double) key2.second);

                // Construct spw:correlation string as first key (Polarization already comes as a string)
                stats_key1.defineRecord(spw_stringStream.str() + ":" + key2.first, stats_key2);

                // Calculate percentage flagged
                stringstream percentage;
                percentage.precision(3);
                //				percentage.fixed;
                if( key2.second > 0 )
                {
                    percentage << " (" << 100.0 *
                            (Double) currentSummary->accumPolarizationflags[key1.first][key2.first]/
                            (Double) key2.second << "%)";
                }

                *logger_p 	<< LogIO::NORMAL
                        << " Spw:" << key1.first << " Correlation:" << key2.first
                        << " flagged: " <<  (Double) currentSummary->accumPolarizationflags[key1.first][key2.first]
                                            << " total: " <<  (Double) key2.second
                                            << percentage.str()
                                            << LogIO::POST;
            }
        }

        result.defineRecord("spw:correlation", stats_key1);
    }

    if (baselineCounts)
    {
        Record stats_key1;

        for (const auto &key1 : currentSummary->accumAntScantotal)
        {
            // Construct antenna name
            stringstream antenna_stringStream;
            antenna_stringStream << flagDataHandler_p->antennaNames_p->operator()(key1.first);

            for (const auto &key2 : key1.second)
            {
                // Construct scan name
                stringstream scan_stringStream;
                scan_stringStream << key2.first;

                Record stats_key2;

                stats_key2.define("flagged", (Double) currentSummary->accumAntScanflags[key1.first][key2.first]);
                stats_key2.define("total", (Double) key2.second);

                // Construct spw:correlation string as first key (Polarization already comes as a string)
                stats_key1.defineRecord(antenna_stringStream.str() + ":" + scan_stringStream.str(), stats_key2);

                // Calculate percentage flagged
                stringstream percentage;
                percentage.precision(3);
                //				percentage.fixed;
                if( key2.second > 0 )
                {
                    percentage << " (" << 100.0 *
                            (Double) currentSummary->accumAntScanflags[key1.first][key2.first]/
                            (Double) key2.second << "%)";
                }

                *logger_p 	<< LogIO::NORMAL
                        << " Spw:" << key1.first << " Correlation:" << key2.first
                        << " flagged: " <<  (Double) currentSummary->accumAntScanflags[key1.first][key2.first]
                                         << " total: " <<  (Double) key2.second
                                         << percentage.str()
                                         << LogIO::POST;
            }
        }

        result.defineRecord("antenna:scan", stats_key1);
    }

    for (const auto &key1 : currentSummary->accumtotal)
    {
        Record stats_key1;
        for (const auto &key2 : key1.second)
        {
            Record stats_key2;

            stats_key2.define("flagged", (Double) currentSummary->accumflags[key1.first][key2.first]);
            stats_key2.define("total", (Double) key2.second);
            stats_key1.defineRecord(key2.first, stats_key2);

            // Calculate percentage flagged
            stringstream percentage;
            percentage.precision(3);
            //			percentage.fixed;
            if( key2.second > 0 )
            {
                percentage << " (" << 100.0 *
                        (Double) currentSummary->accumflags[key1.first][key2.first] /
                        (Double) key2.second << "%)";
            }

            *logger_p 	<< LogIO::NORMAL
                    << " " << key1.first << " " << key2.first
                    << " flagged: " <<  (Double) currentSummary->accumflags[key1.first][key2.first]
                                     << " total: " <<  (Double) key2.second
                                     << percentage.str()
                                     << LogIO::POST;
        }

        result.defineRecord(key1.first, stats_key1);
    }

    result.define("flagged", (Double) currentSummary->accumTotalFlags);
    result.define("total"  , (Double) currentSummary->accumTotalCount);

    // Calculate percentage flagged
    stringstream percentage;
    percentage.precision(3);
    //	percentage.fixed;
    if( currentSummary->accumTotalCount > 0 )
    {
        percentage << " (" << 100.0 *
                (Double) currentSummary->accumTotalFlags /
                (Double) currentSummary->accumTotalCount << "%)";
    }
    *logger_p 	<< LogIO::NORMAL
            << " Total Flagged: " <<  (Double) currentSummary->accumTotalFlags
            << " Total Counts: " <<  (Double) currentSummary->accumTotalCount
            << percentage.str()
            << LogIO::POST;

    return;
}

} //# NAMESPACE CASA - END


