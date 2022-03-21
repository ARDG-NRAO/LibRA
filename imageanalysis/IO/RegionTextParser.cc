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

#include <imageanalysis/IO/RegionTextParser.h>

#include <casa/IO/RegularFileIO.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <imageanalysis/Annotations/AnnAnnulus.h>
#include <imageanalysis/Annotations/AnnCenterBox.h>
#include <imageanalysis/Annotations/AnnCircle.h>
#include <imageanalysis/Annotations/AnnEllipse.h>
#include <imageanalysis/Annotations/AnnLine.h>
#include <imageanalysis/Annotations/AnnPolygon.h>
#include <imageanalysis/Annotations/AnnRectBox.h>
#include <imageanalysis/Annotations/AnnRotBox.h>
#include <imageanalysis/Annotations/AnnSymbol.h>
#include <imageanalysis/Annotations/AnnText.h>
#include <imageanalysis/Annotations/AnnVector.h>
#include <imageanalysis/IO/ParameterParser.h>

#include <measures/Measures/MCDirection.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/VelocityMachine.h>

#include <iomanip>
#include <casa/BasicSL/STLIO.h>

#define _ORIGIN "RegionTextParser::" + String(__FUNCTION__) + ": "

using namespace casacore;
namespace casa {

const Int RegionTextParser::CURRENT_VERSION = 0;
const Regex RegionTextParser::MAGIC("^#CRTF");

const String RegionTextParser::sOnePair = "[[:space:]]*\\[[^\\[,]+,[^\\[,]+\\][[:space:]]*";
const String RegionTextParser::bTwoPair = "\\[" + sOnePair
        + "," + sOnePair;
// explicit onePair at the end because that one should not be followed by a comma
const String RegionTextParser::sNPair = "\\[(" + sOnePair
        + ",)+" + sOnePair + "\\]";
const Regex RegionTextParser::startOnePair("^" + sOnePair);
const Regex RegionTextParser::startNPair("^" + sNPair);

RegionTextParser::RegionTextParser(
    const String& filename, const CoordinateSystem& csys,
    const IPosition& imShape,
    const Int requireAtLeastThisVersion,
    const String& prependRegion,
    const String& globalOverrideChans, const String& globalOverrrideStokes,
    Bool verbose, bool requireImageRegion
) : _csys(csys), _log(new LogIO()), _currentGlobals(),
    _lines(), _globalKeysToApply(), _fileVersion(-1), _imShape(imShape),
    _regions(0), _verbose(verbose) {
    RegularFile file(filename);
    if (! file.exists()) {
        throw AipsError(
            _ORIGIN + "File " + filename + " does not exist."
        );
    }
    if (! file.isReadable()) {
        throw AipsError(
            _ORIGIN + "File " + filename + " is not readable."
        );
    }
    if (! _csys.hasDirectionCoordinate()) {
        throw AipsError(
            _ORIGIN
            + "Coordinate system does not have a direction coordinate"
        );
    }
    _setInitialGlobals();
    _setOverridingChannelRange(globalOverrideChans);
    _setOverridingCorrelations(globalOverrrideStokes);
    RegularFileIO fileIO(file);
    Int bufSize = 4096;
    PtrHolder<char> buffer(new char[bufSize], true);
    int nRead;
    String contents;
    if (! prependRegion.empty()) {
        contents = prependRegion + "\n";
    }
    while ((nRead = fileIO.read(bufSize, buffer, false)) == bufSize) {
        String chunk(buffer, bufSize);
        if (_fileVersion < 0) {
            _determineVersion(chunk, filename, requireAtLeastThisVersion);
        }
        contents += chunk;
    }
    // get the last chunk
    String chunk(buffer, nRead);
    if (_fileVersion < 0) {
        _determineVersion(chunk, filename, requireAtLeastThisVersion);
    }
    contents += chunk;
    _parse(contents, filename, requireImageRegion);
}

RegionTextParser::RegionTextParser(
    const CoordinateSystem& csys, const IPosition& imShape,
    const String& text, const String& prependRegion,
    const String& globalOverrideChans, const String& globalOverrrideStokes,
    Bool verbose, Bool requireImageRegion
) : _csys(csys), _log(new LogIO()), _currentGlobals(), _lines(),
    _globalKeysToApply(), _fileVersion(-1), _imShape(imShape), _regions(0),
    _verbose(verbose) {
    if (! _csys.hasDirectionCoordinate()) {
        throw AipsError(
            _ORIGIN + "Coordinate system has no direction coordinate"
        );
    }
    _setInitialGlobals();
    _setOverridingChannelRange(globalOverrideChans);
    _setOverridingCorrelations(globalOverrrideStokes);
    _parse(prependRegion.empty() ? text : prependRegion + "\n" + text, "", requireImageRegion);
}

RegionTextParser::~RegionTextParser() {}

Int RegionTextParser::getFileVersion() const {
    ThrowIf(
        _fileVersion < 0,
        "File version not associated with simple text strings"
    );
    return _fileVersion;
}

vector<AsciiAnnotationFileLine> RegionTextParser::getLines() const {
    return _lines;
}

void RegionTextParser::_determineVersion(
    const String& chunk, const String& filename,
    const Int requireAtLeastThisVersion
) {
    *_log << LogOrigin("RegionTextParser", __FUNCTION__);
    if (_fileVersion >= 0) {
        // already determined
        return;
    }
    ThrowIf(
        ! chunk.contains(MAGIC),
        _ORIGIN + "File " + filename
        + " does not contain CASA region text file magic value"
    );
    Regex version(MAGIC.regexp() + "v[0-9]+");
    if (chunk.contains(version)) {
        const auto vString = chunk.substr(6);
        auto done = false;
        auto count = 1;
        auto oldVersion = -2000;
        while (! done) {
            try {
                _fileVersion = String::toInt(vString.substr(0, count));
                ++count;
                if (_fileVersion == oldVersion) {
                    done = true;
                }
                else {
                    oldVersion = _fileVersion;
                }
            }
            catch (const AipsError&) {
                done = true;
            }
        }
        if (_fileVersion < requireAtLeastThisVersion) {
            *_log << _ORIGIN << "File version " << _fileVersion
                << " is less than required version "
                << requireAtLeastThisVersion << LogIO::EXCEPTION;
        }
        if (_fileVersion > CURRENT_VERSION) {
            *_log << _ORIGIN << "File version " << _fileVersion
                << " is greater than the most recent version of the spec ("
                << CURRENT_VERSION
                << "). Did you bring this file with you when you traveled "
                << "here from the future perhaps? Unfortunately we don't "
                << "support such possibilities yet." << LogIO::EXCEPTION;
        }
        *_log << LogIO::NORMAL << _ORIGIN << "Found spec version "
            << _fileVersion << LogIO::POST;
    }
    else {
        *_log << LogIO::WARN << _ORIGIN << "File " << filename
            << " does not contain a CASA Region Text File spec version. "
            << "The current spec version, " << CURRENT_VERSION << " will be assumed. "
            << "WE STRONGLY SUGGEST YOU INCLUDE THE SPEC VERSION IN THIS FILE TO AVOID "
            << "POSSIBLE FUTURE BACKWARD INCOMPATIBILTY ISSUES. Simply ensure the first line "
            << "of the file is '#CRTFv" << CURRENT_VERSION << "'" << LogIO::POST;
        _fileVersion = CURRENT_VERSION;
    }
}

void RegionTextParser::_parse(const String& contents, const String& fileDesc, bool requireImageRegion) {
    _log->origin(LogOrigin("AsciiRegionFileParser", __func__));
    static const Regex startAnn("^ann[[:space:]]+");
    static const Regex startDiff("^-[[:space:]]*");
    static const Regex startGlobal("^global[[:space:]]+");
    AnnotationBase::unitInit();
    auto lines = stringToVector(contents, '\n');
    uInt lineCount = 0;
    auto qFreqs = _overridingFreqRange
        ? std::pair<Quantity, Quantity>(
            Quantity(_overridingFreqRange->first.getValue().getValue(), "Hz"),
            Quantity(_overridingFreqRange->second.getValue().getValue(), "Hz")
        )
        : std::pair<Quantity, Quantity>(Quantity(0), Quantity(0));
    for(auto iter=lines.cbegin(); iter!=lines.cend(); ++iter) {
        ++lineCount;
        Bool annOnly = false;
        ostringstream preambleoss;
        preambleoss << fileDesc + " line# " << lineCount << ": ";
        const auto preamble = preambleoss.str();
        Bool difference = false;
        iter->trim();
        if (
            iter->empty() || iter->startsWith("#")
        ) {
            // ignore comments and blank lines
            _addLine(AsciiAnnotationFileLine(*iter));
            continue;
        }
        auto consumeMe = *iter;
        // consumeMe.downcase();
        Bool spectralParmsUpdated;
        ParamSet newParams;
        if (consumeMe.contains(startDiff)) {
            difference = true;
            // consume the difference character to allow further processing of string
            consumeMe.del(0, 1);
            consumeMe.trim();
            *_log << LogIO::NORMAL << preamble << "difference found" << LogIO::POST;
        }
        else if(consumeMe.contains(startAnn)) {
            annOnly = true;
            // consume the annotation chars
            consumeMe.del(0, 3);
            consumeMe.trim();
            *_log << LogIO::NORMAL << preamble << "annotation only found" << LogIO::POST;
        }
        else if(consumeMe.contains(startGlobal)) {
            consumeMe.del(0, 6);
            _currentGlobals = _getCurrentParamSet(
                spectralParmsUpdated, newParams,
                consumeMe, preamble
            );
            std::map<AnnotationBase::Keyword, String> gParms;
            for (const auto& p: newParams) {
                gParms[p.first] = p.second.stringVal;
            }
            _addLine(AsciiAnnotationFileLine(gParms));
            if (
                _csys.hasSpectralAxis() && spectralParmsUpdated
                && newParams.find(AnnotationBase::RANGE) != newParams.end()
            ) {
                qFreqs = _quantitiesFromFrequencyString(
                    newParams[AnnotationBase::RANGE].stringVal, preamble
                );
            }
            *_log << LogIO::NORMAL << preamble << "global found" << LogIO::POST;
            continue;
        }
        // now look for per-line shapes and annotations
        Vector<Quantity> qDirs;
        vector<Quantity> quantities;
        String textString;
        const auto annType = _getAnnotationType(
            qDirs, quantities, textString, consumeMe, preamble
        );
        ParamSet currentParamSet = _getCurrentParamSet(
            spectralParmsUpdated, newParams, consumeMe, preamble
        );
        if (
            newParams.find(AnnotationBase::LABEL) == newParams.end()
            || newParams[AnnotationBase::LABEL].stringVal.empty()
        ) {
            if (newParams.find(AnnotationBase::LABELCOLOR) != newParams.end()) {
                *_log << LogIO::WARN << preamble
                    << "Ignoring labelcolor because there is no associated label specified"
                    << LogIO::POST;
            }
            if (newParams.find(AnnotationBase::LABELPOS) != newParams.end()) {
                *_log << LogIO::WARN << preamble
                    << "Ignoring labelpos because there is no associated label specified"
                    << LogIO::POST;
            }
            if (newParams.find(AnnotationBase::LABELOFF) != newParams.end()) {
                *_log << LogIO::WARN << preamble
                    << "Ignoring labeloff because there is no associated label specified"
                    << LogIO::POST;
            }
        }
        else if (newParams.find(AnnotationBase::LABELCOLOR) == newParams.end()) {
            // if a label is specified but no label color is specified, the labelcolor
            // is to be the same as the annotation color
            newParams[AnnotationBase::LABELCOLOR] = newParams[AnnotationBase::COLOR];
        }
        if (_csys.hasSpectralAxis()) {
            if(spectralParmsUpdated) {
                qFreqs = _quantitiesFromFrequencyString(
                    currentParamSet[AnnotationBase::RANGE].stringVal, preamble
                );
            }
            else if(
                _currentGlobals.find(AnnotationBase::RANGE)
                == _currentGlobals.end()
                || ! _currentGlobals.at(AnnotationBase::RANGE).freqRange
            ) {
                // no global frequency range, so use entire freq span
                qFreqs = std::pair<Quantity, Quantity>(Quantity(0), Quantity(0));
            }
        }
        auto globalsLessLocal = _currentGlobals;
        for (
            auto iter=newParams.cbegin();
            iter != newParams.cend(); ++iter
        ) {
            AnnotationBase::Keyword key = iter->first;
            if (globalsLessLocal.find(key) != globalsLessLocal.end()) {
                globalsLessLocal.erase(key);
            }
        }
        _globalKeysToApply.resize(globalsLessLocal.size(), false);
        uInt i = 0;
        for (
            ParamSet::const_iterator iter=globalsLessLocal.begin();
            iter != globalsLessLocal.end(); ++iter
        ) {
            _globalKeysToApply[i] = iter->first;
            ++i;
        }
        _createAnnotation(
            annType, qDirs, qFreqs, quantities, textString,
            currentParamSet, annOnly, difference, preamble,
            requireImageRegion
        );
    }
    if (_verbose) {
        *_log << LogIO::NORMAL << "Combined " << _regions
            << " image regions (which excludes any annotation regions)" << LogIO::POST;
        if (_regions > 0) {
            *_log << LogIO::NORMAL << "The specified region will select all pixels that are "
                << "included in the region. Full pixels will be included even when they are "
                << "only partially covered by the region(s)." << LogIO::POST;
        }
    }
}

void RegionTextParser::_addLine(const AsciiAnnotationFileLine& line) {
    _lines.push_back(line);
}

AnnotationBase::Type RegionTextParser::_getAnnotationType(
    Vector<Quantity>& qDirs,
    vector<Quantity>& quantities,
    String& textString,
    String& consumeMe, const String& preamble
) const {
    const static String sOnePairOneSingle =
        "\\[" + sOnePair + ",[^\\[,]+\\]";
    const static String sOnePairAndText =
        "\\[" + sOnePair + ",[[:space:]]*[\"\'].*[\"\'][[:space:]]*\\]";
    const static String sTwoPair = bTwoPair + "\\]";
    const static Regex startTwoPair("^" + sTwoPair);
    const static Regex startOnePairAndText("^" + sOnePairAndText);
    const static String sTwoPairOneSingle = bTwoPair
        + ",[[:space:]]*[^\\[,]+[[:space:]]*\\]";
    const static Regex startTwoPairOneSingle("^" + sTwoPairOneSingle);
    const static Regex startOnePairOneSingle("^" + sOnePairOneSingle);
    consumeMe.trim();
    String tmp = consumeMe.through(Regex("[[:alpha:]]+"));
    consumeMe.del(0, (Int)tmp.length());
    consumeMe.trim();
    auto annotationType = AnnotationBase::typeFromString(tmp);
    std::pair<Quantity, Quantity> myPair;
    switch(annotationType) {
    case AnnotationBase::RECT_BOX:
        ThrowIf(
            ! consumeMe.contains(startTwoPair),
            preamble + "Illegal box specification "
        );
        qDirs = _extractNQuantityPairs(consumeMe, preamble);

        if (qDirs.size() != 4) {
            throw AipsError(preamble
                + "rectangle box spec must contain exactly 2 direction pairs but it has "
                + String::toString(qDirs.size())
            );
        }
        break;
    case AnnotationBase::CENTER_BOX:
        ThrowIf(
            ! consumeMe.contains(startTwoPair),
            preamble + "Illegal center box specification " + consumeMe
        );
        qDirs.resize(2);
        quantities.resize(2);
        {
            Vector<Quantity> qs = _extractNQuantityPairs(consumeMe, preamble);
            qDirs[0] = qs[0];
            qDirs[1] = qs[1];
            quantities[0] = qs[2];
            quantities[1] = qs[3];
        }
        break;
    case AnnotationBase::ROTATED_BOX:
        ThrowIf(
            ! consumeMe.contains(startTwoPairOneSingle),
            preamble + "Illegal rotated box specification " + consumeMe
        );
        qDirs.resize(2);
        quantities.resize(3);
        {
            Vector<Quantity> qs = _extractTwoQuantityPairsAndSingleQuantity(consumeMe, preamble);
            qDirs[0] = qs[0];
            qDirs[1] = qs[1];
            quantities[0] = qs[2];
            quantities[1] = qs[3];
            quantities[2] = qs[4];
        }
        break;
    case AnnotationBase::POLYGON:
        // Polygon definitions can be very long with many points.
        // Testing entire polygon string syntax causes regex seg fault.
        ThrowIf(
            ! (
               consumeMe.contains(Regex("^ *\\[ *\\["))
               && consumeMe.contains(Regex("\\] *\\]"))
            ), preamble + "Illegal polygon specification " + consumeMe
        );
        {
            Vector<Quantity> qs = _extractNQuantityPairs(consumeMe, preamble);
            qDirs.resize(qs.size());
            qDirs = qs;
        }
        break;
    case AnnotationBase::CIRCLE:
        ThrowIf(
            ! consumeMe.contains(startOnePairOneSingle),
            preamble + "Illegal circle specification " + consumeMe
        );
        qDirs.resize(2);
        quantities.resize(1);
        {
            Vector<Quantity> qs = _extractQuantityPairAndSingleQuantity(
                consumeMe, preamble
            );
            qDirs[0] = qs[0];
            qDirs[1] = qs[1];
            quantities[0] = qs[2];
        }
        break;
    case AnnotationBase::ANNULUS:
        ThrowIf(
            ! consumeMe.contains(startTwoPair),
            preamble + "Illegal annulus specification " + consumeMe
        );
        qDirs.resize(2);
        quantities.resize(2);
        {
            Vector<Quantity> qs = _extractNQuantityPairs(
                consumeMe, preamble
            );
            qDirs[0] = qs[0];
            qDirs[1] = qs[1];
            quantities[0] = qs[2];
            quantities[1] = qs[3];
        }
        break;
    case AnnotationBase::ELLIPSE:
        if (! consumeMe.contains(startTwoPairOneSingle)) {
            *_log << preamble << "Illegal ellipse specification "
                << consumeMe << LogIO::EXCEPTION;
        }
        qDirs.resize(2);
        quantities.resize(3);
        {
            Vector<Quantity> qs = _extractTwoQuantityPairsAndSingleQuantity(
                consumeMe, preamble
            );
            qDirs[0] = qs[0];
            qDirs[1] = qs[1];
            quantities[0] = qs[2];
            quantities[1] = qs[3];
            quantities[2] = qs[4];
        }
        break;
    case AnnotationBase::LINE:
        if (! consumeMe.contains(startTwoPair)) {
            *_log << preamble << "Illegal line specification "
                << consumeMe << LogIO::EXCEPTION;
        }
        qDirs.resize(4);
        qDirs = _extractNQuantityPairs(consumeMe, preamble);
        if (qDirs.size() != 4) {
            throw AipsError(preamble
                + "line spec must contain exactly 2 direction pairs but it has "
                + String::toString(qDirs.size())
            );
        }
        break;
    case AnnotationBase::VECTOR:
        if (! consumeMe.contains(startTwoPair)) {
            *_log << preamble << "Illegal vector specification "
                << consumeMe << LogIO::EXCEPTION;
        }
        qDirs.resize(4);
        qDirs = _extractNQuantityPairs(consumeMe, preamble);
        if (qDirs.size() != 4) {
            throw AipsError(preamble
                + "line spec must contain exactly 2 direction pairs but it has "
                + String::toString(qDirs.size())
            );
        }
        break;
    case AnnotationBase::TEXT:
        if (! consumeMe.contains(startOnePairAndText)) {
            *_log << preamble << "Illegal text specification "
                << consumeMe << LogIO::EXCEPTION;
        }
        qDirs.resize(2);
        _extractQuantityPairAndString(
            myPair, textString, consumeMe, preamble, true
        );
        qDirs[0] = myPair.first;
        qDirs[1] = myPair.second;
        break;
    case AnnotationBase::SYMBOL:
        if (! consumeMe.contains(startOnePairOneSingle)) {
            *_log << preamble << "Illegal symbol specification "
                << consumeMe << LogIO::EXCEPTION;
        }
        qDirs.resize(2);
        _extractQuantityPairAndString(
            myPair, textString, consumeMe, preamble, false
        );
        qDirs[0] = myPair.first;
        qDirs[1] = myPair.second;
        textString.trim();
        if (textString.length() > 1) {
            throw AipsError(
                preamble
                    + ": A symbol is defined by a single character. The provided string ("
                    + textString
                    + ") has more than one"
            );
        }
        break;
    default:
        ThrowCc(
            preamble + "Unable to determine annotation type"
        );
    }
    return annotationType;
}

RegionTextParser::ParamSet RegionTextParser::getParamSet(
    Bool& spectralParmsUpdated, LogIO& log,
    const String& text, const String& preamble,
    const CoordinateSystem& csys,
    std::shared_ptr<std::pair<MFrequency, MFrequency> > overridingFreqRange,
    std::shared_ptr<Vector<Stokes::StokesTypes> > overridingCorrRange
) {
    ParamSet parms;
    spectralParmsUpdated = false;
    auto consumeMe = text;
    // get key-value pairs on the line
    while (consumeMe.size() > 0) {
        ParamValue paramValue;
        auto key = AnnotationBase::UNKNOWN_KEYWORD;
        consumeMe.trim();
        consumeMe.ltrim(',');
        consumeMe.trim();
        ThrowIf(
            ! consumeMe.contains('='),
            preamble + "Illegal extra characters on line ("
                + consumeMe + "). Did you forget a '='?"
        );
        const auto equalPos = consumeMe.find('=');
        auto keyword = consumeMe.substr(0, equalPos);
        keyword.trim();
        keyword.downcase();
        consumeMe.del(0, (Int)equalPos + 1);
        consumeMe.trim();
        if (keyword == "label") {
            key = AnnotationBase::LABEL;
            paramValue.stringVal = _doLabel(consumeMe, preamble);
        }
        else {
            paramValue.stringVal = _getKeyValue(consumeMe, preamble);
            if (keyword == "coord") {
                key = AnnotationBase::COORD;
            }
            else if (keyword == "corr" && ! overridingCorrRange) {
                if (csys.hasPolarizationCoordinate()) {
                    key = AnnotationBase::CORR;
                    paramValue.stokes = _stokesFromString(
                        paramValue.stringVal, preamble
                    );
                }
                else {
                    log << LogIO::WARN << preamble
                        << "Keyword " << keyword << " specified but will be ignored "
                        << "because the coordinate system has no polarization axis."
                        << LogIO::POST;
                }
            }
            else if (
                ! overridingFreqRange
                && (
                    keyword == "frame" || keyword == "range"
                    || keyword == "veltype" || keyword == "restfreq"
                )
            ) {
                spectralParmsUpdated = true;
                if (! csys.hasSpectralAxis()) {
                    spectralParmsUpdated = false;
                    log << LogIO::WARN << preamble
                        << "Keyword " << keyword << " specified but will be ignored "
                        << "because the coordinate system has no spectral axis."
                        << LogIO::POST;
                }
                else if (keyword == "frame") {
                    key = AnnotationBase::FRAME;
                }
                else if (keyword == "range") {
                    key = AnnotationBase::RANGE;
                }
                else if (keyword == "veltype") {
                    key = AnnotationBase::VELTYPE;
                }
                else if (keyword == "restfreq") {
                    key = AnnotationBase::RESTFREQ;
                    Quantity qRestfreq;
                    ThrowIf(
                        ! readQuantity(qRestfreq, paramValue.stringVal),
                        "Could not convert rest frequency "
                        + paramValue.stringVal + " to quantity"
                    );
                }
            }
            else if (keyword == "linewidth") {
                key = AnnotationBase::LINEWIDTH;
                if (! paramValue.stringVal.matches(Regex("^[1-9]+$"))) {
                    log << preamble << "linewidth (" << paramValue.stringVal
                        << ") must be a positive integer but is not." << LogIO::EXCEPTION;
                }
                paramValue.intVal = String::toInt(paramValue.stringVal);
            }
            else if (keyword == "linestyle") {
                key = AnnotationBase::LINESTYLE;
                paramValue.lineStyleVal = AnnotationBase::lineStyleFromString(
                    paramValue.stringVal
                );
            }
            else if (keyword == "symsize") {
                key = AnnotationBase::SYMSIZE;
                if (! paramValue.stringVal.matches(Regex("^[1-9]+$"))) {
                    log << preamble << "symsize (" << paramValue.stringVal
                        << ") must be a positive integer but is not." << LogIO::EXCEPTION;
                }
                paramValue.intVal = String::toInt(paramValue.stringVal);
            }
            else if (keyword == "symthick") {
                key = AnnotationBase::SYMTHICK;
                if (! paramValue.stringVal.matches(Regex("^[1-9]+$"))) {
                    log << preamble << "symthick (" << paramValue.stringVal
                        << ") must be a positive integer but is not." << LogIO::EXCEPTION;
                }
                paramValue.intVal = String::toInt(paramValue.stringVal);
            }
            else if (keyword == "color") {
                key = AnnotationBase::COLOR;
            }
            else if (keyword == "font") {
                key = AnnotationBase::FONT;
            }
            else if (keyword == "fontsize") {
                key = AnnotationBase::FONTSIZE;
                paramValue.intVal = String::toInt(paramValue.stringVal);
            }
            else if (keyword == "fontstyle") {
                key = AnnotationBase::FONTSTYLE;
                paramValue.fontStyleVal = AnnotationBase::fontStyleFromString(
                    paramValue.stringVal
                );
            }
            else if (keyword == "usetex") {
                String v = paramValue.stringVal;
                v.downcase();
                key = AnnotationBase::USETEX;
                if (
                    v != "true"  && v != "t"
                    && v != "false" && v != "f"
                ) {
                    log << preamble << "Cannot determine boolean value of usetex"
                        << paramValue.stringVal << LogIO::EXCEPTION;
                }
                paramValue.boolVal = (v == "true" || v == "t");
            }
            else if (keyword == "labelcolor") {
                key = AnnotationBase::LABELCOLOR;
            }
            else if (keyword == "labelpos") {
                key = AnnotationBase::LABELPOS;
            }
            else if (keyword == "labeloff") {
                auto v = paramValue.stringVal;
                static const String sInt("[-+]?[0-9]+");
                static const Regex rInt(sInt);
                if (
                    ! v.contains(
                        Regex(
                            sInt + "[[:space:]]*,[[:space:]]*" + sInt
                        )
                    )
                ) {
                    log << preamble << "Illegal label offset specification \""
                        << v << "\"" << LogIO::EXCEPTION;
                }
                // the brackets have been stripped, add them back to make it easier
                // to parse with a method already in existence
                auto pair = _extractSinglePair("[" + v + "]");
                paramValue.intVec = vector<Int>();

                for (
                    auto iter=pair.begin();
                    iter != pair.end(); ++iter
                ) {
                    if (! iter->matches(rInt)) {
                        log << preamble << "Illegal label offset specification, "
                            << *iter << " is not an integer" << LogIO::EXCEPTION;
                    }
                    paramValue.intVec.push_back(String::toInt(*iter));
                }
                key = AnnotationBase::LABELOFF;
            }
            else {
                ThrowCc(preamble + "Unrecognized key " + keyword);
            }
        }
        consumeMe.trim();
        if (key != AnnotationBase::UNKNOWN_KEYWORD) {
            parms[key] = paramValue;
        }
    }
    return parms;
}

RegionTextParser::ParamSet
RegionTextParser::_getCurrentParamSet(
    Bool& spectralParmsUpdated, ParamSet& newParams,
    String& consumeMe, const String& preamble
) const {
    auto currentParams = _currentGlobals;
    newParams = getParamSet(
        spectralParmsUpdated,
        *_log, consumeMe, preamble, _csys, _overridingFreqRange,
        _overridingCorrRange
    );
    for (const auto& p: newParams) {
        currentParams[p.first] = p.second;
    }
    ThrowIf(
        currentParams.find(AnnotationBase::RANGE) == currentParams.end()
        && currentParams.find(AnnotationBase::FRAME) != currentParams.end(),
        preamble + "Frame specified but frequency range not specified"
    );
    ThrowIf(
        currentParams.find(AnnotationBase::RANGE) == currentParams.end()
        && currentParams.find(AnnotationBase::RESTFREQ) != currentParams.end(),
        preamble + "Rest frequency specified but velocity range not specified"
    );
    return currentParams;
}

std::pair<Quantity, Quantity> RegionTextParser::_quantitiesFromFrequencyString(
    const String& freqString, const String& preamble
) const {
    // the brackets have been stripped, add them back to make it easier
    // to parse with a method already in existence
    auto cString = "[" + freqString + "]";
    ThrowIf(! cString.contains(startOnePair),
        preamble + "Incorrect spectral range specification ("
        + freqString + ")"
    );
    return _extractSingleQuantityPair(
        cString, preamble
    );
}

void RegionTextParser::_createAnnotation(
    const AnnotationBase::Type annType,
    const Vector<Quantity>& qDirs,
    const std::pair<Quantity, Quantity>& qFreqs,
    const vector<Quantity>& quantities,
    const String& textString,
    const ParamSet& currentParamSet,
    const Bool annOnly, const Bool isDifference,
    const String& preamble, Bool requireImageRegion
) {
    CountedPtr<AnnotationBase> annotation;
    Vector<Stokes::StokesTypes> stokes(0);
    if (
        currentParamSet.find(AnnotationBase::CORR) != currentParamSet.end()
        && _csys.hasPolarizationCoordinate()
    ) {
        stokes.resize(currentParamSet.at(AnnotationBase::CORR).stokes.size());
        stokes = currentParamSet.at(AnnotationBase::CORR).stokes;
    }
    auto dirRefFrame = currentParamSet.at(AnnotationBase::COORD).stringVal;
    auto freqRefFrame = currentParamSet.find(AnnotationBase::FRAME) == currentParamSet.end()
        ? "" : currentParamSet.at(AnnotationBase::FRAME).stringVal;
    auto doppler = currentParamSet.find(AnnotationBase::VELTYPE) == currentParamSet.end()
        ? "" :    currentParamSet.at(AnnotationBase::VELTYPE).stringVal;
    Quantity restfreq;
    if (
        currentParamSet.find(AnnotationBase::RESTFREQ) != currentParamSet.end()
        && ! readQuantity(
            restfreq, currentParamSet.at(AnnotationBase::RESTFREQ).stringVal
        )
    ) {
        *_log << preamble << "restfreq value "
            << currentParamSet.at(AnnotationBase::RESTFREQ).stringVal << " is not "
            << "a valid quantity." << LogIO::EXCEPTION;
    }
    try {
    switch (annType) {
        case AnnotationBase::RECT_BOX:
            annotation = new AnnRectBox(
                qDirs[0], qDirs[1], qDirs[2], qDirs[3],
                dirRefFrame, _csys, _imShape, qFreqs.first, qFreqs.second,
                freqRefFrame, doppler, restfreq, stokes,
                annOnly, requireImageRegion
            );
            break;
        case AnnotationBase::CENTER_BOX:
            annotation = new AnnCenterBox(
                qDirs[0], qDirs[1], quantities[0], quantities[1],
                dirRefFrame, _csys, _imShape, qFreqs.first, qFreqs.second,
                freqRefFrame, doppler, restfreq, stokes,
                annOnly, requireImageRegion
            );
            break;
        case AnnotationBase::ROTATED_BOX:
            annotation = new AnnRotBox(
                qDirs[0], qDirs[1], quantities[0], quantities[1],
                quantities[2], dirRefFrame, _csys, _imShape, qFreqs.first, qFreqs.second,
                freqRefFrame, doppler, restfreq,  stokes, annOnly, requireImageRegion
            );
            break;
        case AnnotationBase::POLYGON:
            {
                Vector<Quantity> x(qDirs.size()/2);
                Vector<Quantity> y(qDirs.size()/2);
                for (uInt i=0; i<x.size(); ++i) {
                    x[i] = qDirs[2*i];
                    y[i] = qDirs[2*i + 1];
                }
                annotation = new AnnPolygon(
                    x, y, dirRefFrame,  _csys, _imShape, qFreqs.first, qFreqs.second,
                    freqRefFrame, doppler, restfreq,  stokes, annOnly, requireImageRegion
                );
            }
            break;
        case AnnotationBase::CIRCLE:
            if (
                quantities[0].getUnit() == "pix"
                && ! _csys.directionCoordinate().hasSquarePixels()
            ) {
                // radius specified in pixels and pixels are not square, use
                // an AnnEllipse
                annotation = new AnnEllipse(
                    qDirs[0], qDirs[1], quantities[0], quantities[0], Quantity(0, "deg"),
                    dirRefFrame,  _csys, _imShape, qFreqs.first, qFreqs.second,
                    freqRefFrame, doppler, restfreq,  stokes, annOnly, requireImageRegion
                );
            }
            else {
                annotation = new AnnCircle(
                    qDirs[0], qDirs[1], quantities[0],
                    dirRefFrame,  _csys, _imShape, qFreqs.first, qFreqs.second,
                    freqRefFrame, doppler, restfreq,  stokes, annOnly, requireImageRegion
                );
            }
            break;
        case AnnotationBase::ANNULUS:
            annotation = new AnnAnnulus(
                qDirs[0], qDirs[1], quantities[0], quantities[1],
                dirRefFrame,  _csys, _imShape, qFreqs.first, qFreqs.second,
                freqRefFrame, doppler, restfreq,  stokes, annOnly, requireImageRegion
            );
            break;
        case AnnotationBase::ELLIPSE:
            annotation = new AnnEllipse(
                qDirs[0], qDirs[1], quantities[0], quantities[1], quantities[2],
                dirRefFrame,  _csys, _imShape, qFreqs.first, qFreqs.second,
                freqRefFrame, doppler, restfreq,  stokes, annOnly, requireImageRegion
            );
            break;
        case AnnotationBase::LINE:
            annotation = new AnnLine(
                qDirs[0], qDirs[1], qDirs[2],
                qDirs[3], dirRefFrame,  _csys,
                qFreqs.first, qFreqs.second,
                freqRefFrame, doppler, restfreq,  stokes
            );
            break;
        case AnnotationBase::VECTOR:
            annotation = new AnnVector(
                qDirs[0], qDirs[1], qDirs[2],
                qDirs[3], dirRefFrame,  _csys,
                qFreqs.first, qFreqs.second,
                freqRefFrame, doppler, restfreq,  stokes
            );
            break;
        case AnnotationBase::TEXT:
            annotation = new AnnText(
                qDirs[0], qDirs[1], dirRefFrame,
                _csys, textString, qFreqs.first, qFreqs.second,
                freqRefFrame, doppler, restfreq,  stokes
            );
            break;
        case AnnotationBase::SYMBOL:
            annotation = new AnnSymbol(
                qDirs[0], qDirs[1], dirRefFrame,
                _csys, textString.firstchar(),
                qFreqs.first, qFreqs.second, freqRefFrame,
                doppler, restfreq,  stokes
            );
            break;
        default:
            ThrowCc(
                preamble + "Logic error. Unhandled type "
                    +  String::toString(annType) + " in switch statement"
            );
    }
    }
    catch (const WorldToPixelConversionError& x) {
        *_log << LogIO::WARN << preamble
            << "Error converting one or more world coordinates to pixel coordinates. "
            << "This could mean, among other things, that (part of) the region or "
            << "annotation lies far outside the image. This region/annotation will "
            << "be ignored. The related message is: " << x.getMesg() << LogIO::POST;
        return;
    }
    catch (const ToLCRegionConversionError& x) {
        *_log << LogIO::WARN << preamble
            << "Error converting world region to lattice region which probably indicates "
            << "the region lies outside of the image. This region will be ignored."
            << "The related message is: " << x.getMesg() << LogIO::POST;
        return;
    }
    catch (const AipsError& x) {
        ThrowCc(preamble + x.getMesg());
    }
    if (annotation->isRegion()) {
        dynamic_cast<AnnRegion *>(annotation.get())->setDifference(isDifference);
        if (! annOnly) {
            ++_regions;
        }
    }
    annotation->setLineWidth(currentParamSet.at(AnnotationBase::LINEWIDTH).intVal);
    annotation->setLineStyle(
        AnnotationBase::lineStyleFromString(
            currentParamSet.at(AnnotationBase::LINESTYLE).stringVal
        )
    );
    annotation->setSymbolSize(currentParamSet.at(AnnotationBase::SYMSIZE).intVal);
    annotation->setSymbolThickness(currentParamSet.at(AnnotationBase::SYMTHICK).intVal);
    annotation->setColor(currentParamSet.at(AnnotationBase::COLOR).stringVal);
    annotation->setFont(currentParamSet.at(AnnotationBase::FONT).stringVal);
    annotation->setFontSize(currentParamSet.at(AnnotationBase::FONTSIZE).intVal);
    annotation->setFontStyle(
        AnnotationBase::fontStyleFromString(
            currentParamSet.at(AnnotationBase::FONTSTYLE).stringVal
        )
    );
    annotation->setUseTex(currentParamSet.at(AnnotationBase::USETEX).boolVal);
    if (
        currentParamSet.find(AnnotationBase::LABEL) != currentParamSet.end()
        && ! currentParamSet.find(AnnotationBase::LABEL)->second.stringVal.empty()
    ) {
        annotation->setLabel(currentParamSet.at(AnnotationBase::LABEL).stringVal);
        annotation->setLabelColor(currentParamSet.at(AnnotationBase::LABELCOLOR).stringVal);
        annotation->setLabelColor(currentParamSet.at(AnnotationBase::LABELCOLOR).stringVal);
        annotation->setLabelPosition(currentParamSet.at(AnnotationBase::LABELPOS).stringVal);
        annotation->setLabelOffset(currentParamSet.at(AnnotationBase::LABELOFF).intVec);
    }
    annotation->setGlobals(_globalKeysToApply);
    AsciiAnnotationFileLine line(annotation);
    _addLine(line);
}

Array<String> RegionTextParser::_extractTwoPairs(uInt& end, const String& string) const {
    end = 0;
    Int firstBegin = string.find('[', 1);
    Int firstEnd = string.find(']', firstBegin);
    auto firstPair = string.substr(firstBegin, firstEnd - firstBegin + 1);
    Int secondBegin = string.find('[', firstEnd);
    Int secondEnd = string.find(']', secondBegin);
    auto secondPair = string.substr(secondBegin, secondEnd - secondBegin + 1);
    auto first = _extractSinglePair(firstPair);
    auto second = _extractSinglePair(secondPair);

    end = secondEnd;
    Array<String> ret(IPosition(2, 2, 2));
    ret(IPosition(2, 0, 0)) = first[0];
    ret(IPosition(2, 0, 1)) = first[1];
    ret(IPosition(2, 1, 0)) = second[0];
    ret(IPosition(2, 1, 1)) = second[1];
    return ret;
}

Vector<String> RegionTextParser::_extractSinglePair(const String& string) {
    Char quotes[2];
    quotes[0] = '\'';
    quotes[1] = '"';
    Int firstBegin = string.find('[', 0) + 1;
    Int firstEnd = string.find(',', firstBegin);
    auto first = string.substr(firstBegin, firstEnd - firstBegin);
    first.trim();
    first.trim(quotes, 2);
    Int secondBegin = firstEnd + 1;
    Int secondEnd = string.find(']', secondBegin);
    auto second = string.substr(secondBegin, secondEnd - secondBegin);
    second.trim();
    second.trim(quotes, 2);
    Vector<String> ret(2);
    ret[0] = first;
    ret[1] = second;
    return ret;
}

String RegionTextParser::_doLabel(
    String& consumeMe, const String& preamble
) {
    const auto firstChar = consumeMe.firstchar();
    if (firstChar != '\'' && firstChar != '"') {
        ostringstream oss;
        oss << preamble << "keyword 'label' found but first non-whitespace "
            << "character after the '=' is not a quote. It must be.";
        throw AipsError(oss.str());
    }
    const auto posCloseQuote = consumeMe.find(firstChar, 1);
    if (posCloseQuote == String::npos) {
        ostringstream err;
        err << preamble << "Could not find closing quote ("
            << String(firstChar) << ") for label";
        throw AipsError(err.str());
    }
    const auto label = consumeMe.substr(1, posCloseQuote - 1);
    consumeMe.del(0, (Int)posCloseQuote + 1);
    return label;
}

String RegionTextParser::_getKeyValue(
    String& consumeMe, const String& preamble
) {
    String value;
    if (consumeMe.startsWith("[")) {
        if (! consumeMe.contains("]")) {
            ostringstream err;
            err << preamble << "Unmatched open bracket: "
                << consumeMe;
            ThrowCc(err.str());
        }
        Int closeBracketPos = consumeMe.find("]");
        // don't save the open and close brackets
        value = consumeMe.substr(1, closeBracketPos - 1);
        consumeMe.del(0, closeBracketPos + 1);
    }
    if (consumeMe.contains(",")) {
        Int commaPos = consumeMe.find(",");
        if (value.empty()) {
            value = consumeMe.substr(0, commaPos);
        }
        consumeMe.del(0, commaPos);
    }
    else if (value.empty()) {
        // last key-value pair on the line
        value = consumeMe;
        consumeMe = "";
    }
    consumeMe.trim();
    Char quotes[2];
    quotes[0] = '\'';
    quotes[1] = '"';
    value.trim();
    value.trim(quotes, 2);
    value.trim();
    return value;
}

Vector<Quantity> RegionTextParser::_extractTwoQuantityPairsAndSingleQuantity(
    String& consumeMe, const String& preamble
) const {
    Vector<Quantity> quantities = _extractTwoQuantityPairs(
        consumeMe, preamble
    );
    consumeMe.trim();
    consumeMe.ltrim(',');
    consumeMe.trim();
    Char quotes[2];
    quotes[0] = '\'';
    quotes[1] = '"';

    Int end = consumeMe.find(']', 0);
    String qString = consumeMe.substr(0, end);
    qString.trim();

    qString.trim(quotes, 2);
    quantities.resize(5, true);
    if (! readQuantity(quantities[4], qString)) {
        *_log << preamble + "Could not convert "
            << qString << " to quantity." << LogIO::EXCEPTION;
    }
    consumeMe.del(0, end + 1);
    return quantities;
}

void RegionTextParser::_extractQuantityPairAndString(
    std::pair<Quantity, Quantity>& quantities, String& string,
    String& consumeMe, const String& preamble,
    const Bool requireQuotesAroundString
) const {
    // erase the left '['
    consumeMe.del(0, 1);
    SubString pairString = consumeMe.through(startOnePair);
    quantities = _extractSingleQuantityPair(pairString, preamble);
    consumeMe.del(0, (Int)pairString.length() + 1);
    consumeMe.trim();
    consumeMe.ltrim(',');
    consumeMe.trim();
    Int end = 0;
    String::size_type startSearchPos = 0;
    if (requireQuotesAroundString) {
        Char quoteChar = consumeMe.firstchar();
        if (quoteChar != '\'' && quoteChar != '"') {
            *_log << preamble
                << "Quotes around string required but no quotes were found";
        }
        startSearchPos = consumeMe.find(quoteChar, 1);
        if (startSearchPos == String::npos) {
            *_log << preamble
                << "Quotes required around string but no matching close quote found"
                << LogIO::EXCEPTION;
        }
    }
    end = consumeMe.find(']', startSearchPos);
    string = consumeMe.substr(0, end);
    consumeMe.del(0, end + 1);
    string.trim();
    Char quotes[2];
    quotes[0] = '\'';
    quotes[1] = '"';
    string.trim(quotes, 2);
    string.trim();
}

Vector<Quantity> RegionTextParser::_extractQuantityPairAndSingleQuantity(
    String& consumeMe, const String& preamble
) const {
    String qString;

    std::pair<Quantity, Quantity> myPair;
    _extractQuantityPairAndString(
        myPair, qString, consumeMe, preamble, false
    );
    Vector<Quantity> quantities(3);
    quantities[0] = myPair.first;
    quantities[1] = myPair.second;
    ThrowIf(
        ! readQuantity(quantities[2], qString),
        preamble + "Could not convert "
        + qString + " to quantity"
    );
    return quantities;
}

Vector<Quantity> RegionTextParser::_extractTwoQuantityPairs(
    String& consumeMe, const String& preamble
) const {
    const Regex startbTwoPair("^" + bTwoPair);
    String mySubstring = String(consumeMe).through(startbTwoPair);
    uInt end = 0;
    Array<String> pairs = _extractTwoPairs(end, mySubstring);
    Vector<Quantity> quantities(4);

    for (uInt i=0; i<4; ++i) {
        String desc("string " + String::toString(i));
        String value = pairs(IPosition(2, i/2, i%2));
        if (! readQuantity(quantities[i], value)) {
            *_log << preamble << "Could not convert " << desc
                << " (" << value << ") to quantity." << LogIO::EXCEPTION;
        }
    }
    consumeMe.del(0, (Int)end + 1);
    return quantities;
}

Vector<Quantity> RegionTextParser::_extractNQuantityPairs (
        String& consumeMe, const String& preamble
) const {
    String pairs = consumeMe.through(Regex("\\] *\\]"));
    String nPairs(pairs);
    consumeMe.del(0, (Int)pairs.length() + 1);
    pairs.trim();
    // remove the left most [
    pairs.del(0, 1);
    pairs.trim();
    Vector<Quantity> qs(0);

    try {
        while (pairs.length() > 1) {
            std::pair<Quantity, Quantity> myqs = _extractSingleQuantityPair(pairs, preamble);
            qs.resize(qs.size() + 2, true);
            qs[qs.size() - 2] = myqs.first;
            qs[qs.size() - 1] = myqs.second;
            pairs.del(0, (Int)pairs.find(']', 0) + 1);
            pairs.trim();
            pairs.ltrim(',');
            pairs.trim();
        }
    }
    catch (const AipsError&) {
        ThrowCc(
            preamble + "Illegal polygon specification " + nPairs
        );
    }
    return qs;
}

std::pair<Quantity, Quantity> RegionTextParser::_extractSingleQuantityPair(
    const String& pairString, const String& preamble
) const {
    String mySubstring = String(pairString).through(sOnePair, 0);
    Vector<String> pair = _extractSinglePair(mySubstring);
    std::pair<Quantity, Quantity> quantities;
    for (uInt i=0; i<2; ++i) {
        String value = pair[i];
        ThrowIf(
            ! readQuantity(
                i == 0
                    ? quantities.first
                    : quantities.second,
                value
            ),
            preamble + "Could not convert ("
            + value + ") to quantity."
        );
    }
    return quantities;
}

void RegionTextParser::_setOverridingCorrelations(const String& globalOverrideStokes) {
    if (globalOverrideStokes.empty() || ! _csys.hasPolarizationAxis()) {
        // no global override specified
        return;
    }
    String mycopy = globalOverrideStokes;
    vector<String> myStokes = ParameterParser::stokesFromString(mycopy);
    String myCommaSeperatedString;
    vector<String>::const_iterator iter = myStokes.begin();
    vector<String>::const_iterator end = myStokes.end();
    uInt count = 0;
    while(iter != end) {
        myCommaSeperatedString += *iter;
        if (count < myStokes.size() - 1) {
            myCommaSeperatedString += ",";
        }
        ++count;
        ++iter;
    }
    ParamValue corr;
    corr.stokes = _stokesFromString(myCommaSeperatedString, String(__func__));
    _currentGlobals[AnnotationBase::CORR] = corr;
    _overridingCorrRange.reset((new Vector<Stokes::StokesTypes>(corr.stokes)));
}

void RegionTextParser::_setOverridingChannelRange(
    const String& globalOverrideChans
) {
    if (globalOverrideChans.empty() || ! _csys.hasSpectralAxis()) {
        // no global override specified
        return;
    }
    uInt nSelectedChannels = 0;
    uInt nChannels = _imShape[_csys.spectralAxisNumber(false)];
    std::vector<uInt> myChanRange =  ParameterParser::spectralRangesFromChans(
        nSelectedChannels, globalOverrideChans,
        nChannels
    );
    uInt nRanges = myChanRange.size();
    if (nRanges == 0) {
        // no channel range specified
        return;
    }
    ThrowIf(
        nRanges > 2,
        "Overriding spectral specification must be "
        "limited to a sngle channel range"
    );
    MFrequency first, second;
    const SpectralCoordinate specCoord = _csys.spectralCoordinate();
    specCoord.toWorld(first, myChanRange[0]);
    specCoord.toWorld(second, myChanRange[1]);
    _overridingFreqRange.reset(new std::pair<MFrequency, MFrequency>(first, second));
    ParamValue range;
    range.freqRange = _overridingFreqRange;
    _currentGlobals[AnnotationBase::RANGE] = range;

    ParamValue frame;
    frame.intVal = specCoord.frequencySystem(false);
    _currentGlobals[AnnotationBase::FRAME] = frame;

    ParamValue veltype;
    veltype.intVal = specCoord.velocityDoppler();
    _currentGlobals[AnnotationBase::VELTYPE] = veltype;

    ParamValue restfreq;
    restfreq.stringVal = String::toString(
        specCoord.restFrequency()
    ) + "Hz";
    _currentGlobals[AnnotationBase::RESTFREQ] = restfreq;
}

Vector<Stokes::StokesTypes>
RegionTextParser::_stokesFromString(
    const String& stokes, const String& preamble
) {
    const auto maxn = Stokes::NumberOfTypes;
    PtrHolder<string> res(new string[maxn], true);
    Int nStokes = split(stokes, res, maxn, ",");
    Vector<Stokes::StokesTypes> myTypes(nStokes);
    for (Int i=0; i<nStokes; ++i) {
        String x(res[i]);
        x.trim();
        myTypes[i] = Stokes::type(x);
        if (myTypes[i] == Stokes::Undefined) {
            throw AipsError(preamble + "Unknown correlation type " + x);
        }
    }
    return myTypes;
}

void RegionTextParser::_setInitialGlobals() {
    ParamValue coord;
    coord.intVal = _csys.directionCoordinate(
        _csys.findCoordinate(Coordinate::DIRECTION)
    ).directionType(false);
    coord.stringVal = MDirection::showType(coord.intVal);
    _currentGlobals[AnnotationBase::COORD] = coord;

    ParamValue range;
    range.freqRange.reset();
    _currentGlobals[AnnotationBase::RANGE] = range;

    ParamValue corr;
    corr.stokes = Vector<Stokes::StokesTypes>(0);
    _currentGlobals[AnnotationBase::CORR] = corr;

    if (_csys.hasSpectralAxis()) {
        SpectralCoordinate spectral = _csys.spectralCoordinate(
            _csys.findCoordinate(Coordinate::SPECTRAL)
        );

        ParamValue frame;
        frame.intVal = spectral.frequencySystem(false);
        _currentGlobals[AnnotationBase::FRAME] = frame;

        ParamValue veltype;
        veltype.intVal = spectral.velocityDoppler();
        _currentGlobals[AnnotationBase::VELTYPE] = veltype;

        ParamValue restfreq;
        // truncates value, not enough precision
        // restfreq.stringVal = String::toString(spectral.restFrequency()) + "Hz";
        ostringstream oss;
        oss << std::setprecision(20) << spectral.restFrequency() << "Hz";
        restfreq.stringVal = oss.str();
        _currentGlobals[AnnotationBase::RESTFREQ] = restfreq;
    }
    ParamValue linewidth;
    linewidth.intVal = AnnotationBase::DEFAULT_LINEWIDTH;
    _currentGlobals[AnnotationBase::LINEWIDTH] = linewidth;

    ParamValue linestyle;
    linestyle.lineStyleVal = AnnotationBase::DEFAULT_LINESTYLE;
    _currentGlobals[AnnotationBase::LINESTYLE] = linestyle;

    ParamValue symsize;
    symsize.intVal = AnnotationBase::DEFAULT_SYMBOLSIZE;
    _currentGlobals[AnnotationBase::SYMSIZE] = symsize;

    ParamValue symthick;
    symthick.intVal = AnnotationBase::DEFAULT_SYMBOLTHICKNESS;
    _currentGlobals[AnnotationBase::SYMTHICK] = symthick;

    ParamValue color;
    color.color = AnnotationBase::DEFAULT_COLOR;
    color.stringVal = AnnotationBase::colorToString(color.color);
    _currentGlobals[AnnotationBase::COLOR] = color;

    ParamValue font;
    font.stringVal = AnnotationBase::DEFAULT_FONT;
    _currentGlobals[AnnotationBase::FONT] = font;

    ParamValue fontsize;
    fontsize.intVal = AnnotationBase::DEFAULT_FONTSIZE;
    _currentGlobals[AnnotationBase::FONTSIZE] = fontsize;

    ParamValue fontstyle;
    fontstyle.fontStyleVal = AnnotationBase::DEFAULT_FONTSTYLE;
    _currentGlobals[AnnotationBase::FONTSTYLE] = fontstyle;

    ParamValue usetex;
    usetex.boolVal = AnnotationBase::DEFAULT_USETEX;
    _currentGlobals[AnnotationBase::USETEX] = usetex;

    ParamValue labelcolor;
    labelcolor.color = AnnotationBase::DEFAULT_LABELCOLOR;
    labelcolor.stringVal = AnnotationBase::colorToString(labelcolor.color);
    _currentGlobals[AnnotationBase::LABELCOLOR] = labelcolor;

    ParamValue labelpos;
    labelpos.stringVal = AnnotationBase::DEFAULT_LABELPOS;
    _currentGlobals[AnnotationBase::LABELPOS] = labelpos;

    ParamValue labeloff;
    labeloff.intVec = AnnotationBase::DEFAULT_LABELOFF;
    _currentGlobals[AnnotationBase::LABELOFF] = labeloff;
}

}

