/*
 * PhaseShiftTvi.cc
 *
 *  Created on: Mar 12, 2015
 *      Author: jjacobs
 */

#include <msvis/MSVis/PhaseShiftTvi.h>
#include <casa/Arrays/Cube.h>
#include <casa/Arrays/Matrix.h>

using namespace casacore;
namespace casa {

namespace vi {

PhaseShiftTvi::PhaseShiftTvi (ViImplementation2 * inputVi, double dXArcseconds,
                              double dYArcseconds)
: TransformingVi2 (inputVi),
  dXArcseconds_p (dXArcseconds),
  dYArcseconds_p (dYArcseconds)
{}

void
PhaseShiftTvi::phaseCenterShift (Cube<Complex> & cube) const
{
    // Notes:
    //
    // 1) This is an operation that is likely performed elsewhere in CASA so that we ought to
    //    factor out any commonality.
    //
    // 2) This operation will put the uvw and phaseCenter values out alignment with the shifted
    //    data.  If the data is going to be consumed by a more exacting application than plotMS
    //    these values ought to be adjusted as well by this TVI.

    // Get cube shape as scalars

    IPosition shape = cube.shape();
    Int nCorrelations = shape (0);
    Int nChannels = shape (1);
    rownr_t nRows = shape (2);

    const Int uIndex = 0; // uvw matrix is [3,nRows]
    const Int vIndex = 1;

    // Convert the offsets from arcseconds to radians

    double radiansPerArcsecond = C::pi / (180 * 3600);
    double dXRadians = dXArcseconds_p * radiansPerArcsecond;
    double dYRadians = dYArcseconds_p * radiansPerArcsecond;

    // Capture the uvw and the time for the VB window.

    Matrix<Double> theUvw;
    getVii()->uvw (theUvw);

    Vector<Double> timeForRows;
    getVii()->time (timeForRows);

    // Allow for the case where the iterator window has many timestamps by making the
    // frequencies a function of row number but not doing it when not needed.

    double frequencyTime = timeForRows (0);
    Vector<Double> frequency = getVii()->getFrequencies (frequencyTime, getObservatoryFrequencyType ());

    for (rownr_t row = 0; row < nRows; row ++){

        double t = timeForRows (row);

        if (t != frequencyTime){

            // This row has a different timestamp, so get the new frequencies

            frequencyTime = t;
            frequency = getVii()->getFrequencies (frequencyTime, getObservatoryFrequencyType ());
        }

        // Compute the phase change per Hz for this row

        double u = theUvw (uIndex, row);
        double v = theUvw (vIndex, row);
        double rowPhasePerHz = (u * dXRadians + v * dYRadians) * -2 * C::pi / C::c;

        for (Int channel = 0; channel < nChannels; channel ++){

            // Compute the phase change given this channel's frequency

            double channelPhase = rowPhasePerHz * frequency (channel);
            Complex phasor (cos(channelPhase), sin(channelPhase));

            for (Int correlation = 0; correlation < nCorrelations; correlation ++){

                // Apply the phase change

                cube (correlation, channel, row) *= phasor;
            }
        }
    }
}


void
PhaseShiftTvi::visibilityCorrected (Cube<Complex> & vis) const
{
    // Have the upstream VII fill the cube with the appropriate data and
    // then phase shift the entire cube.

    getVii()->visibilityCorrected (vis);

    phaseCenterShift (vis);
}

void
PhaseShiftTvi::visibilityModel (Cube<Complex> & vis) const
{
    // Have the upstream VII fill the cube with the appropriate data and
    // then phase shift the entire cube.

    getVii()->visibilityModel (vis);

    phaseCenterShift (vis);
}

void
PhaseShiftTvi::visibilityObserved (Cube<Complex> & vis) const
{
    // Have the upstream VII fill the cube with the appropriate data and
    // then phase shift the entire cube.

    getVii()->visibilityObserved (vis);

    phaseCenterShift (vis);
}

} // end namespace vi

using namespace casacore;
} // end namespace casa



