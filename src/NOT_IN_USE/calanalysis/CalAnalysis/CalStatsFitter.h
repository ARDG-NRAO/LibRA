
// -----------------------------------------------------------------------------

/*

CalStatsFitter.h

Description:
------------
This header file contains definitions for the CalStatsFitter class.

Classes:
--------
CalStatsFitter - This class contains the fit statistics static enums and
functions.

Modification history:
---------------------
2011 Dec 08 - Nick Elias, NRAO
              Initial version created.
2012 Jan 25 - Nick Elias, NRAO
              Error checking added.

*/

// -----------------------------------------------------------------------------
// Start of define macro to prevent multiple loading
// -----------------------------------------------------------------------------

#ifndef CAL_STATS_FITTER_H
#define CAL_STATS_FITTER_H

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <casacore/casa/aips.h>

#include <casacore/casa/Exceptions/Error.h>

#include <casacore/casa/Arrays/Vector.h>

#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/ArrayLogical.h>

#include <casacore/casa/Arrays/MaskedArray.h>

#include <casacore/scimath/Functionals/Polynomial.h>
#include <casacore/scimath/Mathematics/AutoDiff.h>
#include <casacore/scimath/Fitting/LinearFitSVD.h>

// -----------------------------------------------------------------------------
// Start of casa namespace
// -----------------------------------------------------------------------------

namespace casa {

// -----------------------------------------------------------------------------
// Start of CalStatsFitter class definition
// -----------------------------------------------------------------------------

/*

CalStatsFitter

Description:
------------
This class contains the fit statistics static enums and functions.

NB: At present, this class acts as a namespace for static functions.

Classes:
--------
CalStatsFitter - This class contains the fit statistics enums and functions.

Class public static member functions:
-------------------------------------
fit        - This member function is the fitting interface of this class.
orderName  - This function returns the string corresponding to the
             CalStatsFitter::ORDER enum.
typeName   - This function returns the string corresponding to the
             CalStatsFitter::TYPE enum.
weightName - This function returns the string corresponding to the
             CalStatsFitter::WEIGHT enum.

Class private static member functions (least-squares fitting):
--------------------------------------------------------------
lsqFit - This member function calculates polynomial least-squares fits.

Class private static member functions (robust fitting):
-------------------------------------------------------
robustFit - This member function calculates polynomial robust fits.
slope     - This member function calculates the robust slope.
brackFunc - This member function is root-finding bracketing function used to
            determine the slope.
signum    - This member function calculates the signum function (scalar).
signum    - This member function calculates the signum function (vector).
theil     - This member function estimates the slope and slope error using
            Theil's method.

Modification history:
---------------------
2011 Dec 08 - Nick Elias, NRAO
              Initial version created with enums ORDER, TYPE, and WEIGHT; public
              static member functions init() and fit(); and private static
              member functions lsqFit(), robustFit(), numDataWeight(), slope(),
              brackFunc(), signum() (scalar), and signum() (vector).
2011 Dec 11 - Nick Elias, NRAO
              Added value, value error, and model vectors to the FIT structure.
              Added dealloc() (pointer) and dealloc() (reference) public static
              member functions.
2011 Dec 21 - Nick Elias, NRAO
              Public static member functions init() and dealloc() removed
              because all of their duties are subsumed by the nested class
              FIT (it used used to be a structure).
2012 Jan 24 - Nick Elias, NRAO
              Private static member function theil() added.  Private static
              member function numDataWeight() removed because initial robust
              estimates of fit parameters (before final "trimmed" least squares)
              no longer employ weighting.
2012 Mar 06 - Nick Elias, NRAO
              Static public member functions orderName(), typeName(), and
              weightName() added.
2012 Mar 15 - Nick Elias, NRAO
              Public members dResVar and dResMean added to the nested FIT class.

*/

// -----------------------------------------------------------------------------

class CalStatsFitter {

  public:

    // Order enums
    typedef enum ORDER {
      ORDER_INIT=-1, AVERAGE=0, LINEAR, QUADRATIC
    } ORDER;

