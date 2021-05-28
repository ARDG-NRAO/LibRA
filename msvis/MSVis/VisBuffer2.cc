/*
 * VisBuffer2.cc
 *
 *  Created on: Aug 21, 2012
 *      Author: jjacobs
 */


#include <msvis/MSVis/UtilJ.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <msvis/MSVis/VisBufferImpl2.h>
#include <msvis/MSVis/ViImplementation2.h>
#include <msvis/MSVis/VisibilityIterator2.h>

using namespace casacore;
namespace casa {

namespace vi {

VisBuffer2 *
VisBuffer2::factory(VisBufferOptions options)
{
	return new VisBufferImpl2(options);
}

void
VisBuffer2::associateWithVi2 (const VisibilityIterator2 * vi)
{
    associatedVi_p = vi;
}


ms::MsRow *
VisBuffer2::getRow (Int) const
{
    ThrowIf (true, "Not implemented by this subclass.");

    return 0;
}

ms::MsRow *
VisBuffer2::getRowMutable (Int)
{
    ThrowIf (true, "Not implemented by this subclass.");

    return 0;
}

const VisibilityIterator2 *
VisBuffer2::getVi () const
{
    return associatedVi_p;
}

// Rotate visibility phase for phase center offsets
void VisBufferUtil2::phaseCenterShift (VisBuffer2 & /*vb*/, const Vector<Double>& /*phase*/)
{

  // AlwaysAssert(static_cast<Int>(phase.nelements()) == vb.nRows(), AipsError);

  // // phase is in metres
  // // phase*(-2*pi*f/c) gives phase for the channel of the given baseline in radian
  // //   sign convention will _correct_ data

  // Vector<Double> freq(frequency());
  // Double ph, udx;
  // Complex cph;

  // for (Int row = 0; row < nRow(); ++row){

  //   udx = phase(row) * -2.0 * C::pi/C::c; // in radian/Hz

  //   for (Int channel = 0; channel < nChannels(); ++channel) {
  //     // Calculate the Complex factor for this row and channel
  //     ph = udx * freq(channel);

  //     if(ph!=0.){
  // 	cph = Complex(cos(ph), sin(ph));
  // 	// Shift each correlation:
  // 	for (Int icor = 0; icor < nCorr(); ++icor) {
  // 	  if (visCubeOK_p) {
  // 	    visCube_p(icor, channel, row) *= cph;
  // 	  }
  // 	  if (modelVisCubeOK_p) {
  // 	    modelVisCube_p(icor, channel, row) *= cph;
  // 	  }
  // 	  if (correctedVisCubeOK_p) {
  // 	    correctedVisCube_p(icor, channel, row) *= cph;
  // 	  }
  // 	  // Of course floatDataCube does not have a phase to rotate.
  // 	}
  //     }

  //   }
  // }

}

// Rotate visibility phase for phase center offsets
void
VisBufferUtil2::phaseCenterShift (VisBuffer2 & /*vb*/, Double /*dx*/, Double /*dy*/)
{

  // // no-op if no net shift
  // if (!(abs(dx) > 0 || abs(dy) > 0)) {
  //   return;
  // }

  // // Offsets in radians (input is arcsec)
  // dx *= (C::pi / 180.0 / 3600.0);
  // dy *= (C::pi / 180.0 / 3600.0);

  // // Extra path as fraction of U and V
  // Vector<Double> udx;
  // udx = vb.uvw ().row(0);
  // Vector<Double> vdy;
  // vdy = vb.uvw ().row(1);
  // udx *= dx;  // in m
  // vdy *= dy;

  // // Combine axes
  // udx += vdy;

  // phaseCenterShift (vb, udx);

}



} // end namespace vi

using namespace casacore;
} // end namespace casa


