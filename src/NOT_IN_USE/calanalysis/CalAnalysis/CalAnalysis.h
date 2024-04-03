
// -----------------------------------------------------------------------------

/*

CalAnalysis.h

Description:
------------
This header file contains definitions for the CalStats class.

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
// Start of define macro to prevent multiple loading
// -----------------------------------------------------------------------------

#ifndef CAL_ANALYSIS_H
#define CAL_ANALYSIS_H

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <casacore/casa/BasicSL/String.h>

#include <iostream>
#include <sstream>
using namespace std;

#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/Logging/LogIO.h>

#include <casacore/casa/Containers/Block.h>

#include <casacore/casa/Arrays/Array.h>
#include <casacore/casa/Arrays/ArrayIter.h>
#include <casacore/casa/Arrays/Cube.h>
#include <casacore/casa/Arrays/Slicer.h>
#include <casacore/casa/Arrays/ArrayMath.h>

#include <casacore/tables/Tables/Table.h>
#include <casacore/tables/Tables/TableProxy.h>

#include <casacore/casa/Containers/Record.h>
#include <casacore/casa/Containers/RecordField.h>

#include <casacore/casa/Utilities/GenSort.h>

#include <casacore/ms/MeasurementSets/MSSpectralWindow.h>
#include <casacore/ms/MeasurementSets/MSSpWindowColumns.h>

#include <synthesis/CalTables/NewCalTable.h>
#include <synthesis/CalTables/CTIter.h>

#include <calanalysis/CalAnalysis/CalStats.h>
#include <calanalysis/CalAnalysis/CalStatsDerived.h>
#include <calanalysis/CalAnalysis/CalStatsFitter.h>

// -----------------------------------------------------------------------------
// Start of casa namespace
// -----------------------------------------------------------------------------

namespace casa {

// -----------------------------------------------------------------------------
// Start of CalAnalysis class definition
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
msName       - This member function returns the associated casacore::MS name.
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
msNameGet       - This member function gets the associated casacore::MS name from the new
                  format calibration table.
msNameSet       - This member function sets the associated casacore::MS name private
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
              casacore::Template private member functions parse<T>() and select<T>()
              removed.  casacore::Template static public member function where<T>() added.
              Private member functions getGroup(), rowSelect(), rowGroup(),
              chanSPW(), freqGroup(), and cubeGroup() added.
2012 May 07 - Nick Elias, NRAO
              Public member functions antenna1() and antenna2() added.

*/

// -----------------------------------------------------------------------------

class CalAnalysis {

  public:

    // Real/Amplitude/Phase enums
    typedef enum RAP {
      INIT=-1, REAL=0, AMPLITUDE, PHASE
    } RAP;

    // SPW_INFO nested class
    class SPW_INFO {
      public:
        casacore::Bool bValid;
        casacore::uInt uiNumSPW;
        casacore::Vector<casacore::uInt> oSPW;
        casacore::Vector<casacore::uInt> oNumChannel;
        casacore::Vector<casacore::Vector<casacore::Double> > oFrequency;
        SPW_INFO( const casacore::String& oTableName );
        SPW_INFO( const SPW_INFO& oSPWInfoIn );
        SPW_INFO( void );
        ~SPW_INFO( void );
        SPW_INFO& operator=( const SPW_INFO& oSPWInfoIn );
        casacore::Bool freq( const casacore::Vector<casacore::uInt>& oSPWIn,
            const casacore::Vector<casacore::Vector<casacore::uInt> >& oChannelIn,
            casacore::Vector<casacore::Double>& oFreqOut ) const;
        casacore::Bool spwInfoCheck( const casacore::Vector<casacore::uInt>& oSPWIn,
            const casacore::Vector<casacore::Vector<casacore::uInt> >& oChannelIn, casacore::Vector<casacore::uInt>& oSPWOut,
            casacore::Vector<casacore::Vector<casacore::uInt> >& oChannelOut ) const;
    };

    // INPUT nested class (allowed T: CalStats::NONE, CalStatsFitter::FIT, or
    // CalStatsHist::HIST), used to hold the vector output of stats<T>()
    class INPUT {
      public:
        casacore::Vector<casacore::uInt> oField;
        casacore::Vector<casacore::uInt> oAntenna1;
        casacore::Vector<casacore::Int> oAntenna2;
        casacore::Double dStartTime;
        casacore::Double dStopTime;
        casacore::Vector<casacore::Double> oTime;
        casacore::Vector<casacore::uInt> oSPW;
        casacore::Vector<casacore::Vector<casacore::uInt> > oChannel;
        casacore::Vector<casacore::String> oFeed;
        CalStats::AXIS eAxisIterUserID;
        RAP eRAP;
        casacore::Bool bNorm;
        casacore::Bool bUnwrap;
        casacore::Double dJumpMax;
    };

