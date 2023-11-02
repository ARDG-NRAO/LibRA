//# IlluminationConvFunc.cc: Implementation for IlluminationConvFunc
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

#define USETABLES 0
#include <synthesis/TransformMachines2/IlluminationConvFunc.h>

using namespace casacore;
namespace casa{
  namespace refim{
  //
  //------------------------------------------------------------------------
  //
  /*
  void IlluminationConvFunc::getValue(CF_TYPE *coord,
				      CF_TYPE *raoff1, CF_TYPE *raoff2, 
				      CF_TYPE *decoff1, CF_TYPE *decoff2,
				      CF_TYPE *area,
				      std::complex<CF_TYPE>& weight,
				      std::complex<CF_TYPE>& dweight1,
				      std::complex<CF_TYPE>& dweight2
				      )
  */
  CF_TYPE IlluminationConvFunc::getValue(Double *coord,
					 Double *raoff1, Double *raoff2, 
					 Double *decoff1, Double *decoff2,
					 Double *area,
					 Int *doGrad,
					 Complex& weight,
					 Complex& dweight1,
					 Complex& dweight2,
					 Double& /*currentCFPA*/
					 // ,Double lsigma
					 )
  {
    DComplex Kij;
    Double Rij,u,v,dra,ddec,s, MY_PI=M_PI;
    //    Double cpa,spa;
    //
    // If the PA of the convolution function changed, set up a new
    // rotation matrix
    //
    /*
    if (pa_p != currentCFPA)
      {
	pa_p = currentCFPA;
	cpa = cos(pa_p);
	spa = sin(pa_p);
      }
    */
    //
    // Compute the PA rotated (u,v)
    //
    //    u = cpa*coord[0] - spa*coord[1];
    //    v = spa*coord[0] + cpa*coord[1];
    u = coord[0];
    v = coord[1];
    s=4*sigma;
    dra=(*raoff1- *raoff2);
    ddec=(*decoff1 - *decoff2);
    Rij = (dra*dra + ddec*ddec)*s/2.0;
    //    Kij = Complex(0,M_PI*(u*(*raoff1+*raoff2) + v*(*decoff1+*decoff2)));
    Kij = Complex(0,MY_PI*(u*(*raoff1+*raoff2) + v*(*decoff1+*decoff2)));
    //    Eij0 = (u*u + v*v)*M_PI*M_PI/s;

    //    Rij = (dra*dra + ddec*ddec);
    Rij = 0;
#if(USETABLES)
    //
    // Compute it using lookup tables for exponentials.
    //
    Double tt=imag(Kij);

    //    weight = ExpTable((-Eij0-Rij))*CExpTable(tt)/ *area;
    weight = ExpTable((-Rij))*CExpTable(tt)/ *area;
#else
    //
    // Compute it exactly
    //
    //    weight = (exp((-Eij0-Rij)+Kij)/ *area);
    weight = (exp((-Rij)+Kij)/ *area);
    //    cout << exp(-Rij) << " " << exp(Kij) << " " << dra << " " << ddec << endl;
#endif

    if (doGrad)
      {
	//
	// Following 2 lines when Rij is not zero
	//
	// 	dweight1 = (Complex(0,M_PI*u)-Complex(dra*  s,0.0))*weight;
	// 	dweight2 = (Complex(0,M_PI*v)-Complex(ddec* s,0.0))*weight;
	//
//    	dweight1 = -Complex(2*ddec,0) +  (Complex(0,M_PI*u));//*weight;
//   	dweight2 = -Complex(2*dra,0) + (Complex(0,M_PI*v));//*weight;
        //
	// Following 2 lines when Rij=0
	//
	dweight1 = (Complex(0,MY_PI*u));//*weight;
	dweight2 = (Complex(0,MY_PI*v));//*weight;
      }
    //    cout << "Eij: " << weight << " " << dweight1 << " " << dweight2 << endl;
    return 1.0;
  }
  //
  //------------------------------------------------------------------------
  //
  CF_TYPE IlluminationConvFunc::area(Vector<Int>& convSupport, Vector<Double>& uvScale)
  {
    CF_TYPE u,v, area=0;

    for(Int i=-convSupport(0);i<convSupport(0);i++)
      {
	u = i*M_PI/uvScale(0);
	u *= u;
	for(Int j=-convSupport(0);j<convSupport(0);j++)
	  {
	    v = j*M_PI/uvScale(1);
	    v *= v;
#ifdef USETABLES	      
	    area += ExpTable(-(u+v)/(4*sigma));
#else
	    area += exp(-(u+v)/(4*sigma));
#endif	      
	  }
      }
    return area;
  };
  //
  //------------------------------------------------------------------------
  //
  Vector<Int> IlluminationConvFunc::supportSize(Vector<Double>& uvScale)
  {
    CF_TYPE u;
    Vector<Int> supportSize(uvScale.shape());

    for(Int i=0;i<100;i++)
      {
	u=i*M_PI/uvScale(0);
	u=u*u/(4*sigma);
	u=exp(-u);
	if (u < 1e-5) 
	  {
	    supportSize(0) = i;
	    supportSize(0)++; // Increment it by one since this is
	                      // used in FORTRAN indexing
	    break;
	  }
      }
    return supportSize;
  };
  };
};
