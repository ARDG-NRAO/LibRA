
// -----------------------------------------------------------------------------

/*

CalStatsFitter.cc

Description:
------------
This file contains member functions for the CalStatsFitter class.

Classes:
--------
CalStatsFitter - This class contains the fit statistics static enums and
functions.

Modification history:
---------------------
2011 Dec 08 - Nick Elias, NRAO
              Initial version.
2012 Jan 25 - Nick Elias, NRAO
              Error checking added.

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
// Start of CalStatsFitter class
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

*/

// -----------------------------------------------------------------------------
// Start of CalStatsFitter public static member functions
// -----------------------------------------------------------------------------

/*

CalStatsFitter::fit

Description:
------------
This member function is the fitting interface of this class.

NB: If there are no errors, the value error vector should contain all 1.0
elements.

NB: There is no robust quadratic fit available.

NB: The trim factor required for member function robustFit() is set to 5.0.  It
is not part of the argument list for this member function.

Inputs:
-------
oAbs      - This reference to a Vector<Double> instance contains the abscissae.
oValue    - This reference to a Vector<Double> instance contains the values.
oValueErr - This reference to a Vector<Double> instance contains the value
            errors.
oFlag     - This reference to a Vector<Bool> instance contains the flags.
eOrder    - This reference to a CalStatsFitter::ORDER enum contains the fit
            order (CalStatsFitter::AVERAGE = average fit, CalStatsFitter::LINEAR
            = linear fit, CalStatsFitter::QUADRATIC = quadratic fit).
eType     - This reference to a CalStatsFitter::TYPE enum contains the fit type
            (CalStatsFitter::LSQ = least squares,
            CalStatsFitter::ROBUST = robust).
eWeight   - This reference to a CalStatsFitter::WEIGHT enum contains the weight
            flag (CalStatsFitter::YES = apply weights, CalStatsFitter::NO = don't
            apply weights).

Outputs:
--------
oFlag - This reference to a Vector<Bool> instance contains the flags.
The reference to the FIT structure, returned via the function value.

Modification history:
---------------------
2011 Dec 08 - Nick Elias, NRAO
              Initial version.
2012 Jan 25 - Nick Elias, NRAO
              Error checking added.

*/

// -----------------------------------------------------------------------------

CalStatsFitter::FIT CalStatsFitter::fit( const Vector<Double>& oAbs,
    const Vector<Double>& oValue, const Vector<Double>& oValueErr,
    Vector<Bool>& oFlag, const ORDER& eOrder, const TYPE& eType,
    const WEIGHT& eWeight ) {

  // Calculate the desired fit and populate the pointer to the FIT structure

  CalStatsFitter::FIT fit;

  switch ((uInt) eType) {

    case (uInt) LSQ:
      try {
        fit = CalStatsFitter::FIT( lsqFit( oAbs, oValue, oValueErr, oFlag,
                                           eOrder, eWeight ) );
      }
      catch ( AipsError oAE ) {
        throw( oAE );
      }
      fit.eOrder = eOrder;
      fit.eType = eType;
      fit.eWeight = eWeight;
      break;

    case (uInt) ROBUST:
      switch ((uInt) eOrder) {
        case (uInt) AVERAGE:
        case (uInt) LINEAR:
          try {
            fit = CalStatsFitter::FIT( robustFit( oAbs, oValue, oValueErr, oFlag, eOrder,
                                                  eWeight, 5.0 ) );
          }
          catch ( AipsError oAE ) {
	    throw( oAE );
	  }
          fit.eOrder = eOrder;
          fit.eType = eType;
          fit.eWeight = eWeight;
	  break;
        case (uInt) QUADRATIC:
        default:
          throw( AipsError( "No quadratic robust fit available" ) );
          break;
      }
      break;

    default:
      throw( AipsError( "Invalid type of fit" ) );
      break;

  }


  // Return the FIT structure

  return fit;

}

// -----------------------------------------------------------------------------

