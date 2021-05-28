//# SimACohCalc.cc: Simulated additive errors
//# Copyright (C) 1996,1997,1999,2000,2001
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

#include <synthesis/MeasurementComponents/SimACohCalc.h>
#include <msvis/MSVis/VisBuffer.h>
#include <ms/MeasurementSets/MSColumns.h>
#include <ms/MeasurementSets/MSAntennaColumns.h>
#include <casa/Logging/LogIO.h>
#include <ms/MeasurementSets/MSSpWindowColumns.h>
#include <measures/Measures.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

// Note: this simplistic implementation just generates a new random
// noise value for every call of apply, it doesn't keep track of
// time and antenna to return the same value if called with the same
// coordinates. Thus applyInv will only work correctly if
// called from a separate object in the exact same sequence as apply.

SimACohCalc::SimACohCalc(const Int seed, 
			 const Float antefficiency,
			 const Float correfficiency,
			 const Float spillefficiency,
			 const Float tau, 
			 const Quantity& trx,
			 const Quantity& tatmos,
			 const Quantity& tcmb) : 
  rndGen_p(seed),
  // noiseDist_p(&rndGen_p, 0.0, 0.5),
  noiseDist_p(&rndGen_p, 0.0, 1.0),
  antefficiency_p(antefficiency),
  correfficiency_p(correfficiency),
  spillefficiency_p(spillefficiency),
  tau_p(tau),
  trx_p(trx),
  tatmos_p(tatmos),
  tcmb_p(tcmb)
{
}


SimACohCalc::~SimACohCalc()
{};


VisBuffer& SimACohCalc::apply(VisBuffer& vb)
{
    Complex c[4];

    //    Double averageNoise = 0.0;
    //    Int nAverage = 0;
    //    Double averageT = 0.0;
    //    Int nAverage = 0;

    // In case you are confused, the 1e-4 converts the Diam from cm to m
    Double fact = 4 * C::sqrt2 * 1.38062e-16 * 1e23 * 1e-4 /
      ( antefficiency_p * correfficiency_p * C::pi );
    
    Double lasttime = 0.0;
    Double time = 0.0;
    Vector<MDirection> azel;
    Int ant1, ant2;
    Double el1=0.0, el2=0.0, airmass1=0.0, airmass2=0.0;
    Double tsys=0.0, sigma = 1.0;
    Double diam1, diam2;
    Float deltaNu;
    Double tint;
    Vector<Double> antDiams;

    Double trx_k = trx_p.getValue("K");
    Double tatmos_k = tatmos_p.getValue("K");
    Double tcmb_k = tcmb_p.getValue("K");
    Bool zeroSpacing = false;

    LogIO os(LogOrigin("SimACohCalc", "apply()", WHERE));

    {
      Int iSpW=vb.spectralWindow();
      deltaNu = vb.msColumns().spectralWindow().totalBandwidth()(iSpW) / 
	Float(vb.msColumns().spectralWindow().numChan()(iSpW));

      // live dangerously: assume all vis have the same tint
      tint = vb.msColumns().exposure()(0);  

      antDiams = vb.msColumns().antenna().dishDiameter().getColumn();
    }

    for (Int row=0; row<vb.nRow(); row++) {

      ant1=vb.antenna1()(row);
      ant2=vb.antenna2()(row);
      diam1 = antDiams(ant1);
      diam2 = antDiams(ant2);

      time = vb.time()(row);
      if (time != lasttime) {
	azel = vb.azel(time);
      }
      el1 = azel(ant1).getAngle("rad").getValue()(1);
      el2 = azel(ant2).getAngle("rad").getValue()(1);

      airmass1 = 1.0;  airmass2 = 1.0;
      if ( (el1 > 0.0 && el2 > 0.0) || tau_p == 0.0) {
	airmass1 = 1.0/ sin(el1);
	airmass2 = 1.0/ sin(el2);
	
	tsys = trx_k * sqrt(exp(tau_p * airmass1)) * sqrt(exp(tau_p * airmass2)) 
	  + tatmos_k * 
	  (sqrt(exp(tau_p * airmass1))*sqrt(exp(tau_p * airmass2)) - spillefficiency_p)
	  + tcmb_k;
      
	sigma = fact * tsys / diam1 / diam2 / sqrt( deltaNu * tint );

	vb.sigma()(row) = sigma;
	sigma = max( sigma, 1e-9 );
	vb.weight()(row) *= 1.0/ pow(sigma, 2.0);

	
	zeroSpacing = false;
	if (vb.uvw()(row)(0) == 0.0 && vb.uvw()(row)(1) == 0.0) {
	  zeroSpacing = true;
	}

	for (Int chn=0; chn<vb.nChannel(); chn++) {
	  for (Int i=0; i<4; i++) { 	    
	    if (zeroSpacing) {
              Float re = 1.41421356*noiseDist_p();
	      c[i]= Float(sigma) * Complex(re, 0.0);
	    } else {
	      // huh.  why would one use a Bessel function (product of normals) here?
              // Float re = noiseDist_p()*noiseDist_p();	      
	      // c[i]= Float(sigma) * Complex(re);
	      c[i]= Float(sigma) * Complex(noiseDist_p(),noiseDist_p());
	    }
	    //	    nAverage++;  averageNoise += sigma;
	  }

	  CStokesVector noiseCoh(c);
	  vb.visibility()(chn,row)+=noiseCoh;
	}
	//	nAverage++;  averageT += tsys;
      }
    }
//     if (nAverage > 0) {
//       averageNoise /= Float(nAverage);
//       os << "Average noise added to visibilities: " << averageNoise << "  Jy" << LogIO::POST;
//     }
//    if (nAverage > 0) {
//      averageT /= Float(nAverage);
//      os << "Noise added to visibilities with average Tsys=" << averageT << " K" << LogIO::POST;
//    }
    return vb;
};


VisBuffer& SimACohCalc::applyInv(VisBuffer& vb)
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

