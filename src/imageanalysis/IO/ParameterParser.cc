//# ParameterParser.h
//# Copyright (C) 2007
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or (at your option)
//# any later version.
//#
//# This program is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//# more details.
//#
//# You should have received a copy of the GNU General Public License along
//# with this program; if not, write to the Free Software Foundation, Inc.,
//# 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA

#include <imageanalysis/IO/ParameterParser.h>

#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Utilities/Regex.h>
#include <casacore/casa/Utilities/Sort.h>
#include <casacore/measures/Measures/Stokes.h>
#include <casacore/casa/IO/ArrayIO.h>

using namespace casacore;
namespace casa {

vector<uInt> ParameterParser::consolidateAndOrderRanges(
    uInt& nSelected, const vector<uInt>& ranges
) {
    uInt arrSize = ranges.size()/2;
    uInt arrMin[arrSize];
    uInt arrMax[arrSize];
    for (uInt i=0; i<arrSize; i++) {
        arrMin[i] = ranges[2*i];
        arrMax[i] = ranges[2*i + 1];
    }    
    Sort sort;
    sort.sortKey (arrMin, TpUInt);
    sort.sortKey (arrMax, TpUInt, 0, Sort::Descending);
    Vector<uInt> inxvec;
    vector<uInt> consol(0);
    sort.sort(inxvec, arrSize);
    for (uInt i=0; i<arrSize; i++) {
        uInt idx = inxvec(i);
        uInt size = consol.size();
        uInt min = arrMin[idx];
        uInt max = arrMax[idx];
        uInt lastMax = (i == 0) ? 0 : consol[size-1];
        if (i==0) {
            // consol.resize(2, true);
            // consol[0] = min;
            // consol[1] = max;
            consol.push_back(min);
            consol.push_back(max);
        }
        else if ( 
            // overlaps previous range, so extend
            (min < lastMax && max > lastMax)
            // or contiguous with previous range, so extend
            || min == lastMax + 1
        ) {
            // overwriting the end value, so do not resize
            consol[size-1] = max; 
        }

        else if (min > lastMax + 1) { 
            // non overlap of and not contiguous with previous range,
            // so create new end point pair
            consol.push_back(min);
            consol.push_back(max);
        }
    }    
    nSelected = 0; 
    for (uInt i=0; i<consol.size()/2; i++) {
        nSelected += consol[2*i + 1] - consol[2*i] + 1; 
    }    
    return consol;
}
    
std::vector<uInt> ParameterParser::spectralRangesFromChans(
	uInt& nSelectedChannels, const String& specification,  const uInt nChannels
) {
	// First split on commas
	Vector<String> parts = stringToVector(specification, std::regex("[,;]"));
	static const Regex regexuInt("^[0-9]+$");
	static const Regex regexRange("^[0-9]+[ \n\t\r\v\f]*~[ \n\t\r\v\f]*[0-9]+$");
	static const Regex regexLT("^<.*$");
	static const Regex regexLTEq("^<=.*$");
	static const Regex regexGT("^>.*$");
	static const Regex regexGTEq("^>=.*$");
	vector<uInt> ranges(0);

	for (uInt i=0; i<parts.size(); i++) {
		parts[i].trim();
		uInt min = 0;
		uInt max = 0;
		if (parts[i].matches(regexuInt)) {
			// just one channel
			min = String::toInt(parts[i]);
			max = min;
		}
		else if(parts[i].matches(regexRange)) {
			// a range of channels
			Vector<String> values = stringToVector(parts[i], '~');
			ThrowIf(
				values.size() != 2,
				"Incorrect specification for channel range " + parts[i]
			);
			values[0].trim();
			values[1].trim();
			for(uInt j=0; j < 2; j++) {
				ThrowIf(
					! values[j].matches(regexuInt),
					"For channel specification " + values[j]
					+ " is not a non-negative integer in "
					+ parts[i]
				);
			}
			min = String::toInt(values[0]);
			max = String::toInt(values[1]);
		}
		else if (parts[i].matches(regexLT)) {
			String maxs = parts[i].matches(regexLTEq) ? parts[i].substr(2) : parts[i].substr(1);
			maxs.trim();
			ThrowIf(
				! maxs.matches(regexuInt),
				"In channel specification, " + maxs
					+ " is not a non-negative integer in " + parts[i]
			);
			min = 0;
			max = String::toInt(maxs);
			if (! parts[i].matches(regexLTEq)) {
				ThrowIf(
					max == 0,
					"In channel specification, max channel cannot "
					"be less than zero in " + parts[i]
				);
				max--;
			}
		}
		else if (parts[i].matches(regexGT)) {
			String mins = parts[i].matches(regexGTEq)
	         	? parts[i].substr(2)
	         	: parts[i].substr(1);
			mins.trim();
			ThrowIf(
				! mins.matches(regexuInt),
				"In channel specification, " + mins
					+ " is not an integer in " + parts[i]
			);
			max = nChannels - 1;
			min = String::toInt(mins);
			if(! parts[i].matches(regexGTEq)) {
				min++;
			}
			ThrowIf(
				min > nChannels - 1,
				"Min channel cannot be greater than the (zero-based) number of channels ("
				+ String::toString(nChannels - 1) + ") in the image"
			);
		}
		else {
			ThrowCc(
				"Invalid channel specification in " + parts[i]
			    + " of spec " + specification
			);
		}
		ThrowIf(
			min > max,
			"Min channel " + String::toString(min) + " cannot be greater than max channel "
			+ String::toString(max) + " in " + parts[i]
		);
		ThrowIf(
			max >= nChannels,
			"Zero-based max channel " + String::toString(max)
			+ " must be less than the total number of channels ("
			+ String::toString(nChannels) + ") in the channel specification " + parts[i]
		);
		ranges.push_back(min);
		ranges.push_back(max);
	}
	vector<uInt> consolidatedRanges = consolidateAndOrderRanges(nSelectedChannels, ranges);
	return consolidatedRanges;
}

std::vector<String> ParameterParser::stokesFromString(
	String& specification
) {
	specification.trim();
	specification.ltrim('[');
	specification.rtrim(']');
	specification.upcase();

	Vector<String> parts = stringToVector(specification, std::regex("[,;]"));
	Vector<String> polNames = Stokes::allNames(false);
	uInt nNames = polNames.size();
	Vector<uInt> nameLengths(nNames);
	for (uInt i=0; i<nNames; ++i) {
		nameLengths[i] = polNames[i].length();
	}
	uInt *lengthData = nameLengths.data();

	Vector<uInt> idx(nNames);
	Sort sorter;
	sorter.sortKey(lengthData, TpUInt, 0, Sort::Descending);
	sorter.sort(idx, nNames);

	Vector<String> sortedNames(nNames);
	for (uInt i=0; i<nNames; i++) {
		sortedNames[i] = polNames[idx[i]];
		sortedNames[i].upcase();
	}
	vector<String> myStokes;
	for (uInt i=0; i<parts.size(); i++) {
		String part = parts[i];
		part.trim();
		Vector<String>::iterator iter = sortedNames.begin();
		vector<Int> stokes;
		while (iter != sortedNames.end() && ! part.empty()) {
			if (part.startsWith(*iter)) {
				myStokes.push_back(*iter);
				// consume the string
				part = part.substr(iter->length());
				if (! part.empty()) {
					// reset the iterator to start over at the beginning of the list for
					// the next specified polarization
					iter = sortedNames.begin();
				}
			}
			else {
				++iter;
			}
		}
		ThrowIf(
			! part.empty(),
			"(Sub)String " + part + " in stokes specification part " + parts[i]
			    + " does not match a known polarization."
		);
	}
	return myStokes;
}


}
