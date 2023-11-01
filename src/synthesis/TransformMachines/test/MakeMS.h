//# MakeMS: Tests 
//# Copyright (C) 2016
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
//#
//# $Id$

#include <casacore/casa/aips.h>
#include <casacore/casa/BasicSL/String.h>
#include <synthesis/MeasurementEquations/Simulator.h>
#include <casacore/measures/Measures/MDirection.h>
#include <casacore/measures/Measures/MeasTable.h>

namespace casa {

namespace test {

class MakeMS {

public:

    static inline void makems( casacore::String msname, casacore::MDirection thedir, casacore::Double freq=1.5e9,
                        casacore::Double chanwidth=1e6, casacore::Int nchan=100, casacore::Int nInteg=200 ) {

        std::vector<Double> stnx={ 41.1100006,  134.110001,   268.309998,  439.410004,  
                                   644.210022,  880.309998,  1147.10999,  1442.41003,  1765.41003,
                                   -36.7900009, -121.690002,  -244.789993, -401.190002, -588.48999, 
                                   -804.690002, -1048.48999, -1318.48999, -1613.98999,
                                   -4.38999987,-11.29,       -22.7900009, -37.6899986, -55.3899994, 
                                   -75.8899994, -99.0899963, -124.690002, -152.690002 };
        std::vector<Double> stny={ 3.51999998, -39.8300018,  -102.480003, -182.149994, -277.589996,
                                   -387.839996, -512.119995, -649.76001,  -800.450012,
                                   -2.58999991, -59.9099998,  -142.889999, -248.410004, -374.690002,
                                   -520.599976, -685,        -867.099976, -1066.42004,
                                   77.1500015,  156.910004,   287.980011,  457.429993,  660.409973,
                                   894.700012,  1158.82996,  1451.43005,  1771.48999 };
        std::vector<Double> stnz={ 0.25,       -0.439999998, -1.46000004, -3.77999997, -5.9000001, 
                                   -7.28999996, -8.48999977, -10.5,       -9.56000042,
                                   0.25,       -0.699999988, -1.79999995, -3.28999996, -4.78999996,
                                   -6.48999977, -9.17000008, -12.5299997, -15.3699999,
                                   1.25999999,   2.42000008,   4.23000002,  6.65999985,  9.5,
                                   12.7700005,  16.6800003,  21.2299995,  26.3299999 };
        Vector<Double> xpos(stnx);
        Vector<Double> ypos(stny);
        Vector<Double> zpos(stnz);
        xpos *=5.0; ypos *=5.0;
        xpos-=mean(xpos); ypos-=mean(ypos); zpos-=mean(zpos);
        Vector<Double> diam(27, 25.0);
        Vector<String> antnames(27, "EVLA");
        Vector<String> padnames(27, "KotZot");
        for ( Int k=0; k <27; ++k ) {
            antnames[k] =antnames[k]+ String::toString(k);
            padnames[k]=padnames[k]+String::toString(k);
        }
        MPosition obsPos;
        casacore::MeasTable::Observatory(obsPos, "EVLA");
        Simulator sm(msname);
        sm.setconfig( "EVLA", xpos, ypos, zpos, diam, Vector<Double>(27,0.0), 
                      Vector<String>(27, "ALT-AZ"), antnames, padnames, "local", obsPos );
  
        sm.setspwindow( "Bandobast", Quantity(freq, "Hz"), Quantity(chanwidth, "Hz"),
                        Quantity(chanwidth, "Hz"), MFrequency::LSRK,  nchan, "RR RL LR LL" );
        MEpoch refdate( Quantity(57388.0, "d"), MEpoch::UTC );
        MeasFrame mframe(thedir, refdate, obsPos);
        MDirection thedir_hadec=MDirection::Convert(thedir, MDirection::Ref(MDirection::HADEC, mframe))();
        MEpoch lst=MEpoch::Convert(refdate, MEpoch::Ref(MEpoch::LAST, mframe))();
        MEpoch refepo(lst.get("d")-thedir_hadec.getValue().getLong("d"), MEpoch::UTC);
        sm.setfeed("perfect R L", Vector<Double>(), Vector<Double>(), Vector<String>(1, ""));
        sm.setlimits(0.01, Quantity(10.0, "deg"));
        sm.setauto(0.0);
        sm.setfield("2C666", thedir, "T", Quantity(0.0, "m"));
        sm.settimes(Quantity(1.0,"s"), true, refepo);
        sm.observe("2C666", "Bandobast", Quantity(-Double(nInteg)/2.0, "s"), Quantity(Double(nInteg)/2.0,"s"));
    }
   

};

} // namespace test
} // namespace casa