/*

CalStats::orderName

Description:
------------
This function returns the string corresponding to the CalStatsFitter::ORDER
enum.

Inputs:
-------
eOrder - This reference to the CalStatsFitter::ORDER enum.

Outputs:
--------
The order string, returned via the function value.

Modification history:
---------------------
2012 Mar 06 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

String CalStatsFitter::orderName( const CalStatsFitter::ORDER& eOrder ) {

  // Return the string corresponding to the CalStatsFitter::ORDER enum

  String orderName;

  switch ((uInt) eOrder) {
    case (uInt) CalStatsFitter::AVERAGE:
      orderName = String( "AVERAGE" );
      break;
    case (uInt) CalStatsFitter::LINEAR:
      orderName = String( "LINEAR" );
      break;
    case (uInt) CalStatsFitter::QUADRATIC:
      orderName = String( "QUADRATIC" );
      break;
    default:
      throw( AipsError( "Invalid order" ) );
      break;
  }

  return orderName;

}

// -----------------------------------------------------------------------------

/*

CalStats::typeName

Description:
------------
This function returns the string corresponding to the CalStatsFitter::TYPE enum.

Inputs:
-------
eType - This reference to the CalStatsFitter::TYPE enum.

Outputs:
--------
The type string, returned via the function value.

Modification history:
---------------------
2012 Mar 06 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

String CalStatsFitter::typeName( const CalStatsFitter::TYPE& eType ) {

  // Return the string corresponding to the CalStatsFitter::TYPE enum

  String typeName;

  switch ((uInt) eType) {
    case (uInt) CalStatsFitter::LSQ:
      typeName = String( "LSQ" );
      break;
    case (uInt) CalStatsFitter::ROBUST:
      typeName = String( "ROBUST" );
      break;
    default:
      throw( AipsError( "Invalid type" ) );
      break;
  }

  return typeName;

}

// -----------------------------------------------------------------------------

/*

CalStats::weightName

Description:
------------
This function returns the string corresponding to the CalStatsFitter::WEIGHT
enum.

Inputs:
-------
eWeight - This reference to the CalStatsFitter::WEIGHT enum.

Outputs:
--------
The weight string, returned via the function value.

Modification history:
---------------------
2012 Mar 06 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

String CalStatsFitter::weightName( const CalStatsFitter::WEIGHT& eWeight ) {

  // Return the string corresponding to the CalStatsFitter::WEIGHT enum

  String weightName;

  switch ((uInt) eWeight) {
    case (uInt) CalStatsFitter::NO:
      weightName = String( "NO" );
      break;
    case (uInt) CalStatsFitter::YES:
      weightName = String( "YES" );
      break;
    default:
      throw( AipsError( "Invalid weight" ) );
      break;
  }

  return weightName;

}

// -----------------------------------------------------------------------------
// End of CalStatsFitter public static member functions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Start of CalStatsFitter private static member functions
// -----------------------------------------------------------------------------

/*

CalStatsFitter::lsqFit

Description:
------------
This member function calculates polynomial least-squares fits.

NB: The linear (coefficients times basis functions) SVD fit is performed by
class LinearFitSVD in LinearFitSVD.h.  This class also requires the Polynomial
class in Polynomial.h and the AutoDiff class in AutoDiff.h.

Scaling algorithm:
------------------
* The abscissae are scaled by the sum of the magnitude of their minimum value
  and the magnitude of their maximum value.
* The values are scaled by the sum of the magnitude of their minimum value and
  the magnitude of their maximum value.  The same scale factor is used to scale
  the value errors.
* After the fit, the parameters and covariances are rescaled back.
* NB: I could have used a scaling where the abscissae and values are scaled and
  shifted between 0 and 1, but reconsituting the fit parameters and covariances
  with this scheme is more difficult.  The present scheme minimizes round off
  and reconstituting the fit parameters and covariances is easier, so it will
  do.

Inputs:
-------
oAbs      - This reference to a Vector<Double> instance contains the abscissae.
oValue    - This reference to a Vector<Double> instance contains the values.
oValueErr - This reference to a Vector<Double> instance contains the value
            errors.
oFlag     - This reference to a Vector<Bool> instance contains the flags.
eOrder    - This reference to a CalStatsFitter::ORDER enum determines the fit
            order (CalStatsFitter::AVERAGE = average fit,
            CalStatsFitter::LINEAR = linear fit,
            CalStatsFitter::QUADRATIC = quadratic fit).
eWeight   - This reference to a CalStatsFitter::WEIGHT enum is the weight flag
            (CalStatsFitter::YES = apply weights, CalStatsFitter::NO = don't
            apply weights).

Outputs:
--------
oFlag - This reference to a Vector<Bool> instance contains the flags.
The reference to the FIT structure, returned via the function value.  If an
error occurrs the FIT structure will contain only the initial values and no
error message will be printed.

Modification history:
---------------------
2011 Dec 08 - Nick Elias, NRAO
              Initial version.
2011 Dec 21 - Nick Elias, NRAO
              Abscissa and value scaling performed before fitting to minimize
              round-off errors.
2012 Jan 23 - Nick Elias, NRAO
              Fixed bug for weighting (the CASA tool documentation is confusing
              with respect to how to input errors and weights).
2012 Jan 25 - Nick Elias, NRAO
              Error checking added.
2012 Mar 15 - Nick Elias, NRAO
              Added calculates for residual variance and mean.

*/

