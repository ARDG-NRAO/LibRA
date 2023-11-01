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

#ifndef IMAGEANALYSIS_IMAGEFITTERRESULTS_H
#define IMAGEANALYSIS_IMAGEFITTERRESULTS_H

#include <casacore/casa/Quanta/Quantum.h>
#include <components/ComponentModels/ComponentList.h>

#include <imageanalysis/ImageTypedefs.h>

namespace casa {

template <class T> class ImageFitterResults {
	// <summary>
    // Used exclusively by ImageFitter. Unless you are modifying that class,
    // you should have no reason to use this class.
	// </summary>

	// <reviewed reviewer="" date="" tests="" demos="">
	// </reviewed>

	// <prerequisite>
	// </prerequisite>

	// <etymology>
	// </etymology>

	// <synopsis>
	// </synopsis>

	// <example>
	// <srcblock>
	// </srcblock>
	// </example>

public:

	enum CompListWriteControl {
		NO_WRITE,
		WRITE_NO_REPLACE,
		OVERWRITE
	};

	ImageFitterResults() = delete;

	ImageFitterResults(SPCIIT image, std::shared_ptr<casacore::LogIO> log);

	~ImageFitterResults();

	void setChannels(std::vector<casacore::uInt> chans) { _channels = chans; }

	void setConvolvedList(const ComponentList& list) {
		_convolvedList = list;
	}

	void setDeconvolvedList(const ComponentList& list) {
	    _deconvolvedList = list;
	}

	void setPeakIntensities(const std::vector<casacore::Quantity>& p) {
		_peakIntensities = p;
	}

	void setPeakIntensityErrors(const std::vector<casacore::Quantity>& m) {
		_peakIntensityErrors = m;
	}
	void setMajorAxes(const std::vector<casacore::Quantity>& m) {
		_majorAxes = m;
	}

	void setMinorAxes(const std::vector<casacore::Quantity>& m) {
		_minorAxes = m;
	}

	void setPositionAngles(const std::vector<casacore::Quantity>& m) {
		_positionAngles = m;
	}

	void setFluxDensities(const std::vector<casacore::Quantity>& m) {
		_fluxDensities = m;
	}

	void setFluxDensityErrors(const std::vector<casacore::Quantity>& m) {
		_fluxDensityErrors = m;
	}

	void writeNewEstimatesFile(const casacore::String& filename) const;

	void writeCompList(
		ComponentList& list, const casacore::String& compListName,
		CompListWriteControl writeControl
	) const;

	casacore::String resultsHeader(
		const casacore::String& chans, const casacore::Vector<casacore::uInt>& chanVec,
		const casacore::String& region, const casacore::String& mask,
		std::shared_ptr<std::pair<T, T>> includePixelRange,
		std::shared_ptr<std::pair<T, T>> excludePixelRange,
		const casacore::String& estimates
	) const;

	static std::vector<casacore::String> unitPrefixes(casacore::Bool includeC);

	void setStokes(const casacore::String& s) { _stokes = s; }

	casacore::String fluxToString(casacore::uInt compNumber, casacore::Bool hasBeam) const;

	void setFixed(const casacore::Vector<casacore::String>& s) { _fixed = s; }

	void writeSummaryFile(
	    const casacore::String& filename, const casacore::CoordinateSystem& csys
	) const;

private:
	SPCIIT _image;
	std::shared_ptr<casacore::LogIO> _log;
	ComponentList _convolvedList{}, _deconvolvedList{};
	std::vector<casacore::Quantity> _peakIntensities, _peakIntensityErrors,
		_majorAxes, _minorAxes, _positionAngles, _fluxDensities, _fluxDensityErrors;
	casacore::String _bUnit, _stokes;
	casacore::Vector<casacore::String> _fixed;
	std::vector<casacore::uInt> _channels{};
	const static casacore::String _class;
	static std::vector<casacore::String> _prefixes, _prefixesWithCenti;
};

}

#ifndef AIPS_NO_TEMPLATE_SRC
#include <imageanalysis/IO/ImageFitterResults.tcc>
#endif

#endif
