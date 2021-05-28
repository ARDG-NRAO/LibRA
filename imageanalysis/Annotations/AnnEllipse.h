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

#ifndef ANNOTATIONS_ANNELLIPSE_H
#define ANNOTATIONS_ANNELLIPSE_H

#include <imageanalysis/Annotations/AnnRegion.h>


#include <casa/aips.h>

namespace casa {

// <summary>
// This class represents an annotation of an elliptical (in position coordinates) region specified
// in an ascii region file as proposed in CAS-2285. It is specified by its center position
// semi-major and semi-minor axes, and position angle.
// </summary>
// <author>Dave Mehringer</author>
// <use visibility=export>
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>
// <prerequisite>

// </prerequisite>

// <etymology>
// Holds the specification of a an annotation of an elliptical region as specified in ASCII format.
// Specified by center position, semi-major and semi-minor axes, and position angle.
// </etymology>

// <synopsis>
// This class represents an annotation of an elliptical region in coordinate space specified by
// center, semi-major and semi-minor axes, and position angle
// </synopsis>

class AnnEllipse: public AnnRegion {

public:

	// <src>positionAngle</src> is defined as the angle between north and the
	// ellipse major axis. Note the lengths are for the semi-major and semi-minor axes,
	// not the major and minor axes
	// <group>

	AnnEllipse(
		const casacore::Quantity& xcenter, const casacore::Quantity& ycenter,
		const casacore::Quantity& semiMajorAxis,
		const casacore::Quantity& semiMinorAxis, const casacore::Quantity& positionAngle,
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
	AnnEllipse(
		const casacore::Quantity& xcenter, const casacore::Quantity& ycenter,
		const casacore::Quantity& semiMajorAxis,
		const casacore::Quantity& semiMinorAxis, const casacore::Quantity& positionAngle,
		const casacore::CoordinateSystem& csys,
		const casacore::IPosition& imShape,
		const casacore::Vector<casacore::Stokes::StokesTypes>& stokes,
		const casacore::Bool requireImageRegion=true
	);

	// implicit copy constructor and destructor are fine

	//</group>

	AnnEllipse& operator=(const AnnEllipse& other);

	casacore::Bool operator==(const AnnEllipse& other) const;

	// Get the center position, tranformed to the reference
	// from of the coordinate system if necessary
	casacore::MDirection getCenter() const;

	// get semi-major axis. The quantity will have units
	// of angular measure
	casacore::Quantity getSemiMajorAxis() const;

	// get semi-minor axis. The quantity will have units
	// of angular measure
	casacore::Quantity getSemiMinorAxis() const;

	// get position angle. The quantity will have units
	// of angular measure and will be measured from north through east,
	// using the normal astronomical convention.
	casacore::Quantity getPositionAngle() const;

	virtual std::ostream& print(std::ostream &os) const;


private:
	AnnotationBase::Direction _inputCenter;
	casacore::Quantity _inputSemiMajorAxis, _inputSemiMinorAxis, _inputPositionAngle,
		_convertedSemiMajorAxis, _convertedSemiMinorAxis,
		_convertedPositionAngle;

	void _init(const casacore::Quantity& xcenter, const casacore::Quantity& ycenter);
};

}

#endif /* ASCIIELLIPSEREGION_H_ */
