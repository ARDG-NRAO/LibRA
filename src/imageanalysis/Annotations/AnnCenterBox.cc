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


#include <imageanalysis/Annotations/AnnCenterBox.h>

using namespace casacore;
namespace casa {

AnnCenterBox::AnnCenterBox(
	const Quantity& xcenter,
	const Quantity& ycenter,
	const Quantity& xwidth,
	const Quantity& ywidth,
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
) :  AnnPolygon(
		CENTER_BOX, xcenter, ycenter,
		xwidth, ywidth, Quantity(0, "deg"),
		dirRefFrameString, csys, imShape, beginFreq, endFreq,
		freqRefFrameString, dopplerString,
		restfreq, stokes,
		annotationOnly, requireImageRegion 
	), _inpXCenter(xcenter),
	_inpYCenter(ycenter), _inpXWidth(xwidth), _inpYWidth(ywidth) {}

AnnCenterBox::AnnCenterBox(
	const Quantity& xcenter,
	const Quantity& ycenter,
	const Quantity& xwidth,
	const Quantity& ywidth,
	const CoordinateSystem& csys,
	const IPosition& imShape,
	const Vector<Stokes::StokesTypes>& stokes,
	const Bool requireImageRegion
) : AnnPolygon(
		CENTER_BOX, xcenter, ycenter,
		xwidth, ywidth, Quantity(0, "deg"),
		csys, imShape,
		stokes, requireImageRegion
	), _inpXCenter(xcenter),
	_inpYCenter(ycenter), _inpXWidth(xwidth), _inpYWidth(ywidth)
{}

AnnCenterBox& AnnCenterBox::operator= (
	const AnnCenterBox& other
) {
	if (this == &other) {
		return *this;
	}
	AnnRegion::operator=(other);
	_inpXCenter = other._inpXCenter;
	_inpYCenter = other._inpYCenter;
	_inpXWidth = other._inpXWidth;
	_inpYWidth = other._inpYWidth;
	return *this;
}

ostream& AnnCenterBox::print(ostream &os) const {
	_printPrefix(os);
	os << "centerbox [[" << _printDirection(_inpXCenter, _inpYCenter) << "], ["
		<< _toArcsec(_inpXWidth) << ", " << _toArcsec(_inpYWidth) << "]]";
	_printPairs(os);
	return os;
}


}
