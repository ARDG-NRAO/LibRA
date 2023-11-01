//# CExp.cc: Implementation of CExp (tabulated complex exponential) class
//# Copyright (C) 1997,1998,1999,2000,2001,2002,2003
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
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$
#if !defined(CEXP_H)
#define CEXP_H

#include <casacore/casa/aips.h>
#include <stdlib.h>
#include <math.h>
#include <casacore/casa/Exceptions/Error.h>
#include <iostream>
#include <casacore/casa/Arrays/Vector.h>

namespace casa{

  //#define HASH(A) {(int)(std::fmod(abs((A)+PI2),PI2)/Step)}
#define PI2 6.28318530717958623

#define HASH(A) {(int)(myhash((A)/PI2)/Step)}
#define MYHASH(A)   {((int)((((A)<0)?(((A)+1-(int)(A))*PI2):(((A)-(int)(A))*PI2))/Step))}

inline double myhash(register double arg)
{
  if (arg < 0) return (arg+1-(int)arg)*PI2;
  return (arg-(int)arg)*PI2;
}

// Call this inline as
//  f(arg/PI2)


template <class T> class CExp
{
public:
  CExp<T>() {Size=0;};
  CExp<T>(int n) {Size=n;build(Size);};
  inline void build(int n)
  {
    if (n!=Size)
      {
	Size = n;
	ITable.resize(Size);RTable.resize(Size);

	Step=PI2/Size;
	for (casacore::Int i=0;i<Size;i++) 
	  {
	    ITable(i)=sin(i*Step);
	    RTable(i)=cos(i*Step);
	  }
      }
  }

  inline double f(register T arg)
  {
    if (arg < 0) return (arg+1-(int)arg)*PI2;
    return (arg-(int)arg)*PI2;
  }

  inline int hashFunction(T arg)   {return (int)(std::fmod(abs(arg+PI2),PI2)/Step);}
//{return (int)(myhash(arg/PI2)/Step);}

  inline int myhash2(register double arg)
  { /* Steps must be double here, otherwise compiler will keep converting
       from int to double everytime, because Steps is a variable.
    */
    if (arg < 0) return (int)((arg+1-(int)arg)*Size);
    return (int)((arg-(int)arg)*Size);
  }

#define MYHASH2(A) {A<0     ? (int)(A+1-(int)A)*Size:(int)(A-(int)A)*Size}
#define MYHASH3(a) ((a < 0) ? ((a)-(int)(a)+1)*PI2 : ((a)-(int)(a))*PI2)

  inline std::complex<T> operator()(T& arg)
  {
    //    int N=hashFunction(arg);
    int N=(int)(std::fmod(abs(arg+PI2),PI2)/Step); //Best
    //    int N=HASH(arg);
    //    T t=arg/PI2;int N=MYHASH2(t);
    //    T t=arg/PI2;int N=(int)MYHASH3(t)/Step;
    //    int N=myhash2(arg/PI2);
    return std::complex<T>(RTable[N],ITable[N]);
  }

  inline T imag(T arg) {return ITable[hashFunction(arg)];}
  inline T real(T arg) {return RTable[hashFunction(arg)];}
  inline void reim(T& arg,T& re, T&im) {casacore::Int N=HASH(arg);re=RTable[N];im=ITable[N];}

private:
  casacore::Vector<T> RTable, ITable;
  T Step;
  int Size;
};
};  
#endif
