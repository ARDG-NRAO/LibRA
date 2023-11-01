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


#include <imageanalysis/Annotations/AnnCircle.h>

#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/images/Regions/WCEllipsoid.h>


using namespace casacore;
namespace casa {

AnnCircle::AnnCircle(
	const Quantity& xcenter,
	const Quantity& ycenter,
	const Quantity& radius,
	const String& dirRefFrameString,
	const CoordinateSystem& csys,
	const IPosition& imShape,
	const Quantity& beginFreq,
	const Quantity& endFreq,
	const String& freqRefFrameString,
	const String& dopplerString,
	const Quantity& restfreq,
	const Vector<Stokes::StokesTypes> stokes,
	const Bool annotationOnly,
	const Bool requireImageRegion
) : AnnRegion(
		CIRCLE, dirRefFrameString, csys, imShape, beginFreq,
		endFreq, freqRefFrameString, dopplerString,
		restfreq, stokes, annotationOnly, requireImageRegion
), _inputCenter(AnnotationBase::Direction(1)), _inputRadius(radius) {
	_init(xcenter, ycenter);
}

AnnCircle::AnnCircle(
	const Quantity& xcenter,
	const Quantity& ycenter,
	const Quantity& radius,
	const CoordinateSystem& csys,
	const IPosition& imShape,
	const Vector<Stokes::StokesTypes>& stokes,
	const Bool requireImageRegion
) : AnnRegion(CIRCLE, csys, imShape, stokes, requireImageRegion),
	_inputCenter(AnnotationBase::Direction(1)), _inputRadius(radius) {
	_init(xcenter, ycenter);
}

AnnCircle& AnnCircle::operator= (
	const AnnCircle& other
) {
	if (this == &other) {
		return *this;
	}
	AnnRegion::operator=(other);
	_inputCenter.resize(other._inputCenter.nelements());
	_inputCenter = other._inputCenter;
	_inputRadius = other._inputRadius;
	_convertedRadius = other._convertedRadius;
	return *this;
}


MDirection AnnCircle::getCenter() const {
	return getConvertedDirections()[0];
}

Quantity AnnCircle::getRadius() const {
	return _convertedRadius;
}

ostream& AnnCircle::print(ostream &os) const {
	_printPrefix(os);
	os << "circle [["
		<< _printDirection(_inputCenter[0].first, _inputCenter[0].second)
		<< "], " << _toArcsec(_inputRadius) << "]";
	_printPairs(os);
	return os;
}

void AnnCircle::_init(const Quantity& xcenter, const Quantity& ycenter) {
	ThrowIf(
		_inputRadius.getUnit() == "pix"
			&& ! getCsys().directionCoordinate().hasSquarePixels(),
		"AnnCircle should not be used when radius is "
			"in pixels and pixels are not square. Use AnnEllipse instead."
	);
	_convertedRadius = _lengthToAngle(_inputRadius, _getDirectionAxes()[0]);
	_inputCenter[0].first = xcenter;
	_inputCenter[0].second = ycenter;

	_checkAndConvertDirections(String(__FUNCTION__), _inputCenter);

	Vector<Double> coords = getConvertedDirections()[0].getAngle("rad").getValue();

	Vector<Quantity> center(2);

	center[0] = Quantity(coords[0], "rad");
	center[1] = Quantity(coords[1], "rad");

	try {
		WCEllipsoid circle(
			center, _convertedRadius, _getDirectionAxes(),
			getCsys(), RegionType::Abs
		);
		_setDirectionRegion(circle);
		_extend();
	} catch (const ToLCRegionConversionError& err) {
		if (_requireImageRegion) {
			throw(err);
		} else {
			ImageRegion defaultRegion;
			_setDirectionRegion(defaultRegion);
			_imageRegion = _directionRegion;
		}
	}
}

}
