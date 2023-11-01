#include <synthesis/MeasurementComponents/GlobalFTMachineCallbacks.h>
#include <synthesis/TransformMachines/IlluminationConvFunc.h>

using namespace casacore;
using namespace casa;

extern "C" 
{
  //
  // The Gridding Convolution Function (GCF) used by the underlying
  // gridder written in FORTRAN.
  //
  // The arguments must all be pointers and the value of the GCF at
  // the given (u,v) point is returned in the weight variable.  Making
  // this a function which returns a complex value (namely the weight)
  // has problems when called in FORTRAN - I (SB) don't understand
  // why.
  //
  //---------------------------------------------------------------
  //
  IlluminationConvFunc casa::gwEij;
  void gcppeij(Double *griduvw, Double *area,
	       Double *raoff1, Double *decoff1,
	       Double *raoff2, Double *decoff2, 
	       Int *doGrad,
	       Complex *weight,
	       Complex *dweight1,
	       Complex *dweight2,
	       Double *currentCFPA)
  {
    Complex w,d1,d2;
    gwEij.getValue(griduvw, raoff1, raoff2, decoff1, decoff2,
		   area,doGrad,w,d1,d2,*currentCFPA);
    *weight   = w;
    *dweight1 = d1;
    *dweight2 = d2;
  }
};
