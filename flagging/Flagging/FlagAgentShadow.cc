//# FlagAgentShadow.cc: This file contains the implementation of the FlagAgentShadow class.
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

#include <flagging/Flagging/FlagAgentShadow.h>

#include <measures/Measures/MBaseline.h>
#include <measures/Measures/MCBaseline.h>
#include <measures/Measures/MCDirection.h>
#include <measures/Measures/MCuvw.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MEpoch.h>
#include <measures/Measures/Muvw.h>
#include <casacore/measures/TableMeasures/ScalarMeasColumn.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

// Definition of static members for common pre-processing
vector<Int> FlagAgentShadow::shadowedAntennas_p;
casa::async::Mutex FlagAgentShadow::staticMembersMutex_p;
vector<bool> FlagAgentShadow::startedProcessing_p;
bool FlagAgentShadow::preProcessingDone_p = false;
uShort FlagAgentShadow::nAgents_p = 0;

FlagAgentShadow::FlagAgentShadow(FlagDataHandler *dh, Record config, Bool writePrivateFlagCube, Bool flag):
				FlagAgentBase(dh,config,ROWS_PREPROCESS_BUFFER,writePrivateFlagCube,flag)
{
	setAgentParameters(config);

	// Set preProcessingDone_p static member to false
	preProcessingDone_p = false;

	// Request loading antenna1,antenna2 and uvw
	flagDataHandler_p->preLoadColumn(VisBufferComponent2::Antenna1);
	flagDataHandler_p->preLoadColumn(VisBufferComponent2::Antenna2);
	flagDataHandler_p->preLoadColumn(VisBufferComponent2::Uvw);
	/////flagDataHandler_p->preLoadColumn(VisBufferComponent2::Time);
	flagDataHandler_p->preLoadColumn(VisBufferComponent2::TimeCentroid);
	flagDataHandler_p->preLoadColumn(VisBufferComponent2::PhaseCenter);
	/////flagDataHandler_p->preLoadColumn(vi::Direction1);

	// FlagAgentShadow counters and ids to handle static variables
	staticMembersMutex_p.acquirelock();
	agentNumber_p = nAgents_p;
	nAgents_p += 1;
	staticMembersMutex_p.unlock();

	// Set timekeeper to zero - this will later detect when the timestep changes.
	currTime_p=0.0;

	// Append the supplied additional antennas to COPIES of existing base-class lists.

	// Append to existing lists of antenna info.
	Int nAntsInMS = flagDataHandler_p->antennaNames_p->nelements();
	Int nNewAnts=0;


	//  antennaNames_p
	// antennaDiameters_p
	// antennaPositions_p
	if( additionalAntennas_p.nfields() )
	{

		// For debugging...
		//ostringstream recprint;
		//additionalAntennas_p.print(recprint);
		//cout << " Additional Antennas : " << recprint.str() << endl;

		// TODO : Verify input Record. If invalid, print warning and proceed with no extra antennas.
		Bool validants=true;
		String errorReason;
		for(Int anew=0; anew<(Int) additionalAntennas_p.nfields(); anew++)
		{
			// Extract the record.
			Record arec = additionalAntennas_p.subRecord(RecordFieldId(String::toString(anew)));

			if( ! arec.isDefined("diameter") ||
					( arec.type(arec.fieldNumber("diameter")) != casacore::TpFloat &&
							arec.type(arec.fieldNumber("diameter")) != casacore::TpInt &&
							arec.type(arec.fieldNumber("diameter")) != casacore::TpDouble  ) )
			{
				validants=false;
				errorReason += String("Input Record [") + String::toString(anew) + ("] needs a field 'diameter' of type <double> \n");
			}

			if( ! arec.isDefined("position") ||
					( arec.type(arec.fieldNumber("position")) != casacore::TpArrayFloat &&
							arec.type(arec.fieldNumber("position")) != casacore::TpInt &&
							arec.type(arec.fieldNumber("position")) != casacore::TpArrayDouble  ) )
			{
				validants=false;
				errorReason += String("Input Record [") + String::toString(anew) + ("] needs a field 'position' of type Array<double>\n");
			}
			else
			{
				Array<Double> tpos;
				arec.get( RecordFieldId(String("position")) , tpos );
				if(tpos.shape() != IPosition(1,3))
				{
					validants=false;
					errorReason += String("'position' for Record [") + String::toString(anew)+ ("] must be a vector of 3 floats or doubles\n");
				}
			}

		}// end of valid-ants loop

		// If antenna list is valid, set the number of new antennas to add.
		if(validants)
		{
			nNewAnts = additionalAntennas_p.nfields();
		}
		else // warn and continue.
		{
			*logger_p << LogIO::WARN << "NOT using additional antennas for shadow calculations, for the following reason(s) : " << errorReason << LogIO::POST;
		}
	}// if additionalAnts exist.


	// Make holders for cumulative information
	shadowAntennaPositions_p.resize(nAntsInMS+nNewAnts);
	///        shadowAntennaNames_p.resize(nAntsInMS+nNewAnts);
	shadowAntennaDiameters_p.resize(nAntsInMS+nNewAnts);

	// Copy existing antennas into these arrays
	for(Int antid=0;antid<nAntsInMS;antid++)
	{
		shadowAntennaPositions_p[antid] = flagDataHandler_p->antennaPositions_p->operator()(antid);
		///shadowAntennaNames_p[antid] = flagDataHandler_p->antennaNames_p->operator()(antid);
		shadowAntennaDiameters_p[antid] = flagDataHandler_p->antennaDiameters_p->operator()(antid);
	}

	// If any additional antennas are given, and are valid, add them to the lists
	for(Int antid=0;antid<nNewAnts;antid++)
	{
		// Extract the record.
		Record arec = additionalAntennas_p.subRecord(RecordFieldId(String::toString(antid)));

		// Extract and add new positions
		Array<Double> aposarr;
		arec.get( RecordFieldId(String("position")) , aposarr );
		Vector<Double> aposvec(aposarr);
		MVPosition apos(aposvec(0),aposvec(1),aposvec(2));
		shadowAntennaPositions_p[nAntsInMS+antid] = MPosition(apos,MPosition::Types(MPosition::ITRF));

		// Extract and add new diameters
		Double adia;
		arec.get( RecordFieldId(String("diameter")) , adia );
		shadowAntennaDiameters_p[nAntsInMS+antid] = adia;

		// Extract and add new names
		///String aname aname;
		///arec.get( RecordFieldId(String("name")) , aname );
		///shadowAntennaNames_p[nAntsInMS+antid] = aname;

	}


	firststep_p=false; // Set to true, to print a debug message (antenna uvw coordinates for the first row in the first visbuffer seen by this code...

}// end of constructor

