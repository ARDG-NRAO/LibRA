
// -----------------------------------------------------------------------------

/*

CalStats.h

Description:
------------
This header file contains definitions for the CalStats class.

Classes:
--------
CalStats - This class calculates statistics of new CASA caltables.

Modification history:
---------------------
2011 Nov 11 - Nick Elias, NRAO
              Initial version.
2012 Jan 25 - Nick Elias, NRAO
              Logging capability added.  Error checking added.

*/

// -----------------------------------------------------------------------------
// Start of define macro to prevent multiple loading
// -----------------------------------------------------------------------------

#ifndef CAL_STATS_H
#define CAL_STATS_H

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <cstring>

#define _USE_MATH_DEFINES
#include <cmath>

#include <casa/BasicSL/String.h>

#include <casa/aips.h>

#include <casa/Exceptions/Error.h>
#include <casa/Logging/LogIO.h>

#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Cube.h>
#include <casa/Arrays/ArrayIter.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/ArrayLogical.h>

#include <calanalysis/CalAnalysis/CalStatsFitter.h>

// -----------------------------------------------------------------------------
// Start of casa namespace
// -----------------------------------------------------------------------------

namespace casa {

// -----------------------------------------------------------------------------
// Start of CalStats class definition
// -----------------------------------------------------------------------------

/*

CalStats

Description:
------------
This class calculates statistics of new CASA caltables.

NB: At present this class gets data and calculates fit statistics, but other
things such as histogram statistics will be added later.

In a nutshell:
--------------
* This class can get data (no statistics calculated) and calculate fit
  statistics from these data.
* Hooks are present in the code for calculating histogram statistics in the
  future.
* This class employs internal iterators to move uniformly through the data
  cubes.  This process is invisible to the user.
* The input data are cubes whose axes are feed, frequency, and time.  The other
  axes, such as antenna 1, antenna 2, etc. are handled in another class.  There
  are two iteration axes and one non-iteration axis, which means that this class
  returns ONE-dimensional quantities (data, fits, or histograms) for each
  iteration.  This class does not deal with multi-dimensional fits and
  histograms.
* The feed axis is always an iteration axis (fits cannot be calculated along
  it).  The user-defined iteration axis is either frequency or time, which means
  that the other axis is either time or frequency.
* The input data are obtained from the NewCalTable and NewCalTabIter classes
  that iterate along antenna1, antenna2, spw, etc.
* Once an instance is created, the stats<T> member function is the main user
  interface to calculate statistics.  The choice of T determines which function
  is used (getting data, fit statistics, histogram statistics).
* To minimize the amount of duplicated code, some of the member functions are
  templated and some of the templated member functions are specialized.  The
  class is not templated.

Nested classes:
---------------
AXES   - This nested class contains the axes for the CalStats class.
DATA   - This nested class contains the data for the CalStats class.
ARG<T> - This nested template class contains the arguments for the
         CalStats::stats<T>() template member function.
OUT<T> - This nested template class contains the outputs for the
         CalStats::stats<T>() template member function.

Class public member functions:
------------------------------
CalStats  - This constructor saves input abscissae and data cubes to internal
            copies so that statistics can be calculated.
CalStats  - This copy constructor is unused by this class and unavailable when
            an instance is created.
operator= - This operator= function is unused by this class and unavailable when
            an instance is created.
~CalStats - This destructor deallocates the internal memory of an instance.

Class public state member functions:
------------------------------------
axisIterID    - This member function returns the iteration axis IDs.
axisNonIterID - This member function returns the non-iteration axis ID.
axisIterFeed  - This member function returns the feed iteration axis values.
axisIterUser  - This member function returns the user-defined iteration axis
                values.
axisNonIter   - This member function returns the non-iteration axis values.
statsShape    - This member function returns the shape of the output statistics
                cube.
value         - This member function returns the input value.
valueErr      - This member function returns the input value errors.
flag          - This member function returns the input flags.

Class static public member functions:
-------------------------------------
axisName - This function returns the string corresponding to the CalStats::AXIS
           enum.

Class template public stats member functions:
---------------------------------------------
stats<T> - This member function is the main user interface for calculating the
           the statistics for all iterations.  Allowed T: CalStats::NONE only
	   returns the input data, CalStatsFitter::FIT calculates fit
           statistics, and CalStatsHist::HIST calculates histogram statistics.

Class specialized template public stats member functions:
---------------------------------------------------------
statsWrap<T> - This member function wraps statistics functions and provides a
               uniform interface to stats<T>().  Allowed T: CalStats::NONE only
	       returns the input data, CalStatsFitter::FIT calculates fit
               statistics, and CalStatsHist::HIST calculates histogram
               statistics.

Class protected member functions:
---------------------------------
CalStats - This default constructor is unused by this class and unavailable when
           an instance is created.
next     - This member function simultaneously iterates all of the internal
           copies of the input data cubes.
reset    - This member function simultaneously resets the iterators of all of
           the internal copies of the input data cubes.

Modification history:
---------------------
2011 Nov 11 - Nick Elias, NRAO
              Initial version created with public member functions CalStats()
              (generic) and ~CalStats(); protected member functions CalStats()
              (default), CalStats() (copy), operator=(), next() and reset().
2011 Nov 15 - Nick Elias, NRAO
              Moved the CalTypes namespace and its members, originally defined
              in this file, to the CalStatsFitter class.  Also, the
              CalTypes::AXIS typedef was replaced by CalStats::AXIS.
2011 Dec 08 - Nick Elias, NRAO
              Added the fit axis vector to the internal variables.
2011 Dec 11 - Nick Elias, NRAO
              The structure CalStats::FIT was added and replaces the
              CalStatsFitter::FIT structure output of the calcFit() public stats
              member function (the latter is now part of the former).  Added
              init() and dealloc() public member functions.
2011 Dec 14 - Nick Elias, NRAO
              The structures CalStats::AXES, CalStats::DATA, and CalStats::NONE
              added.  The nested class CalStats::OUT<T> added (C++ does not
              allow templated typedefs, so a nested class is used instead).
              Public member function getData() added.  Public static member
              functions initAxes(), initGet(), initResultNone(),
              initResultFit(), and dealloc() added (overloaded for
              CalStats::DATA and CalStats::OUT<CalStatsFitter::FIT>).  Removed
              public member functions init() and dealloc() (non-overloaded
              version).
2011 Dec 15 - Nick Elias, NRAO
              Private member functions next() and reset() now protected member
              functions.  State public member functions axisIterID(),
              axisNonIterID(), axisIterFeed(), axisIterUser(), axisNonIter(),
              statsShape(), value(), valueErr(), and flag() added.
2011 Dec 16 - Nick Elias, NRAO
              Public member functions getData() and calcFit() replaced by
              stats<T>() template public member function.  Specialized template
              public member function statsWrap<T>() added.  Public static member
              functions initAxes(), initData(), initResultNone(), and
              initResultFit() replaced by template public static member function
              init<T>().
2011 Dec 21 - Nick Elias, NRAO
              casacore::Template public static member functions init<T>() and dealloc<T>
              removed because all of their duties are subsumed by the nested
              classes AXES, DATA, ARG, and OUT (they were previously
              structures).
2012 Jan 25 - Nick Elias, NRAO
              Created working versions of CalStats() (copy) and operator=() and
              turned them into public member functions.
2012 Mar 05 - Nick Elias, NRAO
              Static public member function axisName() added.

*/

// -----------------------------------------------------------------------------

class CalStats {

