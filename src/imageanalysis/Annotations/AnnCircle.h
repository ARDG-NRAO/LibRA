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

#ifndef ANNOTATIONS_ANNCIRCLE_H
#define ANNOTATIONS_ANNCIRCLE_H

#include <casacore/casa/aips.h>
#include <imageanalysis/Annotations/AnnRegion.h>

namespace casa {

// <summary>
// This class represents an annotation for a circular (in position coordinates) region specified
// in an ascii region file as proposed in CAS-2285. It is specified by its center position
// and radius.
// </summary>
// <author>Dave Mehringer</author>
// <use visibility=export>
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>
// <prerequisite>

// </prerequisite>

// <etymology>
// Holds the specification of an annotation of a circular region as specified in ASCII format.
// Specified by center position and radius.
// </etymology>

// <synopsis>
// This class represents an annotation of a circular region in coordinate space specified by
// center and radius. Note that this represents a world coordinate region, even if the input
// radius is in pixels. If the direction coordinate does not have square pixels, then
// AnnEllipse should be used instead.
// </synopsis>

class AnnCircle: public AnnRegion {

public:

	AnnCircle(
		const casacore::Quantity& xcenter,
		const casacore::Quantity& ycneter,
		const casacore::Quantity& radius,
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
	// xcenter and ycenter
	// must be in the same frame as the csys direction coordinate.
	// is a region (not just an annotation), although this value can be changed after
	// construction.
	AnnCircle(
		const casacore::Quantity& xcenter,
		const casacore::Quantity& ycneter,
		const casacore::Quantity& radius,
		const casacore::CoordinateSystem& csys,
		const casacore::IPosition& imShape,
		const casacore::Vector<casacore::Stokes::StokesTypes>& stokes,
		const casacore::Bool requireImageRegion=true
	);

	// implicit copy constructor and destructor are fine

	AnnCircle& operator=(const AnnCircle& other);

	// get center position, converted to the reference frame
	// of the coordinate system if necessary
	casacore::MDirection getCenter() const;

	// get the radius of the circle with angular units
	casacore::Quantity getRadius() const;

	virtual std::ostream& print(std::ostream &os) const;

private:
	AnnotationBase::Direction _inputCenter;
	casacore::Quantity _inputRadius, _convertedRadius;

	void _init(const casacore::Quantity& xcenter, const casacore::Quantity& ycenter);

};

}

#endif /* ASCIICIRCLEREGION_H_ */
