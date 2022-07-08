//# MSTransformDataHandler.cc: This file contains the implementation of the MSTransformDataHandler class.
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

#include <mstransform/MSTransform/MSTransformDataHandler.h>
#include <casacore/tables/Tables/TableProxy.h>
#include <casacore/tables/TaQL/TableParse.h>
#include <casacore/ms/MSOper/MSMetaData.h>
#include <asdmstman/AsdmStMan.h>


using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

/////////////////////////////////////////////
/// MSTransformDataHandler implementation ///
/////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
    MSTransformDataHandler::MSTransformDataHandler(const String& theMS, Table::TableOption option,
                                                   Bool virtualModelCol,Bool virtualCorrectedCol,
                                                   Bool reindex) :
		  ms_p(MeasurementSet(theMS, option)),
		  mssel_p(ms_p),
		  msc_p(NULL),
		  mscIn_p(NULL),
		  keepShape_p(true),
		  antennaSel_p(false),
		  timeBin_p(-1.0),
		  scanString_p(""),
		  intentString_p(""),
		  obsString_p(""),
		  uvrangeString_p(""),
		  taqlString_p(""),
		  timeRange_p(""),
		  arrayExpr_p(""),
		  combine_p(""),
		  fitorder_p(-1),
		  fitspw_p("*"),
		  fitoutspw_p("*"),
		  virtualModelCol_p(virtualModelCol),
		  virtualCorrectedCol_p(virtualCorrectedCol),
		  reindex_p(reindex)
{
	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
MSTransformDataHandler::MSTransformDataHandler(const MeasurementSet& ms,
                                               Bool virtualModelCol,Bool virtualCorrectedCol,
                                               Bool reindex) :
		   ms_p(ms),
		   mssel_p(ms_p),
		   msc_p(NULL),
		   mscIn_p(NULL),
		   keepShape_p(true),
		   antennaSel_p(false),
		   timeBin_p(-1.0),
		   scanString_p(""),
		   intentString_p(""),
		   obsString_p(""),
		   uvrangeString_p(""),
		   taqlString_p(""),
		   timeRange_p(""),
		   arrayExpr_p(""),
		   combine_p(""),
		   fitorder_p(-1),
		   fitspw_p("*"),
		   fitoutspw_p("*"),
		   virtualModelCol_p(virtualModelCol),
		   virtualCorrectedCol_p(virtualCorrectedCol),
		   reindex_p(reindex)
{
	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
MSTransformDataHandler::~MSTransformDataHandler()
{
	if (msc_p) delete msc_p;
	msc_p = nullptr;

	if (mscIn_p) delete mscIn_p;
	mscIn_p = nullptr;

	msOut_p=MeasurementSet();

	// parseColumnNames unavoidably has a static String and Vector<MS::PredefinedColumns>.
	// Collapse them down to free most of that memory.
	parseColumnNames("None");

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Vector<MS::PredefinedColumns>& MSTransformDataHandler::parseColumnNames(String col)
{
	// Memorize both for efficiency and so that the info
	// message at the bottom isn't unnecessarily repeated.
	static String my_colNameStr = "";
	static Vector<MS::PredefinedColumns> my_colNameVect;

	col.upcase();
	if(col == my_colNameStr && col != "")
	{
		return my_colNameVect;
	}
	else if(col == "NONE")
	{
		my_colNameStr = "";
		my_colNameVect.resize(0);
		return my_colNameVect;
	}

	uInt nNames;

	if(col.contains("ALL"))
	{
		nNames = 3;
		my_colNameVect.resize(nNames);
		my_colNameVect[0] = MS::DATA;
		my_colNameVect[1] = MS::MODEL_DATA;
		my_colNameVect[2] = MS::CORRECTED_DATA;
	}
	else
	{
		nNames = dataColStrToEnums(col, my_colNameVect);
	}

	// Whether or not the MS has the columns is checked by verifyColumns().
	// Unfortunately it cannot be done here because this is a static method.


	/*
	 * jagonzal: Redundant logging message (this info is already provided by MSTransformManager)
	 *
	LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));
	// "NONE" is used by the destructor
	if(col != "NONE")
	{
		os << LogIO::NORMAL << "Using ";
		for(uInt i = 0; i < nNames; ++i)
		{
			os << MS::columnName(my_colNameVect[i]) << " ";
		}

		os << " column" << (my_colNameVect.nelements() > 1 ? "s." : ".") << LogIO::POST;
	}
	*/

	my_colNameStr = col;
	return my_colNameVect;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
const Vector<MS::PredefinedColumns>& MSTransformDataHandler::parseColumnNames(	String col,
																				const MeasurementSet& msref,
																				Bool virtualModelCol,
																				Bool virtualCorrectedCol)
{
	// Memorize both for efficiency and so that the info
	// message at the bottom isn't unnecessarily repeated.
	static String my_colNameStr = "";
	static Vector<MS::PredefinedColumns> my_colNameVect;

	// Data columns to pick up if present.
	Vector<MS::PredefinedColumns> wanted;

	col.upcase();

	// This version of parseColumnNames does not reuse results of previous calls
	// but always checks the given columns because it cannot be certain that msref
	// refers to the same MS with every call.

	if (col == "NONE")
	{
		my_colNameStr = "";
		my_colNameVect.resize(0);
		return my_colNameVect;
	}

	LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));

	// Are we choosy?
	const Bool doAny = col.contains("ALL") || col.contains("ANY");

	uInt nPoss;
	if (doAny)
	{
		nPoss = 5;
		wanted.resize(nPoss);
		wanted[0] = MS::DATA;
		wanted[1] = MS::MODEL_DATA;
		wanted[2] = MS::CORRECTED_DATA;
		wanted[3] = MS::FLOAT_DATA;
		wanted[4] = MS::LAG_DATA;
	}
	// split name string into individual names
	else
	{
		nPoss = dataColStrToEnums(col, wanted);
	}

	uInt nFound = 0;
	my_colNameVect.resize(0);
	for (uInt i = 0; i < nPoss; ++i)
	{
		if (msref.tableDesc().isColumn(MS::columnName(wanted[i])))
		{
			++nFound;
			my_colNameVect.resize(nFound, true);
			my_colNameVect[nFound - 1] = wanted[i];
		}
		// CAS-5348 (jagonzal): Model parameters check is done at construction time
		else if (wanted[i] == MS::MODEL_DATA and virtualModelCol)
		{
			++nFound;
			my_colNameVect.resize(nFound, true);
			my_colNameVect[nFound - 1] = wanted[i];
		}
		else if (wanted[i] == MS::CORRECTED_DATA and virtualCorrectedCol)
		{
			++nFound;
			my_colNameVect.resize(nFound, true);
			my_colNameVect[nFound - 1] = wanted[i];
		}
		else if (!doAny)
		{
			ostringstream ostr;
			ostr 	<< "Desired column (" << MS::columnName(wanted[i])
					<< ") not found in the input MS (" << msref.tableName()
					<< ").";
			throw(AipsError(ostr.str()));
		}
	}
	if (nFound == 0) throw(AipsError("Did not find and select any data columns."));

	my_colNameStr = col;

	return my_colNameVect;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
uInt MSTransformDataHandler::dataColStrToEnums(const String& col, Vector<MS::PredefinedColumns>& colvec)
{
	LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));
	String tmpNames(col);
	Vector<String> tokens;
	tmpNames.upcase();

	// split name string into individual names
	char * pch;
	Int it = 0;
	pch = strtok((char*) tmpNames.c_str(), " ,");
	while (pch != NULL)
	{
		tokens.resize(it + 1, true);
		tokens[it] = String(pch);
		++it;
		pch = strtok(NULL, " ,");
	}

	uInt nNames = tokens.nelements();

	uInt nFound = 0;
	for (uInt i = 0; i < nNames; ++i)
	{
		colvec.resize(nFound + 1, true);
		colvec[nFound] = MS::UNDEFINED_COLUMN;

		if (	tokens[i] == "OBSERVED"
				|| tokens[i] == "DATA"
				|| tokens[i] == MS::columnName(MS::DATA))
		{
			colvec[nFound++] = MS::DATA;
		}
		else if (	tokens[i] == "FLOAT"
					|| tokens[i] == "FLOAT_DATA"
					|| tokens[i] == MS::columnName(MS::FLOAT_DATA))
		{
			colvec[nFound++] = MS::FLOAT_DATA;
		}
		else if (	tokens[i] == "LAG"
					|| tokens[i] == "LAG_DATA"
					|| tokens[i] == MS::columnName(MS::LAG_DATA))
		{
			colvec[nFound++] = MS::LAG_DATA;
		}
		else if (	tokens[i] == "MODEL"
					|| tokens[i] == "MODEL_DATA"
					|| tokens[i] == MS::columnName(MS::MODEL_DATA))
		{
			colvec[nFound++] = MS::MODEL_DATA;
		}
		else if (	tokens[i] == "CORRECTED"
					|| tokens[i] == "CORRECTED_DATA"
					|| tokens[i] == MS::columnName(MS::CORRECTED_DATA))
		{
			colvec[nFound++] = MS::CORRECTED_DATA;
		}
		// "NONE" is used by the destructor
		else if (tmpNames != "NONE")
		{
			os << LogIO::SEVERE;

			if (nFound == 0)
			{
				colvec[0] = MS::DATA;
				os << "Unrecognized data column " << tokens[i] << "...trying DATA.";
			}
			else
			{
				os << "Skipping unrecognized data column " << tokens[i];
			}

			os << LogIO::POST;
		}
	}
	return nFound;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Bool MSTransformDataHandler::setmsselect(	const String& spw, const String& field,
                        					const String& baseline, const String& scan,
                        					const String& uvrange, const String& taql,
                        					const Vector<Int>& step, const String& subarray,
                        					const String& correlation, const String& intent,
                        					const String& obs, const String& feed)
{
	LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));
	Bool ok;

	String myspwstr(spw == "" ? "*" : spw);
	Record selrec = ms_p.msseltoindex(myspwstr, field);

	ok = selectSource(selrec.asArrayInt("field"));

	// All of the requested selection functions will be tried, even if an
	// earlier one has indicated its failure.  This allows all of the selection
	// strings to be tested, yielding more complete feedback for the user
	// (fewer retries).  This is a matter of taste, though.  If the selections
	// turn out to be slow, this function should return on the first false.

	if (!selectSpw(myspwstr, step))
	{
		os << LogIO::SEVERE << "No channels selected." << LogIO::POST;
		ok = false;
	}

	if (baseline != "")
	{
		Vector<Int> antid(0);
		Vector<String> antstr(1, baseline);
		selectAntenna(antid, antstr);
	}

	scanString_p = scan;
	intentString_p = intent;
	obsString_p = obs;
	uvrangeString_p = uvrange;
	taqlString_p = taql;
	feedString_p = feed;

	if (subarray != "") selectArray(subarray);

	if (!selectCorrelations(correlation))
	{
		os << LogIO::SEVERE << "No correlations selected." << LogIO::POST;
		ok = false;
	}

	return ok;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Bool MSTransformDataHandler::selectSource(const Vector<Int>& fieldid)
{
	LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));

	Bool cando = true;

	if (fieldid.nelements() < 1)
	{
		fieldid_p = Vector<Int> (1, -1);
	}
	else if (fieldid.nelements() > ms_p.field().nrow())
	{
		os << LogIO::SEVERE << "More fields were requested than are in the input MS." << LogIO::POST;
		cando = false;
	}
	else if (max(fieldid) >= static_cast<Int> (ms_p.field().nrow()))
	{
		// Arriving here is very unlikely since if fieldid came from MSSelection
		// bad fields were presumably already quietly dropped.
		os << LogIO::SEVERE << "At least 1 field was requested that is not in the input MS." << LogIO::POST;
		cando = false;
	}
	else
	{
		fieldid_p = fieldid;
	}

	if (fieldid_p.nelements() == 1 && fieldid_p[0] < 0)
	{
		fieldid_p.resize(ms_p.field().nrow());
		indgen(fieldid_p);
	}

	return cando;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Bool MSTransformDataHandler::selectSpw(const String& spwstr,const Vector<Int>& steps)
{
	LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));

	MSSelection mssel;
	String myspwstr(spwstr == "" ? "*" : spwstr);
	spwString_p = myspwstr; 
	
	mssel.setSpwExpr(myspwstr);

	widths_p = steps.copy();
	if (widths_p.nelements() < 1)
	{
		widths_p.resize(1);
		widths_p[0] = 1;
	}
	else
	{
		for (uInt k = 0; k < widths_p.nelements(); ++k)
		{
			if (widths_p[k] == 0)
			{
				os << LogIO::WARN << "0 cannot be used for channel width...using 1 instead." << LogIO::POST;
				widths_p[k] = 1;
			}
		}
	}

	// Each row should have spw, start, stop, step
	// A single width is a default, but multiple widths should be used literally.
	Matrix<Int> chansel = mssel.getChanList(&ms_p, 1);

	if (chansel.nrow() > 0)
	{
		// Use myspwstr if it selected anything...
		spw_p = chansel.column(0);
		chanStart_p = chansel.column(1);
		chanEnd_p = chansel.column(2);
		chanStep_p = chansel.column(3);

		uInt nspw = chanEnd_p.nelements();
		nchan_p.resize(nspw);

		// A single width is a default, but multiple widths should be used literally.
		if (widths_p.nelements() > 1 && widths_p.nelements() != spw_p.nelements())
		{
			os 		<< LogIO::SEVERE
					<< "Mismatch between the # of widths specified by width and the # of spws."
					<< LogIO::POST;
			return false;
		}

		// Copy the default width to all spws.
		if (widths_p.nelements() < nspw)
		{
			widths_p.resize(nspw, true);
			for (uInt k = 1; k < nspw; ++k)
			{
				widths_p[k] = widths_p[0];
			}
		}

		for (uInt k = 0; k < nspw; ++k)
		{
			// CAS-2224, triggered by spw='0:2' (as opposed to '0:2~2').
			if (chanStep_p[k] == 0) chanStep_p[k] = 1;

			nchan_p[k] = 1 + (chanEnd_p[k] - chanStart_p[k]) / (chanStep_p[k]* widths_p[k]);
			if (nchan_p[k] < 1) nchan_p[k] = 1;
		}
	}
	else
	{
		// Select everything and rely on widths.
		MSSpWindowColumns mySpwTab(ms_p.spectralWindow());
		uInt nspw = mySpwTab.nrow();

		nchan_p = mySpwTab.numChan().getColumn();

		spw_p.resize(nspw);
		indgen(spw_p);

		chanStart_p.resize(nspw);
		chanStep_p.resize(nspw);
		for (uInt k = 0; k < nspw; ++k)
		{
			chanStart_p[k] = 0;
			chanEnd_p[k] = nchan_p[k] - 1;
			chanStep_p[k] = 1;
		}

		if (widths_p.nelements() != spw_p.nelements())
		{
			if (widths_p.nelements() == 1)
			{
				widths_p.resize(spw_p.nelements(), true);
				for (uInt k = 1; k < spw_p.nelements(); ++k)
				{
					widths_p[k] = widths_p[0];
				}

			}
			else
			{
				os 		<< LogIO::SEVERE
						<< "Mismatch between the # of widths specified by width and the # of spws."
						<< LogIO::POST;
				return false;
			}
		}

		for (uInt k = 0; k < nspw; ++k)
		{
			nchan_p[k] = 1 + (nchan_p[k] - 1) / widths_p[k];
		}
	}

	// Check for and filter out selected spws that aren't included in DATA_DESCRIPTION.
	// (See CAS-1673 for an example.)
	std::set<Int> selectedSpwNotInDD(MSTransformDataHandler::findSpwsNotInDD(ms_p, spw_p));
	uInt nSelectedSpwNotInDD = selectedSpwNotInDD.size();
	if (nSelectedSpwNotInDD > 0)
	{
		os << LogIO::NORMAL << "The following a priori selected input spw(s)\n";
		for (std::set<Int>::iterator spwit = selectedSpwNotInDD.begin();
		     spwit != selectedSpwNotInDD.end(); ++spwit)
		{
			os << spw_p[*spwit] << " ";
		}
		os << "\nwere not found in DATA_DESCRIPTION (i. e. no rows in the main "
		      "table reference them) and therefore "
		      "are not included to the output." << LogIO::POST;

		uInt nSelSpw = spw_p.nelements();
		uInt ngoodSelSpwSlots = nSelSpw - nSelectedSpwNotInDD;
		Vector<Int> spwc(ngoodSelSpwSlots);
		Vector<Int> chanStartc(ngoodSelSpwSlots);
		Vector<Int> chanEndc(ngoodSelSpwSlots);
		Vector<Int> nchanc(ngoodSelSpwSlots);
		Vector<Int> chanStepc(ngoodSelSpwSlots);
		std::set<Int>::iterator spwNotDDEnd = selectedSpwNotInDD.end();

		uInt j = 0;
		for (uInt k = 0; k < nSelSpw; ++k)
		{
			if (selectedSpwNotInDD.find(k) == spwNotDDEnd)
			{
				spwc[j] = spw_p[k];
				chanStartc[j] = chanStart_p[k];
				chanEndc[j] = chanEnd_p[k];
				nchanc[j] = nchan_p[k];
				chanStepc[j] = chanStep_p[k];
				++j;
			}
		}
		spw_p.resize(ngoodSelSpwSlots);
		spw_p = spwc;
		chanStart_p.resize(ngoodSelSpwSlots);
		chanStart_p = chanStartc;
		chanEnd_p.resize(ngoodSelSpwSlots);
		chanEnd_p = chanEndc;
		nchan_p.resize(ngoodSelSpwSlots);
		nchan_p = nchanc;
		chanStep_p.resize(ngoodSelSpwSlots);
		chanStep_p = chanStepc;
	}

	mssel.getChanSlices(chanSlices_p, &ms_p, 1);
	return true;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
