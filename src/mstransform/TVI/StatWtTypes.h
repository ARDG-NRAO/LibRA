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

#ifndef STATWTTYPES_H_
#define STATWTTYPES_H_

namespace casa {

namespace vi {

// Only StatWt needs to use this; developers should not use this code directly.
// Shared types among StatWt classes.

class StatWtTypes {

public:
    
    using Baseline = std::pair<casacore::uInt, casacore::uInt>;

    struct ChanBin {
        casacore::uInt start = 0;
        casacore::uInt end = 0;

        bool operator<(const ChanBin& other) const {
            if (start < other.start) {
                return true;
            }
            if (start == other.start && end < other.end) {
                return true;
            }
            return false;
        }
    };

    enum Column {
        // column(s) to use
        // DATA
        DATA,
        // CORRECTED_DATA
        CORRECTED,
        // CORRECTED_DATA - MODEL_DATA
        RESIDUAL,
        // DATA - MODEL_DATA
        RESIDUAL_DATA
    };

    static casacore::String asString(Column col) {
        switch (col) {
        case DATA:
            return "DATA";
        case CORRECTED:
            return "CORRECTED_DATA";
        case RESIDUAL:
            return "CORRECTED_DATA - MODEL_DATA";
        case RESIDUAL_DATA:
            return "DATA - MODEL_DATA";
        default:
            ThrowCc("Unhandled column");
        }
    }

    struct BaselineChanBin {
        Baseline baseline = std::make_pair(0, 0);
        casacore::uInt spw = 0;
        vi::StatWtTypes::ChanBin chanBin;
        bool operator<(const BaselineChanBin& other) const {
            if (baseline < other.baseline) {
                return true;
            }
            if (baseline == other.baseline && spw < other.spw) {
                return true;
            }
            return baseline == other.baseline && spw == other.spw
                && chanBin < other.chanBin;
        };
    };

};

}

}

#endif 

