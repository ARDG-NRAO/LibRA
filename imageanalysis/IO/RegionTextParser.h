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

#ifndef IMAGES_ASCIIANNOTATIONFILEPARSER_H
#define IMAGES_ASCIIANNOTATIONFILEPARSER_H

#include <casa/aips.h>
#include <casa/Arrays/Vector.h>
#include <casa/Containers/Record.h>
#include <casa/Logging/LogIO.h>
#include <casa/OS/RegularFile.h>
#include <casa/Utilities/Regex.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <imageanalysis/Annotations/AnnotationBase.h>
#include <imageanalysis/IO/AsciiAnnotationFileLine.h>


#include <coordinates/Coordinates/CoordinateSystem.h>

namespace casa {

// <summary>
// Parse and store regions and annotations from an ascii region file
// </summary>
// <author>Dave Mehringer</author>
// <use visibility=export>
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>
// <prerequisite>

// </prerequisite>

// <etymology>
// This is a class designed to parse and store regions and annotations from an ascii region file
// </etymology>

// <synopsis>
// This class is for parsing and storing regions and annotations from an ascii region (CRTF) file.
// The format spec can be found at
// https://casa.nrao.edu/casadocs-devel/stable/imaging/image-analysis/region-file-format
// </synopsis>

class RegionTextParser {

public:

    static const casacore::Int CURRENT_VERSION;
    static const casacore::Regex MAGIC;

    // because of nonstandard access patterns, be careful when using ParamValue and ParamSet
    // outside this class. These should probably be made into full fledged classes at some
    // point.
    struct ParamValue {
        casacore::Double doubleVal;
        casacore::Int intVal;
        casacore::String stringVal;
        casacore::Bool boolVal;
        AnnotationBase::LineStyle lineStyleVal;
        AnnotationBase::FontStyle fontStyleVal;
        std::shared_ptr<std::pair<casacore::MFrequency, casacore::MFrequency> > freqRange;
        casacore::Vector<casacore::Stokes::StokesTypes> stokes;
        AnnotationBase::RGB color;
        std::vector<casacore::Int> intVec;
    };

    using ParamSet = std::map<AnnotationBase::Keyword, ParamValue>;

    RegionTextParser() = delete;

    // <group>
    // differentiating between the filename and simple text constructors
    // <src>globalOverrideChans</src> override all spectral selections in the file
    // or text by using this channel selection<src>
    // <src>globalOverrideStokes</src> override all correlation selections in the file
    // or text by using this polarization selection<src>
    // <src>prependRegion</src> allows one to specify region(s) that will be prepended to
    // any text in <src>filename</src> or <src>text</src>
	// <src>requireImageRegion</src> is passed to the AnnRegion constructors
	// to indicate whether to rethrow ToLCRegionConversionError exception when
	// the region is outside the image lattice, or create the object even if
	// the ImageRegion has no lattice region. The default (true) rethrows the exception.
	// CAS-12631: added for CARTA, which can import regions outside an image.
    RegionTextParser(
        const casacore::String& filename, const casacore::CoordinateSystem& csys,
        const casacore::IPosition& imShape, const casacore::Int requireAtLeastThisVersion,
        const casacore::String& prependRegion="",
        const casacore::String& globalOverrideChans="",
        const casacore::String& globalOverrrideStokes="",
        casacore::Bool verbose=true,
        casacore::Bool requireImageRegion=true
    );

    RegionTextParser(
        const casacore::CoordinateSystem& csys,
        const casacore::IPosition& imShape,
        const casacore::String& text,
        const casacore::String& prependRegion="",
        const casacore::String& globalOverrideChans="",
        const casacore::String& globalOverrrideStokes="",
        casacore::Bool verbose=true,
        casacore::Bool requireImageRegion=true
    );
    //</group>

    ~RegionTextParser();

    RegionTextParser& operator=(const RegionTextParser&) = delete;

    casacore::Int getFileVersion() const;

    std::vector<AsciiAnnotationFileLine> getLines() const;

    // get the parameter set from a line of <src>text</src>. <src>preamble</src> is prepended to exception messages.
    static ParamSet getParamSet(
        casacore::Bool& spectralParmsUpdated,
        casacore::LogIO& log, const casacore::String& text, const casacore::String& preamble,
        const casacore::CoordinateSystem& csys,
        std::shared_ptr<std::pair<casacore::MFrequency, casacore::MFrequency> > overridingFreqRange,
        std::shared_ptr<casacore::Vector<casacore::Stokes::StokesTypes> > overridingCorrRange
    );

private:

