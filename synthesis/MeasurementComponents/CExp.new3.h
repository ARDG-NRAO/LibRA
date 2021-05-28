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
#if !defined(CEXP3_H)
#define CEXP3_H

#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <sstream>
#include <complex>
#include <vector>
#include <ctime>

namespace casa{
# define PI2 6.28318530717958623
# define CE_TYPE float

using namespace std;

template <class T> class CExp3
{
public:
  CExp3() { Size = 0; ITable=RTable=NULL; };
  CExp3(int n) { Size = n; build(Size); };
  ~CExp3(){if (ITable) {free(ITable);free(RTable);}}
  inline void build(int n)
  {
    Size = n;
    //    ITable.resize(Size); RTable.resize(Size);
    ITable = (T*)malloc(sizeof(T)*Size);
    RTable = (T*)malloc(sizeof(T)*Size);
    Step = PI2/Size;
    for (int i=0; i<Size; i++) {
      ITable[i] = sin(i*Step);
      RTable[i] = cos(i*Step);
    }
  }
  inline int f(register T arg)
  {
    return (int)((arg<0)?((arg+1-(int)arg)*Size):((arg-(int)arg)*Size));
    //    if (arg < 0) return (int)((arg+1-(int)arg)*Size); return (int)((arg-(int)arg)*Size);
  }

  inline int hashFunction(T arg)
  {
    return f(arg/PI2);
    // return (int)(fmodf(fabsf(arg+PI2),PI2)/Step);
  }

  inline std::complex<T> operator()(T& arg)
  {
    int N=hashFunction(arg);
    return std::complex<T>(RTable[N],ITable[N]);
  }

  inline T imag(T arg) { return ITable[hashFunction(arg)]; }
  inline T real(T arg) { return RTable[hashFunction(arg)]; }
  inline void reim(T& arg,T& re, T&im)
  { int N = hashFunction(arg);
    re = RTable[N]; im = ITable[N];
  }
private:
  //  vector<T> RTable, ITable;
  T *RTable, *ITable;
  T Step;
  int Size;
};
};  
#endif