// -----------------------------------------------------------------------------

CalStatsFitter::FIT CalStatsFitter::lsqFit( const Vector<Double>& oAbs,
    const Vector<Double>& oValue, const Vector<Double>& oValueErr,
    Vector<Bool>& oFlag, const ORDER& eOrder, const WEIGHT& eWeight ) {

  // Get the length of the input arrays

  uInt uiNumData = oAbs.nelements();


  // Eliminate the flagged abscissae, value, and value errors

  MaskedArray<Double> oAbsM( oAbs, !oFlag );
  Vector<Double> oAbsC( oAbsM.getCompressedArray() );

  MaskedArray<Double> oValueM( oValue, !oFlag );
  Vector<Double> oValueC( oValueM.getCompressedArray() );

  MaskedArray<Double> oValueErrM( oValueErr, !oFlag );
  Vector<Double> oValueErrC( oValueErrM.getCompressedArray() );
  if ( !(Bool) eWeight ) oValueErrC = (Double) 1.0;


  // Check to see if there are a sufficient number of data.  The absolute
  // minimum number of data is allowed, which may or may not be a good idea
  // (TBD).

  uInt uiNumDataC = oAbsC.nelements();
  Int iDoF = ((Int) uiNumDataC) - ((Int) eOrder);

  if ( iDoF <= 0 ) {
    throw( AipsError( "Insufficient number of data for least-squares fit" ) );
  }


  // Set the scaling (to eliminate round off) and weighting

  Double dScaleX = abs( max(oAbsC) ) + abs( min(oAbsC) );
  oAbsC /= dScaleX;

  Double dScaleY = abs( max(oValueC) ) + abs( min(oValueC) );
  oValueC /= dScaleY;
  oValueErrC /= dScaleY;


  // Initialize the SVD linear fit object

  LinearFitSVD<Double> oFitter;

  oFitter.asSVD( true );


  // Initialize the basis functions and feed them to the SVD linear fit object

  Polynomial< AutoDiff<Double> > oBasisFuncs( (uInt) eOrder );
  for ( uInt o=0; o<=(uInt)eOrder; o++ ) oBasisFuncs[o] = 1.0;

  oFitter.setFunction( oBasisFuncs );


  // Calculate the fit 

  Vector<Double> oPars( (uInt) eOrder+1 );

  Bool bValid = oFitter.fit( oPars, oAbsC, oValueC, oValueErrC );


  // Initialize the CalStatsFitter::FIT structure

  CalStatsFitter::FIT fit;


  // Populate the FIT structure, including abscissa, parameter,
  // and covariance rescaling

  if ( bValid ) {

    fit.bValid = bValid;

    fit.oPars = Vector<Double>( oPars );
    for ( uInt o=0; o<=(uInt)eOrder; o++ ) {
      Double dScale = pow(dScaleX,(Int)o) / dScaleY;
      fit.oPars[o] /= dScale;
    }

    fit.oModel = Vector<Double>( uiNumData, (Double) 0.0 );
    for ( uInt o=0; o<=(uInt)eOrder; o++ ) {
      fit.oModel += fit.oPars[o] * pow(oAbs,(Int)o);
    }

    fit.oRes = Vector<Double>( oValue - fit.oModel );

    fit.dResMean = mean<Double>( fit.oRes );
    fit.dResVar = variance<Double>( fit.oRes, fit.dResMean );

    Double dMetric = oFitter.chiSquare() / ((Double) iDoF);

    fit.oCovars = Matrix<Double>( oFitter.compuCovariance() );
    for ( uInt o1=0; o1<=(uInt)eOrder; o1++ ) {
      for ( uInt o2=0; o2<=(uInt)eOrder; o2++ ) {
        Double dScale = pow(dScaleX,(Int)o1) * pow(dScaleX,(Int)o2);
	dScale /= pow(dScaleY,2);
        fit.oCovars(o1,o2) /= dScale;
      }
    }

    if ( (Bool) eWeight ) {
      fit.dRedChi2 = dMetric;
    } else {
      fit.oCovars *= dMetric;
      fit.dRedChi2 = 1.0;
    }

  } else {

    throw( AipsError( "Least-squares fit failed" ) );

  }


  // Return the FIT structure

  return fit;

}