std::set<Int> MSTransformDataHandler::findSpwsNotInDD(MeasurementSet& ms,Vector<Int> spwv)
{
	ScalarColumn<Int> spws_in_dd(	ms.dataDescription(),
									MSDataDescription::columnName(MSDataDescription::SPECTRAL_WINDOW_ID));
	std::set<Int> uniqSpwsInDD;
	uInt nspwsInDD = spws_in_dd.nrow();

	for (uInt ddrow = 0; ddrow < nspwsInDD; ++ddrow)
	{
		uniqSpwsInDD.insert(spws_in_dd(ddrow));
	}

	std::set<Int> badSelSpwSlots;
	std::set<Int>::iterator ddend = uniqSpwsInDD.end();
	for (uInt k = 0; k < spwv.nelements(); ++k)
	{
		if (uniqSpwsInDD.find(spwv[k]) == ddend)
		{
			badSelSpwSlots.insert(k);
		}
	}

	return badSelSpwSlots;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void MSTransformDataHandler::selectAntenna(const Vector<Int>& antennaids,const Vector<String>& antennaSel)
{
	antennaSel_p = MSTransformDataHandler::pickAntennas(antennaId_p, antennaSelStr_p, antennaids,antennaSel);
	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Bool MSTransformDataHandler::pickAntennas(	Vector<Int>& selected_antennaids,
			 	 	 	 	 	 	 	 	Vector<String>& selected_antenna_strs,
			 	 	 	 	 	 	 	 	const Vector<Int>& antennaids,
			 	 	 	 	 	 	 	 	const Vector<String>& antennaSel)
{
	Bool didSelect = true;

	if ((antennaids.nelements() == 1) && (antennaids[0] == -1))
	{
		if (antennaSel[0] == "")
		{
			didSelect = false;
		}
		else
		{
			selected_antennaids.resize();
		}
	}
	else
	{
		selected_antennaids = antennaids;
	}

	selected_antenna_strs = antennaSel;

	return didSelect;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void MSTransformDataHandler::selectArray(const String& subarray)
{
	arrayExpr_p = subarray;
	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Bool MSTransformDataHandler::selectCorrelations(const String& corrstr)
{
    LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));

    corrString_p = corrstr;
    const Bool areSelecting = corrstr != "" && corrstr != "*";

    // Get correlation slices
    MSSelection mssel1;
    if (areSelecting) mssel1.setPolnExpr(corrstr.c_str());
    mssel1.getCorrSlices(corrSlices_p, &ms_p);

    // Get correlation map
    // jagonzal (CAS-6951): We have to use another MSSelection because the first one corrupts the correlation
    //                      expression for instance "XX;YY" is turned into "XX" after calling getCorrSlices
    MSSelection mssel2;
    if (areSelecting) mssel2.setPolnExpr(corrstr.c_str());
    return MSTransformDataHandler::getCorrMaps(mssel2, ms_p, inPolOutCorrToInCorrMap_p, areSelecting);
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Bool MSTransformDataHandler::getCorrMaps(MSSelection& mssel,
                                         const MeasurementSet& ms,
                                         Vector<Vector<Int> >& outToIn,
                                         const Bool areSelecting)
{

    // ?? This always returns true!!!?!!
    Bool cando = true;

    // The total number of polids
    uInt npol = ms.polarization().nrow();

    // Nominally empty selection for all polids
    outToIn.resize(npol);
    outToIn.set(Vector<Int> ());
    if (areSelecting)
    {
        // Get the corr indices as an ordered map
        std::map<Int, Vector<Vector<Int> > > corrmap(mssel.getCorrMap(&ms));

        // Iterate over the ordered map to fill the vector maps
        for ( auto mi = corrmap.begin( ); mi != corrmap.end( ); ++mi)
        {
            Int pol = mi->first;
            std::vector<int> correlations_idx = mi->second[0].tovector();
            std::sort(correlations_idx.begin(), correlations_idx.end());
            outToIn[pol] = Vector<Int>(correlations_idx);
        }
    }
    else
    { 	// Make outToIn an identity map.
        ScalarColumn<Int> numCorr(ms.polarization(),MSPolarization::columnName(MSPolarization::NUM_CORR));

        for (uInt polid = 0; polid < npol; ++polid)
        {
            uInt ncorr = numCorr(polid);
            outToIn[polid].resize(ncorr);
            for (uInt cid = 0; cid < ncorr; ++cid)
            {
                outToIn[polid][cid] = cid;
            }
        }
    }

    return cando;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void MSTransformDataHandler::selectTime(Double timeBin, String timerng)
{
	timeBin_p   = timeBin;
	timeRange_p = timerng;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Bool MSTransformDataHandler::makeMSBasicStructure(String& msname,
                                                  String& colname,
                                                  casacore::Bool createWeightSpectrumCols,
                                                  const Vector<Int>& tileShape,
                                                  const String& combine,
                                                  Table::TableOption option)
{
	LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));
        os << LogIO::DEBUG1 << "Preparing to setup output MS with createWeightSpectrumCols: "
           << createWeightSpectrumCols << LogIO::POST;;

	if ((spw_p.nelements() > 0) && (max(spw_p) >= Int(ms_p.spectralWindow().nrow())))
	{
		os << LogIO::SEVERE << "SpectralWindow selection contains elements that do not exist in this MS" << LogIO::POST;
		ms_p = MeasurementSet();
		return false;
	}

	// Watch out!  This throws an AipsError if ms_p doesn't have the requested columns.
	const Vector<MS::PredefinedColumns> colNamesTok = parseColumnNames(colname,ms_p,virtualModelCol_p,virtualCorrectedCol_p);

	if (!makeSelection())
	{
		ms_p = MeasurementSet();
		throw(MSSelectionNullSelection("MSSelectionNullSelection : The selected table has zero rows."));
		return false;
	}

	mscIn_p = new MSColumns(mssel_p);

	// Note again the parseColumnNames() a few lines back that stops setupMS()
	// from being called if the MS doesn't have the requested columns.
	MeasurementSet* outpointer = 0;

	if (tileShape.nelements() == 3)
	{
		outpointer = setupMS(msname, nchan_p[0], ncorr_p[0], colNamesTok,
                                     createWeightSpectrumCols, tileShape);
	}

	// the following calls MSTileLayout...  disabled for now because it
	// forces tiles to be the full spw bandwidth in width (gmoellen, 2010/11/07)
	else if ((tileShape.nelements() == 1) && (tileShape[0] == 0 || tileShape[0]== 1))
	{
		outpointer = setupMS(msname, nchan_p[0], ncorr_p[0],
                                     mscIn_p->observation().telescopeName()(0),
                                     colNamesTok, createWeightSpectrumCols, tileShape[0]);
	}
	else {
		// Sweep all other cases of bad tileshape to a default one.
		// (this probably never happens)
		outpointer = setupMS(msname, nchan_p[0], ncorr_p[0],
                                     mscIn_p->observation().telescopeName()(0),
                                     colNamesTok, createWeightSpectrumCols, 0);
	}

	combine_p = combine;

	msOut_p = *outpointer;

	// handle column keywords copy for CORRECTED_DATA -> DATA
	if (colNamesTok.nelements() == 1 && colNamesTok[0] == MS::CORRECTED_DATA && mssel_p.isColumn(MS::CORRECTED_DATA)) {
	    TableColumn outCol(msOut_p, "DATA");
	    TableColumn inCol(mssel_p, "CORRECTED_DATA");
	    // Copy the keywords CORRECTED_DATA -> DATA
	    copyMainTableKeywords(outCol.rwKeywordSet(), inCol.keywordSet());
	}

	Bool ret = true;
	try
	{
		if (option == Table::Scratch)
		{
			// Set up pointing (has to be done in the copied MS)
			SetupNewTable pointingSetup(msOut_p.pointingTableName(),MSPointing::requiredTableDesc(), Table::New);
			msOut_p.rwKeywordSet().defineTable(MS::keywordName(MS::POINTING),Table(pointingSetup));
			msOut_p.initRefs();

			// Add additional columns to SPECTRAL_WINDOW sub-table
			addOptionalColumns(mssel_p.spectralWindow(), msOut_p.spectralWindow(), true);

			// Initialize output MS Columns
			msc_p = new MSColumns(msOut_p);

			// Write transformed SPECTRAL_WINDOW, DATA_DESCRIPTION_ID and POLARIZATION
			ret &= fillDDTables();
		}
		else
		{
			ret = fillSubTables(colNamesTok);
		}
	}
	catch (AipsError ex)
	{
		ret = false;
		os 	<< LogIO::SEVERE
			<< "Exception filling the sub-tables: " << ex.getMesg() << endl
			<< "Stack Trace: " << ex.getStackTrace()
			<< LogIO::POST;
	}

	if (!ret)
	{
		delete outpointer;
		ms_p = MeasurementSet();
		msOut_p = MeasurementSet();
		os << LogIO::SEVERE << msname << " left unfinished." << LogIO::POST;
		return false;
	}

	//Detaching the selected part
	ms_p = MeasurementSet();

	delete outpointer;
	return true;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Bool MSTransformDataHandler::isAllColumns(const Vector<MS::PredefinedColumns>& colNames)
{
	Bool dCol = false, mCol = false, cCol = false;
	for (uInt i = 0; i < colNames.nelements(); i++)
	{
		if (colNames[i] == MS::DATA) dCol = true;
		else if (colNames[i] == MS::MODEL_DATA) mCol = true;
		else if (colNames[i] == MS::CORRECTED_DATA) cCol = true;
		// else turn off all?
	}

	return (dCol && mCol && cCol);
}

// -----------------------------------------------------------------------
// Modified version of makeSelection that uses the new getter methods
// MSS::getSPWDDIDList() and MSS::getDDIDList()
// -----------------------------------------------------------------------
Bool MSTransformDataHandler::makeSelection()
{

	LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));

	// VisSet/MSIter will check if the SORTED exists
	// jagonzal (CAS-5327): Commenting this out, since this implies all sorts of memory leaks
	// Block<Int> sort;
	// ROVisibilityIterator(ms_p, sort);

	const MeasurementSet *elms;
	elms = &ms_p;
	MeasurementSet sorted;
	if (ms_p.keywordSet().isDefined("SORTED_TABLE"))
	{
		sorted = ms_p.keywordSet().asTable("SORTED_TABLE");

		//If ms is not writable and sort is a subselection...use original ms
		if (ms_p.nrow() == sorted.nrow()) elms = &sorted;
	}

	MSSelection thisSelection;
	if (fieldid_p.nelements() > 0)
	{
		thisSelection.setFieldExpr(MSSelection::indexExprStr(fieldid_p));
	}

	if (spw_p.nelements() > 0)
	{
		thisSelection.setSpwExpr(spwString_p);
	}

	if (antennaSel_p)
	{
		if (antennaId_p.nelements() > 0)
		{
			thisSelection.setAntennaExpr(MSSelection::indexExprStr(antennaId_p));
		}
		if (antennaSelStr_p[0] != "")
		{
			thisSelection.setAntennaExpr(MSSelection::nameExprStr(antennaSelStr_p));
		}

	}

	if (timeRange_p != "")
	{
		thisSelection.setTimeExpr(timeRange_p);
	}


	thisSelection.setUvDistExpr(uvrangeString_p);
	thisSelection.setScanExpr(scanString_p);
	thisSelection.setStateExpr(intentString_p);
	thisSelection.setObservationExpr(obsString_p);

	if (arrayExpr_p != "")
	{
		thisSelection.setArrayExpr(arrayExpr_p);
	}

	if (corrString_p != "")
	{
		thisSelection.setPolnExpr(corrString_p.c_str());
	}

	thisSelection.setTaQLExpr(taqlString_p);
	thisSelection.setFeedExpr(feedString_p);

	TableExprNode exprNode = thisSelection.toTableExprNode(elms);
	selTimeRanges_p = thisSelection.getTimeList();
	selObsId_p = thisSelection.getObservationList();

	// Get the list of DDI for the selected spws
	spw2ddid_p = thisSelection.getSPWDDIDList(elms);

	const MSDataDescription ddtable = elms->dataDescription();
	ScalarColumn<Int> polId(ddtable,MSDataDescription::columnName(MSDataDescription::POLARIZATION_ID));
	const MSPolarization poltable = elms->polarization();
	ArrayColumn<Int> pols(poltable,MSPolarization::columnName(MSPolarization::CORR_TYPE));

	// Get the list of DDI for the selected polarizations
	Vector<Int> polDDIList = thisSelection.getDDIDList(elms);

	// When polDDIList is empty, do not do an intersection
	Bool doIntersection = true;

	if (polDDIList.size() == 0){
		doIntersection = false;
	}

	// intersection between selected DDI from spw selection and
	// selected DDI from polarization selection
	if (doIntersection) {
		Vector<Int> intersectedDDI = set_intersection(spw2ddid_p, polDDIList);
		uInt nddids = intersectedDDI.size();
		if (nddids > 0){
			spw2ddid_p.resize(nddids);
			for (uInt ii = 0; ii < nddids; ++ii){
				spw2ddid_p[ii] = intersectedDDI[ii];
			}
		}
		else {
			os 	<< LogIO::SEVERE << "None of the selected correlations are in spectral window "
				<< LogIO::POST;
		}
	}


	// This is actually the number of selected DDI
	uInt nDDIs = spw2ddid_p.size();

	inNumCorr_p.resize(nDDIs);
	ncorr_p.resize(nDDIs);

	// Map the correlations from input selected DDI to output
	for (uInt k = 0; k < nDDIs; ++k)
	{
		Int ddid = spw2ddid_p[k];

		// Number of input correlations for each DDI
		// It reads the nelements of the CORR_TYPE column cell
		inNumCorr_p[k] = pols(polId(ddid)).nelements();

		// Corresponding number of output correlations for each DDI
		ncorr_p[k] = inPolOutCorrToInCorrMap_p[polId(ddid)].nelements();
		if (ncorr_p[k] == 0)
		{
			os 		<< LogIO::SEVERE
					<< "None of the selected correlations are in spectral window "
					<< spw_p[k] << LogIO::POST;
			return false;
		}
	}


	// Now remake the selected ms
	if (!(exprNode.isNull()))
	{
		mssel_p = MeasurementSet((*elms)(exprNode));
	}
	else
	{
		// Null take all the ms ...setdata() blank means that
		mssel_p = MeasurementSet((*elms));
	}

	if (mssel_p.nrow() == 0) return false;

	// Setup antNewIndex_p now that mssel_p is ready.
	if (antennaSel_p and reindex_p)
	{
		/*
		// Watch out! getAntenna*List() and getBaselineList() return negative numbers for negated antennas!
		ScalarColumn<Int> ant1c(mssel_p, MS::columnName(MS::ANTENNA1));
		ScalarColumn<Int> ant2c(mssel_p, MS::columnName(MS::ANTENNA2));
		Vector<Int> selAnts(ant1c.getColumn());
		uInt nAnts = selAnts.nelements();

		selAnts.resize(2 * nAnts, true);
		selAnts(Slice(nAnts, nAnts)) = ant2c.getColumn();
		nAnts = GenSort<Int>::sort(selAnts, Sort::Ascending,Sort::NoDuplicates);
		selAnts.resize(nAnts, true);
		Int maxAnt = max(selAnts);
		*/

		// jagonzal: Scanning the main table is extremely inefficient, and depends on TaQL
		//           Therefore simply remove the negated antennas from getAntenna1List
		vector<Int> antsSel;

		// Get antennas selected on position 1
		Vector<Int> ant1List = thisSelection.getAntenna1List();
		for (uInt idx=0;idx<ant1List.size();idx++)
		{
			if (ant1List(idx) >= 0) antsSel.push_back(ant1List(idx));
		}

		// Get antennas selected on position 2
		Vector<Int> ant2List = thisSelection.getAntenna2List();
		for (uInt idx=0;idx<ant2List.size();idx++)
		{
			if (ant2List(idx) >= 0) antsSel.push_back(ant2List(idx));
		}

		// Sort and remove duplicates
		std::sort(antsSel.begin(), antsSel.end());
		antsSel.erase(std::unique(antsSel.begin(), antsSel.end()),antsSel.end());

		Vector<Int> selAnts(antsSel);
		uInt nAnts = selAnts.size();
		Int maxAnt = max(selAnts);

		if (maxAnt < 0)
		{
			os 	<< LogIO::SEVERE
				<< "The maximum selected antenna number, " << maxAnt
				<< ", seems to be < 0."
				<< LogIO::POST;
			return false;
		}

		antNewIndex_p.resize(maxAnt + 1);
		//So if you see -1 in the main, feed, or pointing tables, fix it
		antNewIndex_p.set(-1);

    for (uInt k = 0; k < nAnts; ++k)
      antNewIndex_p[selAnts[k]] = k;

    //If the total number of output antennas is the same as the input antennas
    //this means that the selection of baselines includes at the end
    //all the input antennas. Therefore setting antenna selection to false.
    //See CAS-11111
    if(nAnts == elms->antenna().nrow())
      antennaSel_p = false;
	}
	// This still gets tripped up by VLA:OUT.
	else
	{
		// Make a default antNewIndex_p.
		antNewIndex_p.resize(mssel_p.antenna().nrow());
		indgen(antNewIndex_p);
	}

	if (mssel_p.nrow() < ms_p.nrow())
	{
		os 		<< LogIO::NORMAL
				<< mssel_p.nrow() << " out of " << ms_p.nrow()
				<< " rows are going to be considered due to the selection criteria."
				<< LogIO::POST;
	}

	return true;
}


// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
MeasurementSet* MSTransformDataHandler::setupMS(const String& MSFileName, const Int nchan,
                                                const Int nCorr, const String& telescop,
                                                const Vector<MS::PredefinedColumns>& colNames,
                                                casacore::Bool createWeightSpectrumCols,
                                                const Int obstype, const Bool compress,
                                                const asdmStManUseAlternatives asdmStManUse,
                                                Table::TableOption option)
 {
	//Choose an appropriate tileshape
	IPosition dataShape(2, nCorr, nchan);
	IPosition tileShape = MSTileLayout::tileShape(dataShape, obstype, telescop);
	return setupMS(MSFileName, nchan, nCorr, colNames, createWeightSpectrumCols,
                       tileShape.asVector(),compress, asdmStManUse,option);
 }

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
 MeasurementSet* MSTransformDataHandler::setupMS(const String& MSFileName, const Int nchan,
                                                 const Int nCorr,
                                                 const Vector<MS::PredefinedColumns>& colNamesTok,
                                                 casacore::Bool createWeightSpectrumCols,
                                                 const Vector<Int>& tshape, const Bool compress,
                                                 const asdmStManUseAlternatives asdmStManUse,
                                                 Table::TableOption option)
 {
	if (tshape.nelements() != 3) throw(AipsError("TileShape has to have 3 elements "));

	// This is more to shush a compiler warning than to warn users.
	LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));
	if (tshape[0] != nCorr)
		os << LogIO::DEBUG1 << "Warning: using " << tshape[0]
				<< " from the tileshape instead of " << nCorr
				<< " for the number of correlations." << LogIO::POST;
	if (tshape[1] != nchan)
		os << LogIO::DEBUG1 << "Warning: using " << tshape[1]
				<< " from the tileshape instead of " << nchan
				<< " for the number of channels." << LogIO::POST;

	// Choose an appropriate tileshape //////////////////

	IPosition tileShape(tshape);

	// Make the MS table
	TableDesc td = MS::requiredTableDesc();
	Vector<String> tiledDataNames;

	if (option == Table::Scratch)
	{
		SetupNewTable newtab(MSFileName, td, option);
		TableLock lock(TableLock::AutoLocking);
		MeasurementSet *ms = new MeasurementSet(newtab, lock);

		// Set up default sub-tables for the MS
		SetupNewTable dataDescSetup(ms->dataDescriptionTableName(),MSDataDescription::requiredTableDesc(), option);
		ms->rwKeywordSet().defineTable(MS::keywordName(MS::DATA_DESCRIPTION),Table(dataDescSetup));
		SetupNewTable polarizationSetup(ms->polarizationTableName(),MSPolarization::requiredTableDesc(), option);
		ms->rwKeywordSet().defineTable(MS::keywordName(MS::POLARIZATION),Table(polarizationSetup));
		SetupNewTable spectralWindowSetup(ms->spectralWindowTableName(),MSSpectralWindow::requiredTableDesc(), option);
		ms->rwKeywordSet().defineTable(MS::keywordName(MS::SPECTRAL_WINDOW),Table(spectralWindowSetup));
		ms->initRefs();

		return ms;
	}

	// Even though we know the data is going to be the same shape throughout I'll
	// still create a column that has a variable shape as this will permit MS's
	// with other shapes to be appended.
	uInt ncols = colNamesTok.nelements();
	const Bool mustWriteOnlyToData = mustConvertToData(ncols, colNamesTok);

	if (mustWriteOnlyToData)
	{
		MS::addColumnToDesc(td, MS::DATA, 2);
		if (asdmStManUse == DONT)
		{
			if (compress) MS::addColumnCompression(td, MS::DATA, true);
			String hcolName = String("Tiled") + String("DATA");
			td.defineHypercolumn(hcolName, 3, stringToVector("DATA"));
			tiledDataNames.resize(1);
			tiledDataNames[0] = hcolName;
		}
	}
	else
	{
		tiledDataNames.resize(ncols);
		for (uInt i = 0; i < ncols; ++i)
		{
			// Unfortunately MS::PredefinedColumns aren't ordered so that
			// I can just check if colNamesTok[i] is in the "data range".
			if (	colNamesTok[i] == MS::DATA
					|| colNamesTok[i] == MS::MODEL_DATA
					|| colNamesTok[i] == MS::CORRECTED_DATA
					|| colNamesTok[i] == MS::FLOAT_DATA
					|| colNamesTok[i] == MS::LAG_DATA)
			{
				if (asdmStManUse == DONT || colNamesTok[i] != MS::DATA)
				{
					MS::addColumnToDesc(td, colNamesTok[i], 2);
					if (compress) MS::addColumnCompression(td, colNamesTok[i], true);
				}
			}
			else
			{
				throw(AipsError( MS::columnName(colNamesTok[i]) + " is not a recognized data column "));
			}
			if (asdmStManUse == DONT || colNamesTok[i] != MS::DATA)
			{
				String hcolName = String("Tiled") + MS::columnName(colNamesTok[i]);
				td.defineHypercolumn(hcolName, 3,stringToVector(MS::columnName(colNamesTok[i])));
				tiledDataNames[i] = hcolName;
			}
		}
	}

	//other cols for compression
	if (compress && asdmStManUse != USE_FOR_DATA_WEIGHT_SIGMA_FLAG)
	{
		MS::addColumnCompression(td, MS::WEIGHT, true);
		MS::addColumnCompression(td, MS::SIGMA, true);
	}

        if (createWeightSpectrumCols) {
            MS::addColumnToDesc(td, MS::WEIGHT_SPECTRUM, 2);
            MS::addColumnToDesc(td, MS::SIGMA_SPECTRUM, 2);

            td.defineHypercolumn("TiledWgtSpectrum", 3,stringToVector(MS::columnName(MS::WEIGHT_SPECTRUM)));
            td.defineHypercolumn("TiledSigmaSpectrum", 3,stringToVector(MS::columnName(MS::SIGMA_SPECTRUM)));
        }

	td.defineHypercolumn("TiledFlagCategory", 4,stringToVector(MS::columnName(MS::FLAG_CATEGORY)));
	td.defineHypercolumn("TiledUVW", 2, stringToVector(MS::columnName(MS::UVW)));

	if (asdmStManUse != USE_FOR_DATA_WEIGHT_SIGMA_FLAG)
	{
		td.defineHypercolumn("TiledFlag", 3,stringToVector(MS::columnName(MS::FLAG)));
		td.defineHypercolumn("TiledWgt", 2,stringToVector(MS::columnName(MS::WEIGHT)));
		td.defineHypercolumn("TiledSigma", 2,stringToVector(MS::columnName(MS::SIGMA)));
	}

	SetupNewTable newtab(MSFileName, td, option);

	uInt cache_val = 32768;

	// Set the default Storage Manager to be the Incr one
	IncrementalStMan incrStMan("ISMData", cache_val);
	newtab.bindAll(incrStMan, true);

	//Override the binding for specific columns
	IncrementalStMan incrStMan0("Array_ID", cache_val);
	newtab.bindColumn(MS::columnName(MS::ARRAY_ID), incrStMan0);
	IncrementalStMan incrStMan1("EXPOSURE", cache_val);
	newtab.bindColumn(MS::columnName(MS::EXPOSURE), incrStMan1);
	IncrementalStMan incrStMan2("FEED1", cache_val);
	newtab.bindColumn(MS::columnName(MS::FEED1), incrStMan2);
	IncrementalStMan incrStMan3("FEED2", cache_val);
	newtab.bindColumn(MS::columnName(MS::FEED2), incrStMan3);
	IncrementalStMan incrStMan4("FIELD_ID", cache_val);
	newtab.bindColumn(MS::columnName(MS::FIELD_ID), incrStMan4);

    // jagonzal (CAS-6746): Don't use IncrementalStMan with RW cols ///////////////////////////////////////////////////
    // IncrementalStMan incrStMan5("FLAG_ROW",cache_val/4);
    // newtab.bindColumn(MS::columnName(MS::FLAG_ROW), incrStMan5);
    StandardStMan aipsStManFlagRow("FLAG_ROW", cache_val/4);
    newtab.bindColumn(MS::columnName(MS::FLAG_ROW), aipsStManFlagRow);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	IncrementalStMan incrStMan6("INTERVAL", cache_val);
	newtab.bindColumn(MS::columnName(MS::INTERVAL), incrStMan6);
	IncrementalStMan incrStMan7("OBSERVATION_ID", cache_val);
	newtab.bindColumn(MS::columnName(MS::OBSERVATION_ID), incrStMan7);
	IncrementalStMan incrStMan8("PROCESSOR_ID", cache_val);
	newtab.bindColumn(MS::columnName(MS::PROCESSOR_ID), incrStMan8);
	IncrementalStMan incrStMan9("SCAN_NUMBER", cache_val);
	newtab.bindColumn(MS::columnName(MS::SCAN_NUMBER), incrStMan9);
	IncrementalStMan incrStMan10("STATE_ID", cache_val);
	newtab.bindColumn(MS::columnName(MS::STATE_ID), incrStMan10);
	IncrementalStMan incrStMan11("TIME", cache_val);
	newtab.bindColumn(MS::columnName(MS::TIME), incrStMan11);
	IncrementalStMan incrStMan12("TIME_CENTROID", cache_val);
	newtab.bindColumn(MS::columnName(MS::TIME_CENTROID), incrStMan12);

	// Bind ANTENNA1, ANTENNA2 and DATA_DESC_ID to the standardStMan
	// as they may change sufficiently frequently to make the
	// incremental storage manager inefficient for these columns.
	StandardStMan aipsStMan0("ANTENNA1", cache_val);
	newtab.bindColumn(MS::columnName(MS::ANTENNA1), aipsStMan0);
	StandardStMan aipsStMan1("ANTENNA2", cache_val);
	newtab.bindColumn(MS::columnName(MS::ANTENNA2), aipsStMan1);
	StandardStMan aipsStMan2("DATA_DESC_ID", cache_val);
	newtab.bindColumn(MS::columnName(MS::DATA_DESC_ID), aipsStMan2);

	// Bind the DATA, FLAG & WEIGHT/SIGMA_SPECTRUM columns to the tiled stman or
        // asdmStMan
	AsdmStMan sm;

	if (mustWriteOnlyToData)
	{
		if (asdmStManUse == DONT)
		{
			TiledShapeStMan tiledStMan1Data("TiledDATA", tileShape);
			newtab.bindColumn(MS::columnName(MS::DATA), tiledStMan1Data);
		}
		else
		{
			newtab.bindColumn(MS::columnName(MS::DATA), sm);
		}
	}
	else
	{
		for (uInt i = 0; i < ncols; ++i)
		{
			TiledShapeStMan tiledStMan1Data(tiledDataNames[i], tileShape);
			newtab.bindColumn(MS::columnName(colNamesTok[i]), tiledStMan1Data);
		}
		if (asdmStManUse != DONT)
		{
			newtab.bindColumn(MS::columnName(MS::DATA), sm);
		}
	}

	TiledShapeStMan tiledStMan1fc("TiledFlagCategory",IPosition(4, tileShape(0), tileShape(1), 1, tileShape(2)));
	newtab.bindColumn(MS::columnName(MS::FLAG_CATEGORY), tiledStMan1fc);

        if (createWeightSpectrumCols) {
            TiledShapeStMan tiledStMan2("TiledWgtSpectrum", tileShape);
            TiledShapeStMan tiledStMan6("TiledSigmaSpectrum", tileShape);
            newtab.bindColumn(MS::columnName(MS::WEIGHT_SPECTRUM), tiledStMan2);
            newtab.bindColumn(MS::columnName(MS::SIGMA_SPECTRUM), tiledStMan6);
        }

        TiledColumnStMan tiledStMan3("TiledUVW",IPosition(2, 3, (tileShape(0) * tileShape(1) * tileShape(2)) / 3));
	newtab.bindColumn(MS::columnName(MS::UVW), tiledStMan3);
	if (asdmStManUse == USE_FOR_DATA_WEIGHT_SIGMA_FLAG)
	{
		newtab.bindColumn(MS::columnName(MS::FLAG), sm);
		newtab.bindColumn(MS::columnName(MS::WEIGHT), sm);
		newtab.bindColumn(MS::columnName(MS::SIGMA), sm);
	}
	else
	{
            TiledShapeStMan tiledStMan1f("TiledFlag", tileShape);
            TiledShapeStMan tiledStMan4("TiledWgt", IPosition(2, tileShape(0),
                                                              tileShape(1) * tileShape(2)));
            TiledShapeStMan tiledStMan5("TiledSigma",IPosition(2, tileShape(0),
                                                               tileShape(1) * tileShape(2)));

            newtab.bindColumn(MS::columnName(MS::FLAG), tiledStMan1f);
            newtab.bindColumn(MS::columnName(MS::WEIGHT), tiledStMan4);
            newtab.bindColumn(MS::columnName(MS::SIGMA), tiledStMan5);
	}

	// Avoid lock overheads by locking the table permanently
	TableLock lock(TableLock::AutoLocking);
	MeasurementSet *ms = new MeasurementSet(newtab, lock);

	// Set up the sub-tables for the UVFITS MS (we make new tables with 0 rows)
	// Table::TableOption option = Table::New;
	createSubtables(*ms, option);

	// Set the TableInfo
	TableInfo& info(ms->tableInfo());
	info.setType(TableInfo::type(TableInfo::MEASUREMENTSET));
	info.setSubType(String("UVFITS"));
	info.readmeAddLine("This is a measurement set Table holding astronomical observations");

	return ms;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void MSTransformDataHandler::createSubtables(MeasurementSet& ms, Table::TableOption option)
{
	SetupNewTable antennaSetup(ms.antennaTableName(),MSAntenna::requiredTableDesc(), option);
	ms.rwKeywordSet().defineTable(MS::keywordName(MS::ANTENNA),Table(antennaSetup));
	SetupNewTable dataDescSetup(ms.dataDescriptionTableName(),MSDataDescription::requiredTableDesc(), option);
	ms.rwKeywordSet().defineTable(MS::keywordName(MS::DATA_DESCRIPTION),Table(dataDescSetup));
	SetupNewTable feedSetup(ms.feedTableName(), MSFeed::requiredTableDesc(),option);
	ms.rwKeywordSet().defineTable(MS::keywordName(MS::FEED), Table(feedSetup));
	SetupNewTable flagCmdSetup(ms.flagCmdTableName(),MSFlagCmd::requiredTableDesc(), option);
	ms.rwKeywordSet().defineTable(MS::keywordName(MS::FLAG_CMD),Table(flagCmdSetup));
	SetupNewTable fieldSetup(ms.fieldTableName(), MSField::requiredTableDesc(),option);
	ms.rwKeywordSet().defineTable(MS::keywordName(MS::FIELD), Table(fieldSetup));
	SetupNewTable historySetup(ms.historyTableName(),MSHistory::requiredTableDesc(), option);
	ms.rwKeywordSet().defineTable(MS::keywordName(MS::HISTORY),Table(historySetup));
	SetupNewTable observationSetup(ms.observationTableName(),MSObservation::requiredTableDesc(), option);
	ms.rwKeywordSet().defineTable(MS::keywordName(MS::OBSERVATION),Table(observationSetup));
	SetupNewTable polarizationSetup(ms.polarizationTableName(),MSPolarization::requiredTableDesc(), option);
	ms.rwKeywordSet().defineTable(MS::keywordName(MS::POLARIZATION),Table(polarizationSetup));
	SetupNewTable processorSetup(ms.processorTableName(),MSProcessor::requiredTableDesc(), option);
	ms.rwKeywordSet().defineTable(MS::keywordName(MS::PROCESSOR),Table(processorSetup));
	SetupNewTable spectralWindowSetup(ms.spectralWindowTableName(),MSSpectralWindow::requiredTableDesc(), option);
	ms.rwKeywordSet().defineTable(MS::keywordName(MS::SPECTRAL_WINDOW),Table(spectralWindowSetup));
	SetupNewTable stateSetup(ms.stateTableName(), MSState::requiredTableDesc(),option);
	ms.rwKeywordSet().defineTable(MS::keywordName(MS::STATE), Table(stateSetup));

	// Add the optional Source sub table to allow for specification of the rest frequency
	SetupNewTable sourceSetup(ms.sourceTableName(),MSSource::requiredTableDesc(), option);
	ms.rwKeywordSet().defineTable(MS::keywordName(MS::SOURCE),Table(sourceSetup, 0));

	// Update the references to the sub-table keywords
	ms.initRefs();

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Bool MSTransformDataHandler::fillSubTables(const Vector<MS::PredefinedColumns>&)
{
	LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));
	Bool success = true;

	// Copy the sub-tables before doing anything with the main table.
	// Otherwise MSColumns won't work.

	// fill or update
	Timer timer;

	timer.mark();
	success &= copyPointing();
	os << LogIO::DEBUG1 << "copyPointing took " << timer.real() << "s." << LogIO::POST;

	// Optional columns should be set up before msc_p.
	addOptionalColumns(mssel_p.spectralWindow(), msOut_p.spectralWindow(), true);

	// Force the Measures frames for all the time type columns to
	// have the same reference as the TIME column of the main table.
	// Disable the empty table check (with false) because some of the
	// sub-tables (like POINTING) might already have been written.
	// However, empty tables are still empty after setting up the reference codes here.
	msc_p = new MSColumns(msOut_p);
	msc_p->setEpochRef(MEpoch::castType(mscIn_p->timeMeas().getMeasRef().getType()), false);

	// UVW is the only other Measures column in the main table.
	msc_p->uvwMeas().setDescRefCode(Muvw::castType(mscIn_p->uvwMeas().getMeasRef().getType()));

	if (!mscIn_p->flagCategory().isNull() && mscIn_p->flagCategory().isDefined(0))
	{
		msc_p->setFlagCategories(mscIn_p->flagCategories());
	}


	timer.mark();
	if (!fillDDTables()) return false;
	os << LogIO::DEBUG1 << "fillDDTables took " << timer.real() << "s." << LogIO::POST;

	// SourceIDs need to be re-mapped around here
	// (It cannot not be done in selectSource() because mssel_p is not set up yet)
	timer.mark();
	relabelSources();
	os << LogIO::DEBUG1 << "relabelSources took " << timer.real() << "s." << LogIO::POST;

	success &= fillFieldTable();
	success &= copySource();

	success &= copyAntenna();
	// Feed table writing has to be after antenna
	if (!copyFeed()) return false;

	success &= copyFlag_Cmd();
	success &= copyHistory();
	success &= copyObservation();
	success &= copyProcessor();
	success &= copyState();

	timer.mark();
	success &= copySyscal();
	os << LogIO::DEBUG1 << "copySyscal took " << timer.real() << "s." << LogIO::POST;

	timer.mark();
	success &= copyWeather();
	os << LogIO::DEBUG1 << "copyWeather took " << timer.real() << "s." << LogIO::POST;

	timer.mark();
	success &= filterOptSubtable("CALDEVICE");
	os << LogIO::DEBUG1 << "CALDEVICE took " << timer.real() << "s." << LogIO::POST;

	timer.mark();
	success &= filterOptSubtable("SYSPOWER");
	os << LogIO::DEBUG1 << "SYSPOWER took " << timer.real() << "s." << LogIO::POST;

	// Run this after running the other copy*()s.
	// Maybe there should be an option to *not* run it.
	success &= copyGenericSubtables();
	return success;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Bool MSTransformDataHandler::fillFieldTable()
{
	LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));

	uInt nAddedCols = addOptionalColumns(mssel_p.field(), msOut_p.field(), true);

	MSFieldColumns msField(msOut_p.field());

	const MSFieldColumns& fieldIn = mscIn_p->field();
	ScalarColumn<String> code(fieldIn.code());
	ArrayColumn<Double> delayDir(fieldIn.delayDir());
	ScalarColumn<Bool> flagRow(fieldIn.flagRow());
	ScalarColumn<String> name(fieldIn.name());
	ScalarColumn<Int> numPoly(fieldIn.numPoly());
	ArrayColumn<Double> phaseDir(fieldIn.phaseDir());
	ArrayColumn<Double> refDir(fieldIn.referenceDir());
	ScalarColumn<Int> sourceId(fieldIn.sourceId());
	ScalarColumn<Double> time(fieldIn.time());

	String refstr;
	String nameVarRefColDelayDir, nameVarRefColPhaseDir, nameVarRefColRefDir;

	// Need to correctly define the direction measures.

	// DelayDir
	if (delayDir.keywordSet().asRecord("MEASINFO").isDefined("Ref"))
	{
		delayDir.keywordSet().asRecord("MEASINFO").get("Ref", refstr);
		msField.delayDir().rwKeywordSet().asrwRecord("MEASINFO").define("Ref",refstr);
	}

	// it's a variable ref. column
	if (delayDir.keywordSet().asRecord("MEASINFO").isDefined("VarRefCol"))
	{
		delayDir.keywordSet().asRecord("MEASINFO").get("VarRefCol", refstr);
		msField.delayDir().rwKeywordSet().asrwRecord("MEASINFO").define("VarRefCol", refstr);
		nameVarRefColDelayDir = refstr;

		Vector<String> refTypeV;
		delayDir.keywordSet().asRecord("MEASINFO").get("TabRefTypes", refTypeV);
		msField.delayDir().rwKeywordSet().asrwRecord("MEASINFO").define("TabRefTypes", refTypeV);

		Vector<uInt> refCodeV;
		delayDir.keywordSet().asRecord("MEASINFO").get("TabRefCodes", refCodeV);
		msField.delayDir().rwKeywordSet().asrwRecord("MEASINFO").define("TabRefCodes", refCodeV);
		Int refid = msField.delayDir().keywordSet().asRecord("MEASINFO").fieldNumber("Ref");

		// Erase the redundant Ref keyword
		if (refid >= 0)
		{
			msField.delayDir().rwKeywordSet().asrwRecord("MEASINFO").removeField(RecordFieldId(refid));
		}
	}

	// PhaseDir
	if (phaseDir.keywordSet().asRecord("MEASINFO").isDefined("Ref"))
	{
		phaseDir.keywordSet().asRecord("MEASINFO").get("Ref", refstr);
		msField.phaseDir().rwKeywordSet().asrwRecord("MEASINFO").define("Ref",refstr);
	}

	// It's a variable ref. column
	if (phaseDir.keywordSet().asRecord("MEASINFO").isDefined("VarRefCol"))
	{
		phaseDir.keywordSet().asRecord("MEASINFO").get("VarRefCol", refstr);
		msField.phaseDir().rwKeywordSet().asrwRecord("MEASINFO").define("VarRefCol", refstr);
		nameVarRefColPhaseDir = refstr;

		Vector<String> refTypeV;
		phaseDir.keywordSet().asRecord("MEASINFO").get("TabRefTypes", refTypeV);
		msField.phaseDir().rwKeywordSet().asrwRecord("MEASINFO").define("TabRefTypes", refTypeV);

		Vector<uInt> refCodeV;
		phaseDir.keywordSet().asRecord("MEASINFO").get("TabRefCodes", refCodeV);
		msField.phaseDir().rwKeywordSet().asrwRecord("MEASINFO").define("TabRefCodes", refCodeV);

		Int refid = msField.phaseDir().keywordSet().asRecord("MEASINFO").fieldNumber("Ref");
		if (refid >= 0)
		{
			msField.phaseDir().rwKeywordSet().asrwRecord("MEASINFO").removeField(RecordFieldId(refid));
		}
	}

	// ReferenceDir
	if (refDir.keywordSet().asRecord("MEASINFO").isDefined("Ref"))
	{
		refDir.keywordSet().asRecord("MEASINFO").get("Ref", refstr);
		msField.referenceDir().rwKeywordSet().asrwRecord("MEASINFO").define(
				"Ref", refstr);
	}

	// It's a variable ref. column
	if (refDir.keywordSet().asRecord("MEASINFO").isDefined("VarRefCol"))
	{
		refDir.keywordSet().asRecord("MEASINFO").get("VarRefCol", refstr);
		msField.referenceDir().rwKeywordSet().asrwRecord("MEASINFO").define("VarRefCol", refstr);
		nameVarRefColRefDir = refstr;

		Vector<String> refTypeV;
		refDir.keywordSet().asRecord("MEASINFO").get("TabRefTypes", refTypeV);
		msField.referenceDir().rwKeywordSet().asrwRecord("MEASINFO").define("TabRefTypes", refTypeV);

		Vector<uInt> refCodeV;
		refDir.keywordSet().asRecord("MEASINFO").get("TabRefCodes", refCodeV);
		msField.referenceDir().rwKeywordSet().asrwRecord("MEASINFO").define("TabRefCodes", refCodeV);

		Int refid = msField.referenceDir().keywordSet().asRecord("MEASINFO").fieldNumber("Ref");
		if (refid >= 0)
		{
			msField.referenceDir().rwKeywordSet().asrwRecord("MEASINFO").removeField(RecordFieldId(refid));
		}
	}

	// ...and the time measure...
	time.keywordSet().asRecord("MEASINFO").get("Ref", refstr);
	msField.time().rwKeywordSet().asrwRecord("MEASINFO").define("Ref", refstr);

	if (!reindex_p)
	{
		const MSField &inputField = mssel_p.field();
		MSField &outputField = msOut_p.field();
		TableCopy::copyRows(outputField, inputField);
		copyEphemerisTable(msField);
		return true;
	}

	// fieldRelabel_p size: nrow of a input MS, -1 for unselected field ids
	fieldRelabel_p.resize(mscIn_p->field().nrow());
	fieldRelabel_p.set(-1);

	os 	<< LogIO::DEBUG1 << fieldid_p.nelements()
		<< " fields selected out of " << mscIn_p->field().nrow()
		<< LogIO::POST;

	try {

		msOut_p.field().addRow(fieldid_p.nelements());

		for (uInt k = 0; k < fieldid_p.nelements(); ++k)
		{
			fieldRelabel_p[fieldid_p[k]] = k;

			msField.code().put(k, code(fieldid_p[k]));
			msField.delayDir().put(k, delayDir(fieldid_p[k]));
			msField.flagRow().put(k, flagRow(fieldid_p[k]));
			msField.name().put(k, name(fieldid_p[k]));
			msField.numPoly().put(k, numPoly(fieldid_p[k]));
			msField.phaseDir().put(k, phaseDir(fieldid_p[k]));
			msField.referenceDir().put(k, refDir(fieldid_p[k]));
			msField.time().put(k, time(fieldid_p[k]));

			Int inSrcID = sourceId(fieldid_p[k]);
			if (inSrcID < 0)
			{
				msField.sourceId().put(k, -1);
			}
			else
			{
				msField.sourceId().put(k, sourceRelabel_p[inSrcID]);
			}
		}

		if (nAddedCols > 0)
		{
			copyEphemerisTable(msField);

			// need to copy the reference column
			if (!nameVarRefColDelayDir.empty())
			{
				ScalarColumn<Int> dM(mssel_p.field(), nameVarRefColDelayDir);
				ScalarColumn<Int> cdMDirRef(msOut_p.field(),nameVarRefColDelayDir);
				for (uInt k = 0; k < fieldid_p.nelements(); ++k)
				{
					cdMDirRef.put(k, dM(fieldid_p[k]));
				}
			}

			// need to copy the reference column
			if (!nameVarRefColPhaseDir.empty())
			{
				ScalarColumn<Int> dM(mssel_p.field(), nameVarRefColPhaseDir);
				ScalarColumn<Int> cdMDirRef(msOut_p.field(),nameVarRefColPhaseDir);
				for (uInt k = 0; k < fieldid_p.nelements(); ++k)
				{
					cdMDirRef.put(k, dM(fieldid_p[k]));
				}
			}

			// need to copy the reference column
			if (!nameVarRefColRefDir.empty())
			{
				ScalarColumn<Int> dM(mssel_p.field(), nameVarRefColRefDir);
				ScalarColumn<Int> cdMDirRef(msOut_p.field(),nameVarRefColRefDir);
				for (uInt k = 0; k < fieldid_p.nelements(); ++k)
				{
					cdMDirRef.put(k, dM(fieldid_p[k]));
				}
			}
		}

	}
	catch (AipsError x)
	{
		os 	<< LogIO::EXCEPTION << "Error " << x.getMesg() << " setting up the output FIELD table." << LogIO::POST;
	}
	catch (...)
	{
		throw(AipsError("Unknown exception caught and released in fillFieldTable()"));
	}

	return true;
}

