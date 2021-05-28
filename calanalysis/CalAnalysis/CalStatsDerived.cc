
// -----------------------------------------------------------------------------

/*

CalStatsDerived.cc

Description:
------------
This file contains the member functions of the classes derived from CalStats.

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
              Initial version created with classes CalStatsAmp and
              CalStatsPhase.
2011 Dec 11 - Nick Elias, NRAO
              Class CalStatsReal added.
2012 Jan 25 - Nick Elias, NRAO
              Logging capability added.  Error checking added.

*/

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <calanalysis/CalAnalysis/CalStatsDerived.h>
#include <casacore/casa/Utilities/GenSort.h>

// -----------------------------------------------------------------------------
// Start of casa namespace
// -----------------------------------------------------------------------------

using namespace casacore;
namespace casa {

const uInt CalStatsPhase::NUM_ITER_UNWRAP = 50;
const Double CalStatsPhase::NEW_RANGE_FACTOR = 5.0;

// -----------------------------------------------------------------------------
// Start of CalStatsReal class
// -----------------------------------------------------------------------------

/*

CalStatsReal

Description:
------------
This class feeds real data to the CalStats base class.

Inhertited classes:
-------------------
CalStats - This class calculates statistics of new CASA caltables.

Class public member functions:
------------------------------
CalStatsReal  - This constructor feeds real data to the CalStats base class.
~CalStatsReal - This destructor deallocates the internal memory of an instance.

2011 Dec 11 - Nick Elias, NRAO
              Initial version created with public member functions
              CalStatsReal() and ~CalStatsReal().

*/

// -----------------------------------------------------------------------------
// Start of CalStatsReal public member functions
// -----------------------------------------------------------------------------

/*

CalStatsReal::CalStatsReal

Description:
------------
This class feeds real data to the CalStats base class..

NB: The FEED axis is always included as an iteration axis by default because one
cannot perform a fit along it.  The other iteration axis is defined by the user.

NB: The default CalStats constructor is called first as the default, then the
standard one is called at the end.

Inputs:
-------
oValue          - This reference to a Cube<Double> instance contains the values.
oValueErr       - This reference to a Cube<Double> instance contains the value
                  errors.
oFlag           - This reference to a Cube<Bool> instance contains the flags.
oFeed           - This reference to a Vector<String> instance is the feed
                  abscissae.
oFrequency      - This reference to a Vector<Double> instance is the frequency
                  abscissae.
oTime           - This reference to a Vector<Double> instance is the time
                  abscissae.
eAxisIterUserID - This reference to a CalStats::AXIS enum contains either the
                  FREQUENCY or TIME iteration axes (user defined).

Outputs:
--------
None.

Modification history:
---------------------
2011 Dec 11 - Nick Elias, NRAO
              Initial version.
2012 Jan 25 - Nick Elias, NRAO
              Error checking added.

*/

// -----------------------------------------------------------------------------

CalStatsReal::CalStatsReal( const Cube<Double>& oValue,
    const Cube<Double>& oValueErr, const Cube<Bool>& oFlag,
    const Vector<String>& oFeed, const Vector<Double>& oFrequency,
    const Vector<Double>& oTime, const CalStats::AXIS& eAxisIterUserID )
    : CalStats() {

  // Create an instance of the CalStats base class constructor and copy its
  // state to this CalStatsReal instance

  CalStats* poCS;

  try {
    poCS = new CalStats( oValue, oValueErr, oFlag, oFeed, oFrequency, oTime,
        eAxisIterUserID );
  }
  catch ( AipsError oAE ) {
    throw( oAE );
  }

  oAxisIterID = IPosition( poCS->axisIterID() );
  eAxisNonIterID = poCS->axisNonIterID();

  oAxisIterFeed = Vector<String>( poCS->axisIterFeed() );
  oAxisIterUser = Vector<Double>( poCS->axisIterUser() );
  oAxisNonIter = Vector<Double>( poCS->axisNonIter() );

  oStatsShape = IPosition( poCS->statsShape() );

  poValue = new Cube<Double>( poCS->value() );
  poValueErr = new Cube<Double>( poCS->valueErr() );
  poFlag = new Cube<Bool>( poCS->flag() );

  poValueIter = new ArrayIterator<Double>( *poValue, oAxisIterID, false );
  poValueIter->reset();

  poValueErrIter = new ArrayIterator<Double>( *poValueErr, oAxisIterID, false );
  poValueErrIter->reset();

  poFlagIter = new ArrayIterator<Bool>( *poFlag, oAxisIterID, false );
  poFlagIter->reset();

  delete poCS;


  // Return

  return;

}

// -----------------------------------------------------------------------------

/*

CalStatsReal::~CalStatsReal

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
2011 Dec 11 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

CalStatsReal::~CalStatsReal( void ) {}

// -----------------------------------------------------------------------------
// End of CalStatsReal public member functions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// End of CalStatsReal class
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Start of CalStatsAmp class
// -----------------------------------------------------------------------------

/*

CalStatsAmp

Description:
------------
This class converts complex data to amplitudes and initializes the CalStats base
class.

Inhertited classes:
-------------------
CalStats - This class calculates statistics of new CASA caltables.

CalStatsAmp public member functions:
------------------------------------
CalStatsAmp  - This generic constructor converts complex data to amplitudes and
               initializes the CalStats base class.
~CalStatsAmp - This destructor deallocates the internal memory of an instance.

CalStatsAmp public static member functions:
-------------------------------------------
norm - This member function normalizes the amplitudes their errors.

Modification history:
---------------------
2011 Nov 15 - Nick Elias, NRAO
              Initial version created with public member functions are
              CalStatsAmp() and ~CalStatsAmp(); and public static member
              function is norm().

*/

// -----------------------------------------------------------------------------
// Start of CalStatsAmp public member functions
// -----------------------------------------------------------------------------

/*

CalStatsAmp::CalStatsAmp

Description:
------------
This class converts complex data and their errors to amplitudes and their errors
and initializes the CalStats base class.

NB: The FEED axis is always included as an iteration axis by default because one
cannot perform a fit along it.  The other iteration axis is defined by the user.

NB: The default CalStats constructor is called first as the default, then the
standard one is called at the end.

NB: The are no input data real-imaginary cross correlations available, so the
amplitude errors do not include them.

Inputs:
-------
oValue          - This reference to a Cube<DComplex> instance contains the
                  values.
oValueErr       - This reference to a Cube<Double> instance contains the value
                  errors (real and imaginary parts).
oFlag           - This reference to a Cube<Bool> instance contains the flags.
oFeed           - This reference to a Vector<String> instance is the feed
                  abscissae.
oFrequency      - This reference to a Vector<Double> instance is the frequency
                  abscissae.
oTime           - This reference to a Vector<Double> instance is the time
                  abscissae.
eAxisIterUserID - This reference to a CalStats::AXIS enum contains either the
                  FREQUENCY or TIME iteration axes (user defined).
bNorm           - This reference to a Bool variable contains the normalization
                  flag (true = normalize, false = don't normalize).

Outputs:
--------
None.

Modification history:
---------------------
2011 Nov 15 - Nick Elias, NRAO
              Initial version.
2012 Jan 25 - Nick Elias, NRAO
              Error checking added.
2012 Feb 15 - Nick Elias, NRAO
              Value error input parameter changed from DComplex to Double.
2012 Mar 28 - Nick Elias, NRAO
              Changed the normalization code so that vectors are iteratively fed
              to CalStatsAmp::norm().
2012 Mar 29 - Nick Elias, NRAO
              Member function can now normalize along the frequency and time
              axes.

*/

// -----------------------------------------------------------------------------

CalStatsAmp::CalStatsAmp( const Cube<DComplex>& oValue,
    const Cube<Double>& oValueErr, const Cube<Bool>& oFlag,
    const Vector<String>& oFeed, const Vector<Double>& oFrequency,
    const Vector<Double>& oTime, const CalStats::AXIS& eAxisIterUserID,
    const Bool& bNorm ) : CalStats() {

  // Calculate the amplitudes and their errors

  Cube<Double> oAmp( amplitude( oValue ) );

  Cube<Double> oAmpErr = oValueErr.copy();


  // Create the iterators.  The input flag cube is copied since it cannot be
  // modified.

  IPosition oIterShape( 2, (ssize_t) CalStats::FEED, eAxisIterUserID );

  Cube<Bool> oFlagCopy( oFlag.copy() );
  ArrayIterator<Bool> oFlagIter( oFlagCopy, oIterShape, false );

  ArrayIterator<Double> oAmpIter( oAmp, oIterShape, false );
  ArrayIterator<Double> oAmpErrIter( oAmpErr, oIterShape, false );


  // If selected, normalize the amplitudes and their errors

  while ( bNorm && !oAmpIter.pastEnd() ) {

    uInt uiNumAbs = 0;
    if ( eAxisIterUserID == CalStats::TIME ) {
      uiNumAbs = oFrequency.nelements();
    } else {
      uiNumAbs = oTime.nelements();
    }

    if ( uiNumAbs <= 1 ) {
      oFlagIter.next(); oAmpIter.next(); oAmpErrIter.next();
      continue;
    }

    IPosition oShape( 1, uiNumAbs );

    Vector<Bool> oFlagV( oFlagIter.array().copy().reform(oShape) );
    Vector<Double> oAmpV( oAmpIter.array().copy().reform(oShape) );
    Vector<Double> oAmpErrV( oAmpErrIter.array().copy().reform(oShape) );

    norm( oAmpV, oAmpErrV, oFlagV );

    oFlagIter.array() = oFlagV;
    oAmpIter.array() = oAmpV;
    oAmpErrIter.array() = oAmpErrV;

    oFlagIter.next(); oAmpIter.next(); oAmpErrIter.next();

  }


  // Create an instance of the CalStats base class constructor and copy its
  // state to this CalStatsAmp instance

  CalStats* poCS;

  try {
    poCS = new CalStats( oAmp, oAmpErr, oFlagCopy, oFeed, oFrequency, oTime,
        eAxisIterUserID );
  }
  catch ( AipsError oAE ) {
    throw( oAE );
  }

  oAxisIterID = IPosition( poCS->axisIterID() );
  eAxisNonIterID = poCS->axisNonIterID();

  oAxisIterFeed = Vector<String>( poCS->axisIterFeed() );
  oAxisIterUser = Vector<Double>( poCS->axisIterUser() );
  oAxisNonIter = Vector<Double>( poCS->axisNonIter() );

  oStatsShape = IPosition( poCS->statsShape() );

  poValue = new Cube<Double>( poCS->value() );
  poValueErr = new Cube<Double>( poCS->valueErr() );
  poFlag = new Cube<Bool>( poCS->flag() );

  poValueIter = new ArrayIterator<Double>( *poValue, oAxisIterID, false );
  poValueIter->reset();

  poValueErrIter = new ArrayIterator<Double>( *poValueErr, oAxisIterID, false );
  poValueErrIter->reset();

  poFlagIter = new ArrayIterator<Bool>( *poFlag, oAxisIterID, false );
  poFlagIter->reset();

  delete poCS;


  // Return

  return;

}

// -----------------------------------------------------------------------------

/*

CalStatsAmp::~CalStatsAmp

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
2011 Nov 15 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

CalStatsAmp::~CalStatsAmp( void ) {}

// -----------------------------------------------------------------------------
// End of CalStatsAmp public member functions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Start of CalStatsAmp public static member functions
// -----------------------------------------------------------------------------

/*

CalStatsAmp::norm

Description:
------------
This member function normalizes the amplitudes and their errors.

NB: The normalization is applied only along the frequency axis.

NB: The normalization is applied only when the number of unflagged frequencies
is greater than 1.

NB: All flags corresponding to amplitudes less then 1.0E-08 times the peak
amplitude (along the FREQUENCY axis) are updated to true.

Inputs:
-------
oAmp    - This reference to a Vector<Double> instance contains the unnormalized
          amplitudes.
oAmpErr - This reference to a Vector<Double> instance contains the unnormalized
          amplitude errors.
oFlag   - This reference to a Vector<Bool> instance contains the flags.

Outputs:
--------
oAmp    - This reference to a Vector<Double> instance contains the normalized
          amplitudes.
oAmpErr - This reference to a Vector<Double> instance contains the normalized
          amplitude errors.
oFlag   - This reference to a Vector<Bool> instance contains the flags.

Modification history:
---------------------
2011 Dec 11 - Nick Elias, NRAO
              Initial version.
2012 Jan 25 - Nick Elias, NRAO
              Logging capability added.
2012 Mar 28 - Nick Elias, NRAO
              Eliminated iteration and made input amplitude, amplitude error,
              and flag cubes into vectors.  Flags are now actually used.

*/

// -----------------------------------------------------------------------------

void CalStatsAmp::norm( Vector<Double>& oAmp, Vector<Double>& oAmpErr,
    Vector<Bool>& oFlag ) {

  // Eliminate the flagged amplitudes and their errors

  MaskedArray<Double> oAmpM( oAmp, !oFlag );
  Vector<Double> oAmpC( oAmpM.getCompressedArray() );

  MaskedArray<Double> oAmpErrM( oAmpErr, !oFlag );
  Vector<Double> oAmpErrC( oAmpErrM.getCompressedArray() );


  // Return if the length of the abscissa is unity

  uInt uiNumAbsC = oAmpC.nelements();

  if ( uiNumAbsC <= 1 ) {
    LogIO log( LogOrigin( "CalStatsAmp", "norm", WHERE ) );
    log << LogIO::WARN
        << "Abscissa has a dimension <= 1, no normalization"
        << LogIO::POST;
    return;
  }


  // Normalize the amplitudes and their errors along the abscissa.  Flag all low
  // amplitudes.

  Double dAmpMax = max( oAmpC );

  Vector<Bool> oFlagLow( oAmp <= (Double) 1.0E-08*dAmpMax );
  oFlag = oFlag || oFlagLow;

  uInt uiNumAbs = oAmp.nelements();

  for ( uInt a=0; a<uiNumAbs; a++ ) {
    if ( !oFlag[a] ) {
      oAmp[a] /= dAmpMax;
      oAmpErr[a] /= dAmpMax;
    }
  }


  // Return

  return;

}

// -----------------------------------------------------------------------------
// End of CalStatsAmp public static member functions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// End of CalStatsAmp class
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Start of CalStatsPhase class
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
// Start of CalStatsPhase public member functions
// -----------------------------------------------------------------------------

/*

CalStatsPhase::CalStatsPhase

Description:
------------
This class converts complex data and their errors to phases and their errors and
initializes the CalStats base class.

NB: The FEED axis is always included as an iteration axis by default because one
cannot perform a fit along it.  The other iteration axis is defined by the user.

NB: All flags corresponding to amplitudes less then 1.0E-08 times the peak
amplitude are updated to true.

NB: The default CalStats constructor is called first as the default, then the
standard one is called at the end.

NB: The are no input data real-imaginary cross correlations available, so the
phase errors do not include them.

NB: For unwrapping along the time axis (iteration axis CalStats::FREQUENCY), the
unwrapSimple() function is used.  For unwrapping along the frequency axis
(iteration axis CalStats::TIME), dJumpMax==0.0 leads to unwrapGD() while
dJumpMax!=0.0 leads to unwrapSimple().

Inputs:
-------
oValue          - This reference to a Cube<DComplex> instance contains the
                  values.
oValueErr       - This reference to a Cube<Double> instance contains the value
                  errors (real and imaginary parts).
oFlag           - This reference to a Cube<Bool> instance contains the flags.
oFeed           - This reference to a Vector<String> instance is the feed
                  abscissae.
oFrequency      - This reference to a Vector<Double> instance is the frequency
                  abscissae.
oTime           - This reference to a Vector<Double> instance is the time
                  abscissae.
eAxisIterUserID - This reference to a CalStats::AXIS enum contains either the
                  CalStats::FREQUENCY or CalStats::TIME iteration axes (user
                  defined).
bUnwrap         - This reference to a Bool variable contains the unwrapping flag
                  (true = unwrap, false = don't unwrap).
dJumpMax        - This reference to a Double variable contains the maximum
                  deviation from +/- M_PI for adjacent points to be unwrapped
                  by +/- 2.0*M_PI (in radians).  This parameter is always used
                  when the specified iteration axis is CalStats::FREQUENCY
                  (unwrapping along the CalStats::TIME axis).  If the specified
                  iteration axis is CalStats::TIME (unwrapping along the
                  CalStats::FREQUENCY axis), this parameter selects the type of
                  unwrapping (dJumpMax==0.0 --> group-delay unwrapping, dJumpMax
                  != 0.0 --> simple unwrapping).

Outputs:
--------
None.

Modification history:
---------------------
2011 Nov 15 - Nick Elias, NRAO
              Initial version. 
2011 Dec 11 - Nick Elias, NRAO
              Flag updates according to low amplitude was added.
2012 Jan 25 - Nick Elias, NRAO
              Error checking added.
2012 Feb 15 - Nick Elias, NRAO
              Value error input parameter changed from DComplex to Double.
2012 Mar 28 - Nick Elias, NRAO
              Changed the unwrapping code so that vectors are iteratively fed to
              CalStatsPhase::unwrapSimple() or CalStatsPhase::unwrapGD().
2012 Apr 01 - Nick Elias, NRAO
              Input parameter dJumpMax added.

*/

// -----------------------------------------------------------------------------

CalStatsPhase::CalStatsPhase( const Cube<DComplex>& oValue,
    const Cube<Double>& oValueErr, const Cube<Bool>& oFlag,
    const Vector<String>& oFeed, const Vector<Double>& oFrequency,
    const Vector<Double>& oTime, const CalStats::AXIS& eAxisIterUserID,
    const Bool& bUnwrap, const Double& dJumpMax ) : CalStats() {

  // Calculate the phases and the initial phase error cube (set to 0.0)

  Cube<Double> oPhase( phase(oValue) );

  Cube<Double> oPhaseErr( oPhase.shape() );


  // Create the iterators.  The input flag cube is copied since it cannot be
  // modified.

  IPosition oIterShape( 2, (ssize_t) CalStats::FEED, eAxisIterUserID );

  ReadOnlyArrayIterator<DComplex> oValueIter( oValue, oIterShape, false );
  ReadOnlyArrayIterator<Double> oValueErrIter( oValueErr, oIterShape, false );

  Cube<Bool> oFlagCopy( oFlag.copy() );
  ArrayIterator<Bool> oFlagIter( oFlagCopy, oIterShape, false );

  ArrayIterator<Double> oPhaseIter( oPhase, oIterShape, false );
  ArrayIterator<Double> oPhaseErrIter( oPhaseErr, oIterShape, false );


  // If selected, unwrap the phases

  while ( bUnwrap && !oPhaseIter.pastEnd() ) {

    uInt uiNumAbs = 0;
    if ( eAxisIterUserID == CalStats::TIME ) {
      uiNumAbs = oFrequency.nelements();
    } else {
      uiNumAbs = oTime.nelements();
    }

    if ( uiNumAbs <= 1 ) {
      oFlagIter.next(); oPhaseIter.next();
      continue;
    }

    IPosition oShape( 1, uiNumAbs );

    Vector<Bool> oFlagV( oFlagIter.array().copy().reform(oShape) );
    Vector<Double> oPhaseV( oPhaseIter.array().copy().reform(oShape) );

    if ( eAxisIterUserID == CalStats::TIME ) {
      if ( dJumpMax == 0.0 ) {
        unwrapGD( oPhaseV, oFrequency, oFlagV );
      } else {
        unwrapSimple( oPhaseV, dJumpMax, oFlagV );
      }
    } else {
      unwrapSimple( oPhaseV, dJumpMax, oFlagV );
    }

    oPhaseIter.array() = oPhaseV;

    oFlagIter.next(); oPhaseIter.next();

  }


  // Reset the iterators

  oFlagIter.reset(); oPhaseIter.next();


  // Calculate the phase errors.  They require dividing by amplitude.  Set the
  // flags according to low amplitudes (corresponding to phase errors greater
  // than M_PI).  All flagged phase errors are set to M_PI.  The iteration axes
  // are FEED and TIME.  A FREQUENCY for loop is located inside the FEED/TIME
  // iteration loop to set the errors.

  while ( !oValueIter.pastEnd() ) {

    uInt uiNumAbs = oValueIter.array().nelements();
    IPosition oShape( 1, uiNumAbs );

    Vector<Double> oPhaseErrV( oShape );

    Vector<DComplex> oValueV( oValueIter.array().copy().reform(oShape) );
    Vector<Double> oValueErrV( oValueErrIter.array().copy().reform(oShape) );

    Vector<Double> oAmpV( amplitude(oValueV) );

    Vector<Bool> oFlagV( oFlagIter.array().copy().reform(oShape) );
    Vector<Bool> oFlagLow( oAmpV <= oValueErrV/((Double) M_PI) );

    oFlagV = oFlagV || oFlagLow;
    oFlagIter.array() = oFlagV;

    for ( uInt a=0; a<uiNumAbs; a++ ) {
      if ( !oFlagV[a] ) {
        oPhaseErrV[a] = oValueErrV[a] / oAmpV[a];
      } else {
        oPhaseErrV[a] = (Double) M_PI;
      }
    }

    oPhaseErrIter.array() = oPhaseErrV;

    oValueIter.next(); oValueErrIter.next(); oFlagIter.next();
    oPhaseErrIter.next();

  }


  // Create an instance of the CalStats base class constructor and copy its
  // state to this CalStatsPhase instance

  CalStats* poCS;

  try {
    poCS = new CalStats( oPhase, oPhaseErr, oFlagCopy, oFeed, oFrequency, oTime,
        eAxisIterUserID );
  }
  catch ( AipsError oAE ) {
    throw( oAE );
  }

  oAxisIterID = IPosition( poCS->axisIterID() );
  eAxisNonIterID = poCS->axisNonIterID();

  oAxisIterFeed = Vector<String>( poCS->axisIterFeed() );
  oAxisIterUser = Vector<Double>( poCS->axisIterUser() );
  oAxisNonIter = Vector<Double>( poCS->axisNonIter() );

  oStatsShape = IPosition( poCS->statsShape() );

  poValue = new Cube<Double>( poCS->value() );
  poValueErr = new Cube<Double>( poCS->valueErr() );
  poFlag = new Cube<Bool>( poCS->flag() );

  poValueIter = new ArrayIterator<Double>( *poValue, oAxisIterID, false );
  poValueIter->reset();

  poValueErrIter = new ArrayIterator<Double>( *poValueErr, oAxisIterID, false );
  poValueErrIter->reset();

  poFlagIter = new ArrayIterator<Bool>( *poFlag, oAxisIterID, false );
  poFlagIter->reset();

  delete poCS;


  // Return

  return;

}

// -----------------------------------------------------------------------------

/*

CalStatsPhase::~CalStatsPhase

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
2011 Nov 15 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

CalStatsPhase::~CalStatsPhase( void ) {}

// -----------------------------------------------------------------------------
// End of CalStatsPhase public member functions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Start of CalStatsPhase public static member functions
// -----------------------------------------------------------------------------

/*

CalStatsPhase::unwrapSimple

Description:
------------
This member function performs a simple unwrapping procedure for both frequency
and temporal abscissae.

Algorithm:
----------
* If the first point is a little less than +M_PI and the second point is a
  little more than -M_PI, add 2.0*M_PI to the second point.
* If the first point is a little more than -M_PI and the second point is a
  ittle less than +M_PI, subtract 2.0*M_PI from the second point.
* "A little more" means within dJumpMax of +/- M_PI.
* Flagged data are ignored, which could be a problem if a lot of them occur
  sequentially.

Inputs:
-------
oPhase   - This reference to a Vector<Double> instance contains the wrapped
           phases.
dJumpMax - This reference to a Double variable contains the maximum deviation
           from +/- M_PI for adjacent points to be unwrapped by +/- 2.0*M_PI (in
           radians).
oFlag    - This reference to a Vector<Bool> instance contains the flags.

Outputs:
--------
oPhase - This reference to a Vector<Double> instance contains the unwrapped
         phases.

Modification history:
---------------------
2012 Mar 30 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

void CalStatsPhase::unwrapSimple( Vector<Double>& oPhase,
    const Double& dJumpMax, const Vector<Bool>& oFlag ) {

  // Initialize the number of elements and make an immutable copy of the input
  // phase vector

  uInt uiNumAbs = oPhase.nelements();

  Vector<Double> oPhaseIn( oPhase.copy() );


  // Perform the simple unwrapping.  The unwrapping occurs if subsequent points
  // are both within dJumpMax of M_PI or -M_PI.  Flagged data are ignored.

  for ( uInt a=1; a<uiNumAbs; a++ ) {

    if ( oFlag[a-1] ) continue;

    uInt a2 = 0;
    for ( a2=a; a2<uiNumAbs; a2++ ) {
      if ( !oFlag[a2] ) break;
    }
    if ( a2 >= uiNumAbs ) return;

    // The first point is a little less than +M_PI and the second point is a
    // little more than -M_PI, add 2.0*M_PI to the second point
    if ( M_PI-oPhaseIn[a-1] <= dJumpMax && M_PI+oPhaseIn[a2] <= dJumpMax ) {
      for ( uInt a3=a2; a3<uiNumAbs; a3++ ) oPhase[a3] += 2.0 * M_PI;
    }

    // The first point is a little more than -M_PI and the second point is a
    // little less than +M_PI, subtract 2.0*M_PI from the second point
    if ( M_PI+oPhaseIn[a-1] <= dJumpMax && M_PI-oPhaseIn[a2] <= dJumpMax ) {
      for ( uInt a3=a2; a3<uiNumAbs; a3++ ) oPhase[a3] -= 2.0 * M_PI;
    }

  }


  // Return

  return;

}

// -----------------------------------------------------------------------------

/*

CalStatsPhase::unwrapGD

Description:
------------
This member function unwraps the phases along the frequency axis with respect to
the group delay.

NB: The unwrapping is applied only along the frequency axis.  The unwrapping is
applied only when the number of unflagged frequencies is greater than 1 (setting
the bar very low).  If the number of unflagged frequencies is small, you may
get crappy results.

NB: This algorithm should work better with low-S/N data compared to simple
unwrapping, but it will not work if the S/N is abysmally low.  To understand
why this statement is true, consider an analogy with BPOLY.  BPOLY is used when
the S/N per channel is low.  The fit uses all bandpass data at once to maximize
the available S/N.  This phase unwrapping algorithm creates a fringe packet,
using all of the bandpass data at once, to estimate the delay before unwrapping.

NB: If the true delay is outside of the unaliased search window, the unwrapping
aliases into the search window.  This scenario is OK if only continuous phase
plotting is required.

NB: If the dispersion or other wiggles are large, this technique may not work
well.

Algorithm:
----------
* Sort the input frequencies and form the differences.  Sort the frequency
  differences.  The desired frequency increment is the minimum value (except for
  zero, which arises datasets from spectral windows with the same frequencies;
  choose the next largest value).
* Determine the maximum frequency.
* For the first iteration:
  - Calculate the initial time increment using the maximum frequency (Nyquist
    criterion).
  - Calculate the initial time range using the frequency increment.
  - Form the initial time vector.  The edges of the unaliased search window are
    +/- 0.5 times the initial time range (centered around zero group delay).
  - Form the initial squared-amplitude fringe packet.
  - Find the peak of the initial squared-amplitude fringe packet.  The time
    corresponding to this peak is the initial group delay estimate.
* For subsequent iterations:
  - Set the new time range to NEW_RANGE_FACTOR times the previous time
    increment.
  - Set the new time increment to the new time range divided by the number of
    times (this number is the same as the initial iteration and doesn't change).
  - Form the new time vector.  The edges of the search window are +/-  times the
    new time range (centered around the previous group delay estimate).
  - Form the new squared-amplitude fringe packet.
  - Find the peak of the new squared-amplitude fringe packet.  The time
    corresponding to this peak is the new group delay estimate.
  - Has the group delay converged?  If not repeat for another iteration,
    otherwise stop iterating.  The maximum number of iterations is
    NUM_ITER_UNWRAP.
* Form the phase versus frequency using the group delay.
* Subtract the group delay phases from the input phases to form the differential
  delay.
* Subtract the first differential delay from all of the differential delays.
* Starting at the first frequency, search for phase jumps.  For each one found,
  add/subtract 2.0*M_PI*N (N=the number of phase jumps; it can be positive or
  negative) for all subsequenct frequencies.  Flagged data are not unwrapped.

Inputs:
-------
oPhase     - This reference to a Vector<Double> instance contains the wrapped
             phases.
oFrequency - This reference to a Vector<Double> instance is the frequency
             abscissa.
oFlag      - This reference to a Vector<Bool> instance contains the flags.

Outputs:
--------
oPhase - This reference to a Vector<Double> instance contains the unwrapped
         phases.

Modification history:
---------------------
2011 Nov 15 - Nick Elias, NRAO
              Initial stub version.
2012 Jan 25 - Nick Elias, NRAO
              Logging capability added.
2012 Mar 27 - Nick Elias, NRAO
              Initial working version.
2012 Mar 28 - Nick Elias, NRAO
              Eliminated iteration and made input phase and flag cubes into
              vectors.  Flags are now actually used.

*/

// -----------------------------------------------------------------------------

void CalStatsPhase::unwrapGD( Vector<Double>& oPhase,
    const Vector<Double>& oFrequency, const Vector<Bool>& oFlag ) {

  // Eliminate the flagged phases and frequencies

  MaskedArray<Double> oPhaseM( oPhase, !oFlag );
  Vector<Double> oPhaseC( oPhaseM.getCompressedArray() );

  MaskedArray<Double> oFrequencyM( oFrequency, !oFlag );
  Vector<Double> oFrequencyC( oFrequencyM.getCompressedArray() );


  // Return if the length of the frequency axis is unity

  uInt uiNumFrequencyC = oFrequencyC.nelements();

  if ( uiNumFrequencyC <= 1 ) {
    LogIO log( LogOrigin( "CalStatsPhase", "unwrap", WHERE ) );
    log << LogIO::WARN
        << "Frequency axis has a dimension <= 1, no unwrapping performed"
        << LogIO::POST;
    return;
  }


  // Calculate the minimum frequency increment.  If there are overlapping
  // spectral windows, some of the frequency increments may be zero if the
  // duplicate channels are not flagged.  Make sure to ignore all zeros.

  Vector<Double> oFreqDeltaC( uiNumFrequencyC-1 );

  for ( uInt f=1; f<uiNumFrequencyC; f++ ) {
    oFreqDeltaC[f-1] = oFrequencyC[f] - oFrequencyC[f-1];
  }

  Sort::Order eOrder = Sort::Ascending;
  Int iOptions = Sort::QuickSort;
  GenSort<Double>::sort( oFreqDeltaC, eOrder, (int) iOptions );

  uInt fD = 0;
  for ( fD=0; fD<uiNumFrequencyC-1; fD++ ) {
    if ( oFreqDeltaC[fD] != 0.0 ) break;
  }

  if ( fD >= uiNumFrequencyC-1 ) {
    LogIO log( LogOrigin( "CalStatsPhase", "unwrap", WHERE ) );
    log << "Something is very wrong with the frequencies" << LogIO::EXCEPTION;
  }

  Double dFreqDeltaC = oFreqDeltaC[fD];


  // Calculate the maximum frequency

  Double dFreqMaxC = max( oFrequencyC );


  // Calculate the initial time increment and range

  Double dTimeDelta = 0.5 / dFreqMaxC;

  Double dTimeRange = 1.0 / dFreqDeltaC;


  // Initialize the time vector.  The center of the time range is zero group
  // delay.

  uInt uiNumTime = (uInt) fabs( dTimeRange/dTimeDelta + 1.0 );

  Vector<Double> oTime( uiNumTime );
  indgen<Double>( oTime, 0.0-0.5*dTimeRange, dTimeDelta );


  // Form the squared magnitude of the initial fringe packet and find the group
  // delay

  Vector<Double> oFringePacket( fringePacket2( oPhaseC, oFrequencyC, oTime ) );

  Double dGroupDelay = maxLocation<Double>( oTime, oFringePacket );


  // Initialize the number of iterations

  uInt uiNumIter = 0;


  // Iteratively improve the group delay estimate

  while ( uiNumIter < NUM_ITER_UNWRAP ) {

    dTimeRange = NEW_RANGE_FACTOR * dTimeDelta;
    Double dTimeDeltaNew = dTimeRange / (uiNumTime+1);

    indgen<Double>( oTime, dGroupDelay-0.5*dTimeDelta, dTimeDeltaNew );

    oFringePacket = fringePacket2( oPhaseC, oFrequencyC, oTime );
    Double dGroupDelayNew = maxLocation<Double>( oTime, oFringePacket );

    if ( fabs(dGroupDelayNew-dGroupDelay)/dTimeDelta < 1.0E-08 ) break;

    dTimeDelta = dTimeDeltaNew;
    dGroupDelay = dGroupDelayNew;

    uiNumIter++;

  }


  // If the number of iterations has been exceeded, print a warning and set
  // the group delay to the middle of the time range

  if ( uiNumIter > NUM_ITER_UNWRAP ) {

    dGroupDelay = mean( oTime );
  
    LogIO log( LogOrigin( "CalStatsPhase", "unwrap", WHERE ) );
    log << LogIO::WARN
        << "Number of iterations exceeded for group delay calculation\n"
        << "Using the mean time from the last iteration"
        << LogIO::POST;

  }


  // Calculate the differential phases, which are the input phases minus the
  // group delay.  Make sure to subtract the initial value from all values.

  uInt uiNumFrequency = oFrequency.nelements();

  Vector<Double> oDiffPhase( oPhase - 2.0*M_PI*oFrequency*dGroupDelay );
  oDiffPhase -= oDiffPhase[0];


  // If phase wraps are found, unwrap them.  Flagged data are not unwrapped.

  for ( uInt f=1; f<uiNumFrequency; f++ ) {

    if ( oFlag[f-1] ) continue;

    uInt f2 = 0;
    for ( f2=f; f2<uiNumFrequency; f2++ ) if ( !oFlag[f2] ) break;

    if ( f2 >= uiNumFrequency ) break;

    Double dDiffPhase = oDiffPhase[f2] - oDiffPhase[f-1];

    Int N = 0;
    while ( dDiffPhase + 2.0*M_PI*N <= -M_PI ) N++;
    while ( dDiffPhase + 2.0*M_PI*N > M_PI ) N--;

    if ( N == 0 ) continue;

    for ( uInt f3=f2; f3<uiNumFrequency; f3++ ) oPhase[f3] += 2.0 * M_PI * N;

  }


  // Return

  return;

}

// -----------------------------------------------------------------------------
// End of CalStatsPhase public static member functions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Start of CalStatsPhase private static member functions
// -----------------------------------------------------------------------------

/*

Description:
------------
This member function forms the squared-amplitude fringe packet.

Inputs:
-------
oPhase     - This Vector<Double>() instance contains the wrapped phases.
oFrequency - This Vector<Double>() instance contains the frequencies.
oTime      - This Vector<Double>() instance contains the times.

Outputs:
--------
The reference to the Vector<Double>() instance containing the squared-amplitude
fringe packet, returned via the function value.

Modification history:
---------------------
2012 Mar 27 - Nick Elias, NRAO
              Initial version.

*/

// -----------------------------------------------------------------------------

Vector<Double>& CalStatsPhase::fringePacket2( const Vector<Double>& oPhase,
    const Vector<Double>& oFrequency, const Vector<Double>& oTime ) {

  // Initialize the pointer to the squared-amplitude fringe packet

  uInt uiNumTime = oTime.nelements();

  Vector<Double>* poFringePacket;
  poFringePacket = new Vector<Double>( uiNumTime, 0.0 );


  // Calculate the complex fringe packet for all times using all frequencies and
  // phases

  uInt uiNumFrequency = oFrequency.nelements();

  Vector<DComplex> oFringePacketC( uiNumTime, DComplex( 0.0, 0.0 ) );

  for ( uInt f=0; f<uiNumFrequency; f++ ) {
    Vector<DComplex> oFringePacketCF( uiNumTime, DComplex( 0.0, 0.0 ) );
    setReal( oFringePacketCF, cos( 2.0*M_PI*oFrequency[f]*oTime - oPhase[f] ) );
    setImag( oFringePacketCF, sin( 2.0*M_PI*oFrequency[f]*oTime - oPhase[f] ) );
    operator+=( oFringePacketC, oFringePacketCF );
  }


  // Calculate and return the reference to the squared-amplitude fringe packet

  *poFringePacket = square( amplitude( oFringePacketC ) );
  *poFringePacket /= (Double) uiNumFrequency * uiNumFrequency;

  return( *poFringePacket );

}

// -----------------------------------------------------------------------------
// End of CalStatsPhase private static member functions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// End of CalStatsPhase class
// -----------------------------------------------------------------------------

};

// -----------------------------------------------------------------------------
// End of casa namespace
// -----------------------------------------------------------------------------
