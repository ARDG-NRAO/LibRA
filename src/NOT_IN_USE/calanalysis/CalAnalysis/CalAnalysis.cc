
// -----------------------------------------------------------------------------

/*

CalAnalysis.cc

Description:
------------
This file contains member functions for the CalAnalysis class.

Classes:
--------
CalAnalysis - This class acts as the interface between the ROCTIter and CalStats
              classes.

Modification history:
---------------------
2012 Jan 20 - Nick Elias, NRAO
              Initial version.
2012 Jan 25 - Nick Elias, NRAO
              Logging capability added.  Error checking added.
2012 Feb 14 - Nick Elias, NRAO
              Updated this code to reflect changes in NewCalTabIter (now
              ROCTIter) and other classes.

*/

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <calanalysis/CalAnalysis/CalAnalysis.h>

// -----------------------------------------------------------------------------
// Start of casa namespace
// -----------------------------------------------------------------------------

using namespace casacore;
namespace casa {

// -----------------------------------------------------------------------------
// Start of CalAnalysis class
// -----------------------------------------------------------------------------

/*

CalAnalysis

Description:
------------
This class acts as the interface between the ROCTIter and CalAnalysis classes.

In a nutshell:
--------------
* The constructor gets the information from the new format calibration table
  and initializes the class.
* The stats<T>() function calculates statistics (the type depends on T) and
  returns the results.
  - The user can specify the field(s), antenna 1s, antenna 2s, time range,
    feeds, spectral windows, channels, the second iteration axis for the 
    CalStats class (time or frequency), normalization, and unwrapping.
  - The inputs are checked and fixed, if possible.
  - The data are grouped according to unique (field,antenna1,antenna2).
  - The resulting group-based information is fed to the CalStats class and its
    stats<T>() function calculates the desired statistics which are stored in a
    vector of OUTPUT<T>() instances.  Each OUTPUT<T>() element corresponds to a
    group.

NB: There are a lot of get/set member functions.  Unfortunately, they could not
    be overloaded with the same names because of conflicts with corresponding
    member functions without get/set in their names.  Plus, some get functions
    need to call other get functions.

NB: When an instance of this class is created from the python binding, input
    parameters are mostly checked there.  There is checking in this class as
    well, just in case that the class is not called from the python binding.
    There does not appear to be a significant speed penalty.

Nested classes:
---------------
OUTPUT<T> - This nested class contains the outputs for the
            CalAnalysis::stats<T>() template member function.

Class public member functions:
------------------------------
CalAnalysis  - This constructor gets information from the new format calibration
               table for further processing by the stats<T>() function.
~CalAnalysis - This destructor deallocates the internal memory of an instance.
calName      - This member function returns the new format calibration table
               name.
msName       - This member function returns the associated MS name.
visCal       - This member function returns the visibility calibration type.
parType      - This member function returns the parameter type ("Complex" or
               "Float").
polBasis     - This member function returns the polarization basis ("L" or "C").
field        - This member function returns the field numbers.
antenna      - This member function returns the antenna numbers.
antenna1     - This member function returns the antenna 1 numbers.
antenna2     - This member function returns the antenna 2 numbers.
time         - This member function returns the times.
feed         - This member function returns the feeds.
numspw       - This member function returns the number of spectral windows.
spw          - This member function returns the spectral windows.
numChannel   - This member function returns the number of channels for each
               spectral window.
freq         - This member function returns the frequencies for each spectral
               window.

Class template public member functions:
---------------------------------------
stats<T> - This member function is the main user interface for calculating the
           statistics for all iterations.  Allowed T: CalStats::NONE only
           returns the input data, CalStatsFitter::FIT calculates fit
           statistics, and CalStatsHist::HIST calculates histogram statistics.

Class template static public member functions:
----------------------------------------------
exists<T> - This member function determines whether a value appears in a vector.
where<T>  - This member function returns the index of a search value in a
            vector.
unique<T> - This member function returns a unique vector from an input vector.

Class private member functions:
-------------------------------
calNameGet      - This member function gets the new format calibration table
                  name from the new format calibration table.
calNameSet      - This member function sets the new format calibration table
                  name private variable.
msNameGet       - This member function gets the associated MS name from the new
                  format calibration table.
msNameSet       - This member function sets the associated MS name private
                  variable.
visCalGet       - This member function gets the visibility calibration type from
                  the new format calibration table.
visCalSet       - This member function sets the visibility calibration type
                  private variable.
parTypeGet      - This member function gets the parameter type ("Complex" or
                  "Float") from the new format calibration table.
parTypeSet      - This member function sets the parameter type ("Complex" or
                  "Float") private variable.
polBasisGet     - This member function gets the polarization basis ("L" or "C")
                  from the new format calibration table.
polBasisSet     - This member function sets the polarization basis ("L" or "C")
                  private variable.
fieldGet        - This member function gets the field numbers from the new
                  format calibration table.
fieldSet        - This member function sets the field numbers private variables.
fieldCheck      - This member function checks the input field vector and returns
                  the fixed field vector.
antennaGet      - This member function gets the antenna numbers from the new
                  format calibration table.
antennaSet      - This member function sets the antenna numbers private
                  variables.
antenna1Get     - This member function gets the antenna 1 numbers from the new
                  format calibration table.
antenna1Set     - This member function sets the antenna 1 numbers private
                  variables.
antenna1Check   - This member function checks the input antenna 1 vector and
                  returns the fixed antenna 1 vector.
antenna2Get     - This member function gets the antenna 2 numbers from the new
                  format calibration table.
antenna2Set     - This member function sets the antenna 2 numbers private
                  variables.
antenna2Check   - This member function checks the input antenna 2 vector and
                  returns the fixed antenna 2 vector.
timeGet         - This member function gets the times from the new format
                  calibration table.
timeSet         - This member function sets the times private variables.
timeCheck       - This member function checks the time range and returns the
                  corresponding time vector.
feedGet         - This member function gets the feeds from the new format
                  calibration table.
feedSet         - This member function sets the feeds private variables.
feedCheck       - This member function checks the input feed vector and returns
                  the fixed feed vector.
spwInfoGet      - This member function gets the spectral window information from
                  the new format calibration table.
spwInfoSet      - This member function sets the spectral window information
                  private variables.
statsCheckInput - This member function checks and fixes (if possible) the inputs
                  to the CalAnalysis::stats<T>() member function.
getGroup        - This member function gets input selected rows from a new
                  format calibration table and groups them according to unique
                  (field,antenna1,antenna2).
rowSelect       - This member function returns the rows selected by the input
                  parameters.
rowGroup        - This member function returns the row numbers, fields, antenna
                  1s, antenna 2s, spectral windows, unique spectral windows,
                  times, and unique times grouped according to selected rows and
                  input selection.
chanSPW         - This member function maps the spectral windows to the input
                  spectral windows (to get the correct channels) and forms the
                  start channels so that spectral windows can be concatenated.
freqGroup       - This member function concatenates the frequencies from
                  multiple spectral windows for each group.
cubeGroup       - This member function concatenates data from multiple rows into
                  groups according to unique (field,antenna1,antenna2).

Class protected member functions:
---------------------------------
CalAnalysis - This default constructor is unused by this class and unavailable
              when an instance is created.
CalAnalysis - This copy constructor is unused by this class and unavailable when
              an instance is created.
operator=   - This operator= function is unused by this class and unavailable
              when an instance is created.

Modification history:
---------------------
2012 Jan 20 - Nick Elias, NRAO
              Initial version created with public member functions CalAnalysis()
              (generic), ~CalAnalysis(); template static public member function
              stats<T>(); template public member functions exists<T>() and
              unique<T>(); private member functions tableType(), polBasisGet(),
              feedCheck(), timeCheck(), spw_channel(), freq(); template private
              member functions parse<T>(), and select<T>(); and protected member
	      functions CalAnalysis() (default), CalAnalysis() (copy), and
              operator=().
2012 Feb 14 - Nick Elias, NRAO
              Updated this code to reflect changes in NewCalTabIter (now
              ROCTIter) and other classes.  Added the RAP enum.
2012 Mar 13 - Nick Elias, NRAO
              Public member function tableType() renamed to parTypeGet().
              Private member functions msNameGet() and visCalGet() added.
2012 Mar 14 - Nick Elias, NRAO
              Spectral window ID, start channel, and stop channel added to the
              nested OUTPUT<T> class.
2012 Apr 03 - Nick Elias, NRAO
              Private member function calNameGet() added.  Public member
              functions calName(), msName(), visCal(), parType(), and polBasis()
              added.
2012 Apr 04 - Nick Elias, NRAO
              Private member functions calNameSet(), msNameSet(), visCalSet(),
              parTypeSet(), polBasisSet(), feedGet(), feedSet(), timeGet(),
              timeSet(), spwGet(), and spwSet() added.  Public member functions
              feed(), time(), spw(), and numspw() added.
2012 Apr 17 - Nick Elias, NRAO
              Nested class CalAnalysis::SPW_INFO() added.  Private member
              functions fieldGet(), fieldSet(), fieldCheck(), antennaGet(),
              antennaSet(), antennaCheck(), spwInfoGet(), and spwInfoSet()
              added.  Public member functions field(), antenna(), numChannel(),
              and freq() added.  Private member functions spwGet(), spwSet(),
              and spw_channel() removed.
2012 Apr 25 - Nick Elias, NRAO
              Private member function antennaCheck() renamed to antenna1Check().
              private member function antenna2Check() added.  The start and stop
              channel lists versus spectral window are replaced with a channel
              list versus spectral window.
2012 Apr 26 - Nick Elias, NRAO
              Nested class INPUT added (for the stats<T>() member function).
              Spectral window ID, start channel, and stop channel removed from
              the nested OUTPUT<T> class.
2012 Apr 27 - Nick Elias, NRAO
              Private member function statsCheckInput() added.  The member
              functions of the nested class CalAnalysis::SPW_INFO() have been
              moved to a separate file CalAnalysisSPW_INFO.cc.
2012 May 02 - Nick Elias, NRAO
              Private member functions antenna1Get(), antenna1Set(),
              antenna2Get(), and antenna2Set() added.
2012 May 06 - Nick Elias, NRAO
              Template private member functions parse<T>() and select<T>()
              removed.  Template static public member function where<T>() added.
              Private member functions getGroup(), rowSelect(), rowGroup(),
              chanSPW(), freqGroup(), and cubeGroup() added.
2012 May 07 - Nick Elias, NRAO
              Public member functions antenna1() and antenna2() added.

*/

// -----------------------------------------------------------------------------
// Start of CalAnalysis public member functions
// -----------------------------------------------------------------------------

/*

CalAnalysis::CalAnalysis (generic)

Description:
------------
This constructor gets information from the new format calibration table for
further processing by the stats<T>() function.

Inputs:
-------
oTableName - This reference to a String instance contains the new format
             calibration table name.

Outputs:
--------
None.

Modification history:
---------------------
2012 Jan 20 - Nick Elias, NRAO
              Initial version.
2012 Jan 25 - Nick Elias, NRAO
              Error checking added.
2012 Apr 17 - Nick Elias, NRAO
              Called new member functions to make this member function shorter.
2012 May 02 - Nick Elias, NRAO
              New format calibration table iterator no longer initialized here.
2012 May 06 - Nick Elias, NRAO
              Eliminated the call to the NewCalTable() instance.

*/

// -----------------------------------------------------------------------------

CalAnalysis::CalAnalysis( const String& oTableName ) {

  // Set the private variables corresponding to the new format calibration
  // table name, MS name, visibility calibration type, parameter type, and
  // polarization basis

  // Used to check if the new format calibration table name is valid
  try {
    calNameSet( calNameGet(oTableName) );
  }

  catch ( AipsError oAipsError ) {
    throw( oAipsError );
  }

  msNameSet( msNameGet(oTableName) );
  visCalSet( visCalGet(oTableName) );
  parTypeSet( parTypeGet(oTableName) );
  polBasisSet( polBasisGet(oTableName) );


  // Set the private variables corresponding to the field vector, antenna
  // vectors, time vector, feed vector, and spectral window information

  fieldSet( fieldGet(oTableName) );

  antennaSet( antennaGet(oTableName) );
  antenna1Set( antenna1Get(oTableName) );
  antenna2Set( antenna2Get(oTableName) );

  timeSet( timeGet(oTableName) );

  feedSet( feedGet(oTableName) );

  spwInfoSet(CalAnalysis::SPW_INFO(oTableName));


  // Return

  return;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::~CalAnalysis

Description:
------------
This destructor deallocates the internal memory of an instance.

Inputs:
-------
None.

Outputs:
--------
None.

Modification history:
---------------------
2012 Jan 20 - Nick Elias, NRAO
              Initial version.
2012 May 02 - Nick Elias, NRAO
              The new format calibration table iterator pointer may be
              deallocated elsewhere, so it is checked for NULL first.
2012 May 06 - Nick Elias, NRAO
              Removed references to NewCalTable() and CTIter().

*/

// -----------------------------------------------------------------------------

CalAnalysis::~CalAnalysis( void ) {}

// -----------------------------------------------------------------------------

/*

CalAnalysis::calName

Description:
------------
This member function returns the new format calibration table.

Inputs:
-------
None.

Outputs:
--------
The String instance containing the new format calibration table
name, returned via the function value.

Modification history:
---------------------
2012 Apr 03 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

String CalAnalysis::calName( void ) const {

  // return the variable containing the new format calibration table name

  return oCalName;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::msName

Description:
------------
This member function returns the MS name.

Inputs:
-------
None.

Outputs:
--------
The String instance containing the MS name, returned via the
function value.

Modification history:
---------------------
2012 Apr 03 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

String CalAnalysis::msName( void ) const {

  // return the private variable containing the MS name

  return oMSName;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::visCal

Description:
------------
This member function returns the visibility calibration type.

Inputs:
-------
None.

Outputs:
--------
The String instance containing the visibility calibration type,
returned via the function value.

Modification history:
---------------------
2012 Apr 03 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

String CalAnalysis::visCal( void ) const {

  // Return opy the private variable containing the visibility calibration type

  return oVisCal;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::parType

Description:
------------
This member function returns the parameter type ("Complex" or "Float").

Inputs:
-------
None.

Outputs:
--------
The String instance containing the parameter type, returned via
the function value.

Modification history:
---------------------
2012 Apr 03 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

String CalAnalysis::parType( void ) const {

  // Return the private variable containing the parameter type

  return oParType;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::polBasis

Description:
------------
This member function returns the polarization basis ("L" or "C").

Inputs:
-------
None.

Outputs:
--------
The reference to the String instance containing the polarization basis, returned
via the function value.

Modification history:
---------------------
2012 Apr 03 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

String CalAnalysis::polBasis( void ) const {

  // Return the variable containing the polarization basis

  return oPolBasis;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::field

Description:
------------
This member function returns the field numbers.

Inputs:
-------
None.

Outputs:
--------
The Vector<uInt> instance containing the fields, returned via
the function value.

Modification history:
---------------------
2012 Apr 17 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

Vector<uInt> CalAnalysis::field( void ) const {

  // Copy the private variable containing the field numbers and return it

  return oField.copy();

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::antenna

Description:
------------
This member function returns the antenna numbers.

Inputs:
-------
None.

Outputs:
--------
The Vector<uInt> instance containing the antenna numbers,
returned via the function value.

Modification history:
---------------------
2012 Apr 17 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

Vector<uInt> CalAnalysis::antenna( void ) const {

  // Return a copy of the private variable containing the antenna numbers

  return oAntenna.copy();

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::antenna1

Description:
------------
This member function returns the antenna 1 numbers.

Inputs:
-------
None.

Outputs:
--------
The the Vector<uInt> instance containing the antenna 1 numbers,
returned via the function value.

Modification history:
---------------------
2012 May 07 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

Vector<uInt> CalAnalysis::antenna1( void ) const {

  // Copy the private variable containing the antenna 1 numbers and return it

  return oAntenna1.copy();

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::antenna2

Description:
------------
This member function returns the antenna 2 numbers.

Inputs:
-------
None.

Outputs:
--------
The Vector<Int> instance containing the antenna 2 numbers,
returned via the function value.

Modification history:
---------------------
2012 May 07 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

Vector<Int> CalAnalysis::antenna2( void ) const {

  // Copy the private variable containing the antenna 2 numbers and return it

  return oAntenna2.copy();

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::time

Description:
------------
This member function returns the times.

Inputs:
-------
None.

Outputs:
--------
The Vector<Double> instance containing the times, returned via
the function value.

Modification history:
---------------------
2012 Apr 04 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

Vector<Double> CalAnalysis::time( void ) const {

  // Copy the private variable containing the times and return it

  return oTime.copy();

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::feed

Description:
------------
This member function returns the feeds.

Inputs:
-------
None.

Outputs:
--------
The Vector<String> instance containing the feeds, returned via
the function value.

Modification history:
---------------------
2012 Apr 04 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

Vector<String> CalAnalysis::feed( void ) const {

  // Copy the private variable containing the feeds and return it

  return oFeed.copy();

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::numspw

Description:
------------
This member function returns the number of spectral windows.

Inputs:
-------
None.

Outputs:
--------
The uInt variable containing the spectral windows, returned via
the function value.

Modification history:
---------------------
2012 Apr 04 - Nick Elias, NRAO
              Initial version.
2012 Apr 17 - Nick Elias, NRAO
              Modified to handle the SPW_INFO instance.

*/

// -----------------------------------------------------------------------------

uInt CalAnalysis::numspw( void ) const {

  // Return the variable containing the number of spectral windows

  return oSPWInfo.uiNumSPW;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::spw

Description:
------------
This member function returns the spectral windows.

Inputs:
-------
None.

Outputs:
--------
The Vector<uInt> instance containing the spectral windows,
returned via the function value.

Modification history:
---------------------
2012 Apr 04 - Nick Elias, NRAO
              Initial version.
2012 Apr 17 - Nick Elias, NRAO
              Modified to handle the SPW_INFO instance.

*/

// -----------------------------------------------------------------------------

Vector<uInt> CalAnalysis::spw( void ) const {

  // Copy the private variable containing the spectral windows and return it

  return oSPWInfo.oSPW.copy();

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::numChannel

Description:
------------
This member function returns the number of channels for each spectral window.

Inputs:
-------
None.

Outputs:
--------
The array of pointers to the Vector<uInt> instances containing the number of
channels for each spectral window, returned via the function value.

Modification history:
---------------------
2012 Apr 17 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

Vector<uInt> CalAnalysis::numChannel( void ) const {

  // Copy the private variable containing the number of channels for each
  // spectral window and return it

  return oSPWInfo.oNumChannel.copy();

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::freq

Description:
-----------
This member function returns the frequencies for each spectral window.

Inputs:
-------
None.

Outputs:
--------
The reference to the Vector<Vector<uInt> > instance containing the frequencies
for each spectral window, returned via the function value.

Modification history:
---------------------
2012 Apr 17 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

Vector<Vector<Double> > CalAnalysis::freq( void ) const {

  // Copy the private variable containing the frequencies for each spectral
  // window and return it

  uInt uiNumSPW = oSPWInfo.uiNumSPW;

  Vector<Vector<Double> > freq( uiNumSPW );

  for ( uInt s=0; s<uiNumSPW; s++ ) {
    freq[s] = oSPWInfo.oFrequency[s].copy();
  }

  return freq;

}

// -----------------------------------------------------------------------------
// End of CalAnalysis public member functions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Start of CalAnalysis private member functions
// -----------------------------------------------------------------------------

/*

CalAnalysis::calNameGet

Description:
------------
This member function gets the new format calibration table name from the new
format calibration table.

NB: This function is somewhat trivial, but it is included because of the other
get member functions of this class.

Inputs:
-------
oTableName - This reference to a String instance contains the new format
             calibration table name.

Outputs:
--------
The reference to the String instance containing the new format calibration table
name, returned via the function value.

Modification history:
---------------------
2012 Apr 03 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

String CalAnalysis::calNameGet( const String& oTableName ) {

  // Get the new format calibration table name and return it

  String poTableName = oTableName;

  return poTableName;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::calNameSet

Description:
------------
This member function sets the new format calibration table name private
variable.

Inputs:
-------
oCalNameIn - This reference to a String instance contains the new format
             calibration table name.

Outputs:
--------
None.

Modification history:
---------------------
2012 Apr 04 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

void CalAnalysis::calNameSet( const String& oCalNameIn ) {

  // Set the new format calibration table name and return

  oCalName = String( oCalNameIn );

  return;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::msNameGet

Description:
------------
This member function gets the associated MS name from the new format calibration
table.

Inputs:
-------
oTableName - This reference to a String instance contains the new format
             calibration table name.

Outputs:
--------
The reference to the String instance containing the MS name, returned via the
function value.

Modification history:
---------------------
2012 Mar 13 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

String CalAnalysis::msNameGet( const String& oTableName ) {

  // Get the record containing the main table keywords

  Table oTable( oTableName );
  TableProxy oTP( oTable );
  Record oR( oTP.getKeywordSet( String("") ) );


  // Get the associated MS name and return it

  uInt uiIndex = oR.fieldNumber( String( "MSName" ) );
  String poMSName = RecordFieldPtr<String>(oR,uiIndex).get();

  return poMSName;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::msNameSet

Description:
------------
This member function sets the associated MS name private variable.

Inputs:
-------
oMSNameIn - This reference to a String instance contains the assciated MS name.

Outputs:
--------
None.

Modification history:
---------------------
2012 Apr 04 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

void CalAnalysis::msNameSet( const String& oMSNameIn ) {

  // Set the assciated MS name and return

  oMSName = String( oMSNameIn );

  return;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::visCalGet

Description:
------------
This member function gets the visibility calibration type from the new format
calibration table.

Inputs:
-------
oTableName - This reference to a String instance contains the new format
             calibration table name.

Outputs:
--------
The reference to the String instance containing the visibility calibration type
instance, returned via the function value.

Modification history:
---------------------
2012 Mar 13 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

String CalAnalysis::visCalGet( const String& oTableName ) {

  // Get the record containing the main table keywords

  Table oTable( oTableName );
  TableProxy oTP( oTable );
  Record oR( oTP.getKeywordSet( String("") ) );


  // Get the visibility calibration type and return it

  uInt uiIndex = oR.fieldNumber( String( "VisCal" ) );
  String poVisCal = RecordFieldPtr<String>(oR,uiIndex).get();

  return poVisCal;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::visCalSet

Description:
------------
This member function sets the visibility calibration type private variable.

Inputs:
-------
oVisCalIn - This reference to a String instance contains the visibility
            calibration type.

Outputs:
--------
None.

Modification history:
---------------------
2012 Apr 04 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

void CalAnalysis::visCalSet( const String& oVisCalIn ) {

  // Set the visibility calibration type and return

  oVisCal = String( oVisCalIn );

  return;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::parTypeGet

Description:
------------
This member function gets the parameter column type ("Complex" or "Float") from
the new format calibration table.

Inputs:
-------
oTableName - This reference to a String instance contains the new format
             calibration table name.

Outputs:
--------
The reference to the String instance containing the parameter column type,
returned via the function value.

Modification history:
---------------------
2012 Jan 20 - Nick Elias, NRAO
              Initial version.
2012 Mar 13 - Nick Elias, NRAO
              Function renamed to parType().

*/

// -----------------------------------------------------------------------------

String CalAnalysis::parTypeGet( const String& oTableName ) {

  // Get the record containing the main table keywords

  Table oTable( oTableName );
  TableProxy oTP( oTable );
  Record oR( oTP.getKeywordSet( String("") ) );


  // Get the parameter column type and return it

  uInt uiIndex = oR.fieldNumber( String( "ParType" ) );
  String poParType = RecordFieldPtr<String>(oR,uiIndex).get();

  return poParType;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::parTypeSet

Description:
------------
This member function sets the parameter column type ("Complex" or "Float")
private variable.

Inputs:
-------
oParTypeIn - This reference to a String instance contains the parameter column
             type.

Outputs:
--------
None.

Modification history:
---------------------
2012 Apr 04 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

void CalAnalysis::parTypeSet( const String& oParTypeIn ) {

  // Set the parameter column type and return

  oParType = String( oParTypeIn );

  return;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::polBasisGet

Description:
------------
This member function gets the polarization basis ("L" or "C") from the new
format calibration table.

Inputs:
-------
oTableName - This reference to a String instance contains the new format
             calibration table name.

Outputs:
--------
The reference to the String instance containing the polarization basis, returned
via the function value.

Modification history:
---------------------
2012 Jan 20 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

String CalAnalysis::polBasisGet( const String& oTableName ) {

  // Get the record containing the main table keywords

  Table oTable( oTableName );
  TableProxy oTP( oTable );
  Record oR( oTP.getKeywordSet( String("") ) );


  // Get the polarization basis, make it upper case, keep only the initial
  // letter, and return it

  uInt uiIndex = oR.fieldNumber( String( "PolBasis" ) );
  String poPolBasis = RecordFieldPtr<String>(oR,uiIndex).get();

  poPolBasis.upcase();
  poPolBasis.operator=( poPolBasis.operator[](0) );

  return poPolBasis;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::polBasisSet

Description:
------------
This member function sets the polarization basis ("L" or "C") private variable.

Inputs:
-------
oPolBasisIn - This reference to a String instance contains the polarization
              basis.

Outputs:
--------
None.

Modification history:
---------------------
2012 Apr 04 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

void CalAnalysis::polBasisSet( const String& oPolBasisIn ) {

  // Set the polarization basis and return

  oPolBasis = String( oPolBasisIn );

  return;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::fieldGet

Description:
------------
This member function gets the field numbers from the new format calibration
table.

Inputs:
-------
oTableName - This reference to a String instance contains the new format
             calibration table name.

Outputs:
--------
The Vector<uInt> instance containing the field numbers,
returned via the function value.

Modification history:
---------------------
2012 Apr 17 - Nick Elias, NRAO
              Initial version.
2012 May 02 - Nick Elias, NRAO
              Fields are now obtained directly from the FIELD_ID column of the
              main table.

*/

// -----------------------------------------------------------------------------

Vector<uInt> CalAnalysis::fieldGet( const String& oTableName ) {

  // Get the field numbers from the new format calibration table

  Table oTable( oTableName, Table::Old );
  ScalarColumn<Int> oROSC( oTable, String("FIELD_ID") );

  Vector<Int> oFieldInt;
  oROSC.getColumn( oFieldInt, true );

  Vector<Int> oFieldUnique;
  unique<Int>(oFieldInt, oFieldUnique);

  Vector<uInt> field( oFieldUnique.nelements() );
  convertArray<uInt,Int>( field, oFieldUnique );

  return field;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::fieldSet

Description:
------------
This member function sets the field numbers private variables.

Inputs:
-------
oFieldIn - This reference to a Vector<uInt> instance contains the field numbers.

Outputs:
--------
None.

Modification history:
---------------------
2012 Apr 17 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

void CalAnalysis::fieldSet( const Vector<uInt>& oFieldIn ) {

  // Set the number of fields and fields and return

  uiNumField = oFieldIn.nelements();
  oField = Vector<uInt>( oFieldIn.copy() );

  return;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::fieldCheck

Description:
------------
This member function checks the input field vector and returns the fixed field
vector.

Inputs:
-------
oFieldIn - This reference to a Vector<uInt> instance contains the field numbers.

Outputs:
--------
oFieldOut - This reference to the Vector<uInt> instance contains the checked and
            fixed field numbers.
The Bool variable containing the check boolean, returned via
the function value.

Modification history:
---------------------
2012 Apr 17 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

Bool CalAnalysis::fieldCheck( const Vector<uInt>& oFieldIn,
    Vector<uInt>& oFieldOut ) const {

  // Declare the success boolean

  Bool success = false;


  // Check the input field numbers and return the fixed field numbers

  if ( oFieldIn.nelements() == 0 ) {
    success = false;
    return success;
  }

  uInt uiNumFieldOut = 0;
  oFieldOut.resize();

  for ( uInt f=0; f<oFieldIn.nelements(); f++ ) {
    if ( exists<uInt>( oFieldIn[f], oField ) ) {
      uiNumFieldOut += 1;
      oFieldOut.resize( uiNumFieldOut, true );
      oFieldOut[uiNumFieldOut-1] = oFieldIn[f];
    }
  }

  if ( uiNumFieldOut == 0 ) {
    success = false;
    return success;
  }


  // Return true

  success = true;

  return success;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::antennaGet

Description:
------------
This member function gets the antenna numbers from the new format calibration
table.

Inputs:
-------
oTableName - This reference to a String instance contains the new format
             calibration table name.

Outputs:
--------
The Vector<uInt> instance containing the antenna numbers,
returned via the function value.

Modification history:
---------------------
2012 Apr 17 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

Vector<uInt> CalAnalysis::antennaGet( const String& oTableName ) {

  // Create a temporary antenna subtable instance and get the number of antennas
  // (the number of rows)

  Table oTableAntenna( oTableName+"/ANTENNA", Table::Old );
  uInt uiNumRow = oTableAntenna.nrow();


  // Create the vector containing the antenna numbers and return it

  Vector<uInt> antenna( uiNumRow );
  indgen<uInt>( antenna, 0 );

  return antenna;


}

// -----------------------------------------------------------------------------

/*

CalAnalysis::antennaSet

Description:
------------
This member function sets the antenna numbers private variables.

Inputs:
-------
oAntennaIn - This reference to a Vector<uInt> instance contains the antenna
             numbers.

Outputs:
--------
None.

Modification history:
---------------------
2012 Apr 17 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

void CalAnalysis::antennaSet( const Vector<uInt>& oAntennaIn ) {

  // Set the number of antennas and antennas and return

  uiNumAntenna = oAntennaIn.nelements();
  oAntenna = Vector<uInt>( oAntennaIn.copy() );

  return;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::antenna1Get

Description:
------------
This member function gets the antenna 1 numbers from the new format calibration
table.

Inputs:
-------
oTableName - This reference to a String instance contains the new format
             calibration table name.

Outputs:
--------
The Vector<uInt> instance containing the antenna 1 numbers,
returned via the function value.

Modification history:
---------------------
2012 May 02 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

Vector<uInt> CalAnalysis::antenna1Get( const String& oTableName ) {

  // Get the antenna 1 numbers from the new format calibration table

  Table oTable( oTableName, Table::Old );
  ScalarColumn<Int> oROSC( oTable, String("ANTENNA1") );

  Vector<Int> oAntenna1Int;
  oROSC.getColumn( oAntenna1Int, true );

  Vector<Int> oAntenna1Unique;
  unique<Int>(oAntenna1Int, oAntenna1Unique);

  Vector<uInt> antenna1( oAntenna1Unique.nelements() );
  convertArray<uInt,Int>( antenna1, oAntenna1Unique );

  return antenna1;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::antenna1Set

Description:
------------
This member function sets the antenna 1 numbers private variables.

Inputs:
-------
oAntenna1In - This reference to a Vector<uInt> instance contains the antenna 1
              numbers.

Outputs:
--------
None.

Modification history:
---------------------
2012 May 02 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

void CalAnalysis::antenna1Set( const Vector<uInt>& oAntenna1In ) {

  // Set the number of antenna 1 and antenna 1 numbers and return

  uiNumAntenna1 = oAntenna1In.nelements();
  oAntenna1 = Vector<uInt>( oAntenna1In.copy() );

  return;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::antenna1Check

Description:
------------
This member function checks the input antenna 1 vector and returns the fixed
antenna 1 vector.

Inputs:
-------
oAntenna1In - This reference to a Vector<uInt> instance contains the antenna 1
              numbers.

Outputs:
--------
oAntenna1Out - This reference to the Vector<uInt> instance contains the checked
               and fixed antenna 1 numbers.
The Bool variable containing the check boolean, returned via
the function value.

Modification history:
---------------------
2012 Apr 17 - Nick Elias, NRAO
              Initial version.
2012 Apr 25 - Nick Elias, NRAO
              Member function antennaCheck() renamed to antenna1Check().
2012 May 02 - Nick Elias, NRAO
              The comparison is now made to an internal list of antenna 2.

*/

// -----------------------------------------------------------------------------

Bool CalAnalysis::antenna1Check( const Vector<uInt>& oAntenna1In,
    Vector<uInt>& oAntenna1Out ) const {

  // Declare the success boolean

  Bool success(false);


  // Check the input antenna 1 numbers and return the fixed antenna 1 numbers

  if ( oAntenna1In.nelements() == 0 ) {
    success = false;
    return success;
  }

  uInt uiNumAntenna1Out = 0;
  oAntenna1Out.resize();

  for ( uInt a=0; a<oAntenna1In.nelements(); a++ ) {
    if ( exists<uInt>( oAntenna1In[a], oAntenna1 ) ) {
      uiNumAntenna1Out += 1;
      oAntenna1Out.resize( uiNumAntenna1Out, true );
      oAntenna1Out[uiNumAntenna1Out-1] = oAntenna1In[a];
    }
  }

  if ( uiNumAntenna1Out == 0 ) {
    success = false;
    return success;
  }


  // Return true

  success = true;

  return success;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::antenna2Get

Description:
------------
This member function gets the antenna 2 numbers from the new format calibration
table.

Inputs:
-------
oTableName - This reference to a String instance contains the new format
             calibration table name.

Outputs:
--------
The Vector<Int> instance containing the antenna 2 numbers,
returned via the function value.

Modification history:
---------------------
2012 May 02 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

Vector<Int> CalAnalysis::antenna2Get( const String& oTableName ) {

  // Get the antenna 2 numbers from the new format calibration table

  Table oTable( oTableName, Table::Old );
  ScalarColumn<Int> oROSC( oTable, String("ANTENNA2") );

  Vector<Int> oAntenna2Int;
  oROSC.getColumn( oAntenna2Int, true );


  Vector<Int> uniqueA2;
  unique<Int>(oAntenna2Int, uniqueA2);

  Vector<Int> antenna2( uniqueA2 );

  return antenna2;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::antenna2Set

Description:
------------
This member function sets the antenna 2 numbers private variables.

Inputs:
-------
oAntenna2In - This reference to a Vector<uInt> instance contains the antenna 2
              numbers.

Outputs:
--------
None.

Modification history:
---------------------
2012 May 02 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

void CalAnalysis::antenna2Set( const Vector<Int>& oAntenna2In ) {

  // Set the number of antenna 2 and antenna 2 numbers and return

  uiNumAntenna2 = oAntenna2In.nelements();
  oAntenna2 = Vector<Int>( oAntenna2In.copy() );

  return;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::antenna2Check

Description:
------------
This member function checks the input antenna 2 vector and returns the fixed
antenna 2 vector.

Inputs:
-------
oAntenna2In - This reference to a Vector<Int> instance contains the antenna 2
              numbers.

Outputs:
--------
oAntenna2Out - This reference to the Vector<Int> instance contains the checked
               and fixed antenna 2 numbers.
The Bool variable containing the check boolean, returned via
the function value.

Modification history:
---------------------
2012 Apr 25 - Nick Elias, NRAO
              Initial version.
2012 May 02 - Nick Elias, NRAO
              The comparison is now made to an internal list of antenna 2.

*/

// -----------------------------------------------------------------------------

Bool CalAnalysis::antenna2Check( const Vector<Int>& oAntenna2In,
    Vector<Int>& oAntenna2Out ) const {

  // Declare the success boolean

  Bool success( false );


  // Check the input antenna 1 numbers and return the fixed antenna 1 numbers

  if ( oAntenna2In.nelements() == 0 ) {
    success = false;
    return success;
  }

  uInt uiNumAntenna2Out = 0;
  oAntenna2Out.resize();

  for ( uInt a=0; a<oAntenna2In.nelements(); a++ ) {
    if ( oAntenna2In[a] == -1 || exists<Int>( oAntenna2In[a], oAntenna2 ) ) {
      uiNumAntenna2Out += 1;
      oAntenna2Out.resize( uiNumAntenna2Out, true );
      oAntenna2Out[uiNumAntenna2Out-1] = oAntenna2In[a];
    }
  }

  if ( uiNumAntenna2Out == 0 ) {
    success = false;
    return success;
  }


  // Return true

  success = true;

  return success;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::timeGet

Description:
------------
This member function gets the times from the new format calibration table.

Inputs:
-------
oTableName - This reference to a String instance contains the new format
             calibration table name.

Outputs:
--------
The Vector<Double> instance containing the times, returned via
the function value.

Modification history:
---------------------
2012 Apr 04 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

Vector<Double> CalAnalysis::timeGet( const String& oTableName ) {

  // Create a temporary new format calibration table instance

  Table oTable( oTableName, Table::Old );
  ScalarColumn<Double> oROSC( oTable, String("TIME") );


  // Get the sorted and unique time stamps

  Vector<Double> oTimeTemp;
  oROSC.getColumn( oTimeTemp, true );

  Vector<Double> timev( 0 );
  unique<Double>( oTimeTemp, timev );


  // Return the time stamps

  return timev;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::timeSet

Description:
------------
This member function sets the times private variables.

Inputs:
-------
oTimeIn - This reference to a Vector<Double> instance contains the times.

Outputs:
--------
None.

Modification history:
---------------------
2012 Apr 04 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

void CalAnalysis::timeSet( const Vector<Double>& oTimeIn ) {

  // Set the number of times and times and return

  uiNumTime = oTimeIn.nelements();
  oTime = Vector<Double>( oTimeIn.copy() );

  return;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::timeCheck

Description:
------------
This member function checks the time range and returns the corresponding time
vector.

Inputs:
-------
dStartTimeIn - This reference to a Double variable contains the start time.
dStopTimeIn  - This reference to a Double variable contains the stop time.

Outputs:
--------
dStartTimeOut - This reference to a Double variable contains the start time.
dStopTimeOut  - This reference to a Double variable contains the stop time.
oTimeOut      - This reference to the Vector<Double> instance containing the
                time stamps.
The Bool variable containing the check boolean, returned via
the function value.

Modification history:
---------------------
2012 Jan 20 - Nick Elias, NRAO
              Initial version.
2012 May 06 - Nick Elias, NRAO
              Output start and stop times added.

*/

// -----------------------------------------------------------------------------

Bool CalAnalysis::timeCheck( const Double& dStartTimeIn,
    const Double& dStopTimeIn, Double& dStartTimeOut, Double& dStopTimeOut,
    Vector<Double>& oTimeOut ) const {

  // Declare the success boolean

  Bool success( false );


  // Check the start and stop times

  if ( dStartTimeIn > dStopTimeIn ) {
    success = false;
    return success;
  }

  if ( dStartTimeIn > oTime[uiNumTime-1] ) {
    success = false;
    return success;
  }

  if ( dStopTimeIn < oTime[0] ) {
    success = false;
    return success;
  }


  // Get the unique time values and the new start and stop times

  uInt uiNumTimeOut = 0;
  oTimeOut.resize();

  for ( uInt t=0; t<uiNumTime; t++ ) {
    if ( oTime[t] >= dStartTimeIn && oTime[t] <= dStopTimeIn ) {
      oTimeOut.resize( ++uiNumTimeOut, true );
      oTimeOut[uiNumTimeOut-1] = oTime[t];
    }
  }

  if ( oTimeOut.nelements() == 0 ) {
    success = false;
    return success;
  }

  dStartTimeOut = min( oTimeOut );
  dStopTimeOut = max( oTimeOut );


  // Return true

  success = true;

  return success;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::feedGet

Description:
------------
This member function gets the feeds from the new format calibration table.

NB: If the number of feeds in the column is a function of row, this function
will fail.

Inputs:
-------
oTableName - This reference to a String instance contains the new format
             calibration table name.

Outputs:
--------
The Vector<String> instance containing the feeds, returned via
the function value.

Modification history:
---------------------
2012 Apr 04 - Nick Elias, NRAO
              Initial version.
2012 May 02 - Nick Elias, NRAO
              The new format calibration table iterator is no longer used.  A
              call to the main table is now used instead.

*/

// -----------------------------------------------------------------------------

Vector<String> CalAnalysis::feedGet( const String& oTableName ) {

  // Get the number of feeds from the new format calibration table

  Table oTable( oTableName, Table::Old );

  Array<Float> oParamErrCell0;

  try {

    ArrayColumn<Float> oROAC( oTable, String("PARAMERR") );

    oParamErrCell0.resize( oROAC(0).shape() );
    oParamErrCell0 = oROAC( 0 );

  }

  catch ( AipsError oAipsError ) {
    throw( oAipsError );
  }


  // Set the feeds

  uInt uiNumFeedTemp = oParamErrCell0.shape()[0];
  Vector<String> feed( uiNumFeedTemp, "" );

  if ( uiNumFeedTemp == 1 ) {
    feed[0] = "S";
  } else {
    if ( polBasisGet(oTableName) == "L" ) {
      feed[0] = "X";
      feed[1] = "Y";
    } else if ( polBasisGet(oTableName) == "R" ) {
      feed[0] = "R";
      feed[1] = "L";
    } else {
      feed[0] = "1";
      feed[1] = "2";
    }
  }


  // Return the feeds

  return feed;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::feedSet

Description:
------------
This member function sets the feeds private variables.

Inputs:
-------
oFeedIn - This reference to a Vector<String> instance contains the feeds.

Outputs:
--------
None.

Modification history:
---------------------
2012 Apr 04 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

void CalAnalysis::feedSet( const Vector<String>& oFeedIn ) {

  // Set the number of feeds and feeds and return

  uiNumFeed = oFeedIn.nelements();
  oFeed = Vector<String>( oFeedIn.copy() );

  return;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::feedCheck

Description:
------------
This member function checks the input feed vector and returns the fixed feed
vector.

Inputs:
-------
oFeedIn - This reference to a Vector<String> instance contains the feeds ("R"
          and "L" for circular, "X" and "Y" for linear).

Outputs:
--------
oFeedOut - This reference to the Vector<String> instance contains the checked
           and fixed feeds.
The Bool variable containing the check boolean, returned via
the function value.

Modification history:
---------------------
2012 Jan 20 - Nick Elias, NRAO
              Initial version.
2012 Mar 13 - Nick Elias, NRAO
              Now checking for null.

*/

// -----------------------------------------------------------------------------

Bool CalAnalysis::feedCheck( const Vector<String>& oFeedIn,
    Vector<String>& oFeedOut ) const {

  // Declare the success boolean

  Bool success( false );


  // Check the input feed values

  if ( oFeedIn.nelements() == 0 ) {
    success = false;
    return success;
  }

  uInt uiNumFeedOut = 0;
  oFeedOut.resize();

  for ( uInt f=0; f<oFeedIn.nelements(); f++ ) {
    if ( exists<String>( oFeedIn[f], oFeed ) ) {
      uiNumFeedOut += 1;
      oFeedOut.resize( uiNumFeedOut, true );
      oFeedOut[uiNumFeedOut-1] = oFeedIn[f];
    }
  }

  if ( uiNumFeedOut == 0 ) {
    success = false;
    return success;
  }


  // Get the unique feed vector

  oFeedOut.resize();
  unique<String>( oFeedIn, oFeedOut );

  if ( oFeedOut.nelements() > 2 ) {
    success = false;
    return success;
  }

  if ( oFeedOut.nelements() == 1 ) {
    if ( oFeedOut[0] != "R" && oFeedOut[0] != "L" &&
         oFeedOut[0] != "X" && oFeedOut[0] != "Y" &&
         oFeedOut[0] != "1" && oFeedOut[0] != "2" && oFeedOut[0] != "S" ) {
      success = false;
      return success;
    }
  }

  if ( oFeedOut.nelements() == 2 ) {

    if ( oFeedOut[0] != "R" && oFeedOut[0] != "L" &&
         oFeedOut[0] != "X" && oFeedOut[0] != "Y" &&
         oFeedOut[0] != "1" && oFeedOut[0] != "2" ) {
      success = false;
      return success;
    }

    if ( oFeedOut[1] != "R" && oFeedOut[1] != "L" &&
         oFeedOut[1] != "X" && oFeedOut[1] != "Y" &&
         oFeedOut[1] != "1" && oFeedOut[1] != "2" ) {
      success = false;
      return success;
    }

    if ( oFeedOut[0] == "L" && oFeedOut[1] == "R" ) {
      oFeedOut[0] = "R";
      oFeedOut[1] = "L";
    }

    if ( oFeedOut[0] == "Y" && oFeedOut[1] == "X" ) {
      oFeedOut[0] = "X";
      oFeedOut[1] = "Y";
    }

    if ( oFeedOut[0] == "2" && oFeedOut[1] == "1" ) {
      oFeedOut[0] = "1";
      oFeedOut[1] = "2";
    }

  }


  // Return true

  success = true;

  return success;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::spwInfoGet

Description:
------------
This member function gets the spectral window information from the new format
calibration table.

Inputs:
-------
oTableName - This reference to a String instance contains the new format
             calibration table name.

Outputs:
--------
The reference to the SPW_INFO instance containing the spws, returned via the
function value.

Modification history:
---------------------
2012 Apr 17 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

CalAnalysis::SPW_INFO& CalAnalysis::spwInfoGet( const String& oTableName ) {

  // Get the spectral window information and return it

  CalAnalysis::SPW_INFO* poSPWInfo;
  poSPWInfo = new CalAnalysis::SPW_INFO( oTableName );

  return( *poSPWInfo );

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::spwInfoSet

Description:
------------
This member function sets the spectral window information private variables.

Inputs:
-------
oSPWInfoIn - This reference to the SPW_INFO instance contains the spectral
             window information.

Outputs:
--------
None.

Modification history:
---------------------
2012 Apr 17 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

void CalAnalysis::spwInfoSet( const CalAnalysis::SPW_INFO& oSPWInfoIn ) {

  // Set the spectral window information and return

  oSPWInfo = CalAnalysis::SPW_INFO( oSPWInfoIn );

  return;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::statsCheckInput

Description:
------------
This member function checks and fixes (if possible) the inputs to the
CalAnalysis::stats<T>() member function.

Inputs:
-------
oInputIn - This reference to an INPUT instance contains the inputs to the
CalAnalysis::stats<T>() member function.

Outputs:
--------
oInputOut - This reference to an INPUT instance contains the checked and fixed
            inputs to the CalAnalysis::stats<T>() member function.
The Bool variable containing the check boolean, returned via
the function value.

Modification history:
---------------------
2012 Apr 27 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

Bool CalAnalysis::statsCheckInput( const CalAnalysis::INPUT& oInputIn,
    CalAnalysis::INPUT& oInputOut ) {

  // Declare the success boolean

  Bool success( false );


  // Check the fields and create a new field vector

  if ( !fieldCheck( oInputIn.oField, oInputOut.oField ) ) {
    LogIO log( LogOrigin( "CalAnalysis", "statsCheckInput()", WHERE ) );
    log << LogIO::WARN << "One or more invalid fields" << LogIO::POST;
    success = false;
    return success;
  }


  // Check the antennas and create the new antenna vectors

  if ( !antenna1Check( oInputIn.oAntenna1, oInputOut.oAntenna1 ) ) {
    LogIO log( LogOrigin( "CalAnalysis", "statsCheckInput()", WHERE ) );
    log << LogIO::WARN << "One or more invalid antenna 1" << LogIO::POST;
    success = false;
    return success;
  }

  if ( !antenna2Check( oInputIn.oAntenna2, oInputOut.oAntenna2 ) ) {
    LogIO log( LogOrigin( "CalAnalysis", "statsCheckInput<T>()", WHERE ) );
    log << LogIO::WARN << "One or more invalid antenna 2" << LogIO::POST;
    success = false;
    return success;
  }


  // Check the time range and create the corresponding time vector

  Bool bTimeCheck = timeCheck( oInputIn.dStartTime, oInputIn.dStopTime,
      oInputOut.dStartTime, oInputOut.dStopTime, oInputOut.oTime );

  if ( !bTimeCheck ) {
    LogIO log( LogOrigin( "CalAnalysis", "statsCheckInput<T>()", WHERE ) );
    log << LogIO::WARN << "Invalid start and/or stop times" << LogIO::POST;
    success = false;
    return success;
  }


  // Check the feeds and create the new feed vector

  if ( !feedCheck( oInputIn.oFeed, oInputOut.oFeed ) ) {
    LogIO log( LogOrigin( "CalAnalysis", "statsCheckInput<T>()", WHERE ) );
    log << LogIO::WARN << "Invalid feed(s)" << LogIO::POST;
    success = false;
    return success;
  }


  // Check the spectral window info and create the frequency vector

  Vector<uInt> oSPWOld( oInputIn.oSPW );
  Vector<Vector<uInt> > oChannelOld( oInputIn.oChannel );

  Vector<uInt> oSPWNew;
  Vector<Vector<uInt> > oChannelNew;

  Bool bSPWCheck = oSPWInfo.spwInfoCheck( oInputIn.oSPW, oInputIn.oChannel,
      oInputOut.oSPW, oInputOut.oChannel );

  if ( !bSPWCheck ) {
    LogIO log( LogOrigin( "CalAnalysis", "statsCheckInput<T>()", WHERE ) );
    log << LogIO::WARN << "Invalid spectral window information" << LogIO::POST;
    success = false;
    return success;
  }


  // Check the user-defined iteration axis

  if ( oInputIn.eAxisIterUserID != CalStats::FREQUENCY &&
       oInputIn.eAxisIterUserID != CalStats::TIME ) {
    LogIO log( LogOrigin( "CalAnalysis", "statsCheckInput<T>()", WHERE ) );
    log << LogIO::WARN << "User-defined iteration axis must be frequency/time"
        << LogIO::POST;
    success = false;
    return success;
  }

  oInputOut.eAxisIterUserID = oInputIn.eAxisIterUserID;


  // Check the RAP (real, amplitude, phase) parameter

  if ( oInputIn.eRAP != CalAnalysis::REAL &&
       oInputIn.eRAP != CalAnalysis::AMPLITUDE &&
       oInputIn.eRAP != CalAnalysis::PHASE ) {
    LogIO log( LogOrigin( "CalAnalysis", "statsCheckInput<T>()", WHERE ) );
    log << LogIO::WARN << "Invalid RAP parameter" << LogIO::POST;
    success = false;
    return success;
  }

  oInputOut.eRAP = oInputIn.eRAP;


  // Set the normalization and unwrap booleans

  oInputOut.bNorm = oInputIn.bNorm;

  oInputOut.bUnwrap = oInputIn.bUnwrap;


  // Check the maximum phase jump parameter

  if ( oInputIn.dJumpMax < 0.0 ) {
    LogIO log( LogOrigin( "CalAnalysis", "statsCheckInput<T>()", WHERE ) );
    log << LogIO::WARN << "Invalid maximum jump parameter" << LogIO::POST;
    success = false;
    return success;
  }

  oInputOut.dJumpMax = oInputIn.dJumpMax;


  // Return true

  success = true;

  return success;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::getGroup

Description:
------------
This member function gets input selected rows from a new format calibration
table and groups them according to unique (field,antenna1,antenna2).

NB: Each element of the output Vector<T>() instances corresponds to a group.

NB: Each group has a vector of unique times and frequencies (which are not
necessarily unique).

NB: Each group has data cubes, which are dimensioned feed x frequency x time.
Each cube can have data from multiple rows, depending on the input elections.
E.g., channels from multiple spectral windows are concatenated in the data
cubes.

NB: The float parameter cube is empty for a new format calibration table with
complex gains.  Conversely, the complex parameter is empty for a new format
calibration table wth real values.

NB: Sometimes the cubes cannot be completely filled because of the shape of the
new format calibration table.  In this case, the empty cube elements are set to
zero and the corresponding flags set to true.

Inputs:
-------
oNCT             - This NewCalTable instance contains the new format calibration
                   table.
oInput           - This INPUT instance contains the input selections.
oFieldGroup      - This Vector<uInt> instance contains the fields for each
                   group.
oAntenna1Group   - This Vector<uInt> instance contains the antenna 1 for each
                   group.
oAntenna2Group   - This Vector<Int> instance contains the antenna 2 for each
                   group.
oTimeUniqueGroup - This Vector<Vector<Double> > instance contains the unique
                   times for each group.
oFreqGroup       - This Vector<Vector<Double> >instance contains the frequencies
                   for each group.  They can come from multiple spectral windows
                   (which are in different rows).

Outputs:
--------
oCParamGroup   - This Vector<Cube<DComplex> > instance contains the complex
                 parameters.  This instance is empty for new format calibration
                 tables with real parameters.
oFParamGroup   - This Vector<Cube<Double> > instance contains the real
                 parameters.  This instance is empty for new format calibration
                 tables with complex parameters.
oParamErrGroup - This Vector<Cube<Double> > instance contains the parameter
                 errors.
oFlagGroup     - This Vector<Cube<Bool> > instance contains the flags.
The Bool variable containing the check boolean, returned via
the function value.

Modification history:
---------------------
2012 May 06 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

Bool CalAnalysis::getGroup( const NewCalTable& oNCT,
    const CalAnalysis::INPUT& oInput, Vector<uInt>& oFieldGroup,
    Vector<uInt>& oAntenna1Group, Vector<Int>& oAntenna2Group,
    Vector<Vector<Double> >& oTimeUniqueGroup,
    Vector<Vector<Double> >& oFreqGroup, Vector<Cube<DComplex> >& oCParamGroup,
    Vector<Cube<Double> >& oFParamGroup, Vector<Cube<Double> >& oParamErrGroup,
    Vector<Cube<Bool> >& oFlagGroup ) const {

  // Declare the success boolean

  Bool success( false );


  // Get the selected rows

  Vector<uInt> oRowSelect;

  if ( !rowSelect( oNCT, oInput, oRowSelect ) ) {
    LogIO log( LogOrigin( "CalAnalysis", "getGroup()", WHERE ) );
    log << LogIO::WARN << "No matching rows selected" << LogIO::POST;
    success = false;
    return success;
  }


  // Group the selected rows according to unique combinations of field, antenna
  // 1, and antenna 2

  Vector<Vector<uInt> > oRowGroup;

  Vector<Vector<uInt> > oSPWGroup;
  Vector<Vector<uInt> > oSPWUniqueGroup;

  Vector<Vector<Double> > oTimeGroup;

  Bool bRowGroup = rowGroup( oNCT, oRowSelect, oRowGroup, oFieldGroup,
    oAntenna1Group, oAntenna2Group, oSPWGroup, oSPWUniqueGroup, oTimeGroup,
    oTimeUniqueGroup );

  if ( !bRowGroup ) {
    LogIO log( LogOrigin( "CalAnalysis", "getGroup()", WHERE ) );
    log << LogIO::WARN << "Cannot group the data" << LogIO::POST;
    success = false;
    return success;
  }


  // Get the frequency vector, start channel vector, and relative channel vector
  // for each group

  Bool bFreqGroup = freqGroup( oInput, oSPWUniqueGroup, oFreqGroup );

  if ( !bFreqGroup ) {
    LogIO log( LogOrigin( "CalAnalysis", "getGroup()", WHERE ) );
    log << LogIO::WARN << "Cannot group frequencies" << LogIO::POST;
    success = false;
    return success;
  }


  // Get the cubes for each group

  Bool bCubeGroup = cubeGroup( oNCT, oInput, oRowGroup, oSPWGroup,
      oSPWUniqueGroup, oTimeGroup, oTimeUniqueGroup, oFreqGroup, oCParamGroup,
      oFParamGroup, oParamErrGroup, oFlagGroup );

  if ( !bCubeGroup ) {
    LogIO log( LogOrigin( "CalAnalysis", "getGroup()", WHERE ) );
    log << LogIO::WARN << "Cannot group cubes" << LogIO::POST;
    success = false;
    return success;
  }


  // Return true

  success = true;

  return success;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::rowSelect

Description:
------------
This member function returns the rows selected by the input parameters.

Inputs:
-------
oNCT   - This NewCalTable instance contains the new format calibration table.
oInput - This INPUT instance contains the input selections.

Outputs:
--------
oRowSelect - This Vector<uInt> instance contains the row numbers selected by the
             inputs.
The Bool variable containing the check boolean, returned via
the function value.

Modification history:
---------------------
2012 May 06 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

Bool CalAnalysis::rowSelect( const NewCalTable& oNCT,
    const CalAnalysis::INPUT& oInput, Vector<uInt>& oRowSelect ) const {

  // Declare the success boolean

  Bool success( false );


  // Create the column accessors

  ScalarColumn<Int> oFieldCol( oNCT, String("FIELD_ID") );
  ScalarColumn<Int> oAntenna1Col( oNCT, String("ANTENNA1") );
  ScalarColumn<Int> oAntenna2Col( oNCT, String("ANTENNA2") );
  ScalarColumn<Int> oSPWCol( oNCT, String("SPECTRAL_WINDOW_ID") );
  ScalarColumn<Double> oTimeCol( oNCT, String("TIME") );


  // Initialize the number of rows in the new format calibration table, the
  // number of selected rows, and the Vector<uInt> instance containing the
  // selected rows

  uInt uiNumRow = oNCT.nrow();
  uInt uiNumRowSelect = 0;

  oRowSelect.resize();


  // Loop over rows and record the selected ones

  for ( uInt r=0; r<uiNumRow; r++ ) {

    // Get the information from each column for the present row
    uInt uiField = (uInt) oFieldCol( r );
    uInt uiAntenna1 = (uInt) oAntenna1Col( r );
    Int iAntenna2 = oAntenna2Col( r );
    uInt uiSPW = (uInt) oSPWCol( r );
    Double dTime = (Double) oTimeCol( r );

    // Are all of the quantities in the present row present in the input
    // parameters?  If not, don't do anything and increment the loop
    if ( !exists<uInt>( uiField, oInput.oField ) ) continue;
    if ( !exists<uInt>( uiAntenna1, oInput.oAntenna1 ) ) continue;
    if ( !exists<Int>( iAntenna2, oInput.oAntenna2 ) ) continue;
    if ( !exists<uInt>( uiSPW, oInput.oSPW ) ) continue;
    if ( !exists<Double>( dTime, oInput.oTime ) ) continue;

    // Record the selected row
    oRowSelect.resize( ++uiNumRowSelect, true );
    oRowSelect[uiNumRowSelect-1] = r;

  }


  // Were any rows found?  It not, return false

  if ( uiNumRowSelect == 0 ) {
    success = false;
    return success;
  }


  // Return true

  success = true;

  return success;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::rowGroup

Description:
------------
This member function returns the row numbers, fields, antenna 1s, antenna 2s,
spectral windows, unique spectral windows, times, and unique times grouped
according to selected rows and input selection.

Inputs:
-------
oNCT       - This NewCalTable instance contains the new format calibration
             table.
oRowSelect - This Vector<uInt> instance contains the row numbers selected by the
             inputs.

Outputs:
--------
oRowGroup        - This Vector<uInt> instance contains the row numbers for each
                   group.
oFieldGroup      - This Vector<uInt> instance contains the fields for each
                   group.
oAntenna1Group   - This Vector<uInt> instance contains the antenna 1 for each
                   group.
oAntenna2Group   - This Vector<Int> instance contains the antenna 2 for each
                   group.
oSPWGroup        - This Vector<Vector<uInt> > instance contains the spectral
                   windows for each group.
oSPWUniqueGroup  - This Vector<Vector<uInt> > instance contains the unique
                   spectral windows for each group.
oTimeGroup       - This Vector<Vector<Double> > instance contains the times for
                   each group.
oTimeUniqueGroup - This Vector<Vector<Double> > instance contains the unique
                   times for each group.
The Bool variable containing the check boolean, returned via
the function value.

Modification history:
---------------------
2012 May 06 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

Bool CalAnalysis::rowGroup( const NewCalTable& oNCT,
    const Vector<uInt>& oRowSelect, Vector<Vector<uInt> >& oRowGroup,
    Vector<uInt>& oFieldGroup, Vector<uInt>& oAntenna1Group,
    Vector<Int>& oAntenna2Group, Vector<Vector<uInt> >& oSPWGroup,
    Vector<Vector<uInt> >& oSPWUniqueGroup, Vector<Vector<Double> >& oTimeGroup,
    Vector<Vector<Double> >& oTimeUniqueGroup ) const {

  // Declare the success boolean

  Bool success( false );


  // If there are no rows, return false

  uInt uiNumRow = oRowSelect.nelements();

  if ( uiNumRow == 0 ) {
    success = false;
    return success;
  }


  // Create the column accessors

  ScalarColumn<Int> oFieldCol( oNCT, String("FIELD_ID") );
  ScalarColumn<Int> oAntenna1Col( oNCT, String("ANTENNA1") );
  ScalarColumn<Int> oAntenna2Col( oNCT, String("ANTENNA2") );
  ScalarColumn<Int> oSPWCol( oNCT, String("SPECTRAL_WINDOW_ID") );
  ScalarColumn<Double> oTimeCol( oNCT, String("TIME") );


  // Initialize the number of groups according to unique values of
  // (field,antenna1,antenna2) and the Vector<Vector<T> > instances containing
  // the groups

  uInt uiNumGroup = 0;

  oRowGroup.resize( uiNumGroup, false );

  oFieldGroup.resize( uiNumGroup, false );
  oAntenna1Group.resize( uiNumGroup, false );
  oAntenna2Group.resize( uiNumGroup, false );

  oSPWGroup.resize( uiNumGroup, false );
  oSPWUniqueGroup.resize( uiNumGroup, false );

  oTimeGroup.resize( uiNumGroup, false );
  oTimeUniqueGroup.resize( uiNumGroup, false );


  // Loop over the rows and form the groups

  for ( uInt r=0; r<uiNumRow; r++ ) {

    // Get the field, antenna 1, antenna2, spectral window, and time for the
    // present row
    uInt uiField = oFieldCol( oRowSelect[r] );
    uInt uiAntenna1 = oAntenna1Col( oRowSelect[r] );
    Int iAntenna2 = oAntenna2Col( oRowSelect[r] );
    uInt uiSPW = oSPWCol( oRowSelect[r] );
    Double dTime = oTimeCol( oRowSelect[r] );

    // Is this combination of field, antenna 1, and antenna 2 unique?  If so
    // create a new group, otherwise record the existing group.
    uInt g;
    for ( g=0; g<uiNumGroup; g++ ) {
      if ( uiField == oFieldGroup[g] && uiAntenna1 == oAntenna1Group[g] &&
           iAntenna2 == oAntenna2Group[g] ) {
        break;
      }
    }

    if ( uiNumGroup == 0 || g >= uiNumGroup ) { // New group

      // Increment the number of groups
      uiNumGroup++;

      // Add the row to the new group
      oRowGroup.resize( uiNumGroup, true );
      oRowGroup[uiNumGroup-1].resize( 1, true );
      oRowGroup[uiNumGroup-1][0] = oRowSelect[r];

      // Add the field to the new group
      oFieldGroup.resize( uiNumGroup, true );
      oFieldGroup[uiNumGroup-1] = uiField;

      // Add the antenna 1 to the new group
      oAntenna1Group.resize( uiNumGroup, true );
      oAntenna1Group[uiNumGroup-1] = uiAntenna1;

      // Add the antenna 2 to the new group
      oAntenna2Group.resize( uiNumGroup, true );
      oAntenna2Group[uiNumGroup-1] = iAntenna2;

      // Add the spectral window to the new group
      oSPWGroup.resize( uiNumGroup, true );
      oSPWGroup[uiNumGroup-1].resize( 1, true );
      oSPWGroup[uiNumGroup-1][0] = uiSPW;

      // Add the time to the new group
      oTimeGroup.resize( uiNumGroup, true );
      oTimeGroup[uiNumGroup-1].resize( 1, true );
      oTimeGroup[uiNumGroup-1][0] = dTime;

    } else { // Existing group
 
      // Increment the number of rows in the group
      uInt uiNumRowGroup = oRowGroup[g].nelements() + 1;

      // Add the row to the group
      oRowGroup[g].resize( uiNumRowGroup, true );
      oRowGroup[g][uiNumRowGroup-1] = oRowSelect[r];

      // Add the spectral window to the group
      oSPWGroup[g].resize( uiNumRowGroup, true );
      oSPWGroup[g][uiNumRowGroup-1] = uiSPW;

      // Add the time to the group
      oTimeGroup[g].resize( uiNumRowGroup, true );
      oTimeGroup[g][uiNumRowGroup-1] = dTime;

    }

  }


  // Create the unique sorted spectral window and time vectors for each group

  oSPWUniqueGroup.resize( uiNumGroup, false );
  oTimeUniqueGroup.resize( uiNumGroup, false );

  for ( uInt g=0; g<uiNumGroup; g++ ) {
    oSPWUniqueGroup[g].resize();
    unique<uInt>( oSPWGroup[g], oSPWUniqueGroup[g] );
    oTimeUniqueGroup[g].resize();
    unique<Double>( oTimeGroup[g], oTimeUniqueGroup[g] );
  }


  // Return true

  success = true;

  return success;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::chanSPW

Description:
------------
This member function maps the spectral windows to the input spectral windows (to
get the correct channels) and forms the start channels so that spectral windows
can be concatenated.

NB: The spectral windows must be unique and sorted.

Inputs:
-------
oSPW   - This Vector<uInt> instance contains the unique and sorted spectral
         windows.
oInput - This INPUT instance contains the input selections.

Outputs:
--------
oSPWMap    - This Vector<uInt> instance contains the mapped spectral windows.
oChanStart - This Vector<uInt> instance contains the start channels.
The Bool variable containing the check boolean, returned via
the function value.

Modification history:
---------------------
2012 May 06 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

Bool CalAnalysis::chanSPW( const Vector<uInt>& oSPW, const INPUT& oInput,
    Vector<uInt>& oSPWMap, Vector<uInt>& oChanStart ) const {

  // Declare the success boolean

  Bool success( false );


  // Initialize the number of spectral windows, the spectral window map and
  // start instances

  uInt uiNumSPW = oSPW.nelements();

  oSPWMap.resize( uiNumSPW, false );
  oChanStart.resize( uiNumSPW, false );


  // Load the spectral window map

  for ( uInt s=0; s<uiNumSPW; s++ ) {
    if ( !where( oSPW[s], oInput.oSPW, oSPWMap[s] ) ) {
      success = false;
      return success;
    }
  }


  // Load the start channels

  oChanStart[0] = 0;
  for ( uInt s=1; s<uiNumSPW; s++ ) {
    oChanStart[s] = oChanStart[s-1];
    oChanStart[s] += oInput.oChannel[oSPWMap[s-1]].nelements();
  }


  // Return true

  success = true;

  return success;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::freqGroup

Description:
------------
This member function concatenates the frequencies from multiple spectral windows
for each group.

Inputs:
-------
oInput          - This INPUT instance contains the input selections.
oSPWUniqueGroup - This Vector<Vector<uInt> > instance contains the unique
                  spectral windows for each group.

Outputs:
--------
oFreqGroup - This Vector<Vector<Double> >instance contains the frequencies (for
             each group.  They can come from multiple spectral windows (which
             are in different rows).
The Bool variable containing the success boolean, returned via
the function value.

Modification history:
---------------------
2012 May 06 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

Bool CalAnalysis::freqGroup( const INPUT& oInput,
    const Vector<Vector<uInt> >& oSPWUniqueGroup,
    Vector<Vector<Double> >& oFreqGroup ) const {

  // Declare the success boolean

  Bool success = false;


  // Create the instance for the spectral window subtable of the new format
  // calibration table and the accessor instance for the CHAN_FREQ column

  Table oTableSPW( oCalName+String("/SPECTRAL_WINDOW"), Table::Old );

  ArrayColumn<Double> oChanFreqCol( oTableSPW, String("CHAN_FREQ") );


  // Initialize the number of groups and the output instance

  uInt uiNumGroup = oSPWUniqueGroup.nelements();

  oFreqGroup.resize( uiNumGroup, false );


  // Load the output instances

  for ( uInt g=0; g<uiNumGroup; g++ ) {

    // Get the start channels for all spectral windows in the group
    Vector<uInt> oSPWMap;
    Vector<uInt> oChanStart;
    if ( !chanSPW( oSPWUniqueGroup[g], oInput, oSPWMap, oChanStart ) ) {
      success = false;
      return success;
    }

    for ( uInt s=0; s<oSPWUniqueGroup[g].nelements(); s++ ) {

      // Get the number of channels for the mapped spectral window
      uInt uiNumChannelInput = oInput.oChannel[oSPWMap[s]].nelements();

      // Resize the frequency vector for the group
      oFreqGroup[g].resize( oChanStart[s]+uiNumChannelInput, true );

      // Get the channels for the group and spectral window from the spectral
      // window subtable of the new format calibration table
      Vector<Double> oFreqTemp( oChanFreqCol(oSPWUniqueGroup[g][s]) );

      // Load the frequency vector for the group
      for ( uInt c=0; c<uiNumChannelInput; c++ ) {
        uInt uiChanGroup = oChanStart[s] + c;
	uInt uiChanTemp = oInput.oChannel[oSPWMap[s]][c];
        oFreqGroup[g][uiChanGroup] = oFreqTemp[uiChanTemp];
      }

    }

  }


  // Return true

  success = true;

  return success;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::cubeGroup

Description:
------------
This member function concatenates data from multiple rows into groups according
to unique (field,antenna1,antenna2).

Inputs:
-------
oNCT             - This NewCalTable instance contains the new format calibration
                   table.
oInput           - This INPUT instance contains the input selections.
oRowGroup        - This Vector<uInt> instance contains the row numbers for each
                   group.
oSPWGroup        - This Vector<Vector<uInt> > instance contains the spectral
                   windows for each group.
oSPWUniqueGroup  - This Vector<Vector<uInt> > instance contains the unique
                   spectral windows for each group.
oTimeGroup       - This Vector<Vector<Double> > instance contains the times for
                   each group.
oTimeUniqueGroup - This Vector<Vector<Double> > instance contains the unique
                   times for each group.
oFreqGroup       - This Vector<Vector<Double> >instance contains the frequencies
                   for each group.  They can come from multiple spectral windows
                   (which are in different rows).

Outputs:
--------
oCParamGroup   - This Vector<Cube<DComplex> > instance contains the complex
                 parameters.  This instance is empty for new format calibration
                 tables with real parameters.
oFParamGroup   - This Vector<Cube<Double> > instance contains the real
                 parameters.  This instance is empty for new format calibration
                 tables with complex parameters.
oParamErrGroup - This Vector<Cube<Double> > instance contains the parameter
                 errors.
oFlagGroup     - This Vector<Cube<Bool> > instance contains the flags.
The Bool variable containing the check boolean, returned via
the function value.

Modification history:
---------------------
2012 May 06 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

Bool CalAnalysis::cubeGroup( const NewCalTable& oNCT, const INPUT& oInput,
    const Vector<Vector<uInt> >& oRowGroup,
    const Vector<Vector<uInt> >& oSPWGroup,
    const Vector<Vector<uInt> >& oSPWUniqueGroup,
    const Vector<Vector<Double> >& oTimeGroup,
    const Vector<Vector<Double> >& oTimeUniqueGroup,
    const Vector<Vector<Double> >& oFreqGroup,
    Vector<Cube<DComplex> >& oCParamGroup, Vector<Cube<Double> >& oFParamGroup,
    Vector<Cube<Double> >& oParamErrGroup,
    Vector<Cube<Bool> >& oFlagGroup ) const {

  // Declare the success boolean

  Bool success( false );


  // Create accessors for the CPARAM/FPARAM, PARAMERR, and FLAG columns

  ArrayColumn<Complex> poCParamCol;
  ArrayColumn<Float> poFParamCol;

  if ( oParType == String("Complex") ) {
    poCParamCol = ArrayColumn<Complex>( oNCT, String("CPARAM") );
  } else {
    poFParamCol = ArrayColumn<Float>( oNCT, String("FPARAM") );
  }

  ArrayColumn<Float> oParamErrCol( oNCT, String("PARAMERR") );

  ArrayColumn<Bool> oFlagCol( oNCT, String("FLAG") );


  // Initialize the number of groups and the output instances

  uInt uiNumGroup = oRowGroup.nelements();

  oCParamGroup.resize( uiNumGroup, false );
  oFParamGroup.resize( uiNumGroup, false );
  oParamErrGroup.resize( uiNumGroup, false );
  oFlagGroup.resize( uiNumGroup, false );


  // Load the output instances for each group

  for ( uInt g=0; g<uiNumGroup; g++ ) { // Loop over group

    // Get the spectral window maps and start channels for the group
    Vector<uInt> oSPWMap;
    Vector<uInt> oChanStart;
    if ( !chanSPW( oSPWUniqueGroup[g], oInput, oSPWMap, oChanStart ) ) {
      success = false;
      return success;
    }

    // Initialize the shape of the output instances for the group
    uInt uiNumFreq = oFreqGroup[g].nelements();
    uInt uiNumTimeUnique = oTimeUniqueGroup[g].nelements();
    IPosition oShape( 3, uiNumFeed, uiNumFreq, uiNumTimeUnique );

    // Allocate the output instances for the group
    oCParamGroup[g].resize( oShape, false );
    oFParamGroup[g].resize( oShape, false );
    oParamErrGroup[g].resize( oShape, false );
    oFlagGroup[g].resize( oShape, false );

    // Initialize the output instances for the group
    oCParamGroup[g] = DComplex( 0.0, 0.0 );
    oFParamGroup[g] = 0.0;
    oParamErrGroup[g] = 0.0;
    oFlagGroup[g] = true;

    // Get the number of rows for the group
    uInt uiNumRow = oRowGroup[g].nelements();

    for ( uInt r=0; r<uiNumRow; r++ ) { // Loop over rows for the group

      // Get the row number in the new format calibration table
      uInt uiRow = oRowGroup[g][r];

      // Get the CPARAM or FPARAM cell from the new format calibration table
      Array<Complex> oCParamCube;
      Array<Float> oFParamCube;
      if ( oParType == String("Complex") ) {
        IPosition oShape( poCParamCol( uiRow ).shape() );
        oCParamCube.resize( oShape, false );
        oCParamCube = poCParamCol( uiRow );
        oFParamCube = Array<Float>( oShape, 0.0F );
      } else {
        IPosition oShape( poFParamCol(uiRow).shape() );
        oFParamCube.resize( oShape, false );
        oFParamCube = poFParamCol( uiRow );
        oCParamCube = Array<Complex>( oShape, Complex(0.0,0.0) );
      }

      // Get the PARAMERR cell from the new format calibration table
      Array<Float> oParamErrCube( oParamErrCol(uiRow).shape(), 0.0F );
      oParamErrCube = oParamErrCol( uiRow );

      // Get the FLAG cell from the new format calibration table
      Array<Bool> oFlagCube( oFlagCol(uiRow).shape(), false );
      oFlagCube = oFlagCol( uiRow );

      // Map the spectral window
      uInt s = 0;
      if ( !where( oSPWGroup[g][r], oSPWUniqueGroup[g], s ) ) {
        success = false;
        return success;
      }

      // Get the number of input channels and the channels for the mapped
      // spectral window
      uInt uiNumChannelC = oInput.oChannel[oSPWMap[s]].nelements();
      Vector<uInt> oChannelC( oInput.oChannel[oSPWMap[s]] );

      // Map the time
      uInt t = 0;
      if ( !where( oTimeGroup[g][r], oTimeUniqueGroup[g], t ) ) {
        success = false;
        return success;
      }

      // Map the cubes from the new format calibration tables to the output
      // instances for each group
      for ( uInt f=0; f<uiNumFeed; f++ ) { // Loop over feeds
        for ( uInt c=0; c<uiNumChannelC; c++ ) { // Loop over channnels
          IPosition oPosG( 3, f, oChanStart[s]+c, t );
          IPosition oPosC( 2, f, oChannelC[c] );
          oCParamGroup[g](oPosG) = (DComplex) oCParamCube(oPosC);
          oFParamGroup[g](oPosG) = (Double) oFParamCube(oPosC);
          oParamErrGroup[g](oPosG) = (Double) oParamErrCube(oPosC);
          oFlagGroup[g](oPosG) = oFlagCube(oPosC);
        }
      }

    }

  }


  // Return true

  success = true;

  return success;

}

// -----------------------------------------------------------------------------
// End of CalAnalysis private member functions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Start of CalAnalysis protected member functions
// -----------------------------------------------------------------------------

/*

CalAnalysis::CalAnalysis (default)

Description:
------------
This default constructor is unused by this class and unavailable when an
instance is created.

Inputs:
-------
None.

Outputs:
--------
None.

Modification history:
---------------------
2012 Jan 20 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

CalAnalysis::CalAnalysis( void ) {}

// -----------------------------------------------------------------------------

/*

CalAnalysis::CalAnalysis (copy)

Description:
------------
This copy constructor is unused by this class and unavailable when an instance
is created.

Inputs:
-------
oCalAnalysis - A reference to a CalAnalysis instance.

Outputs:
--------
None.

Modification history:
---------------------
2012 Jan 20 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

CalAnalysis::CalAnalysis( const CalAnalysis& oCalAnalysis ) {

  // Overwrite this instance and return.  This code will bomb.  I have written
  // it in this way to keep the compiler from spewing warning messages about
  // unused variables.

  *this = oCalAnalysis;

  return;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::operator=

Description:
------------
This operator= function is unused by this class and unavailable when an instance
is created.

Inputs:
-------
oCalAnalysis - A reference to a CalAnalysis instance.

Outputs:
--------
None.

Modification history:
---------------------
2012 Jan 20 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

CalAnalysis& CalAnalysis::operator=( const CalAnalysis& oCalAnalysis ) {

  // Copy the input instance and return it.  This code will bomb.  I have
  // written it in this way to keep the compiler from spewing warning messages
  // about unused variables.

  CalAnalysis* poCalAnalysis = new CalAnalysis( oCalAnalysis );

  return( *poCalAnalysis );

}

// -----------------------------------------------------------------------------
// End of CalAnalysis protected member functions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// End of CalAnalysis class
// -----------------------------------------------------------------------------

};

// -----------------------------------------------------------------------------
// End of casa namespace
// -----------------------------------------------------------------------------