    // Type enums
    typedef enum TYPE {
      TYPE_INIT=-1, LSQ=0, ROBUST
    } TYPE;

    // Weight enums
    typedef enum WEIGHT {
      WEIGHT_INIT=-1, NO=false, YES=true
    } WEIGHT;

    // FIT nested class
    class FIT {
      public:
        ORDER eOrder;           // Fit order
        TYPE eType;             // Fit type
        WEIGHT eWeight;         // Fit weight
        casacore::Bool bValid;            // Fit validity boolean
        casacore::Vector<casacore::Double> oPars;   // Fit parameters
        casacore::Matrix<casacore::Double> oCovars; // Fit parameter covariances
        casacore::Vector<casacore::Double> oModel;  // Fit model
        casacore::Vector<casacore::Double> oRes;    // Fit residuals
        casacore::Double dResVar;         // Fit variance of residuals
        casacore::Double dResMean;        // Fit mean of residuals
        casacore::Double dRedChi2;        // Fit reduced chi^2
        FIT( void );
        FIT( const FIT& oFit );
        ~FIT( void );
        FIT& operator=( const FIT& oFit );
    };

    // User interface to all fitting capabilities
    static FIT fit( const casacore::Vector<casacore::Double>& oAbs, const casacore::Vector<casacore::Double>& oValue,
        const casacore::Vector<casacore::Double>& oValueErr, casacore::Vector<casacore::Bool>& oFlag,
        const ORDER& eOrder, const TYPE& eType, const WEIGHT& eWeight );

    // The enum names
    static casacore::String orderName( const ORDER& eOrder );
    static casacore::String typeName( const TYPE& eOrder );
    static casacore::String weightName( const WEIGHT& eOrder );

  private:

    // Least-squares fitting function
    static FIT lsqFit( const casacore::Vector<casacore::Double>& oAbs,
        const casacore::Vector<casacore::Double>& oValue, const casacore::Vector<casacore::Double>& oValueErr,
        casacore::Vector<casacore::Bool>& oFlag, const ORDER& eOrder, const WEIGHT& eWeight );

    // Robust fitting function
    static FIT robustFit( const casacore::Vector<casacore::Double>& oAbs,
        const casacore::Vector<casacore::Double>& oValue, const casacore::Vector<casacore::Double>& oValueErr,
        casacore::Vector<casacore::Bool>& oFlag, const ORDER& eOrder, const WEIGHT& eWeight,
        const casacore::Double& dTrim );

    // Functions required to calculate a robust linear fit
    static casacore::Double& slope( const casacore::Vector<casacore::Double>& oAbs,
        const casacore::Vector<casacore::Double>& oValue, const casacore::Double& dSlope,
        const casacore::Double& dSlopeErr, const casacore::Double& dFudge, const casacore::uInt& uiNumSlope,
        const casacore::uInt& uiNumIter );
    static casacore::Double& brackFunc( const casacore::Vector<casacore::Double>& oAbs,
        const casacore::Vector<casacore::Double>& oValue, const casacore::Double& dSlope );

    // Signum functions (also required to calculate a robust linear fit)
    static casacore::Double& signum( const casacore::Double& dValue );
    static casacore::Vector<casacore::Double>& signum( const casacore::Vector<casacore::Double>& oValue );

    // casacore::Function for calculating slope estimate using Theil's method
    static void theil( const casacore::Vector<casacore::Double>& oAbs, const casacore::Vector<casacore::Double>& oValue,
        casacore::Double& dSlope, casacore::Double& dSlopeErr );

};

// -----------------------------------------------------------------------------
// End of CalStatsFitter class definition
// -----------------------------------------------------------------------------

};

// -----------------------------------------------------------------------------
// End of casa namespace
// -----------------------------------------------------------------------------

#endif

// -----------------------------------------------------------------------------
// End of define macro to prevent multiple loading
// -----------------------------------------------------------------------------