FlagAgentShadow::~FlagAgentShadow()
{
	// Compiler automagically calls FlagAgentBase::~FlagAgentBase()

	// NOTE: The following is necessary because the static variables
	// persist even if all the instances of the class were deleted!
	staticMembersMutex_p.acquirelock();
	agentNumber_p = nAgents_p;
	nAgents_p -= 1;
	staticMembersMutex_p.unlock();
}

void
FlagAgentShadow::setAgentParameters(Record config)
{
	logger_p->origin(LogOrigin(agentName_p,__FUNCTION__,WHERE));
	int exists;

	// Amount of shadowing to allow. Float or Double, in units of Meters.
	exists = config.fieldNumber ("tolerance");
	if (exists >= 0)
	{
		if( config.type(exists) != casacore::TpDouble && config.type(exists) != casacore::TpFloat && config.type(exists) != casacore::TpInt )
		{
			throw( AipsError ( "Parameter 'tolerance' must be of type 'double'" ) );
		}

		shadowTolerance_p = config.asDouble("tolerance");
	}
	else
	{
		shadowTolerance_p = 0.0;
	}

	*logger_p << logLevel_p << " tolerance is " << shadowTolerance_p << " meters "<< LogIO::POST;

	// A list of antenna parameters, to add to those in the antenna subtable, to calculate shadows.
	exists = config.fieldNumber ("addantenna");
	if (exists >= 0)
	{
		if( config.type(exists) != casacore::TpRecord )
		{
			throw( AipsError ( "Parameter 'addantenna' must be of type 'record/dict'" ) );
		}

		additionalAntennas_p = config.subRecord( RecordFieldId("addantenna") );
	}
	else
	{
		additionalAntennas_p = Record();
	}

	ostringstream recprint;
	additionalAntennas_p.print(recprint);
	*logger_p << logLevel_p << " addantenna is " << recprint.str() << LogIO::POST;

	return;
}

