//# Copyright (C) 2018
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or(at your option)
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
//# $Id: $
#include <casacore/casa/Utilities/Assert.h>
#include <stdcasa/version.h>

using casa::VersionInfo;
using casacore::AipsError;
using std::vector;
using std::endl;
using std::cout;
using std::cerr;

int main( int, char *[] ) {
    try {
        vector<int> cur { VersionInfo::major( ),
                     VersionInfo::minor( ),
                     VersionInfo::patch( ),
                     VersionInfo::feature( ) };

        AlwaysAssert(VersionInfo::compare("==",cur), AipsError);
        AlwaysAssert(VersionInfo::compare("<=",cur), AipsError);
        AlwaysAssert(VersionInfo::compare(">=",cur), AipsError);
        AlwaysAssert(VersionInfo::compare("<",cur) == false, AipsError);
        AlwaysAssert(VersionInfo::compare(">",cur) == false, AipsError);
        AlwaysAssert(VersionInfo::compare(">",vector<int>{ cur[0] }), AipsError);

        AlwaysAssert(VersionInfo::compare("<",vector<int>{ cur[0], cur[1], cur[2], cur[3]+1 }), AipsError);
        AlwaysAssert(VersionInfo::compare("<",vector<int>{ cur[0], cur[1], cur[2]+1, cur[3] }), AipsError);
        AlwaysAssert(VersionInfo::compare("<",vector<int>{ cur[0], cur[1]+1, cur[2], cur[3] }), AipsError);
        AlwaysAssert(VersionInfo::compare("<",vector<int>{ cur[0]+1, cur[1], cur[2], cur[3] }), AipsError);
        
        AlwaysAssert(VersionInfo::compare(">",vector<int>{ cur[0], cur[1], cur[2], cur[3]+1 }) == false, AipsError);
        AlwaysAssert(VersionInfo::compare(">",vector<int>{ cur[0], cur[1], cur[2]+1, cur[3] }) == false, AipsError);
        AlwaysAssert(VersionInfo::compare(">",vector<int>{ cur[0], cur[1]+1, cur[2], cur[3] }) == false, AipsError);
        AlwaysAssert(VersionInfo::compare(">",vector<int>{ cur[0]+1, cur[1], cur[2], cur[3] }) == false, AipsError);

        AlwaysAssert(VersionInfo::compare("<=",vector<int>{ cur[0], cur[1], cur[2], cur[3]+1 }), AipsError);
        AlwaysAssert(VersionInfo::compare("<=",vector<int>{ cur[0], cur[1], cur[2]+1, cur[3] }), AipsError);
        AlwaysAssert(VersionInfo::compare("<=",vector<int>{ cur[0], cur[1]+1, cur[2], cur[3] }), AipsError);
        AlwaysAssert(VersionInfo::compare("<=",vector<int>{ cur[0]+1, cur[1], cur[2], cur[3] }), AipsError);
        
        AlwaysAssert(VersionInfo::compare(">=",vector<int>{ cur[0], cur[1], cur[2], cur[3]+1 }) == false, AipsError);
        AlwaysAssert(VersionInfo::compare(">=",vector<int>{ cur[0], cur[1], cur[2]+1, cur[3] }) == false, AipsError);
        AlwaysAssert(VersionInfo::compare(">=",vector<int>{ cur[0], cur[1]+1, cur[2], cur[3] }) == false, AipsError);
        AlwaysAssert(VersionInfo::compare(">=",vector<int>{ cur[0]+1, cur[1], cur[2], cur[3] }) == false, AipsError);

        AlwaysAssert(VersionInfo::compare("!=",vector<int>{ cur[0], cur[1], cur[2], cur[3]+1 }), AipsError);
        AlwaysAssert(VersionInfo::compare("!=",vector<int>{ cur[0], cur[1], cur[2]+1, cur[3] }), AipsError);
        AlwaysAssert(VersionInfo::compare("!=",vector<int>{ cur[0], cur[1]+1, cur[2], cur[3] }), AipsError);
        AlwaysAssert(VersionInfo::compare("!=",vector<int>{ cur[0]+1, cur[1], cur[2], cur[3] }), AipsError);


    } catch (const AipsError& x) {
        cerr << "Exception caught: " << x.getMesg() << endl;
        return 1;
    } catch (...) {
        cerr << "Exception caught..." << endl;
        return 1;
    }
    cout << "ok" << endl;
    return 0;
}