// -----------------------------------------------------------------------------

/*

CalStatsFitter::robustFit

Description:
------------
This member function calculates polynomial robust fits.

NB: Unlike member function lsqFit(), this member function does not calculate
quadratic fits.

NB: All outliers will be trimmed.  This trimming will manifest itself in the
input/output abscissae, value, and value errors.

Algorithm:
----------
* Calculate the initial slope and slope error estimate using Theil's method (see
  thiel() member function).
* Using the initial estimate of the fit parameters (and new abscissae and value
  vectors if weighting is selected), calculate the robust fit parameters by
  minimizing the absolute deviation.  No weighting is ever used here.
  - If the order is zero, the median is calculated (in ArrayMath.h).  Forming
    the median is essentially a sorting problem.
  - If the order is one, the slope and intercept are calculated.  The algorithm
    may be found in "Numerical Recipes in C" (in the first edition, section 14.6
    on pages 562-563).  Forming the slope and intercept is essentially a root
    finding problem performed by the slope() member function.  The median (in
    ArrayMath.h) is used as well.  Some input parameters of the slope() member
    function are set by this member function and do not appear in its input
    parameter list.
      # The search range for the robust slope 10.0 times the standard deviation
        of the initial least-squares fit.
      # The number of test slopes within an iteration is set to 20.
      # The number of iterations is set to 30.
* The robust model and residuals are calculated.
* A new flag vector is formed.  false elements correspond to absolute residuals
  less than fTrim (an input parameter of this member function) times the mean
  absolute deviation (avdev() function in ArrayMath.h) of the residuals.  true
  elements, of course, correspond to the opposite case.
* Calculate a "trimmed" least-squares fit using the original absicssae, value,
  and value errors (but with the new flag vector) to get the final estimate of
  the fit parameters and their covariance matrix.

Inputs:
-------
oAbs      - This reference to a Vector<Double> instance contains the abscissae.
oValue    - This reference to a Vector<Double> instance contains the values.
oValueErr - This reference to a Vector<Double> instance contains the value
            errors.
oFlag     - This reference to a Vector<Bool> instance contains the flags.
eOrder    - This reference to a CalStatsFitter::ORDER enum contains the fit
            order (CalStatsFitter::AVERAGE = average fit, CalStatsFitter::LINEAR
	    = linear fit).
eWeight   - This reference to a CalStatsFitter::WEIGHT enum contains the weight
            flag (CalStatsFitter::YES = apply weights, CalStatsFitter::NO =
            don't apply weights).
dTrim     - This reference to a Double variable contains the dimensionless trim
            factor.

Outputs:
--------
oFlag - This reference to a Vector<Bool> instance contains the flags.
The reference to the FIT structure containing the trimmed least-squares fit,
returned via the function value.  If an error occurrs the FIT structure will
contain only the initial values and no error message will be printed.

Modification history:
---------------------
2011 Dec 08 - Nick Elias, NRAO
              Initial version.
2012 Jan 24 - Nick Elias, NRAO
              The robust fit used for the initial estimate of the parameters is
              no longer weighted.  The weighting is used only for the final
              "trimmed" least-squares fit.
2012 Jan 25 - Nick Elias, NRAO
              Error checking added.

*/