void
FlagAgentShadow::preProcessBuffer(const vi::VisBuffer2 &visBuffer)
{
	if (nAgents_p > 1)
	{
		staticMembersMutex_p.acquirelock();

		if (!preProcessingDone_p)
		{
			// Reset processing state variables
			if (startedProcessing_p.size() != nAgents_p) startedProcessing_p.resize(nAgents_p,false);
			for (vector<bool>::iterator iter = startedProcessing_p.begin();iter != startedProcessing_p.end();iter++)
			{
				*iter = false;
			}

			// Do actual pre-processing
			preProcessBufferCore(visBuffer);

			// Mark pre-processing as done so that other agents don't redo it
			preProcessingDone_p = true;
		}

		staticMembersMutex_p.unlock();
	}
	else
	{
		preProcessBufferCore(visBuffer);
	}

	return;
}

void
FlagAgentShadow::preProcessBufferCore(const vi::VisBuffer2 &/*visBuffer*/)
{
	// This function is empty, because shadowedAntennas_p needs to be re-calculated for
	// every new timestep, and it is done inside computeRowFlags(), whenever the
	// timestep changes.
}

// (1) Go through all listed baselines for the current timestep, use existing uvw values to
//      check for shadowing.
// (2) If not ALL baselines exist in the current timestep, or if additional antennas have been
//      supplied, calculate u,v,w, for all antennas, and from there, uvw for all remaining baselines
//      and check for shadows between them too.
//      Note : The calculation of UVW happens per antenna, not baselines. This is an optimization.
//      Note : The direction used for UVW re-calculation is the phasecenter, and not the pointing
//                direction of each antenna. This was done to prevent a performance hit due to
//                accessing vb.direction1() which accesses MS derived columns, which is also thread-unsafe.
//                The only situation where phasecenter is inaccurate, is on-the-fly mosaicing, but
//                unless one is doing an on-the-fly mosaic of the whole sky, using a single phase-center (!!!)
//                this will not adversely affect shadow flags.
void FlagAgentShadow::calculateShadowedAntennas(const vi::VisBuffer2 &visBuffer, Int rownr)
{
	// Init the list of antennas.
	shadowedAntennas_p.clear();
	Double u,v,w, uvDistance;
	Int nAnt = shadowAntennaDiameters_p.nelements();

	// Init the list of baselines, to later decide which to read and which to recalculate.
	Vector<Bool> listBaselines(nAnt*(nAnt-1)/2);
	listBaselines = false;

	/*
	///
	/// Commenting out this section, to force recalculation of UVWs for all baselines
	///
	// We know the starting row for this timestep. Find the ending row.
	// This assumes that all baselines are grouped together.
	// This is guaranteed by the sort-order defined for the visIterator.
	Int endrownr = rownr;
	Double timeval = visBuffer.timeCentroid()(rownr) ;
	for (Int row_i=rownr;row_i<visBuffer.nRows();row_i++)
	{
		if(timeval < visBuffer.timeCentroid()(row_i)) // we have touched the next timestep
		{
			endrownr = row_i-1;
			break;
		}
		else
		{
			endrownr = row_i;
		}
	}


//	Int antenna1, antenna2;

	// (1) Now, for all rows between 'rownr' and 'endrownr', calculate shadowed Ants.
	// This row range represents all listed baselines in the "current" timestep.
    for (Int row_i=rownr;row_i<=endrownr;row_i++)
      {
	// Retrieve antenna ids
	antenna1 = visBuffer.antenna1()(row_i);
	antenna2 = visBuffer.antenna2()(row_i);

	// Check if this row corresponds to autocorrelation (Antennas don't shadow themselves)
	if (antenna1 == antenna2) continue;

	// Record the baseline being processed
	listBaselines[baselineIndex(nAnt,antenna1,antenna2)] = true;

	// Compute uv distance
	u = visBuffer.uvw()(0,row_i);
	v = visBuffer.uvw()(1,row_i);
	w = visBuffer.uvw()(2,row_i);
	uvDistance = sqrt(u*u + v*v);

	decideBaselineShadow(uvDistance, w, antenna1, antenna2);

      }// end of for 'row'
	 */


	// (2) Now, if there are any untouched baselines, calculate 'uvw' for all antennas,
	//      and fill in missing baselines.
	// This is the part that picks up invisible antennas, whether they come from the antenna_subtable or
	// are externally supplied.
	if(product(listBaselines)==false)
	{
		// For the current timestep, compute UVWs for all antennas.
		//    uvwAnt_p will be filled these values.
		computeAntUVW(visBuffer, rownr);

		// For all untouched baselines, calculate uvw and check for shadows.
		for (Int antenna1=0; antenna1<nAnt; antenna1++)
		{
			Double u1=uvwAnt_p(0,antenna1), v1=uvwAnt_p(1,antenna1), w1=uvwAnt_p(2,antenna1);
			for (Int antenna2=antenna1; antenna2<nAnt; antenna2++)
			{
				// Check if this row corresponds to autocorrelation (Antennas don't shadow themselves)
				if (antenna1 == antenna2) continue;

				// Proceed only if we don't already have this.
				if(listBaselines[baselineIndex(nAnt,antenna1,antenna2)] == false)
				{
					Double u2=uvwAnt_p(0,antenna2), v2=uvwAnt_p(1,antenna2), w2=uvwAnt_p(2,antenna2);

					u = u2-u1;
					v = v2-v1;
					w = w2-w1;
					uvDistance = sqrt(u*u + v*v);

					if(firststep_p==true) // this is only a debug message here....
					{
						cout << "Ant1 : " << antenna1 << " : " << u1 << "," << v1 << "," << w1 << "      Ant2 : " << antenna2 << " : "  << u2 << "," << v2<< "," << w2 << "      UVW : " << u << "," << v << "," << w << endl;
					}

					decideBaselineShadow(uvDistance, w, antenna1, antenna2);
				}
			}
		}
	}

	firststep_p=false;// debug message should happen only once (at most).

}// end of calculateShadowedAntennas

