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

#ifndef ANNOTATIONS_REGIONTEXTLIST_H
#define ANNOTATIONS_REGIONTEXTLIST_H

#include <casa/aips.h>
#include <casa/Arrays/Vector.h>
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <imageanalysis/IO/AsciiAnnotationFileLine.h>
#include <imageanalysis/IO/RegionTextParser.h>
#include <images/Regions/WCRegion.h>
#include <images/Regions/WCUnion.h>

namespace casacore{

class WCDifference;
}

namespace casa {


// <summary>
// An ordered list of annotations and comments representing an ascii region file.
// </summary>
// <author>Dave Mehringer</author>
// <use visibility=export>
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>
// <prerequisite>

// </prerequisite>

// <etymology>
// An order list of annotations and comments representing an ascii region file.
// </etymology>

// <synopsis>
// A list of regions and annotations and comments representing an ascii region file.
// See the region file format proposal attached to CAS-2285 (https://bugs.nrao.edu/browse/CAS-2285)
// </synopsis>

class RegionTextList {

public:

    // <group>
    // create an empty list which can be appended to. This constructor
    // is used for constructing an annotation list on the fly, possibly
    // to be written to a file when complete. Do not use this constructor
    // if you want to determine the final composite region.
    RegionTextList();

    // create an empty list which can be appended to. This constructor
    // is used for constructing an annotation list on the fly, possibly
    // to be written to a file when complete. It can be used to determine
    // the composite region as well but it is the caller's responsibility
    // to ensure the regions added to this object are constructed
    // in a consistent manner (eg using the same coordinate system).
    // <src>shape</src> is the image shape and is only used if
    // the first region is a difference; in that case, the all pixels in entire
    // shape are set to good initially.
    // <src>globalOverrideChans</src> override all spectral selections in the file
    // or text by using this channel selection<src>
    // <src>globalOverrideStokes</src> override all correlation selections in the file
    // or text by using this polarization selection<src>
    RegionTextList(
        const casacore::CoordinateSystem& csys, const casacore::IPosition shape
    );

    // create a list by reading it from a file.
    // An exception is thrown if the file is not in the correct
    // format or does not exist. The coordinate system is used for
    // setting defaults and reference frames to be used.
    // <src>shape</src> is the image shape and is only used if
    // the first region is a difference; in that case, the all pixels in entire
    // shape are set to good initially.
    // <src>requireImageRegion</src> is passed to the parser so that file lines are
    // added only for regions inside the image.
    RegionTextList(
        const casacore::String& filename, const casacore::CoordinateSystem& csys,
        const casacore::IPosition shape, const casacore::String& prependRegion="",
        const casacore::String& globalOverrideChans="",
        const casacore::String& globalOverrrideStokes="",
        const casacore::Int requireAtLeastThisVersion=RegionTextParser::CURRENT_VERSION,
        casacore::Bool verbose=true, casacore::Bool requireImageRegion=true
    );

    // create a list by reading it from a text string.
    // An exception is thrown if the text is not in the correct
    // format. The coordinate system is used for
    // setting defaults and reference frames to be used.
    // <src>shape</src> is the image shape and is only used if
    // the first region is a difference; in that case, the all pixels in entire
    // shape are set to good initially.
    // <src>requireImageRegion</src> is passed to the parser so that file lines are
    // added only for regions inside the image.
    RegionTextList(
        const casacore::CoordinateSystem& csys, const casacore::String& text,
        const casacore::IPosition shape,
        const casacore::String& prependRegion="",
        const casacore::String& globalOverrideChans="",
        const casacore::String& globalOverrrideStokes="",
        casacore::Bool verbose=true, casacore::Bool requireImageRegion=true
    );
    //</group>

    ~RegionTextList();

    // add a line to the end of the list
    void addLine(const AsciiAnnotationFileLine& line);

    // number of lines in the list
    casacore::uInt nLines() const;

    // get the line at the specified index
    AsciiAnnotationFileLine lineAt(const casacore::uInt i) const;

    // get all lines in the list
    inline const casacore::Vector<AsciiAnnotationFileLine>& getLines() const {
        return _lines;
    }

    std::ostream& print(std::ostream& os) const;

    // get the composite region.
    casacore::CountedPtr<const casacore::WCRegion> getRegion() const;

    // get the composite region as a region record.
    casacore::Record regionAsRecord() const;

private:
    casacore::Vector<AsciiAnnotationFileLine> _lines = {};
    std::vector<std::shared_ptr<const casacore::WCRegion>> _regions = {};
    casacore::CoordinateSystem _csys = {};
    casacore::IPosition _shape = {};
    casacore::Bool _canGetRegion = false;
    // if false, then the corresponding region is complementary to
    // the result of the previous region operations in the sequence
    std::vector<casacore::Bool> _union = {};
    // _ptrMgr is for memory management only, it holds new pointers that
    // aren't otherwise memory managed
    mutable std::vector<std::shared_ptr<const casacore::WCDifference>> _ptrMgr = {};
    mutable std::shared_ptr<const casacore::WCRegion> _composite = nullptr;
};

inline std::ostream &operator<<(std::ostream& os, const RegionTextList& list) {
    return list.print(os);
};

}

#endif /* IMAGES_ASCIIREGIONFILE_H_ */
