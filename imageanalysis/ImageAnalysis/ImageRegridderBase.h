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

#ifndef IMAGEANALYSIS_IMAGEREGRIDDERBASE_H
#define IMAGEANALYSIS_IMAGEREGRIDDERBASE_H

#include <imageanalysis/ImageAnalysis/ImageTask.h>
#include <scimath/Mathematics/Interpolate2D.h>
#include <casa/namespace.h>

namespace casa {

template <class T> class ImageRegridderBase : public ImageTask<T> {
	// <summary>
	// casacore::Data store of ImageRegridder and ComplexImageRegridder
    // TODO ComplexImageRegridder has been removed so this class can probably
    // be merged into ImageRegridder
	// </summary>

	// <reviewed reviewer="" date="" tests="" demos="">
	// </reviewed>

	// <prerequisite>
	// </prerequisite>

	// <etymology>
	// casacore::Data for image regridder.
	// </etymology>

	// <synopsis>
	// </synopsis>

	template<typename U> friend class ImageRegridderBase;

public:

	ImageRegridderBase() = delete;

	// destructor
	virtual ~ImageRegridderBase();

	// regrid the spectral axis in velocity space rather than frequency space?
	void setSpecAsVelocity(casacore::Bool v) { _specAsVelocity = v; }

	// Set interpolation method.
	void setMethod(const casacore::String& method) {
	    _method = casacore::Interpolate2D::stringToMethod(method);
	}

	void setMethod(casacore::Interpolate2D::Method method) { _method = method; }

	void setDoRefChange(casacore::Bool d) { _doRefChange = d; }

	void setReplicate(casacore::Bool r) { _replicate = r; }

	// throws exception if 3*decimate > length of an axis that will be regridded
	void setDecimate(casacore::Int d);

	void setForceRegrid(casacore::Bool f) { _forceRegrid = f; }

	void setShape(const casacore::IPosition s) { _shape = s; }

	virtual SPIIT regrid() const = 0;

	template <class U> void setConfiguration(const ImageRegridderBase<U>& that);

protected:

	ImageRegridderBase(
		const SPCIIT image, const casacore::Record *const regionRec,
		const casacore::String& maskInp, const casacore::String& outname,
		casacore::Bool overwrite, const casacore::CoordinateSystem& csys,
		const casacore::IPosition& axes, const casacore::IPosition& shape
	);

	casacore::Interpolate2D::Method _getMethod() const { return _method; }

	casacore::Bool _getDoRefChange() const { return _doRefChange; }

	casacore::Bool _getReplicate() const { return _replicate; }

	casacore::Int _getDecimate() const { return _decimate;}

	casacore::Bool _getForceRegrid() const { return _forceRegrid; }

	inline CasacRegionManager::StokesControl _getStokesControl() const {
		return CasacRegionManager::USE_ALL_STOKES;
	}

	inline std::vector<casacore::Coordinate::Type> _getNecessaryCoordinates() const {
		return std::vector<casacore::Coordinate::Type>(0);
	}

	casacore::Bool _getSpecAsVelocity() const { return _specAsVelocity; }

	casacore::IPosition _getShape() const {return _shape;}

	const casacore::CoordinateSystem& _getTemplateCoords() const {
	    return _csysTo;
	}

	casacore::IPosition _getAxes() const { return _axes; }

	casacore::IPosition _getKludgedShape() const { return _kludgedShape; }

	std::vector<casacore::String> _getOutputStokes() const { return _outputStokes; }

	casacore::uInt _getNReplicatedChans() const { return _nReplicatedChans; }

	casacore::Bool _regriddingDirectionAxes() const;

private:
	const casacore::CoordinateSystem _csysTo;
	casacore::IPosition _axes, _shape, _kludgedShape;
	casacore::Bool _specAsVelocity, _doRefChange, _replicate, _forceRegrid;
	casacore::Int _decimate;
	casacore::Interpolate2D::Method _method;
	std::vector<casacore::String> _outputStokes;
	casacore::uInt _nReplicatedChans;

	void _finishConstruction();

};
}

#ifndef AIPS_NO_TEMPLATE_SRC
#include <imageanalysis/ImageAnalysis/ImageRegridderBase.tcc>
#endif //# AIPS_NO_TEMPLATE_SRC

#endif