uInt FlagAgentShadow::baselineIndex(uInt nAnt, uInt a1, uInt a2)
{
	uInt bindex = (nAnt-1)*nAnt/2 - ((nAnt-1)-a1)*((nAnt-1)-a1+1)/2 + a2-a1-1 ;
	AlwaysAssert( bindex < nAnt*(nAnt-1)/2 ,AipsError);
	return bindex;
}


void FlagAgentShadow::decideBaselineShadow(Double uvDistance, Double w, Int antenna1, Int antenna2)
{
	Double antennaDiameter1,antennaDiameter2, antennaDistance;

	// Get antenna diameter
	antennaDiameter1 = shadowAntennaDiameters_p[antenna1];
	antennaDiameter2 = shadowAntennaDiameters_p[antenna2];

	//  Compute effective distance for shadowing
	antennaDistance = (antennaDiameter1+antennaDiameter2)/2.0;

	// Check if one of the antennas can be shadowed
	if (uvDistance < antennaDistance - shadowTolerance_p)
	{
		///////////////////////////////////////////////////////
		// Conventions.
		// (A) For a Right Handed coordinate system, with 'w' pointing towards the source....
		//       ( as defined here : http://casa.nrao.edu/Memos/229.html#SECTION00041000000000000000 )
		//     if(w>0) antenna1 is shadowed by antenna2
		//     if(w<0) antenna2 is shadowed by antenna1
		//
		//     This is implemented in casapy 3.4 and 4.0
		//
		// (B) For a Left Handed Coordinate system, with 'w' pointing away from the source...
		//      ( You get B by flipping the sign on all three axes (u,v,w) of A ).
		//      This is what is present in the data (i.e. filler, simulator, (our use of Measures?)).
		//     if(w<0) antenna1 is shadowed by antenna2
		//     if(w>0) antenna2 is shadowed by antenna1
		//
		//     This is implemented in casapy 4.1 ( from 1 Feb 2013 onwards ).
		//
		///////////////////////////////////////////////////////

		//	if (w>0)  ////// as in casapy 3.4 and casapy 4.0
		if (w<0) ////// casapy 4.1 onwards.
		{
			if (std::find (shadowedAntennas_p.begin(), shadowedAntennas_p.end(), antenna1) == shadowedAntennas_p.end())
			{
				shadowedAntennas_p.push_back(antenna1);
			}
		}
		else
		{
			if (std::find (shadowedAntennas_p.begin(), shadowedAntennas_p.end(), antenna2) == shadowedAntennas_p.end())
			{
				shadowedAntennas_p.push_back(antenna2);
			}
		}
	}
}