  public:

    // Axis enums.  There are always two iteration axes.  The FEED axis is
    // always the first interation axis.  Either the FREQUENCY or TIME axis is
    // the other (user-defined) iteration axis.  The remaining axis (TIME or
    // FREQUENCY) is therefore the non-iteration axis.  NB: If additional enums
    // are added, additional names must be added to the axisName() static
    // private member function.
    typedef enum AXIS {
      INIT=-1, FEED=0, FREQUENCY, TIME
    } AXIS;

    // AXES nested class
    class AXES {
      public:
        AXIS eAxisIterFeedID; // FEED iteration axis ID
        AXIS eAxisIterUserID; // User-defined iteration axis ID
        AXIS eAxisNonIterID;  // Non-iteration axis ID
        casacore::String sFeed;         // FEED axis value
        casacore::Double dAxisIterUser; // User-defined iteration axis value
        AXES( void );
        AXES( const AXES& oAxes );
        ~AXES( void );
        AXES& operator=( const AXES& oAxes );
    };

    // DATA nested class
    class DATA {
      public:
        casacore::Vector<casacore::Double> oAbs;      // The abscissae (non-iteration axis values) 
        casacore::Vector<casacore::Double> oValue;    // The values
        casacore::Vector<casacore::Double> oValueErr; // The value errors
        casacore::Vector<casacore::Bool> oFlag;       // The flags
        DATA( void );
        DATA( const DATA& oDataIn );
        ~DATA( void );
        DATA& operator=( const DATA& oDataIn );
    };

