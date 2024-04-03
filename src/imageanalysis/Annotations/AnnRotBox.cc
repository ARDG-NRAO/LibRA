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

#include <imageanalysis/Annotations/AnnRotBox.h>

using namespace casacore;
namespace casa {

AnnRotBox::AnnRotBox(
	const Quantity& xcenter,
	const Quantity& ycenter,
	const Quantity& xwidth,
	const Quantity& ywidth, const Quantity& positionAngle,
	const String& dirRefFrameString,
	const CoordinateSystem& csys, const IPosition& imShape,
	const Quantity& beginFreq,
	const Quantity& endFreq, const String& freqRefFrameString,
	const String& dopplerString, const Quantity& restfreq,
	const Vector<Stokes::StokesTypes> stokes,
	const Bool annotationOnly,
	const Bool requireImageRegion
) : AnnPolygon(
		ROTATED_BOX, xcenter, ycenter, xwidth, ywidth,
		positionAngle, dirRefFrameString, csys, imShape,
		beginFreq, endFreq, freqRefFrameString,
		dopplerString, restfreq, stokes, annotationOnly,
		requireImageRegion
	), _inputCenter(AnnotationBase::Direction(1, std::make_pair(xcenter, ycenter))),
	_inputWidths(Vector<Quantity>(2)),
	_positionAngle(positionAngle) {
	_inputWidths(0) = xwidth;
	_inputWidths(1) = ywidth;
}

AnnRotBox::AnnRotBox(
	const Quantity& xcenter,
	const Quantity& ycenter,
	const Quantity& xwidth,
	const Quantity& ywidth, const Quantity& positionAngle,
	const CoordinateSystem& csys, const IPosition& imShape,
	const Vector<Stokes::StokesTypes>& stokes,
	const Bool requireImageRegion
) : AnnPolygon(
		ROTATED_BOX, xcenter, ycenter, xwidth, ywidth,
		positionAngle, csys, imShape, stokes, requireImageRegion
	),
	_inputCenter(AnnotationBase::Direction(1, std::make_pair(xcenter, ycenter))),
	_inputWidths(Vector<Quantity>(2)),
	_positionAngle(positionAngle) {
	_inputWidths(0) = xwidth;
	_inputWidths(1) = ywidth;
}

AnnRotBox& AnnRotBox::operator= (
	const AnnRotBox& other
) {
	if (this == &other) {
		return *this;
	}
	AnnPolygon::operator=(other);
	_inputCenter.resize(other._inputCenter.nelements());
	_inputCenter = other._inputCenter;
	_inputWidths.resize(other._inputWidths.nelements());
	_inputWidths = other._inputWidths;
	//_widths.resize(other._widths.nelements());
	//_widths = other._widths;
	_positionAngle = other._positionAngle;
	//_corners.resize(other._corners.nelements());
	//_corners = other._corners;
	return *this;
}

ostream& AnnRotBox::print(ostream &os) const {
	_printPrefix(os);
	os << "rotbox [["
		<< _printDirection(_inputCenter[0].first, _inputCenter[0].second)
		<< "], ["
		<< _toArcsec(_inputWidths[0]) << ", "
		<< _toArcsec(_inputWidths[1]) << "], "
		<< _toDeg(_positionAngle) << "]";
	_printPairs(os);
	return os;
}

}
