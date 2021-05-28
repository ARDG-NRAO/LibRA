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

#ifndef ANNOTATIONS_ANNRECTBOX_H
#define ANNOTATIONS_ANNRECTBOX_H

#include <casa/aips.h>
#include <imageanalysis/Annotations/AnnPolygon.h>

namespace casa {

// <summary>
// This class represents an annotation for rectangular (in position coordinates) region specified
// in an ascii region file as proposed in CAS-2285
// </summary>
// <author>Dave Mehringer</author>
// <use visibility=export>
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>
// <prerequisite>

// </prerequisite>

// <etymology>
// Holds the specification of an annotation for a rectangular region as specified in ASCII format.
// </etymology>

// <synopsis>
// This class represents an annotation for a rectangular region in coordinate space. In general, in order
// to preserve the region through a rotation (eg from one coordinate frame to another), all four corners
// must be tracked through the rotation. Hence, this region is effectively a four corner polygon.
// </synopsis>


class AnnRectBox: public AnnPolygon {

public:

	AnnRectBox(
		const casacore::Quantity& blcx,
		const casacore::Quantity& blcy,
		const casacore::Quantity& trcx,
		const casacore::Quantity& trcy,
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
	// blcx, blcy, trcx, and trcy
	// must be in the same frame as the csys direction coordinate.
	// is a region (not just an annotation), although this value can be changed after
	// construction.
	AnnRectBox(
		const casacore::Quantity& blcx,
		const casacore::Quantity& blcy,
		const casacore::Quantity& trcx,
		const casacore::Quantity& trcy,
		const casacore::CoordinateSystem& csys,
		const casacore::IPosition& imShape,
		const casacore::Vector<casacore::Stokes::StokesTypes>& stokes,
		const casacore::Bool requireImageRegion=true
	);

	// implicit copy constructor and destructor are fine

	AnnRectBox& operator=(const AnnRectBox& other);

	// get the blc and trc direction coords for the box.
	// The output directions will be converted from the input
	// reference frame to the reference frame of the input
	// coordinate system if necessary.
	// blc is the 0th component, trc the 1st in the returned vector.

	//casacore::Vector<casacore::MDirection> getCorners() const;

	std::ostream& print(std::ostream &os) const;

private:
	AnnotationBase::Direction _inputCorners;
};

}

#endif
