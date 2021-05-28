//# Copyright (C) 1996,1997,1998,1999,2000,2001,2003
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

#ifndef IMAGEANALYSIS_CASACasaImageBeamSet
#define IMAGEANALYSIS_CASACasaImageBeamSet

#include <images/Images/ImageBeamSet.h>

namespace casa {

// <summary>
// Represents a set of restoring beams associated with an image. Implements getCommonBeam()
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
// </prerequisite>

// <etymology>
// A Set of Beams associated with an Image.
// </etymology>

// <synopsis>
// </synopsis>
//
// <example>

// </example>


// <motivation>
// Restoring beams are used many places in image analysis tasks.
// </motivation>

// <todo>
// </todo>

class CasaImageBeamSet : public casacore::ImageBeamSet {
public:

	// Construct an empty beam set.
	CasaImageBeamSet();

    // Construct a beam set from an 2-D array of beams representing
    // the frequency and stokes axis.
    // Axis length 1 means it is valid for all channels cq. stokes.
	// If the image has 0 spectral channels or stokes, the corresponding
	// length of the axis in the provided matrix should be 1.
	CasaImageBeamSet(
		const casacore::Matrix<casacore::GaussianBeam>& beams
	);

	// construct an CasaImageBeamSet representing a single beam which is valid for
	// all channels and stokes
	CasaImageBeamSet(const casacore::GaussianBeam& beam);

    // Create an CasaImageBeamSet of the specified shape with all
    // GaussianBeams initialized to <src>beam</src>.
    CasaImageBeamSet(casacore::uInt nchan, casacore::uInt nstokes, const casacore::GaussianBeam& beam=casacore::GaussianBeam::NULL_BEAM);

    // The copy constructor (reference semantics).
    CasaImageBeamSet(const CasaImageBeamSet& other);
    CasaImageBeamSet(const casacore::ImageBeamSet& other);

	~CasaImageBeamSet();

    // Assignment can change the shape (copy semantics).
	CasaImageBeamSet& operator=(const CasaImageBeamSet& other);

    static const casacore::String& className();

	// Get a beam to which all other beams in the set can be convolved.
	// If all other beams can be convolved to the maximum area beam in the set, that beam will be returned.
	// If not, this is guaranteed to be the minimum area beam to which
	// all beams in the set can be convolved if all but one of the beams in the set can be convolved to the beam in the set with the
	// largest area. Otherwise, the returned beam may or may not be the smallest possible beam to which all the beams in the set
	// can be convolved.
	casacore::GaussianBeam getCommonBeam() const;

private:

	static void _transformEllipseByScaling(
		casacore::Double& transformedMajor, casacore::Double& transformedMinor,
		casacore::Double& transformedPa, casacore::Double major, casacore::Double minor,
		casacore::Double pa, casacore::Double xScaleFactor, casacore::Double yScaleFactor
	);
};

std::ostream &operator<<(std::ostream &os, const CasaImageBeamSet& beamSet);

}

#endif