    // OUTPUT nested class (allowed T: CalStats::NONE, CalStatsFitter::FIT, or
    // CalStatsHist::HIST), used to hold the vector output of stats<T>()
    template <typename T>
    class OUTPUT {
      public:
        casacore::uInt uiField;
        casacore::uInt uiAntenna1;
        casacore::Int iAntenna2;
        RAP eRAP;
        casacore::Bool bNorm;
        casacore::Bool bUnwrap;
        casacore::Double dJumpMax;
        casacore::Matrix<CalStats::OUT<T> > oOut;
    };

    // Generic constructor
    CalAnalysis( const casacore::String& oTableName );

    // Destructor
    virtual ~CalAnalysis( void );

    // Return the calibration table name and keyword private variables
    casacore::String calName( void ) const;
    casacore::String msName( void ) const;
    casacore::String visCal( void ) const;
    casacore::String parType( void ) const;
    casacore::String polBasis( void ) const;

    // Get the fields, antennas, times, and feeds
    // spectral windows
    casacore::Vector<casacore::uInt> field( void ) const;
    casacore::Vector<casacore::uInt> antenna( void ) const;
    casacore::Vector<casacore::uInt> antenna1( void ) const;
    casacore::Vector<casacore::Int> antenna2( void ) const;
    casacore::Vector<casacore::Double> time( void ) const;
    casacore::Vector<casacore::String> feed( void ) const;

    // Get the number of spectral windows, spectral windows, number of channels
    // for each spectral window, and frequencies for each spectral window
    casacore::uInt numspw( void ) const;
    casacore::Vector<casacore::uInt> spw( void ) const;
    casacore::Vector<casacore::uInt> numChannel( void ) const;
    casacore::Vector<casacore::Vector<casacore::Double> > freq( void ) const;

    // Calculate statistics for the specified fields, antennas, time range,
    // feeds, spectral windows, and channels (allowed T: CalStats::NONE gets
    // data without calculating statistics, CalStatsFitter::FIT calculates fits,
    // and CalStatsHist::HIST calculates histogram statistics).
    template <typename T>
    casacore::Vector<OUTPUT<T> > stats( const INPUT& oInput,
        const CalStats::ARG<T>& oArg );

    // casacore::Function to determine whether a value is present in an array
    template <typename T>
    static casacore::Bool exists( const T& tValue, const casacore::Vector<T>& oValue );

    // casacore::Function to determine the index of a value present in an array
    template <typename T>
    static casacore::Bool where( const T& tValue, const casacore::Vector<T>& oInput,
        casacore::uInt& uiIndex );

    // casacore::Function to calculate sorted unique values of a vector
    template <typename T>
    static void unique( const casacore::Vector<T>& oVector, casacore::Vector<T>& unq);

  private:
    
    // Get the calibration table name and set the private variable
    casacore::String oCalName;
    casacore::String calNameGet( const casacore::String& oTableName );
    void calNameSet( const casacore::String& oCalNameIn );

    // Get the associated casacore::MS name and set the private variable
    casacore::String oMSName;
    casacore::String msNameGet( const casacore::String& oTableName );
    void msNameSet( const casacore::String& oMSNameIn );

    // Get the visibility calibration type and set the private variable
    casacore::String oVisCal;
    casacore::String visCalGet( const casacore::String& oTableName );
    void visCalSet( const casacore::String& oVisCalIn );

    // Get the parameter column type and set the private variable
    casacore::String oParType;
    casacore::String parTypeGet( const casacore::String& oTableName );
    void parTypeSet( const casacore::String& oParTypeIn );

    // Get the polarization basis and set the private variable
    casacore::String oPolBasis;
    casacore::String polBasisGet( const casacore::String& oTableName );
    void polBasisSet( const casacore::String& oPolBasisIn );

    // Get the field numbers and set the private variables
    casacore::uInt uiNumField; casacore::Vector<casacore::uInt> oField;
    casacore::Vector<casacore::uInt> fieldGet( const casacore::String& oTableName );
    void fieldSet( const casacore::Vector<casacore::uInt>& oFieldIn );
    casacore::Bool fieldCheck( const casacore::Vector<casacore::uInt>& oFieldIn,
        casacore::Vector<casacore::uInt>& oFieldOut ) const;