    const static casacore::String sOnePair;
    const static casacore::String bTwoPair;
    const static casacore::String sNPair;
    const static casacore::Regex startOnePair;
    const static casacore::Regex startNPair;

    casacore::CoordinateSystem _csys;
    std::unique_ptr<casacore::LogIO> _log;
    ParamSet _currentGlobals;
    std::vector<AsciiAnnotationFileLine> _lines;
    casacore::Vector<AnnotationBase::Keyword> _globalKeysToApply;
    casacore::Int _fileVersion;
    casacore::IPosition _imShape;
    casacore::uInt _regions;
    casacore::Bool _verbose = true;

    std::shared_ptr<std::pair<casacore::MFrequency, casacore::MFrequency> > _overridingFreqRange;

    std::shared_ptr<casacore::Vector<casacore::Stokes::StokesTypes> > _overridingCorrRange;

    void _parse(const casacore::String& contents, const casacore::String& fileDesc, casacore::Bool requireImageRegion);

    casacore::Array<casacore::String> _extractTwoPairs(casacore::uInt& end, const casacore::String& string) const;

    // extract s1 and s2 from a string of the form "[s1, s2]"
    static casacore::Vector<casacore::String> _extractSinglePair(const casacore::String& string);

    void _addLine(const AsciiAnnotationFileLine& line);

    AnnotationBase::Type _getAnnotationType(
        casacore::Vector<casacore::Quantity>& qDirs,
        std::vector<casacore::Quantity>& quantities,
        casacore::String& textString,
        casacore::String& consumeMe, const casacore::String& preamble
    ) const;

    ParamSet _getCurrentParamSet(
        casacore::Bool& spectralParmsUpdated, ParamSet& newParams,
        casacore::String& consumeMe, const casacore::String& preamble
    ) const;

    void _createAnnotation(
        const AnnotationBase::Type annType,
        //const casacore::Vector<casacore::MDirection> dirs,
        const casacore::Vector<casacore::Quantity>& qDirs,
        const std::pair<casacore::Quantity, casacore::Quantity>& qFreqs,
        const std::vector<casacore::Quantity>& quantities,
        const casacore::String& textString,
        const ParamSet& currentParamSet,
        const casacore::Bool annOnly, const casacore::Bool isDifference,
        const casacore::String& preamble, casacore::Bool requireImageRegion
    );

    std::pair<casacore::Quantity, casacore::Quantity> _quantitiesFromFrequencyString(
        const casacore::String& freqString, const casacore::String& preamble
    ) const;

    static casacore::String _doLabel(casacore::String& consumeMe, const casacore::String& logPreamble);

    static casacore::String _getKeyValue(casacore::String& consumeMe, const casacore::String& preamble);

    casacore::Vector<casacore::Quantity> _extractQuantityPairAndSingleQuantity(
        casacore::String& consumeMe, const casacore::String& preamble
    ) const;

    casacore::Vector<casacore::Quantity> _extractNQuantityPairs(
            casacore::String& consumeMe, const casacore::String& preamble
    ) const;

    casacore::Vector<casacore::Quantity> _extractTwoQuantityPairs(
        casacore::String& consumeMe, const casacore::String& preamble
    ) const;

    std::pair<casacore::Quantity, casacore::Quantity> _extractSingleQuantityPair(
        const casacore::String& pair, const casacore::String& preamble
    ) const;

    void _setInitialGlobals();

    static casacore::Vector<casacore::Stokes::StokesTypes> _stokesFromString(
        const casacore::String& stokes, const casacore::String& preamble
    );

    casacore::Vector<casacore::Quantity> _extractTwoQuantityPairsAndSingleQuantity(
        casacore::String& consumeMe, const casacore::String& preamble
    ) const;

    void _extractQuantityPairAndString(
        std::pair<casacore::Quantity, casacore::Quantity>& quantities, casacore::String& string,
        casacore::String& consumeMe, const casacore::String& preamble,
        const casacore::Bool requireQuotesAroundString
    ) const;

    casacore::Vector<casacore::Quantity> _extractQuantitiesFromPair(
        const casacore::String& pair, const casacore::String& preamble
    ) const;

    void _determineVersion(
        const casacore::String& chunk, const casacore::String& filename,
        const casacore::Int requireAtLeastThisVersion
    );

    // set the casacore::Stokes/polarizations/correlations that will override all global and per line correlation
    // specifications. If multiple ranges are specified, an exception will be thrown.
    void _setOverridingCorrelations(const casacore::String& globalOverrideStokes);

    // set the (single) channel range that will override all global and per line frequency
    // specifications. If multiple ranges are specified, an exception will be thrown.
    void _setOverridingChannelRange(const casacore::String& globalOverrideChans);

};
}

#endif
