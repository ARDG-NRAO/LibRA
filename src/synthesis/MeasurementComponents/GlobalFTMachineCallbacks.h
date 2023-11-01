//# GlobalFTMachineCallbacks.h: Definition for GlobalFTMachineCallbacks
//# Copyright (C) 1996,1997,1998,1999,2000,2002
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

#ifndef SYNTHESIS_GLOBALFTMACHINECALLBACKS_H
#define SYNTHESIS_GLOBALFTMACHINECALLBACKS_H

#include <synthesis/TransformMachines/IlluminationConvFunc.h>

#define NEED_UNDERSCORES
namespace casa {
  extern "C" 
  {
    //
    // The Gridding Convolution casacore::Function (GCF) used by the underlying
    // gridder written in FORTRAN.
    //
    // The arguments must all be pointers and the value of the GCF at
    // the given (u,v) point is returned in the weight variable.  Making
    // this a function which returns a complex value (namely the weight)
    // has problems when called in FORTRAN - I (SB) don't understand
    // why.
    //
#if defined(NEED_UNDERSCORES)
#define gcppeij gcppeij_
#define globalGCFPtr globalgcfptr_
#endif
    typedef void (*GCFPtr)(casacore::Double *griduvw, casacore::Double *area,
			   casacore::Double *raoff1, casacore::Double *decoff1,
			   casacore::Double *raoff2, casacore::Double *decoff2, 
			   casacore::Int *doGrad,
			   casacore::Complex *weight,
			   casacore::Complex *dweight1,
			   casacore::Complex *dweight2,
			   casacore::Double *currentCFPA);
    
    extern void gcppeij(casacore::Double *griduvw, casacore::Double *area,
		   casacore::Double *raoff1, casacore::Double *decoff1,
		   casacore::Double *raoff2, casacore::Double *decoff2, 
		   casacore::Int *doGrad,
		   casacore::Complex *weight,
		   casacore::Complex *dweight1,
		   casacore::Complex *dweight2,
		   casacore::Double *currentCFPA);
    extern IlluminationConvFunc gwEij;
    //
    //---------------------------------------------------------------
    //
    //  IlluminationConvFunc gwEij;
    void gcppeij(casacore::Double *griduvw, casacore::Double *area,
		 casacore::Double *raoff1, casacore::Double *decoff1,
		 casacore::Double *raoff2, casacore::Double *decoff2, 
		 casacore::Int *doGrad,
		 casacore::Complex *weight,
		 casacore::Complex *dweight1,
		 casacore::Complex *dweight2,
		 casacore::Double *currentCFPA);

    /*
    void (*globalGCFPtr)(casacore::Double*griduvw, casacore::Double *area,
			 casacore::Double *raoff1, casacore::Double *decoff1,
			 casacore::Double *raoff2, casacore::Double *decoff2, 
			 casacore::Int *doGrad,
			 casacore::Complex *weight,
			 casacore::Complex *dweight1,
			 casacore::Complex *dweight2,
			 casacore::Double *currentCFPA) = NULL;
    */
  };
};

#endif
