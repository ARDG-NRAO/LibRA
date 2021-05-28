
// -----------------------------------------------------------------------------

/*

CalStatsFIT.cc

Description:
------------
This file contains member functions for the nested CalStatsFitter::FIT class.

Classes:
--------
CalStatsFitter::FIT - This nested class contains the fit for the CalStatsFitter
                      class.

Modification history:
---------------------
2011 Dec 23 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <calanalysis/CalAnalysis/CalStatsFitter.h>

// -----------------------------------------------------------------------------
// Start of casa namespace
// -----------------------------------------------------------------------------

using namespace casacore;
namespace casa {

// -----------------------------------------------------------------------------
// Start of CalStatsFitter::FIT nested class
// -----------------------------------------------------------------------------

/*

CalStatsFitter::FIT

Description:
------------
This nested class contains the fit for the CalStatsFitter class.

Class public member functions:
------------------------------
FIT       - This constructor is the default that initializes variables.
FIT       - This construtor copies the input instance to the present instance.
~FIT      - This destructor destroys the instance.
operator= - This function sets one instance to another.

Modification history:
---------------------
2011 Dec 23 - Nick Elias, NRAO
              Initial version containing FIT() (default), FIT() (copy), ~FIT(),
              and operator=().

*/

// -----------------------------------------------------------------------------
// Start of CalStatsFitter::FIT public member functions
// -----------------------------------------------------------------------------

/*

CalStatsFitter::FIT::FIT (default)

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

CalStatsFitter::FIT::FIT( void ) {

  // Initialize the public variables and return

  eOrder = CalStatsFitter::ORDER_INIT;
  eType = CalStatsFitter::TYPE_INIT;
  eWeight = CalStatsFitter::WEIGHT_INIT;
  bValid = false;
  oPars = Vector<Double>();
  oCovars = Matrix<Double>();
  oModel = Vector<Double>();
  oRes = Vector<Double>();
  dResVar = 0.0;
  dResMean = 0.0;
  dRedChi2 = 0.0;

  return;

}

// -----------------------------------------------------------------------------

/*

CalStatsFitter::FIT::FIT (copy)

Description:
------------
This construtor copies the input instance to the present instance.

Inputs:
-------
oFit - This reference to a CalStatsFitter::FIT instance contains the fit
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

CalStatsFitter::FIT::FIT( const CalStatsFitter::FIT& oFit ) {

  // Copy the public variables from the input instance to this instance and
  // return

  eOrder = oFit.eOrder;
  eType = oFit.eType;
  eWeight = oFit.eWeight;
  bValid = oFit.bValid;
  oPars = Vector<Double>( oFit.oPars.copy() );
  oCovars = Matrix<Double>( oFit.oCovars.copy() );
  oModel = Vector<Double>( oFit.oModel.copy() );
  oRes = Vector<Double>( oFit.oRes.copy() );
  dResVar = oFit.dResVar;
  dResMean = oFit.dResMean;
  dRedChi2 = oFit.dRedChi2;

  return;

}

// -----------------------------------------------------------------------------

/*

CalStatsFitter::FIT::~FIT

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

CalStatsFitter::FIT::~FIT( void ) {}

// -----------------------------------------------------------------------------

/*

CalStatsFitter::FIT::operator=

Description:
------------
This function sets one instance to another.

Inputs:
-------
oFit - This reference to a CalStatsFitter::FIT instance contains the fit
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

CalStatsFitter::FIT& CalStatsFitter::FIT::operator=(
    const CalStatsFitter::FIT& oFit ) {

  // If this instance is not the same as the input instance, copy the public
  // variables to this one and return

  if ( this != &oFit ) {
    eOrder = oFit.eOrder;
    eType = oFit.eType;
    eWeight = oFit.eWeight;
    bValid = oFit.bValid;
    oPars = Vector<Double>( oFit.oPars.copy() );
    oCovars = Matrix<Double>( oFit.oCovars.copy() );
    oModel = Vector<Double>( oFit.oModel.copy() );
    oRes = Vector<Double>( oFit.oRes.copy() );
    dResVar = oFit.dResVar;
    dResMean = oFit.dResMean;
    dRedChi2 = oFit.dRedChi2;
  }
  
  return( *this );

}

// -----------------------------------------------------------------------------
// End of CalStatsFitter::FIT public member functions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// End of CalStatsFitter::FIT nested class
// -----------------------------------------------------------------------------

};

// -----------------------------------------------------------------------------
// End of casa namespace
// -----------------------------------------------------------------------------