Bool MSTransformDataHandler::copyEphemerisTable(MSFieldColumns & msField)
{
	LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));
	const MSFieldColumns& fieldIn = mscIn_p->field();
	ScalarColumn<Int> eID(fieldIn.ephemerisId());

	if (eID.hasContent())
	{
		uInt nField;
		if (reindex_p)
		{
			nField = fieldid_p.nelements();
		}
		else
		{
			nField = eID.nrow();
		}

		String destPathName = Path(msOut_p.field().tableName()).absoluteName();
		ScalarColumn<String> name(fieldIn.name());

		for (uInt k = 0; k < nField; ++k)
		{
			uInt fieldId;
			if (reindex_p)
			{
				fieldId = fieldid_p[k];
			}
			else
			{
				fieldId = k;
			}

			Int theEphId = eID(fieldId);

			// There is an ephemeris attached to this field
			if (theEphId > -1)
			{
				Path ephPath = Path(fieldIn.ephemPath(fieldId));

				// Copy the ephemeris table over to the output FIELD table
				if (ephPath.length() > 0)
				{
					Directory origEphemDir(ephPath);
					origEphemDir.copy(destPathName + "/" + ephPath.baseName());

					os 	<< LogIO::NORMAL
							<< "Transferring ephemeris " << ephPath.baseName()
							<< " for output field " << name(fieldId)
							<< LogIO::POST;
				}
			}

			if (reindex_p)
			{
				msField.ephemerisId().put(k, theEphId);
			}
		}
	}

	return true;
}

