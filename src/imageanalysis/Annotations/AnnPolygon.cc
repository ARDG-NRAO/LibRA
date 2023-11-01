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

#include <imageanalysis/Annotations/AnnPolygon.h>

#include <casacore/casa/Quanta/QMath.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/images/Regions/WCPolygon.h>
#include <casacore/scimath/Mathematics/Geometry.h>

using namespace casacore;
namespace casa {

AnnPolygon::AnnPolygon(
	const Vector<Quantity>& xPositions,
	const Vector<Quantity>& yPositions,
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
		POLYGON, dirRefFrameString, csys, imShape, beginFreq,
		endFreq, freqRefFrameString, dopplerString,
		restfreq, stokes, annotationOnly, requireImageRegion
), _origXPos(xPositions), _origYPos(yPositions) {
	_init();
}

AnnPolygon::AnnPolygon(
	const Vector<Quantity>& xPositions,
	const Vector<Quantity>& yPositions,
	const CoordinateSystem& csys,
	const IPosition& imShape,
	const Vector<Stokes::StokesTypes>& stokes,
	const Bool requireImageRegion
) : AnnRegion(POLYGON, csys, imShape, stokes, requireImageRegion),
	_origXPos(xPositions), _origYPos(yPositions) {
	_init();
}

AnnPolygon::AnnPolygon(
	AnnotationBase::Type shape,
	const Quantity& blcx,
	const Quantity& blcy,
	const Quantity& trcx,
	const Quantity& trcy,
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
		shape, dirRefFrameString, csys, imShape, beginFreq,
		endFreq, freqRefFrameString, dopplerString,
		restfreq, stokes, annotationOnly, requireImageRegion
), _origXPos(4), _origYPos(4) {
	_initCorners(blcx, blcy, trcx, trcy);
	_init();
}

