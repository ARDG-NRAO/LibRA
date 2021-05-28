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


#include <imageanalysis/Annotations/AnnAnnulus.h>

#include <casa/Arrays/ArrayLogical.h>
#include <casa/Quanta/MVAngle.h>
#include <coordinates/Coordinates/CoordinateUtil.h>
#include <images/Regions/WCDifference.h>
#include <images/Regions/WCEllipsoid.h>
#include <casa/Quanta/QLogical.h>

using namespace casacore;
namespace casa {

AnnAnnulus::AnnAnnulus()
	: AnnRegion(
		ANNULUS, CoordinateUtil::defaultCoords2D(), IPosition(2, 1, 1),
		Vector<Stokes::StokesTypes>()
	), _xcenter(), _ycenter(),
	_innerRadius(), _outerRadius() {}

AnnAnnulus::AnnAnnulus(
	const Quantity& xcenter,
	const Quantity& ycenter,
	const Quantity& innerRadius,
	const Quantity& outerRadius,
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
		ANNULUS, dirRefFrameString, csys, imShape, beginFreq,
		endFreq, freqRefFrameString, dopplerString,
		restfreq, stokes, annotationOnly, requireImageRegion
	), _convertedRadii(Vector<Quantity>(2)),
	_xcenter(xcenter), _ycenter(ycenter),
	_innerRadius(innerRadius), _outerRadius(outerRadius) {
	_init();
}

AnnAnnulus::AnnAnnulus(
	const Quantity& xcenter,
	const Quantity& ycenter,
	const Quantity& innerRadius,
	const Quantity& outerRadius,
	const CoordinateSystem& csys,
	const IPosition& imShape,
	const Vector<Stokes::StokesTypes>& stokes,
	const Bool requireImageRegion
) : AnnRegion(ANNULUS, csys, imShape, stokes, requireImageRegion),
_convertedRadii(Vector<Quantity>(2)),
	_xcenter(xcenter), _ycenter(ycenter),
	_innerRadius(innerRadius), _outerRadius(outerRadius) {
	_init();
}

AnnAnnulus& AnnAnnulus::operator= (
	const AnnAnnulus& other
) {
	if (&other != this) {
		AnnRegion::operator=(other);
		_convertedRadii.resize(other._convertedRadii.nelements());
		_convertedRadii = other._convertedRadii;
		_xcenter = other._xcenter;
		_ycenter = other._ycenter;
		_innerRadius = other._innerRadius;
		_outerRadius = other._outerRadius;
	}
    return *this;
}

Bool AnnAnnulus::operator==(const AnnAnnulus& other) {
	return this == &other || (
		AnnRegion::operator==(other)
		&& allEQ(_convertedRadii, other._convertedRadii)
		&& _xcenter == other._xcenter
		&& _ycenter == other._ycenter
		&& _innerRadius == other._innerRadius
		&& _outerRadius == other._outerRadius
	);
}


MDirection AnnAnnulus::getCenter() const {
	return getConvertedDirections()[0];
}

Vector<Quantity> AnnAnnulus::getRadii() const {
	return _convertedRadii;
}

ostream& AnnAnnulus::print(ostream &os) const {
	_printPrefix(os);
	os << "annulus [[" << _printDirection(_xcenter, _ycenter) << "], ["
		<< _toArcsec(_innerRadius) << ", "
		<< _toArcsec(_outerRadius) << "]] ";
	_printPairs(os);
	return os;
}

void AnnAnnulus::_init() {
	_convertedRadii[0] = _lengthToAngle(_innerRadius, _getDirectionAxes()[0]);
	_convertedRadii[1] = _lengthToAngle(_outerRadius, _getDirectionAxes()[0]);

	if (
		_convertedRadii[0].getValue("rad")
		>= _convertedRadii[1].getValue("rad")
	) {
		throw AipsError(
			String(__FUNCTION__)
			+ "Inner radius must be less than "
			+ "outer radius"
		);
	}

	AnnotationBase::Direction inputCenter(1);
	inputCenter[0].first = _xcenter;
	inputCenter[0].second = _ycenter;

	_checkAndConvertDirections(String(__FUNCTION__), inputCenter);

	Vector<Double> coords = getConvertedDirections()[0].getAngle("rad").getValue();

	Vector<Quantity> qCenter(2);
	qCenter[0] = Quantity(coords[0], "rad");
	qCenter[1] = Quantity(coords[1], "rad");
	try {
		WCEllipsoid inner(qCenter, _innerRadius, _getDirectionAxes(), getCsys(), RegionType::Abs);
		WCEllipsoid outer(qCenter, _outerRadius, _getDirectionAxes(), getCsys(), RegionType::Abs);
		WCDifference annulus(outer, inner);
		_setDirectionRegion(annulus);
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
