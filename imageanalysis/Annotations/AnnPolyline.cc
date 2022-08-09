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


#include <imageanalysis/Annotations/AnnPolyline.h>

#include <casacore/images/Regions/WCPolygon.h>

using namespace casacore;
namespace casa {

AnnPolyline::AnnPolyline(
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
		POLYLINE, dirRefFrameString, csys, imShape, beginFreq,
		endFreq, freqRefFrameString, dopplerString,
		restfreq, stokes, annotationOnly, requireImageRegion
), _origXPos(xPositions), _origYPos(yPositions) {
	_init();
}

AnnPolyline::AnnPolyline(
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

AnnPolyline& AnnPolyline::operator= (
	const AnnPolyline& other
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

Vector<MDirection> AnnPolyline::getCorners() const {
	return getConvertedDirections();
}

ostream& AnnPolyline::print(ostream &os) const {
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

void AnnPolyline::worldVertices(vector<Quantity>& x, vector<Quantity>& y) const {
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

void AnnPolyline::pixelVertices(vector<Double>& x, vector<Double>& y) const {
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

void AnnPolyline::_init() {
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