	// Simplified constructor.
	// all frequencies are used (these can be set after construction).
	// blcx, blcy, trcx, and trcy
	// must be in the same frame as the csys direction coordinate.
	// is a region (not just an annotation), although this value can be changed after
	// construction.
AnnPolygon::AnnPolygon(
	AnnotationBase::Type shape,
	const Quantity& blcx,
	const Quantity& blcy,
	const Quantity& trcx,
	const Quantity& trcy,
	const CoordinateSystem& csys,
	const IPosition& imShape,
	const Vector<Stokes::StokesTypes>& stokes,
	const Bool requireImageRegion
) : AnnRegion(shape, csys, imShape, stokes, requireImageRegion),
	_origXPos(4), _origYPos(4) {
	_initCorners(blcx, blcy, trcx, trcy);
	_init();
}

AnnPolygon::AnnPolygon(
	AnnotationBase::Type shape,
	const Quantity& centerx,
	const Quantity& centery,
	const Quantity& widthx,
	const Quantity& widthy,
	const Quantity& positionAngle,
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
		shape, dirRefFrameString, csys, imShape, beginFreq,
		endFreq, freqRefFrameString, dopplerString,
		restfreq, stokes, annotationOnly, requireImageRegion
		),
		_origXPos(4), _origYPos(4) {
		_initCenterRectCorners(
			centerx, centery, widthx, widthy,
			positionAngle
		);
		_init();
	}

AnnPolygon::AnnPolygon(
	AnnotationBase::Type shape,
	const Quantity& centerx,
	const Quantity& centery,
	const Quantity& widthx,
	const Quantity& widthy,
	const Quantity& positionAngle,
	const CoordinateSystem& csys,
	const IPosition& imShape,
	const Vector<Stokes::StokesTypes>& stokes,
	const Bool requireImageRegion
) : AnnRegion(shape, csys, imShape, stokes, requireImageRegion),
	_origXPos(4), _origYPos(4) {
	_initCenterRectCorners(
		centerx, centery, widthx, widthy,
		positionAngle
	);
	_init();
}

AnnPolygon& AnnPolygon::operator= (
	const AnnPolygon& other
) {
	if (this == &other) {
		return *this;
	}
	AnnRegion::operator=(other);
	_origXPos.resize(other._origXPos.nelements());
	_origXPos = other._origXPos;
	_origYPos.resize(other._origYPos.nelements());
	_origYPos = other._origYPos;
	return *this;
}

Vector<MDirection> AnnPolygon::getCorners() const {
	return getConvertedDirections();
}

ostream& AnnPolygon::print(ostream &os) const {
	_printPrefix(os);
	os << "poly [";
	for (uInt i=0; i<_origXPos.size(); i++) {
		os << "[" << _printDirection(_origXPos[i], _origYPos[i]) << "]";
		if (i < _origXPos.size()-1) {
			os << ", ";
		}
	}
	os << "]";
	_printPairs(os);
	return os;
}

void AnnPolygon::worldVertices(vector<Quantity>& x, vector<Quantity>& y) const {
	const CoordinateSystem csys = getCsys();
	const IPosition dirAxes = _getDirectionAxes();
	String xUnit = csys.worldAxisUnits()[dirAxes[0]];
	String yUnit = csys.worldAxisUnits()[dirAxes[1]];
	Vector<MDirection> corners = getConvertedDirections();
	x.resize(corners.size());
	y.resize(corners.size());
	for (uInt i=0; i<corners.size(); i++) {
		x[i] = Quantity(corners[i].getAngle(xUnit).getValue(xUnit)[0], xUnit);
		y[i] = Quantity(corners[i].getAngle(yUnit).getValue(yUnit)[1], yUnit);
	}
}

void AnnPolygon::pixelVertices(vector<Double>& x, vector<Double>& y) const {
	vector<Quantity> xx, xy;
	worldVertices(xx, xy);

	const CoordinateSystem csys = getCsys();
	Vector<Double> world = csys.referenceValue();
	const IPosition dirAxes = _getDirectionAxes();
	String xUnit = csys.worldAxisUnits()[dirAxes[0]];
	String yUnit = csys.worldAxisUnits()[dirAxes[1]];

	x.resize(xx.size());
	y.resize(xx.size());

	for (uInt i=0; i<xx.size(); i++) {
		world[dirAxes[0]] = xx[i].getValue(xUnit);
		world[dirAxes[1]] = xy[i].getValue(yUnit);
		Vector<Double> pixel;
		csys.toPixel(pixel, world);
		x[i] = pixel[dirAxes[0]];
		y[i] = pixel[dirAxes[1]];
	}
}

void AnnPolygon::_initCorners(
	const Quantity& blcx,
	const Quantity& blcy,
	const Quantity& trcx,
	const Quantity& trcy
) {
	_origXPos[0] = blcx;
	_origYPos[0] = blcy;
	_origXPos[1] = trcx;
	_origYPos[1] = blcy;
	_origXPos[2] = trcx;
	_origYPos[2] = trcy;
	_origXPos[3] = blcx;
	_origYPos[3] = trcy;
}

void AnnPolygon::_initCorners(
	const MDirection& blc,
	const MDirection& corner2,
	const MDirection& trc,
	const MDirection& corner4
) {
	for (uInt i=0; i<4; i++) {
		MDirection dir;
		switch(i) {
		case 0:
			dir = blc;
			break;
		case 1:
			dir = corner2;
			break;
		case 2:
			dir = trc;
			break;
		case 3:
			dir = corner4;
			break;
		default:
			break;
		}
		Quantum<Vector<Double> > dirq = dir.getAngle();
		Vector<Double> x = dirq.getValue();
		String unit = dirq.getUnit();
		_origXPos[i] = Quantity(x[0], unit);
		_origYPos[i] = Quantity(x[1], unit);
	}
}

void AnnPolygon::_initCenterRectCorners(
	const Quantity& centerx,
	const Quantity& centery,
	const Quantity& widthx,
	const Quantity& widthy,
	const Quantity& positionAngle
) {
	ThrowIf(
		! widthx.isConform("rad") && ! widthx.isConform("pix"),
		"x width unit " + widthx.getUnit() + " is not an angular unit."
	);
	ThrowIf(
		! widthy.isConform("rad") && ! widthy.isConform("pix"),
		"y width unit " + widthx.getUnit() + " is not an angular unit."
	);
	ThrowIf(
		! positionAngle.isConform("rad"),
		"position angle unit " + positionAngle.getUnit() + " is not an angular unit."
	);
	ThrowIf(
		widthx.getUnit() == "pix"
		&& ! getCsys().directionCoordinate().hasSquarePixels()
		&& (
			! casacore::near(fmod(positionAngle.getValue("rad"), C::pi), 0.0)
			&& ! casacore::near(fmod(fabs(positionAngle.getValue("rad")), C::pi), C::pi_2)
		),
		"When pixels are not square and units are expressed in "
		"pixels, position angle must be zero"
	);

	Vector<Double> inc = getCsys().increment();
	Double xFactor = inc(_getDirectionAxes()[0]) > 0 ? 1.0 : -1.0;
	Double yFactor = inc(_getDirectionAxes()[1]) > 0 ? 1.0 : -1.0;

	IPosition dirAxes = _getDirectionAxes();
	Quantity wx = _lengthToAngle(widthx, dirAxes[0])/2;
	Quantity wy = _lengthToAngle(widthy, dirAxes[1])/2;

	Vector<MDirection> corners(4);
	MDirection center = _directionFromQuantities(centerx, centery);
	for (uInt i=0; i<4; i++) {
		corners[i] = MDirection(center);
		Int xsign = i == 0 || i == 3 ? -1 : 1;
		Int ysign = i == 0 || i == 1 ? -1 : 1;
		Quantity x = xFactor*xsign*wx;
		Quantity y = yFactor*ysign*wy;
		if (positionAngle.getValue() != 0) {
			// because the pa is measured from north through east (positive y to
			// positive x), this corresponds to a clockwise rotation in normal coordinates
			// so we have to flip the sign of the positionAngle to take that into account.
			std::pair<Double, Double> rotated = Geometry::rotate2D(
				x.getValue("arcsec"), y.getValue("arcsec"), -positionAngle
			);
			x = Quantity(rotated.first, "arcsec");
			y = Quantity(rotated.second, "arcsec");
		}
		corners[i].shift(x, y, true);
	}
	_initCorners(corners[0], corners[1], corners[2], corners[3]);
}

void AnnPolygon::_init() {
	String preamble(String(__FUNCTION__) + ": ");
	if (_origXPos.size() != _origYPos.size()) {
		throw AipsError(
			preamble + "x and y vectors are not the same length but must be."
		);
	}
	AnnotationBase::Direction corners(_origXPos.size());
	for (uInt i=0; i<_origXPos.size(); i++) {
		corners[i].first = _origXPos[i];
		corners[i].second = _origYPos[i];
	}
	_checkAndConvertDirections(String(__FUNCTION__), corners);
	Vector<Double> xv(_origXPos.size()), yv(_origYPos.size());
	for (uInt i=0; i<xv.size(); i++) {
		Vector<Double> coords = getConvertedDirections()[i].getAngle("rad").getValue();
		xv[i] = coords[0];
		yv[i] = coords[1];
	}
	Quantum<Vector<Double> > x(xv, "rad");
	Quantum<Vector<Double> > y(yv, "rad");
	try {
		WCPolygon wpoly(
			x, y, IPosition(_getDirectionAxes()),
			getCsys(), RegionType::Abs
		);
		_setDirectionRegion(wpoly);
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