// -----------------------------------------------------------------------
//  Modified version of fillDDTables
// -----------------------------------------------------------------------
Bool MSTransformDataHandler::fillDDTables()
{
	fillPolTable();
	fillDDITable();
	fillSPWTable();

	return true;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Bool MSTransformDataHandler::fillPolTable()
{
	LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));

	// Input polarization table
	const MSPolarization &poltable = mssel_p.polarization();
	ScalarColumn<Int> numCorr(poltable,MSPolarization::columnName(MSPolarization::NUM_CORR));
	ArrayColumn<Int> corrType(poltable,MSPolarization::columnName(MSPolarization::CORR_TYPE));
	ArrayColumn<Int> corrProd(poltable,MSPolarization::columnName(MSPolarization::CORR_PRODUCT));
	ScalarColumn<Bool> flagRow(poltable,MSPolarization::columnName(MSPolarization::FLAG_ROW));

	// Output polarization table
	MSPolarizationColumns& msPol(msc_p->polarization());

	// Fill output polarization table
	uInt nPol = poltable.nrow(); // nOutputPol = nInputPol (no PolId re-index)
	corrSlice_p.resize(nPol);
	for (uInt polId = 0; polId < nPol; polId++)
	{
		uInt ncorr = inPolOutCorrToInCorrMap_p[polId].nelements();
		const Vector<Int> inCT(corrType(polId));

		// Add row
		msOut_p.polarization().addRow();
		msPol.numCorr().put(polId, ncorr);
		msPol.flagRow().put(polId, flagRow(polId));

		// Setup correlation slices
		if (ncorr > 0 && ncorr < inCT.nelements())
		{
			keepShape_p = false;

			// Check whether the requested correlations can be accessed by slicing.
			// That means there must be a constant stride.  The most likely (only?)
			// way to violate that is to ask for 3 out of 4 correlations.
			if (ncorr > 2)
			{
				os 	<< LogIO::SEVERE
					<< "Sorry, the requested correlation selection is not unsupported.\n"
					<< "Try selecting fewer or all of the correlations."
					<< LogIO::POST;
				return false;
			}

			size_t increment = 2;
			if (ncorr > 1)
			{
				increment = inPolOutCorrToInCorrMap_p[polId][1] - inPolOutCorrToInCorrMap_p[polId][0];
			}
			corrSlice_p[polId] = Slice(inPolOutCorrToInCorrMap_p[polId][0],ncorr,increment);
		}
		else
		{
			corrSlice_p[polId] = Slice(0, ncorr);
		}

		// Apply slices to correlation type and product
		Vector<Int> outCT;
		const Matrix<Int> inCP(corrProd(polId));
		Matrix<Int> outCP;
		outCT.resize(ncorr);
		outCP.resize(2, ncorr);
		for (uInt k = 0; k < ncorr; ++k)
		{
			Int inCorrInd = inPolOutCorrToInCorrMap_p[polId][k];

			outCT[k] = inCT[inCorrInd];
			for (uInt feedind = 0; feedind < 2; ++feedind)
			{
				outCP(feedind, k) = inCP(feedind, inCorrInd);
			}

		}

		// Fill correlation type and product
		msPol.corrType().put(polId, outCT);
		msPol.corrProduct().put(polId, outCP);
	}

	return true;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Bool MSTransformDataHandler::fillDDITable()
{
	LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));

	// Input selected DDIs based on joint SPW-Pol selection
	uInt nddid = spw2ddid_p.size();

	// Input ddi table
	const MSDataDescription &inputDDI = mssel_p.dataDescription();

	ScalarColumn<Int> polIdCol(inputDDI, MSDataDescription::columnName(MSDataDescription::POLARIZATION_ID));
	ScalarColumn<Int> spwIdCol(inputDDI,MSDataDescription::columnName(MSDataDescription::SPECTRAL_WINDOW_ID));

	// Get selected SPWs
	Vector<Int> selectedSpwIds(nddid);
	for (uInt row=0; row<spw2ddid_p.size(); ++row)
	{
		selectedSpwIds[row] = spwIdCol(spw2ddid_p[row]);
	}

	// Get list of unique selected SPWs
	Bool option(false);
	Sort sortSpws(spw_p.getStorage(option), sizeof(Int));
	sortSpws.sortKey((uInt) 0, TpInt);
	Vector<uInt> spwsortindex, spwuniqinds;
	sortSpws.sort(spwsortindex, spw_p.nelements());
	uInt nuniqSpws = sortSpws.unique(spwuniqinds, spwsortindex);
	spw_uniq_p.resize(nuniqSpws);

	// Make map from input to output SPWs
	spwRelabel_p.resize(mscIn_p->spectralWindow().nrow());
	spwRelabel_p.set(-1);
	for (uInt k = 0; k < nuniqSpws; ++k)
	{
		spw_uniq_p[k] = spw_p[spwuniqinds[k]];
		spwRelabel_p[spw_uniq_p[k]] = k;
	}

	if (!reindex_p)
	{
		MSDataDescription &outputDDI = msOut_p.dataDescription();
		TableCopy::copyRows(outputDDI, inputDDI);
		return true;
	}

	// Output SPECTRAL_WINDOW_ID column
	Vector<Int> newSPWId(nddid);
	for (uInt ddi=0; ddi<nddid; ddi++)
	{
		newSPWId[ddi] = spwRelabel_p[spwIdCol(spw2ddid_p[ddi])];
	}

	// Output POLARIZATION_ID column
	Vector<Int> newPolId(nddid);
	for (uInt ddi=0; ddi<nddid; ddi++)
	{
		newPolId[ddi] = polIdCol(spw2ddid_p[ddi]);
	}

	// Fill output DDI table
	MSDataDescColumns& outputDDI(msc_p->dataDescription());
	for (uInt ddi=0; ddi<nddid; ddi++)
	{
		msOut_p.dataDescription().addRow();
		outputDDI.flagRow().put(ddi, false);
		outputDDI.polarizationId().put(ddi, newPolId[ddi]);
		outputDDI.spectralWindowId().put(ddi, newSPWId[ddi]);
	}


	return true;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Bool MSTransformDataHandler::fillSPWTable()
{
	LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));

	// Selected input MS SPW Table Columns (read-only)
	MSSpWindowColumns inSpWCols(mssel_p.spectralWindow());

	// SPW Table Columns of output MS (read-write)
	MSSpWindowColumns& msSpW(msc_p->spectralWindow());

	// Detect which optional columns of SPECTRAL_WINDOW are present.
	// inSpWCols and msSpW should agree because addOptionalColumns() was done for
	// SPECTRAL_WINDOW in fillAllTables() before making msc_p or calling fillDDTables
	Bool haveSpwAN = columnOk(inSpWCols.assocNature());
	Bool haveSpwASI = columnOk(inSpWCols.assocSpwId());
	Bool haveSpwBN = columnOk(inSpWCols.bbcNo());
	Bool haveSpwBS = columnOk(inSpWCols.bbcSideband());
	Bool haveSpwDI = columnOk(inSpWCols.dopplerId());
	Bool haveSpwSWF = mssel_p.spectralWindow().tableDesc().isColumn("SDM_WINDOW_FUNCTION") &&
                          mssel_p.spectralWindow().tableDesc().columnDescSet().isDefined("SDM_WINDOW_FUNCTION");
	Bool haveSpwSNB = mssel_p.spectralWindow().tableDesc().isColumn("SDM_NUM_BIN") &&
                          mssel_p.spectralWindow().tableDesc().columnDescSet().isDefined("SDM_NUM_BIN");
	Bool haveSpwCorrBit = mssel_p.spectralWindow().tableDesc().isColumn("SDM_CORR_BIT") &&
                              mssel_p.spectralWindow().tableDesc().columnDescSet().isDefined("SDM_CORR_BIT");

	uInt nuniqSpws = spw_uniq_p.size();

	// This sets the number of input channels for each spw. But
	// it considers that a SPW ID contains only one set of channels.
	// I hope this is true!!
	// Write to SPECTRAL_WINDOW table
	inNumChan_p.resize(spw_p.nelements());
	for (uInt k = 0; k < spw_p.nelements(); ++k)
	{
	        inNumChan_p[k] = inSpWCols.numChan()(spw_p[k]);
	}

	if (reindex_p)
	{
		msOut_p.spectralWindow().addRow(nuniqSpws);
	}
	else
	{
		const MSSpectralWindow &inputSPW = mssel_p.spectralWindow();
		MSSpectralWindow &outputSPW = msOut_p.spectralWindow();
		TableCopy::copyRows(outputSPW, inputSPW);
	}


	Vector<Vector<Int> > spwinds_of_uniq_spws(nuniqSpws);
	totnchan_p.resize(nuniqSpws);
	for (uInt k = 0; k < nuniqSpws; ++k)
	{
		Int maxchan = 0;
		uInt j = 0;

		totnchan_p[k] = 0;
		spwinds_of_uniq_spws[k].resize();
		for (uInt spwind = 0; spwind < spw_p.nelements(); ++spwind)
		{
			if (spw_p[spwind] == spw_uniq_p[k])
			{
				Int highchan = nchan_p[spwind] * chanStep_p[spwind] + chanStart_p[spwind];

				if (highchan > maxchan) maxchan = highchan;

				totnchan_p[k] += nchan_p[spwind];

				// The true is necessary to avoid scrambling previously assigned values.
				spwinds_of_uniq_spws[k].resize(j + 1, true);

				// Warning!  spwinds_of_uniq_spws[k][j] will compile without warning, but dump core at runtime.
				(spwinds_of_uniq_spws[k])[j] = spwind;

				++j;
			}
		}
		if (maxchan > inSpWCols.numChan()(spw_uniq_p[k]))
		{
			os 	<< LogIO::SEVERE
				<< " Channel settings wrong; exceeding number of channels in spw "
				<< spw_uniq_p[k] << LogIO::POST;
			return false;
		}
	}

	// min_k is an index for getting an spw index via spw_uniq_p[min_k].
	// k is an index for getting an spw index via spw_p[k].
	for (uInt min_k = 0; min_k < nuniqSpws; ++min_k)
	{
		uInt k = spwinds_of_uniq_spws[min_k][0];
		uInt outSPWId = reindex_p? min_k : spw_p[k];

		if (spwinds_of_uniq_spws[min_k].nelements() > 1 || nchan_p[k] != inSpWCols.numChan()(spw_p[k]))
		{
			Vector<Double> effBWIn = inSpWCols.effectiveBW()(spw_uniq_p[min_k]);
			Int nOutChan = totnchan_p[min_k];
			Vector<Double> chanFreqOut(nOutChan);
			Vector<Double> chanFreqIn = inSpWCols.chanFreq()(spw_uniq_p[min_k]);
			Vector<Double> chanWidthOut(nOutChan);
			Vector<Double> chanWidthIn = inSpWCols.chanWidth()(spw_uniq_p[min_k]);
			Vector<Double> spwResolOut(nOutChan);
			Vector<Double> spwResolIn = inSpWCols.resolution()(spw_uniq_p[min_k]);
			Vector<Double> effBWOut(nOutChan);
			Int outChan = 0;
			Int outChanNotDropped = 0;

			keepShape_p = false;

			// The sign of CHAN_WIDTH defaults to +.  Its determination assumes that
			// chanFreqIn is monotonic, but not that the sign of the chanWidthIn is correct.
			Bool neginc = chanFreqIn[chanFreqIn.nelements() - 1] < chanFreqIn[0];

			effBWOut.set(0.0);
			Double totalBW = 0.0;
			for (uInt rangeNum = 0; rangeNum < spwinds_of_uniq_spws[min_k].nelements(); ++rangeNum)
			{
				k = spwinds_of_uniq_spws[min_k][rangeNum];

				Int span = chanStep_p[k] * widths_p[k];

				for (Int j = 0; j < nchan_p[k]; ++j)
				{
					Int inpChan = chanStart_p[k] + j * span;

					if (span >= 1)
					{
						Int lastChan = inpChan + span - 1;

						if (lastChan > chanEnd_p[k])
						{
							// The averaging width is not a factor of the number of
							// selected input channels, so the last output bin receives
							// fewer input channels than the other bins.
							lastChan = chanEnd_p[k];

							Int nchan = lastChan - inpChan + 1;
							os 	<< LogIO::NORMAL
								<< "The last output channel of spw "
								<< spw_p[k] << " has only " << nchan
								<< " input channel";
							if (nchan > 1) os << "s.";
							os << LogIO::POST;

							// jagonzal (CAS-8618): We may have more than one channel dropped per SPW
							// due to multiple channel selections
							spwDropChannelMap_p[spw_p[k]].push_back(outChan);
						}
						else
						{
							for (Int inputChan = inpChan;inputChan<=lastChan;inputChan++)
							{
								spwSelectedChannelMap_p[spw_p[k]][outChanNotDropped].push_back(inputChan);
							}
							outChanNotDropped++;
						}

						chanFreqOut[outChan] = (chanFreqIn[inpChan]
								+ chanFreqIn[lastChan]) / 2;

						Double sep = chanFreqIn[lastChan] - chanFreqIn[inpChan];

						if (neginc) sep = -sep;

						// The internal abs is necessary because the sign of chanWidthIn may be wrong.
						chanWidthOut[outChan] = sep + 0.5 * abs(chanWidthIn[inpChan] + chanWidthIn[lastChan]);
						if (neginc) chanWidthOut[outChan] = -chanWidthOut[outChan];

						spwResolOut[outChan] = 0.5 * (spwResolIn[inpChan] + spwResolIn[lastChan]) + sep;

						for (Int avgChan = inpChan; avgChan <= lastChan; avgChan += chanStep_p[k])
						{
							effBWOut[outChan] += effBWIn[avgChan];
						}

					}
					else
					{
						chanFreqOut[outChan] = chanFreqIn[inpChan];
						spwResolOut[outChan] = spwResolIn[inpChan];
						chanWidthOut[outChan] = chanWidthIn[inpChan];
						effBWOut[outChan] = effBWIn[inpChan];
					}
					// Use CHAN_WIDTH instead of EFFECTIVE_BW
					totalBW += abs(chanWidthOut[outChan]);
					++outChan;
				}
			}
			--outChan;

			msSpW.chanFreq().put(outSPWId, chanFreqOut);
			msSpW.refFrequency().put(outSPWId,min(chanFreqOut[0], chanFreqOut[chanFreqOut.size() - 1]));
			msSpW.resolution().put(outSPWId, spwResolOut);
			msSpW.numChan().put(outSPWId, nOutChan);
			msSpW.chanWidth().put(outSPWId, chanWidthOut);
			msSpW.effectiveBW().put(outSPWId, spwResolOut);
			msSpW.totalBandwidth().put(outSPWId, totalBW);
		}
		else
		{
			msSpW.chanFreq().put(outSPWId, inSpWCols.chanFreq()(spw_p[k]));
			msSpW.refFrequency().put(outSPWId, inSpWCols.refFrequency()(spw_p[k]));
			msSpW.resolution().put(outSPWId, inSpWCols.resolution()(spw_p[k]));
			msSpW.numChan().put(outSPWId, inSpWCols.numChan()(spw_p[k]));
			msSpW.chanWidth().put(outSPWId, inSpWCols.chanWidth()(spw_p[k]));
			msSpW.effectiveBW().put(outSPWId, inSpWCols.effectiveBW()(spw_p[k]));
			msSpW.totalBandwidth().put(outSPWId,inSpWCols.totalBandwidth()(spw_p[k]));
		}

		msSpW.flagRow().put(outSPWId, inSpWCols.flagRow()(spw_p[k]));
		msSpW.freqGroup().put(outSPWId, inSpWCols.freqGroup()(spw_p[k]));
		msSpW.freqGroupName().put(outSPWId, inSpWCols.freqGroupName()(spw_p[k]));
		msSpW.ifConvChain().put(outSPWId, inSpWCols.ifConvChain()(spw_p[k]));
		msSpW.measFreqRef().put(outSPWId, inSpWCols.measFreqRef()(spw_p[k]));
		msSpW.name().put(outSPWId, inSpWCols.name()(spw_p[k]));
		msSpW.netSideband().put(outSPWId, inSpWCols.netSideband()(spw_p[k]));


		if (haveSpwBN) msSpW.bbcNo().put(outSPWId, inSpWCols.bbcNo()(spw_p[k]));
		if (haveSpwBS) msSpW.bbcSideband().put(outSPWId, inSpWCols.bbcSideband()(spw_p[k]));
        if (haveSpwDI) msSpW.dopplerId().put(outSPWId, inSpWCols.dopplerId()(spw_p[k]));
        if (haveSpwSWF) 
        {
            ScalarColumn<String> inSwfCol(mssel_p.spectralWindow(), "SDM_WINDOW_FUNCTION");
            ScalarColumn<String> outSwfCol(msOut_p.spectralWindow(), "SDM_WINDOW_FUNCTION");
            outSwfCol.put(outSPWId, inSwfCol(spw_p[k]));
        }
        if (haveSpwSNB) 
        {
            ScalarColumn<Int> inSnbCol(mssel_p.spectralWindow(), "SDM_NUM_BIN");
            ScalarColumn<Int> outSnbCol(msOut_p.spectralWindow(), "SDM_NUM_BIN");
            outSnbCol.put(outSPWId, inSnbCol(spw_p[k]));
        }
        if (haveSpwCorrBit) 
        {
            ScalarColumn<String> inCorrBitCol(mssel_p.spectralWindow(), "SDM_CORR_BIT");
            ScalarColumn<String> outCorrBitCol(msOut_p.spectralWindow(), "SDM_CORR_BIT");
            outCorrBitCol.put(outSPWId, inCorrBitCol(spw_p[k]));
        }

		if (haveSpwASI)
		{
			if (reindex_p)
			{
				// Get list of SPWs associated to his one
				std::vector<Int> selectedSPWs = spw_p.tovector();

				// Get the list of selected SPWs and association nature
				Array<Int> assocSpwId = inSpWCols.assocSpwId()(spw_p[k]);
				Array<String> assocNature = inSpWCols.assocNature()(spw_p[k]);

				// Find which associated SPWs are selected, and store the transformed Id
				std::vector<Int>::iterator findIt;
				std::vector<Int> selectedAssocSpwId;
				std::vector<String> selectedAssocNature;
				for (uInt idx=0;idx<assocSpwId.size();idx++)
				{
					IPosition pos(1,idx);
					Int spw = assocSpwId(pos);
					findIt = find (selectedSPWs.begin(), selectedSPWs.end(), spw);
					if (findIt != selectedSPWs.end())
					{
						selectedAssocSpwId.push_back(spwRelabel_p[spw]);
						if (haveSpwAN) selectedAssocNature.push_back(assocNature(pos));
					}
				}

				// Store selected associated SPW Ids
				Vector<Int> selectedAssocSpwIdVector(selectedAssocSpwId);
				msSpW.assocSpwId().put(min_k, selectedAssocSpwIdVector);

				// Store selected association nature
				if (haveSpwAN)
				{
					Vector<String> selectedAssocNatureVector(selectedAssocNature);
					msSpW.assocNature().put(min_k, selectedAssocNatureVector);
				}
			}
			else
			{
				msSpW.assocNature().put(outSPWId, inSpWCols.assocNature()(outSPWId));
			}
		}
	}


	return true;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
