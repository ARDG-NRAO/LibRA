//# UVContSubResult.h: implementation of the UVContSubResult class
//#
//#  CASA - Common Astronomy Software Applications (http://casa.nrao.edu/)
//#  Copyright (C) Associated Universities, Inc. Washington DC, USA 2021, All rights reserved.
//#  Copyright (C) European Southern Observatory, 2021, All rights reserved.
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

#include <mstransform/TVI/UVContSubResult.h>

using namespace casacore;

namespace casa { //# NAMESPACE CASA - BEGIN

namespace vi { //# NAMESPACE VI - BEGIN

void UVContSubResult::addOneFit(int field, int scan, int spw, int pol, Complex chiSquared)
{
    auto &polRes = accum[field][scan][spw];
    auto resIt = polRes.find(pol);
    if (resIt == polRes.end()) {
        FitResultAcc fitResult;
        fitResult.count = 1;
        fitResult.chiSqAvg = chiSquared;
        fitResult.chiSqMin = chiSquared;
        fitResult.chiSqMax = chiSquared;
        polRes.emplace(pol, fitResult);
    } else {
        auto &fitResult = resIt->second;
        fitResult.count++;
        fitResult.chiSqAvg += (chiSquared - fitResult.chiSqAvg) /
            static_cast<float>(fitResult.count);
        if (chiSquared < fitResult.chiSqMin)
            fitResult.chiSqMin = chiSquared;
        if (chiSquared > fitResult.chiSqMax)
            fitResult.chiSqMax = chiSquared;
    }
}

Record UVContSubResult::getAccumulatedResult() const
{
    Record fieldRec;
    for (const auto fieldIt : accum) {
        Record srec;
        for (const auto scanIt : fieldIt.second) {
            Record sprec;
            for (const auto spwIt : scanIt.second) {
                Record polrec;
                for (const auto polIt : spwIt.second) {
                    const auto fitResult = polIt.second;
                    Record avg;
                    avg.define("real", fitResult.chiSqAvg.real());
                    avg.define("imag", fitResult.chiSqAvg.imag());
                    Record min;
                    min.define("real", fitResult.chiSqMin.real());
                    min.define("imag", fitResult.chiSqMin.imag());
                    Record max;
                    max.define("real", fitResult.chiSqMax.real());
                    max.define("imag", fitResult.chiSqMax.imag());
                    Record chisq;
                    chisq.defineRecord("average", avg);
                    chisq.defineRecord("min", min);
                    chisq.defineRecord("max", max);

                    Record stats;
                    stats.defineRecord("chi_squared", chisq);
                    stats.define("count", (unsigned int)fitResult.count);
                    polrec.defineRecord(std::to_string(polIt.first), stats);
                }
                Record polTop;
                polTop.defineRecord("polarization", polrec);
                sprec.defineRecord(std::to_string(spwIt.first), polTop);
            }
            Record spwTop;
            spwTop.defineRecord("spw", sprec);
            srec.defineRecord(std::to_string(scanIt.first), spwTop);
        }
        Record scanTop;
        scanTop.defineRecord("scan", srec);
        fieldRec.defineRecord(std::to_string(fieldIt.first), scanTop);
    }
    Record gof;
    gof.defineRecord("field", fieldRec);

    Record uvcont;
    uvcont.defineRecord("goodness_of_fit", gof);
    uvcont.define("description",
                  "summary of data fitting results in uv-continuum subtraction");
    return uvcont;
}

} //# NAMESPACE VI - END

} //# NAMESPACE CASA - END
