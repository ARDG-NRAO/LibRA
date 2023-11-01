
// -----------------------------------------------------------------------------

/*

CalAnalysisSPW_INFO.cc

Description:
------------
This file contains member functions for the nested CalAnalysis::SPW_INFO class.

Classes:
--------
CalAnalysis::SPW_INFO - This nested class contains the spectral window
information for the CalAnalysis class.

Modification history:
---------------------
2012 Apr 27 - Nick Elias, NRAO
              Initial version.

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
// Start of CalAnalysis::SPW_INFO nested class
// -----------------------------------------------------------------------------

/*

CalAnalysis::SPW_INFO

Description:
------------
This nested class contains the spectral window information for the CalAnalysis
class.

Class public member functions:
------------------------------
SPW_INFO     - This member function constructs an instance of this class using
               the new calibration table name as an input.
SPW_INFO     - This member function constructs an instance of this class from
               another instance.
SPW_INFO     - This member function constructs a default instance of this class.
~SPW_INFO    - This member function destructs an instance of this class.
operator=    - This member function copies an instance of this class to this
               instance.
freq         - This member function creates a frequency vector based on
               specified spectral window and channel numbers.
spwInfoCheck - This member function checks and eliminates duplicate spectral
               window and channel numbers.

Modification history:
---------------------
2012 Apr 27 - Nick Elias, NRAO
              Initial version containing public member functions SPW_INFO()
              (standard), SPW_INFO() (copy), SPW_INFO() (default), ~SPW_INFO(),
              operator=(), freq(), and spwInfoCheck().

*/

// -----------------------------------------------------------------------------
// Start of CalAnalysis::SPW_INFO public member functions
// -----------------------------------------------------------------------------