uInt MSTransformDataHandler::addOptionalColumns(const Table& inTab, Table& outTab,const Bool beLazy)
{
	uInt nAdded = 0;
	const TableDesc& inTD = inTab.actualTableDesc();

	// Only rely on the # of columns if you are sure that inTab and outTab
	// can't have the same # of columns without having _different_ columns,
	// i.e. use beLazy if outTab.actualTableDesc() is in its default state.
	uInt nInCol = inTD.ncolumn();
	if (!beLazy || nInCol > outTab.actualTableDesc().ncolumn())
	{
		LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));

		Vector<String> oldColNames = inTD.columnNames();

		for (uInt k = 0; k < nInCol; ++k)
		{
			if (!outTab.actualTableDesc().isColumn(oldColNames[k]))
			{
				try
				{
					outTab.addColumn(inTD.columnDesc(k), false);
					++nAdded;
				}
				// NOT AipsError x
				catch (...)
				{
					os 	<< LogIO::WARN << "Could not add column "
						<< oldColNames[k] << " to " << outTab.tableName()
						<< LogIO::POST;
				}
			}
		}
	}

	return nAdded;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void MSTransformDataHandler::relabelSources()
{
	LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));

	//Source is an optional table, so it may not exist
	if (Table::isReadable(mssel_p.sourceTableName()))
	{
		// Note that mscIn_p->field().sourceId() has ALL of the
		// sourceIDs in the input MS, not just the selected ones.
		const Vector<Int>& inSrcIDs = mscIn_p->field().sourceId().getColumn();

		Int highestInpSrc = max(inSrcIDs);

		// Ensure space for -1.
		if (highestInpSrc < 0) highestInpSrc = 0;

		sourceRelabel_p.resize(highestInpSrc + 1);
		// Default to "any".
		sourceRelabel_p.set(-1);

		// Enable sourceIDs that are actually referred
		// by selected fields, and remap them using j.
		uInt j = 0;
		for (uInt k = 0; k < fieldid_p.nelements(); ++k)
		{
			Int fldInSrcID = inSrcIDs[fieldid_p[k]];

			if (fldInSrcID > -1)
			{
				// Multiple fields can use the same
				if (sourceRelabel_p[fldInSrcID] == -1)
				{
					// source in a mosaic.
					sourceRelabel_p[fldInSrcID] = j;
					++j;
				}
			}
		}
	}
	else
	{
		// Default to "any".
		os 	<< LogIO::NORMAL
			<< "The input MS does not have the optional SOURCE table.\n"
			<< "-1 will be used as a generic source ID." << LogIO::POST;
		sourceRelabel_p.resize(1);
		sourceRelabel_p.set(-1);
	}

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void MSTransformDataHandler::copySubtable(const String& tabName, const Table& inTab,const Bool doFilter)
{
	String outName(msOut_p.tableName() + '/' + tabName);

	if (PlainTable::tableCache()(outName)) PlainTable::tableCache().remove(outName);
	inTab.deepCopy(outName, Table::New, false, Table::AipsrcEndian, doFilter);
	Table outTab(outName, Table::Update);
	msOut_p.rwKeywordSet().defineTable(tabName, outTab);
	msOut_p.initRefs();

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void MSTransformDataHandler::make_map(std::map<Int, Int>& mapper, const Vector<Int>& inv)
{
  std::set<Int> valSet;

	// Strange, but slightly more efficient than going forward.
	for (Int i = inv.nelements(); i--;)
	{
		valSet.insert(inv[i]);
	}

	uInt remaval = 0;
	for (std::set<Int>::const_iterator vs_iter = valSet.begin(); vs_iter != valSet.end(); ++vs_iter)
	{
		mapper[*vs_iter] = remaval;
		++remaval;
	}

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Bool MSTransformDataHandler::copyPointing()
{
	LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));

	//Pointing is allowed to not exist
	if (Table::isReadable(mssel_p.pointingTableName()))
	{
		const MSPointing& oldPoint = mssel_p.pointing();

		if ((!antennaSel_p && timeRange_p == "") or !reindex_p)
		{
			// jagonzal: copySubtables works with POINTING because it
			// is not created by default in the method createSubtables
			copySubtable(MS::keywordName(MS::POINTING), oldPoint);
		}
		else
		{
			setupNewPointing();

			if (oldPoint.nrow() > 0)
			{
				// Could be declared as Table&
				MSPointing& newPoint = msOut_p.pointing();

				// Add optional columns if present in oldPoint.
				uInt nAddedCols = addOptionalColumns(oldPoint, newPoint, true);
				os << LogIO::DEBUG1 << "POINTING has " << nAddedCols << " optional columns." << LogIO::POST;

				const MSPointingColumns oldPCs(oldPoint);
				MSPointingColumns newPCs(newPoint);
				newPCs.setEpochRef(MEpoch::castType(oldPCs.timeMeas().getMeasRef().getType()));
				newPCs.setDirectionRef(MDirection::castType(oldPCs.directionMeasCol().getMeasRef().getType()));
				newPCs.setEncoderDirectionRef(MDirection::castType(oldPCs.encoderMeas().getMeasRef().getType()));

				const ScalarColumn<Int>& antIds = oldPCs.antennaId();
				const ScalarColumn<Double>& time = oldPCs.time();
				ScalarColumn<Int>& outants = newPCs.antennaId();

				uInt nTRanges = selTimeRanges_p.ncolumn();

				uInt outRow = 0;

				// Int for comparison
				Int maxSelAntp1 = antNewIndex_p.nelements();
				// with newAntInd.
				for (uInt inRow = 0; inRow < antIds.nrow(); ++inRow)
				{
					Int newAntInd = antIds(inRow);
					if (antennaSel_p)
					{
						newAntInd = newAntInd < maxSelAntp1 ? antNewIndex_p[newAntInd] : -1;
					}

					Double t = time(inRow);

					if (newAntInd > -1)
					{
						Bool matchT = false;
						if (nTRanges == 0)
						{
							matchT = true;
						}
						else
						{
							for (uInt tr = 0; tr < nTRanges; ++tr)
							{
								if (t >= selTimeRanges_p(0, tr) && t <= selTimeRanges_p(1, tr))
								{
									matchT = true;
									break;
								}
							}
						}

						if (matchT)
						{
							TableCopy::copyRows(newPoint, oldPoint, outRow,inRow, 1, false);
							outants.put(outRow, newAntInd);
							++outRow;
						}
					}
				}
			}
		}
	}
	else
	{
		// Make an empty stub for MSColumns.
		setupNewPointing();
	}


	return true;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void MSTransformDataHandler::setupNewPointing()
{
	SetupNewTable pointingSetup(msOut_p.pointingTableName(),
	MSPointing::requiredTableDesc(), Table::New);

	// POINTING can be large, set some sensible defaults for storageMgrs
	IncrementalStMan ismPointing("ISMPointing");
	StandardStMan ssmPointing("SSMPointing", 32768);
	pointingSetup.bindAll(ismPointing, true);
	pointingSetup.bindColumn(MSPointing::columnName(MSPointing::DIRECTION),ssmPointing);
	pointingSetup.bindColumn(MSPointing::columnName(MSPointing::TARGET),ssmPointing);
	pointingSetup.bindColumn(MSPointing::columnName(MSPointing::TIME),ssmPointing);
	msOut_p.rwKeywordSet().defineTable(MS::keywordName(MS::POINTING),Table(pointingSetup));
	msOut_p.initRefs();

	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Bool MSTransformDataHandler::copySource()
{
	//Source is an optional table, so it may not exist
	if (Table::isReadable(mssel_p.sourceTableName()))
	{
		LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));

		const MSSource& oldSource = mssel_p.source();
		MSSource& newSource = msOut_p.source();

		// Add optional columns if present in oldSource.
		uInt nAddedCols = addOptionalColumns(oldSource, newSource, true);
		os << LogIO::DEBUG1 << "SOURCE has " << nAddedCols << " optional columns." << LogIO::POST;

		const MSSourceColumns incols(oldSource);
		MSSourceColumns outcols(newSource);

		// Copy the Measures frame info.  This has to be done before filling the rows.
		outcols.setEpochRef(MEpoch::castType(incols.timeMeas().getMeasRef().getType()));
		outcols.setDirectionRef(MDirection::castType(incols.directionMeas().getMeasRef().getType()));
		outcols.setPositionRef(MPosition::castType(incols.positionMeas().getMeasRef().getType()));
		outcols.setFrequencyRef(MFrequency::castType(incols.restFrequencyMeas().getMeasRef().getType()));
		outcols.setRadialVelocityRef(MRadialVelocity::castType(incols.sysvelMeas().getMeasRef().getType()));

		if (!reindex_p)
		{
			TableCopy::copyRows(newSource, oldSource);
			return true;
		}

		const ScalarColumn<Int>& inSId = incols.sourceId();
		ScalarColumn<Int>& outSId = outcols.sourceId();
		const ScalarColumn<Int>& inSPW = incols.spectralWindowId();
		ScalarColumn<Int>& outSPW = outcols.spectralWindowId();

		// row number in output.
		uInt outrn = 0;
		uInt nInputRows = inSId.nrow();
		// inSidVal is Int.
		Int maxSId = sourceRelabel_p.nelements();
		Int maxSPWId = spwRelabel_p.nelements();
		for (uInt inrn = 0; inrn < nInputRows; ++inrn)
		{
			Int inSidVal = inSId(inrn);
			// -1 means the source is valid for any SPW.
			Int inSPWVal = inSPW(inrn);
			if (inSidVal >= maxSId)
			{
				os << LogIO::WARN << "Invalid SOURCE ID in SOURCE table row " << inrn << LogIO::POST;
			}
			if (inSPWVal >= maxSPWId)
			{
				os << LogIO::WARN << "Invalid SPW ID in SOURCE table row " << inrn << LogIO::POST;
			}

			if (	(inSidVal > -1) &&
					(inSidVal < maxSId) &&
					(sourceRelabel_p[inSidVal] > -1) &&
					((inSPWVal == -1) || (inSPWVal < maxSPWId && spwRelabel_p[inSPWVal] > -1)))
			{
				// Copy inrn to outrn.
				TableCopy::copyRows(newSource, oldSource, outrn, inrn, 1);
				outSId.put(outrn, sourceRelabel_p[inSidVal]);
				outSPW.put(outrn, inSPWVal > -1 ? spwRelabel_p[inSPWVal] : -1);
				++outrn;
			}
		}

	}

	return true;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Bool MSTransformDataHandler::copyAntenna()
{
	const MSAntenna& oldAnt = mssel_p.antenna();
	MSAntenna& newAnt = msOut_p.antenna();
	const MSAntennaColumns incols(oldAnt);
	MSAntennaColumns outcols(newAnt);
	Bool retval = false;

	outcols.setOffsetRef(MPosition::castType(incols.offsetMeas().getMeasRef().getType()));
	outcols.setPositionRef(MPosition::castType(incols.positionMeas().getMeasRef().getType()));

    //TableCopy::copyRows(newAnt, oldAnt);
    //retval = true;

	if (!antennaSel_p or !reindex_p)
	{
		TableCopy::copyRows(newAnt, oldAnt);
		retval = true;
	}
	else
	{
		uInt nAnt = antNewIndex_p.nelements();
		// Don't use min() here, it's too overloaded.
		if (nAnt > oldAnt.nrow()) nAnt = oldAnt.nrow();

		for (uInt k = 0; k < nAnt; ++k)
		{
			if (antNewIndex_p[k] > -1) TableCopy::copyRows(newAnt, oldAnt, antNewIndex_p[k], k, 1, false);
		}
		retval = true;
	}

	return retval;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Bool MSTransformDataHandler::copyFeed()
{
	LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));

	const MSFeed& oldFeed = mssel_p.feed();
	MSFeed& newFeed = msOut_p.feed();
	const MSFeedColumns incols(oldFeed);
	MSFeedColumns outcols(newFeed);

	outcols.setDirectionRef(MDirection::castType(incols.beamOffsetMeas().getMeasRef().getType()));
	outcols.setEpochRef(MEpoch::castType(incols.timeMeas().getMeasRef().getType()));
	outcols.setPositionRef(MPosition::castType(incols.positionMeas().getMeasRef().getType()));

	if ((!antennaSel_p && allEQ(spwRelabel_p, spw_p)) or !reindex_p)
	{
		TableCopy::copyRows(newFeed, oldFeed);
	}
	else
	{
		const Vector<Int>& antIds = incols.antennaId().getColumn();
		const Vector<Int>& spwIds = incols.spectralWindowId().getColumn();

		// Copy selected rows.
		uInt totNFeeds = antIds.nelements();
		uInt totalSelFeeds = 0;
		Int maxSelAntp1 = antNewIndex_p.nelements();
		for (uInt k = 0; k < totNFeeds; ++k)
		{
			// antenna must be selected, and spwId must be -1 (any) or selected.
			if (	antIds[k] < maxSelAntp1 &&
					antNewIndex_p[antIds[k]] > -1 &&
					(spwIds[k] < 0 || spwRelabel_p[spwIds[k]] > -1)
				)
			{
				TableCopy::copyRows(newFeed, oldFeed, totalSelFeeds, k, 1,false);
				++totalSelFeeds;
			}
		}

		// Remap antenna and spw #s.
		ScalarColumn<Int>& antCol = outcols.antennaId();
		ScalarColumn<Int>& spwCol = outcols.spectralWindowId();

		for (uInt k = 0; k < totalSelFeeds; ++k)
		{

			antCol.put(k, antNewIndex_p[antCol(k)]);
			if (spwCol(k) > -1) spwCol.put(k, spwRelabel_p[spwCol(k)]);
		}
	}

	// Check if selected spw is WVR data. WVR data does not have FEED data
	// so it is not a failure if newFeed.nrow == 0
	if (newFeed.nrow() < 1 and spw_p.size() == 1){
	    // Using the MSMetaData class
	    MSMetaData msmeta(&mssel_p, 0);
	    std::set<uInt> wvrspw = msmeta.getWVRSpw();
	    for (std::set<uInt>::iterator bbit = wvrspw.begin(); bbit != wvrspw.end(); ++bbit){
	        if ((uInt)spw_p[0] == *bbit){
	            os << LogIO::DEBUG1 << "Skipping spw="<<*bbit<<" because it is WVR and has no FEED content" << LogIO::POST;
	            return true;
	        }
	    }
	}

	if (newFeed.nrow() < 1)
	{
//		LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));
		os << LogIO::SEVERE << "No feeds were selected." << LogIO::POST;
		return false;
	}

	return true;
}

// -----------------------------------------------------------------------
//  Get the processorId corresponding to a given DDI
// -----------------------------------------------------------------------
Int MSTransformDataHandler::getProcessorId(Int dataDescriptionId, String msname)
{
    ostringstream taql;
    taql << "SELECT PROCESSOR_ID from " << msname;
    taql << " WHERE DATA_DESC_ID ==" << dataDescriptionId;
    taql << " LIMIT 1";

    casacore::TableProxy *firstSelectedRow = new TableProxy(tableCommand(taql.str()));
    Record colWrapper = firstSelectedRow->getVarColumn(String("PROCESSOR_ID"),0,1,1);
    casacore::Vector<Int> processorId = colWrapper.asArrayInt("r1");

    delete firstSelectedRow;
    return processorId[0];
}


// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Bool MSTransformDataHandler::copyFlag_Cmd()
{

	// Like POINTING, FLAG_CMD is supposed to exist but is allowed not to.
	if (Table::isReadable(mssel_p.flagCmdTableName()))
	{
		const MSFlagCmd& oldFlag_Cmd = mssel_p.flagCmd();

		if (oldFlag_Cmd.nrow() > 0)
		{

			// Could be declared as Table&
			MSFlagCmd& newFlag_Cmd = msOut_p.flagCmd();

			LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));

			// Add optional columns if present in oldFlag_Cmd.
			uInt nAddedCols = addOptionalColumns(oldFlag_Cmd, newFlag_Cmd, true);
			os << LogIO::DEBUG1 << "FLAG_CMD has " << nAddedCols << " optional columns." << LogIO::POST;

			const MSFlagCmdColumns oldFCs(oldFlag_Cmd);
			MSFlagCmdColumns newFCs(newFlag_Cmd);
			newFCs.setEpochRef(MEpoch::castType(oldFCs.timeMeas().getMeasRef().getType()));

			TableCopy::copyRows(newFlag_Cmd, oldFlag_Cmd);

		}
	}

	return true;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Bool MSTransformDataHandler::copyHistory()
{
	LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));

	const MSHistory& oldHistory = mssel_p.history();

	// Could be declared as Table&
	MSHistory& newHistory = msOut_p.history();

	// Add optional columns if present in oldHistory.
	uInt nAddedCols = addOptionalColumns(oldHistory, newHistory, true);
	os << LogIO::DEBUG1 << "HISTORY has " << nAddedCols << " optional columns." << LogIO::POST;

	const MSHistoryColumns oldHCs(oldHistory);
	MSHistoryColumns newHCs(newHistory);
	newHCs.setEpochRef(MEpoch::castType(oldHCs.timeMeas().getMeasRef().getType()));

	TableCopy::copyRows(newHistory, oldHistory);

	return true;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Bool MSTransformDataHandler::copyObservation()
{
	const MSObservation& oldObs = mssel_p.observation();
	MSObservation& newObs = msOut_p.observation();
	const MSObservationColumns oldObsCols(oldObs);
	MSObservationColumns newObsCols(newObs);
	newObsCols.setEpochRef(MEpoch::castType(oldObsCols.releaseDateMeas().getMeasRef().getType()));

	uInt nObs = selObsId_p.nelements();
	if (nObs > 0 and reindex_p)
	{
		for (uInt outrn = 0; outrn < nObs; ++outrn)
		{
			TableCopy::copyRows(newObs, oldObs, outrn, selObsId_p[outrn], 1);
		}

	}
	else
	{
		TableCopy::copyRows(newObs, oldObs);
	}

	return true;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Bool MSTransformDataHandler::copyProcessor()
{
	const MSProcessor& oldProc = mssel_p.processor();
	MSProcessor& newProc = msOut_p.processor();
	TableCopy::copyRows(newProc, oldProc);

	return true;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Bool MSTransformDataHandler::copyState()
{
	// STATE is allowed to not exist, even though it is not optional in
	// the MS def'n.  For one thing, split dropped it for quite a while.
	if (Table::isReadable(mssel_p.stateTableName()))
	{
		LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));
		const MSState& oldState = mssel_p.state();

		if (oldState.nrow() > 0)
		{
			if (!intentString_p.empty() and reindex_p)
			{
				MSState& newState = msOut_p.state();
				const MSStateColumns oldStateCols(oldState);
				MSStateColumns newStateCols(newState);

				// Initialize stateRemapper_p if necessary.
				// if (stateRemapper_p.size() < 1) make_map(stateRemapper_p, mscIn_p->stateId().getColumn());

				// jagonzal (CAS-6351): Do not apply implicit re-indexing //////////////////////////////////////////////////
				//
				// Get list of selected scan intent indexes
				MSSelection mssel;
				mssel.setStateExpr(intentString_p);
				Vector<Int> scanIntentList = mssel.getStateObsModeList(getInputMS());
				//
				// Populate state re-mapper using all selected indexes (not only the implicit ones)
				stateRemapper_p.clear();
				for (uInt index=0; index < scanIntentList.size(); index++)
				{
					stateRemapper_p[scanIntentList(index)] = index;
				}
				///////////////////////////////////////////////////////////////////////////////////////////////////////////

				uInt nStates = stateRemapper_p.size();

				// stateRemapper_p goes from input to output, as is wanted in most
				// places.  Here we need a map going the other way, so make one.
				Vector<Int> outStateToInState(nStates);
				std::map<Int, Int>::iterator mit;

				for (mit = stateRemapper_p.begin(); mit != stateRemapper_p.end(); ++mit)
				{
					outStateToInState[(*mit).second] = (*mit).first;
				}


				for (uInt outrn = 0; outrn < nStates; ++outrn)
				{
					TableCopy::copyRows(newState, oldState, outrn,outStateToInState[outrn], 1);
				}
			}
			// jagonzal (CAS-6351): Do not apply implicit re-indexing
			// Therefore just copy the input state sub-table to the output state sub-table
			else
			{
				MSState& newState = msOut_p.state();
				TableCopy::copyRows(newState, oldState);
			}

		}
	}
	return true;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Bool MSTransformDataHandler::copySyscal()
{
	// SYSCAL is allowed to not exist.
	if (Table::isReadable(mssel_p.sysCalTableName()))
	{
		const MSSysCal& oldSysc = mssel_p.sysCal();

		if (oldSysc.nrow() > 0)
		{
			// Add a SYSCAL subtable to msOut_p with 0 rows for now.
			Table::TableOption option = Table::New;
			TableDesc sysCalTD = MSSysCal::requiredTableDesc();
			SetupNewTable sysCalSetup(msOut_p.sysCalTableName(), sysCalTD,option);
			msOut_p.rwKeywordSet().defineTable(MS::keywordName(MS::SYSCAL),Table(sysCalSetup, 0));

			// update the references to the subtable keywords
			msOut_p.initRefs();

			// Could be declared as Table&.
			MSSysCal& newSysc = msOut_p.sysCal();

			LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));

			uInt nAddedCols = addOptionalColumns(oldSysc, newSysc, true);
			os << LogIO::DEBUG1 << "SYSCAL has " << nAddedCols << " optional columns." << LogIO::POST;

			const MSSysCalColumns incols(oldSysc);
			MSSysCalColumns outcols(newSysc);
			outcols.setEpochRef(MEpoch::castType(incols.timeMeas().getMeasRef().getType()));

			if ((!antennaSel_p && allEQ(spwRelabel_p, spw_p)) or !reindex_p)
			{
				TableCopy::copyRows(newSysc, oldSysc);
			}
			else
			{
				const Vector<Int>& antIds = incols.antennaId().getColumn();
				const Vector<Int>& spwIds = incols.spectralWindowId().getColumn();

				// Copy selected rows.
				uInt totNSyscals = antIds.nelements();
				uInt totalSelSyscals = 0;
				Int maxSelAntp1 = antNewIndex_p.nelements(); // Int for comparison with antIds.
				for (uInt k = 0; k < totNSyscals; ++k)
				{
					// antenna must be selected, and spwId must be -1 (any) or selected.
					if (	antIds[k] < maxSelAntp1 &&
							antNewIndex_p[antIds[k]] > -1 &&
							(spwIds[k] < 0 || spwRelabel_p[spwIds[k]] > -1)
						)
					{
						TableCopy::copyRows(newSysc, oldSysc, totalSelSyscals,k, 1, false);
						++totalSelSyscals;
					}
				}

				// Remap antenna and spw #s.
				ScalarColumn<Int>& antCol = outcols.antennaId();
				ScalarColumn<Int>& spwCol = outcols.spectralWindowId();

				for (uInt k = 0; k < totalSelSyscals; ++k)
				{
					antCol.put(k, antNewIndex_p[antCol(k)]);
					if (spwCol(k) > -1) spwCol.put(k, spwRelabel_p[spwCol(k)]);
				}
			}
		}
	}

	return true;
}

Bool MSTransformDataHandler::copyWeather()
{
	// Weather is allowed to not exist.
	if (Table::isReadable(mssel_p.weatherTableName()))
	{
		const MSWeather& oldWeath = mssel_p.weather();

		if (oldWeath.nrow() > 0)
		{
			// Add a WEATHER subtable to msOut_p with 0 rows for now.
			Table::TableOption option = Table::New;
			TableDesc weatherTD = MSWeather::requiredTableDesc();
			SetupNewTable weatherSetup(msOut_p.weatherTableName(), weatherTD,option);
			msOut_p.rwKeywordSet().defineTable(MS::keywordName(MS::WEATHER),Table(weatherSetup, 0));

			// update the references to the subtable keywords
			msOut_p.initRefs();

			MSWeather& newWeath = msOut_p.weather(); // Could be declared as Table&

			LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));

			uInt nAddedCols = addOptionalColumns(oldWeath, newWeath, true);
			os << LogIO::DEBUG1 << "WEATHER has " << nAddedCols << " optional columns." << LogIO::POST;

			const MSWeatherColumns oldWCs(oldWeath);
			MSWeatherColumns newWCs(newWeath);
			newWCs.setEpochRef(MEpoch::castType(oldWCs.timeMeas().getMeasRef().getType()));

			if (!antennaSel_p or !reindex_p)
			{
				TableCopy::copyRows(newWeath, oldWeath);
			}
			else
			{
				const Vector<Int>& antIds(oldWCs.antennaId().getColumn());
				ScalarColumn<Int>& outants = newWCs.antennaId();

				uInt selRow = 0;
				Int maxSelAntp1 = antNewIndex_p.nelements();

				for (uInt k = 0; k < antIds.nelements(); ++k)
				{
					// Weather station is on antenna?
					if (antIds[k] > -1)
					{
						// remap ant num
						if (antIds[k] < maxSelAntp1)
						{
							Int newAntInd = antNewIndex_p[antIds[k]];

							// Ant selected?
							if (newAntInd > -1)
							{
								TableCopy::copyRows(newWeath, oldWeath, selRow,k, 1);
								outants.put(selRow, newAntInd);
								++selRow;
							}
						}
					}
					else
					{
						// means valid for all antennas.
						TableCopy::copyRows(newWeath, oldWeath, selRow, k, 1);
						outants.put(selRow, -1);
						++selRow;
					}
				}
			}
		}
	}

	return true;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Bool MSTransformDataHandler::filterOptSubtable(const String& subtabname)
{
	LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));

	// subtabname is allowed to not exist.  Use ms_p instead of mssel_p, because
	// the latter has a random name which does NOT necessarily lead to
	// subtabname.  (And if selection took care of subtables, we probably
	// wouldn't have to do it here.)
	if (Table::isReadable(ms_p.tableName() + '/' + subtabname))
	{
		const Table intab(ms_p.tableName() + '/' + subtabname);

		if (intab.nrow() > 0) {

			// Add feed if selecting by it is ever added.
			Bool doFilter = (antennaSel_p || !allEQ(spwRelabel_p, spw_p)) && reindex_p;

			copySubtable(subtabname, intab, doFilter);

			if (doFilter) {

				// At this point msOut_p has subtab with 0 rows.
				Table outtab(msOut_p.tableName() + '/' + subtabname,Table::Update);
				ScalarColumn<Int> inAntIdCol(intab, "ANTENNA_ID");
				ScalarColumn<Int> inSpwIdCol(intab, "SPECTRAL_WINDOW_ID");
				const Vector<Int>& antIds = inAntIdCol.getColumn();
				const Vector<Int>& spwIds = inSpwIdCol.getColumn();

				// Copy selected rows.
				uInt totNOuttabs = antIds.nelements();
				uInt totalSelOuttabs = 0;

				// Int for comparison with antIds.
				Int maxSelAntp1 = antNewIndex_p.nelements();

				Bool haveRemappingProblem = false;
				for (uInt inrow = 0; inrow < totNOuttabs; ++inrow)
				{
					// antenna must be selected, and spwId must be -1 (any) or selected.
					// Extra care must be taken because for a while MSes had CALDEVICE
					// and SYSPOWER, but the ANTENNA_ID and SPECTRAL_WINDOW_ID of those
					// sub-tables were not being re-mapped by split.
					if (antIds[inrow] < maxSelAntp1 && antNewIndex_p[antIds[inrow]] > -1)
					{

						if (spwIds[inrow] < 0 ||
								(spwIds[inrow] < static_cast<Int> (spwRelabel_p.nelements()) &&
										spwRelabel_p[spwIds[inrow]] > -1))
						{
							TableCopy::copyRows(outtab, intab, totalSelOuttabs,inrow, 1, false);
							++totalSelOuttabs;
						}

						// Ideally we'd like to catch antenna errors too.  They are
						// avoided, but because of the way antNewIndex_p is made,
						// antIds[inrow] >= maxSelAntp1
						// is not necessarily an error.  It's not even possible to catch
						// all the spw errors, so we settle for catching the ones we can
						// and reliably avoiding segfaults.
						else if (spwIds[inrow] >= static_cast<Int> (spwRelabel_p.nelements()))
						{
							haveRemappingProblem = true;
						}

					}
				}

				if (haveRemappingProblem)
				{
					os 	<< LogIO::WARN << "At least one row of "
						<< intab.tableName()
						<< " has an antenna or spw mismatch.\n"
						<< "(Presumably from an older split, sorry.)\n"
						<< "If " << subtabname
						<< " is important, it should be fixed with tb or browsetable,\n"
						<< "or by redoing the split that made "
						<< ms_p.tableName() << " (check its history)."
						<< LogIO::POST;
				}


				// Remap antenna and spw #s.
				ScalarColumn<Int> outAntCol(outtab, "ANTENNA_ID");
				ScalarColumn<Int> outSpwCol(outtab, "SPECTRAL_WINDOW_ID");

				for (uInt k = 0; k < totalSelOuttabs; ++k)
				{
					outAntCol.put(k, antNewIndex_p[outAntCol(k)]);
					if (outSpwCol(k) > -1) outSpwCol.put(k, spwRelabel_p[outSpwCol(k)]);
				}
			}
		}
	}

	return true;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
Bool MSTransformDataHandler::copyGenericSubtables()
{
	LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));

	// Already handled subtables will be removed from this,
	// so a modifiable copy is needed.
	TableRecord inkws(mssel_p.keywordSet());

	// Some of the standard subtables need special handling,
	// e.g. DATA_DESCRIPTION, SPECTRAL_WINDOW, and ANTENNA, so they are already
	// defined in msOut_p.  Several more (e.g. FLAG_CMD) were also already
	// created by MS::createDefaultSubtables().  Don't try to write over them - a
	// locking error will result.
	const TableRecord& outkws = msOut_p.keywordSet();
	for (uInt i = 0; i < outkws.nfields(); ++i)
	{
		if (outkws.type(i) == TpTable && inkws.isDefined(outkws.name(i)))
		{
			inkws.removeField(outkws.name(i));
		}
	}

	// Find ephemerides files
	std::vector<String> ephemerides;
	for (uInt i = 0; i < inkws.nfields(); ++i)
	{
		if (inkws.type(i) == TpTable && inkws.name(i).contains("EPHEM"))
		{
			ephemerides.push_back(inkws.name(i));
		}
	}

	// Remove ephemerides files
	for (uInt i = 0; i < ephemerides.size(); ++i)
	{
		inkws.removeField(ephemerides.at(i));
	}


	// msOut_p.rwKeywordSet() (pass a reference, not a copy) will put a lock on msOut_p.
	TableCopy::copySubTables(msOut_p.rwKeywordSet(), inkws, msOut_p.tableName(), msOut_p.tableType(), mssel_p);

	// TableCopy::copySubTables(Table, Table, Bool) includes this other code,
	// which seems to be copying subtables at one level deeper, but not recursively?
	const TableDesc& inDesc = mssel_p.tableDesc();
	const TableDesc& outDesc = msOut_p.tableDesc();
	for (uInt i = 0; i < outDesc.ncolumn(); ++i)
	{
		// Only writable cols can have keywords (and thus subtables) defined.
		if (msOut_p.isColumnWritable(i))
		{
			const String& name = outDesc[i].name();

			if (inDesc.isColumn(name))
			{
			    TableColumn outCol(msOut_p, name);
			    TableColumn inCol(mssel_p, name);

			    TableCopy::copySubTables(outCol.rwKeywordSet(),
						     inCol.keywordSet(),
						     msOut_p.tableName(),
						     msOut_p.tableType(),
						     mssel_p);
			    // Copy the keywords if column is [FLOAT_|CORRECTED_]DATA
			    // KS NOTE CORRECTED_DATA -> DATA case should be handled separately.
			    if (name == "FLOAT_DATA" || name == "DATA" || name == "CORRECTED_DATA")
					copyMainTableKeywords(outCol.rwKeywordSet(), inCol.keywordSet());

			}
		}
	}

	return true;
}

// -----------------------------------------------------------------------
// Method to merge SPW sub-tables from SubMSs to create the MMS level SPW sub-table
// -----------------------------------------------------------------------
Bool MSTransformDataHandler::mergeSpwSubTables(Vector<String> filenames)
{
	LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));

	String filename_0 = filenames(0);
	MeasurementSet ms_0(filename_0,Table::Update);

	if (Table::isReadable(ms_0.spectralWindowTableName()) and !ms_0.spectralWindow().isNull())
	{
		MSSpectralWindow spwTable_0 = ms_0.spectralWindow();

		if (spwTable_0.nrow() > 0)
		{
        	os << LogIO::NORMAL << LogOrigin("MSTransformDataHandler", __FUNCTION__)
        			<< "Merging SPECTRAL_WINDOW sub-tables from all sub-MSs to form MMS-level SPECTRAL_WINDOW sub-table" << LogIO::POST;

        	MSSpWindowColumns spwCols_0(spwTable_0);

			// Map subMS with spw_id to merge the FEED table later
			Vector<uInt> mapSubmsSpwid;

			// subMS_0000 starts with spw 0
			uInt spwStart = 0;
			mapSubmsSpwid.resize(filenames.size());
			mapSubmsSpwid[0] = spwStart;

			// for next subMS
			uInt rowIndex = spwTable_0.nrow();
			spwStart = spwStart + rowIndex;
			for (uInt subms_index=1;subms_index < filenames.size();subms_index++)
			{
				String filename_i = filenames(subms_index);
				MeasurementSet ms_i(filename_i);
				MSSpectralWindow spwTable_i = ms_i.spectralWindow();

				if (spwTable_i.nrow() > 0)
				{
					MSSpWindowColumns spwCols_i(spwTable_i);

					uInt nrow = spwTable_i.nrow();
					spwTable_0.addRow(nrow);

					// Map of this subMS to spw ID
					mapSubmsSpwid[subms_index] = spwStart;

					// for next subMS
					spwStart = spwStart + nrow;

					for (uInt subms_row_index=0;subms_row_index<spwTable_i.nrow();subms_row_index++)
					{
						spwCols_0.measFreqRef().put(rowIndex,spwCols_i.measFreqRef()(subms_row_index));
						spwCols_0.chanFreq().put(rowIndex,spwCols_i.chanFreq()(subms_row_index));
						spwCols_0.refFrequency().put(rowIndex,spwCols_i.refFrequency()(subms_row_index));
						spwCols_0.chanWidth().put(rowIndex,spwCols_i.chanWidth()(subms_row_index));
						spwCols_0.effectiveBW().put(rowIndex,spwCols_i.effectiveBW()(subms_row_index));
						spwCols_0.resolution().put(rowIndex,spwCols_i.resolution()(subms_row_index));
						spwCols_0.flagRow().put(rowIndex,spwCols_i.flagRow()(subms_row_index));
						spwCols_0.freqGroup().put(rowIndex,spwCols_i.freqGroup()(subms_row_index));
						spwCols_0.freqGroupName().put(rowIndex,spwCols_i.freqGroupName()(subms_row_index));
						spwCols_0.ifConvChain().put(rowIndex,spwCols_i.ifConvChain()(subms_row_index));
						spwCols_0.name().put(rowIndex,spwCols_i.name()(subms_row_index));
						spwCols_0.netSideband().put(rowIndex,spwCols_i.netSideband()(subms_row_index));
						spwCols_0.numChan().put(rowIndex,spwCols_i.numChan()(subms_row_index));
						spwCols_0.totalBandwidth().put(rowIndex,spwCols_i.totalBandwidth()(subms_row_index));

						// Optional columns
						if (columnOk(spwCols_i.bbcNo()))
							spwCols_0.bbcNo().put(rowIndex,spwCols_i.bbcNo()(subms_row_index));

						if (columnOk(spwCols_i.assocSpwId()))
							spwCols_0.assocSpwId().put(rowIndex,spwCols_i.assocSpwId()(subms_row_index));

						if(columnOk(spwCols_i.assocNature()))
							spwCols_0.assocNature().put(rowIndex,spwCols_i.assocNature()(subms_row_index));

						if (columnOk(spwCols_i.bbcSideband()))
							spwCols_0.bbcSideband().put(rowIndex,spwCols_i.bbcSideband()(subms_row_index));

						if (columnOk(spwCols_i.dopplerId()))
							spwCols_0.dopplerId().put(rowIndex,spwCols_i.dopplerId()(subms_row_index));

						if (columnOk(spwCols_i.receiverId()))
							spwCols_0.receiverId().put(rowIndex,spwCols_i.receiverId()(subms_row_index));

                        if (spwTable_i.tableDesc().isColumn("SDM_WINDOW_FUNCTION") &&
                            spwTable_i.tableDesc().columnDescSet().isDefined("SDM_WINDOW_FUNCTION"))
                        {
                            ScalarColumn<String> swfCol_i(spwTable_i, "SDM_WINDOW_FUNCTION");
                            ScalarColumn<String> swfCol_0(spwTable_0, "SDM_WINDOW_FUNCTION");
                            swfCol_0.put(rowIndex, swfCol_i(subms_row_index));
                        }

                        if (spwTable_i.tableDesc().isColumn("SDM_NUM_BIN") &&
                            spwTable_i.tableDesc().columnDescSet().isDefined("SDM_NUM_BIN"))
                        {
                            ScalarColumn<Int> snbCol_i(spwTable_i, "SDM_NUM_BIN");
                            ScalarColumn<Int> snbCol_0(spwTable_0, "SDM_NUM_BIN");
                            snbCol_0.put(rowIndex, snbCol_i(subms_row_index));
                        }
                        if (spwTable_i.tableDesc().isColumn("SDM_CORR_BIT") &&
                            spwTable_i.tableDesc().columnDescSet().isDefined("SDM_CORR_BIT"))
                        {
                            ScalarColumn<String> corrBitCol_i(spwTable_i, "SDM_CORR_BIT");
                            ScalarColumn<String> corrBitCol_0(spwTable_0, "SDM_CORR_BIT");
                            corrBitCol_0.put(rowIndex, corrBitCol_i(subms_row_index));
                        }

						rowIndex += 1;
					}
				}
			}

			// Merge the other sub-tables using SPW map generated here
			mergeDDISubTables(filenames);
			mergeFeedSubTables(filenames, mapSubmsSpwid);
			mergeSourceSubTables(filenames, mapSubmsSpwid);
			mergeFreqOffsetTables(filenames, mapSubmsSpwid);
			mergeCalDeviceSubtables(filenames, mapSubmsSpwid);
			mergeSysPowerSubtables(filenames, mapSubmsSpwid);
			mergeSyscalSubTables(filenames, mapSubmsSpwid);
		}
		else
		{
	    	os << LogIO::SEVERE << LogOrigin("MSTransformDataHandler", __FUNCTION__)
	    			<< "SPECTRAL_WINDOW sub-table found but has no valid content" << LogIO::POST;
	    	return false;
		}
	}
	else
	{
    	os << LogIO::SEVERE << LogOrigin("MSTransformDataHandler", __FUNCTION__)
    			<< "SPECTRAL_WINDOW sub-table not found " << LogIO::POST;
    	return false;
	}

	return true;
}