    // Get the antenna numbers and set the private variables
    casacore::uInt uiNumAntenna; casacore::Vector<casacore::uInt> oAntenna;
    casacore::Vector<casacore::uInt> antennaGet( const casacore::String& oTableName );
    void antennaSet( const casacore::Vector<casacore::uInt>& oAntennaIn );

    // Get the antenna 1 numbers and set the private variables
    casacore::uInt uiNumAntenna1; casacore::Vector<casacore::uInt> oAntenna1;
    casacore::Vector<casacore::uInt> antenna1Get( const casacore::String& oTableName );
    void antenna1Set( const casacore::Vector<casacore::uInt>& oAntenna1In );
    casacore::Bool antenna1Check( const casacore::Vector<casacore::uInt>& oAntenna1In,
        casacore::Vector<casacore::uInt>& oAntenna1Out ) const;

    // Get the antenna 2 numbers and set the private variables
    casacore::uInt uiNumAntenna2; casacore::Vector<casacore::Int> oAntenna2;
    casacore::Vector<casacore::Int> antenna2Get( const casacore::String& oTableName );
    void antenna2Set( const casacore::Vector<casacore::Int>& oAntenna2In );
    casacore::Bool antenna2Check( const casacore::Vector<casacore::Int>& oAntenna2In,
        casacore::Vector<casacore::Int>& oAntenna2Out ) const;

    // Get the times and set the private variables
    casacore::uInt uiNumTime; casacore::Vector<casacore::Double> oTime;
    casacore::Vector<casacore::Double> timeGet( const casacore::String& oTableName );
    void timeSet( const casacore::Vector<casacore::Double>& oTimeIn );
    casacore::Bool timeCheck( const casacore::Double& dStartTimeIn, const casacore::Double& dStopTimeIn,
        casacore::Double& dStartTimeOut, casacore::Double& dStopTimeOut,
        casacore::Vector<casacore::Double>& oTimeOut ) const;

    // Get the feeds and set the private variables
    casacore::uInt uiNumFeed; casacore::Vector<casacore::String> oFeed;
    casacore::Vector<casacore::String> feedGet( const casacore::String& oTableName );
    void feedSet( const casacore::Vector<casacore::String>& oFeedIn );
    casacore::Bool feedCheck( const casacore::Vector<casacore::String>& oFeedIn,
        casacore::Vector<casacore::String>& oFeedOut ) const;

    // Get the spectral window information and set the private variables
    SPW_INFO oSPWInfo;
    SPW_INFO& spwInfoGet( const casacore::String& oTableName );
    void spwInfoSet( const SPW_INFO& oSPWInfoIn );

    // Check and possibly fix the inputs to the stats<T>() member function
    casacore::Bool statsCheckInput( const CalAnalysis::INPUT& oInputIn,
        CalAnalysis::INPUT& oInputOut );

    // Gets the data from the new format calibration table grouped according to
    // unique combinations of (field,antenna1,antenna2)
    casacore::Bool getGroup( const NewCalTable& oNCT, const CalAnalysis::INPUT& oInput,
        casacore::Vector<casacore::uInt>& oFieldGroup, casacore::Vector<casacore::uInt>& oAntenna1Group,
        casacore::Vector<casacore::Int>& oAntenna2Group, casacore::Vector<casacore::Vector<casacore::Double> >& oTimeUniqueGroup,
        casacore::Vector<casacore::Vector<casacore::Double> >& oFreqGroup,
        casacore::Vector<casacore::Cube<casacore::DComplex> >& oCParamGroup,
        casacore::Vector<casacore::Cube<casacore::Double> >& oFParamGroup,
        casacore::Vector<casacore::Cube<casacore::Double> >& oParamErrGroup,
        casacore::Vector<casacore::Cube<casacore::Bool> >& oFlagGroup ) const;

    // Select the rows based on the inputs
    casacore::Bool rowSelect( const NewCalTable& oNCT, const CalAnalysis::INPUT& oInput,
        casacore::Vector<casacore::uInt>& oRowSelect ) const;