    // Statistics ARG nested class (allowed T: CalStats::NONE,
    // CalStatsFitter::FIT, or CalStatsHist::HIST), used as an input to
    // stats<T>() and statsWrap<T>().  C++ also does not allow explicit template
    // specialization of nested classes within the parent class, so they are
    // defined immediately after this class.
    template <typename T> class ARG {};

    // NONE nested class
    class NONE {};

    // Statistics OUT nested class (allowed T: CalStats::NONE,
    // CalStatsFitter::FIT, or CalStatsHist::HIST), used to hold the output of
    // statsWrap<T>().
    template <typename T> class OUT {
      public:
      AXES oAxes;
      DATA oData;
      T oT;
      OUT( void );
      OUT( const OUT& oOut );
      ~OUT( void );
      OUT& operator=( const OUT& oOut );
    };

    // Generic constructor
    CalStats( const casacore::Cube<casacore::Double>& oValue, const casacore::Cube<casacore::Double>& oValueErr,
        const casacore::Cube<casacore::Bool>& oFlag, const casacore::Vector<casacore::String>& oFeed,
        const casacore::Vector<casacore::Double>& oFrequency, const casacore::Vector<casacore::Double>& oTime,
        const AXIS& eAxisIterUser );

    // Copy constructor and operator=() function
    CalStats( const CalStats& oCalStats );
    CalStats& operator=( const CalStats& oCalStats );

    // Destructor
    virtual ~CalStats( void );

    // Axis ID states
    casacore::IPosition axisIterID( void ) const;
    AXIS axisNonIterID( void ) const;

    // Axis value states
    casacore::Vector<casacore::String> axisIterFeed( void ) const;
    casacore::Vector<casacore::Double> axisIterUser( void ) const;
    casacore::Vector<casacore::Double> axisNonIter( void ) const;

    // Output statistics cube shape state
    casacore::IPosition statsShape( void ) const;

    // casacore::Input data states
    casacore::Cube<casacore::Double>& value( void ) const;
    casacore::Cube<casacore::Double>& valueErr( void ) const;
    casacore::Cube<casacore::Bool>& flag( void ) const;

    // The axis names
    static casacore::String axisName( const AXIS& eAxis );

    // Calculate statistics (allowed T: CalStats::NONE gets data without
    // calculating statistics, CalStatsFitter::FIT calculates fits, and
    // CalStatsHist::HIST calculates histogram statistics).  Member function
    // stats() is the main user interface and statsWrap() is the supporting
    // wrapper.
    template <typename T> casacore::Matrix<OUT<T> > stats( const ARG<T>& oArg );
    template <typename T> T statsWrap( const casacore::Vector<casacore::Double>& oAbs,
        const casacore::Vector<casacore::Double>& oValue, const casacore::Vector<casacore::Double>& oValueErr,
        casacore::Vector<casacore::Bool>& oFlag, const ARG<T>& oArg );

