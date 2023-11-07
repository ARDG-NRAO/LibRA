
// -----------------------------------------------------------------------------

#include <cstdlib>

#include <math.h>
double rgauss( void );

#include <calanalysis/CalAnalysis/CalStats.h>

using namespace casacore;
using namespace casa;

// -----------------------------------------------------------------------------

int main( void ) {

  // Initialize the shape of the input cubes

  uInt uiNumPol = 2;
  uInt uiNumFreq = 64;
  uInt uiNumTime = 100;

  IPosition oShape( 3, uiNumPol, uiNumFreq, uiNumTime );


  // Initialize the input data cube (constant across frequency axis)

  Cube<Double> oData( oShape );

  for ( uInt p=0; p<uiNumPol; p++ ) {
    for ( uInt t=0; t<uiNumTime; t++ ) {
      Double dData = 10.0 * ( ((Double) rand())/((Double) RAND_MAX) - 0.5);
      for ( uInt f=0; f<uiNumFreq; f++ ) {
        oData.operator()(p,f,t) = dData + 0.1*rgauss();
      }
    }
  }


  // Initialize the input data error cube

  Cube<Double> oDataErr( oShape );

  for ( uInt p=0; p<uiNumPol; p++ ) {
    for ( uInt t=0; t<uiNumTime; t++ ) {
      Double dDataErr = 0.1;
      for ( uInt f=0; f<uiNumFreq; f++ ) {
        oDataErr.operator()(p,f,t) = dDataErr;
      }
    }
  }


  // Initialize the input flag cube

  Cube<Bool> oFlag( oShape, false );


  // Initialize the polarization (feed) abscissa

  Vector<String> oFeed( uiNumPol );
  oFeed[0] = "R";
  oFeed[1] = "L";


  // Initialize the frequency abscissa

  Vector<Double> oFreq( uiNumFreq );
  for ( uInt f=0; f<uiNumFreq; f++ ) oFreq[f] = f*2.0E+06 + 10.0E+09;


  // Initialize the time abscissa

  Vector<Double> oTime( uiNumTime );
  for ( uInt t=0; t<uiNumTime; t++ ) oTime[t] = (Double) t;


  // Initialize the user-supplied iteration axis (the polarization axis is
  // always an iteration axis, by default)

  CalStats::AXIS eAxis = CalStats::TIME;


  // Create a CalStatsReal object and get the data iterated along the
  // polarization and time axes

  CalStats oCS( oData, oDataErr, oFlag, oFeed, oFreq, oTime, eAxis );

  CalStats::ARG<CalStats::NONE> oArg;
  Matrix<CalStats::OUT<CalStats::NONE> >
      oMatrix( oCS.stats<CalStats::NONE>( oArg ) );


  // Write the data for each polarization and time axis

  for ( uInt p=0; p<uiNumPol; p++ ) {
    for ( uInt t=0; t<uiNumTime; t++ ) {
      cout << endl << flush;
      cout << p << ' ' << t << endl << flush;
      cout << oMatrix(p,t).oAxes.eAxisIterFeedID << ' '
           << oMatrix(p,t).oAxes.eAxisIterUserID << ' '
           << oMatrix(p,t).oAxes.eAxisNonIterID << ' '
           << endl << flush;
      cout << oMatrix(p,t).oAxes.sFeed << ' '
           << oMatrix(p,t).oAxes.dAxisIterUser << endl << flush;
      cout << oMatrix(p,t).oData.oAbs << endl << flush;
      cout << oMatrix(p,t).oData.oFlag << endl << flush;
    }
  }


  // Return 0

  return( 0 );

}

// -----------------------------------------------------------------------------

double rgauss( void ) {

  static int iset=0;
  static double gset;
  double fac,r,v1,v2;

  if ( iset == 0 ) {
    do {
      v1 = 2.0 * (((double) rand())/((double) RAND_MAX) - 0.5);
      v2 = 2.0 * (((double) rand())/((double) RAND_MAX) - 0.5);
      r = v1*v1 + v2*v2;
    } while ( r >= 1.0 || r == 0.0 );
    fac = sqrt( -2.0*log(r)/r );
    gset = v1*fac;
    iset = 1;
    return( v2*fac );
  } else {
    iset = 0;
    return( gset );
  }

}