// -----------------------------------------------------------------------
// Method to merge DDI sub-tables from SubMSs to create the MMS-level DDI sub-table
// -----------------------------------------------------------------------
Bool MSTransformDataHandler::mergeDDISubTables(Vector<String> filenames)
{
	LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));

	String filename_0 = filenames(0);
	MeasurementSet ms_0(filename_0,Table::Update);

    if (Table::isReadable(ms_0.dataDescriptionTableName()) and !ms_0.dataDescription().isNull())
    {
    	MSDataDescription ddiTable_0 = ms_0.dataDescription();

    	if (ddiTable_0.nrow() > 0)
    	{
        	os << LogIO::NORMAL << LogOrigin("MSTransformDataHandler", __FUNCTION__)
        			<< "Merging DDI sub-tables from all sub-MSs to form MMS-level DDI sub-table" << LogIO::POST;

        	MSDataDescColumns ddiCols_0(ddiTable_0);

        	uInt rowIndex = ddiTable_0.nrow();
        	for (uInt subms_index=1;subms_index < filenames.size();subms_index++)
        	{
        		String filename_i = filenames(subms_index);
        		MeasurementSet ms_i(filename_i);
        		MSDataDescription dditable_i = ms_i.dataDescription();

        		if (dditable_i.nrow() > 0)
        		{
            		MSDataDescColumns ddicols_i(dditable_i);

            		ddiTable_0.addRow(dditable_i.nrow());

            		for (uInt subms_row_index=0;subms_row_index<dditable_i.nrow();subms_row_index++)
            		{
            			// get the last spw id entered in the 0th DD table
            			uInt spwid = ddiCols_0.spectralWindowId().get(rowIndex-1);

            			ddiCols_0.flagRow().put(rowIndex,ddicols_i.flagRow()(subms_row_index));
            			ddiCols_0.polarizationId().put(rowIndex,ddicols_i.polarizationId()(subms_row_index));

            			// Take into account that some SPW may be pointed by several DDIs
            			uInt deltaDDI = 1;
            			if (subms_row_index>0)
            			{
            				deltaDDI = ddicols_i.spectralWindowId()(subms_row_index) - ddicols_i.spectralWindowId()(subms_row_index-1);
            			}

            			ddiCols_0.spectralWindowId().put(rowIndex,spwid+deltaDDI);
            			rowIndex += 1;
            		}
        		}
        	}
    	}
		else
		{
	    	os << LogIO::SEVERE << LogOrigin("MSTransformDataHandler", __FUNCTION__)
	    			<< "DDI sub-table found but has no valid content" << LogIO::POST;
	    	return false;
		}
    }
    else
    {
    	os << LogIO::SEVERE << LogOrigin("MSTransformDataHandler", __FUNCTION__)
    			<< "DDI sub-table not found " << LogIO::POST;
    	return false;
    }

	return true;
}


// -----------------------------------------------------------------------
// Method to merge FEED sub-tables from SubMSs to create the MMS-level FEED sub-table
// -----------------------------------------------------------------------
Bool MSTransformDataHandler::mergeFeedSubTables(Vector<String> filenames, Vector<uInt> mapSubmsSpwid)
{
	LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));

	if (filenames.size() != mapSubmsSpwid.size())
	{
		os 	<< LogIO::SEVERE << "List of Sub-MSs does not match size of SPW re-indexing map" << LogIO::POST;
		return false;
	}

	String filename_0 = filenames(0);
	MeasurementSet ms_0(filename_0,Table::Update);

    if (Table::isReadable(ms_0.feedTableName()) and !ms_0.feed().isNull())
    {
    	MSFeed feedTable_0 = ms_0.feed();

    	// CAS-7167. The WVR spw has no FEED content.
//    	if (feedTable_0.nrow() >= 0)
//    	{
        os << LogIO::NORMAL << LogOrigin("MSTransformDataHandler", __FUNCTION__)
                << "Merging FEED sub-tables from all sub-MSs to form MMS-level FEED sub-table" << LogIO::POST;

        MSFeedColumns feedCols_0(feedTable_0);

        uInt rowIndex = feedTable_0.nrow();
        for (uInt subms_index=1;subms_index < filenames.size();subms_index++)
        {
            String filename_i = filenames(subms_index);
            MeasurementSet ms_i(filename_i);
            MSFeed feedtable_i = ms_i.feed();

            if (feedtable_i.nrow() > 0)
            {
                MSFeedColumns feedcols_i(feedtable_i);

                feedTable_0.addRow(feedtable_i.nrow());

                // Prepare row reference object
                RefRows refRow(rowIndex,rowIndex+feedtable_i.nrow()-1);

                // Re-index SPW col
                Vector<Int> spectralWindowId_output(feedtable_i.nrow(),mapSubmsSpwid[subms_index]);
                spectralWindowId_output += feedcols_i.spectralWindowId().getColumn();
                feedCols_0.spectralWindowId().putColumnCells(refRow,spectralWindowId_output);

                // Columns that can be just copied
                feedCols_0.position().putColumnCells(refRow,feedcols_i.position().getColumn());
                feedCols_0.beamOffset().putColumnCells(refRow,feedcols_i.beamOffset().getColumn());
                feedCols_0.polarizationType().putColumnCells(refRow,feedcols_i.polarizationType().getColumn());
                feedCols_0.polResponse().putColumnCells(refRow,feedcols_i.polResponse().getColumn());
                feedCols_0.receptorAngle().putColumnCells(refRow,feedcols_i.receptorAngle().getColumn());
                feedCols_0.antennaId().putColumnCells(refRow,feedcols_i.antennaId().getColumn());
                feedCols_0.beamId().putColumnCells(refRow,feedcols_i.beamId().getColumn());
                feedCols_0.feedId().putColumnCells(refRow,feedcols_i.feedId().getColumn());
                feedCols_0.interval().putColumnCells(refRow,feedcols_i.interval().getColumn());
                feedCols_0.numReceptors().putColumnCells(refRow,feedcols_i.numReceptors().getColumn());
                feedCols_0.time().putColumnCells(refRow,feedcols_i.time().getColumn());

                // optional columns
                if (columnOk(feedcols_i.focusLength()))
                {
                    feedCols_0.focusLength().putColumnCells(refRow,feedcols_i.focusLength().getColumn());
                }

                if (columnOk(feedcols_i.phasedFeedId()))
                {
                    feedCols_0.phasedFeedId().putColumnCells(refRow,feedcols_i.phasedFeedId().getColumn());
                }

                // Increment row offset
                rowIndex += feedtable_i.nrow();
            }
        }

//    	}
/*
		else
		{
	    	os << LogIO::SEVERE << LogOrigin("MSTransformDataHandler", __FUNCTION__)
	    			<< "FEED sub-table found but has no valid content" << LogIO::POST;
	    	return false;
		}
*/
    }
    else
    {
    	os << LogIO::SEVERE << LogOrigin("MSTransformDataHandler", __FUNCTION__) <<
    			 "FEED sub-table not found " << LogIO::POST;
    	return false;
    }

	return true;
}

// -----------------------------------------------------------------------
// Method to merge Source sub-tables from SubMSs to create the MMS-level FEED sub-table
// -----------------------------------------------------------------------
Bool MSTransformDataHandler::mergeSourceSubTables(Vector<String> filenames, Vector<uInt> mapSubmsSpwid)
{
	LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));

	if (filenames.size() != mapSubmsSpwid.size())
	{
		os 	<< LogIO::SEVERE << "List of Sub-MSs does not match size of SPW re-indexing map" << LogIO::POST;
		return false;
	}

	String filename_0 = filenames(0);
	MeasurementSet ms_0(filename_0,Table::Update);

	if (Table::isReadable(ms_0.sourceTableName()) and !ms_0.source().isNull())
	{
		MSSource sourceTable_0 = ms_0.source();

		if (sourceTable_0.nrow() > 0)
		{
	    	os << LogIO::NORMAL << LogOrigin("MSTransformDataHandler", __FUNCTION__)
	    			<< "Merging SOURCE sub-tables from all sub-MSs to form MMS-level SOURCE sub-table" << LogIO::POST;

			MSSourceColumns sourceCols_0(sourceTable_0);

			uInt rowIndex = sourceTable_0.nrow();
			for (uInt subms_index=1;subms_index < filenames.size();subms_index++)
			{
				String filename_i = filenames(subms_index);
				MeasurementSet ms_i(filename_i);
				MSSource sourcetable_i = ms_i.source();

				if (sourcetable_i.nrow() > 0)
				{
					MSSourceColumns sourcecols_i(sourcetable_i);

					sourceTable_0.addRow(sourcetable_i.nrow());

		        	// Prepare row reference object
		        	RefRows refRow(rowIndex,rowIndex+sourcetable_i.nrow()-1);

		        	// Re-index SPW col
		        	Vector<Int> spectralWindowId_output(sourcetable_i.nrow(),mapSubmsSpwid[subms_index]);
		        	spectralWindowId_output += sourcecols_i.spectralWindowId().getColumn();
		        	sourceCols_0.spectralWindowId().putColumnCells(refRow,spectralWindowId_output);

		        	// Columns that can be just copied
		    		sourceCols_0.direction().putColumnCells(refRow,sourcecols_i.direction().getColumn());
		    		sourceCols_0.properMotion().putColumnCells(refRow,sourcecols_i.properMotion().getColumn());
		    		sourceCols_0.calibrationGroup().putColumnCells(refRow,sourcecols_i.calibrationGroup().getColumn());
		    		sourceCols_0.code().putColumnCells(refRow,sourcecols_i.code().getColumn());
		    		sourceCols_0.interval().putColumnCells(refRow,sourcecols_i.interval().getColumn());
		    		sourceCols_0.name().putColumnCells(refRow,sourcecols_i.name().getColumn());
		    		sourceCols_0.numLines().putColumnCells(refRow,sourcecols_i.numLines().getColumn());
		    		sourceCols_0.sourceId().putColumnCells(refRow,sourcecols_i.sourceId().getColumn());
		    		sourceCols_0.time().putColumnCells(refRow,sourcecols_i.time().getColumn());

		    		// Optional columns
		    		if (columnOk(sourcecols_i.position()))
		    		{
		    			sourceCols_0.position().putColumnCells(refRow,sourcecols_i.position().getColumn());
		    		}

		    		if (columnOk(sourcecols_i.transition()))
		    		{
		    			sourceCols_0.transition().putColumnCells(refRow,sourcecols_i.transition().getColumn());
		    		}

		    		if (columnOk(sourcecols_i.restFrequency()))
		    		{
		    			sourceCols_0.restFrequency().putColumnCells(refRow,sourcecols_i.restFrequency().getColumn());
		    		}

		    		if (columnOk(sourcecols_i.sysvel()))
		    		{
		    			sourceCols_0.sysvel().putColumnCells(refRow,sourcecols_i.sysvel().getColumn());
		    		}

		    		if (columnOk(sourcecols_i.pulsarId()))
		    		{
		    			sourceCols_0.pulsarId().putColumnCells(refRow,sourcecols_i.pulsarId().getColumn());
		    		}

		    		if (columnOk(sourcecols_i.sourceModel()))
		    		{
		    			sourceCols_0.sourceModel().putColumnCells(refRow,sourcecols_i.sourceModel().getColumn());
		    		}

		        	// Increment row offset
		        	rowIndex += sourcetable_i.nrow();
				}
			}

		}
		else
		{
	    	os << LogIO::SEVERE << LogOrigin("MSTransformDataHandler", __FUNCTION__)
	    			<< "SOURCE sub-table found but has no valid content" << LogIO::POST;
	    	return false;
		}
	}
	else
	{
		os << LogIO::SEVERE << LogOrigin("MSTransformDataHandler", __FUNCTION__)
    			<< "SOURCE sub-table not found " << LogIO::POST;
    	return false;
	}

	return true;
}


// -----------------------------------------------------------------------
// Method to merge Syscal sub-tables from SubMSs to create the MMS-level Syscal sub-table
// -----------------------------------------------------------------------
Bool MSTransformDataHandler::mergeSyscalSubTables(Vector<String> filenames, Vector<uInt> mapSubmsSpwid)
{
	LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));

	if (filenames.size() != mapSubmsSpwid.size())
	{
		os 	<< LogIO::SEVERE << "List of Sub-MSs does not match size of SPW re-indexing map" << LogIO::POST;
		return false;
	}

	String filename_0 = filenames(0);
	MeasurementSet ms_0(filename_0,Table::Update);

	if(Table::isReadable(ms_0.sysCalTableName()) and !ms_0.sysCal().isNull())
	{
		MSSysCal syscalTable_0 = ms_0.sysCal();

		// CAS-7167. The WVR spw has no FEED content.
//		if (syscalTable_0.nrow() >= 0)
//		{
        os << LogIO::NORMAL << LogOrigin("MSTransformDataHandler", __FUNCTION__)
                << "Merging SYSCAL sub-tables from all sub-MSs to form MMS-level SYSCAL sub-table" << LogIO::POST;

        MSSysCalColumns syscalCols_0(syscalTable_0);

        uInt rowIndex = syscalTable_0.nrow();
        for (uInt subms_index=1;subms_index < filenames.size();subms_index++)
        {
            String filename_i = filenames(subms_index);
            MeasurementSet ms_i(filename_i);
            MSSysCal syscaltable_i = ms_i.sysCal();

            if (syscaltable_i.nrow() > 0)
            {
                MSSysCalColumns syscalcols_i(syscaltable_i);

                syscalTable_0.addRow(syscaltable_i.nrow());

                // Prepare row reference object
                RefRows refRow(rowIndex,rowIndex+syscaltable_i.nrow()-1);

                // Re-index SPW col
                Vector<Int> spectralWindowId_output(syscaltable_i.nrow(),mapSubmsSpwid[subms_index]);
                spectralWindowId_output += syscalcols_i.spectralWindowId().getColumn();
                syscalCols_0.spectralWindowId().putColumnCells(refRow,spectralWindowId_output);

                // Columns that can be just copied
                syscalCols_0.antennaId().putColumnCells(refRow,syscalcols_i.antennaId().getColumn());
                syscalCols_0.feedId().putColumnCells(refRow,syscalcols_i.feedId().getColumn());
                syscalCols_0.interval().putColumnCells(refRow,syscalcols_i.interval().getColumn());
                syscalCols_0.time().putColumnCells(refRow,syscalcols_i.time().getColumn());

                // Optional columns
                if (columnOk(syscalcols_i.phaseDiff()))
                {
                    syscalCols_0.phaseDiff().putColumnCells(refRow,syscalcols_i.phaseDiff().getColumn());
                }

                if (columnOk(syscalcols_i.phaseDiffFlag()))
                {
                    syscalCols_0.phaseDiffFlag().putColumnCells(refRow,syscalcols_i.phaseDiffFlag().getColumn());
                }

                if (columnOk(syscalcols_i.tant()))
                {
                    syscalCols_0.tant().putColumnCells(refRow,syscalcols_i.tant().getColumn());
                }

                if (columnOk(syscalcols_i.tantFlag()))
                {
                    syscalCols_0.tantFlag().putColumnCells(refRow,syscalcols_i.tantFlag().getColumn());
                }

                if (columnOk(syscalcols_i.tantSpectrum()))
                {
                    syscalCols_0.tantSpectrum().putColumnCells(refRow,syscalcols_i.tantSpectrum().getColumn());
                }

                if (columnOk(syscalcols_i.tantTsys()))
                {
                    syscalCols_0.tantTsys().putColumnCells(refRow,syscalcols_i.tantTsys().getColumn());
                }

                if (columnOk(syscalcols_i.tantTsysFlag()))
                {
                    syscalCols_0.tantTsysFlag().putColumnCells(refRow,syscalcols_i.tantTsysFlag().getColumn());
                }

                if (columnOk(syscalcols_i.tantTsysSpectrum()))
                {
                    syscalCols_0.tantTsysSpectrum().putColumnCells(refRow,syscalcols_i.tantTsysSpectrum().getColumn());
                }

                if (columnOk(syscalcols_i.tcal()))
                {
                    syscalCols_0.tcal().putColumnCells(refRow,syscalcols_i.tcal().getColumn());
                }

                if (columnOk(syscalcols_i.tcalFlag()))
                {
                    syscalCols_0.tcalFlag().putColumnCells(refRow,syscalcols_i.tcalFlag().getColumn());
                }

                if (columnOk(syscalcols_i.tcalSpectrum()))
                {
                    syscalCols_0.tcalSpectrum().putColumnCells(refRow,syscalcols_i.tcalSpectrum().getColumn());
                }

                if (columnOk(syscalcols_i.trx()))
                {
                    syscalCols_0.trx().putColumnCells(refRow,syscalcols_i.trx().getColumn());
                }

                if (columnOk(syscalcols_i.trxFlag()))
                {
                    syscalCols_0.trxFlag().putColumnCells(refRow,syscalcols_i.trxFlag().getColumn());
                }

                if (columnOk(syscalcols_i.trxSpectrum()))
                {
                    syscalCols_0.trxSpectrum().putColumnCells(refRow,syscalcols_i.trxSpectrum().getColumn());
                }

                if (columnOk(syscalcols_i.tsky()))
                {
                    syscalCols_0.tsky().putColumnCells(refRow,syscalcols_i.tsky().getColumn());
                }

                if (columnOk(syscalcols_i.tskyFlag()))
                {
                    syscalCols_0.tskyFlag().putColumnCells(refRow,syscalcols_i.tskyFlag().getColumn());
                }

                if (columnOk(syscalcols_i.tskySpectrum()))
                {
                    syscalCols_0.tskySpectrum().putColumnCells(refRow,syscalcols_i.tskySpectrum().getColumn());
                }

                if (columnOk(syscalcols_i.tsys()))
                {
                    syscalCols_0.tsys().putColumnCells(refRow,syscalcols_i.tsys().getColumn());
                }

                if (columnOk(syscalcols_i.tsysFlag()))
                {
                    syscalCols_0.tsysFlag().putColumnCells(refRow,syscalcols_i.tsysFlag().getColumn());
                }

                if (columnOk(syscalcols_i.tsysSpectrum()))
                {
                    syscalCols_0.tsysSpectrum().putColumnCells(refRow,syscalcols_i.tsysSpectrum().getColumn());
                }

                // Increment row offset
                rowIndex += syscalcols_i.nrow();
            }
        }

/*
		}
		else
		{
	    	os << LogIO::DEBUG1 << LogOrigin("MSTransformDataHandler", __FUNCTION__)
	    			<< "SYSCAL sub-table found but has no valid content" << LogIO::POST;
	    	return false;
		}
*/
	}
	else
	{
		os << LogIO::DEBUG1 << LogOrigin("MSTransformDataHandler", __FUNCTION__)
    			<< "SYSCAL sub-table not found " << LogIO::POST;
    	return false;
	}

	return true;
}