  protected:

    // The axis IDs.  The two iteration axes are FEED (always) and either TIME
    // or FREQUENCY (user defined).  The non-iteration axis is either FREQUENCY
    // or TIME (the opposite of the user-defined iteration axis).
    casacore::IPosition oAxisIterID;
    AXIS eAxisNonIterID;

    // Internal copies of the iteration and non-iteration axis values
    casacore::Vector<casacore::String> oAxisIterFeed; // Feed axis iteration axis values
    casacore::Vector<casacore::Double> oAxisIterUser; // User-defined iteration axis values
    casacore::Vector<casacore::Double> oAxisNonIter;  // Non-iteration axis values

    // Shape of the output statistics cubes
    casacore::IPosition oStatsShape;

    // Internal copies of input parameter cubes
    casacore::Cube<casacore::Double>* poValue;
    casacore::Cube<casacore::Double>* poValueErr;
    casacore::Cube<casacore::Bool>* poFlag;

    // casacore::Input parameter cube iterators
    casacore::ArrayIterator<casacore::Double>* poValueIter;
    casacore::ArrayIterator<casacore::Double>* poValueErrIter;
    casacore::ArrayIterator<casacore::Bool>* poFlagIter;

    // Unused constructor
    CalStats( void );

    // Simultaneously increment and reset all input parameter cube iterators
    void next( void );
    void reset( void );

};

// -----------------------------------------------------------------------------
// End of CalStats class definition
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Start of ARG<T> specialized class templates
// -----------------------------------------------------------------------------

// Specialization for the CalStats::NONE() class.  It tells the
// CalStats::stats<CalStats::NONE>() method just to return the data with no
// processing.
template <> class CalStats::ARG<CalStats::NONE> {};

// Specialization for the CalStatsFitter::FIT() class.  It tells the
// CalStats::stats<CalStatsFitter::FIT>() method to perform fits and return the
// data and fit parameters.
template <> class CalStats::ARG<CalStatsFitter::FIT> {
  public:
    CalStatsFitter::ORDER eOrder;
    CalStatsFitter::TYPE eType;
    CalStatsFitter::WEIGHT eWeight;
    ARG( void ) {
      eOrder = CalStatsFitter::ORDER_INIT;
      eType = CalStatsFitter::TYPE_INIT;
      eWeight = CalStatsFitter::WEIGHT_INIT;
      return;
    }
};

// -----------------------------------------------------------------------------
// End of ARG<T> specialized class templates
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Start of OUT<T> specialized class template public member functions
// -----------------------------------------------------------------------------

// Default constructor
template <typename T>
CalStats::OUT<T>::OUT( void ) {
  oAxes = CalStats::AXES();
  oData = CalStats::DATA();
  oT = T();
}

// Copy constructor
template <typename T>
  CalStats::OUT<T>::OUT( const CalStats::OUT<T>& oOut ) {
  oAxes = CalStats::AXES( oOut.oAxes );
  oData = CalStats::DATA( oOut.oData );
  oT = T( oOut.oT );
}

// Destructor
template <typename T>
CalStats::OUT<T>::~OUT( void ) {}

// operator=
template <typename T>
CalStats::OUT<T>& CalStats::OUT<T>::operator=( const CalStats::OUT<T>& oOut ) {
  if ( this != &oOut ) {
    oAxes = CalStats::AXES( oOut.oAxes );
    oData = CalStats::DATA( oOut.oData );
    oT = T( oOut.oT );
  }
  return( *this );
}

// -----------------------------------------------------------------------------
// End of OUT<T> specialized class template public member functions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Start of CalStats::stats<T> template public statistics member function
// -----------------------------------------------------------------------------

/*

CalStats::stats<T>

Description:
------------
This member function calculates the desired statistics.  The allowed templates
are CalStats::NONE (no statistics, just the input data), CalStatsFitter::FIT
(fit statistics), and CalStatsHist::HIST (histogram statistics).

Inputs:
-------
oArg - This reference to a CalStats::ARG<T> instance contains the extra input
       parameters.

Outputs:
--------
The reference to the casacore::Matrix<CalStats::OUT<T> > instance containing the
statistics, returned via the function value.

Modification history:
---------------------
2011 Nov 11 - Nick Elias, NRAO
              Initial version.  This template member function replaces the
              getData() and calcFit() member functions.
2012 Jan 25 - Nick Elias, NRAO
              Logging capability added.

*/

// -----------------------------------------------------------------------------

template <typename T>
casacore::Matrix<CalStats::OUT<T>> CalStats::stats( const CalStats::ARG<T>& oArg ) {

  // Initialize the CalStats::OUT<T> array and its iterator

  casacore::Array<CalStats::OUT<T> > out( oStatsShape );

  casacore::ArrayIterator<CalStats::OUT<T> > oOutIter( out, oAxisIterID, false );


  // For each iteration, convert the resulting arrays to vectors and feed them
  // to the CalStatsFitter::fit() function

  while ( !poValueIter->pastEnd() ) {

    casacore::IPosition oPos( poValueIter->pos() );

    casacore::uInt uiLength = poValueIter->array().nelements();
    casacore::IPosition oShape( 1, uiLength );

    casacore::Vector<casacore::Double> oAbs( oAxisNonIter.copy() );
    casacore::Vector<casacore::Double> oValue( poValueIter->array().copy().reform(oShape) );
    casacore::Vector<casacore::Double> oValueErr( poValueErrIter->array().copy().reform(oShape) );
    casacore::Vector<casacore::Bool> oFlag( poFlagIter->array().copy().reform(oShape) );

    CalStats::OUT<T> oOut;

    oOut.oAxes.eAxisIterFeedID = (CalStats::AXIS) oAxisIterID[0];
    oOut.oAxes.eAxisIterUserID = (CalStats::AXIS) oAxisIterID[1];
    oOut.oAxes.eAxisNonIterID = eAxisNonIterID;
    oOut.oAxes.sFeed = casacore::String( oAxisIterFeed[oPos[0]] );
    oOut.oAxes.dAxisIterUser = oAxisIterUser[oPos[oAxisIterID[1]]];

    oOut.oData.oAbs = casacore::Vector<casacore::Double>( oAbs );
    oOut.oData.oValue = casacore::Vector<casacore::Double>( oValue );
    oOut.oData.oValueErr = casacore::Vector<casacore::Double>( oValueErr );

    try {
      oOut.oT = statsWrap<T>( oAbs, oValue, oValueErr, oFlag, oArg );
    }

    catch ( casacore::AipsError oAE ) {
      casacore::LogIO log( casacore::LogOrigin( "CalStats", "stats<T>()", WHERE ) );
      log << casacore::LogIO::NORMAL << oAE.getMesg() << ", iteration: "
          << oPos.asVector() << ", continuing ..." << casacore::LogIO::POST;
      oOut.oT = T();
    }

    // The flag output vector is set here because robust fitting can change them
    oOut.oData.oFlag = casacore::Vector<casacore::Bool>( oFlag );

    oOutIter.array() = casacore::Vector<CalStats::OUT<T> >( 1, oOut );

    next(); oOutIter.next();

  }


  // Reset the input parameter iterators

  reset();


  // Return the reference to the casacore::Matrix<CalStats::OUT<T> > instance

  out.removeDegenerate();

  casacore::Matrix<CalStats::OUT<T> > outMatrix = out;
  return outMatrix;

}

// -----------------------------------------------------------------------------
// End of CalStats::stats<T> template public statistics member function
// -----------------------------------------------------------------------------

};

// -----------------------------------------------------------------------------
// End of casa namespace
// -----------------------------------------------------------------------------

#endif

// -----------------------------------------------------------------------------
// End of define macro to prevent multiple loading
// -----------------------------------------------------------------------------
