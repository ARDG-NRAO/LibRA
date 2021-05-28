
// -----------------------------------------------------------------------------

/*

CalStatsDATA.cc

Description:
------------
This file contains member functions for the nested CalStats::DATA class.

Classes:
--------
CalStats::DATA - This nested class contains the data for the CalStats class.

Modification history:
---------------------
2011 Dec 23 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <calanalysis/CalAnalysis/CalStats.h>

// -----------------------------------------------------------------------------
// Start of casa namespace
// -----------------------------------------------------------------------------

using namespace casacore;
namespace casa {

// -----------------------------------------------------------------------------
// Start of CalStats::DATA nested class
// -----------------------------------------------------------------------------

/*

CalStats::DATA

Description:
------------
This nested class contains the data for the CalStats class.

Class public member functions:
------------------------------
DATA      - This constructor is the default that initializes variables.
DATA      - This construtor copies the input instance to the present instance.
~DATA     - This destructor destroys the instance.
operator= - This function sets one instance to another.

Modification history:
---------------------
2011 Dec 23 - Nick Elias, NRAO
              Initial version containing DATA() (default), DATA() (copy),
              ~DATA(), and operator=().

*/

// -----------------------------------------------------------------------------
// Start of CalStats::DATA public member functions
// -----------------------------------------------------------------------------

/*

CalStats::DATA::DATA (default)

Description:
------------
This constructor is the default that initializes variables.

Inputs:
-------
None.

Outputs:
--------
None.

Modification history:
---------------------
2011 Dec 23 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

CalStats::DATA::DATA( void ) {

  // Initialize the public variables and return

  oAbs = Vector<Double>();
  oValue = Vector<Double>();
  oValueErr = Vector<Double>();
  oFlag = Vector<Bool>();

  return;

}

// -----------------------------------------------------------------------------

/*

CalStats::DATA::DATA (copy)

Description:
------------
This construtor copies the input instance to the present instance.

Inputs:
-------
oDataIn - This reference to a CalStats::DATA instance contains the data
          information.

Outputs:
--------
None.

Modification history:
---------------------
2011 Dec 23 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

CalStats::DATA::DATA( const CalStats::DATA& oDataIn ) {

  // Copy the public variables from the input instance to this instance and
  // return

  oAbs = Vector<Double>( oDataIn.oAbs.copy() );
  oValue = Vector<Double>( oDataIn.oValue.copy() );
  oValueErr = Vector<Double>( oDataIn.oValueErr.copy() );
  oFlag = Vector<Bool>( oDataIn.oFlag.copy() );

  return;

}

// -----------------------------------------------------------------------------

/*

CalStats::DATA::~DATA

Description:
------------
This destructor destroys the instance.

Inputs:
-------
None.

Outputs:
--------
None.

Modification history:
---------------------
2011 Dec 23 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

CalStats::DATA::~DATA( void ) {}

// -----------------------------------------------------------------------------

/*

CalStats::DATA::operator=

Description:
------------
This function sets one instance to another.

Inputs:
-------
oDataIn - This reference to a CalStats::DATA instance contains the data
          information.

Outputs:
--------
None.

Modification history:
---------------------
2011 Dec 23 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

CalStats::DATA& CalStats::DATA::operator=( const CalStats::DATA& oDataIn ) {

  // If this instance is not the same as the input instance, copy the public
  // variables to this one and return

  if ( this != &oDataIn ) {
    oAbs = Vector<Double>( oDataIn.oAbs.copy() );
    oValue = Vector<Double>( oDataIn.oValue.copy() );
    oValueErr = Vector<Double>( oDataIn.oValueErr.copy() );
    oFlag = Vector<Bool>( oDataIn.oFlag.copy() );
  }

  return( *this );

}

// -----------------------------------------------------------------------------
// End of CalStats::DATA public member functions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// End of CalStats::DATA nested class
// -----------------------------------------------------------------------------

};

// -----------------------------------------------------------------------------
// End of casa namespace
// -----------------------------------------------------------------------------