/// NOTE : This function is almost a copy of
///  ms/MeasurementSets/NewMSSimulator::calcAntUVW
///  -- TODO : try to re-use that code by moving out all private-member accesses in the simulator.
///  -- TOCHECK : Should we use vb.timeCentroid() ??  This gives closest results so far, for real and simulated data.
/// NOTE : We are using vb.phasecenter() instead of vb.direction() because of a performance hit
///            and thread-safety problems with vb.direction1().
Bool FlagAgentShadow::computeAntUVW(const vi::VisBuffer2 &vb, Int rownr)
{
	// Get time and timeinterval from the visbuffer.
	Double Time;

	// Centroid gives the closest values to uvws in the MS. For simulated data, gives exact values.
	Time = vb.timeCentroid()(rownr);

	// Make the Time epoch.
	MEpoch epoch(Quantity((Time), "s"), MEpoch::UT1);

	// Get the MDirection of the feed of antenna 1. Assume all ants point in the same direction.
	//MDirection refdir(vb.direction1()(rownr));
	MDirection refdir(vb.phaseCenter());    // Each visbuf sees only one fieldId

	// read position of first antenna as reference. Does not matter, since uvws are only differences.
	MPosition obsPos( shadowAntennaPositions_p[0] );

	// Input measure ref. frame
	MVPosition basePos=obsPos.getValue();
	MeasFrame measFrame(obsPos);
	measFrame.set(epoch);
	measFrame.set(refdir);

    // Convert direction to J2000
	MDirection::Convert covertionEngine(refdir,MDirection::Ref(MDirection::J2000,measFrame));    // Each visbuf sees only one fieldId
    MDirection refdirJ2000 =  covertionEngine(refdir);

	// Baseline measure
	MVBaseline mvbl;
	MBaseline basMeas;
	MBaseline::Ref basref(MBaseline::ITRF, measFrame);
	basMeas.set(mvbl, basref);
	basMeas.getRefPtr()->set(measFrame);

	// going to convert from ITRF vector to J2000 baseline vector I guess !
	if(refdirJ2000.getRef().getType() != MDirection::J2000)
		throw(AipsError("Internal FlagAgentShadow restriction : Conversion to J2000 did not work"));

	Int nAnt = shadowAntennaDiameters_p.nelements();
	if(uvwAnt_p.shape() != IPosition(2,3,nAnt))
	{
		uvwAnt_p.resize(3,nAnt);
	}

	MBaseline::Convert elconv(basMeas, MBaseline::Ref(MBaseline::J2000));
	Muvw::Convert uvwconv(Muvw(), Muvw::Ref(Muvw::J2000, measFrame));
	for(Int k=0; k< nAnt; ++k)
	{
		MPosition antpos=shadowAntennaPositions_p(k);   // msc.antenna().positionMeas()(k);

		MVBaseline mvblA(obsPos.getValue(), antpos.getValue());
		basMeas.set(mvblA, basref);
		MBaseline bas2000 =  elconv(basMeas);
		MVuvw uvw2000 (bas2000.getValue(), refdirJ2000.getValue());
		const Vector<double>& xyz = uvw2000.getValue();
		uvwAnt_p.column(k)=xyz;
	}

	return true;
}


bool
FlagAgentShadow::computeRowFlags(const vi::VisBuffer2 &visBuffer, FlagMapper &/*flags*/, uInt row)
{
	// If we have advanced to a new timestep, calculate new antenna UVW values and shadowed antennas
	// This function resets and fills 'shadowedAntennas_p'.
	if( currTime_p != visBuffer.timeCentroid()(row) )
	{
		currTime_p = visBuffer.timeCentroid()(row) ;
		calculateShadowedAntennas(visBuffer, row);
	}

	bool flagRow = false;
	// Flag row if either antenna1 or antenna2 are in the list of shadowed antennas
	Int antenna1 = visBuffer.antenna1()[row];
	Int antenna2 = visBuffer.antenna2()[row];
	if (	(std::find (shadowedAntennas_p.begin(), shadowedAntennas_p.end(), antenna1) != shadowedAntennas_p.end()) or
			(std::find (shadowedAntennas_p.begin(), shadowedAntennas_p.end(), antenna2) != shadowedAntennas_p.end()) )
	{
		flagRow = true;
	}

	if ((nAgents_p > 1) and preProcessingDone_p)
	{
		startedProcessing_p[agentNumber_p] = true;
		if (std::find (startedProcessing_p.begin(), startedProcessing_p.end(), false) == startedProcessing_p.end())
		{
			preProcessingDone_p = false;
		}
	}

	return flagRow;
}


#if 0