// -----------------------------------------------------------------------
// Method to merge FreqOffset sub-tables from SubMSs to create the MMS-level FreqOffset sub-table
// -----------------------------------------------------------------------
Bool MSTransformDataHandler::mergeFreqOffsetTables(Vector<String> filenames, Vector<uInt> mapSubmsSpwid)
{
	LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));

	if (filenames.size() != mapSubmsSpwid.size())
	{
		os 	<< LogIO::SEVERE << "List of Sub-MSs does not match size of SPW re-indexing map" << LogIO::POST;
		return false;
	}

	String filename_0 = filenames(0);
	MeasurementSet ms_0(filename_0,Table::Update);

	if (Table::isReadable(ms_0.freqOffsetTableName()) and !ms_0.freqOffset().isNull())
	{
		MSFreqOffset freqoffsetTable_0 = ms_0.freqOffset();

		if (freqoffsetTable_0.nrow() > 0)
		{
	    	os << LogIO::NORMAL << LogOrigin("MSTransformDataHandler", __FUNCTION__)
	    			<< "Merging FREQ_OFFSET sub-tables from all sub-MSs to form MMS-level FREQ_OFFSET sub-table" << LogIO::POST;

			MSFreqOffsetColumns freqoffsetCols_0(freqoffsetTable_0);

			uInt rowIndex = freqoffsetTable_0.nrow();
			for (uInt subms_index=1;subms_index < filenames.size();subms_index++)
			{
				String filename_i = filenames(subms_index);
				MeasurementSet ms_i(filename_i);
				MSFreqOffset freqoffsettable_i = ms_i.freqOffset();

				if (freqoffsettable_i.nrow() > 0)
				{
					MSFreqOffsetColumns freqoffsetcols_i(freqoffsettable_i);

					freqoffsetTable_0.addRow(freqoffsettable_i.nrow());

		        	// Prepare row reference object
		        	RefRows refRow(rowIndex,rowIndex+freqoffsettable_i.nrow()-1);

		        	// Re-index SPW col
		        	Vector<Int> spectralWindowId_output(freqoffsettable_i.nrow(),mapSubmsSpwid[subms_index]);
		        	spectralWindowId_output += freqoffsetcols_i.spectralWindowId().getColumn();
		        	freqoffsetCols_0.spectralWindowId().putColumnCells(refRow,spectralWindowId_output);

		        	// Columns that can be just copied
		    		freqoffsetCols_0.antenna1().putColumnCells(refRow,freqoffsetcols_i.antenna1().getColumn());
		    		freqoffsetCols_0.antenna2().putColumnCells(refRow,freqoffsetcols_i.antenna2().getColumn());
		    		freqoffsetCols_0.feedId().putColumnCells(refRow,freqoffsetcols_i.feedId().getColumn());
		    		freqoffsetCols_0.interval().putColumnCells(refRow,freqoffsetcols_i.interval().getColumn());
		    		freqoffsetCols_0.offset().putColumnCells(refRow,freqoffsetcols_i.offset().getColumn());
		    		freqoffsetCols_0.time().putColumnCells(refRow,freqoffsetcols_i.time().getColumn());

		    		// NOTE (jagonzal): FreqOffset does not have optional columns

		        	// Increment row offset
		        	rowIndex += freqoffsettable_i.nrow();
				}
			}

		}
		else
		{
	    	os << LogIO::DEBUG1 << LogOrigin("MSTransformDataHandler", __FUNCTION__)
	    			<< "FREQ_OFFSET sub-table found but has no valid content" << LogIO::POST;
	    	return false;
		}

	}
	else
	{
		os << LogIO::DEBUG1 << LogOrigin("MSTransformDataHandler", __FUNCTION__)
    			<< "FREQ_OFFSET sub-table not found " << LogIO::POST;
    	return false;
	}

	return true;
}

// -----------------------------------------------------------------------
// Method to merge CalDevice sub-tables from SubMSs to create the MMS-level CalDevice sub-table
// -----------------------------------------------------------------------
Bool MSTransformDataHandler::mergeCalDeviceSubtables(Vector<String> filenames, Vector<uInt> mapSubmsSpwid)
{
	LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));

	if (filenames.size() != mapSubmsSpwid.size())
	{
		os 	<< LogIO::SEVERE << "List of Sub-MSs does not match size of SPW re-indexing map" << LogIO::POST;
		return false;
	}

	String filename_0 = filenames(0);
	MeasurementSet ms_0(filename_0,Table::Update);

	if (Table::isReadable(ms_0.tableName() + "/CALDEVICE"))
	{
		Table subtable_0(ms_0.tableName() + "/CALDEVICE", Table::Update);

		if (subtable_0.nrow() > 0)
		{
	    	os << LogIO::NORMAL << LogOrigin("MSTransformDataHandler", __FUNCTION__)
	    			<< "Merging CALDEVICE sub-tables from all sub-MSs to form MMS-level CALDEVICE sub-table" << LogIO::POST;

	        // Get RW access to columns
			ScalarColumn<Int> antennaIdCol_0(subtable_0, "ANTENNA_ID");
			ScalarColumn<Int> feedIdCol_0(subtable_0, "FEED_ID");
			ScalarColumn<Int> spectralWindowIdCol_0(subtable_0, "SPECTRAL_WINDOW_ID");
			ScalarColumn<Double> timeCol_0(subtable_0, "TIME");
			ScalarColumn<Double> intervalCol_0(subtable_0, "INTERVAL");

			ScalarColumn<Int> numCalLoadCol_0(subtable_0, "NUM_CAL_LOAD");
			ArrayColumn<String> calLoadNamesCol_0(subtable_0, "CAL_LOAD_NAMES");
			ScalarColumn<Int> numReceptorCol_0(subtable_0, "NUM_RECEPTOR");
			ArrayColumn<Float> noiseCalCol_0(subtable_0, "NOISE_CAL");
			ArrayColumn<Float> calEffCol_0(subtable_0, "CAL_EFF");
			ArrayColumn<Double> temperatureLoadCol_0(subtable_0, "TEMPERATURE_LOAD");

	        // Get original content of columns
			Vector<Int> antennaId_0;
			if (columnOk(antennaIdCol_0)) antennaId_0 = antennaIdCol_0.getColumn();
			Vector<Int> feedId_0;
			if (columnOk(feedIdCol_0)) feedId_0 = feedIdCol_0.getColumn();
			Vector<Int> spectralWindowId_0;
			if (columnOk(spectralWindowIdCol_0)) spectralWindowId_0 = spectralWindowIdCol_0.getColumn();
			Vector<Double> time_0;
			if (columnOk(timeCol_0)) time_0 = timeCol_0.getColumn();
			Vector<Double> interval_0;
			if (columnOk(intervalCol_0)) interval_0 = intervalCol_0.getColumn();

			Vector<Int> numCalLoad_0;
			if (columnOk(numCalLoadCol_0)) numCalLoad_0 = numCalLoadCol_0.getColumn();
			Array<String> calLoadNames_0;
			if (columnOk(calLoadNamesCol_0)) calLoadNames_0 = calLoadNamesCol_0.getColumn();
			Vector<Int> numReceptor_0;
			if (columnOk(numReceptorCol_0)) numReceptor_0 = numReceptorCol_0.getColumn();
			Array<Float> noiseCal_0;
			if (columnOk(noiseCalCol_0)) noiseCal_0 = noiseCalCol_0.getColumn();
			Array<Float> calEff_0;
			if (columnOk(calEffCol_0)) calEff_0 = calEffCol_0.getColumn();
			Array<Double> temperatureLoad_0;
			if (columnOk(temperatureLoadCol_0)) temperatureLoad_0 = temperatureLoadCol_0.getColumn();

			uInt rowIndex = subtable_0.nrow();
			for (uInt subms_index=1;subms_index < filenames.size();subms_index++)
			{
				String filename_i = filenames(subms_index);
				MeasurementSet ms_i(filename_i);
				Table subtable_i(ms_i.tableName() + "/CALDEVICE", Table::Update);

				if (subtable_i.nrow() > 0)
				{
			        // Get RW access to columns
					ScalarColumn<Int> antennaIdCol_i(subtable_i, "ANTENNA_ID");
					ScalarColumn<Int> feedIdCol_i(subtable_i, "FEED_ID");
					ScalarColumn<Int> spectralWindowIdCol_i(subtable_i, "SPECTRAL_WINDOW_ID");
					ScalarColumn<Double> timeCol_i(subtable_i, "TIME");
					ScalarColumn<Double> intervalCol_i(subtable_i, "INTERVAL");

					ScalarColumn<Int> numCalLoadCol_i(subtable_i, "NUM_CAL_LOAD");
					ArrayColumn<String> calLoadNamesCol_i(subtable_i, "CAL_LOAD_NAMES");
					ScalarColumn<Int> numReceptorCol_i(subtable_i, "NUM_RECEPTOR");
					ArrayColumn<Float> noiseCalCol_i(subtable_i, "NOISE_CAL");
					ArrayColumn<Float> calEffCol_i(subtable_i, "CAL_EFF");
					ArrayColumn<Double> temperatureLoadCol_i(subtable_i, "TEMPERATURE_LOAD");

			        // Get original content of columns
					Vector<Int> antennaId_i;
					if (columnOk(antennaIdCol_i)) antennaId_i = antennaIdCol_i.getColumn();
					Vector<Int> feedId_i;
					if (columnOk(feedIdCol_i)) feedId_i = feedIdCol_i.getColumn();
					Vector<Int> spectralWindowId_i;
					if (columnOk(spectralWindowIdCol_i)) spectralWindowId_i = spectralWindowIdCol_i.getColumn();
					Vector<Double> time_i;
					if (columnOk(timeCol_i)) time_i = timeCol_i.getColumn();
					Vector<Double> interval_i;
					if (columnOk(intervalCol_i)) interval_i = intervalCol_i.getColumn();

					Vector<Int> numCalLoad_i;
					if (columnOk(numCalLoadCol_i)) numCalLoad_i = numCalLoadCol_i.getColumn();
					Array<String> calLoadNames_i;
					if (columnOk(calLoadNamesCol_i)) calLoadNames_i = calLoadNamesCol_i.getColumn();
					Vector<Int> numReceptor_i;
					if (columnOk(numReceptorCol_i)) numReceptor_i = numReceptorCol_i.getColumn();
					Array<Float> noiseCal_i;
					if (columnOk(noiseCalCol_i)) noiseCal_i = noiseCalCol_i.getColumn();
					Array<Float> calEff_i;
					if (columnOk(calEffCol_i)) calEff_i = calEffCol_i.getColumn();
					Array<Double> temperatureLoad_i;
					if (columnOk(temperatureLoadCol_i)) temperatureLoad_i = temperatureLoadCol_i.getColumn();

					// Add n# rows to subtable_i equivalent to n# rows from subtable_0
					subtable_0.addRow(subtable_i.nrow());

		        	// Prepare row reference object
		        	RefRows refRow(rowIndex,rowIndex+subtable_i.nrow()-1);

		        	// Re-index SPW col
		        	Vector<Int> spectralWindowId_output(spectralWindowId_i.size(),mapSubmsSpwid[subms_index]);
		        	spectralWindowId_output += spectralWindowId_i;
		        	spectralWindowIdCol_0.putColumnCells(refRow,spectralWindowId_output);

		        	// Columns that can be just copied
					if (columnOk(antennaIdCol_i))
					{
						antennaIdCol_0.putColumnCells(refRow,antennaId_i);
					}

					if (columnOk(feedIdCol_i))
					{
						feedIdCol_0.putColumnCells(refRow,feedId_i);
					}

					if (columnOk(timeCol_i))
					{
						timeCol_0.putColumnCells(refRow,time_i);
					}

					if (columnOk(intervalCol_i))
					{
						intervalCol_0.putColumnCells(refRow,interval_i);
					}


					if (columnOk(numCalLoadCol_i))
					{
						numCalLoadCol_0.putColumnCells(refRow,numCalLoad_i);
					}

					if (columnOk(calLoadNamesCol_i))
					{
						calLoadNamesCol_0.putColumnCells(refRow,calLoadNames_i);
					}

					if (columnOk(numReceptorCol_i))
					{
						numReceptorCol_0.putColumnCells(refRow,numReceptor_i);
					}

					if (columnOk(noiseCalCol_i))
					{
						noiseCalCol_0.putColumnCells(refRow,noiseCal_i);
					}

					if (columnOk(calEffCol_i))
					{
						calEffCol_0.putColumnCells(refRow,calEff_i);
					}

					if (columnOk(temperatureLoadCol_i))
					{
						temperatureLoadCol_0.putColumnCells(refRow,temperatureLoad_i);
					}

		        	// Increment row offset
		        	rowIndex += subtable_i.nrow();
				}
			}

		}
		else
		{
	    	os << LogIO::DEBUG1 << LogOrigin("MSTransformDataHandler", __FUNCTION__)
	    			<< "CALDEVICE sub-table found but has no valid content" << LogIO::POST;
	    	return false;
		}
	}
	else
	{
		os << LogIO::DEBUG1 << LogOrigin("MSTransformDataHandler", __FUNCTION__)
    			<< "CALDEVICE sub-table not found " << LogIO::POST;
    	return false;
	}

	return true;
}


// -----------------------------------------------------------------------
// Method to merge SysPower sub-tables from SubMSs to create the MMS-level SysPower sub-table
// -----------------------------------------------------------------------
Bool MSTransformDataHandler::mergeSysPowerSubtables(Vector<String> filenames, Vector<uInt> mapSubmsSpwid)
{
	LogIO os(LogOrigin("MSTransformDataHandler", __FUNCTION__));

	if (filenames.size() != mapSubmsSpwid.size())
	{
		os 	<< LogIO::SEVERE << "List of Sub-MSs does not match size of SPW re-indexing map" << LogIO::POST;
		return false;
	}

	String filename_0 = filenames(0);
	MeasurementSet ms_0(filename_0,Table::Update);

	if (Table::isReadable(ms_0.tableName() + "/SYSPOWER"))
	{
		Table subtable_0(ms_0.tableName() + "/SYSPOWER", Table::Update);

		if (subtable_0.nrow() > 0)
		{
	    	os << LogIO::NORMAL << LogOrigin("MSTransformDataHandler", __FUNCTION__)
	    			<< "Merging SYSPOWER sub-tables from all sub-MSs to form MMS-level SYSPOWER sub-table" << LogIO::POST;

	        // Get RW access to columns
			ScalarColumn<Int> antennaIdCol_0(subtable_0, "ANTENNA_ID");
			ScalarColumn<Int> feedIdCol_0(subtable_0, "FEED_ID");
			ScalarColumn<Int> spectralWindowIdCol_0(subtable_0, "SPECTRAL_WINDOW_ID");
			ScalarColumn<Double> timeCol_0(subtable_0, "TIME");
			ScalarColumn<Double> intervalCol_0(subtable_0, "INTERVAL");

			ArrayColumn<Float> switchedDiffCol_0(subtable_0, "SWITCHED_DIFF");
			ArrayColumn<Float> switchedSumCol_0(subtable_0, "SWITCHED_SUM");
			ArrayColumn<Float> requantizerGainCol_0(subtable_0, "REQUANTIZER_GAIN");

	        // Get original content of columns
			Vector<Int> antennaId_0;
			if (columnOk(antennaIdCol_0)) antennaId_0 = antennaIdCol_0.getColumn();
			Vector<Int> feedId_0;
			if (columnOk(feedIdCol_0)) feedId_0 = feedIdCol_0.getColumn();
			Vector<Int> spectralWindowId_0;
			if (columnOk(spectralWindowIdCol_0)) spectralWindowId_0 = spectralWindowIdCol_0.getColumn();
			Vector<Double> time_0;
			if (columnOk(timeCol_0)) time_0 = timeCol_0.getColumn();
			Vector<Double> interval_0;
			if (columnOk(intervalCol_0)) interval_0 = intervalCol_0.getColumn();

			Array<Float> switchedDiff_0;
			if (columnOk(switchedDiffCol_0)) switchedDiff_0 = switchedDiffCol_0.getColumn();
			Array<Float> switchedSum_0;
			if (columnOk(switchedSumCol_0)) switchedSum_0 = switchedSumCol_0.getColumn();
			Array<Float> requantizerGain_0;
			if (columnOk(requantizerGainCol_0)) requantizerGain_0 = requantizerGainCol_0.getColumn();

			uInt rowIndex = subtable_0.nrow();
			for (uInt subms_index=1;subms_index < filenames.size();subms_index++)
			{
				String filename_i = filenames(subms_index);
				MeasurementSet ms_i(filename_i);
				Table subtable_i(ms_i.tableName() + "/SYSPOWER", Table::Update);

				if (subtable_i.nrow() > 0)
				{
			        // Get RW access to columns
					ScalarColumn<Int> antennaIdCol_i(subtable_i, "ANTENNA_ID");
					ScalarColumn<Int> feedIdCol_i(subtable_i, "FEED_ID");
					ScalarColumn<Int> spectralWindowIdCol_i(subtable_i, "SPECTRAL_WINDOW_ID");
					ScalarColumn<Double> timeCol_i(subtable_i, "TIME");
					ScalarColumn<Double> intervalCol_i(subtable_i, "INTERVAL");

					ArrayColumn<Float> switchedDiffCol_i(subtable_i, "SWITCHED_DIFF");
					ArrayColumn<Float> switchedSumCol_i(subtable_i, "SWITCHED_SUM");
					ArrayColumn<Float> requantizerGainCol_i(subtable_i, "REQUANTIZER_GAIN");

			        // Get original content of columns
					Vector<Int> antennaId_i;
					if (columnOk(antennaIdCol_i)) antennaId_i = antennaIdCol_i.getColumn();
					Vector<Int> feedId_i;
					if (columnOk(feedIdCol_i)) feedId_i = feedIdCol_i.getColumn();
					Vector<Int> spectralWindowId_i;
					if (columnOk(spectralWindowIdCol_i)) spectralWindowId_i = spectralWindowIdCol_i.getColumn();
					Vector<Double> time_i;
					if (columnOk(timeCol_i)) time_i = timeCol_i.getColumn();
					Vector<Double> interval_i;
					if (columnOk(intervalCol_i)) interval_i = intervalCol_i.getColumn();

					Array<Float> switchedDiff_i;
					if (columnOk(switchedDiffCol_i)) switchedDiff_i = switchedDiffCol_i.getColumn();
					Array<Float> switchedSum_i;
					if (columnOk(switchedSumCol_i)) switchedSum_i = switchedSumCol_i.getColumn();
					Array<Float> requantizerGain_i;
					if (columnOk(requantizerGainCol_i)) requantizerGain_i = requantizerGainCol_i.getColumn();

					// Add n# rows to subtable_i equivalent to n# rows from subtable_0
					subtable_0.addRow(subtable_i.nrow());

		        	// Prepare row reference object
		        	RefRows refRow(rowIndex,rowIndex+subtable_i.nrow()-1);

		        	// Re-index SPW col
		        	Vector<Int> spectralWindowId_output(spectralWindowId_i.size(),mapSubmsSpwid[subms_index]);
		        	spectralWindowId_output += spectralWindowId_i;
		        	spectralWindowIdCol_0.putColumnCells(refRow,spectralWindowId_output);

		        	// Columns that can be just copied
		        	if (columnOk(antennaIdCol_i))
					{
						antennaIdCol_0.putColumnCells(refRow,antennaId_i);
					}

		        	if (columnOk(feedIdCol_i))
					{
						feedIdCol_0.putColumnCells(refRow,feedId_i);
					}

		        	if (columnOk(timeCol_i))
					{
						timeCol_0.putColumnCells(refRow,time_i);
					}

		        	if (columnOk(intervalCol_i))
					{
						intervalCol_0.putColumnCells(refRow,interval_i);
					}


		        	if (columnOk(switchedDiffCol_i))
					{
						switchedDiffCol_0.putColumnCells(refRow,switchedDiff_i);
					}

		        	if (columnOk(switchedSumCol_i))
					{
						switchedSumCol_0.putColumnCells(refRow,switchedSum_i);
					}

		        	if (columnOk(requantizerGainCol_i))
					{
						requantizerGainCol_0.putColumnCells(refRow,requantizerGain_i);
					}

		        	// Increment row offset
		        	rowIndex += subtable_i.nrow();
				}
			}

		}
		else
		{
	    	os << LogIO::DEBUG1 << LogOrigin("MSTransformDataHandler", __FUNCTION__)
	    			<< "SYSPOWER sub-table found but has no valid content" << LogIO::POST;
	    	return false;
		}
	}
	else
	{
		os << LogIO::DEBUG1 << LogOrigin("MSTransformDataHandler", __FUNCTION__)
    			<< "SYSPOWER sub-table not found " << LogIO::POST;
    	return false;
	}

	return true;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
// template <class T>  Bool MSTransformDataHandler::columnOk (ArrayColumn<T> column)
// {
// 	Bool ret;
// 	if (column.isNull()==false and column.hasContent()==true and column.ndimColumn() > 0)
// 	{
// 		ret = true;
// 	}
// 	else
// 	{
// 		ret = false;
// 	}

// 	return ret;
// }

// // -----------------------------------------------------------------------
// //
// // -----------------------------------------------------------------------
// template <class T>  Bool MSTransformDataHandler::columnOk (ScalarColumn<T> column)
// {
// 	Bool ret;
// 	if (column.isNull()==false and column.hasContent()==true)
// 	{
// 		ret = true;
// 	}
// 	else
// 	{
// 		ret = false;
// 	}

// 	return ret;
// }


// -----------------------------------------------------------------------
// Work-around to copy the keywords of the FLOAT_DATA column to the output MS
// -----------------------------------------------------------------------
void MSTransformDataHandler::copyMainTableKeywords (TableRecord& outKeys,
		const TableRecord& inKeys)
{
	for (uInt i=0; i<inKeys.nfields(); i++) {
		if (inKeys.type(i) == TpString) {
			// Add keywords for MAIN table columns such as FLOAT_DATA
			String ikey = inKeys.name(i);
			if (!outKeys.isDefined (ikey)) {
				String keyval;
				inKeys.get(ikey, keyval);
				outKeys.define(ikey,keyval);
			}

		}

	}
}


} //# NAMESPACE CASA - END
