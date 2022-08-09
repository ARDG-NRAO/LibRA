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

#include <imageanalysis/Annotations/AnnEllipse.h>

#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/casa/Quanta/QLogical.h>
#include <casacore/images/Regions/WCEllipsoid.h>

using namespace casacore;
namespace casa {

AnnEllipse::AnnEllipse(
	const Quantity& xcenter, const Quantity& ycenter,
	const Quantity& semiMajorAxis,
	const Quantity& semiMinorAxis, const Quantity& positionAngle,
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
		ELLIPSE, dirRefFrameString, csys, imShape, beginFreq,
		endFreq, freqRefFrameString, dopplerString,
		restfreq, stokes, annotationOnly, requireImageRegion
), _inputCenter(AnnotationBase::Direction(1)), _inputSemiMajorAxis(semiMajorAxis),
	_inputSemiMinorAxis(semiMinorAxis),
	_inputPositionAngle(positionAngle) {
	_init(xcenter, ycenter);
}

AnnEllipse::AnnEllipse(
	const Quantity& xcenter, const Quantity& ycenter,
	const Quantity& semiMajorAxis,
	const Quantity& semiMinorAxis, const Quantity& positionAngle,
	const CoordinateSystem& csys,
	const IPosition& imShape,
	const Vector<Stokes::StokesTypes>& stokes,
	const Bool requireImageRegion
) : AnnRegion(ELLIPSE, csys, imShape, stokes, requireImageRegion),
	_inputCenter(AnnotationBase::Direction(1)), _inputSemiMajorAxis(semiMajorAxis),
	_inputSemiMinorAxis(semiMinorAxis),
	_inputPositionAngle(positionAngle) {
	_init(xcenter, ycenter);
}

AnnEllipse& AnnEllipse::operator= (
	const AnnEllipse& other
) {
	if (this == &other) {
		return *this;
	}
	AnnRegion::operator=(other);
	_inputCenter.resize(other._inputCenter.nelements());
	_inputCenter = other._inputCenter;
	_inputSemiMajorAxis = other._inputSemiMajorAxis;
	_inputSemiMinorAxis = other._inputSemiMinorAxis;
	_inputPositionAngle = other._inputPositionAngle;
	_convertedSemiMajorAxis = other._convertedSemiMajorAxis;
	_convertedSemiMinorAxis = other._convertedSemiMinorAxis;
	_convertedPositionAngle = other._convertedPositionAngle;
	return *this;
}

Bool AnnEllipse::operator== (
	const AnnEllipse& other
)const {
	if (this == &other) {
		return true;
	}
	return AnnRegion::operator==(other)
		&& allTrue(_inputCenter == other._inputCenter)
		&& _inputSemiMajorAxis == other._inputSemiMajorAxis
		&& _inputSemiMinorAxis == other._inputSemiMinorAxis
		&& _inputPositionAngle == other._inputPositionAngle
		&& _convertedSemiMajorAxis == other._convertedSemiMajorAxis
		&& _convertedSemiMinorAxis == other._convertedSemiMinorAxis
		&& _convertedPositionAngle == other._convertedPositionAngle;
}

MDirection AnnEllipse::getCenter() const {
	return getConvertedDirections()[0];
}

Quantity AnnEllipse::getSemiMajorAxis() const {
	return _convertedSemiMajorAxis;
}

Quantity AnnEllipse::getSemiMinorAxis() const {
	return _convertedSemiMinorAxis;
}

Quantity AnnEllipse::getPositionAngle() const {
	return _convertedPositionAngle;
}

ostream& AnnEllipse::print(ostream &os) const {
	_printPrefix(os);
	os << "ellipse [["
		<< _printDirection(_inputCenter[0].first, _inputCenter[0].second)
		<< "], [" << _toArcsec(_inputSemiMajorAxis)
		<< ", " << _toArcsec(_inputSemiMinorAxis) << "], "
		<< _toDeg(_inputPositionAngle) << "]";
	_printPairs(os);
	return os;
}

void AnnEllipse::_init(
	const Quantity& xcenter, const Quantity& ycenter
) {
	ThrowIf(
		! _inputPositionAngle.isConform("rad"),
		"Position angle must have angular units"
	);
	ThrowIf(
		_inputSemiMajorAxis.getUnit() == "pix"
		&& ! getCsys().directionCoordinate().hasSquarePixels()
		&& (
			! casacore::near(fmod(_inputPositionAngle.getValue("rad"), C::pi), 0.0)
			&& ! casacore::near(fmod(fabs(_inputPositionAngle.getValue("rad")), C::pi), C::pi_2)
		),
		"When pixels are not square and units are expressed in "
		"pixels, position angle must be zero"
	);
	const CoordinateSystem csys = getCsys();
	uInt x = csys.isDirectionAbscissaLongitude() ? 0 : 1;
	uInt y = x == 0 ? 1 : 0;
	_convertedSemiMajorAxis = _lengthToAngle(_inputSemiMajorAxis, _getDirectionAxes()[y]);
	_convertedSemiMinorAxis = _lengthToAngle(_inputSemiMinorAxis, _getDirectionAxes()[x]);
	_convertedPositionAngle = _inputPositionAngle;
	if (
		csys.directionCoordinate().directionType() != _getDirectionRefFrame()
	) {
		Quantity angle;
		csys.directionCoordinate().convert(angle, _getDirectionRefFrame());
		// add the clockwise angle rather than subtract because the pixel
		// axes are aligned with the "from" (current) world coordinate system rather
		// than the "to" world coordinate system
		_convertedPositionAngle += angle;
	}
	if (_convertedSemiMajorAxis < _convertedSemiMinorAxis) {
		std::swap(_convertedSemiMajorAxis, _convertedSemiMinorAxis);
		_convertedPositionAngle = Quantity(
			std::fmod(
				(_inputPositionAngle + Quantity(C::pi_2, "rad")).getValue("rad"),
				C::pi),
			"rad"
		);
		_convertedPositionAngle.convert(_inputPositionAngle);
	}
	_inputCenter[0].first = xcenter;
	_inputCenter[0].second = ycenter;

	_checkAndConvertDirections(String(__func__), _inputCenter);

	Vector<Double> coords = getConvertedDirections()[0].getAngle("rad").getValue();

	Vector<Quantity> qCenter(2);
	qCenter[0] = Quantity(coords[0], "rad");
	qCenter[1] = Quantity(coords[1], "rad");

	// WCEllipsoid expects the angle to the major axis to be relative to the positive x
	// axis. Astronomers however measure the position angle relative to north (positive y axis usually).
	Quantity relToXAxis = _convertedPositionAngle + Quantity(90, "deg");

	try {
		WCEllipsoid ellipse(
			qCenter[0], qCenter[1],
			_convertedSemiMajorAxis, _convertedSemiMinorAxis, relToXAxis,
			_getDirectionAxes()[0], _getDirectionAxes()[1], getCsys()
		);
		_setDirectionRegion(ellipse);
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
