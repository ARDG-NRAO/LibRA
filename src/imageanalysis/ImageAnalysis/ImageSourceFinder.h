//# ImageSourceFinder.h: find sources in image
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002
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
//#
//# $Id: ImageSourceFinder.h 20299 2008-04-03 05:56:44Z gervandiepen $

#ifndef IMAGES_IMAGESOURCEFINDER_H
#define IMAGES_IMAGESOURCEFINDER_H

#include <casacore/casa/aips.h>
#include <casacore/measures/Measures/Stokes.h>      
#include <components/ComponentModels/ComponentType.h>
#include <imageanalysis/ImageAnalysis/ImageTask.h>

namespace casacore{
template <class T> class ImageInterface;
class LogIO;
}

namespace casa { //# NAMESPACE CASA - BEGIN

//# Forward Declarations
class ComponentList;
class SkyComponent;


// <summary>
// Provides functionality to find sources in an image
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class=casacore::ImageInterface>ImageInterface</linkto>
//   <li> <linkto module=Coordinates>Coordinates</linkto> 
// </prerequisite>

// <etymology>
// It finds sources.
// </etymology>


// <synopsis>
// This class provides methods to find sources in an image.
// 
// The finding procedes in two stages.  First, strong point sources
// are found via an efficient algorithm producing POINT components. 
// If you wish, you can further request a Gaussian fit to these 
// found point sources and then return the parameters of the 
// fit (as a GAUSIAN component).
// </synopsis>

// <example>
// <srcBlock>
// </srcBlock>
// </example>

// <motivation>
// This complements source fitting by providing an initial estimate
// </motivation>

// <todo asof="2000/11/08">
// </todo>
 

template <class T> class ImageSourceFinder : public ImageTask<T> {
public:

	ImageSourceFinder() = delete;

	ImageSourceFinder(SPCIIT image, const casacore::Record *const region, const casacore::String& mask);

	ImageSourceFinder (const ImageSourceFinder<T> &other) = delete;

	~ImageSourceFinder();

	ImageSourceFinder<T> &operator=(const ImageSourceFinder<T> &other) = delete;

	// Find strong sources.  nMax specifies the maximum number of sources to find.
	// cutoff is the fractional cutoff (of peak) and soiurces below this limit
	// will not be found. If absFind is true, only positive sources are found, else
	// positive and negative are found. If doPoint=true, the returned components
	// are of type POINT.  If doPoint=false, the position and shape information is
	// returned via a Gaussian fit (and components will be of
	// type GAUSSIAN) to the point sources initially found.    The parameter width
	// specifies the half-width of a square grid of pixels centered on the initial
	// point source location to be used in the fit.  If you set doPoint=false and width=0,
	// a default width of 3 and position angle 0 is returned in the GAUSSIAN component.
	// Because  the flux of the component is integrated, this rough shape influences the
	// flux values as well.
	ComponentList findSources (casacore::Int nMax);

	// Find one source in sky plane.  Exception if no sky
	SkyComponent findSourceInSky(casacore::Vector<casacore::Double>& absPixel);

	void setCutoff(casacore::Double cutoff) { _cutoff = cutoff; }

	void setAbsFind(casacore::Bool af) { _absFind = af; }

	void setDoPoint(casacore::Bool dp) { _doPoint = dp; }

	void setWidth(casacore::Int w) { _width = w; }

	casacore::String getClass() const {
		const static casacore::String x = "ImageSourceFinder";
		return x;
	}

protected:

   	CasacRegionManager::StokesControl _getStokesControl() const {
   		return CasacRegionManager::USE_FIRST_STOKES;
   	}

    // Represents the minimum set of coordinates necessary for the
    // task to function.
    std::vector<casacore::Coordinate::Type> _getNecessaryCoordinates() const {
    	return std::vector<casacore::Coordinate::Type>(1, casacore::Coordinate::DIRECTION);
    }

    inline casacore::Bool _supportsMultipleRegions() const {return true;}

    inline casacore::Bool _supportsMultipleBeams() const {return false;}


private:
	casacore::Double _cutoff = 0.1;
	casacore::Bool _absFind = true;
	casacore::Bool _doPoint = true;
	casacore::Int _width = 4;

	// Find strong (point) sources
	ComponentList _findSources(casacore::Int nMax);
};


}

#ifndef CASACORE_NO_AUTO_TEMPLATES
#include <imageanalysis/ImageAnalysis/ImageSourceFinder.tcc>
#endif
#endif