// -----------------------------------------------------------------------------

CalStatsFitter::FIT CalStatsFitter::robustFit( const Vector<Double>& oAbs,
    const Vector<Double>& oValue, const Vector<Double>& oValueErr,
    Vector<Bool>& oFlag, const ORDER& eOrder, const WEIGHT& eWeight,
    const Double& dTrim ) {

  // Eliminate the flagged abscissae, value, and value errors

  MaskedArray<Double> oAbsM( oAbs, !oFlag );
  Vector<Double> oAbsC( oAbsM.getCompressedArray() );

  MaskedArray<Double> oValueM( oValue, !oFlag );
  Vector<Double> oValueC( oValueM.getCompressedArray() );

  MaskedArray<Double> oValueErrM( oValueErr, !oFlag );
  Vector<Double> oDataErrC( oValueErrM.getCompressedArray() );


  // Minimize the mean absolute deviations to calculate the robust fit
  // parameters

  Double dSlope, dMedian;

  switch ((uInt) eOrder) {

    case (uInt) AVERAGE:
      dSlope = 0.0;
      try {
        dMedian = median( oValueC );
      }
      catch ( AipsError oAE ) {
        throw( oAE );
      }
      break;

    case (uInt) LINEAR:
      try {
        Double dSlopeT, dSlopeTErr;
        theil( oAbsC, oValueC, dSlopeT, dSlopeTErr );
        dSlope = slope( oAbsC, oValueC, dSlopeT, dSlopeTErr, 10.0, 20, 30 );
        dMedian = median( oValueC - dSlope*oAbsC );
      }
      catch( AipsError oAE ) {
        throw( oAE );
      }
      break;

    default:
      throw( AipsError( "Invalid type of robust fit" ) );
      break;

  }


  // Trim the data to make them more robust.  See the ICD of this member
  // function for more details.

  Vector<Double> oModel = Vector<Double>( dSlope*oAbs + dMedian );
  Vector<Double> oRes = Vector<Double>( oValue - oModel );

  Vector<Bool> oFlagTrim;
  if ( !(Bool) eWeight ) {
    oFlagTrim = fabs(oRes) >= dTrim*avdev(oRes);
  } else {
    oFlagTrim = fabs(oRes/oValueErr) >= dTrim*avdev(oRes/oValueErr);
  }

  oFlag = oFlag || oFlagTrim;


  // Calculate the "trimmed" least-squares fit 

  CalStatsFitter::FIT fitTrim;

  try {
    fitTrim = CalStatsFitter::FIT( lsqFit( oAbs, oValue, oValueErr, oFlag, eOrder,
                                           eWeight ) );
  }
  catch ( AipsError oAE ) {
    throw( oAE );
  }


  // Return the FIT structure (trimmed least-squares fit)

  return fitTrim;

}

// -----------------------------------------------------------------------------

