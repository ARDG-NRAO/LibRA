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

#ifndef ANNOTATIONS_ANNCENTERBOX_H
#define ANNOTATIONS_ANNCENTERBOX_H

#include <casacore/casa/aips.h>
#include <imageanalysis/Annotations/AnnPolygon.h>

namespace casa {

// <summary>
// This class represents a annotation for rectangular (in position coordinates) region specified
// in an ascii region file as proposed in CAS-2285. It is specified by its center position
// and side widths.
// </summary>
// <author>Dave Mehringer</author>
// <use visibility=export>
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>
// <prerequisite>

// </prerequisite>

// <etymology>
// Holds the specification of an annotation of a rectangular region as specified in ASCII format.
// Specified by center position and widths of sides.
// </etymology>

// <synopsis>
// This class represents an annotation of rectangular region in coordinate space specified by
// center and widths of sides. In general, in order
// to preserve the region through a rotation (eg from one coordinate frame to another), all four corners
// must be tracked through the rotation. Hence, this region is effectively a four corner polygon.
// </synopsis>


class AnnCenterBox: public AnnPolygon {

public:

	AnnCenterBox(
		const casacore::Quantity& centerx,
		const casacore::Quantity& centery,
		const casacore::Quantity& xwidth,
		const casacore::Quantity& ywidth,
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
	// centerx and centery
	// must be in the same frame as the csys direction coordinate.
	// is a region (not just an annotation), although this value can be changed after
	// construction.
	AnnCenterBox(
		const casacore::Quantity& centerx,
		const casacore::Quantity& centery,
		const casacore::Quantity& xwidth,
		const casacore::Quantity& ywidth,
		const casacore::CoordinateSystem& csys,
		const casacore::IPosition& imShape,
		const casacore::Vector<casacore::Stokes::StokesTypes>& stokes,
		const casacore::Bool requireImageRegion=true
	);

	// implicit copy constructor and destructor are fine

	AnnCenterBox& operator=(const AnnCenterBox& other);

	std::ostream& print(std::ostream &os) const;

private:
	casacore::Quantity _inpXCenter, _inpYCenter, _inpXWidth, _inpYWidth;

};

}

#endif /* ASCIICENTERBOXREGION_H_ */