/*

CalAnalysis::SPW_INFO::SPW_INFO (standard)

Description:
------------
This member function constructs an instance of this class using the new
calibration table name as an input.

Inputs:
-------
oTableName - This reference to a String instance contains the name of the new
             calibration table name.

Outputs:
--------
None.

Modification history:
---------------------
2012 Apr 27 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

CalAnalysis::SPW_INFO::SPW_INFO( const String& oTableName ) {

  // Get the NAME column from the spectral window subtable

  String oSPWName( oTableName + "/SPECTRAL_WINDOW" );

  CTSpectralWindow oCalSPW( oSPWName, Table::Old );
  ArrayColumn<Double> oFreqACD( MSSpWindowColumns(oCalSPW).chanFreq() );


  // Initialize this instance

  bValid = true;

  uiNumSPW = oFreqACD.nrow();

  oSPW = Vector<uInt>( uiNumSPW );
  indgen<uInt>( oSPW, 0 );

  oNumChannel = Vector<uInt>( uiNumSPW );
  for ( uInt s=0; s<uiNumSPW; s++ ) oNumChannel[s] = oFreqACD( s ).nelements();

  oFrequency = Vector<Vector<Double> >( uiNumSPW );
  for ( uInt s=0; s<uiNumSPW; s++ ) oFrequency[s] = oFreqACD( s );


  // Return

  return;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::SPW_INFO::SPW_INFO (copy)

Description:
------------
This member function constructs an instance of this class from another instance.

Inputs:
-------
oSPWInfoIn - This reference to a SPW_INFO& instance contains the spectral
             window information.

Outputs:
--------
None.

Modification history:
---------------------
2012 Apr 27 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

CalAnalysis::SPW_INFO::SPW_INFO( const SPW_INFO& oSPWInfoIn ) {

  // Initialize this instance from the input instance

  bValid = oSPWInfoIn.bValid;

  uiNumSPW = oSPWInfoIn.uiNumSPW;
  oSPW = oSPWInfoIn.oSPW.copy();

  oNumChannel = oSPWInfoIn.oNumChannel.copy();

  oFrequency = Vector<Vector<Double> >( uiNumSPW );
  for ( uInt s=0; s<uiNumSPW; s++ ) {
    oFrequency[s] = oSPWInfoIn.oFrequency[s].copy();
  }


  // Return

  return;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::SPW_INFO::SPW_INFO (default)

Description:
------------
This member function constructs a default instance of this class.

Inputs:
-------
None.

Outputs:
--------
None.

Modification history:
---------------------
2012 Apr 27 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

CalAnalysis::SPW_INFO::SPW_INFO( void ) {

  // Initialize all instances to their default values

  bValid = false;

  uiNumSPW = 0;
  oSPW = Vector<uInt>();

  oNumChannel = Vector<uInt>();

  oFrequency = Vector<Vector<Double> >();


  // Return

  return;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::SPW_INFO::~SPW_INFO

Description:
------------
This member function destructs an instance of this class.

Inputs:
-------
None.

Outputs:
--------
None.

Modification history:
---------------------
2012 Apr 27 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

CalAnalysis::SPW_INFO::~SPW_INFO( void ) {}

// -----------------------------------------------------------------------------

/*

CalAnalysis::SPW_INFO::operator=

Description:
------------
This member function copies an instance of this class to this instance.

Inputs:
-------
oSPWInfoIn - This reference to a SPW_INFO& instance contains the spectral
             window information.

Outputs:
--------
None.

Modification history:
---------------------
2012 Apr 27 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

CalAnalysis::SPW_INFO& CalAnalysis::SPW_INFO::operator=(
    const CalAnalysis::SPW_INFO& oSPWInfoIn ) {

  // Copy the input instance to this instance

  if ( this != &oSPWInfoIn ) {

    bValid = oSPWInfoIn.bValid;

    uiNumSPW = oSPWInfoIn.uiNumSPW;

    oSPW.resize();
    oSPW = oSPWInfoIn.oSPW.copy();

    oNumChannel.resize();
    oNumChannel = oSPWInfoIn.oNumChannel.copy();

    oFrequency.resize();
    oFrequency = Vector<Vector<Double> >( uiNumSPW );
    for ( uInt s=0; s<uiNumSPW; s++ ) {
      oFrequency[s].resize();
      oFrequency[s] = oSPWInfoIn.oFrequency[s].copy();
    }

  }


  // Return the reference to this instance

  return( *this );

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::SPW_INFO::freq

Description:
------------
This member function creates a frequency vector based on specified spectral
window and channel numbers.

NB: Before using this function, the spectral window and channel numbers should
be checked by CalAnalysis::SPW_INFO::spwInfoCheck().

NB: This function now always returns true.  It could return false previously
because it did some checking.  I have not changed the output of this function
just in case I want to add checking again.

Inputs:
-------
oSPWIn     - This reference to a Vector<uInt> contains the spectral window
             numbers.
oChannelIn - This reference to a Vector<Vector<uInt> > contains the channel
             numbers for each spectral window number.

Outputs:
--------
oFreqOut - The reference to the Vector<Double> instance containing the
           frequencies.
The Bool variable containing the success boolean, returned via
the function value.

Modification history:
---------------------
2012 Apr 27 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

Bool CalAnalysis::SPW_INFO::freq( const Vector<uInt>& oSPWIn,
    const Vector<Vector<uInt> >& oChannelIn, Vector<Double>& oFreqOut ) const {

  // Declare the success boolean

  Bool success( false );


  // Check the inputs

  if ( oSPWIn.nelements() != oChannelIn.nelements() ) {
    success = false;
    return success;
  }


  // Initialize the output frequency vector

  oFreqOut.resize();

  uInt uiNumFreqOut = 0;


  // Load the output frequency vector with the frequencies specified by the
  // spectral window and channel numbers

  for ( uInt s=0; s<oSPWIn.nelements(); s++ ) {
    for ( uInt c=0; c<oChannelIn[s].nelements(); c++ ) {
      oFreqOut.resize( ++uiNumFreqOut, true );
      oFreqOut[uiNumFreqOut-1] = oFrequency[oSPWIn[s]][oChannelIn[s][c]];
    }
  }


  // Return true

  success = true;

  return success;

}

// -----------------------------------------------------------------------------

/*

CalAnalysis::SPW_INFO::spwInfoCheck

Description:
------------
This member function checks and eliminates duplicate spectral window and channel
numbers.

NB: The output values are compared against the internal spectral window and
channel vector instances.

Inputs:
-------
oSPWIn     - This reference to a Vector<uInt> contains the spectral window
             numbers.
oChannelIn - This reference to a Vector<Vector<uInt> > contains the channel
             numbers for each spectral window number.

Outputs:
--------
oSPWOut     - This reference to a Vector<uInt> contains the checked and unique
              spectral window numbers.
oChannelOut - This reference to a Vector<Vector<uInt> > contains the checked and
              unique channel numbers for each spectral window number.
The reference to the Bool variable containing the check boolean, returned via
the function value.

Modification history:
---------------------
2012 Apr 27 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

Bool CalAnalysis::SPW_INFO::spwInfoCheck( const Vector<uInt>& oSPWIn,
    const Vector<Vector<uInt> >& oChannelIn, Vector<uInt>& oSPWOut,
    Vector<Vector<uInt> >& oChannelOut ) const {

  // Declare the success boolean

  Bool success( false );


  // Does this instance contain valid spectral window and channel numbers?

  if ( !bValid ) {
    success = false;
    return success;
  }


  // Get the output sorted unique spectral window number vector

  oSPWOut.resize();
  unique<uInt>( oSPWIn, oSPWOut );

  uInt uiNumSPWOut = oSPWOut.nelements();


  // Check the output spectral window numbers

  if ( uiNumSPWOut == 0 ) {
    success = false;
    return success;
  }

  for ( uInt s=0; s<uiNumSPWOut; s++ ) {
    if ( !exists<uInt>( oSPWOut[s], oSPW ) ) {
      success = false;
      return success;
    }
  }


  // Initialize the output channel vector of vectors.  If the input spectral
  // window vector has duplicates, the channels corresponding to these
  // duplicates are concatenated.

  uInt uiNumSPWIn = oSPWIn.nelements();

  oChannelOut.resize( uiNumSPWOut, false );

  for ( uInt so=0; so<uiNumSPWOut; so++ ) {
    oChannelOut[so].resize();
    for ( uInt si=0; si<uiNumSPWIn; si++ ) {
      if ( oSPWIn[si] != oSPWOut[so] ) continue;
      uInt uiNumChannelOut = oChannelOut[so].nelements();
      uInt uiNumChannelIn = oChannelIn[si].nelements();
      for ( uInt c=0; c<uiNumChannelIn; c++ ) {
	oChannelOut[so].resize( ++uiNumChannelOut, true );
        oChannelOut[so][uiNumChannelOut-1] = oChannelIn[si][c];
      }
    }
  }


  // Get output sorted unique channels within each spectral window

  for ( uInt s=0; s<uiNumSPWOut; s++ ) {
    Vector<uInt> oChannelTemp;
    unique<uInt>(oChannelOut[s], oChannelTemp);
    oChannelOut[s].resize();
    oChannelOut[s] = oChannelTemp;
  }


  // Check the output channels for each spectral window

  for ( uInt s=0; s<uiNumSPWOut; s++ ) {
    uInt uiNumChannelOut = oChannelOut[s].nelements();
    for ( uInt c=0; c<uiNumChannelOut; c++ ) {
      if ( oChannelOut[s][c] >= oNumChannel[oSPWOut[s]] ) {
        success = false;
        return success;
      }
    }
  }


  // Return true

  success = true;

  return success;

}

// -----------------------------------------------------------------------------
// End of CalAnalysis::SPW_INFO public member functions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// End of CalAnalysis::SPW_INFO nested class
// -----------------------------------------------------------------------------

};

// -----------------------------------------------------------------------------
// End of casa namespace
// -----------------------------------------------------------------------------
