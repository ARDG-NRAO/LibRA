//# tSubImage.cc: Test program for class SubImage
//# Copyright (C) 1998,1999,2000,2001,2003
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or (at your option)
//# any later version.
//#
//# This program is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//# more details.
//#
//# You should have received a copy of the GNU General Public License along
//# with this program; if not, write to the Free Software Foundation, Inc.,
//# 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id: tSubImage.cc 20567 2009-04-09 23:12:39Z gervandiepen $

#ifndef IMAGEANALYSIS_IMAGEPRIMARYBEAMCORRECTOR_H
#define IMAGEANALYSIS_IMAGEPRIMARYBEAMCORRECTOR_H

#include <imageanalysis/ImageAnalysis/ImageTask.h>
#include <memory>

#include <casacore/casa/namespace.h>

namespace casa {

class ImagePrimaryBeamCorrector : public ImageTask<casacore::Float> {
	// <summary>
	// Top level interface for primary beam correction.
	// </summary>

	// <reviewed reviewer="" date="" tests="" demos="">
	// </reviewed>

	// <prerequisite>
	// </prerequisite>

	// <etymology>
	// Collapses image.
	// </etymology>

	// <synopsis>
	// High level interface for primary beam correction.
	// </synopsis>

	// <example>
	// <srcblock>
	// ImagePrimaryBeamCorrector corrector(...);
	// corrector.correct();
	// </srcblock>
	// </example>

public:

	enum Mode {
		MULTIPLY,
		DIVIDE
	};

	ImagePrimaryBeamCorrector() = delete;

	// if <src>outname</src> is empty, no image will be written
	// if <src>overwrite</src> is true, if image already exists it will be removed
	// if <src>overwrite</src> is false, if image already exists exception will be thrown
	// Only one of <src>regionPtr</src> or <src>region<src> should be specified.
	// <group>

	ImagePrimaryBeamCorrector(
		const SPCIIF image,
		const SPCIIF pbImage,
		const casacore::Record * const &regionPtr,
		const casacore::String& region, const casacore::String& box,
		const casacore::String& chanInp, const casacore::String& stokes,
		const casacore::String& maskInp,
		const casacore::String& outname, const casacore::Bool overwrite,
		const casacore::Float cutoff, const casacore::Bool useCutoff, const Mode mode
	);

	ImagePrimaryBeamCorrector(
		const SPCIIF image,
		const casacore::Array<casacore::Float>& pbArray,
		const casacore::Record * const &regionPtr,
		const casacore::String& region, const casacore::String& box,
		const casacore::String& chanInp, const casacore::String& stokes,
		const casacore::String& maskInp,
		const casacore::String& outname, const casacore::Bool overwrite,
		const casacore::Float cutoff, const casacore::Bool useCutoff, const Mode mode
	);

	// </group>

	// destructor
	~ImagePrimaryBeamCorrector();

	// perform the correction. If <src>wantReturn</src> is true, return a pointer to the
	// collapsed image.
	SPIIF correct(casacore::Bool wantReturn);

	casacore::String getClass() const;

protected:

	std::vector<casacore::Coordinate::Type> _getNecessaryCoordinates() const;

	CasacRegionManager::StokesControl _getStokesControl() const;

private:
	SPIIF _pbImage{};

	casacore::Float _cutoff{0};
	Mode _mode{DIVIDE};
	casacore::Bool _useCutoff{casacore::True}, _concatHistories{casacore::True};
	const static casacore::String _class;

	void _checkPBSanity();
};

}

#endif