/*

CalStatsFitter::slope

Description:
------------
This member function calculates the robust slope.

NB: For the stopping criterion to work reliably, the number of slopes input
parameter should be set to at least ~ 10.

Algorithm:
----------
* Set the initial minimum and maximum slope search range based on the estimated
  slope and its error times the dimensionless fudge factor.
  - Example: slope = 5, slope error = 1, fudge factor = 2 --> initial slope
    search range is 3 to 7.
* Outer iteration loop:
  - Divide up the slope range into "subslopes" according to the number of slopes
    input parameter (2 = bisection, but a value greater than ~ 10 should be used
    for the stopping criterion to work reliably).
  - Calculate the bracketing function (brackFunc() member function) for each
    subslope.
  - Inner subslope loop:
    # Look for a sign change in the bracketing function between the minimum
      subslope of the iteration and the present subslope (scalar signum()
      member function).
    # If there is no sign change, go to the next subslope.
    # If there is a sign change, reset the maximum slope search range to the
      present subslope, reset the minimum slope search range to the previous
      subslope, and break out of the subslope loop.
  - If this is the first iteration, check if the slope has been bracketed.  If
    not, throw an exception.
  - Check for convergence.  I only do this when more than half of the iterations
    have been completed because I want to insure that the average slope is a
    reasonable value.  This strategy is not strictly optimum, since more
    iterations will always be calculated than necessary.  Given the reliability
    and the nominal sizes of vectors containing calibration, I don't expect it
    to be an issue.

Inputs:
-------
oAbs       - This reference to a Vector<Double> instance contains the abscissae.
oValue     - This reference to a Vector<Double> instance contains the values.
dSlope     - This reference to a Double variable contains the initial slope
             estimate.
dSlopeErr  - This reference to a Double variable contains the initial slope
             error estimate.
dFudge     - This reference to a Double variable contains the dimensionless
             fudge factor.
uiNumSlope - This reference to a uInt variable contains the number of subslopes
             per iteration.
uiNumIter  - This reference to a uInt variable contains the number of
             iterations.

Outputs:
--------
The reference to the Double variable that contains the robust slope, returned
via the function value.

Modification history:
---------------------
2011 Dec 08 - Nick Elias, NRAO
              Initial version.
2012 Jan 25 - Nick Elias, NRAO
              Error checking added.

*/

// -----------------------------------------------------------------------------

Double& CalStatsFitter::slope( const Vector<Double>& oAbs,
    const Vector<Double>& oValue, const Double& dSlope, const Double& dSlopeErr,
    const Double& dFudge, const uInt& uiNumSlope, const uInt& uiNumIter ) {

  // Initialize the slope range

  Double dSlopeMin = dSlope - dFudge*dSlopeErr;
  Double dSlopeMax = dSlope + dFudge*dSlopeErr;


  // Perform the bisections iteratively to refine the robust slope range bracket

  for ( uInt i=0; i<uiNumIter; i++ ) {

    Bool bFlag = false;

    Double dSlopeInt = (dSlopeMax-dSlopeMin) / ((Double) uiNumSlope);

    Vector<Double> oIndgen( uiNumSlope+1 ); indgen( oIndgen );
    Vector<Double> oSlopes( oIndgen*dSlopeInt + dSlopeMin );

    Double dValueMin = brackFunc( oAbs, oValue, oSlopes[0] );

    for ( uInt s=1; s<=uiNumSlope; s++ ) {
      Double dValue = brackFunc( oAbs, oValue, oSlopes[s] );
      if ( dValue == 0.0 ) return( oSlopes[s] );
      if ( signum( dValue ) != signum( dValueMin ) ) {
        bFlag = true;
	dSlopeMin = oSlopes[s] - dSlopeInt;
	dSlopeMax = oSlopes[s];
	break;
      }
    }

    if ( !bFlag && i==0 ) {
      throw( AipsError( "Robust slope estimate has not been bracketed" ) );
    }

  }


  // Calculate the robust slope estimate (average of the last range bracket) and
  // return the reference to it

  Double* pdSlopeEst = new Double( 0.5 * ( dSlopeMin + dSlopeMax ) );

  return( *pdSlopeEst );

}

// -----------------------------------------------------------------------------

