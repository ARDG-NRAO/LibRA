//# BeamSquint.cc: Implementation for BeamSquint
//# Copyright (C) 1996,1997,1998,1999,2000,2003
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

#include <casacore/casa/aips.h>
#include <casacore/casa/Logging/LogIO.h>
#include <synthesis/TransformMachines/BeamSquint.h>



using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

BeamSquint::BeamSquint()
{
  squint_p = MDirection( Quantity(0.0, "deg"), Quantity(0.0, "deg"),
			 MDirection::Ref(MDirection::AZEL));
  refFreq_p = 0.0;
};


BeamSquint::BeamSquint(const MDirection& squint, const Quantity& refFreq) :
squint_p(squint),
refFreq_p(refFreq)
{
};

BeamSquint::BeamSquint(const BeamSquint& other)
{
  squint_p = other.squint_p;
  refFreq_p = other.refFreq_p;
};


BeamSquint& BeamSquint::operator=(const BeamSquint& other)
{
  if (this == &other) 
    return *this;

  squint_p = other.squint_p;
  refFreq_p = other.refFreq_p;
  return *this;
};

BeamSquint::~BeamSquint()
{
};

void BeamSquint::show()
{
    LogIO os(LogOrigin("BeamSquint", "show"));
    os << "Beam Squint in arcmin (AZ-EL): "
       << squint_p.getAngle().getValue("'") << LogIO::POST;
    os << "Beam Squint ref freq, GHz    : "
       << refFreq_p.getValue("GHz") << LogIO::POST;
};

Bool BeamSquint::isNonNull()
{
  Double x = 0.0; x = squint_p.getAngle().getValue("'")(0);
  Double y = 0.0; y = squint_p.getAngle().getValue("'")(1);
  if (abs(x) > 1e-9 || abs(y) > 1e-9) {
    return true;
  } else {
    return false;
  }
};

MDirection& BeamSquint::scale(const Quantity& newFreq)
{
  Float fScale = 1.0;
  fScale = refFreq_p.getValue("GHz") / newFreq.getValue("GHz");
  Quantity q0 = Quantity(fScale*(squint_p.getAngle().getValue("'")(0)), "'");
  Quantity q1 = Quantity(fScale*(squint_p.getAngle().getValue("'")(1)), "'");
  MDirection * mdp = new MDirection( q0, q1, 
				     MDirection::Ref(MDirection::AZEL));
  return *mdp;
};


void
BeamSquint::getPointingDirection (const MDirection& pointDir,
				  const Quantity parAngle,
				  const Quantity obsFreq,
				  const SquintType doSquint,
				  MDirection& newPointing)
{
  //  If there is a problem with the squint, look here first!
  //  Our convention: for a source which transits south of zenith,
  //  PA = 0 at its transit, and increases with time.  We need to
  //  rotate the beam by -PA on the sky


  newPointing = MDirection(pointDir);

  //  cout << "BS1:  pre-original pointing: " << pointDir.getAngle().getValue("'") << endl;

  if (doSquint != NONE) {

    // The sign convention here was incorrect, as determined on
    // VLA L-band observations of IC2233. I flipped it to obtain
    // the best V image after correction. - TJC 2003/06/18
    Float fScale =  -refFreq_p.getValue("GHz") / obsFreq.getValue("GHz");

    Double sx = fScale*(squint_p.getAngle().getValue()(0));
    Double sy = fScale*(squint_p.getAngle().getValue()(1));  

    String RRorLL = "RR";

    if (doSquint == LL) {
      RRorLL = "LL";
      sx = -sx;
      sy = -sy;
    }
    
    // rotate by -ParAngle
    Double pa = parAngle.getValue("rad");

    Double dx =  sx * cos(-pa) + sy * sin(-pa);
    Double dy = -sx * sin(-pa) + sy * cos(-pa);
    // but this is in an AZ-EL type frame; flip it for combining with RA/DEC
    dx = -dx;

    newPointing.shift(dx, dy, true);
    //    cout << "BeamSquint: " << RRorLL << "  sx: " << sx << " sy: " << sy 
    //    << " pa: " << pa << " dx: " << dx << " dy: " << dy << endl;
  }    


  //  cout << "BS2:           new pointing: " << newPointing.getAngle().getValue("'") << endl;
  //  cout << "BS3: post-original pointing: " << pointDir.getAngle().getValue("'") << endl;

    
};

} //# NAMESPACE CASA - END