    // Return rows and other quantities for each group
    casacore::Bool rowGroup( const NewCalTable& oNCT, const casacore::Vector<casacore::uInt>& oRowSelect,
        casacore::Vector<casacore::Vector<casacore::uInt> >& oRowGroup, casacore::Vector<casacore::uInt>& oFieldGroup,
        casacore::Vector<casacore::uInt>& oAntenna1Group, casacore::Vector<casacore::Int>& oAntenna2Group,
        casacore::Vector<casacore::Vector<casacore::uInt> >& oSPWGroup,
        casacore::Vector<casacore::Vector<casacore::uInt> >& oSPWUniqueGroup,
        casacore::Vector<casacore::Vector<casacore::Double> >& oTimeGroup,
        casacore::Vector<casacore::Vector<casacore::Double> >& oTimeUniqueGroup ) const;

    // casacore::Map the spectral windows and determine the start channels (for
    // concatenating frequencies from different spectral windows)
    casacore::Bool chanSPW( const casacore::Vector<casacore::uInt>& oSPW, const INPUT& oInput,
        casacore::Vector<casacore::uInt>& oSPWMap, casacore::Vector<casacore::uInt>& oChanStart ) const;

    // Get the frequencies for each goup
    casacore::Bool freqGroup( const INPUT& oInput,
        const casacore::Vector<casacore::Vector<casacore::uInt> >& oSPWUniqueGroup,
        casacore::Vector<casacore::Vector<casacore::Double> >& oFreqGroup ) const;

    // Get the data cubes for each group
    casacore::Bool cubeGroup( const NewCalTable& oNCT, const INPUT& oInput,
        const casacore::Vector<casacore::Vector<casacore::uInt> >& oRowGroup,
        const casacore::Vector<casacore::Vector<casacore::uInt> >& oSPWGroup,
        const casacore::Vector<casacore::Vector<casacore::uInt> >& oSPWUniqueGroup,
        const casacore::Vector<casacore::Vector<casacore::Double> >& oTimeGroup,
        const casacore::Vector<casacore::Vector<casacore::Double> >& oTimeUniqueGroup,
        const casacore::Vector<casacore::Vector<casacore::Double> >& oFreqGroup,
        casacore::Vector<casacore::Cube<casacore::DComplex> >& oCParamGroup,
        casacore::Vector<casacore::Cube<casacore::Double> >& oFParamGroup,
        casacore::Vector<casacore::Cube<casacore::Double> >& oParamErrGroup,
        casacore::Vector<casacore::Cube<casacore::Bool> >& oFlagGroup ) const;

  protected:

    // Unused constructors
    CalAnalysis( void );
    CalAnalysis( const CalAnalysis& oCalAnalysis );

    // Unused operator=() function
    CalAnalysis& operator=( const CalAnalysis& oCalAnalysis );

};

// -----------------------------------------------------------------------------
// End of CalAnalysis class definition
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Start of CalAnalysis template public member functions
// -----------------------------------------------------------------------------

/*

CalAnalysis::stats<T>

Description:
------------
This member function is the main user interface for calculating the statistics
for all unique groups of (field,antenna1,antenna2).  Allowed T:
CalStats::NONE() only returns the input data, CalStatsFitter::FIT() calculates
fit statistics, and CalStatsHist::HIST() calculates histogram statistics.

NB: The stats<T>() function calculates statistics (the type depends on T) and
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

Inputs:
-------
oInput  - This reference to an CalAnalysis::INPUT() instance contains all of the
          input parameters.
oArg<T> - This reference to a CalStats::ARG<T>() instance contains the extra
          input parameters.

Outputs:
--------
The reference to the casacore::Vector<CalAnalysis::OUTPUT<T> >() instance containing the
statistics, returned via the function value.

Modification history:
---------------------
2012 Jan 20 - Nick Elias, NRAO
              Initial version.
2012 Jan 25 - Nick Elias, NRAO
              Logging capability added.
2012 Feb 14 - Nick Elias, NRAO
              Updated this code to reflect changes in NewCalTabIter() (now
              ROCTIter()) and other classes.  Added a RAP enum as an input
              parameter.
2012 Apr 02 - Nick Elias, NRAO
              casacore::Input parameter dJumpMax added.
2012 Apr 25 - Nick Elias, NRAO
              The start and stop channel lists versus spectral window are
              replaced with a channel list versus spectral window.
2012 Apr 26 - Nick Elias, NRAO
              All input parameter instances grouped together as a INPUT()
              instance.
2012 Apr 27 - Nick Elias, NRAO
              casacore::Input parameters are now checked and fixed by the
              statsCheckInput() member function.
2012 May 03 - Nick Elias, NRAO
              Replaced the new format calibration table iterator with straight
              calls to the NewCalTable() class and column accessor classes.
              There is a lot of data reorganization so that they can be fed to
              CalStats<T>().
2012 May 06 - Nick Elias, NRAO
              The data reorganization has been moved to private member function
              getGroup() and is called from here.

*/

