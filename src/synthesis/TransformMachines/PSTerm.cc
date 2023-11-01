//# PSTerm.cc: implementation of PSTerm
//# Copyright (C) 2007
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$
#include <synthesis/TransformMachines/PSTerm.h>
#include <synthesis/TransformMachines/Utils.h>
#include <synthesis/TransformMachines/SynthesisError.h>
#ifdef _OPENMP
#include <omp.h>
#endif

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN
  //
  //-------------------------------------------------------------------------
  //	       
  void PSTerm::init(const IPosition shape, 
		    const Vector<Double>& uvScale,
		    const Vector<Double>& uvOffset,
		    const Double& psScale)
  {
    psCtor_p   = new ConvolveGridder<Double, Complex>(shape, uvScale, uvOffset, "SF");
    sampling_p = (*psCtor_p).cSupport();
    support_p  = (*psCtor_p).cSampling();
    psScale_p=psScale;
  }
  //
  //-------------------------------------------------------------------------
  //	       
  Matrix<Complex>& PSTerm::operator=(Matrix<Complex>& buf)
  {
    applySky(buf);
    return buf;
  }
  //
  //-------------------------------------------------------------------------
  //	       
  Matrix<Complex>& PSTerm::operator*=(Matrix<Complex>& buf)
  {
    applySky(buf,true);
    return buf;
  }
  //
  //-------------------------------------------------------------------------
  //	       
  void PSTerm::applySky(Matrix<Complex>& screen, Bool multiply)
  {
    //    AlwaysAssert(psCtor_p.null()==false, SynthesisFTMachineError);
    Int nx=screen.shape()(0), ny=screen.shape()(1);
    Int convOrig=nx/2;
    Float xpart, psScale_local=psScale_p;
#ifdef _OPENMP
    Int Nth=max(omp_get_max_threads()-2,1);
#endif

    if (!isNoOp())
      {
	for (Int i=0; i<nx;i++)
	  {
	    xpart = square(i-convOrig);
#ifdef _OPENMP
//#pragma omp parallel default(none) firstprivate(xpart,convOrig, i) shared(screen,psScale_local,ny,multiply) num_threads(Nth)
#pragma omp parallel firstprivate(xpart,convOrig, i) shared(psScale_local,ny,multiply) num_threads(Nth)
#endif
   {
#ifdef _OPENMP
#pragma omp for
#endif
	    for (Int j=0;j<ny;j++)
	      {
		Float ypart;
		ypart = sqrt(xpart + square(j-convOrig))*psScale_local;
		Float val = SynthesisUtils::libreSpheroidal(ypart)*(1.0-ypart*ypart);;
		if (val > 0)
		  {
		    if (multiply)  screen(i, j) *= val;
		    else           screen(i, j)  = val;
		  }
	      }
    }
	  }
      }
  }

  void PSTerm::applySky(Matrix<Complex>& screen, 
			const Vector<Double>& sampling,
			const Int inner)
  {
    //GG: convsize: 2048 inner: 512 uvscale: [-0.0397159, 0.0397159, 2.33547e-310] uvoffset:[1024, 1024, 0]
    // Vector<Double> uvScale(3);uvScale[0]=-0.0397159;uvScale[1]=0.0397159;uvScale[2]=0.0;
    // Vector<Double> uvOffset(3,0.0); uvOffset[0]=uvOffset[1]=1024;
    // psCtor_p=new ConvolveGridder<Double, Complex>(IPosition(2, inner, inner),
    // 						  uvScale, uvOffset,
    // 						  "SF");


    AlwaysAssert(psCtor_p.null()==false, SynthesisFTMachineError);

    Int convSize = screen.shape()[0];
    //    Vector<Complex> correction(inner);
    Vector<Complex> correction;
    if (!isNoOp())
      {
	for (Int iy=-inner/2;iy<inner/2;iy++) 
	  {
	    psCtor_p->correctX1D(correction, iy+inner/2);
	    Int k=0;
	    for (Int ix=-inner/2;ix<inner/2;ix++) 
	      {
		screen(ix+convSize/2,iy+convSize/2)=correction(k++);//correction(ix+inner/2);
	      }
	  }
      }
    (void)sampling; 
  }
  //
  //-------------------------------------------------------------------------
  //	       
  void PSTerm::normalizeImage(Lattice<Complex>& skyImage,
			      const Matrix<Float>& weights)
  {
    Int inx = skyImage.shape()(0);
    //    Int iny = skyImage.shape()(1);
    Vector<Complex> correction(inx);
    correction=Complex(1.0, 0.0);
    IPosition cursorShape(4, inx, 1, 1, 1);
    IPosition axisPath(4, 0, 1, 2, 3);
    LatticeStepper lsx(skyImage.shape(), cursorShape, axisPath);
    LatticeIterator<Complex> lix(skyImage, lsx);

    for(lix.reset();!lix.atEnd();lix++) 
      {
	Int pol=lix.position()(2), chan=lix.position()(3);

	if(weights(pol, chan)!=0.0) 
	  {
	    Int iy=lix.position()(1);
	    psCtor_p->correctX1D(correction,iy);
	    // for (Int ix=0;ix<inx;ix++) 
	    //   correction(ix)*=sincConv(ix)*sincConv(iy);

	    lix.rwVectorCursor()/=correction;
	  }
	else 
	  lix.woCursor()=0.0;
      }
  }
};
