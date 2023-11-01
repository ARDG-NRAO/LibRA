//# MSTransformRegridder.cc: This file contains the implementation of the MSTransformRegridder class.
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

#include <mstransform/MSTransform/MSTransformRegridder.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

/////////////////////////////////////////////
/// MSTransformRegridder implementation ///
/////////////////////////////////////////////

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
MSTransformRegridder::MSTransformRegridder()
{
	return;
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
MSTransformRegridder::~MSTransformRegridder()
{
	return;
}


// -----------------------------------------------------------------------
// Make one spectral window from all SPWs given by the SPW Ids vector
// -----------------------------------------------------------------------
Bool MSTransformRegridder::combineSpws(	LogIO& os,
										String msName,
										const Vector<Int>& spwids,
										Vector<Double>& newCHAN_FREQ,
										Vector<Double>& newCHAN_WIDTH,
										vector<vector<Int> >& averageWhichChan,
										vector<vector<Int> >& averageWhichSPW,
										vector<vector<Double> >& averageChanFrac,
										Bool verbose)
{
	MeasurementSet ms_p(msName, Table::Old);
	Bool result = false;
	result = combineSpwsCore(os,ms_p,spwids,newCHAN_FREQ,newCHAN_WIDTH,
							 averageWhichChan, averageWhichSPW, averageChanFrac, verbose);
	return result;
}

// -----------------------------------------------------------------------
// Make one spectral window from all SPWs given by the SPW Ids vector
// -----------------------------------------------------------------------
Bool MSTransformRegridder::combineSpwsCore(	LogIO& os,
											MeasurementSet& ms_p,
											const Vector<Int>& spwids,
											Vector<Double>& newCHAN_FREQ,
											Vector<Double>& newCHAN_WIDTH,
											vector<vector<Int> >& averageWhichChan,
											vector<vector<Int> >& averageWhichSPW,
											vector<vector<Double> >& averageChanFrac,
											Bool verbose)
{
	// Analyze SPW Ids
	if (spwids.nelements() == 0)
	{
		os << LogIO::WARN << LogOrigin("MSTransformRegridder", __FUNCTION__)
				<< "No SPWs selected for combination ..." << LogIO::POST;
		return true;
	}

	// Find all existing spws,
	MSSpectralWindow spwtable = ms_p.spectralWindow();
	Int origNumSPWs = spwtable.nrow();

	vector<Int> spwsToCombine;
	Vector<Bool> includeIt(origNumSPWs, false);

	// jagonzal: This covers for the case when we want to combine all the input SPWs
	if (spwids(0) == -1)
	{
		for (Int i = 0; i < origNumSPWs; i++)
		{
			spwsToCombine.push_back(i);
			includeIt(i) = true;
		}
	}
	// jagonzal: Nominal case when we want to combine a sub-set of the input SPWs
	else
	{
		for (uInt i = 0; i < spwids.nelements(); i++)
		{
			if (spwids(i) < origNumSPWs && spwids(i) >= 0)
			{
				spwsToCombine.push_back(spwids(i));
				includeIt(spwids(i)) = true;
			}
			else
			{
				os	<< LogIO::SEVERE << LogOrigin("MSTransformRegridder", __FUNCTION__)
					<< "Invalid SPW ID selected for combination "
					<< spwids(i) << "valid range is 0 - "
					<< origNumSPWs - 1 << ")" << LogIO::POST;
				return false;
			}
		}
	}

	// jagonzal: Marginal case when there is no actual SPW combination
	if (spwsToCombine.size() <= 1)
	{
		if (verbose)
		{
			os 	<< LogIO::NORMAL << LogOrigin("MSTransformRegridder", __FUNCTION__)
				<< "Less than two SPWs selected. No combination necessary."
				<< LogIO::POST;
		}
		return true;
	}

	// Sort the SPW Ids
	std::sort(spwsToCombine.begin(), spwsToCombine.end());

	uInt nSpwsToCombine = spwsToCombine.size();

	// Prepare access to the SPW table
	MSSpWindowColumns SPWColrs(spwtable);
	ScalarColumn<Int> numChanColr = SPWColrs.numChan();
	ArrayColumn<Double> chanFreqColr = SPWColrs.chanFreq();
	ArrayColumn<Double> chanWidthColr = SPWColrs.chanWidth();
	ScalarColumn<Int> measFreqRefColr = SPWColrs.measFreqRef();
	ArrayColumn<Double> effectiveBWColr = SPWColrs.effectiveBW();
	ScalarColumn<Double> refFrequencyColr = SPWColrs.refFrequency();
	ArrayColumn<Double> resolutionColr = SPWColrs.resolution();
	ScalarColumn<Double> totalBandwidthColr = SPWColrs.totalBandwidth();

	// Create a list of the SPW Ids sorted by first (lowest) channel frequency
	vector<Int> spwsSorted(nSpwsToCombine);
	Vector<Bool> isDescending(origNumSPWs, false);
	Bool negChanWidthWarned = false;


	Double* firstFreq = new Double[nSpwsToCombine];
	uInt count = 0;
	for (uInt i = 0; (Int) i < origNumSPWs; i++)
	{
		if (includeIt(i))
		{
			Vector<Double> CHAN_FREQ(chanFreqColr(i));

			// If frequencies are ascending, take the first channel, otherwise the last
			uInt nCh = CHAN_FREQ.nelements();
			if (CHAN_FREQ(0) <= CHAN_FREQ(nCh - 1))
			{
				firstFreq[count] = CHAN_FREQ(0);
			}
			else
			{
				firstFreq[count] = CHAN_FREQ(nCh - 1);
				isDescending(i) = true;
			}
			count++;
		}
	}

	Sort sort;
	sort.sortKey(firstFreq, TpDouble); // define sort key
	Vector<uInt> inx(nSpwsToCombine);
	sort.sort(inx, nSpwsToCombine);
	for (uInt i = 0; i < nSpwsToCombine; i++)
	{
		spwsSorted[i] = spwsToCombine[inx(i)];
	}
	delete[] firstFreq;


	Int id0 = spwsSorted[0];

	uInt newNUM_CHAN = numChanColr(id0);
	newCHAN_FREQ.assign(chanFreqColr(id0));
	newCHAN_WIDTH.assign(chanWidthColr(id0));
	Bool newIsDescending = isDescending(id0);
	{
		Bool negativeWidths = false;
		for (uInt i = 0; i < newNUM_CHAN; i++)
		{
			if (newCHAN_WIDTH(i) < 0.)
			{
				negativeWidths = true;
				newCHAN_WIDTH(i) = -newCHAN_WIDTH(i);
			}
		}
		if (negativeWidths && verbose)
		{
			os	<< LogIO::NORMAL << LogOrigin("MSTransformRegridder", __FUNCTION__)
				<< " *** Encountered negative channel widths in SPECTRAL_WINDOW table."
				<< LogIO::POST;
			negChanWidthWarned = true;
		}
	}

	// Need to reverse the order for processing
	if (newIsDescending)
	{
		Vector<Double> tempF, tempW;
		tempF.assign(newCHAN_FREQ);
		tempW.assign(newCHAN_WIDTH);
		for (uInt i = 0; i < newNUM_CHAN; i++)
		{
			newCHAN_FREQ(i) = tempF(newNUM_CHAN - 1 - i);
			newCHAN_WIDTH(i) = tempW(newNUM_CHAN - 1 - i);
		}
	}

	Vector<Double> newEFFECTIVE_BW(effectiveBWColr(id0));
	Int newMEAS_FREQ_REF = measFreqRefColr(id0);
	Vector<Double> newRESOLUTION(resolutionColr(id0));

	vector<Int> averageN; // For each new channel store the number of old channels to average over
	//vector<vector<Int> > averageWhichSPW; // For each new channel store the (old) SPWs to average over
	//vector<vector<Int> > averageWhichChan; // For each new channel store the channel numbers to av. over
	//vector<vector<Double> > averageChanFrac; // For each new channel store the channel fraction for each old channel

	// Initialize the averaging vectors
	for (uInt i = 0; i < newNUM_CHAN; i++)
	{
		averageN.push_back(1);
		vector<Int> tv; // Just a temporary auxiliary vector
		tv.push_back(id0);
		averageWhichSPW.push_back(tv);
		if (newIsDescending)
		{
			tv[0] = newNUM_CHAN - 1 - i;
		}
		else
		{
			tv[0] = i;
		}
		averageWhichChan.push_back(tv);
		vector<Double> tvd; // another one
		tvd.push_back(1.);
		averageChanFrac.push_back(tvd);
	}

	if (verbose)
	{
		os 	<< LogIO::NORMAL << LogOrigin("MSTransformRegridder", __FUNCTION__)
			<< "Input SPWs sorted by first (lowest) channel frequency:"
			<< LogIO::POST;

		ostringstream oss; // needed for iomanip functions
		oss	<< "   SPW " << std::setw(3) << id0 << ": " << std::setw(5)
			<< newNUM_CHAN << " channels, first channel = "
			<< std::setprecision(9) << std::setw(14) << std::scientific
			<< newCHAN_FREQ(0) << " Hz";
		if (newNUM_CHAN > 1)
		{
			oss << ", last channel = " << std::setprecision(9)
				<< std::setw(14) << std::scientific << newCHAN_FREQ(newNUM_CHAN - 1) << " Hz";
		}
		os << LogIO::NORMAL << LogOrigin("MSTransformRegridder", __FUNCTION__)
				<< oss.str() << LogIO::POST;
	}

	// Loop over remaining given SPWs
	for (uInt i = 1; i < nSpwsToCombine; i++)
	{
		Int idi = spwsSorted[i];

		uInt newNUM_CHANi = numChanColr(idi);
		Vector<Double> newCHAN_FREQi(chanFreqColr(idi));
		Vector<Double> newCHAN_WIDTHi(chanWidthColr(idi));
		Bool newIsDescendingI = isDescending(idi);
		{
			Bool negativeWidths = false;
			for (uInt ii = 0; ii < newNUM_CHANi; ii++)
			{
				if (newCHAN_WIDTHi(ii) < 0.)
				{
					negativeWidths = true;
					newCHAN_WIDTHi(ii) = -newCHAN_WIDTHi(ii);
				}
			}
			if (negativeWidths && !negChanWidthWarned && verbose)
			{
				os 	<< LogIO::NORMAL << LogOrigin("MSTransformRegridder", __FUNCTION__)
					<< " *** Encountered negative channel widths in SPECTRAL_WINDOW table."
					<< LogIO::POST;
				negChanWidthWarned = true;
			}
		}

		// need to reverse the order for processing
		if (newIsDescendingI)
		{
			Vector<Double> tempF, tempW;
			tempF.assign(newCHAN_FREQi);
			tempW.assign(newCHAN_WIDTHi);
			for (uInt ii = 0; ii < newNUM_CHANi; ii++)
			{
				newCHAN_FREQi(ii) = tempF(newNUM_CHANi - 1 - ii);
				newCHAN_WIDTHi(ii) = tempW(newNUM_CHANi - 1 - ii);
			}
		}

		Vector<Double> newEFFECTIVE_BWi(effectiveBWColr(idi));
		Int newMEAS_FREQ_REFi = measFreqRefColr(idi);
		Vector<Double> newRESOLUTIONi(resolutionColr(idi));

		if (verbose)
		{
			ostringstream oss;
			oss << "   SPW " << std::setw(3) << idi << ": " << std::setw(5)
				<< newNUM_CHANi << " channels, first channel = "
				<< std::setprecision(9) << std::setw(14)
				<< std::scientific << newCHAN_FREQi(0) << " Hz";

			if (newNUM_CHANi > 1)
			{
				oss << ", last channel = " << std::setprecision(9)
					<< std::setw(14) << std::scientific
					<< newCHAN_FREQi(newNUM_CHANi - 1) << " Hz";
			}
			os << LogIO::NORMAL << oss.str() << LogIO::POST;
		}

		vector<Double> mergedChanFreq;
		vector<Double> mergedChanWidth;
		vector<Double> mergedEffBW;
		vector<Double> mergedRes;
		vector<Int> mergedAverageN;
		vector<vector<Int> > mergedAverageWhichSPW;
		vector<vector<Int> > mergedAverageWhichChan;
		vector<vector<Double> > mergedAverageChanFrac;

		// check for compatibility
		if (newMEAS_FREQ_REFi != newMEAS_FREQ_REF)
		{
			os 	<< LogIO::WARN  << LogOrigin("MSTransformRegridder", __FUNCTION__)
				<< "SPW " << idi
				<< " cannot be combined with SPW " << id0
				<< ". Non-matching ref. frame." << LogIO::POST;
			return false;
		}

		// Append or prepend spw to new spw overlap at all?
		if (newCHAN_FREQ(newNUM_CHAN - 1) + newCHAN_WIDTH(newNUM_CHAN - 1)
				/ 2. < newCHAN_FREQi(0) - newCHAN_WIDTHi(0) / 2.)
		{
			// No overlap, and need to append
			for (uInt j = 0; j < newNUM_CHAN; j++)
			{
				mergedChanFreq.push_back(newCHAN_FREQ(j));
				mergedChanWidth.push_back(newCHAN_WIDTH(j));
				mergedEffBW.push_back(newEFFECTIVE_BW(j));
				mergedRes.push_back(newRESOLUTION(j));
				mergedAverageN.push_back(averageN[j]);
				mergedAverageWhichSPW.push_back(averageWhichSPW[j]);
				mergedAverageWhichChan.push_back(averageWhichChan[j]);
				mergedAverageChanFrac.push_back(averageChanFrac[j]);
			}

			vector<Int> tv;
			tv.push_back(idi); // Origin is SPW Id-i
			vector<Int> tv2;
			tv2.push_back(0);
			vector<Double> tvd;
			tvd.push_back(1.); // Fraction is 1.
			for (uInt j = 0; j < newNUM_CHANi; j++)
			{
				mergedChanFreq.push_back(newCHAN_FREQi(j));
				mergedChanWidth.push_back(newCHAN_WIDTHi(j));
				mergedEffBW.push_back(newEFFECTIVE_BWi(j));
				mergedRes.push_back(newRESOLUTIONi(j));
				mergedAverageN.push_back(1); // so far only one channel
				mergedAverageWhichSPW.push_back(tv);
				if (newIsDescendingI)
				{
					tv2[0] = newNUM_CHANi - 1 - j;
				}
				else
				{
					tv2[0] = j;
				}
				mergedAverageWhichChan.push_back(tv2); // channel number is j
				mergedAverageChanFrac.push_back(tvd);
			}
		}
		else if (newCHAN_FREQ(0) - newCHAN_WIDTH(0) / 2. > newCHAN_FREQi(
				newNUM_CHANi - 1) + newCHAN_WIDTHi(newNUM_CHANi - 1) / 2.)
		{
			// No overlap, need to prepend
			vector<Int> tv;
			tv.push_back(idi); // origin is SPW Id-i
			vector<Int> tv2;
			tv2.push_back(0);
			vector<Double> tvd;
			tvd.push_back(1.); // fraction is 1.
			for (uInt j = 0; j < newNUM_CHANi; j++)
			{
				mergedChanFreq.push_back(newCHAN_FREQi(j));
				mergedChanWidth.push_back(newCHAN_WIDTHi(j));
				mergedEffBW.push_back(newEFFECTIVE_BWi(j));
				mergedRes.push_back(newRESOLUTIONi(j));
				mergedAverageN.push_back(1); // so far only one channel
				mergedAverageWhichSPW.push_back(tv);
				if (newIsDescendingI)
				{
					tv2[0] = newNUM_CHANi - 1 - j;
				}
				else
				{
					tv2[0] = j;
				}
				mergedAverageWhichChan.push_back(tv2); // channel number is j
				mergedAverageChanFrac.push_back(tvd);
			}

			for (uInt j = 0; j < newNUM_CHAN; j++)
			{
				mergedChanFreq.push_back(newCHAN_FREQ(j));
				mergedChanWidth.push_back(newCHAN_WIDTH(j));
				mergedEffBW.push_back(newEFFECTIVE_BW(j));
				mergedRes.push_back(newRESOLUTION(j));
				mergedAverageN.push_back(averageN[j]);
				mergedAverageWhichSPW.push_back(averageWhichSPW[j]);
				mergedAverageWhichChan.push_back(averageWhichChan[j]);
				mergedAverageChanFrac.push_back(averageChanFrac[j]);
			}
		}
		// there is overlap
		else
		{
			Int id0StartChan = 0;
			// SPW Id-i starts before SPW Id-0
			if (newCHAN_FREQi(0) - newCHAN_WIDTHi(0) / 2. < newCHAN_FREQ(
					newNUM_CHAN - 1) - newCHAN_WIDTH(newNUM_CHAN - 1) / 2.)
			{


				// Some utilities for the averaging info
				vector<Int> tv; // temporary vector
				tv.push_back(idi); // origin is spw idi
				vector<Int> tv2;
				tv2.push_back(0);
				vector<Double> tvd;
				tvd.push_back(1.); // fraction is 1.

				// Find the first overlapping channel and prepend non-overlapping channels
				Double ubound0 = newCHAN_FREQ(0) + newCHAN_WIDTH(0) / 2.;
				Double lbound0 = newCHAN_FREQ(0) - newCHAN_WIDTH(0) / 2.;
				Double uboundk = 0.;
				Double lboundk = 0.;
				uInt k;
				for (k = 0; k < newNUM_CHANi; k++)
				{
					uboundk = newCHAN_FREQi(k) + newCHAN_WIDTHi(k) / 2.;
					lboundk = newCHAN_FREQi(k) - newCHAN_WIDTHi(k) / 2.;
					if (lbound0 < uboundk)
					{
						break;
					}
					mergedChanFreq.push_back(newCHAN_FREQi(k));
					mergedChanWidth.push_back(newCHAN_WIDTHi(k));
					mergedEffBW.push_back(newEFFECTIVE_BWi(k));
					mergedRes.push_back(newRESOLUTIONi(k));
					mergedAverageN.push_back(1); // So far only one channel
					mergedAverageWhichSPW.push_back(tv);
					if (newIsDescendingI)
					{
						tv2[0] = newNUM_CHANi - 1 - k;
					}
					else
					{
						tv2[0] = k;
					}
					mergedAverageWhichChan.push_back(tv2); // Channel number is k
					mergedAverageChanFrac.push_back(tvd);
				}

				// k-th is the one, actual overlap, need to merge channel k with channel 0
				if (lbound0 < uboundk && lboundk < lbound0)
				{
					Double newWidth = ubound0 - lboundk;
					Double newCenter = lboundk + newWidth / 2.;
					mergedChanFreq.push_back(newCenter);
					mergedChanWidth.push_back(newWidth);
					mergedEffBW.push_back(newWidth);
					mergedRes.push_back(newWidth);
					mergedAverageN.push_back(averageN[0] + 1); // one more channel contributes

					// Channel k is from SPW Id-i
					if (newIsDescendingI)
					{
						tv2[0] = newNUM_CHANi - 1 - k;
					}
					else
					{
						tv2[0] = k;
					}

					for (int j = 0; j < averageN[0]; j++)
					{
						tv.push_back(averageWhichSPW[0][j]); // additional contributors
						tv2.push_back(averageWhichChan[0][j]); // channel 0 from SPW Id-0
						tvd.push_back(averageChanFrac[0][j]);
					}
					mergedAverageWhichSPW.push_back(tv);
					mergedAverageWhichChan.push_back(tv2);
					mergedAverageChanFrac.push_back(tvd);
					id0StartChan = 1;
				}
			}

			// Now move along SPW id0 and merge until end of id0 is reached, then just copy
			for (uInt j = id0StartChan; j < newNUM_CHAN; j++)
			{
				mergedChanFreq.push_back(newCHAN_FREQ(j));
				mergedChanWidth.push_back(newCHAN_WIDTH(j));
				mergedEffBW.push_back(newEFFECTIVE_BW(j));
				mergedRes.push_back(newRESOLUTION(j));

				for (uInt k = 0; k < newNUM_CHANi; k++)
				{
					Double overlap_frac = 0.;

					// Does channel j in SPW Id-0 overlap with channel k in SPW Id-i?
					Double uboundj = newCHAN_FREQ(j) + newCHAN_WIDTH(j)/ 2.;
					Double uboundk = newCHAN_FREQi(k) + newCHAN_WIDTHi(k)/ 2.;
					Double lboundj = newCHAN_FREQ(j) - newCHAN_WIDTH(j)/ 2.;
					Double lboundk = newCHAN_FREQi(k) - newCHAN_WIDTHi(k)/ 2.;

					// Determine fraction

					// Chan k is completely covered by chan j
					if (lboundj <= lboundk && uboundk <= uboundj)
					{
						overlap_frac = 1.;
					}
					// chan j is completely covered by chan k
					else if (lboundk <= lboundj && uboundj <= uboundk)
					{
						overlap_frac = newCHAN_WIDTH(j) / newCHAN_WIDTHi(k);
					}
					// lower end of k is overlapping with j
					else if (lboundj < lboundk && lboundk < uboundj && uboundj < uboundk)
					{
						overlap_frac = (uboundj - lboundk) / newCHAN_WIDTHi(k);
					}
					// upper end of k is overlapping with j
					else if (lboundk < lboundj && lboundj < uboundk && lboundj < uboundk)
					{
						overlap_frac = (uboundk - lboundj) / newCHAN_WIDTHi(k);
					}

					// Update averaging info
					if (overlap_frac > 0.)
					{
						averageN[j] += 1;
						averageWhichSPW[j].push_back(idi);
						if (newIsDescendingI)
						{
							averageWhichChan[j].push_back(newNUM_CHANi - 1 - k);
						}
						else
						{
							averageWhichChan[j].push_back(k);
						}
						averageChanFrac[j].push_back(overlap_frac);
					}
				} // End loop over SPW Id-i


				// Append this channel with updated averaging info
				mergedAverageN.push_back(averageN[j]);
				mergedAverageWhichSPW.push_back(averageWhichSPW[j]);
				mergedAverageWhichChan.push_back(averageWhichChan[j]);
				mergedAverageChanFrac.push_back(averageChanFrac[j]);

			} // End loop over SPW Id-0

			// SPW Id-i still continues, find the last overlapping channel
			if (newCHAN_FREQ(newNUM_CHAN - 1) + newCHAN_WIDTH(
					newNUM_CHAN - 1) / 2. < newCHAN_FREQi(newNUM_CHANi - 1)
					+ newCHAN_WIDTHi(newNUM_CHANi - 1) / 2.)
			{
				Int j = newNUM_CHAN - 1;
				Double uboundj = newCHAN_FREQ(j) + newCHAN_WIDTH(j) / 2.;
				Double lboundj = newCHAN_FREQ(j) - newCHAN_WIDTH(j) / 2.;
				Double uboundk = 0;
				Double lboundk = 0;
				Int k;
				for (k = newNUM_CHANi - 1; k >= 0; k--)
				{
					uboundk = newCHAN_FREQi(k) + newCHAN_WIDTHi(k) / 2.;
					lboundk = newCHAN_FREQi(k) - newCHAN_WIDTHi(k) / 2.;
					if (lboundk <= uboundj)
					{
						break;
					}
				}

				// k-th is the one

				// actual overlap
				if (lboundk < uboundj && uboundj < uboundk)
				{
					Double overlap_frac = (uboundj - lboundk) / newCHAN_WIDTHi(k);

					// Merge channel k completely with channel j
					if (overlap_frac > 0.01)
					{
						Double newWidth = uboundk - lboundj;
						Double newCenter = (lboundj + uboundk) / 2.;
						mergedChanFreq[j] = newCenter;
						mergedChanWidth[j] = newWidth;
						mergedEffBW[j] = newWidth;
						mergedRes[j] = newWidth;
						mergedAverageChanFrac[j][mergedAverageN[j] - 1] = 1.;
					}
					// Create separate, (slightly) more narrow channel
					else
					{
						Double newWidth = uboundk - uboundj;
						Double newCenter = (uboundj + uboundk) / 2.;
						vector<Int> tv;
						tv.push_back(idi); // Origin is SPW Id-i
						vector<Int> tv2;
						tv2.push_back(0);
						vector<Double> tvd;
						tvd.push_back(1.); // Fraction is 1.
						mergedChanFreq.push_back(newCenter);
						mergedChanWidth.push_back(newWidth);
						mergedEffBW.push_back(newWidth);
						mergedRes.push_back(newWidth);
						mergedAverageN.push_back(1); // So far only one channel
						mergedAverageWhichSPW.push_back(tv);
						if (newIsDescendingI)
						{
							tv2[0] = newNUM_CHANi - 1 - k;
						}
						else
						{
							tv2[0] = k;
						}
						mergedAverageWhichChan.push_back(tv2); // Channel number is k
						mergedAverageChanFrac.push_back(tvd);
					}

					k++; // Start appending remaining channels after k
				}

				// Append the remaining channels
				vector<Int> tv;
				tv.push_back(idi); // Origin is SPW Id-i
				vector<Int> tv2;
				tv2.push_back(0);
				vector<Double> tvd;
				tvd.push_back(1.); // Fraction is 1.

				for (uInt m = k; m < newNUM_CHANi; m++)
				{
					mergedChanFreq.push_back(newCHAN_FREQi(m));
					mergedChanWidth.push_back(newCHAN_WIDTHi(m));
					mergedEffBW.push_back(newEFFECTIVE_BWi(m));
					mergedRes.push_back(newRESOLUTIONi(m));
					mergedAverageN.push_back(1); // So far only one channel
					mergedAverageWhichSPW.push_back(tv);

					if (newIsDescendingI)
					{
						tv2[0] = newNUM_CHANi - 1 - m;
					}
					else
					{
						tv2[0] = m;
					}
					mergedAverageWhichChan.push_back(tv2); // Channel number is m
					mergedAverageChanFrac.push_back(tvd);
				}
			} // End if SPW Id-i still continues
		} // End if there is overlap


		newNUM_CHAN = mergedChanFreq.size();
		newCHAN_FREQ.assign(Vector<Double> (mergedChanFreq));
		newCHAN_WIDTH.assign(Vector<Double> (mergedChanWidth));
		newEFFECTIVE_BW.assign(Vector<Double> (mergedEffBW));
		newRESOLUTION.assign(Vector<Double> (mergedRes));
		averageN = mergedAverageN;
		averageWhichSPW.assign(mergedAverageWhichSPW.begin(), mergedAverageWhichSPW.end());
		averageChanFrac.assign(mergedAverageChanFrac.begin(), mergedAverageChanFrac.end());
		averageWhichChan.assign(mergedAverageWhichChan.begin(), mergedAverageWhichChan.end());
	}

	return true;
}

// -----------------------------------------------------------------------
// A wrapper for regridChanBounds() which takes the user interface type re-gridding parameters
// The ready-made grid is returned in newCHAN_FREQ and newCHAN_WIDTH
// -----------------------------------------------------------------------
Bool MSTransformRegridder::calcChanFreqs(	LogIO& os,
			    							Vector<Double>& newCHAN_FREQ,
			    							Vector<Double>& newCHAN_WIDTH,
			    							Double& weightScale,
			    							const Vector<Double>& oldCHAN_FREQ,
			    							const Vector<Double>& oldCHAN_WIDTH,
			    							const MDirection  phaseCenter,
			    							const MFrequency::Types theOldRefFrame,
			    							const MEpoch theObsTime,
			    							const MPosition mObsPos,
			    							const String& mode,
			    							const int nchan,
			    							const String& start,
			    							const String& width,
			    							const String& restfreq,
			    							const String& outframe,
			    							const String& veltype,
			    							const Bool verbose,
			    							const MRadialVelocity mRV)
{
	Vector<Double> newChanLoBound;
	Vector<Double> newChanHiBound;
	String t_phasec;

	String t_mode;
	String t_outframe;
	String t_regridQuantity;
	Double t_restfreq;
	String t_regridInterpMeth;
	Double t_cstart;
	Double t_bandwidth;
	Double t_cwidth;
	Bool t_centerIsStart;
	Bool t_startIsEnd;
	Int t_nchan;
	Int t_width;
	Int t_start;

	if (!convertGridPars(os, mode, nchan, start,width,
						"linear", // A dummy value in this context
						restfreq, outframe,veltype,
						////// output ////
						t_mode, t_outframe, t_regridQuantity, t_restfreq,
						t_regridInterpMeth, t_cstart, t_bandwidth, t_cwidth,
						t_centerIsStart, t_startIsEnd, t_nchan, t_width, t_start))
	{
		// An error occurred
		return false;
	}

	// Reference frame transformation
	Bool needTransform = true;
	Bool doRadVelCorr = false;
	MFrequency::Types theFrame;
	String oframe = outframe;
	oframe.upcase();

	// No output reference frame given
	if (outframe == "")
	{
		// Keep the reference frame as is
		theFrame = theOldRefFrame;
		needTransform = false;
	}
	// GEO transformation + radial velocity correction
	else if (oframe == "SOURCE")
	{
		theFrame = MFrequency::GEO;
		doRadVelCorr = true;
	}
	else if (!MFrequency::getType(theFrame, outframe))
	{
		os << LogIO::SEVERE << LogOrigin("MSTransformRegridder", __FUNCTION__)
				<< "Parameter \"outframe\" value " << outframe << " is invalid." << LogIO::POST;
		return false;
	}
	else if (theFrame == theOldRefFrame)
	{
		needTransform = false;
	}

	uInt oldNUM_CHAN = oldCHAN_FREQ.size();
	if (oldNUM_CHAN == 0)
	{
		newCHAN_FREQ.resize(0);
		newCHAN_WIDTH.resize(0);
		return true;
	}

	if (oldNUM_CHAN != oldCHAN_WIDTH.size())
	{
		os 		<< LogIO::SEVERE << LogOrigin("MSTransformRegridder", __FUNCTION__)
				<< "Internal error: inconsistent dimensions of input channel frequency and width arrays."
				<< LogIO::POST;
		return false;
	}

	Vector<Double> absOldCHAN_WIDTH;
	absOldCHAN_WIDTH.assign(oldCHAN_WIDTH);
	Bool negativeWidths = false;
	for (uInt i = 0; i < oldCHAN_WIDTH.size(); i++)
	{
		if (oldCHAN_WIDTH(i) < 0.)
		{
			negativeWidths = true;
			absOldCHAN_WIDTH(i) = -oldCHAN_WIDTH(i);
		}
	}

	if (negativeWidths && verbose)
	{
		os 		<< LogIO::NORMAL << LogOrigin("MSTransformRegridder", __FUNCTION__)
				<< " *** Encountered negative channel widths in input spectral window."
				<< LogIO::POST;
	}

	Vector<Double> transNewXin;
	Vector<Double> transCHAN_WIDTH(oldNUM_CHAN);

	if (needTransform)
	{
		transNewXin.resize(oldNUM_CHAN);

		// Set up conversion
		Unit unit(String("Hz"));
		MFrequency::Ref fromFrame = MFrequency::Ref(theOldRefFrame,
				MeasFrame(phaseCenter, mObsPos, theObsTime));
		MFrequency::Ref toFrame = MFrequency::Ref(theFrame,
				MeasFrame(phaseCenter, mObsPos, theObsTime));
		MFrequency::Convert freqTrans(unit, fromFrame, toFrame);

		MDoppler radVelCorr;
		Bool radVelSignificant = false;

		// Prepare correction for radial velocity if requested and possible
		if (doRadVelCorr)
		{
			Quantity mrv = mRV.get("m/s");
			radVelCorr = MDoppler(-mrv); // NOTE: Opposite sign to achieve correction!
			Double mRVVal = mrv.getValue();
			if (fabs(mRVVal) > 1E-6)
			{
				radVelSignificant = true;
			}

			if (verbose)
			{
				os 	<< LogIO::NORMAL << LogOrigin("MSTransformRegridder", __FUNCTION__)
					<< "Note: The given additional radial velocity of "
					<< mRVVal << " m/s will be taken into account."
					<< LogIO::POST;
			}
		}

		for (uInt i = 0; i < oldNUM_CHAN; i++)
		{
			transNewXin[i] = freqTrans(oldCHAN_FREQ[i]).get(unit).getValue();

			// eliminate possible offsets
			transCHAN_WIDTH[i] = freqTrans(oldCHAN_FREQ[i] + absOldCHAN_WIDTH[i] / 2.).
								get(unit).getValue() - freqTrans(oldCHAN_FREQ[i] - absOldCHAN_WIDTH[i] / 2.).
								get(unit).getValue();
		}

		// Correct in addition for radial velocity
		if (radVelSignificant)
		{
			transNewXin = radVelCorr.shiftFrequency(transNewXin);

			//shiftFrequency is a scaling, so channel widths scale as well
			transCHAN_WIDTH = radVelCorr.shiftFrequency(transCHAN_WIDTH);
		}

	}
	else
	{
		// Just copy
		transNewXin.assign(oldCHAN_FREQ);
		transCHAN_WIDTH.assign(absOldCHAN_WIDTH);
	}

	// Calculate new grid

	String message;

	if (!regridChanBounds(	newChanLoBound, newChanHiBound, t_cstart,
							t_bandwidth, t_cwidth, t_restfreq, t_regridQuantity, transNewXin,
							transCHAN_WIDTH, message, t_centerIsStart, t_startIsEnd, t_nchan,
							t_width, t_start))
	{
		// There was an error
		os << LogIO::WARN << LogOrigin("MSTransformRegridder", __FUNCTION__) << message << LogIO::POST;
		return false;
	}

	if (verbose)
	{
		os << LogIO::NORMAL << LogOrigin("MSTransformRegridder", __FUNCTION__) << message << LogIO::POST;
	}

	// We have a useful set of channel boundaries
	uInt newNUM_CHAN = newChanLoBound.size();

	// Complete the calculation of the new channel centers and widths
	// from newNUM_CHAN, newChanLoBound, and newChanHiBound
	newCHAN_FREQ.resize(newNUM_CHAN);
	newCHAN_WIDTH.resize(newNUM_CHAN);
	for (uInt i = 0; i < newNUM_CHAN; i++)
	{
		newCHAN_FREQ[i] = (newChanLoBound[i] + newChanHiBound[i]) / 2.;
		newCHAN_WIDTH[i] = newChanHiBound[i] - newChanLoBound[i];
	}

	weightScale = newCHAN_WIDTH[0]/transCHAN_WIDTH[0];

	return true;
}

// -----------------------------------------------------------------------
// Helper function for handling the re-gridding parameter user input
// -----------------------------------------------------------------------
Bool MSTransformRegridder::convertGridPars(	LogIO& os,
											const String& mode,
											const int nchan,
											const String& start,
											const String& width,
											const String& interp,
											const String& restfreq,
											const String& outframe,
											const String& veltype,
											String& t_mode,
											String& t_outframe,
											String& t_regridQuantity,
											Double& t_restfreq,
											String& t_regridInterpMeth,
											Double& t_cstart,
											Double& t_bandwidth,
											Double& t_cwidth,
											Bool& t_centerIsStart,
											Bool& t_startIsEnd,
											Int& t_nchan,
											Int& t_width,
											Int& t_start)
{
	Bool rstat(false);

	try {

		casacore::QuantumHolder qh;
		String error;

		t_mode = mode;
		t_restfreq = 0.;
		if (!restfreq.empty() && !(restfreq == "[]"))
		{
			if (qh.fromString(error, restfreq))
			{
				t_restfreq = qh.asQuantity().getValue("Hz");
			}
			else
			{
				os << LogIO::SEVERE << LogOrigin("MSTransformRegridder", __FUNCTION__)
						<< "restfreq: " << error << LogIO::POST;
				return false;
			}
		}

		// Determine grid
		t_cstart = -9e99; // Default value indicating that the original start of the SPW should be used
		t_bandwidth = -1.; // Default value indicating that the original width of the SPW should be used
		t_cwidth = -1.; // Default value indicating that the original channel width of the SPW should be used
		t_nchan = -1;
		t_width = 0;
		t_start = -1;
		t_startIsEnd = false; 	// false means that start specifies the lower end in frequency (default)
								// true means that start specifies the upper end in frequency

		// start was set
		if (!start.empty() && !(start == "[]"))
		{
			if (t_mode == "channel")
			{
				t_start = atoi(start.c_str());
			}
			if (t_mode == "channel_b")
			{
				t_cstart = Double(atoi(start.c_str()));
			}
			else if (t_mode == "frequency")
			{
				if (qh.fromString(error, start))
				{
					t_cstart = qh.asQuantity().getValue("Hz");
				}
				else
				{
					os << LogIO::SEVERE << LogOrigin("MSTransformRegridder", __FUNCTION__)
							<< "start: " << error << LogIO::POST;
					return false;
				}
			}
			else if (t_mode == "velocity")
			{
				if (qh.fromString(error, start))
				{
					t_cstart = qh.asQuantity().getValue("m/s");
				}
				else
				{
					os << LogIO::SEVERE << LogOrigin("MSTransformRegridder", __FUNCTION__)
							<< "start: " << error << LogIO::POST;
					return false;
				}
			}
		}

		// channel width was set
		if (!width.empty() && !(width == "[]"))
		{
			if (t_mode == "channel")
			{
				Int w = atoi(width.c_str());
				t_width = abs(w);
				if (w < 0)
				{
					t_startIsEnd = true;
				}
			}
			else if (t_mode == "channel_b")
			{
				Double w = atoi(width.c_str());
				t_cwidth = abs(w);
				if (w < 0)
				{
					t_startIsEnd = true;
				}
			}
			else if (t_mode == "frequency")
			{
				if (qh.fromString(error, width))
				{
					Double w = qh.asQuantity().getValue("Hz");
					t_cwidth = abs(w);
					if (w < 0)
					{
						t_startIsEnd = true;
					}
				}
				else
				{
					os << LogIO::SEVERE << LogOrigin("MSTransformRegridder", __FUNCTION__)
							<< "width: " << error << LogIO::POST;
					return false;
				}
			}
			else if (t_mode == "velocity")
			{
				if (qh.fromString(error, width))
				{
					Double w = qh.asQuantity().getValue("m/s");
					t_cwidth = abs(w);
					if (w >= 0)
					{
						t_startIsEnd = true;
					}
				}
				else
				{
					os << LogIO::SEVERE << LogOrigin("MSTransformRegridder", __FUNCTION__)
							<< "width: " << error << LogIO::POST;
					return false;
				}
			}
		}
		// width was not set
		else
		{
			// For the velocity mode the default t_startIsEnd is true if the sign of width is not known
			if (t_mode == "velocity")
			{
				t_startIsEnd = true;
			}
		}

		// Number of output channels was set
		if (nchan > 0)
		{
			if (t_mode == "channel_b")
			{
				if (t_cwidth > 0)
				{
					t_bandwidth = Double(nchan * t_cwidth);
				}
				else
				{
					t_bandwidth = Double(nchan);
				}
			}
			else
			{
				t_nchan = nchan;
			}
		}

		if (t_mode == "channel")
		{
			t_regridQuantity = "freq";
		}
		else if (t_mode == "channel_b")
		{
			t_regridQuantity = "chan";
		}
		else if (t_mode == "frequency")
		{
			t_regridQuantity = "freq";
		}
		else if (t_mode == "velocity")
		{
			if (t_restfreq == 0.)
			{
				os << LogIO::SEVERE << LogOrigin("MSTransformRegridder", __FUNCTION__)
						<< "Need to set restfreq in velocity mode."
						<< LogIO::POST;
				return false;
			}

			t_regridQuantity = "vrad";
			if (veltype == "optical")
			{
				t_regridQuantity = "vopt";
			}
			else if (veltype != "radio")
			{
				os << LogIO::WARN << LogOrigin("MSTransformRegridder", __FUNCTION__)
						<< "Invalid velocity type " << veltype
						<< ", setting type to \"radio\"" << LogIO::POST;
			}

		}
		else
		{
			os << LogIO::WARN << LogOrigin("MSTransformRegridder", __FUNCTION__)
					<< "Invalid mode " << t_mode << LogIO::POST;
			return false;
		}

		t_outframe = outframe;
		t_regridInterpMeth = interp;
		t_centerIsStart = true;

		// End prepare re-gridding parameters
		rstat = true;

	}
	catch (AipsError x)
	{
		os << LogIO::SEVERE << LogOrigin("MSTransformRegridder", __FUNCTION__)
				<< "Exception Reported: " << x.getMesg() << LogIO::POST;
		rstat = false;
	}

	return rstat;
}

// -----------------------------------------------------------------------
// Calculate the final new channel boundaries from the re-regridding parameters and
// the old channel boundaries (already transformed to the desired reference frame).
// Returns false if input parameters were invalid and no useful boundaries could be created
// -----------------------------------------------------------------------
Bool MSTransformRegridder::regridChanBounds(	Vector<Double>& newChanLoBound,
												Vector<Double>& newChanHiBound,
												const Double regridCenterC,
												const Double regridBandwidth,
												const Double regridChanWidthC,
												const Double regridVeloRestfrq,
												const String regridQuant,
												const Vector<Double>& transNewXinC,
												const Vector<Double>& transCHAN_WIDTHC,
												String& message,
												const Bool centerIsStartC,
												const Bool startIsEndC,
												const Int nchanC,
												const Int width,
												const Int startC)
{
	ostringstream oss;

	Vector<Double> transNewXin(transNewXinC);
	Vector<Double> transCHAN_WIDTH(transCHAN_WIDTHC);
	Bool centerIsStart = centerIsStartC;
	Bool startIsEnd = startIsEndC;
	Double regridChanWidth = regridChanWidthC;
	Double regridCenter = regridCenterC;
	Int nchan = nchanC;
	Int start = startC;

	Int oldNUM_CHAN = transNewXin.size();

	// Detect spectral windows defined with descending frequency
	Bool isDescending = false;
	for (uInt i = 1; i < transNewXin.size(); i++)
	{
		if (transNewXin(i) < transNewXin(i - 1))
		{
			isDescending = true;
		}
		// i.e. descending was detected but now we encounter ascending
		else if (isDescending)
		{
			oss << "Channel frequencies are neither in ascending nor in descending order. Cannot process.";
			message = oss.str();
			return false;
		}
	}

	// Need to reverse the order for processing and later reverse the result
	if (isDescending)
	{
		uInt n = transNewXin.size();
		Vector<Double> tempF, tempW;
		tempF.assign(transNewXin);
		tempW.assign(transCHAN_WIDTH);
		for (uInt i = 0; i < n; i++)
		{
			transNewXin(i) = tempF(n - 1 - i);
			transCHAN_WIDTH(i) = tempW(n - 1 - i);
		}

		// Also need to adjust the start values
		if (startC >= 0)
		{
			start = n - 1 - startC;
			if (centerIsStartC)
			{
				startIsEnd = !startIsEnd;
			}
		}
	}

	// Verify regridCenter, regridBandwidth, and regridChanWidth
	// Note: these are in the units corresponding to regridQuant!

	if (regridQuant == "chan")
	{
		// Channel numbers ...
		Int regridCenterChan = -1;
		Int regridBandwidthChan = -1;
		Int regridChanWidthChan = -1;

		// Not set
		if (regridCenter < -1E30)
		{
			// Find channel center closest to center of bandwidth
			lDouble BWCenterF = (transNewXin[0] + transNewXin[oldNUM_CHAN - 1]) / 2.;
			for (Int i = 0; i < oldNUM_CHAN; i++)
			{
				if (transNewXin[i] >= BWCenterF)
				{
					regridCenterChan = i;
					break;
				}
			}
			centerIsStart = false;
		}
		// Valid input
		else if (0. <= regridCenter && regridCenter < Double(oldNUM_CHAN))
		{
			regridCenterChan = (Int) floor(regridCenter);
		}
		// Invalid
		else
		{
			if (centerIsStart)
			{
				oss << "SPW start ";
			}
			else
			{
				oss << "SPW center ";
			}
			oss << regridCenter << " outside valid range which is " << 0 << " - " << oldNUM_CHAN - 1 << ".";
			message = oss.str();
			return false;
		}

		// Not set or nchan set
		if (regridBandwidth <= 0. || nchan > 0)
		{
			if (nchan > 0)
			{
				regridBandwidthChan = nchan;
			}
			else
			{
				regridBandwidthChan = oldNUM_CHAN;
			}
		}
		else
		{
			regridBandwidthChan = (Int) floor(regridBandwidth);
		}

		if (centerIsStart)
		{
			if (startIsEnd)
			{
				regridCenterChan = regridCenterChan - regridBandwidthChan / 2;
			}
			else
			{
				regridCenterChan = regridCenterChan + regridBandwidthChan / 2;
			}
			centerIsStart = false;
		}

		// Center too close to lower edge
		if (regridCenterChan - regridBandwidthChan / 2 < 0)
		{
			regridBandwidthChan = 2 * regridCenterChan + 1;
			oss << " *** Requested output SPW width too large." << endl;
		}
		// Center too close to upper edge
		if (oldNUM_CHAN < regridCenterChan + regridBandwidthChan / 2)
		{
			regridBandwidthChan = 2 * (oldNUM_CHAN - regridCenterChan);
			oss << " *** Requested output SPW width too large." << endl;
		}

		if (regridChanWidth < 1.)
		{
			regridChanWidthChan = 1;
		}
		else if (regridChanWidth > Double(regridBandwidthChan))
		{
			regridChanWidthChan = regridBandwidthChan; // i.e. SPW = a single channel
			oss << " *** Requested output channel width too large. Adjusted to maximum possible value." << endl;
		}
		// Valid input
		else
		{
			regridChanWidthChan = (Int) floor(regridChanWidth);
			if (nchan > 0)
			{
				regridBandwidthChan = nchan * regridChanWidthChan;
			}
		}

		if (regridBandwidthChan != floor(regridBandwidth))
		{
			oss << " *** Output SPW width set to " << regridBandwidthChan << " original channels" << endl;
			oss << "     in an attempt to keep center of output SPW close to center of requested SPW." << endl;
		}

		// Calculate newChanLoBound and newChanHiBound from
		// regridCenterChan, regridBandwidthChan, and regridChanWidthChan
		Int bwLowerEndChan = regridCenterChan - regridBandwidthChan / 2;
		Int bwUpperEndChan = bwLowerEndChan + regridBandwidthChan - 1;
		Int numNewChanDown = 0;
		Int numNewChanUp = 0;

		// Only one new channel
		if (regridChanWidthChan == regridBandwidthChan)
		{
			newChanLoBound.resize(1);
			newChanHiBound.resize(1);
			newChanLoBound[0] = transNewXin[bwLowerEndChan] - transCHAN_WIDTH[bwLowerEndChan] / 2.;
			newChanHiBound[0] = transNewXin[bwUpperEndChan] + transCHAN_WIDTH[bwUpperEndChan] / 2.;
			numNewChanUp = 1;
		}
		// Have more than one new channel
		else
		{
			// Need to accommodate the possibility that the original channels are not contiguous!

			// The numbers of the Channels from which the lower bounds will be taken for the new channels
			vector<Int> loNCBup;
			// Starting from the central channel going up
			vector<Int> hiNCBup; // The numbers of the Channels from which the high
			// Bounds will be taken for the new channels starting from the central channel going up
			vector<Int> loNCBdown; // The numbers of the Channels from which the
			// Lower bounds will be taken for the new channels starting from the central channel going down
			vector<Int> hiNCBdown; 	// The numbers of the Channels from which the high bounds will be taken
									// for the new channels starting from the central channel going down.
									// Want to keep the center of the center channel at the center of the new center
									// channel if the bandwidth is an odd multiple of the new channel width
									// otherwise the center channel is the lower edge of the new center channel
			Int startChan;
			lDouble tnumChan = regridBandwidthChan / regridChanWidthChan;
			if ((Int) tnumChan % 2 != 0)
			{
				// Odd multiple
				startChan = regridCenterChan - regridChanWidthChan / 2;
			}
			else
			{
				startChan = regridCenterChan;
			}

			// Upper half
			for (Int i = startChan; i <= bwUpperEndChan; i += regridChanWidthChan)
			{
				loNCBup.push_back(i);
				if (i + regridChanWidthChan - 1 <= bwUpperEndChan)
				{
					// Can go one more normal step up
					hiNCBup.push_back(i + regridChanWidthChan - 1);
				}
				else
				{
					// Create narrower channels at the edges if necessary
					oss		<< " *** Last channel at upper edge of new SPW made only "
							<< bwUpperEndChan - i + 1
							<< " original channels wide to fit given total bandwidth."
							<< endl;
					hiNCBup.push_back(bwUpperEndChan);
				}
			}

			// Lower half
			for (Int i = startChan - 1; i >= bwLowerEndChan; i -= regridChanWidthChan)
			{
				hiNCBdown.push_back(i);
				if (i - regridChanWidthChan + 1 >= bwLowerEndChan)
				{
					// Can go one more normal step down
					loNCBdown.push_back(i - regridChanWidthChan + 1);
				}
				else
				{
					// Create narrower channels at the edges if necessary
					oss		<< " *** First channel at lower edge of new SPW made only "
							<< i - bwLowerEndChan + 1
							<< " original channels wide to fit given total bandwidth."
							<< endl;
					loNCBdown.push_back(bwLowerEndChan);
				}
			}

			// The number of channels below the central one
			numNewChanDown = loNCBdown.size();

			// The number of channels above and including the central one
			numNewChanUp = loNCBup.size();

			newChanLoBound.resize(numNewChanDown + numNewChanUp);
			newChanHiBound.resize(numNewChanDown + numNewChanUp);
			for (Int i = 0; i < numNewChanDown; i++)
			{
				Int k = numNewChanDown - i - 1; // Need to assign in reverse
				newChanLoBound[i] = transNewXin[loNCBdown[k]] - transCHAN_WIDTH[loNCBdown[k]] / 2.;
				newChanHiBound[i] = transNewXin[hiNCBdown[k]] + transCHAN_WIDTH[hiNCBdown[k]] / 2.;
			}

			for (Int i = 0; i < numNewChanUp; i++)
			{
				newChanLoBound[i + numNewChanDown] = transNewXin[loNCBup[i]] - transCHAN_WIDTH[loNCBup[i]] / 2.;
				newChanHiBound[i + numNewChanDown] = transNewXin[hiNCBup[i]] + transCHAN_WIDTH[hiNCBup[i]] / 2.;
			}
		} // end if

		oss 	<< " New channels defined based on original channels" << endl
				<< " Central channel contains original channel "
				<< regridCenterChan << endl << " Channel width = "
				<< regridChanWidthChan << " original channels" << endl
				<< " Total width of SPW = " << regridBandwidthChan
				<< " original channels == " << numNewChanDown + numNewChanUp
				<< " new channels" << endl;

		uInt nc = newChanLoBound.size();
		oss << " Total width of SPW (in output frame) = "
			<< newChanHiBound[nc- 1] - newChanLoBound[0] << " Hz" << endl;
		oss << " Lower edge = " << std::setprecision(9) << newChanLoBound[0] << " Hz,"
		    << " upper edge = " << std::setprecision(9) << newChanHiBound[nc - 1] << " Hz" << endl;

		if (isDescending)
		{
			Vector<Double> tempL, tempU;
			tempL.assign(newChanLoBound);
			tempU.assign(newChanHiBound);
			for (uInt i = 0; i < nc; i++)
			{
				newChanLoBound(i) = tempL(nc - 1 - i);
				newChanHiBound(i) = tempU(nc - 1 - i);
			}
		}

		message = oss.str();

		return true;
	}
	// We operate on real numbers /////////////////
	else
	{
		// First transform them to frequencies
		lDouble regridCenterF = -1.; // Initialize as "not set"
		lDouble regridBandwidthF = -1.;
		lDouble regridChanWidthF = -1.;

		if (regridQuant == "vrad")
		{
			// radio velocity need restfrq
			if (regridVeloRestfrq < -1E30)  // means "not set"
			{
				oss << "Parameter \"restfreq\" needs to be set if regrid_quantity==vrad. Cannot proceed with regridSpw ...";
				message = oss.str();
				return false;
			}
			else if (regridVeloRestfrq < 0. || regridVeloRestfrq > 1E30)
			{
				oss << "Parameter \"restfreq\" value " << regridVeloRestfrq << " is invalid.";
				message = oss.str();
				return false;
			}

			lDouble regridCenterVel;
			if (regridCenter > -C::c)
			{
				// (We deal with invalid values later)
				if (centerIsStart)
				{
					Double tcWidth;
					if (regridChanWidth > 0.)
					{
						tcWidth = regridChanWidth;
					}
					else
					{
						tcWidth = vrad(	transNewXin[0] - transCHAN_WIDTH[0] / 2.,
										regridVeloRestfrq) - vrad(
										transNewXin[0] + transCHAN_WIDTH[0] / 2.,
										regridVeloRestfrq);
					}

					// start is the center of the last channel (in freq)
					if (startIsEnd)
					{
						regridCenter -= tcWidth / 2.;
					}
					// start is the center of the first channel (in freq)
					else
					{
						regridCenter += tcWidth / 2.;
					}
				}

				regridCenterF = freq_from_vrad(regridCenter, regridVeloRestfrq);

				regridCenterVel = regridCenter;
			}
			// center was not specified
			else
			{
				regridCenterF = (transNewXin[0] + transNewXin[oldNUM_CHAN - 1])/ 2.;
				regridCenterVel = vrad(regridCenterF, regridVeloRestfrq);
				centerIsStart = false;
			}
			if (nchan > 0)
			{
				if (regridChanWidth > 0.)
				{
					lDouble chanUpperEdgeF = freq_from_vrad(regridCenterVel - regridChanWidth / 2.,regridVeloRestfrq);
					regridChanWidthF = 2. * (chanUpperEdgeF - regridCenterF);
				}
				// take channel width from first channel
				else
				{
					regridChanWidthF = transCHAN_WIDTH[0];
				}

				regridBandwidthF = nchan * regridChanWidthF;
				// Can convert start to center
				if (centerIsStart)
				{
					if (startIsEnd)
					{
						regridCenterF = regridCenterF - regridBandwidthF / 2.;
					}
					else
					{
						regridCenterF = regridCenterF + regridBandwidthF / 2.;
					}
					centerIsStart = false;
				}
			}
			else if (regridBandwidth > 0.)
			{
				// Can convert start to center
				if (centerIsStart)
				{
					if (startIsEnd)
					{
						regridCenterVel = regridCenter + regridBandwidth / 2.;
					}
					else
					{
						regridCenterVel = regridCenter - regridBandwidth / 2.;
					}
					regridCenterF = freq_from_vrad(regridCenterVel,regridVeloRestfrq);
					centerIsStart = false;
				}
				lDouble bwUpperEndF = freq_from_vrad(regridCenterVel - regridBandwidth / 2.,regridVeloRestfrq);
				regridBandwidthF = 2. * (bwUpperEndF - regridCenterF);
			}

			if (regridChanWidth > 0. && regridChanWidthF < 0.)
			{
				lDouble chanUpperEdgeF = freq_from_vrad(regridCenterVel - regridChanWidth / 2.,regridVeloRestfrq);
				regridChanWidthF = 2. * (chanUpperEdgeF - freq_from_vrad(regridCenterVel, regridVeloRestfrq));
			}
		}
		else if (regridQuant == "vopt")
		{
			// Optical velocity need restfrq
			if (regridVeloRestfrq < -1E30) // means "not set"
			{
				oss << "Parameter \"restfreq\" needs to be set if regrid_quantity==vopt. Cannot proceed with regridSpw ...";
				message = oss.str();
				return false;
			}
			else if (regridVeloRestfrq <= 0. || regridVeloRestfrq > 1E30)
			{
				oss << "Parameter \"restfreq\" value " << regridVeloRestfrq << " is invalid.";
				message = oss.str();
				return false;
			}

			lDouble regridCenterVel;
			if (regridCenter > -C::c)
			{
				if (centerIsStart)
				{
					Double tcWidth;
					if (regridChanWidth > 0.)
					{
						tcWidth = regridChanWidth;
					}
					else
					{
						tcWidth = vopt(	transNewXin[0] - transCHAN_WIDTH[0] / 2.,
										regridVeloRestfrq) - vopt(
										transNewXin[0] + transCHAN_WIDTH[0] / 2.,
										regridVeloRestfrq);
					}

					// start is the center of the last channel (in freq)
					if (startIsEnd)
					{
						regridCenter -= tcWidth / 2.;
					}
					// start is the center of the first channel (in freq)
					else
					{
						regridCenter += tcWidth / 2.;
					}
				}

				// (We deal with invalid values later)
				regridCenterF = freq_from_vopt(regridCenter, regridVeloRestfrq);
				regridCenterVel = regridCenter;
			}
			// Center was not specified
			else
			{
				regridCenterF = (transNewXin[0] - transCHAN_WIDTH[0]
				                 + transNewXin[oldNUM_CHAN - 1] + transCHAN_WIDTH[oldNUM_CHAN - 1]) / 2.;
				regridCenterVel = vopt(regridCenterF, regridVeloRestfrq);
				centerIsStart = false;
			}

			if (nchan > 0)
			{
				lDouble cw;
				lDouble divbytwo = 0.5;
				if (centerIsStart)
				{
					divbytwo = 1.;
				}
				if (regridChanWidth > 0.)
				{
					cw = regridChanWidth;
				}
				// Determine channel width from first channel
				else
				{
					lDouble upEdge = vopt(transNewXin[0] - transCHAN_WIDTH[0],regridVeloRestfrq);
					lDouble loEdge = vopt(transNewXin[0] + transCHAN_WIDTH[0],regridVeloRestfrq);
					cw = abs(upEdge - loEdge);
				}
				lDouble bwUpperEndF = 0.;

				// Start is end in velocity
				if (centerIsStart && !startIsEnd)
				{
					bwUpperEndF = freq_from_vopt(regridCenterVel - (lDouble) nchan * cw * divbytwo,regridVeloRestfrq);
				}
				else
				{
					bwUpperEndF = freq_from_vopt(regridCenterVel + (lDouble) nchan * cw * divbytwo,regridVeloRestfrq);
				}

				regridBandwidthF = abs(bwUpperEndF - regridCenterF) / divbytwo;

				// Can convert start to center
				if (centerIsStart)
				{
					if (startIsEnd)
					{
						regridCenterVel = regridCenterVel + (lDouble) nchan * cw / 2.;
					}
					else
					{
						regridCenterVel = regridCenterVel - (lDouble) nchan * cw / 2.;
					}

					regridCenterF = freq_from_vopt(regridCenterVel,regridVeloRestfrq);
					centerIsStart = false;
				}
				nchan = 0; // indicate that nchan should not be used in the following
			}
			else if (regridBandwidth > 0.)
			{
				// can convert start to center
				if (centerIsStart)
				{
					if (startIsEnd)
					{
						regridCenterVel = regridCenter + regridBandwidth / 2.;
					}
					else
					{
						regridCenterVel = regridCenter - regridBandwidth / 2.;
					}
					regridCenterF = freq_from_vopt(regridCenterVel, regridVeloRestfrq);
					centerIsStart = false;
				}

				lDouble bwUpperEndF = freq_from_vopt(regridCenterVel - regridBandwidth / 2.,regridVeloRestfrq);
				regridBandwidthF = 2. * (bwUpperEndF - regridCenterF);
			}

			if (regridChanWidth > 0. && regridChanWidthF < 0.)
			{
				lDouble chanUpperEdgeF = freq_from_vopt(regridCenterVel - regridChanWidth / 2.,regridVeloRestfrq);
				regridChanWidthF = 2. * (chanUpperEdgeF - freq_from_vopt(regridCenterVel, regridVeloRestfrq));
			}
		}
		else if (regridQuant == "freq")
		{
			// width parameter overrides regridChanWidth
			if (width > 0)
			{
				regridChanWidth = width * transCHAN_WIDTH[0];
			}

			if (start >= 0)
			{
				Int firstChan = start;
				if (start >= (Int) transNewXin.size())
				{
					oss << " *** Parameter start exceeds total number of channels which is "
						<< transNewXin.size() << ". Set to 0." << endl;
					firstChan = 0;
					startIsEnd = false;
				}

				if (startIsEnd)
				{
					regridCenter = transNewXin[firstChan] + transCHAN_WIDTH[firstChan] / 2.;
				}
				else
				{
					regridCenter = transNewXin[firstChan] - transCHAN_WIDTH[firstChan] / 2.;
				}
				centerIsStart = true;
			}
			else
			{
				// start is the center of the first channel
				if (centerIsStart)
				{
					Double tcWidth;
					if (regridChanWidth > 0.)
					{
						tcWidth = regridChanWidth;
					}
					else
					{
						tcWidth = transCHAN_WIDTH[0];
					}

					if (startIsEnd)
					{
						regridCenter += tcWidth / 2.;
					}
					else
					{
						regridCenter -= tcWidth / 2.;
					}
				}
			}
			regridCenterF = regridCenter;
			regridBandwidthF = regridBandwidth;
			regridChanWidthF = regridChanWidth;
		}
		else if (regridQuant == "wave") {
			// wavelength ...
			lDouble regridCenterWav;
			if (regridCenter > 0.)
			{
				if (centerIsStart)
				{
					Double tcWidth;
					if (regridChanWidth > 0.)
					{
						tcWidth = regridChanWidth;
					}
					else
					{
						tcWidth = lambda(transNewXin[0] - transCHAN_WIDTH[0] / 2.) -
								lambda(transNewXin[0] + transCHAN_WIDTH[0]/ 2.);
					}

					// start is the center of the last channel (in freq)
					if (startIsEnd)
					{
						regridCenter -= tcWidth / 2.;
					}
					// start is the center of the first channel (in freq)
					else
					{
						regridCenter += tcWidth / 2.;
					}
				}
				regridCenterF = freq_from_lambda(regridCenter);
				regridCenterWav = regridCenter;
			}
			// center was not specified
			else
			{
				regridCenterF = (transNewXin[0] + transNewXin[oldNUM_CHAN - 1])/ 2.;
				regridCenterWav = lambda(regridCenterF);
				centerIsStart = false;
			}
			if (nchan > 0)
			{
				lDouble cw;
				lDouble divbytwo = 0.5;
				if (centerIsStart)
				{
					divbytwo = 1.;
				}

				if (regridChanWidth > 0.)
				{
					cw = regridChanWidth;
				}
				// Determine channel width from first channel
				else
				{
					lDouble upEdge = lambda(transNewXin[0] - transCHAN_WIDTH[0]);
					lDouble loEdge = lambda(transNewXin[0] + transCHAN_WIDTH[0]);
					cw = abs(upEdge - loEdge);
				}

				lDouble bwUpperEndF = 0.;
				if (centerIsStart && !startIsEnd)
				{
					bwUpperEndF = freq_from_lambda(regridCenterWav - (lDouble) nchan * cw * divbytwo);
				}
				else
				{
					bwUpperEndF = freq_from_lambda(regridCenterWav + (lDouble) nchan * cw * divbytwo);
				}

				regridBandwidthF = (bwUpperEndF - regridCenterF) / divbytwo;

				// Can convert start to center
				if (centerIsStart)
				{
					if (startIsEnd)
					{
						regridCenterWav = regridCenterWav + (lDouble) nchan* cw / 2.;
					}
					else
					{
						regridCenterWav = regridCenterWav - (lDouble) nchan* cw / 2.;
					}
					regridCenterF = freq_from_lambda(regridCenterWav);
					centerIsStart = false;
				}
				nchan = 0; // indicate that nchan should not be used in the following

			}
			else if (regridBandwidth > 0. && regridBandwidth / 2.< regridCenterWav)
			{
				// Can convert start to center
				if (centerIsStart)
				{
					if (startIsEnd)
					{
						regridCenterWav = regridCenter + regridBandwidth / 2.;
					}
					else
					{
						regridCenterWav = regridCenter - regridBandwidth / 2.;
					}
					regridCenterF = freq_from_lambda(regridCenterWav);
					centerIsStart = false;
				}
				lDouble bwUpperEndF = lambda(regridCenterWav - regridBandwidth / 2.);
				regridBandwidthF = 2. * (bwUpperEndF - regridCenterF);
			}

			if (regridChanWidth > 0. && regridChanWidth / 2. < regridCenterWav)
			{
				lDouble chanUpperEdgeF = lambda(regridCenterWav - regridChanWidth / 2.);
				regridChanWidthF = 2. * (chanUpperEdgeF - regridCenterF);
			}
		}
		else
		{
			oss << "Invalid value " << regridQuant << " for parameter \"mode\".";
			message = oss.str();
			return false;
		}
		// (transformation of regrid parameters to frequencies completed)

		// Then determine the actually possible parameters
		lDouble theRegridCenterF;
		lDouble theRegridBWF;
		lDouble theCentralChanWidthF;

		// For vrad and vopt also need to keep this adjusted value
		lDouble theChanWidthX = -1.;

		if (regridCenterF < 0.) // means "not set"
		{
			// Keep regrid center as it is in the data
			theRegridCenterF = (transNewXin[0] - transCHAN_WIDTH[0] / 2.
								+ transNewXin[oldNUM_CHAN - 1] + transCHAN_WIDTH[oldNUM_CHAN - 1] / 2.) / 2.;
			centerIsStart = false;
		}
		// regridCenterF was set
		else
		{
			// Keep center in limits
			theRegridCenterF = regridCenterF;
			if ((theRegridCenterF - (transNewXin[oldNUM_CHAN - 1] + transCHAN_WIDTH[oldNUM_CHAN - 1] / 2.)) > 1.)  // 1 Hz tolerance
			{
				oss << "*** Requested center of SPW " << theRegridCenterF
						<< " Hz is too large by "
						<< theRegridCenterF - transNewXin[oldNUM_CHAN - 1] + transCHAN_WIDTH[oldNUM_CHAN - 1] / 2.
						<< " Hz\n";
				theRegridCenterF = transNewXin[oldNUM_CHAN - 1] + transCHAN_WIDTH[oldNUM_CHAN - 1] / 2.;
				oss << "*** Reset to maximum possible value " << theRegridCenterF << " Hz";
			}
			else if (theRegridCenterF < (transNewXin[0] - transCHAN_WIDTH[0] / 2.))
			{
				Double diff = (transNewXin[0] - transCHAN_WIDTH[0] / 2.) - theRegridCenterF;

				// Cope with numerical accuracy problems
				if (diff > 1.)
				{
					oss << "*** Requested center of SPW (" << theRegridCenterF
						<< " Hz) is smaller than minimum possible value";
					oss << " by " << diff << " Hz";
				}

				theRegridCenterF = transNewXin[0] - transCHAN_WIDTH[0] / 2.;
				if (diff > 1.)
				{
					oss << "\n*** Reset to minimum possible value " << theRegridCenterF << " Hz";
				}
			}
		}

		if (regridBandwidthF <= 0. || nchan != 0) // "not set" or use nchan instead
		{
			// Keep bandwidth as is
			theRegridBWF = transNewXin[oldNUM_CHAN - 1] - transNewXin[0]
			               	+ transCHAN_WIDTH[0] / 2. + transCHAN_WIDTH[oldNUM_CHAN - 1] / 2.;

			// Use nchan parameter if available
			if (nchan != 0)
			{
				if (nchan < 0)
				{
					if (regridQuant == "freq" || regridQuant == "vrad")  // i.e. equidistant in freq
					{
						// Define via width of first channel to avoid numerical problems

						// channel width not set
						if (regridChanWidthF <= 0.)
						{
							theRegridBWF = transCHAN_WIDTH[0] *
									floor((theRegridBWF + transCHAN_WIDTH[0] * 0.01)/ transCHAN_WIDTH[0]);
						}
						else
						{
							theRegridBWF = regridChanWidthF *
									floor((theRegridBWF + regridChanWidthF * 0.01)/ regridChanWidthF);
						}
					}
				}
				// Channel width not set
				else if (regridChanWidthF <= 0.)
				{
					theRegridBWF = transCHAN_WIDTH[0] * nchan;
				}
				else
				{
					theRegridBWF = regridChanWidthF * nchan;
				}

				// Center was not set by user but calculated
				if (regridCenterF <= 0. || regridCenter < -C::c)
				{
					// Need to update
					theRegridCenterF = transNewXin[0] - transCHAN_WIDTH[0] / 2. + theRegridBWF / 2.;
					centerIsStart = false;
				}
				// Center but not nchan was set by user
				else if (nchan < 0)
				{
					// Verify that the bandwidth is correct
					if (centerIsStart)
					{
						if (startIsEnd)
						{
							theRegridBWF = theRegridCenterF - transNewXin[0] + transCHAN_WIDTH[0] / 2.;
						}
						// start is start
						else
						{
							theRegridBWF = transNewXin[oldNUM_CHAN - 1] +
									transCHAN_WIDTH[oldNUM_CHAN - 1] / 2. -
									theRegridCenterF;
						}
						if (regridQuant == "freq" || regridQuant == "vrad") // i.e. equidistant in freq
						{
							// Define via width of first channel to avoid numerical problems
							if (regridChanWidthF <= 0.) { // channel width not set
								theRegridBWF = transCHAN_WIDTH[0]
								               * floor((theRegridBWF + transCHAN_WIDTH[0]* 0.01) / transCHAN_WIDTH[0]);
							}
							else
							{
								theRegridBWF = regridChanWidthF
										* floor((theRegridBWF+ regridChanWidthF* 0.01)/ regridChanWidthF);
							}
						}
					}
					// center is center
					else {
						theRegridBWF = 2. * min((Double) (theRegridCenterF - transNewXin[0]- transCHAN_WIDTH[0]),
								(Double) (transNewXin[oldNUM_CHAN - 1] + transCHAN_WIDTH[oldNUM_CHAN - 1] - theRegridCenterF));
					}
				}
			}

			// Now can convert start to center
			if (centerIsStart)
			{
				if (startIsEnd)
				{
					theRegridCenterF = theRegridCenterF - theRegridBWF / 2.;
				}
				else
				{
					theRegridCenterF = theRegridCenterF + theRegridBWF / 2.;
				}
				centerIsStart = false;
			}
		}
		// regridBandwidthF was set
		else {
			// Determine actually possible bandwidth
			// width will be truncated to the maximum width possible
			// symmetrically around the value given by "regrid_center"
			theRegridBWF = regridBandwidthF;

			// Now can convert start to center
			if (centerIsStart)
			{
				if (startIsEnd)
				{
					theRegridCenterF = theRegridCenterF - theRegridBWF / 2.;
				}
				else
				{
					theRegridCenterF = theRegridCenterF + theRegridBWF / 2.;
				}
				centerIsStart = false;
			}

			Double rangeTol = 1.; // Hz
			if ((regridQuant == "vopt" || regridQuant == "wave")) // i.e. if the center is the center w.r.t. wavelength
			{
				rangeTol = transCHAN_WIDTH[0];
			}

			if ((theRegridCenterF + theRegridBWF / 2.) - (transNewXin[oldNUM_CHAN - 1] + transCHAN_WIDTH[oldNUM_CHAN - 1] / 2.) > rangeTol)
			{
				oss	<< " *** Input spectral window exceeds upper end of original window. "
						"Adjusting to max. possible value." << endl;
				theRegridBWF = min(	(Double) fabs(transNewXin[oldNUM_CHAN - 1] + transCHAN_WIDTH[oldNUM_CHAN - 1] / 2.- theRegridCenterF),
									(Double) fabs(theRegridCenterF - transNewXin[0]+ transCHAN_WIDTH[0] / 2.)) * 2.;

				if (theRegridBWF < transCHAN_WIDTH[0])
				{
					theRegridCenterF = (	transNewXin[0]
					                        + transCHAN_WIDTH[oldNUM_CHAN - 1]
					                        + transCHAN_WIDTH[oldNUM_CHAN - 1] / 2.
					                        - transCHAN_WIDTH[0] / 2.) / 2.;
					theRegridBWF = transCHAN_WIDTH[oldNUM_CHAN - 1]
					               - transNewXin[0] + transCHAN_WIDTH[oldNUM_CHAN - 1] / 2. + transCHAN_WIDTH[0] / 2.;
				}
			}
			if ((theRegridCenterF - theRegridBWF / 2.) - (transNewXin[0] - transCHAN_WIDTH[0] / 2.) < -rangeTol)
			{
				oss << " *** Input spectral window exceeds lower end of original window. "
						" Adjusting to max. possible value."<< endl;

				theRegridBWF = min( (Double) fabs(transNewXin[oldNUM_CHAN - 1] + transCHAN_WIDTH[oldNUM_CHAN - 1] / 2. - theRegridCenterF),
									(Double) fabs(theRegridCenterF - transNewXin[0] + transCHAN_WIDTH[0] / 2.)) * 2.;

				if (theRegridBWF < transCHAN_WIDTH[0])
				{
					theRegridCenterF = (transNewXin[0]
					                    + transCHAN_WIDTH[oldNUM_CHAN - 1]
					                    + transCHAN_WIDTH[oldNUM_CHAN - 1] / 2.
					                    - transCHAN_WIDTH[0] / 2.) / 2.;
					theRegridBWF = transCHAN_WIDTH[oldNUM_CHAN - 1]
					               - transNewXin[0]
					               + transCHAN_WIDTH[oldNUM_CHAN - 1] / 2.
					               + transCHAN_WIDTH[0] / 2.;
				}
			}
		}

		if (regridChanWidthF <= 0.)  // "not set"
		{
			if (nchan != 0 || centerIsStartC) // use first channel
			{
				theCentralChanWidthF = transCHAN_WIDTH[0];
			}
			else
			{
				// keep channel width similar to the old one
				theCentralChanWidthF = transCHAN_WIDTH[oldNUM_CHAN / 2]; // use channel width from
				// near central channel
			}
		}
		// regridChanWidthF was set
		else
		{
			// Keep in limits
			theCentralChanWidthF = regridChanWidthF;

			// Too large => make a single channel
			if (theCentralChanWidthF > theRegridBWF)
			{
				theCentralChanWidthF = theRegridBWF;
				oss
						<< " *** Requested new channel width exceeds defined SPW width."
						<< endl
						<< "     Creating a single channel with the defined SPW width."
						<< endl;
			}
			// Check if too small
			else if (theCentralChanWidthF < transCHAN_WIDTH[0])
			{
				// Determine smallest channel width
				lDouble smallestChanWidth = 1E30;
				Int ii = 0;
				for (Int i = 0; i < oldNUM_CHAN; i++)
				{
					if (transCHAN_WIDTH[i] < smallestChanWidth)
					{
						smallestChanWidth = transCHAN_WIDTH[i];
						ii = i;
					}
				}

				// 1 Hz tolerance to cope with numerical accuracy problems
				if (theCentralChanWidthF < smallestChanWidth - 1.)
				{
					oss << " *** Requested new channel width (" << theCentralChanWidthF << " Hz) "
					    << "is smaller than smallest original channel width" << endl;
					oss << "     which is " << smallestChanWidth << " Hz" << endl;

					if (regridQuant == "vrad")
					{
						oss << "     or "
							<< (vrad(transNewXin[ii],regridVeloRestfrq)
							  - vrad(transNewXin[ii] + transCHAN_WIDTH[ii] / 2.,regridVeloRestfrq)) * 2. << " m/s";
					}

					if (regridQuant == "vopt")
					{
						oss << "     or "
						    << (vopt(transNewXin[ii],regridVeloRestfrq)
							  - vopt(transNewXin[ii] + transCHAN_WIDTH[ii] / 2.,regridVeloRestfrq)) * 2. << " m/s";
					}

					message = oss.str();
					return false;

				}
				// input channel width was OK, memorize
				else
				{
					theChanWidthX = regridChanWidth;
				}
			}
		}

		oss << " Channels equidistant in " << regridQuant << endl
			<< " Central frequency (in output frame) = " << theRegridCenterF << " Hz";

		if (regridQuant == "vrad")
		{
			oss << " == " << vrad(theRegridCenterF, regridVeloRestfrq) << " m/s radio velocity";
		}
		else if (regridQuant == "vopt")
		{
			oss << " == " << vopt(theRegridCenterF, regridVeloRestfrq) << " m/s optical velocity";
		}
		else if (regridQuant == "wave")
		{
			oss << " == " << lambda(theRegridCenterF) << " m wavelength";
		}
		oss << endl;

		if (isDescending)
		{
			oss << " Channel central frequency is decreasing with increasing channel number." << endl;
		}

		oss << " Width of central channel (in output frame) = " << theCentralChanWidthF << " Hz";

		if (regridQuant == "vrad")
		{
			oss << " == " << vrad(theRegridCenterF - theCentralChanWidthF,regridVeloRestfrq)
					       - vrad(theRegridCenterF,regridVeloRestfrq) << " m/s radio velocity";
		}
		else if (regridQuant == "vopt")
		{
			oss << " == " << vopt(theRegridCenterF - theCentralChanWidthF,regridVeloRestfrq)
					       - vopt(theRegridCenterF,regridVeloRestfrq) << " m/s optical velocity";
		}
		else if (regridQuant == "wave")
		{
			oss << " == " << lambda(theRegridCenterF - theCentralChanWidthF)
					       - lambda(theRegridCenterF) << " m wavelength";
		}
		oss << endl;

		// Now calculate newChanLoBound, and newChanHiBound from theRegridCenterF, theRegridBWF, theCentralChanWidthF
		vector<lDouble> loFBup; // The lower bounds for the new channels starting from the central channel going up
		vector<lDouble> hiFBup; // The lower bounds for the new channels starting from the central channel going up
		vector<lDouble> loFBdown; // The lower bounds for the new channels starting from the central channel going down
		vector<lDouble> hiFBdown; // The lower bounds for the new channels starting from the central channel going down

		lDouble edgeTolerance = theCentralChanWidthF * 0.01; // Needed to avoid numerical accuracy problems

		// Re-gridding in radio velocity ...
		if (regridQuant == "vrad")
		{
			// Create freq boundaries equidistant and contiguous in radio velocity
			lDouble upperEndF = theRegridCenterF + theRegridBWF / 2.;
			lDouble lowerEndF = theRegridCenterF - theRegridBWF / 2.;
			lDouble upperEndV = vrad(upperEndF, regridVeloRestfrq);
			lDouble lowerEndV = vrad(lowerEndF, regridVeloRestfrq);
			lDouble velLo;
			lDouble velHi;

			// Want to keep the center of the center channel at the center
			// of the new center channel if the bandwidth is an odd multiple
			// of the new channel width, otherwise the center channel is the
			// lower edge of the new center channel
			lDouble tnumChan = floor((theRegridBWF + edgeTolerance) / theCentralChanWidthF);

			if ((Int) tnumChan % 2 != 0)
			{
				// Odd multiple
				loFBup.push_back(theRegridCenterF - theCentralChanWidthF / 2.);
				hiFBup.push_back(theRegridCenterF + theCentralChanWidthF / 2.);
				loFBdown.push_back(theRegridCenterF - theCentralChanWidthF / 2.);
				hiFBdown.push_back(theRegridCenterF + theCentralChanWidthF / 2.);
			}
			else
			{
				loFBup.push_back(theRegridCenterF);
				hiFBup.push_back(theRegridCenterF + theCentralChanWidthF);
				loFBdown.push_back(theRegridCenterF);
				hiFBdown.push_back(theRegridCenterF + theCentralChanWidthF);
			}

			// Cannot use original channel width in velocity units
			if (theChanWidthX < 0)
			{
				// Need to calculate back from central channel width in Hz
				theChanWidthX = vrad(loFBup[0], regridVeloRestfrq) - vrad(hiFBup[0], regridVeloRestfrq);
			}

			// calc velocity corresponding to the upper end (in freq) of the
			// last added channel which is the lower end of the next channel
			velLo = vrad(hiFBup[0], regridVeloRestfrq);

			// calc velocity corresponding to the upper end (in freq) of the next channel
			velHi = velLo - theChanWidthX; // vrad goes down as freq goes up!

			// (Preventing accuracy problems)
			while (upperEndV - theChanWidthX / 10. < velHi)
			{
				// calc frequency of the upper end (in freq) of the next channel
				lDouble freqHi = freq_from_vrad(velHi, regridVeloRestfrq);

				// End of bandwidth not yet reached
				if (freqHi <= upperEndF + edgeTolerance)
				{
					loFBup.push_back(hiFBup.back());
					hiFBup.push_back(freqHi);
				}
				else if (freqHi < upperEndF + edgeTolerance)
				{
					loFBup.push_back(hiFBup.back());
					hiFBup.push_back(upperEndF);
					break;
				}
				else
				{
					break;
				}

				// calc velocity corresponding to the upper end (in freq) of the added channel
				velLo = vrad(hiFBup.back(), regridVeloRestfrq);

				// calc velocity corresponding to the upper end (in freq) of the next channel
				velHi = velLo - theChanWidthX; // vrad goes down as freq goes up
			}

			// calc velocity corresponding to the lower end (in freq) of the
			// Last added channel which is the upper end of the next channel
			velHi = vrad(loFBdown[0], regridVeloRestfrq);

			// calc velocity corresponding to the lower end (in freq) of the next channel
			velLo = velHi + theChanWidthX; // vrad goes up as freq goes down!

			// (Preventing accuracy problems)
			while (velLo < lowerEndV + theChanWidthX / 10.)
			{
				// calc frequency of the lower end (in freq) of the next channel
				lDouble freqLo = freq_from_vrad(velLo, regridVeloRestfrq);

				// End of bandwidth not yet reached
				if (freqLo >= lowerEndF - edgeTolerance)
				{
					hiFBdown.push_back(loFBdown.back());
					loFBdown.push_back(freqLo);
				}
				else if (freqLo > lowerEndF - edgeTolerance)
				{
					hiFBdown.push_back(loFBdown.back());
					loFBdown.push_back(lowerEndF);
					break;
				}
				else
				{
					break;
				}

				// calc velocity corresponding to the upper end of the next channel
				velHi = vrad(loFBdown.back(), regridVeloRestfrq);

				// calc velocity corresponding to the lower end (in freq) of the next channel
				velLo = velHi + theChanWidthX; // vrad goes up as freq goes down
			}
		}
		// Regridding in optical velocity ...
		else if (regridQuant == "vopt")
		{
			// Create freq boundaries equidistant and contiguous in optical velocity
			lDouble upperEndF = theRegridCenterF + theRegridBWF / 2.;
			lDouble lowerEndF = theRegridCenterF - theRegridBWF / 2.;
			lDouble upperEndV = vopt(upperEndF, regridVeloRestfrq);
			lDouble lowerEndV = vopt(lowerEndF, regridVeloRestfrq);
			lDouble velLo;
			lDouble velHi;

			// Want to keep the center of the center channel at the center
			// of the new center channel if the bandwidth is an odd multiple
			// of the new channel width, otherwise the center channel is the
			// lower edge of the new center channel

			// Enlarged edge tolerance since channels non-equidistant in freq
			lDouble tnumChan = floor((theRegridBWF + edgeTolerance) / theCentralChanWidthF);

			// Odd multiple
			if ((Int) tnumChan % 2 != 0)
			{

				loFBup.push_back(theRegridCenterF - theCentralChanWidthF / 2.);
				hiFBup.push_back(theRegridCenterF + theCentralChanWidthF / 2.);
				loFBdown.push_back(theRegridCenterF - theCentralChanWidthF / 2.);
				hiFBdown.push_back(theRegridCenterF + theCentralChanWidthF / 2.);
			}
			else
			{
				loFBup.push_back(theRegridCenterF);
				hiFBup.push_back(theRegridCenterF + theCentralChanWidthF);
				loFBdown.push_back(theRegridCenterF);
				hiFBdown.push_back(theRegridCenterF + theCentralChanWidthF);
			}

			// Cannot use original channel width in velocity units
			if (theChanWidthX < 0)
			{
				// Need to calculate back from central channel width in Hz
				theChanWidthX = vopt(loFBup[0], regridVeloRestfrq) - vopt(hiFBup[0], regridVeloRestfrq);
			}

			// calc velocity corresponding to the upper end (in freq) of the
			// last added channel which is the lower end of the next channel
			velLo = vopt(hiFBup[0], regridVeloRestfrq);

			// calc velocity corresponding to the upper end (in freq) of the next channel
			velHi = velLo - theChanWidthX; // vopt goes down as freq goes up!

			// (Preventing accuracy problems)
			while (upperEndV - velHi < theChanWidthX / 10.)
			{
				// calc frequency of the upper end (in freq) of the next channel
				lDouble freqHi = freq_from_vopt(velHi, regridVeloRestfrq);

				// End of bandwidth not yet reached
				if (freqHi <= upperEndF + edgeTolerance)
				{
					loFBup.push_back(hiFBup.back());
					hiFBup.push_back(freqHi);
				}
				else if (freqHi < upperEndF + edgeTolerance)
				{
					loFBup.push_back(hiFBup.back());
					hiFBup.push_back(upperEndF);
					break;
				}
				else
				{
					break;
				}

				// calc velocity corresponding to the upper end (in freq) of the added channel
				velLo = vopt(hiFBup.back(), regridVeloRestfrq);
				// calc velocity corresponding to the upper end (in freq) of the next channel
				velHi = velLo - theChanWidthX; // vopt goes down as freq goes up
			}

			// calc velocity corresponding to the lower end (in freq) of the
			// last added channel which is the upper end of the next channel
			velHi = vopt(loFBdown[0], regridVeloRestfrq);

			// calc velocity corresponding to the lower end (in freq) of the next channel
			velLo = velHi + theChanWidthX; // vopt goes up as freq goes down!

			// (Preventing accuracy problems)
			while (velLo - lowerEndV < theChanWidthX / 10.)
			{
				// calc frequency of the lower end (in freq) of the next channel
				lDouble freqLo = freq_from_vopt(velLo, regridVeloRestfrq);

				// End of bandwidth not yet reached
				if (freqLo >= lowerEndF - edgeTolerance)
				{
					hiFBdown.push_back(loFBdown.back());
					loFBdown.push_back(freqLo);
				}
				else if (freqLo > lowerEndF - edgeTolerance)
				{
					hiFBdown.push_back(loFBdown.back());
					loFBdown.push_back(lowerEndF);
					break;
				}
				else
				{
					break;
				}

				// calc velocity corresponding to the upper end of the next channel
				velHi = vopt(loFBdown.back(), regridVeloRestfrq);
				// calc velocity corresponding to the lower end (in freq) of the next channel
				velLo = velHi + theChanWidthX; // vopt goes up as freq goes down
			}
		}
		// Re-gridding in frequency  ...
		else if (regridQuant == "freq")
		{
			// Create freq boundaries equidistant and contiguous in frequency
			lDouble upperEndF = theRegridCenterF + theRegridBWF / 2.;
			lDouble lowerEndF = theRegridCenterF - theRegridBWF / 2.;

			// Want to keep the center of the center channel at the center
			// of the new center channel if the bandwidth is an odd multiple
			// of the new channel width, otherwise the center channel is the
			// lower edge of the new center channel
			lDouble tnumChan = floor((theRegridBWF + edgeTolerance) / theCentralChanWidthF);

			// Odd multiple
			if ((Int) tnumChan % 2 != 0)
			{
				loFBup.push_back(theRegridCenterF - theCentralChanWidthF / 2.);
				hiFBup.push_back(theRegridCenterF + theCentralChanWidthF / 2.);
				loFBdown.push_back(theRegridCenterF - theCentralChanWidthF / 2.);
				hiFBdown.push_back(theRegridCenterF + theCentralChanWidthF / 2.);
			}
			else
			{
				loFBup.push_back(theRegridCenterF);
				hiFBup.push_back(theRegridCenterF + theCentralChanWidthF);
				loFBdown.push_back(theRegridCenterF);
				hiFBdown.push_back(theRegridCenterF + theCentralChanWidthF);
			}

			while (hiFBup.back() < upperEndF + edgeTolerance)
			{
				// calc frequency of the upper end of the next channel
				lDouble freqHi = hiFBup.back() + theCentralChanWidthF;

				// End of bandwidth not yet reached
				if (freqHi <= upperEndF + edgeTolerance)
				{
					loFBup.push_back(hiFBup.back());
					hiFBup.push_back(freqHi);
				}
				else
				{
					break;
				}
			}

			while (loFBdown.back() > lowerEndF - edgeTolerance)
			{
				// calc frequency of the lower end of the next channel
				lDouble freqLo = loFBdown.back() - theCentralChanWidthF;

				// End of bandwidth not yet reached
				if (freqLo >= lowerEndF - edgeTolerance)
				{
					hiFBdown.push_back(loFBdown.back());
					loFBdown.push_back(freqLo);
				}
				else
				{
					break;
				}
			}
		}
		// Re-gridding in wavelength  ...
		else if (regridQuant == "wave")
		{
			// Create freq boundaries equidistant and contiguous in wavelength
			lDouble upperEndF = theRegridCenterF + theRegridBWF / 2.;
			lDouble lowerEndF = theRegridCenterF - theRegridBWF / 2.;
			lDouble upperEndL = lambda(upperEndF);
			lDouble lowerEndL = lambda(lowerEndF);
			lDouble lambdaLo;
			lDouble lambdaHi;

			// Want to keep the center of the center channel at the center
			// of the new center channel if the bandwidth is an odd multiple
			// of the new channel width, otherwise the center channel is the
			// lower edge of the new center channel
			lDouble tnumChan = floor((theRegridBWF + edgeTolerance) / theCentralChanWidthF);

			// Odd multiple
			if ((Int) tnumChan % 2 != 0)
			{
				loFBup.push_back(theRegridCenterF - theCentralChanWidthF / 2.);
				hiFBup.push_back(theRegridCenterF + theCentralChanWidthF / 2.);
				loFBdown.push_back(theRegridCenterF - theCentralChanWidthF / 2.);
				hiFBdown.push_back(theRegridCenterF + theCentralChanWidthF / 2.);
			}
			else
			{
				loFBup.push_back(theRegridCenterF);
				hiFBup.push_back(theRegridCenterF + theCentralChanWidthF);
				loFBdown.push_back(theRegridCenterF);
				hiFBdown.push_back(theRegridCenterF + theCentralChanWidthF);
			}

			// Cannot use original channel width in wavelength units
			if (theChanWidthX < 0)
			{
				// Need to calculate back from central channel width in Hz
				theChanWidthX = lambda(loFBup[0]) - lambda(hiFBup[0]);
			}

			// calc wavelength corresponding to the upper end (in freq) of the
			// last added channel which is the lower end of the next channel
			lambdaLo = lambda(hiFBup[0]);

			// calc wavelength corresponding to the upper end (in freq) of the next channel
			lambdaHi = lambdaLo - theChanWidthX; // lambda goes down as freq goes up!

			// (Preventing accuracy problems)
			while (upperEndL - lambdaHi < theChanWidthX / 10.)
			{
				// calc frequency of the upper end (in freq) of the next channel
				lDouble freqHi = freq_from_lambda(lambdaHi);

				// End of bandwidth not yet reached
				if (freqHi <= upperEndF + edgeTolerance)
				{
					loFBup.push_back(hiFBup.back());
					hiFBup.push_back(freqHi);
				}
				else if (freqHi < upperEndF + edgeTolerance)
				{
					loFBup.push_back(hiFBup.back());
					hiFBup.push_back(upperEndF);
					break;
				}
				else
				{
					break;
				}

				// calc wavelength corresponding to the upper end (in freq) of the added channel
				lambdaLo = lambda(hiFBup.back());
				// calc wavelength corresponding to the upper end (in freq) of the next channel
				lambdaHi = lambdaLo - theChanWidthX; // lambda goes down as freq goes up
			}

			// calc wavelength corresponding to the lower end (in freq) of the
			// last added channel which is the upper end of the next channel
			lambdaHi = lambda(loFBdown[0]);

			// calc wavelength corresponding to the lower end (in freq) of the next channel
			lambdaLo = lambdaHi + theChanWidthX; // lambda goes up as freq goes down!


			// (Preventing accuracy problems)
			while (lambdaLo - lowerEndL < theChanWidthX / 10.)
			{
				// calc frequency of the lower end (in freq) of the next channel
				lDouble freqLo = freq_from_lambda(lambdaLo);

				// End of bandwidth not yet reached
				if (freqLo >= lowerEndF - edgeTolerance)
				{
					hiFBdown.push_back(loFBdown.back());
					loFBdown.push_back(freqLo);
				}
				else if (freqLo > lowerEndF - edgeTolerance)
				{
					hiFBdown.push_back(loFBdown.back());
					loFBdown.push_back(lowerEndF);
					break;
				}
				else
				{
					break;
				}

				// calc wavelength corresponding to the upper end of the next channel
				lambdaHi = lambda(loFBdown.back());
				// calc wavelength corresponding to the lower end (in freq) of the next channel
				lambdaLo = lambdaHi + theChanWidthX; // wavelength goes up as freq goes down
			}

		}
		 // should not get here
		else
		{
			oss << "Invalid value " << regridQuant << " for parameter \"mode\".";
			message = oss.str();
			return false;
		}

		Int numNewChanDown = loFBdown.size();
		Int numNewChanUp = loFBup.size();

		// central channel contained in both vectors
		newChanLoBound.resize(numNewChanDown + numNewChanUp - 1);

		newChanHiBound.resize(numNewChanDown + numNewChanUp - 1);
		for (Int i = 0; i < numNewChanDown; i++)
		{
			Int k = numNewChanDown - i - 1; // Need to assign in reverse
			newChanLoBound[i] = loFBdown[k];
			newChanHiBound[i] = hiFBdown[k];
		}
		for (Int i = 1; i < numNewChanUp; i++) // Start at 1 to omit the central channel here
		{
			newChanLoBound[i + numNewChanDown - 1] = loFBup[i];
			newChanHiBound[i + numNewChanDown - 1] = hiFBup[i];
		}

		uInt nc = newChanLoBound.size();
		oss << " Number of channels = " << nc << endl;
		oss << " Total width of SPW (in output frame) = " << newChanHiBound[nc - 1] - newChanLoBound[0] << " Hz" << endl;
		oss << " Lower edge = " << newChanLoBound[0] << " Hz,"
			<< " upper edge = " << newChanHiBound[nc - 1] << " Hz" << endl;

		// Original SPW was in reverse order, need to restore that
		if (isDescending)
		{
			Vector<Double> tempL, tempU;
			tempL.assign(newChanLoBound);
			tempU.assign(newChanHiBound);
			for (uInt i = 0; i < nc; i++)
			{
				newChanLoBound(i) = tempL(nc - 1 - i);
				newChanHiBound(i) = tempU(nc - 1 - i);
			}
		}

		message = oss.str();

		return true;

	} // end if (regridQuant== ...
}

} //# NAMESPACE CASA - END
