//# BeamSquint.h: Defines the BeamSquint class
//# Copyright (C) 1996,1997,1998,1999,2000
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

#ifndef SYNTHESIS_BEAMSQUINT_H
#define SYNTHESIS_BEAMSQUINT_H

#include <casa/aips.h>
#include <measures/Measures.h>
#include <coordinates/Coordinates.h>
#include <measures/Measures/MDirection.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> encapsulates beam squint (RR and LL beams at different directions) </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <li> <linkto class="casacore::MDirection">casacore::MDirection</linkto> class
// </prerequisite>
//
// <etymology>
// BeamSquint = Beam Squint.  Obviously so.
// </etymology>
//
// <synopsis> 
// Beam squint is when the RR and LL circular polarization beams
// are located in different places on the sky.  The effect is due
// to the RR and LL feeds being off-axis.  The VLA has particularly bad
// beam squint.  Some instruments like the AT or WSRT have essentially
// no beam squint.  The nominal pointing position is taken to be
// right in between the RR and LL beam centers/
//
// BeamSquint has an casacore::MDirection which represents the angular offset from the nominal
// pointing position at Parallactic angle = 0 (ie, observing a source south of zenith
// at transit) and the actual RR beam.  BeamSquint also has a reference frequency,
// and the magnitude of the squint angular offset is inversely proportional with
// frequency.  The squint offset is a true angular displacement.
//
// The main thing that BeamSquint does is: given the nominal
// pointing position and the parallactic angle, calculate the
// actual squinted beam position for the RR or LL beams.
// If BeamSquint::NONE is requested, the nominal pointing position
// is returned.
// </synopsis> 
//
//
// <example>
// <srcblock>
//    casacore::MDirection oldPointing;
//    casacore::MDirection newPointing;
//    casacore::Quantity parallacticAngle(C::pi/2, "rad");
//    casacore::Quantity observingFreq(1.4142, "GHz");
//    BeamSquint oneBS( casacore::MDirection( casacore::Quantity(1.0, "'"), casacore::Quantity(1.0, "'"),
//                                  casacore::MDirection::Ref(casacore::MDirection::AZEL)),
//                      casacore::Quantity(1.0, "GHz") );
//    oneBS.getPointingDirection(oldPointing, parallacticAngle,
//                               observingFreq, BeamSquint::RR, newPointing);
//
// </srcblock>
// </example>
//
// <motivation>
// Boy, is this UGLY.  We do that UGLY stuff just once, though,
// so you never have to look at it AGAIN (see getPointingDirection
// if you still don't believe me).
// </motivation>
//
// <todo asof="98/10/21">
// <li> Hmmm?
// </todo>

 
class BeamSquint {
public:

  // Allowed Squints: NONE = no squint: PB is centered on the pointing center
  // RR = PB is shifted from pointing center by amount in PBMathInterface's squint_p
  // LL = PB is shifted from pointing center by the NEGATIVE of the
  //             amount in PBMathInterface's squint_p
  // GOFIGURE = do the appropriate thing based on the STOKES of the Image
  enum SquintType{NONE, RR, LL, GOFIGURE};

  // Default constructor initializes to zero
  BeamSquint();

  //Smart constructor to initialize the casacore::MDirection and reference freq
  BeamSquint(const casacore::MDirection& squint, const casacore::Quantity& refFreq);

  //Copy constructor
  BeamSquint(const BeamSquint& other);

  //Operator=
  BeamSquint& operator= (const BeamSquint& other);

  //Destructor
  ~BeamSquint();

  //Show to Logger
  void show();

  //Is BeamSquint nonNull?
  casacore::Bool isNonNull();

  //Return the squint's casacore::MDirection scaled to a particular frequency
  casacore::MDirection& scale(const casacore::Quantity& refFreq);

  //Return the squinted pointing position
  void getPointingDirection (const casacore::MDirection& pointDir,
			     const casacore::Quantity parAngle,
			     const casacore::Quantity obsFreq,
			     const SquintType doSquint,
			     casacore::MDirection& newPointingDir);

protected:

  casacore::MDirection squint_p;

  casacore::Quantity refFreq_p;

};
  

} //# NAMESPACE CASA - END

#endif