// -----------------------------------------------------------------------------

template <typename T>
casacore::Vector<CalAnalysis::OUTPUT<T> > CalAnalysis::stats(
    const CalAnalysis::INPUT& oInput, const CalStats::ARG<T>& oArg ) {

  // Initialize the output vector containing statistics for each field ID,
  // antenna 1, and antenna 2

  casacore::Vector<CalAnalysis::OUTPUT<T> > output;


  // Check and fix the inputs

  INPUT oInputNew;

  casacore::Bool bCheck = statsCheckInput( oInput, oInputNew );

  if ( !bCheck ) {
    casacore::LogIO log( casacore::LogOrigin( "CalAnalysis", "stats<T>()", WHERE ) );
    log << casacore::LogIO::WARN << "Invalid parameter(s)" << casacore::LogIO::POST;
    return output;
  }


  // Create an instance of the new format calibration table class
  // Use createCT to enforce backward compatibility
  NewCalTable oNCT =  NewCalTable::createCT( oCalName, casacore::Table::Old, casacore::Table::Memory );


  // Get the cubes for each group

  casacore::Vector<casacore::uInt> oFieldGroup;
  casacore::Vector<casacore::uInt> oAntenna1Group;
  casacore::Vector<casacore::Int> oAntenna2Group;

  casacore::Vector<casacore::Vector<casacore::Double> > oTimeUniqueGroup;
  casacore::Vector<casacore::Vector<casacore::Double> > oFreqGroup;

  casacore::Vector<casacore::Cube<casacore::DComplex> > oCParamGroup;
  casacore::Vector<casacore::Cube<casacore::Double> > oFParamGroup;
  casacore::Vector<casacore::Cube<casacore::Double> > oParamErrGroup;
  casacore::Vector<casacore::Cube<casacore::Bool> > oFlagGroup;

  casacore::Bool bGetGroup = getGroup( oNCT, oInputNew, oFieldGroup, oAntenna1Group,
      oAntenna2Group, oTimeUniqueGroup, oFreqGroup, oCParamGroup, oFParamGroup,
      oParamErrGroup, oFlagGroup );

  if ( !bGetGroup ) {
    casacore::LogIO log( casacore::LogOrigin( "CalAnalysis", "stats<T>()", WHERE ) );
    log << casacore::LogIO::WARN << "Cannot parse group(s)" << casacore::LogIO::POST;
    return output;
  }


  // Resize the output vector (each group corresponds to an element of
  // casacore::Vector<OUTPUT<T> >())

  casacore::uInt uiNumGroup = oFieldGroup.nelements();

  output.resize( uiNumGroup, false );


  // Send each group to CalStats<T>() and perform the desired operation

  for ( casacore::uInt g=0; g<uiNumGroup; g++ ) {

    output[g].uiField = oFieldGroup[g];
    output[g].uiAntenna1 = oAntenna1Group[g];
    output[g].iAntenna2 = oAntenna2Group[g];
    output[g].eRAP = oInputNew.eRAP;
    output[g].bNorm = oInputNew.bNorm;
    output[g].bUnwrap = oInputNew.bUnwrap;
    output[g].dJumpMax = oInputNew.dJumpMax;

    CalStats* poCS = NULL;

    try {

      switch ((casacore::uInt) oInputNew.eRAP) {
        case (casacore::uInt) REAL:
          poCS = (CalStats*) new CalStatsReal( oFParamGroup[g],
              oParamErrGroup[g], oFlagGroup[g], oInputNew.oFeed, oFreqGroup[g],
              oTimeUniqueGroup[g], oInputNew.eAxisIterUserID );
          break;
	case (casacore::uInt) AMPLITUDE:
	  poCS = (CalStats*) new CalStatsAmp( oCParamGroup[g],
              oParamErrGroup[g], oFlagGroup[g], oInputNew.oFeed, oFreqGroup[g],
              oTimeUniqueGroup[g], oInputNew.eAxisIterUserID, oInputNew.bNorm );
          break;
	case (casacore::uInt) PHASE:
	  poCS = (CalStats*) new CalStatsPhase( oCParamGroup[g],
              oParamErrGroup[g], oFlagGroup[g], oInputNew.oFeed, oFreqGroup[g],
              oTimeUniqueGroup[g], oInputNew.eAxisIterUserID, oInputNew.bUnwrap,
              oInputNew.dJumpMax );
          break;
        default:
          throw( casacore::AipsError( "Invalid parameter (REAL, AMPLITUDE, or PHASE)" ) );
      }

      output[g].oOut = poCS->stats<T>( oArg );

      delete poCS;

    }

    catch ( casacore::AipsError oAE ) {
      casacore::LogIO log( casacore::LogOrigin( "CalAnalysis", "stats<T>()", WHERE ) );
      log << casacore::LogIO::WARN << oAE.getMesg()
          << ", iteration (field,antenna1,antenna2) = (" << oFieldGroup[g]
          << "," << oAntenna1Group[g] << "," << oAntenna2Group[g]
          << "), continuing ..." << casacore::LogIO::POST;
      output[g].oOut = CalStats::OUT<T>();
    }

  }


  // Return the reference to the casacore::Vector<CalAnalysis::OUTPUT<T> > instance

  return output;

}

