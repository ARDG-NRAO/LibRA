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

#ifndef ANNOTATIONS_ANNPOLYLINE_H
#define ANNOTATIONS_ANNPOLYLINE_H

#include <casacore/casa/aips.h>
#include <imageanalysis/Annotations/AnnRegion.h>

namespace casa {

// <summary>
// This class represents a single polyline (in position coordinates) annotation specified
// in an ascii region file as proposed in CAS-2285
// </summary>
// <use visibility=export>
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>
// <prerequisite>

// </prerequisite>

// <etymology>
// Holds the specification of a polyline annotation.
// </etymology>

// <synopsis>
// This class represents a polyline annotation.
// </synopsis>


class AnnPolyline: public AnnRegion {

public:

	AnnPolyline(
		const casacore::Vector<casacore::Quantity>& xPositions,
		const casacore::Vector<casacore::Quantity>& yPositions,
		const casacore::String& dirRefFrameString,
		const casacore::CoordinateSystem& csys,
		const casacore::IPosition& imShape,
		const casacore::Quantity& beginFreq,
		const casacore::Quantity& endFreq,
		const casacore::String& freqRefFrameString,
		const casacore::String& dopplerString,
		const casacore::Quantity& restfreq,
		const casacore::Vector<casacore::Stokes::StokesTypes> stokes,
		const casacore::Bool annotationOnly,
		const casacore::Bool requireImageRegion=true
	);

	// Simplified constructor.
	// all frequencies are used (these can be set after construction).
	// xPositions and yPositions
	// must be in the same frame as the csys direction coordinate.
	// is a region (not just an annotation), although this value can be changed after
	// construction.
	AnnPolyline(
		const casacore::Vector<casacore::Quantity>& xPositions,
		const casacore::Vector<casacore::Quantity>& yPositions,
		const casacore::CoordinateSystem& csys,
		const casacore::IPosition& imShape,
		const casacore::Vector<casacore::Stokes::StokesTypes>& stokes,
		const casacore::Bool requireImageRegion=true
	);

	// implicit copy constructor and destructor are fine

	AnnPolyline& operator=(const AnnPolyline& other);

	// get the vertices converted to the coordinate system used at construction.
	casacore::Vector<casacore::MDirection> getCorners() const;

	// get the world coordinates of the polygon vertices
	void worldVertices(std::vector<casacore::Quantity>& x, std::vector<casacore::Quantity>& y) const;

	// get the pixel coordinates of the polygon vertices
	void pixelVertices(std::vector<casacore::Double>& x, std::vector<casacore::Double>& y) const;


	virtual std::ostream& print(std::ostream &os) const;

private:
	casacore::Vector<casacore::Quantity> _origXPos, _origYPos;

	void _init();
};

}

#endif /* ASCIIPOLYLINEREGION_H_ */
