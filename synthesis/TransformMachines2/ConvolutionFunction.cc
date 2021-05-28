// -*- C++ -*-
//# AWProjectFT.cc: Implementation of AWProjectFT class
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
#include <synthesis/TransformMachines2/ConvolutionFunction.h>
using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN
  using namespace refim;
  ConvolutionFunction::~ConvolutionFunction() {};
  Matrix<Int> ConvolutionFunction::makeBaselineList(const Vector<Int>& antList)
  {
    Int na=antList.nelements();
    Int nb=max(na,na*(na-1));
    Matrix<Int> baselineList(nb,2);
    nb=0;
    for(Int ia=0;ia<na;ia++)
      for (int ja=ia;ja<na;ja++)
	{
	  baselineList(nb,0)=antList[ia];
	  baselineList(nb,1)=antList[ja];
	  nb++;
	}
    return baselineList;
  }
};
