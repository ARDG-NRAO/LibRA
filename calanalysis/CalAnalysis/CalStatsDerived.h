
// -----------------------------------------------------------------------------

/*

CalStatsDerived.h

Description:
------------
This header file contains definitions for the classes derived from CalStats.

Classes:
--------
CalStatsReal  - This class feeds real data to the CalStats base class.
CalStatsAmp   - This class converts complex data to amplitudes and initializes
                the CalStats base class.
CalStatsPhase - This class converts complex data to phases and initializes the
                CalStats base class.

Inhertited classes:
-------------------
CalStats - This class calculates statistics on CASA caltables.

Modification history:
---------------------
2011 Nov 15 - Nick Elias, NRAO
              Initial version.
2012 Jan 25 - Nick Elias, NRAO
              Logging capability added.  Error checking added.

*/

// -----------------------------------------------------------------------------
// Start of define macro to prevent multiple loading
// -----------------------------------------------------------------------------

#ifndef CAL_STATS_DERIVED_H
#define CAL_STATS_DERIVED_H

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#define _USE_MATH_DEFINES
#include <cmath>

#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/MaskedArray.h>

#include <calanalysis/CalAnalysis/CalStats.h>

// -----------------------------------------------------------------------------
// Start of casa namespace definitions
// -----------------------------------------------------------------------------

namespace casa {

// -----------------------------------------------------------------------------
// Start of CalStatsReal class definition
// -----------------------------------------------------------------------------

/*

CalStatsReal

Description:
------------
This class feeds real data to the CalStats base class.

Class public member functions:
------------------------------
CalStatsReal  - This class feeds real data to the CalStats base class.  It is
                primarily used for initial testing.
~CalStatsReal - This destructor deallocates the internal memory of an instance.

Modification history:
---------------------
2011 Dec 11 - Nick Elias, NRAO
              Initial version.  The public member functions are CalStatsReal()
              (generic) and ~CalStatsReal().

*/

// -----------------------------------------------------------------------------

class CalStatsReal : public CalStats {

  public:

    // Generic constructor
    CalStatsReal( const casacore::Cube<casacore::Double>& oValue, const casacore::Cube<casacore::Double>& oValueErr,
        const casacore::Cube<casacore::Bool>& oFlag, const casacore::Vector<casacore::String>& oFeed,
        const casacore::Vector<casacore::Double>& oFrequency, const casacore::Vector<casacore::Double>& oTime,
        const CalStats::AXIS& eAxisIterUserID );

    // Destructor
    ~CalStatsReal( void );

};

// -----------------------------------------------------------------------------
// End of CalStatsReal class definition
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Start of CalStatsAmp class definition
// -----------------------------------------------------------------------------

/*

CalStatsAmp

Description:
------------
This class converts complex data to amplitudes and initializes the CalStats base
class.

Class public member functions:
------------------------------
CalStatsAmp  - This generic constructor converts complex data to amplitudes and
               initializes the CalStats base class.  It is primarily used for
               initial testing.
~CalStatsAmp - This destructor deallocates the internal memory of an instance.

Class public static member functions:
-------------------------------------
norm - This member function normalizes the amplitudes and their errors.

Modification history:
---------------------
2011 Nov 15 - Nick Elias, NRAO
              Initial version.  The public member functions are CalStatsAmp()
              (generic) and ~CalStatsAmp().  The static member function is
              norm().

*/

// -----------------------------------------------------------------------------

class CalStatsAmp : public CalStats {

  public:

    // Generic constructor
    CalStatsAmp( const casacore::Cube<casacore::DComplex>& oValue, const casacore::Cube<casacore::Double>& oValueErr,
        const casacore::Cube<casacore::Bool>& oFlag, const casacore::Vector<casacore::String>& oFeed,
        const casacore::Vector<casacore::Double>& oFrequency, const casacore::Vector<casacore::Double>& oTime,
        const CalStats::AXIS& eAxisIterUserID, const casacore::Bool& bNorm );

    // Destructor
    ~CalStatsAmp( void );

