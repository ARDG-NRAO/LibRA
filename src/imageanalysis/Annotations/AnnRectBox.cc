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


#include <imageanalysis/Annotations/AnnRectBox.h>

#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>

using namespace casacore;
namespace casa {

AnnRectBox::AnnRectBox(
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
) : AnnPolygon(
		RECT_BOX, blcx, blcy, trcx, trcy,
		dirRefFrameString, csys, imShape,
		beginFreq, endFreq, freqRefFrameString,
		dopplerString, restfreq, stokes,
		annotationOnly, requireImageRegion
	), _inputCorners(2) {
	_inputCorners[0].first = blcx;
	_inputCorners[0].second = blcy;
	_inputCorners[1].first = trcx;
	_inputCorners[1].second = trcy;
}

AnnRectBox::AnnRectBox(
	const Quantity& blcx,
	const Quantity& blcy,
	const Quantity& trcx,
	const Quantity& trcy,
	const CoordinateSystem& csys,
	const IPosition& imShape,
	const Vector<Stokes::StokesTypes>& stokes,
	const Bool requireImageRegion
) : AnnPolygon(
	RECT_BOX, blcx, blcy, trcx, trcy, csys, imShape, stokes,
	requireImageRegion
), _inputCorners(2) {
	_inputCorners[0].first = blcx;
	_inputCorners[0].second = blcy;
	_inputCorners[1].first = trcx;
	_inputCorners[1].second = trcy;
}

AnnRectBox& AnnRectBox::operator= (
	const AnnRectBox& other
) {
	if (this == &other) {
		return *this;
	}
	AnnPolygon::operator=(other);
	_inputCorners.resize(other._inputCorners.shape());
	_inputCorners = other._inputCorners;

	return *this;
}

ostream& AnnRectBox::print(ostream &os) const {
	_printPrefix(os);
	os << "box [["
		<< _printDirection(_inputCorners[0].first, _inputCorners[0].second)
		<< "], ["
		<< _printDirection(_inputCorners[1].first, _inputCorners[1].second)
		<< "]]";
	_printPairs(os);
	return os;
}

}