// -----------------------------------------------------------------------------
// End of CalAnalysis template public member functions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Start of CalAnalysis template static public member functions
// -----------------------------------------------------------------------------

/*

CalAnalysis::exists<T>

Description:
------------
This member function determines whether a value appears in a vector.

Inputs:
-------
tValue - This reference to a T variable contains the desired value.
oValue - This reference to a casacore::Vector<T> instance contains the values.

Outputs:
--------
The reference to the existance casacore::Bool variable, returned via the function value.

Modification history:
---------------------
2012 Jan 20 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

template <typename T>
casacore::Bool CalAnalysis::exists( const T& tValue, const casacore::Vector<T>& oValue ) {

  // Determine whether the value is present in an array and return the boolean

  casacore::Bool value = false;

  for ( casacore::uInt v=0; v<oValue.nelements(); v++ ) {
    if ( tValue == oValue[v] ) value = true;
  }

  return value;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::where<T>

Description:
------------
This member function returns the index of a search value in a vector.

Inputs:
-------
tValue - The T instance containing the value for which to search.
oInput - The casacore::Vector<T> instance containing the values to be searched.

Outputs:
--------
uiIndex - The casacore::uInt instance containing the index of the search T instance in the
          searched casacore::Vector<T> instance.
The reference to the casacore::Bool containing the success boolean, returned via the
function value.

Modification history:
---------------------
2012 May 06 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

template <typename T>
casacore::Bool CalAnalysis::where( const T& tValue, const casacore::Vector<T>& oInput,
    casacore::uInt& uiIndex ) {

  // Find the index in the input vector

  for ( casacore::uInt i=0; i<oInput.nelements(); i++ ) {
    if ( oInput[i] == tValue ) {
      uiIndex = i;
      break;
    }
  }


  // If the value was found return true, otherwise return false

  casacore::Bool success = false;

  if ( uiIndex < oInput.nelements() ) {
    success = true;
  } else {
    success = false;
  }

  return success;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::unique<T>

Description:
------------
This member function produces a unique sorted vector from an input vector.

Inputs:
-------
oVector - This reference to a casacore::Vector<T> instance contains the values.

Outputs:
--------
unq - The unique sorted casacore::Vector<T> instance

Modification history:
---------------------
2012 Jan 20 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

template <typename T>
void CalAnalysis::unique( const casacore::Vector<T>& oVector, casacore::Vector<T>& unq ) {

  // Form the unique vector

  for ( casacore::uInt v1=0; v1<oVector.nelements(); v1++ ) {

    casacore::Bool bDupe = false;

    for ( casacore::uInt v2=0; v2<v1; v2++ ) {
      if ( oVector[v1] == oVector[v2] ) {
        bDupe = true;
	break;
      }
    }

    if ( !bDupe ) {
      unq.resize( unq.nelements()+1, true );
      unq.operator[](unq.nelements()-1) = oVector[v1];
    }

  }


  // casacore::Sort the unique vector

  casacore::Sort::Order eOrder = casacore::Sort::Ascending;
  casacore::Int iOptions = casacore::Sort::QuickSort;

  casacore::GenSort<T>::sort( unq, eOrder, (int) iOptions );
}

// -----------------------------------------------------------------------------
// End of CalAnalysis template static public member functions
// -----------------------------------------------------------------------------

};

// -----------------------------------------------------------------------------
// End of casa namespace
// -----------------------------------------------------------------------------

#endif

// -----------------------------------------------------------------------------
// End of define macro to prevent multiple loading
// -----------------------------------------------------------------------------