/*

CalStatsFitter::brackFunc

Description:
------------
This member function is root-finding bracketing function used to determine the
slope.

NB: The algorithm may be found in "Numerical Recipes in C" (in the first
edition, section 14.6 on pages 562-563).

Inputs:
-------
oAbs   - This reference to a Vector<Double> instance contains the abscissae.
oValue - This reference to a Vector<Double> instance contains the values.
dSlope - This reference to a Double variable contains the slope estimate.

Outputs:
--------
The reference to the Double variable containing the bracketing function value,
returned via the function value.

Modification history:
---------------------
2011 Dec 08 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

Double& CalStatsFitter::brackFunc( const Vector<Double>& oAbs,
    const Vector<Double>& oValue, const Double& dSlope ) {

  // Calculate the median (from Arraymath.h), which corresponds to the y offset

  Double dOffset = median( oValue - dSlope*oAbs );


  // Calculate the bracketing function value and return the reference to it

  Double* pdBrackFunc = new Double( 0.0 );
  *pdBrackFunc = sum( oAbs * signum( oValue - dSlope*oAbs - dOffset ) );

  return( *pdBrackFunc );

}

// -----------------------------------------------------------------------------

/*

CalStatsFitter::signum (scalar)

Description:
------------
This member function calculates the signum function.

Inputs:
-------
dValue - This reference to a Double variable contains the input value.

Outputs:
--------
The reference to the Double variable containing the signum value, returned via
the function value.

Modification history:
---------------------
2011 Dec 08 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

Double& CalStatsFitter::signum( const Double& dValue ) {

  // Calculate the signum value

  Double* pdSignum = new Double( 0.0 );

  if ( dValue > 0.0 ) {
    *pdSignum = 1.0;
  } else if ( dValue < 0.0 ) {
    *pdSignum = -1.0;
  }


  // Return the reference to the signum value

  return( *pdSignum );

}

// -----------------------------------------------------------------------------

/*

CalStatsFitter::signum (vector)

Description:
------------
This member function calculates the signum function.

Inputs:
-------
oValue - This reference to a Vector<Double> instance contains the input vector
         values.

Outputs:
--------
The reference to the Vector<Double>& object containing the signum values,
returned via the function value.

Modification history:
---------------------
2011 Dec 08 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

Vector<Double>& CalStatsFitter::signum( const Vector<Double>& oValue ) {

  // Calculate the signum values for all of the input vector values

  uInt uiNumValue = oValue.nelements();
  Vector<Double>* poSignum = new Vector<Double>( uiNumValue );

  for ( uInt v=0; v<uiNumValue; v++ ) {
    poSignum->operator[](v) = signum( oValue[v] );
  }


  // Return the reference to the vector that contains the signum values

  return( *poSignum );

}

// -----------------------------------------------------------------------------

/*

CalStatsFitter::theil

Description:
------------
This member function estimates the slope and slope error using Theil's method.

Algorithm:
----------
* Calculate the slope for each unique pair of points.
* The slope estimate is the median of all calculated slopes.
* The slope error estimate is the absolute deviation of all calculated slopes.

Inputs:
-------
oAbs   - This reference to a Vector<Double> instance contains the abscissae.
oValue - This reference to a Vector<Double> instance contains the values.

Outputs:
--------
dSlope    - This reference to a Double variable contains the slope estimate.
dSlopeErr - This reference to a Double variable contains the slope error
            estimate.

Modification history:
---------------------
2012 Jan 24 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

void CalStatsFitter::theil( const Vector<Double>& oAbs,
    const Vector<Double>& oValue, Double& dSlope, Double& dSlopeErr ) {

  // Calculate the slope and slope error estimate using Theil's method

  Vector<Double> oSlope;

  for ( uInt e1=0,s=0; e1<oAbs.nelements(); e1++ ) {
    for ( uInt e2=e1+1; e2<oAbs.nelements(); e2++ ) {
      if ( oValue[e2] == oValue[e1] && oAbs[e2] == oAbs[e1] ) continue;
      oSlope.resize( ++s, true );
      oSlope[s-1] = (oValue[e2]-oValue[e1]) / (oAbs[e2]-oAbs[e1]);
    }
  }

  dSlope = median( oSlope );
  dSlopeErr = avdev( oSlope );


  // Return

  return;

}

// -----------------------------------------------------------------------------
// End of CalStatsFitter private static member functions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// End of CalStatsFitter class
// -----------------------------------------------------------------------------

};

// -----------------------------------------------------------------------------
// End of casa namespace
// -----------------------------------------------------------------------------