// // Copy of the old version of this function. It has code for "always recalculate UVW, never recalc UVW
// // and 'decide when to calc UVW'.  Above, only the 'decide when to calc UVW' part is used.
// void FlagAgentShadow::calculateShadowedAntennas(const VisBuffer &visBuffer, Int rownr)
// {
//   // Init the list of antennas.
//   shadowedAntennas_p.clear();
//   Double u,v,w, uvDistance;
//   Int nAnt = shadowAntennaDiameters_p.nelements();

//   // Init the list of baselines, to later decide which to read and which to recalculate.
//   Vector<Bool> listBaselines(nAnt*(nAnt-1)/2);
//   listBaselines = false;

//   //uInt countread=0;
//   //uInt countcalc=0;
//   //Double reftime = 4.794e+09;

//   if (decideUVW_p==true)
//     {
//       // We know the starting row for this timestep. Find the ending row.
//       // This assumes that all baselines are grouped together.
// 	// This is guaranteed by the sort-order defined for the visIterator.
//       Int endrownr = rownr;
// 	Double timeval = visBuffer.timeCentroid()(rownr) ;
// 	for (Int row_i=rownr;row_i<visBuffer.nRow();row_i++)
// 	  {
// 	    if(timeval < visBuffer.timeCentroid()(row_i)) // we have touched the next timestep
// 	      {
// 		endrownr = row_i-1;
// 		break;
// 	      }
// 	    else
// 	      {
// 		endrownr = row_i;
// 	      }
// 	  }

// 	//cout << "For time : " << timeval-4.73423e+09 << " start : " << rownr << " end : " << endrownr << endl;

// 	// Now, for all rows between 'rownr' and 'endrownr', calculate shadowed Ants.
//       // This row range represents all listed baselines in the "current" timestep.
// 	Int antenna1, antenna2;
// 	for (Int row_i=rownr;row_i<=endrownr;row_i++)
// 	  {
// 	    // Retrieve antenna ids
// 	    antenna1 = visBuffer.antenna1()(row_i);
// 	    antenna2 = visBuffer.antenna2()(row_i);

// 	    // Check if this row corresponds to autocorrelation (Antennas don't shadow themselves)
// 	    if (antenna1 == antenna2) continue;

// 	    // Record the baseline being processed
// 	    listBaselines[baselineIndex(nAnt,antenna1,antenna2)] = true;

// 	    // Compute uv distance
// 	    u = visBuffer.uvw()(row_i)(0);
// 	    v = visBuffer.uvw()(row_i)(1);
// 	    w = visBuffer.uvw()(row_i)(2);
// 	    uvDistance = sqrt(u*u + v*v);

//           //if(row_i==0 && rownr==0) cout << " Row : " << row_i << "   uvdist : " << uvDistance << " w : " << w << " time-x : " << visBuffer.timeCentroid()(row_i)-reftime << endl;

// 	    decideBaselineShadow(uvDistance, w, antenna1, antenna2);
// 	    //countread++;

// 	  }// end of for 'row'

// 	// Now, if there are any untouched baselines, calculate 'uvw' for all antennas, and fill in missing baselines.
// 	if(product(listBaselines)==false)
// 	  {
// 	    // For the current timestep, compute UVWs for all antennas.
// 	    //    uvwAnt_p will be filled these values.
// 	    computeAntUVW(visBuffer, rownr);

// 	    for (Int antenna1=0; antenna1<nAnt; antenna1++)
// 	      {
// 		Double u1=uvwAnt_p(0,antenna1), v1=uvwAnt_p(1,antenna1), w1=uvwAnt_p(2,antenna1);
// 		for (Int antenna2=antenna1; antenna2<nAnt; antenna2++)
// 		  {
// 		    // Check if this row corresponds to autocorrelation (Antennas don't shadow themselves)
// 		    if (antenna1 == antenna2) continue;

// 		    if(listBaselines[baselineIndex(nAnt,antenna1,antenna2)] == false)
// 		      {
// 			Double u2=uvwAnt_p(0,antenna2), v2=uvwAnt_p(1,antenna2), w2=uvwAnt_p(2,antenna2);

// 			u = u2-u1;
// 			v = v2-v1;
// 			w = w2-w1;
// 			uvDistance = sqrt(u*u + v*v);
// 			//countcalc++;

// 			//if(rownr==0 && antenna1==tant1 && antenna2==tant2) cout << " (r)Row : " << rownr << "   uvdist : " << uvDistance << "  w : " << w << " time-x : " << visBuffer.timeCentroid()(rownr)-reftime << endl;

