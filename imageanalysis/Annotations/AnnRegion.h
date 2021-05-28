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

#ifndef REGIONS_ANNREGION_H
#define REGIONS_ANNREGION_H

#include <coordinates/Coordinates/CoordinateSystem.h>
#include <imageanalysis/Annotations/AnnotationBase.h>
#include <images/Regions/WCBox.h>
#include <images/Regions/ImageRegion.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MFrequency.h>

namespace casa {

// <summary>
// This class represents a annotation referring to a region specified in an ascii region file as proposed
// in CAS-2285
// </summary>
// <author>Dave Mehringer</author>
// <use visibility=export>
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>
// <prerequisite>

// </prerequisite>

// <etymology>
// Holds the specification of an annotation containing a region as specified in ASCII format.
// </etymology>

// <synopsis>
// This class is meant to be a container for all parameters necessary to specify a region per
// the format proposal attached to CAS-2285 (https://bugs.nrao.edu/browse/CAS-2285).

// Conversions of frequency from one reference frame to another are done here. The position of
// the reference pixel in the supplied coordinate system is used when converting frequencies.
// </synopsis>

class AnnRegion: public AnnotationBase {

public:

	virtual ~AnnRegion();

	void setAnnotationOnly(const casacore::Bool isAnnotationOnly);

	// is this region an annotation only? ie just for graphical rendering?
	casacore::Bool isAnnotationOnly() const;

	virtual casacore::TableRecord asRecord() const;

	virtual casacore::ImageRegion asImageRegion() const;

	// this version is deprecated, use the version that returns
	// std::shared_ptr instead
	virtual casacore::CountedPtr<const casacore::WCRegion> getRegion() const;

	virtual std::shared_ptr<const casacore::WCRegion> getRegion2() const;

	// returns true unless overridden.
	virtual casacore::Bool isRegion() const;

	void setDifference(const casacore::Bool difference);

	casacore::Bool isDifference() const;

	// get the pixel range included in the spectral selection.
	// If there is no spectral axis, a zero length vector is returned. Otherwise,
	// a vector of two values is returned. The zeroth value will always be less
	// than or equal to the first.
    std::vector<casacore::Double> getSpectralPixelRange() const;


	casacore::Bool setFrequencyLimits(
		const casacore::Quantity& beginFreq,
		const casacore::Quantity& endFreq,
		const casacore::String& freqRefFrame,
		const casacore::String& dopplerString,
		const casacore::Quantity& restfreq
	);

protected:

	// only to be called by subclasses

	// beginFreq and endFreq can both be 0, in which case
	// all the spectral range is used if a spectral axis exists in
	// <src>csys</csys>. If one of <src>beginFreq</src> or <src>endFreq</src>
	// is given, the other must be given. Frequency units can either conform
	// to Hz, m/s, or pix. If <src>beginFreq</src> and <src>endFreq</src> are not
	// specifed or if they are specified in pixel units,
	// <src>freqRefFrame</src>, <src>dopplerString</src>, and
	// <src>restfreq</src> are ignored. If provided, <src>beginFreq</src>
	// and <src>endFreq</src> must conform to the same units.
	// <src>requireImageRegion</src> indicates whether to rethrow the
	// ToLCRegionConversionError exception when the region is outside the
	// image, or to create the AnnRegion object even if the ImageRegion has no
	// lattice region. The default (true) rethrows the exception to maintain
	// the previous behavior.
	// CAS-12631: added for CARTA, which can import regions outside an image.
	AnnRegion(
		const Type shape,
		const casacore::String& dirRefFrameString,
		const casacore::CoordinateSystem& csys,
		const casacore::IPosition& imShape,
		const casacore::Quantity& beginFreq,
		const casacore::Quantity& endFreq,
		const casacore::String& freqRefFrame,
		const casacore::String& dopplerString,
		const casacore::Quantity& restfreq,
		const casacore::Vector<casacore::Stokes::StokesTypes> stokes,
		const casacore::Bool annotationOnly,
		casacore::Bool requireImageRegion=true
	);

	// use if all coordinate values will be specified in
	// the same frames as the input coordinate system. frequencies
	// and the annotationOnly flag can be set after
	// construction. By default, all frequencies and all polarizations
	// are used, and the annotationOnly flag is false
	AnnRegion(
		const Type shape,
		const casacore::CoordinateSystem& csys,
		const casacore::IPosition& imShape,
		const casacore::Vector<casacore::Stokes::StokesTypes>& stokes,
		casacore::Bool requireImageRegion=true
	);

	// copy constructor
	AnnRegion(const AnnRegion& other);

	// assignment operator
	AnnRegion& operator= (const AnnRegion& rhs);

	casacore::Bool operator== (const AnnRegion& other) const;

	// check if image region has a region
	casacore::Bool hasImageRegion() const;

	// extend the direction plane region over spectral and/or polarization
	// coordinates
	void _extend();

	void _toRecord(const casacore::ImageRegion& region);

	// convert a length in pixels to an angle.
	casacore::Quantity _lengthToAngle(
		const casacore::Quantity& quantity, const casacore::uInt pixelAxis
	) const;

	virtual void _printPrefix(std::ostream& os) const;

	// subclasses must call this at construction to set their base region
	// defined in the direction plane
	void _setDirectionRegion(const casacore::ImageRegion& region);

	casacore::Bool _requireImageRegion;
	casacore::ImageRegion _imageRegion, _directionRegion;

private:

	casacore::Bool _isAnnotationOnly;
	casacore::Bool _isDifference, _constructing;
	casacore::IPosition _imShape;
	std::vector<casacore::Double> _spectralPixelRange;

	static const casacore::String _class;

	casacore::WCBox _makeExtensionBox(
		const casacore::Vector<casacore::Quantity>& freqRange,
		const casacore::Vector<casacore::Stokes::StokesTypes>& stokesRange,
		const casacore::IPosition& pixelAxes
	) const;

	void _init();

	casacore::Bool _hasDirectionRegion();

};

// Just need a identifable expection class, compiler can generate implementation implicitly
class ToLCRegionConversionError : public casacore::AipsError {
public:
	ToLCRegionConversionError(casacore::String msg) : casacore::AipsError(msg) {}
};

}



#endif