    // Normalize member function
    static void norm( casacore::Vector<casacore::Double>& oAmp, casacore::Vector<casacore::Double>& oAmpErr,
        casacore::Vector<casacore::Bool>& oFlag );

};

// -----------------------------------------------------------------------------
// End of CalStatsAmp class definition
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Start of CalStatsPhase class definition
// -----------------------------------------------------------------------------

/*

CalStatsPhase

Description:
------------
This class converts complex data to phases and initializes the CalStats base
class.

Inhertited classes:
-------------------
CalStats - This class calculates statistics of new CASA caltables.

Class public member functions:
------------------------------
CalStatsPhase  - This generic constructor converts complex data to amplitudes
                 and initializes the CalStats base class.
~CalStatsPhase - This destructor deallocates the internal memory of an instance.

CalStatsPhase public static member functions:
---------------------------------------------
unwrapGD     - This member function unwraps the phases along the frequency axis
               with respect to the group delay.
unwrapSimple - This member function performs a simple unwrapping procedure for
               both frequency and temporal abscissae.

CalStatsPhase private static member functions:
----------------------------------------------
fringePacket2 - This member function forms the squared-amplitude fringe packet.

CalStatsPhase templated private static member functions:
--------------------------------------------------------
maxLocation - This member function finds the abscissa corresponding to the peak
              value of an ordinate vector.

Modification history:
---------------------
2011 Nov 15 - Nick Elias, NRAO
              Initial version created with public member functions are
              CalStatsPhase() and ~CalStatsPhase(); and public static member
              function is unwrap().
2012 Mar 27 - Nick Elias, NRAO
              Private static member functions fringePacket2() and maxLocation()
              added. Private static member variables NUM_ITER_UNWRAP and
              NEW_RANGE_FACTOR added.
2012 Mar 30 - Nick Elias, NRAO
              Public static member function unwrap() renamed to unwrapGD().
              Public static member function unwrapSimple() added.

*/

// -----------------------------------------------------------------------------

class CalStatsPhase : public CalStats {

  public:

    // Generic constructor
    CalStatsPhase( const casacore::Cube<casacore::DComplex>& oValue, const casacore::Cube<casacore::Double>& oValueErr,
        const casacore::Cube<casacore::Bool>& oFlag, const casacore::Vector<casacore::String>& oFeed,
        const casacore::Vector<casacore::Double>& oFrequency, const casacore::Vector<casacore::Double>& oTime,
        const CalStats::AXIS& eAxisIterUserID, const casacore::Bool& bUnwrap,
        const casacore::Double& dJumpMax );

    // Destructor
    ~CalStatsPhase( void );

    // Group-delay unwrapping member function
    static void unwrapGD( casacore::Vector<casacore::Double>& oPhase,
        const casacore::Vector<casacore::Double>& oFrequency, const casacore::Vector<casacore::Bool>& oFlag );

    // Simple phase unwrapping member function
    static void unwrapSimple( casacore::Vector<casacore::Double>& oPhase, const casacore::Double& dJumpMax,
        const casacore::Vector<casacore::Bool>& oFlag );

  private:

    // Form the squared-amplitude fringe packet
    static casacore::Vector<casacore::Double>& fringePacket2( const casacore::Vector<casacore::Double>& oPhase,
        const casacore::Vector<casacore::Double>& oFrequency, const casacore::Vector<casacore::Double>& oTime );

    // Find the abscissa corresponding to the peak value of an ordinate vector
    template <typename T> static casacore::Double& maxLocation( const casacore::Vector<T>& oAbs,
        const casacore::Vector<T>& oValue );

    // The maximum number of iterations for unwrapping
    static const casacore::uInt NUM_ITER_UNWRAP;

    // The new time range per unwrapping iteration is this factor times the
    // present time increment
    static const casacore::Double NEW_RANGE_FACTOR;

};

// -----------------------------------------------------------------------------
// End of CalStatsPhase class definition
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Start of CalStatsDerived::maxLocation<T> template private statistics member
// function
// -----------------------------------------------------------------------------

/*

CalStatsPhase::maxLocation<T>

Description:
------------
This member function finds the abscissa corresponding to the peak value of an
ordinate vector.

Inputs:
-------
oAbs   - This casacore::Vector<T>() instance contains the abscissae.
oValue - This casacore::Vector<T>() instance contains the ordinates.

Outputs:
--------
The reference to the casacore::Double variable containing the abscissa corresponding to
the peak value of the ordinate vector, returned via the function value.

Modification history:
---------------------
2012 Mar 27 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

template <typename T>
casacore::Double& CalStatsPhase::maxLocation( const casacore::Vector<T>& oAbs,
    const casacore::Vector<T>& oValue ) {

  // Call the minMax() function and return the position of the maximum value

  casacore::Double dValMin = 0.0;
  casacore::Double dValMax = 0.0;

  casacore::IPosition oPosMin( 1, 0 );
  casacore::IPosition oPosMax( 1, 0 );

  minMax( dValMin, dValMax, oPosMin, oPosMax, oValue );

  casacore::Double* pdAbsMax = new casacore::Double;
  *pdAbsMax = oAbs(oPosMax);

  return( *pdAbsMax );

}

// -----------------------------------------------------------------------------
// End of CalStatsDerived::maxLocation<T> template private statistics member
// function
// -----------------------------------------------------------------------------

};

// -----------------------------------------------------------------------------
// End of casa namespace
// -----------------------------------------------------------------------------

#endif

// -----------------------------------------------------------------------------
// End of define macro to prevent multiple loading
// -----------------------------------------------------------------------------
