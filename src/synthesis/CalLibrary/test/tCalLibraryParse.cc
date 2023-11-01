//# Copyright (C) 2015
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

#include <casacore/casa/aips.h>
#include <synthesis/CalLibrary/CalLibraryTools.h>
#include <casacore/casa/Containers/Record.h>

#include <fstream>

using namespace casacore;
using namespace casa;
using namespace std;

int parseString(String callibString, Bool testString=true) {
  /* Return value of 1 = parsing succeeded */
  try {
    cout << "\ntCalLibraryParse STRING " << endl;

    Record callibRec = callibSetParams(callibString);
    if (testString) {
        bool ok1, ok2, ok3, ok4;
        //cout << "DEBUG: callibRec = " << callibRec << endl;

        // Check 1 - number of fields in returned Record
        ok1 = (callibRec.nfields() == 1);
        cout << "callib Record nfields=1 check: ";
        cout << (ok1 ? "PASS" : "FAIL")<< endl;

        // Check 2 - bcal has 2 fields
        Record bcalRec = callibRec.asRecord("ngc5921.bcal");
        ok2 = (bcalRec.nfields() == 2);
        cout << "bcal Record nfields=2 check: ";
        cout << (ok2 ? "PASS" : "FAIL") << endl;

        // Check 3 - bcal tinterp check
        Record bcalRec0 = bcalRec.asRecord("0");
        ok3 = (bcalRec0.asString("tinterp") == "nearest"); 
        cout << "bcal tinterp='nearest' check: ";
        cout << (ok3 ? "PASS" : "FAIL") << endl;

        // Check 4 - bcal calwt=false 
        ok4 = (bcalRec.asBool("calwt") == 0); 
        cout << "bcal calwt=0 check: ";
        cout << (ok4 ? "PASS" : "FAIL") << endl;

        return (ok1 && ok2 && ok3 && ok4);
    } else {
        // Feedback to user; cannot check for right answers!
        cout << "callib Record nfields = " << callibRec.nfields() << endl;
        cout << "Record parsed from string:" << endl;
        cout << callibRec << endl;
        return 1;
    }
  } catch (AipsError x) {
    cout << "ERROR: " << x.getMesg() << endl;
    return 0;
  }
}

int parseFile(String callibName) {
  /* Return value of 1 = parsing succeeded */
  cout << "\ntCalLibraryParse FILE " << callibName << endl;

  try {
    Record callibRec = callibSetParams(callibName);
    //cout << "DEBUG: callibRec = " << callibRec << endl;
    
    if (callibName == "/tmp/test_callib_parse.txt") {
        bool ok1, ok2, ok3, ok4a, ok4b;

        // Check 1 - number of fields in returned Record
        ok1 = (callibRec.nfields() == 3);  // 3 caltables
        cout << "callib Record nfields=3 check: ";
        cout << (ok1 ? "PASS" : "FAIL") << endl;

        // Check 2 - gcal has 2 Records plus calwt
        ok2 = (callibRec.asRecord("ngc5921.gcal").nfields() == 3);
        cout << "gcal nfields=3 check: ";
        cout << (ok2 ? "PASS" : "FAIL") << endl;

        // Check 3 - gcal kept calwt=true setting
        Record gcalRec = callibRec.asRecord("ngc5921.gcal");
        ok3 = (gcalRec.asBool("calwt") == 1); 
        cout << "gcal calwt=1 check: ";
        cout << (ok3 ? "PASS" : "FAIL") << endl;

        // Check 4 - properly parsed fldmap list
        Record gcalRec1 = gcalRec.asRecord("1");
        ok4a = (gcalRec1.dataType("fldmap") == TpArrayInt); 
        cout << "gcal1 fldmap type is Array<Int> check: ";
        cout << (ok4a ? "PASS" : "FAIL") << endl;
        Array<Int> fldmap = gcalRec1.asArrayInt("fldmap");
        ok4b = (fldmap.size() == 4);
        cout << "gcal1 fldmap size=4 check: ";
        cout << (ok4b ? "PASS" : "FAIL") << endl;

        return (ok1 && ok2 && ok3 && ok4a && ok4b);
    } else {
        // Feedback to user; cannot check for right answers!
        cout << "callib Record nfields = " << callibRec.nfields() << endl;
        cout << "Record parsed from file:" << endl;
        cout << callibRec << endl;
        return 1;
    }
  } catch (AipsError x) {
    cout << "ERROR: " << x.getMesg() << endl;
    return 0;
  }
}

int main(int argc, const char* argv[])
{
    String callib;
    if (argc == 2) {
        callib = argv[1];
        if (callib.contains("="))
            parseString(callib, false);
        else
            parseFile(callib);
    } else {
        String cal1 = "caltable='ngc5921.bcal' calwt=false tinterp='nearest'";
        String cal2 = "caltable='ngc5921.fluxscale' calwt=T tinterp='nearest' fldmap='nearest'";
        String cal3 = "caltable='ngc5921.gcal' calwt=true field='0' tinterp='nearest' fldmap=[0]";
        String cal4 = "calwt=false field='1,2' tinterp='linear' fldmap=[0,1,1,3] caltable='ngc5921.gcal'";
        String cal5 = "# this is a comment";

        int ok_string = parseString(cal1);

        String multistring = cal1 + "," + cal2;
        // Don't check Record, just make sure it parses without error
        int ok_string2 = parseString(multistring, false);

        // make callib file from strings
        const char* filename = "/tmp/test_callib_parse.txt";
        ofstream callibFile;
        callibFile.open(filename);
        callibFile << cal1 << "\n";
        callibFile << cal5 << "\n";  // insert comment
        callibFile << cal2 << "\n";
        callibFile << "\n";  // insert blank line
        callibFile << cal3 << "\n";
        callibFile << cal4 << "\n";
        callibFile.close();
        callib = filename;

        int ok_file = parseFile(callib);

        cout << "\ntCalLibraryParse result = " << ((ok_string && ok_string2 && ok_file) ? "PASS" : "FAIL") << endl;
    }
  return 0;
}  