// 			decideBaselineShadow(uvDistance, w, antenna1, antenna2);

// 			listBaselines[baselineIndex(nAnt,antenna1,antenna2)] = true;
// 		      }
// 		  }
// 	      }
// 	  }

//     }
//   else if(recalculateUVW_p)
//     {
// 	//  (1) For the current timestep, compute UVWs for all antennas.
// 	//    uvwAnt_p will be filled these values.
// 	computeAntUVW(visBuffer, rownr);

// 	// debug code.
// 	// Int   tant1 = visBuffer.antenna1()(rownr);
// 	// Int   tant2 = visBuffer.antenna2()(rownr);

// 	//  (2) For all antenna pairs, calculate UVW of the baselines, and check for shadowing.
// 	for (Int antenna1=0; antenna1<nAnt; antenna1++)
// 	  {
// 	    Double u1=uvwAnt_p(0,antenna1), v1=uvwAnt_p(1,antenna1), w1=uvwAnt_p(2,antenna1);
// 	    for (Int antenna2=antenna1; antenna2<nAnt; antenna2++)
// 	      {
// 		// Check if this row corresponds to autocorrelation (Antennas don't shadow themselves)
// 		if (antenna1 == antenna2) continue;

// 		Double u2=uvwAnt_p(0,antenna2), v2=uvwAnt_p(1,antenna2), w2=uvwAnt_p(2,antenna2);

// 		u = u2-u1;
// 		v = v2-v1;
// 		w = w2-w1;
// 		uvDistance = sqrt(u*u + v*v);
// 		//countcalc++;

//               //if(rownr==0 && antenna1==tant1 && antenna2==tant2) cout << " (r)Row : " << rownr << "   uvdist : " << uvDistance << "  w : " << w << " time-x : " << visBuffer.timeCentroid()(rownr)-reftime << endl;

// 		decideBaselineShadow(uvDistance, w, antenna1, antenna2);

// 	      }// end for antenna2
// 	  }// end for antenna1

//     }// end of recalculateUVW_p==true
//   else // recalculateUVW_p = false
//     {

//       // We know the starting row for this timestep. Find the ending row.
//       // This assumes that all baselines are grouped together.
// 	// This is guaranteed by the sort-order defined for the visIterator.
//       Int endrownr = rownr;
// 	Double timeval = visBuffer.timeCentroid()(rownr) ;
// 	for (Int row_i=rownr;row_i<visBuffer.nRow();row_i++)
// 	  {
// 	    if(timeval < visBuffer.timeCentroid()(row_i)) // we have touched the next timestep
// 	      {
// 		endrownr = row_i-1;
// 		break;
// 	      }
// 	    else
// 	      {
// 		endrownr = row_i;
// 	      }
// 	  }

// 	//cout << "For time : " << timeval-4.73423e+09 << " start : " << rownr << " end : " << endrownr << endl;

// 	// Now, for all rows between 'rownr' and 'endrownr', calculate shadowed Ants.
//       // This row range represents all baselines in the "current" timestep.
// 	Int antenna1, antenna2;
// 	for (Int row_i=rownr;row_i<=endrownr;row_i++)
// 	  {
// 	    // Retrieve antenna ids
// 	    antenna1 = visBuffer.antenna1()(row_i);
// 	    antenna2 = visBuffer.antenna2()(row_i);

// 	    // Check if this row corresponds to autocorrelation (Antennas don't shadow themselves)
// 	    if (antenna1 == antenna2) continue;

// 	    // Compute uv distance
// 	    u = visBuffer.uvw()(row_i)(0);
// 	    v = visBuffer.uvw()(row_i)(1);
// 	    w = visBuffer.uvw()(row_i)(2);
// 	    uvDistance = sqrt(u*u + v*v);
// 	    //countread++;

//           //if(row_i==0 && rownr==0) cout << " Row : " << row_i << "   uvdist : " << uvDistance << " w : " << w << " time-x : " << visBuffer.timeCentroid()(row_i)-reftime << endl;

// 	    decideBaselineShadow(uvDistance, w, antenna1, antenna2);

// 	  }// end of for 'row'
//     }

//   //cout << "Row : " << rownr << "  read : " << countread << "   calc : " << countcalc << endl;

// }// end of calculateShadowedAntennas


#endif


} //# NAMESPACE CASA - END


