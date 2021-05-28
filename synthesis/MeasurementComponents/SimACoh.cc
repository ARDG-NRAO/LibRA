//# SimACoh.cc: Simulated additive errors
//# Copyright (C) 1996,1997,1999,2000
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

#include <casa/BasicMath/Math.h>
#include <synthesis/MeasurementComponents/SimACoh.h>
#include <msvis/MSVis/VisBuffer.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

// Note: this simplistic implementation just generates a new random
// noise value for every call of apply, it doesn't keep track of
// time and antenna to return the same value if called with the same
// coordinates. Thus applyInv will only work correctly if
// called from a separate object in the exact same sequence as apply.

SimACoh::SimACoh(Int seed, Double rms):rndGen_p(seed),
				       //noiseDist_p(&rndGen_p, 0.0, square(rms/2.0))
noiseDist_p(&rndGen_p, 0.0, square(rms))
{
}
VisBuffer& SimACoh::apply(VisBuffer& vb)
{
  Bool zeroSpacing = false;
  Complex c[4];
  for (Int row=0; row<vb.nRow(); row++) {
    zeroSpacing = false;
    if (vb.uvw()(row)(0) == 0.0 && vb.uvw()(row)(1) == 0.0) {
      zeroSpacing = true;
    }
    for (Int chn=0; chn<vb.nChannel(); chn++) {
      if (zeroSpacing) {
	for (Int i=0; i<4; i++) c[i]=Complex(1.41421356*noiseDist_p(), 0.0);
      } else {
	for (Int i=0; i<4; i++) c[i]=Complex(noiseDist_p(),noiseDist_p());
      }
      CStokesVector noiseCoh(c);
      
      vb.visibility()(chn,row)+=noiseCoh;
      
    }
  }
  return vb;
}

VisBuffer& SimACoh::applyInv(VisBuffer& vb)
{
    Complex c[4];
    for (Int row=0; row<vb.nRow(); row++) {
      for (Int chn=0; chn<vb.nChannel(); chn++) {
	for (Int i=0; i<4; i++) c[i]=Complex(noiseDist_p(),noiseDist_p());
	CStokesVector noiseCoh(c);
	vb.visibility()(chn,row)-=noiseCoh;
      }
    }
    return vb;
}

} //# NAMESPACE CASA - END

