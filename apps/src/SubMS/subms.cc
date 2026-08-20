// # Copyright (C) 2021, 2026
// # Associated Universities, Inc. Washington DC, USA.
// #
// # This library is free software; you can redistribute it and/or modify it
// # under the terms of the GNU Library General Public License as published by
// # the Free Software Foundation; either version 2 of the License, or (at your
// # option) any later version.
// #
// # This library is distributed in the hope that it will be useful, but WITHOUT
// # ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// # FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
// # License for more details.is
// #
// # You should have received a copy of the GNU Library General Public License
// # along with this library; if not, write to the Free Software Foundation,
// # Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
// #
// # Correspondence concerning this should be addressed as follows:
// #        Postal address: National Radio Astronomy Observatory
// #                        1003 Lopezville Road,
// #                        Socorro, NM - 87801, USA
// #
// # $Id$

#include <subms.h>
//
//-------------------------------------------------------------------------
//
//
//-------------------------------------------------------------------------

/**
 * Parses a spectral window / frequency selection string against a MeasurementSet
 * without applying the selection to the main visibility data rows.
 *
 * @param ms A reference to the open MeasurementSet containing the SPECTRAL_WINDOW subtable.
 * @param spwExpression The raw selection syntax string (e.g., "0:1.41GHz~1.42GHz^2, 1:10~50^5").
 * @param defaultStep The step/stride size to fall back on if omitted in the syntax (default: 1).
 * @param sorted If true, sorts the output matrix by Spectral Window ID (default: true).
 * 
 * @return A Matrix<Int> where each row contains: [SPW_ID, StartChannel, StopChannel, Step].
 * @throws casacore::MSSelectionError if the expression contains syntax errors or out-of-bounds frequencies.
 */
casacore::Matrix<int> parseFrequencySelection(
    const casacore::MeasurementSet& ms,
    const casacore::String& spwExpression,
    int defaultStep = 1,
    bool sorted = true) 
{
    casacore::MSSelection selection;
    
    // Pass the raw expression text into the selection parser
    selection.setSpwExpr(spwExpression);

    // Compute and return the matrix layout using the MS metadata context.
    // The const_cast is safely used here as getChanList requires a non-const pointer 
    // internally but only queries structural sub-tables without altering metadata state.
    return selection.getChanList(const_cast<casacore::MeasurementSet*>(&ms), defaultStep, sorted);
}


void SubMS_func(const string& MSNBuf, const string& OutMSBuf,
		const string& WhichColStr, const bool& deepCopy,
		const string& fieldStr,const string& timeStr,
		const string& spwStr, const string& baselineStr,
		const string& scanStr, const string& arrayStr,
		const string& uvdistStr,const string& taqlStr,
		const float integ, const int chanStep,
		const string& combineStr, const string& corrStr,
		const string& intentStr, const string& obsStr)
{
  if (chanStep < 1)
    throw AipsError("chanStep must be >= 1");
  //
  //---------------------------------------------------
  //
  //  MSSelection msSelection;
  // try
    {
      MeasurementSet ms(MSNBuf,TableLock(TableLock::AutoNoReadLocking));

      if (OutMSBuf != "")
	{
	  //
	  // Damn CASA::Strings!
	  //
	  String OutMSName(OutMSBuf), WhichCol(WhichColStr);
          //
          // Only parse the spwStr to extract the tuple of SpwDI, Start, Stop, Step indices.
          //
          Matrix<int> freqSelection = parseFrequencySelection(ms,spwStr);
          // Currently, chanStep is used for channel averaging.  In
          // (the unlikely) case where chanstep is provided also via
          // the MSSelection string, throw an error if *any* of the
          // specified chansteps are different from the *single*
          // chanStep that casa::SubMS class supports.
          for (uint i=0; i<freqSelection.nrow(); i++)
            if (freqSelection(i,3) != chanStep)
              throw(AipsError("SubMS app: Per SPW chanstep is not supported.  Yet."));
              
	  //	    SubMS splitter(selectedMS);
	  //
	  // SubMS class is not msselection compliant (it's a strange
	  // mix of msselection and selection-by-hand)!
	  //
	  SubMS splitter(ms);
	  Vector<int> step(1, chanStep);
	  String CspwStr(spwStr), CfieldStr(fieldStr), CbaselineStr(baselineStr),
	    CscanStr(scanStr), CuvdistStr(uvdistStr), CtaqlStr(taqlStr), CtimeStr(timeStr),
	    CarrayStr(arrayStr), Ccombine(combineStr), CcorrStr(corrStr),
	    CintentStr(intentStr), CobsStr(obsStr);
	  // step drives per-spw channel averaging, matching CASA task "split"'s width parameter.
	  if (!splitter.setmsselect(CspwStr, CfieldStr, CbaselineStr, CscanStr, CuvdistStr,
			       CtaqlStr, step, CarrayStr, CcorrStr, CintentStr, CobsStr))
	    throw AipsError("SubMS app: casa::SubMS::setmsselect() failed for the given selection");

	  splitter.selectTime(integ,CtimeStr);
	  if (!splitter.makeSubMS(OutMSName, WhichCol, Vector<Int>(1, 0), Ccombine))
	    throw AipsError("SubMS app: casa::SubMS::makeSubMS failed to produce " + OutMSName);
	}
    }
}
